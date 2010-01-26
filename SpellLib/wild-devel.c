/*
    File:          wild-devel.c
    Author:        Graham Toal
    Purpose:       match words with "(...|...|)", "*", "?", "[aeiou]"
    Creation date: 10/27/00 17:30:34
    Lastedit:      10/27/00 20:35:41

    Description:

WORK IN PROGRESS.  NOT EXHAUSTIVELY TESTED AND CERTAINLY NOT PROVEN CORRECT.
However it might work already, to some extent.

This code allows more complex searches than that in wild.c,
such as "?r(a(ha|ae)m|?m|i*[mn])"

To do: add [~...] maybe.  Can't be bothered right now.


There are two ways I could have written this code:

1) Steal the regexp code, build a data structure defining the search
   expression, and use that data structure to navigate through the dawg

or

2) parse the string on the fly, and wing it, with lots of hacks.

Want to guess which one we do here? :-)  yep, the one that rather assumes
your expressions are all well-written (and if not, it's your own fault).

Assumption: not searching for any of the characters "(|)[]", and
[...] sets do not contain "(|)".  Note no use of "\" supported.

            Look up words containing '?' (single letter wild card)
            or '*' (zero or more letters wild card).  Allows leading
            or trailing '*'s.  Any number of wildcards may be used
            but you're asking for trouble if you enter consecutive '*'s
            (such as 'gr**ham')


    Bugs:   May generate same word on output more than once - you may
            want to pipe it through either "sort -u" or "uniq"...
            Should pass word to a user call-back routine rather than print.
            Should make into library module, not main program.
            One of my other programs which adds the result words to a
            dynamic trie and returns that, which has the side-effect of
            both sorting the words *and* removing duplicates.

    Examples:  wild 'gr?h?m'
               wild 'graham*'
               wild 'gr*ham'
               wild '*ham'
               wild 'gra??*'

    See also:  ../dawg_words (JJ Chew's anagram generator)

*/

#include <stdio.h>
#include <string.h>
#include "../splib.h"

int do_wild(
  NODE *dawg,
  INDEX i, char *word, char *res, int len, int *found, int lastwasword) {
  int endsword, last, ch, target;
  NODE node;
  INDEX link;
  INDEX origi;

  origi = i;

  target = ((int)*word)&255; /* care taken for signed ISO alphabets */

  if (*word == '*') {
    res[len] = '\0'; 
    (void) do_wild(dawg, origi, word+1 /* skip '*', match nothing */,
                     res, len, found, FALSE);
  } else if (*word == '(') {
    /* "(... | ... | ...)" */
    /* Identify each alternative sequence, append the rest of
       the string to it, and recursively enumerate it */
    int level = 1;
    int pats = 0;
    int i;
#define MAX_PATTS 10 /* For testing */
    char *nextpatstart[MAX_PATTS]; /* Inclusive */
    char *nextpatend[MAX_PATTS];   /* Exclusive */
    char *newsearch = malloc(strlen(word));
    nextpatstart[pats] = ++word; /* Points to ch after '(' */
    for (;;) {
      ch = *word;
      if (ch == '(') level += 1;
      if ((ch == '|') && (level == 1)) {
        /* Store an alternative */
        nextpatend[pats] = word; /* End is exclusive */
        pats += 1;
        nextpatstart[pats] = ++word;
        continue;
      }
      if (ch == ')') level -= 1;
      if ((ch == ')') && (level == 0)) {
        nextpatend[pats] = word++;
        break;
      }
      word += 1;
    }
    /* Now recurse using all the patterns, plus the remaining text */
    res[len] = '\0'; /* Undo advance */
    for (i = 0; i <= pats; i++) {
      strncpy(newsearch, nextpatstart[i], nextpatend[i]-nextpatstart[i]);
      newsearch[nextpatend[i]-nextpatstart[i]] = '\0';
      strcat(newsearch, word);
      res[len] = '\0'; /* Undo advance */
      /* fprintf(stderr, "recurse %d: %s+%s\n", i, res, newsearch);*/
      if ((*newsearch == '\0') && lastwasword) {
        fprintf(stdout, "word: %s\n", res); (*found)++;
      } else {
        (void) do_wild(dawg, origi, newsearch, res, len, found, lastwasword);
      }
    }
    free(newsearch);
    return;
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
          fprintf(stdout, "word: %s\n", res); (*found)++;
        } else if ((endsword && *(word+1) == '*') && (*(word+2) == '\0')) {
          /* special-case hack for trailing * */
          fprintf(stdout, "word: %s\n", res); (*found)++;
        }
        if (*(word+1) != '\0' && link != 0)
          (void) do_wild(dawg, link, word+1, res, len+1, found, endsword);
      } else if (target == '[') { /* Is this letter in our set of valid letters? */
        char choices[8000];
        char *s, *saved = word;
        strcpy(choices, word);
        s = strchr(choices, ']'); /* We assume well-formed expressions */
        if (s == NULL) {
          fprintf(stderr, "malformed regex: \"[%s\"\n", choices); return;
	}
        *s = '\0';
        word = strchr(word, ']');
        if (strchr(choices, ch) != NULL) {
          /* single [letterset] 1 letter match */
          if (endsword && *(word+1) == '\0') {
            fprintf(stdout, "word: %s\n", res); (*found)++;
          } else if ((endsword && *(word+1) == '*') && (*(word+2) == '\0')) {
            /* special-case hack for trailing * */
            fprintf(stdout, "word: %s\n", res); (*found)++;
          }
          if (*(word+1) != '\0' && link != 0)
            (void) do_wild(dawg, link, word+1, res, len+1, found, endsword);
	}
        word = saved; /* restore and try other letters at this position */
      } else if (target == '*') {
        /* multiple wildcard - 0-N letters match */
        if (endsword && *(word+1) == '\0') {
          /* trailing* */
          fprintf(stdout, "word: %s\n", res); (*found)++;
        }
        if (link != 0) { /* link == 0 means this letter terminates here */
          /* skip the * and see what we get if it has matched to here: */
          (void) do_wild(dawg, link, word+1 /* skip '*' */, res, len+1, found, endsword);
          /* let this letter match the * and see
             if the rest of the pattern matches: */
          (void) do_wild(dawg, link, word /* keep '*' */, res, len+1, found, endsword);
	}
      }
    }
    if (last) break;
  }

  return(0==0);
}

int wildcard(NODE *dawg, char *word) {
  char result[MAX_WORD_LEN];
  int i = 0;

  (void)do_wild(dawg, (INDEX)ROOT_NODE, word, result, 0, &i, FALSE);
  return(i);
}

int main(int argc, char **argv) {
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
    if (!wildcard(dawg, argv[each])) {
      fprintf(stderr, "(none found)\n");
    }
    if (each+1 != argc) fprintf(stderr, "\n");
  }

  exit(0);
}
