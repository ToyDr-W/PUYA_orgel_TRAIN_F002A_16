//SW･CdS･CVDで電子ｵﾙｺﾞｰﾙを制御するｱﾌﾟﾘｹｰｼｮﾝの共通機能部

//複数のSW･１個のCdS･複数のCVDを実装可能｡但しﾎﾟｰﾄ割当てが可能なもの｡
//それぞれをｵﾙｺﾞｰﾙ演奏･ﾌﾗｯｼｭ音声再生･ﾌﾟﾛｸﾞﾗﾑ音声再生の開始･中断に充てることができる｡
//短ｵﾝで開始､長ｵﾝで中断する｡
//曲･音声はﾗｳﾝﾄﾞﾛﾋﾞﾝ･ﾗﾝﾀﾞﾑが可能｡

//SWﾎﾟｰﾄの内部ﾌﾟﾙｱｯﾌﾟ/ﾀﾞｳﾝはこのｺｰﾄﾞ内で行っている｡
//CdSﾎﾟｰﾄの内部ﾌﾟﾙｱｯﾌﾟ/ﾀﾞｳﾝはfunc_CdS.cで行っている｡
//CVDﾎﾟｰﾄの内部ﾌﾟﾙﾀﾞｳﾝはfunc_CVD.cで行っている｡

//下記のｵﾌﾟｼｮﾝ設定を呼出し元で宣言しておくこと｡
//#define	SLEEP_EN	//Sleep機能を実装するときに宣言する
//#define	KYOKU_AUTO	//ｵﾙｺﾞｰﾙ演奏を自動開始するときに宣言する
//#define	ONSEIN_AUTO	//音声0再生を自動開始するときに宣言する
//#define	ONSEII_AUTO	//音声I再生を自動開始するときに宣言する
//#define	ONSEIC_AUTO	//音声C再生を自動開始するときに宣言する
//#define	ONSEIS_AUTO	//音声S再生を自動開始するときに宣言する
//#define	RAND_CHSEL	0x00000001	//ﾗﾝﾀﾞﾏｲｽﾞの種にするCHSEL(ﾃﾞﾌｫﾙﾄはCH0:PA0)
//#define	KYOKU_RAND	//曲番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEIN_RAND	//音声N番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEII_RAND	//音声I番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEIC_RAND	//音声C番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	ONSEIS_RAND	//音声S番号をﾗﾝﾀﾞﾏｲｽﾞするときに宣言する
//#define	SW_EN		//SWを実装するときに宣言する
//#define	SW_PLNG		//SWをｲﾍﾞﾝﾄではなく､ﾎﾟｰﾘﾝｸﾞするときに宣言する
//#define	CdS_EN		//CdSを実装するときに宣言する
//#define	CVD_EN		//CVDを実装するときに宣言する

//ｵﾙｺﾞｰﾙ箱への応用では以下を宣言しておくこと
//#define	CdS_FUTA	//蓋を開ける(明)とｵﾝ判定する
//#define	CdS_THR	100	//判定閾値を固定の宣言値にする

/*
ｻｳﾝﾄﾞ･ﾄﾚｲﾝでのｶｽﾀﾏｲｽﾞ(20261020 ToyDr.わたなべ)

・音声出力はBTLﾌﾞﾚｰｷ出力

・SWの割り当て
SW0:音声S1を再生(押しボタン1)
SW1:音声S2を再生(押しボタン2)
SW2:音声S3を再生(押しボタン4)
SW3:以降の音声Sをﾗｳﾝﾄﾞﾛﾋﾞﾝに再生(押しボタン3)

・SWの操作
ﾘｱﾙﾓｰﾄﾞのｵﾝ変化のみを処理する
実行中の音声再生/曲演奏を中断してから対応する動作を開始する

・LEDの割り当て
LED0:ﾍｯﾄﾞﾗｲﾄ左
LED1:ﾍｯﾄﾞﾗｲﾄ右

・sleep時はEXTI0～15(SW)割込みでWakeupする
*/

//==============================================
//構成情報(個別ｱﾌﾟﾘｹｰｼｮﾝだけで使うもの)
//==============================================
#define	SLEEP_TMR	1000			//Sleepに入る前の時限[ms]
						//　操作の評価時間よりも長くすること


//==============================================
//操作のﾋﾞｯﾄ定義			(Dr.W)
//==============================================
#define	VOICES1_CNT	0x01		//音声S1操作
#define	VOICES2_CNT	0x02		//音声S2操作
#define	VOICES3_CNT	0x03		//音声S3操作
#define	VOICES4_CNT	0x04		//音声S4操作
#define	NON_CNT		0x00		//操作無し


#ifdef SW_EN				//SWを実装するとき
//==============================================
//SW構成の定義
//==============================================
#define	SW_MODE	0x80			//ﾓｰﾄﾞ設定ﾏｽｸ
#define	SW_POL	0x40			//極性設定ﾏｽｸ
#define	SW_WAKEUP	0x20		//Wakeup設定ﾏｽｸ
static const struct SW_CNF
{
	GPIO_TypeDef *gpio;		//GPIOｱﾄﾞﾚｽ
	unsigned short msk;		//ﾋﾞｯﾄﾏｽｸ
	unsigned char mode;		//b7:ﾓｰﾄﾞ(0=ﾘｱﾙﾓｰﾄﾞ､1=ﾀｲﾏｰﾓｰﾄﾞ)
					//b6:入力極性(0=負論理､1=正論理)
					//b5:Wakeup(0=非対象､1=対象)
}sw_cnf[]=
{

//SW0の構成-------------------------------------
#ifdef SW0_BIT				//SW0を実装するとき
#ifdef SW0_TIMER
#define SW0_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW0_REAL
#define SW0_MODE 0x00
#define SW_REAL
#endif
#ifdef SW0_INV
#define SW0_POL 0x00
#else
#define SW0_POL SW_POL
#endif
#ifdef SW0_NWU
#define SW0_WAKEUP 0x00
#else
#define SW0_WAKEUP SW_WAKEUP
#endif
	{SW0_GPIO,1<<SW0_BIT,SW0_MODE|SW0_POL|SW0_WAKEUP},
#if defined(SW0_EXTI_PORT)&&!defined(SW0_NWU)
					//SW0をEXTI実装するとき
#if SW0_BIT<4
#define SW0_EXTICR1	(SW0_EXTI_PORT<<SW0_BIT*8)
#define SW0_EXTICR2	0x00000000
#define SW0_EXTICR3	0x00000000
#define SW0_EXTICR4	0x00000000
#elif SW0_BIT<8
#define SW0_EXTICR1	0x00000000
#define SW0_EXTICR2	(SW0_EXTI_PORT<<(SW0_BIT-4)*8)
#define SW0_EXTICR3	0x00000000
#define SW0_EXTICR4	0x00000000
#elif SW0_BIT<12
#define SW0_EXTICR1	0x00000000
#define SW0_EXTICR2	0x00000000
#define SW0_EXTICR3	(SW0_EXTI_PORT<<(SW0_BIT-8)*8)
#define SW0_EXTICR4	0x00000000
#else
#define SW0_EXTICR1	0x00000000
#define SW0_EXTICR2	0x00000000
#define SW0_EXTICR3	0x00000000
#define SW0_EXTICR4	(SW0_EXTI_PORT<<(SW0_BIT-12)*8)
#endif
#define SW0_EXTIMSK	(1<<SW0_BIT)
#endif
#endif
#ifndef SW0_EXTIMSK			//SW0をEXTI実装しないとき
#define SW0_EXTICR1	0x00000000
#define SW0_EXTICR2	0x00000000
#define SW0_EXTICR3	0x00000000
#define SW0_EXTICR4	0x00000000
#define SW0_EXTIMSK	0x00000000
#endif

//SW1の構成-------------------------------------
#ifdef SW1_BIT				//SW1を実装するとき
#ifdef SW1_TIMER
#define SW1_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW1_REAL
#define SW1_MODE 0x00
#define SW_REAL
#endif
#ifdef SW1_INV
#define SW1_POL 0x00
#else
#define SW1_POL SW_POL
#endif
#ifdef SW1_NWU
#define SW1_WAKEUP 0x00
#else
#define SW1_WAKEUP SW_WAKEUP
#endif
	{SW1_GPIO,1<<SW1_BIT,SW1_MODE|SW1_POL|SW1_WAKEUP},
#if defined(SW1_EXTI_PORT)&&!defined(SW1_NWU)
					//SW1をEXTI実装するとき
#if SW1_BIT<4
#define SW1_EXTICR1	(SW1_EXTI_PORT<<SW1_BIT*8)
#define SW1_EXTICR2	0x00000000
#define SW1_EXTICR3	0x00000000
#define SW1_EXTICR4	0x00000000
#elif SW1_BIT<8
#define SW1_EXTICR1	0x00000000
#define SW1_EXTICR2	(SW1_EXTI_PORT<<(SW1_BIT-4)*8)
#define SW1_EXTICR3	0x00000000
#define SW1_EXTICR4	0x00000000
#elif SW1_BIT<12
#define SW1_EXTICR1	0x00000000
#define SW1_EXTICR2	0x00000000
#define SW1_EXTICR3	(SW1_EXTI_PORT<<(SW1_BIT-8)*8)
#define SW1_EXTICR4	0x00000000
#else
#define SW1_EXTICR1	0x00000000
#define SW1_EXTICR2	0x00000000
#define SW1_EXTICR3	0x00000000
#define SW1_EXTICR4	(SW1_EXTI_PORT<<(SW1_BIT-12)*8)
#endif
#define SW1_EXTIMSK	(1<<SW1_BIT)
#endif
#endif
#ifndef SW1_EXTIMSK			//SW1をEXTI実装しないとき
#define SW1_EXTICR1	0x00000000
#define SW1_EXTICR2	0x00000000
#define SW1_EXTICR3	0x00000000
#define SW1_EXTICR4	0x00000000
#define SW1_EXTIMSK	0x00000000
#endif

//SW2の構成-------------------------------------
#ifdef SW2_BIT				//SW2を実装するとき
#ifdef SW2_TIMER
#define SW2_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW2_REAL
#define SW2_MODE 0x00
#define SW_REAL
#endif
#ifdef SW2_INV
#define SW2_POL 0x00
#else
#define SW2_POL SW_POL
#endif
#ifdef SW2_NWU
#define SW2_WAKEUP 0x00
#else
#define SW2_WAKEUP SW_WAKEUP
#endif
	{SW2_GPIO,1<<SW2_BIT,SW2_MODE|SW2_POL|SW2_WAKEUP},
#if defined(SW2_EXTI_PORT)&&!defined(SW2_NWU)
					//SW2をEXTI実装するとき
#if SW2_BIT<4
#define SW2_EXTICR1	(SW2_EXTI_PORT<<SW2_BIT*8)
#define SW2_EXTICR2	0x00000000
#define SW2_EXTICR3	0x00000000
#define SW2_EXTICR4	0x00000000
#elif SW2_BIT<8
#define SW2_EXTICR1	0x00000000
#define SW2_EXTICR2	(SW2_EXTI_PORT<<(SW2_BIT-4)*8)
#define SW2_EXTICR3	0x00000000
#define SW2_EXTICR4	0x00000000
#elif SW2_BIT<12
#define SW2_EXTICR1	0x00000000
#define SW2_EXTICR2	0x00000000
#define SW2_EXTICR3	(SW2_EXTI_PORT<<(SW2_BIT-8)*8)
#define SW2_EXTICR4	0x00000000
#else
#define SW2_EXTICR1	0x00000000
#define SW2_EXTICR2	0x00000000
#define SW2_EXTICR3	0x00000000
#define SW2_EXTICR4	(SW2_EXTI_PORT<<(SW2_BIT-12)*8)
#endif
#define SW2_EXTIMSK	(1<<SW2_BIT)
#endif
#endif
#ifndef SW2_EXTIMSK			//SW2をEXTI実装しないとき
#define SW2_EXTICR1	0x00000000
#define SW2_EXTICR2	0x00000000
#define SW2_EXTICR3	0x00000000
#define SW2_EXTICR4	0x00000000
#define SW2_EXTIMSK	0x00000000
#endif

//SW3の構成-------------------------------------
#ifdef SW3_BIT				//SW3を実装するとき
#ifdef SW3_TIMER
#define SW3_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW3_REAL
#define SW3_MODE 0x00
#define SW_REAL
#endif
#ifdef SW3_INV
#define SW3_POL 0x00
#else
#define SW3_POL SW_POL
#endif
#ifdef SW3_NWU
#define SW3_WAKEUP 0x00
#else
#define SW3_WAKEUP SW_WAKEUP
#endif
	{SW3_GPIO,1<<SW3_BIT,SW3_MODE|SW3_POL|SW3_WAKEUP},
#if defined(SW3_EXTI_PORT)&&!defined(SW3_NWU)
					//SW3をEXTI実装するとき
#if SW3_BIT<4
#define SW3_EXTICR1	(SW3_EXTI_PORT<<SW3_BIT*8)
#define SW3_EXTICR2	0x00000000
#define SW3_EXTICR3	0x00000000
#define SW3_EXTICR4	0x00000000
#elif SW3_BIT<8
#define SW3_EXTICR1	0x00000000
#define SW3_EXTICR2	(SW3_EXTI_PORT<<(SW3_BIT-4)*8)
#define SW3_EXTICR3	0x00000000
#define SW3_EXTICR4	0x00000000
#elif SW3_BIT<12
#define SW3_EXTICR1	0x00000000
#define SW3_EXTICR2	0x00000000
#define SW3_EXTICR3	(SW3_EXTI_PORT<<(SW3_BIT-8)*8)
#define SW3_EXTICR4	0x00000000
#else
#define SW3_EXTICR1	0x00000000
#define SW3_EXTICR2	0x00000000
#define SW3_EXTICR3	0x00000000
#define SW3_EXTICR4	(SW3_EXTI_PORT<<(SW3_BIT-12)*8)
#endif
#define SW3_EXTIMSK	(1<<SW3_BIT)
#endif
#endif
#ifndef SW3_EXTIMSK			//SW3をEXTI実装しないとき
#define SW3_EXTICR1	0x00000000
#define SW3_EXTICR2	0x00000000
#define SW3_EXTICR3	0x00000000
#define SW3_EXTICR4	0x00000000
#define SW3_EXTIMSK	0x00000000
#endif

//SW4の構成-------------------------------------
#ifdef SW4_BIT				//SW4を実装するとき
#ifdef SW4_TIMER
#define SW4_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW4_REAL
#define SW4_MODE 0x00
#define SW_REAL
#endif
#ifdef SW4_INV
#define SW4_POL 0x00
#else
#define SW4_POL SW_POL
#endif
#ifdef SW4_NWU
#define SW4_WAKEUP 0x00
#else
#define SW4_WAKEUP SW_WAKEUP
#endif
	{SW4_GPIO,1<<SW4_BIT,SW4_MODE|SW4_POL|SW4_WAKEUP},
#if defined(SW4_EXTI_PORT)&&!defined(SW4_NWU)
					//SW4をEXTI実装するとき
#if SW4_BIT<4
#define SW4_EXTICR1	(SW4_EXTI_PORT<<SW4_BIT*8)
#define SW4_EXTICR2	0x00000000
#define SW4_EXTICR3	0x00000000
#define SW4_EXTICR4	0x00000000
#elif SW4_BIT<8
#define SW4_EXTICR1	0x00000000
#define SW4_EXTICR2	(SW4_EXTI_PORT<<(SW4_BIT-4)*8)
#define SW4_EXTICR3	0x00000000
#define SW4_EXTICR4	0x00000000
#elif SW4_BIT<12
#define SW4_EXTICR1	0x00000000
#define SW4_EXTICR2	0x00000000
#define SW4_EXTICR3	(SW4_EXTI_PORT<<(SW4_BIT-8)*8)
#define SW4_EXTICR4	0x00000000
#else
#define SW4_EXTICR1	0x00000000
#define SW4_EXTICR2	0x00000000
#define SW4_EXTICR3	0x00000000
#define SW4_EXTICR4	(SW4_EXTI_PORT<<(SW4_BIT-12)*8)
#endif
#define SW4_EXTIMSK	(1<<SW4_BIT)
#endif
#endif
#ifndef SW4_EXTIMSK			//SW4をEXTI実装しないとき
#define SW4_EXTICR1	0x00000000
#define SW4_EXTICR2	0x00000000
#define SW4_EXTICR3	0x00000000
#define SW4_EXTICR4	0x00000000
#define SW4_EXTIMSK	0x00000000
#endif

//SW5の構成-------------------------------------
#ifdef SW5_BIT				//SW5を実装するとき
#ifdef SW5_TIMER
#define SW5_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW5_REAL
#define SW5_MODE 0x00
#define SW_REAL
#endif
#ifdef SW5_INV
#define SW5_POL 0x00
#else
#define SW5_POL SW_POL
#endif
#ifdef SW5_NWU
#define SW5_WAKEUP 0x00
#else
#define SW5_WAKEUP SW_WAKEUP
#endif
	{SW5_GPIO,1<<SW5_BIT,SW5_MODE|SW5_POL|SW5_WAKEUP},
#if defined(SW5_EXTI_PORT)&&!defined(SW5_NWU)
					//SW5をEXTI実装するとき
#if SW5_BIT<4
#define SW5_EXTICR1	(SW5_EXTI_PORT<<SW5_BIT*8)
#define SW5_EXTICR2	0x00000000
#define SW5_EXTICR3	0x00000000
#define SW5_EXTICR4	0x00000000
#elif SW5_BIT<8
#define SW5_EXTICR1	0x00000000
#define SW5_EXTICR2	(SW5_EXTI_PORT<<(SW5_BIT-4)*8)
#define SW5_EXTICR3	0x00000000
#define SW5_EXTICR4	0x00000000
#elif SW5_BIT<12
#define SW5_EXTICR1	0x00000000
#define SW5_EXTICR2	0x00000000
#define SW5_EXTICR3	(SW5_EXTI_PORT<<(SW5_BIT-8)*8)
#define SW5_EXTICR4	0x00000000
#else
#define SW5_EXTICR1	0x00000000
#define SW5_EXTICR2	0x00000000
#define SW5_EXTICR3	0x00000000
#define SW5_EXTICR4	(SW5_EXTI_PORT<<(SW5_BIT-12)*8)
#endif
#define SW5_EXTIMSK	(1<<SW5_BIT)
#endif
#endif
#ifndef SW5_EXTIMSK			//SW5をEXTI実装しないとき
#define SW5_EXTICR1	0x00000000
#define SW5_EXTICR2	0x00000000
#define SW5_EXTICR3	0x00000000
#define SW5_EXTICR4	0x00000000
#define SW5_EXTIMSK	0x00000000
#endif

//SW6の構成-------------------------------------
#ifdef SW6_BIT				//SW6を実装するとき
#ifdef SW6_TIMER
#define SW6_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW6_REAL
#define SW6_MODE 0x00
#define SW_REAL
#endif
#ifdef SW6_INV
#define SW6_POL 0x00
#else
#define SW6_POL SW_POL
#endif
#ifdef SW6_NWU
#define SW6_WAKEUP 0x00
#else
#define SW6_WAKEUP SW_WAKEUP
#endif
	{SW6_GPIO,1<<SW6_BIT,SW6_MODE|SW6_POL|SW6_WAKEUP},
#if defined(SW6_EXTI_PORT)&&!defined(SW6_NWU)
					//SW6をEXTI実装するとき
#if SW6_BIT<4
#define SW6_EXTICR1	(SW6_EXTI_PORT<<SW6_BIT*8)
#define SW6_EXTICR2	0x00000000
#define SW6_EXTICR3	0x00000000
#define SW6_EXTICR4	0x00000000
#elif SW6_BIT<8
#define SW6_EXTICR1	0x00000000
#define SW6_EXTICR2	(SW6_EXTI_PORT<<(SW6_BIT-4)*8)
#define SW6_EXTICR3	0x00000000
#define SW6_EXTICR4	0x00000000
#elif SW6_BIT<12
#define SW6_EXTICR1	0x00000000
#define SW6_EXTICR2	0x00000000
#define SW6_EXTICR3	(SW6_EXTI_PORT<<(SW6_BIT-8)*8)
#define SW6_EXTICR4	0x00000000
#else
#define SW6_EXTICR1	0x00000000
#define SW6_EXTICR2	0x00000000
#define SW6_EXTICR3	0x00000000
#define SW6_EXTICR4	(SW6_EXTI_PORT<<(SW6_BIT-12)*8)
#endif
#define SW6_EXTIMSK	(1<<SW6_BIT)
#endif
#endif
#ifndef SW6_EXTIMSK			//SW6をEXTI実装しないとき
#define SW6_EXTICR1	0x00000000
#define SW6_EXTICR2	0x00000000
#define SW6_EXTICR3	0x00000000
#define SW6_EXTICR4	0x00000000
#define SW6_EXTIMSK	0x00000000
#endif

//SW7の構成-------------------------------------
#ifdef SW7_BIT				//SW7を実装するとき
#ifdef SW7_TIMER
#define SW7_MODE SW_MODE
#define SW_TIMER
#endif
#ifdef SW7_REAL
#define SW7_MODE 0x00
#define SW_REAL
#endif
#ifdef SW7_INV
#define SW7_POL 0x00
#else
#define SW7_POL SW_POL
#endif
#ifdef SW7_NWU
#define SW7_WAKEUP 0x00
#else
#define SW7_WAKEUP SW_WAKEUP
#endif
	{SW7_GPIO,1<<SW7_BIT,SW7_MODE|SW7_POL|SW7_WAKEUP},
#if defined(SW7_EXTI_PORT)&&!defined(SW7_NWU)
					//SW7をEXTI実装するとき
#if SW7_BIT<4
#define SW7_EXTICR1	(SW7_EXTI_PORT<<SW7_BIT*8)
#define SW7_EXTICR2	0x00000000
#define SW7_EXTICR3	0x00000000
#define SW7_EXTICR4	0x00000000
#elif SW7_BIT<8
#define SW7_EXTICR1	0x00000000
#define SW7_EXTICR2	(SW7_EXTI_PORT<<(SW7_BIT-4)*8)
#define SW7_EXTICR3	0x00000000
#define SW7_EXTICR4	0x00000000
#elif SW7_BIT<12
#define SW7_EXTICR1	0x00000000
#define SW7_EXTICR2	0x00000000
#define SW7_EXTICR3	(SW7_EXTI_PORT<<(SW7_BIT-8)*8)
#define SW7_EXTICR4	0x00000000
#else
#define SW7_EXTICR1	0x00000000
#define SW7_EXTICR2	0x00000000
#define SW7_EXTICR3	0x00000000
#define SW7_EXTICR4	(SW7_EXTI_PORT<<(SW7_BIT-12)*8)
#endif
#define SW7_EXTIMSK	(1<<SW7_BIT)
#endif
#endif
#ifndef SW7_EXTIMSK			//SW7をEXTI実装しないとき
#define SW7_EXTICR1	0x00000000
#define SW7_EXTICR2	0x00000000
#define SW7_EXTICR3	0x00000000
#define SW7_EXTICR4	0x00000000
#define SW7_EXTIMSK	0x00000000
#endif
};

//EXTICR設定値
#define SW_EXTICR1	(SW0_EXTICR1|SW1_EXTICR1|SW2_EXTICR1|SW3_EXTICR1|\
			SW4_EXTICR1|SW5_EXTICR1|SW6_EXTICR1|SW7_EXTICR1)
#define SW_EXTICR2	(SW0_EXTICR2|SW1_EXTICR2|SW2_EXTICR2|SW3_EXTICR2|\
			SW4_EXTICR2|SW5_EXTICR2|SW6_EXTICR2|SW7_EXTICR2)
#define SW_EXTICR3	(SW0_EXTICR3|SW1_EXTICR3|SW2_EXTICR3|SW3_EXTICR3|\
			SW4_EXTICR3|SW5_EXTICR3|SW6_EXTICR1|SW7_EXTICR3)
#define SW_EXTICR4	(SW0_EXTICR4|SW1_EXTICR4|SW2_EXTICR4|SW3_EXTICR4|\
			SW4_EXTICR4|SW5_EXTICR4|SW6_EXTICR4|SW7_EXTICR4)
#define SW_EXTIMSK	(SW0_EXTIMSK|SW1_EXTIMSK|SW2_EXTIMSK|SW3_EXTIMSK|\
			SW4_EXTIMSK|SW5_EXTIMSK|SW6_EXTIMSK|SW7_EXTIMSK)

//SWの実装数
#define	SW_SU	(sizeof(sw_cnf)/sizeof(sw_cnf[0]))


//==============================================
//SW操作の定義
//==============================================
static const unsigned char sw_cnt[]=		//ｱﾌﾟﾘ制御情報
{

//SW0の操作-------------------------------------
#ifdef SW0_BIT
	VOICES1_CNT,				//(Dr.W)
#endif

//SW1の操作-------------------------------------
#ifdef SW1_BIT
	VOICES2_CNT,				//(Dr.W)
#endif

//SW2の操作-------------------------------------
#ifdef SW2_BIT
	VOICES3_CNT,				//(Dr.W)
#endif

//SW3の操作-------------------------------------
#ifdef SW3_BIT
	VOICES4_CNT,				//(Dr.W)
#endif

//SW4の操作-------------------------------------
#ifdef SW4_BIT
	VOICES5_CNT,				//(Dr.W)
#endif

//SW5の操作-------------------------------------
#ifdef SW5_BIT
	SW5_CNT,
#endif

//SW6の操作-------------------------------------
#ifdef SW6_BIT
	SW6_CNT,
#endif

//SW7の操作-------------------------------------
#ifdef SW7_BIT
	SW7_CNT,
#endif
};

#include "func_SW.c"				//SW評価ﾓｼﾞｭｰﾙを呼込む
#endif


#ifdef CdS_EN				//CdSを実装するとき
//==============================================
//CdS構成の定義
//==============================================
#ifdef CdS_NWU
#define CdS_WAKEUP 0
#else
#define CdS_WAKEUP 1
#endif
#include "func_CdS.c"				//CdS評価ﾓｼﾞｭｰﾙを呼込む
#endif


#ifdef CVD_EN				//CVDを実装するとき
//==============================================
//CVD構成の定義
//==============================================
#define	CVD_MODE	0x80			//ﾓｰﾄﾞ設定ﾏｽｸ
#define	CVD_WAKEUP	0x40			//Wakeup設定ﾏｽｸ
static const struct CVD_CNF
{
	GPIO_TypeDef *gpio;			//GPIOｱﾄﾞﾚｽ
	unsigned short chsel;			//CHSELﾏｽｸ/ANIN番号
	unsigned char mode;			//b7:ﾓｰﾄﾞ(0=ﾘｱﾙﾓｰﾄﾞ､1=ﾀｲﾏｰﾓｰﾄﾞ)
						//b6:Wakeup(0=非対象､1=対象)
						//b3-b0:ﾋﾞｯﾄ番号
}cvd_cnf[]=
{

//CVD0の構成-------------------------------------
#ifdef CVD0_BIT
#ifdef CVD0_TIMER
#define CVD0_MODE CVD_MODE
#define CVD_TIMER
#endif
#ifdef CVD0_REAL
#define CVD0_MODE 0x00
#define CVD_REAL
#endif
#ifdef CVD0_NWU
#define CVD0_WAKEUP 0x00
#else
#define CVD0_WAKEUP CVD_WAKEUP
#endif
	{CVD0_GPIO,CVD0_CHSEL,CVD0_MODE|CVD0_WAKEUP|CVD0_BIT},
#endif

//CVD1の構成-------------------------------------
#ifdef CVD1_BIT
#ifdef CVD1_TIMER
#define CVD1_MODE CVD_MODE
#define CVD_TIMER
#endif
#ifdef CVD1_REAL
#define CVD1_MODE 0x00
#define CVD_REAL
#endif
#ifdef CVD1_NWU
#define CVD1_WAKEUP 0x00
#else
#define CVD1_WAKEUP CVD_WAKEUP
#endif
	{CVD1_GPIO,CVD1_CHSEL,CVD1_MODE|CVD1_WAKEUP|CVD1_BIT},
#endif

//CVD2の構成-------------------------------------
#ifdef CVD2_BIT
#ifdef CVD2_TIMER
#define CVD2_MODE CVD_MODE
#define CVD_TIMER
#endif
#ifdef CVD2_REAL
#define CVD2_MODE 0x00
#define CVD_REAL
#endif
#ifdef CVD2_NWU
#define CVD2_WAKEUP 0x00
#else
#define CVD2_WAKEUP CVD_WAKEUP
#endif
	{CVD2_GPIO,CVD2_CHSEL,CVD2_MODE|CVD2_WAKEUP|CVD2_BIT},
#endif

//CVD3の構成-------------------------------------
#ifdef CVD3_BIT
#ifdef CVD3_TIMER
#define CVD3_MODE CVD_MODE
#define CVD_TIMER
#endif
#ifdef CVD3_REAL
#define CVD3_MODE 0x00
#define CVD_REAL
#endif
#ifdef CVD3_NWU
#define CVD3_WAKEUP 0x00
#else
#define CVD3_WAKEUP CVD_WAKEUP
#endif
	{CVD3_GPIO,CVD3_CHSEL,CVD3_MODE|CVD3_WAKEUP|CVD3_BIT},
#endif

//CVD4の構成-------------------------------------
#ifdef CVD4_BIT
#ifdef CVD4_TIMER
#define CVD4_MODE CVD_MODE
#define CVD_TIMER
#endif
#ifdef CVD4_REAL
#define CVD4_MODE 0x00
#define CVD_REAL
#endif
#ifdef CVD4_NWU
#define CVD4_WAKEUP 0x00
#else
#define CVD4_WAKEUP CVD_WAKEUP
#endif
	{CVD4_GPIO,CVD4_CHSEL,CVD4_MODE|CVD4_WAKEUP|CVD4_BIT},
#endif

//CVD5の構成-------------------------------------
#ifdef CVD5_BIT
#ifdef CVD5_TIMER
#define CVD5_MODE CVD_MODE
#define CVD_TIMER
#endif
#ifdef CVD5_REAL
#define CVD5_MODE 0x00
#define CVD_REAL
#endif
#ifdef CVD5_NWU
#define CVD5_WAKEUP 0x00
#else
#define CVD5_WAKEUP CVD_WAKEUP
#endif
	{CVD5_GPIO,CVD5_CHSEL,CVD5_MODE|CVD5_WAKEUP|CVD5_BIT},
#endif
};

#define	CVD_SU	(sizeof(cvd_cnf)/sizeof(cvd_cnf[0]))
						//CVDの実装数


//==============================================
//CVD操作の定義
//==============================================
static const unsigned char cvd_cnt[]=		//ｱﾌﾟﾘ制御情報
{

//CVD0の操作-------------------------------------
#ifdef CVD0_BIT
	CVD0_CNT,
#endif

//CVD1の操作-------------------------------------
#ifdef CVD1_BIT
	CVD1_CNT,
#endif

//CVD2の操作-------------------------------------
#ifdef CVD2_BIT
	CVD2_CNT,
#endif

//CVD3の操作-------------------------------------
#ifdef CVD3_BIT
	CVD3_CNT,
#endif

//CVD4の操作-------------------------------------
#ifdef CVD4_BIT
	CVD4_CNT,
#endif

//CVD5の操作-------------------------------------
#ifdef CVD5_BIT
	CVD5_CNT,
#endif
};

#include "func_CVD.c"				//CVD評価ﾓｼﾞｭｰﾙを呼込む
#endif

#ifdef	LED_ENT		//音声番号に応じたﾊﾟﾀｰﾝで､LED点滅するとき	(Dr.W)
//==============================================
//LED点滅ｼｰｹﾝｽ		//固有処理のｺｰﾙﾊﾞｯｸ周期(32ms)で並べて置く
//==============================================
const unsigned char led_seq01[]=
{					//　SW0の音声番号のとき
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000010,
	0b00000010,
	0b00000010,
	0b00000010,
	0b00000010,
};
const unsigned char led_seq02[]=
{					//　SW1の音声番号のとき
	0b00000011,
	0b00000011,
	0b00000000,
	0b00000000,
};
const unsigned char led_seq03[]=
{					//　SW2の音声番号のとき
	0b00000011,
	0b00000011,
	0b00000011,
	0b00000011,
	0b00000011,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};
const unsigned char led_seq04[]=
{					//　SW3の音声番号のとき
	0b00000011,
};
#endif


//==============================================
//固有の作業域
//==============================================
#if defined(KYOKU_RAND)||defined(ONSEIN_RAND)||defined(ONSEII_RAND)||defined(ONSEIC_RAND)||defined(ONSEIS_RAND)	//どれかでﾗﾝﾀﾞﾑが必要なとき
static unsigned char rand_x;			//ﾗﾝﾀﾞﾏｲｽﾞの種
#endif
#if PART_N>0
static unsigned char kyoku;
#ifdef KYOKU_RAND
static unsigned char kyoku_rand_msk;
#endif
#endif
#if VOICE_N>0
static unsigned char onseiN;
#ifdef ONSEIN_RAND
static unsigned char onseiN_rand_msk;
#endif
#endif
#if VOICE_I>0
static unsigned char onseiI;
#ifdef ONSEII_RAND
static unsigned char onseiI_rand_msk;
#endif
#endif
#if VOICE_C>0
static unsigned char onseiC;
#ifdef ONSEIC_RAND
static unsigned char onseiC_rand_msk;
#endif
#endif
#if VOICE_S>0
static unsigned char onseiS;
#ifdef ONSEIS_RAND
static unsigned char onseiS_rand_msk;
#endif
#endif


#if defined(KYOKU_RAND)||defined(ONSEIN_RAND)||defined(ONSEII_RAND)||defined(ONSEIC_RAND)||defined(ONSEIS_RAND)	//どれかでﾗﾝﾀﾞﾑが必要なとき
//==============================================
//ﾗﾝﾀﾞﾏｲｽﾞのﾋﾞｯﾄﾏｽｸﾊﾟﾀｰﾝを計算する
//==============================================
static unsigned char rand_msk(
	unsigned char no)			//実装数
{
//DBG_C('\n');
//DBG_C('p');
//DBG_B(no);
	if(no>127) return(0xff);
	if(no>63) return(0x7f);
	if(no>31) return(0x3f);
	if(no>15) return(0x1f);
	if(no>7) return(0x0f);
	if(no>3) return(0x07);
	return(0x03);
}


//==============================================
//ﾗﾝﾀﾞﾏｲｽﾞ
//==============================================
static unsigned char rand(void)
{

//DBG_C('r');
	rand_x=(rand_x<<2)+rand_x+1;
//DBG_B(rand_x);
	return(rand_x);
}
#endif


//==============================================
//ｱﾌﾟﾘの初期設定
//==============================================
static void apl_init(void)
{

//DBG_C('\n');
//DBG_C('$');

	//ﾗﾝﾀﾞﾏｲｽﾞの種
#if defined(KYOKU_RAND)||defined(ONSEIN_RAND)||defined(ONSEII_RAND)||defined(ONSEIC_RAND)||defined(ONSEIS_RAND)	//どれかでﾗﾝﾀﾞﾑが必要なとき
 //DBG_C('r');
	ADC1->SMPR=ADC_SMPR_SMP_Msk;		//ｻﾝﾌﾟﾘﾝｸﾞ時間は最大値(239.5ADCｸﾛｯｸｻｲｸﾙ=17us)
	ADC1->CHSELR=(uint32_t)RAND_CHSEL;	//変換chを設定する
	ADC1->CR|=ADC_CR_ADEN;			//ADC有効
	ADC1->CR|=ADC_CR_ADSTART;		//変換開始
	while(!(ADC1->ISR&ADC_ISR_EOC));	//変換完了を待つ
	rand_x=ADC1->DR;			//変換結果の下位8ﾋﾞｯﾄを種にする
//DBG_C('\n');
//DBG_S(ADC1->DR);

	//ﾗﾝﾀﾞﾏｲｽﾞのﾋﾞｯﾄﾏｽｸﾊﾟﾀｰﾝ設定
#if (PART_N>0)&&defined(KYOKU_RAND)
	kyoku_rand_msk=rand_msk(SONG_IDX.no);
//DBG_B(kyoku_rand_msk);
#endif
#if (VOICE_N>0)&&defined(ONSEIN_RAND)
	onseiN_rand_msk=rand_msk(VOICEN_SU);
//DBG_B(onseiN_rand_msk);
#endif
#if (VOICE_I>0)&&defined(ONSEII_RAND)
	onseiI_rand_msk=rand_msk(VOICEI_SU);
//DBG_B(onseiI_rand_msk);
#endif
#if (VOICE_C>0)&&defined(ONSEIC_RAND)
	onseiC_rand_msk=rand_msk(VOICEC_SU);
//DBG_B(onseiC_rand_msk);
#endif
#if (VOICE_S>0)&&defined(ONSEIS_RAND)
	onseiS_rand_msk=rand_msk(VOICES_SU);
//DBG_B(onseiS_rand_msk);
#endif
#endif

	//LEDﾎﾟｰﾄ設定
#ifdef LED0_BIT				//LED0を実装するとき
	GPIO_OUT(LED0_GPIO,LED0_BIT);		//出力ﾓｰﾄﾞ
#endif
#ifdef LED1_BIT				//LED1を実装するとき
	GPIO_OUT(LED1_GPIO,LED1_BIT);		//出力ﾓｰﾄﾞ
#endif
#ifdef LED2_BIT				//LED2を実装するとき
	GPIO_OUT(LED2_GPIO,LED2_BIT);		//出力ﾓｰﾄﾞ
#endif
#ifdef LED3_BIT				//LED3を実装するとき
	GPIO_OUT(LED3_GPIO,LED3_BIT);		//出力ﾓｰﾄﾞ
#endif
#ifdef LED4_BIT				//LED4を実装するとき
	GPIO_OUT(LED4_GPIO,LED4_BIT);		//出力ﾓｰﾄﾞ
#endif
#ifdef LED5_BIT				//LED5を実装するとき
	GPIO_OUT(LED5_GPIO,LED5_BIT);		//出力ﾓｰﾄﾞ
#endif
#ifdef LED6_BIT				//LED6を実装するとき
	GPIO_OUT(LED6_GPIO,LED6_BIT);		//出力ﾓｰﾄﾞ
#endif

	//SWﾎﾟｰﾄ設定
//DBG_C('p');
#ifdef SW_EN				//SWを実装するとき
	RCC->APBENR2|=RCC_APBENR2_SYSCFGEN;
					//SYSCFGにｸﾛｯｸ供給
#ifdef SW0_BIT				//SW0を実装するとき
#ifdef SW0_INV				//負論理のとき
	GPIO_INPU(SW0_GPIO,SW0_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW0_GPIO,SW0_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW1_BIT				//SW1を実装するとき
#ifdef SW1_INV				//負論理のとき
	GPIO_INPU(SW1_GPIO,SW1_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW1_GPIO,SW1_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW2_BIT				//SW2を実装するとき
#ifdef SW2_INV				//負論理のとき
	GPIO_INPU(SW2_GPIO,SW2_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW2_GPIO,SW2_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW3_BIT				//SW3を実装するとき
#ifdef SW3_INV				//負論理のとき
	GPIO_INPU(SW3_GPIO,SW3_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW3_GPIO,SW3_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW4_BIT				//SW4を実装するとき
#ifdef SW4_INV				//負論理のとき
	GPIO_INPU(SW4_GPIO,SW4_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW4_GPIO,SW4_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW5_BIT				//SW5を実装するとき
#ifdef SW5_INV				//負論理のとき
	GPIO_INPU(SW5_GPIO,SW5_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW5_GPIO,SW5_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW6_BIT				//SW6を実装するとき
#ifdef SW6_INV				//負論理のとき
	GPIO_INPU(SW6_GPIO,SW6_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW6_GPIO,SW6_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#ifdef SW7_BIT				//SW7を実装するとき
#ifdef SW7_INV				//負論理のとき
	GPIO_INPU(SW7_GPIO,SW7_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟする
#else					//正論理のとき
	GPIO_INPD(SW7_GPIO,SW7_BIT);		//入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝする
#endif
#endif
#endif

#ifdef SLEEP_EN				//Sleepを実装するとき
	//Wakeup設定
#if defined(CdS_EN)||defined(CVD_EN)||defined(SW_PLNG)
					//CdSかCVDを実装するとき､
					//　SWをﾎﾟｰﾘﾝｸﾞするときは
					//　LPTIM割込みでWakeupする

	//LPTIM設定
	RCC->CSR|=RCC_CSR_LSION;		//LSIｵﾝ
	while(!(RCC->CSR&RCC_CSR_LSIRDY)){};	//LSIﾚﾃﾞｨを待つ
	RCC->CCIPR|=RCC_CCIPR_LPTIMSEL_0;	//LPTIMｸﾛｯｸはLSI
	RCC->APBENR1|=RCC_APBENR1_LPTIMEN;	//LPTIMにｸﾛｯｸ供給
	LPTIM->IER|=LPTIM_IER_ARRMIE;		//LPTIM割込み許可
	LPTIM->CR|=LPTIM_CR_ENABLE;		//LPTIM有効
	LPTIM->ARR=32*32;			//周期は32ms
//DBG_L(LPTIM->ARR);
	NVIC->ISER[0]=(1<<LPTIM1_IRQn);		//LPTIM割込み許可

#else					//CdSもCVDも実装しないとき､
					//　SWをﾎﾟｰﾘﾝｸﾞしないときは
					//　EXTI0～15(SW)割込みでWakeupする

//DBG_C('s');
#ifdef SW_EN				//SWを実装するとき

	//EXTI設定
#if SW_EXTICR1!=0x00000000
	EXTI->EXTICR[0]=SW_EXTICR1;
#endif
#if SW_EXTICR2!=0x00000000
	EXTI->EXTICR[1]=SW_EXTICR2;
#endif
#if SW_EXTICR3!=0x00000000
	EXTI->EXTICR[2]=SW_EXTICR3;
#endif
#if SW_EXTICR4!=0x00000000
	EXTI->EXTICR[3]=SW_EXTICR4;
#endif
	EXTI->RTSR=EXTI->FTSR=SW_EXTIMSK;

//DBG_C('\n');
//DBG_L(EXTI->FTSR);
//DBG_C('-');
//DBG_L(EXTI->RTSR);
	NVIC->ISER[0]=
		(1<<EXTI0_1_IRQn)|		//EXTI0_1割込み許可
		(1<<EXTI2_3_IRQn)|		//EXTI2_3割込み許可
		(1<<EXTI4_15_IRQn);		//EXTI4_15割込み許可
//DBG_C('-');
//DBG_L(NVIC->ISER[0]);
#endif
#endif
#endif
}


#ifdef	SLEEP_EN			//Sleep機能を実装するとき
#if defined(CdS_EN)||defined(CVD_EN)||defined(SW_PLNG)
					//CdSかCVDを実装するとき､
					//　SWをﾎﾟｰﾘﾝｸﾞするとき
//==============================================
//LPTIM割込み処理
//==============================================
void LPTIM1_IRQHandler(void)
{

//DBS_H;
	LPTIM->ICR|=LPTIM_ICR_ARRMCF;
//DBS_L;
}


#else					//CdSもCVDも実装しないとき､
					//　SWをﾎﾟｰﾘﾝｸﾞしないとき
//==============================================
//EXTI割込みﾊﾝﾄﾞﾗ
//==============================================
void EXTI0_1_IRQHandler(void)
{

	EXTI->PR=0x00000003;			//EXTI0-1割込みﾌﾗｸﾞをｸﾘｱする
}

void EXTI2_3_IRQHandler(void)
{

	EXTI->PR=0x0000000c;			//EXTI2-3割込みﾌﾗｸﾞをｸﾘｱする
}

void EXTI4_15_IRQHandler(void)
{

	EXTI->PR=0x0000fff0;			//EXTI4-115割込みﾌﾗｸﾞをｸﾘｱする
}
#endif


//==============================================
//Sleep処理
//==============================================
static void apl_sleep(void)
{
#if defined(CdS_EN)||defined(CVD_EN)||defined(SW_PLNG)
					//CdSかCVDを実装するとき､
					//　SWをﾎﾟｰﾘﾝｸﾞするとき
	unsigned char n;
#endif

//DBG_C('\n');
//DBG_C('<');

	//LED消灯
#ifdef LED0_BIT				//LED0を実装するとき
	GPIO_L(LED0_GPIO,LED0_BIT);		//消灯する
#endif
#ifdef LED1_BIT				//LED1を実装するとき
	GPIO_L(LED1_GPIO,LED1_BIT);		//消灯する
#endif
#ifdef LED2_BIT				//LED2を実装するとき
	GPIO_L(LED2_GPIO,LED2_BIT);		//消灯する
#endif
#ifdef LED3_BIT				//LED3を実装するとき
	GPIO_L(LED3_GPIO,LED3_BIT);		//消灯する
#endif
#ifdef LED4_BIT				//LED4を実装するとき
	GPIO_L(LED4_GPIO,LED4_BIT);		//消灯する
#endif
#ifdef LED5_BIT				//LED5を実装するとき
	GPIO_L(LED5,GPIO,LED5_BIT);		//消灯する
#endif
#ifdef LED6_BIT				//LED6を実装するとき
	GPIO_L(LED6_GPIO,LED6_BIT);		//消灯する
#endif

#if VOICE_S>0				//音声S再生を実装するとき
	//W25をﾊﾟﾜｰﾀﾞｳﾝ
//DBG_C('a');
	W25_PWR_DOWN();				//W25をﾊﾟﾜｰﾀﾞｳﾝする
#endif

	//SPIをﾘｾｯﾄ
#if VOICE_C+VOICE_S>0			//SPIを使うとき
//DBG_C('b');
	spi_reset();				//SPIﾎﾟｰﾄをﾘｾｯﾄする
#endif

	//PWMを停止
//DBG_C('c');
	dev_pwm_stop();				//PWMを停止する

	//Sleep処理
#if defined(CdS_EN)||defined(CVD_EN)||defined(SW_PLNG)
					//CdSかCVDを実装するとき､
					//　SWをﾎﾟｰﾘﾝｸﾞするときは
					//　LPTIM割込みでWakeupする
	for(;;)
	{
//DBG_C('d');
		//SLEEPDEEPﾓｰﾄﾞへ移行
//DBG_SYNC();
//DBS_H;
		//AWU設定
		LPTIM->CR|=LPTIM_CR_SNGSTRT;	//ｶｳﾝﾄ開始

		//低電力設定
		PWR->CR1|=PWR_CR1_LPR|PWR_CR1_VOS|PWR_CR1_MRRDY_TIME_Msk;
						//ﾚｷﾞｭﾚｰﾀを低電力､VDD=1.0V､MRRDY_TIMEは5us
		RCC->CFGR=0x00000000;		//SYSCLKはHSI
DBS_H;
		__DSB();			//ﾃﾞｰﾀ同期ﾊﾞﾘｱ
		__WFI();			//SLEEPDEEPﾓｰﾄﾞへ移行
		SystemInit();			//ｼｽﾃﾑｸﾛｯｸを再設定する
//DBS_L;
		wait_us(80);
//DBG_C('e');

#ifdef SW_EN				//SWを実装するとき
		//SWを評価する
//DBG_C('f');
		for(n=0;n<SW_SU;n++)
		{
			if(sw_read(n,1)&&(sw_cnf[n].mode&SW_WAKEUP)) break;
						//Wakeup対象で操作を検知したら抜ける
		}
		if(n<SW_SU) break;
#endif

#ifdef CdS_EN				//CdSを実装するとき
		//CdSを評価する
//DBG_C('g');
		if(cds_read(1)&&CdS_WAKEUP) break;
					//Wakeup対象で操作を検知したら抜ける
#endif

#ifdef CVD_EN				//CVDを実装するとき
		//CVDを評価する
//DBG_C('h');
		for(n=0;n<CVD_SU;n++)
		{
			if(cvd_read(n,1)&&(cvd_cnf[n].mode&CVD_WAKEUP)) break;
						//Wakeup対象で操作を検知したら抜ける
		}
		if(n<CVD_SU) break;
#endif
	}

#else					//CdSもCVDも実装しないとき､
					//　SWをﾎﾟｰﾘﾝｸﾞしないときは
					//　EXTI0～15(SW)割込みでWakeupする
//DBG_C('i');
//DBG_SYNC();

	//STOPﾓｰﾄﾞへ移行
	EXTI->IMR=SW_EXTIMSK;			//EXTI割込み許可

	//低電力設定
	PWR->CR1|=PWR_CR1_LPR|PWR_CR1_VOS|PWR_CR1_MRRDY_TIME_Msk;
						//ﾚｷﾞｭﾚｰﾀを低電力､VDD=1.0V､MRRDY_TIMEは5us
	RCC->CFGR=0x00000000;			//SYSCLKはHSI
	__DSB();				//ﾃﾞｰﾀ同期ﾊﾞﾘｱ
	__WFI();				//STOPﾓｰﾄﾞへ移行
	EXTI->IMR=0x00000000;			//EXTI割込み禁止
	SystemInit();				//ｼｽﾃﾑｸﾛｯｸを再設定する
//DBG_C('j');
#endif

	//PWMを再稼働
//DBG_C('k');
	dev_pwm_start();			//PWMを開始する

	//SPIをｾｯﾄ
#if VOICE_C+VOICE_S>0			//SPIを使うとき
//DBG_C('l');
	spi_set();				//SPIﾎﾟｰﾄをｾｯﾄする
#endif

	//SDを初期化
#if VOICE_C>0				//音声C再生を実装するとき
//DBG_C('m');
	mmsd_init();				//SDCを初期化する
#endif

	//W25をﾊﾟﾜｰｱｯﾌﾟ
#if VOICE_S>0				//音声S再生を実装するとき
//DBG_C('n');
	W25_PWR_UP();				//W25をﾊﾟﾜｰｱｯﾌﾟする
#endif
//DBG_C('>');
}
#endif


//==============================================
//固有の処理(ｺｰﾙﾊﾞｯｸ関数)
//==============================================
static void CB_KOYUU(void)
{
#ifdef SLEEP_EN				//Sleep機能を実装するとき
	static unsigned short sleep_tmr=0;	//Sleepﾀｲﾏｰ
#endif
	unsigned char res,cnt,n;

#ifdef	LED_ENT		//音声番号に応じたﾊﾟﾀｰﾝで､LED点滅するとき	(Dr.W)
	static unsigned char led_ent01;
	static unsigned char led_ent02;
	static unsigned char led_ent03;
	static unsigned char led_ent04;
	unsigned char led_n;
#endif

#if PART_N>0				//ｵﾙｺﾞｰﾙ演奏を実装するとき
	struct SONG_IDX *p;
#endif

	//稼働ﾁｪｯｸ
#ifdef SLEEP_EN				//Sleep機能を実装するとき
//DBG_C('e');
	res=0;					//仮に非稼働にしておく
#if PART_N>0				//ｵﾙｺﾞｰﾙ演奏を実装するとき
	res|=song.no;				//稼働しているときは稼働中の設定
#endif
#if VOICE_N>0				//音声N再生を実装するとき
	res|=voiceN[0].no;			//稼働しているときは稼働中の設定
#endif
#if VOICE_I>0				//音声I再生を実装するとき
	res|=voiceI.no;				//稼働しているときは稼働中の設定
#endif
#if VOICE_C>0				//音声C再生を実装するとき
	res|=voiceC.no;				//稼働しているときは稼働中の設定
#endif
#if VOICE_S>0				//音声S再生を実装するとき
	res|=voiceS[0].no;			//稼働しているときは稼働中の設定
#endif
//DBG_B(res);
	if(res)					//稼働しているときは
	{
		sleep_tmr=0;			//　Sleep時限をｸﾘｱする
	}
	else					//稼働していないときは
	{
//DBG_C('X');
//DBG_S(sleep_tmr);
		if(++sleep_tmr>SLEEP_TMR/KOYUU_PR)
						//　Sleep時限を超えたら
		{
			apl_sleep();		//　Sleepする
			sleep_tmr=0;		//　Sleep時限をｸﾘｱする
		}
	}
#endif

	//操作ﾁｪｯｸ
	res=0;					//仮に操作無しにしておく
#ifdef SW_EN				//SWを実装するとき
//DBG_C('s');
	if(!res)				//操作が無かったら
	{
#if VOICE_S>0				//音声Sを実装するとき
					//音声CはSPIを専有するのでﾘｾｯﾄ/ｾｯﾄできない
		spi_reset();			//SPIﾎﾟｰﾄをﾘｾｯﾄする
		wait_us(10);			//SWの電圧安定を待つ
#endif
//DBS_H;
		for(n=0;n<SW_SU;n++)		//SWの実装数まで繰返す
		{
			if((res=sw_read(n,0)))	//SW操作があったとき
			{
				cnt=sw_cnt[n];	//操作対象を得る
				break;		//抜ける
			}
		}
//DBS_L;
#if VOICE_S>0				//音声Sを実装するとき
					//音声CはSPIを専有するのでﾘｾｯﾄ/ｾｯﾄできない
		spi_set();			//SPIﾎﾟｰﾄをｾｯﾄする
#endif
	}
//DBG_B(res);
#endif
#ifdef CdS_EN				//CdSを実装するとき
//DBG_C('c');
	if(!res)				//操作が無かったら
	{
		if((res=cds_read(0)))		//CdS操作があったとき
		{
			cnt=CdS_CNT;		//操作対象を得る
		}
	}
//DBG_B(res);
#endif
#ifdef CVD_EN
//DBG_C('v');				//CVDを実装するとき
	if(!res)				//操作が無かったら
	{
		for(n=0;n<CVD_SU;n++)		//CVDの実装数まで繰返す
		{
			if((res=cvd_read(n,0)))	//CVD操作があったとき
			{
				cnt=cvd_cnt[n];	//操作対象を得る
				break;		//抜ける
			}
		}
	}
//DBG_B(res);
#endif

	//操作の実行
	if(res==1)	//このﾄﾚｲﾝ仕様ｱﾌﾟﾘでは､ｵﾝ変化検知のみで操作する (Dr.W)
	{
		VOICES_SEL(0,0);		//音声再生を初期化する
		if(cnt==3)		//操作が音声S4のとき(Dr.W)
		{			//音声S4以降を順に進める
			if(++onseiS>VOICES_SU) onseiS=4;
						//音声番号を進める
		}
		else			//操作が音声S1-3のとき(Dr.W)
		{			//音声番号に操作対象を代入する
			onseiS=cnt;
		}
	}			
//DBG_B(onseiS);
		VOICES_SEL(0,onseiS);	//再生開始する

	//LED点滅
#if PART_N>0				//ｵﾙｺﾞｰﾙ演奏を実装するとき
	if(song.no)				//演奏中のとき
	{
#ifdef LED0_BIT				//LED0を実装するとき
		if(part[0].onpu_cd&0x0001) GPIO_H(LED0_GPIO,LED0_BIT);
		else GPIO_L(LED0_GPIO,LED0_BIT);
#endif
#ifdef LED1_BIT				//LED1を実装するとき
		if(part[0].onpu_cd&0x0002) GPIO_H(LED1_GPIO,LED1_BIT);
		else GPIO_L(LED1_GPIO,LED1_BIT);
#endif
#ifdef LED2_BIT				//LED2を実装するとき
		if(part[0].onpu_cd&0x0004) GPIO_H(LED2_GPIO,LED2_BIT);
		else GPIO_L(LED2_GPIO,LED2_BIT);
#endif
#ifdef LED3_BIT				//LED3を実装するとき
		if(part[0].onpu_cd&0x0008) GPIO_H(LED3_GPIO,LED3_BIT);
		else GPIO_L(LED3_GPIO,LED3_BIT);
#endif
#ifdef LED4_BIT				//LED4を実装するとき
		if(part[0].onpu_cd&0x0010) GPIO_H(LED4_GPIO,LED4_BIT);
		else GPIO_L(LED4_GPIO,LED4_BIT);
#endif
#ifdef LED5_BIT				//LED5を実装するとき
#if PART_N==1				//1ﾊﾟｰﾄのみ実装するとき
		if(part[0].onpu_cd&0x0020) GPIO_H(LED5_GPIO,LED5_BIT);
		else GPIO_L(LED5_GPIO,LED5_BIT);
#else					//2ﾊﾟｰﾄ以上実装するとき
		if(part[1].onpu_cd&0x0001) GPIO_H(LED5_GPIO,LED5_BIT);
		else GPIO_L(LED5_GPIO,LED5_BIT);
#endif
#endif
#ifdef LED6_BIT				//LED6を実装するとき
#if PART_N==1				//1ﾊﾟｰﾄのみ実装するとき
		if(part[0].onpu_cd&0x0040) GPIO_H(LED6_GPIO,LED6_BIT);
		else GPIO_L(LED6_GPIO,LED6_BIT);
#else					//2ﾊﾟｰﾄ以上実装するとき
		if(part[1].onpu_cd&0x0002) GPIO_H(LED6_GPIO,LED6_BIT);
		else GPIO_L(LED6_GPIO,LED6_BIT);
#endif
#endif
	}
#endif

#ifdef	LED_ENT		//音声番号に応じたﾊﾟﾀｰﾝで､LED点滅するとき	(Dr.W)
#if VOICE_S>0				//音声S再生を実装するとき
	if(voiceS[0].no)		//音声再生中のとき
	{
DBG_C('\n');
		if(cnt&VOICES1_CNT)		//音声S1のとき
		{
DBG_B(led_ent01);
			led_n=led_seq01[led_ent01];
DBG_B(led_n);
#ifdef LED0_BIT				//LED0を実装するとき
				if(led_n&0x0001) GPIO_H(LED0_GPIO,LED0_BIT);
				else GPIO_L(LED0_GPIO,LED0_BIT);
#endif
#ifdef LED1_BIT				//LED1を実装するとき
				if(led_n&0x0002) GPIO_H(LED1_GPIO,LED1_BIT);
				else GPIO_L(LED1_GPIO,LED1_BIT);
#endif
			if(++led_ent01==sizeof(led_seq01)) led_ent01=0;
		}
		if(cnt&VOICES2_CNT)		//音声S2のとき
		{
DBG_B(led_ent02);
			led_n=led_seq02[led_ent02];
DBG_B(led_n);
#ifdef LED0_BIT				//LED0を実装するとき
				if(led_n&0x0001) GPIO_H(LED0_GPIO,LED0_BIT);
				else GPIO_L(LED0_GPIO,LED0_BIT);
#endif
#ifdef LED1_BIT				//LED1を実装するとき
				if(led_n&0x0002) GPIO_H(LED1_GPIO,LED1_BIT);
				else GPIO_L(LED1_GPIO,LED1_BIT);
#endif
			if(++led_ent02==sizeof(led_seq02)) led_ent02=0;
		}
		if(cnt&VOICES3_CNT)		//音声S3のとき
		{
DBG_B(led_ent03);
			led_n=led_seq03[led_ent03];
DBG_B(led_n);
#ifdef LED0_BIT				//LED0を実装するとき
				if(led_n&0x0001) GPIO_H(LED0_GPIO,LED0_BIT);
				else GPIO_L(LED0_GPIO,LED0_BIT);
#endif
#ifdef LED1_BIT				//LED1を実装するとき
				if(led_n&0x0002) GPIO_H(LED1_GPIO,LED1_BIT);
				else GPIO_L(LED1_GPIO,LED1_BIT);
#endif
			if(++led_ent03==sizeof(led_seq03)) led_ent03=0;
		}
		if(cnt&VOICES4_CNT)		//音声S4以上のとき
		{
DBG_B(led_ent04);
			led_n=led_seq04[led_ent04];
DBG_B(led_n);
#ifdef LED0_BIT				//LED0を実装するとき
				if(led_n&0x0001) GPIO_H(LED0_GPIO,LED0_BIT);
				else GPIO_L(LED0_GPIO,LED0_BIT);
#endif
#ifdef LED1_BIT				//LED1を実装するとき
				if(led_n&0x0002) GPIO_H(LED1_GPIO,LED1_BIT);
				else GPIO_L(LED1_GPIO,LED1_BIT);
#endif
			if(++led_ent04==sizeof(led_seq04)) led_ent04=0;
		}
	}
#endif
#endif

}


#if	PART_N>0			//ｵﾙｺﾞｰﾙ演奏を実装するとき
//==============================================
//演奏終了時の処理(ｺｰﾙﾊﾞｯｸ関数)
//==============================================
static void CB_SONG_END(void)
{
}
#endif


#if	VOICE_N>0			//音声N再生を実装するとき
//==============================================
//音声0再生終了時の処理(ｺｰﾙﾊﾞｯｸ関数)
//==============================================
static void CB_VOICEN_END(
	unsigned char ch)			//音声Nのch番号
{
}
#endif


#if	VOICE_I>0			//音声I再生を実装するとき
//==============================================
//音声I再生終了時の処理(ｺｰﾙﾊﾞｯｸ関数)
//==============================================
static void CB_VOICEI_END(void)
{
}
#endif


#if	VOICE_C>0			//音声C再生を実装するとき
//==============================================
//音声C再生終了時の処理(ｺｰﾙﾊﾞｯｸ関数)
//==============================================
static void CB_VOICEC_END(void)
{
}
#endif


#if	VOICE_S>0			//音声S再生を実装するとき
//==============================================
//音声S再生終了時の処理(ｺｰﾙﾊﾞｯｸ関数)
//==============================================
static void CB_VOICES_END(
	unsigned char ch)			//音声Sのch番号
{
}
#endif
