
/* ###--------------------------------------------------------------### */
/* file		: beh_namewrap.c                                              */
/* version	: v110                                                      */
/* authors	: Anthony LESTER                                            */
/* description	: figure level function                                 */
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include <string.h>
#include "bhl_lib.h"

/*------------------------------------------------------------------
  IDENT : beh_wrapAtomExpr
  ------------------------------------------------------------------
  FUNCTION : rajoute un prefix et/ou suffix a chaque atome 
             d'une expression
  ------------------------------------------------------------------*/
void
beh_wrapAtomExpr (chain_list *expr, char *(*wrapFunc)(char *))
{
  if (ATOM (expr))
    {
      if (isBitStr(VALUE_ATOM(expr))) return;
      VALUE_ATOM(expr) = wrapFunc(VALUE_ATOM(expr));
    }
  else
    {
      while ((expr = CDR (expr)))
	    beh_wrapAtomExpr (CAR (expr), wrapFunc);
    }
}

void
beh_namewrap(befig_list *ptbefig, char *(*wrapFunc)(char *))
{
    bepor_list     *ptbepor;
    berin_list     *ptberin;
    bemsg_list     *ptbemsg;
    beaux_list     *ptbeaux;
    beout_list     *ptbeout;
    bebux_list     *ptbebux;
    bebus_list     *ptbebus;
    bereg_list     *ptbereg;
    biabl_list     *ptbiabl;
    bevectpor_list *ptbevectpor;
    bevectaux_list *ptbevectaux;
    bevectout_list *ptbevectout;
    bevectbux_list *ptbevectbux;
    bevectbus_list *ptbevectbus;
    bevectreg_list *ptbevectreg;
    vectbiabl_list *ptvectbiabl;

    ptbefig->NAME = wrapFunc(ptbefig->NAME);
    for (ptbepor = ptbefig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
        ptbepor->NAME = wrapFunc(ptbepor->NAME);
    }
    for (ptberin = ptbefig->BERIN; ptberin; ptberin = ptberin->NEXT) {
        ptberin->NAME = wrapFunc(ptberin->NAME);
    }
    for (ptbemsg = ptbefig->BEMSG; ptbemsg; ptbemsg = ptbemsg->NEXT) {
        beh_wrapAtomExpr(ptbemsg->ABL, wrapFunc);
    }
    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        ptbeaux->NAME = wrapFunc(ptbeaux->NAME);
        beh_wrapAtomExpr(ptbeaux->ABL, wrapFunc);
    }
    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        ptbeout->NAME = wrapFunc(ptbeout->NAME);
        beh_wrapAtomExpr(ptbeout->ABL, wrapFunc);
    }
    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        ptbebux->NAME = wrapFunc(ptbebux->NAME);
        for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) beh_wrapAtomExpr(ptbiabl->CNDABL, wrapFunc);
            if (ptbiabl->VALABL) beh_wrapAtomExpr(ptbiabl->VALABL, wrapFunc);
        }
    }
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        ptbebus->NAME = wrapFunc(ptbebus->NAME);
        for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) beh_wrapAtomExpr(ptbiabl->CNDABL, wrapFunc);
            if (ptbiabl->VALABL) beh_wrapAtomExpr(ptbiabl->VALABL, wrapFunc);
        }
    }
    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        ptbereg->NAME = wrapFunc(ptbereg->NAME);
        for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (ptbiabl->CNDABL) beh_wrapAtomExpr(ptbiabl->CNDABL, wrapFunc);
            if (ptbiabl->VALABL) beh_wrapAtomExpr(ptbiabl->VALABL, wrapFunc);
        }
    }
    for (ptbevectpor = ptbefig->BEVECTPOR; ptbevectpor; ptbevectpor = ptbevectpor->NEXT) {
        ptbevectpor->NAME = wrapFunc(ptbevectpor->NAME);
    }
    for (ptbevectaux = ptbefig->BEVECTAUX; ptbevectaux; ptbevectaux = ptbevectaux->NEXT) {
        ptbevectaux->NAME = wrapFunc(ptbevectaux->NAME);
        beh_wrapAtomExpr(ptbevectaux->ABL, wrapFunc);
    }
    for (ptbevectout = ptbefig->BEVECTOUT; ptbevectout; ptbevectout = ptbevectout->NEXT) {
        ptbevectout->NAME = wrapFunc(ptbevectout->NAME);
        beh_wrapAtomExpr(ptbevectout->ABL, wrapFunc);
    }
    for (ptbevectbux = ptbefig->BEVECTBUX; ptbevectbux; ptbevectbux = ptbevectbux->NEXT) {
        ptbevectbux->NAME = wrapFunc(ptbevectbux->NAME);
        for (ptvectbiabl = ptbevectbux->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) beh_wrapAtomExpr(ptbiabl->CNDABL, wrapFunc);
                if (ptbiabl->VALABL) beh_wrapAtomExpr(ptbiabl->VALABL, wrapFunc);
            }
        }
    }
    for (ptbevectbus = ptbefig->BEVECTBUS; ptbevectbus; ptbevectbus = ptbevectbus->NEXT) {
        ptbevectbus->NAME = wrapFunc(ptbevectbus->NAME);
        for (ptvectbiabl = ptbevectbus->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) beh_wrapAtomExpr(ptbiabl->CNDABL, wrapFunc);
                if (ptbiabl->VALABL) beh_wrapAtomExpr(ptbiabl->VALABL, wrapFunc);
            }
        }
    }
    for (ptbevectreg = ptbefig->BEVECTREG; ptbevectreg; ptbevectreg = ptbevectreg->NEXT) {
        ptbevectreg->NAME = wrapFunc(ptbevectreg->NAME);
        for (ptvectbiabl = ptbevectreg->VECTBIABL; ptvectbiabl; ptvectbiabl = ptvectbiabl->NEXT) {
            for (ptbiabl = ptvectbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                if (ptbiabl->CNDABL) beh_wrapAtomExpr(ptbiabl->CNDABL, wrapFunc);
                if (ptbiabl->VALABL) beh_wrapAtomExpr(ptbiabl->VALABL, wrapFunc);
            }
        }
    }
}

