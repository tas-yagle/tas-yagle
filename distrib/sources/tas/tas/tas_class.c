/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Verison 5                                               */
/*    Fichier : tas_class.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fonctions relatives aux calculs des fronts des cones                     */
/****************************************************************************/

#include "tas.h"

char *CELL ;

//extern char* yagGetName          __P(( losig_list* ));

/*****************************************************************************/
/*                          fonction tas_isnowire()                          */
/* verifie qu'il y a bien une resistance                                     */
/*****************************************************************************/
int tas_isnowire(losig)
losig_list *losig ;
{
 lowire_list *ptwire ;
 float resi = 0.0 ;
 char flag = 'N' ;

 if(losig->PRCN == NULL)
  return(0) ;

 for(ptwire = losig->PRCN->PWIRE ; ptwire != NULL ; ptwire = ptwire->NEXT)
  {
   resi += ptwire->RESI ;

   if(ptwire->RESI > 0.0)
     flag = 'Y' ;

   if((flag == 'Y') && (resi > 1.0))
     break ;
  }

 if(ptwire != NULL)
   return(1) ;
 else
   return(0) ;
}

/*****************************************************************************/
/*                          fonction tas_getfcritic()                        */
/* donne les fronts max et min d'un cone                                     */
/*****************************************************************************/
void tas_getfcritic(cone,frontmax,frontmin)
cone_list *cone ;
front_list *frontmax ;
front_list *frontmin ;
{
 delay_list *delay ;
 ptype_list *ptype ;
 edge_list *incone ;

 frontmax->FUP = TAS_NOFRONT ;
 frontmax->FDOWN = TAS_NOFRONT ;
 frontmin->FUP = TAS_NOFRONT ;
 frontmin->FDOWN = TAS_NOFRONT ;
 
 for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
  {
   if((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL)
    {
     delay = (delay_list *)ptype->DATA ;
     if(((delay->FLL != TAS_NOFRONT) && (delay->FLL > frontmax->FDOWN)) ||
        (frontmax->FDOWN == TAS_NOFRONT))
       frontmax->FDOWN = delay->FLL ;
     if(((delay->FHL != TAS_NOFRONT) && (delay->FHL > frontmax->FDOWN)) ||
        (frontmax->FDOWN == TAS_NOFRONT))
       frontmax->FDOWN = delay->FHL ;
     if(((delay->FLH != TAS_NOFRONT) && (delay->FLH > frontmax->FUP)) ||
        (frontmax->FUP == TAS_NOFRONT))
       frontmax->FUP = delay->FLH ;
     if(((delay->FHH != TAS_NOFRONT) && (delay->FHH > frontmax->FUP)) ||
        (frontmax->FUP == TAS_NOFRONT))
       frontmax->FUP = delay->FHH ;
    }
   if((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL) 
    {
     delay = (delay_list *)ptype->DATA ;
     if(((delay->FLL != TAS_NOFRONT) && (delay->FLL < frontmin->FDOWN)) ||
        (frontmin->FDOWN == TAS_NOFRONT))
       frontmin->FDOWN = delay->FLL ;
     if(((delay->FHL != TAS_NOFRONT) && (delay->FHL < frontmin->FDOWN)) ||
        (frontmin->FDOWN == TAS_NOFRONT))
       frontmin->FDOWN = delay->FHL ;
     if(((delay->FLH != TAS_NOFRONT) && (delay->FLH < frontmin->FUP)) ||
        (frontmin->FUP == TAS_NOFRONT))
       frontmin->FUP = delay->FLH ;
     if(((delay->FHH != TAS_NOFRONT) && (delay->FHH < frontmin->FUP)) ||
        (frontmin->FUP == TAS_NOFRONT))
       frontmin->FUP = delay->FHH ;
    }
   if((ptype = getptype(incone->USER,TAS_DELAY_MAXT)) != NULL) 
    {
     delay = (delay_list *)ptype->DATA ;
     if(((delay->FLL != TAS_NOFRONT) && (delay->FLL > frontmax->FDOWN)) ||
        (frontmax->FDOWN == TAS_NOFRONT))
       frontmax->FDOWN = delay->FLL ;
     if(((delay->FHL != TAS_NOFRONT) && (delay->FHL > frontmax->FDOWN)) ||
        (frontmax->FDOWN == TAS_NOFRONT))
       frontmax->FDOWN = delay->FHL ;
     if(((delay->FLH != TAS_NOFRONT) && (delay->FLH > frontmax->FUP)) ||
        (frontmax->FUP == TAS_NOFRONT))
       frontmax->FUP = delay->FLH ;
     if(((delay->FHH != TAS_NOFRONT) && (delay->FHH > frontmax->FUP)) ||
        (frontmax->FUP == TAS_NOFRONT))
       frontmax->FUP = delay->FHH ;
    }
   if((ptype = getptype(incone->USER,TAS_DELAY_MINT)) != NULL)
    {
     delay = (delay_list *)ptype->DATA ;
     if(((delay->FLL != TAS_NOFRONT) && (delay->FLL < frontmin->FDOWN)) ||
        (frontmin->FDOWN == TAS_NOFRONT))
       frontmin->FDOWN = delay->FLL ;
     if(((delay->FHL != TAS_NOFRONT) && (delay->FHL < frontmin->FDOWN)) ||
        (frontmin->FDOWN == TAS_NOFRONT))
       frontmin->FDOWN = delay->FHL ;
     if(((delay->FLH != TAS_NOFRONT) && (delay->FLH < frontmin->FUP)) ||
        (frontmin->FUP == TAS_NOFRONT))
       frontmin->FUP = delay->FLH ;
     if(((delay->FHH != TAS_NOFRONT) && (delay->FHH < frontmin->FUP)) ||
        (frontmin->FUP == TAS_NOFRONT))
       frontmin->FUP = delay->FHH ;
    }
  }
}

/*****************************************************************************/
/*                          fonction tas_comdelay()                          */
/* prend le plus critique des deux delays                                    */
/*****************************************************************************/
void tas_comdelay(delayext,delaycone,type)
delay_list *delayext ;
delay_list *delaycone ;
long type ;
{
 long delay1 ;
 long delay2 ;

 if(delayext->TPHH != TAS_NOTIME)
  delay1 = delayext->TPHH ; 
 else
  delay1 = TAS_NOTIME ; 

 if((delayext->RCHH != TAS_NOTIME) && (delay1 != TAS_NOTIME))
  delay1 += delayext->RCHH ;

 if(delaycone->TPHH != TAS_NOTIME)
  delay2 = delaycone->TPHH ;  
 else
  delay2 = TAS_NOTIME ;    

 if((delaycone->RCHH != TAS_NOTIME) && (delay2 != TAS_NOTIME))
  delay2 += delaycone->RCHH ;

 if((delay1 != TAS_NOTIME) && (delay2 != TAS_NOTIME))
   {
    if(type == TAS_DELAY_MAX)
     {
      if(delay1 > delay2){
        delaycone->TPHH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SHH = TAS_NOS ;
#endif
        delaycone->FHH = TAS_NOFRONT ;
        if(delaycone->TMHH){
            if(delaycone->FMHH && (delaycone->TMHH != delaycone->FMHH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMHH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMHH->NAME);
            delaycone->TMHH = NULL;
            delaycone->FMHH = NULL;
        }
      }else{
        delayext->TPHH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SHH = TAS_NOS ;
#endif
        delayext->FHH = TAS_NOFRONT ;
        if(delayext->TMHH){
            if(delayext->FMHH && (delayext->TMHH != delayext->FMHH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMHH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMHH->NAME);
            delayext->TMHH = NULL;
            delayext->FMHH = NULL;
        }
      }
     }
    else
     {
      if(delay1 < delay2){
        delaycone->TPHH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SHH = TAS_NOS ;
#endif
        delaycone->FHH = TAS_NOFRONT ;
        if(delaycone->TMHH){
            if(delaycone->FMHH && (delaycone->TMHH != delaycone->FMHH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMHH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMHH->NAME);
            delaycone->TMHH = NULL;
            delaycone->FMHH = NULL;
        }
      }else{
        delayext->TPHH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SHH = TAS_NOS ;
#endif
        delayext->FHH = TAS_NOFRONT ;
        if(delayext->TMHH){
            if(delayext->FMHH && (delayext->TMHH != delayext->FMHH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMHH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMHH->NAME);
            delayext->TMHH = NULL;
            delayext->FMHH = NULL;
        }
      }
     }
   }

 if(delayext->TPHL != TAS_NOTIME)
  delay1 = delayext->TPHL ;  
 else
  delay1 = TAS_NOTIME ;    

 if((delayext->RCHH != TAS_NOTIME) && (delay1 != TAS_NOTIME))
  delay1 += delayext->RCHH ;

 if(delaycone->TPHL != TAS_NOTIME)
  delay2 = delaycone->TPHL ;
 else
  delay2 = TAS_NOTIME ;

 if((delaycone->RCHH != TAS_NOTIME) && (delay2 != TAS_NOTIME))
  delay2 += delaycone->RCHH ;

 if((delay1 != TAS_NOTIME) && (delay2 != TAS_NOTIME))
   {
    if(type == TAS_DELAY_MAX)
     {
      if(delay1 > delay2){
        delaycone->TPHL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SHL = TAS_NOS ;
#endif
        delaycone->FHL = TAS_NOFRONT ;
        if(delaycone->TMHL){
            if(delaycone->FMHL && (delaycone->TMHL != delaycone->FMHL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMHL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMHL->NAME);
            delaycone->TMHL = NULL;
            delaycone->FMHL = NULL;
        }
      }else{
        delayext->TPHL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SHL = TAS_NOS ;
#endif
        delayext->FHL = TAS_NOFRONT ;
        if(delayext->TMHL){
            if(delayext->FMHL && (delayext->TMHL != delayext->FMHL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMHL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMHL->NAME);
            delayext->TMHL = NULL;
            delayext->FMHL = NULL;
        }
      }
     }
    else
     {
      if(delay1 < delay2){
        delaycone->TPHL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SHL = TAS_NOS ;
#endif
        delaycone->FHL = TAS_NOFRONT ;
        if(delaycone->TMHL){
            if(delaycone->FMHL && (delaycone->TMHL != delaycone->FMHL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMHL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMHL->NAME);
            delaycone->TMHL = NULL;
            delaycone->FMHL = NULL;
        }
      }else{
        delayext->TPHL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SHL = TAS_NOS ;
#endif
        delayext->FHL = TAS_NOFRONT ;
        if(delayext->TMHL){
            if(delayext->FMHL && (delayext->TMHL != delayext->FMHL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMHL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMHL->NAME);
            delayext->TMHL = NULL;
            delayext->FMHL = NULL;
        }
      }
     }
   }


 if(delayext->TPLL != TAS_NOTIME)
  delay1 = delayext->TPLL ;
 else
  delay1 = TAS_NOTIME ;

 if((delayext->RCLL != TAS_NOTIME) && (delay1 != TAS_NOTIME))
  delay1 += delayext->RCLL ;

 if(delaycone->TPLL != TAS_NOTIME)
  delay2 = delaycone->TPLL ;
 else
  delay2 = TAS_NOTIME ;

 if((delaycone->RCLL != TAS_NOTIME) && (delay2 != TAS_NOTIME))
  delay2 += delaycone->RCLL ;

 if((delay1 != TAS_NOTIME) && (delay2 != TAS_NOTIME))
   {
    if(type == TAS_DELAY_MAX)
     {
      if(delay1 > delay2){
        delaycone->TPLL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SLL = TAS_NOS ;
#endif
        delaycone->FLL = TAS_NOFRONT ;
        if(delaycone->TMLL){
            if(delaycone->FMLL && (delaycone->TMLL != delaycone->FMLL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMLL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMLL->NAME);
            delaycone->TMLL = NULL;
            delaycone->FMLL = NULL;
        }
      }else{
        delayext->TPLL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SLL = TAS_NOS ;
#endif
        delayext->FLL = TAS_NOFRONT ;
        if(delayext->TMLL){
            if(delayext->FMLL && (delayext->TMLL != delayext->FMLL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMLL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMLL->NAME);
            delayext->TMLL = NULL;
            delayext->FMLL = NULL;
        }
      }
     }
    else
     {
      if(delay1 < delay2){
        delaycone->TPLL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SLL = TAS_NOS ;
#endif
        delaycone->FLL = TAS_NOFRONT ;
        if(delaycone->TMLL && (delaycone->TMLL != delaycone->FMLL)){
            if(delaycone->FMLL){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMLL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMLL->NAME);
            delaycone->TMLL = NULL;
            delaycone->FMLL = NULL;
        }
      }else{
        delayext->TPLL = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SLL = TAS_NOS ;
#endif
        delayext->FLL = TAS_NOFRONT ;
        if(delayext->TMLL){
            if(delayext->FMLL && (delayext->TMLL != delayext->FMLL)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMLL->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMLL->NAME);
            delayext->TMLL = NULL;
            delayext->FMLL = NULL;
        }
      }
     }
   }

 if(delayext->TPLH != TAS_NOTIME)
  delay1 = delayext->TPLH ;
 else
  delay1 = TAS_NOTIME ;

 if((delayext->RCLL != TAS_NOTIME) && (delay1 != TAS_NOTIME))
  delay1 += delayext->RCLL ;

 if(delaycone->TPLH != TAS_NOTIME)
  delay2 = delaycone->TPLH ;
 else
  delay2 = TAS_NOTIME ;

 if((delaycone->RCLL != TAS_NOTIME) && (delay2 != TAS_NOTIME))
  delay2 += delaycone->RCLL ;

 if((delay1 != TAS_NOTIME) && (delay2 != TAS_NOTIME))
   {
    if(type == TAS_DELAY_MAX)
     {
      if(delay1 > delay2){
        delaycone->TPLH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SLH = TAS_NOS ;
#endif
        delaycone->FLH = TAS_NOFRONT ;
        if(delaycone->TMLH){
            if(delaycone->FMLH && (delaycone->TMLH != delaycone->FMLH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMLH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMLH->NAME);
            delaycone->TMLH = NULL;
            delaycone->FMLH = NULL;
        }
      }else{
        delayext->TPLH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SLH = TAS_NOS ;
#endif
        delayext->FLH = TAS_NOFRONT ;
        if(delayext->TMLH){
            if(delayext->FMLH && (delayext->TMLH != delayext->FMLH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMLH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMLH->NAME);
            delayext->TMLH = NULL;
            delayext->FMLH = NULL;
        }
      }
     }
    else
     {
      if(delay1 < delay2){
        delaycone->TPLH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delaycone->SLH = TAS_NOS ;
#endif
        delaycone->FLH = TAS_NOFRONT ;
        if(delaycone->TMLH){
            if(delaycone->FMLH && (delaycone->TMLH != delaycone->FMLH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delaycone->FMLH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delaycone->TMLH->NAME);
            delaycone->TMLH = NULL;
            delaycone->FMLH = NULL;
        }
      }else{
        delayext->TPLH = TAS_NOTIME ;
#ifdef USEOLDTEMP
        delayext->SLH = TAS_NOS ;
#endif
        delayext->FLH = TAS_NOFRONT ;
        if(delayext->TMLH){
            if(delayext->FMLH && (delayext->TMLH != delayext->FMLH)){
                if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                    stm_freemodel (CELL, delayext->FMLH->NAME);
            }
            if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
                stm_freemodel (CELL, delayext->TMLH->NAME);
            delayext->TMLH = NULL;
            delayext->FMLH = NULL;
        }
      }
     }
   }

if((delayext->TPHH == TAS_NOTIME) && (delayext->TPHL == TAS_NOTIME))
  delayext->RCHH = TAS_NOTIME ;
if((delayext->TPLL == TAS_NOTIME) && (delayext->TPLH == TAS_NOTIME))
  delayext->RCLL = TAS_NOTIME ;

if((delaycone->TPHH == TAS_NOTIME) && (delaycone->TPHL == TAS_NOTIME))
  delaycone->RCHH = TAS_NOTIME ;
if((delaycone->TPLL == TAS_NOTIME) && (delaycone->TPLH == TAS_NOTIME))
  delaycone->RCLL = TAS_NOTIME ;
}

/*****************************************************************************/
/*                          fonction tas_cleanloconin()                      */
/* detect les entrees sur connecteur et sur cone                             */
/*****************************************************************************/
void tas_cleanloconin(cnsfig)
cnsfig_list *cnsfig ;
{
 locon_list *locon ;
 ptype_list *ptype ;
 chain_list *chain ;
 cone_list *conein ;
 cone_list *cone ;
 edge_list *incone ;
 edge_list *inconecone ;
 edge_list *inconelocon ;
 delay_list *delayext ;
 delay_list *delaycone ;

 for(locon = cnsfig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   if((ptype = getptype(locon->USER,CNS_EXT)) != NULL)
     {
      conein = (cone_list *)ptype->DATA ;
      if((ptype = getptype(locon->USER,CNS_CONE)) != NULL)
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
            chain = chain->NEXT)
         {
          cone = (cone_list *)chain->DATA ;
          inconecone = NULL ;
          inconelocon = NULL ;
          for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
           {
            if((incone->TYPE & CNS_EXT) == CNS_EXT)
              {
               if(incone->UEDGE.LOCON == locon)
                 inconelocon = incone ;
              }
            else
              {
               if(incone->UEDGE.CONE == conein)
                 inconecone = incone ;
              }
           }
          if((inconecone != NULL) && (inconelocon != NULL))
           {
            if((ptype = getptype(inconelocon->USER,TAS_DELAY_MIN)) != NULL)
             {
              delayext = ptype->DATA ;
              if((ptype = getptype(inconecone->USER,TAS_DELAY_MIN)) != NULL)
               {
                delaycone = ptype->DATA ;
                tas_comdelay(delayext,delaycone,TAS_DELAY_MIN) ;
               }
             }
            if((ptype = getptype(inconelocon->USER,TAS_DELAY_MAX)) != NULL)
             {
              delayext = ptype->DATA ;
              if((ptype = getptype(inconecone->USER,TAS_DELAY_MAX)) != NULL)
               {
                delaycone = ptype->DATA ;
                tas_comdelay(delayext,delaycone,TAS_DELAY_MAX) ;
               }
             }
           }
         }
       }
     }
  }
}

/*****************************************************************************/
/*                          fonction tas_mergercdelay()                      */
/* merge les delay de portes et les delay rc                                 */
/*****************************************************************************/
void tas_mergercdelay(cnsfig)
cnsfig_list *cnsfig ;
{
 cone_list *cone ;
 cone_list *conex ;
 edge_list *incone ;
 locon_list *locon ;
 delay_list *delay ;
 delay_list *delayxmin ;
 delay_list *delayxmax ;
 ptype_list *ptype ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 ptype_list *ptypemaxuu ;
 ptype_list *ptypemaxdd ;
 ptype_list *ptypeminuu ;
 ptype_list *ptypemindd ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {
     if((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL)
      {
       delay = (delay_list *)ptype->DATA ;
       if((delay->TPHH != TAS_NOTIME) && (delay->RCHH != TAS_NOTIME))
         {
          delay->TPHH += delay->RCHH ;
         }
       if((delay->TPHL != TAS_NOTIME) && (delay->RCHH != TAS_NOTIME))
         {
          delay->TPHL += delay->RCHH ;
         }
       if((delay->TPLH != TAS_NOTIME) && (delay->RCLL != TAS_NOTIME))
         {
          delay->TPLH += delay->RCLL ;
         }
       if((delay->TPLL != TAS_NOTIME) && (delay->RCLL != TAS_NOTIME))
         {
          delay->TPLL += delay->RCLL ;
         }
      }
     if((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL)
      {
       delay = (delay_list *)ptype->DATA ;
       if((delay->TPHH != TAS_NOTIME) && (delay->RCHH != TAS_NOTIME))
         {
          delay->TPHH += delay->RCHH ;
         }
       if((delay->TPHL != TAS_NOTIME) && (delay->RCHH != TAS_NOTIME))
         {
          delay->TPHL += delay->RCHH ;
         }
       if((delay->TPLH != TAS_NOTIME) && (delay->RCLL != TAS_NOTIME))
         {
          delay->TPLH += delay->RCLL ;
         }
       if((delay->TPLL != TAS_NOTIME) && (delay->RCLL != TAS_NOTIME))
         {
          delay->TPLL += delay->RCLL ;
         }
      }
    }

   if((cone->TYPE & CNS_EXT) == CNS_EXT)
     {
       chain_list *cl;
       cl=cns_get_cone_external_connectors(cone);
       while (cl!=NULL)
       {
      /*ptype = getptype(cone->USER,CNS_EXT) ;
      if(ptype != NULL)*/
         
//         locon = (locon_list *)ptype->DATA ;
         locon = (locon_list *)cl->DATA ;
         if((locon->DIRECTION == CNS_O) ||
         (locon->DIRECTION == CNS_B) ||
         (locon->DIRECTION == CNS_T) ||
         (locon->DIRECTION == CNS_Z))
          {
           if((ptype = getptype(locon->USER,TAS_DELAY_MAX)) != NULL) 
            {
             delayxmax = (delay_list *)ptype->DATA ;
            }
           else
             delayxmax = NULL ;
           if((ptype = getptype(locon->USER,TAS_DELAY_MIN)) != NULL) 
            {
             delayxmin = (delay_list *)ptype->DATA ;
            }
           else
             delayxmin = NULL ;
           for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
            {
             if(((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL) &&
                (delayxmin != NULL))
              {
               delay = (delay_list *)ptype->DATA ;
               if((delay->TPHH != TAS_NOTIME) && (delayxmin->RCHH != TAS_NOTIME))
                 {
                  delay->TPHH += delayxmin->RCHH ;
                 }
               if((delay->TPLH != TAS_NOTIME) && (delayxmin->RCHH != TAS_NOTIME))
                 {
                  delay->TPLH += delayxmin->RCHH ;
                 }
               if((delay->TPHL != TAS_NOTIME) && (delayxmin->RCLL != TAS_NOTIME))
                 {
                  delay->TPHL += delayxmin->RCLL ;
                 }
               if((delay->TPLL != TAS_NOTIME) && (delayxmin->RCLL != TAS_NOTIME))
                 {
                  delay->TPLL += delayxmin->RCLL ;
                 }
              }
             if(((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL) &&
                (delayxmax != NULL))
              {
               delay = (delay_list *)ptype->DATA ;
               if((delay->TPHH != TAS_NOTIME) && (delayxmax->RCHH != TAS_NOTIME))
                 {
                  delay->TPHH += delayxmax->RCHH ;
                 }
               if((delay->TPLH != TAS_NOTIME) && (delayxmax->RCHH != TAS_NOTIME))
                 {
                  delay->TPLH += delayxmax->RCHH ;
                 }
               if((delay->TPHL != TAS_NOTIME) && (delayxmax->RCLL != TAS_NOTIME))
                 {
                  delay->TPHL += delayxmax->RCLL ;
                 }
               if((delay->TPLL != TAS_NOTIME) && (delayxmax->RCLL != TAS_NOTIME))
                 {
                  delay->TPLL += delayxmax->RCLL ;
                 }
              }
            }
          }
         
       cl=delchain(cl,cl);
       }
     }

   if((cone->TYPE & (CNS_LATCH | CNS_FLIP_FLOP)) != 0)
     {
      for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
       {
        if((incone->TYPE & (CNS_EXT|CNS_COMMAND)) == CNS_COMMAND)
         {
          conex = incone->UEDGE.CONE ;

          if((conex->TYPE & TAS_DEJAEMPILE) != TAS_DEJAEMPILE)
           {
            chain = addchain(chain,(void*)conex) ;
            conex->TYPE |= TAS_DEJAEMPILE ;
           }

          if((ptypeminuu = getptype(conex->USER,TAS_RC_MINHH)) == NULL)
           {
           conex->USER = addptype(conex->USER,TAS_RC_MINHH,(void *)TAS_NOTIME) ;
           ptypeminuu = conex->USER ;
           }

          if((ptypemindd = getptype(conex->USER,TAS_RC_MINLL)) == NULL)
           {
           conex->USER = addptype(conex->USER,TAS_RC_MINLL,(void *)TAS_NOTIME) ;
           ptypemindd = conex->USER ;
           }

          if((ptypemaxuu = getptype(conex->USER,TAS_RC_MAXHH)) == NULL)
           {
           conex->USER = addptype(conex->USER,TAS_RC_MAXHH,(void *)TAS_NOTIME) ;
           ptypemaxuu = conex->USER ;
           }

          if((ptypemaxdd = getptype(conex->USER,TAS_RC_MAXLL)) == NULL)
           {
           conex->USER = addptype(conex->USER,TAS_RC_MAXLL,(void *)TAS_NOTIME) ;
           ptypemaxdd = conex->USER ;
           }
    
          if((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL) 
           {
            delayxmax = (delay_list *)ptype->DATA ;
           }
          if((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL) 
           {
            delayxmin = (delay_list *)ptype->DATA ;
           }
          if(delayxmin->RCHH != TAS_NOTIME)
           {
            if(((long)ptypeminuu->DATA == TAS_NOTIME) || 
               ((long)ptypeminuu->DATA > delayxmin->RCHH))
              ptypeminuu->DATA = (void *)delayxmin->RCHH ;
           }
          if(delayxmin->RCLL != TAS_NOTIME)
           {
            if(((long)ptypemindd->DATA == TAS_NOTIME) || 
               ((long)ptypemindd->DATA > delayxmin->RCLL))
              ptypemindd->DATA = (void *)delayxmin->RCLL ;
           }
          if(delayxmax->RCHH != TAS_NOTIME)
           {
            if(((long)ptypemaxuu->DATA == TAS_NOTIME) || 
               ((long)ptypemaxuu->DATA < delayxmax->RCHH))
              ptypemaxuu->DATA = (void *)delayxmax->RCHH ;
           }
          if(delayxmax->RCLL != TAS_NOTIME)
           {
            if(((long)ptypemaxdd->DATA == TAS_NOTIME) || 
               ((long)ptypemaxdd->DATA < delayxmax->RCLL))
              ptypemaxdd->DATA = (void *)delayxmax->RCLL ;
           }
         }
       }
     }
  }

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  {
   cone = (cone_list *)chainx->DATA ;
   cone->TYPE &= ~(TAS_DEJAEMPILE) ;

   ptypeminuu = getptype(cone->USER,TAS_RC_MINHH) ;
   ptypemindd = getptype(cone->USER,TAS_RC_MINLL) ;
   ptypemaxuu = getptype(cone->USER,TAS_RC_MAXHH) ;
   ptypemaxdd = getptype(cone->USER,TAS_RC_MAXLL) ;
    
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
     {
      if((incone->TYPE & CNS_EXT) == CNS_EXT)
        continue ;
      if((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL)
       {
        delay = (delay_list *)ptype->DATA ;
        if((delay->TPHH != TAS_NOTIME) && 
           ((long)ptypeminuu->DATA != TAS_NOTIME))
          {
           delay->TPHH += (long)ptypeminuu->DATA ;
          }
        if((delay->TPLH != TAS_NOTIME) && 
           ((long)ptypeminuu->DATA != TAS_NOTIME))
          {
           delay->TPLH += (long)ptypeminuu->DATA ;
          }
        if((delay->TPHL != TAS_NOTIME) && 
           ((long)ptypemindd->DATA != TAS_NOTIME))
          {
           delay->TPHL += (long)ptypemindd->DATA ;
          }
        if((delay->TPLL != TAS_NOTIME) && 
           ((long)ptypemindd->DATA != TAS_NOTIME))
          {
           delay->TPLL += (long)ptypemindd->DATA ;
          }
       }
      if((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL)
       {
        delay = (delay_list *)ptype->DATA ;
        if((delay->TPHH != TAS_NOTIME) && 
           ((long)ptypemaxuu->DATA != TAS_NOTIME))
          {
           delay->TPHH += (long)ptypemaxuu->DATA ;
          }
        if((delay->TPLH != TAS_NOTIME) && 
           ((long)ptypemaxuu->DATA != TAS_NOTIME))
          {
           delay->TPLH += (long)ptypemaxuu->DATA ;
          }
        if((delay->TPHL != TAS_NOTIME) && 
           ((long)ptypemaxdd->DATA != TAS_NOTIME))
          {
           delay->TPHL += (long)ptypemaxdd->DATA ;
          }
        if((delay->TPLL != TAS_NOTIME) && 
           ((long)ptypemaxdd->DATA != TAS_NOTIME))
          {
           delay->TPLL += (long)ptypemaxdd->DATA ;
          }
       }
     }
   cone->USER = delptype(cone->USER,TAS_RC_MINHH) ;
   cone->USER = delptype(cone->USER,TAS_RC_MINLL) ;
   cone->USER = delptype(cone->USER,TAS_RC_MAXHH) ;
   cone->USER = delptype(cone->USER,TAS_RC_MAXLL) ;
  }

 freechain(chain) ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {
     if((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL)
      {
       delay = (delay_list *)ptype->DATA ;
       delay->RCLL=delay->RCHH=TAS_NOTIME;
      }
     if((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL)
      {
       delay = (delay_list *)ptype->DATA ;
       delay->RCLL=delay->RCHH=TAS_NOTIME;
      }
    }
  }

}

/*****************************************************************************
*                        fonction  tas_getlosigcone()                        *
*    renvoie le signal du cone s'il existe                                   *
*****************************************************************************/
losig_list *tas_getlosigcone(cone)
cone_list *cone ;
{
 branch_list *branch[3];
 link_list *link ;
 link_list *link1 = NULL ;
 link_list *link2 = NULL ;
 int i ;

 branch[0]=cone->BRVDD ;
 branch[1]=cone->BRVSS ;
 branch[2]=cone->BREXT ;

 for(i = 0 ; i < 3 ; i++)
  {
   for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
    {
     for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT)
      {
       if(link1 == NULL)
          link1 = link ;
       else if(link2 == NULL)
          link2 = link ;
       else
         break ;
      }
     if(link1 != NULL)
      break ;
    }
   if(link1 != NULL)
    break ;
  }

 if(link1 == NULL)
    {
     return(NULL) ;
    }
 else if(link2 == NULL)
    {
     if((link1->TYPE & (CNS_IN | CNS_INOUT)) != 0)
      {
       return(link1->ULINK.LOCON->SIG) ;
      }
     else
      {
       if((link1->ULINK.LOTRS->DRAIN == NULL) || 
          (link1->ULINK.LOTRS->SOURCE == NULL))
         return(NULL) ;
       if((link1->ULINK.LOTRS->DRAIN->SIG == NULL) || 
          (link1->ULINK.LOTRS->SOURCE->SIG == NULL))
         return(NULL) ;
       if(link1->ULINK.LOTRS->DRAIN->SIG->TYPE == TAS_ALIM)
         return(link1->ULINK.LOTRS->SOURCE->SIG) ;
       else
         return(link1->ULINK.LOTRS->DRAIN->SIG) ;
      }
    }
 else
    {
     if((link1->ULINK.LOTRS->DRAIN == NULL) || 
        (link1->ULINK.LOTRS->SOURCE == NULL))
       return(NULL) ;
     if((link1->ULINK.LOTRS->DRAIN->SIG == NULL) || 
        (link1->ULINK.LOTRS->SOURCE->SIG == NULL))
       return(NULL) ;
     if((link2->TYPE & (CNS_IN | CNS_INOUT)) != 0)
      {
       if(link2->ULINK.LOCON->SIG == NULL)
          return(NULL) ;
       if(link2->ULINK.LOCON->SIG == link1->ULINK.LOTRS->DRAIN->SIG)
        return(link1->ULINK.LOTRS->SOURCE->SIG) ;
       else
        return(link1->ULINK.LOTRS->DRAIN->SIG) ;
      }
     else
      {
       if((link2->ULINK.LOTRS->DRAIN == NULL) || 
          (link2->ULINK.LOTRS->SOURCE == NULL))
         return(NULL) ;
       if((link2->ULINK.LOTRS->DRAIN->SIG == NULL) || 
          (link2->ULINK.LOTRS->SOURCE->SIG == NULL))
         return(NULL) ;
       if((link2->ULINK.LOTRS->DRAIN->SIG == link1->ULINK.LOTRS->DRAIN->SIG) ||
          (link2->ULINK.LOTRS->SOURCE->SIG == link1->ULINK.LOTRS->DRAIN->SIG))
        return(link1->ULINK.LOTRS->SOURCE->SIG) ;
       else
        return(link1->ULINK.LOTRS->DRAIN->SIG) ;
      }
    }
}

/*****************************************************************************
*                        fonction  tas_setelmlimits()                        *
*    met les limits de delai d'elmore des signaux                            *
*****************************************************************************/
void tas_setelmlimits(losig,max,min)
losig_list *losig ;
long max ;
long min ;
{
 ptype_list *ptype ;

 if(TAS_CONTEXT->TAS_FIND_MIN != 'Y')
  min = max ;

 ptype = getptype(losig->USER,TAS_SIG_MAXELM) ;
 if(ptype == NULL)
  {
   losig->USER = addptype(losig->USER,TAS_SIG_MAXELM,(void *)max) ;
  }
 else
  {
   if(max > (long)ptype->DATA)
    ptype->DATA = (void *)max ;
  }
 ptype = getptype(losig->USER,TAS_SIG_MINELM) ;
 if(ptype == NULL)
  {
   losig->USER = addptype(losig->USER,TAS_SIG_MINELM,(void *)min) ;
  }
 else
  {
   if(min < (long)ptype->DATA)
    ptype->DATA = (void *)min ;
  }
}

/*****************************************************************************
*                        fonction  tas_reducechaincon()                      *
*    reduit la list des connecteurs                                          *
*****************************************************************************/
chain_list *tas_reducechaincon(chaincon)
chain_list *chaincon ;
{
 locon_list *locon ;
 locon_list *loconnext ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainw ;
 chain_list *chainwire ;
 chain_list *chainres = NULL ;
 num_list *node ;
 num_list *nodex ;
 float res ;
 float maxres ;
 char flag ;
 
 for(chain = chaincon ; chain != NULL ; chain = chain->NEXT)
   {
    locon = (locon_list *)chain->DATA ;
    if(getptype(locon->USER,TAS_CON_MARQUE) == NULL)
      locon->USER = addptype(locon->USER,TAS_CON_MARQUE,NULL) ;
   }

 for(chain = chaincon ; chain != NULL ; chain = chain->NEXT)
  {
   locon = (locon_list *)chain->DATA ;
   if(getptype(locon->USER,TAS_CON_MARQUE) == NULL)
    continue ;
   for(chainx = chain->NEXT ; chainx != NULL ; chainx = chainx->NEXT)
    {
     loconnext = (locon_list *)chainx->DATA ;
     if(getptype(loconnext->USER,TAS_CON_MARQUE) == NULL)
      continue ;
     maxres = 0.0 ;
     flag = 'N' ;
     for(node = locon->PNODE ; node != NULL ; node = node->NEXT)
      {
       for(nodex = loconnext->PNODE ; nodex != NULL ; nodex = nodex->NEXT)
        {
         chainwire = NULL ;
         res = 0.0 ;
         if(node->DATA == nodex->DATA/* || (chainwire = getway(locon->SIG,node->DATA, nodex->DATA)) != NULL*/)
           {
            flag = 'Y' ;
           }
         for(chainw = chainwire ; chainw != NULL ; chainw = chainw->NEXT)
          {
           res += ((lowire_list *)chainw->DATA)->RESI ;
          }
         freechain(chainwire) ;
        }
       if(res > maxres)
        maxres = res ;
      }
     if((maxres < 10.0) && (flag == 'Y'))
      loconnext->USER = delptype(loconnext->USER,TAS_CON_MARQUE) ;
    }
  }
 for(chain = chaincon ; chain != NULL ; chain = chain->NEXT)
   {
    locon = (locon_list *)chain->DATA ;
    if(getptype(locon->USER,TAS_CON_MARQUE) != NULL)
     {
      locon->USER = delptype(locon->USER,TAS_CON_MARQUE) ;
      chainres = addchain(chainres,locon) ;
     }
   }
 return(chainres) ;
}

/*****************************************************************************
*                        fonction  tas_calc_not_func()                       *
*****************************************************************************/
void tas_calc_not_func(cnsfig)
cnsfig_list *cnsfig ;
{
 locon_list *locon ;
 chain_list *chain ;
 cone_list *cone ;
 front_list *front ;
 ptype_list *ptype ;
 char        flag ;
 
 if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
  {
   for(locon =  cnsfig->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
     if((locon->DIRECTION == CNS_B) || (locon->DIRECTION == CNS_O) ||
         (locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_T))
       {
        ptype = getptype(locon->USER,CNS_EXT) ;
        if(ptype != NULL)
         {
          cone = (cone_list *)ptype->DATA ;
          chain = NULL ;
          if(locon->SIG != NULL)
          if((ptype = getptype(locon->SIG->USER,LOFIGCHAIN)) != NULL)
            {
             for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
                 chain = chain->NEXT)
               if((((locon_list *)chain->DATA)->TYPE == INTERNAL) &&
                  ((((locon_list *)chain->DATA)->DIRECTION == CNS_B) || 
                  (((locon_list *)chain->DATA)->DIRECTION == CNS_O) ||
                  (((locon_list *)chain->DATA)->DIRECTION == CNS_Z) || 
                  (((locon_list *)chain->DATA)->DIRECTION == CNS_T)))
                   break ; 
            }

          if(chain != NULL)
            continue ;

          if(cone->BRVDD == NULL)
           flag = 'Y' ;
          else if((cone->BRVDD->TYPE & CNS_NOT_FUNCTIONAL) == 
                   CNS_NOT_FUNCTIONAL)
           flag = 'Y' ;
          else
           flag = 'N' ;
          if(flag == 'Y') 
           {
            front = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA ;
            front->FUP = TAS_NOFRONT ;
            front = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA ;
            front->FUP = TAS_NOFRONT ;
           }
          if(cone->BRVSS == NULL)
           flag = 'Y' ;
          else if((cone->BRVSS->TYPE & CNS_NOT_FUNCTIONAL) == 
                   CNS_NOT_FUNCTIONAL)
           flag = 'Y' ;
          else
           flag = 'N' ;
          if(flag == 'Y') 
           {
            front = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA ;
            front->FDOWN = TAS_NOFRONT ;
            front = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA ;
            front->FDOWN = TAS_NOFRONT ;
           }
         }
       }
    }
  }
}

/*****************************************************************************
*                        fonction  tas_tprc_ext()                            *
*****************************************************************************/
long tas_tprc_ext(ifl,cone,nbfront)
inffig_list *ifl;
cone_list *cone ;
long nbfront ;
{
 locon_list *locon ;
 losig_list *losig ;
 edge_list *incone ;

   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {
     if((incone->TYPE & CNS_EXT) == CNS_EXT)
      {
       locon = incone->UEDGE.LOCON ;
       losig = locon->SIG ;

       if(locon->TYPE != EXTERNAL)
          continue ;

       nbfront = tas_tprcincone(ifl,cone,incone,'N','Y',nbfront) ;

      }
    }
   return nbfront;
}

/*****************************************************************************
*                        fonction  tas_tprc_cone()                           *
*****************************************************************************/
long tas_tprc_cone(ifl,cone, nbfront)
inffig_list *ifl;
cone_list *cone ;
long nbfront;
{
 losig_list *losig ;
 locon_list *locon ;
 edge_list *incone ;
 edge_list *outcone ;
 chain_list *chainincone ;
 chain_list *chaincone ;
 chain_list *chain ;
 chain_list *chainx ;
 char        flag ;

   chainincone = NULL ;
   chaincone = NULL ;
   
   if((losig = tas_getlosigcone(cone)) == NULL) 
       return (nbfront) ;

   if(getptype(losig->USER,TAS_SIG_NORCDELAY) != NULL)
       return (nbfront) ;

   if((cone->TYPE & CNS_EXT) == CNS_EXT)
    {
      chain_list *cl;
      cl=cns_get_cone_external_connectors(cone);
      if (cl!=NULL)
      {
        locon = (locon_list *)cl->DATA;//getptype(cone->USER,CNS_EXT)->DATA ; 
        if(locon->TYPE == EXTERNAL)
          nbfront = tas_tprcincone(ifl,cone,NULL,'Y','N',nbfront) ;
        freechain(cl);
      } 
    }

   for(outcone = cone->OUTCONE ; outcone != NULL ; outcone = outcone->NEXT)
    {
     if((outcone->TYPE & CNS_EXT) != CNS_EXT)
      {
       for(incone = outcone->UEDGE.CONE->INCONE ; incone != NULL ; 
           incone = incone->NEXT)
        {
         if((incone->TYPE & CNS_EXT) != CNS_EXT)
           if(incone->UEDGE.CONE == cone)
            {
             chainincone = addchain(chainincone,incone) ;
             chaincone = addchain(chaincone,outcone->UEDGE.CONE) ;
            }
        }
      }
    }

   for(chain = chainincone , chainx = chaincone ;
       chain != NULL ; chain = chain->NEXT , chainx = chainx->NEXT)
    {
     incone = (edge_list *)chain->DATA ;
     nbfront = tas_tprcincone(ifl,(cone_list *)chainx->DATA,incone,'N','Y',nbfront);
     flag = 'N' ;
    }

   freechain(chainincone) ;
   freechain(chaincone) ;

   return (nbfront) ;
}

/*****************************************************************************
*                        fonction  tas_tprcincone()                          *
*    calcul des delai d'elmore entre les cones                               *
*****************************************************************************/
long tas_tprcincone(ifl,cone,incone,flagcone,flagincone,nbfront )
inffig_list *ifl;
cone_list *cone ;
edge_list *incone ;
char flagcone ;
char flagincone ;
long nbfront ;
{
 static front_list *front ;
 locon_list *loconcone ;
 locon_list *loconincone ;
 locon_list *locont ;
 locon_list *locontx ;
 ptype_list *ptypecone ;
 ptype_list *ptypeincone ;
 chain_list *chaincone ;
 chain_list *chainincone ;
 chain_list *chainin ;
 chain_list *chainc ;
 delay_list *delay ;
 delay_list *delaymax ;
 delay_list *delaymin ;
 front_list *frontconemax ;
 front_list *frontconemin ;
 front_list *frontinconemax ;
 front_list *frontinconemin ;
 rcx_list *ptrcx ;
 ptype_list *ptype ;
 long        valmaxcup, valfmaxcup ;
 long        valmincup, valfmincup ;
 long        valmaxup, valfmaxup  ;
 long        valminup, valfminup ;
 long        valmaxxup, valfmaxxup ;
 long        valminxup, valfminxup ;
 long        valmaxcdown, valfmaxcdown ;
 long        valmincdown, valfmincdown ;
 long        valmaxdown, valfmaxdown ;
 long        valmindown, valfmindown ;
 long        valmaxxdown, valfmaxxdown ;
 long        valminxdown, valfminxdown ;
 long        nbdelay = (long)0 ;
 char        flag ;
 stm_pwl     *pwlmax_up_out ;
 stm_pwl     *pwlmax_dw_out ;
 stm_pwl     *pwlmin_up_out ;
 stm_pwl     *pwlmin_dw_out ;
 mbk_pwl     *mbkpwlmax = NULL;
 mbk_pwl     *mbkpwlmin = NULL;
 mbk_pwl     **ptmbkpwlmax;
 mbk_pwl     **ptmbkpwlmin;

 RCXFLOAT    dmaxup = 0.0,
             dminup = 0.0,
             fmaxup = 0.0,
             fminup = 0.0,
             dmaxdown = 0.0,
             dmindown = 0.0,
             fmaxdown = 0.0,
             fmindown = 0.0;
 rcx_slope   slope;
 cone_list  *driver = NULL;
 output_carac_trans s_carac_min ;
 output_carac_trans s_carac_max ;
 stm_pwl    *pwlmaxup = NULL ;
 stm_pwl    *pwlminup = NULL ;
 stm_pwl    *pwlmaxdw = NULL ;
 stm_pwl    *pwlmindw = NULL ;
 stm_pwl    *pwlmaxcup = NULL ;
 stm_pwl    *pwlmincup = NULL ;
 stm_pwl    *pwlmaxcdw = NULL ;
 stm_pwl    *pwlmincdw = NULL ;
 int         usedmax ;
 int         usedmin ;
 tas_driver  drivermaxup, drivermaxdn, driverminup, drivermindn ;
 tas_driver  drivermaxxup, drivermaxxdn, driverminxup, driverminxdn ;
 tas_driver  drivermaxcup, drivermaxcdn, drivermincup, drivermincdn ;
 lotrs_list *lotrs ;
 int         ongrid ;

 long        valmaxup_grid ;
 long        valfmaxup_grid ;
 long        valminup_grid ;
 long        valfminup_grid ;
 long        valmaxdown_grid ;
 long        valfmaxdown_grid ;
 long        valmindown_grid ;
 long        valfmindown_grid ;
 tas_driver  drivermaxxup_grid, drivermaxxdn_grid, driverminxup_grid, driverminxdn_grid ;
 
 if( V_BOOL_TAB[__STM_USE_MSC].VALUE ) {
   ptmbkpwlmax = &mbkpwlmax ;
   ptmbkpwlmin = &mbkpwlmin ;
 }
 else {
   ptmbkpwlmax = NULL ;
   ptmbkpwlmin = NULL ;
 }

 if (flagincone == 'Y') {
    if((incone->TYPE & CNS_EXT) == CNS_EXT)
        avt_log (LOGTAS, 2, "Computing RC delay from connector '%s' to cone %ld '%s'\n", incone->UEDGE.LOCON->NAME, 
                                                                                        cone->INDEX, 
                                                                                        cone->NAME);
    else
    if((incone->UEDGE.CONE->TYPE & CNS_EXT) == CNS_EXT)
        avt_log (LOGTAS, 2, "Computing RC delay from connector '%s' to cone %ld '%s'\n", incone->UEDGE.CONE->NAME, 
                                                                                        cone->INDEX, 
                                                                                        cone->NAME);
    else
        avt_log (LOGTAS, 2, "Computing RC delay from cone %ld '%s' to cone %ld '%s'\n", incone->UEDGE.CONE->INDEX,
                                                                                      incone->UEDGE.CONE->NAME, 
                                                                                      cone->INDEX, 
                                                                                      cone->NAME);
 }

 if(flagincone == 'Y')
     if((ptypeincone = getptype(incone->USER,TAS_RC_LOCON)) != NULL){
         locont = NULL ;
         flag = 'N' ;
         if((incone->TYPE & CNS_EXT) == CNS_EXT){
             if((incone->UEDGE.LOCON->DIRECTION == CNS_T)){
                 locont = incone->UEDGE.LOCON ;
                 if(locont->TYPE != EXTERNAL)
                     locont = NULL ;
             }
             if((ptype = getptype(incone->UEDGE.LOCON->USER,CNS_EXT)) != NULL){
                 if((ptype = getptype(((cone_list *)ptype->DATA)->USER,TAS_RC_CONE)) != NULL){
                     ptypecone = ptype ;
                 }
             }
             if(ptype == NULL){
                 if(incone->UEDGE.LOCON->TYPE == EXTERNAL)
                   ptypecone = addptype(NULL,TAS_RC_CONE, addchain(NULL,incone->UEDGE.LOCON)) ;
                 else
                   ptypecone = NULL ;
                 flag = 'Y' ;
             }
         }else{
             driver = incone->UEDGE.CONE ;
             if((incone->UEDGE.CONE->TYPE & CNS_EXT) == CNS_EXT){
                 chain_list *cl;
                 cl=cns_get_cone_external_connectors(incone->UEDGE.CONE);
    
//                 ptype = getptype(incone->UEDGE.CONE->USER,CNS_EXT) ;
                 if(cl!=NULL/*ptype != NULL*/){
                     if(((locon_list *)cl->DATA)->DIRECTION == CNS_T){
                         locont = (locon_list *)cl->DATA ;
                         if(locont->TYPE != EXTERNAL)
                             locont = NULL ;
                     }
                    freechain(cl);
                 }
             }
             ptypecone = getptype(incone->UEDGE.CONE->USER,TAS_RC_CONE) ;
         }
         if(ptypecone != NULL){
             if(nbfront == (long)0){
                 front = (front_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(front_list)) ;
                 HEAD_FRONT = addchain(HEAD_FRONT,(void*)front) ;
                 for(nbfront = 0 ; nbfront < TAS_BLOC_SIZE ; nbfront++){
                     (front + nbfront)->FUP = TAS_NOFRONT ;
                     (front + nbfront)->FDOWN = TAS_NOFRONT ;
                     (front + nbfront)->PWLUP = NULL ;
                     (front + nbfront)->PWLDN = NULL ;
                     (front + nbfront)->DRIVERUP.R = -1.0 ;
                     (front + nbfront)->DRIVERUP.V = -1.0 ;
                     (front + nbfront)->DRIVERDN.V = -1.0 ;
                     (front + nbfront)->DRIVERDN.V = -1.0 ;
                 }
             }
             nbfront -=2 ;
    
             incone->USER = addptype(incone->USER,TAS_SLOPE_MIN,front+nbfront);
             incone->USER = addptype(incone->USER,TAS_SLOPE_MAX,front+nbfront+1);
    
             delaymax = (delay_list *)getptype(incone->USER,TAS_DELAY_MAX)->DATA ;
             delaymin = (delay_list *)getptype(incone->USER,TAS_DELAY_MIN)->DATA ;
    
             if((incone->TYPE & CNS_EXT) != CNS_EXT){
                 frontconemax = (front_list *)getptype(incone->UEDGE.CONE->USER, TAS_SLOPE_MAX)->DATA ;
                 frontconemin = (front_list *)getptype(incone->UEDGE.CONE->USER, TAS_SLOPE_MIN)->DATA ;
             }else if((ptype = getptype(incone->UEDGE.LOCON->USER,CNS_EXT)) != NULL){
                 frontconemax = (front_list *)getptype(((cone_list *)ptype->DATA)->USER,TAS_SLOPE_MAX)->DATA ;
                 frontconemin = (front_list *)getptype(((cone_list *)ptype->DATA)->USER,TAS_SLOPE_MIN)->DATA ;
             }else{
                 frontconemax = (front_list *)getptype(incone->UEDGE.LOCON->USER,TAS_SLOPE_MAX)->DATA ;
                 frontconemin = (front_list *)getptype(incone->UEDGE.LOCON->USER,TAS_SLOPE_MIN)->DATA ;
             }
    
             frontinconemax = front+nbfront+1 ;
             frontinconemin = front+nbfront ;
    
             valmaxcup = TAS_NOTIME ;
             valmincup = TAS_NOTIME ;
             valmaxcdown = TAS_NOTIME ;
             valmincdown = TAS_NOTIME ;
             locontx = locont ;
             valmaxup = TAS_NOTIME ;
             valminup = TAS_NOTIME ;
             valmaxdown = TAS_NOTIME ;
             valmindown = TAS_NOTIME ;

             valmaxup_grid    = TAS_NOTIME ;
             valminup_grid    = TAS_NOTIME ;
             valmaxdown_grid  = TAS_NOTIME ;
             valmindown_grid  = TAS_NOTIME ;
             valfmaxup_grid   = TAS_NOFRONT ;
             valfminup_grid   = TAS_NOFRONT ;
             valfmaxdown_grid = TAS_NOFRONT ;
             valfmindown_grid = TAS_NOFRONT ;

             pwlmaxup = NULL ;
             pwlminup = NULL ;
             pwlmaxdw = NULL ;
             pwlmindw = NULL ;
             pwlmaxcup = NULL ;
             pwlmincup = NULL ;
             pwlmaxcdw = NULL ;
             pwlmincdw = NULL ;
             drivermaxxup.R = -1.0 ;
             drivermaxxup.V = -1.0 ;
             driverminxup.R = -1.0 ;
             driverminxup.V = -1.0 ;
             drivermaxcup.R = -1.0 ;
             drivermaxcup.V = -1.0 ;
             drivermincup.R = -1.0 ;
             drivermincup.V = -1.0 ;
             drivermaxxdn.R = -1.0 ;
             drivermaxxdn.V = -1.0 ;
             driverminxdn.R = -1.0 ;
             driverminxdn.V = -1.0 ;
             drivermaxcdn.R = -1.0 ;
             drivermaxcdn.V = -1.0 ;
             drivermincdn.R = -1.0 ;
             drivermincdn.V = -1.0 ;
             chaincone = (chain_list *)ptypecone->DATA ;
             chainc = tas_reducechaincon(chaincone) ;
             chaincone = chainc ;
             while(chaincone != NULL ){
                 if(locontx != NULL)
                     loconcone = locontx ;
                 else
                     loconcone = (locon_list *)chaincone->DATA ;
                 ptrcx = getrcx(loconcone->SIG) ;
                 chainin = tas_reducechaincon((chain_list *)ptypeincone->DATA) ;
                 if((ptrcx) && (loconcone->TYPE != 'C')){
                     for(chainincone = chainin ; chainincone != NULL ; chainincone = chainincone->NEXT){
                         loconincone = (locon_list *)chainincone->DATA ;
                         if(loconcone == loconincone)
                             continue ;
        
                         pwlmax_up_out   = NULL;
                         pwlmin_up_out   = NULL;
                         pwlmax_dw_out   = NULL;
                         pwlmin_dw_out   = NULL;
                         if(loconcone->SIG == loconincone->SIG){
                             ongrid = 0 ;
                             lotrs  = NULL ;
                             if( loconincone->TYPE == 'T' ) {
                               lotrs = (lotrs_list*)(loconincone->ROOT);
                               if( lotrs->GRID == loconincone )
                                 ongrid = 1 ;
                             }
                             
                             if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
                                 tas_get_output_carac( driver, 'm', 'U', &s_carac_min );
                                 tas_get_output_carac( driver, 'M', 'U', &s_carac_max );
                                 if((frontconemax->FUP != TAS_NOFRONT) && (frontconemin->FUP != TAS_NOFRONT)){
                                     fmaxup = stm_thr2scm (frontconemax->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                     fminup = stm_thr2scm (frontconemin->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_UP);
                                     slope.F0MAX  = fmaxup ;
                                     slope.FCCMAX = fmaxup ;
                                     slope.SENS   = TRC_SLOPE_UP;
                                     slope.CCA    = -1.0;
                                     slope.MIN.slope = fminup ;                slope.MAX.slope = fmaxup ;
                                     slope.MIN.vend  = s_carac_min.VF ;        slope.MAX.vend  = s_carac_max.VF ;
                                     slope.MIN.vt    = s_carac_min.VT ;        slope.MAX.vt    = s_carac_max.VT ;
                                     slope.MIN.vdd   = s_carac_min.VDD ;       slope.MAX.vdd   = s_carac_max.VDD ;
                                     slope.MIN.rlin  = s_carac_min.RLIN ;      slope.MAX.rlin  = s_carac_max.RLIN ;
                                     slope.MIN.vth   = s_carac_min.VTH ;       slope.MAX.vth   = s_carac_max.VTH ;
                                     slope.MIN.vsat  = s_carac_min.VSAT ;      slope.MAX.vsat  = s_carac_max.VSAT ;
                                     slope.MIN.r     = s_carac_min.R ;         slope.MAX.r     = s_carac_max.R ;
                                     slope.MIN.c1    = s_carac_min.C1 ;        slope.MAX.c1    = s_carac_max.C1 ;
                                     slope.MIN.c2    = s_carac_min.C2 ;        slope.MAX.c2    = s_carac_max.C2 ;
                                     slope.MIN.slnrm = frontconemin->FUP/TTV_UNIT ;
                                     slope.MAX.slnrm = frontconemax->FUP/TTV_UNIT ;
                                     slope.MIN.pwl   = stm_pwl_to_mbk_pwl( frontconemin->PWLUP, s_carac_min.VT, s_carac_min.VDD );
                                     slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLUP, s_carac_max.VT, s_carac_max.VDD );
                                     rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                        &dmaxup, &dminup, &fmaxup, &fminup, ptmbkpwlmax, ptmbkpwlmin );
                                     mbk_pwl_free_pwl( slope.MIN.pwl );
                                     mbk_pwl_free_pwl( slope.MAX.pwl );
                                     if( mbkpwlmax ) {
                                       pwlmax_up_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                       mbk_pwl_free_pwl( mbkpwlmax );
                                     }
                                     if( mbkpwlmin ) {
                                       pwlmin_up_out = mbk_pwl_to_stm_pwl( mbkpwlmin );
                                       mbk_pwl_free_pwl( mbkpwlmin );
                                     }
                                     fmaxup = stm_scm2thr (fmaxup, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                     fminup = stm_scm2thr (fminup, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_UP);
                                     dmaxup = TTV_UNIT * dmaxup ;
                                     dminup = TTV_UNIT * dminup ;
                                     fmaxup = TTV_UNIT * fmaxup ;
                                     fminup = TTV_UNIT * fminup ;
                                 }
                                 tas_get_output_carac( driver, 'm', 'D', &s_carac_min );
                                 tas_get_output_carac( driver, 'M', 'D', &s_carac_max );
                                 if((frontconemax->FDOWN != TAS_NOFRONT) && (frontconemin->FDOWN != TAS_NOFRONT)){
                                     fmaxdown = stm_thr2scm (frontconemax->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                     fmindown = stm_thr2scm (frontconemin->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_DN);
                                     slope.F0MAX  = fmaxdown;
                                     slope.FCCMAX = fmaxdown;
                                     slope.SENS   = TRC_SLOPE_DOWN;
                                     slope.CCA    = -1.0;
                                     slope.MIN.slope = fmindown ;              slope.MAX.slope = fmaxdown ;
                                     slope.MIN.vend  = s_carac_min.VF ;        slope.MAX.vend  = s_carac_max.VF ;
                                     slope.MIN.vt    = s_carac_min.VT ;        slope.MAX.vt    = s_carac_max.VT ;
                                     slope.MIN.vdd   = s_carac_min.VDD ;       slope.MAX.vdd   = s_carac_max.VDD ;
                                     slope.MIN.rlin  = s_carac_min.RLIN ;      slope.MAX.rlin  = s_carac_max.RLIN ;
                                     slope.MIN.vth   = s_carac_min.VTH ;       slope.MAX.vth   = s_carac_max.VTH ;
                                     slope.MIN.vsat  = s_carac_min.VSAT ;      slope.MAX.vsat  = s_carac_max.VSAT ;
                                     slope.MIN.r     = s_carac_min.R ;         slope.MAX.r     = s_carac_max.R ;
                                     slope.MIN.c1    = s_carac_min.C1 ;        slope.MAX.c1    = s_carac_max.C1 ;
                                     slope.MIN.c2    = s_carac_min.C2 ;        slope.MAX.c2    = s_carac_max.C2 ;
                                     slope.MIN.slnrm = frontconemin->FDOWN/TTV_UNIT ;
                                     slope.MAX.slnrm = frontconemax->FDOWN/TTV_UNIT ;
                                     slope.MIN.pwl   = stm_pwl_to_mbk_pwl( frontconemin->PWLDN, s_carac_min.VT, s_carac_min.VDD );
                                     slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLDN, s_carac_max.VT, s_carac_max.VDD );
                                     pwlmax_dw_out   = NULL;
                                     pwlmin_dw_out   = NULL;
                                     rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                        &dmaxdown, &dmindown, &fmaxdown, &fmindown, ptmbkpwlmax, ptmbkpwlmin );
                                     mbk_pwl_free_pwl( slope.MIN.pwl );
                                     mbk_pwl_free_pwl( slope.MAX.pwl );
                                     if( mbkpwlmax ) {
                                       pwlmax_dw_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                       mbk_pwl_free_pwl( mbkpwlmax );
                                     }
                                     if( mbkpwlmin ) {
                                       pwlmin_dw_out = mbk_pwl_to_stm_pwl( mbkpwlmin );
                                       mbk_pwl_free_pwl( mbkpwlmin );
                                     }
                                     fmaxdown = stm_scm2thr (fmaxdown, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                     fmindown = stm_scm2thr (fmindown, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_DN);
                                     dmaxdown = TTV_UNIT * dmaxdown ;
                                     dmindown = TTV_UNIT * dmindown ;
                                     fmaxdown = TTV_UNIT * fmaxdown ;
                                     fmindown = TTV_UNIT * fmindown ;
                                 }
                             }else{
                                 tas_get_output_carac( driver, 'M', 'U', &s_carac_max );
                                 if(frontconemax->FUP != TAS_NOFRONT){
                                     fmaxup = stm_thr2scm (frontconemax->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                     slope.F0MAX  = fmaxup;
                                     slope.FCCMAX = fmaxup;
                                     slope.SENS   = TRC_SLOPE_UP;
                                     slope.CCA    = -1.0;
                                     slope.MIN.slope = -1.0 ;                   slope.MAX.slope = fmaxup ;
                                     slope.MIN.vend  = -1.0 ;                   slope.MAX.vend  = s_carac_max.VF ;
                                     slope.MIN.vt    = -1.0 ;                   slope.MAX.vt    = s_carac_max.VT ;
                                     slope.MIN.vdd   = -1.0 ;                   slope.MAX.vdd   = s_carac_max.VDD ;
                                     slope.MIN.rlin  = -1.0 ;                   slope.MAX.rlin  = s_carac_max.RLIN ;
                                     slope.MIN.vth   = -1.0 ;                   slope.MAX.vth   = s_carac_max.VTH ;
                                     slope.MIN.vsat  = -1.0 ;                   slope.MAX.vsat  = s_carac_max.VSAT ;
                                     slope.MIN.r     = -1.0 ;                   slope.MAX.r     = s_carac_max.R ;
                                     slope.MIN.c1    = -1.0 ;                   slope.MAX.c1    = s_carac_max.C1 ;
                                     slope.MIN.c2    = -1.0 ;                   slope.MAX.c2    = s_carac_max.C2 ;
                                     slope.MIN.slnrm = -1.0 ;
                                     slope.MAX.slnrm = frontconemax->FUP/TTV_UNIT ;
                                     slope.MIN.pwl   = NULL ;
                                     slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLUP, s_carac_max.VT, s_carac_max.VDD  );
                                     rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                        &dmaxup, NULL, &fmaxup, NULL, ptmbkpwlmax, NULL );
                                     mbk_pwl_free_pwl( slope.MAX.pwl );
                                     if( mbkpwlmax ) {
                                       pwlmax_up_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                       mbk_pwl_free_pwl( mbkpwlmax );
                                     }
                                     fmaxup = stm_scm2thr (fmaxup, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                     dmaxup = TTV_UNIT * dmaxup ;
                                     fmaxup = TTV_UNIT * fmaxup ;
                                 }
                                 tas_get_output_carac( driver, 'M', 'D', &s_carac_max );
                                 if(frontconemax->FDOWN != TAS_NOFRONT){
                                     fmaxdown = stm_thr2scm (frontconemax->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                     slope.F0MAX  = fmaxdown;
                                     slope.FCCMAX = fmaxdown;
                                     slope.SENS   = TRC_SLOPE_DOWN;
                                     slope.CCA    = -1.0;
                                     slope.MIN.slope = -1.0 ;                   slope.MAX.slope = fmaxdown ;
                                     slope.MIN.vend  = -1.0 ;                   slope.MAX.vend  = s_carac_max.VF ;
                                     slope.MIN.vt    = -1.0 ;                   slope.MAX.vt    = s_carac_max.VT ;
                                     slope.MIN.vdd   = -1.0 ;                   slope.MAX.vdd   = s_carac_max.VDD ;
                                     slope.MIN.rlin  = -1.0 ;                   slope.MAX.rlin  = s_carac_max.RLIN ;
                                     slope.MIN.vth   = -1.0 ;                   slope.MAX.vth   = s_carac_max.VTH ;
                                     slope.MIN.vsat  = -1.0 ;                   slope.MAX.vsat  = s_carac_max.VSAT ;
                                     slope.MIN.r     = -1.0 ;                   slope.MAX.r     = s_carac_max.R ;
                                     slope.MIN.c1    = -1.0 ;                   slope.MAX.c1    = s_carac_max.C1 ;
                                     slope.MIN.c2    = -1.0 ;                   slope.MAX.c2    = s_carac_max.C2 ;
                                     slope.MIN.slnrm = -1.0 ;
                                     slope.MAX.slnrm = frontconemax->FDOWN/TTV_UNIT ;
                                     slope.MIN.pwl   = NULL ;
                                     slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLDN, s_carac_max.VT, s_carac_max.VDD  );
                                     rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                        &dmaxdown, NULL, &fmaxdown, NULL, ptmbkpwlmax, NULL );
                                     mbk_pwl_free_pwl( slope.MAX.pwl );
                                     if( mbkpwlmax ) {
                                       pwlmax_dw_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                       mbk_pwl_free_pwl( mbkpwlmax );
                                     }
                                     fmaxdown = stm_scm2thr (fmaxdown, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                     dmaxdown = TTV_UNIT * dmaxdown ;
                                     fmaxdown = TTV_UNIT * fmaxdown ;
                                 }
                             }
                         }

                         usedmax = 0 ;
                         usedmin = 0 ;
                         
                         drivermaxup = frontconemax->DRIVERUP ;
                         driverminup = frontconemin->DRIVERUP ;

                         if(dmaxup >= 0.0){
                             valmaxxup = (long)(dmaxup + 0.5) ;
                             valfmaxxup = (long)(fmaxup + 0.5) ;
                             if((TAS_CONTEXT->TAS_FIND_MIN == 'Y') && (dminup >= 0.0)){
                                 valminxup = (long)(dminup + 0.5) ;
                                 valfminxup = (long)(fminup + 0.5) ; 
                                 if(valmaxxup < valminxup){
                                     long delay;
                                     delay = valminxup ;
                                     valminxup = valmaxxup ;
                                     valmaxxup = delay ;
                                 }
                                 if(valfmaxxup < valfminxup){
                                     long delay;
                                     tas_driver driver;
                                     stm_pwl *ptpwl ;
                                     delay = valfminxup ;
                                     valfminxup = valfmaxxup ;
                                     valfmaxxup = delay ;
                                     ptpwl = pwlmax_up_out ;
                                     pwlmax_up_out = pwlmin_up_out ;
                                     pwlmin_up_out = ptpwl ;
                                     driver = driverminup ;
                                     driverminup = drivermaxup ;
                                     drivermaxup = driver ;
                                 }
                             }else valminxup = TAS_NOTIME ;
                         }else{
                             valmaxxup = TAS_NOTIME ;
                             valminxup = TAS_NOTIME ;
                             tas_error(58,yagGetName(ifl, loconcone->SIG),TAS_WARNING) ;
                         }
                         if(locontx == NULL){
                             if(((valmaxup < valmaxxup) || (valmaxup == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                                 valmaxup = valmaxxup ;
                                 valfmaxup = valfmaxxup ;
                                 drivermaxxup = drivermaxup ;
                                 if( pwlmaxup ) stm_pwl_destroy( pwlmaxup );
                                 pwlmaxup = pwlmax_up_out ;
                                 usedmax = 1;
                             }
                             if(((valmaxup == valmaxxup) || (valmaxup == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                                 if(valfmaxup < valfmaxxup){
                                     valmaxup = valmaxxup ;
                                     valfmaxup = valfmaxxup ;
                                     drivermaxxup = drivermaxup ;
                                     if( pwlmaxup ) stm_pwl_destroy( pwlmaxup );
                                     pwlmaxup = pwlmax_up_out ;
                                     usedmax = 1;
                                 }
                             }
                             if(((valminup > valminxup) || (valminup == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                                 valminup = valminxup ;
                                 valfminup = valfminxup ;
                                 driverminxup = driverminup ;
                                 if( pwlminup ) stm_pwl_destroy( pwlminup );
                                 pwlminup = pwlmin_up_out ;
                                 usedmin = 1;
                             }
                             if(((valminup == valminxup) || (valminup == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                                 if(valfminup > valfminxup){
                                     valminup = valminxup ;
                                     valfminup = valfminxup ;
                                     driverminxup = driverminup ;
                                     if( pwlminup ) stm_pwl_destroy( pwlminup );
                                     pwlminup = pwlmin_up_out ;
                                     usedmin = 1;
                                 }
                             }
                             if( ongrid && MLO_IS_TRANSN( lotrs->TYPE ) ) {
                               if(((valmaxup_grid < valmaxxup) || (valmaxup_grid == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                                   valmaxup_grid = valmaxxup ;
                                   valfmaxup_grid = valfmaxxup ;
                                   drivermaxxup_grid = drivermaxup ;
                               }
                               if(((valmaxup_grid == valmaxxup) || (valmaxup_grid == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                                   if(valfmaxup_grid < valfmaxxup){
                                       valmaxup_grid = valmaxxup ;
                                       valfmaxup_grid = valfmaxxup ;
                                       drivermaxxup_grid = drivermaxup ;
                                   }
                               }
                               if(((valminup_grid > valminxup) || (valminup_grid == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                                   valminup_grid = valminxup ;
                                   valfminup_grid = valfminxup ;
                                   driverminxup_grid = driverminup ;
                               }
                               if(((valminup_grid == valminxup) || (valminup_grid == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                                   if(valfminup_grid > valfminxup){
                                       valminup_grid = valminxup ;
                                       valfminup_grid = valfminxup ;
                                       driverminxup_grid = driverminup ;
                                   }
                               }
                             }
                         }else{
                             if(((valmaxcup < valmaxxup) || (valmaxcup == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                                 valmaxcup = valmaxxup ;
                                 valfmaxcup = valfmaxxup ;
                                 drivermaxcup = drivermaxup ;
                                 if( pwlmaxcup ) stm_pwl_destroy( pwlmaxcup );
                                 pwlmaxcup = pwlmax_up_out;
                                 usedmax = 1;
                             }
                             if(((valmaxcup == valmaxxup) || (valmaxcup == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                                 if(valfmaxcup < valfmaxxup){
                                     valmaxcup = valmaxxup ;
                                     valfmaxcup = valfmaxxup ;
                                     drivermaxcup = drivermaxup ;
                                     if( pwlmaxcup ) stm_pwl_destroy( pwlmaxcup );
                                     pwlmaxcup = pwlmax_up_out;
                                     usedmax = 1;
                                 }
                             }
                             if(((valmincup > valminxup) || (valmincup == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                                 valmincup = valminxup ;
                                 valfmincup = valfminxup ;
                                 drivermincup = driverminup ;
                                 if( pwlmincup ) stm_pwl_destroy( pwlmincup );
                                 pwlmincup = pwlmin_up_out;
                                 usedmin = 1;
                             }
                             if(((valmincup == valminxup) || (valmincup == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                                 if(valfmincup > valfminxup){
                                     valmincup = valminxup ;
                                     valfmincup = valfminxup ;
                                     drivermincup = driverminup ;
                                     if( pwlmincup ) stm_pwl_destroy( pwlmincup );
                                     pwlmincup = pwlmin_up_out;
                                     usedmin = 1;
                                 }
                             }
                         }

                         if( !usedmax ) stm_pwl_destroy( pwlmax_up_out );
                         if( !usedmin ) stm_pwl_destroy( pwlmin_up_out );
          
                         usedmax = 0 ;
                         usedmin = 0 ;

                         drivermaxdn = frontconemax->DRIVERDN ;
                         drivermindn = frontconemin->DRIVERDN ;

                         if(dmaxdown >= 0.0){
                             valmaxxdown = (long)(dmaxdown + 0.5) ;
                             valfmaxxdown = (long)(fmaxdown + 0.5) ;
                             if((TAS_CONTEXT->TAS_FIND_MIN == 'Y') && (dmindown >= 0.0)){
                                 valminxdown = (long)(dmindown + 0.5) ;
                                 valfminxdown = (long)(fmindown + 0.5) ;
                                 if(valmaxxdown < valminxdown){
                                     long delay;
                                     delay = valminxdown ;
                                     valminxdown = valmaxxdown ;
                                     valmaxxdown = delay ;
                                 }
                                 if(valfmaxxdown < valfminxdown){
                                     long delay;
                                     tas_driver driver ;
                                     stm_pwl *ptpwl ;
                                     delay = valfminxdown ;
                                     valfminxdown = valfmaxxdown ;
                                     valfmaxxdown = delay ;
                                     ptpwl = pwlmax_dw_out ;
                                     pwlmax_dw_out = pwlmin_dw_out ;
                                     pwlmin_dw_out = ptpwl ;
                                     driver = drivermindn ;
                                     drivermindn = drivermaxdn ;
                                     drivermaxdn = driver ;
                                 }
                             }else valminxdown = TAS_NOTIME ;
                         }else{
                             valmaxxdown = TAS_NOTIME ;
                             valminxdown = TAS_NOTIME ;
                             tas_error(58,yagGetName(ifl,loconcone->SIG),TAS_WARNING) ;
                         }
                         if(locontx == NULL){
                             if(((valmaxdown < valmaxxdown) || (valmaxdown == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                                 valmaxdown = valmaxxdown ;
                                 valfmaxdown = valfmaxxdown ;
                                 drivermaxxdn = drivermaxdn ;
                                 if( pwlmaxdw ) stm_pwl_destroy( pwlmaxdw );
                                 pwlmaxdw = pwlmax_dw_out ;
                                 usedmax = 1;
                             }
                             if(((valmaxdown == valmaxxdown) || (valmaxdown == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                                 if(valfmaxdown < valfmaxxdown){
                                     valmaxdown = valmaxxdown ;
                                     valfmaxdown = valfmaxxdown ;
                                     drivermaxxdn = drivermaxdn ;
                                     if( pwlmaxdw ) stm_pwl_destroy( pwlmaxdw );
                                     pwlmaxdw = pwlmax_dw_out ;
                                     usedmax = 1;
                                 }
                             }
                             if(((valmindown > valminxdown) || (valmindown == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                                 valmindown = valminxdown ;
                                 valfmindown = valfminxdown ;
                                 driverminxdn = drivermindn ;
                                 if( pwlmindw ) stm_pwl_destroy( pwlmindw );
                                 pwlmindw = pwlmin_dw_out ;
                                 usedmin = 1;
                             }
                             if(((valmindown == valminxdown) || (valmindown == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                                 if(valfmindown > valfminxdown){
                                     valmindown = valminxdown ;
                                     valfmindown = valfminxdown ;
                                     driverminxdn = drivermindn ;
                                     if( pwlmindw ) stm_pwl_destroy( pwlmindw );
                                     pwlmindw = pwlmin_dw_out ;
                                     usedmin = 1;
                                 }
                             }
                             if( ongrid && MLO_IS_TRANSP( lotrs->TYPE ) ) {
                               if(((valmaxdown_grid < valmaxxdown) || (valmaxdown_grid == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                                   valmaxdown_grid = valmaxxdown ;
                                   valfmaxdown_grid = valfmaxxdown ;
                                   drivermaxxdn_grid = drivermaxdn ;
                               }
                               if(((valmaxdown_grid == valmaxxdown) || (valmaxdown_grid == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                                   if(valfmaxdown_grid < valfmaxxdown){
                                       valmaxdown_grid = valmaxxdown ;
                                       valfmaxdown_grid = valfmaxxdown ;
                                       drivermaxxdn_grid = drivermaxdn ;
                                   }
                               }
                               if(((valmindown_grid > valminxdown) || (valmindown_grid == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                                   valmindown_grid = valminxdown ;
                                   valfmindown_grid = valfminxdown ;
                                   driverminxdn_grid = drivermindn ;
                               }
                               if(((valmindown_grid == valminxdown) || (valmindown_grid == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                                   if(valfmindown_grid > valfminxdown){
                                       valmindown_grid = valminxdown ;
                                       valfmindown_grid = valfminxdown ;
                                       driverminxdn_grid = drivermindn ;
                                   }
                               }
                             }
                         }else{
                             if(((valmaxcdown < valmaxxdown) || (valmaxcdown == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                                 valmaxcdown = valmaxxdown ;
                                 valfmaxcdown = valfmaxxdown ;
                                 drivermaxcdn = drivermaxdn ;
                                 if( pwlmaxcdw ) stm_pwl_destroy( pwlmaxcdw );
                                 pwlmaxcdw = pwlmax_dw_out ;
                                 usedmax = 1;
                             }
                             if(((valmaxcdown == valmaxxdown) || (valmaxcdown == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                                 if(valfmaxcdown < valfmaxxdown){
                                     valmaxcdown = valmaxxdown ;
                                     valfmaxcdown = valfmaxxdown ;
                                     drivermaxcdn = drivermaxdn ;
                                     if( pwlmaxcdw ) stm_pwl_destroy( pwlmaxcdw );
                                     pwlmaxcdw = pwlmax_dw_out ;
                                     usedmax = 1;
                                 }
                             }
                             if(((valmincdown > valminxdown) || (valmincdown == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                                 valmincdown = valminxdown ;
                                 valfmincdown = valfminxdown ;
                                 drivermincdn = drivermindn ;
                                 if( pwlmincdw ) stm_pwl_destroy( pwlmincdw );
                                 pwlmincdw = pwlmin_dw_out ;
                                 usedmin = 1;
                             }
                             if(((valmincdown == valminxdown) || (valmincdown == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                                 if(valfmincdown > valfminxdown){
                                     valmincdown = valminxdown ;
                                     valfmincdown = valfminxdown ;
                                     drivermincdn = drivermindn ;
                                     if( pwlmincdw ) stm_pwl_destroy( pwlmincdw );
                                     pwlmincdw = pwlmin_dw_out ;
                                     usedmin = 1;
                                 }
                             }
                         }

                         if( !usedmax ) stm_pwl_destroy( pwlmax_dw_out );
                         if( !usedmin ) stm_pwl_destroy( pwlmin_dw_out );
              
                     }
                 }
                 freechain(chainin) ;
                 if(locontx == NULL)
                 chaincone = chaincone->NEXT ;
                 else
                 locontx = NULL ;
             }
             freechain(chainc) ;
  
             if( V_BOOL_TAB[ __TAS_PROPAGATE_RC_ACTIVE ].VALUE ) {
               if( valmaxup != TAS_NOTIME && valmaxup_grid != TAS_NOTIME ) {
                 valmaxup = valmaxup_grid ;
                 drivermaxxup = drivermaxxup_grid ;
               }
               if( valfmaxup != TAS_NOFRONT && valfmaxup_grid != TAS_NOFRONT )
                 valfmaxup = valfmaxup_grid ;
               if( valmaxdown != TAS_NOTIME && valmaxdown_grid != TAS_NOTIME ) {
                 valmaxdown = valmaxdown_grid ;
                 drivermaxxdn = drivermaxxdn_grid ;
               }
               if( valfmaxdown != TAS_NOFRONT && valfmaxdown_grid != TAS_NOFRONT )
                 valfmaxdown = valfmaxdown_grid ;
               if( valminup != TAS_NOTIME && valminup_grid != TAS_NOTIME ) {
                 valminup = valminup_grid ;
                 driverminxup = driverminxup_grid ;
               }
               if( valfminup != TAS_NOFRONT && valfminup_grid != TAS_NOFRONT )
                 valfminup = valfminup_grid ;
               if( valmindown != TAS_NOTIME && valmindown_grid != TAS_NOTIME ) {
                 valmindown = valmindown_grid ;
                 driverminxdn = driverminxdn_grid ;
               }
               if( valfmindown != TAS_NOFRONT && valfmindown_grid != TAS_NOFRONT )
                 valfmindown = valfmindown_grid ;
             }
               
             if((valmaxup != TAS_NOTIME) && (valmaxdown != TAS_NOTIME)){
                 if(frontconemax->FUP != TAS_NOFRONT){
                     delaymax->RCHH = valmaxup ;
                     delaymax->FRCHH = valfmaxup ;
#ifdef USEOLDTEMP             
                     stm_pwl_destroy(delaymax->PWLRCHH);
                     delaymax->PWLRCHH = pwlmaxup ;
#endif
                     frontinconemax->FUP = valfmaxup ;
                     frontinconemax->PWLUP = pwlmaxup ;
                     frontinconemax->DRIVERUP = drivermaxxup ;
                 }else if(loconincone->DIRECTION == CNS_T){
                     frontinconemax->FUP = tas_get_pinslew(loconincone, 'U') ;
                 }
                 if(frontconemax->FDOWN != TAS_NOFRONT){
                     delaymax->RCLL = valmaxdown ;
                     delaymax->FRCLL = valfmaxdown ;
#ifdef USEOLDTEMP             
                     stm_pwl_destroy( delaymax->PWLRCLL );
                     delaymax->PWLRCLL = pwlmaxdw ;
#endif
                     frontinconemax->FDOWN = valfmaxdown ;
                     frontinconemax->PWLDN = pwlmaxdw ;
                     frontinconemax->DRIVERDN = drivermaxxdn ;
                 }else if(loconincone->DIRECTION == CNS_T){
                     frontinconemax->FDOWN = tas_get_pinslew(loconincone, 'D') ;
                 }
             }
             if((TAS_CONTEXT->TAS_FIND_MIN == 'Y') && (valminup != TAS_NOTIME) && (valmindown != TAS_NOTIME)){
                 if(frontconemin->FUP != TAS_NOFRONT){
                     delaymin->RCHH = valminup ;
                     delaymin->FRCHH = valfminup ;
#ifdef USEOLDTEMP             
                     stm_pwl_destroy( delaymin->PWLRCHH );
                     delaymin->PWLRCHH = pwlminup ;
#endif
                     frontinconemin->FUP = valfminup ;
                     frontinconemin->PWLUP = pwlminup ;
                     frontinconemin->DRIVERUP = driverminxup ;

                 }else if(loconincone->DIRECTION == CNS_T){
                     frontinconemin->FUP = tas_get_pinslew(loconincone, 'U') ;
                 }
                 if(frontconemin->FDOWN != TAS_NOFRONT){
                     delaymin->RCLL = valmindown ;
                     delaymin->FRCLL = valfmindown ;
#ifdef USEOLDTEMP             
                     stm_pwl_destroy( delaymin->PWLRCLL );
                     delaymin->PWLRCLL = pwlmindw ;
#endif
                     frontinconemin->FDOWN = valfmindown ;
                     frontinconemin->PWLDN = pwlmindw ;
                     frontinconemin->DRIVERDN = driverminxdn ;
                 }else if(loconincone->DIRECTION == CNS_T){
                     frontinconemin->FDOWN = tas_get_pinslew(loconincone, 'D') ;
                 }
             }
      
             if(locont != NULL){
                 if(nbdelay == (long)0){
                     delay = (delay_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(delay_list)) ;
                     HEAD_DELAY = addchain(HEAD_DELAY,(void*)delay) ;
                     for(nbdelay = 0 ; nbdelay < TAS_BLOC_SIZE ; nbdelay++){
                         (delay + nbdelay)->TPLH = TAS_NOTIME ;
                         (delay + nbdelay)->TPHL = TAS_NOTIME ;
                         (delay + nbdelay)->TPHH = TAS_NOTIME ;
                         (delay + nbdelay)->TPLL = TAS_NOTIME ;
                         (delay + nbdelay)->FLH = TAS_NOFRONT ;
                         (delay + nbdelay)->FHL = TAS_NOFRONT ;
                         (delay + nbdelay)->FHH = TAS_NOFRONT ;
                         (delay + nbdelay)->FLL = TAS_NOFRONT ;
#ifdef USEOLDTEMP                         
#ifdef USEOLDTEMP
                         (delay + nbdelay)->RLH = TAS_NORES ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->RHL = TAS_NORES ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->RHH = TAS_NORES ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->RLL = TAS_NORES ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->SLH = TAS_NOS ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->SHL = TAS_NOS ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->SHH = TAS_NOS ;
#endif
#ifdef USEOLDTEMP
                         (delay + nbdelay)->SLL = TAS_NOS ;
#endif
                         (delay + nbdelay)->RCHH = TAS_NOTIME ;
                         (delay + nbdelay)->RCLL = TAS_NOTIME ;
                         (delay + nbdelay)->FRCHH = TAS_NOFRONT ;
                         (delay + nbdelay)->FRCLL = TAS_NOFRONT ;
#endif
#ifdef USEOLDTEMP             
                         (delay + nbdelay)->PWLTPLH = NULL ;
                         (delay + nbdelay)->PWLTPHL = NULL ;
                         (delay + nbdelay)->PWLRCLL = NULL ;
                         (delay + nbdelay)->PWLRCHH = NULL ;
#endif
                         (delay + nbdelay)->TMLH = NULL ;
                         (delay + nbdelay)->TMHL = NULL ;
                         (delay + nbdelay)->TMHH = NULL ;
                         (delay + nbdelay)->TMLL = NULL ;
                         (delay + nbdelay)->FMLH = NULL ;
                         (delay + nbdelay)->FMHL = NULL ;
                         (delay + nbdelay)->FMHH = NULL ;
                         (delay + nbdelay)->FMLL = NULL ;
                         (delay + nbdelay)->CARAC = NULL ;
                     }
                 }
                 nbdelay -=2 ;
                 incone->USER = addptype(incone->USER,TAS_DELAY_MINT,delay + nbdelay);
                 if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
#ifdef USEOLDTEMP             
                     stm_pwl_destroy( (delay + nbdelay)->PWLRCHH );
                     stm_pwl_destroy( (delay + nbdelay)->PWLRCLL );
#endif
                     (delay + nbdelay)->RCHH = valmincup ; 
                     (delay + nbdelay)->RCLL = valmincdown ; 
                     (delay + nbdelay)->FRCHH = valfmincup ; 
                     (delay + nbdelay)->FRCLL = valfmincdown ; 
#ifdef USEOLDTEMP             
                     (delay + nbdelay)->PWLRCHH = pwlmincup ;
                     (delay + nbdelay)->PWLRCLL = pwlmincdw ;
#endif
                 }
                 incone->USER = addptype(incone->USER,TAS_DELAY_MAXT,delay + nbdelay + 1);
#ifdef USEOLDTEMP             
                 stm_pwl_destroy( (delay + nbdelay + 1)->PWLRCHH );
                 stm_pwl_destroy( (delay + nbdelay + 1)->PWLRCLL );
#endif
                 (delay + nbdelay + 1)->RCHH = valmaxcup ;
                 (delay + nbdelay + 1)->RCLL = valmaxcdown ;
                 (delay + nbdelay + 1)->FRCHH = valfmaxcup ;
                 (delay + nbdelay + 1)->FRCLL = valfmaxcdown ;
#ifdef USEOLDTEMP             
                 (delay + nbdelay + 1)->PWLRCHH = pwlmaxcup ;
                 (delay + nbdelay + 1)->PWLRCLL = pwlmaxcdw ;
#endif
             }

             if (valmaxup != TAS_NOTIME)
                avt_log (LOGTAS, 3, "                TPHH MAX = %ld\n", valmaxup);
             if (valmaxdown != TAS_NOTIME)
                avt_log (LOGTAS, 3, "                TPLL MAX = %ld\n", valmaxdown);
             if (valminup != TAS_NOTIME)
                avt_log (LOGTAS, 3, "                TPHH MIN = %ld\n", valminup);
             if (valmindown != TAS_NOTIME)
                avt_log (LOGTAS, 3, "                TPLL MIN = %ld\n", valmindown);
         }
         if(flag == 'Y'){
             freechain((chain_list *)ptypecone->DATA) ;
             freeptype(ptypecone) ;
         }
     }

 if(flagcone == 'Y') 
     if((cone->TYPE & CNS_EXT) == CNS_EXT){
         chain_list *cl;
         frontconemax = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA ;
         frontconemin = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA ;

         
         cl=cns_get_cone_external_connectors(cone);
         while (cl!=NULL)
         {
           //loconincone = (locon_list *)getptype(cone->USER,CNS_EXT)->DATA ;
           loconincone = (locon_list *)cl->DATA;
           if((loconincone->DIRECTION == CNS_O) || 
             (loconincone->DIRECTION == CNS_B) ||
             (loconincone->DIRECTION == CNS_T) || 
             (loconincone->DIRECTION == CNS_Z)){
               if((ptypecone = getptype(cone->USER,TAS_RC_CONE)) != NULL){
                   delaymax = (delay_list *)getptype(loconincone->USER,TAS_DELAY_MAX)->DATA ;
                   delaymin = (delay_list *)getptype(loconincone->USER,TAS_DELAY_MIN)->DATA ;
                   valmaxup = TAS_NOTIME ;
                   valminup = TAS_NOTIME ;
                   valmaxdown = TAS_NOTIME ;
                   valmindown = TAS_NOTIME ;
                   drivermaxxup.R = -1.0 ;
                   drivermaxxup.V = -1.0 ;
                   driverminxup.R = -1.0 ;
                   driverminxup.V = -1.0 ;
                   drivermaxxdn.R = -1.0 ;
                   drivermaxxdn.V = -1.0 ;
                   driverminxdn.R = -1.0 ;
                   driverminxdn.V = -1.0 ;
                   pwlmaxup = NULL ;
                   pwlminup = NULL ;
                   pwlmaxdw = NULL ;
                   pwlmindw = NULL ;
                   chaincone = (chain_list *)ptypecone->DATA ;
                   chainc = tas_reducechaincon(chaincone) ;
                   chaincone = chainc ;
                   for(chaincone = chainc ; chaincone != NULL ; chaincone = chaincone->NEXT){
                       loconcone = (locon_list *)chaincone->DATA ;
                       if(loconcone == loconincone)
                           continue ;
                       ptrcx = getrcx(loconcone->SIG) ;
                       if(ptrcx){
                           pwlmax_up_out   = NULL;
                           pwlmin_up_out   = NULL;
                           pwlmax_dw_out   = NULL;
                           pwlmin_dw_out   = NULL;
                           if(loconcone->SIG == loconincone->SIG){
                               if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
                                   tas_get_output_carac( driver, 'm', 'U', &s_carac_min );
                                   tas_get_output_carac( driver, 'M', 'U', &s_carac_max );
                                   if((frontconemax->FUP != TAS_NOFRONT) && (frontconemin->FUP != TAS_NOFRONT)){
                                       fmaxup = stm_thr2scm (frontconemax->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                       fminup = stm_thr2scm (frontconemin->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_UP);
                                       slope.F0MAX  = fmaxup;
                                       slope.FCCMAX = fmaxup;
                                       slope.SENS   = TRC_SLOPE_UP;
                                       slope.CCA    = -1.0;
                                       slope.MIN.slope = fminup ;                slope.MAX.slope = fmaxup ;
                                       slope.MIN.vend  = s_carac_min.VF ;        slope.MAX.vend  = s_carac_max.VF ;
                                       slope.MIN.vt    = s_carac_min.VT ;        slope.MAX.vt    = s_carac_max.VT ;
                                       slope.MIN.vdd   = s_carac_min.VDD ;       slope.MAX.vdd   = s_carac_max.VDD ;
                                       slope.MIN.rlin  = s_carac_min.RLIN ;      slope.MAX.rlin  = s_carac_max.RLIN ;
                                       slope.MIN.vth   = s_carac_min.VTH ;       slope.MAX.vth   = s_carac_max.VTH ;
                                       slope.MIN.vsat  = s_carac_min.VSAT ;      slope.MAX.vsat  = s_carac_max.VSAT ;
                                       slope.MIN.r     = s_carac_min.R ;         slope.MAX.r     = s_carac_max.R ;
                                       slope.MIN.c1    = s_carac_min.C1 ;        slope.MAX.c1    = s_carac_max.C1 ;
                                       slope.MIN.c2    = s_carac_min.C2 ;        slope.MAX.c2    = s_carac_max.C2 ;
                                       slope.MIN.slnrm = frontconemin->FUP/TTV_UNIT ;
                                       slope.MAX.slnrm = frontconemax->FUP/TTV_UNIT ;
                                       slope.MIN.pwl   = stm_pwl_to_mbk_pwl( frontconemin->PWLUP, s_carac_min.VT, s_carac_min.VDD );
                                       slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLUP, s_carac_max.VT, s_carac_max.VDD );
                                       rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                          &dmaxup, &dminup, &fmaxup, &fminup, ptmbkpwlmax, ptmbkpwlmin );
                                       mbk_pwl_free_pwl( slope.MIN.pwl );
                                       mbk_pwl_free_pwl( slope.MAX.pwl );
                                       if( mbkpwlmax ) {
                                         pwlmax_up_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                         mbk_pwl_free_pwl( mbkpwlmax );
                                       }
                                       if( mbkpwlmin ) {
                                         pwlmin_up_out = mbk_pwl_to_stm_pwl( mbkpwlmin );
                                         mbk_pwl_free_pwl( mbkpwlmin );
                                       }
                                       fmaxup = stm_scm2thr (fmaxup, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                       fminup = stm_scm2thr (fminup, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_UP);
                                       dmaxup = TTV_UNIT * dmaxup ;
                                       dminup = TTV_UNIT * dminup ;
                                       fmaxup = TTV_UNIT * fmaxup ;
                                       fminup = TTV_UNIT * fminup ;
                                   }
                                   tas_get_output_carac( driver, 'm', 'D', &s_carac_min );
                                   tas_get_output_carac( driver, 'M', 'D', &s_carac_max );
                                   if((frontconemax->FDOWN != TAS_NOFRONT) && (frontconemin->FDOWN != TAS_NOFRONT)){
                                       fmaxdown = stm_thr2scm (frontconemax->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                       fmindown = stm_thr2scm (frontconemin->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_DN);
                                       slope.F0MAX  = fmaxdown;
                                       slope.FCCMAX = fmaxdown;
                                       slope.SENS   = TRC_SLOPE_DOWN;
                                       slope.CCA    = -1.0;
                                       slope.MIN.slope = fmindown ;              slope.MAX.slope = fmaxdown ;
                                       slope.MIN.vend  = s_carac_min.VF ;        slope.MAX.vend  = s_carac_max.VF ;
                                       slope.MIN.vt    = s_carac_min.VT ;        slope.MAX.vt    = s_carac_max.VT ;
                                       slope.MIN.vdd   = s_carac_min.VDD ;       slope.MAX.vdd   = s_carac_max.VDD ;
                                       slope.MIN.rlin  = s_carac_min.RLIN ;      slope.MAX.rlin  = s_carac_max.RLIN ;
                                       slope.MIN.vth   = s_carac_min.VTH ;       slope.MAX.vth   = s_carac_max.VTH ;
                                       slope.MIN.vsat  = s_carac_min.VSAT ;      slope.MAX.vsat  = s_carac_max.VSAT ;
                                       slope.MIN.r     = s_carac_min.R ;         slope.MAX.r     = s_carac_max.R ;
                                       slope.MIN.c1    = s_carac_min.C1 ;        slope.MAX.c1    = s_carac_max.C1 ;
                                       slope.MIN.c2    = s_carac_min.C2 ;        slope.MAX.c2    = s_carac_max.C2 ;
                                       slope.MIN.slnrm = frontconemin->FDOWN/TTV_UNIT ;
                                       slope.MAX.slnrm = frontconemax->FDOWN/TTV_UNIT ;
                                       slope.MIN.pwl   = stm_pwl_to_mbk_pwl( frontconemin->PWLDN, s_carac_min.VT, s_carac_min.VDD );
                                       slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLDN, s_carac_max.VT, s_carac_max.VDD );
                                       rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                          &dmaxdown, &dmindown, &fmaxdown, &fmindown, ptmbkpwlmax, ptmbkpwlmin );
                                       mbk_pwl_free_pwl( slope.MIN.pwl );
                                       mbk_pwl_free_pwl( slope.MAX.pwl );
                                       if( mbkpwlmax ) {
                                         pwlmax_dw_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                         mbk_pwl_free_pwl( mbkpwlmax );
                                       }
                                       if( mbkpwlmin ) {
                                         pwlmin_dw_out = mbk_pwl_to_stm_pwl( mbkpwlmin );
                                         mbk_pwl_free_pwl( mbkpwlmin );
                                       }
                                       fmaxdown = stm_scm2thr (fmaxdown, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                       fmindown = stm_scm2thr (fmindown, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_min.VT, s_carac_min.VF, s_carac_min.VDD, STM_DN);
                                       dmaxdown = TTV_UNIT * dmaxdown ;
                                       dmindown = TTV_UNIT * dmindown ;
                                       fmaxdown = TTV_UNIT * fmaxdown ;
                                       fmindown = TTV_UNIT * fmindown ;
                                   }
                               }else{
                                   tas_get_output_carac( driver, 'M', 'U', &s_carac_max );
                                   if(frontconemax->FUP != TAS_NOFRONT){
                                       fmaxup = stm_thr2scm (frontconemax->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                       slope.F0MAX  = fmaxup;
                                       slope.FCCMAX = fmaxup;
                                       slope.SENS   = TRC_SLOPE_UP;
                                       slope.CCA    = -1.0;
                                       slope.MIN.slope = -1.0 ;                   slope.MAX.slope = fmaxup ;
                                       slope.MIN.vend  = -1.0 ;                   slope.MAX.vend  = s_carac_max.VF ;
                                       slope.MIN.vt    = -1.0 ;                   slope.MAX.vt    = s_carac_max.VT ;
                                       slope.MIN.vdd   = -1.0 ;                   slope.MAX.vdd   = s_carac_max.VDD ;
                                       slope.MIN.rlin  = -1.0 ;                   slope.MAX.rlin  = s_carac_max.RLIN ;
                                       slope.MIN.vth   = -1.0 ;                   slope.MAX.vth   = s_carac_max.VTH ;
                                       slope.MIN.vsat  = -1.0 ;                   slope.MAX.vsat  = s_carac_max.VSAT ;
                                       slope.MIN.r     = -1.0 ;                   slope.MAX.r     = s_carac_max.R ;
                                       slope.MIN.c1    = -1.0 ;                   slope.MAX.c1    = s_carac_max.C1 ;
                                       slope.MIN.c2    = -1.0 ;                   slope.MAX.c2    = s_carac_max.C2 ;
                                       slope.MIN.slnrm = -1.0 ;
                                       slope.MAX.slnrm = frontconemax->FUP/TTV_UNIT ;
                                       slope.MIN.pwl   = NULL ;
                                       slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLUP, s_carac_max.VT, s_carac_max.VDD  );
                                       rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/, 
                                                          &dmaxup, NULL, &fmaxup, NULL, ptmbkpwlmax, NULL );
                                       mbk_pwl_free_pwl( slope.MAX.pwl );
                                       if( mbkpwlmax ) {
                                         pwlmax_up_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                         mbk_pwl_free_pwl( mbkpwlmax );
                                       }
                                       fmaxup = stm_scm2thr (fmaxup, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_UP);
                                       dmaxup = TTV_UNIT * dmaxup ;
                                       fmaxup = TTV_UNIT * fmaxup ;
                                   }
                                   tas_get_output_carac( driver, 'M', 'D', &s_carac_max );
                                   if(frontconemax->FDOWN != TAS_NOFRONT){
                                       fmaxdown = stm_thr2scm (frontconemax->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                       slope.F0MAX  = fmaxdown;
                                       slope.FCCMAX = fmaxdown;
                                       slope.SENS   = TRC_SLOPE_DOWN;
                                       slope.CCA    = -1.0;
                                       slope.MIN.slope = -1.0 ;                   slope.MAX.slope = fmaxdown ;
                                       slope.MIN.vend  = -1.0 ;                   slope.MAX.vend  = s_carac_max.VF ;
                                       slope.MIN.vt    = -1.0 ;                   slope.MAX.vt    = s_carac_max.VT ;
                                       slope.MIN.vdd   = -1.0 ;                   slope.MAX.vdd   = s_carac_max.VDD ;
                                       slope.MIN.rlin  = -1.0 ;                   slope.MAX.rlin  = s_carac_max.RLIN ;
                                       slope.MIN.vth   = -1.0 ;                   slope.MAX.vth   = s_carac_max.VTH ;
                                       slope.MIN.vsat  = -1.0 ;                   slope.MAX.vsat  = s_carac_max.VSAT ;
                                       slope.MIN.r     = -1.0 ;                   slope.MAX.r     = s_carac_max.R ;
                                       slope.MIN.c1    = -1.0 ;                   slope.MAX.c1    = s_carac_max.C1 ;
                                       slope.MIN.c2    = -1.0 ;                   slope.MAX.c2    = s_carac_max.C2 ;
                                       slope.MIN.slnrm = -1.0 ;
                                       slope.MAX.slnrm = frontconemax->FDOWN/TTV_UNIT ;
                                       slope.MIN.pwl   = NULL ;
                                       slope.MAX.pwl   = stm_pwl_to_mbk_pwl( frontconemax->PWLDN, s_carac_max.VT, s_carac_max.VDD  );
                                       rcx_getdelayslope( TAS_CONTEXT->TAS_LOFIG, loconcone, loconincone, &slope, (double)tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/,
                                                          &dmaxdown, NULL, &fmaxdown, NULL, ptmbkpwlmax, NULL );
                                       mbk_pwl_free_pwl( slope.MAX.pwl );
                                       if( mbkpwlmax ) {
                                         pwlmax_dw_out = mbk_pwl_to_stm_pwl( mbkpwlmax );
                                         mbk_pwl_free_pwl( mbkpwlmax );
                                       }
                                       fmaxdown = stm_scm2thr (fmaxdown, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, s_carac_max.VT, s_carac_max.VF, s_carac_max.VDD, STM_DN);
                                       dmaxdown = TTV_UNIT * dmaxdown ;
                                       fmaxdown = TTV_UNIT * fmaxdown ;
                                   }
                               }
                           }

                           usedmax = 0;
                           usedmin = 0;
                          
                           drivermaxup = frontconemax->DRIVERUP ;
                           driverminup = frontconemin->DRIVERUP ;

                           if(dmaxup >= 0.0){
                               valmaxxup = (long)(dmaxup + 0.5) ;
                               valfmaxxup = (long)(fmaxup + 0.5) ;
                               if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
                                   valminxup = (long)(dminup + 0.5) ;
                                   valfminxup = (long)(fminup + 0.5) ;
                                   if(valmaxxup < valminxup){
                                       long delay;
                                       delay = valminxup ;
                                       valminxup = valmaxxup ;
                                       valmaxxup = delay ;
                                   }
                                   if(valfmaxxup < valfminxup){
                                       long delay;
                                       tas_driver driver ;
                                       stm_pwl *ptpwl;
                                       delay = valfminxup ;
                                       valfminxup = valfmaxxup ;
                                       valfmaxxup = delay ;
                                       ptpwl = pwlmax_up_out ;
                                       pwlmax_up_out = pwlmin_up_out ;
                                       pwlmin_up_out = ptpwl ;
                                       driver = drivermaxup ;
                                       drivermaxup = driverminup ;
                                       driverminup = driver ;
                                       
                                   }
                               }else valminxup = TAS_NOTIME ;
                           }else{
                               valmaxxup = TAS_NOTIME ;
                               valminxup = TAS_NOTIME ;
                               tas_error(58,yagGetName(ifl,loconcone->SIG),TAS_WARNING) ;
                           }
                           if(((valmaxup < valmaxxup) || (valmaxup == TAS_NOTIME)) && (valmaxxup != TAS_NOTIME)){
                               valmaxup = valmaxxup ;
                               valfmaxup = valfmaxxup ;
                               drivermaxxup = drivermaxup ;
                               if( pwlmaxup ) stm_pwl_destroy( pwlmaxup );
                               pwlmaxup = pwlmax_up_out ;
                               usedmax = 1 ;
                           }
                           if(((valminup > valminxup) || (valminup == TAS_NOTIME)) && (valminxup != TAS_NOTIME)){
                               valminup = valminxup ;
                               valfminup = valfminxup ;
                               driverminxup = driverminup ;
                               if( pwlminup ) stm_pwl_destroy( pwlminup );
                               pwlminup = pwlmin_up_out ;
                               usedmin = 1 ;
                           }
                  
                           if( !usedmax ) stm_pwl_destroy( pwlmax_up_out );
                           if( !usedmin ) stm_pwl_destroy( pwlmin_up_out );

                           usedmax = 0;
                           usedmin = 0;

                           drivermaxdn = frontconemax->DRIVERDN ;
                           drivermindn = frontconemin->DRIVERDN ;

                           if(dmaxdown >= 0.0){
                               valmaxxdown = (long)(dmaxdown + 0.5) ;
                               valfmaxxdown = (long)(fmaxdown + 0.5) ;
                               if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
                                   valminxdown = (long)(dmindown + 0.5) ;
                                   valfminxdown = (long)(fmindown + 0.5) ;
                                   if(valmaxxdown < valminxdown){
                                       long delay;
                                       delay = valminxdown ;
                                       valminxdown = valmaxxdown ;
                                       valmaxxdown = delay ;
                                   }
                                   if(valfmaxxdown < valfminxdown){
                                       long delay;
                                       tas_driver driver ;
                                       stm_pwl *ptpwl;
                                       delay = valfminxdown ;
                                       valfminxdown = valfmaxxdown ;
                                       valfmaxxdown = delay ;
                                       ptpwl = pwlmax_dw_out ;
                                       pwlmax_dw_out = pwlmin_dw_out ;
                                       pwlmin_dw_out = ptpwl ;
                                       driver = drivermaxdn ;
                                       drivermaxdn = drivermindn ;
                                       drivermindn = driver ;
                                   }
                               }else valminxdown = TAS_NOTIME ;
                           }else{
                               valmaxxdown = TAS_NOTIME ;
                               valminxdown = TAS_NOTIME ;
                               tas_error(58,yagGetName(ifl,loconcone->SIG),TAS_WARNING) ;
                           }
                           if(((valmaxdown < valmaxxdown) || (valmaxdown == TAS_NOTIME)) && (valmaxxdown != TAS_NOTIME)){
                               valmaxdown = valmaxxdown ;
                               valfmaxdown = valfmaxxdown ;
                               drivermaxxdn = drivermaxdn ;
                               if( pwlmaxdw ) stm_pwl_destroy( pwlmaxdw );
                               pwlmaxdw = pwlmax_dw_out ;
                               usedmax = 1 ;
                           }
                           if(((valmindown > valminxdown) || (valmindown == TAS_NOTIME)) && (valminxdown != TAS_NOTIME)){
                               valmindown = valminxdown ;
                               valfmindown = valfminxdown ;
                               driverminxdn = drivermindn ;
                               if( pwlmindw ) stm_pwl_destroy( pwlmindw );
                               pwlmindw = pwlmin_dw_out ;
                               usedmin = 1 ;
                           }
                           
                           if( !usedmax ) stm_pwl_destroy( pwlmax_dw_out );
                           if( !usedmin ) stm_pwl_destroy( pwlmin_dw_out );
                       }
                   }
                   freechain(chainc) ;

                   if(getptype(loconincone->USER,TAS_SLOPE_MAX) == NULL){
                       if(nbfront == (long)0){
                           front = (front_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(front_list)) ;
                           HEAD_FRONT = addchain(HEAD_FRONT,(void*)front) ;
                           for(nbfront = 0 ; nbfront < TAS_BLOC_SIZE ; nbfront++){
                               (front + nbfront)->FUP = TAS_NOFRONT ;
                               (front + nbfront)->FDOWN = TAS_NOFRONT ;
                               (front + nbfront)->PWLUP = NULL ;
                               (front + nbfront)->PWLDN = NULL ;
                               (front + nbfront)->DRIVERUP.R = -1.0 ;
                               (front + nbfront)->DRIVERUP.V = -1.0 ;
                               (front + nbfront)->DRIVERDN.R = -1.0 ;
                               (front + nbfront)->DRIVERDN.V = -1.0 ;
                           }
                       }
                       nbfront -=2 ;
                       loconincone->USER = addptype(loconincone->USER,TAS_SLOPE_MIN, front+nbfront);
                       loconincone->USER = addptype(loconincone->USER,TAS_SLOPE_MAX, front+nbfront+1);
                       frontinconemin = front+nbfront ;
                       frontinconemax = front+nbfront+1 ;
                   }else{
                       frontinconemin = (front_list *)getptype(loconincone->USER, TAS_SLOPE_MIN)->DATA ;
                       frontinconemax = (front_list *)getptype(loconincone->USER, TAS_SLOPE_MAX)->DATA ;
                   }
                   
                   if((valmaxup != TAS_NOTIME) && (valmaxdown != TAS_NOTIME)){
                       if(frontconemax->FUP != TAS_NOFRONT){
                           delaymax->RCHH = valmaxup ;
                           delaymax->FRCHH = valfmaxup ;
#ifdef USEOLDTEMP             
                           stm_pwl_destroy( delaymax->PWLRCHH );
                           delaymax->PWLRCHH = pwlmaxup ;
#endif
                           frontinconemax->FUP = valfmaxup ;
                           frontinconemax->PWLUP = pwlmaxup ;
                           frontinconemax->DRIVERUP = drivermaxxup ;
                       }else if(loconincone->DIRECTION == CNS_T){
                           frontinconemax->FUP = tas_get_pinslew(loconincone, 'U') ;
                       }
                       if(frontconemax->FDOWN != TAS_NOFRONT){
                           delaymax->RCLL = valmaxdown ;
                           delaymax->FRCLL = valfmaxdown ;
#ifdef USEOLDTEMP             
                           stm_pwl_destroy( delaymax->PWLRCLL );
                           delaymax->PWLRCLL = pwlmaxdw ;
#endif
                           frontinconemax->FDOWN = valfmaxdown ;
                           frontinconemax->PWLDN = pwlmaxdw ;
                           frontinconemax->DRIVERDN = drivermaxxdn ;
                       }else if(loconincone->DIRECTION == CNS_T){
                           frontinconemax->FDOWN = tas_get_pinslew(loconincone, 'D') ;
                       }
                   }
                   if((TAS_CONTEXT->TAS_FIND_MIN == 'Y') && (valminup != TAS_NOTIME) && (valmindown != TAS_NOTIME)){
                       if(frontconemin->FUP != TAS_NOFRONT){
                           delaymin->RCHH = valminup ;
                           delaymin->FRCHH = valfminup ;
#ifdef USEOLDTEMP             
                           stm_pwl_destroy( delaymin->PWLRCHH );
                           delaymin->PWLRCHH = pwlminup ;
#endif
                           frontinconemin->FUP = valfminup ;
                           frontinconemin->PWLUP = pwlminup;
                           frontinconemin->DRIVERUP = driverminxup ;
                       }else if(loconincone->DIRECTION == CNS_T){
                           frontinconemin->FUP = tas_get_pinslew(loconincone, 'U') ;
                       }
                       if(frontconemin->FDOWN != TAS_NOFRONT){
                           delaymin->RCLL = valmindown ;
                           delaymin->FRCLL = valfmindown ;
#ifdef USEOLDTEMP             
                           stm_pwl_destroy( delaymin->PWLRCLL );
                           delaymin->PWLRCLL = pwlmindw ;
#endif
                           frontinconemin->FDOWN = valfmindown ;
                           frontinconemin->PWLDN = pwlmindw;
                           frontinconemin->DRIVERDN = driverminxdn ;
                       }else if(loconincone->DIRECTION == CNS_T){
                           frontinconemin->FDOWN = tas_get_pinslew(loconincone, 'D') ;
                       }
                   }
               }
           }
           cl=delchain(cl,cl);
         }
     }

     return(nbfront) ;
}

/*****************************************************************************
*                        fonction  tas_flatrcx()                             *
*    tratement des interconexion entre cone                                  *
*****************************************************************************/
void tas_flatrcx(lofig,cnsfig,ttvfig)
lofig_list *lofig ;
cnsfig_list *cnsfig ;
ttvfig_list *ttvfig ;
{
 losig_list *losig ;
 losig_list *losigcone ;
 locon_list *locon ;
 lotrs_list *lotrs ;
 cone_list *cone ;
 cone_list *conex ;
 edge_list *incone ;
 branch_list *branch[3];
 link_list *link ;
 ptype_list *ptype ;
 ptype_list *ptypecone ;
 chain_list *chain ;
 rcx_list *ptrcx ;
 char flaglocon ;
 int i, forceit ;

 if((lofig == NULL)/* || (TAS_CONTEXT->TAS_CNS_LOAD == 'Y')*/)
  return ;

 TAS_CONTEXT->TAS_CALCRCX = 'N' ;

 rcx_create(lofig);

 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   chain = NULL ;

   if((ptype = getptype(locon->SIG->USER,LOFIGCHAIN)) != NULL)
     {
      for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         if(((locon_list *)chain->DATA)->TYPE == INTERNAL)
            break ; 
     }

   if((getptype(locon->USER,CNS_EXT) == NULL) &&
      (getptype(locon->USER,CNS_CONE) == NULL) && (chain == NULL))
    {
     if(getptype(locon->SIG->USER,TAS_SIG_NORCDELAY) == NULL) {
     if( TAS_CONTEXT->TAS_CALCRCN == 'Y' )
     locon->SIG->USER = addptype(locon->SIG->USER,TAS_SIG_NORCDELAY,(void*)0x1);
     else
     locon->SIG->USER = addptype(locon->SIG->USER,TAS_SIG_NORCDELAY,NULL);
     }
    }
  }
 
 tas_builtrcxview(lofig,ttvfig) ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
   {
    if((cone->TYPE & (CNS_VDD|CNS_VSS)) != 0)
      continue ;

    branch[0]=cone->BREXT ;
    branch[1]=cone->BRVDD ;
    branch[2]=cone->BRVSS ;
    ptypecone = NULL ;
    if((losigcone = tas_getlosigcone(cone)) != NULL)
     {
      if(((ptrcx = getrcx(losigcone)) != NULL) &&
         (getptype(losigcone->USER,TAS_SIG_NORCDELAY) == NULL))
       {
        if(cone->OUTCONE == NULL)
         {
          losigcone->USER = addptype(losigcone->USER,TAS_SIG_NORCDELAY,NULL) ;
          setrcxmodel(lofig,losigcone,RCX_NORCTC) ;
         }
        else if((ptypecone = getptype(cone->USER,TAS_RC_CONE)) == NULL)
          {
           cone->USER = addptype(cone->USER,TAS_RC_CONE,(void*)NULL) ;
           ptypecone = cone->USER ;
          }
       }
     }

    forceit=0;
    if ((cone->TYPE & TAS_BREAK)!=0 || (cone->TYPE & (CNS_LATCH|CNS_PRECHARGE))!=0)
    {
      branch_list *br;
      int brk=0;
      for(i = 0 ; i < 3 && brk==0; i++)
       for(br=branch[i]; br != NULL && brk==0; br = br->NEXT)
         if ((br->TYPE & CNS_NOT_FUNCTIONAL)!=CNS_NOT_FUNCTIONAL) brk=1;

      if (!brk) forceit=1;
    }
        
    for(i = 0 ; i < 3 ; i++)
     for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
       {
        // ajout 15/9/2005
        if (!forceit && (branch[i]->TYPE & CNS_NOT_FUNCTIONAL)==CNS_NOT_FUNCTIONAL) continue;
        //
        lotrs = NULL ;
        for(link = branch[i]->LINK ; link != NULL ; link=link->NEXT)
         {
          if((link == branch[i]->LINK) && (ptypecone != NULL) &&
             ((cone->TYPE & (CNS_VDD|CNS_VSS)) == 0))
           {
            if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0)
              {
               locon = link->ULINK.LOCON ;
               if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) 
               || (locon->DIRECTION == CNS_T) || (locon->DIRECTION == CNS_Z))
                flaglocon = 'N' ;
               else
                flaglocon = 'Y' ;
              }
            else
              {
               if(link->ULINK.LOTRS->DRAIN->SIG == losigcone)
                 locon = link->ULINK.LOTRS->DRAIN ;
               else
                 locon = link->ULINK.LOTRS->SOURCE ;
               flaglocon = 'Y' ;
              }
            if((getptype(locon->USER,TAS_CON_MARQUE) == NULL) && 
               (flaglocon == 'Y'))
             {
              locon->USER = addptype(locon->USER,TAS_CON_MARQUE,(void*)NULL) ;
              ptypecone->DATA = (void *)addchain((chain_list *)
                                                 ptypecone->DATA,
                                                 (void*)locon) ;
             }
           }
          if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0)
           {
            if((link == branch[i]->LINK) || (lotrs == NULL))
             continue ;
            locon = link->ULINK.LOCON ;
            if((lotrs->DRAIN == NULL) || (lotrs->SOURCE == NULL))
              continue ;
            losig = locon->SIG ;
            if(((ptrcx = getrcx(losig)) == NULL) ||
               (getptype(losig->USER,TAS_SIG_NORCDELAY) != NULL))
             continue ;
            if(locon->SIG == lotrs->DRAIN->SIG)
             locon = lotrs->DRAIN ;
            else if(locon->SIG == lotrs->SOURCE->SIG)
             locon = lotrs->SOURCE ;
            else continue ;
           }
          else
           {
            lotrs = link->ULINK.LOTRS ;
            if(lotrs->GRID == NULL)
             continue ;
            losig = lotrs->GRID->SIG ;     
            if(((ptrcx = getrcx(losig)) == NULL) || 
               (getptype(losig->USER,TAS_SIG_NORCDELAY) != NULL))
             continue ;
            conex = (cone_list *)getptype(lotrs->USER,CNS_DRIVINGCONE)->DATA ;
            if((conex->TYPE & (CNS_VDD|CNS_VSS)) != 0)
              continue ;
           }
          for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
           {
            if((incone->TYPE & (CNS_FEEDBACK|CNS_VDD|CNS_VSS|TAS_PATH_INOUT|CNS_BLEEDER)) != 0)
              if((incone->TYPE & (CNS_COMMAND|CNS_VDD|CNS_VSS|CNS_BLEEDER|TAS_PATH_INOUT)) != CNS_COMMAND)
                continue ;

            if(((incone->TYPE & CNS_EXT) != CNS_EXT) &&
               ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0))
              {
               if(conex == incone->UEDGE.CONE)
                 {
                  if((ptype = getptype(lotrs->GRID->USER,
                                       TAS_CON_INCONE)) != NULL)
                    {
                     if(ptype->DATA != incone)
                       {
                        ptype->DATA = incone ;
                        if((ptype = getptype(incone->USER,
                                       TAS_RC_LOCON)) != NULL)
                         {
                          ptype->DATA = (void *)addchain(
                                       (chain_list*)ptype->DATA,lotrs->GRID) ;
                         }
                        else
                         {
                          incone->USER = addptype(incone->USER,TAS_RC_LOCON,
                                            (void*)addchain((chain_list*)NULL,
                                            (void *)lotrs->GRID)) ;
                         }
                       }
                    }
                  else
                    {
                     lotrs->GRID->USER = addptype(lotrs->GRID->USER,
                                       TAS_CON_INCONE,(void *)incone) ;
                     if((ptype = getptype(incone->USER,
                                       TAS_RC_LOCON)) != NULL)
                      {
                       ptype->DATA = (void *)addchain(
                                     (chain_list*)ptype->DATA,lotrs->GRID) ;
                      }
                     else
                      {
                       incone->USER = addptype(incone->USER,TAS_RC_LOCON,
                                            (void*)addchain((chain_list*)NULL,
                                            (void *)lotrs->GRID)) ;
                      }
                    }
                  break ;
                 }
              }
             else if(((incone->TYPE & CNS_EXT) == CNS_EXT) &&
                     ((link->TYPE & (CNS_IN | CNS_INOUT)) != 0))
              {
               if(link->ULINK.LOCON == incone->UEDGE.LOCON)
                {
                 if((ptype = getptype(locon->USER,
                                       TAS_CON_INCONE)) != NULL)
                    {
                     if(ptype->DATA != incone)
                       {
                        ptype->DATA = incone ;
                        if((ptype = getptype(incone->USER,
                                       TAS_RC_LOCON)) != NULL)
                         {
                          ptype->DATA = (void *)addchain(
                                       (chain_list*)ptype->DATA,locon) ;
                         }
                        else
                         {
                          incone->USER = addptype(incone->USER,TAS_RC_LOCON,
                                            (void*)addchain((chain_list*)NULL,
                                            (void *)locon)) ;
                         }
                       }
                    }
                  else
                    {
                     locon->USER = addptype(locon->USER,
                                       TAS_CON_INCONE,(void *)incone) ;
                     if((ptype = getptype(incone->USER,
                                       TAS_RC_LOCON)) != NULL)
                      {
                       ptype->DATA = (void *)addchain(
                                     (chain_list*)ptype->DATA,locon) ;
                      }
                     else
                      {
                       incone->USER = addptype(incone->USER,TAS_RC_LOCON,
                                            (void*)addchain((chain_list*)NULL,
                                            (void *)locon)) ;
                      }
                    }
                 break ;
                }
              }
           }
         }
       }

    for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
      {
       if((ptype = getptype(incone->USER,
                            TAS_RC_LOCON)) != NULL)
         {
          for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
              chain = chain->NEXT)
           {
            locon = (locon_list *)chain->DATA ;
            if(getptype(locon->USER,TAS_CON_INCONE) != NULL)
              locon->USER = delptype(locon->USER,TAS_CON_INCONE) ;
           }
         }
      }

    if(ptypecone != NULL)
      {
       for(chain = (chain_list *)ptypecone->DATA ; chain != NULL ; 
           chain = chain->NEXT)
        {
         locon = (locon_list *)chain->DATA ;
         locon->USER = delptype(locon->USER,TAS_CON_MARQUE) ;
        }
      }
   }

 tas_detectsig(cnsfig) ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
   {
    for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
      {
       if((ptype = getptype(incone->USER,TAS_RC_LOCON)) != NULL)
          {
           for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
               chain = chain->NEXT)
            {
             locon = (locon_list *)chain->DATA ;
             if(getptype(locon->SIG->USER,TAS_SIG_NORCDELAY) != NULL)
               break ;
            }
           if(chain != NULL)
            {
             freechain((chain_list *)ptype->DATA) ;
             incone->USER = delptype(incone->USER,TAS_RC_LOCON) ;
            }
           else
            {
             for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
               chain = chain->NEXT)
              {
               locon = (locon_list *)chain->DATA ;
               if((ptype = getptype(locon->USER,TAS_CON_INCONE)) != NULL)
               {
                ptype = (ptype_list *)ptype->DATA ;
                if(ptype->DATA == NULL)
                  {
                   ptype->DATA = tas_getsigname(NULL,NULL,incone) ;
                  }
               }
              }
            }
          }
      }

    if((ptype = getptype(cone->USER,TAS_RC_CONE)) != NULL)
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
            chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if(getptype(locon->SIG->USER,TAS_SIG_NORCDELAY) != NULL)
            break ;
         }
        if(chain != NULL)
         {
          freechain((chain_list *)ptype->DATA) ;
          cone->USER = delptype(cone->USER,TAS_RC_CONE) ;
         }
        else
         {
          if((cone->TYPE & CNS_EXT) == CNS_EXT)
           {
            chain_list *cl;
            cl=cns_get_cone_external_connectors(cone);
            if(cl!=NULL/*(ptype = getptype(cone->USER,CNS_EXT)) != NULL*/)
             {
              locon = (locon_list *)cl->DATA ;
              if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) ||
                 (locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_T))
                {
                 tas_getsigname(cone,NULL,NULL) ;
                }
              freechain(cl);
             }
             else
               tas_getsigname(cone,NULL,NULL) ;
            }
          else
            tas_getsigname(cone,NULL,NULL) ;
         }
       }

       tas_rcxsetdriver( cone );
       tas_rcxsetnodebytransition( cone );
   }
 
 for(losig = lofig->LOSIG; losig; losig = losig->NEXT){
     if(((ptrcx = getrcx(losig)) != NULL) &&
         (getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)){
         TAS_CONTEXT->TAS_CALCRCX = 'Y';
         break;
     }
 }

 buildrcx(lofig) ;
}

/* tas_rcxsetnodebytransition()
Informe rcx des noeuds à prendre pour le calcul des délais rc suivant la 
transition.
Version light : ne calcule cette information que sur les grilles des 
transistors.
*/
void tas_rcxsetnodebytransition( cone_list *cone )
{
  edge_list  *incone ;
  chain_list *chain ;
  chain_list *chainin ;
  ptype_list *ptypeincone ;
  num_list   *headup ;
  num_list   *headdn ;
  num_list   *pnode ;
  locon_list *locon ;
  lotrs_list *lotrs ;
  losig_list *currentsig ;
  
  if( !V_BOOL_TAB[ __AVT_RC_BY_TRANSITION ].VALUE )
    return ;

  for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT) {
  
    ptypeincone = getptype( incone->USER, TAS_RC_LOCON );

    if( !ptypeincone )
      continue ;

    chainin = tas_reducechaincon( (chain_list *)ptypeincone->DATA ) ;
  
    /* premiere passe : récupère la liste des node up et down */
    headup = NULL ;
    headdn = NULL ;
    currentsig = NULL ;
    
    for( chain = chainin ; chain ; chain = chain->NEXT ) {
    
      locon = (locon_list*)chain->DATA ;
      if( currentsig ) {
        if( currentsig != locon->SIG ) /* complex case */
          break ;
      }
      currentsig = locon->SIG ;
      if( !getrcx( locon->SIG ) ) 
        break ;
      
      if( locon->TYPE == 'T' ) {
        lotrs = (lotrs_list*)locon->ROOT ;
        if( locon == lotrs->GRID ) {
          for( pnode = locon->PNODE ; pnode ; pnode = pnode->NEXT ) {
            if( MLO_IS_TRANSN( lotrs->TYPE ) ) 
              headup = addnum( headup, pnode->DATA );
            if( MLO_IS_TRANSP( lotrs->TYPE ) )
              headdn = addnum( headdn, pnode->DATA );
          }
        }
      }
    }

    /* seconde passe : met à jour les infos rcx */
    if( headup && headdn && !chain ) {
      for( chain = chainin ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA ;
        if( locon->TYPE == 'T' ) {
          lotrs = (lotrs_list*)locon->ROOT ;
          if( locon == lotrs->GRID ) {
            rcx_setnodebytransition( locon, TRC_SLOPE_UP,   headup );
            rcx_setnodebytransition( locon, TRC_SLOPE_DOWN, headdn );
          }
        }
      }
    }
    freenum( headup );
    freenum( headdn );

    freechain( chainin );
  }
}

void tas_rcxsetdriver( cone_list *cone )
{
  num_list    *driversup ;
  num_list    *driversdw ;
  num_list    *driver ;
  int          i, is_parallel, is_up, is_dw;
  losig_list  *losig ;
  locon_list  *locon ;
  lotrs_list  *lotrs ;
  branch_list *br[3] ;
  branch_list *branch ;
  num_list    *pnode ;
  static int   drivermode=2 ;
  static int   drivergate=2 ;
  char        *env ;
  char         buf[1024] ;
  char        *ptr ;
  ptype_list  *ptype ;
  chain_list  *chain ;
  chain_list  *chlocon ;

  env = V_STR_TAB[__TAS_RCX_DRIVER].VALUE ;
  
  if( env ) {

    strncpy( buf, env, 1024 );

    ptr = strtok( buf, " ,+" );
    
    while( ptr ) {

      if( strcasecmp( ptr, "NONE" ) == 0 ) {
        drivermode = 0 ;
      }
      
      if( strcasecmp( ptr, "ALL" ) == 0 ) {
        drivermode = 1 ;
      }
      
      if( strcasecmp( ptr, "ALL_DIRECTION" ) == 0 ) {
        drivermode = 2 ;
      }

      if( strcasecmp( ptr, "SELECT_DIRECTION" ) == 0 ) {
        drivermode = 3 ;
      }

      if( strcasecmp( ptr, "ALL_GATES" ) == 0 ) {
        drivergate = 1 ;
      }

      if( strcasecmp( ptr, "PARALLEL_GATES" ) == 0 ) {
        drivergate = 2 ;
      }

      if( strcasecmp( ptr, "AUTO" ) == 0 ) {
        drivermode = 4 ;
        drivergate = 1 ;
      }
      
      ptr = strtok( NULL, " ,+" );
    }
  }

  if( !drivermode )
    return ;

  if( getptype(cone->USER,CNS_PARALLEL) || getptype(cone->USER,CNS_PARATRANS) ) {
    is_parallel = 1;
  }
  else {
    is_parallel = 0;
    if( drivergate == 2 ) return ;
  }
    
  driversup = NULL ;
  driversdw = NULL ;

  losig = tas_getlosigcone(cone);

  br[0] = cone->BRVDD ;
  br[1] = cone->BRVSS ;
  br[2] = cone->BREXT ;
  
  for( i=0 ; i<=2 ; i++ ) {

    for( branch = br[i] ; branch ; branch = branch->NEXT ) {
     
      if( ( branch->TYPE & CNS_NOT_FUNCTIONAL ) == CNS_NOT_FUNCTIONAL )
       continue ;
       
      is_up = ( ( branch->TYPE & CNS_VDD ) == CNS_VDD || ( branch->TYPE & CNS_EXT ) == CNS_EXT );
      is_dw = ( ( branch->TYPE & CNS_VSS ) == CNS_VSS || ( branch->TYPE & CNS_EXT ) == CNS_EXT );
       
      chlocon = NULL ;

      if( ( branch->LINK->TYPE & CNS_EXT ) == CNS_EXT ) {
      
        locon = branch->LINK->ULINK.LOCON ;
        chlocon = addchain( chlocon, locon );
      
      }
      else {

        lotrs = branch->LINK->ULINK.LOTRS ;
       
        ptype = getptype( lotrs->USER, MBK_TRANS_PARALLEL );
        if( ptype ) {

          for( chain = (chain_list*)ptype->DATA ; chain ; chain = chain->NEXT ) {
          
            lotrs = (lotrs_list*)chain->DATA ;

            locon = NULL ;
            
            if( lotrs->DRAIN->SIG == losig )
              locon = lotrs->DRAIN ;
            
            if( lotrs->SOURCE->SIG == losig )
              locon = lotrs->SOURCE ;

            if( locon )
              chlocon = addchain( chlocon, locon );
          }
        }
        else {
       
          locon = NULL ;

          if( lotrs->DRAIN->SIG == losig )
            locon = lotrs->DRAIN ;
          
          if( lotrs->SOURCE->SIG == losig )
            locon = lotrs->SOURCE ;

          if( locon )
            chlocon = addchain( chlocon, locon );
        }
      }

      for( chain = chlocon ; chain ; chain = chain->NEXT ) {

        locon = (locon_list*)chain->DATA ;
      
        for( pnode = locon->PNODE ; pnode ; pnode = pnode->NEXT ) {
      
          if( ( drivermode == 2 && is_up ) ||
              drivermode == 1 ||
              ( drivermode == 4 && is_up && ( is_parallel || !driversup ) ) ||
              ( drivermode == 3 && is_up && !driversup )                       ) {

            for( driver = driversup ; driver ; driver = driver->NEXT ) {
              if( driver->DATA == pnode->DATA )
                break ;
            }

            if( !driver )
              driversup = addnum( driversup, pnode->DATA );
          }

          if( ( drivermode == 2 && is_dw ) ||
              drivermode == 1 ||
              ( drivermode == 4 && is_dw && ( is_parallel || !driversdw ) ) ||
              ( drivermode == 3 && is_dw && !driversdw )                       ) {
              
            for( driver = driversdw ; driver ; driver = driver->NEXT ) {
              if( driver->DATA == pnode->DATA )
                break ;
            }

            if( !driver )
              driversdw = addnum( driversdw, pnode->DATA );
          }
        }
      }

      freechain( chlocon );
    }
  }

  if( driversup || driversdw )
    rcx_setdriver( losig, driversup, driversdw );
}

/*****************************************************************************
*                        fonction  visu_branche()                            *
*    debuggage: affiche le type et la resistance du chemin                   *
*****************************************************************************/
int visu_branche(path)
branch_list *path ;

{
link_list *link ;

if(path != NULL) 
    {
    fprintf(stdout,"path type = %ld - ",(long)path->TYPE)  ;
    if((path->TYPE & CNS_VDD) == CNS_VDD) fprintf(stdout,"VDD ") ;
    else if((path->TYPE & CNS_VSS) == CNS_VSS) fprintf(stdout,"VSS ") ;
    else if((path->TYPE & CNS_EXT) == CNS_EXT) fprintf(stdout,"EXT ") ; 

    if((path->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        fprintf(stdout,"non-functional!") ;
 
    fprintf(stdout,"\n\tresistance value = %ld\n",
      (long)(getptype(path->USER,TAS_RESIST)->DATA)) ;

    fflush(stdout) ;
    for(link = (link_list *)path->LINK ; link != NULL ; link = link->NEXT) 
    affiche_maillon(link) ;
    }
return(0);
}

/*****************************************************************************
*                            fonction tas_caparabl()                         *
*          calcul de la capacite de bleeder ramener de chaque cone           *
*****************************************************************************/
void tas_caparabl(cone)
cone_list    *cone ;
{
branch_list   *path[3] ;
ptype_list *ptype ;
chain_list *chain ;
short i;
char flag = 'N' ;
front_list *slope;

path[0] = cone->BRVDD;
path[1] = cone->BRVSS;
path[2] = cone->BREXT;

if (TAS_PATH_TYPE == 'm')
	slope = (front_list *) ((getptype (cone->USER, TAS_SLOPE_MIN))->DATA);
else
	slope = (front_list *) ((getptype (cone->USER, TAS_SLOPE_MAX))->DATA);

for(i=0;i<3;i++)
 {
  for(; path[i] != NULL ; path[i] = path[i]->NEXT)
  if((path[i]->TYPE & (CNS_NOT_FUNCTIONAL | CNS_BLEEDER)) == 0)
    {
     link_list   *link = (link_list *)path[i]->LINK ;
     for(; link != NULL ; link = link->NEXT)
      {
       link->TYPE &= ~(TAS_LINK_BL) ;
       if((ptype = getptype(link->USER,TAS_LINK_BLDUP)) != NULL)
        {
            
         double cb = 0.0 ; 
         for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
             chain = chain->NEXT) 
           {
            if( slope->FUP != TAS_NOFRONT )
              cb += calcul_FB((cone_list *)chain->DATA,
                              ((cone_list *)chain->DATA)->BRVDD,'U') * stm_thr2scm (slope->FUP/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, STM_DEFAULT_VTN, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_UP);
           }
         freechain((chain_list *)ptype->DATA) ;
         ptype->DATA = (void*)((long)(cb * TAS_CONTEXT->TAS_CAPARAPREC * 1000)) ;
         link->TYPE |= TAS_LINK_BL ;
         flag = 'Y' ;
        }
       if((ptype = getptype(link->USER,TAS_LINK_BLDDOWN)) != NULL)
        {
         double cb = 0.0 ; 
         for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
             chain = chain->NEXT)
           {
            if( slope->FDOWN != TAS_NOFRONT )
              cb += calcul_FB((cone_list *)chain->DATA,
                              ((cone_list *)chain->DATA)->BRVSS,'D') * stm_thr2scm (slope->FDOWN/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, STM_DEFAULT_VTP, 0.0, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_DN);
           }
         freechain((chain_list *)ptype->DATA) ;
         ptype->DATA = (void*)((long)(cb * TAS_CONTEXT->TAS_CAPARAPREC * 1000)) ;
         link->TYPE |= TAS_LINK_BL ;
         flag = 'Y' ;
        }
      }
     }
 }

 if(flag == 'Y')
  {
    front_list *slope ;
    avt_log (LOGTAS, 4, "        caparabl: cone %ld '%s'\n", cone->INDEX,cone->NAME);
    front(cone) ;
      slope = (front_list *)((getptype(cone->USER, TAS_SLOPE_MAX))->DATA) ;

      avt_log (LOGTAS, 4, "        caparabl: cone %ld '%s' FUP = ",cone->INDEX, cone->NAME) ;
      if(slope->FUP == TAS_NOFRONT) 
          avt_log (LOGTAS, 4, "NOFRONT ") ;
      else 
          avt_log (LOGTAS, 4, "%ld ",slope->FUP) ;
      avt_log (LOGTAS, 4, "FDOWN = ") ;
      if(slope->FDOWN == TAS_NOFRONT) 
          avt_log (LOGTAS, 4, "NOFRONT\n") ;
      else 
          avt_log (LOGTAS, 4, "%ld\n",slope->FDOWN) ;
      avt_log (LOGTAS, 4, "\n") ;
  }
}

/*****************************************************************************
*                            fonction res_branche()                          *
*          calcul de la resistance equivalente de chaque branche             *
*****************************************************************************/
int res_branche(cone)
cone_list    *cone ;

{
branch_list   *path[3] ;
short i;
ptype_list    *ptype_lotrs, *ptype_link ;
lotrs_list    *lotrs;
double        res_sw;
link_list     *link_sw;

/*---------------------------------------------------------------------------*/
/* si c'est une branche fonctionnelle, on rajoute la resistance equivalente  */
/* de tous ses transistors. Si c'est une branche non-fonctionnelle, on lui   */
/* donne une resistance nulle pour ne pas la prendre en compte.              */
/*---------------------------------------------------------------------------*/
path[0] = cone->BRVDD;
path[1] = cone->BRVSS;
path[2] = cone->BREXT;

for(i=0;i<3;i++)
{
for(; path[i] != NULL ; path[i] = path[i]->NEXT)
if((path[i]->TYPE & (CNS_NOT_FUNCTIONAL | CNS_BLEEDER)) == 0)
    {
    link_list   *link = (link_list *)path[i]->LINK ;
    long        res=0 ;
    double      resist = 0.0 ;

    /* somme des resistances */
    for( /* initialise */ ; link != NULL ; link = link->NEXT)
      {
        if(link->TYPE & (CNS_IN | CNS_INOUT)) continue;
        resist = (double)tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R) *
                 (double)TAS_GETLENGTH(link) /
                 (double)TAS_GETWIDTH(link);
        if( (ptype_lotrs = getptype( link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH )) != NULL) {
          lotrs = (lotrs_list*)ptype_lotrs->DATA ;
          ptype_link = getptype(lotrs->USER, TAS_TRANS_LINK);
          if(ptype_link){
            link_sw = ((chain_list*)ptype_link->DATA)->DATA;
            res_sw = (double)tas_getparam(lotrs,TAS_CASE,TP_R) *
                     (double)TAS_GETLENGTH(link_sw) /
                     (double)TAS_GETWIDTH(link_sw);
            res += (long)(resist * res_sw / (resist + res_sw));
          }else{
            res += (long)(resist);
          }
        }else{
          res += (long)(resist);
        }
      }
    getptype(path[i]->USER,TAS_RESIST)->DATA = (void *)res ;
    }
else /* si branche non fonctionnelle */
    {
    getptype(path[i]->USER,TAS_RESIST)->DATA = (void *) TAS_NORES ;
    }
}

path[0] = cone->BRVDD;
path[1] = cone->BRVSS;
path[2] = cone->BREXT;

for(i=0;i<3;i++)
{
for(; path[i] != NULL ; path[i] = path[i]->NEXT)
    {
    avt_log (LOGTAS, 4, "        res_branche: to ") ;

    if((path[i]->TYPE & CNS_EXT) == CNS_EXT) avt_log (LOGTAS, 4, "EXT") ; 
    else if((path[i]->TYPE & CNS_VDD) == CNS_VDD) avt_log (LOGTAS, 4, "VDD") ;
    else avt_log (LOGTAS, 4, "VSS") ;              

    avt_log (LOGTAS, 4, " in cone %ld '%s' is ",cone->INDEX,cone->NAME) ;
    if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
    avt_log (LOGTAS, 4, "%ld (false path)\n",
            (long)(getptype(path[i]->USER,TAS_RESIST)->DATA)) ;
    else avt_log (LOGTAS, 4, "%ld units\n",
                 (long)(getptype(path[i]->USER,TAS_RESIST)->DATA)) ;
    }
}

return(0) ;
}

/*****************************************************************************
*                     fonction clas_liste_branche()                          *
*           classement de la liste des branches d'un type donne              *
*****************************************************************************/
branch_list *clas_liste_branche(path)
branch_list *path ;
{
branch_list *aux_path ;
chain_list *chainpath  = NULL ;
chain_list *chain ;
short     test=0    ;

for(aux_path = path ; aux_path != NULL ; aux_path = aux_path->NEXT)
 {
  chainpath = addchain(chainpath,aux_path) ;
 }

if(path != NULL) do /* classement si chemin existe */
for(chain = chainpath ; chain->NEXT != NULL ; chain = chain->NEXT)
   { 
    branch_list *nxpath ;
    chain_list *chainx ;

    aux_path = (branch_list *)chain->DATA ;

    test = 0 ;
    for(chainx = chain->NEXT ; chainx != NULL ; chainx = chainx->NEXT)
        {
        nxpath = (branch_list *)chainx->DATA ;

        if((long)(getptype(nxpath->USER,TAS_RESIST)->DATA) > 
           (long)(getptype(aux_path->USER,TAS_RESIST)->DATA))
            {
             chainx->DATA = (void*)aux_path ;
             chain->DATA = (void*)nxpath ;
             aux_path = nxpath ;
             test = 1 ;
            }
        }
   }
while(test != 0) ;

if(path != NULL)
 {
  chain = chainpath ;
  path = (branch_list *)chain->DATA ;
  chain = chain->NEXT ;
  aux_path = path ;

  for(; chain != NULL ; chain = chain->NEXT)
   {
    aux_path->NEXT = (branch_list *)chain->DATA ;
    aux_path = aux_path->NEXT ;
   }
  aux_path->NEXT = NULL ;
 }

freechain(chainpath) ;

    if(path == NULL) avt_log (LOGTAS, 4, "        clas_list_br: no path\n") ;
    else
        {
        avt_log (LOGTAS, 4, "        clas_list_br: the greatest resistor is %ld",
                (long)getptype(path->USER,TAS_RESIST)->DATA) ;
        avt_log (LOGTAS, 4, " / path type = %ld\n",path->TYPE ) ;
        }

 return(path) ;
}


/*****************************************************************************
*                           fonction clas_branche()                          *
* classement des branches du cone par ordre de resistivite croissante        *
* par type de branche (d'abord celles  allant vers les connecteurs externes  * 
* puis celle allant vers vdd et enfin celles menant vers vss                 *
*****************************************************************************/
void clas_branche(cone)
cone_list *cone ;

{

if((cone->BRVDD == NULL)&&(cone->BRVSS == NULL)&&(cone->BREXT == NULL))
 return ;

avt_log (LOGTAS, 4, "        clas_branche: cone %ld '%s'\n",cone->INDEX, cone->NAME) ; 
cone->BREXT = clas_liste_branche(cone->BREXT) ;
cone->BRVDD = clas_liste_branche(cone->BRVDD) ;
cone->BRVSS = clas_liste_branche(cone->BRVSS) ;

}

/*****************************************************************************
*                            fonction valfupfdown()                          *
*                 calcul de FUP ou FDOWN sur la branche choisie              *
*****************************************************************************/
long valfupfdown(path, sens)
branch_list    *path ;
char            sens ;

{
link_list *link ;
long      slope = 0 ;
double    res, capa = 0.0 , restrans = 0.0 ;

/*----------------------------------------------------------------------------*/
/* si il n'y a pas de maillon, il n'y a pas de front=>TAS_NOFRONT.            */
/* si il n`y a qu'un seul maillon et que c'est un connecteur => TAS_CONTEXT->FRONT_CON.    */
/* sinon on calcule le front du a la branche.                                 */
/*----------------------------------------------------------------------------*/
if(path == NULL) /* aucun maillon */
    {
    avt_log (LOGTAS, 4, "        valfupfdown: no slope\n" ) ;
    return(TAS_NOFRONT) ; 
    }

link = (link_list *)path->LINK ;

if(link->NEXT == NULL) /* si un seul maillon sur la branche */
    {
    if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) /* si connecteur => front connecteur */
        {
        slope = tas_get_pinslew(link->ULINK.LOCON, sens);
        avt_log (LOGTAS, 4, "        valfupfdown: value of slope is %ld", (long)slope) ;
        avt_log (LOGTAS, 4, " -connector slope-\n") ;
        return slope;
        }
    }

while(link->NEXT != NULL) /* il existe des maillons transistors */
    {

    res = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R);

    restrans = (res*(double)TAS_GETLENGTH(link) /
                          (double)TAS_GETWIDTH(link)) ;

    if(slope != 0)
    restrans = (restrans * 0.95) ;

    if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
    restrans = (restrans * 0.5) ; 

    if(slope == 0)
    capa += (tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                         tas_getcapabl(link)) ;

    else
    capa += (0.95*(tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                            tas_getcapabl(link))) ;

    slope += (long)(restrans*capa*tlc_getcapafactor()) ;
    link = link->NEXT ;
    }

if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0) /* si dernier maillon PAS connecteur */
    {
    res = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R);

    restrans = (res*(double)TAS_GETLENGTH(link) /
                          (double)TAS_GETWIDTH(link)) ;

    if(slope != 0)
    restrans = (restrans * 0.95) ;

    if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
    restrans = (restrans * 0.5) ;

    if(slope==0L)
    capa += (tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                          tas_getcapabl(link)) ;

    else
    capa += (0.95*(tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                            tas_getcapabl(link))) ;

    slope += (long)(restrans*capa*tlc_getcapafactor()) ;
    }
/* si dernier maillon connecteur, alors pas touche! */

if( slope > 0 )
  slope = stm_scm2thr (slope, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, STM_DEFAULT_VT, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_UP);

slope = (long)( TTV_UNIT * slope  / 1000) ;

if(slope == 0) /* front nul => erreur: plus tard cause des problemes ! */
 {
  tas_error(1,NULL,TAS_WARNING) ;
  slope = 1 ;
 }

return(slope) ;
}


/*****************************************************************************
*                           fonction decision()                              *
* decide de prendre soit la branche EXT soit la branche VSS,VDD suivant      *
* qu'on ait trouve des  branches qui convenaient,et si les 2 branches        *
* existent prend la branche la plus resistive                                *  
*****************************************************************************/
branch_list *decision(ext,alim,ext_path,alim_path)
char      ext ;
char      alim ;
branch_list *ext_path ;
branch_list *alim_path ;

{
branch_list *path ; /* branche decide qui doit etre renvoyee */

if(ext == 'n') /* si pas de branche externe */
    {
    if(alim == 'n') path = NULL ; /* si non plus branche alim */
    else path = alim_path ; /* sinon branche alim */
    }
else /* si branche externe est oui */
    {
    if(alim == 'n') path = ext_path ; /* si pas de branche alim=>externe */
    else /* si les deux existent, il faut renvoyer la plus resistive */
    path = ((long)getptype(ext_path->USER,TAS_RESIST)->DATA > 
            (long)getptype(alim_path->USER,TAS_RESIST)->DATA) ?
           ext_path : alim_path ;
    }

return(path) ;
}


/*****************************************************************************
*                               fonction front()                             *
*                               calcul de front                              *
*****************************************************************************/
long front(cone)
cone_list *cone ;

{
char       ext='n',
           exl='n',
           vdd='n',
           vss='n' ;

branch_list  *pathext ;
branch_list  *pathvdd ;
branch_list  *pathvss ;
front_list *slope ;
locon_list *locon ;
ptype_list *ptype ;
chain_list *chain ;

/*---------------------------------------------------------------------------*/
/* a ce niveau, les branches de chaque cone sont classees dans l'ordre       */
/* decroissant et par type : EXT, VDD et VSS. Dans un premier temps on       */
/* la branche la plus resistive pour chaque type (premiere branche de chaque */
/* liste).                                                                   */
/*---------------------------------------------------------------------------*/

if(cone->BREXT != NULL)
  if((cone->BREXT->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
      {
       ext = 'o' ;
       for(pathext = cone->BREXT ; pathext != NULL ; pathext = pathext->NEXT)
         if((pathext->LINK->NEXT != NULL) && 
             ((pathext->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL))
         {
          exl = 'o' ;
          break ;
         }
      }
if(cone->BRVDD != NULL)
   if((cone->BRVDD->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
        vdd = 'o' ;
if(cone->BRVSS != NULL)
   if((cone->BRVSS->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
        vss = 'o' ;

if((cone->TYPE & CNS_EXT) == CNS_EXT)
 {
  if((vdd == 'n') && (vss == 'n') && (exl == 'n'))
   {
    chain_list *cl;
    cl=cns_get_cone_external_connectors(cone);
    locon = (locon_list *)cl->DATA; //getptype(cone->USER,CNS_EXT)->DATA ;
    freechain(cl);
    ptype = getptype(locon->SIG->USER,LOFIGCHAIN) ;
    chain = NULL ;
    if(ptype != NULL)
      {
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
          {
           if((((locon_list *)chain->DATA)->TYPE == INTERNAL) &&
              ((((locon_list *)chain->DATA)->DIRECTION != CNS_I) &&
               (((locon_list *)chain->DATA)->DIRECTION != CNS_X)))
               break ;
          }
      }
    if(chain == NULL)
      {
       if(locon->DIRECTION == CNS_T)
        {
         locon->DIRECTION = CNS_I ;
        }
       else if((locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_O))
        {
         locon->DIRECTION = CNS_X ;
        }
      }
   }
 }
        
/*---------------------------------------------------------------------------*/
/* pour calculer FUP il faut prendre la branche la plus resistive entre      */
/* une externe et une VDD. Idem pour FDOWN, mais avec une branche VSS.       */
/* C'est la fonction decision() qui choisit la branche.                      */
/*---------------------------------------------------------------------------*/
tlc_setmaxfactor() ;

slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA ;

if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0)
 {
  slope->FUP = TAS_NOFRONT ;
  slope->FDOWN = TAS_NOFRONT ;
 }
else
 {
  if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
    slope->FUP = valfupfdown(decision(ext,vdd,cone->BREXT,cone->BRVDD), 'U') ;
  else
    slope->FUP = TAS_NOFRONT ;

  if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
    slope->FDOWN = valfupfdown(decision(ext,vss,cone->BREXT,cone->BRVSS), 'D') ;
  else
    slope->FDOWN = TAS_NOFRONT ;
 }

tlc_setminfactor() ;

/* calcul du front min */
if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
{
TAS_PATH_TYPE = 'm' ;
if(cone->BREXT == NULL) pathext = NULL ;
else
 {
  for(pathext = cone->BREXT ; pathext->NEXT != NULL ; pathext = pathext->NEXT)
   {
      if((pathext->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        break ;
   }
  if((pathext->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
    pathext = NULL ;
 }

if(cone->BRVDD == NULL) pathvdd = NULL ;
else
 {
  for(pathvdd = cone->BRVDD ; pathvdd->NEXT != NULL ; pathvdd = pathvdd->NEXT)
   {
      if((pathvdd->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        break ;
   }
  if((pathvdd->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
    pathvdd = NULL ;
 }

if(cone->BRVSS == NULL) pathvss = NULL ;
else
 {
  for(pathvss = cone->BRVSS ; pathvss->NEXT != NULL ; pathvss = pathvss->NEXT)
   {
      if((pathvss->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        break ;
   }
  if((pathvss->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
    pathvss = NULL ;
 }

slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA ;

if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0)
 {
  slope->FUP = TAS_NOFRONT ;
  slope->FDOWN = TAS_NOFRONT ;
 }
else if(pathext == NULL) 
 {
  if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
    slope->FUP = valfupfdown(pathvdd, 'U') ;
  else
    slope->FUP = TAS_NOFRONT ;
  if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
    slope->FDOWN = valfupfdown(pathvss, 'D') ;
  else
    slope->FDOWN = TAS_NOFRONT ;
 }
else
 {
  if(pathvdd == NULL) 
   {
    if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
      slope->FUP = valfupfdown(pathext, 'U') ;
    else
      slope->FUP = TAS_NOFRONT ;
   }
  else 
   {
    if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
    slope->FUP = valfupfdown((((long)getptype(pathext->USER,TAS_RESIST)->DATA > 
        (long)getptype(pathvdd->USER,TAS_RESIST)->DATA) ? pathext : pathvdd), 'U') ;
    else
      slope->FUP = TAS_NOFRONT ;
   }
  if(pathvss == NULL)
    {
     if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
       slope->FDOWN = valfupfdown(pathext, 'D') ;
     else
       slope->FDOWN = TAS_NOFRONT ;
    }
  else 
   {
     if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
   slope->FDOWN = valfupfdown((((long)getptype(pathext->USER,TAS_RESIST)->DATA > 
       (long)getptype(pathvss->USER,TAS_RESIST)->DATA) ? pathext : pathvss), 'D') ;
     else
       slope->FDOWN = TAS_NOFRONT ;
   }
 }
TAS_PATH_TYPE = 'M' ;
}

    avt_log (LOGTAS, 4, "        front: ") ;
    if(((cone->BREXT)==NULL && (cone->BRVDD)==NULL && (cone->BRVSS)==NULL)) 
        avt_log (LOGTAS, 4, "no path\n") ;
    else
        {
        avt_log (LOGTAS, 4, "types of pathes are ") ;
        if(cone->BREXT != NULL) avt_log (LOGTAS, 4, "extern ") ;
        if(cone->BRVDD != NULL) avt_log (LOGTAS, 4, "VDD ") ;
        if(cone->BRVSS != NULL) avt_log (LOGTAS, 4, "VSS") ;
        avt_log (LOGTAS, 4, "\n") ;
        }

return(0) ;
}

/*****************************************************************************
*                      fonction interface()                                  *
* calcule les capacites devant etre donnees pour chaque connecteur.          *
*****************************************************************************/
void interface_locon (locons)
locon_list *locons ;

{
locon_list *locon ;
ptype_list *ptuser ;
ptype_list *ptype ;
caraccon_list *carac ;
chain_list *hchain , *chain ;
double     capa, capaup, capaupmax, capadn, capadnmax, clo ;
elpcapaswitch_list *elpcsw;
char flag;
float outcapa;


/*-------------------------------------------------------------------*/
/* les resistances du fichier '.prf', je crois qu'elles ne servent a */
/* rien. Alors je ne calcule rien du tout jusqu'a avis contraire.    */
/* Ici on ne calcule que des capacites.                              */
/*-------------------------------------------------------------------*/
for(locon = locons ; locon != NULL ; locon = locon->NEXT){
if((locon->DIRECTION != 'X') && (locon->DIRECTION != CNS_VDDC) && 
    (locon->DIRECTION != TAS_ALIM) && 
      (locon->DIRECTION != CNS_VSSC))
    {
    ptuser = getptype(locon->USER,CNS_CONE);
    capa         = 0.0;
    capaup       = 0.0;
    capaupmax    = 0.0;
    capadn       = 0.0;
    capadnmax    = 0.0;
    clo          = 0.0;
    flag         = 0;

    if(ptuser != NULL) 
    {
    hchain = (chain_list *)ptuser->DATA;
    for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
        {
        branch_list *path = ((cone_list *)chain->DATA)->BREXT ;

        for( /* initialise */ ; path != NULL ; path = path->NEXT)
            {
            link_list *link ;

            /* recherche de la branche contenant le connecteur */
            for(link = (link_list *)path->LINK ; link->NEXT != NULL ;
                link = link->NEXT) ;

            /* si dernier maillon pas connecteur => avertissement */
            if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
            tas_error(34,(char *)chain->DATA,TAS_WARNING) ;

            if(link->ULINK.LOCON == locon)
                {
                clo = tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) ;

                if(TAS_CONTEXT->TAS_CAPASWITCH != 0.0)
                for(link = (link_list *)path->LINK ; link->NEXT != NULL ;
                    link = link->NEXT)
                    {
                    chain_list *chainx ;

                    /* gere les capas ramenees pour les switchs */
                    if((ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH)) != NULL){
                        if((ptype = getptype(((lotrs_list*)ptype->DATA)->USER,TAS_CAPA_PASS)) != NULL){
                            continue ;
                        }
                    }
                    
                    ptype = getptype(link->ULINK.LOTRS->USER,TAS_CAPA_PASS) ;
                    if(ptype != NULL) 
                       chainx = (chain_list *)ptype->DATA ;
                    else chainx = NULL ;
                    if(chainx != hchain)
                        {
                        flag = 1;
                        capa += tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) ;
                        if(link->ULINK.LOTRS->TRNAME){
                            avt_log (LOGTAS, 1, "Capacitance value of %.2f fF (%.1f%%) added on locon %s via transistor %s\n", tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT)*TAS_CONTEXT->TAS_CAPASWITCH, TAS_CONTEXT->TAS_CAPASWITCH*100.0, locon->NAME, link->ULINK.LOTRS->TRNAME);
                        }else{
                            avt_log (LOGTAS, 1, "Capacitance value of %.2f fF (%.1f%%) added on locon %s via transistor ???\n", tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT)*TAS_CONTEXT->TAS_CAPASWITCH, TAS_CONTEXT->TAS_CAPASWITCH*100.0, locon->NAME);
                        }
                        switch(V_INT_TAB[__ELP_CAPA_LEVEL].VALUE){
                            case ELP_CAPA_LEVEL0:
                                capaup = capaupmax = capadn = capadnmax = capa;
                                break;
                            case ELP_CAPA_LEVEL1:
                                capaup    += tas_gettotalcapa(NULL, link->SIG, ELP_CAPA_UP);
                                capaupmax = capaup;
                                capadn    += tas_gettotalcapa(NULL, link->SIG, ELP_CAPA_DN);
                                capadnmax = capadn;
                                break;
                            case ELP_CAPA_LEVEL2:
                                capaup    += tas_gettotalcapa(NULL, link->SIG, ELP_CAPA_UP);
                                capaupmax += tas_gettotalcapa(NULL, link->SIG, ELP_CAPA_UP_MAX);
                                capadn    += tas_gettotalcapa(NULL, link->SIG, ELP_CAPA_DN);
                                capadnmax += tas_gettotalcapa(NULL, link->SIG, ELP_CAPA_DN_MAX);
                        }
                        /*---------------------------------------------------*/
                        /* on tag pour ne pas prendre la meme capa plusieurs */
                        /* fois.                                             */
                        /*---------------------------------------------------*/
                        link->ULINK.LOTRS->USER = addptype(
                        link->ULINK.LOTRS->USER,TAS_CAPA_PASS,(void *)hchain) ;
                        }
                    }
                else break ;
                }
            } /* fin des branches */

        if(path != NULL) break ;
        } /* fin des chains */
    }
    else
        {
        cone_list *conex ;
        ptype = getptype(locon->USER,CNS_EXT) ;
        if(ptype != NULL) conex = (cone_list *)ptype->DATA;

        if(ptype == NULL)
          {
           ptype = getptype(locon->SIG->USER,LOFIGCHAIN) ;
           if(ptype != NULL)
             for(chain = (chain_list*)ptype->DATA ; chain != NULL ;
                 chain = chain->NEXT)
                 if(((locon_list *)chain->DATA)->TYPE == INTERNAL)
                    break ;
           if(chain == NULL)
             tas_error(4,locon->NAME,TAS_WARNING) ;
          }

        else
          if(conex->BREXT != NULL)
            clo = tas_getcapalink(NULL, conex->BREXT->LINK,TAS_UNKNOWN_EVENT) ;
          else if(conex->BRVDD != NULL)
            clo = tas_getcapalink(NULL, conex->BRVDD->LINK,TAS_UNKNOWN_EVENT) ;
          else if(conex->BRVSS != NULL)
            clo = tas_getcapalink(NULL, conex->BRVSS->LINK,TAS_UNKNOWN_EVENT) ;
        }

    carac = (caraccon_list *)getptype(locon->USER,TAS_CON_CARAC)->DATA ;
    carac->C = (float)(clo + capa*TAS_CONTEXT->TAS_CAPASWITCH) ;
    if(flag){
        elpcsw = (elpcapaswitch_list*)mbkalloc( sizeof( elpcapaswitch_list ) );
        elpcsw->CUP    = (float)(clo + capaup*TAS_CONTEXT->TAS_CAPASWITCH)/1000.0 ;
        elpcsw->CUPMAX = (float)(clo + capaupmax)/1000.0 ;
        elpcsw->CUPMIN = (float)(clo)/1000.0 ;
        elpcsw->CDN    = (float)(clo + capadn*TAS_CONTEXT->TAS_CAPASWITCH)/1000.0 ;
        elpcsw->CDNMAX = (float)(clo + capadnmax)/1000.0 ;
        elpcsw->CDNMIN = (float)(clo)/1000.0 ;
        locon->SIG->USER = addptype(locon->SIG->USER, ELP_CAPASWITCH, elpcsw) ;
    }
    
    if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) || 
      (locon->DIRECTION == CNS_T) || (locon->DIRECTION == CNS_Z))
     {
      cone_list *cone ;
      branch_list *pathvdd ;
      branch_list *pathvss ;
    

      if((ptuser = getptype(locon->USER,CNS_EXT)) == NULL)
      {
        if(TAS_CONTEXT->TAS_CAPAOUT != 0.0)
         carac->C -= (float)(1000.0 * TAS_CONTEXT->TAS_CAPAOUT) ;
        continue ;
      }

      cone = ptuser->DATA ;

      outcapa=tas_get_cone_output_capacitance(cone);
      if (outcapa!=0.0)
        carac->C -= (float)(1000.0 * outcapa) ;

      
      if(cone->BRVDD != NULL)
       {
        if((cone->BRVDD->TYPE & CNS_NOT_FUNCTIONAL)!= CNS_NOT_FUNCTIONAL)
         {
          carac->RUPMAX = (long)getptype(cone->BRVDD->USER,TAS_RESIST)->DATA ;
          if(carac->RUPMAX == 0)
               carac->RUPMAX++ ;
         }
       }
      else carac->RUPMAX = TAS_NORES ;
      if(cone->BRVSS != NULL)
       {
        if((cone->BRVSS->TYPE & CNS_NOT_FUNCTIONAL)!= CNS_NOT_FUNCTIONAL)
         {
          carac->RDOWNMAX = (long)getptype(cone->BRVSS->USER,TAS_RESIST)->DATA ;
          if(carac->RDOWNMAX == 0)
               carac->RDOWNMAX++ ;
         }
       }
      else carac->RDOWNMAX = TAS_NORES ;

      if(cone->BRVDD != NULL)
       {
        for(pathvdd = cone->BRVDD;pathvdd->NEXT != NULL;pathvdd = pathvdd->NEXT)
         {
          if((pathvdd->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            break ;
         }
        if((pathvdd->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
          pathvdd = NULL ;
       }
      else pathvdd = NULL ;

      if(cone->BRVSS != NULL)
       {
        for(pathvss = cone->BRVSS;pathvss->NEXT != NULL;pathvss = pathvss->NEXT)
         {
          if((pathvss->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            break ;
         }
        if((pathvss->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
          pathvss = NULL ;
       }
      else pathvss = NULL ;

      if(pathvdd != NULL)
        {
         carac->RUPMIN = (long)getptype(pathvdd->USER,TAS_RESIST)->DATA ;
         if(carac->RUPMIN == 0)
              carac->RUPMIN++ ;
        }

      if(pathvss != NULL)
        {
         carac->RDOWNMIN = (long)getptype(pathvss->USER,TAS_RESIST)->DATA ;
         if(carac->RDOWNMIN == 0)
              carac->RDOWNMIN++ ;
        }
     }
    } /* fin du parcours des connecteurs */
    if((locon->DIRECTION == 'X') && 
       ((ptype=getptype(locon->USER,TAS_CON_CARAC))!=NULL) &&
        (TAS_CONTEXT->TAS_CNS_LOAD == 'N')) 
     {
      caraccon_list *carac = (caraccon_list *)ptype->DATA ;
      clo = tas_gettotalcapa(NULL, locon->SIG, ELP_CAPA_TYPICAL);
      carac->C = (float)clo ;
     }

}

for(locon = locons ; locon != NULL ; locon = locon->NEXT)
if((locon->DIRECTION != 'X') && (locon->DIRECTION != CNS_VDDC) &&
    (locon->DIRECTION != TAS_ALIM) && 
    (locon->DIRECTION != TAS_ALIM) && 
      (locon->DIRECTION != CNS_VSSC))
    {
    ptuser = getptype(locon->USER,CNS_CONE);

    if(ptuser != NULL) 
    {
    hchain = (chain_list *)ptuser->DATA;
    for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
        {
        branch_list *path = ((cone_list *)chain->DATA)->BREXT ;

        for( ; path != NULL ; path = path->NEXT)
            {
            link_list *link ;

            for(link = (link_list *)path->LINK ; link->NEXT != NULL ;
                link = link->NEXT)
                  {
                    ptype = getptype(link->ULINK.LOTRS->USER,TAS_CAPA_PASS) ;
                    if(ptype != NULL) 
                    link->ULINK.LOTRS->USER = delptype(
                    link->ULINK.LOTRS->USER,TAS_CAPA_PASS) ;
                  }
             }
          }
    }
    }

}

/*****************************************************************************
*                      fonction interface()                                  *
* calcule les capacites devant etre donnees pour chaque connecteur.          *
*****************************************************************************/

int interface (cnsfig_list *cnsfig)
{
    interface_locon (cnsfig->LOCON);
    interface_locon (cnsfig->INTCON);

    return 0;
}

/*****************************************************************************
*                                fonction tas_findbufdelay()                 *
* calcul les delay des buffers jusqu'au premier nom buffer qu'il renvoie     *
*****************************************************************************/
cone_list *tas_findbufdelay(cone,delayup,delaydown,sens)
cone_list *cone ;
long *delayup ;
long *delaydown ;
char sens ;
{
 cone_list *coneend ;
 delay_list *delay ;


 
 if((cone->TYPE & (TAS_CONE_INV|TAS_CONE_BUF)) == 0)
   return(cone) ;

 if (getptype(cone->USER, TAS_CONE_MARQUE)!=NULL) return (cone) ;
 cone->USER = addptype(cone->USER, TAS_CONE_MARQUE, NULL) ;

 if((cone->TYPE & TAS_CONE_BUF) == TAS_CONE_BUF)
  {
   if(tas_buftoinv(cone) == 0)
    {
     cone->USER = testanddelptype(cone->USER, TAS_CONE_MARQUE) ;
     return(cone) ;
    }
  }

 if(sens == 'D') 
  {
   coneend = tas_findbufdelay(cone->INCONE->UEDGE.CONE,delayup,delaydown,'I') ;
   delay = (delay_list *)getptype(cone->INCONE->USER,TAS_DELAY_MAX)->DATA ;
   *delayup += delay->TPLH ;
   if(delay->RCLL != TAS_NOTIME)
       *delayup += delay->RCLL ;
   *delaydown += delay->TPHL ;
   if(delay->RCHH != TAS_NOTIME)
       *delaydown += delay->RCHH ;
  }
 else
  {
   coneend = tas_findbufdelay(cone->INCONE->UEDGE.CONE,delayup,delaydown,'D') ;
   delay = (delay_list *)getptype(cone->INCONE->USER,TAS_DELAY_MAX)->DATA ;
   *delayup += delay->TPHL ;
   if(delay->RCHH != TAS_NOTIME)
       *delayup += delay->RCHH ;
   *delaydown += delay->TPLH ;
   if(delay->RCLL != TAS_NOTIME)
       *delaydown += delay->RCLL ;
  }
 cone->USER = testanddelptype(cone->USER, TAS_CONE_MARQUE) ;
 return(coneend) ;
}

/*****************************************************************************
*                                fonction tas_buftoinv()                     *
*            transforme les buffers en inverseurs                            *
*****************************************************************************/
int tas_buftoinv(cone)
cone_list *cone ;
{
 cone_list *conext = NULL ;
 cone_list *conex ;
 edge_list *incone ;
 branch_list *branch[2] ;
 front_list *slope ;
 
 front_list *frontin ;
 long delayup ;
 long delaydown ;
 long delayrefup ;
 long delayrefdown ;
 long frefup ;
 long frefdown ;
 long err ;
 long ferr ;
 long res ;
 int i ;

 for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
  {
   if((incone->TYPE & CNS_CONE) == CNS_CONE)
     {
      delayup = (long)0 ;
      delaydown = (long)0 ;
      conex = tas_findbufdelay(incone->UEDGE.CONE,&delayup,&delaydown,'D') ;
      frontin = (front_list *)((getptype(incone->UEDGE.CONE->USER,
                                       TAS_SLOPE_MAX))->DATA);
     }
   else continue ;
   if(conext == NULL)
    {
     delayrefup = delayup ;
     delayrefdown = delaydown ;
     conext = conex ;
     frefup = frontin->FUP ;
     frefdown = frontin->FDOWN ;
    }
   else
    {
     if(conex != conext)
       return(0) ;
     if(delayup > delayrefup)
       err = delayup - delayrefup ;
     else
       err = delayrefup - delayup ;
     if(frontin->FUP > frefup)
       ferr = frontin->FUP - frefup ;
     else
       ferr = frefup - frontin->FUP ;
     if((err > (frefup/(long)10)) || (ferr > (frefup/(long)10)))
       return(0) ;
     if(delaydown > delayrefdown)
       err = delaydown - delayrefdown ;
     else
       err = delayrefdown - delaydown ;
     if(frontin->FDOWN > frefdown)
       ferr = frontin->FDOWN - frefdown ;
     else
       ferr = frefdown - frontin->FDOWN ;
     if((err > (frefdown/(long)10)) || (ferr > (frefdown/(long)10)))
       return(0) ;
    }
  }
      
 branch[0] = cone->BRVSS ;
 branch[1] = cone->BRVDD ;

 for(i = 0 ; i < 2 ; i++)
  {
   branch_list *branchx ;
   res = (long)0 ;
   for(branchx = branch[i] ; branchx != NULL ; branchx = branchx->NEXT) 
   {
    res += (long)(((double)TAS_GETWIDTH(branchx->LINK)/
                   (double)TAS_GETLENGTH(branchx->LINK)));
   }
   for(branchx = branch[i] ; branchx != NULL ; branchx = branchx->NEXT) 
   {
    TAS_GETCLINK(branchx->LINK)->WIDTH = res * 
                                         TAS_GETLENGTH(branchx->LINK) ;
   }
  }

 cone->TYPE &= ~(TAS_CONE_BUF) ;
 cone->TYPE |= TAS_CONE_INV ;

 avt_log (LOGTAS, 3, "\n\n        buftoinv: cone %ld '%s'\n\n",cone->INDEX, cone->NAME);
 res_branche(cone) ;
 clas_branche(cone) ;

 if((cone->TYPE & CNS_EXT) == CNS_EXT)
  {
   chain_list *cl;
   cl=cns_get_cone_external_connectors(cone);
   while (cl!=NULL)
   {
     locon_list *locon = (locon_list *)cl->DATA; //getptype(cone->USER,CNS_EXT)->DATA ;
     caraccon_list *carac = (caraccon_list *)getptype(locon->USER,
                                                      TAS_CON_CARAC)->DATA ;
     branch_list *pathvdd ;
     branch_list *pathvss ;
     if(cone->BRVDD != NULL)
         {
          if((cone->BRVDD->TYPE & CNS_NOT_FUNCTIONAL)!= CNS_NOT_FUNCTIONAL)
           {
            carac->RUPMAX = (long)getptype(cone->BRVDD->USER,TAS_RESIST)->DATA ;
            if(carac->RUPMAX == 0)
                 carac->RUPMAX++ ;
           }
         }
        else carac->RUPMAX = TAS_NORES ;
        if(cone->BRVSS != NULL)
         {
          if((cone->BRVSS->TYPE & CNS_NOT_FUNCTIONAL)!= CNS_NOT_FUNCTIONAL)
           {
            carac->RDOWNMAX = (long)getptype(cone->BRVSS->USER,TAS_RESIST)->DATA ;
            if(carac->RDOWNMAX == 0)
                 carac->RDOWNMAX++ ;
           }
         }
        else carac->RDOWNMAX = TAS_NORES ;

        if(cone->BRVDD != NULL)
         {
          for(pathvdd = cone->BRVDD;pathvdd->NEXT != NULL;pathvdd = pathvdd->NEXT)
           {
            if((pathvdd->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
              break ;
           }
          if((pathvdd->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
            pathvdd = NULL ;
         }
        else pathvdd = NULL ;

        if(cone->BRVSS != NULL)
         {
          for(pathvss = cone->BRVSS;pathvss->NEXT != NULL;pathvss = pathvss->NEXT)
           {
            if((pathvss->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
              break ;
           }
          if((pathvss->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
            pathvss = NULL ;
         }
        else pathvss = NULL ;

        if(pathvdd != NULL)
          {
           carac->RUPMIN = (long)getptype(pathvdd->USER,TAS_RESIST)->DATA ;
           if(carac->RUPMIN == 0)
                carac->RUPMIN++ ;
          }

        if(pathvss != NULL)
          {
           carac->RDOWNMIN = (long)getptype(pathvss->USER,TAS_RESIST)->DATA ;
           if(carac->RDOWNMIN == 0)
                carac->RDOWNMIN++ ;
          }
     cl=delchain(cl,cl);
   }
  }
 
 slope = (front_list *)((getptype(cone->USER, TAS_SLOPE_MAX))->DATA) ;
 avt_log (LOGTAS, 4, "       buftoinv: cone %ld '%s' FUP = ",cone->INDEX,
         cone->NAME) ;
 if(slope->FUP == TAS_NOFRONT) avt_log (LOGTAS, 3, "NOFRONT ") ;
 else avt_log (LOGTAS, 4, "%ld ",slope->FUP) ;
 avt_log (LOGTAS, 4, "FDOWN = ") ;
 if(slope->FDOWN == TAS_NOFRONT) avt_log (LOGTAS, 4, "NOFRONT\n\n") ;
 else avt_log (LOGTAS, 4, "%ld\n\n",slope->FDOWN) ;
 tas_tpd(cone) ;

 TAS_PATH_TYPE = 'M' ;
   
return(0);
}
/*****************************************************************************
*                                fonction tas_modelmemsym()                 *
*****************************************************************************/
int tas_ismemsym (cone_list *cone)
{
   return (((cone->TYPE & (CNS_MEMSYM|CNS_LATCH)) == (CNS_MEMSYM|CNS_LATCH)) ||
       ((cone->TYPE & (CNS_MEMSYM|CNS_FLIP_FLOP)) == (CNS_MEMSYM|CNS_FLIP_FLOP)));
}

void tas_modelmemsym(cnsfig_list *cnsfig)
{
   cone_list *mem;
   cone_list *memsym;
   edge_list *cmd;
   edge_list *incone;
   edge_list *inmemsym;
   delay_list *delay_cmd_mem;
   delay_list *delay_memsym_mem;
   delay_list *delay_cmd_memsym;
   delay_list *delaymax_cmd_mem;
   delay_list *delaymax_memsym_mem;
   delay_list *delaymax_cmd_memsym;
   delay_list *delaymin_cmd_mem;
   delay_list *delaymin_memsym_mem;
   delay_list *delaymin_cmd_memsym;
   int i;
   float csttab[2];
   char type;
   char *name;
   char *modname;
   char *cmd_name;
   timing_model *tmodel;
   timing_cell *cell = stm_getcell(CELL);
 
   for(mem = cnsfig->CONE ; mem ; mem = mem->NEXT) {
      if (tas_ismemsym (mem)) {
          memsym = NULL;
          delaymax_memsym_mem = NULL;
          delaymin_memsym_mem = NULL;
          for(incone= mem->INCONE ; incone ; incone = incone->NEXT){
              if((incone->TYPE & CNS_EXT) != CNS_EXT){
                  if (tas_ismemsym (incone->UEDGE.CONE)) {
                      for(inmemsym = incone->UEDGE.CONE->INCONE ; inmemsym ; inmemsym = inmemsym->NEXT){
                          if((inmemsym->TYPE & CNS_EXT) != CNS_EXT){
                              if(inmemsym->UEDGE.CONE == mem) {
                                  memsym = incone->UEDGE.CONE;
                                  delaymax_memsym_mem = (delay_list *)getptype(incone->USER,TAS_DELAY_MAX)->DATA ;
                                  delaymin_memsym_mem = (delay_list *)getptype(incone->USER,TAS_DELAY_MIN)->DATA ;
                                  break;
                              }
                          }   
                      }
                  }
              }
              if (memsym) break;
          }
 
          if (!memsym) continue;
          for(incone= mem->INCONE ; incone ; incone = incone->NEXT){
              delaymax_cmd_mem = NULL;
              delaymax_cmd_memsym = NULL;
              delaymin_cmd_mem = NULL;
              delaymin_cmd_memsym = NULL;
              if((incone->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                  delaymax_cmd_mem = (delay_list *)getptype(incone->USER,TAS_DELAY_MAX)->DATA ;
                  delaymin_cmd_mem = (delay_list *)getptype(incone->USER,TAS_DELAY_MIN)->DATA ;
                  cmd = incone;
                  
                  if ((cmd->TYPE & CNS_CONE) == CNS_CONE) cmd_name = cmd->UEDGE.CONE->NAME;
                  else cmd_name = cmd->UEDGE.LOCON->NAME;
           
                  for(inmemsym = memsym->INCONE ; inmemsym ; inmemsym = inmemsym->NEXT) {
                      if (inmemsym->UEDGE.PTR == cmd->UEDGE.PTR){
                          delaymax_cmd_memsym = (delay_list *)getptype(inmemsym->USER,TAS_DELAY_MAX)->DATA ;
                          delaymin_cmd_memsym = (delay_list *)getptype(inmemsym->USER,TAS_DELAY_MIN)->DATA ;
                      }
                  }
              } else {
                  continue;
              }
              for(i = 0; i < 2; i++){
                  if(!i){
                      delay_cmd_mem = delaymax_cmd_mem;
                      delay_cmd_memsym = delaymax_cmd_memsym;
                      delay_memsym_mem = delaymax_memsym_mem;
                      type = 'M';
                  } else {
                      delay_cmd_mem = delaymin_cmd_mem;
                      delay_cmd_memsym = delaymin_cmd_memsym;
                      delay_memsym_mem = delaymin_memsym_mem;
                      type = 'm';
                  }
                      
                  if (delay_cmd_mem && delay_cmd_memsym && delay_memsym_mem) {
                      if ((delay_cmd_mem->TPHL != TAS_NOTIME) && (delay_cmd_memsym->TPHL != TAS_NOTIME) 
                          && (delay_memsym_mem->TPLH != TAS_NOTIME)) {
                          delay_cmd_mem->TPHH = delay_cmd_memsym->TPHL + delay_memsym_mem->TPLH;
                          delay_cmd_mem->FHH = delay_memsym_mem->FLH ;
                          modname = stm_mod_name (cmd_name, 'U', mem->NAME, 'U', STM_DELAY|STM_SLEW, type, 0);
                          delay_cmd_mem->TMHH = stm_mod_create (modname);
                          tmodel = delay_cmd_mem->TMHH;
                          stm_mod_update (tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
                          stm_mod_update_transition (tmodel, STM_HH);
                          delay_cmd_mem->TMHH->UTYPE = STM_MOD_MODSCM;
                          csttab[0] = delay_cmd_mem->TPHH / TTV_UNIT;
                          csttab[1] = delay_cmd_mem->FHH / TTV_UNIT;
                          delay_cmd_mem->TMHH->UMODEL.SCM = stm_modscm_cst_create(csttab);
                          name = stm_storemodel(CELL, NULL, delay_cmd_mem->TMHH, 0);
                          if (name == delay_cmd_mem->TMHH->NAME)
                              DETAILED_MODELS = addchain (DETAILED_MODELS, name);
                          else {
                              stm_mod_destroy(delay_cmd_mem->TMHH);
                              delay_cmd_mem->TMHH = stm_getmodel(CELL, name);
                          }
#ifdef USEOLDTEMP
                          delay_cmd_mem->RHH = delay_memsym_mem->RLH ;
#endif
#ifdef USEOLDTEMP
                          delay_cmd_mem->SHH = delay_memsym_mem->SLH ;
#endif
                          delay_cmd_mem->FMHH = delay_cmd_mem->TMHH;
                          delay_memsym_mem->TPHL = TAS_NOTIME;
                          delay_memsym_mem->FHL = TAS_NOFRONT;
                          if(delay_memsym_mem->TMHL) stm_cell_delmodel (cell, delay_memsym_mem->TMHL, 0);
                          delay_memsym_mem->TMHL = NULL;
                      }
                      if ((delay_cmd_mem->TPLL != TAS_NOTIME) && (delay_cmd_memsym->TPLH != TAS_NOTIME) 
                          && (delay_memsym_mem->TPHL != TAS_NOTIME)) {
                          delay_cmd_mem->TPLL = delay_cmd_memsym->TPLH + delay_memsym_mem->TPHL;
                          delay_cmd_mem->FLL = delay_memsym_mem->FHL ;
                          modname = stm_mod_name (cmd_name, 'D', mem->NAME, 'D', STM_DELAY|STM_SLEW, type, 0);
                          delay_cmd_mem->TMLL = stm_mod_create (modname);
                          tmodel = delay_cmd_mem->TMLL;
                          stm_mod_update (tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
                          stm_mod_update_transition (tmodel, STM_LL);
                          delay_cmd_mem->TMLL->UTYPE = STM_MOD_MODSCM;
                          csttab[0] = delay_cmd_mem->TPLL / TTV_UNIT;
                          csttab[1] = delay_cmd_mem->FLL / TTV_UNIT;
                          delay_cmd_mem->TMLL->UMODEL.SCM = stm_modscm_cst_create(csttab);
                          name = stm_storemodel(CELL, NULL, delay_cmd_mem->TMLL, 0);
                          if (name == delay_cmd_mem->TMLL->NAME)
                              DETAILED_MODELS = addchain (DETAILED_MODELS, name);
                          else {
                              stm_mod_destroy(delay_cmd_mem->TMLL);
                              delay_cmd_mem->TMLL = stm_getmodel(CELL, name);
                          }
#ifdef USEOLDTEMP
                          delay_cmd_mem->RLL = delay_memsym_mem->RHL ;
#endif
#ifdef USEOLDTEMP
                          delay_cmd_mem->SLL = delay_memsym_mem->SHL ;
#endif
                          delay_cmd_mem->FMLL = delay_cmd_mem->TMLL;
                          delay_memsym_mem->TPLH = TAS_NOTIME;
                          delay_memsym_mem->FLH = TAS_NOFRONT;
                          if(delay_memsym_mem->TMLH) stm_cell_delmodel (cell, delay_memsym_mem->TMLH, 0);
                          delay_memsym_mem->TMLH = NULL;
                      }
                  }
                  if (delay_memsym_mem){
                      if ((delay_memsym_mem->TPHL != TAS_NOTIME) && (delay_memsym_mem->TPLH != TAS_NOTIME)){
                          delay_memsym_mem->TPHL = TAS_NOTIME;
                          delay_memsym_mem->FHL = TAS_NOFRONT;
                          if(delay_memsym_mem->TMHL) stm_cell_delmodel (cell, delay_memsym_mem->TMHL, 0);
                          delay_memsym_mem->TMHL = NULL;
                          delay_memsym_mem->TPLH = TAS_NOTIME;
                          delay_memsym_mem->FLH = TAS_NOFRONT;
                          if(delay_memsym_mem->TMLH) stm_cell_delmodel (cell, delay_memsym_mem->TMLH, 0);
                          delay_memsym_mem->TMLH = NULL;
                      }
                  }
              }
          }
      }
   }
}

/*****************************************************************************
*                                fonction tas_detectmemsym()                 *
*            detect les latch symetriques et adapte les entrees              *
*****************************************************************************/
void tas_detectmemsym(cnsfig)
cnsfig_list *cnsfig ;
{
 cone_list *cone ;
 cone_list *conein ;
 edge_list *incone ;
 edge_list *inconein ;
 edge_list *inconemem ;
 edge_list *inconemsym ;
 delay_list *delay ;
 delay_list *delayin ;
 delay_list *delaymem ;
 delay_list *delaymsym ;
 float csttab[2];
 char *name;
 char *modname;
 char *input_name;
 timing_model *tmodel;
 
  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   if(((cone->TYPE & (CNS_MEMSYM|CNS_LATCH)) != (CNS_MEMSYM|CNS_LATCH)) &&
      ((cone->TYPE & (CNS_MEMSYM|CNS_FLIP_FLOP)) != (CNS_MEMSYM|CNS_FLIP_FLOP)))
    continue ;
   
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {

    if((incone->TYPE&CNS_CONE)==CNS_CONE){
        if(incone->UEDGE.CONE->NAME)
            input_name = incone->UEDGE.CONE->NAME;
    }else if(incone->UEDGE.LOCON->NAME){ 
            input_name = incone->UEDGE.LOCON->NAME;
    }

        
     if((incone->TYPE & TAS_IN_MEMSYM) != TAS_IN_MEMSYM) continue ;
     conein = (cone_list *)getptype(incone->USER,TAS_IN_CONESYM)->DATA ;
     for(inconemsym = cone->INCONE ; inconemsym != NULL ;
         inconemsym = inconemsym->NEXT)
      if((inconemsym->TYPE & CNS_EXT) != CNS_EXT) 
       if(inconemsym->UEDGE.CONE == conein) break ;
     for(inconemem = conein->INCONE ; inconemem != NULL ;
         inconemem = inconemem->NEXT)
      if((inconemem->TYPE & CNS_EXT) != CNS_EXT) 
       if(inconemem->UEDGE.CONE == cone) break ;
     for(inconein = conein->INCONE ; inconein != NULL ;
         inconein = inconein->NEXT)
     if(inconein->UEDGE.PTR == incone->UEDGE.PTR) break ;
        inconein->TYPE |= CNS_FEEDBACK ;
     delay = (delay_list *)getptype(incone->USER,TAS_DELAY_MAX)->DATA ;
     delayin = (delay_list *)getptype(inconein->USER,TAS_DELAY_MAX)->DATA ;
     delaymem = (delay_list *)getptype(inconemem->USER,TAS_DELAY_MAX)->DATA ;
     delaymsym = (delay_list *)getptype(inconemsym->USER,TAS_DELAY_MAX)->DATA ;
     if(delayin->TPHL != TAS_NOTIME)
      {
       delay->TPHH = delayin->TPHL + delaymsym->TPLH ;
       delay->FHH = delaymsym->FLH ;
       modname = stm_mod_name(input_name, 'U', cone->NAME, 'U', STM_DELAY|STM_SLEW, 'M', 0);
       delay->TMHH = stm_mod_create(modname);
       tmodel = delay->TMHH;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
       stm_mod_update_transition(tmodel, STM_HH);
       delay->TMHH->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPHH / TTV_UNIT;
       csttab[1] = delay->FHH / TTV_UNIT;
       delay->TMHH->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMHH, 0);
       if (name == delay->TMHH->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMHH);
           delay->TMHH = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RHH = delaymsym->RLH ;
#endif
#ifdef USEOLDTEMP
       delay->SHH = delayin->SHL ;
#endif
       delay->FMHH = delay->TMHH ;
#ifdef USEOLDTEMP
       delaymem->RHL = delayin->RHL ;
#endif
#ifdef USEOLDTEMP
       delaymem->SHL = delayin->SHL ;
#endif
       delaymem->TMHL = delayin->TMHL ;
       delaymem->FHL = delayin->FHL ;
       delaymem->TPHL = delayin->TPHL ;
      }
     if(delayin->TPLH != TAS_NOTIME)
      {
       delay->TPLL = delayin->TPLH + delaymsym->TPHL ;
       delay->FLL = delaymsym->FHL ;
       modname = stm_mod_name(input_name, 'D', cone->NAME, 'D', STM_DELAY|STM_SLEW, 'M', 0);
       delay->TMLL = stm_mod_create(modname);
       tmodel = delay->TMLL;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
       stm_mod_update_transition(tmodel, STM_LL);
       delay->TMLL->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPLL / TTV_UNIT;
       csttab[1] = delay->FLL / TTV_UNIT;
       delay->TMLL->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMLL, 0);
       if (name == delay->TMLL->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMLL);
           delay->TMLL = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RLL = delaymsym->RHL ;
#endif
#ifdef USEOLDTEMP
       delay->SLL = delayin->SLH ;
#endif
       delay->FMLL = delay->TMLL ;
       delaymem->TPLH = delayin->TPLH ;
       delaymem->TMLH = delayin->TMLH ;
       delaymem->FLH = delayin->FLH ;
#ifdef USEOLDTEMP
       delaymem->RLH = delayin->RLH ;
#endif
#ifdef USEOLDTEMP
       delaymem->SLH = delayin->SLH ;
#endif
      }
     if(delayin->TPLL != TAS_NOTIME)
      {
       delay->TPLH = delayin->TPLL + delaymsym->TPLH ;
       delay->FLH = delaymsym->FLH ;
       modname = stm_mod_name(input_name, 'D', cone->NAME, 'U', STM_DELAY|STM_SLEW, 'M', 0);
       delay->TMLH = stm_mod_create(modname);
       tmodel = delay->TMLH;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
       stm_mod_update_transition(tmodel, STM_LH);
       delay->TMLH->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPLH / TTV_UNIT;
       csttab[1] = delay->FLH / TTV_UNIT;
       delay->TMLH->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMLH, 0);
       if (name == delay->TMLH->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMLH);
           delay->TMLH = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RLH = delaymsym->RLH ;
#endif
#ifdef USEOLDTEMP
       delay->SLH = delayin->SLL ;
#endif
       delay->FMLH = delay->TMLH ;
       delaymem->TPHL = delayin->TPLL ;
       delaymem->TMHL = delayin->TMLL ;
       delaymem->FHL = delayin->FLL ;
#ifdef USEOLDTEMP
       delaymem->RHL = delayin->RLL ;
#endif
#ifdef USEOLDTEMP
       delaymem->SHL = delayin->SLL ;
#endif
      }
     if(delayin->TPHH != TAS_NOTIME)
      {
       delay->TPHL = delayin->TPHH + delaymsym->TPHL ;
       delay->FHL = delaymsym->FHL ;
       modname = stm_mod_name(input_name, 'U', cone->NAME, 'D', STM_DELAY|STM_SLEW, 'M', 0);
       delay->TMHL = stm_mod_create(modname);
       tmodel = delay->TMHL;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
       stm_mod_update_transition(tmodel, STM_HL);
       delay->TMHL->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPHL / TTV_UNIT;
       csttab[1] = delay->FHL / TTV_UNIT;
       delay->TMHL->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMHL, 0);
       if (name == delay->TMHL->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMHL);
           delay->TMHL = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RHL = delaymsym->RHL ;
#endif
#ifdef USEOLDTEMP
       delay->SHL = delayin->SHH ;
#endif
       delay->FMHL = delay->TMHL ;
       delaymem->TPLH = delayin->TPHH ;
       delaymem->TMLH = delayin->TMHH ;
       delaymem->FLH = delayin->FHH ;
#ifdef USEOLDTEMP
       delaymem->RLH = delayin->RHH ;
#endif
#ifdef USEOLDTEMP
       delaymem->SLH = delayin->SHH ;
#endif
      }
     delay = (delay_list *)getptype(incone->USER,TAS_DELAY_MIN)->DATA ;
     delayin = (delay_list *)getptype(inconein->USER,TAS_DELAY_MIN)->DATA ;
     delaymem = (delay_list *)getptype(inconemem->USER,TAS_DELAY_MIN)->DATA ;
     delaymsym = (delay_list *)getptype(inconemsym->USER,TAS_DELAY_MIN)->DATA ;
     if(delayin->TPHL != TAS_NOTIME)
      {
       delay->TPHH = delayin->TPHL + delaymsym->TPLH ;
       delay->FHH = delaymsym->FLH ;
       modname = stm_mod_name(input_name, 'U', cone->NAME, 'U', STM_DELAY|STM_SLEW, 'm', 0);
       delay->TMHH = stm_mod_create(modname);
       tmodel = delay->TMHH;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
       stm_mod_update_transition(tmodel, STM_HH);
       delay->TMHH->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPHH / TTV_UNIT;
       csttab[1] = delay->FHH / TTV_UNIT;
       delay->TMHH->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMHH, 0);
       if (name == delay->TMHH->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMHH);
           delay->TMHH = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RHH = delaymsym->RLH ;
#endif
#ifdef USEOLDTEMP
       delay->SHH = delayin->SHL ;
#endif
       delay->FMHH = delay->TMHH ;
       delaymem->TPHL = delayin->TPHL ;
       delaymem->TMHL = delayin->TMHL ;
       delaymem->FHL = delayin->FHL ;
#ifdef USEOLDTEMP
       delaymem->RHL = delayin->RHL ;
#endif
#ifdef USEOLDTEMP
       delaymem->SHL = delayin->SHL ;
#endif
      }
     if(delayin->TPLH != TAS_NOTIME)
      {
       delay->TPLL = delayin->TPLH + delaymsym->TPHL ;
       delay->FLL = delaymsym->FHL ;
       modname = stm_mod_name(input_name, 'D', cone->NAME, 'D', STM_DELAY|STM_SLEW, 'm', 0);
       delay->TMLL = stm_mod_create(modname);
       tmodel = delay->TMLL;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
       stm_mod_update_transition(tmodel, STM_LL);
       delay->TMLL->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPLL / TTV_UNIT;
       csttab[1] = delay->FLL / TTV_UNIT;
       delay->TMLL->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMLL, 0);
       if (name == delay->TMLL->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMLL);
           delay->TMLL = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RLL = delaymsym->RHL ;
#endif
#ifdef USEOLDTEMP
       delay->SLL = delayin->SLH ;
#endif
       delay->FMLL = delay->TMLL ;
       delaymem->TPLH = delayin->TPLH ;
       delaymem->TMLH = delayin->TMLH ;
       delaymem->FLH = delayin->FLH ;
#ifdef USEOLDTEMP
       delaymem->RLH = delayin->RLH ;
#endif
#ifdef USEOLDTEMP
       delaymem->SLH = delayin->SLH ;
#endif
      }
     if(delayin->TPLL != TAS_NOTIME)
      {
       delay->TPLH = delayin->TPLL + delaymsym->TPLH ;
       delay->FLH = delaymsym->FLH ;
       modname = stm_mod_name(input_name, 'D', cone->NAME, 'U', STM_DELAY|STM_SLEW, 'm', 0);
       delay->TMLH = stm_mod_create(modname);
       tmodel = delay->TMLH;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
       stm_mod_update_transition(tmodel, STM_LH);
       delay->TMLH->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPLH / TTV_UNIT;
       csttab[1] = delay->FLH / TTV_UNIT;
       delay->TMLH->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMLH, 0);
       if (name == delay->TMLH->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMLH);
           delay->TMLH = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RLH = delaymsym->RLH ;
#endif
#ifdef USEOLDTEMP
       delay->SLH = delayin->SLL ;
#endif
       delay->FMLH = delay->TMLH ;
       delaymem->TPHL = delayin->TPLL ;
       delaymem->TMHL = delayin->TMLL ;
       delaymem->FHL = delayin->FLL ;
#ifdef USEOLDTEMP
       delaymem->RHL = delayin->RLL ;
#endif
#ifdef USEOLDTEMP
       delaymem->SHL = delayin->SLL ;
#endif
      }
     if(delayin->TPHH != TAS_NOTIME)
      {
       delay->TPHL = delayin->TPHH + delaymsym->TPHL ;
       delay->FHL = delaymsym->FHL ;
       modname = stm_mod_name(input_name, 'U', cone->NAME, 'D', STM_DELAY|STM_SLEW, 'm', 0);
       delay->TMHL = stm_mod_create(modname);
       tmodel = delay->TMHL;
       stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
       stm_mod_update_transition(tmodel, STM_HL);
       delay->TMHL->UTYPE = STM_MOD_MODSCM;
       csttab[0] = delay->TPHL / TTV_UNIT;
       csttab[1] = delay->FHL / TTV_UNIT;
       delay->TMHL->UMODEL.SCM = stm_modscm_cst_create(csttab);
       name = stm_storemodel(CELL, NULL, delay->TMHL, 0);
       if (name == delay->TMHL->NAME)
           DETAILED_MODELS = addchain (DETAILED_MODELS, name);
       else {
           stm_mod_destroy(delay->TMHL);
           delay->TMHL = stm_getmodel(CELL, name);
       }
#ifdef USEOLDTEMP
       delay->RHL = delaymsym->RHL ;
#endif
#ifdef USEOLDTEMP
       delay->SHL = delayin->SHH ;
#endif
       delay->FMHL = delay->TMHL ;
       delaymem->TPLH = delayin->TPHH ;
       delaymem->TMLH = delayin->TMHH ;
       delaymem->FLH = delayin->FHH ;
#ifdef USEOLDTEMP
       delaymem->RLH = delayin->RHH ;
#endif
#ifdef USEOLDTEMP
       delaymem->SLH = delayin->SHH ;
#endif
      }
    }
  }
}

/*****************************************************************************
*                                fonction tas_detectbuf()                    *
*            detect les buffers en parralleles dans les circuits             *
*****************************************************************************/
void tas_detectbuf(cnsfig)
cnsfig_list *cnsfig ;
{
 cone_list *cone ;
 edge_list *incone ;
 branch_list *branch[2] ;
 int nbr[2] ;
 int nbincone ;
 int i ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   cone->TYPE &= ~(TAS_CONE_INV | TAS_CONE_BUF) ;
   if(((cone->TECTYPE & CNS_CMOS) != CNS_CMOS) ||
      ((cone->TYPE & (CNS_CONFLICT|CNS_TRI|
                      CNS_MEMSYM|CNS_FLIP_FLOP|CNS_LATCH|CNS_RS|
                      CNS_MASTER|CNS_SLAVE)) != 0) ||
      ((cone->TECTYPE & (CNS_VDD_DEGRADED|CNS_VSS_DEGRADED|
                         CNS_ZERO|CNS_ONE)) != 0))
     continue ;
   if(cone->BREXT != NULL)
    {
     if(cone->BREXT->LINK->NEXT != NULL)
       continue ;
    }
   if(((branch[0] = cone->BRVDD) != NULL) &&
     ((branch[1] = cone->BRVSS) != NULL))
    {
     if((branch[0]->NEXT == NULL) && (branch[1]->NEXT == NULL))
      {
       if((incone = cone->INCONE) != NULL)
        {
         if((incone->NEXT == NULL) && ((incone->TYPE & CNS_CONE) == CNS_CONE))
          {
           cone->TYPE |= TAS_CONE_INV ;
          }
        }
      }
     else
      {
       for(i = 0 ; i < 2 ; i++)
        {
         nbr[i] = 0 ;
         for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
          {
           if(branch[i]->LINK == NULL) break ;
           if(branch[i]->LINK->NEXT != NULL) break ;
           nbr[i]++ ;
          }
         if(branch[i] != NULL)
           break ;
        }
       if((i == 2) && (nbr[0] == nbr[1]))
        {
         nbincone = 0 ;
         for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
          if((incone->TYPE & CNS_CONE) == CNS_CONE)
             nbincone++ ;
          else
             break ;
         if(nbincone == nbr[0])
           {
            cone->TYPE |= TAS_CONE_BUF ;
           }
        }
      }
    }
  }
}

/*****************************************************************************
*                        fonction tas_updateslope()                          *
*            mise a jour de la structure front                               *
*****************************************************************************/
void tas_updateslope(cone)
cone_list *cone ;
{
    front_list *slope;
    edge_list  *edge;
    delay_list *delay;
    int        rcfup=1;
    int        rcfdown=1;
    output_carac *carac;
    ptype_list *ptl;
                
    slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA;
    for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
        delay = (delay_list *)getptype(edge->USER,TAS_DELAY_MAX)->DATA;
        ptl = getptype( cone->USER, TAS_OUT_CARAC_MAX );
        if( ptl )
            carac = ( output_carac* ) ptl->DATA ;
        else {
            carac = tas_alloc_output_carac();
            cone->USER = addptype( cone->USER, TAS_OUT_CARAC_MAX, carac );
        }
        if(slope && delay){
            if((delay->FLH != TAS_NOFRONT) && (delay->FLH != 0)){
                if(rcfup){
#ifdef USEOLDTEMP             
                    slope->PWLUP = delay->PWLTPLH;
#endif
                    slope->FUP = delay->FLH;
                    if( delay->CARAC )
                      memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    rcfup = 0;
                }else if(slope->FUP < delay->FLH) {
#ifdef USEOLDTEMP             
                    slope->PWLUP = delay->PWLTPLH;
#endif
                    slope->FUP = delay->FLH;
                    if( delay->CARAC )
                      memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                }
            }
            if((delay->FHH != TAS_NOFRONT) && (delay->FHH != 0)){
                if(rcfup){
                    slope->PWLUP = NULL ;
                    slope->FUP = delay->FHH;
                    if( delay->CARAC )
                      memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    rcfup = 0;
                }else if(slope->FUP < delay->FHH) {
                    slope->PWLUP = NULL ;
                    slope->FUP = delay->FHH;
                    if( delay->CARAC )
                      memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                }
            }
            if((delay->FHL != TAS_NOFRONT) && (delay->FHL != 0)){
                if(rcfdown){
#ifdef USEOLDTEMP             
                    slope->PWLDN = delay->PWLTPHL;
#endif
                    slope->FDOWN = delay->FHL;
                    if( delay->CARAC )
                      memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    rcfdown = 0;
                }else if(slope->FDOWN < delay->FHL) {
#ifdef USEOLDTEMP             
                    slope->PWLDN = delay->PWLTPHL;
#endif
                    slope->FDOWN = delay->FHL;
                    if( delay->CARAC )
                      memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                }
            }
            if((delay->FLL != TAS_NOFRONT) && (delay->FLL != 0)){
                if(rcfdown){
                    slope->PWLDN = NULL;
                    slope->FDOWN = delay->FLL;
                    if( delay->CARAC )
                      memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    rcfdown = 0;
                }else if(slope->FDOWN < delay->FLL) {
                    slope->PWLDN = NULL;
                    slope->FDOWN = delay->FLL;
                    if( delay->CARAC )
                      memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                }
            }
        }
    }
    if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
        rcfup = 1;
        rcfdown = 1;
        slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA;
        for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
            delay = (delay_list *)getptype(edge->USER,TAS_DELAY_MIN)->DATA;
            ptl = getptype( cone->USER, TAS_OUT_CARAC_MIN );
            if( ptl )
                carac = ( output_carac* ) ptl->DATA ;
            else {
                carac = tas_alloc_output_carac();
                cone->USER = addptype( cone->USER, TAS_OUT_CARAC_MIN, carac );
            }
            if(slope && delay){
                if((delay->FLH != TAS_NOFRONT) && (delay->FLH != 0)){
                    if(rcfup){
#ifdef USEOLDTEMP             
                        slope->PWLUP = delay->PWLTPLH;
#endif
                        slope->FUP = delay->FLH;
                        if( delay->CARAC )
                              memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                        rcfup = 0;
                    }else if(slope->FUP > delay->FLH) {
#ifdef USEOLDTEMP             
                        slope->PWLUP = delay->PWLTPLH;
#endif
                        slope->FUP = delay->FLH;
                        if( delay->CARAC )
                              memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    }
                }
                if((delay->FHH != TAS_NOFRONT) && (delay->FHH != 0)){
                    if(rcfup){
                        slope->FUP = delay->FHH;
                        slope->PWLUP = NULL;
                        if( delay->CARAC )
                              memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                        rcfup = 0;
                    }else if(slope->FUP > delay->FHH) {
                        slope->FUP = delay->FHH;
                        slope->PWLUP = NULL;
                        if( delay->CARAC )
                              memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    }
                }
                if((delay->FHL != TAS_NOFRONT) && (delay->FHL != 0)){
                    if(rcfdown){
#ifdef USEOLDTEMP             
                        slope->PWLDN = delay->PWLTPHL;
#endif
                        slope->FDOWN = delay->FHL;
                        if( delay->CARAC )
                              memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                        rcfdown = 0;
                    }else if(slope->FDOWN > delay->FHL) {
#ifdef USEOLDTEMP             
                        slope->PWLDN = delay->PWLTPHL;
#endif
                        slope->FDOWN = delay->FHL;
                        if( delay->CARAC )
                              memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    }
                }
                if((delay->FLL != TAS_NOFRONT) && (delay->FLL != 0)){
                    if(rcfdown){
                        slope->PWLDN = NULL;
                        slope->FDOWN = delay->FLL;
                        if( delay->CARAC )
                              memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                        rcfdown = 0;
                    }else if(slope->FDOWN > delay->FLL) {
                        slope->PWLDN = NULL;
                        slope->FDOWN = delay->FLL;
                        if( delay->CARAC )
                              memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    }
                }
            }
        }
    }
}

/*****************************************************************************
*                        fonction tas_mark_links_unused_switch_command()     *
*****************************************************************************/
void tas_mark_links_unused_switch_command(cone, lotrs, level)
cone_list  *cone;
lotrs_list *lotrs;
long level;
{
    branch_list *path[3];
    link_list   *link;
    short i;
    
    path[0] = cone->BREXT;
    path[1] = cone->BRVDD;
    path[2] = cone->BRVSS;

    for(i = 0 ; i < 3 ; i++)
    for(; path[i] != NULL ; path[i] = path[i]->NEXT)
    if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
    if(((path[i]->TYPE & CNS_EXT) == CNS_EXT) || 
      ((path[i]->TYPE & level) == level)){
        for(link = (link_list *)path[i]->LINK ; link != NULL ; link = link->NEXT){
            if(link->ULINK.LOTRS == lotrs){
                if(!getptype(link->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)){
                    link->USER = addptype(link->USER, TAS_LINK_UNUSED_SWITCH_COMMAND, (void*)1);
                }
            }   
        }
    }
}

/*****************************************************************************
*                        fonction tas_choose_min_delay_switch()              *
*****************************************************************************/
void tas_choose_min_delay_switch(cone, edge, delay, d_in_down, d_in_up, type)
cone_list  *cone;
edge_list  *edge;
delay_list *delay;
long        d_in_down;
long        d_in_up;
char        type;
{
    link_list *link;
    lotrs_list *lotrs_other;
    ptype_list *ptype;
    cone_list *cone_avant;
    edge_list *edgeother;
    delay_list *delayother;
    timing_cell *cell = stm_getcell(CELL);
    long        d_down = 0;
    long        d_up = 0;
    delay_list *delaymin = NULL;
    delay_list *delayothermin = NULL;

    if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
      delaymin = (delay_list *)getptype(edge->USER,TAS_DELAY_MIN)->DATA;

    if(existe_tpd (cone, edge->UEDGE.CONE, CNS_VDD, CNS_TP, &link)){
      delayother = NULL ;
      delayothermin = NULL ;
      if((link->TYPE & CNS_SWITCH) == CNS_SWITCH){
        if((ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH)) != NULL){
          lotrs_other = (lotrs_list*)ptype->DATA;
          cone_avant = (cone_list *) getptype (((lotrs_list*)ptype->DATA)->USER, CNS_DRIVINGCONE)->DATA;
          if(cone_avant){
            for(edgeother = cone->INCONE ; edgeother != NULL ; edgeother = edgeother->NEXT){
              if((edgeother->TYPE & CNS_CONE) == CNS_CONE){
                if(edgeother->UEDGE.CONE == cone_avant){
                  if(type == 'M'){
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MAX)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }else{
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MIN)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }
                  if(type == 'M') delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MAX)->DATA;
                  else delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                    delayothermin = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  break;
                }
              }
            }
            if(delayother){
              if((delay->TPLH != TAS_NOTIME) && (delayother->TPHH != TAS_NOTIME)){
                if((d_in_down + delay->TPLH) < (d_up + delayother->TPHH)){
                  delayother->TPHH = TAS_NOTIME;
                  delayother->FHH = TAS_NOFRONT;
                  if(delayother->TMHH) stm_cell_delmodel (cell, delayother->TMHH, 0);
                  delayother->TMHH = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VDD);
                  if(delayothermin){
                    delayothermin->TPHH = TAS_NOTIME;
                    delayothermin->FHH = TAS_NOFRONT;
                    if(delayothermin->TMHH) stm_cell_delmodel (cell, delayothermin->TMHH, 0);
                    delayothermin->TMHH = NULL;
                  }
                }else{
                  delay->TPLH = TAS_NOTIME;
                  delay->FLH = TAS_NOFRONT;
                  if(delay->TMLH) stm_cell_delmodel (cell, delay->TMLH, 0);
                  delay->TMLH = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VDD);
                  if(delaymin){
                    delaymin->TPLH = TAS_NOTIME;
                    delaymin->FLH = TAS_NOFRONT;
                    if(delaymin->TMLH) stm_cell_delmodel (cell, delaymin->TMLH, 0);
                    delaymin->TMLH = NULL;
                  }                    
                }
              }
              if((delay->TPHL != TAS_NOTIME) && (delay->FHL == 0) && (delayother->TPLL != TAS_NOTIME) && (delayother->FLL == 0)){
                if((d_in_up + delay->TPHL) < (d_down + delayother->TPLL)){
                  delayother->TPLL = TAS_NOTIME;
                  delayother->FLL = TAS_NOFRONT;
                  if(delayother->TMLL) stm_cell_delmodel (cell, delayother->TMLL, 0);
                  delayother->TMLL = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VSS);
                  if(delayothermin){
                    delayothermin->TPLL = TAS_NOTIME;
                    delayothermin->FLL = TAS_NOFRONT;
                    if(delayothermin->TMLL) stm_cell_delmodel (cell, delayothermin->TMLL, 0);
                    delayothermin->TMLL = NULL;
                  }
                }else{
                  delay->TPHL = TAS_NOTIME;
                  delay->FHL = TAS_NOFRONT;
                  if(delay->TMHL) stm_cell_delmodel (cell, delay->TMHL, 0);
                  delay->TMHL = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VSS);
                  if(delaymin){
                    delaymin->TPHL = TAS_NOTIME;
                    delaymin->FHL = TAS_NOFRONT;
                    if(delaymin->TMHL) stm_cell_delmodel (cell, delaymin->TMHL, 0);
                    delaymin->TMHL = NULL;
                  }
                }
              }
            }
          }
        }
      }
    }
    if(existe_tpd (cone, edge->UEDGE.CONE, CNS_VDD, CNS_TN, &link)){
      delayother = NULL ;
      delayothermin = NULL ;
      if((link->TYPE & CNS_SWITCH) == CNS_SWITCH){
        if((ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH)) != NULL){
          lotrs_other = (lotrs_list*)ptype->DATA;
          cone_avant = (cone_list *) getptype (((lotrs_list*)ptype->DATA)->USER, CNS_DRIVINGCONE)->DATA;
          if(cone_avant){
            for(edgeother = cone->INCONE ; edgeother != NULL ; edgeother = edgeother->NEXT){
              if((edgeother->TYPE & CNS_CONE) == CNS_CONE){
                if(edgeother->UEDGE.CONE == cone_avant){
                  if(type == 'M'){
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MAX)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }else{
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MIN)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }
                  if(type == 'M') delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MAX)->DATA;
                  else delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                    delayothermin = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  break;
                }
              }
            }
            if(delayother){
              if((delay->TPHH != TAS_NOTIME) && (delayother->TPLH != TAS_NOTIME)){
                if((d_in_up + delay->TPHH) < (d_down + delayother->TPLH)){
                  delayother->TPLH = TAS_NOTIME;
                  delayother->FLH = TAS_NOFRONT;
                  if(delayother->TMLH) stm_cell_delmodel (cell, delayother->TMLH, 0);
                  delayother->TMLH = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VDD);
                  if(delayothermin){
                    delayothermin->TPLH = TAS_NOTIME;
                    delayothermin->FLH = TAS_NOFRONT;
                    if(delayothermin->TMLH) stm_cell_delmodel (cell, delayothermin->TMLH, 0);
                    delayothermin->TMLH = NULL;
                  }
                }else{
                  delay->TPHH = TAS_NOTIME;
                  delay->FHH = TAS_NOFRONT;
                  if(delay->TMHH) stm_cell_delmodel (cell, delay->TMHH, 0);
                  delay->TMHH = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VDD);
                  if(delaymin){
                    delaymin->TPHH = TAS_NOTIME;
                    delaymin->FHH = TAS_NOFRONT;
                    if(delaymin->TMHH) stm_cell_delmodel (cell, delaymin->TMHH, 0);
                    delaymin->TMHH = NULL;
                  }
                }
              }
              if((delay->TPLL != TAS_NOTIME) && (delay->FLL == 0) && (delayother->TPHL != TAS_NOTIME) && (delayother->FHL == 0)){
                if((d_in_down + delay->TPLL) < (d_up + delayother->TPHL)){
                  delayother->TPHL = TAS_NOTIME;
                  delayother->FHL = TAS_NOFRONT;
                  if(delayother->TMHL) stm_cell_delmodel (cell, delayother->TMHL, 0);
                  delayother->TMHL = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VSS);
                  if(delayothermin){
                    delayothermin->TPHL = TAS_NOTIME;
                    delayothermin->FHL = TAS_NOFRONT;
                    if(delayothermin->TMHL) stm_cell_delmodel (cell, delayothermin->TMHL, 0);
                    delayothermin->TMHL = NULL;
                  }
                }else{
                  delay->TPLL = TAS_NOTIME;
                  delay->FLL = TAS_NOFRONT;
                  if(delay->TMLL) stm_cell_delmodel (cell, delay->TMLL, 0);
                  delay->TMLL = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VSS);
                  if(delaymin){
                    delaymin->TPLL = TAS_NOTIME;
                    delaymin->FLL = TAS_NOFRONT;
                    if(delaymin->TMLL) stm_cell_delmodel (cell, delaymin->TMLL, 0);
                    delaymin->TMLL = NULL;
                  }
                }
              }
            }
          }
        }
      }
    }
    if(existe_tpd (cone, edge->UEDGE.CONE, CNS_VSS, CNS_TP, &link)){
      delayother = NULL ;
      delayothermin = NULL ;
      if((link->TYPE & CNS_SWITCH) == CNS_SWITCH){
        if((ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH)) != NULL){
          lotrs_other = (lotrs_list*)ptype->DATA;
          cone_avant = (cone_list *) getptype (((lotrs_list*)ptype->DATA)->USER, CNS_DRIVINGCONE)->DATA;
          if(cone_avant){
            for(edgeother = cone->INCONE ; edgeother != NULL ; edgeother = edgeother->NEXT){
              if((edgeother->TYPE & CNS_CONE) == CNS_CONE){
                if(edgeother->UEDGE.CONE == cone_avant){
                  if(type == 'M'){
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MAX)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }else{
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MIN)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }
                  if(type == 'M') delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MAX)->DATA;
                  else delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                    delayothermin = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  break;
                }
              }
            }
            if(delayother){
              if((delay->TPLL != TAS_NOTIME) && (delayother->TPHL != TAS_NOTIME)){
                if((d_in_down + delay->TPLL) < (d_up + delayother->TPHL)){
                  delayother->TPHL = TAS_NOTIME;
                  delayother->FHL = TAS_NOFRONT;
                  if(delayother->TMHL) stm_cell_delmodel (cell, delayother->TMHL, 0);
                  delayother->TMHL = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VSS);
                  if(delayothermin){
                    delayothermin->TPHL = TAS_NOTIME;
                    delayothermin->FHL = TAS_NOFRONT;
                    if(delayothermin->TMHL) stm_cell_delmodel (cell, delayothermin->TMHL, 0);
                    delayothermin->TMHL = NULL;
                  }
                }else{
                  delay->TPLL = TAS_NOTIME;
                  delay->FLL = TAS_NOFRONT;
                  if(delay->TMLL) stm_cell_delmodel (cell, delay->TMLL, 0);
                  delay->TMLL = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VSS);
                  if(delaymin){
                    delaymin->TPLL = TAS_NOTIME;
                    delaymin->FLL = TAS_NOFRONT;
                    if(delaymin->TMLL) stm_cell_delmodel (cell, delaymin->TMLL, 0);
                    delaymin->TMLL = NULL;
                  }
                }
              }
              if((delay->TPHH != TAS_NOTIME) && (delay->FHH == 0) && (delayother->TPLH != TAS_NOTIME) && (delayother->FLH == 0)){
                if((d_in_up + delay->TPHH) < (d_down + delayother->TPLH)){
                  delayother->TPLH = TAS_NOTIME;
                  delayother->FLH = TAS_NOFRONT;
                  if(delayother->TMLH) stm_cell_delmodel (cell, delayother->TMLH, 0);
                  delayother->TMLH = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VDD);
                  if(delayothermin){
                    delayothermin->TPLH = TAS_NOTIME;
                    delayothermin->FLH = TAS_NOFRONT;
                    if(delayothermin->TMLH) stm_cell_delmodel (cell, delayothermin->TMLH, 0);
                    delayothermin->TMLH = NULL;
                  }
                }else{
                  delay->TPHH = TAS_NOTIME;
                  delay->FHH = TAS_NOFRONT;
                  if(delay->TMHH) stm_cell_delmodel (cell, delay->TMHH, 0);
                  delay->TMHH = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VDD);
                  if(delaymin){
                    delaymin->TPHH = TAS_NOTIME;
                    delaymin->FHH = TAS_NOFRONT;
                    if(delaymin->TMHH) stm_cell_delmodel (cell, delaymin->TMHH, 0);
                    delaymin->TMHH = NULL;
                  }
                }
              }
            }
          }
        }
      }
    }
    if(existe_tpd (cone, edge->UEDGE.CONE, CNS_VSS, CNS_TN, &link)){
      delayother = NULL ;
      delayothermin = NULL ;
      if((link->TYPE & CNS_SWITCH) == CNS_SWITCH){
        if((ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH)) != NULL){
          lotrs_other = (lotrs_list*)ptype->DATA;
          cone_avant = (cone_list *) getptype (((lotrs_list*)ptype->DATA)->USER, CNS_DRIVINGCONE)->DATA;
          if(cone_avant){
            for(edgeother = cone->INCONE ; edgeother != NULL ; edgeother = edgeother->NEXT){
              if((edgeother->TYPE & CNS_CONE) == CNS_CONE){
                if(edgeother->UEDGE.CONE == cone_avant){
                  if(type == 'M'){
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MAX)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }else{
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_UP_MIN)) != NULL)
                          d_up = (long)ptype->DATA; 
                      if((ptype = getptype(edgeother->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN)) != NULL)
                          d_down = (long)ptype->DATA; 
                  }
                  if(type == 'M') delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MAX)->DATA;
                  else delayother = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                    delayothermin = (delay_list *)getptype(edgeother->USER,TAS_DELAY_MIN)->DATA;
                  break;
                }
              }
            }
            if(delayother){
              if((delay->TPHL != TAS_NOTIME) && (delayother->TPLL != TAS_NOTIME)){
                if((d_in_up + delay->TPHL) < (d_down + delayother->TPLL)){
                  delayother->TPLL = TAS_NOTIME;
                  delayother->FLL = TAS_NOFRONT;
                  if(delayother->TMLL) stm_cell_delmodel (cell, delayother->TMLL, 0);
                  delayother->TMLL = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VSS);
                  if(delayothermin){
                    delayothermin->TPLL = TAS_NOTIME;
                    delayothermin->FLL = TAS_NOFRONT;
                    if(delayothermin->TMLL) stm_cell_delmodel (cell, delayothermin->TMLL, 0);
                    delayothermin->TMLL = NULL;
                  }
                }else{
                  delay->TPHL = TAS_NOTIME;
                  delay->FHL = TAS_NOFRONT;
                  if(delay->TMHL) stm_cell_delmodel (cell, delay->TMHL, 0);
                  delay->TMHL = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VSS);
                  if(delaymin){
                    delaymin->TPHL = TAS_NOTIME;
                    delaymin->FHL = TAS_NOFRONT;
                    if(delaymin->TMHL) stm_cell_delmodel (cell, delaymin->TMHL, 0);
                    delaymin->TMHL = NULL;
                  }
                }
              }
              if((delay->TPLH != TAS_NOTIME) && (delay->FLH == 0) && (delayother->TPHH != TAS_NOTIME) && (delayother->FHH == 0)){
                if((d_in_down + delay->TPLH) < (d_up + delayother->TPHH)){
                  delayother->TPHH = TAS_NOTIME;
                  delayother->FHH = TAS_NOFRONT;
                  if(delayother->TMHH) stm_cell_delmodel (cell, delayother->TMHH, 0);
                  delayother->TMHH = NULL;
                  tas_mark_links_unused_switch_command(cone, lotrs_other, CNS_VDD);
                  if(delayothermin){
                    delayothermin->TPHH = TAS_NOTIME;
                    delayothermin->FHH = TAS_NOFRONT;
                    if(delayothermin->TMHH) stm_cell_delmodel (cell, delayothermin->TMHH, 0);
                    delayothermin->TMHH = NULL;
                  }
                }else{
                  delay->TPLH = TAS_NOTIME;
                  delay->FLH = TAS_NOFRONT;
                  if(delay->TMLH) stm_cell_delmodel (cell, delay->TMLH, 0);
                  delay->TMLH = NULL;
                  tas_mark_links_unused_switch_command(cone, link->ULINK.LOTRS, CNS_VDD);
                  if(delaymin){
                    delaymin->TPLH = TAS_NOTIME;
                    delaymin->FLH = TAS_NOFRONT;
                    if(delaymin->TMLH) stm_cell_delmodel (cell, delaymin->TMLH, 0);
                    delaymin->TMLH = NULL;
                  }
                }
              }
            }
          }
        }
      }
    }
}
/*****************************************************************************
*                        fonction tas_updatedelay()                          *
*            mise a jour de la structure front                               *
*****************************************************************************/
void tas_updatedelay(cone)
cone_list *cone ;
{
    front_list *slope;
    edge_list  *edge;
    delay_list *delay;
    int        rcfup=1;
    int        rcfdown=1;
    long       d_in_up = 0;
    long       d_in_down = 0;
    long       rchh = 0;
    long       rcll = 0;
    long       delay_up_max = 0;
    long       delay_down_max = 0;
    long       delay_up_min = 0;
    long       delay_down_min = 0;
    output_carac *carac ;
    ptype_list   *ptl ;
    
    slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA;
    if((TAS_CONTEXT->TAS_DELAY_SWITCH == 'P') && (getptype(cone->USER,CNS_SWITCH))){
        for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
            d_in_up = 0;
            d_in_down = 0;
            delay = (delay_list *)getptype(edge->USER,TAS_DELAY_MAX)->DATA;
            if(((edge->TYPE & CNS_CONE) == CNS_CONE) && (slope && delay)){
                if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MAX))
                    d_in_up = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MAX)->DATA;
                if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX))
                    d_in_down = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX)->DATA;
                tas_choose_min_delay_switch(cone, edge, delay, d_in_down, d_in_up, 'M');
            }
        }
    }
    for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
        d_in_up = 0;
        d_in_down = 0;
        delay = (delay_list *)getptype(edge->USER,TAS_DELAY_MAX)->DATA;
        ptl = getptype( cone->USER, TAS_OUT_CARAC_MAX );
        if( ptl )
            carac = ( output_carac* ) ptl->DATA ;
        else {
            carac = tas_alloc_output_carac();
            cone->USER = addptype( cone->USER, TAS_OUT_CARAC_MAX, carac );
        }
        if((edge->TYPE & CNS_CONE) == CNS_CONE){
            if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MAX))
                d_in_up = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MAX)->DATA;
            if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX))
                d_in_down = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MAX)->DATA;
        }
        if(delay->RCLL != TAS_NOTIME)
            rcll = delay->RCLL;
        else
            rcll = 0;
        if(delay->RCHH != TAS_NOTIME)
            rchh = delay->RCHH;
        else
            rchh = 0;
        if(slope && delay){
            if((delay->TPLH != TAS_NOTIME) && (delay->FLH != TAS_NOFRONT) && (delay->FLH != 0)){
                if(rcfup){
                    slope->FUP = delay->FLH;
#ifdef USEOLDTEMP             
                    slope->PWLUP = delay->PWLTPLH;
#endif
                    stm_mod_driver( delay->FMLH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                    delay_up_max = delay->TPLH + d_in_down + rcll;
                    if( delay->CARAC )
                        memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    rcfup = 0;
                }else if(delay_up_max < delay->TPLH + d_in_down + rcll){
                    slope->FUP = delay->FLH;
#ifdef USEOLDTEMP             
                    slope->PWLUP = delay->PWLTPLH;
#endif
                    stm_mod_driver( delay->FMLH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                    delay_up_max = delay->TPLH + d_in_down + rcll;
                    if( delay->CARAC )
                        memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                }
            }
            if((delay->TPHH != TAS_NOTIME) && (delay->FHH != TAS_NOFRONT) && (delay->FHH != 0)){
                if(rcfup){
                    slope->FUP = delay->FHH;
                    slope->PWLUP = NULL ;
                    stm_mod_driver( delay->FMHH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                    delay_up_max = delay->TPHH + d_in_up + rchh;
                    if( delay->CARAC )
                        memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    rcfup = 0;
                }else if(delay_up_max < delay->TPHH + d_in_up + rchh){
                    slope->FUP = delay->FHH;
                    slope->PWLUP = NULL ;
                    stm_mod_driver( delay->FMHH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                    delay_up_max = delay->TPHH + d_in_up + rchh;
                    if( delay->CARAC )
                        memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                }
            }
            if((delay->TPHL != TAS_NOTIME) && (delay->FHL != TAS_NOFRONT) && (delay->FHL != 0)){
                if(rcfdown){
                    slope->FDOWN = delay->FHL;
#ifdef USEOLDTEMP             
                    slope->PWLDN = delay->PWLTPHL;
#endif
                    stm_mod_driver( delay->FMHL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                    delay_down_max = delay->TPHL + d_in_up + rchh;
                    if( delay->CARAC )
                        memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    rcfdown = 0;
                }else if(delay_down_max < delay->TPHL + d_in_up + rchh){
                    slope->FDOWN = delay->FHL;
#ifdef USEOLDTEMP             
                    slope->PWLDN = delay->PWLTPHL;
#endif
                    stm_mod_driver( delay->FMHL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                    delay_down_max = delay->TPHL + d_in_up + rchh;
                    if( delay->CARAC )
                        memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                }
            }
            if((delay->TPLL != TAS_NOTIME) && (delay->FLL != TAS_NOFRONT) && (delay->FLL != 0)){
                if(rcfdown){
                    slope->FDOWN = delay->FLL;
                    slope->PWLDN = NULL;
                    stm_mod_driver( delay->FMLL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                    delay_down_max = delay->TPLL + d_in_down + rcll;
                    if( delay->CARAC )
                        memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    rcfdown = 0;
                }else if(delay_down_max < delay->TPLL + d_in_down + rcll){
                    slope->FDOWN = delay->FLL;
                    slope->PWLDN = NULL;
                    stm_mod_driver( delay->FMLL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                    delay_down_max = delay->TPLL + d_in_down + rcll;
                    if( delay->CARAC )
                        memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                }
            }
        }
    }
    cone->USER = addptype(cone->USER, TAS_DELAY_UP_MAX, (void*)delay_up_max);
    cone->USER = addptype(cone->USER, TAS_DELAY_DOWN_MAX, (void*)delay_down_max);
    
    if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
        rcfup = 1;
        rcfdown = 1;
        slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA;
/*        if((TAS_CONTEXT->TAS_DELAY_SWITCH == 'P') && (getptype(cone->USER,CNS_SWITCH))){
            for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
                d_in_up = 0;
                d_in_down = 0;
                delay = (delay_list *)getptype(edge->USER,TAS_DELAY_MIN)->DATA;
                if(((edge->TYPE & CNS_CONE) == CNS_CONE) && (slope && delay)){
                    if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MIN))
                        d_in_up = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MIN)->DATA;
                    if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN))
                        d_in_down = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN)->DATA;
                    tas_choose_min_delay_switch(cone, edge, delay, d_in_down, d_in_up, 'm');
                }
            }
        }*/
        for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
            d_in_up = 0;
            d_in_down = 0;
            delay = (delay_list *)getptype(edge->USER,TAS_DELAY_MIN)->DATA;
            ptl = getptype( cone->USER, TAS_OUT_CARAC_MIN );
            if( ptl )
                carac = ( output_carac* ) ptl->DATA ;
            else {
                carac = tas_alloc_output_carac();
                cone->USER = addptype( cone->USER, TAS_OUT_CARAC_MIN, carac );
            }
            if((edge->TYPE & CNS_CONE) == CNS_CONE){
                if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MIN))
                    d_in_up = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_UP_MIN)->DATA;
                if(getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN))
                    d_in_down = (long)getptype(edge->UEDGE.CONE->USER, TAS_DELAY_DOWN_MIN)->DATA;
            }
            if(delay->RCLL != TAS_NOTIME)
                rcll = delay->RCLL;
            else
                rcll = 0;
            if(delay->RCHH != TAS_NOTIME)
                rchh = delay->RCHH;
            else
                rchh = 0;
            if(slope && delay){
                if((delay->TPLH != TAS_NOTIME) && (delay->FLH != TAS_NOFRONT) && (delay->FLH != 0)){
                    if(rcfup){
                        slope->FUP = delay->FLH;
#ifdef USEOLDTEMP             
                        slope->PWLUP = delay->PWLTPLH;
#endif
                        stm_mod_driver( delay->FMLH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                        delay_up_min = delay->TPLH + d_in_down + rcll;
                        if( delay->CARAC )
                            memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                        rcfup = 0;
                    }else if(delay_up_min > delay->TPLH + d_in_down + rcll){
                        slope->FUP = delay->FLH;
#ifdef USEOLDTEMP             
                        slope->PWLUP = delay->PWLTPLH;
#endif
                        stm_mod_driver( delay->FMLH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                        delay_up_min = delay->TPLH + d_in_down + rcll;
                        if( delay->CARAC )
                            memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    }
                }
                if((delay->TPHH != TAS_NOTIME) && (delay->FHH != TAS_NOFRONT) && (delay->FHH != 0)){
                    if(rcfup){
                        slope->FUP = delay->FHH;
                        slope->PWLUP = NULL ;
                        stm_mod_driver( delay->FMHH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                        delay_up_min = delay->TPHH + d_in_up + rchh;
                        if( delay->CARAC )
                            memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                        rcfup = 0;
                    }else if(delay_up_min > delay->TPHH + d_in_up + rchh){
                        slope->FUP = delay->FHH;
                        slope->PWLUP = NULL ;
                        stm_mod_driver( delay->FMHH, &slope->DRIVERUP.R, &slope->DRIVERUP.V );
                        delay_up_min = delay->TPHH + d_in_up + rchh;
                        if( delay->CARAC )
                            memcpy( &(carac->UP), &(delay->CARAC->UP), sizeof( output_carac_trans ) );
                    }
                }
                if((delay->TPHL != TAS_NOTIME) && (delay->FHL != TAS_NOFRONT) && (delay->FHL != 0)){
                    if(rcfdown){
                        slope->FDOWN = delay->FHL;
#ifdef USEOLDTEMP             
                        slope->PWLDN = delay->PWLTPHL;
#endif
                        stm_mod_driver( delay->FMHL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                        delay_down_min = delay->TPHL + d_in_up + rchh;
                        if( delay->CARAC )
                            memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                        rcfdown = 0;
                    }else if(delay_down_min > delay->TPHL + d_in_up + rchh){
                        slope->FDOWN = delay->FHL;
#ifdef USEOLDTEMP             
                        slope->PWLDN = delay->PWLTPHL;
#endif
                        stm_mod_driver( delay->FMHL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                        delay_down_min = delay->TPHL + d_in_up + rchh;
                        if( delay->CARAC )
                            memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    }
                }
                if((delay->TPLL != TAS_NOTIME) && (delay->FLL != TAS_NOFRONT) && (delay->FLL != 0)){
                    if(rcfdown){
                        slope->FDOWN = delay->FLL;
                        slope->PWLDN = NULL;
                        stm_mod_driver( delay->FMLL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                        delay_down_min = delay->TPLL + d_in_down + rcll;
                        if( delay->CARAC )
                            memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                        rcfdown = 0;
                    }else if(delay_down_min > delay->TPLL + d_in_down + rcll){
                        slope->FDOWN = delay->FLL;
                        slope->PWLDN = NULL;
                        stm_mod_driver( delay->FMLL, &slope->DRIVERDN.R, &slope->DRIVERDN.V );
                        delay_down_min = delay->TPLL + d_in_down + rcll;
                        if( delay->CARAC )
                            memcpy( &(carac->DW), &(delay->CARAC->DW), sizeof( output_carac_trans ) );
                    }
                }
            }
        }
        cone->USER = addptype(cone->USER, TAS_DELAY_UP_MIN, (void*)delay_up_min);
        cone->USER = addptype(cone->USER, TAS_DELAY_DOWN_MIN, (void*)delay_down_min);
    }
//    printf("\ncone=%s   delayupmax=%ld   delaydownmax=%ld\n", cone->NAME, delay_up_max, delay_down_max);
//    printf("cone=%s   delayupmin=%ld   delaydownmin=%ld\n", cone->NAME, delay_up_min, delay_down_min);
}

/*****************************************************************************
*                                fonction tas_delaycone()                    *
*            fonction recursive de propagation des fronts                    *
*****************************************************************************/
#ifdef DELAY_DEBUG_STAT
  static int nbtot, nbcur;
  static long TM;
#endif
long tas_delaycone(ifl,cone,nbfront,counter)
inffig_list *ifl;
cone_list *cone ;
long nbfront;
long counter;
{
 edge_list *edge ;
 ptype_list *ptype ;

 if((cone->TYPE & (CNS_VSS|CNS_VDD|TAS_MARQUE)) != 0)
   return (nbfront);

 if(((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP|CNS_PRECHARGE)) != 0) && (counter > 10000))
   return (nbfront);

 if(/*((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) == 0) &&*/ !(getptype(cone->USER, TAS_CONE_MARQUE)))
  {
   for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT)
     {
      if((edge->TYPE & (CNS_EXT | CNS_FEEDBACK | CNS_VDD | CNS_VSS | /*CNS_LOOP |*/
                         /*CNS_COMMAND |*/ TAS_PATH_INOUT | CNS_BLEEDER)) == 0)
        {
         cone->USER = addptype(cone->USER, TAS_CONE_MARQUE, (void*)1) ;
         nbfront = tas_delaycone(ifl,edge->UEDGE.CONE,nbfront,counter+1) ;
         cone->USER = delptype(cone->USER,TAS_CONE_MARQUE) ;
       }
      else if((edge->TYPE & CNS_EXT) == CNS_EXT)
       {
        if((ptype = getptype(edge->UEDGE.LOCON->USER,CNS_EXT)) != NULL)
          {
           if(cone != (cone_list *)ptype->DATA){
           cone->USER = addptype(cone->USER, TAS_CONE_MARQUE, (void*)1) ;
           nbfront = tas_delaycone(ifl,(cone_list *)ptype->DATA,nbfront,counter+1) ;
           cone->USER = delptype(cone->USER,TAS_CONE_MARQUE) ;
           }
          }
       } 
     }
   }
/* else
     return (nbfront) ;*//* pour ne pas calculer 2 fois le meme cone */

 if(TAS_CONTEXT->TAS_CALCRCX == 'Y')
     nbfront = tas_tprc_ext(ifl,cone,nbfront);
 tas_tpd(cone) ;
 if((cone->TYPE & TAS_CONE_BUF) == TAS_CONE_BUF)
     tas_buftoinv(cone) ;
 if((TAS_CONTEXT->TAS_DELAY_PROP == 'N') && (TAS_CONTEXT->TAS_NO_PROP == 'N') && ((cone->TYPE & (TAS_BYPASSIN|TAS_BYPASSOUT)) == 0))
     tas_updateslope(cone);
 else if((TAS_CONTEXT->TAS_NO_PROP == 'N') && ((cone->TYPE & (TAS_BYPASSIN|TAS_BYPASSOUT)) == 0))
     tas_updatedelay(cone);
 if(TAS_CONTEXT->TAS_CALCRCX == 'Y')
     nbfront = tas_tprc_cone(ifl,cone, nbfront);
 cone->TYPE |= TAS_MARQUE;

 tas_clean_carac_cone( cone );
#ifdef DELAY_DEBUG_STAT
 nbcur++;
 mbk_DisplayProgress(&TM, "delaycone:", nbcur, nbtot, ' ');
#endif
 return(nbfront) ;
}

/*****************************************************************************
*                                fonction tas_scm2tables()                   *
*****************************************************************************/
void tas_scm2tables(timing_cell *cell, edge_list *input, ht *htd, ht *hts, delay_list *delay, front_list *slope, float capa)
{
    timing_model *tmodels, *tmodeld, *tmodelscm;
    char *scmname;
    float slew;

    if(delay->TMHL){
        if(!slope) slew = -1.0;
        else slew = slope->FUP/TTV_UNIT;
        tmodeld = (timing_model*)gethtitem(htd, delay->TMHL->NAME);
        tmodels = (timing_model*)gethtitem(hts, delay->FMHL->NAME);
        if((long)tmodeld != EMPTYHT)
            delay->TMHL = tmodeld;
        else{
            scmname = delay->TMHL->NAME;
            tmodelscm = delay->TMHL;
            tmodels = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_slew (tmodels, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodels, 0);
            stm_addreferences(cell->NAME, tmodels->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            tmodeld = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_delay (tmodeld, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodeld, 0);
            stm_addreferences(cell->NAME, tmodeld->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            addhtitem(htd, scmname, (long)tmodeld);
            addhtitem(hts, scmname, (long)tmodels);
            delay->TMHL = tmodeld;
        }
        delay->FMHL = tmodels;
    }
    if(delay->TMLH){
        if(!slope) slew = -1.0;
        else slew = slope->FDOWN/TTV_UNIT;
        tmodeld = (timing_model*)gethtitem(htd, delay->TMLH->NAME);
        tmodels = (timing_model*)gethtitem(hts, delay->FMLH->NAME);
        if((long)tmodeld != EMPTYHT)
            delay->TMLH = tmodeld;
        else{
            scmname = delay->TMLH->NAME;
            tmodelscm = delay->TMLH;
            tmodels = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_slew (tmodels, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodels, 0);
            stm_addreferences(cell->NAME, tmodels->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            tmodeld = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_delay (tmodeld, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodeld, 0);
            stm_addreferences(cell->NAME, tmodeld->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            addhtitem(htd, scmname, (long)tmodeld);
            addhtitem(hts, scmname, (long)tmodels);
            delay->TMLH = tmodeld;
        }
        delay->FMLH = tmodels;
    }
    if(delay->TMLL){
        if(!slope) slew = -1.0;
        else slew = slope->FDOWN/TTV_UNIT;
        tmodeld = (timing_model*)gethtitem(htd, delay->TMLL->NAME);
        tmodels = (timing_model*)gethtitem(hts, delay->FMLL->NAME);
        if((long)tmodeld != EMPTYHT)
            delay->TMLL = tmodeld;
        else{
            scmname = delay->TMLL->NAME;
            tmodelscm = delay->TMLL;
            tmodels = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_slew (tmodels, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodels, 0);
            stm_addreferences(cell->NAME, tmodels->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            tmodeld = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_delay (tmodeld, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodeld, 0);
            stm_addreferences(cell->NAME, tmodeld->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            addhtitem(htd, scmname, (long)tmodeld);
            addhtitem(hts, scmname, (long)tmodels);
            delay->TMLL = tmodeld;
        }
        delay->FMLL = tmodels;
    }
    if(delay->TMHH){
        if(!slope) slew = -1.0;
        else slew = slope->FUP/TTV_UNIT;
        tmodeld = (timing_model*)gethtitem(htd, delay->TMHH->NAME);
        tmodels = (timing_model*)gethtitem(hts, delay->FMHH->NAME);
        if((long)tmodeld != EMPTYHT)
            delay->TMHH = tmodeld;
        else{
            scmname = delay->TMHH->NAME;
            tmodelscm = delay->TMHH;
            tmodels = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_slew (tmodels, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodels, 0);
            stm_addreferences(cell->NAME, tmodels->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            tmodeld = stm_mod_duplicate(tmodelscm->NAME, tmodelscm);
            stm_mod_defaultstm2tbl_delay (tmodeld, STM_NOTYPE, STM_NOTYPE, slew, capa);
            stm_storemodel(cell->NAME, NULL, tmodeld, 0);
            stm_addreferences(cell->NAME, tmodeld->NAME, stm_getreferences(cell->NAME, tmodelscm->NAME) - 1);
            addhtitem(htd, scmname, (long)tmodeld);
            addhtitem(hts, scmname, (long)tmodels);
            delay->TMHH = tmodeld;
        }
        delay->FMHH = tmodels;
    }
#ifndef __ALL__WARNING__
	input = NULL;
#endif
}

/*****************************************************************************
*                                fonction tas_timing()                       *
*            depart du calcul des fronts et des temps elementaires           *
*****************************************************************************/
int tas_timing(cnsfig,lofig,ttvfig)
cnsfig_list *cnsfig ;
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
front_list *slope ;
cone_list *cone ;
chain_list *modelchain;
timing_cell *cell;
ht *htd, *hts;
edge_list *input;
timing_model *tmodel;
delay_list *delay;
ptype_list *ptype;
long nbfront ;
inffig_list *ifl;
//long nbcone = 0, n_cone = 0 ;

mbk_comcheck( 0, cns_signcns(cnsfig), 0 );

if( V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE && TAS_CONTEXT->TAS_DELAY_SWITCH != 'N' ) {
  avt_errmsg( TAS_ERRMSG, "026", AVT_WARNING );
}

if( V_BOOL_TAB[ __AVT_HACK_NETLIST ].VALUE ) {

  printf( "starting the hack of the netlist %s\n", TAS_CONTEXT->TAS_LOFIG->NAME );
  mccHackNetlist( TAS_CONTEXT->TAS_LOFIG );
  printf( "hacked netlist drived !\n" );
}
              
#ifdef DELAY_DEBUG_STAT
mbk_debugstat(NULL,1);
#endif

tas_newname(NULL,1);
CELL = cnsfig->NAME ;
if(TAS_CONTEXT->TAS_VALID_SCM != 'Y')
    stm_addcell (CELL) ;

ifl=getloadedinffig(cnsfig->NAME);

tas_DetectConeConnect(cnsfig) ;
tas_prelink(cnsfig) ;

tas_add_disable_gate_delay(cnsfig);

if(TAS_CONTEXT->TAS_CAPARAPREC != 0.0) {
    tas_capara(cnsfig);
}

if(TAS_CONTEXT->TAS_DIF_LATCH != 'Y')
tas_StuckAt(cnsfig) ;

if(TAS_CONTEXT->TAS_DIF_LATCH == 'Y')
tas_DifLatch(cnsfig) ;

    avt_log (LOGTAS, 2, "TIMING FOR SLOPE AND TPxx\n");
    avt_log (LOGTAS, 2, "1ST STEP PRE-TREATMENT & SLOPE\n");

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  pre(detect+prelk+capara+stuck+difl):",0);
#endif

tas_flatrcx(lofig,cnsfig,ttvfig) ;

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  flatrcx:",0);
#endif

//for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT) nbcone++;
tas_AffectConnectorsInputSlew (ifl, cnsfig);

for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
    //if (!((nbcone / ++n_cone) % 10)) fprintf (stdout, "%ld p.cent done\n", nbcone / n_cone);

    if((getptype(cone->USER,CNS_PARALLEL)) != NULL) tas_para(cone) ;
    if((getptype(cone->USER,CNS_SWITCH)) != NULL) tas_switch_cmos(cone) ;
    if((getptype(cone->USER,CNS_SWITCH)) != NULL) tas_addptype_switch_cmos(cone) ;

    res_branche(cone) ;

    clas_branche(cone) ;

    front(cone) ;

        slope = (front_list *)((getptype(cone->USER, TAS_SLOPE_MAX))->DATA) ;

        avt_log (LOGTAS, 3, "                Slope for cone %ld '%s' FUP = ", cone->INDEX, cone->NAME) ;
        if(slope->FUP == TAS_NOFRONT)  
            avt_log (LOGTAS, 3, "NOFRONT ") ;
        else 
            avt_log (LOGTAS, 3, "%ld ",slope->FUP) ;
        avt_log (LOGTAS, 3, "FDOWN = ") ;
        if(slope->FDOWN == TAS_NOFRONT) 
            avt_log (LOGTAS, 3, "NOFRONT\n") ;
        else 
            avt_log (LOGTAS, 3, "%ld\n",slope->FDOWN) ;
    }

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  cone loop:",0);
#endif

if(TAS_CONTEXT->TAS_CAPARAPREC != 0.0)
  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
    tas_caparabl(cone) ;

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  caparabl:",0);
#endif

interface(cnsfig) ;

if (TAS_CONTEXT->TAS_CARAC == 'Y')
	tas_AffectCaracCon (cnsfig) ;

if(TAS_CONTEXT->TAS_CALCRCX == 'Y')
  tas_calc_not_func(cnsfig) ;

avt_log (LOGTAS, 2, "2ND STEP TPxx\n") ;
avt_log (LOGTAS, 2, "\n") ;

tas_detectbuf(cnsfig) ;

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  interf+affec+notfunc+detect:",0);
#endif

for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  cone->TYPE &= ~(TAS_MARQUE) ;

#ifdef DELAY_DEBUG_STAT
  nbtot=countchain((chain_list *)cnsfig->CONE);
  nbcur=0;
  TM=0;
#endif
  
nbfront = 0 ;
for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
    if((cone->TYPE & TAS_MARQUE) == TAS_MARQUE)
     continue ;
    nbfront = tas_delaycone(ifl, cone,nbfront,0) ;
  }

#ifdef DELAY_DEBUG_STAT
mbk_DisplayProgress(&TM, "delaycone:", nbcur, nbtot, 'e');
#endif
    
for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT){
    cone->TYPE &= ~(TAS_MARQUE);
    if(getptype(cone->USER, TAS_DELAY_UP_MAX))
        cone->USER = delptype(cone->USER, TAS_DELAY_UP_MAX);
    if(getptype(cone->USER, TAS_DELAY_DOWN_MAX))
        cone->USER = delptype(cone->USER, TAS_DELAY_DOWN_MAX);
    if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
        if(getptype(cone->USER, TAS_DELAY_UP_MIN))
            cone->USER = delptype(cone->USER, TAS_DELAY_UP_MIN);
        if(getptype(cone->USER, TAS_DELAY_DOWN_MIN))
            cone->USER = delptype(cone->USER, TAS_DELAY_DOWN_MIN);
    }
}

if( V_BOOL_TAB[__TAS_MODEL_MEMSYM].VALUE )
    tas_modelmemsym(cnsfig) ;
 
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("  delaycone+loop2+detectmemsym:",0);
#endif 
 
 if((TAS_CONTEXT->TAS_MERGERCN == 'Y') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
   tas_mergercdelay(cnsfig) ;

 tas_cleanloconin(cnsfig) ;
 
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("  mergerc+cleancon:",0);
#endif

tasdebuglotrs();

if(TAS_CONTEXT->TAS_CARACMODE == TAS_LUT_CPL){
    htd = addht(100);
    hts = addht(100);
    cell = stm_getcell(cnsfig->NAME) ;
    for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT){
        input = cone->INCONE;
        while (input){
            char *namein = NULL;
            char *nameout = NULL;
            float capa;
            front_list *slope = NULL;

            if((input->TYPE & CNS_CONE)==CNS_CONE){
                if((input->UEDGE.CONE->TYPE & CNS_EXT)==CNS_EXT){
                    namein = input->UEDGE.CONE->NAME;
                }
            }else if((input->TYPE & CNS_EXT)==CNS_EXT){
                namein = input->UEDGE.LOCON->NAME;
            }
            if((cone->TYPE & CNS_EXT)==CNS_EXT){
                nameout = cone->NAME;
                capa = -1.0;
            }else{
                capa = tas_getcapai(cone);
            }
/*            if(namein || nameout)
                stm_addaxisvalues(TTV_HTSLOPE_AXIS, TTV_HTCAPA_AXIS, namein, nameout);
*/
            ptype = getptype(input->USER, TAS_DELAY_MAX);
            if(ptype){
                delay = (delay_list*)ptype->DATA;
                if(!namein){
                    if((input->TYPE & CNS_CONE)==CNS_CONE){
                        slope = (front_list *)getptype(input->UEDGE.CONE->USER,TAS_SLOPE_MAX)->DATA;
                    }
                }
                tas_scm2tables(cell, input, htd, hts, delay, slope, capa);
            }
            if(TAS_CONTEXT->TAS_FIND_MIN == 'Y'){
                ptype = getptype(input->USER, TAS_DELAY_MIN);
                if(ptype){
                    delay = (delay_list*)ptype->DATA;
                    if(!namein){
                        if((input->TYPE & CNS_CONE)==CNS_CONE){
                            slope = (front_list *)getptype(input->UEDGE.CONE->USER,TAS_SLOPE_MIN)->DATA;
                        }
                    }
                    tas_scm2tables(cell, input, htd, hts, delay, slope, capa);
                }
            }
            input = input->NEXT;
/*            if(namein || nameout)
                stm_delaxisvalues();*/
        }
    }
    delht(htd);
    delht(hts);
    if(cell->MODEL_LIST != NULL){
        for(modelchain = dupchainlst(cell->MODEL_LIST); modelchain; modelchain = modelchain->NEXT){
            tmodel = (timing_model*)modelchain->DATA;
            if(tmodel->UTYPE == STM_MOD_MODSCM)
                stm_cell_delmodel (cell, tmodel, 1);
        }
    }
}

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  lutcpl:",0);
#endif

tas_rssfig( cnsfig );

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  rssfig:",0);
#endif

 if((TAS_CONTEXT->TAS_RC_FILE != NULL))
  tas_drivrcfile(cnsfig) ;

#ifdef DELAY_DEBUG_STAT
mbk_debugstat("  drive:",0);
#endif

 return(0);
}

void tasdebuglotrs()
{
  lotrs_list *lotrs ;
  char *ptname ;
  float vb, vs ;
  float vddmax ;
  int vb_valid ;

  if( V_STR_TAB[ __TAS_DEBUG_LOTRS ].VALUE ) {
    ptname = namefind(V_STR_TAB[ __TAS_DEBUG_LOTRS ].VALUE) ;
    
    if( ptname ) {
    
      for( lotrs = TAS_CONTEXT->TAS_LOFIG->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) {
      
        if( lotrs->TRNAME == ptname ) {
        
          vddmax = tas_getparam( lotrs, TAS_CASE, TP_VDDmax);
          
          vb_valid = 0 ;
          if( lotrs->BULK && lotrs->BULK->SIG) {
            if( getlosigalim( lotrs->BULK->SIG, &vb) )
              vb_valid=1;
          } 
          if( !vb_valid ) {
            if( (lotrs->TYPE & CNS_TP) == CNS_TP)
              vb = vddmax ;
            else
              vb = 0.0;
          }
          if( (lotrs->TYPE & CNS_TP) == CNS_TP)
            vs = vddmax ;
          else
            vs = 0.0 ;
          
          tpiv_carac_static( lotrs, vddmax, vddmax, vddmax, vb, vs );
        }
      }
    }
  }
}

/*****************************************************************************
* Fonctions de mémorisation temporaire des caractéristiques de sortie        *
* d'une porte.                                                               *
*****************************************************************************/

int TAS_NB_OUTPUT_CARAC = 0;

void tas_set_output_carac( output_carac **ptcarac,
                           timing_model *tmodel,
                           char          transition,
                           char          lmodel,
                           double        r,
                           double        c1,
                           double        c2,
                           double        slope
                         )
{
  output_carac  *carac ;
 
  if( *ptcarac )
    carac = *ptcarac ;
  else {
    carac = tas_alloc_output_carac();
    *ptcarac = carac ;
  }
  
  
  if( transition == 'U' ) {
    carac->UP.VALID = 1 ;
    carac->UP.R     = r ;
    carac->UP.C1    = c1 ;
    carac->UP.C2    = c2 ;
    carac->UP.MODEL = lmodel ;
    carac->UP.VT  = stm_mod_vt(tmodel);
    carac->UP.VF  = stm_mod_vf(tmodel);
    carac->UP.VDD = stm_mod_vdd(tmodel);
    carac->UP.VSAT = stm_mod_vsat(tmodel);
    carac->UP.RLIN = stm_mod_rlin(tmodel);
    carac->UP.VTH  = stm_mod_vth(tmodel);
    carac->UP.SLOPE  = slope;
  }
  else {
    carac->DW.VALID = 1 ;
    carac->DW.R     = r ;
    carac->DW.C1    = c1 ;
    carac->DW.C2    = c2 ;
    carac->DW.MODEL = lmodel ;
    carac->DW.VT  = stm_mod_vt(tmodel);
    carac->DW.VF  = stm_mod_vf(tmodel);
    carac->DW.VDD = stm_mod_vdd(tmodel);
    carac->DW.VSAT = stm_mod_vsat(tmodel);
    carac->DW.RLIN = stm_mod_rlin(tmodel);
    carac->DW.VTH  = stm_mod_vth(tmodel);
    carac->DW.SLOPE  = slope;
  }
  
}
                             
int tas_get_output_carac( cone_list *cone, 
                          char type, 
                          char trans,
                          output_carac_trans *infos
                        )
{
  ptype_list            *ptl = NULL ;
  output_carac_trans    *r = NULL ;
  int                    ret ;
  output_carac          *carac ;

  if( cone ) {
    if( type == 'm' )
      ptl = getptype( cone->USER, TAS_OUT_CARAC_MIN ) ;

    if( type == 'M' )
      ptl = getptype( cone->USER, TAS_OUT_CARAC_MAX ) ;
  }
    
  if( ptl ) {
  
    carac = (output_carac*)ptl->DATA ;

    if( trans == 'U' ) {
      if( carac->UP.VALID ) {
        r = &(carac->UP);
      }
    }

    if( trans == 'D' ) {
      if( carac->DW.VALID ) {
        r = &(carac->DW);
      }
    }

  }

  if( r ) {
    memcpy( infos, r, sizeof( output_carac_trans ) );
    ret = 1 ;
  }
  else {
    if( trans == 'U' ) {
      infos->VT  = STM_DEFAULT_VTN ;
      infos->VF  = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE ;
    }
    else {
      infos->VT  = STM_DEFAULT_VTP ;
      infos->VF  = 0.0 ;
    }
    infos->VTH   = V_FLOAT_TAB[__SIM_VTH].VALUE * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    infos->VDD   = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE ;
    infos->VSAT  = -1.0 ;
    infos->RLIN  = -1.0 ;
    infos->MODEL =  0 ;
    infos->R     = -1.0 ;
    infos->C1    = -1.0 ;
    infos->C2    = -1.0 ;
    infos->SLOPE = stm_thr2scm (STM_DEF_SLEW, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, STM_DEFAULT_VTN, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_UP);
    infos->VALID = 1 ;
    ret = 0 ;

  }
   
  return ret ;
}

output_carac* tas_alloc_output_carac()
{
  output_carac          *carac ;
  carac = (output_carac*) mbkalloc( sizeof( output_carac ) );
  TAS_NB_OUTPUT_CARAC++;
  carac->UP.VALID = 0 ;
  carac->DW.VALID = 0 ;
  return carac ;
}

void tas_clean_carac_cone( cone_list *cone )
{
    edge_list    *edge ;
    delay_list   *delay ;
    output_carac *carac ;
    ptype_list   *ptl ;

    for(edge = cone->INCONE ; edge != NULL ; edge = edge->NEXT){
        ptl = getptype(edge->USER,TAS_DELAY_MIN);
        if( ptl ) {
          delay = (delay_list *)ptl->DATA;
          if( delay ) {
            if( delay->CARAC ) {
              mbkfree( delay->CARAC );
              delay->CARAC = NULL ;
            }
          }
        }
        ptl = getptype(edge->USER,TAS_DELAY_MAX);
        if( ptl ) {
          delay = (delay_list *)ptl->DATA;
          if( delay ) {
            if( delay->CARAC ) {
              mbkfree( delay->CARAC );
              delay->CARAC = NULL ;
            }
          }
        }
    }
    ptl = getptype( cone->USER, TAS_OUT_CARAC_MIN );
    if( ptl ) {
      carac = (output_carac*)ptl->DATA ;
      mbkfree(carac);
      cone->USER = delptype( cone->USER, TAS_OUT_CARAC_MIN );
    }
    ptl = getptype( cone->USER, TAS_OUT_CARAC_MAX );
    if( ptl ) {
      carac = (output_carac*)ptl->DATA ;
      mbkfree(carac);
      cone->USER = delptype( cone->USER, TAS_OUT_CARAC_MAX );
    }
}
