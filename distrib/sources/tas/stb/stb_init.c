/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_init.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>

#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_error.h"
#include "stb_init.h"
#include "stb_transfer.h"
#include "stb_overlap.h"
#include "stb_clock_tree.h"
#include "stb_clocks.h"

/*****************************************************************************
* global variables                                                           *
*****************************************************************************/

#define STRIP_CMD

long  STB_DEF_SLEW=STB_NO_TIME;

/*****************************************************************************
*                           fonction stb_levelise()                          *
*****************************************************************************/
chain_list *
stb_levelise (stbfig)
     stbfig_list *stbfig;
{
 chain_list *chainnode = NULL;
 long level;
 long type;

 if (stbfig->GRAPH == STB_RED_GRAPH)
  {
   type = TTV_FIND_PATH ;
  }
 else if (stbfig->GRAPH == STB_DET_GRAPH)
  {
   type = TTV_FIND_LINE ;
  }

 if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
  level = stbfig->FIG->INFO->LEVEL;
 else
  level = 0 ;

 chainnode = ttv_levelise(stbfig->FIG,level,type) ;

 return (chainnode);
}

/*****************************************************************************
*                           fonction stb_initnode()                          *
*****************************************************************************/
void
stb_initnode (stbfig)
     stbfig_list *stbfig;
{
 ttvevent_list *event;
 chain_list *chain;

 for (chain = stbfig->NODE; chain; chain = chain->NEXT)
  {
   event = (ttvevent_list *) chain->DATA;
   if (stb_getstbnode (event) == NULL)
    stb_addstbnode (event);
  }
}

/*****************************************************************************
*                           fonction stb_init()                              *
*****************************************************************************/
void
stb_init (stbfig)
     stbfig_list *stbfig;
{
 stbfig->NODE = stb_levelise (stbfig);
 stb_initnode (stbfig);
}

/*****************************************************************************
*                           fonction stb_initckpath()                        *
*****************************************************************************/
void stb_init_backward (stbfig_list *stbfig, ttvevent_list *event)
{
  stbnode *node;
  stbnode *cknode;
  ttvevent_list *ckevent;
  ttvline_list *line;
  char level;

  if((node = stb_getstbnode (event)) == NULL)
    return ;

  if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL;
  else
    level = event->ROOT->ROOT->INFO->LEVEL;

  if (stbfig->GRAPH == STB_RED_GRAPH)
    {
      ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                     TTV_STS_CL_PJT, TTV_FILE_TTX);
      line = event->INPATH;
    }
  else
    {
      ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                     TTV_STS_CLS_FED, TTV_FILE_DTX);
      line = event->INLINE;
    }

  for (; line; line = line->NEXT)
    {
      if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
          (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
           (line->FIG != stbfig->FIG)) ||
          (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
           (line->FIG != stbfig->FIG) &&
           ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
        continue;

      ckevent = line->NODE;
      cknode = stb_getstbnode (ckevent);
      if (((event->ROOT->TYPE & TTV_SIG_R) != 0 && (event->TYPE & TTV_NODE_UP)!=0)
          || (node->FLAG & STB_NODE_DATA_ON_PRECHARGE)!=0)
         cknode->FLAG|=STB_NODE_DATA_ON_PRECHARGE;
    }
}

int
stb_initckpath (stbfig, event)
     stbfig_list *stbfig;
     ttvevent_list *event;
{
  stbnode *node;
  stbnode *cknode;
  ttvevent_list *ckevent, *originalclock;
  ttvline_list *line;
  long delaymax;
  long delaymin;
  long sumax = STB_NO_TIME;
  long sdmax = STB_NO_TIME;
  long sumin = STB_NO_TIME;
  long sdmin = STB_NO_TIME;
  long period;
  char fumax = 'N';
  char fdmax = 'N';
  char fumin = 'N';
  char fdmin = 'N';
  char active;
  char flag;
  char namebuf[1024];
  char index;
  char level;
  stbnode *stbevent;
  char ctkindex;
  int onclockpath, onfilterdirectivepath;
  int change=0;

  if ((event->ROOT->TYPE & TTV_SIG_Q)!=0)
  {
    ptype_list *ptype;
    if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
     {
        ckevent = (ttvevent_list *) ptype->DATA;
        index = stb_getstbnode (ckevent)->CK->CKINDEX;
        stb_getstbnode (event)->CK->CTKCKINDEX = index;
     }
  }
  
  if ((event->ROOT->TYPE & (TTV_SIG_L | TTV_SIG_Q | TTV_SIG_R | TTV_SIG_B)) !=
      0)
    return 0;

  if((node = stb_getstbnode (event)) == NULL)
    return 0;

  flag = 'N';
  period = STB_NO_TIME;
  ctkindex = STB_NO_INDEX;
  onclockpath=onfilterdirectivepath=0;
  originalclock=NULL;

  if (node->CK!=NULL)
    {
      if(node->CK->SUPMAX != STB_NO_TIME && node->CK->SDNMAX != STB_NO_TIME && node->CK->SUPMIN != STB_NO_TIME && node->CK->SDNMIN != STB_NO_TIME)
        {
          node->CK->CTKCKINDEX=stb_getstbnode(node->CK->ORIGINAL_CLOCK)->CK->CKINDEX;
          return 1;
        }

      if(node->CK->SUPMAX != STB_NO_TIME || node->CK->SDNMAX != STB_NO_TIME || node->CK->SUPMIN != STB_NO_TIME || node->CK->SDNMIN != STB_NO_TIME)
        {
          if ((index=node->CK->CKINDEX)!=STB_NO_INDEX)
            period=node->CK->PERIOD;
        }
    }

  if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL;
  else
    level = event->ROOT->ROOT->INFO->LEVEL;

  if (stbfig->GRAPH == STB_RED_GRAPH)
    {
      ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                     TTV_STS_CL_PJT, TTV_FILE_TTX);
      line = event->INPATH;
    }
  else
    {
      ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                     TTV_STS_CLS_FED, TTV_FILE_DTX);
      line = event->INLINE;
    }

  
  for (; line; line = line->NEXT)
    {
      if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
          (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
           (line->FIG != stbfig->FIG)) ||
          (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
           (line->FIG != stbfig->FIG) &&
           ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
        continue;

      ckevent = line->NODE;
      if ((ckevent->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_R)) != 0) continue;
      cknode = stb_getstbnode (ckevent);
/*
      if ((cknode->FLAG & STB_NODE_DATA_ON_PRECHARGE)!=0)
        node->FLAG|=STB_NODE_DATA_ON_PRECHARGE;
*/
      if (cknode->CK == NULL)
        continue;

      if (cknode->CK->CTKCKINDEX==STB_NO_INDEX && cknode->CK->ORIGINAL_CLOCK!=NULL)
       {
         cknode->CK->CTKCKINDEX=stb_getstbnode(cknode->CK->ORIGINAL_CLOCK)->CK->CKINDEX;
       }


      if (stb_has_filter_directive(event))
      {
        /*if (cknode->CK->FLAGS & STBCK_ONFILTER_PATH) continue;
        else*/ onfilterdirectivepath=1;
      }
              
      if (cknode->CK->FLAGS & STBCK_ONCLOCK_PATH) onclockpath=1;
      if (cknode->CK->FLAGS & STBCK_ONFILTER_PATH) onfilterdirectivepath=1;
      

      flag = 'Y';
      if ((period != STB_NO_TIME) && (cknode->CK->CKINDEX != index))
        {          
          stb_error (ERR_MULTIPLE_CKPATH,
                     ttv_getsigname (stbfig->FIG, namebuf, event->ROOT), 0,
                     STB_WARNING);
          continue;
        }
      else
        {
          period = cknode->CK->PERIOD;
          index = cknode->CK->CKINDEX;
          if (cknode->CK->CTKCKINDEX!=STB_NO_INDEX)
            ctkindex= cknode->CK->CTKCKINDEX;
          else
            ctkindex = stb_getckindex( cknode->CK );
        }

      if (cknode->CK->ORIGINAL_CLOCK!=NULL && originalclock==NULL)
         originalclock=cknode->CK->ORIGINAL_CLOCK;
      
      delaymin = ttv_getdelaymin (line);
      delaymax = ttv_getdelaymax (line);

      if (originalclock!=NULL && getptype(originalclock->ROOT->USER, STB_IDEAL_CLOCK)!=NULL)
        delaymin=delaymax=0;

      if (delaymin == TTV_NOTIME)
        delaymin = delaymax;

      if (delaymax == TTV_NOTIME)
        delaymax = delaymin;

      if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        {
          if( cknode->CK->SUPMAX != STB_NO_TIME ) {
            if (fumax == 'N')
              sumax = cknode->CK->SUPMAX + delaymax;
            else
              {
                if (sumax < (cknode->CK->SUPMAX + delaymax))
                  sumax = cknode->CK->SUPMAX + delaymax;
              }
            fumax = 'Y';
          }
          if( cknode->CK->SUPMIN != STB_NO_TIME ) {
            if (fumin == 'N')
              sumin = cknode->CK->SUPMIN + delaymin;
            else
              {
                if (sumin > (cknode->CK->SUPMIN + delaymin))
                  sumin = cknode->CK->SUPMIN + delaymin;
              }
            fumin = 'Y';
          }
        }
      if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
          ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        {
          if( cknode->CK->SDNMAX != STB_NO_TIME ) {
            if (fumax == 'N')
              sumax = cknode->CK->SDNMAX + delaymax;
            else
              {
                if (sumax < (cknode->CK->SDNMAX + delaymax))
                  sumax = cknode->CK->SDNMAX + delaymax;
              }
            fumax = 'Y';
          }
          if( cknode->CK->SDNMIN != STB_NO_TIME ) {
            if (fumin == 'N')
              sumin = cknode->CK->SDNMIN + delaymin;
            else
              {
                if (sumin > (cknode->CK->SDNMIN + delaymin))
                  sumin = cknode->CK->SDNMIN + delaymin;
              }
            fumin = 'Y';
          }
        }
      if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
          ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        {
          if( cknode->CK->SUPMAX != STB_NO_TIME ) {
            if (fdmax == 'N')
              sdmax = cknode->CK->SUPMAX + delaymax;
            else
              {
                if (sdmax < (cknode->CK->SUPMAX + delaymax))
                  sdmax = cknode->CK->SUPMAX + delaymax;
              }
            fdmax = 'Y';
          }
          if( cknode->CK->SUPMIN != STB_NO_TIME ) {
            if (fdmin == 'N')
              sdmin = cknode->CK->SUPMIN + delaymin;
            else
              {
                if (sdmin > (cknode->CK->SUPMIN + delaymin))
                  sdmin = cknode->CK->SUPMIN + delaymin;
              }
            fdmin = 'Y';
          }
        }
      if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
          ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        {
          if( cknode->CK->SDNMAX != STB_NO_TIME ) {
            if (fdmax == 'N')
              sdmax = cknode->CK->SDNMAX + delaymax;
            else
              {
                if (sdmax < (cknode->CK->SDNMAX + delaymax))
                  sdmax = cknode->CK->SDNMAX + delaymax;
              }
            fdmax = 'Y';
          }
          if( cknode->CK->SDNMIN != STB_NO_TIME ) {
            if (fdmin == 'N')
              sdmin = cknode->CK->SDNMIN + delaymin;
            else
              {
                if (sdmin > (cknode->CK->SDNMIN + delaymin))
                  sdmin = cknode->CK->SDNMIN + delaymin;
              }
            fdmin = 'Y';
          }
        }
    }

  if (flag == 'Y')
    {
      if ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        active = STB_SLOPE_UP;
      else
        active = STB_SLOPE_DN;

      if( fdmax == 'Y' || fdmin == 'Y' || fumax == 'Y' || fumin == 'Y' ) {
        stbnode *opev;
        stbevent = stb_getstbnode (event);
        if (stbevent->CK==NULL)
          stb_addstbck (stbevent, sumin, sumax, sdmin, sdmax, period,
                        stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX, active,
                        STB_NO_VERIF, STB_TYPE_CLOCKPATH, originalclock);
        else
          {
            if (sumin!=STB_NO_TIME) stbevent->CK->SUPMIN=sumin;
            if (sumax!=STB_NO_TIME) stbevent->CK->SUPMAX=sumax;
            if (sdmin!=STB_NO_TIME) stbevent->CK->SDNMIN=sdmin;
            if (sdmax!=STB_NO_TIME) stbevent->CK->SDNMAX=sdmax;
          }
        if( stbevent->CK ) stbevent->CK->CTKCKINDEX = ctkindex ;
        if (onclockpath) stbevent->CK->FLAGS|=STBCK_ONCLOCK_PATH;
        if (onfilterdirectivepath) stbevent->CK->FLAGS|=STBCK_ONFILTER_PATH;

        opev = stb_getstbnode (ttv_opposite_event(event));
        if (opev->CK!=NULL)
          {
            if (stbevent->CK->SDNMIN!=STB_NO_TIME && opev->CK->SDNMIN!=stbevent->CK->SDNMIN) opev->CK->SDNMIN=stbevent->CK->SDNMIN;
            else if (opev->CK->SDNMIN!=STB_NO_TIME && opev->CK->SDNMIN!=stbevent->CK->SDNMIN) stbevent->CK->SDNMIN=opev->CK->SDNMIN;
            if (stbevent->CK->SDNMAX!=STB_NO_TIME && opev->CK->SDNMAX!=stbevent->CK->SDNMAX) opev->CK->SDNMAX=stbevent->CK->SDNMAX;
            else if (opev->CK->SDNMAX!=STB_NO_TIME && opev->CK->SDNMAX!=stbevent->CK->SDNMAX) stbevent->CK->SDNMAX=opev->CK->SDNMAX;
            if (stbevent->CK->SUPMIN!=STB_NO_TIME && opev->CK->SUPMIN!=stbevent->CK->SUPMIN) opev->CK->SUPMIN=stbevent->CK->SUPMIN;
            else if (opev->CK->SUPMIN!=STB_NO_TIME && opev->CK->SUPMIN!=stbevent->CK->SUPMIN) stbevent->CK->SUPMIN=opev->CK->SUPMIN;
            if (stbevent->CK->SUPMAX!=STB_NO_TIME && opev->CK->SUPMAX!=stbevent->CK->SUPMAX) opev->CK->SUPMAX=stbevent->CK->SUPMAX;
            else if (opev->CK->SUPMAX!=STB_NO_TIME && opev->CK->SUPMAX!=stbevent->CK->SUPMAX) stbevent->CK->SUPMAX=opev->CK->SUPMAX;
          }
        change=1;
//        printf("$ %s %d %d %d %d\n",event->ROOT->NAME, stbevent->CK->SDNMIN, stbevent->CK->SDNMAX, stbevent->CK->SUPMIN, stbevent->CK->SUPMAX);
      }
    }
  return change;
}

void stb_propagate_signal(stbfig_list *sb, chain_list *clocks)
{
 ttvline_list *ptline ;
 long level ;
 long dual ;
 chain_list *cl = NULL ;
 ptype_list *ptype ;
 chain_list *chain ;
 ttvevent_list *event;
 ttvsig_list *tvs;
 
 level = 1 ;

 dual = TTV_STS_DUAL_F|TTV_STS_DUAL_E|TTV_STS_DUAL_D ;


 for (cl=clocks; cl!=NULL; cl=cl->NEXT)
   {
     tvs=(ttvsig_list *)cl->DATA;
     ttv_fifopush(&tvs->NODE[0]);
     ttv_fifopush(&tvs->NODE[1]);
   }

 while ((event=ttv_fifopop())!=NULL)
   {
     
     ttv_expfigsig(sb->FIG,event->ROOT,level,sb->FIG->INFO->LEVEL,
                   TTV_STS_CLS_FED|dual, TTV_FILE_DTX);
     
     ptype = getptype(event->USER,TTV_NODE_DUALLINE) ;

     if(ptype == NULL) chain = NULL ;
     else chain = (chain_list *)ptype->DATA ;

     while (chain!=NULL)
       {
         ptline=(ttvline_list *)chain->DATA;
         if((ptline->TYPE & TTV_LINE_CONT) == 0)
           {
             if (stb_initckpath (sb, ptline->ROOT)
                 || ((ptline->ROOT->ROOT->TYPE & TTV_SIG_Q)==TTV_SIG_Q
                     || (ptline->ROOT->ROOT->TYPE & TTV_SIG_B)==TTV_SIG_B))
               ttv_fifopush(ptline->ROOT);
           }
         chain=chain->NEXT;
       }     
   }
 ttv_fifoclean();
}

/*****************************************************************************
*                           fonction stb_filterbadpath()                     *
*****************************************************************************/
ttvpath_list *
stb_filterbadpath (stbfig, ptsig, path)
     stbfig_list *stbfig;
     ttvsig_list *ptsig;
     ttvpath_list *path;
{
 chain_list *chain;
 ttvsig_list *clock;
 ttvpath_list *ptpath;
 ttvpath_list *ptsav;
 char name[1024];

 for (chain = stbfig->PRIOCLOCK; chain; chain = chain->NEXT)
  {
   if (mbk_TestREGEX(ttv_getsigname (stbfig->FIG, name, ptsig),    ((stbpriority *) chain->DATA)->MASK))
    clock = ((stbpriority *) chain->DATA)->CLOCK;
   else
    continue;

   for (ptpath = path; ptpath; ptpath = ptpath->NEXT)
    if (ptpath->NODE->ROOT == clock)
     break;

   if (ptpath)
    {
     ptsav = NULL;
     ptpath = path;
     while (ptpath)
      {
       if (ptpath->NODE->ROOT != clock)
        {
         if (ptsav == NULL)
          {
           path = path->NEXT;
           ptpath->NEXT = NULL;
           ttv_freepathlist (ptpath);
           ptpath = path;
          }
         else
          {
           ptsav->NEXT = ptpath->NEXT;
           ptpath->NEXT = NULL;
           ttv_freepathlist (ptpath);
           ptpath = ptsav->NEXT;
          }
        }
       else
        {
         ptsav = ptpath;
         ptpath = ptpath->NEXT;
        }
      }
     break;
    }
  }

 return path;
}

/*****************************************************************************
*                           fonction stb_initbreak()                         *
*****************************************************************************/
ptype_list *
stb_initbreak (stbfig, cbo)
     stbfig_list *stbfig;
     chain_list **cbo;
{
 stbnode *node, *nodecmd;
 ttvsig_list *ptsig;
 ttvevent_list *event, *breakev;
 ttvevent_list *nodesetup;
 ttvevent_list *nodehold;
 ttvline_list *line;
 ptype_list *ptype, *pt;
 ptype_list *ptypesig;
 chain_list *chainsig = NULL;
 chain_list *chain;
 chain_list *chainb;
 chain_list *chainx;
 int i;
 char buf[1024];
 char level;
 char type;
 double valmax;

 *cbo = NULL;

 chainb = append (dupchainlst (stbfig->BREAK), dupchainlst (stbfig->MEMORY));
 chainb = append (chainb, dupchainlst (stbfig->COMMAND));

 for (chain = chainb; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   if ((ptsig->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT)
    continue;
   for (i = 0; i < 2; i++)
    {
     event = ptsig->NODE + i;
     if ((event->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT)
      continue;

     if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
      level = stbfig->FIG->INFO->LEVEL;
     else
      level = event->ROOT->ROOT->INFO->LEVEL;

     if (stbfig->GRAPH == STB_RED_GRAPH)
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CL_PJT, TTV_FILE_TTX);
       line = event->INPATH;
      }
     else
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CLS_FED, TTV_FILE_DTX);
       line = event->INLINE;
      }
     for (; line; line = line->NEXT)
      {
       if ((((line->TYPE & TTV_LINE_CONT) != TTV_LINE_CONT) ||
            (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
             (line->FIG != stbfig->FIG))) ||
           (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
            (line->FIG != stbfig->FIG) &&
            ((stbfig->STABILITYMODE & STB_STABILITY_LAST) ==
             STB_STABILITY_LAST))
           || ((line->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) ==
               TTV_SIG_BYPASSIN)
           || ((line->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
        continue;
       else if ((line->ROOT->ROOT->TYPE & TTV_SIG_Q)!=0 && (line->NODE->ROOT->TYPE & TTV_SIG_B)==0)
              continue;
       else if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A)
        {
         if (*cbo)
          {
           if ((*cbo)->DATA != ptsig)
            *cbo = addchain (*cbo, ptsig);
          }
         else
          {
           *cbo = addchain (*cbo, ptsig);
          }
        }
       else if (((line->TYPE & TTV_LINE_O) == TTV_LINE_O) ||
                ((line->TYPE & TTV_LINE_U) == TTV_LINE_U))
        {
         if((node = stb_getstbnode (line->NODE)) == NULL) continue ;
         if ((line->NODE->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
          type = STB_TYPE_LATCH;
         else if ((line->NODE->ROOT->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
          type = STB_TYPE_FLIPFLOP;
         else
          type = STB_TYPE_BREAK;
         if (node->CK == NULL)
          stb_addstbck (node, STB_NO_TIME, STB_NO_TIME,
                        STB_NO_TIME, STB_NO_TIME,
                        STB_NO_TIME, STB_NO_TIME,
                        STB_NO_TIME, STB_NO_INDEX, (char) 0, (char) 0, type, NULL);

         if ((line->TYPE & TTV_LINE_O) == TTV_LINE_O)
          {
           if ((ptype = getptype (line->NODE->USER, STB_NODE_HOLD)) == NULL)
            {
             line->NODE->USER =
              addptype (line->NODE->USER, STB_NODE_HOLD, line->ROOT);
             valmax = ttv_getdelaymax (line);
             if ((node->CK->HOLD == STB_NO_TIME)
                 || (node->CK->HOLD < (valmax + stbfig->HOLD)))
              node->CK->HOLD = valmax + stbfig->HOLD;
            }
           else
            {
             if (line->ROOT != ptype->DATA)
              {
               stb_error (ERR_MULTIPLE_CONSTRAINT,
                          ttv_getsigname (stbfig->FIG, buf, line->NODE->ROOT),
                          0, STB_WARNING);
               continue;
              }
             else
              {
               valmax = ttv_getdelaymax (line);
               if ((node->CK->HOLD == STB_NO_TIME)
                   || (node->CK->HOLD < (valmax + stbfig->HOLD)))
                node->CK->HOLD = valmax + stbfig->HOLD;
              }
            }
          }
         else
          {
           if ((ptype = getptype (line->NODE->USER, STB_NODE_SETUP)) == NULL)
            {
             line->NODE->USER =
              addptype (line->NODE->USER, STB_NODE_SETUP, line->ROOT);
             valmax = ttv_getdelaymax (line);
             if (ttv_has_strict_setup(line->NODE)) valmax=0;
             if ((node->CK->SETUP = STB_NO_TIME)
                 || (node->CK->SETUP < (valmax + stbfig->SETUP)))
              node->CK->SETUP = valmax + stbfig->SETUP;
            }
           else
            {
             if (line->ROOT != ptype->DATA)
              {
               stb_error (ERR_MULTIPLE_CONSTRAINT,
                          ttv_getsigname (stbfig->FIG, buf, line->NODE->ROOT),
                          0, STB_WARNING);
               continue;
              }
             else
              {
               valmax = ttv_getdelaymax (line);
               if (ttv_has_strict_setup(line->NODE)) valmax=0;
               if ((node->CK->SETUP == STB_NO_TIME)
                   || (node->CK->SETUP < (valmax + stbfig->SETUP)))
                node->CK->SETUP = valmax + stbfig->SETUP;
              }
            }
          }

         if ((ptype = getptype (line->NODE->ROOT->USER, TTV_SIG_CMD)) == NULL)
          {
           line->NODE->ROOT->USER = addptype (line->NODE->ROOT->USER,
                                              TTV_SIG_CMD, addchain (NULL,
                                                                     line->
                                                                     ROOT));
           chainsig = addchain (chainsig, line->NODE->ROOT);
          }
         else
          {
           for (chainx = (chain_list *) ptype->DATA; chainx;
                chainx = chainx->NEXT)
            if (chainx->DATA == line->ROOT)
             break;
           if (chainx == NULL)
            ptype->DATA = addchain (ptype->DATA, line->ROOT);
          }
        }
      }
    }
  }

 freechain (chainb);

 ptypesig = NULL;
 for (chain = chainsig; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   for (i = 0; i < 2; i++)
    {
     event = ptsig->NODE + i;

     node = stb_getstbnode (event);

     nodehold = NULL;
     nodesetup = NULL;

     if ((ptype = getptype (event->USER, STB_NODE_HOLD)))
      {
       nodehold = (ttvevent_list *) ptype->DATA;
       event->USER = delptype (event->USER, STB_NODE_HOLD);
      }

     if ((ptype = getptype (event->USER, STB_NODE_SETUP)))
      {
       nodesetup = (ttvevent_list *) ptype->DATA;
       event->USER = delptype (event->USER, STB_NODE_SETUP);
      }

     if ((nodesetup == NULL) && (nodehold == NULL))
      continue;

     if (nodehold && nodesetup)
      if (nodehold->ROOT != nodesetup->ROOT)
       {
        stb_error (ERR_MULTIPLE_CONSTRAINT,
                   ttv_getsigname (stbfig->FIG, buf, nodesetup->ROOT), 0,
                   STB_WARNING);
        continue;
       }

     if (nodesetup == NULL)
      {
       stb_error (ERR_ONLY_HOLD, ttv_getsigname (stbfig->FIG, buf, ptsig), 0,
                  STB_WARNING);
       if ((nodehold->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         node->CK->ACTIVE = STB_SLOPE_UP;
         node->CK->VERIF = STB_VERIF_EDGE | STB_UP;
        }
       else
        {
         node->CK->ACTIVE = STB_SLOPE_DN;
         node->CK->VERIF = STB_VERIF_EDGE | STB_DN;
        }
      }

     if (nodehold == NULL)
      {
       stb_error (ERR_ONLY_SETUP, ttv_getsigname (stbfig->FIG, buf, ptsig), 0,
                  STB_WARNING);
       if ((nodesetup->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         node->CK->ACTIVE = STB_SLOPE_UP;
         node->CK->VERIF = STB_VERIF_EDGE | STB_UP;
        }
       else
        {
         node->CK->ACTIVE = STB_SLOPE_DN;
         node->CK->VERIF = STB_VERIF_EDGE | STB_DN;
        }
      }

     breakev=nodesetup;
     if (nodehold == nodesetup)
      {
       if ((nodesetup->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         node->CK->ACTIVE = STB_SLOPE_UP;
         node->CK->VERIF = STB_VERIF_EDGE | STB_UP;
        }
       else
        {
         node->CK->ACTIVE = STB_SLOPE_DN;
         node->CK->VERIF = STB_VERIF_EDGE | STB_DN;
        }
      }
     else
      {
       if (nodesetup && nodehold)
        {
         if ((nodesetup->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          {
           node->CK->ACTIVE = STB_STATE_UP;
           node->CK->VERIF = STB_VERIF_STATE | STB_UP;
          }
         else
          {
           node->CK->ACTIVE = STB_STATE_DN;
           node->CK->VERIF = STB_VERIF_STATE | STB_DN;
          }
        }
       if (!nodesetup && nodehold)
        {
         breakev=nodehold;
         if ((nodehold->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          {
           node->CK->ACTIVE = STB_SLOPE_UP;
           node->CK->VERIF = STB_VERIF_EDGE | STB_UP;
          }
         else
          {
           node->CK->ACTIVE = STB_SLOPE_DN;
           node->CK->VERIF = STB_VERIF_EDGE | STB_DN;
          }
        }
       if (nodesetup && !nodehold)
        {
         if ((nodesetup->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          {
           node->CK->ACTIVE = STB_SLOPE_UP;
           node->CK->VERIF = STB_VERIF_EDGE | STB_UP;
          }
         else
          {
           node->CK->ACTIVE = STB_SLOPE_DN;
           node->CK->VERIF = STB_VERIF_EDGE | STB_DN;
          }
        }
      }
     nodecmd=stb_getstbnode(breakev);
     if (nodecmd->CK!=NULL) node->CK->ORIGINAL_CLOCK=nodecmd->CK->ORIGINAL_CLOCK;
     if ((pt=getptype(event->USER, STB_BREAK_TEST_EVENT))==NULL)
       event->USER=addptype(event->USER, STB_BREAK_TEST_EVENT, breakev);
     else
       pt->DATA=breakev;
    }
   ptype = getptype (ptsig->USER, TTV_SIG_CMD);
   ptypesig = addptype (ptypesig, (long) ptsig, ptype->DATA);
   ptsig->USER = delptype (ptsig->USER, TTV_SIG_CMD);
  }
 freechain (chainsig);
 return (ptypesig);
}



void stb_initsetuphold (stbfig_list *stbfig, ttvsig_list *ptsig)
{
 stbnode *node;
 ttvevent_list *event, *cmd;
 ttvline_list *line;
 int i;
 char level;
 double valmax;

 if ((ptsig->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT)
   return;

 for (i = 0; i < 2; i++)
   {
     event = ptsig->NODE + i;
     if ((event->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT)
       continue;
     
     if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
       level = stbfig->FIG->INFO->LEVEL;
     else
       level = event->ROOT->ROOT->INFO->LEVEL;
     
     if (stbfig->GRAPH == STB_RED_GRAPH)
       {
         ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                        stbfig->FIG->INFO->LEVEL, TTV_STS_CL_PJT, TTV_FILE_TTX);
         line = event->INPATH;
       }
     else
       {
         ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                        stbfig->FIG->INFO->LEVEL, TTV_STS_CLS_FED, TTV_FILE_DTX);
         line = event->INLINE;
       }
     for (; line; line = line->NEXT)
      {
            if ((((line->TYPE & TTV_LINE_CONT) != TTV_LINE_CONT) ||
                 (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
                  (line->FIG != stbfig->FIG))) ||
                (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
                 (line->FIG != stbfig->FIG) &&
                 ((stbfig->STABILITYMODE & STB_STABILITY_LAST) ==
                  STB_STABILITY_LAST))
                || ((line->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) ==
                    TTV_SIG_BYPASSIN)
                || ((line->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
              continue;
            else if (((line->TYPE & TTV_LINE_O) == TTV_LINE_O) ||
                     ((line->TYPE & TTV_LINE_U) == TTV_LINE_U))
              {
                if((node = stb_getstbnode (line->NODE)) == NULL) continue ;

                if (((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF) || 
                    (STB_OPEN_LATCH_PHASE == 'Y'))
                  cmd=(event->TYPE & TTV_NODE_UP)==TTV_NODE_UP?event->ROOT->NODE:event->ROOT->NODE+1;
                else
                  cmd=event;

                stb_getstbck(node, cmd); // a voir pour le cmd

                if (node->CK != NULL && (node->CK->CMD==NULL || node->CK->CMD==cmd))
                  {
                    valmax = ttv_getdelaymax (line);
                    if ((line->TYPE & TTV_LINE_O) == TTV_LINE_O)
                      {
                        if ((node->CK->HOLD == STB_NO_TIME)
                            || (node->CK->HOLD < (valmax + stbfig->HOLD)))
                          node->CK->HOLD = valmax + stbfig->HOLD;
                      }
                    else
                      {
                        if (ttv_has_strict_setup(line->NODE)) valmax=0;
                        if ((node->CK->SETUP == STB_NO_TIME)
                            || (node->CK->SETUP < (valmax + stbfig->SETUP)))
                          node->CK->SETUP = valmax + stbfig->SETUP;
                      }
                  }
                stb_getstbck(node, NULL);
              }
      }
   }
}

void stb_initunsetsetuphold (stbfig_list *stbfig)
{
 stbnode *node;
 ttvevent_list *event;
 ttvsig_list *ptsig;
 chain_list *chain;
 stbck *sck;
 int i;

 for (chain = stbfig->MEMORY; chain; chain = chain->NEXT)
  {
    ptsig = (ttvsig_list *) chain->DATA;
    
    for (i=0; i<2; i++)
      {
        event = ptsig->NODE+i;
        node = stb_getstbnode (event);
        for (sck=node->CK; sck!=NULL; sck=sck->NEXT)
          {
            if (sck->SETUP==STB_NO_TIME) sck->SETUP=stbfig->SETUP;
            if (sck->HOLD==STB_NO_TIME) sck->HOLD=stbfig->HOLD;
          }
      }
    }
}

int stb_cmd_can_generate_event (stbfig_list *stbfig, ttvevent_list *event, ttvevent_list *cmd)
{
 ttvevent_list *cmdl;
 ttvline_list *line;
 char level;

 if ((event->ROOT->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT)
   return 0;

 if ((event->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT)
   return 0;
     
 if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
   level = stbfig->FIG->INFO->LEVEL;
 else
   level = event->ROOT->ROOT->INFO->LEVEL;
 
 if (stbfig->GRAPH == STB_RED_GRAPH)
   {
     ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                    stbfig->FIG->INFO->LEVEL, TTV_STS_CL_PJT, TTV_FILE_TTX);
     line = event->INPATH;
   }
 else
   {
     ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                    stbfig->FIG->INFO->LEVEL, TTV_STS_CLS_FED, TTV_FILE_DTX);
     line = event->INLINE;
   }

 for (; line; line = line->NEXT)
   {
     if(((line->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) && (line->FIG != stbfig->FIG))  continue ;
     
     if ((line->TYPE & TTV_LINE_A)!=0 && line->NODE==cmd) return 1;
     if ((((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
          (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) && (line->FIG != stbfig->FIG))) ||
         (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) && (line->FIG != stbfig->FIG) &&
          ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST))
         || ((line->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN)
         || ((line->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
       continue;

     cmdl = ttv_getlinecmd(stbfig->FIG,line,TTV_LINE_CMDMAX) ;
     if (cmd==cmdl) return 1;
     cmdl = ttv_getlinecmd(stbfig->FIG,line,TTV_LINE_CMDMIN) ;
     if (cmd==cmdl) return 1;
   }

 return 0;
}


static void stb_getslopeclockmax(s,f,ckslope,delay)
long *s ;
char *f ;
long ckslope ;
long delay ;
{
 if(*f == 'N')
   *s = ckslope + delay ;
 else
  {
   if (*s < (ckslope + delay))
      *s = ckslope + delay;
   }
  *f = 'Y';
}

static void stb_getslopeclockmin(s,f,ckslope,delay)
long *s ;
char *f ;
long ckslope ;
long delay ;
{
 if(*f == 'N')
   *s = ckslope + delay ;
 else
  {
   if (*s > (ckslope + delay))
      *s = ckslope + delay;
   }
  *f = 'Y';
}

/*****************************************************************************
*                           fonction stb_initcmd()                           *
*****************************************************************************/

void displaystarts(ttvfig_list *tvf, ttvsig_list *ptsig, long find)
{
  ttvpath_list *path, *pth;
  char buf[1024];
  ht *h;
  
  avt_log(LOGERROR, 1, "  start points found:"); 
  
  ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
  path =
  ttv_getpathnocross_v2 (tvf, NULL, ptsig, NULL, TTV_DELAY_MAX,
               TTV_DELAY_MIN, TTV_FIND_MIN | TTV_FIND_CMD | find,0);
  ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);

  h=addht(100);
  for (pth=path; pth!=NULL; pth=pth->NEXT)
  {
    if (gethtitem(h, pth->NODE->ROOT)!=EMPTYHT) continue;
    addhtitem(h, pth->NODE->ROOT, 0);
    ttv_getsigname (tvf, buf, pth->NODE->ROOT);
    avt_log(LOGERROR, 1, " %s", buf);
    if ((pth->NODE->ROOT->TYPE & TTV_SIG_L)!=0) avt_log(LOGERROR, 1, "(L)");
    else if ((pth->NODE->ROOT->TYPE & TTV_SIG_R)!=0) avt_log(LOGERROR, 1, "(P)");
    else if ((pth->NODE->ROOT->TYPE & TTV_SIG_C)!=0) avt_log(LOGERROR, 1, "(C)");
    else if ((pth->NODE->ROOT->TYPE & TTV_SIG_B)!=0) avt_log(LOGERROR, 1, "(B)");
    else if ((pth->NODE->ROOT->TYPE & TTV_SIG_Q)!=0) avt_log(LOGERROR, 1, "(Q)");
    else avt_log(LOGERROR, 1, "(?)");
  }
  delht(h);
  ttv_freepathlist (path);
  avt_log(LOGERROR, 1, "\n");
}

static char *stberr_getsignaltype(ttvsig_list *tvs)
{
  if ((tvs->TYPE & TTV_SIG_L)!=0) return "latch";
  if ((tvs->TYPE & TTV_SIG_R)!=0) return "precharge";
  if ((tvs->TYPE & TTV_SIG_Q)!=0) return "command";
  if ((tvs->TYPE & TTV_SIG_C)!=0) return "connector";
  if ((tvs->TYPE & TTV_SIG_B)!=0) return "break-point";
  return "signal";
}
void
stb_initcmd (stbfig, cmd, active, verif, type, typesig, ctk)
     stbfig_list *stbfig;
     ttvevent_list *cmd;
     char active;
     char verif;
     char type;
     char typesig;
     int  ctk;
{
 ttvsig_list *ptsig;
 stbnode *node;
 stbnode *cknode;
 ttvsig_list *clock;
 ttvevent_list *ckevent, *refclock_cmd, *refclock_event;
 ttvevent_list *event;
 ttvpath_list *path;
 ttvpath_list *scanpath;
 ptype_list *ptype;
 char buf[1024];
 char message[1024];
 long find;
 long sumax;
 long sdmax;
 long sumin;
 long sdmin;
 long suhzmax;
 long sdhzmax;
 long suhzmin;
 long sdhzmin;
 char fumax = 'N', downishz=0, upishz=0;
 char fdmax = 'N';
 char fumin = 'N';
 char fdmin = 'N';
 char fuhzmax = 'N';
 char fdhzmax = 'N';
 char fuhzmin = 'N';
 char fdhzmin = 'N';
 char flagerr = 'N', moreckflags;
 int clockset=0;
 chain_list *clockevents[2];
 int dir, nbev;

 node = stb_getstbnode (cmd);

 if(ctk == 1)
   verif = 0 ;

 if (node->CK)
 if((node->CK->SUPMAX != STB_NO_TIME) ||
       (node->CK->SDNMAX != STB_NO_TIME) ||
       (node->CK->SUPMIN != STB_NO_TIME) ||
       (node->CK->SDNMIN != STB_NO_TIME))
  {
   if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    event = cmd->ROOT->NODE;
   else
    event = cmd->ROOT->NODE + 1;

   if ((type == STB_TYPE_EVAL) || (type == STB_TYPE_PRECHARGE))
    {
     node->CK->ACTIVE = active;
     node->CK->TYPE = type;
    }
   else
    {
     if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      {
       node->CK->ACTIVE |= active;
       stb_getstbnode (event)->CK->ACTIVE |= active;
      }
    }

   if ((typesig == STB_TYPE_FLIPFLOP) || (typesig == STB_TYPE_BREAK) ||
       (STB_OPEN_LATCH_PHASE == 'Y'))
    event = cmd;

   if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
    ckevent = (ttvevent_list *) ptype->DATA;
   else
    ckevent = event;

   if ((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    {
     stb_getstbnode (ckevent->ROOT->NODE)->CK->ACTIVE |= STB_SLOPE_UP;
     stb_getstbnode (ckevent->ROOT->NODE + 1)->CK->ACTIVE |= STB_SLOPE_UP;
    }
   else
    {
     stb_getstbnode (ckevent->ROOT->NODE)->CK->ACTIVE |= STB_SLOPE_DN;
     stb_getstbnode (ckevent->ROOT->NODE + 1)->CK->ACTIVE |= STB_SLOPE_DN;
    }
   return;
  }

 if (stbfig->GRAPH == STB_RED_GRAPH)
  find = TTV_FIND_PATH | TTV_FIND_SIG;
 else
  find = TTV_FIND_LINE | TTV_FIND_SIG;

 ptsig = cmd->ROOT;

 clock = NULL;

 if (V_INT_TAB[__STB_COREL_SKEW_ANA_DEPTH].VALUE>0)
    find|=TTV_FIND_PATHDET;

 ttv_activate_simple_critic(1);

 ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
 path =
  ttv_getpathnocross_v2 (stbfig->FIG, NULL, ptsig, stbfig->CLOCK, TTV_DELAY_MAX,
               TTV_DELAY_MIN, TTV_FIND_MAX | TTV_FIND_CMD | find,0);
 ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);

 ttv_activate_simple_critic(0);

 if (path == NULL)
  return;

 path = stb_filterbadpath (stbfig, cmd->ROOT, path);

 if ((cmd->ROOT->TYPE & TTV_SIG_R)!=0)
 {
   for (scanpath = path; scanpath; scanpath = scanpath->NEXT)
   {
     ckevent = scanpath->NODE;
     if ((scanpath->ROOT->TYPE & TTV_NODE_UP)!=0)
     {
       clock = ckevent->ROOT;
       clockset=1;
       break;
     }
   }
 }

 clockevents[0]=clockevents[1]=NULL;
 for (scanpath = path; scanpath; scanpath = scanpath->NEXT)
  {
   ckevent = scanpath->NODE;
   if (scanpath->ROOT->TYPE & TTV_NODE_UP) dir=1; else dir=0;

   if (getptype (ckevent->ROOT->USER, STB_IDEAL_CLOCK))
       scanpath->DELAY = 0;
   
   if (getchain(clockevents[dir], ckevent)==NULL)
      clockevents[dir]=addchain(clockevents[dir], ckevent);

   if (clock == NULL)
    clock = ckevent->ROOT;
   else if (clock != ckevent->ROOT && (!clockset || (scanpath->ROOT->TYPE & TTV_NODE_UP)!=0))
    {
     stb_error (ERR_MULTIPLE_CLOCKS,
                ttv_getsigname (stbfig->FIG, buf, cmd->ROOT), 0, STB_WARNING);
     continue;
    }
   cknode = stb_getstbnode (ckevent);
   event = scanpath->ROOT;

   if( !cknode->CK ) 
     continue ;

   if ((((event->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR)
        && ((scanpath->TYPE & TTV_FIND_S) == TTV_FIND_S))
       || (((event->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS)
           && ((scanpath->TYPE & TTV_FIND_R) == TTV_FIND_R)))
    {
     if ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      event = event->ROOT->NODE;
     else
      event = event->ROOT->NODE + 1;
    }

   if ((scanpath->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)
     {
      if ((ptype = getptype (event->USER, STB_NODE_CLOCK)) == NULL)
       event->USER = addptype (event->USER, STB_NODE_CLOCK, ckevent);
      else
       {
        if (ptype->DATA != ckevent)
         {
          if (flagerr == 'N')
           stb_error (ERR_MULTIPLE_PHASES,
                      ttv_getsigname (stbfig->FIG, buf, cmd->ROOT), 0,
                      STB_WARNING);
          flagerr = 'Y';
          continue;
         }
       }
      
   /* si flip flop ou break la phase de fermeture reference c'est la commande 
      sinon c'est l'evenement opposé à la commance */
      if (((event == cmd)
           && ((typesig == STB_TYPE_FLIPFLOP) || (typesig == STB_TYPE_BREAK)))
          || ((((event != cmd) && (STB_OPEN_LATCH_PHASE == 'N')) ||
               ((event == cmd) && (STB_OPEN_LATCH_PHASE == 'Y')))
              && ((typesig == STB_TYPE_LATCH) || (typesig == STB_TYPE_PRECHARGE)
                  || (typesig == STB_TYPE_EVAL))))
       {
        if ((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         {
          stb_getstbnode (ckevent->ROOT->NODE)->CK->ACTIVE |= STB_SLOPE_UP;
          stb_getstbnode (ckevent->ROOT->NODE + 1)->CK->ACTIVE |= STB_SLOPE_UP;
         }
        else
         {
          stb_getstbnode (ckevent->ROOT->NODE)->CK->ACTIVE |= STB_SLOPE_DN;
          stb_getstbnode (ckevent->ROOT->NODE + 1)->CK->ACTIVE |= STB_SLOPE_DN;
         }
       }
      if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmax(&sumax,&fumax,cknode->CK->SUPMAX,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmax(&sumax,&fumax,cknode->CK->SDNMAX,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmax(&sdmax,&fdmax,cknode->CK->SUPMAX,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmax(&sdmax,&fdmax,cknode->CK->SDNMAX,scanpath->DELAY) ;
     }
   else
     {
      if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmax(&suhzmax,&fuhzmax,cknode->CK->SUPMAX,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmax(&suhzmax,&fuhzmax,cknode->CK->SDNMAX,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmax(&sdhzmax,&fdhzmax,cknode->CK->SUPMAX,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmax(&sdhzmax,&fdhzmax,cknode->CK->SDNMAX,scanpath->DELAY) ;
     }
      
   if (V_INT_TAB[__STB_COREL_SKEW_ANA_DEPTH].VALUE>0)
     stb_mark_path_diverging_node(stbfig, scanpath);
  }

 ttv_freepathlist (path);

 for (dir=0; dir<2; dir++)
 {
   nbev=countchain(clockevents[dir]);
   if (nbev<=1) cmd->ROOT->NODE[dir].USER=addptype(cmd->ROOT->NODE[dir].USER, STB_ONE_OR_NO_CLOCK_EVENT, clockevents[dir]);
 }

 ttv_activate_simple_critic(1);

 ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
 path =
  ttv_getpathnocross_v2 (stbfig->FIG, NULL, ptsig, stbfig->CLOCK, TTV_DELAY_MAX,
               TTV_DELAY_MIN, TTV_FIND_MIN | TTV_FIND_CMD | find,0);
 ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);

 ttv_activate_simple_critic(0);

 if (path == NULL)
  {
   sumin = sumax;
   sdmin = sdmax;
   fumin = fumax;
   fdmin = fdmax;
  }

 path = stb_filterbadpath (stbfig, cmd->ROOT, path);

 for (scanpath = path; scanpath; scanpath = scanpath->NEXT)
  {
   ckevent = scanpath->NODE;

   if (getptype (ckevent->ROOT->USER, STB_IDEAL_CLOCK))
       scanpath->DELAY = 0;

   if (clock == NULL)
    clock = ckevent->ROOT;
   else if (clock != ckevent->ROOT && (!clockset || (scanpath->ROOT->TYPE & TTV_NODE_UP)!=0))
    {
     stb_error (ERR_MULTIPLE_CLOCKS,
                ttv_getsigname (stbfig->FIG, buf, cmd->ROOT), 0, STB_WARNING);
     continue;
    }

   cknode = stb_getstbnode (ckevent);

   if( !cknode->CK ) 
     continue ;

   cknode->CK->FLAGS|=STBCK_ONCLOCK_PATH;
   event = scanpath->ROOT;

   if ((((event->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR)
        && ((scanpath->TYPE & TTV_FIND_S) == TTV_FIND_S))
       || (((event->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS)
           && ((scanpath->TYPE & TTV_FIND_R) == TTV_FIND_R)))
    {
     if ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      event = event->ROOT->NODE;
     else
      event = event->ROOT->NODE + 1;
    }

   if ((scanpath->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)
     {
      if ((ptype = getptype (event->USER, STB_NODE_CLOCK)) == NULL)
       event->USER = addptype (event->USER, STB_NODE_CLOCK, ckevent);
      else
       {
        if (ptype->DATA != ckevent)
         {
          if (flagerr == 'N')
           stb_error (ERR_MULTIPLE_PHASES,
                      ttv_getsigname (stbfig->FIG, buf, cmd->ROOT), 0,
                      STB_WARNING);
          flagerr = 'Y';
          continue;
         }
       }
      
      if (((event == cmd)
           && ((typesig == STB_TYPE_FLIPFLOP) || (typesig == STB_TYPE_BREAK)))
          || ((((event != cmd) && (STB_OPEN_LATCH_PHASE == 'N')) ||
               ((event == cmd) && (STB_OPEN_LATCH_PHASE == 'Y')))
              && ((typesig == STB_TYPE_LATCH) || (typesig == STB_TYPE_PRECHARGE)
                  || (typesig == STB_TYPE_EVAL))))
       {
        if ((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         {
          stb_getstbnode (ckevent->ROOT->NODE)->CK->ACTIVE |= STB_SLOPE_UP;
          stb_getstbnode (ckevent->ROOT->NODE + 1)->CK->ACTIVE |= STB_SLOPE_UP;
         }
        else
         {
          stb_getstbnode (ckevent->ROOT->NODE)->CK->ACTIVE |= STB_SLOPE_DN;
          stb_getstbnode (ckevent->ROOT->NODE + 1)->CK->ACTIVE |= STB_SLOPE_DN;
         }
       }
      if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmin(&sumin,&fumin,cknode->CK->SUPMIN,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmin(&sumin,&fumin,cknode->CK->SDNMIN,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmin(&sdmin,&fdmin,cknode->CK->SUPMIN,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmin(&sdmin,&fdmin,cknode->CK->SDNMIN,scanpath->DELAY) ;
     }
   else
     {
      if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmin(&suhzmin,&fuhzmin,cknode->CK->SUPMIN,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
        stb_getslopeclockmin(&suhzmin,&fuhzmin,cknode->CK->SDNMIN,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmin(&sdhzmin,&fdhzmin,cknode->CK->SUPMIN,scanpath->DELAY) ;
      else if (((ckevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          && ((event->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
        stb_getslopeclockmin(&sdhzmin,&fdhzmin,cknode->CK->SDNMIN,scanpath->DELAY) ;
     }
      
   if (V_INT_TAB[__STB_COREL_SKEW_ANA_DEPTH].VALUE>0)
     stb_mark_path_diverging_node(stbfig, scanpath);
  }

 ttv_freepathlist (path);

 if ((cmd->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
  event = ptsig->NODE + 1;
 else
  event = ptsig->NODE;

 moreckflags=0;

 if (fumax == 'N' || fdmax == 'N' || fumin == 'N' || fdmin == 'N')
  {
   if (fumax == 'N' && fdmax == 'Y' && fumin == 'N' && fdmin == 'Y')
    {

     if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
      {
       ckevent = (ttvevent_list *) ptype->DATA;
       if (getptype (cmd->USER, STB_NODE_CLOCK) == NULL)
        cmd->USER = addptype (cmd->USER, STB_NODE_CLOCK, ttv_opposite_event(ckevent));
      }
     else if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      {
       ckevent = (ttvevent_list *) ptype->DATA;
       if (getptype (event->USER, STB_NODE_CLOCK) == NULL)
        event->USER = addptype (event->USER, STB_NODE_CLOCK, ttv_opposite_event(ckevent));
      }
     else
     {
       avt_errmsg(STB_ERRMSG, "006", AVT_WARNING, "rising", stberr_getsignaltype(cmd->ROOT), ttv_getsigname (stbfig->FIG, buf, cmd->ROOT));
       return;
     }

     if ((type==STB_TYPE_PRECHARGE || type==STB_TYPE_EVAL) && fdhzmax!='N' && fdhzmin!='N')
     {
       sumax=sdhzmax;
       sumin=sdhzmin;
       upishz=1;
     }
     else
     {
       cknode = stb_getstbnode (ckevent);
       if ((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         sumax = sdmax - cknode->CK->SUPMAX + cknode->CK->SDNMAX;
         sumin = sdmin - cknode->CK->SUPMIN + cknode->CK->SDNMIN;
        }
       else
        {
         sumax = sdmax - cknode->CK->SDNMAX + cknode->CK->SUPMAX;
         sumin = sdmin - cknode->CK->SDNMIN + cknode->CK->SUPMIN;
        }
       moreckflags=STBCK_FAKE_UP;
       avt_errmsg(STB_ERRMSG, "006", AVT_WARNING, "rising", stberr_getsignaltype(cmd->ROOT), ttv_getsigname (stbfig->FIG, buf, cmd->ROOT));
     }
    }
   else if (fumax == 'Y' && fdmax == 'N' && fumin == 'Y' && fdmin == 'N')
    {
     if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
      {
       ckevent = (ttvevent_list *) ptype->DATA;
       if (getptype (cmd->USER, STB_NODE_CLOCK) == NULL)
        cmd->USER = addptype (cmd->USER, STB_NODE_CLOCK, ttv_opposite_event(ckevent));
      }
     else if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      {
       ckevent = (ttvevent_list *) ptype->DATA;
       if (getptype (event->USER, STB_NODE_CLOCK) == NULL)
        event->USER = addptype (event->USER, STB_NODE_CLOCK, ttv_opposite_event(ckevent));
      }
     else
     {
       avt_errmsg(STB_ERRMSG, "006", AVT_WARNING, "falling", stberr_getsignaltype(cmd->ROOT), ttv_getsigname (stbfig->FIG, buf, cmd->ROOT));
       return;
     }

     if ((type==STB_TYPE_PRECHARGE || type==STB_TYPE_EVAL) && fuhzmax!='N' && fuhzmin!='N')
     {
       sdmax=suhzmax;
       sdmin=suhzmin;
       downishz=1;
     }
     else
     {
       cknode = stb_getstbnode (ckevent);
       if ((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         sdmax = sumax - cknode->CK->SUPMAX + cknode->CK->SDNMAX;
         sdmin = sumin - cknode->CK->SUPMIN + cknode->CK->SDNMIN;
        }
       else
        {
         sdmax = sumax - cknode->CK->SDNMAX + cknode->CK->SUPMAX;
         sdmin = sumin - cknode->CK->SDNMIN + cknode->CK->SUPMIN;
        }
       moreckflags=STBCK_FAKE_DOWN;
       avt_errmsg(STB_ERRMSG, "006", AVT_WARNING, "falling", stberr_getsignaltype(cmd->ROOT), ttv_getsigname (stbfig->FIG, buf, cmd->ROOT));
     }
    }
   else
    {
     if (!((cmd->ROOT->TYPE & TTV_SIG_QR) == TTV_SIG_QR))
      { 
	   ttv_getsigname (stbfig->FIG, buf, cmd->ROOT);
        sprintf (message, "'%s' rising & falling", buf);
	   stb_error (ERR_NO_CLOCK, message, 0, STB_WARNING);
       if (V_BOOL_TAB[__STB_SHOWME].VALUE)
         displaystarts(stbfig->FIG, cmd->ROOT, find);
      }
     return;
    }
  }

 if ((ptype = getptype (event->USER, STB_NODE_CLOCK))) refclock_event=(ttvevent_list *)ptype->DATA;
 else refclock_event=NULL;
 if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK))) refclock_cmd=(ttvevent_list *)ptype->DATA;
 else refclock_cmd=NULL;

 if((type == STB_TYPE_PRECHARGE) || (type == STB_TYPE_EVAL))
  {
   char flags, uhzmax=0, dhzmax=0, uhzmin=0, dhzmin=0;
   stbnode *n;
   if((fuhzmax == 'N') || ((fdmax == 'Y') && (sdmax > suhzmax)))
      suhzmax = sdmax, uhzmax=1 ;
   if((fdhzmax == 'N') || ((fumax == 'Y') && (sumax > sdhzmax)))
      sdhzmax = sumax, dhzmax=1 ;

   if((fuhzmin == 'N') || ((fdmin == 'Y') && (sdmin < suhzmin)))
      suhzmin = sdmin, uhzmin=1 ;
   if((fdhzmin == 'N') || ((fumin == 'Y') && (sumin < sdhzmin)))
      sdhzmin = sumin, dhzmin=1 ;

   if((((cmd->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) && 
       (type == STB_TYPE_EVAL)) || 
      (((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && 
       (type == STB_TYPE_PRECHARGE)))
       {
        stb_addstbck (n=stb_getstbnode (cmd), sdhzmin, sdhzmax, sdmin, sdmax,
                 cknode->CK->PERIOD, stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,
                 active, verif, type, refclock_cmd);
        if (dhzmin) n->CK->FLAGS|=STBCK_MIN_EDGE_NOT_HZ;
        if (dhzmax) n->CK->FLAGS|=STBCK_MAX_EDGE_NOT_HZ;
        if (downishz) n->CK->FLAGS|=STBCK_MIN_EDGE_HZ|STBCK_MAX_EDGE_HZ;
        n->CK->FLAGS|=moreckflags;

        stb_addstbck (n=stb_getstbnode (event), sumin, sumax, suhzmin, suhzmax,
                 cknode->CK->PERIOD, stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,
                 active, verif, type, refclock_event);
        if (uhzmin) n->CK->FLAGS|=STBCK_MIN_EDGE_NOT_HZ;
        if (uhzmax) n->CK->FLAGS|=STBCK_MAX_EDGE_NOT_HZ;
        if (upishz) n->CK->FLAGS|=STBCK_MIN_EDGE_HZ|STBCK_MAX_EDGE_HZ;
        n->CK->FLAGS|=moreckflags;
       }
      else
       {
        stb_addstbck (n=stb_getstbnode (cmd), sumin, sumax, suhzmin, suhzmax,
                 cknode->CK->PERIOD, stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,
                 active, verif, type, refclock_cmd);
        if (uhzmin) n->CK->FLAGS|=STBCK_MIN_EDGE_NOT_HZ;
        if (uhzmax) n->CK->FLAGS|=STBCK_MAX_EDGE_NOT_HZ;
        if (upishz) n->CK->FLAGS|=STBCK_MIN_EDGE_HZ|STBCK_MAX_EDGE_HZ;
        n->CK->FLAGS|=moreckflags;

        stb_addstbck (n=stb_getstbnode (event), sdhzmin, sdhzmax, sdmin, sdmax,
                 cknode->CK->PERIOD, stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,
                 active, verif, type, refclock_event);
        if (dhzmin) n->CK->FLAGS|=STBCK_MIN_EDGE_NOT_HZ;
        if (dhzmax) n->CK->FLAGS|=STBCK_MAX_EDGE_NOT_HZ;
        if (downishz) n->CK->FLAGS|=STBCK_MIN_EDGE_HZ|STBCK_MAX_EDGE_HZ;
        n->CK->FLAGS|=moreckflags;
       }
  }
 else
  {
   stb_addstbck (stb_getstbnode (cmd), sumin, sumax, sdmin, sdmax,
                 cknode->CK->PERIOD, stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,
                 active, verif, type, refclock_cmd);

   stb_addstbck (stb_getstbnode (event), sumin, sumax, sdmin, sdmax,
                 cknode->CK->PERIOD, stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,
                 active, verif, type, refclock_event);
   stb_getstbnode (event)->CK->FLAGS|=STBCK_ONCLOCK_PATH|moreckflags;
   stb_getstbnode (cmd)->CK->FLAGS|=STBCK_ONCLOCK_PATH|moreckflags;
  }
}

/*****************************************************************************
*                           fonction stb_checkclock()                        *
*****************************************************************************/

void
stb_checkclock (stbfig)
     stbfig_list *stbfig;
{
 stbnode *node;
 ttvsig_list *ptsig;
 ttvevent_list *cmd;
 chain_list *chaincmd;
 chain_list *chain;
 chain_list *chcmd;
 char buf[1024];
 char message[1024];

 for (chain = stbfig->MEMORY; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   chaincmd = ttv_getlrcmd (stbfig->FIG, ptsig);
   for (chcmd = chaincmd; chcmd; chcmd = chcmd->NEXT)
    {
     cmd = (ttvevent_list *) chcmd->DATA;
     if(((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL) && (STB_OPEN_LATCH_PHASE == 'N'))
        cmd = ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? cmd->ROOT->NODE :
                                                           cmd->ROOT->NODE + 1 ;
     node = stb_getstbnode (cmd);
     if (node->CK)
      if (node->CK->CKINDEX == STB_NO_INDEX) 
       {
        if (!((cmd->ROOT->TYPE & TTV_SIG_QR) == TTV_SIG_QR))
	     {
          ttv_getsigname (stbfig->FIG, buf, cmd->ROOT);
          if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
           sprintf (message, "'%s' rising", buf);
          else
           sprintf (message, "'%s' falling", buf);
	      stb_error (ERR_NO_CLOCK, message, 0, STB_WARNING);
          if (V_BOOL_TAB[__STB_SHOWME].VALUE)
            {
             long find;
             if (stbfig->GRAPH == STB_RED_GRAPH)
              find = TTV_FIND_PATH | TTV_FIND_SIG;
             else
              find = TTV_FIND_LINE | TTV_FIND_SIG;

             displaystarts(stbfig->FIG, cmd->ROOT, find);
            }
         }
       }
    }
   freechain(chaincmd);
  }
}


/*****************************************************************************
*                           fonction stb_initclock()                         *
*****************************************************************************/

static void stb_initdirectives_ck(stbfig_list *stbfig, int ctk)
{
  ptype_list *dck, *pt;
  chain_list *cl;
  ttvevent_list *tve;
  stbnode *node;

  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
  {
    tve=(ttvevent_list *)cl->DATA;
    if ((tve->ROOT->TYPE & (TTV_SIG_L | TTV_SIG_Q | TTV_SIG_R | TTV_SIG_B)) ==
      0)
    {
      dck=ttv_get_directive_slopes(tve, -1, 0, -1);
      dck=(ptype_list *)append((chain_list *)ttv_get_directive_slopes(tve, -1, -1, 1), (chain_list *)dck);
      for (pt=dck; pt!=NULL; pt=pt->NEXT)
      {
        tve=(ttvevent_list *)pt->DATA;
        if ((node=stb_getstbnode(tve))->CK==NULL)
        {
          stb_initcmd (stbfig, tve, 0, STB_NO_VERIF, STB_TYPE_CLOCKPATH, STB_TYPE_BREAK,ctk);
        }
      }
      freeptype(dck);
    }
  }
}

void stb_mark_path_to_latch_data(stbfig_list *stbfig)
{
 long find=0;
 ttvpath_list *path, *scanpath;
 int i;
 chain_list *cl;
 if (stbfig->GRAPH == STB_RED_GRAPH)
  find = TTV_FIND_PATH | TTV_FIND_SIG;
 else
  find = TTV_FIND_LINE | TTV_FIND_SIG;

 find|=TTV_FIND_PATHDET|TTV_FIND_DUAL;

 ttv_activate_simple_critic(1);

 for (cl=stbfig->CLOCK; cl!=NULL; cl=cl->NEXT)
 {
   for (i=0; i<2; i++)
   {
    path =
     ttv_getpathnocross_v2 (stbfig->FIG, NULL, (ttvsig_list *)cl->DATA, NULL, TTV_DELAY_MAX,
                 TTV_DELAY_MIN, (i==0?TTV_FIND_MAX:TTV_FIND_MIN) | find,0);

    for (scanpath = path; scanpath; scanpath = scanpath->NEXT)
      if ((path->ROOT->ROOT->TYPE & TTV_SIG_L)!=0)
        stb_mark_path_diverging_node(stbfig, scanpath);

    ttv_freepathlist (path);
   }
 }
 ttv_activate_simple_critic(0);
}

void
stb_initclock (stbfig,ctk)
     stbfig_list *stbfig;
     int ctk ;
{
 stbnode *node;
 stbnode *cknode;
 ttvsig_list *ptsig;
 ttvevent_list *ckevent;
 ttvevent_list *cke;
 ttvevent_list *cmd;
 ttvevent_list *cmdlinemax;
 ttvevent_list *cmdlinemin;
 ttvevent_list *event;
 ttvline_list *line;
 ttvpath_list *path;
 ttvpath_list *scanpath;
 ptype_list *ptype;
 ptype_list *lrb;
 ptype_list *lrbx;
 chain_list *cbo;
 chain_list *chaincmd;
 chain_list *chain;
 chain_list *chcmd;
 long sumax;
 long sdmax;
 long sumin;
 long sdmin;
 long period;
 long s;
 long sl;
 long u;
 long d;
 long find;
 char buf[1024];
 char index;
 char active;
 char activex;
 char verif;
 char level;
 int i, warn;
 double valmax, valmin;

 if (stbfig->GRAPH == STB_RED_GRAPH)
  find = TTV_FIND_PATH;
 else
  find = TTV_FIND_LINE;

 stb_compute_clock_latency(stbfig, find);
 
 if (V_INT_TAB[__STB_COREL_SKEW_ANA_DEPTH].VALUE>0)
 {
   stb_clock_tree_mark_divergence(stbfig);
   stb_mark_path_to_latch_data(stbfig);
 }

 stb_initdirectives_ck(stbfig, ctk);
 
 for (chain = stbfig->MEMORY; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   chaincmd = ttv_getlrcmd (stbfig->FIG, ptsig);
   if (chaincmd == NULL)
    {
     if ((ptsig->TYPE & TTV_SIG_LR) == TTV_SIG_LR)
      {
       active = STB_STATE_UP;
       event = ptsig->NODE + 1;
       activex = STB_VERIF_EDGE | STB_DN;
      }
     else if ((ptsig->TYPE & TTV_SIG_LS) == TTV_SIG_LS)
      {
       active = STB_STATE_DN;
       event = ptsig->NODE;
       activex = STB_VERIF_EDGE | STB_UP;
      }
     else
      {
       continue;
      }
     stb_initcmd (stbfig, event, active, activex, STB_TYPE_LATCH,
                  STB_TYPE_LATCH,ctk);
    }
   else
    {
     if (stbfig->CLOCK == NULL)
      stb_error (ERR_BAD_CLOCKS, stbfig->FIG->INFO->FIGNAME, 0, STB_FATAL);

     for (chcmd = chaincmd; chcmd; chcmd = chcmd->NEXT)
      {
       cmd = (ttvevent_list *) chcmd->DATA;
       if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF && STB_OPEN_LATCH_PHASE == 'N')
          stb_initcmd (stbfig, cmd, STB_STATE_DN, STB_VERIF_STATE | STB_DN,
                       STB_TYPE_COMMAND, STB_TYPE_FLIPFLOP,ctk);
         else
          stb_initcmd (stbfig, cmd, STB_STATE_UP, STB_VERIF_STATE | STB_UP,
                       STB_TYPE_COMMAND, STB_TYPE_LATCH,ctk);
        }
       else
        {
         if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF && STB_OPEN_LATCH_PHASE == 'N')
          stb_initcmd (stbfig, cmd, STB_STATE_UP, STB_VERIF_STATE | STB_UP,
                       STB_TYPE_COMMAND, STB_TYPE_FLIPFLOP,ctk);
         else
          stb_initcmd (stbfig, cmd, STB_STATE_DN, STB_VERIF_STATE | STB_DN,
                       STB_TYPE_COMMAND, STB_TYPE_LATCH,ctk);
        }
      }
    }
   freechain (chaincmd);
  }

 for (chain = stbfig->PRECHARGE; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   for (i = 0; i < 2; i++)
    {
     event = ptsig->NODE + i;

     if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
      level = stbfig->FIG->INFO->LEVEL;
     else
      level = event->ROOT->ROOT->INFO->LEVEL;

     if (stbfig->GRAPH == STB_RED_GRAPH)
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CL_PJT, TTV_FILE_TTX);
       line = event->INPATH;
      }
     else
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CLS_FED, TTV_FILE_DTX);
       line = event->INLINE;
      }

     if( line == NULL )
       continue ;

     while (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
            (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
             (line->FIG != stbfig->FIG)) ||
            (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
             (line->FIG != stbfig->FIG) &&
             ((stbfig->STABILITYMODE & STB_STABILITY_LAST) ==
              STB_STABILITY_LAST)))
      line = line->NEXT;

     if (line == NULL)
      continue;

     if ((line->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
      activex = STB_TYPE_PRECHARGE;
     else
      activex = STB_TYPE_EVAL;

     if ((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      {
       stb_initcmd (stbfig, event, STB_STATE_UP, STB_VERIF_EDGE | STB_DN,
                    activex, activex,ctk);
      }
     else
      {
       stb_initcmd (stbfig, event, STB_STATE_DN, STB_VERIF_EDGE | STB_UP,
                    activex, activex,ctk);
      }
     if(stb_getstbnode(event)->CK == NULL)
          stb_addstbck(stb_getstbnode(event),STB_NO_TIME,STB_NO_TIME,STB_NO_TIME,STB_NO_TIME,STB_NO_TIME,
                   stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX,(char )0,STB_NO_VERIF,activex,NULL) ;
    }
  }

 lrb = stb_initbreak (stbfig, &cbo);

 for (lrbx = lrb; lrbx; lrbx = lrbx->NEXT)
  {
   ptsig = (ttvsig_list *) lrbx->TYPE;
   chaincmd = (chain_list *) lrbx->DATA;

   if (stbfig->CLOCK == NULL)
    stb_error (ERR_BAD_CLOCKS, stbfig->FIG->INFO->FIGNAME, 0, STB_FATAL);

   for (chcmd = chaincmd; chcmd; chcmd = chcmd->NEXT)
    {
     cmd = (ttvevent_list *) chcmd->DATA;
     if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      {
       stb_initcmd (stbfig, cmd, STB_STATE_DN, STB_VERIF_STATE | STB_DN,
                    STB_TYPE_COMMAND, STB_TYPE_BREAK,ctk);
      }
     else
      {
       stb_initcmd (stbfig, cmd, STB_STATE_UP, STB_VERIF_STATE | STB_UP,
                    STB_TYPE_COMMAND, STB_TYPE_BREAK,ctk);
      }
    }
  }

 for (chain = cbo; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   if ((ptsig->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT)
    continue;

   for (i = 0; i < 2; i++)
    {
     event = ptsig->NODE + i;
     if ((event->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT)
      continue;

     if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
      level = stbfig->FIG->INFO->LEVEL;
     else
      level = event->ROOT->ROOT->INFO->LEVEL;

     if (stbfig->GRAPH == STB_RED_GRAPH)
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CL_PJT, TTV_FILE_TTX);
       line = event->INPATH;
      }
     else
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CLS_FED, TTV_FILE_DTX);
       line = event->INLINE;
      }

     for (; line; line = line->NEXT)
      {
       if ((((line->TYPE & TTV_LINE_A) != TTV_LINE_A) ||
            (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
             (line->FIG != stbfig->FIG))) ||
           (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
            (line->FIG != stbfig->FIG) &&
            ((stbfig->STABILITYMODE & STB_STABILITY_LAST) ==
             STB_STABILITY_LAST))
           || ((line->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) ==
               TTV_SIG_BYPASSIN)
           || ((line->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
        continue;

       if((stb_getstbnode(line->ROOT) == NULL) ||
          (stb_getstbnode(line->NODE) == NULL))
          continue;

       cmd = line->NODE;
       if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
         stb_initcmd (stbfig, cmd, STB_STATE_DN, STB_VERIF_STATE | STB_DN,
                      STB_TYPE_COMMAND, STB_TYPE_BREAK,ctk);
        }
       else
        {
         stb_initcmd (stbfig, cmd, STB_STATE_UP, STB_VERIF_STATE | STB_UP,
                      STB_TYPE_COMMAND, STB_TYPE_BREAK,ctk);
        }
      }
    }
  }

 stbfig->PHASENUMBER = stb_sortphase (stbfig, stbfig->CLOCK);

 for (chain = stbfig->MEMORY; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
//   if (getptype (ptsig->USER,STB_IS_CLOCK)!=NULL) continue;
   chaincmd = ttv_getlrcmd (stbfig->FIG, ptsig);
   index = STB_NO_INDEX;
   active = 0;
   event = NULL;

   if (chaincmd == NULL)
    {
     if ((ptsig->TYPE & TTV_SIG_LR) == TTV_SIG_LR)
      event = ptsig->NODE;
     else if ((ptsig->TYPE & TTV_SIG_LS) == TTV_SIG_LS)
      event = ptsig->NODE + 1;

     if (event)
      {
       if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
        {
         ckevent = (ttvevent_list *) ptype->DATA;
         index = stb_getstbnode (ckevent)->CK->CKINDEX;
         stb_getstbnode (ptsig->NODE)->CK->CKINDEX = index;
         stb_getstbnode (ptsig->NODE + 1)->CK->CKINDEX = index;
         continue;
        }
      }
    }

   if (chaincmd==NULL)
     avt_errmsg(STB_ERRMSG, "047", AVT_WARNING, ttv_getsigname (stbfig->FIG, buf, ptsig));
   
   for (chcmd = chaincmd; chcmd; chcmd = chcmd->NEXT)
    {
     cmd = (ttvevent_list *) chcmd->DATA;
     node = stb_getstbnode (cmd);

     if((node->CK == NULL) || 
        ((node->CK->SUPMIN == STB_NO_TIME) && 
         (node->CK->SUPMAX == STB_NO_TIME) && 
         (node->CK->SDNMIN == STB_NO_TIME) && 
         (node->CK->SDNMAX == STB_NO_TIME)))
      continue;

     if (((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF) || 
          (STB_OPEN_LATCH_PHASE == 'Y'))
      event = cmd;
     else
      {
       if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        event = cmd->ROOT->NODE;
       else
        event = cmd->ROOT->NODE + 1;
      }

     if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
      cke = (ttvevent_list *) ptype->DATA;
     else
      cke = event;

     if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      ckevent = (ttvevent_list *) ptype->DATA;
     else
      ckevent = cmd;

     if ((ckevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      {
       if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
        {
         activex = STB_SLOPE_UP;
         verif = STB_UP;
        }
       else
        {
         activex = STB_STATE_UP;
         verif = STB_DN;
        }
      }
     else
      {
       if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
        {
         activex = STB_SLOPE_DN;
         verif = STB_DN;
        }
       else
        {
         activex = STB_STATE_DN;
         verif = STB_UP;
        }
      }

     if ((active == 0) || (STB_MULTIPLE_COMMAND == 'Y'))
      active = activex;

     if (node->CK)
      {
       if((index == STB_NO_INDEX) || (STB_MULTIPLE_COMMAND == 'Y'))
        {
         if ((cmd->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)) ==
             (ckevent->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)))
          {
           sumax = node->CK->SUPMAX;
           sdmax = node->CK->SDNMAX;
           sumin = node->CK->SUPMIN;
           sdmin = node->CK->SDNMIN;
          }
         else
          {
           sdmax = node->CK->SUPMAX;
           sumax = node->CK->SDNMAX;
           sdmin = node->CK->SUPMIN;
           sumin = node->CK->SDNMIN;
          }
         index = stb_getstbnode (cke)->CK->CKINDEX;

         if((index != STB_NO_INDEX) && (STB_MULTIPLE_COMMAND == 'Y'))
          {
           int ev0, ev1;
           period = stb_getstbnode (cke)->CK->PERIOD;
           if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
            {
#ifdef STRIP_CMD
              ev0=stb_cmd_can_generate_event(stbfig, ptsig->NODE, cmd);
              ev1=stb_cmd_can_generate_event(stbfig, ptsig->NODE+1, cmd);
              if (ev0 || (!ev0 && !ev1))
#endif
                stb_chainstbck(stb_getstbnode (ptsig->NODE), 
                               sumin, sumax, sdmin, sdmax,period,
                               STB_NO_TIME, STB_NO_TIME, index, active,
                               STB_VERIF_EDGE | verif, STB_TYPE_FLIPFLOP,cmd,node->CK->ORIGINAL_CLOCK);
                
#ifdef STRIP_CMD
              if (ev1 || (!ev0 && !ev1))
#endif
                stb_chainstbck(stb_getstbnode (ptsig->NODE + 1), 
                               sumin, sumax, sdmin,
                               sdmax, period, STB_NO_TIME, STB_NO_TIME, index,
                               active, STB_VERIF_EDGE | verif, STB_TYPE_FLIPFLOP,cmd,node->CK->ORIGINAL_CLOCK);
            }
           else
            {
#ifdef STRIP_CMD
              ev0=stb_cmd_can_generate_event(stbfig, ptsig->NODE, cmd);
              ev1=stb_cmd_can_generate_event(stbfig, ptsig->NODE+1, cmd);
              if (ev0 || (!ev0 && !ev1))
#endif
                stb_chainstbck(stb_getstbnode (ptsig->NODE),
                               sumin, sumax, sdmin, sdmax, period,
                               STB_NO_TIME, STB_NO_TIME, index, active,
                               STB_VERIF_EDGE | verif, STB_TYPE_LATCH,cmd,node->CK->ORIGINAL_CLOCK);
#ifdef STRIP_CMD
              if (ev1 || (!ev0 && !ev1))
#endif
                stb_chainstbck(stb_getstbnode (ptsig->NODE + 1),
                               sumin, sumax, sdmin,
                               sdmax, period, STB_NO_TIME, STB_NO_TIME, index,
                               active, STB_VERIF_EDGE | verif, STB_TYPE_LATCH,cmd,node->CK->ORIGINAL_CLOCK);
            }
          }
        }
       else
        {
         if ((cmd->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)) ==
             (ckevent->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)))
          {
           if (sumax < node->CK->SUPMAX)
            sumax = node->CK->SUPMAX;
           if (sdmax < node->CK->SDNMAX)
            sdmax = node->CK->SDNMAX;
           if (sumin > node->CK->SUPMIN)
            sumin = node->CK->SUPMIN;
           if (sdmin > node->CK->SDNMIN)
            sdmin = node->CK->SDNMIN;
          }
         else
          {
           if (sdmax < node->CK->SUPMAX)
            sdmax = node->CK->SUPMAX;
           if (sumax < node->CK->SDNMAX)
            sumax = node->CK->SDNMAX;
           if (sdmin > node->CK->SUPMIN)
            sdmin = node->CK->SUPMIN;
           if (sumin > node->CK->SDNMIN)
            sumin = node->CK->SDNMIN;
          }
        }
      }

     else if (active != activex)
      stb_error (ERR_MULTIPLE_CLOCKS,
                 ttv_getsigname (stbfig->FIG, buf, cmd->ROOT), 0,
                 STB_WARNING);
    }

   if ((index != STB_NO_INDEX) && (STB_MULTIPLE_COMMAND != 'Y'))
    {
     period = stb_getstbnode (cke)->CK->PERIOD;
     if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
      {
       stb_addstbck (stb_getstbnode (ptsig->NODE), sumin, sumax, sdmin, sdmax,
                     period,
                     STB_NO_TIME, STB_NO_TIME, index, active,
                     STB_VERIF_EDGE | verif, STB_TYPE_FLIPFLOP, NULL);
       stb_addstbck (stb_getstbnode (ptsig->NODE + 1), sumin, sumax, sdmin,
                     sdmax, period, STB_NO_TIME, STB_NO_TIME, index,
                     active, STB_VERIF_EDGE | verif, STB_TYPE_FLIPFLOP, NULL);
      }
     else
      {
       stb_addstbck (stb_getstbnode (ptsig->NODE), sumin, sumax, sdmin, sdmax,
                     period,
                     STB_NO_TIME, STB_NO_TIME, index, active,
                     STB_VERIF_EDGE | verif, STB_TYPE_LATCH, NULL);
       stb_addstbck (stb_getstbnode (ptsig->NODE + 1), sumin, sumax, sdmin,
                     sdmax, period, STB_NO_TIME, STB_NO_TIME, index,
                     active, STB_VERIF_EDGE | verif, STB_TYPE_LATCH, NULL);
      }
    }
   else if(index == STB_NO_INDEX)
    {
     if ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
      {
       node = stb_getstbnode (ptsig->NODE);
       if (node->CK == NULL)
        stb_addstbck (node, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, index, active,
                      STB_NO_VERIF, STB_TYPE_FLIPFLOP, NULL);
       node = stb_getstbnode (ptsig->NODE + 1);
       if (node->CK == NULL)
        stb_addstbck (node, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, index, active,
                      STB_NO_VERIF, STB_TYPE_FLIPFLOP, NULL);
      }
     else
      {
       node = stb_getstbnode (ptsig->NODE);
       if (node->CK == NULL)
        stb_addstbck (node, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, index, active,
                      STB_NO_VERIF, STB_TYPE_LATCH, NULL);
       node = stb_getstbnode (ptsig->NODE + 1);
       if (node->CK == NULL)
        stb_addstbck (node, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, STB_NO_TIME,
                      STB_NO_TIME, STB_NO_TIME, index, active,
                      STB_NO_VERIF, STB_TYPE_LATCH, NULL);
      }
    }
   freechain (chaincmd);
   // vraie clock en debut de stbck
   stb_getstbck(stb_getstbnode (ptsig->NODE), NULL);
   stb_getstbck(stb_getstbnode (ptsig->NODE+1), NULL);
  }

 for (chain = cbo; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   if ((ptsig->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT)
    continue;

    warn=0;

   for (i = 0; i < 2; i++)
    {
     event = ptsig->NODE + i;
     if ((event->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT)
      continue;
     if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
      level = stbfig->FIG->INFO->LEVEL;
     else
      level = event->ROOT->ROOT->INFO->LEVEL;
     if (stbfig->GRAPH == STB_RED_GRAPH)
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CL_PJT, TTV_FILE_TTX);
       line = event->INPATH;
      }
     else
      {
       ttv_expfigsig (stbfig->FIG, event->ROOT, level,
                      stbfig->FIG->INFO->LEVEL, TTV_STS_CLS_FED, TTV_FILE_DTX);
       line = event->INLINE;
      }

     for (; line; line = line->NEXT)
      {
       if ((((line->TYPE & TTV_LINE_A) != TTV_LINE_A) ||
            (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
             (line->FIG != stbfig->FIG))) ||
           (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
            (line->FIG != stbfig->FIG) &&
            ((stbfig->STABILITYMODE & STB_STABILITY_LAST) ==
             STB_STABILITY_LAST))
           || ((line->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) ==
               TTV_SIG_BYPASSIN)
           || ((line->NODE->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
        continue;

       if((stb_getstbnode(line->ROOT) == NULL) ||
          (stb_getstbnode(line->NODE) == NULL))
           continue ;

       cmd = line->NODE;
       node = stb_getstbnode (event);
       cknode = stb_getstbnode (cmd);

       if (cknode->CK)
        {
         if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
          ckevent = (ttvevent_list *) ptype->DATA;
         else
          ckevent = cmd;

         if(node->CK == NULL)
           index = stb_getstbnode (ckevent)->CK->CKINDEX;
         else if(node->CK->CKINDEX == STB_NO_INDEX)
           index = stb_getstbnode (ckevent)->CK->CKINDEX;
         else
         {
           stb_getstbck(node, cmd);
           if (node->CK->CMD!=cmd)
             stb_getstbck(node, (cmd->TYPE & TTV_NODE_UP)==TTV_NODE_UP?cmd->ROOT->NODE:cmd->ROOT->NODE+1);
                 
           index = node->CK->CKINDEX;
         }

         if(node->CK != NULL && cknode->CK!=NULL && node->CK->ORIGINAL_CLOCK==NULL)
            node->CK->ORIGINAL_CLOCK=cknode->CK->ORIGINAL_CLOCK;

         if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           {
            if((cknode->CK->SUPMAX == STB_NO_TIME) ||
               (cknode->CK->SUPMIN == STB_NO_TIME))
              continue ;
           }
         else
           {
            if((cknode->CK->SDNMAX == STB_NO_TIME) ||
               (cknode->CK->SDNMIN == STB_NO_TIME))
              continue ;
           }

         if (index != STB_NO_INDEX)
          {
           valmax = ttv_getdelaymax (line);
           valmin = ttv_getdelaymin (line);

           if (valmin == TTV_NOTIME)
             valmin = valmax;
            
           if (valmax == TTV_NOTIME)
             valmax = valmin;

           if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
            {
             u = cknode->CK->SUPMAX + valmax;
             d = cknode->CK->SUPMIN + valmin ;
            }
           else
            {
             u = cknode->CK->SDNMAX + valmax;
             d = cknode->CK->SDNMIN + valmin;
            }

           cmdlinemax = ttv_getlinecmd(stbfig->FIG,line,TTV_LINE_CMDMAX) ;
           cmdlinemin = ttv_getlinecmd(stbfig->FIG,line,TTV_LINE_CMDMIN) ;

           if(((cmdlinemax != cmd) && (cmdlinemin != cmd)) || 
              (node->CK == NULL) || (node->CK->CKINDEX == STB_NO_INDEX))
            {
             if (!warn && (line->ROOT->ROOT->TYPE & TTV_SIG_L)!=0)
             {
               avt_errmsg(STB_ERRMSG, "046", AVT_WARNING, ttv_getsigname (stbfig->FIG, buf, line->ROOT->ROOT));
               warn++;
             }
             if ((line->ROOT->ROOT->TYPE & TTV_SIG_B)!=0)
             {
               if (node->SPECIN == NULL)
                node->SPECIN = stb_alloctab (stbfig->PHASENUMBER);

               node->SPECIN[(int) index] =
                stb_mergestbpairlist (node->SPECIN[(int) index],
                                      stb_addstbpair (NULL, d, u));
             }
            }
           else
            {
             if(cmdlinemax == cmd)
               {
                if(STB_MULTIPLE_COMMAND == 'Y' || (valmax >  node->CK->ACCESSMAX) ||
                   (node->CK->ACCESSMAX == 0))
                  node->CK->ACCESSMAX = valmax ;
               }
             if(cmdlinemin == cmd)
               {
                if(STB_MULTIPLE_COMMAND == 'Y' || (valmin <  node->CK->ACCESSMIN) ||
                   (node->CK->ACCESSMIN == 0))
                  node->CK->ACCESSMIN = valmin ;
               }
            }
          }
         stb_getstbck(node, NULL);
        }
      }
    }
  }

 freechain (cbo);

 for (lrbx = lrb; lrbx; lrbx = lrbx->NEXT)
  {
   ptsig = (ttvsig_list *) lrbx->TYPE;
   chaincmd = (chain_list *) lrbx->DATA;

   index = STB_NO_INDEX;
   event = NULL;
   active = 0;

   for (chcmd = chaincmd; chcmd; chcmd = chcmd->NEXT)
    {
     cmd = (ttvevent_list *) chcmd->DATA;
     node = stb_getstbnode (cmd);
     if (node->CK == NULL)
      continue;
     if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      event = cmd->ROOT->NODE;
     else
      event = cmd->ROOT->NODE + 1;
     if ((ptype = getptype (event->USER, STB_NODE_CLOCK)))
      {
       cke = (ttvevent_list *) ptype->DATA;
      }
     else
      {
       cke = event;
      }
     if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      {
       ckevent = (ttvevent_list *) ptype->DATA;
      }
     else
      {
       ckevent = cmd;
      }
     if (node->CK)
      {
       if (node->CK->ORIGINAL_CLOCK==NULL && getptype (cmd->USER, STB_NODE_CLOCK)!=NULL)
         node->CK->ORIGINAL_CLOCK=ckevent;

       if (index == STB_NO_INDEX)
        {
         if ((cmd->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)) ==
             (ckevent->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)))
          {
           sumax = node->CK->SUPMAX;
           sdmax = node->CK->SDNMAX;
           sumin = node->CK->SUPMIN;
           sdmin = node->CK->SDNMIN;
          }
         else
          {
           sdmax = node->CK->SUPMAX;
           sumax = node->CK->SDNMAX;
           sdmin = node->CK->SUPMIN;
           sumin = node->CK->SDNMIN;
           active = 1;
          }
         index = stb_getstbnode (ckevent)->CK->CKINDEX;
        }
       else
        {
         if ((cmd->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)) ==
             (ckevent->TYPE & (TTV_NODE_UP | TTV_NODE_DOWN)))
          {
           if (sumax < node->CK->SUPMAX)
            sumax = node->CK->SUPMAX;
           if (sdmax < node->CK->SDNMAX)
            sdmax = node->CK->SDNMAX;
           if (sumin > node->CK->SUPMIN)
            sumin = node->CK->SUPMIN;
           if (sdmin > node->CK->SDNMIN)
            sdmin = node->CK->SDNMIN;
          }
         else
          {
           if (sdmax < node->CK->SUPMAX)
            sdmax = node->CK->SUPMAX;
           if (sumax < node->CK->SDNMAX)
            sumax = node->CK->SDNMAX;
           if (sdmin > node->CK->SUPMIN)
            sdmin = node->CK->SUPMIN;
           if (sumin > node->CK->SDNMIN)
            sumin = node->CK->SDNMIN;
           active = 1;
          }
        }
      }
    }
   if (index != STB_NO_INDEX)
    period = stb_getstbnode (ckevent)->CK->PERIOD;
   for (i = 0; i < 2; i++)
    {
     node = stb_getstbnode (ptsig->NODE + i);
     if (node->CK == NULL)
      continue;
         if(node->CK->TYPE == STB_TYPE_COMMAND)
            continue ;
     if (index != STB_NO_INDEX)
      {
       node->CK->SUPMIN = sumin;
       node->CK->SUPMAX = sumax;
       node->CK->SDNMIN = sdmin;
       node->CK->SDNMAX = sdmax;
       node->CK->PERIOD = period;
      }
     if ((node->CK->CKINDEX == STB_NO_INDEX)
         && (node->CK->TYPE != STB_TYPE_CLOCK))
      node->CK->CKINDEX = index;
     if (index == STB_NO_INDEX)
      {
       node->CK->VERIF = STB_NO_VERIF;
      }
     if (active == 1)
      {
       if ((node->CK->VERIF & STB_UP) == STB_UP)
        {
         node->CK->VERIF &= ~(STB_UP);
         node->CK->VERIF |= STB_DN;
        }
       else if ((node->CK->VERIF & STB_DN) == STB_DN)
        {
         node->CK->VERIF &= ~(STB_DN);
         node->CK->VERIF |= STB_UP;
        }

       if ((node->CK->ACTIVE & STB_UP) == STB_UP)
        {
         node->CK->ACTIVE &= ~(STB_UP);
         node->CK->ACTIVE |= STB_DN;
        }
       else if ((node->CK->ACTIVE & STB_DN) == STB_DN)
        {
         node->CK->ACTIVE &= ~(STB_DN);
         node->CK->ACTIVE |= STB_UP;
        }
      }
    }
   freechain (chaincmd);
  }

 freeptype (lrb);

 for (chain = stbfig->COMMAND; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   for (i = 0; i < 2; i++)
    {
     cmd = ptsig->NODE + i;
     node = stb_getstbnode (cmd);
     if (((ptype = getptype (cmd->USER, STB_NODE_CLOCK))) && (node->CK))
      {
       if ((node->CK->ACTIVE & STB_STATE_ALL) == STB_STATE_ALL)
        {
         if ((node->CK->VERIF & STB_DN) == STB_DN)
          node->CK->ACTIVE = STB_STATE_DN;
         else
          node->CK->ACTIVE = STB_STATE_UP;
        }
       if (((((i == 0) && ((node->CK->ACTIVE & STB_STATE_UP) == STB_STATE_UP))
           || ((i == 1)
               && ((node->CK->ACTIVE & STB_STATE_DN) == STB_STATE_DN))) &&
               (STB_OPEN_LATCH_PHASE == 'N')) ||
           ((((i == 1) && ((node->CK->ACTIVE & STB_STATE_UP) == STB_STATE_UP))
           || ((i == 0)
               && ((node->CK->ACTIVE & STB_STATE_DN) == STB_STATE_DN))) &&
               (STB_OPEN_LATCH_PHASE == 'Y')))
        {
         ckevent = (ttvevent_list *) ptype->DATA;
         index = stb_getstbnode (ckevent)->CK->CKINDEX;
         node->CK->CKINDEX = index;
         node->CK->ORIGINAL_CLOCK=ckevent;
         if (i == 0)
         {
          stb_getstbnode (ptsig->NODE + 1)->CK->CKINDEX = index;
//          stb_getstbnode (ptsig->NODE + 1)->CK->ORIGINAL_CLOCK=ckevent;
         }
         else
         {
          stb_getstbnode (ptsig->NODE)->CK->CKINDEX = index;
//          stb_getstbnode (ptsig->NODE)->CK->ORIGINAL_CLOCK=ckevent;
         }
        }
      }
     else if (node->CK == NULL)
      stb_addstbck (node, STB_NO_TIME, STB_NO_TIME, STB_NO_TIME, STB_NO_TIME,
                    STB_NO_TIME,
                    stbfig->SETUP, stbfig->HOLD, STB_NO_INDEX, (char) 0,
                    STB_NO_VERIF, STB_TYPE_COMMAND, NULL);

    }

   // mise a jour des setup et hold pour les flip-flop et les latchs
   stb_initsetuphold (stbfig, ptsig);
  }

 // initialisation des SETUP et HOLD qui valent encore STB_NO_TIME pour les MEMORY
 stb_initunsetsetuphold(stbfig);

/* for (chain = stbfig->MEMORY; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   if ((ptype = getptype (ptsig->NODE->USER, STB_NODE_CLOCK)))
    ptsig->NODE->USER = delptype (ptsig->NODE->USER, STB_NODE_CLOCK);
   if ((ptype = getptype ((ptsig->NODE + 1)->USER, STB_NODE_CLOCK)))
    (ptsig->NODE + 1)->USER =
     delptype ((ptsig->NODE + 1)->USER, STB_NODE_CLOCK);
   chaincmd = ttv_getlrcmd (stbfig->FIG, ptsig);
   for (chcmd = chaincmd; chcmd; chcmd = chcmd->NEXT)
    {
     cmd = (ttvevent_list *) chcmd->DATA;
     if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      cmd->USER = delptype (cmd->USER, STB_NODE_CLOCK);
     if ((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      cmd = cmd->ROOT->NODE;
     else
      cmd = cmd->ROOT->NODE + 1;
     if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      cmd->USER = delptype (cmd->USER, STB_NODE_CLOCK);
    }
   freechain (chaincmd);
  }
*/
 for (chain = stbfig->COMMAND; chain; chain = chain->NEXT)
  {
   ptsig->TYPE &= ~(TTV_SIG_MARQUE);
  }

 for (chain = stbfig->BREAK; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
/*   if ((ptype = getptype (ptsig->NODE->USER, STB_NODE_CLOCK)))
    ptsig->NODE->USER = delptype (ptsig->NODE->USER, STB_NODE_CLOCK);
   if ((ptype = getptype ((ptsig->NODE + 1)->USER, STB_NODE_CLOCK)))
    (ptsig->NODE + 1)->USER =
     delptype ((ptsig->NODE + 1)->USER, STB_NODE_CLOCK);
*/
   if((node = stb_getstbnode (ptsig->NODE)) == NULL) continue ;

   if (node->CK != NULL)
    {
     if (node->CK->TYPE == STB_TYPE_COMMAND)
      {
       stbfig->COMMAND = addchain (stbfig->COMMAND, ptsig);
      }
    }
  }

 for (chain = stbfig->COMMAND; chain; chain = chain->NEXT)
  {
   ptsig->TYPE &= ~(TTV_SIG_MARQUE);
  }

 for (chain = stbfig->PRECHARGE; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   for (i = 0; i < 2; i++)
    {
     event = ptsig->NODE + i;

     if(STB_OPEN_LATCH_PHASE == 'N')
       {
        if((event->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
            cmd = event->ROOT->NODE ;
        else
            cmd = event->ROOT->NODE + 1 ;
       }
     else
       cmd = event ;

     if ((ptype = getptype (cmd->USER, STB_NODE_CLOCK)))
      {
       ckevent = (ttvevent_list *) ptype->DATA;
       stb_getstbnode (event)->CK->CKINDEX =
       stb_getstbnode (ckevent)->CK->CKINDEX;
//       stb_getstbnode (event)->CK->ORIGINAL_CLOCK=ckevent;
       cmd->USER = delptype (cmd->USER, STB_NODE_CLOCK);
      }
#if 0
     // [!] checker qu'il y a un cknode->CK plus loin sinon coredump
     node = stb_getstbnode (event);
     if (node->CK)
      if (node->CK->TYPE == STB_TYPE_EVAL)
       {
        ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
        if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) ==
            STB_STABILITY_LAST)
         path =
          ttv_getpathnocross_v2 (stbfig->FIG, stbfig->FIG, ptsig, NULL, TTV_DELAY_MAX,
                       TTV_DELAY_MIN, TTV_FIND_MAX | find | TTV_FIND_DUAL,0);
        else
         path =
          ttv_getpathnocross_v2 (stbfig->FIG, NULL, ptsig, NULL, TTV_DELAY_MAX,
                       TTV_DELAY_MIN, TTV_FIND_MAX | find | TTV_FIND_DUAL,0);
        ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
        s = STB_MIN_TIME;
        for (scanpath = path; scanpath; scanpath = scanpath->NEXT)
         {
          if (scanpath->NODE != event)
           continue;
          event = scanpath->ROOT;
          cknode = stb_getstbnode (event);
          if ((event->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
           {
            if ((cknode->CK->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
             {
              if (cknode->CK->CKINDEX < node->CK->CKINDEX)
               sl = cknode->CK->SDNMAX + cknode->CK->PERIOD;
              else
               sl = cknode->CK->SDNMAX;
              if (s < sl)
               s = sl;
             }
            else
             {
              if (cknode->CK->CKINDEX < node->CK->CKINDEX)
               sl = cknode->CK->SUPMAX + cknode->CK->PERIOD;
              else
               sl = cknode->CK->SUPMAX;
              if (s < sl)
               s = sl;
             }
           }
          else if ((event->ROOT->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
           {
            if ((cknode->CK->ACTIVE & STB_SLOPE_UP) == STB_SLOPE_UP)
             {
              if (cknode->CK->CKINDEX < node->CK->CKINDEX)
               sl = cknode->CK->SUPMAX + cknode->CK->PERIOD;
              else
               sl = cknode->CK->SUPMAX;
              if (s < sl)
               s = sl;
             }
            else
             {
              if (cknode->CK->CKINDEX < node->CK->CKINDEX)
               sl = cknode->CK->SDNMAX + cknode->CK->PERIOD;
              else
               sl = cknode->CK->SDNMAX;
              if (s < sl)
               s = sl;
             }
           }
         }
/*        if (s != STB_MIN_TIME)
         {
          if ((node->CK->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
           {
            if (s < node->CK->SDNMAX)
             node->CK->HOLD = s - node->CK->SDNMAX ;
           }
          else
           {
            if (s < node->CK->SUPMAX)
             node->CK->HOLD = s - node->CK->SUPMAX ;
           }
         }*/

        ttv_freepathlist (path);
       }
#endif
    }
  }
}
