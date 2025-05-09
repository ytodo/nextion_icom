//////////////////////////////////////////////////////////////////////////////////
//	ファイル名	DispLOGinfo.c
//			2020.03.07-2025.04.18
//	機能	dispstatus_ref	dstarrepeaterd.logから現状をピックアップ
//			dispstatus_dmon	dmonitor.logから現状をピックアップ
//			Nextion上にラストハードやステータスとして表示する。
//////////////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"

FILE	*fp;						// ファイルポインタ
char	*tmpptr;					// 一時的ポインタ
char	tmpstr[32]	= {'\0'};		// 一時的文字列

/********************************************************
 * dstarrepeaterd-yyyy-mm-dd.log を読み込み、
 * 処理の結果情報を取得する
 ********************************************************/
void dispstatus_ref(void)
{
	char	mycall[14]		= {'\0'};
	char	urcall[9]		= {'\0'};
	char	status2[32]		= {'\0'};
	time_t	timer;
	struct	tm *timeptr;

	/*
	 * ログファイルからリフレクタへのリンク情報を抽出する
	 */

	/* コマンドの標準出力オープン */
	if ((fp = popen("tail -n1 /tmp/tmplog.txt", "r")) == NULL )
	{
		printf("LOGinfo open error!!");
		exit(EXIT_FAILURE);
	}

	/* ファイル行を配列に取得 */
	while ((fgets(line, sizeof(line), fp)) != NULL)
	{
		/* 一巡して全く同じ内容ならパス */
		if (strncmp(line, chkline, strlen(line)) == 0) break;

		/* 重複チェック */
		strcpy(chkline, line);

		/*
		 * ラストパケットの抽出
		 */

		/* ネットワークラストパケット */
		if (strstr(line, "Stats for") != NULL)
		{
			/* TX Hang */
			sleep(TXHANG);

			/* IDLE 画面に戻る */
			sendcmd("page IDLE");
		}

		/* RF ラストパケット */
		if ((tmpptr = strstr(line, "AMBE for")) != NULL)
		{
			/* TX Hang */
			sleep(TXHANG);

			/* IDLE 画面に戻る */
			sendcmd("page IDLE");
		}


		/*
		 * リフレクタへの接続情報の取得
		 */

		/* リンク */
		if ((tmpptr = strstr(line, "Linked to")) != NULL)
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

		/* disconnect */
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
		if ((tmpptr = strstr(line, "Radio")) != NULL)
		{
			sendcmd("page DSTAR");

			/* ヘッダーログを取得 */
			status2[0] = '\0';

			/* JST 時刻の算出 */
			timer = time(NULL);
			timeptr = localtime(&timer);

			/* Radio header の場合RF を表示 */
			strftime(status2, sizeof(status2), "RF  %H:%M ", timeptr);

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
			timer = time(NULL);
			timeptr = localtime(&timer);

			/* Network header の場合Net を表示 */
			strftime(status2, sizeof(status2), "Net %H:%M ", timeptr);

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
		line[0]  = '\0';

	}	// Loop

	/* ファイルクローズ */
	pclose(fp);

	return;
}


/************************************************************
	dmonitor のログファイルよりラストハード及び
	状況を取得し変数status に入れる
 ************************************************************/
void	dispstatus_dmon(void)
{
	FILE	*fp;
	char	*tmpptr;
	char	mycall[9]		= {'\0'};
	char	mycallpre[9]	= {'\0'};


	/* コマンドの標準出力オープン */
	if ((fp = popen("tail -n2 /tmp/tmplog.txt", "r")) == NULL)
	{
		printf("dmonitor.log open error!\n");
		return;
	}

	/* 標準出力を配列に取得 */
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if ((strcmp(line, chkline) == 0)) continue;
		strcpy(chkline, line);

		/* 過去のデータをクリアする  */
		memset(status,  '\0', sizeof(status));
		memset(tmpstr,  '\0', sizeof(tmpstr));

		/* <1>どこに接続したかを取得 */
		if ((tmpptr = strstr(line, "Connected")) != NULL)
		{
			memset(rptcall, '\0', sizeof(rptcall));
			strncpy(rptcall, tmpptr + 13, 8);
			disp_rpt();
		}

		/* <2>dmonitorの開始とバージョンを取得 */
		if ((tmpptr = strstr(line, "dmonitor start")) != NULL)
		{
			strncpy(status, tmpptr, 21);
			disp_stat();
		}

		/* <3>dmonitorへの信号がZRからかGW側からかを判断して status 代入の準備のみする */
		if ((tmpptr = strstr(line, "from ZR")) != NULL || (tmpptr = strstr(line, "from GW")) != NULL)
		{
			/* MyCallsignの取得 */
			memset(mycall, '\0', sizeof(mycall));
			strncpy(mycall, tmpptr - 9, 8);         // My Callsign

			/* MyCallsignが単なるループではない場合 */
			if (strncmp(mycall, mycallpre, 8) != 0)
			{
				if (strncmp(nx.dmon_ver, "02", 2) == 0)
				{
					strncpy(status, line + 5, 5);             // 日付時分
					strcat(status, " ");
					strncat(status, line + 11, 5);
				}
				if (strncmp(nx.dmon_ver, "01", 2) == 0)
				{
					printf(line);
					strncpy(status, line, 12);
				}
				strcat(status, " ");
				strncat(status, mycall, 8);            // コールサイン
				strncat(status, tmpptr + 4, 3);        // ZR/GW
			}
			disp_stat();
		}

		/* <4>無線機から送信したときのログを出力 */
		if ((tmpptr = strstr(line, "from Rig"   )) != NULL ||
		    (tmpptr = strstr(line, "from DVAP"  )) != NULL ||
		    (tmpptr = strstr(line, "from DVMEGA")) != NULL)
		{
			if (strncmp("Rig",  tmpptr + 5, 3) == 0)
			{
				if (strncmp(nx.rigtype, "ICOM", 4) == 0)
				{
					strncpy(tmpstr, " TM", 3);
				} else {
					strncpy(tmpstr, " RF", 3);
				}
			} else {
				strncpy(tmpstr, " RF", 3);
			}
			if (strncmp(nx.dmon_ver, "02", 2) == 0)
			{
				strncpy(status, line + 5, 5);             // 日付時分
				strcat(status, " ");
				strncat(status, line + 11, 5);
			}
			if (strncmp(nx.dmon_ver, "01", 2) == 0)
			{
				strncpy(status, line, 12);
			}
			strcat(status, " ");
			strncat(status, tmpptr - 9, 8);         // コールサイン
			strcat(status, tmpstr);                 // Terminal-AP Mode/DVAP/DVMEGA/Node

			/* RFからのエリアCQ操作でエリアリピータへの接続コマンドとする */
			if (strncmp(tmpptr, "/", 1) == 0 && strncmp(nx.rigtype, "ICOM", 4) != 0) strcpy(rfcommand, tmpstr);
			disp_stat();
		}

		/* <5-1>モデムの接続状況 */
		if ((nx.debug == 1) && (((tmpptr = strstr(line, "Frequency Set")) != NULL) || (strstr(line, "RIG(ID-xxPlus) init") != NULL)))
		{
			/* RFを使う場合には周波数表示で初期化表示とする */
			if (strstr(line, "DVAP"))   strcpy(status, "DVAP FREQ. ");
			if (strstr(line, "DVMEGA")) strcpy(status, "DVMEGA FREQ. ");

			/* ケーブル接続の場合はポートの初期化を表示する */
			if (strcmp(nx.rigtype, "ICOM") == 0)
			{
				strcpy(status, "RIG initializing is done.");
			} else {
				strncat(status, tmpptr + 14, 3);
				strcat(status, ".");
				strncat(status, tmpptr + 17, 3);
				strcat(status, " MHz");
			}
			disp_stat();
		}
		/* <5-2>ノードアダプターV7の接続状況 */
		if ((nx.debug == 1) && ((tmpptr = strstr(line, "NODE Alter Header")) != NULL))
		{
			strncpy(status, "NODE ADAPTER INITIALIZED", 24);
			disp_stat();
		}

		/* <6>Last packet wrong ステータスの場合、文字を黄色に */
		if ((nx.debug == 1) && (strstr(line, "Last packet wrong") != NULL))
		{
			strcpy(status, "Last packet is wrong...");
			disp_stat();
		}

		/* <7>バッファの拡張のサイズを取得 */
		if ((tmpptr = strstr(line, "New FiFo buffer")) != NULL)
		{
			strcpy(status, tmpptr + 9);
			status[strlen(status) - 1] = '\0';
			disp_stat();
		}

		/* <8>UNLINKコマンドの処理 */
		if ((strstr(line, "my2:UNLK") != NULL) || (strstr(line, "UNLINK   from Rig") != NULL))
		{
			strcpy(status, "UNLINK FROM RIG");
			strcpy(rfcommand, "UNLINK");
			disp_stat();
		}

		/* <9>接続解除を取得 */
		if (strstr(line, "dmonitor end") != NULL)
		{
			rptcall[0] = '\0';
			strcpy(rptcall, "NONE");
			disp_rpt();
			strcpy(status, "Disconnected");
			disp_stat();
		}

		/* ドロップパケット比の表示 */
		if ((nx.debug == 1) && ((tmpptr = strstr(line, "drop packet")) != NULL))
		{
			strcpy(status, "Drop PKT ");
			strcat(status, tmpptr + 17);
			status[strlen(status) - 1] = '\0';
			disp_stat();
		}

		/* <11>SCAN中の接続リピータを表示 */
		if ((tmpptr = strstr(line, "ZR area")) != NULL)
		{
			strncpy(rptcall, tmpptr + 8, 8);
			disp_rpt();
		}
	}

	/* 標準出力クローズ */
	pclose(fp);

	return;
}


 /*********************************************************************
 変数rptcall の内容をNextionに表示する関数
 *********************************************************************/
int disp_rpt()
{
	/* rptcallの内容が前回と異なる場合表示する */
	if (strcmp(rptcall, chkrptcall) != 0)
	{
		chkrptcall[0] = '\0';
		strcpy(chkrptcall, rptcall);
		sprintf(command, "DMON.t1.txt=\"LINK TO : %s\"", rptcall);
		sendcmd(command);
		sprintf(command, "DMON.link.txt=\"LINK TO : %s\"", rptcall);
		sendcmd(command);
	}
	return(0);
}


/*********************************************************************
 変数status の内容をNextionに表示する関数
 *********************************************************************/
int disp_stat()
{
	/* statusの内容が過去３回と異なる場合表示する */
	if ((strcmp(status, chkstat1) != 0) && (strcmp(status, chkstat2) != 0) && (strcmp(status, chkstat3) != 0))
	{
		strcpy(chkstat3, chkstat2);
		strcpy(chkstat2, chkstat1);
		strcpy(chkstat1, status);

		/* STATUS1 => STATUS2 */
		sendcmd("DMON.stat2.txt=DMON.stat1.txt");

		/* 取得ステイタス=> STATUS1 */
		sprintf(command, "DMON.stat1.txt=\"%s\"", status);
		sendcmd(command);
		sendcmd("DMON.t2.txt=DMON.stat1.txt");
		sendcmd("DMON.t3.txt=DMON.stat2.txt");
		sendcmd("USERS.t8.txt=DMON.stat1.txt");
		sendcmd("USERS.t9.txt=DMON.stat2.txt");
	}
	return(0);
}
