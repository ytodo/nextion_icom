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
    				sprintf(&touchcmd[i], "%c", buf[i]);
				j++;
			}
		}
		touchcmd[i] = '\0';
	}
}


/*********************************************
 * IDLE 画面に復帰時データを再表示
 *********************************************/
void reflesh_idle()
{
	sendcmd("dim=dims");
	sendcmd("page IDLE");
	sendcmd("t0.txt=station.txt");
	sendcmd("t1.txt=status.txt");
	sendcmd("t2.txt=status2.txt");
	sendcmd("t3.txt=ipaddr.txt");
	sendcmd("t30.txt=type.txt");
}
