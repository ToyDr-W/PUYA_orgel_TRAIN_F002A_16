//==============================================
//大域変数
//==============================================
#if VOICE_S==1				//単一ch実装のとき
static unsigned char spi_addr;			//SPIｱﾄﾞﾚｽ設定(0=未､1=済)
#endif


//==============================================
//SPIｾﾚｸﾄ
//==============================================
#define	spi_sel()	GPIO_L(SPI_SS_GPIO,SPI_SS_BIT)
						//SSをL


//==============================================
//SPIﾃﾞｨｾﾚｸﾄ
//==============================================
#ifdef SHSPI2				//2線SPIのとき
#define	spi_dis()	GPIO_H(SPI_SS_GPIO,SPI_SS_BIT),wait_us(SPI_SS_TIME)
						//SSをHにして､SSのﾈｹﾞｰﾄ時間を待つ
						//直前のSCK=HによりSSはHになる
#else					//4線SPI､3線SPIのとき
#define	spi_dis()	GPIO_H(SPI_SS_GPIO,SPI_SS_BIT)
						//SSをH
#endif


#ifdef SPI_SOFT				//SPIをｿﾌﾄ実装するとき
#if VOICE_C>0				//SDを使うとき
//==============================================
//SPI送受信
//SDで使用する
//==============================================
static unsigned char spi_trans(			//受信したﾃﾞｰﾀを返す
	unsigned char data)			//送信するﾃﾞｰﾀ
{
	unsigned char c=8; 
	unsigned short s;

//DBG_C('<');
//DBG_B(data);

	s=data;					//送信するﾃﾞｰﾀを取り込む
	while(c--)
	{
		s<<=1;				//ﾃﾞｰﾀを左ｼﾌﾄしておく
		if(GPIO_R(SPI_MISO_GPIO,SPI_MISO_BIT)) s|=1;
						//MISOをﾃﾞｰﾀに反映する
		if(s&0x0100) GPIO_H(SPI_MOSI_GPIO,SPI_MOSI_BIT);
						//MSBからMOSIに
		else GPIO_L(SPI_MOSI_GPIO,SPI_MOSI_BIT);
						//　反映する
#if defined(SHSPI3)||defined(SHSPI2)	//3線SPIか2線SPIのとき
		GPIO_OUT(SPI_MOSI_GPIO,SPI_MOSI_BIT);
						//MOSIを出力ﾓｰﾄﾞ
#endif
#ifdef SHSPI2				//2線SPIのとき
		__disable_irq();		//割込み禁止
#endif
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをH
		GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをL
#ifdef SHSPI2				//2線SPIのとき
		__enable_irq();			//割込み許可
#endif
#if defined(SHSPI3)||defined(SHSPI2)	//3線SPIか2線SPIのとき
#ifdef SPI_MISO_PU			//MISOをﾌﾟﾙｱｯﾌﾟするとき
		GPIO_INPU(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟ
#endif
#ifdef SPI_MISO_PD			//MISOをﾌﾟﾙﾀﾞｳﾝするとき
		GPIO_INPD(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝ
#endif
#if !defined(SPI_MISO_PU)&&!defined(SPI_MISO_PD)
					//ﾌﾟﾙｱｯﾌﾟﾀﾞｳﾝが無指定のとき
		GPIO_IN(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞ
#endif
#endif
	}

//DBG_S(s);
//DBG_C('>');

	return((unsigned char)s);		//受信ﾃﾞｰﾀを返す
}
#endif


#if VOICE_S>0				//SPIﾒﾓﾘを使うとき
//==============================================
//SCK扱いのﾏｸﾛ置換え
//==============================================
#ifdef SHSPI2 				//2線SPIのとき
#define spi_send(x,y) spi_send_(x,y)		//最終ﾋﾞｯﾄのSCKの扱いを有効にする
#else					//3線SPIまたは4線SPIのとき
#define spi_send(x,y) spi_send_(x)		//最終ﾋﾞｯﾄのSCKの扱いを無視する
#endif


//==============================================
//SPI半二重送信
//SPIﾒﾓﾘで使用する
//受信ﾃﾞｰﾀは取込まない
//最初にMOSIを出力ﾓｰﾄﾞに設定する
//最後のSCKをLにする直前にMISOを入力ﾓｰﾄﾞに設定する(W25QのDIとDOを直結可能)
//2線SPIでSCK扱いが指定されているときは最終のSCKをHに保持する(ﾊﾟﾜｰﾀﾞｳﾝ設定の対応)
//==============================================
static void spi_send_(
#ifdef SHSPI2 				//2線SPIのとき
	unsigned char data,			//送信するﾃﾞｰﾀ
	unsigned char sck_h)			//2線SPIで最終ﾋﾞｯﾄのSCKの扱い
						//　0=Lにする､1=Hを保持する
#else					//3線SPIまたは4線SPIのとき
	unsigned char data)			//送信するﾃﾞｰﾀ
#endif
{
	unsigned char c=8;

//DBS_H;
//DBG_C('+');
//DBG_B(data);
#if defined(SHSPI3)||defined(SHSPI2)	//2線SPIまたは3線SPIのとき
		GPIO_OUT(SPI_MOSI_GPIO,SPI_MOSI_BIT);
						//MOSIを出力ﾓｰﾄﾞ
#endif
	while(c--)
	{
		if(data&0x80) GPIO_H(SPI_MOSI_GPIO,SPI_MOSI_BIT);
						//ﾃﾞｰﾀﾋﾞｯﾄを
		else GPIO_L(SPI_MOSI_GPIO,SPI_MOSI_BIT);
						//　MOSIに乗せる
		data<<=1;			//ﾃﾞｰﾀを次に進める
#if !defined(SHSPI3)&&!defined(SHSPI2)	//4線SPIのとき
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをH
		GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをL
#endif
#ifdef SHSPI3				//3線SPIのとき
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをH
		if(!c)				//最終ﾋﾞｯﾄのとき
		{
#ifdef SPI_MISO_PU			//MISOをﾌﾟﾙｱｯﾌﾟするとき
			GPIO_INPU(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟ
#endif
#ifdef SPI_MISO_PD			//MISOをﾌﾟﾙﾀﾞｳﾝするとき
			GPIO_INPD(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝ
#endif
#if !defined(SPI_MISO_PU)&&!defined(SPI_MISO_PD)
					//ﾌﾟﾙｱｯﾌﾟﾀﾞｳﾝが無指定のとき
			GPIO_IN(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞ
#endif
		}
		GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをL
#endif
#ifdef SHSPI2 				//2線SPIのとき
		__disable_irq();		//割込み禁止
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをH
		if(!c)				//最終ﾋﾞｯﾄのとき
		{
#ifdef SPI_MISO_PU			//MISOをﾌﾟﾙｱｯﾌﾟするとき
			GPIO_INPU(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞﾌﾟﾙｱｯﾌﾟ
#endif
#ifdef SPI_MISO_PD			//MISOをﾌﾟﾙﾀﾞｳﾝするとき
			GPIO_INPD(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞﾌﾟﾙﾀﾞｳﾝ
#endif
#if !defined(SPI_MISO_PU)&&!defined(SPI_MISO_PD)
					//ﾌﾟﾙｱｯﾌﾟﾀﾞｳﾝが無指定のとき
			GPIO_IN(SPI_MISO_GPIO,SPI_MISO_BIT);
						//MISOを入力ﾓｰﾄﾞ
#endif
			if(!sck_h) GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//2線SPIで最終ﾋﾞｯﾄのSCKの扱いが
						//1のときはSCKをLにしない
		}
		else			//最終ﾋﾞｯﾄ以外は
		{
			GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをL
		}
		__enable_irq();			//割込み許可
#endif
	};
//DBS_L;
}


//==============================================
//SPI半二重受信
//SPIﾒﾓﾘで使用する
//MISOのﾓｰﾄﾞ設定は行わない(入力ﾓｰﾄﾞになっていることが前提)
//MOSIは駆動しない
//==============================================
static unsigned char spi_recv(void)		//受信したﾃﾞｰﾀを返す
{
	unsigned char c=8;
	volatile unsigned char data;

//DBS_H;
//DBG_C('-');
	while(c--)
	{
		data<<=1;			//ﾃﾞｰﾀを次に進める
		if(GPIO_R(SPI_MISO_GPIO,SPI_MISO_BIT)) data|=1;
						//MISOをﾃﾞｰﾀに取込む
#ifdef SHSPI2 				//2線SPIのとき
		__disable_irq();		//割込み禁止
#endif
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
		GPIO_H(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをH
		GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをL
#ifdef SHSPI2 				//2線SPIのとき
		__enable_irq();			//割込み許可
#endif
	};
//DBG_B(data);
//DBS_L;
	return(data);				//受信したﾃﾞｰﾀを返す
}
#endif


#else					//内蔵SPIﾓｼﾞｭｰﾙを使うとき
//==============================================
//内蔵SPIﾓｼﾞｭｰﾙを使うときのﾏｸﾛ置換え
//==============================================
#define spi_send(x,y) spi_trans(x)		//最終ﾋﾞｯﾄのSCK扱いを無視する
#define spi_recv() spi_trans(0)			//ﾀﾞﾐｰの送信ﾃﾞｰﾀを付加する


//==============================================
//SPI送受信
//==============================================
static unsigned char spi_trans(			//受信したﾃﾞｰﾀを返す
	unsigned char data)			//送信するﾃﾞｰﾀ
{

//DBS_H;
//DBG_C('(');
//DBG_B(data);
	*(__IO uint8_t *)&(SPI1->DR)=data;	//送信ﾊﾞｯﾌｧに1ﾊﾞｲﾄを送る
						//　1ﾊﾞｲﾄ巾でｱｸｾｽすることが重要
	while(!(SPI1->SR&SPI_SR_RXNE)){};	//受信完了を待つ
	data=SPI1->DR;				//受信したﾃﾞｰﾀを取込む
//DBG_B(data);
//DBG_C(')');
//DBS_L;
	return(data);				//受信したﾃﾞｰﾀを返す
}
#endif


//==============================================
//SPIﾎﾟｰﾄをｾｯﾄする
//==============================================
static void spi_set(void)
{

	spi_dis();				//ﾃﾞｨｾﾚｸﾄ
	GPIO_OUT(SPI_SS_GPIO,SPI_SS_BIT);	//SSを出力ﾓｰﾄﾞ
#ifdef SPI_SOFT				//SPIをｿﾌﾄ実装するとき
#ifndef SHSPI2				//2線SPIでないとき
	GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);	//SCKをL
	GPIO_OUT(SPI_SCK_GPIO,SPI_SCK_BIT);	//SCKを出力ﾓｰﾄﾞ
#endif
#if !defined(SHSPI3)&&!defined(SHSPI2)	//4線SPIのとき
	GPIO_L(SPI_MOSI_GPIO,SPI_MOSI_BIT);	//MOSIをL
	GPIO_OUT(SPI_MOSI_GPIO,SPI_MOSI_BIT);	//MOSIを出力ﾓｰﾄﾞ
#endif
#ifdef SPI_MISO_PU			//MISOをﾌﾟﾙｱｯﾌﾟするとき
	GPIO_INPU(SPI_MISO_GPIO,SPI_MISO_BIT);	//MISOをﾌﾟﾙｱｯﾌﾟ入力
#endif
#ifdef SPI_MISO_PD			//MISOをﾌﾟﾙﾀﾞｳﾝするとき
	GPIO_INPD(SPI_MISO_GPIO,SPI_MISO_BIT);	//MISOをﾌﾟﾙﾀﾞｳﾝ入力
#endif
#if !defined(SPI_MISO_PU)&&!defined(SPI_MISO_PD)
					//MISOのﾌﾟﾙﾀﾞｳﾝ/ﾀﾞｳﾝが無指定のとき
	GPIO_IN(SPI_MISO_GPIO,SPI_MISO_BIT);	//MISOを入力
#endif
#else					//内蔵SPIﾓｼﾞｭｰﾙを使うとき
	GPIO_AF(SPI_SCK_GPIO,SPI_SCK_BIT,SPI_SCK_AF);
						//SCKを交代機能に設定
	GPIO_AF(SPI_MOSI_GPIO,SPI_MOSI_BIT,SPI_MOSI_AF);
						//MOSIを交代機能に設定
	GPIO_AF(SPI_MISO_GPIO,SPI_MISO_BIT,SPI_MISO_AF);
						//MISOを交代機能に設定
#endif
#if VOICE_S==1				//単一ch実装のとき
	spi_addr=0;				//SPIｱﾄﾞﾚｽ設定未にする
#endif
}


#if VOICE_C+VOICE_S>0&&(defined(SLEEP_EN)||defined(SW_EN))
//==============================================
//SPIﾎﾟｰﾄをﾘｾｯﾄする
//==============================================
static void spi_reset(void)
{

	spi_dis();				//ﾃﾞｨｾﾚｸﾄ
#ifndef SHSPI2				//2線SPIでないとき
#ifdef SPI_SCK_PU			//SCKをﾌﾟﾙｱｯﾌﾟするとき
	GPIO_INPU(SPI_SCK_GPIO,SPI_SCK_BIT);	//SCKをﾌﾟﾙｱｯﾌﾟ入力
#endif
#ifdef SPI_SCK_PD			//SCKをﾌﾟﾙﾀﾞｳﾝするとき
	GPIO_INPD(SPI_SCK_GPIO,SPI_SCK_BIT);	//SCKをﾌﾟﾙﾀﾞｳﾝ入力
#endif
#if !defined(SPI_SCK_PU)&&!defined(SPI_SCK_PD)
					//SCKのﾌﾟﾙﾀﾞｳﾝ/ﾀﾞｳﾝが無指定のとき
	GPIO_IN(SPI_SCK_GPIO,SPI_SCK_BIT);	//SCKを入力
#endif
#ifdef SPI_MOSI_PU			//MOSIをﾌﾟﾙｱｯﾌﾟするとき
	GPIO_INPU(SPI_MOSI_GPIO,SPI_MOSI_BIT);	//MOSIをﾌﾟﾙｱｯﾌﾟ入力
#endif
#ifdef SPI_MOSI_PD			//MOSIをﾌﾟﾙﾀﾞｳﾝするとき
	GPIO_INPD(SPI_MOSI_GPIO,SPI_MOSI_BIT);	//MOSIをﾌﾟﾙﾀﾞｳﾝ入力
#endif
#if !defined(SPI_MOSI_PU)&&!defined(SPI_MOSI_PD)
					//MOSIのﾌﾟﾙﾀﾞｳﾝ/ﾀﾞｳﾝが無指定のとき
	GPIO_IN(SPI_MOSI_GPIO,SPI_MOSI_BIT);	//MOSIを入力
#endif
#endif
#if !defined(SHSPI3)&&!defined(SHSPI2)	//4線SPIのとき
#ifdef SPI_MISO_PU			//MISOをﾌﾟﾙｱｯﾌﾟするとき
	GPIO_INPU(SPI_MISO_GPIO,SPI_MISO_BIT);	//MISOをﾌﾟﾙｱｯﾌﾟ入力
#endif
#ifdef SPI_MISO_PD			//MISOをﾌﾟﾙﾀﾞｳﾝするとき
	GPIO_INPD(SPI_MISO_GPIO,SPI_MISO_BIT);	//MISOをﾌﾟﾙﾀﾞｳﾝ入力
#endif
#if !defined(SPI_MISO_PU)&&!defined(SPI_MISO_PD)
					//MISOのﾌﾟﾙﾀﾞｳﾝ/ﾀﾞｳﾝが無指定のとき
	GPIO_IN(SPI_MISO_GPIO,SPI_MISO_BIT);	//MISOを入力
#endif
#endif
}
#endif


#if VOICE_S>0
#ifdef SLEEP_EN				//Sleep機能を実装するとき
//==============================================
//W25をﾊﾟﾜｰﾀﾞｳﾝ(API関数)
//==============================================
static void W25_PWR_DOWN(void)
{

	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_PwrDwn,1);			//ﾊﾟﾜｰﾀﾞｳﾝｺﾏﾝﾄﾞ
	spi_dis();				//ﾃﾞｨｾﾚｸﾄ
	wait_us(3);				//ﾊﾟﾜｰﾀﾞｳﾝ時間を待つ
}
#endif


//==============================================
//W25をﾊﾟﾜｰｱｯﾌﾟ(API関数)
//==============================================
static void W25_PWR_UP(void)
{

	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_Re_PwrDwn,0);		//ﾊﾟﾜｰﾀﾞｳﾝ解除ｺﾏﾝﾄﾞ
	spi_dis();				//ﾃﾞｨｾﾚｸﾄ
	wait_us(3);				//ﾊﾟﾜｰｱｯﾌﾟ時間を待つ
}
#endif


//==============================================
//SPI初期設定
//==============================================
#if VOICE_C>0				//音声Cを実装するとき
static void mmsd_init(void);
#endif
static void spi_init(void)
{

	spi_set();				//SPIﾎﾟｰﾄをｾｯﾄする
#ifndef SPI_SOFT			//内蔵SPIﾓｼﾞｭｰﾙを使うとき
	RCC->APBENR2|=RCC_APBENR2_SPI1EN;	//SPI1にｸﾛｯｸ供給
	SPI1->CR1=SPI_CR1_MSTR|			//ﾏｽﾀﾓｰﾄﾞ
		SPI_CR1_SSM|SPI_CR1_SSI|	//SSはｿﾌﾄ管理
		SPI_CR1_BR_0;			//ﾎﾞｰﾚｰﾄ設定(Fpclk/4=12Mbps)
//		SPI_CR1_BR_2;			//ﾎﾞｰﾚｰﾄ設定(Fpclk/32=1.5Mbps)
	SPI1->CR2|=SPI_CR2_FRXTH;		//受信ﾊﾞｯﾌｧ閾値(8ﾋﾞｯﾄ)
	SPI1->CR1|=SPI_CR1_SPE;			//SPIを有効
#endif
#if VOICE_C>0				//音声Cを実装するとき
	mmsd_init();				//SDCを初期化する
#endif
#if VOICE_S>0				//音声Sを実装するとき
	W25_PWR_UP();				//W25をﾊﾟﾜｰｱｯﾌﾟ
#endif
}
