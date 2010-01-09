#ifndef grope_h
#define grope_h 1

#ifdef TESTING_DEFS
/*###################################################################*/
/*
 * This is an exercise to assist in writing portable programs.
 *
 * To use as a header file, eg "grope.h", just leave this file
 * as it is.  To use it to define new entries, rename it as
 * "grope.c" and compile it as "cc -DTESTING_DEFS grope".
 *
 * To customise this test program for your system, I have set it up
 * so that all features are enabled.  If you find that one feature
 * causes a compile-time error, test a suitable set of '#ifdef's for
 * your machine and #undef the values below which are not appropriate.
 *
 * Please do your best to see that your tests are unique, and that
 * you do not undo any tests already implemented.
 *
 * One last request; PLEASE PLEASE PLEASE send me any updates you
 * may make. If you cannot get through to me on email, send me a
 * paper copy (or even better, a floppy copy :-)) to Grahan Toal,
 * c/o 6 Pypers wynd, Prestonpans, East Lothian, Scotland EH32 9AH.
 *
 *   Graham Toal <gtoal@uk.ac.ed>
 * [PS: I can read DOS and RISC OS floppies only]
 */
#define STDLIBS 1
#define PROTOTYPES 1
#define KNOWS_VOID 1
#define KNOWS_LINE 1
#define KNOWS_REGISTER 1
#endif /* TESTING_DEFS */
/*###################################################################*/
#define SYS_ANY 1
#define COMPILER_ANY 1
  /* Don't yet know what machine it is.  Add a test once identified. */
/*--------------------------*/
#ifdef MSDOS
#define SYS_MSDOS 1
#endif
/*--------------------------*/
#ifdef __STDC__
#define STDLIBS 1
#define PROTOTYPES 1
#define KNOWS_VOID 1
  /* If the compiler defines STDC it should have these. We can undef
     them later if the compiler was telling lies! */
#endif
/*--------------------------*/
#ifdef MPU8086
#define SYS_MSDOS 1
  /* Aztec does not #define MSDOS.
     We assume it is Aztec on MSDOS from the MPU8086.
   */
#ifdef __STDC__
#define COMPILER_AZTEC 1
  /* Aztec is known to also define __STDC__ (illegally).  Therefore if
     __STDC__ is not defined, it is probably not Aztec */
#endif
#endif

#ifdef SYS_MSDOS
/*--------------------------*/
#ifdef __STDC__
/*----------------------*/
#define COMPILER_MICROSOFT 1
  /* I assume that the combination of #define MSDOS and #define __STDC__
     without any other collaboration means MICROSOFT.  (Who, incidentally,
     are being naughty by declaring __STDC__) */
#define KNOWS_LINE 1
#else
/*----------------------*/
#ifdef __ZTC__
/*------------------*/
#define COMPILER_ZORTECH 1
#undef STDLIBS
  /* Another system without locale.h */
#define PROTOTYPES 1
#else
/*------------------*/
/* A non-std msdos compiler */
#undef STDLIBS
#undef PROTOTYPES
/*------------------*/
#endif
/*----------------------*/
#endif
/*--------------------------*/
#endif
#ifdef TURBOC
  /* Although Turbo C correctly does not define __STDC__, it has SOME
     standard libraries (but not all - locale.h?) and accepts prototypes. */
#undef STDLIBS
#define PROTOTYPES 1
#define SYS_MSDOS 1
#define COMPILER_TURBOC 1
  /* TURBOC is defined, but has no value.  This allows it to be tested
     with #if as well as #ifdef. */
#endif
/*--------------------------*/
#ifdef COMPILER_MICROSOFT
#undef STDLIBS
  /* Again, like Turbo C, does not know locale.h */
#define PROTOTYPES 1
#endif
/*--------------------------*/
#ifdef SYS_MSDOS
#define MEMMODELS 1
  /* We assume ALL MSDOS machines use memory models */
#endif
/*--------------------------*/
#ifdef UX63
#undef STDLIBS
#undef PROTOTYPES
#define SYS_UX63 1
#define COMPILER_PCC 1
/*#define LIB_STRINGS 1 - apparently not... */
#endif
/*--------------------------*/
#ifdef sun
#undef STDLIBS
#undef PROTOTYPES
#define SYS_SUN 1
#define COMPILER_PCC 1
#endif
/*--------------------------*/
#ifdef THINK_C
#define SYS_MAC 1
#define COMPILER_THINKC 1
#define KNOWS_VOID 1
#endif
/*--------------------------*/
#ifdef sparc
#undef STDLIBS
#undef PROTOTYPES
#define SYS_SPARC 1
#define COMPILER_PCC 1
#endif
/*--------------------------*/
#ifdef ARM
#define SYS_RISCOS 1
  /* I fear Acorn may define 'ARM' on both unix and risc os versions.
     Should find out whether they define others as well, to differentiate. */
#endif
#ifdef __ARM__
#define SYS_RISCOS 1
  /* I fear Acorn may define 'ARM' on both unix and risc os versions.
     Should find out whether they define others as well, to differentiate. */
#endif
#ifdef __arm
#define SYS_RISCOS 1
  /* I fear Acorn may define 'ARM' on both unix and risc os versions.
     Should find out whether they define others as well, to differentiate. */
#endif
/*--------------------------*/
#ifdef SYS_RISCOS
#define COMPILER_NORCROFT 1
#define KNOWS_REGISTER 1
#define KNOWS_LINE 1
#endif
/*--------------------------*/
#ifdef vms
#define SYS_VMS 1
#endif
/*--------------------------*/

/*--------------------------*/
#ifdef SYS_UX63
#undef SYS_ANY
#endif
#ifdef SYS_MSDOS
#undef SYS_ANY
#endif
#ifdef SYS_SUN
#undef SYS_ANY
#endif
#ifdef SYS_SPARC
#undef SYS_ANY
#endif
#ifdef SYS_RISCOS
#undef SYS_ANY
#endif
#ifdef SYS_MAC
#undef SYS_ANY
#endif
#ifdef SYS_VMS
#undef SYS_ANY
#endif
/*--------------------------*/
#ifdef COMPILER_PCC
#undef COMPILER_ANY
#endif
#ifdef COMPILER_MICROSOFT
#undef COMPILER_ANY
#endif
#ifdef COMPILER_TURBOC
#undef COMPILER_ANY
#endif
#ifdef COMPILER_ZORTECH
#undef COMPILER_ANY
#endif
#ifdef COMPILER_AZTEC
#undef COMPILER_ANY
#endif
#ifdef COMPILER_NORCROFT
#undef COMPILER_ANY
#endif
#ifdef COMPILER_THINKC
#undef COMPILER_ANY
#endif
/*--------------------------*/
/* ##################################################################### */
#ifdef TESTING_DEFS
#include <stdio.h>
/* ======================================================================= */
#ifdef STDLIBS
              /* If any of these fail, make sure STDLIBS is not
                 defined for your machine. */
#include <stdlib.h>      /* STDLIBS should not be defined. */
#include <time.h>        /* STDLIBS should not be defined. */
#include <locale.h>      /* STDLIBS should not be defined. */
#endif
/* ======================================================================= */
#ifdef KNOWS_VOID
void test() {            /* KNOWS_VOID should not be defined */
  /* Make sure your ifdef's don't #define KNOWS_VOID if this fails */
}
#endif
/* ======================================================================= */
#ifdef KNOWS_REGISTER
int regtest() {
register int i = 0;          /* KNOWS_REGISTER should not be defined */
  /* Make sure your ifdef's don't #define KNOWS_REGISTER if this fails */
  return(i);
}
#endif
/* ======================================================================= */
#ifdef PROTOTYPES
int main(int argc, char **argv)   /* PROTOTYPES should not be defined */
/* If this fails, make sure PROTOTYPES is not defined for your machine. */
#else
int main(argc,argv)
int argc;
char **argv;
#endif
{
/*-------------------------------------------------------------------------*/
  printf("We should know just what the machine is, or 'SYS_ANY':\n");
#ifdef SYS_UX63
  printf("#define SYS_UX63 %d\n", SYS_UX63);
#endif
#ifdef SYS_MSDOS
  printf("#define SYS_MSDOS %d\n", SYS_MSDOS);
#endif
#ifdef SYS_SUN
  printf("#define SYS_SUN %d\n", SYS_SUN);
#endif
#ifdef SYS_SPARC
  printf("#define SYS_SPARC %d\n", SYS_SPARC);
#endif
#ifdef SYS_RISCOS
  printf("#define SYS_RISCOS %d\n", SYS_RISCOS);
#endif
#ifdef SYS_MAC
  printf("#define SYS_MAC %d\n", SYS_MAC);
#endif
#ifdef SYS_VMS
  printf("#define SYS_VMS %d\n", SYS_VMS);
#endif
#ifdef SYS_ANY
  printf("#define SYS_ANY %d\n", SYS_ANY);
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the machine has different memory models or not:\n");
#ifdef MEMMODELS
  printf("#define MEMMODELS %d\n", MEMMODELS);
#else
  printf("#undef MEMMODELS\n");
#endif
/*-------------------------------------------------------------------------*/
  printf("One compiler name should be given, or 'COMPILER_ANY':\n");
#ifdef COMPILER_PCC
  printf("#define COMPILER_PCC %d\n", COMPILER_PCC);
#endif
#ifdef COMPILER_MICROSOFT
  printf("#define COMPILER_MICROSOFT %d\n", COMPILER_MICROSOFT);
#endif
#ifdef COMPILER_TURBOC
  printf("#define COMPILER_TURBOC %d\n", COMPILER_TURBOC);
#endif
#ifdef COMPILER_ZORTECH
  printf("#define COMPILER_ZORTECH %d\n", COMPILER_ZORTECH);
#endif
#ifdef COMPILER_AZTEC
  printf("#define COMPILER_AZTEC %d\n", COMPILER_AZTEC);
  /* Can exist on other machines as well as MSDOS */
#endif
#ifdef COMPILER_LATTICE
  /* Currently coming through as 'COMPILER_ANY' - haven't found one to test */
  /* Can exist on other machines as well as MSDOS. Meanwhile pass it in     */
  /* on the command_line?                                                   */
  printf("#define COMPILER_LATTICE %d\n", COMPILER_LATTICE);
#endif
#ifdef COMPILER_GCC
  /* Ditto. Test in appropriate places for each machine. */
  printf("#define COMPILER_GCC %d\n", COMPILER_GCC);
#endif
#ifdef COMPILER_NORCROFT
  printf("#define COMPILER_NORCROFT %d\n", COMPILER_NORCROFT);
#endif
#ifdef COMPILER_THINKC
  printf("#define COMPILER_THINKC %d\n", COMPILER_THINKC);
#endif
#ifdef COMPILER_ANY
  printf("#define COMPILER_ANY %d\n", COMPILER_ANY);
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the compiler accepts ANSI-like libraries or not:\n");
#ifdef STDLIBS
  printf("#define STDLIBS %d\n",STDLIBS);
#else
  printf("#undef STDLIBS\n");
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the machine accepts ANSI prototypes or not:\n");
#ifdef PROTOTYPES
  printf("#define PROTOTYPES %d\n", PROTOTYPES);
#else
  printf("#undef PROTOTYPES\n");
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the machine needs nonstandard #include <strings.h> or not:\n");
#ifdef LIB_STRINGS
  printf("#define LIB_STRINGS %d\n", LIB_STRINGS);
#else
  printf("#undef LIB_STRINGS\n");
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the machine accepts the 'void' keyword or not:\n");
#ifdef KNOWS_VOID
  printf("#define KNOWS_VOID %d\n", KNOWS_VOID);
#else
  printf("#undef KNOWS_VOID\n");
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the compiler accepts the '__LINE__' directive or not:\n");
#ifdef KNOWS_LINE
  printf("#define KNOWS_LINE %d\n", KNOWS_LINE);
#else
  printf("#undef KNOWS_LINE\n");
#endif
/*-------------------------------------------------------------------------*/
  printf("Either the machine accepts the 'register' keyword or not:\n");
#ifdef KNOWS_REGISTER
  printf("#define KNOWS_REGISTER %d\n", KNOWS_REGISTER);
#else
  printf("#undef KNOWS_REGISTER\n");
#endif
  /* Note - this is intended to be used only if your compiler accepts
     'register' *AND* compiles code with it correctly!  Some compilers
     show up bugs when register optimisations are attempted! */
/*-------------------------------------------------------------------------*/
  if (argv[argc] != 0) printf("Warning! argv[argc] != NULL !!! (Non ansii)\n");
}
#endif /* TESTING_DEFS */
#endif /* Already seen? */

