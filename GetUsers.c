///////////////////////////////////////////////////////////////////////////
//	ファイル名	GetUsers.c
//			2020.03.07-
//	機能	multi_forwardに接続しているユーザの表示
///////////////////////////////////////////////////////////////////////////
#include "Nextion.h"

void	getusers(void)
{
	FILE    *fp;
	char    *tmpptr;
	char    *users_tbl  = "/var/www/html/connected_table.html";
	char    line[64]    = {'\0'};
	char    tmpstr[32]  = {'\0'};
	char    command[32] = {'\0'};
	char    statpre[32] = {'\0'};
	char    ret[16]     = {'\0'};
	int     i           = 0;
	int     j           = 0;


	/* 画面を切り替える */
	sendcmd("page USERS");
	sprintf(command, "USERS.b0.txt=\"LINKED USERS on %s\"", chkrptcall);
	sendcmd(command);

	/* [Return]ボタンが押されるまで繰り返す */
	while (1)
	{
		/* [Return]の検出 */
		recvdata(ret);
		if ((strncmp(ret, "Return", 6)) == 0) break;

		/* コマンドの標準出力オープン */
		if ((fp = fopen(users_tbl, "r")) == NULL)
		{
			printf("File open error!\n");
			return;
		}

		/* ファイルを一行ずつ読み最終行まで繰り返す  */
		i = 0;
		while ((fgets(line, sizeof(line), fp)) != NULL)
		{
			/* <td> を見つけ内容を取得し、リストに表示 */
			if ((tmpptr = strstr(line, "<td>")) != NULL)
			{
				strncpy(tmpstr, tmpptr + 4, 8);
				sprintf(command, "USERS.t%d.txt=\"%s\"", i, tmpstr);
				sendcmd(command);
				i++;
				if (i >= 8) break;
        		}
        	}

		/* ファイルクローズ */
		fclose(fp);

        	/* リスト8件に満たない場合、残りをクリアする */
		if (i < 8 )
		{
			for (j = i; j < 8; j++)
			{
				sprintf(command, "USERS.t%d.txt=\" \"", j);
				sendcmd(command);
			}
		}

		/* ステータス・ラストハードの読み取り */
		dispstatus_dmon();

		/* 1秒に一回リフレッシュする */
		sleep(1);
	}
	return;
}

