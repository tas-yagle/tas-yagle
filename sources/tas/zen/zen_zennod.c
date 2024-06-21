/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_zenfig.c                                                */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : //Stephane Picault                                        */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"

static  ht    *ZEN_HTNOD = NULL;

/****************************************************************************/
/*{{{                    zen_searchzennod()                                 */ 
/*                                                                          */
/* cherche un noeud dans la liste des noeuds                                */
/****************************************************************************/
zennod_list *zen_searchzennod(ZenFig,name)
zenfig_list *ZenFig;
char        *name;
{
  /*  zennod_list *ScanZenNod;

      for (ScanZenNod=ZenFig->NOD;ScanZenNod;ScanZenNod=ScanZenNod->NEXT)
      if (ScanZenNod->NAME==name) return ScanZenNod;
      return NULL;
   */
  long                res;
  static zenfig_list *zenfig = NULL;

  if (!ZenFig)
  {
    delht(ZEN_HTNOD);
    ZEN_HTNOD = NULL;
    return NULL;
  }
  else if (zenfig)
    ;
//  {
//    if (ZenFig!=zenfig) ZEN_WARNING(0,"attention nouvelle zenfig");
//  }
  else zenfig = ZenFig;

  if (!ZEN_HTNOD) ZEN_HTNOD = addht(ZEN_HTSIZE);

  if ((res=gethtitem(ZEN_HTNOD,name))==EMPTYHT) return NULL;
  else return (zennod_list*)res;
}

/*}}}************************************************************************/
/*{{{                    zen_addzennod()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
zennod_list *zen_addzennod(lastzennod,name,abl)
zennod_list *lastzennod;   /* pointer on the last zennod_list     */
char        *name;         /* node's name                         */
chain_list  *abl;
{
  zennod_list   *NewZenNod;
  pCircuit       cct;
  pNode          bdd;

  cct = (abl)?initializeCct(name,1,1) :NULL;
  bdd = (abl)?zen_ablToBddCct(cct,abl):NULL;
//  if (bdd) addOutputCct(cct,name,bdd);

  if (!ZEN_HTNOD) ZEN_HTNOD = addht(ZEN_HTSIZE);

  NewZenNod = zen_alloczennod();

  
  NewZenNod->NEXT   = lastzennod;
  NewZenNod->NAME   = namealloc(name);
  NewZenNod->INDEX  = 0;
  NewZenNod->FLAGS  = 0;
  NewZenNod->VALUE  = ZEN_VALUE_UNKNOWN;
  NewZenNod->INNOD  = NULL;
  NewZenNod->OUTNOD = NULL;
  NewZenNod->CCT    = cct;
  NewZenNod->BDD    = bdd;
  NewZenNod->NBZER  = -1;
  NewZenNod->NBONE  = -1;

  addhtitem(ZEN_HTNOD,NewZenNod->NAME,(long)NewZenNod);
  return NewZenNod;
}

/*}}}************************************************************************/
/*{{{                    zen_alloczennod()                                  */
/*                                                                          */
/* allocation memoire d'un zennod                                           */
/****************************************************************************/
zennod_list *zen_alloczennod()
{
  return (zennod_list*)mbkalloc(sizeof(zennod_list));
}

/*}}}************************************************************************/
/*{{{                    zen_freezennod()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_freezennod(listzennod)
zennod_list *listzennod;
{
  if (!listzennod) return;

  freechain(listzennod->OUTNOD);
  freechain(listzennod->INNOD);
  zen_freezennod(listzennod->NEXT);
  destroyCct(listzennod->CCT);
  mbkfree(listzennod);
}


/*}}}************************************************************************/
/*{{{                    zen_viewzennod(                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_viewzennod(node)
zennod_list *node;
{
  chain_list    *ScanChain;
  zennod_list   *ScanNod;

  avt_log(LOGZEN,2,"\n\t--> Node");
  avt_log(LOGZEN,2,"\n\t\tNAME\t: %s",node->NAME);
  avt_log(LOGZEN,2,"\n\t\tINDEX\t= %ld",node->INDEX);
  if (node->BDD)
  {
    avt_log(LOGZEN,2,"\n\t\tBDD\t:\n");
    displayExprLog(LOGZEN,2,bddToAblCct(node->CCT,node->BDD));
  }
  else
    avt_log(LOGZEN,2,"\n\t\tBDD\t: (Null)\n");
  avt_log(LOGZEN,2,"\t\tVALUE\t= %c",node->VALUE);
  for (ScanChain=node->INNOD;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanNod = (zennod_list*)ScanChain->DATA;
    avt_log(LOGZEN,2,"\n\t\tINNOD\t: (%ld)\t: %s\t= %c", 
        ScanNod->INDEX,ScanNod->NAME,ScanNod->VALUE);
  }
  for (ScanChain=node->OUTNOD;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanNod = (zennod_list*)ScanChain->DATA;
    avt_log(LOGZEN,2,"\n\t\tOUTNOD\t: (%ld)\t: %s\t= %c", 
        ScanNod->INDEX,ScanNod->NAME,ScanNod->VALUE);
  }  
  avt_log(LOGZEN,2,"\n\t<-- Node");

  if ( ((node->BDD)&&(!node->INNOD)) || ((!node->BDD)&&(node->INNOD)))
    ZEN_WARNING(0,"no bdd on zennode");
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
zennod_list *zen_addOutNode(zenfig,beout,name,abl)
zenfig_list *zenfig;
beout_list  *beout;
char        *name;
chain_list **abl;
{
  zennod_list   *newZenNode;
  
  *abl = beout->ABL;
  newZenNode = zen_addzennod(zenfig->NOD,name,*abl);
  zenfig->NOD_OUT = addchain(zenfig->NOD_OUT,newZenNode);
  zenfig->NOD = newZenNode;
  return newZenNode;

}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
zennod_list *zen_addRegNode(zenfig,bereg,name1,name2,abl)
zenfig_list *zenfig;
bereg_list  *bereg;
char        *name1;
char        *name2;
chain_list **abl;
{
  zennod_list   *newZenNode = NULL;
  chain_list    *chainx;
  biabl_list    *biabl;
  
  for (biabl=bereg->BIABL;biabl;biabl=biabl->NEXT)
  {
    chainx = supportChain_listExpr(biabl->CNDABL);
    for (;chainx;chainx=chainx->NEXT)
      if (chainx->DATA==name2)
      {
        *abl = biabl->CNDABL;
        newZenNode = zen_addzennod(zenfig->NOD,name1,*abl);
        zenfig->NOD = newZenNode;
        break;
      }
    if (newZenNode) break;
    chainx = supportChain_listExpr(biabl->VALABL);
    for (;chainx;chainx=chainx->NEXT)
      if (chainx->DATA==name2)
      {
        *abl = biabl->VALABL;
        newZenNode = zen_addzennod(zenfig->NOD,name1,*abl);
        zenfig->NOD = newZenNode;
        break;
      }
    if (newZenNode) break;
  }
  if (newZenNode) zenfig->NOD_OUT = addchain(zenfig->NOD_OUT,newZenNode);
  return newZenNode;

}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
zennod_list *zen_addAuxNode(zenfig,beaux,name,abl)
zenfig_list *zenfig;
beaux_list  *beaux;
char        *name;
chain_list **abl;
{
  zennod_list   *newZenNode;

  *abl = beaux->ABL;
  newZenNode = zen_addzennod(zenfig->NOD,name,*abl);
  zenfig->NOD = newZenNode;
  return newZenNode;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
zennod_list *zen_addRinNode(zenfig,name)
zenfig_list *zenfig;
char        *name;
{
  zennod_list   *newZenNode;
  
  newZenNode = zen_addzennod(zenfig->NOD,name,createAtom(name));
  zenfig->NOD_IN = addchain(zenfig->NOD_IN,newZenNode);
  zenfig->NOD = newZenNode;
  return newZenNode;
}
