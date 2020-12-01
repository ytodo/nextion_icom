////////////////////////////////////////////////////////////////////////
//	D-STAR  Nextion display for ICOM Terminal/Access Mode
//
//	ファイル名	dmonitor.c
//			2020.03.05 -
//
//	機能	Multi_Forwardが搭載されているリピータリストを取得して
//		「接続可能リピータ」としてdmonitor用ディスプレイに表示。
//		タッチパネルから接続する
//		また、同様にターミナルモードのDStarRepeaterの接続と、
//		状態表示もし、双方を切れ替えて使用する
////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"

void dmonitor(void)
{
	int	arraycount;
	int	fd;
	int	i;
	int	flag;
	char	chkusercmd[8]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

	/* dmonitor関連サービスの起動 */
	system("sudo systemctl restart auto_repmon");
	system("sudo systemctl restart rpt_conn");

	/* 現在利用可能なリピータリストの取得*/
	st.num = getlinkdata();

	/* メインスクリーンの初期設定 */
	sendcmd("dim=dims");
	sendcmd("page DMON");

//	sprintf(command, "DMON.version.txt=\"Ver.%d.%d.%d\"", VERSION, VERSUB, RELEASE);
//	sendcmd(command);
//	sendcmd("MAIN.t6.txt=version.txt");	// バージョン表示

	sprintf(command, "DMON.station.txt=\"STATION : %s\"", nx.station);
	sendcmd(command);
	sprintf(command, "DMON.t0.txt=\"STATION : %s\"", nx.station);
	sendcmd(command);
	sendcmd("DMON.t1.txt=\"LINK TO : NONE\"");
	sendcmd("DMON.link.txt=\"LINK TO : NONE\"");

	/* Nextionの初期化と読み込んだリピータ総数の表示 */
	sendcmd("DMON.t2.txt=\"\"");
	sendcmd("DMON.t3.txt=\"\"");
	sendcmd("DMON.stat1.txt=\"\"");
	sendcmd("DMON.stat2.txt=\"\"");
	sprintf(command, "DMON.stat1.txt=\"Reading %d Repeaters\"", st.num);
	sendcmd(command);
	sendcmd("DMON.t2.txt=DMON.stat1.txt");


	/* 全リストを空にした後リピータ数分の文字配列にコールサインを格納 */
	for (i = 0; i < 21; i++)
	{
		sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i, linkdata[i].call);
		sendcmd(command);
	}

	/* チェックし	たIPアドレスをSYSTEM pageに表示 */
	sprintf(command, "SYSTEM.va0.txt=\"%s\"", ds.ipaddress);
	sendcmd(command);


	/* 送・受信ループ */
	while (1)
	{
		/* タッチされたデータを読み込む */
		recvdata(usercmd);

		/* もしタッチデータが選択されていない場合、初回のみデフォルトリピータをセットする */
		if ((strlen(usercmd) == 0) && (strlen(chkusercmd) == 0) && (strlen(nx.default_rpt) != 0))
		{
			strcpy(usercmd, nx.default_rpt);
		}

		/* タッチデータが選択されている場合、前回と同じかチェック（同じならパス） */
		if ((strlen(usercmd) > 1 && strncmp(usercmd, chkusercmd, 8) != 0) || ((strncmp(usercmd, "next", 4) == 0) || (strncmp(usercmd, "previous", 8) == 0)))
		{
			if (((strncmp(usercmd, "next", 4) == 0) && (strncmp(chkusercmd, "next", 4) == 0)) ||
				((strncmp(usercmd, "previous", 8) == 0) && (strncmp(chkusercmd, "previous", 8) == 0)))
			{
				strncpy(usercmd, "dummy", 5);
				usercmd[5] = '\0';
				continue;
			}

			/* 比較後、保存変数をクリア */
			chkusercmd[0] = '\0';

			/* 現在の返り値を保存 */
			strncpy(chkusercmd, usercmd, 8);

			/* 指定リピータに接続する */
			i = 0;
			flag = 0;
			for (i = 0; i < st.num; i++)
			{
				if (strncmp(linkdata[i].call, usercmd, 8) == 0)
				{
					/* 接続コマンド実行前処理 */
					system("sudo systemctl stop rpt_conn");
					system("sudo killall -q -9 repeater_scan");
					system("sudo killall -q -9 dmonitor");
					system("sudo rm -f /var/run/dmonitor.pid");
					system("sudo killall -q -9 rpt_conn");
					system("sudo rm -f /var/run/rpt_conn.pid");
					system("sudo rig_port_check");
					system("sudo cp /dev/null /var/tmp/update.log");
					system("sudo cp /var/www/html/error_msg.html.save /var/tmp/error_msg.html");
					system("sudo touch /var/tmp/error_msg.html");
					system("sudo cp /var/www/html/short_msg.html.save /var/tmp/short_msg.html");
					system("sudo touch /var/tmp/short_msg.html");
					system("ulimit -c unlimited");

					/* 接続コマンドの実行 */
					sprintf(command, "sudo /usr/bin/dmonitor '%s' %s %s '%s' '%s'", nx.station, linkdata[i].addr, linkdata[i].port, linkdata[i].call, linkdata[i].zone);
					system(command);
					sendcmd("page DMON");
					flag = 1;		// リピータ接続だった時 1 となる
				}
			}

			/* リピータ接続でなかった時 */
			if (flag == 0) syscmdswitch();
		}

		/* ステータス・ラストハードの読み取り */
		dispstatus_dmon();

		/* 無線機からのコマンドを接続解除の間受け取る準備 */
		if (strcmp(status, "Disconnected") == 0)
		{
			system("sudo killall -q -2 dmonitor");
			system("sudo rm -f /var/nun/dmonitor.pid");
			usleep(atoi(nx.microsec) * 10);
			system("sudo systemctl restart rpt_conn");
			status[0] = '\0';
		}
		usleep(atoi(nx.microsec) * 5);

	} // Loop

	return;
}
