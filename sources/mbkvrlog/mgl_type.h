/*****************************************************************************
* constants                                                                  *
*****************************************************************************/

/* ###--------------- Simple Flag Values ---------------------------### */

#define MGL_UKNDFN     0		/* unknown port map mode	*/
#define MGL_EXPDFN     1		/* explicit port map		*/
#define MGL_IMPDFN     2		/* implicit port map		*/

/* ###--------------- Hash Table Fields and Values -----------------### */

/* Value defines */
#define MGL_FIGDFN     1		/* root model			*/	
#define MGL_MODELDFN     2		/* child model			*/

/* Value masks for signal */
#define MGL_ICNDFN     1		/* input port			*/
#define MGL_OCNDFN     2		/* output port			*/
#define MGL_BCNDFN     3		/* inout port			*/
#define MGL_XCNDFN     4		/* linkage port			*/

#define MGL_BITDFN     8		/* bit type			*/
#define MGL_MUXDFN    16		/* mux_bit type			*/
#define MGL_WORDFN    24		/* wor_bit type			*/
#define MGL_BTVDFN    40        /* bit_vector   type            */
#define MGL_MXVDFN    48        /* mux_vector   type            */
#define MGL_WRVDFN    56        /* wor_vector   type            */
#define MGL_RGVDFN    64        /* reg_vector   type            */
#define MGL_CVTDFN    72        /* convertion   type            */
#define MGL_BOLDFN    80        /* boolean      type            */

#define MGL_NORDFN   128		/* non guarded signal		*/
#define MGL_BUSDFN   256		/* guarded signal (bus)		*/
#define MGL_REGDFN   384        /* guarded signal (register)*/

#define MGL_MODDFN     0		/* mod_val field of dct_recrd	*/
#define MGL_SIGDFN     1		/* sig_val field of dct_recrd	*/
#define MGL_CCNDFN     2		/* ccn_val field of dct_recrd	*/
#define MGL_RNGDFN     3		/* rng_val field of dct_recrd	*/
#define MGL_LBLDFN     4		/* lbl_val field of dct_recrd	*/
#define MGL_LFTDFN     5        /* wmx_val field of dct_recrd   */
#define MGL_RGTDFN     6        /* wmn_val field of dct_recrd   */
#define MGL_PNTDFN     7		/* pnt_val field of dct_recrd	*/

#define MGL_UPTDFN     1        /* direction is up               */
#define MGL_DWTDFN     0        /* direction is down             */

/* Hash Table Constants */

#define MGL_NAMDFN 0
#define MGL_NEWDFN 1

#define MGL_PNTDFN     7		/* pnt_val field of dct_recrd	*/

#define MGL_ALODFN     60		/* minimal size of allocation	 */
                                /* for dct_entry and dct_recrd	 */
/* modifier par Fabrice le 11/2/2002 */
//#define MGL_HSZDFN     97       /* number of entry points in the */
#define MGL_HSZDFN     HASHVAL       /* number of entry points in the */
  
#define MGL_IDENT      ((long) 0x00000001 )
#define MGL_RESULT     ((long) 0x00000002 )
#define MGL_CONCAT     ((long) 0x00000004 )

typedef struct {
    char   *NAME;
    int     LEFT;
    int     RIGHT;
    int     ERR_FLAG;
} mgl_name;

typedef struct {
    long        TYPE;
    int         RESULT;
    char       *IDENT;
    chain_list *CONCAT;
} mgl_expr;

typedef struct {
    char               FILENAME[200];
    int                LINENUM;
    lofig_list        *LOFIG;
    lofig_list        *TOPFIG;
    struct dct_entry **HSHTAB;
    int                SIGIDX;
    char              *FIGNAME;
    char              *MODELNAME;
    chain_list        *NM1LST;
    chain_list        *NM2LST;
    chain_list        *NM3LST;
    lofig_list        *MODEL;
    chain_list        *ASSIGN_LIST;
    char               BLDFLG;
    char               ONLY_ORIENT;
    char               ERRFLG;
} mgl_scompcontext;

/* hash table data structures */

struct dct_entry {
    struct dct_entry *next;
    struct dct_recrd *data;
    char             *key;
};

struct dct_recrd {
    struct dct_recrd *next;
    char             *key;
    short             fd0_val;
    short             fd1_val;
    short             fd2_val;
    short             fd3_val;
    short             fd4_val;
    short             fd5_val;
    short             fd6_val;
    long              pnt_val;
};


