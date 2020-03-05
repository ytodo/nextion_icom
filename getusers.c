#include "dmonitor.h"

int getusers()
{
	FILE    *fp;
	char    *users_tbl  = "/var/www/html/connected_table.html";
	char    line[64]    = {'\0'};
	char    tmpstr[32]  = {'\0'};
	char    *tmpptr;
	char    command[32] = {'\0'};
	char    statpre[32] = {'\0'};
	char    ret[16]     = {'\0'};
	int     i           = 0;
	int     j           = 0;

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
			return (EXIT_FAILURE);
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

		/* ステータス・ラストハードの表示 */
		getstatus();

		if ((strncmp(status, "", 1) != 0) && (strncmp(status, statpre, 24) != 0))
		{
			strcpy(statpre, status);

			/* 取得ステイタス=> STATUS1 */
			sendcmd("t9.txt=t8.txt");
			sprintf(command, "t8.txt=\"%s\"", status);
			sendcmd(command);

			/* statusをクリアする */
			status[0] = '\0';
		}

		/* 1秒に一回リフレッシュする */
		sleep(1);
	}

	return(EXIT_SUCCESS);
}

