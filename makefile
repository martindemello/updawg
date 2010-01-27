CFLAGS = -fPIC -I. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gee-1.0
CC = gcc
VALACFLAGS = --Xcc="-I." --Xcc="-I/usr/include/gee-1.0" --Xcc="-L." --Xcc="-ldawg" --Xcc="-lgee"
VALAFLAGS = --pkg gee-1.0 --pkg libspell --pkg hildon-1 --vapidir . --includedir .

default : anags

clean:
	rm *.o *.so* *.a SpellLib/*.o

libspell.a :
	cd SpellLib; make ../libspell.a

libdawg.a : dawg_words.o libspell.a
	cp libspell.a libdawg.a
	ar rs libdawg.a dawg_words.o

anags : anags.vala libdawg.a
	valac -o anags $(VALAFLAGS) $(VALACFLAGS) anags.vala

