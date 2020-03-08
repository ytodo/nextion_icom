//////////////////////////////////////////////////////////////////////////////////
//	ファイル名	GetLinkData.c
//			2020.03.07-
//	機能	サーバよりmulti_forward のインストールされたゲートウェイリストを
//		ダウンロードして配列を作成する。
//////////////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"
#define		RPTSTBL	"/tmp/repeater.json"

int 	getlinkdata(void)
{
	char	*ptrcall;	// リピータのコールサイン用ポインタ
	char	*ptraddr;	// アドレス用ポインタ
	char	*ptrport;	// ポート用ポインタ
	char	*ptrzone;	// ゾーンリピータ用ポインタ
	char	*ptrstat;
	char	line[512]	= {'\0'};
	int	i = 0;
	FILE	*fp;

	/* d-star.info からリピータリストを取得してテーブルを作成する */
	system("cd /tmp; rm repeater.json*; wget http://hole-punchd.d-star.info:30011/repeater.json; cd");

	/* テーブルをオープンする */
	if ((fp = fopen(RPTSTBL, "r")) == NULL)
	{
		printf("File open error!\n");
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

	fclose(fp);

	/* Test */
//	int j = 0;
//	for (j = 0; j < i; j++) {
//		printf("%3d | %s | %s | %s | %s\n", j + 1, linkdata[j].call, linkdata[j].addr, linkdata[j].port, linkdata[j].zone);
//	}

	/* 軒数を返す */
	return (i);
}
