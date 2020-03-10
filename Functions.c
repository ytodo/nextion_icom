////////////////////////////////////////////////////////////////////////////////
//	ファイル名	Functions.c
//			2020.03.07-
//	機能	openport	ttyAMA0ポートを開ける（from ON7LDS source)
//		sendcmd		Nextionへのコマンド送信
//		recvdata	Nextionからのコマンド受信
//		reflesh_idle	Nextionページの再表示
//		基本的なファンクション・コマンドのツールボックス    
////////////////////////////////////////////////////////////////////////////////
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
            		if (buf[i] >= 30 && buf[i] <= 122 || buf[i] == 20)
			{
    				sprintf(&touchcmd[i], "%c", buf[i]);
				j++;
			}
		}
		touchcmd[i] = '\0';
	}
usleep(nx.microsec);
	return;
}


/*********************************************
 * IDLE 画面に復帰時データを再表示
 *********************************************/
void reflesh_pages(void)
{
	/* 明るさをバー指定値に設定 */
	sendcmd("dim=dims");

	/* MAIN reflesh */
	sendcmd("MAIN.t0.txt=MAIN.status_dmon.txt");
	sendcmd("MAIN.t1.txt=MAIN.status_ref.txt");

	/* IDLE reflesh */
	sendcmd("IDLE.t0.txt=IDLE.station.txt");
	sendcmd("IDLE.status.txt=IDLE.ref.txt");
	sendcmd("IDLE.t1.txt=IDLE.status.txt");
	sendcmd("IDLE.t2.txt=IDLE.status2.txt");
	sendcmd("IDLE.t3.txt=IDLE.ipaddr.txt");
	sendcmd("IDLE.t30.txt=IDLE.type.txt");
	dispipaddr();

	/* DMON reflesh */
	sendcmd("DMON.t0.txt=DMON.station.txt");
	sendcmd("DMON.t1.txt=DMON.link.txt");
	sendcmd("DMON.t2.txt=DMON.stat1.txt");
	sendcmd("DMON.t3.txt=DMON.stat2.txt");

	return;
}
