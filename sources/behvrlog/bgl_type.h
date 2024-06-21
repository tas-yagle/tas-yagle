/*****************************************************************************
* constants                                                                  *
*****************************************************************************/

/* ###--------------- Simple Flag Values ---------------------------### */

#define BGL_UKNDFN     0		/* unknown port map mode	*/
#define BGL_EXPDFN     1		/* explicit port map		*/
#define BGL_IMPDFN     2		/* implicit port map		*/

/* ###--------------- Hash Table Fields and Values -----------------### */

/* Value defines */
#define BGL_FIGDFN     1		/* root model			*/	
#define BGL_MODELDFN     2		/* child model			*/

/* Value masks for signal */
#define BGL_ICNDFN     1		/* input port			*/
#define BGL_OCNDFN     2		/* output port			*/
#define BGL_BCNDFN     3		/* inout port			*/
#define BGL_XCNDFN     4		/* linkage port			*/

#define BGL_BITDFN     8		/* bit type			*/
#define BGL_MUXDFN    16		/* mux_bit type			*/
#define BGL_WORDFN    24		/* wor_bit type			*/
#define BGL_BTVDFN    40        /* bit_vector   type            */
#define BGL_MXVDFN    48        /* mux_vector   type            */
#define BGL_WRVDFN    56        /* wor_vector   type            */
#define BGL_RGVDFN    64        /* reg_vector   type            */
#define BGL_CVTDFN    72        /* convertion   type            */
#define BGL_BOLDFN    80        /* boolean      type            */

#define BGL_NORDFN   128		/* non guarded signal		*/
#define BGL_BUSDFN   256		/* guarded signal (bus)		*/
#define BGL_REGDFN   384        /* guarded signal (register)*/

#define BGL_MODDFN     0		/* mod_val field of dct_recrd	*/
#define BGL_SIGDFN     1		/* sig_val field of dct_recrd	*/
#define BGL_CCNDFN     2		/* ccn_val field of dct_recrd	*/
#define BGL_RNGDFN     3		/* rng_val field of dct_recrd	*/
#define BGL_LBLDFN     4		/* lbl_val field of dct_recrd	*/
#define BGL_LFTDFN     5        /* wmx_val field of dct_recrd   */
#define BGL_RGTDFN     6        /* wmn_val field of dct_recrd   */
#define BGL_PNTDFN     7		/* pnt_val field of dct_recrd	*/

#define BGL_UPTDFN     1        /* direction is up               */
#define BGL_DWTDFN     0        /* direction is down             */

/* Hash Table Constants */

#define BGL_NAMDFN 0
#define BGL_NEWDFN 1

#define BGL_PNTDFN     7		/* pnt_val field of dct_recrd	*/

#define BGL_ALODFN     60		/* minimal size of allocation	 */
                                /* for dct_entry and dct_recrd	 */
#define BGL_HSZDFN     97       /* number of entry points in the */
  
#define BGL_IDENT      ((long) 0x00000001 )
#define BGL_RESULT     ((long) 0x00000002 )
#define BGL_CONCAT     ((long) 0x00000004 )
#define BGL_EXPR       ((long) 0x00000008 )
#define BGL_IF         ((long) 0x00000010 )

#define NE 		 9
#define EQ 		10 
#define NOPI 		11
#define NOPS 		12
#define ANDM 		13
#define CONC 		14
#define CONVRT 		15

#define BGL_UNGDFN 	 0
#define BGL_GRDDFN 	 1

#define WE_ARE_IN_A_CONDITION			1
#define WE_ARE_IN_A_REGISTER_BLOCK		2
#define WE_ARE_IN_A_CONDITION_ELSE_STATEMENT	4
#define WE_ARE_IN_A_CONDITION_STATEMENT		8

#define SIG_IS_BUS		1
#define SIG_IS_REG		2
#define SIG_IS_IN		4
#define SIG_IS_IN_ELSE		8

/* flags du parser */
#define DONTCARE                1
#define IN_PRIMITIVE            2

typedef struct bgldecl {
    struct bgldecl *NEXT     ;   /* next port or signal                    */
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
} bgldecl_list;

typedef struct bglcond {
    struct bglcond  *NEXT;
    struct chain     *CND;
    struct chain     *VAL;   
    char             *IDENT;  /* name */
    unsigned int      TIME;   /* wavwform's delay */
    int               NUM;    /* number of components */
} bglcond_list;

typedef struct {
    char   *NAME;
    int     LEFT;
    int     RIGHT;
    int     ERR_FLAG;
} bgl_name;

typedef struct {
    long            TYPE;
    int             RESULT;
    chain_list     *IDENT;
    chain_list     *CONCAT;
    chain_list     *LIST_ABL;
    short           WIDTH;
    unsigned int    TIME;
} bgl_expr;

typedef struct {
    char               FILENAME[200];
    int                LINENUM;
    befig_list        *BEFIG;
    befig_list        *TOPFIG;
    struct beden     **HSHTAB;
    char              *FIGNAME;
    chain_list        *NM1LST;
    chain_list        *NM2LST;
    chain_list        *NM3LST;
    chain_list        *PILE_CONDITION;
  chain_list *CURRENT_EDGE_ABL;
    chain_list	      *WHERE_ARE_WE; // pour gerer le type des signaux
//  chain_list        *CUSTOM_GATES;
  chain_list        *CUSTOM_GATE_INSTANCES;
    char               ERRFLG;
  int CURRENT_DELAY;
  char FLAGS;
    bgldecl_list      *ALLSIGNALS;
} bgl_bcompcontext;

typedef struct {
  chain_list *abl0,*abl1;
  int cond_sig;
  char xedge;
  chain_list *abl_cond;
} zero_un_if;

typedef struct {
  char *name;
  zero_un_if *zui;
} custom_gate;

typedef struct {
  char *gate_name;
  chain_list *terminals;
  befig_list *befig;
} custom_gate_instance;

typedef struct 
{	
  char type; // 0 = delay, 1 event sinon not supported
  union 
  {
    bgl_expr    expr;
    int delay;
  } u;
} doe;
