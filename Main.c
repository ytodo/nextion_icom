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
//      D-STAR  Nextion display for ICOM Terminal/Access Mode
//
//      ファイル名      Main.c
//                      2020.03.05 -
//
//      機能    Multi_Forwardが搭載されているリピータリストを取得して
//              「接続可能リピータ」としてdmonitor用ディスプレイに表示。
//              タッチパネルから接続する
//              また、同様にターミナルモードのDStarRepeaterの接続と、
//              状態表示もし、双方を切れ替えて使用する
////////////////////////////////////////////////////////////////////////
#include        "Nextion.h"
#define         WAITTIME        0.5     // sec

int main(void)
{
	int	num;				// 返り値のi を受ける（件数）
	int	arraycount;
	int	fd;
	int	i;
	int	flag;
	int	bufcnt;
	char	concallpre[32]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

	/* GPIO シリアルポートのオープン*/
	fd = openport(SERIALPORT, BAUDRATE);

	/* 環境設定ファイルの読み取り */
	sendcmd("page MAIN");
	getconfig();

	///// dmonitor の準備 /////

	/* 現在利用可能なリピータリストの取得*/
	num = getlinkdata();

	/* 読み込んだリピータの総数を表示 */
	sprintf(command, "DMON.stat1.txt=\"Read %d Repeaters\"", num);
	sendcmd(command);
	sendcmd("DMON.t2.txt=DMON.stat1.txt");
	sendcmd("DMON.t3.txt=\"\"");

	/* 全リストを空にした後リピータ数分の文字配列にコールサインを格納 */
	for (i = 0; i < 21; i++)
	{
		sprintf(command, "RPTLIST.va%d.txt=\"\"", i);
		sendcmd(command);
		sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i, linkdata[i].call);
		sendcmd(command);
	}

	///// DStarRepeater の準備 /////



	reflesh_pages();



	/* 送・受信ループ */
	while(1)
	{
		/*
		 *	受信処理
		 */

		/* タッチパネルのデータを読み込む */
		recvdata(usercmd);

		/* もしタッチデータが選択されていない場合、初回のみデフォルトリピータをセットする */
		if ((strlen(usercmd) == 0) && (strlen(nx.default_rpt) != 0))
		{
			strcpy(usercmd, nx.default_rpt);
		}

		/* タッチデータが選択されている場合、前回と同じかチェック（同じならパス） */
		if ((strlen(usercmd) > 1) && (strcmp(usercmd, concallpre) != 0))
		{
			/* 比較後、保存変数をクリア */
			concallpre[0] = '\0';

			/* MAINのモードスイッチの状態を保存 */
			if (strncmp(usercmd, "dmonitor",       8) == 0)   st.mode = 1;
			if (strncmp(usercmd, "dstarrepeater", 13) == 0)	{ st.mode = 2;
								} else 	  st.mode = 0;

printf("%s    %s   %d\n", usercmd, concallpre, st.mode);

			/* 現在の返り値を保存 */
			strcpy(concallpre, usercmd);

			/* コマンドをスイッチに振り分ける */
			if (strncmp(usercmd, "restart",  7) == 0) flag = 1;
			if (strncmp(usercmd, "reboot",   6) == 0) flag = 2;
			if (strncmp(usercmd, "shutdown", 8) == 0) flag = 3;
			if (strncmp(usercmd, "Update",   6) == 0) flag = 4;
			if (strncmp(usercmd, "UP",       2) == 0) flag = 5;
			if (strncmp(usercmd, "DWN",      3) == 0) flag = 6;
			if (strncmp(usercmd, "USERS",    5) == 0) flag = 7;
			if (strncmp(usercmd, "next",	 4) == 0) flag = 8;
			if (strncmp(usercmd, "previous", 8) == 0) flag = 9;

			/* 比較後、usercmd をクリア */
			usercmd[0] = '\0';

			switch (flag) {
			case 1:						// ドライバのリスタート
				/* dmonitor モードのとき */
				if (st.mode == 1)
				{
					sendcmd("dim=10");
					dmonitor_restart();
					sendcmd("page DMON");
				}

				/* DStarRepeater モードのとき */
				if (st.mode == 2)
				{
	                                sendcmd("dim=10");
					dstarrepeater_restart();
					sendcmd("page IDLE");
				}

				break;

			case 2:						// 再起動
				sendcmd("dim=10");
				modem_stop();
				system("shutdown -r now");
				break;

                        case 3:						// シャットダウン
				sendcmd("dim=10");
				modem_stop();
				system("shutdown -h now");
				break;

			case 4:						// OS及びdmonitorのアップデート

				/* [Update]ボタンの表示を変える */
				sendcmd("SYSTEM.b4.bco=63488");
				sendcmd("SYSTEM.b4.pco=65535");
				sendcmd("SYSTEM.b4.txt=\"Checking Update\"");

				/* システムコマンドの実行 */
				system("killall -q -s 2 dmonitor");
				system("rm /var/run/dmonitor.pid");
				system("apt clean && apt update && apt install dmonitor");

				/* [REBOOT]の表示及びrebootコマンド発行 */
				sendcmd("SYSTEM.b4.bco=64512");
				sendcmd("SYSTEM.b4.txt=\"Restarting...\"");
				sendcmd("dim=10");
				system("systemctl restart nextion.service");
				break;

			case 5:						// バッファの増加
				if (strncmp(usercmd, "up", 2) == 0) break;
				strcpy(usercmd, "up");
				system("killall -q -s SIGUSR1 dmonitor");
				break;

			case 6:						// バッファの減少
				if (strncmp(usercmd, "dwn", 3) == 0) break;
				strcpy(usercmd, "dwn");
				system("killall -q -s SIGUSR2 dmonitor");
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

			default:

				/* dmonitorの時 */
				if (st.mode == 1)
				{
					/* 指定リピータに接続する */
					i = 0;
					system("systemctl restart auto_repmon.service");
					usleep(atoi(nx.microsec) * 10000);	// リスト読み込み完了を確実にするウェイト
					for (i = 0; i < num; i++)
					{
						if (strncmp(linkdata[i].call, usercmd, 8) == 0)
						{
							/* 現在稼働中のdmonitor をKILL */
							system("killall -q -s 2 dmonitor");
							system("rm /var/run/dmonitor.pid");
							system("rig_port_check");

							/* 接続コマンドの実行 */
							sprintf(command, "dmonitor '%s' %s %s '%s' '%s'", nx.station, linkdata[i].addr, linkdata[i].port, linkdata[i].call, linkdata[i].zone);
//							sendcmd("page DMON");
							system(command);
							break;
						}
					}
					sendcmd("page DMON");
				}

				/* DStarRepeaterの時 */
				if (st.mode == 2)
				{
					sendcmd("page IDLE");
				}

			}
			reflesh_pages();
			flag = 0;
		}


		/*
		 *	送信処理
		 */

		///// MAINページ /////

		/* MAINへの簡易ラストハード表示 */
		dispstatus_dmon();
		dispstatus_ref();

		///// IDLE (REF Main) /////

		/* CPU 温度の表示 */
		disptemp();
		/* 日付･時刻表示 */
		jstimer = time(NULL);
		jstimeptr = localtime(&jstimer);
		strftime(tmpstr, sizeof(tmpstr), "%Y.%m.%d %H:%M:%S ", jstimeptr);
		sprintf(command, "MAIN.t2.txt=\"%s\"", tmpstr);
		sendcmd(command);
	}
	return(0);
}
