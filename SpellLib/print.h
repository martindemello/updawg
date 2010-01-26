/* Headers for print.c */
extern void dawg_print(NODE *dawg, INDEX node);
extern int fuzz(NODE *dawg, INDEX i, char *word, char *res, int len, int **Table, int *found);
extern int fuzzy_print(NODE *dawg, char *word, int **Table);
extern void pack_print(NODE *ptrie, INDEX node);
extern void dawg_print_prefix(NODE *dawg, char *prefix, INDEX node);
/* End of headers for print.c */
