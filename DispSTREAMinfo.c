/* sample
07:41:46.929695 IP (tos 0x0, ttl 64, id 17506, offset 0, flags [DF], proto UDP (17), length 77)
    10.0.0.14.20013 > 10.0.0.4.20010: [bad udp cksum 0x145c -> 0x6115!] UDP, length 49
        0x0000:  4500 004d 4462 4000 4011 e22c 0a00 000e  E..MDb@.@..,....
        0x0010:  0a00 0004 4e2d 4e2a 0039 145c 4453 5250  ....N-N*.9.\DSRP
        0x0020:  2027 0800 0000 004a 4c33 5a42 5320 474a  .'.....JL3ZBS.GJ
        0x0030:  4c33 5a42 5320 4344 4353 3034 3742 4c4a  L3ZBS.CDCS047BLJ
        0x0040:  4533 4843 5a20 2049 4438 3002 3e         E3HCZ..ID80.>
07:41:46.929896 IP (tos 0x0, ttl 64, id 17507, offset 0, flags [DF], proto UDP (17), length 77)
    10.0.0.14.20013 > 10.0.0.4.20010: [bad udp cksum 0x145c -> 0x6115!] UDP, length 49
        0x0000:  4500 004d 4463 4000 4011 e22b 0a00 000e  E..MDc@.@..+....
        0x0010:  0a00 0004 4e2d 4e2a 0039 145c 4453 5250  ....N-N*.9.\DSRP
        0x0020:  2027 0800 0000 004a 4c33 5a42 5320 474a  .'.....JL3ZBS.GJ
        0x0030:  4c33 5a42 5320 4344 4353 3034 3742 4c4a  L3ZBS.CDCS047BLJ
        0x0040:  4533 4843 5a20 2049 4438 3002 3e         E3HCZ..ID80.>
07:41:46.930052 IP (tos 0x0, ttl 64, id 17508, offset 0, flags [DF], proto UDP (17), length 49)
    10.0.0.14.20013 > 10.0.0.4.20010: [bad udp cksum 0x1440 -> 0x9234!] UDP, length 21
        0x0000:  4500 0031 4464 4000 4011 e246 0a00 000e  E..1Dd@.@..F....
        0x0010:  0a00 0004 4e2d 4e2a 001d 1440 4453 5250  ....N-N*...@DSRP
        0x0020:  2127 0800 009e 8d32 8826 1a3f 61e8 552d  !'.....2.&.?a.U-
        0x0030:  16                                       .
07:41:46.930173 IP (tos 0x0, ttl 64, id 17509, offset 0, flags [DF], proto UDP (17), length 49)
    10.0.0.14.20013 > 10.0.0.4.20010: [bad udp cksum 0x1440 -> 0xf149!] UDP, length 21
        0x0000:  4500 0031 4465 4000 4011 e245 0a00 000e  E..1De@.@..E....
        0x0010:  0a00 0004 4e2d 4e2a 001d 1440 4453 5250  ....N-N*...@DSRP
        0x0020:  2127 0801 009e 8d32 8826 1a3f 61e8 3016  !'.....2.&.?a.0.
        0x0030:  dc                                       .



07:41:47.134074 IP (tos 0x0, ttl 64, id 17524, offset 0, flags [DF], proto UDP (17), length 49)
    10.0.0.14.20013 > 10.0.0.4.20010: [bad udp cksum 0x1440 -> 0x5301!] UDP, length 21
        0x0000:  4500 0031 4474 4000 4011 e236 0a00 000e  E..1Dt@.@..6....
        0x0010:  0a00 0004 4e2d 4e2a 001d 1440 4453 5250  ....N-N*...@DSRP
        0x0020:  2127 080b 009e 8d32 8826 1a3f 61e8 5555  !'.....2.&.?a.UU
        0x0030:  55                                       U
07:41:47.134246 IP (tos 0x0, ttl 64, id 17525, offset 0, flags [DF], proto UDP (17), length 52)
    10.0.0.14.20013 > 10.0.0.4.20010: [bad udp cksum 0x1443 -> 0x89ea!] UDP, length 24
        0x0000:  4500 0034 4475 4000 4011 e232 0a00 000e  E..4Du@.@..2....
        0x0010:  0a00 0004 4e2d 4e2a 0020 1443 4453 5250  ....N-N*...CDSRP
        0x0020:  2127 084c 009e 8d32 8826 1a3f 61e8 5555  !'.L...2.&.?a.UU
        0x0030:  5555 c87a                                UU.z

*/

/**************************************************
 *  Dashboard for D-STAR Repeater Gateway         *
 *      lastheard version 1.0                     *
 *      2018.12.17 - 2019.01.21                   *
 *                                                *
 *  Xchange が搭載されているリピータの            *
 *  ラストハードを表示する                        *
 *                                                *
 *                    Created by Yosh Todo/JE3HCZ *
 **************************************************/

#include "recv.h"

/**************************************************
 * ラストハード･ログ生成メインモジュール          *
 **************************************************/
void dispstreaminfo(void)
{
	/* IPv4 UDP のソケットを作成*/
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return (-1);
    }

    /* 待ち受けるIP とポート番号を設定 */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(20011);           /* 受信ポートxchage より */
    addr.sin_addr.s_addr = INADDR_ANY;

    /* バインドする */
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return (-1);
    }

    /* UDP パケットの捕捉と仕分け、再構成 */
    while (1) {


        /* 受信バッファの初期化 */
        memset(recvbuf, 0, sizeof(recvbuf));

        /* 受信パケット用ソケット */
        if (recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from_addr, &sin_size) < 0) {
            perror("recvfrom");
            return (-1);
        }

        /**************************************************
         * 音声系データの管理データL （L の後ろに続く     *
         * データ長）から判断                             *
         * 0x30 （４８バイト）, 0x13 （19バイト）、       *
         * 0x16 （22バイト）等                            *
         **************************************************/
        switch (recvbuf[9]) {
		case 0x30:

            /*
             * 同一通話ID （PTTON からPTTOFF まで）で一度ログを出す
             * 書き出し直後にflag を３にセットして判断
             */
            if (m_flag != 3) {

                /* ヘッダーをログ分として構成する */
                header();
            }

            break;

		/* ヘッダー部に続く19バイトのフレーム */
		case 0x13:

            /* 各フレーム末尾３バイト（24bits ）のデータセグメントを処理する */
            for (i = 0; i < 3; i++) {
                sdata[i] = recvbuf[26 + i];
            }

            /* Last Flameか？ */
            if (memcmp(sdata, last, 3) == 0) {
                linecount();
                m_counter = 0;
                m_flag = 0;
                m_sync = 0;

                break;
            }

            /* sync packetか? */
            if (memcmp(sdata, sync, 3) == 0) {
                memset(sdata, 0, sizeof(sdata));
                m_sync = 1;

                break;
            }

            /*
             * 同一通話ID （PTTON からPTTOFF まで）で一度ログを出す
             * 書き出し直後にflag を３にセットして判断
             */
            if ((m_flag < 2) && (m_sync == 1)) {

                /* Slow Data 表示サブルーティン */
                slowdata();
            }

            /* slowdata の構成が終了した時点でm_flag に２がセットされる */
            if (m_flag == 2) {

                /* 一行ログを出力する */
                write();

                /* その時点でm_flag を３にセットしてLast Flame を待つ */
                m_flag = 3;
                m_sync = 0;
            }

			break;

		/* ラストフレーム */
		case 0x16:

			/* Last flame */
			m_counter = 0;
            m_flag = 0;
            m_sync = 0;
   			break;

		default:

            break;
        }
    }

    /* ソケットのクローズ   */
    close(sock);
}


/**************************************************
 * 関数の定義                                     *
 **************************************************/

/**************************************************
 * インターネット側通信ヘッダ（status)の表示      *
 *                                                *
 * 音声系データの管理データL （L の後ろに続く     *
 * データ長）から判断   0x30 （４８バイト）。     *
 * インターネット側はスクランブルされない。       *
 **************************************************/
int header(void)
{

    /* access time */
    timer = time(NULL);
    timeptr = localtime(&timer);
    strftime(tmstr, N, "%Y/%m/%d %H:%M:%S", timeptr);
    sprintf(logline, "%s", tmstr);

    /* My   */
    strcat(logline, " D-STAR my: ");
    line[0] = '\0';
    for (j = 0; j < 8; ++j) {
        sprintf(c, "%c", recvbuf[44 + j]);
        strcat(line, c);
    }
    strcat(logline, line);
    strcat(logline, "/");
    line[0] = '\0';
    for (j = 0; j < 4; ++j) {
        sprintf(c, "%c", recvbuf[52 + j]);
        if (c == '\0' ) strcpy(c, " ");
        strcat(line, c);
    }
    strcat(logline, line);
    strcat(logline, " |");

    /* rpt1 */
    strcat(logline, " rpt1: ");
    line[0] = '\0';
    for (j = 0; j < 8; ++j) {
        sprintf(c, "%c", recvbuf[28 + j]);
        strcat(line, c);
    }
    strcat(logline, line);

    /* ur   */
    strcat(logline, " | ur: ");
    line[0] = '\0';
    for (j = 0; j < 8; ++j) {
        sprintf(c, "%c", recvbuf[36 + j]);
        strcat(line, c);
    }
    strcat(logline, line);
    strcat(logline, " |");

    return(0);
}


/**************************************************
 * データセグメント（Slow Data ）情報の構成と表示 *
 *                                                *
 * データセグメント３バイト（24bits）を取り出して *
 * sync フレームとlast フレームを判別。           *
 * さらにそれ以外の２フレームを再構成し６バイト   *
 * （48bits ）のデータを作成するときスクランブルを*
 * 解除する。                                     *
 * そのデータの最初の１バイト（ミニヘッダ8bits　）*
 * に規定されている内容によってメッセージを再構成 *
 * する。   メッセージ用ミニヘッダ（0x40～0x43)   *
 **************************************************/
int slowdata(void)
{
	/* データ・セグメントのスクランブルを解く */
    for (i = 0; i < 3; i++) {
		sdata[i] = (sdata[i] ^ scbl[i]);
	}

	/* ミニヘッダ 0x40 ～0x43 のメッセージを選別 */
	if ((sdata[0] >= 0x40) && (sdata[0] <= 0x43) && (m_flag == 0)) {

		/* メッセージ20 バイトの先頭にタイトル */
		if (sdata[0] == 0x40) strcat(logline, " Short MSG: ");

        /* ミニヘッダ以外の２バイト（16bits）を表示 */
		line[0] = '\0';
		sprintf(line, "%c%c", sdata[1], sdata[2]);
		strcat(logline, line);
		m_counter++;
		m_flag = 1;
		return;
	}

	/* ミニヘッダを含まないブロック３バイト（24bits ）を接続 */
	if (m_flag == 1) {
		line[0] = '\0';
		sprintf(line, "%c%c%c", sdata[0], sdata[1], sdata[2]);
		strcat(logline, line);
		m_counter++;
		m_flag = 0;
	}

	/* メッセージの末尾又は合成パケット四つ分(カウント８）でクリア */
	if (m_counter > 7) {

        /* ログ一行の書き出しフラッグを立てる */
		m_flag = 2;
	}

	return;
}


/**************************************************
 * ログの一行分をログファイルに出力               *
 *                                                *
 * LOGFILE で示すファイルパスに追加で書き込む     *
 **************************************************/
int write(void)
{
	/* ログファイルを追加モードでオープンする */
	if ((fp = fopen(LOGFILE, "a")) == NULL) {
		printf("File open error!\n");
		return (-1);
	}

	/* ログ一行を書き込む */
	fprintf(fp, "%s\n", logline);
	printf("%s\n", logline);

	/* ファイルを閉じる */
	fclose(fp);

	return (0);
}



/**************************************************
 * ログが規定行数を超えたら古いものから削除       *
 *                                                *
 * LOGFILE で示すファイルパスから配列に読み込み   *
 * 規定を越えた行数分を古いエントリーから省いて   *
 * 再度上書きする。                               *
 **************************************************/
int linecount()
{

    int     count       = 0;
    int     i           = 0;
    char    buf[N]      = {'\0'};
    char    logs[LOGMAX + 1000][N]   = {'\0'};


	/* ログファイルを読み取りでオープン */
	if ((fp = fopen(LOGFILE, "r")) == NULL) {
		printf("Log file open error! (Read)\n");
		return (-1);
	}

	/* 各行を2次元配列に入れる */
	while ((fgets(buf, sizeof(buf), fp)) != NULL) {
		sprintf(&logs[count][0], "%s", buf);
		count++;
	}

	/* ファイルを閉じる */
	fclose(fp);

	/* 規定行数を越えたら古い方から指定行数省いて書き込み */
	if (count > LOGMAX) {

		/* ログファイルを上書きでオープン */
		if ((fp = fopen(LOGFILE, "w")) == NULL) {
			printf("Log file open error! (Write)\n");
			return (-1);
		}

		/* 配列入れたデータを削除件数省いて書き出す */
		for (i = 0; i < (count - LOGDEL); i++) {
			fprintf(fp, "%s", &logs[i + LOGDEL][0]);
		}

    	/* ファイルを閉じる */
	    fclose(fp);
	}


	return (0);
}

