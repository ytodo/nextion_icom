#include "Nextion.h"

repeater_t	linkdata[LISTSIZE]      = {'\0'};
char		stat_dmon[32]		= {'\0'};
char		stat_dstar1[32]		= {'\0'};
char		stat_dstar2[32]		= {'\0'};
char		linkref[8]		= {'\0'};
char		usercmd[32]		= {'\0'};
char		station_dmon[8]		= {'\0'};
char		station_dstar[8]	= {'\0'};
char		modemtype[16]		= {'\0'};
char		cputemp[8]		= {'\0'};
char		dstarlogpath[32]	= {'\0'};
char		chklink[16]		= {'\0'};
char		chklink2[16]		= {'\0'};
char		chkstat[256]		= {'\0'};
char		chkstat2[256]		= {'\0'};
char		chkstat3[256]		= {'\0'};
int		cnt_temp		= SLEEPCOUNT;
int		net_flag		= 0;
int		rf_flag			= 0;
