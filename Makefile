CC:=gcc

CFLAGS:=-Wall -L.\ -I.\inc \
	-std=c99 \
	-finput-charset=UTF-8 \
	-fexec-charset=UTF-8

SRC:=winmain.c \
	event.c \
	cmd.c \
	painter.c \
	openfile.c \
	jpeg_loader.c

LIB:=-lfreeimage -lgdi32 -lshell32 -lComdlg32

EXE:=duma

all: debug
	$(EXE)

debug:
	$(CC) $(CFLAGS) -Og -g -DDEBUG $(SRC) -o $(EXE) $(LIB)

ok:
	$(CC) $(CFLAGS) -O3 -mwindows $(SRC) -o $(EXE) $(LIB)

