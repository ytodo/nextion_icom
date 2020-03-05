/************************************************************
	dmonitor のログファイルよりラストハード及び
	状況を取得し変数status に入れる
 ************************************************************/

#include "dmonitor.h"

int getstatus()
{
	FILE	*fp;
	char	*getstatus   = "tail -n3 /var/log/dmonitor.log";
	char	*tmpstr;
	char	ret[2]       = {'\0'};
	char	line[128]    = {'\0'};
	char	jitter_av[8] = {'\0'};
	char	jitter_mx[8] = {'\0'};
	char	jitter_mi[8] = {'\0'};

	/* コマンドの標準出力オープン */
	if ((fp = popen(getstatus, "r")) == NULL)
	{
		printf("File open error!\n");
		return (EXIT_FAILURE);
	}

	/* 過去のデータをクリアする  */
	memset(&status[0], '\0', sizeof(status));
	memset(&rptcall[0],'\0', sizeof(rptcall));

	/* 標準出力を配列に取得 */
	while ((fgets(line, sizeof(line), fp)) != NULL)
	{
		/* status に関する文字列があったら */
		if ((tmpstr = strstr(line, "from")) != NULL)
		{
			/* 日付時間とコールサインをログとして出力 */
			if ((strstr(line, "Connected") == NULL) && (strstr(line, "Last packet") == NULL))
			{
				memset(&status[0], '\0', sizeof(status));
				strncpy(status, line, 16);
				strncat(status, tmpstr - 9, 8);
				stat = 0;
			}

			/* どこに接続したかを取得 */
			if ((tmpstr = strstr(line, "Connected")) != NULL)
			{
				strncpy(rptcall, tmpstr + 13, 8);
			}

			/* Last packet wrong ステータスの場合、文字を黄色に */
			if ((stat == 1) && (debug == 1) && (strstr(line, "Last packet wrong") != NULL))
			{
				strcpy(status, "Last packet is wrong...");
				break;
			}
		}

		/* dmonitorの開始とバージョンを取得 */
		if ((tmpstr = strstr(line, "dmonitor start")) != NULL)
		{
			memset(&status[0], '\0', sizeof(status));
			strncpy(status, tmpstr, 21);
		}

		/* バッファの拡張のサイズを取得 */
		if ((tmpstr = strstr(line, "New FiFo buffer")) != NULL)
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, tmpstr + 9);
			status[strlen(status) - 1] = '\0';
		}

		/* 接続解除を取得 */
		if (strstr(line, "dmonitor end") != NULL)
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "Disconnected");
		}

		/* 無線機の接続状況 */
		if ((debug == 1) && (strstr(line, "init/re-init") != NULL))
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "RIG initializing is done.");
		}

		/* ドロップパケット比の表示 */
		if ((debug == 1) && ((tmpstr = strstr(line, "drop")) != NULL))
		{
			memset(&status[0], '\0', sizeof(status));
			strcpy(status, "Drop PKT ");
			strcat(status, tmpstr + 17);
			status[strlen(status) - 1] = '\0';
			stat = 1;
		}
	}
	pclose(fp);
	return (EXIT_SUCCESS);
}


/* sample
Nov 16 09:35:00 ham12 dmonitor[30968]: drop pakcet rate 0.00% (0/22)
Nov 16 09:35:00 ham12 dmonitor[30968]: jitter info. ave:20mSec. max:26mSec. min:17mSec.
Nov 16 09:35:20 ham12 dmonitor[30968]: dmonitor end
Nov 16 09:35:20 ham12 dmonitor[9408]: dmonitor start V01.27 (Compiled Nov 11 2019 12:50:00)
Nov 16 09:35:20 ham12 dmonitor[9408]: Connected to JP3YIY A (153.131.76.69:51000) from JE3HCZ D
Nov 16 09:35:21 ham12 dmonitor[9408]: RIG(ID-xxPlus) open
Nov 16 09:35:21 ham12 dmonitor[9408]: hole punch done.
Nov 16 09:35:21 ham12 dmonitor[9408]: RIG(ID-xxPlus) init/re-init done
*/

