//==============================================
//ﾎﾟｰﾄｱｸｾｽのﾏｸﾛ定義
//==============================================
#define	GPIO_OUT(gpio,bit)	gpio->MODER=gpio->MODER&~(3<<bit*2)|1<<bit*2
#define	GPIO_OUTOD(gpio,bit)	gpio->OTYPER|=1<<bit,\
				gpio->MODER=gpio->MODER&~(3<<bit*2)|1<<bit*2
#define	GPIO_H(gpio,bit)	gpio->BSRR=1<<bit
#define	GPIO_L(gpio,bit)	gpio->BRR=1<<bit
#define	GPIO_R(gpio,bit)	(gpio->IDR&1<<bit)
#define	GPIO_IN(gpio,bit)	gpio->MODER&=~(3<<bit*2),\
				gpio->PUPDR&=~(3<<bit*2)
#define	GPIO_PU(gpio,bit)	gpio->PUPDR=gpio->PUPDR&~(3<<bit*2)|1<<bit*2
#define	GPIO_PUX(gpio,bit)	gpio->PUPDR&=~(3<<bit*2)
#define	GPIO_INPU(gpio,bit)	gpio->MODER&=~(3<<bit*2),\
				gpio->PUPDR=gpio->PUPDR&~(3<<bit*2)|1<<bit*2
#define	GPIO_INPD(gpio,bit)	gpio->MODER&=~(3<<bit*2),\
				gpio->PUPDR=gpio->PUPDR&~(3<<bit*2)|2<<bit*2
#define	GPIO_AF(gpio,bit,af)	gpio->MODER=gpio->MODER&~(3<<bit*2)|2<<bit*2,\
				gpio->AFR[bit>>3]|=af<<(bit&7)*4


//==============================================
//ｼｽﾃﾑｸﾛｯｸ設定
//==============================================
void SystemInit(void)
{

#if SYS_CLK>24				//ｼｽﾃﾑｸﾛｯｸが24MHz超えのとき
	FLASH->ACR|=FLASH_ACR_LATENCY;		//ﾌﾗｯｼｭﾚｲﾃﾝｼは1wait
#endif
	RCC->ICSCR=RCC->ICSCR&0xffff0000|*(uint32_t *)(0x1fff0f10);
						//HSIは24MHzの工場校正値
//	RCC->ICSCR=RCC->ICSCR&0xffff0000|*(uint32_t *)(0x1fff0f04);
						//HSIは8MHzの工場校正値
	RCC->CR|=(1<<24);;			//PLLｵﾝ
	while(!(RCC->CR&(1<<25))){};		//PLLﾚﾃﾞｨを待つ
	RCC->CFGR|=0x00000002;			//SYSCLKはPLL
	while((RCC->CFGR&0x00000038)!=0x00000010){};
						//PLLに切替るまで待つ
}


//==============================================
//共通ｺｰﾄﾞの呼込み
//==============================================
#include "common.c"				//共通ｺｰﾄﾞを呼込む

#ifdef	ISP_SPIFLASH_EN			//ISP環境でSPIﾌﾗｯｼｭを書替えるとき
#include "dev_UART.c"				//UARTを呼込む
#undef	VOICE_S					//SPI音声を
#define	VOICE_S	1				//　使用する設定にする
#include "dev_SPI.c"				//SPIを呼込む

#else					//電子ｵﾙｺﾞｰﾙを実行するとき
#if defined(UART_TX)||defined(UART_RX)	//UARTを実装するとき
#include "dev_UART.c"				//UARTを呼込む
#endif
#if VOICE_I>0				//音声Iを使うとき
#include "dev_I2C.c"				//I2Cを呼込む
#endif
#if VOICE_C+VOICE_S>0			//音声Cか音声Sを使うとき
#include "dev_SPI.c"				//SPIを呼込む
#endif
#include "orgel.c"				//orgelｴﾝｼﾞﾝを呼込む
#include "dev_PWM.c"				//PWMを呼込む
#endif


//==============================================
//固有の初期設定
//==============================================
static void CB_INIT(void)
{

	//ﾀﾞｳﾝﾛｰﾄﾞ猶予時間
	wait_ms(1000);				//絶対に取るな!

	//低電力設定
	RCC->APBENR1|=RCC_APBENR1_PWREN;	//電源ｲﾝﾀﾌｪｰｽにｸﾛｯｸ供給
	SCB->SCR|=SCB_SCR_SLEEPDEEP_Msk;	//SLEEPDEEP設定

	//GPIO設定
	RCC->IOPENR|=
		RCC_IOPENR_GPIOAEN|		//GPIOA､GPIOB､GPIOFにｸﾛｯｸ供給
		RCC_IOPENR_GPIOBEN|
		RCC_IOPENR_GPIOFEN;

	//PA1ﾁｪｯｸ
	GPIO_INPD(GPIOA,1);			//PA1をﾌﾟﾙﾀﾞｳﾝ入力ﾓｰﾄﾞ
	wait_ms(1);				//電圧安定時間
	if(GPIO_R(GPIOA,1))			//PA1がHだったら
	{
		wait_ms(2000);			//2秒待つ(この間にﾃﾞﾊﾞｶﾞを繋ぐ)
	}

	//GPIO設定の続き
	GPIOA->MODER=GPIOF->MODER=0xffffffff;	//全ﾎﾟｰﾄをｱﾅﾛｸﾞﾓｰﾄﾞにしておく
	GPIOA->PUPDR=GPIOF->PUPDR=0x00000000;	//全ﾎﾟｰﾄのﾌﾟﾙｱｯﾌﾟ/ﾀﾞｳﾝを無効にしておく

	//ﾃﾞﾊﾞｸﾞ信号設定
#ifdef SHDBS				//ﾃﾞﾊﾞｸﾞ信号を出力するとき
	dbs_init();				//ﾃﾞﾊﾞｸﾞ情報を初期化する
//待ち時間のﾃｽﾄ
//for(;;){wait_1us();DBS_H;wait_1us();DBS_L;}
//for(;;){wait_us(10);DBS_H;wait_us(10);DBS_L;}
#endif

	//ﾃﾞﾊﾞｸﾞ情報設定
#if defined(SHDBG)||defined(SHDBG_IN)	//ﾃﾞﾊﾞｸﾞ情報出力か入力するとき
	dbg_init();				//ﾃﾞﾊﾞｸﾞ情報を初期化する
#ifdef SHDBG				//ﾃﾞﾊﾞｸﾞ情報を出力するとき
//ﾃﾞﾊﾞｸﾞ情報出力のﾃｽﾄ
//for(;;){DBG_C('U');}
#endif
#endif

//DBG_C('\n');
//DBG_C('i');

	//周辺機能設定
#ifdef	ISP_SPIFLASH_EN			//ISP環境でSPIﾌﾗｯｼｭを書替えるとき
	uart_init();				//UART初期設定
	spi_init();				//SPI初期設定

#else					//電子ｵﾙｺﾞｰﾙを実行するとき
#if defined(UART_TX)||defined(UART_RX)	//UARTを使うとき
	uart_init();				//UART初期設定
#endif
#if VOICE_I>0				//I2Cを使うとき
	i2c_init();				//I2C初期設定
#endif
#if VOICE_C+VOICE_S>0			//SPIを使うとき
	spi_init();				//SPI初期設定
#endif
	pwm_init();				//PWM初期設定

	//ADCの実装要否
#if defined(CdS_EN)||defined(CVD_EN)	//CdSかCVDを実装するとき
#define	ADC_EN					//ADCの実装が必要
#endif
#if defined(KYOKU_RAND)||defined(ONSEIN_RAND)||defined(ONSEII_RAND)||defined(ONSEIC_RAND)||defined(ONSEIS_RAND)
					//どれかでﾗﾝﾀﾞﾑが必要なとき
#define	ADC_EN					//ADCを実装が必要
#endif

#ifdef ADC_EN				//ADCの実装が必要なとき
	//ADC設定(ADC1)
	RCC->APBENR2|=RCC_APBENR2_ADCEN;	//ADCにｸﾛｯｸ供給
	//ADC1->CFGR1(ﾘｾｯﾄ値)			//ｼﾝｸﾞﾙﾓｰﾄﾞ､結果右詰､12ﾋﾞｯﾄ分解能
	ADC1->CFGR2=0x80000000;			//ADCｸﾛｯｸはHSI=24MHz
	ADC->CCR|=ADC_CCR_VREFEN;		//Vref有効
//DBG_C('a');
#endif
#endif

	//ｱﾌﾟﾘの初期設定(LED設定はｱﾌﾟﾘの初期化で実施)
//DBG_C('p');
	apl_init();

//DBG_C('j');
}
