/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_timing.c                                                */
/*                                                                          */
/*    (c) copyright 1991 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : ../../....     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#ifndef WITHOUT_TAS
#include "yag_headers.h"

static void yagTimeBefig(befig_list *ptbefig);
static void yagDetectSensitive(cnsfig_list *ptcnsfig);
static unsigned int yagGetTotalDelay(chain_list *ptconelist);
static unsigned int yagGetDelay(cone_list *ptcone, chain_list *cndexpr, chain_list *valexpr);
static int yagConstantAbl(chain_list *ptexpr);
static int yagTimeSensitiveBebux(bebux_list *ptbebux, cone_list *ptcone);
static int yagTimeSensitiveBereg(bereg_list *ptbereg, cone_list *ptcone);
static int yagTimeSensitiveBebus(bebus_list *ptbebus, cone_list *ptcone);

/****************************************************************************
*                         function yagPrepTiming();                         *
****************************************************************************/
void
yagPrepTiming(lofig_list *ptlofig)
{
    inffig_list *ifl;
    
    tas_setenv();
    TAS_CONTEXT->TAS_CALCRCX = 'Y';
//    TAS_CONTEXT->TAS_MERGERCN = 'Y';
    TAS_CONTEXT->FRONT_CON = TAS_NOFRONT;
    if (YAG_CONTEXT->YAG_TAS_TIMING == YAG_MIN_TIMING) {
        TAS_CONTEXT->TAS_FIND_MIN = 'Y';
    }
    TAS_CONTEXT->TAS_SILENTMODE = 'Y';
    if ((ifl=getloadedinffig(ptlofig->NAME))!=NULL)
      tas_update_mcctemp (ifl);
    tas_TechnoParameters();
}

static chain_list *yag_buildminterms(cone_list *ptcone, chain_list *cndabl, chain_list *valabl)
{
  chain_list *support, *list=NULL, *tmpexpr, *samedelay, *expr, *cl, *ch;
  pCircuit cct;
  pNode exprbdd;
  int nb, count;
  long timing, l;
  ht *ht;
  
  support=supportChain_listExpr(cndabl);
  nb=countchain(support);
  freechain(support);
  if (nb>1)
  {
    list=ablCreateMintermList(cndabl);
/*    exprbdd=ablToBddCct(cct, cndabl);
    displayExpr(bddToAblCct(cct, exprbdd));*/
    ht=addht(20);
    while (list!=NULL)
    {
      expr=(chain_list *)list->DATA;
      timing=yagGetDelay(ptcone, expr, valabl);
      if ((l=gethtitem(ht, (void *)timing))==EMPTYHT) support=NULL;
      else support=(chain_list *)l;
      support=addchain(support, expr);
      addhtitem(ht, (void *)timing, (long)support);           
      list=delchain(list, list);
    }
    samedelay=GetAllHTElems(ht);
    delht(ht);

    if (countchain(samedelay)<=1)
    {
       while (samedelay)
        {
          for (list=(chain_list *)samedelay->DATA; list!=NULL; list=list->NEXT)
           freeExpr((chain_list *)list->DATA);
          freechain((chain_list *)samedelay->DATA);
          samedelay=delchain(samedelay, samedelay);
        }
       return NULL;
    }
    
    support=NULL;
    while (samedelay)
    {
      list=(chain_list *)samedelay->DATA;
      if (list->NEXT==NULL)
        support=addchain(support, list->DATA);
      else
       {
         chain_list *sublist=NULL;
         int cnt=countchain(list);
         if (cnt<=8) {cnt=100;}
         else cnt=1;
         cl=list;
         while (cl!=NULL)
         {
           count =0;
           ch=NULL;
           cct=initializeCct("temp",nb, 10);
           while (cl!=NULL)
           {
             if (count>=cnt && cl->NEXT!=NULL) break;
             tmpexpr=(chain_list *)cl->DATA;
             ch=addchain(ch, ablToBddCct(cct, tmpexpr));
             freeExpr(tmpexpr);
             count++;
             cl=cl->NEXT;
           }
           if (countchain(ch)>1)
             exprbdd=applyBdd (OR, ch);
           else
             exprbdd=(pNode)ch->DATA;
           sublist=addchain(sublist, bddToAblCct(cct, exprbdd));
           freechain(ch);
//           support=addchain(support, bddToAblCct(cct, exprbdd));
           destroyCct(cct);
         }
         if (sublist!=NULL && sublist->NEXT!=NULL)
         {
           tmpexpr=createExpr (OR);
           while (sublist!=NULL)
            {
               addQExpr(tmpexpr, (chain_list *)sublist->DATA);
               sublist=delchain(sublist, sublist);
            }
           support=addchain(support, tmpexpr);
         }
         else
         {
           support=addchain(support, sublist->DATA);
           freechain(sublist);
         }
       }
      freechain(list);
      samedelay=delchain(samedelay, samedelay);
    }
//    destroyCct(cct);
    list=support;
  }
  return list;
}

static int
yagSplitIntoMinterms(biabl_list **biabl, cone_list *ptcone)
{
    biabl_list *ptbiabl;
    biabl_list *ptnewbiabl = NULL, *tmpb, *knext;
    chain_list *ptchain;

    if (!ptcone) return FALSE;
    
    for (ptbiabl = *biabl; ptbiabl; ptbiabl = ptbiabl->NEXT) 
    {
      ptchain=yag_buildminterms(ptcone, ptbiabl->CNDABL, ptbiabl->VALABL);
      if (ptchain==NULL || countchain(ptchain)>V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE)
      {
        knext=ptbiabl->NEXT; ptbiabl->NEXT=NULL;
        tmpb=beh_dupbiabl(ptbiabl);
        ptbiabl->NEXT=knext;
        tmpb->NEXT=ptnewbiabl;
        ptnewbiabl = tmpb;
        while (ptchain!=NULL)
        {
          freeExpr((chain_list *)ptchain->DATA);
          ptchain=delchain(ptchain, ptchain);
        }
      }
      else
      {
/*        if (ptchain!=NULL && ptchain->NEXT!=NULL)
            printf("split %s\n",ptcone->NAME);*/
        while (ptchain!=NULL)
        {
          knext=ptbiabl->NEXT; ptbiabl->NEXT=NULL;
          tmpb=beh_dupbiabl(ptbiabl);
          ptbiabl->NEXT=knext;
          tmpb->NEXT=ptnewbiabl;
          ptnewbiabl = tmpb;
          freeExpr(tmpb->CNDABL);
          tmpb->CNDABL=(chain_list *)ptchain->DATA;
          ptchain=delchain(ptchain, ptchain);
        }
      }
    }
    ptnewbiabl = (biabl_list *)reverse((chain_list *)ptnewbiabl);
    beh_frebiabl(*biabl);
    *biabl = ptnewbiabl;
    return TRUE;
}


static void
yagDetectSensitive(cnsfig_list *ptcnsfig)
{
    cone_list *ptcone;
    edge_list *ptinput;
    unsigned int upmax, upmin;
    unsigned int dnmax, dnmin;
    unsigned int delay;
    chain_list *name_expr;
    chain_list *one_expr, *zero_expr;
    char *name;

    if (YAG_CONTEXT->YAG_SENSITIVE_RATIO < 1) return;
    one_expr = createAtom("'1'");
    zero_expr = createAtom("'0'");

    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if (YAG_CONTEXT->YAG_SENSITIVE_MAX > 0 && yagCountEdges(ptcone->INCONE) > YAG_CONTEXT->YAG_SENSITIVE_MAX) continue; 
        upmax = 0;
        upmin = 0;
        dnmax = 0;
        dnmin = 0;
        for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
            if ((ptinput->TYPE & CNS_EXT) != 0) {
                name = ptinput->UEDGE.LOCON->NAME;
            }
            else name = ptinput->UEDGE.CONE->NAME;
            name_expr = createAtom(mbk_devect(name, "[", "]"));
            delay = yagGetDelay(ptcone, name_expr, createAtom("'1'"));
            if (delay != 0) {
                if (upmax == 0 || delay > upmax) upmax = delay;
                if (upmin == 0 || delay < upmin) upmin = delay;
            }
            delay = yagGetDelay(ptcone, name_expr, createAtom("'0'"));
            if (delay != 0) {
                if (dnmax == 0 || delay > dnmax) dnmax = delay;
                if (dnmin == 0 || delay < dnmin) dnmin = delay;
            }
            freeExpr(name_expr);
        }
        if (upmax > upmin * YAG_CONTEXT->YAG_SENSITIVE_RATIO
        || dnmax > dnmin * YAG_CONTEXT->YAG_SENSITIVE_RATIO) {
            ptcone->TECTYPE |= YAG_SENSITIVE;
        }
    }
    freeExpr(one_expr);    
    freeExpr(zero_expr);    
}

static unsigned int
yagGetRCDelay(cone_list *ptcone, char *input, int dir)
{
    edge_list  *ptincone;
    ptype_list *ptuser;
    delay_list *delay;
    char       *inputname;
    long        delaymax = 0;
    int         useRising = TRUE;
    int         useFalling = TRUE;
    int         n = 0;

    if (ptcone == NULL) return 0;

    if (dir==1) useFalling = FALSE;
    else if (dir==0) useRising = FALSE;

    for (ptincone = ptcone->INCONE; ptincone; ptincone = ptincone->NEXT) {
        if ((ptincone->TYPE & CNS_EXT) == 0) inputname = ptincone->UEDGE.CONE->NAME;
        else inputname = ptincone->UEDGE.LOCON->NAME;
        if (input!=mbk_devect(inputname, "[", "]")) continue;
        ptuser = getptype(ptincone->USER, TAS_DELAY_MAX);
        if (ptuser != NULL) {
            delay = (delay_list *)ptuser->DATA;
            if (useRising && delay->RCHH != TAS_NOTIME) {
                delaymax += delay->RCHH;
                n++;
            }
            if (useFalling && delay->RCLL != TAS_NOTIME) {
                delaymax += delay->RCLL;
                n++;
            }
        }
        if (n!=0) delaymax/=n;
        delaymax=mbk_long_round(delaymax/TTV_UNIT);
        if (delaymax > 0) return ((unsigned int)delaymax);
        else return 0;
    }
    return 0;
}

static ht *RC_WRAP_HT;
static cone_list *RC_WRAP_CONE;

static char *rc_wrap_func(char *input)
{
  char buf[2048];
  int rcvalup, rcvaldown;
  char *inputrc, *c;
  int minrc, rcstep;

  minrc=mbk_long_round(V_FLOAT_TAB[__YAG_MIN_RC].VALUE*1e12);
  if (minrc<=0) minrc=1;
  rcstep=mbk_long_round(V_FLOAT_TAB[__YAG_RC_STEP].VALUE*1e12);
  if (rcstep<1) rcstep=1;
  
  rcvaldown=yagGetRCDelay(RC_WRAP_CONE, input, 0);
  rcvalup=yagGetRCDelay(RC_WRAP_CONE, input, 1);
  if (rcvalup>minrc || rcvaldown>minrc)
    {
       rcvalup=mbk_long_round((float)rcvalup/rcstep)*rcstep;
       rcvaldown=mbk_long_round((float)rcvaldown/rcstep)*rcstep;
       if (rcvalup==0) rcvalup=minrc;
       if (rcvaldown==0) rcvaldown=minrc;
       if (rcvalup==rcvaldown) 
         sprintf(buf, "%s|%dps", input, rcvalup);
       else
         sprintf(buf, "%s|%dps|%dps", input, rcvalup,rcvaldown);
       if ((c=strchr(buf, ' '))!=NULL) *c='|';
       inputrc=namealloc(buf);
       if (gethtitem(RC_WRAP_HT, inputrc)==EMPTYHT)
         addhtitem(RC_WRAP_HT, inputrc, (long)addptype(addptype(NULL, rcvaldown, input), rcvalup, input));
       return inputrc;
    }
  return input;
}

static void yagCheckRCandUpdateNames(cone_list *cn, ht *rcht, chain_list *cndexpr, chain_list *valexpr)
{
  RC_WRAP_HT=rcht;
  RC_WRAP_CONE=cn;

  if (cndexpr!=NULL) beh_wrapAtomExpr(cndexpr, rc_wrap_func);
  if (valexpr!=NULL) beh_wrapAtomExpr(valexpr, rc_wrap_func);
}

static void yadAddRC(ht *rcht, befig_list *ptbefig)
{
  chain_list *allrc;
  ptype_list *pt;
  char *inputrc;

  allrc=GetAllHTKeys(rcht);
  while (allrc!=NULL)
  {
    inputrc=(char *)allrc->DATA;
    pt=(ptype_list *)gethtitem(rcht, inputrc);
    ptbefig->BEAUX = beh_addbeaux (ptbefig->BEAUX, inputrc, createAtom((char *)pt->DATA), NULL, 0);
    ptbefig->BEAUX->TIMER=pt->TYPE;
    ptbefig->BEAUX->TIMEF=pt->NEXT->TYPE;
    freeptype(pt);
    allrc=delchain(allrc, allrc);
  }
}

/****************************************************************************
*                         function yagAddTiming();                          *
****************************************************************************/
void
yagAddTiming(cnsfig_list *ptcnsfig, lofig_list *ptlofig)
{
    time_t          start = 0;
    time_t          end = 0;
    struct rusage   END;
    struct rusage   START;
    locon_list     *ptlocon;
    inffig_list    *ifl;

    ifl = getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);

    cns_addmultivoltage(ifl, ptcnsfig);

    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_TAS_TECHNO, NULL);
    if (MCC_HEADTECHNO == NULL && ELP_MODEL_LIST == NULL) {
        if (!YAG_CONTEXT->YAG_SILENT_MODE) {
            yagChrono(&END,&end);
            if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
            yagMessage(MES_NO_TECHNO_FILE, NULL);
        }
        YAG_CONTEXT->YAG_TAS_TIMING = YAG_NO_TIMING;
    }
    else {
        elpLofigAddCapas (ptlofig,( (mcc_use_multicorner() == 0 ) ? elpTYPICAL : elpWORST ));
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }

    if (YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        yagChrono(&START,&start);
        yagMessage(MES_TAS_TIMING, NULL);
        tas_detectinout(ptcnsfig);
        tas_initcnsfigalloc(ptcnsfig);
        TAS_CONTEXT->TAS_LOFIG = ptlofig;

        tas_loconorient(ptlofig, NULL);
        tas_timing(ptcnsfig, ptlofig, NULL);
        for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            ptlocon->NAME = mbk_devect(ptlocon->NAME, "[", "]");
        }
        trcflushdelaycache();

        yagDetectSensitive(ptcnsfig);
        if (ptcnsfig->BEFIG != NULL) yagTimeBefig(ptcnsfig->BEFIG);
        tas_freecnsfigalloc(ptcnsfig);
        yagChrono(&END,&end);
        yagPrintTime(&START,&END,start,end);
    }
}

static int
already_timed(biabl_list *ptbiabl)
{
    for (;ptbiabl; ptbiabl = ptbiabl->NEXT) {
        if (ptbiabl->TIME != 0) return TRUE;
    }
    return FALSE;
}

static void yag_check_single_delay(biabl_list *BIABL)
{
  unsigned int uptime, downtime;
  biabl_list  *ptbiabl;

  if (V_FLOAT_TAB[__YAG_SINGLE_DELAY_RATIO].VALUE>1.0)
  {
    uptime = 0;
    downtime = INT_MAX;
    for (ptbiabl = BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
      if (downtime>ptbiabl->TIME) downtime=ptbiabl->TIME;
      if (uptime<ptbiabl->TIME) uptime=ptbiabl->TIME;
    }
    if (downtime>0 && (float)(uptime-downtime)/(float)downtime<V_FLOAT_TAB[__YAG_SINGLE_DELAY_RATIO].VALUE) {
       for (ptbiabl = BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
          ptbiabl->TIME=(int)(0.5+(uptime+downtime)/2.0);
       }
    }
  }
}

/****************************************************************************
*                         function yagTimeBefig();                          *
****************************************************************************/
static void
yagTimeBefig(befig_list *ptbefig)
{
    beaux_list  *ptbeaux;
    beaux_list  *ptprevbeaux = NULL;
    beaux_list  *ptnextbeaux = NULL;
    beout_list  *ptbeout;
    beout_list  *ptprevbeout = NULL;
    beout_list  *ptnextbeout = NULL;
    bebux_list  *ptbebux;
    bebux_list  *ptprevbebux = NULL;
    bebux_list  *ptnextbebux = NULL;
    bebus_list  *ptbebus;
    bebus_list  *ptprevbebus = NULL;
    bebus_list  *ptnextbebus = NULL;
    bereg_list  *ptbereg;
    bequad_list *ptquad;
    biabl_list  *ptbiabl;
    cone_list   *ptcone;
    ptype_list  *ptuser;
    unsigned int conedelay;
    unsigned int uptime, downtime;
    int          processed, sensitive, split;
    chain_list  *oneexpr, *zeroexpr;
    ht *rcht;

    oneexpr = createAtom("'1'");
    zeroexpr = createAtom("'0'");

    initializeBdd(0);
    rcht=addht(10000);

    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        if (already_timed(ptbebux->BIABL)) continue;
        conedelay = 0;
        processed = FALSE;
        if (ptbebux->BINODE != NULL) ptquad = (bequad_list *)ptbebux->BINODE->VALNODE;
        else ptquad = NULL;
        if (ptquad != NULL) {
            ptuser = getptype(ptquad->USER, YAG_CONE_PTYPE);
            if (ptuser != NULL) {
                ptcone = (cone_list *)ptuser->DATA;
                if (ptcone != NULL) {
                    if ((ptcone->TECTYPE & YAG_SENSITIVE) == YAG_SENSITIVE && strchr(ptbebux->NAME, ' ') == NULL) {
                        processed = yagTimeSensitiveBebux(ptbebux, ptcone);
                        if (processed) {
                            ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, ptbebux->NAME, ptbebux->BIABL, NULL,0);
                            ptbebux->BIABL = NULL;
                            ptbebux->NAME = NULL;
                        }
                    }
                    else if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbebux->BIABL, ptcone);
                }
                if (!processed) {
                    for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                        ptbiabl->TIME = yagGetDelay(ptcone, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                }
                for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                  yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
            }
            else {
                ptuser = getptype(ptquad->USER, YAG_CONELIST_PTYPE);
                if (ptuser != NULL) {
                    conedelay = yagGetTotalDelay((chain_list *)ptuser->DATA);
                }
                if (conedelay != 0) {
                    for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                        if (ptbiabl->TIME != 0) continue;
                        ptbiabl->TIME = conedelay;
                    }
                }
            }
            if (!processed) yag_check_single_delay(ptbebux->BIABL);
        }
    }
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        if (already_timed(ptbebus->BIABL)) continue;
        processed = FALSE;
        if (ptbebus->BINODE != NULL) ptquad = (bequad_list *)ptbebus->BINODE->VALNODE;
        else ptquad = NULL;
        if (ptquad != NULL) {
            ptuser = getptype(ptquad->USER, YAG_CONE_PTYPE);
            if (ptuser != NULL) {
                ptcone = (cone_list *)ptuser->DATA;
                if (ptcone != NULL) {
                    if ((ptcone->TECTYPE & YAG_SENSITIVE) == YAG_SENSITIVE && strchr(ptbebus->NAME, ' ') == NULL) {
                        processed = yagTimeSensitiveBebus(ptbebus, ptcone);
                        if (processed) {
                            ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, ptbebus->NAME, ptbebus->BIABL, NULL,0);
                            ptbebus->BIABL = NULL;
                            ptbebus->NAME = NULL;
                        }
                    }
                    else if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbebus->BIABL, ptcone);
                }
                if (!processed) {
                    for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                        ptbiabl->TIME = yagGetDelay(ptcone, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                }
                for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                  yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
            }
            else {
                ptuser = getptype(ptquad->USER, YAG_CONELIST_PTYPE);
                if (ptuser != NULL) {
                    conedelay = yagGetTotalDelay((chain_list *)ptuser->DATA);
                }
                if (conedelay != 0) {
                    for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                        if (ptbiabl->TIME != 0) continue;
                        ptbiabl->TIME = conedelay;
                    }
                }
            }
            if (!processed) yag_check_single_delay(ptbebus->BIABL);
        }
    }
    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        if (already_timed(ptbereg->BIABL)) continue;
        processed = FALSE;
        if (ptbereg->BINODE != NULL) ptquad = (bequad_list *)ptbereg->BINODE->VALNODE;
        else ptquad = NULL;
        if (ptquad != NULL) {
            ptuser = getptype(ptquad->USER, YAG_CONE_PTYPE);
            if (ptuser != NULL) {
                ptcone = (cone_list *)ptuser->DATA;
                if (ptcone != NULL) {
                    if ((ptcone->TECTYPE & YAG_SENSITIVE) == YAG_SENSITIVE) {
                        processed = yagTimeSensitiveBereg(ptbereg, ptcone);
                    }
                    else if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbereg->BIABL, ptcone);
                }
                if (!processed) {
                    for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                        ptbiabl->TIME = yagGetDelay(ptcone, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                }
                for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                   yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
            }
            else {
                ptuser = getptype(ptquad->USER, YAG_CONELIST_PTYPE);
                if (ptuser != NULL) {
                    conedelay = yagGetTotalDelay((chain_list *)ptuser->DATA);
                }
                if (conedelay != 0) {
                    for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                        if (ptbiabl->TIME != 0) continue;
                        ptbiabl->TIME = conedelay;
                    }
                }
            }
            if (!processed) yag_check_single_delay(ptbereg->BIABL);
        }
    }
    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        if (ptbeaux->TIME != 0) continue;
        processed = FALSE;
        if (ptbeaux->TIME != 0) continue;
        ptquad = (bequad_list *)ptbeaux->NODE;
        if (ptquad != NULL) {
            ptuser = getptype(ptquad->USER, YAG_CONE_PTYPE);
            if (ptuser != NULL) {
                ptcone = (cone_list *)ptuser->DATA;
                uptime = yagGetDelay(ptcone, NULL, oneexpr);
                downtime = yagGetDelay(ptcone, NULL, zeroexpr);
                if (ptcone != NULL) sensitive = ((ptcone->TECTYPE & YAG_SENSITIVE) == YAG_SENSITIVE);
                else sensitive = FALSE;
                if (ptcone != NULL) split = ((ptcone->TECTYPE & YAG_SPLITTIMING) == YAG_SPLITTIMING);
                else split = FALSE;
                if (((YAG_CONTEXT->YAG_SPLITTIMING_RATIO >= 1.0 && (uptime > downtime * YAG_CONTEXT->YAG_SPLITTIMING_RATIO || downtime > uptime * YAG_CONTEXT->YAG_SPLITTIMING_RATIO)) || sensitive || split)
                && strchr(ptbeaux->NAME, ' ') == NULL) {
                    ptbiabl = beh_addbiabl(NULL, "label", copyExpr(oneexpr), createAtom(namealloc("'u'")));
                    ptbiabl->FLAG |= BEH_CND_PRECEDE;
                    ptbiabl = beh_addbiabl(ptbiabl, "label", notExpr(copyExpr(ptbeaux->ABL)), copyExpr(zeroexpr));
                    ptbiabl->FLAG |= BEH_CND_PRECEDE;
                    ptbiabl->TIME = downtime;
                    ptbiabl = beh_addbiabl(ptbiabl, "label", copyExpr(ptbeaux->ABL), copyExpr(oneexpr));
                    ptbiabl->TIME = uptime;
                    if (sensitive) {
                        ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, ptbeaux->NAME, ptbiabl, NULL,0);
                        processed = yagTimeSensitiveBereg(ptbefig->BEREG, ptcone);
                        if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbefig->BEREG->BIABL, ptcone);
                        for (ptbiabl = ptbefig->BEREG->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                          yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                    else {
                        ptbefig->BEBUX = beh_addbebux(ptbefig->BEBUX, ptbeaux->NAME, ptbiabl, NULL, 'M',0);
                        yagWarning(WAR_SPLIT_TIMING, NULL, ptbeaux->NAME, NULL, 0);
                        if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbefig->BEBUX->BIABL, ptcone);
                        for (ptbiabl = ptbefig->BEBUX->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                          yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                    ptbeaux->NAME = NULL;
                }
                else {
                    ptbeaux->TIME = yagGetDelay((cone_list *)ptuser->DATA, NULL, NULL);
                    ptbeaux->TIMER = uptime;
                    ptbeaux->TIMEF = downtime;
                    yagCheckRCandUpdateNames(ptcone, rcht, NULL, ptbeaux->ABL);
                }
            }
            else {
                ptuser = getptype(ptquad->USER, YAG_CONELIST_PTYPE);
                if (ptuser != NULL) {
                    ptbeaux->TIME = yagGetTotalDelay((chain_list *)ptuser->DATA);
                }
            }
        }
    }
    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        if (ptbeout->TIME != 0) continue;
        processed = FALSE;
        if (ptbeout->TIME != 0) continue;
        ptquad = (bequad_list *)ptbeout->NODE;
        if (ptquad != NULL) {
            ptuser = getptype(ptquad->USER, YAG_CONE_PTYPE);
            if (ptuser != NULL) {
                ptcone = (cone_list *)ptuser->DATA;
                uptime = yagGetDelay(ptcone, NULL, oneexpr);
                downtime = yagGetDelay(ptcone, NULL, zeroexpr);
                if (ptcone != NULL) sensitive = ((ptcone->TECTYPE & YAG_SENSITIVE) == YAG_SENSITIVE);
                else sensitive = FALSE;
                if (ptcone != NULL) split = ((ptcone->TECTYPE & YAG_SPLITTIMING) == YAG_SPLITTIMING);
                else split = FALSE;
                if (((YAG_CONTEXT->YAG_SPLITTIMING_RATIO >= 1.0 && (uptime > downtime * YAG_CONTEXT->YAG_SPLITTIMING_RATIO || downtime > uptime * YAG_CONTEXT->YAG_SPLITTIMING_RATIO)) || sensitive || split)
                && strchr(ptbeout->NAME, ' ') == NULL) {
                    ptbiabl = beh_addbiabl(NULL, "label", copyExpr(oneexpr), createAtom(namealloc("'u'")));
                    ptbiabl->FLAG |= BEH_CND_PRECEDE;
                    ptbiabl = beh_addbiabl(ptbiabl, "label", notExpr(copyExpr(ptbeout->ABL)), copyExpr(zeroexpr));
                    ptbiabl->FLAG |= BEH_CND_PRECEDE;
                    ptbiabl->TIME = downtime;
                    ptbiabl = beh_addbiabl(ptbiabl, "label", copyExpr(ptbeout->ABL), copyExpr(oneexpr));
                    ptbiabl->TIME = uptime;
                    if (sensitive) {
                        ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, ptbeout->NAME, ptbiabl, NULL,0);
                        processed = yagTimeSensitiveBereg(ptbefig->BEREG, ptcone);
                        if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbefig->BEREG->BIABL, ptcone);
                        for (ptbiabl = ptbefig->BEREG->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                          yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                    else {
                        ptbefig->BEBUS = beh_addbebus(ptbefig->BEBUS, ptbeout->NAME, ptbiabl, NULL, 'M',0);
                        yagWarning(WAR_SPLIT_TIMING, NULL, ptbeout->NAME, NULL, 0);
                        if (V_INT_TAB[__YAG_MAX_SPLIT_CMD_TIMING].VALUE>0)
                            yagSplitIntoMinterms(&ptbefig->BEBUS->BIABL, ptcone);
                        for (ptbiabl = ptbefig->BEBUS->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
                          yagCheckRCandUpdateNames(ptcone, rcht, ptbiabl->CNDABL, ptbiabl->VALABL);
                    }
                    ptbeout->NAME = NULL;
                }
                else {
                    ptbeout->TIME = yagGetDelay((cone_list *)ptuser->DATA, NULL, NULL);
                    ptbeout->TIMER = uptime;
                    ptbeout->TIMEF = downtime;
                    yagCheckRCandUpdateNames(ptcone, rcht, NULL, ptbeout->ABL);
                }
            }
            else {
                ptuser = getptype(ptquad->USER, YAG_CONELIST_PTYPE);
                if (ptuser != NULL) {
                    ptbeout->TIME = yagGetTotalDelay((chain_list *)ptuser->DATA);
                }
            }
        }
    }
    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptnextbeaux) {
        ptnextbeaux = ptbeaux->NEXT;
        if (ptbeaux->NAME == NULL) {
            freeExpr(ptbeaux->ABL);
            mbkfree(ptbeaux);
            if (ptprevbeaux == NULL) {
                ptbefig->BEAUX = ptnextbeaux;
            }
            else ptprevbeaux->NEXT = ptnextbeaux;
        }
        else ptprevbeaux = ptbeaux;
    }
    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptnextbeout) {
        ptnextbeout = ptbeout->NEXT;
        if (ptbeout->NAME == NULL) {
            freeExpr(ptbeout->ABL);
            mbkfree(ptbeout);
            if (ptprevbeout == NULL) {
                ptbefig->BEOUT = ptnextbeout;
            }
            else ptprevbeout->NEXT = ptnextbeout;
        }
        else ptprevbeout = ptbeout;
    }
    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptnextbebux) {
        ptnextbebux = ptbebux->NEXT;
        if (ptbebux->NAME == NULL) {
            mbkfree(ptbebux);
            if (ptprevbebux == NULL) {
                ptbefig->BEBUX = ptnextbebux;
            }
            else ptprevbebux->NEXT = ptnextbebux;
        }
        else ptprevbebux = ptbebux;
    }
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptnextbebus) {
        ptnextbebus = ptbebus->NEXT;
        if (ptbebus->NAME == NULL) {
            mbkfree(ptbebus);
            if (ptprevbebus == NULL) {
                ptbefig->BEBUS = ptnextbebus;
            }
            else ptprevbebus->NEXT = ptnextbebus;
        }
        else ptprevbebus = ptbebus;
    }

    destroyBdd(1);

    freeExpr(zeroexpr);
    freeExpr(oneexpr);

    // creating transmission lines
    yadAddRC(rcht, ptbefig);
    delht(rcht);
}

/****************************************************************************
*                         function yagGetTotalDelay();                      *
****************************************************************************/
static unsigned int
yagGetTotalDelay(chain_list *ptconelist)
{
    chain_list     *ptchain;
    unsigned int    result = 0;

    for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) {
        result += yagGetDelay((cone_list *)ptchain->DATA, NULL, NULL);
    }
    return result;
}

/****************************************************************************
*                         function yagGetDelay();                           *
****************************************************************************/
static unsigned int
yagGetDelay(cone_list *ptcone, chain_list *cndexpr, chain_list *valexpr)
{
    edge_list  *ptincone;
    ptype_list *ptuser;
    delay_list *delay;
    chain_list *driverinputs = NULL;
    char       *inputname;
    long        delaymax = 0;
    long        delaymin = 0;
    long        delaysum = 0;
    int         useRising = TRUE;
    int         useFalling = TRUE;
    int         n = 0;

    if (ptcone == NULL) return 0;

    if (valexpr != NULL) {
        if (ATOM(valexpr)) {
            if (!strcmp (VALUE_ATOM (valexpr), "'1'")) useFalling = FALSE;
            if (!strcmp (VALUE_ATOM (valexpr), "'0'")) useRising = FALSE;
            if (!strcmp (VALUE_ATOM (valexpr), "'z'") || !strcmp (VALUE_ATOM (valexpr), "'u'")) {
                return 0;
            }
        }
    }

    if (cndexpr != NULL) driverinputs = supportChain_listExpr(cndexpr);
    if (valexpr != NULL) driverinputs = append(driverinputs, supportChain_listExpr(valexpr));

    switch (YAG_CONTEXT->YAG_TAS_TIMING) {
    case YAG_MAX_TIMING:
        for (ptincone = ptcone->INCONE; ptincone; ptincone = ptincone->NEXT) {
            if ((ptincone->TYPE & CNS_EXT) == 0) inputname = ptincone->UEDGE.CONE->NAME;
            else inputname = ptincone->UEDGE.LOCON->NAME;
            if (driverinputs != NULL && yagGetChain(driverinputs, mbk_devect(inputname, "[", "]")) == NULL) continue;
            ptuser = getptype(ptincone->USER, TAS_DELAY_MAX);
            if (ptuser != NULL) {
                delay = (delay_list *)ptuser->DATA;
                if (useRising && (delay->TPHH != TAS_NOTIME) && (n == 0 || delay->TPHH > delaymax)) {
                    delaymax = delay->TPHH;
                    n++;
                }
                if (useFalling && (delay->TPHL != TAS_NOTIME) && (n == 0 || delay->TPHL > delaymax)) {
                    delaymax = delay->TPHL;
                    n++;
                }
                if (useRising && (delay->TPLH != TAS_NOTIME) && (n == 0 || delay->TPLH > delaymax)) {
                    delaymax = delay->TPLH;
                    n++;
                }
                if (useFalling && (delay->TPLL != TAS_NOTIME) && (n == 0 || delay->TPLL > delaymax)) {
                    delaymax = delay->TPLL;
                    n++;
                }
            }
        }
        delaymax=mbk_long_round(delaymax/TTV_UNIT);
        freechain(driverinputs);
        if (delaymax > 0) return ((unsigned int)delaymax);
        else return (unsigned int)(V_FLOAT_TAB[__YAG_DELTA_DELAY].VALUE*1e12*BEH_CNV_PS+0.5);
        break;
    case YAG_MIN_TIMING:
        for (ptincone = ptcone->INCONE; ptincone; ptincone = ptincone->NEXT) {
            if ((ptincone->TYPE & CNS_EXT) == 0) inputname = ptincone->UEDGE.CONE->NAME;
            else inputname = ptincone->UEDGE.LOCON->NAME;
            if (driverinputs != NULL && yagGetChain(driverinputs, inputname) == NULL) continue;
            ptuser = getptype(ptincone->USER, TAS_DELAY_MIN);
            if (ptuser != NULL) {
                delay = (delay_list *)ptuser->DATA;
                if (useRising && (delay->TPHH != TAS_NOTIME) && (n == 0 || delay->TPHH < delaymin)) {
                    delaymin = delay->TPHH;
                    n++;
                }
                if (useFalling && (delay->TPHL != TAS_NOTIME) && (n == 0 || delay->TPHL < delaymin)) {
                    delaymin = delay->TPHL;
                    n++;
                }
                if (useRising && (delay->TPLH != TAS_NOTIME) && (n == 0 || delay->TPLH < delaymin)) {
                    delaymin = delay->TPLH;
                    n++;
                }
                if (useFalling && (delay->TPLL != TAS_NOTIME) && (n == 0 || delay->TPLL < delaymin)) {
                    delaymin = delay->TPLL;
                    n++;
                }
            }
        }
        delaymin=mbk_long_round(delaymin/TTV_UNIT);
        freechain(driverinputs);
        if (delaymin > 0) return ((unsigned int)delaymin);
        else return (unsigned int)(V_FLOAT_TAB[__YAG_DELTA_DELAY].VALUE*1e12*BEH_CNV_PS+0.5);
        break;
    case YAG_MED_TIMING:
        for (ptincone = ptcone->INCONE; ptincone; ptincone = ptincone->NEXT) {
            if ((ptincone->TYPE & CNS_EXT) == 0) inputname = ptincone->UEDGE.CONE->NAME;
            else inputname = ptincone->UEDGE.LOCON->NAME;
            if (driverinputs != NULL && yagGetChain(driverinputs, inputname) == NULL) continue;
            ptuser = getptype(ptincone->USER, TAS_DELAY_MAX);
            if (ptuser != NULL) {
                delay = (delay_list *)ptuser->DATA;
                if (useRising && delay->TPHH != TAS_NOTIME) {
                    delaysum += delay->TPHH;
                    n++;
                }
                if (useFalling && delay->TPHL != TAS_NOTIME) {
                    delaysum += delay->TPHL;
                    n++;
                }
                if (useRising && delay->TPLH != TAS_NOTIME) {
                    delaysum += delay->TPLH;
                    n++;
                }
                if (useFalling && delay->TPLL != TAS_NOTIME) {
                    delaysum += delay->TPLL;
                    n++;
                }
            }
        }
        if (n) delaysum=mbk_long_round(delaysum/(TTV_UNIT*n));
        freechain(driverinputs);
        if (delaysum > 0) return ((unsigned int)(delaysum));
        else return (unsigned int)(V_FLOAT_TAB[__YAG_DELTA_DELAY].VALUE*1e12*BEH_CNV_PS+0.5);
        break;
    }
    return 0;
}

static int
yagConstantAbl(ptexpr)
    chain_list *ptexpr;
{
    if (!ATOM(ptexpr)) return FALSE;
    if (!strcmp(VALUE_ATOM(ptexpr), "'0'") 
     || !strcmp(VALUE_ATOM(ptexpr), "'1'") 
     || !strcmp(VALUE_ATOM(ptexpr), "'z'") 
     || !strcmp(VALUE_ATOM(ptexpr), "'u'")) {
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
*                    function yagTimeSensitiveBebux();                      *
****************************************************************************/
static int
yagTimeSensitiveBebux(bebux_list *ptbebux, cone_list *ptcone)
{
    biabl_list *ptbiabl;
    biabl_list *ptnewbiabl = NULL;
    chain_list *ptsupport;
    chain_list *ptchain;
    chain_list *ptinputexpr, *trigger, *ptnewcnd;
    unsigned int delay;

    /* Check that all drivers are constants */
    for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        if (!yagConstantAbl(ptbiabl->VALABL)) return FALSE;
    }

    for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        ptsupport = supportChain_listExpr(ptbiabl->CNDABL);
        for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
            ptinputexpr = createAtom(ptchain->DATA);
            delay = yagGetDelay(ptcone, ptinputexpr, ptbiabl->VALABL);
            trigger = createExpr(STABLE);
            addQExpr(trigger, ptinputexpr);
            ptnewcnd = createExpr(AND);
            addQExpr(ptnewcnd, copyExpr(ptbiabl->CNDABL));
            addQExpr(ptnewcnd, notExpr(trigger));
            ptnewbiabl = beh_addbiabl(ptnewbiabl, "label", ptnewcnd, copyExpr(ptbiabl->VALABL));
            ptnewbiabl->TIME = delay;
        }
    }
    ptnewbiabl = (biabl_list *)reverse((chain_list *)ptnewbiabl);
    beh_frebiabl(ptbebux->BIABL);
    ptbebux->BIABL = ptnewbiabl;
    return TRUE;
}
    
/****************************************************************************
*                    function yagTimeSensitiveBereg();                      *
****************************************************************************/
static int
yagTimeSensitiveBereg(bereg_list *ptbereg, cone_list *ptcone)
{
    biabl_list *ptbiabl;
    biabl_list *ptnewbiabl = NULL;
    chain_list *ptsupport;
    chain_list *ptchain;
    chain_list *ptinputexpr, *trigger, *ptnewcnd;
    unsigned int delay;

    /* Check that all drivers are constants */
    for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        if (!yagConstantAbl(ptbiabl->VALABL)) return FALSE;
    }

    for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        ptsupport = supportChain_listExpr(ptbiabl->CNDABL);
        for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
            ptinputexpr = createAtom(ptchain->DATA);
            delay = yagGetDelay(ptcone, ptinputexpr, ptbiabl->VALABL);
            trigger = createExpr(STABLE);
            addQExpr(trigger, ptinputexpr);
            ptnewcnd = createExpr(AND);
            addQExpr(ptnewcnd, copyExpr(ptbiabl->CNDABL));
            addQExpr(ptnewcnd, notExpr(trigger));
            ptnewbiabl = beh_addbiabl(ptnewbiabl, "label", ptnewcnd, copyExpr(ptbiabl->VALABL));
            ptnewbiabl->TIME = delay;
        }
    }
    ptnewbiabl = (biabl_list *)reverse((chain_list *)ptnewbiabl);
    beh_frebiabl(ptbereg->BIABL);
    ptbereg->BIABL = ptnewbiabl;
    return TRUE;
}

/****************************************************************************
*                    function yagTimeSensitiveBebus();                      *
****************************************************************************/
static int
yagTimeSensitiveBebus(bebus_list *ptbebus, cone_list *ptcone)
{
    biabl_list *ptbiabl;
    biabl_list *ptnewbiabl = NULL;
    chain_list *ptsupport;
    chain_list *ptchain;
    chain_list *ptinputexpr, *trigger, *ptnewcnd;
    unsigned int delay;

    /* Check that all drivers are constants */
    for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        if (!yagConstantAbl(ptbiabl->VALABL)) return FALSE;
    }

    for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        ptsupport = supportChain_listExpr(ptbiabl->CNDABL);
        for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
            ptinputexpr = createAtom(ptchain->DATA);
            delay = yagGetDelay(ptcone, ptinputexpr, ptbiabl->VALABL);
            trigger = createExpr(STABLE);
            addQExpr(trigger, ptinputexpr);
            ptnewcnd = createExpr(AND);
            addQExpr(ptnewcnd, copyExpr(ptbiabl->CNDABL));
            addQExpr(ptnewcnd, notExpr(trigger));
            ptnewbiabl = beh_addbiabl(ptnewbiabl, "label", ptnewcnd, copyExpr(ptbiabl->VALABL));
            ptnewbiabl->TIME = delay;
        }
    }
    ptnewbiabl = (biabl_list *)reverse((chain_list *)ptnewbiabl);
    beh_frebiabl(ptbebus->BIABL);
    ptbebus->BIABL = ptnewbiabl;
    return TRUE;
}

/****************************************************************************
*                    function yagCalcStmResPair();                          *
****************************************************************************/
int
yagCalcStmResPair(chain_list *uplist, chain_list *downlist, cone_list *ptcone, float *ptupresistance, float *ptdownresistance)
{
    int     result = FALSE;
    
    if (yagCalcStmResistance(uplist, ptcone, CNS_VDD, ptupresistance)) {
        result = yagCalcStmResistance(downlist, ptcone, CNS_VSS, ptdownresistance);
    }
    return result;
}

/****************************************************************************
*                    function yagCalcStmResistance();                       *
****************************************************************************/
int
yagCalcStmResistance(chain_list *ptbranchlist, cone_list *ptcone, long type, float *ptresistance)
{
    chain_list     *ptchain;
    alim_list      *powercone;
    float           sum = 0.0;
    float           current;
    float           vout, vsupply, vin;
    int             result;
    
    powercone =cns_get_multivoltage(ptcone);
    if (powercone == NULL) {
        vout = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE / 2;
        if (type == CNS_VDD) {
            vsupply = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
            vin = 0.0;
        }
        else {
            vsupply = 0.0;
            vin = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
        }
    }
    else {
        vout = powercone->VSSMIN + (powercone->VDDMAX - powercone->VSSMIN) / 2;
        if (type == CNS_VDD) {
            vsupply = powercone->VDDMAX;
            vin = 0.0;
        }
        else {
            vsupply = powercone->VSSMIN;
            vin = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
        }
    }
    for (ptchain = ptbranchlist; ptchain; ptchain = ptchain->NEXT) {
        result = yagStmBranchCurrent((branch_list *)ptchain->DATA, vin, vout, vsupply, &current);
        if (!result) return FALSE;
        sum += current;
    }
    if (type == CNS_VSS) sum = -sum;
    *ptresistance = 1 / sum;
    return TRUE;
}

int 
yagStmBranchCurrent(branch_list *ptbranch, float vin, float vout, float vsupply, float *ptcurrent)
{
    stm_solver_maillon_list *ptstmlink;
    tpiv       *ptstmbranch;
    link_list  *ptlink;
    cone_list  *prevcone;
    lotrs_list *ptparatrans, *ptlotrs;
    ptype_list *ptuser;
    alim_list  *power;
    chain_list *ptparachain, *ptchain;
    double      vbulk;
    int         result;
    long        width, length;
    char        brtype;

    /* Only for non-degraded branches with at least one transistor */
    if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT && ptbranch->LINK->NEXT == NULL) return FALSE;
    if ((ptbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED) return FALSE;
    
    /* temporarily modify width for parallel transistors */
    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & CNS_EXT) == 0) {
            ptlotrs = ptlink->ULINK.LOTRS;
            if ((ptuser = getptype(ptlotrs->USER, MBK_TRANS_PARALLEL)) != NULL) {
                ptparachain = (chain_list *)ptuser->DATA;
                ptlotrs->USER = addptype(ptlotrs->USER, YAG_WIDTH_PTYPE, (void *)ptlotrs->WIDTH);
                length = ptlotrs->LENGTH;
                width = 0;
                for (ptchain = ptparachain; ptchain; ptchain = ptchain->NEXT) {
                    ptparatrans = (lotrs_list *)ptchain->DATA;
                    width += ptparatrans->WIDTH * (length / ptparatrans->LENGTH);
                }
                ptlotrs->WIDTH = width;
            }
        }
    }
  
    switch (TAS_CONTEXT->TAS_LEVEL) {
    case 2:
    case 4:
        brtype = TAS_TRMODEL_SPICE;
        break;
    default:
        brtype = TAS_TRMODEL_MCCRSAT;
    }
  
    /* create tpiv style branch and set supply */
    ptstmbranch = tpiv_createbranch(ptbranch->LINK, brtype);
    for (ptstmlink = ptstmbranch->HEAD; ptstmlink->NEXT; ptstmlink = ptstmlink->NEXT);
    ptstmlink->MAILLON->VS = vsupply;

    /* fix gate and bulk voltages */
    for( ptstmlink = ptstmbranch->HEAD, ptlink = ptbranch->LINK; ptstmlink; ptstmlink = ptstmlink->NEXT, ptlink = ptlink->NEXT) {
        if (TAS_CONTEXT->TAS_LEVEL == 2) {
            elp_lotrs_param_get(ptlink->ULINK.LOTRS,NULL,NULL, NULL,  NULL,NULL, NULL, NULL, NULL, NULL, &vbulk, NULL, NULL, NULL, NULL);
        }
        else vbulk = vsupply;
        tas_tpiv_set_vb( ptstmlink->MAILLON, vbulk );

        if ((ptlink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
            prevcone = (cone_list *)getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE)->DATA;
            power=cns_get_multivoltage(prevcone);
            if (power) {
                tas_tpiv_set_vg( ptstmlink->MAILLON, power->VDDMAX - power->VSSMIN );
                continue;
            }
        }
        tas_tpiv_set_vg( ptstmlink->MAILLON, vin );
    }

    result = stm_solver_i(ptstmbranch->HEAD, vout, ptcurrent);

    tpiv_freebranch(ptstmbranch);

    /* restore width */
    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & CNS_EXT) == 0) {
            ptlotrs = ptlink->ULINK.LOTRS;
            if ((ptuser = getptype(ptlotrs->USER, YAG_WIDTH_PTYPE)) != NULL) {
                ptlotrs->WIDTH = (long)ptuser->DATA;
                ptlotrs->USER = delptype(ptlotrs->USER, YAG_WIDTH_PTYPE);
            }
        }
    }

    return result;
}

void
yagPrepStmSolver(cone_list *ptcone)
{
    inffig_list *ifl;

    ifl = getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);
    cns_addmultivoltage_cone(NULL, ifl, ptcone);
}

#endif
