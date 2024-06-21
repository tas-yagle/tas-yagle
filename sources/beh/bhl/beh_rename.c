
/* ###--------------------------------------------------------------### */
/* file		: beh_rename.c                                              */
/* version	: v110                                                      */
/* authors	: Anthony LESTER                                            */
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include <string.h>
#include "bhl_lib.h"
#include EQT_H

/*------------------------------------------------------------------
  IDENT : replaceAtomExpr
  ------------------------------------------------------------------
  FUNCTION : replace each occurence of an atomic name               
  ------------------------------------------------------------------*/

void
replaceAtomExpr(chain_list *expr, char *oldname, char *newname)
{
    if (ATOM (expr)) {
        if (VALUE_ATOM(expr) == oldname) VALUE_ATOM(expr) = newname;
    }
    else {
        while ((expr = CDR (expr))) {
	        replaceAtomExpr (CAR (expr), oldname, newname);
        }
    }
}

/*------------------------------------------------------------------
  IDENT : shiftExprStr   
  ------------------------------------------------------------------
  FUNCTION : add a constant to an expression string               
  ------------------------------------------------------------------*/

char *
shiftExprStr(char *exprstr, int shift)
{
    char        buf[1024];
    char       *new_expr, *res;

    sprintf(buf, "%s+%d", exprstr, shift);
    new_expr = eqt_getSimpleEquation(buf);
    res = namealloc(new_expr);
    mbkfree(new_expr);
    return res;
}

/*------------------------------------------------------------------
  IDENT : renameVectAtom 
  ------------------------------------------------------------------
  FUNCTION : rename a vectorised atom with optional shift         
  ------------------------------------------------------------------*/

char *
renameVectAtom(char *oldatom, char *newname, int shift)
{
    int         left, right, num;
    char        stem[256];
    char        buf[256];
    char       *pt;

    num = sscanf(oldatom, "%s %d:%d", stem, &left, &right);
    if (newname == NULL) newname = namealloc(stem);
    switch (num) {
        case 1:
            if ((pt = strchr(oldatom, (int)' ')) == NULL) {
                return namealloc(newname);
            }
            else {
                if (shift != 0) sprintf(buf, "%s %s", newname, shiftExprStr(pt+1, shift));
                else sprintf(buf, "%s %s", newname, pt+1);
                return namealloc(buf);
            }
        case 2:
            sprintf(buf, "%s %d", newname, left+shift);
            return namealloc(buf);
        case 3:
            sprintf(buf, "%s %d:%d", newname, left+shift, right+shift);
            return namealloc(buf);
    }
    return namealloc(newname);
}

/*------------------------------------------------------------------
  IDENT : replaceAtomExpr
  ------------------------------------------------------------------
  FUNCTION : replace each occurence of a vectorised atomic name               
  ------------------------------------------------------------------*/

void
replaceVectAtomExpr(chain_list *expr, char *oldname, char *newname,
					int shift)
{
    if (ATOM (expr)) {
        if (getVectAblVarName(expr) == oldname) {
            VALUE_ATOM(expr) = renameVectAtom(VALUE_ATOM(expr), newname, shift);
        }
    }
    else {
        while ((expr = CDR (expr))){
	        replaceVectAtomExpr (CAR (expr), oldname, newname, shift);
        }
    }
}

/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
beh_renameport(befig_list *ptbefig, bepor_list *ptbepor, berin_list *ptberin,
			   char *newname)
{
    bemsg_list     *ptbemsg;
    beaux_list     *ptbeaux;
    beout_list     *ptbeout;
    bebux_list     *ptbebux;
    bebus_list     *ptbebus;
    bereg_list     *ptbereg;
    biabl_list     *ptbiabl;
    bevectaux_list *ptbevectaux;
    bevectout_list *ptbevectout;
    bevectbux_list *ptbevectbux;
    bevectbus_list *ptbevectbus;
    bevectreg_list *ptbevectreg;
    vectbiabl_list *ptvectbiabl;
    chain_list     *ptchain;
    char           *oldname;

    newname = namealloc(newname);
    oldname = namealloc(ptbepor->NAME);
    ptbepor->NAME = newname;
    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        if (ptbeout->NAME == oldname) ptbeout->NAME = newname;
    }
    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        if (ptbereg->NAME == oldname) ptbereg->NAME = newname;
    }
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        if (ptbebus->NAME == oldname) ptbebus->NAME = newname;
    }
    if (ptberin == NULL) return;
    ptberin->NAME = newname;
    for (ptchain = ptberin->OUT_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbeout = (beout_list *)ptchain->DATA;
        replaceAtomExpr(ptbeout->ABL, oldname, newname);
    }
    for (ptchain = ptberin->OUT_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectout = (bevectout_list *)ptchain->DATA;
        replaceAtomExpr(ptbevectout->ABL, oldname, newname);
    }
    for (ptchain = ptberin->AUX_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbeaux = (beaux_list *)ptchain->DATA;
        replaceAtomExpr(ptbeaux->ABL, oldname, newname);
    }
    for (ptchain = ptberin->MSG_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbemsg = (bemsg_list *)ptchain->DATA;
        replaceAtomExpr(ptbemsg->ABL, oldname, newname);
    }
    for (ptchain = ptberin->AUX_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectaux = (bevectaux_list *)ptchain->DATA;
        replaceAtomExpr(ptbevectaux->ABL, oldname, newname);
    }
    for (ptchain = ptberin->BUS_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbebus = (bebus_list *)ptchain->DATA;
        for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) replaceAtomExpr(ptbiabl->CNDABL, oldname, newname);
            if (ptbiabl->VALABL) replaceAtomExpr(ptbiabl->VALABL, oldname, newname);
        }
    }
    for (ptchain = ptberin->BUX_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbebux = (bebux_list *)ptchain->DATA;
        for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) replaceAtomExpr(ptbiabl->CNDABL, oldname, newname);
            if (ptbiabl->VALABL) replaceAtomExpr(ptbiabl->VALABL, oldname, newname);
        }
    }
    for (ptchain = ptberin->REG_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbereg = (bereg_list *)ptchain->DATA;
        for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) replaceAtomExpr(ptbiabl->CNDABL, oldname, newname);
            if (ptbiabl->VALABL) replaceAtomExpr(ptbiabl->VALABL, oldname, newname);
        }
    }
    for (ptchain = ptberin->BUS_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectbus = (bevectbus_list *)ptchain->DATA;
        for (ptvectbiabl = ptbevectbus->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) replaceAtomExpr(ptbiabl->CNDABL, oldname, newname);
                if (ptbiabl->VALABL) replaceAtomExpr(ptbiabl->VALABL, oldname, newname);
            }
        }
    }
    for (ptchain = ptberin->BUX_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectbux = (bevectbux_list *)ptchain->DATA;
        for (ptvectbiabl = ptbevectbux->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) replaceAtomExpr(ptbiabl->CNDABL, oldname, newname);
                if (ptbiabl->VALABL) replaceAtomExpr(ptbiabl->VALABL, oldname, newname);
            }
        }
    }
    for (ptchain = ptberin->REG_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectreg = (bevectreg_list *)ptchain->DATA;
        for (ptvectbiabl = ptbevectreg->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) replaceAtomExpr(ptbiabl->CNDABL, oldname, newname);
                if (ptbiabl->VALABL) replaceAtomExpr(ptbiabl->VALABL, oldname, newname);
            }
        }
    }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
beh_renamevectport(befig_list *ptbefig, bevectpor_list *ptbevectpor,
				   berin_list *ptberin, char *newname, int shift)
{
    beaux_list     *ptbeaux;
    beout_list     *ptbeout;
    bebux_list     *ptbebux;
    bebus_list     *ptbebus;
    bereg_list     *ptbereg;
    biabl_list     *ptbiabl;
    bevectaux_list *ptbevectaux;
    bevectout_list *ptbevectout;
    bevectbux_list *ptbevectbux;
    bevectbus_list *ptbevectbus;
    bevectreg_list *ptbevectreg;
    vectbiabl_list *ptvectbiabl;
    chain_list     *ptchain;
    loop_list      *ptloop;
    char           *oldname;

    newname = namealloc(newname);
    oldname = namealloc(ptbevectpor->NAME);
    ptbevectpor->NAME = newname;
    ptbevectpor->LEFT  += shift;
    ptbevectpor->RIGHT += shift;
    for (ptbevectout = ptbefig->BEVECTOUT; ptbevectout; ptbevectout = ptbevectout->NEXT) {
        if (ptbevectout->NAME == oldname) {
            ptbevectout->NAME = newname;
            ptbevectout->LEFT += shift;
            ptbevectout->RIGHT += shift;
        }
    }
    for (ptbevectreg = ptbefig->BEVECTREG; ptbevectreg; ptbevectreg = ptbevectreg->NEXT) {
        if (ptbevectreg->NAME == oldname) {
            ptbevectreg->NAME = newname;
            ptbevectreg->LEFT += shift;
            ptbevectreg->RIGHT += shift;
            for (ptvectbiabl = ptbevectreg->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
                ptvectbiabl->LEFT += shift;
                ptvectbiabl->RIGHT += shift;
                for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                    ptloop = beh_getloop(ptbiabl);
                    for (;ptloop; ptloop = ptloop->NEXT) {
                        if (ptloop->INDEX != NULL) {
                            ptloop->INDEX = shiftExprStr(ptloop->INDEX, shift);
                        }
                    }
                }
            }
        }
    }
    for (ptbevectbus = ptbefig->BEVECTBUS; ptbevectbus; ptbevectbus = ptbevectbus->NEXT) {
        if (ptbevectbus->NAME == oldname) {
            ptbevectbus->NAME = newname;
            ptbevectbus->LEFT += shift;
            ptbevectbus->RIGHT += shift;
            for (ptvectbiabl = ptbevectbus->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
                ptvectbiabl->LEFT += shift;
                ptvectbiabl->RIGHT += shift;
                for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                    ptloop = beh_getloop(ptbiabl);
                    for (;ptloop; ptloop = ptloop->NEXT) {
                        if (ptloop->INDEX != NULL) {
                            ptloop->INDEX = shiftExprStr(ptloop->INDEX, shift);
                        }
                    }
                }
            }
        }
    }
    if (ptberin == NULL) return;
    ptberin->NAME   = newname;
    ptberin->LEFT  += shift;
    ptberin->RIGHT += shift;
    for (ptchain = ptberin->OUT_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbeout = (beout_list *)ptchain->DATA;
        replaceVectAtomExpr(ptbeout->ABL, oldname, newname, shift);
    }
    for (ptchain = ptberin->OUT_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectout = (bevectout_list *)ptchain->DATA;
        replaceVectAtomExpr(ptbevectout->ABL, oldname, newname, shift);
    }
    for (ptchain = ptberin->AUX_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbeaux = (beaux_list *)ptchain->DATA;
        replaceVectAtomExpr(ptbeaux->ABL, oldname, newname, shift);
    }
    for (ptchain = ptberin->AUX_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectaux = (bevectaux_list *)ptchain->DATA;
        replaceVectAtomExpr(ptbevectaux->ABL, oldname, newname, shift);
    }
    for (ptchain = ptberin->BUS_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbebus = (bebus_list *)ptchain->DATA;
        for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) replaceVectAtomExpr(ptbiabl->CNDABL, oldname, newname, shift);
            if (ptbiabl->VALABL) replaceVectAtomExpr(ptbiabl->VALABL, oldname, newname, shift);
        }
    }
    for (ptchain = ptberin->BUX_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbebux = (bebux_list *)ptchain->DATA;
        for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) replaceVectAtomExpr(ptbiabl->CNDABL, oldname, newname, shift);
            if (ptbiabl->VALABL) replaceVectAtomExpr(ptbiabl->VALABL, oldname, newname, shift);
        }
    }
    for (ptchain = ptberin->REG_REF; ptchain; ptchain = ptchain->NEXT) {
        ptbereg = (bereg_list *)ptchain->DATA;
        for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) replaceVectAtomExpr(ptbiabl->CNDABL, oldname, newname, shift);
            if (ptbiabl->VALABL) replaceVectAtomExpr(ptbiabl->VALABL, oldname, newname, shift);
        }
    }
    for (ptchain = ptberin->BUS_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectbus = (bevectbus_list *)ptchain->DATA;
        for (ptvectbiabl = ptbevectbus->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) replaceVectAtomExpr(ptbiabl->CNDABL, oldname, newname, shift);
                if (ptbiabl->VALABL) replaceVectAtomExpr(ptbiabl->VALABL, oldname, newname, shift);
            }
        }
    }
    for (ptchain = ptberin->BUX_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectbux = (bevectbux_list *)ptchain->DATA;
        for (ptvectbiabl = ptbevectbux->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) replaceVectAtomExpr(ptbiabl->CNDABL, oldname, newname, shift);
                if (ptbiabl->VALABL) replaceVectAtomExpr(ptbiabl->VALABL, oldname, newname, shift);
            }
        }
    }
    for (ptchain = ptberin->REG_VEC; ptchain; ptchain = ptchain->NEXT) {
        ptbevectreg = (bevectreg_list *)ptchain->DATA;
        for (ptvectbiabl = ptbevectreg->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) replaceVectAtomExpr(ptbiabl->CNDABL, oldname, newname, shift);
                if (ptbiabl->VALABL) replaceVectAtomExpr(ptbiabl->VALABL, oldname, newname, shift);
            }
        }
    }
}

/*}}}************************************************************************/
