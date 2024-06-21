/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_transfer.c                                              */
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
#include "stb_transfer.h"
#include "stb_error.h"
#include "stb_directives.h"
#include "stb_falseslack.h"
#include "stb_relaxation_correction.h"

stbpair_list *stb_clockmaskstblist_simple (stbpair_list *list, long slopeopen, long slopeclose);
/*****************************************************************************
*                           fonction stb_getstblatchclose()                 *
*****************************************************************************/
void stb_getstblatchclose(stbfig_list *stbfig, ttvevent_list *latch, ttvevent_list *cmd, long *closemin, long *closemax, ttvevent_list **clockevent) 
{
    stbnode      *node;
    stbck        *clock = NULL;
    stbck        *cclock ; 

    if(closemin != NULL)
      *closemin = (long)-1;
    if(closemax != NULL)
      *closemax = (long)-1;
    *clockevent = NULL ;

    if ((node = stb_getstbnode (latch))) {
        clock = node->CK;
    }
    else
     return ;

   for(; clock != NULL ; clock = clock->NEXT)
     if((clock->CMD == cmd) || (cmd == NULL) || (clock->CMD == NULL)) {
        if(clock->CKINDEX == STB_NO_INDEX)
           continue ;
        cclock = stb_getclocknode(stbfig, clock->CKINDEX, NULL, clockevent, clock) ;
        if((STB_OPEN_LATCH_PHASE == 'Y') && 
           (((latch->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) || 
            ((latch->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)))
          {
           if(((*clockevent)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
              *clockevent = (*clockevent)->ROOT->NODE ;
           else
              *clockevent = (*clockevent)->ROOT->NODE + 1 ;
          }
        if(((*clockevent)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          {
           if(closemax != NULL)
             *closemax = clock->SUPMAX - cclock->SUPMAX ;
           if(closemin != NULL)
             *closemin = clock->SUPMIN - cclock->SUPMIN ;
          }
        else
          {
           if(closemax != NULL)
             *closemax = clock->SDNMAX - cclock->SDNMAX ;
           if(closemin != NULL)
             *closemin = clock->SDNMIN - cclock->SDNMIN ;
          }
   }
}

/*****************************************************************************
*                           fonction stb_getstblatchaccess()                 *
*****************************************************************************/
void stb_getstblatchaccess(stbfig_list *stbfig, ttvevent_list *latch, ttvevent_list *cmd, long *accessmin, long *accessmax, ttvevent_list **clockevent) 
{
    stbnode      *node;
    stbck        *clock = NULL;
    stbck        *cclock ; 

    if(accessmin != NULL)
      *accessmin = (long)-1;
    if(accessmax != NULL)
      *accessmax = (long)-1;
    *clockevent = NULL ;

    if ((node = stb_getstbnode (latch))) {
        clock = node->CK;
    }
    else
     return ;

   for(; clock != NULL ; clock = clock->NEXT)
     if((clock->CMD == cmd) || (cmd == NULL) || (clock->CMD == NULL)) {
        if(clock->CKINDEX == STB_NO_INDEX)
           continue ;
        cclock = stb_getclocknode(stbfig, clock->CKINDEX, NULL, clockevent, clock) ;
        if((STB_OPEN_LATCH_PHASE == 'N') && 
           (((latch->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) || 
            ((latch->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)))
          {
           if(((*clockevent)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
              *clockevent = (*clockevent)->ROOT->NODE ;
           else
              *clockevent = (*clockevent)->ROOT->NODE + 1 ;
          }
        if(((*clockevent)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          {
           if(accessmax != NULL)
             *accessmax = clock->SUPMAX - cclock->SUPMAX ;
           if(accessmin != NULL)
             *accessmin = clock->SUPMIN - cclock->SUPMIN ;
          }
        else
          {
           if(accessmax != NULL)
             *accessmax = clock->SDNMAX - cclock->SDNMAX ;
           if(accessmin != NULL)
             *accessmin = clock->SDNMIN - cclock->SDNMIN ;
          }
   }
}

/*****************************************************************************
*                           fonction stb_getstbdelta()                       *
*****************************************************************************/
static inline void stb_getstbdelta_sub(stbfig_list *stbfig, ttvevent_list *latch, long *deltamin, long *deltamax, ttvevent_list *cmd, int docorrect) 
{
    stbnode      *node;
    stbpair_list *pair = NULL, *mpair;
    stbck        *clock = NULL, *ck;
    long          supmin, supmax;
    long          sdnmin, sdnmax;
    stbpair_list *STBTAB[256];
    stbpair_list *STBHZ[256]; 
    int i, doonce=0;

    *deltamax = 0;
    *deltamin = 0;

    if ((node = stb_getstbnode (latch))) {
        clock = node->CK;
//        if (clock && (latch->ROOT->TYPE & TTV_SIG_L)!=0 && (clock->VERIF & STB_VERIF_IS_TRANSPARENT)==0) return;
        if (node->STBTAB!=NULL)
        {
          if (cmd==NULL)
              pair = stb_globalstbtab (node->STBTAB, node->NBINDEX);
          else
          {
             for (i=0; i<node->NBINDEX; i++)
             {
                 STBHZ[i]=NULL, STBTAB[i]=NULL;
             }
             for(ck=clock; ck != NULL && ck->CMD!=cmd; ck = ck->NEXT) ;
             if ((node->FLAG & STB_NODE_STABCORRECT)!=0) docorrect=1;
             stb_transferstbline (stbfig, node->EVENT, node , STBTAB, STBHZ, 0, 1, NULL, ck, 1, docorrect?STB_TRANSFERT_CORRECTION:0) ;
             pair = stb_globalstbtab (STBTAB, node->NBINDEX);
             stb_freestbtabpair(STBHZ, node->NBINDEX);
             stb_freestbtabpair(STBTAB, node->NBINDEX);
             if (ck!=NULL) clock=ck, doonce=1;
          }
        }
        else
          pair = NULL;
    }
    
    if (pair && clock) {
      for (mpair=pair; mpair!=NULL && mpair->NEXT!=NULL; mpair=mpair->NEXT) ;
      for(; clock != NULL ; clock = clock->NEXT)
        {
          if (clock->CMD!=NULL && stbfig!=NULL && !stb_cmd_can_generate_event(stbfig, latch, clock->CMD)) continue;
          if (cmd!=NULL && clock->CMD!=NULL && clock->CMD!=cmd) continue;
          if ((clock->ACTIVE & STB_STATE_UP) == STB_STATE_UP) {
            
            supmax = clock->SUPMAX;
            supmin = clock->SUPMIN;
            if ((clock->SUPMAX > clock->SDNMAX) && (STB_OPEN_LATCH_PHASE == 'N')) {
              supmax -= clock->PERIOD;
              supmin -= clock->PERIOD;
            }
            
            if ((mpair->U > supmax + clock->ACCESSMAX) && 
                ((mpair->U - supmax - clock->ACCESSMAX) > *deltamax)) 
              *deltamax = mpair->U - supmax - clock->ACCESSMAX;
            if ((pair->D > supmin + clock->ACCESSMIN) &&
                ((pair->D - supmin - clock->ACCESSMIN) > *deltamin))
              *deltamin = pair->D - supmin - clock->ACCESSMIN;
          }
          else
            if ((clock->ACTIVE & STB_STATE_DN) == STB_STATE_DN) {
              
              sdnmax = clock->SDNMAX;
              sdnmin = clock->SDNMIN;
              if ((clock->SDNMAX > clock->SUPMAX) && (STB_OPEN_LATCH_PHASE == 'N')) {
                sdnmax -= clock->PERIOD;
                sdnmin -= clock->PERIOD;
              }
              
              if ((mpair->U > sdnmax + clock->ACCESSMAX) && 
                  ((mpair->U - sdnmax - clock->ACCESSMAX) > *deltamax)) 
                *deltamax = mpair->U - sdnmax - clock->ACCESSMAX;
              if ((pair->D > sdnmin + clock->ACCESSMIN) &&
                  ((pair->D - sdnmin - clock->ACCESSMIN) > *deltamin))
                *deltamin = pair->D - sdnmin - clock->ACCESSMIN;
            }
          if (doonce) break;
        }
    }

    stb_freestbpair (pair);
/*    if (V_BOOL_TAB[__STB_STABILITY_CORRECTION].VALUE && (node->FLAG & STB_NODE_STABCORRECT)==0 && (*deltamin>0 || *deltamax>0))
    {
      node->FLAG|=STB_NODE_STABCORRECT;
      stb_getstbdelta_sub(stbfig, latch, deltamin, deltamax, cmd, 1) ;
    }*/
            
}

void stb_getstbdelta(stbfig_list *stbfig, ttvevent_list *latch, long *deltamin, long *deltamax, ttvevent_list *cmd) 
{
   stbnode *node;
   stbck *ck;
   long dmin, dmax;
   if (stbfig==NULL)
   {
     *deltamin=*deltamax=0;
   }
   else if (cmd!=NULL) stb_getstbdelta_sub(stbfig, latch, deltamin, deltamax, cmd, 0);
   else
   {
     node = stb_getstbnode (latch);
     *deltamin=TTV_NOTIME;
     *deltamax=0;
      if (node!=NULL)
      {
        for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
        {
          stb_getstbdelta_sub(stbfig, latch, &dmin, &dmax, ck->CMD, 0);
          if (dmax>*deltamax) *deltamax=dmax;
          if (*deltamin==TTV_NOTIME || dmin<*deltamin) *deltamin=dmin;
        }
      }
      if (*deltamin==TTV_NOTIME) *deltamin=0;
   }
}

/*****************************************************************************
*                           fonction stb_interstbpairlist()                  *
*****************************************************************************/
int
stb_interstbpairlist (list1, list2)
     stbpair_list *list1;
     stbpair_list *list2;
{
 stbpair_list *pt1;
 stbpair_list *pt2;

 for (pt1 = list1; pt1; pt1 = pt1->NEXT)
  for (pt2 = list2; pt2; pt2 = pt2->NEXT)
   if (stb_interstbpair (pt1, pt2) == 1)
    return (1);

 return (0);
}

/*****************************************************************************
*                           fonction stb_globalstbtab()                      *
*****************************************************************************/
stbpair_list *
stb_globalstbtab (tab, size)
     stbpair_list **tab;
     char size;
{
 stbpair_list *ptstbpair;
 stbpair_list *ptres = NULL;
 int i;

 for (i = 0; i < (int) size; i++)
  {
   ptstbpair = stb_dupstbpairlist (tab[i]);
   ptres = stb_mergestbpairlist (ptres, ptstbpair);
  }

 return (ptres);
}


/*****************************************************************************
*                           fonction stb_globalstbpairnode()                 *
*****************************************************************************/
stbpair_list *
stb_globalstbpairnode (node)
     stbnode *node;
{
 return (stb_globalstbtab (node->STBTAB, node->NBINDEX));
}

/*****************************************************************************
*                           fonction stb_interstbpair()                     *
*****************************************************************************/
int
stb_interstbpair (elm1, elm2)
     stbpair_list *elm1;
     stbpair_list *elm2;
{
 if ((elm2->D > elm1->U) || (elm1->D > elm2->U))
  return (0);
 else
  return (1);
}

/*****************************************************************************
*                           fonction stb_compstbpairlist()                   *
*****************************************************************************/
int
stb_compstbpairlist (list1, list2)
     stbpair_list *list1;
     stbpair_list *list2;
{
 stbpair_list *pt1;
 stbpair_list *pt2;

 for (pt1 = list1, pt2 = list2; pt1 && pt2; pt1 = pt1->NEXT, pt2 = pt2->NEXT)
  {
   if ((pt1->D != pt2->D) || (pt1->U != pt2->U))
    break;
  }

 if (pt1 || pt2)
  return (0);
 else
  return (1);
}

/*****************************************************************************
*                           fonction stb_beforestbpair()                     *
*****************************************************************************/
stbpair_list *
stb_beforestbpair (list, elm)
     stbpair_list *list;
     stbpair_list *elm;
{
 stbpair_list *ptstbpair;
 stbpair_list *prev = NULL;

 ptstbpair = list;
 while (ptstbpair->U < elm->D)
  {
   prev = ptstbpair;
   ptstbpair = ptstbpair->NEXT;
   if (ptstbpair == NULL)
    break;
  }

 return (prev);
}

/*****************************************************************************
*                           fonction stb_afterstbpair()                      *
*****************************************************************************/
stbpair_list *
stb_afterstbpair (list, elm)
     stbpair_list *list;
     stbpair_list *elm;
{
 stbpair_list *ptstbpair;

 ptstbpair = list;
 while (elm->U >= ptstbpair->D)
  {
   ptstbpair = ptstbpair->NEXT;
   if (ptstbpair == NULL)
    break;
  }

 return (ptstbpair);
}

/*****************************************************************************
*                           fonction stb_globalstbpair()                     *
*****************************************************************************/
stbpair_list *
stb_globalstbpair (head)
     stbpair_list *head;
{
 stbpair_list *ptstbpair;
 long u;
 long d;
 unsigned char pU, pD;

 if (head == NULL)
  return (NULL);

 u = head->U;
 d = head->D;
 pU=head->phase_U;
 pD=head->phase_D;
 ptstbpair = head->NEXT;

 while (ptstbpair)
  {
   if (ptstbpair->D < d)
    d = ptstbpair->D, pD=ptstbpair->phase_D;

   if (ptstbpair->U > u)
    u = ptstbpair->U, pU=ptstbpair->phase_U;

   ptstbpair = ptstbpair->NEXT;
  }

 if (d > u)
  d = u, pD=pU;

 return (stb_addstbpair_and_phase (NULL, d, u, pD, pU));
}

/*****************************************************************************
*                           fonction stb_copystbtabpair()                    *
*****************************************************************************/
void
stb_copystbtabpair (tabs, tabd, periode, periodemax, periodemin, size, index, periodindex, trans)
     stbpair_list **tabs;
     stbpair_list **tabd;
     long *periodemax ;
     long *periodemin ;
     long periode;
     char size;
     char index;
     char periodindex;
     int trans;
{
 stbpair_list *ptstbpair;
 stbpair_list **tab;
 char i;

 if (tabd != NULL)
  {
   tab = tabd;
  }
 else
  {
   tab = tabs;
  }
 for (i = 0; i < size; i++)
  {
   if (tabs == NULL)
    {
     tabd[(int) i] = NULL;
     continue;
    }
   if (tabd != NULL)
    tabd[(int) i] = stb_dupstbpairlist (tabs[(int) i]);
   if (i > index || (i==index && trans==0))
    for (ptstbpair = tab[(int) i]; ptstbpair != NULL;
         ptstbpair = ptstbpair->NEXT)
     {
      ptstbpair->U = ptstbpair->U - periode + ((periodemax != NULL && periodindex!=STB_NO_INDEX) ? periodemax[(int)periodindex] : (long)0) ;
      ptstbpair->D = ptstbpair->D - periode + ((periodemin != NULL && periodindex!=STB_NO_INDEX) ? periodemin[(int)periodindex] : (long)0) ;
     }
   else if(index < size)
    for (ptstbpair = tab[(int) i]; ptstbpair != NULL;
         ptstbpair = ptstbpair->NEXT)
     {
      ptstbpair->U = ptstbpair->U + ((periodemax != NULL && periodindex!=STB_NO_INDEX) ? periodemax[(int)periodindex] : (long)0) ;
      ptstbpair->D = ptstbpair->D + ((periodemin != NULL && periodindex!=STB_NO_INDEX) ? periodemin[(int)periodindex] : (long)0) ;
     }
  }
}

/*****************************************************************************
*                           fonction stb_nodeglobalpair()                    *
*****************************************************************************/
stbpair_list *
stb_nodeglobalpair (stbpair_list **tabpair, long periode, char size, char index)
{
 stbpair_list *ptstbpair;
 stbpair_list *ptres = NULL;
 long u;
 long d;
 char i;

 for (i = 0; i < size; i++)
  {
   for (ptstbpair = *(tabpair + i); ptstbpair; ptstbpair = ptstbpair->NEXT)
    {
     if (i >= index)
      {
       u = ptstbpair->U - periode;
       d = ptstbpair->D - periode;
      }
     else
      {
       u = ptstbpair->U;
       d = ptstbpair->D;
      }
     if (ptres == NULL)
      ptres = stb_addstbpair_and_phase (NULL, d, u, ptstbpair->phase_D, ptstbpair->phase_U);
     else
      {
       if (d < ptres->D)
        ptres->phase_D=ptstbpair->phase_D, ptres->D = d;

       if (u > ptres->U)
        ptres->phase_U=ptstbpair->phase_U, ptres->U = u;
      }
    }
  }

 if (ptres != NULL)
  {
   if (ptres->D > ptres->U)
    ptres->phase_D=ptres->phase_U, ptres->D = ptres->U;
  }

 return (ptres);
}

/*****************************************************************************
*                           fonction stb_mergestbtab()                       *
*****************************************************************************/
stbpair_list **
stb_mergestbtab (tab1, tab2, size)
     stbpair_list **tab1;
     stbpair_list **tab2;
     char size;
{
 int i;

 for (i = 0; i < (int) size; i++)
  {
   tab1[i] = stb_mergestbpairlist (tab1[i], tab2[i]);
   tab2[i] = NULL;
  }

 return (tab1);
}

/*****************************************************************************
*                           fonction stb_mergestbpairlist()                  *
*****************************************************************************/
stbpair_list *
stb_mergestbpairlist (list1, list2)
     stbpair_list *list1;
     stbpair_list *list2;
{
 stbpair_list *ptstbpair;
 stbpair_list *ptstbpairx;
 stbpair_list *ptbefore;
 stbpair_list *ptafter;
 stbpair_list *ptrange;

 if (list1 == NULL)
  return (list2);
 else if (list2 == NULL)
  return (list1);

 ptstbpair = list1;

 while (ptstbpair)
  {
   list1 = list1->NEXT;

   ptbefore = stb_beforestbpair (list2, ptstbpair);
   ptafter = stb_afterstbpair (list2, ptstbpair);

   if (ptbefore == NULL)
    {
     ptrange = list2;
     list2 = NULL;
    }
   else
    {
     ptrange = ptbefore->NEXT;
     ptbefore->NEXT = NULL;
    }

   if (ptafter != NULL)
    {
     if (ptrange == ptafter)
      {
       ptrange = NULL;
      }
     else
      {
       for (ptstbpairx = ptrange; ptstbpairx->NEXT != ptafter;
            ptstbpairx = ptstbpairx->NEXT);
       ptstbpairx->NEXT = NULL;
      }
    }

   ptstbpair->NEXT = ptrange;

   ptrange = stb_globalstbpair (ptstbpair);
   stb_freestbpair (ptstbpair);


   ptrange =
    (stbpair_list *) append ((chain_list *) ptrange, (chain_list *) ptafter);
   list2 =
    (stbpair_list *) append ((chain_list *) list2, (chain_list *) ptrange);

   ptstbpair = list1;
  }

 return (list2);
}

/*****************************************************************************
*                           fonction stb_fusestbpair()                       *
*****************************************************************************/
stbpair_list *
stb_fusestbpair (list)
     stbpair_list *list;
{
 stbpair_list *ptstbpair;
 stbpair_list *ptsav;

 ptstbpair = list;

 while (ptstbpair)
  {
   if (ptstbpair->NEXT != NULL)
    {
     if (ptstbpair->U >= ptstbpair->NEXT->D)
      {
       ptstbpair->U = ptstbpair->NEXT->U;
       ptstbpair->phase_U = ptstbpair->NEXT->phase_U;
       ptsav = ptstbpair->NEXT;
       ptstbpair->NEXT = ptsav->NEXT;
       ptsav->NEXT = NULL;
       stb_freestbpair (ptsav);
      }
     else
      ptstbpair = ptstbpair->NEXT;
    }
   else
    break;
  }

 return (list);
}

/*****************************************************************************
*                           fonction stb_adddelaystbpair()                   *
*****************************************************************************/
stbpair_list *
stb_adddelaystbpair (list, delaymax, delaymin, periodeSetup, periodeHold)
     stbpair_list *list;
     long delaymax;
     long delaymin;
     long periodeSetup;
     long periodeHold;
{
 stbpair_list *ptstbpair;

 for (ptstbpair = list; ptstbpair; ptstbpair = ptstbpair->NEXT)
  {
   ptstbpair->D = ptstbpair->D + delaymin - periodeHold;
   ptstbpair->U = ptstbpair->U + delaymax - periodeSetup;
  }

 return (stb_fusestbpair (list));
}

/*****************************************************************************
*                           fonction stb_transferstbpair()                   *
*****************************************************************************/
stbpair_list *
stb_transferstbpair (lists, listd, delaymax, delaymin, periodeSetup, periodeHold)
     stbpair_list *lists;
     stbpair_list *listd;
     long delaymax;
     long delaymin;
     long periodeSetup;
     long periodeHold;
{
 lists = stb_adddelaystbpair (lists, delaymax, delaymin, periodeSetup, periodeHold);
 listd = stb_mergestbpairlist (lists, listd);

 return (listd);
}

/*****************************************************************************
*                           fonction stb_calcsetuphold()                     *
*****************************************************************************/

void stb_setdetail(Setup_Hold_Computation_Detail_INFO *detail, long val, long instab, long clock, long margin, long period, long misc, Extended_Setup_Hold_Computation_Data_Item *eshcdi, short flags, unsigned char phase, long movedperiod)
{
  if (detail!=NULL)
    {
      detail->VALUE=val;
      detail->instab=instab;
      detail->clock=clock;
      detail->margin=margin;
      detail->period=period;
      detail->misc=misc;
      detail->skew=eshcdi==NULL?0:eshcdi->value;
      detail->skew_common_node=eshcdi==NULL?NULL:eshcdi->common;
      detail->flags=flags;
      detail->phase_origin=phase;
      detail->data_margin=eshcdi==NULL?0:eshcdi->data_margin;
      detail->clock_margin=eshcdi==NULL?0:eshcdi->clock_margin;
      detail->moved_clock_period=movedperiod;
      detail->uncertainty=eshcdi->clock_uncertainty;
    }
} 

// what==0 => hz et !hz, what=1 => !hz, wath=2 => hz
int
stb_calcsetuphold (stbfig_list *stbfig, stbpair_list **tabpair, stbpair_list **tabhz, stbnode *node, long *s, long *h, int flagindex, long EventPeriodHold, Extended_Setup_Hold_Computation_Data *eshcd, Setup_Hold_Computation_Detail *detail, int what, stbck *endnodeck, int specout)
{
 stbpair_list *global;
 stbpair_list *globalhz;
 stbpair_list *globalverif;
 stbck *clock;
 long slopesetup, slopemovesetup;
 long slopehold, slopemovehold;
 long setup, ls;
 long hold, lh;
 long period;
 char flag = 'N';
 char index;
 char active;
 char detflags=0;

 *s = STB_NO_TIME;
 *h = STB_NO_TIME;

 if (detail) detail->setup.VALUE=detail->hold.VALUE=STB_NO_TIME;

 if (specout==-1 && V_BOOL_TAB[__STB_ENBALE_COMMAND_CHECK].VALUE==0 &&
      ((node->EVENT->ROOT->TYPE & TTV_SIG_Q)==TTV_SIG_Q 
       || ((node->EVENT->ROOT->TYPE & TTV_SIG_R)==TTV_SIG_R && (node->EVENT->TYPE & TTV_NODE_UP)!=0))
    ) 
  return 0;

 if (specout==-1 && endnodeck != NULL)
  {
   period = endnodeck->PERIOD;
   if (flagindex == 1)
    {
     if ((endnodeck->TYPE == STB_TYPE_LATCH) ||
         (endnodeck->TYPE == STB_TYPE_FLIPFLOP) ||
         (endnodeck->TYPE == STB_TYPE_PRECHARGE) ||
         (endnodeck->TYPE == STB_TYPE_EVAL))
      index = STB_NO_INDEX;
     else
      index = endnodeck->CKINDEX;
    }
   else
    index = STB_NO_INDEX;

   if ((endnodeck->TYPE == STB_TYPE_PRECHARGE
       || endnodeck->TYPE == STB_TYPE_EVAL) && what!=1)
    globalhz =
     stb_nodeglobalpair (tabhz, endnodeck->PERIOD, node->NBINDEX, index);
   else
    globalhz = NULL;
  }
 else
  {
   index = STB_NO_INDEX;
   period = STB_NO_TIME;
  }

 if (what!=2)
 {
 global = stb_nodeglobalpair (tabpair, period, node->NBINDEX, index);

 if (global == NULL)
  {
   if (endnodeck)
    if ((endnodeck->TYPE != STB_TYPE_CLOCK)
        && (endnodeck->TYPE != STB_TYPE_CLOCKPATH)
        && (endnodeck->TYPE != STB_TYPE_PRECHARGE)
        && (endnodeck->TYPE != STB_TYPE_COMMAND)
        && ((node->FLAG & STB_NODE_DATA) == 0))
        node->FLAG |= STB_NODE_NODATA ;
   /* return (1); */
  }
 else
  {
   node->FLAG &= ~(STB_NODE_NODATA) ;
   node->FLAG |= STB_NODE_DATA ;
  }
 }
 else global=NULL;

 if (specout>=0 && node->SPECOUT != NULL && global != NULL )
  {
   if ((endnodeck != NULL) && (flagindex == 0))
    {
     index = endnodeck->CKINDEX;
     period = endnodeck->PERIOD;
    }

/*   globalverif =
    stb_nodeglobalpair (node->SPECOUT, period, node->NBINDEX, index);*/

   globalverif = stb_globalstbpair(node->SPECOUT[specout]);
   if (globalverif != NULL)
    {
     if ((stbfig->STABILITYMODE & STB_STABILITY_SETUP) == STB_STABILITY_SETUP)
      *s = globalverif->U - (eshcd==NULL?0:eshcd->setup_clock_gap[1].clock_uncertainty) - (global->U + (eshcd==NULL?0:eshcd->setup_clock_gap[1].data_margin));
     if ((stbfig->STABILITYMODE & STB_STABILITY_HOLD) == STB_STABILITY_HOLD)
      *h = (global->D + (eshcd==NULL?0:eshcd->hold_clock_gap[1].data_margin)) - (globalverif->D+(eshcd==NULL?0:eshcd->hold_clock_gap[1].clock_uncertainty));

     if (detail!=NULL)
       {
         stb_setdetail(&detail->setup, *s, global->U, globalverif->U, 0, 0, 0, &eshcd->setup_clock_gap[1], STB_DEBUG_SPECOUTMODE, global->phase_U, 0);
         stb_setdetail(&detail->hold, *h, global->D, globalverif->D, 0, 0, 0, &eshcd->hold_clock_gap[1], STB_DEBUG_SPECOUTMODE, global->phase_D, 0);
       }
     stb_freestbpair (globalverif);
     flag = 'Y';
    }
  }

 clock = endnodeck;
 if (specout==-1 && clock != NULL)
  {
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
   if ((clock->VERIF & STB_NO_VERIF) == STB_NO_VERIF)
    {
     if( global ) 
       stb_freestbpair( global );
     if (node->SPECOUT != NULL)
      {
       if ((*s > 0) && (*h > 0))
        return (1);
       else
        return (0);
      }
     else
      {
       return (1);
      }
    }
   slopemovesetup=slopemovehold=0;
   if ((active & STB_SLOPE) == STB_SLOPE)
    {
     if ((active & STB_SLOPE_UP) == STB_SLOPE_UP)
      {
       slopesetup = clock->SUPMIN;
       slopehold = clock->SUPMAX;
       detflags|=STB_DEBUG_SETUP_UPCK|STB_DEBUG_HOLD_UPCK;
      }
     else
      {
       slopesetup = clock->SDNMIN;
       slopehold = clock->SDNMAX;
      }
    }
   else if ((active & STB_STATE) == STB_STATE)
    {
     if ((active & STB_STATE_UP) == STB_STATE_UP)
      {
       if(STB_OPEN_LATCH_PHASE == 'N')
         {
          if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
           slopesetup = clock->SDNMIN;
          else
           slopesetup = (clock->SUPMIN < clock->SDNMIN) ?
            clock->SUPMIN : clock->SUPMIN - clock->PERIOD, detflags|=STB_DEBUG_SETUP_UPCK;
          slopehold = clock->SDNMAX;
         }
       else
         {
          if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
           slopemovesetup=(clock->SUPMIN < clock->SDNMIN) ?0:clock->PERIOD, slopesetup=clock->SDNMIN+slopemovesetup;
          else
           slopesetup = clock->SUPMIN, detflags|=STB_DEBUG_SETUP_UPCK;
          slopemovehold=(clock->SUPMAX < clock->SDNMAX) ?0:clock->PERIOD;
          slopehold = clock->SDNMAX+slopemovehold;
         }
      }
     else
      {
       if(STB_OPEN_LATCH_PHASE == 'N')
         {
          if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
           slopesetup = clock->SUPMIN, detflags|=STB_DEBUG_SETUP_UPCK;
          else
           slopesetup = (clock->SDNMIN < clock->SUPMIN) ?
            clock->SDNMIN : clock->SDNMIN - clock->PERIOD;
          slopehold = clock->SUPMAX;
          detflags|=STB_DEBUG_HOLD_UPCK; 
         }
       else
         {
           if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
             slopemovesetup=(clock->SDNMIN < clock->SUPMIN) ?0:clock->PERIOD, slopesetup = clock->SUPMIN+slopemovesetup, detflags|=STB_DEBUG_SETUP_UPCK;
           else
             slopesetup = clock->SDNMIN ;
           slopemovehold=(clock->SDNMAX < clock->SUPMAX) ?0:clock->PERIOD;
           slopehold = clock->SUPMAX+slopemovehold;
           detflags|=STB_DEBUG_HOLD_UPCK; 
         }
      }
    }
   if (clock->SETUP != STB_NO_TIME && global && (eshcd==NULL || !eshcd->setup_clock_gap[1].same_origin))
     {
       setup = (slopesetup + (eshcd==NULL?0:eshcd->setup_clock_gap[1].clock_margin) - (eshcd==NULL?0:eshcd->setup_clock_gap[1].clock_uncertainty)) - (global->U  + (eshcd==NULL?0:eshcd->setup_clock_gap[1].data_margin)) - clock->SETUP + (eshcd==NULL?0:eshcd->setup_clock_gap[1].value);
       if (detail!=NULL)
         stb_setdetail(&detail->setup, setup, global->U, slopesetup, clock->SETUP, 0, 0, &eshcd->setup_clock_gap[1], detflags, global->phase_U, slopemovesetup);
     }
   else
    setup = STB_NO_TIME;
   if (clock->HOLD != STB_NO_TIME && global && (eshcd==NULL || !eshcd->hold_clock_gap[1].same_origin))
     {
       hold = (global->D  + (eshcd==NULL?0:eshcd->hold_clock_gap[1].data_margin)) + EventPeriodHold - ((slopehold + (eshcd==NULL?0:eshcd->hold_clock_gap[1].clock_margin)) + clock->HOLD - clock->PERIOD + (eshcd==NULL?0:eshcd->hold_clock_gap[1].clock_uncertainty)) + (eshcd==NULL?0:eshcd->hold_clock_gap[1].value);
       if (detail!=NULL)
         stb_setdetail(&detail->hold, hold, global->D, slopehold, clock->HOLD, clock->PERIOD, EventPeriodHold, &eshcd->hold_clock_gap[1], detflags, global->phase_D, slopemovehold);
     }
   else
    hold = STB_NO_TIME;
   if (flag == 'N')
    {
     if ((stbfig->STABILITYMODE & STB_STABILITY_SETUP) == STB_STABILITY_SETUP)
      *s = setup;
     if ((stbfig->STABILITYMODE & STB_STABILITY_HOLD) == STB_STABILITY_HOLD)
      *h = hold;
     flag = 'Y';
    }
   else
    {
     if ((setup < *s)
         && ((stbfig->STABILITYMODE & STB_STABILITY_SETUP) ==
             STB_STABILITY_SETUP))
       *s = setup;
     if ((hold < *h)
         && ((stbfig->STABILITYMODE & STB_STABILITY_HOLD) ==
             STB_STABILITY_HOLD))
       *h = hold;
    }
   if (globalhz != NULL)
    {
     ls=setup;
     lh=hold;
     detflags=STB_DEBUG_HZPATH;
     slopemovesetup=slopemovehold=0;
     if ((active & STB_STATE_UP) == STB_STATE_UP)
      {
       if(STB_OPEN_LATCH_PHASE == 'N')
         {
          slopesetup =
           (clock->SUPMIN <
            clock->SDNMIN) ? clock->SUPMIN : clock->SUPMIN - clock->PERIOD, detflags|=STB_DEBUG_SETUP_UPCK;
          slopehold = clock->SDNMAX;
         }
       else
         {
          slopesetup = clock->SUPMIN ;
          detflags|=STB_DEBUG_SETUP_UPCK;
          slopemovehold=(clock->SUPMAX < clock->SDNMAX) ?0:clock->PERIOD;
          slopehold = clock->SDNMAX+slopemovehold;
         }
      }
     else
      {
       if(STB_OPEN_LATCH_PHASE == 'N')
         {
          slopesetup =
           (clock->SDNMIN <
            clock->SUPMIN) ? clock->SDNMIN : clock->SDNMIN - clock->PERIOD;
          slopehold = clock->SUPMAX;
          detflags|=STB_DEBUG_HOLD_UPCK;
         }
       else
         {
          slopesetup = clock->SDNMIN ;
          slopemovehold=(clock->SDNMAX < clock->SUPMAX) ?0:clock->PERIOD;
          slopehold = clock->SUPMAX+slopemovehold;
          detflags|=STB_DEBUG_HOLD_UPCK;
         }
      }
     if (clock->SETUP != STB_NO_TIME && (eshcd==NULL || !eshcd->setup_clock_gap[0].same_origin))
       {
         setup = (slopesetup + (eshcd==NULL?0:eshcd->setup_clock_gap[0].clock_margin) - (eshcd==NULL?0:eshcd->setup_clock_gap[0].clock_uncertainty)) - (globalhz->U + (eshcd==NULL?0:eshcd->setup_clock_gap[0].data_margin)) - clock->SETUP + (eshcd==NULL?0:eshcd->setup_clock_gap[0].value);
         if (setup<ls && detail!=NULL)
           stb_setdetail(&detail->setup, setup, globalhz->U, slopesetup, clock->SETUP, 0, 0, &eshcd->setup_clock_gap[0],detflags, globalhz->phase_U, slopemovesetup);
       }
     else
      setup = STB_NO_TIME;
     if ((node->EVENT->ROOT->TYPE & TTV_SIG_R)!=TTV_SIG_R)
     {
       if (clock->HOLD != STB_NO_TIME && (eshcd==NULL || !eshcd->hold_clock_gap[0].same_origin))
         {
           hold = (globalhz->D + (eshcd==NULL?0:eshcd->hold_clock_gap[0].data_margin)) + EventPeriodHold - ((slopehold + (eshcd==NULL?0:eshcd->hold_clock_gap[0].clock_margin)) + clock->HOLD - clock->PERIOD + (eshcd==NULL?0:eshcd->hold_clock_gap[0].clock_uncertainty)) + (eshcd==NULL?0:eshcd->hold_clock_gap[0].value);
           if (hold<lh && detail!=NULL)
             stb_setdetail(&detail->hold, hold, globalhz->D, slopehold, clock->HOLD, clock->PERIOD, EventPeriodHold, &eshcd->hold_clock_gap[0],detflags,globalhz->phase_D, slopemovehold);
         }
       else
        hold = STB_NO_TIME;
     }
     if (flag == 'N')
      {
       if ((stbfig->STABILITYMODE & STB_STABILITY_SETUP) ==
           STB_STABILITY_SETUP)
         *s = setup;

       if ((node->EVENT->ROOT->TYPE & TTV_SIG_R)!=TTV_SIG_R)
       {
         if ((stbfig->STABILITYMODE & STB_STABILITY_HOLD) == STB_STABILITY_HOLD)
           *h = hold;
       }
       flag = 'Y';
      }
     else
      {
       if ((setup < *s)
           && ((stbfig->STABILITYMODE & STB_STABILITY_SETUP) ==
               STB_STABILITY_SETUP))
         *s = setup;
       if ((node->EVENT->ROOT->TYPE & TTV_SIG_R)!=TTV_SIG_R)
       {
         if ((hold < *h)
             && ((stbfig->STABILITYMODE & STB_STABILITY_HOLD) ==
                 STB_STABILITY_HOLD))
           *h = hold;
       }
      }
      stb_freestbpair( globalhz );
    }
  }
 else if (node->SPECOUT == NULL)
  {
   *s = STB_NO_TIME;
   *h = STB_NO_TIME;
  }

 if( global )
   stb_freestbpair( global );

 if ((*s > 0) && (*h > 0))
  return (1);
 else
  return (0);
}

/*****************************************************************************
*                           fonction stb_clockmaskstblist()                  *
*****************************************************************************/
stbpair_list *
stb_clockmaskstblist (stbfig_list *stbfig, stbpair_list *list, stbck *clock, int init, char index, int filtermode, ttvevent_list *tve)
{
 stbpair_list *ptstbpair;
 stbpair_list *ptstbpairnext;
 long u;
 long d;
 long slopeopen;
 long slopeopenmax;
 long slopeclose, slopeclosemin;

 if (clock == NULL)
  return (list);

 if ((clock->ACTIVE & STB_SLOPE) == STB_SLOPE)
  {
   if ((list == NULL) && (init == 0))
    return (NULL);
   if ((clock->ACTIVE & STB_SLOPE_UP) == STB_SLOPE_UP)
    {
     ptstbpair = stb_addstbpair (NULL, clock->SUPMIN + clock->ACCESSMIN, 
                                       clock->SUPMAX + clock->ACCESSMAX);
    }
   else
    {
     ptstbpair = stb_addstbpair (NULL, clock->SDNMIN + clock->ACCESSMIN,
                                       clock->SDNMAX + clock->ACCESSMAX);
    }
   stb_freestbpair (list);
   return (ptstbpair);
  }
 else if ((clock->ACTIVE & STB_STATE) == STB_STATE)
  {
   if (init)
    {
     if ((clock->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
      {
       if (((stbfig->STABILITYMODE & STB_STABILITY_WORST) != 0) && (init == 1))
        {
         if(STB_OPEN_LATCH_PHASE == 'N')
           u = clock->SDNMAX ;
         else
           u = ((clock->SDNMAX >
              clock->SUPMAX) ? clock->SDNMAX : clock->SDNMAX + clock->PERIOD) ;
        }
       else
        {
         if(STB_OPEN_LATCH_PHASE == 'N')
           u = ((clock->SUPMAX <
                clock->SDNMAX) ? clock->SUPMAX : clock->SUPMAX - clock->PERIOD) +
              clock->ACCESSMAX ;
         else
           u = clock->SUPMAX + clock->ACCESSMAX ;
        }
       if(STB_OPEN_LATCH_PHASE == 'N')
       d =
        ((clock->SUPMIN <
         clock->SDNMIN) ? clock->SUPMIN : clock->SUPMIN - clock->PERIOD) + 
         clock->ACCESSMIN ;
       else
       d = clock->SUPMIN + clock->ACCESSMIN ;
      }
     else
      {
       if (((stbfig->STABILITYMODE & STB_STABILITY_WORST) != 0) && (init == 1))
        {
         if(STB_OPEN_LATCH_PHASE == 'N')
           u = clock->SUPMAX ;
         else
           u = ((clock->SUPMAX <
                clock->SDNMAX) ? clock->SUPMAX : clock->SDNMAX + clock->PERIOD) ;
        }
       else
        {
         if(STB_OPEN_LATCH_PHASE == 'N')
           u = ((clock->SDNMAX <
                clock->SUPMAX) ? clock->SDNMAX : clock->SDNMAX - clock->PERIOD) +
               clock->ACCESSMAX ;
         else
           u = clock->SDNMAX  + clock->ACCESSMAX ;
        }
       if(STB_OPEN_LATCH_PHASE == 'N')
         d =
          ((clock->SDNMIN <
           clock->SUPMIN) ? clock->SDNMIN : clock->SDNMIN - clock->PERIOD) + 
           clock->ACCESSMIN ;
        else
         d = clock->SDNMIN + clock->ACCESSMIN ;
      }
     if (d > u)
      d = u;
     ptstbpair = stb_addstbpair (NULL, d, u);
     stb_freestbpair (list);
     return (ptstbpair);
    }
   else
    {
     if (list == NULL)
      return (NULL);

     if ((clock->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
      {
       if(STB_OPEN_LATCH_PHASE == 'N')
         {
          slopeopen =
           ((clock->SUPMIN <
            clock->SDNMIN) ? clock->SUPMIN : clock->SUPMIN - clock->PERIOD) + 
            clock->ACCESSMIN ;
           slopeopenmax = ((clock->SUPMAX <
              clock->SDNMAX) ? clock->SUPMAX : clock->SUPMAX - clock->PERIOD) + 
              clock->ACCESSMAX ;
           slopeclose = clock->SDNMAX;
           slopeclosemin = clock->SDNMIN;
          }
       else
          {
           slopeopen = clock->SUPMIN + clock->ACCESSMIN ;
           slopeopenmax = clock->SUPMAX + clock->ACCESSMAX ;
           slopeclose = ((clock->SUPMAX < clock->SDNMAX) ? 
                   clock->SDNMAX : clock->SDNMAX + clock->PERIOD) ;
           slopeclosemin = ((clock->SUPMIN < clock->SDNMIN) ? 
                   clock->SDNMIN : clock->SDNMIN + clock->PERIOD) ;
          }
       if(slopeopenmax > slopeclose)
           slopeopenmax = slopeclose ;
      }
     else
      {
       if(STB_OPEN_LATCH_PHASE == 'N')
         {
          slopeopen =
           ((clock->SDNMIN <
            clock->SUPMIN) ? clock->SDNMIN : clock->SDNMIN - clock->PERIOD) + 
           clock->ACCESSMIN ;
           slopeopenmax = ((clock->SDNMAX <
               clock->SUPMAX) ? clock->SDNMAX : clock->SDNMAX - clock->PERIOD) + 
             clock->ACCESSMAX ;
           slopeclose = clock->SUPMAX;
           slopeclosemin = clock->SUPMIN;
          }
       else
          {
           slopeopen = clock->SDNMIN + clock->ACCESSMIN ;
           slopeopenmax = clock->SDNMAX + clock->ACCESSMAX ;
           slopeclose = ((clock->SDNMAX < clock->SUPMAX) ? 
                   clock->SUPMAX : clock->SUPMAX + clock->PERIOD) ;
           slopeclosemin = ((clock->SDNMIN < clock->SUPMIN) ? 
                   clock->SUPMIN : clock->SUPMIN + clock->PERIOD) ;
          }
       if(slopeopenmax > slopeclose)
           slopeopenmax = slopeclose ;
      }

     if (filtermode)
     {
       list=stb_clockmaskstblist_simple(list, slopeopen, slopeclose);
       if (filtermode==2)
        { // positionnement de la stabilite par rapport a l'event que peut generer la clock
          if (((clock->ACTIVE==STB_STATE_UP) && (tve->TYPE & TTV_NODE_UP)!=0)
              || ((clock->ACTIVE==STB_STATE_DN) && (tve->TYPE & TTV_NODE_UP)==0))
             ptstbpair = stb_addstbpair (NULL, slopeopen, slopeopenmax);
          else if (((clock->ACTIVE==STB_STATE_UP) && (tve->TYPE & TTV_NODE_UP)==0)
              || ((clock->ACTIVE==STB_STATE_DN) && (tve->TYPE & TTV_NODE_UP)!=0))
             ptstbpair = stb_addstbpair (NULL, slopeclosemin, slopeclose);
          else
             ptstbpair=NULL;
           
          list=stb_mergestbpairlist(list, ptstbpair);
        }
       return list;
     }

     ptstbpair = list;
     while (ptstbpair->U < slopeopen)
      {
       ptstbpair = ptstbpair->NEXT;
       list->NEXT = NULL;
       stb_freestbpair (list);
       list = ptstbpair;
       if (ptstbpair == NULL)
        break;
      }

     if (list == NULL)
      {
       if(index != clock->CKINDEX)
         return(NULL) ;

       d = slopeopen;
       u = slopeopenmax ;

       if (d > u)
        d = u;
       list = stb_addstbpair (NULL, d, u);
       return (list);
      }

     for (ptstbpair = list; ptstbpair->NEXT != NULL;
          ptstbpair = ptstbpair->NEXT);

     if (ptstbpair->U >= slopeclose)
      {
       if (slopeopen > slopeclose)
        slopeopen = slopeclose;
       if(list->D > slopeopen)
           slopeopen = list->D ;
       stb_freestbpair (list);
       list = stb_addstbpair (NULL, slopeopen, slopeclose);
       return (list);
      }

     if (list->D < slopeopen)
      list->D = slopeopen;

     for (ptstbpair = list; ptstbpair != NULL; ptstbpair = ptstbpairnext)
       {
        ptstbpairnext = ptstbpair->NEXT ;
        if(ptstbpair->U < slopeopenmax)
           {
            if(ptstbpair == list)
              {
               ptstbpair->U = slopeopenmax ;
              }
            else
              {
               list->NEXT = ptstbpair->NEXT ;
               ptstbpair->NEXT = NULL ;
               stb_freestbpair (ptstbpair);
              }
            }
           else
            {
             if((ptstbpair->D <= slopeopenmax) && (ptstbpair != list))
               {
                  list->U = ptstbpair->U ;
                  list->NEXT = ptstbpair->NEXT ;
                  ptstbpair->NEXT = NULL ;
                  stb_freestbpair (ptstbpair);
               }
               break ;
            }
         }

         return (list);
    }
  }
 else
  {
   return (list);
  }
}
stbpair_list *stb_clockmaskstblist_simple (stbpair_list *list, long slopeopen, long slopeclose)
{
 stbpair_list *ptstbpair, *prev=NULL;
 stbpair_list *ptstbpairnext, *tmppair=NULL;
 long u;
 long d;
 int removebefore=0, removeafter=0, hasmid=0;

 if (list == NULL) return NULL;

 ptstbpair = list;
 while (ptstbpair!=NULL && ptstbpair->U < slopeopen)
   {
     ptstbpair = ptstbpair->NEXT;
     list->NEXT = NULL;
     stb_freestbpair (list);
     removebefore=1;
     list = ptstbpair;
   }
 
 if (list!=NULL)
   {
     if (list->D < slopeopen) list->D = slopeopen;

     for (ptstbpair = list; ptstbpair!= NULL && ptstbpair->D <= slopeclose;
          prev=ptstbpair, ptstbpair = ptstbpair->NEXT) ; 
     
     if (prev!=NULL)
       {
         if (prev->NEXT!=NULL) removeafter=1;
         stb_freestbpair (prev->NEXT);
         prev->NEXT=NULL;
         if (prev->U>slopeclose) prev->U=slopeclose, removeafter=1;
       }
     else
       {
         removeafter=1;
         stb_freestbpair (list);
         list=NULL;
       }
   }

 return (list);
}

/*****************************************************************************
*                           fonction stb_initstbnode()                       *
*****************************************************************************/
void
stb_initstbnode (stbfig, event)
     stbfig_list *stbfig;
     ttvevent_list *event;
{
 stbnode *node = stb_getstbnode (event);
 stbdomain_list *ptdomain = NULL;
 stbpair_list *sbp;
 char i;
 int ignore, addhz=0;
 stbck *ck;

 if (node->STBTAB == NULL)
  {
   node->STBTAB = stb_alloctab (stbfig->PHASENUMBER);
   node->NBINDEX = stbfig->PHASENUMBER;
   if (getptype (node->EVENT->ROOT->USER,STB_IS_CLOCK)==NULL)
   {
     for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
     {
       if (ck != NULL)
        if ((ck->TYPE == STB_TYPE_LATCH) || 
            (ck->TYPE == STB_TYPE_FLIPFLOP) ||
            (ck->TYPE == STB_TYPE_PRECHARGE) ||
            (ck->TYPE == STB_TYPE_EVAL))
         {
          for (i = 0; i < node->NBINDEX; i++)
           {
            if (ck != NULL)
             if (ck->CKINDEX == i)
             {
              node->STBTAB[(int)i] = stb_mergestbpairlist (node->STBTAB[(int)i], stb_clockmaskstblist (stbfig, NULL, ck, 1, i, 0, node->EVENT));
              if (stbfig->ANALYSIS == STB_GLOB_ANALYSIS)
              {
                node->STBTAB[(int)i] = stb_globalstbpair(sbp=node->STBTAB[(int)i]);
                stb_freestbpair(sbp);
              }
             }
           }
         }
     }
     if (node->SPECIN != NULL)
      {
       for (i = 0; i < node->NBINDEX; i++)
        {
         ignore=0;
         for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
         {
           if (ck != NULL)
            {
             ptdomain = stb_getstbdomain (stbfig->CKDOMAIN, ck->CKINDEX);
             if ((ck->CKINDEX != STB_NO_INDEX)
                 && ((i < ptdomain->CKMIN || i > ptdomain->CKMAX)
                     || (stb_isdisable (stbfig, i, ck->CKINDEX))))
              ignore=1;
            }
         }
         if (ignore) continue;
         if (*(node->STBTAB + i)!=NULL) stb_freestbpair(*(node->STBTAB + i));
         *(node->STBTAB + i) = stb_dupstbpairlist (*(node->SPECIN + i));
        }
      }
   }
  }

 for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
 {
   if (ck != NULL)
    if (((ck->TYPE == STB_TYPE_PRECHARGE) ||
         (ck->TYPE == STB_TYPE_EVAL)) && (node->STBHZ == NULL))
            addhz=1;
 }
 if (addhz) node->STBHZ = stb_alloctab (stbfig->PHASENUMBER);
}

/*****************************************************************************
*                           fonction stb_transferstbline()                   *
*****************************************************************************/
void
stb_transferstbline (stbfig_list *stbfig, ttvevent_list *event, stbnode *node , stbpair_list **tabpair, stbpair_list **tabhz, char flagrc, int justretransfer, ttvevent_list *pathstartnode, stbck *ck, int quiet, int flags)
{
 stbnode *nodex;
 stbpair_list *tabsav[256];
 stbpair_list **tab;
 stbpair_list **tabx;
 stbdomain_list *ptdomain = NULL;
 stbpair_list *ptstbpair;
 ttvevent_list *pevent;
 ttvevent_list *cmdmax;
 ttvevent_list *cmdmin;
 ttvline_list *line;
 ttvline_list *ptline;
 long delaymax;
 long delaymin;
 long delay;
 long periode, periodeSetup, periodeHold, output_periode, input_periode;
 long type;
 char i, j;
 char buf[2048];
 char namebuf[1024];
 char flager = 'N', ckedge;
 stbck *ick;
 long EventPeriodeHold;
 long thissetup, thishold;
 int res, possible_set_reset=0, ondirective=0, nb0, nb1;
 char directive_saveckindex, directive_saveactive;
 long start, startmax, end, endmin, move;
 char filterstate;
 valid_range_info *vri;
 ttvevent_list *clockedsig;

 if (stbfig->GRAPH == STB_RED_GRAPH)
  {
   line = event->INPATH;
   type = TTV_FIND_PATH;
  }
 else
  {
   line = event->INLINE;
   type = TTV_FIND_LINE;
  }

 if (ck==NULL) ck=node->CK;

 if (getptype (node->EVENT->ROOT->USER,STB_IS_CLOCK)!=NULL
     && (ck==NULL || stb_IsClockCK(node->EVENT, ck))) return;

 if (stb_get_filter_directive_info(stbfig, node->EVENT, STB_NO_INDEX, &start, &startmax, &end, &endmin, &filterstate, &move, &clockedsig, NULL))
   {
     ck=stb_getstbnode(clockedsig)->CK;
/*     if (filterstate=='u') ck=stb_getstbnode(&node->EVENT->ROOT->NODE[1])->CK;
     else ck=stb_getstbnode(&node->EVENT->ROOT->NODE[0])->CK;*/
     directive_saveckindex=ck->CKINDEX;
     directive_saveactive=ck->ACTIVE;
     if (node->EVENT->ROOT!=clockedsig->ROOT) ondirective=1;
     else ondirective=2;
     ck->CKINDEX=ck->CTKCKINDEX;
     ck->ACTIVE=filterstate=='u'?STB_STATE_UP:STB_STATE_DN;
   }
 
 
 if (ck != NULL)
  if ((ck->TYPE == STB_TYPE_PRECHARGE)
      || (ck->TYPE == STB_TYPE_EVAL))
   {
    for (i = 0; i < node->NBINDEX; i++)
     {
      tabhz[(int) i] = NULL;
     }
   }
 
 EventPeriodeHold=0;
 
 for (ptline = line; ptline != NULL; ptline = ptline->NEXT)
  {
    if ((ptline->TYPE & TTV_LINE_A)!=0 && ck!=NULL && ptline->NODE==ck->CMD) possible_set_reset=1;
    if (((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
       (((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
       ((ptline->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
       (ttv_islineonlyend(stbfig->FIG,ptline,type) == 1)) ||
       (((ptline->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
        (ptline->FIG != stbfig->FIG)) ||
       (((ptline->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
        (ptline->FIG != stbfig->FIG) &&
        ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST))
       || ((ptline->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN)
       || ((ptline->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
    continue;

   pevent = ptline->NODE;

#if 1
#ifdef PRECHTEST
   // propagation que des vrais data pour les precharges
   if ((event->ROOT->TYPE & TTV_SIG_R)!=0 && stb_sigisonclockpath(pevent, NULL))
     continue;
#endif
#endif

   cmdmax = ttv_getlinecmd(stbfig->FIG,ptline,TTV_LINE_CMDMAX) ;
   cmdmin = ttv_getlinecmd(stbfig->FIG,ptline,TTV_LINE_CMDMIN) ;

   if ((pevent->ROOT->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
   {
      if (pevent==cmdmax || pevent==cmdmin)
        continue;
   }

   if ((flags & STB_TRANSFERT_NOT_COMMANDED)!=0 && (cmdmax!=NULL || cmdmin!=NULL))
      continue;

   if(ck != NULL)
   if(ck->CMD != NULL)
     {
      if((ck->CMD != cmdmax) && (ck->CMD != cmdmin)) 
        continue ;

      if (!stb_checkvalidcommand(event, ck)) continue;
     }

// correction due to false path/false slack
   if ((flags & STB_TRANSFERT_CORRECTION)!=0 || getptype(node->EVENT->ROOT->USER, TTV_SIG_MULTICYCLE_OUT)!=NULL)
   {
     stb_compute_falsepath_and_falseslack_effect(stbfig, node, ptline, cmdmax, ck, flags);
   }

//----

   if ((ptline->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ)
    {
     if ((ptline->ROOT->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R)
        continue ;
     tab = tabhz;
    }
   else
     tab = tabpair;

   tabx = NULL ;
   nodex = stb_getstbnode (pevent);

   if (!justretransfer)
   {
     if ((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
      {
       if(flagrc == STB_NO_RC_DELAY) continue ;
       if(nodex->FLAGRC == STB_RC_DELAY)
        {
         stb_transferstbnode (stbfig, event, STB_NO_RC_DELAY) ;
         node->FLAGRC = STB_NO_RC_DELAY ;
         stb_transferstbnode (stbfig, pevent, STB_RC_DELAY) ;
         tabx =  nodex->STBTAB ;
         for (i = 0; i < nodex->NBINDEX; i++)
          {
           tabsav[(int) i] = NULL;
          }
         nodex->STBTAB = tabsav ;
         stb_transferstbnode (stbfig, pevent, STB_NO_RC_DELAY) ;
        }
       node->FLAGRC = STB_RC_DELAY ;
      }
   }

   delaymin = ttv_getdelaymin (ptline);
   delaymax = ttv_getdelaymax (ptline);

   if (delaymax == TTV_NOTIME)
    delaymax = delaymin;
   if (delaymin == TTV_NOTIME)
    delaymin = delaymax;

   if (delaymax < delaymin)
    {
     delay = delaymin;
     delaymin = delaymax;
     delaymax = delay;
    }

   if ((delaymax == TTV_NOTIME) && (delaymin == TTV_NOTIME))
    continue;

   for (i = 0; i < node->NBINDEX; i++)
    {
     if (ck != NULL)
      {
       ptdomain = stb_getstbdomain (stbfig->CKDOMAIN, ck->CKINDEX);

       if (((ck->TYPE == STB_TYPE_LATCH) || 
            (ck->TYPE == STB_TYPE_FLIPFLOP) ||
            (ck->TYPE == STB_TYPE_EVAL) ||
            (ck->TYPE == STB_TYPE_PRECHARGE) ||
            ondirective)
           && (stb_cmpphase(stbfig, i, ck->CKINDEX)>=0/*i >= ck->CKINDEX*/))
        {
         if ((stb_cmpphase(stbfig,i, ck->CKINDEX)==0/*i == ck->CKINDEX*/) && 
             ((ck->TYPE == STB_TYPE_LATCH) ||
              (ck->TYPE == STB_TYPE_EVAL) ||
              (ck->TYPE == STB_TYPE_PRECHARGE) ||
              ondirective))
          {
           if ((stbfig->STABILITYMODE & STB_STABILITY_FF) != 0)
            periode = ck->PERIOD;
           else if ((stbfig->STABILITYMODE & STB_STABILITY_LT) != 0)
            periode = 0;
           else if (*(nodex->STBTAB + i) != NULL)
            {
             if (!quiet && flager == 'N')
              stb_error (ERR_MONOPHASE,
                         ttv_getsigname (stbfig->FIG, buf,
                                         event->ROOT), 0, STB_WARNING);
             flager = 'Y';
             continue;
            }
           else
            periode = ck->PERIOD;
          }
         else
          {
           periode = ck->PERIOD;
          }
        }
       else
        periode = 0;
       if ((ck->TYPE == STB_TYPE_LATCH)
           || (ck->TYPE == STB_TYPE_FLIPFLOP) ||
              (ck->TYPE == STB_TYPE_EVAL) ||
              (ck->TYPE == STB_TYPE_PRECHARGE) ||
              ondirective)
        {
         j = ck->CKINDEX;
         if ((flags & STB_TRANSFERT_NOPHASECHANGE)!=0 || j == STB_NO_INDEX)
          j = i;
        }
       else
        j = i;
      }
     else
      {
       periode = 0;
       j = i;
      }
    /* 
     input_periode=0;
     ick=stb_getclock(stbfig, i, NULL, &ckedge, NULL);
     if (ick!=NULL) input_periode=ick->PERIOD;
     
     output_periode=0;
     ick=stb_getclock(stbfig, j, NULL, &ckedge, NULL);
     if (ick!=NULL) output_periode=ick->PERIOD;
  
     stb_getmulticycleperiod(pathstartnode, node->EVENT, input_periode, output_periode, &periodeSetup, &periodeHold, &nb0, &nb1);
*/
     periodeSetup=periodeHold=0;
     if (flags & STB_TRANSFERT_NOFILTERING)
     {
       /*periodeSetup=periodeHold=*/periode = 0;
     }

     if ((ck != NULL && ck->CKINDEX != STB_NO_INDEX)
         && ((i < ptdomain->CKMIN || i > ptdomain->CKMAX) ||
             (stb_isdisable (stbfig, i, ck->CKINDEX))))
      {
       if (!quiet && *(nodex->STBTAB + i) != NULL)
        {
         stb_error (ERR_CROSSING_DOMAIN,
                    ttv_getsigname (stbfig->FIG, namebuf, event->ROOT), 0,
                    STB_WARNING);
        }
      }
     else
      {
       ptstbpair = stb_dupstbpairlist (*(nodex->STBTAB + i));
       // aplying valid stability range
       if (ptstbpair!=NULL && (vri=get_valid_range_info(ptline))!=NULL)
       {
//         if (vri[(int)i].realpair!=NULL)
         {
            stb_freestbpair(ptstbpair);
            ptstbpair=stb_dupstbpairlist(vri[(int)i].realpair);
            delaymax=delaymin=0; // already counted in realpair
         }
       }
       //---
       stb_assign_phase_to_stbpair(ptstbpair, i);
       tab[(int) j] =
        stb_transferstbpair (ptstbpair, tab[(int) j], delaymax-periodeSetup, delaymin-periodeSetup,
                             periode, periode);
       if (ptstbpair!=NULL && periodeHold>EventPeriodeHold) EventPeriodeHold=periodeHold;
      }
    }

   if(tabx != NULL)
    {
     nodex->STBTAB = tabx ;
     for (i = 0; i < nodex->NBINDEX; i++)
      {
       stb_freestbpair (tabsav[(int) i]);
       tabsav[(int) i] = NULL;
      }
    }
  }

 if ((node->SPECIN != NULL)
     && ((event->ROOT->TYPE & TTV_SIG_CT) != TTV_SIG_CT))
  {
   for (i = 0; i < node->NBINDEX; i++)
    {
     if (ck != NULL)
      {
       ptdomain = stb_getstbdomain (stbfig->CKDOMAIN, ck->CKINDEX);
       if ((ck->CKINDEX != STB_NO_INDEX)
           && ((i < ptdomain->CKMIN || i > ptdomain->CKMAX)
               || (stb_isdisable (stbfig, i, ck->CKINDEX))))
        continue;
      }

     tabpair[(int) i] = stb_mergestbpairlist (tabpair[(int) i],
                                              stb_dupstbpairlist (*
                                                                  (node->
                                                                   SPECIN +
                                                                   i)));
    }
  }

#ifdef COMPUTE_SH_ON_THE_FLY
 if (!justretransfer)
 {
   if (ck != NULL)
    {
     if ((ck->TYPE == STB_TYPE_PRECHARGE)
         || (ck->TYPE == STB_TYPE_EVAL))
      tab = tabhz;
     else
      tab = NULL;

    }
   else
    tab = NULL;

   res=stb_calcsetuphold
       (stbfig, tabpair, tab, node, &thissetup, &thishold, 1, EventPeriodeHold, NULL, NULL);
   
   if ((node->FLAG & STB_NODE_NOSETUP)==0) node->SETUP=thissetup;
   if ((node->FLAG & STB_NODE_NOHOLD)==0) node->HOLD=thishold;
   if ((node->FLAG & (STB_NODE_NOHOLD|STB_NODE_NOSETUP))==0)
   {
     if (res==0) stbfig->STABILITYFLAG = STB_UNSTABLE;
   }
   else
   {
     if (((node->FLAG & STB_NODE_NOSETUP)==0 && thissetup<=0)
         || ((node->FLAG & STB_NODE_NOHOLD)==0 && thishold<=0)) stbfig->STABILITYFLAG = STB_UNSTABLE;
   }
 }
#endif
 if (ttv_islocononlyend (stbfig->FIG, event, type) == 1)
  {
   for (i = 0; i < node->NBINDEX; i++)
    {
     stb_freestbpair (tabpair[(int) i]);
     tabpair[(int) i] = NULL;
    }
  }
 else if ((node->SPECIN != NULL)
          && ((event->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
  {
   for (i = 0; i < node->NBINDEX; i++)
    {
     if (ck != NULL)
      {
       ptdomain = stb_getstbdomain (stbfig->CKDOMAIN, ck->CKINDEX);
       if ((ck->CKINDEX != STB_NO_INDEX)
         && ((i < ptdomain->CKMIN || i > ptdomain->CKMAX) ||
             (stb_isdisable (stbfig, i, ck->CKINDEX))))
        continue;
      }
     tabpair[(int) i] = stb_mergestbpairlist (tabpair[(int) i],
                                              stb_dupstbpairlist (*
                                                                  (node->
                                                                   SPECIN +
                                                                   i)));
    }
  }

 if ((flags & STB_TRANSFERT_NOFILTERING)==0)
 {
   if (ck != NULL)
     {
       if ((ck->TYPE == STB_TYPE_LATCH)
           || (ck->TYPE == STB_TYPE_FLIPFLOP)
           || (ck->TYPE == STB_TYPE_EVAL)
           || (ck->TYPE == STB_TYPE_PRECHARGE)
           || ondirective)
         {
           /*
             if ((ck->TYPE == STB_TYPE_EVAL)
             || (ck->TYPE == STB_TYPE_PRECHARGE))
             {
             for (i = ck->CKINDEX; i < node->NBINDEX; i++)
             {
             for (ptstbpair = tabpair[(int) i]; ptstbpair != NULL;
             ptstbpair = ptstbpair->NEXT)
             {
             ptstbpair->U = ptstbpair->U - ck->PERIOD;
             ptstbpair->D = ptstbpair->D - ck->PERIOD;
             }
             }
             }*/
           if ((ck->TYPE == STB_TYPE_LATCH && (V_INT_TAB[__STB_SUPPRESS_LAG].VALUE & AVT_STB_SUPPRESS_LAG_LATCH)!=0)
               || ((ck->TYPE == STB_TYPE_EVAL || ck->TYPE == STB_TYPE_PRECHARGE) && (V_INT_TAB[__STB_SUPPRESS_LAG].VALUE & AVT_STB_SUPPRESS_LAG_PRECH)!=0))
           {
             if (ck->CKINDEX != STB_NO_INDEX)
               {
                 if (tabpair[(int)ck->CKINDEX]!=NULL)
                  {
                    stb_freestbpair(tabpair[(int) ck->CKINDEX]);
                    tabpair[(int) ck->CKINDEX]=NULL;
                    tabpair[(int)ck->CKINDEX] = stb_clockmaskstblist (stbfig, tabpair[(int)ck->CKINDEX], ck, 1, (int)ck->CKINDEX, ondirective, event);
                  }
               }
           }
           else
           {
                             
             for (i = 0; i < node->NBINDEX; i++)
               {
                 if (ck->CKINDEX != STB_NO_INDEX)
                   {
                     tabpair[(int) i] =
                       stb_clockmaskstblist (stbfig, tabpair[(int) i], ck, 0, i, ondirective,event);
                   }
               }
           }
             
           // cas d'un reset
           if (ck->CKINDEX != STB_NO_INDEX && tabpair[(int) ck->CKINDEX]==NULL && possible_set_reset)
             tabpair[(int)ck->CKINDEX]=stb_clockmaskstblist (stbfig, tabpair[(int)ck->CKINDEX], ck, 1, (int)ck->CKINDEX, ondirective,event);
           
           if ((ck->TYPE == STB_TYPE_EVAL)
               || (ck->TYPE == STB_TYPE_PRECHARGE))
             {
               /*
                 for (i = ck->CKINDEX; i < node->NBINDEX; i++)
                 {
                 for (ptstbpair = tabpair[(int) i]; ptstbpair != NULL;
                 ptstbpair = ptstbpair->NEXT)
                 {
                 ptstbpair->U = ptstbpair->U + ck->PERIOD;
                 ptstbpair->D = ptstbpair->D + ck->PERIOD;
                 }
                 }*/
               
               if ((ck->TYPE == STB_TYPE_EVAL) || 
                   (ck->TYPE == STB_TYPE_PRECHARGE))
                 {
                   ptstbpair = NULL ;
                   for (i = 0 ; i < node->NBINDEX; i++)
                     {
                       ptstbpair = tabpair[(int) i];
                       if(ptstbpair != NULL)
                         break ;
                     }
                   if(ptstbpair == NULL)
                     tabpair[(int)ck->CKINDEX] =  
                       stb_clockmaskstblist (stbfig, NULL, ck, 2, i, ondirective,event) ;
                 }
             }
         }
/*       else
         {
           long start, startmax, end, endmin, move;
           char filterstate;
           if (stb_has_filter_directive(node->EVENT))
             {
               for (i = 0; i < node->NBINDEX; i++)
                 {
                  if (stb_get_filter_directive_info(node->EVENT, i, &start, &startmax, &end, &endmin, &filterstate, &move))
                  {
                    tabpair[(int) i] =
                      stb_clockmaskstblist_simple (tabpair[(int) i], start, startmax, end, endmin, node, filterstate);
                  }
                 }
             }
         }
*/
     }
 }

 if (ondirective)
 {
   ck->CKINDEX=directive_saveckindex;
   ck->ACTIVE=directive_saveactive;
 }
}

/*****************************************************************************
*                           fonction stb_transferstbnode()                   *
*****************************************************************************/
//#define OLD_MODE_PLUS_VALID_NEW_MODE
static int stb_speed_up=0;

void stb_set_speed_up(int val)
{
  stb_speed_up=val;
}

static int stb_event_at_input( stbfig_list *stbfig, ttvevent_list *event)
{
  ttvline_list *line, *ptline;
  long type;

  if (stbfig->GRAPH == STB_RED_GRAPH)
  {
   line = event->INPATH;
   type = TTV_FIND_PATH;
  }
 else
  {
   line = event->INLINE;
   type = TTV_FIND_LINE;
  }

 for (ptline = line; ptline != NULL; ptline = ptline->NEXT)
  {
    if (((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
       (((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
       ((ptline->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
       (ttv_islineonlyend(stbfig->FIG,ptline,type) == 1)) ||
       (((ptline->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
        (ptline->FIG != stbfig->FIG)) ||
       (((ptline->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
        (ptline->FIG != stbfig->FIG) &&
        ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST))
       || ((ptline->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN)
       || ((ptline->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
    continue;

    if ((ptline->NODE->TYPE & TTV_NODE_MARK_1)==0) return 1;
  }
 return 0;
}

void stb_err()
{

}

static int stb_ismemsym(ttvsig_list *tvs)
{
  long prop=ttv_testsigflag(tvs, 0xffffffff);
  if ((tvs->TYPE & TTV_SIG_L)==0) return 0;
  if ((((unsigned)prop)>>24)==TTV_SIG_GATE_MEMSYM) return 1;
  return 0;
}

void
stb_transferstbnode (stbfig, event, flagrc)
     stbfig_list *stbfig;
     ttvevent_list *event;
     char flagrc ;
{
 stbnode *node = stb_getstbnode (event);
 stbpair_list *tabpair[256];
 stbpair_list *tabhz[256];
 stbpair_list *tabsav[256];
 stbpair_list *ptstbpair;
 stbck *ptstbck ;
 long setup ;
 long hold ;
 char i, change=0;
 char level;
 int docorrection=0;

 if (((event->ROOT->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT) ||
     ((event->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT))
  return;

 if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
  level = stbfig->FIG->INFO->LEVEL;
 else
  level = event->ROOT->ROOT->INFO->LEVEL;

 if (stbfig->GRAPH == STB_RED_GRAPH)
  {
   ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                  TTV_STS_CL_PJT, TTV_FILE_TTX);
  }
 else
  {
   ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                  TTV_STS_CLS_FED, TTV_FILE_DTX);
  }

 if (stb_speed_up && !stb_event_at_input(stbfig, event))
   {
     event->TYPE|=TTV_NODE_MARK_1;
#ifndef OLD_MODE_PLUS_VALID_NEW_MODE
     return; // stabilite inchange
#endif
   }
 else
   event->TYPE&=~TTV_NODE_MARK_1;

 ptstbck = node->CK ;

 for (i = 0; i < node->NBINDEX; i++)
  {
   tabsav[(int) i] = NULL;
  }

 for (i = 0; i < node->NBINDEX; i++)
  {
   tabpair[(int) i] = NULL;
  }

 setup = STB_NO_TIME ;
 hold = STB_NO_TIME ;

 /*
   if (strcmp(node->EVENT->ROOT->NAME,"F1867251")==0)
   printf("m");
 */
 do 
  {
    while(1)
     {
      if(ptstbck != NULL)
        {
         stb_transferstbline (stbfig, event, node , tabsav, tabhz, flagrc, 0, NULL, ptstbck, 0, docorrection?STB_TRANSFERT_CORRECTION:0) ;

         if(setup > node->SETUP)
           setup = node->SETUP ;
         else
           node->SETUP = setup ;

         if(hold > node->HOLD)
           hold = node->HOLD ;
         else
           node->HOLD = hold ;

         for (i = 0; i < node->NBINDEX; i++)
          {
           tabpair[(int)i] = stb_mergestbpairlist(tabpair[(int)i],tabsav[(int)i]) ;
           tabsav[(int) i] = NULL ;
          }
        }
      else
        stb_transferstbline (stbfig, event, node , tabpair, tabhz, flagrc, 0, NULL, NULL, 0, 0) ;

      if(ptstbck == NULL)
        break ;

      ptstbck = ptstbck->NEXT ;

      if(ptstbck == NULL)
        break ;
     }

     if (node->CK!=NULL && stb_ismemsym(event->ROOT))
     {
       // transfert des lines non commandees au cas ou il y en aurait
       int flagcorr=docorrection?STB_TRANSFERT_CORRECTION:0;
       ptstbck=node->CK;
       node->CK=NULL;
       stb_transferstbline (stbfig, event, node , tabsav, tabhz, flagrc, 0, NULL, NULL, 0, flagcorr|STB_TRANSFERT_NOT_COMMANDED) ;
       node->CK=ptstbck;
       for (i = 0; i < node->NBINDEX; i++)
          {
           tabpair[(int)i] = stb_mergestbpairlist(tabpair[(int)i],tabsav[(int)i]) ;
           tabsav[(int) i] = NULL ;
          }
     }

//     node->CK = ptstbck ;
     if (V_BOOL_TAB[__STB_STABILITY_CORRECTION].VALUE)
     {
       if (docorrection)
       {
         docorrection=0;
         node->FLAG|=STB_NODE_STABCORRECT;
       }
       else
       {
         // datalag avec fauxchemin ou faux slack?
         if ((node->EVENT->ROOT->TYPE & TTV_SIG_L)!=0 || (node->EVENT->ROOT->TYPE & TTV_SIG_R)!=0)
         {
           long lagmin, lagmax;
           stb_getstbdelta(stbfig, node->EVENT, &lagmin, &lagmax, NULL);
           if ((lagmin>0 || lagmax>0 || ((node->EVENT->ROOT->TYPE & TTV_SIG_R)!=0)) && (ttv_canbeinfalsepath(node->EVENT, 'o') || stb_hasfalseslack(stbfig, node->EVENT)))
           {
#ifdef RELAX_CORRECT_DEBUG
              printf("should analyse %s (%lx) lagmax=%ld \n",node->EVENT->ROOT->NAME,node->EVENT->TYPE,lagmax);
#endif            
              docorrection=1;
              for (i = 0; i < node->NBINDEX; i++)
              {
                 tabsav[(int) i] = NULL;
                 stb_freestbpair(tabpair[(int) i]);
                 tabpair[(int) i] = NULL;
              }
              ptstbck = node->CK ;
           }
         }
       }
     }
   } while (docorrection);


 for (i = 0; i < node->NBINDEX; i++)
  {

   if (stbfig->ANALYSIS == STB_GLOB_ANALYSIS)
    {
     ptstbpair = stb_globalstbpair (tabpair[(int) i]);
     stb_freestbpair (tabpair[(int) i]);
     tabpair[(int) i] = ptstbpair;
     if (node->CK != NULL)
      if ((node->CK->TYPE == STB_TYPE_PRECHARGE)
          || (node->CK->TYPE == STB_TYPE_EVAL))
       {
        ptstbpair = stb_globalstbpair (tabhz[(int) i]);
        stb_freestbpair (tabhz[(int) i]);
        tabhz[(int) i] = ptstbpair;
       }
    }
#if 0
//OLD_MODE_PLUS_VALID_NEW_MODE
     if (stb_speed_up && !stb_event_at_input(stbfig, event) && stb_compstbpairlist (tabpair[(int) i], *(node->STBTAB + i)) == 0)
       {
         stb_err();
       }
#endif
   if (node->CK != NULL)
    {
     if (((node->CK->TYPE == STB_TYPE_LATCH)
          || (node->CK->TYPE == STB_TYPE_FLIPFLOP) ||
           (node->CK->TYPE == STB_TYPE_PRECHARGE) ||
           (node->CK->TYPE == STB_TYPE_EVAL))
         && (node->CK->CKINDEX != STB_NO_INDEX))
      {
        change|=0x10;
        if (stb_compstbpairlist (tabpair[(int) i], *(node->STBTAB + i)) == 0)
          {
/*            if (strcmp(node->EVENT->ROOT->NAME,"b0qr4<0>")==0)
            {
              long d, u;
              u=tabpair[(int) i]->U-(*(node->STBTAB + i))->U;
              d=tabpair[(int) i]->D-(*(node->STBTAB + i))->D;
              printf("%s %s %s p:%d   d|u: %d|%d    (%d %d) (%d %d)\n",node->CK->TYPE == STB_TYPE_LATCH?"L":"P",
                              node->EVENT->ROOT->NAME, (node->EVENT->TYPE & TTV_NODE_UP)!=0?"R":"F", i, d, u, (*(node->STBTAB + i))->D, (*(node->STBTAB + i))->U,
                              tabpair[(int) i]->D, tabpair[(int) i]->U);
              if (d>0 || u<0)
                    stb_err();
            }*/
            stbfig->CHANGEFLAG++;
            change|=0x1;
          }
      }
    }
/*
   if (strcmp(node->EVENT->ROOT->NAME,"F1867251")==0 && tabpair[(int) i]!=NULL)
   {
     printf("<<<<%s <%d %d>\n", node->EVENT->ROOT->NAME, tabpair[(int) i]->D, tabpair[(int) i]->U);
   }
   */
   stb_freestbpair (*(node->STBTAB + i));
   *(node->STBTAB + i) = tabpair[(int) i];

   if (node->CK != NULL)
    if ((node->CK->TYPE == STB_TYPE_PRECHARGE)
        || (node->CK->TYPE == STB_TYPE_EVAL))
     {
      stb_freestbpair (*(node->STBHZ + i));
      *(node->STBHZ + i) = tabhz[(int) i];
     }
  }
 
 if (stb_speed_up && change==0x10) event->TYPE|=TTV_NODE_MARK_1;
   
}

/*****************************************************************************
*                           fonction stb_getdelayedge()                      *
*****************************************************************************/
void
stb_getdelayedge(stbfig_list *stbfig, ttvevent_list *event, char phaseevent, long *tmax, long *tmin, char phasefilter, int dataclock, char *phasemax, char *phasemin)
{
  stbnode       *node ;
  stbdomain_list *domain ;
  stbdomain_list sdomain ;
  stbpair_list  *pair ;
  int            phase ;
  long           d ;
  long           u ;
  stbck         *ck ;
  long           lmin =  LONG_MAX ;
  long           lmax = -LONG_MAX ;
  long           period ;
  long           per ;

  if( tmax ) *tmax = TTV_NOTIME ;
  if( tmin ) *tmin = TTV_NOTIME ;
  if (phasemax) *phasemax=STB_NO_INDEX;
  if (phasemin) *phasemin=STB_NO_INDEX;

  if( !stbfig || !event )
    return ;
    
  node = stb_getstbnode( event );

  if( !node )
    return ;

  if(phaseevent != STB_NO_INDEX)
   {
    domain = stb_getstbdomain( stbfig->CKDOMAIN, phaseevent );
    period = stb_getperiod(stbfig, phaseevent) ;
   }
  else
   {
    domain = &sdomain ;
    domain->CKMIN = 0 ;
    domain->CKMAX = node->NBINDEX -1 ;
    period = 0 ;
   }
 
  if (dataclock==0 || (dataclock & 1)!=0)
  {
    if( node->STBTAB ) {
      for( phase = domain->CKMIN ; phase <= domain->CKMAX ; phase++ ) {
        if (phasefilter!=STB_NO_INDEX && phase!=phasefilter) continue;
        pair = node->STBTAB[ phase ] ;
        if(stb_cmpphase(stbfig,phase,phaseevent)>=0/* phase >= phaseevent*/) {
           if(stb_cmpphase(stbfig,phase,phaseevent)==0/*phase == phaseevent*/) {
              if((stbfig->STABILITYMODE & STB_STABILITY_FF) != 0)
                 per = period ;
              else if((stbfig->STABILITYMODE & STB_STABILITY_LT) != 0)
                 per = (long)0 ;
              }
           else per = period ;
        }
        else {
         per = (long)0 ;
        }
        if ( pair ) {
          if ( tmin ) {
            d = pair->D - per ;
            if( d < lmin ) 
            {
              lmin = d ;
              if (phasemin) *phasemin=phase;
            }
          }
          if ( tmax ) {
            for( ; pair->NEXT ; pair = pair->NEXT ) ;
            u = pair->U - per ;
            if( u > lmax )
            {
              lmax = u ;
              if (phasemax) *phasemax=phase;
            }
          }
        }
      }
    }
  }

  if (dataclock==0 || (dataclock & 2)!=0)
  {
    for( ck = node->CK ; ck ; ck = ck->NEXT ) {
      if((ck->TYPE != STB_TYPE_CLOCK) && (ck->TYPE != STB_TYPE_CLOCKPATH) &&
         (ck->TYPE != STB_TYPE_COMMAND))
          break ;
      phase=ck->CKINDEX;
      if (phase==STB_NO_INDEX) phase=ck->CTKCKINDEX;
      if (phasefilter!=STB_NO_INDEX && phase!=phasefilter) continue;
      if(ck->SUPMIN!=STB_NO_TIME && (ck->SUPMIN < lmin) && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
      {
        lmin = ck->SUPMIN ;
        if (phasemin) *phasemin=phase;
      }
      if(ck->SDNMIN!=STB_NO_TIME && (ck->SDNMIN < lmin) && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
      {
        lmin = ck->SDNMIN ;
        if (phasemin) *phasemin=phase;
      }
      if(ck->SUPMAX!=STB_NO_TIME && (ck->SUPMAX > lmax) && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
      {
        lmax = ck->SUPMAX ;
        if (phasemax) *phasemax=phase;
      }
      if(ck->SDNMAX!=STB_NO_TIME && (ck->SDNMAX > lmax) && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
      {
        lmax = ck->SDNMAX ;
        if (phasemax) *phasemax=phase;
      }
    }
  }

  if( tmax && lmax != -LONG_MAX )
    *tmax = lmax ;
  if( tmin && lmin !=  LONG_MAX )
    *tmin = lmin ;
}

/*****************************************************************************
*                           fonction stb_geteventphase()                     *
*****************************************************************************/
void
stb_geteventphase( stbfig_list   *stbfig, ttvevent_list *event, char *pphase, char *phmax, char *phmin, int clock, int data)
{
  char             phase ;
  stbdomain_list *domain ;
  stbnode        *node ;
  stbck            *ck ;
  char             nullmax ;
  char             nullmin ;
  char             nullphase ;
 
  if( !phmax ) 
phmax = &nullmax ;
  if( !phmin ) 
    phmin = &nullmin ;
  if( !pphase ) 
    pphase = &nullphase ;

  *pphase = STB_NO_INDEX ;
  *phmax  = STB_NO_INDEX ;
  *phmin  = STB_NO_INDEX ;
  
  if( !event || !stbfig )
    return ;

  node = stb_getstbnode( event );
  if( !node )
    return ;

  if (clock)
  {
  for(ck = node->CK ; ck != NULL ; ck = ck->NEXT)
    if(ck->CKINDEX != STB_NO_INDEX)
     {
      domain = stb_getstbdomain( stbfig->CKDOMAIN, ck->CKINDEX );
      *phmin  = domain->CKMIN ;
      *phmax  = domain->CKMAX ;
      *pphase = ck->CKINDEX ;
      break ;
     }
  }
  
  if( data && (node->STBTAB && (!clock || !ck))) {
    for( phase = 0 ; phase < node->NBINDEX ; phase++ ) {
      if( node->STBTAB[(int)phase] ) {
        domain = stb_getstbdomain( stbfig->CKDOMAIN, phase );
        if( domain ) {
          *phmin  = domain->CKMIN ;
          *phmax  = domain->CKMAX ;
          *pphase = phase ;
          break ;
        }
      }
    }
  }
}

/*****************************************************************************
*                           fonction stb_getlatchslope()                     *
*****************************************************************************/
void
stb_getlatchslope(stbfig_list   *stbfig,ttvevent_list *latch,ttvevent_list *cmd,long *slopeopenmax,long *slopeopenmin,long *slopeclosemax,long *slopeclosemin,long *period,int addaccess, char phase, long *move)
{
 stbnode *ptstbnode = stb_getstbnode(latch) ;
 stbck *clock ;
 char active ;
 long lslopeopenmax ;
 long lslopeopenmin ;
 long lslopeclosemax ;
 long lslopeclosemin ;
 long lperiod ;

 if(slopeopenmax == NULL) slopeopenmax = &lslopeopenmax ;
 if(slopeopenmin == NULL) slopeopenmin = &lslopeopenmin ;
 if(slopeclosemax == NULL) slopeclosemax = &lslopeclosemax ;
 if(slopeclosemin == NULL) slopeclosemin = &lslopeclosemin ;
 if(period == NULL) period = &lperiod ;
 
 stb_getstbck(ptstbnode,cmd) ;

 active = ptstbnode->CK->ACTIVE ;
 clock = ptstbnode->CK ;
 *move=0;

 if(clock == NULL)
   {
    *slopeopenmin = STB_NO_TIME ;
    *slopeopenmax = STB_NO_TIME ;
    *slopeclosemin = STB_NO_TIME ;
    *slopeclosemax = STB_NO_TIME ;
    *period = STB_NO_TIME ;
    return ;
   }

 *period = clock->PERIOD ;

 if ((active & STB_SLOPE) == STB_SLOPE)
  {
   if ((active & STB_SLOPE_UP) == STB_SLOPE_UP)
    {
     *slopeopenmin = clock->SUPMIN;
     *slopeopenmax = clock->SUPMAX;
     *slopeclosemin = clock->SUPMIN;
     *slopeclosemax = clock->SUPMAX;
    }
   else
    {
     *slopeopenmin = clock->SDNMIN;
     *slopeopenmax = clock->SDNMAX;
     *slopeclosemin = clock->SDNMIN;
     *slopeclosemax = clock->SDNMAX;
    }
  }
 else if ((active & STB_STATE) == STB_STATE)
  {
   if ((active & STB_STATE_UP) == STB_STATE_UP)
    {
     *slopeopenmin = clock->SUPMIN;
     *slopeopenmax = clock->SUPMAX;
     *slopeclosemin = clock->SDNMIN;
     *slopeclosemax = clock->SDNMAX;
    }
   else
    {
     *slopeopenmin = clock->SDNMIN;
     *slopeopenmax = clock->SDNMAX;
     *slopeclosemin = clock->SUPMIN;
     *slopeclosemax = clock->SUPMAX;
    }
  }

 if(STB_OPEN_LATCH_PHASE == 'N')
   {
    if(*slopeopenmin > *slopeclosemin)
        *slopeopenmin = *slopeopenmin - *period ;
    if(*slopeopenmax > *slopeclosemax)
        *slopeopenmax = *slopeopenmax - *period ;
   }
 else
   {
    if(*slopeopenmin > *slopeclosemin)
        *slopeclosemin = *slopeclosemin + *period ;
    if(*slopeopenmax > *slopeclosemax)
        *slopeclosemax = *slopeclosemax + *period ;
   }
 if (phase!=STB_NO_INDEX && stb_cmpphase(stbfig,phase,clock->CKINDEX)>0/*phase>clock->CKINDEX*/)
 {
   *move=stb_synchronize_slopes(stbfig, phase, clock->CKINDEX, *period, 0);
   *slopeopenmin+=*move;
   *slopeopenmax+=*move;
   *slopeclosemin+=*move;
   *slopeclosemax+=*move;
 }
 if (addaccess)
 {
    *slopeopenmin+=clock->ACCESSMIN;
    *slopeopenmax+=clock->ACCESSMAX;
/*    *slopeclosemin+=clock->ACCESSMIN;
    *slopeclosemax+=clock->ACCESSMAX;*/
 }
}

int stb_get_setup_and_hold_test_events(stbck *clock, int hz, ttvevent_list **setup_event, ttvevent_list **hold_event, stbnode *node, int *setup_mustbehz, int *hold_mustbehz)
{
  long active;
  ptype_list *pt;
  
  *setup_event=*hold_event=NULL;
  *setup_mustbehz=*hold_mustbehz=0;

  if(STB_OPEN_LATCH_PHASE == 'N') return 0; // <- on ne sais pas faire

  if (clock==NULL) return 0;
  if (node!=NULL && node->SPECOUT!=NULL) return 1;

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

  if ((pt=getptype(node->EVENT->USER, STB_BREAK_TEST_EVENT))!=NULL)
  {
    *setup_event=*hold_event=(ttvevent_list *)pt->DATA;
  }
  else if ((node->EVENT->ROOT->TYPE & TTV_SIG_R)==0)
  {
    if (clock->CMD==NULL) return 0;
    if (!hz && (active & STB_SLOPE) == STB_SLOPE)
      {
        *setup_event=*hold_event=clock->CMD;
      }
    else if ((active & STB_STATE) == STB_STATE)
      {
        *hold_event=ttv_opposite_event(clock->CMD);
        if (!hz && (clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
          *setup_event=*hold_event;
        else
          *setup_event=clock->CMD;
      }
  }
  else
  {
    if (hz)
    {
      if ((node->CK->FLAGS & STBCK_MAX_EDGE_HZ)!=0) *hold_event=ttv_opposite_event(node->EVENT), *hold_mustbehz=1;
      else *hold_event=node->EVENT;
      if ((node->CK->FLAGS & STBCK_MIN_EDGE_HZ)!=0) *setup_event=ttv_opposite_event(node->EVENT), *setup_mustbehz=1;
      else *setup_event=node->EVENT;
    }
    else
    {
      if ((node->CK->FLAGS & STBCK_MAX_EDGE_NOT_HZ)!=0) *hold_event=ttv_opposite_event(node->EVENT);
      else *hold_event=node->EVENT, *hold_mustbehz=1;
      if ((node->CK->FLAGS & STBCK_MIN_EDGE_NOT_HZ)!=0) *setup_event=ttv_opposite_event(node->EVENT);
      else *setup_event=node->EVENT, *setup_mustbehz=1;
    }
  }
  return 1;
}

long stb_getsetupslope(stbfig_list *stbfig, stbnode *node, stbck *ck, int hz, char *phase)
{
  long slopesetup, slopemovesetup;
  long period;
  char index;
  char active;
  int i;

  if (V_BOOL_TAB[__STB_ENBALE_COMMAND_CHECK].VALUE==0 &&
      ((node->EVENT->ROOT->TYPE & TTV_SIG_Q)==TTV_SIG_Q 
       || ((node->EVENT->ROOT->TYPE & TTV_SIG_R)==TTV_SIG_R && (node->EVENT->TYPE & TTV_NODE_UP)!=0))
      ) 
    return TTV_NOTIME;

  if (node->SPECOUT != NULL)
    {
      stbpair_list *pair[256];
      stbpair_list *globalverif;

      for (i=0; i<node->NBINDEX; i++)
        {
          pair[i]=stb_dupstbpairlist (node->SPECOUT[i]);
          stb_assign_phase_to_stbpair(pair[i], i);
        }

      globalverif = stb_nodeglobalpair (pair, period, node->NBINDEX, STB_NO_INDEX);
      slopesetup=globalverif->U;
      *phase=globalverif->phase_U;
      stb_freestbpair (globalverif);
      stb_freestbtabpair(pair, node->NBINDEX);
      return slopesetup;
    }

  if (ck != NULL)
    {
      active = ck->ACTIVE;
      if ((ck->TYPE == STB_TYPE_COMMAND)
          && ((ck->VERIF & STB_NO_VERIF) != STB_NO_VERIF))
        {
          if ((ck->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
            {
              if ((((ck->VERIF & STB_DN) == STB_DN) && ((ck->VERIF & STB_VERIF_STATE) == STB_VERIF_STATE))
                  || (((ck->VERIF & STB_UP) == STB_UP) && ((ck->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)))
                active = STB_STATE_DN;
            }
          else if ((ck->ACTIVE & STB_STATE_DN) == STB_STATE_DN)
            {
              if ((((ck->VERIF & STB_UP) == STB_UP) && ((ck->VERIF & STB_VERIF_STATE) == STB_VERIF_STATE))
                  || (((ck->VERIF & STB_DN) == STB_DN) && ((ck->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)))
                active = STB_STATE_UP;
            }
        }
      if ((ck->VERIF & STB_NO_VERIF) == STB_NO_VERIF)
        {
          return TTV_NOTIME;
        }
      if ((active & STB_SLOPE) == STB_SLOPE)
        {
          if ((active & STB_SLOPE_UP) == STB_SLOPE_UP)
            return ck->SUPMIN-ck->SETUP;
          else
            return ck->SDNMIN-ck->SETUP;
        }

      *phase=ck->CKINDEX;
      if (!hz)
        {
          if ((active & STB_STATE) == STB_STATE)
            {
              if ((active & STB_STATE_UP) == STB_STATE_UP)
                {
                  if ((ck->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
                    slopemovesetup=(ck->SUPMIN < ck->SDNMIN) ?0:ck->PERIOD, slopesetup=ck->SDNMIN+slopemovesetup;
                  else
                    slopesetup = ck->SUPMIN;
                  return slopesetup-ck->SETUP;
                }
              else
                {
                  if ((ck->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE && !ttv_has_strict_setup(node->EVENT))
                    slopemovesetup=(ck->SDNMIN < ck->SUPMIN) ?0:ck->PERIOD, slopesetup = ck->SUPMIN+slopemovesetup;
                  else
                    slopesetup = ck->SDNMIN ;
                  return slopesetup-ck->SETUP;
                }
            }
        }
      else
        {
          if ((active & STB_STATE_UP) == STB_STATE_UP)
            return ck->SUPMIN-ck->SETUP ;
          else
            return ck->SDNMIN-ck->SETUP ;
        }
    }
  return TTV_NOTIME;
}

