#include "Nextion.h"

/*********************************************
 * 環境設定ファイルnextion.ini の内容を読む
 *********************************************/

int getconfig(void)
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
                return (EXIT_FAILURE);
        }

        while ((fgets(line, sizeof(line), fp)) != NULL)
        {
                if ((ret = strstr(line, "STATION"))     != NULL) sscanf(line, "STATION=%[^\n]",     nextion_ini.station);
                if ((ret = strstr(line, "DEFAULT_RPT")) != NULL) sscanf(line, "DEFAULT_RPT=%[^\n]", nextion_ini.default_rpt);
                if ((ret = strstr(line, "SLEEPTIME"))   != NULL) sscanf(line, "SLEEPTIME=%d",       nextion_ini.microsec);
                if ((ret = strstr(line, "DEBUG"))       != NULL) sscanf(line, "DEBUG=%d",           nextion_ini.debug);
        }
        fclose(fp);


        /* dstarrepeater をオープンする */
        if ((fp = fopen(INIFILE, "r")) == NULL)
        {
                printf("File open error!\n");
                return (EXIT_FAILURE);
        }

	if ((fp = fopen(CONFFILE, "r")) != NULL)
	{
		/* テーブルを読み込み変数に格納する */
		while ((fgets(line, sizeof(line), fp)) != NULL)
		{
			if ((ret = strstr(line, "callsign"))     != NULL) sscanf(line, "callsign=%[^\n]",     dstarrepeater.station);
			if ((ret = strstr(line, "localAddress")) != NULL) sscanf(line, "localAddress=%[^\n]", dstarrepeater.ipaddress);
			if ((ret = strstr(line, "localPort"))    != NULL) sscanf(line, "localPort=%[^\n]",    dstarrepeater.localport);
			if ((ret = strstr(line, "modemType"))    != NULL) sscanf(line, "modemType=%[^\n]",    dstarrepeater.modemtype);
		}

		/* ファイルクローズ */
		fclose(fp);

		/* ステーション名にバンド名を追加設定 */
		if (strcmp(&dstarrepeater.station[7], "B") == 0) strcpy(tmpstr, " (70cm)");
		if (strcmp(&dstarrepeater.station[7], "C") == 0) strcpy(tmpstr, " (2m)");
		strcat(dstarrepeater.station, tmpstr);
	}


	/*
	 * コマンドの標準出力オープン（ネットワークインターフェース名取得）
	 */
	cmdline = "for DEV in `find /sys/devices -name net | grep -v virtual`; do ls $DEV/; done";
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(ifname, sizeof(ifname), fp);
		ifname[strlen(ifname) - 1] = '\0';

		/* 標準出力クローズ */
		pclose(fp);
	}

	/*
	 * コマンドの標準出力オープン（IPアドレスの取得）
	 */
	cmdline = "hostname -I | cut -f1 -d' '";
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(ifaddr, sizeof(ifaddr), fp);

		/* 設定IP アドレスと実際のIP アドレスの比較 */
		if (strcmp(ifaddr, dstarrepeater.ipaddress) == FALES && strcmp(dstarrepeater.ipaddress, "127.0.0.1") == FALES)
		{
			strcpy(ipaddress, "Different IP set!");
	        }
		else
		{
			sprintf(dstarrepeater.ipaddress, "%s:%s", ifname, ifaddr);
		}

		/* 標準出力クローズ */
		pclose(fp);
	}

	return (EXIT_SUCCESS);
}

