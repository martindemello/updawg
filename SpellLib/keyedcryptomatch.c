/*

    File:          keyedcryptomatch.c
    Author:        Graham Toal
    Purpose:       find words which match the letter pattern of a cryptogram
    Creation date: 24/09/00 14:55:22
    Lastedit:      24/09/00 15:14:19

    Description:

       See cryptomatch.c in this directory for the description of the
    basic algorithm.  This variation is similar in principle but MUCH
    faster because when it looks up a pattern, that pattern is indexed
    by an initial key which is followed in a single operation without
    any searching necessary.  This effectively reduces the size of the
    dawg to be searched to include only those words that are potential
    candidates based on their internal pattern, ignoring any fixed
    mappings already calculated by the crypto solver.  This reduces
    the search space considerably.  The timings are:

    cryptomatch ABCDEFG
    15 seconds

    keyedcryptomatch ABCDEFG
    5 seconds

    (incidentally, the differences are actually greater than this because
    of the fixed overhead in the testbed of loading the dict file, which
    would not exist in a real program which loaded it once at the start
    and accessed it in ram many times)

    A simple optimisation is possible to speed up the case where there
    are no fixed letters in the cryptoword at all, but since that is
    the least frequent case, I'm not going to bother doing it here.
    "Left as an exercise ..." for the proverbial reader.

*/


#include <stdio.h>
#include <ctype.h>
#include "../splib.h"

static int debug = FALSE;

char *pattern(char *word)
{
  /* Generate the canonical pattern of a word, eg pattern -> ABCCDEF */
  /* may contain wildcards, eg ABCwag -> ABCDEF (not ABCABC)         */
  static char result[128];
  char mapsto[256];
  char mapping[128];
  char line[128];
  char *s, *mapp;
  int nextmapch;
  int i;

  for (i = 0; i < 256; i++) mapsto[i] = 0;

  nextmapch = 'A'; mapp = mapping;
  s = word;

  for (;;) {
    if (mapsto[*s] != 0) {
      *mapp++ = mapsto[*s];
    } else {
      *mapp++ = nextmapch;
      mapsto[*s] = nextmapch++;
    }
    s += 1;
    if (*s == '\0') break;
  }
  *mapp = '\0';
  sprintf(result, "%s=", mapping);

  return(result);
}

INDEX dawg_locate_prefix(NODE *dawg, char *word, INDEX edge)
{
  /* Finds the sub-trie in the dictionary whose prefix is word */
  for (;;) {
    if (*word == (((dawg[edge] >> V_LETTER) & M_LETTER))) {
      if (*++word == '\0') {
        return(dawg[edge]&M_NODE_POINTER);
      } else {
        if ((edge = (dawg[edge] & M_NODE_POINTER)) == ROOT_NODE) break;
        continue;
      }
    }
    if (((dawg[edge++]) & M_END_OF_NODE) != 0) break;
  }
  /* What to do when none found? -- fail-safe, or some error code...? */
  return(ROOT_NODE);
}

int cryptomatch(
  NODE *dawg, INDEX i,
  char *word, char *res,
  int len, int *found,
  int *map, int *mapsto
)
{
/* Find all words which match this pattern */
int endsword, last, ch, target;
NODE node;
INDEX link;
int newmap[256];
int newmapsto[256];
int n;
  for (n = 0; n < 256; n++) newmap[n] = map[n];
  for (n = 0; n < 256; n++) newmapsto[n] = mapsto[n]; /* the copy may not be needed */
  for (;;) {
    node = dawg[i++];
    ch = (int)((node >> V_LETTER) & M_LETTER);
    last = ((node & (INDEX)M_END_OF_NODE) != 0);
    endsword = ((node & M_END_OF_WORD) != 0);
    link = node & M_NODE_POINTER;

    res[len] = ch; res[len+1] = '\0';
    target = ((int)(*word))&255;
    if (ch != 0) {
    if ((islower(target)) && (ch == target)) { /* lower-case, exact match */
      if (endsword && *(word+1) == '\0') {
        fprintf(stdout, "word: %s\n", res); (*found)++;
      }
      /* no need to update map before recursing */
      if (debug) fprintf(stderr, "A: %s %c %c\n", res, ch, target);
      if (*(word+1) != '\0' && link != 0)
        (void) cryptomatch(dawg, link, word+1, res, len+1, found, newmap, newmapsto);
    } else if (isupper(target)
               && (newmap[target] == 0) && (mapsto[ch] == 0)) {
      /* Unmapped letter, either by us or on input */
      if (endsword && *(word+1) == '\0') {
        fprintf(stdout, "word: %s\n", res); (*found)++;
      }
      /* UC target maps to this LC letter */
      newmap[target] = ch;
      newmapsto[ch] = target; /* Hmmm... if we put the actual char in here ... */
      if (debug) fprintf(stderr, "B: %s %c %c\n", res, ch, target);
      if (*(word+1) != '\0' && link != 0)
        (void) cryptomatch(dawg, link, word+1, res, len+1, found, newmap, newmapsto);
      /* Need to undo newmap afterwards */
      newmap[target] = 0;
      newmapsto[ch] = 0;
    } else if (isupper(target)
               && (newmap[target] == ch)) {
      /* We already know the mapping but it is to the same letter */
      if (endsword && *(word+1) == '\0') {
        fprintf(stdout, "word: %s\n", res); (*found)++;
      }
      /* UC target maps to this LC letter */
      newmap[target] = ch;
      if (debug) fprintf(stderr, "C: %s %c %c\n", res, ch, target);
      if (*(word+1) != '\0' && link != 0)
        (void) cryptomatch(dawg, link, word+1, res, len+1, found, newmap, newmapsto);
      /* Don't need to undo map as it wasn't changed */
    }
    }
    if (last) break;
  }
  return(0==0);
}

int
crypto(NODE *dawg, INDEX root, char *word)
{
char result[MAX_WORD_LEN];
int i = 0;
int map[256];
int mapsto[256];
  for (i = 0; i < 256; i++) {
    mapsto[i] = 0;
    map[i] = 0;
  }
  for (i = 0; i < strlen(word); i++) {
    if (islower(word[i])) mapsto[word[i]] = word[i];
    /* something maps to word[i].  We don't know what it is, but 
       we do know it is NOT one of the upper case letters in this word */
  }
  /* Search in given sub-trie; not necessarily whole dictionary */
  (void)cryptomatch(dawg, root, word, result, 0, &i, map, mapsto);
  return(i);
}

int
main(int argc, char **argv)
{
NODE *dawg;
INDEX root;
INDEX edges;
int each;
char *pat;

  if (argc == 1) {
    fprintf(stderr, "usage: %s 'tQQZ'\n", argv[0]);
    exit(1);
  }
  if (!dawg_init("keyedpatterns.dwg", &dawg, &edges)) exit(2);
  for (each = 1; each < argc; each++) {
    fprintf(stderr, "* Matches:\n");

    /* First we locate the subset of the dictionary which could match */
    root = dawg_locate_prefix(dawg, pat = pattern(argv[each]), ROOT_NODE);
    if (root != ROOT_NODE) {
      /* Then we find the exact matches using any given letters */
      if (!crypto(dawg, root, argv[each])) {
        fprintf(stderr, "(none found)\n");
      }
    } else {
      fprintf(stderr, "(none found)\n");
    }
    if (each+1 != argc) fprintf(stderr, "\n");
  }

  exit(0);
}
