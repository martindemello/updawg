/*
    A rather quick hack to open a file via a path -- could be made
    more general.  Would prefer to use a standard one out of a library
    but there doesn't seem to be one...
      Seems to cover all the bases -- elements of path may end in '/' if
    that's your style; absolute (rooted) filenames do not go via path,
    and '.' works OK.  I would prefer not to do the open() in this code
    but I suppose I have to worry about efficiency. [I would prefer not
    to do the open because I might want to find a file for reading via
    FILE * instead, or for some other reason that opening it.]

    pathopen(
       "PATHNAME",                    Open a dictionary file via PATHNAME
       default path,                  what to use if PATHNAME not found
       dictionary name,               root dict name (eg "words", or "english" etc)
       extension,                     "dwg" for a dawg file at the moment.
       &expanded name)                Fills in full name with path and extension.
                                   -> int file descriptor or -1 for not found.

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef MEMDEBUG
#include "../../mnemosyne/mnemosyn.h"
#endif

int     pathopen_debug = (0 != 0);

/* Generic Unix thingummybobs - could put inline but it's easier
   for me to test at home on my Archimedes if I make it portable... */

#define DIRSEP "/"
#define EXTSEPCH '.'
#define PATHSEPCH ':'
#define ROOT "/"
#define HOME "."		/* To catch non-path names ./fred & ../fred */
#define HERE "./"

#ifdef __riscos
#undef DIRSEP
#undef EXTSEPCH
#undef PATHSEPCH
#undef ROOT
#undef HERE
#define DIRSEP "."
#define EXTSEPCH '-'
#define PATHSEPCH ','
#define ROOT "$."
#define HERE "@."
#endif

int     getpath_debug = (0 == 0);	/* May be tweaked by debugger */

static int
absolute(char *fname)
{				/* Don't look in path if absolute filename
				 * given */
	/* This could easily be a macro, but I've made it a function so that
	 * it can more easily be expanded later.  (For instance, on the
	 * Archimedes it is quite a hassle to work out whether a filename is
	 * absolute...) */
	if (strncmp(fname, ROOT, strlen(ROOT)) == 0)
		return (0 == 0);
	if (strncmp(fname, HOME, strlen(HOME)) == 0)
		return (0 == 0);
	return (0 != 0);
}

int
pathopen(char *pathname, char *defpath, char *basename, char *ext, char **fullnamep)
{
#define NULLEXIT(p) if (p == NULL) exit(EXIT_FAILURE); else	/* OK */
#define fullname (*fullnamep)

	char   *dictp = NULL, *dictpath = NULL;
	char   *sep = NULL, *place = NULL;
	char   *filename = NULL;
	char   *savepath = NULL, *savefile = NULL;
	int     f = -1;

	dictp = "";
	if (absolute(basename) || (dictp = getenv(pathname)) != NULL) {
		savepath = dictpath = malloc(strlen(dictp) + 1);
		NULLEXIT(dictpath);
		strcpy(dictpath, dictp);
	} else {
		savepath = dictpath = malloc(strlen(defpath) + 1);
		NULLEXIT(dictpath);
		strcpy(dictpath, defpath);
	}

	for (;;) {
		sep = strchr(dictpath, PATHSEPCH);
		if (sep == NULL) {	/* last element of path */
			place = dictpath;
			dictpath = dictpath + strlen(dictpath);
		} else {	/* More to come */
			place = dictpath;
			*sep = '\0';
			dictpath = sep + 1;
		}
		if (*place == '\0') {	/* "/usr/share/dict:/usr/dict:" --
					 * last ":" implies current dir */
			savefile = filename = malloc(strlen(basename) + 1);
			NULLEXIT(filename);
			strcpy(filename, basename);
		} else {	/* Use given path.  Allow it to be of the form
				 * "/usr/dict/" */
			if (strncmp(place + strlen(place) - strlen(DIRSEP), DIRSEP, strlen(DIRSEP)) == 0) {
				savefile = filename = malloc(strlen(place) + strlen(basename) + 1);
				NULLEXIT(filename);
				sprintf(filename, "%s%s", place, basename);
			} else {
				savefile = filename = malloc(strlen(place) + strlen(DIRSEP) + strlen(basename) + 1);
				NULLEXIT(filename);
				sprintf(filename, "%s%s%s", place, DIRSEP, basename);
			}
		}
		if (strncmp(filename, HERE, strlen(HERE)) == 0) {	/* For tidyness, strip
									 * leading "./" */
			filename += strlen(HERE);
		}
		if (strrchr(filename, EXTSEPCH) == NULL && ext != NULL) {
			/* If no extension given, add default */
			char   *fileext = malloc(strlen(filename) + 1 + strlen(ext) + 1);
			NULLEXIT(fileext);
			sprintf(fileext, "%s%c%s", filename, EXTSEPCH, ext);
			free(savefile);
			savefile = filename = fileext;
		}
		if (getpath_debug)
			if (pathopen_debug)
				fprintf(stderr, "Looking for %s\n", filename);
		if ((f = open(filename, O_RDONLY)) != -1) {
			fullname = filename;	/* Returns a mallocked string */
			free(savepath);
			return (f);	/* return the fd of the file */
		}
		if (sep == NULL && *dictpath == '\0')
			break;
	}

	free(savefile);
	free(savepath);
	fullname = NULL;

	return (-1);		/* could not open the file */
#undef NULLEXIT
#undef fullname
}
#ifdef DBMAIN

#define DICTPATH "/usr/share/dict:/usr/dict:."
#ifdef __riscos
#undef DICTPATH
#define DICTPATH "$.dicts,"
#endif

#define MAINDECL int main(int argc, char **argv)
MAINDECL			/* Sneakily hide it from 'mkptypes' :-) */
{
	char   *fullname;
	int     fd;
	if (argc != 2) {
		fprintf(stderr, "syntax: pathopen dictname\n");
		exit(EXIT_FAILURE);
	}
	getpath_debug = (0 == 0);
	fd = pathopen("DICTPATH", DICTPATH, argv[1], "dwg", &fullname);
	fprintf(stderr, "pathopen(\"DICTPATH\", \"%s\", \"%s\", \"dwg\", -> ", DICTPATH, argv[1]);
	if (fd != -1) {
		fprintf(stderr, "\"%s\") -> %d\n", fullname, fd);
		free(fullname);
	} else {
		fprintf(stderr, "NULL) -> -1\n");
	}
}
#endif				/* DBMAIN */
