#include "Nextion.h"

void	dispcapture(void)
{
	char    cmdline[64]     = {'\0'};
	char    *tmpptr;
	char	tmpstr[32]	= {'\0'};
	char	callsign[9]	= {'\0'};
	char	subcall[5]	= {'\0'};
	char	line[256]	= {'\0'};
	int	flag		= 0;

	FILE	*fp;


	while (1)
	{
		/* tcpdump を起動してストリーミングを監視する(ラストパケットで一旦消す） */
		sprintf(cmdline, "sudo tcpdump -i lo -Av udp port 20010 -c1");

		/* ircddbgateway.dumpをオープン */
		if ((fp = popen(cmdline, "r")) == NULL)
        	{
	                return;
        	}

	        line[0] = '\0';
        	fgets(line,  sizeof(line),  fp);
	        fgets(line,  sizeof(line),  fp);
        	fgets(line,  sizeof(line),  fp);

	        /* ソケットのクローズ   */
        	pclose(fp);

	        if ((tmpptr = strstr(line, "DSRP")) != NULL)
        	{
                	strcpy(line, tmpptr);

			// 0         1         2         3         4
			// 01234567890123456789012345678901234567890123456789
			// DSRP ......JL3ZBS PJL3ZBS GCQCQCQ  JE3HCZ  ID31b.

			if (flag == 0)
			{
				strncpy(callsign, &line[35], 8);
				strncpy(subcall, &line[43], 4);

	                        /* JST 時刻の算出 */
				jstimer = time(NULL);
				jstimeptr = localtime(&jstimer);

	                        /* LastheardとしてMAINページに表示 */
        	               	strftime(tmpstr, sizeof(tmpstr), "%H:%M ", jstimeptr);

				sprintf(command, "MAIN.t1.txt=\"%s %s/%s\"", tmpstr, callsign, subcall);
				sendcmd(command);

				flag = 1;
			}

	        	// 0         1         2
	        	// 012345678901234567890
			// DSRP!......&.'.lp.UUU
			// DSRP!..L.UUUU.z......

			if (flag == 1)
			{
				if (!strncmp(&line[18], "UUU", 3)) flag = 0;
			}
		}
	}
	return;
}
