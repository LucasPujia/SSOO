.POSIX:
.SUFFIXES:
.SUFFIXES: .c

DEFINES = -D_POSIX_SOURCE -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700

.c:
	$(CC) $(DEFINES) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

BIN = cliente servidor

SRC = $(BIN:=.c)

all: $(BIN)

$(SRC):

clean:
	rm -f $(BIN)
