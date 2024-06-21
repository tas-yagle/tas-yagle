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

/****************************************************************************/
/*{{{                    zen_addzenfig()                                    */
/*                                                                          */
/* ajoute une nouvelle zenfig                                               */
/****************************************************************************/
zenfig_list *zen_addzenfig(lastzenfig,name)
zenfig_list *lastzenfig;   /* pointer on the last zenfig_list  */
char        *name;         /* node's name                      */
{
  zenfig_list   *NewZenFig;

  NewZenFig = zen_alloczenfig();

  NewZenFig->NEXT     = lastzenfig;
  NewZenFig->NAME     = namealloc(name);
  NewZenFig->NOD      = NULL;
  NewZenFig->NOD_IN   = NULL;
  NewZenFig->NOD_OUT  = NULL;
  NewZenFig->NOD_PATH = NULL;

  return NewZenFig;
}

/*}}}************************************************************************/
/*{{{                    zen_alloczenfig()                                  */
/*                                                                          */
/* allocation memoire d'une zenfig                                          */
/****************************************************************************/
zenfig_list *zen_alloczenfig()
{
  return (zenfig_list*)mbkalloc(sizeof(zenfig_list));
}

/*}}}************************************************************************/
/*{{{                    zen_freezenfig()                                   */
/* libere une zenfig                                                        */
/*                                                                          */
/****************************************************************************/
void zen_freezenfig(listzenfig)
zenfig_list *listzenfig;
{
  if (!listzenfig) return;

  zen_freezennod(listzenfig->NOD);
  freechain(listzenfig->NOD_PATH);
  freechain(listzenfig->NOD_IN);
  freechain(listzenfig->NOD_OUT);
  zen_freezenfig(listzenfig->NEXT);
  searchBeBux(NULL,NULL);
  searchBeBus(NULL,NULL);
  searchBeReg(NULL,NULL);
  searchBeOut(NULL,NULL);
  searchBeRin(NULL,NULL);
  searchBeAux(NULL,NULL);
  zen_searchzennod(NULL,NULL);
  mbkfree(listzenfig);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_viewzenfig(figure)
zenfig_list *figure;
{
  zennod_list   *ScanNod;
  chain_list    *ScanChain;

  fprintf(stdout,"\n--> Figure");
  fprintf(stdout,"\n\tNAME\t: %s",figure->NAME);

  fprintf(stdout,"\n\tNOD");
  for (ScanNod=figure->NOD;ScanNod;ScanNod=ScanNod->NEXT)
    zen_viewzennod(ScanNod);
  fprintf(stdout,"\n\tNOD_IN\n\t\t");
  for (ScanChain=figure->NOD_IN;ScanChain;ScanChain=ScanChain->NEXT)
    fprintf(stdout,"%s - ",((zennod_list *)ScanChain->DATA)->NAME);
  fprintf( stdout, "\n\tNOD_OUT\n\t\t");
  for (ScanChain=figure->NOD_OUT;ScanChain;ScanChain=ScanChain->NEXT)
    fprintf(stdout,"%s - ",((zennod_list *)ScanChain->DATA)->NAME);
  fprintf( stdout, "\n\tNOD_PATH\n\t\t");
  for (ScanChain=figure->NOD_PATH;ScanChain;ScanChain=ScanChain->NEXT)
    fprintf(stdout,"%s - ",((zennod_list *)ScanChain->DATA)->NAME);
  fprintf(stdout,"\n<-- Figure");
}

/*}}}************************************************************************/
/*{{{                    zen_addNodeToZenfig()                              */
/*                                                                          */
/* selon le type d'abl on ajoute le noeud a la zenfig                       */
/****************************************************************************/
zennod_list *zen_addNodeToZenfig(zenfig,name1,name2,be,type,abl)
zenfig_list *zenfig;
char        *name1;
char        *name2;
void        *be;
int          type;
chain_list **abl;
{
  switch (type)
  {
    case ZEN_BEOUT : return zen_addOutNode(zenfig,be,name1,abl);
    case ZEN_BEREG : return zen_addRegNode(zenfig,be,name1,name2,abl);
    case ZEN_BEAUX : return zen_addAuxNode(zenfig,be,name1,abl); 
    case ZEN_BERIN : return zen_addRinNode(zenfig,name1);
    default        : return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    zen_newNode()                                      */
/*                                                                          */
/* ajout des noeuds a la zenfig                                             */
/****************************************************************************/
zennod_list *zen_newNode(BeFigure,ZenFig,name1,name2)
befig_list  *BeFigure;
zenfig_list *ZenFig;
char        *name1;
char        *name2;
{
  chain_list    *Abl;
  chain_list    *chainx;
  void          *be;
  zennod_list   *nodx;
  zennod_list   *NewZenNod;

  if (!(NewZenNod=zen_searchzennod(ZenFig,name1)) )
  { // si le node n'existe pas
    if ((be=searchBeOut(BeFigure,name1)))
      NewZenNod = zen_addNodeToZenfig(ZenFig,name1,NULL,be,ZEN_BEOUT,&Abl);
    else if ((be=searchBeReg(BeFigure,name1)))
      NewZenNod = zen_addNodeToZenfig(ZenFig,name1,name2,be,ZEN_BEREG,&Abl);
    else if ((be=searchBeAux(BeFigure,name1)))
      NewZenNod = zen_addNodeToZenfig(ZenFig,name1,NULL,be,ZEN_BEAUX,&Abl);
    if (NewZenNod)
    { // si on l'a trouve
      chainx = reverse(supportChain_listExpr(Abl));
      for (;chainx;chainx=chainx->NEXT)
        if ((nodx=zen_newNode(BeFigure,ZenFig,(chainx->DATA),NULL)))
        {
           NewZenNod->INNOD = addchain(NewZenNod->INNOD,nodx);
           nodx->OUTNOD = addchain(nodx->OUTNOD,NewZenNod);
        }
//      NewZenNod->INDEX = searchInputCct(ZenFig->CIRCUI,name1);
    }
    else if ((be=searchBeRin(BeFigure,name1)))
      NewZenNod = zen_addNodeToZenfig(ZenFig,name1,NULL,NULL,ZEN_BERIN,NULL);
  }
  return NewZenNod;
}


/*}}}************************************************************************/
/*{{{                    zen_createzenfig()                                 */
/*                                                                          */
/*  Cree une zenfig_list a partir d'une befig et d'un chemin precis...      */
/****************************************************************************/
zenfig_list *zen_createzenfig(BeFigure,Path)
befig_list  *BeFigure;
ptype_list  *Path;
{
  zenfig_list   *NewZenFig;
  chain_list    *ScanChain;
  beout_list    *BeOut;
  bereg_list    *BeReg;
  beaux_list    *BeAux;
  zennod_list   *ScanNod;
  char           buf[256];

  if (!BeFigure) ZEN_ERROR(0,"Befigure is null");
  if (!Path)     ZEN_ERROR(0,"Path is null");

  NewZenFig = zen_addzenfig(NULL,BeFigure->NAME);
  NewZenFig->CIRCUI = BeFigure->CIRCUI;

  if ((BeOut=searchBeOut(BeFigure,Path->DATA)))
    zen_newNode(BeFigure,NewZenFig,BeOut->NAME,NULL);
  else if ((BeReg=searchBeReg(BeFigure,Path->DATA)))
  {
    if (!(Path->NEXT)) ZEN_ERROR(0,"Path is null")
    else
      zen_newNode(BeFigure,NewZenFig,BeReg->NAME,(Path->NEXT)->DATA);
  }
  else if ((BeAux=searchBeAux(BeFigure,Path->DATA)))
    zen_newNode(BeFigure,NewZenFig,BeAux->NAME,NULL);
  // on positionne les fronts sur le chemin
  for (;Path;Path=Path->NEXT)
  {
    if ((ScanNod=zen_searchzennod(NewZenFig,Path->DATA)))
    {
      NewZenFig->NOD_PATH = addchain(NewZenFig->NOD_PATH,ScanNod);
      ScanNod->VALUE = (char)Path->TYPE;
    }
    else
    {
      sprintf(buf,"\"%s\" can't be find",(char*)Path->DATA);
      ZEN_ERROR(0,buf);
    }
  }
  // test
  for (ScanChain=NewZenFig->NOD_PATH;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanNod = ScanChain->DATA;
    sprintf(buf,"%-15s -> %c",ScanNod->NAME,ScanNod->VALUE);
//    ZEN_TST(0,buf);
  }

  NewZenFig->NOD_PATH = reverse(NewZenFig->NOD_PATH);
  return NewZenFig;
}
