#ifndef UARTBPS				//ﾎﾞｰﾚｰﾄが宣言されていないとき
#define	UARTBPS 38400				//38400bpsとする
#endif
#ifdef UART_SOFT			//UARTをｿﾌﾄ実装するとき
//==============================================
//UART出力の初期設定
//==============================================
static void uart_init(void)
{

	GPIO_H(UART_GPIO,UART_BIT);		//ｼﾘｱﾙ情報をH
	GPIO_OUT(UART_GPIO,UART_BIT);		//ｼﾘｱﾙ情報を出力
}


//==============================================
//送信完了の同期
//==============================================
#define	uart_out_sync()


//==============================================
//1文字を送信(ｿﾌﾄUART)
//==============================================
static void uart_out(				//戻り値無し
	unsigned char c)			//送信する文字
{
	unsigned char n=8;

	GPIO_L(UART_GPIO,UART_BIT);		//ｽﾀｰﾄﾋﾞｯﾄ
	wait_us(1000000/UARTBPS);		//1ﾋﾞｯﾄ巾
	while(n--)				//8ﾋﾞｯﾄ分繰返す
	{
		if(c&1) GPIO_H(UART_GPIO,UART_BIT);
						//ﾃﾞｰﾀﾋﾞｯﾄ
		else GPIO_L(UART_GPIO,UART_BIT);
		wait_us(1000000/UARTBPS);	//1ﾋﾞｯﾄ巾
		c>>=1;				//次のﾃﾞｰﾀﾋﾞｯﾄへ進む
	}
	GPIO_H(UART_GPIO,UART_BIT);		//ｽﾄｯﾌﾟﾋﾞｯﾄ
	wait_us(2000000/UARTBPS);		//2ﾋﾞｯﾄ巾
}


#else					//内蔵UARTﾓｼﾞｭｰﾙを使うとき
//==============================================
//UARTの初期設定
//==============================================
static void uart_init(void)
{

	RCC->APBENR2|=RCC_APBENR2_USART1EN;	//USART1にｸﾛｯｸ供給
	USART1->BRR=(SYS_CLK*10000000/UARTBPS+5)/10;
						//ﾎﾞｰﾚｰﾄ設定
	USART1->CR2=USART_CR2_STOP;		//ｽﾄｯﾌﾟ2ﾋﾞｯﾄ
	USART1->CR1=USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;
						//ﾃﾞｰﾀ8ﾋﾞｯﾄ､ﾊﾟﾘﾃｨ無し､送信有効､受信有効､
						//　UART有効
#ifdef UART_TX				//UART送信を実装するとき
	GPIO_AF(UART_GPIO,UART_BIT,UART_AF);	//ｼﾘｱﾙ情報出力を交代機能に設定
#endif
#ifdef UART_RX				//UART受信を実装するとき
	GPIO_AF(UART_IN_GPIO,UART_IN_BIT,UART_IN_AF);
						//ｼﾘｱﾙ情報入力を交代機能に設定
#endif
}


//==============================================
//送信完了の同期
//==============================================
static void uart_sync(void)
{

	while(!(USART1->SR&USART_SR_TC));	//送信完了を待つ
}


//==============================================
//1文字を送信
//初期化をせずに送信しようとすると無限ﾙｰﾌﾟして戻らない?
//==============================================
static void uart_out(				//戻り値無し
	unsigned char c)			//送信する文字
{

	while(!(USART1->SR&USART_SR_TXE));	//送信ﾃﾞｰﾀﾚｼﾞｽﾀの空きを待つ
	USART1->DR=c;				//1文字を送信する
}


//==============================================
//1文字を受信
//初期化をせずに受信しようとすると0x0000が返される
//==============================================
static short uart_in(void)			//受信完了時は受信ﾃﾞｰﾀを返す
						//未受信時は-1を返す
{

	if(USART1->SR&USART_SR_RXNE) return(USART1->DR);
						//受信完了していたら受信ﾃﾞｰﾀを返す
	return(-1);				//未受信を返す
}
#endif
