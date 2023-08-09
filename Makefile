.PHONY: all test install clean

prefix = /usr/local
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include

CC = cc
INSTALL = install

ALL = libchtrie.a log_malloc.o marktime.o chtrie.o

all: $(ALL)

chtrie.o: chtrie.c chtrie.h
	$(CC) $(CFLAGS) -c -o $@ $<

log_malloc.o: log_malloc.c log_malloc.h
	$(CC) $(CFLAGS) -c -o $@ $<

marktime.o: marktime.c marktime.h
	$(CC) $(CFLAGS) -c -o $@ $<

libchtrie.a: chtrie.o
	ar -cr $@ $^

run.tmp: chtrie.o chtrie.h test.c
	$(CC) $(CFLAGS) -o $@ marktime.o log_malloc.o chtrie.o test.c -lreadline

lhprun.tmp: chtrie.o chtrie.h lhptest.c
	$(CC) $(CFLAGS) -o $@ marktime.o log_malloc.o chtrie.o lhptest.c -lreadline

test: run.tmp
	./runtest

lhptest: lhprun.tmp
	./lhpruntest

install: all
	$(INSTALL) -d $(includedir) $(libdir)
	$(INSTALL) chtrie.h $(includedir)/
	$(INSTALL) libchtrie.a $(libdir)/

clean:
	rm -rf $(ALL) *.tmp
