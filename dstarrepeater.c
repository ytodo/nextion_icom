/*
 *  Copyright (C) 2018-2020 by Yosh Todo JE3HCZ
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
//	ファイル名	dstarrepeater.c 
//			2020.03.05 - 
//
//	機能	ircDDBGatewayとDStarRepeaterによるシステムに
//		アドオンとして常駐し状況をNextionに表示したり
//		Nextionコマンドによりリフレクタの接続を変更し、
//		また、システムコマンドを実行する。
//
////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"
#define		WAITTIME 0.5  // sec

void dstarrepeater(void)
{
	int	fd;
    	int	i;
    	int	flag;
    	char	command[64]	= {'\0'};
    	char	usercmd[32]	= {'\0'};
    	char	tmpstr[32]	= {'\0'};

	/* GPIO シリアルポートのオープン*/
	fd = openport(SERIALPORT, BAUDRATE);

	/* DStarRepeaterを再起動する */
//	system("sudo systemctl restart dstarrepeater.service");
//	system("sudo systemctl restart ircddbgateway.service");

	/* グローバル変数の初期設定 */
	sprintf(command, "IDLE.station.txt=\"%s\"", ds.station);	// ノードコールサイン
	sendcmd(command);
	sendcmd("IDLE.status.txt=IDLE.ref.txt");			// ステータス
//        dispipaddr();							// IPアドレス
	sprintf(command, "IDLE.type.txt=\"%s\"", ds.modemtype);		// リピータ形式
	sendcmd(command);

	/* グローバル変数の値を画面表示 */
	sendcmd("page IDLE");

	reflesh_pages();						// IDLE 画面の表示ルーティン

	/* 送・受信ループ */
	while (1) {

		/*
		 * 受信処理
		 */

		/* タッチパネルのデータを読み込む */
		recvdata(usercmd);

		/* コマンドをスイッチに振り分ける */
		if (strncmp(usercmd, "restart",  7) == 0) flag = 1;
		if (strncmp(usercmd, "reboot",   6) == 0) flag = 2;
		if (strncmp(usercmd, "shutdown", 8) == 0) flag = 3;
		if (strncmp(usercmd, "return",   6) == 0) flag = 9;

		switch (flag)
		{
			case 1:				// restart
				sendcmd("dim=10");
				system("sudo systemctl restart nextion.service");
				system("sudo systemctl stop dstarrepeater.service");
				system("sudo systemctl start dstarrepeater.service");
				sendcmd("page IDLE");

				break;

			case 2:				// reboot
				sendcmd("dim=10");
				system("sudo shutdown -r now");
				break;

			case 3:				// shutdown
				sendcmd("dim=10");
				system("sudo shutdown -h now");
				break;

			case 9:				// return
				system("sudo systemctl stop dstarrepeater.service");
				sendcmd("page MAIN");
				return;

			default:
//				system("sudo systemctl stop ircddbgateway.service");
//				system("sudo systemctl start ircddbgateway.service");
				break;
		}

		/*
		 * 送信処理
		 */

		/* CPU 温度の表示 */
		disptemp();

		/* ログステータスの読み取り */
		dispstatus_ref();

//		sleep(WAITTIME);
	}

	/* GPIO シリアルポートのクローズ*/
	close(fd);
	return;
}

