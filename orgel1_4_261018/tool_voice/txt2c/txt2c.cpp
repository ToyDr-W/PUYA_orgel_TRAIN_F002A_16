//#define SHDBG		//ﾃﾞﾊﾞｸﾞﾛｸﾞ(log.log)を出力するときに宣言する

//【実行形態】
//Win32ｺﾝｿｰﾙｱﾌﾟﾘｹｰｼｮﾝ
//
//【機能】
//①1行1ﾃﾞｰﾀのﾃｷｽﾄﾌｧｲﾙからﾃﾞｰﾀ定義のCｿｰｽｺｰﾄﾞを生成する
//②入力ﾃﾞｰﾀは符号無し16ﾋﾞｯﾄ､PCM0ﾚﾍﾞﾙは32768で表す
//③入力ﾃﾞｰﾀの最小値と最大値を検査して､最小値を0へｼﾌﾄし､ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞを変更する
//④IMA-ADPCMの初期値はpredicted=32768､step_index=0とすること
//
//【制約】
//ｻﾝﾌﾟﾙ数の最大値は #define MAX_DATA で定める
//
//【ﾊﾟﾗﾒｰﾀ】
//P1:入力ﾌｧｲﾙ(txt形式)のﾊﾟｽ名
//P2:出力ﾌｧｲﾙ(txt形式)のﾊﾟｽ名
//P3:処理ﾃﾞｰﾀ数
//P4:ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞ変換値[%]
//　0=変換しない
//　1～100=指定%に変換する
//　IMA=IMA-ADPCMｴﾝｺｰﾄﾞで1ﾊﾞｲﾄに先行ｻﾝﾌﾟﾙを下位､後行ｻﾝﾌﾟﾙを上位に出力する
//　省力時は8ﾋﾞｯﾄPCMで出力する
//
//【改変履歴】
//2015.11.02	新規作成
//2026.8.20	IMA-ADPCMｴﾝｺｰﾄﾞをｻﾎﾟｰﾄ


#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <share.h>
#include <stdlib.h>


//==============================================
//大域ﾃﾞｰﾀ
//==============================================
//ﾊﾟﾗﾒｰﾀ
static char *i_fname;				//入力ﾌｧｲﾙ(wav形式)ﾊﾟｽ名
static char *o_fname;				//出力ﾌｧｲﾙ(ｱｾﾝﾌﾞﾗｿｰｽ形式)ﾊﾟｽ名
static int data_n;				//生成するﾃﾞｰﾀ数
static int dyn_n;				//ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞ変換値[%](0=変換しない､1～100=指定%に変換する)
static unsigned short min,max;			//ｻﾝﾌﾟﾙの最小値､最大値
static char ima;				//IMA-ADPCMｴﾝｺｰﾄﾞ指定

//ﾃﾞｰﾀﾊﾞｯﾌｧ
#define MAX_DATA 100000
static unsigned short data[MAX_DATA+1];


////////////////////////////////////////////////
//ここからﾃﾞﾊﾞｸﾞ用の記述
//==============================================
//ﾃﾞﾊﾞｸﾞﾛｸﾞを出力するﾏｸﾛ
//==============================================
#define LOGFNAME "log.log"
#ifdef SHDBG
	#define Hm(x) {SHSHDBGm(__FILE__,__LINE__,(char *)(x));}
	#define Hd(x) {SHSHDBGd(__FILE__,__LINE__,#x,(int)(x));}
	#define Hld(x) {SHSHDBGld(__FILE__,__LINE__,#x,(long)(x));}
	#define Hu(x) {SHSHDBGu(__FILE__,__LINE__,#x,(unsigned int)(x));}
	#define Hlu(x) {SHSHDBGlu(__FILE__,__LINE__,#x,(unsigned long)(x));}
	#define Hx(x) {SHSHDBGx(__FILE__,__LINE__,#x,(unsigned int)(x));}
	#define Hlx(x) {SHSHDBGlx(__FILE__,__LINE__,#x,(unsigned long)(x));}
	#define Hs(x) {SHSHDBGs(__FILE__,__LINE__,#x,(char *)(x));}
	#define Hdump(x,l) {SHSHDBGdump(__FILE__,__LINE__,#x,(unsigned char *)(x),l);}
#else
	#define Hm(x)
	#define Hd(x)
	#define Hld(x)
	#define Hu(x)
	#define Hlu(x)
	#define Hx(x)
	#define Hlx(x)
	#define Hs(x)
	#define Hdump(x,l)
#endif

//==============================================
//ﾛｸﾞﾌｧｲﾙへ出力する
//==============================================
static void SHSHDBGlog(
	char *log)							//ﾛｸﾞ内容
{
	int fd;

//	if((fd=open(LOGFNAME,O_RDWR|O_APPEND|O_CREAT,07666))==-1)
	if((_sopen_s(&fd,LOGFNAME,O_RDWR|O_APPEND|O_CREAT,_SH_DENYNO,_S_IREAD|_S_IWRITE))==-1)
										//ﾛｸﾞﾌｧｲﾙをｵｰﾌﾟﾝする
	{
		return;
	}
	_write(fd,log,strlen(log));			//ﾛｸﾞを書出す
	_close(fd);
}

//==============================================
//ﾛｸﾞ編集(ﾒｯｾｰｼﾞ表示のみ)
//==============================================
static void SHSHDBGm(
	char *file,
	int line,
	char *msg)
{
	char s[128];

	sprintf_s(s,sizeof(s),"%.32s %d:%.64s\n",file,line,msg);
	SHSHDBGlog(s);
}

//==============================================
//ﾛｸﾞ編集関数(%d表示)
//==============================================
static void SHSHDBGd(
	char *file,
	int line,
	char *name,
	int value)
{
	char s[128];

	sprintf_s(s,sizeof(s),"%.32s %d:%s=%d\n",file,line,name,value);
	SHSHDBGlog(s);
}

//==============================================
//ﾛｸﾞ編集関数(%u表示)
//==============================================
static void SHSHDBGu(
	char *file,
	int line,
	char *name,
	unsigned int value)
{
	char s[128];

	sprintf_s(s,sizeof(s),"%.32s %d:%s=%u\n",file,line,name,value);
	SHSHDBGlog(s);
}

//==============================================
//ﾛｸﾞ編集関数(%x表示)
//==============================================
static void SHSHDBGx(
	char *file,
	int line,
	char *name,
	unsigned int value)
{
	char s[128];

	sprintf_s(s,sizeof(s),"%.32s %d:%s=0x%x\n",file,line,name,value);
	SHSHDBGlog(s);
}

//==============================================
//ﾛｸﾞ編集関数(%s表示)
//==============================================
static void SHSHDBGs(
	char *file,
	int line,
	char *name,
	char *value)
{
	char s[192];

	sprintf_s(s,sizeof(s),"%.32s %d:%s=%.128s\n",file,line,name,value);
	SHSHDBGlog(s);
}

//==============================================
//ﾛｸﾞ編集関数(16進ﾀﾞﾝﾌﾟ表示)
//==============================================
static void SHSHDBGdump(
	char *file,
	int line,
	char *name,
	unsigned char *data,
	int len)
{
	char s[128],*p;
	int n,m,l;

	sprintf_s(s,sizeof(s),"%.32s %d:%s(%x)%d\n",file,line,name,data,len);
	SHSHDBGlog(s);
	for(n=0;n<len;n+=16)
	{
		sprintf_s(s,sizeof(s),"[%04x]",n);
		p=s+6;
		if((m=len-n)>16) m=16;
		for(l=0;l<m;l++)
		{
			if(l==8) *p++='-';
			else if(l) *p++=' ';
			sprintf_s(p,3,"%02x",*data++);
			p+=2;
		}
		*p++='\n';
		*p='\0';
		SHSHDBGlog(s);
	}
}


////////////////////////////////////////////////
//ここからｱﾌﾟﾘｹｰｼｮﾝの記述
//==============================================
//元ﾌｧｲﾙのﾛｰﾄﾞ処理
//==============================================
static int wav_load(void)
{
	FILE *i_fp;
	char buf[128];
	int	c,n;

//Hm("wav_load入る");
	if((i_fp=fopen(i_fname,"r"))==NULL)
	{
		printf("入力10進txt形式)ﾌｧｲﾙのfopen()ｴﾗｰ fname:%s errno:%d\n",i_fname,errno);
		return(1);
	}
	min=65535;
	max=0;
	for(n=0;n<MAX_DATA;n++)
	{
//Hd(n);
		if(fgets(buf,sizeof(buf)-1,i_fp)==NULL)
		{
			if(ferror(i_fp))
			{
				printf("入力(10進txt形式)ﾌｧｲﾙのfgets()ｴﾗｰ fname:%s errno:%d\n",i_fname,errno);
				return(9);
			}
			if(feof(i_fp)) break;
		}
//Hs(buf);
		c=atoi(buf);
//Hd(c);
		data[n]=c;
		if(min>c) min=c;
		if(max<c) max=c;
	}
	data_n=n;
//Hd(data_n);
Hd(min);
Hd(max);
	data[data_n]=data[0];
	if(fclose(i_fp))
	{
		printf("入力(10進txt形式)ﾌｧｲﾙのfclose()ｴﾗｰ fname:%s errno:%d\n",i_fname,errno);
		return(9);
	}
//Hdump(data,data_n*2);
//Hm("wav_load出る");
	return(0);
}


//==============================================
//ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞの変換処理
//==============================================
static void dyn_ext(void)
{
	int n;
	short s;
	unsigned short u;

//Hm("dyn_ext入る");
//Hd(min);
//Hd(max);

	//変換処理
	for(n=0;n<data_n;n++)
	{
//Hd(data[n]);
Hd(data[n]);
		u=((unsigned long)data[n]-min)*65536/(max-min);
Hd(u);
		s=u-32768;
Hd(s);
		data[n]=s*dyn_n/100+32768;
Hd(data[n]);
	}
//Hdump(data,data_n*2);
//Hm("dyn_ext出る");
}


//==============================================
//IMA-ADPCM制御ﾃｰﾌﾞﾙ
//==============================================
//ｽﾃｯﾌﾟｻｲｽﾞﾃｰﾌﾞﾙ(89段階)
static const short STEP_TABLE[89]=
	{
	7,8,9,10,11,12,13,14,
	16,17,19,21,23,25,28,31,
	34,37,41,45,50,55,60,66,
	73,80,88,97,107,118,130,143,
	157,173,190,209,230,253,279,307,
	337,371,408,449,494,544,598,658,
	724,796,876,963,1060,1166,1282,1411,
	1552,1707,1878,2066,2272,2499,2749,3024,
	3327,3660,4026,4428,4871,5358,5894,6484,
	7132,7845,8630,9493,10442,11487,12635,13899,
	15289,16818,18500,20350,22385,24623,27086,29794,
	32767
	};

//ｲﾝﾃﾞｯｸｽ調整ﾃｰﾌﾞﾙ(ｴﾝｺｰﾄﾞ値の下位3bitで引く)
static const char INDEX_TABLE[8]=
	{
	-1,-1,-1,-1,2,4,6,8
};

static long predicted;				//直前の予測値
static short step_index;			//ｽﾃｯﾌﾟｲﾝﾃﾞｯｸｽ(0～88)
static long step;


//==============================================
//IMA-ADPCM共通処理
//==============================================
static void adpcm_step(
	unsigned char nibble)			//ｴﾝｺｰﾄﾞ値(4ﾋﾞｯﾄIMA-ADPCM値)
{
	long delta;

	//予測値を更新
	delta=step>>3;
	if(nibble&4) delta+=step;
	if(nibble&2) delta+=step>>1;
	if(nibble&1) delta+=step>>2;
	if(nibble&8) predicted-=delta;
	else predicted+=delta;

	//16bitにｸﾗﾝﾌﾟ
	if(predicted>65535) predicted=65535;
	else if(predicted<0) predicted=0;

	//step_indexを更新
	step_index+=INDEX_TABLE[nibble&7];
	if(step_index<0) step_index=0;
	else if(step_index>88) step_index=88;
}


//==============================================
//IMA-ADPCMﾃﾞｺｰﾄﾞ処理
//==============================================
static unsigned short adpcm_decode(		//ﾃﾞｺｰﾄﾞ値(16ﾋﾞｯﾄPCM値)を返す
										//　0ﾚﾍﾞﾙは32768
	unsigned char nibble)			//ｴﾝｺｰﾄﾞ値(4ﾋﾞｯﾄIMA-ADPCM値)
{

	//ﾃﾞｺｰﾄﾞの初期化
	step=STEP_TABLE[step_index];

	//共通処理
	adpcm_step(nibble);

	return((short)predicted);
}
	

//==============================================
//IMA-ADPCMｴﾝｺｰﾄﾞ処理
//==============================================
static unsigned char adpcm_encode(		//ｴﾝｺｰﾄﾞ値(4ﾋﾞｯﾄIMA-ADPCM値)を返す
	unsigned short sample)			//ｻﾝﾌﾟﾙ値(16ﾋﾞｯﾄPCM値)
						//　0ﾚﾍﾞﾙは32768
{
	long diff;
	unsigned char nibble;

Hd(sample);
Hd(predicted);
	//ｴﾝｺｰﾄﾞの初期化
	step=STEP_TABLE[step_index];
Hd(step);
	diff=sample-predicted;
Hd(diff);
	nibble=0;

	//bit3:符号ﾋﾞｯﾄ
	if(diff<0) {nibble=8;diff=-diff;}

	//bit2?0:差分をｽﾃｯﾌﾟｻｲｽﾞで量子化
	if(diff>=step) {nibble|=4;diff-=step;}
	if(diff>=step>>1) {nibble|=2;diff-=step>>1;}
	if(diff>=step>>2) {nibble|=1;}
Hd(nibble);

	//共通処理
	adpcm_step(nibble);
Hd(predicted);
Hd(step_index);

	return(nibble&0x0F);
}


//==============================================
//出力ﾌｧｲﾙの生成処理
//==============================================
static int tbl_out(void)
{
	FILE *o_fp;
	int n;
	unsigned char n_l,n_h;

//Hm("tbl_out入る");
	if((o_fp=fopen(o_fname,"w"))==NULL)
	{
		printf("出力(Cｿｰｽ)ﾌｧｲﾙのfopen()ｴﾗｰ fname:%s errno:%d\n",o_fname,errno);
		return(1);
	}

	//IMA-ADPCMｴﾝｺｰﾄﾞ有り
	if(ima)
	{
		predicted=32768;
		step_index=0;
		for(n=0;n+1<data_n;n+=2)
		{
			n_l=adpcm_encode(data[n]);
			n_h=adpcm_encode(data[n+1]);
			if(fprintf(o_fp,"\t(%d<<4)+%d,\n",n_h,n_l)<0)
			{
				printf("出力(Cｿｰｽ)ﾌｧｲﾙのfprintf()ｴﾗｰ fname:%s errno:%d\n",o_fname,errno);
				return(9);
			}
		}
	}

	//IMA-ADPCMｴﾝｺｰﾄﾞ無し
	else
	{
		for(n=0;n<data_n;n++)
		{
			if(fprintf(o_fp,"\t%d,\n",data[n]>>8)<0)
			{
				printf("出力(Cｿｰｽ)ﾌｧｲﾙのfprintf()ｴﾗｰ fname:%s errno:%d\n",o_fname,errno);
				return(9);
			}
		}
	}

	if(fclose(o_fp))
	{
		printf("出力(Cｿｰｽ)ﾌｧｲﾙのfclose()ｴﾗｰ fname:%s errno:%d\n",o_fname,errno);
		return(9);
	}
//Hm("tbl_out出る");
	return(0);
}


//==============================================
//ﾒｲﾝ処理
//==============================================
int main(int argc,char *argv[])
{

	//ﾊﾟﾗﾒｰﾀ取り込み
//Hd(argc);
	if(argc<5)
	{
		printf("【ﾊﾟﾗﾒｰﾀ】\n"
			"P1:入力ﾌｧｲﾙ(wav形式)のﾊﾟｽ名\n"
			"P2:出力ﾌｧｲﾙ(txt形式)のﾊﾟｽ名\n"
			"P3:処理ﾃﾞｰﾀ数\n"
			"P4:ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞ変換値[%]\n"
			"　0=変換しない\n"
			"　1～100=指定%に変換する\n"
			"P5:IMA-ADPCMｴﾝｺｰﾄﾞを行うときは IMA を指定する\n");
			
		return(9);
	}

	//P1:入力ﾌｧｲﾙ(wav形式)のﾊﾟｽ名
	i_fname=argv[1];
//Hs(i_fname);

	//P2:出力ﾌｧｲﾙ(ｱｾﾝﾌﾞﾗｿｰｽ形式)のﾊﾟｽ名
	o_fname=argv[2];
//Hs(o_fname);

	//P3:処理ﾃﾞｰﾀ数
	data_n=atoi(argv[3]);
//Hd(data_n);
	if(data_n<1 || MAX_DATA<data_n)
	{
		printf("P3:wavﾌｧｲﾙのﾃﾞｰﾀ数は 1～%ld の値を指定すること\n",MAX_DATA);
		return(9);
	}

	//P4:ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞ変換値
	dyn_n=atoi(argv[4]);
//Hd(dyn_n);
	if(dyn_n<0)
	{
		printf("P4:ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞ変換値は 0以上 で指定すること\n");
		return(9);
	}

	//P5:IMA-ADPCMｴﾝｺｰﾄﾞ指定
	if(argc>=6)
	{
//Hs(argv[5]);
		if(strcmp(argv[5],"IMA")==0) ima=1;
	}
//Hd(ima);

	//入力ﾌｧｲﾙ(txt形式)のﾛｰﾄﾞ
	if(wav_load()) return(9);

	//ﾀﾞｲﾅﾐｯｸﾚﾝｼﾞの変換
	if(dyn_n) dyn_ext();

	//出力ﾌｧｲﾙ(txt形式)の生成
	if(tbl_out()) return(9);

	printf("%dﾃﾞｰﾀ処理しました\n",data_n);
	return(0);
}

