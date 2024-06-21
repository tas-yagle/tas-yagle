/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_fig.c                                                   */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fonction d'acces a la ttvfig                                             */
/****************************************************************************/

#include "ttv.h"
#include "ttv_ssta.h"

// for the APIs
#include "api_communication.h"
#include "ttv_communication.c"
// ------------

ttvsbloc_list *TTV_HEAD_REFSIG = NULL ;
chain_list *TTV_FREE_REFSIG = NULL ;
long TTV_NUMB_REFSIG ;
ht *TTV_HT_MODEL = NULL ;
float TTV_UNIT = 10.0 ;

typedef int testname_func(ttvfig_list *ttvfig, ttvsig_list *ptsig, chain_list *mask);
chain_list *ttv_getallsigbytype_sub(ttvfig_list *ttvfig,long type,chain_list *mask, testname_func testmask);

/*****************************************************************************/
/*                        function ttv_addhtmodel()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/*                                                                           */
/* ajoute un model dans la table des modeles                                 */
/*****************************************************************************/
void ttv_addhtmodel(ttvfig)
ttvfig_list *ttvfig ;
{
  if(TTV_HT_MODEL == NULL)
   TTV_HT_MODEL = addht(10) ;

  if((ttvfig->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
    sethtitem(TTV_HT_MODEL,ttvfig->INFO->FIGNAME,(long)ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_delhtmodel()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/*                                                                           */
/* supprime un model dans la table des modeles                               */
/*****************************************************************************/
void ttv_delhtmodel(ttvfig)
ttvfig_list *ttvfig ;
{
  if(TTV_HT_MODEL == NULL)
     return ;

    delhtitem(TTV_HT_MODEL,ttvfig->INFO->FIGNAME) ;
}


/*****************************************************************************/
/*                        function ttv_gethtmodel()                          */
/* parametres :                                                              */
/* name : nom de la figure                                                   */
/*                                                                           */
/* trouve le model dans la table des models                                  */
/*****************************************************************************/
ttvfig_list *ttv_gethtmodel(name)
char *name ;
{
 ttvfig_list *ttvfig ;

 if(TTV_HT_MODEL == NULL)
  return(NULL) ;

  ttvfig = (ttvfig_list *)gethtitem(TTV_HT_MODEL,name) ;

  if((ttvfig == (ttvfig_list *)EMPTYHT) || (ttvfig == (ttvfig_list *)DELETEHT))
    ttvfig = NULL ;

 return(ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_addsigtab()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/*                                                                           */
/* ajoute un tableau de signal a une figure                                  */
/*****************************************************************************/
ttvsbloc_list **ttv_addsigtab(ttvfig)
ttvfig_list *ttvfig ;
{
 ptype_list *ptype = getptype(ttvfig->USER,TTV_FIG_SIGBLOCTAB) ;
 ttvsbloc_list **tabbloc = NULL ;
 ttvsbloc_list *ptsbloc ;
 long nbloc ;
 long i ;

 if(ptype != NULL)
  return((ttvsbloc_list **)ptype->DATA) ;

 if(ttvfig->ISIG != NULL)
  {
   nbloc = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC)) % TTV_MAX_SBLOC ;
   nbloc = (ttvfig->NBISIG + nbloc) ;
   nbloc = nbloc / TTV_MAX_SBLOC ;

   tabbloc = (ttvsbloc_list **)mbkalloc((int)nbloc * sizeof(ttvsbloc_list *)) ;

   ptsbloc = ttvfig->ISIG ;

   for(i = 0L ; i < nbloc ; i++)
     {
      *(tabbloc + i) = ptsbloc ;
      ptsbloc = ptsbloc->NEXT ;
     }

   ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_SIGBLOCTAB,tabbloc) ;
  }

 return(tabbloc) ;
}

/*****************************************************************************/
/*                        function ttv_delsigtab()                           */
/* parametres :                                                              */
/* ttvfig :                                                                  */
/*                                                                           */
/* ajoute un tableau de signal a une figure                                  */
/*****************************************************************************/
void ttv_delsigtab(ttvfig)
ttvfig_list *ttvfig ;
{
 ptype_list *ptype = getptype(ttvfig->USER,TTV_FIG_SIGBLOCTAB) ;

 if(ptype != NULL)
  {
   mbkfree(ptype->DATA) ;
   ttvfig->USER = delptype(ttvfig->USER,TTV_FIG_SIGBLOCTAB) ;
  }
}

static void ttv_set_rcx_node_info(ttvfig_list *ttvfig, ttvsig_list *ptsig)
{
   lofig_list *rcx_lofig;
   locon_list *rcx_locon;
   num_list *cl;
   int nm;
 
   if ((rcx_lofig=rcx_getlofig(ttvfig->INFO->FIGNAME, NULL))!=NULL)
   {
      rcx_locon=rcx_gethtrcxcon(NULL, rcx_lofig, ptsig->NAME);
      if (rcx_locon && rcx_locon->PNODE)
      {
         cl=rcx_getnodebytransition( rcx_locon, TRC_SLOPE_DOWN);
         if (cl!=NULL) nm=cl->DATA; else nm=rcx_locon->PNODE->DATA;
         if (nm>0) ptsig->PNODE[0]=nm;
         cl=rcx_getnodebytransition( rcx_locon, TRC_SLOPE_UP);
         if (cl!=NULL) nm=cl->DATA; else nm=rcx_locon->PNODE->DATA;
         if (nm>0) ptsig->PNODE[1]=nm;
      }
   }
}
/*****************************************************************************/
/*                        function ttv_addsig()                              */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on rajoute un signal                                 */
/* name : nom du signal                                                      */
/* type : type du signal                                                     */
/*                                                                           */
/* ajoute un signal non reference a une ttvfig le type doit etre TTV_SIG_S   */
/* avec un masque supplementaire si on veut                                  */
/*****************************************************************************/
static int ttv_addrcxpnode=0;
int ttv_addsig_addrcxpnode(int mode)
{
  int old=ttv_addrcxpnode;
  ttv_addrcxpnode=mode;
  return old;
}

ttvsig_list *ttv_addsig(ttvfig,name,net,capa,type)
ttvfig_list *ttvfig ;
char *name ;
char *net ;
float capa ;
long type ;
{
 ttvsig_list *ptsig ;
 long nbsig;
 
 nbsig = (long)(ttvfig->NBISIG  % (long)TTV_MAX_SBLOC);
 if(nbsig == (long)0) 
       ttvfig->ISIG = ttv_allocsbloc(ttvfig,ttvfig->ISIG) ;

 ptsig = ttvfig->ISIG->SIG + ((long)TTV_MAX_SBLOC - (nbsig + 1)) ;
 ttvfig->NBISIG =  ttvfig->NBISIG + (long)1 ;
 ptsig->NAME = namealloc(name) ;
 ptsig->NETNAME = namealloc(net) ;
 ptsig->CAPA = capa ;
 ptsig->ROOT = ttvfig ;
 ptsig->TYPE = type ;
 ptsig->USER = NULL ;
 ptsig->NODE[0].ROOT = ptsig ;
 ptsig->NODE[0].TYPE = TTV_NODE_DOWN ;
 ptsig->NODE[0].FIND = NULL ;
 ptsig->NODE[0].INLINE = NULL ;
 ptsig->NODE[0].INPATH = NULL ;
 ptsig->NODE[0].USER = NULL ;
 ptsig->NODE[1].ROOT = ptsig ;
 ptsig->NODE[1].TYPE = TTV_NODE_UP ;
 ptsig->NODE[1].FIND = NULL ;
 ptsig->NODE[1].INLINE = NULL ;
 ptsig->NODE[1].INPATH = NULL ;
 ptsig->NODE[1].USER = NULL ;
 ptsig->PNODE[0]=-1;
 ptsig->PNODE[1]=-1;

 ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
 if (ttv_addrcxpnode) ttv_set_rcx_node_info(ttvfig, ptsig);
 return(ptsig) ;
}

/*****************************************************************************/
/*                        function ttv_addrefsig()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on rajoute un signal                                 */
/* name : nom du signal                                                      */
/* type : type du signal                                                     */
/* headchain : chain list des autres signaux references du meme type         */
/*                                                                           */
/* cree un signal reference a une ttvfig et l'ajoute dans une chain_list     */
/* il faut construire le tableau plus tard pour que la ttvfig soit coherente */
/*****************************************************************************/
void ttv_init_refsig(ttvfig_list *ttvfig, ttvsig_list *ptsig, char *name, char *net, float capa, long type)
{
 ptsig->NAME = namealloc(name) ;
 ptsig->NETNAME = namealloc(net) ;
 ptsig->CAPA = capa ;
 ptsig->ROOT = ttvfig ;
 ptsig->TYPE = type ;
 ptsig->USER = NULL ;

 ptsig->NODE[0].INLINE = NULL ;
 ptsig->NODE[1].INLINE = NULL ;
 ptsig->NODE[0].INPATH = NULL ;
 ptsig->NODE[1].INPATH = NULL ;
 ptsig->NODE[0].ROOT = ptsig ;
 ptsig->NODE[0].TYPE = TTV_NODE_DOWN ;
 ptsig->NODE[0].FIND = NULL ;
 ptsig->NODE[0].USER = NULL ;
 ptsig->NODE[1].ROOT = ptsig ;
 ptsig->NODE[1].TYPE = TTV_NODE_UP ;
 ptsig->NODE[1].FIND = NULL ;
 ptsig->NODE[1].USER = NULL ;
 ptsig->PNODE[0]=-1;
 ptsig->PNODE[1]=-1;
 ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
 if (ttv_addrcxpnode) ttv_set_rcx_node_info(ttvfig, ptsig);
}

chain_list *ttv_addrefsig(ttvfig,name,net,capa,type,headchain)
ttvfig_list *ttvfig ;
char *name ;
char *net ;
float capa ;
long type ;
chain_list *headchain ;
{
 ttvsig_list *ptsig ;
 long nbsig;
 
 if(TTV_FREE_REFSIG == NULL)
    {
     nbsig = (long)(TTV_NUMB_REFSIG  % (long)TTV_MAX_SBLOC);
     if(nbsig == (long)0) 
       TTV_HEAD_REFSIG = ttv_allocsbloc(ttvfig,TTV_HEAD_REFSIG) ;
    ptsig = TTV_HEAD_REFSIG->SIG + ((long)TTV_MAX_SBLOC - (nbsig + 1)) ;
    TTV_NUMB_REFSIG = TTV_NUMB_REFSIG + (long)1 ;
    }
 else
    {
     ptsig = (ttvsig_list *)TTV_FREE_REFSIG->DATA ;
     TTV_FREE_REFSIG = delchain(TTV_FREE_REFSIG,TTV_FREE_REFSIG) ;
    }

 ttv_init_refsig(ttvfig, ptsig, name, net, capa, type);

 if((ptsig->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_Q | TTV_SIG_R | TTV_SIG_B |
                    TTV_SIG_N)) != 0)
    {
     ptsig->NODE[0].TYPE |= TTV_NODE_STOP ;
     ptsig->NODE[1].TYPE |= TTV_NODE_STOP ;
    }

 return(addchain(headchain,ptsig)) ;
}

/*****************************************************************************/
/*                        function ttv_gettypesig()                          */
/* parametres :                                                              */
/*                                                                           */
/* renvoie le type d'un signal                                               */
/*****************************************************************************/
long ttv_gettypesig(ptsig)
ttvsig_list *ptsig ;
{
 long type ;

 type = ptsig->TYPE & TTV_SIG_TYPE ;

 if((type & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B)) != 0)
   type &= ~(TTV_SIG_C|TTV_SIG_N) ;

 return(type) ;
}

/*****************************************************************************/
/*                        function ttv_delrefsig()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on rajoute un signal                                 */
/* name : nom du signal                                                      */
/* type : type du signal                                                     */
/* headchain : chain list des autres signaux references du meme type         */
/*                                                                           */
/* cree un signal reference a une ttvfig et l'ajoute dans une chain_list     */
/* il faut construire le tableau plus tard pour que la ttvfig soit coherente */
/*****************************************************************************/
void ttv_delrefsig(ptsig)
ttvsig_list *ptsig ;
{
 ptype_list *ptype ;

 TTV_FREE_REFSIG = addchain(TTV_FREE_REFSIG,ptsig) ;

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

 ptsig->TYPE |= TTV_SIG_F ;

 freeptype(ptsig->USER) ;
 ttv_freenodeuserdata(ptsig->NODE[0].USER);
 freeptype(ptsig->NODE[0].USER) ;
 ttv_freenodeuserdata(ptsig->NODE[1].USER);
 freeptype(ptsig->NODE[1].USER) ;
 ptsig->USER = NULL ;
 ptsig->NODE[0].USER = NULL ;
 ptsig->NODE[1].USER = NULL ;
}

/*****************************************************************************/
/*                        function ttv_addline()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig ou l'on rajoute un lien                                   */
/* root : noeud extremite du lien                                            */
/* node : noeud debut du lien                                                */
/* valmax : valeur du temps le plus long du lien                             */
/* fmax : valeur du front le plus long du lien                               */
/* valmin : valeur du temps le plus court du lien                            */
/* fmin : valeur du front le plus court du lien                              */
/* type : type de lien                                                       */
/*                                                                           */
/* cree un lien dans une ttvfig et relie les deux noeuds                     */
/*****************************************************************************/
ttvline_list *ttv_addline(ttvfig,root,node,valmax,fmax,valmin,fmin,type)
ttvfig_list *ttvfig ;
ttvevent_list *root ;
ttvevent_list *node ;
long valmax ;
long fmax ;
long valmin ;
long fmin ;
long type ;
{
 ttvline_list *ptline ;
 long nbline ;
 long typex ;

 typex = type & TTV_LINE_TYPE ; 

 switch(typex)
   {
    case TTV_LINE_T : nbline = (long)(ttvfig->NBTBLOC % (long)TTV_MAX_LBLOC) ;
                      if(nbline == (long)0) 
                         ttvfig->TBLOC = ttv_alloclbloc(ttvfig,
                                                   ttvfig->TBLOC,TTV_LINE_T) ;
                      ptline = ttvfig->TBLOC->LINE + 
                               ((long)TTV_MAX_LBLOC - (nbline + 1)) ;
                      ttvfig->NBTBLOC = ttvfig->NBTBLOC + (long)1 ;
                      if(root->INPATH != NULL)
                        root->INPATH->TYPE &= ~(TTV_LINE_ROOT) ;
                      ptline->NEXT = root->INPATH ;
                      root->INPATH = ptline ;
                      break ;
                     
    case TTV_LINE_J : nbline = (long)(ttvfig->NBJBLOC % (long)TTV_MAX_LBLOC) ;
                      if(nbline == (long)0) 
                         ttvfig->JBLOC = ttv_alloclbloc(ttvfig,ttvfig->JBLOC,
                                                   TTV_LINE_J) ;
                      ptline = ttvfig->JBLOC->LINE + 
                               ((long)TTV_MAX_LBLOC - (nbline + 1)) ;
                      ttvfig->NBJBLOC = ttvfig->NBJBLOC + (long)1 ;
                      if(root->INPATH != NULL)
                        root->INPATH->TYPE &= ~(TTV_LINE_ROOT) ;
                      ptline->NEXT = root->INPATH ;
                      root->INPATH = ptline ;
                      break ;

    case TTV_LINE_P : nbline = (long)(ttvfig->NBPBLOC % (long)TTV_MAX_LBLOC) ;
                      if(nbline == (long)0) 
                         ttvfig->PBLOC = ttv_alloclbloc(ttvfig,ttvfig->PBLOC,
                                                   TTV_LINE_P) ;
                      ptline = ttvfig->PBLOC->LINE + 
                               ((long)TTV_MAX_LBLOC - (nbline + 1)) ;
                      ttvfig->NBPBLOC = ttvfig->NBPBLOC + (long)1 ;
                      if(root->INPATH != NULL)
                        root->INPATH->TYPE &= ~(TTV_LINE_ROOT) ;
                      ptline->NEXT = root->INPATH ;
                      root->INPATH = ptline ;
                      break ;

    case TTV_LINE_D : nbline = (long)(ttvfig->NBDBLOC % (long)TTV_MAX_LBLOC) ;
                      if(nbline == (long)0) 
                         ttvfig->DBLOC = ttv_alloclbloc(ttvfig,ttvfig->DBLOC,
                                                   TTV_LINE_D) ;
                      ptline = ttvfig->DBLOC->LINE + 
                               ((long)TTV_MAX_LBLOC - (nbline + 1)) ;
                      ttvfig->NBDBLOC = ttvfig->NBDBLOC + (long)1 ;
                      if(root->INLINE != NULL)
                        root->INLINE->TYPE &= ~(TTV_LINE_ROOT) ;
                      ptline->NEXT = root->INLINE ;
                        root->INLINE = ptline ;
                      break ;

    case TTV_LINE_E : nbline = (long)(ttvfig->NBEBLOC % (long)TTV_MAX_LBLOC) ;
                      if(nbline == (long)0) 
                         ttvfig->EBLOC = ttv_alloclbloc(ttvfig,ttvfig->EBLOC,
                                                   TTV_LINE_E) ;
                      ptline = ttvfig->EBLOC->LINE + 
                               ((long)TTV_MAX_LBLOC - (nbline + 1)) ;
                      ttvfig->NBEBLOC = ttvfig->NBEBLOC + (long)1 ;
                      if(root->INLINE != NULL)
                       root->INLINE->TYPE &= ~(TTV_LINE_ROOT) ;
                      ptline->NEXT = root->INLINE ;
                      root->INLINE = ptline ;
                      break ;
    case TTV_LINE_F : nbline = (long)(ttvfig->NBFBLOC % (long)TTV_MAX_LBLOC) ;
                      if(nbline == (long)0) 
                         ttvfig->FBLOC = ttv_alloclbloc(ttvfig,ttvfig->FBLOC,
                                                   TTV_LINE_F) ;
                      ptline = ttvfig->FBLOC->LINE + 
                               ((long)TTV_MAX_LBLOC - (nbline + 1)) ;
                      ttvfig->NBFBLOC = ttvfig->NBFBLOC + (long)1 ;
                      if(root->INLINE != NULL)
                       root->INLINE->TYPE &= ~(TTV_LINE_ROOT) ;
                      ptline->NEXT = root->INLINE ;
                      root->INLINE = ptline ;
                      break ;
  }

 ptline->FIG = ttvfig ;
 ptline->ROOT = root ;
 ptline->INDEX = TTV_LINE_NOINDEX ;
 ptline->TYPE = (type | TTV_LINE_ROOT) ;
 ptline->NODE = node ;
 ptline->VALMAX = valmax ;
 ptline->FMAX = fmax ;
 ptline->VALMIN = valmin ;
 ptline->FMIN = fmin ;
 ptline->USER = NULL ;
 ptline->MDMIN = NULL ;
 ptline->MFMIN = NULL ;
 ptline->MDMAX = NULL ;
 ptline->MFMAX = NULL ;

 if(ptline->NEXT != NULL)
  if((ptline->NEXT->FIG != ptline->FIG) || 
     ((typex & ptline->NEXT->TYPE) != typex))
     ttv_addprevline(ptline) ;

 return(ptline) ;
}

/*****************************************************************************/
/*                        function ttv_addcaracline()                        */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/* rmax : resistance min                                                     */
/* rmin : resistance min                                                     */
/* smax : parrametre de front max                                            */
/* smin : parrametre de front min                                            */
/*                                                                           */
/* fonction d'allocation d'une caracterisation de lien                       */
/*****************************************************************************/
void ttv_addcaracline(ptline,mdmax,mdmin,mfmax,mfmin)
ttvline_list *ptline ;
char *mdmax ;
char *mdmin ;
char *mfmax ;
char *mfmin ;
{
 ptline->MDMAX = mdmax ;
 ptline->MDMIN = mdmin ;
 ptline->MFMAX = mfmax ;
 ptline->MFMIN = mfmin ;
}

/*****************************************************************************/
/*                        function ttv_calcaracline()                        */
/* parametres :                                                              */
/* ptline : ancienne line                                                    */
/* ptlinenew : nouvelle line                                                 */
/*                                                                           */
/* calcul les caracteristique electriques des nouvelles line                 */
/*****************************************************************************/
int ttv_calcaracline(ptline,ptlinenew,type,index)
ttvline_list *ptline ;
ttvline_list *ptlinenew ;
long type ;
int index ;
{
 ttvfig_list *ttvfig ;
 chain_list *chainfig = NULL ;
 chain_list *chain ;
 char *modeldmax = NULL ;
 char *modeldmin = NULL ;
 char *modelfmax = NULL ;
 char *modelfmin = NULL ;
 char buf[1024], buf0[1024];
 char *name, *hierinsname ;
 
 if(index < 0)
   {
    ttvfig = ptline->FIG ;

    while(ttvfig != ptlinenew->FIG)
       {
        chainfig = addchain(chainfig,ttvfig) ;
        ttvfig = ttvfig->ROOT ;
       }

    name = buf ;
    for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
       {
        sprintf(name,"%s.",((ttvfig_list *)chain->DATA)->INFO->FIGNAME) ;
        name += strlen(name) ;
       }

    hierinsname = buf0 ;
    for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
       {
         if (chain->NEXT!=NULL)
           sprintf(hierinsname,"%s.",((ttvfig_list *)chain->DATA)->INSNAME) ;
         else
           strcpy(hierinsname,((ttvfig_list *)chain->DATA)->INSNAME) ;
        hierinsname += strlen(hierinsname) ;
       }
    hierinsname=buf0;

    freechain(chainfig) ;
   }
 else
   {
    ttvfig = ptline->FIG ;

    while(ttvfig != ptlinenew->FIG)
       {
        chainfig = addchain(chainfig,ttvfig) ;
        ttvfig = ttvfig->ROOT ;
       }

    hierinsname = buf0 ;
    for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
       {
         if (chain->NEXT!=NULL)
           sprintf(hierinsname,"%s.",((ttvfig_list *)chain->DATA)->INSNAME) ;
         else
           strcpy(hierinsname,((ttvfig_list *)chain->DATA)->INSNAME) ;
        hierinsname += strlen(hierinsname) ;
       }
    hierinsname=buf0;

    freechain(chainfig) ;
   }

   if((type & TTV_FIND_MAX) == TTV_FIND_MAX)

   if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    {
     if(ptline->MDMAX != NULL)
      {
       if(index < 0)
        {
         sprintf(name,"%s",ptline->MDMAX);
         modeldmax = namealloc(buf);
        }
       else
        {
         sprintf(buf,"m%d",index);
         modeldmax = namealloc(buf);
        }
       ptlinenew->MDMAX = stm_dupmodelbyname_and_updatedynamicmodelinfo(ptlinenew->FIG->INFO->FIGNAME, modeldmax, ptline->FIG->INFO->FIGNAME, ptline->MDMAX, hierinsname);
       if(index >= 0)
         if(modeldmax == ptlinenew->MDMAX)
            index++ ;
      }
     if(ptline->MFMAX != NULL)
      {
       if(index < 0)
        {
         sprintf(name,"%s",ptline->MFMAX);
         modelfmax = namealloc(buf);
        }
       else
        {
         sprintf(buf,"m%d",index);
         modelfmax = namealloc(buf);
        }
       ptlinenew->MFMAX = stm_dupmodelbyname_and_updatedynamicmodelinfo(ptlinenew->FIG->INFO->FIGNAME, modelfmax, ptline->FIG->INFO->FIGNAME, ptline->MFMAX, hierinsname);
       if(index >= 0)
         if(modelfmax == ptlinenew->MFMAX)
            index++ ;
      }
    }

   if((type & TTV_FIND_MIN) == TTV_FIND_MIN)
    {
     if(ptline->MDMIN != NULL)
      {
       if(index < 0)
        {
         sprintf(name,"%s",ptline->MDMIN);
         modeldmin = namealloc(buf);
        }
       else
        {
         sprintf(buf,"m%d",index);
         modeldmin = namealloc(buf);
        }
       ptlinenew->MDMIN = stm_dupmodelbyname_and_updatedynamicmodelinfo(ptlinenew->FIG->INFO->FIGNAME, modeldmin, ptline->FIG->INFO->FIGNAME, ptline->MDMIN, hierinsname);
       if(index >= 0)
         if(modeldmin == ptlinenew->MDMIN)
            index++ ;
      }
     if(ptline->MFMIN != NULL)
      {
       if(index < 0)
        {
         sprintf(name,"%s",ptline->MFMIN);
         modelfmin = namealloc(buf);
        }
       else
        {
         sprintf(buf,"m%d",index);
         modelfmin = namealloc(buf);
        }
       ptlinenew->MFMIN = stm_dupmodelbyname_and_updatedynamicmodelinfo(ptlinenew->FIG->INFO->FIGNAME, modelfmin, ptline->FIG->INFO->FIGNAME, ptline->MFMIN, hierinsname);
       if(index >= 0)
         if(modelfmin == ptlinenew->MFMIN)
            index++ ;
      }
    }

  return(index) ;
}

/*****************************************************************************/
/*                        function ttv_addconttype()                         */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/* type : type de contraintes                                                */
/*                                                                           */
/* ajoute un type de contraintes à une line                                  */
/*****************************************************************************/
void ttv_addconttype(ptline,type)
ttvline_list *ptline ;
long type ;
{
 ptype_list *ptype ;

 if((ptype = getptype(ptline->USER,TTV_LINE_CONTR)) != NULL)
   ptype->DATA = (void *)type ;
 else 
   ptline->USER = addptype(ptline->USER,TTV_LINE_CONTR,(void *)type) ;
}

/*****************************************************************************/
/*                        function ttv_getconttype()                         */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/*                                                                           */
/* renvoie le type de contrainte                                             */
/*****************************************************************************/
long ttv_getconttype(ptline)
ttvline_list *ptline ;
{
 ptype_list *ptype ;

 if((ptype = getptype(ptline->USER,TTV_LINE_CONTR)) != NULL)
   return((long)ptype->DATA) ;
 else 
  {
   if((ptline->TYPE & TTV_LINE_U) == TTV_LINE_U)
     return(TTV_LINE_SETUP) ;
   if((ptline->TYPE & TTV_LINE_O) == TTV_LINE_O)
     return(TTV_LINE_HOLD) ;
   if((ptline->TYPE & TTV_LINE_A) == TTV_LINE_A)
     return(TTV_LINE_ACCESS) ;
   return((long)0) ;
  }
}

/*****************************************************************************/
/*                        function ttv_delconttype()                         */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/*                                                                           */
/* efface la contrainte d'un lien                                            */
/*****************************************************************************/
void ttv_delconttype(ptline)
ttvline_list *ptline ;
{
 if(getptype(ptline->USER,TTV_LINE_CONTR) != NULL)
   ptline->USER = delptype(ptline->USER,TTV_LINE_CONTR) ;
}

/*****************************************************************************/
/*                        function ttv_addmodelline()                        */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/* model : model de la line                                                  */
/* instance :  instance de la line                                           */
/*                                                                           */
/* donne un nom de modele et d'instance à une line                           */
/*****************************************************************************/
void ttv_addmodelline(ptline,model,instance)
ttvline_list *ptline ;
char *model ;
char *instance ;
{
 ptype_list *ptype ;

 if((ptype = getptype(ptline->USER,TTV_LINE_MODNAME)) != NULL)
   ptype->DATA = model ;
 else 
   ptline->USER = addptype(ptline->USER,TTV_LINE_MODNAME,model) ;

 if((ptype = getptype(ptline->USER,TTV_LINE_INSNAME)) != NULL)
   ptype->DATA = instance ;
 else
   ptline->USER = addptype(ptline->USER,TTV_LINE_INSNAME,instance) ;
}

/*****************************************************************************/
/*                        function ttv_getmodelline()                        */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/* model : model de la line                                                  */
/* instance :  instance de la line                                           */
/*                                                                           */
/* retrouve le nom d'instance et de model d'une line                         */
/*****************************************************************************/
void ttv_getmodelline(ptline,model,instance)
ttvline_list *ptline ;
char **model ;
char **instance ;
{
 ptype_list *ptype ;

 if((ptype = getptype(ptline->USER,TTV_LINE_MODNAME)) != NULL)
   *model = (char *)ptype->DATA ;
 else
   *model = ptline->FIG->INFO->FIGNAME ;

 if((ptype = getptype(ptline->USER,TTV_LINE_INSNAME)) != NULL)
   *instance = (char *)ptype->DATA ;
 else
   *instance = ptline->FIG->INSNAME ;
}

/*****************************************************************************/
/*                        function ttv_delmodelline()                        */
/* parametres :                                                              */
/* ptline : lien a caracterise                                               */
/*                                                                           */
/* efface un nom de modele et d'instance à une line                          */
/*****************************************************************************/
void ttv_delmodelline(ptline)
ttvline_list *ptline ;
{
 if(getptype(ptline->USER,TTV_LINE_MODNAME) != NULL)
   ptline->USER = delptype(ptline->USER,TTV_LINE_MODNAME) ;

 if(getptype(ptline->USER,TTV_LINE_INSNAME) != NULL)
   ptline->USER = delptype(ptline->USER,TTV_LINE_INSNAME) ;
}

/*****************************************************************************/
/*                        function ttv_delline()                             */
/* parametres :                                                              */
/* ptline : lien a supprimer                                                 */
/*                                                                           */
/* supprime un lien du graphe                                                */
/* renvoie 1 si le lien est supprimer 0 sinon                                */
/*****************************************************************************/
int ttv_delline(ptline)
ttvline_list *ptline ;
{
 ttvline_list *ptlinex ;
 ttvline_list *ptlinesav = NULL ;

 if((ptline->TYPE & (TTV_LINE_D | TTV_LINE_E | TTV_LINE_F)) != 0)
  {
   ptlinex = ptline->ROOT->INLINE ;
   if(ptlinex == ptline)
    ptline->ROOT->INLINE = ptlinex->NEXT ;
  }
 else 
  {
   ptlinex = ptline->ROOT->INPATH ;
   if(ptlinex == ptline)
    ptline->ROOT->INPATH = ptlinex->NEXT ;
  }

 for(; (ptlinex != NULL) && (ptlinex != ptline) ; ptlinex = ptlinex->NEXT)
  {
   ptlinesav = ptlinex ;
  }

 if(ptlinex == NULL) return 0 ;

 if(ptlinesav != NULL) ptlinesav->NEXT = ptlinex->NEXT ;

 if((ptlinex->TYPE & TTV_LINE_ROOT) == TTV_LINE_ROOT)
  {
   if(ptlinex->NEXT != NULL)
    {
     ptlinex->NEXT->TYPE |= TTV_LINE_ROOT ;
     if((ptlinex->NEXT->TYPE & TTV_LINE_PREV) == TTV_LINE_PREV)
       ttv_delprevline(ptlinex->NEXT) ;
    }
  }

 if((ptlinex->TYPE & TTV_LINE_PREV) == TTV_LINE_PREV)
   {
    ttv_addprevline(ptlinesav) ;
   }
 else 
   {
    if(ptlinex->NEXT != NULL)
    if((ptlinex->NEXT->TYPE & TTV_LINE_PREV) == TTV_LINE_PREV)
        ttv_addprevline(ptlinesav) ;
   }

 if((ptline->FIG->STATUS & TTV_STS_DUAL) != 0) 
  {
   char flag = 'N' ;

   if(((ptline->TYPE & TTV_LINE_T) == TTV_LINE_T) &&
      ((ptline->FIG->STATUS & TTV_STS_DUAL_T) == TTV_STS_DUAL_T))
     flag = 'Y' ;
   if(((ptline->TYPE & TTV_LINE_J) == TTV_LINE_J) &&
      ((ptline->FIG->STATUS & TTV_STS_DUAL_J) == TTV_STS_DUAL_J))
     flag = 'Y' ;
   if(((ptline->TYPE & TTV_LINE_P) == TTV_LINE_P) &&
      ((ptline->FIG->STATUS & TTV_STS_DUAL_P) == TTV_STS_DUAL_P))
     flag = 'Y' ;
   if(((ptline->TYPE & TTV_LINE_D) == TTV_LINE_D) &&
      ((ptline->FIG->STATUS & TTV_STS_DUAL_D) == TTV_STS_DUAL_D))
     flag = 'Y' ;
   if(((ptline->TYPE & TTV_LINE_E) == TTV_LINE_E) &&
      ((ptline->FIG->STATUS & TTV_STS_DUAL_E) == TTV_STS_DUAL_E))
     flag = 'Y' ;
   if(((ptline->TYPE & TTV_LINE_F) == TTV_LINE_F) &&
      ((ptline->FIG->STATUS & TTV_STS_DUAL_F) == TTV_STS_DUAL_F))
     flag = 'Y' ;

    if(flag == 'Y')
     {
      ptype_list *ptype ;
      chain_list *chain ;
      chain_list *chainsav ;
      if((ptline->TYPE & (TTV_LINE_D | TTV_LINE_E | TTV_LINE_F)) != 0)
        ptype = getptype(ptline->NODE->USER,TTV_NODE_DUALLINE) ;
      else
        ptype = getptype(ptline->NODE->USER,TTV_NODE_DUALPATH) ;
      if(ptype != NULL)
       {
        for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
             chain = chain->NEXT)
          {
           if((ttvline_list *)chain->DATA == ptline) break ;
           chainsav = chain ;
          }
        if(chain != NULL)
         {
          if(chain == (chain_list *)ptype->DATA) 
            ptype->DATA = (void *)chain->NEXT ;
          else
           chainsav->NEXT = chain->NEXT ;
          if(chain->NEXT != NULL)
            ptlinex = (ttvline_list *)chain->NEXT->DATA ;
          if((ptline->TYPE & TTV_LINE_ROOT_DUAL) == TTV_LINE_ROOT_DUAL)
            {
             if(chain->NEXT != NULL)
              {
               ptlinex->TYPE |= TTV_LINE_ROOT_DUAL ;
               if((ptlinex->TYPE & TTV_LINE_PREV_DUAL) == TTV_LINE_PREV_DUAL)
                {
                 ptlinex->TYPE &= ~(TTV_LINE_PREV_DUAL) ;
                 ptlinex->USER = delptype(ptlinex->USER,TTV_LINE_PREVLDUAL) ;
                }
              }
            }
          if((ptline->TYPE & TTV_LINE_PREV_DUAL) == TTV_LINE_PREV_DUAL)
            {
             if(chain->NEXT != NULL)
              {
                if((ptype = getptype(ptlinex->USER,TTV_LINE_PREVLDUAL)) == NULL)
                   ptlinex->USER = addptype(ptlinex->USER,TTV_LINE_PREVLDUAL,
                                            (void *)chainsav) ;
                else ptype->DATA = (void *)chainsav ;
              }
            }
          else 
            {
             if(chain->NEXT != NULL)
              if((ptlinex->TYPE & TTV_LINE_PREV_DUAL) == TTV_LINE_PREV_DUAL)
               getptype(ptlinex->USER,TTV_LINE_PREVLDUAL)->DATA = 
                               (void*)chainsav ;
            }
          chain->NEXT=NULL;
          freechain(chain);
         }
       }
     }
  }

 ptline->TYPE |= TTV_LINE_FR ;
 freeptype(ptline->USER) ;
 ptline->USER = NULL ;
 return(1) ;
}

/*****************************************************************************/
/*                        function ttv_getlinetype()                         */
/* parametres :                                                              */
/* ptline : lien                                                             */
/*                                                                           */
/* donne le type d'un arc                                                    */
/*****************************************************************************/
long ttv_getlinetype(ptline)
ttvline_list *ptline ;
{
 long type ;

 type  = (ptline->TYPE & (TTV_LINE_HZ|TTV_LINE_EV|TTV_LINE_PR|TTV_LINE_R|
                          TTV_LINE_S|TTV_LINE_DENPTMAX|TTV_LINE_DENPTMIN|
                          TTV_LINE_A|TTV_LINE_U|TTV_LINE_O|TTV_LINE_RC|
                          TTV_LINE_TYPE)) ;

 return(type) ;
}

/*****************************************************************************/
/*                        function ttv_getnewlinetype()                      */
/* parametres :                                                              */
/* ptline : lien                                                             */
/* node   : noeud                                                            */
/* root   : noeud                                                            */
/*                                                                           */
/* donne le type d'un arc avec les extremitées                               */
/*****************************************************************************/
long ttv_getnewlinetype(ptline,sigin,sigout)
ttvline_list *ptline ;
ttvsig_list *sigin ;
ttvsig_list *sigout ;
{
 long type ;

 if((((sigin->TYPE & TTV_SIG_C) == TTV_SIG_C) ||
     ((sigin->TYPE & TTV_SIG_N) == TTV_SIG_N)) || 
    (((sigout->TYPE & TTV_SIG_C) == TTV_SIG_C) ||
     ((sigout->TYPE & TTV_SIG_N) == TTV_SIG_N))) 
   {
    type = ptline->TYPE & (TTV_LINE_T | TTV_LINE_D) ;
   }
 else
   {
    if((ptline->TYPE & (TTV_LINE_T|TTV_LINE_P|TTV_LINE_J)) != 0)
     {
      if((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
        type = TTV_LINE_J ;
      else
        type = TTV_LINE_P ;
     }
    else
     {
      if((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
        type = TTV_LINE_E ;
      else
        type = TTV_LINE_F ;
     }
   }

 type |= (ptline->TYPE & (TTV_LINE_HZ|TTV_LINE_EV|TTV_LINE_PR|TTV_LINE_R|
                          TTV_LINE_S|TTV_LINE_DENPTMAX|TTV_LINE_DENPTMIN|
                          TTV_LINE_A|TTV_LINE_U|TTV_LINE_O|TTV_LINE_RC)) ;

 return(type) ;
}

/*****************************************************************************/
/*                        function ttv_delcmd()                              */
/* parametres :                                                              */
/* ttvfig : ttvfig ou il faut chercher une command                           */
/* cmd : noeud command                                                       */
/*                                                                           */
/* effacce les references aux commandes dans une ttvfig                      */
/*****************************************************************************/
void ttv_delcmd(ttvfig,cmd)
ttvfig_list *ttvfig ;
ttvsig_list *cmd ;
{
 ttvsig_list *ptsig ;
 ptype_list *ptype ;

 if((ptype = getptype(cmd->USER,TTV_SIG_CMDNEW)) != NULL)
   {
    ptsig = (ttvsig_list *)ptype->DATA ;
    if((ptype = getptype(ptsig->USER,TTV_SIG_CMDOLD)) != NULL)
      {
       ptype->DATA = delchaindata((chain_list *)ptype->DATA,cmd) ;
       if(ptype->DATA == NULL)
         {
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_CMDOLD) ;
         }
      }
    cmd->USER = delptype(cmd->USER,TTV_SIG_CMDNEW) ;
   }
#ifndef __ALL__WARNING_
 ttvfig = NULL;
#endif
}

/*****************************************************************************/
/*                        function ttv_getcmd()                              */
/* parametres :                                                              */
/* ttvfig : ttvfig ou il faut chercher une command                           */
/* cmd : noeud command                                                       */
/*                                                                           */
/* trouve une commande dans la figure hierarchique ttvfig                    */
/*****************************************************************************/
ttvevent_list *ttv_getcmd(ttvfig,cmd)
ttvfig_list *ttvfig ;
ttvevent_list *cmd ;
{
 ptype_list *ptype ;

 if(cmd == NULL) return(NULL) ;

 if(((cmd->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
    (cmd->ROOT->ROOT == ttvfig))
   return(cmd) ;

 while(cmd->ROOT->ROOT->ROOT != NULL)
  {
   if((ptype = getptype(cmd->ROOT->USER,TTV_SIG_CMDNEW)) != NULL)
     {
      if((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
        cmd = (((ttvsig_list *)ptype->DATA)->NODE + 1) ;
      else
        cmd = (((ttvsig_list *)ptype->DATA)->NODE) ;
      if(((cmd->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N)) == 0) ||
         (cmd->ROOT->ROOT == ttvfig))
        return(cmd) ;
     }
   else return(NULL) ;
  }

 return(NULL) ;
}

/*****************************************************************************/
/*                        function ttv_getlinecmd()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* line : lien ou il faut rechercher une command                             */
/* type : type de command TTV_LINE_CMDMAX ou TTV_LINE_CMDMIN                 */
/*                                                                           */
/* renvoie une command a un lien qui fini sur un latch                       */
/*****************************************************************************/
ttvevent_list *ttv_getlinecmd(ttvfig,line,type)
ttvfig_list *ttvfig ;
ttvline_list *line ;
long type ;
{
 ptype_list *ptype ;

 if((line->ROOT->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
  {
   ptype = getptype(line->USER,type) ;
   if(ptype != NULL)
    return(ttv_getcmd(ttvfig,(ttvevent_list *)ptype->DATA)) ;
  }

 return(NULL) ;
}

/*****************************************************************************/
/*                        function ttv_addcmd()                              */
/* parametres :                                                              */
/* line : lien ou il faut ajouter une command                                */
/* type : type de command TTV_LINE_CMDMAX ou TTV_LINE_CMDMIN                 */
/* cmd : noeud command                                                       */
/*                                                                           */
/* ajoute une command a un lien qui fini sur un latch                        */
/*****************************************************************************/
ptype_list *ttv_addcmd(line,type,cmd)
ttvline_list *line ;
ttvevent_list *cmd ;
long type ;
{
 if(cmd == NULL) return(NULL) ;
 line->USER = addptype(line->USER,type,(void*)ttv_getcmd(line->FIG,cmd)) ;
 return(line->USER) ;
}

/*****************************************************************************/
/*                        function ttv_getlrcmd()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig ou il faut chercher une command                           */
/* ptsig : latch ou precharge                                                */
/*                                                                           */
/* donne la liste des commandes d'un latch ou d'une precharge de la ttvfig   */
/*****************************************************************************/
chain_list *ttv_getlrcmd(ttvfig,ptsig)
ttvfig_list *ttvfig ;
ttvsig_list *ptsig ;
{
 ptype_list *ptype ;
 ttvevent_list *cmd ;
 chain_list *chain ;
 chain_list *chainres = NULL ;

 if((ptype = getptype(ptsig->USER,TTV_SIG_CMD)) != NULL)
  for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT) 
   {
    cmd = ttv_getcmd(ttvfig,(ttvevent_list *)chain->DATA) ;
    if(cmd == NULL)
      continue ;
    if((cmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
     {
      chainres = addchain(chainres,cmd) ;
      cmd->TYPE |= TTV_NODE_CMDMARQUE ;
     }
   }

  for(chain = chainres ; chain != NULL ; chain = chain->NEXT) 
   {
    ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
   }

 return(chainres) ;
}

/*****************************************************************************/
/*                        function ttv_addprevline()                         */
/* parametres :                                                              */
/* ptline : lien ou il faut ajouter une un prevline                          */
/*                                                                           */
/* ajoute un ptype sur le lien d'avant pour la liberation des liens          */
/*****************************************************************************/
void ttv_addprevline(ptline)
ttvline_list *ptline ;
{
  if(ptline->NEXT == NULL) return ;

  if((ptline->NEXT->TYPE & TTV_LINE_PREV) != TTV_LINE_PREV)
    ptline->NEXT->USER = addptype(ptline->NEXT->USER,TTV_LINE_PREVLINE,
                                  (void *)ptline) ;
  else
    getptype(ptline->NEXT->USER,TTV_LINE_PREVLINE)->DATA = (void*)ptline ;

 ptline->NEXT->TYPE |= TTV_LINE_PREV ;
}

/*****************************************************************************/
/*                        function ttv_delprevline()                         */
/* parametres :                                                              */
/* ptline : lien ou il faut suprimer une un prevline                         */
/*                                                                           */
/* suprime un ptype sur le lien d'avant pour la liberation des liens         */
/*****************************************************************************/
void ttv_delprevline(ptline)
ttvline_list *ptline ;
{
 ptline->USER = delptype(ptline->USER,TTV_LINE_PREVLINE) ;
 ptline->TYPE &= ~(TTV_LINE_PREV) ;
}


/*****************************************************************************/
/*                        function ttv_islinelevel()                         */
/* parametres :                                                              */
/* ptline : lien ou il faut suprimer une un prevline                         */
/*                                                                           */
/* suprime un ptype sur le lien d'avant pour la liberation des liens         */
/*****************************************************************************/
int ttv_islinelevel(ttvfig,ptline,level)
ttvfig_list *ttvfig ;
ttvline_list *ptline ;
long level ;
{
  if((((ptline->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
      (ptline->FIG != ttvfig)) ||
     (((ptline->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
      (ptline->FIG->INFO->LEVEL < level)) ||
     (ptline->FIG->INFO->LEVEL > ttvfig->INFO->LEVEL))
      return(0) ;
  return(1) ;
}

/*****************************************************************************/
/*                        function ttv_getttvfig()                           */
/* parametres :                                                              */
/* figname : nom de modele de la ttvfig a retrouver                          */
/*                                                                           */
/* recherche une ttvfig dans la liste des ttvfig si elle n'existe pas elle   */
/* est cree                                                                  */
/*****************************************************************************/
ttvfig_list *ttv_getttvfig(char *figname, long type)
{
 ttvfig_list *ttvfig ;
 FILE *file ;
 char filename[256], realfilename[256] ;
 char *name = namealloc(figname) ;
 char buf[1024], *c;

 ttvfig = ttv_gethtmodel(name) ;

 if (ttvfig!=NULL && ttvfig->INFO->FILENAME!=NULL)
   strcpy(realfilename,ttvfig->INFO->FILENAME);
 else
   strcpy(realfilename,figname);

 if ((c=strrchr(figname,'/'))!=NULL) strcpy(buf, c+1);
 else strcpy(buf, figname);
 if ((c=strstr(buf,".dtx"))!=NULL || (c=strstr(buf,".ttx"))!=NULL)
 {
  if (*(c+1)=='t') type|=TTV_FILE_TTX; else type|=TTV_FILE_DTX;
  *c='\0';
  name=namealloc(buf);
  strcpy(realfilename, figname);
  strcpy(figname, buf);
 }


 if((ttvfig != NULL) && 
    ((ttvfig->STATUS & (TTV_STS_C|TTV_STS_L)) == (TTV_STS_C|TTV_STS_L)) &&
    ((type & TTV_FILE_INF) != TTV_FILE_INF))
   {
    return(ttvfig) ;
   }
 else if(((file = mbkfopen(realfilename,"ttx",READ_TEXT)) != NULL) &&
    (((type & TTV_FILE_TTX) == TTV_FILE_TTX) ||
     ((type & TTV_FILE_DTX) != TTV_FILE_DTX)))
  {
   if(fclose(file) != 0)
    {
     sprintf(filename,"%s.ttx",figname) ;
     ttv_error(21,filename,TTV_WARNING) ;
    }
   if(ttvfig == NULL)
    {
     ttvfig = ttv_allocttvfig(figname,figname,NULL) ;
     if (strcmp(realfilename, figname)!=0)
       ttvfig->INFO->FILENAME=strdup(realfilename);
     ttv_addhtmodel(ttvfig) ;
    }
   ttv_parsttvfig(ttvfig,TTV_STS_C|TTV_STS_L,TTV_FILE_TTX) ;
  }
 else if(((file = mbkfopen(realfilename,"dtx",READ_TEXT)) != NULL) &&
         (((type & TTV_FILE_DTX) == TTV_FILE_DTX) ||
          ((type & TTV_FILE_TTX) != TTV_FILE_TTX)))
  {
   if(fclose(file) != 0)
    {
     sprintf(filename,"%s.dtx",figname) ;
     ttv_error(21,filename,TTV_WARNING) ;
    }
   if(ttvfig == NULL)
    {
     ttvfig = ttv_allocttvfig(figname,figname,NULL) ;
     if (strcmp(realfilename, figname)!=0)
       ttvfig->INFO->FILENAME=strdup(realfilename);
     ttv_addhtmodel(ttvfig) ;
    }
   ttv_parsttvfig(ttvfig,TTV_STS_C|TTV_STS_L,TTV_FILE_DTX) ;
  }
 else
  {
   sprintf(filename,"%s.ttx/dtx",figname) ;
   ttv_error(11,filename,TTV_WARNING) ;
  }

 if(((type & TTV_FILE_INF) == TTV_FILE_INF) && (ttvfig != NULL))
   ttv_getinffile(ttvfig) ;

 return(ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_getttvins()                           */
/* parametres :                                                              */
/* ttvfig : figure pere                                                      */
/* figname : nom de modele de la ttvins a retrouver                          */
/* figname : nom d'instance de la ttvins a retrouver                         */
/*                                                                           */
/* recherche une ttvins dans la liste des instances d'une figure             */
/*****************************************************************************/
ttvfig_list *ttv_getttvins(ttvfig,figname,insname)
ttvfig_list *ttvfig ;
char *figname ;
char *insname ;
{
 ttvfig_list *ttvins = NULL ;
 chain_list *chain = ttv_getttvfiglist(ttvfig) ;
 chain_list *chainx ;
 char buf[1024] ;

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  {
   if((strcmp(((ttvfig_list *)chainx->DATA)->INFO->FIGNAME,figname) == 0) &&
      (strcmp(ttv_getinsname(ttvfig,buf,((ttvfig_list *)chainx->DATA)),insname) == 0))
     {
      ttvins = (ttvfig_list *)chainx->DATA ;
      break ;
     }
  }

 freechain(chain) ;
 return(ttvins) ;
}


/*****************************************************************************/
/*                        function ttv_givehead()                            */
/* parametres :                                                              */
/* figname : nom de modele de la ttvfig a retrouver                          */
/*                                                                           */
/* recherche une ttvfig dans la liste des ttvfig haut niveau                 */
/* en cree une nouvelle sinon                                                */
/*****************************************************************************/
ttvfig_list *ttv_givehead(figname,insname,root)
char *figname ;
char *insname ;
ttvfig_list *root ;
{
 ttvfig_list *ttvfig ;
 chain_list *chain ;
 char *name = namealloc(figname) ;
 char *nameins = namealloc(insname) ;

 for(chain = TTV_HEAD_TTVFIG ; chain != NULL ; chain = chain->NEXT)
   {
    ttvfig = (ttvfig_list *)chain->DATA ;
    if(ttvfig->INFO->FIGNAME == name)
      {
       if(root != NULL)
        {
         ttvfig->STATUS &= ~(TTV_STS_HEAD) ;
         TTV_HEAD_TTVFIG = delchaindata(TTV_HEAD_TTVFIG,ttvfig) ;
         ttvfig->INSNAME = ttv_checkfigname(nameins) ;
         ttvfig->ROOT = root ;
         root->INS = addchain(root->INS,ttvfig) ;
        }
       return(ttvfig) ;
      }
   }


 ttvfig = ttv_allocttvfig(nameins,name,root) ;

 return(ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_givettvfig()                          */
/* parametres :                                                              */
/* figname : nom de modele de la ttvfig a retrouver                          */
/*                                                                           */
/* recherche une ttvfig dans la liste des ttvfig si c'est une instance il    */
/* en cree une nouvelle sinon il renvoie la precedante                       */
/*****************************************************************************/
ttvfig_list *ttv_givettvfig(figname,insname,root)
char *figname ;
char *insname ;
ttvfig_list *root ;
{
 ttvfig_list *ttvfig ;
 ttvfig_list *ttvfigx ;
 chain_list *chain ;
 char *name = namealloc(figname) ;
 char *nameins = namealloc(insname) ;
 
 ttvfig = ttv_gethtmodel(name) ;

 if(ttvfig != NULL)
  {
   if(((ttvfig->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD) ||
      ((ttvfig->ROOT == root) && (nameins == ttvfig->INSNAME))) 
    {
     if((root != NULL) && (ttvfig->ROOT == NULL))
      {
       ttvfig->STATUS &= ~(TTV_STS_HEAD) ;
       TTV_HEAD_TTVFIG = delchaindata(TTV_HEAD_TTVFIG,ttvfig) ;
       ttvfig->INSNAME = ttv_checkfigname(nameins) ;
       ttvfig->ROOT = root ;
       root->INS = addchain(root->INS,ttvfig) ;
      }
    }
   else
    {
     for(chain = ttvfig->INFO->INSTANCES ; chain != NULL ; chain = chain->NEXT)
      {
       ttvfigx = (ttvfig_list *)chain->DATA ;
       if(((ttvfigx->STATUS & TTV_STS_HEAD) == TTV_STS_HEAD) ||
          ((ttvfigx->ROOT == root) && (nameins == ttvfigx->INSNAME))) 
        {
         if((root != NULL) && (ttvfigx->ROOT == NULL))
          {
           ttvfigx->STATUS &= ~(TTV_STS_HEAD) ;
           TTV_HEAD_TTVFIG = delchaindata(TTV_HEAD_TTVFIG,ttvfigx) ;
           ttvfigx->INSNAME = ttv_checkfigname(nameins) ;
           ttvfigx->ROOT = root ;
           root->INS = addchain(root->INS,ttvfigx) ;
          }
         ttvfig = ttvfigx ;
         break ;
        }
      }
      if(chain == NULL)
       {
        ttvfig = ttv_allocttvfig(nameins,name,root) ;
       }
    }
  }
 else
  {
   ttvfig = ttv_allocttvfig(nameins,name,root) ;
  }

 return(ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_getttvfiglist()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig pere                                                      */
/*                                                                           */
/* renvoie la liste des figures d'un arbre hierarchique                      */
/*****************************************************************************/
chain_list *ttv_getttvfiglist(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chainfig = NULL ;
 chain_list *chain ;
 
 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   chainfig = append(ttv_getttvfiglist((ttvfig_list *)chain->DATA),chainfig) ;
  }
 return(addchain(chainfig,(void*)ttvfig)) ;
}

/*****************************************************************************/
/*                        function ttv_getinsname()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ptname : buffer ou le nom sera ecrit                                      */
/* ttvins : instance                                                         */
/*                                                                           */
/* ecrit dans le buffer ptname le nom hierarchique d'une instance            */
/*****************************************************************************/
char *ttv_getinsname(ttvfig,ptname,ttvins)
ttvfig_list *ttvfig ;
char *ptname ;
ttvfig_list *ttvins ;
{
 ttvfig_list *ttvfigx ;
 chain_list *chain = NULL ;
 chain_list *chainx ;

 if((ttvfigx = ttvins->ROOT) == NULL)
   {
    strcpy(ptname,ttvins->INSNAME) ;
    return(ptname) ;
   }

 while((ttvfigx != ttvfig) && (ttvfigx->ROOT != NULL))
 {
  chain = addchain(chain,ttvfigx->INSNAME) ;
  ttvfigx = ttvfigx->ROOT ;
 }

 *ptname = '\0' ;

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
   {
    strcat(ptname,(char *)chainx->DATA) ;
    strcat(ptname,&SEPAR) ;
   }
 
 freechain(chain) ;
 strcat(ptname,ttvins->INSNAME) ;
 return(ptname) ;
}

/*****************************************************************************/
/*                        function ttv_getsigname()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ptname : buffer ou le nom sera ecrit                                      */
/* ptsig : signal                                                            */
/*                                                                           */
/* ecrit dans le buffer ptname le nom hierarchique du signal dans la ttvfig  */
/*****************************************************************************/
char *ttv_getsigname_sub(ttvfig_list *ttvfig,char *ptname,ttvsig_list *ptsig, int retonly)
{
 ttvfig_list *ttvfigx ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 char separ[2];

 if (retonly && ptsig->ROOT->ROOT==NULL) return ptsig->NAME;

 separ[0] = SEPAR ;
 separ[1] = '\0' ;

 ttvfigx = ptsig->ROOT ;

 while((ttvfigx != ttvfig) && (ttvfigx->ROOT != NULL))
 {
  chain = addchain(chain,ttvfigx->INSNAME) ;
  ttvfigx = ttvfigx->ROOT ;
 }

 *ptname = '\0' ;

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
   {
    strcat(ptname,(char *)chainx->DATA) ;
    strcat(ptname,separ) ;
   }
 
 freechain(chain) ;
 strcat(ptname,ptsig->NAME) ;
 return(ptname) ;
}

char *ttv_getsigname(ttvfig_list *ttvfig,char *ptname,ttvsig_list *ptsig)
{
  return ttv_getsigname_sub(ttvfig,ptname,ptsig,0);
}

/*****************************************************************************/
/*                        function ttv_getnetname()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ptname : buffer ou le nom sera ecrit                                      */
/* ptsig : signal                                                            */
/*                                                                           */
/* ecrit dans le buffer ptname le nom hierarchique du signal dans la ttvfig  */
/*****************************************************************************/
char *ttv_getnetname_sub(ttvfig_list *ttvfig,char *ptname,ttvsig_list *ptsig, int retonly)
{
 ttvfig_list *ttvfigx ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 char separ[2];

 if (retonly && ptsig->ROOT->ROOT==NULL) return ptsig->NETNAME;

 separ[0] = SEPAR ;
 separ[1] = '\0' ;

 ttvfigx = ptsig->ROOT ;

 while((ttvfigx != ttvfig) && (ttvfigx->ROOT != NULL))
 {
  chain = addchain(chain,ttvfigx->INSNAME) ;
  ttvfigx = ttvfigx->ROOT ;
 }

 *ptname = '\0' ;

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
   {
    strcat(ptname,(char *)chainx->DATA) ;
    strcat(ptname,separ) ;
   }
 
 freechain(chain) ;
 strcat(ptname,ptsig->NETNAME) ;
 return(ptname) ;
}

char *ttv_getnetname(ttvfig_list *ttvfig,char *ptname,ttvsig_list *ptsig)
{
  return ttv_getnetname_sub(ttvfig,ptname,ptsig,0);
}

/*****************************************************************************/
/*                        function ttv_getsig()                              */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* name : nom du signal                                                      */
/*                                                                           */
/* recherche un signal dans une ttvfig grace a son nom hierachique           */
/*****************************************************************************/
ttvsig_list *ttv_getsig(ttvfig,name)
ttvfig_list *ttvfig ;
char *name ;
{
 ttvsig_list *ptsig = NULL ;
 ttvfig_list *ttvins ;
 char *signame ;
 chain_list *chain ;

 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_C)) != NULL)
    return(ptsig) ;
 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_N)) != NULL)
    return(ptsig) ;
 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_Q)) != NULL)
    return(ptsig) ;
 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_L)) != NULL)
    return(ptsig) ;
 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_R)) != NULL)
    return(ptsig) ;
 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_I)) != NULL)
    return(ptsig) ;
 if((ptsig = ttv_getsigbyname(ttvfig,name,TTV_SIG_B)) != NULL)
    return(ptsig) ;
 if((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S)
   ttv_parsttvfig(ttvfig,TTV_STS_S,TTV_FILE_DTX) ;
 ttvins = ttvfig ;
 while((ptsig = ttv_getsigbyname(ttvins,name,TTV_SIG_S)) == NULL)
  {
   if(ttvins->INS != NULL)
    {
     signame = name ;
     while((*name != SEPAR) && (*name != '\0'))
       name ++;
     if(*name != '\0')
      {
       *name = '\0' ;
       for(chain = ttvins->INS ; chain != NULL ; chain = chain->NEXT)
          if(strcmp(((ttvfig_list *)chain->DATA)->INSNAME,signame) == 0)
            break ;
       *name = SEPAR ;
       name++ ;
       if(chain != NULL)
         ttvins = (ttvfig_list *)chain->DATA ;
       else
        break ;
      }
     else
       break ;
    }
   else
    break ;
   
   if((ttvins->STATUS & TTV_STS_S) != TTV_STS_S)
     ttv_parsttvfig(ttvins,TTV_STS_S,TTV_FILE_DTX) ;
  }
 return(ptsig) ;
}

/*****************************************************************************/
/*                        function ttv_getsigbyhash()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* name : nom du signal                                                      */
/*                                                                           */
/* recherche un signal dans une ttvfig grace a son nom hierachique           */
/*****************************************************************************/
ttvsig_list *ttv_getsigbyhash(ttvfig,name)
ttvfig_list *ttvfig ;
char *name ;
{
 ttvsig_list *ptsig = NULL ;
 ttvfig_list *ttvins ;
 ptype_list *ptype ;
 char *signame ;
 chain_list *chain ;

 if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) == NULL)
  {
   ttv_builthtabttvfig(ttvfig,TTV_STS_L) ;
   if(((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) == NULL) &&
      (getptype(ttvfig->USER,TTV_STS_HTAB_S) == NULL))
     return(NULL) ;
  }

 if(ptype != NULL)
 if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,name)) != EMPTYHT)
   return(ptsig) ;

 if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) == NULL)
  {
   if((ttvfig->STATUS & TTV_STS_S) != TTV_STS_S)
      ttv_parsttvfig(ttvfig,TTV_STS_S,TTV_FILE_DTX) ;
   ttv_builthtabttvfig(ttvfig,TTV_STS_S) ;
   if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) == NULL)
     return(NULL) ;
  }

 if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,name)) != EMPTYHT)
   return(ptsig) ;

 if(ttvfig->INS != NULL)
  {
   signame = name ;
   while((*name != SEPAR) && (*name != '\0'))
     name ++;
   if(*name != '\0')
    {
     *name = '\0' ;
     for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
        if(strcmp(((ttvfig_list *)chain->DATA)->INSNAME,signame) == 0)
          break ;
     *name = SEPAR ;
     name++ ;
     if(chain != NULL)
      {
       ttvins = (ttvfig_list *)chain->DATA ;
       return(ttv_getsigbyhash(ttvins,namealloc(name))) ;
      }
    }
  }

 return(NULL) ;
}

/*****************************************************************************/
/*                        function ttv_getsigbyname()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* name : nom du signal                                                      */
/* type : type de signal                                                     */
/*                                                                           */
/* recherche un signal dans une ttvfig grace a son nom hierachique           */
/* la fonction est recursive s'il n'existe pas elle cherche dans le fils     */
/*****************************************************************************/
ttvsig_list *ttv_getsigbyname(ttvfig,name,type)
ttvfig_list *ttvfig ;
char *name ;
long type ;
{
 ttvsig_list *ptsig = NULL ;
 static long i,j ;
 static char signame[1024] ;
 static ttvsbloc_list *ptsbloc ; 
 static long nbfree ;
 static long nbend ;
 static char *signamex ;
 static ptype_list *ptype ;
 static chain_list *chain ;

 switch(type & TTV_SIG_TYPEALL)
  {
   case TTV_SIG_C :
   if((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
     {
      ptsig = *(ttvfig->CONSIG + i) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
    return(ptsig) ;
   }
   break ;
   case TTV_SIG_N :
   if((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
     {
      ptsig = *(ttvfig->NCSIG + i) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
    return(ptsig) ;
   }
   break ;
   case TTV_SIG_Q :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
     {
      ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
              : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_L :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
     {
      ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
      : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_R :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
     {
      ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
              : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_I :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
     {
      ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
              : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_B :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
     {
      ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
              : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
      if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_S :
   if((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
     {
      for(i = 0 ; i < ttvfig->NBESIG ; i++)
       {
        ptsig = *(ttvfig->ESIG + i) ;
        if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
          break ;
        else
          ptsig = NULL ;
       }
      if(((type & TTV_SIG_S) == TTV_SIG_S) && 
         ((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S) && (ptsig == NULL))
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
            if(mbk_casestrcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
              break ;
            else
              ptsig = NULL ;
           }
         }
       }
     }
   }
   break ;
  }

 if(ptsig != NULL) return(ptsig) ;
 else 
   {
    if(ttvfig->INS == NULL) return(NULL) ;
    signamex = name ;
    while((*name != SEPAR) && (*name != '\0'))
      name ++;
    if(*name == '\0') return(NULL) ;
    *name = '\0' ;
    for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
       if(mbk_casestrcmp(((ttvfig_list *)chain->DATA)->INSNAME,signamex) == 0)
         break ;
    *name = SEPAR ;
    name++ ;
    if(chain == NULL) return(NULL) ;
    return(ttv_getsigbyname((ttvfig_list *)chain->DATA,name,type)) ;
   }
}

/*****************************************************************************/
/*                        function ttv_getsigbyinsname()                     */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* name : nom du signal                                                      */
/* type : type de signal                                                     */
/*                                                                           */
/* recherche un signal dans une ttvfig grace a son nom hierachique           */
/* la fonction est recursive elle cherche dans les fils d'abord              */
/*****************************************************************************/
ttvsig_list *ttv_getsigbyinsname(ttvfig,name,type)
ttvfig_list *ttvfig ;
char *name ;
long type ;
{
 char *signamex ;
 ttvsig_list *ptsig = NULL ;
 static long i,j ;
 static char signame[1024] ;
 static ttvsbloc_list *ptsbloc ; 
 static long nbfree ;
 static long nbend ;
 static ptype_list *ptype ;
 static chain_list *chain ;

 switch(type & TTV_SIG_TYPEALL)
  {
   case TTV_SIG_C :
   if((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
     {
      ptsig = *(ttvfig->CONSIG + i) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
    return(ptsig) ;
   }
   break ;
   case TTV_SIG_N :
   if((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
     {
      ptsig = *(ttvfig->NCSIG + i) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
    return(ptsig) ;
   }
   break ;
 }

 if(ttvfig->INS != NULL)
  {
   signamex = name ;
   while((*name != SEPAR) && (*name != '\0'))
     name ++;
   if(*name != '\0') 
    {
     *name = '\0' ;
     for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
        if(strcmp(((ttvfig_list *)chain->DATA)->INSNAME,signamex) == 0)
          break ;
     *name = SEPAR ;
     name++ ;
     if(chain != NULL) 
     if((ptsig = ttv_getsigbyinsname((ttvfig_list *)chain->DATA,name,
                                      type|TTV_SIG_MARQUE)) != NULL)
       return(ptsig) ;
    }
   name = signamex ;
  }

 switch(type & TTV_SIG_TYPEALL)
  {
   case TTV_SIG_Q :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
     {
      ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
              : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_L :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
     {
      ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
              : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_R :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
     {
      ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
              : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_I :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
     {
      ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
              : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_B :
   if((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_L)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
    for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
     {
      ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
              : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
      if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
        break ;
      else
        ptsig = NULL ;
     }
   }
   break ;
   case TTV_SIG_S :
   if((ttvfig->STATUS & TTV_STS_SE) == TTV_STS_SE)
   {
    if((ptype = getptype(ttvfig->USER,TTV_STS_HTAB_S)) != NULL)
     {
      if((long)(ptsig = (ttvsig_list *)gethtitem((ht *)ptype->DATA,
                        (void*)namealloc(name))) == EMPTYHT)
       ptsig = NULL ;
     }
    else
     {
      for(i = 0 ; i < ttvfig->NBESIG ; i++)
       {
        ptsig = *(ttvfig->ESIG + i) ;
        if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
          break ;
        else
          ptsig = NULL ;
       }
      if(((type & TTV_SIG_S) == TTV_SIG_S) &&
         ((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S) && (ptsig == NULL))
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
            if(strcmp(ttv_getsigname(ttvfig,signame,ptsig),name) == 0)
              break ;
            else
              ptsig = NULL ;
           }
         }
       }
     }
   }
  }

 if((ptsig != NULL) && ((type & TTV_SIG_MARQUE) == TTV_SIG_MARQUE))
   {
    if(((ptsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
       ((ptsig->TYPE & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B)) != 0))
      ptsig = NULL ;
   }
 
 return(ptsig) ;
}

/*****************************************************************************/
/*                        function ttv_getsigbyindex()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* name : index du signal                                                    */
/* type : type de signal                                                     */
/*                                                                           */
/* recherche un signal dans une ttvfig grace a son index qui correspond a    */
/* sa position dans le tableau des signaux du type                           */
/*****************************************************************************/
ttvsig_list *ttv_getsigbyindex(ttvfig,index,type)
ttvfig_list *ttvfig ;
long index ;
long type ;
{
 if((type & TTV_SIG_C) == TTV_SIG_C)
   {
    if(index > ttvfig->NBCONSIG) return (NULL) ;
    return(*(ttvfig->CONSIG + (index - 1L))) ;
   }
 else if((type & TTV_SIG_Q) == TTV_SIG_Q)
   {
    if(index > ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG) return (NULL) ;
    return(((index -1L) < ttvfig->NBELCMDSIG) ? 
    *(ttvfig->ELCMDSIG + (index - 1L)) :
    *(ttvfig->ILCMDSIG + (index - 1L) - ttvfig->NBELCMDSIG)) ;
   }
 else if((type & TTV_SIG_L) == TTV_SIG_L)
   {
    if(index > ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG) return (NULL) ;
    return(((index -1L) < ttvfig->NBELATCHSIG) ? 
    *(ttvfig->ELATCHSIG + (index - 1L)) :
    *(ttvfig->ILATCHSIG + (index - 1L) - ttvfig->NBELATCHSIG)) ;
   }
 else if((type & TTV_SIG_R) == TTV_SIG_R)
   {
    if(index > ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) return (NULL) ;
    return(((index -1L) < ttvfig->NBEPRESIG) ? 
    *(ttvfig->EPRESIG + (index - 1L)) :
    *(ttvfig->IPRESIG + (index - 1L) - ttvfig->NBEPRESIG)) ;
   }
 else if((type & TTV_SIG_B) == TTV_SIG_B)
   {
    if(index > ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) return (NULL) ;
    return(((index -1L) < ttvfig->NBEBREAKSIG) ? 
    *(ttvfig->EBREAKSIG + (index - 1L)) :
    *(ttvfig->IBREAKSIG + (index - 1L) - ttvfig->NBEBREAKSIG)) ;
   }
 else if((type & TTV_SIG_I) == TTV_SIG_I)
   {
    if(index > ttvfig->NBEXTSIG + ttvfig->NBINTSIG) return (NULL) ;
    return(((index -1L) < ttvfig->NBEXTSIG) ? 
    *(ttvfig->EXTSIG + (index - 1L)) :
    *(ttvfig->INTSIG + (index - 1L) - ttvfig->NBEXTSIG)) ;
   }
 else if((type & TTV_SIG_N) == TTV_SIG_N)
   {
    if(index > ttvfig->NBNCSIG) return (NULL) ;
    return(*(ttvfig->NCSIG + (index - 1L))) ;
   }
 else if((type & TTV_SIG_S) == TTV_SIG_S)
   {
    if(index <= ttvfig->NBESIG) return(*(ttvfig->ESIG + (index - 1L))) ;
    index = index - ttvfig->NBESIG ;
    if(index > ttvfig->NBISIG) return(NULL) ;
    index = ttvfig->NBISIG - (index - 1L) ;
    if(ttvfig->NBISIG != 0)
     {
      long i ;
      ttvsbloc_list *ptsbloc = ttvfig->ISIG ;
      ttvsbloc_list **tabbloc = ttv_addsigtab(ttvfig) ;

      long nbloc = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC)) 
                    % TTV_MAX_SBLOC ;

      nbloc = (index + nbloc - 1L) ;
      index = nbloc % TTV_MAX_SBLOC ;
      nbloc = nbloc / TTV_MAX_SBLOC ;

      if(tabbloc != NULL)
       {
        ptsbloc = *(tabbloc + nbloc) ;
       }
      else
       {
        for(i = 0L ; i < nbloc ; i++)
         {
          ptsbloc = ptsbloc->NEXT ;
         }
       }
      return(ptsbloc->SIG + index) ;
     }
    else return(NULL) ; 
   }

   return(NULL);
}

/*****************************************************************************/
/*                        function ttv_getsigbytype()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ttvins : ttvins courante                                                  */
/* type : type des signaux                                                   */
/* mask : mask de selection                                                  */
/*                                                                           */
/* recherche tous les signaux d'une ttvfig qui sont de type "type"           */
/* renvoie la liste de ces signaux                                           */
/*****************************************************************************/

chain_list *ttv_getsigbytype_sub(ttvfig_list *ttvfig,ttvfig_list *ttvins,long type,chain_list *mask, testname_func testmask)
{
 ttvfig_list *ttvfigx ;
 ttvsig_list *ptsig ;
 chain_list *chain = NULL ;
 ttvsbloc_list *ptsbloc ;
 long nbfree ;
 long nbend ;
 long i,j ;

 if(ttvfig == NULL) return(NULL) ;

 if(ttvins == NULL) 
     return(ttv_getallsigbytype_sub(ttvfig,type,mask,testmask)) ;

 for(ttvfigx = ttvins ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
  if(ttvfigx == ttvfig) break ;

 if(ttvfigx == NULL) return(NULL) ;

 if(((type & TTV_SIG_C) == TTV_SIG_C) && 
    ((ttvins->STATUS & TTV_STS_C) == TTV_STS_C) && (ttvfig == ttvins))
   {
    for(i = 0 ; i < ttvins->NBCONSIG ; i++)
     {
      if((mask == NULL) || 
         (testmask(ttvfig,(*(ttvins->CONSIG + i)),mask) == 1))
      chain = addchain(chain,(void *)(*(ttvins->CONSIG + i))) ;
     }
   }
 if(((type & TTV_SIG_N) == TTV_SIG_N) &&
    ((ttvins->STATUS & TTV_STS_C) == TTV_STS_C) && (ttvfig == ttvins))
   {
    for(i = 0 ; i < ttvins->NBNCSIG ; i++)
     {
      if((mask == NULL) || 
         (testmask(ttvfig,(*(ttvins->NCSIG + i)),mask) == 1))
      chain = addchain(chain,(void *)(*(ttvins->NCSIG + i))) ;
     }
   }
 if(((type & TTV_SIG_Q) == TTV_SIG_Q) &&
         ((ttvins->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvins->NBELCMDSIG + ttvins->NBILCMDSIG ; i++)
     {
      ptsig = (i < ttvins->NBELCMDSIG) ? *(ttvins->ELCMDSIG + i)
              : *(ttvins->ILCMDSIG + i - ttvins->NBELCMDSIG) ;
      if((((type & TTV_SIG_C) != TTV_SIG_C) || 
          ((ptsig->TYPE & TTV_SIG_C) != TTV_SIG_C)) &&
         (((type & TTV_SIG_N) != TTV_SIG_N) || 
          ((ptsig->TYPE & TTV_SIG_N) != TTV_SIG_N)))
      if((mask == NULL) || 
         (testmask(ttvfig,ptsig,mask) == 1))
      chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_L) == TTV_SIG_L) &&
         ((ttvins->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvins->NBELATCHSIG + ttvins->NBILATCHSIG ; i++)
     {
      ptsig = (i < ttvins->NBELATCHSIG) ? *(ttvins->ELATCHSIG + i)
              : *(ttvins->ILATCHSIG + i - ttvins->NBELATCHSIG) ;
      if((((type & TTV_SIG_C) != TTV_SIG_C) || 
          ((ptsig->TYPE & TTV_SIG_C) != TTV_SIG_C)) &&
         (((type & TTV_SIG_N) != TTV_SIG_N) || 
          ((ptsig->TYPE & TTV_SIG_N) != TTV_SIG_N)))
      if((mask == NULL) || 
         (testmask(ttvfig,ptsig,mask) == 1))
      chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_R) == TTV_SIG_R) &&
         ((ttvins->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvins->NBEPRESIG + ttvins->NBIPRESIG ; i++)
     {
      ptsig = (i < ttvins->NBEPRESIG) ? *(ttvins->EPRESIG + i)
              : *(ttvins->IPRESIG + i - ttvins->NBEPRESIG) ;
      if((((type & TTV_SIG_C) != TTV_SIG_C) || 
          ((ptsig->TYPE & TTV_SIG_C) != TTV_SIG_C)) &&
         (((type & TTV_SIG_N) != TTV_SIG_N) || 
          ((ptsig->TYPE & TTV_SIG_N) != TTV_SIG_N)))
      if((mask == NULL) || 
         (testmask(ttvfig,ptsig,mask) == 1))
      chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_I) == TTV_SIG_I) &&
         ((ttvins->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvins->NBEXTSIG + ttvins->NBINTSIG ; i++)
     {
      ptsig = (i < ttvins->NBEXTSIG) ? *(ttvins->EXTSIG + i)
              : *(ttvins->INTSIG + i - ttvins->NBEXTSIG) ;
      if((mask == NULL) || 
         (testmask(ttvfig,ptsig,mask) == 1))
      chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_B) == TTV_SIG_B) &&
         ((ttvins->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvins->NBEBREAKSIG + ttvins->NBIBREAKSIG ; i++)
     {
      ptsig = (i < ttvins->NBEBREAKSIG) ? *(ttvins->EBREAKSIG + i)
              : *(ttvins->IBREAKSIG + i - ttvins->NBEBREAKSIG) ;
      if((((type & TTV_SIG_C) != TTV_SIG_C) || 
          ((ptsig->TYPE & TTV_SIG_C) != TTV_SIG_C)) &&
         (((type & TTV_SIG_N) != TTV_SIG_N) || 
          ((ptsig->TYPE & TTV_SIG_N) != TTV_SIG_N)))
      if((mask == NULL) || 
         (testmask(ttvfig,ptsig,mask) == 1))
      chain = addchain(chain,(void *)ptsig) ;
     }
   }

 if(((type & TTV_SIG_S) == TTV_SIG_S) &&
    ((ttvins->STATUS & TTV_STS_S) == TTV_STS_S))
   {
    for(i = 0 ; i < ttvins->NBESIG ; i++)
      {
       ptsig = *(ttvins->ESIG + i) ;
       if((mask == NULL) ||
           (testmask(ttvfig,ptsig,mask) == 1))
        {
         chain = addchain(chain,(void *)ptsig) ;
        }
      }

    if(ttvins->NBISIG != 0)
      {
       ptsbloc = ttvins->ISIG ;
       nbfree = (TTV_MAX_SBLOC - (ttvins->NBISIG % TTV_MAX_SBLOC))
                 % TTV_MAX_SBLOC ;
       nbend = ttvins->NBISIG + nbfree ;
       for(i = nbfree ; i < nbend ; i++)
        {
         j = i % TTV_MAX_SBLOC ;
         if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
         ptsig = ptsbloc->SIG + j ;
         if((mask == NULL) ||
            (testmask(ttvfig,ptsig,mask) == 1))
          {
           chain = addchain(chain,(void *)ptsig) ;
          }
        }
      }
   }

 return(chain) ;
}

chain_list *ttv_getsigbytype(ttvfig_list *ttvfig,ttvfig_list *ttvins,long type,chain_list *mask)
{
   return ttv_getsigbytype_sub(ttvfig,ttvins,type,mask,ttv_testmask);
}

chain_list *ttv_getsigbytype_and_netname(ttvfig_list *ttvfig,ttvfig_list *ttvins,long type,chain_list *mask)
{
   if (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_USE_NODE_NAME_ONLY)
     return ttv_getsigbytype_sub(ttvfig,ttvins,type,mask,ttv_testmask);
   return ttv_getsigbytype_sub(ttvfig,ttvins,type,mask,ttv_testnetnamemask);
}

/*****************************************************************************/
/*                        function ttv_getsignamelist()                      */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ttvins : ttvfig instance                                                  */
/* mask : mask de selection                                                  */
/*                                                                           */
/* recherche tous les signaux d'une ttvfig nom cnqlri                        */
/* renvoie la liste de noms des signaux a ne liberer qu'avec la fonction     */
/* ttv_freesignamelist()                                                     */
/*****************************************************************************/
chain_list *ttv_getsignamelist(ttvfig,ttvins,mask)
ttvfig_list *ttvfig ;
ttvfig_list *ttvins ;
chain_list *mask ;
{
 ttvfig_list *ttvfigx ;
 char bufname[1024] ;
 char *name ;
 chain_list *chain = NULL ;
 ttvsbloc_list *ptsbloc ;
 long nbfree ;
 long nbend ;
 long i,j ;

 if(ttvfig == NULL) return(NULL) ;

 if(ttvins == NULL) 
   ttvins = ttvfig ;

 for(ttvfigx = ttvins ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
  if(ttvfigx == ttvfig) break ;
 
 if(ttvfigx == NULL) return(NULL) ;

 if(ttvfig == ttvins)
   ttv_parsttvfig(ttvins,TTV_STS_C|TTV_STS_L|TTV_STS_S,TTV_FILE_DTX) ;
 else
   ttv_parsttvfig(ttvins,TTV_STS_L|TTV_STS_S,TTV_FILE_DTX) ;

 for(i = 0 ; i < ttvins->NBESIG ; i++)
   {
    if(((mask == NULL) || 
        (ttv_testmask(ttvfig,(*(ttvins->ESIG + i)),mask) == 1)) &&
       ((*(ttvins->ESIG + i))->ROOT == ttvins))
     {
      ttv_getsigname(ttvfig,bufname,*(ttvins->ESIG + i)) ;
      name = mbkalloc(strlen(bufname) + 1) ;
      chain = addchain(chain,(void *)strcpy(name,bufname)) ;
     }
   }

 if(ttvins->NBISIG != 0)
   {
    ptsbloc = ttvins->ISIG ;
    nbfree = (TTV_MAX_SBLOC - (ttvins->NBISIG % TTV_MAX_SBLOC)) 
              % TTV_MAX_SBLOC ;
    nbend = ttvins->NBISIG + nbfree ;
    for(i = nbfree ; i < nbend ; i++)
     {
      j = i % TTV_MAX_SBLOC ;
      if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
      if((mask == NULL) || 
         (ttv_testmask(ttvfig,(ptsbloc->SIG + j),mask) == 1))
       {
        ttv_getsigname(ttvfig,bufname,ptsbloc->SIG + j) ;
        name = mbkalloc(strlen(bufname) + 1) ;
        chain = addchain(chain,(void *)strcpy(name,bufname)) ;
       }
     }
   }

 for(i = 0 ; i < ttvins->NBINTSIG ; i++)
  {
   if(((mask == NULL) || 
      (ttv_testmask(ttvfig,(*(ttvins->INTSIG + i)),mask) == 1)) &&
       ((*(ttvins->INTSIG + i))->ROOT == ttvins))
    {
     ttv_getsigname(ttvfig,bufname,*(ttvins->INTSIG + i)) ;
     name = mbkalloc(strlen(bufname) + 1) ;
     chain = addchain(chain,(void *)strcpy(name,bufname)) ;
    }
  }

 for(i = 0 ; i < ttvins->NBEXTSIG ; i++)
  {
   if(((mask == NULL) ||
      (ttv_testmask(ttvfig,(*(ttvins->EXTSIG + i)),mask) == 1)) &&
       ((*(ttvins->EXTSIG + i))->ROOT == ttvins))
    {
     ttv_getsigname(ttvfig,bufname,*(ttvins->EXTSIG + i)) ;
     name = mbkalloc(strlen(bufname) + 1) ;
     chain = addchain(chain,(void *)strcpy(name,bufname)) ;
    }
  }

 if(ttvfig == ttvins)
  {
   for(i = 0 ; i < ttvins->NBNCSIG ; i++)
    {
     if((mask == NULL) || 
        (ttv_testmask(ttvfig,(*(ttvins->NCSIG + i)),mask) == 1))
      {
       ttv_getsigname(ttvfig,bufname,*(ttvins->NCSIG + i)) ;
       name = mbkalloc(strlen(bufname) + 1) ;
       chain = addchain(chain,(void *)strcpy(name,bufname)) ;
      }
    }
  }


 return(chain) ;
}

/*****************************************************************************/
/*                        function ttv_freenamelist()                        */
/* parametres :                                                              */
/* chainname : liste de nom                                                  */
/*                                                                           */
/* libere une liste de nom nom allouee par namealloc                         */
/*****************************************************************************/
void ttv_freenamelist(chainname)
chain_list *chainname ;
{
 chain_list *chain = NULL ;
 
 for(chain = chainname ; chain != NULL ; chain = chain->NEXT)
   mbkfree(chain->DATA) ;

 freechain(chainname) ;
}
 

/*****************************************************************************/
/*                        function ttv_getallsigbytype()                     */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* type : type des signaux                                                   */
/* mask : mask de selection                                                  */
/*                                                                           */
/* recherche tous les signaux d'une ttvfig et de ses fils qui sont de type   */
/* "type"                                                                    */
/* renvoie la liste de ces signaux                                           */
/*****************************************************************************/
chain_list *ttv_getallsigbytype_sub(ttvfig_list *ttvfig,long type,chain_list *mask, testname_func testmask)
{
 chain_list *chain = NULL ;
 chain_list *chainx ;
 ttvsbloc_list *ptsbloc ;
 long nbfree ;
 long nbend ;
 long j ;
 static chain_list **chaintab ;
 static ttvfig_list *ttvfigr ;
 static ttvsig_list *ptsig ;
 static long i ;
 static long level ;

 if((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)
  {
   ttvfigr = ttvfig ;
   chaintab = (chain_list **)mbkalloc(ttvfig->INFO->LEVEL * sizeof(chain_list*)) ;
   for(i = 0 ; i < ttvfigr->INFO->LEVEL ; i++)
     *(chaintab + i) = NULL ;
  }

 if(((type & TTV_SIG_C) == TTV_SIG_C) && 
    ((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C))
   {
    for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
     {
      if(((*(ttvfig->CONSIG + i))->ROOT == ttvfig) &&
         ((mask == NULL) || 
          (testmask(ttvfigr,(*(ttvfig->CONSIG + i)),mask) == 1)))
        chain = addchain(chain,(void *)(*(ttvfig->CONSIG + i))) ;
     }
   }

 if(((type & TTV_SIG_N) == TTV_SIG_N) &&
         ((ttvfig->STATUS & TTV_STS_C) == TTV_STS_C))
   {
    for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
     {
      if(((*(ttvfig->NCSIG + i))->ROOT == ttvfig) &&
         ((mask == NULL) || 
          (testmask(ttvfigr,(*(ttvfig->NCSIG + i)),mask) == 1)))
        chain = addchain(chain,(void *)(*(ttvfig->NCSIG + i))) ;
     }
   }

 if(((type & TTV_SIG_Q) == TTV_SIG_Q) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBELCMDSIG ; i++)
     {
      ptsig = *(ttvfig->ELCMDSIG +i) ;
      if((ptsig->ROOT == ttvfig) &&
         (((type & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_MARQUE)) == 0) ||
          (((ptsig->TYPE&(TTV_SIG_C|TTV_SIG_N)&type) == 0) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          (((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          ((ptsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)) &&
         ((mask == NULL) || 
         (testmask(ttvfigr,ptsig,mask) == 1)))
       {
        if((level = ttv_getsiglevel(ptsig)) > ttvfigr->INFO->LEVEL)
          level = ttvfigr->INFO->LEVEL ;
        level-- ;
        *(chaintab + level) = addchain(*(chaintab + level),(void *)ptsig) ;
       }
     }
   }

 if(((type & TTV_SIG_L) == TTV_SIG_L) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBELATCHSIG ; i++)
     {
      ptsig = *(ttvfig->ELATCHSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         (((type & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_MARQUE)) == 0) ||
          (((ptsig->TYPE&(TTV_SIG_C|TTV_SIG_N)&type) == 0) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          (((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          ((ptsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)) &&
         ((mask == NULL) || 
         (testmask(ttvfigr,ptsig,mask) == 1)))
       {
        if((level = ttv_getsiglevel(ptsig)) > ttvfigr->INFO->LEVEL)
          level = ttvfigr->INFO->LEVEL ;
        level-- ;
        *(chaintab + level) = addchain(*(chaintab + level),(void *)ptsig) ;
       }
     }
   }

 if(((type & TTV_SIG_R) == TTV_SIG_R) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBEPRESIG ; i++)
     {
      ptsig = *(ttvfig->EPRESIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         (((type & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_MARQUE)) == 0) ||
          (((ptsig->TYPE&(TTV_SIG_C|TTV_SIG_N)&type) == 0) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          (((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          ((ptsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)) &&
         ((mask == NULL) || 
         (testmask(ttvfigr,ptsig,mask) == 1)))
       {
        if((level = ttv_getsiglevel(ptsig)) > ttvfigr->INFO->LEVEL)
          level = ttvfigr->INFO->LEVEL ;
        level-- ;
        *(chaintab + level) = addchain(*(chaintab + level),(void *)ptsig) ;
       }
     }
   }

 if(((type & TTV_SIG_I) == TTV_SIG_I) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBEXTSIG ; i++)
     {
      ptsig = *(ttvfig->EXTSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
       {
        if((level = ttv_getsiglevel(ptsig)) > ttvfigr->INFO->LEVEL)
          level = ttvfigr->INFO->LEVEL ;
        level-- ;
        *(chaintab + level) = addchain(*(chaintab + level),(void *)ptsig) ;
       }
     }
   }

 if(((type & TTV_SIG_B) == TTV_SIG_B) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBEBREAKSIG ; i++)
     {
      ptsig = *(ttvfig->EBREAKSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         (((type & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_MARQUE)) == 0) ||
          (((ptsig->TYPE&(TTV_SIG_C|TTV_SIG_N)&type) == 0) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          (((ttvfig->STATUS & TTV_STS_C) != TTV_STS_C) &&
           ((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)) ||
          ((ptsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)) &&
         ((mask == NULL) || 
         (testmask(ttvfigr,ptsig,mask) == 1)))
       {
        if((level = ttv_getsiglevel(ptsig)) > ttvfigr->INFO->LEVEL)
          level = ttvfigr->INFO->LEVEL ;
        level-- ;
        *(chaintab + level) = addchain(*(chaintab + level),(void *)ptsig) ;
       }
     }
   }

  if(((type & TTV_SIG_S) == TTV_SIG_S) &&
    ((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S))
   {
    for(i = 0 ; i < ttvfig->NBESIG ; i++)
     {
      ptsig = *(ttvfig->ESIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
       {
        if((level = ttv_getsiglevel(ptsig)) > ttvfigr->INFO->LEVEL)
          level = ttvfigr->INFO->LEVEL ;
        level-- ;
        *(chaintab + level) = addchain(*(chaintab + level),(void *)ptsig) ;
       }
     }
   }

 for(i = ttvfigr->INFO->LEVEL - (long)1 ; i >= (long)0 ; i--)
   {
    chain = append(*(chaintab + i),chain) ;
    *(chaintab + i) = NULL ;
   }


 if(((type & TTV_SIG_Q) == TTV_SIG_Q) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBILCMDSIG ; i++)
     {
      ptsig = *(ttvfig->ILCMDSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
        chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_L) == TTV_SIG_L) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBILATCHSIG ; i++)
     {
      ptsig = *(ttvfig->ILATCHSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
        chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_R) == TTV_SIG_R) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBIPRESIG ; i++)
     {
      ptsig = *(ttvfig->IPRESIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
        chain = addchain(chain,(void *)ptsig) ;
     }
   }
 if(((type & TTV_SIG_I) == TTV_SIG_I) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBINTSIG ; i++)
     {
      ptsig = *(ttvfig->INTSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
        chain = addchain(chain,ptsig) ;
     }
   }
 if(((type & TTV_SIG_B) == TTV_SIG_B) &&
         ((ttvfig->STATUS & TTV_STS_L) == TTV_STS_L))
   {
    for(i = 0 ; i < ttvfig->NBIBREAKSIG ; i++)
     {
      ptsig = *(ttvfig->IBREAKSIG + i) ;
      if((ptsig->ROOT == ttvfig) &&
         ((mask == NULL) ||
          (testmask(ttvfigr,ptsig,mask) == 1)))
        chain = addchain(chain,(void *)ptsig) ;
     }
   }

  if(((type & TTV_SIG_S) == TTV_SIG_S) &&
    ((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S))
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
         if((mask == NULL) ||
            (testmask(ttvfigr,ptsig,mask) == 1))
          {
           chain = addchain(chain,(void *)ptsig) ;
          }
        }
      }
   }

 if((type & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_I|TTV_SIG_B|TTV_SIG_S)) != 0)
 for(chainx = ttvfig->INS ; chainx != NULL ; chainx = chainx->NEXT)
  {
   chain = append(ttv_getallsigbytype_sub((ttvfig_list *)chainx->DATA,
                  (type&~(TTV_SIG_C|TTV_SIG_N))|TTV_SIG_MARQUE,mask,testmask),chain) ;
  }

 if((type & TTV_SIG_MARQUE) != TTV_SIG_MARQUE)
   mbkfree(chaintab) ;

 return(chain) ;
}

chain_list *ttv_getallsigbytype(ttvfig_list *ttvfig,long type,chain_list *mask)
{
  return ttv_getallsigbytype_sub(ttvfig,type,mask,ttv_testmask);
}

/*****************************************************************************/
/*                        function ttv_setsiglevel()                         */
/* ptsig : signal                                                            */
/* level : niveau a ajouter                                                  */
/*                                                                           */
/* positionne un niveau hierarchique d'un signal                             */
/*****************************************************************************/
void ttv_setsiglevel(ptsig,level)
ttvsig_list *ptsig ;
long level ;
{
 ptsig->TYPE &= ~(TTV_SIG_LEVEL) ;

 if(level >  (TTV_SIG_LEVEL >> 16))
   ptsig->TYPE |= TTV_SIG_LEVEL ;
 else
   ptsig->TYPE |= ((level << 16) & TTV_SIG_LEVEL) ;
}

/*****************************************************************************/
/*                        function ttv_getsiglevel()                         */
/* ptsig : signal                                                            */
/*                                                                           */
/* renvoie le niveau jusqu'ou se trouve le signal                            */
/*****************************************************************************/
long ttv_getsiglevel(ptsig)
ttvsig_list *ptsig ;
{
 ttvfig_list *ttvfig ;
 ttvfig_list *ttvfigx ;
 long type ;
 long i ;
 char flag ;
 long level ;

 if((level = ((ptsig->TYPE & TTV_SIG_LEVEL) >> 16 )) < (TTV_SIG_LEVEL >> 16))
    return(level) ; 

 type = ptsig->TYPE ;
 ttvfig = ptsig->ROOT->ROOT ;
 ttvfigx = ptsig->ROOT ;
 flag = 'N' ;

 while(ttvfig != NULL)
  {
   if((type & TTV_SIG_C) == TTV_SIG_C)
     {
      flag = 'N' ;
      break ;
     }
   else if((type & TTV_SIG_N) == TTV_SIG_N)
     {
      flag = 'N' ;
      break ;
     }
   else if((type & TTV_SIG_Q) == TTV_SIG_Q)
     {
      for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
       {
        if(((i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
         : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG)) == ptsig)
          {
           flag = 'Y' ;
           break ;
          }
       }
     }
   else if((type & TTV_SIG_L) == TTV_SIG_L)
     {
      for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
       {
        if(((i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
        : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG)) == ptsig)
          {
           flag = 'Y' ;
           break ;
          }
       }
     }
   else if((type & TTV_SIG_R) == TTV_SIG_R)
     {
      for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
       {
        if(((i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
        : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG)) == ptsig)
          {
           flag = 'Y' ;
           break ;
          }
       }
     }
   else if((type & TTV_SIG_I) == TTV_SIG_I)
     {
      for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
       {
        if(((i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
        : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG)) == ptsig)
          {
           flag = 'Y' ;
           break ;
          }
       }
     }
   else if((type & TTV_SIG_B) == TTV_SIG_B)
     {
      for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
       {
        if(((i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
        : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG)) == ptsig)
          {
           flag = 'Y' ;
           break ;
          }
       }
     }
   else if(((type & TTV_SIG_S) == TTV_SIG_S) &&
           ((ttvfig->STATUS & TTV_STS_S) == TTV_STS_S))
     {
      for(i = 0 ; i < ttvfig->NBESIG ; i++)
       {
        if((*(ttvfig->ESIG + i)) == ptsig)
          {
           flag = 'Y' ;
           break ;
          }
       }
     }
   if(flag == 'Y')
      flag = 'N' ;
   else break ;
   ttvfigx = ttvfig ;
   ttvfig = ttvfig->ROOT ;
  }

 return(ttvfigx->INFO->LEVEL) ;
}

/*****************************************************************************/
/*                        function ttv_delsiglevel()                         */
/* ttvfig : figure ou l'on supprime le signal                                */
/* ptsig : signal                                                            */
/*                                                                           */
/* decremente le niveau hierarchique d'un signal du niveau de la ttvfig      */
/*****************************************************************************/
void ttv_delsiglevel(ttvfig,ptsig)
ttvfig_list *ttvfig ;
ttvsig_list *ptsig ;
{
 ttvfig_list *ttvfigx ;

 if(ttvfig->INFO->LEVEL >  ttv_getsiglevel(ptsig))
   return ;
 
 ttvfigx = ptsig->ROOT ; 
 
 if(ttvfigx == ttvfig)
   {
    ptsig->TYPE &= ~(TTV_SIG_LEVEL) ;
    return ;
   }

 while(ttvfigx != NULL)
  {
   if(ttvfigx->ROOT == ttvfig) break ;
   ttvfigx = ttvfigx->ROOT ;
  }
 
 if(ttvfigx != NULL)
  ttv_setsiglevel(ptsig,ttvfigx->INFO->LEVEL) ;
}

/*****************************************************************************/
/*                        function ttv_expfigsig()                           */
/* ttvfig : figure                                                           */
/* ptsig : signal                                                            */
/* leveld : niveau bas                                                       */
/* levelu : niveau haut                                                      */
/* status : etat                                                             */
/* file :  type de fichier                                                   */
/*                                                                           */
/* expanse les figures peres d'un signal dans un etat particulier            */
/*****************************************************************************/
void ttv_expfigsig(ttvfig,ptsig,leveld,levelu,status,file)
ttvfig_list *ttvfig ;
ttvsig_list *ptsig ;
long leveld ;
long levelu ;
long status ;
long file ;
{
 ttvfig_list *ttvfigx ;
 long level = ttv_getsiglevel(ptsig) ;
 chain_list *chainfig = NULL ;
 chain_list *chain ;

 ttvfigx = ptsig->ROOT ;

 while(ttvfigx != NULL)
  {
   if((ttvfigx->INFO->LEVEL > levelu) || (ttvfigx->INFO->LEVEL > level))
     break ;
   if((ttvfigx->INFO->LEVEL < leveld) && (ttvfigx != ttvfig))
    {
     ttv_parsttvfig(ttvfigx,status & ~(TTV_STS_C|TTV_STS_D|
                                       TTV_STS_T|TTV_STS_F|TTV_STS_P|
                                       TTV_STS_DUAL_D|TTV_STS_DUAL_T|
                                       TTV_STS_DUAL_F|TTV_STS_DUAL_P),file) ;
    }
   else
    {
     if(ttvfigx == ttvfig)
       ttv_parsttvfig(ttvfigx,status,file) ;
     else
       ttv_parsttvfig(ttvfigx,status & ~(TTV_STS_C|TTV_STS_D|TTV_STS_T|
                                         TTV_STS_DUAL_D|TTV_STS_DUAL_T),file) ;
    }
   if((ttvfigx->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE)
    {
     ttvfigx->STATUS |= TTV_STS_NOT_FREE ;
     chainfig = addchain(chainfig,ttvfigx) ;
    }
   if(ttvfigx == ttvfig)
       break ;
   ttvfigx = ttvfigx->ROOT ;
  }

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
   {
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_NOT_FREE) ;
   }
 freechain(chainfig) ;
}

/*****************************************************************************/
/*                        function ttv_openfile()                            */
/* ttvfig : ttvfig                                                           */
/* type : type de fichier                                                    */
/*                                                                           */
/* ouvre un fichier en lecture ou ecritue                                    */
/*****************************************************************************/
long ttv_getloadedfigtypes(ttvfig_list *tvf)
{
  ptype_list *pt;
  long res=0;
  if ((pt=getptype(tvf->USER, TTV_FIG_TYPE))!=NULL) res=(long)pt->DATA;
  else
  {
    res=ttv_existefile(tvf, TTV_FILE_DTX|TTV_FILE_TTX|TTV_FILE_CTX);
    tvf->USER=addptype(tvf->USER, TTV_FIG_TYPE, (void *)res);
  }
  if (tvf->NBPBLOC!=0 || tvf->NBJBLOC!=0 || tvf->NBTBLOC!=0) res|=TTV_FILE_TTX;
  if (tvf->NBFBLOC!=0 || tvf->NBEBLOC!=0 || tvf->NBDBLOC!=0) res|=TTV_FILE_DTX;
  
  return res;
}

FILE *ttv_openfile(ttvfig,type,mode)
ttvfig_list *ttvfig ;
long type ;
char *mode ;
{
 FILE *file ;
 char *name = ttvfig->INFO->FIGNAME, *ext ;
 int noext=0;
 ptype_list *pt;

 if (ttvfig->INFO->FILENAME!=NULL &&
      (strcmp(mode,WRITE_TEXT)==0 || (type & TTV_FILE_DTX)==TTV_FILE_DTX)) name=ttvfig->INFO->FILENAME, noext=1;

 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   if (noext) ext=NULL; else ext="ttx";
   file = mbkfopen(name, ext ,mode) ;
  }
 else if((type & TTV_FILE_CTX) == TTV_FILE_CTX)
  {
   if (noext) ext=NULL; else ext="ctx";
   file = mbkfopen(name, ext ,mode) ;
  }
 else if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   if (noext) ext=NULL; else ext="dtx";
   file = mbkfopen(name, ext ,mode) ;
  }
 else if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
  {
   if (noext) ext=NULL; else ext="ttv";
   file = mbkfopen(name, ext ,mode) ;
  }
 else if((type & TTV_FILE_DTV) == TTV_FILE_DTV)
  {
   if (noext) ext=NULL; else ext="dtv";
   file = mbkfopen(name, ext,mode) ;
  }
 else return NULL ;

 if ((pt=getptype(ttvfig->USER, TTV_FIG_TYPE))==NULL)
   ttvfig->USER=pt=addptype(ttvfig->USER, TTV_FIG_TYPE, 0);

 pt->DATA=(void *)(((long)pt->DATA)|type);
 
 return(file) ;
}

/*****************************************************************************/
/*                        function ttv_existefile()                          */
/* name : nom de la figure                                                   */
/* type : type de fichier                                                    */
/*                                                                           */
/* renvoie les types de fichier qui existe                                   */
/*****************************************************************************/
long ttv_existefile(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 FILE *file ;
 char *name = ttvfig->INFO->FIGNAME ;
 char filename[256] ;

 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
    if((file = ttv_openfile(ttvfig,TTV_FILE_TTX,READ_TEXT)) != NULL)
       {
        if(fclose(file) != 0)
         {
          sprintf(filename,"%s.ttx",name) ;
          ttv_error(21,filename,TTV_WARNING) ;
         }
       }
     else
       type &= ~(TTV_FILE_TTX) ;
  }
 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
    if((file = ttv_openfile(ttvfig,TTV_FILE_DTX,READ_TEXT)) != NULL)
       {
        if(fclose(file) != 0)
         {
          sprintf(filename,"%s.dtx",name) ;
          ttv_error(21,filename,TTV_WARNING) ;
         }
       }
     else
       type &= ~(TTV_FILE_DTX) ;
  }
 if((type & TTV_FILE_CTX) == TTV_FILE_CTX)
  {
    if((file = ttv_openfile(ttvfig,TTV_FILE_CTX,READ_TEXT)) != NULL)
       {
        if(fclose(file) != 0)
         {
          sprintf(filename,"%s.ctx",name) ;
          ttv_error(21,filename,TTV_WARNING) ;
         }
       }
     else
       type &= ~(TTV_FILE_CTX) ;
  }

 return(type) ;
}

/*****************************************************************************/
/*                        function ttv_addlooplist()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* chainloop : liste des boucles                                             */
/*                                                                           */
/* tranforme les noeuds en nom et elimine les double boucles                 */
/*****************************************************************************/
chain_list *ttv_addlooplist(ttvfig,chainloop)
ttvfig_list *ttvfig ;
chain_list *chainloop ;
{
/* ttvsig_list *ptsig ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainl ;
 char *name ;
 char bufname[1024] ;*/

 if(chainloop == NULL) return(NULL) ;

/* for(chain = chainloop ; chain->NEXT !=NULL ; chain = chain->NEXT)
  {
   if(chain->DATA == NULL) continue ;

   for(chainx = (chain_list *)chain->DATA ; chainx != NULL ; 
       chainx = chainx->NEXT)
     ((ttvevent_list *)chainx->DATA)->ROOT->TYPE |= TTV_SIG_MARQUE ;

   for(chainl = chain->NEXT ; chainl != NULL ; chainl = chainl->NEXT)
    {
     if(chainl->DATA == NULL) continue ;

     for(chainx = (chain_list *)chainl->DATA ; chainx != NULL ; 
         chainx = chainx->NEXT)
     if((((ttvevent_list *)chainx->DATA)->ROOT->TYPE & TTV_SIG_MARQUE) != 
          TTV_SIG_MARQUE) break ;
     if(chainx == NULL)
      {
       freechain((chain_list *)chainl->DATA) ;
       chainl->DATA = NULL ;
      }
    }
   
   for(chainx = (chain_list *)chain->DATA ; chainx != NULL ; 
       chainx = chainx->NEXT)
     ((ttvevent_list *)chainx->DATA)->ROOT->TYPE &= ~(TTV_SIG_MARQUE) ;
  }

 chain = chainloop ;

 chainloop = NULL ;
 chainl = NULL ;

 while(chain != NULL)
  {
   chainx = chain->NEXT ;
   if(chain->DATA == NULL)
    {
     chain->NEXT = chainl ;
     chainl = chain ;
    }
   else
    {
     chain->NEXT = chainloop ;
     chainloop = chain ;
    }
   chain = chainx ;
  }

 freechain(chainl) ;
*/
 if(chainloop != NULL)
   ttv_error(51,ttvfig->INFO->FIGNAME,TTV_WARNING) ;

/* for(chain = chainloop ; chain !=NULL ; chain = chain->NEXT)
  {
   for(chainx = (chain_list *)chain->DATA ; chainx != NULL ; 
       chainx = chainx->NEXT)
    {
     ptsig = (ttvsig_list *)chainx->DATA ;
     ttv_getsigname(ttvfig,bufname,ptsig) ;
     name = mbkalloc(strlen(bufname) + 1) ;
     chainx->DATA = (void *)strcpy(name,bufname) ;
    }
  }*/
 
 return(chainloop) ;
}

/*****************************************************************************/
/*                        function ttv_addloop()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* node : noeud courant                                                      */
/*                                                                           */
/* construit les boucles et recupere le lien a detruire                      */
/*****************************************************************************/
chain_list *ttv_addloop(ttvfig,node)
ttvfig_list *ttvfig ;
ttvevent_list *node ;
{
 ttvevent_list *nodex ;
 ttvline_list *ptline = NULL ;
 chain_list *chain = NULL ;

 chain = addchain(chain,(void*)node) ;
 nodex = node->FIND->OUTLINE->ROOT ;
 if(node->FIND->OUTLINE->FIG == ttvfig)
  ptline = node->FIND->OUTLINE ;
 while(nodex != node)
  {
   chain = addchain(chain,(void*)nodex) ;
   if((nodex->FIND->OUTLINE->FIG == ttvfig) && (ptline == NULL))
    ptline = nodex->FIND->OUTLINE ;
   nodex = nodex->FIND->OUTLINE->ROOT ;
  }

 if(ptline != NULL)
  {
   if((ptline->TYPE & TTV_LINE_LOOP) == TTV_LINE_LOOP)
     chain = addchain(chain,(void*)NULL) ;
   else
    {
     ptline->TYPE |= TTV_LINE_LOOP ;
     chain = addchain(chain,(void*)ptline) ;
    }
  }
 else
 {
  chain = addchain(chain,(void*)ptline) ;
 }

 return(chain) ;
}

/*****************************************************************************/
/*                        function ttv_marknode()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* node : noeud courant                                                      */
/* type : type de recherche                                                  */
/* mark : type de marquage                                                   */
/*                                                                           */
/* marque les noeuds deja traités                                            */
/*****************************************************************************/
void ttv_marknode(ttvfig,node,type,mark)
ttvfig_list *ttvfig ;
ttvevent_list *node ;
long type ;
long mark ;
{
 static ptype_list *ptype ;
 ttvline_list *in ;
 chain_list *chain ;

 if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
  {
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     ptype = getptype(node->USER,TTV_NODE_DUALPATH) ;
   else
     ptype = getptype(node->USER,TTV_NODE_DUALLINE) ;

   if(ptype == NULL) in = NULL ;
   else
    {
     chain = (chain_list *)ptype->DATA ;
     if(chain == NULL) in = NULL ;
     else in = (ttvline_list *)chain->DATA ;
    }
  }
 else
  {
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     in =  node->INPATH ;
   else
     in =  node->INLINE ;
  }

while(in != NULL)
  {
   if((((in->TYPE & (TTV_LINE_D|TTV_LINE_F|TTV_LINE_T|TTV_LINE_P)) != 0) &&
      (ttvfig != in->FIG)) || ((in->TYPE & TTV_LINE_LOOP) == TTV_LINE_LOOP))
    {
     if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
       {
        chain = chain->NEXT ;
        if(chain == NULL) in = NULL ;
        else in = (ttvline_list *)chain->DATA ;
       }
     else in = in->NEXT ;
     continue ;
    }

   if((in->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
    {
     if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
       {
        chain = chain->NEXT ;
        if(chain == NULL) in = NULL ;
        else in = (ttvline_list *)chain->DATA ;
       }
     else in = in->NEXT ;
     continue ;
    }

   if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      {
       if((in->TYPE & TTV_LINE_USEDDUAL) != TTV_LINE_USEDDUAL)
          return ;
      }
   else
      {
       if((in->TYPE & TTV_LINE_USED) != TTV_LINE_USED)
          return ;
      }

   if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      {
       chain = chain->NEXT ;
       if(chain == NULL) in = NULL ;
       else in = (ttvline_list *)chain->DATA ;
      }
   else in = in->NEXT ;
  }

 if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
     node->TYPE |= TTV_NODE_USEDDUAL ;
    }
   else
    {
     node->TYPE |= TTV_NODE_USED ;
    }
#ifndef __ALL__WARNING_
   mark = 0;
#endif
}

/*****************************************************************************/
/*                        function ttv_depthmark()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* node : noeud courant                                                      */
/* type : type de recherche                                                  */
/* mark : type de marquage                                                   */
/*                                                                           */
/* parcours en profondeur du graphe                                          */
/*****************************************************************************/
chain_list *ttv_depthmark(ttvfig,node,type,mark)
ttvfig_list *ttvfig ;
ttvevent_list *node ;
long type ;
long mark ;
{
 static ttvfig_list *ttvfigx ;
 static ptype_list *ptype ;
 ttvevent_list *nodex ;
 ttvline_list *inx ;
 ttvline_list *in ;
 chain_list *chain ;
 chain_list *chainres = NULL ;

 ttv_fifopush(node) ;

 if(((mark & TTV_SIG_EXT) == TTV_SIG_EXT) &&
    ((node->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_I|TTV_SIG_B)) != 0))
  {
   node->ROOT->TYPE |= TTV_SIG_EXT ;
  }

 if(((mark & TTV_NODE_LINEEXT) == TTV_NODE_LINEEXT) && (node->FIND->OUTLINE != NULL))
  {
   in = node->FIND->OUTLINE ;
   if((in->FIG == ttvfig) && ((in->TYPE & TTV_LINE_F) == TTV_LINE_F))
     in->TYPE |= TTV_LINE_EXT ;
   if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
     if((node->TYPE & TTV_NODE_USEDDUAL) == TTV_NODE_USEDDUAL)
      {
       return(NULL) ;
      }
    }
  }

 if((mark & TTV_NODE_LOOP) == TTV_NODE_LOOP)
  {
   if(((node->TYPE & TTV_NODE_LOOP) == TTV_NODE_LOOP) &&
      ((node->ROOT->TYPE & TTV_SIG_L) != TTV_SIG_L))
     
     {
      chainres = addchain(chainres,ttv_addloop(ttvfig,node)) ;
      node->TYPE |= TTV_NODE_LOOPNODE ;
      return(chainres) ;
     }

   if((node->TYPE & TTV_NODE_USED) == TTV_NODE_USED)
     {
      return(NULL) ;
     }
   node->TYPE |= TTV_NODE_LOOP ;
  }

 if(((node->TYPE & TTV_NODE_STOP) == TTV_NODE_STOP) &&
    ((((((node->ROOT->TYPE & (TTV_SIG_CB|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_CB) &&
     ((node->ROOT->TYPE & (TTV_SIG_CT|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_CT)) ||
     (ttv_islocononlyend(ttvfig,node,type) != 0)) &&
     ((node->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_N) &&
     ((node->ROOT->TYPE & TTV_SIG_B) != TTV_SIG_B)) &&
     ((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_Q)) != TTV_SIG_Q)))
    {
     if((type & TTV_FIND_MARQUE) == TTV_FIND_MARQUE)
      {
       node->TYPE &= ~(TTV_NODE_LOOP) ;
       return(chainres) ;
      }
     else
      type |= TTV_FIND_MARQUE ;
    }
 else
    {
     type |= TTV_FIND_MARQUE ;
    }

 if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
  {
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     ptype = getptype(node->USER,TTV_NODE_DUALPATH) ;
   else
     ptype = getptype(node->USER,TTV_NODE_DUALLINE) ;

   if(ptype == NULL) in = NULL ;
   else
    {
     chain = (chain_list *)ptype->DATA ;
     if(chain == NULL) in = NULL ;
     else in = (ttvline_list *)chain->DATA ;
    }
  }
 else
  {
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     in =  node->INPATH ;
   else
     in =  node->INLINE ;
  }
 
 while(in != NULL)
  {
   if(((in->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
      (((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
      ((in->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
      (ttv_islineonlyend(ttvfig,in,type) == 1)))
     {
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
       {
        chain = chain->NEXT ;
        if(chain == NULL) in = NULL ;
        else in = (ttvline_list *)chain->DATA ;
        continue ;
       }
      else
       {
        in = in->NEXT ;
        continue ;
       }
     }
   if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
     nodex = in->ROOT ;

     if(((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                (node->FIND->OUTLINE != NULL))
      {
       if((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
         {
          chain = chain->NEXT ;
          if(chain == NULL) in = NULL ;
          else in = (ttvline_list *)chain->DATA ;
          continue ;
         }
      }
    }
   else
    {
     nodex = in->NODE ;
     if(((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
        (node->FIND->OUTLINE != NULL))
      {
       if((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
        {
         in = in->NEXT ;
         continue ;
        }
      }
    }
 
   if((((in->TYPE & (TTV_LINE_D|TTV_LINE_F|TTV_LINE_T|TTV_LINE_P)) != 0) && 
      (ttvfig != in->FIG)) || ((in->TYPE & TTV_LINE_LOOP) == TTV_LINE_LOOP))
    {
     if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
       {
        chain = chain->NEXT ;
        if(chain == NULL) in = NULL ;
        else in = (ttvline_list *)chain->DATA ;
       }
     else in = in->NEXT ;
     continue ;
    }
 
   if(node->ROOT->ROOT != nodex->ROOT->ROOT)
    {
     ttvfigx = nodex->ROOT->ROOT ;
     while(ttvfigx != ttvfig)
      {
       if((ttvfigx->STATUS & TTV_STS_NOT_FREE) == TTV_STS_NOT_FREE)
        {
         if(((type & TTV_FIND_DUAL) != TTV_FIND_DUAL) ||
            (((type & TTV_FIND_PATH) == TTV_FIND_PATH) &&
            ((ttvfigx->STATUS & TTV_STS_DUAL_J) == TTV_STS_DUAL_J)) ||
            (((type & TTV_FIND_LINE) == TTV_FIND_LINE) &&
            ((ttvfigx->STATUS & TTV_STS_DUAL_E) == TTV_STS_DUAL_E)))
          break ;
        }
       ttvfigx->STATUS |= TTV_STS_NOT_FREE ;
       if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
        {
         if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
           ttv_parsttvfig(ttvfigx,TTV_STS_L_J|TTV_STS_DUAL_J,TTV_FILE_TTX) ;
         else
           ttv_parsttvfig(ttvfigx,TTV_STS_L_J,TTV_FILE_TTX) ;
        }
       else
        {
         if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
           ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_E|TTV_STS_DUAL_E,
                          TTV_FILE_DTX) ;
         else
           ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_E,TTV_FILE_DTX) ;
        }
       ttvfigx = ttvfigx->ROOT ;
      }
    }

   ttv_fifopush(nodex) ;

   inx = nodex->FIND->OUTLINE ;
   nodex->FIND->OUTLINE = in ;
   chainres = append(ttv_depthmark(ttvfig,nodex,type,mark),chainres) ;
   if((nodex->TYPE & TTV_NODE_LOOPNODE) != TTV_NODE_LOOPNODE)
     nodex->FIND->OUTLINE = NULL ;
   else
    {
     nodex->FIND->OUTLINE = inx ;
     nodex->TYPE &= ~(TTV_NODE_LOOPNODE) ;
    }

   if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      {
       in->TYPE |= TTV_LINE_USEDDUAL ;
       chain = chain->NEXT ;
       if(chain == NULL) in = NULL ;
       else in = (ttvline_list *)chain->DATA ;
      }
   else
      {
       in->TYPE |= TTV_LINE_USED ;
       in = in->NEXT ;
      }
  }

 if((mark & TTV_NODE_LOOP) == TTV_NODE_LOOP)
  {
   node->TYPE &= ~(TTV_NODE_LOOP) ;
   ttv_marknode(ttvfig,node,type,mark) ;
  }

 if(((mark & TTV_NODE_LINEEXT) == TTV_NODE_LINEEXT) &&
    ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL))
   ttv_marknode(ttvfig,node,type,mark) ;

 return(chainres) ;
}

/*****************************************************************************/
/*                        function ttv_detectloop()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/*                                                                           */
/* detect les boucles combinatoires dans le graphe et les liens externes     */
/* et renvoie la liste des boucles                                           */
/*****************************************************************************/
chain_list *ttv_detectloop(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 ttvsig_list *ptsig ;
 ttvline_list *ptline ;
 ttvline_list *ptlinenext ;
 ttvevent_list *ptnode ;
 ttvlbloc_list *ptlbloc ;
 ttvsbloc_list *ptsbloc ;
 ptype_list *ptype ;
 chain_list *chainsig ;
 chain_list *chainloop = NULL ;
 chain_list *chainnode = NULL ;
 chain_list *chainroot = NULL ;
 chain_list *chain ;
 chain_list *chainx ;
 long mark ;
 long dualtype ;
 long j ;
 long i ;

 if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
  {
   chainsig = (chain_list *)ttvfig->CONSIG ;
   ttv_parsttvfig(ttvfig,TTV_STS_P|TTV_STS_J|TTV_STS_T|TTV_STS_DUAL_P|
                                  TTV_STS_DUAL_J|TTV_STS_DUAL_T,TTV_FILE_TTX) ;
  }
 else
  {
   chainsig = (chain_list *)ttvfig->CONSIG ;
   ttv_parsttvfig(ttvfig,TTV_STS_F|TTV_STS_E|TTV_STS_D|TTV_STS_DUAL_F|
                                  TTV_STS_DUAL_E|TTV_STS_DUAL_D,TTV_FILE_DTX) ;
  }
 
 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;

   if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
    {
     if((ptsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO)
       continue ;
    }

   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    {
     mark = TTV_NODE_LOOP | TTV_SIG_EXT ;
    }
   else
    {
     mark = TTV_NODE_LOOP | TTV_NODE_LINEEXT | TTV_SIG_EXT ;
    }

   chainloop = append(ttv_depthmark(ttvfig,ptsig->NODE,type,mark)
                         ,chainloop) ;
   chainloop = append(ttv_depthmark(ttvfig,ptsig->NODE+1,type,mark)
                         ,chainloop) ;
  }

 chainsig = (chain_list *)ttvfig->NCSIG ;
 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     mark = TTV_NODE_LOOP | TTV_SIG_EXT ;
   else
     mark = TTV_NODE_LOOP | TTV_NODE_LINEEXT | TTV_SIG_EXT ;
    chainloop = append(ttv_depthmark(ttvfig,ptsig->NODE,type,
                       mark),chainloop) ;
    chainloop = append(ttv_depthmark(ttvfig,ptsig->NODE+1,type,
                       mark),chainloop) ;
  }

 chainsig = NULL ;

 for(chain = (chain_list *)ttvfig->ELCMDSIG ; chain != NULL ; 
     chain = chain->NEXT)
   chainsig = addchain(chainsig,chain->DATA) ;

 for(chain = (chain_list *)ttvfig->ELATCHSIG ; chain != NULL ; 
     chain = chain->NEXT)
   chainsig = addchain(chainsig,chain->DATA) ;

 for(chain = (chain_list *)ttvfig->EPRESIG ; chain != NULL ; 
     chain = chain->NEXT)
   chainsig = addchain(chainsig,chain->DATA) ;

 for(chain = (chain_list *)ttvfig->EBREAKSIG ; chain != NULL ; 
     chain = chain->NEXT)
   chainsig = addchain(chainsig,chain->DATA) ;

 for(chain = (chain_list *)ttvfig->EXTSIG ; chain != NULL ; 
     chain = chain->NEXT)
   chainsig = addchain(chainsig,chain->DATA) ;

 if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
   {
    for(chain = (chain_list *)ttvfig->ESIG ; chain != NULL ; 
        chain = chain->NEXT)
      chainsig = addchain(chainsig,chain->DATA) ;

    if(ttvfig->NBISIG != 0)
      {
       ttvfig->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvfig->ISIG) ;
       ptsbloc = ttvfig->ISIG ;
       for(i = 0 ; i < ttvfig->NBISIG ; i++)
         { 
          j = (TTV_MAX_SBLOC -1) - (i % TTV_MAX_SBLOC) ;
          if((j == (TTV_MAX_SBLOC - 1)) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
          chainsig = addchain(chainsig,ptsbloc->SIG + j) ;
         }
       ttvfig->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvfig->ISIG) ;
      }
   }

 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   chainloop = append(ttv_depthmark(ttvfig,ptsig->NODE,type,TTV_NODE_LOOP)
                         ,chainloop) ;
   chainloop = append(ttv_depthmark(ttvfig,ptsig->NODE+1,type,TTV_NODE_LOOP)
                         ,chainloop) ;
  }

 freechain(chainsig) ;

 chainsig = (chain_list *)ttvfig->NCSIG ;
 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     mark = TTV_SIG_EXT ;
   else
     mark = TTV_NODE_LINEEXT | TTV_SIG_EXT ;
   ttv_depthmark(ttvfig,ptsig->NODE,type|TTV_FIND_DUAL,mark) ;
   ttv_depthmark(ttvfig,ptsig->NODE+1,type|TTV_FIND_DUAL,mark) ;
  }
 chainsig = (chain_list *)ttvfig->CONSIG ;
 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
     mark = TTV_SIG_EXT ;
   else
     mark = TTV_NODE_LINEEXT | TTV_SIG_EXT ;
   if((ptsig->TYPE & TTV_SIG_CI) != TTV_SIG_CI) continue ;
   ttv_depthmark(ttvfig,ptsig->NODE,type|TTV_FIND_DUAL,mark) ;
   ttv_depthmark(ttvfig,ptsig->NODE+1,type|TTV_FIND_DUAL,mark) ;
  }

 if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
   for(ptlbloc = ttvfig->FBLOC ; ptlbloc != NULL ; ptlbloc = ptlbloc->NEXT)
    {
     for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
      {
       ptline = ptlbloc->LINE + i ;
       if((ptline->TYPE & (TTV_LINE_EXT|TTV_LINE_LOOP|TTV_LINE_FR)) 
                          == TTV_LINE_EXT) 
        {
         ptline = ttv_addline(ttvfig,ptline->ROOT,ptline->NODE,ptline->VALMAX,
                        ptline->FMAX,ptline->VALMIN,ptline->FMIN,TTV_LINE_E) ;
         ttv_addcaracline(ptline,(ptlbloc->LINE + i)->MDMAX,
                                 (ptlbloc->LINE + i)->MDMIN,
                                 (ptlbloc->LINE + i)->MFMAX,
                                 (ptlbloc->LINE + i)->MFMIN);
         ptline->TYPE |= ((ptlbloc->LINE + i)->TYPE
                         & (TTV_LINE_HZ|TTV_LINE_EV|TTV_LINE_PR|
                            TTV_LINE_R|TTV_LINE_S|TTV_LINE_RC|
                             TTV_LINE_A|TTV_LINE_U|TTV_LINE_O)) ;
         if((ptype = getptype((ptlbloc->LINE + i)->USER,
                              TTV_LINE_CMDMAX))!= NULL)
         ttv_addcmd(ptline,TTV_LINE_CMDMAX,(ttvevent_list *)ptype->DATA) ;
         if((ptype = getptype((ptlbloc->LINE + i)->USER,
                              TTV_LINE_CMDMIN))!= NULL)
         ttv_addcmd(ptline,TTV_LINE_CMDMIN,(ttvevent_list *)ptype->DATA) ;
         (ptlbloc->LINE + i)->USER = addptype((ptlbloc->LINE + i)->USER,TTV_LINE_NEW,ptline) ;
         if(((ptlbloc->LINE + i)->ROOT->TYPE & TTV_NODE_ROOT) != TTV_NODE_ROOT)
          {
           chainroot = addchain(chainroot,(ptlbloc->LINE + i)->ROOT) ;
           (ptlbloc->LINE + i)->ROOT->TYPE |= TTV_NODE_ROOT ;
          }
         if(((ptlbloc->LINE + i)->NODE->TYPE & TTV_NODE_NODE) != TTV_NODE_NODE)
          {
           chainnode = addchain(chainnode,(ptlbloc->LINE + i)->NODE) ;
           (ptlbloc->LINE + i)->NODE->TYPE |= TTV_NODE_NODE ;
          }
        }
      }
    }
   for(chainx = chainnode ; chainx != NULL ; chainx = chainx->NEXT)
     {
      ptnode = (ttvevent_list *)chainx->DATA ;
      ptnode->TYPE &= ~(TTV_NODE_NODE) ;
      if((ptype = getptype(ptnode->USER,TTV_NODE_DUALLINE))!= NULL)
         {
          for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
              chain = chain->NEXT)
             {
              ptline = (ttvline_list *)chain->DATA ;
              if((ptype = getptype(ptline->USER,TTV_LINE_NEW))!= NULL)
                {
                 chain->DATA = ptype->DATA ;
                }
             }
         }
     }
   freechain(chainnode) ;
   dualtype = ttvfig->STATUS & TTV_STS_DUAL_F ;
   ttvfig->STATUS &= ~(TTV_STS_DUAL_F) ;
   for(chainx = chainroot ; chainx != NULL ; chainx = chainx->NEXT)
     {
      ptnode = (ttvevent_list *)chainx->DATA ;
      ptnode->TYPE &= ~(TTV_NODE_ROOT) ;
      for(ptline = ptnode->INLINE ; ptline != NULL ; ptline = ptlinenext)
        {
         ptlinenext = ptline->NEXT ;
         if((ptype = getptype(ptline->USER,TTV_LINE_NEW))!= NULL)
           {
            ptline->USER = delptype(ptline->USER,TTV_LINE_NEW) ;
            ttv_delline(ptline) ;
           }
         }
     }
   ttvfig->STATUS |= dualtype ;
   freechain(chainroot) ;
   for(ptlbloc = ttvfig->DBLOC ; ptlbloc != NULL ; ptlbloc = ptlbloc->NEXT)
    {
     for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
      {
       ptline = ptlbloc->LINE + i ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
          continue ;
       if((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
        {
         if(((ptline->NODE->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) &&
            ((ptline->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) == 0))
          {
           ptline->ROOT->ROOT->TYPE |= TTV_SIG_EXT ;
          }
         if(((ptline->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) &&
            ((ptline->NODE->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) == 0))
          {
           ptline->NODE->ROOT->TYPE |= TTV_SIG_EXT ;
          }
        }
      }
    }
  }
 else
  {
   for(ptlbloc = ttvfig->TBLOC ; ptlbloc != NULL ; ptlbloc = ptlbloc->NEXT)
    {
     for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
      {
       ptline = ptlbloc->LINE + i ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
          continue ;
       if((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
        {
         if(((ptline->NODE->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) &&
            ((ptline->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) == 0))
          {
           ptline->ROOT->ROOT->TYPE |= TTV_SIG_EXT ;
          }
         if(((ptline->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) &&
            ((ptline->NODE->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) == 0))
          {
           ptline->NODE->ROOT->TYPE |= TTV_SIG_EXT ;
          }
        }
      }
    }
  }
 
 if(chainloop != NULL)
  {
   for(chain = chainloop ; chain != NULL ; chain = chain->NEXT)
    {
     chainsig = (chain_list *)chain->DATA ;
     chain->DATA = (void*)chainsig->NEXT ;
     chainsig->NEXT = NULL ;
     if(chainsig->DATA != NULL)
       ttv_delline((ttvline_list *)chainsig->DATA) ;
     freechain(chainsig) ;
    }
  }

 while((ptnode = ttv_fifopop()) != NULL)
  {
   ptnode->TYPE &= ~(TTV_NODE_LOOP|TTV_NODE_USED|
                     TTV_NODE_USEDDUAL|TTV_NODE_LINEEXT|TTV_NODE_LOOPNODE) ;
  }

 ttv_fifoclean() ;

 return(chainloop) ;
}

/*****************************************************************************/
/*                        function ttv_builtrefsig()                         */
/* parametres :                                                              */
/* ttvfig : figure ttv                                                       */
/*                                                                           */
/* construit les signaux d'une ttvfig                                        */
/*****************************************************************************/
ttvfig_list *ttv_builtrefsig(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chain ;
 chain_list *chainsig ;
 
 ttvfig->NBCONSIG = 0 ;
 for(chain = (chain_list *)ttvfig->CONSIG ; chain != NULL ; 
     chain = chain->NEXT)
   {
    ttvfig->NBCONSIG ++ ;
   }

 ttvfig->CONSIG = ttv_allocreflist((chain_list *)ttvfig->CONSIG,ttvfig->NBCONSIG) ;

 ttvfig->NBNCSIG = 0 ;
 for(chain = (chain_list *)ttvfig->NCSIG ; chain != NULL ; 
     chain = chain->NEXT)
   {
    ttvfig->NBNCSIG ++ ;
   }

 ttvfig->NCSIG = ttv_allocreflist((chain_list *)ttvfig->NCSIG,ttvfig->NBNCSIG) ;

 ttvfig->NBESIG = 0 ;
 for(chain = (chain_list *)ttvfig->ESIG ; chain != NULL ; 
     chain = chain->NEXT)
   {
    ttvfig->NBESIG ++ ;
   }

 ttvfig->ESIG = ttv_allocreflist((chain_list *)ttvfig->ESIG,ttvfig->NBESIG) ;

 for(chain = (chain_list *)ttvfig->ELATCHSIG ; chain != NULL ; 
     chain = chain->NEXT)
  {
   ttvsig_list *ptsig ;
   ptype_list *ptype ;
   chain_list *chainl ;

   ptsig = (ttvsig_list *)chain->DATA ;
   ptype = getptype(ptsig->USER,TTV_SIG_CMD) ;
   if(ptype == NULL) continue ;
   for(chainl = (chain_list *)ptype->DATA ; chainl != NULL ;
       chainl = chainl->NEXT)
     ((ttvevent_list *)chainl->DATA)->TYPE |= TTV_NODE_CMDMARQUE ;
   for(chainl = (chain_list *)ptype->DATA ; chainl != NULL ;
       chainl = chainl->NEXT)
    {
     ttvevent_list *cmd ;

     cmd = ttv_getcmd(ttvfig,(ttvevent_list *)chainl->DATA) ;
     if(cmd == NULL) continue ;
     if((ptsig->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
      {
       cmd->ROOT->TYPE |= TTV_SIG_EXT ;
      }
     if(((cmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE) &&
       (ttv_getsiglevel(cmd->ROOT) == ttvfig->INFO->LEVEL))
      {
       ptype->DATA = (void*)addchain((chain_list *)ptype->DATA,(void*)cmd) ;
       cmd->TYPE |= TTV_NODE_CMDMARQUE ;
      }
     }
   for(chainl = (chain_list *)ptype->DATA ; chainl != NULL ;
       chainl = chainl->NEXT)
      ((ttvevent_list *)chainl->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
  }

 chainsig = (chain_list *)ttvfig->ELCMDSIG ;
 ttvfig->ELCMDSIG = NULL ;
 ttvfig->NBELCMDSIG = (long)0 ;
 ttvfig->NBILCMDSIG = (long)0 ;

 while(chainsig != NULL) 
   {
    chain = chainsig->NEXT ;
    if((((ttvsig_list *)chainsig->DATA)->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
     {
      ((ttvsig_list *)chainsig->DATA)->TYPE &= ~(TTV_SIG_EXT) ;
      chainsig->NEXT = (chain_list *)ttvfig->ELCMDSIG ;
      ttvfig->ELCMDSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBELCMDSIG++ ;
     }
    else
     {
      chainsig->NEXT = (chain_list *)ttvfig->ILCMDSIG ;
      ttvfig->ILCMDSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBILCMDSIG++ ;
     }
    chainsig = chain ;
   }

 ttvfig->ELCMDSIG = ttv_allocreflist((chain_list*)ttvfig->ELCMDSIG,ttvfig->NBELCMDSIG) ;
 ttvfig->ILCMDSIG = ttv_allocreflist((chain_list*)ttvfig->ILCMDSIG,ttvfig->NBILCMDSIG) ;

 chainsig = (chain_list *)ttvfig->ELATCHSIG ;
 ttvfig->ELATCHSIG = NULL ;
 ttvfig->NBELATCHSIG = (long)0 ;
 ttvfig->NBILATCHSIG = (long)0 ;

 while(chainsig != NULL) 
   {
    chain = chainsig->NEXT ;
    if((((ttvsig_list *)chainsig->DATA)->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
     {
      ((ttvsig_list *)chainsig->DATA)->TYPE &= ~(TTV_SIG_EXT) ;
      chainsig->NEXT = (chain_list *)ttvfig->ELATCHSIG ;
      ttvfig->ELATCHSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBELATCHSIG++ ;
     }
    else
     {
      chainsig->NEXT = (chain_list *)ttvfig->ILATCHSIG ;
      ttvfig->ILATCHSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBILATCHSIG++ ;
     }
    chainsig = chain ;
   }

 ttvfig->ELATCHSIG = ttv_allocreflist((chain_list*)ttvfig->ELATCHSIG,ttvfig->NBELATCHSIG) ;
 ttvfig->ILATCHSIG = ttv_allocreflist((chain_list*)ttvfig->ILATCHSIG,ttvfig->NBILATCHSIG) ;

 chainsig = (chain_list *)ttvfig->EPRESIG ;
 ttvfig->EPRESIG = NULL ;
 ttvfig->NBEPRESIG = (long)0 ;
 ttvfig->NBIPRESIG = (long)0 ;

 while(chainsig != NULL) 
   {
    chain = chainsig->NEXT ;
    if((((ttvsig_list *)chainsig->DATA)->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
     {
      ((ttvsig_list *)chainsig->DATA)->TYPE &= ~(TTV_SIG_EXT) ;
      chainsig->NEXT = (chain_list *)ttvfig->EPRESIG ;
      ttvfig->EPRESIG = (ttvsig_list **)chainsig ;
      ttvfig->NBEPRESIG++ ;
     }
    else
     {
      chainsig->NEXT = (chain_list *)ttvfig->IPRESIG ;
      ttvfig->IPRESIG = (ttvsig_list **)chainsig ;
      ttvfig->NBIPRESIG++ ;
     }
    chainsig = chain ;
   }

 ttvfig->EPRESIG     = ttv_allocreflist((chain_list*)ttvfig->EPRESIG,ttvfig->NBEPRESIG) ;
 ttvfig->IPRESIG     = ttv_allocreflist((chain_list*)ttvfig->IPRESIG,ttvfig->NBIPRESIG) ;

 chainsig = (chain_list *)ttvfig->EBREAKSIG ;
 ttvfig->EBREAKSIG = NULL ;
 ttvfig->NBEBREAKSIG = (long)0 ;
 ttvfig->NBIBREAKSIG = (long)0 ;

 while(chainsig != NULL) 
   {
    chain = chainsig->NEXT ;
    if((((ttvsig_list *)chainsig->DATA)->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
     {
      ((ttvsig_list *)chainsig->DATA)->TYPE &= ~(TTV_SIG_EXT) ;
      chainsig->NEXT = (chain_list *)ttvfig->EBREAKSIG ;
      ttvfig->EBREAKSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBEBREAKSIG++ ;
     }
    else
     {
      chainsig->NEXT = (chain_list *)ttvfig->IBREAKSIG ;
      ttvfig->IBREAKSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBIBREAKSIG++ ;
     }
    chainsig = chain ;
   }

 ttvfig->EBREAKSIG     = ttv_allocreflist((chain_list*)ttvfig->EBREAKSIG,ttvfig->NBEBREAKSIG) ;
 ttvfig->IBREAKSIG     = ttv_allocreflist((chain_list*)ttvfig->IBREAKSIG,ttvfig->NBIBREAKSIG) ;

 chainsig = (chain_list *)ttvfig->EXTSIG ;
 ttvfig->EXTSIG = NULL ;
 ttvfig->NBEXTSIG = (long)0 ;
 ttvfig->NBINTSIG = (long)0 ;

 while(chainsig != NULL) 
   {
    chain = chainsig->NEXT ;
    if((((ttvsig_list *)chainsig->DATA)->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
     {
      ((ttvsig_list *)chainsig->DATA)->TYPE &= ~(TTV_SIG_EXT) ;
      chainsig->NEXT = (chain_list *)ttvfig->EXTSIG ;
      ttvfig->EXTSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBEXTSIG++ ;
     }
    else
     {
      chainsig->NEXT = (chain_list *)ttvfig->INTSIG ;
      ttvfig->INTSIG = (ttvsig_list **)chainsig ;
      ttvfig->NBINTSIG++ ;
     }
    chainsig = chain ;
   }

 ttvfig->EXTSIG     = ttv_allocreflist((chain_list*)ttvfig->EXTSIG,ttvfig->NBEXTSIG) ;
 ttvfig->INTSIG     = ttv_allocreflist((chain_list*)ttvfig->INTSIG,ttvfig->NBINTSIG) ;

 return(ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_printloop()                           */
/* parametres :                                                              */
/* nb_loop : nombre de boucles                                               */
/* list_loop : list des boucles                                              */
/* figname : nom de la figure                                                */
/*                                                                           */
/* construit les signaux d'une ttvfig                                        */
/*****************************************************************************/
void ttv_printloop(nb_loop,list_loop,figname)
int nb_loop ;
chain_list *list_loop ;
char *figname ;
{
 if(nb_loop != 0)
    {
     FILE *file ;
     char f[64] ;
     char buffer[1024] ;
     int i = 1 ;

     strcpy(f,figname) ; 
     strcat(f,".loop") ;

     if((file = fopen(f,"w")) == NULL) /*  erreur en ouverture du fichier */
     ttv_error(20,f,TTV_WARNING) ;

     sprintf(buffer,"List of circuit loops : %s.loop\n",figname);
     avt_printExecInfo(file, "#", buffer, "");
     if(nb_loop == 1) fprintf(file,"\none loop was detected:\n") ;
     else fprintf(file,"%d loops were detected:\n",nb_loop) ;
 
     while(list_loop != NULL)
        {
         chain_list  *aux_l ;

         fprintf(file,"loop no %d:\n",i) ;
         i++ ;

        for(aux_l=(chain_list *)list_loop->DATA;aux_l;aux_l=aux_l->NEXT)
            {
             fprintf(file,"(%s)\n",(char*)aux_l->DATA) ;
            }

         fprintf(file,"\n") ;
         list_loop = list_loop->NEXT ;
        }

     if(fclose(file) != 0) ttv_error(21,f,TTV_WARNING) ;
    }
}

/*****************************************************************************/
/*                        function ttv_existeline()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/* node : noeud initial                                                      */
/* root : noeud final                                                        */
/* type : type d'arc                                                         */
/*                                                                           */
/* verifie si un arc existe dans un model                                    */
/*****************************************************************************/
int ttv_existeline(ttvfig,node,root,type)
ttvfig_list *ttvfig ;
ttvevent_list *node ;
ttvevent_list *root ;
long type ;
{
 ttvline_list *line ;

 if((type & (TTV_LINE_F|TTV_LINE_D|TTV_LINE_E)) != 0)
  {
   line = root->INLINE ;
  }
 else
  {
   line = root->INPATH ;
  }

 while(line != NULL)
  {
   if((line->NODE == node) && ((line->TYPE & type) == type))
    return(1) ;
   line = line->NEXT ;
  }

 return(0) ;
#ifndef __ALL__WARNING_
 ttvfig = NULL;
#endif
}

/*****************************************************************************/
/*                        function ttv_getline()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/* ttvins : instance                                                         */
/* node : noeud initial                                                      */
/* root : noeud final                                                        */
/* cmd : command                                                             */
/* type : type d'arc                                                         */
/* typecmd : type de commande                                                */
/* nb : nombre de lien a trouver                                             */
/* order : ordonner ou pas                                                   */
/*                                                                           */
/* recherche une lien en fonction de ses caractéristiques                    */
/*****************************************************************************/
ttvline_list *ttv_getline(ttvfig,ttvins,node,root,cmd,type,typecmd,nb,order)
ttvfig_list *ttvfig ;
ttvfig_list *ttvins ;
ttvevent_list *node ;
ttvevent_list *root ;
ttvevent_list *cmd ;
long type ;
long typecmd ;
int nb ;
int order ;
{
 ttvline_list *line ;
 int nbx = 1 ;

 if((type & (TTV_LINE_F|TTV_LINE_D|TTV_LINE_E)) != 0)
  {
   line = root->INLINE ;
  }
 else
  {
   line = root->INPATH ;
  }

 while(line != NULL)
  {
   if((order == 1) && (node->ROOT != line->NODE->ROOT))
     return(NULL) ;
   if((line->NODE == node) && ((line->TYPE & type) == type) &&
      ((ttvins == NULL) || (line->FIG == ttvins)))
    {
     if((cmd == NULL) || (ttv_getlinecmd(ttvfig,line,typecmd) == cmd))
      {
       if(nb == nbx)
         return(line) ;
       else
         nbx++ ;
      }
    }
   line = line->NEXT ;
  }

 return(line) ;
}

/*****************************************************************************/
/*                        function ttv_getrcxlofig()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/*                                                                           */
/* recupere la lofig rcx pour une ttvfig                                     */
/*****************************************************************************/
lofig_list *ttv_getrcxlofig(ttvfig)
ttvfig_list *ttvfig ;
{
 ptype_list *ptype = getptype(ttvfig->INFO->USER,TTV_FIG_LOFIG) ;
 lofig_list *lofig ;
 
 if(ptype != NULL)
  return((lofig_list *)ptype->DATA) ;

 lofig = rcx_getlofig(ttvfig->INFO->FIGNAME, NULL) ;
 if(lofig){
     locklofig(lofig) ;
     ttvfig->INFO->USER = addptype(ttvfig->INFO->USER,TTV_FIG_LOFIG,lofig) ;
 }

 return(lofig) ;
}

/*****************************************************************************/
/*                        function ttv_delrcxlofig()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/*                                                                           */
/* supprime la lofig rcx d'une ttvfig                                        */
/*****************************************************************************/
void ttv_delrcxlofig(ttvfig)
ttvfig_list *ttvfig ;
{
 ptype_list *ptype = getptype(ttvfig->INFO->USER,TTV_FIG_LOFIG) ;

 if(ptype != NULL)
   {
    if(ptype->DATA != NULL)
     {
      unlocklofig((lofig_list *)ptype->DATA) ;
      rcx_dellofig((lofig_list *)ptype->DATA) ;
     }
    ttvfig->INFO->USER = delptype(ttvfig->INFO->USER,TTV_FIG_LOFIG) ;
   }
}

/*****************************************************************************/
/*                        function ttv_getdelaypos()                         */
/* parametres :                                                              */
/* line : line du delay                                                      */
/* size : taille du tableau                                                  */
/*                                                                           */
/* calcul la position et la taille dans le tableau des delay                 */
/*****************************************************************************/
int ttv_getdelaypos(ttvfig,type,size)
ttvfig_list *ttvfig ;
long type ;
int *size ;
{
 int res ;

 switch(type & TTV_LINE_TYPE)
  {
   case TTV_LINE_P : res = TTV_DELAY_P ;
                     *size = ttvfig->NBPBLOC ;
                     break ;
   case TTV_LINE_J : res = TTV_DELAY_J ;
                     *size = ttvfig->NBJBLOC ;
                     break ;
   case TTV_LINE_T : res = TTV_DELAY_T ;
                     *size = ttvfig->NBTBLOC ;
                     break ;
   case TTV_LINE_F : res = TTV_DELAY_F ;
                     *size = ttvfig->NBFBLOC ;
                     break ;
   case TTV_LINE_E : res = TTV_DELAY_E ;
                     *size = ttvfig->NBEBLOC ;
                     break ;
   case TTV_LINE_D : res = TTV_DELAY_D ;
                     *size = ttvfig->NBDBLOC ;
                     break ;
  }

 return res ;
}

/*****************************************************************************/
/*                        function ttv_getlinedelay()                        */
/* parametres :                                                              */
/* line : line du delay                                                      */
/*                                                                           */
/* recherche le delay d'une line                                             */
/*****************************************************************************/
ttvdelay_list *ttv_getlinedelay(line)
ttvline_list *line ;
{
 ttvfig_list *ttvfig = line->FIG ;
 ttvdelay_list **delay ;
 int size ;
 int pos ;

 if(ttvfig->DELAY == NULL)
  return(NULL) ;

 pos = ttv_getdelaypos(line->FIG,line->TYPE,&size) ;

 if((delay = ttvfig->DELAY[pos]) == NULL)
  return(NULL) ;

 if(line->INDEX == TTV_LINE_NOINDEX)
    ttv_alloclineindex(ttvfig,line->TYPE) ;

 return(delay[line->INDEX]) ;
}

/*****************************************************************************/
/*                        function ttv_addlinedelay()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig                                                           */
/* line : line du delay                                                      */
/* dmax ; delay max                                                          */
/* dmin ; delay min                                                          */
/* fmax ; front max                                                          */
/* fmin ; front min                                                          */
/*                                                                           */
/* ajoute la structure delay a une line                                      */
/*****************************************************************************/
ttvdelay_list *ttv_addlinedelay(line,dmax,dmin,fmax,fmin,cmax,cmin)
ttvline_list *line ;
long dmax ;
long dmin ;
long fmax ;
long fmin ;
float cmax ;
float cmin ;
{
 ttvdelay_list *ptdelay ;

 ptdelay = ttv_getlinedelay(line) ;

 // On vérifie si les informations à mémoriser seront différentes de celles
 // contenues sur le line
 if( ( ( dmax != TTV_NOTIME && dmax == line->VALMAX            ) ||
       ( ptdelay            && ptdelay->VALMAX == line->VALMAX )    ) &&
     ( ( fmax != TTV_NOSLOPE && fmax == line->FMAX          ) ||
       ( ptdelay             && ptdelay->FMAX == line->FMAX )       ) &&
     ( ( dmin != TTV_NOTIME && dmin == line->VALMIN            ) ||
       ( ptdelay            && ptdelay->VALMIN == line->VALMIN )    ) &&
     ( ( fmin != TTV_NOSLOPE && fmin == line->FMIN          ) ||
       ( ptdelay             && ptdelay->FMIN == line->FMIN )       ) &&
     ( cmax == TTV_NOCAPA  && ptdelay && ptdelay->CMAX == TTV_NOCAPA ) &&
     ( cmin == TTV_NOCAPA  && ptdelay && ptdelay->CMIN == TTV_NOCAPA )               )
 {
   if( ptdelay )
     ttv_dellinedelay( line );
   return NULL;
 }

 if(ptdelay == NULL)
  ptdelay = ttv_alloclinedelay(line) ;

 if(dmax != TTV_NOTIME)
   ptdelay->VALMAX = dmax ;
 if(dmin != TTV_NOTIME)
   ptdelay->VALMIN = dmin ;
 if(fmax != TTV_NOSLOPE)
   ptdelay->FMAX = fmax ;
 if(fmin != TTV_NOSLOPE)
   ptdelay->FMIN = fmin ;
 if(cmax != TTV_NOCAPA)
   ptdelay->CMAX = cmax ;
 if(cmin != TTV_NOCAPA)
   ptdelay->CMIN = cmin ;
 return(ptdelay) ;
}

/*****************************************************************************/
/*                        function ttv_dellinedelay()                        */
/* parametres :                                                              */
/* line a supprimer                                                          */
/*                                                                           */
/* supprime la structure delay d'une line                                     */
/*****************************************************************************/
void ttv_dellinedelay(line)
ttvline_list *line ;
{
 ttvfig_list *ttvfig = line->FIG ;
 ttvdelay_list **delay ;
 int size ;
 int pos ;
 ptype_list *ptype;

 pos = ttv_getdelaypos(line->FIG,line->TYPE,&size) ;

 if(ttvfig->DELAY == NULL)
  {
   return ;
  }

 delay = ttvfig->DELAY[pos] ;
 if(delay == NULL)
  {
   return ;
  }

 if(line->INDEX == TTV_LINE_NOINDEX)
    ttv_alloclineindex(ttvfig,line->TYPE) ;

 if(delay[line->INDEX] == NULL)
   {
    return ;
   }

 if ((ptype = getptype (delay[line->INDEX]->USER, STM_SLOPEMAX_PWL_PTYPE))) {
     stm_pwl_destroy ((stm_pwl*)ptype->DATA);
     delay[line->INDEX]->USER = delptype (delay[line->INDEX]->USER, STM_SLOPEMAX_PWL_PTYPE);
 }

 if ((ptype = getptype (delay[line->INDEX]->USER, STM_SLOPEMIN_PWL_PTYPE))) {
     stm_pwl_destroy ((stm_pwl*)ptype->DATA);
     delay[line->INDEX]->USER = delptype (delay[line->INDEX]->USER, STM_SLOPEMIN_PWL_PTYPE);
 }

 mbkfree(delay[line->INDEX]) ;
 delay[line->INDEX] = NULL ;
}

/*****************************************************************************/
/*                        function ttv_getnodedelay()                        */
/* parametres :                                                              */
/* node : node du delay                                                      */
/*                                                                           */
/* recherche le delay d'une node                                             */
/*****************************************************************************/
ttvdelay_list *ttv_getnodedelay(node)
ttvevent_list *node ;
{
 ptype_list *ptype ;

 ptype = getptype(node->USER,TTV_NODE_DELAY) ;
 if(ptype != NULL)
   return(ptype->DATA) ;
 else
   return(NULL) ;
}

/*****************************************************************************/
/*                        function ttv_addnodedelay()                        */
/* parametres :                                                              */
/* node : node du delay                                                      */
/* dmax ; delay max                                                          */
/* dmin ; delay min                                                          */
/* fmax ; front max                                                          */
/* fmin ; front min                                                          */
/*                                                                           */
/* ajoute la structure delay a une node                                      */
/*****************************************************************************/
ttvdelay_list *ttv_addnodedelay(node,dmax,dmin,fmax,fmin)
ttvevent_list *node ;
long dmax ;
long dmin ;
long fmax ;
long fmin ;
{
 ttvdelay_list *delay = ttv_getnodedelay(node) ;

 if(delay == NULL)
  {
   delay = (ttvdelay_list *)mbkalloc(sizeof(ttvdelay_list)) ;
   node->USER = addptype(node->USER,TTV_NODE_DELAY,delay) ;
  }

 delay->VALMAX = dmax ;
 delay->VALMIN = dmin ;
 delay->FMAX = fmax ;
 delay->FMIN = fmin ;
 delay->USER = NULL ;
 delay->RDRIVERMAX = -1.0 ;
 delay->RDRIVERMIN = -1.0 ;
 delay->CDRIVERMAX = -1.0 ;
 delay->CDRIVERMIN = -1.0 ;

 return(delay) ;
}

/*****************************************************************************/
/*                        function ttv_delnodedelay()                        */
/* parametres :                                                              */
/* node a supprimer                                                          */
/*                                                                           */
/* supprime la structure delay d'une node                                     */
/*****************************************************************************/
void ttv_delnodedelay(node)
ttvevent_list *node ;
{
 ptype_list    *ptype ;
 ttvdelay_list *delay = ttv_getnodedelay(node) ;
 

 if(delay != NULL)
  {
   node->USER = delptype(node->USER,TTV_NODE_DELAY) ;
   ptype = getptype( delay->USER, STM_SLOPEMAX_PWL_PTYPE );
   if( ptype ) {
     stm_pwl_destroy( ptype->DATA );
     delay->USER = delptype( delay->USER, STM_SLOPEMAX_PWL_PTYPE );
   }
   ptype = getptype( delay->USER, STM_SLOPEMIN_PWL_PTYPE );
   if( ptype ) {
     stm_pwl_destroy( ptype->DATA );
     delay->USER = delptype( delay->USER, STM_SLOPEMIN_PWL_PTYPE );
   }
   mbkfree(delay) ;
  }
}

/*****************************************************************************/
/*                        function ttv_getdelaymax ()                        */
/* line : line du delay                                                      */
/*                                                                           */
/* calcule le delai a partir du modele ou de VALMAX                          */
/*****************************************************************************/
long ttv_getdelaymax(line)
ttvline_list *line ;
{
 ttvdelay_list *ptdelay ;
 ttvpropinfo_list *propinfo;
 ssta_entry_values *sev;
 
 if ((sev=ttv_ssta_getvalues(line))!=NULL)
   return sev->delaymax;

 if((propinfo = ttv_get_prop_info(line->ROOT)) != NULL)
   if (propinfo->CTK.DELAY!=TTV_NOTIME)
     return(propinfo->CTK.DELAY);
 
 if((ptdelay = ttv_getlinedelay(line)) != NULL){
     if(ptdelay->VALMAX == TTV_NOTIME)
         return(line->VALMAX) ;
     return (ptdelay->VALMAX) ;
 }else{
     return line->VALMAX;
 }
}

/*****************************************************************************/
/*                        function ttv_getdelaymin ()                        */
/* line : line du delay                                                      */
/*                                                                           */
/* calcule le delai a partir du modele ou de VALMIN                          */
/*****************************************************************************/
long ttv_getdelaymin(line)
ttvline_list *line ;
{
 ttvdelay_list *ptdelay ;
 ttvpropinfo_list *propinfo;
 ssta_entry_values *sev;

 if ((sev=ttv_ssta_getvalues(line))!=NULL)
   return sev->delaymin;

 if((propinfo = ttv_get_prop_info(line->ROOT)) != NULL)
   if (propinfo->CTK.DELAY!=TTV_NOTIME)
     return(propinfo->CTK.DELAY);

 if((ptdelay = ttv_getlinedelay(line)) != NULL){
     if(ptdelay->VALMIN == TTV_NOTIME)
         return(line->VALMIN) ;
     return (ptdelay->VALMIN) ;
 }else{
     return line->VALMIN;
 }
}

/*****************************************************************************/
/*                        function ttv_getslopemax ()                        */
/* line : line du delay                                                      */
/*                                                                           */
/* calcule le front a partir du modele ou de FMAX                            */
/*****************************************************************************/
long ttv_getslopemax(line)
ttvline_list *line ;
{
 ttvdelay_list *ptdelay ;
 ttvpropinfo_list *propinfo;
 ssta_entry_values *sev;

 if ((sev=ttv_ssta_getvalues(line))!=NULL)
   return sev->slopemax;

 if((propinfo = ttv_get_prop_info(line->ROOT)) != NULL)
   if (propinfo->CTK.SLEW!=TTV_NOTIME)
     return(propinfo->CTK.SLEW);

 if((ptdelay = ttv_getlinedelay(line)) != NULL){
     if(ptdelay->FMAX == TTV_NOTIME)
         return(line->FMAX) ;
     return (ptdelay->FMAX) ;
 }else{
     return line->FMAX;
 }
}

/*****************************************************************************/
/*                        function ttv_getslopemin ()                        */
/* line : line du delay                                                      */
/*                                                                           */
/* calcule le front a partir du modele ou de FMIN                            */
/*****************************************************************************/
long ttv_getslopemin(line)
ttvline_list *line ;
{
 ttvdelay_list *ptdelay ;
 ttvpropinfo_list *propinfo;
 ssta_entry_values *sev;

 if ((sev=ttv_ssta_getvalues(line))!=NULL)
   return sev->slopemin;

 if((propinfo = ttv_get_prop_info(line->ROOT)) != NULL)
   if (propinfo->CTK.SLEW!=TTV_NOTIME)
     return(propinfo->CTK.SLEW);

 if((ptdelay = ttv_getlinedelay(line)) != NULL){
     if(ptdelay->FMIN == TTV_NOTIME)
         return(line->FMIN) ;
     return (ptdelay->FMIN) ;
 }else{
     return line->FMIN;
 }
}

/*****************************************************************************/
/*                        function ttv_getcapamax ()                        */
/* line : line du delay                                                      */
/*                                                                           */
/*****************************************************************************/
float ttv_getcapamax(line)
ttvline_list *line ;
{
 ttvdelay_list *ptdelay ;

 if((ptdelay = ttv_getlinedelay(line)) != NULL)
   {
    if(ptdelay->VALMAX == TTV_NOTIME)
        return(line->ROOT->ROOT->CAPA) ;
    return (ptdelay->CMAX) ;
   }
 else
    return line->ROOT->ROOT->CAPA;
}

/*****************************************************************************/
/*                        function ttv_getcapamin ()                        */
/* line : line du delay                                                      */
/*                                                                           */
/*****************************************************************************/
float ttv_getcapamin(line)
ttvline_list *line ;
{
 ttvdelay_list *ptdelay ;

 if((ptdelay = ttv_getlinedelay(line)) != NULL)
   {
    if(ptdelay->VALMIN == TTV_NOTIME)
        return(line->ROOT->ROOT->CAPA) ;
    return (ptdelay->CMIN) ;
   }
 else
    return line->ROOT->ROOT->CAPA;
}

/*****************************************************************************/
/*                        function ttv_absmaxdiffslope()                     */
/*                                                                           */
/* Renvoie la variation de front maximum.                                    */
/*****************************************************************************/
long ttv_absmaxdiffslope( delta_old, val_old, val_new )
long delta_old;
long val_old;
long val_new;
{
  long delta_new;

  if( val_new != TTV_NOSLOPE ) {
  
    if( val_old != TTV_NOSLOPE )
      delta_new = val_new - val_old;
    else
      delta_new = val_new ;

    if( delta_new < 0l ) delta_new = -delta_new;

    if( delta_new > delta_old )
      return delta_new;
  }

  return delta_old;
  
}

/*****************************************************************************/
/*                        function ttv_getslopeforload()                     */
/*                                                                           */
/* Renvoie le front maximum sur une line, en prenant en compte les lines RC  */
/* en aval.                                                                  */
/* which : TTV_MODE_LINE | TTV_MODE_DELAY. Renvoie soit le délai sans        */
/*         agression, soit le délai recalculé.                               */
/*****************************************************************************/
float ttv_getslopeforload( line, which )
ttvline_list *line;
long          which;
{
  long          f ;
  long          fmax = 0 ;
  ptype_list   *ptype ;
  chain_list   *lines = NULL ;
  ttvline_list *linerc ;

  if((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) {

    ptype = getptype(line->ROOT->USER,TTV_NODE_DUALLINE) ;

    if( ptype ) {

      for( lines = (chain_list *)ptype->DATA ; lines ; lines = lines->NEXT ) {

        linerc = (ttvline_list*)lines->DATA;

        if( which == TTV_MODE_LINE )
          f = linerc->FMAX;
        else
          f = ttv_getslopemax( linerc );
          
        if( f == TTV_NOSLOPE ) {
      
          if( which == TTV_MODE_LINE )
            f = linerc->FMIN;
          else
            f = ttv_getslopemin( linerc );
        }

        if( f != TTV_NOSLOPE && f > fmax )
          fmax = f;
      }
    }
  }

  if( which == TTV_MODE_LINE )
    f = line->FMAX;
  else
    f = ttv_getslopemax( line );
    
  if( f == TTV_NOSLOPE ) {

    if( which == TTV_MODE_LINE )
      f = line->FMIN;
    else
      f = ttv_getslopemin( line );
  }
  
  if( f != TTV_NOSLOPE && f > fmax )
    fmax = f;

  if( fmax == 0 )
    fmax = STM_DEF_SLEW * TTV_UNIT ;

  return fmax;
}

/*****************************************************************************/
/*                        function ttv_calclinedelayslope ()                 */
/*                                                                           */
/* calcul les delays et les fronts d'une line                                */
/*****************************************************************************/
long ttv_calclinedelayslope(line,smin,smax,pwlmin,pwlmax,vtmin,vtmax,vfmin,vfmax,vddmin,vddmax,vthmin,vthmax,capa,dmin,fmin,ptpwlmin,dmax,fmax,ptpwlmax, mode,vsatmin,rlinmin,rmin,c1min,c2min,vsatmax,rlinmax,rmax,c1max,c2max,nrjmin,nrjmax)
ttvline_list *line ;
long smin ;
long smax ;
stm_pwl *pwlmin;
stm_pwl *pwlmax;
float vtmin ;
float vtmax ;
float vfmin ;
float vfmax ;
float vddmin ;
float vddmax ;
float vthmin ;
float vthmax ;
float capa ;
long *dmin ;
long *fmin ;
stm_pwl **ptpwlmin;
long *dmax ;
long *fmax ;
stm_pwl **ptpwlmax;
char mode ;
float vsatmin ;
float rlinmin ;
double *rmin ;
double *c1min ;
double *c2min ;
float vsatmax ;
float rlinmax ;
double *rmax ;
double *c1max ;
double *c2max ;
float  *nrjmin ;
float  *nrjmax ;
{
 locon_list *locon=NULL ;
 locon_list *loconb ;
 locon_list *locone ;
 lofig_list *lofig ;
 rcx_list *ptrcx ;
 double fdmin=-1, *ptfdmin ;
 double ffmin=-1, *ptffmin ;
 double fdmax=-1, *ptfdmax ;
 double ffmax=-1, *ptffmax ;
 char typemax ;
 char typemin ;
 char *figname, *input, *output ;
 long delta=0 ;
 float noisedeltadelaymin; // Influence du bruit sur les délais de type porte
 float noisedeltadelaymax; // due à la diaphonie.
 ttvdelay_list *delay ;
 float f;
 timing_model *modmax;
 timing_model *modmin;
 noise_scr    *modscr;
 rcx_slope     slope;
 long          search;
 double       tc1max = -1.0 ;
 double       tc1min = -1.0 ;
 double       tc2max = -1.0 ;
 double       tc2min = -1.0 ;
 double       trmax = -1.0 ;
 double       trmin = -1.0 ;
 double       smax_shrinked;
 double       smin_shrinked;
 float        fl_d, fl_f ;
 float       *ptfl_d, *ptfl_f ;
 mbk_pwl     *mbkpwlmax ;
 mbk_pwl     *mbkpwlmin ;
 double       cinmax, cinmin ;
 double       rdrivermax, rdrivermin ;
 stm_driver   driver ;
 double       slew ;
 char dir;

 if( c1max ) cinmax = *c1max ;
 else cinmax = -1.0 ;
 if( c1min ) cinmin = *c1min ;
 else cinmin = -1.0 ;
 if( rmax ) rdrivermax = *rmax ;
 else rdrivermax = -1.0 ;
 if( rmin ) rdrivermin = *rmin ;
 else rdrivermin = -1.0 ;

 if( !c1max ) c1max = &tc1max ;
 if( !c2max ) c2max = &tc2max ;
 if( !rmax )  rmax  = &trmax ;
 if( !c1min ) c1min = &tc1min ;
 if( !c2min ) c2min = &tc2min ;
 if( !rmin )  rmin  = &trmin ;

 if( line->TYPE & ( TTV_LINE_F | TTV_LINE_E | TTV_LINE_D ) )
   search = TTV_FIND_LINE ;
 else
   search = TTV_FIND_PATH ;

 if( mode == TTV_MODE_DELAY ) {
 
   if( line->VALMAX == TTV_NOTIME ) {
     *dmax = line->VALMAX ;
     dmax  = NULL ;
   }
 
   if( line->VALMIN == TTV_NOTIME ) {
     *dmin = line->VALMIN ;
     dmin  = NULL ;
   }
 
   if( line->FMAX == TTV_NOSLOPE ) {
     *fmax = line->FMAX ;
     fmax  = NULL ;
   }
 
   if( line->FMIN == TTV_NOSLOPE ) {
     *fmin = line->FMIN ;
     fmin  = NULL ;
   }
 }
 
 figname = line->FIG->INFO->FIGNAME ;

 if( ( ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) && ((line->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ)) || 
     ( ((line->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) && ( search == TTV_FIND_PATH ) ) )
   {
    lofig = ttv_getrcxlofig(line->ROOT->ROOT->ROOT) ;

    if((line->MDMAX == NULL) && (dmax != NULL))
       {
        *dmax = line->VALMAX ;
        dmax = NULL ;
       }

    if((line->MDMIN == NULL) && (dmin != NULL))
       {
        *dmin = line->VALMIN ;
        dmin = NULL ;
       }

    if((line->MFMAX == NULL) && (fmax != NULL))
       {
        *fmax = line->FMAX ;
        fmax = NULL ;
       }

    if((line->MFMIN == NULL) && (fmin != NULL))
       {
        *fmin = line->FMIN ;
        fmin = NULL ;
       }

    if(lofig == NULL)
     {
      *c1max = line->ROOT->ROOT->CAPA / 1000.0 + capa / 1000.0 ;
      *c2max = 0.0 ;
      *rmax = -1.0;
      *c1min = line->ROOT->ROOT->CAPA / 1000.0 + capa / 1000.0 ;
      *c2min = 0.0 ;
      *rmin = -1.0;
      typemax = RCX_CAPALOAD ;
      typemin = RCX_CAPALOAD ;
      noisedeltadelaymin=0.0;
      noisedeltadelaymax=0.0;
     }
    else
     {
      locon = rcx_gethtrcxcon(NULL,lofig,line->ROOT->ROOT->NAME) ;

      if((locon == NULL) || (locon->PNODE == NULL) || 
         ((ptrcx = getrcx(locon->SIG)) == NULL))
       {
        *c1max = line->ROOT->ROOT->CAPA / 1000.0 + capa / 1000.0 ;
        *c2max = 0.0 ;
        *rmax = -1.0 ;
        *c1min = line->ROOT->ROOT->CAPA / 1000.0 + capa / 1000.0 ;
        *c2min = 0.0 ;
        *rmin = -1.0;
        typemax = RCX_CAPALOAD ;
        typemin = RCX_CAPALOAD ;
        noisedeltadelaymin=0.0;
        noisedeltadelaymax=0.0;
       }
      else
       {
        if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
          slope.F0MAX  = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                           line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                           line->ROOT,
                                           search        | 
                                           TTV_FIND_GATE | 
                                           TTV_FIND_RC   | 
                                           TTV_FIND_MAX,
                                           TTV_MODE_LINE
                                         )/TTV_UNIT;
          slope.FCCMAX = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                           line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                           line->ROOT,
                                           search        | 
                                           TTV_FIND_GATE | 
                                           TTV_FIND_RC   | 
                                           TTV_FIND_MAX,
                                           TTV_MODE_DELAY
                                         )/TTV_UNIT;
        }
        else {
          slope.F0MAX  = -1.0 ;
          slope.FCCMAX = -1.0 ;
        }
        
        slope.SENS   = ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                       ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
        slope.CCA   = -1.0 ;

        if((dmax != NULL) || (fmax != NULL)) {
          typemax = rcx_rcnload( lofig, 
                                 locon->SIG, 
                                 locon->PNODE,
                                 rmax, c1max, c2max, 
                                 RCX_BESTLOAD, 
                                 capa/1000.0,
                                 &slope, 
                                 RCX_MAX 
                               ) ;
        }

        if((dmin != NULL) || (fmin != NULL)) {
          typemin = rcx_rcnload( lofig, 
                                 locon->SIG, 
                                 locon->PNODE,
                                 rmin, c1min, c2min, 
                                 RCX_BESTLOAD, 
                                 capa/1000.0,
                                 &slope, 
                                 RCX_MIN 
                               );
        }
       }
     }

    TTV_COM_CURRENT_LINE=line; // <- pour les apis
    TTV_COM=ttv_communication;
    TTV_COM_DELAY_COMPUTATION_TYPE=1; // delay max
    
    /* names for warning messages */
/*    input = mbkalloc(strlen(line->NODE->ROOT->NETNAME)+4);
    strcpy(input, line->NODE->ROOT->NETNAME);
    if ((line->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) strcat(input, "(R)");
    else strcat(input, "(F)");
    output = mbkalloc(strlen(line->ROOT->ROOT->NETNAME)+4);
    strcpy(output, line->ROOT->ROOT->NETNAME);
    if ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) strcat(output, "(R)");
    else strcat(output, "(F)");*/
    if ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) dir='R';
    else dir='F';
    input=output=line->ROOT->ROOT->NETNAME;

    if( dmax || fmax ) {
      ptfl_d = NULL ;
      ptfl_f = NULL ;
      if(dmax) ptfl_d = &fl_d ;
      if(fmax) ptfl_f = &fl_f ;

      driver.r = rdrivermax ;
      driver.c = cinmax ;
      driver.v = 0.0 ;

      if(typemax == RCX_CAPALOAD)
        stm_gettiming( figname,line->MDMAX, line->MFMAX, (float)smax/TTV_UNIT, pwlmax, &driver, *c1max*1000.0, ptfl_d, ptfl_f, ptpwlmax, output, input, dir );
      else 
        stm_gettiming_pi( figname,line->MDMAX, line->MFMAX, (float)smax/TTV_UNIT, pwlmax, &driver, *c1max*1000.0, *c2max*1000.0, *rmax, ptfl_d, ptfl_f, ptpwlmax, output, input, dir );
        
      if( dmax ) *dmax = (long)(TTV_UNIT*fl_d+0.5);
      if( fmax ) *fmax = (long)(TTV_UNIT*fl_f+0.5);
    }
   
    /*
    if(dmax != NULL)
     {
      if(typemax == RCX_CAPALOAD)
        *dmax = (long)(TTV_UNIT*stm_getdelay(figname,line->MDMAX,*c1max*1000.0,smax/TTV_UNIT,pwlmax) + 0.5) ;
      else
        *dmax = (long)(TTV_UNIT*stm_getdelay_pi(figname,line->MDMAX,*c1max*1000.0,*c2max*1000.0,*rmax,smax/TTV_UNIT,pwlmax) + 0.5) ;
     }
    if(fmax != NULL)
     {
      if(typemax == RCX_CAPALOAD)
        *fmax = (long)(TTV_UNIT*stm_getslew(figname,line->MFMAX,*c1max*1000.0,smax/TTV_UNIT,pwlmax, ptpwlmax) + 0.5) ;
      else
        *fmax = (long)(TTV_UNIT*stm_getslew_pi(figname,line->MFMAX,*c1max*1000.0,*c2max*1000.0,*rmax,smax/TTV_UNIT,pwlmax, ptpwlmax) + 0.5) ;
     }
    */

    TTV_COM_DELAY_COMPUTATION_TYPE=0; // delay min

    if( dmin || fmin ) {
      ptfl_d = NULL ;
      ptfl_f = NULL ;
      if(dmin) ptfl_d = &fl_d ;
      if(fmin) ptfl_f = &fl_f ;

      driver.r = rdrivermin ;
      driver.c = cinmin ;
      driver.v = 0.0 ;

      if(typemin == RCX_CAPALOAD)
        stm_gettiming( figname,line->MDMIN, line->MFMIN, (float)smin/TTV_UNIT, pwlmin, &driver, *c1min*1000.0, ptfl_d, ptfl_f, ptpwlmin, output, input, dir );
      else 
        stm_gettiming_pi( figname,line->MDMIN, line->MFMIN, (float)smin/TTV_UNIT, pwlmin, &driver, *c1min*1000.0, *c2min*1000.0, *rmin, ptfl_d, ptfl_f, ptpwlmin, output, input, dir );
        
      if( dmin ) *dmin = (long)(TTV_UNIT*fl_d+0.5);
      if( fmin ) *fmin = (long)(TTV_UNIT*fl_f+0.5);
    }
    
/*    mbkfree(input);
    mbkfree(output);
*/
    /*
    if(dmin != NULL)
     {
      if(typemin == RCX_CAPALOAD)
        *dmin = (long)(TTV_UNIT*stm_getdelay(figname,line->MDMIN,*c1min*1000.0,smin/TTV_UNIT,pwlmin) + 0.5) ;
      else
        *dmin = (long)(TTV_UNIT*stm_getdelay_pi(figname,line->MDMIN,*c1min*1000.0,*c2min*1000.0,*rmin,smin/TTV_UNIT,pwlmin) + 0.5) ;
     }
    if(fmin != NULL)
     {
      if(typemin == RCX_CAPALOAD)
        *fmin = (long)(TTV_UNIT*stm_getslew(figname,line->MFMIN,*c1min*1000.0,smin/TTV_UNIT,pwlmin,ptpwlmin) + 0.5) ;
      else
        *fmin = (long)(TTV_UNIT*stm_getslew_pi(figname,line->MFMIN,*c1min*1000.0,*c2min*1000.0,*rmin,smin/TTV_UNIT,pwlmin,ptpwlmin) + 0.5) ;
     }
     */

     // Correction due au bruit
    TTV_COM_DELAY_COMPUTATION_TYPE=1; // delay max

     if( dmax != NULL && locon != NULL && rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
     
       if( fmax == NULL ) {
         if( typemax == RCX_CAPALOAD ) 
           f = (long)(TTV_UNIT*stm_getslew(figname,line->MFMAX,*c1max*1000.0,smax/TTV_UNIT,pwlmax, ptpwlmax, line->ROOT->ROOT->NAME) + 0.5) ;
         else
           f = (long)(TTV_UNIT*stm_getslew_pi(figname,line->MFMAX,*c1max*1000.0,*c2max*1000.0,*rmax,smax/TTV_UNIT,pwlmax, ptpwlmax, line->ROOT->ROOT->NAME) + 0.5) ;
       }
       else f = *fmax;

       modmax = stm_getmodel( figname, line->MDMAX );

       /* la structure slope a été remplie juste au dessus 
       slope.F0MAX  = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                        line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                        line->ROOT,
                                        search        | 
                                        TTV_FIND_GATE | 
                                        TTV_FIND_RC   | 
                                        TTV_FIND_MAX,
                                        TTV_MODE_LINE
                                      )/TTV_UNIT;
       slope.FCCMAX = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                        line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                        line->ROOT,
                                        search        | 
                                        TTV_FIND_GATE | 
                                        TTV_FIND_RC   | 
                                        TTV_FIND_MAX,
                                        TTV_MODE_DELAY
                                      )/TTV_UNIT;
       slope.SENS   = ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                      ? TRC_SLOPE_UP : TRC_SLOPE_DOWN;
       slope.CCA    = -1.0 ;
       */

       switch( stm_noise_getmodeltype(modmax) ) {
         case STM_NOISE_SCR:
           modscr = stm_noise_getmodel_scr( modmax );
           noisedeltadelaymax = rcx_noisedelay_scr(  lofig,
                                                     locon->SIG, 
                                                     f/TTV_UNIT,
                                                    &slope,
                                                     stm_noise_scr_resi( modscr
                                                                       ),
                                                     vddmax,
                                                     RCX_MAX
                                                  )*TTV_UNIT;
           break;
         default:
           noisedeltadelaymax = 0.0;
       }
       *dmax = *dmax + (long)noisedeltadelaymax;
     }

     // Correction due au bruit
     TTV_COM_DELAY_COMPUTATION_TYPE=0; // delay min
     if(dmin != NULL && locon != NULL && rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {

       if( fmin == NULL ) {
         if( typemin == RCX_CAPALOAD )
           f = TTV_UNIT*stm_getslew(figname,line->MFMIN,*c1min*1000.0,smin/TTV_UNIT,pwlmin,ptpwlmin, line->ROOT->ROOT->NAME);
         else
           f = TTV_UNIT*stm_getslew_pi(figname,line->MFMIN,*c1min*1000.0,*c2min*1000.0,*rmin,smin/TTV_UNIT,pwlmin,ptpwlmin, line->ROOT->ROOT->NAME);
       }
       else f = *fmin;

       modmin = stm_getmodel( figname, line->MDMIN );

       slope.F0MAX  = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                        line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                        line->ROOT,
                                        search        | 
                                        TTV_FIND_GATE | 
                                        TTV_FIND_RC   | 
                                        TTV_FIND_MAX,
                                        TTV_MODE_LINE
                                      )/TTV_UNIT;
       slope.FCCMAX = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                        line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                        line->ROOT,
                                        search        | 
                                        TTV_FIND_GATE | 
                                        TTV_FIND_RC   | 
                                        TTV_FIND_MAX,
                                        TTV_MODE_DELAY
                                      )/TTV_UNIT;

       slope.SENS   = ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                      ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
       slope.CCA = -1.0 ;

       switch( stm_noise_getmodeltype( modmin ) ) {
         case STM_NOISE_SCR:
           modscr = stm_noise_getmodel_scr( modmin );
           noisedeltadelaymin = rcx_noisedelay_scr(  lofig,
                                                     locon->SIG, 
                                                     f/TTV_UNIT,
                                                    &slope,
                                                     stm_noise_scr_resi( modscr
                                                                       ),
                                                     vddmin,
                                                     RCX_MIN
                                                  )*TTV_UNIT;
           break;
         default:
           noisedeltadelaymin = 0.0;
       }
       *dmin = *dmin - (long)noisedeltadelaymin;
     }
     if(nrjmin){
         modmin = stm_getmodel( figname, line->MDMIN );
         if(modmin){
             if( typemin == RCX_CAPALOAD )
                 *nrjmin = stm_energy_eval (modmin, *c1min*1000 , stm_mod_shrinkslew_thr2scm( modmin, smin/TTV_UNIT ));
             else
                 *nrjmin = stm_energy_eval (modmin, (*c1min+*c2min)*1000 , stm_mod_shrinkslew_thr2scm( modmin, smin/TTV_UNIT ));
         }else{
             *nrjmin = 0.0;
         }
     }
     if(nrjmax){
         modmax = stm_getmodel( figname, line->MDMAX );
         if(modmax){
             if( typemax == RCX_CAPALOAD )
                 *nrjmax = stm_energy_eval (modmax, *c1max*1000 , stm_mod_shrinkslew_thr2scm( modmax, smax/TTV_UNIT ));
             else
                 *nrjmax = stm_energy_eval (modmax, (*c1max+*c2max)*1000 , stm_mod_shrinkslew_thr2scm( modmax, smax/TTV_UNIT ));
         }else{
             *nrjmax = 0.0;
         }
     }
     TTV_COM_CURRENT_LINE=NULL;
     TTV_COM=NULL;
   }
 else if ((line->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ)
   {
    lofig = ttv_getrcxlofig(line->FIG) ;

    if(lofig == NULL)
     {
      ttv_error(53,line->FIG->INFO->FIGNAME,TTV_ERROR) ;
     }

    loconb = rcx_gethtrcxcon (NULL, lofig, line->NODE->ROOT->NAME) ;
    locone = rcx_gethtrcxcon (NULL, lofig, line->ROOT->ROOT->NAME) ;

    if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
      slope.F0MAX  = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                       line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                       line->NODE,
                                       search        | 
                                       TTV_FIND_GATE | 
                                       TTV_FIND_RC   | 
                                       TTV_FIND_MAX,
                                       TTV_MODE_LINE
                                     )/TTV_UNIT;
      slope.FCCMAX = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                       line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                       line->NODE,
                                       search        | 
                                       TTV_FIND_GATE | 
                                       TTV_FIND_RC   | 
                                       TTV_FIND_MAX,
                                       TTV_MODE_DELAY
                                     )/TTV_UNIT;
    } else {
       slope.F0MAX  = -1.0 ;
       slope.FCCMAX = -1.0 ;
    }
    slope.SENS   = ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                   ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
    slope.CCA = -1.0;
   
    if((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP){
        smax_shrinked = stm_thr2scm(smax/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, vtmax, vfmax, vddmax, STM_UP);
        smin_shrinked = stm_thr2scm(smin/TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, vtmin, vfmin, vddmin, STM_UP);
    }else{
        smax_shrinked = stm_thr2scm(smax/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, vtmax, vfmax, vddmax, STM_DN);
        smin_shrinked = stm_thr2scm(smin/TTV_UNIT, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, vtmin, vfmin, vddmin, STM_DN);
    }

    slope.MAX.slope   = smax_shrinked ;
    slope.MAX.vend    = vfmax ;
    slope.MAX.vt      = vtmax ;
    slope.MAX.vdd     = vddmax ;
    slope.MAX.vth     = vthmax ;
    slope.MAX.vsat    = vsatmax ;
    slope.MAX.rlin    = rlinmax ;
    slope.MAX.r       = *rmax ;
    slope.MAX.c1      = *c1max ;
    slope.MAX.c2      = *c2max ;
    slope.MAX.slnrm   = smax/TTV_UNIT ;
    slope.MAX.pwl     = stm_pwl_to_mbk_pwl(pwlmax,vtmax,vddmax) ;

    slope.MIN.slope   = smin_shrinked ;
    slope.MIN.vend    = vfmin ;
    slope.MIN.vt      = vtmin ;
    slope.MIN.vdd     = vddmin ;
    slope.MIN.vth     = vthmin ;
    slope.MIN.vsat    = vsatmin ;
    slope.MIN.rlin    = rlinmin ;
    slope.MIN.r       = *rmin ;
    slope.MIN.c1      = *c1min ;
    slope.MIN.c2      = *c2min ;
    slope.MIN.slnrm   = smin/TTV_UNIT ;
    slope.MIN.pwl     = stm_pwl_to_mbk_pwl(pwlmin,vtmin,vddmin) ;

    ptfdmin = NULL ;
    ptffmin = NULL ;
    ptfdmax = NULL ;
    ptffmax = NULL ;
    if( dmax ) ptfdmax = &fdmax ;
    if( fmax ) ptffmax = &ffmax ;
    if( dmin ) ptfdmin = &fdmin ;
    if( fmin ) ptffmin = &ffmin ;
    rcx_getdelayslope( lofig, 
                       loconb, 
                       locone, 
                       &slope, 
                       capa/1000.0, 
                       ptfdmax, 
                       ptfdmin, 
                       ptffmax, 
                       ptffmin, 
                       ptpwlmax ? &mbkpwlmax : NULL, 
                       ptpwlmin ? &mbkpwlmin : NULL 
                     ) ;

    mbk_pwl_free_pwl( slope.MAX.pwl );
    mbk_pwl_free_pwl( slope.MIN.pwl );

    if( ptpwlmax ) {
      *ptpwlmax = mbk_pwl_to_stm_pwl( mbkpwlmax );
      mbk_pwl_free_pwl( mbkpwlmax );
    }
    if( ptpwlmin ) {
      *ptpwlmin = mbk_pwl_to_stm_pwl( mbkpwlmin );
      mbk_pwl_free_pwl( mbkpwlmin );
    }

    if((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP){
        if(ffmax >= 0.0)
            ffmax = stm_scm2thr(ffmax, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, vtmax, vfmax, vddmax, STM_UP);
        if(ffmin >= 0.0)
            ffmin = stm_scm2thr(ffmin, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, vtmin, vfmin, vddmin, STM_UP);
    }else{
        if(ffmax >= 0.0)
            ffmax = stm_scm2thr(ffmax, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, vtmax, vfmax, vddmax, STM_DN);
        if(ffmin >= 0.0)
            ffmin = stm_scm2thr(ffmin, STM_DEFAULT_SMINF, STM_DEFAULT_SMAXF, vtmin, vfmin, vddmin, STM_DN);
    }
    
    if(dmax != NULL)
    { 
      if (fdmax >= 0.0) *dmax = (long)(TTV_UNIT*fdmax + 0.5) ;
      else {
        if( line->VALMAX != TTV_NOTIME )
          *dmax=line->VALMAX;
        else
          *dmax=0l;
      }
    }
    if(dmin != NULL)
    {
      if (fdmin >= 0.0) *dmin = (long)(TTV_UNIT*fdmin + 0.5) ;
      else {
        if( line->VALMIN != TTV_NOTIME )
          *dmin=line->VALMIN;
        else
          *dmin=0l;
      }
    }
    if(fmax != NULL)
    {
      if (ffmax >= 0.0) *fmax = (long)(TTV_UNIT*ffmax + 0.5) ;
      else {
        if( line->FMAX != TTV_NOSLOPE )
          *fmax=line->FMAX;
        else
          *fmax=smax;
      }
    }
    if(fmin != NULL)
    {
      if (ffmin >= 0.0) *fmin = (long)(TTV_UNIT*ffmin + 0.5) ;
      else {
        if( line->FMIN != TTV_NOSLOPE )
          *fmin=line->FMIN;
        else
          *fmin=smin;
      }
    }
    if(nrjmin){
        *nrjmin = 0.0;
    }
    if(nrjmax){
        *nrjmax = 0.0;
    }
   }
 else
   {
    if(dmax != NULL) {
      modmax = stm_getmodel( figname, line->MDMAX );
      slew = stm_mod_shrinkslew_thr2scm( modmax, smax/TTV_UNIT );
      *dmax = (long)(TTV_UNIT*(stm_get_t(vtmax,vtmax,vddmax,0.0,slew) - stm_get_t(vthmax,vtmax,vddmax,0.0,slew))) ;
    }
    if(dmin != NULL) {
      modmin = stm_getmodel( figname, line->MDMIN );
      slew = stm_mod_shrinkslew_thr2scm( modmin, smin/TTV_UNIT );
      *dmin = (long)(TTV_UNIT*(stm_get_t(vtmin,vtmin,vddmin,0.0,slew) - stm_get_t(vthmin,vtmin,vddmin,0.0,slew))) ;
    }
    if(fmax != NULL)
     *fmax = line->FMAX ;
    if(fmin != NULL)
     *fmin = line->FMIN ;
    if(nrjmin){
        *nrjmin = 0.0;
    }
    if(nrjmax){
        *nrjmax = 0.0;
    }
   }

 if( dmin && dmax )
   if( *dmin > *dmax )
     *dmax = *dmin ;

 if(mode == TTV_MODE_LINE)
 {
   if(dmax != NULL) line->VALMAX = *dmax ;
   if(dmin != NULL) line->VALMIN = *dmin ;
   if(fmax != NULL) {
     delta = ttv_absmaxdiffslope( delta, line->FMAX, *fmax );
     line->FMAX = *fmax ;
   }
   if(fmin != NULL) {
     delta = ttv_absmaxdiffslope( delta, line->FMIN, *fmin );
     line->FMIN = *fmin ;
   }
 }
 else if(mode == TTV_MODE_DELAY)
 {
   delay = ttv_getlinedelay( line );
   if( delay ) {
     if(fmax != NULL) delta = ttv_absmaxdiffslope( delta, delay->FMAX, *fmax );
     if(fmin != NULL) delta = ttv_absmaxdiffslope( delta, delay->FMIN, *fmin );
   }
   else {
     if(fmax != NULL) delta = ttv_absmaxdiffslope( delta, line->FMAX, *fmax );
     if(fmin != NULL) delta = ttv_absmaxdiffslope( delta, line->FMIN, *fmin );
   }

   if(((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) && ((line->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ)) {
     ttv_addlinedelay(line,
                      (dmax == NULL) ? TTV_NOTIME : *dmax,
                      (dmin == NULL) ? TTV_NOTIME : *dmin,
                      (fmax == NULL) ? TTV_NOSLOPE : *fmax,
                      (fmin == NULL) ? TTV_NOSLOPE : *fmin,
                      (dmax == NULL) ? TTV_NOCAPA : ((typemax==RCX_CAPALOAD) ? *c1max*1000.0 : (*c1max+*c2max)*1000.0) ,
                      (dmin == NULL) ? TTV_NOCAPA : ((typemin==RCX_CAPALOAD) ? *c1min*1000.0 : (*c1min+*c2min)*1000.0)
                     ) ;
   }
   else {
     ttv_addlinedelay(line,
                      (dmax == NULL) ? TTV_NOTIME : *dmax,
                      (dmin == NULL) ? TTV_NOTIME : *dmin,
                      (fmax == NULL) ? TTV_NOSLOPE : *fmax,
                      (fmin == NULL) ? TTV_NOSLOPE : *fmin,
                      TTV_NOCAPA,
                      TTV_NOCAPA
                     ) ;
   }
 }

 return delta;
}

/*****************************************************************************/
/*                        function ttv_calcgatercdelayslope ()               */
/*                                                                           */
/* calcul les delays et les fronts d'une line                                */
/*****************************************************************************/
void ttv_calcgatercdelayslope(line1,line2,smin,smax,capa,dmin,fmin,dmax,fmax,mode)
ttvline_list *line1 ;
ttvline_list *line2 ;
long smin ;
long smax ;
float capa ;
long *dmin ;
long *fmin ;
long *dmax ;
long *fmax ;
char mode ;
{
 timing_model *modelmin, *modelmax ;
 float vtmin, vtmax ;
 float vfmin, vfmax ;
 float vfrcmin, vfrcmax ;
 float vddmin, vddmax ;
 float vthmin, vthmax ;
 long dmin1 = 0;
 long fmin1 = 0;
 long dmax1 = 0;
 long fmax1 = 0;
 double rmin, c1min, c2min, vsatmin, rlinmin ;
 double rmax, c1max, c2max, vsatmax, rlinmax ;
 lofig_list *lofig ;
 locon_list *locon ;

 if((line1->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
  {
   if( line2 ) 
     modelmin = stm_getmodel(line2->FIG->INFO->FIGNAME,line2->MDMIN) ;
   else
     modelmin = NULL;
   vtmin = stm_mod_vt(modelmin);
   vfmin = stm_mod_vf(modelmin);
   vthmin = stm_mod_vth(modelmin);
   vddmin = stm_mod_vdd(modelmin);
   if( line2 )
     modelmax = stm_getmodel(line2->FIG->INFO->FIGNAME,line2->MDMAX) ;
   else
     modelmax = NULL;
   vtmax = stm_mod_vt(modelmax);
   vfmax = stm_mod_vf(modelmax);
   vthmax = stm_mod_vth(modelmax);
   vddmax = stm_mod_vdd(modelmax);
   if((line1->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP){
       vfrcmin = vddmin;
       vfrcmax = vddmax;
   }else{
       vfrcmin = 0.0;
       vfrcmax = 0.0;
   }
   if(dmin && dmax){
       ttv_calclinedelayslope(line1,smin,smax,NULL,NULL,vtmin,vtmax,vfrcmin,vfrcmax,vddmin,vddmax,vthmin,vthmax,capa,&dmin1,&fmin1,NULL,&dmax1,&fmax1,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
       if( line2 )
         ttv_calclinedelayslope(line2,fmin1,fmax1,NULL,NULL,vtmin,vtmax,vfmin,vfmax,vddmin,vddmax,vthmin,vthmax,capa,dmin,fmin,NULL,dmax,fmax,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
   }else if(dmin){
       ttv_calclinedelayslope(line1,smin,smin,NULL,NULL,vtmin,vtmin,vfrcmin,vfrcmin,vddmin,vddmin,vthmin,vthmin,capa,&dmin1,&fmin1,NULL,NULL,NULL,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
       if( line2 )
         ttv_calclinedelayslope(line2,fmin1,fmin1,NULL,NULL,vtmin,vtmin,vfmin,vfmin,vddmin,vddmin,vthmin,vthmin,capa,dmin,fmin,NULL,NULL,NULL,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
   }else if(dmax){
       ttv_calclinedelayslope(line1,smax,smax,NULL,NULL,vtmax,vtmax,vfrcmax,vfrcmax,vddmax,vddmax,vthmax,vthmax,capa,NULL,NULL,NULL,&dmax1,&fmax1,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
       if( line2 )
         ttv_calclinedelayslope(line2,fmax1,fmax1,NULL,NULL,vtmax,vtmax,vfmax,vfmax,vddmax,vddmax,vthmax,vthmax,capa,NULL,NULL,NULL,dmax,fmax,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
   }
  }
 else
  {
   modelmin = stm_getmodel(line1->FIG->INFO->FIGNAME,line1->MDMIN) ;
   vtmin = stm_mod_vt(modelmin);
   vfmin = stm_mod_vf(modelmin);
   vthmin = stm_mod_vth(modelmin);
   vddmin = stm_mod_vdd(modelmin);
   vsatmin = stm_mod_vsat(modelmin);
   rlinmin = stm_mod_rlin(modelmin);
   modelmax = stm_getmodel(line1->FIG->INFO->FIGNAME,line1->MDMAX) ;
   vtmax = stm_mod_vt(modelmax);
   vfmax = stm_mod_vf(modelmax);
   vthmax = stm_mod_vth(modelmax);
   vddmax = stm_mod_vdd(modelmax);
   vsatmax = stm_mod_vsat(modelmax);
   rlinmax = stm_mod_rlin(modelmax);
   
   lofig = ttv_getrcxlofig( line1->ROOT->ROOT->ROOT ) ;
   if( lofig ) {
     locon = rcx_gethtrcxcon( NULL, lofig, line1->ROOT->ROOT->NAME ) ;
     if( locon ) {
       rcx_rcnload_reset( locon->SIG );
     }
   }

   c1max = -1.0 ;
   if(dmin && dmax){
       ttv_calclinedelayslope(line1,smin,smax,NULL,NULL,vtmin,vtmax,vfmin,vfmax,vddmin,vddmax,vthmin,vthmax,capa,&dmin1,&fmin1,NULL,&dmax1,&fmax1,NULL,mode,-1.0,-1.0,&rmin,&c1min,&c2min,-1.0,-1.0,&rmax,&c1max,&c2max,NULL,NULL );
       if( line2 )
         ttv_calclinedelayslope(line2,fmin1,fmax1,NULL,NULL,vtmin,vtmax,vfmin,vfmax,vddmin,vddmax,vthmin,vthmax,capa,dmin,fmin,NULL,dmax,fmax,NULL,mode,vsatmin,rlinmin,&rmin,&c1min,&c2min,vsatmax,rlinmax,&rmax,&c1max,&c2max,NULL,NULL );
   }else if(dmin){
       ttv_calclinedelayslope(line1,smin,smin,NULL,NULL,vtmin,vtmin,vfmin,vfmin,vddmin,vddmin,vthmin,vthmin,capa,&dmin1,&fmin1,NULL,NULL,NULL,NULL,mode,-1.0,-1.0,&rmin,&c1min,&c2min,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
       if( line2 )
         ttv_calclinedelayslope(line2,fmin1,fmin1,NULL,NULL,vtmin,vtmin,vfmin,vfmin,vddmin,vddmin,vthmin,vthmin,capa,dmin,fmin,NULL,NULL,NULL,NULL,mode,vsatmin,rlinmin,&rmin,&c1min,&c2min,-1.0,-1.0,NULL,NULL,NULL,NULL,NULL );
   }else if(dmax){
       ttv_calclinedelayslope(line1,smax,smax,NULL,NULL,vtmax,vtmax,vfmax,vfmax,vddmax,vddmax,vthmax,vthmax,capa,NULL,NULL,NULL,&dmax1,&fmax1,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,-1.0,-1.0,&rmax,&c1max,&c2max,NULL,NULL );
       if( line2 )
         ttv_calclinedelayslope(line2,fmax1,fmax1,NULL,NULL,vtmax,vtmax,vfmax,vfmax,vddmax,vddmax,vthmax,vthmax,capa,NULL,NULL,NULL,dmax,fmax,NULL,mode,-1.0,-1.0,NULL,NULL,NULL,vsatmax,rlinmax,&rmax,&c1max,&c2max,NULL,NULL );
   }
  }

 if(dmin)
     *dmin += dmin1 ;
 if(dmax)
     *dmax += dmax1 ;
}

/*****************************************************************************/
/*                        function ttv_calcnodedelay()                       */
/*                                                                           */
/* Met à jour la structure delay du node ROOT d'une line.                    */
/* Si ils ne sont pas utilisés, ptpwlmax et ptpwlmin sont effacés.           */
/*****************************************************************************/
int ttv_calcnodedelay( line, type, dmax, fmax, ptpwlmax, dmin, fmin, ptpwlmin )
ttvline_list *line;
long type;
long dmax;
long fmax;
stm_pwl *ptpwlmax;
long dmin;
long fmin;
stm_pwl *ptpwlmin;
{
  ttvdelay_list *delayfrom;
  ttvdelay_list *delayto;
  int            ret=0;
  ptype_list    *ptype;

  delayfrom = ttv_getnodedelay( line->NODE );
  delayto   = ttv_getnodedelay( line->ROOT );

  /* ------------------------------------ */
  
  if((type & TTV_FIND_DELAY) == TTV_FIND_DELAY) {
  
    if( (dmax != TTV_NOTIME) && ( ( delayto->VALMAX == TTV_NOTIME ) || ((dmax + delayfrom->VALMAX) > delayto->VALMAX))) {
    
      if( delayfrom->VALMAX != TTV_NOTIME )
        delayto->VALMAX = dmax + delayfrom->VALMAX ;
      else
        delayto->VALMAX = dmax;

      delayto->FMAX = fmax ;
      if (ptpwlmax) {
        if ((ptype = getptype (delayto->USER, STM_SLOPEMAX_PWL_PTYPE))) {
          stm_pwl_destroy (ptype->DATA);
          ptype->DATA = ptpwlmax;
        } else 
          delayto->USER = addptype (delayto->USER, STM_SLOPEMAX_PWL_PTYPE, ptpwlmax);
      }
      ret = ret | TTV_PROP_MAX;
    }
    else 
      stm_pwl_destroy( ptpwlmax );

    if((dmin != TTV_NOTIME) && ( ( delayto->VALMIN == TTV_NOTIME ) || ((dmin + delayfrom->VALMIN) < delayto->VALMIN))) {

      if( delayfrom->VALMIN != TTV_NOTIME )
        delayto->VALMIN = dmin + delayfrom->VALMIN ;
      else
        delayto->VALMIN = dmin;
      delayto->FMIN = fmin ;
      if (ptpwlmin) {
        if ((ptype = getptype (delayto->USER, STM_SLOPEMIN_PWL_PTYPE))) {
          stm_pwl_destroy (ptype->DATA);
          ptype->DATA = ptpwlmin;
        } else 
          delayto->USER = addptype (delayto->USER, STM_SLOPEMIN_PWL_PTYPE, ptpwlmin);
      }
      ret = ret | TTV_PROP_MIN;
    }
    else 
      stm_pwl_destroy( ptpwlmin );
  }
  else
  {
    if((fmax != TTV_NOSLOPE) && (( delayto->FMAX == TTV_NOSLOPE ) || (fmax > delayto->FMAX))) {

      if( delayfrom->VALMAX != TTV_NOTIME )
        delayto->VALMAX = dmax + delayfrom->VALMAX ;
      else
        delayto->VALMAX = dmax;
      delayto->FMAX = fmax ;
      if (ptpwlmax) {
        if ((ptype = getptype (delayto->USER, STM_SLOPEMAX_PWL_PTYPE))) {
          stm_pwl_destroy (ptype->DATA);
          ptype->DATA = ptpwlmax;
        } else 
          delayto->USER = addptype (delayto->USER, STM_SLOPEMAX_PWL_PTYPE, ptpwlmax);
      }
      ret = ret | TTV_PROP_MAX;
    } 
    else 
      stm_pwl_destroy( ptpwlmax );

    if((fmin != TTV_NOSLOPE) && (( delayto->FMIN == TTV_NOSLOPE ) || (fmin < delayto->FMIN))) {
      if( delayfrom->VALMIN != TTV_NOTIME )
        delayto->VALMIN = dmin + delayfrom->VALMIN ;
      else
        delayto->VALMIN = dmin;
      delayto->FMIN = fmin ;
      if (ptpwlmin) {
        if ((ptype = getptype (delayto->USER, STM_SLOPEMIN_PWL_PTYPE))) {
          stm_pwl_destroy (ptype->DATA);
          ptype->DATA = ptpwlmin;
        } else 
          delayto->USER = addptype (delayto->USER, STM_SLOPEMIN_PWL_PTYPE, ptpwlmin);
      }
      ret = ret | TTV_PROP_MIN;
    }
    else 
      stm_pwl_destroy( ptpwlmin );
  }

  return ret ;
}

/*****************************************************************************/
/*                        function ttv_updatenodedelay ()                    */
/*                                                                           */
/* Met a jour les structure ttvdelay des node, uniquement en appliquant les  */
/* règles de propagation dans la vue TTV.                                    */
/*                                                                           */
/*****************************************************************************/
void ttv_updatenodedelayslope(ttvfig,level,node,type,mode)
ttvfig_list *ttvfig ;
long level ;
ttvevent_list *node ;
long type ;
char mode ;
{
 ttvline_list *line ;
 chain_list *chain ;
 ptype_list *ptype ;
 ttvdelay_list *ptdelay ;
 ttvdelay_list *delay ;
 long dmin = TTV_NOTIME;
 long dmax = TTV_NOTIME;
 long fmin = STM_DEF_SLEW * TTV_UNIT ;
 long fmax = STM_DEF_SLEW * TTV_UNIT ;

 if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                  TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
   line = node->INLINE ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }
 else
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                  TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
   line = node->INPATH ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }

 delay = ttv_getnodedelay(node) ;

 if(delay == NULL)
  {
   if((node->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C)
     delay = ttv_addnodedelay(node,TTV_NOTIME,TTV_NOTIME,
                                   TTV_NOSLOPE,TTV_NOSLOPE) ;
   else
     delay = ttv_addnodedelay(node,TTV_NOTIME, TTV_NOTIME,fmax,fmin) ;
  }


 for(; line != NULL ; line = line->NEXT)
  {
   if(((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) ||
       ((line->TYPE & TTV_LINE_O) == TTV_LINE_O) ||
       ((line->TYPE & TTV_LINE_U) == TTV_LINE_U) ||
       ((line->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) ||
      (ttv_islinelevel(ttvfig,line,level) == 0))
     continue ;

   ptdelay = ttv_getnodedelay(line->NODE) ;

   if(ptdelay == NULL)
    {
     continue ;
    }
   else
    {
     if((type & TTV_FIND_MIN) == TTV_FIND_MIN) {
       dmin = ttv_getdelaymin( line );
       fmin = ttv_getslopemin( line );
     }

     if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
       dmax = ttv_getdelaymax( line );
       fmax = ttv_getslopemax( line );
     }
     ttv_calcnodedelay( line, type, dmax, fmax, NULL, dmin, fmin, NULL);
    }
  }

 for(; chain != NULL ; chain = chain->NEXT)
  {
   line = (ttvline_list *)chain->DATA ;

   if(((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
      (ttv_islinelevel(ttvfig,line,level) == 0))
    continue ;

   ptdelay = ttv_getnodedelay(line->ROOT) ;

   if(ptdelay == NULL)
    {
     ptdelay = ttv_addnodedelay(line->ROOT,TTV_NOTIME, TTV_NOTIME, TTV_NOSLOPE, TTV_NOSLOPE) ;
    }

   if( delay->FMIN == TTV_NOSLOPE ) delay->FMIN = STM_DEF_SLEW * TTV_UNIT ;
   if( delay->FMAX == TTV_NOSLOPE ) delay->FMAX = STM_DEF_SLEW * TTV_UNIT ;

   if((type & TTV_FIND_MIN) == TTV_FIND_MIN) {
     dmin = ttv_getdelaymin( line );
     fmin = ttv_getslopemin( line );
   }

   if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
     dmax = ttv_getdelaymax( line );
     fmax = ttv_getslopemax( line );
   }

   ttv_calcnodedelay( line, type, dmax, fmax, NULL, dmin, fmin, NULL );

  }
#ifndef __ALL__WARNING_
 mode = 0;
#endif
}

/*****************************************************************************/
/*                        function ttv_calcnodedelayslope ()                 */
/*                                                                           */
/* calcul les delays et les fronts d'un noeud                                */
/* Renvoie la valeur absolue de la variation maximum des fronts sur les      */
/* line concernés.                                                           */
/*****************************************************************************/
static void ttv_find_a_slope(ttvfig_list *ttvfig, ttvevent_list *node, long level, long type, ttvdelay_list *delay)
{
  long min=TTV_NOTIME, max=TTV_NOTIME;
  ttvline_list *line;
  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
    {
     ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                    TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
     line = node->INLINE ;
    }
   else
    {
     ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                    TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
     line = node->INPATH ;
    }

  while (line!=NULL)
  {
    if(!((line->TYPE & TTV_LINE_O) == TTV_LINE_O || (line->TYPE & TTV_LINE_U) == TTV_LINE_U ||
       ttv_islinelevel(ttvfig,line,level) == 0))
    {
      if (line->FMAX!=TTV_NOTIME && (max==TTV_NOTIME || line->FMAX>max)) max=line->FMAX;
      if (line->FMIN!=TTV_NOTIME && (min==TTV_NOTIME || line->FMIN<min)) min=line->FMIN;
    }
    line=line->NEXT;
  }

  delay->FMAX=max;
  delay->FMIN=min;
}

long ttv_calcnodedelayslope(ttvfig,level,node,type,mode)
ttvfig_list *ttvfig ;
long level ;
ttvevent_list *node ;
long type ;
char mode ;
{
 ttvline_list *line ;
 chain_list *chain ;
 ptype_list *ptype;
 ttvdelay_list *ptdelay ;
 ttvdelay_list *delay ;
 timing_model *model ;
 float def_vt ;
 float def_vf ;
 float def_vdd ;
 float def_vth ;
 float def_vsat ;
 float def_rlin ;
 float vt_max ;
 float vf_max ;
 float vdd_max ;
 float vsat_max ;
 float rlin_max ;
 float vth_max ;
 float vt_min ;
 float vf_min ;
 float vdd_min ;
 float vsat_min ;
 float rlin_min ;
 float vth_min ;
 float capa = 0.0;
 long *ptdmin ;
 long *ptdmax ;
 long *ptfmin ;
 long *ptfmax ;
 long dmin = TTV_NOTIME;
 long dmax = TTV_NOTIME;
 long fmin = TTV_NOSLOPE;
 long fmax = TTV_NOSLOPE;
 long delta, deltamax=0;
 double rmin, c1min, c2min, rmax, c1max, c2max ;
 long  vfmin = TTV_NOSLOPE;
 long  vfmax = TTV_NOSLOPE;
 double vrmin = -1.0, vc1min = -1.0, vc2min = -1.0 ;
 double vrmax = -1.0, vc1max = -1.0, vc2max = -1.0 ;
 int  ret;
 stm_pwl *pwlmin=NULL, *pwlmax=NULL, *ptpwlmin = NULL , *ptpwlmax = NULL ;
 stm_pwl **ptptpwlmin, **ptptpwlmax ;
 int flagcreatedelay ;

 if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                  TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
   line = node->INLINE ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }
 else
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                  TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
   line = node->INPATH ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }

 def_vt   = stm_mod_default_vt() ;
 def_vdd  = stm_mod_default_vdd() ;
 def_vth  = stm_mod_default_vth() ;
 def_vsat = -1.0 ;
 def_rlin = -1.0 ;
 
 if( ( node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
   def_vf = stm_mod_default_vfu() ;
 else
   def_vf = stm_mod_default_vfd() ;


 vt_min   = def_vt ;
 vdd_min  = def_vdd ;
 vsat_min = def_vsat ;
 rlin_min = def_rlin ;
 vth_min  = def_vth ;
 vf_min   = def_vf ;
 vt_max   = def_vt ;
 vdd_max  = def_vdd ;
 vsat_max = def_vsat ;
 rlin_max = def_rlin ;
 vth_max  = def_vth ;
 vf_max   = def_vf ;

 if((node->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0)
    capa = ttv_get_signal_output_capacitance(ttvfig, node->ROOT); //ttvfig->INFO->CAPAOUT ;

 if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
  {
   ptdmax = &dmax ;
   ptfmax = &fmax ;
   if ((node->ROOT->TYPE & (TTV_SIG_C)) != 0) fmax=ttv_getnodeslew(node, TTV_FIND_MAX);
   else fmax = (long)STM_DEF_SLEW * TTV_UNIT  ;
  }
 else
  {
   ptdmax = NULL ;
   ptfmax = NULL ;
   dmax = TTV_NOTIME ;
   fmax = TTV_NOSLOPE ;
  }

 if((type & TTV_FIND_MIN) == TTV_FIND_MIN)
  {
   ptdmin = &dmin ;
   ptfmin = &fmin ;
   if ((node->ROOT->TYPE & (TTV_SIG_C)) != 0) fmin=ttv_getnodeslew(node, TTV_FIND_MIN);
   else fmin = (long)STM_DEF_SLEW * TTV_UNIT  ;
  }
 else
  {
   ptdmin = NULL ;
   ptfmin = NULL ;
   dmin = TTV_NOTIME ;
   fmin = TTV_NOSLOPE ;
  }

 delay = ttv_getnodedelay(node) ;

 if(delay == NULL)
  {
   if(((node->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) ||
      ((node->ROOT->TYPE & TTV_SIG_CO) == TTV_SIG_CO))
     delay = ttv_addnodedelay(node,TTV_NOTIME,TTV_NOTIME,
                                   TTV_NOSLOPE,TTV_NOSLOPE) ;
   else
     delay = ttv_addnodedelay(node,TTV_NOTIME, TTV_NOTIME,fmax,fmin) ;
  }

 for(; line != NULL ; line = line->NEXT)
  {
   if(((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) ||
       ((line->TYPE & TTV_LINE_O) == TTV_LINE_O) ||
       ((line->TYPE & TTV_LINE_U) == TTV_LINE_U) ||
      (ttv_islinelevel(ttvfig,line,level) == 0))
     continue ;

   flagcreatedelay = 0;
   ptdelay = ttv_getnodedelay(line->NODE) ;

   c1max = line->NODE->ROOT->CAPA ;

   if(ptdelay == NULL)
    {
     ptdelay = ttv_addnodedelay(line->NODE,TTV_NOTIME, TTV_NOTIME, TTV_NOSLOPE, TTV_NOSLOPE) ;
     ttv_find_a_slope(ttvfig, line->NODE, level, type, ptdelay);

     flagcreatedelay = 1;
    }

   if( ptdelay->FMIN == TTV_NOSLOPE ) ptdelay->FMIN = STM_DEF_SLEW * TTV_UNIT ;
   if( ptdelay->FMAX == TTV_NOSLOPE ) ptdelay->FMAX = STM_DEF_SLEW * TTV_UNIT ;

   pwlmin = NULL;
   pwlmax = NULL;
   ptpwlmin = NULL;
   ptpwlmax = NULL;

   c1max = -1.0 ;
   c2max = -1.0 ;
   rmax  = -1.0 ;
   c1min = -1.0 ;
   c2min = -1.0 ;
   rmin  = -1.0 ;

   if(((type & TTV_FIND_LINETOP) == TTV_FIND_LINETOP) &&
      (line->FIG != ttvfig))
     {
      if((type & TTV_FIND_MIN) == TTV_FIND_MIN)
       {
        dmin = ttv_getdelaymin( line );
        fmin = ttv_getslopemin( line );
       }

      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
       {
        dmax = ttv_getdelaymax( line );
        fmax = ttv_getslopemax( line );
       }
      model = NULL ;
     }
    else
     {

      /* ------------------------------------ */
      /* Get PWL Slope                        */
      /* ------------------------------------ */
     if (V_BOOL_TAB[__STM_USE_MSC].VALUE ) {
         if ((ptype = getptype (ptdelay->USER, STM_SLOPEMIN_PWL_PTYPE)))
             pwlmin = (stm_pwl*)ptype->DATA;
         if ((ptype = getptype (ptdelay->USER, STM_SLOPEMAX_PWL_PTYPE)))
             pwlmax = (stm_pwl*)ptype->DATA;
         ptptpwlmax = &ptpwlmax ;
         ptptpwlmin = &ptpwlmin ;
     }
     else {
         ptptpwlmax = NULL ;
         ptptpwlmin = NULL ;
     }
      /* ------------------------------------ */

      c1max = ptdelay->CDRIVERMAX ;
      rmax  = ptdelay->RDRIVERMAX ;
      c1min = ptdelay->CDRIVERMIN ;
      rmin  = ptdelay->RDRIVERMIN ;
      delta= ttv_calclinedelayslope( line,
                                     ptdelay->FMIN, ptdelay->FMAX,
                                     pwlmin,        pwlmax,
                                     def_vt,        def_vt,
                                     def_vf,        def_vf,
                                     def_vdd,       def_vdd,
                                     def_vth,       def_vth,
                                     capa,
                                     ptdmin, ptfmin, ptptpwlmin,
                                     ptdmax, ptfmax, ptptpwlmax,
                                     mode,
                                     def_vsat, def_rlin,
                                     &rmin,&c1min,&c2min,
                                     def_vsat, def_rlin,
                                     &rmax,&c1max,&c2max,NULL,NULL
                                   ) ;

      deltamax = delta > deltamax ? delta : deltamax;

      model = stm_getmodel(line->FIG->INFO->FIGNAME,line->MDMAX) ;
     }

   ret = 0;
   if((line->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ)
       ret = ttv_calcnodedelay(line,type,dmax,fmax,ptpwlmax,dmin,fmin,ptpwlmin);

   /* Récupère les paramètres de la transition qui a donnée le pire cas.
      La prise en compte des paramètres sur le chemin le plus long n'est pas 
      réalisée. Il faudrait le faire dans ttv_calcnodedelay().
   */

   if( ret & TTV_PROP_MIN ) { 
     vfmin  = fmin ;
     vrmin  = rmin ;
     vc1min = c1min ;
     vc2min = c2min ;
     if( model ) {
       vt_min   = stm_mod_vt(model);
       vf_min   = stm_mod_vf(model);
       vdd_min  = stm_mod_vdd(model);
       vsat_min = stm_mod_vsat(model);
       rlin_min = stm_mod_rlin(model);
       vth_min  = stm_mod_vth(model);
       delay->RDRIVERMIN = rlin_min;
     }
     else {
       vt_min   = def_vt ;
       vf_min   = def_vf ;
       vdd_min  = def_vdd ;
       vsat_min = def_vsat ;
       rlin_min = def_rlin ;
       vth_min  = def_vth ;
       delay->RDRIVERMIN = -1;
     }
     if( rmin > 0.0 && c2min > 0.0 )
       delay->CDRIVERMIN = 1000.0*(c1min+c2min) ;
     else
       delay->CDRIVERMIN = 1000.0*c1min ;
   }
   if( ret & TTV_PROP_MAX ) {
     vfmax  = fmax ;
     vrmax  = rmax ;
     vc1max = c1max ;
     vc2max = c2max ;
     if( model ) {
       vt_max   = stm_mod_vt(model);
       vf_max   = stm_mod_vf(model);
       vdd_max  = stm_mod_vdd(model);
       vsat_max = stm_mod_vsat(model);
       rlin_max = stm_mod_rlin(model);
       vth_max  = stm_mod_vth(model);
       delay->RDRIVERMAX = rlin_max;
     }
     else {
       vt_max   = def_vt ;
       vf_max   = def_vf ;
       vdd_max  = def_vdd ;
       vsat_max = def_vsat ;
       rlin_max = def_rlin ;
       vth_max  = def_vth ;
       delay->RDRIVERMAX = -1;
     }
     if( rmax > 0.0 && c2max > 0.0 )
       delay->CDRIVERMAX = 1000.0*(c1max+c2max) ;
     else
       delay->CDRIVERMAX = 1000.0*c1max ;
   }

   if( flagcreatedelay == 1 ) 
     ttv_delnodedelay( line->NODE );
  }

 for(; chain != NULL ; chain = chain->NEXT)
  {
   line = (ttvline_list *)chain->DATA ;

   if(((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
      (ttv_islinelevel(ttvfig,line,level) == 0))
    continue ;

   ptdelay = ttv_getnodedelay(line->ROOT) ;

   if(ptdelay == NULL)
    {
     ptdelay = ttv_addnodedelay(line->ROOT,TTV_NOTIME, TTV_NOTIME, TTV_NOSLOPE, TTV_NOSLOPE) ;
    }

   if((type & TTV_FIND_MIN) == TTV_FIND_MIN) {
     ptdelay->RDRIVERMIN = delay->RDRIVERMIN ;
     ptdelay->CDRIVERMIN = delay->CDRIVERMIN ;
   }
   if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
     ptdelay->RDRIVERMAX = delay->RDRIVERMAX ;
     ptdelay->CDRIVERMAX = delay->CDRIVERMAX ;
   }

   if( delay->FMIN == TTV_NOSLOPE ) delay->FMIN = STM_DEF_SLEW * TTV_UNIT ;
   if( delay->FMAX == TTV_NOSLOPE ) delay->FMAX = STM_DEF_SLEW * TTV_UNIT ;

   if(((type & TTV_FIND_LINETOP) == TTV_FIND_LINETOP) &&
      (line->FIG != ttvfig))
     {
      if((type & TTV_FIND_MIN) == TTV_FIND_MIN)
       {
        dmin = ttv_getdelaymin( line );
        fmin = ttv_getslopemin( line );
       }

      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
       {
        dmax = ttv_getdelaymax( line );
        fmax = ttv_getslopemax( line );
       }
       ptpwlmin = NULL ;
       ptpwlmax = NULL ;
     }
    else {
     /* ------------------------------------ */
     /* Get PWL Slope                        */
     /* ------------------------------------ */
     pwlmin = NULL;
     pwlmax = NULL;
     ptpwlmin = NULL;
     ptpwlmax = NULL;
     if (V_BOOL_TAB[__STM_USE_MSC].VALUE ) {
         if ((ptype = getptype (delay->USER, STM_SLOPEMIN_PWL_PTYPE)))
             pwlmin = (stm_pwl*)ptype->DATA;
         if ((ptype = getptype (delay->USER, STM_SLOPEMAX_PWL_PTYPE)))
             pwlmax = (stm_pwl*)ptype->DATA;
         ptptpwlmax = &ptpwlmax ;
         ptptpwlmin = &ptpwlmin ;
     }
     else {
         ptptpwlmax = NULL ;
         ptptpwlmin = NULL ;
     }
     /* ------------------------------------ */
      delta = ttv_calclinedelayslope( line,
                                      delay->FMIN, delay->FMAX,
                                      pwlmin, pwlmax,
                                      vt_min, vt_max,
                                      vf_min, vf_max,
                                      vdd_min, vdd_max,
                                      vth_min, vth_max, 
                                      capa,
                                      ptdmin, ptfmin, ptptpwlmin, ptdmax, ptfmax, ptptpwlmax,
                                      mode, 
                                      vsat_min, rlin_min, 
                                      &vrmin, &vc1min, &vc2min,
                                      vsat_max, rlin_max, 
                                      &vrmax, &vc1max, &vc2max,NULL,NULL
                                    ) ;

      deltamax = delta > deltamax ? delta : deltamax;
     }

   ttv_calcnodedelay(line,type,dmax,fmax,ptpwlmax,dmin,fmin,ptpwlmin);
  }

  return deltamax;
}

/*****************************************************************************/
/*                        function ttv_calcnodeconstraint ()                 */
/*                                                                           */
/* calcul les delays et les fronts d'un noeud                                */
/* Renvoie la valeur absolue de la variation maximum des fronts sur les      */
/* line concernés.                                                           */
/*****************************************************************************/
long ttv_calcnodeconstraint(ttvfig,level,node,type,mode)
ttvfig_list *ttvfig ;
long level ;
ttvevent_list *node ;
long type ;
char mode ;
{
 ttvline_list *line ;
 ttvdelay_list *ptdelay = NULL;
 long delaymax ;
 long delaymin ;
 long slopemax ;
 long slopemin ;
 long slopeckmax ;
 long slopeckmin ;

 if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                  TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
   line = node->INLINE ;
  }
 else
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                  TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
   line = node->INPATH ;
  }

 for(; line != NULL ; line = line->NEXT)
  {
   if(((line->TYPE & (TTV_LINE_U|TTV_LINE_O)) == 0) ||
      ((line->TYPE & TTV_LINE_A) == TTV_LINE_A) ||
      (line->FIG->INFO->LEVEL < level))
     continue ;

   ptdelay = ttv_getnodedelay(line->ROOT) ;
   
   if(ptdelay == NULL)
    {
     ptdelay = ttv_addnodedelay(line->ROOT,TTV_NOTIME, TTV_NOTIME, TTV_NOSLOPE, TTV_NOSLOPE) ;
    }

   if( ptdelay->FMIN == TTV_NOSLOPE ) 
       slopeckmin = STM_DEF_SLEW;
   else
       slopeckmin = ptdelay->FMIN / TTV_UNIT;
   if( ptdelay->FMAX == TTV_NOSLOPE ) 
       slopeckmax = STM_DEF_SLEW;
   else
       slopeckmax = ptdelay->FMAX / TTV_UNIT;


   ptdelay = ttv_getnodedelay(line->NODE) ;
   
   if(ptdelay == NULL)
    {
     ptdelay = ttv_addnodedelay(line->NODE,TTV_NOTIME, TTV_NOTIME, TTV_NOSLOPE, TTV_NOSLOPE) ;
    }

   if( ptdelay->FMIN == TTV_NOSLOPE )
       slopemin = STM_DEF_SLEW;
   else
       slopemin = ptdelay->FMIN / TTV_UNIT;
       
   if( ptdelay->FMAX == TTV_NOSLOPE )
       slopemax = STM_DEF_SLEW;
   else
       slopemax = ptdelay->FMAX / TTV_UNIT;

   if((type & TTV_FIND_MAX) == TTV_FIND_MAX){
     if(line->MDMAX)
       delaymax = (long)(stm_getconstraint(line->FIG->INFO->FIGNAME,line->MDMAX,
                                      (float)slopemax,(float)slopeckmax) * TTV_UNIT + 0.5) ;
     else
       delaymax = line->VALMAX;
   }
   else
     delaymax = TTV_NOTIME ;

   if((type & TTV_FIND_MIN) == TTV_FIND_MIN){
     if(line->MDMIN)
       delaymin = (long)(stm_getconstraint(line->FIG->INFO->FIGNAME,line->MDMIN,
                                      (float)slopemin,(float)slopeckmin) * TTV_UNIT + 0.5) ;
     else
       delaymin = line->VALMIN;
   }
   else
     delaymin = TTV_NOTIME ;

   if(mode == TTV_MODE_LINE)
    {
     if(delaymax) 
         line->VALMAX = delaymax ;
     if(delaymin) 
         line->VALMIN = delaymin ;
     line->FMAX = (long)0 ;
     line->FMIN = (long)0 ;
    }
   else if(mode == TTV_MODE_DELAY)
    {
     ttv_addlinedelay(line,delaymax,delaymin,(long)0,(long)0,TTV_NOCAPA,TTV_NOCAPA) ;
    }
  }

  return 0l;
}

/*****************************************************************************/
/*                        function ttv_getdelaynode ()                       */
/*                                                                           */
/* calcul les delays et les fronts d'un noeud                                */
/*****************************************************************************/
long ttv_getdelaynode(ttvfig,level,node,type)
ttvfig_list *ttvfig ;
long level ;
ttvevent_list *node ;
long type ;
{
 ttvline_list *line ;
 chain_list *chain ;
 ptype_list *ptype ;
 long delay = TTV_NOTIME ;
 long curdelay ;

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL,
                  TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
   line = node->INLINE ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }
 else
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL,
                  TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
   line = node->INPATH ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }

  if((type & TTV_FIND_GATE) == TTV_FIND_GATE)
   {
    for(; line != NULL ; line = line->NEXT)
     {
      if(((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) ||
         (line->FIG->INFO->LEVEL < level))
        continue ;

      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
       {
        curdelay = ttv_getdelaymax(line) ;
        if((delay == TTV_NOTIME) || (delay > curdelay))
          delay = curdelay ;
       }
      else
       {
        curdelay = ttv_getdelaymin(line) ;
        if((delay == TTV_NOTIME) || (delay < curdelay))
          delay = curdelay ;
       }
     }
   if(delay == TTV_NOTIME)
     delay = (long)0 ;
  }
 else
  {
   for(; chain != NULL ; chain = chain->NEXT)
    {
     line = (ttvline_list *)chain->DATA ;
  
     if(((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
        (line->FIG->INFO->LEVEL < level))
      continue ;

     if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
      {
       curdelay = ttv_getdelaymax(line) ;
       if((delay == TTV_NOTIME) || (delay > curdelay))
         delay = curdelay ;
      }
     else
      {
       curdelay = ttv_getdelaymin(line) ;
       if((delay == TTV_NOTIME) || (delay < curdelay))
         delay = curdelay ;
      }
    }
   if(delay == TTV_NOTIME)
     delay = (long)0 ;
  }

 return(delay) ;
}

/*****************************************************************************/
/*                        function ttv_getslopenode ()                       */
/*                                                                           */
/* calcul les fronts d'un noeud.                                             */
/*****************************************************************************/
long ttv_getslopenode(ttvfig,level,node,type, which)
ttvfig_list   *ttvfig ;
long           level ;
ttvevent_list *node ;
long           type ;
long           which ;
{
 ttvline_list *line ;
 chain_list *chain ;
 ptype_list *ptype ;
 long slope = TTV_NOTIME ;
 long curslope ;

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL,
                  TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
   line = node->INLINE ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }
 else
  {
   ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL,
                  TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
   line = node->INPATH ;
   if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
      chain = (chain_list *)ptype->DATA ;
   else
      chain = NULL ;
  }

  if((type & TTV_FIND_GATE) == TTV_FIND_GATE)
   {
    for(; line != NULL ; line = line->NEXT)
     {
      if(((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) ||
         (line->FIG->INFO->LEVEL < level))
        continue ;

      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
       {
        if( which == TTV_MODE_LINE )
          curslope = line->FMAX;
        else
          curslope = ttv_getslopemax(line) ;
        if((slope == TTV_NOTIME) || (curslope > slope))
          slope = curslope ;
       }
      else
       {
        if( which == TTV_MODE_LINE )
          curslope = line->FMIN ;
        else
          curslope = ttv_getslopemin(line) ;
        if((slope == TTV_NOTIME) || (curslope < slope))
          slope = curslope ;
       }
     }
  }

  if((type & TTV_FIND_RC) == TTV_FIND_RC)
  {
   for(; chain != NULL ; chain = chain->NEXT)
    {
     line = (ttvline_list *)chain->DATA ;
  
     if(((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
        (line->FIG->INFO->LEVEL < level))
      continue ;

     if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
      {
       if( which == TTV_MODE_LINE )
         curslope = line->FMAX;
       else
         curslope = ttv_getslopemax(line) ;
       if((slope == TTV_NOTIME) || (curslope > slope))
         slope = curslope ;
      }
     else
      {
        if( which == TTV_MODE_LINE )
          curslope = line->FMIN ;
        else
         curslope = ttv_getslopemin(line) ;
       if((slope == TTV_NOTIME) || (curslope < slope))
         slope = curslope ;
      }
    }
  }

 if(slope == TTV_NOTIME)
   slope = (long)0 ;
 return(slope) ;
}

/*****************************************************************************/
/*                        function ttv_depthfirst ()                         */
/*                                                                           */
/* recherche les noeuds du graphe en profondeur                              */
/*****************************************************************************/
static chain_list *thisappend(chain_list *head, chain_list *last, chain_list *old)
{
  if (last==NULL) return old;
  
  last->NEXT=old;  
  return head;
}

static chain_list *ttv_depthfirst(ttvfig_list *fig,long level,ttvevent_list *node,long status,long file,long type, int depth, chain_list **last)
{
 ttvevent_list *nodex;
 ttvline_list *in;
 chain_list *chainres = NULL, *thislast, *head;

 *last=NULL;
 if((node->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
  return (NULL);

 node->TYPE |= TTV_NODE_MARQUE;
  
 depth++;
 ttv_expfigsig (fig,node->ROOT,level,fig->INFO->LEVEL,status,file);

 if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
  in = node->INPATH;
 else
  in = node->INLINE;

 ttv_fifopush(node) ;

 while (in)
  {
   nodex = in->NODE;

   ttv_fifopush(nodex) ;

   if (((in->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT && (in->TYPE & TTV_LINE_A)!=TTV_LINE_A) ||
      (((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
      ((in->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
      (ttv_islineonlyend(fig,in,type) == 1)) ||
       (((in->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
       (in->FIG != fig)) ||
      (((in->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) && (in->FIG != fig)
       && (fig->INFO->LEVEL < level)))
	{
	 in = in->NEXT;
	 continue;
	}

   if (((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) && (node->FIND->OUTLINE))
	{
	 if ((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
	  {
	   in = in->NEXT;
	   continue;
	  }
	}

   if(((nodex->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L) ||
      ((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
      (((nodex->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT) &&
       (ttv_islocononlyend(fig,nodex,type) != 0)))
	{
      if ((nodex->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT || depth>10000)
      {
	    in = in->NEXT;
	    continue;
      }
	}

   nodex->FIND->OUTLINE = in;
   head=ttv_depthfirst(fig,level,nodex,status,file,type,depth,&thislast);
   if (chainres==NULL) *last=thislast;
   chainres=thisappend(head, thislast, chainres);
//   chainres = append(ttv_depthfirst(fig,level,nodex,status,file,type,depth,&thislast),chainres);
   
   nodex->FIND->OUTLINE = NULL;

   in = in->NEXT;
  }

 depth--;
 chainres = addchain (chainres, (void *) node);
 if (chainres->NEXT==NULL) *last=chainres;

 return (chainres);
}

/*****************************************************************************/
/*                        function ttv_levelise ()                           */
/*                                                                           */
/* recherche les noeuds du graphe en profondeur                              */
/*****************************************************************************/
chain_list *ttv_levelise(ttvfig,level,type)
ttvfig_list *ttvfig;
long level ;
long type ;
{
 ttvfig_list *ttvins;
 ttvsig_list *ptsig;
 chain_list *chainnode = NULL;
 chain_list *chainsig ;
 chain_list *chain, *thislast, *head;
 ttvevent_list *event;
 long status ; 
 long file ;

 if(ttvfig->INFO->LEVEL == level)
   ttvins = ttvfig ;
 else
   ttvins = NULL ;

 if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
  {
   status = TTV_STS_CL_PJT;
   file = TTV_FILE_TTX;
  }
 else
  {
   status = TTV_STS_CLS_FED;
   file = TTV_FILE_DTX;
  }

 chainsig = ttv_getsigbytype(ttvfig, ttvins, TTV_SIG_C, NULL) ;

 for (chain = chainsig ; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode); 
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE+1, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
  }

 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig, ttvins, TTV_SIG_L, NULL) ;

 for (chain = chainsig ; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE+1, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
  }

 freechain(chainsig) ;


 chainsig = ttv_getsigbytype(ttvfig, ttvins, TTV_SIG_Q, NULL) ;

 for (chain = chainsig ; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE+1, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
  }

 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig, ttvins, TTV_SIG_B, NULL) ;

 for (chain = chainsig ; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode); 
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE+1, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
  }

 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig, ttvins, TTV_SIG_R, NULL) ;

 for (chain = chainsig ; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode); 
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE+1, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
  }

 freechain(chainsig) ;

 if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
 chainsig = ttv_getsigbytype(ttvfig,ttvins,TTV_SIG_N|TTV_SIG_I,NULL) ;
 else
 chainsig = ttv_getsigbytype(ttvfig,ttvins,TTV_SIG_N|TTV_SIG_I|TTV_SIG_S,NULL) ;


 for (chain = chainsig ; chain; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *) chain->DATA;
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode); 
   head=ttv_depthfirst (ttvfig,level,ptsig->NODE+1, status, file,type,0,&thislast);
   chainnode=thisappend(head, thislast, chainnode);
  }

 freechain(chainsig) ;

 chainnode = reverse (chainnode);

 for (chain = chainnode; chain; chain = chain->NEXT)
  {
   event = (ttvevent_list *) chain->DATA;
   event->TYPE &= ~(TTV_NODE_MARQUE);
  }

 ttv_fifoclean() ;

 return (chainnode);
}

/*****************************************************************************/
/*                        function ttv_calcfigdelay ()                       */
/*                                                                           */
/* recherche les noeuds du graphe en profondeur                              */
/*****************************************************************************/

chain_list *ttv_calcfigdelay(ttvfig_list *ttvfig,chain_list *chainnode,long level,long type, char mode, int (*init_func)(ttvevent_list *, void *), int (*end_func)(ttvevent_list *, void *), void *data, int noconstraint)
{
 chain_list *chain;
 ttvevent_list *event;

 if(chainnode == NULL)
  {
   chainnode = ttv_levelise(ttvfig,level,type) ;
  }

 for (chain = chainnode; chain; chain = chain->NEXT)
  {
   event = (ttvevent_list *) chain->DATA;
   if (init_func==NULL || init_func(event, data)==1)
     ttv_calcnodedelayslope(ttvfig,level,event,type,mode) ;
   else
     ttv_updatenodedelayslope( ttvfig,level,event,type,mode );
   if (end_func!=NULL) end_func(event, data);
  }
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("after calcnodedelayslope:",0);
#endif
 if (!noconstraint)
 {  
   for (chain = chainnode; chain; chain = chain->NEXT)
   {
    event = (ttvevent_list *) chain->DATA;
    ttv_calcnodeconstraint(ttvfig,level,event,type,mode) ;
   }
#ifdef DELAY_DEBUG_STAT
  mbk_debugstat("after calcnodeconstraint:",0);
#endif
 }
 return(chainnode) ;
}

/*****************************************************************************/
/*                        function ttv_cleanfigmodel ()                      */
/*                                                                           */
/* met a jour les model dans la ttvfig et dans stm                           */
/*****************************************************************************/
void ttv_cleanfigmodel(ttvfig,keep,clean)
ttvfig_list *ttvfig;
long keep ;
long clean ;
{
 ttvlbloc_list *ptlbloc[6] ;
 ttvline_list *ptline ;
 ht *htab ;
 long i ;
 long j ;
 long nb = (long)0 ;

 if(((keep & TTV_LINE_D) == TTV_LINE_D) ||
    ((clean & TTV_LINE_D) == TTV_LINE_D))
  {
   ptlbloc[0] = ttvfig->DBLOC ;
   nb += ttvfig->NBDBLOC ;
  }
 else
  {
   ptlbloc[0] = NULL ;
  }

 if(((keep & TTV_LINE_E) == TTV_LINE_E) ||
    ((clean & TTV_LINE_E) == TTV_LINE_E))
  {
   ptlbloc[1] = ttvfig->EBLOC ;
   nb += ttvfig->NBEBLOC ;
  }
 else
   ptlbloc[1] = NULL ;

 if(((keep & TTV_LINE_F) == TTV_LINE_F) ||
    ((clean & TTV_LINE_F) == TTV_LINE_F))
  {
   ptlbloc[2] = ttvfig->FBLOC ;
   nb += ttvfig->NBFBLOC ;
  }
 else
   ptlbloc[2] = NULL ;

 if(((keep & TTV_LINE_T) == TTV_LINE_T) ||
    ((clean & TTV_LINE_T) == TTV_LINE_T))
  {
   ptlbloc[3] = ttvfig->TBLOC ;
   nb += ttvfig->NBTBLOC ;
  }
 else
   ptlbloc[3] = NULL ;

 if(((keep & TTV_LINE_J) == TTV_LINE_J) ||
    ((clean & TTV_LINE_J) == TTV_LINE_J))
  {
   ptlbloc[4] = ttvfig->JBLOC ;
   nb += ttvfig->NBJBLOC ;
  }
 else
   ptlbloc[4] = NULL ;

 if(((keep & TTV_LINE_P) == TTV_LINE_P) ||
    ((clean & TTV_LINE_P) == TTV_LINE_P))
  {
   ptlbloc[5] = ttvfig->PBLOC ;
   nb += ttvfig->NBPBLOC ;
  }
 else
   ptlbloc[5] = NULL ;

 if(nb != (long)0)
   htab = addht(nb) ;
 else
   return ;

 for(i = 0 ; i < 6 ; i++)
 for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
   {
    for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
     {
      ptline = ptlbloc[i]->LINE + j ;
      if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
        continue ;
      if((ptline->TYPE & keep) != 0)
        {
         if(ptline->MDMAX != NULL)
           sethtitem(htab,ptline->MDMAX,(long)1) ;
         if(ptline->MDMIN != NULL)
           sethtitem(htab,ptline->MDMIN,(long)1) ;
         if((ptline->MFMAX != NULL) && (ptline->MDMAX != ptline->MFMAX))
           sethtitem(htab,ptline->MFMAX,(long)1) ;
         if((ptline->MFMIN != NULL) && (ptline->MDMIN != ptline->MFMIN))
           sethtitem(htab,ptline->MFMIN,(long)1) ;
        }
      else if((ptline->TYPE & clean) != 0)
        {
         ptline->MDMAX = NULL ;
         ptline->MDMIN = NULL ;
         ptline->MFMAX = NULL ;
         ptline->MFMIN = NULL ;
        }
     }
   }

 stm_cleanfigmodel(ttvfig->INFO->FIGNAME,htab) ;

 delht(htab) ;
}

/*****************************************************************************/
/*                        function ttv_movedelayline ()                      */
/*                                                                           */
/* met a jour les model dans la ttvfig et dans stm                           */
/*****************************************************************************/
void ttv_movedelayline(ttvfig,type)
ttvfig_list *ttvfig;
long type ;
{
 ttvlbloc_list *ptlbloc[6] ;
 ttvline_list *ptline ;
 long i ;
 long j ;

 if((type & TTV_LINE_D) == TTV_LINE_D)
   ptlbloc[0] = ttvfig->DBLOC ;
 else
   ptlbloc[0] = NULL ;

 if((type & TTV_LINE_E) == TTV_LINE_E)
   ptlbloc[1] = ttvfig->EBLOC ;
 else
   ptlbloc[1] = NULL ;

 if((type & TTV_LINE_F) == TTV_LINE_F)
   ptlbloc[2] = ttvfig->FBLOC ;
 else
   ptlbloc[2] = NULL ;

 if((type & TTV_LINE_T) == TTV_LINE_T)
  {
   ptlbloc[3] = ttvfig->TBLOC ;
  }
 else
   ptlbloc[3] = NULL ;

 if((type & TTV_LINE_J) == TTV_LINE_J)
  {
   ptlbloc[4] = ttvfig->JBLOC ;
  }
 else
   ptlbloc[4] = NULL ;

 if((type & TTV_LINE_P) == TTV_LINE_P)
  {
   ptlbloc[5] = ttvfig->PBLOC ;
  }
 else
   ptlbloc[5] = NULL ;

 for(i = 0 ; i < 6 ; i++)
 for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
   {
    for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
     {
      ptline = ptlbloc[i]->LINE + j ;
      if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
        continue ;
      ptline->VALMAX =  ttv_getdelaymax(ptline) ;
      ptline->FMAX = ttv_getslopemax(ptline) ;
      ptline->VALMIN =  ttv_getdelaymin(ptline) ;
      ptline->FMIN = ttv_getslopemin(ptline) ;
     }
   }
 ttv_freettvfigdelay(ttvfig) ;
}

/*****************************************************************************/
/*                        function ttv_freefigdelay ()                       */
/*                                                                           */
/* recherche les noeuds du graphe en profondeur                              */
/*****************************************************************************/
void ttv_freefigdelay(chainnode)
chain_list *chainnode ;
{
 chain_list *chain;

 for (chain = chainnode; chain; chain = chain->NEXT)
  {
   ttv_delnodedelay((ttvevent_list *) chain->DATA) ;
  }
}

/*****************************************************************************/
/*                        function ttvenv ()                                 */
/*                                                                           */
/* chargement d'une cellule                                                  */
/*****************************************************************************/
ttvfig_list *ttv_readcell_TTVFIG;

void ttv_readcell(char *name)
{
 ttvfig_list *ttvfig ;
 ttvsig_list *ptsig ;
 lofig_list *lofig ;
 losig_list *losig ;
 long i ;
 long j = 1 ;
 long type ;
 char dir ;
 
 ttv_readcell_TTVFIG=ttvfig = ttv_getttvfig(name, 0) ;

 if(ttvfig == NULL)
   return ;

 ttv_lockttvfig(ttvfig) ;

 if((lofig = getloadedlofig(ttvfig->INFO->FIGNAME)) == NULL)
  {
   lofig = addlofig(ttvfig->INFO->FIGNAME) ;

   if(ttvfig->NBCONSIG != 0)
     {                     
      for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
        { 
         ptsig = *(ttvfig->CONSIG + i) ;
         losig = addlosig(lofig, j++, addchain(NULL,ptsig->NAME), EXTERNAL) ; 
         type = ptsig->TYPE & TTV_SIG_TYPECON ;
         switch(type)
           {
            case TTV_SIG_CI : dir = IN ;
            case TTV_SIG_CO : dir = OUT ;
            case TTV_SIG_CZ : dir = TRISTATE ;
            case TTV_SIG_CB : dir = INOUT ;
            case TTV_SIG_CT : dir = TRANSCV ;
            case TTV_SIG_CX : dir = UNKNOWN ;
           }
         addlocon(lofig,ptsig->NAME,losig,dir) ;
        }
     }
  }

 addcatalog(lofig->NAME) ;
 locklofig(lofig) ;
}

/*****************************************************************************/
/*                        function ttvenv ()                                 */
/*                                                                           */
/* chargement des bibliothèques                                              */
/*****************************************************************************/
void ttvenv()
{
 readlibfile("ttv", ttv_readcell, 0);
 
}

int ttv_SetPrecisionLevel(int val)
{
  int old=V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE;
  V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=val;
  return old;
}

/*****************************************************************************/
/*                        function ttv_isemptyttvins()                       */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* verifie q'une figure est une black box                                    */
/*****************************************************************************/
int ttv_isemptyttvins(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 long i ;
 ttvfig_list *ttvfigx ;

 if((ttvfig->INS != NULL) ||
    (ttvfig->NBILCMDSIG != (long)0) ||
    (ttvfig->NBILATCHSIG != (long)0) ||
    (ttvfig->NBIBREAKSIG != (long)0) ||
    (ttvfig->NBIPRESIG != (long)0) ||
    (ttvfig->NBEXTSIG != (long)0) ||
    (ttvfig->NBINTSIG != (long)0) ||
    (ttvfig->NBESIG != (long)0) ||
    (ttvfig->NBISIG != (long)0) ||
    (ttvfig->NBPBLOC != (long)0) ||
    (ttvfig->NBJBLOC != (long)0) ||
    (ttvfig->NBFBLOC != (long)0) ||
    (ttvfig->NBEBLOC != (long)0))
  return(0) ;

 for(i = 0 ; i < ttvfig->NBELCMDSIG ; i++)
  if(((*(ttvfig->ELCMDSIG + i))->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    return(0) ;

 for(i = 0 ; i < ttvfig->NBELATCHSIG ; i++)
  if(((*(ttvfig->ELATCHSIG + i))->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    return(0) ;

 for(i = 0 ; i < ttvfig->NBEBREAKSIG ; i++)
  if(((*(ttvfig->EBREAKSIG + i))->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    return(0) ;

 for(i = 0 ; i < ttvfig->NBEPRESIG ; i++)
  if(((*(ttvfig->EPRESIG + i))->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    return(0) ;

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   if((ttvfig->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
    {
     ttvfigx = ttvfig ;
    }
   else
    {
     ttvfigx = ttvfig->INFO->MODEL ;
    }

   ttv_parsttvfig(ttvfigx,TTV_STS_CL|TTV_STS_S,TTV_FILE_DTX) ;
   ttv_freememoryiffull(ttvfigx,TTV_STS_S) ;

   if(ttvfigx->NBISIG != (long)0)
      return(0) ;
  }

 return(1) ;
}

/*****************************************************************************/
/*                        function ttv_copyttvinsinttvfig()                  */
/*                                                                           */
/* mise à plat d'une ttvfig recursivement                                    */
/*****************************************************************************/
chain_list *ttv_copyttvinsinttvfig(ttvfig,ttvins,type)
ttvfig_list *ttvfig ;
ttvfig_list *ttvins ;
long type ;
{
 ttvsig_list *ptsig ;
 ttvsig_list *ptsignew ;
 ttvevent_list *event ;
 ttvevent_list *cmd ;
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ttvevent_list *node ;
 ttvevent_list *root ;
 ttvsbloc_list *ptsbloc ;
 ttvlbloc_list *ptlbloc[4] ;
 chain_list *chainsig ;
 chain_list *chain ;
 chain_list *chainres = NULL ;
 ptype_list *ptype ;
 char buf1[1024] ;
 char buf2[1024] ;
 char lock = 'N' ;
 long i ;
 long j ;
 long nbend ;
 long typeline ;

 if((ttvins->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK)
   {
    ttv_lockttvfig(ttvins) ;
    lock = 'Y' ;
   }

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   ttv_parsttvfig(ttvins,TTV_STS_LS_FE,TTV_FILE_DTX) ;
   ptlbloc[0] = ttvins->EBLOC ;
   ptlbloc[1] = ttvins->FBLOC ;
  }
 else
  {
   ptlbloc[0] = NULL ;
   ptlbloc[1] = NULL ;
  }

 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   ttv_parsttvfig(ttvins,TTV_STS_L_PJ,TTV_FILE_TTX) ;
   ptlbloc[2] = ttvins->PBLOC ;
   ptlbloc[3] = ttvins->JBLOC ;
  }
 else
  {
   ptlbloc[2] = NULL ;
   ptlbloc[3] = NULL ;
  }

 chainsig = ttv_getsigbytype(ttvins,ttvins,TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_I,NULL) ;

 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
   {
    ptsig = (ttvsig_list *)chain->DATA ;
    if((ptsig->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0)
       continue ;
    if(ptsig->ROOT == ttvins)
     {
      switch(ptsig->TYPE & TTV_SIG_TYPEALL)
      {
       case TTV_SIG_Q : ttvfig->ELCMDSIG = (ttvsig_list **)ttv_addrefsig(
                        ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                        ttv_getnetname(ttvfig,buf2,ptsig),
                        ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE,
                        (chain_list *)ttvfig->ELCMDSIG) ;
                        ptsignew = ((chain_list *)ttvfig->ELCMDSIG)->DATA ;
                       break ;
       case TTV_SIG_L : ttvfig->ELATCHSIG = (ttvsig_list **)ttv_addrefsig(
                        ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                        ttv_getnetname(ttvfig,buf2,ptsig),
                        ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE,
                        (chain_list *)ttvfig->ELATCHSIG) ;
                        ptsignew = ((chain_list *)ttvfig->ELATCHSIG)->DATA ;
                       break ;
       case TTV_SIG_R : ttvfig->EPRESIG = (ttvsig_list **)ttv_addrefsig(
                        ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                        ttv_getnetname(ttvfig,buf2,ptsig),
                        ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE,
                        (chain_list *)ttvfig->EPRESIG) ;
                        ptsignew = ((chain_list *)ttvfig->EPRESIG)->DATA ;
                       break ;
       case TTV_SIG_B : ttvfig->EBREAKSIG = (ttvsig_list **)ttv_addrefsig(
                        ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                        ttv_getnetname(ttvfig,buf2,ptsig),
                        ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE,
                        (chain_list *)ttvfig->EBREAKSIG) ;
                        ptsignew = ((chain_list *)ttvfig->EBREAKSIG)->DATA ;
                       break ;
       case TTV_SIG_I : ttvfig->EXTSIG = (ttvsig_list **)ttv_addrefsig(
                        ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                        ttv_getnetname(ttvfig,buf2,ptsig),
                        ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE,
                        (chain_list *)ttvfig->EXTSIG) ;
                        ptsignew = ((chain_list *)ttvfig->EXTSIG)->DATA ;
                       break ;
      }
      chainres = addchain(chainres,ptsig) ;
      ptsig->USER = addptype(ptsig->USER,TTV_SIG_NEW,ptsignew) ;
      ttv_setsigflag(ptsignew, ttv_testsigflag(ptsig, 0xffffffff));
     }
   }

 freechain(chainsig) ;

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
   {
    for(i = 0 ; i < ttvins->NBESIG ; i++)
      {
       ptsig = *(ttvins->ESIG + i) ;
       if(ptsig->ROOT == ttvins)
        {
         for(j = 0 ; j < 2 ; j ++)
           {
            event = ptsig->NODE + j ;
            for(ptline = event->INLINE ; ptline != NULL ; ptline = ptline->NEXT)
             if(((ptline->TYPE & TTV_LINE_D) == TTV_LINE_D) && 
                 (ptline->FIG == ttvfig))
                break ;
            if(ptline == NULL)
             {
              ptype = getptype(event->USER,TTV_NODE_DUALLINE) ;
              if(ptype != NULL)
                {
                 for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
                     chain = chain->NEXT)
                   {
                    ptline = (ttvline_list *)chain->DATA ;
                     if(((ptline->TYPE & TTV_LINE_D) == TTV_LINE_D) &&
                        (ptline->FIG == ttvfig))
                        break ;
                   }
                 if(chain == NULL)
                   ptline = NULL ;
                }
             }
            if(ptline != NULL)
              break ;
           }

         if(ptline == NULL)
           {
            ptsignew = ttv_addsig(ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                                  ttv_getnetname(ttvfig,buf2,ptsig),
                                  ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE) ;
           }
         else
           {
            ttvfig->ESIG = (ttvsig_list **)ttv_addrefsig(
                     ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                     ttv_getnetname(ttvfig,buf2,ptsig),
                     ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE,
                     (chain_list *)ttvfig->ESIG) ;
                     ptsignew = ((chain_list *)ttvfig->ESIG)->DATA ;
           }
         chainres = addchain(chainres,ptsig) ;
         ptsig->USER = addptype(ptsig->USER,TTV_SIG_NEW,ptsignew) ;
         ttv_setsigflag(ptsignew, ttv_testsigflag(ptsig, 0xffffffff));
        }
      }

     if(ttvins->NBISIG != 0)
      {
       ttvins->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvins->ISIG) ;
       nbend = ttvins->NBISIG ;
       ptsbloc = ttvins->ISIG ;
       for(i = 0 ; i < nbend ; i++)
         { 
          j = (TTV_MAX_SBLOC -1) - (i % TTV_MAX_SBLOC) ;
          if((j == (TTV_MAX_SBLOC - 1)) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
          ptsig = ptsbloc->SIG + j ;
          ptsignew = ttv_addsig(ttvfig,ttv_getsigname(ttvfig,buf1,ptsig),
                                ttv_getnetname(ttvfig,buf2,ptsig),
                                ptsig->CAPA,ptsig->TYPE & TTV_SIG_TYPE) ;
          ptsig->USER = addptype(ptsig->USER,TTV_SIG_NEW,ptsignew) ;
          ttv_setsigflag(ptsignew, ttv_testsigflag(ptsig, 0xffffffff));
         }
       ttvins->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvins->ISIG) ;
     }
   }

  for(j = 0 ; j < 4 ; j++)
   {
   for(; ptlbloc[j] != NULL ; ptlbloc[j] = ptlbloc[j]->NEXT)
   for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
    {
     ptline = ptlbloc[j]->LINE + i ;

     if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
        continue ;

     ptype = getptype(ptline->NODE->ROOT->USER,TTV_SIG_NEW) ;
     ptsignew = ptype->DATA ;
     if((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
      node = ptsignew->NODE + 1 ;
     else
      node = ptsignew->NODE ;

     ptype = getptype(ptline->ROOT->ROOT->USER,TTV_SIG_NEW) ;
     ptsignew = ptype->DATA ;
     if((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
       root = ptsignew->NODE + 1 ;
     else
       root = ptsignew->NODE ;

     typeline = ttv_getnewlinetype(ptline,ptline->NODE->ROOT,
                                          ptline->ROOT->ROOT) ;

     ptlinex = ttv_addline(ttvfig,root,node,ptline->VALMAX,ptline->FMAX,
                                  ptline->VALMIN,ptline->FMIN,typeline) ;

     ttv_calcaracline(ptline,ptlinex,TTV_FIND_MAX|TTV_FIND_MIN,-1) ;

     if((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
       {
        ptype = getptype(ptline->USER,TTV_LINE_CMDMAX) ;
        if(ptype != NULL)
         {
          cmd = (ttvevent_list *)ptype->DATA ;
          ttv_addcmd(ptlinex,TTV_LINE_CMDMAX,cmd) ;
         }
        ptype = getptype(ptline->USER,TTV_LINE_CMDMIN) ;
        if(ptype != NULL)
         {
          cmd = (ttvevent_list *)ptype->DATA ;
          ttv_addcmd(ptlinex,TTV_LINE_CMDMIN,cmd) ;
         }
       }
    }
  }

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
   {
     if(ttvins->NBISIG != 0)
      {
       ttvins->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvins->ISIG) ;
       nbend = ttvins->NBISIG ;
       ptsbloc = ttvins->ISIG ;
       for(i = 0 ; i < nbend ; i++)
         { 
          j = (TTV_MAX_SBLOC -1) - (i % TTV_MAX_SBLOC) ;
          if((j == (TTV_MAX_SBLOC - 1)) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
          ptsig = ptsbloc->SIG + j ;
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_NEW) ;
         }
       ttvins->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvins->ISIG) ;
      }
   }

 if(lock == 'Y')
   ttv_unlockttvfig(ttvins) ;

 return(chainres) ;
}

/*****************************************************************************/
/*                        function ttv_flatttvfigrec()                       */
/*                                                                           */
/* mise à plat d'une ttvfig recursivement                                    */
/*****************************************************************************/
chain_list  *ttv_flatttvfigrec(ttvfig,ttvinslist,type)
ttvfig_list *ttvfig ;
chain_list *ttvinslist ;
long type ;
{
 ttvfig_list *ttvins ;
 chain_list *chain ;
 chain_list *chainres = NULL ;

 for(chain = ttvinslist ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   chainres = append(ttv_flatttvfigrec(ttvfig,ttvins->INS,type),chainres) ;
   chainres = append(ttv_copyttvinsinttvfig(ttvfig,ttvins,type),chainres) ;
   ttv_freettvfigmemory(ttvins,TTV_STS_DTX|TTV_STS_TTX) ;
  }

 return(chainres) ;
}

/*****************************************************************************/
/*                        function ttv_flatttvfigfromlist()                  */
/*                                                                           */
/* mise à plat d'une ttvfig recursivement                                    */
/*****************************************************************************/
void ttv_flatttvfigfromlist(ttvfig,ttvinslist,type)
ttvfig_list *ttvfig ;
chain_list *ttvinslist ;
long type ;
{
 ttvsig_list *ptsig ;
 ttvsig_list *ptsignew ;
 ttvsig_list *ptsigcmd ;
 ttvevent_list *cmd ;
 ttvevent_list *cmdmax ;
 ttvevent_list *cmdmin ;
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ttvevent_list *node ;
 ttvevent_list *root ;
 ttvlbloc_list *ptlbloc[6] ;
 ptype_list *ptype ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainnext ;
 chain_list *chainres ;
 chain_list *chainsav ;
 chain_list *chaincmd ;
 chain_list **chaintab[6] ;
 long typeline ;
 int i ;
 int j ;

 if(ttvinslist == NULL)
    return ;

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
   ttv_allocdualline(ttvfig,TTV_STS_DUAL_D) ;

 chainres = ttv_flatttvfigrec(ttvfig,ttvinslist,type) ;
 
 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
   ttv_freedualline(ttvfig,TTV_STS_DUAL_D) ;

 chaintab[0] = ((chain_list **)&ttvfig->ELCMDSIG) ;
 chaintab[1] = ((chain_list **)&ttvfig->ELATCHSIG) ;
 chaintab[2] = ((chain_list **)&ttvfig->EBREAKSIG) ;
 chaintab[3] = ((chain_list **)&ttvfig->EPRESIG) ;
 chaintab[4] = ((chain_list **)&ttvfig->EXTSIG) ;
 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
   chaintab[5] = ((chain_list **)&ttvfig->ESIG) ;
 else
   chaintab[5] = NULL ;

 for(chain = chainres ; chain != NULL ; chain = chain->NEXT)
   {
    ptsig = (ttvsig_list *)chain->DATA ;

    if(getptype(ptsig->USER,TTV_SIG_CMD) != NULL)
     {
      if((ptype = getptype(ptsig->USER,TTV_SIG_NEW)) != NULL)
       {
        ptsignew = ptype->DATA ;
        chaincmd = ttv_getlrcmd(ttvfig,ptsig) ;
        for(chainx = chaincmd ; chainx != NULL ; chainx = chainx->NEXT)
          {
           cmd = (ttvevent_list *)chainx->DATA ;
           ptsig = cmd->ROOT ;
           if((ptype = getptype(ptsig->USER,TTV_SIG_NEW)) != NULL)
             {
              ptsigcmd = ptype->DATA ;
              if((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                  cmd = ptsigcmd->NODE + 1 ;
              else
                  cmd = ptsigcmd->NODE ;
              chainx->DATA = cmd ;
             }
          }
         if((ptype = getptype(ptsignew->USER,TTV_SIG_CMD)) != NULL)
           {
            freechain(ptype->DATA) ;
            ptsignew->USER = delptype(ptsignew->USER,TTV_SIG_CMD) ;
           }
         ptsignew->USER = addptype(ptsignew->USER,TTV_SIG_CMD,chaincmd) ;
       }
     }
   }

 for(i= 0 ; i < 6 ; i++)
 for(chain = *chaintab[i] ; chain != NULL ; chain = chainnext)
   {
    chainnext = chain->NEXT ;
    ptsig = (ttvsig_list *)chain->DATA ;
    if((ptype = getptype(ptsig->USER,TTV_SIG_NEW)) != NULL)
     {
      ptsignew = ptype->DATA ;

      if(chain == *chaintab[i])
       {
        *chaintab[i] = (*chaintab[i])->NEXT ;
       }
      else
       {
        chainsav->NEXT = chain->NEXT ;
       }
       chain->NEXT = NULL ;
       freechain(chain) ;
     }
    else
     {
      chainsav = chain ;
     }
   }

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   ptlbloc[0] = ttvfig->EBLOC ;
   ptlbloc[1] = ttvfig->FBLOC ;
   ptlbloc[2] = ttvfig->DBLOC ;
  }
 else
  {
   ptlbloc[0] = NULL ;
   ptlbloc[1] = NULL ;
   ptlbloc[2] = NULL ;
  }

 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   ptlbloc[3] = ttvfig->PBLOC ;
   ptlbloc[4] = ttvfig->JBLOC ;
   ptlbloc[5] = ttvfig->TBLOC ;
  }
 else
  {
   ptlbloc[3] = NULL ;
   ptlbloc[4] = NULL ;
   ptlbloc[5] = NULL ;
  }

  for(j = 0 ; j < 6 ; j++)
  for(; ptlbloc[j] != NULL ; ptlbloc[j] = ptlbloc[j]->NEXT)
   for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
    {
     ptline = ptlbloc[j]->LINE + i ;

     if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
        continue ;

     if((ptype = getptype(ptline->NODE->ROOT->USER,TTV_SIG_NEW)) != NULL)
       {
        ptsignew = ptype->DATA ;
        if((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         node = ptsignew->NODE + 1 ;
        else
         node = ptsignew->NODE ;
       }
     else node = ptline->NODE ;

     if((ptype = getptype(ptline->ROOT->ROOT->USER,TTV_SIG_NEW)) != NULL)
       {
        ptsignew = ptype->DATA ;
        if((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          root = ptsignew->NODE + 1 ;
        else
          root = ptsignew->NODE ;
       }
     else root = ptline->ROOT ;

     cmdmax = NULL ;
     cmdmin = NULL ;

     if((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
       {
        ptype = getptype(ptline->USER,TTV_LINE_CMDMAX) ;
        if(ptype != NULL)
         {
          cmd = (ttvevent_list *)ptype->DATA ;
          if((ptype = getptype(cmd->ROOT->USER,TTV_SIG_NEW)) != NULL)
            {
             ptsignew = ptype->DATA ;
             if((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
               cmdmax = ptsignew->NODE + 1 ;
             else
               cmdmax = ptsignew->NODE ;
             getptype(ptline->USER,TTV_LINE_CMDMAX)->DATA = cmdmax ;
            }
          else cmdmax = cmd ;
         }
        ptype = getptype(ptline->USER,TTV_LINE_CMDMIN) ;
        if(ptype != NULL)
         {
          cmd = (ttvevent_list *)ptype->DATA ;
          if((ptype = getptype(cmd->ROOT->USER,TTV_SIG_NEW)) != NULL)
            {
             ptsignew = ptype->DATA ;
             if((cmd->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
               cmdmin = ptsignew->NODE + 1 ;
             else
               cmdmin = ptsignew->NODE ;
             getptype(ptline->USER,TTV_LINE_CMDMIN)->DATA = cmdmin ;
            }
          else cmdmin = cmd ;
         }
       }

     if((node == ptline->NODE) && (root == ptline->ROOT))
      {
       continue ;
      }

     typeline = ttv_getlinetype(ptline) ;

     ptlinex = ttv_addline(ttvfig,root,node,ptline->VALMAX,ptline->FMAX,
                                  ptline->VALMIN,ptline->FMIN,typeline) ;

     ttv_addcaracline(ptlinex,ptline->MDMAX,ptline->MDMIN,ptline->MFMAX,ptline->MFMIN) ;

     if(cmdmax != NULL) ttv_addcmd(ptlinex,TTV_LINE_CMDMAX,cmdmax) ;
     if(cmdmin != NULL) ttv_addcmd(ptlinex,TTV_LINE_CMDMIN,cmdmin) ;

     ttv_delline(ptline) ;
    }

 for(chain = chainres ; chain != NULL ; chain = chain->NEXT)
   {
    ptsig = (ttvsig_list *)chain->DATA ;

    if(getptype(ptsig->USER,TTV_SIG_NEW) != NULL)
       ptsig->USER = delptype(ptsig->USER,TTV_SIG_NEW) ;
   }

 freechain(chainres) ;
}

/*****************************************************************************/
/*                        function ttv_flatttvfig()                          */
/*                                                                           */
/* mise à plat d'une ttvfig                                                  */
/*****************************************************************************/
void ttv_flatttvfig(ttvfig,ttvinslist,type)
ttvfig_list *ttvfig ;
chain_list *ttvinslist ;
long type ;
{
 ttvfig_list *ttvins ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainnext ;
 chain_list *chainsig ;
 char lock = 'N' ;

 if(ttvinslist == NULL)
    return ;

 if((ttvfig->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK)
   {
    ttv_lockttvfig(ttvfig) ;
    lock = 'Y' ;
   }

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
   ttv_parsttvfig(ttvfig,TTV_STS_DTX|TTV_STS_DUAL_D,TTV_FILE_DTX) ;
 
 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
   ttv_parsttvfig(ttvfig,TTV_STS_TTX,TTV_FILE_TTX) ;

 chain = ttv_chainreflist(ttvfig,ttvfig->CONSIG,ttvfig->NBCONSIG) ;
 mbkfree(ttvfig->CONSIG);
 ttvfig->CONSIG = (ttvsig_list **)chain ;

 chain = ttv_chainreflist(ttvfig,ttvfig->NCSIG,ttvfig->NBNCSIG) ;
 mbkfree(ttvfig->NCSIG);
 ttvfig->NCSIG = (ttvsig_list **)chain ;

 chain = ttv_chainreflist(ttvfig,ttvfig->ELCMDSIG,ttvfig->NBELCMDSIG) ;
 mbkfree(ttvfig->ELCMDSIG);
 ttvfig->ELCMDSIG = (ttvsig_list **)chain ;
 chain = ttv_chainreflist(ttvfig,ttvfig->ILCMDSIG,ttvfig->NBILCMDSIG) ;
 ttvfig->ELCMDSIG = (ttvsig_list **)append(chain,(chain_list *)ttvfig->ELCMDSIG) ;
 mbkfree(ttvfig->ILCMDSIG);
 ttvfig->ILCMDSIG = NULL ;

 chain = ttv_chainreflist(ttvfig,ttvfig->ELATCHSIG,ttvfig->NBELATCHSIG) ;
 mbkfree(ttvfig->ELATCHSIG);
 ttvfig->ELATCHSIG = (ttvsig_list **)chain ;
 chain = ttv_chainreflist(ttvfig,ttvfig->ILATCHSIG,ttvfig->NBILATCHSIG) ;
 ttvfig->ELATCHSIG = (ttvsig_list **)append(chain,(chain_list *)ttvfig->ELATCHSIG) ;
 mbkfree(ttvfig->ILATCHSIG);
 ttvfig->ILATCHSIG = NULL ;

 chain = ttv_chainreflist(ttvfig,ttvfig->EBREAKSIG,ttvfig->NBEBREAKSIG) ;
 mbkfree(ttvfig->EBREAKSIG);
 ttvfig->EBREAKSIG = (ttvsig_list **)chain ;
 chain = ttv_chainreflist(ttvfig,ttvfig->IBREAKSIG,ttvfig->NBIBREAKSIG) ;
 ttvfig->EBREAKSIG = (ttvsig_list **)append(chain,(chain_list *)ttvfig->IBREAKSIG) ;
 mbkfree(ttvfig->IBREAKSIG);
 ttvfig->IBREAKSIG = NULL ;

 chain = ttv_chainreflist(ttvfig,ttvfig->EPRESIG,ttvfig->NBEPRESIG) ;
 mbkfree(ttvfig->EPRESIG);
 ttvfig->EPRESIG = (ttvsig_list **)chain ;
 chain = ttv_chainreflist(ttvfig,ttvfig->IPRESIG,ttvfig->NBIPRESIG) ;
 ttvfig->EPRESIG = (ttvsig_list **)append(chain,(chain_list *)ttvfig->IPRESIG) ;
 mbkfree(ttvfig->IPRESIG);
 ttvfig->IPRESIG = NULL ;

 chain = ttv_chainreflist(ttvfig,ttvfig->EXTSIG,ttvfig->NBEXTSIG) ;
 mbkfree(ttvfig->EXTSIG);
 ttvfig->EXTSIG = (ttvsig_list **)chain ;
 chain = ttv_chainreflist(ttvfig,ttvfig->INTSIG,ttvfig->NBINTSIG) ;
 ttvfig->EXTSIG = (ttvsig_list **)append(chain,(chain_list *)ttvfig->INTSIG) ;
 mbkfree(ttvfig->INTSIG);
 ttvfig->INTSIG = NULL ;

 chain = ttv_chainreflist(ttvfig,ttvfig->ESIG,ttvfig->NBESIG) ;
 mbkfree(ttvfig->ESIG);
 ttvfig->ESIG = (ttvsig_list **)chain ;

 stm_getcell(ttvfig->INFO->FIGNAME) ;
 ttv_flatttvfigfromlist(ttvfig,ttvinslist,type) ;

 for(chain = ttvinslist ; chain != NULL ; chain = chainnext)
  {
   chainnext = chain->NEXT ;

   ttvins = (ttvfig_list *)chain->DATA ;

   ttvins->ROOT->INS = delchaindata(ttvins->ROOT->INS,ttvins) ;
   ttvins->ROOT = NULL ;
   ttvins->INSNAME = ttvins->INFO->FIGNAME ;
   ttvins->STATUS |= TTV_STS_HEAD ;
   TTV_HEAD_TTVFIG = addchain(TTV_HEAD_TTVFIG,ttvins) ;

   chainx = ttv_getsigbytype(ttvins,ttvins,TTV_SIG_Q,NULL) ;
   for(chainsig = chainx ; chainsig != NULL ; chainsig = chainsig->NEXT)
    {
     ttv_delcmd(ttvins,(ttvsig_list *)chainsig->DATA) ;
    }
   freechain(chainx) ;

   ttv_freeallttvfig(ttvins) ;
  }

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   chain = ttv_detectloop(ttvfig,TTV_FIND_LINE) ;
   if(chain != NULL)
    {
     for(chainx = chain ; chain != NULL ; chain = chain->NEXT)
       freechain((chain_list *)chainx->DATA) ;
     freechain(chain) ;
    }
  }

 if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   chain = ttv_detectloop(ttvfig,TTV_FIND_PATH) ;
   if(chain != NULL)
    {
     for(chainx = chain ; chain != NULL ; chain = chain->NEXT)
       freechain((chain_list *)chainx->DATA) ;
     freechain(chain) ;
    }
  }

 ttv_cleantagttvfig(TTV_STS_FREE_MASK) ;

 ttv_builtrefsig(ttvfig) ;

 ttv_setttvlevel(ttvfig) ;
 ttv_setsigttvfiglevel(ttvfig) ;

 if(lock == 'Y')
   ttv_unlockttvfig(ttvfig) ;
}

/*****************************************************************************
*                           fonction ttv_getlosigfromevent()                 *
******************************************************************************
* Récupère un losig correspondant à un ttvsig.                               *
*****************************************************************************/
losig_list* ttv_getlosigfromevent( ttvfig, signal, insname, chainfig, lofig )
ttvfig_list     *ttvfig;
ttvsig_list     *signal;
char           **insname;
chain_list     **chainfig;
lofig_list     **lofig;
{
  losig_list    *losig=NULL;
  
  *lofig = rcx_getlofig( signal->ROOT->INFO->FIGNAME, NULL );
  if( !*lofig )
    return NULL;
 
  losig = rcx_gethtrcxsig( NULL, *lofig, signal->NETNAME );
    
  if( !losig ) 
    return NULL ;

  ttvfig = signal->ROOT;
  *insname = ttvfig->INSNAME ;
  *chainfig = addchain( NULL, rcx_getlofig( ttvfig->INFO->FIGNAME, NULL ) );
  ttvfig = ttvfig->ROOT;
  while( ttvfig ) {
    *insname = concatname( ttvfig->INSNAME, *insname );
    *chainfig = addchain( *chainfig, rcx_getlofig( ttvfig->INFO->FIGNAME, NULL ) );
    ttvfig = ttvfig->ROOT;
  }
  *chainfig = reverse( *chainfig );

  return( losig );
}

/*****************************************************************************
*                           fonction ttv_getttvinsbyhiername                 *
******************************************************************************
* Récupère une instance TTV à partir de son nom hiérarchique :               *
* toto.titi.tata.                                                            *
* Les modèles ne sont pas pris en compte.                                    *
* Paramètres : topfig : la ttvfig au toplevel.                               *
*****************************************************************************/
ttvfig_list* ttv_getttvinsbyhiername( ttvfig_list *topfig, char *completename )
{
  char           *insname,
                 *hiername;
  ttvfig_list    *ttvfig,
                 *ttvins;
  chain_list     *scanins;

  ttvfig = topfig;
  leftunconcatname( completename, &insname, &hiername );
  if( ! insname ) {
    insname = hiername;
    hiername = NULL;
  }
  
  if( strcmp( ttvfig->INSNAME, insname ) != 0 ) {
    fflush( stdout );
    fprintf( stderr, "top level and insname mismatch !\n" );
    EXIT(1);
  }

  while( hiername ) {
  
    leftunconcatname( hiername, &insname, &hiername );
    
    if( ! insname ) {
      insname = hiername;
      hiername = NULL;
    }
    
    for( scanins = ttvfig->INS ; scanins ; scanins = scanins->NEXT ) {
      ttvins = (ttvfig_list*)(scanins->DATA);
      if( strcmp( ttvins->INSNAME, insname ) == 0 )
        break;
    }
    if( !scanins ) {
      return NULL ;
    }

    ttvfig = ttvins;
  }

  return( ttvfig );
}

/*****************************************************************************
*                           fonction ttv_isttvsigdriver()                    *
******************************************************************************
* Renvoie 1 si le ttvsig passé en argument correspond au driver d'un réseau  *
* RC.                                                                        *
*                                                                            *
*****************************************************************************/
int ttv_isttvsigdriver( ttvfig_list *toplevel, 
                        long level,
                        long type,
                        ttvsig_list *sig 
                      )
{

  ttvevent_list *node;
  ttvline_list  *line;
  chain_list    *chain;
  ptype_list    *ptype;
  
  if( ( sig->NODE[0].TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN )
    node = &(sig->NODE[0]);
  else {
    if( ( sig->NODE[1].TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
      node = &(sig->NODE[1]);
    else
      return 0;
  }
  
  
  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
    ttv_expfigsig( toplevel, sig, level, toplevel->INFO->LEVEL, 
                   TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX );
    if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
      chain = (chain_list *)ptype->DATA ;
    else
      chain = NULL ;
  }
  else
  {
    ttv_expfigsig( toplevel, sig, level, toplevel->INFO->LEVEL, 
                   TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
    if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
      chain = (chain_list *)ptype->DATA ;
    else
      chain = NULL ;
  }

  for(; chain != NULL ; chain = chain->NEXT)
  {
    line = (ttvline_list *)chain->DATA ;
    if( !ttv_islinelevel( toplevel, line, level ) )
      continue;

    if( ( ( line->TYPE & TTV_LINE_RC ) == TTV_LINE_RC ) )
      break ;
  }

  if( chain ) return 1;
  return 0;

}

/******************************************************************************\
ttv_getttvsigfast()
ttv_addttvsigfast()

Récupère le ttvsig correspondant à un losig en utilisant des tables de hash.

Valeurs pour ttvsig :

EMPTYHT :       le losig n'a jamais été mis dans les tables de hash. Il faut 
                rechercher le ttvsig en passant par les fonctions de ttv.
Autres ( y compris NULL ) : le ttvsig correspondant.
\******************************************************************************/

void ttv_addttvsigfast( ttvfig_list *topfig,
                        char        *insname,
                        losig_list  *losig,
                        ttvsig_list *ttvsig
                      )
{
  ht            *hashsig;
  ht            *hashins;
  ptype_list    *ptl;
  
  // Récupère ou crée la table des instances.
  ptl = getptype( topfig->USER, TTV_FIG_HTAB_SIG );
  if( !ptl ) {
    topfig->USER = addptype( topfig->USER, TTV_FIG_HTAB_SIG, addht( 10 ) );
    ptl = topfig->USER;
  }
  hashins = (ht*)ptl->DATA;

  // Récupère ou crée la tables des losig pour cette instance.
  hashsig = (ht*)gethtitem( hashins, insname );
  if( hashsig == (ht*)EMPTYHT ) {
    hashsig = addht(10);
    addhtitem( hashins, insname, (long)hashsig );
  }

  addhtitem( hashsig, losig, (long)ttvsig );
  
}

ttvsig_list* ttv_getttvsigfast( ttvfig_list *topfig,
                                char        *insname,
                                losig_list  *losig
                              )
{
  ht *hashsig;
  ht *hashins;
  ptype_list *ptl;
  ttvsig_list *ttvsig;
  
  // Récupère la table de hash des noms d'instance hiérarchiques.
  ptl = getptype( topfig->USER, TTV_FIG_HTAB_SIG );
  if( !ptl ) return (ttvsig_list*)EMPTYHT;
  hashins = (ht*)ptl->DATA;

  // Cette table contient une table de hash des signaux.
  hashsig = (ht*)gethtitem( hashins, insname );
  if( !hashsig || hashsig==(ht*)EMPTYHT || hashsig==(ht*)DELETEHT )
    return (ttvsig_list*)EMPTYHT;

  // La valeur NULL est autorisée : aux itérations précédente il n'y avait
  // pas de ttvsig correspondant à ce losig.
  ttvsig = (ttvsig_list*)gethtitem( hashsig, losig );
  if( ttvsig==(ttvsig_list*)EMPTYHT || ttvsig==(ttvsig_list*)DELETEHT )
    return (ttvsig_list*)EMPTYHT;
  return ttvsig;
}

/*****************************************************************************
*                           fonction ttv_getttvsig()                         *
******************************************************************************
* Récupère un signal TTV driver sur un losig. Pour l'instant, cette fonction *
* ne supporter qu'un seul driver par signal, comme cela semble être le cas   *
* dans TTV.                                                                  *
* Si fastmode==1, on utilise des tables de hash pour accélerer les           *
* recherches ultérieures. Ces tables de hash sont assez couteuses en mémoire *
*****************************************************************************/
ttvsig_list* ttv_getttvsig_sub(ttvfig_list   *figdest, losig_list *losig, long level, long type)
{
  ttvsig_list   *driver;
  rcx_list      *rcxsig;
  ttvsig_list   *ttvsig;
  chain_list    *scaninternal;
  locon_list    *internal;
  
  if( !figdest ) 
    return NULL ;
  rcxsig = getrcx( losig );
  if( !rcxsig ) 
    return NULL;

  driver = NULL;
  for( scaninternal = rcxsig->RCXINTERNAL ; 
       scaninternal ; 
       scaninternal = scaninternal->NEXT ) {
       
    internal = (locon_list*)scaninternal->DATA;
    ttvsig = ttv_getsigbyhash( figdest, internal->NAME );
    if( ttvsig && ttv_isttvsigdriver( figdest, level, type, ttvsig ) ) {
      driver = ttvsig;
      break;
    }
  }

  if( !driver && rcxsig->RCXEXTERNAL ) {
    for( scaninternal = rcxsig->RCXEXTERNAL ; 
       scaninternal ; 
       scaninternal = scaninternal->NEXT ) {
      internal = (locon_list*)scaninternal->DATA;
      ttvsig = ttv_getsigbyhash( figdest, internal->NAME );
     //ttvsig = ttv_getsigbyhash( figdest, rcxsig->RCXEXTERNAL->NAME );
      if( ttvsig && ttv_isttvsigdriver( figdest, level, type, ttvsig ) )
      {
        driver = ttvsig;
        break;
      }
    }
  }

  if( !driver ) {
    ttvsig = ttv_getsigbyhash( figdest, getsigname( losig ) );
    if( ttvsig )
      driver = ttvsig;
  }

  return driver;
}

ttvsig_list* ttv_getttvsig( ttvfig_list *topttvfig,
                            long level,
                            long type,
                            char *insname,
                            losig_list *losig,
                            char fastmode
                          )
{
  ttvfig_list   *figdest;
  ttvsig_list   *driver;
  ttvsig_list   *ttvsig;

  if( fastmode ) {
    ttvsig = ttv_getttvsigfast( topttvfig, insname, losig );
    if( ttvsig != (ttvsig_list*)EMPTYHT ) return ttvsig;
  }

  figdest = ttv_getttvinsbyhiername( topttvfig, insname );

  driver=ttv_getttvsig_sub(figdest, losig, level, type);
          
  level=0; // Not used, avoid a warning.
  type=0;

  if( fastmode ) {
    ttv_addttvsigfast( topttvfig, insname, losig, driver );
  }

  return( driver );
}

/******************************************************************************\
Fonctions de haut niveau
\******************************************************************************/
int ttvi_ttvsig2losig( ttvfig_list *ttvfig,
                       lofig_list *lofig,
                       ttvsig_list *ttvsig,
                       losig_list **losig
                     )
{
  char *insname;
  chain_list *chainfig;
  lofig_list *pfig;
  
  *losig = ttv_getlosigfromevent( ttvfig, ttvsig, &insname, &chainfig, &pfig );
  freechain( chainfig );

  lofig = NULL;

  if( *losig )
    return 1;
  return 0;
}

int ttvi_losig2ttvsig( lofig_list *lofig,
                       ttvfig_list *ttvfig,
                       losig_list *losig,
                       ttvsig_list **ttvsig
                     )
{
  *ttvsig = ttv_getttvsig( ttvfig,
                           ttvfig->INFO->LEVEL,
                           TTV_FIND_LINE,
                           ttvfig->INSNAME,
                           losig,
                           1
                         );
  lofig = NULL;
  if( *ttvsig )
    return 1;
  return 0;
}

/******************************************************************************\
FUNC : ttv_addsigcapas
\******************************************************************************/
void ttv_addsigcapas ( ttvsig_list *ttvsig,
                       float cu,
                       float cumin,
                       float cumax,
                       float cd,
                       float cdmin,
                       float cdmax
                     )
{
 ttvsig_capas *ttvsigcapa;

 if ( !getptype (ttvsig->USER, TTV_SIG_CAPAS)) {
   ttvsigcapa = ttv_allocsigcapas ( cu, cumin, cumax, cd, cdmin, cdmax);
   ttvsig->USER = addptype ( ttvsig->USER, TTV_SIG_CAPAS, ttvsigcapa);
 }
}

/******************************************************************************\
FUNC : ttv_getsigcapas
\******************************************************************************/
ttvsig_capas *ttv_getsigcapas ( ttvsig_list *ttvsig,
                                float *cu,
                                float *cumin,
                                float *cumax,
                                float *cd,
                                float *cdmin,
                                float *cdmax)
{
 ttvsig_capas *ttvsigcapa=NULL;
 ptype_list *ptype;

 if ( (ptype = getptype (ttvsig->USER, TTV_SIG_CAPAS)) )  {
   ttvsigcapa = (ttvsig_capas*)ptype->DATA;
   if ( cu )    *cu    = ttvsigcapa->CAPAUP;
   if ( cumin ) *cumin = ttvsigcapa->CAPAUPMIN;
   if ( cumax ) *cumax = ttvsigcapa->CAPAUPMAX;
   if ( cd )    *cd    = ttvsigcapa->CAPADN;
   if ( cdmin ) *cdmin = ttvsigcapa->CAPADNMIN;
   if ( cdmax ) *cdmax = ttvsigcapa->CAPADNMAX;
 }

 return ttvsigcapa;
}

/******************************************************************************\
FUNC : ttv_delsigcapas
\******************************************************************************/
void ttv_delsigcapas ( ttvsig_list *ttvsig)
{
 ttvsig_capas *ttvsigcapa=NULL;

 if ( (ttvsigcapa = ttv_getsigcapas ( ttvsig, NULL, NULL, NULL,
                                              NULL, NULL, NULL )) ) {
   mbkfree ( ttvsigcapa );
   ttvsig->USER = delptype (ttvsig->USER, TTV_SIG_CAPAS);
 }
}

/******************************************************************************\
FUNC : ttv_setallsigcapas
\******************************************************************************/
void ttv_setallsigcapas ( lofig_list *lofig,losig_list *losig, ttvsig_list *ttvsig)
{
 float cu=0.0,cumin=0.0,cumax=0.0;
 float cd=0.0,cdmin=0.0,cdmax=0.0;
 ptype_list *ptype;

 if ( lofig && ttvsig && losig ) {
   if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL1 ) {
     ptype = getptype(losig->USER,ELP_CAPASWITCH);
     if(ptype){
         cu = ((elpcapaswitch_list *)ptype->DATA)->CUP;
         cd = ((elpcapaswitch_list *)ptype->DATA)->CDN;
     }else{
         cu    = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_UP );
         cd    = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_DN );
     }
   }
   else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 ) {
     ptype = getptype(losig->USER,ELP_CAPASWITCH);
     if(ptype){
         cu    = ((elpcapaswitch_list *)ptype->DATA)->CUP;
         cd    = ((elpcapaswitch_list *)ptype->DATA)->CDN;
         cumin = ((elpcapaswitch_list *)ptype->DATA)->CUPMIN;
         cumax = ((elpcapaswitch_list *)ptype->DATA)->CUPMAX;
         cdmin = ((elpcapaswitch_list *)ptype->DATA)->CDNMIN;
         cdmax = ((elpcapaswitch_list *)ptype->DATA)->CDNMAX;
     }else{
         cu    = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_UP );
         cd    = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_DN );
         cumin = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_UP_MIN );
         cumax = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_UP_MAX );
         cdmin = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_DN_MIN );
         cdmax = elpGetTotalCapaSig( lofig, losig, ELP_CAPA_DN_MAX );
     }
   }
   if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL1 ||
        V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 )
     ttv_addsigcapas ( ttvsig,
                       cu, cumin, cumax,
                       cd, cdmin, cdmax
                     );
 }
}

void ttv_getallsigcapas ( ttvsig_list *ttvsig,
                          float *cu,
                          float *cumin,
                          float *cumax,
                          float *cd,
                          float *cdmin,
                          float *cdmax
                        )
{
  lofig_list    *lofig ;
  locon_list    *locon = NULL ;

  if( ttvsig ) {
    
    lofig = ttv_getrcxlofig( ttvsig->ROOT ) ;
    if( lofig ) {
      locon = rcx_gethtrcxcon( NULL, lofig, ttvsig->NAME ) ;
    }

    if( locon ) {
      if( cu )    *cu    = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM, TRC_HALF, 0x0 );
      if( cumin ) *cumin = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_MIN, TRC_HALF, 0x0 );
      if( cumax ) *cumax = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_MAX, TRC_HALF, 0x0 );
      if( cd )    *cd    = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM, TRC_HALF, 0x0 );
      if( cdmin ) *cdmin = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_MIN, TRC_HALF, 0x0 );
      if( cdmax ) *cdmax = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_MAX, TRC_HALF, 0x0 );
    }
    else {
      if( cu )    *cu    = ttvsig->CAPA ;
      if( cumin ) *cumin = ttvsig->CAPA ;
      if( cumax ) *cumax = ttvsig->CAPA ;
      if( cd )    *cd    = ttvsig->CAPA ;
      if( cdmin ) *cdmin = ttvsig->CAPA ;
      if( cdmax ) *cdmax = ttvsig->CAPA ;
    }
  }
  else {
    if( cu )    *cu    = 0.0 ;
    if( cumin ) *cumin = 0.0 ;
    if( cumax ) *cumax = 0.0 ;
    if( cd )    *cd    = 0.0 ;
    if( cdmin ) *cdmin = 0.0 ;
    if( cdmax ) *cdmax = 0.0 ;
  }
}

ttvcriticmc* ttv_getassociatedcriticmc( ttvcritic_list *c )
{
  ptype_list *ptl ;

  ptl = getptype( c->USER, TTV_CRITIC_TABMC );
  if( ptl )
    return (ttvcriticmc*)ptl->DATA ;
  return NULL ;
}

void ttv_setassociatedcriticmc( ttvcritic_list *c, ttvcriticmc *mc )
{
  if( !mc || !c )
    return ;

  c->USER = addptype( c->USER, TTV_CRITIC_TABMC, mc );
}

void ttv_delassociatedcriticmc( ttvcritic_list *c )
{
  ptype_list  *ptl ;
  ttvcriticmc *e ;
 
  if( !c )
    return ;

  ptl = getptype( c->USER, TTV_CRITIC_TABMC );
  if( ptl ) {
    e = (ttvcriticmc*)ptl->DATA ;
    ttv_freecriticmc( e );
    c->USER = delptype( c->USER, TTV_CRITIC_TABMC );
  }
}

static HeapAlloc mcheap ;

ttvcriticmc* ttv_alloccriticmc( int n )
{
  ttvcriticmc *mc ;
  static int f=0 ;

  if( !f ) {
    f = 1 ;
    CreateHeap( sizeof( ttvcriticmc ), 16, &mcheap ) ;
  }
    
  mc = (ttvcriticmc*)AddHeapItem( &mcheap );

  if( n> 0 ) {
    mc->DELAY = mbkalloc( sizeof(long) * n );
    mc->SLOPE = mbkalloc( sizeof(long) * n );
  }
  else {
    mc->DELAY = NULL ;
    mc->SLOPE = NULL ;
  }
  mc->NB = n ;
  return mc ;
}

void ttv_freecriticmc( ttvcriticmc *mc )
{
  if( mc ) {
    mbkfree( mc->DELAY );
    mbkfree( mc->SLOPE );
    DelHeapItem( &mcheap, mc );
  }
}

unsigned int ttv_signtimingfigure( ttvfig_list *tf )
{
  unsigned int s ;
  int i, j ;
  int l ;
  unsigned int c ;

  if (tf->SIGN!=0) return tf->SIGN;
  s=0 ;

  for( i=0 ; i<tf->NBCONSIG ; i++ ) {
    l = strlen( tf->CONSIG[i]->NAME );
    for( j=0 ; j<l ; j++ ) {
      c = (unsigned int)tf->CONSIG[i]->NAME[j] ;
      s = ROT(s,3) ^ c ;
    }
  }
  tf->SIGN=s;
  return s ;
}


float ttv_get_signal_capa(ttvsig_list *tvs)
{
  float val;
  if (ttv_ssta_getsigcapa(tvs, &val))
     return val;
  return tvs->CAPA;
}
