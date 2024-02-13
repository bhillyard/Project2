CC 	= gcc

CFLAGS  = -Wall -g -I ../include

LD 	= gcc

LDFLAGS  = -Wall -g -L../lib64

PROGS = lwp.o

all: $(PROGS)

allclean: clean
	@rm -f $(EXTRACLEAN)

clean:	
	rm -f $(OBJS) *~ TAGS

lwp: lwp.o ../lib64/libPLN.so
$(LD) $(LDFLAGS) -o lwp lwp.o
-IPLN

lwp.o: lwp.c fp.h lwp.h
$(CC) $(CFLAGS) -c lwp.c

