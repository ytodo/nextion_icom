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
char	line[256]	= {'\0'};		// ファイルから読んだ一行
char	tmpstr[32]	= {'\0'};		// 一時的文字列

/********************************************************
 * dstarrepeaterd-yyyy-mm-dd.log を読み込み、
 * 処理の結果情報を取得する
 ********************************************************/
void dispstatus_ref(void)
{
	char	line2[256]		= {'\0'};
	char	fname[32]		= {'\0'};	// ファイル名
	char	mycall[8]		= {'\0'};
	char	urcall[8]		= {'\0'};
	char	dstarlogpath[32]	= {'\0'};	// D-STAR Repeater ログのフルパス
	char	status2[32] 		= {'\0'};
	/*
	 * ログファイルからリフレクタへのリンク情報を抽出する
	 */

	/* 日付入りログファイル名の作成 */
	timer = time(NULL);
	timeptr = gmtime(&timer);
	strftime(fname, sizeof(fname), "dstarrepeaterd-%Y-%m-%d.log", timeptr);
	sprintf(dstarlogpath, "%s%s", LOGDIR, fname);

	/* コマンドの標準出力オープン */
	sprintf(cmdline, "tail -n2 %s", dstarlogpath);
	if ((fp = popen(cmdline, "r")) != NULL )
	{

		/* 標準出力2行分を配列２つに取得し連結 */
		fgets(line,  sizeof(line),  fp);
		fgets(line2, sizeof(line2), fp);
		line[strlen(line) - 1]  = '\0';
		line2[strlen(line2) -1] = '\0';
		strcat(line, line2);

		/*
		 * リフレクタへの接続情報の取得
		 */
		if ((tmpptr = strstr(line, "Linked")) != NULL)
		{

			/* 一巡して全く同じ内容ならパス */
			if (strncmp(tmpptr, chklink, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
        			chklink[0] = '\0';
				strncpy(chklink, tmpptr, 60);

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
				sendcmd("IDLE.t1.txt=status.txt");

				/* ステータス２の表示 */
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);
				sendcmd("IDLE.t2.txt=status2.txt");
				sendcmd("MAIN.status_ref.txt=IDLE.status2.txt");
				sendcmd("MAIN.t1.txt=MAIN.status_ref.txt");
			}
		}


		/*
		 * リフレクタへのdisconnect を取得
		 */
		if ((tmpptr = strstr(line, "Not linked")) != NULL)
		{
			/* 一巡して全く同じ内容ならパス */
			if (strncmp(tmpptr, chklink2, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chklink2[0] = '\0';
				strncpy(chklink2, tmpptr, 60);

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

				reflesh_pages();
			}
		}


		/*
		 * RF ヘッダーの取得
		 */
		if ((tmpptr = strstr(line, "Radio header")) != NULL && rf_flag == 0)
		{
			/* 取得ログとダブりチェック用保存ログとの比較（違っていれば）*/
			if (strncmp(line, chkstat2, 60) != 0)
			{
				sendcmd("page DSTAR");

				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat2[0] = '\0';
				strncpy(chkstat2, line, 60);

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
				strncpy(urcall, tmpptr + 48, 8);
				urcall[8] = '\0';
				strcat(status2, mycall);
				status2[23] = '\0';

				/* ステータス２の表示 */
				sprintf(command, "DSTAR.t0.txt=\"R %s\"", mycall);
				sendcmd(command);
				sprintf(command, "DSTAR.t1.txt=\"%s\"", urcall);
				sendcmd(command);
				sendcmd("DSTAR.t2.txt=IDLE.ref.txt");

				/* ステータス2 を保存 */
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);

			}
			rf_flag = 1;
		}


		/*
		 * ネットワーク ヘッダーの取得
		 */
		if ((tmpptr = strstr(line, "Network header")) != NULL && net_flag == 0 )
		{
			/* 取得ログとダブりチェック用保存ログとの比較（違っていれば）*/
			if (strncmp(line, chkstat3, 60) != 0)
			{
				/* TX Delay */
				sleep(TXDELAY);

				sendcmd("page DSTAR");
				net_flag = 1;

				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat3[0] = '\0';
				strncpy(chkstat3, line, 60);

				/* ヘッダーログを取得 */
				status2[0] = '\0';

				/* JST 時刻の算出 */
				jstimer = time(NULL);
				jstimeptr = localtime(&jstimer);

				/* Network header の場合Net を表示 */
				strftime(status2, sizeof(status2), "Net %H:%M ", jstimeptr);

				/* ログよりコールサインMY, UR を取得 */
				strncpy(mycall, tmpptr + 30, 13);
				mycall[13] = '\0';
				strncpy(urcall, tmpptr + 51, 8);
				urcall[8] = '\0';
				strcat(status2, mycall);
				status2[23] = '\0';

				/* ステータス２の表示 */
				sprintf(command, "DSTAR.t0.txt=\"N %s\"", mycall);
				sendcmd(command);
				sprintf(command, "DSTAR.t1.txt=\"%s\"", urcall);
				sendcmd(command);
				sendcmd("DSTAR.t2.txt=IDLE.ref.txt");

				/* ステータス2 を保存 */
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);
			}
			net_flag = 1;
		}


		/*
		 * RF ラストパケット
		 */
		if ((tmpptr = strstr(line, "AMBE for")) != NULL && rf_flag == 1)
		{
			if (strncmp(line, chkstat2, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat2[0] = '\0';
				strncpy(chkstat2, line, 60);

				/* TX Hang */
				sleep(TXHANG);

				/* IDLE 画面に戻る */
				reflesh_pages();
			}
			rf_flag = 0;
		}


		/*
		 * ネットワークラストパケット
		 */
		if ((tmpptr = strstr(line, "Stats for")) != NULL && net_flag == 1)
		{
			if (strncmp(line, chkstat3, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat3[0] = '\0';
				strncpy(chkstat3, line, 60);

				/* TX Hang */
				sleep(TXHANG);

				/* IDLE 画面に戻る */
				reflesh_pages();
			}
			net_flag = 0;
		}

		/* 標準出力クローズ */
		pclose(fp);

	}
	return;
}



/************************************************************
	dmonitor のログファイルよりラストハード及び
	状況を取得し変数status に入れる
 ************************************************************/
void	dispstatus_dmon(void)
{

	char	ret[2]		= {'\0'};
	char	jitter_av[8]	= {'\0'};
	char	jitter_mx[8]	= {'\0'};
	char	jitter_mi[8]	= {'\0'};
	char	status[32]	= {'\0'};
	char	rptcall[9]	= {'\0'};
	int	stat		= 0;

	/* コマンドの標準出力オープン */
	strcpy(cmdline, "tail -n3 /var/log/dmonitor.log");
	if ((fp = popen(cmdline, "r")) == NULL)
	{
		printf("File open error!\n");
		return;
	}

	/* 過去のデータをクリアする  */
	memset(&status[0], '\0', sizeof(status));
	memset(&rptcall[0],'\0', sizeof(rptcall));

	/* 標準出力を配列に取得 */
	while ((fgets(line, sizeof(line), fp)) != NULL)
	{
		/* status に関する文字列があったら */
		if ((tmpptr = strstr(line, "from")) != NULL)
		{
			/* 日付時間とコールサインをログとして出力 */
			if ((strstr(line, "Connected") == NULL) && (strstr(line, "Last packet") == NULL))
			{
				memset(&status[0], '\0', sizeof(status));
				strncpy(status, line, 16);			// 日付と時刻
				strncat(status, tmpptr - 9, 8);			// Callsign
				status[24] = '\0';

				/* JST 時刻の算出 */
				jstimer = time(NULL);
				jstimeptr = localtime(&jstimer);

				/* LastheardとしてMAINページに表示 */
				strftime(tmpstr, sizeof(tmpstr), "%m.%d %H:%M ", jstimeptr);
				strncat(tmpstr, tmpptr - 9, 8);
				tmpstr[20] = '\0';
				sprintf(command, "MAIN.status_dmon.txt=\"%s\"", tmpstr);
				sendcmd(command);
				sendcmd("MAIN.t0.txt=MAIN.status_dmon.txt");
				stat = 0;

			}

			/* どこに接続したかを取得 */
			if ((tmpptr = strstr(line, "Connected")) != NULL)
			{
				strncpy(rptcall, tmpptr + 13, 8);
				rptcall[8] = '\0';
			}

			/* Last packet wrong ステータスの場合、文字を黄色に */
			if ((stat == 1) && (nx.debug == "1") && (strstr(line, "Last packet wrong") != NULL))
			{
				strcpy(status, "Last packet wrong...");
				break;
			}
		}

		/* dmonitorの開始とバージョンを取得 */
		if ((tmpptr = strstr(line, "dmonitor start")) != NULL)
		{
			memset(&status[0], '\0', sizeof(status));
			strncpy(status, tmpptr, 21);
		}

		/* バッファの拡張のサイズを取得 */
		if ((tmpptr = strstr(line, "New FiFo buffer")) != NULL)
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, tmpptr + 9);
			status[strlen(status) - 1] = '\0';
		}

		/* 接続解除を取得 */
		if ((nx.debug == "1") && (strstr(line, "dmonitor end") != NULL))
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "Disconnected");
		}

		/* 無線機の接続状況 */
		if ((nx.debug == "1") && (strstr(line, "init/re-init") != NULL))
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "Initializing RIG is done.");
		}

		/* ドロップパケット比の表示 */
		if ((nx.debug == "1") && ((tmpptr = strstr(line, "drop")) != NULL))
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "Drop PKT ");
			strcat(status, tmpptr + 17);
			status[strlen(status) - 1] = '\0';
			stat = 1;
		}
	}
	pclose(fp);

	/* 接続先の表示*/
	if ((strncmp(rptcall, "", 1) != 0) && (strncmp(rptcall, rptcallpre, 8) != 0))
	{
		strcpy(rptcallpre, rptcall);
		sprintf(command, "DMON.t1.txt=\"LINK TO : %s\"", rptcall);
		sendcmd(command);
		sprintf(command, "DMON.link.txt=\"LINK TO : %s\"", rptcall);
		sendcmd(command);
	}

	/* ステータス・ラストハードの表示 */
	if ((strncmp(status, "", 1) != 0) && (strncmp(status, statpre, 24) != 0))
	{
		strcpy(statpre, status);

		/* STATUS1 => STATUS2 */
//		sendcmd("DMON.stat2.txt=DMON.stat1.txt");

		/* 取得ステイタス=> STATUS1 */
		sprintf(command, "DMON.stat1.txt=\"%s\"", status);
		sendcmd(command);
//		sendcmd("DMON.t2.txt=DMON.stat1.txt");
//		sendcmd("DMON.t3.txt=DMON.stat2.txt");
//		sendcmd("USERS.t8.txt=DMON.stat1.txt");
//		sendcmd("USERS.t9.txt=DMON.stat2.txt");

		/* statusをクリアする */
		status[0] = '\0';
	}

	return;
}


/* sample
Nov 16 09:35:00 ham12 dmonitor[30968]: drop pakcet rate 0.00% (0/22)
Nov 16 09:35:00 ham12 dmonitor[30968]: jitter info. ave:20mSec. max:26mSec. min:17mSec.
Nov 16 09:35:20 ham12 dmonitor[30968]: dmonitor end
Nov 16 09:35:20 ham12 dmonitor[9408]: dmonitor start V01.27 (Compiled Nov 11 2019 12:50:00)
Nov 16 09:35:20 ham12 dmonitor[9408]: Connected to JP3YIY A (153.131.76.69:51000) from JE3HCZ D
Nov 16 09:35:21 ham12 dmonitor[9408]: RIG(ID-xxPlus) open
Nov 16 09:35:21 ham12 dmonitor[9408]: hole punch done.
Nov 16 09:35:21 ham12 dmonitor[9408]: RIG(ID-xxPlus) init/re-init done
*/
