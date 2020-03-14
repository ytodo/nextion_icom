/*
 *  Copyright (C) 2020- by Yosh Todo JE3HCZ
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  本プログラムはD-STAR Network の一部の機能を実現するための物で、
 *  アマチュア無線の技術習得とその本来の用途以外では使用しないでください。
 *
 */

////////////////////////////////////////////////////////////////////////
// 	D-STAR  Nextion display for ICOM Terminal/Access Mode
//
//	ファイル名	Main.c 
//			2020.03.05 - 
//
//	機能	Multi_Forwardが搭載されているリピータリストを取得して
//		「接続可能リピータ」としてdmonitor用ディスプレイに表示。
//		タッチパネルから接続する
//		また、同様にターミナルモードのDStarRepeaterの接続と、
//		状態表示もし、双方を切れ替えて使用する
////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"
#define		WAITTIME	0.5	// sec

void dmonitor(void)
{
	int	num;				// 返り値のi を受ける（件数）
	int	arraycount;
	int	fd;
	int	i;
	int	flag;
	int	bufcnt;
	char	chkusercmd[8]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

	/* 現在利用可能なリピータリストの取得*/
	num = getlinkdata();

	/* メインスクリーンの初期設定 */
	sendcmd("dim=dims");
	sendcmd("page DMON");

	sprintf(command, "DMON.station.txt=\"STATION : %s\"", nx.station);
	sendcmd(command);
//	sprintf(command, "t0.txt=\"STATION : %s\"",nx.station);
//	sendcmd(command);
//	sendcmd("t1.txt=\"LINK TO : NONE\"");
//	sendcmd("link.txt=\"LINK TO : NONE\"");

	/* 読み込んだリピータの総数を表示 */
	sprintf(command, "DMON.stat1.txt=\"Read %d Repeaters\"", num);
	sendcmd(command);
//	sendcmd("DMON.t2.txt=DMON.stat1.txt");
	sendcmd("DMON.t3.txt=\"\"");

	/* 全リストを空にした後リピータ数分の文字配列にコールサインを格納 */
	for (i = 0; i < 21; i++)
	{
		sprintf(command, "RPTLIST.va%d.txt=\"\"", i);
		sendcmd(command);
		sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i, linkdata[i].call);
		sendcmd(command);
	}

	/* チェックしたIPアドレスをSYSTEM pageに表示 */
	sprintf(command, "SYSTEM.va0.txt=\"%s\"", ds.ipaddress);
	sendcmd(command);

//	reflesh_pages();

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

printf("usercmd: %s  chk: %s\n", usercmd, chkusercmd);

			/* コマンドをスイッチに振り分ける */
			if (strncmp(usercmd, "restart",	 7) == 0) flag =  1;
			if (strncmp(usercmd, "reboot",	 6) == 0) flag =  2;
			if (strncmp(usercmd, "poweroff", 8) == 0) flag =  3;
			if (strncmp(usercmd, "update",	 6) == 0) flag =  4;
			if (strncmp(usercmd, "UP",	 2) == 0) flag =  5;
			if (strncmp(usercmd, "DWN",	 3) == 0) flag =  6;
			if (strncmp(usercmd, "USERS",	 5) == 0) flag =  7;
			if (strncmp(usercmd, "next",	 4) == 0) flag =  8;
			if (strncmp(usercmd, "previous", 8) == 0) flag =  9;
			if (strncmp(usercmd, "return",	 6) == 0) flag = 10;

			switch (flag) {
			case 1:						// nextionドライバのリスタート
				sendcmd("dim=10");
				system("sudo killall -q -s 2 dmonitor");
				system("sudo rm /var/run/dmonitor.pid");
				system("sudo systemctl restart nextion.service");
				sendcmd("dim=dims");
				break;

			case 2:						// 再起動
				sendcmd("dim=10");
				system("sudo killall -q -s 2 dmonitor");
				system("sudo rm /var/run/dmonitor.pid");
				system("sudo shutdown -r now");
				break;

			case 3:						// シャットダウン
				sendcmd("dim=10");
				system("sudo killall -q -s 2 dmonitor");
				system("sudo rm /var/run/dmonitor.pid");
				system("sudo shutdown -h now");
				break;

			case 4:						// OS及びdmonitorのアップデート
				/* システムコマンドの実行 */
				system("sudo killall -q -s 2 dmonitor");
				system("sudo rm /var/run/dmonitor.pid");
				system("sudo apt clean && apt update && apt install dmonitor");

				sendcmd("dim=10");
				system("sudo systemctl restart nextion.service");
				break;

			case 5:						// バッファの増加
				if (strncmp(usercmd, "up", 2) == 0) break;
				strcpy(usercmd, "up");
				system("sudo killall -q -s SIGUSR1 dmonitor");
				break;

			case 6:						// バッファの減少
				if (strncmp(usercmd, "dwn", 3) == 0) break;
				strcpy(usercmd, "dwn");
				system("sudo killall -q -s SIGUSR2 dmonitor");
				break;

			case 7:						// Remote Usersパネルへ接続ユーザ表示
				sendcmd("page USERS");
				sprintf(command, "USERS.b0.txt=\"LINKED USERS on %s\"", rptcallpre);
				sendcmd(command);
				getusers();
				strcpy(usercmd, "Return");
				break;

			case 8:
				break;

			case 9:
				previous_page(num);
				break;

			case 10:
				system("sudo killall -q -s 2 dmonitor");
				system("sudo rm /var/run/dmonitor.pid");
				sendcmd("page MAIN");
				return;

			default:

				/* 指定リピータに接続する */
				i = 0;
//				system("systemctl restart auto_repmon.service");
//				usleep(atoi(nx.microsec) * 100);		// リスト読み込み完了を確実にするウェイト
//				num = getlinkdata();
				for (i = 0; i < num; i++)
				{
					if (strncmp(linkdata[i].call, usercmd, 8) == 0)
					{
						/* 現在稼働中のdmonitor をKILL */
						system("sudo killall -q -s 2 dmonitor");
						system("sudo rm /var/run/dmonitor.pid");
						system("sudo rig_port_check");

						/* 接続コマンドの実行 */
						sprintf(command, "sudo dmonitor '%s' %s %s '%s' '%s'", nx.station, linkdata[i].addr, linkdata[i].port, linkdata[i].call, linkdata[i].zone);
printf("%s\n", command);
						/* killした後、disconnectの表示を待って再接続 */
						usleep(atoi(nx.microsec) * 100);
						system(command);
					}
				}
			}
			flag = 0;
		}

		/* ステータス・ラストハードの読み取り */
		dispstatus_dmon();
//		reflesh_pages();


	}		// Loop


	/* GPIO シリアルポートのクローズ*/
	close(fd);

	return;
}
