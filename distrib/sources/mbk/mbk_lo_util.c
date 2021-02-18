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
 * Purpose : utilities for logical structures
 * Date    : 05/11/92
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

#include MUT_H
#include MLO_H
#include RCN_H
#include MLU_H
#include AVT_H
#include INF_H
#include MCC_H
#include MSL_H

#include "mbk_lo_util.h"
#include "rcn.h"

/* local for flattenlofig */
#define FLATTEN_CTC 19971215
#define FLATTEN_POSTRAIT 19971216

void debugctc (losig_list *, int);
void debugctc2 (lofig_list *);

static chain_list *BLACKLIST=NULL;
static mbk_match_rules *BLACKLIST_MATCH_RULE=NULL, *BLACKLIST_MATCH_RULE_UNUSED=NULL;

int MBK_HANDLE_GLOBAL_NODES=1;
int MLU_TRACE_EQT = 0;
lofig_list *(*HOOK_GETLOADEDLOFIG) (char *name) = NULL;
static int bl_loaded = 0;

/*******************************************************************************
* function getlofig()                                                          *
* if mode == 'A'  all the figure is needed                                     *
* if mode == 'P'  interface only is needed                                     *
*****************************************************************************/
lofig_list *getlofig (figname, mode)
char *figname;
char mode;
{
   lofig_list *ptfig;
   int locked = 0;

   if (HT_LOFIG == NULL) {
      HT_LOFIG = addht (50);
   }

   figname = namealloc (figname);
   /* scan figure list */
   ptfig = (lofig_list *) gethtitem (HT_LOFIG, figname);
   if (ptfig == (lofig_list *) EMPTYHT || ptfig == (lofig_list *) DELETEHT)
      ptfig = NULL;
   if (ptfig == NULL) {
      ptfig = HEAD_LOFIG;
      while (ptfig != NULL && ptfig->NAME != figname)
         ptfig = ptfig->NEXT;
   }

   if (ptfig) {
      if (mode == 'A' && ptfig->MODE == 'P') {
         locked = islofiglocked (ptfig);
         if (locked)
            unlocklofig (ptfig);
         dellofig (ptfig->NAME);
         ptfig = NULL;
      }
   }

   if (ptfig == NULL) {      /* figure doesn't exist */
      ptfig = (lofig_list *) mbkalloc (sizeof (lofig_list));
      ptfig->NAME = figname;
      ptfig->MODELCHAIN = NULL;
      ptfig->LOINS = NULL;
      ptfig->LOTRS = NULL;
      ptfig->LOCON = NULL;
      ptfig->LOSIG = NULL;
      mbk_init_NewBKSIG(&ptfig->BKSIG);
      ptfig->USER = NULL;
      ptfig->NEXT = HEAD_LOFIG;
      HEAD_LOFIG = ptfig;
      ptfig->MODE = mode;
      addhtitem (HT_LOFIG, figname, (long)ptfig);
      loadlofig (ptfig, figname, mode);
      if (locked)
         locklofig (ptfig);
      return ptfig;
   }

   return ptfig;
}

/*******************************************************************************
* function givelosig()                                                         *
*******************************************************************************/
losig_list *givelosig (ptfig, index)
lofig_list *ptfig;
long index;
{
   losig_list *ptsig = NULL;
/*   ptype_list *pt = NULL;
   int low;
   int type;
   int sigsize;
   sigsize = getsigsize (ptfig);
   low = (index % sigsize);
   type = (index / sigsize);
   for (pt = ptfig->BKSIG; pt; pt = pt->NEXT)
      if (pt->TYPE == type)
         break;
   if (pt != NULL) {
      ptsig = (losig_list *) (pt->DATA) + low;
      if (ptsig->INDEX == index)
         return ptsig;
   }*/
   if (index<=ptfig->BKSIG.maxindex) {
     ptsig = getlosig(ptfig, index);
     if (ptsig->INDEX != index) {
       ptsig->INDEX = index;
       ptsig->TYPE = 'I';
       ptsig->NEXT = ptfig->LOSIG;
       ptfig->LOSIG = ptsig;
     }
   }
   else ptsig = addlosig (ptfig, index, (chain_list *) NULL, 'I');
   return ptsig;
}

void (*FlattenOnCreateLOTRS) (lotrs_list * tr) = NULL;
void (*FlattenOnCreateLOINS) (loins_list * li) = NULL;

/*******************************************************************************
* function  flattenlofig()                                                     *
*                                                                              *
* the instance "insname" is suppressed as a hierarchical level                 *
* in figure pointed by "ptfig". Capacitances and netlist                       *
* consistency is preserved.                                                    *
* Modified by Gregoire AVOT for rcn support.                                   *
*******************************************************************************/

#define LFCON_TO_INSCON 0xdafe001
#define LFSIG_TO_LFEXTCON 0xdafe002
#define LF_HIERNAME_TO_NAMEALLOC 0xdafe003
#define LFCANDIDAT 0x1
#define LFPOSTRAIT 0x2
#define LFREBUILDCTC 0x4


void mbk_addvcardnode(losig_list *ls, char *name, float v, long rcn)
{
  ptype_list *pt;
  chain_list *cl;

  vcardnodeinfo *vni;

  if ((pt=getptype(ls->USER, MBK_VCARD_NODES))==NULL)
    ls->USER=pt=addptype(ls->USER, MBK_VCARD_NODES, NULL);
  for (cl=(chain_list *)pt->DATA; cl; cl=cl->NEXT)
    {
      vni=(vcardnodeinfo *)cl->DATA;
      if (vni->value==v && vni->rcn==rcn) break;
    }
       
  if (cl==NULL)
    {
      vni=(vcardnodeinfo *)mbkalloc(sizeof(vcardnodeinfo));
      vni->name=name;
      vni->value=v;
      vni->rcn=rcn;
      pt->DATA=addchain((chain_list *)pt->DATA, vni);
    }
}

void mbk_dupvcardnodes(losig_list *src, losig_list *dest)
{
  ptype_list *pt;
  chain_list *cl;
  vcardnodeinfo *vni;

  if ((pt=getptype(src->USER, MBK_VCARD_NODES))!=NULL)
    {
      for (cl=(chain_list *)pt->DATA; cl; cl=cl->NEXT)
        {
          vni=(vcardnodeinfo *)cl->DATA;
          mbk_addvcardnode(dest, vni->name, vni->value, vni->rcn);
        }
    }
}

static void mbk_flat_locon_pnodename (locon_list * intlocon, loins_list * ptins, char *insname, AdvancedNameAllocator *ana)
{
   locon_list *extlocon;
   locon_list *testlocon;
   ptype_list *ptptype, *p0;
   chain_list *head;
   chain_list *scan;
   chain_list *headgoodpnodename;
   char buf[1024];
   
   extlocon = NULL;
   if (MBK_DUP_PNAME_FF == 'Y') {
      ptptype = getptype (intlocon->SIG->USER, LOFIGCHAIN);

      if (ptptype) {

         head = (chain_list *) ptptype->DATA;

         for (scan = head; scan; scan = scan->NEXT) {

            testlocon = ((locon_list *) (scan->DATA));

            if (testlocon == intlocon)
               continue;

            if (testlocon->TYPE == EXTERNAL) {
               extlocon = testlocon;
               continue;
            }

            break;
         }
         if (scan)
            extlocon = NULL;
      }

      if (extlocon) {

         for (testlocon = ptins->LOCON; testlocon; testlocon = testlocon->NEXT)
            if (testlocon->NAME == extlocon->NAME)
               break;

         if (testlocon) {
            extlocon = testlocon;
            ptptype = getptype (extlocon->USER, PNODENAME);
            if (ptptype)
               headgoodpnodename = (chain_list *) ptptype->DATA;
            else
               extlocon = NULL;
         }
         else
            extlocon = NULL;
      }
   }

   ptptype = getptype (intlocon->USER, PNODENAME);
   if (extlocon) {
      if (ptptype) {

        headgoodpnodename=dupchainlst (headgoodpnodename);
        if ((p0 = getptype (ptins->USER, LF_HIERNAME_TO_NAMEALLOC)) != NULL) 
         {
           AdvancedNameAllocName (ana, (int)(long)p0->DATA, buf);
           for (scan = headgoodpnodename; scan; scan = scan->NEXT)
             scan->DATA = concatname (buf, (char *)(scan->DATA));
         }
        else
           strcpy(buf, ptins->INSNAME);
        
        freechain(ptptype->DATA);
        ptptype->DATA=headgoodpnodename;
/*        intlocon->USER = delptype (intlocon->USER, PNODENAME);
        intlocon->USER = addptype (intlocon->USER, PNODENAME, headgoodpnodename);*/
      }
   }
   else {
      if (ptptype) {
         for (scan = (chain_list *) (ptptype->DATA); scan; scan = scan->NEXT)
            scan->DATA = concatname (insname, (char *)(scan->DATA));
      }
   }
}

void flattenlofig (lofig_list * ptfig, char *insname, char concat)
{
  loins_list *li;
  AdvancedNameAllocator *ana;
  chain_list *cl;
  
  if (MBK_GLOBALPARAMS)
    {
      EQT_CONTEXT_HIERARCHY[0]=eqt_init(32);
      eqt_import_vars (EQT_CONTEXT_HIERARCHY[0], MBK_GLOBALPARAMS);
    }

  if (insname == NULL)
    {
      rflattenlofig(ptfig, concat, NO);
    }
  else
    {
      ana = CreateAdvancedNameAllocator (CASE_SENSITIVE);
      li = getloins (ptfig, insname);
      flattenlofig_bypointer (ptfig, cl = addchain (NULL, li), ana, concat);
      freechain (cl);
      flatten_setup_realname_from_hiername (ptfig, ana);
      FreeAdvancedNameAllocator (ana);
    }
    
  if (MBK_GLOBALPARAMS)
    {
      eqt_term(EQT_CONTEXT_HIERARCHY[0]);
      EQT_CONTEXT_HIERARCHY[0] = NULL;
    }
}

void flattenloinslist(lofig_list * ptfig, chain_list *cl, char concat)
{
  AdvancedNameAllocator *ana;
  ana = CreateAdvancedNameAllocator (CASE_SENSITIVE);
  flattenlofig_bypointer (ptfig, cl, ana, concat);
  flatten_setup_realname_from_hiername (ptfig, ana);
  FreeAdvancedNameAllocator (ana);
}

static void maken2 (lofig_list * lf, loins_list * li)
{
   locon_list *lcf, *lci;
   ptype_list *p;
   for (lcf = lf->LOCON; lcf != NULL; lcf = lcf->NEXT) {
      if (getptype (lcf->SIG->USER, LFSIG_TO_LFEXTCON) == NULL) {
         for (lci = li->LOCON; lci != NULL && lci->NAME != lcf->NAME; lci = lci->NEXT);
         if (lci == NULL) {
            avt_errmsg(MBK_ERRMSG, "000", AVT_FATAL, lcf->NAME, li->INSNAME);
         }
         lcf->USER = addptype (lcf->USER, LFCON_TO_INSCON, lci);
         if ((p = getptype (lcf->SIG->USER, LFSIG_TO_LFEXTCON)) == NULL)
            p = lcf->SIG->USER = addptype (lcf->SIG->USER, LFSIG_TO_LFEXTCON, NULL);
         p->DATA = addchain ((chain_list *) p->DATA, lcf);
      }
   }
}

static void addcorresp (lofig_list * lf, loins_list * li)
{
   locon_list *lcf, *lci;
   ptype_list *p;
   int swapped = 0;
   if (lf->LOCON != NULL && li->LOCON != NULL && lf->LOCON->NAME != li->LOCON->NAME) {
      li->LOCON = (locon_list *) reverse ((chain_list *) li->LOCON);
      swapped = 1;
   }

   for (lcf = lf->LOCON, lci = li->LOCON; lcf != NULL && lci != NULL; lcf = lcf->NEXT, lci = lci->NEXT) {
      if (lcf->NAME != lci->NAME) {
         maken2 (lf, li);
         return;
      }
      lcf->USER = addptype (lcf->USER, LFCON_TO_INSCON, lci);
      if ((p = getptype (lcf->SIG->USER, LFSIG_TO_LFEXTCON)) == NULL)
         p = lcf->SIG->USER = addptype (lcf->SIG->USER, LFSIG_TO_LFEXTCON, NULL);
      p->DATA = addchain ((chain_list *) p->DATA, lcf);
   }
   if (lcf != lci) {
            avt_errmsg(MBK_ERRMSG, "001", AVT_FATAL, li->INSNAME, lcf->NAME);
   }
   if (swapped)
      li->LOCON = (locon_list *) reverse ((chain_list *) li->LOCON);
}

static chain_list *getsigexternalconnectors (losig_list * ls)
{
   ptype_list *p;
   chain_list *cl;
   if ((p = getptype (ls->USER, LFSIG_TO_LFEXTCON)) == NULL)
      return NULL;
   cl = (chain_list *) p->DATA;
   return cl;
}

static locon_list *getcorrespinstanceconnector (locon_list * lc)
{
   ptype_list *p;
   locon_list *clc;
   if ((p = getptype (lc->USER, LFCON_TO_INSCON)) == NULL)
      return NULL;
   clc = (locon_list *) p->DATA;
   return clc;
}

static void cleansigexternalconnectors (losig_list * ls)
{
   if (getptype (ls->USER, LFSIG_TO_LFEXTCON) == NULL)
      exit (4);
   ls->USER = delptype (ls->USER, LFSIG_TO_LFEXTCON);
}
static void cleancorrespinstanceconnector (locon_list * lc)
{
   if (getptype (lc->USER, LFCON_TO_INSCON) == NULL)
      exit (5);
   lc->USER = delptype (lc->USER, LFCON_TO_INSCON);
}

/* dupliquer le contexte de ptins dans toutes les instances de figins */
/* pour chaque instance de figins, evaluer les parametres dans le contexte duplique */
/* rajouter les parametres dans le contexte duplique */
/* mettre a jour la valeur des resistances, des capas et des transistors dans figins */
/* detruire le contexte de ptins */

//extern eqt_ctx *spi_init_eqt ();

eqt_param *MBK_GLOBALPARAMS = NULL;
chain_list *MBK_GLOBALFUNC=NULL;
ht *MBK_MONTE_MODEL_PARAMS=NULL;
ptype_list *MBK_MONTE_DIST=NULL;
ptype_list *MBK_MONTE_PARAMS=NULL;

char *mbk_montecarlo_key(char *model, char *subcktname, int type, char location)
{
  char txt[1024];
  sprintf(txt,"%s::%s::%d::%c", model, subcktname?subcktname:"", type, location);
  return namealloc(txt);
}

/*extern float SPI_SCALE_CAPAFACTOR;
extern float SPI_SCALE_RESIFACTOR;
extern float SPI_SCALE_TRANSFACTOR;
extern float SPI_SCALE_DIODEFACTOR;
*/
#define EQTFAST 1

static inline double opt_eval(eqt_ctx *ctx, optparam_list *ptopt)
{
  if (ptopt->EQT_EXPR==NULL)
    return eqt_eval (ctx, ptopt->UDATA.EXPR, EQTFAST);

  return eqt_calcval (ctx, ptopt->EQT_EXPR);  
}

static void print_mlu_error(lofig_list *fig, eqt_ctx *ctx, char *var, char *expr, char *insname, chain_list **unk)
{
  chain_list *cl, *ch;
  int nb;
  char buf[2048];

  cl=eqt_GetVariables(ctx, expr, 0);
  nb=countchain(cl);

  strcpy(buf,"");
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      if (ch!=cl)
      {
        if (ch->NEXT==NULL)
         strcat(buf,"and ");
        else
         strcat(buf,", ");
      }
      strcat(buf,"'");
      strcat(buf,(char *)ch->DATA);
      strcat(buf,"'");
//      eqt_addvar(ctx, (char *)ch->DATA, 0);
//      *unk=addchain(*unk, ch->DATA);
    }
  freechain(cl);

  avt_errmsg(MBK_ERRMSG, "052", AVT_ERROR, var, expr, fig->NAME, nb>1?"s":"", buf, nb>1?"are":"is");
  insname=NULL;
}

int need_scale (char *param)
{
    return (!strcasecmp (param, "sa") || !strcasecmp (param, "sb") || !strcasecmp (param, "sd"));
}

static int m_is_allowed(char what)
{
  switch (what)
  {
    case 'i': // instances
      switch (V_INT_TAB[__SIM_TOOLMODEL].VALUE)
      {
        case SIM_TOOLMODEL_ELDO:
        case SIM_TOOLMODEL_HSPICE:
          return 1;
        case SIM_TOOLMODEL_TITAN:
        case SIM_TOOLMODEL_SPICE:
        default:
          return 0;
      }
    case 't': // transistors
      return 1;
    case 'r': // resistor
      switch (V_INT_TAB[__SIM_TOOLMODEL].VALUE)
      {
        case SIM_TOOLMODEL_ELDO:
        case SIM_TOOLMODEL_HSPICE:
        case SIM_TOOLMODEL_TITAN:
           return 1;
        case SIM_TOOLMODEL_SPICE:
        default:
          return 0;
      }
    case 'c': // capacitor
      switch (V_INT_TAB[__SIM_TOOLMODEL].VALUE)
      {
        case SIM_TOOLMODEL_ELDO:
        case SIM_TOOLMODEL_HSPICE:
        case SIM_TOOLMODEL_TITAN:
          return 1;
        case SIM_TOOLMODEL_SPICE:
        default:
           return 0;
      }
   }
  return 0;
}

static eqt_ctx *nom_sub_ctx=NULL;

static double nom_sub_mod(char *var)
{
  double val=0;
  val=eqt_getvar_in_context_only(nom_sub_ctx, var);
  return val;
}

static void flatten_eval_montecarlo(eqt_ctx *evalctx, eqt_ctx *origctx, char *subname, int global)
{
  char *mckey;
  ptype_list *mcparam, *pt;
  double *res;
  int cnt, i;
  long l;
  if (MBK_MONTE_MODEL_PARAMS!=NULL)
    {
      eqt_setspecialfunc(evalctx, EQTSPECFUNC_NOM_SUB, nom_sub_mod);
      nom_sub_ctx=origctx;
      mckey=mbk_montecarlo_key(subname, NULL, 555, global?'G':'L'); 
      if ((l=gethtitem(MBK_MONTE_MODEL_PARAMS, mckey))!=EMPTYHT) mcparam=(ptype_list *)l;
      else mcparam=NULL;
      cnt=countchain((chain_list *)mcparam);
      if (cnt>0)
        {
          res=(double *)mbkalloc(sizeof(double)*cnt);
          for (pt=mcparam, i=0; pt!=NULL; pt=pt->NEXT, i++)
            {
              res[i]=eqt_eval(evalctx, (char *)pt->DATA, EQTFAST);
              if (!eqt_resistrue(evalctx))
                avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, (char *)pt->TYPE, (char *)pt->DATA, " : set to 0");
              else if (!finite(res[i]))
                avt_errmsg(MCC_ERRMSG, "038", AVT_ERROR, (char *)pt->TYPE, (char *)pt->DATA," : returned NaN or Inf");
            }

          for (pt=mcparam, i=0; pt!=NULL; pt=pt->NEXT, i++)
            if (eqt_isdefined(origctx, (char *)pt->TYPE, 0))
              eqt_addvar(origctx, (char *)pt->TYPE, res[i]);
            else
              avt_errmsg(MBK_ERRMSG, "053", AVT_ERROR, global?"global":"local", (char *)pt->TYPE, subname);

          mbkfree(res);
        }
      nom_sub_ctx=NULL;
      eqt_setspecialfunc(evalctx, EQTSPECFUNC_NOM_SUB, NULL);
    }
}

void flatten_parameters (lofig_list * fig, loins_list *ptfatherloins, eqt_param * param_tab, int x, int y, 
        int Tx, int Ty, int R, int A, eqt_ctx **mc_ctx)
{
   optparam_list *optparams, *saveparams, *ptopt, *ptprev, *ptnext;
   loins_list *ptins, *ptnewloins, *ptnewloinslist=NULL;
   lotrs_list *pttrs;
   lotrs_list *ptnewlotrs, *ptnewlotrslist = NULL;
   ptype_list *ptduptrs, *ptduptrslist = NULL;
   ptype_list *ptduploins, *ptduploinslist = NULL;
   ptype_list *ptype;
   static eqt_ctx *ins_ctx=NULL, *min_ctx=NULL;
   static eqt_ctx *ctx=NULL;
   eqt_param *defaults;
   double value, scale, foundval;
   double trans_w, trans_l, trans_as, trans_ad;
   double diode_area, diode_pj, tc1, tc2, dtemp;
   loctc_list *ptctc;
   losig_list *ptsig;
   lofig_list *ptfig;
   chain_list *ptch;
   lowire_list *ptwire;
   locon_list *lc;
   char *var;
   lofig_list *ins_fig;
   ptype_list *xy_ptype;
   xy_struct  *xy;
   char *diode_model, *diodesubckt;
   int capa_diode_flag;
   int model_instance = 0;
   int i;
   char namebuf[1024];
   lofiginfo *lfif;
   int cntctx=0, newdone, m;
   int minctx = 0;
   
   if ((lfif=mbk_getlofiginfo(fig, 0))!=NULL) scale=lfif->scale;
   else scale=1;
           
   if (ptfatherloins != NULL)
     {
       if ((ptype = getptype(ptfatherloins->USER, MIN_PARAM_CONTEXT)) != NULL) 
         {
           fig->LOTRS->USER = addptype(fig->LOTRS->USER, PARAM_CONTEXT, ptype->DATA);
           ptfatherloins->USER = delptype(ptfatherloins->USER, MIN_PARAM_CONTEXT);
         }
     }

   if (MLU_TRACE_EQT)
      fprintf (stdout, "******** FIGURE %s *********\n", fig->NAME);



   // get default parameters
  

/*   if (*moreparams!=NULL) {
       for (ptch=*moreparams; ptch!=NULL; ptch=ptch->NEXT) {
           eqt_addvar(ctx, (char *)ptch->DATA, 0);
       }
   }*/
   if (EQT_CONTEXT_HIERARCHY[0])
   {
      minctx++;
      cntctx++;
   }
   else if (MBK_GLOBALPARAMS)
   {
      EQT_CONTEXT_HIERARCHY[cntctx]=eqt_init(32);
      eqt_import_vars (EQT_CONTEXT_HIERARCHY[cntctx], MBK_GLOBALPARAMS);
      cntctx++;
   }
   if (*mc_ctx==NULL && MBK_MONTE_MODEL_PARAMS!=NULL)
     {
       *mc_ctx = eqt_init(EQT_NB_VARS) ;
       eqt_add_spice_extension(*mc_ctx);
       if (MBK_GLOBALFUNC) eqt_import_func(MBK_GLOBALFUNC, *mc_ctx);
       mbk_EvalGlobalMonteCarloDistributions(*mc_ctx);       
     }

   if (ctx) eqt_resetvars(ctx, 0);
   else
   {
     ctx = spi_init_eqt ();
     if (MBK_GLOBALFUNC)
      eqt_import_func(MBK_GLOBALFUNC, ctx);
   }
   if (param_tab)
   {
     eqt_ctx *tmpctx;
     tmpctx=eqt_init(32);
     eqt_import_vars (tmpctx, param_tab);
     if (*mc_ctx!=NULL) flatten_eval_montecarlo(*mc_ctx, tmpctx, fig->NAME, 0);
     EQT_CONTEXT_HIERARCHY[cntctx]=tmpctx;
     cntctx++;
   }

   if ((ptype = getptype (fig->USER, OPT_PARAMS))) 
     {
       static ht *allparams=NULL;
       if (allparams==NULL) allparams=addht(128);
       else resetht(allparams);
       optparams = (optparam_list *) ptype->DATA;
       for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT) 
         {
           if (eqt_isdefined(ctx, ptopt->UNAME.SPECIAL, 0)) continue;
           addhtitem(allparams, ptopt->UNAME.SPECIAL, (long)ptopt->UDATA.EXPR);
         }
       mbk_recur_Eval_Start(ctx, allparams, fig->NAME);
//       delht(allparams);
     }

/*   if (param_tab)
      eqt_import_vars (ctx, param_tab);
   if (MBK_GLOBALFUNC)
      eqt_import_func(MBK_GLOBALFUNC, ctx);*/
   defaults = eqt_export_vars (ctx);
//   ins_ctx=min_ctx=NULL;

   // Update instances
   for (ptins = fig->LOINS; ptins; ptins = ptins->NEXT) {
      if (MLU_TRACE_EQT)
         fprintf (stdout, "\n----------------\nINSTANCE %s\n\n", ptins->INSNAME);
      if (getptype (ptins->USER, PARAM_CONTEXT))
         continue;

      ins_fig = getloadedlofig (ptins->FIGNAME);
      if (ins_fig==NULL) continue;

      if (ins_ctx ==NULL)
      {
        if (ins_ctx) eqt_resetvars(ins_ctx, 0);
        else ins_ctx = spi_init_eqt ();
        if (MBK_GLOBALFUNC)
          eqt_import_func(MBK_GLOBALFUNC, ins_ctx);
      }
      else eqt_resetvars(ins_ctx, 0);

      /* Test for single transistor instance and save passed parameters for MCC */
      model_instance = 0;
      if (ins_fig->LOTRS != NULL && ins_fig->LOTRS->NEXT == NULL) {
         pttrs = ins_fig->LOTRS;
         if (getptype(ins_fig->LOTRS->USER, TRANS_FIGURE) != NULL) {
            model_instance = 1;
            if (min_ctx==NULL)
            {
              min_ctx = spi_init_eqt();
              if (MBK_GLOBALFUNC)
                 eqt_import_func(MBK_GLOBALFUNC, min_ctx);
            }
            else eqt_resetvars(min_ctx, 0);
         }
      }

      if (ins_fig && (ptype = getptype (ins_fig->USER, PARAM_CONTEXT))) {
         eqt_import_vars (ins_ctx, (eqt_param *) ptype->DATA);
         if (*mc_ctx!=NULL)
           {
             eqt_ctx *savectx;
             if (param_tab)
               savectx=EQT_CONTEXT_HIERARCHY[cntctx-1], EQT_CONTEXT_HIERARCHY[cntctx-1]=NULL;
             flatten_eval_montecarlo(*mc_ctx, ins_ctx, ins_fig->NAME, 1);
             if (param_tab)
               EQT_CONTEXT_HIERARCHY[cntctx-1]= savectx;
           }
      }
      eqt_import_vars (ins_ctx, defaults);

      if ((ptype = getptype (ptins->USER, OPT_PARAMS))) {
         int is_m;
         optparams = (optparam_list *) ptype->DATA;

         for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT) {
            if (m_is_allowed('i') && !strcasecmp(ptopt->UNAME.SPECIAL, "m")) is_m=1; else is_m=0;

            if (ptopt->TAG == '$') {
               if (MLU_TRACE_EQT)
                  fprintf (stdout, "PARAM: %s = %s\n", ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR);
               value = opt_eval (ins_ctx, ptopt);
               if (!eqt_resistrue (ins_ctx)) {
                  print_mlu_error(fig, ctx, ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR, NULL, NULL);
                  value = opt_eval (ctx, ptopt);
               }
               else if (!finite(value))
                  avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR," : returned NaN or Inf");
               else if (MLU_TRACE_EQT)
                  fprintf (stdout, "---> '%s' = %g\n", ptopt->UDATA.EXPR, value);
               if (!is_m) eqt_addvar (ins_ctx, ptopt->UNAME.SPECIAL, value);
            }
            else {
               if (MLU_TRACE_EQT)
                  fprintf (stdout, "PARAM: %s = %g\n", ptopt->UNAME.SPECIAL, ptopt->UDATA.VALUE);
               if (!is_m) eqt_addvar (ins_ctx, ptopt->UNAME.SPECIAL, ptopt->UDATA.VALUE);
               value = ptopt->UDATA.VALUE;
            }
            if (!is_m && model_instance == 1) eqt_addvar (min_ctx, ptopt->UNAME.SPECIAL, value);
            if (is_m) {
               ptduploinslist = addptype(ptduploinslist, mbk_long_round(value), ptins);
            }
            
         }
         freeoptparams (optparams);
         ptins->USER = delptype (ptins->USER, OPT_PARAMS);
         /* save min context in instance for MCC */
         if (model_instance == 1) {
            ptins->USER = addptype(ptins->USER, MIN_PARAM_CONTEXT, eqt_export_vars(min_ctx));
         }
      }
      ptins->USER = addptype (ptins->USER, PARAM_CONTEXT, eqt_export_vars (ins_ctx));

      if ((xy_ptype = getptype (ptins->USER, MSL_XY_PTYPE))) {
          xy = (xy_struct*)xy_ptype->DATA;
          xy->X += x;
          xy->Y += y;
      }
   }
//   if (ins_ctx!=NULL) eqt_term (ins_ctx);
//   if (min_ctx!=NULL) eqt_term(min_ctx);

   // update capacitances and wires
   for (ptsig = fig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
      if ((var=getlosigalim_expr(ptsig))!=NULL)
      {
        value=eqt_eval (ctx, var, EQTFAST);
        if (!eqt_resistrue (ctx)) {
           print_mlu_error(fig, ctx, "{voltage source value}", var, NULL, NULL);
           value = eqt_eval (ctx, var, EQTFAST);
        }
        dellosigalim_expr(ptsig);
        addlosigalim (ptsig, value, NULL);
      }
      if (!ptsig->PRCN)
         continue;
      for (ptch = ptsig->PRCN->PCTC; ptch; ptch = ptch->NEXT) {
         ptctc = (loctc_list *) ptch->DATA;
         if (!(ptype = getptype (ptctc->USER, OPT_PARAMS)))
            continue;
         optparams = (optparam_list *) ptype->DATA;
         capa_diode_flag=0;
         m=1;
         foundval=-1;
         for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT) {
            var = ptopt->UNAME.SPECIAL;
            if (!strcasecmp (var, "diode_model")) {
               diode_model = ptopt->UDATA.EXPR;
               diodesubckt=NULL;
               capa_diode_flag = 1;
               diode_area=1;
               diode_pj=0;
               continue; // don't try to evaluate anything
            }
            if (!strcasecmp (var, "diode_subckt")) {
               diodesubckt=ptopt->UDATA.EXPR;
               continue;
            }
            if (ptopt->TAG == '$') {
               value = opt_eval (ctx, ptopt);
               if (!eqt_resistrue (ctx)) {
                  print_mlu_error(fig, ctx, "{capa value}", ptopt->UDATA.EXPR, NULL,NULL);
                  value = opt_eval (ctx, ptopt);
               }
               else if (!finite(value))
                 avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, "{capa value}", ptopt->UDATA.EXPR," : returned NaN or Inf");
               else if (value<0)
               {
                 avt_errmsg(MBK_ERRMSG, "070", AVT_ERROR, ptopt->UDATA.EXPR, "capacitance");
               }
               else if (MLU_TRACE_EQT)
                  fprintf (stdout, "CAPA ---> '%s' = %g\n", ptopt->UDATA.EXPR, value);
            }
            else
               value = ptopt->UDATA.VALUE;

            if (m_is_allowed('c') && !strcasecmp (var, "m")) 
            {
              m=mbk_long_round(value);
            }
            else if (!strcasecmp (var, "val")) {
               foundval=value;
            }
            else if (!strcasecmp (var, "diode_area")) {
               value*=scale*scale;
               capa_diode_flag = 1;
               diode_area = value;
            }
            else if (!strcasecmp (var, "diode_pj")) {
               value*=scale;
               capa_diode_flag = 1;
               diode_pj = value;
            }
         }
         if (foundval!=-1) 
         {
           rcn_removecapa(ptctc->SIG1, ptctc->CAPA);
           rcn_removecapa(ptctc->SIG2, ptctc->CAPA);
           ptctc->CAPA = foundval * SPI_SCALE_CAPAFACTOR * 1e12* m;
           rcn_addcapa(ptctc->SIG1, ptctc->CAPA);
           rcn_addcapa(ptctc->SIG2, ptctc->CAPA);
         }
         else
          ptctc->CAPA*=m;
         if ( capa_diode_flag ) {
/*           if ( (ptfig = getloadedlofig(fig->NAME) )) {
               if ( (ptype = getptype(ptfig->USER,PARAM_CONTEXT) ) ) {
                 eqt_free_param (ptype->DATA);
                 ptype->DATA = eqt_export_vars (ctx);
               }
           
           }*/
           MCC_CURRENT_SUBCKT=diodesubckt;
           value = mcc_calcDioCapa ( MCC_MODELFILE, diode_model,
                                     MCC_DIODE, MCC_TYPICAL, 
                                     0.0, MCC_VDDmax, MCC_TEMP,
                                     (diode_area > 0.0) ? diode_area:0.0,  
                                     (diode_pj > 0.0) ? diode_pj:0.0
                                   );
           if (!finite(value))
             avt_errmsg(MBK_ERRMSG, "071", AVT_ERROR, "NaN or Inf for");
           else if (value<0)
             avt_errmsg(MBK_ERRMSG, "071", AVT_ERROR, "negative");
           MCC_CURRENT_SUBCKT=NULL;
           rcn_removecapa(ptctc->SIG1, ptctc->CAPA);
           rcn_removecapa(ptctc->SIG2, ptctc->CAPA);
           ptctc->CAPA = value * SPI_SCALE_CAPAFACTOR * 1.0e12;
           rcn_addcapa(ptctc->SIG1, ptctc->CAPA);
           rcn_addcapa(ptctc->SIG2, ptctc->CAPA);
           if ( !rcn_isCapaDiode (ptctc) )
             rcn_setCapaDiode( ptctc, 1); 
         }

         freeoptparams (optparams);
         ptctc->USER = delptype (ptctc->USER, OPT_PARAMS);
      }
      for (ptwire = ptsig->PRCN->PWIRE; ptwire; ptwire = ptwire->NEXT) {
         if (!(ptype = getptype (ptwire->USER, OPT_PARAMS)))
            continue;
         optparams = (optparam_list *) ptype->DATA;
         m=1;
         foundval=-1;
         tc1=0; tc2=0; dtemp=0;
         for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT) {
            var = ptopt->UNAME.SPECIAL;
            if (ptopt->TAG == '$') {
             value = opt_eval (ctx, ptopt);
             if (!eqt_resistrue (ctx)) {
                print_mlu_error(fig, ctx, var, ptopt->UDATA.EXPR, NULL, NULL);
                value = opt_eval (ctx, ptopt);
             }
             else if (!finite(value))
               avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, var, ptopt->UDATA.EXPR," : returned NaN or Inf");
             else if (MLU_TRACE_EQT)
                fprintf (stdout, "RESI ---> '%s' = %g\n", ptopt->UDATA.EXPR, value);
            }
            else
               value = ptopt->UDATA.VALUE;

            if (m_is_allowed('r') && !strcasecmp (var, "m"))
              m=mbk_long_round(value);
            else if (!strcasecmp (var, "val") || !strcasecmp (var,"r"))
            {
              foundval=value;
              if (value<0)
              {
                avt_errmsg(MBK_ERRMSG, "070", AVT_ERROR, ptopt->UDATA.EXPR, "resistance");
              }
            }
            else if (!strcasecmp (var, "tc1")) tc1=value;
            else if (!strcasecmp (var, "tc2")) tc2=value;
            else if (!strcasecmp (var, "dtemp")) dtemp=value;
         }
         
         if (foundval==-1) foundval=ptwire->RESI/SPI_SCALE_RESIFACTOR;
         
         ptwire->RESI = mcc_calcResiSimple(foundval, tc1, tc2, dtemp)* SPI_SCALE_RESIFACTOR/(double)m ;
         /*                
         if (foundval==-1)
           ptwire->RESI/=m;
         else
           ptwire->RESI =(foundval * SPI_SCALE_RESIFACTOR)/(double)m;
         */
         freeoptparams (optparams);
         ptwire->USER = delptype (ptwire->USER, OPT_PARAMS);
      }
   }

   // update transistor sizes
   for (pttrs = fig->LOTRS; pttrs; pttrs = pttrs->NEXT) {
     if (pttrs->X!=LONG_MIN) pttrs->X += x;
     if (pttrs->Y!=LONG_MIN) pttrs->Y += y;
      if (!(ptype = getptype (pttrs->USER, OPT_PARAMS)))
         continue;
      trans_l = 0.0;
      trans_w = 0.0;
      trans_as = 0.0;
      trans_ad = 0.0;
      diode_area = 0.0;
      diode_pj = 0.0;

      optparams = (optparam_list *) ptype->DATA;
      ptprev = NULL;
      saveparams = NULL;
      for (ptopt = optparams; ptopt; ptopt = ptnext) {
         ptnext = ptopt->NEXT;
         if (isknowntrsparam (ptopt->UNAME.STANDARD)) {
            if (**(ptopt->UNAME.STANDARD) == '$') {
               value = opt_eval (ctx, ptopt);
               if (!eqt_resistrue (ctx)) {
                  value = 0.0;
                  print_mlu_error(fig, ctx, *(ptopt->UNAME.STANDARD-1), ptopt->UDATA.EXPR, NULL, NULL);
        //          avt_errmsg(MBK_ERRMSG, "002", AVT_ERROR, ptopt->UDATA.EXPR);
               }
               else if (!finite(value))
                 avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, *(ptopt->UNAME.STANDARD-1), ptopt->UDATA.EXPR," : returned NaN or Inf");
               ptopt->UNAME.STANDARD -= 1;
               if (need_scale (*(ptopt->UNAME.STANDARD))) ptopt->UDATA.VALUE = value * scale;
               else ptopt->UDATA.VALUE = value;
               
            }
            if (m_is_allowed('t') && ptopt->UNAME.STANDARD == MBK_M) {
               ptduptrslist = addptype(ptduptrslist, value, pttrs);
               ptprev = ptopt;
               continue;
            }
            ptopt->NEXT = saveparams;
            saveparams = ptopt;
            if (ptprev == NULL)
               ptype->DATA = ptnext;
            else
               ptprev->NEXT = ptnext;
            continue;
         }
         if (ptopt->TAG == '$') {
            var = ptopt->UNAME.SPECIAL;
            value = opt_eval (ctx, ptopt);
            if (!eqt_resistrue (ctx)) {
               value = 0.0;
               print_mlu_error(fig, ctx, var, ptopt->UDATA.EXPR, NULL, NULL);
//               avt_errmsg(MBK_ERRMSG, "002", AVT_ERROR, ptopt->UDATA.EXPR);
            }
            else if (!finite(value))
              avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, var, ptopt->UDATA.EXPR," : returned NaN or Inf");
         }
         else {
            var = ptopt->UNAME.SPECIAL;
            value = ptopt->UDATA.VALUE;
         }

         if (!strcasecmp (var, "l")) {
            value*=scale;
            trans_l = value;
            pttrs->LENGTH = (long)(value * SCALE_X * 1E6 * SPI_SCALE_TRANSFACTOR + 0.5);
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "w")) {
            value*=scale;
            trans_w = value;
            pttrs->WIDTH = (long)(value * SCALE_X * 1E6 * SPI_SCALE_TRANSFACTOR + 0.5);
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "as")) {
            value*=scale*scale;
            trans_as = value;
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "ad")) {
            value*=scale*scale;
            trans_ad = value;
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "ps")) {
            value*=scale;
            pttrs->PS = (long)(value * SCALE_X * 1E6 * SPI_SCALE_TRANSFACTOR + 0.5);
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "pd")) {
            value*=scale;
            pttrs->PD = (long)(value * SCALE_X * 1E6 * SPI_SCALE_TRANSFACTOR + 0.5);
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "diode_area")) {
            value*=scale*scale;
            diode_area = value;
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else if (!strcasecmp (var, "diode_pj")) {
            value*=scale;
            diode_pj = value;
            if (MLU_TRACE_EQT)
               fprintf (stdout, "TRANS ---> %s: '%s' = %g\n", var, ptopt->UDATA.EXPR, value);
         }
         else {
            if (ptopt->TAG == '$') {
               ptopt->UNAME.SPECIAL = namealloc (var);
               ptopt->UDATA.VALUE = value;
               ptopt->TAG = ' ';
            }
            ptopt->NEXT = saveparams;
            saveparams = ptopt;
            if (ptprev == NULL)
               ptype->DATA = ptnext;
            else
               ptprev->NEXT = ptnext;
            continue;
         }
         ptprev = ptopt;
      }
      if (trans_w != 0 && trans_as != 0) {
         pttrs->XS =
            (long)(((pttrs->WIDTH != 0) ? (trans_as * SCALE_X * 1E6 * SPI_SCALE_TRANSFACTOR / trans_w) : 0) + 0.5);
      }
      if (trans_w != 0 && trans_ad != 0) {
         pttrs->XD =
            (long)(((pttrs->WIDTH != 0) ? (trans_ad * SCALE_X * 1E6 * SPI_SCALE_TRANSFACTOR / trans_w) : 0) + 0.5);
      }

      if (diode_area != 0 && diode_pj != 0) {
         pttrs->PD = (long)(diode_pj * SCALE_X * 1E6 * SPI_SCALE_DIODEFACTOR + 0.5);
         pttrs->XD = (long)(((diode_area * SPI_SCALE_DIODEFACTOR * SCALE_X * 1E6) / (diode_pj / 4)) + 0.5);
      }

      freeoptparams ((optparam_list *) ptype->DATA);
      pttrs->USER = delptype (pttrs->USER, OPT_PARAMS);
      if (saveparams)
         pttrs->USER = addptype (pttrs->USER, OPT_PARAMS, saveparams);
      if (!pttrs->LENGTH && MLU_TRACE_EQT)
         avt_errmsg(MBK_ERRMSG, "003", AVT_WARNING);
      if (!pttrs->WIDTH && MLU_TRACE_EQT)
         avt_errmsg(MBK_ERRMSG, "004", AVT_WARNING);
   }

   // create new transistors due to M factor and update lofigchain
   for (ptduptrs = ptduptrslist; ptduptrs; ptduptrs = ptduptrs->NEXT) {
      for (i=1; i < ptduptrs->TYPE; i++) {
         ptnewlotrs = rduplotrs((lotrs_list *)ptduptrs->DATA);
         if (ptnewlotrs->TRNAME) {
            sprintf(namebuf, "%s_m%d", ptnewlotrs->TRNAME, i);
            ptnewlotrs->TRNAME = namealloc(namebuf);
         }
         ptnewlotrs->NEXT = ptnewlotrslist;
         ptnewlotrslist = ptnewlotrs;
         ptype = getptype(ptnewlotrs->GRID->SIG->USER, LOFIGCHAIN);
         if (ptype) {
            ptype->DATA = addchain((chain_list *)ptype->DATA, ptnewlotrs->GRID);
         }
         ptype = getptype(ptnewlotrs->DRAIN->SIG->USER, LOFIGCHAIN);
         if (ptype) {
            ptype->DATA = addchain((chain_list *)ptype->DATA, ptnewlotrs->DRAIN);
         }
         ptype = getptype(ptnewlotrs->SOURCE->SIG->USER, LOFIGCHAIN);
         if (ptype) {
            ptype->DATA = addchain((chain_list *)ptype->DATA, ptnewlotrs->SOURCE);
         }
         if (ptnewlotrs->BULK) {
            ptype = getptype(ptnewlotrs->BULK->SIG->USER, LOFIGCHAIN);
            if (ptype) {
               ptype->DATA = addchain((chain_list *)ptype->DATA, ptnewlotrs->BULK);
            }
         }
      }
   }
   freeptype(ptduptrslist);
        
   // add new transistors due to M factor 
   fig->LOTRS = (lotrs_list *)append((chain_list *)ptnewlotrslist, (chain_list *)fig->LOTRS);

   // create new instances due to M factor and update lofigchain
   for (ptduptrs = ptduploinslist; ptduptrs; ptduptrs = ptduptrs->NEXT) {
      for (i=1; i < ptduptrs->TYPE; i++) {
         ptins=(loins_list *)ptduptrs->DATA;
         ptnewloins = rduploins(ptins);
         sprintf(namebuf, "%s_m%d", ptnewloins->INSNAME, i);
         ptnewloins->INSNAME = namealloc(namebuf);
         ptnewloins->NEXT = ptnewloinslist;
         ptnewloinslist = ptnewloins;
         
         if ((ptype = getptype (ptins->USER, PARAM_CONTEXT))!=NULL) 
         {
           ptnewloins->USER=addptype(ptnewloins->USER, PARAM_CONTEXT, eqt_dupvars((eqt_param *)ptype->DATA));
         }
         for (lc=ptnewloins->LOCON; lc!=NULL; lc=lc->NEXT)
         {
           ptype = getptype(lc->SIG->USER, LOFIGCHAIN);
           if (ptype) {
              ptype->DATA = addchain((chain_list *)ptype->DATA, lc);
           }
         }
      }
   }
   freeptype(ptduploinslist);
        
   // add new transistors due to M factor 
   fig->LOINS = (loins_list *)append((chain_list *)ptnewloinslist, (chain_list *)fig->LOINS);
    
//   eqt_term (ctx);
   if (defaults)
      eqt_free_param (defaults);

   for (i=minctx; i<cntctx; i++)
   {
     eqt_term(EQT_CONTEXT_HIERARCHY[i]);
     EQT_CONTEXT_HIERARCHY[i]=NULL;
   }
   Tx=Ty=R=A=0; // avoid warning
}


void mbk_transfert_loins_params (loins_list * li, lofig_list * origlf, lofig_list * newlf)
{
   eqt_ctx *ctx;
   ptype_list *ptype;
   eqt_param *temp;

   ctx = spi_init_eqt ();
   if ((ptype = getptype (origlf->USER, PARAM_CONTEXT))) {
      eqt_import_vars (ctx, (eqt_param *) ptype->DATA);
   }
   if ((ptype = getptype (li->USER, PARAM_CONTEXT))) {
      eqt_import_vars (ctx, (eqt_param *) ptype->DATA);
   }

   newlf->USER = addptype (newlf->USER, PARAM_CONTEXT, eqt_export_vars (ctx));
   eqt_term (ctx);
}


static void cleanlofigchain_from_to_be_freed_connectors (losig_list * ls)
{
   ptype_list *p;
   chain_list *cl, *prev, *next;
   if ((ls->FLAGS & LFCANDIDAT) == 0)
      return;
   ls->FLAGS &= ~LFCANDIDAT;
   if ((p = getptype (ls->USER, LOFIGCHAIN)) == NULL)
      return;

   prev = NULL;
   for (cl = (chain_list *) p->DATA; cl != NULL; cl = next) {
      next = cl->NEXT;
      if (((locon_list *) cl->DATA)->DIRECTION == -1) {
         if (prev == NULL)
            p->DATA = delchain ((chain_list *) p->DATA, (chain_list *) p->DATA);
         else
            prev->NEXT = delchain (prev->NEXT, cl);
      }
      else
         prev = cl;
   }
}

#ifdef BASE_STAT
extern long ynmsize, i_nbptype, i_nbchain, i_nbnum;
#endif

void flatten_setup_realname_from_hiername (lofig_list * lf, AdvancedNameAllocator * ana)
{
   loins_list *li;
   ptype_list *p0;
   locon_list *scanlocon;
   chain_list *scanchain;
   char *c, buf[2048];

   for (li = lf->LOINS; li != NULL; li = li->NEXT) {
      if ((p0 = getptype (li->USER, LF_HIERNAME_TO_NAMEALLOC)) != NULL) {
         AdvancedNameAllocName (ana, (int)(long)p0->DATA, buf);
         li->INSNAME = namealloc (buf);
         li->USER = delptype (li->USER, LF_HIERNAME_TO_NAMEALLOC);

         c = strrchr (buf, SEPAR);
         if (c == NULL)
            EXIT (77);
         *c = '\0';
         for (scanlocon = li->LOCON; scanlocon; scanlocon = scanlocon->NEXT) {
            if ((p0 = getptype (scanlocon->USER, PNODENAME)) != NULL) {
               for (scanchain = (chain_list *) p0->DATA; scanchain; scanchain = scanchain->NEXT)
                  scanchain->DATA = concatname (buf, (char *)scanchain->DATA);
            }
         }
      }
   }
}
// -------------------
static int buildmatchrule(inffig_list *ifl, char *section, mbk_match_rules *mr)
{
  chain_list *cl;
  mbk_CreateREGEX(mr, CASE_SENSITIVE, 0);
  if (ifl==NULL) return 0;
  if (inf_GetPointer(ifl, section, "", (void **)&cl))
  {
    while (cl!=NULL)
    {
      mbk_AddREGEX(mr, ((inf_assoc *)cl->DATA)->orig);
      cl=cl->NEXT;
    }
    return 1;
  }
  return 0;
}
// -------------------
static inline int sighasnorc(losig_list *ls)
{
  if (ls->PRCN==NULL || mbk_LosigIsVSS(ls) || mbk_LosigIsVDD(ls) || ls->PRCN->PWIRE==NULL) return 1;
  return 0;
}


static void lazy_duplosigalim(losig_list *scanlosig, losig_list *newsig)
{
  float val, val1;
  if (getlosigalim(newsig, &val) && getlosigalim(scanlosig, &val1))
    {
      if (fabs(val-val1)>1e-3)
        avt_errmsg(MBK_ERRMSG, "005", AVT_WARNING, getsigname(newsig), val, val1);
    }
  else
  {
   duplosigalim (scanlosig, newsig);
   newsig->ALIMFLAGS|=scanlosig->ALIMFLAGS;
  }
}

static void transfert_direction_information_from_instance_to_figure(loins_list *li, lofig_list *lf)
{
  locon_list *lc, *lflc;
  ptype_list *pt;
  for (lc=li->LOCON, lflc=lf->LOCON;  lc!=NULL && lflc!=NULL; lc=lc->NEXT, lflc=lflc->NEXT)
  {
    if (lflc->DIRECTION==UNKNOWN && (pt=getptype(lc->USER, LOCON_INFORMATION))!=NULL)
    {
       if ((long)pt->DATA==LOCON_INFORMATION_INPUT) lflc->DIRECTION=IN;
       else if ((long)pt->DATA==LOCON_INFORMATION_OUTPUT) lflc->DIRECTION=OUT;
    }
  }
}

// -------------------
void flattenlofig_bypointer (lofig_list * ptfig, chain_list * ptinslist, AdvancedNameAllocator * ana, char concat)
{
   struct st_interf {
      struct st_interf *next;
      int base;
      losig_list *sig;
   };

   struct st_contact {
      struct st_contact *next;
      locon_list *con1;      /* on instance */
      int node1;
      locon_list *con2;      /* on lofig of instance */
      int node2;
      int interf;
   };

   chain_list *nextscanctc;
   loins_list *ptins;
   lofig_list *figins;
   losig_list *scanlosig;
   int maxsig;
   locon_list *ptcon;
   chain_list *scanchain, *cl;
   ptype_list *ptptype;
   struct st_interf *interf;
   struct st_interf *ptinterf;
   struct st_contact *contact;
   struct st_contact *ptcontact;
   int nbinterf;
   num_list *sn1;
   num_list *sn2;
   lowire_list *scanwire, *curwire;
   int node1;
   struct st_interf *ctinterf;
   int *newnode;
   losig_list *newsig;
   locon_list *scanlocon;
   int maxnode;
   chain_list *scanctc;
   locon_list *nextlocon;
   void *forfree;
   loctc_list *ptctc;
   loctc_list *ptctc2;
   chain_list *prevctc;
   int othernode;
   int scanctcnode;
   losig_list *othersig;
   chain_list *posttreatsig = NULL;
//   ptype_list *ptbksig = NULL;
   int maxnode_itrf;
   int newnode_itrf;
   chain_list *chainhtab;
   chain_list *chainht;
   char buf_insname[2048];
   char *insname = buf_insname;
   int insname_length;
   int verif;
   locon_list *eptcon;

   lotrs_list *scantrs;
   loins_list *scanins;

   int i, cnt;
   char all;
   long key;
   ht *htab;

   chain_list *rebuildctc = NULL, *candidat_list = NULL;
   char iscache, *name;
   HeapAlloc contact_heap, interf_heap;
   eqt_param *param;
   ptype_list *ptype, *p0;
   int IDLE_LOCON = 0, GROUP_MODE = 1;
   struct st_interf *best;
   ptype_list *xy_ptype;
   xy_struct  *xy;
   int x, y, Tx, Ty, R, A;
   eqt_ctx *mc_ctx=NULL;


   long nbtr = 0, nbins = 0, nblocon = 0;
   long newlosig = 0, nblorc = 0;
   long cntpost = 0, cntrebui = 0;
   void *nextkey ;
   long nextitem ;

   mbk_match_rules IGNORE_TRANSISTOR;
   mbk_match_rules IGNORE_INSTANCE;
   mbk_match_rules IGNORE_RESISTANCE;
   mbk_match_rules IGNORE_CAPACITANCE, IGNORE_DIODE, *ic_or_id;
   mbk_match_rules IGNORE_NAMES;

   inffig_list *ifl;
   //---------NEW-------------
   ht *globalht=NULL;
   int local_concat, keptoldsig, has_ignore_names;
   // ------ end of declarations ------------
   
         MBK_HANDLE_GLOBAL_NODES=V_BOOL_TAB[__MBK_HANDLE_GLOBAL_NODES].VALUE;

   ifl=getloadedinffig(ptfig->NAME);
   buildmatchrule(ifl, INF_IGNORE_TRANSISTOR, &IGNORE_TRANSISTOR);
   buildmatchrule(ifl, INF_IGNORE_INSTANCE, &IGNORE_INSTANCE);
   buildmatchrule(ifl, INF_IGNORE_RESISTANCE, &IGNORE_RESISTANCE);
   buildmatchrule(ifl, INF_IGNORE_CAPACITANCE, &IGNORE_CAPACITANCE);
   buildmatchrule(ifl, INF_IGNORE_DIODE, &IGNORE_DIODE);
   has_ignore_names=buildmatchrule(ifl, INF_IGNORE_NAMES, &IGNORE_NAMES);

   CreateHeap (sizeof (struct st_contact), 1024, &contact_heap);
   CreateHeap (sizeof (struct st_interf), 1024, &interf_heap);

  if( rcn_getlofigcache( ptfig ) ) {
        avt_errmsg(MBK_ERRMSG, "006", AVT_FATAL, ptfig->NAME);
    return;
  }
        
   if ((ptype = getptype (ptfig->USER, PARAM_CONTEXT))) {
      flatten_parameters (ptfig, NULL, param = (eqt_param *) ptype->DATA, 0, 0, 0, 0, 0, 0, &mc_ctx);
   }
   else
      flatten_parameters (ptfig, NULL, NULL, 0, 0, 0, 0, 0, 0, &mc_ctx);

   /* recupere l'index maximum des signaux sur la figure */
/*   maxsig = -1;
   for (ptptype = ptfig->BKSIG; ptptype; ptptype = ptptype->NEXT) {
      if (ptptype->TYPE > maxsig) {
         maxsig = ptptype->TYPE;
         ptbksig = ptptype;
      }
   }

   scanlosig = (losig_list *) (ptbksig->DATA);
   for (i = 0; i < getsigsize (ptfig); i++) {
      if (scanlosig[i].INDEX > maxsig)
         maxsig = scanlosig[i].INDEX;
   }
*/
   if (ptfig->BKSIG.TAB==NULL) maxsig=0;
   else maxsig=ptfig->BKSIG.maxindex;
   maxsig++;


   for (scanlosig = ptfig->LOSIG; scanlosig != NULL; scanlosig = scanlosig->NEXT)
      scanlosig->FLAGS = 0;   // pas candidat

   /* si insname == NULL on met a plat au niveau transistor */

   if (ptinslist == NULL) {
      all = 'Y';
   }
   else {
      all = 'N';
      if (ptinslist->NEXT == NULL)
         GROUP_MODE = 0;
   }

   /* On verifie que le lofigchain est bien a jour. Tous les signaux d'une
    * lofig ont le ptype dans leur champ USER, meme si il est vide */

   if (!getptype (ptfig->LOSIG->USER, LOFIGCHAIN)) {
      avt_errmsg(MBK_ERRMSG, "007", AVT_WARNING, ptfig->NAME);

      lofigchain (ptfig);
   }

   if (rcn_getlofigcache (ptfig))
      iscache = 1;
   else
      iscache = 0;

   while (ptfig->LOINS != NULL) {
      if (all == 'Y') {
         ptins = ptfig->LOINS;
      }
      else {
         ptins = (loins_list *) ptinslist->DATA;
         ptinslist = ptinslist->NEXT;
      }

      if ((p0 = getptype (ptins->USER, LF_HIERNAME_TO_NAMEALLOC)) == NULL)
         strcpy (buf_insname, ptins->INSNAME);
      else
         AdvancedNameAllocName (ana, (int)(long)p0->DATA, buf_insname);

      insname_length = strlen (insname);

      figins = rduplofig (getlofig (ptins->FIGNAME, 'A'));

      lofigchain (figins);

      transfert_direction_information_from_instance_to_figure(ptins, figins);
      mbk_set_transistor_instance_connector_orientation_info (figins);

      if ((xy_ptype = getptype (ptins->USER, MSL_XY_PTYPE))) {
          xy = (xy_struct*)xy_ptype->DATA;
          x = xy->X;
          y = xy->Y;
          Tx = xy->Tx;
          Ty = xy->Ty;
          R = xy->R;
          A = xy->A;
      }
      else {
          x = 0;
          y = 0;
          Tx = 0;
          Ty = 0;
          R = 0;
          A = 0;
      }

      if ((ptype = getptype (ptins->USER, PARAM_CONTEXT))) {
         flatten_parameters (figins, ptins, param = (eqt_param *) ptype->DATA, x, y, Tx, Ty, R, A, &mc_ctx);
         ptins->USER = delptype (ptins->USER, PARAM_CONTEXT);
         if (param)
            eqt_free_param (param);
         if ((xy_ptype = getptype (ptins->USER, MSL_XY_PTYPE))) {
             mbkfree (xy_ptype->DATA);
             ptins->USER = delptype (ptins->USER, MSL_XY_PTYPE);
         }
      }
      else {
         fprintf (stderr, "NO CONTEXT FOR '%s'\n", ptins->INSNAME);
      }

      if ((ptptype=getptype(ptins->USER, BBOX_AS_UNUSED))!=NULL) name=(char *)ptptype->DATA;
      else name=NULL;

      if (concat != NO) {
         int insnameindex, removed=0;
         for (scanins = figins->LOINS; scanins; scanins = scanins->NEXT) {
            strcpy (&buf_insname[insname_length + 1], scanins->INSNAME);
            buf_insname[insname_length] = SEPAR;

            if (name!=NULL) scanins->USER=addptype(scanins->USER, BBOX_AS_UNUSED, name);

            if (mbk_CheckREGEX(&IGNORE_INSTANCE, buf_insname)==0)
              {
                insnameindex = AdvancedNameAlloc (ana, buf_insname);
                
                scanins->USER = addptype (scanins->USER, LF_HIERNAME_TO_NAMEALLOC, (void *)(long)insnameindex);
              }
            else
              {
                avt_log( LOGSTATPRS, 3, "instance '%s' ignored in circuit '%s'\n", buf_insname, ptfig->NAME);
                scanins->INSNAME=NULL, removed++; // marquage pour remove
              }
            buf_insname[insname_length] = '\0';
         }

         for (scantrs = figins->LOTRS; scantrs; scantrs = scantrs->NEXT) {
            if (scantrs->TRNAME)
               scantrs->TRNAME = concatname (insname, scantrs->TRNAME);

            if (name!=NULL) scantrs->USER=addptype(scantrs->USER, BBOX_AS_UNUSED, name);
            
            if (mbk_CheckREGEX(&IGNORE_TRANSISTOR, scantrs->TRNAME)==0)
              {
                mbk_flat_locon_pnodename (scantrs->DRAIN, ptins, insname, ana);
                mbk_flat_locon_pnodename (scantrs->GRID, ptins, insname, ana);
                mbk_flat_locon_pnodename (scantrs->SOURCE, ptins, insname, ana);
                
                if (scantrs->BULK)
                  if (scantrs->BULK->SIG)
                    mbk_flat_locon_pnodename (scantrs->BULK, ptins, insname, ana);
              }
            else
              {
                avt_log( LOGSTATPRS, 3, "transistor '%s' ignored in circuit '%s'\n", scantrs->TRNAME, ptfig->NAME);
                scantrs->TRNAME=(char *)1, removed++; // marquage? pour remove
              }
         }
         
         if (removed)
           {
             delflaggedloins(figins);
             delflaggedlotrs(figins, (char *)1);
             lofigchain (figins);
           }
      }
      for (ptcon = ptins->LOCON; ptcon; ptcon = ptcon->NEXT)
         if (ptcon->SIG->PRCN && gettabnode (ptcon->SIG->PRCN))
            freetable (ptcon->SIG);

      /* Si un seul signal a un RC, on en cree un sur tous les autres */

      for (ptcon = ptins->LOCON; ptcon; ptcon = ptcon->NEXT)
         if (ptcon->SIG->PRCN)
            break;

      if (!ptcon)
         for (scanlosig = figins->LOSIG; scanlosig; scanlosig = scanlosig->NEXT)
            if (scanlosig->PRCN)
               break;

      if (scanlosig || ptcon) {
         for (ptcon = ptins->LOCON; ptcon; ptcon = ptcon->NEXT) {
            if (!ptcon->SIG->PRCN/* || (ptcon->SIG->PRCN->PWIRE==NULL && ptcon->SIG->PRCN->PCTC==NULL)*/) {
               if (!ptcon->SIG->PRCN) addlorcnet (ptcon->SIG);
               ptptype = getptype( ptcon->SIG->USER, LOFIGCHAIN );
               for( scanchain = (chain_list*)ptptype->DATA ; scanchain ; scanchain = scanchain->NEXT ) {
                 eptcon = (locon_list*)scanchain->DATA ;
                 if( !eptcon->PNODE ) // inutile de le vérifier, mais bon...
                   eptcon->PNODE = addnum( NULL, 1l );
               }
               nblorc++;
            }

            if (gettabnode (ptcon->SIG->PRCN))
               freetable (ptcon->SIG);
         }

         for (scanlosig = figins->LOSIG; scanlosig; scanlosig = scanlosig->NEXT) {
            if (!scanlosig->PRCN/* || (scanlosig->PRCN->PWIRE==NULL && scanlosig->PRCN->PCTC==NULL)*/) {
              if (!scanlosig->PRCN) addlorcnet (scanlosig);
               ptptype = getptype( scanlosig->USER, LOFIGCHAIN );
               for( scanchain = (chain_list*)ptptype->DATA ; scanchain ; scanchain = scanchain->NEXT ) {
                 eptcon = (locon_list*)scanchain->DATA ;
                 if( !eptcon->PNODE ) // inutile de le vérifier, mais bon...
                   eptcon->PNODE = addnum( NULL, 1l );
               }
               nblorc++;
            }
            if (gettabnode (scanlosig->PRCN))
               freetable (scanlosig);
         }
      }

      addcorresp (figins, ptins);

      /* Au boulot */
      for (scanlosig = figins->LOSIG; scanlosig; scanlosig = scanlosig->NEXT) {
         if (iscache && scanlosig->PRCN && (scanlosig->PRCN->PWIRE || scanlosig->PRCN->PCTC)) 
            avt_errmsg(MBK_ERRMSG, "006", AVT_FATAL, ptfig->NAME);

         if (scanlosig->TYPE == INTERNAL) {
   //---------NEW-------------
           keptoldsig=0;
           local_concat=concat;
           
           if (MBK_HANDLE_GLOBAL_NODES && mbk_LosigIsGlobal(scanlosig) && sighasnorc(scanlosig))
             {
               if (globalht==NULL) 
                 {
                   globalht=addht(10);
                   for (newsig=ptfig->LOSIG; newsig!=NULL; newsig=newsig->NEXT)
                     /*if (mbk_LosigIsGlobal(newsig))*/ addhtitem(globalht, getsigname(newsig), (long)newsig);
                 }
               if ((key=gethtitem(globalht, getsigname(scanlosig)))!=EMPTYHT)
                 {
                   newsig=(losig_list *)key;
                   if (sighasnorc(newsig))
                     {
                       keptoldsig=1; // do nothing
                     }
                   else
                     {
                       newsig = addlosig (ptfig, maxsig, NULL, INTERNAL);
                     }
                 }
               else
                 {
                   newsig = addlosig (ptfig, maxsig, NULL, INTERNAL);
                   local_concat=NO;
//                   addhtitem(globalht, getsigname(scanlosig), (long)newsig);
                 }
             }
     //-------------
           else
             {
               newsig = addlosig (ptfig, maxsig, NULL, INTERNAL);
             }

            lazy_duplosigalim (scanlosig, newsig);
            if (!keptoldsig)
              {
                mbk_dupvcardnodes(scanlosig, newsig);
                if ((ptptype=getptype(newsig->USER, MBK_VCARD_NODES))!=NULL)
                    {
                      chain_list *cl;
                      vcardnodeinfo *vci;
                      for (cl=(chain_list *)ptptype->DATA; ptptype; ptptype=ptptype->NEXT)
                        {
                          vci=(vcardnodeinfo *)cl->DATA;
                          vci->name=concatname (insname, vci->name);
                        }
                    }
              }

            if (!keptoldsig) newsig->FLAGS = 0;

            if (!GROUP_MODE && (newsig->FLAGS & LFREBUILDCTC) == 0) 
              {
                rebuildctc = addchain (rebuildctc, newsig);
                cntrebui++;
              }
            
            newsig->FLAGS |= LFREBUILDCTC;
            
            if (!keptoldsig)
              {
                maxsig++;
                newlosig++;
                
                ptptype = newsig->USER = addptype (newsig->USER, LOFIGCHAIN, NULL);
                
                /* concatene les nom si necessaire */
                
                if (concat != NO && local_concat!=NO)
                  for (scanchain = scanlosig->NAMECHAIN; scanchain; scanchain = scanchain->NEXT)
                    scanchain->DATA = concatname (insname, (char *)(scanchain->DATA));
                
                newsig->NAMECHAIN = scanlosig->NAMECHAIN;
                scanlosig->NAMECHAIN = NULL;
                if (globalht!=NULL) addhtitem(globalht, getsigname(newsig), (long)newsig);
              }
            else
             ptptype = getptype (newsig->USER, LOFIGCHAIN);

            for (scanchain = getptype (scanlosig->USER, LOFIGCHAIN)->DATA; scanchain; scanchain = scanchain->NEXT) {
               ptcon = (locon_list *) (scanchain->DATA);
               if (!(keptoldsig && ptcon->TYPE==EXTERNAL))
               {
                 ptcon->SIG = newsig;
                 ptptype->DATA = (void *)addchain ((chain_list *) ptptype->DATA, ptcon);
               }
            }

            if (scanlosig->PRCN) {

              if (!keptoldsig)
                duplorcnet (newsig, scanlosig);
              else
                givelorcnet(newsig);

               if (concat != NO)
               {
                 lowire_list *nw;
                 for (scanwire = scanlosig->PRCN->PWIRE; scanwire; scanwire = nw) 
                 {
                   nw=scanwire->NEXT;
                   if ((p0=getptype(scanwire->USER, RESINAME))!=NULL)
                   {
                    p0->DATA=concatname (insname, (char *)p0->DATA);
                    if (mbk_CheckREGEX(&IGNORE_RESISTANCE, p0->DATA)!=0)
                     {
                        avt_log( LOGSTATPRS, 3, "resistor '%s' ignored in circuit '%s'\n", (char *)p0->DATA, ptfig->NAME);
                        dellowire(scanlosig, scanwire->NODE1, scanwire->NODE2);
                     }
                   }
                 }
               }
               
               /* Duplication des CTC */

               for (scanctc = scanlosig->PRCN->PCTC; scanctc; scanctc = nextscanctc) {
                  nextscanctc=scanctc->NEXT;
                  ptctc = (loctc_list *) scanctc->DATA;
                  if (!ptctc) {
                     avt_errmsg(MBK_ERRMSG, "008", AVT_ERROR, scanlosig->INDEX, insname);
                  }
                  if (concat != NO && ptctc->SIG1 == scanlosig)
                  {
                    if ((p0=getptype(ptctc->USER, MSL_CAPANAME))!=NULL)
                    {
                     p0->DATA=concatname (insname, (char *)p0->DATA);
                     if (rcn_isCapaDiode(ptctc)) ic_or_id=&IGNORE_DIODE; else ic_or_id=&IGNORE_CAPACITANCE;
                     if (mbk_CheckREGEX(ic_or_id, p0->DATA)!=0)
                      {
                         avt_log( LOGSTATPRS, 3, "capacitance '%s' ignored in circuit '%s'\n", (char *)p0->DATA, ptfig->NAME);
                         delloctc(ptctc);
                         continue;
                      }
                    }
                  }

                  if (ptctc->SIG1 == scanlosig) {
                     newsig->PRCN->PCTC = addchain (newsig->PRCN->PCTC, ptctc);
                     ptctc->SIG1 = newsig;
                  }

                  if (ptctc->SIG2 == scanlosig) {
                     newsig->PRCN->PCTC = addchain (newsig->PRCN->PCTC, ptctc);
                     ptctc->SIG2 = newsig;
                  }
               }
            }
         }
         else {

            /* cree la liste interf */
            interf = (struct st_interf *)AddHeapItem (&interf_heap);
            interf->base = 0;
            interf->next = NULL;
            interf->sig = scanlosig;


            scanlosig->FLAGS = 0;
            cl = getsigexternalconnectors (scanlosig);
            for (scanchain = cl; scanchain; scanchain = scanchain->NEXT) {
               ptcon = (locon_list *) scanchain->DATA;
               ptcon->DIRECTION = -1;
               IDLE_LOCON++;
               scanlocon = getcorrespinstanceconnector (ptcon);
               if (!scanlocon) {
                     avt_errmsg(MBK_ERRMSG, "009", AVT_FATAL, ptcon->NAME, insname);
               }

               scanlocon->DIRECTION = -1;

               ptinterf = (struct st_interf *)AddHeapItem (&interf_heap);
               ptinterf->base = 0;
               ptinterf->next = interf;
               ptinterf->sig = scanlocon->SIG;
               interf = ptinterf;

               if (!scanlocon->PNODE && ptcon->PNODE) {
                  for (maxnode_itrf = 0, sn1 = ptcon->PNODE; sn1; sn1 = sn1->NEXT, maxnode_itrf++);

                  newnode_itrf = addlonode (scanlocon->SIG, NULL);

                  for (i = 1; i <= maxnode_itrf; i++)
                     setloconnode (scanlocon, newnode_itrf);
               }

               if (scanlocon->PNODE && !ptcon->PNODE) {
                  for (maxnode_itrf = 0, sn1 = scanlocon->PNODE; sn1; sn1 = sn1->NEXT, maxnode_itrf++);

                  newnode_itrf = addlonode (ptcon->SIG, NULL);

                  for (i = 1; i <= maxnode_itrf; i++)
                     setloconnode (ptcon, newnode_itrf);
               }

            }
            {
               best = NULL;
               for (ptinterf = interf; ptinterf; ptinterf = ptinterf->next) {
                  if (ptinterf->sig != scanlosig) {
                     if (ptinterf->sig->FLAGS != 0) {
                        best = ptinterf;
                        break;
                     }
                     else
                        best = ptinterf;
                  }
               }
               ptinterf = best;
            }

            if (ptinterf) {
               struct st_interf *tempptinterf;
               newsig = ptinterf->sig;
               for (tempptinterf = interf; tempptinterf; tempptinterf = tempptinterf->next)
                 if (tempptinterf->sig != newsig)
                 {
                  lazy_duplosigalim (tempptinterf->sig, newsig);
                  if (mbk_LosigIsVDD(tempptinterf->sig)) mbk_SetLosigVDD(newsig);
                  else if (mbk_LosigIsVSS(tempptinterf->sig)) mbk_SetLosigVSS(newsig);
                 }
               ptptype = getptype (newsig->USER, LOFIGCHAIN);
               if ((ptinterf->sig->FLAGS & LFCANDIDAT) == 0) {
                  ptinterf->sig->FLAGS |= LFCANDIDAT;
                  candidat_list = addchain (candidat_list, ptinterf->sig);
                  if (!GROUP_MODE && (newsig->FLAGS & LFREBUILDCTC) == 0) {
                     rebuildctc = addchain (rebuildctc, newsig);
                     cntrebui++;
                  }

                  newsig->FLAGS |= LFREBUILDCTC;
               }
            }
            else {
               newsig = addlosig (ptfig, maxsig, NULL, INTERNAL);
               lazy_duplosigalim (scanlosig, newsig);
               newsig->FLAGS = 0;
               if (!GROUP_MODE && (newsig->FLAGS & LFREBUILDCTC) == 0) {
                  rebuildctc = addchain (rebuildctc, newsig);
                  cntrebui++;
               }

               newsig->FLAGS |= LFREBUILDCTC;


               maxsig++;
               newsig->USER = addptype (newsig->USER, LOFIGCHAIN, NULL);
               ptptype = newsig->USER;
            }
            if ((newsig->FLAGS & LFPOSTRAIT) == 0) {
               posttreatsig = addchain (posttreatsig, newsig);
               newsig->FLAGS |= LFPOSTRAIT;
               cntpost++;
            }

            /* cree la liste contact */
            contact = NULL;
            if (newsig->PRCN)
            {
               nbinterf = newsig->PRCN->NBNODE;
               if (nbinterf<=1) nbinterf=2; //should always be >=2
            }
            else
               nbinterf = 1;


            cl = getsigexternalconnectors (scanlosig);
            for (scanchain = cl; scanchain; scanchain = scanchain->NEXT) {
               ptcon = (locon_list *) (scanchain->DATA);
               scanlocon = getcorrespinstanceconnector (ptcon);
               if (!scanlocon) {
                     avt_errmsg(MBK_ERRMSG, "009", AVT_FATAL, ptcon->NAME, ptfig->NAME);
               }
               cleancorrespinstanceconnector (ptcon);
               if (scanlocon->PNODE || ptcon->PNODE) {
                  for (sn1 = scanlocon->PNODE, sn2 = ptcon->PNODE; sn1 && sn2; sn1 = sn1->NEXT, sn2 = sn2->NEXT) {
                     ptcontact = (struct st_contact *)AddHeapItem (&contact_heap);

                     ptcontact->next = contact;
                     ptcontact->con1 = scanlocon;
                     ptcontact->con2 = ptcon;
                     ptcontact->node1 = sn1->DATA;
                     ptcontact->node2 = sn2->DATA;
                     if (scanlocon->SIG == newsig)
                        ptcontact->interf = sn1->DATA;
                     else {
                        ptcontact->interf = nbinterf++;
                     }

                     contact = ptcontact;
                  }

                  if (scanlocon->PNODE->NEXT == NULL && sn2) {
                     for (; sn2; sn2 = sn2->NEXT) {
                        ptcontact = (struct st_contact *)AddHeapItem (&contact_heap);
                        ptcontact->next = contact;
                        ptcontact->con1 = scanlocon;
                        ptcontact->con2 = ptcon;
                        ptcontact->node1 = contact->node1;
                        ptcontact->node2 = sn2->DATA;
                        ptcontact->interf = contact->interf;
                        contact = ptcontact;
                     }
                  }
                  else if (sn1 || sn2) {
                     avt_errmsg(MBK_ERRMSG, "010", AVT_FATAL, ptcon->NAME, figins->NAME);
                  }
               }
               else {
                  ptcontact = (struct st_contact *)AddHeapItem (&contact_heap);

                  ptcontact->next = contact;
                  ptcontact->con1 = scanlocon;
                  ptcontact->con2 = ptcon;
                  ptcontact->node1 = 0;
                  ptcontact->node2 = 0;
                  ptcontact->interf = 0;

                  contact = ptcontact;
               }
            }

            cleansigexternalconnectors (scanlosig);
            freechain (cl);

            /* Met a jour le champs base des structures inter */
            if (interf->sig->PRCN) {
               if (!newsig->PRCN) {
                  addlorcnet (newsig);
                  nblorc++;
               }
               maxnode = nbinterf;

               for (ptinterf = interf; ptinterf; ptinterf = ptinterf->next) {
                  if (ptinterf->sig != newsig) {
                     ptinterf->base = maxnode - 1;
                     maxnode = maxnode + ptinterf->sig->PRCN->NBNODE - 1;

                     rcn_addcapa (newsig, rcn_getcapa (figins, ptinterf->sig));
                  }
                  else
                     ptinterf->base = 0;
               }
            }

            /* calcul le type de newsig */

            for (ptinterf = interf; ptinterf; ptinterf = ptinterf->next)
               if (ptinterf->sig != scanlosig && ptinterf->sig->TYPE == EXTERNAL)
                  break;

            R=0;
            if (ptinterf)
            {
               if (newsig->TYPE == INTERNAL && !FLATTEN_KEEP_ALL_NAMES)
               {
                  freechain(newsig->NAMECHAIN);
                  newsig->NAMECHAIN=NULL;
                  R=1;
               }
               newsig->TYPE = EXTERNAL;
            }
            else
               newsig->TYPE = INTERNAL;

            /* met a jour le nom de newsig */
            A=(has_ignore_names==0 && newsig->NAMECHAIN == NULL && R==0);
            for (ptinterf = interf; ptinterf; ptinterf = ptinterf->next) {
               if (ptinterf->sig == newsig)
                  continue;

               if (A || has_ignore_names || FLATTEN_KEEP_ALL_NAMES == 1 || (ptinterf->sig!=scanlosig && (ptinterf->sig->TYPE==EXTERNAL || newsig->TYPE == INTERNAL)))
               {
                 if (concat != NO) {
                    if (ptinterf->sig == scanlosig)
                       for (scanchain = ptinterf->sig->NAMECHAIN; scanchain; scanchain = scanchain->NEXT)
                        {
                          scanchain->DATA = concatname (insname, (char *)scanchain->DATA);
                        }
                 }

                 newsig->NAMECHAIN = append (ptinterf->sig->NAMECHAIN, newsig->NAMECHAIN);
                 ptinterf->sig->NAMECHAIN = NULL;

                 if (has_ignore_names)
                 {
                   cl=NULL;
                   for (scanchain = newsig->NAMECHAIN; scanchain; scanchain = scanchain->NEXT)
                    {                      
                      if (mbk_CheckREGEX(&IGNORE_NAMES, (char *)scanchain->DATA)==0 || (cl==NULL && scanchain->NEXT==NULL))
                        cl=addchain(cl, scanchain->DATA);
                    }
                   freechain(newsig->NAMECHAIN);
                   newsig->NAMECHAIN=cl;
                 }
               }
            }

            if (newsig->TYPE == INTERNAL && !FLATTEN_KEEP_ALL_NAMES && countchain(newsig->NAMECHAIN)>1)
            {
              char *bestname;
              bestname=getsigname(newsig);
              freechain(newsig->NAMECHAIN);
              newsig->NAMECHAIN=addchain(NULL, bestname);
            }

            /* Pour chaque signal S de interf */
            for (ptinterf = interf; ptinterf; ptinterf = ptinterf->next) {
               if (ptinterf->sig == newsig)
                  continue;

               if ((ptinterf->sig->FLAGS & LFCANDIDAT) != 0)
               {
                  ptinterf->sig->FLAGS &= ~LFCANDIDAT;
                  candidat_list=delchaindata(candidat_list, ptinterf->sig);
               }
               /* Pour chaque connecteur C de S */
               for (scanchain =
                   (chain_list *) (getptype (ptinterf->sig->USER, LOFIGCHAIN)->DATA);
                   scanchain; scanchain = scanchain->NEXT) {
                  ptcon = (locon_list *) (scanchain->DATA);

                  ptcontact = NULL;
                  if (ptcon->DIRECTION == -1)
                     for (ptcontact = contact; ptcontact; ptcontact = ptcontact->next)
                        if (ptcon == ptcontact->con1 || ptcon == ptcontact->con2)
                           break;

                  /* Si C n'est pas sur l'interface */
                  if (!ptcontact) {
                     ptptype->DATA = (void *)addchain ((chain_list *) ptptype->DATA, ptcon);

                     for (sn1 = ptcon->PNODE; sn1; sn1 = sn1->NEXT) {
                        for (ptcontact = contact; ptcontact; ptcontact = ptcontact->next) {
                           if (ptcontact->node1 == sn1->DATA && ptcontact->con1->SIG == ptcon->SIG)
                              break;
                           if (ptcontact->node2 == sn1->DATA && ptcontact->con2->SIG == ptcon->SIG)
                              break;
                        }

                        if (ptcontact)
                           /* cas particulier : si un noeud appartient aussi a un
                            * connecteur de l'interface */
                           sn1->DATA = ptcontact->interf;
                        else
                           sn1->DATA = sn1->DATA + ptinterf->base;
                     }

                     ptcon->SIG = newsig;
                     for (sn1 = ptcon->PNODE; sn1; sn1 = sn1->NEXT)
                        if (ptcon->SIG->PRCN->NBNODE <= sn1->DATA)
                           ptcon->SIG->PRCN->NBNODE = sn1->DATA + 1;
                  }
               }

               if (ptinterf->sig->PRCN) {
                  /* Pour chaque noeud de S, on calcule son nouvel index dans
                   * un tableau newnode */
                  newnode = mbkalloc (sizeof (int) * (ptinterf->sig->PRCN->NBNODE + 1));
                  memset (newnode, 0, sizeof (int) * (ptinterf->sig->PRCN->NBNODE + 1));

                  for (ptcontact = contact; ptcontact; ptcontact = ptcontact->next) {
                     if (ptinterf->sig == ptcontact->con1->SIG) {
                        newnode[ptcontact->node1] = ptcontact->interf;
                     }
                     else if (ptinterf->sig == ptcontact->con2->SIG) {
                        newnode[ptcontact->node2] = ptcontact->interf;
                     }
                  }

                  for (node1 = 1; node1 <= ptinterf->sig->PRCN->NBNODE; node1++)
                     if (newnode[node1] == 0)
                        newnode[node1] = node1 + ptinterf->base;

                  if ((ptptype=getptype(ptinterf->sig->USER, MBK_VCARD_NODES))!=NULL)
                    {
                      chain_list *cl;
                      vcardnodeinfo *vci;
                      for (cl=(chain_list *)ptptype->DATA; ptptype; ptptype=ptptype->NEXT)
                        {
                          vci=(vcardnodeinfo *)cl->DATA;
                          if (vci->rcn<=ptinterf->sig->PRCN->NBNODE)
                            mbk_addvcardnode(newsig, concatname (insname, vci->name), vci->value, newnode[vci->rcn]);
                        }
                    }

                  /* Pour chaque wire W de S */
                  for (scanwire = ptinterf->sig->PRCN->PWIRE; scanwire; scanwire = scanwire->NEXT) {
                     if ((p0=getptype(scanwire->USER, RESINAME))!=NULL)
                      {
                        name=concatname (insname, (char *)p0->DATA);
                        if (mbk_CheckREGEX(&IGNORE_RESISTANCE, name)!=0)
                        {
                           avt_log( LOGSTATPRS, 3, "resistor '%s' ignored in circuit '%s'\n", name, ptfig->NAME);
                           continue;
                        }
                      }
                     curwire = addlowire (newsig,
                                     scanwire->FLAG,
                                     scanwire->RESI,
                                     scanwire->CAPA, newnode[scanwire->NODE1], newnode[scanwire->NODE2]
                        );
                     if (curwire!=NULL && p0!=NULL)
                      curwire->USER=addptype(curwire->USER, RESINAME, name);
                     
                     if (curwire && (p0 = getptype (scanwire->USER, OPT_PARAMS)))
                        curwire->USER =
                           addptype (curwire->USER, OPT_PARAMS,
                                   dupoptparamlst ((optparam_list *) p0->DATA));
                  }

                  /* Pour chaque CTC de S */
                  for (scanctc = ptinterf->sig->PRCN->PCTC; scanctc; scanctc = scanctc->NEXT) {

                     ptctc = (loctc_list *) scanctc->DATA;

                     if ((p0=getptype(ptctc->USER, MSL_CAPANAME))!=NULL && ptctc->SIG1 == ptinterf->sig)
                     {
                       name=concatname (insname, (char *)p0->DATA);
                       if (rcn_isCapaDiode(ptctc)) ic_or_id=&IGNORE_DIODE; else ic_or_id=&IGNORE_CAPACITANCE;
                       if (mbk_CheckREGEX(ic_or_id, name)!=0)
                        {
                           avt_log( LOGSTATPRS, 3, "capacitance '%s' ignored in circuit '%s'\n", name, ptfig->NAME);
                           rcn_removecapa(newsig, ptctc->CAPA);
                           delloctc(ptctc);
                           continue;
                        }
                       p0->DATA=name;
                     }

                     if (ptctc->SIG1 == ptinterf->sig) {
                        ptctc->SIG1 = newsig;
                        if (ptctc->NODE1)
                           ptctc->NODE1 = newnode[ptctc->NODE1];
                     }
                     else {
                        for (ctinterf = interf; ctinterf; ctinterf = ctinterf->next) {
                           if (ptctc->SIG1 == ctinterf->sig) {
                              ptctc->SIG1 = newsig;

                              if (ptctc->NODE1) {
                                 for (ptcontact = contact; ptcontact; ptcontact = ptcontact->next) {
                                    if ((ctinterf->sig == ptcontact->con1->SIG &&
                                        ptctc->NODE1 == ptcontact->node1) ||
                                       (ctinterf->sig == ptcontact->con2->SIG &&
                                        ptctc->NODE1 == ptcontact->node2)) {
                                       ptctc->NODE1 = ptcontact->interf;
                                       break;
                                    }
                                 }
                                 if (!ptcontact)
                                    ptctc->NODE1 = ptctc->NODE1 + ctinterf->base;
                                 break;
                              }
                           }
                        }
                     }

                     if (ptctc->SIG2 == ptinterf->sig) {
                        ptctc->SIG2 = newsig;
                        if (ptctc->NODE2)
                           ptctc->NODE2 = newnode[ptctc->NODE2];
                     }
                     else {
                        for (ctinterf = interf; ctinterf; ctinterf = ctinterf->next) {
                           if (ptctc->SIG2 == ctinterf->sig) {
                              ptctc->SIG2 = newsig;

                              if (ptctc->NODE2) {
                                 for (ptcontact = contact; ptcontact; ptcontact = ptcontact->next) {
                                    if ((ctinterf->sig == ptcontact->con1->SIG &&
                                        ptctc->NODE2 == ptcontact->node1) ||
                                       (ctinterf->sig == ptcontact->con2->SIG &&
                                        ptctc->NODE2 == ptcontact->node2)) {
                                       ptctc->NODE2 = ptcontact->interf;
                                       break;
                                    }
                                 }
                                 if (!ptcontact)
                                    ptctc->NODE2 = ptctc->NODE2 + ctinterf->base;
                                 break;
                              }
                           }
                        }
                     }

                     if (!ptctc) {
                        avt_errmsg(MBK_ERRMSG, "011", AVT_ERROR, scanlosig->INDEX, insname);
                     }
                     newsig->PRCN->PCTC = addchain (newsig->PRCN->PCTC, ptctc);
                  }

                  mbkfree (newnode);
               }
            }

            for (ptinterf = interf; ptinterf; ptinterf = ptinterf->next) {
               if ((ptinterf->sig != scanlosig) && (ptinterf->sig != newsig)) {
                  ptptype = getptype (ptinterf->sig->USER, LOFIGCHAIN);
                  freechain ((chain_list *) ptptype->DATA);
                  ptptype->DATA = NULL;
                  if (ptinterf->sig->PRCN && ptinterf->sig->PRCN->PCTC) {
                     freechain (ptinterf->sig->PRCN->PCTC);
                     ptinterf->sig->PRCN->PCTC = NULL;
                  }
                  /*if (ptinterf->sig->FLAGS != 0)
                     EXIT (5);*/
                  dellosig (ptfig, ptinterf->sig->INDEX);
               }
            }

            for (ptcontact = contact; ptcontact; ptcontact = (struct st_contact *)forfree) {
               forfree = ptcontact->next;
               DelHeapItem (&contact_heap, ptcontact);
            }

            for (ptinterf = interf; ptinterf; ptinterf = (struct st_interf *)forfree) {
               forfree = ptinterf->next;
               DelHeapItem (&interf_heap, ptinterf);
            }
         }
         /* vire scanlosig */
         ptptype = getptype (scanlosig->USER, LOFIGCHAIN);
         freechain ((chain_list *) ptptype->DATA);
         ptptype->DATA = NULL;

         if (scanlosig->PRCN && scanlosig->PRCN->PCTC) {
            freechain (scanlosig->PRCN->PCTC);
            scanlosig->PRCN->PCTC = NULL;
         }

         dellosig (figins, scanlosig->INDEX);


      }
      if (all == 'Y') {       
         delloins (ptfig, ptins->INSNAME);
      }
      else
         ptins->INSNAME = NULL;   // flag pour delete

      // zinaps le 14/1/2003
      if (FlattenOnCreateLOINS != NULL)
         for (scanins = figins->LOINS; scanins; scanins = scanins->NEXT)
            FlattenOnCreateLOINS (scanins);

      if (FlattenOnCreateLOTRS != NULL)
         for (scantrs = figins->LOTRS; scantrs; scantrs = scantrs->NEXT)
            FlattenOnCreateLOTRS (scantrs);

      for (scantrs = figins->LOTRS; scantrs; scantrs = scantrs->NEXT) {
         nbtr++;
         nblocon += 4;
      }
      for (scanins = figins->LOINS; scanins; scanins = scanins->NEXT) {
         locon_list *lc;
         nbins++;
         for (lc = scanins->LOCON; lc != NULL; lc = lc->NEXT)
            nblocon++;
      }

      ptfig->LOINS = (loins_list *) append ((chain_list *) figins->LOINS, (chain_list *) ptfig->LOINS);
      ptfig->LOTRS = (lotrs_list *) append ((chain_list *) figins->LOTRS, (chain_list *) ptfig->LOTRS);

      figins->LOINS = NULL;
      figins->LOTRS = NULL;

      for (scanlocon = figins->LOCON; scanlocon; scanlocon = nextlocon) {
         nextlocon = scanlocon->NEXT;
         freenum (scanlocon->PNODE);
         delloconuser (scanlocon);
         mbkfree ((void *)scanlocon);
      }
      figins->LOCON = NULL;

      mbk_free_NewBKSIG(&figins->BKSIG);
/*      for (ptptype = figins->BKSIG; ptptype; ptptype = ptptype->NEXT)
         mbkfree ((void *)ptptype->DATA);
      freeptype (figins->BKSIG);
      figins->BKSIG = NULL;*/
      figins->LOSIG = NULL;

      freechain (figins->MODELCHAIN);
      figins->MODELCHAIN = NULL;

      /* Les seules valeurs qui peuvent arriver sont : PTSIGSIZE */

      dellofiguser (figins);

      if (figins->MODELCHAIN ||
         figins->LOCON || figins->LOSIG || figins->BKSIG.TAB || figins->LOINS || figins->LOTRS || figins->USER) {
             avt_errmsg(MBK_ERRMSG, "012", AVT_WARNING, figins->NAME, figins->USER->TYPE);
      }
      mbkfree ((void *)figins);

      if (all == 'N' && ptinslist == NULL)
         break;

      if (IDLE_LOCON > 1000000) {
         IDLE_LOCON = 0;
         for (cl = candidat_list; cl != NULL; cl = cl->NEXT)
            cleanlofigchain_from_to_be_freed_connectors ((losig_list *) cl->DATA);
         freechain (candidat_list);
         candidat_list = NULL;
      }
   }

   if (all == 'N') 
     {
       int cnt = 0;
       for (cl = candidat_list; cl != NULL; cl = cl->NEXT, cnt++)
         cleanlofigchain_from_to_be_freed_connectors ((losig_list *) cl->DATA);
       freechain (candidat_list);
       delflaggedloins (ptfig);
     }
   else
     {
       for (cl = candidat_list; cl != NULL; cl = cl->NEXT)
         cleanlofigchain_from_to_be_freed_connectors ((losig_list *) cl->DATA);
       freechain (candidat_list);
       candidat_list = NULL;
     }
   
   /* Post traitement sur les CTC dont un des noeuds est a 0 */

   /* Variable de debuggage */
   verif = 0;

   /* Regroupe les CTC en double */
   for (scanchain = posttreatsig; scanchain; scanchain = scanchain->NEXT) {
      scanlosig = (losig_list *) (scanchain->DATA);

      scanlosig->FLAGS &= ~LFPOSTRAIT;
      if ((scanlosig->INDEX == 0) || (!scanlosig->PRCN) || !scanlosig->PRCN->PCTC)
         continue;

      htab = addht (scanlosig->PRCN->NBNODE + 100);

      for (scanctc = scanlosig->PRCN->PCTC; scanctc; scanctc = scanctc->NEXT) {
         ptctc = (loctc_list *) (scanctc->DATA);

         if (ptctc->CAPA >= 0.0) {
            if (ptctc->SIG1 == scanlosig) {
               othersig = ptctc->SIG2;
               othernode = ptctc->NODE2;
               scanctcnode = ptctc->NODE1;
            }
            else {
               othersig = ptctc->SIG1;
               othernode = ptctc->NODE1;
               scanctcnode = ptctc->NODE2;
            }
            if (othersig == scanlosig && othernode == scanctcnode) {
               if (!GROUP_MODE && (othersig->FLAGS & LFREBUILDCTC) == 0) {
                  rebuildctc = addchain (rebuildctc, othersig);
               }
               othersig->FLAGS |= LFREBUILDCTC;
               ptctc->CAPA = -1.0;
               verif = verif + 2;
            }
            else {
               key = othersig->INDEX << 18;
               key = (key | (othernode << 7));
               key = (key | scanctcnode);

               if ((chainhtab = (chain_list *) gethtitem (htab, (void *)key)) == (chain_list *) EMPTYHT) {
                  chainhtab = addchain (NULL, ptctc);
                  addhtitem (htab, (void *)key, (long)chainhtab);
               }
               else {
                  for (chainht = chainhtab; chainht; chainht = chainht->NEXT) {
                     ptctc2 = (loctc_list *) (chainht->DATA);

                     if (ptctc2->CAPA < 0.0)
                        continue;

                     if (((ptctc2->SIG1 == scanlosig && ptctc2->SIG2 == othersig) &&
                         (ptctc2->NODE1 == scanctcnode && ptctc2->NODE2 == othernode)) ||
                        ((ptctc2->SIG2 == scanlosig && ptctc2->SIG1 == othersig) &&
                         (ptctc2->NODE2 == scanctcnode && ptctc2->NODE1 == othernode))) {
                        if (!GROUP_MODE && (othersig->FLAGS & LFREBUILDCTC) == 0) {
                           rebuildctc = addchain (rebuildctc, othersig);
                        }
                        othersig->FLAGS |= LFREBUILDCTC;
//                        ptctc->CAPA += ptctc2->CAPA;
//                        ptctc2->CAPA = -1.0;
                        ptctc2->CAPA += ptctc->CAPA;
                        if ((cnt=rcn_isCapaDiode(ptctc))!=0) rcn_setCapaDiode(ptctc2, cnt);
                        ptctc->CAPA = -1.0;
                        verif = verif + 2;
                     }
                  }
                  if (ptctc->CAPA>0)
                    chainhtab = addchain (chainhtab, ptctc);
//                  chainhtab = addchain (chainhtab, ptctc);
                  addhtitem (htab, (void *)key, (long)chainhtab);
               }
            }
         }
      }

      scanhtkey( htab, 1, &nextkey, &nextitem );
      while( nextitem != EMPTYHT ) {
        freechain( (chain_list*)nextitem );
        scanhtkey( htab, 0, &nextkey, &nextitem );
      }
      
      delht (htab);
   }



   // Greg, le 30/09/02 : il faut assurer la cohérence des champs PREV dans les 
   // ctc.... gros bordel.

   if (!GROUP_MODE) {
      for (scanchain = rebuildctc; scanchain; scanchain = scanchain->NEXT) {
         newsig = (losig_list *) scanchain->DATA;

         /*    if( !getptype( newsig->USER, FLATTEN_POSTRAIT ) ) {
            newsig->USER = addptype( newsig->USER, FLATTEN_POSTRAIT, NULL );
          */
/*         if ((newsig->FLAGS & LFPOSTRAIT) == 0) {
            newsig->FLAGS |= LFPOSTRAIT;*/
            if (newsig->PRCN) {
               prevctc = NULL;
               for (scanctc = newsig->PRCN->PCTC; scanctc; scanctc = nextscanctc) {
                  nextscanctc=scanctc->NEXT;
                  ptctc = (loctc_list *) scanctc->DATA;
                  
                  if (ptctc->SIG1==ptctc->SIG2)
                   {
                     // la ctc est presente 2 fois dans la liste
                     // on l'enleve la 2eme fois
                     if (ptctc->CAPA<-1.5) freeloctc(ptctc); 
                     else ptctc->CAPA=-2;
                     scanctc->NEXT=NULL; freechain(scanctc);
                     if (prevctc==NULL) newsig->PRCN->PCTC=nextscanctc;
                     else prevctc->NEXT=nextscanctc;
                   }
                  else
                   {
                     if (ptctc->SIG1 == newsig)
                       ptctc->PREV1 = prevctc;
                     else
                       ptctc->PREV2 = prevctc;
                     prevctc = scanctc;
                   }
               }
//            }
         }
      }
   }
   else {
      for (newsig = ptfig->LOSIG; newsig != NULL; newsig = newsig->NEXT) {
         if ((newsig->FLAGS & LFREBUILDCTC) != 0) {
            if (newsig->PRCN) {
               prevctc = NULL;
               for (scanctc = newsig->PRCN->PCTC; scanctc; scanctc = nextscanctc) {
                  nextscanctc=scanctc->NEXT;
                  ptctc = (loctc_list *) scanctc->DATA;
                  if (ptctc->SIG1==ptctc->SIG2)
                   {
                     // la ctc est presente 2 fois dans la liste
                     // on l'enleve la 2eme fois
                     if (ptctc->CAPA<-1.5) freeloctc(ptctc); 
                     else ptctc->CAPA=-2;
                     scanctc->NEXT=NULL; freechain(scanctc);
                     if (prevctc==NULL) newsig->PRCN->PCTC=nextscanctc;
                     else prevctc->NEXT=nextscanctc;
                   }
                  else
                   {
                     if (ptctc->SIG1 == newsig)
                       ptctc->PREV1 = prevctc;
                     else
                       ptctc->PREV2 = prevctc;
                     prevctc = scanctc;
                   }
               }
            }
         }
      }
   }

   /* On fait le menage en virant les CTC dont la capa vaut -1 */

   for (scanchain = posttreatsig; scanchain; scanchain = scanchain->NEXT) {
      scanlosig = (losig_list *)scanchain->DATA;
      if ((scanlosig->INDEX == 0) || (!scanlosig->PRCN))
         continue;

      for (scanctc = scanlosig->PRCN->PCTC; scanctc; scanctc = nextscanctc) {
         nextscanctc = scanctc->NEXT;
         ptctc = (loctc_list *)scanctc->DATA;

         if (ptctc->CAPA < 0.0) delloctc(ptctc);
      }
      rcn_setcapa( scanlosig, rcn_calccapa( scanlosig ) );
   }
   freechain (posttreatsig);

   for (ptcon=ptfig->LOCON; ptcon!=NULL; ptcon=ptcon->NEXT)
   {
     for (cl=ptcon->SIG->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
       if (cl->DATA==ptcon->NAME) break;
     if (cl==NULL) ptcon->SIG->NAMECHAIN=addchain(ptcon->SIG->NAMECHAIN, ptcon->NAME);
   }
#if DEBUGCTC
   if (dump) {
      chain_list *cl;
      for (newsig = ptfig->LOSIG; newsig != NULL; newsig = newsig->NEXT) {
         prevctc = NULL;
         for (scanctc = newsig->PRCN->PCTC; scanctc != NULL; scanctc = scanctc->NEXT) {
            ptctc = (loctc_list *) scanctc->DATA;
            if (ptctc->SIG1 == newsig && ptctc->PREV1 != prevctc) {
               printf ("**err(15) %s vs %s\n", newsig->NAMECHAIN->DATA, ptctc->SIG2->NAMECHAIN->DATA);
               break;
            }
            if (ptctc->SIG2 == newsig && ptctc->PREV2 != prevctc) {
               printf ("**err(16) %s vs %s\n", newsig->NAMECHAIN->DATA, ptctc->SIG1->NAMECHAIN->DATA);
               break;
            }
            if (!(ptctc->SIG1 == newsig || ptctc->SIG2 == newsig))
               exit (16);
            prevctc = scanctc;
         }
      }
   }
#endif
   if (globalht!=NULL) delht(globalht);
   freechain (rebuildctc);

   mbk_FreeREGEX(&IGNORE_TRANSISTOR);
   mbk_FreeREGEX(&IGNORE_INSTANCE);
   mbk_FreeREGEX(&IGNORE_RESISTANCE);
   mbk_FreeREGEX(&IGNORE_CAPACITANCE);
   mbk_FreeREGEX(&IGNORE_DIODE);
   mbk_FreeREGEX(&IGNORE_NAMES);

   DeleteHeap (&interf_heap);
   DeleteHeap (&contact_heap);
   if (mc_ctx!=NULL) eqt_term(mc_ctx);
}

void debugctc2 (lofig_list * ptfig)
{
   losig_list *sig;
   chain_list *scanchain;
   loctc_list *ptctc;

   printf ("Information sur la figure %s.\n", ptfig->NAME);
   for (sig = ptfig->LOSIG; sig; sig = sig->NEXT) {
      printf ("Signal %2ld (%16lX)\n", sig->INDEX, (long)sig);
      if (sig->PRCN) {
         for (scanchain = sig->PRCN->PCTC; scanchain; scanchain = scanchain->NEXT) {
            ptctc = (loctc_list *) (scanchain->DATA);
            printf ("  CTC (%16lX) entre le signal %2ld (%16lX) et le signal %2ld (%16lX).\n",
                  (long)ptctc, ptctc->SIG1->INDEX, (long)ptctc->SIG1, ptctc->SIG2->INDEX, (long)ptctc->SIG2);
         }
      }
   }
}

void debugctc (losig_list * headlosig, int niveau)
{
   losig_list *scanlosig;
   chain_list *scanctc;
   chain_list *scanctc2;
   loctc_list *ptctc;

   /* Boucle de debuggage */
   for (scanlosig = headlosig; scanlosig; scanlosig = scanlosig->NEXT) {
      if (!scanlosig->PRCN)
         continue;

      for (scanctc = scanlosig->PRCN->PCTC; scanctc; scanctc = scanctc->NEXT) {
         ptctc = (loctc_list *) (scanctc->DATA);
         if (getptype (ptctc->USER, FLATTEN_CTC)) {
            printf ("(%d) FLATTEN_CTC trouve dans la CTC (%16lX)  entre %ld.%ld et %ld.%ld.\n",
                  niveau, (long)ptctc, ptctc->SIG1->INDEX, ptctc->NODE1, ptctc->SIG2->INDEX, ptctc->NODE2);
            EXIT (-1);
         }

         for (scanctc2 = (ptctc->SIG1 == scanlosig ? ptctc->SIG2 : ptctc->SIG1)->PRCN->PCTC;
             scanctc2; scanctc2 = scanctc2->NEXT) {
            if (scanctc2->DATA == ptctc)
               break;
         }
         if (!scanctc2) {
            printf ("(%d) CTC (%16lX) entre %ld:%ld et %ld:%ld sur signal %ld non trouvee sur l'autre signal.\n",
                  niveau,
                  (long)ptctc,
                  ptctc->SIG1->INDEX, ptctc->NODE1, ptctc->SIG2->INDEX, ptctc->NODE2, scanlosig->INDEX);
            EXIT (-1);
         }
      }
   }
}

/*******************************************************************************
* function loadlofig                                                           *
*******************************************************************************/
void loadlofig (ptfig, name, mode)
lofig_list *ptfig;
char *name;
char mode;
{
   if (TRACE_MODE == 'Y')
      (void)fprintf (stdout, "--- mbk --- loadlofig : reading file %s.%s mode %c\n", name, IN_LO, mode);
   if ((!strcmp (IN_LO, "hns")) || (!strcmp (IN_LO, "fne"))
      || (!strcmp (IN_LO, "hdn")) || (!strcmp (IN_LO, "fdn")))
      vtiloadlofig (ptfig, name, mode);
   else if ((!strcmp (IN_LO, "al")) || (!strcmp (IN_LO, "alx")))
      alcloadlofig (ptfig, name, mode);
   else if (!strcmp (IN_LO, "spi") || !strcmp (IN_LO, "sp") || !strcmp (IN_LO, "cir"))
      spiceloadlofig (ptfig, name, mode);
   else if (!strcmp (IN_LO, "edi"))
      edifloadlofig (ptfig, name, mode);
   else if (!strcmp (IN_LO, "vst"))
      vhdlloadlofig (ptfig, name, mode);
   else if (!strcmp (IN_LO, "vhd"))
      vhdlloadlofig (ptfig, name, mode);
   else if (!strcmp (IN_LO, "vlg"))
      verilogloadlofig (ptfig, name, mode);
   else if (!strcmp (IN_LO, "v"))
      verilogloadlofig (ptfig, name, mode);
   else {
      (void)fflush (stdout);
      (void)fprintf (stderr, "*** mbk error ***\n");
      (void)fprintf (stderr, "MBK_IN_LO : '%s' unknown format\n", IN_LO);
      EXIT (1);
   }

   if (TRACE_MODE == 'Y')
      (void)fprintf (stdout, "--- mbk --- loadlofig : done reading file %s.%s\n", name, IN_LO);

   if (MBK_LOAD_PARA && mode == 'A') {
      if (TRACE_MODE == 'Y')
         (void)fprintf (stdout, "--- mbk --- loadlofig : reading file %s.%s\n", name, IN_PARASITICS);

      if ((!strcmp (IN_PARASITICS, "spf")) || (!strcmp (IN_PARASITICS, "dspf")))
         spf_Annotate (ptfig);
      else if (!strcmp (IN_PARASITICS, "spef"))
         spef_Annotate (ptfig);

      if (TRACE_MODE == 'Y')
         (void)fprintf (stdout, "--- mbk --- loadlofig : done reading file %s.%s\n", name, IN_PARASITICS);
   }
}

/*******************************************************************************
* function savelofig                                                           *
*******************************************************************************/

void savelofig (ptfig)
lofig_list *ptfig;
{
   if ((!strcmp (OUT_LO, "hns")) || (!strcmp (OUT_LO, "fne"))
      || (!strcmp (OUT_LO, "hdn")) || (!strcmp (OUT_LO, "fdn")))
      vtisavelofig (ptfig);
   else if ((!strcmp (OUT_LO, "al")) || (!strcmp (OUT_LO, "alx")))
      alcsavelofig (ptfig);
   else if (!strcmp (OUT_LO, "spi") || !strcmp (OUT_LO, "sp") || !strcmp (OUT_LO, "cir"))
      spicesavelofig (ptfig);
   else if (!strcmp (OUT_LO, "edi"))
      edifsavelofig (ptfig);
   else if (!strcmp (OUT_LO, "vst"))
      vhdlsavelofig (ptfig,NULL);
   else if (!strcmp (OUT_LO, "vhd"))
      vhdlsavelofig (ptfig,NULL);
   else if (!strcmp (OUT_LO, "cct"))
      hilosavelofig (ptfig);
   else if (!strcmp (OUT_LO, "vlg"))
      vlogsavelofig (ptfig,NULL);
   else if (!strcmp (OUT_LO, "v"))
      vlogsavelofig (ptfig,NULL);
   else {
      (void)fflush (stdout);
      (void)fprintf (stderr, "*** mbk error ***\n");
      (void)fprintf (stderr, "MBK_OUT_LO : %s unknown format\n", OUT_LO);
      EXIT (1);
   }
}

/*******************************************************************************
* function rflattenlofig()                                                     *
* flatten recursif sur une figure logique ptfig.                               *
* concat permet de generer les cheminons (concat != 'N')                       *
* la mise a plat s'effectue jusqu'au cellules du catalogue                     *
* non comprises.                                                               *
* catal = `y` ou catal=`Y` indique qu`il faut tenir compte du                  *
* catalogue. Une autre valeur implique un flatten complet.                     *
*******************************************************************************/
static int IsInCatal (char *name, char catal)
{
   if (catal && incatalog (name) && !incatalogdelete (name))
      return 1;
   return 0;
}

static void hierrflattenlofig (lofig_list * ptlofig, AdvancedNameAllocator * ana, char concat, char catal)
{
   loins_list *ptloins;
   chain_list *ptchain = NULL;
   chain_list *ptchainsav = NULL;
   chain_list *ptchainx = NULL;

   for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
      if (IsInCatal (ptloins->FIGNAME, catal)) {
         ptchainsav = addchain (ptchainsav, ptloins);
      }
      else {
         ptchain = addchain (ptchain, ptloins);
      }
   }
   ptlofig->LOINS = NULL;

   for (ptchainx = ptchainsav; ptchainx != NULL; ptchainx = ptchainx->NEXT) {
      ptloins = (loins_list *) ptchainx->DATA;
      ptloins->NEXT = ptlofig->LOINS;
      ptlofig->LOINS = ptloins;
   }
   freechain (ptchainsav);

   if (ptchain == NULL)
      return;
   for (ptchainx = ptchain; ptchainx != NULL; ptchainx = ptchainx->NEXT) {
      ptloins = (loins_list *) ptchainx->DATA;
      ptloins->NEXT = ptlofig->LOINS;
      ptlofig->LOINS = ptloins;
   }
   ptchain = reverse (ptchain);

   flattenlofig_bypointer (ptlofig, ptchain, ana, concat);

   freechain (ptchain);

   hierrflattenlofig (ptlofig, ana, concat, catal);
}

void rflattenlofig (ptfig, concat, catal)
lofig_list *ptfig;
char concat, catal;
{
   AdvancedNameAllocator *ana;

   catal = catal == NO ? 0 : 1;

   ana = CreateAdvancedNameAllocator (CASE_SENSITIVE);
   hierrflattenlofig (ptfig, ana, concat, catal);
   flatten_setup_realname_from_hiername (ptfig, ana);
   FreeAdvancedNameAllocator (ana);
}

/*******************************************************************************
* function unflatOutsideList()                                                 *
* Unflat the instances for which the name of the model is                      *
* not in the list of models for which only a black box exists.                 *
* This list is given in the file called MBK_BLACKBOX_NAME.                     *
*******************************************************************************/
lofig_list *unflatOutsideList (ptlofig, figname, insname)
lofig_list *ptlofig;
char *figname;
char *insname;
{
   lofig_list *ptnewfig;
   loins_list *ptloins;
   chain_list *inslist = NULL;
   char *name;
   int blacklisted = 0;

#ifdef AVERTEC_LICENSE
   if(avt_givetoken("HITAS_LICENSE_SERVER", "bbox")!=AVT_VALID_TOKEN) EXIT(1);
#endif

   for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
      name = ptloins->FIGNAME;
      if (IsInBlackList(name)) blacklisted = 1;
      else inslist = addchain (inslist, ptloins);
   }

   if (blacklisted) {
      ptnewfig = unflattenlofig (ptlofig, figname, insname, inslist);
      freechain (inslist);
      return ptnewfig;
   }
   else {
      freechain (inslist);
      return ptlofig;
   }
}

/*******************************************************************************
* function IsInBlackList()                                                     *
*******************************************************************************/

int IsInBlackList (char *figname)
{
  chain_list *cl;
  const char *pref="unused:";
  
  if (!bl_loaded)
    {
      loadBlackList();
      bl_loaded = 1;
    }

  if (BLACKLIST == NULL)
      return 0;

  if (BLACKLIST_MATCH_RULE==NULL)
  {
    char *temp;
    BLACKLIST_MATCH_RULE=(mbk_match_rules *)mbkalloc(sizeof(mbk_match_rules));
    BLACKLIST_MATCH_RULE_UNUSED=(mbk_match_rules *)mbkalloc(sizeof(mbk_match_rules));
    mbk_CreateREGEX(BLACKLIST_MATCH_RULE, CASE_SENSITIVE, 1);
    mbk_CreateREGEX(BLACKLIST_MATCH_RULE_UNUSED, CASE_SENSITIVE, 1);
    for (cl=BLACKLIST; cl!=NULL; cl=cl->NEXT)
      {
        temp=(char *)cl->DATA;
        if (strncasecmp(temp, pref,strlen(pref))!=0)
          mbk_AddREGEX(BLACKLIST_MATCH_RULE, temp);
        else
          mbk_AddREGEX(BLACKLIST_MATCH_RULE_UNUSED, namealloc(&temp[strlen(pref)]));
      }
  }
   
  if (mbk_CheckREGEX(BLACKLIST_MATCH_RULE_UNUSED, figname)) return 2;
  return mbk_CheckREGEX(BLACKLIST_MATCH_RULE, figname);
}

/*******************************************************************************
* function flatOutsideList()                                                   *
* Flat the instances for which the name of the model is                        *
* not in the list of models for which only a black box exists.                 *
* This list is given in the file called MBK_BLACKBOX_NAME.                     *
*******************************************************************************/
static void hierFlatOutsideList (lofig_list * ptlofig, AdvancedNameAllocator * ana)
{
   loins_list *ptloins;
   chain_list *ptchain = NULL;
   chain_list *ptchainsav = NULL;
   chain_list *ptchainx = NULL;
   int ret;

   for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
      ret=0;
      if (getptype(ptloins->USER, BBOX_AS_UNUSED)==NULL && (ret=IsInBlackList (ptloins->FIGNAME))==1) {
         ptchainsav = addchain (ptchainsav, ptloins);
      }
      else {
         ptchain = addchain (ptchain, ptloins);
         if (ret==2)
         {
           ptype_list *p0;
           char buf[512], buf1[512];
           if ((p0 = getptype (ptloins->USER, LF_HIERNAME_TO_NAMEALLOC)) != NULL) 
           {
             AdvancedNameAllocName (ana, (int)(long)p0->DATA, buf);
             sprintf(buf1, "%s`%s",ptloins->FIGNAME,buf);
           }
           else
            sprintf(buf1, "%s`%s",ptloins->FIGNAME,ptloins->INSNAME);
           ptloins->USER=addptype(ptloins->USER, BBOX_AS_UNUSED, namealloc(buf1));
         }
      }
   }
   ptlofig->LOINS = NULL;

   for (ptchainx = ptchainsav; ptchainx != NULL; ptchainx = ptchainx->NEXT) {
      ptloins = (loins_list *) ptchainx->DATA;
      ptloins->NEXT = ptlofig->LOINS;
      ptlofig->LOINS = ptloins;
   }
   freechain (ptchainsav);

   if (ptchain == NULL)
   {
     ptype_list *ptype;
     eqt_ctx *mc_ctx = NULL;
     ptype = getptype (ptlofig->USER, PARAM_CONTEXT);
     flatten_parameters (ptlofig, NULL, ptype ? (eqt_param *) ptype->DATA : NULL , 0, 0, 0, 0, 0, 0, &mc_ctx);
     if (mc_ctx!=NULL) eqt_term(mc_ctx);
     return;
   }
   for (ptchainx = ptchain; ptchainx != NULL; ptchainx = ptchainx->NEXT) {
      ptloins = (loins_list *) ptchainx->DATA;
      ptloins->NEXT = ptlofig->LOINS;
      ptlofig->LOINS = ptloins;
   }
   ptchain = reverse (ptchain);

   flattenlofig_bypointer (ptlofig, ptchain, ana, 'Y');

   freechain (ptchain);

   hierFlatOutsideList (ptlofig, ana);
}

static int cntchain (void *cls)
{
   int c = 0;
   chain_list *cl = (chain_list *) cls;
   while (cl != NULL) {
      c++;
      cl = cl->NEXT;
   }
   return c;

}
static int cntoptparam (ptype_list * p, long num)
{
   if ((p = getptype (p, num)) != NULL)
      return cntchain (p->DATA);
   return 0;
}

void sizeoflofig (lofig_list * lf)
{
   long nbcon = 0;
   long nbtr = 0;
   long nbins = 0;
   long nbsig = 0;
   long nbnum = 0;
   long _nbchain = 0;
   long _nbptype = 0;
   long nbprcn = 0;
   long nbopt = 0;
   long a, b, c, d;
   long nbvss = 0, nbvdd = 0;

   locon_list *lc;
   lotrs_list *lt;
   loins_list *li;
   losig_list *ls;
   ptype_list *p;


   for (lc = lf->LOCON; lc != NULL; lc = lc->NEXT) {
      nbcon++;
      _nbptype += cntchain (lc->USER);
      nbnum += cntchain (lc->PNODE);
      _nbchain += cntoptparam (lc->USER, PNODENAME);
   }
   for (lt = lf->LOTRS; lt != NULL; lt = lt->NEXT) {
      nbtr++;
      _nbptype += cntchain (lt->USER);
      nbcon += 3;
      nbnum += cntchain (lt->GRID->PNODE);
      nbnum += cntchain (lt->SOURCE->PNODE);
      nbnum += cntchain (lt->DRAIN->PNODE);
      _nbptype += cntchain (lt->GRID->USER);
      _nbptype += cntchain (lt->SOURCE->USER);
      _nbptype += cntchain (lt->DRAIN->USER);
      _nbchain += cntoptparam (lt->GRID->USER, PNODENAME);
      _nbchain += cntoptparam (lt->SOURCE->USER, PNODENAME);
      _nbchain += cntoptparam (lt->DRAIN->USER, PNODENAME);
      if (lt->BULK) {
         nbcon++;
         nbnum += cntchain (lt->BULK->PNODE);
         _nbptype += cntchain (lt->BULK->USER);
         _nbchain += cntoptparam (lt->BULK->USER, PNODENAME);
      }
      nbopt += cntoptparam (lt->USER, OPT_PARAMS);
   }
   for (li = lf->LOINS; li != NULL; li = li->NEXT) {
      nbins++;
      _nbptype += cntchain (li->USER);
      for (lc = li->LOCON; lc != NULL; lc = lc->NEXT) {
         nbcon++;
         _nbptype += cntchain (lc->USER);
         nbnum += cntchain (lc->PNODE);
         _nbchain += cntoptparam (lc->USER, PNODENAME);
      }
      nbopt += cntoptparam (li->USER, OPT_PARAMS);
   }
   for (ls = lf->LOSIG; ls != NULL; ls = ls->NEXT) {
      nbsig++;
      _nbptype += cntchain (ls->USER);
      if (ls->PRCN)
         nbprcn++;
      if ((p = getptype (ls->USER, LOFIGCHAIN)) != NULL)
         _nbchain += cntchain (p->DATA);
      _nbchain += cntchain (ls->NAMECHAIN);
      if (mbk_LosigIsVSS(ls))
         nbvss++;
      if (mbk_LosigIsVDD(ls))
         nbvdd++;
   }
   printf ("reports:\nnblocon=%ld (%ldk)\n", nbcon, (sizeof (locon_list) * nbcon) / 1024);
   printf ("nbtr=%ld (%ldk)\n", nbtr, (sizeof (lotrs_list) * nbtr) / 1024);
   printf ("nbins=%ld (%ldk)\n", nbins, (sizeof (loins_list) * nbins) / 1024);
   printf ("nbsig=%ld (%ldk)\n", nbsig, (sizeof (losig_list) * nbsig) / 1024);
   printf ("nbnum=%ld (%ldk)\n", nbnum, (sizeof (num_list) * nbnum) / 1024);
   printf ("nbchain=%ld (%ldk)\n", _nbchain, (sizeof (chain_list) * _nbchain) / 1024);
   printf ("nbptype=%ld (%ldk)\n", _nbptype, (sizeof (ptype_list) * _nbptype) / 1024);
   printf ("nblorc=%ld (%ldk)\n", nbprcn, (sizeof (lorcnet_list) * nbprcn) / 1024);
   printf ("nbopt=%ld (%ldk)\n", nbopt, (sizeof (optparam_list) * nbopt) / 1024);
   printf ("total=%ldk\n", a = ((sizeof (locon_list) * nbcon) +
                                (sizeof (lotrs_list) * nbtr) +
                                (sizeof (loins_list) * nbins) +
                                (sizeof (losig_list) * nbsig) +
                                (sizeof (num_list) * nbnum) +
                                (sizeof (chain_list) * _nbchain) +
                                (sizeof (ptype_list) * _nbptype) +
                                (sizeof (lorcnet_list) * nbprcn) +
                                (sizeof (optparam_list) * nbopt)
                                ) / 1024);
#ifdef BASE_STAT
   printf (" freechain(%ldk) + freeptype(%ldk) + num(%ldk)\n", b = (i_nbchain * sizeof (chain_list)) / 1024, c = (i_nbptype * sizeof (ptype_list)) / 1024, d = (i_nbnum * sizeof (num_list)) / 1024);
#else
   b=c=d=0;
#endif
   printf ("FINAL: %ldk\n", a + b + c + d);
   printf (" %ld VSS, %ld VDD signals\n", nbvss, nbvdd);
}

/*******************************************************************************
* function loadBlackList()                                                     *
*******************************************************************************/

void
loadBlackList()
{
    FILE           *fpBlackBox;
    char            buffer[1024];

    fpBlackBox = mbkfopen (MBK_BBOX_NAME, NULL, "r");
    if (fpBlackBox == NULL) {
        return;
    }

    BLACKLIST = NULL; //addht (10);
    while (!feof (fpBlackBox)) {
        if (fscanf (fpBlackBox, "%s\n", buffer) != 0) {
            BLACKLIST=addchain(BLACKLIST, namealloc (buffer));
        }
    }
    fclose (fpBlackBox);
}

void SetBlackList(chain_list *list)
{
  deleteBlackList();

  BLACKLIST = NULL;
  while (list!=NULL) {
     BLACKLIST=addchain(BLACKLIST, namealloc((char *)list->DATA));
     list=list->NEXT;
  }
  bl_loaded=1;
}

void
deleteBlackList()
{
    if (BLACKLIST_MATCH_RULE!=NULL)
    {
      mbk_FreeREGEX(BLACKLIST_MATCH_RULE);
      mbkfree(BLACKLIST_MATCH_RULE);
      BLACKLIST_MATCH_RULE=NULL;
    }

    freechain(BLACKLIST);
    BLACKLIST=NULL;
}

/*******************************************************************************
* function flatOutsideList()                                                   *
*******************************************************************************/

lofig_list *flatOutsideList (ptlofig)
lofig_list *ptlofig;
{
   AdvancedNameAllocator *ana;

#ifdef AVERTEC_LICENSE
   if(avt_givetoken("HITAS_LICENSE_SERVER", "bbox")!=AVT_VALID_TOKEN) EXIT(1);
#endif

   ana = CreateAdvancedNameAllocator (CASE_SENSITIVE);
   hierFlatOutsideList (ptlofig, ana);

   flatten_setup_realname_from_hiername (ptlofig, ana);

   FreeAdvancedNameAllocator (ana);

   return ptlofig;
}
/*
static locon_list *getextlocon (ptsig)
losig_list *ptsig;
{
   chain_list *loconchain;
   locon_list *ptlocon;
   chain_list *ptchain;

   if (ptsig->TYPE != 'E')
      return NULL;
   loconchain = (chain_list *) getptype (ptsig->USER, LOFIGCHAIN)->DATA;
   for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
      ptlocon = (locon_list *) ptchain->DATA;
      if (ptlocon->TYPE == EXTERNAL)
         return ptlocon;
   }
   return NULL;
}
*/
/*
UNFLATLOFIG

II. Algorithme
--------------

1. On fait un LOFIGCHAIN sur 'ptfig'.

2. On veut construire deux 'chain_list' :
   LI : signaux internes a la nouvelle figure
   LX : signaux externes a la nouvelle figure

   Pour la figure 'ptfig', on parcourt la liste des signaux :
      Pour chaque signal, on parcourt la liste des connecteurs qui lui
      sont associes (par LOFIGCHAIN):
        * Si AUCUN connecteur n'appartient a une instance presente dans
          la liste 'list', on passe au signal suivant (pas d'insertion)

        * Sinon, si TOUS les connecteurs appartiennent a une instance
          presente dans la liste 'list', le signal est insere dans la
          liste LI
        * Sinon, le signal est insere dans la liste LX
          (au moins un connecteur n'appartient pas a la nouvelle figure)

3. On construit la nouvelle figure. Il faut creer :
   - une liste des signaux
   - une liste des connecteurs externes
   - une liste des instances, chacune contenant une liste de connecteurs

 a. liste des signaux
    on parcourt la liste LI et la liste LX : on ajoute chaque signal a
    la liste des signaux de la nouvelle figure.
    On construit une structure ptype (de nom UNFLATLOFIG), donnant a
    chaque signal de LI ou LX un pointeur vers le signal cree dans la
    nouvelle figure.

 b. liste des connecteurs externes
    on parcourt la liste LX :
    pour chaque signal, on cherche le premier connecteur interne (a la
    nouvelle figure) et le premier connecteur externe (a la nouvelle
    figure), de preference un terminal de la figure pere: s'il en
    existe un (terminal) on prend celui-la, sinon on prend le
    premier externe a la nouvelle figure, mais non terminal dans
    la figure pere.
    On ajoute un connecteur externe a la nouvelle figure. Il pointe
    sur le signal courant (dans la nouvelle). Sa direction est:
    - si le connecteur n'appartenant pas a la nouvelle figure est
      un terminal du pere (externe), le nouveau connecteur prend la
      direction de ce terminal.
    - si le connecteur n'appartenant pas a la nouvelle figure est un
      connecteur d'instance, le nouveau connecteur prend la direction
      du premier connecteur INTERNE a la nouvelle figure trouve 
      precedemment. 
    Le nom du nouveau connecteur est:
    - le nom du signal auquel il est connecte si ce dernier en a un.
    - sinon :
      . si le premier connecteur n'appartenant pas a la nouvelle
        figure est un terminal, le nouveau prend ce nom,
      . si le premier connecteur n'appartenant pas a la nouvelle
        figure est un connecteur d'instance, le nouveau prend le
        nom du connecteur INTERNE a la nouvelle figure (comme pour
        la direction).
    Dans tous les cas, on concatene au nom un compteur (comptant le
    nombre de connecteurs externes crees) pour assurer l'unicite.

 c. liste des instances
    on parcourt la liste 'list' :
    on insere chaque instance dans la liste d'instances de la nouvelle
    figure. Pour chaque connecteur d'une instance de 'list', on
    ajoute un connecteur dans la liste des connecteurs de l'instance
    dans la nouvelle figure, dont le signal dans la nouvelle figure est
    obtenu en consultant la structure ptype du signal dans la figure pere.
    Les connecteurs prennent les directions des connecteurs dans les
    instances de la figure pere.

4. On modifie la figure pere.
    On instancie la nouvelle figure.
    On detruit les signaux internes (LI).
    On detruit les instances de la liste 'list'.
*/
/*******************************************************************************
* function unflattenlofig                                                      *
*******************************************************************************/
lofig_list *unflattenlofig (ptfig, figname, insname, list)
lofig_list *ptfig;
char *figname;
char *insname;
chain_list *list;
{
   lofig_list *newlofig = NULL;
   chain_list *li_head = NULL, *lx_head = NULL, *ptchain, *ptchain1;
   chain_list *loconchain, *ptnextchain, *ptprevchain;
   chain_list *outconchain = NULL;
   chain_list *sigchain = NULL;
   chain_list *savechain = NULL;
   chain_list *ctctodelete = NULL;
   losig_list *ptlosig, *newlosig;
   losig_list *vss_sig = NULL;
   losig_list *father_vss = NULL;
   losig_list *ptprevsig, *ptnextsig;
   losig_list *ptctcsig;
   losig_list *ptsource, *ptdrain, *ptgrid, *ptbulk;
   loctc_list *ptctc;
   num_list *ptnum;
   num_list *loconnodes = NULL, *toplevelnodes = NULL;
   loins_list *ptloins, *newloins;
   locon_list *ptlocon, *newlocon;
   locon_list *intlocon, *extlocon, *translocon;
   lotrs_list *ptlotrs, *newlotrs;
   ptype_list *ptuser;
   long *nodetab = NULL;
   long countnode = 0;
   long vss_node = 0, father_vss_node = 0;
   char loconname[BUFSIZE];
   char *name, *delete_flag;
   char locondir;
   int foundterm;
   int onvsssig, coupledvsssig;
   int i;

   if (ptfig == NULL)
      return NULL;
   lofigchain (ptfig);

   /* mark instances */
   for (ptchain = list; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptloins = (loins_list *) ptchain->DATA;
      ptloins->USER = addptype (ptloins->USER, UNFLATLOFIG, NULL);
   }

   /* scan signals:
      construct LI and LX. */
   for (ptlosig = ptfig->LOSIG; ptlosig != NULL; ptlosig = ptlosig->NEXT) {
      int found_int = 0, found_ext = 0;

      /* scan connectors of signal */
      loconchain = (chain_list *) getptype (ptlosig->USER, LOFIGCHAIN)->DATA;
      for (; loconchain; loconchain = loconchain->NEXT) {
         ptlocon = (locon_list *) (loconchain->DATA);
         if (ptlocon->TYPE == 'T')
            found_int++;
         else if (ptlocon->TYPE!='I' || (ptlocon->TYPE=='I' && getptype (((loins_list *) ptlocon->ROOT)->USER, UNFLATLOFIG) == NULL))
            found_ext++;
         else
            found_int++;
      }
      if (found_ext != 0) {   /* potentially external vss for top figure */
         if (mbk_LosigIsVSS(ptlosig)) father_vss = ptlosig;
      }
      if (found_int == 0) {
         if (mbk_LosigIsVSS(ptlosig)) father_vss = ptlosig; /* potentially internal vss for top figure */
         continue;         /* no insertion */
      }
      if (found_ext == 0) {
         li_head = addchain (li_head, (void *)(ptlosig));   /* insert LI */
      }
      else {
         lx_head = addchain (lx_head, (void *)(ptlosig));   /* insert LX */
      }
   }                     /* endfor ptlosig */

   /***** add new figure : */
   newlofig = addlofig (figname);

   /* create signals list */
   for (ptchain = li_head; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *)ptchain->DATA;
      newlosig = duplosig(ptlosig, newlofig);
      newlosig->NEXT = newlofig->LOSIG;
      newlofig->LOSIG = newlosig;
      newlosig->TYPE = INTERNAL;
      if (mbk_LosigIsVSS (ptlosig)) vss_sig = newlosig;
      newlosig->PRCN = ptlosig->PRCN;
      ptlosig->USER = addptype (ptlosig->USER, UNFLATLOFIG, newlosig);
   }
   for (ptchain = lx_head; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *)ptchain->DATA;
      newlosig = duplosig(ptlosig, newlofig);
      newlosig->NEXT = newlofig->LOSIG;
      newlofig->LOSIG = newlosig;
      newlosig->TYPE = EXTERNAL;
      if (mbk_LosigIsVSS (ptlosig)) vss_sig = newlosig;
      newlosig->PRCN = ptlosig->PRCN;
      ptlosig->USER = addptype (ptlosig->USER, UNFLATLOFIG, newlosig);
   }

   /* obtain ground node for crosstalk capacitances */
   if (vss_sig == NULL) {
      vss_sig = addlosig(newlofig, newlofig->BKSIG.maxindex + 1, addchain(NULL, namealloc("0")), INTERNAL);
      mbk_SetLosigVSS(vss_sig);
   }
   if (father_vss == NULL) {
      father_vss = addlosig(ptfig, ptfig->BKSIG.maxindex + 1, addchain(NULL, namealloc("0")), INTERNAL);
      mbk_SetLosigVSS(father_vss);
   }
   /* set the node to 1 if nodes exist else 0 !!! Check consequences */
   if (vss_sig->PRCN == NULL)
      vss_node = 0;
   else if (vss_sig->PRCN->NBNODE == 0)
      vss_node = 0;
   else
      vss_node = 1;
   if (father_vss->PRCN == NULL)
      father_vss_node = 0;
   else if (father_vss->PRCN->NBNODE == 0)
      father_vss_node = 0;
   else
      father_vss_node = 1;

   /* update crosstalk capacitances */
   sigchain = NULL;
   for (ptlosig = newlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
      if (ptlosig->PRCN != NULL) {
         for (ptchain = ptlosig->PRCN->PCTC; ptchain; ptchain = ptnextchain) {
            ptnextchain = ptchain->NEXT;
            ptctc = (loctc_list *) ptchain->DATA;
            ptctcsig = ptctc->SIG1;
            onvsssig = 0;
            coupledvsssig = 0;
            if (getptype (ptctcsig->USER, RCN_REPORTED) == NULL) {
               if ((ptuser = getptype (ptctcsig->USER, UNFLATLOFIG)) == NULL) {
                  if (ptctcsig->PRCN != NULL) {   /* list of ctc signals to update */
                     for (ptchain1 = savechain; ptchain1; ptchain1 = ptchain1->NEXT) {
                        if (ptchain1->DATA == ptctcsig)
                           break;
                     }
                     if (ptchain1 == NULL)
                        savechain = addchain (savechain, ptctcsig);
                  }
                  if (mbk_LosigIsVSS(ptctcsig)) onvsssig = 1;
                  if (!onvsssig) {   /* add new ctc to top figure */
                     addloctc (ptctcsig, ptctc->NODE1, father_vss, father_vss_node, ptctc->CAPA);
                  }
                  if (mbk_LosigIsVSS(ptctc->SIG2)) coupledvsssig = 1;
                  if (!coupledvsssig) {   /* problem if SIG2 == vss_sig */
                     ptctc->SIG1 = vss_sig;
                     ptctc->NODE1 = vss_node;
                     if (vss_sig->PRCN != NULL) {
                        vss_sig->PRCN->PCTC = addchain (vss_sig->PRCN->PCTC, ptctc);
                     }
                  }
                  else {   /* ctc must be completely removed */
                     if (ptctc->SIG1->PRCN != NULL) {   /* list of ctc signals to update */
                        for (ptchain1 = savechain; ptchain1; ptchain1 = ptchain1->NEXT) {
                           if (ptchain1->DATA == ptctc->SIG1)
                              break;
                        }
                        if (ptchain1 == NULL)
                           savechain = addchain (savechain, ptctc->SIG1);
                     }
                     if (ptctc->SIG2->PRCN != NULL) {   /* list of ctc signals to update */
                        for (ptchain1 = savechain; ptchain1; ptchain1 = ptchain1->NEXT) {
                           if (ptchain1->DATA == ptctc->SIG2)
                              break;
                        }
                        if (ptchain1 == NULL)
                           savechain = addchain (savechain, ptctc->SIG2);
                     }
                     ptctc->SIG1 = NULL;
                     ptctc->SIG2 = NULL;
                     ctctodelete = addchain (ctctodelete, ptctc);
                  }
               }
               else {
                  ptctc->SIG1 = (losig_list *) ptuser->DATA;
               }
               if (ptctc->SIG1 != NULL) {
                  ptctc->SIG1->USER = addptype (ptctc->SIG1->USER, RCN_REPORTED, NULL);
                  sigchain = addchain (sigchain, ptctc->SIG1);
               }
            }
            ptctcsig = ptctc->SIG2;
            if (ptctcsig == NULL)
               continue;
            onvsssig = 0;
            coupledvsssig = 0;
            if (getptype (ptctcsig->USER, RCN_REPORTED) == NULL) {
               if ((ptuser = getptype (ptctcsig->USER, UNFLATLOFIG)) == NULL) {
                  if (ptctcsig->PRCN != NULL) {   /* list of ctc signals in top figure */
                     for (ptchain1 = savechain; ptchain1; ptchain1 = ptchain1->NEXT) {
                        if (ptchain1->DATA == ptctcsig)
                           break;
                     }
                     if (ptchain1 == NULL)
                        savechain = addchain (savechain, ptctcsig);
                  }
                  if (mbk_LosigIsVSS(ptctcsig)) onvsssig = 1;
                  if (!onvsssig) {   /* add new ctc to top figure */
                     addloctc (ptctcsig, ptctc->NODE1, father_vss, father_vss_node, ptctc->CAPA);
                  }
                  if (mbk_LosigIsVSS(ptctc->SIG2)) coupledvsssig = 1;
                  if (!coupledvsssig) {   /* problem if SIG1 == vss_sig */
                     ptctc->SIG2 = vss_sig;
                     ptctc->NODE2 = vss_node;
                     if (vss_sig->PRCN != NULL) {
                        vss_sig->PRCN->PCTC = addchain (vss_sig->PRCN->PCTC, ptctc);
                     }
                  }
                  else {   /* ctc must be completely removed */
                     if (ptctc->SIG1->PRCN != NULL) {   /* list of ctc signals to update */
                        for (ptchain1 = savechain; ptchain1; ptchain1 = ptchain1->NEXT) {
                           if (ptchain1->DATA == ptctc->SIG1)
                              break;
                        }
                        if (ptchain1 == NULL)
                           savechain = addchain (savechain, ptctc->SIG1);
                     }
                     if (ptctc->SIG2->PRCN != NULL) {   /* list of ctc signals to update */
                        for (ptchain1 = savechain; ptchain1; ptchain1 = ptchain1->NEXT) {
                           if (ptchain1->DATA == ptctc->SIG2)
                              break;
                        }
                        if (ptchain1 == NULL)
                           savechain = addchain (savechain, ptctc->SIG2);
                     }
                     ptctc->SIG1 = NULL;
                     ptctc->SIG2 = NULL;
                     ctctodelete = addchain (ctctodelete, ptctc);
                  }
               }
               else {
                  ptctc->SIG2 = (losig_list *) ptuser->DATA;
               }
               if (ptctc->SIG2 != NULL) {
                  ptctc->SIG2->USER = addptype (ptctc->SIG2->USER, RCN_REPORTED, NULL);
                  sigchain = addchain (sigchain, ptctc->SIG2);
               }
            }
         }
      }
   }
   for (ptchain = sigchain; ptchain; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *) ptchain->DATA;
      ptlosig->USER = delptype (ptlosig->USER, RCN_REPORTED);
   }
   freechain (sigchain);
   sigchain = NULL;

   /* delete ctc references in signals */
   for (ptchain = savechain; ptchain; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *) ptchain->DATA;
      if (ptlosig->PRCN != NULL) {
         ptprevchain = NULL;
         for (ptchain1 = ptlosig->PRCN->PCTC; ptchain1; ptchain1 = ptnextchain) {
            ptnextchain = ptchain1->NEXT;
            ptctc = (loctc_list *) ptchain1->DATA;
            if (ptctc->SIG1 != ptlosig && ptctc->SIG2 != ptlosig) {
               if (ptprevchain != NULL)
                  ptprevchain->NEXT = ptnextchain;
               else
                  ptlosig->PRCN->PCTC = ptnextchain;
               ptchain1->NEXT = NULL;
               freechain (ptchain1);
            }
            else
               ptprevchain = ptchain1;
         }
      }
   }
   freechain (savechain);

   /* delete unnecessary ctcs */
   for (ptchain = ctctodelete; ptchain; ptchain = ptchain->NEXT) {
      freeloctc ((loctc_list *) ptchain->DATA);
   }
   freechain (ctctodelete);

   /* create terminals list */
   for (ptchain = lx_head; ptchain != NULL; ptchain = ptchain->NEXT) {
      intlocon = NULL;
      extlocon = NULL;
      translocon = NULL;
      foundterm = 0;

      /* scan connectors list of the current signal */
      ptlosig = (losig_list *) (ptchain->DATA);
      if (ptlosig->PRCN != NULL) {
         loconnodes = NULL;
         toplevelnodes = NULL;
         countnode = 0;
         outconchain = NULL;
         nodetab = mbkalloc ((ptlosig->PRCN->NBNODE + 1) * sizeof (long));
         memset (nodetab, 0, (ptlosig->PRCN->NBNODE + 1) * sizeof (long));
         nodetab[0] = ptlosig->PRCN->NBNODE;
      }

      loconchain = getptype (ptlosig->USER, LOFIGCHAIN)->DATA;
      for (; loconchain; loconchain = loconchain->NEXT) {
         ptlocon = (locon_list *) (loconchain->DATA);
         if (ptlocon->TYPE == 'T') {
            if (translocon == NULL) translocon = ptlocon;
         }
         else if (ptlocon->TYPE!='I' || (ptlocon->TYPE=='I' && getptype (((loins_list *) ptlocon->ROOT)->USER, UNFLATLOFIG) == NULL)) {   /* external connector of new figure */
            if (ptlocon->TYPE == EXTERNAL) {   /* external con. in root figure too */
               foundterm++;
               extlocon = ptlocon;
            }
            else if (extlocon == NULL)
               extlocon = ptlocon;
            if (ptlosig->PRCN != NULL) {
               for (ptnum = ptlocon->PNODE; ptnum; ptnum = ptnum->NEXT) {
                  if (nodetab[ptnum->DATA] == 0) {
                     countnode++;
                     nodetab[ptnum->DATA] = countnode;
                  }
               }
               outconchain = addchain (outconchain, ptlocon);
            }
         }
         else if (intlocon == NULL)
            intlocon = ptlocon;   /* internal con. in new */
      }                  /* endfor loconchain */

      /* build connector RC node lists */
      if (ptlosig->PRCN != NULL) {
         ptlosig->PRCN = NULL;
         addlorcnet (ptlosig);
         ptlosig->PRCN->NBNODE = countnode;

         for (ptchain1 = outconchain; ptchain1; ptchain1 = ptchain1->NEXT) {
            ptlocon = (locon_list *) ptchain1->DATA;
            for (ptnum = ptlocon->PNODE; ptnum; ptnum = ptnum->NEXT) {
               ptnum->DATA = nodetab[ptnum->DATA];
            }
         }
         for (i = 1; i <= nodetab[0]; i++) {
            if (nodetab[i] != 0) {
               loconnodes = addnum (loconnodes, i);
               toplevelnodes = addnum (toplevelnodes, nodetab[i]);
            }
         }
         ptlosig->USER = addptype (ptlosig->USER, UNFLATRC, reverse ((chain_list *) toplevelnodes));
         mbkfree (nodetab);
         freechain (outconchain);
      }

      /* create new external connector for the new figure */
      locondir = foundterm ? extlocon->DIRECTION : UNKNOWN;
      name = "unknown";
      if (foundterm)
         name = extlocon->NAME;
      else if (ptlosig->NAMECHAIN != NULL) {
         name = getsigname (ptlosig);
      }
      else if (intlocon) {
         name = intlocon->NAME;
      }
      strcpy (loconname, name);

      newlosig = (losig_list *) getptype (ptlosig->USER, UNFLATLOFIG)->DATA;
      newlofig->LOCON = addlocon (newlofig, loconname, newlosig, locondir);
      if (ptlosig->PRCN != NULL) {
         loconnodes = (num_list *) reverse ((chain_list *) loconnodes);
         newlofig->LOCON->PNODE = loconnodes;
      }
   }

   /* create instances list */
   for (ptchain = list; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptloins = (loins_list *)ptchain->DATA;
      newloins = rduploins(ptloins);
      newloins->NEXT = newlofig->LOINS;
      newlofig->LOINS = newloins;
      for (newlocon = newloins->LOCON, ptlocon = ptloins->LOCON; newlocon != NULL && ptlocon != NULL; newlocon = newlocon->NEXT, ptlocon = ptlocon->NEXT) {
         newlocon->SIG = (losig_list *)getptype(ptlocon->SIG->USER, UNFLATLOFIG)->DATA;
      }
   }
   
   /* create transistor list */
   for (ptlotrs = ptfig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
      ptsource = (losig_list *)getptype(ptlotrs->SOURCE->SIG->USER, UNFLATLOFIG)->DATA;
      ptdrain = (losig_list *)getptype(ptlotrs->DRAIN->SIG->USER, UNFLATLOFIG)->DATA;
      ptgrid = (losig_list *)getptype(ptlotrs->GRID->SIG->USER, UNFLATLOFIG)->DATA;
      if (ptlotrs->BULK) ptbulk = (losig_list *)getptype(ptlotrs->BULK->SIG->USER, UNFLATLOFIG)->DATA;
      newlotrs = rduplotrs(ptlotrs);
      newlotrs->NEXT = newlofig->LOTRS;
      newlofig->LOTRS = newlotrs;
      newlotrs->SOURCE->SIG = ptsource;
      newlotrs->DRAIN->SIG = ptdrain;
      newlotrs->GRID->SIG = ptgrid;
      if (ptlotrs->BULK) newlotrs->BULK->SIG = ptbulk;
   }
   newlofig->LOTRS = (lotrs_list *)reverse((chain_list *)newlofig->LOTRS);

   /***** free ptype lists (unflatlofig) */
   /* free unflatlofig in lx signals */
   for (ptchain = lx_head; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *)ptchain->DATA;
      ptlosig->USER = delptype(ptlosig->USER, UNFLATLOFIG);
   }

   /* free unflatlofig in li signals */
   for (ptchain = li_head; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *)ptchain->DATA;
      ptlosig->USER = delptype(ptlosig->USER, UNFLATLOFIG);
   }

   /* free unflatlofig in list instances */
   for (ptchain = list; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptloins = (loins_list *)ptchain->DATA;
      ptloins->USER = delptype(ptloins->USER, UNFLATLOFIG);
   }

   /***** modify the root figure :*/
   /* add new instantiated figure */
   lx_head = reverse(lx_head);
   ptfig->LOINS = addloins(ptfig, insname, getlofig (figname, 'P'), lx_head);
   for (ptlocon = ptfig->LOINS->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
      if ((ptuser = getptype (ptlocon->SIG->USER, UNFLATRC)) != NULL) {
         ptlocon->PNODE = (num_list *)ptuser->DATA;
      }
   }

   /* delete signals */
   for (ptchain = li_head; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptlosig = (losig_list *)ptchain->DATA;
      freechain (ptlosig->NAMECHAIN);
      ptlosig->NAMECHAIN = NULL;
   }
   ptprevsig = NULL;
   for (ptlosig = ptfig->LOSIG; ptlosig; ptlosig = ptnextsig) {
      ptnextsig = ptlosig->NEXT;
      if (ptlosig->NAMECHAIN == NULL) {
         if (ptprevsig != NULL)
            ptprevsig->NEXT = ptnextsig;
         else
            ptfig->LOSIG = ptnextsig;
         ptlosig->INDEX = 0L;
         dellosiguser (ptlosig);
      }
      else
         ptprevsig = ptlosig;
   }
   freechain (li_head);
   freechain (lx_head);

   delete_flag = namealloc("mbk_delete_flag");

   /* delete instances */
   for (ptchain = list; ptchain != NULL; ptchain = ptchain->NEXT) {
      ptloins = (loins_list *) ptchain->DATA;
      ptloins->INSNAME = NULL;
   }
   delflaggedloins(ptfig);

   /* update modelchain */
   freechain (ptfig->MODELCHAIN);
   ptfig->MODELCHAIN = NULL;
   for (ptloins = ptfig->LOINS; ptloins; ptloins = ptloins->NEXT) {
      name = ptloins->FIGNAME;
      for (ptchain = ptfig->MODELCHAIN; ptchain; ptchain = ptchain->NEXT) {
         if (ptchain->DATA == name)
            break;
      }
      if (ptchain == NULL) {
         ptfig->MODELCHAIN = addchain (ptfig->MODELCHAIN, name);
      }
   }
   
   /* delete transistors */
   for (ptlotrs = ptfig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) ptlotrs->TRNAME = delete_flag;
   delflaggedlotrs(ptfig, delete_flag);

   /* free lofigchain */
   for (ptlosig = ptfig->LOSIG; ptlosig != NULL; ptlosig = ptlosig->NEXT) {
      ptuser = getptype (ptlosig->USER, (long)LOFIGCHAIN);
      if (ptuser) {
         freechain ((chain_list *) ptuser->DATA);
         ptlosig->USER = delptype (ptlosig->USER, LOFIGCHAIN);
      }
   }

   return newlofig;
}

#define OK      1
#define PROBLEM 0
#define NOVBE  -1



struct typoin {               /* structure used by dast_dbg *//* to store its state       */
   short type;               /* code of the current structure */
   short mark;               /* stop mark            */
   void *data;               /* pointer of the structure */
};

#define VHD_ERRDFN  -1
#define VHD__XTDFN  0
#define VHD__UPDFN  1
#define VHD__TPDFN  2
#define VHD__SPDFN  3
#define VHD__BKDFN  4

#define VHD_lofigDFN 5
#define VHD_nextDFN 6
#define VHD_modelchainDFN 7
#define VHD_loconDFN 8
#define VHD_losigDFN 9
#define VHD_bksigDFN 10
#define VHD_loinsDFN 11
#define VHD_lotrsDFN 13
#define VHD_nameDFN 14
#define VHD_modeDFN 15
#define VHD_userDFN 16
#define VHD_drainDFN 17
#define VHD_gridDFN 18
#define VHD_sourceDFN 19
#define VHD_bulkDFN 36
#define VHD_widthDFN 20
#define VHD_lengthDFN 21
#define VHD_xDFN 22
#define VHD_yDFN 23
#define VHD_typeDFN 24
#define VHD_insnameDFN 25
#define VHD_fignameDFN 26
#define VHD_sigDFN 27
#define VHD_rootDFN 28
#define VHD_directionDFN 29
#define VHD_valDFN 30
#define VHD_namechainDFN 31
#define VHD_capaDFN 32
#define VHD_indexDFN 33
#define VHD_ptypeDFN 34
#define VHD_dataDFN 35

#define VHD_MAXDFN 37
static int vhd_getcmd ();
static int vhd_hash ();

void mlodebug (head_pnt, stru_name)

void *head_pnt;
char *stru_name;

{

   int i;
   int state;
   int newsta0;
   int readcmd = 0;
   char readtab[3][20];
   int stk_pnt = 0;
   int type = VHD__XTDFN;
   struct chain *ptchain;

   struct typoin current_pnt;
   struct typoin stk[200];

   char empty[4];
   char avail[12];

   int key[VHD_MAXDFN];
   char *tab[VHD_MAXDFN];
   void *nxt[VHD_MAXDFN];
   short typ[VHD_MAXDFN];

   struct lofig *lofig_pnt;
   struct locon *locon_pnt;
   struct losig *losig_pnt;
   ptype_list *ptype_pnt;
   struct loins *loins_pnt;
   struct lotrs *lotrs_pnt;

   key[VHD_lofigDFN] = vhd_hash ("lofig");
   key[VHD_nextDFN] = vhd_hash ("next");
   key[VHD_modelchainDFN] = vhd_hash ("modelchain");
   key[VHD_loconDFN] = vhd_hash ("locon");
   key[VHD_losigDFN] = vhd_hash ("losig");
   key[VHD_bksigDFN] = vhd_hash ("bksig");
   key[VHD_loinsDFN] = vhd_hash ("loins");
   key[VHD_lotrsDFN] = vhd_hash ("lotrs");
   key[VHD_nameDFN] = vhd_hash ("name");
   key[VHD_modeDFN] = vhd_hash ("mode");
   key[VHD_userDFN] = vhd_hash ("user");
   key[VHD_drainDFN] = vhd_hash ("drain");
   key[VHD_gridDFN] = vhd_hash ("grid");
   key[VHD_sourceDFN] = vhd_hash ("source");
   key[VHD_bulkDFN] = vhd_hash ("bulk");
   key[VHD_widthDFN] = vhd_hash ("width");
   key[VHD_lengthDFN] = vhd_hash ("length");
   key[VHD_xDFN] = vhd_hash ("x");
   key[VHD_yDFN] = vhd_hash ("y");
   key[VHD_typeDFN] = vhd_hash ("type");
   key[VHD_insnameDFN] = vhd_hash ("insname");
   key[VHD_fignameDFN] = vhd_hash ("figname");
   key[VHD_sigDFN] = vhd_hash ("sig");
   key[VHD_rootDFN] = vhd_hash ("root");
   key[VHD_directionDFN] = vhd_hash ("direction");
   key[VHD_valDFN] = vhd_hash ("val");
   key[VHD_namechainDFN] = vhd_hash ("namechain");
   key[VHD_capaDFN] = vhd_hash ("capa");
   key[VHD_indexDFN] = vhd_hash ("index");
   key[VHD_ptypeDFN] = vhd_hash ("ptype");

   /* ###------------------------------------------------------### */
   /*    Set of predefined commands                */
   /* ###------------------------------------------------------### */

   key[0] = vhd_hash ("_exit");
   key[1] = vhd_hash ("_up");
   key[2] = vhd_hash ("_top");
   key[3] = vhd_hash ("_stop");
   key[4] = vhd_hash ("_back");

   (void)strcpy (empty, "");
   (void)strcpy (avail, "AVAILABLE");

   for (i = 0; i < VHD_MAXDFN; i++)
      typ[i] = i;

   /* ###------------------------------------------------------### */
   /*    Find the type of the head structure           */
   /* ###------------------------------------------------------### */

   readcmd = vhd_hash (stru_name);
   for (i = 0; i < VHD_MAXDFN; i++) {
      if (readcmd == key[i]) {
         type = typ[i];
         break;
      }
   }

   /* ###------------------------------------------------------### */
   /*    Exit if the head structure is empty           */
   /* ###------------------------------------------------------### */

   if (head_pnt == NULL)
      type = VHD__XTDFN;

   current_pnt.data = head_pnt;
   current_pnt.type = type;

   state = type;

   while (state != VHD__XTDFN) {
      /* ###------------------------------------------------------### */
      /*    Print structure's field until the exit command is read    */
      /* ###------------------------------------------------------### */

      for (i = 0; i < VHD_MAXDFN; i++) {
         tab[i] = empty;
         nxt[i] = NULL;
      }

      /* ###------------------------------------------------------### */
      /*    _exit and _stop commands are allways available        */
      /* ###------------------------------------------------------### */

      tab[VHD__XTDFN] = avail;
      tab[VHD__SPDFN] = avail;

      /* ###------------------------------------------------------### */
      /*    _up, _top, and _back commands are available only if the   */
      /* stack is not empty                       */
      /* ###------------------------------------------------------### */

      if (stk_pnt != 0) {
         tab[VHD__UPDFN] = avail;
         tab[VHD__TPDFN] = avail;
         tab[VHD__BKDFN] = avail;
      }

      switch (state) {

      case (VHD_lofigDFN):

         /* ###--------- lofig ---------### */

         lofig_pnt = (struct lofig *)(current_pnt.data);

         if (lofig_pnt->NEXT != NULL) {
            tab[VHD_nextDFN] = avail;
            nxt[VHD_nextDFN] = (void *)lofig_pnt->NEXT;
            typ[VHD_nextDFN] = VHD_lofigDFN;
         }
         if (lofig_pnt->LOCON != NULL) {
            tab[VHD_loconDFN] = avail;
            nxt[VHD_loconDFN] = (void *)lofig_pnt->LOCON;
            typ[VHD_loconDFN] = VHD_loconDFN;
         }
         if (lofig_pnt->LOSIG != NULL) {
            tab[VHD_losigDFN] = avail;
            nxt[VHD_losigDFN] = (void *)lofig_pnt->LOSIG;
            typ[VHD_losigDFN] = VHD_losigDFN;
         }
         if (lofig_pnt->BKSIG.TAB != NULL) {
            tab[VHD_bksigDFN] = avail;
            nxt[VHD_bksigDFN] = (void *)lofig_pnt->BKSIG.TAB;
            typ[VHD_bksigDFN] = VHD_ptypeDFN;
         }
         if (lofig_pnt->LOINS != NULL) {
            tab[VHD_loinsDFN] = avail;
            nxt[VHD_loinsDFN] = (void *)lofig_pnt->LOINS;
            typ[VHD_loinsDFN] = VHD_loinsDFN;
         }
         if (lofig_pnt->LOTRS != NULL) {
            tab[VHD_lotrsDFN] = avail;
            nxt[VHD_lotrsDFN] = (void *)lofig_pnt->LOTRS;
            typ[VHD_lotrsDFN] = VHD_lotrsDFN;
         }
         if (lofig_pnt->USER != NULL) {
            tab[VHD_userDFN] = avail;
            nxt[VHD_userDFN] = (void *)lofig_pnt->USER;
            typ[VHD_userDFN] = VHD_ptypeDFN;
         }

         ptchain = lofig_pnt->MODELCHAIN;
         (void)printf ("   modelchain  :\n");
         while (ptchain != NULL) {
            (void)printf ("                 %s\n", (char *)ptchain->DATA);
            ptchain = ptchain->NEXT;
         }

         (void)printf ("   name        : %s\n", lofig_pnt->NAME);
         (void)printf ("   mode        : %c\n", lofig_pnt->MODE);
         (void)printf ("-> locon       : %s\n", tab[VHD_loconDFN]);
         (void)printf ("-> losig       : %s\n", tab[VHD_losigDFN]);
         (void)printf ("-> bksig       : %s\n", tab[VHD_bksigDFN]);
         (void)printf ("-> loins       : %s\n", tab[VHD_loinsDFN]);
         (void)printf ("-> lotrs       : %s\n", tab[VHD_lotrsDFN]);
         (void)printf ("-> user        : %s\n", tab[VHD_userDFN]);
         (void)printf ("-> next        : %s\n", tab[VHD_nextDFN]);

         break;

      case (VHD_lotrsDFN):

         /* ###--------- lotrs ---------### */

         lotrs_pnt = (struct lotrs *)(current_pnt.data);

         if (lotrs_pnt->NEXT != NULL) {
            tab[VHD_nextDFN] = avail;
            nxt[VHD_nextDFN] = (void *)lotrs_pnt->NEXT;
            typ[VHD_nextDFN] = VHD_lotrsDFN;
         }
         if (lotrs_pnt->DRAIN != NULL) {
            tab[VHD_drainDFN] = avail;
            nxt[VHD_drainDFN] = (void *)lotrs_pnt->DRAIN;
            typ[VHD_drainDFN] = VHD_loconDFN;
         }
         if (lotrs_pnt->GRID != NULL) {
            tab[VHD_gridDFN] = avail;
            nxt[VHD_gridDFN] = (void *)lotrs_pnt->GRID;
            typ[VHD_gridDFN] = VHD_loconDFN;
         }
         if (lotrs_pnt->SOURCE != NULL) {
            tab[VHD_sourceDFN] = avail;
            nxt[VHD_sourceDFN] = (void *)lotrs_pnt->SOURCE;
            typ[VHD_sourceDFN] = VHD_loconDFN;
         }
         if (lotrs_pnt->BULK != NULL) {
            tab[VHD_sourceDFN] = avail;
            nxt[VHD_sourceDFN] = (void *)lotrs_pnt->BULK;
            typ[VHD_sourceDFN] = VHD_loconDFN;
         }
         if (lotrs_pnt->USER != NULL) {
            tab[VHD_userDFN] = avail;
            nxt[VHD_userDFN] = (void *)lotrs_pnt->USER;
            typ[VHD_userDFN] = VHD_ptypeDFN;
         }

         (void)printf ("-> drain       : %s\n", tab[VHD_drainDFN]);
         (void)printf ("-> grid        : %s\n", tab[VHD_gridDFN]);
         (void)printf ("-> source      : %s\n", tab[VHD_sourceDFN]);
         (void)printf ("-> bulk        : %s\n", tab[VHD_bulkDFN]);
         (void)printf ("   length      : %ld\n", lotrs_pnt->LENGTH);
         (void)printf ("   width       : %ld\n", lotrs_pnt->WIDTH);
         (void)printf ("   y           : %ld\n", lotrs_pnt->Y);
         (void)printf ("   x           : %ld\n", lotrs_pnt->X);
         (void)printf ("   type        : %c\n", lotrs_pnt->TYPE);
         (void)printf ("-> user        : %s\n", tab[VHD_userDFN]);
         (void)printf ("-> next        : %s\n", tab[VHD_nextDFN]);

         break;

      case (VHD_loinsDFN):

         /* ###--------- loins ---------### */

         loins_pnt = (struct loins *)(current_pnt.data);

         if (loins_pnt->NEXT != NULL) {
            tab[VHD_nextDFN] = avail;
            nxt[VHD_nextDFN] = (void *)loins_pnt->NEXT;
            typ[VHD_nextDFN] = VHD_loinsDFN;
         }
         if (loins_pnt->LOCON != NULL) {
            tab[VHD_loconDFN] = avail;
            nxt[VHD_loconDFN] = (void *)loins_pnt->LOCON;
            typ[VHD_loconDFN] = VHD_loconDFN;
         }
         if (loins_pnt->USER != NULL) {
            tab[VHD_userDFN] = avail;
            nxt[VHD_userDFN] = (void *)loins_pnt->USER;
            typ[VHD_userDFN] = VHD_ptypeDFN;
         }

         (void)printf ("   insname     : %s\n", loins_pnt->INSNAME);
         (void)printf ("   figname     : %s\n", loins_pnt->FIGNAME);
         (void)printf ("-> locon       : %s\n", tab[VHD_loconDFN]);
         (void)printf ("-> user        : %s\n", tab[VHD_userDFN]);
         (void)printf ("-> next        : %s\n", tab[VHD_nextDFN]);

         break;

      case (VHD_loconDFN):

         /* ###--------- locon ---------### */

         locon_pnt = (struct locon *)(current_pnt.data);

         if (locon_pnt->NEXT != NULL) {
            tab[VHD_nextDFN] = avail;
            nxt[VHD_nextDFN] = (void *)locon_pnt->NEXT;
            typ[VHD_nextDFN] = VHD_loconDFN;
         }
         if (locon_pnt->SIG != NULL) {
            tab[VHD_sigDFN] = avail;
            nxt[VHD_sigDFN] = (void *)locon_pnt->SIG;
            typ[VHD_sigDFN] = VHD_losigDFN;
         }
         if (locon_pnt->ROOT != NULL) {
            tab[VHD_rootDFN] = avail;
            nxt[VHD_rootDFN] = (void *)locon_pnt->ROOT;
            if (locon_pnt->TYPE == 'I')
               typ[VHD_rootDFN] = VHD_loinsDFN;
            else
               typ[VHD_rootDFN] = VHD_lofigDFN;
         }
         if (locon_pnt->USER != NULL) {
            tab[VHD_userDFN] = avail;
            nxt[VHD_userDFN] = (void *)locon_pnt->USER;
            typ[VHD_userDFN] = VHD_ptypeDFN;
         }

         (void)printf ("   name        : %s\n", locon_pnt->NAME);
         (void)printf ("-> sig         : %s\n", tab[VHD_sigDFN]);
         (void)printf ("-> root        : %s\n", tab[VHD_rootDFN]);
         (void)printf ("   direction   : %c\n", locon_pnt->DIRECTION);
         (void)printf ("   type        : %c\n", locon_pnt->TYPE);
         (void)printf ("-> user        : %s\n", tab[VHD_userDFN]);
         (void)printf ("-> next        : %s\n", tab[VHD_nextDFN]);

         break;

      case (VHD_losigDFN):

         /* ###--------- losig ---------### */

         losig_pnt = (struct losig *)(current_pnt.data);

         if (losig_pnt->NEXT != NULL) {
            tab[VHD_nextDFN] = avail;
            nxt[VHD_nextDFN] = (void *)losig_pnt->NEXT;
            typ[VHD_nextDFN] = VHD_losigDFN;
         }
         if (losig_pnt->USER != NULL) {
            tab[VHD_userDFN] = avail;
            nxt[VHD_userDFN] = (void *)losig_pnt->USER;
            typ[VHD_userDFN] = VHD_ptypeDFN;
         }

         ptchain = losig_pnt->NAMECHAIN;
         (void)printf ("   namechain   :\n");
         while (ptchain != NULL) {
            (void)printf ("                 %s\n", (char *)ptchain->DATA);
            ptchain = ptchain->NEXT;
         }

         (void)printf ("   capa        : %f\n", rcn_getcapa (NULL, losig_pnt));
         (void)printf ("   index       : %ld\n", losig_pnt->INDEX);
         (void)printf ("   type        : %c\n", losig_pnt->TYPE);
         (void)printf ("-> user        : %s\n", tab[VHD_userDFN]);
         (void)printf ("-> next        : %s\n", tab[VHD_nextDFN]);

         break;

      case (VHD_ptypeDFN):

         /* ###--------- ptype ---------### */

         ptype_pnt = (ptype_list *) (current_pnt.data);

         if (ptype_pnt->NEXT != NULL) {
            tab[VHD_nextDFN] = avail;
            nxt[VHD_nextDFN] = (void *)ptype_pnt->NEXT;
            typ[VHD_nextDFN] = VHD_ptypeDFN;
         }
         if (ptype_pnt->DATA != NULL) {
            tab[VHD_dataDFN] = avail;
            nxt[VHD_dataDFN] = (void *)ptype_pnt->DATA;
            typ[VHD_dataDFN] = VHD_ERRDFN;
/*......typ[VHD_dataDFN] = .......................*/
         }

         (void)printf ("   type        : %ld\n", ptype_pnt->TYPE);
         (void)printf ("-> data        : %s\n", tab[VHD_dataDFN]);
         (void)printf ("-> next        : %s\n", tab[VHD_nextDFN]);

         break;


      }

      /* ###------------------------------------------------------### */
      /*    Reading new command                   */
      /* ###------------------------------------------------------### */

      readcmd = vhd_getcmd (readtab);

      for (i = 0; i < VHD_MAXDFN; i++) {
         if (readcmd == key[i])
            break;
      }
      if ((i != VHD_MAXDFN + 1) && (tab[i] == avail))
         newsta0 = i;
      else
         newsta0 = -1;

      /* ###------------------------------------------------------### */
      /*    Analysing new command                 */
      /* ###------------------------------------------------------### */

      switch (newsta0) {
      case (VHD__UPDFN):
         current_pnt = stk[--stk_pnt];
         break;

      case (VHD__BKDFN):
         while ((--stk_pnt != 0) && (stk[stk_pnt].mark != 1));
         current_pnt = stk[stk_pnt];
         break;

      case (VHD__SPDFN):
         current_pnt.mark = 1;
         break;

      case (VHD__TPDFN):
         current_pnt = stk[0];
         stk_pnt = 0;
         break;

      case (VHD__XTDFN):
         current_pnt.data = NULL;
         current_pnt.type = VHD__XTDFN;
         current_pnt.mark = 0;
         break;

      case (VHD_ERRDFN):
         break;

      default:
         stk[stk_pnt++] = current_pnt;
         current_pnt.type = typ[newsta0];
         current_pnt.mark = 0;
         current_pnt.data = nxt[newsta0];
         break;

      }

      state = current_pnt.type;
   }
}


static int vhd_getcmd (prvcmd)

char prvcmd[3][20];

{
   char readstr[60];
   char comd0[20];
   char comd1[20];
   char comd2[20];
   int code;

   (void)printf ("\n\n\n COMMAND >> ");
   comd0[0] = '\0';
   comd1[0] = '\0';
   comd2[0] = '\0';
   (void)fgets (readstr, 60, stdin);
   (void)sscanf (readstr, "%s%s%s", comd0, comd1, comd2);

   if (strcmp (comd0, ".")) {
      (void)strcpy (prvcmd[0], comd0);
      (void)strcpy (prvcmd[1], comd1);
      (void)strcpy (prvcmd[2], comd2);
   }
   code = vhd_hash (prvcmd[0]);

   (void)printf ("\n\n\n\n\n");
   return (code);
}



static int vhd_hash (str)

char *str;

{
   int code = 0;

   while (*str != '\0')
      code = code << 1 ^ *str++;

   return (code);
}

optparam_list *dupoptparamlst (optparam_ptr)
optparam_list *optparam_ptr;
{
   optparam_list *optparam_rpt = NULL;   /* Returned locon pointer   */
   optparam_list *optparam_tmpptr = NULL;   /* Temporary pointer        */

   while (optparam_ptr != NULL) {
      optparam_tmpptr = dupoptparam (optparam_ptr);
      optparam_tmpptr->NEXT = optparam_rpt;
      optparam_rpt = optparam_tmpptr;

      optparam_ptr = optparam_ptr->NEXT;
   }
   optparam_rpt = (optparam_list *) reverse ((chain_list *) optparam_rpt);
   return (optparam_rpt);
}

optparam_list *dupoptparam (optparam_ptr)
optparam_list *optparam_ptr;
{
   optparam_list *optparam_rpt = NULL;   /* Returned optparam pointer    */
//   eqt_ctx dummy;
   static eqt_ctx *dummy=NULL;
   if (optparam_ptr == NULL)
      return NULL;

   optparam_rpt = (optparam_list *) (mbkalloc (sizeof (optparam_list)));
   *optparam_rpt = *optparam_ptr;
   if (optparam_ptr->TAG=='$')
     {
       if (optparam_rpt->EQT_EXPR==NULL)
         {
           if (dummy==NULL) dummy=spi_init_eqt ();
//           optparam_ptr->EQT_EXPR=optparam_rpt->EQT_EXPR=eqt_create (&dummy, optparam_ptr->UDATA.EXPR);
           optparam_ptr->EQT_EXPR=optparam_rpt->EQT_EXPR=eqt_create (dummy, optparam_ptr->UDATA.EXPR);
           if (optparam_rpt->EQT_EXPR!=NULL) ((eqt_node *)optparam_rpt->EQT_EXPR)->REF_COUNT=2;
//           eqt_term (dummy);
         }
       else
         ((eqt_node *)optparam_rpt->EQT_EXPR)->REF_COUNT++;
     }
   optparam_rpt->NEXT = NULL;

   return (optparam_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Author : VUONG H.N.                     */
/*  Date   : Dec 11 1991                  */
/*  Cont.  : Essentially functions to duplicate lofig structures   */
/*           such a complete LOFIG or just a chain_list.      */
/*##------------------------------------------------------------------##*/

/*##------------------------------------------------------------------##*/
/*  Function : duploconlst()                  */
/*  contents : duplicate a locon list and return a pointer on the new   */
/*             structure.                  */
/*  called func. : duplocon(), reverse(),            */
/*##------------------------------------------------------------------##*/

locon_list *duploconlst (locon_ptr)
locon_list *locon_ptr;
{
   locon_list *locon_rpt = NULL;   /* Returned locon pointer   */
   locon_list *locon_tmpptr = NULL;   /* Temporary pointer        */

   while (locon_ptr != NULL) {
      locon_tmpptr = duplocon (locon_ptr);
      locon_tmpptr->NEXT = locon_rpt;
      locon_rpt = locon_tmpptr;

      locon_ptr = locon_ptr->NEXT;
   }
   locon_rpt = (locon_list *) reverse ((chain_list *) locon_rpt);
   return (locon_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duplocon()                  */
/*  contents : duplicate a locon and return a pointer on the new   */
/*          structure.                        */
/*  called func. : mbkalloc(),                    */
/*  note : the ROOT and the SIG pointer are also duplicated      */
/*##------------------------------------------------------------------##*/

locon_list *duplocon (locon_ptr)
locon_list *locon_ptr;
{
   locon_list *locon_rpt = NULL;   /* Returned locon pointer   */
   num_list *sn;
   ptype_list *ptnodename;

   if (locon_ptr == NULL)
      return NULL;

   locon_rpt = (locon_list *) (mbkalloc (sizeof (locon_list)));
   locon_rpt->NEXT = NULL;
   locon_rpt->NAME = locon_ptr->NAME;
   locon_rpt->SIG = locon_ptr->SIG;
   locon_rpt->ROOT = locon_ptr->ROOT;
   locon_rpt->DIRECTION = locon_ptr->DIRECTION;
   locon_rpt->TYPE = locon_ptr->TYPE;

   locon_rpt->PNODE = NULL;
   for (sn = locon_ptr->PNODE; sn; sn = sn->NEXT)
      locon_rpt->PNODE = addnum (locon_rpt->PNODE, sn->DATA);
   locon_rpt->PNODE = (num_list *) reverse ((chain_list *) locon_rpt->PNODE);

   locon_rpt->USER = NULL;      /* The ptype_list is not duplicated */

   if ((ptnodename = getptype (locon_ptr->USER, PNODENAME)))
      locon_rpt->USER = addptype (locon_rpt->USER, PNODENAME, dupchainlst (ptnodename->DATA)
         );
   if ((ptnodename = getptype (locon_ptr->USER, LOCON_INFORMATION)))
      locon_rpt->USER = addptype (locon_rpt->USER, LOCON_INFORMATION, ptnodename->DATA);

   return (locon_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duploinslst()                  */
/*  contents : duplicate a loins list and return a pointer on the new   */
/*             structure.                  */
/*  called func. : duploins(), reverse(),            */
/*##------------------------------------------------------------------##*/

loins_list *duploinslst (loins_ptr)
loins_list *loins_ptr;
{
   loins_list *loins_rpt = NULL;   /* Returned loins pointer   */
   loins_list *loins_tmpptr = NULL;   /* Temporary pointer        */

   while (loins_ptr != NULL) {
      loins_tmpptr = duploins (loins_ptr);
      loins_tmpptr->NEXT = loins_rpt;
      loins_rpt = loins_tmpptr;

      loins_ptr = loins_ptr->NEXT;
   }
   loins_rpt = (loins_list *) reverse ((chain_list *) loins_rpt);
   return (loins_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duploins()                  */
/*  contents : duplicate a loins and return a pointer on the new   */
/*          structure.                        */
/*  called func. : mbkalloc(),                   */
/*  note : the LOCON pointer is also duplicated            */
/*##------------------------------------------------------------------##*/

loins_list *duploins (loins_ptr)
loins_list *loins_ptr;
{
   loins_list *loins_rpt = NULL;   /* Returned loins pointer   */
   ptype_list *ptuser;
   xy_struct *xy;

   loins_rpt = (loins_list *) (mbkalloc (sizeof (loins_list)));
   loins_rpt->NEXT = NULL;
   loins_rpt->LOCON = loins_ptr->LOCON;
   loins_rpt->INSNAME = loins_ptr->INSNAME;
   loins_rpt->FIGNAME = loins_ptr->FIGNAME;
   loins_rpt->USER = NULL;      /* The ptype_list is not duplicated */

   if ((ptuser = getptype (loins_ptr->USER, OPT_PARAMS)) != NULL) {
      loins_rpt->USER = addptype (loins_rpt->USER, OPT_PARAMS, dupoptparamlst ((optparam_list *) ptuser->DATA));
   }

   if ((ptuser = getptype (loins_ptr->USER, MSL_XY_PTYPE)) != NULL) {
      xy = mbkalloc (sizeof (struct xy_struct));
      xy->X = ((xy_struct*)ptuser->DATA)->X;
      xy->Y = ((xy_struct*)ptuser->DATA)->Y;
      xy->A = xy->R = xy->Tx = xy->Ty = 0;
      loins_rpt->USER = addptype (loins_rpt->USER, MSL_XY_PTYPE, xy);
   }

   return (loins_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duplotrslst()                  */
/*  contents : duplicate a lotrs list and return a pointer on the new   */
/*             structure.                  */
/*  called func. : duplotrs(), reverse(),            */
/*##------------------------------------------------------------------##*/

lotrs_list *duplotrslst (lotrs_ptr)
lotrs_list *lotrs_ptr;
{
   lotrs_list *lotrs_rpt = NULL;   /* Returned lotrs pointer   */
   lotrs_list *lotrs_tmpptr = NULL;   /* Temporary pointer        */

   while (lotrs_ptr != NULL) {
      lotrs_tmpptr = duplotrs (lotrs_ptr);
      lotrs_tmpptr->NEXT = lotrs_rpt;
      lotrs_rpt = lotrs_tmpptr;

      lotrs_ptr = lotrs_ptr->NEXT;
   }
   lotrs_rpt = (lotrs_list *) reverse ((chain_list *) lotrs_rpt);
   return (lotrs_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duplotrs()                  */
/*  contents : duplicate a lotrs and return a pointer on the new   */
/*          structure.                        */
/*  called func. : mbkalloc(), duplocon()             */
/*  note : the DRAIN, SOURCE, GRID, BULK pointers are also duplicated   */
/*##------------------------------------------------------------------##*/

lotrs_list *duplotrs (lotrs_ptr)
lotrs_list *lotrs_ptr;
{
   lotrs_list *lotrs_rpt = NULL;   /* Returned lotrs pointer   */
   ptype_list *ptuser;

   lotrs_rpt = (lotrs_list *) mbkalloc (sizeof (lotrs_list));
   lotrs_rpt->NEXT = NULL;
   lotrs_rpt->TRNAME = lotrs_ptr->TRNAME;
   lotrs_rpt->DRAIN = lotrs_ptr->DRAIN;
   lotrs_rpt->GRID = lotrs_ptr->GRID;
   lotrs_rpt->SOURCE = lotrs_ptr->SOURCE;
   lotrs_rpt->BULK = lotrs_ptr->BULK;
   lotrs_rpt->WIDTH = lotrs_ptr->WIDTH;
   lotrs_rpt->LENGTH = lotrs_ptr->LENGTH;
   lotrs_rpt->PS = lotrs_ptr->PS;
   lotrs_rpt->PD = lotrs_ptr->PD;
   lotrs_rpt->XS = lotrs_ptr->XS;
   lotrs_rpt->XD = lotrs_ptr->XD;
   lotrs_rpt->X = lotrs_ptr->X;
   lotrs_rpt->Y = lotrs_ptr->Y;
   lotrs_rpt->TYPE = lotrs_ptr->TYPE;
   lotrs_rpt->MODINDEX = lotrs_ptr->MODINDEX;
   lotrs_rpt->USER = NULL;      /* The ptype_list is not duplicated */

   if ((ptuser = getptype (lotrs_ptr->USER, OPT_PARAMS)) != NULL) {
      lotrs_rpt->USER = addptype (lotrs_rpt->USER, OPT_PARAMS, dupoptparamlst ((optparam_list *) ptuser->DATA));
   }
   if ((ptuser = getptype (lotrs_ptr->USER, TRANS_FIGURE)) != NULL) {
      lotrs_rpt->USER = addptype (lotrs_rpt->USER, TRANS_FIGURE, ptuser->DATA);
   }

   return (lotrs_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : rduplotrs()                                              */
/*  contents : duplicate a lotrs and return a pointer on the new        */
/*             structure. The locon structures are duplicated but       */
/*             the losig is kept as the original                        */
/*##------------------------------------------------------------------##*/

lotrs_list *rduplotrs (lotrs_ptr)
    lotrs_list *lotrs_ptr;
{
    lotrs_list *lotrs_rpt = NULL;   /* Returned lotrs pointer   */

    lotrs_rpt = duplotrs(lotrs_ptr);
    lotrs_rpt->GRID = duplocon (lotrs_rpt->GRID);
    lotrs_rpt->GRID->ROOT = lotrs_rpt;
    lotrs_rpt->SOURCE = duplocon (lotrs_rpt->SOURCE);
    lotrs_rpt->SOURCE->ROOT = lotrs_rpt;
    lotrs_rpt->DRAIN = duplocon (lotrs_rpt->DRAIN);
    lotrs_rpt->DRAIN->ROOT = lotrs_rpt;
    if (lotrs_rpt->BULK) {
        lotrs_rpt->BULK = duplocon (lotrs_rpt->BULK);
        lotrs_rpt->BULK->ROOT = lotrs_rpt;
    }
    return (lotrs_rpt);
}

loins_list *rduploins (loins_list *loins_ptr)
{
    loins_list *loins_rpt = NULL;   /* Returned loins pointer   */
    locon_list *ptcon;

    loins_rpt = duploins(loins_ptr);
    loins_rpt->LOCON = duploconlst (loins_rpt->LOCON);
    for (ptcon = loins_rpt->LOCON; ptcon; ptcon = ptcon->NEXT) {
        ptcon->ROOT = loins_rpt;
    }
    return (loins_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duplosiglst()                  */
/*  contents : duplicate a losig_list and return a pointer on the new   */
/*             structure, a pointer on BKSIG must be given.      */
/*  called func. :duplosig(), reverse(),            */
/*##------------------------------------------------------------------##*/

losig_list *duplosiglst (losig_list *losig_ptr, lofig_list *lf)
{
   losig_list *losig_rpt = NULL;   /* Returned losig pointer   */
   losig_list *losig_tmpptr = NULL;   /* Temporary pointer        */

   while (losig_ptr != NULL) {
      losig_tmpptr = duplosig (losig_ptr, lf);
      losig_tmpptr->NEXT = losig_rpt;
      losig_rpt = losig_tmpptr;

      losig_ptr = losig_ptr->NEXT;
   }
   losig_rpt = (losig_list *) reverse ((chain_list *) losig_rpt);
   return (losig_rpt);
}


/*##------------------------------------------------------------------##*/
/*  Function : duplosig()                                               */
/*  contents : duplicate a losig and return a pointer on the new        */
/*             structure, a pointer on the BKSIG must be given.         */
/*  called func. : mbkalloc, addptype, dupchainlst         */
/*##------------------------------------------------------------------##*/

losig_list *duplosig (losig_list *losig_ptr, lofig_list *lf)
{
   long index = losig_ptr->INDEX;
   losig_list *ptsig = NULL;
   ptsig=mbk_NewBKSIG_getindex(lf, index);
   /* check index  unicity */
   if (ptsig->INDEX != 0L) avt_errmsg(MBK_ERRMSG, "015", AVT_FATAL, index);

   ptsig->NEXT = NULL;
   ptsig->NAMECHAIN = dupchainlst (losig_ptr->NAMECHAIN);
   ptsig->PRCN = NULL;
   ptsig->INDEX = losig_ptr->INDEX;
   ptsig->USER = NULL;
   ptsig->PRCN = NULL;         /* The rc data is not duplicated    */
   ptsig->TYPE = losig_ptr->TYPE;   /* The ptype_list is not duplicated */
   ptsig->RCNCACHE = 0;
   ptsig->ALIMFLAGS= losig_ptr->ALIMFLAGS;

   duplosigalim (losig_ptr, ptsig);
   mbk_dupvcardnodes(losig_ptr, ptsig);

   if (TRACE_MODE == 'Y') {
      (void)fprintf (stdout, "--- mbk --- duplosig : %ld ", index);
      if (ptsig->NAMECHAIN)
         if (ptsig->NAMECHAIN->DATA)
            (void)fprintf (stdout, "name : %s", (char *)((chain_list *) (ptsig->NAMECHAIN))->DATA);
      (void)fprintf (stdout, "\n");
   }

   return ptsig;
}

/*##------------------------------------------------------------------##*/
/*  Function : duplofiglst()                  */
/*  contents : duplicate a lofig list and return a pointer on the new   */
/*             structure.                  */
/*  called func. : duplofig(), reverse(),            */
/*##------------------------------------------------------------------##*/

lofig_list *duplofiglst (lofig_ptr)
lofig_list *lofig_ptr;
{
   lofig_list *lofig_rpt = NULL;   /* Returned lofig pointer   */
   lofig_list *lofig_tmpptr = NULL;   /* Temporary pointer        */

   while (lofig_ptr != NULL) {
      lofig_tmpptr = duplofig (lofig_ptr);
      lofig_tmpptr->NEXT = lofig_rpt;
      lofig_rpt = lofig_tmpptr;

      lofig_ptr = lofig_ptr->NEXT;
   }
   lofig_rpt = (lofig_list *) reverse ((chain_list *) lofig_rpt);
   return (lofig_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : duplofig()                  */
/*  contents : duplicate a lofig and return a pointer on the new   */
/*          structure.                        */
/*  called func. : mbkalloc(), dupchainlst(),            */
/*  note : the LOCON, LOSIG, BKSIG, LOINS, LOTRS pointers are       */
/*         also duplicated                  */
/*##------------------------------------------------------------------##*/

lofig_list *duplofig (lofig_ptr)
lofig_list *lofig_ptr;
{
   lofig_list *lofig_rpt = NULL;   /* Returned lofig pointer   */
   ptype_list *ptuser;
   lofiginfo *lfif, *lfif0;
   
   lofig_rpt = (lofig_list *) (mbkalloc (sizeof (lofig_list)));
   mbk_init_NewBKSIG(&lofig_rpt->BKSIG);
   lofig_rpt->USER = NULL;      /* The ptype_list is not duplicated */
   setsigsize (lofig_rpt, getsigsize (lofig_ptr));
   lofig_rpt->NEXT = NULL;
   lofig_rpt->MODELCHAIN = dupchainlst (lofig_ptr->MODELCHAIN);
   lofig_rpt->LOCON = lofig_ptr->LOCON;
   lofig_rpt->LOSIG = lofig_ptr->LOSIG;
   memcpy(&lofig_rpt->BKSIG, &lofig_ptr->BKSIG, sizeof(NewBKSIG));
   lofig_rpt->LOINS = lofig_ptr->LOINS;
   lofig_rpt->LOTRS = lofig_ptr->LOTRS;
   lofig_rpt->NAME = lofig_ptr->NAME;
   lofig_rpt->MODE = lofig_ptr->MODE;

   if ((ptuser = getptype (lofig_ptr->USER, OPT_PARAMS)) != NULL) {
      lofig_rpt->USER = addptype (lofig_rpt->USER, OPT_PARAMS, dupoptparamlst ((optparam_list *) ptuser->DATA));
   }
   
   if ((lfif=mbk_getlofiginfo(lofig_ptr, 0))!=NULL)
   {
     lfif0=mbk_getlofiginfo(lofig_rpt, 1);
     memcpy(lfif0, lfif, sizeof(lofiginfo));
   }
   
   return (lofig_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : rduplofig()                  */
/*  contents : recursively duplicate a lofig and return a pointer on    */
/*             the new structure. This structure is the entirely   */
/*             double of the lofig duplicated. Except, that all the   */
/*             USER fields are empty.                 */
/*  called func. : mbkalloc(),                   */
/*##------------------------------------------------------------------##*/

lofig_list *rduplofig (lofig_ptr)
lofig_list *lofig_ptr;
{
   lofig_list *lofig_rpt = NULL;   /* Returned lofig pointer   */
   locon_list *locon_pt = NULL;
   loins_list *loins_pt = NULL;
   lotrs_list *lotrs_pt = NULL;
   losig_list *losig_pt = NULL;
   chain_list *headctc;
   chain_list *scanchain;
   loctc_list *ptctc, *dupctc;
   ptype_list *ptuser;
   long index = 0;
   int cnt;

   lofig_rpt = duplofig (lofig_ptr);
   mbk_init_NewBKSIG(&lofig_rpt->BKSIG);
                   
   lofig_rpt->LOSIG = duplosiglst (lofig_rpt->LOSIG, lofig_rpt);
   lofig_rpt->LOCON = duploconlst (lofig_rpt->LOCON);
   lofig_rpt->LOINS = duploinslst (lofig_rpt->LOINS);
   lofig_rpt->LOTRS = duplotrslst (lofig_rpt->LOTRS);

   /* Now verify and restitute the coherence of the figure   */

   /* Restitute coherence in the LOTRS List */
   lotrs_pt = lofig_rpt->LOTRS;
   while (lotrs_pt != NULL) {
      lotrs_pt->GRID = duplocon (lotrs_pt->GRID);
      locon_pt = lotrs_pt->GRID;
      while (locon_pt != NULL) {
         index = (locon_pt->SIG)->INDEX;
         locon_pt->SIG = getlosig (lofig_rpt, index);
         locon_pt->ROOT = (void *)lotrs_pt;

         locon_pt = locon_pt->NEXT;
      }

      lotrs_pt->DRAIN = duplocon (lotrs_pt->DRAIN);
      locon_pt = lotrs_pt->DRAIN;
      while (locon_pt != NULL) {
         index = (locon_pt->SIG)->INDEX;
         locon_pt->SIG = getlosig (lofig_rpt, index);
         locon_pt->ROOT = (void *)lotrs_pt;

         locon_pt = locon_pt->NEXT;
      }

      lotrs_pt->SOURCE = duplocon (lotrs_pt->SOURCE);
      locon_pt = lotrs_pt->SOURCE;
      while (locon_pt != NULL) {
         index = (locon_pt->SIG)->INDEX;
         locon_pt->SIG = getlosig (lofig_rpt, index);
         locon_pt->ROOT = (void *)lotrs_pt;

         locon_pt = locon_pt->NEXT;
      }

      // zinaps le 20/7/2002
      if (lotrs_pt->BULK) {
         lotrs_pt->BULK = duplocon (lotrs_pt->BULK);
      }
      else
         lotrs_pt->BULK = NULL;
      locon_pt = lotrs_pt->BULK;
      while (locon_pt != NULL) {
         if (locon_pt->SIG) {
            index = (locon_pt->SIG)->INDEX;
            locon_pt->SIG = getlosig (lofig_rpt, index);
         }
         else {
            locon_pt->SIG = NULL;
         }
         locon_pt->ROOT = (void *)lotrs_pt;

         locon_pt = locon_pt->NEXT;
      }

      lotrs_pt = lotrs_pt->NEXT;
   }

   /* Restitute coherence in the LOINS List */
   loins_pt = lofig_rpt->LOINS;
   while (loins_pt != NULL) {
      loins_pt->LOCON = duploconlst (loins_pt->LOCON);
      locon_pt = loins_pt->LOCON;
      while (locon_pt != NULL) {
         index = (locon_pt->SIG)->INDEX;
         locon_pt->SIG = getlosig (lofig_rpt, index);
         locon_pt->ROOT = (void *)loins_pt;

         locon_pt = locon_pt->NEXT;
      }
      loins_pt = loins_pt->NEXT;
   }

   /* Restitute coherence in the LOCON List */
   locon_pt = lofig_rpt->LOCON;
   while (locon_pt != NULL) {
      index = (locon_pt->SIG)->INDEX;
      locon_pt->SIG = getlosig (lofig_rpt, index);
      locon_pt->ROOT = (void *)lofig_rpt;

      locon_pt = locon_pt->NEXT;
   }

   /* Duplicate rc data */
   for (losig_pt = lofig_ptr->LOSIG; losig_pt; losig_pt = losig_pt->NEXT)
      duplorcnet (getlosig (lofig_rpt, losig_pt->INDEX), losig_pt);

   headctc = getallctc (lofig_ptr);
   for (scanchain = headctc; scanchain; scanchain = scanchain->NEXT) {
      ptctc = (loctc_list *) scanchain->DATA;
      dupctc = addloctc (getlosig (lofig_rpt, ptctc->SIG1->INDEX), ptctc->NODE1,
                     getlosig (lofig_rpt, ptctc->SIG2->INDEX), ptctc->NODE2, ptctc->CAPA);
      if ((ptuser = getptype (ptctc->USER, OPT_PARAMS)))
         dupctc->USER = addptype (dupctc->USER, OPT_PARAMS, dupoptparamlst ((optparam_list *) ptuser->DATA));
      if ((cnt=rcn_isCapaDiode(ptctc))!=0) rcn_setCapaDiode(dupctc, cnt);
      if ((ptuser=getptype(ptctc->USER, MSL_CAPANAME))!=NULL)
        dupctc->USER = addptype (dupctc->USER, MSL_CAPANAME, ptuser->DATA);

   }
   freechain (headctc);

   return (lofig_rpt);
}

/* sort routines :
   connectors and signals may have to be sorted by name for some
   language purposes. */

static char Buffer0[255];      /* may be of use for other functions */

static int connectorcmp (flc, slc)
locon_list **flc, **slc;
{
   return naturalstrcmp ((*slc)->NAME, (*flc)->NAME);
}

void sortlocon (connectors)
locon_list **connectors;
{
   long n, i;
   locon_list *lc, **tc;

   if (!connectors || !(*connectors))
      return;

   /* first :
      count connectors. */
   for (n = 0, lc = *connectors; lc; lc = lc->NEXT)
      n++;
   tc = (locon_list **) mbkalloc (n * sizeof (locon_list *));
   for (n = 0, lc = *connectors; lc; lc = lc->NEXT)
      tc[n++] = lc;
   /* second :
      sort them. */
   qsort ((void *)tc, n, sizeof (locon_list *), connectorcmp);
   /* tables :
      the last element of the table is t[n - 1].
      relink the list in the sorted order. */
   n--;
   *connectors = tc[0];
   for (i = 0; i < n; i++)
      tc[i]->NEXT = tc[i + 1];
   tc[n]->NEXT = (locon_list *) NULL;
   mbkfree ((void *)tc);
}

static int signalcmp (fls, sls)
losig_list **fls, **sls;
{
   return naturalstrcmp (getsigname (*fls), getsigname (*sls));
}

void sortlosig (signals)
losig_list **signals;
{
   long n, i;
   losig_list *lc, **tc;

   if (!signals || !(*signals))
      return;

   /* first :
      count signals. */
   for (n = 0, lc = *signals; lc; lc = lc->NEXT)
      n++;
   tc = (losig_list **) mbkalloc (n * sizeof (losig_list *));
   for (n = 0, lc = *signals; lc; lc = lc->NEXT)
      tc[n++] = lc;
   /* second :
      sort them. */
   qsort ((void *)tc, n, sizeof (losig_list *), signalcmp);
   /* tables :
      the last element of the table is t[n - 1].
      relink the list in the sorted order. */
   n--;
   *signals = tc[0];
   for (i = 0; i < n; i++)
      tc[i]->NEXT = tc[i + 1];
   tc[n]->NEXT = (losig_list *) NULL;
   mbkfree ((void *)tc);
}

/* checking :
   the validity of a list of connectors for vectorization purposes
   is to be checked for some file formats, so let's do it.
   The algorithm checks that :
      vectorized radical do not appear on themselves
      vectorized connectors are all together and ordered properly */
#define HTSIZE 200
int checkloconorder (c)
locon_list *c;
{
   char *s = NULL, *t = NULL;
   ht *table;
   int first = 1, previous = 0, delta = 0, current = 0;

   table = addht (HTSIZE);
   while (c) {
      /* vectorized ?
         no. */
      if ((s = strchr (c->NAME, ' ')) == NULL) {
        avoid_redundancy:
         if (sethtitem (table, (void *)c->NAME, 0)) {
            delht (table);
            avt_errmsg(MBK_ERRMSG, "016", AVT_FATAL, c->NAME);
            return 1;
         }
         first = 1;
         c = c->NEXT;
      }
      else {
         while (c) {
            int i = 1;
            /* redundancy :
               I'ven't found an elegant way to do that, too bad. */
            if (!s)
               if ((s = strchr (c->NAME, ' ')) == NULL)
                  goto avoid_redundancy;
            /* Temporary change :
               alter the string contents just a microsecond. */
            *s = '\0';
            strcpy (Buffer0, c->NAME);
            *s = ' ';
            while (*(s + i) && isdigit ((int)*(s + i)))
               i++;
            if (*(s + i)) {
               delht (table);
               avt_errmsg(MBK_ERRMSG, "017", AVT_FATAL, Buffer0, s + 1, c->NAME);
               return 1;
            }
            current = atoi (s + 1);
            s = NULL;
            /* first :
               the name is not yet known. so I store it.
               we do that each time a new vector is encountered. */
            if (first) {
              its_first:
               if (sethtitem (table, (void *)(t = namealloc (Buffer0)), 0)) {
                  delht (table);
                  avt_errmsg(MBK_ERRMSG, "016", AVT_FATAL, Buffer0);
                  return 1;
               }
               delta = first = 0;
               previous = current;
               c = c->NEXT;
            }
            else {         /* let's see if it's a good vhdl legal vector */
               /* its a new vector :
                  in fact this vector follows an other vector! */
               if (t != namealloc (Buffer0))
                  goto its_first;
               if (!delta)
                  delta = previous - current;
               else if (delta != previous - current) {
                  delht (table);
                  avt_errmsg(MBK_ERRMSG, "018", AVT_FATAL, Buffer0, Buffer0, previous, Buffer0, current); 
               }
               if (delta != 1 && delta != -1) {
                  delht (table);
                  avt_errmsg(MBK_ERRMSG, "018", AVT_FATAL, c->NAME, Buffer0, previous, Buffer0, current); 
               }
               previous = current;
               c = c->NEXT;
            }
         }
      }
   }
   delht (table);
   return 0;
}

//_____________________________________________________

int bigcomp (const void *p1, const void *p2)
{
   losig_list *sig1, *sig2;
   int index1, index2;

   sig1 = *(losig_list **) p1;
   sig2 = *(losig_list **) p2;

   index1 = vectorindex ((char *)sig1->NAMECHAIN->DATA);
   index2 = vectorindex ((char *)sig2->NAMECHAIN->DATA);

   if (index1 > index2)
      return -1;
   if (index1 < index2)
      return 1;

   return 0;
}

//_____________________________________________________

int littlecomp (const void *p1, const void *p2)
{
   losig_list *sig1, *sig2;
   int index1, index2;

   sig1 = *(losig_list **) p1;
   sig2 = *(losig_list **) p2;

   index1 = vectorindex ((char *)sig1->NAMECHAIN->DATA);
   index2 = vectorindex ((char *)sig2->NAMECHAIN->DATA);

   if (index1 > index2)
      return 1;
   if (index1 < index2)
      return -1;

   return 0;
}


//_____________________________________________________

chain_list *sort_sigs (losig_list * sig_list, ht * endht)
{
   char *radix, *stocked_sig_radix;
   losig_list *sig, *stocked_sig;
   chain_list *ch, *ch1, *sigchain = NULL;
   ht *htab;
   losig_list **sig_array;
   chain_list *ordered_list = NULL;
   long n;
   int i, endianess;

   /* Sort by name */

   htab = addht (100);

   for (sig = sig_list; sig; sig = sig->NEXT) {
      radix = vectorradical (sig->NAMECHAIN->DATA);
      if (gethtitem (htab, radix) == EMPTYHT) {
         sigchain = addchain (sigchain, addchain (NULL, sig));
         addhtitem (htab, radix, 0);
      }
      else {
         for (ch = sigchain; ch; ch = ch->NEXT) {
            stocked_sig = (losig_list *) ((chain_list *) ch->DATA)->DATA;
            stocked_sig_radix = vectorradical ((char *)stocked_sig->NAMECHAIN->DATA);
            if (stocked_sig_radix == radix) {
               ch->DATA = addchain (ch->DATA, sig);
               break;
            }
         }
      }
   }

   delht (htab);

   /* Create arrays and sort */

   for (ch = sigchain; ch; ch = ch->NEXT) {
      n = 0;
      for (ch1 = (chain_list *) ch->DATA; ch1; ch1 = ch1->NEXT)
         n++;
      sig_array = (losig_list **) mbkalloc ((n + 1) * sizeof (losig_list *));
      i = 0;
      sig_array[i++] = (losig_list *) n;
      for (ch1 = (chain_list *) ch->DATA; ch1; ch1 = ch1->NEXT)
         sig_array[i++] = (losig_list *) ch1->DATA;
      freechain ((chain_list *) ch->DATA);
      ch->DATA = sig_array;
      if (n > 1) {
         if (!endht)
            qsort (&sig_array[1], n, sizeof (losig_list *), littlecomp);
         else {
            radix = vectorradical ((char *)((losig_list *) (((chain_list *) ch->DATA)->DATA))->NAMECHAIN->DATA);
            if ((endianess = gethtitem (endht, radix)) != EMPTYHT) {
               if (endianess == MBK_LITTLE_ENDIAN)
                  qsort (&sig_array[1], n, sizeof (losig_list *), littlecomp);
               else if (endianess == MBK_BIG_ENDIAN)
                  qsort (&sig_array[1], n, sizeof (losig_list *), bigcomp);
               else
                  qsort (&sig_array[1], n, sizeof (losig_list *), littlecomp);
            }
            else
               qsort (&sig_array[1], n, sizeof (losig_list *), littlecomp);
         }
      }
   }

   /* Reorder losigs */

   for (ch = sigchain; ch; ch = ch->NEXT) {
      sig_array = (losig_list **) ch->DATA;
      for (i = 1; i <= (long)sig_array[0]; i++)
         ordered_list = addchain (ordered_list, sig_array[i]);
      mbkfree (sig_array);
   }

   freechain (sigchain);
   return ordered_list;
}

void vectorbounds (lofig_list * ptfig, char *radical, int *begin, int *end)
{
   locon_list *ptcon;
   int counting = 0, start=0;

   ptcon = ptfig->LOCON;
   *begin=-1;
   *end=-1;
   
   while (ptcon) 
     {
       if (vectorradical (ptcon->NAME) == radical)
         {
           counting=vectorindex(ptcon->NAME);
           if (counting==-1) return;
           if (*begin==-1) *begin=counting;
           *end=counting;
           start=1;
         }
       else if (start) break;
       ptcon = ptcon->NEXT;
     }
}

void ClearLOTRSLoconFlags (lotrs_list * tr)
{
   tr->GRID->FLAGS = 0;
   tr->SOURCE->FLAGS = 0;
   tr->DRAIN->FLAGS = 0;
   if (tr->BULK != NULL)
      tr->BULK->FLAGS = 0;
}

void ClearAllLOTRSLoconFlags (lotrs_list * tr)
{
   while (tr != NULL) {
      tr->GRID->FLAGS = 0;
      tr->SOURCE->FLAGS = 0;
      tr->DRAIN->FLAGS = 0;
      if (tr->BULK != NULL)
         tr->BULK->FLAGS = 0;
      tr = tr->NEXT;
   }
}

void ClearAllLoconFlags (locon_list * lc)
{
   while (lc != NULL) {
      lc->FLAGS = 0;
      lc = lc->NEXT;
   }
}

void ClearAllLOINSLoconFlags (loins_list * li)
{
   while (li != NULL) {
      ClearAllLoconFlags (li->LOCON);
      li = li->NEXT;
   }
}

void ClearLOFIGLoconFlags (lofig_list * lf)
{
   ClearAllLoconFlags (lf->LOCON);
   ClearAllLOINSLoconFlags (lf->LOINS);
   ClearAllLOTRSLoconFlags (lf->LOTRS);
}

void freeflatmodel (lofig_list * fig)
{
   losig_list *ptsig;
   locon_list *ptcon;
   lotrs_list *pttrs;
   void *ptold = NULL;

   if (fig->LOINS)
      return;
   if (getptype (fig->USER, LOFIG_LOCK))
      return;

   freectclist (fig, NULL);
   for (ptsig = fig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
      freechain (ptsig->NAMECHAIN);
      if (ptsig->PRCN)
         freelorcnet (ptsig);
      dellosiguser (ptsig);
   }

   mbk_free_NewBKSIG(&fig->BKSIG);
   for (ptcon = fig->LOCON; ptcon; ptcon = (locon_list *) ptold) {
      ptold = (void *)ptcon->NEXT;
      delloconuser (ptcon);
      mbkfree (ptcon);
   }

   for (pttrs = fig->LOTRS; pttrs; pttrs = (lotrs_list *) ptold) {
      delloconuser (pttrs->GRID);
      delloconuser (pttrs->SOURCE);
      delloconuser (pttrs->DRAIN);
      if (pttrs->BULK) {
         delloconuser (pttrs->BULK);
         mbkfree (pttrs->BULK);
      }

      mbkfree (pttrs->GRID);
      mbkfree (pttrs->SOURCE);
      mbkfree (pttrs->DRAIN);
      //      mbkfree((void *)pttrs->BULK);
      ptold = pttrs->NEXT;
      dellotrsuser (pttrs);
      mbkfree (pttrs);
   }

   dellofiguser (fig);
   freechain (fig->MODELCHAIN);
   mbkfree (fig);
}


static void merge_trans (lotrs_list * pttrans, lotrs_list * pt2trans)
{
   ptype_list *ptuser;

   if ((ptuser = getptype (pttrans->USER, MBK_TRANS_PARALLEL)) == NULL) {
      pttrans->USER = addptype (pttrans->USER, MBK_TRANS_PARALLEL, addchain (NULL, pttrans));
      ptuser = pttrans->USER;
   }
   ptuser->DATA = addchain ((chain_list *) ptuser->DATA, pt2trans);
}

/****************************************************************************
 *                         function mbkTranscompare();                      *
 ****************************************************************************/
static int mbkTransCompare (const void *elem1, const void *elem2)
{
   long sig1, sig2;
   lotrs_list *t1, *t2;

   t1 = *(lotrs_list **) elem1;
   t2 = *(lotrs_list **) elem2;
   sig1 = (long)t1->SOURCE->SIG + (long)t1->DRAIN->SIG;
   sig2 = (long)t2->SOURCE->SIG + (long)t2->DRAIN->SIG;

   if (sig1 > sig2) return 1;
   else if (sig1 < sig2) return -1;
   
   if (t1->LENGTH > t2->LENGTH) return 1;
   else if (t1->LENGTH < t2->LENGTH) return -1;

   if (t1->TRNAME!=NULL && t2->TRNAME!=NULL)
     return mbk_casestrcmp(t1->TRNAME, t2->TRNAME);

   return 0;
}

static int mbkSameBBox(lotrs_list *tr0, lotrs_list *tr1)
{
  ptype_list *pt, *pt1;
  pt=getptype(tr0->USER, BBOX_AS_UNUSED);
  pt1=getptype(tr1->USER, BBOX_AS_UNUSED);
  if ((pt==NULL && pt1==NULL) || (pt!=NULL && pt1!=NULL && pt->DATA==pt1->DATA)) return 1;
  return 0;
}

/****************************************************************************
 *                         function mbk_removeparallel();                   *
 ****************************************************************************/
long mbk_removeparallel (lofig_list * ptlofig, int remove)
{
   lotrs_list *pttrans = NULL;
   lotrs_list *pt2trans = NULL;
   lotrs_list *ptprevtrans = NULL;
   lotrs_list *ptnexttrans = NULL;
   losig_list *ptlosig = NULL;
   locon_list *ptlocon = NULL;
   chain_list *ptchain;
   chain_list *loconchain, *gridchain;
   lotrs_list **gridtable;
   ptype_list *ptuser;
   long numparallel = 0L;
   long signature = 0L;
   int numgrids = 0;
   int parallel = 0;
   int transtodel;
   int i, j;

   lofigchain (ptlofig);
   for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
      gridchain = NULL;
      numgrids = 0;
      loconchain = (chain_list *) getptype (ptlosig->USER, LOFIGCHAIN)->DATA;
      for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
         ptlocon = (locon_list *) ptchain->DATA;
         if (ptlocon->TYPE == 'T' && ptlocon->NAME == MBK_GRID_NAME) {
            gridchain = addchain (gridchain, ptlocon->ROOT);
            numgrids++;
         }
      }
      if (numgrids > 0)
         gridtable = (lotrs_list **) mbkalloc (numgrids * sizeof (lotrs_list *));
      i = 0;
      for (ptchain = gridchain; ptchain; ptchain = ptchain->NEXT) {
         pttrans = (lotrs_list *) ptchain->DATA;
         gridtable[i++] = pttrans;
      }
      if (numgrids > 2)
         qsort (gridtable, numgrids, sizeof (lotrs_list *), mbkTransCompare);
      i = 0;
      for (i = 0; i < numgrids; i++) {
         pttrans = gridtable[i];
         if (getptype (pttrans->USER, MBK_TRANS_MARK) != NULL)
            continue;
         signature = (long)pttrans->SOURCE->SIG + (long)pttrans->DRAIN->SIG;
         for (j = i + 1; j < numgrids; j++) {
            parallel = 0;
            pt2trans = gridtable[j];
            if (signature != (long)pt2trans->SOURCE->SIG + (long)pt2trans->DRAIN->SIG)
               break;
            if ((pt2trans->DRAIN->SIG == pttrans->DRAIN->SIG)
               && (pt2trans->TYPE == pttrans->TYPE)
               && (pt2trans->SOURCE->SIG == pttrans->SOURCE->SIG)
               && mbkSameBBox(pt2trans, pttrans)) {
               /* transistors in parallel */
               parallel = 1;
            }
            else if ((pt2trans->SOURCE->SIG == pttrans->DRAIN->SIG)
                   && (pt2trans->TYPE == pttrans->TYPE)
                   && (pt2trans->DRAIN->SIG == pttrans->SOURCE->SIG)
                   && mbkSameBBox(pt2trans, pttrans)) {
               /* transistors in parallel */
               parallel = 1;
            }
            if (parallel) {
               merge_trans (pttrans, pt2trans);
               numparallel++;
               if (remove) {
                  pt2trans->USER = addptype (pt2trans->USER, MBK_TRANS_MARK, 0);
               }
            }
         }
      }
      if (gridchain != NULL)
         freechain (gridchain);
      if (numgrids > 0)
         mbkfree (gridtable);
   }
   if (numparallel > 0) {
      for (pttrans = ptlofig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
         if ((ptuser = getptype (pttrans->USER, MBK_TRANS_PARALLEL)) != NULL
            && getptype (pttrans->USER, MBK_TRANS_MARK) == NULL) {
            /* make sure that non-instance is at beginning of list */
            ptuser->DATA = reverse((chain_list *)ptuser->DATA);
         }
      }
   }

   if (remove) {
      ptprevtrans = NULL;
      transtodel = 0;
      for (pttrans = ptlofig->LOTRS; pttrans; pttrans = ptnexttrans) {
         ptnexttrans = pttrans->NEXT;
         if (getptype (pttrans->USER, MBK_TRANS_MARK) != NULL) {
            if (ptprevtrans == NULL) {
               ptlofig->LOTRS = ptnexttrans;
            }
            else
               ptprevtrans->NEXT = ptnexttrans;
            transtodel = 1;
         }
         else
            ptprevtrans = pttrans;
      }

      if (transtodel)
         lofigchain (ptlofig);
   }

   return numparallel;
}

/****************************************************************************
 *                         function mbk_restoreparallel();                  *
 ****************************************************************************/
void mbk_restoreparallel (lofig_list * ptlofig)
{
   lotrs_list *pttrans;
   lotrs_list *ptparatrans;
   ptype_list *ptuser;
   chain_list *transchain;
   chain_list *ptchain;
   int changed = 0;

   for (pttrans = ptlofig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
      if ((ptuser = getptype (pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
         transchain = (chain_list *) ptuser->DATA;
         for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
            ptparatrans = (lotrs_list *) ptchain->DATA;
            if (ptparatrans == pttrans)
               continue;
            /* if not marked then already restored */
            if (getptype (ptparatrans->USER, MBK_TRANS_MARK) == NULL)
               return;
            ptparatrans->USER = delptype (ptparatrans->USER, MBK_TRANS_MARK);
            changed = 1;
            ptparatrans->NEXT = ptlofig->LOTRS;
            ptlofig->LOTRS = ptparatrans;
         }
      }
   }
   if (changed)
      lofigchain (ptlofig);
}

/****************************************************************************
 *                         function mbk_markparallel();                     *
 ****************************************************************************/
void mbk_markparallel (lofig_list * ptlofig)
{
   lotrs_list *pttrans;
   lotrs_list *ptparatrans;
   ptype_list *ptuser;
   chain_list *transchain;
   chain_list *ptchain;

   for (pttrans = ptlofig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
      if (getptype (pttrans->USER, MBK_TRANS_MARK) != NULL) continue;
      if ((ptuser = getptype (pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
         transchain = (chain_list *) ptuser->DATA;
         for (ptchain = transchain->NEXT; ptchain; ptchain = ptchain->NEXT) {
            ptparatrans = (lotrs_list *) ptchain->DATA;
            ptparatrans->USER = addptype (ptparatrans->USER, MBK_TRANS_MARK, 0);
         }
      }
   }
}

/****************************************************************************
 *                         function mbk_unmarkparallel();                   *
 ****************************************************************************/
void mbk_unmarkparallel (lofig_list * ptlofig)
{
   lotrs_list *pttrans;

   for (pttrans = ptlofig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
      if (getptype (pttrans->USER, MBK_TRANS_MARK) != NULL) {
         pttrans->USER = delptype (pttrans->USER, MBK_TRANS_MARK);
      }
   }
}

/****************************************************************************
 *                         function mbk_freeparallel();                     *
 ****************************************************************************/
void mbk_freeparallel (lofig_list * ptlofig)
{
   lotrs_list *pttrans;
   lotrs_list *ptparatrans;
   ptype_list *ptuser;
   chain_list *transchain;
   chain_list *ptchain;

   for (pttrans = ptlofig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
      if ((ptuser = getptype (pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
         transchain = (chain_list *) ptuser->DATA;
         for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
            ptparatrans = (lotrs_list *) ptchain->DATA;
            ptparatrans->USER=testanddelptype(ptparatrans->USER, MBK_TRANS_PARALLEL);
//            ptparatrans->USER = delptype (ptparatrans->USER, MBK_TRANS_PARALLEL);
         }
         freechain (transchain);
      }
   }
}

int mbk_checklofig (lofig_list * fig)
{
   ht *trans_ht;
   int fig_ok = 1;
   lotrs_list *tr;

   trans_ht = addht (101);

   for (tr = fig->LOTRS; tr; tr = tr->NEXT) {
      if (gethtitem (trans_ht, tr->TRNAME) == EMPTYHT)
         addhtitem (trans_ht, tr->TRNAME, 0);
      else {
         avt_errmsg(MBK_ERRMSG, "018", AVT_ERROR, fig->NAME, tr->TRNAME);
         fig_ok = 0;
         break;
      }
   }

   delht (trans_ht);

   return fig_ok;
}

char *mbk_putconname(locon_list *lc, char *buf)
{
  switch(lc->TYPE)
    {
    case 'E': strcpy(buf, lc->NAME); break;
    case 'T': sprintf(buf, "%s.%s", ((lotrs_list *)lc->ROOT)->TRNAME, lc->NAME); break;
    case 'I': sprintf(buf, "%s.%s", ((loins_list *)lc->ROOT)->INSNAME, lc->NAME); break;
    default:
      sprintf(buf, "?%s", lc->NAME);
    }
  return buf;
}

int mbk_GetPowerSupply (lofig_list *lofig, float *supply)
{
    losig_list *losig;
    float alim, alim_max = 0;
    int losigalim = 0;

    for (losig = lofig->LOSIG; losig; losig = losig->NEXT)
        if (getlosigalim (losig, &alim)) { 
            alim_max = alim_max > alim? alim_max: alim;
            losigalim = 1;
        }

    if (losigalim) *supply = alim_max;

    return losigalim;
}

char *mbk_get_subckt_name(lotrs_list *tr)
{
  char *subcktname=NULL, *modelname;
  ptype_list *ptype;

  if(tr->MODINDEX < 0) return NULL;

  if (( ptype = getptype ( tr->USER, TRANS_FIGURE )))
    subcktname = (char*)ptype->DATA;
  else 
  {
    modelname=getlotrsmodel(tr);
    subcktname = mcc_getsubckt( MCC_MODELFILE, modelname, 
                                MLO_IS_TRANSN(tr->TYPE)?MCC_NMOS:MCC_PMOS,
                                MCC_TYPICAL, ((float)tr->LENGTH/SCALE_X)*1.0e-6,
                                ((float)tr->WIDTH/SCALE_X)*1.0e-6);
  }
  return subcktname;
}

char *mbk_LosigTestREGEX(losig_list *testlosig, char *refname)
{
    chain_list *ptchain;
    
    for (ptchain = testlosig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
        if (mbk_TestREGEX((char *)ptchain->DATA, refname)) return (char *)ptchain->DATA;
    }
    return NULL;
}


static unsigned int mbk_montecarlo_global_seed=0, displayseed=1, mbk_montecarlo_global_seed_saved=0;

unsigned int mbk_get_montecarlo_global_seed(int reset)
{
  if (reset) mbk_montecarlo_global_seed=0;
  if (mbk_montecarlo_global_seed==0)
  {          
    mbk_montecarlo_global_seed=mbk_get_a_seed();
    mbk_montecarlo_global_seed_saved=mbk_montecarlo_global_seed;
    displayseed=1;
  }
  if (displayseed)
  {
    if (V_BOOL_TAB[ __AVT_ENABLE_STAT ].VALUE)
    {
      char buf[256];
      sprintf(buf, "info:Global monte-carlo seed = %u", mbk_montecarlo_global_seed);
      avt_log(-1, 0, "%s\n", &buf[5]);
      if (MBK_SOCK!=-1)
        mbk_sendmsg( MBK_COM_DATA, buf, strlen(buf)+1);
    }
    displayseed=0;
  }
  return  mbk_montecarlo_global_seed;
}

void mbk_set_montecarlo_global_seed(unsigned int value)
{
  mbk_montecarlo_global_seed=value;
}
void mbk_init_montecarlo_global_seed(unsigned int value)
{
  mbk_montecarlo_global_seed=value;
  mbk_montecarlo_global_seed_saved=mbk_montecarlo_global_seed;
  displayseed=1;
}
unsigned int mbk_get_montecarlo_global_seed_init_saved()
{
  return mbk_montecarlo_global_seed_saved;
}

static void mbk_recur_Eval_PrintError(char *var, char *expr, chain_list *cl, char *where)
{
  chain_list *ch;
  int nb;
  char buf[2048];

  nb=countchain(cl);
  
  strcpy(buf,"");
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      if (ch!=cl)
      {
        if (ch->NEXT==NULL)
         strcat(buf,"and ");
        else
         strcat(buf,", ");
      }
      strcat(buf,"'");
      strcat(buf,(char *)ch->DATA);
      strcat(buf,"'");
    }

  avt_errmsg(MBK_ERRMSG, "052", AVT_ERROR, var, expr, where, nb>0?"s":"", buf, nb>0?"are":"is");
}

int mbk_recur_Eval(eqt_ctx *ctx, ht *allparams, char *name, char *where)
{
  char *expr;
  chain_list *support;
  long l;
  double value;
  if ((l=gethtitem(allparams, name))!=EMPTYHT)
    {
      expr=(char *)l;
      delhtitem(allparams, name);
      support=eqt_GetVariables(ctx, expr, 1);
      while (support!=NULL)
        {
          mbk_recur_Eval(ctx, allparams, (char *)support->DATA, where);
          support=delchain(support, support);
        }
      value=eqt_eval(ctx, expr, EQTFAST);
      if (!eqt_resistrue(ctx))
        {
          support=eqt_GetVariables(ctx, expr, 0);
          mbk_recur_Eval_PrintError(name, expr, support, where);
          freechain(support);
//          avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, name, expr," : set to 0");
        }
      else if (!finite(value))
        avt_errmsg(MBK_ERRMSG, "051", AVT_ERROR, name, expr," : returned NaN or Inf");
      eqt_addvar(ctx, name, value);
      return 0;
    }

  return 1;
}

void mbk_recur_Eval_Start(eqt_ctx *ctx, ht *allparams, char *where)
{
  chain_list *cl;
  cl=GetAllHTKeys(allparams);
  while (cl!=NULL)
   {
     mbk_recur_Eval(ctx, allparams, (char *)cl->DATA, where);
     cl=delchain(cl, cl);
   }  
}

void mbk_EvalGlobalMonteCarloDistributions(eqt_ctx *ctx)
{
  ptype_list *pt;
  ht *pht;
  chain_list *cl;
  unsigned int oldseed;

  oldseed=eqt_get_current_srand_seed();
  eqt_srand(mbk_get_montecarlo_global_seed(0));
  pht=addht(100);
  for (pt=MBK_MONTE_DIST; pt!=NULL; pt=pt->NEXT)
    addhtitem(pht, (void *)pt->TYPE, (long)pt->DATA);
  for (pt=MBK_MONTE_PARAMS; pt!=NULL; pt=pt->NEXT)
    addhtitem(pht, (void *)pt->TYPE, (long)pt->DATA);

  cl=GetAllHTKeys(pht);
  while (cl!=NULL)
    {
      mbk_recur_Eval(ctx, pht, (char *)cl->DATA, "monte-carlo");
      cl=delchain(cl, cl);
    }
  delht(pht);
  eqt_srand(oldseed);
}

void mbk_set_transistor_instance_connector_orientation_info (lofig_list * lf)
{
    locon_list *lc, *lc0;
    chain_list *cl;
    ptype_list *pt;
    long flag;

    for (lc = lf->LOCON; lc != NULL; lc = lc->NEXT) {
        if (lc->DIRECTION == IN)
            flag = LOCON_INFORMATION_INPUT;
        else if (lc->DIRECTION == OUT || lc->DIRECTION == INOUT || lc->DIRECTION == TRISTATE)
            flag = LOCON_INFORMATION_OUTPUT;
        else
            flag = 0;

        if (flag != 0) {
            if ((pt = getptype (lc->SIG->USER, LOFIGCHAIN)) != NULL) {
                for (cl = (chain_list *) pt->DATA; cl != NULL; cl = cl->NEXT) {
                    lc0 = (locon_list *) cl->DATA;
                    if (lc0->TYPE=='I' || (lc0->NAME != MBK_GRID_NAME && lc0->NAME != MBK_BULK_NAME)) {
                        if ((pt = getptype (lc0->USER, LOCON_INFORMATION)) == NULL)
                            pt = lc0->USER = addptype (lc0->USER, LOCON_INFORMATION, 0);

                        if ((long)pt->DATA != 0 && (long)pt->DATA != flag)
                            printf ("conflicting orientation information on net '%s' in figure '%s'\n",
                                    getsigname (lc->SIG), lf->NAME);
                        else
                            pt->DATA = (void *)(long)flag;
                    }
                }
            }
        }
    }
}

#define COL_RESI_NAME 0
#define COL_RESI_VAL 1
#define COL_RESI_NODE 2
#define COL_VCARD_VALUE 3

void mbk_CheckPowerSupplies(lofig_list *lf)
{
  ptype_list *pt;
  chain_list *p1, *p2, *pt0, *root;
  chain_list *cl, *ch;
  num_list *nm;
  locon_list *lc;
  int i=0;
  char *name;
  losig_list *sig;
  lowire_list *lw;
  vcardnodeinfo *p1vci, *p2vci;

  for (sig=lf->LOSIG; sig; sig=sig->NEXT)
    {      
      if ((pt=getptype(sig->USER, MBK_VCARD_NODES))!=NULL)
        {
          root=(chain_list *)pt->DATA;
          if (root->NEXT!=NULL)
            {
              rcn_flush_signal(lf, sig );
              rcn_refresh_signal( lf, sig );
              pt0=dupchainlst(root);
              for (p1=pt0; p1; p1=p1->NEXT)
                {
                  for (p2=pt0; p2; p2=p2->NEXT)
                    {
                      if (p1!=p2 && p1->DATA!=NULL && p2->DATA!=NULL)
                        {
                          long a, nb;
                          char buf[512];
                          p1vci=(vcardnodeinfo *)p1->DATA;
                          p2vci=(vcardnodeinfo *)p2->DATA;
                          if (p1vci->value!=p2vci->value)
                            cl=rcn_get_a_r_way(sig, p1vci->rcn, p2vci->rcn);
                          else
                            cl=NULL;
                          if (cl!=NULL)
                            {
                              Board *b;
                              b=Board_CreateBoard();
                              Board_SetSize(b, COL_RESI_NAME, 20, 'l');
                              Board_SetSize(b, COL_RESI_VAL, 10, 'r');
                              Board_SetSize(b, COL_RESI_NODE, 10, 'l');
                              Board_SetSize(b, COL_VCARD_VALUE, 10, 'l');
                              Board_NewLine(b);
                              Board_SetValue(b, COL_RESI_NAME, "Resistor Name");
                              Board_SetValue(b, COL_RESI_VAL, "Resistor Value");
                              Board_SetValue(b, COL_RESI_NODE, "Node");
                              Board_SetValue(b, COL_VCARD_VALUE, "Voltage");
                              Board_NewSeparation(b);
                              Board_NewLine(b);
                              a=p1vci->rcn;
                              sprintf(buf,"[%ld] %s", a, p1vci->name);
                              Board_SetValue(b, COL_RESI_NODE,  buf);
                              sprintf(buf,"%gv", p1vci->value);
                              Board_SetValue(b, COL_VCARD_VALUE,  buf);

                              while (cl!=NULL)
                                {
                                  lw=(lowire_list *)cl->DATA;
                                  if (lw->NODE1!=a) nb=lw->NODE1; else nb=lw->NODE2;

                                  Board_NewLine(b);

                                  if ((pt=getptype(lw->USER, RESINAME))!=NULL)
                                    Board_SetValue(b, COL_RESI_NAME,  (char *)pt->DATA);

                                  if (cl->NEXT==NULL)
                                    {
                                      sprintf(buf,"[%ld] %s", (long)p2vci->rcn, p2vci->name);
                                      Board_SetValue(b, COL_RESI_NODE,  buf);
                                      sprintf(buf,"%gv", p2vci->value);
                                      Board_SetValue(b, COL_VCARD_VALUE,  buf);
                                    }
                                  else
                                    {
                                      for (ch=root; ch!=NULL && ((vcardnodeinfo *)ch->DATA)->rcn!=nb; ch=ch->NEXT) ;
                                      if (ch!=NULL)
                                        {
                                          name=((vcardnodeinfo *)ch->DATA)->name;
                                          sprintf(buf,"%gv", ((vcardnodeinfo *)ch->DATA)->value);
                                          Board_SetValue(b, COL_VCARD_VALUE,  buf);
                                        }
                                      else
                                        name=NULL;
                                      sprintf(buf,"[%ld] %s", nb, name==NULL?"":name);
                                      Board_SetValue(b, COL_RESI_NODE,  buf);
                                    }
                                  sprintf(buf,"%.1f", lw->RESI);
                                  Board_SetValue(b, COL_RESI_VAL,  buf);
                                  a=nb;
                                  cl=delchain(cl, cl);
                                }
                              Board_NewSeparation(b);
                              avt_log(LOGERROR, 0, "*** Resistive path beetween connector '%s' (%gv) and '%s' (%gv)\n", p1vci->name, p1vci->value, p2vci->name, p2vci->value);
                              Board_Display_sub(NULL, LOGERROR, 0, b, "    ");
                              Board_FreeBoard(b);
              
//                              avt_fprintf(stdout, "\n");            
                              i++;
                            }
                        }
                    }
                  p1->DATA=NULL;
                }
              freechain(pt0);
            }
        }
    }
  if (i>0)
    {
      avt_errmsg(MBK_ERRMSG, "072", AVT_ERROR);
    }
}
