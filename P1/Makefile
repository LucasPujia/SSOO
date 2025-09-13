CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

%: %.c
	$(CC) $(CFLAGS) $< -o $@ -lm

clean:
	rm -f *.o *.out *.exe