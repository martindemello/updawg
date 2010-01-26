/*

    File:    prefix.c
    Author:  Graham Toal
    Purpose: Check the stem of a word using DAWG or TRIE.
    Functions exported:  dawg_prefix

    Description:  returns TRUE if the argument is found to be
                  a initial stem of a word in the dawg (or a proper
                  word in its own right)

    Call as:     dawg_prefix(dawg, "stem");

    Examples:
                 dawg_prefix(dawg, "argu") => TRUE (argue, argument)
                 dawg_prefix(dawg, "word") => TRUE (word)
                 dawg_prefix(dawg, "zxcv") => FALSE

    Notes: used in my boggle program.  Sometimes (as in wordsquare program)
           it's more effective to do prefix checking incrementally and avoid
           the (N^2)/2 complexity of repeated tests against a word which
           is increasing by one letter at a time.  However this is often
           useful as a quick prototype which you optimise later.

*/

#include "../splib.h"
#include <ctype.h> /* for isalpha, tolower */

int
dawg_prefix(NODE *dict, char *word)
{
  NODE *edge = dict+ROOT_NODE;

  if (edge == dict) return(0!=0);
  for (;;) {
    if (*word == (((*edge >> V_LETTER) & M_LETTER))) {
      if (*++word == '\0') {
        return(0==0);
      } else {
        if ((edge = dict+(*edge & M_NODE_POINTER)) == dict) break;
        continue;
      }
    }
    if (((*edge++) & M_END_OF_NODE) != 0) break;
  }
  return(0!=0);
}
