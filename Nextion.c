/////////////////////////////////////////////////////
//	ファイル名	Nextion.c
//			2020.03.07-
//	機能	Nextion.hで宣言した変数をここで
//		まとめて定義する
/////////////////////////////////////////////////////
#include "Nextion.h"

repeater_t	linkdata[LISTSIZE]      = {'\0'};
nextion_ini_t	nx			= {'\0'};
dstarrepeater_t	ds			= {'\0'};
status_t	st			= {0, 0};
char		command[32]		= {'\0'};
char		cmdline[128]		= {'\0'};
char		stat_dmon[32]		= {'\0'};
char		stat_dstar1[32]		= {'\0'};
char		stat_dstar2[32]		= {'\0'};
char		usercmd[32]		= {'\0'};
char		linkref[9]		= {'\0'};
char		station_dmon[9]		= {'\0'};
char		station_dstar[9]	= {'\0'};
char		modemtype[16]		= {'\0'};
char		chklink[16]		= {'\0'};
char		chklink2[16]		= {'\0'};
char		chkstat[256]		= {'\0'};
char		chkstat2[256]		= {'\0'};
char		chkstat3[256]		= {'\0'};
int		cnt_temp		= SLEEPCOUNT;
int		net_flag		= 0;
int		rf_flag			= 0;
char		rptcallpre[32]		= {'\0'};
char		statpre[32]		= {'\0'};
