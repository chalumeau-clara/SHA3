# Makefile
# 19-Nov-11 Markku-Juhani O. Saarinen <mjos@iki.fi>

BINARY          = sha3
OBJS     	= sha3.o main.o
CC              = gcc
CFLAGS		= -Wall -O3


$(BINARY):      $(OBJS)
		$(CC) -o $(BINARY) $(OBJS)

.c.o:
		$(CC) $(CFLAGS)  -c $< -o $@

clean:
		rm -rf  $(OBJS) $(BINARY) *~
