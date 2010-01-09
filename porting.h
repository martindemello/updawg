/*

    File:    utils.c
    Author:  Graham Toal
    Purpose: Portability library

Description:

    This is a cut-down version of my portability header-file.
    We cater here only for Unix and Risc OS (the latter being
    my development environment).

*/

/* Temporary while developing at vangogh.berkeley.edu: */
#define DEFAULT_DAWG "web2"
#define EXT_CHAR '.'

#define READ_TEXT "r"
#define WRITE_TEXT "w"
#define WRITE_BIN "wb"
#define READ_BIN "rb"


/* system configuration */

#include <stdlib.h>  /* for malloc, free & exit */
#define RANGECHECK(idx,max) (idx)
