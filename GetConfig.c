#include "Nextion.h"

/************************************************************
 * 環境設定ファイルnextion.ini と dstarrepeater の内容を読む
 ************************************************************/

void getconfig(void)
{
	FILE	*fp;
	char	*ret;
	char	*cmdline;
	char	line[64]	= {'\0'};
	char	ifname[16]	= {'\0'};
	char	ifaddr[16]	= {'\0'};
	char	tmpstr[10]	= {'\0'};
	int	i = 0;

	/*
	 * 設定ファイルをオープンし構造体に記録する
	 */

	/* nextion.iniをオープンする */
        if ((fp = fopen(INIFILE, "r")) == NULL)
        {
                printf("File open error!\n");
                return;
        }

        while ((fgets(line, sizeof(line), fp)) != NULL)
        {
                if ((ret = strstr(line, "STATION"))     != NULL) sscanf(line, "STATION=%[^\n]",     nx.station);
                if ((ret = strstr(line, "DEFAULT_RPT")) != NULL) sscanf(line, "DEFAULT_RPT=%[^\n]", nx.default_rpt);
                if ((ret = strstr(line, "SLEEPTIME"))   != NULL) sscanf(line, "SLEEPTIME=%d",       nx.microsec);
                if ((ret = strstr(line, "DEBUG"))       != NULL) sscanf(line, "DEBUG=%d",           nx.debug);
        }
        fclose(fp);


        /* dstarrepeater をオープンする */
        if ((fp = fopen(INIFILE, "r")) == NULL)
        {
                printf("File open error!\n");
                return;
        }

	if ((fp = fopen(CONFFILE, "r")) != NULL)
	{
		/* テーブルを読み込み変数に格納する */
		while ((fgets(line, sizeof(line), fp)) != NULL)
		{
			if ((ret = strstr(line, "callsign"))     != NULL) sscanf(line, "callsign=%[^\n]",     ds.station);
			if ((ret = strstr(line, "localAddress")) != NULL) sscanf(line, "localAddress=%[^\n]", ds.ipaddress);
			if ((ret = strstr(line, "localPort"))    != NULL) sscanf(line, "localPort=%[^\n]",    ds.localport);
			if ((ret = strstr(line, "modemType"))    != NULL) sscanf(line, "modemType=%[^\n]",    ds.modemtype);
		}

		/* ファイルクローズ */
		fclose(fp);

		/* ステーション名にバンド名を追加設定 */
		if (strcmp(&ds.station[7], "B") == 0) strcpy(tmpstr, " (70cm)");
		if (strcmp(&ds.station[7], "C") == 0) strcpy(tmpstr, " (2m)");
		strcat(ds.station, tmpstr);
	}

	return;
}

