#ifdef DAWGUTILS_H
#define DAWGUTILS_H

extern void dawg_anagrams(NODE *dawg, register char *rack, int bingosOnly, GList** retwords, GList** retblanks);
extern void dawg_wildcard(NODE *dawg, char *word, GeeMap* retwords);
#endif
