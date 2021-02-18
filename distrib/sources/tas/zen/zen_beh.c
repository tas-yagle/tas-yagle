/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_beh.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Stephane Picault                                        */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"


/****************************************************************************/
/*{{{                    Display  Functions                                 */
/*                                                                          */
/****************************************************************************/
void display_berin_ref(ptBeRin)
berin_list  *ptBeRin;
{
  chain_list    *ScanChain;
  beout_list    *ScanBeOut;
  bebus_list    *ScanBeBus;
  beaux_list    *ScanBeAux;
  beaux_list    *ScanBeDly;
  bebux_list    *ScanBeBux;
  bereg_list    *ScanBeReg;
  bemsg_list    *ScanBeMsg;

  fprintf(stdout,"\t--> ");
  for (ScanChain=ptBeRin->OUT_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeOut = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeOut->NAME);
  }
  for (ScanChain=ptBeRin->BUS_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeBus = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeBus->NAME);
  }
  for (ScanChain=ptBeRin->AUX_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeAux = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeAux->NAME);
  }
  for (ScanChain=ptBeRin->BUX_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeBux = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeBux->NAME);
  }
  for (ScanChain=ptBeRin->REG_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeReg = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeReg->NAME);
  }
  for (ScanChain=ptBeRin->MSG_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeMsg = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeMsg->LABEL);
  }
  for (ScanChain=ptBeRin->DLY_REF;ScanChain;ScanChain=ScanChain->NEXT)
  {
    ScanBeDly = ScanChain->DATA;
    fprintf(stdout,"%s\t",ScanBeDly->NAME);
  }
  fprintf(stdout,"\n");
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void display_BeMachin(beh,name)
befig_list  *beh;
char        *name;
{
  fprintf(stdout,"%s\tis a\t",name);
  if (searchBeAux(beh,name)) fprintf(stdout,"BeAux\t");
  if (searchBeRin(beh,name)) fprintf(stdout,"BeRin\t");
  if (searchBeOut(beh,name)) fprintf(stdout,"BeOut\t");
  if (searchBeReg(beh,name)) fprintf(stdout,"BeReg\t");
  if (searchBeBus(beh,name)) fprintf(stdout,"BeBus\t");
  if (searchBeBux(beh,name)) fprintf(stdout,"BeBux\t");
  fprintf(stdout,"\n");
}

/****************************************************************************/
/*                       Display Binode                                     */
/*                                                                          */
/****************************************************************************/
void display_binode(ptCct,ptBinod)
pCircuit     ptCct;
binode_list *ptBinod;
{
  binode_list *ScanBinod;

  for (ScanBinod=ptBinod;ScanBinod;ScanBinod=ScanBinod->NEXT)
  {
    if (ScanBinod->CNDNODE)
    {
      avt_log(LOGZEN,2,"\t\t\t\tCND\t");
      //displayBdd(ScanBinod->CNDNODE,1);
      displayExprLog(LOGZEN,2,bddToAblCct(ptCct,ScanBinod->CNDNODE));
    }
    if (ScanBinod->CNDNODE)
    {
      avt_log(LOGZEN,2,"\t\t\t\tVAL\t");
      //displayBdd(ScanBinod->VALNODE,1);
      displayExprLog(LOGZEN,2,bddToAblCct(ptCct,ScanBinod->VALNODE));
    }
  }
}

/****************************************************************************/
/*                       Display Biabl                                      */
/*                                                                          */
/****************************************************************************/
void display_biabl(ptBiabl)
biabl_list *ptBiabl ;
{
  biabl_list *ScanBiabl ;

  for (ScanBiabl=ptBiabl;ScanBiabl;ScanBiabl=ScanBiabl->NEXT)
  {
    avt_log(LOGZEN,2,"\t\t%s\n",ScanBiabl->LABEL);
    if (ScanBiabl->CNDABL)
    {
      avt_log(LOGZEN,2,"\t\t\t\tCND\t");
      displayExprLog(LOGZEN,2,ScanBiabl->CNDABL);
    }
    if (ScanBiabl->VALABL)
    {
      avt_log(LOGZEN,2,"\t\t\t\tVAL\t");
      displayExprLog(LOGZEN,2,ScanBiabl->VALABL);
    }
  }
}

/****************************************************************************/
/*                       Display  Befig                                     */
/*                                                                          */
/****************************************************************************/
void display_befig(ptBeFig)
befig_list  *ptBeFig;
{
  bepor_list    *ScanBePor;
  begen_list    *ScanBeGen;
  beout_list    *ScanBeOut;
  bebus_list    *ScanBeBus;
  berin_list    *ScanBeRin;
  beaux_list    *ScanBeAux;
  beaux_list    *ScanBeDly;
  bebux_list    *ScanBeBux;
  bereg_list    *ScanBeReg;
  bemsg_list    *ScanBeMsg;

  avt_log(LOGZEN,2,"BEFIG %s(\n",ptBeFig->NAME);
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit port\n");
  for (ScanBePor=ptBeFig->BEPOR;ScanBePor;ScanBePor=ScanBePor->NEXT)
    avt_log(LOGZEN,2,"\tPOR %s\t%c\t%c\n",ScanBePor->NAME,ScanBePor->DIRECTION,
                                        ScanBePor->TYPE);
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit global constant\n");
  for (ScanBeGen=ptBeFig->BEGEN;ScanBeGen;ScanBeGen=ScanBeGen->NEXT)
    avt_log(LOGZEN,2,"\tGEN %s\t%s\n",ScanBeGen->NAME,ScanBeGen->TYPE);
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit simple output port\n");
  for (ScanBeOut=ptBeFig->BEOUT;ScanBeOut;ScanBeOut=ScanBeOut->NEXT)
  {
    avt_log(LOGZEN,2,"\tOUT %s\n",ScanBeOut->NAME);
    if (ScanBeOut->NODE)
    {
      avt_log(LOGZEN,2,"\t\tBDD\t");
     // displayBdd(ScanBeOut->NODE,1);
      displayExprLog(LOGZEN,2,bddToAblCct(ptBeFig->CIRCUI,ScanBeOut->NODE));
    }
    if (ScanBeOut->ABL)
    {
      avt_log(LOGZEN,2,"\t\tABL\t");
      displayExprLog(LOGZEN,2,ScanBeOut->ABL);
    }
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit bused output port\n");
  for (ScanBeBus=ptBeFig->BEBUS;ScanBeBus;ScanBeBus=ScanBeBus->NEXT)
  {
    avt_log(LOGZEN,2,"\tBUS %s\t%c\n",ScanBeBus->NAME,ScanBeBus->TYPE);
    if (ScanBeBus->BINODE)
    {
      avt_log(LOGZEN,2,"\t\tBI_BDD\n");
      display_binode(ptBeFig->CIRCUI,ScanBeBus->BINODE);
    }
    if (ScanBeBus->BIABL)
    {
      avt_log(LOGZEN,2,"\t\tBI_ABL\n");
      display_biabl(ScanBeBus->BIABL);
    }
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit input signal\n");
  for (ScanBeRin=ptBeFig->BERIN;ScanBeRin;ScanBeRin=ScanBeRin->NEXT)
  {
    avt_log(LOGZEN,2,"\tRIN %s\n",ScanBeRin->NAME);
    display_berin_ref(ScanBeRin);
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit simple internal signal\n");
  for (ScanBeAux=ptBeFig->BEAUX;ScanBeAux;ScanBeAux=ScanBeAux->NEXT)
  {
    avt_log(LOGZEN,2,"\tAUX %s\n",ScanBeAux->NAME);
    if (ScanBeAux->NODE)
    {
      avt_log(LOGZEN,2,"\t\tBDD\t");
      //displayBdd(ScanBeAux->NODE,1);
      displayExprLog(LOGZEN,2,bddToAblCct(ptBeFig->CIRCUI,ScanBeAux->NODE));
    }
    if (ScanBeAux->ABL)
    {
      avt_log(LOGZEN,2,"\t\tABL\t");
      displayExprLog(LOGZEN,2,ScanBeAux->ABL);
    }
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit delayed internal signal\n");
  for (ScanBeDly=ptBeFig->BEDLY;ScanBeDly;ScanBeDly=ScanBeDly->NEXT)
  {
    avt_log(LOGZEN,2,"\tDLY %s\n",ScanBeDly->NAME);
    if (ScanBeDly->NODE)
    {
      avt_log(LOGZEN,2,"\t\tBDD\t");
      //displayBdd(ScanBeDly->NODE,1);
      displayExprLog(LOGZEN,2,bddToAblCct(ptBeFig->CIRCUI,ScanBeDly->NODE));
    }
    if (ScanBeDly->ABL)
    {
      avt_log(LOGZEN,2,"\t\tABL\t");
      displayExprLog(LOGZEN,2,ScanBeDly->ABL);
    }
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit bused internal signal\n");
  for (ScanBeBux=ptBeFig->BEBUX;ScanBeBux;ScanBeBux=ScanBeBux->NEXT)
  {
    avt_log(LOGZEN,2,"\tBUX %s\t%c\n",ScanBeBux->NAME,ScanBeBux->TYPE);
    if (ScanBeBux->BINODE)
    {
      avt_log(LOGZEN,2,"\t\tBI_BDD\n");
      display_binode(ptBeFig->CIRCUI,ScanBeBux->BINODE);
    }
    if (ScanBeBux->BIABL)
    {
      avt_log(LOGZEN,2,"\t\tBI_ABL\n");
      display_biabl(ScanBeBux->BIABL);
    }
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* one-bit internal register\n");
  for (ScanBeReg=ptBeFig->BEREG;ScanBeReg;ScanBeReg=ScanBeReg->NEXT)
  {
    avt_log(LOGZEN,2,"\tREG %s\n",ScanBeReg->NAME);
    if (ScanBeReg->BINODE)
    {
      avt_log(LOGZEN,2,"\t\tBI_BDD\n");
      display_binode(ptBeFig->CIRCUI,ScanBeReg->BINODE);
    }
    if (ScanBeReg->BIABL)
    {
      avt_log(LOGZEN,2,"\t\tBI_ABL\n");
      display_biabl(ScanBeReg->BIABL);
    }
  }
  /*--------------------*/
  avt_log(LOGZEN,2,"* assert condition\n");
  for (ScanBeMsg=ptBeFig->BEMSG;ScanBeMsg;ScanBeMsg=ScanBeMsg->NEXT)
  {
    avt_log(LOGZEN,2,"\tMSG %s\t%c\t%s\n",ScanBeMsg->LABEL,ScanBeMsg->LEVEL,
                                        ScanBeMsg->MESSAGE);
    if (ScanBeMsg->NODE)
    {
      avt_log(LOGZEN,2,"\t\tBDD\t");
      //displayBdd(ScanBeMsg->NODE,1);
      displayExprLog(LOGZEN,2,bddToAblCct(ptBeFig->CIRCUI,ScanBeMsg->NODE));
    }
    if (ScanBeMsg->ABL)
    {
      avt_log(LOGZEN,2,"\t\tABL\t");
      displayExprLog(LOGZEN,2,ScanBeMsg->ABL);
    }
  }
  avt_log(LOGZEN,2,")BEFIG %s\n",ptBeFig->NAME);
  if (ptBeFig->CIRCUI) displayCctLog(LOGZEN,2,ptBeFig->CIRCUI,1);
  else avt_log(LOGZEN,2,"\nBdd non charges\n");
}

/*}}}************************************************************************/
/*{{{                      Search  Functions                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
beaux_list *searchBeAux(beh,name)
befig_list  *beh;
char        *name;
{
  beaux_list    *scanBe;
  long           res;
  static  ht    *htable = NULL;

  if (!beh) 
    if(htable)
    {
      delht(htable);
      htable = NULL;
      return NULL;
    }
    else return NULL;
  else if (!htable)
  {
    htable = addht(ZEN_HTSIZE);
    for (scanBe=beh->BEAUX;scanBe;scanBe=scanBe->NEXT)
      addhtitem(htable,scanBe->NAME,(long)scanBe);
  }
  if ((res=gethtitem(htable,name))==EMPTYHT) return NULL;
  else return (beaux_list*)res;
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
berin_list *searchBeRin(beh,name)
befig_list  *beh;
char        *name;
{
  berin_list    *scanBe;
  long           res;
  static  ht    *htable = NULL;

  if (!beh) 
    if(htable)
    {
      delht(htable);
      htable = NULL;
      return NULL;
    }
    else return NULL;
  else if (!htable)
  {
    htable = addht(ZEN_HTSIZE);
    for (scanBe=beh->BERIN;scanBe;scanBe=scanBe->NEXT)
      addhtitem(htable,scanBe->NAME,(long)scanBe);
  }
  if ((res=gethtitem(htable,name))==EMPTYHT) return NULL;
  else return (berin_list*)res;
}


/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
beout_list *searchBeOut(beh,name)
befig_list  *beh;
char        *name;
{
  beout_list    *scanBe;
  long           res;
  static  ht    *htable = NULL;

  if (!beh) 
    if(htable)
    {
      delht(htable);
      htable = NULL;
      return NULL;
    }
    else return NULL;
  else if (!htable)
  {
    htable = addht(ZEN_HTSIZE);
    for (scanBe=beh->BEOUT;scanBe;scanBe=scanBe->NEXT)
      addhtitem(htable,scanBe->NAME,(long)scanBe);
  }
  if ((res=gethtitem(htable,name))==EMPTYHT) return NULL;
  else return (beout_list*)res;
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bereg_list *searchBeReg(beh,name)
befig_list  *beh;
char        *name;
{
  bereg_list    *scanBe;
  long           res;
  static  ht    *htable = NULL;

  if (!beh) 
    if(htable)
    {
      delht(htable);
      htable = NULL;
      return NULL;
    }
    else return NULL;
  else if (!htable)
  {
    htable = addht(ZEN_HTSIZE);
    for (scanBe=beh->BEREG;scanBe;scanBe=scanBe->NEXT)
      addhtitem(htable,scanBe->NAME,(long)scanBe);
  }
  if ((res=gethtitem(htable,name))==EMPTYHT) return NULL;
  else return (bereg_list*)res;
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bebus_list *searchBeBus(beh,name)
befig_list  *beh;
char        *name;
{
  bebus_list    *scanBe;
  long           res;
  static  ht    *htable = NULL;

  if (!beh) 
    if(htable)
    {
      delht(htable);
      htable = NULL;
      return NULL;
    }
    else return NULL;
  else if (!htable)
  {
    htable = addht(ZEN_HTSIZE);
    for (scanBe=beh->BEBUS;scanBe;scanBe=scanBe->NEXT)
      addhtitem(htable,scanBe->NAME,(long)scanBe);
  }
  if ((res=gethtitem(htable,name))==EMPTYHT) return NULL;
  else return (bebus_list*)res;
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bebux_list *searchBeBux(beh,name)
befig_list  *beh;
char        *name;
{
  bebux_list    *scanBe;
  long           res;
  static  ht    *htable = NULL;

  if (!beh) 
    if(htable)
    {
      delht(htable);
      htable = NULL;
      return NULL;
    }
    else return NULL;
  else if (!htable)
  {
    htable = addht(ZEN_HTSIZE);
    for (scanBe=beh->BEBUX;scanBe;scanBe=scanBe->NEXT)
      addhtitem(htable,scanBe->NAME,(long)scanBe);
  }
  if ((res=gethtitem(htable,name))==EMPTYHT) return NULL;
  else return (bebux_list*)res;
}

/*}}}************************************************************************/
/*{{{                    Replace Functions                                  */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                       zen_remplacebebux()                                */
/*                                                                          */
/****************************************************************************/
befig_list *zen_remplacebebux(ptBeFig)
befig_list  *ptBeFig;
{
  bebux_list    *ScanBeBux;
  biabl_list    *ScanBiAbl;
  chain_list    *AblBeAux;
  chain_list    *tmpAbl;
  chain_list    *ablCnd;
  chain_list    *ablVal;
  pNode          bdd = NULL;

  for (ScanBeBux=ptBeFig->BEBUX;ScanBeBux;ScanBeBux=ScanBeBux->NEXT)
  {
    AblBeAux = NULL;

    for (ScanBiAbl=ScanBeBux->BIABL;ScanBiAbl;ScanBiAbl=ScanBiAbl->NEXT)
    {
      ablCnd = copyExpr(ScanBiAbl->CNDABL);
      ablVal = copyExpr(ScanBiAbl->VALABL);
      tmpAbl = createBinExpr(AND,ablVal,ablCnd);
      if (!AblBeAux) AblBeAux = tmpAbl;
      else AblBeAux = createBinExpr(OR,AblBeAux,tmpAbl);
    }
    //bdd = ablToBddCct(ptBeFig->CIRCUI,AblBeAux);
    ptBeFig->BEAUX = beh_addbeaux(ptBeFig->BEAUX,ScanBeBux->NAME,AblBeAux,bdd,0);
  }
  return (ptBeFig);
}

/****************************************************************************/
/*                       zen_remplacebebus()                                */
/*                                                                          */
/****************************************************************************/
befig_list *zen_remplacebebus(ptBeFig)
befig_list  *ptBeFig;
{
  bebus_list    *ScanBeBus;
  biabl_list    *ScanBiAbl;
  chain_list    *AblBeOut;
  chain_list    *tmpAbl;
  chain_list    *ablCnd;
  chain_list    *ablVal;
  pNode          bdd = NULL;

  for (ScanBeBus=ptBeFig->BEBUS;ScanBeBus;ScanBeBus=ScanBeBus->NEXT)
  {
    //fprintf(stdout,"\tBUS %s\t%c\n",
    //(char *)(ScanBeBus->NAME),
    //(char)(ScanBeBus->TYPE));
    AblBeOut=NULL;
    for (ScanBiAbl=ScanBeBus->BIABL;ScanBiAbl;ScanBiAbl=ScanBiAbl->NEXT)
    {
      ablCnd = copyExpr(ScanBiAbl->CNDABL);
      ablVal = copyExpr(ScanBiAbl->VALABL);
      tmpAbl = createBinExpr(AND,ablVal,ablCnd);
      if (!AblBeOut) AblBeOut = tmpAbl;
      else AblBeOut = createBinExpr(OR,AblBeOut,tmpAbl);
    }
    //displayExprLog(LOGZEN,2,"ZEN",AblBeOut);
    //bdd = ablToBddCct(ptBeFig->CIRCUI,AblBeOut);
    ptBeFig->BEOUT = beh_addbeout(ptBeFig->BEOUT,ScanBeBus->NAME,AblBeOut,bdd, 0);
  }
  return (ptBeFig);
}

/*}}}************************************************************************/
/*{{{                    BDD Construction Functions                         */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*{{{                    beh_mbddtot()                                      */
/* make bdds for all primary signals: simple and bussed	                    */
/*		  outputs, simple and bussed internal signals and	                */
/*		  registers						                                    */
/****************************************************************************/
void zen_mbddtot(pt_befig)
befig_list  *pt_befig;		/* pointer on current BEFIG	*/
{
  pCircuit       cct = pt_befig->CIRCUI;
  cct = NULL;
/*
  beaux_list    *pt_beaux;
  bemsg_list    *pt_bemsg;
  beout_list    *pt_beout;
  bebus_list    *pt_bebus;
  bebux_list    *pt_bebux;
  bereg_list    *pt_bereg;
  biabl_list    *pt_biabl;
  binode_list   *pt_binode;
  pNode          pt_cndbdd;
  pNode          pt_valbdd;
  char           buf[256];

  //    make a bdd for each simple internal signal
  for (pt_beaux=pt_befig->BEAUX;pt_beaux;pt_beaux=pt_beaux->NEXT)
  {
      sprintf(buf,"%s\n",pt_beaux->NAME); ZEN_TRACE(0,buf);
    pt_valbdd = zen_ablToBddCct(cct,pt_beaux->ABL);
    // perhaps only for aux, in case of big expr in other type, should use this
    // solution
    if (pt_valbdd==ZEN_EXPLODEDBDD) pt_valbdd = BDD_one;
    if (pt_befig->TYPE & BEH_NODE_QUAD)
      ((struct bequad *)(pt_beaux->NODE))->BDD = pt_valbdd;
    else
      pt_beaux->NODE = pt_valbdd;
  }
	//    make a bdd for each delayed internal signal
  for (pt_beaux=pt_befig->BEDLY;pt_beaux;pt_beaux=pt_beaux->NEXT)
  {
      sprintf(buf,"%s\n",pt_beaux->NAME); ZEN_TRACE(0,buf);
    pt_valbdd = zen_ablToBddCct(cct,pt_beaux->ABL);
    if (pt_befig->TYPE & BEH_NODE_QUAD)
      ((struct bequad *)(pt_beaux->NODE))->BDD = pt_valbdd;
    else
      pt_beaux->NODE = pt_valbdd;
  }
	//    make a bdd for each assertion
  for (pt_bemsg=pt_befig->BEMSG;pt_bemsg;pt_bemsg=pt_bemsg->NEXT)
  {
      sprintf(buf,"bemssg\n"); ZEN_TRACE(0,buf);
    pt_valbdd = zen_ablToBddCct(cct,pt_bemsg->ABL);
    if (pt_befig->TYPE & BEH_NODE_QUAD)
      ((bequad_list *)(pt_bemsg->NODE))->BDD = pt_valbdd;
    else
      pt_bemsg->NODE = pt_valbdd;
  }
	//    make a bdd for each simple output port
  for (pt_beout=pt_befig->BEOUT;pt_beout;pt_beout=pt_beout->NEXT)
  {
      sprintf(buf,"%s\n",pt_beout->NAME); ZEN_TRACE(0,buf);
    pt_valbdd = zen_ablToBddCct(cct, pt_beout->ABL);
    if (pt_befig->TYPE & BEH_NODE_QUAD)
      ((bequad_list *)(pt_beout->NODE))->BDD = pt_valbdd;
    else
      pt_beout->NODE = pt_valbdd;
  }
	//    make a bdd for each bussed output port
  for (pt_bebus=pt_befig->BEBUS;pt_bebus;pt_bebus=pt_bebus->NEXT)
  {
      sprintf(buf,"%s\n",pt_bebus->NAME); ZEN_TRACE(0,buf);
    for (pt_biabl=pt_bebus->BIABL,pt_binode=pt_bebus->BINODE; pt_biabl;
         pt_biabl=pt_biabl->NEXT ,pt_binode=pt_binode->NEXT)
    {
      pt_cndbdd = zen_ablToBddCct(cct,pt_biabl->CNDABL);
      pt_valbdd = zen_ablToBddCct(cct,pt_biabl->VALABL);

      if (pt_befig->TYPE & BEH_NODE_QUAD)
      {
        ((bequad_list *)(pt_binode->CNDNODE))->BDD = pt_cndbdd;
        ((bequad_list *)(pt_binode->VALNODE))->BDD = pt_valbdd;
      }
      else
      {
        pt_binode->CNDNODE = pt_cndbdd;
        pt_binode->VALNODE = pt_valbdd;
      }
    }
  }
	//    make a bdd for each bussed internal signal
  for (pt_bebux=pt_befig->BEBUX;pt_bebux;pt_bebux=pt_bebux->NEXT)
  {
      sprintf(buf,"%s\n",pt_bebux->NAME); ZEN_TRACE(0,buf);
    for (pt_biabl=pt_bebux->BIABL,pt_binode=pt_bebux->BINODE; pt_biabl;
         pt_biabl=pt_biabl->NEXT ,pt_binode=pt_binode->NEXT)
    {
      pt_cndbdd = zen_ablToBddCct(cct,pt_biabl->CNDABL);
      pt_valbdd = zen_ablToBddCct(cct,pt_biabl->VALABL);

      if (pt_befig->TYPE & BEH_NODE_QUAD)
      {
        ((bequad_list *)(pt_binode->CNDNODE))->BDD = pt_cndbdd;
        ((bequad_list *)(pt_binode->VALNODE))->BDD = pt_valbdd;
      }
      else
      {
        pt_binode->CNDNODE = pt_cndbdd;
        pt_binode->VALNODE = pt_valbdd;
      }
    }
  }
	//    make a bdd for each internal register
  for (pt_bereg=pt_befig->BEREG;pt_bereg;pt_bereg=pt_bereg->NEXT)
  {
      sprintf(buf,"%s\n",pt_bereg->NAME); ZEN_TRACE(0,buf);
    for (pt_biabl=pt_bereg->BIABL,pt_binode=pt_bereg->BINODE; pt_biabl;
         pt_biabl=pt_biabl->NEXT ,pt_binode=pt_binode->NEXT)
    {
      pt_cndbdd = zen_ablToBddCct(cct,pt_biabl->CNDABL);
      pt_valbdd = zen_ablToBddCct(cct,pt_biabl->VALABL);
      if (pt_befig->TYPE & BEH_NODE_QUAD)
      {
        ((bequad_list *)(pt_binode->CNDNODE))->BDD = pt_cndbdd;
        ((bequad_list *)(pt_binode->VALNODE))->BDD = pt_valbdd;
      }
      else
      {
        pt_binode->CNDNODE = pt_cndbdd;
        pt_binode->VALNODE = pt_valbdd;
      }
    }
  }*/
}

/*}}}************************************************************************/
/*{{{                    zen_makbehbdd()                                    */
/* translate all abls into bdds				                                */
/****************************************************************************/
void zen_makbehbdd(pt_befig)
befig_list  *pt_befig;
{
  int       err_flg = 0;

//  if (!(pt_befig->TYPE & BEH_NODE_BDD));
  {
    zen_indexbdd(pt_befig);
//   zen_mbddtot(pt_befig);
    pt_befig->TYPE |= BEH_NODE_BDD;

    if (err_flg != 0)
      pt_befig->ERRFLG = 1;
  }
}

/*}}}************************************************************************/
/*{{{                    zen_cutAbl()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *zen_cutAbl(abl)
chain_list  *abl;
{
  chain_list    *expn = abl;
  chain_list    *expd = abl;
  chain_list    *expr = abl;

  if (ATOM(abl)) return abl;
  while (OPER(abl)==OPER((expd=CADR(expn))))
  {
    freechain(CAR(expd)); // on libere l'operateur
    CDR(expn)=delchain(CDR(expn),CDR(expn));
    expn=CDR(expn);
    CDR(expn)=delchain(expd,expd);
  }
  while ((expr = CDR (expr))) zen_cutAbl( CAR(expr));
  return abl;
}


/*}}}************************************************************************/
/*{{{                    zen_ablToBddCct()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
pNode zen_ablToBddCct(cct,expr/*,iCct*/)
pCircuit     cct;
chain_list  *expr;
//pCircuit     iCct;
{
  static int     i = 0;
  pNode          res;
  chain_list    *support = NULL;
  chain_list    *chainx;
  int            bigAbl;
//  chain_list    *bddList = NULL;
  char           buf[256];

  bigAbl = (profExpr(expr)>20);
  expr = zen_cutAbl(expr);

  if (bigAbl)
  {
//    expr = zen_cutAbl(expr);
    setBddCeiling(numberNodeAllBdd()+5000);
    i++;
//    support = reverse(supportChain_listExpr(expr));
  }
//  else
    support = reverse(supportChain_listExpr(expr));

  for (chainx=support;chainx;chainx=chainx->NEXT)
    addInputCct(cct,chainx->DATA);
  freechain(support);

//  if (bigAbl)
//  {
//    for (chainx=expr->NEXT;chainx;chainx=chainx->NEXT)
//      bddList = addListBdd(bddList,ablToBddCct(cct,chainx->DATA));
//    res = applyBdd(OPER(expr),reverse(bddList));
//    freechain(bddList);
//  }
//  else
    res = ablToBddCct(cct,expr);


  if (bddSystemAbandoned())
  {
    sprintf(buf,"zen_ablToBddCct explosion (%d%s time)",i,
                (i==1)?"st":((i==2)?"nd":((i==3)?"rd":"th")));
    ZEN_TRACE(0,buf);
  //res = ablToBddCct(iCct,expr);
  //printf("ok repare\n");
  //EXIT (0);
    unsetBddCeiling();
    return ZEN_EXPLODEDBDD;
  }
  unsetBddCeiling();
//zen_displayExprLog(LOGZEN,2,"ZEN",bddToAblCct(cct,res));
  return res;
}

/*}}}************************************************************************/
/*{{{                    zen_initCct()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_initCct(befig)
befig_list  *befig;
{
  int            nbin  = 0;
  int            nbout = 0;
  berin_list    *berin;
  beout_list    *beout;
  bebus_list    *bebus;
  beaux_list    *beaux;
  bebux_list    *bebux;
  bereg_list    *bereg;
  bemsg_list    *bemsg;
  biabl_list    *bebia;

  for (berin=befig->BERIN;berin;nbin++,berin=berin->NEXT);
  for (beout=befig->BEOUT;beout;nbout++,beout=beout->NEXT);
  for (bebus=befig->BEBUS;bebus;bebus=bebus->NEXT)
    for (bebia=bebus->BIABL;bebia;nbout+=2,bebia=bebia->NEXT);
  for (beaux=befig->BEAUX;beaux;nbout++,beaux=beaux->NEXT);
  for (beaux=befig->BEDLY;beaux;nbout++,beaux=beaux->NEXT);
  for (bebux=befig->BEBUX;bebux;bebux=bebux->NEXT)
    for (bebia=bebux->BIABL;bebia;nbout+=2,bebia=bebia->NEXT);
  for (bereg=befig->BEREG;bereg;bereg=bereg->NEXT)
    for (bebia=bereg->BIABL;bebia;nbout+=2,bebia=bebia->NEXT);
  for (bemsg=befig->BEMSG;bemsg;nbout++,bemsg=bemsg->NEXT);

//  befig->CIRCUI = initializeCct (befig->NAME,nbin*2,nbout);
}

/*}}}************************************************************************/
/*{{{                    zen_indexbdd()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_indexbdd(pt_fig)
befig_list  *pt_fig;
{
  if ((pt_fig)&&(!pt_fig->CIRCUI)) zen_initCct(pt_fig);
}
