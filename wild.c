/*

    File:          wild.c
    Author:        Graham Toal
    Purpose:       match words with multi-char wildcards (cr*word *zzl?s)
    Creation date: 09/06/99 21:57:12
    Lastedit:      09/06/99 22:24:59

    Description:

            Look up words containing '?' (single letter wild card)
            or '*' (zero or more letters wild card).  Allows leading
            or trailing '*'s.  Any number of wildcards may be used
            but you're asking for trouble if you enter consecutive '*'s
            (such as 'gr**ham')

    Bugs:   May generate same word on output more than once - you may
            want to pipe it through either "sort -u" or "uniq"...
            Should pass word to a user call-back routine rather than print.
            Should make into library module, not main program.

    Examples:  wild 'gr?h?m'
               wild 'graham*'
               wild 'gr*ham'
               wild '*ham'
               wild 'gra??*'

    See also:  ../dawg_words (JJ Chew's anagram generator)

*/

#include <stdio.h>
#include <unistd.h>
#include <gee.h>
#include <string.h>

#include "splib.h"

void do_wild(
  NODE *dawg, INDEX i, char *word, char *res, char *blanks, int len, GeeMap* retwords) {

  int endsword, last, ch, target;
  NODE node;
  INDEX link;
  INDEX origi;

  origi = i;

  target = ((int)*word)&255; /* care taken for signed ISO alphabets */

  if (*word == '*') {
    res[len] = '\0'; 
    (void) do_wild(dawg, origi, word+1 /* skip '*', match nothing */,
                     res, blanks, len, retwords);
  }

  for (;;) {
    node = dawg[i++];
    ch = (int)((node >> V_LETTER) & M_LETTER);
    last = ((node & (INDEX)M_END_OF_NODE) != 0);
    endsword = ((node & M_END_OF_WORD) != 0);
    link = node & M_NODE_POINTER;

    res[len] = ch; res[len+1] = '\0';

    if (ch != 0) {
      if (ch == target || target == '?') {
        /* single wildcard 1 letter match */
        if (endsword && *(word+1) == '\0') {
          gee_map_set(GEE_MAP (retwords), g_strndup(res, len+1), g_memdup(blanks, len+1));
        } else if ((endsword && *(word+1) == '*') && (*(word+2) == '\0')) {
          /* special-case hack for trailing * */
          gee_map_set(GEE_MAP (retwords), g_strndup(res, len+1), g_memdup(blanks, len+1));
        }
        if (*(word+1) != '\0' && link != 0)
          (void) do_wild(dawg, link, word+1, res, blanks, len+1, retwords);
      } else if (target == '*') {
        /* multiple wildcard - 0-N letters match */
        if (endsword && *(word+1) == '\0') {
          /* trailing* */
          gee_map_set(GEE_MAP (retwords), g_strndup(res, len+1), g_memdup(blanks, len+1));
        }
        if (link != 0) { /* link == 0 means this letter terminates here */
          /* skip the * and see what we get if it has matched to here: */
          (void) do_wild(dawg, link, word+1 /* skip '*' */, res, blanks, len+1, retwords);
          /* let this letter match the * and see
             if the rest of the pattern matches: */
          (void) do_wild(dawg, link, word /* keep '*' */, res, blanks, len+1, retwords);
        }
      }
    }
    if (last) break;
  }

  return;
}

void dawg_wildcard(NODE *dawg, char *word, GeeMap* retwords) {
  char result[MAX_WORD_LEN];
  char blanks[MAX_WORD_LEN];

  (void)do_wild(dawg, (INDEX)ROOT_NODE, word, result, blanks, 0, retwords);
  return;
}

#if 0
int main(int argc, char **argv) {
  NODE *dawg;
  INDEX edges;
  int each;

  if (argc == 1) {
    fprintf(stderr, "usage: %s 'p?tt?rn'\n", argv[0]);
    exit(1);
  }
  if (!dawg_init("csw.dwg", &dawg, &edges)) exit(2);
  for (each = 1; each < argc; each++) {
    if (!wildcard(dawg, argv[each])) {
      fprintf(stderr, "(none found)\n");
    }
    if (each+1 != argc) fprintf(stderr, "\n");
  }

  exit(0);
}
#endif
