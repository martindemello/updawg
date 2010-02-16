/* dawg_words.c - find words in a dawg using letters on your rack */

/* Copyright (C) 1994 by John J. Chew, III <jjchew@math.utoronto.ca>
 * All rights reserved.
 *
 * Copyright (C) 2010 by Martin DeMello <martindemello@gmail.com>
 * Changes:
 * * Added wildcards ('*' = 0 or more blanks)
 * * Output to a gee_array_list rather than stdout
 *
 */

/* based on Graham Toal's DAWG package, as posted to alt.sources */

#undef DEBUG
#define MAXWORD 256

#include <stdio.h>
#include <unistd.h>
#include <gee.h>
#include <string.h>

#include "splib.h"

#ifdef DEBUG
#define PR_DUMP(Tag, I, Old, New) \
  printf("%s: i=%d old=%08x[%c]%c%c new=%08x[%c]%c%c ", (Tag), (I), \
      (Old)-dawg, ((*(Old))>>V_LETTER) & M_LETTER, \
      (*(Old)&M_END_OF_NODE)?'N':' ', \
      (*(Old)&M_END_OF_WORD)?'W':' ', \
      (New)-dawg, (*(New))>>V_LETTER & M_LETTER, \
      (*(New)&M_END_OF_NODE)?'N':' ', \
      (*(New)&M_END_OF_WORD)?'W':' ' \
      ); \
putRack(nChars); putchar(' '); \
putWord(word, blanks, i);

#endif

#define D gee_iterator_get

void putRack(unsigned int *count) {
  register int i, j;
  for (i=0; i<256; i++) for (j=0; j<count[i]; j++) putchar(i);
}

void putWord(char *chars, char *blanks, int length) {
  register char ch;
  while (length--) {
    ch = *chars++;
    *blanks++ == 1 ? putchar(ch ^ ('a'-'A')) : putchar(ch); /* ASCIIism */
  }
  putchar('\n');
  return;
}

void dawg_anagrams(NODE *dawg, register char *rack, int bingosOnly, GeeArrayList* retwords, GeeArrayList* retblanks) {
  unsigned int       bingoLength = 0;
  char               blanks[MAXWORD];
  int                wilds[MAXWORD];
  NODE              *edge;
  register int       i;
  register unsigned  u;
  unsigned int       nBlanks = 0;
  unsigned int       wild = FALSE;
  unsigned int       used_chars = 0;
  unsigned int       nChars[256];
  NODE              *stemEdges[MAXWORD];
  char               word[MAXWORD];
  int               q;

  (void) memset(&nChars[0], 0, sizeof nChars);
  while (u = *rack++) {
    if (u == '*') {
      wild = TRUE;
    } else {
      bingoLength++;
      u == '?' ? nBlanks++ : nChars[u]++;
    }
  }

  edge = dawg + ROOT_NODE;
  i = 0;
#ifdef DEBUG
  PR_DUMP("st", i, edge, edge)
#endif

    while (1) {
      u = (*edge >> V_LETTER) & M_LETTER;     /* u = char at this node       */
      if (nChars[u]) {                        /* if still a u on rack        */
        nChars[u]--;                            /* take it off the rack      */
        blanks[i] = 2;                          /* not a blank               */
        wilds[i] = 0;
        used_chars++;                         /* we have used a character    */
        goto added_letter;
      }
      else if (nBlanks) {
        nBlanks--;
        used_chars++;                       /* we have used a character    */
        blanks[i] = 1;
        wilds[i] = 0;
        goto added_letter;
      }
      else if (wild) {
        blanks[i] = 1;
        wilds[i] = 1;
added_letter:
        word[i] = u;
        if (*edge & M_END_OF_WORD) {              /* if we have a word       */
          if (bingoLength <= used_chars) {                   /* if it's long enough   */
            gee_collection_add(GEE_COLLECTION (retwords), g_strndup(word, i+1));
            gee_collection_add(GEE_COLLECTION (retblanks), g_memdup(blanks, i+1));
            if (!wilds[i]) {
              goto unadd;
            }
          }
          if (!bingosOnly) {                        /* if we don't care       */
            gee_collection_add(GEE_COLLECTION (retwords), g_strndup(word, i+1));
            gee_collection_add(GEE_COLLECTION (retblanks), g_memdup(blanks, i+1));
          }
        }
        if (*edge & M_NODE_POINTER) {             /* can we go deeper?       */
#ifdef DEBUG
          PR_DUMP("dn", i, edge, dawg+(*edge&M_NODE_POINTER))
#endif
            stemEdges[i++] = edge;                    /* save this stem        */
          edge = dawg + (*edge & M_NODE_POINTER);   /* walk down graph       */
          continue;                                 /* try again             */
        }
unadd:
        if (!wilds[i]) {
          blanks[i] == 1 ? nBlanks++ : nChars[u]++;
          used_chars--;
        }
      }
up:
      while (*edge & M_END_OF_NODE) {
        if (i-- == 0) return;
#ifdef DEBUG
        PR_DUMP("up", i, edge, stemEdges[i])
#endif
        if (!wilds[i]) {
          blanks[i] == 1 ? nBlanks++ : nChars[word[i]]++;
          used_chars--;
        }
        edge = stemEdges[i];
      }
#ifdef DEBUG
      PR_DUMP("nx", i, edge, edge+1)
#endif
        edge++;                                   /* try next edge       */
    }
  return;
}

#if 0
int main(unsigned argc, char **argv) {
  int           bingosOnly = 1;
  NODE         *dawg;
  char         *dictFileName = 0;
  register int  i;
  INDEX         nedges;
  unsigned int  nErrors = 0;
  extern int    optind;
  extern char  *optarg;
  char         *rackString = 0;
  GeeArrayList        *output;
  GeeArrayList        *blanks;
  GeeIterator *li, *lj;
  int q;

  g_type_init ();

  while (EOF != (i = getopt(argc, argv, "abd:r:"))) switch(i) {
    case 'a': bingosOnly = 0; break;
    case 'b': bingosOnly = 1; break;
    case 'd': dictFileName = optarg; break;
    case 'r': rackString = optarg; break;
    case '?': nErrors = 1; break;
  }

  if (nErrors || !dictFileName || !rackString || optind != argc) {
    fprintf(stderr, "Usage: %s [-a|-b] -d dict -r rack\n", argv[0]);
    fputs("  -a       list all words, not just bingos\n", stderr);
    fputs("  -b       list bingos only (default)\n", stderr);
    fputs("  -d dict  DAWG dictionary in which to find words\n", stderr);
    fputs("  -r rack  letters with which to make word (?=blank)\n", stderr);
    exit(2);
  }

  if (!dawg_init(dictFileName, &dawg, &nedges)) exit(2);

  output = gee_array_list_new (G_TYPE_STRING, ((GBoxedCopyFunc) g_strdup), g_free, g_direct_equal);
  blanks = gee_array_list_new (G_TYPE_STRING, ((GBoxedCopyFunc) g_strdup), g_free, g_direct_equal);
  dawg_anagrams(dawg, rackString, bingosOnly, output, blanks);
  printf("%d\n", gee_collection_get_size( GEE_COLLECTION(output)));
  li = gee_iterable_iterator (GEE_ITERABLE (output));
  lj = gee_iterable_iterator (GEE_ITERABLE (blanks));
  char *w, *b;
  while (gee_iterator_next(li)) {
    gee_iterator_next(lj);
    w = (char *) gee_iterator_get(li);
    b = (char *) gee_iterator_get(lj);
    putWord(w, b, strlen(w));
  }

  exit(0);
}
#endif
