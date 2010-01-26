/**************************************************************************

    File:          typo.c
    Author:        Graham Toal
    Purpose:       See what we find when we reverse the most common
                   typing errors for an unknown word.

    Creation date: 27/06/90
    Last edit:     16/08/91 15:44:29

    Description:

    This program applies various transformations to each string and
    attempts to find entries in the dawg which match the string when
    transformed.  The transformations made to the word are removing
    characters, inserting characters, transposing characters, and
    replacing characters.

**************************************************************************/

/* Spelling library utilities */

#include "../splib.h"
#include "typo.h"
#include <string.h>             /* For strcpy. May remove this some time. */

/* This one just gets wrong letters */
static int
fix_wrong(
             NODE * dawg,
             INDEX i,
             char *word,
             char *res,
             int len,
             int errs_allowed,
             int *found,
             int callback(char *res, void *userptr),
             void *userptr)
{
    int             endsword, last, ch;
    NODE            node;
    INDEX           link;

    for (;;) {
        node = dawg[i++];
        ch = (int) ((node >> V_LETTER) & M_LETTER);
        last = ((node & (INDEX) M_END_OF_NODE) != 0);
        endsword = ((node & M_END_OF_WORD) != 0);
        link = node & M_NODE_POINTER;

        res[len] = ch;
        res[len + 1] = '\0';

        if (ch != 0) {
            if (ch == ((int) *word) & 255) {
                if (endsword && *(word + 1) == '\0') {
                    (*found) += 1;
                    if (callback(res, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                }
                if (*(word + 1) != '\0' && link != 0) {
                    if (fix_wrong(dawg, link, word + 1, res,
                                  len + 1, errs_allowed, found,
                                  callback, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                }
            } else {
                /* Try a different letter here instead? */
                if (errs_allowed > 0) {
                    if (endsword && *(word + 1) == '\0') {
                        (*found) += 1;
                        if (callback(res, userptr) == STATUS_QUIT) {
                            return (STATUS_QUIT);
                        }
                    }
                    if (*(word + 1) != '\0' && link != 0) {
                        if (fix_wrong(dawg, link, word + 1, res,
                                      len + 1, errs_allowed - 1, found,
                                      callback, userptr) == STATUS_QUIT) {
                            return (STATUS_QUIT);
                        }
                    }
                }
            }
        }
        if (last)
            break;
    }
    return (STATUS_CONTINUE);
}


/* And this one corrects omitted letters by inserting one. */

static int
fix_insert(
              NODE * dawg,
              INDEX i,
              char *word,
              char *res,
              int len,
              int errs_allowed,
              int *found,
              int callback(char *res, void *userptr),
              void *userptr)
{
    int             endsword, last, ch;
    NODE            node;
    INDEX           link;

    for (;;) {
        node = dawg[i++];
        ch = (int) ((node >> V_LETTER) & M_LETTER);
        endsword = ((node & M_END_OF_WORD) != 0);
        last = ((node & M_END_OF_NODE) != 0);

        link = node & M_NODE_POINTER;

        res[len] = ch;
        res[len + 1] = '\0';

        if (ch != 0) {

            if (endsword && *word == '\0' && errs_allowed > 0) {
                if (callback(res, userptr) == STATUS_QUIT) {
                    return (STATUS_QUIT);
                }
            }
            if (ch == ((int) *word) & 255) {
                if (endsword && *(word + 1) == '\0') {
                    if (callback(res, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                }
                if (*word == '\0') {
                    if (errs_allowed > 0) {
                        if (fix_insert(dawg, link, word + 1, res,
                                       len + 1, errs_allowed, found,
                                       callback, userptr) == STATUS_QUIT) {
                            return (STATUS_QUIT);
                        }
                    }
                } else {
                    if (link != 0) {
                        if (fix_insert(dawg, link, word + 1, res,
                                       len + 1, errs_allowed, found,
                                       callback, userptr) == STATUS_QUIT) {
                            return (STATUS_QUIT);
                        }
                    }
                }
            }
            /* Insert this letter (len+1) and see if rest matches */
            if (errs_allowed > 0) {
                if (link != 0) {
                    if (fix_insert(dawg, link, word, res,
                                   len + 1, errs_allowed - 1, found,
                                   callback, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                }
            }
        }
        if (last)
            break;
    }
    return (STATUS_CONTINUE);
}


/* And finally catch inserted letters by deleting one */

static int
fix_delete(
              NODE * dawg,
              INDEX i,
              char *word,
              char *res,
              int len,
              int errs_allowed,
              int *found,
              int callback(char *res, void *userptr),
              void *userptr)
{
    int             endsword, last, ch;
    NODE            node;
    INDEX           link;

    if (errs_allowed > 0) {
        if (*(word + 1) != '\0') {
            if (fix_delete(dawg, i, word + 1, res,
                           len, errs_allowed - 1, found,
                           callback, userptr) == STATUS_QUIT) {
                return (STATUS_QUIT);
            }
        } else {
            /* Shouldn't get this far, but does :-( */
            return (STATUS_CONTINUE);
        }
    }
    for (;;) {
        node = dawg[i++];
        ch = (int) ((node >> V_LETTER) & M_LETTER);
        endsword = ((node & M_END_OF_WORD) != 0);
        last = ((node & M_END_OF_NODE) != 0);

        link = node & M_NODE_POINTER;

        res[len] = ch;
        res[len + 1] = '\0';

        if (ch != 0) {
            if (ch == ((int) *word) & 255) {

                if (errs_allowed > 0 &&
                    endsword &&
                    *(word + 1) != '\0' &&
                    *(word + 2) == '\0') {
                    if (callback(res, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                }
                if (endsword && *(word + 1) == '\0') {
                    if (callback(res, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                    return (STATUS_CONTINUE);
                }
                if (*(word + 1) != '\0' && link != 0) {
                    if (fix_delete(dawg, link, word + 1, res,
                                   len + 1, errs_allowed, found,
                                   callback, userptr) == STATUS_QUIT) {
                        return (STATUS_QUIT);
                    }
                }
            }
        }
        if (last)
            break;
    }
    return (STATUS_CONTINUE);
}

/* Hard to use the dawg to transpose letters, so transpose
   them externally and check if the result is valid. Note that
   the 'word' parameter must be writable (ie not a literal
   string in a source file.)  To enforce this, the word
   is copied in the calling wrapper, 'dawg_transpose'.  This
   neatly sidesteps the problem of whether it is a bad thing
   or not to leave the word scrambled while calling the
   users call-back procedure.
 */
static int
fix_transpose(
                 NODE * dawg,
                 char *word,
                 int first_swap,
                 int typos_allowed,
                 int *found,
                 int callback(char *res, void *userptr),
                 void *userptr)
{
    int             l, c;
    if (word[first_swap] == '\0')
        return (STATUS_CONTINUE);
    for (l = first_swap; word[l + 1] != '\0'; l++) {
        c = word[l];
        word[l] = word[l + 1];
        word[l + 1] = c;
        if (dawg_check(dawg, word)) {
            (*found)++;
            if (callback(word, userptr) == STATUS_QUIT) {
                /* We had better restore the word before returning! */
                c = word[l];
                word[l] = word[l + 1];
                word[l + 1] = c;
                return (STATUS_QUIT);
            }
        }
        if ((word[l + 1] != '\0') && (typos_allowed > 1)) {
            if (fix_transpose(
                                 dawg, word, l + 1, typos_allowed - 1,
                                 found, callback, userptr) == STATUS_QUIT)
                return (STATUS_QUIT);
        }
        c = word[l];
        word[l] = word[l + 1];
        word[l + 1] = c;
    }
    return (STATUS_CONTINUE);
}


/* If MAX_WORD_LEN is too much of a restriction, change the code here to
   allocate 'temp' off the heap.  Remember to allow extra bytes for possible
   insertions of characters. (ie strlen(word)+1+typos_allowed).  Realloc
   temp any time you get a longer word.  (Cheaper than allocating and
   freeing for every word.  I rather object to the overhead of counting
   the length of the string however.  Doubles the cost of a spellcheck.
   Oh well, this stuff was all pretty dear anyway...
 */

int
dawg_wrong(
              NODE * dawg,
              char *word,
              int typos_allowed,
              int callback(char *res, void *userptr),
              void *userptr,
              int *found
)
{
    char            temp[MAX_WORD_LEN];
    return (fix_wrong(dawg, (INDEX) ROOT_NODE, word, temp,
                      0, typos_allowed, found, callback, userptr));
}

int
dawg_insert(
               NODE * dawg,
               char *word,
               int typos_allowed,
               int callback(char *res, void *userptr),
               void *userptr,
               int *found
)
{
    char            temp[MAX_WORD_LEN];
    return (fix_insert(dawg, (INDEX) ROOT_NODE, word, temp,
                       0, typos_allowed, found, callback, userptr));
}

int
dawg_delete(
               NODE * dawg,
               char *word,
               int typos_allowed,
               int callback(char *res, void *userptr),
               void *userptr,
               int *found
)
{
    char            temp[MAX_WORD_LEN];
    return (fix_delete(dawg, (INDEX) ROOT_NODE, word, temp,
                       0, typos_allowed, found, callback, userptr));
}

int
dawg_transpose(
                  NODE * dawg,
                  char *word,
                  int typos_allowed,
                  int callback(char *res, void *userptr),
                  void *userptr,
                  int *found
)
{
    int             i;
    char            temp[MAX_WORD_LEN];
    strcpy(temp, word);
    return (fix_transpose(dawg, temp, 0, typos_allowed, found, callback, userptr));
}

int
dawg_typos(
              NODE * dawg,
              char *word,
              int typos_allowed,
              int callback(char *res, void *userptr),
              void *userptr)
{
    int             found = 0;
    /* 'found' is a bit redundant here, but I wanted it for some debugging
       I was doing once.  Users can keep their own count from within the
       callback procedure if they want it. */
    if (dawg_transpose(dawg, word, typos_allowed, callback, userptr, &found) == STATUS_QUIT)
        return (STATUS_QUIT);
    if (dawg_wrong(dawg, word, typos_allowed, callback, userptr, &found) == STATUS_QUIT)
        return (STATUS_QUIT);
    if (dawg_delete(dawg, word, typos_allowed, callback, userptr, &found) == STATUS_QUIT)
        return (STATUS_QUIT);
    if (dawg_insert(dawg, word, typos_allowed, callback, userptr, &found) == STATUS_QUIT)
        return (STATUS_QUIT);
    return (found);
}

#ifdef TEST
long accented_words[1332] = {
       0x0, 0x44000109, 0x4500010c, 0x50000113, 0x52000118, 0x5300011e, 
0x57000127, 0x6100017a, 0x620001cb, 0x6300025d, 0x640002c8, 0x650002ed, 
0x66000339, 0x6700035c, 0x6800036c, 0x69000374, 0x6a000382, 0x6b000388, 
0x6c000390, 0x6d0003ca, 0x6e0003eb, 0x6f0003f7, 0x70000433, 0x72000474, 
0x730004aa, 0x740004ba, 0x750004c2, 0x760004d3, 0xe940052c,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,        0x0,        0x0, 
       0x0,        0x0,        0x0,        0x0,   0x400000, 0xaec00000, 
0x73400101, 0x63400102, 0x69400103, 0x74400104, 0x65400105, 0x6e400106, 
0x61400107, 0x69400108, 0x79400101, 0x6b40010a, 0x7340010b, 0x73000101, 
0xaec00000, 0x6c40010d, 0x6540010f, 0x74400110, 0x6e400111, 0x65400112, 
0x6f40010d, 0x6c400114, 0x75400115, 0x67400116, 0x65400117, 0x6340010d, 
0x61400119, 0x7240011a, 0x6540011b, 0x7a40011c, 0x6140011d, 0x65000102, 
0xaec00000, 0x6840011f, 0x73400121, 0x65400122, 0x6d400123, 0x64400124, 
0x6c400125, 0x65400126, 0x73c00000, 0xe9c00128, 0x67400129, 0xe940012a, 
0x62000129, 0x7240012b, 0xe9c00000, 0x6c40012e, 0x6240012f, 0x61400130, 
0x6e40012e, 0x72400132, 0x61400133, 0x63000131, 0x68400134, 0x65c00000, 
0xe9c00137, 0x74400138, 0x6e400139, 0x6f40013a, 0x7240013b, 0x6640013c, 
0x74c00137, 0x6e40013e, 0x6140013f, 0xe7400140, 0x73800000, 0x74c00128, 
0x6e400142, 0x65400144, 0x67000129, 0x6d400145, 0xe9400146, 0x61000141, 
0x72400148, 0x72c00128, 0x6140014b, 0x7a40014c, 0xe140014d, 0x65400128, 
0x6300014f, 0x7ac00000, 0x65400150, 0x72400152, 0xe9400153, 0x65c00128, 
0xe9400155, 0x6300014e, 0x66000154, 0x6c400156, 0x6d400129, 0x6940015a, 
0x75c00128, 0xe740015c, 0x7240015d, 0x75400129, 0x7140015f, 0x69400160, 
0x6c400161, 0xe8400128, 0x66c00128, 0x69400164, 0x74400165, 0x69400166, 
0x72400167, 0x6500015e, 0x70000162, 0x72000163, 0xe9400168, 0x74400155, 
0xea40016d, 0x6c400129, 0x6240016f, 0x6d400170, 0x65400171, 0x73400172, 
0x68400129, 0x63400174, 0x61400175, 0x74400176, 0x6f40015f, 0x6e400138, 
0x6200012c, 0x63000135, 0x6600013d, 0x67000149, 0x6c000157, 0x6e00015b, 
0x70000169, 0x7200016e, 0x73000173, 0x74000177, 0x76000178, 0xee400179, 
0x7240012e, 0x72400155, 0xe8400187, 0x64400188, 0x61400189, 0x72000186, 
0x7940018a, 0x72400137, 0xe840018d, 0x6740018e, 0x72400129, 0x72400190, 
0x7200018f, 0x75400191, 0x7340012e, 0x7440018d, 0xe2400195, 0x75400196, 
0x61000194, 0x65400197, 0x6240012e, 0x6f400132, 0x68000129, 0x6c400129, 
0x6e400179, 0x6f40019e, 0x6300019c, 0x7440019f, 0x6d00019a, 0x7400019b, 
0x754001a0, 0x69400188, 0x734001a5, 0x734001a6, 0x7440012e, 0x654001a8, 
0x764001a9, 0x73400129, 0x610001a7, 0x650001aa, 0x690001ab, 0x6f000175, 
0xfb40012f, 0x73400155, 0x734001b1, 0x6cc00128, 0x654001b3, 0x6d4001b4, 
0x614001b5, 0x610001b2, 0x684001b6, 0x6540014b, 0x694001b9, 0x744001ba, 
0x694001bb, 0x694001b1, 0x614001bd, 0x6e4001be, 0x6900014f, 0x73c00000, 
0x6ec001c0, 0x6f4001c2, 0x630001b7, 0x6e0001bc, 0x720001bf, 0x744001c3, 
0x65800128, 0x694001b1, 0x744001c8, 0x6100018b, 0x65000192, 0x6c000198, 
0x6f0001a2, 0x720001ac, 0xe90001c4, 0xea4001ca, 0x70400129, 0x614001d2, 
0x6d400155, 0xe84001d4, 0x75400155, 0x714001d6, 0x61c00128, 0x644001d8, 
0x6ec00128, 0x610001d9, 0x6f4001da, 0x66000129, 0x6e0001d3, 0x720001d5, 
0xef0001d7, 0xf14001db, 0x64400186, 0x6e4001e2, 0x76400129, 0x654001e4, 
0x6c4001e5, 0x62000186, 0x704001e6, 0x64c00000, 0x67c00000, 0x6e4001ea, 
0x650001e9, 0x690001eb, 0x73c00000, 0xe9c001ec, 0x734001ef, 0x6d0001e7, 
0x734001f0, 0x78c00000, 0x75c001f3, 0x694001da, 0x614001f5, 0x610001f4, 
0x6c4001f6, 0x654001f7, 0x744001f9, 0x694001d8, 0x684001fb, 0x634001fc, 
0x610001f1, 0x69000132, 0xe20001fa, 0xe94001fd, 0x7340016d, 0x61400202, 
0xe9400203, 0x6e000204, 0x72400129, 0x6840012e, 0x63400207, 0x64800000, 
0x73c00000, 0xe9c00209, 0x6840020b, 0x6c400155, 0xe840020d, 0x7440020e, 
0x6e40020f, 0x6300020c, 0x65400210, 0x65000208, 0x69000211, 0x6f400160, 
0x6d000188, 0x7040019b, 0x6e4001a8, 0x69400155, 0x72400219, 0x6f000218, 
0xe8000187, 0xe940021a, 0x7240021b, 0x64800000, 0x690001eb, 0x73c00000, 
0xe9c0021f, 0x6d40015a, 0x6f400223, 0x6600021e, 0x67000222, 0x73400224, 
0x67400155, 0xe8400228, 0x74000229, 0x76400156, 0x65800128, 0x73c00000, 
0xe9c0022c, 0x6840022e, 0x6300022f, 0x6c000156, 0x70400129, 0x6d000216, 
0x6e000225, 0x7200022a, 0x75400230, 0x65800128, 0x6f4001da, 0x74400237, 
0xfb400239, 0x68400155, 0x6300023b, 0x6d400155, 0x65800209, 0x690001eb, 
0x79c00000, 0x6d000155, 0x7040023e, 0x69000130, 0x6f00023a, 0xe800023c, 
0xea400241, 0x73c0014f, 0xe1400247, 0x64400248, 0x72400249, 0xe140024a, 
0x6fc00128, 0x6100024c, 0x6f4001d8, 0xe740024d, 0x6100024f, 0xe9c00128, 
0x72000250, 0x76400156, 0x624001d8, 0x6f400254, 0x64400255, 0x72400256, 
0x7440016d, 0x65400258, 0x6c400259, 0x6540025a, 0x7440025b, 0x610001dd, 
0x650001e3, 0x680001fe, 0x69000205, 0x6c000213, 0x6f000233, 0x72000243, 
0x7300024b, 0x75000252, 0xf3000257, 0xf440025c, 0x744001a8, 0x65400268, 
0x63400269, 0x6e40026a, 0x724001a5, 0x7240026c, 0x74400129, 0x6e40026e, 
0x6140026f, 0x6d400270, 0x7540012e, 0x67400272, 0x6e400273, 0x69400274, 
0x74400275, 0x64400187, 0x61000271, 0x73000276, 0xe8400277, 0x6e40022e, 
0x6e40027b, 0x6e400132, 0x6f00027d, 0xe9400155, 0x6740027e, 0x74c00000, 
0x72800000, 0x73c00281, 0x65c00282, 0x6c400284, 0x61000280, 0xf4400285, 
0x75400175, 0x74c00128, 0x6e400289, 0x6100028a, 0x73c00000, 0x74c0028b, 
0x6340020d, 0x6f000288, 0x7500028d, 0xe240028e, 0x63400155, 0x6e400292, 
0x61400293, 0xe9400294, 0x73400138, 0x73400296, 0x61400297, 0x6c4001a9, 
0x6c000299, 0x72c00128, 0x68000295, 0x6c000298, 0x6f40029a, 0x6740012e, 
0xfb400289, 0x6100029f, 0x6f4002a0, 0x68400137, 0x634002a3, 0x724002a4, 
0x69c00128, 0x744002a6, 0x6e4002a7, 0x6440012e, 0x610002a5, 0x650002a8, 
0x6f4002a9, 0x6340023b, 0xea4002ad, 0x69400132, 0x634002af, 0x614002b0, 
0x76400186, 0x754002b2, 0x654002b3, 0x6f4002b4, 0x74000155, 0x75c0022c, 
0x6e4002b6, 0x7540022e, 0x714002b9, 0x614002ba, 0x650002b8, 0x724002bb, 
0x6f400289, 0x6200028f, 0x6300029c, 0x670002a1, 0x6d0002aa, 0x700002ae, 
0x720002b1, 0x730002b5, 0x740002bc, 0x764002be, 0x6100026b, 0x6500026d, 
0x69000278, 0x6f00027c, 0x72000286, 0xe94002bf, 0x73400160, 0x754002ce, 
0x624002cf, 0x6540028a, 0x6d4002d1, 0xfb4002d2, 0x6100029f, 0x6f4002d3, 
0x7640012e, 0x654002d6, 0x7940012e, 0x754002d8, 0x6140029f, 0xf440016d, 
0x634002db, 0x650002dc, 0xe9400155, 0x720002dd, 0xea400139, 0x670002d4, 
0x6c0002d7, 0x6e0002d9, 0x720002da, 0x744002df, 0x6c400137, 0x674002e6, 
0xe84002e7, 0x694002e8, 0x704002e9, 0x6f4001ab, 0x704002eb, 0x6d0002d0, 
0x6e0002e1, 0x730002ea, 0x784002ec, 0x65800128, 0x6900014f, 0x79c00000, 
0x630002f1, 0x74c00128, 0x6e4002f4, 0x614002f6, 0xe94002f7, 0x6e4002f8, 
0x64400155, 0x6e400129, 0x6e4002fb, 0x610002fa, 0x6f4002fc, 0x65400293, 
0x690002f9, 0xe70002fd, 0xef4002ff, 0x6ec00000, 0xee400303, 0x68400304, 
0x61400305, 0x6c400306, 0x6540026e, 0x6c400308, 0x6c400309, 0x6940030a, 
0x6c000307, 0x7540030b, 0x6340022e, 0x6e40030e, 0x68400138, 0x63400310, 
0x6100030f, 0x74400311, 0x654001e9, 0xe9c00314, 0x62400315, 0x6d400316, 
0x72000219, 0x7540014b, 0x65400318, 0x6e40031a, 0x68400259, 0x6340031c, 
0x61000317, 0xe200031b, 0xe80002ad, 0xe940031d, 0x61400289, 0xe7400322, 
0x7440026e, 0x65000324, 0x6c400129, 0x72000323, 0x75400325, 0x70400138, 
0x7540014b, 0x6540032a, 0x6840032b, 0x6340032c, 0x70000329, 0xee40032d, 
0x654001f5, 0x6c400330, 0x75400331, 0x6100032e, 0xe4400332, 0x65c0021a, 
0x65800209, 0x694001eb, 0x74400336, 0x61000300, 0x6500030c, 0x69000312, 
0x6c00031e, 0x6f000327, 0x72000333, 0xe9000335, 0xea400338, 0xf3c00128, 
0x6c000188, 0x70400341, 0x68c00000, 0x63400344, 0x69400345, 0x6c400346, 
0x74400347, 0xfc400348, 0x6d400349, 0x634001ef, 0x77400330, 0x6840034c, 
0x6100034b, 0xfc40034d, 0x67400188, 0x75400350, 0x67400137, 0xe8400352, 
0x6f4001da, 0x64400354, 0x69400355, 0x72400356, 0xe9400357, 0x6940016d, 
0x68400359, 0x7440035a, 0x61000342, 0x6500034a, 0x6c00034e, 0x6f000351, 
0x72000353, 0x75000358, 0xf640035b, 0x7440015f, 0x69400363, 0x6e400155, 
0x79400365, 0x62000364, 0xfc400366, 0x73400194, 0x69400369, 0x7240036a, 
0x61000367, 0xe940036b, 0x724001d3, 0x6140036e, 0x75c0022c, 0x6e400370, 
0xe9400371, 0x67400372, 0x64000156, 0x6700036f, 0x6e400373, 0x7040012e, 
0x6e4001d8, 0x61400378, 0x73000377, 0xe7400379, 0x674001b9, 0x75400128, 
0x7340037d, 0x6e4001da, 0x7540037f, 0x74400380, 0x6100037a, 0x6500026e, 
0xe400037c, 0xe900037e, 0xf6400381, 0x6d4001b5, 0xfc400387, 0xe7400354, 
0x61400389, 0x6d40038a, 0x63400156, 0x6c4001b9, 0x6440038d, 0x6e40038e, 
0x6900038b, 0x7900038c, 0xe440038f, 0x6140015a, 0x69400365, 0x6f400394, 
0x64400395, 0x72000393, 0xe9400396, 0x61800128, 0x6fc00128, 0xf1400399, 
0x6f40039b, 0x7240039c, 0x67400336, 0x71000272, 0xe840039e, 0x734001ab, 
0x614003a1, 0x6c4003a2, 0x6e400156, 0x694001b4, 0x720003a5, 0x73c00000, 
0x650003a3, 0x690003a4, 0xe9c003a6, 0x63000397, 0x6400039d, 0x6e00039f, 
0x730001ab, 0x744003a8, 0x6940018e, 0x6e4003b0, 0x754003b1, 0x73400188, 
0x7340015a, 0x69000190, 0x72000132, 0x754003b4, 0x72c0014f, 0x614003b8, 
0x6a4003b9, 0xe94003ba, 0x644003bb, 0x61400228, 0x61000228, 0x6540014b, 
0x6900021a, 0x794003be, 0x6500014b, 0x73c001b1, 0x610003c0, 0x690003c2, 
0x7240024c, 0x6e0003bd, 0x740003c4, 0x7a400129, 0x610003ab, 0x650003b2, 
0x690003b3, 0x6f0003b5, 0x750003bc, 0xe94003c7, 0x72800000, 0x73400281, 
0x654003d0, 0x79c00000, 0x6900014f, 0x79800000, 0xe9c00128, 0x6c0003d3, 
0x72800000, 0x73000281, 0x744003d4, 0x65c003d7, 0x668003d2, 0x764003db, 
0xef4003dc, 0x7540012f, 0x624003df, 0xeb4001b3, 0x6940012a, 0x6140028a, 
0x694003e3, 0x634003e4, 0x6c0003e2, 0x6f4003e5, 0x65800000, 0x670003e6, 
0x76400129, 0x610003de, 0x650003e0, 0x6f0003e1, 0xe94003e8, 0x7200012e, 
0xfac00128, 0xe0c00128, 0x744003f1, 0x724003f2, 0x620003ef, 0x654003f3, 
0x7640026c, 0x6c00012e, 0x6d0003f4, 0x754003f6, 0x73000296, 0x74400139, 
0x73400156, 0x6e4003fc, 0x6f4001a8, 0x61800128, 0x6f4001da, 0x630003fe, 
0x650003f2, 0x73000129, 0xf10003ff, 0xf9c00000, 0x73000194, 0xe9c00128, 
0x69400406, 0x6c40012f, 0x69400409, 0x7440040a, 0x6e40040b, 0x744001a5, 
0x63400129, 0x6e40040e, 0x6540040f, 0x6900040c, 0x7200040d, 0x7300012e, 
0x74400410, 0x72400336, 0xe8400415, 0x69400416, 0x6d400417, 0x6740022e, 
0xe9400419, 0x7440041a, 0x754001b1, 0x65000209, 0x694001eb, 0x6500041c, 
0x73c0041d, 0x6940041f, 0x63400421, 0x65000418, 0x6f00041b, 0xe9400422, 
0x65c0021f, 0xe9400426, 0x72400427, 0x72400394, 0x6e4001d7, 0x6e400281, 
0x6140042b, 0x6c40042c, 0x6c40042d, 0x6100042a, 0x6940042e, 0x62000429, 
0x7440042f, 0x610003fa, 0x650003fd, 0x69000401, 0x6c000408, 0x6f000411, 
0x72000423, 0x75000428, 0xe200026e, 0xe8000187, 0xe9400431, 0x6f40027d, 
0x7340043d, 0x6940043e, 0x72400208, 0x65400440, 0x68400441, 0xf3400303, 
0xe24002ad, 0x69400129, 0x6e000445, 0x72400160, 0x61400446, 0x754003a1, 
0x6f400449, 0x63000442, 0x6a000443, 0x6c000444, 0x6d000448, 0x7040044a, 
0x71400272, 0x73000450, 0x764001a5, 0x73000129, 0x75400129, 0x66400129, 
0x66400455, 0x75400456, 0x61400457, 0x65400289, 0x6c400459, 0x6c40045a, 
0x68000458, 0x69000289, 0x6f40045b, 0x65800128, 0x6d400155, 0x6940045f, 
0x73800000, 0x78c00000, 0x75c00462, 0x61400464, 0x65000465, 0x7540015a, 
0x6300045c, 0x67000461, 0x73400466, 0x72800128, 0x73400155, 0x7540046b, 
0x6540046d, 0x7640046e, 0x654001da, 0x67400470, 0x74400471, 0x6e400472, 
0x6100043f, 0x6500044b, 0x69000451, 0x6f000453, 0x75000194, 0xe9000468, 
0xea00046f, 0xf400020d, 0xf6400473, 0x64800000, 0x694001eb, 0x64800000, 
0x6500047d, 0x73c00000, 0xe9c0047f, 0x74400482, 0x75400483, 0xe440037c, 
0x6c400485, 0x68400486, 0x6d400138, 0x67000179, 0x72400156, 0x6640016f, 
0x6600048b, 0x70400389, 0x69000489, 0x7540048c, 0x6e800000, 0x73c00000, 
0x65c00490, 0x73400492, 0x65400493, 0x6c400494, 0x74400495, 0xe4400496, 
0x6140038b, 0x72400498, 0xfac00128, 0x6a40049a, 0x7540049b, 0x7240049c, 
0x63000163, 0x7540049d, 0xea4001d4, 0x724004a0, 0x64400336, 0x6300049e, 
0x700004a1, 0xe84004a2, 0x63c00128, 0x614004a6, 0x61000293, 0x724004a7, 
0x61000484, 0x63000487, 0x65000488, 0x6f00048e, 0x70000497, 0x74000499, 
0x750004a3, 0xe94004a8, 0x6340012e, 0x724004b2, 0x62400341, 0x650004b3, 
0x6d4004b4, 0x6d400219, 0xe94004b7, 0x79400188, 0x650002fc, 0x690004b5, 
0x720004b8, 0x750004b9, 0xea40016d, 0xe9400137, 0x644004bf, 0x64400138, 
0x6e0004c0, 0x724004c1, 0xf140024c, 0x654004c4, 0x754004c5, 0x674004c6, 
0x69000186, 0x724004c7, 0xe0c00000, 0x6c4004ca, 0x694004cb, 0x75400137, 
0x714004cd, 0x694004ce, 0x6e4004cf, 0x6f4004d0, 0x724004d1, 0x610004c8, 
0x690001f0, 0x6f0004cc, 0xe94004d2, 0x754002ad, 0x69400202, 0x6e4004d8, 
0x610004d7, 0xe94004d9, 0x7240026e, 0x704001d2, 0x614004dd, 0x6900014b, 
0x74c00128, 0x614004df, 0x73400137, 0x694004e2, 0x614004e3, 0x734004e4, 
0x72000175, 0x734004e5, 0x7340032b, 0x69400187, 0x6f4004e9, 0x744004ea, 
0x610004e8, 0x694004eb, 0x6c40020d, 0x650004ee, 0x72400219, 0x610004dc, 
0x680004de, 0x6c0004e1, 0x6f0004e6, 0x720004ec, 0x754004ef, 0x610001da, 
0x6f400228, 0x7540016d, 0x654004f9, 0x75c00137, 0x644004fb, 0x724004fc, 
0x634001ba, 0x73c00137, 0x694004ff, 0x69400296, 0x650004fd, 0x690004fe, 
0x72000500, 0x75000501, 0xe9400155, 0x70400155, 0x69400507, 0x75400508, 
0x65800128, 0xe8400187, 0x6d000155, 0x74400155, 0x7340050c, 0x6900050e, 
0x73c00000, 0x6700050a, 0x6c0003bd, 0x70000155, 0x74c0050f, 0x69c00137, 
0x64400515, 0x72400516, 0x6900020d, 0x75400517, 0x6500014b, 0xe8400187, 
0x6740051a, 0x6e40051c, 0x6e400137, 0x6e40051e, 0x6100051d, 0x6500051f, 
0x694001b9, 0x61000511, 0x6f000518, 0x72000520, 0x754002fa, 0x6c40015f, 
0x654002d2, 0x6e400528, 0x6f000527, 0xe9400529, 0x620004da, 0x630004f1, 
0x6c0004f7, 0x6d0004fa, 0x70000502, 0x71000509, 0x74000523, 0x7640052a
};

int callback(char *res, void *userptr)
{
  printf("%s->%s\n", (char *)userptr, res);
  return(STATUS_CONTINUE);
}

/* Bare minimum confidence test. */
int
main(int argc, char **argv)
{
  printf("One typo: shouldn't find anything\n");
  dawg_typos(accented_words, "epee", 1, callback, "epee");
  printf("Two typos: should find something\n");
  dawg_typos(accented_words, "epee", 2, callback, "epee");
  printf("Done.\n");
  return(EXIT_SUCCESS);
}

#endif /* TEST */
