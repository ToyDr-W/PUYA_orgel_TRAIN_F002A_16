//ISP形態でSPIﾌﾗｯｼｭを書替えるｱﾌﾟﾘｹｰｼｮﾝの共通機能部


//==============================================
//大域ﾃﾞｰﾀ
//==============================================
static unsigned long addr;			//SPIﾒﾓﾘｱﾄﾞﾚｽ
static unsigned char data[259];			//UART受信ﾊﾞｯﾌｧ
static unsigned short len;			//UART受信ﾃﾞｰﾀ数


//==============================================
//UARTﾌﾞﾛｯｸ受信
//260ﾊﾞｲﾄ受信又は100msの無受信でﾌﾞﾛｯｸ終了と判定する
//==============================================
static void recv_blk(void)
{
#define	TOUT	100000				//ﾀｲﾑｱｳﾄ値[us]
	short c;

//DBG_C('<');
//DBG_C('b');
	for(len=0;len<259;len++)
	{
//DBG_C('@');
//DBG_B(len);
		SysTick->CTRL=0x00000004;	//ｸﾛｯｸｿｰｽはAHB､例外ｱｻｰﾄしない､ｶｳﾝﾀ無効
		SysTick->LOAD=SYS_CLK*TOUT-SYS_CLK/2;
						//比較値を設定
		SysTick->VAL=0;			//ｶｳﾝﾀを初期化
		SysTick->CTRL=0x00000005;	//ｸﾛｯｸｿｰｽはAHB､例外ｱｻｰﾄしない､ｶｳﾝﾀ有効
		while(!(SysTick->CTRL&0x00010000))
						//ﾀｲﾑｱｳﾄするまで繰返す
		{
			if((c=uart_in())>=0) break;
		}
//DBG_C('-');
//DBG_B(c);
		if(c<0) break;
//DBG_B(c);
		data[len]=c;
	}
//DBG_C('=');
//DBG_S(len);
//DBG_C('>');
}


//==============================================
//SPIｱﾄﾞﾚｽを設定する
//==============================================
void set_addr(void)
{

//DBG_C('<');
//DBG_C('a');
//DBG_L(addr);
	spi_send(addr>>16,0);			//ｱﾄﾞﾚｽ(MSB)
	spi_send(addr>>8,0);			//ｱﾄﾞﾚｽ
	spi_send(0,0);				//ｱﾄﾞﾚｽ(LSB)
//DBG_C('>');
}


//==============================================
//SPI読込みｱﾄﾞﾚｽを設定する
//==============================================
void read_addr(void)
{

//DBG_C('<');
//DBG_C('r');
	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_ReadData,0);		//ﾃﾞｰﾀ読込みｺﾏﾝﾄﾞ
	set_addr();				//ｱﾄﾞﾚｽ設定
//DBG_C('>');
}


//==============================================
//SPI書込み許可を設定する
//==============================================
void write_enable(void)
{

//DBG_C('<');
//DBG_C('e');
	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_WriteEnable,1);		//書込み許可ｺﾏﾝﾄﾞ
	spi_dis();				//ﾃﾞｨｾﾚｸﾄ
//DBG_C('>');
}


//==============================================
//SPI書込みｱﾄﾞﾚｽを設定する
//==============================================
void write_addr(void)
{

//DBG_C('<');
//DBG_C('w');
	write_enable();				//書込み許可
	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_PageProgram,0);		//ﾃﾞｰﾀ書込みｺﾏﾝﾄﾞ
	set_addr();				//ｱﾄﾞﾚｽ設定
//DBG_C('>');
}


//==============================================
//SPI書込み完了を待つ
//==============================================
void write_exec(void)
{

//DBG_C('<');
//DBG_C('d');
	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_ReadStsReg1,0);		//ｽﾃｰﾀｽﾚｼﾞｽﾀ1を読む
	while(spi_recv()&W25_BusyMsk){};	//Busyﾋﾞｯﾄのｸﾘｱを待つ
	spi_dis();				//ﾃﾞｨｾﾚｸﾄ
//DBG_C('>');
}


//==============================================
//SPIﾌﾗｯｼｭをﾁｯﾌﾟ消去する
//==============================================
static void spiflash_erase(void)
{

//DBG_C('\n');
//DBG_C('<');
//DBG_C('c');
	write_enable();				//書込み許可
	spi_sel();				//ｾﾚｸﾄ
	spi_send(W25_ChipErase,1);		//ﾁｯﾌﾟ消去(最終ﾊﾞｲﾄ指定)
	spi_dis();				//ﾃﾞｨｾﾚｸﾄ(書込み開始)
	write_exec();				//書込み完了を待つ
	uart_out('O');				//OKを返す
	uart_out('K');
	uart_out('\n');
//DBG_C('>');
}


//==============================================
//SPIﾌﾗｯｼｭを書込む
//==============================================
static void spiflash_write(void)
{
	unsigned short n;
	unsigned char x;
	unsigned char sts=0;

//DBG_C('\n');
//DBG_C('<');
//DBG_C('l');
	uart_out('O');				//OKを返す
	uart_out('K');
	uart_out('\n');

	for(;;)					//繰返し
	{
//DBG_C('@');

		//ﾌﾞﾛｯｸ受信
		recv_blk();			//PCから1ﾍﾟｰｼﾞ分(260ﾊﾞｲﾄ)を受信する
		if(len==0)
		{
			if(sts) break;		//転送開始後のﾀｲﾑｱｳﾄは転送終了と判断する
			continue;		//転送開始前のﾀｲﾑｱｳﾄは受信を繰返す待つ
		}
		if(len<259)			//ﾌﾞﾛｯｸ長が259に満たないとき
		{
//DBG_C('x');
			uart_out(2);		//2(ﾌﾞﾛｯｸ長が259に満たない)を返す
			break;
		}
		sts=1;				//転送開始の印
//DBG_C('-');
//DBG_S(len);
//DBG_C('-');
//DBG_B(data[0]);
//DBG_B(data[1]);
//DBG_B(data[2]);
//DBG_B(data[3]);
//DBG_B(data[4]);

		//ﾁｪｯｸｻﾑをﾁｪｯｸ
		for(n=0,x=0;n<259;n++)		//ﾌﾞﾛｯｸ全体を繰返す
		{
			x^=data[n];		//ﾁｪｯｸｻﾑはEORで計算する
		}
		if(x)				//ﾁｪｯｸｻﾑ不一致のとき
		{
//DBG_B(x);
			uart_out(3);		//3(ﾁｪｯｸｻﾑ不一致)を返す
			break;
		}

		//SPIﾌﾗｯｼｭ書込み
		addr=(data[0]<<16)+(data[1]<<8);//書込みｱﾄﾞﾚｽを取得する
//DBG_C('-');
//DBG_L(addr);
		write_addr();			//書込みｱﾄﾞﾚｽを設定する
		for(n=2;n<257;n++)		//最後のﾃﾞｰﾀの直前まで繰返す
		{
			spi_send(data[n],0);	//ﾃﾞｰﾀを書込む
		}
		spi_send(data[257],1);		//最後のﾃﾞｰﾀを書込む(最終ﾋﾞｯﾄ指定)
		spi_dis();			//ﾃﾞｨｾﾚｸﾄ(書込み開始)
		write_exec();			//書込み完了を待つ

		//SPIﾌﾗｯｼｭ照合
		read_addr();			//読込みｱﾄﾞﾚｽを設定する
		for(n=2;n<258;n++)		//ﾍﾟｰｼﾞ内を繰返す
		{
			if(spi_recv()!=data[n]) break;
						//ﾃﾞｰﾀを照合する
		}
		spi_dis();			//ﾃﾞｨｾﾚｸﾄ
//DBG_C('=');
//DBG_S(n);

		//照合結果を応答
		if(n==258)
		{
			uart_out(0);		//0(照合OK)を返す
		}
		else
		{
			uart_out(1);		//1(照合NG)を返す
			break;
		}
	}
//DBG_C('>');
}


//==============================================
//ｱﾌﾟﾘの初期設定
//==============================================
static void apl_init(void)
{
}


//==============================================
//main
//==============================================
int main(void)
{

	CB_INIT();				//ﾃﾞﾊﾞｲｽの初期設定
DBG_C('\n');
DBG_C('!');

	for(;;)
	{
		recv_blk();			//UARTからﾌﾞﾛｯｸ受信する
//DBG_B(len);
		if(len!=5) continue;		//ﾌﾞﾛｯｸ長が5でなければ無視する
DBG_C('@');
DBG_B(len);
DBG_C('-');
DBG_B(data[0]);
		if(data[0]=='E')		//ﾁｯﾌﾟ消去指示のとき
		{
			spiflash_erase();	//ﾁｯﾌﾟ消去する
		}
		else if(data[0]=='W')		//書込み指示のとき
		{
			spiflash_write();	//書込みする
		}
	}
DBG_C('>');
}
