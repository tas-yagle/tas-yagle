/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_sim.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "cbh_sim.h"

int     CBH_DEBUG = 0;

/****************************************************************************/
/*{{{                    simulation                                         */
/****************************************************************************/
/*{{{                    cbh_printLofigCst()                                */
/*                                                                          */
/* affiche l'etat des contraintes sur une lofig                             */
/****************************************************************************/
void cbh_printLofigCst(lofig,loins,in,out)
lofig_list  *lofig;
loins_list  *loins;
locon_list  *in;
locon_list  *out;
{
  locon_list    *locon;
  char           buf[64];
  char           trans;
  int            cst;

  if (loins)
    sprintf(buf," of %s",loins->INSNAME);
  else
    sprintf(buf,"%c",'\0');
  printf(" state of constraint for %s%s\n",lofig->NAME,buf);

  for (locon = lofig->LOCON;locon;locon=locon->NEXT)
  {
    trans = (locon==in)?'I':((locon==out)?'O':' ');
    cst = (cst=cbh_getLoconCst(locon))?(cst):(cbh_getConstraint(locon->SIG));
    printf("%2s%c %-5s: %c\n","",trans,locon->NAME,cbh_cst(cst));
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_getToBeTreatedList()                           */
/*                                                                          */
/* renvoie la liste des signaux a traiter d'une lofig                       */
/****************************************************************************/
chain_list *cbh_getToBeTreatedList(toBeTreatedList,lofig)
chain_list  *toBeTreatedList;
lofig_list  *lofig;
{
  int            constraint;
  locon_list    *inList;
  losig_list    *sig;

  inList = lofig->LOCON;
  for (;inList;inList=inList->NEXT)
  {
    sig = inList->SIG;
    constraint = cbh_getConstraint(sig);
    if ((constraint==CBH_CONSTRAINT_1)||(constraint==CBH_CONSTRAINT_0))
    {
      if (toBeTreatedList)
        cbh_addSigToTreat(toBeTreatedList,sig);
      else
        toBeTreatedList = addchain(toBeTreatedList,sig);
    }
  }
  return toBeTreatedList;
}

/*}}}************************************************************************/
/*{{{                    cbh_doLoinsToLofig()                               */
/*                                                                          */
/* fait la correspondance entre les loins et les lofigs                     */
/****************************************************************************/
void cbh_doLoinsToLofig(mainlofig)
lofig_list  *mainlofig;
{
  loins_list    *loins;

  for (loins=mainlofig->LOINS;loins;loins=loins->NEXT)
    cbh_doMatchLofig(loins);
}

/*}}}************************************************************************/
/*{{{                    cbh_doMatchLofig()                                 */
/*                                                                          */
/* fait la correspondance entre une loins et sa lofig                       */
/****************************************************************************/
lofig_list *cbh_doMatchLofig(loins)
loins_list  *loins;
{
  lofig_list    *lofig;

  lofig = getloadedlofig(loins->FIGNAME);
  cbh_doMatchLocon(lofig,loins);
  loins->USER = addptype(loins->USER,CBH_TYPE_CST,lofig);
  return lofig;
}

/*}}}************************************************************************/
/*{{{                    cbh_getLofigFromLoins()                            */
/*                                                                          */
/* recupere la correspondance d'une loins (lofigs)                          */
/****************************************************************************/
lofig_list *cbh_getLofigFromLoins(loins)
loins_list  *loins;
{
  ptype_list    *ptype;

  if ((ptype=getptype(loins->USER,CBH_TYPE_CST)))
    return ptype->DATA;
  else
    CBH_PRINT("pas de loins ptype pour %s\n",loins->INSNAME);
  return NULL;
}

/*}}}************************************************************************/
/*{{{                    cbh_doMatchLocon()                                 */
/*                                                                          */
/* fait la correspondance entre les locons des loins et des lofigs          */
/****************************************************************************/
void cbh_doMatchLocon(lofig,loins)
lofig_list  *lofig;
loins_list  *loins;
{
  locon_list    *loinsLocon;
  locon_list    *matchLocon;

  for (loinsLocon=loins->LOCON;loinsLocon;loinsLocon=loinsLocon->NEXT)
  {
    matchLocon = getlocon(lofig,loinsLocon->NAME);
    loinsLocon->DIRECTION = matchLocon->DIRECTION;
    loinsLocon->USER = addptype(loinsLocon->USER,CBH_TYPE_LOCON,matchLocon);
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_getMatchLocon()                                */
/*                                                                          */
/* renvoie le locon (correspondant au locon d'une loins) de la lofig        */
/****************************************************************************/
locon_list *cbh_getMatchLocon(locon)
locon_list  *locon;
{
  ptype_list    *ptype;

  if ((ptype=getptype(locon->USER,CBH_TYPE_LOCON)))
    return ptype->DATA;
  else
    CBH_PRINT("pas de loinslocon ptype pour %s:%s\n",
              ((loins_list*)locon->ROOT)->INSNAME,locon->NAME);
  return NULL;
}

/*}}}************************************************************************/
/*{{{                    cbh_getLoconList()                                 */
/*                                                                          */
/* renvoie une chain_list de locon attache a un signal                      */
/****************************************************************************/
chain_list *cbh_getLoconList(sig)
losig_list  *sig;
{
  ptype_list    *ptype;

  if ((ptype=getptype(sig->USER,LOFIGCHAIN)))
    return ptype->DATA;
  else
    CBH_PRINT("pas de loconins pour le signal %s\n",(char*)(sig->NAMECHAIN->DATA));
  return NULL;
}

/*}}}************************************************************************/
/*{{{                    cbh_propagate()                                    */
/*                                                                          */
/* propage une transition d'un signal sur toutes les loins connectees       */
/****************************************************************************/
chain_list *cbh_propagate(toBeTreatedList)
chain_list  *toBeTreatedList;
{
  losig_list    *losig;
  chain_list    *loconList;
  chain_list    *hasBeenTreated;

  hasBeenTreated = toBeTreatedList;
  losig = toBeTreatedList->DATA;
  loconList = cbh_getLoconList(losig);

  for (;loconList;loconList=loconList->NEXT)

    toBeTreatedList = cbh_applyConstraint(hasBeenTreated,loconList->DATA);

  return cbh_delTreatedList(toBeTreatedList,hasBeenTreated);
}

/*}}}************************************************************************/
/*{{{                    cbh_applyConstraint()                              */
/*                                                                          */
/* applique une contrainte a une loins et ajoute les signaux a traiter      */
/****************************************************************************/
chain_list *cbh_applyConstraint(toBeTreatedList,loconIns)
chain_list  *toBeTreatedList;
locon_list  *loconIns;
{
  pCircuit       cct;
  char          *outf;
  char           outz[256];
  char           outc[256];
  int            constraint;
  lofig_list    *lofig;
  loins_list    *loins;
  locon_list    *loinsOut;
  locon_list    *lofigOut;
  chain_list    *chainx;
  chain_list    *loinsinout[2];
  chain_list    *lofiginout[2];
  chain_list    *lofigInList;
  pNode          bddf;
  pNode          bddc;
  pNode          bddz;


  if (loconIns->TYPE=='E') return toBeTreatedList;

  // on recupere la liste des locons de la loins la lofig et le circuit
  loins = (loins_list*)loconIns->ROOT;
  cbh_getLoconFromLoins(loins,loinsinout,NULL);
  lofig = cbh_getLofigFromLoins(loins);
  cbh_getLoconFromLofig(lofig,lofiginout,NULL);
  cct = cbh_getcctfromlofig(lofig);

  cbh_setCstToLofig(loins);

  if (CBH_TRACE) cbh_printLofigCst(lofig,loins,NULL,NULL);

  for (chainx=loinsinout[CBH_OUT];chainx;chainx=chainx->NEXT)
  {
    loinsOut = (locon_list*)chainx->DATA;
    lofigOut = cbh_getMatchLocon(loinsOut);
    outf = lofigOut->NAME;
    sprintf(outz,"%s__hzfunc"  ,outf);
    sprintf(outc,"%s__conflict",outf);
    bddf = searchOutputCct(cct,outf);
    bddc = searchOutputCct(cct,outc);
    bddz = searchOutputCct(cct,outz);
    lofigInList = lofiginout[CBH_IN];

    if (cbh_propageBdd(bddc,cct,lofigInList)==CBH_CONSTRAINT_1)
    {// si on a un conflit
      if (cbh_setConstraint(loinsOut->SIG,CBH_CONSTRAINT_X))
      {
        cbh_addSigToTreat(toBeTreatedList,loinsOut->SIG);
        CBH_PRINT("%4sconflicting      from%8s.%-4s on %s\n","",loins->INSNAME,
                  loconIns->NAME,getsigname(loinsOut->SIG));
      }
    }
    else if (cbh_propageBdd(bddz,cct,lofigInList)==CBH_CONSTRAINT_1)
    {// si on a haute  impedance
      if (cbh_setConstraint(loinsOut->SIG,CBH_CONSTRAINT_Z))
      {
        cbh_addSigToTreat(toBeTreatedList,loinsOut->SIG);
        CBH_PRINT("%4shz ouput         from%8s.%-4s on %s\n","",loins->INSNAME,
                  loconIns->NAME,getsigname(loinsOut->SIG));
      }
    }
    else
    {// sinon si rien de tout ca...
      constraint = cbh_propageBdd(bddf,cct,lofigInList);
      if ((constraint==CBH_CONSTRAINT_1)||(constraint==CBH_CONSTRAINT_0))
      {
        switch (cbh_setConstraint(loinsOut->SIG,constraint))
        {
          case 1 : CBH_PRINT("%4spropagation of %c from%8s.%-4s on %s\n",
                             "",cbh_cst(constraint),loins->INSNAME,loconIns->NAME,
                             getsigname(loinsOut->SIG));

          case 2 : cbh_addSigToTreat(toBeTreatedList,loinsOut->SIG);
          default: break;
        }
      }
      else CBH_PRINT("%4sno propagation   from%8s.%-4s on %s\n", "",
                     loins->INSNAME,loconIns->NAME,getsigname(loinsOut->SIG));
    }
  }
  cbh_delCstFromLofig(loins);
  cbh_freeInOutList(loinsinout);
  cbh_freeInOutList(lofiginout);
  return toBeTreatedList;
}

/*}}}************************************************************************/
/*{{{                    cbh_cst()                                          */
/*                                                                          */
/* renvoie un char representant la contrainte                               */
/****************************************************************************/
char cbh_cst(constraint)
int         constraint;
{
  switch (constraint)
  {
    case CBH_NO_CONSTRAINT  : return '?';
    case CBH_CONSTRAINT_0   : return '0';
    case CBH_CONSTRAINT_1   : return '1';
    case CBH_CONSTRAINT_X   : return 'C';
    case CBH_CONSTRAINT_Z   : return 'Z';
    case CBH_CONSTRAINT_U   : return 'U';
    case CBH_CST_FULL       : return 'F';
    default                 : return '?';
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_getLoconCst()                                  */
/*                                                                          */
/* renvoie  la contrainte affectee a un locon                               */
/****************************************************************************/
int cbh_getLoconCst(locon)
locon_list  *locon;
{
  ptype_list    *ptype;

  if (!locon->USER) return 0;
  ptype = getptype(locon->USER,CBH_TYPE_CST);
  if (ptype) return (int)(long)ptype->DATA;
  else return 0;
}

/*}}}************************************************************************/
/*{{{                    cbh_delLoconCst()                                  */
/*                                                                          */
/* detruit  la contrainte affectee a un locon                               */
/****************************************************************************/
void cbh_delLoconCst(locon)
locon_list  *locon;
{
  if (cbh_getLoconCst(locon))
    locon->USER = delptype(locon->USER,CBH_TYPE_CST);
}

/*}}}************************************************************************/
/*{{{                    cbh_setLoconCst()                                  */
/*                                                                          */
/* positionne la contrainte affectee a un locon                             */
/****************************************************************************/
void cbh_setLoconCst(locon,cst)
locon_list  *locon;
int          cst;
{
  cbh_delLoconCst(locon);
  locon->USER = addptype(locon->USER,CBH_TYPE_CST,(void*)(long)cst);
}

/*}}}************************************************************************/
/*{{{                    cbh_delConstraint()                                */
/*                                                                          */
/* desaffecte une contrainte a un signal                                    */
/****************************************************************************/
void cbh_delConstraint(losig)
losig_list  *losig;
{
  if (cbh_getConstraint(losig))
    losig->USER = delptype(losig->USER,CBH_TYPE_CST);
}

/*}}}************************************************************************/
/*{{{                    cbh_getConstraint()                                */
/*                                                                          */
/* renvoie  la contrainte affectee a un signal                              */
/****************************************************************************/
int cbh_getConstraint(losig)
losig_list  *losig;
{
  ptype_list    *ptype;

  ptype = getptype(losig->USER,CBH_TYPE_CST);
  if (ptype) return (int)(long)ptype->DATA;
  else return 0;
}

/*}}}************************************************************************/
/*{{{                    cbh_setConstraint()                                */
/*                                                                          */
/* affecte une contrainte a un signal                                       */
/****************************************************************************/
int cbh_setConstraint(sig,constraint)
losig_list  *sig;
int          constraint;
{
  int        eConstraint;

  if ((eConstraint=cbh_getConstraint(sig))==constraint)
  {
    CBH_PRINT("%6sunchanged constraint on %s\n","",getsigname(sig));
    return 0;
  }
  else if (eConstraint==CBH_NO_CONSTRAINT)
  {
    sig->USER = addptype(sig->USER,CBH_TYPE_CST,(void*)(long)constraint);
    return 1;
  }
  else
  {
    cbh_delConstraint(sig);
    sig->USER = addptype(sig->USER,CBH_TYPE_CST,(void*)CBH_CONSTRAINT_X);
    CBH_PRINT("%4sconflicting constraint on %s\n","",getsigname(sig));
    return 2;
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_propageBdd()                                   */
/*                                                                          */
/*  propage les contraintes d'entrees sur un bdd                            */
/****************************************************************************/
int cbh_propageBdd(bdd,cct,lofigInList)
pNode        bdd;
pCircuit     cct;
chain_list  *lofigInList;
{
  chain_list    *chainx;
  pNode          res;

  if (!bdd) return CBH_NO_CONSTRAINT;
  if      (bdd->index==1) return CBH_CONSTRAINT_1;
  else if (bdd->index==0) return CBH_CONSTRAINT_0;
  res = bdd;
  if (lofigInList)
    for (chainx=lofigInList;chainx;chainx=chainx->NEXT)
      res = cbh_constraintBdd(res,cct,chainx->DATA);
  if      (res->index==1) return CBH_CONSTRAINT_1;
  else if (res->index==0) return CBH_CONSTRAINT_0;
  else return CBH_CONSTRAINT_X;
}

/*}}}************************************************************************/
/*{{{                    cbh_constraintBdd()                                */
/*                                                                          */
/* applique la contrainte d'un locon sur un bdd                             */
/****************************************************************************/
pNode cbh_constraintBdd(bdd,cct,locon)
pNode        bdd;
pCircuit     cct;
locon_list  *locon;
{
  pNode      tmpBdd;

  tmpBdd = ablToBddCct(cct,createAtom(locon->NAME));
  switch (cbh_getLoconCst(locon))
  {
    case CBH_CONSTRAINT_0 : return constraintBdd(bdd,notBdd(tmpBdd));
    case CBH_CONSTRAINT_1 : return constraintBdd(bdd,tmpBdd);
    default               : return bdd;
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_addSigToTreat()                                */
/*                                                                          */
/* ajoute un signal dans la liste des signaux a traiter                     */
/****************************************************************************/
void cbh_addSigToTreat(tBTL,sig)
chain_list  *tBTL;
losig_list  *sig;
{
  if (tBTL->DATA==sig) return;
  if (tBTL->NEXT) cbh_addSigToTreat(tBTL->NEXT,sig);
  else tBTL->NEXT = addchain(NULL,sig);
}

/*}}}************************************************************************/
/*{{{                    cbh_delTreatedList()                               */
/*                                                                          */
/* supprime un signal qui vient d'etre traite                               */
/****************************************************************************/
chain_list *cbh_delTreatedList(tBTL,hBT)
chain_list  *tBTL;
chain_list  *hBT;
{
  return delchain(tBTL,hBT);
}

/*}}}************************************************************************/
/*{{{                    cbh_sim()                                          */
/*                                                                          */
/* fait une simulation sur une lofig                                        */
/****************************************************************************/
void cbh_sim(lofig)
lofig_list  *lofig;
{
  chain_list    *toBeTreatedList = NULL;
  losig_list    *sig;
  int            i;

  if (!CBH_CLASSIFIER) return;
  if (CBH_TRACE)
  {
    printf("-----------------------------------\n");
    printf("PROPAGATION EVALUATION             \n");
    printf("              begin                \n");
    cbh_printLofigCst(lofig,NULL,NULL,NULL);
  }

  toBeTreatedList = cbh_getToBeTreatedList(toBeTreatedList,lofig);
  cbh_doLoinsToLofig(lofig);
  if (!cbh_getLoconList(lofig->LOSIG)) lofigchain(lofig);

  for (i=1;toBeTreatedList;i++)
  {
    sig = toBeTreatedList->DATA;
    CBH_PRINT("%2sstep %4d evaluation of %s\n","",i,getsigname(sig));
    toBeTreatedList = cbh_propagate(toBeTreatedList);
  }

  if (CBH_TRACE)
  {
    printf("                end                \n");
    cbh_printLofigCst(lofig,NULL,NULL,NULL);
    printf("-----------------------------------\n");
  }

  // test de la detection de propagation
  if (CBH_TEST)
  {
    lofig_list    *tmp;
    chain_list    *inout[2];

    tmp = getloadedlofig("tbufxl");
    cbh_getLoconFromLofig(tmp,inout,NULL);
    cbh_existTrans(tmp,inout[CBH_IN]->DATA,inout[CBH_OUT]->DATA,CBH_TRANS_ALL);
    cbh_confForTrans(tmp,inout[CBH_IN]->DATA,inout[CBH_OUT]->DATA,CBH_TRANS_UU|CBH_TRANS_DD);

    cbh_getLoconFromLofig(tmp,inout,NULL);
    cbh_existTrans(tmp,inout[CBH_IN]->NEXT->DATA,inout[CBH_OUT]->DATA,CBH_TRANS_ALL);

    tmp = getloadedlofig("and4x1");
    cbh_getLoconFromLofig(tmp,inout,NULL);
    cbh_setLoconCst(inout[CBH_IN]->NEXT->NEXT->DATA,CBH_CONSTRAINT_0);
    cbh_confForTrans(tmp,inout[CBH_IN]->DATA,inout[CBH_OUT]->DATA,CBH_TRANS_UU|CBH_TRANS_DD);

    cbh_delCstOnLofig(tmp,NULL,NULL);
    cbh_confForTrans(tmp,inout[CBH_IN]->DATA,inout[CBH_OUT]->DATA,CBH_TRANS_UU|CBH_TRANS_DD);

    tmp = getloadedlofig("xor2x1");
    cbh_getLoconFromLofig(tmp,inout,NULL);
    cbh_confForTrans(tmp,inout[CBH_IN]->DATA,inout[CBH_OUT]->DATA,CBH_TRANS_ALL);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                 detection de transition                               */
/****************************************************************************/
/*{{{                    cbh_existTrans()                                   */
/*                                                                          */
/* verifie si les transitions existent                                      */
/****************************************************************************/
long cbh_existTrans(lofig,in,out,type)
lofig_list  *lofig;
locon_list  *in;
locon_list  *out;
long         type;
{
  return cbh_findTrans2(lofig,in,out,type,CBH_ALLTRANS);
}

/*}}}************************************************************************/
/*{{{                    cbh_confForTrans()                                 */
/*                                                                          */
/* renvoie un ptype d'une configuration pour que la transition existe       */
/****************************************************************************/
ptype_list *cbh_confForTrans(lofig,in,out,type)
lofig_list  *lofig;
locon_list  *in;
locon_list  *out;
long         type;
{
  ptype_list    *pType;

  if (cbh_findTrans2(lofig,in,out,type,CBH_ONETRANS))
  {
    pType = cbh_resConf(NULL,lofig,in,out);
    if (CBH_DEBUG)
    {
      printf("\nfound configuration :\n");
      cbh_printLofigCst(lofig,NULL,in,out);
      cbh_printPTypeCst(pType);
      printf("\n###########################\n\n");
    }
    cbh_delCstOnLofig(lofig,0,0);
    if (pType)
      return pType;
    else
      return CBH_GOOD_TRANS;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    cbh_isInput()                                      */
/*                                                                          */
/* test si le locon est un locon d'entree                                   */
/****************************************************************************/
int cbh_isInput(locon)
locon_list  *locon;
{
  char           dir;

  return ( ((dir=locon->DIRECTION)=='I')||(dir=='T')||(dir=='B') );
}

/*}}}************************************************************************/
/*{{{                    cbh_inWithoutA()                                   */
/*                                                                          */
/* cree une chainlist des entrees sans l'entree a                           */
/****************************************************************************/
chain_list *cbh_inWithoutA(lofig,a,nb)
lofig_list  *lofig;
locon_list  *a;
int         *nb;
{
  locon_list    *locon;
  chain_list    *head;

  head = NULL;
  if (nb) *nb = 0;

  for (locon=lofig->LOCON;locon;locon=locon->NEXT)
    if (cbh_isInput(locon)&&(locon!=a))
    {
      head = addchain(head,locon);
      if (nb) (*nb)++;
    }
  return head;
}

/*}}}************************************************************************/
/*{{{                    cbh_ltob()                                         */
/*                                                                          */
/* renvoie une chaine de charactere de la representation binaire d'un long  */
/****************************************************************************/
char *cbh_ltob(l,b)
long l;
char b[33];
{
  int i;
  b[32] = '\0';
  for (i=0;i<32;i++)
    b[32-(i+1)] = ('0' + ((l>>i)%2));
  return b;
}

/*}}}************************************************************************/
/*{{{                    cbh_findTrans()                                    */
/*                                                                          */
/* trouve les transitions de type type, renvoie le types des transitions    */
/* trouvees                                                                 */
/****************************************************************************/
long cbh_findTrans2(lofig,in,out,type,mode)
lofig_list  *lofig;
locon_list  *in;
locon_list  *out;
long         type;
int          mode;
{
  int            i;
  long           tmp;
  pNode          bdd;
  pNode          bddUp,     bddDn;
  char           outCF[256];
  char           outHZ[256];
  pNode          bddZ,      bddC;
  pNode          bddI,      bddF;
  pNode          tmpBdd,    tmpBddI;
  pCircuit       cct;
  long           target;
  long           res;
  int            hz;
  char           buf[256];

  if (CBH_DEBUG)
    printf("\n###########################\n\n");
  if (CBH_DEBUG)
    printf("transition from %s to %s of %s\n",in->NAME,out->NAME,lofig->NAME);
  res     = 0;

  sprintf(outHZ,"%s__hzfunc"  ,out->NAME);
  sprintf(outCF,"%s__conflict",out->NAME);
  cct     = cbh_getcctfromlofig(lofig);
  bdd     = searchOutputCct(cct,out->NAME);
  sprintf(buf,"%s of %s has no bbd ", out->NAME,lofig->NAME);
  if ( (!bdd) && (CBH_WARN(buf,0)) )
    return 0;
  bddC    = ((bddC = searchOutputCct(cct,outCF))) ? notBdd(bddC) : BDD_one;
  if ((!(hz = 0)) && (bddZ = searchOutputCct(cct,outHZ)))
  {
    bddZ  = notBdd(bddZ);
    hz ++;
  }
  else
    bddZ  = BDD_one;

  if (mode==CBH_ALLTRANS)
    cbh_delCstOnLofig(lofig,0,0);

  bdd     = cbh_constraintBddWithCst(lofig,bdd ,cct);
  bddC    = cbh_constraintBddWithCst(lofig,bddC,cct);
  bddZ    = cbh_constraintBddWithCst(lofig,bddZ,cct);

  bddUp   = applyBinBdd(AND,       bdd ,applyBinBdd(AND,bddZ,bddC));
  bddDn   = applyBinBdd(AND,notBdd(bdd),applyBinBdd(AND,bddZ,bddC));

  tmpBddI = BDD_one;
  
  for (i = 0, tmp = type; i < 32; i ++, tmp = type >> i)
    if ((tmp % 2))
    {
      if (CBH_DEBUG)
        cbh_printTrans(i);

      target = cbh_decodeTrans(in,out,i,CBH_SET_TRANS);
      bddI   = (target == CBH_CONSTRAINT_0) ? bddDn : bddUp;
      bddI   = (hz) ? applyBinBdd(OR,bddI,notBdd(bddZ)) : bddI;
      bddI   = cbh_constraintBdd(bddI,cct,in);
      target = cbh_decodeTrans(in,out,i,CBH_CHK_TRANS);
      bddF   = (target == CBH_CONSTRAINT_0) ? bddDn : bddUp;
      bddF   = cbh_constraintBdd(bddF,cct,in);

      if (!zeroBdd(tmpBdd = applyBinBdd(AND,bddF,bddI)))
      {
        cbh_setCstWithBdd(lofig,tmpBdd,cct);
        if (CBH_DEBUG)
        {
          printf ("possible transition\n");
          cbh_printLofigCst(lofig,NULL,in,out);
        }
        if (mode == CBH_ONETRANS)
          if (zeroBdd(tmpBddI = applyBinBdd(AND,tmpBddI,tmpBdd)))
            break;
        res |= 1<<i;
      }
      else
      {
        if (CBH_DEBUG)
          printf ("->impossible transition\n");
        if (mode==CBH_ONETRANS)
          break;
      }
    }

  if (res == type)
  {
    if (CBH_DEBUG)
      printf("\n->ok for all types of transitions check\n");
    if (mode == CBH_ONETRANS)
      cbh_setCstWithBdd(lofig,tmpBdd,cct);
  }
  else if (mode == CBH_ONETRANS)
  {
    res = 0;
    if (CBH_DEBUG)
    {
      printf("\nfailed all types of transitions check for one configuration\n");
      cbh_printLofigCst(lofig,NULL,in,out);
    }
  }

  if (CBH_DEBUG)
  {
    printf("\nresult %#010lx\n",res);
    if (mode==CBH_ALLTRANS)
      printf("\n###########################\n\n");
  }

  if (mode==CBH_ALLTRANS)
    cbh_delCstOnLofig(lofig,0,0);

  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_delCstedLoconFromList()                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cbh_delCstedLoconFromList(inList)
chain_list  *inList;
{
  chain_list    *res;
  chain_list    *head;

  res = inList;
  head = res;

  for (;res;res=res->NEXT)
    if (cbh_getLoconCst(res->DATA)) head = delchain(head,res->DATA);
  return head;
}

/*}}}************************************************************************/
/*{{{                    cbh_setCstWithBdd()                                */
/*                                                                          */
/* positionne les contraintes sur les locons qui verifie le bdd             */
/****************************************************************************/
int cbh_setCstWithBdd(lofig,bdd,cct)
lofig_list  *lofig;
pNode        bdd;
pCircuit     cct;
{
  locon_list    *locon;
  char          *name;

  if (oneBdd(bdd)) return 1;
  else if (zeroBdd(bdd)) return 0;
  else
  {
    name = searchIndexCct(cct,bdd->index);
    for (locon=lofig->LOCON;locon;locon=locon->NEXT)
      if (locon->NAME==name) break;
    if (!locon) return 0;
    if (cbh_setCstWithBdd(lofig,bdd->high,cct))
    {
      cbh_setLoconCst(locon,CBH_CONSTRAINT_1);
      return 1;
    }
    else if (cbh_setCstWithBdd(lofig,bdd->low,cct))
    {
      cbh_setLoconCst(locon,CBH_CONSTRAINT_0);
      return 1;
    }
    else return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_decodeTrans()                                  */
/*                                                                          */
/* decode le type de transition et affecte les contraintes des locons       */
/****************************************************************************/
long cbh_decodeTrans(in,out,code,mode)
locon_list  *in;
locon_list  *out;
int          code;
int          mode;
{
  if (mode==CBH_CHK_TRANS)
    switch (code)
    {
      case 0  : code = 3; break;
      case 1  : code = 2; break;
      case 2  : code = 1; break;
      case 3  : code = 0; break;
      default : printf("not implemented \n"); return 0;
    }

  switch (code)
  {
    case 0  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_0 ,out,CBH_CONSTRAINT_0 );
              break;
    case 1  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_0 ,out,CBH_CONSTRAINT_1 );
              break;
    case 2  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_1 ,out,CBH_CONSTRAINT_0 );
              break;
    case 3  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_1 ,out,CBH_CONSTRAINT_1 );
              break;
    case 4  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_0 ,out,CBH_CONSTRAINT_X );
              break;
              /*    case 5  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_0 ,out,CBH_CONSTRAINT_Z );
                    break;
                    case 6  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_0 ,out,CBH_NO_CONSTRAINT);
                    break;
                    case 7  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_1 ,out,CBH_CONSTRAINT_X );
                    break;
                    case 8  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_1 ,out,CBH_CONSTRAINT_Z );
                    break;
                    case 9  : cbh_setLoconForTrans(in,CBH_CONSTRAINT_1 ,out,CBH_NO_CONSTRAINT);
                    break;
                    case 10 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_X ,out,CBH_CONSTRAINT_0 );
                    break;
                    case 11 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_X ,out,CBH_CONSTRAINT_1 );
                    break;
                    case 12 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_X ,out,CBH_CONSTRAINT_X );
                    break;
                    case 13 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_X ,out,CBH_CONSTRAINT_Z );
                    break;
                    case 14 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_X ,out,CBH_NO_CONSTRAINT);
                    break;
                    case 15 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_Z ,out,CBH_CONSTRAINT_0 );
                    break;
                    case 16 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_Z ,out,CBH_CONSTRAINT_1 );
                    break;
                    case 17 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_Z ,out,CBH_CONSTRAINT_X );
                    break;
                    case 18 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_Z ,out,CBH_CONSTRAINT_Z );
                    break;
                    case 19 : cbh_setLoconForTrans(in,CBH_CONSTRAINT_Z ,out,CBH_NO_CONSTRAINT);
                    break;
                    case 20 : cbh_setLoconForTrans(in,CBH_NO_CONSTRAINT,out,CBH_CONSTRAINT_0 );
                    break;
                    case 21 : cbh_setLoconForTrans(in,CBH_NO_CONSTRAINT,out,CBH_CONSTRAINT_1 );
                    break;
                    case 22 : cbh_setLoconForTrans(in,CBH_NO_CONSTRAINT,out,CBH_CONSTRAINT_X );
                    break;
                    case 23 : cbh_setLoconForTrans(in,CBH_NO_CONSTRAINT,out,CBH_CONSTRAINT_Z );
                    break;
                    case 24 : cbh_setLoconForTrans(in,CBH_NO_CONSTRAINT,out,CBH_NO_CONSTRAINT);
                    break;*/
  }
  return cbh_getLoconCst(out);
}

/*}}}************************************************************************/
/*{{{                    cbh_printTrans()                                   */
/*                                                                          */
/* affiche le type de transition  traitee                                   */
/****************************************************************************/
void cbh_printTrans(code)
long        code;
{
  printf("\ntransition de type ");
  switch (code)
  {
    case 0  : printf("UU\n"); break;
    case 1  : printf("UD\n"); break;
    case 2  : printf("DU\n"); break;
    case 3  : printf("DD\n"); break;
  }
  printf("=====================\n");
}

/*}}}************************************************************************/
/*{{{                    cbh_printPTypeCst()                                */
/*                                                                          */
/* affiche la ptype_list locon-contrainte                                   */
/****************************************************************************/
void cbh_printPTypeCst(pType)
ptype_list  *pType;
{
  locon_list    *locon;

  printf("\n resultant ptype for transition:\n");
  for (;pType;pType=pType->NEXT)
  {
    locon = pType->DATA;
    printf("%4s%-10s: set to %c\n","",locon->NAME,cbh_cst(pType->TYPE));
  }
  printf("\n");
}

/*}}}************************************************************************/
/*{{{                    cbh_setLoconForTrans()                             */
/*                                                                          */
/* affecte les contraintes de transitions sur les locons                    */
/****************************************************************************/
void cbh_setLoconForTrans(in,cstin,out,cstout)
locon_list  *in;
int          cstin;
locon_list  *out;
int          cstout;
{
  cbh_setLoconCst(in,cstin);
  cbh_setLoconCst(out,cstout);
}

/*}}}************************************************************************/
/*{{{                    cbh_resConf()                                      */
/*                                                                          */
/* lorsque l'on a une bonne configuration des locons/contraintes            */
/* cree une ptype_list de celle-ci                                          */
/****************************************************************************/
ptype_list *cbh_resConf(loins,lofig,in,out)
loins_list  *loins;
lofig_list  *lofig;
locon_list  *in;
locon_list  *out;
{
  ptype_list    *res;
  locon_list    *locon;
  int            cst;

  res = NULL;

  if (loins)
    locon = loins->LOCON;
  else if (lofig)
    locon = lofig->LOCON;
  for (;locon;locon=locon->NEXT)
    if ((locon != in) && (locon != out) && cbh_isInput(locon))
      if ((cst = cbh_getLoconCst(locon)))
        res = addptype(res,cst,locon);
  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_setCstToLofig()                                */
/*                                                                          */
/* met les contraintes sur les locons de la lofig (les contraintes sont sur */
/* les signaux qui attaquent la loins)                                      */
/****************************************************************************/
/* -july, the 22th 2002- on positionne desormais les contraintes egalement  */
/*                       sur les locons de la loins                         */
/****************************************************************************/
void cbh_setCstToLofig(loins)
loins_list  *loins;
{
  int            constraint;
  lofig_list    *lofig;
  locon_list    *loinsLocon;
  locon_list    *lofigLocon;

  loinsLocon = loins->LOCON;
  lofigLocon = cbh_getMatchLocon(loinsLocon);
  lofig = (lofig_list*)lofigLocon->ROOT;

  for (;loinsLocon;loinsLocon=loinsLocon->NEXT)
  {
    lofigLocon = cbh_getMatchLocon(loinsLocon);
    constraint = cbh_getConstraint(loinsLocon->SIG);
    cbh_setLoconCst(lofigLocon,constraint);
    cbh_setLoconCst(loinsLocon,constraint);
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_constraintBddWithCst()                         */
/*                                                                          */
/* applique les contraintes des locons sur le bdd                           */
/****************************************************************************/
pNode cbh_constraintBddWithCst(lofig,bdd,cct)
lofig_list  *lofig;
pNode        bdd;
pCircuit     cct;
{
  pNode          res;
  chain_list    *loconIn;

  res = bdd;
  loconIn = cbh_inWithoutA(lofig,NULL,NULL);
  for (;loconIn;loconIn=loconIn->NEXT)
    res = cbh_constraintBdd(res,cct,loconIn->DATA);
  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_delCstOnLofig()                                */
/*                                                                          */
/* delete toutes les contraintes de la lofig sauf in et out                 */
/****************************************************************************/
void cbh_delCstOnLofig(lofig,in,out)
lofig_list  *lofig;
locon_list  *in;
locon_list  *out;
{
  locon_list    *locon;

  for (locon=lofig->LOCON;locon;locon=locon->NEXT)
    if ((locon!=in)&&(locon!=out)) cbh_delLoconCst(locon);
}

/*}}}************************************************************************/
/*{{{                    cbh_delCstFromLofig()                              */
/*                                                                          */
/* efface les contraintes des locons de la lofig                            */
/****************************************************************************/
void cbh_delCstFromLofig(loins)
loins_list  *loins;
{
  locon_list    *loinsLocon;

  for (loinsLocon=loins->LOCON;loinsLocon;loinsLocon=loinsLocon->NEXT)
    cbh_delLoconCst(cbh_getMatchLocon(loinsLocon));
}

/*}}}************************************************************************/
/*{{{                    cbh_setLofigCstOnLoins()                           */
/*                                                                          */
/* recupere les contraintes du modele sur l'instance                        */
/****************************************************************************/
void cbh_setLofigCstOnLoins(loins)
loins_list  *loins;
{
  locon_list    *loinsLocon;
  locon_list    *lofigLocon;
  lofig_list    *lofig;

  lofig = cbh_getLofigFromLoins(loins);
  for (loinsLocon=loins->LOCON;loinsLocon;loinsLocon=loinsLocon->NEXT)
  {
    lofigLocon = cbh_getMatchLocon(loinsLocon);
    cbh_setLoconCst(loinsLocon,cbh_getLoconCst(lofigLocon));
  }
  cbh_delCstOnLofig(lofig,NULL,NULL);
}

/*}}}************************************************************************/
/*{{{                    cbh_existLoinsTrans()                              */
/*                                                                          */
/* verifie si une transition existe sur une loins                           */
/****************************************************************************/
long cbh_existLoinsTrans(loins,in,out,type,cstList)
loins_list  *loins;
locon_list  *in;
locon_list  *out;
long         type;
ptype_list  *cstList;
{
  locon_list    *loinsLocon;
  locon_list    *lofigLocon;
  lofig_list    *lofig;
  long           res;

  if (!CBH_CLASSIFIER) return 0;
  // set les contraintes sur la lofig
  if (!(lofig=cbh_getLofigFromLoins(loins)))
    lofig = cbh_doMatchLofig(loins);
  cbh_delCstOnLofig(lofig,NULL,NULL);
  for (;cstList;cstList=cstList->NEXT)
  {
    loinsLocon = cstList->DATA;
    if (!(lofigLocon=cbh_getMatchLocon(loinsLocon)))
    {
      cbh_doMatchLocon(lofig,loins);
      lofigLocon = cbh_getMatchLocon(loinsLocon);
    }
    cbh_setLoconCst(lofigLocon,cstList->TYPE);
  }

  res = cbh_existTrans(lofig,cbh_getMatchLocon(in),cbh_getMatchLocon(out),type);
  cbh_setLofigCstOnLoins(loins);
  cbh_delCstOnLofig(lofig,NULL,NULL);

  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_confLoinsForTrans()                            */
/*                                                                          */
/* trouve une configuration pour que la transition existe sur une loins     */
/****************************************************************************/
ptype_list *cbh_confLoinsForTrans(loins,in,out,type,cstList)
loins_list  *loins;
locon_list  *in;
locon_list  *out;
long         type;
ptype_list  *cstList;
{
  locon_list    *loinsLocon;
  locon_list    *lofigLocon;
  lofig_list    *lofig;
  ptype_list    *res;

  if (!CBH_CLASSIFIER)
    return NULL;
  // set contraints on lofig
  if (!(lofig=cbh_getLofigFromLoins(loins)))
    lofig = cbh_doMatchLofig(loins);
  cbh_delCstOnLofig(lofig,NULL,NULL);
  for (;cstList;cstList=cstList->NEXT)
  {
    loinsLocon = cstList->DATA;
    if (!(lofigLocon = cbh_getMatchLocon(loinsLocon)))
    {
      cbh_doMatchLocon(lofig,loins);
      lofigLocon = cbh_getMatchLocon(loinsLocon);
    }
    cbh_setLoconCst(lofigLocon,cstList->TYPE);
  }

  res = cbh_confForTrans(lofig,cbh_getMatchLocon(in),cbh_getMatchLocon(out),
                         type);
  cbh_setLofigCstOnLoins(loins);
  res = cbh_mvResLofigLoins(loins,res);
  //  cbh_delCstOnLofig(lofig,NULL,NULL);

  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_mvResLofigLoins()                              */
/*                                                                          */
/* convertit un ptype de resultat de lofig en ptype de resultat de loins    */
/***************************************************************************/
ptype_list *cbh_mvResLofigLoins(loins,res)
loins_list  *loins;
ptype_list  *res;
{
  locon_list    *loinsLocon;
  locon_list    *lofigLocon;
  ptype_list    *chain;

  if (res==CBH_GOOD_TRANS) return res;
  if ((chain=res))
    for (lofigLocon=chain->DATA;chain;chain=chain->NEXT)
    {
      lofigLocon=chain->DATA;
      for (loinsLocon=loins->LOCON;loinsLocon;loinsLocon=loinsLocon->NEXT)
        if (cbh_getMatchLocon(loinsLocon)==lofigLocon)
        {
          chain->DATA = loinsLocon;
          break;
        }
    }
  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_getHZCstFLoins()                               */
/*                                                                          */
/* renvoie la liste des contraintes pour que l'instance ne soit pas HZ      */
/****************************************************************************/
ptype_list *cbh_getHZCstFLoins(loins,insOut,cstList,enable)
loins_list  *loins;
locon_list  *insOut;
ptype_list  *cstList;
int          enable;
{
  locon_list    *loinsLocon;
  locon_list    *lofigLocon;
  lofig_list    *lofig;
  locon_list    *out;
  ptype_list    *res;

  if (!CBH_CLASSIFIER) return NULL;
  // set les contraintes sur la lofig
  if (!(lofig=cbh_getLofigFromLoins(loins)))
    lofig = cbh_doMatchLofig(loins);
  cbh_delCstOnLofig(lofig,NULL,NULL);
  for (;cstList;cstList=cstList->NEXT)
  {
    loinsLocon = cstList->DATA;
    if (!(lofigLocon=cbh_getMatchLocon(loinsLocon)))
    {
      cbh_doMatchLocon(lofig,loins);
      lofigLocon = cbh_getMatchLocon(loinsLocon);
    }
    cbh_setLoconCst(lofigLocon,cstList->TYPE);
  }
  if (!(out=cbh_getMatchLocon(insOut)))
  {
    cbh_doMatchLocon(lofig,loins);
    out = cbh_getMatchLocon(insOut);
  }
  if (!out) return NULL;

  res = cbh_getHzCst(lofig,out,enable);
  cbh_setLofigCstOnLoins(loins);
  res = cbh_mvResLofigLoins(loins,res);
  //  cbh_delCstOnLofig(lofig,NULL,NULL);
  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_getHzCst()                                     */
/*                                                                          */
/* renvoie la liste des contraintes pour que la lofig ne soit pas HZ        */
/****************************************************************************/
ptype_list *cbh_getHzCst(lofig,out,enable)
lofig_list  *lofig;
locon_list  *out;
int          enable;
{
  pCircuit       cct;
  char           outHZ[256];
  pNode          bddZ;

  cct = cbh_getcctfromlofig(lofig);
  sprintf(outHZ,"%s__hzfunc",out->NAME);
  if ((bddZ=searchOutputCct(cct,outHZ)))
  {
    if (enable) bddZ = notBdd(bddZ);
    bddZ = cbh_constraintBddWithCst(lofig,bddZ,cct);
    cbh_setCstWithBdd(lofig,bddZ,cct);
  }
  else return NULL;
  return (cbh_resConf(NULL,lofig,NULL,out));

}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
ptype_list *cbh_getCstToEnableLoins(loins,insOut,cstList)
loins_list  *loins;
locon_list  *insOut;
ptype_list  *cstList;
{
  return cbh_getHZCstFLoins(loins,insOut,cstList,1);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
ptype_list *cbh_getCstToDisableLoins(loins,insOut,cstList)
loins_list  *loins;
locon_list  *insOut;
ptype_list  *cstList;
{
  return cbh_getHZCstFLoins(loins,insOut,cstList,0);
}

/*}}}************************************************************************/
/*}}}                                                                      **/
/*{{{fonctions desuetes qui servaient a l'ancienne version de findTrans    **/
/**                                                                        **/
/**                                                                        **/
/****************************************************************************/
/****************************************************************************/

///****************************************************************************/
///*                       cbh_findTrans()                                    */
///* trouve les transitions de type type, renvoie le types des transitions    */
///* trouvees                                                                 */
///****************************************************************************/
//long cbh_findTrans(lofig,in,out,type,mode)
//lofig_list  *lofig;
//locon_list  *in;
//locon_list  *out;
//long         type;
//int          mode;
//{
//  int            i;
//  long           tmp;
//  pNode          bdd;
//  pNode          tmpbdd;
//  pCircuit       cct;
//  long           target;
//  chain_list    *loconList;
//  long           res;
//  char           bin[33];
//  long           combi;
//
//  if (CBH_DEBUG) printf("###########################\n");
//  if (CBH_DEBUG) printf("transition de %s a %s of %s\n",
//                        in->NAME,out->NAME,lofig->NAME);
//  res = 0;
//  cct = cbh_getcctfromlofig(lofig);
//  bdd = searchOutputCct(cct,out->NAME);
//  loconList = cbh_inWithoutA(lofig,in,NULL);
//  cbh_delCstOnLofig(lofig,0,0);
//
//  for (i=0,tmp=type;i<32;i++,tmp=type>>i)
//    if ((tmp%2))
//    {
//      if (CBH_DEBUG) cbh_printTrans(i);
//      target = cbh_decodeTrans(in,out,i,CBH_SET_TRANS);
//      tmpbdd = cbh_constraintBdd(bdd,cct,in);
//      // tant que l'on a pas use toutes les combinaisons
//      while ((combi=cbh_lookCombi(tmpbdd,cct,loconList,target))!=CBH_CST_FULL)
//      {
//        if (CBH_DEBUG) cbh_printLofigCst(lofig,NULL,in,out);
//        if (combi==target)
//        {
//          if (CBH_DEBUG) printf ("combi satisfaisante\n");
//
//          if (cbh_checkTrans(in,out,i,bdd,cct,lofig,loconList))
//          {
//            if (mode==CBH_ALLTRANS)
//            {
//              if (CBH_DEBUG) printf ("-->vraiment\n");
//              res |= 1<<i;
//              cbh_delCstOnLofig(lofig,in,out);
//              break;
//            }
//          }
//          else if (CBH_DEBUG) printf("-->pour init mais pas pour la suite\n");
//        }
//        else if (CBH_DEBUG) printf("echec\n");
//      }
//    }
//  if (res==type)
//  {
//    if (CBH_DEBUG) printf("->ok pour tous les types\n");
//    if (CBH_DEBUG) cbh_printLofigCst(lofig,NULL,in,out);
//    if (mode==CBH_ONETRANS) res = 0;
//  }
//  if (CBH_DEBUG) printf("resultat %#010lx 0b%s\n",res,cbh_ltob(res,bin));
//
//  if (CBH_DEBUG) printf("###########################\n");
//  cbh_delCstOnLofig(lofig,0,0);
//
//  return res;
//}
//
///****************************************************************************/
///*                       cbh_lookCombi()                                    */
///* cherche une combinaison qui soit fonctionne soit est au bout             */
///****************************************************************************/
//long cbh_lookCombi(bdd,cct,loconList,target)
//pNode        bdd;
//pCircuit     cct;
//chain_list  *loconList;
//long         target;
//{
//  chain_list    *next;
//  long           cst;
//  locon_list    *locon;
//  long           res;
//  pNode          bddCst;
//
//  // on regarde si on a atteint la cible ou si la liste est vide
//  if (!loconList)
//    if ((res=cbh_bddToCst(bdd,target))!=CBH_CONSTRAINT_U) return res;
//
//  next  = loconList->NEXT;
//  locon = loconList->DATA;
//
//  switch ((cst=cbh_getLoconCst(locon)))
//  {
//    case CBH_NO_CONSTRAINT :
//      if (CBH_DEBUG) printf("pas de contrainte pour %s\n",locon->NAME);
//      // sinon on affecte le locon avec 0
//      if (CBH_DEBUG) printf("on affecte 0 a %s\n",locon->NAME);
//      cbh_setLoconCst(locon,CBH_CONSTRAINT_0);
//      bddCst = cbh_constraintBdd(bdd,cct,locon);
//      // on regarde si on a atteint une feuille, la cible??
//      if ((res=cbh_bddToCst(bddCst,target))!=CBH_NOT_LEAF) return res;
//      else
//      {
//        // si c'est le dernier de la liste
//        if (!next) return CBH_CST_FULL;
//        // sinon on regarde la suite
//        else
//        {
//          res = cbh_lookCombi(bddCst,cct,next,target);
//          // si on a rien trouve en positionnant 0 au locon, on passe a la suite
//          // sinon on renvois le resultat
//          if ((res!=CBH_CST_FULL)||(res!=CBH_CONSTRAINT_U)) return res;
//        }
//      }
//      if (CBH_DEBUG) printf("->echec de 0 pour %s\n",locon->NAME);
//    case CBH_CONSTRAINT_0  :
//      if (CBH_DEBUG) printf("contrainte 0 pour %s\n",locon->NAME);
//      // on verifie que tout est positionne en faisant un appel en descandant
//      bddCst = cbh_constraintBdd(bdd,cct,locon);
//      if (cbh_bddToCst(bddCst,target)==CBH_NOT_LEAF)
//      {
//        if (next)
//          if (CBH_DEBUG) printf("->ca ne donne pas une feuille de bdd\n");
//          if ((res=cbh_lookCombi(bddCst,cct,next,target))!=CBH_CST_FULL) return res;
//      }
//      else if (CBH_DEBUG) printf("->ca donne une feuille de bdd donc\n");
//      // sinon on affecte 1 au locon
//      if (CBH_DEBUG) printf("on affecte 1 a %s\n",locon->NAME);
//      cbh_setLoconCst(locon,CBH_CONSTRAINT_1);
//      bddCst = cbh_constraintBdd(bdd,cct,locon);
//      // on regarde si on a atteint la cible
//      if ((res=cbh_bddToCst(bddCst,target))!=CBH_NOT_LEAF) return res;
//      else
//      {
//        // si c'est le dernier de la liste
//        if (!next) return CBH_CST_FULL;
//        else
//        {
//          res = cbh_lookCombi(bddCst,cct,next,target);
//          // si on a rien trouve en positionnant 0 au locon, on passe a la suite
//          // sinon on renvois le resultat
//          if ((res!=CBH_CST_FULL)||(res!=CBH_CONSTRAINT_U)) return res;
//        }
//      }
//      if (CBH_DEBUG) printf("->echec de 1 pour %s\n",locon->NAME);
//    case  CBH_CONSTRAINT_1 :
//      if (CBH_DEBUG) printf("contrainte 1 pour %s\n",locon->NAME);
//      // on verifie que tout est positionne en faisant un appel en descandant
//      bddCst = cbh_constraintBdd(bdd,cct,locon);
//      if (cbh_bddToCst(bddCst,target)==CBH_NOT_LEAF)
//      {
//        if (next)
//        {
//          if (CBH_DEBUG) printf("->ca ne donne pas une feuille de bdd\n");
//          if ((res=cbh_lookCombi(bddCst,cct,next,target))!=CBH_CST_FULL) return res;
//        }
//      }
//      else if (CBH_DEBUG) printf("->ca donne une feuille de bdd donc\n");
//      //    sinon on delete la contrainte et on retourne plein
//      if (CBH_DEBUG) printf("->on detruit la contrainte 1 de %s\n",locon->NAME);
//      cbh_delLoconCst(locon);
//      return CBH_CST_FULL;
//    default : return CBH_CONSTRAINT_U;
//  }
//}
//
///****************************************************************************/
///*                       cbh_evalBdd()                                      */
///* evalue un bdd en fonction des contraintes d'entrees                      */
///****************************************************************************/
//int cbh_evalBdd(bdd,cct,locon)
//pNode        bdd;
//pCircuit     cct;
//locon_list  *locon;
//{
//  char          *name;
//  locon_list    *chainx;
//  int            constraint;
//
//  if (!bdd) return 0;
//  if (bdd->index==1) return CBH_CONSTRAINT_1;
//  if (bdd->index==0) return CBH_CONSTRAINT_0;
//
//  name = searchIndexCct(cct,bdd->index);
//  for (chainx=locon;chainx;chainx=chainx->NEXT)
//    if (chainx->NAME==name)
//    {
//      if ((constraint=cbh_getLoconCst(chainx))==CBH_CONSTRAINT_0)
//        return cbh_evalBdd(bdd->low,cct,locon);
//      else if (constraint==CBH_CONSTRAINT_1)
//        return cbh_evalBdd(bdd->high,cct,locon);
//      else return /*cbh_evalBdd(bdd->high,cct,locon); */ CBH_CONSTRAINT_U;
//    }
//  return CBH_CONSTRAINT_U;
//}
//
///****************************************************************************/
///*                       cbh_checkAllCstSet()                               */
///* verifie que tous les locons sont contraint sinon renvoie le fautif       */
///****************************************************************************/
//locon_list *cbh_checkAllCstSet(locon)
//locon_list  *locon;
//{
//  for (;locon;locon=locon->NEXT)
//    if ((cbh_getLoconCst(locon)==CBH_NO_CONSTRAINT)&&cbh_isInput(locon))
//    {
//      cbh_setLoconCst(locon,CBH_CONSTRAINT_0);
//      return locon;
//    }
//  return NULL;
//}
//
///****************************************************************************/
///*                       cbh_checkTrans()                                   */
///* verifie la validite de la transition: les locons sont initialises avec   */
///* les valeurs d'init, on change les contraintes des locons de la transition*/
///* pour verifier que la transition s'effectue                               */
///****************************************************************************/
//int cbh_checkTrans(in,out,code,bdd,cct,lofig,loconList)
//locon_list  *in;
//locon_list  *out;
//int          code;
//pNode        bdd;
//pCircuit     cct;
//lofig_list  *lofig;
//chain_list  *loconList;
//{
//  int        resOut;
//  int        resBdd;
//  pNode      tmpBdd;
//
//  cbh_decodeTrans(in,out,code,CBH_CHK_TRANS);
//  tmpBdd = cbh_constraintBddWithCst(lofig,bdd,cct);
//  resOut = cbh_getLoconCst(out);
//  resBdd = cbh_evalBddUnFull(tmpBdd,cct,lofig->LOCON,resOut);
//  cbh_decodeTrans(in,out,code,CBH_SET_TRANS);
//  return (resOut==resBdd);
//}
//
///****************************************************************************/
///*                       cbh_evalBddUnFull()                                */
///* evalue un bdd meme si toutes les entrees ne sont pas contraintes         */
///****************************************************************************/
//int cbh_evalBddUnFull(bdd,cct,locon,resOut)
//pNode        bdd;
//pCircuit     cct;
//locon_list  *locon;
//int          resOut;
//{
//  int            resBdd;
//  locon_list    *unCst;
//
//  resBdd = cbh_evalBdd(bdd,cct,locon);
//  if (resBdd==resOut) return resBdd;
//  else
//  {
//    if ((unCst=cbh_checkAllCstSet(locon)))
//    {
//      if ((resBdd=cbh_evalBdd(cbh_constraintBdd(bdd,cct,unCst),cct,locon))==resOut)
//        return resBdd;
//      else
//      {
//        cbh_setLoconCst(unCst,CBH_CONSTRAINT_1);
//        if ((resBdd=cbh_evalBdd(cbh_constraintBdd(bdd,cct,unCst),cct,locon))!=resOut)
//          cbh_delLoconCst(unCst);
//        return resBdd;
//      }
//    }
//    else return resBdd;
//  }
//}
//
///****************************************************************************/
///*                       cbh_bddToCst()                                     */
///* convertit un bdd en contrainte si le bdd n'est pas terminal, renvoie     */
///* not leaf                                                                 */
///****************************************************************************/
//int cbh_bddToCst(bdd,target)
//pNode        bdd;
//int          target;
//{
//  switch (bdd->index)
//  {
//    case 1 :
//      if ((target==CBH_CONSTRAINT_1)) return target;
//      else return CBH_CONSTRAINT_U;
//    case 0 :
//      if ((target==CBH_CONSTRAINT_0)) return target;
//      else return CBH_CONSTRAINT_U;
//    default : return CBH_NOT_LEAF;
//  }
//}
//}}}
