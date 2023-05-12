TARGET_EXEC=lsh
CC=clang
CFLAGS=-Wall -Werror -g

$(TARGET_EXEC): main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm $(TARGET_EXEC)
	rm -rf lsh.dSYM
