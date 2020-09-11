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
	sudo mv $(PROGRAM)		/usr/local/bin
	sudo cp $(PROGRAM).service	/etc/systemd/system
	sudo systemctl daemon-reload
	sudo killall -s 9 dmonitor
	sudo systemctl stop auto_repmon.service
	sudo systemctl disable auto_repmon.service
	sudo systemctl stop rpt_conn.service
	sudo systemctl disable rpt_conn.service
	sudo systemctl enable 	$(PROGRAM).service
	sudo systemctl restart	$(PROGRAM).service

# Dependency of Header Files
$(OBJECTS)		: Nextion.h
