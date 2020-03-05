#include "Nextion.h"
#define SLEEPTIME	300000

int dispstreaminfo(void)
{
	FILE	*fp;
	char	*ret;
	char	cmdline[32]	= {'\0'};
	char	line[256]	= {'\0'};

	/*
	 * 設定ファイルをオープンする
	 */

    /* CPU 温度を取得するシステムコマンド */
//    sprintf(cmdline, "tcpdump -i %s -A udp port %s -c1", ifname, localport);

//    if ((fp = popen(cmdline, "r")) != NULL ) {

        /* 接続リフレクタに関する標準出力を配列に取得 */
//        fgets(line, sizeof(line), fp);

//printf("%s\n", line);

//        usleep(SLEEPTIME);

        /* 標準出力クローズ */
//        pclose(fp);

//    } else {
//        exit(EXIT_FAILURE);
//    }

	return (EXIT_SUCCESS);
}


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
