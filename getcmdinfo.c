/***************************************************************
	RaspberryPiのIPアドレスをコマンドでチェックして
	その標準所津力から文字列を取得グローバル変数の
	ipaddress に入れ main.c に渡す
 ***************************************************************/

#include "dmonitor.h"

int getipaddr()
{
	FILE	*fp;
	char	*cmdline;
	char	ifname[16]	= {'\0'};
	char	ifaddr[32]	= {'\0'};

	/* コマンドの標準出力をオープン */
	cmdline = "for DEV in `find /sys/devices -name net | grep -v virtual`; do ls $DEV/; done";
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(ifname, sizeof(ifname), fp);
		ifname[strlen(ifname) - 1] = '\0';

		/* 標準出力クローズ */
		pclose(fp);
	}

	/* コマンドの標準出力オープン（IPアドレスの取得）*/
	cmdline = "hostname -I | cut -f1 -d' '";
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(ifaddr, sizeof(ifaddr), fp);
		ifaddr[strlen(ifaddr) - 1] = '\0';

		/* 標準出力クローズ */
		pclose(fp);
	}

	/* インターフェースとIPアドレスの代入 */
	sprintf(ipaddress, "%s : %s", ifname, ifaddr);

    return(0);
}
