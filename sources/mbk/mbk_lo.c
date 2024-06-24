/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* 
 * Purpose : acces functions to logical structures
 * Date    : 30/04/91
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 * Modified by <Gregoire.Avot@lip6.fr> 1997,98
 */

#ident  "$Id: mbk_lo.c,v 1.108 2009/05/20 12:22:03 fabrice Exp $"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include MUT_H
#include MLO_H
#include RCN_H
#include EQT_H
#include AVT_H
#include "mbk_lo.h"
#include "mlu_lib.h"

int NB_TAT_LOSIG = 0;

//#define MBK_TRACE_BAD_PTYPE

/*******************************************************************************
 * global variable for netlist view                                             *
 *******************************************************************************/
lofig_list *HEAD_LOFIG;            /* physical figure list head   */
ht *HT_LOFIG = NULL ;
voidt *VT_MODNAMEINDEX = NULL ;
ht *HT_MODINDEXNAME = NULL ;
char *TN_MODEL_NAME = NULL ;
char *TP_MODEL_NAME = NULL ;



void mbk_init_NewBKSIG(NewBKSIG *bksig)
{
  bksig->maxindex=-1;
  bksig->TAB=NULL;
}

void mbk_free_NewBKSIG(NewBKSIG *bksig)
{
  if (bksig->TAB!=NULL) FreeAdvancedTableAllocator((AdvancedTableAllocator *)bksig->TAB);
  bksig->TAB=NULL;
}

losig_list *mbk_NewBKSIG_getindex(lofig_list *ptfig, int index)
{
  int sigsize;
  AdvancedTableAllocator *ata;

  if (ptfig->BKSIG.TAB==NULL)
  {
    sigsize = getsigsize( ptfig );
    ptfig->BKSIG.TAB=CreateAdvancedTableAllocator(sigsize, sizeof(losig_list));
    ata=(AdvancedTableAllocator *)ptfig->BKSIG.TAB;
    ata->init=1;
  }
  else ata=(AdvancedTableAllocator *)ptfig->BKSIG.TAB;
  
  if (index>ptfig->BKSIG.maxindex) ptfig->BKSIG.maxindex=index;
  return (losig_list *)GetAdvancedTableElem(ata, index);
}

/*******************************************************************************
 * function delalllofig()                                                          *
 *******************************************************************************/
void delalllofig()
{
  lofig_list *lofig ;
  lofig_list *next ;

  for( lofig = HEAD_LOFIG ; lofig ; lofig = next ) {
    next = lofig->NEXT;
    dellofig( lofig->NAME );
  }
}

/*******************************************************************************
 * function addlofig()                                                          *
 *******************************************************************************/
lofig_list *addlofig(char *name)
{
  lofig_list *ptfig;

  if (HT_LOFIG == NULL) {
    HT_LOFIG = addht(50);
  }
  name = namealloc(name);
  /* scan figure list */
  ptfig = (lofig_list *)gethtitem(HT_LOFIG, name);
  if (ptfig == (lofig_list *)EMPTYHT) ptfig = NULL;
  else if (ptfig == (lofig_list *)DELETEHT) {
    ptfig = HEAD_LOFIG;
    while (ptfig != NULL && ptfig->NAME != name)
      ptfig = ptfig->NEXT;
  }

  if (ptfig != NULL)   /* figure exists */
      avt_errmsg (MBK_ERRMSG, "020", AVT_FATAL, name);

  ptfig = (lofig_list *)mbkalloc(sizeof(lofig_list));
  ptfig->MODE   = 'A';
  ptfig->NAME   = name;
  ptfig->MODELCHAIN  = NULL;
  ptfig->LOINS  = NULL;
  ptfig->LOTRS  = NULL;
  ptfig->LOCON  = NULL;
  ptfig->LOSIG  = NULL;
  mbk_init_NewBKSIG(&ptfig->BKSIG);
  ptfig->USER   = NULL;
  ptfig->NEXT   = HEAD_LOFIG;
  HEAD_LOFIG = ptfig;

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- addlofig  : %s\n", name);

  addhtitem(HT_LOFIG, name, (long)ptfig);
  return ptfig;
}

/*******************************************************************************
 * function addlomodel()                                                        *
 * used by the parsers to construct a temporary model                           *
 *******************************************************************************/
lofig_list *addlomodel(lofig_list *model, char *name)
{
  lofig_list *fig;

  name = namealloc(name);
  /* check to see if the model has already been loaded */
  if(FAST_MODE != 'Y') {
    for (fig = model; fig && fig->NAME != name; fig = fig->NEXT);

    if (fig)  /* figure exists */
      avt_errmsg (MBK_ERRMSG, "021", AVT_FATAL, name);
    
  }
  fig = (lofig_list *)mbkalloc(sizeof(lofig_list));
  fig->MODE = 'A';
  fig->NAME = name;
  fig->MODELCHAIN = NULL;
  fig->LOINS = NULL;
  fig->LOTRS = NULL;
  fig->LOCON = NULL;
  fig->LOSIG = NULL;
  mbk_init_NewBKSIG(&fig->BKSIG);
  fig->USER = NULL;
  fig->NEXT = model;
  model = fig;

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- addlomodel  : %s\n", name);

  return  model;
}

/*******************************************************************************
 * function getloadedlofig()                                                    *
 *******************************************************************************/
lofig_list *getloadedlofig(char *figname)
{
  lofig_list *ptfig;

  figname = namealloc(figname);
  if (HT_LOFIG == NULL) {
    HT_LOFIG = addht(50);
  }

  /* scan figure list */
  ptfig = (lofig_list *)gethtitem(HT_LOFIG, figname);
  if (ptfig == (lofig_list *)EMPTYHT) ptfig = NULL;
  else if (ptfig == (lofig_list *)DELETEHT) {
    ptfig = HEAD_LOFIG;
    while (ptfig != NULL && ptfig->NAME != figname)
      ptfig = ptfig->NEXT;
  }
  return ptfig;
}

/*******************************************************************************
 * function addlofigparam()                                                     *
 *******************************************************************************/
void
addlofigparam(lofig_list *ptlofig, char *param, float value, char *expr)
{
    optparam_list *newparam;
    ptype_list    *ptuser;
    
    ptuser = getptype(ptlofig->USER, OPT_PARAMS);
    if (ptuser == NULL) {
        newparam = addoptparam(NULL, param, value, expr);
        ptlofig->USER = addptype(ptlofig->USER, OPT_PARAMS, newparam);
    }
    else {
        newparam = addoptparam((optparam_list *)ptuser->DATA, param, value, expr);
        ptuser->DATA = newparam;
    }
}

/*******************************************************************************
 * function getlofigparam()                                                     *
 *******************************************************************************/
float
getlofigparam(lofig_list *ptlofig, void *param, char **expr, int *status)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptlofig->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        return getoptparam((optparam_list *)ptuser->DATA, param, expr, status);
    }

    if (expr) *expr = NULL;
    if (status) *status = 0;
    return 0.0;
}

/*******************************************************************************
 * function freelofigparams()                                                  *
 *******************************************************************************/
void
freelofigparams(lofig_list *ptlofig)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptlofig->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        freeoptparams((optparam_list *)ptuser->DATA);
        ptlofig->USER = delptype(ptlofig->USER, OPT_PARAMS);
    }
}
  
/*******************************************************************************
 * function addlotrs()                                                          *
 *******************************************************************************/
extern lotrs_list *addlotrs (lofig_list *ptfig, char type, long x, long y, long width, long length, long ps, long pd, long xs, long xd, losig_list *ptgrid, losig_list *ptsource, losig_list *ptdrain, losig_list *ptbulk, char *name)
{
  lotrs_list  *pttrs;
  locon_list  *ptcon;

  if (name!=NULL) name = namealloc( name );

  if (!MLO_IS_TRANSN(type) && !MLO_IS_TRANSP(type)) 
    avt_errmsg (MBK_ERRMSG, "022", AVT_FATAL, (long)type);
  
  pttrs = (lotrs_list *)mbkalloc(sizeof(lotrs_list));
  pttrs->X    = x;
  pttrs->Y    = y;
  pttrs->WIDTH   = width;
  pttrs->LENGTH  = length;
  pttrs->MODINDEX = EMPTYHT ;
  pttrs->PS   = ps;
  pttrs->PD   = pd;
  pttrs->XS   = xs;
  pttrs->XD   = xd;
  pttrs->TYPE   = type;
  pttrs->NEXT   = ptfig->LOTRS;
  ptfig->LOTRS   = pttrs;
  pttrs->USER   = NULL;

  ptcon = (locon_list *)mbkalloc(sizeof(locon_list));
  ptcon->NAME   = namealloc("grid");
  ptcon->SIG    = ptgrid;
  ptcon->ROOT   = (void *)pttrs;
  ptcon->TYPE   = 'T';
  ptcon->DIRECTION = 'I';
  ptcon->USER   = NULL;
  ptcon->PNODE  = NULL;
  pttrs->GRID   = ptcon;
	
  ptcon = (locon_list *)mbkalloc(sizeof(locon_list));
  ptcon->NAME   = namealloc("drain");
  ptcon->SIG    = ptdrain;
  ptcon->ROOT   = (void *)pttrs;
  ptcon->TYPE   = 'T';
  ptcon->DIRECTION = 'I';
  ptcon->USER   = NULL;
  ptcon->PNODE  = NULL;
  pttrs->DRAIN    = ptcon;
	
  ptcon = (locon_list *)mbkalloc(sizeof(locon_list));
  ptcon->NAME   = namealloc("source");
  ptcon->SIG    = ptsource;
  ptcon->ROOT   = (void *)pttrs;
  ptcon->TYPE   = 'T';
  ptcon->DIRECTION = 'I';
  ptcon->USER   = NULL;
  ptcon->PNODE  = NULL;
  pttrs->SOURCE   = ptcon;
	
  ptcon = (locon_list *)mbkalloc(sizeof(locon_list));
  ptcon->NAME   = namealloc("bulk");
  ptcon->SIG    = ptbulk;
  ptcon->ROOT   = (void *)pttrs;
  ptcon->TYPE   = 'T';
  ptcon->DIRECTION = 'I';
  ptcon->USER   = NULL;
  ptcon->PNODE  = NULL;
  pttrs->BULK   = ptcon;

  /* No check is done for transistor name */
  pttrs->TRNAME = name;

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout,
                  "--- mbk --- addlotrs  : %s  X=%ld, Y=%ld, W=%ld, L=%ld, PS = %ld, PD = %ld, XS = %ld, XD = %ld\n",
                  MLO_IS_TRANSN(type) ? "TRANSN" : "TRANSP", x, y, width, length,
                  ps, pd, xs, xd);

  return pttrs;
}

/*******************************************************************************
 * function addlotrsparam()                                                     *
 *******************************************************************************/
void
addlotrsparam(lotrs_list *ptlotrs, char *param, float value, char *expr)
{
    optparam_list *newparam;
    ptype_list    *ptuser;
    
    newparam = makelotrsparam(NULL, param, value, expr); 

    ptuser = getptype(ptlotrs->USER, OPT_PARAMS);
    if (ptuser == NULL) {
        ptlotrs->USER = addptype(ptlotrs->USER, OPT_PARAMS, newparam);
    }
    else {
        newparam->NEXT = ptuser->DATA;
        ptuser->DATA = newparam;
    }
}

/*******************************************************************************
 * function makelotrsparam()                                                     *
 *******************************************************************************/

optparam_list *
makelotrsparam(optparam_list *ptheadparam, char *param, float value, char *expr)
{
    optparam_list *newparam;
    char         **known;
    char           buf[256];
    
    newparam = (optparam_list *)mbkalloc(sizeof(optparam_list)); 
    newparam->TAG = ' ' ;
    newparam->NEXT = ptheadparam;
    if ((known = getknowntrsparam(param)) != NULL) {
        if (expr != NULL) known++;
        newparam->UNAME.STANDARD = known;
    }
    else {
           newparam->TAG=expr?'$':' ';

           downstr(param, buf);
        
           newparam->UNAME.SPECIAL = namealloc(buf);
    }
    if (expr) newparam->UDATA.EXPR = expr;
    else newparam->UDATA.VALUE = value;

    newparam->EQT_EXPR=NULL;
    return newparam;
}

/*******************************************************************************
 * function getlotrsparam()                                                     *
 *******************************************************************************/
float
getlotrsparam(lotrs_list *ptlotrs, void *param, char **expr, int *status)
{
    char           buf[256];
    ptype_list    *ptuser;
    optparam_list *ptparamlist;
    char         **testknown;
    char          *testunknown;


    ptuser = getptype(ptlotrs->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        if (isknowntrsparam(param)) {
            testknown = param;
            for (ptparamlist = (optparam_list *)ptuser->DATA; ptparamlist; ptparamlist = ptparamlist->NEXT) {
                if (ptparamlist->UNAME.STANDARD == testknown) {
                    if (expr) *expr = NULL;
                    if (status) *status = 1;
                    return ptparamlist->UDATA.VALUE;
                }
                else if (ptparamlist->UNAME.STANDARD == testknown+1) {
                    if (expr) *expr = ptparamlist->UDATA.EXPR;
                    if (status) *status = 2;
                    return 0.0;
                }
            }
        }
        else {
            downstr((char *)param, buf);
            testunknown = namealloc(buf);
            for (ptparamlist = (optparam_list *)ptuser->DATA; ptparamlist; ptparamlist = ptparamlist->NEXT) {
                if (ptparamlist->TAG==' ' && ptparamlist->UNAME.SPECIAL == testunknown) {
                    if (expr) *expr = NULL;
                    if (status) *status = 1;
                    return ptparamlist->UDATA.VALUE;
                }
                else if (ptparamlist->TAG=='$' && ptparamlist->UNAME.SPECIAL == testunknown) {
                    if (expr) *expr = ptparamlist->UDATA.EXPR;
                    if (status) *status = 2;
                    return 0.0;
                }
            }
        }

    }

    if (expr) *expr = NULL;
    if (status) *status = 0;
    return 0.0;
}

/*******************************************************************************
 * function freelotrsparams()                                                  *
 *******************************************************************************/
void
freelotrsparams(lotrs_list *ptlotrs)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptlotrs->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        freeoptparams(ptuser->DATA);
        /*
        optparam_list *ptparam;
        optparam_list *ptnextparam;
        for (ptparam = (optparam_list *)ptuser->DATA; ptparam; ptparam = ptnextparam) {
            ptnextparam = ptparam->NEXT;
            mbkfree(ptparam);
        }*/
        ptlotrs->USER = delptype(ptlotrs->USER, OPT_PARAMS);
    }
}

/*******************************************************************************
 * function addlotrsmodel()                                                     *
 *******************************************************************************/
short addlotrsmodel(lotrs_list *lotrs, char *model)
{
  char *pt ;
  short index ;

  if(VT_MODNAMEINDEX == NULL) 
    VT_MODNAMEINDEX = addvt(10) ;
  if(HT_MODINDEXNAME == NULL)
    HT_MODINDEXNAME = addht(10) ;

  pt = namealloc(model) ;
  index = (short )gethtitem(HT_MODINDEXNAME,pt) ;

  if((index == (short)EMPTYHT) || (index == (short)DELETEHT))
    {
      index = (short)addvtitem(VT_MODNAMEINDEX,pt) ;
      addhtitem(HT_MODINDEXNAME,pt,(long)index) ;
    }
  if (lotrs) lotrs->MODINDEX = index ;
  return index;
}
 
/*******************************************************************************
 * function getlotrsmodel()                                                     *
 *******************************************************************************/
char *getlotrsmodel(lotrs_list *lotrs)
{
  if((VT_MODNAMEINDEX == NULL) || (HT_MODINDEXNAME == NULL) ||
     (lotrs->MODINDEX == EMPTYHT))
    {
      if(TN_MODEL_NAME == NULL)
        TN_MODEL_NAME =  namealloc("tn") ;

      if(TP_MODEL_NAME == NULL)
        TP_MODEL_NAME =  namealloc("tp") ;

      return((MLO_IS_TRANSN(lotrs->TYPE)) ? TN_MODEL_NAME : TP_MODEL_NAME) ;
    }

  return((char *)getvtitem(VT_MODNAMEINDEX,lotrs->MODINDEX)) ;
}

/*******************************************************************************
 * function addloins()                                                          *
 *******************************************************************************/
loins_list *addloins(lofig_list *ptfig, char *insname, lofig_list *ptnewfig, chain_list *sigchain)
{
  locon_list *ptcon = NULL;
  locon_list *ptnewcon = NULL;
  loins_list *ptins = NULL;
  char *figname;
  chain_list *ptchain;

  insname = namealloc(insname);
  figname = ptnewfig->NAME;

  /* check insname  unicity */
  if (strcmp(insname, "*") && FAST_MODE != 'Y') {
    for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT) {
      if (ptins->INSNAME == insname) {
        avt_errmsg (MBK_ERRMSG, "023", AVT_FATAL, insname, ptfig->NAME);
      }
    }
  }

  if (ptfig->NAME == figname)
        avt_errmsg (MBK_ERRMSG, "024", AVT_FATAL, figname);

  ptins = (loins_list *)mbkalloc(sizeof(loins_list));

  ptins->INSNAME  = insname;
  ptins->FIGNAME  = figname;
  ptins->LOCON   = NULL;
  ptins->USER    = NULL;
  ptins->NEXT    = ptfig->LOINS;
  ptfig->LOINS   = ptins;

  /* update model list   */
  for (ptchain = ptfig->MODELCHAIN; ptchain; ptchain = ptchain->NEXT)
    if (ptchain->DATA == (void *)figname)
      break;

  if (!ptchain)
    ptfig->MODELCHAIN = addchain(ptfig->MODELCHAIN, (void *)figname);

  for (ptcon = ptnewfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
    if (sigchain == NULL) 
        avt_errmsg (MBK_ERRMSG, "025", AVT_FATAL, ptnewfig->NAME, insname, ptfig->NAME);
    
    ptnewcon = (locon_list *)mbkalloc(sizeof(locon_list));
    ptnewcon->NAME = ptcon->NAME;
    ptnewcon->DIRECTION = ptcon->DIRECTION;
    ptnewcon->TYPE = 'I';
    ptnewcon->SIG  = (losig_list *)sigchain->DATA;
    ptnewcon->ROOT = (void *)ptins;
    ptnewcon->USER = NULL;
    ptnewcon->PNODE= NULL;
    ptnewcon->NEXT = ptins->LOCON;
    ptins->LOCON  = ptnewcon;
    sigchain = sigchain->NEXT;
  }
  if (sigchain != NULL) 
        avt_errmsg (MBK_ERRMSG, "025", AVT_FATAL, ptnewfig->NAME, insname, ptfig->NAME);

  ptins->LOCON = (locon_list *)reverse((chain_list *)ptins->LOCON);

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- addloins  : %s of figure %s\n", 
                  insname, figname);
  return ptins;
}

/*******************************************************************************
 * function addloinsparam()                                                     *
 *******************************************************************************/
void
addloinsparam(loins_list *ptloins, char *param, float value, char *expr)
{
    optparam_list *newparam;
    ptype_list    *ptuser;
    
    newparam = (optparam_list *)mbkalloc(sizeof(optparam_list)); 
    newparam->NEXT = NULL;

    newparam->TAG=expr?'$':' ';

    newparam->UNAME.SPECIAL = namealloc(param);
    if (expr) newparam->UDATA.EXPR = expr;
    else newparam->UDATA.VALUE = value;
    newparam->EQT_EXPR=NULL;
    ptuser = getptype(ptloins->USER, OPT_PARAMS);
    if (ptuser == NULL) {
        ptloins->USER = addptype(ptloins->USER, OPT_PARAMS, newparam);
    }
    else {
        newparam->NEXT = ptuser->DATA;
        ptuser->DATA = newparam;
    }
}

/*******************************************************************************
 * function getloinsparam()                                                     *
 *******************************************************************************/
float
getloinsparam(loins_list *ptloins, void *param, char **expr, int *status)
{
    char           buf[256];
    ptype_list    *ptuser;
    optparam_list *ptparamlist;
    char          *testunknown;


    ptuser = getptype(ptloins->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        downstr((char *)param, buf);
        testunknown = namealloc(buf);
        for (ptparamlist = (optparam_list *)ptuser->DATA; ptparamlist; ptparamlist = ptparamlist->NEXT) {
            if (ptparamlist->TAG==' ' && ptparamlist->UNAME.SPECIAL == testunknown) {
                if (expr) *expr = NULL;
                if (status) *status = 1;
                return ptparamlist->UDATA.VALUE;
            }
            else if (ptparamlist->TAG=='$' && ptparamlist->UNAME.SPECIAL == testunknown) {
                if (expr) *expr = ptparamlist->UDATA.EXPR;
                if (status) *status = 2;
                return 0.0;
            }
        }
    }

    if (expr) *expr = NULL;
    if (status) *status = 0;
    return 0.0;
}

/*******************************************************************************
 * function freeloinsparams()                                                  *
 *******************************************************************************/
void
freeloinsparams(loins_list *ptloins)
{
    optparam_list *ptparam;
    optparam_list *ptnextparam;
    ptype_list    *ptuser;

    ptuser = getptype(ptloins->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        for (ptparam = (optparam_list *)ptuser->DATA; ptparam; ptparam = ptnextparam) {
            ptnextparam = ptparam->NEXT;
            mbkfree(ptparam);
        }
        ptloins->USER = delptype(ptloins->USER, OPT_PARAMS);
    }
}

/*******************************************************************************
 * function addlocon()                                                          *
 *******************************************************************************/
locon_list *addlocon( lofig_list *ptfig, char *name, losig_list *ptsig, char dir)
{
  locon_list *ptcon;

  name = namealloc(name);

  /* check name  unicity */
  if (FAST_MODE != 'Y') {
    for (ptcon = ptfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
      if (ptcon->NAME == name) {
        avt_errmsg (MBK_ERRMSG, "026", AVT_FATAL, name, ptfig->NAME);
      }
    }
    if (dir != 'I' && dir != 'O' && dir != 'X' && dir != 'B' 
        && dir != 'Z' && dir != 'T' && dir != 'A') {
        avt_errmsg (MBK_ERRMSG, "027", AVT_FATAL, dir, ptfig->NAME);
    }
  }

  ptcon            = (locon_list *)mbkalloc(sizeof(locon_list));
  ptcon->NAME      = name;
  ptcon->TYPE      = 'E';
  ptcon->SIG       = ptsig;
  ptcon->ROOT      = (void *)ptfig;
  ptcon->DIRECTION = dir;
  ptcon->USER      = NULL;
  ptcon->NEXT      = ptfig->LOCON;
  ptcon->PNODE     = NULL;
  ptfig->LOCON     = ptcon;

  if (TRACE_MODE == 'Y')
    {
      if (ptsig!=NULL)
        (void)fprintf(stdout, "--- mbk --- addlocon  : %s linked to %ld\n", 
                      name, ptsig->INDEX);
      else
        (void)fprintf(stdout, "--- mbk --- addlocon  : %s  no signal\n", name);
    }

  return ptcon;
}

/*******************************************************************************
 * function addlosig()                                                       *
 *******************************************************************************/

losig_list *addlosig(lofig_list *ptfig, long index, chain_list *namechain, char type)
{
  losig_list *ptsig;
  chain_list *ptchain;
/*  int high;
  int low;
  int i;
  int sigsize;
  losig_list *pthead;
  ptype_list *pt;

  sigsize = getsigsize( ptfig );

  low = (index % sigsize);
  high = (index / sigsize);
  for (pt = ptfig->BKSIG; pt; pt = pt->NEXT)
    if (pt->TYPE == high)
      break;
  if (pt == NULL) {
    pthead = (losig_list *)mbkalloc(sigsize * sizeof(losig_list));
    ptsig = pthead;
    for (i = 0; i < sigsize; i++) {
      ptsig->INDEX = 0;
      ptsig++;
    }
    ptfig->BKSIG = (ptype_list *)addptype(ptfig->BKSIG,
                                          (long)high, (void *)pthead);
    pt = ptfig->BKSIG;
  }
  ptsig = (losig_list *)(pt->DATA) + low;*/

  ptsig = mbk_NewBKSIG_getindex(ptfig, index);

  /* check index  unicity */
  if (ptsig->INDEX != 0L) {
        avt_errmsg (MBK_ERRMSG, "028", AVT_FATAL, index, ptfig->NAME);
  }
  for (ptchain = namechain; ptchain; ptchain = ptchain->NEXT)
    ptchain->DATA = (void *)namealloc((char *)ptchain->DATA);

  ptsig->NAMECHAIN  = namechain;
  ptsig->INDEX      = index;
  ptsig->USER       = NULL;
  ptsig->PRCN       = NULL;
  ptsig->TYPE       = type;
  ptsig->NEXT       = ptfig->LOSIG;
  ptfig->LOSIG      = ptsig;
  ptsig->RCNCACHE   = 0;
  ptsig->ALIMFLAGS  = 0;

  if (TRACE_MODE == 'Y') {
    (void)fprintf(stdout, "--- mbk --- addlosig : %ld ", index);
    if (ptsig->NAMECHAIN)
      if (ptsig->NAMECHAIN->DATA)
        (void)fprintf(stdout, "name : %s",
                      (char *)((chain_list *)(ptsig->NAMECHAIN))->DATA);
    (void)fprintf(stdout, "\n");
  }
	
  return ptsig;
}

/*******************************************************************************
* function dellofig()                                                          *
*******************************************************************************/
int dellofig(char *name)
{
  void *ptold=NULL;
  lofig_list *ptfig;
  int ret;

  name = namealloc(name);
  for (ptfig = HEAD_LOFIG; ptfig; ptfig = ptfig->NEXT) {
    if (ptfig->NAME == name)
      break;
    ptold = (void *)ptfig;
  }

  if (ptfig == NULL || islofiglocked(ptfig) )
    return 0;
    
  if (ptfig == HEAD_LOFIG) 
    HEAD_LOFIG = ptfig->NEXT;
  else 
    ((lofig_list *)ptold)->NEXT = ptfig->NEXT;
  delhtitem(HT_LOFIG, name);
  ret=freelofig(ptfig);
  if (ret && TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- dellofig  : %s\n", name);
  return ret;
}

int freelofig(lofig_list *ptfig)
{
  losig_list *ptsig;
  loins_list *ptins;
  locon_list *ptcon;
  lotrs_list *pttrs;

  void *ptold=NULL,*ptt; /* Nom sponsorise par France Telecon */


  if( ptfig && islofiglocked(ptfig) )
    return( 0 );
 
  mbk_freeparallel(ptfig);
  
  rcn_disable_cache( ptfig );
   
  freectclist( ptfig, NULL );
  for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT)
    {
      freechain(ptsig->NAMECHAIN);
      if(ptsig->PRCN)
        freelorcnet(ptsig);
      dellosiguser( ptsig );
    }
	
  for (ptins = ptfig->LOINS; ptins; ptins = (loins_list *)ptold) {
    freeloinsparams (ptins);
    delloinsuser( ptins );
    for (ptcon = ptins->LOCON; ptcon; ptcon = (locon_list *)ptt) {
      ptt = (void *)ptcon->NEXT;
      delloconuser( ptcon );
      mbkfree((void *)ptcon);
    }
    ptold = (void *)ptins->NEXT;
    mbkfree((void *)ptins);
  }

  mbk_free_NewBKSIG(&ptfig->BKSIG);
/* 
  ptype_list *pt;
  for (pt = ptfig->BKSIG; pt; pt = pt->NEXT) {
    mbkfree((void *)pt->DATA);
  }
  freeptype(ptfig->BKSIG);*/
  for (ptcon = ptfig->LOCON; ptcon; ptcon = (locon_list *)ptold) {
    ptold = (void *)ptcon->NEXT;
    delloconuser( ptcon );
    mbkfree((void *)ptcon);
  }


  for (pttrs = ptfig->LOTRS; pttrs; pttrs = (lotrs_list *)ptold) {
    delloconuser( pttrs->GRID );
    delloconuser( pttrs->SOURCE );
    delloconuser( pttrs->DRAIN );
    if (pttrs->BULK!=NULL)
      {
        delloconuser( pttrs->BULK );
        mbkfree((void *)pttrs->BULK);
      }
	  
    mbkfree((void *)pttrs->GRID);
    mbkfree((void *)pttrs->SOURCE);
    mbkfree((void *)pttrs->DRAIN);
    //		mbkfree((void *)pttrs->BULK);
    ptold = (void *)pttrs->NEXT;
    freelotrsparams (pttrs);
    dellotrsuser( pttrs );
    mbkfree((void *)pttrs);
  }
  dellofiguser( ptfig );
  freechain(ptfig->MODELCHAIN);
  mbkfree((void *)ptfig);
  return 1;
}

/*******************************************************************************
 * function  freelomodel()                                                      *
 *******************************************************************************/
void freelomodel(lofig_list *ptmodel)
{
  lofig_list *ptfig;
  locon_list *ptcon;
  locon_list *ptc;

  if (!ptmodel) /* nothing to be freed */
    return;

  ptfig = ptmodel->NEXT;
  while (1) { 
    for (ptcon = ptmodel->LOCON; ptcon; ptcon = ptc) {
      ptc = ptcon->NEXT;
      mbkfree((void *)ptcon);
    }
    mbkfree((void *)ptmodel);
    ptmodel = ptfig;
    if (!ptfig)
      break;
    ptfig = ptfig->NEXT;
  }

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- freelomodel\n");
}

/*******************************************************************************
 * function  dellosig()                                                         *
 *******************************************************************************/
int dellosig( lofig_list *ptfig, long index)
{
  losig_list *ptsav=NULL;
  losig_list *ptsig;

  for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
    if (ptsig->INDEX == index)
      break;
    ptsav = ptsig;
  }

  if (ptsig == NULL)
    return 0;
  else if (ptsig == ptfig->LOSIG)
    ptfig->LOSIG = ptsig->NEXT;
  else
    ptsav->NEXT = ptsig->NEXT;

  if( ptsig->PRCN )
    freelorcnet( ptsig );
  ptsig->INDEX = 0L;

  freechain(ptsig->NAMECHAIN);
  ptsig->NAMECHAIN=NULL;

  dellosiguser( ptsig );

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- dellosig  : %ld\n", index);

  return 1;
}

/*******************************************************************************
 * function dellotrs()                                                          *
 *******************************************************************************/
int dellotrs( lofig_list  *ptfig, lotrs_list  *pttrs)
{
  lotrs_list  *pt;
  lotrs_list  *ptsav=NULL;

  for (pt = ptfig->LOTRS; pt; pt = pt->NEXT) {
    if (pt == pttrs)
      break;
    ptsav = pt;
  }
  if (pt == NULL)
    return 0;
  else if (pt == ptfig->LOTRS)
    ptfig->LOTRS = pt->NEXT;
  else
    ptsav->NEXT = pt->NEXT;

  if( pt->GRID->PNODE )
    delrcnlocon( pt->GRID );
  delloconuser( pt->GRID );
  mbkfree((void *)pt->GRID);

  if( pt->SOURCE->PNODE )
    delrcnlocon( pt->SOURCE );
  delloconuser( pt->SOURCE );
  mbkfree((void *)pt->SOURCE);

  if( pt->DRAIN->PNODE )
    delrcnlocon( pt->DRAIN );
  delloconuser( pt->DRAIN );
  mbkfree((void *)pt->DRAIN);

  if (pt->BULK!=NULL)
    {
      if( pt->BULK->PNODE )
        delrcnlocon( pt->BULK );
      delloconuser( pt->BULK );
      mbkfree((void *)pt->BULK);
    }

  freelotrsparams(pt);
  dellotrsuser( pt );
  mbkfree((void *)pt);
  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- dellotrs  : \n");
  return 1;
}  

int delflaggedlotrs( lofig_list  *ptfig, char *flagname)
{
  lotrs_list  *pt, *next;
  lotrs_list  *ptsav=NULL;

  for (pt = ptfig->LOTRS; pt; pt = next) {
   next=pt->NEXT;
   if (pt->TRNAME==flagname)
   {
     if (ptsav == NULL) ptfig->LOTRS = pt->NEXT;
     else ptsav->NEXT = pt->NEXT;

     if( pt->GRID->PNODE )
       delrcnlocon( pt->GRID );
     delloconuser( pt->GRID );
     mbkfree((void *)pt->GRID);
   
     if( pt->SOURCE->PNODE )
       delrcnlocon( pt->SOURCE );
     delloconuser( pt->SOURCE );
     mbkfree((void *)pt->SOURCE);
   
     if( pt->DRAIN->PNODE )
       delrcnlocon( pt->DRAIN );
     delloconuser( pt->DRAIN );
     mbkfree((void *)pt->DRAIN);
   
     if (pt->BULK!=NULL)
       {
         if( pt->BULK->PNODE )
           delrcnlocon( pt->BULK );
         delloconuser( pt->BULK );
         mbkfree((void *)pt->BULK);
       }
   
     freelotrsparams(pt);
     dellotrsuser( pt );
     mbkfree((void *)pt);
   }
   else ptsav=pt;
  }
  return 1;
}  

/*******************************************************************************
 * function delloins()                                                          *
 *******************************************************************************/
int delloins( lofig_list *ptfig, char *insname)
{
  loins_list *ptins;
  locon_list *ptcon;
  chain_list *ptchain;
  chain_list *pttmpchain=NULL;
  void    *pt=NULL;
  char *figname;

  insname = namealloc(insname);
  for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT) {
    if (ptins->INSNAME == insname)
      break;
    pt = (void *)ptins;
  }
  if (ptins == NULL)   
    return 0;
  if (ptins == ptfig->LOINS) {
    figname = ptins->FIGNAME;
    ptfig->LOINS = ptins->NEXT;
  } else {
    figname = ptins->FIGNAME;
    ((loins_list *)pt)->NEXT = ptins->NEXT;
  }
  for (ptcon = ptins->LOCON; ptcon; ptcon = (locon_list *)pt) {
    if( ptcon->PNODE )
      delrcnlocon( ptcon );
    pt = (void *)ptcon->NEXT;
    delloconuser( ptcon );
    mbkfree((void *)ptcon);
  }

  delloinsuser( ptins );
  mbkfree((void *)ptins);

  for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT)
    if (ptins->FIGNAME == figname)
      break;
  if (ptins == NULL) { /* if no more instance have this model, destroy */
    for (ptchain = ptfig->MODELCHAIN; ptchain; ptchain = ptchain->NEXT) {
      if (ptchain->DATA == (void *)figname)
        break;
      pttmpchain = ptchain;
    }
    if (ptchain == ptfig->MODELCHAIN && ptchain != NULL)
      ptfig->MODELCHAIN = ptchain ->NEXT;
    else if (ptchain)
      pttmpchain->NEXT = ptchain->NEXT;

    if( ptchain )
      {
        ptchain->NEXT = NULL;
        freechain( ptchain );
      }
  }

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- delloins  : %s\n", insname);
  return 1;
}

int delflaggedloins(lofig_list *ptfig)
{
  loins_list *ptins;
  locon_list *ptcon, *nextlocon;
  chain_list *newmodelchain=NULL;
  loins_list *pt=NULL, *next;
  int cnt=0;
  ht *tmodel;

  tmodel = addht(1000);

  for (ptins = ptfig->LOINS; ptins; ptins = next) 
    {
      next=ptins->NEXT;
      if (ptins->INSNAME == NULL)
        {
          if (pt == NULL) ptfig->LOINS = ptins->NEXT;
          else pt->NEXT=ptins->NEXT;
          for (ptcon = ptins->LOCON; ptcon; ptcon = nextlocon) 
            {
              nextlocon=ptcon->NEXT;
              if( ptcon->PNODE ) delrcnlocon( ptcon );
              delloconuser( ptcon );
              mbkfree((void *)ptcon);
            }
          delloinsuser( ptins );
          freeptype(ptins->USER);
          if (TRACE_MODE == 'Y')
            (void)fprintf(stdout, "--- mbk --- delloins\n");
          mbkfree((void *)ptins);
          cnt++;
        }
      else 
        {
          if( gethtitem( tmodel, ptins->FIGNAME ) == EMPTYHT ) {
            newmodelchain=addchain(newmodelchain, ptins->FIGNAME);
            addhtitem( tmodel, ptins->FIGNAME, 1l );
          }
          /*
          for (ptchain = newmodelchain; ptchain && ptchain->DATA!=ptins->FIGNAME; ptchain = ptchain->NEXT) ;
          if (ptchain==NULL) newmodelchain=addchain(newmodelchain, ptins->FIGNAME);
          */
          pt = ptins;
        }
  }

  delht( tmodel );
  freechain(ptfig->MODELCHAIN);
  ptfig->MODELCHAIN=newmodelchain;

  return cnt;
}
/*******************************************************************************
 * function dellocon()                                                          *
 *******************************************************************************/
int dellocon( lofig_list *ptfig, char *name)
{
  locon_list *ptcon;
  locon_list *ptsav=NULL;

  name = namealloc(name);
  for (ptcon = ptfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
    if (ptcon->NAME == name)
      break;
    ptsav = ptcon;
  }
  if (ptcon == NULL)    
    return 0;
  else if (ptcon == ptfig->LOCON)   
    ptfig->LOCON = ptcon->NEXT;
  else 
    ptsav->NEXT = ptcon->NEXT;

  if( ptcon->PNODE )
    delrcnlocon( ptcon );

  delloconuser( ptcon );
  mbkfree((void *)ptcon);
  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- mbk --- dellocon  : %s\n", name);

  return 1;
}

/*******************************************************************************
 * function getlomodel                              *
 * gives a pointer to a model or NULL if it doesn't exist             *
 *******************************************************************************/
lofig_list *getlomodel( lofig_list *ptmodel, char *name)
{
  lofig_list *ptfig;

  name = namealloc(name);
  for (ptfig = ptmodel; ptfig; ptfig = ptfig->NEXT)
    if (ptfig->NAME == name)
      return ptfig;
  return NULL;
}

/*******************************************************************************
 * function getloins()                                                          *
 *******************************************************************************/
loins_list *getloins( lofig_list *ptfig, char *name)
{
  loins_list *ptins;

  name = namealloc(name);

  for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT)
    if (ptins->INSNAME == name)
      return ptins;

        avt_errmsg (MBK_ERRMSG, "029", AVT_FATAL, name, ptfig->NAME);
  return NULL; /* never reached */
}

/*******************************************************************************
 * function getlotrs()                                                          *
 *******************************************************************************/
lotrs_list *getlotrs( lofig_list *ptfig, char *name)
{
  lotrs_list  *pttrs;

  name = namealloc(name);

  for (pttrs = ptfig->LOTRS; pttrs; pttrs = pttrs->NEXT)
    if (pttrs->TRNAME == name)
      return pttrs;

        avt_errmsg (MBK_ERRMSG, "030", AVT_FATAL, name, ptfig->NAME);
  return NULL; /* never reached */
}

/*******************************************************************************
 * function getlocon()                                                          *
 *******************************************************************************/
locon_list *getlocon( lofig_list *ptfig, char *name)
{
  locon_list  *ptcon;

  name = namealloc(name);

  for (ptcon = ptfig->LOCON; ptcon; ptcon = ptcon->NEXT)
    if (ptcon->NAME == name)
      return ptcon;

        avt_errmsg (MBK_ERRMSG, "031", AVT_FATAL, name, ptfig->NAME);
  return NULL; /* never reached */
}

/*******************************************************************************
 * function getlosig()                                                          *
 *******************************************************************************/
losig_list *getlosig(lofig_list *ptfig, long index)
{

/*
  losig_list *ptsig;
  ptype_list *pt;
  int low;
  int high;
  int sigsize;

  sigsize = getsigsize( ptfig );
  low = (index % sigsize);
  high = (index / sigsize);
  for (pt = ptfig->BKSIG; pt; pt = pt->NEXT)
    if (pt->TYPE == high) break;
  if (pt != NULL) {
    ptsig = (losig_list *)(pt->DATA) + low;
    if (ptsig->INDEX == index)
      return ptsig;
  }
*/
  if (index<=ptfig->BKSIG.maxindex)
    return mbk_NewBKSIG_getindex(ptfig, index);

        avt_errmsg (MBK_ERRMSG, "032", AVT_FATAL, index, ptfig->NAME);
  return NULL; /* never reached */
}

/*******************************************************************************
 * function  lofigchain_local()                                                 *
 * For each signal, construct the list of connectors associated                 *
 * with the signal.(Field   USER  of the "losig" structure with                 *
 * the code "LOFIGCHAIN_LOCAL")                                                 *
 * This provide the "dual" representation of the net-list                       *
 *******************************************************************************/

void dellofigchain_local( lofig_list *ptfig)
{
  losig_list *ptsig = NULL;
  ptype_list *ptype = NULL;

  if (ptfig->MODE == 'P')
    return;

  /*  cleaning ...   */
  for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
    if (ptype != (ptype_list *)NULL) {
      freechain((chain_list *)ptype->DATA);
      ptype->DATA = (void *)NULL;
    } else
      ptsig->USER = delptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
  }
}

void lofigchain_local(lofig_list *ptfig)
{
  locon_list *ptcon = NULL;
  losig_list *ptsig = NULL;
  loins_list *ptins = NULL;
  lotrs_list *pttrs = NULL;
  ptype_list *ptype = NULL;

  if (ptfig->MODE == 'P') {
    /*
      (void)fflush(stdout);
      (void)fprintf(stderr, "*** mbk error ***\nlofigchain impossible ");
      (void)fprintf(stderr, ": figure %s is interface only\n", 
      ptfig->NAME);
      EXIT(1);
    */
    return;
  }

  /*  cleaning ...   */
  for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
    if (ptype != (ptype_list *)NULL) {
      freechain((chain_list *)ptype->DATA);
      ptype->DATA = (void *)NULL;
    } else
      ptsig->USER = addptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL, (void *)NULL);
  }

  /*  scan connector list  */
  for (ptcon=ptfig->LOCON; ptcon; ptcon=ptcon->NEXT) {
    ptsig = ptcon->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, (void *)ptcon);
  }

  /*  scan instance  list    */ 
  for (ptins =ptfig->LOINS; ptins; ptins=ptins->NEXT) {
    for (ptcon=ptins->LOCON; ptcon; ptcon=ptcon->NEXT) {
      ptsig=ptcon->SIG;
      ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
      ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                     (void *)ptcon);
    }
  }

  /*  scan transistor list   */
  for (pttrs = ptfig->LOTRS; pttrs; pttrs = pttrs->NEXT) {
    ptsig = pttrs->GRID->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                   (void *)pttrs->GRID);
    ptsig = pttrs->SOURCE->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                   (void *)pttrs->SOURCE);
    ptsig = pttrs->DRAIN->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                   (void *)pttrs->DRAIN);
    if (pttrs->BULK!=NULL)
      {
        ptsig = pttrs->BULK->SIG;
        if( ptsig ) {
          ptype = getptype(ptsig->USER, (long)LOFIGCHAIN_LOCAL);
          ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                         (void *)pttrs->BULK);
        }
      }
  }
  if (DEBUG_MODE == 'Y')
    (void)printf("--- mbk --- lofigchain_local %s\n", ptfig->NAME);
}

/*******************************************************************************
 * function addoptparam()                                                     *
 *******************************************************************************/
optparam_list *
addoptparam(optparam_list *ptheadparam, char *param, float value, char *expr)
{
    optparam_list *newparam;
    char           buf[256];
    
    newparam = (optparam_list *)mbkalloc(sizeof(optparam_list)); 
    newparam->EQT_EXPR=NULL;
    newparam->NEXT = ptheadparam;

    newparam->TAG =expr?'$':' ';
    downstr(param, buf);
    newparam->UNAME.SPECIAL = namealloc(buf);
    if (expr) newparam->UDATA.EXPR = expr;
    else newparam->UDATA.VALUE = value;

    return newparam;
}

/*******************************************************************************
 * function getoptparam()                                                     *
 *******************************************************************************/
float
getoptparam(optparam_list *ptheadparam, void *param, char **expr, int *status)
{
    char           buf[256];
    optparam_list *ptparamlist;
    char          *testunknown;


    if (ptheadparam != NULL) {
        downstr((char *)param, buf);
        testunknown = namealloc(buf);
        for (ptparamlist = ptheadparam; ptparamlist; ptparamlist = ptparamlist->NEXT) {
            if (ptparamlist->TAG==' ' && ptparamlist->UNAME.SPECIAL == testunknown) {
                if (expr) *expr = NULL;
                if (status) *status = 1;
                return ptparamlist->UDATA.VALUE;
            }
            else if (ptparamlist->TAG=='$' && ptparamlist->UNAME.SPECIAL == testunknown) {
                if (expr) *expr = ptparamlist->UDATA.EXPR;
                if (status) *status = 2;
                return 0.0;
            }
        }
    }

    if (expr) *expr = NULL;
    if (status) *status = 0;
    return 0.0;
}

/*******************************************************************************
 * function freeoptparams()                                                  *
 *******************************************************************************/
void
freeoptparams(optparam_list *ptheadparam)
{
    optparam_list *ptparam;
    optparam_list *ptnextparam;

    for (ptparam = ptheadparam; ptparam; ptparam = ptnextparam) {
        ptnextparam = ptparam->NEXT;        
        if (ptparam->EQT_EXPR!=NULL && --((eqt_node *)ptparam->EQT_EXPR)->REF_COUNT==0) 
          eqt_freenode(ptparam->EQT_EXPR);
        mbkfree(ptparam);
    }
}
  
/*******************************************************************************
 * function  lofigchain()                                                       *
 * For each signal, construct the list of connectors associated                 *
 * with the signal.(Field   USER  of the "losig" structure with                 *
 * the code "LOFIGCHAIN")                                                       *
 * This provide the "dual" representation of the net-list                       *
 *******************************************************************************/
void lofigchain(lofig_list *ptfig)
{
  locon_list *ptcon = NULL;
  losig_list *ptsig = NULL;
  loins_list *ptins = NULL;
  lotrs_list *pttrs = NULL;
  ptype_list *ptype = NULL;

  if (ptfig->MODE == 'P') {
    /*
      (void)fflush(stdout);
      (void)fprintf(stderr, "*** mbk error ***\nlofigchain impossible ");
      (void)fprintf(stderr, ": figure %s is interface only\n", 
      ptfig->NAME);
      EXIT(1);
    */
    return;
  }

  /*  cleaning ...   */
  for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
    if (ptype != (ptype_list *)NULL) {
      freechain((chain_list *)ptype->DATA);
      ptype->DATA = (void *)NULL;
    } else
      ptsig->USER = addptype(ptsig->USER, (long)LOFIGCHAIN, (void *)NULL);
  }

  /*  scan connector list  */
  for (ptcon=ptfig->LOCON; ptcon; ptcon=ptcon->NEXT) {
    ptsig = ptcon->SIG;
    if(ptsig) {
      ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
      ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, (void *)ptcon);
    }
  }

  /*  scan instance  list    */ 
  for (ptins =ptfig->LOINS; ptins; ptins=ptins->NEXT) {
    for (ptcon=ptins->LOCON; ptcon; ptcon=ptcon->NEXT) {
      ptsig=ptcon->SIG;
      if(ptsig){
        ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
        ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                       (void *)ptcon);
      }
    }
  }

  /*  scan transistor list   */
  for (pttrs = ptfig->LOTRS; pttrs; pttrs = pttrs->NEXT) {
    ptsig = pttrs->GRID->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                   (void *)pttrs->GRID);
    ptsig = pttrs->SOURCE->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                   (void *)pttrs->SOURCE);
    ptsig = pttrs->DRAIN->SIG;
    ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
    ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                   (void *)pttrs->DRAIN);
    if (pttrs->BULK!=NULL)
      {
        ptsig = pttrs->BULK->SIG;
        if( ptsig ) {
          ptype = getptype(ptsig->USER, (long)LOFIGCHAIN);
          ptype->DATA = (void *)addchain((chain_list *)ptype->DATA, 
                                         (void *)pttrs->BULK);
        }
      }
  }
  if (DEBUG_MODE == 'Y')
    (void)printf("--- mbk --- lofigchain %s\n", ptfig->NAME);
}

/*******************************************************************************
 * function getsigname()                                                        *
 * choose the least concatened signal name                                      *
 *******************************************************************************/
char *getsigname(losig_list *ptsig)
{
  chain_list *ptscan;
  char *ptchar;
  char buffer[50];
  char *result = NULL, *newc;
  register int nseparmin = 1000;
  int l0, l1;

  if (ptsig == NULL)
    return NULL;

  for (ptscan = ptsig->NAMECHAIN; ptscan; ptscan = ptscan->NEXT) {
    register int nsepar = 0;
    newc=(char *)ptscan->DATA;
    if (newc != NULL) {
      for (ptchar = newc; *ptchar != '\0'; ptchar++)
        if (*ptchar == SEPAR)
          nsepar++;
      if (nsepar < nseparmin) {
        nseparmin = nsepar;
        result = newc;
      }
      else if (nsepar == nseparmin) {
        if ((l0=strlen(newc)) < (l1=strlen(result)) || (l0==l1 && mbk_casestrcmp(newc,result)<0)) {
          result = newc;
        }
      }
    }
  }
  if (!result) {
    (void)sprintf(buffer, "mbk_%ld", ptsig->INDEX);
    result = namealloc(buffer);
  }
  return result;
}

/*******************************************************************************
 * function mbkisgnalname()                                                     *
 * Return 1 if signal is named 'name', 0 else. name must be nameallocated.      *
 *******************************************************************************/
int mbkissignalname(losig_list *losig, char *name)
{
  chain_list *chain ;
  
  if( !losig )
    return 0;

  for( chain = losig->NAMECHAIN ; chain ; chain = chain->NEXT ) {
    if( (char*)chain->DATA == name )
      return 1 ;
  }

  return 0;
}

/*******************************************************************************
 * function viewlo()                                                            *
 * display on screen the content of all logical figures                         *
 *******************************************************************************/
void viewlo()
{
  lofig_list  *ptfig;

  if (HEAD_LOFIG == NULL) {
        avt_errmsg (MBK_ERRMSG, "033", AVT_ERROR);
    return;
  } else for (ptfig = HEAD_LOFIG; ptfig; ptfig = ptfig->NEXT)
    viewlofig(ptfig);
}

/*******************************************************************************
 * function viewlofig()                                                         *
 * display on screen the content of logical figure ptfig                        *
 *******************************************************************************/
void viewlofig(lofig_list *ptfig)
{
  locon_list *ptcon;
  losig_list *ptsig;
  loins_list *ptins;
  lotrs_list *pttrs;
  chain_list *scanchain;
  ptype_list *pt;

  if (ptfig->NAME == NULL)
    (void)printf("!!! figure without name !!! /n");
  else
    (void)printf("\nfigure   : %-20s  mode : %c\n", ptfig->NAME, ptfig->MODE);

  if (ptfig->LOCON == NULL) 
    (void)printf("   |---empty connector list\n");   
  else 
    {
      for (ptcon = ptfig->LOCON; ptcon; ptcon = ptcon->NEXT)
        viewlofigcon(ptcon);

      pt = getptype( ptfig->USER, PH_INTERF );
      if( pt )
        {
          (void)printf("   |---connector list physical order\n");   
          for( scanchain = (chain_list*)(pt->DATA); scanchain ; scanchain = scanchain->NEXT )
            (void)printf("   |    |---%s\n",(char*)(scanchain->DATA) );   
          (void)printf("   |\n");   
        }
    }

  if (ptfig->LOSIG == NULL)
    (void)printf("   |---empty signal list\n");
  else for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT)
    viewlosig(ptsig);

  if (ptfig->LOINS == NULL)
    (void)printf("   |---empty instance list\n");
  else for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT)
    viewloins(ptins);

  if (ptfig->LOTRS == NULL)
    (void)printf("   |---empty transistor list\n");
  else for (pttrs = ptfig->LOTRS; pttrs; pttrs = pttrs->NEXT)
    viewlotrs(pttrs);

  if (ptfig->USER != NULL)
    {
      (void)printf("   |---USER field\n");
      for( pt = ptfig->USER ; pt ; pt = pt->NEXT )
        (void)printf("   |    |-- %ld\n",pt->TYPE);
    }
  else
    (void)printf("   |---empty USER field\n");

  (void)printf("   | \n");
}   

/*******************************************************************************
 * function viewlofigcon()                                                      *
 *******************************************************************************/
void viewlofigcon(locon_list *ptcon)
{
  num_list	*scannum;
  ptype_list      *scanptype;
  chain_list      *scanchain;

  if (ptcon->NAME == NULL)
    (void)printf("   |--- !!! connector without name !!!\n");
  else
    (void)printf("   |---connector  : \"%s\"\n", ptcon->NAME);
  (void)printf("   |    |---direction  : %c\n", ptcon->DIRECTION);
  if (ptcon->SIG == NULL)
    (void)printf("   |    |---no signal\n");
  else
    (void)printf("   |    |---signal    : %ld\n", ptcon->SIG->INDEX);
  if (ptcon->ROOT == NULL)
    (void)printf("   |---no root\n");
  else
    (void)printf("   |    |---root    : %s\n",
                 ((char *)(((lofig_list *)(ptcon->ROOT))->NAME)));
  (void)printf("   |    |---type    : %c \n", ptcon->TYPE);

  (void)printf("   |    |---USER list\n");
  for( scanptype = ptcon->USER ; scanptype ; scanptype = scanptype->NEXT )
    {
      switch( scanptype->TYPE )
        {
        case PNODENAME:
          (void)printf("   |    |   |---PNODENAME\n");
          for( scanchain = (chain_list*)(scanptype->DATA);
               scanchain;
               scanchain = scanchain->NEXT )
            (void)printf("   |    |   |   |---%s\n", (char*)(scanchain->DATA)?(char*)(scanchain->DATA):"NULL" );
          break;
        default:
          (void)printf("   |    |    |   |---%ld\n",scanptype->TYPE);
        }
    }

  if(ptcon->PNODE == NULL)
    (void)printf("   |    |---no node\n");
  else {
    (void)printf("   |    |---node\n");
    for(scannum = ptcon->PNODE; scannum; scannum = scannum->NEXT )
      (void)printf("   |    |    |---%ld\n",scannum->DATA);

  }

  (void)printf("   |\n");
}

/*******************************************************************************
 * function viewlosig()                                                         *
 *******************************************************************************/
void viewlosig_sub(losig_list *ptsig, int resume)
{ 
  chain_list  *ptchain;
  lowire_list *scanwire;
  chain_list  *scanctc;
  loctc_list  *ptctc;
  char         levelrc;
  ptype_list  *ptptype;
  num_list    *scannum;

  (void)printf("   |---signal    : %ld \n" , ptsig->INDEX);
  if (ptsig->NAMECHAIN == NULL)
    (void)printf("   |    |---no alias names\n");
  else for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {  
    if (ptchain->DATA == NULL)
      (void)printf("   |    |--- !!! empty name !!!\n");
    else
      (void)printf("   |    |---alias   : \"%s\"\n", (char *)ptchain->DATA);
  }
  (void)printf("   |    |---type    : %c\n" , ptsig->TYPE);

  levelrc = rclevel( ptsig );
	
  if( levelrc == MBK_RC_A )
    (void)printf("   |    |---no parasitic data\n");
  else {
    (void)printf("   |    |---parasitic data\n");
    (void)printf("   |    |    |---capacitance : %g\n" , rcn_getcapa(NULL, ptsig));
    if( levelrc == MBK_RC_C || levelrc == MBK_RC_E ) {
      (void)printf("   |    |    |---wire list\n");
      for( scanwire = ptsig->PRCN->PWIRE ; scanwire != NULL ; scanwire = scanwire->NEXT )
        (void)printf("   |    |    |    |-n1=%3ld n2=%3ld r=%g c=%g\n",
                     scanwire->NODE1,
                     scanwire->NODE2,
                     scanwire->RESI,
                     scanwire->CAPA   );
    }
    else
      (void)printf("   |    |    |---no wire\n");
    if( levelrc == MBK_RC_D || levelrc == MBK_RC_E ) {
      (void)printf("   |    |    |---crosstalk capacitance list\n");
      for( scanctc = ptsig->PRCN->PCTC ; scanctc != NULL ; scanctc = scanctc->NEXT ) {
        ptctc = (loctc_list*)scanctc->DATA;
        (void)printf("   |    |    |    |-S1=%3ld N1=%3ld S2=%3ld N2=%3ld Capa=%g.\n", 
                     ptctc->SIG1->INDEX,
                     ptctc->NODE1,
                     ptctc->SIG2->INDEX,
                     ptctc->NODE2,
                     ptctc->CAPA
                     );
      }
    }
    else
      (void)printf("   |    |    |---no crosstalk capacitance\n");
    (void)printf("   |    |    |---%ld node\n",ptsig->PRCN->NBNODE);
  }
  (void)printf("   |    |---USER list\n");
  for( ptptype = ptsig->USER ; ptptype ; ptptype = ptptype->NEXT )
    {
      switch( ptptype->TYPE )
        {
        case LOFIGCHAIN:
          printf( "   |    |   |-LOFIGCHAIN\n" );
          for( ptchain = (chain_list*)ptptype->DATA ; ptchain ; ptchain = ptchain->NEXT )
            {
              printf( "   |    |   |   |-%s ",((locon_list*)ptchain->DATA)->NAME);
              switch( ((locon_list*)ptchain->DATA)->TYPE )
                {
                case 'T':
                  printf( "(T%c : %s) ",MLO_IS_TRANSN(((lotrs_list*)((locon_list*)ptchain->DATA)->ROOT)->TYPE)?'N':'P',((lotrs_list*)((locon_list*)ptchain->DATA)->ROOT)->TRNAME ? ((lotrs_list*)((locon_list*)ptchain->DATA)->ROOT)->TRNAME : "No name" );
                  break;
                case 'I':
                  printf( "(I : %s) ",((loins_list*)((locon_list*)ptchain->DATA)->ROOT)->INSNAME ? ((loins_list*)((locon_list*)ptchain->DATA)->ROOT)->INSNAME : "No name" );
                  break;
                case 'E':
                  printf( "(Figure) " );
                  break;
                default:
                  printf( "Not a valid type('%c') ", ((locon_list*)ptchain->DATA)->TYPE);
                }
              if (!resume)
              {
                for( scannum = ((locon_list*)ptchain->DATA)->PNODE ; scannum ; scannum = scannum->NEXT )
                  printf( "%ld ", scannum->DATA );
              }
              printf( "\n" );
            }
          break;
        default:
          break;
        }
    }
  {
  }
  (void)printf("   | \n");
}

void viewlosig(losig_list *ptsig)
{
 viewlosig_sub(ptsig, 0);
}

/*******************************************************************************
 * function viewloins()                                                         *
 *******************************************************************************/
void viewloins(loins_list *ptins)
{
  locon_list  *ptcon;
  chain_list *scanchain;
  ptype_list *pt;

  if (ptins->INSNAME == NULL)
    (void)printf("   |--- !!! no instance name !!!\n");
  else
    (void)printf("   |---instance   : %s\n", ptins->INSNAME);
  if (ptins->FIGNAME == NULL)
    (void)printf("   |   |--- !!! no instance model	!!!\n");
  else
    (void)printf("   |   |---model     : %s \n" , ptins->FIGNAME);
  if (ptins->LOCON == NULL)
    (void)printf("   |   |---empty list of connectors\n");
  else
    {
      for (ptcon = ptins->LOCON; ptcon != NULL; ptcon = ptcon->NEXT)
        viewloinscon(ptcon);
      pt = getptype( ptins->USER, PH_INTERF );
      if( pt )
        {
          (void)printf("   |   |---connector list physical order\n");   
          for( scanchain = (chain_list*)(pt->DATA); scanchain ; scanchain = scanchain->NEXT )
            (void)printf("   |   |    |---%s\n",(char*)(scanchain->DATA) );   
          (void)printf("   |   |\n");   
        }
    }
  if (ptins->USER != NULL)
    (void)printf("   |   |---non empty USER field\n");
  (void)printf("   | \n");
}

/*******************************************************************************
 * function viewlotrs()                                                         *
 *******************************************************************************/
void viewlotrs(lotrs_list *pttrs)
{
  (void)printf("   |---transistor\n");
  if (MLO_IS_TRANSN(pttrs->TYPE))
    (void)printf("   |   |---type  : TRANSN \n");
  else if (MLO_IS_TRANSP(pttrs->TYPE))
    (void)printf("   |   |---type  : TRANSP \n");
  else 
    (void)printf("   |   |---illegal type\n");
  if (pttrs->TRNAME != NULL )
    (void)printf("   |   |---name : %s\n", pttrs->TRNAME );
  else
    (void)printf("   |   |---no name\n" );

  if (pttrs->SOURCE == NULL)
    (void)printf("   |   |--- !!! no signal on source !!! \n");
  else
    viewloinscon( pttrs->SOURCE );

  if (pttrs->GRID == NULL)
    (void)printf("   |   |--- !!! no signal on grid !!!\n");
  else
    viewloinscon( pttrs->GRID );

  if (pttrs->DRAIN == NULL)
    (void)printf("   |   |--- !!! no signal on drain !!!\n");
  else
    viewloinscon( pttrs->DRAIN );

  if (pttrs->BULK == NULL)
    (void)printf("   |   |--- !!! no signal on bulk !!!\n");
  else
    viewloinscon( pttrs->BULK );

  (void)printf("   |   |---x      : %ld\n", pttrs->X);
  (void)printf("   |   |---y      : %ld\n", pttrs->Y);
  (void)printf("   |   |---width  : %ld\n", pttrs->WIDTH);
  (void)printf("   |   |---length : %ld\n", pttrs->LENGTH);
  (void)printf("   |   |---ps     : %ld\n", pttrs->PS);
  (void)printf("   |   |---pd     : %ld\n", pttrs->PD);
  (void)printf("   |   |---xs     : %ld\n", pttrs->XS);
  (void)printf("   |   |---xd     : %ld\n", pttrs->XD);
  if (pttrs->USER != NULL)
    (void)printf("   |   |---non empty USER field\n");
  (void)printf("   |\n");
}

/*******************************************************************************
 * function viewloinscon()                                                      *
 *******************************************************************************/
void viewloinscon(locon_list *ptcon)
{
  num_list	*scannum;
  ptype_list      *scanptype;
  chain_list      *scanchain;

  if (ptcon->NAME == NULL)
    (void)printf("   |   |---no connector name\n");
  else
    (void)printf("   |   |---connector  : \"%s\"\n" , ptcon->NAME);
  (void)printf("   |   |    |---direction  : %c\n" , ptcon->DIRECTION);
  if (ptcon->SIG == NULL)
    (void)printf("   |   |    |---no signal\n");
  else
    (void)printf("   |   |    |---signal    : %ld\n" , ptcon->SIG->INDEX);
  if ((ptcon->ROOT) == NULL)
    (void)printf("   |   |    |---no root\n");
  else
    switch( ptcon->TYPE )
      {
      case 'I':
	(void)printf("   |   |    |---root    : %s\n" , ((char*)(((loins_list *)(ptcon->ROOT))->INSNAME)));
        break;
      case 'T':
	(void)printf("   |   |    |---root    : %s\n" , 
                     ((lotrs_list*)(ptcon->ROOT))->TRNAME?((lotrs_list*)(ptcon->ROOT))->TRNAME:"no transistor name" );
        break;
      default:
	(void)printf("   |   |    |---root    : *** BAD TYPE ***\n" );
      }
  (void)printf("   |   |    |---type    : %c\n" , ptcon->TYPE);
  if(ptcon->PNODE == NULL)
    (void)printf("   |   |    |---no node\n");
  else {
    (void)printf("   |   |    |---node\n");
    for(scannum = ptcon->PNODE; scannum; scannum = scannum->NEXT )
      (void)printf("   |   |    |    |---%ld\n",scannum->DATA);
  }
  (void)printf("   |   |    |---USER list\n");
  for( scanptype = ptcon->USER ; scanptype ; scanptype = scanptype->NEXT )
    {
      switch( scanptype->TYPE )
        {
        case PNODENAME:
          (void)printf("   |   |    |    |---PNODENAME\n");
          for( scanchain = (chain_list*)(scanptype->DATA);
               scanchain;
               scanchain = scanchain->NEXT )
            (void)printf("   |   |    |    |   |---%s\n", (char*)(scanchain->DATA)?(char*)(scanchain->DATA):"NULL" );
          break;
        default:
          (void)printf("   |   |    |    |---%ld\n",scanptype->TYPE);
        }
    }
  (void)printf("   |   | \n");
}

/*******************************************************************************
 * function getsigsize()                                                        *
 *******************************************************************************/

int getsigsize(lofig_list *ptfig)
{
  ptype_list    *ptl;

  ptl = getptype( ptfig->USER, PTSIGSIZE );
  if( ptl )
    return( (int)((long)(ptl->DATA)) );

  return( SIGSIZE );
}

/*******************************************************************************
 * function setsigsize()                                                        *
 *******************************************************************************/

void setsigsize(lofig_list *ptfig, int nb)
{
  ptype_list    *ptl;

  if( ptfig->BKSIG.TAB )
    {
        avt_errmsg (MBK_ERRMSG, "034", AVT_FATAL);
    }
  
  ptl = getptype( ptfig->USER, PTSIGSIZE );
  if( !ptl )
    ptfig->USER = addptype( ptfig->USER, PTSIGSIZE, (void*)((long)nb) );
  else
    ptl->DATA = (void*)((long)nb);
}


int getnumberoflosig(lofig_list *ptfig)
{

  if (!ptfig->BKSIG.TAB ) return 0;
  return ptfig->BKSIG.maxindex;
  /*
  long        max;
  long        bkmax;
  long        bkmaxmax;
  losig_list *blk;
  long        sz;
  long        i;
  ptype_list *ptl;
  sz    = getsigsize( ptfig );
  bkmax = LONG_MAX;
  max   = -1;

  do {
    bkmaxmax = bkmax;
    bkmax    = -1;

    for( ptl = ptfig->BKSIG ; ptl ; ptl = ptl->NEXT ) {
      if( ptl->TYPE > bkmax && ptl->TYPE < bkmaxmax ) {
        bkmax = ptl->TYPE;
        blk   = ptl->DATA;
      }
    }
    if( bkmax == -1 ) break;
  
    max = 0;

    for( i = 0 ; i<sz ; i++ ) {
      if( blk[i].INDEX > max )
        max = blk[i].INDEX;
    }
    
  }
  while( !max );

  return max;
  */
}

/*******************************************************************************
 * function for cleaning USER field of structures.                              *
 *******************************************************************************/

void delloconuser(locon_list *ptlocon)
{
  ptype_list *scanptype;
  ptype_list *next;

  for( scanptype = ptlocon->USER ; scanptype ; scanptype = next )
    {
      next = scanptype->NEXT;

      switch( scanptype->TYPE )
        {
        case PNODENAME:
          freechain( scanptype->DATA );
          break;
        default:
#ifdef MBK_TRACE_BAD_PTYPE
          fprintf( stderr, "WARNING in delloconuser() : unknown ptype %08X\n",
                   (unsigned int)scanptype->TYPE
                   );
#endif
          break;
        }
/*
  int         del=0;
      ptype_list *prev;
      prev = NULL;
      if( del )
        {
          if( !prev )
            ptlocon->USER = next;
          else
            prev->NEXT = next;
          scanptype->NEXT = NULL;
          freeptype( scanptype );
        }
      else
        prev = scanptype;
*/
    }
  freeptype(ptlocon->USER); ptlocon->USER=NULL;
}

//typedef struct  eqt_param eqt_param;
//extern void     eqt_free_param  (eqt_param *param);

void dellofiguser(lofig_list *ptlofig)
{
  ptype_list *scanptype;
  ptype_list *next;

  for( scanptype = ptlofig->USER ; scanptype ; scanptype = next )
    {
      next = scanptype->NEXT;

      switch( scanptype->TYPE )
        {
        case PARAM_CONTEXT:
          eqt_free_param((eqt_param*)scanptype->DATA);
          break;
        case OPT_PARAMS:
          freeoptparams(scanptype->DATA);
          break;
        case PH_REAL_INTERF:
        case PH_INTERF:
          freechain( scanptype->DATA );
          break;
        case PTSIGSIZE:
        case LOFIG_LOCK:
          break;
        case LOFIG_QUICK_SIG_HT:
        case LOFIG_QUICK_INS_HT:
        case LOFIG_QUICK_TRS_HT:
          delht_v2((ht_v2 *)scanptype->DATA );
          break;
        case LOFIG_INFO:
          mbkfree(scanptype->DATA);
          break;
        case 0x00079350: // SPEF_INDEX
          delit((it *)scanptype->DATA);
          break;
        default:
#ifdef MBK_TRACE_BAD_PTYPE
          fprintf( stderr, "WARNING in dellofiguser() : unknown ptype %08X\n",
                   (unsigned int)scanptype->TYPE
                   );
#endif
          break;
        }
/*
      if( del )
        {
          if( !prev )
            ptlofig->USER = next;
          else
            prev->NEXT = next;
          scanptype->NEXT = NULL;
          freeptype( scanptype );
        }
      else
        prev = scanptype;
*/
    }
  freeptype(ptlofig->USER); ptlofig->USER=NULL;
}

void delloinsuser(loins_list *ptloins)
{
  ptype_list *scanptype;
  ptype_list *next;

  for( scanptype = ptloins->USER ; scanptype ; scanptype = next )
    {
      next = scanptype->NEXT;

      switch( scanptype->TYPE )
        {
        case PARAM_CONTEXT:
          eqt_free_param((eqt_param*)scanptype->DATA);
          break;
        case PH_INTERF:
        case PH_REAL_INTERF: // should not happen for now, perhaps later
          freechain( scanptype->DATA );
          break;
        default:
#ifdef MBK_TRACE_BAD_PTYPE
          fprintf( stderr, "WARNING in delloinsuser() : unknown ptype %08X\n",
                   (unsigned int)scanptype->TYPE
                   );
#endif
          break;
        }
/*
 *
  ptype_list *prev;

  prev = NULL;
      int         del;
      if( del )
        {
          if( !prev )
            ptloins->USER = next;
          else
            prev->NEXT = next;
          scanptype->NEXT = NULL;
          freeptype( scanptype );
        }
      else
        prev = scanptype;
*/
    }
  freeptype(ptloins->USER); ptloins->USER=NULL;
}

void dellotrsuser(lotrs_list *ptlotrs)
{
  ptype_list *scanptype;
  ptype_list *next;

  for( scanptype = ptlotrs->USER ; scanptype ; scanptype = next )
    {
      next = scanptype->NEXT;

      switch( scanptype->TYPE )
        {
        case PARAM_CONTEXT:
            eqt_free_param((eqt_param *)scanptype->DATA);
            break;
        case OPT_PARAMS:
            freeoptparams((optparam_list *)scanptype->DATA);
            break;
        case MCC_COMPUTED_KEY:
            freechain((chain_list *)scanptype->DATA);
            break;
        default:
#ifdef MBK_TRACE_BAD_PTYPE
          fprintf( stderr, "WARNING in dellotrsuser() : unknown ptype %08X\n",
                   (unsigned int)scanptype->TYPE
                   );
#endif
          break;
        }
/*
      ptype_list *prev;
      prev = NULL; 
      int         del;
      if( del )
        {
          if( !prev )
            ptlotrs->USER = next;
          else
            prev->NEXT = next;
          scanptype->NEXT = NULL;
          freeptype( scanptype );
        }
      else
        prev = scanptype;
*/
    }
  freeptype(ptlotrs->USER); ptlotrs->USER=NULL;
}

void dellosiguser(losig_list *ptlosig)
{
  ptype_list *scanptype;
  ptype_list *next;
  chain_list *cl;

  for( scanptype = ptlosig->USER ; scanptype ; scanptype = next )
    {
      next = scanptype->NEXT;

      switch( scanptype->TYPE )
        {
        case LOSIGALIM:
          break ;
        case LOFIGCHAIN:
        case LOFIGCHAIN_LOCAL:
          freechain( scanptype->DATA );
          break;
        case MBK_VCARD_NODES:
          for (cl=(chain_list *)scanptype->DATA; cl!=NULL; cl=cl->NEXT)
            mbkfree(cl->DATA);
          freechain((chain_list *)scanptype->DATA);
          break;
        default:
#ifdef MBK_TRACE_BAD_PTYPE
          fprintf( stderr, "WARNING in dellosiguser() : unknown ptype %08X\n",
                   (unsigned int)scanptype->TYPE
                   );
#endif
          break;
        }
/*
      ptype_list *prev;
      int         del;
      if( del )
        {
          if( !prev )
            ptlosig->USER = next;
          else
            prev->NEXT = next;
          scanptype->NEXT = NULL;
          freeptype( scanptype );
        }
      else
        prev = scanptype;
*/
    }
  freeptype(ptlosig->USER); ptlosig->USER=NULL;
}

void locklofig( lofig_list *ptfig )
{
  if( !getptype( ptfig->USER, LOFIG_LOCK ) )
    ptfig->USER = addptype( ptfig->USER, LOFIG_LOCK, NULL );
}

void unlocklofig( lofig_list *ptfig )
{
  if( getptype( ptfig->USER, LOFIG_LOCK ) )
    ptfig->USER = delptype( ptfig->USER, LOFIG_LOCK );
}

int islofiglocked( lofig_list *ptfig )
{
  if( getptype( ptfig->USER, LOFIG_LOCK ) ) return 1;
  return 0;
}

losig_list* mbk_getlosigbyname( lofig_list *lofig, char *name )
{
  losig_list *losig;
  chain_list *scan;

  name = namealloc( name );

  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
    for( scan = losig->NAMECHAIN ; scan ; scan = scan->NEXT ) { 
      if( scan->DATA == name )
        return losig;
    }
  }

  return( NULL );
}

losig_list* mbk_quickly_getlosigbyname( lofig_list *lofig, char *name )
{
  losig_list *losig;
  chain_list *scan;
  ptype_list *p;
  long l;
  ht_v2 *h;

  if ((name = namefind( name ))==NULL) return NULL;

  if ((p=getptype(lofig->USER, LOFIG_QUICK_SIG_HT))==NULL)
    {
      l=(countchain((chain_list *)lofig->LOSIG)+19)/20;
      h=addht_v2(l);
      for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
        for( scan = losig->NAMECHAIN ; scan ; scan = scan->NEXT ) { 
          addhtitem_v2(h, scan->DATA, (long)losig);
        }
      }
      p=lofig->USER=addptype(lofig->USER, LOFIG_QUICK_SIG_HT, h);
    }

  if ((l=gethtitem_v2((ht_v2 *)p->DATA, name))==EMPTYHT) return NULL;
  return (losig_list *)l;
}

// for this one name must be nameallocated
loins_list* mbk_quickly_getloinsbyname( lofig_list *lofig, char *name )
{
  loins_list *li;
  ptype_list *p;
  long l;
  ht_v2 *h;

  if ((p=getptype(lofig->USER, LOFIG_QUICK_INS_HT))==NULL)
    {
      l=(countchain((chain_list *)lofig->LOINS)+19)/20;
      h=addht_v2(l);
      for( li = lofig->LOINS ; li ; li = li->NEXT ) {
          addhtitem_v2(h, li->INSNAME, (long)li);
      }
      p=lofig->USER=addptype(lofig->USER, LOFIG_QUICK_INS_HT, h);
    }

  if ((l=gethtitem_v2((ht_v2 *)p->DATA, name))==EMPTYHT) return NULL;
  return (loins_list *)l;
}

// for this one name must be nameallocated
lotrs_list* mbk_quickly_getlotrsbyname( lofig_list *lofig, char *name )
{
  lotrs_list *ptlotrs;
  ptype_list *ptuser;
  long l;
  ht_v2 *h;

  if ((ptuser=getptype(lofig->USER, LOFIG_QUICK_TRS_HT))==NULL)
    {
      l=(countchain((chain_list *)lofig->LOTRS)+19)/20;
      h=addht_v2(l);
      for( ptlotrs = lofig->LOTRS ; ptlotrs ; ptlotrs = ptlotrs->NEXT ) {
          addhtitem_v2(h, ptlotrs->TRNAME, (long)ptlotrs);
      }
      ptuser=lofig->USER=addptype(lofig->USER, LOFIG_QUICK_TRS_HT, h);
    }

  if ((l=gethtitem_v2((ht_v2 *)ptuser->DATA, name))==EMPTYHT) return NULL;
  return (lotrs_list *)l;
}

int mbk_istranscrypt(char *name)
{
  chain_list *ptchain;
    
  //    name = namealloc(name);
  for (ptchain = CRYPTMOS; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

int mbk_istransn(char *name)
{
  chain_list *ptchain;
    
  //    name = namealloc(name);
  for (ptchain = TNMOS; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

int mbk_istransp(char *name)
{
  chain_list *ptchain;
    
  //    name = namealloc(name);
  for (ptchain = TPMOS; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

char mbk_gettranstype(char *type)
{
  if (mbk_istransn(type))
    return(TRANSN) ;
  else if (mbk_istransp(type))
    return(TRANSP) ;
  else
    return(TRANSN) ;
}

int mbk_isdioden(char *name)
{
  chain_list *ptchain;

  name = namealloc(name);
  for (ptchain = DNMOS; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

int mbk_isdiodep(char *name)
{
  chain_list *ptchain;

  name = namealloc(name);
  for (ptchain = DPMOS; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

char mbk_getdiodetype(char *type)
{
  if (mbk_isdioden(type))
    return(TRANSN) ;
  else if (mbk_isdiodep(type))
    return(TRANSP) ;
  else
    return(TRANSN) ;
}

int mbk_isjfetn(char *name)
{
  chain_list *ptchain;

  name = namealloc(name);
  for (ptchain = JFETN; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

int mbk_isjfetp(char *name)
{
  chain_list *ptchain;

  name = namealloc(name);
  for (ptchain = JFETP; ptchain; ptchain = ptchain->NEXT) {
    if (ptchain->DATA == name) return 1;
  }
  return 0;
}

char mbk_getjfettype(char *type)
{
  if (mbk_isjfetn(type))
    return(TRANSN) ;
  else if (mbk_isjfetp(type))
    return(TRANSP) ;
  else
    return(TRANSN) ;
}

/*******************************************************************************

Fonction de positionnement d'un offset pour l'acces direct dans des fichiers 
pour des losig.

*******************************************************************************/

void mbk_losig_setfilepos( lofig_list     *lofig, 
                           losig_list     *losig, 
                           FILE           *file, 
                           MBK_OFFSET_MAX *pos,
                           long            flagtab,
                           long            flagheap
                           )
{
  ptype_list *ptl;
  HeapAlloc  *heap;
  it         *look;

  MBK_OFFSET_STD  offset_std;
  MBK_OFFSET_LFS  offset_lfs;
  MBK_OFFSET_LFS *ptoffset_lfs;

  look=NULL;
  ptl = getptype( lofig->USER, flagtab );
  if( !ptl ) {
    look = addit(10);
    lofig->USER = addptype( lofig->USER, flagtab, look );
  }
  else {
    look = (it*)ptl->DATA;
  }

  if( mbk_getfileacces( file ) == MBK_FILE_STD ) {
    offset_std = *(MBK_OFFSET_STD*)pos;
    setititem( look, losig->INDEX, (long)offset_std );
  }
  else { //MBK_FILE_LFS

    offset_lfs = *(MBK_OFFSET_LFS*)pos;
    ptl = getptype( lofig->USER, flagheap );
    if( !ptl ) {
      heap = mbkalloc( sizeof(HeapAlloc) );
      CreateHeap( sizeof(MBK_OFFSET_LFS), 32, heap  );
      lofig->USER = addptype( lofig->USER, flagheap, heap );
    }
    else {
      ptl = getptype( lofig->USER, flagheap );
      heap = (HeapAlloc*)ptl->DATA;
    }
    
    ptoffset_lfs = (MBK_OFFSET_LFS*) getititem( look, losig->INDEX );
    if( ptoffset_lfs == (MBK_OFFSET_LFS*)EMPTYHT ) {
      ptoffset_lfs  = AddHeapItem( heap );
      setititem( look, losig->INDEX, (long)ptoffset_lfs );
    }
    *ptoffset_lfs = offset_lfs;
  }
}

char mbk_losig_getfilepos( lofig_list     *lofig, 
                           losig_list     *losig, 
                           FILE           *file,
                           MBK_OFFSET_MAX *pos, 
                           long            flagtab
                           ) 
{
  ptype_list     *ptl;
  it             *look;
  MBK_OFFSET_LFS *offset_lfs;
  MBK_OFFSET_STD  offset_std;
  
  look=NULL;
  ptl = getptype( lofig->USER, flagtab );
  if( !ptl ) {
    look = addit(10);
    lofig->USER = addptype( lofig->USER, flagtab, look );
  }
  else {
    look = (it*)ptl->DATA;
  }

  if( mbk_getfileacces( file ) == MBK_FILE_STD ) {
    offset_std = (MBK_OFFSET_STD)getititem( look, losig->INDEX );
    if( offset_std == (MBK_OFFSET_STD)EMPTYHT ||
        offset_std == (MBK_OFFSET_STD)DELETEHT   ){
      *(MBK_OFFSET_STD*)pos = (MBK_OFFSET_STD)0;
      return 0;
    }else{
      *(MBK_OFFSET_STD*)pos = offset_std;
      return 1;
    }
  }
  else {
    offset_lfs = (MBK_OFFSET_LFS*)getititem( look, losig->INDEX );
    if( offset_lfs == (MBK_OFFSET_LFS*)EMPTYHT ||
        offset_lfs == (MBK_OFFSET_LFS*)DELETEHT    ){
      *(MBK_OFFSET_LFS*)pos = (MBK_OFFSET_LFS)0;
      return 0;
    }else{
      *(MBK_OFFSET_LFS*)pos = *offset_lfs;
      return 1;
    }
  }
}

void mbk_losig_clearfilepos( lofig_list *lofig, 
                             FILE *file, 
                             long flagtab, 
                             long flagheap 
                             )
{
  ptype_list *ptl;

  ptl = getptype( lofig->USER, flagtab );
  if( ptl ) {
    delit( (it*)ptl->DATA );
    lofig->USER = delptype( lofig->USER, flagtab );
  }

  ptl = getptype( lofig->USER, flagheap );
  if( ptl ) {
    DeleteHeap( (HeapAlloc*)ptl->DATA );
    mbkfree( ptl->DATA );
    lofig->USER = delptype( lofig->USER, flagheap );
  }

  file=NULL; //unused parameter.
}

void mbkSwitchContext(mbkContext *ctx)
{
  ht *tempht;
  lofig_list *temphead;
  tempht=HT_LOFIG;
  temphead=HEAD_LOFIG;
  HT_LOFIG=ctx->HT_LOFIG;
  HEAD_LOFIG=ctx->HEAD_LOFIG;
  ctx->HT_LOFIG=tempht;
  ctx->HEAD_LOFIG=temphead;
}

mbkContext *mbkCreateContext()
{
  mbkContext *ctx=(mbkContext *)mbkalloc(sizeof(mbkContext));
  ctx->HT_LOFIG=addht(100);
  ctx->HEAD_LOFIG=NULL;
  return ctx;
}

void mbkFreeContext(mbkContext *ctx)
{
  mbkfree(ctx);
}

void mbkFreeAndSwitchContext(mbkContext *ctx)
{
  mbkSwitchContext(ctx);
  delht(ctx->HT_LOFIG);
  mbkfree(ctx);
}

/*******************************************************************************
* Power Supply Marking Functions                                               *
*******************************************************************************/

int mbk_MarkAlim(losig_list *ls)
{
  long flags=0;
  chain_list *ch;
  float value;
  
  if (getlosigalim(ls, &value))
   {
     if ((long)(value*SCALE_ALIM + 0.5) > VDD_VSS_THRESHOLD) flags|=MBK_ALIM_VDD_FLAG;
     else flags|=MBK_ALIM_VSS_FLAG;
   }

  for (ch=ls->NAMECHAIN; ch!=NULL; ch=ch->NEXT)
   {
     // type yagle
     if (ls->TYPE=='D') flags|=MBK_ALIM_VDD_FLAG;
     else if (ls->TYPE=='S') flags|=MBK_ALIM_VSS_FLAG;
     // ----
     if (isvss((char *)ch->DATA)) flags|=MBK_ALIM_VSS_FLAG;
     if (isvdd((char *)ch->DATA)) flags|=MBK_ALIM_VDD_FLAG;
     if (isglobalvss((char *)ch->DATA)) flags|=MBK_ALIM_VSS_FLAG|MBK_HAS_GLOBAL_NODE_FLAG;
     if (isglobalvdd((char *)ch->DATA)) flags|=MBK_ALIM_VDD_FLAG|MBK_HAS_GLOBAL_NODE_FLAG;
   }
 
  ls->ALIMFLAGS&=~(MBK_ALIM_VSS_FLAG|MBK_ALIM_VDD_FLAG);
  ls->ALIMFLAGS|=flags|MBK_SIGNAL_ALIM_DONE_FLAG;

  return flags;
}

void mbk_MarkLofig(lofig_list *lf)
{
  losig_list *ls;
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    mbk_MarkAlim(ls);
}

int mbk_GetAlimFlags(losig_list *ls)
{
  if ((ls->ALIMFLAGS & MBK_SIGNAL_ALIM_DONE_FLAG)==0)
    mbk_MarkAlim(ls);
  return ls->ALIMFLAGS;
}

int mbk_LosigIsVDD(losig_list *ls)
{
  if ((ls->ALIMFLAGS & MBK_SIGNAL_ALIM_DONE_FLAG)==0)
    mbk_MarkAlim(ls);
  return (ls->ALIMFLAGS & MBK_ALIM_VDD_FLAG)!=0;
}

int mbk_LosigIsVSS(losig_list *ls)
{
  if ((ls->ALIMFLAGS & MBK_SIGNAL_ALIM_DONE_FLAG)==0)
    mbk_MarkAlim(ls);
  return (ls->ALIMFLAGS & MBK_ALIM_VSS_FLAG)!=0;
}

int mbk_LosigIsGlobal(losig_list *ls)
{
  return (ls->ALIMFLAGS & MBK_HAS_GLOBAL_NODE_FLAG)!=0;
}

void mbk_SetLosigVDD(losig_list *ls)
{
  ls->ALIMFLAGS&=~(MBK_ALIM_VSS_FLAG|MBK_ALIM_VDD_FLAG);
  ls->ALIMFLAGS |= (MBK_ALIM_VDD_FLAG|MBK_SIGNAL_ALIM_DONE_FLAG);
}

void mbk_SetLosigVSS(losig_list *ls)
{
  ls->ALIMFLAGS&=~(MBK_ALIM_VSS_FLAG|MBK_ALIM_VDD_FLAG);
  ls->ALIMFLAGS |= (MBK_ALIM_VSS_FLAG|MBK_SIGNAL_ALIM_DONE_FLAG);
}

/*##------------------------------------------------------------------##*/
/*  Function : duplosigalim()                                           */
/*##------------------------------------------------------------------##*/

void duplosigalim (losig_list *losig_ptr, losig_list *losig_rpt)
{
   float alim=0;
   char *expr;

   expr=getlosigalim_expr(losig_ptr);
   if (getlosigalim (losig_ptr, &alim) || expr!=NULL)
      addlosigalim (losig_rpt, alim, expr);
}

void mbk_copylosiginfo(losig_list *source, losig_list *dest)
{
  duplosigalim (source, dest);
  dest->ALIMFLAGS=source->ALIMFLAGS;
}

/*******************************************************************************
* function addlosigalim                                                        *
*******************************************************************************/
void addlosigalim(losig_list *losig, float alim, char *expr)
{
 ptype_list *ptype ;
 long lalim ;


 if (expr!=NULL)
 {
   if((ptype = getptype(losig->USER,LOSIGALIM_EXPR)) == NULL)
     losig->USER = addptype(losig->USER,LOSIGALIM_EXPR,expr) ;
   else
   {
     avt_errmsg (MBK_ERRMSG, "035", AVT_WARNING, getsigname(losig));
     ptype->DATA = expr;
   }
 }
 else
 {
   lalim = (long)(SCALE_ALIM * alim + 0.5) ;

   if((ptype = getptype(losig->USER,LOSIGALIM)) == NULL)
       losig->USER = addptype(losig->USER,LOSIGALIM,(void *)lalim) ;
    else
      {
          avt_errmsg (MBK_ERRMSG, "035", AVT_WARNING, getsigname(losig));
       ptype->DATA = (void *)lalim ;
      }
   /* Add the Supply Marks */
   mbk_MarkAlim(losig);
 }
}

/*******************************************************************************
* function dellosigalim                                                        *
*******************************************************************************/
int dellosigalim(losig_list *losig)
{
 if(getptype(losig->USER,LOSIGALIM) != NULL)
    {
     losig->USER = delptype(losig->USER,LOSIGALIM) ;
     return 1 ;
    }

 return 0 ;
}
void dellosigalim_expr(losig_list *losig)
{
 if(getptype(losig->USER,LOSIGALIM_EXPR) != NULL)
    {
     losig->USER = delptype(losig->USER,LOSIGALIM_EXPR) ;
    }
}

/*******************************************************************************
* function getlosigalim                                                        *
*******************************************************************************/
int getlosigalim(losig_list *losig, float *alim)
{
 ptype_list *ptype ;
 long lalim ;

 if((ptype = getptype(losig->USER,LOSIGALIM)) != NULL)
    {
     lalim = (long)ptype->DATA ;
     if ( alim )
       *alim = (float)lalim / SCALE_ALIM ;
     return 1 ;
    }

 return 0 ;
}
char *getlosigalim_expr(losig_list *losig)
{
 ptype_list *ptype ;

 if((ptype = getptype(losig->USER,LOSIGALIM_EXPR)) != NULL)
    return (char *)ptype->DATA;

 return NULL ;
}

lofiginfo *mbk_getlofiginfo(lofig_list *lf, int create)
{
  ptype_list *pt;
  lofiginfo *lfif;
  pt=getptype(lf->USER, LOFIG_INFO);
  if (pt==NULL) 
  {
     if (!create) return NULL;
  }
  else return (lofiginfo *)pt->DATA;
  lfif=mbkalloc(sizeof(lofiginfo));
  // init
  lfif->scale=1;
  //
  lf->USER=addptype(lf->USER, LOFIG_INFO, lfif);
  return lfif;
}

int mbk_can_cross_transistor_to(losig_list *ls, lotrs_list *tr, char dir)
{
  locon_list *from, *to;
  long flagfrom, flagto, testfrom, testto;
  ptype_list *pt;
  if (ls==tr->SOURCE->SIG) from=tr->SOURCE, to=tr->DRAIN;
  else to=tr->SOURCE, from=tr->DRAIN;
  if ((pt=getptype(from->USER, LOCON_INFORMATION))==NULL) flagfrom=0;
  else flagfrom=(long)pt->DATA;
  if ((pt=getptype(to->USER, LOCON_INFORMATION))==NULL) flagto=0;
  else flagto=(long)pt->DATA;
  
  if (dir=='i') testfrom=LOCON_INFORMATION_OUTPUT, testto=LOCON_INFORMATION_INPUT;
  else testfrom=LOCON_INFORMATION_INPUT, testto=LOCON_INFORMATION_OUTPUT;

  if (flagfrom!=0 && (flagfrom & testfrom)==0) return 0;
  if (flagto!=0 && (flagto & testto)==0) return 0;
  return 1;
}

void mbk_debug_losig( char *label, lofig_list *lofig )
{
losig_list *ls;
char *name;
if ((name=getenv("MBK_DEBUG_LOSIG"))!=NULL)
{
ls= mbk_quickly_getlosigbyname( lofig,namealloc(name) );
printf("debug_losig (%s) %s:\n", label, name);
if (ls!=NULL)
  viewlosig_sub(ls,1);
else
  printf("signal not found\n");
}
}
