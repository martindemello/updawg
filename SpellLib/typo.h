#define STATUS_CONTINUE 1
#define STATUS_QUIT 0

/* Headers for typo.c */
extern int dawg_wrong(NODE *dawg, char *word, int typos_allowed, int callback (char *res, void *userptr ), void *userptr, int *found);
extern int dawg_insert(NODE *dawg, char *word, int typos_allowed, int callback (char *res, void *userptr ), void *userptr, int *found);
extern int dawg_delete(NODE *dawg, char *word, int typos_allowed, int callback (char *res, void *userptr ), void *userptr, int *found);
extern int dawg_transpose(NODE *dawg, char *word, int typos_allowed, int callback (char *res, void *userptr ), void *userptr, int *found);
extern int dawg_typos(NODE *dawg, char *word, int typos_allowed, int callback (char *res, void *userptr ), void *userptr);
extern int callback(char *res, void *userptr);
/* End of headers for typo.c */
