###################################################
# Makefile for compiling nextion for dmonitor
###################################################

# Program Name and object files
PROGRAM	= nextion
OBJECTS = DispCMDinfo.o DispLOGinfo.o dmonitor.o dstarrepeater.o Functions.o GetConfig.o GetLinkData.o GetUsers.o Main.o Nextion.o GetActive.o

# Redefine MACRO
CC	= gcc

# Define extention of Suffix Rules
.SUFFIXES	: .c .o

# Rule of compiling program
$(PROGRAM)	: $(OBJECTS)
	$(CC) -o $(PROGRAM) $^

# Sufix Rule
.c.o	:
	$(CC) -c $<

# Target of Delete files
.PHONY	: clean
clean	:
	$(RM)  $(PROGRAM) $(OBJECTS)

### Install files ###
install	:
# プログラムのコンパイル
	@echo "コンパイルしています..."
	@make > /dev/null
# プログラムの配置
	@echo "ファイルを配置しています..."
	@sudo mv $(PROGRAM)		/usr/local/bin
	@sudo cp $(PROGRAM).ini		/etc
	@sudo cp dmonitor_log		/etc/logrotate.d
# ユニットファイルの配置
	@sudo cp $(PROGRAM).service	/etc/systemd/system
	@sudo cp ircddbgateway.service	/etc/systemd/system
	@sudo cp ircddbgateway.timer	/etc/systemd/system
	@sudo cp dstarrepeater.service	/etc/systemd/system
	@sudo cp nextion.service	/etc/systemd/system
	@sudo cp nextion.timer		/etc/systemd/system
	@sudo systemctl daemon-reload
# serviceの起動設定
	@echo "サービスの有効／無効を調整しています..."
	@sudo systemctl stop	auto_repmon.service
	@sudo systemctl disable	auto_repmon.service	> /dev/null
	@sudo systemctl stop	rpt_conn.service
	@sudo systemctl disable	rpt_conn.service	> /dev/null
	@sudo systemctl stop	lighttpd.service
	@sudo systemctl disable	lighttpd.service	> /dev/null
	@sudo systemctl enable	ircddbgateway		> /dev/null
	@echo "D*SWITCH (Nextion Addon Driver) を自動起動に設定しています..."
	@sudo systemctl enable	$(PROGRAM).timer	> /dev/null
	@echo
	@echo "/etc/nextion.iniを編集してください。"
	@echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"

### nextionファイルのアップデート ###
NUM = $(shell pgrep -c dmonitor)
update	:
# dmonitorが動いていたら止める
ifneq ($(NUM),0)
	@echo 'dmonitorをstopします。'
	@sudo killall -2 dmonitor
endif
# プログラムのダウンロードとコンパイル
	@echo "アップデートのチェックとコンパイルをします..."
	@git pull
	@make > /dev/null
	@sudo mv $(PROGRAM)		/usr/local/bin
	@sudo cp dmonitor_log		/etc/logrotate.d
# serviceの起動設定
	@echo "サービスの有効／無効を調整しています..."
	@sudo systemctl stop    auto_repmon.service
	@sudo systemctl disable auto_repmon.service     > /dev/null
	@sudo systemctl stop    rpt_conn.service
	@sudo systemctl disable rpt_conn.service        > /dev/null
	@sudo systemctl stop	lighttpd.service
	@sudo systemctl disable	lighttpd.service		> /dev/null
	@sudo systemctl enable  ircddbgateway.service	> /dev/null
# Nextionの再起動
	@echo "D*SWITCH (Nextion Addon Driver) をリスタートしています..."
	@sudo systemctl enable  $(PROGRAM).service
	@sudo systemctl restart $(PROGRAM).service

# Dependency of Header Files
$(OBJECTS)		: Nextion.h

