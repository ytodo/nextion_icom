///////////////////////////////////////////////////////////////////////////////////////
//	ファイル名	Functions.c
//			2020.03.07-
//	機能	openport		ttyAMA0ポートを開ける（from ON7LDS source)
//		sendcmd			Nextionへのコマンド送信
//		recvdata		Nextionからのコマンド受信
//		reflesh_idle		Nextionページの再表示
//		dmonitor_restart	dmonitorのrestart用システムコマンド
//		dstarrepeater_restart	DStarRepeaterのrestart用システムコマンド
//		modem_stop		reboot/shutdown前にサービスを止める
//		基本的なファンクション・コマンドのツールボックス
///////////////////////////////////////////////////////////////////////////////////////
#include "Nextion.h"

int fd;

/*********************************************
 * シリアルポートのオープン
 *   This is from ON7LDS's NextionDriver
 *********************************************/
int openport(char *devicename, long baudrate)
{
	struct	termios newtio;

	fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);

	// ポートが開けない時戻る
	if (fd < 0) {
		printf("Port Open Error\n");
		return(EXIT_FAILURE);
	}

	tcgetattr(fd, &newtio);

	cfsetispeed(&newtio,baudrate);
	cfsetospeed(&newtio,baudrate);

	newtio.c_cflag &= ~PARENB;
	newtio.c_cflag &= ~CSTOPB;          // ストップビット   : 1bit
	newtio.c_cflag &= ~CSIZE;           // データビットサイズ
	newtio.c_cflag |=  CS8;             // データビット     : 8bits

	newtio.c_cflag &= ~CRTSCTS;
	newtio.c_cflag |= CREAD | CLOCAL;   // 受信有効｜ローカルライン（モデム制御無し）

	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;

	tcflush(fd, TCIOFLUSH);

	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
	{
		exit(EXIT_FAILURE);
	}

	ioctl(fd, TCSETS, &newtio);		//ポートの設定を有効にする

	return (fd);
}



/*********************************************
 * コマンドの送信
 *********************************************/
void sendcmd(char *cmd)
{
	/* コマンドの終了を示す文字列 */
	char	endofcmd[3] = {0xff, 0xff, 0xff};

	/* コマンド文字列と終了文字列を書き込む */
	if (strlen(cmd) > 0)
	{
		write(fd, cmd ,strlen(cmd));
		write(fd, endofcmd, 3);
	}
	return;
}


/*********************************************
 * コマンドの受信
 *********************************************/
void recvdata(char *touchcmd)
{
	int	len	= 0;
	int	i	= 0;
	int	j	= 0;
	char	buf[32]	= {'\0'};

	/* GPIO RxD のASCIIデータを長さ分読み込む */
	len = read(fd, buf, sizeof(buf));
	if (0 < len)
	{
		for (i = 0; i < len; i++)
		{
            		if (buf[i] >= 30 && buf[i] <= 122)
			{
    				sprintf(&touchcmd[j], "%c", buf[i]);
				j++;
			}
		}
		touchcmd[j] = '\0';
	}
	return;
}



/*********************************************
 * 日付と時刻の表示
 *********************************************/
void dispclock(void)
{
	char	tmpstr[20] = {'\0'};

	/* 日付･時刻表示 */
	jstimer = time(NULL);
	jstimeptr = localtime(&jstimer);
	strftime(tmpstr, sizeof(tmpstr), "%Y.%m.%d %H:%M:%S ", jstimeptr);
	tmpstr[20] = '\0';
	sprintf(command, "MAIN.t2.txt=\"%s\"", tmpstr);
	sendcmd(command);

	return;
}



/*********************************************
 * システムコマンドを選択する
 *********************************************/
void syscmdswitch(void)
{
        int 	flag 		= 0;
	int	i		= 0;
	char	nodecall[9]	= {'\0'};
	char	refcall[9]	= {'\0'};


        /* 共通 */
        if (strncmp(usercmd, "restart",  7) == 0) flag =  1;
        if (strncmp(usercmd, "reboot",   6) == 0) flag =  2;
        if (strncmp(usercmd, "shutdown", 8) == 0) flag =  3;

        /* MAINのみ */
        if (strncmp(usercmd, "dmonitor", 8) == 0) flag =  4;
        if (strncmp(usercmd, "dstarrpt", 8) == 0) flag =  5;

        /* dmonitorのみ */
        if (strncmp(usercmd, "update",   6) == 0) flag =  6;
        if (strncmp(usercmd, "UP",       2) == 0) flag =  7;
        if (strncmp(usercmd, "DWN",      3) == 0) flag =  8;
        if (strncmp(usercmd, "USERS",    5) == 0) flag =  9;
        if (strncmp(usercmd, "next",     4) == 0) flag = 10;
        if (strncmp(usercmd, "previous", 8) == 0) flag = 11;

        /* dmonitor, dstarrepeater共通 */
        if (strncmp(usercmd, "return",   6) == 0) flag = 12;

        /* リフレクタ専用コマンド*/
        if (((strncmp(usercmd, "REF", 3) == 0
           || strncmp(usercmd, "XLX", 3) == 0
           || strncmp(usercmd, "DCS", 3) == 0
           || strncmp(usercmd, "XRF", 3) == 0)) && st.mode == 2) flag = 13;

        switch (flag) {
        case 1:                                         // restart
                switch (st.mode) {
                case 0: // MAIN
                        sendcmd("dim=10");
                        system("sudo systemctl stop dstarrepeater.service");
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        system("sudo killall -q -s 9 sleep");
                        sendcmd("dim=dims");
                        sendcmd("page MAIN");
                        break;

                case 1: // dmonitor
                        sendcmd("dim=10");
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        dmonitor();
                        break;

                case 2: // dstarrepeater
                        sendcmd("dim=10");
                        system("sudo systemctl stop dstarrepeater.service");
                        system("sudo systemctl start dstarrepeater.service");
                        sendcmd("dim=dims");
                        sendcmd("page IDLE");
                        dstarrepeater();
                        break;

                default:
                        break;

                }
                break;

        case 2:                                         // reboot
                switch (st.mode) {
                case 0: // MAIN
                        sendcmd("dim=10");
                        system("sudo systemctl stop ircddbgateway.service");
                        system("sudo systemctl stop dstarrepeater.service");
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        system("sudo killall -q -s 9 sleep");
                        system("sudo shutdown -r now");
                        break;

                case 1: //dmonitor
                        sendcmd("dim=10");
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        system("sudo shutdown -r now");
                        break;

                case 2: // dstarrepeater
                        sendcmd("dim=10");
                        system("sudo shutdown -r now");
                        break;

                default:
                        break;

                }
                break;

        case 3:                                         // shutdown
                switch (st.mode) {
                case 0:
                        sendcmd("dim=10");
                        system("sudo systemctl stop ircddbgateway.service");
                        system("sudo systemctl stop dstarrepeater.service");
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        system("sudo killall -q -s 9 sleep");
                        system("sudo shutdown -h now");
                        break;

                case 1: // dmonitor
                        sendcmd("dim=10");
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        system("sudo shutdown -h now");
                        break;

                case 2: // dstarrepeater
                        sendcmd("dim=10");
                        system("sudo shutdown -h now");
                        break;

                default:
                        break;

                }
                break;

        case 4:                                         // dmonitor 起動
                st.mode = 1;
                dmonitor();
                break;

        case 5:                                         // dstarrepeater 起動
                st.mode = 2;
                system("sudo systemctl restart dstarrepeater.service");
		dstarrepeater();
                break;

        case 6:                                         // update
                system("sudo killall -q -s 2 dmonitor");
                system("sudo rm /var/run/dmonitor.pid");
                system("sudo apt clean && apt update && apt install dmonitor");

                sendcmd("dim=10");
                system("sudo systemctl restart nextion.service");
                break;

        case 7:                                         // バッファの増加
                if (strncmp(usercmd, "up", 2) == 0) break;
                strcpy(usercmd, "up");
                system("sudo killall -q -s SIGUSR1 dmonitor");
                break;

        case 8:                                         // バッファの減少
                if (strncmp(usercmd, "dwn", 3) == 0) break;
                strcpy(usercmd, "dwn");
                system("sudo killall -q -s SIGUSR2 dmonitor");
                break;

        case 9:                                         // Remote Usersパネルへ接続ユーザ表示
                sendcmd("page USERS");
                sprintf(command, "USERS.b0.txt=\"LINKED USERS on %s\"", rptcallpre);
                sendcmd(command);
                getusers();
                strcpy(usercmd, "Return");
                break;

        case 10:                                        // リピータリスト次ページ
                next_page();
                break;

        case 11:                                        // リピータリスト前ページ
                previous_page();
                break;

        case 12:                                        // return
                switch (st.mode) {
                case 1: // dmonitor
                        system("sudo killall -q -s 2 dmonitor");
                        system("sudo rm /var/run/dmonitor.pid");
                        system("sudo systemctl restart nextion.service");
                        st.mode = 0;
                        sendcmd("page MAIN");
                        break;

                case 2: // dstarrepeater
                        system("sudo systemctl stop dstarrepeater.service");
                        system("sudo systemctl restart nextion.service");
                        sendcmd("page MAIN");
                        st.mode = 0;
                        break;

                default:
                        break;

                }
                break;

        case 13:                                        // リフレクタコマンド

		/* XLXリフレクタはDCSプロトコルに変換する */
		if (strncmp(usercmd, "XLX", 3) == 0)
		{
			strcpy(refcall, "DCS");
			strncat(&refcall[3], &usercmd[3], 3);
			strcat(refcall, "_");
			strncat(refcall, &usercmd[6], 1);
		}
		else
		{
			strncpy(refcall, usercmd, 6);
			strcat(refcall, "_");
			strncat(refcall, &usercmd[6], 1);
		}

		/* ノードコールサインの整形 */
		strncpy(nodecall, ds.station, 8);
		for (i = 0; i < 8; i++)
		{
			if (strncmp(&nodecall[i], " ", 1) == 0)
			{
				strncpy(&nodecall[i], "_", 1);
			}
		}


		sprintf(command, "remotecontrold %s link never %s", nodecall, refcall);

printf("%s\n", command);

		system(command);
                sendcmd("page IDLE");

                break;


        default:
                break;

        }
        return;
}
