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
	char	chkusercmd[32]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

	/* GPIO シリアルポートのオープン*/
	fd = openport(SERIALPORT, BAUDRATE);

	/* 環境設定ファイルの読み取り */
	getconfig();
	sendcmd("page MAIN");

        /* チェックしたIPアドレスをSYSTEM pageに表示 */
//        sprintf(command, "SYSTEM.va0.txt=\"%s\"", ds.ipaddress);
//        sendcmd(command);

	dispipaddr();
	reflesh_pages();



	/* 送・受信ループ */
	while(1)
	{
		/* MAINへの簡易ラストハード表示 */
//		dispstatus_dmon();
//		dispstatus_ref();

		/* 日付･時刻表示 */
		dispclock();

		/* タッチパネルのデータを読み込む */
		recvdata(usercmd);

		/* タッチデータが選択されている場合、前回と同じかチェック（同じならパス） */
		if ((strlen(usercmd) > 4) && (strncmp(usercmd, chkusercmd, 8) != 0))
		{
			/* 比較後、保存変数をクリア */
			chkusercmd[0] = '\0';

			/* 現在の返り値を保存 */
			strncpy(chkusercmd, usercmd, 8);

	                /* コマンドをスイッチに振り分ける */
        	        if (strncmp(usercmd, "restart",  7) == 0) flag = 1;
                	if (strncmp(usercmd, "reboot",   6) == 0) flag = 2;
	                if (strncmp(usercmd, "shutdown", 8) == 0) flag = 3;
			if (strncmp(usercmd, "dmonitor", 8) == 0) flag = 4;
			if (strncmp(usercmd, "dstarrpt", 8) == 0) flag = 5;

	                switch (flag)
        	        {
                	        case 1:                         // restart
                        	        sendcmd("dim=10");
                                	system("sudo systemctl stop ircddbgateway.service");
	                                system("sudo systemctl stop dstarrepeater.service");
					system("sudo killall -q -s 2 dmonitor");
					system("sudo rm /var/run/dmonitor.pid");
					system("sudo killall -q -s 9 sleep");
                	                sendcmd("page MAIN");
                        	        break;

	                        case 2:                         // reboot
        	                        sendcmd("dim=10");
                                	system("sudo systemctl stop ircddbgateway.service");
	                                system("sudo systemctl stop dstarrepeater.service");
					system("sudo killall -q -s 2 dmonitor");
					system("sudo rm /var/run/dmonitor.pid");
					system("sudo killall -q -s 9 sleep");
                	                system("sudo shutdown -r now");
                        	        break;

	                        case 3:                         // shutdown
        	                        sendcmd("dim=10");
                                	system("sudo systemctl stop ircddbgateway.service");
	                                system("sudo systemctl stop dstarrepeater.service");
					system("sudo killall -q -s 2 dmonitor");
					system("sudo rm /var/run/dmonitor.pid");
					system("sudo killall -q -s 9 sleep");
                	                system("sudo shutdown -h now");
                        	        break;

				case 4:
					dmonitor();
					break;

				case 5:

				        /* DStarRepeaterを再起動する */
				        system("sudo systemctl restart dstarrepeater.service");
				        system("sudo systemctl restart ircddbgateway.service");
					dstarrepeater();


					break;
	                        default:
                	                break;
                	}

			/* MAINのモードスイッチの状態を保存 */
//			if (strncmp(usercmd, "dmonitor", 8) == 0)
//			{
//				st.mode = 1;
//				dispstatus_dmon();
//				dmonitor();
//			}
//			if (strncmp(usercmd, "dstarrpt", 8) == 0)
//			{
//				st.mode = 2;
//				dispstatus_ref();
//				dstarrepeater();
//			}
		}
//		reflesh_pages();
//		flag = 0;

	}

	/* GPIO シリアルポートのクローズ */
	close(fd);

	return(EXIT_SUCCESS);
}
