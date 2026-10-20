#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   設定
   ============================================================ */

#define MEMORY_SIZE       (16UL * 1024UL * 1024UL)  /* 16MB */
#define BLOCK_SIZE        256UL
#define ADDRESS_SIZE      2UL
#define CHECKSUM_SIZE     1UL

/* 2バイトアドレス + 256バイトデータ + 1バイトチェックサム */
#define TRANSFER_SIZE     (ADDRESS_SIZE + BLOCK_SIZE + CHECKSUM_SIZE)

#define PROGRESS_SIZE     (128UL * 1024UL)   /* 128KB */

#define ACK_BYTE          0x00

/* ACK待ちタイムアウト */
#define ACK_TIMEOUT_MS    10000UL

#define MAX_LINE_SIZE     1024


/* ============================================================
   プロトタイプ宣言
   ============================================================ */

static int hex_to_byte(const char *p, unsigned char *value);

static int load_hex_file(
    const char *filename,
    unsigned char *memory,
    unsigned long *used_size);

static HANDLE open_com_port(
    const char *port_name,
    DWORD baud_rate);

static int wait_ack(HANDLE hCom);

static int send_block(
    HANDLE hCom,
    unsigned long address,
    const unsigned char *data);

static int block_is_all_ff(
    const unsigned char *data);

static int parse_baud_rate(
    const char *text,
    DWORD *baud_rate);


/* ============================================================
   16進文字1文字を数値へ変換
   ============================================================ */

static int hex_char_to_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    return -1;
}


/* ============================================================
   16進2文字を1バイトへ変換
   ============================================================ */

static int hex_to_byte(const char *p, unsigned char *value)
{
    int h;
    int l;

    h = hex_char_to_value(p[0]);
    l = hex_char_to_value(p[1]);

    if (h < 0 || l < 0)
        return 0;

    *value = (unsigned char)((h << 4) | l);

    return 1;
}


/* ============================================================
   Intel HEXファイル読み込み
   ============================================================ */

static int load_hex_file(
    const char *filename,
    unsigned char *memory,
    unsigned long *used_size)
{
    FILE *fp;
    char line[MAX_LINE_SIZE];

    unsigned long base_address;
    unsigned long max_address;

    int eof_found;

    fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("ERROR: Cannot open HEX file.\n");
        printf("       File = %s\n", filename);
        return 0;
    }

    base_address = 0;
    max_address = 0;
    eof_found = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t line_length;

        unsigned char record_length;
        unsigned char address_high;
        unsigned char address_low;
        unsigned char record_type;

        unsigned int record_address;

        unsigned char data[256];

        unsigned char checksum;
        unsigned char calculated_checksum;

        unsigned int i;

        unsigned long absolute_address;

        line_length = strlen(line);

        /* CR/LF除去 */
        while (line_length > 0 &&
               (line[line_length - 1] == '\r' ||
                line[line_length - 1] == '\n')) {
            line[line_length - 1] = '\0';
            line_length--;
        }

        /* 空行は無視 */
        if (line_length == 0)
            continue;

        /* Intel HEXは ':' から始まる */
        if (line[0] != ':') {
            printf("ERROR: Invalid HEX record.\n");
            fclose(fp);
            return 0;
        }

        /*
         * 最低でも
         *
         * : LL AAAA TT CC
         *
         * の10文字が必要
         */
        if (line_length < 11) {
            printf("ERROR: HEX record is too short.\n");
            fclose(fp);
            return 0;
        }

        /* データ長 */
        if (!hex_to_byte(&line[1], &record_length)) {
            printf("ERROR: Invalid record length.\n");
            fclose(fp);
            return 0;
        }

        /* アドレス */
        if (!hex_to_byte(&line[3], &address_high) ||
            !hex_to_byte(&line[5], &address_low)) {
            printf("ERROR: Invalid record address.\n");
            fclose(fp);
            return 0;
        }

        record_address =
            ((unsigned int)address_high << 8) |
            (unsigned int)address_low;

        /* レコードタイプ */
        if (!hex_to_byte(&line[7], &record_type)) {
            printf("ERROR: Invalid record type.\n");
            fclose(fp);
            return 0;
        }

        /*
         * 1レコードの必要文字数
         *
         * ':' 1
         * length 2
         * address 4
         * type 2
         * data 2 * record_length
         * checksum 2
         */
        if (line_length <
            (size_t)(11 + ((unsigned int)record_length * 2))) {

            printf("ERROR: HEX record length mismatch.\n");
            fclose(fp);
            return 0;
        }

        /* データ読み込み */
        for (i = 0; i < record_length; i++) {
            if (!hex_to_byte(
                    &line[9 + (i * 2)],
                    &data[i])) {

                printf("ERROR: Invalid HEX data.\n");
                fclose(fp);
                return 0;
            }
        }

        /* チェックサム */
        if (!hex_to_byte(
                &line[9 + ((unsigned int)record_length * 2)],
                &checksum)) {

            printf("ERROR: Invalid HEX checksum field.\n");
            fclose(fp);
            return 0;
        }

        /*
         * Intel HEXチェックサム
         *
         * length
         * address high
         * address low
         * record type
         * data
         * checksum
         *
         * の合計の下位8bitが0
         */
        calculated_checksum = 0;

        calculated_checksum += record_length;
        calculated_checksum += address_high;
        calculated_checksum += address_low;
        calculated_checksum += record_type;

        for (i = 0; i < record_length; i++)
            calculated_checksum += data[i];

        calculated_checksum += checksum;

        if (calculated_checksum != 0) {
            printf("ERROR: HEX checksum error.\n");
            printf("       Address = %04X\n", record_address);
            printf("       Type    = %02X\n", record_type);
            fclose(fp);
            return 0;
        }

        /* ====================================================
           レコード処理
           ==================================================== */

        switch (record_type) {

        case 0x00:
            /*
             * Data Record
             */

            absolute_address =
                base_address + (unsigned long)record_address;

            if (absolute_address >= MEMORY_SIZE ||
                record_length >
                MEMORY_SIZE - absolute_address) {

                printf("ERROR: HEX address exceeds 16MB.\n");
                printf("       Address = 0x%08lX\n",
                       absolute_address);

                fclose(fp);
                return 0;
            }

            if (record_length > 0) {
                memcpy(
                    &memory[absolute_address],
                    data,
                    record_length);

                if (absolute_address +
                    (unsigned long)record_length >
                    max_address) {

                    max_address =
                        absolute_address +
                        (unsigned long)record_length;
                }
            }

            break;


        case 0x01:
            /*
             * End Of File Record
             */

            if (record_length != 0) {
                printf("ERROR: Invalid EOF record.\n");
                fclose(fp);
                return 0;
            }

            eof_found = 1;

            break;


        case 0x02:
            /*
             * Extended Segment Address Record
             *
             * 形式:
             *
             * :02000002SSSSCC
             *
             * ベースアドレス = SSSS << 4
             */

            if (record_length != 2 ||
                record_address != 0) {

                printf("ERROR: Invalid Extended Segment Address record.\n");
                fclose(fp);
                return 0;
            }

            base_address =
                ((unsigned long)data[0] << 8 |
                 (unsigned long)data[1]) << 4;

            break;


        case 0x04:
            /*
             * Extended Linear Address Record
             *
             * 形式:
             *
             * :02000004HHHHCC
             *
             * ベースアドレス = HHHH << 16
             */

            if (record_length != 2 ||
                record_address != 0) {

                printf("ERROR: Invalid Extended Linear Address record.\n");
                fclose(fp);
                return 0;
            }

            base_address =
                ((unsigned long)data[0] << 8 |
                 (unsigned long)data[1]) << 16;

            break;


        default:
            /*
             * その他のレコードタイプは、
             * メモリデータには使用しない。
             *
             * チェックサム検証は既に済んでいる。
             */
            break;
        }

        if (eof_found)
            break;
    }

    fclose(fp);

    if (!eof_found) {
        printf("ERROR: EOF record was not found.\n");
        return 0;
    }

    *used_size = max_address;

    return 1;
}


/* ============================================================
   COMポートを開く
   ============================================================ */

static HANDLE open_com_port(
    const char *port_name,
    DWORD baud_rate)
{
    HANDLE hCom;
    DCB dcb;
    COMMTIMEOUTS timeouts;
    char device_name[64];

    sprintf(
        device_name,
        "\\\\.\\%s",
        port_name);

    hCom = CreateFileA(
        device_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hCom == INVALID_HANDLE_VALUE) {

        printf("ERROR: Cannot open COM port.\n");
        printf("       Port = %s\n", port_name);
        printf("       Windows error = %lu\n",
               GetLastError());

        return INVALID_HANDLE_VALUE;
    }

    /*
     * DCB取得
     */
    memset(&dcb, 0, sizeof(dcb));

    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(hCom, &dcb)) {

        printf("ERROR: GetCommState failed.\n");
        printf("       Windows error = %lu\n",
               GetLastError());

        CloseHandle(hCom);

        return INVALID_HANDLE_VALUE;
    }

    /*
     * UART設定
     *
     * 8bit
     * No parity
     * 1 stop bit
     * No flow control
     */
    dcb.BaudRate = baud_rate;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    dcb.fBinary = TRUE;

    dcb.fParity = FALSE;

    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;

    dcb.fDsrSensitivity = FALSE;

    dcb.fTXContinueOnXoff = TRUE;

    dcb.fOutX = FALSE;
    dcb.fInX  = FALSE;

    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;

    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(hCom, &dcb)) {

        printf("ERROR: SetCommState failed.\n");
        printf("       Windows error = %lu\n",
               GetLastError());

        CloseHandle(hCom);

        return INVALID_HANDLE_VALUE;
    }

    /*
     * 通信タイムアウト設定
     */
    memset(&timeouts, 0, sizeof(timeouts));

    /*
     * ReadFileで1バイト受信する際、
     * ここでは短い待ち時間にして、
     * wait_ack()側で全体のタイムアウトを管理する。
     */
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 50;

    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 10000;

    if (!SetCommTimeouts(hCom, &timeouts)) {

        printf("ERROR: SetCommTimeouts failed.\n");
        printf("       Windows error = %lu\n",
               GetLastError());

        CloseHandle(hCom);

        return INVALID_HANDLE_VALUE;
    }

    /*
     * 送受信バッファをクリア
     */
    if (!PurgeComm(
            hCom,
            PURGE_RXCLEAR |
            PURGE_TXCLEAR)) {

        printf("ERROR: PurgeComm failed.\n");
        printf("       Windows error = %lu\n",
               GetLastError());

        CloseHandle(hCom);

        return INVALID_HANDLE_VALUE;
    }

    return hCom;
}


/* ============================================================
   ACK待ち
   ============================================================ */

static int wait_ack(HANDLE hCom)
{
    DWORD start_time;
    unsigned char response;
    DWORD received;

    start_time = GetTickCount();

    while (1) {

        received = 0;

        if (!ReadFile(
                hCom,
                &response,
                1,
                &received,
                NULL)) {

            printf("\nERROR: ReadFile failed.\n");
            printf("       Windows error = %lu\n",
                   GetLastError());

            return 0;
        }

        if (received == 1) {

            /*
             * 0x00 = 正常
             */
            if (response == ACK_BYTE)
                return 1;

            /*
             * 0x00以外はエラー
             */
            printf("\nERROR: Target returned 0x%02X\n",
                   response);

            return 0;
        }

        /*
         * タイムアウト確認
         */
        if ((GetTickCount() - start_time) >=
            ACK_TIMEOUT_MS) {

            printf("\nERROR: ACK timeout.\n");
            printf("       Timeout = %lu ms\n",
                   (unsigned long)ACK_TIMEOUT_MS);

            return 0;
        }
    }
}


/* ============================================================
   256バイトブロック送信
   ============================================================ */

static int send_block(
    HANDLE hCom,
    unsigned long address,
    const unsigned char *data)
{
    unsigned char packet[TRANSFER_SIZE];

    unsigned char xor_value;

    unsigned long i;

    DWORD written;

    /*
     * 24bitアドレスの上位16bitだけを送信する。
     *
     * 例:
     *
     * address = 0x001200
     *
     * 上位16bit = 0x0012
     *
     * 送信:
     *   00 12
     *
     * アドレスはビッグエンディアン。
     */

    packet[0] =
        (unsigned char)((address >> 16) & 0xFF);

    packet[1] =
        (unsigned char)((address >> 8) & 0xFF);

    /*
     * 256バイトデータ
     */
    memcpy(
        &packet[ADDRESS_SIZE],
        data,
        BLOCK_SIZE);

    /*
     * XORチェックサム
     *
     * 対象:
     *
     *   address byte 0
     *   address byte 1
     *   data[0] ～ data[255]
     *
     * 合計258バイト
     */

    xor_value = 0;

    xor_value ^= packet[0];
    xor_value ^= packet[1];

    for (i = 0; i < BLOCK_SIZE; i++)
        xor_value ^= data[i];

    /*
     * 最後の1バイトがチェックサム
     */
    packet[
        ADDRESS_SIZE + BLOCK_SIZE
    ] = xor_value;

    /*
     * 259バイト送信
     */
    written = 0;

    if (!WriteFile(
            hCom,
            packet,
            TRANSFER_SIZE,
            &written,
            NULL)) {

        printf("\nERROR: WriteFile failed.\n");
        printf("       Windows error = %lu\n",
               GetLastError());

        return 0;
    }

    if (written != TRANSFER_SIZE) {

        printf("\nERROR: Incomplete UART transmission.\n");
        printf("       Expected = %lu bytes\n",
               (unsigned long)TRANSFER_SIZE);
        printf("       Written  = %lu bytes\n",
               (unsigned long)written);

        return 0;
    }

    return 1;
}


/* ============================================================
   256バイトすべてが0xFFか確認
   ============================================================ */

static int block_is_all_ff(
    const unsigned char *data)
{
    unsigned long i;

    for (i = 0; i < BLOCK_SIZE; i++) {

        if (data[i] != 0xFF)
            return 0;
    }

    return 1;
}


/* ============================================================
   ボーレート解析
   ============================================================ */

static int parse_baud_rate(
    const char *text,
    DWORD *baud_rate)
{
    char *endptr;
    unsigned long value;

    if (text == NULL || text[0] == '\0')
        return 0;

    endptr = NULL;

    value = strtoul(
        text,
        &endptr,
        10);

    if (endptr == text ||
        *endptr != '\0' ||
        value == 0 ||
        value > 0xFFFFFFFFUL) {

        return 0;
    }

    *baud_rate = (DWORD)value;

    return 1;
}


/* ============================================================
   main
   ============================================================ */

int main(int argc, char *argv[])
{
    const char *port_name;
    const char *hex_filename;

    DWORD baud_rate;

    unsigned char *memory;

    unsigned long send_size;

    unsigned long address;

    unsigned long block_count;
    unsigned long skipped_block_count;

    unsigned long transmitted_data_bytes;
    unsigned long transmitted_uart_bytes;

    unsigned long next_progress;

    HANDLE hCom;

    /*
     * 引数確認
     */
    if (argc != 4) {

        printf("\n");
        printf("Usage:\n");
        printf("  hexsend.exe COM3 115200 firmware.hex\n");
        printf("\n");

        return 1;
    }

    port_name = argv[1];

    if (!parse_baud_rate(
            argv[2],
            &baud_rate)) {

        printf("ERROR: Invalid baud rate.\n");
        printf("       Baud rate = %s\n",
               argv[2]);

        return 1;
    }

    hex_filename = argv[3];

    printf("\n");
    printf("========================================\n");
    printf(" Intel HEX UART Sender\n");
    printf("========================================\n");
    printf("\n");

    printf("COM Port : %s\n", port_name);
    printf("Baud Rate: %lu\n",
           (unsigned long)baud_rate);
    printf("HEX File : %s\n", hex_filename);

    printf("\n");

    /*
     * 最大16MBメモリ確保
     */
    printf("Allocate memory (%lu bytes)...\n",
           (unsigned long)MEMORY_SIZE);

    memory =
        (unsigned char *)malloc(MEMORY_SIZE);

    if (memory == NULL) {

        printf("ERROR: Cannot allocate memory.\n");
        printf("       Size = %lu bytes\n",
               (unsigned long)MEMORY_SIZE);

        return 1;
    }

    /*
     * 全領域を0xFFで初期化
     */
    printf("Initialize memory with 0xFF...\n");

    memset(
        memory,
        0xFF,
        MEMORY_SIZE);

    /*
     * Intel HEX読み込み
     */
    printf("Load HEX file...\n");

    send_size = 0;

    if (!load_hex_file(
            hex_filename,
            memory,
            &send_size)) {

        free(memory);

        return 1;
    }

    printf("HEX file loaded successfully.\n");
    printf("Used address range: 0x000000 - 0x%08lX\n",
           send_size);

    /*
     * 送信サイズを256バイト境界まで拡張
     *
     * メモリ自体は既に0xFFなので、
     * 最終ブロックも自動的に0xFFでパディングされる。
     */
    if (send_size != 0) {

        send_size =
            (send_size + BLOCK_SIZE - 1) &
            ~(BLOCK_SIZE - 1);
    }

    printf("Transfer size: %lu bytes\n",
           send_size);

    printf("UART packet size: %lu bytes/block\n",
           (unsigned long)TRANSFER_SIZE);

    printf("\n");

    /*
     * COMポートを開く
     */
    printf("Open COM port.\n");

    hCom =
        open_com_port(
            port_name,
            baud_rate);

    if (hCom == INVALID_HANDLE_VALUE) {

        free(memory);

        return 1;
    }

    printf("COM port opened successfully.\n");

    printf("\n");
    printf("Start transfer.\n");
    printf("\n");

    /*
     * カウンタ初期化
     */
    block_count = 0;
    skipped_block_count = 0;

    transmitted_data_bytes = 0;
    transmitted_uart_bytes = 0;

    next_progress = PROGRESS_SIZE;

    /*
     * 256バイト単位で送信
     */
    for (
        address = 0;
        address < send_size;
        address += BLOCK_SIZE) {

        /*
         * 全部0xFFのブロックは送信しない
         */
        if (block_is_all_ff(
                &memory[address])) {

            skipped_block_count++;

            continue;
        }

        /*
         * 259バイト送信
         *
         * 2バイトアドレス
         * +256バイトデータ
         * +1バイトXOR
         */
        if (!send_block(
                hCom,
                address,
                &memory[address])) {

            printf("\n");
            printf("Transfer aborted.\n");

            CloseHandle(hCom);
            free(memory);

            return 1;
        }

        block_count++;

        transmitted_data_bytes +=
            BLOCK_SIZE;

        transmitted_uart_bytes +=
            TRANSFER_SIZE;

        /*
         * 受信側からACKを待つ
         */
        if (!wait_ack(hCom)) {

            printf("\n");
            printf("Transfer aborted.\n");

            CloseHandle(hCom);
            free(memory);

            return 1;
        }

        /*
         * 128KBごとに'*'を表示
         *
         * 実際に送信したデータ量を基準にする。
         */
        while (
            transmitted_data_bytes >=
            next_progress) {

            printf("*");
            fflush(stdout);

            next_progress +=
                PROGRESS_SIZE;
        }
    }

    printf("\n\n");

    /*
     * 終了処理
     */
    CloseHandle(hCom);

    free(memory);

    /*
     * 結果表示
     */
    printf("Transfer completed successfully.\n");
    printf("\n");

    printf("Transferred blocks : %lu\n",
           block_count);

    printf("Skipped FF blocks  : %lu\n",
           skipped_block_count);

    printf("Data bytes         : %lu\n",
           transmitted_data_bytes);

    printf("UART bytes         : %lu\n",
           transmitted_uart_bytes);

    printf("\n");

    return 0;
}
