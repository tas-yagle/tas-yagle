/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : zen                                                         */
/*    Fichier : zen_main.c                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Stephane Picault                                        */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"

int          ZEN_DELAY      = 30;
chain_list  *ZEN_NODTOTREAT = NULL;
chain_list  *ZEN_NODTREATED = NULL;
int          ZEN_TESTMODE   = 0;
int          ZEN_TRACEMODE  = 0;
int          ZEN_FAST       = 0;
int          ZEN_LEVEL      = 1000000;

#define TRUS 0
#define FALS 1
#define OUI  1
#define NON  0
#define DONT 2

/*{{{*/
/*}}}************************************************************************/
/*{{{                    zen_bddIsTerm1()                                   */
/*                                                                          */
/* le bdd est-il teminal1?                                                  */
/****************************************************************************/
int zen_bddIsTerm1(bdd)
pNode        bdd;
{
  return ((bdd->high==BDD_one)&&(bdd->low==BDD_zero));
}

/*}}}************************************************************************/
/*{{{                    zen_bddIsTerm0()                                   */
/*                                                                          */
/* le bdd est-il teminal0?                                                  */
/****************************************************************************/
int zen_bddIsTerm0(bdd)
pNode        bdd;
{
  return ((bdd->high==BDD_zero)&&(bdd->low==BDD_one));
}

/*}}}************************************************************************/
/*{{{                    zen_cleanNBdd()                                    */
/*                                                                          */
/* remet tous les nouveaux bdd a jour                                       */
/****************************************************************************/
void zen_cleanNBdd(zenfig)
zenfig_list *zenfig;
{
  zennod_list   *znodx;
  for (znodx=zenfig->NOD;znodx;znodx=znodx->NEXT)
  {
    znodx->SBDD = znodx->BDD;
    znodx->NBDD = znodx->BDD;
  }
}

/*}}}************************************************************************/
/*{{{                    zen_cleanTreatedNod()                              */
/*                                                                          */
/* nettoye les noeuds traites                                               */
/****************************************************************************/
void zen_cleanTreatedNod()
{
  chain_list    *chainx;

  for (chainx=ZEN_NODTREATED;chainx;chainx=chainx->NEXT)
    ((zennod_list*)chainx->DATA)->VALUE = 'X';
}

/*}}}************************************************************************/
/*{{{                   zen_algo1()                                        */
/*                                                                          */
/*   retourne : 1 si chemin vrai                                            */
/*              0 si chemin faux                                            */
/*              2 s'il ne peut conclure                                     */
/****************************************************************************/
int zen_algo1(ZenFig)
zenfig_list *ZenFig;
{
  int        res;

  zen_cleanNBdd(ZenFig);
  switch (zen_algo1Loc(ZenFig,ZenFig->NOD_PATH))
  {
    case FALS : RET(NON);
    case TRUS : /*if (!zen_resteIn(ZenFig))*/ { RET(OUI); }
    default   : RET(DONT);
  }
fin :
  zen_cleanNBdd(ZenFig);
  zen_cleanTreatedNod();
  return res;
}

/*}}}************************************************************************/
/*{{{                    zen_algo1Loc()                                     */
/*                                                                          */
/*   retour : 0 si bout de chemin ou bon                                    */
/*            2 si on ne peut pas conclure                                  */
/*            1 si faux chemin                                              */
/****************************************************************************/
int zen_algo1Loc(zfig,Path)
zenfig_list *zfig;
chain_list  *Path;
{
  if (!(Path->NEXT)) return TRUS;
  else
    switch (zen_algo1Loc(zfig,Path->NEXT))
    {
      case TRUS : return zen_impderiv(zfig,Path);
      case FALS : return FALS;
      default   : return DONT;
    }
}

/*}}}************************************************************************/
/*{{{                    zen_reduceBdd()                                    */
/*                                                                          */
/* reduit les bdds qui dependent de celui que l'on vient de traiter in path */
/****************************************************************************/
int zen_reduceBdd(zfig,znod)
zenfig_list *zfig;
zennod_list *znod;
{
  zennod_list   *znodx;
  char          *name;
  
  if (zen_bddIsTerm0(znod->NBDD))
  {
    name = searchIndexCct(znod->CCT,znod->NBDD->index);
    znodx = zen_searchzennod(zfig,name);
    if (zen_setNodValue(znodx,'0',ZEN_LEVEL)==FALS)  return FALS;
    else if (zen_simulAOutNod(znod,ZEN_LEVEL)==FALS) return FALS;
    else return TRUS;
  }
  else if (zen_bddIsTerm1(znod->NBDD))
  {
    name = searchIndexCct(znod->CCT,znod->NBDD->index);
    znodx = zen_searchzennod(zfig,name);
    if (zen_setNodValue(znodx,'1',ZEN_LEVEL)==FALS)  return FALS;
    else if (zen_simulAOutNod(znod,ZEN_LEVEL)==FALS) return FALS;
    else return TRUS;
  }
  else return zen_simulAOutNod(znod,ZEN_LEVEL);
}

/*}}}************************************************************************/
/*{{{                    zen_testStuck()                                    */
/*                                                                          */
/* detecte les collages /pas definitif                                      */
/****************************************************************************/
chain_list *zen_testStuck(znod,znodN)
zennod_list *znod;
zennod_list *znodN;
{
  chain_list    *chainx;
  char           buf[64];
  zennod_list   *znodx;
//  pNode          bdds;
  chain_list    *abls;
  
  if ((!znod->INNOD)||(znod->BDD!=znod->SBDD))
    return bddToAblCct(znod->CCT,znod->SBDD);
  else
  {
    abls = bddToAblCct(znod->CCT,znod->SBDD);
    for (chainx=znod->INNOD;chainx;chainx=chainx->NEXT)
    {
      znodx = chainx->DATA;
      if (znodx!=znodN)
      {
        abls = substExpr(abls,znodx->NAME,zen_testStuck(znodx,NULL));
//        bdds = ablToBddCct(znod->CCT,zen_testStuck(znodx,NULL));
        znod->SBDD = ablToBddCct(znod->CCT,abls);
        if (znod->SBDD==BDD_one)
        {
          sprintf(buf,"stuck to one on %s",znod->NAME);
          ZEN_TRACE(0,buf);
          return NULL;
        }
      }
    }
    return abls;
  }
}

/*}}}************************************************************************/
/*{{{                    zen_impderiv()                                     */
/*                                                                          */
/* si il existe 1 seule config pour obtenir le signal, resultat de toto2one */
/* sinon renvoit 0                                                          */
/****************************************************************************/
int zen_impderiv(zfig,path)
zenfig_list *zfig;
chain_list  *path;
{
  zennod_list   *znod = path->DATA;
  zennod_list   *znod2 = path->NEXT->DATA;
  pNode          bdd;
  int            res;

  bdd = zen_deriveBdd(znod,znod2);
  if (bdd==BDD_zero)     { RET(FALS); }
  else if (bdd==BDD_one) { RET(TRUS); }
  else               { RET(zen_reduceBdd(zfig,znod)); }
  // on laisse le tout propre
fin :
  return res;
}

/*}}}************************************************************************/
/*{{{ les gestionnaires pour le temps                                       */
/****************************************************************************/
pid_t       ZEN_CHILD; // pid du fils
pid_t       ZEN_FATHER;// pid du pere

void zen_killChild()
{
  kill(ZEN_CHILD,SIGKILL);
}

void zen_killFather()
{
  kill(ZEN_FATHER,SIGTERM);
}
/****************************************************************************/
/*                       zen_setDelay()                                     */
/*                                                                          */
/* positionne le delay d'attente de l'algo2                                 */
/****************************************************************************/
void zen_setDelay(delay)
int     delay;
{
  ZEN_DELAY = delay;
}

/*}}}************************************************************************/
/*{{{                    zen_algo2()                                        */
/*                                                                          */
/* gere le temps car c'est de l'argent                                      */
/****************************************************************************/
int zen_algo2(ZenFig)
zenfig_list *ZenFig;
{
  int               res;

//  ZEN_FATHER = getpid();
//  switch (ZEN_CHILD=fork())
//  {
//    case -1 : ZEN_ERROR(0,"zen_algo2");
//    case  0 :
    res = zen_algo1(ZenFig);
//              zen_killFather();
              return res;
//    default : mbksetautoackchld(ZEN_CHILD);
//              signal(SIGALRM,zen_killChild);
//              //      signal(SIGCHLD,zen_killFather);
//              alarm(ZEN_DELAY);

//              mbkwaitpid(ZEN_CHILD,1,NULL);
//              pause();
//              return 3;
//  }
}

/*}}}************************************************************************/
/*{{{                    zen_inWithoutAList()                               */
/*                                                                          */
/*                                                                          */
/* renvoie la liste des entrees sans l'entree A                             */
/****************************************************************************/
chain_list *zen_inWithoutAList(zennod,A)
zennod_list *zennod;
zennod_list *A;
{
  chain_list    *chainx;
  chain_list    *inChain = NULL;

  for (chainx=zennod->INNOD;chainx;chainx=chainx->NEXT)
    if (chainx->DATA!=A)
      inChain = addchain(inChain,chainx->DATA);
  return inChain;
}

/*}}}************************************************************************/
/*{{{                    zen_desimplic()                                    */
/*                                                                          */
/* supprime les contraintes sur toute la liste des nouveaux noeuds          */
/****************************************************************************/
chain_list *zen_desimplic(Chain)
chain_list  *Chain;
{
  chain_list    *chainx;

  for (chainx=Chain;chainx;chainx=chainx->NEXT)
    ((zennod_list*)chainx->DATA)->VALUE = 'X';
  return NULL;//zen_freechain(Chain);
}

/*}}}************************************************************************/
/*{{{                    zen_setNodValue()                                  */
/*                                                                          */
/* applique une contrainte a un noeud et tient a jour la liste des noeuds   */
/* modifie                                                                  */
/****************************************************************************/
int zen_setNodValue(znod,Value,level)  // renvoit 0 si OK sinon 1
zennod_list *znod;
char         Value;
int          level;
{
//  chain_list    *ScanChain;
  int            res;

  if      (znod->VALUE==Value)         return TRUS;
  else if (znod->BDD==ZEN_EXPLODEDBDD) return DONT;
  else if (znod->VALUE=='X')
  {
    znod->VALUE = Value;
    if      (Value=='0') znod->NBDD = BDD_zero;
    else if (Value=='1') znod->NBDD = BDD_one;
//    switch (zen_simulNod(znod))
//    {
//      case DONT : return DONT;
//      case FALS : return FALS;
//      default   :
//        if ((Value=='1')||(Value=='0'))
//          switch (toto(znod,Value))
//          {
//            case FALS : return FALS;
//            case DONT : return DONT;
//            default   : break;
//          }
        if ((res=zen_simulAOutNod(znod,level))!=TRUS) return res;
        else if (znod->VALUE==Value) return TRUS;
        else return FALS;
//    }
  }
  else return TRUS;
}

/*}}}************************************************************************/
/*{{{                    zen_nbResOne()                                     */
/*                                                                          */
/* renvoit le nombre de chemins pour obtenir le bdd one                     */
/****************************************************************************/
int zen_nbResOne(bdd)
pNode        bdd;
{
  if (bdd==BDD_zero)     return 0;
  else if (bdd==BDD_one) return 1;
  else return (zen_nbResOne(bdd->high)+zen_nbResOne(bdd->low));
}

/*}}}************************************************************************/
/*{{{                    zen_nbResZero()                                    */
/*                                                                          */
/* renvoit le nombre de chemins pour obtenir le bdd zero                    */
/****************************************************************************/
int zen_nbResZero(bdd)
pNode       bdd;
{
  if     (bdd==BDD_zero) return 1;
  else if (bdd==BDD_one) return 0;
  else return (zen_nbResZero(bdd->high)+zen_nbResZero(bdd->low));
}

/*}}}************************************************************************/
/*{{{                    zen_simulNod()                                     */
/*                                                                          */
/* simulation d'un noeud en fonction de toutes ces entrees                  */
/****************************************************************************/
int zen_simulNod(znod,level)
zennod_list *znod;
int          level;
{
  if      (!(znod->NBDD))               return TRUS; // entree primaire
  else if (znod->NBDD==ZEN_EXPLODEDBDD) return DONT; // explosion
  else if (zen_doBddFSetNod(znod))
  {
    if      (znod->NBDD==BDD_zero) return zen_setNodValue(znod,'0',level);
    else if (znod->NBDD==BDD_one)  return zen_setNodValue(znod,'1',level);
    else                       return TRUS;
  }
  else return TRUS;
}

/*}}}************************************************************************/
/*{{{                    zen_simulAOutNod()                                 */
/*                                                                          */
/* simule tous les noeuds attaques                                          */
/****************************************************************************/
int zen_simulAOutNod(znod,level)
zennod_list *znod;
int          level;
{
  chain_list    *chainx;
  zennod_list   *outznod;
  pNode          bdd;
  
  for (chainx=znod->OUTNOD;chainx;chainx=chainx->NEXT)
  {
    outznod = chainx->DATA;
    bdd = outznod->NBDD;
    switch (zen_simulOutNod(outznod,znod,level))
    {
      case FALS : return FALS;
      case DONT : return DONT;
      default   : if ((!ZEN_FAST)&&(level>0)&&(bdd!=outznod->NBDD))
                    zen_simulAOutNod(outznod,level-1);
    }
  }
  return TRUS;
}

/*}}}************************************************************************/
/*{{{                    zen_simulOutNod()                                  */
/*                                                                          */
/* simule un noeud attaque en fonction de la nouvelle entree                */
/****************************************************************************/
int zen_simulOutNod(znod,inznod,level)
zennod_list *znod;
zennod_list *inznod;
int          level;
{
  chain_list    *abl;
//  chain_list    *abls;

  switch (inznod->VALUE)
  {
    case '0' : abl = notExpr(createAtom(inznod->NAME))      ; break;
    case '1' : abl = createAtom(inznod->NAME)               ; break;
    case 'X' : abl = bddToAblCct(inznod->CCT,inznod->NBDD)  ; break;
    default  : //return TRUS;
  abl = bddToAblCct(inznod->CCT,inznod->NBDD);
//  abls = bddToAblCct(znod->CCT,znod->BDD);
//  abls = substExpr(abls,inznod->NAME,abl);
//  znod->NBDD = ablToBddCct(znod->CCT,abls);
  znod->NBDD = constraintBdd(znod->NBDD,ablToBddCct(znod->CCT,abl));
  return TRUS;
  }

  znod->NBDD = constraintBdd(znod->NBDD,ablToBddCct(znod->CCT,abl));

  if      (znod->NBDD==BDD_zero) return zen_setNodValue(znod,'0',level);
  else if (znod->NBDD==BDD_one)  return zen_setNodValue(znod,'1',level);
  else                       return TRUS;
}

/*}}}************************************************************************/
/*{{{                    zen_ZNodGotValue()                                  */
/*                                                                          */
/* verifie si un noeud a une valeur positionnee 1 ou 0                      */
/****************************************************************************/
int zen_ZNodGotValue(znod)
zennod_list *znod;
{
  switch (znod->VALUE)
  {
    case '1' : return 1;
    case '0' : return 1;
    default  : return 0;
  }
}
/*}}}************************************************************************/
/*{{{                    zen_searchUnSetNode()                              */
/*                                                                          */
/* cherche la liste des noeuds a traiter                                    */
/****************************************************************************/
chain_list *zen_searchUnSetNode(ChainNod)
zennod_list *ChainNod;
{
  chain_list   *ScanChain;
  chain_list   *NewChain = NULL;

  for (;ChainNod;ChainNod=ChainNod->NEXT)
    if (zen_ZNodGotValue(ChainNod))
      for (ScanChain=ChainNod->INNOD;ScanChain;ScanChain=ScanChain->NEXT)
        if (((zennod_list*)ScanChain->DATA)->VALUE=='X')
        {
          NewChain = addchain(NewChain,ChainNod);
          break;
        }
  return NewChain;
}

/*}}}************************************************************************/
/*{{{                    zen_searchSetNod()                                 */
/*                                                                          */
/* cherche la liste des bdds des noeuds qui ont une valeur positionnee      */
/****************************************************************************/
chain_list *zen_searchSetNod(znod)
zennod_list *znod;
{
  chain_list    *listBdd;
  pNode          nodeA;
  chain_list    *chainx;
  zennod_list   *ScanNod;
  int            bddIndex;
//  chain_list    *abl;

  // on ajoute un pour effectuer un and binaire (au moins deux operandes).
  listBdd = addListBdd(NULL,BDD_one);
  for (chainx=znod->INNOD;chainx;chainx=chainx->NEXT)
    if (zen_ZNodGotValue(ScanNod=chainx->DATA))
    {
      bddIndex = searchInputCct(znod->CCT,ScanNod->NAME);
      nodeA = createNodeTermBdd(bddIndex);
//      abl = createAtom(ScanNod->NAME);
//      nodeA = ablToBddCct(znod->CCT,abl);
      if (ScanNod->VALUE=='0') nodeA = notBdd(nodeA);
      listBdd = addListBdd(listBdd,nodeA);
    }
  if (listBdd->DATA==BDD_one) return NULL;
  else return listBdd;
}

/*}}}************************************************************************/
/*{{{                    zen_doBddFSetNod()                                 */
/*                                                                          */
/* cree le bdd correspondant aux entrees renvoie si il y a qq chose de      */
/* changer                                                                  */
/****************************************************************************/
int zen_doBddFSetNod(znod)
zennod_list *znod;
{
  chain_list    *listBdd;

  if (!znod->INNOD) return 1;
  if (!(listBdd=zen_searchSetNod(znod))) return 0;
  else
  {
    znod->NBDD = constraintBdd(znod->NBDD,applyBdd(AND,listBdd));
    freechain(listBdd);
    return 1;
  }
}

/*}}}************************************************************************/
/*{{{                    zen_deriveBdd()                                    */
/*                                                                          */
/* derivate a BDD by a variable                                             */
/****************************************************************************/
pNode zen_deriveBdd(nod,nextNod)
zennod_list *nod;
zennod_list *nextNod;
{
  pNode      high;
  pNode      low;
  pNode      varbdd;

  varbdd = createNodeTermBdd(searchInputCct(nod->CCT,nextNod->NAME));
  high = constraintBdd(nod->NBDD,varbdd);
  low  = constraintBdd(nod->NBDD,notBdd(varbdd));
  nod->NBDD = applyBinBdd(XOR,high,low);
  return nod->NBDD;
}

/*}}}************************************************************************/
/*{{{                    vv()                                               */
/*                                                                          */
/* affichage d'une chainlist de zennod                                      */
/****************************************************************************/
void vv(chain,string)
chain_list  *chain;
char        *string;
{
  chain_list    *chainx;
  zennod_list   *ZenNod;

  fprintf(stdout,"%s",string);
  for (chainx=chain;chainx;chainx=chainx->NEXT)
  {
    ZenNod = chainx->DATA;
    fprintf(stdout,"\t%s",ZenNod->NAME);
  }
}

/*}}}************************************************************************/
/*{{{                    zen_resteIn()                                      */
/*                                                                          */
/* test si dans la liste des noeuds entrant il y en a qui ont une valeur    */
/* inconnue                                                                 */
/****************************************************************************/
int zen_resteIn(ZenFig)
zenfig_list  *ZenFig;
{
  chain_list   *ScanChain;
  int           cpt;

  for (ScanChain=ZenFig->NOD_IN,cpt=0;ScanChain;ScanChain=ScanChain->NEXT)
    if (((zennod_list*)ScanChain->DATA)->VALUE=='X') cpt++;

  return cpt;
}

/*}}}************************************************************************/
/*{{{                    zen_freechain()                                    */
/*                                                                          */
/* permet de faire un freechain en renvoyant un NULL                        */
/****************************************************************************/
chain_list *zen_freechain(chain)
chain_list  *chain;
{
  if (chain) freechain(chain);
  return NULL;
}
