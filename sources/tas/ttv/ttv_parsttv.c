/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_parsttv.c                                               */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* parser des fichiers ttx et dtx                                           */
/****************************************************************************/

#include "ttv.h"

long TTV_OLD_NEW = 0 ;
long TTV_LAST_SIG ;
long TTV_LAST_LINECOUNT ;
static char *bufferline = NULL ;
static int buffersize = 0 ;

/*****************************************************************************/
/*                        function ttv_dupline()                             */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* ttvmodel : figure model                                                   */
/* ptlbloc : liste des lien                                                  */
/* nbline : nombre de lien                                                   */
/* status : etat                                                             */
/*                                                                           */
/* index les signaux d'une figure                                            */
/*****************************************************************************/
void ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbline,status)
ttvfig_list *ttvfig ;
ttvfig_list *ttvmodel ;
ttvlbloc_list *ptlbloc ;
long nbline ;
long status ;
{
 ttvline_list *ttvline ;
 ttvline_list *ttvlinex ;
 ttvsig_list *ptsig1 ;
 ttvsig_list *ptsig2 ;
 ttvevent_list *ptnode1 ;
 ttvevent_list *ptnode2 ;
 long index1 ;
 long index2 ;
 long i ;
 long j ;
 long type ;

 for(i = 0 ; i < nbline ; i++)
  {
   j = (TTV_MAX_LBLOC -1) - (i % TTV_MAX_LBLOC) ;
   if((j == (TTV_MAX_LBLOC - 1)) && (i != 0)) ptlbloc = ptlbloc->NEXT ;
   ttvline = ptlbloc->LINE + j ;
   if((ttvline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
       continue ;
   index1 = (long)getptype(ttvline->NODE->ROOT->USER,TTV_SIG_INDEX)->DATA ;
   index2 = (long)getptype(ttvline->ROOT->ROOT->USER,TTV_SIG_INDEX)->DATA ;
   type = ttvline->NODE->ROOT->TYPE ;
   if(((type & (TTV_SIG_B|TTV_SIG_R|TTV_SIG_Q|TTV_SIG_L)) != 0) &&
       (( type & (TTV_SIG_C|TTV_SIG_N)) != 0))
       type &= ~(TTV_SIG_C|TTV_SIG_N);
   ptsig1 = ttv_getsigbyindex(ttvfig,index1,type) ;
   type = ttvline->ROOT->ROOT->TYPE ;
   if(((type & (TTV_SIG_B|TTV_SIG_R|TTV_SIG_Q|TTV_SIG_L)) != 0) &&
       (( type & (TTV_SIG_C|TTV_SIG_N)) != 0))
       type &= ~(TTV_SIG_C|TTV_SIG_N);
   ptsig2 = ttv_getsigbyindex(ttvfig,index2,type) ;
   ptnode1 = ((ttvline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 
              ptsig1->NODE + 1 :  ptsig1->NODE ;
   ptnode2 = ((ttvline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 
              ptsig2->NODE + 1 :  ptsig2->NODE ;
   ttvlinex = ttv_addline(ttvfig,ptnode2,ptnode1,ttvline->VALMAX,ttvline->FMAX,
              ttvline->VALMIN,ttvline->FMIN,ttvline->TYPE & (TTV_LINE_TYPE|
                                                             TTV_LINE_A|
                                                             TTV_LINE_U|
                                                             TTV_LINE_O|
                                                             TTV_LINE_R|
                                                             TTV_LINE_S|
                                                             TTV_LINE_RC|
                                                             TTV_LINE_HZ|
                                                             TTV_LINE_EV|
                                                             TTV_LINE_PR)) ;
   if((ptnode1 = ttv_getlinecmd(ttvmodel,ttvline,TTV_LINE_CMDMAX)) != NULL)
    {
     index1 = (long)getptype(ptnode1->ROOT->USER,TTV_CMD_INDEX)->DATA ;
     ptsig1 = ttv_getsigbyindex(ttvfig,index1,TTV_SIG_Q) ; 
     ptnode2 = ((ptnode1->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ?
              ptsig1->NODE + 1 :  ptsig1->NODE ;
     ttv_addcmd(ttvlinex,TTV_LINE_CMDMAX,ptnode2) ;
    }
   if((ptnode1 = ttv_getlinecmd(ttvmodel,ttvline,TTV_LINE_CMDMIN)) != NULL)
    {
     index1 = (long)getptype(ptnode1->ROOT->USER,TTV_CMD_INDEX)->DATA ;
     ptsig1 = ttv_getsigbyindex(ttvfig,index1,TTV_SIG_Q) ; 
     ptnode2 = ((ptnode1->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ?
              ptsig1->NODE + 1 :  ptsig1->NODE ;
     ttv_addcmd(ttvlinex,TTV_LINE_CMDMIN,ptnode2) ;
    }

   ttv_addcaracline(ttvlinex,ttvline->MDMAX,ttvline->MDMIN,
                    ttvline->MFMAX,ttvline->MFMIN) ;
  }
#ifndef __ALL__WARNING_
 status = 0;
#endif
}

/*****************************************************************************/
/*                        function ttv_setdupindex()                         */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* index les signaux d'une figure                                            */
/*****************************************************************************/
void ttv_setdupindex(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvsig_list *ptsig ;
 ttvsbloc_list *ptsbloc ;
 long i ;
 long j ;
 long index ;
 long nbend ;

 if(ttvfig->NBCONSIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
    {
     ptsig = *(ttvfig->CONSIG + i) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
    }
  }

 if(ttvfig->NBNCSIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
    {
     ptsig = *(ttvfig->NCSIG + i) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
    }
  }

 if((ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
    {
     ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
             : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
        ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
     else
       getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
     if(getptype(ptsig->USER,TTV_CMD_INDEX) == NULL)
        ptsig->USER = addptype(ptsig->USER,TTV_CMD_INDEX,(void*)(i+1)) ;
    }
  }

 if((ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
    {
     ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
             : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
     else
       getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
    }
  }


 if((ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
    {
     ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
              : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
     else
       getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
     
     }
  }

 if((ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
              : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
     else
       getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
     
     }
  }

 if((ttvfig->NBEXTSIG + ttvfig->NBINTSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
             : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
    }
  }

 if(ttvfig->NBESIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBESIG ; i++)
    {
     ptsig = *(ttvfig->ESIG + i) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void*)(i+1)) ;
    }
  }

 if(ttvfig->NBISIG != 0)
  {
   index = ttvfig->NBESIG + 1 ;
   ttvfig->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvfig->ISIG) ;
   nbend = ttvfig->NBISIG ;
   ptsbloc = ttvfig->ISIG ;

   for(i = 0 ; i < nbend ; i++)
    {
     j = (TTV_MAX_SBLOC -1) - (i % TTV_MAX_SBLOC) ;
     if((j == (TTV_MAX_SBLOC - 1)) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
     ptsig = ptsbloc->SIG + j ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,
                                        (void *)(index)) ;
     index++ ;
    }
   ttvfig->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvfig->ISIG) ;
  }
}

/*****************************************************************************/
/*                        function ttv_deldupindex()                         */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* efface les indexes des signaux                                            */
/*****************************************************************************/
void ttv_deldupindex(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvsig_list *ptsig ;
 ttvsbloc_list *ptsbloc ;
 long i ;
 long j ;
 long nbfree ;
 long nbend ;

 if(ttvfig->NBCONSIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
    {
     ptsig = *(ttvfig->CONSIG + i) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
    }
  }

 if(ttvfig->NBNCSIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
    {
     ptsig = *(ttvfig->NCSIG + i) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
    }
  }

 if((ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
    {
     ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
             : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
        ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
     if(getptype(ptsig->USER,TTV_CMD_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_CMD_INDEX) ;
    }
  }

 if((ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
    {
     ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
             : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
    }
  }


 if((ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
    {
     ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
              : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
     
     }
  }

 if((ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
              : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
     
     }
  }

 if((ttvfig->NBEXTSIG + ttvfig->NBINTSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
             : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
    }
  }

 if(ttvfig->NBESIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBESIG ; i++)
    {
     ptsig = *(ttvfig->ESIG + i) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
    }
  }

 if(ttvfig->NBISIG != 0)
  {
   ptsbloc = ttvfig->ISIG ;
   nbfree = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC)) 
             % TTV_MAX_SBLOC ;
   nbend = ttvfig->NBISIG + nbfree ;

   for(i = nbfree ; i < nbend ; i++)
    {
     j = i % TTV_MAX_SBLOC ;
     if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
     ptsig = ptsbloc->SIG + j ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
    }
  }
}

/*****************************************************************************/
/*                        function ttv_getmodeldup()                         */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* status : etat                                                             */
/*                                                                           */
/* renvoie un model a dupliquer                                              */
/*****************************************************************************/
ttvfig_list *ttv_getmodeldup(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ttvfig_list *ttvfigm ;
 ptype_list *ptype ;
 chain_list *chain ;

 chain = ttvfig->INFO->INSTANCES ;
 ttvfigm = ttvfig->INFO->MODEL ;

 if(ttvfigm == NULL)
  return(NULL) ;

 if((ttvfigm->STATUS & status) != status)
  {
   for(; chain != NULL ; chain = chain->NEXT)
    if((((ttvfig_list *)chain->DATA)->STATUS & status) == status)
     {
      ttvfigm  = (ttvfig_list *)chain->DATA ;
      break ;
     }
   if(chain == NULL)
    return(NULL) ;
  }

 if((getptype(ttvfig->USER,TTV_FIG_POS_S) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_S)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_S,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_S) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_S)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_S,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_POS_D) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_D)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_D,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_D) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_D)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_D,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_POS_E) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_E)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_E,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_E) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_E)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_E,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_POS_F) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_F)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_F,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_F) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_F)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_F,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_POS_T) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_T)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_T,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_T) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_T)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_T,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_POS_J) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_J)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_J,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_J) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_J)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_J,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_POS_P) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_POS_P)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_P,ptype->DATA) ;

 if((getptype(ttvfig->USER,TTV_FIG_LINE_P) == NULL) &&
    ((ptype = getptype(ttvfigm->USER,TTV_FIG_LINE_P)) != NULL))
   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_P,ptype->DATA) ;

 return(ttvfigm) ;
}

/*****************************************************************************/
/*                        function ttv_dupttvfig()                           */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* status : etat                                                             */
/* type : type de fichier                                                    */
/* typeht : type de table de hash                                            */
/* flaght : flag de construction de la table de hash                         */
/*                                                                           */
/* comlplete une figure grace a d'autre modele                               */
/* renvoie 1 si c'est terminer 0 sinon                                       */
/*****************************************************************************/
int ttv_dupttvfig(ttvfig,status,type,typeht,flaght)
ttvfig_list *ttvfig ;
long status ;
long type ;
long typeht ;
char flaght ;
{
 ttvfig_list *ttvmodel = NULL ;
 ttvlbloc_list *ptlbloc ;
 ttvsbloc_list *ptsbloc ;
 ttvsig_list *ttvsig ;
 ttvsig_list *ttvsigx ;
 ttvsig_list *ttvsigc ;
 ttvevent_list *ptnode ;
 ptype_list *ptype ;
 ptype_list *ptypex ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainref ;
 char buf[1024] ;
 long i ;
 long j ;
 long nbend ;
 char flagfree ;
 char flagset ;

 if((ttvfig->STATUS & TTV_STS_HEADER) != TTV_STS_HEADER)
  {
   ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_HEADER) ;
   if(ttvmodel == NULL)
     return(0) ;
   ttvfig->INFO->TOOLNAME = ttvmodel->INFO->TOOLNAME ;
   ttvfig->INFO->TOOLVERSION = ttvmodel->INFO->TOOLVERSION ;
   ttvfig->INFO->TECHNONAME = ttvmodel->INFO->TECHNONAME ;
   ttvfig->INFO->TECHNOVERSION = ttvmodel->INFO->TECHNOVERSION ;
   ttvfig->INFO->SLOPE = ttvmodel->INFO->SLOPE ;
   ttvfig->INFO->CAPAOUT = ttvmodel->INFO->CAPAOUT ;
   ttvfig->INFO->STHHIGH = ttvmodel->INFO->STHHIGH ;
   ttvfig->INFO->STHLOW = ttvmodel->INFO->STHLOW ;
   ttvfig->INFO->DTH = ttvmodel->INFO->DTH ;
   ttvfig->INFO->TEMP = ttvmodel->INFO->TEMP ;
   ttvfig->INFO->TNOM = ttvmodel->INFO->TNOM ;
   ttvfig->INFO->VDD = ttvmodel->INFO->VDD ;
   ttvfig->INFO->TTVDAY = ttvmodel->INFO->TTVDAY ;
   ttvfig->INFO->TTVMONTH = ttvmodel->INFO->TTVMONTH ;
   ttvfig->INFO->TTVYEAR = ttvmodel->INFO->TTVYEAR ;
   ttvfig->INFO->TTVHOUR = ttvmodel->INFO->TTVHOUR ;
   ttvfig->INFO->TTVMIN = ttvmodel->INFO->TTVMIN ;
   ttvfig->INFO->TTVSEC = ttvmodel->INFO->TTVSEC ;
   ttvfig->INFO->LEVEL = ttvmodel->INFO->LEVEL ;
   ttvfig->INFO->DTB_VERSION = ttvmodel->INFO->DTB_VERSION ;
   for(chain = ttvmodel->INS ; chain != NULL ; chain = chain->NEXT)
     {
      ttv_givettvfig(((ttvfig_list *)chain->DATA)->INFO->FIGNAME,
                     ((ttvfig_list *)chain->DATA)->INSNAME,ttvfig) ;
      if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
       ttv_parsttvfig((ttvfig_list *)ttvfig->INS->DATA,TTV_STS_L,type) ;
      else if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
       ttv_parsttvfig((ttvfig_list *)ttvfig->INS->DATA,
                      (TTV_STS_L | TTV_STS_SE),type) ;
     }
   ttvfig->STATUS |= TTV_STS_HEADER ;
  }
 else if(((type & TTV_FILE_DTX) == TTV_FILE_DTX) &&
         (((status & (TTV_STS_SE|TTV_STS_S)) != 0) &&
          ((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE)))
  {
   for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
     {
      ttv_parsttvfig((ttvfig_list *)chain->DATA,(TTV_STS_L | TTV_STS_SE),type) ;
     }
  }

 if((ttvfig->STATUS & status) == status)
   return(1) ;

 if(flaght == 'Y') 
   ttv_builthtabins(ttvfig,typeht) ;

// if((ttvfig->STATUS & TTV_STS_C) != (status & TTV_STS_C))
 if(((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
    ((status & TTV_STS_C) == TTV_STS_C))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_C) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_C) != (status & TTV_STS_C))
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_C) ;
   if(ttvmodel == NULL)
     return(0) ;
   chain = ttv_getsigbytype(ttvmodel,ttvmodel,TTV_SIG_C,NULL) ;
   chainref = NULL ;
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ttvsig = (ttvsig_list *)chainx->DATA ;
     if((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) 
       {
        if(((ttvsig->TYPE & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B)) == 0) ||
          ((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L))
         {
          chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                   ttvsig->NETNAME,ttvsig->CAPA,
          ttvsig->TYPE & (TTV_SIG_TYPE|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B),chainref);
          ttvsigx = (ttvsig_list *)chainref->DATA ;
         }
        else
         {
          if((ttvsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_Q) ;
          else if((ttvsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_L) ;
          else if((ttvsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_R) ;
          else if((ttvsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_B) ;
          ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
          chainref = addchain(chainref,(void*)ttvsigx) ;
          ttvsigx->TYPE |= ttvsig->TYPE & (TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|
                                           TTV_SIG_B) ;
         }
        ttvfig->NBCONSIG++ ;
       }
     else
      ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_C) ;
      ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
      ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
    }
   if(((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) && (chain != NULL))
    {
     ttvfig->CONSIG = ttv_allocreflist(reverse(chainref),ttvfig->NBCONSIG) ;
     freechain(chain) ;
    }
   chain = ttv_getsigbytype(ttvmodel,ttvmodel,TTV_SIG_N,NULL) ;
   chainref = NULL ;
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ttvsig = (ttvsig_list *)chainx->DATA ;
     if((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) 
       {
        if(((ttvsig->TYPE & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B)) == 0) ||
          ((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L))
         {
          chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                          ttvsig->NETNAME,ttvsig->CAPA,
          ttvsig->TYPE & (TTV_SIG_TYPE|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B),chainref);
          ttvsigx = (ttvsig_list *)chainref->DATA ;
          ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
          ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
         }
        else
         {
          if((ttvsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_Q) ;
          else if((ttvsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_L) ;
          else if((ttvsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_R) ;
          else if((ttvsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
           ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_B) ;
          ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
          chainref = addchain(chainref,(void*)ttvsigx) ;
          ttvsigx->TYPE |= ttvsig->TYPE & (TTV_SIG_N|TTV_SIG_L|
                                           TTV_SIG_R|TTV_SIG_B) ;
         }
        ttvfig->NBNCSIG++ ;
       }
     else
      ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_N) ;
    }
   if(((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) && (chain != NULL))
    {
     ttvfig->NCSIG = ttv_allocreflist(reverse(chainref),ttvfig->NBNCSIG) ;
     freechain(chain) ;
    }
   ttvfig->STATUS |= (status & TTV_STS_C) ;
  }

 if((ttvfig->STATUS & status) == status)
   return(1) ;

 if(((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L) &&
    ((status & TTV_STS_L) == TTV_STS_L))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_L) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_L) != TTV_STS_L)
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_L) ;
   if(ttvmodel == NULL)
     return(0) ;
   chainref = NULL ;
   for(i = 0 ; i < ttvmodel->NBELCMDSIG + ttvmodel->NBILCMDSIG ; i++)
     {
      if((i == ttvmodel->NBELCMDSIG) && (ttvmodel->NBELCMDSIG != 0))
        {
         ttvfig->NBELCMDSIG = ttvmodel->NBELCMDSIG ;
         ttvfig->ELCMDSIG = ttv_allocreflist(chainref,ttvfig->NBELCMDSIG) ;
         chainref = NULL ;
        }
      ttvsig = (i < ttvmodel->NBELCMDSIG) ? *(ttvmodel->ELCMDSIG + i)
               : *(ttvmodel->ILCMDSIG + i - ttvmodel->NBELCMDSIG) ;
      if(( ptype = getptype(ttvsig->USER,TTV_CMD_INDEX)) != NULL)
        ptype->DATA = (void *)(i+1) ;
      else
        ttvsig->USER = addptype(ttvsig->USER,TTV_CMD_INDEX,(void *)(i+1)) ;
      if(((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
        ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
       {
        if(ttvsig->ROOT == ttvmodel)
         {
          chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                          ttvsig->NETNAME,ttvsig->CAPA,
             ttvsig->TYPE & (TTV_SIG_TYPE|TTV_SIG_C|TTV_SIG_N),chainref) ;
          ttvsigx = (ttvsig_list *)chainref->DATA ;
          ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
          ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
         }
        else
         {
          ttvsigx = ttv_getsigbyinsname(ttvfig,ttv_getsigname(ttvmodel,
                                       buf,ttvsig),TTV_SIG_Q) ;
          ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
          chainref = addchain(chainref,(void*)ttvsigx) ;
         }
       }
      else
       {
        if((ttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_C) ;
        else if((ttvsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_N) ;
        ttvsigx->TYPE |= ttvsig->TYPE & (TTV_SIG_Q|TTV_SIG_C|TTV_SIG_N) ;
        chainref = addchain(chainref,(void*)ttvsigx) ;
      }
     if((ptype = getptype(ttvsig->USER,TTV_SIG_CMDOLD)) != NULL)
      {
       ttvsigx->USER = addptype(ttvsigx->USER,TTV_SIG_CMDOLD,(void *)NULL) ;
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
           chain = chain->NEXT)
        {
         ttvsigx->USER->DATA = (void *)addchain(
         (chain_list *)ttvsigx->USER->DATA,
         ttv_getsigbyname(ttvfig,
                        ttv_getsigname(ttvmodel,buf,(ttvsig_list *)chain->DATA),
                          TTV_SIG_Q)) ;
         ((ttvsig_list *)((chain_list *)ttvsigx->USER->DATA)->DATA)->USER = 
         addptype(
               ((ttvsig_list *)((chain_list *)ttvsigx->USER->DATA)->DATA)->USER,
               TTV_SIG_CMDNEW,ttvsigx) ;
        }
      }
    }
   if(ttvmodel->NBILCMDSIG != 0)
    {
     ttvfig->NBILCMDSIG = ttvmodel->NBILCMDSIG ;
     ttvfig->ILCMDSIG = ttv_allocreflist(chainref,ttvfig->NBILCMDSIG) ;
     chainref = NULL ;
    }
   else if(ttvmodel->NBELCMDSIG != 0)
    {
     ttvfig->NBELCMDSIG = ttvmodel->NBELCMDSIG ;
     ttvfig->ELCMDSIG = ttv_allocreflist(chainref,ttvfig->NBELCMDSIG) ;
     chainref = NULL ;
    }
   chainref = NULL ;
   for(i = 0 ; i < ttvmodel->NBELATCHSIG + ttvmodel->NBILATCHSIG ; i++)
     {
      if((i == ttvmodel->NBELATCHSIG) && (ttvmodel->NBELATCHSIG != 0))
        {
         ttvfig->NBELATCHSIG = ttvmodel->NBELATCHSIG ;
         ttvfig->ELATCHSIG = ttv_allocreflist(chainref,ttvfig->NBELATCHSIG) ;
         chainref = NULL ;
        }
      ttvsig = (i < ttvmodel->NBELATCHSIG) ? *(ttvmodel->ELATCHSIG + i)
               : *(ttvmodel->ILATCHSIG + i - ttvmodel->NBELATCHSIG) ;
      if(((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
        ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
       {
        if(ttvsig->ROOT == ttvmodel)
         {
          chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                          ttvsig->NETNAME,ttvsig->CAPA,
             ttvsig->TYPE & (TTV_SIG_TYPE|TTV_SIG_C|TTV_SIG_N),chainref) ;
          ttvsigx = (ttvsig_list *)chainref->DATA ;
          ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
          ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
         }
        else
         {
          ttvsigx = ttv_getsigbyinsname(ttvfig,ttv_getsigname(ttvmodel,
                                       buf,ttvsig),TTV_SIG_L) ;
          ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
          chainref = addchain(chainref,(void*)ttvsigx) ;
         }
       }
      else
       {
        if((ttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_C) ;
        else if((ttvsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_N) ;
        ttvsigx->TYPE |= ttvsig->TYPE & (TTV_SIG_L|TTV_SIG_C|TTV_SIG_N) ;
        chainref = addchain(chainref,(void*)ttvsigx) ;
      }
     if((ptype = getptype(ttvsig->USER,TTV_SIG_CMD)) != NULL)
      {
       if((ptypex = getptype(ttvsigx->USER,TTV_SIG_CMD)) != NULL)
        {
         for(chain = (chain_list *)ptypex->DATA ; chain != NULL ;
             chain = chain->NEXT)
           {
            ((ttvevent_list *)chain->DATA)->TYPE |= TTV_NODE_CMDMARQUE ;
            chainx = chain ;
           }
        }
       else
        {
         ttvsigx->USER = addptype(ttvsigx->USER,TTV_SIG_CMD,(void*)NULL) ;
         ptypex = ttvsigx->USER ;
         chainx = NULL ;
        }
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
           chain = chain->NEXT)
        {
         if(((ttvevent_list *)chain->DATA)->ROOT->ROOT->INFO->LEVEL != 
            ttvmodel->INFO->LEVEL)
           continue ;
         nbend = (long)getptype(((ttvevent_list *)chain->DATA)->ROOT->USER,
                                  TTV_CMD_INDEX)->DATA ;
         ttvsigc = ttv_getsigbyindex(ttvfig,nbend,TTV_SIG_Q) ;
         if((((ttvevent_list *)chain->DATA)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           ptnode = ttvsigc->NODE + 1 ;
         else
           ptnode = ttvsigc->NODE ;
         if((ptnode->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
          {
           if(chainx == NULL)
            {
             ptypex->DATA = (void *)
                        addchain((chain_list *)ptypex->DATA,(void *)ptnode) ;
             chainx = (chain_list *)ptypex->DATA ;
            }
           else
            {
             chainx->NEXT = addchain(chainx->NEXT,(void *)ptnode) ;
             chainx = chainx->NEXT ;
            }
          }
        }
       for(chain = (chain_list *)ptypex->DATA ; chain != NULL ; 
           chain = chain->NEXT)
          ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
      }
    }
   if(ttvmodel->NBILATCHSIG != 0)
    {
     ttvfig->NBILATCHSIG = ttvmodel->NBILATCHSIG ;
     ttvfig->ILATCHSIG = ttv_allocreflist(chainref,ttvfig->NBILATCHSIG) ;
     chainref = NULL ;
    }
   else if(ttvmodel->NBELATCHSIG != 0)
    {
     ttvfig->NBELATCHSIG = ttvmodel->NBELATCHSIG ;
     ttvfig->ELATCHSIG = ttv_allocreflist(chainref,ttvfig->NBELATCHSIG) ;
     chainref = NULL ;
    }
   chainref = NULL ;
   for(i = 0 ; i < ttvmodel->NBEPRESIG + ttvmodel->NBIPRESIG ; i++)
     {
      if((i == ttvmodel->NBEPRESIG) && (ttvmodel->NBEPRESIG != 0))
        {
         ttvfig->NBEPRESIG = ttvmodel->NBEPRESIG ;
         ttvfig->EPRESIG = ttv_allocreflist(chainref,ttvfig->NBEPRESIG) ;
         chainref = NULL ;
        }
      ttvsig = (i < ttvmodel->NBEPRESIG) ? *(ttvmodel->EPRESIG + i)
               : *(ttvmodel->IPRESIG + i - ttvmodel->NBEPRESIG) ;
      if(((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
        ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
       {
        if(ttvsig->ROOT == ttvmodel)
         {
          chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                          ttvsig->NETNAME,ttvsig->CAPA,
             ttvsig->TYPE & (TTV_SIG_TYPE|TTV_SIG_C|TTV_SIG_N),chainref) ;
          ttvsigx = (ttvsig_list *)chainref->DATA ;
          ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
          ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
         }
        else
         {
          ttvsigx = ttv_getsigbyinsname(ttvfig,ttv_getsigname(ttvmodel,
                                       buf,ttvsig),TTV_SIG_R) ;
          ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
          chainref = addchain(chainref,(void*)ttvsigx) ;
         }
       }
      else
       {
        if((ttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_C) ;
        else if((ttvsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_N) ;
        ttvsigx->TYPE |= ttvsig->TYPE & (TTV_SIG_R|TTV_SIG_C|TTV_SIG_N) ;
        chainref = addchain(chainref,(void*)ttvsigx) ;
      }
     if((ptype = getptype(ttvsig->USER,TTV_SIG_CMD)) != NULL)
      {
       if((ptypex = getptype(ttvsigx->USER,TTV_SIG_CMD)) != NULL)
        {
         for(chain = (chain_list *)ptypex->DATA ; chain != NULL ;
             chain = chain->NEXT)
           {
            ((ttvevent_list *)chain->DATA)->TYPE |= TTV_NODE_CMDMARQUE ;
            chainx = chain ;
           }
        }
       else
        {
         ttvsigx->USER = addptype(ttvsigx->USER,TTV_SIG_CMD,(void*)NULL) ;
         ptypex = ttvsigx->USER ;
         chainx = NULL ;
        }
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
           chain = chain->NEXT)
        {
         if(((ttvevent_list *)chain->DATA)->ROOT->ROOT->INFO->LEVEL !=
            ttvmodel->INFO->LEVEL)
           continue ;
         nbend = (long)getptype(((ttvevent_list *)chain->DATA)->ROOT->USER,
                                  TTV_CMD_INDEX)->DATA ;
         ttvsigc = ttv_getsigbyindex(ttvfig,nbend,TTV_SIG_Q) ;
         if((((ttvevent_list *)chain->DATA)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           ptnode = ttvsigc->NODE + 1 ;
         else
           ptnode = ttvsigc->NODE ;
         if((ptnode->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
          {
           if(chainx == NULL)
            {
             ptypex->DATA = (void *)
                        addchain((chain_list *)ptypex->DATA,(void *)ptnode) ;
             chainx = (chain_list *)ptypex->DATA ;
            }
           else
            {
             chainx->NEXT = addchain(chainx->NEXT,(void *)ptnode) ;
             chainx = chainx->NEXT ;
            }
          }
        }
       for(chain = (chain_list *)ptypex->DATA ; chain != NULL ; 
           chain = chain->NEXT)
          ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
      }
    }
   if(ttvmodel->NBIPRESIG != 0)
    {
     ttvfig->NBIPRESIG = ttvmodel->NBIPRESIG ;
     ttvfig->IPRESIG = ttv_allocreflist(chainref,ttvfig->NBIPRESIG) ;
     chainref = NULL ;
    }
   else if(ttvmodel->NBEPRESIG != 0)
    {
     ttvfig->NBEPRESIG = ttvmodel->NBEPRESIG ;
     ttvfig->EPRESIG = ttv_allocreflist(chainref,ttvfig->NBEPRESIG) ;
     chainref = NULL ;
    }
   chainref = NULL ;
   for(i = 0 ; i < ttvmodel->NBEBREAKSIG + ttvmodel->NBIBREAKSIG ; i++)
     {
      if((i == ttvmodel->NBEBREAKSIG) && (ttvmodel->NBEBREAKSIG != 0))
        {
         ttvfig->NBEBREAKSIG = ttvmodel->NBEBREAKSIG ;
         ttvfig->EBREAKSIG = ttv_allocreflist(chainref,ttvfig->NBEBREAKSIG) ;
         chainref = NULL ;
        }
      ttvsig = (i < ttvmodel->NBEBREAKSIG) ? *(ttvmodel->EBREAKSIG + i)
               : *(ttvmodel->IBREAKSIG + i - ttvmodel->NBEBREAKSIG) ;
      if(((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
        ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
       {
        if(ttvsig->ROOT == ttvmodel)
         {
          chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                          ttvsig->NETNAME,ttvsig->CAPA,
             ttvsig->TYPE & (TTV_SIG_TYPE|TTV_SIG_C|TTV_SIG_N),chainref) ;
          ttvsigx = (ttvsig_list *)chainref->DATA ;
          ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
          ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
         }
        else
         {
          ttvsigx = ttv_getsigbyinsname(ttvfig,ttv_getsigname(ttvmodel,
                                       buf,ttvsig),TTV_SIG_R) ;
          ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
          chainref = addchain(chainref,(void*)ttvsigx) ;
         }
       }
      else
       {
        if((ttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_C) ;
        else if((ttvsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
         ttvsigx = ttv_getsigbyname(ttvfig,ttvsig->NAME,TTV_SIG_N) ;
        ttvsigx->TYPE |= ttvsig->TYPE & (TTV_SIG_R|TTV_SIG_C|TTV_SIG_N) ;
        chainref = addchain(chainref,(void*)ttvsigx) ;
      }
    }
   if(ttvmodel->NBIBREAKSIG != 0)
    {
     ttvfig->NBIBREAKSIG = ttvmodel->NBIBREAKSIG ;
     ttvfig->IBREAKSIG = ttv_allocreflist(chainref,ttvfig->NBIBREAKSIG) ;
     chainref = NULL ;
    }
   else if(ttvmodel->NBEBREAKSIG != 0)
    {
     ttvfig->NBEBREAKSIG = ttvmodel->NBEBREAKSIG ;
     ttvfig->EBREAKSIG = ttv_allocreflist(chainref,ttvfig->NBEBREAKSIG) ;
     chainref = NULL ;
    }
   chainref = NULL ;
   for(i = 0 ; i < ttvmodel->NBEXTSIG + ttvmodel->NBINTSIG ; i++)
     {
      if((i == ttvmodel->NBEXTSIG) && (ttvmodel->NBEXTSIG != 0))
        {
         ttvfig->NBEXTSIG = ttvmodel->NBEXTSIG ;
         ttvfig->EXTSIG = ttv_allocreflist(chainref,ttvfig->NBEXTSIG) ;
         chainref = NULL ;
        }
      ttvsig = (i < ttvmodel->NBEXTSIG) ? *(ttvmodel->EXTSIG + i)
               : *(ttvmodel->INTSIG + i - ttvmodel->NBEXTSIG) ;
      if(ttvsig->ROOT == ttvmodel)
       {
        chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                        ttvsig->NETNAME,ttvsig->CAPA,
                                        TTV_SIG_I,chainref) ;
        ttvsigx = (ttvsig_list *)chainref->DATA ;
        ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
        ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
       }
      else
       {
        ttvsigx = ttv_getsigbyinsname(ttvfig,ttv_getsigname(ttvmodel,
                                     buf,ttvsig),TTV_SIG_I) ;
        ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
        chainref = addchain(chainref,(void*)ttvsigx) ;
       }
    }
   if(ttvmodel->NBINTSIG != 0)
    {
     ttvfig->NBINTSIG = ttvmodel->NBINTSIG ;
     ttvfig->INTSIG = ttv_allocreflist(chainref,ttvfig->NBINTSIG) ;
     chainref = NULL ;
    }
   else if(ttvmodel->NBEXTSIG != 0)
    {
     ttvfig->NBEXTSIG = ttvmodel->NBEXTSIG ;
     ttvfig->EXTSIG = ttv_allocreflist(chainref,ttvfig->NBEXTSIG) ;
     chainref = NULL ;
    }
   ttvfig->STATUS |= TTV_STS_L ;
   if((ttvmodel->NBELCMDSIG + ttvmodel->NBILCMDSIG) != 0)
    {
     for(i = 0 ; i < ttvmodel->NBELCMDSIG + ttvmodel->NBILCMDSIG ; i++)
      {
       ttvsig = (i < ttvmodel->NBELCMDSIG) ? *(ttvmodel->ELCMDSIG + i)
               : *(ttvmodel->ILCMDSIG + i - ttvmodel->NBELCMDSIG) ;
       if(getptype(ttvsig->USER,TTV_CMD_INDEX) != NULL)
         ttvsig->USER = delptype(ttvsig->USER,TTV_CMD_INDEX) ;
      }
    }
  }

 if((ttvfig->STATUS & status) == status)
   return(1) ;

 if(((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE) &&
    ((status & (TTV_STS_S|TTV_STS_SE)) != 0))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_SE) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_SE) != TTV_STS_SE)
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_SE) ;
   if(ttvmodel == NULL)
     return(0) ;
   chainref = NULL ;
   for(i = 0 ; i < ttvmodel->NBESIG ; i++)
    {
     ttvsig = *(ttvmodel->ESIG + i) ;
     if(ttvsig->ROOT == ttvmodel)
      {
       chainref = ttv_addrefsig(ttvfig,ttvsig->NAME,
                                       ttvsig->NETNAME,ttvsig->CAPA,
                                       TTV_SIG_S,chainref) ;
       ttvsigx = (ttvsig_list *)chainref->DATA ;
       ttv_setsigflag(ttvsigx, ttv_testsigflag(ttvsig, 0xffffffff));
       ttvsigx->PNODE[0]=ttvsig->PNODE[0]; ttvsigx->PNODE[1]=ttvsig->PNODE[1];
      }
     else
      {
       ttvsigx = ttv_getsigbyinsname(ttvfig,ttv_getsigname(ttvmodel,
                                     buf,ttvsig),TTV_SIG_S) ;
       ttv_setsiglevel(ttvsigx,ttvfig->INFO->LEVEL) ;
       chainref = addchain(chainref,(void*)ttvsigx) ;
      }
    }
   if(ttvmodel->NBESIG != 0)
    {
     ttvfig->NBESIG = ttvmodel->NBESIG ;
     ttvfig->ESIG = ttv_allocreflist(chainref,ttvfig->NBESIG) ;
     chainref = NULL ;
    }
   ttvfig->STATUS |= TTV_STS_SE ;
  }
    
 if((ttvfig->STATUS & status) == status)
   return(1) ;

 if(((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S) &&
    ((status & TTV_STS_S) == TTV_STS_S))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_S) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_S) != TTV_STS_S)
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_S) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(ttvmodel->NBISIG != 0)
    {
     ttvmodel->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvmodel->ISIG) ;
     nbend = ttvmodel->NBISIG ;
     ptsbloc = ttvmodel->ISIG ;

     for(i = 0 ; i < nbend ; i++)
      {
       j = (TTV_MAX_SBLOC -1) - (i % TTV_MAX_SBLOC) ;
       if((j == (TTV_MAX_SBLOC - 1)) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
       ttvsig = ptsbloc->SIG + j ;
       ttvsigx = ttv_addsig(ttvfig,ttvsig->NAME,ttvsig->NETNAME,
                            ttvsig->CAPA,TTV_SIG_S) ;
      }
     ttvmodel->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvmodel->ISIG) ;
    }
   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;
   ttvfig->STATUS |= TTV_STS_S ;
  }

 if((ttvfig->STATUS & status) == status)
   return(1) ;

 flagset = 'N' ;

 if(((ttvfig->STATUS & TTV_STS_J) != TTV_STS_J) &&
    ((status & TTV_STS_J) == TTV_STS_J))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_J) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_J) != TTV_STS_J)
    {
     if(flagset == 'Y')
      {
       ttv_deldupindex(ttvmodel) ;
       flagset = 'N' ;
      }
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_J) ;
    }
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(flagset == 'N')
    {
     ttv_setdupindex(ttvmodel) ;
     flagset = 'Y' ;
    }

   if(ttvmodel->NBJBLOC != 0)
    {
     ttvmodel->JBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->JBLOC) ;
     nbend = ttvmodel->NBJBLOC ;
     ptlbloc = ttvmodel->JBLOC ;
     ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbend,status) ;
     ttvmodel->JBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->JBLOC) ;
    }

   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;

   ttvfig->STATUS |= TTV_STS_J ;
   if((status & TTV_STS_DUAL_J) != 0)
     ttv_allocdualline(ttvfig,TTV_STS_DUAL_J) ;
  }

 if((ttvfig->STATUS & status) == status){
  if(flagset == 'Y')
      ttv_deldupindex(ttvmodel) ;
  flagset = 'N';
  return(1) ;
 }

 if(((ttvfig->STATUS & TTV_STS_E) != TTV_STS_E) &&
    ((status & TTV_STS_E) == TTV_STS_E))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_E) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_E) != TTV_STS_E)
    {
     if(flagset == 'Y')
      {
       ttv_deldupindex(ttvmodel) ;
       flagset = 'N' ;
      }
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_E) ;
    }
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(flagset == 'N')
    {
     ttv_setdupindex(ttvmodel) ;
     flagset = 'Y' ;
    }

   if(ttvmodel->NBEBLOC != 0)
    {
     ttvmodel->EBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->EBLOC) ;
     nbend = ttvmodel->NBEBLOC ;
     ptlbloc = ttvmodel->EBLOC ;
     ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbend,status) ;
     ttvmodel->EBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->EBLOC) ;
    }

   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;

   ttvfig->STATUS |= TTV_STS_E ;
   if((status & TTV_STS_DUAL_E) != 0)
     ttv_allocdualline(ttvfig,TTV_STS_DUAL_E) ;
  }

 if((ttvfig->STATUS & status) == status){
  if(flagset == 'Y')
      ttv_deldupindex(ttvmodel) ;
  flagset = 'N';
  return(1) ;
 }

 if(((ttvfig->STATUS & TTV_STS_P) != TTV_STS_P) &&
    ((status & TTV_STS_P) == TTV_STS_P))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_P) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_P) != TTV_STS_P)
    {
     if(flagset == 'Y')
      {
       ttv_deldupindex(ttvmodel) ;
       flagset = 'N' ;
      }
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_P) ;
    }
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(flagset == 'N')
    {
     ttv_setdupindex(ttvmodel) ;
     flagset = 'Y' ;
    }

   if(ttvmodel->NBPBLOC != 0)
    {
     ttvmodel->PBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->PBLOC) ;
     nbend = ttvmodel->NBPBLOC ;
     ptlbloc = ttvmodel->PBLOC ;
     ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbend,status) ;
     ttvmodel->PBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->PBLOC) ;
    }

   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;

   ttvfig->STATUS |= TTV_STS_P ;
   if((status & TTV_STS_DUAL_P) != 0)
     ttv_allocdualline(ttvfig,TTV_STS_DUAL_P) ;
  }

 if((ttvfig->STATUS & status) == status){
  if(flagset == 'Y')
      ttv_deldupindex(ttvmodel) ;
  flagset = 'N';
 return(1) ;
 }

 if(((ttvfig->STATUS & TTV_STS_F) != TTV_STS_F) &&
    ((status & TTV_STS_F) == TTV_STS_F))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_F) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_F) != TTV_STS_F)
    {
     if(flagset == 'Y')
      {
       ttv_deldupindex(ttvmodel) ;
       flagset = 'N' ;
      }
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_F) ;
    }
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(flagset == 'N')
    {
     ttv_setdupindex(ttvmodel) ;
     flagset = 'Y' ;
    }

   if(ttvmodel->NBFBLOC != 0)
    {
     ttvmodel->FBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->FBLOC) ;
     nbend = ttvmodel->NBFBLOC ;
     ptlbloc = ttvmodel->FBLOC ;
     ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbend,status) ;
     ttvmodel->FBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->FBLOC) ;
    }

   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;

   ttvfig->STATUS |= TTV_STS_F ;
   if((status & TTV_STS_DUAL_F) != 0)
     ttv_allocdualline(ttvfig,TTV_STS_DUAL_F) ;
  }

 if((ttvfig->STATUS & status) == status){
  if(flagset == 'Y')
      ttv_deldupindex(ttvmodel) ;
  flagset = 'N';
  return(1) ;
 }

 if(((ttvfig->STATUS & TTV_STS_T) != TTV_STS_T) &&
    ((status & TTV_STS_T) == TTV_STS_T))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_T) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_T) != TTV_STS_T)
    {
     if(flagset == 'Y')
      {
       ttv_deldupindex(ttvmodel) ;
       flagset = 'N' ;
      }
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_T) ;
    }
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(flagset == 'N')
    {
     ttv_setdupindex(ttvmodel) ;
     flagset = 'Y' ;
    }

   if(ttvmodel->NBTBLOC != 0)
    {
     ttvmodel->TBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->TBLOC) ;
     nbend = ttvmodel->NBTBLOC ;
     ptlbloc = ttvmodel->TBLOC ;
     ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbend,status) ;
     ttvmodel->TBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->TBLOC) ;
    }

   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;

   ttvfig->STATUS |= TTV_STS_T ;
   if((status & TTV_STS_DUAL_T) != 0)
     ttv_allocdualline(ttvfig,TTV_STS_DUAL_T) ;
  }

 if((ttvfig->STATUS & status) == status){
  if(flagset == 'Y')
      ttv_deldupindex(ttvmodel) ;
  flagset = 'N';
  return(1) ;
 }

 if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
    ((status & TTV_STS_D) == TTV_STS_D))
  {
   if(ttvmodel == NULL)
    ttvmodel = ttv_getmodeldup(ttvfig,status & TTV_STS_D) ;
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_D) != TTV_STS_D)
    {
     if(flagset == 'Y')
      {
       ttv_deldupindex(ttvmodel) ;
       flagset = 'N' ;
      }
     ttvmodel = ttv_getmodeldup(ttvfig,TTV_STS_D) ;
    }
   if(ttvmodel == NULL)
     return(0) ;
   if((ttvmodel->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
     flagfree = 'Y' ;
   else
     flagfree = 'N' ;
   ttvmodel->STATUS |= TTV_STS_NOT_FREE ;
   if(flagset == 'N')
    {
     ttv_setdupindex(ttvmodel) ;
     flagset = 'Y' ;
    }

   if(ttvmodel->NBDBLOC != 0)
    {
     ttvmodel->DBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->DBLOC) ;
     nbend = ttvmodel->NBDBLOC ;
     ptlbloc = ttvmodel->DBLOC ;
     ttv_dupline(ttvfig,ttvmodel,ptlbloc,nbend,status) ;
     ttvmodel->DBLOC = (ttvlbloc_list *)reverse((chain_list *)ttvmodel->DBLOC) ;
    }

   if(flagfree == 'N')
     ttvmodel->STATUS &= ~(TTV_STS_NOT_FREE) ;

   ttvfig->STATUS |= TTV_STS_D ;
   if((status & TTV_STS_DUAL_D) != 0)
     ttv_allocdualline(ttvfig,TTV_STS_DUAL_D) ;
  }

 if((ttvfig->STATUS & status) == status){
  if(flagset == 'Y')
      ttv_deldupindex(ttvmodel) ;
  flagset = 'N';
  return(1) ;
 }else{
  return(0) ;
 }
}

/*****************************************************************************/
/*                        function ttv_getfilepos()                          */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* status : etat                                                             */
/* file : fichier                                                            */
/* c : premier caractere de la liste                                         */
/* type : type de fichier                                                    */
/*                                                                           */
/* calcul la nouvelle position dans un fichier                               */
/*****************************************************************************/
long ttv_getfilepos(ttvfig,status,file,c,type)
ttvfig_list *ttvfig ;
long status ;
FILE *file ;
char c ;
long type ;
{
 ptype_list *ptype ;

 switch(c)
   {
    case 'X' : if((((ttvfig->STATUS & TTV_STS_HEADER) == TTV_STS_HEADER) &&
                   ((((status & TTV_STS_S) != TTV_STS_S) &&
                     ((status & TTV_STS_SE) != TTV_STS_SE)) ||
                   ((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE))) &&
                  (((status & TTV_STS_C) != TTV_STS_C))
                     && ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
                {
                 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                  {
                   if(((ttvfig->STATUS & TTV_STS_J) != TTV_STS_J) &&
                      ((status & TTV_STS_J) == TTV_STS_J))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_J)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return((long)0) ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_J)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_P) != TTV_STS_P) &&
                      ((status & TTV_STS_P) == TTV_STS_P))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_P)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return((long)0) ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_P)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_T) != TTV_STS_T) &&
                      ((status & TTV_STS_T) == TTV_STS_T))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_T)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return((long)0) ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_T)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                  }
                 else
                  {
                    if(((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S) &&
                        (((status & TTV_STS_S) == TTV_STS_S) ||
                         ((status & TTV_STS_SE) == TTV_STS_SE)) &&
                       (((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE) ||
                        ((status & TTV_STS_S) == TTV_STS_S)))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_S)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return((long)0) ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_S)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_E) != TTV_STS_E) &&
                      ((status & TTV_STS_E) == TTV_STS_E))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_E)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return((long)0) ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_E)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_F) != TTV_STS_F) &&
                      ((status & TTV_STS_F) == TTV_STS_F))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_F)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_F)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
                      ((status & TTV_STS_D) == TTV_STS_D))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_D)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_D)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                  }
                }
               break ;
    case 'C' : 
    case 'N' : if(((status & TTV_STS_C) != TTV_STS_C)
                   && ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
                {
                 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                  {
                   if(((ttvfig->STATUS & TTV_STS_J) != TTV_STS_J) &&
                      ((status & TTV_STS_J) == TTV_STS_J))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_J)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_J)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_P) != TTV_STS_P) &&
                      ((status & TTV_STS_P) == TTV_STS_P))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_P)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_P)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_T) != TTV_STS_T) &&
                      ((status & TTV_STS_T) == TTV_STS_T))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_T)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_T)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                  }
                 else
                  {
                    if(((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S) &&
                        (((status & TTV_STS_S) == TTV_STS_S) ||
                         ((status & TTV_STS_SE) == TTV_STS_SE)) &&
                       (((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE) ||
                        ((status & TTV_STS_S) == TTV_STS_S)))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_S)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_S)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_E) != TTV_STS_E) &&
                      ((status & TTV_STS_E) == TTV_STS_E))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_E)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_E)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_F) != TTV_STS_F) &&
                      ((status & TTV_STS_F) == TTV_STS_F))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_F)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_F)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
                      ((status & TTV_STS_D) == TTV_STS_D))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_D)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_D)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                  }
                }
               break ;
    case 'Q' : 
    case 'L' :
    case 'R' :
    case 'B' :
    case 'I' : if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
                {
                 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                  {
                   if(((ttvfig->STATUS & TTV_STS_J) != TTV_STS_J) &&
                      ((status & TTV_STS_J) == TTV_STS_J))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_J)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_J)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_P) != TTV_STS_P) &&
                      ((status & TTV_STS_P) == TTV_STS_P))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_P)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_P)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_T) != TTV_STS_T) &&
                      ((status & TTV_STS_T) == TTV_STS_T))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_T)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_T)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                  }
                 else
                  {
                    if(((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S) &&
                        (((status & TTV_STS_S) == TTV_STS_S) ||
                         ((status & TTV_STS_SE) == TTV_STS_SE)) &&
                       (((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE) ||
                        ((status & TTV_STS_S) == TTV_STS_S)))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_S)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_S)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_E) != TTV_STS_E) &&
                      ((status & TTV_STS_E) == TTV_STS_E))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_E)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_E)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_F) != TTV_STS_F) &&
                      ((status & TTV_STS_F) == TTV_STS_F))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_F)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_F)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                   else if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
                      ((status & TTV_STS_D) == TTV_STS_D))
                    {
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_D)) != NULL)
                       fseek(file,(long)ptype->DATA,0) ;
                     else return(long)0 ;
                     if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_D)) != NULL)
                        return((long)ptype->DATA) ;
                    }
                  }
                }
               break ;
    case 'S' : if(((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S) ||
                  (((status & TTV_STS_S) != TTV_STS_S) && 
                   ((status & TTV_STS_SE) != TTV_STS_SE)) ||
                  (((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE) &&
                   ((status & TTV_STS_S) != TTV_STS_S)))
                {
                 if(((ttvfig->STATUS & TTV_STS_E) != TTV_STS_E) &&
                    ((status & TTV_STS_E) == TTV_STS_E))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_E)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_E)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                 else if(((ttvfig->STATUS & TTV_STS_F) != TTV_STS_F) &&
                    ((status & TTV_STS_F) == TTV_STS_F))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_F)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_F)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                 else if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
                    ((status & TTV_STS_D) == TTV_STS_D))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_D)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_D)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                }
               break ;
    case 'J' : if(((ttvfig->STATUS & TTV_STS_J) == TTV_STS_J) ||
                  ((status & TTV_STS_J) != TTV_STS_J))
                {
                 if(((ttvfig->STATUS & TTV_STS_P) != TTV_STS_P) &&
                    ((status & TTV_STS_P) == TTV_STS_P))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_P)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_P)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                 else if(((ttvfig->STATUS & TTV_STS_T) != TTV_STS_T) &&
                    ((status & TTV_STS_T) == TTV_STS_T))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_T)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_T)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                }
               break ;
    case 'E' : if(((ttvfig->STATUS & TTV_STS_E) == TTV_STS_E) ||
                  ((status & TTV_STS_E) != TTV_STS_E))
                {
                 if(((ttvfig->STATUS & TTV_STS_F) != TTV_STS_F) &&
                    ((status & TTV_STS_F) == TTV_STS_F))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_F)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_F)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                 else if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
                    ((status & TTV_STS_D) == TTV_STS_D))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_D)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_D)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                }
               break ;
    case 'P' : if(((ttvfig->STATUS & TTV_STS_P) == TTV_STS_P) ||
                  ((status & TTV_STS_P) != TTV_STS_P))
                {
                 if(((ttvfig->STATUS & TTV_STS_T) != TTV_STS_T) &&
                    ((status & TTV_STS_T) == TTV_STS_T))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_T)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_T)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                }
               break ;
    case 'F' : if(((ttvfig->STATUS & TTV_STS_F) == TTV_STS_F) ||
                  ((status & TTV_STS_F) != TTV_STS_F))
                {
                 if(((ttvfig->STATUS & TTV_STS_D) != TTV_STS_D) &&
                    ((status & TTV_STS_D) == TTV_STS_D))
                  {
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_POS_D)) != NULL)
                     fseek(file,(long)ptype->DATA,0) ;
                   else return(long)0 ;
                   if((ptype = getptype(ttvfig->USER,TTV_FIG_LINE_D)) != NULL)
                      return((long)ptype->DATA) ;
                  }
                }
               break ;
    default  : 
               break ;
   }
return(long)0 ;
}


/*****************************************************************************/
/*                        function ttv_addfilepos()                          */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* file : fichier en cours de lecture                                        */
/* c : premier caractere de la liste                                         */
/*                                                                           */
/* calcul la position de c dans le fichier qui en cours de parse             */
/*****************************************************************************/
void ttv_addfilepos(ttvfig,file,c)
ttvfig_list *ttvfig ;
FILE *file ;
char *c ;
{
 switch (*c)
  {
   case 'S' : if(getptype(ttvfig->USER,TTV_FIG_POS_S) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_S,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              else
                 return ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_S) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_S,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   case 'D' : if(getptype(ttvfig->USER,TTV_FIG_POS_D) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_D,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_D) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_D,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   case 'E' : if(getptype(ttvfig->USER,TTV_FIG_POS_E) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_E,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              else
                 return ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_E) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_E,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   case 'F' : if(getptype(ttvfig->USER,TTV_FIG_POS_F) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_F,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              else
                 return ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_F) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_F,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   case 'T' : if(getptype(ttvfig->USER,TTV_FIG_POS_T) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_T,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              else
                 return ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_T) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_T,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   case 'J' : if(getptype(ttvfig->USER,TTV_FIG_POS_J) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_J,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              else
                 return ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_J) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_J,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   case 'P' : if(getptype(ttvfig->USER,TTV_FIG_POS_P) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_POS_P,
                       (void *)((long)(ftell(file) - (long)(strlen(c))))) ;
              else
                 return ;
              if(getptype(ttvfig->USER,TTV_FIG_LINE_P) == NULL)
                ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_LINE_P,
                       (void *)(TTV_LAST_LINECOUNT)) ;
             return ;
   default :
             return ;
  }
}

/*****************************************************************************/
/*                        function ttv_parserror()                           */
/* parametres :                                                              */
/* filename : nom de fichier                                                 */
/* linecount : ligne d'erreur                                                */
/*                                                                           */
/* declanche un message d'erreur de syntaxe dans le fichier filename a la    */
/* ligne linecount en appelant la fonction ttv_error                         */
/*****************************************************************************/
void ttv_parserror(filename,linecount) 
char *filename ;
long linecount ;
{
 ttvfig_list *ttvfig ;
 char chlinecount[1024] ;
 char *ptchar ; 
 char *figname ; 
 chain_list *chainfig = NULL ;
 chain_list *chainhead = NULL ;
 chain_list *chain ;
 char flag = 'N' ;

  if(TTV_LANG == TTV_LANG_E)
     sprintf(chlinecount,"line %ld in file %s",linecount,filename) ;
  else
     sprintf(chlinecount,"ligne %ld dans le fichier %s",linecount,filename) ;

  for(ptchar = filename ; *ptchar != SEPAR && *ptchar != '\0' ; ptchar++ ) ;
  
  if(*ptchar == SEPAR)
   {
    *ptchar = '\0' ;
    flag = 'Y' ;
   }
   
  figname = namealloc(filename) ;

  if(flag == 'Y')
   *ptchar = SEPAR ;

  for(ttvfig = TTV_LIST_TTVFIG ; ttvfig != NULL ; ttvfig = ttvfig->NEXT)
   {
    if(ttvfig->INFO->FIGNAME == figname) 
     chainfig = addchain(chainfig,ttvfig) ;
   }

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
  {
   ttvfig = (ttvfig_list *)chain->DATA ;
   if((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C)
      {
       if(ttvfig->NCSIG != NULL) 
        {
         if(TTV_LAST_SIG != TTV_SIG_N)
          ttvfig->NCSIG =  ttv_allocreflist((chain_list*)ttvfig->NCSIG,ttvfig->NBNCSIG) ;
        }
       else if(ttvfig->CONSIG != NULL) 
        {
         if(TTV_LAST_SIG != TTV_SIG_C)
          ttvfig->CONSIG = ttv_allocreflist((chain_list*)ttvfig->CONSIG,ttvfig->NBCONSIG) ;
        }
      }
  
   if((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L)
      {
       if((ttvfig->INTSIG != NULL) || (ttvfig->EXTSIG != NULL))
        {
         if(TTV_LAST_SIG != TTV_SIG_I)
          {
           if(ttvfig->INTSIG != NULL)
             ttvfig->INTSIG = ttv_allocreflist((chain_list*)ttvfig->INTSIG,ttvfig->NBINTSIG) ;
           if(ttvfig->EXTSIG != NULL)
             ttvfig->EXTSIG = ttv_allocreflist((chain_list*)ttvfig->EXTSIG,ttvfig->NBEXTSIG) ;
          }
        }
       else if((ttvfig->EPRESIG != NULL) || (ttvfig->IPRESIG != NULL))
        {
         if(TTV_LAST_SIG != TTV_SIG_R)
          {
           if(ttvfig->EPRESIG != NULL)
           ttvfig->EPRESIG = ttv_allocreflist((chain_list*)ttvfig->EPRESIG,ttvfig->NBEPRESIG) ;
           if(ttvfig->IPRESIG != NULL)
           ttvfig->IPRESIG = ttv_allocreflist((chain_list*)ttvfig->IPRESIG,ttvfig->NBIPRESIG) ;
          }
        }
       else if((ttvfig->EBREAKSIG != NULL) || (ttvfig->IBREAKSIG != NULL))
        {
         if(TTV_LAST_SIG != TTV_SIG_B)
          {
           if(ttvfig->EBREAKSIG != NULL)
           ttvfig->EBREAKSIG = ttv_allocreflist((chain_list*)ttvfig->EBREAKSIG,ttvfig->NBEBREAKSIG) ;
           if(ttvfig->IBREAKSIG != NULL)
           ttvfig->IBREAKSIG = ttv_allocreflist((chain_list*)ttvfig->IBREAKSIG,ttvfig->NBIBREAKSIG) ;
          }
        }
       else if((ttvfig->ELATCHSIG != NULL) || (ttvfig->ILATCHSIG != NULL))
        {
         if(TTV_LAST_SIG != TTV_SIG_L)
          {
           if(ttvfig->ELATCHSIG != NULL)
             ttvfig->ELATCHSIG = ttv_allocreflist((chain_list*)ttvfig->ELATCHSIG,
                                                  ttvfig->NBELATCHSIG) ;
           if(ttvfig->ILATCHSIG != NULL)
             ttvfig->ILATCHSIG = ttv_allocreflist((chain_list*)ttvfig->ILATCHSIG,
                                                  ttvfig->NBILATCHSIG) ;
          }
        }
       else if((ttvfig->ELCMDSIG != NULL) || (ttvfig->ILCMDSIG != NULL))
        {
         if(TTV_LAST_SIG != TTV_SIG_Q)
          {
           if(ttvfig->ELCMDSIG != NULL)
             ttvfig->ELCMDSIG = ttv_allocreflist((chain_list*)ttvfig->ELCMDSIG,
                                                 ttvfig->NBELCMDSIG) ;
           if(ttvfig->ILCMDSIG != NULL)
             ttvfig->ILCMDSIG = ttv_allocreflist((chain_list*)ttvfig->ILCMDSIG,
                                                 ttvfig->NBILCMDSIG) ;
          }
        }
      }
  
   if(((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE) && (ttvfig->ESIG != NULL))
      {
       if(TTV_LAST_SIG != TTV_SIG_S)
         ttvfig->ESIG = ttv_allocreflist((chain_list*)ttvfig->ESIG,ttvfig->NBESIG) ;
      }
  }

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
  {
   ttvfig = (ttvfig_list *)chain->DATA ;
   while((ttvfig != NULL) && ((ttvfig->STATUS & TTV_STS_HEAD) != TTV_STS_HEAD))
    ttvfig = ttvfig->ROOT ;
   if(ttvfig != NULL)
    if((ttvfig->STATUS & TTV_STS_MARQUE) != TTV_STS_MARQUE)
     {
      chainhead = addchain(chainhead,ttvfig) ;
      ttvfig->STATUS |= TTV_STS_MARQUE ;
     }
  }

  freechain(chainfig) ;

  for(chain = chainhead ; chain != NULL ; chain = chain->NEXT)
    ttv_freeallttvfig((ttvfig_list *)chain->DATA) ;

  freechain(chainhead) ;

  ttv_error(10,chlinecount,TTV_ERROR) ;
}

/*****************************************************************************/
/*                        function ttv_decodcontype()                        */
/* parametres :                                                              */
/* type : type de connecteur                                                 */
/*                                                                           */
/* renvoie le type de connecteur en fonction de l'article dans le fichier    */
/* renvoie 0 si il y a une erreur                                            */
/*****************************************************************************/
long ttv_decodcontype(type)
char *type ;
{
 long typeq = 0L ;

 if(type[1] == 'Q') typeq = TTV_SIG_Q ;
 else if(type[1] == 'L') typeq = TTV_SIG_L ;
 else if(type[1] == 'B') typeq = TTV_SIG_B ;
 else if(type[1] == 'R') typeq = TTV_SIG_R ;
 else if(type[1] != '\0') return(0L) ;

 switch (type[0])
  {
   case 'I' :
             return (typeq | TTV_SIG_CI) ;
   case 'O' :
             return (typeq | TTV_SIG_CO) ;
   case 'Z' :
             return (typeq | TTV_SIG_CZ) ;
   case 'B' :
             return (typeq | TTV_SIG_CB) ;
   case 'T' :
             return (typeq | TTV_SIG_CT) ;
   case 'X' :
             return (typeq | TTV_SIG_CX) ;
   default :
             return(0L);
  }
}

/*****************************************************************************/
/*                        function ttv_decodlatchtype()                      */
/* parametres :                                                              */
/* type : type de latch                                                      */
/*                                                                           */
/* renvoie le type de latch en fonction de l'article dans le fichier         */
/* renvoie 0 si il y a une erreur                                            */
/*****************************************************************************/
long ttv_decodlatchtype(type)
char *type ;
{
 long typeq ;
 
 if(type[1] == 'C') typeq = TTV_SIG_C | TTV_SIG_EXT ;
 else if(type[1] == 'N') typeq = TTV_SIG_N | TTV_SIG_EXT ;
 else if(type[1] == 'E') typeq = TTV_SIG_EXT ;
 else if(type[1] == 'I') typeq = (long)0 ;
 else return(0L) ;
 
 switch (type[0])
  {
   case 'R' :
             return (typeq | TTV_SIG_LR) ;
   case 'S' :
             return (typeq | TTV_SIG_LS) ;
   case 'L' :
             return (typeq | TTV_SIG_LL) ;
   case 'F' :
             return (typeq | TTV_SIG_LF) ;
   default :
             return(0L);
  }
}

/*****************************************************************************/
/*                        function ttv_decodsigtype()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* index : index du signal                                                   */
/* type : type du signal                                                     */
/*                                                                           */
/* renvoie le signal en fonction du type et de l'index du signal             */
/* renvoie NULL en cas d'erreur                                              */
/*****************************************************************************/
ttvsig_list *ttv_decodsigtype(ttvfig,index,type)
ttvfig_list *ttvfig ;
long index ;
char type ;
{
 long typel ;

 switch (type)
  {
   case 'C' : typel = TTV_SIG_C ;
              break ;
   case 'Q' : typel = TTV_SIG_Q ;
              break ;
   case 'L' : typel = TTV_SIG_L ;
              break ;
   case 'B' : typel = TTV_SIG_B ;
              break ;
   case 'I' : typel = TTV_SIG_I ;
              break ;
   case 'N' : typel = TTV_SIG_N ;
              break ;
   case 'S' : typel = TTV_SIG_S ;
              break ;
   case 'P' : typel = TTV_SIG_R ;
              break ;
   case 'E' : typel = TTV_SIG_R ;
              break ;
   case 'R' : typel = TTV_SIG_R ;
              break ;
   case 'Z' : typel = TTV_SIG_L ;
              break ;
   case 'O' : typel = TTV_SIG_L ;
              break ;
   default  : return (NULL) ;
  } 

 return(ttv_getsigbyindex(ttvfig,index,typel)) ;
}

/*****************************************************************************/
/*                        function ttv_getnextchain()                        */
/* parametres :                                                              */
/* donne le nouveau token                                                    */
/*****************************************************************************/
chain_list *ttv_getnextchain(chain)
chain_list *chain ;
{
 while(chain)
  {
   chain = chain->NEXT ;
   if(chain == NULL)
      break ;
   if(chain->DATA != NULL)
     break ;
  }

 return(chain) ;
}

/*****************************************************************************/
/*                        function ttv_parsnc()                              */
/* parametres :                                                              */
/* retrouve le nom du net et sa capa                                         */
/*****************************************************************************/
typedef struct
{
  long prop;
  int gate;
  long riseslope, fallslope;
  float outcapa;
  int pnode[2];
  float low, high;
} more_signal_info;

static chain_list *ttv_parsnc(float *capa, char **net, chain_list *chain,char *nomsig, long type, ttvfig_list *tvf, more_signal_info *msi, int mode)
{
 char *item;
 chain_list *chain0;
 if (mode==0 || mode==1)
 {
   *capa = 0.0 ;
   *net = nomsig ;
   msi->prop=0;
   msi->gate=0;
   msi->fallslope=msi->riseslope=-1;
   msi->outcapa=-1;
   msi->pnode[0]=msi->pnode[1]=-1;
   msi->low=msi->high=-1;
 }
 if(chain->NEXT == NULL) return(chain) ;
 if (mode!=2) chain = chain->NEXT ;

 if(mode!=2 && chain->DATA == NULL)
   {
    if (mode==0)
    {
      if((type & (TTV_SIG_C|TTV_SIG_N)) != 0) 
       {
        if((chain = ttv_getnextchain(chain)) != NULL)
         {
          *capa = atof((char*)chain->DATA) ;

          if((type & TTV_SIG_N) == TTV_SIG_N) 
            if((chain = ttv_getnextchain(chain)) != NULL)
            *capa = atof((char*)chain->DATA) ;
         }
        if(chain)
         {
          while(chain->NEXT != NULL)
           {
            chain = chain->NEXT ;
           }
         }
       }
    }
   }
 else
   {
    if (mode==0 || mode==1)
    {
      *net = (char*)chain->DATA ;
       if((chain = ttv_getnextchain(chain)) != NULL)
        {
         *capa = atof((char*)chain->DATA) ;
        }
    }
    if (mode==0 || mode==2)
    {
      if (tvf->INFO->DTB_VERSION>=1)
      {
        if (((type & TTV_SIG_C)!=0 || (tvf->INFO->DTB_VERSION>=3 && (type & TTV_SIG_B)!=0)) && tvf!=NULL && (chain = ttv_getnextchain(chain)) != NULL)
        {
          char *nnet=namealloc(*net);
          msi->fallslope=mbk_long_round(atof((char*)chain->DATA)*TTV_UNIT);
          if ((chain = ttv_getnextchain(chain)) != NULL)
            msi->riseslope=mbk_long_round(atof((char*)chain->DATA)*TTV_UNIT);
          if ((chain = ttv_getnextchain(chain)) != NULL)
            msi->outcapa=atof((char*)chain->DATA);
          if (tvf->INFO->DTB_VERSION>=3)
          {
            if ((chain = ttv_getnextchain(chain)) != NULL && strcmp((char*)chain->DATA,".")!=0)
              msi->low=atof((char*)chain->DATA);
            if ((chain = ttv_getnextchain(chain)) != NULL && strcmp((char*)chain->DATA,".")!=0)
              msi->high=atof((char*)chain->DATA);
          }
        }
        if ((chain = ttv_getnextchain(chain)) != NULL)
        {
          item=(char *)chain->DATA;
          if (strcmp(item,"M")==0) msi->prop|=TTV_SIG_FLAGS_ISMASTER;
          else if (strcmp(item,"S")==0) msi->prop|=TTV_SIG_FLAGS_ISSLAVE;
        }
        if ((chain = ttv_getnextchain(chain)) != NULL)
        {
          item=(char *)chain->DATA;
          if (strcmp(item,".")!=0) msi->gate=atoi(item);
        }
      }
      if (tvf->INFO->DTB_VERSION>=2)
      {
        char *end;
        chain0=chain;
        if ((chain = ttv_getnextchain(chain)) != NULL && *((char*)chain->DATA)=='\'')
        {
           msi->pnode[0]=strtol(((char*)chain->DATA)+1, &end, 10);
           if ((chain = ttv_getnextchain(chain)) != NULL && *((char*)chain->DATA)=='\'')
             msi->pnode[1]=strtol(((char*)chain->DATA)+1, &end, 10);
        } else chain=chain0;
      }
    }
   }
 return(chain) ;
}

static void setpropandgate(ttvsig_list *tvs, more_signal_info *msi)
{
  if (msi->prop!=0) ttv_setsigflag(tvs, msi->prop);
  if (msi->gate!=0) ttv_setsigflaggate(tvs, msi->gate);
  if (msi->fallslope>0) ttv_set_UTD_slope(&tvs->NODE[0], msi->fallslope);
  if (msi->riseslope>0) ttv_set_UTD_slope(&tvs->NODE[1], msi->riseslope);
  if (msi->outcapa>=0) ttv_set_UTD_outputcapa(tvs, msi->outcapa);
  tvs->PNODE[0]=msi->pnode[0];
  tvs->PNODE[1]=msi->pnode[1];
  if (msi->high!=msi->low)
    ttv_set_signal_swing(tvs, msi->low, msi->high);
}

/*****************************************************************************/
/*                        function ttv_parscmd()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* chain : chain des articles decrivant la liste des commandes               */
/* ptsig : signal latch ou precharge                                         */
/*                                                                           */
/* met a jour la liste des commandes des latchs et des precharges et         */
/* retourne la nouvelle liste                                                */
/* renvoie le dernier element de la chain si c'est bon et NULL si il y a une */
/* erreur                                                                    */
/*****************************************************************************/
chain_list *ttv_parscmd(ttvfig,chain,ptsig) 
ttvfig_list *ttvfig ;
chain_list *chain ;
ttvsig_list *ptsig ;
{
 chain_list *chainav ;
 chain_list *chainend ;
 ptype_list *ptype ;

 chainend = NULL ;

 if((ptype = getptype(ptsig->USER,TTV_SIG_CMD)) != NULL)
   {
    for(chainav = (chain_list *)ptype->DATA ; chainav != NULL ;
        chainav = chainav->NEXT)
      {
       ((ttvevent_list *)chainav->DATA)->TYPE |= TTV_NODE_CMDMARQUE ;
       chainend = chainav ;
      }
   }
 else
   {
    ptype =  ptsig->USER = addptype(ptsig->USER,TTV_SIG_CMD,(void*)NULL) ;
   }

 chainav = NULL ;

 if(chain != NULL)
  {
   if(chain->DATA == NULL)
    chain = chain->NEXT ;
  }

 for(; chain != NULL ;chain = chain->NEXT)
   {
    ttvsig_list *ptsigx ;
    ttvevent_list *ptnode ;

    if(chain->DATA == NULL)
     {
      chainav = chain ;
      chain = chain->NEXT ;
      break ;
     }

    ptsigx = ttv_getsigbyindex(ttvfig,
                          atol((char*)chain->DATA),TTV_SIG_Q) ;
    chain = chain->NEXT ;
    if(chain == NULL) return(NULL) ;
    if(*((char *)chain->DATA) == 'U')
       ptnode = ptsigx->NODE+1 ;
    else
       ptnode = ptsigx->NODE ;
    if((ptnode->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
     {
      if(chainend == NULL) 
      {
       chainend = ptype->DATA = 
                  addchain((chain_list *)ptype->DATA,(void *)ptnode) ;
      }
    else
      {
       chainend->NEXT = addchain(chainend->NEXT,(void *)ptnode) ;
       chainend = chainend->NEXT ;
      }
     }
    chainav = chain ;
   }

 for(chain = (chain_list *)ptype->DATA ; chain != NULL ;chain = chain->NEXT)
   ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;

 return(chainav) ;
}

/*****************************************************************************/
/*                        function ttv_parsttvline()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* ptsig1 : premier signal du lien                                           */
/* ptsig2 : deuxieme signal du lien                                          */
/* type : type de lien                                                       */
/* chain : chain des articles decrivant le lien                              */
/* typesig2 : type du deuxieme signal pour la precharge PR ou EV             */ 
/* status : etat de la ttvfig demande pour les arcs elementaire hors chemin  */
/*                                                                           */
/* parse les arcs elementaires et les chemins                                */
/* renvoie le dernier element de la chain si c'est bon et NULL si il y a une */
/* erreur                                                                    */
/*****************************************************************************/
chain_list *ttv_parsttvline(ttvfig,ptsig1,ptsig2,type,chain,typesig2,status)
ttvfig_list *ttvfig ;
ttvsig_list *ptsig1 ;
ttvsig_list *ptsig2 ;
long type ;
chain_list *chain ;
char typesig2 ;
long status ;
{
 ttvevent_list *ptnode1 ;
 ttvevent_list *ptnode2 ;
 ttvevent_list *ptnodecmd ;
 ttvline_list *ptline ;
 chain_list *chainav = NULL ;
 char *path ;
 char *model ;
 char *slope ;
 long v,f,r,s ;
 long types ;
 long typeline ;
 char f1,f2 ;
 char f1x = 'X',f2x = 'X',prev = 'X' ;

 if(chain == NULL) return(chain) ;

 if((ptsig1 == NULL) || (ptsig2 == NULL))
  return(NULL) ;

 types = type ;

 for(; chain != NULL ; chain = chain->NEXT)
   {
    if(chain->DATA == NULL)
     {
      chain = chain->NEXT ;
      if(chain == NULL)
       break ;
     }
    type = types ;
    if(((ptsig2->TYPE & TTV_SIG_L) == TTV_SIG_L) && 
        isdigit((int)*(char *)chain->DATA))
     {
      ttvsig_list *ptsigcmd = ttv_getsigbyindex(ttvfig,atol((char*)chain->DATA),
                                                TTV_SIG_Q);
      if((chain = chain->NEXT) == NULL) return(NULL) ;
      if((*(char *)chain->DATA) == 'U') ptnodecmd = ptsigcmd->NODE+1 ;
      else if((*(char *)chain->DATA) == 'D') ptnodecmd = ptsigcmd->NODE ;
      else return(NULL) ;
      if((chain = chain->NEXT) == NULL) return(NULL) ;
     }
    else ptnodecmd = NULL ;
    
    if(chain->DATA == NULL)
     {
      chain = chain->NEXT ;
      if(chain == NULL)
       break ;
     }

    f1 = *(char *)chain->DATA ;
    f2 = *((char *)chain->DATA+1) ;
    
    if(f1 == 'U') ptnode1 = ptsig1->NODE+1 ;
    else if(f1 == 'D') ptnode1 = ptsig1->NODE ;
    else return(NULL) ;

    if(f2 == 'U') ptnode2 = ptsig2->NODE+1 ;
    else if(f2 == 'D') ptnode2 = ptsig2->NODE ;
    else if(f2 == 'Z')
       {
        if(typesig2 == 'E') ptnode2 = ptsig2->NODE ;
        else if(typesig2 == 'P') ptnode2 = ptsig2->NODE+1 ;
        else ptnode2 = ptsig2->NODE ;
       }
    else if(f2 == 'Y')
       {
        if(typesig2 == 'E') ptnode2 = ptsig2->NODE ;
        else if(typesig2 == 'P') ptnode2 = ptsig2->NODE+1 ;
        else ptnode2 = ptsig2->NODE + 1 ;
       }
    else return(NULL) ;
    
    if((chain = chain->NEXT) == NULL) return(NULL) ;
    path = (char *)chain->DATA ;

    if((chain = chain->NEXT) == NULL) return(NULL) ;
    v = mbk_long_round(atof((char *)chain->DATA) * TTV_UNIT) ;
    if((chain = chain->NEXT) == NULL) return(NULL) ;
    f = mbk_long_round(atof((char *)chain->DATA) * TTV_UNIT) ;
 
    if((*path == 'i') || (*path == 'I'))
     {
      type |= TTV_LINE_RC ;
      path++ ;
     }
    else if((*path == 's') || (*path == 'S'))
     {
      type |= TTV_LINE_U ;
      path++ ;
     }
    else if((*path == 'h') || (*path == 'H'))
     {
      type |= TTV_LINE_O ;
      path++ ;
     }
    else if((*path == 'a') || (*path == 'A'))
     {
      type |= TTV_LINE_A ;
      path++ ;
     }

    if(strcmp(path,"MAX") == 0) typeline = TTV_LINE_DEPTMAX ;
    else if(strcmp(path,"MIN") == 0) typeline = TTV_LINE_DEPTMIN ;
    else if(strcmp(path,"max") == 0) typeline = TTV_LINE_DENPTMAX ;
    else if(strcmp(path,"min") == 0) typeline = TTV_LINE_DENPTMIN ;
    else return(NULL) ;
 

    model = NULL ;
    slope = NULL ;

    if(chain->NEXT != NULL)
     {
      if(chain->NEXT->DATA != NULL)
       {
        if(*((char *)chain->NEXT->DATA) == 'M')
         {
          chain = chain->NEXT ;
          chain = chain->NEXT ;
          model = namealloc((char *)chain->DATA) ;
          if((((type & (TTV_LINE_O|TTV_LINE_U)) == 0) || 
              ((type & TTV_LINE_A) == TTV_LINE_A))  && (chain->NEXT != NULL))
          {
           chain = chain->NEXT ;
           if(!chain->DATA) return (NULL);
           slope = namealloc((char *)chain->DATA) ;
          }
         }
        else
         {
          if((ptsig1->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
           {
            if((chain = chain->NEXT) == NULL) return(NULL) ;
            s = atol((char *)chain->DATA) ;
           }
          else
           s = TTV_NOS ;
          if((ptsig2->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
           {
            if((chain = chain->NEXT) == NULL) return(NULL) ;
            r = atol((char *)chain->DATA) ;
           }
          else
           r = TTV_NORES ;
         }
       }
      if(chain->NEXT->DATA == NULL)
       {
        chain = chain->NEXT ;
       }
     }

   if(((ptsig2->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
      ((ptsig2->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
      ((ptsig2->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
     {
      if((f2 == 'Z') || ((f2 == 'Y'))) type |= TTV_LINE_HZ ;
      if(typesig2 == 'P') type |= TTV_LINE_PR ;
      else if(typesig2 == 'E') type |= TTV_LINE_EV ;
     }

   if(((ptsig2->TYPE & TTV_SIG_LS) == TTV_SIG_LS) ||
      ((ptsig2->TYPE & TTV_SIG_LR) == TTV_SIG_LR))
     {
      if(typesig2 == 'Z') type |= TTV_LINE_R ;
      else if(typesig2 == 'O') type |= TTV_LINE_S ;
     }

    type |= (typeline & (TTV_LINE_DENPTMAX|TTV_LINE_DENPTMIN)) ;

    ptline = ttv_getline(ttvfig,ttvfig,ptnode1,ptnode2,NULL,type,(long)0,1,1);
    if(ptline != NULL)
     {
      if((typeline & (TTV_LINE_DENPTMAX|TTV_LINE_DEPTMAX)) != 0)
       {
        if(ptline->VALMAX != TTV_NOTIME)
          ptline = NULL ;
       }
      else
       {
        if(ptline->VALMIN != TTV_NOTIME)
          ptline = NULL ;
       }
     }

    if((typeline == TTV_LINE_DEPTMAX) || ((typeline == TTV_LINE_DENPTMAX) && 
       (((status & TTV_STS_DENOTINPT) == TTV_STS_DENOTINPT) ||
       ((ttvfig->STATUS & TTV_STS_DENOTINPT) == TTV_STS_DENOTINPT))))
      {
       if(ptline == NULL)
        {
         ptline = ttv_addline(ttvfig,ptnode2,ptnode1,v,f,TTV_NOTIME,
                                                         TTV_NOSLOPE,type) ;

         ttv_addcaracline(ptline,model,NULL,slope,NULL) ;
        }
       else
        {
         ptline->VALMAX = v ;
         ptline->FMAX = f ;
         ptline->MDMAX = model ;
         ptline->MFMAX = slope ;
         ptline->TYPE |= type ;
        }

       if(ptnodecmd != NULL)
          ttv_addcmd(ptline,TTV_LINE_CMDMAX,ptnodecmd) ;

       prev = 'M' ;
       f1x = f1 ;
       f2x = f2 ;
      }
    else
    if((typeline == TTV_LINE_DEPTMIN) || ((typeline == TTV_LINE_DENPTMIN) &&
       (((status & TTV_STS_DENOTINPT) == TTV_STS_DENOTINPT) ||
       ((ttvfig->STATUS & TTV_STS_DENOTINPT) == TTV_STS_DENOTINPT))))
      {
       if(ptline == NULL)
        {
         ptline = ttv_addline(ttvfig,ptnode2,ptnode1,TTV_NOTIME,
                                                     TTV_NOSLOPE,v,f,type) ;

         ttv_addcaracline(ptline,NULL,model,NULL,slope) ;
        }
       else
        {
         ptline->VALMIN = v ;
         ptline->FMIN = f ;
         ptline->MDMIN = model ;
         ptline->MFMIN = slope ;
         ptline->TYPE |= type ;
        }

       if(ptnodecmd != NULL)
          ttv_addcmd(ptline,TTV_LINE_CMDMIN,ptnodecmd) ;

       prev = 'm' ;
       f1x = f1 ;
       f2x = f2 ;
      }
    chainav = chain ;
   }

 return(chainav) ;
}

/*****************************************************************************/
/*                        function ttv_fgetsline()                           */
/* parametres :                                                              */
/* file : pointeur sur le fichier parse                                      */
/* filename : nom du fichier parser                                          */
/* buf : buffer ou l'on met la ligne a parser                                */
/* linecount : numero de ligne courant                                       */
/*                                                                           */
/* place tout les carracteres du fichier file jusqu'qu ";" dans le buffer    */
/* et renvoie le nouveau numero de ligne                                     */
/*****************************************************************************/
long ttv_fgetsline(file,filename,linecount)
FILE *file ;
char *filename ;
long linecount ;
{
 char *bufx ;
 char *buf = bufferline ;
 int size = buffersize ;
 int len ;

 TTV_LAST_LINECOUNT = linecount ;
 linecount++ ;

 if(fgets(buf,size,file) == NULL) 
    {
     ttv_parserror(filename,linecount) ;
    }
 
 bufx = buf ;

 while(isspace((int)*bufx) != 0)
   {
    if(*bufx == '\n')
     {
      if(fgets(buf,size,file) == NULL) 
         {
          ttv_parserror(filename,linecount) ;
         }
      linecount++ ;
      bufx = buf ;
     }
    else
     {
      bufx++ ;
     }
   }

 if(*buf != '#')
 while(strchr(buf,';') == NULL) 
   {
    len = strlen(buf) ;
    buf = buf + len ;
    if((*(buf - 1) != '\n')||(size-len < 100))
     {
      buffersize += buffersize ;
      bufferline = mbkrealloc(bufferline,buffersize) ;
      len = strlen(bufferline) ;
      buf = bufferline + len ;
      size = buffersize - len ;
     }
    else
     {
      size -= len ;
     }
    if(fgets(buf,size,file) == NULL) 
       {
        ttv_parserror(filename,linecount) ;
       }
    linecount++ ;
   }

 return(linecount) ;
}

/*****************************************************************************/
/*                        function ttv_getlinearg()                          */
/* parametres :                                                              */
/* buf : buffer ou l'on trouve la ligne a parser                             */
/*                                                                           */
/* analyse une ligne et renvoie une chaine liste de tout les articles        */
/* contenus dans cette ligne ou NULL si il y a une erreur                    */
/*****************************************************************************/
chain_list *ttv_getlinearg(buf)
char *buf ;
{
 chain_list *chain = NULL ;
 int open = 0 ;
 int close = 0 ;
 int argavant = 0 ;

 for(;;)
   {
    if(isspace((int)*buf) != 0)
      {
       if(argavant == 1) 
        {
         *buf = '\0' ;
         argavant = 0 ;
        }
       buf++ ;
      }
    else if((isalnum((int)*buf) != 0) || 
            (strchr("'!{}\\:/$<>[]._-|+@\\",(int)*buf) != NULL))
      { 
       argavant = 1 ;
       chain = addchain(chain,(void*)buf) ;
       buf++ ;
       while((isalnum((int)*buf) != 0) ||
             (strchr("'!{}\\:/$<>[]._-|+@\\",(int)*buf) != NULL))
        {
         buf++ ;
        }
      }
    else 
      {
       switch(*buf)
        {
         case '(' :
                    if(argavant == 1) 
                     {
                      *buf = '\0' ;
                      argavant = 0 ;
                     }
                    if(chain != NULL)
                     {
                      if(chain->DATA != NULL)
                        chain = addchain(chain,NULL) ;
                     }
                    open++ ;
                    buf++ ;
                    break ;
         case ')' :
                    if(argavant == 1) 
                     {
                      *buf = '\0' ;
                      argavant = 0 ;
                     }
                    if(chain != NULL)
                     {
                      if(chain->DATA != NULL)
                        chain = addchain(chain,NULL) ;
                     }
                    close++ ;
                    buf++ ;
                    break ;
         case ';' : if(argavant == 1) 
                      *buf = '\0' ;
                    argavant = 0 ;
                    if((open != close) || (argavant == 1))  
                     {
                      freechain(chain) ;
                      return(NULL) ;
                     }
                    return(reverse(chain)) ;
         default  : freechain(chain) ;
                    return(NULL) ;
        }
      }
   }

 if((open != close) || (argavant == 1))  
  {
   freechain(chain) ;
   return(NULL) ;
  }

 return(reverse(chain)) ;
}

/*****************************************************************************/
/*                        function ttv_parsline()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* buf : buffer ou l'on trouve la ligne a parser                             */
/* linecount : numero de la ligne courante                                   */
/* status : etat de la ttvfig demande                                        */
/* filename : nom du fichier                                                 */
/*                                                                           */
/* parse l' article contenu dans le buffer en fonction de l'etat de l'etat   */
/* de la figure ttv que l'on desire                                          */
/* renvoie la ttvfig dans le cas ou la ttvfig et dans l'etat demander        */
/* et NULL s'il faut continuer                                               */
/*****************************************************************************/
ttvfig_list *ttv_parsline(ttvfig_list *ttvfig,char *buf,long linecount,long status,char *filename)
{
 chain_list *chainx ;
 chain_list *chain ;
 chain_list *chainref ;
 char  *nomsig ;
 char  *net ;
 float capa ;
 long  type ;
 ttvfig_list *ttvins ;
 ttvsig_list *ttvsig ;
 ttvsig_list *ttvsig1,*ttvsig2 ;
 long index1,index2 ;
 char type1,type2 ;
 ttvinfo_list info ;
 more_signal_info msi;
 int trig_bracket;

 switch(*buf)
   {
    case '#' : return NULL;
    case 'H' : buf++ ;
               if (*buf!=' ') info.DTB_VERSION=(*buf)-'0', buf++;
               else info.DTB_VERSION=0;
               chain = chainx = ttv_getlinearg(buf) ;
               if(chainx == NULL) 
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TOOLNAME = namealloc((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TOOLVERSION = namealloc((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(namealloc((char*)chainx->DATA) != ttvfig->INFO->FIGNAME)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TECHNONAME = namealloc((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TECHNOVERSION = namealloc((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.SLOPE = atol((char*)chainx->DATA) * TTV_UNIT ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.CAPAOUT = atof((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(chainx->NEXT != NULL)
                 if(chainx->NEXT->DATA != NULL)
                  {
                   info.STHHIGH = atof((char*)chainx->DATA) ; 
                   if((chainx = ttv_getnextchain(chainx)) == NULL)
                     {
                      freechain(chain) ;
                      ttv_parserror(filename,linecount) ;
                     }
                   info.STHLOW = atof((char*)chainx->DATA) ; 
                   if((chainx = ttv_getnextchain(chainx)) == NULL)
                     {
                      freechain(chain) ;
                      ttv_parserror(filename,linecount) ;
                     }
                   info.DTH = atof((char*)chainx->DATA) ; 
                   if((chainx = ttv_getnextchain(chainx)) == NULL)
                     {
                      freechain(chain) ;
                      ttv_parserror(filename,linecount) ;
                     }
                   info.TEMP = atof((char*)chainx->DATA) ; 
                   if((chainx = ttv_getnextchain(chainx)) == NULL)
                     {
                      freechain(chain) ;
                      ttv_parserror(filename,linecount) ;
                     }
                   if (((char*)chainx->DATA)[0]=='@')
                   {
                     info.TNOM=atof(&((char*)chainx->DATA)[1]);
                     if((chainx = ttv_getnextchain(chainx)) == NULL)
                     {
                      freechain(chain) ;
                      ttv_parserror(filename,linecount) ;
                     }
                   }
                   else info.TNOM=-1000;
                   info.VDD = atof((char*)chainx->DATA) ; 
                   if((chainx = ttv_getnextchain(chainx)) == NULL)
                     {
                      freechain(chain) ;
                      ttv_parserror(filename,linecount) ;
                     }
                  }
               ttvfig->INFO->LEVEL = atol((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TTVDAY = atoi((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TTVMONTH = atoi((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TTVYEAR = atoi((char*)chainx->DATA) ; 
               if(((info.TTVYEAR >= (long)95) && (info.TTVYEAR <= (long)99)) ||
                  ((info.TTVYEAR >= (long)100) && (info.TTVYEAR < (long)195)))
                    info.TTVYEAR = info.TTVYEAR + (long)1900 ;
               else if((info.TTVYEAR >= (long)0) && (info.TTVYEAR < (long)95))
                    info.TTVYEAR = info.TTVYEAR + (long)2000 ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TTVHOUR = atoi((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TTVMIN = atoi((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               info.TTVSEC = atoi((char*)chainx->DATA) ; 
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               if((ttvfig->STATUS & TTV_STS_HEADER) == TTV_STS_HEADER)
                {
                 if(ttv_checkdate(ttvfig->INFO,&info) != 0)
                   ttv_error(40,filename,TTV_ERROR) ;
                 if(ttv_checktechno(ttvfig->INFO,&info) != 0)
                   ttv_error(40,filename,TTV_ERROR) ;
                 if(ttv_checktool(ttvfig->INFO,&info) != 0)
                   ttv_error(40,filename,TTV_ERROR) ;
                 return(NULL) ;
                }
               else
                {
                 ttvfig->INFO->TOOLNAME = info.TOOLNAME ;
                 ttvfig->INFO->TOOLVERSION = info.TOOLVERSION ;
                 ttvfig->INFO->TECHNONAME = info.TECHNONAME ;
                 ttvfig->INFO->TECHNOVERSION = info.TECHNOVERSION ;
                 ttvfig->INFO->SLOPE = info.SLOPE ;
                 ttvfig->INFO->CAPAOUT = info.CAPAOUT ;
                 ttvfig->INFO->STHHIGH = info.STHHIGH ;
                 ttvfig->INFO->STHLOW = info.STHLOW ;
                 ttvfig->INFO->DTH = info.DTH ;
                 ttvfig->INFO->TEMP = info.TEMP ;
                 ttvfig->INFO->TNOM = info.TNOM ;
                 ttvfig->INFO->VDD = info.VDD ;
                 ttvfig->INFO->TTVDAY = info.TTVDAY ;
                 ttvfig->INFO->TTVMONTH = info.TTVMONTH ;
                 ttvfig->INFO->TTVYEAR = info.TTVYEAR ;
                 ttvfig->INFO->TTVHOUR = info.TTVHOUR ;
                 ttvfig->INFO->TTVMIN = info.TTVMIN ;
                 ttvfig->INFO->TTVSEC = info.TTVSEC ;
                 ttvfig->INFO->DTB_VERSION = info.DTB_VERSION ;
                }
               return(NULL) ;
    case 'X' : if(((ttvfig->STATUS & TTV_STS_HEADER) == TTV_STS_HEADER) &&
                  ((((status & TTV_STS_S) != TTV_STS_S) &&
                    ((status & TTV_STS_SE) != TTV_STS_SE)) ||
                  ((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE)))
                 return(NULL) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL) 
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(chainx->NEXT == NULL) 
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(chainx->NEXT->NEXT != NULL) 
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               ttvins = ttv_givettvfig((char*)chainx->DATA,
                        (char*)chainx->NEXT->DATA,ttvfig) ;
               freechain(chain) ;
               return(ttvins) ;
    case 'C' : if(((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C) ||
                  ((status & TTV_STS_C) != TTV_STS_C))
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)  
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               type = ttv_decodcontype((char*)chainx->DATA) ;
               if(type == 0L)   
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               nomsig = (char*)chainx->DATA ;  
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,type, ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(((type & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B)) == 0) || 
                  ((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L))
                {
                 chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                }
               else
                {
                 if((type & TTV_SIG_Q) == TTV_SIG_Q)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_Q) ;
                 else if((type & TTV_SIG_L) == TTV_SIG_L)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_L) ;
                 else if((type & TTV_SIG_R) == TTV_SIG_R)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_R) ;
                 else if((type & TTV_SIG_B) == TTV_SIG_B)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_B) ;
                 if(ttvsig == NULL)   
                  {
                   freechain(chain) ;
                   ttv_parserror(filename,linecount) ;
                  }
                 ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                 chainref = addchain(NULL,(void*)ttvsig) ;
                 ttvsig->TYPE |= type ;
                }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               setpropandgate(ttvsig, &msi);

               if(index1 != (ttvfig->NBCONSIG + 1L))
                 {
                  ttvfig->NBCONSIG++ ;
                  chainref->NEXT = (chain_list*)ttvfig->CONSIG ;
                  ttvfig->CONSIG = (ttvsig_list **)chainref ;
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  ttvfig->NBCONSIG++ ;
                  chainref->NEXT = (chain_list*)ttvfig->CONSIG ;
                  ttvfig->CONSIG = (ttvsig_list **)chainref ;
                 }
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               freechain(chain) ;
               return(NULL) ;
    case 'N' : if(((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C) || 
                  ((status & TTV_STS_C) != TTV_STS_C))
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;
               if(chainx == NULL)    
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
               if(*((char*)chainx->DATA) == 'I')
                  type = TTV_SIG_N ;
               else if(*((char*)chainx->DATA) == 'Q')
                  type = TTV_SIG_N | TTV_SIG_Q ;
               else if(*((char*)chainx->DATA) == 'R')
                  type = TTV_SIG_N | TTV_SIG_R ;
               else if(*((char*)chainx->DATA) == 'L')
                  type = TTV_SIG_N | TTV_SIG_L ;
               else if(*((char*)chainx->DATA) == 'B')
                  type = TTV_SIG_N | TTV_SIG_B ;
               else
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,type & ~TTV_SIG_B,ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(((type & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B)) == 0) || 
                  ((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L))
                 chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
               else
                {
                 if((type & TTV_SIG_Q) == TTV_SIG_Q)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_Q) ;
                 else if((type & TTV_SIG_L) == TTV_SIG_L)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_L) ;
                 else if((type & TTV_SIG_R) == TTV_SIG_R)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_R) ;
                 else if((type & TTV_SIG_B) == TTV_SIG_B)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_B) ;
                 if(ttvsig == NULL)    
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                 chainref = addchain(NULL,(void*)ttvsig) ;
                 ttvsig->TYPE |= type ;
                }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               setpropandgate(ttvsig, &msi);
               if(index1 != (ttvfig->NBNCSIG + 1L))
                 {
                  chainref->NEXT = (chain_list*)ttvfig->NCSIG ;
                  ttvfig->NCSIG = (ttvsig_list **)chainref ;
                  ttvfig->NBNCSIG++ ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  chainref->NEXT = (chain_list*)ttvfig->NCSIG ;
                  ttvfig->NBNCSIG++ ;
                  ttvfig->NCSIG = (ttvsig_list **)chainref ;
                 }
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'Q' : if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)    
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(*((char*)chainx->DATA) == 'E')
                  type = TTV_SIG_Q | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'C') 
                  type = TTV_SIG_CQ | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'N') 
                  type = TTV_SIG_NQ | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'I')
                  type = TTV_SIG_Q ;
               else 
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,0L,ttvfig, &msi,1)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               trig_bracket=0;
               if (chainx->NEXT!=NULL && chainx->NEXT->DATA==NULL)
               {
                 trig_bracket=1;
                 chainx = ttv_getnextchain(chainx) ;
               }
               if(((type & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
                  ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
                {
                 if((ttvfig->INS == NULL) || trig_bracket/*(chainx != NULL)*/)
                 chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                 else
                   {
                    ttvsig = ttv_getsigbyinsname(ttvfig,nomsig,TTV_SIG_Q) ;
                    if((ttvsig == NULL) || 
                       ((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0))
                      chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                    else
                     {
                      ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                      chainref = addchain(NULL,(void*)ttvsig) ;
                     }
                   }
                }
               else 
                {
                 if((type & TTV_SIG_C) == TTV_SIG_C)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_C) ;
                 else if((type & TTV_SIG_N) == TTV_SIG_N)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_N) ;
                 else     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 if(ttvsig == NULL)     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                 chainref = addchain(NULL,(void*)ttvsig) ;
                 ttvsig->TYPE |= type ;
                }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               if(((index1 != (ttvfig->NBELCMDSIG + 1L)) &&
                   ((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)) ||
                  (((index1 - (ttvfig->NBELCMDSIG + 1L)) != ttvfig->NBILCMDSIG)
                  && ((type & TTV_SIG_EXT) !=  TTV_SIG_EXT)))
                 {
                  chainref->NEXT = (chain_list*)ttvfig->ELCMDSIG ;
                  ttvfig->ELCMDSIG = (ttvsig_list **)chainref ;
                  ttvfig->NBELCMDSIG++ ;
                  ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  if((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)
                    {
                     chainref->NEXT = (chain_list*)ttvfig->ELCMDSIG ;
                     ttvfig->ELCMDSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBELCMDSIG++ ;
                     ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                    }
                   else
                    {
                     chainref->NEXT = (chain_list*)ttvfig->ILCMDSIG ;
                     ttvfig->ILCMDSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBILCMDSIG++ ;
                    }
                 }
               if(trig_bracket && chainx != NULL)
                 {
                  chainref = NULL ;
                  ttvsig1=NULL;
                  while(chainx != NULL)
                    {
                     if(chainx->DATA == NULL)
                      {
                       //chainx = chainx->NEXT ;
                       break ;
                      }
                     ttvsig1 = ttv_getsigbyname(ttvfig,(char*)chainx->DATA,
                                                TTV_SIG_Q) ;
                     if(ttvsig1 == NULL) break ;
                     chainref = addchain(chainref,(void*)ttvsig1) ;
                     ttvsig1->USER = addptype(ttvsig1->USER,
                                              TTV_SIG_CMDNEW,ttvsig) ;
                     chainx = chainx->NEXT ;
                    }
                  ttvsig->USER = addptype(ttvsig->USER,TTV_SIG_CMDOLD,
                                          (void*)chainref) ;
                  if(ttvsig1==NULL)     
                   {
                           
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 }
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,0L,ttvfig, &msi,2)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               setpropandgate(ttvsig, &msi);
               chainx = ttv_getnextchain(chainx) ;
               if(chainx != NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'L' : if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
                 return(NULL) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = ttv_decodlatchtype((char*)chainx->DATA) ;
               if(type == 0L)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,0L,ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(((type & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
                  ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
                {
                 if(ttvfig->INS == NULL)
                   chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                 else
                   {
                    ttvsig = ttv_getsigbyinsname(ttvfig,nomsig,TTV_SIG_L) ;
                    if((ttvsig == NULL) || 
                       ((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0))
                      chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                    else
                     {
                      ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                      chainref = addchain(NULL,(void*)ttvsig) ;
                     }
                   }
                }
               else
                {
                 if((type & TTV_SIG_C) == TTV_SIG_C)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_C) ;
                 else if((type & TTV_SIG_N) == TTV_SIG_N)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_N) ;
                 else     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 if(ttvsig == NULL)     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                 chainref = addchain(NULL,(void*)ttvsig) ;
                 ttvsig->TYPE |= type ;
                }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               setpropandgate(ttvsig, &msi);
               if(((index1 != (ttvfig->NBELATCHSIG + 1L)) && 
                   ((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)) || 
                 (((index1 - (ttvfig->NBELATCHSIG + 1L)) != ttvfig->NBILATCHSIG)
                  && ((type & TTV_SIG_EXT) !=  TTV_SIG_EXT)))
                 {
                  chainref->NEXT = (chain_list*)ttvfig->ELATCHSIG ;
                  ttvfig->ELATCHSIG = (ttvsig_list **)chainref ;
                  ttvfig->NBELATCHSIG++ ;
                  ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  if((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)
                    {
                     chainref->NEXT = (chain_list*)ttvfig->ELATCHSIG ;
                     ttvfig->ELATCHSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBELATCHSIG++ ;
                     ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                    }
                   else
                    {
                     chainref->NEXT = (chain_list*)ttvfig->ILATCHSIG ;
                     ttvfig->ILATCHSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBILATCHSIG++ ;
                    }
                 }
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                {
                 chainx = ttv_parscmd(ttvfig,chainx,ttvsig) ;
                 if(chainx == NULL)     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 if(chainx->NEXT != NULL)     
                  {
                   freechain(chain) ;
                   ttv_parserror(filename,linecount) ;
                  }
                }
               freechain(chain) ;
               return(NULL) ;
    case 'R' : if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
                 return(NULL) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(*((char*)chainx->DATA) == 'E')
                  type = TTV_SIG_R | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'I')
                  type = TTV_SIG_R ;
               else if(*((char*)chainx->DATA) == 'C')
                  type = TTV_SIG_CR | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'N')
                  type = TTV_SIG_NR | TTV_SIG_EXT ;
               else
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,0L,ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(((type & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
                  ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
                {
                 if(ttvfig->INS == NULL)
                   chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                 else
                   {
                    ttvsig = ttv_getsigbyinsname(ttvfig,nomsig,TTV_SIG_R) ;
                    if((ttvsig == NULL) || 
                       ((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0))
                      chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                    else
                     {
                      ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                      chainref = addchain(NULL,(void*)ttvsig) ;
                     }
                   }
                }
               else
                {
                 if((type & TTV_SIG_C) == TTV_SIG_C)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_C) ;
                 else if((type & TTV_SIG_N) == TTV_SIG_N)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_N) ;
                 else     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 if(ttvsig == NULL)     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                 chainref = addchain(NULL,(void*)ttvsig) ;
                 ttvsig->TYPE |= type ;
                }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               setpropandgate(ttvsig, &msi);
               if(((index1 != (ttvfig->NBEPRESIG + 1L)) &&
                   ((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)) ||
                  (((index1 - (ttvfig->NBEPRESIG + 1L)) != ttvfig->NBIPRESIG) &&
                   ((type & TTV_SIG_EXT) !=  TTV_SIG_EXT)))
                 {
                  chainref->NEXT = (chain_list*)ttvfig->EPRESIG ;
                  ttvfig->EPRESIG = (ttvsig_list **)chainref ;
                  ttvfig->NBEPRESIG++ ;
                  ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  if((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)
                    {
                     chainref->NEXT = (chain_list*)ttvfig->EPRESIG ;
                     ttvfig->EPRESIG = (ttvsig_list **)chainref ;
                     ttvfig->NBEPRESIG++ ;
                     ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                    }
                   else
                    {
                     chainref->NEXT = (chain_list*)ttvfig->IPRESIG ;
                     ttvfig->IPRESIG = (ttvsig_list **)chainref ;
                     ttvfig->NBIPRESIG++ ;
                    }
                 }
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'B' : if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
                 return(NULL) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(*((char*)chainx->DATA) == 'E')
                  type = TTV_SIG_B | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'I')
                  type = TTV_SIG_B ;
               else if(*((char*)chainx->DATA) == 'C')
                  type = TTV_SIG_C | TTV_SIG_B | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'N')
                  type = TTV_SIG_N | TTV_SIG_B | TTV_SIG_EXT ;
               else
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,TTV_SIG_B,ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(((type & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
                  ((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C))
                {
                 if(ttvfig->INS == NULL)
                   chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                 else
                   {
                    ttvsig = ttv_getsigbyinsname(ttvfig,nomsig,TTV_SIG_B) ;
                    if((ttvsig == NULL) || 
                       ((ttvsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0))
                      chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
                    else
                     {
                      ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                      chainref = addchain(NULL,(void*)ttvsig) ;
                     }
                   }
                }
               else
                {
                 if((type & TTV_SIG_C) == TTV_SIG_C)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_C) ;
                 else if((type & TTV_SIG_N) == TTV_SIG_N)
                   ttvsig = ttv_getsigbyname(ttvfig,nomsig,TTV_SIG_N) ;
                 else     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 if(ttvsig == NULL)     
                   {
                    freechain(chain) ;
                    ttv_parserror(filename,linecount) ;
                   }
                 ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                 chainref = addchain(NULL,(void*)ttvsig) ;
                 ttvsig->TYPE |= type ;
                }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               setpropandgate(ttvsig, &msi);
               if(((index1 != (ttvfig->NBEBREAKSIG + 1L)) &&
                   ((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)) ||
                  (((index1 - (ttvfig->NBEBREAKSIG + 1L)) != ttvfig->NBIBREAKSIG) &&
                   ((type & TTV_SIG_EXT) !=  TTV_SIG_EXT)))
                 {
                  chainref->NEXT = (chain_list*)ttvfig->EBREAKSIG ;
                  ttvfig->EBREAKSIG = (ttvsig_list **)chainref ;
                  ttvfig->NBEBREAKSIG++ ;
                  ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  if((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)
                    {
                     chainref->NEXT = (chain_list*)ttvfig->EBREAKSIG ;
                     ttvfig->EBREAKSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBEBREAKSIG++ ;
                     ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                    }
                   else
                    {
                     chainref->NEXT = (chain_list*)ttvfig->IBREAKSIG ;
                     ttvfig->IBREAKSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBIBREAKSIG++ ;
                    }
                 }
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'I' : if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
                 return(NULL) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(*((char*)chainx->DATA) == 'E')
                  type = TTV_SIG_I | TTV_SIG_EXT ;
               else if(*((char*)chainx->DATA) == 'I')
                  type = TTV_SIG_I ;
               else     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,0L,ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(ttvfig->INS == NULL)
                 chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,type,NULL) ;
               else
                 {
                  ttvsig = ttv_getsigbyinsname(ttvfig,nomsig,TTV_SIG_I) ;
                  if(ttvsig == NULL) 
                   chainref = ttv_addrefsig(ttvfig,nomsig,
                                            net,capa,type,NULL) ;
                  else 
                    {
                     ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                     chainref = addchain(NULL,(void*)ttvsig) ; 
                    }
                 }
               ttvsig = (ttvsig_list *)chainref->DATA ;
               setpropandgate(ttvsig, &msi);
               if(((index1 != (ttvfig->NBEXTSIG + 1L)) &&
                   ((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)) ||
                  (((index1 - (ttvfig->NBEXTSIG + 1L)) != ttvfig->NBINTSIG) &&
                   ((type & TTV_SIG_EXT) !=  TTV_SIG_EXT)))
                 {
                  chainref->NEXT = (chain_list*)ttvfig->EXTSIG ;
                  ttvfig->EXTSIG = (ttvsig_list **)chainref ;
                  ttvfig->NBEXTSIG++ ;
                  ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               else
                 {
                  if((type & TTV_SIG_EXT) ==  TTV_SIG_EXT)
                    {
                     chainref->NEXT = (chain_list*)ttvfig->EXTSIG ;
                     ttvfig->EXTSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBEXTSIG++ ;
                     ttvsig->TYPE &= ~(TTV_SIG_EXT) ;
                    }
                   else
                    {
                     chainref->NEXT = (chain_list*)ttvfig->INTSIG ;
                     ttvfig->INTSIG = (ttvsig_list **)chainref ;
                     ttvfig->NBINTSIG++ ;
                    }
                 }
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'S' : if(((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S) ||
                  (((status & TTV_STS_S) != TTV_STS_S) && 
                   ((status & TTV_STS_SE) != TTV_STS_SE)) ||
                  (((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE) &&
                   ((status & TTV_STS_S) != TTV_STS_S)))
                 return(NULL) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(*((char*)chainx->DATA) == 'E')
                  type = 1L ;
               else if(*((char*)chainx->DATA) == 'I')
                  type = 0L ;
               if(((type == 1L) &&  
                  ((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE)) ||
                  ((type == 0L) && 
                  ((status & TTV_STS_S) != TTV_STS_S)))
                  {
                   freechain(chain) ;
                   return(NULL) ;
                  }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char*)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               nomsig = (char*)chainx->DATA ;
               if((chainx = ttv_parsnc(&capa,&net,chainx,nomsig,0L,ttvfig, &msi,0)) == NULL)
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if(type == 1L) 
                 {
                  if(ttvfig->INS == NULL)
                  chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,TTV_SIG_S,
                                           NULL) ;
                  else
                    {
                     ttvsig = ttv_getsigbyinsname(ttvfig,nomsig,TTV_SIG_S) ;
                     if(ttvsig == NULL)
                       chainref = ttv_addrefsig(ttvfig,nomsig,net,capa,TTV_SIG_S,
                                                NULL) ;
                     else
                      {
                       chainref = addchain(NULL,(void*)ttvsig) ;
                       ttv_setsiglevel(ttvsig,ttvfig->INFO->LEVEL) ;
                      }
                    }
                  ttvsig = (ttvsig_list *)chainref->DATA ;
                  if(index1 != (ttvfig->NBESIG + 1L))
                    {
                     chainref->NEXT = (chain_list*)ttvfig->ESIG ;
                     ttvfig->NBESIG++ ;
                     freechain(chain) ;
                     ttv_parserror(filename,linecount) ;
                    }
                  else
                    {
                     ttvfig->NBESIG++ ;
                     chainref->NEXT = (chain_list*)ttvfig->ESIG ;
                     ttvfig->ESIG = (ttvsig_list **)chainref ;
                    }
                 }
               else
                 {
                  ttvsig = ttv_addsig(ttvfig,nomsig,net,capa,TTV_SIG_S) ;
                  if((index1 - ttvfig->NBESIG) != ttvfig->NBISIG)
                    {
                     freechain(chain) ;
                     ttv_parserror(filename,linecount) ;
                    }
                 }
               setpropandgate(ttvsig, &msi);
               if((chainx = ttv_getnextchain(chainx)) != NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'P' : if(((ttvfig->STATUS & TTV_STS_P) == TTV_STS_P) ||
                  ((status & TTV_STS_P) != TTV_STS_P))
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = TTV_LINE_P ;
               type1 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type2 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index2 = atol((char *)chainx->DATA) ;
               ttvsig1 = ttv_decodsigtype(ttvfig,index1,type1) ; 
               ttvsig2 = ttv_decodsigtype(ttvfig,index2,type2) ; 
               if(((ttvsig1->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) ||
                  ((ttvsig2->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0))
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               chainx = ttv_parsttvline(ttvfig,ttvsig1,ttvsig2,type,chainx,
                                        type2,status) ;
               if((chainx == NULL) || (chainx->NEXT != NULL))
                      
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'J' : if(((ttvfig->STATUS & TTV_STS_J) == TTV_STS_J) ||
                  ((status & TTV_STS_J) != TTV_STS_J))
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = TTV_LINE_J ;
               type1 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type2 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index2 = atol((char *)chainx->DATA) ;
               ttvsig1 = ttv_decodsigtype(ttvfig,index1,type1) ;
               ttvsig2 = ttv_decodsigtype(ttvfig,index2,type2) ;
               if(((ttvsig1->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) ||
                  ((ttvsig2->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0))
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               chainx = ttv_parsttvline(ttvfig,ttvsig1,ttvsig2,type,chainx,
                                        type2,status) ;
               if((chainx == NULL) || (chainx->NEXT != NULL))
                      
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'F' : if(((ttvfig->STATUS & TTV_STS_F) == TTV_STS_F) ||
                  ((status & TTV_STS_F) != TTV_STS_F))
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = TTV_LINE_F ;
               type1 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type2 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index2 = atol((char *)chainx->DATA) ;
               ttvsig1 = ttv_decodsigtype(ttvfig,index1,type1) ;
               ttvsig2 = ttv_decodsigtype(ttvfig,index2,type2) ;
               if(((ttvsig1->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) ||
                  ((ttvsig2->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0))
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               chainx = ttv_parsttvline(ttvfig,ttvsig1,ttvsig2,type,chainx,
                                        type2,status) ;
               if((chainx == NULL) || (chainx->NEXT != NULL))
                      
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'E' : if(((ttvfig->STATUS & TTV_STS_E) == TTV_STS_E) ||
                  ((status & TTV_STS_E) != TTV_STS_E))
                {
                 return(NULL) ;
                }
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = TTV_LINE_E ;
               type1 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type2 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index2 = atol((char *)chainx->DATA) ;
               ttvsig1 = ttv_decodsigtype(ttvfig,index1,type1) ; 
               ttvsig2 = ttv_decodsigtype(ttvfig,index2,type2) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               if(((ttvsig1->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) ||
                  ((ttvsig2->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0))
                {
                 freechain(chain) ;
                 ttv_parserror(filename,linecount) ;
                }
               chainx = ttv_parsttvline(ttvfig,ttvsig1,ttvsig2,type,chainx,
                                        type2,status) ;
               if((chainx == NULL) || (chainx->NEXT != NULL))
                      
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'T' : if(((ttvfig->STATUS & TTV_STS_T) == TTV_STS_T) ||
                  ((status & TTV_STS_T) != TTV_STS_T))
                 return(ttvfig) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = TTV_LINE_T ;
               type1 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type2 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index2 = atol((char *)chainx->DATA) ;
               ttvsig1 = ttv_decodsigtype(ttvfig,index1,type1) ; 
               ttvsig2 = ttv_decodsigtype(ttvfig,index2,type2) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               chainx = ttv_parsttvline(ttvfig,ttvsig1,ttvsig2,type,chainx,
                                        type2,status) ;
               if((chainx == NULL) || (chainx->NEXT != NULL))
                      
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'D' : if(((ttvfig->STATUS & TTV_STS_D) == TTV_STS_D) ||
                  ((status & TTV_STS_D) != TTV_STS_D))
                 return(ttvfig) ;
               buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;  
               if(chainx == NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type = TTV_LINE_D ;
               type1 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index1 = atol((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               type2 = *((char *)chainx->DATA) ;
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               index2 = atol((char *)chainx->DATA) ;
               ttvsig1 = ttv_decodsigtype(ttvfig,index1,type1) ; 
               ttvsig2 = ttv_decodsigtype(ttvfig,index2,type2) ; 
               if((chainx = ttv_getnextchain(chainx)) == NULL)
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               chainx = ttv_parsttvline(ttvfig,ttvsig1,ttvsig2,type,chainx,
                                        type2,status) ;
               if((chainx == NULL) || (chainx->NEXT != NULL))
                      
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               freechain(chain) ;
               return(NULL) ;
    case 'G' : buf++ ;
               chain = chainx = ttv_getlinearg(buf) ;
               if(chainx != NULL)     
                 {
                  freechain(chain) ;
                  ttv_parserror(filename,linecount) ;
                 }
               return(NULL) ;
    default  : ttv_parserror(filename,linecount) ;
   }

   return(NULL);
}

/*****************************************************************************/
/*                        function ttv_builthtabfig()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* status : etat qui definie les signaux a mettre dans la table              */
/*                                                                           */
/* construit les tables de hash pour toute la hierarchie                     */
/*****************************************************************************/
void ttv_builthtabfig(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
  ttv_builthtabttvfig(ttvfig,status) ;
  ttv_builthtabins(ttvfig,status) ;
}

/*****************************************************************************/
/*                        function ttv_builthtabins()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* status : etat qui definie les signaux a mettre dans la table              */
/*                                                                           */
/* construit les tables de hash pour les instances                           */
/*****************************************************************************/
void ttv_builthtabins(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ttvfig_list *ttvfigx ;
 chain_list *chain ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   {
    ttvfigx = (ttvfig_list *)chain->DATA ; 
    ttv_builthtabttvfig(ttvfigx,status) ;
    ttv_builthtabins(ttvfigx,status) ;
   }
}

/*****************************************************************************/
/*                        function ttv_builthtabttvfig()                     */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* status : etat qui definie les signaux a mettre dans la table              */
/*                                                                           */
/* construit les tables de hash pour parser les latch et les signaux         */
/*****************************************************************************/
void ttv_builthtabttvfig(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ttvsig_list *ptsig ;
 ttvsbloc_list *ptsbloc ;
 chain_list *chainsig ;
 chain_list *chainx ;
 ht *htab ;
 long i ;
 long j ;
 long nbfree ;
 long nbend ;
 long lht ;

 if(((status & TTV_STS_L) == TTV_STS_L) && 
    (getptype(ttvfig->USER,TTV_STS_HTAB_L) == NULL))
  {
   if((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C)
   chainsig = ttv_getsigbytype(ttvfig,ttvfig,TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|
                                               TTV_SIG_I|TTV_SIG_B|
                                               TTV_SIG_C|TTV_SIG_N,NULL) ;
   else
   chainsig = ttv_getsigbytype(ttvfig,ttvfig,TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|
                                               TTV_SIG_I|TTV_SIG_B,NULL) ;

   lht = ttvfig->NBELCMDSIG + ttvfig->NBELATCHSIG + ttvfig->NBEPRESIG
         + ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG
         + ttvfig->NBEXTSIG + ttvfig->NBILCMDSIG + ttvfig->NBILATCHSIG 
         + ttvfig->NBIPRESIG + ttvfig->NBINTSIG ;

   if((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C)
      {
       lht += ttvfig->NBCONSIG + ttvfig->NBNCSIG ;
      }

   if(lht != (long)0)
    {
     htab = addht(2*lht) ;
     ttvfig->USER = addptype(ttvfig->USER,TTV_STS_HTAB_L,(void*)htab) ; 
     for(chainx = chainsig ; chainx != NULL ; chainx = chainx->NEXT)
      {
       ptsig = (ttvsig_list *)chainx->DATA ;
       if(ptsig->ROOT == ttvfig)
        addhtitem(htab,(void*)ptsig->NAME,(long)ptsig) ;
      }
    }
   freechain(chainsig) ;
  }

 if(((status & TTV_STS_S) == TTV_STS_S) &&
    (getptype(ttvfig->USER,TTV_STS_HTAB_S) == NULL))
  {
   lht = ttvfig->NBESIG ;

   if((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S)
     lht += ttvfig->NBISIG ;

   if(lht != (long)0)
    {
     htab = addht(2*lht) ;

     ttvfig->USER = addptype(ttvfig->USER,TTV_STS_HTAB_S,(void*)htab) ; 

     for(i = 0 ; i < ttvfig->NBESIG ; i++)
      {
       ptsig = *(ttvfig->ESIG + i) ;
       if(ptsig->ROOT == ttvfig)
        addhtitem(htab,(void*)ptsig->NAME,(long)ptsig) ;
      }

     if((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S)
      {
       if(ttvfig->NBISIG != 0)
        {
         ptsbloc = ttvfig->ISIG ;
         nbfree = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC))
                   % TTV_MAX_SBLOC ;
         nbend = ttvfig->NBISIG + nbfree ;

         for(i = nbfree ; i < nbend ; i++)
          {
           j = i % TTV_MAX_SBLOC ;
           if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
           ptsig = ptsbloc->SIG + j ;
           addhtitem(htab,(void*)ptsig->NAME,(long)ptsig) ;
          }
        }
      }
    }
  }
}

/*****************************************************************************/
/*                        function ttv_freehtabfig()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/*                                                                           */
/* detruit les tables de hash pour toute la figure                           */
/*****************************************************************************/
void ttv_freehtabfig(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ttvfig_list *ttvfigx ;
 chain_list *chain ;

 ttv_freehtabttvfig(ttvfig,status) ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   {
    ttvfigx = (ttvfig_list *)chain->DATA ; 
    ttv_freehtabfig(ttvfigx,status) ;
   }
}

/*****************************************************************************/
/*                        function ttv_freehtabttvfig()                      */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/*                                                                           */
/* detruit les tables de hash pour parser les latch et les signaux           */
/*****************************************************************************/
void ttv_freehtabttvfig(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ptype_list *ptype ;

 if(((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL) &&
    ((status & TTV_STS_L) == TTV_STS_L))
  {
   delht((ht*)ptype->DATA) ;
   ttvfig->USER = delptype(ttvfig->USER,TTV_STS_HTAB_L) ;
  }

 if(((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL) &&
    ((status & TTV_STS_S) == TTV_STS_S))
  {
   delht((ht*)ptype->DATA) ;
   ttvfig->USER = delptype(ttvfig->USER,TTV_STS_HTAB_S) ;
  }
}

/*****************************************************************************/
/*                        function ttv_calcstatus()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* status : etat de la ttvfig demande                                        */
/* prevchar : dernier element traite du fichier                              */
/*                                                                           */
/* calcul l'etat courant de la figure                                        */
/*****************************************************************************/
void ttv_calcstatus(ttvfig,status,prevchar,curchar)
ttvfig_list *ttvfig ;
long status ;
char prevchar ;
char curchar ;
{
 switch(prevchar)
  {
   case 'X' : if((ttvfig->STATUS & TTV_STS_HEADER)
                  != TTV_STS_HEADER)
              ttv_checkallinstool(ttvfig) ;
              ttvfig->STATUS |= TTV_STS_HEADER ;
              break ;
   case 'C' : if(((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
                 ((status & TTV_STS_C) == TTV_STS_C))
               {
                ttvfig->CONSIG = ttv_allocreflist((chain_list*)ttvfig->CONSIG,
                                                  ttvfig->NBCONSIG) ;
                TTV_LAST_SIG = TTV_SIG_C ;
                if(curchar != 'N')
                 {
                  ttvfig->STATUS |= TTV_STS_C ;
                  if((getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
                    {
                     ttv_freehtabttvfig(ttvfig,TTV_STS_L) ;
                     ttv_builthtabttvfig(ttvfig,TTV_STS_L) ;
                    }
                 }
               }
              break ;
   case 'N' : if(((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
                 ((status & TTV_STS_C) == TTV_STS_C))
               {
                ttvfig->NCSIG = ttv_allocreflist((chain_list*)ttvfig->NCSIG,
                                                 ttvfig->NBNCSIG) ;
                TTV_LAST_SIG = TTV_SIG_N ;
                ttvfig->STATUS |= TTV_STS_C ;
                if((getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
                  {
                   ttv_freehtabttvfig(ttvfig,TTV_STS_L) ;
                   ttv_builthtabttvfig(ttvfig,TTV_STS_L) ;
                  }
               }
              break ;
   case 'Q' : if(((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L) &&
                 ((status & TTV_STS_L) == TTV_STS_L))
               {
                ttvfig->ELCMDSIG = ttv_allocreflist((chain_list*)ttvfig->ELCMDSIG,
                                                    ttvfig->NBELCMDSIG) ;
                ttvfig->ILCMDSIG = ttv_allocreflist((chain_list*)ttvfig->ILCMDSIG,
                                                    ttvfig->NBILCMDSIG) ;
                TTV_LAST_SIG = TTV_SIG_Q ;
               }
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              break ;
   case 'L' : if(((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L) &&
                 ((status & TTV_STS_L) == TTV_STS_L))
               {
                ttvfig->ELATCHSIG = ttv_allocreflist((chain_list*)ttvfig->ELATCHSIG,
                                                     ttvfig->NBELATCHSIG) ;
                ttvfig->ILATCHSIG = ttv_allocreflist((chain_list*)ttvfig->ILATCHSIG,
                                                     ttvfig->NBILATCHSIG) ;
                TTV_LAST_SIG = TTV_SIG_L ;
               }
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              break ;
   case 'R' : if(((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L) &&
                 ((status & TTV_STS_L) == TTV_STS_L))
               {
                ttvfig->EPRESIG = ttv_allocreflist((chain_list*)ttvfig->EPRESIG,
                                                   ttvfig->NBEPRESIG) ;
                ttvfig->IPRESIG = ttv_allocreflist((chain_list*)ttvfig->IPRESIG,
                                                   ttvfig->NBIPRESIG) ;
                TTV_LAST_SIG = TTV_SIG_R ;
               }
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              break ;
   case 'B' : if(((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L) &&
                 ((status & TTV_STS_L) == TTV_STS_L))
               {
                ttvfig->EBREAKSIG = ttv_allocreflist((chain_list*)ttvfig->EBREAKSIG,
                                                   ttvfig->NBEBREAKSIG) ;
                ttvfig->IBREAKSIG = ttv_allocreflist((chain_list*)ttvfig->IBREAKSIG,
                                                   ttvfig->NBIBREAKSIG) ;
                TTV_LAST_SIG = TTV_SIG_B ;
               }
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              break ;
   case 'I' : if(((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L) &&
                 ((status & TTV_STS_L) == TTV_STS_L))
               {
                ttvfig->EXTSIG = ttv_allocreflist((chain_list*)ttvfig->EXTSIG,
                                                  ttvfig->NBEXTSIG) ;
                ttvfig->INTSIG = ttv_allocreflist((chain_list*)ttvfig->INTSIG,
                                                  ttvfig->NBINTSIG) ;
                TTV_LAST_SIG = TTV_SIG_I ;
               }
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              break ;
   case 'S' : if(((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE) &&
                 (((status & TTV_STS_S) == TTV_STS_S) ||
                 ((status & TTV_STS_SE) == TTV_STS_SE)))
               {
                ttvfig->ESIG = ttv_allocreflist(
                        (chain_list*)ttvfig->ESIG,ttvfig->NBESIG) ;
                ttvfig->STATUS |= TTV_STS_SE ;
                TTV_LAST_SIG = TTV_SIG_S ;
               }
              if(((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S) &&
                 ((status & TTV_STS_S) == TTV_STS_S) &&
                 ((getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL))
                  {
                   ttvfig->STATUS |= TTV_STS_S ;
                   ttv_freehtabttvfig(ttvfig,TTV_STS_S) ;
                   ttv_builthtabttvfig(ttvfig,TTV_STS_S) ;
                  }
              else if((status & TTV_STS_S) == TTV_STS_S)
                ttvfig->STATUS |= TTV_STS_S ;

              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & (TTV_STS_DTX&~(TTV_STS_E|
                                                         TTV_STS_F|
                                                         TTV_STS_D|
                                                         TTV_STS_DUAL_E|
                                                         TTV_STS_DUAL_F|
                                                         TTV_STS_DUAL_D))) ;
              break ;
   case 'E' : if((status & TTV_STS_E) == TTV_STS_E)
                ttvfig->STATUS |= TTV_STS_E ;
              if((status & TTV_STS_DUAL_E) != 0)
                ttv_allocdualline(ttvfig,TTV_STS_DUAL_E) ;
              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & (TTV_STS_DTX&~(TTV_STS_D|
                                                         TTV_STS_F|
                                                         TTV_STS_DUAL_D|
                                                         TTV_STS_DUAL_F))) ;
              break ;
   case 'F' : if((status & TTV_STS_F) == TTV_STS_F)
                ttvfig->STATUS |= TTV_STS_F ;
              if((status & TTV_STS_DUAL_F) != 0)
                ttv_allocdualline(ttvfig,TTV_STS_DUAL_F) ;
              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & (TTV_STS_DTX&~(TTV_STS_D|
                                                         TTV_STS_DUAL_D))) ;
              break ;
   case 'D' : if((status & TTV_STS_D) == TTV_STS_D)
                ttvfig->STATUS |= TTV_STS_D ;
              if((status & TTV_STS_DUAL_D) != 0)
                ttv_allocdualline(ttvfig,TTV_STS_DUAL_D) ;
              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & TTV_STS_DTX) ;
              break ;
   case 'J' : if((status & TTV_STS_J) == TTV_STS_J)
                ttvfig->STATUS |= TTV_STS_J ;
              if((status & TTV_STS_DUAL_J) != 0)
                ttv_allocdualline(ttvfig,TTV_STS_DUAL_J) ;
              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & (TTV_STS_TTX&~(TTV_STS_T|
                                                         TTV_STS_P|
                                                         TTV_STS_DUAL_T|
                                                         TTV_STS_DUAL_P))) ;
              break ;
   case 'P' : if((status & TTV_STS_P) == TTV_STS_P)
                ttvfig->STATUS |= TTV_STS_P ;
              if((status & TTV_STS_DUAL_P) != 0)
                ttv_allocdualline(ttvfig,TTV_STS_DUAL_P) ;
              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & (TTV_STS_TTX&~(TTV_STS_T|
                                                         TTV_STS_DUAL_T))) ;
              break ;
   case 'T' : if((status & TTV_STS_T) == TTV_STS_T)
                ttvfig->STATUS |= TTV_STS_T ;
              if((status & TTV_STS_DUAL_T) != 0)
                ttv_allocdualline(ttvfig,TTV_STS_DUAL_T) ;
              ttvfig->STATUS |= TTV_STS_L ;
              if((status & TTV_STS_C) == TTV_STS_C)
                ttvfig->STATUS |= TTV_STS_C ;
              ttvfig->STATUS |= (status & TTV_STS_TTX) ;
           break ;
  }
}

/*****************************************************************************/
/*                        function ttv_parsttvfig()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante que l'on parse                                   */
/* status : etat de la ttvfig demande                                        */
/* type : type du fichier ttx ou dtx                                         */
/*                                                                           */
/* parse la figure ttv en fonction de l'etat que l'on desire                 */
/* la fonction est reccursive si les fils ne sont pas presents ils sont      */
/* parser avant jusqu'a la hierarchie la plus basse                          */ 
/* renvoie la ttvfig dans l'etat demande                                     */
/*****************************************************************************/
ttvfig_list *ttv_parsttvfig(ttvfig,status,type)
ttvfig_list *ttvfig ;
long status ;
long type ;
{
 ttvfig_list *ttvins ;
 FILE *ttvfile = NULL ;
 long linecount = 0L ;
 long l ;
 long typehtab ;
 char htab = 'N' ;
 char prevchar = '\0' ;
 char filecomp ;
 static char filename[256] ;

 TTV_LAST_SIG = (long)0 ;
 status &= ~(TTV_STS_DENOTINPT) ;

 if(ttvfig == NULL) return(NULL) ;

 TTV_OLD_NEW ++ ;

 if(TTV_OLD_NEW < TTV_OLD_MAX) 
  {
   ttvfig->OLD = TTV_OLD_NEW ;
  }
 else
  {
   for(ttvins = TTV_LIST_TTVFIG ; ttvins != NULL ; ttvins = ttvins->NEXT)
     if(ttvins->OLD < TTV_OLD_MIN) ttvfig->OLD = (long)0 ;
     else ttvins->OLD -= TTV_OLD_MIN ;
   TTV_OLD_NEW -= TTV_OLD_MIN ;
   ttvfig->OLD = TTV_OLD_NEW ;
  }

 if((ttvfig->STATUS & status) == status)
    return(ttvfig) ;

 if(type == (long)0)
  {
   if((ttvfile = ttv_openfile(ttvfig,TTV_FILE_TTX,READ_TEXT)) != NULL)
    {
     if(fclose(ttvfile) != 0)
      {
       sprintf(filename,"%s.ttx",ttvfig->INFO->FIGNAME) ;
       ttv_error(21,filename,TTV_WARNING) ;
      }
     type = TTV_FILE_TTX ;
    }
   else if((ttvfile = ttv_openfile(ttvfig,TTV_FILE_DTX,READ_TEXT)) != NULL)
    {
     if(fclose(ttvfile) != 0)
      {
       sprintf(filename,"%s.dtx",ttvfig->INFO->FIGNAME) ;
       ttv_error(21,filename,TTV_WARNING) ;
      }
     type = TTV_FILE_DTX ;
    }
  }

 if((status & TTV_STS_DUAL) != 0)
  {
   if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
     status &= ~(TTV_STS_DUAL_D|TTV_STS_DUAL_E|TTV_STS_DUAL_F) ;
   else
     status &= ~(TTV_STS_DUAL_T|TTV_STS_DUAL_J|TTV_STS_DUAL_P) ;
   ttv_allocdualline(ttvfig,status) ;
   if((ttvfig->STATUS & status) == status) {
     mbk_comcheck( 0, 0, ttv_signtimingfigure(ttvfig) );
     return(ttvfig) ;
   }
  }

 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   if((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L)
    {
     typehtab = TTV_STS_L ;
     htab = 'Y' ;
    }
   else
    {
     typehtab = (long)0 ;
     htab = 'N' ;
    }

   sprintf(filename,"%s.ttx",ttvfig->INFO->FIGNAME) ;
   status &= ~(TTV_STS_D | TTV_STS_E | TTV_STS_S) ;
  }
 else if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   if(((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L) &&
      ((ttvfig->STATUS & TTV_STS_SE) != TTV_STS_SE))
    {
     typehtab = TTV_STS_S ;
     htab = 'Y' ;
    }
   else if((ttvfig->STATUS & TTV_STS_L) != TTV_STS_L)
    {
     typehtab = TTV_STS_S | TTV_STS_L ;
     htab = 'Y' ;
    }
   else
    {
     typehtab = (long)0 ;
     htab = 'N' ;
    }

   if(((status & TTV_STS_DENOTINPT) == TTV_STS_DENOTINPT) &&
     ((ttvfig->STATUS & TTV_STS_DENOTINPT) != TTV_STS_DENOTINPT) &&
     ((status & (TTV_STS_D|TTV_STS_E|TTV_STS_F)) != 0) &&
     ((ttvfig->STATUS & (TTV_STS_D|TTV_STS_E|TTV_STS_F)) != 0))
     {
      ttv_freettvfigmemory(ttvfig,TTV_STS_D|TTV_STS_E|TTV_STS_F) ;
     }
   sprintf(filename,"%s.dtx",ttvfig->INFO->FIGNAME) ;
   status &= ~(TTV_STS_T | TTV_STS_P) ;
  } 

 if((ttvfig->INFO->MODEL != NULL) ||
    (ttvfig->INFO->INSTANCES != NULL))
   {
    if(ttv_dupttvfig(ttvfig,status,type,typehtab,htab) != 0)
     {
      if(htab == 'Y') 
       ttv_freehtabfig(ttvfig,TTV_STS_L|TTV_STS_S) ;
      mbk_comcheck( 0, 0, ttv_signtimingfigure(ttvfig) );
      return(ttvfig) ;
     }
   }
 
 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   ttvfile = ttv_openfile(ttvfig,TTV_FILE_TTX,READ_TEXT) ;
  } 
 else if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   ttvfile = ttv_openfile(ttvfig,TTV_FILE_DTX,READ_TEXT) ;
  } 

 if(ttvfile ==  NULL) ttv_error(11,filename,TTV_ERROR) ;

 if(MBKFOPEN_FILTER == YES)
  {
   filecomp = TTV_FILE_COMP ;
  }
 else
  {
   filecomp = TTV_FILE_NOTCOMP ;
  }

 if(bufferline == NULL)
   {
    bufferline = (char *)mbkalloc(1024) ;
    buffersize = 1024 ;
   }

 for(;;)
  {
   linecount = ttv_fgetsline(ttvfile,filename,linecount) ;
 
   switch(*bufferline)
    {
     case '#' : break ;
     case 'X' : prevchar = *bufferline ;
                ttvins = ttv_parsline(ttvfig,bufferline,linecount,status,
                                      filename) ;
                if(ttvins == NULL) break ;
                if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                   ttv_parsttvfig(ttvins,TTV_STS_L,type) ;
                else if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
                   ttv_parsttvfig(ttvins,(TTV_STS_L | TTV_STS_SE),type) ;
                if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                  {
                   sprintf(filename,"%s.ttx",ttvfig->INFO->FIGNAME) ;
                  }
                 else if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
                  {
                   sprintf(filename,"%s.dtx",ttvfig->INFO->FIGNAME) ;
                  }
                ttv_checkfigins(ttvfig,ttvins) ;
                break ;
     case 'G' : ttv_parsline(ttvfig,bufferline,linecount,status,filename) ;
                ttv_calcstatus(ttvfig,status,prevchar,*bufferline) ;
                if(fclose(ttvfile) != 0 )
                  ttv_error(21,filename,TTV_WARNING) ;
                ttvfig->STATUS |= TTV_STS_HEADER ;
                if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                    ttvfig->STATUS |= (TTV_STS_TTX & status) ;
                else
                    ttvfig->STATUS |= (TTV_STS_DTX & status) ;
                if(htab == 'Y') ttv_freehtabfig(ttvfig,TTV_STS_L|TTV_STS_S) ; 
                mbk_comcheck( 0, 0, ttv_signtimingfigure(ttvfig) );
                return(ttvfig) ;
     default  : if(prevchar != *bufferline) 
                  {
                   ttv_calcstatus(ttvfig,status,prevchar,*bufferline) ;
                   if(filecomp == TTV_FILE_NOTCOMP)
                    {
                     ttv_addfilepos(ttvfig,ttvfile,bufferline) ;
                     l = ttv_getfilepos(ttvfig,status,ttvfile,
                                        *bufferline,type) ;
                    }
                   else
                    {
                     l = (long) 0 ;
                    }
                   if(l != (long)0)
                      linecount = l ;
                   if((prevchar == 'X') && (htab == 'Y'))
                     ttv_builthtabins(ttvfig,typehtab) ;
                   if(l == (long)0)
                     ttvins = ttv_parsline(ttvfig,bufferline,linecount,status,
                                           filename) ;
                   else 
                     ttvins = NULL ;
                  }
                 else
                  {
                   ttvins = ttv_parsline(ttvfig,bufferline,linecount,status,
                                         filename) ;
                  }
                if((ttvins != NULL) || ((ttvfig->STATUS & status) == status))
                  {
                   if(fclose(ttvfile) != 0)
                     ttv_error(21,filename,TTV_WARNING) ;
                   ttvfig->STATUS |= TTV_STS_HEADER ;
                   if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
                      ttvfig->STATUS |= (TTV_STS_TTX & status) ;
                   else
                      ttvfig->STATUS |= (TTV_STS_DTX & status) ;
                   if(htab == 'Y') ttv_freehtabfig(ttvfig,TTV_STS_L|TTV_STS_S) ; 
                   mbk_comcheck( 0, 0, ttv_signtimingfigure(ttvfig) );
                   return(ttvins) ;
                  }
                prevchar = *bufferline ;
                break ;
    }
  }
}

