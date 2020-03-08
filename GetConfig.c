//////////////////////////////////////////////////////////////////////////////
//	ファイル名	GetConfig.c
//			2020.03.07-
//	機能	環境設定ファイルnextion.ini と dstarrepeater の内容を読む
//////////////////////////////////////////////////////////////////////////////
#include "Nextion.h"

void getconfig(void)
{
	FILE	*fp;
	char	*ret;
	char	line[64]	= {'\0'};
	int	i = 0;

	/*
	 *	設定ファイルをオープンし構造体に記録する
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

	/* ファイルクローズ */
        fclose(fp);

        /* dstarrepeater をオープンする */
        if ((fp = fopen(CONFFILE, "r")) == NULL)
        {
                printf("File open error!\n");
                return;
        }

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
	if (strcmp(&ds.station[7], "B") == 0) strcat(ds.station, " (70cm)");
	if (strcmp(&ds.station[7], "C") == 0) strcat(ds.station, " (2m)");



	/*
	 *	取得した設定値をNextionにグローバル変数に保存
	 */

	sprintf(command, "IDLE.station.txt=\"%s\"", ds.station);
	sendcmd(command);
	sprintf(command, "IDLE.type.txt=\"%s\"", ds.modemtype);
	sendcmd(command);

	return;
}