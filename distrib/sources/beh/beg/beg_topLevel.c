/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : beg_topLevel.c                                              */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "beg_topLevel.h"
#include "beg_gen.h"
#include "beg_gen.h"
#include BVL_H

static inline void eatAllInstances(befig_list *befig, loins_list *loins, ht *befight, ht *signalht);

static ht *CORRESP_HT_HT=NULL;

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    eatAllInstances()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void eatAllInstances(befig_list *befig, loins_list *loins,
                                   ht *befight, ht *signalht)
{
  befig_list    *figx;
  loins_list    *loinx;
  chain_list    *corlist;
  ht *ptx;

  corlist       = NULL;
  for (loinx = loins; loinx; loinx = loinx->NEXT)
  {
    figx        = (befig_list *)gethtitem(befight,loinx->INSNAME);
    if (figx != (void *)EMPTYHT)
    {
      // i change instance signal names
      beh_changeNameInterface(signalht,figx);
      // TODO correspondance table have to be updated

      ptx = begGetCorrespHT(figx->NAME);
      if (ptx!=NULL)
        {
          corlist         = addchain(corlist, ptx);
          begGetCorrespHTUnlink(figx->NAME);
        }
/*      if ((ptx = getptype(figx->USER,BEG_CORRESP_PTYPE)) != NULL)
      {
        corlist         = append(corlist,(chain_list *)ptx->DATA);
        figx->USER      = delptype(figx->USER,BEG_CORRESP_PTYPE);
      }*/
      beg_eat_figure(figx);
    }
  }
  if (corlist)
    befig->USER     = addptype(befig->USER,BEG_CORRESP_PTYPE,corlist);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void eatInstance(loins_list *loins, ht *corht, ht *fight, ht *sight)
{
  long           tmp;
  befig_list    *befig;

  // in this case corht is unused and should be NULL so
  corht         = NULL;
  
  if ((tmp = gethtitem(fight,loins->INSNAME)) != EMPTYHT)
  {
    befig       = (befig_list*)tmp;
    // i change instance signal names
    beh_changeNameInterface(sight,befig);
    // TODO correspondance table have to be updated
    beg_updateConsDir(befig);
    beg_eat_figure(befig);
  }
  beg_updateConsDir(NULL);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/* mode = 0 is at yagle level                                               */
/* mode = 1 is at genius api level                                          */
/*                                                                          */
/****************************************************************************/
void beg_addAllInstances(char *curbefig, befig_list *befig, loins_list *loins, ht *fight,
                         ht *sight, int mode,
                         void (*eatfunc)(loins_list*,ht*,ht*,ht*))
{
  loins_list    *loinsx;
  befig_list    *figx;
  char           buf[1024]; //, *curbefig;
  ht            *htloins, *corht, *tht;
  ptype_list    *ptx, head, *modh;
  chain_list    *chainx, *inslist, *modlist;
  chain_list    *corlist;
  long           tmp;
  int            i;

  //  curbefig          = befig->NAME;
  corlist           = NULL;
  
  corht=NULL;
  if ((mode & 1)==1)
    {
      if (mode & DISABLE_CREATE_COR)
        corht = begGetCorrespHT/*Create*/(curbefig/*befig->NAME*/);
      else
        corht = begGetCorrespHTCreate(curbefig/*befig->NAME*/);
    }
  /*
    )!=NULL getptype(befig->USER,BEG_CORRESP_PTYPE)))
    corht           = ptx; //((chain_list*)(ptx->DATA))->DATA;
    else
    corht           = NULL;
  */

  // get the a list of instance classed by model 
  htloins           = addht(32);
  for (loinsx = loins; loinsx; loinsx = loinsx->NEXT)
    {
      if ((mode & 1)==1 || (tmp = gethtitem(fight,loinsx->INSNAME)) != EMPTYHT)
        {
          modh          = (ptype_list *)gethtitem(htloins,loinsx->FIGNAME);
          if (modh == (ptype_list *)EMPTYHT)
            {
              modh        = addptype(NULL,0,NULL);
              addhtitem(htloins,loinsx->FIGNAME,(long)modh);
            }
          inslist       = modh->DATA;
          modh->DATA    = addchain(inslist,loinsx);
          (modh->TYPE) ++;
      
          if ((mode & 1)==0)
            {
              figx        = (befig_list*)tmp;
              if ((tht=begGetCorrespHT(figx->NAME))!=NULL)
                {
                  corlist         = addchain(corlist, tht);
                  begGetCorrespHTUnlink(figx->NAME);
                }
              /*
                if ((ptx = getptype(figx->USER,BEG_CORRESP_PTYPE)) != NULL)
                {
                corlist         = append(corlist,(chain_list *)ptx->DATA);
                figx->USER      = delptype(figx->USER,BEG_CORRESP_PTYPE);
                }
              */
            }
        }
    }
  // get list of models
  modlist           = GetAllHTElems(htloins);
  delht(htloins);
  
  // go trough the list of models to sort by instanciation count
  head.NEXT         = NULL;
  for ( i = 0; modlist; modlist = delchain(modlist,modlist), i ++)
    {
      modh            = modlist->DATA;
      if (!head.NEXT)
        head.NEXT     = modh;
      else
        {
          for (ptx = &head; modh->TYPE <= ptx->NEXT->TYPE; ptx = ptx->NEXT)
            if (!ptx->NEXT->NEXT)
              {
                ptx       = ptx->NEXT;
                break;
              }
          modh->NEXT    = ptx->NEXT;
          ptx->NEXT     = modh;
        }
      // on the same time replace list of instance by one 
      // behavioral figure containing all of this model instanciations
      inslist         = reverse(modh->DATA);
      sprintf(buf,"tmp_figure_for_beg%d",i);
      // TODO perhaps find something else about namealloc
      if ((mode & DISABLE_CREATE_BEH)==0)
        {
          beg_def_befig(namealloc(buf));
          beg_add_pors(befig);
        }

      for (chainx = inslist; chainx; chainx = delchain(chainx,chainx))
        {
          loinsx        = chainx->DATA;
          eatfunc(loinsx,corht,fight,sight);
        }
      
      if ((mode & DISABLE_CREATE_BEH)==0)
        {

          beg_sort();
          if ((mode & DISABLE_COMPACT)==0)
            beg_compact();
          modh->DATA      = beg_get_befig(BEG_NOVERIF);
        }
    }

  // add all the temporally behavioral figures
  if ((mode & DISABLE_CREATE_BEH)==0)
    {
      beg_def_befig(befig->NAME);
      for (ptx = head.NEXT; ptx; ptx = delptype(ptx,ptx->TYPE))
        beg_eat_figure(ptx->DATA);
    }

  if ((mode & 1)==0 && corlist)
    befig->USER     = addptype(befig->USER,BEG_CORRESP_PTYPE,corlist);
}

/*}}}************************************************************************/
/*{{{                    beg_topLevel()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

static ht *CORRESPHT=NULL, *VECTORSHT=NULL;

static char *
begNameWrapSuppressEXT(char *name)
{
  long l;
  if (strncasecmp(name,"ext@",4)==0) return namealloc(&name[4]);
  if (CORRESPHT!=NULL &&
      gethtitem(VECTORSHT, vectorradical(name))==EMPTYHT &&
      (l=gethtitem(CORRESPHT, name))!=EMPTYHT && vectorindex((char *)l)==-1)
    return (char *)l;
  return name;
}

static ht *begvectorsht(befig_list *bf)
{
  bevectreg_list *vreg;
  bevectbux_list *vbux;
  bevectpor_list *vpor;
  bevectaux_list *vaux;
  bevectout_list *vout;
  bevectbus_list *vbus;
  berin_list *rin;
  ht *h;

  h=addht(1000);

  for (vpor=bf->BEVECTPOR; vpor!=NULL; vpor=vpor->NEXT)
    addhtitem(h, vpor->NAME, 0);
  for (vreg=bf->BEVECTREG; vreg!=NULL; vreg=vreg->NEXT)
    addhtitem(h,  vreg->NAME, 0);
  for (vaux=bf->BEVECTAUX; vaux!=NULL; vaux=vaux->NEXT)
    addhtitem(h,  vaux->NAME, 0);
  for (vout=bf->BEVECTOUT; vout!=NULL; vout=vout->NEXT)
    addhtitem(h,  vout->NAME, 0);
  for (vbux=bf->BEVECTBUX; vbux!=NULL; vbux=vbux->NEXT)
    addhtitem(h,  vbux->NAME, 0);
  for (vbus=bf->BEVECTBUS; vbus!=NULL; vbus=vbus->NEXT)
    addhtitem(h,  vbus->NAME, 0);
  for (rin = bf->BERIN; rin; rin = rin->NEXT)
  {
    if (vectorindex(rin->NAME)!=-1)
      addhtitem(h,  vectorradical(rin->NAME), 0);
  }

  return h;
}

static void begbuildmorealias(befig_list *bf, ht *morealiases)
{
  ptype_list *pt;
  if ((pt=getptype(bf->USER,BEG_RENAME_LIST))!=NULL)
   {
      long l;
      chain_list *cl;
      pt=(ptype_list *)pt->DATA;
      while (pt!=NULL)
      {
        if ((l=gethtitem(morealiases, (void *)pt->DATA))!=EMPTYHT)
          cl=(chain_list *)l;
        else
          cl=NULL;
        addhtitem(morealiases, pt->DATA, (long)addchain(cl, (void *)pt->TYPE));
        pt=pt->NEXT;
      }
   }
}

void beg_topLevel(befig_list **befig, chain_list *insbefigs, int compact,
                  loins_list *loins, ht **renamed, ht **morealiases)
{
  ht            *befight, *signalht;
  befig_list    *figx, *yagfig, *topfig;
  loins_list    *loinx;
  chain_list    *chainx;
  ptype_list *ptuser;
  int mode=0;
  char *figname;

  yagfig            = *befig;
  figname=yagfig->NAME;
  beg_def_befig(V_BOOL_TAB[__API_USE_CORRESP].VALUE?namealloc("temp_to_rename"):yagfig->NAME);
  befight           = addht(40);
  
  for (chainx = insbefigs; chainx; chainx = chainx->NEXT)
  {
    figx            = (befig_list *)chainx->DATA;
    addhtitem(befight,figx->NAME,(long)figx);
  }

  // pre-treatment of cnsfig to avoid signal problem
  // this signal problem occurs when genius has several top-level instances :
  // each genius instances has a signal name which could be different on other instances
  // and need is to have a single name for all instances
  //   .. anto ..
  signalht          = NULL;
  *morealiases = NULL;
  for (loinx = loins; loinx; loinx = loinx->NEXT)
  {
    figx            = (befig_list *)gethtitem(befight,loinx->INSNAME);
    if (figx != (void *)EMPTYHT)
     {
      if (*morealiases==NULL) *morealiases=addht(1000);
      begbuildmorealias(figx, *morealiases);
      beh_addRefName(&signalht,figx);
     }
  }
  // i have a table of correspondance between circuit signals names and 
  // all instances signals names
  // i have to create a single name TODO for it i use the defined strategy,
  // by default i take the first instance signal name
  // here NULL correspond to the function
  beh_createName(signalht,NULL);
  // i change the circuit signals name by the new name
  // i rechange also signals which belong to the main inteface
  beh_changeNameInside(signalht,yagfig);

  // carefull, don't want to free the yagfig->NAME env
  beg_eat_por(yagfig);
  yagfig->NAME      = NULL;
  beg_eat_figure(yagfig);
  
  if (compact)
  {
    beg_sort();
    beg_compact();
  }
  else mode=DISABLE_COMPACT;
  
  topfig            = beg_get_befig(BEG_NOVERIF);
  //eatAllInstances(*befig,loins,befight,signalht);
  beg_addAllInstances(topfig->NAME, topfig,loins,befight,signalht,mode,eatInstance);
  
  topfig            = beg_get_befig(BEG_NOVERIF);
  beg_forceZSort();
  
  *befig            = topfig;
  
  delht(befight);
//  delht(signalht);
  // cleaning the BEG structures
  beg_terminate(topfig);

  if (V_BOOL_TAB[__API_USE_CORRESP].VALUE)
  {
    char *tmp=topfig->NAME;
    topfig->NAME=figname;
    CORRESPHT=bvl_buildfinalcorrespht(topfig);
    VECTORSHT=begvectorsht(topfig);
    topfig->NAME=tmp;
  }
  beh_namewrap(topfig,begNameWrapSuppressEXT);
  if (CORRESPHT!=NULL)
  {
    delht(VECTORSHT);
    delht(CORRESPHT);
    CORRESPHT=NULL;
    beg_def_befig(figname);
    beg_eat_por(topfig);
    topfig->NAME      = NULL;
    beg_eat_figure(topfig);
    topfig = beg_get_befig(BEG_NOVERIF);
    beg_terminate(topfig);
    *befig = topfig;
  }
  *renamed=signalht;
}

/*}}}************************************************************************/
/*}}}************************************************************************/

ht *begGetCorrespHTCreate(char *name)
{
  ht            *htb;
  long l;

  if (CORRESP_HT_HT==NULL) CORRESP_HT_HT=addht(100000);

  if ((l=gethtitem(CORRESP_HT_HT, name))==EMPTYHT)
  {
    htb         = addht(BEGAPI_HT_BASE);
//    ptype       = addptype(befig->USER,BEG_CORRESP_PTYPE,addchain(NULL,htb));
//    befig->USER = ptype;
    addhtitem(CORRESP_HT_HT, name, (long)htb);
  }
  else
    // htb         = ((chain_list*)ptype->DATA)->DATA;
    htb=(ht *)l;

  return htb;
}

void begGetCorrespHTUnlink(char *name)
{
  delhtitem(CORRESP_HT_HT, name);
}
void begGetCorrespHTRelink(char *name, ht *newc)
{
  addhtitem(CORRESP_HT_HT, name, (long)newc);
}
void begFreeCorrespHT(befig_list *bf, char *name)
{
  ht *sr;
  ptype_list *pt;
  chain_list *cl;
  if ((sr=begGetCorrespHT(name))!=NULL)
  {
     begGetCorrespHTUnlink(name);
     mbv_freeht(sr);
  }
  if (bf!=NULL && (pt=getptype(bf->USER,BEG_CORRESP_PTYPE))!=NULL)
  {
    for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
    {
      mbv_freeht((ht *)cl->DATA);
    }
    freechain(pt->DATA);
    bf->USER=delptype(bf->USER, BEG_CORRESP_PTYPE);
  }
}

/*}}}************************************************************************/
/*{{{                    begGetCorrespHT()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
ht *begGetCorrespHT(char *name)
{
  ht            *htb;
  long l;

  if (CORRESP_HT_HT==NULL) return NULL;

  if ((l=gethtitem(CORRESP_HT_HT, name))!=EMPTYHT)
    htb=(ht *)l;
  else
    htb=NULL;
/*
  if ((ptype = getptype(befig->USER,BEG_CORRESP_PTYPE)))
    htb     = ((chain_list*)ptype->DATA)->DATA;
  else
    htb     = NULL;
*/
  return htb;
}
