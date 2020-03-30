
//////////////////////////////////////////////////////////////////////////////////
//	ファイル名	GetLinkData.c
//			2020.03.07-
//
//	機能	サーバよりmulti_forward のインストールされたゲートウェイリストを
//		ダウンロードして配列を作成する。
//
//		getlinkdata	このファイルのメイン機能　元となる配列作成
//		previous_page	配列を元にページを戻す
//
//////////////////////////////////////////////////////////////////////////////////

#include	"Nextion.h"
#define		RPTSTBL	"/tmp/repeater.json"

/*****************************************************************
 *	サーバよりmulti_forward のインストールされた
 *	ゲートウェイリストをダウンロードして配列を作成する。
 *****************************************************************/
int 	getlinkdata(void)
{
	FILE	*fp;
	char	*ptrcall;	// リピータのコールサイン用ポインタ
	char	*ptraddr;	// アドレス用ポインタ
	char	*ptrport;	// ポート用ポインタ
	char	*ptrzone;	// ゾーンリピータ用ポインタ
	char	*ptrstat;
	char	line[512]	= {'\0'};
	int	i = 0;

	/* d-star.info からリピータリストを取得してテーブルを作成する */
	system("cd /tmp; rm repeater.json*; wget http://hole-punchd.d-star.info:30011/repeater.json; cd");

	/* テーブルをオープンする */
	if ((fp = fopen(RPTSTBL, "r")) == NULL)
	{
		printf("Repeater List file open error!\n");
		return (EXIT_FAILURE);
	}

	/* File sample
		{"callsign":"JL3ZBS A","ip_address":"xx.xx.xx.xx","port":51000,"status":"off", "area":"3","ur_call":"8SPCs ",
		"my_call":"8SPCs","rpt1_call":"8SPCs ","rpt2_call":"8SPCs","zr_call":"JL3ZBS G"},
	*/

	/* テーブルを読み込み構造体に格納する */
	while ((fgets(line, sizeof(line), fp)) != NULL)
	{
		/* Find out Callsign, IP Address and Port number */
		if ((ptrcall = strstr(line, "callsign")) != NULL )
		{
			ptraddr = strstr(line, "ip_address");
			ptrport = strstr(line, "port");
			ptrzone = strstr(line, "zr_call");
			ptrstat = strstr(line, "status");

			/* コールサインの保存 */
			strncpy(linkdata[i].call, ptrcall + 11, 8);

			/* 第八番目もじ（拡張子）が空白だったら「Ａ」とする */
			if (strncmp(&linkdata[i].call[7], " ", 1) == 0)
			{
				strncpy(&linkdata[i].call[7], "A", 1);
			}
			linkdata[i].call[8] = '\0';

			/* ＩＰアドレスの保存 */
			strncpy(linkdata[i].addr, ptraddr + 13, strlen(ptraddr) - strlen(ptrport) - 16);
			linkdata[i].addr[strlen(ptraddr) - strlen(ptrport) - 16] = '\0';

			/* ポート番号の保存 */
			strncpy(linkdata[i].port, (ptrport + 6), (strlen(ptrport) - strlen(ptrstat) - 8));
			linkdata[i].port[strlen(ptrport) - strlen(ptrstat) - 8] = '\0';

			/* ZR_CALLの取得 */
			strncpy(linkdata[i].zone, (ptrzone + 10), 8);
			linkdata[i].zone[8] = '\0';

			i++;
		}
	}

	/* ファイルのクローズ*/
	fclose(fp);

	/* Test */
//	int j = 0;
//	for (j = 0; j < i; j++) {
//		printf("%3d | %s | %15s | %5s | %s\n", j + 1, linkdata[j].call, linkdata[j].addr, linkdata[j].port, linkdata[j].zone);
//	}

	/* 軒数を返す */
	return (i);
}



/*****************************************************************
 *	表示ページを前ページへ戻す
 *****************************************************************/
void	previous_page(void)
{
	int 	i 		= 0;
	int	pages		= 0;
	int	lastpagenum	= 0;

	pages 		= st.num / 21;
	lastpagenum	= st.num % 21;

	/* 現在の表示が最初のページの時 */
	if (st.selected_page == 0)
	{
		for (i = 0; i < 21; i++)
		{
			if (i < lastpagenum )		// 実在するデータ分を代入
			{
				sprintf(command, "RPTLIST.t%d.txt=\"%s\"",  i + 100, linkdata[i + 21 * pages].call);
				sendcmd(command);
				sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i, 	     linkdata[i + 21 * pages].call);
				sendcmd(command);
			}
			else				// データが21に満たない場合残りをクリアする
			{
				sprintf(command, "RPTLIST.t%d.txt=\"        \"",  i + 100);
				sendcmd(command);
				sprintf(command, "RPTLIST.va%d.txt=\"\"", i);
				sendcmd(command);
			}
		}
		st.selected_page = pages + 1;       	// 表示した最後のページを保存
	}

	/* 現在のページが最初のページではない場合 */
	else
	{
		st.selected_page -= 1;

		for (i = 0; i < 21; i++)
		{
			sprintf(command, "RPTLIST.t%d.txt=\"%s\"",  i + 100, linkdata[i + 21 * st.selected_page].call);
			sendcmd(command);
			sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i      , linkdata[i + 21 * st.selected_page].call);
			sendcmd(command);
		}
	}

	return;
}


/*****************************************************************
 *	表示ページを次ページへ送る
 *****************************************************************/
void	next_page(void)
{
	int 	i 		= 0;
	int	pages		= 0;
	int	lastpagenum	= 0;

	pages 		= st.num / 21;
	lastpagenum	= st.num % 21;

	/* 現在の表示が最後のページより一つ前の時 */
	if (st.selected_page == pages)
	{
		for (i = 0; i < 21; i++)
		{
			if (i < lastpagenum )		// 実在するデータ分を代入
                        {
				sprintf(command, "RPTLIST.t%d.txt=\"%s\"",  i + 100, linkdata[i + 21 * pages].call);
				sendcmd(command);
				sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i,       linkdata[i + 21 * pages].call);
				sendcmd(command);
			}
			else				// データが21に満たない場合残りをクリアする
			{
				sprintf(command, "RPTLIST.t%d.txt=\"        \"",  i + 100);
				sendcmd(command);
				sprintf(command, "RPTLIST.va%d.txt=\"\"", i);
				sendcmd(command);
			}
		}
		st.selected_page = pages + 1;
	}

	/* 現在のページが最後のページより一つ前ではない場合 */
	else
	{
		if (st.selected_page == pages + 1)
		{
			st.selected_page = 0;
		}
		else
		{
			st.selected_page += 1;
		}

		for (i = 0; i < 21; i++)
		{
			sprintf(command, "RPTLIST.t%d.txt=\"%s\"",  i + 100, linkdata[i + 21 * st.selected_page].call);
			sendcmd(command);
			sprintf(command, "RPTLIST.va%d.txt=\"%s\"", i      , linkdata[i + 21 * st.selected_page].call);
			sendcmd(command);
		}
	}

	return;
}
