/********************************************************
 * dstarrepeaterd-yyyy-mm-dd.log を読み込み、
 * 処理の結果情報を取得する
 ********************************************************/

#include	"Nextion.h"

int disploginfo(void)
{
	FILE	*fp;
	char	*tmpptr;
	char	cmdline[64]	= {'\0'};
	char	line[256]	= {'\0'};
	char	line2[256]	= {'\0'};
	char	command[32]	= {'\0'};
	char	fname[32]	= {'\0'};
	char	mycall[10]	= {'\0'};
	char	urcall[10]	= {'\0'};

	/*
	 * ログファイルからリフレクタへのリンク情報を抽出する
	 */

	/* 日付入りログファイル名の作成 */
	timer = time(NULL);
	timeptr = gmtime(&timer);
	strftime(fname, sizeof(fname), "dstarrepeaterd-%Y-%m-%d.log", timeptr);
	sprintf(dstarlogpath, "%s%s", LOGDIR, fname);

	/* コマンドの標準出力オープン */
	sprintf(cmdline, "tail -n2 %s", dstarlogpath);
	if ((fp = popen(cmdline, "r")) != NULL )
	{

		/* 標準出力2行分を配列２つに取得し連結 */
		fgets(line,  sizeof(line),  fp);
		fgets(line2, sizeof(line2), fp);
		line[strlen(line) - 1]  = '\0';
		line2[strlen(line2) -1] = '\0';
		strcat(line, line2);

		/*
		 * リフレクタへの接続情報の取得
		 */
		if ((tmpptr = strstr(line, "Linked")) != NULL)
		{

			/* 一巡して全く同じ内容ならパス */
			if (strncmp(tmpptr, chklink, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
        			chklink[0] = '\0';
				strncpy(chklink, tmpptr, 60);

				/* リンク先リフレクタを取得 */
				linkref[0] = '\0';
				strncpy(linkref, tmpptr + 10, 8);
				linkref[8] = '\0';

				/* 接続時のログを取得 */
				status2[0] = '\0';
				strncpy(status2, tmpptr, 20);
				status2[20] = '\0';

				/* Nextion グローバル変数ref に接続中のリフレクタを代入 */
				sprintf(command, "IDLE.ref.txt=\"%s\"", linkref);
				sendcmd(command);
				sprintf(command, "IDLE.status.txt=\"%s\"", linkref);
				sendcmd(command);
				sendcmd("IDLE.t1.txt=status.txt");

				/* ステータス２の表示 */
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);
				sendcmd("IDLE.t2.txt=status2.txt");
			}
		}


		/*
		 * リフレクタへのdisconnect を取得
		 */
		if ((tmpptr = strstr(line, "Not linked")) != NULL)
		{
			/* 一巡して全く同じ内容ならパス */
			if (strncmp(tmpptr, chklink2, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chklink2[0] = '\0';
				strncpy(chklink2, tmpptr, 60);

				/* リンク先リフレクタを取得 */
				linkref[0] = '\0';

				/* 接続時のログを取得 */
				status2[0] = '\0';
				strncpy(status2, tmpptr, 20);
				status2[20] = '\0';

				/* Nextion グローバル変数に代入 */
				sendcmd("IDLE.ref.txt=\"Not linked\"");
				sendcmd("IDLE.status.txt=\"NODE IDLE\"");
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);

				reflesh_idle();
			}
		}


		/*
		 * RF ヘッダーの取得
		 */
		if ((tmpptr = strstr(line, "Radio header")) != NULL && rf_flag == 0)
		{
			/* 取得ログとダブりチェック用保存ログとの比較（違っていれば）*/
			if (strncmp(line, chkstat2, 60) != 0)
			{
				sendcmd("page DSTAR");

				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat2[0] = '\0';
				strncpy(chkstat2, line, 60);

				/* ヘッダーログを取得 */
				status2[0] = '\0';

				/* JST 時刻の算出 */
				jstimer = time(NULL);
				jstimeptr = localtime(&jstimer);

				/* Radio header の場合RF を表示 */
				strftime(status2, sizeof(status2), "RF  %H:%M ", jstimeptr);

				/* ログよりコールサインMY, UR を取得 */
				strncpy(mycall, tmpptr + 27, 13);
				mycall[13] = '\0';
				strncpy(urcall, tmpptr + 48, 8);
				urcall[8] = '\0';
				strcat(status2, mycall);
				status2[23] = '\0';

				/* ステータス２の表示 */
				sprintf(command, "DSTAR.t0.txt=\"R %s\"", mycall);
				sendcmd(command);
				sprintf(command, "DSTAR.t1.txt=\"%s\"", urcall);
				sendcmd(command);
				sendcmd("DSTAR.t2.txt=IDLE.ref.txt");

				/* ステータス2 を保存 */
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);
			}
			rf_flag = 1;
		}


		/*
		 * ネットワーク ヘッダーの取得
		 */
		if ((tmpptr = strstr(line, "Network header")) != NULL && net_flag == 0 )
		{
			/* 取得ログとダブりチェック用保存ログとの比較（違っていれば）*/
			if (strncmp(line, chkstat3, 60) != 0)
			{
				/* TX Delay */
				sleep(TXDELAY);

				sendcmd("page DSTAR");
				net_flag = 1;

				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat3[0] = '\0';
				strncpy(chkstat3, line, 60);

				/* ヘッダーログを取得 */
				status2[0] = '\0';

				/* JST 時刻の算出 */
				jstimer = time(NULL);
				jstimeptr = localtime(&jstimer);

				/* Network header の場合Net を表示 */
				strftime(status2, sizeof(status2), "Net %H:%M ", jstimeptr);

				/* ログよりコールサインMY, UR を取得 */
				strncpy(mycall, tmpptr + 30, 13);
				mycall[13] = '\0';
				strncpy(urcall, tmpptr + 51, 8);
				urcall[8] = '\0';
				strcat(status2, mycall);
				status2[23] = '\0';

				/* ステータス２の表示 */
				sprintf(command, "DSTAR.t0.txt=\"N %s\"", mycall);
				sendcmd(command);
				sprintf(command, "DSTAR.t1.txt=\"%s\"", urcall);
				sendcmd(command);
				sendcmd("DSTAR.t2.txt=IDLE.ref.txt");

				/* ステータス2 を保存 */
				sprintf(command, "IDLE.status2.txt=\"%s\"", status2);
				sendcmd(command);
			}
			net_flag = 1;
		}


		/*
		 * RF ラストパケット
		 */
		if ((tmpptr = strstr(line, "AMBE for")) != NULL && rf_flag == 1)
		{
			if (strncmp(line, chkstat2, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat2[0] = '\0';
				strncpy(chkstat2, line, 60);

				/* TX Hang */
				sleep(TXHANG);

				/* IDLE 画面に戻る */
				reflesh_idle();
			}
			rf_flag = 0;
		}


		/*
		 * ネットワークラストパケット
		 */
		if ((tmpptr = strstr(line, "Stats for")) != NULL && net_flag == 1)
		{
			if (strncmp(line, chkstat3, 60) != 0)
			{
				/* 一旦ダブりチェック用変数をクリアして新たに代入 */
				chkstat3[0] = '\0';
				strncpy(chkstat3, line, 60);

				/* TX Hang */
				sleep(TXHANG);

				/* IDLE 画面に戻る */
				reflesh_idle();
			}
			net_flag = 0;
		}

		/* 標準出力クローズ */
		pclose(fp);

	}
	else
	{
		exit(EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
