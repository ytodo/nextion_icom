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

////////////////////////////////////////////////////////////////////////
// 	D-STAR  Nextion display for ICOM Terminal/Access Mode
//
//	ファイル名	Nextion.h 
//			2020.03.05 - 
//
//	機能	Multi_Forwardが搭載されているリピータリストを取得して
//		「接続可能リピータ」としてdmonitor用ディスプレイに表示。
//		タッチパネルから接続する
//		また、同様にターミナルモードのDStarRepeaterの接続と、
//		状態表示もし、双方を切れ替えて使用する
////////////////////////////////////////////////////////////////////////

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
#define	CONFFILE	"/etc/dstarrepeater"
#define	INIFILE		"/etc/nextion.ini"
#define	LOGDIR		"/var/log/"
#define	DUMPFILE	"/tmp/tcpdump.dstar"
#define	RIGFILE		"/var/www/rig.type"
#define	BAUDRATE	B9600
#define	LISTSIZE	512			// 最大リピータ数
#define	TXHANG		1			// ラストパケット検出後のハングタイム（秒）
#define	TXDELAY		0			// 実際の送信開始に対する表示開始の遅れ（秒）
#define	WAITTIME	100000			// 単位microsec（0.1秒）
#define	VERSION		0			// バージョン情報
#define	VERSUB		0
#define RELEASE		1

/* 日付表示関連 */
time_t	timer;
struct	tm *timeptr;
time_t	jstimer;
struct	tm *jstimeptr;

/* Variables */
typedef struct {				// Hole Punch リピータリスト
	char    call[9];			// リピータコールサイン
	char    addr[16];			// リピータグローバルアドレス
	char    port[6];			// multi_forward接続ポート
	char    zone[9];			// リピータゾーンコール
	char    name[20];			// 予備
} repeater_t;
extern	repeater_t	linkdata[LISTSIZE];     // リピータリスト構造体配列の宣言

typedef	struct {
	char	station[9];			// ノードコール（Terminal:個人コール/Access Point:クラブコール）
	char	default_rpt[9];			// 立ち上げ時自動接続リピータ
	char	default_ref[9];			// 立ち上げ時自動接続リフレクタ
	char	clock_color[8];			// デジタル時計の文字色指定
	char	microsec[8];			// リスト書き込み時のスピード調整用
	char	debug[1];			// 0:通常／1:デバッグモード(status表示が多くなる）
	char	rigtype[8];			// ICOM, DVAP, DVMEGA, NODE が入る
	char	nextion_port[16];		// ttyUSB0, ttyAMA0等nextion.iniで指定
} nextion_ini_t;
extern	nextion_ini_t	nx;			// nextion.iniの内容

typedef	struct {
	char	station[16];			// ノードコール（Terminal:個人コール/Access Point:クラブコール）
	char	ipaddress[16];			// DStarRepeaterのIPアドレス
	char	localport[6];			// DStarRepeaterのローカルポート
	char	modemtype[32];			// DStarRepeaterのモデムタイプ
} dstarrepeater_t;
extern	dstarrepeater_t	ds;			// dstarrepeaterの設定内容

typedef struct {
	int	mode;				// 使用中のモード
	int	selected_page;			// 表示されているページ (num % 21)
	int	num;				// 読み込み済みリピータ数
} status_t;
extern	status_t	st;			// Nextionの使用状況まとめ

extern	char	command[64];			// Nextionに送信するコマンド
extern	char	cmdline[128];			// システムコマンド
extern	char	stat_dmon[32];			// ログファイルからの状況取得用
extern	char	stat_dstar1[32];		// 	〃
extern	char	stat_dstar2[32]; 		// 	〃（主にラストハード）
extern	char	usercmd[32];			// タッチパネルからのコマンド
extern	char	linkref[9];			// 接続先リフレクタ
extern	char	station_dmon[9];		// dmonitor接続用コールサイン
extern	char	station_dstar[9];		// リフレクタ接続用コールサイン
extern	char	chklink[16];			// ループ内の多重処理禁止用
extern	char	chklink2[16];			// ループ内の多重処理禁止用
extern	char	line[256];
extern	char	chkline[256];			// ループ内の多重処理禁止用
extern	char	chkline2[256];
extern	char	rptcall[9];
extern	char	chkrptcall[9];
extern	char	status[32];
extern	char	chkstat1[32];
extern	char	chkstat2[32];
extern	char	chkstat3[32];

/* Functions */
int	openport(char *devicename, long baudrate);
int	getlinkdata(void);			// GetLinkData.c
void	next_page(void);			//	〃
void	previous_page(void);			//	〃
void	dmonitor(void);				// dmonitor.c
void	dstarrepeater(void);			// dstarrepeater.c
void	getusers(void);				// GetUsers.c
void	getactive(void);			// GetActive.c
void	getconfig(void);			// GetConfig.c
void	dispstatus_dmon(void);			// DispLOGinfo.c
void	dispstatus_ref(void);			//	〃
int	disp_stat();				//      〃
int	disp_rpt();				//      〃
void	dispipaddr(void);			// DispCMDinfo.c
void	disptemp(void);				//	〃
void	sendcmd(char *cmd);			// Functions.c
void	recvdata(char *touchcmd);		//	〃
void	dispclock(void);			//	〃
void	dispcapture(void);			//      〃
void	syscmdswitch(void);			//	〃

#endif // __NEXTION_H__
