////////////////////////////////////////////////////////////////////////
// 	D-STAR  Nextion display for ICOM Terminal/Access Mode
//
//	ファイル名	dstarrepeater.c
//			2020.03.05 - 2021.02.17
//
//	機能	ircDDBGatewayとDStarRepeaterによるシステムに
//		アドオンとして常駐し状況をNextionに表示したり
//		Nextionコマンドによりリフレクタの接続を変更し、
//		また、システムコマンドを実行する。
//
////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"


void dstarrepeater(void)
{
	int	i;
	int	flag;
	char	chkusercmd[32]	= {'\0'};
	char	refcall[9]	= {'\0'};
	char	nodecall[9]	= {'\0'};

        char    fname[32]               = {'\0'};       // ファイル名
        char    dstarlogpath[32]        = {'\0'};       // D-STAR Repeater ログのフルパス


	system("sudo /usr/bin/rig_port_check");
	system("sudo systemctl restart dstarrepeater");

        /* 日付入りログファイル名の作成 */
        timer = time(NULL);
        timeptr = gmtime(&timer);
        strftime(fname, sizeof(fname), "-%Y-%m-%d.log", timeptr);
        sprintf(dstarlogpath, "%s%s%s", LOGDIR, DSLOGFILE, fname);

        /* コマンドの標準出力をファイルへ */
	system("sudo pkill tail");			// 重複禁止処理
	sprintf(cmdline, "tail -f %s | grep -E 'Stats|AMBE|Linked to|Not linked|Radio|Network' --line-buffered > /tmp/tmplog.txt &", dstarlogpath);
	system(cmdline);

	/* メインスクリーンの初期設定 */
	sendcmd("dim=dims");
	sendcmd("page IDLE");

	/* グローバル変数の初期設定 */
	sprintf(command, "IDLE.station.txt=\"%s\"", ds.station);	// ノードコールサイン
	sendcmd(command);
	sendcmd("t20.txt=\"\"");
	sendcmd("t1.pco=2016");
	sendcmd("t1.txt=\"Linking...\"");
	sprintf(command, "IDLE.type.txt=\"%s\"", ds.modemtype);		// リピータ形式
	sendcmd(command);

	usercmd[0] = '\0';
	usleep(WAITTIME * 30);	// 3secs

	/* 送・受信ループ */
	while (1) {

		/*
		 * 送信処理
		 */

		/* CPU 温度の表示 */
		disptemp();

		/* ログステータスの読み取り */
		dispstatus_ref();


		/*
		 * 受信処理
		 */

		/* タッチパネルのデータを読み込む */
		recvdata(usercmd);

		/* もしタッチデータが選択されていない場合、初回のみデフォルトリフレクタをセットする */
		if ((strlen(usercmd) == 0) && (strlen(chkusercmd) == 0) && (strlen(nx.default_ref) != 0))
		{
			/* ircDDBGateway コマンド形式に変換 */
			strncpy(usercmd, nx.default_ref, 6);
			usercmd[6] = '\0';
			strncat(usercmd, &nx.default_ref[7], 1);
			strcat(usercmd, "L");
			usercmd[8] = '\0';
		}

		/* タッチデータが選択されている場合、前回と同じかチェック（同じならパス） */
		if ((strlen(usercmd) > 0) && (strncmp(usercmd, chkusercmd, 8) != 0))
		{
			/* 比較後、保存変数をクリア */
			chkusercmd[0] = '\0';

			/* 現在の返り値を保存 */
			strncpy(chkusercmd, usercmd, 8);

			/* リフレクタ接続 */
			if (strncmp(usercmd, "REF", 3) == 0 || strncmp(usercmd, "XLX", 3) == 0 ||
			    strncmp(usercmd, "DCS", 3) == 0 || strncmp(usercmd, "XRF", 3) == 0 || (strlen(usercmd) == 1 && strncmp(usercmd, "U", 1) == 0))
			{
				/* ノードコールサインの整形 */
				strncpy(nodecall, ds.station, 8);
				for (i = 0; i < 8; i++)
				{
					if (!strncmp(&nodecall[i], " ", 1))
					{
						nodecall[i] = '_';
					}
				}

				/* リフレクタ接続解除 */
				if (strncmp(usercmd, "U", 1) == 0)
				{
					sprintf(cmdline, "remotecontrold %s unlink", nodecall);
					system(cmdline);
				}
				else
				{
					/* リフレクタ接続コマンドの整形 */
					strncpy(refcall, usercmd, 6);
					refcall[6] = '\0';
					strcat(refcall, "_");
					strncat(refcall, &usercmd[6], 1);

					sprintf(cmdline, "remotecontrold %s link never %s", nodecall, refcall);
					system(cmdline);
				}
				sendcmd("page IDLE");
			}
			else
			{
				/* リフレクタ接続でなかった時 */
				syscmdswitch();
			}
		}
		usleep(nx.microsec * 30);
	}	// Loop

	return;
}
