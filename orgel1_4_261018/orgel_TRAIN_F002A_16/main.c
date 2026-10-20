//#define	SHDBS		//ﾃﾞﾊﾞｸﾞ信号を出力するときに宣言する
//#define	SHDBG		//ﾃﾞﾊﾞｸﾞ情報を出力するときに宣言する
//#define	SHBPS	115200	//ﾃﾞﾊﾞｸﾞ情報のﾎﾞｰﾚｰﾄを宣言する(ﾃﾞﾌｫﾙﾄ値は38400bps)
//#define	SHDBG_SOFT	//ﾃﾞﾊﾞｸﾞ情報のUARTをｿﾌﾄ実装するときに宣言する		
//#define	SHDBG_IN	//ﾃﾞﾊﾞｸﾞ情報を入力するときに宣言する(UARTのｿﾌﾄ実装は不可)	

//#define	I2C_SOFT	//I2Cをｿﾌﾄ実装するときに宣言する
#define	SPI_SOFT	//SPIをｿﾌﾄ実装するときに宣言する
//#define	SHSPI2		//2線SPIのときに宣言する
#define	SHSPI3		//3線SPIのときに宣言する
#define		SLEEP_EN	//Sleep機能を実装するときに宣言する
//#define		KYOKU_AUTO	//ｵﾙｺﾞｰﾙ演奏を自動開始するときに宣言する
//#define		ONSEIN_AUTO	//音声N(ch0)再生を自動開始するときに宣言する
//#define		ONSEII_AUTO	//音声I再生を自動開始するときに宣言する
//#define		ONSEIC_AUTO	//音声C再生を自動開始するときに宣言する
//#define		ONSEIS_AUTO	//音声S(ch0)再生を自動開始するときに宣言する
//#define	RAND_CHSEL	0x00000080	//ﾗﾝﾀﾞﾏｲｽﾞの種にするCHSEL(PA7:ADC_IN7)
//#define	KYOKU_RAND	//曲番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEIN_RAND	//音声N番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEII_RAND	//音声I番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEIC_RAND	//音声C番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEIS_RAND	//音声S番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
#define		SW_EN		//SWを実装するときに宣言する
//#define	SW_PLNG		//SWをｲﾍﾞﾝﾄではなく､ﾎﾟｰﾘﾝｸﾞするときに宣言する
//#define	CdS_EN		//CdSを実装するときに宣言する
//#define	CVD_EN		//CVDを実装するときに宣言する

//ｵﾙｺﾞｰﾙ箱への応用では以下を宣言しておくこと
//#define	CdS_FUTA	//蓋を開ける(明)とｵﾝ判定する
//#define	CdS_THR	100	//判定閾値を固定の宣言値にする

#define		VOICEN_IMA	//音声NがIMA_ADPCMｴﾝｺｰﾄﾞのときに宣言する
#define		VOICEI_IMA	//音声IがIMA_ADPCMｴﾝｺｰﾄﾞのときに宣言する
#define		VOICES_IMA	//音声SがIMA_ADPCMｴﾝｺｰﾄﾞのときに宣言する

//#define	ISP_SPIFLASH_EN	//ISP環境でSPIﾌﾗｯｼｭを書替えるときに宣言する
				//(自動的に､ﾃﾞﾊﾞｸﾞ情報のUART内蔵ﾓｼﾞｭｰﾙを230400bpsで使用する)
				//(通常のｱﾌﾟﾘは実行されない)

//2個のLEDの点滅ﾊﾟﾀｰﾝを､ｶｽﾀﾏｲｽﾞできる	(Dr.W)
#define	LED_ENT	//音声番号に応じたﾊﾟﾀｰﾝで､LED点滅するときに宣言する

/*
このｻﾝﾌﾟﾙﾌﾟﾛｼﾞｪｸﾄは､ﾄｲｺｰの｢ｻｳﾝﾄﾞ･ﾄﾚｲﾝ｣(旧ﾓﾃﾞﾙ)を対象にしている｡

2025/9/1 おもちゃ修理「電子ｶﾙﾃ」ﾌﾞﾛｸﾞ（治療後記）を､ご参照ください｡
→ https://blog.canpan.info/charts/archive/487
最近治療したﾄｲｺｰのｻｳﾝﾄﾞ･ﾄﾚｲﾝの高音質に驚き､再入院に備えて､
治療済みの実機から元音源をﾊﾞｯｸｱｯﾌﾟしておく｡
2026/10/21 ADPCM､SPIﾌﾗｯｼｭへのISP書き込みに対応。(ToyDr.わたなべ)

以下､ｶｽﾀﾏｲｽﾞ要件を箇条書き
･音声ﾃﾞｰﾀはSPIﾌﾗｯｼｭに置き､音声出力はﾌﾞﾚｰｷ出力でｽﾋﾟｰｶを駆動する｡
･4個のSWと2個のLEDを実装する｡CdSとCVDは実装しない｡
･4個のSWで､対応する音声S再生する｡
･　各SWは負論理入力､ﾘｱﾙﾓｰﾄﾞを前提に､ｵﾌからｵﾝへの変化でｵﾝ判定､
･　再生中のSWｵﾝでは再生中断､そのSWに対応する音声を頭から再生する｡
･2個のLED点滅をｼｰｹﾝｽ制御できる｡LEDは2系統､それぞれ正論理出力する｡
･　点滅ﾊﾟﾀｰﾝは4種類､交互点滅/同時に速点滅/全点灯/同時に遅点滅｡

根のﾒｲﾝｿｰｽで､ﾎﾟｰﾄ割当と音声ﾃﾞｰﾀ(SPIﾌﾗｯｼｭ)ｲﾝﾃﾞｯｸｽを設定、
LED点滅や駅名･発着時ｱﾅｳﾝｽは､ｵﾌﾟｼｮﾝ選択で実装可能とする｡
(最後尾SW3では､複数音声をﾗｳﾝﾄﾞﾛﾋﾞﾝに再生できる)
*/

/*
PUYA32電子ｵﾙｺﾞｰﾙ演奏+音声再生ver1_4
これは根のｿｰｽｺｰﾄﾞ

・ISP形態でSPIﾌﾗｯｼｭを書替える機能(ISP_SPIFLASH)に対応している｡

・ﾀｰｹﾞｯﾄはPY32F002A_16ﾋﾟﾝ(内部48MHzｸﾛｯｸ)

以下は電子ｵﾙｺﾞｰﾙとして動作するときの説明である｡

・複数のSW･単数のCdS･複数のCVDを実装可能｡但しﾎﾟｰﾄ割当てが可能なもの｡
・それぞれをｵﾙｺﾞｰﾙ演奏･ﾌﾗｯｼｭ音声再生･ﾌﾟﾛｸﾞﾗﾑ音声再生の開始･中断に充てることができる｡
・短ｵﾝで開始､長ｵﾝで中断する｡
・曲･音声はﾗｳﾝﾄﾞﾛﾋﾞﾝ･ﾗﾝﾀﾞﾑが可能｡

・SWはﾎﾟｰﾄ-GND間に設置し､負論理(導通=L)で入力する｡

・CdSはﾎﾟｰﾄ-GND間に設置し､正論理(暗=H)で入力する｡

・CVDはCVDﾊﾟｯﾄﾞﾎﾟｰﾄから10kΩの抵抗を介してﾎﾟｰﾄに入力する｡
　10kΩはﾎﾟｰﾄ保護のためであり､必須ではない｡

・xxxx_AUTOを宣言することで､自動的に曲を演奏開始､及び音声を再生開始する｡

・音声ﾃﾞｰﾀはﾌﾟﾛｸﾞﾗﾑﾒﾓﾘ､I2Cﾌﾗｯｼｭ､SDｶｰﾄﾞ､SPIﾌﾗｯｼｭに格納しておく｡


//==============================================
//I2Cﾌﾗｯｼｭﾒﾓﾘのﾋﾟﾝ接
//==============================================
1:A0(Vssに繋ぐ)
2:A1(Vssに繋ぐ)
3:A2(Vssに繋ぐ)
4:Vss
5:SDA(1.5kで外部ﾌﾟﾙｱｯﾌﾟ)
6:SCL(1.5kで外部ﾌﾟﾙｱｯﾌﾟ)
7:WP(Vccに繋ぐ)
8:Vcc


//==============================================
//SDCのﾋﾟﾝ接(SPIﾓｰﾄﾞ)
//==============================================
9:空き(不使用)
1:CS(2線SPIでは10k*220pの時定数回路を介してCLKに繋ぐ)
2:DI
3:Vss1
4:Vdd
5:CLK
6:Vss2
7:DO(2線SPI･3線SPIでは1.5kを介してDIに繋ぐ)
8:IRQ(不使用)


//==============================================
//SPIﾌﾗｯｼｭﾒﾓﾘのﾋﾟﾝ接
//==============================================
W25Qのﾋﾟﾝ接(標準SPIﾓｰﾄﾞ)
1:CS(2線SPIでは10k*220pの時定数回路を介してCLKに繋ぐ)
2:DO(2線SPI･3線SPIではDIに繋ぐ)
3:WP(GNDに接続､ISPを利用する場合はVccに接続)
4:GND
5:DI
6:CLK
7:HOLD(Vccに接続)
8:Vcc



//==============================================
//PY32のﾋﾟﾝ接
//==============================================
PY32F002A(16ﾋﾟﾝ)のﾎﾟｰﾄの割当て
1:(PA7)/(I2C_SDA)SDA(24FCのSDAに繋ぐ)/(ADC_IN7)CdS
2:(PB0)SW1/(TIM1_CH2N)ｺﾝﾌﾟﾘ逆相出力
3:(PB1)SW1/(TIM1_CH3N)ﾌﾞﾘｯｼﾞ･ﾌﾞﾚｰｷ逆相出力
4:Vcc
5:(PA9)/(TIM1_CH2)正相出力
6:(PA13-SWD)ﾃﾞﾊﾞｸﾞ信号出力
7:(PA14-SWC)ISP時のﾃﾞﾊﾞｸﾞ情報出力
8:(PF2-NRST)SW0=====>PF2はNRST機能を無効にしてGPIO機能に設定しておくこと
9:(PF0-OSCIN)/(USART1_RX)ﾃﾞﾊﾞｸﾞ情報入力､UART入力
10:(PF1-OSCOUT)ﾃﾞﾊﾞｸﾞ情報出力/(USART1_TX)ﾃﾞﾊﾞｸﾞ情報出力､UART出力
11:(PA0)/(SPI1_MISO)MISO(M25QのDO･SDCのDOに繋ぐ､2線3線SPIでは不使用)
12:(PA1)BOOT0代替/(SPI1_MOSI)MOSI(M25QのDI･SDCのDIに繋ぐ)
13:Vss
14:(PA2)/(SPI1_SCK)SCK(M25QのCLK･SDCのCLKに繋ぐ)
15:(PA3)/(I2C_SCL)SCL(24FCのSCLに繋ぐ)/(ADC_IN3)CVD0
16:(PA6)SS(M25QのCS･SDCのCSに繋ぐ､2線SPIでは不使用)
*/


//==============================================
//ﾍｯﾀﾞﾌｧｲﾙの呼込み
//==============================================
#include "py32f0xx.h"				//ｼｽﾃﾑのﾍｯﾀﾞﾌｧｲﾙ
#include "orgel_cnf.h"				//構成情報
#include "../orgel.h"				//ｱﾌﾟﾘのﾍｯﾀﾞﾌｧｲﾙ


//==============================================
//構成情報(ｱﾌﾟﾘｹｰｼｮﾝとﾀｰｹﾞｯﾄﾃﾞﾊﾞｲｽに関わるもの)
//==============================================
//UARTのﾎﾟｰﾄ定義
#define	UART_GPIO	GPIOF			//UART出力のﾎﾟｰﾄ
#define	UART_BIT	1			//UART出力のﾋﾞｯﾄ
#define	UART_AF		8			//UART出力のUART_TX交代機能番号
#define	UART_IN_GPIO	GPIOF			//UART入力のﾎﾟｰﾄ
#define	UART_IN_BIT	0			//UART入力のﾋﾞｯﾄ
#define	UART_IN_AF	8			//UART入力のUART_RX交代機能番号

//ﾃﾞﾊﾞｸﾞ信号のﾎﾟｰﾄ定義
#define	DBS_GPIO	GPIOA			//ﾃﾞﾊﾞｸﾞ信号のﾎﾟｰﾄ
#define	DBS_BIT		13			//ﾃﾞﾊﾞｸﾞ信号のﾋﾞｯﾄ

//ﾃﾞﾊﾞｸﾞ情報のﾎﾟｰﾄ定義
#ifdef	ISP_SPIFLASH_EN			//ISP環境でSPIﾌﾗｯｼｭを書替えるとき
#define	SHDBG_SOFT				//ﾃﾞﾊﾞｸﾞ情報のUARTをｿﾌﾄ実装する
#define	DBG_GPIO	GPIOA			//ﾃﾞﾊﾞｸﾞ情報出力のﾎﾟｰﾄ
#define	DBG_BIT		14			//ﾃﾞﾊﾞｸﾞ情報出力のﾋﾞｯﾄ
#else					//電子ｵﾙｺﾞｰﾙ動作のとき
#define	DBG_GPIO	GPIOF			//ﾃﾞﾊﾞｸﾞ情報のﾎﾟｰﾄ
#define	DBG_BIT		1			//ﾃﾞﾊﾞｸﾞ情報のﾋﾞｯﾄ
#define	DBG_AF		8			//UARTの交代機能番号
#define	DBG_IN_GPIO	GPIOF			//ﾃﾞﾊﾞｸﾞ情報のﾎﾟｰﾄ
#define	DBG_IN_BIT	0			//ﾃﾞﾊﾞｸﾞ情報のﾋﾞｯﾄ
#define	DBG_IN_AF	8			//UARTの交代機能番号
#endif

//PWMのﾎﾟｰﾄ定義
#define	PWM_F_GPIO	GPIOA			//PWM正相のﾎﾟｰﾄ
#define	PWM_F_BIT	9			//PWM正相のﾋﾞｯﾄ
#define	PWM_F_AF	2			//PWM正相の交代機能番号
#define	PWM_R_GPIO	GPIOB			//PWM逆相のﾎﾟｰﾄ
#define	PWM_R_BIT	1			//PWM逆相のﾋﾞｯﾄ
#define	PWM_R_AF	2			//PWM逆相の交代機能番号
#define	PWM_C_GPIO	GPIOB			//PWM相補のﾎﾟｰﾄ
#define	PWM_C_BIT	0			//PWM相補のﾋﾞｯﾄ
#define	PWM_C_AF	2			//PWM相補の交代機能番号

//I2Cのﾎﾟｰﾄ定義
#define I2C_SCL_GPIO	GPIOA			//SCLのﾎﾟｰﾄ
#define I2C_SCL_BIT	3			//SCLのﾋﾞｯﾄ
#define	I2C_SCL_AF	12			//SCLの交代機能番号
#define I2C_SDA_GPIO	GPIOA			//SDAのﾎﾟｰﾄ
#define I2C_SDA_BIT	7			//SDAのﾋﾞｯﾄ
#define	I2C_SDA_AF	12			//SDAの交代機能番号

//SPIのﾎﾟｰﾄ定義
#define SPI_SCK_GPIO	GPIOA			//SCKのﾎﾟｰﾄ
#define SPI_SCK_BIT	2			//SCKのﾋﾞｯﾄ
#define	SPI_SCK_AF	10			//SCKの交代機能番号
#define SPI_MOSI_GPIO	GPIOA			//MOSIのﾎﾟｰﾄ
#define SPI_MOSI_BIT	1			//MOSIのﾋﾞｯﾄ
#define	SPI_MOSI_AF	10			//MOSIの交代機能番号
#ifdef SHSPI2				//2線SPIのとき
#define	SPI_SS_TIME	10			//SSﾈｹﾞｰﾄ待ち時間[us]
#define SPI_SS_GPIO	SPI_SCK_GPIO		//SSのﾎﾟｰﾄ
#define SPI_SS_BIT	SPI_SCK_BIT		//SSのﾋﾞｯﾄ
#else					//3線SPIまたは4線SPIのとき
#define SPI_SS_GPIO	GPIOA			//SSのﾎﾟｰﾄ
#define SPI_SS_BIT	6			//SSのﾋﾞｯﾄ
#endif
#if defined(SHSPI2)||defined(SHSPI3)	//2線SPIまたは3線SPIのとき
#define	SPI_SOFT				//SPIをｿﾌﾄ実装する
#define SPI_MISO_GPIO	SPI_MOSI_GPIO		//MISOのﾎﾟｰﾄ
#define SPI_MISO_BIT	SPI_MOSI_BIT		//MISOのﾋﾞｯﾄ
#else					//4線SPIのとき
#define SPI_MISO_GPIO	GPIOA			//MISOのﾎﾟｰﾄ
#define SPI_MISO_BIT	0			//MISOのﾋﾞｯﾄ
#define	SPI_MISO_AF	10			//MISOの交代機能番号
#endif

//SPIのﾌﾟﾙｱｯﾌﾟ/ﾀﾞｳﾝ定義
#define	SPI_SCK_PU				//SPI_SCKをﾌﾟﾙｱｯﾌﾟする
//#define	SPI_SCK_PD			//SPI_SCKをﾌﾟﾙﾀﾞｳﾝする
#define	SPI_MOSI_PU				//SPI_MOSIをﾌﾟﾙｱｯﾌﾟする
//#define	SPI_MOSI_PD			//SPI_MOSIをﾌﾟﾙﾀﾞｳﾝする
#define	SPI_MISO_PU				//SPI_MISOをﾌﾟﾙｱｯﾌﾟする
//#define	SPI_MISO_PD			//SPI_MISOをﾌﾟﾙﾀﾞｳﾝする

//SW0のﾎﾟｰﾄ定義	(Dr.W)
#define	SW0_GPIO	GPIOF			//SW0のﾎﾟｰﾄ
#define	SW0_EXTI_PORT	2			//SW0のEXTIﾎﾟｰﾄ(0=PA､1=PB､2=PF)
#define	SW0_BIT		2			//SW0のﾋﾞｯﾄ
#define	SW0_INV					//SW0入力は負論理
					//以下のﾀｲﾏｰﾓｰﾄﾞとﾘｱﾙﾓｰﾄﾞは択一
//#define SW0_TIMER				//SW0はﾀｲﾏｰﾓｰﾄﾞ
#define	SW0_REAL			//SW0はﾘｱﾙﾓｰﾄﾞ
#define	SW0_CNT		VOICES1_CNT
					//SW0の操作対象

//SW1のﾎﾟｰﾄ定義	 (Dr.W)
#define	SW1_GPIO	GPIOB			//SW1のﾎﾟｰﾄ
#define	SW1_EXTI_PORT	1			//SW1のEXTIﾎﾟｰﾄ(0=PA､1=PB､2=PF)
#if BTL==1				//ｺﾝﾌﾟﾘ出力のとき
#define	SW1_BIT		1			//SW1のﾋﾞｯﾄ
#else					//ｼﾝｸﾞﾙ･ﾌﾞﾘｯｼﾞ･ﾌﾞﾚｰｷ出力のとき
#define	SW1_BIT		0			//SW1のﾋﾞｯﾄ
#endif
#define	SW1_INV					//SW1入力は負論理
					//以下のﾀｲﾏｰﾓｰﾄﾞとﾘｱﾙﾓｰﾄﾞは択一
//#define SW1_TIMER				//SW1はﾀｲﾏｰﾓｰﾄﾞ
#define	SW1_REAL			//SW1はﾘｱﾙﾓｰﾄﾞ
#define	SW1_CNT		VOICES2_CNT	//SW1の操作対象

//SW2のﾎﾟｰﾄ定義	 (Dr.W)
#define	SW2_GPIO	GPIOF			//SW2のﾎﾟｰﾄ
#define	SW2_EXTI_PORT	2			//SW2のEXTIﾎﾟｰﾄ(0=PA､1=PB､2=PF)
#if BTL==1				//ｺﾝﾌﾟﾘ出力のとき
#define	SW2_BIT		0			//SW2のﾋﾞｯﾄ
#else					//ｼﾝｸﾞﾙ･ﾌﾞﾘｯｼﾞ･ﾌﾞﾚｰｷ出力のとき
#define	SW2_BIT		1			//SW2のﾋﾞｯﾄ
#endif
#define	SW2_INV					//SW2入力は負論理
					//以下のﾀｲﾏｰﾓｰﾄﾞとﾘｱﾙﾓｰﾄﾞは択一
//#define SW2_TIMER				//SW2はﾀｲﾏｰﾓｰﾄﾞ
#define	SW2_REAL			//SW2はﾘｱﾙﾓｰﾄﾞ
#define	SW2_CNT		VOICES3_CNT	//SW2の操作対象

//SW3のﾎﾟｰﾄ定義	 (Dr.W)
#define	SW3_GPIO	GPIOA			//SW3のﾎﾟｰﾄ
#define	SW3_EXTI_PORT	2			//SW3のEXTIﾎﾟｰﾄ(0=PA､1=PB､2=PF)
#define	SW3_BIT		3			//SW3のﾋﾞｯﾄ
#define	SW3_INV					//SW3入力は負論理
					//以下のﾀｲﾏｰﾓｰﾄﾞとﾘｱﾙﾓｰﾄﾞは択一
//#define SW3_TIMER				//SW3はﾀｲﾏｰﾓｰﾄﾞ
#define	SW3_REAL			//SW3はﾘｱﾙﾓｰﾄﾞ
#define	SW3_CNT		VOICES4_CNT	//SW3の操作対象 

//CdSのﾎﾟｰﾄ定義
#define	CdS_GPIO	GPIOA			//CdSのﾎﾟｰﾄ
#define	CdS_BIT		7			//CdSのﾋﾞｯﾄ
#define	CdS_CHSEL	0x0080			//CdSのADCCHSEL
//#define	CdS_INV				//CdS入力は負論理(暗=L)
					//以下のﾀｲﾏｰﾓｰﾄﾞとﾘｱﾙﾓｰﾄﾞは択一
#define CdS_TIMER				//CdSはﾀｲﾏｰﾓｰﾄﾞ
//#define	CdS_REAL			//CdSはﾘｱﾙﾓｰﾄﾞ
#define	CdS_CNT		ORGEL_CNT|VOICEN_CNT|VOICEI_CNT|VOICEC_CNT|VOICES_CNT
					//CdSの操作対象

//CVD0のﾎﾟｰﾄ定義
#define	CVD0_GPIO	GPIOA			//CVD0のﾎﾟｰﾄ
#define	CVD0_BIT	3			//CVD0のﾋﾞｯﾄ
#define	CVD0_CHSEL	0x0008			//CVD0のADCCHSEL
					//以下のﾀｲﾏｰﾓｰﾄﾞとﾘｱﾙﾓｰﾄﾞは択一
#define CVD0_TIMER				//CVD0はﾀｲﾏｰﾓｰﾄﾞ
//#define	CVD0_REAL			//CVD0はﾘｱﾙﾓｰﾄﾞ
#define	CVD0_CNT	ORGEL_CNT|VOICEN_CNT|VOICEI_CNT|VOICEC_CNT|VOICES_CNT
					//CVD0の操作対象

//LEDのﾎﾟｰﾄ定義	 (Dr.W)
#define	LED0_GPIO	GPIOA		//LED0のﾎﾟｰﾄ
#define	LED0_BIT	0		//LED0のﾋﾞｯﾄ
#define	LED1_GPIO	GPIOF		//LED1のﾎﾟｰﾄ
//#define	LED1_BIT	7		//LED1のﾋﾞｯﾄ
//#define	LED2_GPIO	GPIOD		//LED2のﾎﾟｰﾄ
//#define	LED2_BIT	3		//LED2のﾋﾞｯﾄ
//#define	LED3_GPIO	GPIOC		//LED3のﾎﾟｰﾄ
//#define	LED3_BIT	2		//LED3のﾋﾞｯﾄ


#if	PART_N+KB_N>0
//==============================================
//ｿﾝｸﾞﾃﾞｰﾀのｲﾝﾃﾞｯｸｽ
//==============================================
extern const struct SONG_IDX SONG_IDX;		//曲ｲﾝﾃﾞｯｸｽ実体
#endif


#if	VOICE_N>0
//==============================================
//音声ﾃﾞｰﾀ
//==============================================
#ifdef VOICEN_IMA			//IMA-ADPCMのとき
#include	"../voice_WAN_IMA.c"
#include	"../voice_DAME_IMA.c"
#include	"../voice_CHIKO_IMA.c"

#else					//8ﾋﾞｯﾄPCMのとき
#include	"../voice_WAN.c"
#include	"../voice_DAME.c"
#include	"../voice_CHIKO.c"

#endif
//==============================================
//音声ﾃﾞｰﾀ(ﾌﾟﾛｸﾞﾗﾑﾌﾗｯｼｭ)のｲﾝﾃﾞｯｸｽ
//==============================================
static const struct VOICEN_HDR voiceN_idx[]=
{
	von_mac(voice_WAN,sizeof(voice_WAN),8000)
	von_mac(voice_DAME,sizeof(voice_DAME),8000)
	von_mac(voice_CHIKO,sizeof(voice_CHIKO),8000)
};
#define VOICEN_SU	(sizeof(voiceN_idx)/sizeof(voiceN_idx[0]))
#endif


#if	VOICE_I>0
//==============================================
//音声ﾃﾞｰﾀ(I2Cﾌﾗｯｼｭ)のｲﾝﾃﾞｯｸｽ
//==============================================
static const struct VOICEI_HDR voiceI_idx[]=
{

#ifdef VOICEI_IMA			//IMA-ADPCMのとき
	//voice_DAMEのW25Q_IMAを使う
	voi_mac(0x000000,0x0007df,8000,0xa0)	//1:WAN_1ch16bit16ksps.wav
	voi_mac(0x0007df,0x000797,8000,0xa0)	//2:DAME_1ch16bit16ksps.wav
	voi_mac(0x000f76,0x001d0b,8000,0xa0)	//3:CHIKO_1ch16bit16ksps.wav

#else					//8ﾋﾞｯﾄPCMのとき
	//voice_DAMEのW25Qを使う
	voi_mac(0x000000,0x000fbe,8000,0xa0)	//1:WAN_1ch16bit16ksps.wav
	voi_mac(0x000fbe,0x000f2f,8000,0xa0)	//2:DAME_1ch16bit16ksps.wav
	voi_mac(0x001eed,0x003a17,8000,0xa0)	//3:CHIKO_1ch16bit16ksps.wav

/*
	//voice_Mirrorna\W25Q16.hexを使う
	voi_mac(0x000000,0x002401,8000,0xa0)	//1:sharin.wav
	voi_mac(0x002401,0x004575,8000,0xa0)	//2:kirakiraomeme.wav
	voi_mac(0x006976,0x001956,8000,0xa0)	//3:aidorumitai.wav
	voi_mac(0x0082cc,0x002b18,8000,0xa0)	//4:kyounooshare.wav
	voi_mac(0x00ade4,0x001d61,8000,0xa0)	//5:egaogasuteki.wav
	voi_mac(0x00cb45,0x0033a5,8000,0xa0)	//6:kirarin.wav
*/
#endif

};
#define VOICEI_SU	(sizeof(voiceI_idx)/sizeof(voiceI_idx[0]))
#endif


#if	VOICE_C>0
//==============================================
//音声ﾃﾞｰﾀ(SDｶｰﾄﾞ)のｲﾝﾃﾞｯｸｽ
//==============================================
static const struct VOICEC_HDR voiceC_idx[]=
{

	//｢ｱﾝﾊﾟﾝﾏﾝｼｮｰ｣の音声ﾃﾞｰﾀ(SDのANPANMAN.wav)を使う
	voc_mac(0x00029e,0x00e367,8000)		//anpan
//	voc_mac(0x002000,0x000080,8000)		//anpanの一部(ﾃｽﾄ用)
//	voc_mac(0x004000,0x000080,8000)		//anpanの一部(ﾃｽﾄ用)

};
#define VOICEC_SU	(sizeof(voiceC_idx)/sizeof(voiceC_idx[0]))
#endif


#if	VOICE_S>0
//==============================================
//音声ﾃﾞｰﾀ(SPIﾌﾗｯｼｭ)のｲﾝﾃﾞｯｸｽ
//==============================================
static const struct VOICES_HDR voiceS_idx[]=
{

#ifdef VOICES_IMA			//IMA-ADPCMのとき
	//｢湘南電車｣の音声ﾃﾞｰﾀ(voice_JR113\W25Q16_IMA.hex)を使う
	vos_mac(0x000000,0x022854,16000)	//1:JR113_1.wav
	vos_mac(0x022854,0x0184dc,16000)	//2:JR113_2.wav
	vos_mac(0x03ad30,0x00c2a8,16000)	//3:JR113_3.wav
	vos_mac(0x046fd8,0x025600,16000)	//4:JR113_4.wav

	//｢サウンド新幹線｣の音声ﾃﾞｰﾀ(voice_N700A\W25Q16_IMA.hex)を使う
//	vos_mac(0x000000,0x00a9de,16000)	//1:N700A1.wav
//	vos_mac(0x00a9de,0x00cb5a,16000)	//2:N700A2.wav
//	vos_mac(0x017538,0x006fa2,16000)	//3:N700A3.wav
//	vos_mac(0x01e4da,0x0228b2,16000)	//4:N700A4.wav
//	vos_mac(0x040d8c,0x00c556,16000)	//5:N700A5.wav

#else
	//｢湘南電車｣の音声ﾃﾞｰﾀ(voice_JR113\W25Q32.hex)を使う
	vos_mac(0x000000,0x0450a8,16000)	//1:JR113_1.wav
	vos_mac(0x0450a8,0x0309b8,16000)	//2:JR113_2.wav
	vos_mac(0x075a60,0x018550,16000)	//3:JR113_3.wav
	vos_mac(0x08dfb0,0x04ac00,16000)	//4:JR113_4.wav

	//｢サウンド新幹線｣の音声ﾃﾞｰﾀ(voice_N700A\W25Q32.hex)を使う
//	vos_mac(0x000000,0x00a9de,16000)	//1:N700A1.wav
//	vos_mac(0x00a9de,0x00cb5a,16000)	//2:N700A2.wav
//	vos_mac(0x017538,0x006fa2,16000)	//3:N700A3.wav
//	vos_mac(0x01e4da,0x0228b2,16000)	//4:N700A4.wav
//	vos_mac(0x040d8c,0x00c556,16000)	//5:N700A5.wav

#endif
};
#define VOICES_SU	(sizeof(voiceS_idx)/sizeof(voiceS_idx[0]))
#endif


//==============================================
//共通ｺｰﾄﾞの呼込み
//==============================================
#ifdef ISP_SPIFLASH_EN			//ISP環境でSPIﾌﾗｯｼｭを書替えるとき
#define	UARTBPS	230400				//UARTは230400bps
#define	UART_TX					//UART送信を使う
#define	UART_RX					//UART受信を使う
#include "../dev_F002A_16.c"			//F002A_16ﾋﾟﾝﾃﾞﾊﾞｲｽｺｰﾄﾞ
#include "../apl_ISP_SPIFLASH.c"		//ISP実行ｱﾌﾟﾘｹｰｼｮﾝ
#else					//電子ｵﾙｺﾞｰﾙ動作のとき
#include "../dev_F002A_16.c"			//F002A_16ﾋﾟﾝﾃﾞﾊﾞｲｽｺｰﾄﾞ
#include "../apl_SW_TRAIN.c"				//SW操作ｱﾌﾟﾘｹｰｼｮﾝ	(Dr.W)
#endif
