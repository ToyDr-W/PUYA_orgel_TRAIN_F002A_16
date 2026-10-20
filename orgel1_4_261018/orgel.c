//電子ｵﾙｺﾞｰﾙVer1_4のｴﾝｼﾞﾝ部(CH32V/PUYA32/STM32で共通)

//言語処理系の違いを隠蔽するため､MounRiverStudioの場合は呼込み元で下記を宣言しておくこと
//#define	MounRiverStudio

//音価･音高･音符ﾃﾞｰﾀの設計は｢ドキュメント\電子オルゴール.xls｣を参照
//音声ﾃﾞｰﾀと曲ﾃﾞｰﾀの枠組みはorgel.hを参照
//ｿﾝｸﾞ･音符ﾃﾞｰﾀの枠組みはorgel.incを参照
//ﾎﾟｰﾄ割当て､ﾀｲﾐﾝｸﾞ設定は固有処理部を参照


//==============================================
//言語処理系の違いを隠蔽する
//==============================================
#ifdef	MounRiverStudio
#define	SONG_IDX_BASE	0	//MounRiverStudioではSONG_IDXのﾍﾞｰｽを加算する必要がない
#else
#define	SONG_IDX_BASE	(unsigned long)&SONG_IDX
				//uVisionではSONG_IDXのﾍﾞｰｽを加算する
#endif


//==============================================
//KB音量ﾃｰﾌﾞﾙ
//==============================================
#if KB_N>0
static const unsigned char kb_vol[]={
	KB0_VOL,
	KB1_VOL,
};
#endif


//==============================================
//音声N音量ﾃｰﾌﾞﾙ
//==============================================
#if VOICE_N>0
static const unsigned char voiceN_vol[]={
	VOICEN0_VOL*255/100,
	VOICEN1_VOL*255/100,
	VOICEN2_VOL*255/100,
};
#endif


//==============================================
//音声S音量ﾃｰﾌﾞﾙ
//==============================================
#if VOICE_S>0
static const unsigned char voiceS_vol[]={
	VOICES0_VOL*255/100,
	VOICES1_VOL*255/100,
	VOICES2_VOL*255/100,
};
#endif


//==============================================
//音価ﾃｰﾌﾞﾙ(音価ｺｰﾄﾞをﾌﾟﾘｽｹｰﾙ値に変換する)
//==============================================
static const unsigned char onka_tbl[]={
	2,	//1=32分3連符		=kR32
	4,	//2=16分3連符		=kR16
	6,	//3=32分音符		=kK32
	8,	//4=8分3連符		=kR8
	9,	//5=付点32分音符	=kP32
	12,	//6=16分音符		=kK16
	16,	//7=4分3連符		=kR4
	18,	//8=付点16分音符	=kP16
	24,	//9=8分音符		=kK8
	32,	//10=2分3連符		=kR2
	36,	//11=付点8分音符	=kP8
	48,	//12=4分音符		=kK4
	64,	//13=全3連符		=kR1
	72,	//14=付点4分音符	=kP4
	96,	//15=2分音符		=kK2
	144,	//16=付点2分音符	=kP2
	192,	//17=全音符		=kK1
	80,	//18=kW80		=kW80
	84,	//19=kW84		=kW84
	168,	//20=kW168		=kW168
	255,	//21=kW255		=kW255
};


//==============================================
//ﾎﾟﾙﾀﾒﾝﾄﾃｰﾌﾞﾙ(音価ｺｰﾄﾞを音源飛び数増減量率/ﾌﾟﾘｽｹｰﾙに変換する)
//==============================================
static const unsigned char port_tbl[]={
	0,		//1=32分3連符		=kR32
	0,		//2=16分3連符		=kR16
	0,		//3=32分音符		=kK32
	0,		//4=8分3連符		=kR8
	0,		//5=付点32分音符	=kP32
	0,		//6=16分音符		=kK16
	0,		//7=4分3連符		=kR4
	4096/(18-1),	//8=付点16分音符	=kP16
	4096/(24-1),	//9=8分音符		=kK8
	4096/(32-1),	//10=2分3連符		=kR2
	4096/(36-1),	//11=付点8分音符	=kP8
	4096/(48-1),	//12=4分音符		=kK4
	4096/(64-1),	//13=全3連符		=kR1
	4096/(72-1),	//14=付点4分音符	=kP4
	4096/(96-1),	//15=2分音符		=kK2
	4096/(144-1),	//16=付点2分音符	=kP2
	4096/(192-1),	//17=全音符		=kK1
	4096/(80-1),	//18=kW80		=kW80
	4096/(84-1),	//19=kW84		=kW84
	4096/(168-1),	//20=kW168		=kW168
	4096/(255-1),	//21=kW255		=kW255
};


//==============================================
//音高ｺｰﾄﾞをｵｸﾀｰﾌﾞ･音名に変換するﾃｰﾌﾞﾙ
//　上位ﾆﾌﾞﾙ:ｵｸﾀｰﾌﾞ番号(0起算)(音符ﾃﾞｰﾀ上のｵｸﾀｰﾌﾞ番号とは逆順になっていることに留意すること)
//　下位ﾆﾌﾞﾙ:音名番号(0起算)(音符ﾃﾞｰﾀ上の音名番号とは境界が異なることに留意すること)
//==============================================
static const unsigned char onkou_mei_tbl[]={
	0x58,	//0:Do
	0x59,	//1:DoS/ReF
	0x5a,	//2:Re
	0x5b,	//3:ReS/MiF
	0x40,	//4:Mi
	0x41,	//5:Fa
	0x42,	//6:FaS/SoF
	0x43,	//7:So
	0x44,	//8:SoS/LaF
	0x45,	//9:La
	0x46,	//10:LaS/SiF
	0x47,	//11:Si
	0x48,	//12:Do
	0x49,	//13:DoS/ReF
	0x4a,	//14:Re
	0x4b,	//15:ReS/MiF
	0x30,	//16:Mi
	0x31,	//17:Fa
	0x32,	//18:FaS/SoF
	0x33,	//19:So
	0x34,	//20:SoS/LaF
	0x35,	//21:La
	0x36,	//22:LaS/SiF
	0x37,	//23:Si
	0x38,	//24:Do
	0x39,	//25:DoS/ReF
	0x3a,	//26:Re
	0x3b,	//27:ReS/MiF
	0x20,	//28:Mi
	0x21,	//29:Fa
	0x22,	//30:FaS/SoF
	0x23,	//31:So
	0x24,	//32:SoS/LaF
	0x25,	//33:La
	0x26,	//34:LaS/SiF
	0x27,	//35:Si
	0x28,	//36:Do
	0x29,	//37:DoS/ReF
	0x2a,	//38:Re
	0x2b,	//39:ReS/MiF
	0x10,	//40:Mi
	0x11,	//41:Fa
	0x12,	//42::FaS/SoF
	0x13,	//43:So
	0x14,	//44:SoS/LaF
	0x15,	//45:La
	0x16,	//46:LaS/SiF
	0x17,	//47:Si
	0x18,	//48:Do
	0x19,	//49:DoS/ReF
	0x1a,	//50:Re
	0x1b,	//51:ReS/MiF
	0x00,	//52:Mi
	0x01,	//53:Fa
	0x02,	//54:FaS/SoF
	0x03,	//55:So
	0x04,	//56:SoS/LaF
	0x05,	//57:La
	0x06,	//58:LaS/SiF
	0x07,	//59:Si
	0x08,	//60:Do
	0x09,	//61:DoS/ReF
	0x0a,	//62:Re
	0x0b,	//63:ReS/MiF
};


//==============================================
//音高ﾃｰﾌﾞﾙ
//==============================================
#define	smp_mac(fff)	((unsigned long)fff*256/10*256/25*PWM_PR/10*SONG_PS/4000+5)/10
						//fff=周波数[0.01Hz]
static const unsigned short onkou_tbl[]={
	smp_mac(131851),	//4:Mi
	smp_mac(139691),	//5:Fa
	smp_mac(147998),	//6:FaS/SoF
	smp_mac(156798),	//7:So
	smp_mac(166122),	//8:SoS/LaF
	smp_mac(176000),	//9:La
	smp_mac(186466),	//10:LaS/SiF
	smp_mac(197553),	//11:Si
	smp_mac(209300),	//0:Do
	smp_mac(221746),	//1:DoS/ReF
	smp_mac(234932),	//2:Re
	smp_mac(248902),	//3:ReS/MiF
};


//==============================================
//PWM出力ﾊﾞｯﾌｧ
//==============================================
static volatile short buf[OUT_BUF_N];		//ﾊﾞｯﾌｧ
static volatile unsigned char buf_write,buf_read;
						//ﾊﾞｯﾌｧの書込み､読込み位置


//==============================================
//制御ﾃｰﾌﾞﾙ
//==============================================
static struct KYOTUU kyotuu;			//共通制御ﾃｰﾌﾞﾙ
#if	PART_N+KB_N>0
static struct SONG song;			//演奏制御ﾃｰﾌﾞﾙ
static struct PART part[PART_N+KB_N];		//ﾊﾟｰﾄ制御ﾃｰﾌﾞﾙ
#endif
#if	VOICE_N>0
static struct VOICEN voiceN[VOICE_N];		//音声N再生制御ﾃｰﾌﾞﾙ
#endif
#if	VOICE_I>0
static struct VOICEI voiceI;			//音声I再生制御ﾃｰﾌﾞﾙ
#endif
#if	VOICE_C>0
static struct VOICEC voiceC;			//音声C再生制御ﾃｰﾌﾞﾙ
#endif
#if	VOICE_S>0
static struct VOICES voiceS[VOICE_S];		//音声S再生制御ﾃｰﾌﾞﾙ
#endif


//==============================================
//ﾐｭｰﾄ設定処理
//==============================================
//未実装


#if PART_N>0
//==============================================
//曲ﾃﾞｰﾀ初期化処理
//==============================================
static void song_init(void)
{
	struct PART *part_pt;
	unsigned short m,n;
	unsigned char *q;

//DBG_C('\n');
	song.key=song.song_hdr->key;		//移調度を取込む
//DBG_B(song.key);
	song.onka_psn=song.song_hdr->onka_psn;	//音価ﾌﾟﾘｽｹｰﾙ値を取込む
//DBG_S(song.onka_psn);
	song.song_ps=0;				//演奏ﾌﾟﾘｽｹｰﾗをｸﾘｱ
	song.onka_ps=0;				//音価ﾌﾟﾘｽｹｰﾗをｸﾘｱ
//DBG_D(&song,sizeof(song));
	for(m=0,part_pt=part;m<PART_N;m++,part_pt++)
						//ﾊﾟｰﾄ個別制御ﾃｰﾌﾞﾙの初期設定
	{
//DBG_C('\n');
//DBG_C('m');
//DBG_B(m);
//DBG_L(part_pt);
		if(m<song.song_hdr->part_su)	//実装ﾊﾟｰﾄのとき
		{
			for(n=0,q=(unsigned char *)part_pt;n<sizeof(struct PART);n++,q++)
				*q=0;		//先ずｸﾘｱしておく
			part_pt->song_part=&(song.song_hdr->part[m]);
						//ﾊﾟｰﾄ情報へのﾎﾟｲﾝﾀを取得する
//DBG_L(part_pt->song_part);
			part_pt->onpu=(unsigned short *)
				((unsigned long)part_pt->song_part->onpu+SONG_IDX_BASE);
						//音符ﾃﾞｰﾀへのﾎﾟｲﾝﾀを取得する
//DBG_L(part_pt->onpu);
			part_pt->ongen=(struct WAVE_HDR *)
				((unsigned long)part_pt->song_part->ongen+SONG_IDX_BASE);
						//音源波形へのﾎﾟｲﾝﾀを取得する
//DBG_L(part_pt->ongen);
			part_pt->enve=(struct ENVE_HDR *)
				((unsigned long)part_pt->song_part->enve+SONG_IDX_BASE);
						//ｴﾝﾍﾞﾛｰﾌﾟ波形へのﾎﾟｲﾝﾀを取得する
//DBG_L(part_pt->enve);
			part_pt->enve_psn=part_pt->song_part->enve_psn;
						//ｴﾝﾍﾞﾛｰﾌﾟ処理ﾌﾟﾘｽｹｰﾙ値を取得する
//DBG_L(part_pt->enve_psn);
			part_pt->pitch=(struct PITCH_HDR *)
				((unsigned long)part_pt->song_part->pitch+SONG_IDX_BASE);
						//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形へのﾎﾟｲﾝﾀを取得する
//DBG_L(part_pt->pitch);
			part_pt->pitch_psn=part_pt->song_part->pitch_psn;
						//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形へのﾎﾟｲﾝﾀを取得する
//DBG_L(part_pt->pitch_psn);
//DBG_D(&part[m],sizeof(part[m]));
		}
		else part_pt->song_part=0;	//未実装ﾊﾟｰﾄは演奏終了にしておく
	}
}


//==============================================
//曲選択処理(API関数)
//==============================================
static void SONG_SEL(
	unsigned char no)			//曲番号(0は演奏中止)
{
	unsigned n;

//DBG_C('\n');
//DBG_C('s');
//DBG_B(no);
//DBG_L(&SONG_IDX);
//DBG_D(&SONG_IDX,16);
//DBG_C('\n');
//DBG_B(song.no);
	if(no)					//演奏開始のとき
	{
//DBG_B(SONG_IDX.no);
		if(song.no==no) return;		//演奏中の曲声番号と同じときは戻る
		if(no>SONG_IDX.no) return;	//実装曲数を超えているときは戻る
		song.no=no;			//曲番号を記憶
//DBG_B(song.no);
//DBG_L(SONG_IDX.song_hdr[song.no-1]);
		song.song_hdr=(struct SONG_HDR *)
			((unsigned long)SONG_IDX.song_hdr[song.no-1]+SONG_IDX_BASE);
						//曲ﾍｯﾀﾞへのﾎﾟｲﾝﾀを取得
//DBG_L(song.song_hdr);
//DBG_D(song.song_hdr,64);
		song_init();			//曲ﾃﾞｰﾀを初期化
	}
	else					//演奏中止のとき
	{
		for(n=0;n<PART_N;n++) part[n].song_part=0;
						//全ﾊﾟｰﾄを演奏終了にする
		song.no=0;			//演奏終了にする
		song.pcm=0;			//全ﾊﾟｰﾄ分のPCM値をｸﾘｱしておく
	}
//DBG_C('>');
}
#endif


#if KB_N>0
//==============================================
//KB演奏開始処理(API関数)
//==============================================
static void KB_START(
	unsigned char kb,			//KB演奏ﾊﾟｰﾄ番号(0起算)
						//制御表はpart[PART_N+kb]にｱｸｾｽする
	const unsigned short kb_psn,		//音価ﾌﾟﾘｽｹｰﾙ値
	const struct WAVE_HDR *wave,		//音源波形ﾃｰﾌﾞﾙｱﾄﾞﾚｽ
	const struct ENVE_HDR *enve,		//ｴﾝﾍﾞﾛｰﾌﾟ波形ﾃｰﾌﾞﾙｱﾄﾞﾚｽ
	const unsigned short enve_psn,		//ｴﾝﾍﾞﾛｰﾌﾟ処理ﾌﾟﾘｽｹｰﾙ値
	const struct PITCH_HDR *pitch,		//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形ﾃｰﾌﾞﾙｱﾄﾞﾚｽ
	const unsigned short pitch_psn,		//ﾋﾟｯﾁﾍﾞﾝﾄﾞ処理ﾌﾟﾘｽｹｰﾙ値
	const unsigned short velo)		//音量
{
	unsigned short n;
	unsigned char *p;

//DBG_C('\n');
//DBG_C('w');
//DBG_B(kb);
//DBG_L(wave);
//DBG_L(enve);
//DBG_L(pitch);
	if(kb>=KB_N) return;			//KB用ﾊﾟｰﾄ数を超えているときは戻る
//DBG_C('t');
	for(n=0,p=(unsigned char *)&part[PART_N+kb];n<sizeof(part[0]);n++,p++) *p=0;
						//先ずｸﾘｱしておく
	song.onka_kb_psn=kb_psn;		//KB演奏用音価ﾌﾟﾘｽｹｰﾙ値を設定する
//DBG_S(song.onka_kb_psn);
	part[PART_N+kb].ongen=wave;		//音源波形ﾃｰﾌﾞﾙｱﾄﾞﾚｽを設定する
//DBG_L(part[PART_N+kb].ongen);
	part[PART_N+kb].enve=enve;		//ｴﾝﾍﾞﾛｰﾌﾟ波形ﾃｰﾌﾞﾙｱﾄﾞﾚｽを設定する
//DBG_L(part[PART_N+kb].enve);
	part[PART_N+kb].enve_psn=enve_psn;	//ｴﾝﾍﾞﾛｰﾌﾟ処理実行のﾌﾟﾘｽｹｰﾙ値を設定する
//DBG_S(part[PART_N+kb].enve_psn);
	part[PART_N+kb].pitch=pitch;		//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形ﾃｰﾌﾞﾙｱﾄﾞﾚｽを設定する
//DBG_L(part[PART_N+kb].pitch);
	part[PART_N+kb].pitch_psn=pitch_psn;	//ﾋﾟｯﾁﾍﾞﾝﾄﾞ処理実行のﾌﾟﾘｽｹｰﾙ値を設定する
//DBG_S(part[PART_N+kb].pitch_psn);
	part[PART_N+kb].velo=velo*kb_vol[kb];	//音量を設定する
//DBG_B(part[PART_N+kb].velo);
	part[PART_N+kb].song_part=(const struct SONG_PART *)0xffff;
						//演奏中を設定する
//DBG_C('>');
}


//==============================================
//KB演奏停止処理(API関数)
//==============================================
static void KB_STOP(
	unsigned char kb)			//KB演奏を終了するﾊﾟｰﾄ番号(0起算)
						//制御表はpart[PART_N+kb]にｱｸｾｽする
{

//DBG_C('\n');
//DBG_C('p');
	if(kb>=KB_N) return;			//KB用ﾊﾟｰﾄ数を超えているときは戻る
	part[PART_N+kb].song_part=0;		//演奏終了を設定する
//DBG_C('>');
}


//==============================================
//KB演奏音符設定処理(API関数)
//==============================================
static void KB_ONPU(
	unsigned char kb,			//KB演奏に切替えるﾊﾟｰﾄ番号(0起算)
						//制御表はpart[PART_N+kb]にｱｸｾｽする
	unsigned short onpu)			//音符ｺｰﾄﾞ(0は演奏停止)
{

//DBG_C('\n');
//DBG_C('o');
//DBG_S(onpu);
	if(kb>=KB_N) return;			//KB用ﾊﾟｰﾄ数を超えているときは戻る
	part[PART_N+kb].onpu_kb=onpu;		//KB演奏用ﾊﾞｯﾌｧに音符ｺｰﾄﾞを設定する
	part[PART_N+kb].onka_zan=0;		//音価残をｸﾘｱしておく
//DBG_C('>');
}
#endif


#if PART_N+KB_N>0
//==============================================
//ｵﾙｺﾞｰﾙ及びKB演奏処理
//==============================================
static void song_exe(void)
{
	struct PART *part_pt;			//ﾊﾟｰﾄ個別制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀ
	unsigned char n,m,c,k;
	unsigned short u;
	short j,s;
	union					//8ﾋﾞｯﾄｼﾌﾄ演算の効率化
	{
		unsigned short sss;
		unsigned char bbb[2];
	} sssbbb;

//DBG_C('<');
	//初期化
	song.pcm=0;				//全ﾊﾟｰﾄ分のPWM値を0にしておく
	m=0;					//ｵﾙｺﾞｰﾙ演奏中のﾊﾟｰﾄ数をｸﾘｱしておく

#if PART_N>0
	//ｵﾙｺﾞｰﾙ音価ﾌﾟﾘｽｹｰﾗを更新
//DBG_S(song.onka_ps);
	if(song.onka_ps) song.onka_ps--;	//ｵﾙｺﾞｰﾙ音価ﾌﾟﾘｽｹｰﾗが残っていればﾃﾞｸﾘ
	else song.onka_ps=song.onka_psn;	//満了していたら初期化する
#endif

#if KB_N>0
	//KB音価ﾌﾟﾘｽｹｰﾗを更新
//DBG_S(song.onka_kb_ps);
	if(song.onka_kb_ps) song.onka_kb_ps--;	//KB音価ﾌﾟﾘｽｹｰﾗが残っていればﾃﾞｸﾘ
	else song.onka_kb_ps=song.onka_kb_psn;	//満了していたら初期化する
#endif

	//ﾊﾟｰﾄ演奏処理
	for(n=0,part_pt=part;n<PART_N+KB_N;n++,part_pt++)
						//ｵﾙｺﾞｰﾙとｷｰﾎﾞｰﾄﾞのﾊﾟｰﾄを繰返す
	{
//DBG_B(n);
//DBG_L(part_pt->song_part);
		if(!part_pt->song_part) continue;
						//演奏が終了しているﾊﾟｰﾄは飛ばす
#if PART_N>0
		if(n<PART_N) m++;		//ｵﾙｺﾞｰﾙ演奏中のﾊﾟｰﾄ数をｲﾝｸﾘ
#endif

		//音符を取出す
//DBG_C('t');
		while(!part_pt->onka_zan)	//音価残が無くなったら次の音符へ進む
						//　通常音符が現れるか､音符終了まで繰返す
						//　つまり､制御音符は一気に処理する
		{
//DBG_C('\n');
//DBG_C('>');
//DBG_B(n);
//DBG_C('+');
#if PART_N>0
			if(n<PART_N)		//ｵﾙｺﾞｰﾙ演奏のとき
			{
//DBG_C('o');
//DBG_L(part_pt->song_part->onpu);
//DBG_S(part_pt->onpu_of);
				part_pt->onpu_cd=*(part_pt->onpu+part_pt->onpu_of++);
						//音符ｺｰﾄﾞを取得し､音符ｽﾃｯﾌﾟを進めておく
			}
#endif
#if KB_N>0
			if(n>=PART_N)		//KB演奏のとき
			{
//DBG_C('k');
				part_pt->onpu_cd=part_pt->onpu_kb;
						//KB演奏用ﾊﾞｯﾌｧから音符ｺｰﾄﾞを取出す
				part_pt->onpu_kb=0;
						//KB演奏用ﾊﾞｯﾌｧをｸﾘｱしておく
			}
#endif
//DBG_S(part_pt->onpu_cd);
			part_pt->ongen_tobi=0;	//ｻﾝﾌﾟﾙ飛び数を0にしておく
//DBG_S(part_pt->ongen_tobi);
			part_pt->port_tobi_ps=0;//ﾎﾟﾙﾀﾒﾝﾄｻﾝﾌﾟﾙ飛び数増減量を0にしておく
//DBG_S(part_pt->port_tobi_ps);
			if(part_pt->onpu_cd&0x2000)
						//機能ｺｰﾄﾞ2のとき
			{
#if PART_N>0
				if(part_pt->onpu_cd&0x1000)
						//ﾘﾋﾟｰﾄ分岐以外のとき
				{
					if(part_pt->onpu_cd&0x0800)
							//波形切換えのとき
					{
//DBG_C('w');
						switch(part_pt->onpu_cd&0x0700)
						{
						case 0x0000:	//音源波形のとき
//DBG_C('o');
							part_pt->ongen=(struct WAVE_HDR *)
								((unsigned long)
								*(part_pt->onpu+(part_pt->onpu_of++))+
								SONG_IDX_BASE);
								//音源波形ｱﾄﾞﾚｽを取得し､
								//　音符ｽﾃｯﾌﾟを進めておく
//DBG_L(part_pt->ongen);
							break;
						case 0x0100:	//ｴﾝﾍﾞﾛｰﾌﾟ波形のとき
//DBG_C('e');
							part_pt->enve=(struct ENVE_HDR *)
								((unsigned long)
								*(part_pt->onpu+(part_pt->onpu_of++))+
								SONG_IDX_BASE);
								//ｴﾝﾍﾞﾛｰﾌﾟ波形ｱﾄﾞﾚｽを取得し､
								//　音符ｽﾃｯﾌﾟを進めておく
//DBG_L(part_pt->enve);
							break;
						case 0x0200:	//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形のとき
//DBG_C('p');
							part_pt->pitch=(struct PITCH_HDR *)
								((unsigned long)
								*(part_pt->onpu+(part_pt->onpu_of++))+
								SONG_IDX_BASE);
								//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形ｱﾄﾞﾚｽを取得し､
								//　音符ｽﾃｯﾌﾟを進めておく
//DBG_L(part_pt->pitch);
							break;
						}
					}
					else if(part_pt->onpu_cd&0x0400)
							//各種値設定のとき
					{
//DBG_C('k');
						switch(part_pt->onpu_cd&0x0300)
						{
						case 0x0000:	//ﾍﾞﾛｼﾃｨ設定のとき
//DBG_C('v');
							part_pt->velo=part_pt->onpu_cd&0x00ff;
//DBG_S(part_pt->velo);
							break;
						case 0x0100:	//ｴﾝﾍﾞﾛｰﾌﾟﾀｲﾑ設定のとき
//DBG_C('e');
							part_pt->enve_psn=
								part_pt->song_part->enve_psn*
								(part_pt->onpu_cd&0x00ff);
//DBG_S(part_pt->enve_psn);
							break;
						case 0x0200:	//ﾃﾝﾎﾟ設定のとき
//DBG_C('t');

							song.onka_psn=
								(song.song_hdr->onka_psn*
								(part_pt->onpu_cd&0x00ff))>>7;
//DBG_S(song.onka_psn);
							break;
						case 0x0300:	//ﾋﾟｯﾁﾍﾞﾝﾄﾞﾀｲﾑ設定のとき
//DBG_C('\n');
//DBG_C('p');
							part_pt->pitch_psn=
								part_pt->song_part->pitch_psn*
								(part_pt->onpu_cd&0x00ff);
//DBG_L(part_pt->pitch_psn);
							part_pt->pitch_ps=0;
//DBG_S(part_pt->pitch_ps);
							part_pt->pitch_of=0;
//DBG_S(part_pt->pitch_of);
							break;												}
					}
					else		//無条件分岐のとき
					{
//DBG_C('j');
						part_pt->onpu_of=part_pt->onpu_cd&0x03ff;
							//分岐先を設定する
//DBG_S(part_pt->onpu_of);
					}
				}
				else		//ﾘﾋﾟｰﾄ分岐のとき
				{
//DBG_C('r');
					c=(part_pt->onpu_cd&0x0c00)>>10;
							//ﾘﾋﾟｰﾄ番号を得る
//DBG_B(c);
//DBG_B(part_pt->rpt[c]);
					if(part_pt->rpt[c])
							//ﾘﾋﾟｰﾄ回数残があるとき
					{
//DBG_C('z');
						part_pt->rpt[c]--;
							//ﾘﾋﾟｰﾄ回数残をﾃﾞｸﾘする
//DBG_B(part_pt->rpt[c]);
						part_pt->onpu_of=part_pt->onpu_cd&0x03ff;
							//分岐先を設定する
//DBG_S(part_pt->onpu_of);
					}
				}
#endif
			}
			else if(part_pt->onpu_cd&0x1f00)
						//演奏音符のとき
			{
//DBG_C('\n');
//DBG_S(part_pt->onpu_cd);
				sssbbb.sss=part_pt->onpu_cd;
						//音符の音価ｺｰﾄﾞのﾋﾞｯﾄ分離
				k=(sssbbb.bbb[1]&0x1f)-1;
						//音価ｺｰﾄﾞ(0起算)を得る
//DBG_B(k);
				part_pt->onka_zan=onka_tbl[k];
						//音価残を設定する	
//DBG_B(part_pt->onka_zan);
				if(part_pt->onpu_cd&0x003f)
						//発音音符のとき
				{
//DBG_C('\n');
					if(part_pt->onpu_cd&0x4000)
							//効果音発声のとき
					{
//DBG_C('e');
						part_pt->efct_tobi=part_pt->onpu_cd&0x00ff;
								//ｻﾝﾌﾟﾙ飛び数を設定
//DBG_S(part_pt->efct_tobi);
						part_pt->efct_of=0;
								//ｻﾝﾌﾟﾙｵﾌｾｯﾄを初期化する
//DBG_L(part_pt->efct_of);
//DBG_S(part_pt->onpu_of);
						part_pt->efct=(struct EFCT_HDR *)
							((unsigned long)
							*(part_pt->onpu+(part_pt->onpu_of++))+
							SONG_IDX_BASE);
								//効果音波形ｱﾄﾞﾚｽを取得し､
								//　音符ｽﾃｯﾌﾟを進める
//DBG_L(part_pt->efct);
//DBG_S(part_pt->efct->smp);
//DBG_L(part_pt->efct);
					}
					else		//通常演奏のとき
					{
				
//DBG_B(song.key);
						c=((part_pt->onpu_cd&0x003f)+song.key)&0x3f;
							//音高に移調度を加味して､範囲内に規制する
//DBG_B(c);
						c=onkou_mei_tbl[c];
							//ｵｸﾀｰﾌﾞ番号と音名ｺｰﾄﾞを得る
//DBG_B(c);
						u=onkou_tbl[c&0x0f];
							//音名のｻﾝﾌﾟﾙ飛び数を得る
						if(c>>=4) u>>=c;
							//ｵｸﾀｰﾌﾞ番号でｼﾌﾄする
						part_pt->ongen_tobi=u;
							//音源飛び数を設定
//DBG_S(part_pt->ongen_tobi);
						if(part_pt->onpu_cd&0x0080)
							//ﾎﾟﾙﾀﾒﾝﾄのとき
						{
//DBG_C('P');
							k=port_tbl[k];
								//音価ｺｰﾄﾞから音源飛び数増減量率を得る
//DBG_B(k);
							u=*(part_pt->onpu+part_pt->onpu_of);
								//次の音符ｺｰﾄﾞを取得する
//DBG_S(u);
							if((u&0xff80)==0x0080) part_pt->onpu_of++;
								//次の音符がﾎﾟﾙﾀﾒﾝﾄ終着のときは
								//　音符ﾃﾞｰﾀのｵﾌｾｯﾄを進めておく
//DBG_S(part_pt->onpu_of);
							if(k&&!(u&0xe000)&&(u&0x1f00||u&0x0080)&&u&0x003f)
								//音源飛び数増減量率が有効且つ､
								//　次の音符の音高が有効のとき
							{
								c=((u&0x003f)+song.key)&0x3f;
									//音高に移調度を加味して､
									//　範囲内に規制する
//DBG_B(c);
								c=onkou_mei_tbl[c];
									//ｵｸﾀｰﾌﾞ番号と音名ｺｰﾄﾞを得る
//DBG_B(c);
								u=onkou_tbl[c&0x0f];
									//音名のｻﾝﾌﾟﾙ飛び数を得る
								if(c>>=4) u>>=c;
									//ｵｸﾀｰﾌﾞ番号でｼﾌﾄする
								part_pt->port_tobi_gl=u;
									//ﾎﾟﾙﾀﾒﾝﾄの飛び数目標値を設定
//DBG_S(part_pt->port_tobi_gl);
								if(part_pt->port_tobi_gl>part_pt->ongen_tobi)
									//飛び数増加のとき
								{
//DBG_C('U');
									part_pt->port_tobi_ps=
										(unsigned long)
										(part_pt->port_tobi_gl-
										part_pt->ongen_tobi)*k/4096;
								}
								else	//飛び数減少のとき
								{
//DBG_C('D');
									part_pt->port_tobi_ps=
										(unsigned long)
										(part_pt->ongen_tobi-
										part_pt->port_tobi_gl)*k/4096;
								}
//DBG_S(part_pt->port_tobi_ps);
							}
						}
					}
				}
				if(!(part_pt->onpu_cd&0x0040))
						//ﾀｲ･ｽﾗｰでないとき
				{
//DBG_C('c');
					part_pt->enve_of=0;
						//ｴﾝﾍﾞﾛｰﾌﾟ波形ﾃｰﾌﾞﾙｵﾌｾｯﾄをｸﾘｱ
					part_pt->enve_ps=0;
						//ｴﾝﾍﾞﾛｰﾌﾟ処理対応のﾌﾟﾘｽｹｰﾗを初期化
					part_pt->pitch_of=0;
						//ﾋﾟｯﾁﾍﾞﾝﾄﾞ波形ﾃｰﾌﾞﾙｵﾌｾｯﾄをｸﾘｱ
					part_pt->pitch_ps=0;
						//ﾋﾟｯﾁﾍﾞﾝﾄﾞ処理対応のﾌﾟﾘｽｹｰﾗを初期化
				}
			}
#if PART_N>0
			else if(part_pt->onpu_cd&0x0040)
						//ﾘﾋﾟｰﾄ回数設定のとき
			{
//DBG_C('r');
				c=(part_pt->onpu_cd&0x0030)>>4;
						//ﾘﾋﾟｰﾄ番号を得る
//DBG_B(c);
				part_pt->rpt[c]=part_pt->onpu_cd&0x000f;
//DBG_B(part_pt->rpt[c]);
			}
			else if(part_pt->onpu_cd&0x0020)
						//転調設定のとき
			{
//DBG_C('i');
				if((c=part_pt->onpu_cd&0x001f)==0)
					song.key=song.song_hdr->key;
							//増減値が0のときは
							//　ｿﾝｸﾞﾍｯﾀﾞのｷｰに戻す
				else
				{			//増減値が0でなければ
					song.key+=c;	//ｶﾚﾝﾄのｷｰから増減する
					if(c>15) song.key-=32;
							//負数のときに是正する
				}
//DBG_B(song.key);
			}
#endif
			else break;		//演奏終了のとき
		}

		//音符の発音
//DBG_C('h');
//DBG_B(part_pt->onka_zan);
		if(part_pt->onka_zan)		//音価残があれば(音符終了でなければ)
						//　演奏処理を実行する
		{

			//音価を更新
//DBG_C('a');
//DBG_S(song.onka_ps);
			if(((n<PART_N)&&(!song.onka_ps))||
				((n>=PART_N)&&(!song.onka_kb_ps)))

						//ｵﾙｺﾞｰﾙ演奏で音価ﾌﾟﾘｽｹｰﾗが満了､又は
						//　KB演奏でKB用音価ﾌﾟﾘｽｹｰﾗが満了していたら
			{
				part_pt->onka_zan--;
						//音価残をﾃﾞｸﾘする
//DBG_B(part_pt->onka_zan);

				//ﾎﾟﾙﾀﾒﾝﾄ処理
				if(part_pt->port_tobi_ps)
						//ﾎﾟﾙﾀﾒﾝﾄ演奏のとき
				{
//DBG_C('p');
//DBG_S(part_pt->ongen_tobi);
					if(part_pt->port_tobi_gl>part_pt->ongen_tobi)
						//飛び数増加のとき
					{
//DBG_C('u');
						if((part_pt->ongen_tobi+=part_pt->port_tobi_ps)>
							part_pt->port_tobi_gl)
							part_pt->ongen_tobi=part_pt->port_tobi_gl;
						//飛び数を増加して､目標値に規制する
					}
					else	//飛び数減少のとき
					{
//DBG_C('d');
						if((part_pt->ongen_tobi-=part_pt->port_tobi_ps)<
							part_pt->port_tobi_gl)
							part_pt->ongen_tobi=part_pt->port_tobi_gl;
						//飛び数を減少して､目標値に規制する
					}
//DBG_S(part_pt->ongen_tobi);
				}
			}

			//発音処理
//DBG_C('h');
//DBG_S(part_pt->onpu_cd);
			if(part_pt->onpu_cd&0x003f)
						//発音音符のとき
			{

				//ｴﾝﾍﾞﾛｰﾌﾟ処理
//DBG_C('\n');
//DBG_C('e');
//DBG_S(part_pt->enve_psn);
				if(part_pt->enve_psn)
						//ｴﾝﾍﾞﾛｰﾌﾟが設定されているときは
						//　ｴﾝﾍﾞﾛｰﾌﾟ処理を行う
				{
//DBG_C('s');
//DBG_S(part_pt->enve_ps);
					if(!part_pt->enve_ps)
						//ｴﾝﾍﾞﾛｰﾌﾟ処理のﾀｲﾐﾝｸﾞのとき
					{
//DBG_C('t');
//DBG_S(part_pt->velo);
//DBG_B(part_pt->enve_of);
//DBG_B(part_pt->enve->data[part_pt->enve_of]);
						part_pt->velo_enve=((unsigned short)
							part_pt->velo*
							part_pt->enve->
							data[part_pt->enve_of])/256;
						//ｴﾝﾍﾞﾛｰﾌﾟ処理後のﾍﾞﾛｼﾃｨ値を求める
//DBG_S(part_pt->velo_enve);

//DBG_B(part_pt->enve->smp);
						//ｻﾝﾌﾟﾙｵﾌｾｯﾄを進める
						if(part_pt->enve_of<
							part_pt->enve->smp)
							part_pt->enve_of++;
							//ｻﾝﾌﾟﾙｵﾌｾｯﾄを進めて､最後に規制する
//DBG_B(part_pt->enve_of);
					}
					if(part_pt->enve_ps++>part_pt->enve_psn)
					{
						//ｴﾝﾍﾞﾛｰﾌﾟのﾌﾟﾘｽｹｰﾗをｲﾝｸﾘして満了したら
						part_pt->enve_ps=0;
							//ｴﾝﾍﾞﾛｰﾌﾟのﾌﾟﾘｽｹｰﾗをｸﾘｱ
					}
				}
				else		//ｴﾝﾍﾞﾛｰﾌﾟが設定されていないときは
				{
					part_pt->velo_enve=part_pt->velo;
						//　ﾍﾞﾛｼﾃｨ設定値をそのまま使う
				}
//DBG_S(part_pt->velo_enve);

				//音源ｻﾝﾌﾟﾙ値を得る
//DBG_C('\n');
//DBG_C('w');
				if(part_pt->onpu_cd&0x4000)
						//効果音発声のとき
				{
//DBG_C('e');
//DBG_L(part_pt->efct_of);
					s=(short)(part_pt->efct->data[
						(unsigned short)(part_pt->efct_of/256)])-128;
							//効果音ｻﾝﾌﾟﾙ値を得る
//DBG_S(s);
					if((part_pt->efct_of+=part_pt->efct_tobi)>
						part_pt->efct->smp*256)
							//ｵﾌｾｯﾄを進めて､ｻﾝﾌﾟﾙ数を超えたら
						part_pt->efct_of-=part_pt->efct_tobi;
							//　戻す
//DBG_L(part_pt->efct_of);
				}
				else		//通常演奏のとき
				{



//DBG_S(part_pt->ongen_of);
					sssbbb.sss=part_pt->ongen_of;
//DBG_B(sssbbb.bbb[1]);
					s=(short)(part_pt->ongen->data[sssbbb.bbb[1]])-128;
							//音源ｻﾝﾌﾟﾙ値を得る
//DBG_C('\n');
//DBG_S(s);
//DBG_C('o');
//DBG_S(part_pt->ongen_of);
//DBG_S(part_pt->ongen_tobi);
					part_pt->ongen_of+=part_pt->ongen_tobi;
							//波形ﾃﾞｰﾀのｵﾌｾｯﾄを進めて､
							//　ｻﾝﾌﾟﾙ数は256固定のため
							//　ｵｰﾊﾞﾌﾛｰは無視する
//DBG_S(part_pt->ongen_of);

					//ﾋﾟｯﾁﾍﾞﾝﾄﾞ処理
//DBG_C('\n');
//DBG_C('p');
					if(part_pt->pitch_psn)
							//ﾋﾟｯﾁﾍﾞﾝﾄﾞが設定されているときは
							//　ﾋﾟｯﾁﾍﾞﾝﾄﾞ処理を行う
					{
//DBG_C('\n');
//DBG_S(part_pt->pitch_ps);
						if(!part_pt->pitch_ps)
							//ﾋﾟｯﾁﾍﾞﾝﾄﾞ処理のﾀｲﾐﾝｸﾞのとき
						{
//DBG_C('\n');
//DBG_C('p');
							j=part_pt->pitch->data
								[part_pt->pitch_of];
								//ﾋﾟｯﾁﾍﾞﾝﾄﾞｻﾝﾌﾟﾙ値を得る
//DBG_S(j);
							if(j>128) j-=256;
								//signedにする
//DBG_S(j);
//DBG_S(part_pt->ongen_tobi);
							part_pt->ongen_tobi=
								(signed)part_pt->ongen_tobi+j;
								//音源ｻﾝﾌﾟﾙ飛び数を増減する
//DBG_S(part_pt->ongen_tobi);
							if(++part_pt->pitch_of>
								part_pt->pitch->smp)
								part_pt->pitch_of--;
								//ｻﾝﾌﾟﾙｵﾌｾｯﾄを進めて､最後に規制する
//DBG_S(part_pt->pitch_of);
						}
						if(++part_pt->pitch_ps>=part_pt->pitch_psn)
							part_pt->pitch_ps=0;
								//ﾌﾟﾘｽｹをｲﾝｸﾘして､満了したら
								//　初期化する
//DBG_S(part_pt->pitch_ps);
					}
				}

				//PWMﾃﾞｭｰﾃｨ値を合算
				s*=part_pt->velo_enve;	//ﾍﾞﾛｼﾃｨを加味して､16ﾋﾞｯﾄPCMｽｹｰﾙに変換
//DBG_C('-');
//DBG_S(s);
//DBG_C('-');
//DBG_S(part_pt->velo_enve);
				song.pcm+=s;	//全ﾊﾟｰﾄ分のPWMﾃﾞｭｰﾃｨ値に､
						//　このﾊﾟｰﾄ分を加算する
//DBG_S(song.pcm);
			}
		}

		else				//音符終了のとき
		{
			//ﾊﾟｰﾄ終了処理
//DBG_C('t');
			if(n<PART_N)		//ｵﾙｺﾞｰﾙ演奏のときは
			{
				part_pt->song_part=0;
						//ﾊﾟｰﾄ演奏終了を表示する
			}
		}
	}
#if PART_N>0
	if(song.no)				//ｵﾙｺﾞｰﾙ演奏していたとき
	{
//DBG_B(m);
		if(!m)				//全ﾊﾟｰﾄが終了したら
		{
//DBG_L(song.song_hdr->next);
			if(song.song_hdr->next)	//連続演奏するとき
			{
//DBG_C('r');
				song.song_hdr=(struct SONG_HDR *)
					(song.song_hdr->next+SONG_IDX_BASE);
						//曲ﾍｯﾀﾞへのﾎﾟｲﾝﾀを取得
//DBG_L(song.song_hdr);
//DBG_D(song.song_hdr,64);
				song_init();	//曲ﾃﾞｰﾀを初期化
			}
			else			//単曲演奏のとき
			{
//DBG_C('s');
				song.no=0;	//曲演奏終了の表示
				song.pcm=0;	//全ﾊﾟｰﾄ分のPCM値をｸﾘｱしておく
				CB_SONG_END();	//演奏終了時ｺｰﾙﾊﾞｯｸ関数を呼ぶ
			}
		}
	}
#endif
//DBG_C('>');
}
#endif


#if (defined(VOICEN_IMA)&&VOICE_N>0)||(defined(VOICEI_IMA)&&VOICE_I>0)||\
	(defined(VOICES_IMA)&&VOICE_S>0)
//==============================================
//IMA-ADPCM制御ﾃｰﾌﾞﾙ
//==============================================
//ｽﾃｯﾌﾟｻｲｽﾞﾃｰﾌﾞﾙ(89段階)
static const unsigned short STEP_TABLE[89]=
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
static const short INDEX_TABLE[8]=
{
	-1,-1,-1,-1,2,4,6,8
};


//==============================================
//IMA-ADPCMﾃﾞｺｰﾄﾞ処理
//==============================================
static unsigned short ima_decode(		//ﾃﾞｺｰﾄﾞ値(16ﾋﾞｯﾄPCM値)を返す
						//　0ﾚﾍﾞﾙは32768
	struct IMA *ima,			//
	unsigned char nibble)			//ｴﾝｺｰﾄﾞ値(4ﾋﾞｯﾄADPCM値)
{
	unsigned short delta,step;

//DBG_C('\n');
//DBG_B(nibble);
	//ﾃﾞｺｰﾄﾞの初期化
//DBG_C('-');
//DBG_S(ima->step_index);
	step=STEP_TABLE[ima->step_index];
//DBG_C('-');
//DBG_S(step);
	//予測値を更新
	delta=step>>3;
	if(nibble&4) delta+=step;
	step>>=1;
	if(nibble&2) delta+=step;
	step>>=1;
	if(nibble&1) delta+=step;
//DBG_C('-');
//DBG_S(delta);
	if(nibble&8)
	{
		if(ima->predicted<delta) ima->predicted=0;
		else ima->predicted-=delta;
	}
	else
	{
		if((65535-ima->predicted)<delta) ima->predicted=65535;
		else ima->predicted+=delta;
	}
//DBG_C('=');
//DBG_S(ima->predicted);

	//step_indexを更新
	ima->step_index+=INDEX_TABLE[nibble&7];
	if(ima->step_index<0) ima->step_index=0;
	else if(ima->step_index>88) ima->step_index=88;
//DBG_C('-');
//DBG_S(ima->step_index);

	return(ima->predicted);
}
#endif


#if VOICE_N>0
//==============================================
//音声N選択処理(API関数)
//==============================================
static void VOICEN_SEL(				//戻り値無し
	unsigned char ch,			//ch番号
	unsigned char no)			//音声番号(0は再生中止)
{
	struct VOICEN *voiceN_pt;		//音声N再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀ

//DBG_C('\n');
//DBG_C('s');
//DBG_B(ch);
//DBG_B(no);
//DBG_D(voiceN_idx,sizeof(voiceN_idx));
//DBG_C('i');
	voiceN_pt=voiceN+ch;			//音声N再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀを設定
	if(no)					//再生開始のとき
	{
		if(voiceN_pt->no==no) return;	//発声中の音声番号と同じ場合は戻る
		if(no>sizeof(voiceN_idx)/sizeof(voiceN_idx[0])) return;
						//実装数を超えているときは戻る
		voiceN_pt->no=no--;		//音声番号を設定
		voiceN_pt->begin=(unsigned long)voiceN_idx[no].adr;
						//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを設定
//DBG_L(voiceN_pt->begin);
		voiceN_pt->end=voiceN_pt->begin+voiceN_idx[no].len;
						//音声ﾃﾞｰﾀの終了ｱﾄﾞﾚｽを設定
//DBG_L(voiceN_pt->end);
		voiceN_pt->of=0x0100;		//音声ﾃﾞｰﾀのｵﾌｾｯﾄを初期化しておく
						//(初回処理でサンプルを取出すように)
//DBG_S(voiceN_pt->of);
		voiceN_pt->pcm=0;		//PCM値をｸﾘｱしておく
//DBG_S(voiceN_pt->pcm);
		voiceN_pt->tobi=voiceN_idx[no].tobi;
						//再生処理のｻﾝﾌﾟﾙ飛び数[1/256]を設定
						//(音声ﾍｯﾀﾞには[1/256]の精度で格納されている)
//DBG_S(voiceN_pt->tobi);
#ifdef VOICEN_IMA			//IMA-ADPCMを実装するとき
		voiceN_pt->tobi>>=1;		//再生処理のｻﾝﾌﾟﾙ飛び数をﾆﾌﾞﾙ単位にする
//DBG_S(voiceN_pt->tobi);
		voiceN_pt->ima.predicted=32768;	//予測値を初期化
		voiceN_pt->ima.step_index=0;	//ｽﾃｯﾌﾟｲﾝﾃﾞｯｸｽを初期化
#endif
	}
	else					//再生中断のとき
	{
		voiceN_pt->no=0;		//再生終了にする
		voiceN_pt->pcm=0;		//PCM値をｸﾘｱしておく
	}
//DBG_D(voiceN_pt,sizeof(voiceN[0]));
}


//==============================================
//音声N再生処理ｻﾝﾌﾟﾙﾚｰﾄ設定(API関数)
//==============================================
static void VOICEN_SMP(				//戻り値無し
	unsigned char ch,			//ch番号
	unsigned smp)				//再生処理ｻﾝﾌﾟﾙﾚｰﾄ[sps]
{
//DBG_C('\n');
//DBG_C('f');

	voiceN[ch].tobi=(unsigned long)256*smp*PWM_PR/1000000;
//DBG_S(voiceN[ch].tobi);
}


//==============================================
//音声N再生処理
//==============================================
static void voiceN_exe(
	unsigned char ch)			//ch番号
{
	struct VOICEN *voiceN_pt;		//音声N再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀ
#ifdef VOICEN_IMA			//IMA-ADPCMｴﾝｺｰﾄﾞのとき
	static unsigned char data;		//ｻﾝﾌﾟﾙﾃﾞｰﾀ
#endif

//DBG_C('\n');
//DBG_C('p');
//DBG_B(ch);
	voiceN_pt=voiceN+ch;			//音声N再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀを設定する
	if(voiceN_pt->begin>=voiceN_pt->end)	//音声ﾃﾞｰﾀが終わったとき
	{
//DBG_C('e');
		voiceN_pt->no=0;		//再生終了にする
		voiceN_pt->pcm=0;		//PCM値をｸﾘｱしておく
		CB_VOICEN_END(ch);		//音声再生終了処理(ｺｰﾙﾊﾞｯｸ関数)を呼出す
		return;
	}

#ifdef VOICEN_IMA			//IMA-ADPCMｴﾝｺｰﾄﾞのとき
	if(voiceN_pt->of&0x0100)		//ｵﾌｾｯﾄ計算の整数部に桁上がりしたら
	{
		voiceN_pt->of=voiceN_pt->of&0x00ff|0x8000;
						//ｵﾌｾｯﾄの整数部をｸﾘｱして､
						//　先行ﾆﾌﾞﾙ処理済みにする
		data=*(unsigned char *)(voiceN_pt->begin);
						//ｻﾝﾌﾟﾙを取出す
		voiceN_pt->pcm=ima_decode(&voiceN_pt->ima,data&0x0f)-32768;
						//先行ﾆﾌﾞﾙをIMA-ADPCMﾃﾞｺｰﾄﾞする

//DBG_C('a');
//DBG_C('=');
//DBG_S(voiceN_pt->pcm);
		voiceN_pt->pcm=(long)voiceN_pt->pcm*voiceN_vol[ch]/256;
						//音量配分を乗じる
//DBG_C('-');
//DBG_S(voiceN_pt->pcm);
	}
	else if((voiceN_pt->of&0x8080)==0x8080)	//後行ﾆﾌﾞﾙになったら
	{
		voiceN_pt->of&=0x00ff;		//ｵﾌｾｯﾄの整数部をｸﾘｱして､
						//　後行ﾆﾌﾞﾙ処理済みにする
		voiceN_pt->pcm=ima_decode(&voiceN_pt->ima,data>>4)-32768;
						//後行ﾆﾌﾞﾙをIMA-ADPCMﾃﾞｺｰﾄﾞする
//DBG_C('b');
//DBG_C('=');
//DBG_S(voiceN_pt->pcm);
		voiceN_pt->pcm=(long)voiceN_pt->pcm*voiceN_vol[ch]/256;
						//音量配分を乗じる
//DBG_C('-');
//DBG_S(voiceN_pt->pcm);
		voiceN_pt->begin++;		//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを進める
//DBG_L(voiceN_pt->begin);
	}

#else					//8ﾋﾞｯﾄPCMｺｰﾄﾞのとき
	if(voiceN_pt->of&0x0100)		//ｵﾌｾｯﾄ計算の整数部に桁上がりしたら
	{
		voiceN_pt->of&=0x00ff;		//ｵﾌｾｯﾄの整数部をｸﾘｱしておく
			voiceN_pt->pcm=*(unsigned char *)(voiceN_pt->begin)-128;
						//ｻﾝﾌﾟﾙを取り出して､
						//　8ﾋﾞｯﾄPCM形式をsignedに変換
//DBG_C('\n');
//DBG_S(voiceN_pt->pcm);
//DBG_S(voiceN_vol[ch]);
		voiceN_pt->pcm*=voiceN_vol[ch];	//音量配分を乗じて､16ﾋﾞｯﾄPCMｽｹｰﾙに変換
//DBG_S(voiceN_pt->pcm);
		voiceN_pt->begin++;		//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを進める
//DBG_L(voiceN_pt->begin);
	}
#endif

	voiceN_pt->of+=voiceN_pt->tobi;	//音声ﾃﾞｰﾀのｵﾌｾｯﾄを進める
//DBG_S(voiceN_pt->of);
}
#endif


#if VOICE_I>0
//==============================================
//音声I選択処理(API関数)
//==============================================
static void VOICEI_SEL(				//戻り値無し
	unsigned short no)			//音声番号(0は再生中止)
{

//DBS_H;
//DBG_C('\n');
//DBG_C('s');
//DBG_S(no);
//DBG_D(voiceI_idx,sizeof(voiceI_idx));
//DBG_C('i');
	if(voiceI.no!=no)			//発声中の音声番号と異なるときは
	{
		if(voiceI.no)			//再生中のときは
		{
			i2c_read_last();	//I2C読込み(最後のﾃﾞｰﾀ)
			i2c_end();		//I2C終了
			voiceI.no=0;		//再生停止にする
			voiceI.pcm=0;		//PCM値をｸﾘｱしておく
		}
	}

	if(no)					//再生開始のとき
	{
		if(no>sizeof(voiceI_idx)/sizeof(voiceI_idx[0])) return;
						//実装数を超えているときは戻る
		if(voiceI.no==no) return;	//発声中の音声番号と同じ場合は戻る
		voiceI.no=no--;			//音声番号を設定して､番号を0起算にする
		voiceI.begin=voiceI_idx[no].adr;//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを設定
//DBG_L(voiceI.begin);
		voiceI.end=voiceI.begin+voiceI_idx[no].len;
						//音声ﾃﾞｰﾀの終了ｱﾄﾞﾚｽを設定
//DBG_L(voiceI.end);
			voiceI.of=0x0100;	//音声ﾃﾞｰﾀのｵﾌｾｯﾄを初期化しておく
						//(初回処理でｻﾝﾌﾟﾙを取出すように)
//DBG_S(voiceI.of);
		voiceI.of=255;			//再生開始時にﾃﾞｰﾀを取得するようにしておく
//DBG_S(voiceI.of);
		voiceI.pcm=0;			//PCM値をｸﾘｱしておく
//DBG_S(voiceI.pcm);
		voiceI.tobi=voiceI_idx[no].tobi;//再生処理のｻﾝﾌﾟﾙ飛び数[1/256]を設定
						//(音声ﾍｯﾀﾞには[1/256]の精度で格納されている)
//DBG_S(voiceI.tobi);
#ifdef VOICEI_IMA			//IMA-ADPCMを実装するとき
		voiceI.tobi>>=1;		//再生処理のｻﾝﾌﾟﾙ飛び数をﾆﾌﾞﾙ単位にする
//DBG_S(voiceI.tobi);
		voiceI.ima.predicted=32768;	//予測値を初期化
		voiceI.ima.step_index=0;	//ｽﾃｯﾌﾟｲﾝﾃﾞｯｸｽを初期化
#endif
		voiceI.sla=voiceI_idx[no].sla;	//I2Cｽﾚｰﾌﾞｱﾄﾞﾚｽを設定
//DBG_B(voiceI.sla);
		i2c_begin(voiceI.sla,voiceI.begin);
						//I2C開始
	}
//DBG_D(&voiceI,sizeof(voiceI));
//DBS_L;
}


//==============================================
//音声I再生処理ｻﾝﾌﾟﾙﾚｰﾄ設定(API関数)
//==============================================
static void VOICEI_SMP(				//戻り値無し
	unsigned smp)				//再生処理ｻﾝﾌﾟﾙﾚｰﾄ[sps]
{
//DBG_C('\n');
//DBG_C('p');

	voiceI.tobi=(unsigned long)256*smp*PWM_PR/1000000;
//DBG_S(voiceI.tobi);
}


//==============================================
//音声I再生処理
//==============================================
static void voiceI_exe(void)
{
	static unsigned char data;

//DBG_C('\n');
//DBG_C('p');
//DBG_L(voiceI.begin);
	if(voiceI.begin>=voiceI.end)		//音声ﾃﾞｰﾀが終わったとき
	{
		i2c_end();			//I2C終了
//DBG_C('i');
		voiceI.no=0;			//再生終了にする
		voiceI.pcm=0;			//PCM値をｸﾘｱしておく
		CB_VOICEI_END();		//音声再生終了処理(ｺｰﾙﾊﾞｯｸ関数)を呼出す
		return;
	}
//DBG_C('j');
#ifdef VOICEI_IMA			//IMA-ADPCMｴﾝｺｰﾄﾞのとき
//DBG_S(voiceI.of);
	if(voiceI.of&0x0100)			//ｵﾌｾｯﾄ計算の整数部に桁上がりしたら
	{
		voiceI.of=voiceI.of&0x00ff|0x8000;
						//ｵﾌｾｯﾄの整数部をｸﾘｱして､
						//　先行ﾆﾌﾞﾙ処理済みにする
//DBG_S(voiceI.of);
		if(++voiceI.begin>=voiceI.end)	//音声ﾃﾞｰﾀが終わるとき
		{
			data=i2c_read_last();	//I2C読込み(最終ﾃﾞｰﾀ)
		}
		else				//音声ﾃﾞｰﾀが途中のとき
		{
			data=i2c_read();	//I2C読込み
		}
//DBG_B(data);
		voiceI.pcm=ima_decode(&voiceI.ima,data&0x0f)-32768;
						//先行ﾆﾌﾞﾙをIMA-ADPCMﾃﾞｺｰﾄﾞする
//DBG_C('a');
//DBG_C('=');
//DBG_S(voiceI.pcm);
		voiceI.pcm=(long)voiceI.pcm*(VOICEI_VOL*256/100)/256;
						//音量配分を乗じる
//DBG_C('-');
//DBG_S(voiceI.pcm);
	}
	else if((voiceI.of&0x8080)==0x8080)	//後行ﾆﾌﾞﾙになったら
	{
		voiceI.of&=0x00ff;		//ｵﾌｾｯﾄの整数部をｸﾘｱして､
						//　後行ﾆﾌﾞﾙ処理済みにする
		voiceI.pcm=ima_decode(&voiceI.ima,data>>4)-32768;
						//後行ﾆﾌﾞﾙをIMA-ADPCMﾃﾞｺｰﾄﾞする
//DBG_C('b');
//DBG_C('=');
//DBG_S(voiceI.pcm);
		voiceI.pcm=(long)voiceI.pcm*(VOICEI_VOL*256/100)/256;
						//音量配分を乗じる
//DBG_S(voiceI.pcm);
	}

#else					//8ﾋﾞｯﾄPCMｺｰﾄﾞのとき
//DBG_S(voiceI.of);
	if(voiceI.of&0x0100)			//ｵﾌｾｯﾄ計算の整数部に桁上がりしたら
	{
		voiceI.of&=0x00ff;		//ｵﾌｾｯﾄの整数部をｸﾘｱしておく
//DBG_S(voiceI.of);
		if(++voiceI.begin>=voiceI.end)	//音声ﾃﾞｰﾀが終わるとき
		{
			data=i2c_read_last();	//I2C読込み(最終ﾃﾞｰﾀ)
		}
		else				//音声ﾃﾞｰﾀが途中のとき
		{
			data=i2c_read();	//I2C読込み
		}
//DBG_C('\n');
//DBG_B(data);
		voiceI.pcm=(short)data-128;	//音声ﾃﾞｰﾀをsignedに変換
//DBG_S(voiceI.pcm);
		voiceI.pcm*=VOICEI_VOL*256/100;	//音量配分をを乗じて､16ﾋﾞｯﾄPCMｽｹｰﾙに変換
//DBG_C('-');
//DBG_S(voiceI.pcm);
	}
#endif

	voiceI.of+=voiceI.tobi;			//音声ﾃﾞｰﾀのｵﾌｾｯﾄを進める
//DBG_S(voiceI.of);
}
#endif


#if VOICE_C>0
#include "func_SDC.c"


//==============================================
//音声C選択処理(API関数)
//==============================================
static void VOICEC_SEL(				//戻り値無し
	unsigned short no)			//音声番号(0は再生中止)
{

//DBG_C('\n');
//DBG_C('s');
//DBG_S(no);
//DBG_D(voiceC_idx,40);
//DBG_C('i');
	if(no)					//再生開始のとき
	{
		if(no>sizeof(voiceC_idx)/sizeof(voiceC_idx[0])) return;
						//実装数を超えているときは戻る
		if(voiceC.no==no) return;	//発声中の音声番号と同じ場合は戻る
		voiceC.no=no--;			//音声番号を設定して､番号を0起算にする
//DBG_S(voiceC.no);
		voiceC.begin=voiceC_idx[no].adr;//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを設定
//DBG_L(voiceC.begin);
		voiceC.end=voiceC.begin+voiceC_idx[no].len*2;
						//音声ﾃﾞｰﾀの終了ｱﾄﾞﾚｽを設定
//DBG_L(voiceC.end);
		voiceC.bct=0;			//ﾌﾞﾛｯｸ内ﾃﾞｰﾀｱﾄﾞﾚｽｶｳﾝﾀを初期化
//DBG_S(voiceC.bct);
		voiceC.tobi=voiceC_idx[no].tobi;//再生処理のｻﾝﾌﾟﾙ飛び数[1/256]を設定
//DBG_S(voiceC.tobi);
		voiceC.of=255;			//再生開始時にﾃﾞｰﾀを取得するようにしておく
//DBG_S(voiceC.of);
		voiceC.pcm=0;			//PCM値を初期化
//DBG_B(voiceC.pcm);
	}
	else					//再生中断のとき
	{
		if(voiceC.no)			//再生中だったとき
		{
			voiceC.begin=voiceC.end-2;
						//最終ﾍﾟｰｼﾞにしておく
			voiceC.no=0xffff;	//再生終了中にする
		}
	}
//DBG_D(&voiceC,sizeof(voiceC));
}


//==============================================
//音声C再生処理ｻﾝﾌﾟﾙﾚｰﾄ設定(API関数)
//==============================================
static void VOICEC_SMP(				//戻り値無し
	unsigned smp)				//再生処理ｻﾝﾌﾟﾙﾚｰﾄ[sps]
{
//DBG_C('\n');
//DBG_C('p');

	voiceC.tobi=(unsigned long)256*smp*PWM_PR/1000000;
//DBG_S(voiceC.tobi);
}


//==============================================
//音声C再生処理
//==============================================
static void voiceC_exe(void)
{

//DBG_C('\n');
//DBG_C('p');
//DBG_S(voiceC.bct);
//DBG_L(voiceC.begin);
	voiceC.of+=voiceC.tobi;			//音声ﾃﾞｰﾀのｵﾌｾｯﾄを進める
//DBG_S(voiceC.of);
	if(!(voiceC.of&0xff00)) return;		//ｵﾌｾｯﾄの整数部が同じだったら戻る
	voiceC.of&=0x00ff;			//ｵﾌｾｯﾄの整数部をｸﾘｱしておく
//DBG_S(voiceC.of);
	if(!(voiceC.bct++))			//ﾍﾟｰｼﾞ先頭のとき
	{
//DBG_C('r');
		mmsd_read();			//SD読込み開始を設定する
	}
	voiceC.pcm=(short)spi_trans(0xff)-128;	//音声ﾃﾞｰﾀを取出して､
						//　8ﾋﾞｯﾄPCM形式をsignedに変換
//DBG_S(voiceC.pcm);
	voiceC.pcm*=VOICEC_VOL*256/100;		//音量配分をを乗じて､16ﾋﾞｯﾄPCMｽｹｰﾙに変換
//DBG_S(voiceC.pcm);
	if(voiceC.bct==512)			//ﾌﾞﾛｯｸの最後になったら
	{
		voiceC.bct=0;			//ﾌﾞﾛｯｸ内ﾊﾞｲﾄｱﾄﾞﾚｽをｸﾘｱする
		if((voiceC.begin+=2)==voiceC.end)
						//音声ﾃﾞｰﾀの最後になったら
		{
//DBG_C('e');
//DBG_S(voiceC.no);

			//SDを初期化
			mmsd_init();		//SDCを初期化する

			//再生終了
			if(voiceC.no!=0xffff)
			{
				voiceC.no=0;	//再生終了にする
				CB_VOICEC_END();//音声再生終了処理(ｺｰﾙﾊﾞｯｸ関数)を呼出す
			}
			voiceC.no=0;		//再生終了にする
			voiceC.pcm=0;		//PCM値をｸﾘｱしておく
		}
	}
}
#endif


#if VOICE_S>0
//==============================================
//音声S選択処理(API関数)
//==============================================
static void VOICES_SEL(				//戻り値無し
	unsigned char ch,			//ch番号
	unsigned short no)			//音声番号(0は再生中止)
{
	struct VOICES *voiceS_pt;		//音声S再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀ

//DBG_C('\n');
//DBG_C('s');
//DBG_S(no);
//DBG_D(voiceS_idx,sizeof(voiceS_idx));
//DBG_C('i');
	voiceS_pt=voiceS+ch;			//音声S再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀを設定
	if(no)					//再生開始のとき
	{
		if(no>sizeof(voiceS_idx)/sizeof(voiceS_idx[0])) return;
						//実装数を超えているときは戻る
		if(voiceS_pt->no==no) return;	//発声中の音声番号と同じ場合は戻る
		voiceS_pt->no=no--;		//音声番号を設定して､番号を0起算にする
		voiceS_pt->begin=voiceS_idx[no].adr;
						//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを設定
//DBG_L(voiceS_pt->begin);
		voiceS_pt->end=voiceS_pt->begin+voiceS_idx[no].len;
						//音声ﾃﾞｰﾀの終了ｱﾄﾞﾚｽを設定
//DBG_L(voiceS_pt->end);
		voiceS_pt->of=0x0100;		//音声ﾃﾞｰﾀのｵﾌｾｯﾄを初期化しておく
						//(初回処理でサンプルを取出すように)
//DBG_S(voiceS_pt->of);
		voiceS_pt->pcm=0;		//PCM値をｸﾘｱしておく
//DBG_S(voiceS_pt->pcm);
		voiceS_pt->tobi=voiceS_idx[no].tobi;
						//再生処理のｻﾝﾌﾟﾙ飛び数[1/256]を設定
						//(音声ﾍｯﾀﾞには[1/256]の精度で格納されている)
//DBG_S(voiceS_pt->tobi);
#ifdef VOICES_IMA			//IMA-ADPCMを実装するとき
		voiceS_pt->tobi>>=1;		//再生処理のｻﾝﾌﾟﾙ飛び数をﾆﾌﾞﾙ単位にする
//DBG_S(voiceS_pt->tobi);
		voiceS_pt->ima.predicted=32768;	//予測値を初期化
		voiceS_pt->ima.step_index=0;	//ｽﾃｯﾌﾟｲﾝﾃﾞｯｸｽを初期化
#endif
#if VOICE_S==1				//単一ch実装のとき
		spi_addr=0;			//SPIｱﾄﾞﾚｽ設定未にする
		spi_dis();			//ﾃﾞｨｾﾚｸﾄ
#endif
	}
	else					//再生中断のとき
	{
		voiceS_pt->no=0;		//再生終了にする
		voiceS_pt->pcm=0;		//PCM値をｸﾘｱしておく
#if VOICE_S<2				//単一ch実装のとき
		spi_dis();			//ﾃﾞｨｾﾚｸﾄ
#endif
	}
//DBG_D(&voiceS,sizeof(voiceS));
}


//==============================================
//音声S再生処理ｻﾝﾌﾟﾙﾚｰﾄ設定(API関数)
//==============================================
static void VOICES_SMP(				//戻り値無し
	unsigned char ch,			//ch番号
	unsigned smp)				//再生処理ｻﾝﾌﾟﾙﾚｰﾄ[sps]
{
//DBG_C('\n');
//DBG_C('p');

	voiceS[ch].tobi=(unsigned long)256*smp*PWM_PR/1000000;
//DBG_S(voiceS[ch].tobi);
}


//==============================================
//音声Sｻﾝﾌﾟﾙ取得
//==============================================
static unsigned char voiceS_get(		//ｻﾝﾌﾟﾙを返す
	struct VOICES *voiceS_pt)		//音声S再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀ
{
	unsigned char data;
#if VOICE_S>1&&SPI_BUF_N>1		//SPI音声複数ch且つSPIﾊﾞｯﾌｧ複数のとき
	unsigned char n;
#endif

#if VOICE_S==1				//単一ch実装のとき
	if(!spi_addr)				//SPIｱﾄﾞﾚｽ設定未のとき
#endif
#if VOICE_S>1&&SPI_BUF_N>1		//SPI音声複数ch且つSPIﾊﾞｯﾌｧ複数のとき
	if(!voiceS_pt->spi_buf_n)		//SPIﾊﾞｯﾌｧにﾃﾞｰﾀが無いとき
#endif
	{
		GPIO_L(SPI_SCK_GPIO,SPI_SCK_BIT);
						//SCKをL
		spi_sel();			//ｾﾚｸﾄ
		spi_send(W25_ReadData,0);	//ReadDataｺﾏﾝﾄﾞ
		spi_send(voiceS_pt->begin>>16,0);
						//ｱﾄﾞﾚｽ(MSB)
		spi_send(voiceS_pt->begin>>8,0);//ｱﾄﾞﾚｽ
		spi_send(voiceS_pt->begin,0);	//ｱﾄﾞﾚｽ(LSB)
#if VOICE_S==1				//単一ch実装のとき
		spi_addr=1;			//SPIｱﾄﾞﾚｽ設定済にする
#endif
#if VOICE_S>1&&SPI_BUF_N>1		//SPI音声複数ch且つSPIﾊﾞｯﾌｧ複数のとき
		for(n=0;n<SPI_BUF_N;n++)	//ﾊﾞｯﾌｧの大きさだけ詰める
		{
			voiceS_pt->spi_buf[n]=spi_recv();
						//SPIﾌﾗｯｼｭからﾊﾞｯﾌｧへ移す
		}
		voiceS_pt->spi_buf_n=SPI_BUF_N;	//ﾊﾞｯﾌｧ満杯にする
		voiceS_pt->begin+=SPI_BUF_N;	//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを進める
#endif
	}
#if VOICE_S>1&&SPI_BUF_N>1		//SPI音声複数ch且つSPIﾊﾞｯﾌｧ複数のとき
	data=voiceS_pt->spi_buf[SPI_BUF_N-voiceS_pt->spi_buf_n--];
						//ﾊﾞｯﾌｧからｻﾝﾌﾟﾙを取出す､
#else					//SPI音声単一又はSPIﾊﾞｯﾌｧ無しのとき
	data=spi_recv();			//SPIﾌﾗｯｼｭからｻﾝﾌﾟﾙを取出す
	voiceS_pt->begin++;			//音声ﾃﾞｰﾀの開始ｱﾄﾞﾚｽを進める
#endif
//DBG_B(data);
#if VOICE_S>1				//複数ch実装のとき
		spi_dis();			//ﾃﾞｨｾﾚｸﾄ
#endif
	return(data);
}


//==============================================
//音声S再生処理
//==============================================
static void voiceS_exe(
	unsigned char ch)			//ch番号
{
	struct VOICES *voiceS_pt;		//音声S再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀ
	static unsigned char data;		//ｻﾝﾌﾟﾙﾃﾞｰﾀ

//DBG_C('\n');
//DBG_C('p');
//DBG_B(ch);
	voiceS_pt=voiceS+ch;			//音声S再生制御ﾃｰﾌﾞﾙﾎﾟｲﾝﾀを設定する
	if(voiceS_pt->begin>=voiceS_pt->end)	//音声ﾃﾞｰﾀが終わったとき
	{
//DBG_C('e');
		voiceS_pt->no=0;		//再生終了にする
		voiceS_pt->pcm=0;		//PCM値をｸﾘｱしておく
		CB_VOICES_END(ch);		//音声再生終了処理(ｺｰﾙﾊﾞｯｸ関数)を呼出す
		return;
	}

#ifdef VOICES_IMA			//IMA-ADPCMｴﾝｺｰﾄﾞのとき
	if(voiceS_pt->of&0x0100)		//ｵﾌｾｯﾄ計算の整数部に桁上がりしたら
	{
		voiceS_pt->of=voiceS_pt->of&0x00ff|0x8000;
						//ｵﾌｾｯﾄの整数部をｸﾘｱして､
						//　先行ﾆﾌﾞﾙ処理済みにする
		data=voiceS_get(voiceS_pt);	//ｻﾝﾌﾟﾙを取出す
//DBG_B(data);
		voiceS_pt->pcm=ima_decode(&voiceS_pt->ima,data&0x0f)-32768;
						//先行ﾆﾌﾞﾙをIMA-ADPCMﾃﾞｺｰﾄﾞする
//DBG_C('a');
//DBG_C('=');
//DBG_S(voiceS_pt->pcm);
		voiceS_pt->pcm=(long)voiceS_pt->pcm*voiceS_vol[ch]/256;
						//音量配分を乗じる
//DBG_C('-');
//DBG_S(voiceS_pt->pcm);
	}
	else if((voiceS_pt->of&0x8080)==0x8080)	//後行ﾆﾌﾞﾙになったら
	{
		voiceS_pt->of&=0x00ff;		//ｵﾌｾｯﾄの整数部をｸﾘｱして､
						//　後行ﾆﾌﾞﾙ処理済みにする
		voiceS_pt->pcm=ima_decode(&voiceS_pt->ima,data>>4)-32768;
						//後行ﾆﾌﾞﾙをIMA-ADPCMﾃﾞｺｰﾄﾞする
//DBG_C('b');
//DBG_C('=');
//DBG_S(voiceS_pt->pcm);
		voiceS_pt->pcm=(long)voiceS_pt->pcm*voiceS_vol[ch]/256;
						//音量配分を乗じる
//DBG_C('-');
//DBG_S(voiceS_pt->pcm);
	}

#else					//8ﾋﾞｯﾄPCMｺｰﾄﾞのとき
	if(voiceS_pt->of&0x0100)		//ｵﾌｾｯﾄ計算の整数部に桁上がりしたら
	{
		voiceS_pt->of&=0x00ff;		//ｵﾌｾｯﾄの整数部をｸﾘｱしておく
		data=voiceS_get(voiceS_pt);	//ｻﾝﾌﾟﾙを取出す
//DBG_B(data);
		voiceS_pt->pcm=data-128;	//8ﾋﾞｯﾄPCM形式をsignedに変換
//DBG_C('\n');
//DBG_S(voiceS_pt->pcm);
//DBG_S(voiceS_vol[ch]);
		voiceS_pt->pcm*=voiceS_vol[ch];	//音量配分を乗じて､16ﾋﾞｯﾄPCMｽｹｰﾙに変換
//DBG_S(voiceS_pt->pcm);
	}
#endif

	voiceS_pt->of+=voiceS_pt->tobi;	//音声ﾃﾞｰﾀのｵﾌｾｯﾄを進める
//DBG_S(voiceS_pt->of);
}
#endif


//==============================================
//ﾊﾞｯﾌｧへ詰める
//ﾊﾞｯﾌｧﾌﾙの場合は待合せる
//==============================================
static void put_buf(
	short pcm)				//PCMﾃﾞｰﾀ
{
	unsigned char n;

//DBG_S(pcm);
	n=buf_write;				//書込みﾎﾟｲﾝﾀを取得する
	if(++n==OUT_BUF_N) n=0;			//書込み位置を仮に進める
	while(n==buf_read);			//ﾊﾞｯﾌｧが空くのを待つ
	buf[buf_write]=pcm;			//ﾃﾞｰﾀをﾊﾞｯﾌｧに書込む
	buf_write=n;				//書込みﾎﾟｲﾝﾀを更新する
}


//==============================================
//ﾒｲﾝ処理
//==============================================
int main(void)
{

	CB_INIT();				//ﾃﾞﾊﾞｲｽの初期設定

#if PART_N+KB_N>0				//ｵﾙｺﾞｰﾙ演奏･KB演奏を実装するとき
	//&SONG_IDXをﾘﾝｸするためのおまじない
	void song_idx_dmy(void);		//song_idxのｴﾝﾄﾘ
	song_idx_dmy();				//ﾀﾞﾐｰｺｰﾙする
#endif


//==============================================
//ﾒｲﾝﾙｰﾌﾟ
//==============================================
	while(1)
	{
//DBG_C('\n');
//DBG_C('m');


//==============================================
//固有処理
//==============================================
//DBG_S(kyotuu.ps);
		if(!(kyotuu.ps--))		//固有処理のﾀｲﾐﾝｸﾞになったら
		{
//DBG_C('k');
			kyotuu.ps=(unsigned long)KOYUU_PR*1000/PWM_PR-1;
						//固有処理ﾌﾟﾘｽｹｰﾗを初期化
//DBG_C('#');
			CB_KOYUU();		//固有処理(ｺｰﾙﾊﾞｯｸ関数)を呼出す
//DBG_C('$');
		}


//==============================================
//出力事前処理
//==============================================
		kyotuu.pcm=0;			//PCM合算値をｸﾘｱしておく
//DBG_C('c');


#if PART_N+KB_N>0
//==============================================
//ｵﾙｺﾞｰﾙ演奏処理
//==============================================
//DBG_B(song.song_ps);
		if(!(song.song_ps--))		//演奏処理のﾀｲﾐﾝｸﾞになったら
		{
//DBG_C('o');
			song.song_ps=SONG_PS-1;	//演奏処理ﾌﾟﾘｽｹｰﾗを初期化
			song_exe();		//演奏処理を実行する
//DBG_C('=');
//DBG_S(song.pcm);
		}
		kyotuu.pcm+=song.pcm;		//PCM値を反映する
#endif


//DBG_C('n');
#if VOICE_N>0
//==============================================
//音声0再生処理
//==============================================
//DBG_S(voiceN[0].no);
		if(voiceN[0].no)		//再生中のとき
		{
			voiceN_exe(0);		//演奏処理を実行する
//DBG_C('-');
//DBG_S(voiceN[0].pcm);
		}
		kyotuu.pcm+=voiceN[0].pcm;	//PCM値を反映する
#endif


#if VOICE_N>1
//==============================================
//音声1再生処理
//==============================================
		if(voiceN[1].no)		//再生中のとき
		{
			voiceN_exe(1);		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceN[1].pcm;	//PCM値を反映する
#endif


#if VOICE_N>2
//==============================================
//音声2再生処理
//==============================================
		if(voiceN[2].no)		//再生中のとき
		{
			voiceN_exe(2);		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceN[2].pcm;	//PCM値を反映する
#endif


#if VOICE_I>0
//==============================================
//音声I再生処理
//==============================================
//DBG_C('i');
		if(voiceI.no)			//再生中のとき
		{
			voiceI_exe();		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceI.pcm;		//PCM値を反映する
#endif


#if VOICE_C>0
//==============================================
//音声C再生処理
//==============================================
//DBG_C('c');
		if(voiceC.no)			//再生中のとき
		{
			voiceC_exe();		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceC.pcm;		//PCM値を反映する
#endif


#if VOICE_S>0
//DBG_C('s');
//==============================================
//音声S0再生処理
//==============================================
		if(voiceS[0].no)		//再生中のとき
		{
			voiceS_exe(0);		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceS[0].pcm;	//PCM値を反映する
#endif


#if VOICE_S>1
//==============================================
//音声S1再生処理
//==============================================
		if(voiceS[1].no)		//再生中のとき
		{
			voiceS_exe(1);		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceS[1].pcm;	//PCM値を反映する
#endif


#if VOICE_S>2
//==============================================
//音声S2再生処理
//==============================================
		if(voiceS[2].no)		//再生中のとき
		{
			voiceS_exe(2);		//演奏処理を実行する
		}
		kyotuu.pcm+=voiceS[2].pcm;	//PCM値を反映する
#endif


//==============================================
//ﾊﾞｯﾌｧ書出し
//==============================================
//DBG_C('b');
//DBG_S(kyotuu.pcm);
		if(kyotuu.pcm>=32767) kyotuu.pcm=32767;
						//有効な範囲に規制する
		else if(kyotuu.pcm<-32768) kyotuu.pcm=-32768;
//DBG_C('=');
//DBG_S(kyotuu.pcm);
		put_buf(kyotuu.pcm);		//16ﾋﾞｯﾄPCM形式でﾊﾞｯﾌｧに出力する
	}
}
