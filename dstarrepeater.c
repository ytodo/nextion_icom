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
	char	chkusercmd[32]	= {'\0'};
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

	/* GPIO シリアルポートのクローズ*/
	close(fd);
	return;
}

