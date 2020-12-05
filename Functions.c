///////////////////////////////////////////////////////////////////////////////////////
//	ファイル名	Functions.c
//			2020.03.07-
//	機能	openport		ttyAMA0ポートを開ける（from ON7LDS source)
//		sendcmd			Nextionへのコマンド送信
//		recvdata		Nextionからのコマンド受信
//		dispclock		デジタル時計の表示
//		syscmdswitch		タッチパネルから受けるコマンドの分岐
//		基本的なファンクション・コマンドのツールボックス
///////////////////////////////////////////////////////////////////////////////////////
#include "Nextion.h"

int	fd;

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

	sprintf(command, "MAIN.t0.pco=%s", nx.clock_color);
	sendcmd(command);
	strftime(tmpstr, sizeof(tmpstr), "%H:%M:%S", jstimeptr);
	sprintf(command, "MAIN.t0.txt=\"%s\"", tmpstr);
	sendcmd(command);

	sprintf(command, "MAIN.t1.pco=%s", nx.clock_color);
	sendcmd(command);
	strftime(tmpstr, sizeof(tmpstr), "%Y.%m.%d", jstimeptr);
	sprintf(command, "MAIN.t1.txt=\"%s\"", tmpstr);
	sendcmd(command);

	return;
}


/*********************************************
 * システムコマンドを選択する
 *********************************************/
void syscmdswitch(void)
{
	int	flag	= 0;
	int	i	= 0;

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
	if (strncmp(usercmd, "ACTIVE",   6) == 0) flag = 13;

	/* dmonitor, dstarrepeater共通 */
	if (strncmp(usercmd, "return",   6) == 0) flag = 12;

	switch (flag)
	{
	case 1:						// restart
		switch (st.mode) {
		case 0: // MAIN (初期立上げ時の状態に戻る）
			sendcmd("dim=10");
			system("sudo systemctl restart ircddbgateway");
			system("sudo systemctl stop dstarrepeater");
			system("sudo killall -q -2 dmonitor");
			system("sudo rm -f /var/run/dmonitor.pid");
			system("sudo systemctl stop auto_repmon");
			system("sudo killall -q -9 sleep");
			system("sudo killall -q -9 repeater_scan");
			system("sudo /var/www/cgi-bin/repUpd");
			system("sudo systemctl restart nextion");
			break;

		case 1: // dmonitor
			sendcmd("dim=10");
			system("sudo killall -q -9 dmonitor");
			system("sudo rm -f /var/run/dmonitor.pid");
			system("sudo killall -q -9 sleep");
			system("sudo killall -q -9 repeater_scan");
			system("sudo /var/www/cgi-bin/repUpd");
			dmonitor();
			break;

		case 2: // dstarrepeater
			sendcmd("dim=10");
			system("sudo systemctl restart dstarrepeater");
			dstarrepeater();
			break;
		}
		break;

	case 2:						// reboot
		sendcmd("dim=10");
		sendcmd("page SPLASH");
		sendcmd("SPLASH.t3.txt=\"Stopping...\"");
		system("sudo systemctl stop ircddbgateway");
		system("sudo systemctl stop dstarrepeater");
		system("sudo killall -q -2 dmonitor");
		system("sudo rm -f /var/run/dmonitor.pid");
//		system("sudo killall -q -9 sleep");
		system("sudo shutdown -r now");
		break;

	case 3:						// shutdown
		sendcmd("dim=10");
		sendcmd("page SPLASH");
		sendcmd("SPLASH.t3.txt=\"Stopping...\"");
		system("sudo systemctl stop ircddbgateway");
		system("sudo systemctl stop dstarrepeater");
		system("sudo killall -q -2 dmonitor");
		system("sudo rm -f /var/run/dmonitor.pid");
//		system("sudo killall -q -9 sleep");
		system("sudo shutdown -h now");
		break;

	case 4:						// dmonitor 起動
		st.mode = 1;
		system("sudo systemctl stop dstarrepeater");
		dmonitor();
		break;

	case 5:						// dstarrepeater 起動
		st.mode = 2;
		system("sudo systemctl restart dstarrepeater");
		dstarrepeater();
		break;

	case 6:						// update
		sendcmd("SYSTEM.b5.pco=65504");
		sendcmd("SYSTEM.b5.txt=\"UPDATE\"");
		system("sudo killall -q -s 2 dmonitor");
		system("sudo rm /var/run/dmonitor.pid");
		system("sudo apt clean && sudo apt update && sudo apt install dmonitor -y");

		sendcmd("dim=10");
		system("sudo systemctl restart nextion");
		break;

	case 7:						// バッファの増加
		if (strncmp(usercmd, "up", 2) == 0) break;
		strcpy(usercmd, "up");
		system("sudo killall -q -s SIGUSR1 dmonitor");
		break;

	case 8:						// バッファの減少
		if (strncmp(usercmd, "dwn", 3) == 0) break;
		strcpy(usercmd, "dwn");
		system("sudo killall -q -s SIGUSR2 dmonitor");
		break;

	case 9:						// Remote Usersパネルへ接続ユーザ表示
		getusers();
		strcpy(usercmd, "Return");
		break;

	case 10:					// リピータリスト次ページ
		next_page();
		break;

	case 11:					// リピータリスト前ページ
		previous_page();
		break;

	case 12:					// return
		switch (st.mode) {
		case 0:	// MAIN
			/* 関連する全てのサービスを停止 */
			sendcmd("dim=10");
			system("sudo killall -q -2 dmonitor");
			system("sudo rm -f /var/run/dmonitor.pid");
			system("sudo systemctl stop rpt_conn");
			system("sudo systemctl stop auto_repmon");
			system("sudo systemctl stop dstarrepeater.service");
			st.mode = 0;
			sendcmd("page MAIN");
			break;

		case 1:	// dmonitor
			/* dmonitor関連のサービスを停止 */
			sendcmd("dim=10");
			system("sudo killall -q -2 dmonitor");
			system("sudo rm -f /var/run/dmonitor.pid");
			system("sudo systemctl stop rpt_conn");
			system("sudo systemctl stop auto_repmon");

			/* nextionを再起動してmodeをMAIN待機画面（０）とする */
			system("sudo systemctl restart nextion");
			st.mode = 0;
			sendcmd("page MAIN");
			break;

		case 2:	// dstarrepeater
			/* dstarrepeaterを停止 */
			sendcmd("dim=10");
			system("sudo systemctl stop dstarrepeater");

			/* nextionを再起動してmodeをMAIN待機画面（０）とする */
			system("sudo systemctl restart nextion");
			st.mode = 0;
			sendcmd("page MAIN");
			break;
		}
		break;

	case 13:					// ACTIVE REPEATERSパネルへの表示と移動
		getactive();
		break;

	default:
		break;

	}
	return;
}

