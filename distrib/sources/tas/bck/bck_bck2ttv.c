/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SDF Version 1.00                                            */
/*    Fichier : bck_bck2ttv.c                                               */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim Dioury                                              */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "bck_bck2ttv.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/****************************************************************************/
/* bck_addlocontype                                                         */
/****************************************************************************/

void bck_addlocontype(locon,type)
locon_list *locon ;
long type ;
{
 if(getptype(locon->USER,type) == NULL)
  locon->USER = addptype(locon->USER,type,NULL) ;
}

/****************************************************************************/
/* bck_addlosigtype                                                         */
/****************************************************************************/

void bck_addlosigtype(losig,type)
losig_list *losig ;
long type ;
{
 if(getptype(losig->USER,type) == NULL)
  losig->USER = addptype(losig->USER,type,NULL) ;
}

/****************************************************************************/
/* bck_setloconin                                                           */
/****************************************************************************/

void bck_setloconin(locon)
locon_list *locon ;
{
 switch(locon->DIRECTION)
  {
   case IN        : break ;
   case INOUT     : break ;
   case TRANSCV   : break ;
   case OUT       : locon->DIRECTION = INOUT ;
                    break ;
   case TRISTATE  : locon->DIRECTION = TRANSCV ;
                    break ;
   default :        locon->DIRECTION = IN ;
                    break ;
  }
}

/****************************************************************************/
/* bck_setloconout                                                          */
/****************************************************************************/

void bck_setloconout(locon)
locon_list *locon ;
{
 switch(locon->DIRECTION)
  {
   case IN        : locon->DIRECTION = INOUT ;
                    break ;
   case INOUT     : break ;
   case TRANSCV   : break ;
   case OUT       : break ;
   case TRISTATE  : break ;
   default :        locon->DIRECTION = OUT ;
                    break ;
  }
}

/****************************************************************************/
/* bck_detectalim                                                           */
/****************************************************************************/

void bck_detectalim(lofig) 
lofig_list *lofig ;
{
 losig_list *losig ;
 locon_list *locon ;
 loins_list *loins ;

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(losig->TYPE == EXTERNAL)
    continue ;
   if(((mbk_LosigIsVDD(losig)) != 0) ||
      ((mbk_LosigIsVSS(losig)) != 0))
     {
      losig->TYPE = BCK_ALIM ;
     }
  }
 
 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   if(((mbk_LosigIsVDD(locon->SIG)) != 0) ||
      ((mbk_LosigIsVSS(locon->SIG)) != 0))
    {
     locon->SIG->TYPE = BCK_ALIM ;
     locon->DIRECTION = BCK_ALIM ;
    }
  }

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
     if(locon->SIG->TYPE == BCK_ALIM)
      {
       locon->DIRECTION = BCK_ALIM ;
       continue ;
      }
    }
  }
}

/****************************************************************************/
/* bck_delptype                                                             */
/****************************************************************************/

void bck_delptype(lofig) 
lofig_list *lofig ;
{
 losig_list *losig ;
 locon_list *locon ;
 loins_list *loins ;
 ptype_list *pt;

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(getptype(losig->USER,BCK_LOSIG_SIG) != NULL)
      losig->USER = delptype(losig->USER,BCK_LOSIG_SIG) ;
   if(getptype(losig->USER,BCK_LOSIG_BREAK) != NULL)
      losig->USER = delptype(losig->USER,BCK_LOSIG_BREAK) ;
   if(getptype(losig->USER,BCK_LOSIG_CLOCK) != NULL)
      losig->USER = delptype(losig->USER,BCK_LOSIG_CLOCK) ;
   if(getptype(losig->USER,BCK_LOSIG_EXT) != NULL)
      losig->USER = delptype(losig->USER,BCK_LOSIG_EXT) ;
  }
 
 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   if(getptype(locon->USER,BCK_LOCON_SIG) != NULL)
      locon->USER = delptype(locon->USER,BCK_LOCON_SIG) ;
   if(getptype(locon->USER,BCK_LOCON_BREAK) != NULL)
      locon->USER = delptype(locon->USER,BCK_LOCON_BREAK) ;
   if(getptype(locon->USER,BCK_LOCON_CLOCK) != NULL)
      locon->USER = delptype(locon->USER,BCK_LOCON_CLOCK) ;
   if(getptype(locon->USER,BCK_LOCON_EXT) != NULL)
      locon->USER = delptype(locon->USER,BCK_LOCON_EXT) ;
  }

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   if ((pt=getptype(loins->USER, PTYPE_BCK_INS))!=NULL)
   {
     mbkfree(pt->DATA);
     loins->USER=delptype(loins->USER, PTYPE_BCK_INS);
   }
   for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
     if(getptype(locon->USER,BCK_LOCON_SIG) != NULL)
        locon->USER = delptype(locon->USER,BCK_LOCON_SIG) ;
     if(getptype(locon->USER,BCK_LOCON_BREAK) != NULL)
        locon->USER = delptype(locon->USER,BCK_LOCON_BREAK) ;
     if(getptype(locon->USER,BCK_LOCON_CLOCK) != NULL)
        locon->USER = delptype(locon->USER,BCK_LOCON_CLOCK) ;
     if(getptype(locon->USER,BCK_LOCON_EXT) != NULL)
        locon->USER = delptype(locon->USER,BCK_LOCON_EXT) ;
    }
  }
}

/****************************************************************************/
/* bck_detectbreakinf                                                       */
/****************************************************************************/

void bck_detectbreakinf(lofig)
lofig_list *lofig ;
{
 locon_list *locon ;
 losig_list *losig ;
 loins_list *loins ;
 FILE *file ;
 list_list *ptlist ;
 chain_list *hchain = NULL ;
 chain_list *chain ;
 ht *htab ;
 char buf[1024] ;
 inffig_list *ifl;
 chain_list *maincl, *cl;

 if ((ifl=getinffig(lofig->NAME))==NULL) return;

 htab = addht(100) ;

 //maincl=infGetInfo(ifl, INF_SIGLIST_INFO);
 //for(ptlist = INF_SIGLIST ; ptlist != NULL ; ptlist = ptlist->NEXT)
 maincl=inf_GetEntriesByType(ifl, INF_BREAK, INF_ANY_VALUES);
 for(cl = maincl; cl!=NULL; cl=cl->NEXT)
   {
//    ptlist=(list_list *)cl->DATA;
//    if(ptlist->TYPE == INF_BREAK)
     addhtitem(htab,namealloc((char *)cl->DATA),(long)0) ;
    if(strchr((char *)cl->DATA,'*') != NULL)
     hchain = addchain(hchain,(char *)cl->DATA) ;
   }
 freechain(maincl);

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if((losig->TYPE == BCK_ALIM) || (losig->TYPE == EXTERNAL) ||
      (getptype(losig->USER,PTYPE_BCK_RC) != NULL))
       continue ;
   if(gethtitem(htab,namealloc(getsigname(losig))) != EMPTYHT)
      bck_addlosigtype(losig,BCK_LOSIG_BREAK) ;
  }

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
     if((locon->DIRECTION == BCK_ALIM) ||
       (getptype(locon->SIG->USER,PTYPE_BCK_RC) == NULL))
      {
       continue ;
      }
     sprintf(buf,"%s%c%s",loins->INSNAME,SEPAR,locon->NAME) ;
     if(gethtitem(htab,namealloc(buf)) != EMPTYHT)
       bck_addlocontype(locon,BCK_LOCON_BREAK) ;
    }
  }

 for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
   {
    for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
     {
      if((losig->TYPE == BCK_ALIM) || (losig->TYPE == EXTERNAL) ||
         (getptype(losig->USER,PTYPE_BCK_RC) != NULL))
          continue ;
      //if(ttv_jokersubst(getsigname(losig),chain->DATA,'*') == 1)
      if(mbk_TestREGEX(getsigname(losig),chain->DATA))
         bck_addlosigtype(losig,BCK_LOSIG_BREAK) ;
     }

    for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
     {
      for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
       {
        if((locon->DIRECTION == BCK_ALIM) ||
          (getptype(locon->SIG->USER,PTYPE_BCK_RC) == NULL))
         {
          continue ;
         }
        sprintf(buf,"%s%c%s",loins->INSNAME,SEPAR,locon->NAME) ;
        // if(ttv_jokersubst(buf,chain->DATA,'*') == 1)
        if(mbk_TestREGEX(buf,chain->DATA))
          bck_addlocontype(locon,BCK_LOCON_BREAK) ;
       }
     }
   }

 freechain(hchain) ;

 delht(htab) ;
}

/****************************************************************************/
/* bck_detectbreak                                                          */
/****************************************************************************/

void bck_detectbreak(lofig)
lofig_list *lofig ;
{
 locon_list *locon ;
 loins_list *loins ;
 ptype_list *ptype ;
 bck_annot *bck ;
 bck_checklist *check ;

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   if((ptype = getptype(loins->USER,PTYPE_BCK_INS)) != NULL)
    {
     if((bck = (bck_annot *)ptype->DATA) != NULL)
      {
       for(check = bck->CHECKS ; check != NULL ; check = check->NEXT)
        {
         locon = check->COMMAND ;
         bck_setloconin(locon) ;

         if(getptype(locon->SIG->USER,PTYPE_BCK_RC) == NULL)
           bck_addlosigtype(locon->SIG,BCK_LOSIG_CLOCK) ;

         bck_addlocontype(locon,BCK_LOCON_CLOCK) ;

         locon = check->DATA ;
         bck_setloconin(locon) ;

         if(getptype(locon->SIG->USER,PTYPE_BCK_RC) == NULL)
           bck_addlosigtype(locon->SIG,BCK_LOSIG_BREAK) ;

         bck_addlocontype(locon,BCK_LOCON_BREAK) ;
        }
      }
    }
  }
}

/****************************************************************************/
/* bck_detectaccess                                                         */
/****************************************************************************/

void bck_detectaccess(lofig)
lofig_list *lofig ;
{
 loins_list *loins ;
 ptype_list *ptype ;
 bck_annot *bck ;
 bck_delaylist *delay ;

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   if((ptype = getptype(loins->USER,PTYPE_BCK_INS)) != NULL)
    {
     if((bck = (bck_annot *)ptype->DATA) != NULL)
      {
       for(delay = bck->DELAYS ; delay != NULL ; delay = delay->NEXT)
        {
         bck_setloconin(delay->START) ;
         bck_setloconout(delay->END) ;
         if(getptype(delay->START->USER,BCK_LOCON_CLOCK) != NULL)
          {
           if(getptype(delay->END->SIG->USER,PTYPE_BCK_RC) == NULL)
             bck_addlosigtype(delay->END->SIG,BCK_LOSIG_BREAK) ;
           bck_addlocontype(delay->END,BCK_LOCON_BREAK) ;
          }
        }
      }
    }
  }
}

/****************************************************************************/
/* bck_detectext                                                            */
/****************************************************************************/

void bck_detectext(lofig)
lofig_list *lofig ;
{
 loins_list *loins ;
 ptype_list *ptype ;
 bck_annot *bck ;
 bck_delaylist *delay ;

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   if((ptype = getptype(loins->USER,PTYPE_BCK_INS)) != NULL)
    {
     if((bck = (bck_annot *)ptype->DATA) != NULL)
      {
       for(delay = bck->DELAYS ; delay != NULL ; delay = delay->NEXT)
        {
         if((delay->START->SIG->TYPE == EXTERNAL) &&
            (delay->END->SIG->TYPE == INTERNAL) && 
            (getptype(delay->END->SIG->USER,BCK_LOSIG_CLOCK) == NULL) &&
            (getptype(delay->END->SIG->USER,BCK_LOSIG_BREAK) == NULL) &&
            (getptype(delay->END->USER,BCK_LOCON_CLOCK) == NULL) &&
            (getptype(delay->END->USER,BCK_LOCON_BREAK) == NULL))
          {
           if(getptype(delay->END->SIG->USER,PTYPE_BCK_RC) != NULL)
            bck_addlocontype(delay->END,BCK_LOCON_EXT) ;
           else
            bck_addlosigtype(delay->END->SIG,BCK_LOSIG_EXT) ;
          }

         if((delay->END->SIG->TYPE == EXTERNAL) &&
            (delay->START->SIG->TYPE == INTERNAL) && 
            (getptype(delay->START->SIG->USER,BCK_LOSIG_CLOCK) == NULL) &&
            (getptype(delay->START->SIG->USER,BCK_LOSIG_BREAK) == NULL) &&
            (getptype(delay->START->USER,BCK_LOCON_CLOCK) == NULL) &&
            (getptype(delay->START->USER,BCK_LOCON_BREAK) == NULL))
          {
           if(getptype(delay->START->SIG->USER,PTYPE_BCK_RC) != NULL)
            bck_addlocontype(delay->START,BCK_LOCON_EXT) ;
           else
            bck_addlosigtype(delay->START->SIG,BCK_LOSIG_EXT) ;
          }
        }
      }
    }
  }
}

/****************************************************************************/
/* bck_getlocontype                                                         */
/****************************************************************************/

long bck_getlocontype(locon)
locon_list *locon ;
{
 switch(locon->DIRECTION)
  {
   case IN : return TTV_SIG_CI ;
   case OUT : return TTV_SIG_CO ;
   case TRISTATE : return TTV_SIG_CZ ;
   case INOUT : return TTV_SIG_CB ;
   case TRANSCV : return TTV_SIG_CT ;
   default : return TTV_SIG_CX ;
  }
}

/****************************************************************************/
/* bck_getextlocon                                                          */
/****************************************************************************/

ttvsig_list *bck_getextlocon(locon)
locon_list *locon ;
{
 ptype_list *ptype ;
 chain_list *chain ;

 if(locon->SIG->TYPE == INTERNAL)
   return(NULL) ;

 if((ptype = getptype(locon->SIG->USER,LOFIGCHAIN)) == NULL)
   return(NULL) ;

 for(chain = (chain_list *)ptype->DATA ; chain ; chain = chain->NEXT)
   {
    locon = (locon_list *)chain->DATA ;
    if(locon->TYPE == EXTERNAL)
     break ;
   }

 if(locon == NULL)
  return(NULL) ;

 if((ptype = getptype(locon->USER,BCK_LOCON_SIG)) != NULL)
   return((ttvsig_list *)ptype->DATA) ;
 else if((ptype = getptype(locon->SIG->USER,BCK_LOSIG_SIG)) != NULL)
   return((ttvsig_list *)ptype->DATA) ;
 else
   return(NULL) ;
}

/****************************************************************************/
/* bck_addttvsig                                                            */
/****************************************************************************/

void bck_addttvsig(lofig,ttvfig)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
 losig_list *losig ;
 loins_list *loins ;
 locon_list *locon ;
 chain_list *chain ;
 chain_list *chainc ;
 chain_list *chainb ;
 chain_list *chainn ;
 ttvsig_list *ptsig ;
 char *name ;
 char buf[2048] ;
 long type ;

 chain = NULL ;
 chainc = NULL ;
 chainb = NULL ;
 chainn = NULL ;


 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   if(locon->SIG->TYPE == BCK_ALIM)
    continue ;

   locon->NAME = ttv_revect(locon->NAME) ;
   type = bck_getlocontype(locon) ;
   chain = ttv_addrefsig(ttvfig,locon->NAME,ttv_revect(getsigname(locon->SIG)),
                                rcn_getcapa(lofig,locon->SIG)*1000.0,TTV_SIG_C|type,chain) ;
   ptsig = (ttvsig_list *)chain->DATA ;
   if((getptype(locon->SIG->USER,BCK_LOSIG_BREAK) != NULL) ||
      (getptype(locon->SIG->USER,BCK_LOSIG_CLOCK) != NULL))
     {
      ptsig->TYPE |= TTV_SIG_B ;
      chainb = addchain(chainb,ptsig) ;
     }
//   ttvfig->NBCONSIG ++ ;
   if(getptype(locon->SIG->USER,PTYPE_BCK_RC) != NULL)
     locon->USER = addptype(locon->USER,BCK_LOCON_SIG,(void *)ptsig) ;
   else
     locon->SIG->USER = addptype(locon->SIG->USER,BCK_LOSIG_SIG,(void *)ptsig) ;
  }

 chainc = chain ;
 chain = NULL ;

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if((losig->TYPE == BCK_ALIM) || (losig->TYPE == EXTERNAL) ||
      (getptype(losig->USER,PTYPE_BCK_RC) != NULL) ||
      (getptype(losig->USER,BCK_LOSIG_SIG) != NULL))
    continue ;

   name = ttv_revect(getsigname(losig)) ;

   if((getptype(losig->USER,BCK_LOSIG_BREAK) != NULL) ||
      (getptype(losig->USER,BCK_LOSIG_CLOCK) != NULL))
    {
     chainb = ttv_addrefsig(ttvfig,name,name,rcn_getcapa(lofig,losig)*1000.0,TTV_SIG_B,chainb) ;
     ptsig = (ttvsig_list *)chainb->DATA ;
     losig->USER = addptype(losig->USER,BCK_LOSIG_SIG,(void *)ptsig) ;
    }
   else if(getptype(losig->USER,BCK_LOSIG_EXT) != NULL)
    {
     chain = ttv_addrefsig(ttvfig,name,name,rcn_getcapa(lofig,losig)*1000.0,TTV_SIG_S,chain) ;
     ptsig = (ttvsig_list *)chain->DATA ;
     losig->USER = addptype(losig->USER,BCK_LOSIG_SIG,(void *)ptsig) ;
//     ttvfig->NBESIG++ ;
    }
   else
    {
     ptsig = ttv_addsig(ttvfig,name,name,rcn_getcapa(lofig,losig)*1000.0,TTV_SIG_S) ;
     losig->USER = addptype(losig->USER,BCK_LOSIG_SIG,(void *)ptsig) ;
    }
  }
 
 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
     locon->NAME = ttv_revect(locon->NAME) ;

     losig = locon->SIG ;

     if((locon->SIG->TYPE == BCK_ALIM) || (losig->TYPE == BCK_ALIM) ||
        (getptype(losig->USER,PTYPE_BCK_RC) == NULL) ||
        (getptype(losig->USER,BCK_LOSIG_SIG) != NULL))
       continue ;

     sprintf(buf,"%s%c%s",loins->INSNAME,SEPAR,locon->NAME) ;
     name = ttv_revect(buf) ;

     if(losig->TYPE == INTERNAL)
      {
       if((getptype(locon->USER,BCK_LOCON_BREAK) != NULL) ||
          (getptype(locon->USER,BCK_LOCON_CLOCK) != NULL))
        {
         chainb = ttv_addrefsig(ttvfig,name,ttv_revect(getsigname(locon->SIG)),
                                rcn_getcapa(lofig,locon->SIG)*1000.0,TTV_SIG_B,chainb) ;
         ptsig = (ttvsig_list *)chainb->DATA ;
         locon->USER = addptype(locon->USER,BCK_LOCON_SIG,(void *)ptsig) ;
        }
       else if(getptype(locon->USER,BCK_LOCON_EXT) != NULL)
        {
         chain = ttv_addrefsig(ttvfig,name,ttv_revect(getsigname(locon->SIG)),
                                rcn_getcapa(lofig,locon->SIG)*1000.0,TTV_SIG_S,chain) ;
         ptsig = (ttvsig_list *)chain->DATA ;
         locon->USER = addptype(locon->USER,BCK_LOCON_SIG,(void *)ptsig) ;
//         ttvfig->NBESIG++ ;
        }
       else
        {
         ptsig = ttv_addsig(ttvfig,name,ttv_revect(getsigname(locon->SIG)),
                                rcn_getcapa(lofig,locon->SIG)*1000.0,TTV_SIG_S) ;
         locon->USER = addptype(locon->USER,BCK_LOCON_SIG,(void *)ptsig) ;
        }
      }
     else
      {
       if((getptype(locon->USER,BCK_LOCON_BREAK) != NULL) ||
          (getptype(locon->USER,BCK_LOCON_CLOCK) != NULL))
        {
         chainn = ttv_addrefsig(ttvfig,name,ttv_revect(getsigname(locon->SIG)),
                                rcn_getcapa(lofig,locon->SIG)*1000.0,TTV_SIG_B|TTV_SIG_N,chainn) ;
         ptsig = (ttvsig_list *)chainn->DATA ;
         locon->USER = addptype(locon->USER,BCK_LOCON_SIG,(void *)ptsig) ;
         chainb = addchain(chainb,ptsig) ;
        }
       else
        {
         chainn = ttv_addrefsig(ttvfig,name,ttv_revect(getsigname(locon->SIG)),
                                rcn_getcapa(lofig,locon->SIG)*1000.0,TTV_SIG_N,chainn) ;
         ptsig = (ttvsig_list *)chainn->DATA ;
         locon->USER = addptype(locon->USER,BCK_LOCON_SIG,(void *)ptsig) ;
        }
//       ttvfig->NBNCSIG++ ;
      }
    }
  }

 ttvfig->CONSIG = (ttvsig_list **)chainc ;
 ttvfig->NCSIG = (ttvsig_list **)chainn ;
 ttvfig->EBREAKSIG = (ttvsig_list **)chainb ;
 ttvfig->ESIG = (ttvsig_list **)chain ;
 ttvfig->STATUS |= (TTV_STS_CLS | TTV_STS_HEAD) ;
 ttvfig->NBCONSIG = (long)0 ;
 ttvfig->NBNCSIG = (long)0 ;
 ttvfig->NBESIG = (long)0 ;
}

/****************************************************************************/
/* bck_getsiglocon                                                          */
/****************************************************************************/

ttvsig_list *bck_getsiglocon(locon)
locon_list *locon ;
{
 ptype_list *ptype ;

 if((ptype = getptype(locon->USER,BCK_LOCON_SIG)) != NULL)
   return((ttvsig_list *)ptype->DATA) ;
 else if((ptype = getptype(locon->SIG->USER,BCK_LOSIG_SIG)) != NULL)
   return((ttvsig_list *)ptype->DATA) ;
 else
   return(NULL) ;
}

/****************************************************************************/
/* bck_geteventlocon                                                        */
/****************************************************************************/

ttvevent_list *bck_geteventlocon (locon_list *locon, short type)
{
    ttvsig_list *ptsig;

    ptsig = bck_getsiglocon (locon);

    switch (type) {
        case EVNO:
        case EV10:
        case EVZ0:
        case EVX0:
        case NEGEDGE: 
            return ptsig->NODE;
        case EV__ :
        case EV01 :
        case EVZ1 :
        case EVX1 :
        case POSEDGE :
            return ptsig->NODE + 1;
        default:
            return NULL;
    }
}
/*
ttvevent_list *bck_geteventlocon(locon,type)
locon_list *locon ;
short type ;
{
 ttvsig_list *ptsig ;

 ptsig = bck_getsiglocon(locon) ;

 switch(type)
  {
   case EVNO :
   case EV10 :
   case EVZ0 :
   case EVX0 :
   case EV1Z :
   case EV1X :
   case EVXZ :
   case NEGEDGE : 
      return(ptsig->NODE) ;

   case EV__ :
   case EV01 :
   case EV0Z :
   case EV0X :
   case EVZ1 :
   case EVX1 :
   case EVZX :
   case POSEDGE :
   case EV_Z :
      return(ptsig->NODE+1) ;
  }
 return(NULL) ;
}
*/
/****************************************************************************/
/* bck_getlinetype                                                          */
/****************************************************************************/

long bck_getlinetype(locons,evs,locone,eve,type,delay)
locon_list *locons ;
short evs ;
locon_list *locone ;
short eve ;
long type ;
char delay ;
{
 ttvevent_list *start ;
 ttvevent_list *end ;
 long res ;

 start = bck_geteventlocon(locons,evs) ;
 end = bck_geteventlocon(locone,eve) ;

 if (!start || !end)
     return 0;

 if(((start->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) ||
    ((end->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0))
   {
    res = TTV_LINE_D ;

    if((delay == 'C') || (getptype(locons->USER,BCK_LOCON_CLOCK) != NULL))
     res |= TTV_LINE_CONT ;
   }
 else
   {
    if((delay == 'C') || (getptype(locons->USER,BCK_LOCON_CLOCK) != NULL))
     res = TTV_LINE_E | TTV_LINE_CONT ;
    else
     res = TTV_LINE_F ;
   }

 if((delay == 'C') || (getptype(locons->USER,BCK_LOCON_CLOCK) != NULL))
  {
   if(delay == 'D')
    {
     res |= TTV_LINE_A ;
    }
   else
    {
     switch(type)
      {
       case SETUP :
       case RECOVERY : res |= TTV_LINE_U ;
                               break ;

       case HOLD :
       case REMOVAL : res |= TTV_LINE_O ;
                             break ;
      }
    }
  }
 else if((type & INTERCONNECT) == INTERCONNECT)
  {
   res |= TTV_LINE_RC ;
  }
 
 return(res) ;
}

/****************************************************************************/
/* bck_detectunconnect                                                      */
/****************************************************************************/

void bck_detectunconnect(losig)
losig_list *losig ;
{
 ptype_list *ptype ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chaini = NULL ;
 chain_list *chaino = NULL ;
 chain_list *chaina = NULL ;
 locon_list *locon ;
 bck_delaylist *delay ;
 bck_delaylist *delayx ;
 bck_delaylist *delays ;
 bck_translist *trans ;

 ptype = getptype(losig->USER,LOFIGCHAIN) ;

 for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
   {
    locon = (locon_list *)chain->DATA ;

    if(locon->TYPE == INTERNAL)
     {
      if((locon->DIRECTION == IN) || (locon->DIRECTION == TRANSCV))
        { 
         chaini = addchain(chaini,locon) ;
        }
      if((locon->DIRECTION == OUT) || (locon->DIRECTION == TRANSCV) ||
          (locon->DIRECTION == TRISTATE) || (locon->DIRECTION == INOUT))
        { 
         chaino = addchain(chaino,locon) ;
        }
     }
    else if(locon->TYPE == EXTERNAL)
     {
      if((locon->DIRECTION == IN) || (locon->DIRECTION == TRANSCV))
        { 
         chaino = addchain(chaino,locon) ;
        }
      if((locon->DIRECTION == OUT) || (locon->DIRECTION == TRANSCV) ||
          (locon->DIRECTION == TRISTATE) || (locon->DIRECTION == INOUT))
        { 
         chaini = addchain(chaini,locon) ;
        }
     }
   }

 if((chaini == NULL) && (chaino == NULL))
   return ;

 ptype = getptype(losig->USER,PTYPE_BCK_RC) ;

 delayx = NULL ;

 for(delay = (bck_delaylist *)ptype->DATA ; delay != NULL ;
     delay = delay->NEXT)
   {
    if(delay->START == NULL)
     {
      if(chaino != NULL)
       {
        for(chain = chaino ; chain != NULL ; chain = chain->NEXT)
         {
          if(chain->DATA != delay->END)
           {
            if(delay->START == NULL)
              delay->START = (locon_list *)chain->DATA ;
            else
             {
              delayx = bck_adddelay(delayx, ABSOLUTE|INTERCONNECT) ;
              delayx->START = (locon_list *)chain->DATA ;
              delayx->END = delay->END ;
              for(trans = delay->TRANSLIST ; trans != NULL ; 
                  trans = trans->NEXT) 
                {
                 delayx->TRANSLIST = bck_addtrans(delayx->TRANSLIST) ;
                 delayx->TRANSLIST->EVENT1 = trans->EVENT1 ;
                 delayx->TRANSLIST->EVENT2 = trans->EVENT2 ;
                 delayx->TRANSLIST->VALUE = trans->VALUE ;
                }
             }
           }
         }
       }
     }
   }

 if(delayx != NULL)
   ptype->DATA = append((chain_list *)ptype->DATA,(chain_list *)delayx) ;
   
 
 for(delay = (bck_delaylist *)ptype->DATA ; delay != NULL ;
     delay = delayx)
   {
    delayx = delay->NEXT ;
    if(delay->START == NULL)
     {
      if(delay == (bck_delaylist *)ptype->DATA)
       {
        ptype->DATA = ((bck_delaylist *)ptype->DATA)->NEXT ;
       }
      else
       {
        delays->NEXT = delay->NEXT ;
       }
      delay->NEXT = NULL ;
      bck_freedelays(delay) ;
     }
    else
     delays = delay ;
   }

 for(delay = (bck_delaylist *)ptype->DATA ; delay != NULL ;
     delay = delay->NEXT)
   {
    ptype = getptype(delay->START->USER,BCK_LOCON_IN) ;
    
    if(ptype == NULL)
     {
      delay->START->USER = addptype(delay->START->USER,BCK_LOCON_IN,
                                    addchain(NULL,delay->END)) ;
     }
    else
     {
      ptype->DATA = addchain((chain_list *)ptype->DATA,delay->END) ;
     }
   }

 for(chain = chaino ; chain != NULL ; chain = chain->NEXT)
   {
    locon = (locon_list *)chain->DATA ;
    ptype = getptype(locon->USER,BCK_LOCON_IN) ;

    if(ptype == NULL)
     {
      chaina = dupchainlst(chaini) ;
     }
    else
     {
      for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ; 
          chainx = chainx->NEXT)
        {
         locon = (locon_list *)chainx->DATA ;
         locon->USER = addptype(locon->USER,BCK_LOCON_MARQUE,NULL) ;
        }
      for(chainx = chaini ; chainx != NULL ; chainx = chainx->NEXT)
        {
         locon = (locon_list *)chainx->DATA ;
         if(getptype(locon->USER,BCK_LOCON_MARQUE) == NULL)
          {
           chaina = addchain(chaina,locon) ;
          }
        }
      for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ; 
          chainx = chainx->NEXT)
        {
         locon = (locon_list *)chainx->DATA ;
         locon->USER = delptype(locon->USER,BCK_LOCON_MARQUE) ;
        }
     }
    ptype = getptype(losig->USER,PTYPE_BCK_RC) ;
    for(chainx = chaina ; chainx != NULL ; chainx = chainx->NEXT)
      {
       locon = (locon_list *)chainx->DATA ;
       if(locon != (locon_list *)chain->DATA)
        {
         ptype->DATA = bck_adddelay((bck_delaylist *)ptype->DATA,
                                     ABSOLUTE|INTERCONNECT) ;
         delay = (bck_delaylist *)ptype->DATA ;
         delay->START = (locon_list *)chain->DATA ;
         delay->END = (locon_list *)chainx->DATA ;
         delay->TRANSLIST = bck_addtrans(delay->TRANSLIST) ;
         delay->TRANSLIST->EVENT1 = EV10 ;
         delay->TRANSLIST->EVENT2 = EV10 ;
         delay->TRANSLIST->VALUE = (long)0 ;
         delay->TRANSLIST = bck_addtrans(delay->TRANSLIST) ;
         delay->TRANSLIST->EVENT1 = EV01 ;
         delay->TRANSLIST->EVENT2 = EV01 ;
         delay->TRANSLIST->VALUE = (long)0 ;
        }
      }
   }
 freechain(chaina);

 ptype = getptype(losig->USER,LOFIGCHAIN) ;

 for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
   {
    locon = (locon_list *)chain->DATA ;
    ptype = getptype(locon->USER,BCK_LOCON_IN) ;
    if(ptype != NULL)
     {
      freechain((chain_list *)ptype->DATA) ;
      locon->USER = delptype(locon->USER,BCK_LOCON_IN) ;
     }
   }

 freechain(chaino) ;
 freechain(chaini) ;
}

/****************************************************************************/
/* bck_ifexistetrans                                                        */
/****************************************************************************/

int bck_ifexistetrans(loins,node,namenode,root,nameroot)
loins_list *loins ;
ttvevent_list *node ;
char *namenode ;
ttvevent_list *root ;
char *nameroot ;
{
 static ttvfig_list *model = NULL ;
 static ttvsig_list *nodesig = NULL ;
 static ttvsig_list *rootsig = NULL ;
 static ttvsig_list *nodesigmodel = NULL ;
 static ttvsig_list *rootsigmodel = NULL ;
 ttvevent_list *nodex ;
 ttvevent_list *rootx ;

 if (!node || !root)
     return 0;

 if(model == NULL)
  {
   model = ttv_getttvfig(loins->FIGNAME,(long)0) ;
   nodesig = NULL ;
   rootsig = NULL ;
  }
 else if(loins->FIGNAME != model->INFO->FIGNAME)
  {
   model = ttv_getttvfig(loins->FIGNAME,(long)0) ;
   nodesig = NULL ;
   rootsig = NULL ;
  }

 if(model == NULL)
  return(1) ; /* pas de librairie dtx/ttx */

 ttv_parsttvfig(model,TTV_STS_CLS_FED,TTV_FILE_DTX) ;

 if(nodesig != node->ROOT)
  {
   nodesigmodel = ttv_getsigbyname(model,namenode,TTV_SIG_C) ; 
   nodesig = node->ROOT ;
  }

 if(rootsig != root->ROOT)
  {
   rootsigmodel = ttv_getsigbyname(model,nameroot,TTV_SIG_C) ; 
   rootsig = root->ROOT ;
  }

 if((nodesigmodel == NULL) || (rootsigmodel == NULL))
   return(0) ;

 if((node->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
  nodex = nodesigmodel->NODE + 1 ;
 else
  nodex = nodesigmodel->NODE ;

 if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
  rootx = rootsigmodel->NODE + 1 ;
 else
  rootx = rootsigmodel->NODE ;

 return(ttv_existeline(model,nodex,rootx,TTV_LINE_D)) ;
}

/****************************************************************************/
/* bck_addttvline                                                           */
/****************************************************************************/

void bck_addttvline(lofig,ttvfig)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
 losig_list *losig ;
 loins_list *loins ;
 ptype_list *ptype ;
 bck_annot *bck ;
 bck_delaylist *delay ;
 bck_checklist *check ;
 bck_translist *trans ;
 ttvevent_list *start ;
 ttvevent_list *end ;
 ttvline_list *ptline ;
 long type ;

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   if((ptype = getptype(loins->USER,PTYPE_BCK_INS)) != NULL)
    {
     if((bck = (bck_annot *)ptype->DATA) != NULL)
      {
       for(delay = bck->DELAYS ; delay != NULL ; delay = delay->NEXT)
        {
         if((delay->START->SIG->TYPE == BCK_ALIM) ||
            (delay->END->SIG->TYPE == BCK_ALIM))
           continue ;
         for(trans = delay->TRANSLIST ; trans != NULL ; trans = trans->NEXT)
          {
           start = bck_geteventlocon(delay->START,trans->EVENT1) ;
           end = bck_geteventlocon(delay->END,trans->EVENT2) ;
           type = bck_getlinetype(delay->START,trans->EVENT1,
                                  delay->END,trans->EVENT2,delay->TYPE,'D') ;
           //if(bck_ifexistetrans(loins,start,delay->START->NAME,
           //                           end,delay->END->NAME) == 0)
           //  continue ;
           ptline = ttv_addline(ttvfig,end,start,trans->VALUE,(long)0,
                                trans->VALUE,(long)0,type) ;
          }
        }
       for(check = bck->CHECKS ; check != NULL ; check = check->NEXT)
        {
         if((check->DATA->SIG->TYPE == BCK_ALIM) ||
            (check->COMMAND->SIG->TYPE == BCK_ALIM))
           continue ;
         for(trans = check->TRANSLIST ; trans != NULL ; trans = trans->NEXT)
          {
           start = bck_geteventlocon(check->DATA,trans->EVENT1) ;
           end = bck_geteventlocon(check->COMMAND,trans->EVENT2) ;
           type = bck_getlinetype(check->DATA,trans->EVENT1,
                                 check->COMMAND,trans->EVENT2,check->TYPE,'C') ;
           //if(bck_ifexistetrans(loins,start,check->DATA->NAME,
           //                           end,check->COMMAND->NAME) == 0)
           //  continue ;
           ptline = ttv_addline(ttvfig,end,start,trans->VALUE,(long)0,
                                trans->VALUE,(long)0,type) ;
          }
        }
      }
    }
  }

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if((losig->TYPE == BCK_ALIM) ||
      ((ptype = getptype(losig->USER,PTYPE_BCK_RC)) == NULL) ||
      (getptype(losig->USER,BCK_LOSIG_SIG) != NULL))
    continue ;

   bck_detectunconnect(losig) ;

   for(delay = (bck_delaylist *)ptype->DATA ; delay != NULL ; 
       delay = delay->NEXT)
     {
      for(trans = delay->TRANSLIST ; trans != NULL ; trans = trans->NEXT)
       {
        start = bck_geteventlocon(delay->START,trans->EVENT1) ;
        end = bck_geteventlocon(delay->END,trans->EVENT2) ;
        type = bck_getlinetype(delay->START,trans->EVENT1,
                               delay->END,trans->EVENT2,delay->TYPE,'D') ;
        if (type)
            ptline = ttv_addline(ttvfig,end,start,trans->VALUE,(long)0,
                             trans->VALUE,(long)0,type) ;
       }
     }
  }
}

/****************************************************************************/
/* bck_bck2ttv                                                              */
/****************************************************************************/

ttvfig_list *bck_bck2ttv( lofig_list *lofig, char *name)
{
 ttvfig_list *ttvfig ;
 chain_list *chainloop ;
 chain_list *chain ;

 if (!lofig) return NULL;

 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Conversion to TTV\n") ;

 lofigchain(lofig) ;
 ttvfig = ttv_givehead(name,name,NULL)  ;
 ttv_lockttvfig(ttvfig) ;
 ttv_setttvlevel(ttvfig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Transition expansion\n") ;
 bck_expandtrans(lofig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Alimentations detection\n") ;
 bck_detectalim(lofig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Breakpoints detection\n") ;
 bck_detectbreak(lofig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- User defined breakpoints detection\n") ;
 bck_detectbreakinf(lofig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Access detection\n") ;
 bck_detectaccess(lofig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Ext detection\n") ;
 bck_detectext(lofig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Add signals\n") ;
 bck_addttvsig(lofig,ttvfig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Add lines\n") ;
 bck_addttvline(lofig,ttvfig) ;
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Deleting temp info\n") ;
 bck_delptype(lofig) ;

 ttvfig->STATUS |= TTV_STS_DTX ;
 
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Loop detection\n") ;

 chainloop = ttv_detectloop(ttvfig,TTV_FIND_LINE) ;

 if(chainloop != NULL)
  {
   int nbloop = 0 ;
   chainloop = ttv_addlooplist(ttvfig,chainloop) ;
   for(chain = chainloop ; chain != NULL ; chain = chain->NEXT)
      nbloop++ ;
   ttv_printloop(nbloop,chainloop,ttvfig->INFO->FIGNAME) ;
   for(chain = chainloop ; chain != NULL ; chain = chain->NEXT)
    {
     ttv_freenamelist((chain_list *)chain->DATA) ;
    }
   freechain(chainloop) ;
  }

 ttv_cleantagttvfig(TTV_STS_FREE_MASK) ;

 ttv_builtrefsig(ttvfig);
 ttv_setttvdate(ttvfig,TTV_DATE_LOCAL) ;
 ttvfig->INFO->TOOLNAME = namealloc("bck2tv") ;
 ttvfig->INFO->TOOLVERSION = AVT_FULLVERSION ;
 ttvfig->INFO->SLOPE = (long)0 ;
 ttvfig->INFO->CAPAOUT = (long)0 ;
 
 if (TRACE_MODE == 'Y')
	 fprintf (stdout, "--- SDF --- Conversion to TTV DONE\n") ;

 ttv_unlockttvfig(ttvfig) ;
 return(ttvfig) ;
}
