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

# Install files
install	:
# プログラムのコンパイル
	@echo "コンパイルしています..."
	@make > /dev/null
# プログラムの配置
	@echo "ファイルを配置しています..."
	@sudo mv $(PROGRAM)		/usr/local/bin
	@sudo cp $(PROGRAM).ini		/etc
# ユニットファイルの配置
	@sudo cp $(PROGRAM).service	/etc/systemd/system
	@sudo cp ircddbgateway.service	/etc/systemd/system
	@sudo cp dstarrepeater.service	/etc/systemd/system
	@sudo systemctl daemon-reload
# serviceの起動設定
	@echo "サービスの有効／無効を調整しています..."
	@sudo systemctl stop	monitorix
	@sudo systemctl disable	monitorix
	@sudo systemctl stop	lightdm.service
	@sudo systemctl disable lightdm.service
	@sudo systemctl stop	auto_repmon.service
	@sudo systemctl disable	auto_repmon.service
	@sudo systemctl stop	rpt_conn.service
	@sudo systemctl disable	rpt_conn.service
	@sudo systemctl enable	ircddbgateway.service
	@sudo systemctl enable	$(PROGRAM).service
	@sudo systemctl restart	$(PROGRAM).service

update	:
# プログラムのダウンロードとコンパイル
	@echo "アップデートのチェックとコンパイルをします..."
	@git pull
	@make > /dev/null
	@sudo mv $(PROGRAM)		/usr/local/bin
	@echo "D*MONITOR をリスタートしています..."
	@sudo killall -2 dmonitor
	@sudo systemctl enable nextion.service
	@sudo systemctl restart nextion.service

# Dependency of Header Files
$(OBJECTS)		: Nextion.h

