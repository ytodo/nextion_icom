////////////////////////////////////////////////////////////////////////
// 	D-STAR  Nextion display for ICOM Terminal/Access Mode
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
	int	bufcnt;
	char	chkusercmd[8]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

        /* GPIO シリアルポートのオープン*/
        fd = openport(SERIALPORT, BAUDRATE);

	/* メインスクリーンの初期設定 */
	sendcmd("dim=dims");
	sendcmd("page DMON");

	/* 現在利用可能なリピータリストの取得*/
	st.num = getlinkdata();

	sprintf(command, "DMON.station.txt=\"STATION : %s\"", nx.station);
	sendcmd(command);

	/* 読み込んだリピータの総数を表示 */
	sprintf(command, "DMON.stat1.txt=\"Read %d Repeaters\"", st.num);
	sendcmd(command);
	sendcmd("DMON.t3.txt=\"\"");

	/* 全リストを空にした後リピータ数分の文字配列にコールサインを格納 */
	for (i = 0; i < 21; i++)
	{
		sprintf(command, "RPTLIST.va0.txt=\"%s\"", linkdata[0].call);
		sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i, linkdata[i].call);
		sendcmd(command);

	}

	/* チェックしたIPアドレスをSYSTEM pageに表示 */
	sprintf(command, "SYSTEM.va0.txt=\"%s\"", ds.ipaddress);
	sendcmd(command);


	/* 送・受信ループ */
	while (1)
	{
		/* タッチパネルのデータを読み込む */
		recvdata(usercmd);

		/* もしタッチデータが選択されていない場合、初回のみデフォルトリピータをセットする */
		if ((strlen(usercmd) == 0) && (strlen(nx.default_rpt) != 0))
		{
			strcpy(usercmd, nx.default_rpt);
		}

		/* タッチデータが選択されている場合、前回と同じかチェック（同じならパス） */
		if ((strlen(usercmd) > 1) && (strncmp(usercmd, chkusercmd, 8) != 0))
		{
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
					/* 現在稼働中のdmonitor をKILL */
					system("sudo killall -q -s 2 dmonitor");
					system("sudo rm /var/run/dmonitor.pid");
					system("sudo rig_port_check");

					/* 接続コマンドの実行 */
					sprintf(command, "sudo dmonitor '%s' %s %s '%s' '%s'", nx.station, linkdata[i].addr, linkdata[i].port, linkdata[i].call, linkdata[i].zone);

					/* killした後、disconnectの表示を待って再接続 */
					usleep(atoi(nx.microsec) * 100);
					system(command);
					flag = 1;		// リピータ接続だった時 1 となる
				}
			}
			if (flag != 1) syscmdswitch();		// リピータ接続でなかった時 SYSTEM COMMANDへ

		}

		/* ステータス・ラストハードの読み取り */
		dispstatus_dmon();

	}		// Loop


	/* GPIO シリアルポートのクローズ*/
	close(fd);

	return;
}
