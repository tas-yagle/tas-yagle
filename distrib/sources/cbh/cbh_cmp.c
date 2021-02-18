/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_cmp.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/
#include "cbh_cmp.h"

chain_list  *CBH_MAINREF_ABLLIST[CBH_MAXNBFUNC];
chain_list  *CBH_MAINNEW_ABLLIST[CBH_MAXNBFUNC];
chain_list  *CBH_MAINREF_BDDLIST[CBH_MAXNBFUNC];

int          CBH_PERMUT = 0;

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    cbh_cmpfanout()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
float cbh_cmpfanout(lofig,fanout)
lofig_list  *lofig;
float       fanout;
{
  cbhcomb *ptcbhcomb;

  if (!lofig->LOCON) return 0;

  if ((ptcbhcomb=cbh_getcombfromlocon(lofig->LOCON))!=NULL)
    return (ptcbhcomb->FANOUT-fanout);
  else return 0;
}

/*}}}************************************************************************/
/*{{{                    cbh_equalVarExpr()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int cbh_equalVarExpr(expr1,expr2)
chain_list *expr1;
chain_list *expr2;
{
  if (expr1==expr2)
    return 1;
  else if ((!expr1)||(!expr2))
    return 0;
  if ((ATOM(expr1)) && (ATOM(expr2)))
    return equalExpr(expr1,expr2);
  return equalVarExpr(expr1,expr2);
}

/*}}}************************************************************************/
/*{{{                    cbh_addabl()                                       */
/*                                                                          */
/* cree la liste d'abl                                                      */
/****************************************************************************/
void cbh_addabl(mainabllist,abl,name,where)
chain_list  *mainabllist[CBH_MAXNBFUNC];
chain_list  *abl;
char        *name;
int         where;
{
  abl = addchain(abl,name);
  mainabllist[where] = addchain(mainabllist[where],abl);
}

/*}}}************************************************************************/
/*{{{                    cbh_createabllist()                                */
/*                                                                          */
/* cree les deux listes d'abl                                               */
/****************************************************************************/
int cbh_createabllist(lofig,func,mainabllist,out)
lofig_list  *lofig;
int         func;
chain_list  *mainabllist[CBH_MAXNBFUNC];
chain_list  *out;
{
  chain_list    *chainx;
  cbhcomb       *ptcomb;
  cbhseq        *ptseq;
  int           i;
  locon_list    *locon;

  for (i=0;i<CBH_MAXNBFUNC;i++)
    mainabllist[i] = NULL;

  for (chainx=out;chainx;chainx=chainx->NEXT)
  {
    locon = (locon_list*)chainx->DATA;
    ptcomb = cbh_getcombfromlocon(locon);
    if (!ptcomb) return 0;

    if (ptcomb->FUNCTION)
      cbh_addabl(mainabllist,ptcomb->FUNCTION ,locon->NAME,CBH_FUNCTION);
    if (ptcomb->HZFUNC)
      cbh_addabl(mainabllist,ptcomb->HZFUNC   ,locon->NAME,CBH_HZFUNC);
    if (ptcomb->CONFLICT)
      cbh_addabl(mainabllist,ptcomb->CONFLICT ,locon->NAME,CBH_CONFLICT);
  }

  if (func==CBH_SEQ)
  {
    ptseq = cbh_getseqfromlofig(lofig);

    if (!ptseq) return 0;

    if (ptseq->RSCONFNEG)
      cbh_addabl(mainabllist,ptseq->RSCONFNEG ,lofig->NAME,CBH_RSCONFNEG);
    if (ptseq->RSCONF)
      cbh_addabl(mainabllist,ptseq->RSCONF    ,lofig->NAME,CBH_RSCONF);
    if (ptseq->SET)
      cbh_addabl(mainabllist,ptseq->SET       ,lofig->NAME,CBH_SET);
    if (ptseq->RESET)
      cbh_addabl(mainabllist,ptseq->RESET     ,lofig->NAME,CBH_RESET);
    if (ptseq->DATA)
      cbh_addabl(mainabllist,ptseq->DATA      ,lofig->NAME,CBH_DATA);
    if (ptseq->SLAVECLOCK)
      cbh_addabl(mainabllist,ptseq->SLAVECLOCK,lofig->NAME,CBH_SLAVECLOCK);
    if (ptseq->CLOCK)
      cbh_addabl(mainabllist,ptseq->CLOCK     ,lofig->NAME,CBH_CLOCK);
  }
  return 1;
}

/*}}}************************************************************************/
/*{{{                    cbh_existloop()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int cbh_existloop(lofig,loop,name)
lofig_list  *lofig;
int          loop[2];
char        *name[2];
{
  chain_list    *support;
  chain_list    *chainx;
  cbhseq        *ptseq;

  loop[CBH_Q]  = 0;
  loop[CBH_NQ] = 0;

  ptseq = cbh_getseqfromlofig(lofig);
  if (!ptseq)
    return 0;
  if (ptseq->DATA!=NULL)
  {
    support = supportChain_listExpr(ptseq->DATA);
    for (chainx = support; chainx; chainx = chainx->NEXT)
      if (chainx->DATA == ptseq->NAME)
      {
        loop[CBH_Q]  = 1;
        name[CBH_Q]  = ptseq->NAME;
      }
      else if (chainx->DATA == ptseq->NEGNAME)
      {
        loop[CBH_NQ] = 1;
        name[CBH_NQ] = ptseq->NAME;
      }
    freechain(support);
  }
  return (loop[CBH_Q]+loop[CBH_NQ]);
}

/*}}}************************************************************************/
/*{{{                    cbh_cmplofig()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int cbh_cmplofig(reflofig,newlofig)
lofig_list  *reflofig;
lofig_list  *newlofig;
{
  int            func;
  chain_list    *newinout[2];
  int            nbinout[2];
  cbhcomb       *ptcomb;
  cbhseq        *ptseq;

  // on recupere les fonctionnalites
  cbh_getLoconFromLofig(newlofig,newinout,nbinout);
  ptseq  = cbh_getseqfromlofig(newlofig);
  ptcomb = cbh_getcombfromlocon(newlofig->LOCON);
  if (ptseq!=NULL)
    func = CBH_SEQ;
  else if (ptcomb!=NULL)
    func = CBH_COMB;
  else
    func = CBH_NOTYPE;

  return cbh_cmplofig_IO(reflofig,newlofig,func,newinout,nbinout);
}

/*}}}************************************************************************/
/*{{{                    cbh_cmplofig_IO()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int cbh_cmplofig_IO(reflofig,newlofig,func,newinout,nbinout)
lofig_list  *reflofig;
lofig_list  *newlofig;
int          func;
chain_list  *newinout[2];
int          nbinout[2];
{
  chain_list    *refinout[2];
  int            i;
  int            loop[2];
  char          *refnamein[2];
  char          *newnamein[2];

  /* ajouter une comparaison de pointeur, on y gagne un peu */
  if (newlofig==reflofig)
    return 1;

  cbh_getLoconFromLofig(reflofig,refinout,nbinout);

  if ((!newinout[CBH_OUT]))
  {
    cbh_freeInOutList(refinout);
    return 1;
  }

  CBH_PERMUT   = 0;

  for (i=0;i<CBH_MAXNBFUNC;i++)
    CBH_MAINREF_BDDLIST[i] = NULL;

  // creation des listes d'abl
  cbh_createabllist(reflofig,func,CBH_MAINREF_ABLLIST,refinout[CBH_OUT]);
  cbh_createabllist(newlofig,func,CBH_MAINNEW_ABLLIST,newinout[CBH_OUT]);

  // comparaison de boucle au niveau des latchs
  if ( cbh_existloop(newlofig,loop,newnamein) !=
       cbh_existloop(reflofig,loop,refnamein))
    return cbh_freeMainAblList(0,refinout);

  // comparaison des abls
  if (cbh_cmpabl())
    return cbh_freeMainAblList(1,refinout);
  // comparaison des bdds
  else if (CBH_BDD)
  {
    if (cbh_cmpbdd_abl(newlofig,reflofig,refinout,loop,refnamein,newnamein,
                       newinout,nbinout))
      return cbh_freeMainAblList(1,refinout);
    else
      return cbh_freeMainAblList(0,refinout);
  }
  else
    return cbh_freeMainAblList(0,refinout);
}

/*}}}************************************************************************/
/*{{{                    cbh_freeMainAblList()                              */
/*                                                                          */
/* libere les mainabl et les reflocons et retourne la valeur d'entree       */
/****************************************************************************/
int cbh_freeMainAblList(ret,refinout)
chain_list *refinout[2];
int         ret;
{
  int       i;

  for (i=0;i<CBH_MAXNBFUNC;i++)
    cbh_freechain(CBH_MAINREF_BDDLIST[i]);
  cbh_freeabllist(CBH_MAINNEW_ABLLIST);
  cbh_freeabllist(CBH_MAINREF_ABLLIST);
  if (refinout)
    cbh_freeInOutList(refinout);
  return ret;
}

/*}}}************************************************************************/
/*{{{                    cbh_freeabllist()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_freeabllist(mainabllist)
chain_list  *mainabllist[CBH_MAXNBFUNC];
{
  int            i;
  chain_list    *data;
  chain_list    *chainx;

  for (i = 0; i < CBH_MAXNBFUNC; i ++)
    if (mainabllist[i])
    {
      for (chainx=mainabllist[i];chainx;chainx=chainx->NEXT)
        if ((data=chainx->DATA))
          chainx->DATA = delchain(data,data);
      freechain(mainabllist[i]);
      mainabllist[i] = NULL;
    }
}

/*}}}************************************************************************/
/*{{{                    cbh_initcct()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
pCircuit cbh_initcct(lofig,nbin,nbout,loop,nbabl)
lofig_list  *lofig;
int          nbin;
int          nbout;
int          loop[2];
int          nbabl;
{
  pCircuit      cct;

  cct = initializeCct(lofig->NAME,nbin+loop[CBH_Q]+loop[CBH_NQ],nbout+nbabl);

  return cct;
}

/*}}}************************************************************************/
/*{{{                    cbh_addincct()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_addincct(cct,loconin,loop,name)
pCircuit     cct;
chain_list  *loconin;
int          loop[2];
char        *name[2];
{
  chain_list    *chainx;

  for (chainx=loconin;chainx;chainx=chainx->NEXT)
    if (chainx->DATA)
      addInputCct(cct,((locon_list*)chainx->DATA)->NAME);
  if (loop[CBH_Q])
    addInputCct(cct,name[CBH_Q]);
  if (loop[CBH_NQ])
    addInputCct(cct,name[CBH_NQ]);
}

/*}}}************************************************************************/
/*{{{                    cbh_addoutcct()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_addoutcct(cct,bdd,name,where)
pCircuit     cct;
pNode        bdd;
char        *name;
int          where;
{
  char      buf[1024];

  strcpy(buf,name);
  switch (where)
  {
    case CBH_HZFUNC :
         strcat(buf,"__hzfunc");
         break;
    case CBH_CONFLICT :
         strcat(buf,"__conflict");
         break;
    case CBH_CLOCK :
         strcat(buf,"__clock");
         break;
    case CBH_SLAVECLOCK :
         strcat(buf,"__slaveclock");
         break;
    case CBH_DATA :
         strcat(buf,"__data");
         break;
    case CBH_RESET :
         strcat(buf,"__reset");
         break;
    case CBH_SET :
         strcat(buf,"__set");
         break;
    case CBH_RSCONF :
         strcat(buf,"__rsconf");
         break;
    case CBH_RSCONFNEG :
         strcat(buf,"__rsconfneg");
         break;
    case CBH_FUNCTION :
         break;
    default :
         return;
  }
  addOutputCct(cct,namealloc(buf),bdd);
}

/*}}}************************************************************************/
/*{{{                    cbh_cmpabl()                                       */
/*                                                                          */
/* compare deux abl et indique si elles sont identiques ou pas du tout      */
/****************************************************************************/
int cbh_cmpabl()
{
  chain_list    *refabllist;
  chain_list    *newabllist;
  chain_list    *chainx;
  chain_list    *refabl;
  chain_list    *newabl;
  int            res;
  int            resinter;
  int            i;

  res = 1;

  for (i = 0; i < CBH_MAXNBFUNC; i ++)
  {
    refabllist = CBH_MAINREF_ABLLIST[i];
    newabllist = CBH_MAINNEW_ABLLIST[i];

    if ((!refabllist)&&(newabllist))
      return 0;
    if ((refabllist)&&(!newabllist))
      return 0;

    if ((refabllist)&&(newabllist))
    {
      for (;refabllist;refabllist=refabllist->NEXT)
      {
        resinter    = 0;
        refabl      = ((chain_list*)(refabllist->DATA))->NEXT;
        for (chainx = newabllist; chainx; chainx = chainx->NEXT)
        {
          newabl    = ((chain_list*)(chainx->DATA))->NEXT;
          resinter |= cbh_equalVarExpr(refabl,newabl);
        }
        res        &= resinter;
      }
    }
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_cmpbdd_abl()                                   */
/*                                                                          */
/* compare deux bdd a partir des abl et indique si elles sont identiques    */
/* ou pas du tout                                                           */
/****************************************************************************/
int cbh_cmpbdd_abl(newlofig,reflofig,refinout,loop,refnamein,newnamein,newinout,nbinout)
lofig_list  *newlofig;
lofig_list  *reflofig;
chain_list  *refinout[2];
int          loop[2];
char        *refnamein[2];
char        *newnamein[2];
chain_list  *newinout[2];
int          nbinout[2];
{
  pCircuit       refPc;
  chain_list    *chainx;
  chain_list    *refabl;
  int            i;
  pNode          refbdd;
  chain_list    *newabllist;
  chain_list    *refabllist;
  pCircuit       existCct;
  char          *refname;
  int            res;

  existCct = cbh_getcctfromlofig(reflofig);

  if (!existCct)
  {
    refPc = cbh_initcct(reflofig,nbinout[CBH_IN],nbinout[CBH_OUT],loop,7);
    cbh_addccttolofig(reflofig,refPc);
    cbh_addincct(refPc,refinout[CBH_IN],loop,refnamein);
  }
  else refPc = existCct;

  for (i = 0; i < CBH_MAXNBFUNC; i ++)
  {
    refabllist = CBH_MAINREF_ABLLIST[i];
    newabllist = CBH_MAINNEW_ABLLIST[i];

    if ((!refabllist)&&(newabllist))
      return 0;
    if ((refabllist)&&(!newabllist))
      return 0;
    if ((refabllist)&&(newabllist))
    {
      for (chainx = refabllist; chainx; chainx = chainx->NEXT)
      {
        // ici les bdd doivent etre calcule pour chaque sorties de reflofig
        if (CAR(chainx))
        {
          refname = VALUE_ATOM((CAR(chainx)));
          refabl  = CDAR(chainx);
          refbdd  = ablToBddCct(refPc,refabl);
          cbh_addoutcct(refPc,refbdd,refname,i);
          CBH_MAINREF_BDDLIST[i] = addchain(CBH_MAINREF_BDDLIST[i],refbdd);
        }
      }
    }
  }
  res = (cbh_lookbdd(newinout[CBH_IN],nbinout,newlofig,loop,newnamein)!=NULL);
  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_cmpbdd()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
pCircuit cbh_cmpbdd(newin,newlofig,loop,newnamein,nbinout)
chain_list  *newin;
lofig_list  *newlofig;
int          loop[2];
char        *newnamein[2];
int          nbinout[2];
{
  pCircuit       newpC;
  chain_list    *chainx;
  chain_list    *newabl;
  chain_list    *refbddlist;
  chain_list    *refabllist;
  chain_list    *newabllist;
  pNode          newbdd;
  pNode          refbdd;
  char          *newname;
  int            i;
  int            res;
  int            resinter;
  int            existCct;

  existCct = (cbh_getcctfromlofig(newlofig)) ? 1 : 0;

  newpC = cbh_initcct(newlofig,nbinout[CBH_IN],nbinout[CBH_OUT],loop,7);
  cbh_addincct(newpC,newin,loop,newnamein);

  for (i = 0; i < CBH_MAXNBFUNC; i ++)
  {
    newabllist = CBH_MAINNEW_ABLLIST[i];
    refabllist = CBH_MAINREF_ABLLIST[i];
    refbddlist = CBH_MAINREF_BDDLIST[i];

    for (res = 1; newabllist; newabllist = newabllist->NEXT)
    {
      newname = (CAR(newabllist))->DATA;
      newabl  = (CAR(newabllist))->NEXT;
      newbdd  = ablToBddCct(newpC,newabl);
      cbh_addoutcct(newpC,newbdd,newname,i);
      {
        resinter = 0;
        for (chainx = refbddlist; chainx; chainx = chainx->NEXT)
        {
          refbdd = (pNode)chainx->DATA;
          if ((resinter = equalBdd(newbdd,refbdd)))
            break;
        }
      }
      res &= resinter;
      if (!res)
        break;
    }
    if (!res)
      break;
  }
  if (res)
  {
    if (!existCct) cbh_addccttolofig(newlofig,newpC);
    return newpC;
  }
  else
  {
    destroyCct(newpC);
    return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    cbh_lookbdd()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cbh_lookbdd(input,nbinout,newlofig,loop,newnamein)
chain_list  *input;
int          nbinout[2];
lofig_list  *newlofig;
int          loop[2];
char        *newnamein[2];
{
  chain_list    *tail = NULL;
  chain_list    *res  = NULL;

  if (input)
    tail = cbh_circularchain(input);
  else
    return (chain_list *) cbh_cmpbdd(input,newlofig,loop,newnamein,nbinout);
  res   = cbh_lookpermut(res,tail,nbinout[CBH_IN],newlofig,loop,newnamein,
                         nbinout);
  input = input->NEXT;
  cbh_uncircularchain(tail);
  return res;
}

/*}}}************************************************************************/
/*{{{                    cbh_lookpermut()                                   */
/*                                                                          */
/* compare deux abl et indique si elles sont identiques ou pas du tout      */
/****************************************************************************/
chain_list *cbh_lookpermut(reschain,tmpchain,n,newlofig,loop,newnamein,nbinout)
chain_list  *reschain;
chain_list  *tmpchain;
int          n;
lofig_list  *newlofig;
int          loop[2];
char        *newnamein[2];
int          nbinout[2];
{
  int   i;
  chain_list    *maskedchain;

  if (n <= 1)
  {
    CBH_PERMUT ++;
    reschain = addchain(reschain,tmpchain->DATA);
    reschain = reverse(reschain);
    /* ici on fait la comparaison des bdd */
    if (cbh_cmpbdd(reschain,newlofig,loop,newnamein,nbinout))
      return reschain ;
    reschain = reverse(reschain);
    reschain = delchain(reschain,reschain);
    return reschain ;
  }
  else
    for(i = 0; i < n; i ++, tmpchain = tmpchain->NEXT)
    {
      maskedchain = cbh_masksndhead(tmpchain);
      reschain = addchain(reschain,maskedchain->DATA);
      if (reschain != cbh_lookpermut(reschain,tmpchain,n-1,newlofig,loop,
                                     newnamein,nbinout))
      {
        cbh_unmasksndhead(tmpchain,maskedchain);
        return reschain;
      }
      cbh_unmasksndhead(tmpchain,maskedchain);
      reschain = delchain(reschain,reschain) ;
    }
  return reschain;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{     functions de traitement utile pour faire les permutations         */
/****************************************************************************/
/*{{{                    cbh_circularchain()                                */
/*                                                                          */
/* rend une chaine circulaire et retourne le dernier element de la          */
/* chaine d'entree                                                          */
/****************************************************************************/
chain_list *cbh_circularchain(chain)
chain_list  *chain;
{
  chain_list    *circlechain;

  for (circlechain=chain;circlechain->NEXT;circlechain=circlechain->NEXT);
  circlechain->NEXT=chain;
  return circlechain;
}

/*}}}************************************************************************/
/*{{{                    cbh_uncircularchain()                              */
/*                                                                          */
/* transforme une chaine circulaire en chaine simple                        */
/****************************************************************************/
void cbh_uncircularchain(tail)
chain_list  *tail;
{
  tail->NEXT = NULL;
}

/*}}}************************************************************************/
/*{{{                    cbh_masksndhead()                                  */
/*                                                                          */
/* masque le deusieme element d'une chaine circulaire                       */
/****************************************************************************/
chain_list *cbh_masksndhead(chain)
chain_list  *chain;
{
  chain_list    *sndhead;

  sndhead = chain->NEXT;
  chain->NEXT = sndhead->NEXT;
  return sndhead;
}

/*}}}************************************************************************/
/*{{{                    cbh_unmasksndhead()                                */
/*                                                                          */
/* demasque le deusieme element d'une chaine circulaire                     */
/****************************************************************************/
void cbh_unmasksndhead(chain,sndhead)
chain_list  *chain;
chain_list  *sndhead;
{
  chain->NEXT = sndhead;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
