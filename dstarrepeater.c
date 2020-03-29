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

void dstarrepeater(void)
{
	int	fd;
    	int	i;
    	int	flag;
    	char	command[64]	= {'\0'};
	char	chkusercmd[32]	= {'\0'};
    	char	tmpstr[32]	= {'\0'};

        /* メインスクリーンの初期設定 */
        sendcmd("dim=dims");
        sendcmd("page IDLE");

	/* グローバル変数の初期設定 */
	sprintf(command, "IDLE.station.txt=\"%s\"", ds.station);	// ノードコールサイン
	sendcmd(command);
	sendcmd("t1.pco=2016");
	sendcmd("t1.txt=\"Linking...\"");
	sprintf(command, "IDLE.type.txt=\"%s\"", ds.modemtype);		// リピータ形式
	sendcmd(command);

	usercmd[0] = '\0';
	usleep(WAITTIME * 30);

	/* 送・受信ループ */
	while (1) {

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
                if ((strlen(usercmd) > 4) && (strncmp(usercmd, chkusercmd, 8) != 0))
                {
                      /* 比較後、保存変数をクリア */
                        chkusercmd[0] = '\0';

                        /* 現在の返り値を保存 */
                        strncpy(chkusercmd, usercmd, 8);

			/* コマンドをスイッチに振り分ける */
			syscmdswitch();
		}

		/*
		 * 送信処理
		 */

		/* CPU 温度の表示 */
		disptemp();

		/* ログステータスの読み取り */
		dispstatus_ref();

	}

	return;
}
