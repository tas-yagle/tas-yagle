/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_alloc.c                                                 */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* allocation des structures de la ttvfig                                   */
/****************************************************************************/

#include "ttv.h"

ttvfig_list *TTV_LIST_TTVFIG = NULL ;
chain_list *TTV_HEAD_TTVFIG = NULL ;
ptype_list *TTV_FREE_MEMTTVFIG = NULL ;
ht *TTV_FREE_MEMTTVFIGHT = NULL ;
ttvsbloc_list *TTV_FREE_SBLOC = NULL ;
ttvlbloc_list *TTV_FREE_LBLOC = NULL ;
ttvcritic_list *TTV_FREE_CRITIC = NULL ;
ttvfind_list *TTV_FREE_FIND = NULL ;
ttvpath_list *TTV_FREE_PATH = NULL ;
long TTV_NUMB_SIG = 0 ;
long TTV_MAX_SIG = TTV_ALLOC_MAX ;
long TTV_NUMB_LINE = 0 ;
long TTV_MAX_LINE = TTV_ALLOC_MAX ;
static HeapAlloc *TTV_FIND_STB_HEAP=NULL;
static HeapAlloc *TTV_PATH_STB_HEAP=NULL;

/*****************************************************************************/
/*                        function ttv_allocttvfig()                         */
/* parametres :                                                              */
/* iname : nom d'instance de la figure ttvfig                                */
/* fname : nom de figure de la ttvfig                                        */
/* root : pointeur sur la ttvfig pere si elle existe                         */
/*                                                                           */
/* fonction d'allocation d'une ttvfig et d'initialisation des champs         */
/*****************************************************************************/
ttvfig_list *ttv_allocttvfig(iname,fname,root)
char *iname ;
char *fname ;
ttvfig_list *root ;
{
 ttvfig_list *ttvfig ;
 ttvfig_list *model = ttv_gethtmodel(fname) ;

 ttvfig = (ttvfig_list *)mbkalloc(sizeof(ttvfig_list)) ;
 ttvfig->ROOT         = root ;
 ttvfig->OLD          = TTV_OLD_NEW ;
 if(model == NULL)
  {
   ttv_allocinfottvfig(ttvfig) ;
   ttvfig->INFO->FIGNAME= namealloc(fname) ;
   ttvfig->STATUS       = TTV_STS_MODEL ;
   ttv_addhtmodel(ttvfig) ;
  }
 else
  {
   ttvfig->INFO = model->INFO ;
   ttvfig->INFO->INSTANCES = addchain(ttvfig->INFO->INSTANCES,ttvfig) ;
   ttvfig->STATUS       = (long)0 ;
  }
 ttvfig->INSNAME      = ttv_checkfigname(namealloc(iname)) ;
 if(root != NULL)
  {
   root->INS = addchain(root->INS,ttvfig) ;
  }
 else 
  {
   ttvfig->STATUS       |= TTV_STS_HEAD ;
   TTV_HEAD_TTVFIG = addchain(TTV_HEAD_TTVFIG,ttvfig) ;
  }
 ttvfig->CONSIG       = NULL ;
 ttvfig->NBCONSIG     = (long)0 ;
 ttvfig->NCSIG        = NULL ;
 ttvfig->NBNCSIG      = (long)0 ;
 ttvfig->ELCMDSIG     = NULL ;
 ttvfig->NBELCMDSIG   = (long)0 ;
 ttvfig->ILCMDSIG     = NULL ;
 ttvfig->NBILCMDSIG   = (long)0 ;
 ttvfig->ELATCHSIG    = NULL ;
 ttvfig->NBELATCHSIG  = (long)0 ;
 ttvfig->ILATCHSIG    = NULL ;
 ttvfig->NBILATCHSIG  = (long)0 ;
 ttvfig->EBREAKSIG    = NULL ;
 ttvfig->NBEBREAKSIG  = (long)0 ;
 ttvfig->IBREAKSIG    = NULL ;
 ttvfig->NBIBREAKSIG  = (long)0 ;
 ttvfig->EPRESIG      = NULL ;
 ttvfig->NBEPRESIG    = (long)0 ;
 ttvfig->IPRESIG      = NULL ;
 ttvfig->NBIPRESIG    = (long)0 ;
 ttvfig->EXTSIG       = NULL ;
 ttvfig->NBEXTSIG     = (long)0 ;
 ttvfig->INTSIG       = NULL ;
 ttvfig->NBINTSIG     = (long)0 ;
 ttvfig->ESIG         = NULL ;
 ttvfig->NBESIG       = (long)0 ;
 ttvfig->ISIG         = NULL ;
 ttvfig->NBISIG       = (long)0 ;
 ttvfig->INS          = NULL ;
 ttvfig->PBLOC        = NULL ;
 ttvfig->NBPBLOC      = (long)0 ;
 ttvfig->JBLOC        = NULL ;
 ttvfig->NBJBLOC      = (long)0 ;
 ttvfig->TBLOC        = NULL ;
 ttvfig->NBTBLOC      = (long)0 ;
 ttvfig->FBLOC        = NULL ;
 ttvfig->NBFBLOC      = (long)0 ;
 ttvfig->EBLOC        = NULL ;
 ttvfig->NBEBLOC      = (long)0 ;
 ttvfig->DBLOC        = NULL ;
 ttvfig->NBDBLOC      = (long)0 ;
 ttvfig->DELAY        = NULL ;
 ttvfig->USER         = NULL ;
 ttvfig->SIGN         = 0 ;
 ttvfig->NEXT         = TTV_LIST_TTVFIG ;
 TTV_LIST_TTVFIG      = ttvfig ;
 
 return ttvfig ;
}

/*****************************************************************************/
/*                        function ttv_allocinfottvfig()                     */
/* parametres :                                                              */
/* ttvfig : information sur une ttvfig                                       */
/*                                                                           */
/* ajoute l'info sur une ttvfig                                              */
/*****************************************************************************/
void ttv_allocinfottvfig(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvinfo_list *info ;

 info = (ttvinfo_list *)mbkalloc(sizeof(ttvinfo_list)) ;

 ttvfig->INFO = info ;
 ttvfig->INFO->TOOLNAME      = NULL ;
 ttvfig->INFO->TOOLVERSION   = NULL ;
 ttvfig->INFO->TECHNONAME    = NULL ;
 ttvfig->INFO->TECHNOVERSION = NULL ;
 ttvfig->INFO->TTVYEAR       = 0 ;
 ttvfig->INFO->TTVMONTH      = 0 ;
 ttvfig->INFO->TTVDAY        = 0 ;
 ttvfig->INFO->TTVHOUR       = 0 ;
 ttvfig->INFO->TTVMIN        = 0 ;
 ttvfig->INFO->TTVSEC        = 0 ;
 ttvfig->INFO->SLOPE         = 0 ;
 ttvfig->INFO->CAPAOUT       = 0 ;
 ttvfig->INFO->STHHIGH       = -1.0 ;
 ttvfig->INFO->STHLOW        = -1.0 ;
 ttvfig->INFO->DTH           = -1.0 ;
 ttvfig->INFO->TEMP          = -1000.0 ;
 ttvfig->INFO->TNOM          = -1000.0 ;
 ttvfig->INFO->VDD           = -1.0 ;
 ttvfig->INFO->LEVEL         = (long)0 ;
 ttvfig->INFO->INSTANCES     = NULL ;
 ttvfig->INFO->MODEL         = ttvfig ;
 ttvfig->INFO->USER          = NULL ;
 ttvfig->INFO->FILENAME      = NULL ;
 ttvfig->INFO->DTB_VERSION     = 2;
}

/*****************************************************************************/
/*                        function ttv_lockttvfig()                          */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig a locker                                  */
/*                                                                           */
/* lock une ttvfig                                                           */
/*****************************************************************************/
void ttv_lockttvfig(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig->STATUS |= TTV_STS_LOCK ;
}

/*****************************************************************************/
/*                        function ttv_unlockttvfig()                        */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig a delocker                                */
/*                                                                           */
/* lock une ttvfig                                                           */
/*****************************************************************************/
void ttv_unlockttvfig(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig->STATUS &= ~(TTV_STS_LOCK) ;
}

/*****************************************************************************/
/*                        function ttv_freettvfig()                          */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig a supprimer                               */
/*                                                                           */
/* fonction de liberation de la ttvfig. les references et la liste des liens */
/* sont supprimees une ttvfig ne peut-etre supprimer que si elle est sommet  */
/* renvoie :                                                                 */
/* 1 si la ttvfig est supprimer 0 sinon                                      */
/*****************************************************************************/
int ttv_freettvfig(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig_list *ptaux ;
 ttvfig_list *ptsav ;
 ptype_list *ptype ;
 chain_list *chain ;

 if(((ttvfig->STATUS & (TTV_STS_HEAD|TTV_STS_LOCK)) != TTV_STS_HEAD) || (ttvfig->ROOT != NULL))
   return 0 ;

 if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
    {
     delht((ht*)ptype->DATA) ;
     ttvfig->USER = delptype(ttvfig->USER,TTV_STS_HTAB_L) ;
    }

 if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL)
    {
     delht((ht*)ptype->DATA) ;
     ttvfig->USER = delptype(ttvfig->USER,TTV_STS_HTAB_S) ;
    }

 ttv_freefalsepath(ttvfig) ;

 if((ttvfig->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
   {
    if(ttvfig->INFO->INSTANCES != NULL)
      {
       chain = ttvfig->INFO->INSTANCES ;
       ptaux = (ttvfig_list *)chain->DATA ;
       ptaux->STATUS |= TTV_STS_MODEL ;
       ttvfig->STATUS &= ~(TTV_STS_MODEL) ;
       ttv_addhtmodel(ptaux) ;
       ptaux->INFO->MODEL = ptaux ;
       ptaux->INFO->INSTANCES = ptaux->INFO->INSTANCES->NEXT ;
       chain->NEXT = NULL ;
       freechain(chain) ;
      }
   }
 else
   {
    ttvfig->INFO->INSTANCES = delchaindata(ttvfig->INFO->INSTANCES,
                                           (void *)ttvfig) ;
   }

 ptaux = TTV_LIST_TTVFIG ;
 while((ptaux != NULL) && (ptaux != ttvfig))
  {
   ptsav = ptaux ;
   ptaux = ptaux->NEXT ;
  }


 if(ptaux == NULL) return 0;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   {
    ptaux = (ttvfig_list *)chain->DATA ;
    ptaux->STATUS |= TTV_STS_HEAD ;
    TTV_HEAD_TTVFIG = addchain(TTV_HEAD_TTVFIG,ptaux) ;
    ptaux->ROOT = NULL ;
   }

 ptaux = ptsav ;

 if(ttvfig == TTV_LIST_TTVFIG)
   TTV_LIST_TTVFIG = ttvfig->NEXT ;
 else
   ptaux->NEXT = ttvfig->NEXT ;
 
   ttv_freettvfigmemory(ttvfig,TTV_STS_D|TTV_STS_E|TTV_STS_F|TTV_STS_T|
                               TTV_STS_J|TTV_STS_P|TTV_STS_S) ;
   ttv_freereflist(ttvfig,ttvfig->CONSIG,ttvfig->NBCONSIG) ;
   ttv_freereflist(ttvfig,ttvfig->NCSIG,ttvfig->NBNCSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ELCMDSIG,ttvfig->NBELCMDSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ILCMDSIG,ttvfig->NBILCMDSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ELATCHSIG,ttvfig->NBELATCHSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ILATCHSIG,ttvfig->NBILATCHSIG) ;
   ttv_freereflist(ttvfig,ttvfig->EBREAKSIG,ttvfig->NBEBREAKSIG) ;
   ttv_freereflist(ttvfig,ttvfig->IBREAKSIG,ttvfig->NBIBREAKSIG) ;
   ttv_freereflist(ttvfig,ttvfig->EPRESIG,ttvfig->NBEPRESIG) ;
   ttv_freereflist(ttvfig,ttvfig->IPRESIG,ttvfig->NBIPRESIG) ;
   ttv_freereflist(ttvfig,ttvfig->EXTSIG,ttvfig->NBEXTSIG) ;
   ttv_freereflist(ttvfig,ttvfig->INTSIG,ttvfig->NBINTSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ESIG,ttvfig->NBESIG) ;
   freechain(ttvfig->INS) ;
   freeptype(ttvfig->USER) ;
   if((ttvfig->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
     {
      ttv_delrcxlofig(ttvfig) ;
      ttv_delhtmodel(ttvfig) ;
      mbkfree(ttvfig->INFO) ;
     }
   if((ttvfig->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD)
       TTV_HEAD_TTVFIG = delchaindata(TTV_HEAD_TTVFIG,ttvfig) ;
   ttv_freettvfigdelay(ttvfig) ;
   ttv_delinfreelist(NULL,ttvfig) ;
   mbkfree((void *)ttvfig) ;

   return 1 ;
}

/*****************************************************************************/
/*                        function ttv_freettvfiglist()                      */
/* parametres :                                                              */
/* chainfig :  liste a supprimer                                             */
/*                                                                           */
/* fonction de liberation de ttvfig. les references et la liste des liens    */
/* sont supprimees une ttvfig ne peut-etre supprimer que si elle est sommet  */
/* et que si c'est une figure de plus bas niveau                             */
/* renvoie :                                                                 */
/* 1 si la ttvfig est supprimer 0 sinon                                      */
/*****************************************************************************/
int ttv_freettvfiglist(chainfig)
chain_list *chainfig ;
{
 ttvfig_list *ttvfig ;
 ttvfig_list *ptaux ;
 ttvfig_list *ptnext ;
 ttvfig_list *ptsav ;
 ptype_list *ptype ;
 chain_list *chainmodel = NULL ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainnext ;
 chain_list *chainprev ;

 for(ttvfig = TTV_LIST_TTVFIG ; ttvfig != NULL ; ttvfig = ttvfig->NEXT)
   ttvfig->STATUS &= ~(TTV_STS_MARQUE) ;

 chainx = NULL ;

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
  {
   ttvfig = (ttvfig_list *)chain->DATA ;

   if(((ttvfig->STATUS & (TTV_STS_HEAD|TTV_STS_LOCK)) == TTV_STS_HEAD) && 
      (ttvfig->ROOT == NULL) &&  (ttvfig->INS == NULL))
    {
     ttvfig->STATUS |= TTV_STS_MARQUE ;

     if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
       {
        delht((ht*)ptype->DATA) ;
        ttvfig->USER = delptype(ttvfig->USER,TTV_STS_HTAB_L) ;
       }

     if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL)
       {
        delht((ht*)ptype->DATA) ;
        ttvfig->USER = delptype(ttvfig->USER,TTV_STS_HTAB_S) ;
       }

     ttv_freefalsepath(ttvfig) ;

     if((ttvfig->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
      {
       chainmodel = addchain(chainmodel,(void *)ttvfig) ;
      }
     else
      {
       chainx = addchain(chainx,ttvfig) ;
      }
    }
  }

 chainfig = chainx ;

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
  {
   ttvfig = (ttvfig_list *)chain->DATA ;
   ttvfig = ttvfig->INFO->MODEL ;
   if((ttvfig->STATUS & TTV_STS_MARQUE) != TTV_STS_MARQUE)
    chainmodel = addchain(chainmodel,ttvfig) ;
  }

 for(chainx = chainmodel ; chainx != NULL ; chainx = chainx->NEXT)
  {
   ttvfig = (ttvfig_list *)chainx->DATA ;
   for(chain = ttvfig->INFO->INSTANCES ; chain != NULL ; chain = chainnext)
    {
     chainnext = chain->NEXT ;
     ptaux = (ttvfig_list *)chain->DATA ;
     if((ptaux->STATUS & TTV_STS_MARQUE) == TTV_STS_MARQUE)
      {
       if(chain == ttvfig->INFO->INSTANCES)
        {
         ttvfig->INFO->INSTANCES = ttvfig->INFO->INSTANCES->NEXT ;
        }
       else
        {
         chainprev->NEXT = chain->NEXT ;
        }
       chain->NEXT = NULL ;
       freechain(chain) ;
      }
     else
      chainprev = chain ;
    }
  }

 freechain(chainfig) ;

 for(chain = chainmodel ; chain != NULL ; chain = chain->NEXT)
   {
    ttvfig = (ttvfig_list *)chain->DATA ;
    if((ttvfig->STATUS & TTV_STS_MARQUE) != TTV_STS_MARQUE)
     continue ;
    if(ttvfig->INFO->INSTANCES != NULL)
      {
       chain = ttvfig->INFO->INSTANCES ;
       ptaux = (ttvfig_list *)chain->DATA ;
       ptaux->STATUS |= TTV_STS_MODEL ;
       ttvfig->STATUS &= ~(TTV_STS_MODEL) ;
       ttv_addhtmodel(ptaux) ;
       ptaux->INFO->MODEL = ptaux ;
       ttvfig->INFO->INSTANCES = ttvfig->INFO->INSTANCES->NEXT ;
       chain->NEXT = NULL ;
       freechain(chain) ;
      }
   }

 freechain(chainmodel) ;

 for(ttvfig = TTV_LIST_TTVFIG ; ttvfig != NULL ; ttvfig = ptnext)
  {
   ptnext = ttvfig->NEXT ;

   if((ttvfig->STATUS & TTV_STS_MARQUE) != TTV_STS_MARQUE)
    {
     ptsav = ttvfig ;
     continue ;
    }

   if(ttvfig == TTV_LIST_TTVFIG)
    {
     TTV_LIST_TTVFIG = TTV_LIST_TTVFIG->NEXT ;
    }
   else
    {
     ptsav->NEXT = ttvfig->NEXT ;
    }

   ttv_freettvfigmemory(ttvfig,TTV_STS_D|TTV_STS_E|TTV_STS_F|TTV_STS_T|
                               TTV_STS_J|TTV_STS_P|TTV_STS_S) ;
   ttv_freereflist(ttvfig,ttvfig->CONSIG,ttvfig->NBCONSIG) ;
   ttv_freereflist(ttvfig,ttvfig->NCSIG,ttvfig->NBNCSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ELCMDSIG,ttvfig->NBELCMDSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ILCMDSIG,ttvfig->NBILCMDSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ELATCHSIG,ttvfig->NBELATCHSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ILATCHSIG,ttvfig->NBILATCHSIG) ;
   ttv_freereflist(ttvfig,ttvfig->EBREAKSIG,ttvfig->NBEBREAKSIG) ;
   ttv_freereflist(ttvfig,ttvfig->IBREAKSIG,ttvfig->NBIBREAKSIG) ;
   ttv_freereflist(ttvfig,ttvfig->EPRESIG,ttvfig->NBEPRESIG) ;
   ttv_freereflist(ttvfig,ttvfig->IPRESIG,ttvfig->NBIPRESIG) ;
   ttv_freereflist(ttvfig,ttvfig->EXTSIG,ttvfig->NBEXTSIG) ;
   ttv_freereflist(ttvfig,ttvfig->INTSIG,ttvfig->NBINTSIG) ;
   ttv_freereflist(ttvfig,ttvfig->ESIG,ttvfig->NBESIG) ;
   if((ttvfig->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
     {
      ttv_delhtmodel(ttvfig) ;
      ttv_delrcxlofig(ttvfig) ;
      if (ttvfig->INFO->FILENAME!=NULL) mbkfree(ttvfig->INFO->FILENAME);
      mbkfree(ttvfig->INFO) ;
     }
   if((ttvfig->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD)
       TTV_HEAD_TTVFIG = delchaindata(TTV_HEAD_TTVFIG,ttvfig) ;
   freechain(ttvfig->INS) ;
   freeptype(ttvfig->USER) ;
   ttv_freettvfigdelay(ttvfig) ;
   ttv_delinfreelist(NULL,ttvfig) ;
   mbkfree((void *)ttvfig) ;
  }

 TTV_NUMB_REFSIG = ttv_cleansbloclist(TTV_HEAD_REFSIG,TTV_NUMB_REFSIG) ;
 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_freettvfigtree()                      */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig a supprimer                               */
/*                                                                           */
/* fonction recursive de liberation de la ttvfig et de tous ses fils         */
/* attention elle ne peut supprimer qu'une ttvfig sommet                     */
/* renvoie :                                                                 */
/* 1 si la ttvfig est supprimer 0 sinon                                      */
/*****************************************************************************/
int ttv_freettvfigtree(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chain ;
 chain_list *hchain = NULL ;

 if(ttvfig == NULL)
   return(0) ;
 
 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   {
    hchain = addchain(hchain,chain->DATA) ;
   }

 if(ttv_freettvfig(ttvfig) == 0) 
  {
   freechain(hchain) ;
   return 0 ;
  }
 else
  {
   for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
     {
      ttv_freettvfigtree((ttvfig_list *)chain->DATA) ;
     }
   freechain(hchain) ;
  }

  return(1);
}

/*****************************************************************************/
/*                        function ttv_freeall()                             */
/* parametres :                                                              */
/*                                                                           */
/* supprime toute la ttvfig                                                  */
/*****************************************************************************/
void ttv_freeall()
{
 ttvfig_list *ttvfig ;
 chain_list *chain ;
 chain_list *chainnext ;

 for(chain = TTV_HEAD_TTVFIG ; chain != NULL ; chain = chainnext)
   {
    chainnext = chain->NEXT ;
    ttvfig = (ttvfig_list *)chain->DATA ;
    if((ttvfig->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK)
      ttv_freeallttvfig(ttvfig) ;
   }
}

/*****************************************************************************/
/*                        function ttv_freeallttvfig()                       */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig a supprimer                               */
/*                                                                           */
/* supprime toute la ttvfig et nettoie la liste des noeuds pour la           */
/* reallocat attention elle ne peut supprimer qu'une ttvfig sommet           */
/* renvoie :                                                                 */
/* 1 si la ttvfig est supprimer 0 sinon                                      */
/*****************************************************************************/
int ttv_freeallttvfig(ttvfig)
ttvfig_list *ttvfig ;
{
 if(ttv_freettvfigtree(ttvfig) == 0)
    return(0) ;
 else
   TTV_NUMB_REFSIG = ttv_cleansbloclist(TTV_HEAD_REFSIG,TTV_NUMB_REFSIG) ;

 return(1);
}

/*****************************************************************************/
/*                        function ttv_allocsbloc()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on doit alloue des signaux                           */
/* pthead : pointeur sur la liste des sblocs ou l'on doit ajouter le bloc    */
/*                                                                           */
/* fonction d'allocation d'un bloc de signaux                                */
/*****************************************************************************/
ttvsbloc_list *ttv_allocsbloc(ttvfig,pthead)
ttvfig_list *ttvfig ;
ttvsbloc_list *pthead ;
{
 ttvsbloc_list *pt ;
 long i ;

 if(TTV_FREE_SBLOC == NULL)
  {
   if(TTV_NUMB_SIG < TTV_MAX_SIG)
     {
      pt = (ttvsbloc_list *)mbkalloc(sizeof(ttvsbloc_list)) ;
      TTV_FREE_SBLOC = pt ;
      TTV_NUMB_SIG++ ;
      pt->NEXT = NULL ;
     }
    else if(ttv_getsbloclist(ttvfig) == 0)
     {
/*
      if(TTV_LANG == TTV_LANG_E)
        ttv_error(50,"signals",TTV_WARNING) ;
      else
        ttv_error(50,"signaux",TTV_WARNING) ;
*/
      pt = (ttvsbloc_list *)mbkalloc(sizeof(ttvsbloc_list)) ;
      TTV_NUMB_SIG++ ;
      TTV_MAX_SIG++ ;
      TTV_FREE_SBLOC = pt ;
      pt->NEXT = NULL ;
     }
  }

 pt = TTV_FREE_SBLOC ;
 TTV_FREE_SBLOC = TTV_FREE_SBLOC->NEXT ;
 pt->NEXT = pthead ;

 for(i = 0 ; i < TTV_MAX_SBLOC ; i++)
  pt->SIG[i].TYPE = TTV_SIG_F ;

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_freesbloclist()                       */
/* parametres :                                                              */
/* ptheah : tete de liste des blocs de signaux                               */
/*                                                                           */
/* fonction de liberation d'une liste de blocs de signaux                    */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
void ttv_freenodeuserdata(ptype_list *ptype)
{
  ptype_list *pt;
  if ((pt=getptype(ptype, TTV_NODE_DUALLINE))!=NULL) freechain((chain_list *)pt->DATA);
  if ((pt=getptype(ptype, TTV_NODE_DUALPATH))!=NULL) freechain((chain_list *)pt->DATA);
}
int ttv_freesbloclist(pthead)
ttvsbloc_list *pthead ;
{
 ttvsig_list *ptsig ;
 ttvsbloc_list *ptsbloc ;
 ttvsbloc_list *ptsblocx ;
 ptype_list *ptype ;
 long i ;

 if(pthead == NULL) return 0 ;

 for(ptsbloc = pthead ; ptsbloc != NULL ; ptsbloc = ptsbloc->NEXT)
  {
   ptsblocx = ptsbloc ;
   for(i = 0 ; i < TTV_MAX_SBLOC ; i++)
     {
      ptsig = ptsbloc->SIG + i ;

      if((ptsig->TYPE & TTV_SIG_F) == TTV_SIG_F)
        continue ;

      if((ptsig->TYPE & TTV_SIG_S) != TTV_SIG_S)
        {
         if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
           {
            if((ptype = getptype(ptsig->USER,TTV_SIG_CMD)) != NULL)
              freechain((chain_list *)ptype->DATA) ;
           }

          if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
           {
            if((ptype = getptype(ptsig->USER,TTV_SIG_CMDOLD)) != NULL)
            freechain((chain_list *)ptype->DATA) ;
           }
        }

      if ((ptype=getptype(ptsig->USER, TTV_SIG_SWING))!=NULL)
        mbkfree(ptype->DATA);
      freeptype(ptsig->USER) ;
      ttv_freenodeuserdata(ptsig->NODE[0].USER);
      freeptype(ptsig->NODE[0].USER) ;
      ttv_freenodeuserdata(ptsig->NODE[1].USER);
      freeptype(ptsig->NODE[1].USER) ;
     }
  }

 ptsblocx->NEXT = TTV_FREE_SBLOC ; 
 TTV_FREE_SBLOC = pthead ;

 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_getsbloclist()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on veut alloue des noeuds                            */
/*                                                                           */
/* recupere des noeuds des ttvfig en fonction de la position et de l'age     */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_getsbloclist(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvfigx ;
 ttvfig_list *ttvfigf ;
 chain_list *chain ;
 chain_list *chain0 = NULL ;
 chain_list *chain1 = NULL ;
 chain_list *chain2 = NULL ;
 ptype_list *ptype ;
 ptype_list *ptypenext ;
 ptype_list *ptypesav = TTV_FREE_MEMTTVFIG ;

 if(TTV_FREE_MEMTTVFIG != NULL)
  {
   for(ptype = TTV_FREE_MEMTTVFIG ; ptype != NULL ; ptype = ptypenext)
    {
     ptypenext = ptype->NEXT ;

     if((ptype->DATA != ttvfig) &&
        ((ptype->TYPE & TTV_STS_S &
         ((ttvfig_list *)ptype->DATA)->STATUS) != 0) &&
        ((((ttvfig_list *)ptype->DATA)->STATUS&(TTV_STS_NOT_FREE|TTV_STS_LOCK))
                                     == 0))
      {
       ttv_freettvfigmemory((ttvfig_list *)ptype->DATA,
                            ptype->TYPE & ~(TTV_STS_T|TTV_STS_P|TTV_STS_J|
                                            TTV_STS_D|TTV_STS_F|TTV_STS_E)) ;
       ptype->TYPE &= ~(TTV_STS_S) ;
      }

     if(ptype->TYPE == 0)
       ttv_delinfreelist(ptypesav,(ttvfig_list *)ptype->DATA) ;

     if(TTV_FREE_SBLOC != NULL)
      return(1) ;
     ptypesav = ptype ;
    }
  }

 ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
 ttv_tagttvfigfree(ttvfig,NULL,NULL,TTV_STS_FREE_2) ;

 for(ttvfigx = ttvfig ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
   {
    ttv_tagttvfigfree(ttvfigx,NULL,NULL,TTV_STS_FREE_1) ;
   }

 for(ttvfigx = TTV_LIST_TTVFIG ; ttvfigx != NULL ; ttvfigx = ttvfigx->NEXT)
   {
    if((ttvfigx->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD)
       ttv_tagttvfigfree(ttvfigx,NULL,NULL,TTV_STS_FREE_0) ;
   }

 for(ttvfigx = TTV_LIST_TTVFIG ; ttvfigx != NULL ; ttvfigx = ttvfigx->NEXT)
   {
    if(((ttvfigx->INS == NULL) || (ttvfigx == ttvfig->ROOT)) && 
       ((ttvfigx->STATUS & TTV_STS_MARQUE) != TTV_STS_MARQUE))
      {
       long freelevel ;
 
       freelevel = ttvfigx->STATUS & TTV_STS_FREE_MASK ;
       switch(freelevel)
         {
          case TTV_STS_FREE_0 : ttvfigf = ttvfigx ;
                                while((ttvfigf->NBISIG == 0) ||
                                     ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigf = ttvfigf->ROOT ;
                                     if(ttvfigf == NULL)
                                        break ;
                                    }
                                if((ttvfigf != NULL) && (ttvfigf != ttvfig) && 
                                   ((ttvfigf->STATUS & TTV_STS_MARQUE) 
                                     != TTV_STS_MARQUE) && 
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_FREE_0))
                                   {
                                    ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                    chain0 = addchain(chain0,(void*)ttvfigf) ;
                                   }
                                break ;
          case TTV_STS_FREE_1 : if(chain0 != NULL) 
                                  {
                                   if(chain1 != NULL)
                                    {
                                     for(chain = chain1 ; chain != NULL ;
                                         chain = chain->NEXT)
                                       ((ttvfig_list *)chain->DATA)->STATUS &= 
                                          ~(TTV_STS_MARQUE) ;
                                     freechain(chain1) ;
                                     chain1 = NULL ;
                                    }
                                   break ; 
                                  }
                                ttvfigf = ttvfigx ;
                                while((ttvfigf->NBISIG == 0) ||
                                     ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigf = ttvfigf->ROOT ;
                                     if(ttvfigf == NULL)
                                        break ;
                                    }
                                if((ttvfigf != NULL) && (ttvfigf != ttvfig) && 
                                   ((ttvfigf->STATUS & TTV_STS_MARQUE) 
                                     != TTV_STS_MARQUE) &&
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_FREE_1))
                                   {
                                    ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                    chain1 = addchain(chain1,(void*)ttvfigf) ;
                                   }
                                break ;
          case TTV_STS_FREE_2 : if((chain0 != NULL) || (chain1 != NULL))
                                  {
                                   if(chain2 != NULL)
                                    {
                                     for(chain = chain2 ; chain != NULL ;
                                         chain = chain->NEXT)
                                       ((ttvfig_list *)chain->DATA)->STATUS &= 
                                          ~(TTV_STS_MARQUE) ;
                                     freechain(chain2) ;
                                     chain2 = NULL ;
                                    }
                                   break ; 
                                  }
                                ttvfigf = ttvfigx ;
                                while((ttvfigf->NBISIG == 0) ||
                                     ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigf = ttvfigf->ROOT ;
                                     if(ttvfigf == NULL)
                                        break ;
                                    }
                                if((ttvfigf != NULL) && (ttvfigf != ttvfig) && 
                                   ((ttvfigf->STATUS & TTV_STS_MARQUE) 
                                     != TTV_STS_MARQUE) &&
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_FREE_2))
                                   {
                                    ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                    chain2 = addchain(chain2,(void*)ttvfigf) ;
                                   }
                                break ;
          default             : break ; 
         }
      }
   }

 ttvfigf = NULL ;

 if(chain0 != NULL)
  {
   for(chain = chain1 ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
   for(chain = chain2 ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
   freechain(chain1) ;
   freechain(chain2) ;
  }
 else
  {
   chain0 = chain1 ;
   if(chain0 != NULL)
    {
     for(chain = chain2 ; chain != NULL ; chain = chain->NEXT)
      ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
     freechain(chain2) ;
    }
   else chain0 = chain2 ;
  }

 if(chain0 != NULL)
  {
   ttvfigf = (ttvfig_list *)chain0->DATA ;
   ttvfigf->STATUS &= ~(TTV_STS_MARQUE) ;
   chain = chain0->NEXT ;
   for(; chain != NULL ; chain = chain->NEXT)
     {
      ttvfigx = (ttvfig_list *)chain->DATA ;
      ttvfigx->STATUS &= ~(TTV_STS_MARQUE) ;
      if(((ttvfigf->STATUS & (TTV_STS_E|TTV_STS_F)) != 0) && 
         ((ttvfigx->STATUS & (TTV_STS_E|TTV_STS_F)) == 0)) 
        {
         ttvfigf = ttvfigx ;
         continue ;
        }
      if((((ttvfigx->STATUS & (TTV_STS_E|TTV_STS_F)) != 0) && 
         ((ttvfigf->STATUS & (TTV_STS_E|TTV_STS_F)) == 0)) ||  
          (ttvfigf->OLD < ttvfigx->OLD))
         continue ;
      if( (ttvfigf->OLD = ttvfigf->OLD) )
        {
         if(ttvfigf->NBISIG >= ttvfigf->NBISIG)
           continue ;
        }
      ttvfigf = ttvfigx ;
     }
   freechain(chain0) ;
  }

 if((ttvfigf != ttvfig) && (ttvfigf != NULL))
   {
    if((ttvfigf->STATUS & (TTV_STS_E|TTV_STS_F)) != 0) 
     {
      ttv_freettvfigmemory(ttvfigf,TTV_STS_F|TTV_STS_E) ;
     }
    ttv_freettvfigmemory(ttvfigf,TTV_STS_S) ;
    ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
    return(1) ;
   }
 else
   {
    ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
    return(0) ;
   }
}

/*****************************************************************************/
/*                        function ttv_cleansbloclist()                      */
/* parametres :                                                              */
/* ptheah : tete de liste des blocs de signaux                               */
/*                                                                           */
/* fonction de nettoyage d'une liste de blocs de signaux                     */
/* renvoie le nouveau nombre de signaux de la liste                          */
/*****************************************************************************/
long ttv_cleansbloclist(pthead,nb)
ttvsbloc_list *pthead ;
long nb ;
{
 ttvsig_list *ptsig ;
 ttvsbloc_list *ptsbloc ;
 ttvsbloc_list *ptsblocx = NULL ;
 ttvsbloc_list *ptsblocsav ;
 chain_list *chain ;
 chain_list *chainx ;
 long i ;

 if((pthead == NULL) || (nb == (long)0))
   return((long)0) ;

 if(pthead->NEXT == NULL) return(nb) ;
 ptsblocsav = pthead ;

 for(ptsbloc = pthead->NEXT ; ptsbloc != NULL ; ptsbloc = ptsbloc->NEXT)
  {
   for(i = 0 ; i < TTV_MAX_SBLOC ; i++)
    {
     ptsig = ptsbloc->SIG + i ;
     if((ptsig->TYPE & TTV_SIG_F) != TTV_SIG_F) break ;
    }
   if(i == TTV_MAX_SBLOC) 
    {
     ptsblocsav->NEXT = ptsbloc->NEXT ;
     nb -= TTV_MAX_SBLOC ;
     ptsbloc->NEXT = ptsblocx ;
     ptsblocx = ptsbloc ;
     ptsbloc = ptsblocsav ;
    }
   else
    {
     ptsblocsav = ptsblocsav->NEXT ;
    }
  }

 if(ptsblocx != NULL)
   {
    for(ptsbloc = ptsblocx ; ptsbloc != NULL ; ptsbloc = ptsbloc->NEXT)
     {
      for(i = 0 ; i < TTV_MAX_SBLOC ; i++)
       {
        (ptsbloc->SIG + i)->TYPE &= ~(TTV_SIG_F) ;
       }
     }
    chainx = TTV_FREE_REFSIG ;
    chain = TTV_FREE_REFSIG ;
    while(chain != NULL)
      {
       ptsig = (ttvsig_list *)chain->DATA ;
       if((ptsig->TYPE & TTV_SIG_F) != TTV_SIG_F)
         {
          if(chain == TTV_FREE_REFSIG)
            {
             TTV_FREE_REFSIG = delchain(TTV_FREE_REFSIG,chain) ; 
             chainx = TTV_FREE_REFSIG ;
             chain = chainx ;
            }
          else
            {
             chainx->NEXT = delchain(chainx->NEXT,chain) ;
             chain = chainx->NEXT ;
            }
         }
       else
         {
          if(chain != TTV_FREE_REFSIG) 
            chainx = chainx->NEXT ;
          chain = chain->NEXT ;
         }
      }
    for(ptsbloc = ptsblocx ; ptsbloc != NULL ; ptsbloc = ptsbloc->NEXT)
     {
      for(i = 0 ; i < TTV_MAX_SBLOC ; i++)
       {
        (ptsbloc->SIG + i)->TYPE |= TTV_SIG_F ;
       }
     }
    ttv_freesbloclist(ptsblocx) ;
   }

 return(nb) ;
}

/*****************************************************************************/
/*                        function ttv_alloclbloc()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on doit alloue des liens                             */
/* pthead : pointeur sur la liste des lblocs ou l'on doit ajouter le bloc    */
/*                                                                           */
/* fonction d'allocation d'un bloc de liens                                  */
/*****************************************************************************/
ttvlbloc_list *ttv_alloclbloc(ttvfig,pthead,type)
ttvfig_list *ttvfig ;
ttvlbloc_list *pthead ;
long type ;
{
 ttvlbloc_list *pt ;
 long i ;

 if(TTV_FREE_LBLOC == NULL)
  {
   if(TTV_NUMB_LINE < TTV_MAX_LINE)
     {
      pt = (ttvlbloc_list *)mbkalloc(sizeof(ttvlbloc_list)) ;
      TTV_NUMB_LINE++ ;
      TTV_FREE_LBLOC = pt ;
      pt->NEXT = NULL ;
     }
   else if(ttv_getlbloclist(ttvfig,type) == 0)
     {
/*
      if(TTV_LANG == TTV_LANG_E)
        ttv_error(50,"paths",TTV_WARNING) ;
      else
        ttv_error(50,"chemins",TTV_WARNING) ;
*/
      pt = (ttvlbloc_list *)mbkalloc(sizeof(ttvlbloc_list)) ;
      TTV_NUMB_LINE++ ;
      TTV_MAX_LINE++ ;
      TTV_FREE_LBLOC = pt ;
      pt->NEXT = NULL ;
     }
  }

 pt = TTV_FREE_LBLOC ;
 TTV_FREE_LBLOC = TTV_FREE_LBLOC->NEXT ;
 pt->NEXT = pthead ;

 for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
  pt->LINE[i].TYPE = TTV_LINE_FR ;

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_freelbloclist()                       */
/* parametres :                                                              */
/* ptheah : tete de liste des blocs de liens                                 */
/*                                                                           */
/* fonction de liberation d'une liste de blocs de liens                      */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_freelbloclist(pthead)
ttvlbloc_list *pthead ;
{
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ttvline_list *ptlinsav ;
 ttvlbloc_list *ptlbloc ;
 ttvlbloc_list *ptlblocx ;
 long i ;

 if(pthead == NULL) return 0 ;

 for(ptlbloc = pthead ; ptlbloc != NULL ; ptlbloc = ptlbloc->NEXT)
  {
   ptlblocx = ptlbloc ;
   for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
     {
      ptline = ptlbloc->LINE + i ;

      if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
        continue ;

      if((ptline->TYPE & TTV_LINE_PREV) == TTV_LINE_PREV)
        {
         for(ptlinex = ptline->NEXT ; ptlinex != NULL ; 
             ptlinex = ptlinex->NEXT)
            {
             if((ptlinex->TYPE & TTV_LINE_PREV) == TTV_LINE_PREV)
                break ;
            }
         ptlinsav = (ttvline_list *)getptype(ptline->USER,
                                             TTV_LINE_PREVLINE)->DATA ;
         ptlinsav->NEXT = ptlinex ;
         if(ptlinex != NULL)
          {
           getptype(ptlinex->USER,TTV_LINE_PREVLINE)->DATA = (void *)ptlinsav ;
          }
        }
      else if((ptline->TYPE & TTV_LINE_ROOT) == TTV_LINE_ROOT)
        {
         for(ptlinex = ptline ; ptlinex != NULL ; ptlinex = ptlinex->NEXT)
            {
             if((ptlinex->TYPE & TTV_LINE_PREV) == TTV_LINE_PREV)
                break ;
            }

         if(ptline->ROOT->INLINE == ptline)
             ptline->ROOT->INLINE = ptlinex ;
         else if(ptline->ROOT->INPATH == ptline)
             ptline->ROOT->INPATH = ptlinex ;

         if(ptlinex != NULL)
           {
            ttv_delprevline(ptlinex) ;
            ptlinex->TYPE |= TTV_LINE_ROOT ;
           }
        }

      freeptype(ptline->USER) ;
     }
  }

 ptlblocx->NEXT = TTV_FREE_LBLOC ;
 TTV_FREE_LBLOC = pthead ;

 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_getlbloclist()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on veut alloue des liens                             */
/*                                                                           */
/* recupere des liens des ttvfig en fonction de la position et de l'age      */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_getlbloclist(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 ttvfig_list *ttvfigx ;
 ttvfig_list *ttvfigf ;
 ttvfig_list *ttvfigff ;
 chain_list *chain ;
 chain_list *chain0 = NULL ;
 chain_list *chain1 = NULL ;
 chain_list *chain2 = NULL ;
 chain_list *chain3 = NULL ;
 ptype_list *ptype ;
 ptype_list *ptypenext ;
 ptype_list *ptypesav = TTV_FREE_MEMTTVFIG ;


 if(TTV_FREE_MEMTTVFIG != NULL)
  {
   for(ptype = TTV_FREE_MEMTTVFIG ; ptype != NULL ; ptype = ptype = ptypenext)
    {
     ptypenext = ptype->NEXT ;

     if((ptype->DATA != ttvfig) && 
        ((ptype->TYPE & (TTV_STS_T|TTV_STS_P|TTV_STS_J|
                         TTV_STS_D|TTV_STS_F|TTV_STS_E) & 
         ((ttvfig_list *)ptype->DATA)->STATUS) != 0) &&
        ((((ttvfig_list *)ptype->DATA)->STATUS&(TTV_STS_NOT_FREE|TTV_STS_LOCK))
                                     == 0))
      {
       ttv_freettvfigmemory((ttvfig_list *)ptype->DATA,
                            ptype->TYPE & ~(TTV_STS_S)) ;
       ptype->TYPE &= ~(ptype->TYPE & ~(TTV_STS_S)) ;
      }

     if(ptype->TYPE == 0)
       ttv_delinfreelist(ptypesav,(ttvfig_list *)ptype->DATA) ;

     if(TTV_FREE_LBLOC != NULL)
      return(1) ;
     ptypesav = ptype ;
    }
  }

 ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
 ttv_tagttvfigfree(ttvfig,NULL,NULL,TTV_STS_FREE_2) ;

 for(ttvfigx = ttvfig ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
   {
    ttv_tagttvfigfree(ttvfigx,NULL,NULL,TTV_STS_FREE_1) ;
   }

 for(ttvfigx = TTV_LIST_TTVFIG ; ttvfigx != NULL ; ttvfigx = ttvfigx->NEXT)
   {
    if((ttvfigx->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD)
       ttv_tagttvfigfree(ttvfigx,NULL,NULL,TTV_STS_FREE_0) ;
   }

 for(ttvfigx = TTV_LIST_TTVFIG ; ttvfigx != NULL ; ttvfigx = ttvfigx->NEXT)
   {
    if(((ttvfigx->INS == NULL) || (ttvfigx == ttvfig->ROOT)) && 
       ((ttvfigx->STATUS & TTV_STS_MARQUE) != TTV_STS_MARQUE))
      {
       long freelevel ;
 
       freelevel = ttvfigx->STATUS & TTV_STS_FREE_MASK ;
       switch(freelevel)
         {
          case TTV_STS_FREE_0 : ttvfigf = ttvfigx ;
                                while(((ttvfigf->NBTBLOC == 0) &&
                                       (ttvfigf->NBJBLOC == 0) &&
                                       (ttvfigf->NBPBLOC == 0) &&
                                       (ttvfigf->NBEBLOC == 0) &&
                                       (ttvfigf->NBFBLOC == 0) &&
                                       (ttvfigf->NBDBLOC == 0)) ||
                                      ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigf = ttvfigf->ROOT ;
                                     if(ttvfigf == NULL)
                                        break ;
                                    }
                                ttvfigff = ttvfigf ;
                                if(ttvfigf != NULL)
                                while(((((type & (TTV_LINE_T | TTV_LINE_J | 
                                         TTV_LINE_P)) != 0) && 
                                      (ttvfigff->NBEBLOC == 0) && 
                                      (ttvfigff->NBFBLOC == 0) && 
                                      (ttvfigff->NBDBLOC == 0)) || 
                                      (((type & (TTV_LINE_D | TTV_LINE_E |
                                         TTV_LINE_F)) != 0) && 
                                      (ttvfigff->NBTBLOC == 0) && 
                                      (ttvfigff->NBJBLOC == 0) && 
                                      (ttvfigff->NBPBLOC == 0))) ||
                                      ((ttvfigff->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigff = ttvfigff->ROOT ;
                                     if(ttvfigff == NULL)
                                        break ;
                                    }
                                if(ttvfigff != NULL) ttvfigf = ttvfigff ;
                                else if(ttvfigf == ttvfig) ttvfigf = NULL ;
                                if((ttvfigf != NULL) && 
                                   ((ttvfigf->STATUS & TTV_STS_MARQUE) 
                                     != TTV_STS_MARQUE) &&
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_FREE_0))
                                   {
                                    ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                    chain0 = addchain(chain0,(void*)ttvfigf) ;
                                   }
                                break ;
          case TTV_STS_FREE_1 : if(chain0 != NULL) 
                                  {
                                   if(chain1 != NULL)
                                    {
                                     for(chain = chain1 ; chain != NULL ;
                                         chain = chain->NEXT)
                                       ((ttvfig_list *)chain->DATA)->STATUS &= 
                                          ~(TTV_STS_MARQUE) ;
                                     freechain(chain1) ;
                                     chain1 = NULL ;
                                    }
                                   break ; 
                                  }
                                ttvfigf = ttvfigx ;
                                while(((ttvfigf->NBTBLOC == 0) && 
                                       (ttvfigf->NBJBLOC == 0) &&
                                       (ttvfigf->NBPBLOC == 0) &&
                                       (ttvfigf->NBEBLOC == 0) &&
                                       (ttvfigf->NBFBLOC == 0) &&
                                       (ttvfigf->NBDBLOC == 0)) ||
                                      ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                       
                                    {
                                     ttvfigf = ttvfigf->ROOT ;
                                     if(ttvfigf == NULL)
                                        break ;
                                    }
                                ttvfigff = ttvfigf ;
                                if(ttvfigf != NULL)
                                while(((((type & (TTV_LINE_T | TTV_LINE_J | 
                                         TTV_LINE_P)) != 0) && 
                                      (ttvfigff->NBEBLOC == 0) && 
                                      (ttvfigff->NBFBLOC == 0) && 
                                      (ttvfigff->NBDBLOC == 0)) || 
                                      (((type & (TTV_LINE_D | TTV_LINE_E |
                                         TTV_LINE_F)) != 0) && 
                                      (ttvfigff->NBTBLOC == 0) && 
                                      (ttvfigff->NBJBLOC == 0) && 
                                      (ttvfigff->NBPBLOC == 0))) ||
                                      ((ttvfigff->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigff = ttvfigff->ROOT ;
                                     if(ttvfigff == NULL)
                                        break ;
                                    }
                                if(ttvfigff != NULL) ttvfigf = ttvfigff ;
                                else if(ttvfigf == ttvfig) ttvfigf = NULL ;
                                if((ttvfigf != NULL) && 
                                   ((ttvfigf->STATUS & TTV_STS_MARQUE) 
                                     != TTV_STS_MARQUE) &&
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_FREE_1))
                                   {
                                    ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                    chain1 = addchain(chain1,(void*)ttvfigf) ;
                                   }
                                break ;
          case TTV_STS_FREE_2 : if((chain0 != NULL) || (chain1 != NULL))
                                  {
                                   if(chain2 != NULL)
                                    {
                                     for(chain = chain2 ; chain != NULL ;
                                         chain = chain->NEXT)
                                       ((ttvfig_list *)chain->DATA)->STATUS &= 
                                          ~(TTV_STS_MARQUE) ;
                                     freechain(chain2) ;
                                     chain2 = NULL ;
                                    }
                                   break ; 
                                  }
                                ttvfigf = ttvfigx ;
                                while(((ttvfigf->NBTBLOC == 0) &&
                                       (ttvfigf->NBJBLOC == 0) &&
                                       (ttvfigf->NBPBLOC == 0) &&
                                       (ttvfigf->NBEBLOC == 0) &&
                                       (ttvfigf->NBFBLOC == 0) &&
                                       (ttvfigf->NBDBLOC == 0)) ||
                                      ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigf = ttvfigf->ROOT ;
                                     if(ttvfigf == NULL)
                                        break ;
                                    }
                                ttvfigff = ttvfigf ;
                                if(ttvfigf != NULL)
                                while(((((type & (TTV_LINE_T | TTV_LINE_J | 
                                         TTV_LINE_P)) != 0) && 
                                      (ttvfigff->NBEBLOC == 0) && 
                                      (ttvfigff->NBFBLOC == 0) && 
                                      (ttvfigff->NBDBLOC == 0)) || 
                                      (((type & (TTV_LINE_D | TTV_LINE_E | 
                                         TTV_LINE_F)) != 0) && 
                                      (ttvfigff->NBTBLOC == 0) && 
                                      (ttvfigff->NBJBLOC == 0) && 
                                      (ttvfigff->NBPBLOC == 0))) ||
                                      ((ttvfigff->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigff = ttvfigff->ROOT ;
                                     if(ttvfigff == NULL)
                                        break ;
                                    }
                                if(ttvfigff != NULL) ttvfigf = ttvfigff ;
                                else if(ttvfigf == ttvfig) ttvfigf = NULL ;
                                if((ttvfigf != NULL) && 
                                   ((ttvfigf->STATUS & TTV_STS_MARQUE) 
                                     != TTV_STS_MARQUE) &&
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_FREE_2))
                                   {
                                    ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                    chain2 = addchain(chain2,(void*)ttvfigf) ;
                                   }
                                break ;
          case TTV_STS_NOT_FREE : if((chain0 != NULL) || (chain1 != NULL) ||
                                     (chain2 != NULL))
                                    {
                                     if(chain3 != NULL)
                                      {
                                       for(chain = chain3 ; chain != NULL ;
                                           chain = chain->NEXT)
                                        ((ttvfig_list *)chain->DATA)->STATUS &=
                                            ~(TTV_STS_MARQUE) ;
                                       freechain(chain3) ;
                                       chain3 = NULL ;
                                      }
                                     break ;
                                    }
                                  ttvfigf = ttvfigx ;
                                  while(((ttvfigf->NBTBLOC == 0) &&
                                       (ttvfigf->NBJBLOC == 0) &&
                                       (ttvfigf->NBPBLOC == 0) &&
                                       (ttvfigf->NBEBLOC == 0) &&
                                       (ttvfigf->NBFBLOC == 0) &&
                                       (ttvfigf->NBDBLOC == 0)) ||
                                      ((ttvfigf->STATUS & TTV_STS_LOCK) != 0))
                                      {
                                       ttvfigf = ttvfigf->ROOT ;
                                       if(ttvfigf == NULL)
                                          break ;
                                      }
                                ttvfigff = ttvfigf ;
                                if(ttvfigf != NULL)
                                while(((((type & (TTV_LINE_T | TTV_LINE_J | 
                                         TTV_LINE_P)) != 0) && 
                                      (ttvfigff->NBEBLOC == 0) && 
                                      (ttvfigff->NBFBLOC == 0) && 
                                      (ttvfigff->NBDBLOC == 0)) || 
                                      (((type & (TTV_LINE_D | TTV_LINE_E |
                                       TTV_LINE_F)) != 0) && 
                                      (ttvfigff->NBTBLOC == 0) && 
                                      (ttvfigff->NBJBLOC == 0) && 
                                      (ttvfigff->NBPBLOC == 0))) ||
                                      ((ttvfigff->STATUS & TTV_STS_LOCK) != 0))
                                    {
                                     ttvfigff = ttvfigff->ROOT ;
                                     if(ttvfigff == NULL)
                                        break ;
                                    }
                                if(ttvfigff != NULL) ttvfigf = ttvfigff ;
                                else if(ttvfigf == ttvfig) ttvfigf = NULL ;
                                  if((ttvfigf != NULL) &&
                                     ((ttvfigf->STATUS & TTV_STS_MARQUE)
                                       != TTV_STS_MARQUE) &&
                                   ((ttvfigf->STATUS & TTV_STS_FREE_MASK) 
                                     == TTV_STS_NOT_FREE))
                                     {
                                      ttvfigf->STATUS |= TTV_STS_MARQUE ;
                                      chain3 = addchain(chain3,(void*)ttvfigf) ;
                                     }
                                  break ; 
         }
      }
   }

 ttvfigf = NULL ;

 if(chain0 != NULL)
  {
   for(chain = chain1 ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
   for(chain = chain2 ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
   for(chain = chain3 ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
   freechain(chain1) ;
   freechain(chain2) ;
   freechain(chain3) ;
  }
 else
  {
   chain0 = chain1 ;
   if(chain0 != NULL)
    {
     for(chain = chain2 ; chain != NULL ; chain = chain->NEXT)
      ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
     for(chain = chain3 ; chain != NULL ; chain = chain->NEXT)
      ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
     freechain(chain2) ;
     freechain(chain3) ;
    }
   else
    {
     chain0 = chain2 ;
     if(chain0 != NULL)
       {
        for(chain = chain3 ; chain != NULL ; chain = chain->NEXT)
         ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_MARQUE) ;
        freechain(chain3) ;
       }
     else chain0 = chain3 ;
    }
  }

 if(chain0 != NULL)
  {
   ttvfigf = (ttvfig_list *)chain0->DATA ;
   ttvfigf->STATUS &= ~(TTV_STS_MARQUE) ;
   chain = chain0->NEXT ;
   for(; chain != NULL ; chain = chain->NEXT)
     {
      ttvfigx = (ttvfig_list *)chain->DATA ;
      ttvfigx->STATUS &= ~(TTV_STS_MARQUE) ;
      if((type & (TTV_LINE_T | TTV_LINE_P | TTV_LINE_J)) != 0) 
       {
        if(((ttvfigf->NBDBLOC == 0) && (ttvfigf->NBEBLOC == 0) &&
            (ttvfigf->NBFBLOC == 0)) &&
           ((ttvfigx->NBDBLOC != 0) || (ttvfigx->NBEBLOC != 0) ||
           (ttvfigx->NBFBLOC != 0)))
           {
            ttvfigf = ttvfigx ;
            continue ;
           }
        else if(((ttvfigf->NBDBLOC != 0) || (ttvfigf->NBEBLOC != 0) ||
                 (ttvfigf->NBFBLOC != 0)) &&
                ((ttvfigx->NBDBLOC == 0) && (ttvfigx->NBEBLOC == 0) &&
                (ttvfigx->NBFBLOC == 0)))
          {
           continue ;
          }
       }
      else
       {
        if(((ttvfigf->NBTBLOC == 0) && (ttvfigf->NBJBLOC == 0) && 
            (ttvfigf->NBPBLOC == 0)) &&
           ((ttvfigx->NBTBLOC != 0) || (ttvfigx->NBJBLOC != 0) || 
            (ttvfigx->NBPBLOC != 0)))
           {
            ttvfigf = ttvfigx ;
            continue ;
           }
        else if(((ttvfigf->NBTBLOC != 0) || (ttvfigf->NBJBLOC != 0) || 
                 (ttvfigf->NBPBLOC != 0)) &&
                ((ttvfigx->NBTBLOC == 0) && (ttvfigx->NBJBLOC == 0) && 
                 (ttvfigx->NBPBLOC == 0)))
          {
           continue ;
          }
       }
          
      if(ttvfigf->OLD < ttvfigx->OLD)
         continue ;
      ttvfigf = ttvfigx ;
     }
   freechain(chain0) ;
  }

 if(ttvfigf != NULL)
   {
    if((((type & (TTV_LINE_T | TTV_LINE_J | TTV_LINE_P)) != 0) && 
        ((ttvfigf->NBEBLOC != 0) || (ttvfigf->NBDBLOC != 0) ||
         (ttvfigf->NBFBLOC != 0))) ||
       ((ttvfigf->NBTBLOC == 0) && (ttvfigf->NBJBLOC == 0) && 
        (ttvfigf->NBPBLOC == 0) &&
        ((ttvfigf->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE)))
     {
      if(((ttvfigf->STATUS & TTV_STS_D) == TTV_STS_D) && 
          (ttvfigf->NBDBLOC != 0))
       {
        ttv_freettvfigmemory(ttvfigf,TTV_STS_D) ;
       }
      else if(((ttvfigf->STATUS & TTV_STS_F) == TTV_STS_F) && 
              (ttvfigf->NBFBLOC != 0))
       {
        ttv_freettvfigmemory(ttvfigf,TTV_STS_F) ;
       }
      else if(((ttvfigf->STATUS & TTV_STS_E) == TTV_STS_E) && 
              (ttvfigf->NBEBLOC != 0))
       {
        ttv_freettvfigmemory(ttvfigf,TTV_STS_E) ;
       }
      ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
      return(1) ;
     }
    else if((((type & (TTV_LINE_D | TTV_LINE_E | TTV_LINE_F)) != 0) &&
             ((ttvfigf->NBTBLOC != 0) || (ttvfigf->NBJBLOC != 0) || 
              (ttvfigf->NBPBLOC != 0))) ||
            ((ttvfigf->NBEBLOC == 0) && (ttvfigf->NBDBLOC == 0) &&
             (ttvfigf->NBFBLOC == 0) &&
            ((ttvfigf->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE)))
     {
      if(((ttvfigf->STATUS & TTV_STS_T) == TTV_STS_T) && 
         (ttvfigf->NBTBLOC != 0))
       {
        ttv_freettvfigmemory(ttvfigf,TTV_STS_T) ;
       }
      else if(((ttvfigf->STATUS & TTV_STS_P) == TTV_STS_P) && 
              (ttvfigf->NBPBLOC != 0))
       {
        ttv_freettvfigmemory(ttvfigf,TTV_STS_P) ;
       }
      else if(((ttvfigf->STATUS & TTV_STS_J) == TTV_STS_J) && 
              (ttvfigf->NBJBLOC != 0))
       {
        ttv_freettvfigmemory(ttvfigf,TTV_STS_J) ;
       }
      ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
      return(1) ;
     }
   else
     {
      ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
      return(0) ;
     }
   }
 else
   {
    ttv_cleantagttvfig(TTV_STS_FREE_0 | TTV_STS_FREE_1 | TTV_STS_FREE_2) ;
    return(0) ;
   }
}

/*****************************************************************************/
/*                        function ttv_allocpath()                           */
/* parametres :                                                              */
/* headpath : liste des chemin                                               */
/* ttvfig : ttvfig du chemin                                                 */
/* root : noeud extremite du chemin                                          */
/* node : noeud origine du chemin                                            */
/* type : type de recherche                                                  */
/* delay : delai                                                             */
/* slope : front                                                             */
/*                                                                           */
/* rajoue un chemin a la liste des chemins                                   */
/*****************************************************************************/
static ttvpath_list *ttv_getnewpath()
{
 ttvpath_list *pt ;
 int i ;
#ifndef MORE_NOHEAPALLOC
 if (TTV_FREE_PATH == NULL)
  {
   pt = (ttvpath_list *)mbkalloc(TTV_MAX_BLOC * sizeof(ttvpath_list));
   TTV_FREE_PATH = pt;
   for (i = 1 ; i < TTV_MAX_BLOC ; i++)
   {
    pt->NEXT = pt + 1;
    pt++;
   }
   pt->NEXT = NULL;
  }

 pt = TTV_FREE_PATH;
 TTV_FREE_PATH = TTV_FREE_PATH->NEXT;
#else
 pt=mbkalloc(sizeof(ttvpath_list));
#endif
 return pt;
}

ttvpath_list *ttv_allocpath(headpath,ttvfig,root,node,cmd,latch,cmdlatch,latchlist,data,access,refaccess,type,delay,slope,newdelay,newslope,start,starts,md,mf,crossmin,phase,clockpathdelay)
ttvpath_list *headpath ;
ttvfig_list *ttvfig ;
ttvevent_list  *root ;
ttvevent_list  *node ;
ttvevent_list  *cmd ;
ttvevent_list  *latch ;
ttvevent_list  *cmdlatch ;
ptype_list  *latchlist ;
long data ;
long access ;
long refaccess ;
long type ;
long delay ;
long slope ;
long newdelay ;
long newslope ;
long start ;
long starts ;
timing_model *md ;
timing_model *mf ;
long crossmin;
char phase;
long clockpathdelay;
{
 ttvpath_list *pt ;

 pt=ttv_getnewpath();

 pt->FIG = ttvfig ;
 pt->ROOT = root ;
 pt->NODE = node ;
 pt->CMD = cmd ;
 pt->LATCH = latch ;
 pt->CMDLATCH = cmdlatch ;
 pt->LATCHLIST = latchlist ;
 pt->CRITIC = NULL ;
 pt->ACCESS = access ;
 pt->DATADELAY = data ;
 pt->REFACCESS = refaccess ;
 pt->TYPE = type ;
 pt->DELAY = newdelay ;
 pt->SLOPE = newslope ;
 pt->REFDELAY = delay ;
 pt->REFSLOPE = slope ;
 pt->DELAYSTART = start ;
 pt->SLOPESTART = starts ;
 pt->NEXT = headpath ;
 pt->MD = md ;
 pt->MF = mf ;
 pt->CROSSMINDELAY = crossmin ;
 pt->USER = NULL ;
 pt->TTV_MORE_SEARCH_OPTIONS=0;
 pt->PHASE=phase;
 pt->CLOCKPATHDELAY=clockpathdelay;

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_freepathlist()                        */
/* parametres :                                                              */
/* pthead : liste des noeuds du detail du chemin a liberer                   */
/*                                                                           */
/* fonction de liberation d'un detail de chemin                              */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_freepathlist(pthead)
ttvpath_list *pthead ;
{
 ttvpath_list *pt, *npt ;
 if(pthead == NULL) return 0 ;
 for(pt=pthead;pt;pt=npt)
  {
   npt=pt->NEXT;
   if(pt->CRITIC)
     ttv_freecriticlist(pt->CRITIC) ;
   if(pt->MD)
	{
     stm_mod_destroy(pt->MD);
	 pt->MD=NULL;
	}
   if(pt->MF)
	{
     stm_mod_destroy(pt->MF);
	 pt->MF=NULL;
	}
   if(pt->LATCHLIST != NULL)
       freeptype(pt->LATCHLIST) ;
   freeptype(pt->USER);
#ifdef MORE_NOHEAPALLOC
   mbkfree(pt);
#endif 
  }
#ifndef MORE_NOHEAPALLOC
 TTV_FREE_PATH = (ttvpath_list *)append((chain_list *)pthead, 
                                             (chain_list *)TTV_FREE_PATH);
#endif
 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_allocfind()                          */
/* parametres :                                                              */
/*                                                                           */
/* ajoute un element à la recherche                                          */
/*****************************************************************************/

void ttv_initfindstb(ttvfind_stb_stuff *sfs)
{
  sfs->CROSSDELAY = sfs->NOMORECROSSDELAY = TTV_NOTIME ; 
  sfs->IDEAL_CROSSDELAY = sfs->IDEAL_NOMORECROSSDELAY = TTV_NOTIME ; 
  sfs->CROSSDELAY_MIN = TTV_NOTIME ; 
  sfs->PHASE = sfs->STARTPHASE = TTV_NO_PHASE ;
  sfs->PERIOD_CHANGE=0;
  sfs->NEXT=NULL;
  sfs->FLAGS=0;
}

ttvfind_stb_stuff *ttv_allocfindstb()
{
  ttvfind_stb_stuff *sfs;
  if (TTV_FIND_STB_HEAP==NULL)
    {
      TTV_FIND_STB_HEAP=(HeapAlloc *)mbkalloc(sizeof(HeapAlloc));
      CreateHeap(sizeof(ttvfind_stb_stuff), TTV_MAX_BLOC, TTV_FIND_STB_HEAP);
    }
  sfs=(ttvfind_stb_stuff *)AddHeapItem(TTV_FIND_STB_HEAP);
  ttv_initfindstb(sfs);
  return sfs;
}

void ttv_freefindstblist(ttvfind_stb_stuff *head)
{
  ttvfind_stb_stuff *next;
  if (TTV_FIND_STB_HEAP!=NULL)
    {
      while (head!=NULL)
        {
          next=head->NEXT;
          DelHeapItem(TTV_FIND_STB_HEAP, head);
          head=next;
        }
    }
}

ttvpath_stb_stuff *ttv_allocpath_stb_stuff()
{
  ttvpath_stb_stuff *sfs;
  if (TTV_PATH_STB_HEAP==NULL)
    {
      TTV_PATH_STB_HEAP=(HeapAlloc *)mbkalloc(sizeof(HeapAlloc));
      CreateHeap(sizeof(ttvpath_stb_stuff), TTV_MAX_BLOC, TTV_PATH_STB_HEAP);
    }
  sfs=(ttvpath_stb_stuff *)AddHeapItem(TTV_PATH_STB_HEAP);
  return sfs;
}
void ttv_freepathstblist(ttvpath_stb_stuff *head)
{
  ttvpath_stb_stuff *next;
  if (TTV_PATH_STB_HEAP!=NULL)
    {
      while (head!=NULL)
        {
          next=head->NEXT;
          DelHeapItem(TTV_PATH_STB_HEAP, head);
          head=next;
        }
    }
}


ttvfind_list *ttv_allocfind(node)
ttvevent_list *node ;
{
 ttvfind_list *pt ;
 int i ;

 if (TTV_FREE_FIND == NULL)
  {
   pt = (ttvfind_list *)mbkalloc(TTV_MAX_BLOC * sizeof(ttvfind_list));
   TTV_FREE_FIND = pt;
   for (i = 1 ; i < TTV_MAX_BLOC ; i++)
   {
    pt->NEXT = pt + 1;
    pt++;
   }
   pt->NEXT = NULL;
  }

 pt = TTV_FREE_FIND;
 TTV_FREE_FIND = TTV_FREE_FIND->NEXT;

 pt->NEXT = node->FIND ;
 node->FIND = pt ;
 pt->TYPE = (long)0 ; 
 pt->DELAY = TTV_NOTIME ; 
 pt->OUTLINE = NULL ;
 pt->PERIODE = 0 ;

 pt->ORGPHASE = TTV_NO_PHASE ;
 pt->FIFO = 0 ;
 pt->THRU_FILTER = 0 ;
 pt->FLAGS = 0 ;
 pt->STB=NULL;

 return(pt) ;
}
               
/*****************************************************************************/
/*                        function ttv_freefindlist()                        */
/* parametres :                                                              */
/* pthead : liste des find a liberer                                         */
/*                                                                           */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_freefindlist(pthead)
ttvfind_list *pthead ;
{
 ttvfind_list *pt, *last ;

 if(pthead == NULL) return 0 ;

 for(pt = pthead ; pt!= NULL ; last=pt, pt = pt->NEXT)
   ttv_freefindstblist(pt->STB);

 last->NEXT = TTV_FREE_FIND ;
 TTV_FREE_FIND = pthead ;

 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_alloccritic()                         */
/* parametres :                                                              */
/* headcritic : liste des noeuds du detail du chemin                         */
/* ttvfigh : ttvfig courante                                                 */
/* ttvfig : ttvfig du delai elementaire                                      */
/* node : noeud a rajouter au detail du chemin                               */
/* type : type de recherche                                                  */
/* delay : delai                                                             */
/* slope : front                                                             */
/*                                                                           */
/* ajoute un element noeud au detail d'un chemin                             */
/*****************************************************************************/
static ttvcritic_list *ttv_getnewcritic()
{
 ttvcritic_list *pt ;
 int i;
#ifndef MORE_NOHEAPALLOC
 if (TTV_FREE_CRITIC == NULL)
  {
   pt = (ttvcritic_list *)mbkalloc(TTV_MAX_BLOC * sizeof(ttvcritic_list));
   TTV_FREE_CRITIC = pt;
   for (i = 1 ; i < TTV_MAX_BLOC ; i++)
   {
    pt->NEXT = pt + 1;
    pt++;
   }
   pt->NEXT = NULL;
  }

 pt = TTV_FREE_CRITIC;
 TTV_FREE_CRITIC = TTV_FREE_CRITIC->NEXT;
#else
 pt=(ttvcritic_list *)mbkalloc(sizeof(ttvcritic_list));
#endif
 return pt;
}

ttvcritic_list *ttv_alloccritic(ttvcritic_list *headcritic,ttvfig_list *ttvfigh,ttvfig_list *ttvfig,ttvevent_list *node,long type,long data,long delay,long slope,long newdelay,long newslope, char nodeflags, char *linemodelname, ttvline_list *line)
{
 ttvcritic_list *pt ;
 char bufname[1024] ;
 char *name ;
 long gate;

 pt=ttv_getnewcritic();

 if (getptype(node->ROOT->USER, TTV_SIG_CLOCK)!=NULL)
   pt->NODE_FLAG=TTV_NODE_FLAG_ISCLOCK;
 else
   pt->NODE_FLAG=0;

 if (!(TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_SIMPLE_CRITIC))
   {
     if (node->ROOT->ROOT->ROOT==NULL)
     {
       pt->NAME = node->ROOT->NAME;
       pt->NETNAME = node->ROOT->NETNAME;
       pt->NODE_FLAG|=TTV_NODE_FLAG_NOALLOC;
     }
     else
     {
       ttv_getsigname(ttvfigh,bufname,node->ROOT) ;
       name = mbkalloc(strlen(bufname) + 1) ;
       
       pt->NAME = strcpy(name,bufname) ;
       
       ttv_getnetname(ttvfigh,bufname,node->ROOT) ;
       name = mbkalloc(strlen(bufname) + 1) ;
       
       pt->NETNAME = strcpy(name,bufname) ;
     }
   }
 else
   pt->NAME =pt->NETNAME =NULL;

 pt->FIG = ttvfig ;
 pt->TYPE = type ;
 pt->SIGTYPE = node->ROOT->TYPE ;
 pt->DATADELAY = data ;
 pt->DELAY = newdelay ;
 pt->SLOPE = newslope ;
 pt->REFDELAY = delay ;
 pt->REFSLOPE = slope ;
 pt->USER = NULL ;
 pt->NEXT = headcritic ;
 pt->NODE=node; // beware, use only in stb
 pt->LINEMODELNAME=linemodelname;
 pt->LINE=line;

 if (!(TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_SIMPLE_CRITIC))
   {
     pt->SIMDELAY = TTV_NOTIME ;
     pt->SIMSLOPE = TTV_NOSLOPE ;
     pt->MODNAME = NULL ;
     pt->INSNAME = NULL ;
     pt->CAPA = ttv_get_signal_capa(node->ROOT) ;
     if ((node->ROOT->TYPE & TTV_SIG_CO)==TTV_SIG_CO
         || (node->ROOT->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ
         || (node->ROOT->TYPE & TTV_SIG_CB)==TTV_SIG_CB
         || (node->ROOT->TYPE & TTV_SIG_CT)==TTV_SIG_CT
         || (node->ROOT->TYPE & TTV_SIG_N)==TTV_SIG_N
         )
       pt->OUTPUT_CAPA=ttv_get_signal_output_capacitance(ttvfigh, node->ROOT);
     else
       pt->OUTPUT_CAPA=0;
     
     pt->CAPA += pt->OUTPUT_CAPA;
     
     if((node->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
     {
       pt->SNODE = TTV_UP ;
       pt->PNODE = node->ROOT->PNODE[1];
     }
     else
     {
       pt->SNODE = TTV_DOWN ;
       pt->PNODE = node->ROOT->PNODE[0];
     }
     

     pt->NODE_FLAG|=nodeflags;

     pt->PROP=ttv_testsigflag(node->ROOT, 0xffffffff);
   }

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_freecriticlist()                      */
/* parametres :                                                              */
/* pthead : liste des noeuds du detail du chemin a liberer                   */
/*                                                                           */
/* fonction de liberation d'un detail de chemin                              */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_freecriticlist(pthead)
ttvcritic_list *pthead ;
{
 ttvcritic_list *pt, *next ;

 if(pthead == NULL) return 0 ;

 for(pt = pthead ; pt->NEXT != NULL ; pt = next)
   {
    next=pt->NEXT;
    freeptype(pt->USER);
    if ((pt->NODE_FLAG & TTV_NODE_FLAG_NOALLOC)==0)
    {
      if (pt->NAME!=NULL) mbkfree((void*)pt->NAME) ;
      if (pt->NETNAME!=NULL) mbkfree((void*)pt->NETNAME) ;
    }
#ifdef MORE_NOHEAPALLOC
    mbkfree(pt);
#endif
   }

 freeptype(pt->USER);
 if ((pt->NODE_FLAG & TTV_NODE_FLAG_NOALLOC)==0)
   {
     if (pt->NAME!=NULL) mbkfree((void*)pt->NAME) ;
     if (pt->NETNAME!=NULL) mbkfree((void*)pt->NETNAME) ;
   }
#ifdef MORE_NOHEAPALLOC
 mbkfree(pt);
#else
 pt->NEXT = TTV_FREE_CRITIC ;
 TTV_FREE_CRITIC = pthead ;
#endif

 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_allocreflist()                        */
/* parametres :                                                              */
/* chain : chaine de signaux a referencer                                    */
/* nb : nombre de signaux a referencer                                       */
/*                                                                           */
/* cree un tableau de signaux a referencer                                   */
/*****************************************************************************/
ttvsig_list **ttv_allocreflist(chain,nb)
chain_list *chain ;
long nb ;
{
 long i ;
 ttvsig_list **pt ;
 chain_list *chainx ;

 if((chain == NULL) || (nb == 0L)) return(NULL);

 pt = (ttvsig_list **)mbkalloc(nb * sizeof(ttvsig_list*));
   
 chainx = chain ;

 for (i = nb-1 ; i >= 0 ; i--)
  {
   *(pt + i) = (ttvsig_list *)chainx->DATA ;
   chainx = chainx->NEXT ;
  }

 freechain(chain) ;

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_freereflist()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig du liberer                                                */
/* pthead : tableau a liberer                                                */
/* nb : taille tableau                                                       */
/*                                                                           */
/* libere un tableau de signaux reference                                    */
/* si nb = 0 ou ttvfig = NULL ne libere pas les signaux                      */
/* renvoie 1 si c'est libere 0 sinon                                         */
/*****************************************************************************/
int ttv_freereflist(ttvfig,pthead,nb)
ttvfig_list *ttvfig ;
ttvsig_list **pthead ;
long nb ;
{
 ttvsig_list *ptsig ;
 long i ;

 if(pthead == NULL) return 0 ;

 if(ttvfig != NULL)
 if((ttvfig->ROOT == NULL) && ((ttvfig->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD))
 for(i = 0 ; i < nb ; i++)
   {
    ptsig = *(pthead + i) ;
    ttv_delsiglevel(ttvfig,ptsig) ;
    if(ptsig->ROOT == ttvfig)
     {
      if(((ptsig->TYPE & (TTV_SIG_L|TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B)) == 0) || 
         ((pthead != ttvfig->CONSIG) && (pthead != ttvfig->NCSIG)) ||
         (((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) && 
          (ttvfig->ILCMDSIG == NULL) && (ttvfig->ELCMDSIG == NULL)) ||
         (((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L) && 
          (ttvfig->ILATCHSIG == NULL) && (ttvfig->ELATCHSIG == NULL)) ||
         (((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R) && 
          (ttvfig->IPRESIG == NULL) && (ttvfig->EPRESIG == NULL)) ||
         (((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B) &&
          (ttvfig->IBREAKSIG == NULL) && (ttvfig->EBREAKSIG == NULL)))
        ttv_delrefsig(ptsig) ;

     }
   }

 mbkfree(pthead) ;

 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_chainreflist()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig du liberer                                                */
/* pthead : tableau a liberer                                                */
/* nb : taille tableau                                                       */
/*                                                                           */
/* renvoie la chain des signaux d'un tableau                                 */
/*****************************************************************************/
chain_list *ttv_chainreflist(ttvfig,pthead,nb)
ttvfig_list *ttvfig ;
ttvsig_list **pthead ;
long nb ;
{
 ttvsig_list *ptsig ;
 chain_list *chain = NULL ;
 long i ;

 if(pthead == NULL) return NULL ;

 if(ttvfig != NULL)
 for(i = 0 ; i < nb ; i++)
   {
    ptsig = *(pthead + i) ;
    chain = addchain(chain,ptsig) ;
   }
 return(chain) ;
}

/*****************************************************************************/
/*                        function ttv_tagttvfigfree()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ttvfigf1 : ttvfig f1 a marquer jusqu'a ttvfig                             */
/* ttvfigf2 : ttvfig f2 a marquer jusqu'a ttvfig                             */
/* type : type de marquage                                                   */
/*                                                                           */
/* marque les ttvfig d'un arbre suivant la priorite de liberation de type    */
/*****************************************************************************/
void ttv_tagttvfigfree(ttvfig,ttvfigf1,ttvfigf2,type)
ttvfig_list *ttvfig ;
ttvfig_list *ttvfigf1 ;
ttvfig_list *ttvfigf2 ;
long type ;
{
 ttvfig_list *ttvfigx ;
 chain_list *chain ;

 if((ttvfigf1 != NULL) || (ttvfigf2 != NULL))
   {
    if((ttvfig == ttvfigf1) && (ttvfig == ttvfigf2))
      {
       if((ttvfig->STATUS & TTV_STS_FREE_MASK) < type)
         {
          ttvfig->STATUS &= ~(TTV_STS_FREE_MASK) ;
          ttvfig->STATUS |= type ;
         }
       return ;
      }

    ttvfigx = ttvfigf1 ;
    while((ttvfigx != NULL) && (ttvfigx != ttvfig))
      {
       if((ttvfigx->STATUS & TTV_STS_FREE_MASK) < type)
         {
          ttvfigx->STATUS &= ~(TTV_STS_FREE_MASK) ;
          ttvfigx->STATUS |= type ;
         }
       ttvfigx = ttvfigx->ROOT ;
      }

    ttvfigx = ttvfigf2 ;
    while((ttvfigx != NULL) && (ttvfigx != ttvfig))
      {
       if((ttvfigx->STATUS & TTV_STS_FREE_MASK) < type)
         {
          ttvfigx->STATUS &= ~(TTV_STS_FREE_MASK) ;
          ttvfigx->STATUS |= type ;
         }
       ttvfigx = ttvfigx->ROOT ;
      }
    return ;
   }
 else
   {
    if(((ttvfig->STATUS & TTV_STS_FREE_MASK) >= type) &&
       ((ttvfig->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE))
       return ;
    else
     {
      if((ttvfig->STATUS & TTV_STS_FREE_MASK) < type)
         {
          ttvfig->STATUS &= ~(TTV_STS_FREE_MASK) ;
          ttvfig->STATUS |= type ;
         }
      for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
        {
         ttv_tagttvfigfree((ttvfig_list *)chain->DATA,ttvfigf1,ttvfigf2,type) ;
        }
     }
   }
}

/*****************************************************************************/
/*                        function ttv_cleantagttvfig()                      */
/* parametres :                                                              */
/* type : type de marquage                                                   */
/*                                                                           */
/* enleve les marques de free de toutes les ttvfig                           */
/*****************************************************************************/
void ttv_cleantagttvfig(type)
long type ;
{
 ttvfig_list *ttvfigx ;
 
 for(ttvfigx = TTV_LIST_TTVFIG ; ttvfigx != NULL ; ttvfigx = ttvfigx->NEXT)
    {
     ttvfigx->STATUS &= ~(type) ;
    }
}

/*****************************************************************************/
/*                        function ttv_addinfreelist()                       */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig ou il on peut supprimer de la memoire     */
/* type : type de liberation possible                                        */
/*                                                                           */
/* met une figure dans la liste des figures ou il est possible de liberer    */
/* de la memoire                                                             */
/*****************************************************************************/
void ttv_addinfreelist(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 ptype_list *ptype ;

 if((ttvfig->STATUS & type) == 0)
     return ;

 if(TTV_FREE_MEMTTVFIGHT == NULL)
   {
    TTV_FREE_MEMTTVFIGHT = addht(100) ;
   }

 if((ptype = (ptype_list *)gethtitem(TTV_FREE_MEMTTVFIGHT,(void *)ttvfig)) == (void *)EMPTYHT)
  {
   TTV_FREE_MEMTTVFIG = addptype(TTV_FREE_MEMTTVFIG,type,(void *)ttvfig) ;
   addhtitem(TTV_FREE_MEMTTVFIGHT,(void *)ttvfig,(long)TTV_FREE_MEMTTVFIG) ;
  }
 else
  {
   ptype->TYPE |= type & ttvfig->STATUS ;
  }
}

/*****************************************************************************/
/*                        function ttv_delinfreelist()                       */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig a supprimer de la liste                   */
/*                                                                           */
/* supprime une figure de la liste des figures ou il est possible de liberer */
/* de la memoire                                                             */
/* si ttvfig == NULL alors on les supprime toutes                            */
/*****************************************************************************/
int ttv_delinfreelist(head,ttvfig)
ptype_list *head ;
ttvfig_list *ttvfig ;
{
 ptype_list *ptype ;
 ptype_list *ptypesav ;
 int res = 0 ;

 if(ttvfig == NULL)
  {
   if(ttvfig == NULL)
    {
     freeptype(TTV_FREE_MEMTTVFIG) ;
     TTV_FREE_MEMTTVFIG = NULL ;
     delht(TTV_FREE_MEMTTVFIGHT) ;
     TTV_FREE_MEMTTVFIGHT = NULL ;
     return(1) ;
    }
   else
     return(0) ;
  }
 else
  {
   if(head == NULL)
    {
     if(TTV_FREE_MEMTTVFIGHT != NULL)
     if(gethtitem(TTV_FREE_MEMTTVFIGHT,(void *)ttvfig) == EMPTYHT)
       return(0) ;
     ptype = TTV_FREE_MEMTTVFIG ;
    }
   else
    {
     ptype = head->NEXT ;
     ptypesav = head ;
    }
   while(ptype != NULL)
    {
     if(ptype->DATA == (void*)ttvfig)
      {
       if(ptype == TTV_FREE_MEMTTVFIG)
        {
         TTV_FREE_MEMTTVFIG = TTV_FREE_MEMTTVFIG->NEXT ;
         ptype->NEXT = NULL ;
         freeptype(ptype) ;
        }
       else
        {
         ptypesav->NEXT = ptype->NEXT ;
         ptype->NEXT = NULL ;
         freeptype(ptype) ;
        }
       delhtitem(TTV_FREE_MEMTTVFIGHT,(void *)ttvfig) ;
       return(1) ;
      }
      ptypesav = ptype ;
      ptype = ptype->NEXT ;
    }
  }

 return(res) ;
}

/*****************************************************************************/
/*                        function ttv_freememoryifmax()                    */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig ou il faut supprimer de la memoire        */
/* type : type de memoire qu'il faut liberer                                 */
/*                                                                           */
/* fonction de liberation de memoire sur une ttvfig en fonction du type      */
/* renvoie :                                                                 */
/* 1 si la memoire a ete supprimer 0 sinon                                   */
/*****************************************************************************/
int ttv_freememoryifmax(ttvfig,type)
    ttvfig_list *ttvfig ;
    long type ;
{
 int res = 1 ;

 if((ttvfig->STATUS & TTV_STS_LOCK) == TTV_STS_LOCK)
   return(0) ;

 if(((type & (TTV_STS_D|TTV_STS_E|TTV_STS_F|TTV_STS_T|TTV_STS_J|TTV_STS_D)) != 0) && (TTV_NUMB_LINE >= TTV_MAX_LINE))
   res = ttv_freettvfigmemory(ttvfig,type&~(TTV_STS_S)) ;
 if(((type & TTV_STS_S) != 0) && (TTV_NUMB_SIG >= TTV_MAX_SIG))
   res = ttv_freettvfigmemory(ttvfig,type&~(TTV_STS_D|TTV_STS_E|TTV_STS_F|TTV_STS_T|TTV_STS_J|TTV_STS_D)) ;

 return(res) ;
}

/*****************************************************************************/
/*                        function ttv_freememoryiffull()                    */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig ou il faut supprimer de la memoire        */
/* type : type de memoire qu'il faut liberer                                 */
/*                                                                           */
/* fonction de liberation de memoire sur une ttvfig en fonction du type      */
/* renvoie :                                                                 */
/* 1 si la memoire a ete supprimer 0 sinon                                   */
/*****************************************************************************/
int ttv_freememoryiffull(ttvfig,type)
    ttvfig_list *ttvfig ;
    long type ;
{
 int res = 1 ;

 if((ttvfig->STATUS & TTV_STS_LOCK) == TTV_STS_LOCK)
   return(0) ;

 if(((type & (TTV_STS_D|TTV_STS_E|TTV_STS_F|TTV_STS_T|TTV_STS_J|TTV_STS_D)) != 0) && (TTV_NUMB_LINE >= TTV_MAX_LINE) && (TTV_FREE_LBLOC == NULL))
   res = ttv_freettvfigmemory(ttvfig,type&~(TTV_STS_S)) ;
 if(((type & TTV_STS_S) != 0) && (TTV_NUMB_SIG >= TTV_MAX_SIG) && (TTV_FREE_SBLOC == NULL))
   res = ttv_freettvfigmemory(ttvfig,type&~(TTV_STS_D|TTV_STS_E|TTV_STS_F|TTV_STS_T|TTV_STS_J|TTV_STS_D)) ;

 return(res) ;
}

/*****************************************************************************/
/*                        function ttv_freettvfigmemory()                    */
/* parametres :                                                              */
/* ttvfig : pointeur sur la ttvfig ou il faut supprimer de la memoire        */
/* type : type de memoire qu'il faut liberer                                 */
/*                                                                           */
/* fonction de liberation de memoire sur une ttvfig en fonction du type      */
/* renvoie :                                                                 */
/* 1 si la memoire a ete supprimer 0 sinon                                   */
/*****************************************************************************/
int ttv_freettvfigmemory(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 int res = 1 ;

 if((ttvfig->STATUS & TTV_STS_LOCK) == TTV_STS_LOCK)
   return(0) ;

 if(((type & TTV_STS_D) == TTV_STS_D) || ((type & TTV_STS_C) == TTV_STS_C))
   {
    ttv_freedualline(ttvfig,TTV_STS_DUAL_D) ;
    ttvfig->STATUS &= ~(TTV_STS_D|TTV_STS_DENOTINPT) ;
    ttvfig->NBDBLOC = (long)0 ;
    if(ttv_freelbloclist(ttvfig->DBLOC) == 0)
      res = 0 ;
    else ttvfig->DBLOC = NULL ;
   }

 if(((type & TTV_STS_E) == TTV_STS_E) || ((type & TTV_STS_S) == TTV_STS_S))
   {
    ttv_freedualline(ttvfig,TTV_STS_DUAL_E) ;
    ttvfig->STATUS &= ~(TTV_STS_E|TTV_STS_DENOTINPT) ;
    ttvfig->NBEBLOC = (long)0 ;
    if(ttv_freelbloclist(ttvfig->EBLOC) == 0)
      res = 0 ;
    else ttvfig->EBLOC = NULL ;
   }

if(((type & TTV_STS_F) == TTV_STS_F) || ((type & TTV_STS_S) == TTV_STS_S))
   {
    ttv_freedualline(ttvfig,TTV_STS_DUAL_F) ;
    ttvfig->STATUS &= ~(TTV_STS_F|TTV_STS_DENOTINPT) ;
    ttvfig->NBFBLOC = (long)0 ;
    if(ttv_freelbloclist(ttvfig->FBLOC) == 0)
      res = 0 ;
    else ttvfig->FBLOC = NULL ;
   }

 if(((type & TTV_STS_T) == TTV_STS_T) || ((type & TTV_STS_C) == TTV_STS_C))
   {
    ttv_freedualline(ttvfig,TTV_STS_DUAL_T) ;
    ttvfig->STATUS &= ~(TTV_STS_T) ;
    ttvfig->NBTBLOC = (long)0 ;
    if(ttv_freelbloclist(ttvfig->TBLOC) == 0)
      res = 0 ;
    else ttvfig->TBLOC = NULL ;
   }

 if((type & TTV_STS_J) == TTV_STS_J)
   {
    ttv_freedualline(ttvfig,TTV_STS_DUAL_J) ;
    ttvfig->STATUS &= ~(TTV_STS_J) ;
    ttvfig->NBJBLOC = (long)0 ;
    if(ttv_freelbloclist(ttvfig->JBLOC) == 0)
      res = 0 ;
    else ttvfig->JBLOC = NULL ;
   }

 if((type & TTV_STS_P) == TTV_STS_P)
   {
    ttv_freedualline(ttvfig,TTV_STS_DUAL_P) ;
    ttvfig->STATUS &= ~(TTV_STS_P) ;
    ttvfig->NBPBLOC = (long)0 ;
    if(ttv_freelbloclist(ttvfig->PBLOC) == 0)
      res = 0 ;
    else ttvfig->PBLOC = NULL ;
   }

 if((type & TTV_STS_S) == TTV_STS_S)
   {
    ttvfig->STATUS &= ~(TTV_STS_S) ;
    ttvfig->NBISIG = (long)0 ;
    
    ttv_delsigtab(ttvfig) ;
    if(ttv_freesbloclist(ttvfig->ISIG) == 0)
      res = 0 ;
    else ttvfig->ISIG = NULL ;
    if((getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL)
      {
       ttv_freehtabttvfig(ttvfig,TTV_STS_S) ;
       ttv_builthtabttvfig(ttvfig,TTV_STS_S) ;
      }
   }

 if((type & TTV_STS_C) == TTV_STS_C)
   {
    ttvfig->STATUS &= ~(TTV_STS_C) ;
    if(ttv_freereflist(ttvfig,ttvfig->NCSIG, ttvfig->NBNCSIG) != 0)
     {
      ttvfig->NCSIG = NULL ;
      ttvfig->NBNCSIG = (long)0 ;
     }        
    else res = 0;
    if(ttv_freereflist(ttvfig,ttvfig->CONSIG, ttvfig->NBCONSIG) != 0)
     {
      ttvfig->CONSIG = NULL ;
      ttvfig->NBCONSIG = (long)0 ;
     }
    else res = 0;
    if((getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
      {
       ttv_freehtabttvfig(ttvfig,TTV_STS_L) ;
       ttv_builthtabttvfig(ttvfig,TTV_STS_L) ;
      }
   }

 return(res) ;
}

/*****************************************************************************/
/*                        function ttv_allocdualline()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig ou il faut contruire le graphe dual                       */
/* status : etat dual demander                                               */
/*                                                                           */
/* ajoute a chaque noeud du graphe des liens dual du type demander           */
/*****************************************************************************/
void ttv_allocdualline(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ttvevent_list *ptnode ;
 ttvlbloc_list *ptlbloc ;
 ptype_list *ptype ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 long i ;

 if(((status & TTV_STS_DUAL_T) == TTV_STS_DUAL_T) && 
    ((ttvfig->STATUS & TTV_STS_DUAL_T) != TTV_STS_DUAL_T) &&
    ((ttvfig->STATUS & TTV_STS_T) == TTV_STS_T))
   {
    chain = addchain(chain,(void *)ttvfig->TBLOC) ;
    ttvfig->STATUS |= TTV_STS_DUAL_T ;
   }

 if(((status & TTV_STS_DUAL_J) == TTV_STS_DUAL_J) && 
    ((ttvfig->STATUS & TTV_STS_DUAL_J) != TTV_STS_DUAL_J) &&
    ((ttvfig->STATUS & TTV_STS_J) == TTV_STS_J))
   {
    chain = addchain(chain,(void *)ttvfig->JBLOC) ;
    ttvfig->STATUS |= TTV_STS_DUAL_J ;
   }
    
 if(((status & TTV_STS_DUAL_P) == TTV_STS_DUAL_P) && 
    ((ttvfig->STATUS & TTV_STS_DUAL_P) != TTV_STS_DUAL_P) &&
    ((ttvfig->STATUS & TTV_STS_P) == TTV_STS_P))
   {
    chain = addchain(chain,(void *)ttvfig->PBLOC) ;
    ttvfig->STATUS |= TTV_STS_DUAL_P ;
   }
    
 if(((status & TTV_STS_DUAL_D) == TTV_STS_DUAL_D) && 
    ((ttvfig->STATUS & TTV_STS_DUAL_D) != TTV_STS_DUAL_D) &&
    ((ttvfig->STATUS & TTV_STS_D) == TTV_STS_D))
   {
    chain = addchain(chain,(void *)ttvfig->DBLOC) ;
    ttvfig->STATUS |= TTV_STS_DUAL_D ;
   }
    
 if(((status & TTV_STS_DUAL_E) == TTV_STS_DUAL_E) && 
    ((ttvfig->STATUS & TTV_STS_DUAL_E) != TTV_STS_DUAL_E) &&
    ((ttvfig->STATUS & TTV_STS_E) == TTV_STS_E))
   {
    chain = addchain(chain,(void *)ttvfig->EBLOC) ;
    ttvfig->STATUS |= TTV_STS_DUAL_E ;
   }
    
 if(((status & TTV_STS_DUAL_F) == TTV_STS_DUAL_F) && 
    ((ttvfig->STATUS & TTV_STS_DUAL_F) != TTV_STS_DUAL_F) &&
    ((ttvfig->STATUS & TTV_STS_F) == TTV_STS_F))
   {
    chain = addchain(chain,(void *)ttvfig->FBLOC) ;
    ttvfig->STATUS |= TTV_STS_DUAL_F ;
   }
    
 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
 for(ptlbloc = (ttvlbloc_list *)chainx->DATA ; ptlbloc != NULL ; 
     ptlbloc = ptlbloc->NEXT)
  {
   for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
     {
      ptline = ptlbloc->LINE + i ;
      if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR) continue ;
      ptnode = ptline->NODE ;

      ptline->TYPE |= TTV_LINE_ROOT_DUAL ;

      if((ptline->TYPE & (TTV_LINE_D | TTV_LINE_E | TTV_LINE_F)) != 0)
        {
         ptype = getptype(ptnode->USER,TTV_NODE_DUALLINE) ;
         if(ptype == NULL)
         ptype = ptnode->USER = addptype(ptnode->USER,TTV_NODE_DUALLINE,NULL) ;
        }
      else 
        {
         ptype = getptype(ptnode->USER,TTV_NODE_DUALPATH) ;
         if(ptype == NULL)
         ptype = ptnode->USER = addptype(ptnode->USER,TTV_NODE_DUALPATH,NULL) ;
        }

      ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,ptline) ;

      if(((chain_list *)ptype->DATA)->NEXT != NULL)
        {
         ptlinex = (ttvline_list *)(((chain_list *)ptype->DATA)->NEXT)->DATA ;
         ptlinex->TYPE &= ~(TTV_LINE_ROOT_DUAL) ;
        
         if((ptlinex->FIG != ptline->FIG) ||
            ((ptline->TYPE & TTV_LINE_TYPE) != (ptlinex->TYPE & TTV_LINE_TYPE)))
          {
           ptlinex->TYPE |= TTV_LINE_PREV_DUAL ;
           ptlinex->USER = 
                    addptype(ptlinex->USER,TTV_LINE_PREVLDUAL,ptype->DATA) ;
          }
        }
     }
  }

 freechain(chain) ;
}

/*****************************************************************************/
/*                        function ttv_freedualline()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig ou il faut detruire le graphe dual                        */
/* status : etat dual demander                                               */
/*                                                                           */
/* enleve les liens du graphe dual                                           */
/*****************************************************************************/
void ttv_freedualline(ttvfig,status)
ttvfig_list *ttvfig ;
long status ;
{
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ttvevent_list *ptnode ;
 ttvlbloc_list *ptlbloc ;
 ptype_list *ptype ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 chain_list *chainline ;
 chain_list *chainsav ;
 long i ;

 if(((status & TTV_STS_DUAL_T) == TTV_STS_DUAL_T) &&
    ((ttvfig->STATUS & TTV_STS_DUAL_T) == TTV_STS_DUAL_T) &&
    ((ttvfig->STATUS & TTV_STS_T) == TTV_STS_T))
   {
    chain = addchain(chain,(void *)ttvfig->TBLOC) ;
    ttvfig->STATUS &= ~(TTV_STS_DUAL_T) ;
   }

 if(((status & TTV_STS_DUAL_J) == TTV_STS_DUAL_J) &&
    ((ttvfig->STATUS & TTV_STS_DUAL_J) == TTV_STS_DUAL_J) &&
    ((ttvfig->STATUS & TTV_STS_J) == TTV_STS_J))
   {
    chain = addchain(chain,(void *)ttvfig->JBLOC) ;
    ttvfig->STATUS &= ~(TTV_STS_DUAL_J) ;
   }

 if(((status & TTV_STS_DUAL_P) == TTV_STS_DUAL_P) &&
    ((ttvfig->STATUS & TTV_STS_DUAL_P) == TTV_STS_DUAL_P) &&
    ((ttvfig->STATUS & TTV_STS_P) == TTV_STS_P))
   {
    chain = addchain(chain,(void *)ttvfig->PBLOC) ;
    ttvfig->STATUS &= ~(TTV_STS_DUAL_P) ;
   }

 if(((status & TTV_STS_DUAL_D) == TTV_STS_DUAL_D) &&
    ((ttvfig->STATUS & TTV_STS_DUAL_D) == TTV_STS_DUAL_D) &&
    ((ttvfig->STATUS & TTV_STS_D) == TTV_STS_D))
   {
    chain = addchain(chain,(void *)ttvfig->DBLOC) ;
    ttvfig->STATUS &= ~(TTV_STS_DUAL_D) ;
   }

 if(((status & TTV_STS_DUAL_E) == TTV_STS_DUAL_E) &&
    ((ttvfig->STATUS & TTV_STS_DUAL_E) == TTV_STS_DUAL_E) &&
    ((ttvfig->STATUS & TTV_STS_E) == TTV_STS_E))
   {
    chain = addchain(chain,(void *)ttvfig->EBLOC) ;
    ttvfig->STATUS &= ~(TTV_STS_DUAL_E) ;
   }

 if(((status & TTV_STS_DUAL_F) == TTV_STS_DUAL_F) &&
    ((ttvfig->STATUS & TTV_STS_DUAL_F) == TTV_STS_DUAL_F) &&
    ((ttvfig->STATUS & TTV_STS_F) == TTV_STS_F))
   {
    chain = addchain(chain,(void *)ttvfig->FBLOC) ;
    ttvfig->STATUS &= ~(TTV_STS_DUAL_F) ;
   }

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
 for(ptlbloc = (ttvlbloc_list *)chainx->DATA ; ptlbloc != NULL ;
     ptlbloc = ptlbloc->NEXT)
  {
   for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
     {
      ptline = ptlbloc->LINE + i ;

      if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
        continue ;

      if((ptline->TYPE & TTV_LINE_PREV_DUAL) == TTV_LINE_PREV_DUAL)
       {
        chainsav = (chain_list *)getptype(ptline->USER,
                                            TTV_LINE_PREVLDUAL)->DATA ;
        chainsav = chainsav->NEXT ;
        for(chainline = chainsav->NEXT ; chainline != NULL ; 
                                         chainline = chainline->NEXT)
           {
            ptlinex = (ttvline_list *)chainline->DATA ;
            if((ptlinex->TYPE & TTV_LINE_PREV_DUAL) == TTV_LINE_PREV_DUAL)
               break ;
            chainsav = chainline ;
           }

        chainsav->NEXT = NULL ;
        chainsav = (chain_list *)getptype(ptline->USER,
                                          TTV_LINE_PREVLDUAL)->DATA ;
        freechain(chainsav->NEXT) ;
        chainsav->NEXT = chainline ;

        if(chainline != NULL)
          getptype(ptlinex->USER,TTV_LINE_PREVLDUAL)->DATA = (void *)chainsav ;

        ptline->USER = delptype(ptline->USER,TTV_LINE_PREVLDUAL) ;
        ptline->TYPE &= ~(TTV_LINE_PREV_DUAL) ;
       }
      else if((ptline->TYPE & TTV_LINE_ROOT_DUAL) == TTV_LINE_ROOT_DUAL)
       {
        ptnode = ptline->NODE ;
        ptline->TYPE &= ~(TTV_LINE_ROOT_DUAL) ;

        if((ptline->TYPE & (TTV_LINE_D | TTV_LINE_E | TTV_LINE_F)) != 0)
          {
           ptype = getptype(ptnode->USER,TTV_NODE_DUALLINE) ;
          }
        else
          {
           ptype = getptype(ptnode->USER,TTV_NODE_DUALPATH) ;
          }

         for(chainline = (chain_list *)ptype->DATA ; chainline != NULL ;
                                    chainline = chainline->NEXT)
          {
           ptlinex = (ttvline_list *)chainline->DATA ;
           if((ptlinex->TYPE & TTV_LINE_PREV_DUAL) == TTV_LINE_PREV_DUAL)
              break ;
           chainsav = chainline ;
          }

         if(chainline != NULL)
           {
            ptlinex->USER = delptype(ptlinex->USER,TTV_LINE_PREVLDUAL) ;
            ptlinex->TYPE |= TTV_LINE_ROOT_DUAL ;
            ptlinex->TYPE &= ~(TTV_LINE_PREV_DUAL) ;
            chainsav->NEXT = NULL ;
           }

          freechain(ptype->DATA) ;
          ptype->DATA = chainline ;
       }
     }
 }

 freechain(chain) ;
}

/*****************************************************************************/
/*                        function ttv_alloclinedelay()                      */
/* parametres :                                                              */
/* line a allouer                                                            */
/*                                                                           */
/* ajoute la structure delay d'une line                                      */
/*****************************************************************************/
ttvdelay_list *ttv_alloclinedelay(line)
ttvline_list *line ;
{
 ttvfig_list *ttvfig = line->FIG ;
 int size ;
 int pos ;
 int index ;

 pos = ttv_getdelaypos(line->FIG,line->TYPE,&size) ;
 if(line->INDEX == TTV_LINE_NOINDEX)
    ttv_alloclineindex(ttvfig,line->TYPE) ;
 index = line->INDEX ;
 return(ttv_allocdelayline(ttvfig,pos,index,size)) ;
}

/*****************************************************************************/
/*                        function ttv_allocdelayline()                      */
/* parametres :                                                              */
/* line a allouer                                                            */
/*                                                                           */
/* ajoute la structure delay d'une line                                      */
/*****************************************************************************/
ttvdelay_list *ttv_allocdelayline(ttvfig,pos,index,size)
ttvfig_list *ttvfig ;
int pos ;
int index ;
int size ;
{
 ttvdelay_list **delay ;
 ttvdelay_list *ptdelay ;
 int i ;

 if(ttvfig->DELAY == NULL)
  {
   ttvfig->DELAY = (ttvdelay_list ***)mbkalloc(6 * sizeof(ttvdelay_list **)) ;
   for(i = 0 ; i < 6 ; i++)
     ttvfig->DELAY[i] = NULL ;
  }

 delay = ttvfig->DELAY[pos] ;

 if(delay == NULL)
  {
   ttvfig->DELAY[pos] = mbkalloc((size + 1) * sizeof(ttvdelay_list *)) ;
   delay = ttvfig->DELAY[pos] ;
   for(i = 0 ; i < size ; i++)
     delay[i] = NULL ;
   delay[i] = TTV_DELAY_END ;
  }

 ptdelay = delay[index] ;

 if(ptdelay == NULL)
   {
    delay[index] = mbkalloc(sizeof(ttvdelay_list)) ;
    ptdelay = delay[index] ;
    ptdelay->VALMAX = TTV_NOTIME ;
    ptdelay->VALMIN = TTV_NOTIME ;
    ptdelay->FMAX = TTV_NOSLOPE ;
    ptdelay->FMIN = TTV_NOSLOPE ;
    ptdelay->CMAX = TTV_NOCAPA ;
    ptdelay->CMIN = TTV_NOCAPA ;
    ptdelay->CDRIVERMAX = -1.0 ;
    ptdelay->CDRIVERMIN = -1.0 ;
    ptdelay->RDRIVERMAX = -1.0 ;
    ptdelay->RDRIVERMIN = -1.0 ;
    ptdelay->USER = NULL ;
   }

 return(ptdelay) ;
}

/*****************************************************************************/
/*                        function ttv_freettvfigdelay()                     */
/* parametres :                                                              */
/* line a allouer                                                            */
/*                                                                           */
/* ajoute la structure delay d'une line                                      */
/*****************************************************************************/
void ttv_freettvfigdelay(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvdelay_list **delay ;
 int tab[6] ;
 int i ;
 int j ;

 if(ttvfig->DELAY == NULL)
  {
   return ;
  }

 tab[TTV_DELAY_P] = ttvfig->NBPBLOC ;
 tab[TTV_DELAY_J] = ttvfig->NBJBLOC ;
 tab[TTV_DELAY_T] = ttvfig->NBTBLOC ;
 tab[TTV_DELAY_F] = ttvfig->NBFBLOC ;
 tab[TTV_DELAY_E] = ttvfig->NBEBLOC ;
 tab[TTV_DELAY_D] = ttvfig->NBDBLOC ;

 for(i = 0 ; i < 6 ; i++)
  {
   if(ttvfig->DELAY[i] != NULL)
    {
     delay = ttvfig->DELAY[i] ;
     for(j = 0 ; j < tab[i] ; j++)
       mbkfree(delay[j]) ;
     mbkfree(delay) ;
    }
  }

 mbkfree(ttvfig->DELAY) ;

 ttvfig->DELAY = NULL ;
}

/*****************************************************************************/
/*                        function ttv_alloclineindex()                      */
/* parametres :                                                              */
/* ttvfig : ttvfig ou il faut contruire le graphe dual                       */
/* type : type de line                                                       */
/*                                                                           */
/* ajoute l'index a chaque ttvline                                           */
/*****************************************************************************/
void ttv_alloclineindex(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 ttvline_list *ptline ;
 ttvlbloc_list *ptlbloc ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 long i ;
 int index ;

 type &= TTV_LINE_TYPE ;

 if((type & TTV_LINE_T) == TTV_LINE_T)
   {
    chain = addchain(chain,(void *)ttvfig->TBLOC) ;
   }

 if((type & TTV_LINE_J) == TTV_LINE_J)
   {
    chain = addchain(chain,(void *)ttvfig->JBLOC) ;
   }
    
 if((type & TTV_LINE_P) == TTV_LINE_P)
   {
    chain = addchain(chain,(void *)ttvfig->PBLOC) ;
   }
    
 if((type & TTV_LINE_D) == TTV_LINE_D)
   {
    chain = addchain(chain,(void *)ttvfig->DBLOC) ;
   }
    
 if((type & TTV_LINE_E) == TTV_LINE_E)
   {
    chain = addchain(chain,(void *)ttvfig->EBLOC) ;
   }
    
 if((type & TTV_LINE_F) == TTV_LINE_F)
   {
    chain = addchain(chain,(void *)ttvfig->FBLOC) ;
   }
    
 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  {
   index = 0 ;
   for(ptlbloc = (ttvlbloc_list *)chainx->DATA ; ptlbloc != NULL ; 
       ptlbloc = ptlbloc->NEXT)
    {
     for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
       {
        ptline = ptlbloc->LINE + i ;
        if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR) continue ;
        ptline->INDEX = index++ ;
       }
    }
  }

 freechain(chain) ;
}

/*****************************************************************************/
/*                        function ttv_allocsigcapas ()                      */
/*****************************************************************************/
ttvsig_capas *ttv_allocsigcapas ( float cu, float cumin, float cumax,
                                  float cd, float cdmin, float cdmax)
{
 ttvsig_capas *ttvsigcapa;

 ttvsigcapa = (ttvsig_capas*)mbkalloc(sizeof(ttvsig_capas));

 ttvsigcapa->CAPAUP    = cu;
 ttvsigcapa->CAPAUPMIN = cumin;
 ttvsigcapa->CAPAUPMAX = cumax;
 ttvsigcapa->CAPADN    = cd;
 ttvsigcapa->CAPADNMIN = cdmin;
 ttvsigcapa->CAPADNMAX = cdmax;

 return ttvsigcapa;
}

ttvcritic_list *ttv_dupcritic(ttvcritic_list *pt)
{
  ttvcritic_list *cr;
  if (pt==NULL) return NULL;
  cr=ttv_getnewcritic();
  memcpy(cr, pt, sizeof(ttvcritic_list));
  cr->USER=NULL;
  if ((cr->NODE_FLAG & TTV_NODE_FLAG_NOALLOC)==0)
  {
    if (cr->NAME!=NULL) cr->NAME=mbkstrdup(cr->NAME);
    if (cr->NETNAME!=NULL) cr->NETNAME=mbkstrdup(cr->NETNAME);
  }
  cr->NEXT=ttv_dupcritic(pt->NEXT);
  return cr;
}

ttvpath_list *ttv_duppath(ttvpath_list *pt)
{
  ttvpath_list *npt;

  npt=ttv_getnewpath();
  memcpy(npt, pt, sizeof(ttvpath_list));
  npt->NEXT=NULL;
  npt->USER=dupptypelst(npt->USER);
  npt->LATCHLIST=dupptypelst(npt->LATCHLIST);
  npt->MD=npt->MF=NULL;
  npt->CRITIC=ttv_dupcritic(npt->CRITIC);
  return npt;
}
