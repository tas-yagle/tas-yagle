/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : bgl_parse.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

//#define BGL_DEBUG

extern int           BGL_CASE_SENSITIVE;
extern ht           *BGL_CASEHT;

extern int           BGL_USE_LIBRARY;

extern int           BGL_AUXMOD;

extern FILE    *bgl_bcompin;

extern int      bgl_bcompdebug;

typedef union YYSTYPE {
    double      decimal;
    struct {
        int value;
        int size;
    }           integer;
    int         base;
    char       *text;
    char        car;
    bgl_name    name;
    bgl_expr    expr;
    biabl_list *biabl;
    chain_list *list;
    doe delay_or_event;
    custom_gate_instance *gateinstance;
    zero_un_if z_u_i;
    zero_un_if *ptz_u_i;
} YYSTYPE;


#ifdef __cplusplus
extern "C" {
#endif
void             *bgl_initparser(FILE *ptinbuf);
void              bgl_delparser(void *parm);
bgl_bcompcontext *bgl_getcontext(void *parm);
int               bgl_bcompparse(void *parm);
void 	          bgl_bcompclean(bgl_bcompcontext *context);
int               bgl_bcomplex(YYSTYPE *lvalp, void *parm);
int               bgl_bcomperror(char *str, void *parm);
#ifdef __cplusplus
}
#endif

