/*
 *  Copyright (C) 2018-2020 by Yosh Todo JE3HCZ
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  本プログラムはD-STAR Network の一部の機能を実現するための物で、
 *  アマチュア無線の技術習得とその本来の用途以外では使用しないでください。
 *
 */

/*********************************************************************
 	D-STAR  Nextion display for ICOM Terminal/Access Mode
		Nextion.h 
		2020.03.05 - 
 
	Multi_Forwardが搭載されているリピータリストを
	取得して「接続可能リピータ」としてdmonitor用
	ディスプレイに表示。タッチパネルから接続する
 
 *********************************************************************/
#ifndef __NEXTION_H__
#define __NEXTION_H__

/* Header Files */
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<termios.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/ioctl.h>
#include	<time.h>

/* Macros */
#define	SERIALPORT	"/dev/ttyAMA0"
#define	CONFFILE	"/etc/dstarrepeater"
#define	INIFILE		"/etc/nextion.ini"
#define	LOGDIR		"/var/log/"
#define	DUMPFILE	"/tmp/tcpdump.dstar"
#define	USBPORT		"/dev/IDxxPlus"

#define	BAUDRATE	B9600
#define	LISTSIZE	512			// 最大リピータ数
#define	SLEEPCOUNT	200			// コマンド実行待ちカウント
#define	TXHANG		2			// ラストパケット検出後のハングタイム（秒）
#define	TXDELAY		1			// 実際の送信開始に対する表示開始の遅れ（秒）

#define	TRUE		1
#define FALSE		0

/* 日付表示関連 */
time_t	timer;
struct	tm *timeptr;
time_t	jstimer;
struct	tm *jstimeptr;

/* Variables */
typedef struct {				// Hole Punch リピータリスト
	char    name[20];			// 予備
	char    call[8];			// リピータコールサイン
	char    addr[16];			// リピータグローバルアドレス
	char    port[6];			// multi_forward接続ポート
	char    zone[8];			// リピータゾーンコール
} repeater_t;
extern	repeater_t	linkdata[LISTSIZE];     // リピータリスト構造体配列の宣言

typedef	struct {
	char	station[8];			// ノードコール（Terminal:個人コール/Access Point:クラブコール）
	char	default_rpt[8];			// 立ち上げ時自動接続リピータ
	int	microsec;			// リスト書き込み時のスピード調整用
	int	debug;				// 1の時デバッグモード
} nextion_ini_t;
extern	nextion_ini_t	nx;			// nextion.iniの内容

typedef	struct {
	char	station[8];			// ノードコール（Terminal:個人コール/Access Point:クラブコール）
	char	ipaddress[16];			// DStarRepeaterのIPアドレス
	char	localport[6];			// DStarRepeaterのローカルポート
	char	modemtype[16];			// DStarRepeaterのモデムタイプ
} dstarrepeater_t;
extern	dstarrepeater_t	ds;			// dstarrepeaterの設定内容

typedef struct {
	char	ipaddress[32];			// RasPiのIPアドレス
} getdatas_t;
extern	getdatas_t	gd;			// コマンド実行等で得たデータ

extern	char	stat_dmon[32];			// ログファイルからの状況取得用
extern	char	stat_dstar1[32];		// 	〃
extern	char	stat_dstar2[32]; 		// 	〃（主にラストハード）
extern	char	usercmd[32];			// タッチパネルからのコマンド
extern	char	linkref[8];			// 接続先リフレクタ
extern	char	station_dmon[8];		// dmonitor接続用コールサイン
extern	char	station_dstar[8];		// リフレクタ接続用コールサイン
extern	char	chklink[16];			// ループ内の多重処理禁止用
extern	char	chklink2[16];			// ループ内の多重処理禁止用
extern	char	chkstat[256];			// ループ内の多重処理禁止用
extern	char	chkstat2[256];			// ループ内の多重処理禁止用
extern	char	chkstat3[256];			// ループ内の多重処理禁止用
extern	int	cnt_temp;			// 温度測定間隔調整用カウンタ
extern	int	net_flag;			// ネット側ストリームon/off状態
extern	int	rf_flag;			// RF 側ストリームon/off状態
extern	char	rptcallpre[8];
extern	char	statpre[32];

/* Functions */
int	openport(char *devicename, long baudrate);
int	getlinkdata(void);
void	getusers(void);
void	getconfig(void);
void	dispstatus_dmon(void);
void	dispstatus_ref(void);
int	dispstreaminfo(void);
void	getipaddr(void);
void	disptempinfo(void);
void	sendcmd(char *cmd);
void	reflesh_idle(void);

void	recvdata(char *touchcmd);		// 統一する
void	recvdata(char *rptcon);


#endif // __NEXTION_H__
