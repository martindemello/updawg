#define PAGE_SIZE 4096		/* Dbugging why mmap fails some times */
/*

    File:    init.c
    Authors: Richard Hooker, Graham Toal
    Purpose: Loading of dictionary for spelling checker.
    Functions exported:  dawg_init, pack_init
    Internal functions:  spell_init

Description:

The address of a dictionary (either a PACKed dict or a DAWG) is set up by
this procedure.  This gives us the option of connecting the dict in read-only
(or copy-on-write) virtual memory.  On non-VM machines, we simply allocate a
large buffer into which the relevant part of the dictionary is loaded.

The magic number is used to check the dict type, *and* the machine byte-sex.
If the sex is reversed, even a VM system has to load the data into writable
memory (so that it can reverse it).

The dictionary format is currently:

           [magic number]
           [no. of edges]
Dawg --->  [     0      ]
           [... edges to end of dict ...]

The base address of the dawg once loaded into memory is that of the [0].
If at some future date we extend the dawg format, the new files will
not have a [0] word at that position in the file.

*/


#include "../splib.h"
#include "pathnames.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef MEMDEBUG
#include "../../mnemosyne/mnemosyn.h"
#endif

int     init_debug = (0 != 0);

/*######################### INTERNAL FUNCTIONS #########################*/


static long
sex_change(long number)
{				/* Most inefficient and cumbersome (but safe)
				 * version) */
	long    b0, b1, b2, b3;
	b0 = number & 255L;
	number >>= 8L;
	b1 = number & 255L;
	number >>= 8L;
	b2 = number & 255L;
	number >>= 8L;
	b3 = number & 255L;
	number = b0;
	number <<= 8L;
	number = number | b1;
	number <<= 8L;
	number = number | b2;
	number <<= 8L;
	number = number | b3;
	return (number);
}


/*####################### EXPORTED FUNCTIONS #########################*/

/* There's a problem with this interface -- no means of *unload*ing
   the dict when you're done with it!  I'd return the file handle,
   but I'm using fopen() to read the file in the case of accidental
   sex-reversal */

int
dawg_init(char *filename, NODE ** dictp, INDEX * nedgesp)
{
/* Two VAR parameters */
#define dict (*dictp)
#define nedges (*nedgesp)
	int     fd;
	INDEX   count;
	int     dict_handle;
	int     rc;
	long    dawg_magic;
	int     reversing = FALSE;
	struct stat statBuf;
	char   *full_name = NULL;
	NODE   *dawg = NULL;
	INDEX   i;

	/* init_dict("") gets default */
	if (*filename == '\0')
		filename = DEFAULT_DAWG;

	/* Note: full_name is mallocked by this. Remember to free it when
	 * done. */
	dict_handle = pathopen("DICTPATH", _SPELL_PATH, filename, "dwg", &full_name);
	if (dict_handle == -1) {
		fprintf(stderr, "Can\'t open word list \"%s\"\n", filename);
		return (FALSE);
	}
	read(dict_handle, (char *) &dawg_magic, sizeof(dawg_magic));	/* And check result! */
	read(dict_handle, (char *) &nedges, sizeof(nedges));	/* And check result! */

	if (init_debug)
		fprintf(stderr, "dawg contains %8lx edges\n", nedges);

	/* Allocate precisely enough memory for all edges + 0 at root node. */
	if ((dict = malloc(((size_t) (nedges + 1)) * sizeof(NODE *))) == 0) {
		fprintf(stderr, "Can\'t allocate space (%ld bytes) for dictionary\n",
		    (long) (nedges + 1) * sizeof(NODE *));
		return (FALSE);
	}
	count = read(dict_handle, (char *) dict, sizeof(NODE) * (size_t) nedges);	/* Better be 32-bit
										 * machine! */
	if (count != sizeof(NODE) * nedges) {
		fprintf(stderr,
		    "Failed to read dictionary %s - wanted %ld bytes, got %ld\n",
		    filename, sizeof(NODE) * nedges, count);
		return (FALSE);
	}

	if (dawg_magic == DAWG_MAGIC) {
		/* nedges should correspond to statBuf.st_size -- unless we
		 * add extra fields as a trailer later */
		return (TRUE);
	}
	if (dict[0] != 0L) {
		fprintf(stderr,
		    "*** Error: file %s is in a newer dawg format than I know about!\n",
		    full_name);
		return (FALSE);
	}
	if (dawg_magic != REVERSE_DAWG_MAGIC) {
		fprintf(stderr, "*** Error: file %s is not in dawg format\n", full_name);
		return (FALSE);
	}
	fprintf(stderr,
	    "*** Warning: You are using a byte-sex reversed dictionary (%s)\n",
	    filename);
	fprintf(stderr,
	    "             It will be more efficient if you can reverse it.\n");
	/* I'm going to unmap & close, though I think what I should have done
	 * was map in MAP_PRIVATE mode which gives copy-on-write, and swap in
	 * situ. */
	/* Can't find munmap at link time! */
	/* munmap((caddr_t) dawg, statBuf.st_size); */
	close(dict_handle);	/* Reopen and read into ram, sex-changing */
	return (TRUE);
#undef nedges
#undef dict
}
#ifdef TEST
#define TESTDECL int main(int argc, char **argv)
TESTDECL
{
	int     web2, words, web2a;
	NODE   *web2dawg, *wordsdawg, *web2adawg;
	INDEX   web2edges, wordsedges, web2aedges;
	web2 = dawg_init("web2", &web2dawg, &web2edges);
	words = dawg_init("words", &wordsdawg, &wordsedges);
	web2a = dawg_init("web2a", &web2adawg, &web2aedges);
	if (web2)
		fprintf(stderr, "web loaded.\n");
	else
		fprintf(stderr, "failed to load web2.\n");
	if (words)
		fprintf(stderr, "words loaded.\n");
	else
		fprintf(stderr, "failed to load words.\n");
	if (web2a)
		fprintf(stderr, "web2a loaded.\n");
	else
		fprintf(stderr, "failed to load web2a.\n");
	if (!(web2 && words && web2a))
		exit(EXIT_FAILURE);
	if (argc == 1)
		exit(EXIT_SUCCESS);
	if (dawg_check(wordsdawg, argv[1]))
		fprintf(stderr, "%s in words\n", argv[1]);
	else
		fprintf(stderr, "%s is not in words\n", argv[1]);
	if (dawg_check(web2adawg, argv[1]))
		fprintf(stderr, "%s in web2a\n", argv[1]);
	else
		fprintf(stderr, "%s is not in web2a\n", argv[1]);
	if (dawg_check(web2dawg, argv[1]))
		fprintf(stderr, "%s in web2\n", argv[1]);
	else
		fprintf(stderr, "%s is not in web2\n", argv[1]);
	exit(0);
}
#endif
