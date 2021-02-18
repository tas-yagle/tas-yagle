/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_fact.c                                                  */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* factorisation de chemin                                                  */
/****************************************************************************/

#include "ttv.h"

chain_list *TTV_ALLOC_FLINE = NULL ;
chain_list *TTV_CMPT_NODE = NULL ;
chain_list *TTV_CMPT_SIGLIST = NULL ;
ttvfline_list *TTV_FREE_FLINE = NULL ;
long TTV_MAX_FCYCLE = (long)10 ;
long TTV_MAX_FLINE = (long)0 ;
long TTV_MAX_CMPT = (long)0 ;

/*****************************************************************************/
/*                        function ttv_addcouple()                           */
/* parametres :                                                              */
/* node1 : noeud d'entree                                                    */
/* node : noeud de recherche                                                 */
/* node2 : noeud de sortie                                                   */
/*                                                                           */
/* rajoute un couple de noeud a un cmpt                                      */
/*****************************************************************************/
void ttv_addcouple(node1,node,node2)
ttvevent_list *node1 ;
ttvevent_list *node ;
ttvevent_list *node2 ;
{
 ttvcmpt_list *cmpt ;
 ht *htab ;

 if((node == NULL) || (node1 == NULL) || (node2 == NULL))
   return ;

 cmpt = ttv_getcmpt(node) ;

 if(((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT) || 
    (cmpt->NBCOUPLE > (cmpt->NBNODEIN + cmpt->NBNODEOUT + (long)1)))
  return ;

 if(cmpt->TABNODE == NULL)
  {
   cmpt->TABNODE = addht(cmpt->NBNODEIN) ;
   htab = addht(cmpt->NBNODEOUT) ;
   cmpt->TABLIST = addchain(cmpt->TABLIST,htab) ;
   addhtitem(cmpt->TABNODE,(void *)node1,(long)htab) ;
   addhtitem(htab,(void *)node2,(long)node2) ;
   cmpt->NBCOUPLE++ ;
   return ;
  }
  
 htab = (ht *)gethtitem(cmpt->TABNODE,(void *)node1) ;

 if((htab == (ht *)EMPTYHT) || (htab == (ht *)DELETEHT))
  {
   htab = addht(cmpt->NBNODEOUT) ;
   cmpt->TABLIST = addchain(cmpt->TABLIST,htab) ;
   addhtitem(cmpt->TABNODE,(void *)node1,(long)htab) ;
   addhtitem(htab,(void *)node2,(long)node2) ;
   cmpt->NBCOUPLE++ ;
   return ;
  }

 if(sethtitem(htab,(void *)node2,(long)node2) == 0)
   cmpt->NBCOUPLE++ ;
}

/*****************************************************************************/
/*                        function ttv_checkcouple()                         */
/* parametres :                                                              */
/* node1 : noeud d'entree                                                    */
/* node : noeud de recherche                                                 */
/* node2 : noeud de sortie                                                   */
/*                                                                           */
/* verifie si un couple existe dans un noeud                                 */
/*****************************************************************************/
int ttv_checkcouple(node1,node,node2)
ttvevent_list *node1 ;
ttvevent_list *node ;
ttvevent_list *node2 ;
{
 ttvcmpt_list *cmpt ;
 ht *htab ;
 long value ;

 if((node == NULL) || (node1 == NULL) || (node2 == NULL))
   return(0) ;

 cmpt = ttv_getcmpt(node) ;

  if(cmpt == NULL)
     return(0) ;

 if(cmpt->TABNODE == NULL)
   return(1) ;

 htab = (ht *)gethtitem(cmpt->TABNODE,(void *)node1) ;

 if((htab == (ht *)EMPTYHT) || (htab == (ht *)DELETEHT))
   return(0) ;
 
 value = gethtitem(htab,(void *)node2) ;

 if((value == EMPTYHT) || (value == DELETEHT))
  return(0) ;
 else
  return(1) ;
}

/*****************************************************************************/
/*                        function ttv_delcouple()                           */
/* parametres :                                                              */
/* node : noeud                                                              */
/*                                                                           */
/* supprime les couples d'un noeud                                           */
/*****************************************************************************/
int ttv_delcouple(node)
ttvevent_list *node ;
{
 ttvcmpt_list *cmpt ;
 chain_list *chain ;

 cmpt = ttv_getcmpt(node) ;

 if(cmpt == NULL)
    return(0) ;

 if(cmpt->TABNODE == NULL)
  {
   cmpt->NBCOUPLE = (long)0 ;
   return(0) ;
  }

 for(chain = cmpt->TABLIST ; chain != NULL ; chain = chain->NEXT)
  {
   delht((ht *)chain->DATA) ;
  }

 if(cmpt->TABLIST != NULL)
  {
   freechain(cmpt->TABLIST) ;
   cmpt->TABLIST = NULL ;
  }

 if(cmpt->TABNODE != NULL)
  {
   delht(cmpt->TABNODE) ;
   cmpt->TABNODE = NULL ;
  }

 cmpt->NBCOUPLE = (long)0 ;

 return(1) ;
}

/*****************************************************************************/
/*                        function ttv_cmptcompar()                          */
/* parametres :                                                              */
/* chain1 : premier cmpt                                                     */
/* chain2 : deuxieme cmpt                                                    */
/*                                                                           */
/* compare deux cmpt en fonction des gains qu'ils engendrent                 */
/*****************************************************************************/
int ttv_cmptcompar(chain1,chain2)
chain_list **chain1 ;
chain_list **chain2 ;
{
 ttvcmpt_list *cmpt1 ;
 ttvcmpt_list *cmpt2 ;

 cmpt1 = ttv_getcmpt((ttvevent_list *)((*chain1)->DATA)) ;
 cmpt2 = ttv_getcmpt((ttvevent_list *)((*chain2)->DATA)) ;

 return((int)((cmpt1->NBPATH - (cmpt1->NBIN+cmpt1->NBOUT)) - 
        (cmpt2->NBPATH - (cmpt2->NBIN+cmpt2->NBOUT)))) ;
}

/*****************************************************************************/
/*                        function ttv_classcmpt()                           */
/* parametres :                                                              */
/* chain : liste des node a classer                                          */
/*                                                                           */
/* classe la liste de node en fonction des gains qu'ils engendrent           */
/*****************************************************************************/
chain_list *ttv_classcmpt(chain)
chain_list *chain ;
{
 chain_list *chainx ;
 chain_list **chaintab ;
 long nbchain = 0 ;
 long nbchainx ;

 if(chain == NULL)
   return(chain) ;

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
   nbchain++ ;

 chaintab = (chain_list**)mbkalloc(nbchain * sizeof(chain_list *)) ;

 chainx = chain ;

 for(nbchainx = (long)0 ; nbchainx < nbchain ; nbchainx++)
  {
   *(chaintab + nbchainx) = chainx ;
   chainx = chainx->NEXT ;
  }

 qsort(chaintab,nbchain,sizeof(chain_list *),(int (*)(const void*,const void*))ttv_cmptcompar)  ;

 chain = *chaintab ;
 chainx = chain ;

 for(nbchainx = (long)1 ; nbchainx < nbchain ; nbchainx++)
  {
   chainx->NEXT = *(chaintab + nbchainx) ;
   chainx = chainx->NEXT ;
  }

 chainx->NEXT = NULL ;

 mbkfree(chaintab) ;

 return(chain) ;
}

/*****************************************************************************/
/*                        function ttv_freeallcmpt()                         */
/* parametres :                                                              */
/*                                                                           */
/* supprime tous les compteurs                                               */
/*****************************************************************************/
void ttv_freeallcmpt()
{
 ptype_list *ptype ;
 chain_list *chain ;

 for(chain = TTV_CMPT_NODE ; chain != NULL ; chain = chain->NEXT)
   {
    if((ptype = getptype(((ttvevent_list *)chain->DATA)->USER,TTV_NODE_CMPT))
        != NULL)
      {
       ttv_freecmpt((ttvevent_list *)chain->DATA) ;
      }
   }

 for(chain = TTV_ALLOC_FLINE ; chain != NULL ; chain = chain->NEXT)
  mbkfree(chain->DATA) ;

 freechain(TTV_ALLOC_FLINE) ;
 freechain(TTV_CMPT_NODE) ;

 TTV_ALLOC_FLINE = NULL ;
 TTV_FREE_FLINE = NULL ;
 TTV_CMPT_NODE = NULL ;
}

/*****************************************************************************/
/*                        function ttv_delallcmpt()                          */
/* parametres :                                                              */
/*                                                                           */
/* supprime tous les compteurs                                               */
/*****************************************************************************/
void ttv_delallcmpt()
{
 chain_list *chain ;
 int i ;

 for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
   {
    for(i = 0 ; i < 2 ; i++)
     {
      ttv_freecmpt((ttvevent_list *)(((ttvsig_list *)chain->DATA)->NODE + i)) ;
     }
   }
}

/*****************************************************************************/
/*                        function ttv_freeflinelist()                       */
/* parametres :                                                              */
/* fline : lien                                                              */
/*                                                                           */
/* supprime une liste de lien                                                */
/*****************************************************************************/
int ttv_freeflinelist(fline)
ttvfline_list *fline ;
{
 if(fline == NULL) return 0 ;

 TTV_FREE_FLINE = (ttvfline_list *)append((chain_list *)fline,
                                          (chain_list *)TTV_FREE_FLINE);
 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_freefline()                           */
/* parametres :                                                              */
/* cmpt : compteur                                                           */
/* type : type de lien                                                       */
/*                                                                           */
/* supprime les liens d'un compteur                                          */
/*****************************************************************************/
int ttv_freefline(cmpt,type)
ttvcmpt_list *cmpt ;
long type ;
{
 if(cmpt == NULL) return 0 ;

 if((type & TTV_FLIN_IN) == TTV_FLIN_IN)
  {
   if(cmpt->PATHIN == NULL)
    return(0) ;
   TTV_FREE_FLINE = (ttvfline_list *)append((chain_list *)cmpt->PATHIN,
                                            (chain_list *)TTV_FREE_FLINE);
   cmpt->PATHIN = NULL ;
   cmpt->NBNODEIN = (long)0 ;
  }
 if((type & TTV_FLIN_OUT) == TTV_FLIN_OUT)
  {
   if(cmpt->PATHOUT == NULL)
    return(0) ;
   TTV_FREE_FLINE = (ttvfline_list *)append((chain_list *)cmpt->PATHOUT,
                                            (chain_list *)TTV_FREE_FLINE);
   cmpt->PATHOUT = NULL ;
   cmpt->NBNODEOUT = (long)0 ;
  }

 return 1 ;
}

/*****************************************************************************/
/*                        function ttv_freecmpt()                            */
/* parametres :                                                              */
/* node : noeud                                                              */
/*                                                                           */
/* supprime le cmpt d'un noeud                                               */
/*****************************************************************************/
int ttv_freecmpt(node)
ttvevent_list *node ;
{
 ttvcmpt_list *cmpt ;
 ptype_list *ptype ;

 if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
   return(0) ;
 else
  cmpt = (ttvcmpt_list *)ptype->DATA ;

 ttv_freefline(cmpt,TTV_FLIN_IN|TTV_FLIN_OUT) ;

 if(cmpt->TABIN != NULL)
   delht(cmpt->TABIN) ;
 
 if(cmpt->TABOUT != NULL)
   delht(cmpt->TABOUT) ;

 ttv_delcouple(node) ;

 node->USER = delptype(node->USER,TTV_NODE_CMPT) ;

 mbkfree(cmpt) ;

 return(1) ;
}

/*****************************************************************************/
/*                        function ttv_allocfline()                          */
/* parametres :                                                              */
/* cmpt : ajoue d'un lien a un compteur                                      */
/* node : noeud a ajouter                                                    */
/* type : type de lien in ou out                                             */
/*                                                                           */
/* fonction d'allocation de lien pour un compteur                            */
/*****************************************************************************/
ttvfline_list *ttv_allocfline(cmpt,node,type)
ttvcmpt_list *cmpt ;
ttvevent_list *node ;
long type ;
{
 ttvfline_list *pt ;
 int i ;

 if(cmpt != NULL)
  {
   if((type & TTV_FLIN_IN) == TTV_FLIN_IN)
    {
     if(cmpt->TABIN != NULL)
      {
       pt = (ttvfline_list *)gethtitem(cmpt->TABIN,node) ;
       if((pt != (ttvfline_list *)EMPTYHT) && (pt != (ttvfline_list *)DELETEHT))
         return(pt) ;
      }
    }
   else
    {
     if(cmpt->TABOUT != NULL)
      {
       pt = (ttvfline_list *)gethtitem(cmpt->TABOUT,node) ;
       if((pt != (ttvfline_list *)EMPTYHT) && (pt != (ttvfline_list *)DELETEHT))
         return(pt) ;
      }
    }
  }

 if (TTV_FREE_FLINE == NULL)
  {
   pt = (ttvfline_list *)mbkalloc(TTV_MAX_BLOC * sizeof(ttvfline_list));
   TTV_MAX_FLINE += TTV_MAX_BLOC ;
   TTV_FREE_FLINE = pt;
   TTV_ALLOC_FLINE = addchain(TTV_ALLOC_FLINE,(void *)pt) ;
   for (i = 1 ; i < TTV_MAX_BLOC ; i++)
   {
    pt->NEXT = pt + 1;
    pt++;
   }
   pt->NEXT = NULL;
  }

 pt = TTV_FREE_FLINE;
 TTV_FREE_FLINE = TTV_FREE_FLINE->NEXT;
 
 pt->NBIN = (long)0 ;
 pt->NBOUT = (long)0 ;
 pt->NBPATH = (long)0 ;
 pt->NODE = node ;
 pt->NEXT = NULL ;

 if(cmpt != NULL)
  {
   if((type & TTV_FLIN_IN) == TTV_FLIN_IN)
    {
     if(cmpt->TABIN == NULL)
       cmpt->TABIN = addht(10) ;
     pt->NEXT = cmpt->PATHIN ;
     cmpt->PATHIN = pt ;
     cmpt->NBNODEIN++ ;
     addhtitem(cmpt->TABIN,(void *)node,(long)pt) ;
    }
   else
    {
     if(cmpt->TABOUT == NULL)
       cmpt->TABOUT = addht(10) ;
     pt->NEXT = cmpt->PATHOUT ;
     cmpt->PATHOUT = pt ;
     cmpt->NBNODEOUT++  ;
     addhtitem(cmpt->TABOUT,(void *)node,(long)pt) ;
    }
  }

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_alloccmpt()                           */
/* parametres :                                                              */
/* node : node ou il faut ajouter les compteur                               */
/*                                                                           */
/* fonction d'allocation de compteur pour un noeud                           */
/*****************************************************************************/
ttvcmpt_list *ttv_alloccmpt(node)
ttvevent_list *node ;
{
 ttvcmpt_list *pt ;
 ptype_list *ptype ;

 if((ptype = getptype(node->USER,TTV_NODE_CMPT)) != NULL)
   return((ttvcmpt_list *)ptype->DATA) ;

 pt = (ttvcmpt_list *)mbkalloc(sizeof(ttvcmpt_list));
 TTV_MAX_CMPT ++ ;
 pt->NBIN = (long)0 ;
 pt->NBOUT = (long)0 ;
 pt->NBPATH = (long)0 ;
 pt->PATHIN = NULL ;
 pt->PATHOUT = NULL ;
 pt->TABNODE = NULL ;
 pt->TABLIST = NULL ;
 pt->NBCOUPLE = (long)0 ;
 pt->NBNODEIN = (long)0 ;
 pt->NBNODEOUT = (long)0 ;
 pt->TABIN = NULL ;
 pt->TABOUT = NULL ;

 node->USER = addptype(node->USER,TTV_NODE_CMPT,(void *)pt) ;

 TTV_CMPT_NODE = addchain(TTV_CMPT_NODE,(void *)node) ;

 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_getcmpt()                             */
/* parametres :                                                              */
/* node : node ou il faut ajouter les compteur                               */
/*                                                                           */
/* fonction d'allocation de compteur pour un noeud                           */
/*****************************************************************************/
ttvcmpt_list *ttv_getcmpt(node)
ttvevent_list *node ;
{
 ptype_list *ptype ;

 if((ptype = getptype(node->USER,TTV_NODE_CMPT)) != NULL)
   return((ttvcmpt_list *)ptype->DATA) ;
 else 
   return(NULL) ;
}

/*****************************************************************************/
/*                        function ttv_clearcmpt()                           */
/* parametres :                                                              */
/* node : node ou il faut netoyer les compteur                               */
/*                                                                           */
/* fonction de nettoyage des compteur d'un noeud                             */
/*****************************************************************************/
int ttv_clearcmpt(cmpt,type)
ttvcmpt_list *cmpt ;
long type ;
{
 ttvevent_list *ptnode ;
 ttvfline_list *fline ;
 ttvfline_list *flinex ;
 ptype_list *ptype ;
 chain_list *chainnode ;
 chain_list *chain ;
 long nb ;
 int flag  = (long)0 ;

 if(cmpt == NULL) return(0) ;

 if((type & TTV_FLIN_IN) == TTV_FLIN_IN)
  {
   flag = (long)1 ;
   chainnode = NULL ;
   delht(cmpt->TABIN) ;
   nb = (long)0 ;
   for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT)
     {
      if((ptype = getptype(fline->NODE->USER,TTV_NODE_FIN)) == NULL)
        {
         fline->NODE->USER = addptype(fline->NODE->USER,TTV_NODE_FIN,
                ttv_allocfline(NULL,fline->NODE,TTV_FLIN_IN)) ;
         ptype = fline->NODE->USER ;
         chainnode = addchain(chainnode,(void *)fline->NODE) ;
        }
       flinex = (ttvfline_list *)ptype->DATA ;
       flinex->NBIN += fline->NBIN ;
       flinex->NBOUT += fline->NBOUT ;
       flinex->NBPATH += fline->NBPATH ;
     }
   ttv_freefline(cmpt,TTV_FLIN_IN) ;
   cmpt->TABIN = addht(10) ;
   for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
     {
      ptnode = (ttvevent_list *)chain->DATA ;
      ptype = getptype(ptnode->USER,TTV_NODE_FIN) ;
      flinex = (ttvfline_list *)ptype->DATA ;
      flinex->NEXT =  cmpt->PATHIN ;
      cmpt->PATHIN = flinex ;
      addhtitem(cmpt->TABIN,(void *)flinex->NODE,(long)flinex) ;
      ptnode->USER = delptype(ptnode->USER,TTV_NODE_FIN) ;
      nb++ ;
     }
   cmpt->NBNODEIN = nb ;
   freechain(chainnode) ;
  }

 if((type & TTV_FLIN_OUT) == TTV_FLIN_OUT)
  {
   flag = (long)1 ;
   chainnode = NULL ;
   delht(cmpt->TABOUT) ;
   nb = (long)0 ;
   for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT)
     {
      if((ptype = getptype(fline->NODE->USER,TTV_NODE_FOUT)) == NULL)
        {
         fline->NODE->USER = addptype(fline->NODE->USER,TTV_NODE_FOUT,
                ttv_allocfline(NULL,fline->NODE,TTV_FLIN_OUT)) ;
         ptype = fline->NODE->USER ;
         chainnode = addchain(chainnode,(void *)fline->NODE) ;
        }
       flinex = (ttvfline_list *)ptype->DATA ;
       flinex->NBIN += fline->NBIN ;
       flinex->NBOUT += fline->NBOUT ;
       flinex->NBPATH += fline->NBPATH ;
     }
   ttv_freefline(cmpt,TTV_FLIN_OUT) ;
   cmpt->TABOUT = addht(10) ;
   for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
     {
      ptnode = (ttvevent_list *)chain->DATA ;
      ptype = getptype(ptnode->USER,TTV_NODE_FOUT) ;
      flinex = (ttvfline_list *)ptype->DATA ;
      flinex->NEXT =  cmpt->PATHOUT ;
      cmpt->PATHOUT = flinex ;
      addhtitem(cmpt->TABOUT,(void *)flinex->NODE,(long)flinex) ;
      ptnode->USER = delptype(ptnode->USER,TTV_NODE_FOUT) ;
      nb++ ;
     }
   cmpt->NBNODEOUT = nb ;
   freechain(chainnode) ;
  }

 return(flag) ;
}

/*****************************************************************************/
/*                        function ttv_verifactsig()                         */
/* parametres :                                                              */
/* ptsig : signal a verifier                                                 */
/* type : type de recherche                                                  */
/*                                                                           */
/* verifie si les noeud d'un signal sont des points de factorisation         */
/*****************************************************************************/
int ttv_verifactsig(ptsig,type)
ttvsig_list *ptsig ;
long type ;
{
 ttvevent_list *node ;
 int i ;
 int flag = 0 ;

 for(i = 0 ; i < 2 ; i++)
  {
   node = ptsig->NODE + i ;
   if((node->TYPE & (TTV_NODE_MIN|TTV_NODE_MOUT)) != 0)
     {
      node->TYPE &= ~(TTV_NODE_MIN|TTV_NODE_MOUT) ;
      if((type & TTV_FIND_FACTGLO) == TTV_FIND_FACTGLO)
        {
         node->TYPE &= ~(TTV_NODE_PFACT) ;
         flag = 1 ;
         if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
          {
           node->TYPE |= TTV_NODE_IMAX ;
           }
         else
          {
           node->TYPE |= TTV_NODE_IMIN ;
          }
        }
     }
   if(((type & TTV_FIND_FACTGLO) != TTV_FIND_FACTGLO) &&
      ((node->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT))
    {
     flag = 1 ;
     node->TYPE &= ~(TTV_NODE_PFACT) ;
     if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
         {
          node->TYPE |= TTV_NODE_IMAX ;
         }
        else
         {
          node->TYPE |= TTV_NODE_IMIN ;
         }
    }
  }
 return(flag) ;
}

/*****************************************************************************/
/*                        function ttv_checkfactsig()                        */
/* parametres :                                                              */
/* ptsig : signal a verifier                                                 */
/*                                                                           */
/* test si un signal est un point de factorisation                           */
/*****************************************************************************/
int ttv_checkfactsig(ptsig)
ttvsig_list *ptsig ;
{
 if(((ptsig->NODE[0].TYPE & (TTV_NODE_IMAX|TTV_NODE_IMIN)) != 0) ||
    ((ptsig->NODE[1].TYPE & (TTV_NODE_IMAX|TTV_NODE_IMIN)) != 0))
   return(1) ;
 else 
   return(0) ;
}

/*****************************************************************************/
/*                        function ttv_classnodetype()                       */
/* parametres :                                                              */
/* chainnode : liste de noeud                                                */
/* type : type de recherche                                                  */
/*                                                                           */
/* classe les noeuds deja points de factorisation en noeud prioritaire       */
/*****************************************************************************/
chain_list *ttv_classnodetype(chainnode)
chain_list *chainnode ;
{
 ttvevent_list *node ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainnodex ;

 chainnode = ttv_classcmpt(chainnode) ;

 chainnodex = NULL ;
 chain = chainnode ;
 
 while(chain != NULL)
  {
   node = (ttvevent_list *)chain->DATA ;
   if(((node->TYPE & (TTV_NODE_IMAX|TTV_NODE_IMIN)) != 0) ||
      ((node->ROOT->TYPE & TTV_SIG_I) == TTV_SIG_I))
    {
     if(chain == chainnode)
      {
       chainnode = chainnode->NEXT ;
       chain->NEXT = chainnodex ;
       chainnodex = chain ;
       chain = chainnode ;
      }
     else
      {
       chainx->NEXT = chain->NEXT ;
       chain->NEXT = chainnodex ;
       chainnodex = chain ;
       chain = chainx->NEXT ;
      }
    }
   else
    {
     chainx = chain ;
     chain = chain->NEXT ;
    }
  }

 chainnodex = reverse(chainnodex) ;
 chainnode = append(chainnode,chainnodex) ;

 return(chainnode) ;
}

/*****************************************************************************/
/*                        function ttv_delcmpt()                             */
/* parametres :                                                              */
/* node : node ou il faut supprimer les compteur                             */
/*                                                                           */
/* fonction de suppression de compteur pour un noeud                         */
/*****************************************************************************/
int ttv_delcmpt(node,gain)
ttvevent_list *node ;
long gain ;
{
 ttvcmpt_list *pt ;
 ttvcmpt_list *cmpt ;
 ttvcmpt_list *cmptx ;
 ttvfline_list *fline ;
 ttvfline_list *flinex ;
 ttvfline_list *flineaux ;
 chain_list *chainin = NULL ;
 chain_list *chainout = NULL ;
 chain_list *chain ;
 ptype_list *ptype ;
 long nbin ;
 long nbout ;
 char flagin = 'N' ;
 char flagout = 'N' ;

 if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
   return(0) ;
 else
  pt = (ttvcmpt_list *)ptype->DATA ;

 if(gain < (long) 2)
  {
   for(fline = pt->PATHIN ; fline != NULL ; fline = fline->NEXT)
     {
      if((fline->NODE->TYPE & TTV_NODE_OLDFACT) != TTV_NODE_OLDFACT)
       {
        if(flagin == 'N')
          flagin = 'Y' ;
        else
         {
          flagin = 'N' ;
          break ;
         }
       }
   }
  
   for(fline = pt->PATHOUT ; fline != NULL ; fline = fline->NEXT)
     {
      if((fline->NODE->TYPE & TTV_NODE_OLDFACT) != TTV_NODE_OLDFACT)
       {
        if(flagout == 'N')
         flagout = 'Y' ;
        else
         {
          flagout = 'N' ;
          break ;
         }
       }
     }
   }
  
 for(fline = pt->PATHIN ; fline != NULL ; fline = fline->NEXT)
   {
    if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
     continue ;
    cmpt = ttv_getcmpt(fline->NODE) ;
    if(((fline->NODE->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT) &&
       (gain < (long)2))
     {
      chainin = addchain(chainin,(void *)fline->NODE) ;
     }
    for(flinex = pt->PATHOUT ; flinex != NULL ; flinex = flinex->NEXT)
     {
      if(((flinex->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT) ||
         (ttv_checkcouple(fline->NODE,node,flinex->NODE) == 0))
       continue ;
      cmptx = ttv_getcmpt(flinex->NODE) ;
      if(((flinex->NODE->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT) &&
         (gain < (long)2))
       {
        chainout = addchain(chainout,(void *)flinex->NODE) ;
       }
      if(cmpt != NULL)
       {
        flineaux = ttv_allocfline(cmpt,flinex->NODE,TTV_FLIN_OUT) ;
        flineaux->NBIN = fline->NBIN ;
        flineaux->NBOUT = flinex->NBOUT ;
        flineaux->NBPATH = fline->NBPATH ;
        fline->NODE->TYPE |= TTV_NODE_NEWFOUT ;
       }
      if(cmptx != NULL)
       {
        flineaux = ttv_allocfline(cmptx,fline->NODE,TTV_FLIN_IN) ;
        flineaux->NBIN = fline->NBIN ;
        flineaux->NBOUT = flinex->NBOUT ;
        flineaux->NBPATH = fline->NBPATH ;
        flinex->NODE->TYPE |= TTV_NODE_NEWFIN ;
        if((flagin == 'N') && 
           ((flinex->NODE->TYPE & TTV_NODE_MAROUT) != TTV_NODE_MAROUT)) 
         {
          flinex->NODE->TYPE &= ~(TTV_NODE_MARFACT|TTV_NODE_MARIN) ;
         }
       }
     }
    if((flagout == 'N') && (cmpt != NULL) && 
       ((fline->NODE->TYPE & TTV_NODE_MARIN) != TTV_NODE_MARIN))
     {
      fline->NODE->TYPE &= ~(TTV_NODE_MARFACT|TTV_NODE_MAROUT) ;
     }
   }

 if(gain < (long) 2)
  {
   for(chain = chainin ; chain != NULL ; chain = chain->NEXT)
     {
      cmpt = ttv_getcmpt((ttvevent_list *)chain->DATA) ;
      /*ttv_clearcmpt(cmpt,TTV_FLIN_OUT) ;*/
      nbin = (long)0 ;
      for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT)
        {
         if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          continue ;
         nbin++ ;
         if(nbin == (long)4)
            break ;
        }
      nbout = (long)0 ;
      for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT)
        {
         if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          continue ;
         nbout++ ;
         if(nbout == (long)4)
            break ;
        }
      if(((nbin*nbout) - (nbin+nbout)) == gain)
       {
        ((ttvevent_list *)chain->DATA)->TYPE |= TTV_NODE_MARFACT ;
       }
     }
   
   for(chain = chainout ; chain != NULL ; chain = chain->NEXT)
     {
      cmpt = ttv_getcmpt((ttvevent_list *)chain->DATA) ;
      /*ttv_clearcmpt(cmpt,TTV_FLIN_IN) ;*/
      nbin = (long)0 ;
      for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT)
        {
         if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          continue ;
         nbin++ ;
         if(nbin == (long)4)
            break ;
        }
      nbout = (long)0 ;
      for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT)
        {
         if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          continue ;
         nbout++ ;
         if(nbout == (long)4)
            break ;
        }
      if(((nbin*nbout) - (nbin+nbout)) == gain)
       {
        ((ttvevent_list *)chain->DATA)->TYPE |= TTV_NODE_MARFACT ;
       }
     }
   freechain(chainin) ;
   freechain(chainout) ;
  }


 ttv_freecmpt(node) ;

 return(1) ;
}

/*****************************************************************************/
/*                        function ttv_deletebadfact()                       */
/* parametres :                                                              */
/* type : type de recherche                                                  */
/*                                                                           */
/* supprmie les noeuds qui ne sont pas des points de factorisations          */
/* en fonction ddu nombre de couple et des entrees sorties                   */
/*****************************************************************************/
int ttv_deletebadfact(type)
long type ;
{
 ttvsig_list *ptsig ;
 ttvevent_list *node ;
 ttvcmpt_list *cmpt ;
 ptype_list *ptype ;
 chain_list *chainnode ;
 chain_list *chain ;
 long i ;
 int flag = 1 ;

 chainnode = NULL ;
 for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   for(i = 0 ; i < 2 ; i++)
    {
     node = (ttvevent_list *)ptsig->NODE + i ;
     if((((node->TYPE & TTV_NODE_IMAX) == TTV_NODE_IMAX) &&
        ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
        (((node->TYPE & TTV_NODE_IMIN) == TTV_NODE_IMIN) &&
        ((type & TTV_FIND_MIN) == TTV_FIND_MIN)) ||
       ((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT))
      continue ;
     if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
      {
       node->TYPE &= ~(TTV_NODE_PFACT) ;
       continue ;
      }
     else
       cmpt = (ttvcmpt_list *)ptype->DATA ;
     /*ttv_clearcmpt(cmpt,TTV_FLIN_IN|TTV_FLIN_OUT) ;*/
     if(cmpt->NBCOUPLE <= (cmpt->NBNODEIN+cmpt->NBNODEOUT+(long)1))
      {
       chainnode = addchain(chainnode,(void *)node) ;
       node->TYPE |= TTV_NODE_MARFACT ;
      }
    }
  }
 chainnode = ttv_classnodetype(chainnode) ;
 for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
  {
   node = (ttvevent_list *)chain->DATA ;

   if((node->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT)
    {
     ttv_delcmpt(node,(long)3) ;
     node->TYPE &= ~(TTV_NODE_MARFACT|TTV_NODE_PFACT) ;
     node->TYPE |= TTV_NODE_OLDFACT ;
    }
   else
    {
     flag = 0 ;
    }
  }

 freechain(chainnode) ;
 ttv_cleanfactline() ;
 ttv_cleansiglist() ;
 return(flag) ;
}

/*****************************************************************************/
/*                        function ttv_deleteglobal()                        */
/* parametres :                                                              */
/* type : type de recherche                                                  */
/*                                                                           */
/* supprmie les noeuds qui ne sont pas des points de factorisations          */
/* en fonction de leur gain global                                           */
/*****************************************************************************/
void ttv_deleteglobal(type)
long type ;
{
 ttvsig_list *ptsig ;
 ttvevent_list *node ;
 ttvcmpt_list *cmpt ;
 ptype_list *ptype ;
 chain_list *chainnode ;
 chain_list *chain ;
 long i ;

 chainnode = NULL ;
 for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   for(i = 0 ; i < 2 ; i++)
    {
     node = (ttvevent_list *)ptsig->NODE + i ;
     if((((node->TYPE & TTV_NODE_IMAX) == TTV_NODE_IMAX) &&
        ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
        (((node->TYPE & TTV_NODE_IMIN) == TTV_NODE_IMIN) &&
        ((type & TTV_FIND_MIN) == TTV_FIND_MIN)) ||
       ((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT))
      continue ;
     if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
      {
       node->TYPE &= ~(TTV_NODE_PFACT) ;
       continue ;
      }
     else
       cmpt = (ttvcmpt_list *)ptype->DATA ;
     if((cmpt->NBPATH - (cmpt->NBIN + cmpt->NBOUT)) < (long)2)
      {
       ttv_delcmpt(node,(long)2) ;
       node->TYPE &= ~(TTV_NODE_PFACT) ;
       node->TYPE |= TTV_NODE_OLDFACT ;
      }
    }
  }

 ttv_cleanfactline() ;
 ttv_cleansiglist() ;
}

/*****************************************************************************/
/*                        function ttv_deletenofact()                        */
/* parametres :                                                              */
/* type : type de recherche                                                  */
/*                                                                           */
/* supprmie les noeuds qui ne sont pas des points de factorisations          */
/* en fonction de leur gain local                                            */
/*****************************************************************************/
void ttv_deletenofact(type)
long type ;
{
 ttvsig_list *ptsig ;
 ttvevent_list *node ;
 ttvcmpt_list *cmpt ;
 ttvfline_list *fline ;
 ptype_list *ptype ;
 chain_list *chainnode ;
 chain_list *chain ;
 chain_list *chainaux[6] ;
 long gain ;
 long i ;
 long nbin ;
 long nbout ;

 for(gain = -1 ; gain < 3 ; gain++ )
  {
   chainnode = NULL ;
   for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
    {
     ptsig = (ttvsig_list *)chain->DATA ;
     for(i = 0 ; i < 2 ; i++)
      {
       node = (ttvevent_list *)ptsig->NODE + i ;
       if((((node->TYPE & TTV_NODE_IMAX) == TTV_NODE_IMAX) &&
          ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
          (((node->TYPE & TTV_NODE_IMIN) == TTV_NODE_IMIN) &&
          ((type & TTV_FIND_MIN) == TTV_FIND_MIN)) ||
         ((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT))
        continue ;
       if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
        {
         node->TYPE &= ~(TTV_NODE_PFACT) ;
         continue ;
        }
       else
         cmpt = (ttvcmpt_list *)ptype->DATA ;
       /*ttv_clearcmpt(cmpt,TTV_FLIN_IN|TTV_FLIN_OUT) ;*/
       nbin = (long)0 ;
       for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT)
         {
          if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          continue ;
          nbin++ ;
          if(nbin == (long)4)
             break ;
         }
       nbout = (long)0 ;
       for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT)
         {
          if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
           continue ;
          nbout++ ;
          if(nbout == (long)4)
             break ;
         }
       if(((nbin*nbout) - (nbin+nbout)) == gain)
        {
         chainnode = addchain(chainnode,(void *)node) ;
         node->TYPE |= TTV_NODE_MARFACT ;
         if(nbin == (long)1)
           node->TYPE |= TTV_NODE_MARIN ;
         if(nbout == (long)1)
           node->TYPE |= TTV_NODE_MAROUT ;
        }
       else if(((nbin*nbout) - (nbin+nbout)) < gain)
        {
         gain = ((nbin*nbout) - (nbin+nbout)) ;
         for(chainaux[0] = chainnode ; chainaux[0] != NULL ; 
             chainaux[0] = chainaux[0]->NEXT)
          {
           ((ttvevent_list *)chainaux[0]->DATA)->TYPE &= ~(TTV_NODE_MARFACT|
                                               TTV_NODE_MARIN|TTV_NODE_MAROUT) ;
          }
         freechain(chainnode) ;
         chainnode = addchain(NULL,(void *)node) ;
         node->TYPE |= TTV_NODE_MARFACT ;
         if(nbin == (long)1)
           node->TYPE |= TTV_NODE_MARIN ;
         if(nbout == (long)1)
           node->TYPE |= TTV_NODE_MAROUT ;
        }
      }
    }
   if(gain == (long)-1)
    {
     chainnode = ttv_classnodetype(chainnode) ;
     for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
      {
       node = (ttvevent_list *)chain->DATA ;

       if((node->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT)
        {
         ttv_delcmpt(node,gain) ;
         node->TYPE &= ~(TTV_NODE_MARFACT|TTV_NODE_PFACT) ;
         node->TYPE |= TTV_NODE_OLDFACT ;
        }
      }
     for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
      {
       node = (ttvevent_list *)chain->DATA ;
       node->TYPE &= ~(TTV_NODE_MARIN|TTV_NODE_MAROUT) ;
      }
     freechain(chainnode) ;
     continue ;
    }
   if(gain == (long)2)
    {
     for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
      {
       node = (ttvevent_list *)chain->DATA ;
       node->TYPE &= ~(TTV_NODE_MARIN|TTV_NODE_MAROUT|TTV_NODE_MARFACT) ;
      }
     freechain(chainnode) ;
     continue ;
    }
   chainaux[0] = NULL ;
   chainaux[1] = NULL ;
   chainaux[2] = NULL ;
   chainaux[3] = NULL ;
   chainaux[4] = NULL ;
   chainaux[5] = NULL ;
   for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
    {
     node = (ttvevent_list *)chain->DATA ;
     if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
         continue ;
       else
         cmpt = (ttvcmpt_list *)ptype->DATA ;
      nbin = (long)0 ;
      for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT)
        {
         if((fline->NODE->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT)
          {
           nbin++ ;
          }
        }
      for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT)
        {
         if((fline->NODE->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT)
          {
           nbin++ ;
          }
        }
      chainaux[nbin] = addchain(chainaux[nbin],node) ;
    }
   freechain(chainnode) ;
   for(i = 0 ; i < 6 ; i++)
    {
     chainaux[i] = ttv_classnodetype(chainaux[i]) ;
     for(chain = chainaux[i] ; chain != NULL ; chain = chain->NEXT)
      {
       node = (ttvevent_list *)chain->DATA ;

       if((node->TYPE & TTV_NODE_MARFACT) == TTV_NODE_MARFACT)
        {
         ttv_delcmpt(node,gain) ;
         node->TYPE &= ~(TTV_NODE_MARFACT|TTV_NODE_PFACT) ;
         node->TYPE |= TTV_NODE_OLDFACT ;
        }
      }
     freechain(chainaux[i]) ;
    }
  }

 ttv_cleanfactline() ;
 ttv_cleansiglist() ;
}

/*****************************************************************************/
/*                        function ttv_cleansiglist()                        */
/* parametres :                                                              */
/*                                                                           */
/* supprmie les signaux qui ne sont plus des points de factorisation         */
/*****************************************************************************/
void ttv_cleansiglist()
{
 ttvsig_list *ptsig ;
 ttvevent_list *node ;
 chain_list *chainsig ;
 chain_list *chain ;
 chain_list *chainx ;
 int i ;
 char flag ;

 chainsig = NULL ;
 chain = TTV_CMPT_SIGLIST ;
 
 while(chain != NULL)
  {
   flag = 'N' ;
   ptsig = (ttvsig_list *)chain->DATA ;
   for(i = 0 ; i < 2 ; i++)
    {
     node = (ttvevent_list *)ptsig->NODE + i ;
     if(((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT) ||
        (ttv_getcmpt(node) == NULL))
      {
       node->TYPE &= ~(TTV_NODE_FACTMASK) ;
       ttv_freecmpt(node) ;
      }
     else
      {
       flag = 'Y' ;
      }
    }
   if(flag == 'N')
    {
     if(chain == TTV_CMPT_SIGLIST)
      {
       TTV_CMPT_SIGLIST = TTV_CMPT_SIGLIST->NEXT ;
       chain->NEXT = chainsig ;
       chainsig = chain ;
       chain = TTV_CMPT_SIGLIST ;
      }
     else
      {
       chainx->NEXT = chain->NEXT ;
       chain->NEXT = chainsig ;
       chainsig = chain ;
       chain = chainx->NEXT ;
      }
    }
   else
    {
     chainx = chain ;
     chain = chain->NEXT ;
    }
  }
 freechain(chainsig) ;
}

/*****************************************************************************/
/*                        function ttv_cleanfactline()                       */
/* parametres :                                                              */
/*                                                                           */
/* supprmie les liens allant au noeud de factorisation non retenus           */
/*****************************************************************************/
void ttv_cleanfactline()
{
 ttvsig_list *ptsig ;
 ttvevent_list *node ;
 ttvcmpt_list *cmpt ;
 ttvfline_list *fline ;
 ttvfline_list *flinex ;
 chain_list *chainnode ;
 chain_list *chain ;
 int i ;

 chainnode = NULL ;
 for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;
   for(i = 0 ; i < 2 ; i++)
    {
     node = (ttvevent_list *)ptsig->NODE + i ;
     if((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT)
      {
       node->TYPE &= ~(TTV_NODE_NEWFIN|TTV_NODE_NEWFOUT) ;
       if((node->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
         chainnode = addchain(chainnode,(void *)node) ;
       continue ;
      }
     if((node->TYPE & TTV_NODE_NEWFIN) == TTV_NODE_NEWFIN)
      {
       node->TYPE &= ~(TTV_NODE_NEWFIN) ;
       cmpt = ttv_getcmpt(node) ;
       fline = cmpt->PATHIN ;
       while(fline != NULL)
        {
         if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          {
           delhtitem(cmpt->TABIN,(void *)fline->NODE) ;
           if(cmpt->PATHIN == fline)
            {
             cmpt->PATHIN = cmpt->PATHIN->NEXT ;
             fline->NEXT = NULL ;
             ttv_freeflinelist(fline) ;
             fline = cmpt->PATHIN ;
            }
           else
            {
             flinex->NEXT = fline->NEXT ;
             fline->NEXT = NULL ;
             ttv_freeflinelist(fline) ;
             fline = flinex->NEXT ;
            }
           cmpt->NBNODEIN-- ;
           continue ;
          }
         flinex = fline ;
         fline = fline->NEXT ;
        }
      }
     if((node->TYPE & TTV_NODE_NEWFOUT) == TTV_NODE_NEWFOUT)
      {
       node->TYPE &= ~(TTV_NODE_NEWFOUT) ;
       cmpt = ttv_getcmpt(node) ;
       fline = cmpt->PATHOUT ; 
       while(fline != NULL)
        {
         if((fline->NODE->TYPE & TTV_NODE_OLDFACT) == TTV_NODE_OLDFACT)
          {
           delhtitem(cmpt->TABOUT,(void *)fline->NODE) ;
           if(cmpt->PATHOUT == fline)
            {
             cmpt->PATHOUT = cmpt->PATHOUT->NEXT ;
             fline->NEXT = NULL ;
             ttv_freeflinelist(fline) ;
             fline = cmpt->PATHOUT ;
            }
           else
            {
             flinex->NEXT = fline->NEXT ;
             fline->NEXT = NULL ;
             ttv_freeflinelist(fline) ;
             fline = flinex->NEXT ;
            }
           cmpt->NBNODEOUT-- ;
           continue ;
          }
         flinex = fline ;
         fline = fline->NEXT ;
        }
      }
    }
  }
 for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
  {
   ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_OLDFACT) ;
  }

 freechain(chainnode) ;
}

/*****************************************************************************/
/*                        function ttv_count()                               */
/* parametres :                                                              */
/* ttvfig : ttvfig que l'on veut factorise                                   */
/* chainin : chaine des entre ou des sortie                                  */
/* type : type de factorisation dual ou pas                                  */
/*                                                                           */
/* compte le nombre de chemin et de sortie ou des entree et le memorise      */
/* dans des ptype                                                            */
/* renvoie la liste des points de factorisations trouve si la parcours est   */
/* dual et le nombre de chemin si le parcours n'est pas dual                 */
/*****************************************************************************/
long ttv_count(ttvfig,root,chainin,type)
ttvfig_list *ttvfig ;
ttvevent_list *root ;
chain_list *chainin ;
long type ;
{
 ttvevent_list *node ;
 ttvcmpt_list *cmpt ;
 ttvcmpt_list *cmptx ;
 ttvfline_list *fline ;
 ttvfline_list *flinex ;
 chain_list *chain ;
 chain_list *chainmarque  = NULL ;
 ttvevent_list *nodes ;
 ttvevent_list *nodee ;
 ttvevent_list *nodem ;
 ttvevent_list *pnode ;
 ttvevent_list *snode ;
 long nbse ;
 long nbss ;
 long nbsm ;
 long nbee ;
 long nbes ;
 long nbem ;
 long nbme ;
 long nbms ;
 long nbmm ;
 long nbe ;
 long nbs ;
 long nbm  = 0 ;
 char flag ;
 char flagext ;
 char flagfig ;

 for(chain = chainin ; chain != NULL ; chain = chain->NEXT)
  {
   if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
     {
      nodes = NULL ;
      nodee = NULL ;
      nodem = NULL ;
      nbse = (long)0 ;
      nbss = (long)0 ;
      nbsm = (long)0 ;
      nbee = (long)0 ;
      nbes = (long)0 ;
      nbem = (long)0 ;
      nbme = (long)0 ;
      nbms = (long)0 ;
      nbmm = (long)0 ;
      flag = 'E' ;
     }
   else nbm ++ ;

   node = (ttvevent_list *)chain->DATA ;
   if(((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0) || 
      ((root->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0)) 
     flagext = 'Y' ;
   else
     flagext = 'N' ;
   node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
   pnode = node ;
   snode = NULL ;
   flagfig = 'N' ;
   while(node->FIND->OUTLINE != NULL)
    {
     if(node->FIND->OUTLINE->FIG == ttvfig)
      {
       flagfig = 'Y' ;
       break ;
      }
     if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      node = node->FIND->OUTLINE->NODE ;
     else
      node = node->FIND->OUTLINE->ROOT ;
     if(node == root)
        break ;
    }
   if(flagfig == 'N')
    continue ;
   node = pnode ;
   while(node->FIND->OUTLINE != NULL)
    {
     if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
      {
       node->FIND->OUTLINE->TYPE |= TTV_LINE_DEPTMAX ;
      }
     else
      {
       node->FIND->OUTLINE->TYPE |= TTV_LINE_DEPTMIN ;
      }
        
     if((node->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
      {
       if(flagext == 'Y') 
         node->ROOT->TYPE |= TTV_SIG_EXT ;
       cmpt = ttv_alloccmpt(node) ;
       if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
         {
          if((pnode->TYPE & TTV_NODE_PUSED) != TTV_NODE_PUSED)
            fline = ttv_allocfline(cmpt,pnode,TTV_FLIN_OUT) ;
          if((pnode->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
           {
            cmptx = ttv_alloccmpt(pnode) ;
            if((pnode->TYPE & TTV_NODE_PUSED) != TTV_NODE_PUSED)
              flinex = ttv_allocfline(cmptx,node,TTV_FLIN_IN) ;
           }
          if((node->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)
            {
             node->TYPE |= TTV_NODE_MARQUE ;
             chainmarque = addchain(chainmarque,(void *)node) ;
             cmpt->NBIN++ ;
            }
          if((pnode->TYPE & TTV_NODE_PUSED) != TTV_NODE_PUSED)
            {
             fline->NBIN++ ;
             if((pnode->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
               flinex->NBIN++ ;
             pnode->TYPE |= TTV_NODE_PUSED ;
            }
          nbe = cmpt->NBIN ;
          nbm = cmpt->NBPATH ;
          nbs = cmpt->NBOUT ;
          if((nbmm - (nbme + nbms)) < (nbm - (nbe + nbs)))
            {
             nbme = nbe ;
             nbms = nbs ;
             nbmm = nbm ;
             nodem = node ;
            }
          if(nbee < nbe)
            {
             if((nbem - (nbee + nbes)) < (nbm - (nbe + nbs)))
               {
                nbee = nbe ;
                nbes = nbs ;
                nbem = nbm ;
                nodee = node ;
                flag = 'E' ;
               }
            }
          if(nbss < nbs)
            {
             if((nbsm - (nbse + nbss)) < (nbm - (nbe + nbs)))
               {
                nbse = nbe ;
                nbss = nbs ;
                nbsm = nbm ;
                nodes = node ;
                flag = 'S' ;
               }
            }
          if((node->TYPE & (TTV_NODE_MIN|TTV_NODE_MOUT)) != 0)
            if((node != nodee) && (node != nodes) && (node != nodem))
             node->TYPE &= ~(TTV_NODE_MIN | TTV_NODE_MOUT) ; 
          pnode = node ;
         }
       else
         {
          if(flagext == 'Y') 
            node->ROOT->TYPE |= TTV_SIG_EXT ;
          fline = ttv_allocfline(cmpt,pnode,TTV_FLIN_IN) ;
          if((pnode->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
           {
            cmptx = ttv_alloccmpt(pnode) ;
            flinex = ttv_allocfline(cmptx,node,TTV_FLIN_OUT) ;
           }
          if((node->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)
            { 
             node->TYPE |= TTV_NODE_MARQUE ;
             chainmarque = addchain(chainmarque,(void *)node) ;
             cmpt->NBOUT++ ;
            }
          if((pnode->TYPE & TTV_NODE_PUSED) != TTV_NODE_PUSED)
            {
             fline->NBOUT++ ;
             if((pnode->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
               flinex->NBOUT++ ;
             pnode->TYPE |= TTV_NODE_PUSED ;
            }
          cmpt->NBPATH++ ;
          fline->NBPATH++ ;
          if((pnode->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
            flinex->NBPATH++ ;
          if((type & TTV_FIND_COUPLE) == TTV_FIND_COUPLE)
            ttv_addcouple(snode,pnode,node) ;
          snode = pnode ;
          pnode = node ;
         }
      }
     if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      node = node->FIND->OUTLINE->NODE ;
     else
      node = node->FIND->OUTLINE->ROOT ;
     if(node == root)
      {
       if(node->FIND->OUTLINE != NULL)
        {
         if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
          {
           node->FIND->OUTLINE->TYPE |= TTV_LINE_DEPTMAX ;
          }
         else
          {
           node->FIND->OUTLINE->TYPE |= TTV_LINE_DEPTMIN ;
          }
        }
        break ;
      }
    }
   if((pnode->TYPE & TTV_NODE_PFACT) == TTV_NODE_PFACT)
    {
     cmptx = ttv_alloccmpt(pnode) ;
     if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      {
       if((pnode->TYPE & TTV_NODE_PUSED) != TTV_NODE_PUSED)
        {
         flinex = ttv_allocfline(cmptx,node,TTV_FLIN_IN) ;
         flinex->NBIN++ ;
         pnode->TYPE |= TTV_NODE_PUSED ;
        }
      }
     else
      {
       flinex = ttv_allocfline(cmptx,node,TTV_FLIN_OUT) ;
       if((pnode->TYPE & TTV_NODE_PUSED) != TTV_NODE_PUSED)
        {
         flinex->NBOUT++ ;
         pnode->TYPE |= TTV_NODE_PUSED ;
        }
       flinex->NBPATH++ ;
      }
     if((type & TTV_FIND_COUPLE) == TTV_FIND_COUPLE)
       ttv_addcouple(snode,pnode,node) ;
    }
   if(((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) && (nodem != NULL))
     {
      if(((nbmm - (nbme + nbms)) >= (((nbem + nbsm) - (nbse * nbes)) 
         - (nbee + nbss + 1))) ||
         (flag == 'E') || ((nodem == nodes) && (nodem == nodee)))
       {
        nodee->TYPE &= ~(TTV_NODE_MIN | TTV_NODE_MOUT) ; 
        nodes->TYPE &= ~(TTV_NODE_MIN | TTV_NODE_MOUT) ; 
        nodem->TYPE |= (TTV_NODE_MIN|TTV_NODE_MOUT) ;
       }
      else if((nbes < nbee) && (nbss > nbse))
       {
        nodee->TYPE |= TTV_NODE_MIN ;
        nodee->TYPE &= ~(TTV_NODE_MOUT) ;
        nodes->TYPE |= TTV_NODE_MOUT ;
        nodes->TYPE &= ~(TTV_NODE_MIN) ;
       }
      else
       {
        nodee->TYPE &= ~(TTV_NODE_MIN | TTV_NODE_MOUT) ; 
        nodes->TYPE &= ~(TTV_NODE_MIN | TTV_NODE_MOUT) ; 
        nodem->TYPE |= (TTV_NODE_MIN|TTV_NODE_MOUT) ;
       }
     }
  }

 for(chain = chainin ; chain != NULL ; chain = chain->NEXT)
  ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE|TTV_NODE_PUSED) ;
 for(chain = chainmarque ; chain != NULL ; chain = chain->NEXT)
  ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE|TTV_NODE_PUSED) ;

 freechain(chainmarque) ;
 return(nbm) ;
}

/*****************************************************************************/
/*                        function ttv_filterrs()                            */
/* parametres :                                                              */
/* chain : list de noeuds                                                    */
/*                                                                           */
/* filtre les destinations rs                                                */
/*****************************************************************************/
chain_list *ttv_filterrs(chain,type)
chain_list *chain ;
char type ;
{
 chain_list *ch ;
 chain_list *chx ;
 chain_list *chrs = NULL ;
 chain_list *chnotrs = NULL ;

 ch = chain ;
 while(ch != NULL)
  {
   chx = ch ;
   ch = ch->NEXT ;
   if(((((ttvevent_list *)chx->DATA)->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
      ((((ttvevent_list *)chx->DATA)->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
    {
     chx->NEXT = chrs ;
     chrs = chx ;
    }
   else
    {
     chx->NEXT = chnotrs ;
     chnotrs = chx ;
    }
  }
 if(type == 'Y')
  {
   for(ch = chnotrs ; ch != NULL ; ch = ch->NEXT)
    ((ttvevent_list *)ch->DATA)->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
   freechain(chnotrs) ;
   return(chrs) ;
  }
 else
  {
   for(ch = chrs ; ch != NULL ; ch = ch->NEXT)
    ((ttvevent_list *)ch->DATA)->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
   freechain(chrs) ;
   return(chnotrs) ;
  }
}

/*****************************************************************************/
/*                        function ttv_countsep()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig a factoriser                                              */
/* type : type de factorisation lien ou chemin                               */
/*                                                                           */
/* compte le nombre de chemin et d'entree de sortie que voie un signal       */
/* renvoie le nombre de chemin total dans le graphe                          */
/*****************************************************************************/
long ttv_countsep(ttvfig,type,marque)
ttvfig_list *ttvfig ;
long type ;
char marque ;
{
 ttvsig_list *sig ;
 ttvevent_list *event ;
 ttvlbloc_list *ptlbloc[3] ;
 ttvline_list *ptline ;
 chain_list *chain ;
 chain_list *chaindualhz = NULL ;
 chain_list *chainduals = NULL ;
 chain_list *chaindualr = NULL ;
 chain_list *ch ;
 long nbpath = 0 ;
 long i ;

 if(ttvfig == NULL)
   return(0) ;
 
 type &= ~(TTV_FIND_DUAL | TTV_FIND_HZ) ;

 if((((ttvfig->STATUS & TTV_STS_DTX) != TTV_STS_DTX) &&
    ((type & TTV_FIND_LINE) == TTV_FIND_LINE)) ||
    (((ttvfig->STATUS & TTV_STS_TTX) != TTV_STS_TTX) &&
    ((type & TTV_FIND_PATH) == TTV_FIND_PATH)))
   {
    ttv_error(31,ttvfig->INFO->FIGNAME,TTV_WARNING) ;
    return(0) ;
   }

 for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
  {
   sig = *(ttvfig->CONSIG + i) ;
   if(((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
      ((sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
      continue ;
   if((sig->TYPE & TTV_SIG_CO) == TTV_SIG_CO)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type,ttvfig->INFO->LEVEL) ;
     nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type,ttvfig->INFO->LEVEL) ;
     nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

 for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
  {
   sig = *(ttvfig->NCSIG + i) ;
   if(((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
      ((sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
      continue ;
   chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type,ttvfig->INFO->LEVEL) ;
   nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
   ttv_fifoclean() ;
   freechain(chain) ;
   chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type,ttvfig->INFO->LEVEL) ;
   nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
   ttv_fifoclean() ;
   freechain(chain) ;
  }

 for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
  {
   sig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
         : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
     {
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
      ttv_fifoclean() ;
      freechain(chain) ;
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
      ttv_fifoclean() ;
      freechain(chain) ;
     }
  }

 for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
  {
   sig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
         : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
   if(((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
      ((sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
     {
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_R,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
      ttv_fifoclean() ;
      for(ch = chain ; ch != NULL ; ch = ch->NEXT)
        if((((ttvevent_list *)ch->DATA)->TYPE & TTV_NODE_INR) != TTV_NODE_INR)
         {
          chaindualr = addchain(chaindualr,ch->DATA) ;
          ((ttvevent_list *)ch->DATA)->TYPE |= TTV_NODE_INR ;
         }
      freechain(chain) ;
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_R,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
      ttv_fifoclean() ;
      for(ch = chain ; ch != NULL ; ch = ch->NEXT)
        if((((ttvevent_list *)ch->DATA)->TYPE & TTV_NODE_INR) != TTV_NODE_INR)
         {
          chaindualr = addchain(chaindualr,ch->DATA) ;
          ((ttvevent_list *)ch->DATA)->TYPE |= TTV_NODE_INR ;
         }
      freechain(chain) ;
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_S,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
      ttv_fifoclean() ;
      for(ch = chain ; ch != NULL ; ch = ch->NEXT)
        if((((ttvevent_list *)ch->DATA)->TYPE & TTV_NODE_INS) != TTV_NODE_INS)
         {
          chainduals = addchain(chainduals,ch->DATA) ;
          ((ttvevent_list *)ch->DATA)->TYPE |= TTV_NODE_INS ;
         }
      freechain(chain) ;
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_S,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
      ttv_fifoclean() ;
      for(ch = chain ; ch != NULL ; ch = ch->NEXT)
        if((((ttvevent_list *)ch->DATA)->TYPE & TTV_NODE_INS) != TTV_NODE_INS)
         {
          chainduals = addchain(chainduals,ch->DATA) ;
          ((ttvevent_list *)ch->DATA)->TYPE |= TTV_NODE_INS ;
         }
      freechain(chain) ;
     }
   else if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type,ttvfig->INFO->LEVEL) ;
     nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type,ttvfig->INFO->LEVEL) ;
     nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

 for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
  {
   sig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
         : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type,ttvfig->INFO->LEVEL) ;
     nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type,ttvfig->INFO->LEVEL) ;
     nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
   chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_HZ,ttvfig->INFO->LEVEL) ;
   nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_HZ) ;
   ttv_fifoclean() ;
   for(ch = chain ; ch != NULL ; ch = ch->NEXT)
     if((((ttvevent_list *)ch->DATA)->TYPE & TTV_NODE_INHZ) != TTV_NODE_INHZ)
      {
       chaindualhz = addchain(chaindualhz,ch->DATA) ;
       ((ttvevent_list *)ch->DATA)->TYPE |= TTV_NODE_INHZ ;
      }
   freechain(chain) ;
   chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_HZ,ttvfig->INFO->LEVEL) ;
   nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_HZ) ;
   ttv_fifoclean() ;
   for(ch = chain ; ch != NULL ; ch = ch->NEXT)
     if((((ttvevent_list *)ch->DATA)->TYPE & TTV_NODE_INHZ) != TTV_NODE_INHZ)
      {
       chaindualhz = addchain(chaindualhz,ch->DATA) ;
       ((ttvevent_list *)ch->DATA)->TYPE |= TTV_NODE_INHZ ;
      }
   freechain(chain) ;
  }

 for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
  {
   sig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
         : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
     {
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE,chain,type) ;
      ttv_fifoclean() ;
      freechain(chain) ;
      chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type,ttvfig->INFO->LEVEL) ;
      nbpath += (long)ttv_count(ttvfig,sig->NODE+1,chain,type) ;
      ttv_fifoclean() ;
      freechain(chain) ;
     }
  }

 if(((type & TTV_FIND_LINE) == TTV_FIND_LINE) && (marque == 'Y'))
  {
   long j ;

   ptlbloc[0] = ttvfig->DBLOC ;
   ptlbloc[1] = ttvfig->EBLOC ;
   ptlbloc[2] = ttvfig->FBLOC ;
   for(i = 0 ; i < 2 ; i++)
    for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
      {
       for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
        {
         ptline = ptlbloc[i]->LINE + j ;
         if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
           continue ;
         if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
          {
           if((ptline->TYPE & TTV_LINE_DEPTMAX) == TTV_LINE_DEPTMAX)
               ptline->TYPE &= ~(TTV_LINE_DEPTMAX) ;
           else
               ptline->TYPE |= TTV_LINE_DENPTMAX ;
          }
         else
          {
           if((ptline->TYPE & TTV_LINE_DEPTMIN) == TTV_LINE_DEPTMIN)
               ptline->TYPE &= ~(TTV_LINE_DEPTMIN) ;
           else
               ptline->TYPE |= TTV_LINE_DENPTMIN ;
          }
        }
      } 
  }

 for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
  {
   sig = *(ttvfig->CONSIG + i) ;
   if((sig->TYPE & TTV_SIG_CI) == TTV_SIG_CI)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

  for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
  {
   sig = *(ttvfig->NCSIG + i) ;
   chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
   if((chaindualr != NULL) || (chainduals != NULL))
     chain = ttv_filterrs(chain,'N') ;
   ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_DUAL) ;
   ttv_fifoclean() ;
   freechain(chain) ;
   chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
   if((chaindualr != NULL) || (chainduals != NULL))
     chain = ttv_filterrs(chain,'N') ;
   ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_DUAL) ;
   ttv_fifoclean() ;
   freechain(chain) ;
  }

 for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
  {
   sig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
         : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

 for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
  {
   sig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
         : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

 for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
  {
   sig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
         : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

 for(ch = chaindualr ; ch ; ch = ch->NEXT)
  {
   event = (ttvevent_list *)ch->DATA ;
   event->TYPE &= ~(TTV_NODE_INR) ;
   chain = ttv_findpath(ttvfig,ttvfig,event,NULL,type|TTV_FIND_DUAL|TTV_FIND_R,
                        ttvfig->INFO->LEVEL) ;
   chain = ttv_filterrs(chain,'Y') ;
   ttv_count(ttvfig,event,chain,type|TTV_FIND_DUAL|TTV_FIND_R) ;
   ttv_fifoclean() ;
   freechain(chain) ;
  }

 freechain(chaindualr) ;

 for(ch = chainduals ; ch ; ch = ch->NEXT)
  {
   event = (ttvevent_list *)ch->DATA ;
   event->TYPE &= ~(TTV_NODE_INS) ;
   chain = ttv_findpath(ttvfig,ttvfig,event,NULL,type|TTV_FIND_DUAL|TTV_FIND_S,
                        ttvfig->INFO->LEVEL) ;
   chain = ttv_filterrs(chain,'Y') ;
   ttv_count(ttvfig,event,chain,type|TTV_FIND_DUAL|TTV_FIND_S) ;
   ttv_fifoclean() ;
   freechain(chain) ;
  }

 freechain(chainduals) ;

 for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
  {
   sig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
         : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
   if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
    {
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
     chain = ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_DUAL,ttvfig->INFO->LEVEL) ;
     if((chaindualr != NULL) || (chainduals != NULL))
       chain = ttv_filterrs(chain,'N') ;
     ttv_count(ttvfig,sig->NODE+1,chain,type|TTV_FIND_DUAL) ;
     ttv_fifoclean() ;
     freechain(chain) ;
    }
  }

 for(ch = chaindualhz ; ch ; ch = ch->NEXT)
  {
   event = (ttvevent_list *)ch->DATA ;
   event->TYPE &= ~(TTV_NODE_INHZ) ;
   chain = ttv_findpath(ttvfig,ttvfig,event,NULL,type|TTV_FIND_DUAL|TTV_FIND_HZ,
                        ttvfig->INFO->LEVEL) ;
   ttv_count(ttvfig,event,chain,type|TTV_FIND_DUAL|TTV_FIND_HZ) ;
   ttv_fifoclean() ;
   freechain(chain) ;
  }

 freechain(chaindualhz) ;

 ttv_fifodelete() ;

 return(nbpath) ;
}

/*****************************************************************************/
/*                        function ttv_detectinter()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig que l'on veut factorise                                   */
/* type : type de detection sur lien ou sur chemin                           */
/*                                                                           */
/* detecte les points intermediaires pour la factorisation                   */
/*****************************************************************************/
void ttv_detectinter(ttvfig,type)
ttvfig_list *ttvfig;
int type;
{
 /*FILE *file ;*/
 ttvsig_list *ptsig ;
 ttvevent_list *node ;
 ttvcmpt_list *cmpt ;
 ttvsbloc_list *ptsbloc ;
 ttvsbloc_list *ptsblocsav ;
 ttvlbloc_list *ptlbloc[3] ;
 ttvline_list *ptline ;
 chain_list *chaini ;
 chain_list *chainx ;
 chain_list *chain ;
 ptype_list *ptype ;
 long i,j,nbfree,nbend ;
 long nbpath ;
 long nbx ;
 long typenode ;
 long typenodex = (long)0 ;
 int test ;
 char flag ;
 char nbloop ;
 char nbloopx ;
 char marque ;
 
 if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
  {
   nbloop = 0 ;
   typenodex |= TTV_NODE_IMAX ;
  }
 else
  {
   typenodex &= ~(TTV_NODE_IMAX) ;
   nbloop = 1 ;
  }

 if((type & TTV_FIND_MIN) == TTV_FIND_MIN)
  {
   typenodex |= TTV_NODE_IMIN ;
   nbloopx = 2 ;
  }
 else
  {
   typenodex &= ~(TTV_NODE_IMIN) ;
   nbloopx = 1 ;
  }

 if(((type & TTV_FIND_MIN) != TTV_FIND_MIN) &&
    ((type & TTV_FIND_MAX) != TTV_FIND_MAX))
  {
   typenodex |= TTV_NODE_IMAX ;
   typenodex |= TTV_NODE_IMIN ;
   nbloop = 0 ;
   nbloopx = 2 ;
  }

 type &= ~(TTV_FIND_DUAL|TTV_FIND_MAX|TTV_FIND_MIN|TTV_FIND_HZ) ;
 type |= TTV_FIND_HIER ;

 /*file = mbkfopen(ttvfig->INFO->FIGNAME,"cnt",WRITE_TEXT) ;
 fprintf(file,"figure %s\n\n",ttvfig->INFO->FIGNAME) ;*/

 for(nbloop = 0 ; nbloop < nbloopx ; nbloop++ )
  {
   marque = 'Y' ;
   if(nbloop == 0) 
    {
     type |= TTV_FIND_MAX ;
     type &= ~(TTV_FIND_MIN) ;
     /*fprintf(file,"MAX PATH FACTORISATION\n\n") ;*/
     typenode = TTV_NODE_IMAX ;
    }
   else
    {
     /*fprintf(file,"MIN PATH FACTORISATION\n\n") ;*/
     type |= TTV_FIND_MIN ;
     type &= ~(TTV_FIND_MAX) ;
     typenode = TTV_NODE_IMIN ;
    }

   for(i = 0 ; i < ttvfig->NBINTSIG + ttvfig->NBEXTSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
           : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
     if(ptsig->ROOT == ttvfig)
      {
       ptsig->NODE[0].TYPE |= typenodex ;
       ptsig->NODE[1].TYPE |= typenodex ;
      }
     else
      {
       ptsig->NODE[0].TYPE &= ~(typenode) ;
       ptsig->NODE[1].TYPE &= ~(typenode) ;
      }
     ptsig->NODE[0].TYPE &= ~(TTV_NODE_FACTMASK) ;
     ptsig->NODE[1].TYPE &= ~(TTV_NODE_FACTMASK) ;
     ptsig->NODE[0].TYPE |= TTV_NODE_PFACT ;
     ptsig->NODE[1].TYPE |= TTV_NODE_PFACT ;
     TTV_CMPT_SIGLIST = addchain(TTV_CMPT_SIGLIST,(void *)ptsig) ;
    }

   for(i = 0 ; i < ttvfig->NBESIG ; i++)
    {
     ptsig = *(ttvfig->ESIG + i) ;
     if(ptsig->ROOT == ttvfig)
      {
       ptsig->NODE[0].TYPE &= ~(TTV_NODE_FACTMASK) ;
       ptsig->NODE[1].TYPE &= ~(TTV_NODE_FACTMASK) ;
       ptsig->NODE[0].TYPE |= TTV_NODE_PFACT ;
       ptsig->NODE[1].TYPE |= TTV_NODE_PFACT ;
       ptsig->NODE[0].TYPE &= ~(typenode) ;
       ptsig->NODE[1].TYPE &= ~(typenode) ;
       TTV_CMPT_SIGLIST = addchain(TTV_CMPT_SIGLIST,(void *)ptsig) ;
      }
    }

   ptsbloc = ttvfig->ISIG ;
   nbfree = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC)) % TTV_MAX_SBLOC ;
   nbend = ttvfig->NBISIG + nbfree ;

   for(i = nbfree ; i < nbend ; i++)
    {
     j = i % TTV_MAX_SBLOC ;
     if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
     ptsig = ptsbloc->SIG + j ;
     ptsig->NODE[0].TYPE &= ~(TTV_NODE_FACTMASK) ;
     ptsig->NODE[1].TYPE &= ~(TTV_NODE_FACTMASK) ;
     ptsig->NODE[0].TYPE |= TTV_NODE_PFACT ;
     ptsig->NODE[1].TYPE |= TTV_NODE_PFACT ;
     ptsig->NODE[0].TYPE &= ~(typenode) ;
     ptsig->NODE[1].TYPE &= ~(typenode) ;
     TTV_CMPT_SIGLIST = addchain(TTV_CMPT_SIGLIST,(void *)ptsig) ;
    }

   test = 1 ;
   flag = 'Y' ;
   if((type & TTV_FIND_FACTGLO) != TTV_FIND_FACTGLO)
    {
     while((test != 0) && (test <= TTV_MAX_FCYCLE))
      {
       if(flag == 'Y')
        {
         nbpath = ttv_countsep(ttvfig,type,marque) ;
         marque = 'N' ;

         ttv_deleteglobal(type) ;
         ttv_deletenofact(type) ;

         ttv_delallcmpt() ;
        }

       ttv_countsep(ttvfig,type|TTV_FIND_COUPLE,marque) ;

       flag = 'N' ;

       for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
        {
         ptsig = (ttvsig_list *)chain->DATA ;
         for(i = 0 ; i < 2 ; i++)
          {
           node = (ttvevent_list *)ptsig->NODE + i ;
           if((node->TYPE & TTV_NODE_PFACT) != TTV_NODE_PFACT)
            continue ;
           if((cmpt = ttv_getcmpt(node)) == NULL)
             continue ;
           /*ttv_clearcmpt(cmpt,TTV_FLIN_IN|TTV_FLIN_OUT) ;*/
           if(((cmpt->NBNODEIN * cmpt->NBNODEOUT) - 
              (cmpt->NBNODEIN + cmpt->NBNODEOUT)) < (long)2)
            flag = 'Y' ;
          }
         if(flag == 'Y')
          break ;
        }

       if(flag == 'Y')
        {
         if(test < TTV_MAX_FCYCLE)
           ttv_delallcmpt() ;
         else
          {
           /* ne sert a rien */
           /*ttv_delallcmpt() ;
           ttv_countsep(ttvfig,type|TTV_FIND_COUPLE,marque) ;*/
           /* fin sert a rien */
           break ;
          }
        }
       else
        {
         if((ttv_deletebadfact(type) == 0) && (test < TTV_MAX_FCYCLE))
          {
           ttv_delallcmpt() ;
          }
         else
          {
           /* ne sert a rien */
           /*ttv_delallcmpt() ;
           ttv_countsep(ttvfig,type|TTV_FIND_COUPLE,marque) ;*/
           /* fin sert a rien */
           break ;
          }
        }
       test++ ;
      }
    }
   else
    {
     ttv_countsep(ttvfig,type,marque) ;
     marque = 'N' ;
    }

   /*fprintf(file,"nb cycle = %ld nb fline = %ld nb cmpt = %ld\n\n",test,TTV_MAX_FLINE,TTV_MAX_CMPT) ;*/
   nbx = (long)0 ;
   for(chain = TTV_CMPT_SIGLIST ; chain != NULL ; chain = chain->NEXT)
    {
     ptsig = (ttvsig_list *)chain->DATA ;
     if(ttv_verifactsig(ptsig,type) != 0)
      {
       nbx++ ;
       /*fprintf(file,"%s : factorisation point \n\n",ttv_getsigname(ttvfig,buf,ptsig)) ;*/
      }
     /*else
       fprintf(file,"%s : not factorisation point \n\n",ttv_getsigname(ttvfig,buf,ptsig)) ;*/
   
     if((type & TTV_FIND_FACTGLO) == TTV_FIND_FACTGLO)
       continue ;
     for(i = 0 ; i < 2 ; i++)
      {
      /* fprintf(file,"node %ld : ",i) ;
       node = (ttvevent_list *)ptsig->NODE + i ;
       if(((node->TYPE & TTV_NODE_IMAX) == TTV_NODE_IMAX) &&
         ((type & TTV_FIND_MAX) == TTV_FIND_MAX))
         fprintf(file,"FP : MAX ") ;
       if(((node->TYPE & TTV_NODE_IMIN) == TTV_NODE_IMIN) &&
         ((type & TTV_FIND_MIN) == TTV_FIND_MIN))
         fprintf(file,"FP : MIN ") ;
       if(((node->TYPE & TTV_NODE_IMAX) == TTV_NODE_IMAX) &&
         ((type & TTV_FIND_MIN) == TTV_FIND_MIN))
         fprintf(file,"MAX ") ;
       if(((node->TYPE & TTV_NODE_IMIN) == TTV_NODE_IMIN) &&
         ((type & TTV_FIND_MAX) == TTV_FIND_MAX))
         fprintf(file,"MIN ") ;
       if((ptype = getptype(node->USER,TTV_NODE_CMPT)) == NULL)
         cmpt = NULL ;
       else
         cmpt = (ttvcmpt_list *)ptype->DATA ;*/
       /*ttv_clearcmpt(cmpt,TTV_FLIN_IN|TTV_FLIN_OUT) ;*/
       /*if(cmpt != NULL)
        {
         long nbinx ;
         long nboutx ;
         long nbinmax ;
         long nboutmax ;
         long nbpathin ;
         long nbpathout ;
  
         fprintf(file,"nbin = %ld nbout = %ld nbpath = %ld gain = %ld\n",
                       cmpt->NBIN,cmpt->NBOUT,cmpt->NBPATH,
                       cmpt->NBPATH - (cmpt->NBIN + cmpt->NBOUT)) ;
         if(cmpt->NBCOUPLE <= (cmpt->NBNODEIN+cmpt->NBNODEOUT+(long)1))
         fprintf(file,"BF nbnodein = %ld nbnodeout = %ld nbfantotal = %ld nbcouple = %ld\n",cmpt->NBNODEIN,cmpt->NBNODEOUT,cmpt->NBNODEIN+cmpt->NBNODEOUT,cmpt->NBCOUPLE) ;
         else
         fprintf(file,"GF nbnodein = %ld nbnodeout = %ld nbfantotal = %ld nbcouple = %ld\n",cmpt->NBNODEIN,cmpt->NBNODEOUT,cmpt->NBNODEIN+cmpt->NBNODEOUT,cmpt->NBCOUPLE) ;
  
         nbin = 0 ;
         nbinx = 0 ;
         nboutmax = 0 ;
         nbpathin = 0 ;

         for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT) 
           {
            fprintf(file,"IN : %s %c nbin = %ld nbout = %ld nbpath = %ld gain = %ld\n",
                          ttv_getsigname(ttvfig,buf,fline->NODE->ROOT),
                          ((fline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 'U':'D',
                          fline->NBIN,fline->NBOUT,fline->NBPATH,
                          fline->NBPATH - (fline->NBIN + fline->NBOUT)) ;
            nbin += fline->NBIN ;
            nbinx++ ;
            nbpathin += fline->NBPATH ;
            if(fline->NBOUT > nboutmax)
               nboutmax = fline->NBOUT ;
            if(fline->NBOUT > cmpt->NBOUT)
              fprintf(file,"ERROR OUT : nbout = %d nbcmptout = %ld\n",
                      fline->NBOUT,cmpt->NBOUT) ;
            if((fline->NODE->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
              fprintf(file,"ERROR MUL : %s %c\n",ttv_getsigname(ttvfig,buf,fline->NODE->ROOT),((fline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 'U':'D') ;
            fline->NODE->TYPE |= TTV_NODE_MARQUE ;
           }

         for(fline = cmpt->PATHIN ; fline != NULL ; fline = fline->NEXT) 
            fline->NODE->TYPE &= ~(TTV_NODE_MARQUE) ;

         nbout = 0 ;
         nboutx = 0 ;
         nbinmax = 0 ;
         nbpathout = 0 ;

         for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT) 
           {
            fprintf(file,"OUT : %s %c nbin = %ld nbout = %ld nbpath = %ld gain = %ld\n",
                          ttv_getsigname(ttvfig,buf,fline->NODE->ROOT),
                          ((fline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 'U':'D',
                          fline->NBIN,fline->NBOUT,fline->NBPATH,
                          fline->NBPATH - (fline->NBIN + fline->NBOUT)) ;
            nbout += fline->NBOUT ;
            nboutx++ ;
            nbpathout += fline->NBPATH ;
            if(fline->NBIN > nbinmax)
               nbinmax = fline->NBIN ;
            if(fline->NBIN > cmpt->NBIN)
              fprintf(file,"ERROR IN : nbin = %d nbcmptin = %ld\n",
                                   fline->NBIN,cmpt->NBIN) ;
            if((fline->NODE->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
              fprintf(file,"ERROR MUL : %s %c\n",ttv_getsigname(ttvfig,buf,fline->NODE->ROOT),((fline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 'U':'D') ;
            fline->NODE->TYPE |= TTV_NODE_MARQUE ;
           }
         for(fline = cmpt->PATHOUT ; fline != NULL ; fline = fline->NEXT) 
            fline->NODE->TYPE &= ~(TTV_NODE_MARQUE) ;

         if((((nbinx*nboutx) - (nbinx+nboutx)) < (long)2) || 
             (nbinx == (long)0) || (nboutx == (long)0))
           fprintf(file,"ERROR FAN : ") ;
         fprintf(file,"node fan : fanin = %d fanout = %ld\n",
                       nbinx,nboutx) ;
         if((nbpathin !=  cmpt->NBPATH) || (nbpathout != cmpt->NBPATH))
           fprintf(file,"ERROR PATH : nbpathin = %d nbpathout = %ld  nbpath = %ld\n",
                                nbpathin,nbpathout,cmpt->NBPATH) ;
         if((nbin != cmpt->NBIN) || (nboutmax > cmpt->NBOUT))
           fprintf(file,"ERROR IN : nbin = %d nbcmptin = %ld nboutmax = %ld\n" ,
                                nbin,cmpt->NBIN,nbpathout) ;
         if((nbout != cmpt->NBOUT)  || (nbinmax > cmpt->NBIN))
           fprintf(file,"ERROR OUT : nbout = %d nbcmptout = %ld nbinmax = %ld \n",
                                nbout,cmpt->NBOUT,nbinmax) ;
        }
       else fprintf(file,"nothing\n") ;
       fprintf(file,"\n") ;
       fflush(file) ;*/
      }
    }
   ttv_freeallcmpt() ;
   freechain(TTV_CMPT_SIGLIST) ;
  TTV_CMPT_SIGLIST = NULL ;
  }
/* fprintf(file,"\nnb fact = %ld nb path = %ld\n",nbx,nbpath) ;*/

 chainx = NULL ;
 chaini = NULL ;
 flag = 'N' ;
 ptsblocsav = NULL ;

 if((nbx=ttvfig->NBESIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBESIG ; i++)
    {
     ptsig = *(ttvfig->ESIG + i) ;
     ptsig->NODE[0].TYPE &= ~(TTV_NODE_PFACT) ;
     ptsig->NODE[1].TYPE &= ~(TTV_NODE_PFACT) ;
     if(ttv_checkfactsig(ptsig) != 0)
      {
       flag = 'Y' ;
       chaini = addchain(chaini,ptsig) ;
       ptsig->TYPE |= TTV_SIG_I ;
       ptsig->TYPE &= ~(TTV_SIG_S) ;
       nbx-- ;
      }
     else
      {
       ptsig->TYPE &= ~(TTV_SIG_EXT) ;
       chainx = addchain(chainx,(void*)ptsig) ;
      }
    }
   if(flag == 'Y')
    {
     ttv_freereflist(ttvfig,ttvfig->ESIG,(long)0) ;
     ttvfig->ESIG = ttv_allocreflist(chainx,nbx) ;
     ttvfig->NBESIG = nbx ;
    }
   else 
     freechain(chainx) ;
   chainx = NULL ;
   flag = 'N' ;
  }


 if((nbx = ttvfig->NBISIG) != 0)
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
     ptsig->NODE[0].TYPE &= ~(TTV_NODE_PFACT) ;
     ptsig->NODE[1].TYPE &= ~(TTV_NODE_PFACT) ;
     if(ttv_checkfactsig(ptsig) != 0)
      {
       ttvsig_list *ptsigx ;

       flag = 'Y' ;
       ptsig->TYPE |= TTV_SIG_F ;
       chaini = ttv_addrefsig(ttvfig,ptsig->NAME,ptsig->NETNAME,ptsig->CAPA,TTV_SIG_I,chaini) ;
       ptsigx = (ttvsig_list *)chaini->DATA ;
       if((ptsig->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
        {
         ptsigx->TYPE |= TTV_SIG_EXT ;
         ptsig->TYPE &= ~(TTV_SIG_EXT) ;
        }
       ptsigx->NODE[0].TYPE |= (ptsig->NODE[0].TYPE & TTV_NODE_IMAX) ;
       ptsigx->NODE[1].TYPE |= (ptsig->NODE[1].TYPE & TTV_NODE_IMAX) ;
       ptsigx->NODE[0].TYPE |= (ptsig->NODE[0].TYPE & TTV_NODE_IMIN) ;
       ptsigx->NODE[1].TYPE |= (ptsig->NODE[1].TYPE & TTV_NODE_IMIN) ;
       ptsigx->USER = ptsig->USER ;
       ptsigx->NODE[0].USER = ptsig->NODE[0].USER ;
       ptsigx->NODE[1].USER = ptsig->NODE[1].USER ;
       ptsigx->NODE[0].INLINE = ptsig->NODE[0].INLINE ;
       ptsigx->NODE[1].INLINE = ptsig->NODE[1].INLINE ;
       ptsigx->NODE[0].INPATH = ptsig->NODE[0].INPATH ;
       ptsigx->NODE[1].INPATH = ptsig->NODE[1].INPATH ;
       ptsig->USER = NULL ;
       ptsig->NODE[0].USER = NULL ;
       ptsig->NODE[1].USER = NULL ;
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_NEW,(void*)ptsigx) ;
       chainx = addchain(chainx,ptsig) ;
       nbx-- ;
      }
     else
      {
       ptsig->TYPE &= ~(TTV_SIG_EXT) ;
      }
    }

   if(flag == 'Y')
    {
     nbx = ttvfig->NBISIG - nbx ; 
     ptsbloc = ttvfig->ISIG ;
     nbfree = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC)) 
               % TTV_MAX_SBLOC ;
     nbend = ttvfig->NBISIG + nbfree ;

     for(i = nbfree ; i < nbend ; i++)
      {
       j = i % TTV_MAX_SBLOC ;
       if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
       ptsig = ptsbloc->SIG + j ;
       if((ptsig->TYPE & TTV_SIG_F) == TTV_SIG_F)
          ptsig->TYPE |= TTV_SIG_MARQUE ;
       nbx-- ;
       if(nbx == (long)0) break ;
      }

     for(chain = chainx ; chain != NULL ; chain = chain->NEXT)
       {
        ttvsig_list *ptsigx ;

        ttvfig->NBISIG -- ;
        ptsig = (ttvsig_list *)chain->DATA ;
        ptsigx = ttvfig->ISIG->SIG + nbfree ;
        if((ptsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE)
          {
           ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;	
           continue ;
          }
        while((ptsigx->TYPE & TTV_SIG_F) == TTV_SIG_F)
         {
          nbfree++ ;
          if(nbfree == TTV_MAX_SBLOC)
           {
            ptsbloc = ttvfig->ISIG ;
            ttvfig->ISIG = ptsbloc->NEXT ;
            ptsbloc->NEXT = ptsblocsav ;
            ptsblocsav = ptsbloc ;
            nbfree = 0 ;
           }
          ptsigx = ttvfig->ISIG->SIG + nbfree ;
         }
        ptype = ptsig->USER ;
        ptsig->NAME = ptsigx->NAME ;
        ptsig->NETNAME = ptsigx->NETNAME ;
        ptsig->CAPA = ptsigx->CAPA ;
        ptsig->ROOT = ptsigx->ROOT ;
        ptsig->TYPE = ptsigx->TYPE ;
        ptsig->USER = ptsigx->USER ;
        ptsig->NODE[0].ROOT = ptsig ;
        ptsig->NODE[0].TYPE = ptsigx->NODE[0].TYPE ;
        ptsig->NODE[0].INLINE = ptsigx->NODE[0].INLINE ;
        ptsig->NODE[0].INPATH = ptsigx->NODE[0].INPATH ;
        ptsig->NODE[0].FIND = ptsigx->NODE[0].FIND ;
        ptsig->NODE[0].USER = ptsigx->NODE[0].USER ;
        ptsig->NODE[1].ROOT = ptsig ;
        ptsig->NODE[1].TYPE = ptsigx->NODE[1].TYPE ;
        ptsig->NODE[1].INLINE = ptsigx->NODE[1].INLINE ;
        ptsig->NODE[1].INPATH = ptsigx->NODE[1].INPATH ;
        ptsig->NODE[1].FIND = ptsigx->NODE[1].FIND ;
        ptsig->NODE[1].USER = ptsigx->NODE[1].USER ;
        ptype->NEXT = ptsig->USER ;
        ptsig->USER = ptype ;
        ptsigx->USER = NULL ;
        ptsigx->NODE[0].USER = NULL ;
        ptsigx->NODE[1].USER = NULL ;
        ptsigx->USER = addptype(ptsigx->USER,TTV_SIG_NEW,(void*)ptsig) ;
        chainx = addchain(chainx,(void*)ptsigx) ;
        nbfree++ ;
        if(nbfree == TTV_MAX_SBLOC)
         {
          ptsbloc = ttvfig->ISIG ;
          ttvfig->ISIG = ptsbloc->NEXT ;
          ptsbloc->NEXT = ptsblocsav ;
          ptsblocsav = ptsbloc ;
          nbfree = 0 ;
         }
       }
     flag = 'N' ;
    }
   if(ttvfig->ISIG != NULL)
    {
     ptsig = ttvfig->ISIG->SIG + nbfree ;
     while((ptsig->TYPE & TTV_SIG_F) == TTV_SIG_F)
       {
        nbfree++ ;
        if(nbfree == TTV_MAX_SBLOC)
         {
          ptsbloc = ttvfig->ISIG ;
          ttvfig->ISIG = ptsbloc->NEXT ;
          ptsbloc->NEXT = ptsblocsav ;
          ptsblocsav = ptsbloc ;
          nbfree = 0 ;
         }
        ptsig = ttvfig->ISIG->SIG + nbfree ;
       }
    }
  }

 ptlbloc[2] = ttvfig->FBLOC ;
 ptlbloc[1] = ttvfig->EBLOC ;
 ptlbloc[0] = ttvfig->DBLOC ;
 for(i = 0 ; i < 3 ; i++)
 for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
   {
     for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
      {
       ptline = ptlbloc[i]->LINE + j ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
          continue ;
       if((ptype = getptype(ptline->ROOT->ROOT->USER,TTV_SIG_NEW)) != NULL)
        {
         if((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           ptline->ROOT = ((ttvsig_list*)ptype->DATA)->NODE+1 ;
         else
           ptline->ROOT = ((ttvsig_list*)ptype->DATA)->NODE ;
        }
       if((ptype = getptype(ptline->NODE->ROOT->USER,TTV_SIG_NEW)) != NULL)
        {
         if((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           ptline->NODE = ((ttvsig_list*)ptype->DATA)->NODE+1 ;
         else
           ptline->NODE = ((ttvsig_list*)ptype->DATA)->NODE ;
        }
       if((ptline->NODE->ROOT->TYPE & TTV_SIG_I) == TTV_SIG_I)
        {
         ptline->NODE->ROOT->TYPE |= TTV_SIG_MARQUE ;
         if((ptline->TYPE & TTV_LINE_D) == TTV_LINE_D)
             ptline->NODE->ROOT->TYPE |= TTV_SIG_EXT ;
        }
       if((ptline->ROOT->ROOT->TYPE & TTV_SIG_I) == TTV_SIG_I)
        {
         ptline->ROOT->ROOT->TYPE |= TTV_SIG_MARQUE ;
         if((ptline->TYPE & TTV_LINE_D) == TTV_LINE_D)
             ptline->ROOT->ROOT->TYPE |= TTV_SIG_EXT ;
        }
      }
   }

 ptlbloc[2] = ttvfig->PBLOC ;
 ptlbloc[1] = ttvfig->JBLOC ;
 ptlbloc[0] = ttvfig->TBLOC ;
 for(i = 0 ; i < 3 ; i++)
 for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
   {
    for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
      {
       ptline = ptlbloc[i]->LINE + j ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
          continue ;
       if((ptype = getptype(ptline->ROOT->ROOT->USER,TTV_SIG_NEW)) != NULL)
        {
         if((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           ptline->ROOT = ((ttvsig_list*)ptype->DATA)->NODE+1 ;
         else
           ptline->ROOT = ((ttvsig_list*)ptype->DATA)->NODE ;
        }
       if((ptype = getptype(ptline->NODE->ROOT->USER,TTV_SIG_NEW)) != NULL)
        {
         if((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
           ptline->NODE = ((ttvsig_list*)ptype->DATA)->NODE+1 ;
         else
           ptline->NODE = ((ttvsig_list*)ptype->DATA)->NODE ;
        }
      }
   }

 for(chain = chainx ; chain != NULL ; chain = chain->NEXT)
 ((ttvsig_list *)chain->DATA)->USER = 
   delptype(((ttvsig_list *)chain->DATA)->USER,TTV_SIG_NEW) ;

 freechain(chainx) ;
 ttv_freesbloclist(ptsblocsav) ;

 if(chaini != NULL) flag = 'Y' ;
 else flag = 'N' ;
 for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
   {
    ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
          : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
    if((i >= ttvfig->NBEXTSIG) && ((ptsig->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT))
     {
      flag = 'Y' ;
     }
    ptsig->NODE[0].TYPE &= ~(TTV_NODE_PFACT) ;
    ptsig->NODE[1].TYPE &= ~(TTV_NODE_PFACT) ;
    if((ttv_checkfactsig(ptsig) != 0) || (ptsig->ROOT == ttvfig) ||
       ((ptsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE))
     {
      ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
      chaini = addchain(chaini,ptsig) ;
     }
    else
     {
      ptsig->TYPE &= ~(TTV_SIG_EXT) ;
      flag = 'Y' ;
     }
   }

 if(flag == 'Y')
  {
   ttv_freereflist(ttvfig,ttvfig->EXTSIG,(long)0) ;
   ttv_freereflist(ttvfig,ttvfig->INTSIG,(long)0) ;
   ttvfig->EXTSIG = NULL ;
   ttvfig->INTSIG = NULL ;
   ttvfig->NBEXTSIG = (long)0 ;
   ttvfig->NBINTSIG = (long)0 ;
   chain = chaini ;
   while(chain != NULL)
     {
      ptsig = (ttvsig_list *)chain->DATA ;
      chaini = chain->NEXT ;
      if((ptsig->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
       {
        ptsig->TYPE &= ~(TTV_SIG_EXT) ;
        chain->NEXT = (chain_list *)ttvfig->EXTSIG ;
        ttvfig->EXTSIG = (ttvsig_list **)chain ;
        ttvfig->NBEXTSIG++ ;
       }
      else
       {
        chain->NEXT = (chain_list *)ttvfig->INTSIG ;
        ttvfig->INTSIG = (ttvsig_list **)chain ;
        ttvfig->NBINTSIG++ ;
       }
      chain = chaini ;
     }
   ttvfig->EXTSIG = ttv_allocreflist((chain_list *)ttvfig->EXTSIG,
                                     ttvfig->NBEXTSIG) ;
   ttvfig->INTSIG = ttv_allocreflist((chain_list *)ttvfig->INTSIG,
                                     ttvfig->NBINTSIG) ;
  }
 else 
   freechain(chaini) ;

 ttv_delsigtab(ttvfig) ;
}
