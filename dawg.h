/**
*
*       DAWG.H
*
*       Header file for Directed Acyclic Word Graph access
*
*       The format of a DAWG node (8-bit arbitrary data) is:
*
*        31                24 23  22  21                                     0
*       +--------------------+---+---+--+-------------------------------------+
*       |      Letter        | W | N |??|            Node pointer             |
*       +--------------------+---+---+--+-------------------------------------+
*
*      where N flags the last edge in a node and W flags an edge as the
*      end of a word. 21 bits are used to store the node pointer, so the
*      dawg can contain up to 262143 edges. (and ?? is reserved - all code
*      generating dawgs should set this bit to 0 for now)
*
*      The root node of the dawg is at address 1 (because node 0 is reserved
*      for the node with no edges).
*
*      Files containing a DAWG have a magic-number immediately before the 0-th
*      element.
*
**/

#ifndef TRUE
#define TRUE (0==0)
#endif
#ifndef FALSE
#define FALSE (0!=0)
#endif

#define DAWG_MAGIC 0xC64144F7L           /* Arbitrary value */
#define REVERSE_DAWG_MAGIC 0xF74441C6L   /* reverse byte-sex */

#define TERMINAL_NODE 0     /* First word always contains 0L */
#define ROOT_NODE 1         /* Root of dag                   */

#define V_END_OF_WORD   23                      /* Bit number of W */
#define M_END_OF_WORD   (1L << V_END_OF_WORD)
#define V_END_OF_NODE   22                      /* Bit number of N */
#define M_END_OF_NODE   (1L << V_END_OF_NODE)

#define V_LETTER        24
#define M_LETTER        0xFF

#define M_NODE_POINTER  0x1FFFFFL     /* Bit mask for node pointer */

/* max_chars and base_chars are a hangover from the days when this
   was trying to save space, and dawgs were only able to contain
   characters in the range 'a'..'z' 'A'..'Z' (squeezed into 6 bits).
   Now that we're '8-bit clean' we no longer need these.  Later code
   in fact doesn't support the old style; but some procedures still
   subtract 'BASE_CHAR' from the character to normalize it.  Since it
   is now 0 it is a no-op... 

   Just in case this code ever migrates to a 16-bit environment (Japanese?),
   I'll leave these in.

 */
#define MAX_CHARS       256
#define BASE_CHAR       0

/* Enough? (This is fairly arbitrary) */
#define MAX_WORD_LEN    256

#define MAX_LINE        256

typedef long NODE;
typedef long INDEX;

