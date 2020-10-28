//////////////////////////////////////////////////////////////////////////////////
//	ファイル名	DispLOGinfo.c
//			2020.03.07-
//	機能	dispstatus_ref	dstarrepeaterd.logから現状をピックアップ
//		dispstatus_dmon	dmonitor.logから現状をピックアップ
//		Nextion上にラストハードやステータスとして表示する。
//////////////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"

FILE	*fp;					// ファイルポインタ
char	*tmpptr;				// 一時的ポインタ
char	tmpstr[32]	= {'\0'};		// 一時的文字列
char	line[256]	= {'\0'};		// ファイルから読んだ一行


/********************************************************
 * dstarrepeaterd-yyyy-mm-dd.log を読み込み、
 * 処理の結果情報を取得する
 ********************************************************/
void dispstatus_ref(void)
{
	char	fname[32]		= {'\0'};	// ファイル名
	char	mycall[14]		= {'\0'};
	char	urcall[9]		= {'\0'};
	char	dstarlogpath[32]	= {'\0'};	// D-STAR Repeater ログのフルパス
	char	status2[32] 		= {'\0'};
	char	line2[128]		= {'\0'};

	/*
	 * ログファイルからリフレクタへのリンク情報を抽出する
	 */

	/* 日付入りログファイル名の作成 */
	timer = time(NULL);
	timeptr = gmtime(&timer);
	strftime(fname, sizeof(fname), "dstarrepeaterd-%Y-%m-%d.log", timeptr);
	sprintf(dstarlogpath, "%s%s", LOGDIR, fname);

	/* コマンドの標準出力オープン */
	sprintf(cmdline, "tail -n3 %s | egrep -v 'RTI_DATA_NAK'", dstarlogpath);
	if ((fp = popen(cmdline, "r")) == NULL )
	{
		printf("LOGinfo open error!!");
		exit(EXIT_FAILURE);
	}

	/* 標準出力を配列に取得 */
	fgets(line,  sizeof(line),  fp);
	line[strlen(line) - 1] = '\0';
	fgets(line2, sizeof(line2), fp);
	line[strlen(line2) - 1] = '\0';
	strcat(line, line2);

	/* 標準出力クローズ */
	pclose(fp);

	/* Transmitting to だけしかない行は省く */
	if (strstr(line, "Transmitting to") != NULL && strstr(line, "Network header") == NULL) return;

	/* 一巡して全く同じ内容ならパス */
	if (!strncmp(line, chkstat, sizeof(line))) return;

	/* 重複チェック */
	strcpy(chkstat, line);


	/*
	 * リフレクタへの接続情報の取得
	 */
	if ((tmpptr = strstr(line, "Linked")) != NULL)
	{
		/* リンク先リフレクタを取得 */
		linkref[0] = '\0';
		strncpy(linkref, tmpptr + 10, 8);
		linkref[8] = '\0';

		/* 接続時のログを取得 */
		status2[0] = '\0';
		strncpy(status2, tmpptr, 20);
		status2[20] = '\0';

		/* Nextion グローバル変数ref に接続中のリフレクタを代入 */
		sprintf(command, "IDLE.ref.txt=\"%s\"", linkref);
		sendcmd(command);
		sprintf(command, "IDLE.status.txt=\"%s\"", linkref);
		sendcmd(command);
		sendcmd("IDLE.t1.pco=65535");
		sendcmd("IDLE.t1.txt=status.txt");

		/* ステータス２の表示 */
		sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
		sendcmd(command);
		sendcmd("IDLE.t2.txt=status2.txt");
	}


	/*
	 * リフレクタへのdisconnect を取得
	 */
	if ((tmpptr = strstr(line, "Not linked")) != NULL)
	{
		/* リンク先リフレクタを取得 */
		linkref[0] = '\0';

		/* 接続時のログを取得 */
		status2[0] = '\0';
		strncpy(status2, tmpptr, 20);
		status2[20] = '\0';

		/* Nextion グローバル変数に代入 */
		sendcmd("IDLE.ref.txt=\"Not linked\"");
		sendcmd("IDLE.status.txt=\"NODE IDLE\"");
		sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
		sendcmd(command);

		sendcmd("IDLE.t1.txt=IDLE.status.txt");
		sendcmd("IDLE.t2.txt=IDLE.status2.txt");
	}


	/*
	 * RF ヘッダーの取得
	 */
	if ((tmpptr = strstr(line, "Radio header")) != NULL)
	{
		sendcmd("page DSTAR");

		/* ヘッダーログを取得 */
		status2[0] = '\0';

		/* JST 時刻の算出 */
		jstimer = time(NULL);
		jstimeptr = localtime(&jstimer);

		/* Radio header の場合RF を表示 */
		strftime(status2, sizeof(status2), "RF  %H:%M ", jstimeptr);

		/* ログよりコールサインMY, UR を取得 */
		strncpy(mycall, tmpptr + 27, 13);
		mycall[13] = '\0';
		strcat(status2, mycall);
		status2[23] = '\0';
		strncpy(urcall, tmpptr + 48, 8);
		urcall[8] = '\0';

		/* ステータスの表示 */
		sprintf(command, "DSTAR.t0.txt=\"R %s\"", mycall);
		sendcmd(command);
		sprintf(command, "DSTAR.t1.txt=\"%s\"", urcall);
		sendcmd(command);
		sendcmd("DSTAR.t2.txt=IDLE.ref.txt");

		/* ステータス2 を保存 */
		sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
		sendcmd(command);
		sendcmd("IDLE.t2.txt=\"IDLE.status2.txt");

	}


	/*
	 * RF ラストパケット
	 */
	if ((tmpptr = strstr(line, "AMBE for")) != NULL)
	{
		/* TX Hang */
		sleep(TXHANG);

		/* IDLE 画面に戻る */
		sendcmd("page IDLE");
	}


	/*
	 * ネットワーク ヘッダーの取得
	 */
	if ((tmpptr = strstr(line, "Network header")) != NULL)
	{
		/* TX Delay */
		sleep(TXDELAY);
		sendcmd("page DSTAR");

		/* ヘッダーログを初期化 */
		status2[0] = '\0';

		/* JST 時刻の算出 */
		jstimer = time(NULL);
		jstimeptr = localtime(&jstimer);

		/* Network header の場合Net を表示 */
		strftime(status2, sizeof(status2), "Net %H:%M ", jstimeptr);

		/* ログよりコールサインMY, UR を取得 */
		strncpy(mycall, tmpptr + 30, 13);
		mycall[13] = '\0';
		strcat(status2, mycall);
		status2[23] = '\0';
		strncpy(urcall, tmpptr + 51, 8);
		urcall[8] = '\0';

		/* ステータス２の表示 */
		sprintf(command, "DSTAR.t0.txt=\"N %s\"", mycall);
		sendcmd(command);
		sprintf(command, "DSTAR.t1.txt=\"%s\"", urcall);
		sendcmd(command);
		sendcmd("DSTAR.t2.txt=IDLE.ref.txt");

		/* ステータス2 を保存 */
		sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
		sendcmd(command);
		sendcmd("IDLE.t2.txt=\"IDLE.status2.txt");
	}


	/*
	 * ネットワークラストパケット
	 */
	if ((tmpptr = strstr(line, "Stats for")) != NULL)
	{
		/* TX Hang */
		sleep(TXHANG);

		/* IDLE 画面に戻る */
		sendcmd("page IDLE");
	}

	line[0]  = '\0';
	line2[0] = '\0';

	return;
}



/************************************************************
	dmonitor のログファイルよりラストハード及び
	状況を取得し変数status に入れる
 ************************************************************/
void	dispstatus_dmon(void)
{

        char    *getstatus      = "tail -n3 /var/log/dmonitor.log";
        char    mycall[8]       = {'\0'};
        char    mycallpre[8]    = {'\0'};
        char    tmpstat[32]     = {'\0'};
	char	ret[2]		= {'\0'};
	char	status[32]	= {'\0'};
	char	rptcall[9]	= {'\0'};
	int	stat		= 0;

	/* コマンドの標準出力オープン */
	if ((fp = popen(getstatus, "r")) == NULL)
	{
		printf("dmonitor.log file open error!\n");
		return;
	}

	/* 標準出力を配列に取得 */
	while ((fgets(line, sizeof(line), fp)) != NULL)
	{
		/* 過去のデータをクリアする  */
		memset(&status[0], '\0', sizeof(status));
		memset(&rptcall[0],'\0', sizeof(rptcall));

		/* status に関する文字列があったら */
		if ((tmpptr = strstr(line, "from")) != NULL)
		{
	                /* <1-1>dmonitorへの信号がZRからかGW側からかを判断して status 代入の準備のみする */
        	        if ((tmpptr = strstr(line, "from ZR")) != NULL || (tmpptr = strstr(line, "from GW")) != NULL)
                	{
	                        memset(&tmpstat[0], '\0', sizeof(tmpstat));

        	                /* MyCallsignの取得 */
                	        memset(&mycall[0], '\0', sizeof(mycall));
                        	strncpy(mycall, tmpptr - 9, 8);         // My Callsign

	                        /* MyCallsignが単なるループではない場合 */
        	                if (strncmp(mycall, mycallpre, 8) != 0)
                	        {
                        	        strncpy(tmpstat, line, 12);             // 日付時分
                                	strcat(tmpstat, " ");
	                                strncat(tmpstat, mycall, 8);            // コールサイン
        	                        strncat(tmpstat, tmpptr + 4, 3);        // ZR/GW
                	        }
	                }

	                /* <2>無線機から送信したときのログを出力 */
        	        if ((tmpptr = strstr(line, "from")) != NULL)
                	{
	                        if (strncmp("Rig",  tmpptr + 5, 3) == 0)
				{
					if (nx.type == "ICOM") 	 strncpy(tmpstr, " TM", 3);
					if (nx.type == "DVAP")	 strncpy(tmpstr, " RF", 3);
					if (nx.type == "NODE")   strncpy(tmpstr, " RF", 3);
					if (nx.type == "DVMEGA") strncpy(tmpstr, " RF", 3);
				}

                	        strncpy(status, line, 12);              // 日付時分
                        	strcat(status, " ");
	                        strncat(status, tmpptr - 9, 8);         // コールサイン
        	                strcat(status, tmpstr);                 // Terminal-AP Mode/DVAP Mode
                	        stat = 0;
	                }

			/* <3>どこに接続したかを取得 */
			if ((tmpptr = strstr(line, "Connected")) != NULL)
			{
				strncpy(rptcall, tmpptr + 13, 8);
				rptcall[8] = '\0';
			}
		}

                /* <1-2>rpt2, rpt1, ur, my の行が見つかったら,コールサインを照合して前段のtmpstatをstatusに代入 */
                if ((tmpptr = strstr(line, "my:")) != NULL)
                {
                        if (strncmp(mycall, tmpptr + 3, 8) == 0) strcpy(status, tmpstat);
                        stat = 0;
                }


		/* <4>dmonitorの開始とバージョンを取得 */
		if ((tmpptr = strstr(line, "dmonitor start")) != NULL)
		{
			strncpy(status, tmpptr, 21);
		}

                /* <6>DVAP使用時の周波数 */
                if ((atoi(nx.debug) == 1) && ((tmpptr = strstr(line, "Frequency Set")) != NULL))
                {
                        strcpy(status, "DVAP FREQ. ");
                        strncat(status, tmpptr + 14, 3);
                        strcat(status, ".");
                        strncat(status, tmpptr + 17, 3);
                        strcat(status, " MHz");
                        break;
                }

		/* Last packet wrong ステータスの場合、文字を黄色に */
		if ((stat == 1) && (nx.debug == "1") && (strstr(line, "Last packet wrong") != NULL))
		{
			strcpy(status, "Last packet wrong...");
		}

		/* バッファの拡張のサイズを取得 */
		if ((tmpptr = strstr(line, "New FiFo buffer")) != NULL)
		{
			strcpy(status, tmpptr + 9);
			status[strlen(status) - 1] = '\0';
		}

                /* <10>UNLINKコマンドの処理 */
                if (strstr(line, "my2:UNLK") != NULL)
                {
                        strcpy(status, "UNLINK FROM RIG");

               	        system("sudo systemctl restart rpt_conn");
                       	system("sudo systemctl restart auto_repmon_light");
                        system("sudo killall -q -s 2 dmonitor");
       	                system("sudo rm -f /var/nun/dmonitor.pid");
                }

		/* 接続解除を取得 */
		if ((atoi(nx.debug) == 1) && (strstr(line, "dmonitor end") != NULL))
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "Disconnected");
			strcpy(rptcall, "NONE");
		}

                /* <9>無線機の接続状況 */
                if ((atoi(nx.debug) == 1) && (strstr(line, "init/re-init") != NULL))
                {
                        strcpy(status, "RIG initializing is done.");
                }

		/* ドロップパケット比の表示 */
		if ((atoi(nx.debug) == 1) && ((tmpptr = strstr(line, "drop packet")) != NULL))
		{
			strcpy(status, "Drop PKT ");
			strcat(status, tmpptr + 17);
			status[strlen(status) - 1] = '\0';
			stat = 1;
		}

	}

	/* 標準出力クローズ */
	pclose(fp);


        /* 接続先の表示*/
        if (((strncmp(rptcall, "J", 1) == 0) || (strncmp(rptcall, "NONE", 4) == 0)) && (strncmp(rptcall, rptcallpre, 8) != 0))
        {
                strncpy(rptcallpre, rptcall, 8);
                sprintf(command, "MAIN.t1.txt=\"LINK TO : %s\"", rptcall);
                sendcmd(command);
                sprintf(command, "MAIN.link.txt=\"LINK TO : %s\"", rptcall);
                sendcmd(command);
        }

	/* ステータス・ラストハードの表示 */
        if ((strlen(status) != 0) && (strncmp(status, statpre, 24) != 0))
	{
		strcpy(statpre, status);

		/* STATUS1 => STATUS2 */
		sendcmd("DMON.stat2.txt=DMON.stat1.txt");

		/* 取得ステイタス=> STATUS1 */
		sprintf(command, "DMON.stat1.txt=\"%s\"", status);
		sendcmd(command);
		sendcmd("DMON.t2.txt=DMON.stat1.txt");
		sendcmd("DMON.t3.txt=DMON.stat2.txt");
		sendcmd("USERS.t8.txt=DMON.stat1.txt");
		sendcmd("USERS.t9.txt=DMON.stat2.txt");

		/* statusをクリアする */
		status[0] = '\0';
	}

	return;
}
