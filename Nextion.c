/////////////////////////////////////////////////////
//	ファイル名	Nextion.c
//			2020.03.07- 2025.04.19
//	機能	Nextion.hで宣言した変数をここで
//			まとめて定義する
/////////////////////////////////////////////////////
#include "Nextion.h"

repeater_t	linkdata[LISTSIZE]	= {'\0'};
nextion_ini_t	nx				= {'\0'};
dstarrepeater_t	ds				= {'\0'};
status_t		st				= {0, 0, 0};
char		command[128]		= {'\0'};
char		cmdline[128]		= {'\0'};
char		stat_dmon[32]		= {'\0'};
char		stat_dstar1[32]		= {'\0'};
char		stat_dstar2[32]		= {'\0'};
char		usercmd[32]			= {'\0'};
char		linkref[9]			= {'\0'};
char		station_dmon[9]		= {'\0'};
char		station_dstar[9]	= {'\0'};
char		modemtype[16]		= {'\0'};
char		chklink[16]			= {'\0'};
char		chklink2[16]		= {'\0'};
char		line[256]			= {'\0'};
char		chkline[256]		= {'\0'};
char		chkline2[256]		= {'\0'};
char		rptcall[9]			= {'\0'};
char		chkrptcall[9]		= {'\0'};
char		status[32]			= {'\0'};
char		rfcommand[32]		= {'\0'};
char		chkstat1[32]		= {'\0'};
char		chkstat2[32]		= {'\0'};
char		chkstat3[32]		= {'\0'};
