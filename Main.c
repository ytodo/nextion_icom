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

int main(void)
{
	int	num;				// 返り値のi を受ける（件数）
	int	arraycount;
	int	fd;
	int	i = 0;
	char	chkusercmd[32]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

	/* GPIO シリアルポートのオープン*/
	fd = openport(SERIALPORT, BAUDRATE);

	/* メインスクリーンの初期設定 */
	sendcmd("dim=dims");
	sendcmd("page MAIN");
	usercmd[0] = '\0';

	/* 設定項目の取得と表示 */
	getconfig();
	dispipaddr();

	/* 送・受信ループ */
	while(1)
	{
		/* MAINへの簡易ラストハード表示 */
		dispstatus_dmon();
//		dispstatus_ref();

		/* 日付･時刻表示 */
		dispclock();
		usleep(WAITTIME * 5);		// 0.5秒

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
			syscmdswitch();
		}
	}

	/* GPIO シリアルポートのクローズ */
	close(fd);

	return(EXIT_SUCCESS);
}
