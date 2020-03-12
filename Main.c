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
	sendcmd("page MAIN");
	getconfig();

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
		if ((strlen(usercmd) > 1) && (strcmp(usercmd, chkusercmd) != 0))
		{
			/* 比較後、保存変数をクリア */
			chkusercmd[0] = '\0';

			/* 現在の返り値を保存 */
			strcpy(chkusercmd, usercmd);

			/* MAINのモードスイッチの状態を保存 */
			if (strncmp(usercmd, "dmonitor", 8) == 0)
			{
				st.mode = 1;
				sendcmd("page DMON");
				dmonitor();
			}
			if (strncmp(usercmd, "dstarrpt", 8) == 0)
			{
				st.mode = 2;
				sendcmd("page IDLE")
				dstarrepeater();
			}
		}
		reflesh_pages();
		flag = 0;


		/*
		 *	送信処理
		 */

		///// MAINページ /////

		/* MAINへの簡易ラストハード表示 */
		dispstatus_dmon();
		dispstatus_ref();

		/* 日付･時刻表示 */
		jstimer = time(NULL);
		jstimeptr = localtime(&jstimer);
		strftime(tmpstr, sizeof(tmpstr), "%Y.%m.%d %H:%M:%S ", jstimeptr);
		sprintf(command, "MAIN.t2.txt=\"%s\"", tmpstr);
		sendcmd(command);
	}

	/* GPIO シリアルポートのクローズ */
	close(fd);
	return(EXIT_SUCCESS);
}
