CFLAGS = -fPIC -I. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gee-1.0
CC = gcc
VALACFLAGS = --Xcc="-I." --Xcc="-I/usr/include/gee-1.0" --Xcc="-L." --Xcc="-ldawg" --Xcc="-lgee"
VALAGUIFLAGS = --pkg gee-1.0 --pkg libspell --pkg hildon-1 --vapidir . --includedir .
VALACLIFLAGS = --pkg gee-1.0 --pkg libspell --pkg readline --vapidir . --includedir .
EXECS = updawg updawg-cli

default : updawg

clean:
	rm -f *.o *.so* *.a SpellLib/*.o $(EXECS)

libspell.a :
	cd SpellLib; make ../libspell.a

libdawg.a : dawg_words.o wild.o libspell.a
	cp libspell.a libdawg.a
	ar rs libdawg.a dawg_words.o wild.o

updawg : updawg.vala libdawg.a
	valac -o updawg $(VALAGUIFLAGS) $(VALACFLAGS) updawg.vala dawgsearch.vala

updawg-cli : updawg-cli.vala libdawg.a
	valac -o updawg-cli $(VALACLIFLAGS) $(VALACFLAGS) --Xcc="-lreadline" updawg-cli.vala dawgsearch.vala

wild: wild.vala libdawg.a
	valac -o wild $(VALAFLAGS) $(VALACFLAGS) wild.vala

install: updawg updawg-cli updawg.png csw.dwg
	install -d ${DESTDIR}/usr/bin
	install updawg ${DESTDIR}/usr/bin
	install updawg-cli ${DESTDIR}/usr/bin
	install -d ${DESTDIR}/usr/share/applications/hildon
	install updawg.desktop ${DESTDIR}/usr/share/applications/hildon
	install -d ${DESTDIR}/usr/share/pixmaps
	install updawg.png ${DESTDIR}/usr/share/pixmaps
	install -d ${DESTDIR}/usr/share/updawg
	install csw.dwg ${DESTDIR}/usr/share/updawg
