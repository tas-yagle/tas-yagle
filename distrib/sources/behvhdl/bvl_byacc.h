# ifndef BVL_BYACC_H
# define BVL_BYACC_H

/* ###--------------------------------------------------------------### */
/*                                  */
/* file     : bvl_byacc.h                       */
/* date     : Oct  4 1993                       */
/* version  : v108                          */
/* author   : Pirouz BAZARGAN SABET                 */
/* content  : declaration of external functions and global variables*/
/*        used by yacc                      */
/*                                  */
/* ###--------------------------------------------------------------### */

    // *************************************************
    // declaration structure for ports and signals
    // ************************************************* 

typedef struct bvldecl {
  struct bvldecl *NEXT     ;   /* next port or signal                    */
  char           *NAME     ;   /* name                                   */
  struct biabl   *BIABL    ;   /* list of drivers (ABL)                  */
  struct binode  *BINODE   ;   /* list of drivers (BDD)                  */
  struct chain   *ABL      ;   /* equation (ABL)                         */
  struct node    *NODE     ;   /* equation (BDD)                         */
  char            DIRECTION;   /* port's mode (I, O, B, Z, T)            */
  char            TYPE     ;   /* port's type (B, M or W)                */
  unsigned int    TIME     ;   /* next port or signal                    */
  char            OBJTYPE  ;   /* type :  A (beaux, bepor, berin, beout) */
                              /*         B (bebus, bebux)               */
                              /*         R (bereg)                      */
} bvldecl_list;

typedef struct bvlexpr {
  char          *IDENT   ;      /* identifier or constant name  */
  unsigned int   TIME    ;      /* waveform's delay     */
  struct chain  *LIST_ABL;      /* pointer on bvl_abllst list   */
  short          WIDTH   ;      /* width of bit vector      */
} bvlexpr;

typedef struct bvlname {
  char          *NAME;          /* identifier name      */
  short          LEFT;          /* vector's left index      */
  short          RIGHT;         /* vector's right index     */
  char           FLAG;          /* scalar ('S') or array 'A'    */
} bvlname;

typedef struct gtype {
  int  VALU;
  char FLAG;
} gtype;

typedef struct bvlcond {
  struct bvlcond  *NEXT;
  struct chain     *CND;
  struct chain     *VAL;   
  char             *IDENT;  /* name */
  unsigned int      TIME;   /* wavwform's delay */
  int               NUM;    /* number of components */
} bvlcond;

extern int    BVL_AUXMOD;       /* simplify internal sig (= 1)  */
extern int    BVL_CHECK;        /* activate coherency checks */

extern struct chain *BVL_INTLST;
extern struct begen *BVL_GENPNT;

extern char          BVL_ERRFLG;    /* set to 1 in case of error    */
extern struct befig *BVL_HEDFIG;    /* head of befigs       */

# endif
