gcc -fPIC -c -g dawg_words.c -I. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gee-1.0
gcc -shared -Wl,-soname,libdawg.so.1 -o libdawg.so.1.0.1 dawg_words.o -L. -lspell -lgee
valac -o printdawg --pkg libspell --pkg gee-1.0 --vapidir . --includedir . --Xcc="-I." --Xcc="-L." --Xcc="-lspell" printdawg.vala
valac -o anags --pkg gee-1.0 --pkg libspell --vapidir . --includedir . --Xcc="-I." --Xcc="-I/usr/include/gee-1.0" --Xcc="-L." --Xcc="-lspell" --Xcc="-ldawg" --Xcc="-lgee" anags.vala
