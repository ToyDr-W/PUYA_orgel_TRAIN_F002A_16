echo off
del orgel_cnf.h
del orgel_cnf.inc
rem//==============================================
rem//構成情報(ｵﾙｺﾞｰﾙ演奏+音声再生処理に関わるもの)
rem//==============================================
call make_cnf.bat	SYS_CLK 	48	//ｼｽﾃﾑｸﾛｯｸ周波数[MHz](48=公証値､64=ｸﾛｯｸｱｯﾌﾟ)
call make_cnf.bat	BTL 		0	//BTL出力指定
rem						//　0=ｼﾝｸﾞﾙ出力
rem						//　1=ｺﾝﾌﾟﾘ出力
rem						//　2=ﾌﾞﾘｯｼﾞ出力
rem						//　3=ﾌﾞﾚｰｷ出力
call make_cnf.bat	PWM_STEP	1024
call make_cnf.bat	PWM_FRQ		(SYS_CLK*1000/PWM_STEP)	//PWM周波数=46.875[kHz]
call make_cnf.bat	PWM_PR		(PWM_STEP/SYS_CLK)	//PWM周期=21.333[us]
call make_cnf.bat	PWM_DB_CMP	0x10	//PWMﾃﾞｯﾄﾞﾊﾞﾝﾄﾞ(ｺﾝﾌﾟﾘ出力時)[BDTRb7-0]
call make_cnf.bat	PWM_DB_BRG	10	//PWMﾃﾞｯﾄﾞﾊﾞﾝﾄﾞ(ﾌﾞﾘｯｼﾞ出力時)[0.021us]
call make_cnf.bat	OUT_BUF_N	50	//出力ﾊﾞｯﾌｧｻｲｽﾞ
call make_cnf.bat	SPI_BUF_N	8	//SPIﾊﾞｯﾌｧｻｲｽﾞ
call make_cnf.bat	SONG_PS		2	//演奏処理ﾌﾟﾘｽｹｰﾙ値
rem						//　演奏処理周期=PWM_PR*SONG_PS
call make_cnf.bat	KOYUU_PR	32	//固有処理のｺｰﾙﾊﾞｯｸ周期[ms]
call make_cnf.bat	VOICEN0_VOL	100	//音声N0のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICEN1_VOL	100	//音声N1のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICEN2_VOL	100	//音声N2のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICEI_VOL	100	//音声Iのﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICEC_VOL	100	//音声Cのﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICES0_VOL	100	//音声S0のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICES1_VOL	100	//音声S1のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICES2_VOL	100	//音声S2のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	ORGEL_VOL	100	//ｵﾙｺﾞｰﾙ演奏のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	KB0_VOL		100	//KB0演奏のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	KB1_VOL		100	//KB1演奏のﾎﾞﾘｭｰﾑ配分[%]
call make_cnf.bat	VOICE_N		1	//音声N発声数の最大値(最大値3､
rem						//　0のときは音声再生機能を実装しない)
call make_cnf.bat	VOICE_I		0	//音声I発声数の最大値(最大値1､
rem						//　0のときは音声再生機能を実装しない)
call make_cnf.bat	VOICE_C		0	//音声C発声数の最大値(最大値1､
rem						//　0のときは音声再生機能を実装しない)
call make_cnf.bat	VOICE_S		0	//音声S発声数の最大値(
rem						//　0のときは音声再生機能を実装しない)
call make_cnf.bat	PART_N		4	//演奏ﾊﾟｰﾄ数の最大値(最大値4､
rem						//　0のときはｵﾙｺﾞｰﾙ演奏機能を
rem						//　実装しない)
call make_cnf.bat	KB_N		0	//KB演奏ﾊﾟｰﾄ数の最大値(最大値2､
rem						//　0のときはKB演奏機能を実装しない)
echo; end >> orgel_cnf.inc
