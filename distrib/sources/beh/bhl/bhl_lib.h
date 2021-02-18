
/* ###--------------------------------------------------------------### */
/* file		: bhl110.h						*/
/* date		: Oct 30 1995						*/
/* version	: v110							*/
/* author	: Pirouz BAZARGAN SABET					*/
/* contents	: high level library for behavioural description	*/
/* ###--------------------------------------------------------------### */

#ifndef BEH_BHLDEF
#define BEH_BHLDEF

#include MUT_H
#include LOG_H
#include BEH_H

	/* ###------------------------------------------------------### */
	/*    defines							*/
	/* ###------------------------------------------------------### */

	/* ###------------------------------------------------------### */
	/*    structure definitions					*/
	/* ###------------------------------------------------------### */

	/* ###------------------------------------------------------### */
	/*    functions							*/
	/* ###------------------------------------------------------### */

extern int           beh_chkbefig       (befig_list *befig, unsigned int  mode);
extern void          beh_debug          (void *head_pnt, char *type);
extern void          beh_makbdd         (befig_list *befig);
extern void          beh_makgex         (befig_list *befig);
extern void          beh_makderiv       (befig_list *befig);
extern void          beh_makvarlist     (befig_list *befig);
extern void          beh_makquad        (befig_list *befig);
extern void          beh_freabl         (befig_list *befig);
extern void          beh_depend         (befig_list *befig);
extern struct chain *beh_namelist       (chain_list *pt_exp);
extern struct chain *beh_unamlist       (chain_list *pt_exp);
extern void          beh_indexbdd       (befig_list *befig);

extern void          beh_namewrap       (befig_list *befig, char *(*wrapFunc)(char *));
extern void          beh_wrapAtomExpr       (chain_list *expr, char *(*wrapFunc)(char *));
extern void          replaceAtomExpr    (chain_list *expr, char *oldname, char *newname);
extern char         *shiftExprStr       (char *exprstr, int shift);
extern char         *renameVectAtom     (char *oldatom, char *newname, int shift);
extern void          replaceVectAtomExpr(chain_list *expr, char *oldname, char *newname, int shift);

extern void          beh_renameport     (befig_list *befig, bepor_list *ptbepor, berin_list *ptberin, char *newname);
extern void          beh_renamevectport (befig_list *befig, bevectpor_list *ptbevectpor, berin_list *ptberin, char *newname, int shift);

#endif
