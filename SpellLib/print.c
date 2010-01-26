/*

    File:    print.c
    Author:  Graham Toal
    Purpose: Print a packed trie to stderr.
    Functions exported:  dawg_print, pack_print, dawg_print_prefix
    Internal functions:  pack_pr dawg_pr dawg_pr_prefix

Description:
  Pre-order traverse of packed TRIE or DAWG.  Will be modified
  soon to take output file as parameter.  Then sometime after that
  to callback with each string at it is generated.  Meanwhile,
  people requiring custom versions of dawg-walking stuff might
  as well just copy this code and edit it to do what they want.

  The special version print_dawg_prefix takes a NODE from somewhere
  in a dict as a parameter, and prints out only those words which
  contain that node.  You have to locate the node seperately with
  'locate_prefix', and pass the prefix string in so it can be printed.

*/

#include "../splib.h"

#include <stdio.h>

static void
dawg_pr(NODE *dawg, INDEX node, int len)
{
  static char word[MAX_WORD_LEN];
  NODE *edge;

  for (edge = (NODE *)&dawg[node]; ; edge++) {
  long c;
    c = *edge;           /* Don't rewrite this - its avoiding a MSC bug */
    c = c >> V_LETTER;
    c = c & M_LETTER;
    word[len] = (char)c;
    if ((*edge & M_END_OF_WORD) != 0) {
      word[len+1] = '\0';
      fprintf(stdout, "%s\n", word);
    }
    c = *edge & M_NODE_POINTER;
    if ((*edge & M_NODE_POINTER) != 0)
      dawg_pr (dawg, c, len + 1);
    if ((*edge & M_END_OF_NODE) != 0) break; /* End of node */
  }
}

void
dawg_print(NODE *dawg, INDEX node)
{
  dawg_pr(dawg, node, 0);
}




int
fuzz(NODE *dawg, INDEX i, char *word, char *res, int len,
     int **Table, int *found)
{
/*char result[MAX_WORD_LEN];*/
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
    if ((Table[ch][target] != 0) || target == '?') {
      if (endsword && *(word+1) == '\0') {
        fprintf(stdout, "word: %s\n", res); (*found)++;
      }
      if (*(word+1) != '\0' && link != 0)
        (void) fuzz(dawg, link, word+1, res, len+1, Table, found);
    }
    }
    if (last) return(0==0);
  }
  return(0==0);
}

int
fuzzy_print(NODE *dawg, char *word, int **Table)
{
static char result[MAX_WORD_LEN];
int i = 0;
  (void)fuzz(dawg, (INDEX)ROOT_NODE, word, result, 0, Table, &i);
  return(i);
}





static void
pack_pr(NODE *ptrie, INDEX i, int pos)
{
static char s[MAX_WORD_LEN+1];
int b;
INDEX p;
  for (b = BASE_CHAR; b < BASE_CHAR+MAX_CHARS; b++) {
    if (b != 0) {
      p = ptrie[i+b-BASE_CHAR];
      if (((p>>V_LETTER)&M_LETTER) == b) {
      	s[pos] = b; s[pos+1] = '\0';
        if ((p & M_END_OF_WORD) != 0) fprintf(stderr, "%s\n", s);
        if ((p &= M_NODE_POINTER) != 0) {
          pack_pr(ptrie, p, pos+1);
        }
      }
    }
  }
}


void
pack_print(NODE *ptrie, INDEX node)
{
  pack_pr(ptrie, node, 0);
}


static void
dawg_pr_prefix(NODE *dawg, char *prefix, INDEX node, int len)
{
  NODE *edge;
  static char word[MAX_WORD_LEN];
  long c;

  for (edge = (NODE *)&dawg[node]; ; edge++) {
    /* Don't 'fix' - compiler bugaround for microsoft :-( */
    c = *edge; c = c >> V_LETTER; c = c & M_LETTER;
    word[len] = (char)c;
    if ((*edge & M_END_OF_WORD) != 0) {
      word[len+1] = 0;
      fprintf(stdout, "%s%s\n", prefix, word);
    }
    c = *edge & M_NODE_POINTER;
    if (c != 0) dawg_pr_prefix(dawg, prefix, c, len + 1);
    /* End of node - check repair later - I accidentally nobbled it */
    if ((*edge & M_END_OF_NODE) != 0) break;
  }
}

void
dawg_print_prefix(NODE *dawg, char *prefix, INDEX node)
{
  dawg_pr_prefix(dawg, prefix, node, 0);
}
