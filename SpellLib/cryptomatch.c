/*

    File:          cryptomatch.c
    Author:        Graham Toal
    Purpose:       find words which match the letter pattern of a cryptogram
    Creation date: 22/09/00 21:43:02
    Lastedit:      22/09/00 23:00:56

    Description:

    A utility to be used in solving cryptograms.  Given an encrypted
    word, it finds all the words in a dictionary which can match the
    pattern of letters in that word.  The convention is that all the
    dictionary words are in lower-case, and that the input word is
    in upper case, except for any already-decrypted letters, which
    are in lower-case.  (A mixed-case dictionary *may* cause a 
    problem - I haven't verified the code for that case)

    For example XQQB could match "tool" or "peek" etc, but tQQB could
    only match "tool" etc - not "peek".

    Similarly, but not identically, "toQB" should *not* be able to
    match "tool" because something already maps to "o" even if we don't
    know what exactly.  Also ABCABX -> tumtum is also wrong - C and X
    can't both map to same letter.  mapsto[] is used to ensure that
    neither of these cases can happen.

    A slightly better interface would be to pass in a decoding
    matrix filled with all the known letters (not just the ones
    in this word) which would eliminate more dead ends.  I may add
    this after I get the basic interface working.  This code could
    also update the matrix with new associations as a help to the
    higher-level code.  For the moment, this code works fine.

   Example:

% ./cryptomatch ABCABX
* Matches:
word: acracy
word: alkali
word: anhang
word: bamban
word: barbal
word: barbas
word: bilbie
word: bimbil
word: bowboy
word: caecal
word: calcar
word: daedal
word: deaden
word: deader
word: dogdom
word: ferfet
word: glegly
word: hashab
word: inring
word: intine
word: inwind
word: kunkur
word: leglen
word: leglet
word: madman
word: mahmal
word: mormon
word: oclock
word: oniony
word: osmose
word: ouroub
word: pampas
word: parpal
word: pompon
word: rebrew
word: recrew
word: regret
word: sensed
word: taotai
word: tartan
word: teated
word: tented
word: tenter
word: tested
word: tester
word: tintie
word: toston
word: unbung
word: unhung
word: unrung
word: unsung
word: unsunk
word: untune
word: velvet
word: vervel
word: vervet

*/


#include <stdio.h>
#include <ctype.h>
#include "../splib.h"

static int debug = FALSE;

int cryptomatch(
  NODE *dawg, INDEX i,
  char *word, char *res,
  int len, int *found,
  int *map, int *mapsto
)
{
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
crypto(NODE *dawg, char *word)
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
  (void)cryptomatch(dawg, (INDEX)ROOT_NODE, word, result, 0, &i, map, mapsto);
  return(i);
}

int
main(int argc, char **argv)
{
NODE *dawg;
INDEX edges;
int each;


  if (argc == 1) {
    fprintf(stderr, "usage: %s 'tQQZ'\n", argv[0]);
    exit(1);
  }
  if (!dawg_init("", &dawg, &edges)) exit(2);
  for (each = 1; each < argc; each++) {
    fprintf(stderr, "* Matches:\n");
    if (!crypto(dawg, argv[each])) {
      fprintf(stderr, "(none found)\n");
    }
    if (each+1 != argc) fprintf(stderr, "\n");
  }

  exit(0);
}
