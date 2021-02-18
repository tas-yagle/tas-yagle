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

#ifdef __cplusplus
extern "C" {
#endif
void             *bgl_initparser(FILE *ptinbuf);
void              bgl_delparser(void *parm);
bgl_bcompcontext *bgl_getcontext(void *parm);
int               bgl_bcompparse(void *parm);
void 	          bgl_bcompclean(bgl_bcompcontext *context);
#ifdef __cplusplus
}
#endif

