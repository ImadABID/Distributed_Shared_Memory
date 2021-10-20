ALL: default 

CC           = gcc
CLINKER      = $(CC)
OPTFLAGS     = -O0


SHELL = /bin/sh

CFLAGS  =   -DREENTRANT -Wunused -Wall -g 
CCFLAGS = $(CFLAGS)
LIBS =  -lpthread

EXECS = common.o dsmexec dsmwrap truc 

default: $(EXECS)

dsmexec: dsmexec.o common.o dsmexec_utils.o
	$(CLINKER) $(OPTFLAGS) -o $@ $^ $(LIBS)
	@-if [ ! -d "bin" ]; then mkdir bin ; fi
	@-mv $@ ./bin/

dsmwrap: dsmwrap.o common.o
	$(CLINKER) $(OPTFLAGS) -o $@ $^ $(LIBS)
	@-if [ ! -d "bin" ]; then mkdir bin ; fi
	@-mv $@ ./bin/

truc: truc.o common.o
	$(CLINKER) $(OPTFLAGS) -o $@ $^ $(LIBS)
	@-if [ ! -d "bin" ]; then mkdir bin ; fi
	mv $@ ./bin/

dsmexec_utils.o: dsmexec_utils.c dsmexec_utils.h
	gcc -o dsmexec_utils.o -c dsmexec_utils.c

clean:
	@-/bin/rm -f *.o *~ PI* $(EXECS) *.out core 

veryclean: clean
	@-/bin/rm -f ./bin/*
	@-/bin/rmdir ./bin

.c:
	$(CC) $(CFLAGS) -o $* $< $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $<
.o:
	${CLINKER} $(OPTFLAGS) -o $* $*.o $(LIBS)
