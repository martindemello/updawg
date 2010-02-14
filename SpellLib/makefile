MEMDEBUG =#  ../../mnemosyne/mnemosyn.o
# Must tidy up this shar file and the one in the directory above...
CC = gcc -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings

# Makefile for ../libspell.a

OBJECTS=check.o init.o print.o prime.o pathopen.o # cryptomatch.o wordio.o

all:	../libspell.a init
	echo All up to date

../libspell.a: $(OBJECTS)
	ar rc $@ $?
	ranlib $@

# These get build by implicit rules.

check.o: check.c
# wordio.o: wordio.c
init.o: init.c
print.o: print.c
prime.o: prime.c
pathopen.o: pathopen.c
cryptomatch.o: cryptomatch.c
# EOF

And the main Makefile is now:

CFLAGS =
LIBFLAG =

LINK = cc
LFLAGS =

LIBRARY=	libspell.a

#all: sextest texcheck dawg pdawg $(LIBRARY) sharfile init
#	echo All up to date

init:	init.c pathopen.c
	$(CC) -DTEST -oinit init.c pathopen.o $(MEMDEBUG) check.o
	$(CC) -c init.c

texcheck: texcheck.c $(LIBRARY)
	$(CC) -o texcheck texcheck.c $(LIBRARY)

sextest: sextest.c $(LIBRARY)
	$(CC) -o sextest sextest.c $(LIBRARY)

dawg: dawg.c $(LIBRARY)
	$(CC) -o dawg dawg.c $(LIBRARY)

pdawg: pdawg.c $(LIBRARY)
	$(CC) -o pdawg pdawg.c $(LIBRARY)

$(LIBRARY): SpellLib/init.c \
	SpellLib/print.c \
	SpellLib/prime.c \
	SpellLib/check.c
# SpellLib/wordio.c
	(cd SpellLib; make -$(MAKEFLAGS) ../libspell.a)

sharfile: spelllib.shar
	echo Sharfile created

spelllib.shar:	check.c check.h init.c init.h pathnames.h pathopen.c pathopen.h prime.c prime.h print.c print.h
	shar check.c check.h init.c init.h pathnames.h pathopen.c pathopen.h prime.c prime.h print.c print.h makefile > spelllib.shar
