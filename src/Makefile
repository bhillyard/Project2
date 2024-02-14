CC      = gcc

CFLAGS  = -Wall -g -I../include

LD      = gcc

LDFLAGS = -Wall -g -L../lib64

PROGS   = lwp.o

OBJS    = lwp.o rr.o util.o magic64.o

SRCS    = lwp.c rr.c util.c magic64.S

HDRS    = lwp.h rr.h util.h fp.h schedulers.h

EXTRACLEAN = core $(PROGS)

all: $(PROGS)

allclean: clean
	@rm -f $(EXTRACLEAN)

clean:  
	rm -f $(OBJS) *~ TAGS

my_program: $(OBJS) liblwp.a
	$(LD) $(LDFLAGS) -o my_program $(OBJS) -L. -llwp

lwp.o: lwp.c lwp.h
	$(CC) $(CFLAGS) -c lwp.c

rr.o: rr.c
	$(CC) $(CFLAGS) -c rr.c

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

magic64.o: magic64.S
	$(CC) $(CFLAGS) -c magic64.S

liblwp.a: lwp.o rr.o util.o magic64.o
	ar r liblwp.a lwp.o rr.o util.o magic64.o
	rm lwp.o rr.o util.o magic64.o

submission: $(SRCS) $(HDRS) Makefile README
	tar -cf project2_submission.tar $(SRCS) $(HDRS) Makefile README
	gzip project2_submission.tar