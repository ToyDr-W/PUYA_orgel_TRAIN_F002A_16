//==============================================
//PWM周期割込み処理
//==============================================
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	short pcm;				//PCM値

//DBS_H;					//割込み処理開始を表示する
	TIM1->SR=0;				//割込みﾌﾗｸﾞをｸﾘｱする

	//PCM値を取得
	if(buf_read==buf_write) return;		//ﾃﾞｰﾀが無かったら飛ばす
	pcm=buf[buf_read];			//PCM値を取出す
	if(buf_read==(OUT_BUF_N-1)) buf_read=0;	//ﾊﾞｯﾌｧの後尾になったら先頭に戻す
	else buf_read++;			//ﾊﾞｯﾌｧを進める

	//ﾃﾞｭｰﾃｨｻｲｸﾙを設定
#if BTL==0				//ｼﾝｸﾞﾙ出力のとき
	TIM1->CCR2=((long)pcm+32768)*PWM_STEP/65536;
						//正相を設定する
#endif
#if BTL==1				//ｺﾝﾌﾟﾘ出力のとき
	TIM1->CCR2=((long)pcm+32768)*PWM_STEP/65536;
						//正相･逆相を設定する
#endif
#if BTL>1				//ﾌﾞﾘｯｼﾞ･ﾌﾞﾚｰｷ出力のとき
	if(pcm>0)				//正値のとき
	{
		TIM1->CCR4=0;			//逆相を止める
		TIM1->CCR2=(long)pcm*PWM_STEP/32768;
						//正相を設定する
	}
	else if(pcm<0)				//負値のとき
	{
		TIM1->CCR2=0;			//正相を止める
		TIM1->CCR4=(long)(-pcm)*PWM_STEP/32768;
						//逆相を設定する
	}
	else					//停止のとき
	{
		TIM1->CCR2=0;			//正相を止める
		TIM1->CCR4=0;			//逆相を止める
	}
#endif
//DBS_L;					//割込み処理終了を表示する
}


//==============================================
//PWM周期を待つ
//==============================================
static void wait_pwm(void)
{

	TIM1->SR=0;				//ｲﾍﾞﾝﾄﾌﾗｸﾞをｸﾘｱする
	while(!(TIM1->SR&TIM_SR_UIF));		//更新ｲﾍﾞﾝﾄを待つ
}


#ifdef	SLEEP_EN			//Sleep機能を実装するとき
//==============================================
//PWMを停止
//==============================================
static void dev_pwm_stop(void)
{

//DBG_C('\n');
//DBG_C('s');
	//DC更新を待つ
//DBG_C('1');
	wait_ms(10);				//10ms間はPWM割込み処理を回す

	//PWMを停止
//DBG_C('2');
	TIM1->DIER=0;				//TIM1割込み禁止

#if BTL==0				//ｼﾝｸﾞﾙ出力のとき
//DBG_C('3');
	//ﾗﾝﾌﾟ下降
	while(TIM1->CCR2)			//DCが0になるまで繰返す
	{
		TIM1->CCR2--;			//DCをﾃﾞｸﾘする
		wait_pwm();			//PWM周期を待つ
	}
#endif
#if BTL==1				//ｺﾝﾌﾟﾘ出力のとき
//DBG_C('4');
	TIM1->CCR2=PWM_STEP/2;			//DCを50%にする
	wait_pwm();				//PWM周期を待つ
	TIM1->CCER=0;				//PWM出力を止める
#endif
#if BTL>1				//ﾌﾞﾘｯｼﾞ･ﾌﾞﾚｰｷ出力のとき
//DBG_C('5');
	TIM1->CCR2=TIM1->CCR4=0;		//DCを0%にする
	wait_pwm();				//PWM周期を待つ
#endif
#if BTL==3				//ﾌﾞﾚｰｷ出力のとき
	TIM1->CCER&=~(TIM_CCER_CC2P|TIM_CCER_CC4P);
						//CH2とCH4をｱｸﾃｨﾌﾞHにする
#endif
//DBG_C('6');
}
#endif


//==============================================
//PWMを再開
//==============================================
static void dev_pwm_start(void)
{

//DBG_C('\n');
//DBG_C('r');
	//ﾗﾝﾌﾟ上昇
#if BTL==0				//ｼﾝｸﾞﾙ出力のとき
//DBG_C('1');
	while(TIM1->CCR2++<PWM_STEP/2)		//DCが50%になるまでｲﾝｸﾘする
	{
		wait_pwm();			//PWM周期を待つ
	}
#endif
#if BTL==1				//ｺﾝﾌﾟﾘ出力のとき
//DBG_C('2');
	TIM1->CCER|=TIM_CCER_CC2E|TIM_CCER_CC2NE;
						//CH2とCH2Nをﾋﾟﾝ出力
#endif
#if BTL==3				//ﾌﾞﾚｰｷ出力のとき
//DBG_C('3');
	TIM1->CCER|=TIM_CCER_CC2P|TIM_CCER_CC4P;//CH2とCH4をｱｸﾃｨﾌﾞLにする
#endif

	//PWMを再開
//DBG_C('4');
	TIM1->DIER=TIM_DIER_UIE;		//TIM1更新割込み許可
//DBG_C('5');
}


//==============================================
//PWM初期設定
//==============================================
static void pwm_init(void)
{

//DBG_C('p');

	//PWM設定(TIM1)
//DBG_C('p');
	RCC->APBENR2|=RCC_APBENR2_TIM1EN;	//TIM1にｸﾛｯｸ供給
	//TIM1->CR1のCMSﾋﾞｯﾄは00(ｴｯｼﾞｱﾗｲﾝﾄﾞﾓｰﾄﾞ)初期値
	//TIM1->SMCRのSMSﾋﾞｯﾄは000(ｽﾚｰﾌﾞﾓｰﾄﾞ無効)初期値
	//TIM1->PSCは0(ｸﾛｯｸ分周しない)初期値	//ｸﾛｯｸは48MHz
#if BTL==2				//ﾌﾞﾘｯｼﾞ出力のとき
	TIM1->ARR=PWM_STEP-1+PWM_DB_BRG;	//PWM周期(ﾃﾞｯﾄﾞﾊﾞﾝﾄﾞを考慮)
#else
	TIM1->ARR=PWM_STEP-1;			//PWM周期
#endif
	TIM1->BDTR|=TIM_BDTR_AOE;		//自動出力可
	TIM1->CNT=0;				//ｶｳﾝﾀをｸﾘｱ
	TIM1->CR1|=TIM_CR1_CEN;			//ｶｳﾝﾀ有効
//DBG_C('t');

	//PWM設定(TIM1_CH2)
	TIM1->CCMR1|=(TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_1|TIM_CCMR1_OC2PE);
						//CH2をPWMﾓｰﾄﾞ1､ﾌﾟﾘﾛｰﾄﾞ有効
	TIM1->CCER|=TIM_CCER_CC2E;		//CH2をﾋﾟﾝ出力
#ifdef PWM_F_BIT			//正相出力ﾋﾞｯﾄが実装されているとき
	GPIO_AF(PWM_F_GPIO,PWM_F_BIT,PWM_F_AF);	//PWM正相を交代機能設定
#endif

#if BTL==1				//ｺﾝﾌﾟﾘ出力のとき
//DBG_C('c');
	TIM1->CCR2=PWM_STEP/2;			//CH2のDCを50%
	TIM1->BDTR|=PWM_DB_CMP;			//ﾃﾞｯﾄﾞﾀｲﾑ設定
	TIM1->CCER|=TIM_CCER_CC2NE;		//CH2Nをﾋﾟﾝ出力
#ifdef PWM_C_BIT			//ｺﾝﾌﾟﾘ出力ﾋﾞｯﾄが実装されているとき
	GPIO_AF(PWM_C_GPIO,PWM_C_BIT,PWM_C_AF);	//PWMｺﾝﾌﾟﾘを交代機能設定
#endif
#endif

	//PWM設定(TIM1_CH4)
#if BTL>1				//ﾌﾞﾘｯｼﾞ出力､ﾌﾞﾚｰｷ出力のとき
//DBG_C('b');
	TIM1->CCMR2|=(TIM_CCMR2_OC4M_2|TIM_CCMR2_OC4M_1|TIM_CCMR2_OC4PE);
						//CH4をPWMﾓｰﾄﾞ1､ﾌﾟﾘﾛｰﾄﾞ有効
	TIM1->CCER|=TIM_CCER_CC4E;		//CH4をﾋﾟﾝ出力
#ifdef PWM_R_BIT			//逆相出力ﾋﾞｯﾄが実装されているとき
	GPIO_AF(PWM_R_GPIO,PWM_R_BIT,PWM_R_AF);	//PWM逆相を交代機能設定
#endif
#endif
#if BTL==3				//ﾌﾞﾚｰｷ出力のとき
//DBG_C('k');
	TIM1->CCER|=TIM_CCER_CC2P|TIM_CCER_CC4P;//CH2とCH4をｱｸﾃｨﾌﾞLにする
#endif
//DBG_C('q');

	//PWM周期割込み設定
	NVIC->ISER[0]=(1<<TIM1_BRK_UP_TRG_COM_IRQn);
						//TIM1割込み許可
//DBG_C('i');
	dev_pwm_start();			//PWMを開始する
}
