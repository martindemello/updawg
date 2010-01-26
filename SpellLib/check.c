/*

    File:    check.c
    Author:  Graham Toal
    Purpose: Check a word using DAWG or TRIE.
    Functions exported:  dawg_check, dawg_checkeq, dawg_case, dawg_caseeq

    Description:

    Call as:     dawg_check(dawg, "word");
    The case-insensitive versions are more expensive so the calling program is
    adviced to call the literal-match version first and the case-insensitive
    one only if that fails.

*/

#include "../splib.h"
#include <ctype.h> /* for isalpha, tolower */

/* Simple of check of word in dawg - must be exact match */
int
dawg_check(NODE *dict, char *word)
{
  NODE *edge = dict+ROOT_NODE;

  if (edge == dict) return(0!=0);
  for (;;) {
    if (*word == (((*edge >> V_LETTER) & M_LETTER))) {
      if (*++word == '\0') {
        return((*edge & M_END_OF_WORD) != 0);
      } else {
        if ((edge = dict+(*edge & M_NODE_POINTER)) == dict) break;
        continue;
      }
    }
    if (((*edge++) & M_END_OF_NODE) != 0) break;
  }
  return(0!=0);
}

/* Check of word in dawg allowing case to differ */
/*#define MATCHES(a,b) (((a) == (b)) || (isalpha(a) && isalpha(b) && (tolower(a) == tolower(b))))*/
#define MATCHES(a,b) (mylower(a)==mylower(b))

static int mylower(int a)
{
  if (('A' <= a) && (a <= 'Z')) return(a - 'A' + 'a');
  return(a);
}

/* Don't care what dict version was, just that it exists */
static int
casecheck(NODE *dawg, INDEX i, char *word, int len)
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

    target = ((int)*word)&255;
    if (ch != 0) {
    if (MATCHES(ch, target)) {
      if (endsword && *(word+1) == '\0') {
        return(0==0);
      }
      if (*(word+1) != '\0' && link != 0) {
        if (casecheck(dawg, link, word+1, len+1)) return(0==0);
      }
    }
    }
    if (last) return(0!=0);
  }
  return(0!=0);
}

int
dawg_casecheck(NODE *dawg, char *word)
{
  return(casecheck(dawg, (INDEX)ROOT_NODE, word, 0));
}

/* Simple check, but accepts dicts with "left=right" in them.
   Returns true if "left" is found. */
int
dawg_checkeq(NODE *dict, char *word)
{
  int ch;
  NODE *edge = dict+ROOT_NODE;

  if (edge == dict) return(0!=0);
  for (;;) {
    ch = (int)(((*edge >> V_LETTER) & M_LETTER));
    if (ch == '=') {
      if (*word == '\0') return(0==0);
    }
    if (ch == *word) {
      word += 1;
      if ((edge = dict+(*edge & M_NODE_POINTER)) == dict) break; /* End of dict */
      continue; /* edge moved down a level */
    }
    if (((*edge++) & M_END_OF_NODE) != 0) break; /* End of options */
  }
  return(0!=0);
}

/* As dawg_checkeq, but allows case to differ. */
/* Don't care what dict version was, just that it exists */
static int
casecheckeq(NODE *dawg, INDEX i, char *word, int len)
{
int endsword, last, ch, target;
NODE edge;
INDEX link;

  for (;;) {
    edge = dawg[i++];
    ch = (int)((edge >> V_LETTER) & M_LETTER);
    last = ((edge & (INDEX)M_END_OF_NODE) != 0);
    endsword = ((edge & M_END_OF_WORD) != 0);
    link = edge & M_NODE_POINTER;

    target = ((int)*word)&255;

    if (ch == '=') {
      if (*word == '\0') return(0==0);
    }
    if (ch != 0) {
    if (MATCHES(ch, target)) {
      if (*word == '\0') return(0!=0);
      if (link != 0) {
        if (casecheckeq(dawg, link, word+1, len+1)) return(0==0);
      }
    }
    }
    if (last) return(0!=0);
  }
  return(0!=0);
}

int
dawg_casecheckeq(NODE *dict, char *word)
{
  return(casecheckeq(dict, (INDEX)ROOT_NODE, word, 0));
}
