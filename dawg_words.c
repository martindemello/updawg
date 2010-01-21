/* dawg_words.c - find words in a dawg using letters on your rack */

/* Copyright (C) 1994 by John J. Chew, III <jjchew@math.utoronto.ca>
* All rights reserved.
* 
* Usage: dawg_words.c -d dict.dawg -r rack
*
* Lists words to stdout.
*/

/* based on Graham Toal's DAWG package, as posted to alt.sources */
/* See Also... SpellLib/wild.c */

#undef DEBUG
#define MAXWORD 256

#include <stdio.h>
#include <unistd.h>

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

void putRack(unsigned int *count) {
  register int i, j; 
  for (i=0; i<256; i++) for (j=0; j<count[i]; j++) putchar(i);
  }
   
void putWord(char *chars, int *blanks, int length) {
  register char ch;
  while (length--) {
    ch = *chars++;
#ifdef OLD_BUG
    *blanks++ ? putchar(ch ^ ('a'-'A')) : putchar(ch); /* ASCIIism */
#else
    /* code was not i18n.  We also want html ... */ 
    *blanks++ ? printf("<strong>%c</strong>", ch): putchar(ch);
#endif
    }
  putchar('\n');
  return;
  }

void processRack(NODE *dawg, register char *rack, int bingosOnly) {
  unsigned int       bingoLength = 0;
  int                blanks[MAXWORD];
  NODE              *edge;
  register int       i;
  register unsigned  u;
  unsigned int       nBlanks = 0;
  unsigned int       nChars[256];
  NODE              *stemEdges[MAXWORD];
  char               word[MAXWORD];
  
  (void) memset(&nChars[0], 0, sizeof nChars);
  while (u = *rack++) { bingoLength++; u == '?' ? nBlanks++ : nChars[u]++; }

  edge = dawg + ROOT_NODE;
  i = 0;
#ifdef DEBUG
  PR_DUMP("st", i, edge, edge)
#endif

  while (1) {
    u = (*edge >> V_LETTER) & M_LETTER;     /* u = char at this node       */
    if (nChars[u]) {                        /* if still a u on rack        */
      nChars[u]--;                            /* take it off the rack      */
      blanks[i] = 0;                          /* not a blank               */
      goto added_letter;
      }
    else if (nBlanks) {
      nBlanks--;
      blanks[i] = 1;
added_letter:
      word[i] = u;
      if (*edge & M_END_OF_WORD) {              /* if we have a word       */
	if (bingoLength == i+1) {                   /* if it's long enough   */
	  putWord(word, blanks, i+1);
	  goto unadd;
	  }
	if (!bingosOnly)                         /* if we don't care       */
	  putWord(word, blanks, i+1);
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
      blanks[i] ? nBlanks++ : nChars[u]++;
      }
up:
    while (*edge & M_END_OF_NODE) {
      if (i-- == 0) return;
#ifdef DEBUG
      PR_DUMP("up", i, edge, stemEdges[i])
#endif
      blanks[i] ? nBlanks++ : nChars[word[i]]++;
      edge = stemEdges[i];
      }
#ifdef DEBUG
    PR_DUMP("nx", i, edge, edge+1)
#endif
    edge++;                                   /* try next edge       */
    }
  return;
  }

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

  processRack(dawg, rackString, bingosOnly);

  exit(0);
  }

