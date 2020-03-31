////////////////////////////////////////////////////////////////////////////////////////
//	ファイル名	DispCMDinfo.c
//			2020.03.07-
//	機能	dispipaddr	本プログラム実行中の物理インターフェースIPを取得表示
//		disptemp	本プログラム実行中のRaspberryPiにおけるCPU温度を表示
//		システムコマンドを実行することによってデータを取得
////////////////////////////////////////////////////////////////////////////////////////
#include	"Nextion.h"


/********************************************************
 * Main から呼び出されIPアドレスをシステムコマンドで
 * 取得し、Nextionグローバル変数に代入する。
 ********************************************************/
void dispipaddr(void)
{
	FILE	*fp;
	char	ifname[16]	= {'\0'};
	char	ifaddr[32]	= {'\0'};

	/* コマンドの標準出力をオープン（ネットワーク・インターフェース名の取得） */
	strcpy(cmdline, "for DEV in `find /sys/devices -name net | grep -v virtual`; do ls $DEV/; done");
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(ifname, sizeof(ifname), fp);
		ifname[strlen(ifname) - 1] = '\0';
	}

	/* 標準出力クローズ */
	pclose(fp);

	/* コマンドの標準出力オープン（IPアドレスの取得）*/
	strcpy(cmdline, "hostname -I | cut -f1 -d' '");
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(ifaddr, sizeof(ifaddr), fp);
		ifaddr[strlen(ifaddr) - 1] = '\0';
	}

	/* 標準出力クローズ */
	pclose(fp);

	/* 設定IP アドレスと実際のIP アドレスの比較 */
	if (!strcmp(ifaddr, ds.ipaddress) && !strcmp(ds.ipaddress, "127.0.0.1"))
	{
		sprintf(command, "IDLE.ipaddr.txt=\"%s\"", "Different IP set!");
		sendcmd(command);
	}
	else
	{
		sprintf(command, "IDLE.ipaddr.txt=\"%s:%s\"", ifname, ifaddr);
		sendcmd(command);
	}

	return;
}


/********************************************************
 * Main から呼び出されCPU の温度をシステムコマンドで
 * 取得し、Nextion に表示する。
 ********************************************************/
void disptemp(void)
{
	FILE	*fp;
	char	*tmpptr;
	char	tmpstr[32]	= {'\0'};
	char	line[512]	= {'\0'};
	char	cputemp[16]	= {'\0'};

	/*
	 * CPU 温度の標準出力を取得
	 */

	/* CPU 温度を取得するシステムコマンド */
	strcpy(cmdline, "vcgencmd measure_temp");

	/* コマンドの返りをファイルとしてオープン */
	if ((fp = popen(cmdline, "r")) != NULL)
	{
		fgets(line, sizeof(line), fp);
		usleep(WAITTIME * 2);		// 完全に返り値を取得するまで待つ
       		line[strlen(line) - 1] = '\0';
		strcpy(cputemp, &line[5]);	// 先頭の不要文字をカットする

		/* CPU 温度の表示*/
		sprintf(command, "IDLE.temp.txt=\"%s\"", cputemp);
		sendcmd(command);
		sendcmd("IDLE.t20.txt=temp.txt");

		/* CPU 温度による表示色変更 */
		strcpy(tmpstr, cputemp);
		tmpstr[strlen(tmpstr) - 4] = '\0';
		int temp = atoi(tmpstr);
		if (temp < 45)
		{
			sendcmd("IDLE.t20.pco=2016");
			sendcmd("IDLE.t20.bco=25356");
		}
		if (temp >= 45 && temp < 50)
		{
			sendcmd("IDLE.t20.pco=65504");
			sendcmd("IDLE.t20.bco=25356");
		}
		if (temp >= 50 && temp < 55)
		{
			sendcmd("IDLE.t20.pco=64520");
			sendcmd("IDLE.t20.bco=25356");
		}
		if (temp >= 55)
		{
			sendcmd("IDLE.t20.pco=65504");
			sendcmd("IDLE.t20.bco=63488");
		}
	}

	/* 標準出力クローズ */
	pclose(fp);

	return;
}
