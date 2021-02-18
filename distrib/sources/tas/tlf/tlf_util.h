/******************************************/
/* tlf_util.h                             */
/******************************************/

/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include TLF_H
#include BEH_H
#include CBH_H



#define     TYPE_FLOAT      'F'
#define     TYPE_STRING     'S'


/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/
extern char       TLF_TRACE_MODE ;
extern char      *LIBRARY_TLF_NAME ;

/******************************************************************************/
/* EXTERNS                                                                    */
/******************************************************************************/
//extern  char *cname;
/******************************************************************************/
/* FONCTIONS                                                                  */
/******************************************************************************/
extern char        *tlf_chainlistToStr(chain_list *pchain);
extern void         tlf_aff_cbhseq(cbhseq *p);
extern void         tlf_aff_cbhcomb(cbhcomb *p);
extern void         tlf_setenv ( void );
extern char        *tlf_treatname(char *str);
extern char        *tlf_getlibname(void);
extern short tlf_multout(lofig_list **ptrlofig, cbhcomb **ptrcbhcomb, cbhseq **ptrcbhseq, char *cname);
extern void         tlf_affchain(chain_list *p, char *format, char type);
extern chain_list  *tlf_replaceInAbl(chain_list *abl, char *oldExpr, char *newExpr);
extern short        tlf_lookingForExpr(chain_list *ch, char *expr);


