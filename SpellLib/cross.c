/*

    File:          cross.c
    Author:        Graham Toal
    Purpose:       match words with single-char wildcards (cr?ssw?rd p?zzl?s)
    Creation date: 28/06/90 14:01:34
    Lastedit:      28/06/90 14:05:45

    Description:

    SUPERSEDED BY wild.c which allows '*' as well as '?'.

*/


/* Manadatory header files */
#include <stdio.h>
#include "../splib.h"


int fix_cross(
  NODE *dawg, INDEX i,
  char *word, char *res,
  int len, int *found
)
{
int endsword, last, ch, target;
NODE node;
INDEX link;

  for (;;) {
    node = dawg[i++];
    ch = (int)((node >> V_LETTER) & M_LETTER);
    last = ((node & (INDEX)M_END_OF_NODE) != 0);
    endsword = ((node & M_END_OF_WORD) != 0);
    link = node & M_NODE_POINTER;

    res[len] = ch; res[len+1] = '\0';
    target = ((int)*word)&255;
    if (ch != 0) {
    if (ch == target || target == '?') {
      if (endsword && *(word+1) == '\0') {
        fprintf(stdout, "word: %s\n", res); (*found)++;
      }
      if (*(word+1) != '\0' && link != 0)
        (void) fix_cross(dawg, link, word+1, res, len+1, found);
    }
    }
    if (last) break;
  }
  return(0==0);
}

int
crossword(NODE *dawg, char *word)
{
char result[MAX_WORD_LEN];
int i = 0;
  (void)fix_cross(dawg, (INDEX)ROOT_NODE, word, result, 0, &i);
  return(i);
}

int
main(int argc, char **argv)
{
NODE *dawg;
INDEX edges;
int each;


  if (argc == 1) {
    fprintf(stderr, "usage: %s 'p?tt?rn'\n", argv[0]);
    exit(1);
  }
  if (!dawg_init("", &dawg, &edges)) exit(2);
  for (each = 1; each < argc; each++) {
    fprintf(stderr, "* Matches:\n");
    if (!crossword(dawg, argv[each])) {
      fprintf(stderr, "(none found)\n");
    }
    if (each+1 != argc) fprintf(stderr, "\n");
  }

  exit(0);
}
