#
# Makefile for the UM
# 
CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -l40locality -lcii40 -lm

EXECS   = writetests um

all: $(EXECS)

um: um.o memory.o arithmetic.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
writetests: umlabwrite.o umlab.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o *.dump *out *err *reference um writetests failedTests.txt out outReference

