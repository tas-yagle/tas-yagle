/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_debug.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_debug.h"
#include "stb_transfer.h"
#include "stb_error.h"
#include "stb_clocks.h"
#include "stb_clock_tree.h"
#include "stb_directives.h"
#include "stb_falseslack.h"

/*****************************************************************************
*                           fonction stb_compstbdebug()                      *
*****************************************************************************/
int stb_compstbdebug(debug1,debug2)
stbdebug_list **debug1 ;
stbdebug_list **debug2 ;
{
 long max1 = STB_NO_TIME  ;
 long max2 = STB_NO_TIME  ;

 if((*debug1)->SETUP < max1)
   max1 = (*debug1)->SETUP ;

 if((*debug1)->HOLD < max1)
   max1 = (*debug1)->HOLD ;

 if((*debug2)->SETUP < max2)
   max2 = (*debug2)->SETUP ;

 if((*debug2)->HOLD < max2)
   max2 = (*debug2)->HOLD ;

 if (max1>max2) return 1;
 if (max1<max2) return -1;
 return 0;
// return((int)(max1-max2)) ;
}

/*****************************************************************************
*                           fonction stb_sortstbdebug()                      *
*****************************************************************************/
stbdebug_list *stb_sortstbdebug(debug)
stbdebug_list *debug ;
{
 stbdebug_list **debugtab ;
 stbdebug_list *ptdebug ;
 int nb = 0 ;
 int i ;

 if(debug == NULL)
   return(NULL) ;

 for(ptdebug = debug ; ptdebug ; ptdebug = ptdebug->NEXT)
    nb++ ;

 debugtab = (stbdebug_list**)mbkalloc(nb * sizeof(stbdebug_list *)) ;

 ptdebug = debug  ;

 for(i = 0  ; i < nb  ; i ++)
  {
   *(debugtab + i) = ptdebug  ;
   ptdebug = ptdebug->NEXT  ;
  }

 qsort(debugtab,nb,sizeof(stbdebug_list *),
       (int (*)(const void*,const void*))stb_compstbdebug) ;

 debug = *debugtab ;
 ptdebug = debug ;

 for(i = 1  ; i < nb  ; i ++)
  {
   ptdebug->NEXT = *(debugtab + i) ;
   ptdebug = ptdebug->NEXT  ;
  }

 ptdebug->NEXT = NULL ;

 mbkfree(debugtab) ;

 return(debug) ;
}

/*****************************************************************************
*                           fonction stb_getstbdelay()                       *
*****************************************************************************/
stbdelay_list *stb_getstbdelay(ttvevent_list *root,ttvevent_list *node,ttvevent_list *cmd,long type,ttvevent_list *accesscmd, ttvevent_list *clock)
{
 ptype_list *ptype ;
 stbdelay_list *delay ;
 chain_list *chain ;

 if(node == NULL)
  {
   if((ptype = getptype(root->USER,STB_NODE_DELAY)) != NULL)
    {
     for(delay = (stbdelay_list *)ptype->DATA ; delay != NULL ; 
         delay = delay->NEXT)
       if((cmd == NULL) || (cmd == delay->CMD))
          return(delay) ;
     return(NULL) ;
    }
   else
     return(NULL) ;
  }

 if(((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) && 
    ((type & TTV_FIND_HZ) == TTV_FIND_HZ))
  {
   if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    {
     ptype = getptype(node->USER,STB_NODE_HZUP) ;
    }
   else
    {
     ptype = getptype(node->USER,STB_NODE_HZDN) ;
    }
  }
 else
  {
   if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    {
     ptype = getptype(node->USER,STB_NODE_DELAYUP) ;
    }
   else
    {
     ptype = getptype(node->USER,STB_NODE_DELAYDN) ;
    }
  }

 if(ptype == NULL)
  return(NULL) ;
 else
  {
   stbdelay_list *sd;
   for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
      {
       sd=(stbdelay_list *)chain->DATA;
       if(sd->ACCESSCMD==accesscmd && sd->CLOCK==clock && (sd->CMD == cmd || cmd == NULL))
         return((stbdelay_list *)chain->DATA) ;
      }
   return(NULL) ;
  }
}

/*****************************************************************************
*                           fonction stb_delstbdelay()                       *
*****************************************************************************/
void stb_delstbdelay(root)
ttvevent_list *root ;
{
 ttvevent_list *node ;
 ptype_list *ptype ;
 stbdelay_list *pt ;
 stbdelay_list *pts ;

 if((ptype = getptype(root->USER,STB_NODE_DELAY)) == NULL)
   return ;
 else {
   pts = (stbdelay_list *)ptype->DATA ;
   root->USER = delptype(root->USER,STB_NODE_DELAY) ;
 }

 for(pt = pts ; pt != NULL ; pt = pt->NEXT)
   {
    node = pt->NODE ;

    if(((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) && 
       ((pt->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ))
     {
      if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
       {
        if((ptype = getptype(node->USER,STB_NODE_HZUP)) != NULL)
         {
          freechain((chain_list *)ptype->DATA) ;
          node->USER = delptype(node->USER,STB_NODE_HZUP) ;
         }
       }
      else
       {
        if((ptype = getptype(node->USER,STB_NODE_HZDN)) != NULL)
         {
          freechain((chain_list *)ptype->DATA) ;
          node->USER = delptype(node->USER,STB_NODE_HZDN) ;
         }
       }
     }
    else
     {
      if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
       {
        if((ptype = getptype(node->USER,STB_NODE_DELAYUP)) != NULL)
         {
          freechain((chain_list *)ptype->DATA) ;
          node->USER = delptype(node->USER,STB_NODE_DELAYUP) ;
         }
       }
      else
       {
        if((ptype = getptype(node->USER,STB_NODE_DELAYDN)) != NULL)
         {
          freechain((chain_list *)ptype->DATA) ;
          node->USER = delptype(node->USER,STB_NODE_DELAYDN) ;
         }
       }
     }
   }

  pt = pts ;

  while(pt != NULL)
  {
   pts = pts->NEXT ;
   if(pt->PHASELISTMAX != NULL)
     freeptype(pt->PHASELISTMAX) ;
   if(pt->PHASELISTMIN != NULL)
     freeptype(pt->PHASELISTMIN) ;
   freeptype(pt->STARTMIN_PP);
   freeptype(pt->STARTMAX_PP);
   mbkfree(pt) ;
   pt = pts ;
  }
}

/*****************************************************************************
*                           fonction stb_addstbdelay()                       *
*****************************************************************************/
stbdelay_list *stb_addstbdelay(ttvevent_list *root,ttvevent_list *node,long delaymax,long delaymin,
                               long minstart,long maxstart,long type,ttvevent_list *cmd,ptype_list *lmax,
                               ptype_list *lmin,ttvevent_list *accesscmd, ttvevent_list *clock)
{
 stbdelay_list *pt ;
 ptype_list *ptype ;
 chain_list *chain ;
 ptype_list *ptypemax = NULL ;
 ptype_list *ptypemin = NULL ;

 for(;lmax != NULL ; lmax = lmax->NEXT)
     ptypemax = addptype(ptypemax,lmax->TYPE,lmax->DATA) ;

 for(;lmin != NULL ; lmin = lmin->NEXT)
     ptypemin = addptype(ptypemin,lmin->TYPE,lmin->DATA) ;

 if(ptypemax != NULL)
     ptypemax = (ptype_list *)reverse((chain_list *)ptypemax) ;

 if(ptypemin != NULL)
     ptypemin = (ptype_list *)reverse((chain_list *)ptypemin) ;

 if((pt = stb_getstbdelay(root,node,cmd,type,accesscmd,clock)) == NULL)
  {
   pt = (stbdelay_list *)mbkalloc(sizeof(stbdelay_list)) ;
   pt->DELAYMAX = TTV_NOTIME ;
   pt->DELAYMIN = TTV_NOTIME ;
   pt->PHASELISTMAX = ptypemax ;
   pt->PHASELISTMIN = ptypemin ;
   pt->MINSTART = TTV_NOTIME ;
   pt->STARTMIN_PP=pt->STARTMAX_PP=NULL;
  }
 else
   {
    if(delaymax != TTV_NOTIME)
       {
        if((delaymax > pt->DELAYMAX) || (pt->DELAYMAX == TTV_NOTIME))
         {
          pt->DELAYMAX = delaymax ;
          if(pt->PHASELISTMAX != NULL)
            freeptype(pt->PHASELISTMAX) ;
          pt->PHASELISTMAX = ptypemax ;
          pt->MAXSTART = maxstart ;
          freeptype(pt->STARTMAX_PP);
          pt->STARTMAX_PP=NULL;
         }
       }
    if(delaymin != TTV_NOTIME)
       {
        if((delaymin < pt->DELAYMIN) || (pt->DELAYMIN == TTV_NOTIME))
         {
          pt->DELAYMIN = delaymin ;
          if(pt->PHASELISTMIN != NULL)
            freeptype(pt->PHASELISTMIN) ;
          pt->PHASELISTMIN = ptypemin  ;
          pt->MINSTART = minstart ;
          freeptype(pt->STARTMIN_PP);
          pt->STARTMIN_PP=NULL;
         }
       }
    return(pt) ;
   }

 pt->NODE = node ;
 pt->CMD = cmd ;
 pt->CLOCK = clock ;
 pt->ACCESSCMD = accesscmd ;
 if(delaymax != TTV_NOTIME)
   pt->DELAYMAX = delaymax ;
 if(delaymin != TTV_NOTIME)
   pt->DELAYMIN = delaymin ;
 pt->TYPE = type ;
 pt->MINSTART = minstart ;
 pt->MAXSTART = maxstart ;

 if((ptype = getptype(root->USER,STB_NODE_DELAY)) == NULL)
  {
   pt->NEXT = NULL ;
   root->USER = addptype(root->USER,STB_NODE_DELAY,pt) ;
  }
 else
  {
   pt->NEXT = (stbdelay_list *)ptype->DATA ;
   ptype->DATA = (void *)pt ;
  }

 if(((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) && 
    ((type & TTV_FIND_HZ) == TTV_FIND_HZ))
  {
   if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    {
     if((ptype = getptype(node->USER,STB_NODE_HZUP)) == NULL)
       {
        node->USER = addptype(node->USER,STB_NODE_HZUP,addchain(NULL,pt)) ;
       }
      else
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
            chain = chain->NEXT)
          {
           if((((stbdelay_list *)chain->DATA)->CMD == cmd) || (cmd == NULL))
             break ;
          }
        if(chain == NULL)
           ptype->DATA = addchain((chain_list *)ptype->DATA,(void *)pt) ;
        else
          chain->DATA = pt ;
       }
    }
   else
    {
     if((ptype = getptype(node->USER,STB_NODE_HZDN)) == NULL)
       {
        node->USER = addptype(node->USER,STB_NODE_HZDN,addchain(NULL,pt)) ;
       }
      else
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
            chain = chain->NEXT)
          {
           if((((stbdelay_list *)chain->DATA)->CMD == cmd) || (cmd == NULL))
             break ;
          }
        if(chain == NULL)
           ptype->DATA = addchain((chain_list *)ptype->DATA,(void *)pt) ;
        else
          chain->DATA = pt ;
       }
    }
  }
 else
  {
   if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    {
     if((ptype = getptype(node->USER,STB_NODE_DELAYUP)) == NULL)
       {
        node->USER = addptype(node->USER,STB_NODE_DELAYUP,addchain(NULL,pt)) ;
       }
      else
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
            chain = chain->NEXT)
          {
           if((((stbdelay_list *)chain->DATA)->CMD == cmd) || (cmd == NULL))
             break ;
          }
        if(chain == NULL)
           ptype->DATA = addchain((chain_list *)ptype->DATA,(void *)pt) ;
        else
          chain->DATA = pt ;
       }
    }
   else
    {
     if((ptype = getptype(node->USER,STB_NODE_DELAYDN)) == NULL)
       {
        node->USER = addptype(node->USER,STB_NODE_DELAYDN,addchain(NULL,pt)) ;
       }
      else
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
            chain = chain->NEXT)
          {
           if((((stbdelay_list *)chain->DATA)->CMD == cmd) || (cmd == NULL))
             break ;
          }
        if(chain == NULL)
           ptype->DATA = addchain((chain_list *)ptype->DATA,(void *)pt) ;
        else
          chain->DATA = pt ;
       }
    }
  }

 return(pt) ;
}

/*****************************************************************************
*                           fonction stb_addstbchrono()                      *
*****************************************************************************/
stbchrono_list *stb_addstbchrono(head,verif,type,slope,ckupmin,ckupmax,ckdnmin,ckdnmax)
stbchrono_list *head ;
char verif ; 
char type ; 
char slope ; 
long ckupmin ;
long ckupmax ;
long ckdnmin ;
long ckdnmax ;
{
 stbchrono_list *pt ;

 pt = (stbchrono_list *)mbkalloc(sizeof(stbchrono_list)) ;

 pt->VERIF = verif ;
 pt->SLOPE = slope ;
 pt->TYPE = type ;
 pt->NEXT = head ;
 pt->SPECS = NULL ;
 pt->SPECS_U = NULL ;
 pt->SPECS_D = NULL ;
 pt->SIG1S = NULL ;
 pt->SIG1S_U = NULL ;
 pt->SIG1S_D = NULL ;
 pt->SIG2S = NULL ;
 pt->SIG2S_U = NULL ;
 pt->SIG2S_D = NULL ;
 pt->CKUP_MIN = ckupmin ;
 pt->CKUP_MAX = ckupmax ;
 pt->CKDN_MIN = ckdnmin ;
 pt->CKDN_MAX = ckdnmax ;

 return(pt) ;
}


/*****************************************************************************
*                           fonction stb_addstbdebug()                       *
*****************************************************************************/
stbdebug_list *stb_addstbdebug(stbdebug_list *head,
                               ttvsig_list *sig1,
                               ttvevent_list *sig1event,
                               ttvsig_list *sig2,
                               long per,
                               char *name,
                               char *cmd,
                               ttvevent_list *cmdev,
                               long ckrup_min,
                               long ckrup_max,
                               long ckrdn_min,
                               long ckrdn_max,
                               long ckup_min,
                               long ckup_max,
                               long ckdn_min,
                               long ckdn_max,
                               long ockup_min,
                               long ockup_max,
                               long ockdn_min,
                               long ockdn_max,
                               long ockperiod,
                               long ms,
                               long mh,
                               long s,
                               long h,
                               long holdperiod,
                               long updelta,
                               long downdelta,
                               Setup_Hold_Computation_Detail *detail,
                               ttvevent_list *start_cmd_event,
                               ttvevent_list *start_clock
        )
{
 stbdebug_list *pt ;
// int i;

 pt = (stbdebug_list *)mbkalloc(sizeof(stbdebug_list)) ;

 pt->NEXT = head ;
 pt->SIG1 = sig1 ;
 pt->SIG1_EVENT = sig1event ;
 pt->SIG2 = sig2 ;
 pt->PERIODE = per ;
 pt->CKNAME = name ;
 pt->CMDNAME = cmd ;
 pt->CMD_EVENT = cmdev ;
 pt->CKREFUP_MIN = ckrup_min ;
 pt->CKREFUP_MAX = ckrup_max ;
 pt->CKREFDN_MIN = ckrdn_min ;
 pt->CKREFDN_MAX = ckrdn_max ;
 pt->CKUP_MIN = ckup_min ;
 pt->CKUP_MAX = ckup_max ;
 pt->CKDN_MIN = ckdn_min ;
 pt->CKDN_MAX = ckdn_max ;
 pt->CKORGUP_MIN = ockup_min ;
 pt->CKORGUP_MAX = ockup_max ;
 pt->CKORGDN_MIN = ockdn_min ;
 pt->CKORGDN_MAX = ockdn_max ;
 pt->MARGESETUP = ms ;
 pt->MARGEHOLD = mh ;
 pt->SETUP = s ;
 pt->HOLD = h ;
 pt->CHRONO = NULL ;
 pt->CKORGPERIOD=ockperiod;
 pt->HOLDPERIOD=holdperiod;

 pt->UPDELTA=updelta;
 pt->DOWNDELTA=downdelta;
 memcpy(pt->detail, detail, sizeof(Setup_Hold_Computation_Detail)*2);
 pt->START_CMD_EVENT=start_cmd_event;
 pt->START_CLOCK=start_clock;
 return(pt) ;
}

/*****************************************************************************
*                           fonction stb_freestbchronolist()                 *
*****************************************************************************/
void stb_freestbchronolist(head)
stbchrono_list *head ;
{
 stbchrono_list *pt = head ;

 while(pt != NULL)
  {
   if(pt->SPECS != NULL)
    stb_freestbpair(pt->SPECS) ;
   if(pt->SPECS_U != NULL)
    stb_freestbpair(pt->SPECS_U) ;
   if(pt->SPECS_D != NULL)
    stb_freestbpair(pt->SPECS_D) ;
   if(pt->SIG1S != NULL)
    stb_freestbpair(pt->SIG1S) ;
   if(pt->SIG1S_U != NULL)
    stb_freestbpair(pt->SIG1S_U) ;
   if(pt->SIG1S_D != NULL)
    stb_freestbpair(pt->SIG1S_D) ;
   if(pt->SIG2S != NULL)
    stb_freestbpair(pt->SIG2S) ;
   if(pt->SIG2S_U != NULL)
    stb_freestbpair(pt->SIG2S_U) ;
   if(pt->SIG2S_D != NULL)
    stb_freestbpair(pt->SIG2S_D) ;
   head = head->NEXT ;
   mbkfree(pt) ;
   pt = head ;
  }
}

/*****************************************************************************
*                           fonction stb_freestbdebuglist()                  *
*****************************************************************************/
void stb_freestbdebuglist(head)
stbdebug_list *head ;
{
 stbdebug_list *pt = head ;
 
 while(pt != NULL)
  {
   if(pt->CKNAME != NULL)
     mbkfree(pt->CKNAME) ;
   if(pt->CMDNAME != NULL)
     mbkfree(pt->CMDNAME) ;
   stb_freestbchronolist(pt->CHRONO) ;
   head = head->NEXT ;
   mbkfree(pt) ;
   pt = head ;
  }
}

/*****************************************************************************
*                           fonction stb_getckname()                         *
*****************************************************************************/
char *stb_getckname(stbfig_list *stbfig, char index, stbck *ck)
{
  char buf[1024];
  char ckedge;
if (stb_getclock(stbfig, index , buf, &ckedge, ck)==NULL)
    return NULL;
  return mbkstrdup(buf);
}

static stbpair_list *stb_dup_and_merge_if_needed(stbfig_list *sbf, stbpair_list **ptstbpair, int nbindex)
{
  stbpair_list *sp, *tsp;
  sp=stb_globalstbtab(ptstbpair, nbindex);
  if((sbf->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
    {
      tsp = stb_globalstbpair(sp) ;
      stb_freestbpair(sp) ;
      sp=tsp;
    }
  return sp;
}

/*****************************************************************************
*                           fonction stb_addprechrono()                      *
*****************************************************************************/
void stb_addprechrono(stbfig,debug,tab,tabout,node,slope,ptsig,periodemax,periodemin,periodindex,transparence)
stbfig_list *stbfig ;
stbdebug_list *debug ;
stbpair_list **tab ;
stbpair_list **tabout ;
stbnode *node ;
char slope ;
ttvsig_list *ptsig ;
long *periodemax ;
long *periodemin ;
char periodindex;
int transparence;
{
 stbpair_list *ptstbpair, *uppair, *downpair ;
 stbnode *node1 ;
 stbnode *node2 ;
 stbpair_list *tabx[256] ;
 stbpair_list *pts ;
 char type ;
 char verif ;

 stb_copystbtabpair(tab,NULL,0,periodemax,periodemin,
                    node->NBINDEX,node->CK->CKINDEX,periodindex,transparence) ;
 ptstbpair = stb_globalstbtab(tab,node->NBINDEX) ;
 stb_freestbtabpair(tab,node->NBINDEX) ;

 if((stbfig->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
   {
    pts = stb_globalstbpair(ptstbpair) ;
    stb_freestbpair(ptstbpair) ;
    ptstbpair = pts ;
   }
 if(ptstbpair != NULL)
  {
   if(node->CK->TYPE == STB_TYPE_PRECHARGE)
    {
     if((slope & STB_HZ) == STB_HZ)
      {
       type = STB_TYPE_PREHZ ;
       if((node->CK->VERIF & STB_UP) == STB_UP)
         verif = STB_VERIF_STATE | STB_DN ;
       else
         verif = STB_VERIF_STATE | STB_UP ;
      }
     else
      {
       verif = node->CK->VERIF ;
       type = STB_TYPE_PRECHARGE ;
      }
    }
   else if(node->CK->TYPE == STB_TYPE_EVAL)
    {
     if((slope & STB_HZ) == STB_HZ)
      {
       type = STB_TYPE_EVALHZ ;
       if((node->CK->VERIF & STB_UP) == STB_UP)
         verif = STB_VERIF_STATE | STB_DN ;
       else
         verif = STB_VERIF_STATE | STB_UP ;
      }
     else
      {
       verif = node->CK->VERIF ;
       type = STB_TYPE_EVAL ;
      }
    }
   else
    {
     type = node->CK->TYPE ;
     verif = node->CK->VERIF ;
    }
   debug->CHRONO = stb_addstbchrono(debug->CHRONO,verif,
                                    type,slope,node->CK->SUPMIN,
                                    node->CK->SUPMAX,node->CK->SDNMIN,
                                    node->CK->SDNMAX) ;
   debug->CHRONO->SIG2S = ptstbpair ;
  }
 else
  return ;

 if((node->SPECOUT != NULL) && ((slope & STB_HZ) != STB_HZ))
   {
    stb_copystbtabpair(tabout,tab,node->CK->PERIOD,periodemax,periodemin,
                       node->NBINDEX,node->CK->CKINDEX,periodindex,0) ;
    ptstbpair = stb_globalstbtab(tab,node->NBINDEX) ;
    stb_freestbtabpair(tab,node->NBINDEX) ;

    if((stbfig->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
      {
       pts = stb_globalstbpair(ptstbpair) ;
       stb_freestbpair(ptstbpair) ;
       ptstbpair = pts ;
      }
    debug->CHRONO->SPECS = ptstbpair ;
   }

 node1 = stb_getstbnode(ptsig->NODE) ;
 node2 = stb_getstbnode(ptsig->NODE+1) ;
 stb_copystbtabpair(node1->STBTAB,tab,node->CK->PERIOD,NULL,NULL,
                             node1->NBINDEX,node->CK->CKINDEX,periodindex,1) ;
 stb_copystbtabpair(node2->STBTAB,tabx,node->CK->PERIOD,NULL,NULL,
                             node2->NBINDEX,node->CK->CKINDEX,periodindex,1) ;

 uppair=stb_dup_and_merge_if_needed(stbfig, tabx, node->NBINDEX);
 downpair=stb_dup_and_merge_if_needed(stbfig, tab, node->NBINDEX);
 ptstbpair = stb_globalstbtab(stb_mergestbtab(tab,
                              tabx,node->NBINDEX),
                              node->NBINDEX) ;

 if((stbfig->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
   {
    pts = stb_globalstbpair(ptstbpair) ;
    stb_freestbpair(ptstbpair) ;
    ptstbpair = pts ;
   }


 debug->CHRONO->SIG1S = ptstbpair ;
 debug->CHRONO->SIG1S_U=uppair;
 debug->CHRONO->SIG1S_D=downpair;

 stb_freestbtabpair(tab,node->NBINDEX) ;
 stb_freestbtabpair(tabx,node->NBINDEX) ;
}

void stb_get_setup_and_hold_slopes_delta(stbck *clock, long *updelta, long *downdelta)
{
  long active;
  *updelta=*downdelta=0;

  active = clock->ACTIVE;
  if ((clock->TYPE == STB_TYPE_COMMAND)
      && ((clock->VERIF & STB_NO_VERIF) != STB_NO_VERIF))
    {
      if ((clock->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
        {
          if ((((clock->VERIF & STB_DN) == STB_DN) && ((clock->VERIF & STB_VERIF_STATE) == STB_VERIF_STATE))
              || (((clock->VERIF & STB_UP) == STB_UP) && ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)))
            active = STB_STATE_DN;
        }
      else if ((clock->ACTIVE & STB_STATE_DN) == STB_STATE_DN)
        {
          if ((((clock->VERIF & STB_UP) == STB_UP) && ((clock->VERIF & STB_VERIF_STATE) == STB_VERIF_STATE))
              || (((clock->VERIF & STB_DN) == STB_DN) && ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)))
            active = STB_STATE_UP;
        }
    }

  if ((active & STB_SLOPE) == STB_SLOPE) return;

  if ((active & STB_STATE) == STB_STATE)
    {
      if ((active & STB_STATE_UP) == STB_STATE_UP)
        {
          if(STB_OPEN_LATCH_PHASE == 'N')
            {
              if ((clock->VERIF & STB_VERIF_EDGE) != STB_VERIF_EDGE)
                *updelta = (clock->SUPMIN < clock->SDNMIN) ? 0 : -clock->PERIOD;
            }
          else
            {
              if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)
                *downdelta = (clock->SUPMIN < clock->SDNMIN) ? 0 : clock->PERIOD;
              *downdelta = (clock->SUPMAX < clock->SDNMAX) ? 0 : clock->PERIOD;
            }
        }
      else
        {
          if(STB_OPEN_LATCH_PHASE == 'N')
            {
              if ((clock->VERIF & STB_VERIF_EDGE) != STB_VERIF_EDGE)
                *downdelta = (clock->SDNMIN < clock->SUPMIN) ? 0 : -clock->PERIOD;
            }
          else
            {
              if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)
                *updelta =  (clock->SDNMIN < clock->SUPMIN) ? 0 : clock->PERIOD;
              *updelta= (clock->SDNMAX < clock->SUPMAX) ? 0 : clock->PERIOD;
            }
        }
    }
}

/*****************************************************************************
*                           fonction stb_debugstberror()                     *
*****************************************************************************/

static int getslopedate(stbck *ck, int max, long *clockslope, long *cmdslope, ttvevent_list *ev)
{
  long tab[4];
  int ud;
  stbnode *ock;

  if (ck->ORIGINAL_CLOCK==NULL) return 0;

  ock=stb_getstbnode(ck->ORIGINAL_CLOCK);
  if (ck->ORIGINAL_CLOCK->TYPE & TTV_NODE_UP) ud=1; else ud=0;

  tab[0]=ock->CK->SDNMIN;
  tab[1]=ock->CK->SUPMIN;
  tab[2]=ock->CK->SDNMAX;
  tab[3]=ock->CK->SUPMAX;
  *clockslope=tab[max*2+ud];

  tab[0]=ck->SDNMIN;
  tab[1]=ck->SUPMIN;
  tab[2]=ck->SDNMAX;
  tab[3]=ck->SUPMAX;
  if (ev!=NULL)
    {
      if (ev->TYPE & TTV_NODE_UP) ud=1; else ud=0;
    }

//  if ((ev->TYPE & TTV_NODE_UP)!=(ck->ORIGINAL_CLOCK->TYPE & TTV_NODE_UP)) ud=(ud+1) & 1;
  *cmdslope=tab[max*2+ud];
  return 1;
}


static void stb_get_path_margin_info(inffig_list *ifl, ttvevent_list *tve_data, ttvevent_list *tve_clock, int setup, float *factor_data, long *delta_data, float *factor_clock, long *delta_clock, int clockisdata, int dataisclock)
{
  char section[1024], *c, *d;
  ptype_list *pt;
  
  if (setup) c=INF_PATHDELAYMARGINMIN, d=INF_PATHDELAYMARGINMAX;
  else d=INF_PATHDELAYMARGINMIN, c=INF_PATHDELAYMARGINMAX;

  *factor_data=*factor_clock=-1;
  if (tve_clock!=NULL && (pt=getptype(tve_clock->ROOT->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX))!=NULL)
    {
      if (!clockisdata)
        sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINCLOCK",""%s", c, (tve_clock->TYPE & TTV_NODE_UP)==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);      
      else
        sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINDATA",""%s", c, (tve_clock->TYPE & TTV_NODE_UP)==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);      

      ttv_getmargin(ifl, tve_clock->ROOT, (char *)pt->DATA, section, factor_clock, delta_clock);
    }

  if (tve_data!=NULL && (pt=getptype(tve_data->ROOT->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX))!=NULL)
    {
      if (!dataisclock)
        sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINDATA",%s", d, (tve_data->TYPE & TTV_NODE_UP)==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);
      else
        sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINCLOCK",%s", d, (tve_data->TYPE & TTV_NODE_UP)==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);
      ttv_getmargin(ifl, tve_data->ROOT, (char *)pt->DATA, section, factor_data, delta_data);
    }
}

static void stb_compute_margins(inffig_list *ifl, stbck *inputck, stbpair_list *start_stab, stbpair_list *end_stab, ttvevent_list *tve_data, stbck *outputck, ttvevent_list *tve_clock, long min, long max, int setup, datamoved_type datamoved, int outputindex, Extended_Setup_Hold_Computation_Data_Item *eshcdi, char flags, int ideal, int clockisdata, int dataisclock)
{
  float factor_data, factor_clock;
  long delta_data, delta_clock;
  long path_start_time, cmdslope, path_end_time, clock_path_start_time, data_start, data_lag=0;
  long overlapc, overlapd; 
  stbpair_list *runpair;
  int err2=0;

  eshcdi->clock_margin=0;
  eshcdi->data_margin=0;

  if (start_stab==NULL || end_stab==NULL) return;

  stb_get_path_margin_info(ifl, tve_data, tve_clock, setup, &factor_data, &delta_data, &factor_clock, &delta_clock,clockisdata, dataisclock);

  if (factor_data==-1 && factor_clock==-1) return;

  if (factor_data!=-1)
    {
      if (!setup) data_start=start_stab->D;
      else 
        {
          for (runpair=start_stab; runpair->NEXT!=NULL; runpair=runpair->NEXT) ;
          data_start=runpair->U;
        }

      if (inputck!=NULL/* && inputck->CMD!=NULL*/)
        {
          long access/*, v1, v2*/;
          
          if (getslopedate(inputck, setup?1:0, &path_start_time, &cmdslope, NULL/*inputck->CMD*/))
          {
            if (setup) access=inputck->ACCESSMAX;
            else access=inputck->ACCESSMIN;
            data_lag=data_start-(cmdslope+access);
            if (data_lag<0) data_lag=0;
          }
          else data_lag=0;
/*          stb_getstbdelta(checkstbfig, checkout, &v1, &v2, inputck->CMD);
          if (setup) v1=v2;

          if (data_lag!=v1) printf("lag: %ld %ld %ld -> %ld vs good %ld\n",data_start,cmdslope,access,data_lag,v1);
*/
        }
      else
        path_start_time=data_start;
      
      if (!setup) path_end_time=end_stab->D;
      else 
        {
          for (runpair=end_stab; runpair->NEXT!=NULL; runpair=runpair->NEXT) ;
          path_end_time=runpair->U;
        }
    }

  if (factor_clock!=-1 && tve_clock!=NULL)
    err2=getslopedate(stb_getstbnode(tve_clock)->CK, setup?0:1, &clock_path_start_time, &cmdslope, tve_clock);

  path_end_time-=datamoved[outputindex][inputck?inputck->CKINDEX:0];
  

//  printf("%s > %s / %s : %ld -> %ld  | %ld -> %ld | (mM %ld %ld)\n",setup?"setup":"hold", tve_data->ROOT->NAME, tve_clock->ROOT->NAME, path_start_time, path_end_time, clock_path_start_time, cmdslope, min, max);

/*
  if (setup || (factor_data==-1 || factor_clock==-1)) overlapc=overlapd=min=max=0;
  else
*/
  if (!setup) overlapc=max, overlapd=min;
  else overlapc=min, overlapd=max;

  if (factor_data!=-1 && overlapd!=0) overlapd-=path_start_time;
    
  if (!ideal && err2)
    {
      if (overlapc!=0) overlapc-=clock_path_start_time;
      cmdslope-=clock_path_start_time;
      eshcdi->clock_margin=mbk_long_round(overlapc+((float)cmdslope-overlapc)*factor_clock+delta_clock)-cmdslope;
    }
  else eshcdi->clock_margin=0;

  if (factor_data!=-1)
    {
      path_end_time-=path_start_time+data_lag;
      eshcdi->data_margin=mbk_long_round(overlapd+((float)path_end_time-overlapd)*factor_data+delta_data)-path_end_time;
    }
  else  eshcdi->data_margin=0;
//  printf("  >> path change: margedata:%ld / margeclock:%ld , od=%ld oc=%ld\n", eshcdi->data_margin, eshcdi->clock_margin, overlapd, overlapc);
}

static void mymin(Extended_Setup_Hold_Computation_Data_Item *lastval, int val, ttvevent_list *common)
{
  if (lastval->value==-1 || val<lastval->value)
    {
      lastval->value=val;
      lastval->common=common;
    }
}


static long stb_find_uncertainty(ttvfig_list *tvf, ttvevent_list *startclockev, ttvevent_list *endclockev, int setuphold, chain_list *assoclist)
{
  long flags=0;
  char startname[512], endname[512];
  char startnetname[512], endnetname[512];
  char *startname_p=startname, *endname_p=endname, *startnetname_p=startnetname, *endnetname_p=endnetname;
  chain_list *cl;
  inf_assoc *assoc;
  
  if (assoclist!=NULL)
    {
      if (setuphold) flags|=INF_CLOCK_UNCERTAINTY_SETUP;
      else flags|=INF_CLOCK_UNCERTAINTY_HOLD;
      if (endclockev!=NULL)
        {
          flags|=(endclockev->TYPE & TTV_NODE_UP)?INF_CLOCK_UNCERTAINTY_END_RISE:INF_CLOCK_UNCERTAINTY_END_FALL;
          if ((endclockev->ROOT->TYPE & TTV_SIG_C)==0)
            {
              ttv_getsigname(tvf, endname, endclockev->ROOT);
              ttv_getnetname(tvf, endnetname, endclockev->ROOT);
            }
          else
            {
              endname_p=endclockev->ROOT->NAME;
              endnetname_p=endclockev->ROOT->NETNAME;
            }
        }
      if (startclockev!=NULL)
        {
          flags|=(startclockev->TYPE & TTV_NODE_UP)?INF_CLOCK_UNCERTAINTY_START_RISE:INF_CLOCK_UNCERTAINTY_START_FALL;
          if ((startclockev->ROOT->TYPE & TTV_SIG_C)==0)
            {
              ttv_getsigname(tvf, startname, startclockev->ROOT);
              ttv_getnetname(tvf, startnetname, startclockev->ROOT);
            }
          else
            {
              startname_p=startclockev->ROOT->NAME;
              startnetname_p=startclockev->ROOT->NETNAME;
            }
        }

      for (cl=assoclist; cl!=NULL; cl=cl->NEXT)
        {
          assoc = (inf_assoc *) cl->DATA;
          if ((flags & assoc->lval)==flags
              && (startclockev==NULL || mbk_TestREGEX(startnetname_p, assoc->orig) || mbk_TestREGEX(startname_p, assoc->orig))
              && (endclockev==NULL || mbk_TestREGEX(endnetname_p, assoc->dest) || mbk_TestREGEX(endname_p, assoc->dest))
              )
            return mbk_long_round(assoc->dval*1e12*TTV_UNIT);
        }
    }
  return 0;
}

void stb_getslacktopclocks(ttvevent_list *directive_setup_ev, stbnode *output, int outhz, stbck *inputck, stbck *outputck, ttvevent_list **startclock, ttvevent_list **setupclock, ttvevent_list **holdclock)
{
  ttvevent_list *setup_event, *hold_event;
  int setup_mustbehz, hold_mustbehz;

  *startclock=NULL;
  *setupclock=*holdclock=NULL;

  if (directive_setup_ev!=NULL)
    {
       setup_event=directive_setup_ev;
       hold_event=ttv_opposite_event(directive_setup_ev);
       setup_mustbehz=hold_mustbehz=0;
       outputck=stb_getstbnode(directive_setup_ev)->CK;
    }
  else
    stb_get_setup_and_hold_test_events(outputck, outhz, &setup_event, &hold_event, output, &setup_mustbehz, &hold_mustbehz);

  if (inputck!=NULL && inputck->ORIGINAL_CLOCK!=NULL)
    *startclock=inputck->ORIGINAL_CLOCK;
  
  *setupclock=stb_get_top_node(setup_event, setup_mustbehz);
  if (setup_event==hold_event && setup_mustbehz==hold_mustbehz) *holdclock=*setupclock;
  else *holdclock=stb_get_top_node(hold_event, hold_mustbehz);
}


void stb_compute_common_clock_gap(inffig_list *ifl, stbfig_list *sf, ttvsig_list *input, stbck *inputck, stbnode *node, int trans_involved[4], stbpair_list *start_stab[2][256], stbpair_list **end_stab, stbpair_list **end_stabhz, datamoved_type datamoved, Extended_Setup_Hold_Computation_Data *eshcd, ttvevent_list *directive_setup_ev, int flags, ptype_list *geneclockinfo)
{
  int i, nothz;
  long min, max, minh, maxh;
  int depth, abort=0, phase;
  stbnode *inputnode;
  stbck *ck, *clock;
  ttvevent_list *setup_event, *hold_event, *common_node, *inputcmd;
  stbpair_list *selected_end_stab;
  int outidx, ideal=0, setup_mustbehz, hold_mustbehz, ok;
  chain_list *uncertainty_list;
  
  if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0 || !inf_GetPointer (ifl, INF_CLOCK_UNCERTAINTY, "", (void **)&uncertainty_list)) uncertainty_list=NULL;

  if (directive_setup_ev!=NULL) clock=stb_getstbnode(directive_setup_ev)->CK;
  else clock=node->CK;

  depth=V_INT_TAB[__STB_COREL_SKEW_ANA_DEPTH].VALUE;
  
  if (clock!=NULL && clock->ORIGINAL_CLOCK!=NULL && getptype(clock->ORIGINAL_CLOCK->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) ideal=1;
  
  if (clock==NULL || inputck==NULL || depth==0 || ideal || (flags & (STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA|STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK))!=0)
    {
      for (i=0; i<2; i++)
        {
          eshcd->setup_clock_gap[i].value=eshcd->hold_clock_gap[i].value=0;
          eshcd->setup_clock_gap[i].common=eshcd->hold_clock_gap[i].common=NULL;
          eshcd->setup_clock_gap[i].clock_uncertainty=eshcd->hold_clock_gap[i].clock_uncertainty=0;
        }
    }
  else
    {      
      for (i=0; i<2; i++)
        {
          eshcd->setup_clock_gap[i].value=eshcd->hold_clock_gap[i].value=-1;
          eshcd->setup_clock_gap[i].common=eshcd->hold_clock_gap[i].common=NULL;
          eshcd->setup_clock_gap[i].clock_uncertainty=eshcd->hold_clock_gap[i].clock_uncertainty=0;
        }
      
      for (i=0;i<4;i++)
        {
          if (trans_involved[i])
            {
              nothz=i>>1;
              inputnode=stb_getstbnode(&input->NODE[i & 1]);
              ck=inputck;          
              
              if ((inputnode->EVENT->ROOT->TYPE & TTV_SIG_R)!=0) inputcmd=inputnode->EVENT;
              else if (ck!=NULL && ck->CMD==NULL && getptype(inputnode->EVENT->ROOT->USER, TTV_SIG_CLOCK)!=NULL)
                 inputcmd=node->EVENT;
              else
                 inputcmd=ck!=NULL?ck->CMD:NULL;
              
              if (directive_setup_ev!=NULL)
              {
                 setup_event=directive_setup_ev;
                 hold_event=ttv_opposite_event(directive_setup_ev);
                 setup_mustbehz=hold_mustbehz=0;
                 ok=1;
              }
              else
                ok=stb_get_setup_and_hold_test_events(clock, !nothz, &setup_event, &hold_event, node, &setup_mustbehz, &hold_mustbehz);
              if (inputcmd!=NULL)
               {
                 if (ok)
                    {
                      /*                      stb_get_common_node_info(ck->CMD, setup_event, depth, &min, &max, &common_node);
                                              mymin(&eshcd->setup_clock_gap[nothz], max-min, common_node);
                                              if (hold_event==setup_event) mymin(&eshcd->hold_clock_gap[nothz], max-min, common_node);
                                              else */
                      {
                        stb_get_common_node_info(inputcmd, hold_event, depth, &min, &max, &common_node, 0, hold_mustbehz);
                        if (common_node==NULL)
                          {
                            min=0;
                            max=stb_get_generated_clock_common_node_info(geneclockinfo, inputcmd, hold_event, 0, hold_mustbehz, &common_node);
                          }
                        mymin(&eshcd->hold_clock_gap[nothz], max-min, common_node);
                      }                      
                    }
               }
              else
               {
                 // on ne peut pas savoir, on reste dans le pire cas
                 if (!abort) for (i=0; i<2; i++) eshcd->setup_clock_gap[i].value=eshcd->hold_clock_gap[i].value=0;
                 abort=1;
               }

              if (ok && uncertainty_list!=NULL)
                {
                  ttvevent_list *hev, *sev, *startev;
                  hev=stb_get_top_node(hold_event, hold_mustbehz);
                  sev=stb_get_top_node(setup_event, setup_mustbehz);
                  if (inputcmd!=NULL) startev=stb_get_top_node(inputcmd, 0);
                  else startev=NULL;
                  eshcd->setup_clock_gap[nothz].clock_uncertainty=stb_find_uncertainty(sf->FIG,  startev, sev, 1, uncertainty_list);
                  eshcd->hold_clock_gap[nothz].clock_uncertainty=stb_find_uncertainty(sf->FIG,  startev, hev, 0, uncertainty_list);
                }
            }
        }

      for (i=0; i<2; i++)
        {
          if (eshcd->setup_clock_gap[i].value==-1) eshcd->setup_clock_gap[i].value=0;
          if (eshcd->hold_clock_gap[i].value==-1) eshcd->hold_clock_gap[i].value=0;
        }
    }

  for (i=0; i<2; i++)
    {
      eshcd->setup_clock_gap[i].clock_margin=eshcd->setup_clock_gap[i].data_margin=0;
      eshcd->hold_clock_gap[i].clock_margin=eshcd->hold_clock_gap[i].data_margin=0;
      eshcd->hold_clock_gap[i].same_origin=eshcd->setup_clock_gap[i].same_origin=0;
    }

  if (node->EVENT->TYPE & TTV_NODE_UP) outidx=1; else outidx=0;

  if ((flags & (STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA|STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK))==0 && input!=node->EVENT->ROOT)
    {
      for (i=0;i<4;i++)
        {
          if (trans_involved[i])
            {
              nothz=i>>1;
              inputnode=stb_getstbnode(&input->NODE[i & 1]);
              ck=inputck;          
              if ((inputnode->EVENT->ROOT->TYPE & TTV_SIG_R)!=0) inputcmd=inputnode->EVENT;
              else if (ck!=NULL && ck->CMD==NULL && getptype(inputnode->EVENT->ROOT->USER, TTV_SIG_CLOCK)!=NULL)
                 inputcmd=node->EVENT;
              else inputcmd=ck!=NULL?ck->CMD:NULL;

              min=max=minh=maxh=0;
              if (directive_setup_ev!=NULL)
                {
                   setup_event=directive_setup_ev;
                   hold_event=ttv_opposite_event(directive_setup_ev);
                   setup_mustbehz=hold_mustbehz=0;
                }
              else
                stb_get_setup_and_hold_test_events(clock, !nothz, &setup_event, &hold_event, node, &setup_mustbehz, &hold_mustbehz);

              if (inputcmd!=NULL)
                {
#ifdef NEWTEST
                  ttvevent_list *inev, *top;
                  int nocompsetup=0, nocomphold=0;
                  if ((inev=stb_get_top_node(inputcmd, 0))!=NULL)
                    {
                      if ((top=stb_get_top_node(setup_event, setup_mustbehz))!=NULL && inev==top) nocompsetup=1;
                      if (setup_event==hold_event && setup_mustbehz==hold_mustbehz) nocomphold=nocompsetup;
                      else if ((top=stb_get_top_node(hold_event, hold_mustbehz))!=NULL && inev==top) nocomphold=1;
                      eshcd->setup_clock_gap[nothz].same_origin=nocompsetup;
                      eshcd->hold_clock_gap[nothz].same_origin=nocomphold;
                    }
#endif
                  if (!ideal)
                    {
                      stb_get_common_node_info(inputcmd, setup_event, depth, &min, &max, &common_node, 0, setup_mustbehz);
                      if (setup_event!=hold_event || setup_mustbehz!=hold_mustbehz) stb_get_common_node_info(inputcmd, hold_event, depth, &minh, &maxh, &common_node, 0, hold_mustbehz);
                      else minh=min, maxh=max;
                    }
                }
          
              phase=STB_NO_INDEX;
              if (flags & STB_DEBUG_DIRECTIVE)
               {
                 int j;
                 for (phase=0; phase<node->NBINDEX && end_stab[phase]==NULL; phase++) ;
                 if (phase>=node->NBINDEX) phase=STB_NO_INDEX;
               }
              if (phase==STB_NO_INDEX)
               {
                 if (clock!=NULL) phase=(int)clock->CKINDEX;
                 else if (ck!=NULL) phase=ck->CKINDEX;
                 else phase=0; // combinatoire
               }
              if (phase==STB_NO_INDEX) selected_end_stab=NULL;
              else if (nothz) selected_end_stab=end_stab[phase];
              else selected_end_stab=end_stabhz[phase];
              if (selected_end_stab!=NULL)
                {
                  /*              checkstbfig=sf;
                                  checkout=inputnode->EVENT;*/
                  if (inputck==NULL) phase=0; // combinatoire
                  else phase=inputck->CKINDEX;
                  if (phase!=STB_NO_INDEX)
                    {
                      stb_compute_margins(ifl, inputck, start_stab[i & 1][phase], selected_end_stab, node->EVENT, clock, setup_event, min, max, 1, datamoved, nothz?outidx:2+outidx, &eshcd->setup_clock_gap[nothz], flags, ideal,0,0);
                      stb_compute_margins(ifl, inputck, start_stab[i & 1][phase], selected_end_stab, node->EVENT, clock, hold_event, minh, maxh, 0, datamoved, nothz?outidx:2+outidx, &eshcd->hold_clock_gap[nothz], flags, ideal,0,0);
                    }
                }
            }
        }
    }
}

static int getbestperiod(char *phaseused, long *period, int nbphase)
{
 int i;
 int per=STB_NO_INDEX;
 for (i=0; i<nbphase; i++)
  if (phaseused[i] && (per==STB_NO_INDEX || period[per]<period[i])) per=i;
 return per;
}

void stb_assign_operations_low(Setup_Hold_Computation_Detail_INFO *detail, long datamoved, EventPeriodeHold_type *period_stuff)
{
  int dec=0;
  
  if (detail->VALUE!=STB_NO_TIME)
   {
     detail->datamoved=datamoved;
     detail->nb_setup_cycle=period_stuff->nb_setup_cycle;
     detail->mc_setup_period=period_stuff->mc_setup_period;
     detail->mc_hold_period=0;
   }
}

void stb_assign_operations(int i, Setup_Hold_Computation_Detail *detail, datamoved_type datamoved, EventPeriodeHold_type *period_stuff)
{
  int dec=0;
  
  if (detail[i].setup.VALUE!=STB_NO_TIME)
   {
     if (detail[i].setup.flags & STB_DEBUG_HZPATH) dec=2;
     detail[i].setup.datamoved=datamoved[i+dec][detail[i].setup.phase_origin];
     detail[i].setup.nb_setup_cycle=period_stuff[i].nb_setup_cycle;
     detail[i].setup.mc_setup_period=period_stuff[i].mc_setup_period;
     detail[i].setup.mc_hold_period=0;
   }
  if (detail[i].hold.VALUE!=STB_NO_TIME)
   {
     if (detail[i].hold.flags & STB_DEBUG_HZPATH) dec=2;
     detail[i].hold.datamoved=datamoved[i+dec][detail[i].hold.phase_origin];
     detail[i].hold.nb_setup_cycle=period_stuff[i].nb_setup_cycle;
     detail[i].hold.mc_setup_period=period_stuff[i].mc_setup_period;
     detail[i].hold.nb_hold_cycle=period_stuff[i].nb_hold_cycle;
     detail[i].hold.mc_hold_period=period_stuff[i].mc_hold_period;
   }
}

long stb_getcross_PP(ptype_list *list, char phase)
{
  ptype_list *pt;
  if ((pt=getptype(list, (long)phase))!=NULL) return (long)pt->DATA;
  if ((pt=getptype(list, (long)STB_NO_INDEX))!=NULL) return (long)pt->DATA;
  return TTV_NOTIME;
}

void stb_add_infos(stbfig_list *stbfig, ttvevent_list *output, int i, Setup_Hold_Computation_Detail *detail, stbck *inputck, ttvevent_list *input, stbdelay_list **useddelay)
{
  long datamin=0, datamax=0, start;
  char phase;
  if (inputck)
  {
    stb_getstbdelta(stbfig, input, &datamin, &datamax, inputck->CMD);
    if (useddelay[i]!=NULL)
    {
      long crossmin, crossmax;
      phase= inputck?inputck->CKINDEX:STB_NO_INDEX;
      crossmin=stb_getcross_PP(useddelay[i]->STARTMIN_PP, phase);
      crossmax=stb_getcross_PP(useddelay[i]->STARTMAX_PP, phase);
      if (datamax>0)
      {
        start = ttv_getinittime(stbfig->FIG,input,STB_NO_INDEX,TTV_FIND_MAX,phase,NULL) ;
        if (crossmax!=TTV_NOTIME) datamax=datamax-(start-crossmax);
      }
      if (datamin>0)
      {
        start = ttv_getinittime(stbfig->FIG,input,STB_NO_INDEX,TTV_FIND_MIN,phase,NULL) ;
        if (crossmin!=TTV_NOTIME) datamin=datamin-(start-crossmin);
      }
    }
  }
  detail[i].hold.lag=datamin;
  detail[i].setup.lag=datamax;
}

chain_list *stb_debug_retreive_delays(stbfig_list *stbfig, ttvsig_list *sig, int dir, int forceclockordata, int falseaccessmode, long compflags)
{
  // forceclockordata: 1=>clock, 2=>data
  long find;
  int oldprecisionlevel;
  ttvpath_list *path, *scanpath;
  chain_list *chainsig=NULL;
  ptype_list *ptype, *pt;
  int forceclock=1, forcedata=1, ideal;
  ttvevent_list *cmdlatch, *clock, *start;
  ttvpath_stb_stuff *tps;
  stbdelay_list *delay ;
  
  if (getptype(sig->NODE[0].USER,STB_NODE_DELAY) != NULL || getptype(sig->NODE[1].USER,STB_NODE_DELAY)!=NULL) return NULL;

  ttv_search_mode(1, TTV_MORE_OPTIONS_KEEP_PHASE_INFO);

  if(stbfig->GRAPH == STB_RED_GRAPH)
    find = TTV_FIND_PATH ;
  else
    find = TTV_FIND_LINE ;

  if (stbfig->CLOCK && falseaccessmode)
  {
    ttv_set_ttv_getpath_clock_list(stbfig->CLOCK);
    ttv_activate_multi_cmd_critic_mode(1);
    ttv_activate_path_and_access_mode(1);
    find|=TTV_FIND_ACCESS;
  }
  
  if (forceclockordata==1) forcedata=0;
  if (forceclockordata==2) forceclock=0;
          
  oldprecisionlevel=ttv_SetPrecisionLevel(0);

  if (dir==1) find |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPDW;
  if (dir==0) find |= TTV_FIND_NOT_DWUP | TTV_FIND_NOT_UPUP;

  if (compflags & STB_COMPUTE_HOLD_ONLY)
    path=NULL;
  else if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    path = ttv_getpath(stbfig->FIG,stbfig->FIG,sig,NULL, TTV_DELAY_MAX,TTV_DELAY_MIN,TTV_FIND_MAX|find) ;
  else
    path = ttv_getpath(stbfig->FIG,NULL,sig,NULL, TTV_DELAY_MAX,TTV_DELAY_MIN,TTV_FIND_MAX|find) ;
  
  for(scanpath = path ; scanpath != NULL ; scanpath = scanpath->NEXT)
    {
      cmdlatch=NULL;
      clock=NULL;
      start=scanpath->NODE;
      if (scanpath->LATCH!=NULL)
      {
         cmdlatch=scanpath->CMDLATCH;
         clock=scanpath->NODE;
         start=scanpath->LATCH;
      }
      if(((scanpath->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) &&
         ((scanpath->ROOT->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R))
        continue ;
      if ((start->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R|TTV_SIG_N))==TTV_SIG_N)
              continue;
      if (!((forceclock && getptype(start->ROOT->USER, STB_IS_CLOCK)!=NULL)
          || (forcedata && (getptype(start->ROOT->USER, STB_IS_CLOCK)==NULL
                            || (scanpath->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_FLAG_THRU_FILTER_FOUND)!=0)
             ))
         ) continue;
                 
#ifdef PRECHTEST
      if ((scanpath->ROOT->ROOT->TYPE & TTV_SIG_R) != 0)
        {
          ptype=getptype(scanpath->USER, TTV_PATH_PREVIOUS_NODE);
          if (ptype!=NULL && stb_sigisonclockpath((ttvevent_list *)ptype->DATA, scanpath->NODE)) continue;
        }
#endif
      if (getptype(scanpath->NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) ideal=1; else ideal=0;

      delay=stb_addstbdelay(scanpath->ROOT,start,ideal?0:scanpath->DELAY-scanpath->CLOCKPATHDELAY, TTV_NOTIME, TTV_NOTIME,scanpath->CROSSMINDELAY,scanpath->TYPE,scanpath->CMD,scanpath->LATCHLIST,NULL,cmdlatch,clock) ;
      pt=getptype(scanpath->USER,TTV_PATH_PHASE_INFO);
      if (pt!=NULL)
        {
          for (tps=(ttvpath_stb_stuff *)pt->DATA; tps!=NULL; tps=tps->NEXT)
            delay->STARTMAX_PP=addptype(delay->STARTMAX_PP, (long)tps->STARTPHASE, (void *)tps->STARTTIME);
        }
      else
        delay->STARTMAX_PP=addptype(delay->STARTMAX_PP, (long)STB_NO_INDEX, (void *)scanpath->CROSSMINDELAY);
        
      if (compflags & STB_COMPUTE_SETUP_ONLY)
      {
        delay=stb_addstbdelay(scanpath->ROOT,start,TTV_NOTIME, ideal?0:scanpath->DELAY-scanpath->CLOCKPATHDELAY,scanpath->CROSSMINDELAY, TTV_NOTIME,scanpath->TYPE,scanpath->CMD,NULL,scanpath->LATCHLIST,cmdlatch,clock) ;
        if (pt!=NULL)
          {
            for (tps=(ttvpath_stb_stuff *)pt->DATA; tps!=NULL; tps=tps->NEXT)
              delay->STARTMIN_PP=addptype(delay->STARTMIN_PP, (long)tps->STARTPHASE, (void *)tps->STARTTIME);
          }
        else
          delay->STARTMIN_PP=addptype(delay->STARTMIN_PP, (long)STB_NO_INDEX, (void *)scanpath->CROSSMINDELAY);
      }
      if (pt!=NULL) ttv_freepathstblist((ttvpath_stb_stuff *)pt->DATA);

      if((start->ROOT->TYPE & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)
        {
          chainsig = addchain(chainsig,start->ROOT) ;
          start->ROOT->TYPE |= TTV_SIG_MARQUE ;
        }
    }

  ttv_freepathlist(path) ;
  
  if (compflags & STB_COMPUTE_SETUP_ONLY)
    path=NULL;
  else if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    path = ttv_getpath(stbfig->FIG,stbfig->FIG,sig,NULL, TTV_DELAY_MAX,TTV_DELAY_MIN,TTV_FIND_MIN|find) ;
  else
    path = ttv_getpath(stbfig->FIG,NULL,sig,NULL, TTV_DELAY_MAX,TTV_DELAY_MIN,TTV_FIND_MIN|find) ;
  
  for(scanpath = path ; scanpath != NULL ; scanpath = scanpath->NEXT)
    {
      cmdlatch=NULL;
      clock=NULL;
      start=scanpath->NODE;
      if (scanpath->LATCH!=NULL)
      {
         cmdlatch=scanpath->CMDLATCH;
         clock=scanpath->NODE;
         start=scanpath->LATCH;
      }
      if(((scanpath->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) &&
         ((scanpath->ROOT->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R))
        continue ;
      if ((start->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R|TTV_SIG_N))==TTV_SIG_N)
              continue;
      if (!((forceclock && getptype(start->ROOT->USER, STB_IS_CLOCK)!=NULL)
          || (forcedata && (getptype(start->ROOT->USER, STB_IS_CLOCK)==NULL
                            || (scanpath->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_FLAG_THRU_FILTER_FOUND)!=0)
             ))
         ) continue;
#ifdef PRECHTEST
      if ((scanpath->ROOT->ROOT->TYPE & TTV_SIG_R) != 0)
        {
          ptype=getptype(scanpath->USER, TTV_PATH_PREVIOUS_NODE);
          if (ptype!=NULL && stb_sigisonclockpath((ttvevent_list *)ptype->DATA, scanpath->NODE)) continue;
        }
#endif
      if (getptype(scanpath->NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) ideal=1; else ideal=0;
      delay=stb_addstbdelay(scanpath->ROOT,start,TTV_NOTIME, ideal?0:scanpath->DELAY-scanpath->CLOCKPATHDELAY,scanpath->CROSSMINDELAY, TTV_NOTIME,scanpath->TYPE,scanpath->CMD,NULL,scanpath->LATCHLIST,cmdlatch,clock) ;
      pt=getptype(scanpath->USER,TTV_PATH_PHASE_INFO);
      if (pt!=NULL)
        {
          for (tps=(ttvpath_stb_stuff *)pt->DATA; tps!=NULL; tps=tps->NEXT)
            delay->STARTMIN_PP=addptype(delay->STARTMIN_PP, (long)tps->STARTPHASE, (void *)tps->STARTTIME);
        }
      else
        delay->STARTMIN_PP=addptype(delay->STARTMIN_PP, (long)STB_NO_INDEX, (void *)scanpath->CROSSMINDELAY);

      if (compflags & STB_COMPUTE_SETUP_ONLY)
      {
        delay=stb_addstbdelay(scanpath->ROOT,start,ideal?0:scanpath->DELAY-scanpath->CLOCKPATHDELAY, TTV_NOTIME, TTV_NOTIME,scanpath->CROSSMINDELAY,scanpath->TYPE,scanpath->CMD,scanpath->LATCHLIST,NULL,cmdlatch,clock) ;
        if (pt!=NULL)
          {
            for (tps=(ttvpath_stb_stuff *)pt->DATA; tps!=NULL; tps=tps->NEXT)
              delay->STARTMAX_PP=addptype(delay->STARTMAX_PP, (long)tps->STARTPHASE, (void *)tps->STARTTIME);
          }
        else
          delay->STARTMAX_PP=addptype(delay->STARTMAX_PP, (long)STB_NO_INDEX, (void *)scanpath->CROSSMINDELAY);
      }
      if (pt!=NULL) ttv_freepathstblist((ttvpath_stb_stuff *)pt->DATA);

      if((start->ROOT->TYPE & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)
        {
          chainsig = addchain(chainsig,start->ROOT) ;
          start->ROOT->TYPE |= TTV_SIG_MARQUE ;
        }
    }
  
  ttv_freepathlist(path) ;
  ttv_search_mode(0, TTV_MORE_OPTIONS_KEEP_PHASE_INFO);
  ttv_SetPrecisionLevel(oldprecisionlevel);
  ttv_set_ttv_getpath_clock_list(NULL);
  ttv_activate_multi_cmd_critic_mode(0);
  ttv_activate_path_and_access_mode(0);
  return chainsig;
}

stbdebug_list *stb_debugstberror(stbfig_list *stbfig, ttvsig_list *sig, long margin, long computeflags)
{
 stbdebug_list *debug = NULL ;
 ttvpath_list *path ;
 ttvpath_list *scanpath ;
 ttvevent_list *event, *sig1event ;
 ttvevent_list *ptevent ;
 ttvsig_list *ptsig ;
 stbdelay_list *delay, *useddelay[2];
 chain_list *chainsig = NULL ;
 chain_list *chaincmd ;
 chain_list *chainx ;
 chain_list *chain ;
 stbnode *node ;
 stbnode *nodex ;
 stbdomain_list *ptdomain = NULL;
 stbpair_list *tabpair[2][256] ;
 stbpair_list *tabhz[2][256] ;
 stbpair_list *(*tab)[256] ;
 stbpair_list *ptstbpair, *uppair, *downpair ;
 stbpair_list *pts ;
 stbck *ck, *inputck, *run_inputck, *endnodeck, *endnodecksave[2] ;
 char *name ;
 char *cmd ;
 long delaymax ;
 long delaymin ;
 long delaym ;
 long periode ;
 long periodemax[256] ;
 long periodemin[256] ;
 long ckperiode, input_periode, output_periode ;
 long  find ;
 long setup ;
 long hold ;
 long s ;
 long h ;
 long ms ;
 long mh ;
 long ckrup_min ;
 long ckrup_max ;
 long ckrdn_min ;
 long ckrdn_max ;
 long ckup_min ;
 long ckup_max ;
 long ckdn_min ;
 long ckdn_max ;
 long ockup_min ;
 long ockup_max ;
 long ockdn_min ;
 long ockdn_max ;
 int i, k ;
 int j ;
 char u ;
 char v ;
 char ckedge ;
 char index ;
 char nodelay ;
 char specin ;
 char buf[1024] ;
 ptype_list *ptype;
 char chperiod ;
 char phaseused[256];
 int bestperiodindex ;
 int eventsetup, eventhold, transparence, has_multicycle;
 long periodeSetup, periodeHold;
 datamoved_type datamoved;
 int datamoved_start;
 EventPeriodeHold_type EventPeriodeHold[2];
 long ockperiod;
 int diff, free_tag, doonce;
 long updelta, downdelta;
 int involved_input_transistion[2][4];
 Extended_Setup_Hold_Computation_Data eshcd[2];
 Setup_Hold_Computation_Detail detail[2];
 stbpair_list *STBTAB[2][256], **pSTBTAB;
 stbpair_list *STBHZ[2][256], **pSTBHZ;
 inffig_list *ifl;
 int what, lim,specout, specoutend, falseaccessmode=0, falseslack_setup[2][2], falseslack_hold[2][2];
 long datamin, datamax;
 ptype_list *geneclockinfo;


 ifl=getloadedinffig(stbfig->FIG->INFO->FIGNAME);
 geneclockinfo=stb_build_generated_clock_info(stbfig->CLOCK);

 //----
 computeflags|=STB_DIFFERENTIATE_INPUT_EVENTS|STB_DIFFERENTIATE_INPUT_COMMANDS;
 //----

 if (!(computeflags & STB_DIFFERENTIATE_INPUT_EVENTS))
   computeflags&=~STB_DIFFERENTIATE_INPUT_COMMANDS;

 if (computeflags & STB_UPDATE_SETUP_HOLD)
   {
     for(i = 0 ; i < 2 ; i++)
       {
         event = sig->NODE + i ;
         node = stb_getstbnode(event) ;
         node->SETUP=node->HOLD=STB_NO_TIME;
       }
   }

 if ((stbfig->FLAGS & STB_HAS_FALSE_ACCESS)!=0 && V_BOOL_TAB[__STB_HANDLE_FALSE_ACCESS].VALUE)
 { 
   if(ttv_canbeinfalsepath(sig->NODE, 'o') || ttv_canbeinfalsepath(sig->NODE+1, 'o'))
   {
     if (ttv_hasaccessfalsepath(stbfig->FIG, sig->NODE, stbfig->CLOCK) || ttv_hasaccessfalsepath(stbfig->FIG, sig->NODE+1, stbfig->CLOCK))
       falseaccessmode=1;
   }
 }

 if (!((sig->TYPE & TTV_SIG_Q)==TTV_SIG_Q && V_BOOL_TAB[__STB_ENBALE_COMMAND_CHECK].VALUE==0))
   {

     if ((sig->TYPE & (TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R))!=0)
     {
       int mode=0;
       if ((sig->TYPE & TTV_SIG_R)!=0 || (sig->TYPE & TTV_SIG_LR)==TTV_SIG_LR || (sig->TYPE & TTV_SIG_LS)==TTV_SIG_LS) mode=2;
       chainsig=stb_debug_retreive_delays(stbfig, sig, 2, mode, falseaccessmode,computeflags);
     }
     else
       chainsig=NULL;

     if(chainsig == NULL)
       {
         nodelay = 'Y' ;
         chainsig = addchain(chainsig,sig) ;
       }
     else
       nodelay = 'N' ;

     specin = 'N' ;

     if((sig->TYPE & TTV_SIG_CT) != TTV_SIG_CT)
       {
         node = stb_getstbnode(sig->NODE) ;
         if(node->SPECIN != NULL)
           specin = 'Y' ;
         node = stb_getstbnode(sig->NODE+1) ;
         if(node->SPECIN != NULL)
           specin = 'Y' ;
         if((specin == 'Y') && (nodelay == 'N'))
           chainsig = addchain(chainsig,sig) ;
       }
     if ((sig->TYPE & (TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R))==0)
       chaincmd = NULL ;
     else 
       {
         if(((sig->TYPE & TTV_SIG_L) == TTV_SIG_L) && (STB_MULTIPLE_COMMAND == 'Y'))
           chaincmd = ttv_getlrcmd(stbfig->FIG,sig);
         else
           chaincmd = NULL ;
     
         if(chaincmd == NULL)
           chaincmd = addchain(NULL,NULL) ;
       }

     for(chainx = chaincmd ; chainx != NULL ; chainx = chainx->NEXT)
       {
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {         
             for (diff=0; diff<2; diff++)
               {
                 ptsig = (ttvsig_list *)chain->DATA ;
                 chperiod = 'N' ;
                 if (computeflags & STB_DIFFERENTIATE_INPUT_EVENTS)
                   {
                     node = stb_getstbnode(&ptsig->NODE[diff]);
                     if (!(computeflags & STB_DIFFERENTIATE_INPUT_COMMANDS))
                       if(node->CK == NULL)
                         node = stb_getstbnode(ttv_opposite_event(node->EVENT)) ;
                   }
                 else
                   {
                     node = stb_getstbnode(ptsig->NODE) ;
                     if(node->CK == NULL)
                       node = stb_getstbnode(ptsig->NODE+1) ;
                   }


                 sig1event=NULL;
                 if (computeflags & STB_DIFFERENTIATE_INPUT_EVENTS)
                   sig1event=&ptsig->NODE[diff];

                 if (node->CK==NULL || !(computeflags & STB_DIFFERENTIATE_INPUT_COMMANDS)) doonce=1; else doonce=0;

                 for (run_inputck=node->CK; doonce || run_inputck!=NULL; run_inputck=run_inputck->NEXT)
                   {
                     inputck=run_inputck;
                     if (!stb_checkvalidcommand(sig1event, inputck)) continue;
//                    if (getptype(ptsig->USER, TTV_SIG_CLOCK)!=NULL && run_inputck!=NULL && run_inputck->CMD!=NULL) continue;
                 
                     if((inputck != NULL) && (inputck->SUPMAX != STB_NO_TIME) && (inputck->SDNMAX != STB_NO_TIME))
                       {
                         ockup_min = inputck->SUPMIN ;
                         ockup_max = inputck->SUPMAX ;
                         ockdn_min = inputck->SDNMIN ;
                         ockdn_max = inputck->SDNMAX ;
                         ockperiod = inputck->PERIOD ;
                       }
                     else
                       {
                         for (i=0; i<node->NBINDEX; i++)
                           if (node->STBTAB[i]!=NULL)
                             {
                               inputck=stb_getclock(stbfig, i, NULL, &ckedge, NULL);
                               break;
                             }
                         if (inputck!=NULL)
                           {
                             ockup_min = inputck->SUPMIN ;
                             ockup_max = inputck->SUPMAX ;
                             ockdn_min = inputck->SDNMIN ;
                             ockdn_max = inputck->SDNMAX ;
                             ockperiod = inputck->PERIOD ;
                           }
                         else
                           {
                             ockup_min = STB_NO_TIME ;
                             ockup_max = STB_NO_TIME ;
                             ockdn_min = STB_NO_TIME ;
                             ockdn_max = STB_NO_TIME ;
                             ockperiod = STB_NO_TIME ;
                           }
                       }
                     // IIII
                     if (computeflags & STB_DIFFERENTIATE_INPUT_COMMANDS)
                       {
                         for( j=0; j<2; j++)
                           for (i=0; i<node->NBINDEX; i++)
                             {
                               STBHZ[j][i]=NULL, STBTAB[j][i]=NULL;
                             }
                         nodex = stb_getstbnode(&ptsig->NODE[0]);
                         if (getptype(ptsig->USER, STB_IS_CLOCK)!=NULL && (run_inputck==NULL || run_inputck->CMD==NULL))
                           create_clock_stability(nodex, STBTAB[0]);
                         else
                           stb_transferstbline (stbfig, nodex->EVENT, nodex , STBTAB[0], STBHZ[0], 0, 1, NULL, inputck, 1, (nodex->FLAG & STB_NODE_STABCORRECT)!=0?STB_TRANSFERT_CORRECTION:0) ;
                         nodex = stb_getstbnode(&ptsig->NODE[1]);
                         if (getptype(ptsig->USER, STB_IS_CLOCK)!=NULL && (run_inputck==NULL || run_inputck->CMD==NULL))
                           create_clock_stability(nodex, STBTAB[1]);
                         else
                           stb_transferstbline (stbfig, nodex->EVENT, nodex , STBTAB[1], STBHZ[1], 0, 1, NULL, inputck, 1, (nodex->FLAG & STB_NODE_STABCORRECT)!=0?STB_TRANSFERT_CORRECTION:0) ;
                       }
                     else
                       {
                         for (j=0; j<2; j++)
                           {
                             nodex = stb_getstbnode(&ptsig->NODE[j]);
                             for (i=0; i<node->NBINDEX; i++)
                               {
                                 if (nodex->STBHZ!=NULL) STBHZ[j][i]=nodex->STBHZ[i]; else STBHZ[j][i]=NULL;
                                 if (nodex->STBTAB!=NULL) STBTAB[j][i]=nodex->STBTAB[i]; else STBTAB[j][i]=NULL;
                               }
                           }
                       }
                     // OOOO
                     ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;

                     // ---------------------------------------------------------------------------------------                     
                     if (stb_getstbnode(sig->NODE)->SPECOUT!=NULL || stb_getstbnode(sig->NODE+1)->SPECOUT!=NULL)
                        specoutend=stb_getstbnode(sig->NODE)->NBINDEX;
                     else 
                        specoutend=0;

                     for (specout=-1; specout<specoutend; specout++)
                     {

                     for(i = 0 ; i < (int)stbfig->PHASENUMBER ; i++)
                       {
                         phaseused[i]=0;
                         for(j = 0 ; j < 2 ; j++)
                           {
                             tabpair[j][i] = NULL ;
                             tabhz[j][i] = NULL ;
                           }
                       }
                     if((sig->TYPE & TTV_SIG_R) == TTV_SIG_R)
                       {
                         i = 0 ;
                       }
                     else
                       {
                         i = 2 ;
                       }
                 
                     transparence=0; has_multicycle=0;
                 
                     for(j = 0 ; j < 2 ; j++)
                       {
                         EventPeriodeHold[j].total=EventPeriodeHold[j].period=0;
                         endnodecksave[j]=NULL;
                         for (k=0; k<4; k++) involved_input_transistion[j][k]=0;
                         useddelay[j]=NULL;
                         for (k=0; k<2; k++) falseslack_setup[j][k]=falseslack_hold[j][k]=0;
                       }
                     for (k=0; k<4; k++)
                       for (j=0; j<node->NBINDEX; j++)
                         datamoved[k][j]=0;
                 
                 
                     for(; i < 4 ; i++)
                       {
                         if (computeflags & STB_DIFFERENTIATE_INPUT_EVENTS)
                           {
                             if ((i % 2)!=diff) continue;
                           }
                     
                         ptevent = ptsig->NODE + (i % 2) ;
                         pSTBTAB=STBTAB[i %2];
                         pSTBHZ=STBHZ[i %2];
                         for(j = 0 ; j < 2 ; j++)
                           {
                             ttvevent_list *accesscmd, *accessclock;
                             event = sig->NODE + j ;
                             if ((sig->TYPE & TTV_SIG_B)!=0 && specout<0 && getptype(event->USER, STB_BREAK_TEST_EVENT)==NULL)
                                continue;

                             node = stb_getstbnode(event) ;
                             // -----------------------------
                             endnodeck=node->CK;
                             accesscmd=accessclock=NULL;
                             if (specout>=0 && (node->SPECOUT==NULL || node->SPECOUT[specout]==NULL)) continue;
                             if (inputck!=NULL && inputck->CMD!=NULL && falseaccessmode)
                               accesscmd=inputck->CMD, accessclock=inputck->ORIGINAL_CLOCK;
                             if(i < 2)
                               {
                                 delay = stb_getstbdelay(event,ptevent,NULL,TTV_FIND_HZ,accesscmd,accessclock) ;
                                 tab = tabhz ;
                                 datamoved_start=2;
                               }
                             else
                               {
                                 delay = stb_getstbdelay(event,ptevent,(ttvevent_list *)chainx->DATA,(long)0,accesscmd,accessclock) ;
                                 tab = tabpair ;
                                 datamoved_start=0;
                               }
                             if(delay != NULL)
                               {
                                 if(delay->DELAYMAX == TTV_NOTIME)
                                   delaymax = delay->DELAYMIN ;
                                 else
                                   delaymax = delay->DELAYMAX ;
                             
                                 if(delay->DELAYMIN == TTV_NOTIME)
                                   delaymin = delay->DELAYMAX ;
                                 else
                                   delaymin = delay->DELAYMIN ;
                             
                             
                                 if(delaymax < delaymin)
                                   {
                                     delaym = delaymin ;
                                     delaymin = delaymax ;
                                     delaymax = delaym ;
                                   }
                               }
                             else
                               {
                                 if((sig == ptsig) && (( nodelay == 'Y') || ( specin == 'Y')))
                                   {
                                     delaymin = (long)0 ;
                                     delaymax = (long)0 ;
                                   }
                                 else
                                   continue ;
                               }
                             if(specout==-1)
                              {
                               if (node->CK != NULL)
                               {
                                 if(delay != NULL)
                                   if(delay->CMD != NULL)
                                   {
//                                     stb_getstbck(node,delay->CMD) ;
                                     endnodeck=stb_findstbck(node,delay->CMD) ;
//                                     endnodeck=node->CK;
                                     if (endnodeck==NULL/*->CMD!=NULL && endnodeck->CMD!=delay->CMD*/) continue;
                                   }
                               }
                              }
                             else
                             {
                                char edge;
                                endnodeck=stb_getclock(stbfig, specout, NULL, &edge, NULL);
                                if (endnodeck==NULL && stbfig->CLOCK!=NULL) continue;
                             }
                             // falseslack check
                             if (delay!=NULL && stb_hasfalseslack(stbfig, event))
                             {
                               ttvevent_list *startclock, *endclocksetup, *endclockhold;
                               int fs=0, fh=0;
                             
                               stb_getslacktopclocks(NULL, node, i>=2?0:1, inputck, endnodeck, &startclock, &endclocksetup, &endclockhold);
                               if (startclock!=NULL && endclocksetup!=NULL)
                                 fs=stb_isfalseslack(stbfig, startclock, ptevent, event, endclocksetup, INF_FALSESLACK_SETUP);
                               
                               if (startclock!=NULL && endclockhold!=NULL)
                                 fh=stb_isfalseslack(stbfig, startclock, ptevent, event, endclocksetup, INF_FALSESLACK_HOLD);
                               if ((i<2 && (fs & INF_FALSESLACK_HZ)!=0) || (i>=2 && (fs & INF_FALSESLACK_NOTHZ)!=0)) falseslack_setup[j][i/2]=1;
                               if ((i<2 && (fh & INF_FALSESLACK_HZ)!=0) || (i>=2 && (fh & INF_FALSESLACK_NOTHZ)!=0)) falseslack_hold[j][i/2]=1;
                               if (falseslack_setup[j][i/2] && falseslack_hold[j][i/2])
                                 delay=NULL;
                             }
                             if((delay != NULL) || (sig == ptsig))
                               {
                                 nodex = stb_getstbnode(ptevent) ;
                                 input_periode=0;
                                 if (inputck!=NULL)
                                   {
                                     ck=stb_getclock(stbfig, inputck->CKINDEX, NULL, &ckedge, NULL);
                                     if (ck!=NULL) input_periode=ck->PERIOD;
                                   }
                                 else if (inputck!=NULL) input_periode=inputck->PERIOD;
                             
                                 for(u = 0 ; u < node->NBINDEX ; u++)
                                   {
                                     if(endnodeck != NULL)
                                       {
                                         ptdomain = stb_getstbdomain(stbfig->CKDOMAIN, endnodeck->CKINDEX) ;
                                     
                                         if(stb_cmpphase(stbfig, u,endnodeck->CKINDEX)>=0/*u >= endnodeck->CKINDEX*/)
                                           {
                                             if(specout==-1 && (u == endnodeck->CKINDEX) && 
                                                ((endnodeck->TYPE == STB_TYPE_LATCH) ||
                                                 (endnodeck->TYPE == STB_TYPE_EVAL) ||
                                                 (endnodeck->TYPE == STB_TYPE_PRECHARGE)))
                                               {                   
                                                 if((stbfig->STABILITYMODE & STB_STABILITY_FF) != 0)
                                                   {
                                                     //periode = endnodeck->PERIOD ;
                                                     periode = stb_synchronize_slopes(stbfig, u, endnodeck->CKINDEX, endnodeck->PERIOD, STB_DECIFEQUALPHASE);
                                                   }
                                                 else if((stbfig->STABILITYMODE & STB_STABILITY_LT) != 0)
                                                 {
                                               
                                                   //periode=0;
                                                   periode = stb_synchronize_slopes(stbfig, u, endnodeck->CKINDEX, endnodeck->PERIOD, 0);
                                                   transparence=1 ;
                                                 }
                                                 else if(*(STBTAB + u) != NULL)
                                                   {
                                                     continue ;
                                                   }
                                                 else
                                                 {
                                                   //periode = endnodeck->PERIOD ;
                                                   periode = stb_synchronize_slopes(stbfig, u, endnodeck->CKINDEX, endnodeck->PERIOD, STB_DECIFEQUALPHASE);
                                                 }
                                               }
                                             else
                                               {
                                                 //periode = endnodeck->PERIOD ;
                                                 periode = stb_synchronize_slopes(stbfig, u, endnodeck->CKINDEX, endnodeck->PERIOD, specout==-1?STB_DECIFEQUALPHASE:STB_SPECOUTMODE);
                                               }
                                           }
                                         else
                                           periode = 0 ;
                                         if(endnodeck->CKINDEX != STB_NO_INDEX)
                                           v = endnodeck->CKINDEX ;
                                         else
                                           v = u ;
                                       }
                                     else
                                       {
                                         periode = 0 ;
                                         v = u ;
                                       }

                                     output_periode=0;
                                     if (endnodeck!=NULL)
                                       {
                                         ck=stb_getclock(stbfig, endnodeck->CKINDEX, NULL, &ckedge, NULL);
                                         if (ck!=NULL) output_periode=ck->PERIOD;
                                       }
            
                                     stb_getmulticycleperiod(nodex->EVENT, node->EVENT, input_periode, output_periode, &periodeSetup, &periodeHold, &EventPeriodeHold[j].nb_setup_cycle, &EventPeriodeHold[j].nb_hold_cycle);
                                     EventPeriodeHold[j].mc_setup_period=periodeSetup;
                                     EventPeriodeHold[j].mc_hold_period=periodeHold;

                                     periodemax[(int)u] = (long)0 ;
                                     periodemin[(int)u] = (long)0 ;
                                     if(delay != NULL)
                                       {
                                         if(delay->PHASELISTMAX != NULL)
                                           periodemax[(int)u] = stb_calperiode(stbfig,u,node,
                                                                               delay->PHASELISTMAX,v,periode) ;
                                         if(delay->PHASELISTMIN != NULL)
                                           periodemin[(int)u] = stb_calperiode(stbfig,u,node,
                                                                               delay->PHASELISTMIN,v,periode) ;
                                         if (periodemin[(int)u]!=periodemax[(int)u])
                                           periodemin[(int)u]=periodemax[(int)u];
                                       }

                                     periodemax[(int)u]+=periodeSetup;
                                     periodemin[(int)u]+=periodeSetup;

                                     if ((endnodeck != NULL && endnodeck->CKINDEX != STB_NO_INDEX)
                                         && (u < ptdomain->CKMIN || u > ptdomain->CKMAX)) 
                                       { 
                                         if (*(pSTBTAB + u) != NULL)
                                           {
                                           }
                                       }
                                     else
                                       {
                                         if(sig != ptsig)
                                           {
                                             long crossmin, crossmax;
                                             ptstbpair = stb_dupstbpairlist(*(pSTBTAB + u)) ;

                                             stb_assign_phase_to_stbpair(ptstbpair, u);
                                             crossmin=stb_getcross_PP(delay->STARTMIN_PP, u);
                                             crossmax=stb_getcross_PP(delay->STARTMAX_PP, u);
                                             ptstbpair=stb_clippair(ptstbpair, crossmin, crossmax);
                                             if(( crossmin != TTV_NOTIME || crossmax!=TTV_NOTIME) && (ptstbpair != NULL))
                                               useddelay[j]=delay;

                                             if((periodemax[(int)u] + periode) > 0 && ptstbpair!=NULL)
                                               chperiod = 'Y', datamoved[datamoved_start+j][(int)u]=-(periodemax[(int)u] + periode) ;
                                             tab[j][(int)v] = stb_transferstbpair(ptstbpair,tab[j][(int)v],
                                                                                  delaymax-periodemax[(int)u],
                                                                                  delaymin-periodemin[(int)u],
                                                                                  periode, periode) ;
                                             if (periodeSetup>0) has_multicycle=1;
                                             if (periodeHold>=EventPeriodeHold[j].total) 
                                               EventPeriodeHold[j].total=periodeHold, EventPeriodeHold[j].period=output_periode;
                                           }
                                         else ptstbpair = NULL;
                                         if((sig == ptsig) && ( specin == 'Y') && (nodex->SPECIN != NULL) && (i & 1)==j)
                                           {
                                             ptstbpair = stb_dupstbpairlist(*(nodex->SPECIN + u)) ;
                                             stb_assign_phase_to_stbpair(ptstbpair, u);
                                             tab[j][(int)v] = stb_transferstbpair(ptstbpair,tab[j][(int)v],
                                                                                  (long)0,(long)0,periode+periodeSetup, periode+periodeSetup) ;
                                             if (ptstbpair!=NULL) datamoved[datamoved_start+j][(int)u]=-(periode+periodeSetup);
                                           }
                                         if (ptstbpair!=NULL)
                                           {
                                             phaseused[(int)u]=1;
                                             involved_input_transistion[j][i]=1;
                                           }
                                       }
                                   }
                               }
                             endnodecksave[j]=endnodeck;
                           }
                       }
   
                     if ((sig->TYPE & TTV_SIG_R)==TTV_SIG_R) lim=2; else lim=1;
                     free_tag=0;
                     for (what=1; what<=lim; what++)
                     {
                       setup = STB_NO_TIME ;
                       hold = STB_NO_TIME ;
                       eventsetup=eventhold=0;
                       for(i = 0 ; i < 2 ; i++)
                         {
                           event = sig->NODE + i ;
                           node = stb_getstbnode(event) ;

                           stb_compute_common_clock_gap(ifl, stbfig, ptsig, inputck, node, involved_input_transistion[i], STBTAB, tabpair[i], tabhz[i], datamoved, &eshcd[i], NULL, 0, geneclockinfo);

                           if((sig->TYPE & TTV_SIG_R) == TTV_SIG_R)
                             {
                               stb_calcsetuphold(stbfig,tabpair[i],tabhz[i],node,&s,&h,0,EventPeriodeHold[i].total, &eshcd[i], &detail[i],what, endnodecksave[i], specout) ;
                             }
                           else
                             {
                               stb_calcsetuphold(stbfig,tabpair[i],NULL,node,&s,&h,0,EventPeriodeHold[i].total, &eshcd[i], &detail[i],0, endnodecksave[i], specout) ;
                             }
                           stb_assign_operations(i, detail, datamoved, EventPeriodeHold);
                           stb_add_infos(stbfig, event, i, detail, inputck, sig1event, useddelay);

                           if (falseslack_setup[i][2-what]) s=detail[i].setup.VALUE=STB_NO_TIME;
                           if (falseslack_hold[i][2-what]) h=detail[i].hold.VALUE=STB_NO_TIME;
                           if ((computeflags & STB_REMOVE_LAGGING_PRECHARGES)!=0 && (ptsig->TYPE & TTV_SIG_R)!=0 && TTV_MaxPathPeriodPrecharge>0)
                           {
                              if (sig1event!=NULL)
                              {
                                stb_getstbdelta(stbfig, sig1event, &datamin, &datamax, NULL);
                                if (datamax>0) s=detail[i].setup.VALUE=STB_NO_TIME;
                                if (datamin>0) h=detail[i].hold.VALUE=STB_NO_TIME;
                              }
                           }
                           if((node->FLAG & STB_NODE_NOSETUP)==0 && s < setup ) { setup = s ; eventsetup=i; }
                           if((node->FLAG & STB_NODE_NOHOLD)==0 && h < hold) { hold = h ; eventhold=i; }
                           if (computeflags & STB_UPDATE_SETUP_HOLD)
                             {
                               if ((node->FLAG & STB_NODE_NOSETUP)==0 && (node->SETUP==STB_NO_TIME || node->SETUP>s)) node->SETUP=s;
                               if ((node->FLAG & STB_NODE_NOHOLD)==0 && (node->HOLD==STB_NO_TIME || node->HOLD>h)) node->HOLD=h;
                             }
                           if ((node->FLAG & STB_NODE_NOSETUP)!=0 || (computeflags & STB_COMPUTE_HOLD_ONLY)!=0) detail[i].setup.VALUE=STB_NO_TIME;
                           if ((node->FLAG & STB_NODE_NOHOLD)!=0 || (computeflags & STB_COMPUTE_SETUP_ONLY)!=0) detail[i].hold.VALUE=STB_NO_TIME;
                         }

                       if((setup!=STB_NO_TIME && setup <= margin) || (hold!=STB_NO_TIME && hold <= margin))
                         {
                           node = stb_getstbnode(sig->NODE) ;
                           nodex = stb_getstbnode(sig->NODE+1) ;
                           name = NULL ;
                           if((endnodecksave[0] != NULL) && (endnodecksave[0]->CKINDEX != STB_NO_INDEX))
                             {
                               ck = stb_getclock(stbfig, endnodecksave[0]->CKINDEX, NULL, &ckedge, endnodecksave[0]) ;

                               ckrup_min = ck->SUPMIN ;
                               ckrup_max = ck->SUPMAX ;
                               ckrdn_min = ck->SDNMIN ;
                               ckrdn_max = ck->SDNMAX ;

                               if((chainx->DATA != endnodecksave[0]->CMD) && (nodex->CK != NULL))
                                 {
                                   stb_get_setup_and_hold_slopes_delta(endnodecksave[1], &updelta, &downdelta);
                                   bestperiodindex=getbestperiod(phaseused, periodemax, (int)stbfig->PHASENUMBER);
                                   //                         if (bestperiodindex!=STB_NO_INDEX) updelta+=periodemax[bestperiodindex], downdelta+=periodemax[bestperiodindex];

                                   ckup_min = endnodecksave[1]->SUPMIN + updelta;
                                   ckup_max = endnodecksave[1]->SUPMAX + updelta;
                                   ckdn_min = endnodecksave[1]->SDNMIN + downdelta;
                                   ckdn_max = endnodecksave[1]->SDNMAX + downdelta;
                                 }
                               else
                                 {
                                   stb_get_setup_and_hold_slopes_delta(endnodecksave[0], &updelta, &downdelta);
                                   bestperiodindex=getbestperiod(phaseused, periodemax, (int)stbfig->PHASENUMBER);
                                   //                         if (bestperiodindex!=STB_NO_INDEX) updelta+=periodemax[bestperiodindex], downdelta+=periodemax[bestperiodindex];

                                   ckup_min = endnodecksave[0]->SUPMIN + updelta;
                                   ckup_max = endnodecksave[0]->SUPMAX + updelta;
                                   ckdn_min = endnodecksave[0]->SDNMIN + downdelta;
                                   ckdn_max = endnodecksave[0]->SDNMAX + downdelta;
                                 }

                               index = ck->CKINDEX ;
       
                               if (eventsetup==0 || nodex->CK==NULL) 
                                 ms = endnodecksave[0]->SETUP ;
                               else
                                 ms = endnodecksave[1]->SETUP ;
                               if (eventhold==0 || nodex->CK==NULL)
                                 mh = endnodecksave[0]->HOLD ;
                               else
                                 mh = endnodecksave[1]->HOLD ;
                             }
                           else
                             {
                               updelta=downdelta=0;
                               ms = 0 ;
                               mh = 0 ;
                               ck = NULL ;
                               for(u = 0 ; u < node->NBINDEX ; u++)
                                 {
                                   if(node->SPECOUT != NULL)
                                     if(node->SPECOUT[(int)u] != NULL)
                                       ck = stb_getclock(stbfig, u , NULL, &ckedge, NULL) ;
                                 }
                               if(ck == NULL)
                                 ck = stb_getclock(stbfig, (char)0 , NULL, &ckedge, NULL) ;
                               if(ck != NULL)
                                 {
                                   ckrup_min = ck->SUPMIN ;
                                   ckrup_max = ck->SUPMAX ;
                                   ckrdn_min = ck->SDNMIN ;
                                   ckrdn_max = ck->SDNMAX ;
                                   index = ck->CKINDEX ;
                                 }
                               else
                                 {
                                   ckrup_min = STB_NO_TIME ;
                                   ckrup_max = STB_NO_TIME ;
                                   ckrdn_min = STB_NO_TIME ;
                                   ckrdn_max = STB_NO_TIME ;
                                   index = STB_NO_INDEX ;
                                 }
                               ckup_min = STB_NO_TIME ;
                               ckup_max = STB_NO_TIME ;
                               ckdn_min = STB_NO_TIME ;
                               ckdn_max = STB_NO_TIME ;
                               bestperiodindex = STB_NO_INDEX;
                             }
                           if(ck != NULL)
                             {
                               name = stb_getckname(stbfig,ck->CKINDEX, endnodecksave[0]) ;
                               ckperiode = ck->PERIOD ;
                             }
                           else
                             {
                               ckperiode = stbfig->CLOCKPERIOD ; 
                               name = NULL ;
                             }

                           if(chainx->DATA != NULL)
                             {
                               ttv_getsigname(stbfig->FIG,buf,((ttvevent_list*)chainx->DATA)->ROOT) ;
                               cmd = mbkalloc(strlen(buf) + 1) ;
                               strcpy(cmd,buf) ;
                             }
                           else cmd = NULL ;

                           if (bestperiodindex!=STB_NO_INDEX && EventPeriodeHold[eventhold].total==periodemax[bestperiodindex]) has_multicycle=0;

                           debug = stb_addstbdebug(debug,ptsig,sig1event,sig,ckperiode,name,cmd,(ttvevent_list *)chainx->DATA,
                                                   ckrup_min,
                                                   ckrup_max,
                                                   ckrdn_min,
                                                   ckrdn_max,
                                                   ckup_min,
                                                   ckup_max,
                                                   ckdn_min,
                                                   ckdn_max,
                                                   ockup_min,
                                                   ockup_max,
                                                   ockdn_min,
                                                   ockdn_max,
                                                   ockperiod,
                                                   ms,mh,setup,hold,has_multicycle?EventPeriodeHold[eventhold].total+EventPeriodeHold[eventhold].period:0,
                                                   updelta,downdelta,
                                                   detail,
                                                   inputck==NULL || doonce?NULL:inputck->CMD,
                                                   inputck==NULL?NULL:inputck->ORIGINAL_CLOCK
                                                   ) ;


                           if (computeflags & STB_COMPUTE_DEBUG_CHRONO)
                             {

                               if((sig->TYPE & TTV_SIG_R) == TTV_SIG_R)
                                 {
                                   if (what!=2)
                                   {
                                     stb_addprechrono(stbfig,debug,tabpair[0],node->SPECOUT,
                                                      node,STB_DN,ptsig,periodemax,periodemin,bestperiodindex,transparence) ;
                                     stb_addprechrono(stbfig,debug,tabpair[1],nodex->SPECOUT,
                                                      nodex,STB_UP,ptsig,periodemax,periodemin,bestperiodindex,transparence) ;
                                   }
                                   if (what!=1)
                                   {
                                     stb_addprechrono(stbfig,debug,tabhz[0],NULL,node,STB_DN|STB_HZ,ptsig,periodemax,periodemin,bestperiodindex,transparence) ;
                                     stb_addprechrono(stbfig,debug,tabhz[1],NULL,nodex,STB_UP|STB_HZ,ptsig,periodemax,periodemin,bestperiodindex,transparence) ;
                                   }
                                   free_tag|=1;
                                 }
                               else
                                 {
                                   if((endnodecksave[0] == NULL) && (index != STB_NO_INDEX))
                                     {
                                       periode = stb_getperiod(stbfig, index);
                                       for(u = index ; u < node->NBINDEX ; u++)
                                         if((tabpair[0][(int)u] != NULL) || (tabpair[1][(int)u] != NULL))
                                           {
                                             chperiod = 'Y';                                   
                                             /*                                   debug->DATAMOVED[0]-=periode;
                                                                                  debug->DATAMOVED[1]-=periode;*/
                                           }
                                     }
                                   else
                                     periode = 0 ;

                                   stb_copystbtabpair(tabpair[0],NULL,periode,periodemax,periodemin,
                                                      node->NBINDEX,index,bestperiodindex,0) ;
                                   stb_copystbtabpair(tabpair[1],NULL,periode,periodemax,periodemin,
                                                      node->NBINDEX,index,bestperiodindex,0) ;

                                   uppair=stb_dup_and_merge_if_needed(stbfig, tabpair[1], node->NBINDEX);
                                   downpair=stb_dup_and_merge_if_needed(stbfig, tabpair[0], node->NBINDEX);
                                   ptstbpair = stb_globalstbtab(stb_mergestbtab(tabpair[0],
                                                                                tabpair[1],node->NBINDEX),
                                                                node->NBINDEX) ;

                                   if(ptstbpair == NULL)
                                     {
                                       for(i = 0 ; i < 2 ; i++)
                                         stb_freestbtabpair(tabpair[i],node->NBINDEX) ;
                                       continue ;
                                     }

                                   if((stbfig->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
                                     {
                                       pts = stb_globalstbpair(ptstbpair) ;
                                       stb_freestbpair(ptstbpair) ;
                                       ptstbpair = pts ;
                                     }
                                   if(endnodecksave[0] != NULL)
                                     debug->CHRONO = stb_addstbchrono(debug->CHRONO,endnodecksave[0]->VERIF,
                                                                      endnodecksave[0]->TYPE,STB_UP|STB_DN,
                                                                      ckup_min,ckup_max,
                                                                      ckdn_min,ckdn_max) ;
                                   else
                                     debug->CHRONO = stb_addstbchrono(debug->CHRONO,(char)0,
                                                                      (char)0,STB_UP|STB_DN,
                                                                      ckup_min,ckup_max,
                                                                      ckdn_min,ckdn_max) ;
                                   debug->CHRONO->SIG2S = ptstbpair ;

                                   debug->CHRONO->SIG2S_U=uppair;
                                   debug->CHRONO->SIG2S_D=downpair;

                                   for(i = 0 ; i < 2 ; i++)
                                     stb_freestbtabpair(tabpair[i],node->NBINDEX) ;

                                   if((node->SPECOUT != NULL) || (nodex->SPECOUT != NULL))
                                     {
                                       if (index == STB_NO_INDEX) periode = STB_NO_TIME;
                                       else
                                         {
                                           periode = stb_getperiod(stbfig, index);
                                           if(chperiod == 'N')
                                             periode = 0 ;
                                         }
                                       stb_copystbtabpair(node->SPECOUT,tabpair[0],periode,periodemax,periodemin,
                                                          node->NBINDEX,index,bestperiodindex,0) ;
                                       stb_copystbtabpair(nodex->SPECOUT,tabpair[1],periode,periodemax,periodemin,
                                                          nodex->NBINDEX,index,bestperiodindex,0) ;

                                       uppair=stb_dup_and_merge_if_needed(stbfig, tabpair[1], node->NBINDEX);
                                       downpair=stb_dup_and_merge_if_needed(stbfig, tabpair[0], node->NBINDEX);
                                       ptstbpair = stb_globalstbtab(stb_mergestbtab(tabpair[0],
                                                                                    tabpair[1],node->NBINDEX),
                                                                    node->NBINDEX) ;

                                       if((stbfig->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
                                         {
                                           pts = stb_globalstbpair(ptstbpair) ;
                                           stb_freestbpair(ptstbpair) ;
                                           ptstbpair = pts ;
                                         }
                                       debug->CHRONO->SPECS = ptstbpair ;
                                       debug->CHRONO->SPECS_U=uppair;
                                       debug->CHRONO->SPECS_D=downpair;
        
                                       for(i = 0 ; i < 2 ; i++)
                                         stb_freestbtabpair(tabpair[i],node->NBINDEX) ;
                                     }
        
                                   node = stb_getstbnode(ptsig->NODE) ;
                                   nodex = stb_getstbnode(ptsig->NODE+1) ;
                                   if (index == STB_NO_INDEX) periode = STB_NO_TIME;
                                   else
                                     {
                                       periode = stb_getperiod(stbfig, index);
                                       if(chperiod == 'N')
                                         periode = 0 ;
                                     }
                                   if((sig == ptsig) && ( specin == 'Y') && (nodex->SPECIN != NULL))
                                     {
                                       stb_copystbtabpair(node->SPECIN,tabpair[0],periode,NULL,NULL,
                                                          node->NBINDEX,index,bestperiodindex,transparence) ;
                                       stb_copystbtabpair(nodex->SPECIN,tabpair[1],periode,NULL,NULL,
                                                          nodex->NBINDEX,index,bestperiodindex,transparence) ;
                                     }
                                   else
                                     {
                                       stb_copystbtabpair(STBTAB[0],tabpair[0],periode,NULL,NULL,
                                                          node->NBINDEX,index,bestperiodindex,transparence) ;
                                       stb_copystbtabpair(STBTAB[1],tabpair[1],periode,NULL,NULL,
                                                          nodex->NBINDEX,index,bestperiodindex,transparence) ;
                                     }

                                   if ((computeflags & STB_DIFFERENTIATE_INPUT_EVENTS)==0 || diff==1)
                                     uppair=stb_dup_and_merge_if_needed(stbfig, tabpair[1], node->NBINDEX);
                                   else
                                     uppair=NULL;
                                   if ((computeflags & STB_DIFFERENTIATE_INPUT_EVENTS)==0 || diff==0)
                                     downpair=stb_dup_and_merge_if_needed(stbfig, tabpair[0], node->NBINDEX);
                                   else
                                     downpair=NULL;
                                   ptstbpair = stb_globalstbtab(stb_mergestbtab(tabpair[0],
                                                                                tabpair[1],node->NBINDEX),
                                                                node->NBINDEX) ;

                                   if((stbfig->ANALYSIS & STB_GLOB_ANALYSIS) == STB_GLOB_ANALYSIS)
                                     {
                                       pts = stb_globalstbpair(ptstbpair) ;
                                       stb_freestbpair(ptstbpair) ;
                                       ptstbpair = pts ; 
                                     }
                                   debug->CHRONO->SIG1S = ptstbpair ;
                                   debug->CHRONO->SIG1S_U=uppair;
                                   debug->CHRONO->SIG1S_D=downpair;
                                 }
                             }
                         }
                     }
                     if(!free_tag)
                       {
                         for(i = 0 ; i < 2 ; i++)
                           {
                             stb_freestbtabpair(tabpair[i],node->NBINDEX) ;
                             stb_freestbtabpair(tabhz[i],node->NBINDEX) ;
                           }
                       }
                     // ---------------------------------------------------------------------------------------
                   }
                     if (computeflags & STB_DIFFERENTIATE_INPUT_COMMANDS)
                       {
                         for (j=0; j<2; j++)
                           {
                             stb_freestbtabpair(STBHZ[j], node->NBINDEX);
                             stb_freestbtabpair(STBTAB[j], node->NBINDEX);
                           }
                       }

                     if (doonce) break;
                   } // start cmd
                 if ((computeflags & STB_DIFFERENTIATE_INPUT_EVENTS)==0) break;
               } // diff
           }
       }
     freechain(chaincmd) ;

     freechain(chainsig) ;

     stb_getstbck(stb_getstbnode(sig->NODE), NULL);
     stb_getstbck(stb_getstbnode(sig->NODE+1), NULL);
     stb_delstbdelay(sig->NODE) ;
     stb_delstbdelay(sig->NODE+1) ;
   }

 // ----------- Directives check ---------

 for(i = 0 ; i < 2 ; i++)
   {
     event = sig->NODE + i ;
     node = stb_getstbnode(event) ;
     
     debug=stb_calc_directive_setuphold(stbfig,debug, node, margin, computeflags,geneclockinfo) ;
   }

 stb_free_built_generated_clock_info(geneclockinfo);
 return(debug) ;
}
