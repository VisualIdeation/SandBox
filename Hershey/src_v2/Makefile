#
# Makes the hershey library.
# Makes the binary Hershey font files for an SGI
#
FONTLIB = ../lib
CFLAGS = -O -float -DFONTLIB=\"$(FONTLIB)\"
CFLAGS = -g -DFONTLIB=\"$(FONTLIB)\" -m32
CFLAGS = -g -DFONTLIB=\"$(FONTLIB)\"
CC = cc

OURLIBS = libhershey.a
LIBS = -lgl_s
RANLIB = echo

.IGNORE:

HDR = h2v.h

all:	libhershey.a
#all:	libhershey.a h2v fdisp fonts

libhershey.a: htext.o check.o halloc.o fhtext.o
#	CC -c htext.c
#	CC -c check.c
#	CC -c halloc.c
#	CC -c fhtext.c
	ar rcv libhershey.a htext.o check.o halloc.o fhtext.o
	$(RANLIB) libhershey.a


h2v.o:	h2v.c h2v.h
	cc -c h2v.c

getchar.o:	getchar.c
	cc -c getchar.c

h2v:	h2v.o getchar.o
	cc -o $@ $@.o getchar.o -lm

fdisp:	fdisp.o libhershey.a
	cc -o $@ $@.o $(OURLIBS) $(LIBS) -lm

fonts:	h2v
	h2v ../data/hersh.oc
	h2v ../data/hersh.or ../fonts/japan.hmp japanese
	mv astrology cursive cyrillic futura.l futura.m gothic.eng \
	gothic.ger gothic.ita greek japanese markers math.low \
	math.upp meteorology music script symbolic times.g \
	times.i times.ib times.r times.rb $(FONTLIB)
	touch fonts

h2v: h2v.h

clean:
	rm -f astrology cursive cyrillic futura.l futura.m gothic.eng \
	gothic.ger gothic.ita greek japanese markers math.low \
	math.upp meteorology music script symbolic times.g \
	times.i times.ib times.r times.rb *.o core fonts libhershey.a

clobber:
	rm -f astrology cursive cyrillic futura.l futura.m gothic.eng \
	gothic.ger gothic.ita greek japanese markers math.low \
	math.upp meteorology music script symbolic times.g \
	times.i times.ib times.r times.rb *.o core h2v hdisp fdisp fonts \
	libhershey.a


