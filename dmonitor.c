////////////////////////////////////////////////////////////////////////
//	D-STAR  Nextion display for ICOM Terminal/Access Mode
//
//	ファイル名	dmonitor.c
//			2020.03.05 - 2025.04.19
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
	int	i;
	int	flag;
	char	chkusercmd[8]	= {'\0'};
	char	tmpstr[32]		= {'\0'};
	char    dmonlogcmd[256] = {'\0'};

	/*
	 *	起動と初期表示設定
	 */

	/* dmonitor関連サービスの起動 */
	if (AUTOREPMON == "auto_repmon_light")
	{
		system("sudo systemctl stop auto_repmon");
	}
	sprintf(command, "sudo systemctl restart %s", AUTOREPMON);
	system(command);
	system("sudo systemctl restart rpt_conn");


    /* dmonitor.logのうち必要な項目のみのリスト作成 */
	system("sudo pkill tail");
	sprintf(dmonlogcmd, "tail -f %s%s | grep -E 'dmonitor start|dmonitor end|Connected|Frequency|init|from|drop packet|FiFo' --line-buffered > /tmp/tmplog.txt &", LOGDIR, DMLOGFILE);
	system(dmonlogcmd);

	/* 現在利用可能なリピータリストの取得*/
	st.num = getlinkdata();
	st.selected_page = -1;				// リピータリストページ初回無表示対策
	next_page();						// Thanks JA1UXX

	/* dmonitorスクリーンの初期設定 */
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

	/* チェックしたIPアドレスをSYSTEM pageに表示 */
	sprintf(command, "SYSTEM.va0.txt=\"%s\"", ds.ipaddress);
	sendcmd(command);

	/* dmonitor専用ページへ移行 */
	sendcmd("page DMON");

	/*
	 *	 送・受信ループ
	 */
	while (1)
	{
		/* タッチされたデータを読み込む */
		recvdata(usercmd);

		/* RFCommand データが入っている場合 */
		if ((strlen(usercmd) == 0) && (strncmp(rfcommand, "/", 1) == 0))
			strcpy(usercmd, rfcommand);

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
				/* リピータリストに指定したコールサインが有った場合接続手順に入る */
				if (strncmp(linkdata[i].call, usercmd, 8) == 0)
				{
					sendcmd("dim=dims");
					sendcmd("page DMON");

					/* 実行前処理 スキャン中なら止める */
					system("sudo killall -q -9 repeater_scan");

					/* dmonitorを完全に終了させる */
					system("sudo killall -q -2 dmonitor");
					system("sudo rm -f /var/run/dmonitor.pid");
					usleep(nx.microsec * 300);

					/* リピータコネクタを停止する */
					system("sudo systemctl stop rpt_conn");
					system("sudo killall -q -9 rpt_conn");
					system("sudo rm -f /var/run/rpt_conn.pid");

					/* リグ又はモデムポートをチェックする */
					system("sudo /usr/bin/rig_port_check");
					system("ulimit -c unlimited");

					/* 接続コマンドの実行 */
					sprintf(command, "sudo /usr/bin/dmonitor '%s' %s %s '%s' '%s'", nx.station, linkdata[i].addr, linkdata[i].port, linkdata[i].call, linkdata[i].zone);
					system(command);
					flag = 1;		// リピータ接続だった時 1 となる
				}
			}

			/* リピータ接続でなかった時 */
			if (flag == 0) syscmdswitch();
		}

		/* ステータス・ラストハードの読み取り */
		dispstatus_dmon();

		/* 無線機からのコマンドを接続解除の間受け取る準備 */
		if (strcmp(rfcommand, "UNLINK") == 0)
		{
			system("sudo killall -q -2 dmonitor");
			system("sudo rm -f /var/nun/dmonitor.pid");
			usleep(nx.microsec * 300);
			system("sudo systemctl start rpt_conn");
	 		status[0] = '\0';
			rfcommand[0] = '\0';
		}

		/* CPUの速さによるループ調整（nextion.ini:SLEEPTIME）*/
		usleep(nx.microsec * 5);

	} // Loop

	return;
}
