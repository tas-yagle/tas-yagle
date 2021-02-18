/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_util.c                                                  */
/*                                                                          */
/*    (c) copyright 2001 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Anthony LESTER                                          */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/
#include <cbh_util.h>

static chain_list *cbh_reduceabl(chain_list *expr);
static chain_list *cbh_reducecndabl(chain_list *expr);

cbh_classifier  *CBH_CLASSIFIER = NULL;
int              CBH_TRACE = 0;    
int              CBH_BDD = 0; 
int              CBH_TEST = 0;

void cbh_gettraceenv(void);

/****************************************************************************/
/*{{{                    Utilities                                          */
/****************************************************************************/
/*{{{             cbh_newcomb                                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
cbhcomb *
cbh_newcomb(void)
{
  cbhcomb *ptcbhcomb;

  ptcbhcomb             = mbkalloc(sizeof(cbhcomb));
  ptcbhcomb->NAME       = NULL;
  ptcbhcomb->FUNCTION   = NULL;
  ptcbhcomb->HZFUNC     = NULL;
  ptcbhcomb->CONFLICT   = NULL;
  ptcbhcomb->FANOUT     = 1.0;

  return ptcbhcomb;
}

/*}}}************************************************************************/
/*{{{             cbh_newseq                                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
cbhseq *
cbh_newseq(void)
{
  cbhseq *ptcbhseq;

  ptcbhseq = mbkalloc(sizeof(cbhseq));
  ptcbhseq->NEXT        = NULL;
  ptcbhseq->SEQTYPE     = CBH_UNKNOWN;
  ptcbhseq->NAME        = NULL;
  ptcbhseq->NEGNAME     = NULL;
  ptcbhseq->PIN         = NULL;
  ptcbhseq->HZPIN       = NULL;
  ptcbhseq->NEGPIN      = NULL;
  ptcbhseq->HZNEGPIN    = NULL;
  ptcbhseq->STATEPIN    = NULL;
  ptcbhseq->LATCHNAME   = NULL;
  ptcbhseq->SLAVENAME   = NULL;
  ptcbhseq->POLARITY = 0;
  ptcbhseq->MSPOLARITY  = 0;
  ptcbhseq->CLOCK       = NULL;
  ptcbhseq->SLAVECLOCK  = NULL;
  ptcbhseq->DATA        = NULL;
  ptcbhseq->RESET       = NULL;
  ptcbhseq->SET         = NULL;
  ptcbhseq->RSCONF      = NULL;
  ptcbhseq->RSCONFNEG   = NULL;

  return ptcbhseq;
}

/*}}}************************************************************************/
/*{{{             cbh_delcomb                                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
cbh_delcomb(cbhcomb *ptcbhcomb)
{
  if (ptcbhcomb != NULL)
  {
    if (ptcbhcomb->FUNCTION != NULL)
      freeExpr(ptcbhcomb->FUNCTION);
    if (ptcbhcomb->HZFUNC != NULL)
      freeExpr(ptcbhcomb->HZFUNC);
    if (ptcbhcomb->CONFLICT != NULL)
      freeExpr(ptcbhcomb->CONFLICT);
    mbkfree(ptcbhcomb);
  }
}

/*}}}************************************************************************/
/*{{{             cbh_delseq                                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
cbh_delseq(cbhseq *ptcbhseq)
{
  if (ptcbhseq != NULL)
  {
    if (ptcbhseq->CLOCK != NULL)
      freeExpr(ptcbhseq->CLOCK);
    if (ptcbhseq->SLAVECLOCK != NULL)
      freeExpr(ptcbhseq->SLAVECLOCK);
    if (ptcbhseq->DATA != NULL)
      freeExpr(ptcbhseq->DATA);
    if (ptcbhseq->RESET != NULL)
      freeExpr(ptcbhseq->RESET);
    if (ptcbhseq->SET != NULL)
      freeExpr(ptcbhseq->SET);
    if (ptcbhseq->RSCONF != NULL)
      freeExpr(ptcbhseq->RSCONF);
    if (ptcbhseq->RSCONFNEG != NULL)
      freeExpr(ptcbhseq->RSCONFNEG);
    mbkfree(ptcbhseq);
  }
}

/*}}}************************************************************************/
/*{{{             cbh_clearcct                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
cbh_clearcct(befig_list *ptcellbefig)
{
    if (ptcellbefig->CIRCUI != NULL && ptcellbefig->CIRCUI != (void *)-1) {
        destroyCct(ptcellbefig->CIRCUI);
        ptcellbefig->CIRCUI = (void *)-1;
    }
}

/*}}}************************************************************************/
/*{{{             cbh_writeabl                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
cbh_writeabl(FILE *ptfile, chain_list *expr, char lib_or_tlf)
{
  char      *buffer;
  int        bufsize = 512;

  if( !ptfile )
    ptfile = stdout ;
  if (expr == NULL)
    return;
  buffer    = mbkalloc(bufsize);
  buffer[0] = '\0';
  buffer    = cbh_abl2str(expr, buffer, &bufsize, lib_or_tlf);
  fprintf(ptfile, "%s", buffer);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{             cbh_abl2str                                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *
cbh_abl2str(chain_list *expr, char *buffer, int *size_pnt, char lib_or_tlf)
{
  int             operator;
  char           *oper;
  struct chain   *operand;
  static char    *str_z     = NULL;
  static char    *str_o     = NULL;
  static char    *str_l     = NULL;
  static char    *str_h     = NULL;
  static char    *str_d     = NULL;
  static char    *str_hz    = NULL;
  static char    *str_n     = NULL;
  static char    *str_t     = NULL;
  static char    *str_x     = NULL;
  char           *name;

  if (str_z == NULL)
  {
    str_z   = namealloc("'0'");
    str_o   = namealloc("'1'");
    str_l   = namealloc("'l'");
    str_h   = namealloc("'h'");
    str_d   = namealloc("'d'");
    str_hz  = namealloc("'z'");
    str_n   = namealloc("'n'");
    str_t   = namealloc("'t'");
    str_x   = namealloc("'x'");
  }

  if (*size_pnt < ((int)strlen(buffer) + 256))
  {
    *size_pnt   = *size_pnt + 512;
    buffer      = (char *) mbkrealloc(buffer, *size_pnt);
  }

  if (expr->NEXT == NULL)
  {
    if ((expr->DATA == (void *) str_hz) || (expr->DATA == (void *) str_d))
      strcat(buffer, expr->DATA);
    else if (expr->DATA == (void *) str_o)
      strcat(buffer, "1");
    else if (expr->DATA == (void *) str_z)
      strcat(buffer, "0");
    else if (expr->DATA == (void *) str_h)
      strcat(buffer, "H");
    else if (expr->DATA == (void *) str_l)
      strcat(buffer, "L");
    else if (expr->DATA == (void *) str_n)
      strcat(buffer, "N");
    else if (expr->DATA == (void *) str_t)
      strcat(buffer, "T");
    else if (expr->DATA == (void *) str_x)
      strcat(buffer, "X");
    else{
        name = cbh_vectorize(expr->DATA);
        if (isdigit((int)name[0])){
            if(lib_or_tlf == 'L'){
                strcat(buffer, "\\\"");
                strcat(buffer, name);
                strcat(buffer, "\\\"");
            }else if(lib_or_tlf == 'T'){
                strcat(buffer, "\"");
                strcat(buffer, name);
                strcat(buffer, "\"");
            }
        }else{
            strcat(buffer, name);
        }
    }
  }
  else
  {
    operator = OPER(expr);
    operand  = CADR(expr);

    if (operator == NOT)
    {
      strcat(buffer, "!(");
      buffer = cbh_abl2str(operand, buffer, size_pnt, lib_or_tlf);
      strcat(buffer, ")");
    }
    else
    {
      oper = cbh_oper2char(operator);
      strcat(buffer, "(");
      while ((expr = expr->NEXT) != NULL)
      {
        buffer = cbh_abl2str(expr->DATA, buffer, size_pnt, lib_or_tlf);
        if (expr->NEXT)
        {
          strcat(buffer, " ");
          strcat(buffer, oper);
          strcat(buffer, " ");
        }
      }
      strcat(buffer, ")");
    }
  }

  return (buffer);
}

/*}}}************************************************************************/
/*{{{             cbh_oper2char                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *
cbh_oper2char(short oper)
{
  switch (oper)
  {
    case NOT:
         return (namealloc("!"));
    case AND:
         return (namealloc("&"));
    case OR:
         return (namealloc("|"));
    case XOR:
         return (namealloc("^"));
    default:
         avt_errmsg(CBH_ERRMSG,"000",AVT_FATAL,"002");
         //printf("\nablError - cbh_oper2char: unknown operator %d\n", oper);
         //EXIT(-1);
  }
  return NULL;
}

/*}}}************************************************************************/
/*{{{             cbh_vectorize                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *cbh_vectorize(char *name)
{
  char           *new_name;
  char            tmp[1024];

  if(vectorindex(name) == -1){
      new_name = name;
  }else{
      if(LIB_BUS_DELIM[1] != '\0')
          sprintf(tmp, "%s%c%d%c", vectorradical(name), LIB_BUS_DELIM[0], vectorindex(name), LIB_BUS_DELIM[1]);
      else
          sprintf(tmp, "%s%c%d", vectorradical(name), LIB_BUS_DELIM[0], vectorindex(name));
      new_name = namealloc(tmp);
  }
  return (new_name);
}

/*}}}************************************************************************/
/*{{{             cbh_getchain                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *
cbh_getchain(chain_list *pthead, void *ptdata)
{
  chain_list     *pt = NULL;

  for (pt = pthead; pt != NULL; pt = pt->NEXT)
  {
    if ((char *) pt->DATA == ptdata)
      break;
  }
  return (pt);
}

/*}}}************************************************************************/
/*{{{             cbh_unionchain                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *
cbh_unionchain(chain_list *ptchain1, chain_list *ptchain2)
{
  chain_list     *ptchain;

  for (ptchain = ptchain2; ptchain; ptchain = ptchain->NEXT)
    if (cbh_getchain(ptchain1, ptchain->DATA) == NULL)
      ptchain1 = addchain(ptchain1, ptchain->DATA);

  return ptchain1;
}

/*}}}************************************************************************/
/*{{{             cbh_countchains                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int
cbh_countchains(chain_list *headchain)
{
  chain_list     *ptchain;
  int             count = 0;

  for (ptchain = headchain; ptchain; ptchain = ptchain->NEXT)
    count++;
  return count;
}

/*}}}************************************************************************/
/*{{{             cbh_testnegname                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int
cbh_testnegname(char *name)
{
  char   c;

  if (name == NULL)
    return FALSE;
  c     = *(name + strlen(name) - 1);
  if (c == 'n' || c == 'N')
    return TRUE;
  return FALSE;
}

/*}}}************************************************************************/
/*{{{             cbh_calhzfunc                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_calchzfunc(pCircuit pC, biabl_list *ptheadbiabl, cbhcomb *ptcbhcomb)
{
  biabl_list *ptbiabl;
  pNode       ptval, ptcond;
  pNode       sup = BDD_zero;
  pNode       sdn = BDD_zero;
  pNode       hz, conflict;

  for (ptbiabl = ptheadbiabl; ptbiabl; ptbiabl = ptbiabl->NEXT)
  {
    ptcond  = ablToBddCct(pC, ptbiabl->CNDABL);
    ptval   = ablToBddCct(pC, ptbiabl->VALABL);
    if (ptval == BDD_one)
      sup = applyBinBdd(OR, sup, ptcond);
    else if (ptval == BDD_zero)
      sdn = applyBinBdd(OR, sdn, ptcond);
    else
    {
      sup = applyBinBdd(OR, sup, applyBinBdd(AND, ptcond, ptval));
      sdn = applyBinBdd(OR, sdn, applyBinBdd(AND, ptcond, notBdd(ptval)));
    }
  }
  hz                    = applyBinBdd(AND, notBdd(sup), notBdd(sdn));
  if (hz != BDD_zero)
    ptcbhcomb->HZFUNC   = bddToAblCct(pC, hz);
  conflict              = applyBinBdd(AND, sup, sdn);
  if (conflict != BDD_zero)
    ptcbhcomb->CONFLICT = bddToAblCct(pC, conflict);
  ptcbhcomb->FUNCTION   = bddToAblCct(pC, constraintBdd(sup, notBdd(hz)));
}

/*}}}************************************************************************/
/*{{{             cbh_calcsense                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int cbh_calcsense(befig_list *ptcellbefig, chain_list *ptexpr, char *varname)
{
  pNode       f0, f1;
  pNode       ptvarbdd, ptbdd;
  int         polarity = CBH_NONE;
  int         index;

  if (ptcellbefig->CIRCUI == NULL)
    return CBH_ERROR;
  index       = searchInputCct(ptcellbefig->CIRCUI, varname);
  ptvarbdd    = createNodeTermBdd(index);
  ptbdd       = ablToBddCct(ptcellbefig->CIRCUI, ptexpr);
  f1          = constraintBdd(ptbdd, ptvarbdd);
  f0          = constraintBdd(ptbdd, notBdd(ptvarbdd));
  if (applyBinBdd(AND, f1, notBdd(f0)) != BDD_zero)
    polarity |= CBH_NONINVERT;
  if (applyBinBdd(AND, f0, notBdd(f1)) != BDD_zero)
    polarity |= CBH_INVERT;

  return polarity;
}

/*}}}************************************************************************/
/*{{{             cbh_calccondition                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *
cbh_calccondition(befig_list *ptcellbefig, char *pinname, char *varname, int polarity)
{
    pNode       f0, f1;
    pNode       ptvarbdd, ptbdd = NULL;
    pNode       condition = NULL;
    beout_list *ptbeout   = NULL;
    int         index;

    if (ptcellbefig->ERRFLG != 0 || ptcellbefig->CIRCUI == NULL) return NULL;
    if (ptcellbefig->BEAUX || ptcellbefig->BEBUX || ptcellbefig->BEREG) return NULL;
    pinname = namealloc(pinname);
    varname = namealloc(varname);
    index = searchInputCct(ptcellbefig->CIRCUI, varname);
    ptvarbdd = createNodeTermBdd(index);
    for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        if (ptbeout->NAME == pinname) {
            ptbdd = ablToBddCct(ptcellbefig->CIRCUI, ptbeout->ABL);
        }
    }
    if (!ptbdd) return NULL;

    f1 = constraintBdd(ptbdd, ptvarbdd);
    f0 = constraintBdd(ptbdd, notBdd(ptvarbdd));

    if (polarity == CBH_NONINVERT) {
        condition = applyBinBdd(AND, f1, notBdd(f0));
    }
    else if (polarity == CBH_INVERT) {
        condition = applyBinBdd(AND, f0, notBdd(f1));
    }

    if (condition) return bddToAblCct(ptcellbefig->CIRCUI, condition);
    return NULL;
}

static int search_inverted(chain_list *ptabl, char *varname)
{
  int        num = 0;

  if (ATOM(ptabl))
    return 0;

  /* on verifie si le prochain signal est la VAR */
  if (OPER(ptabl) == NOT)
    if (VALUE_ATOM(CADR(ptabl)) == varname)
      return 1;
    else
      return 0;
  else
    while ( (ptabl = CDR(ptabl)) )
      num += search_inverted(CAR(ptabl), varname);

  return num;
}

static int search_non_inverted(chain_list *ptabl, char *varname)
{
  int        num = 0;

  if (ATOM(ptabl)) {
    if (VALUE_ATOM(ptabl) == varname)
      return 1;
    else
      return 0;
  }

  /* on verifie si le prochain signal est la VAR */
  if (OPER(ptabl) == NOT)
    return 0;
  else
    while ( (ptabl = CDR(ptabl)) )
      num += search_inverted(CAR(ptabl), varname);

  return num;
}

int cbh_calcsense_abl(chain_list *ptexpr, char *varname)
{
  chain_list *ptexpr1, *ptexpr2;
  int         polarity = CBH_NONE;
  int         num_invert, num_non_invert;

  ptexpr1 = devXorExpr(ptexpr);
  ptexpr2 = flatPolarityExpr(ptexpr1, 1);
  num_invert = search_non_inverted(ptexpr2, varname);
  num_non_invert = search_inverted(ptexpr2, varname);
  freeExpr(ptexpr1);
  freeExpr(ptexpr2);

  if (num_invert > 0 && num_non_invert == 0) polarity = CBH_INVERT;
  if (num_non_invert > 0 && num_invert == 0) polarity = CBH_NONINVERT;
  return polarity;
}

/*}}}************************************************************************/
/*{{{             cbh_suppressaux                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int cbh_suppressaux(befig_list *ptcellbefig)
{
  beaux_list    *ptbeaux;
  beout_list    *ptbeout;
  berin_list    *ptberin;
  berin_list    *pttmpberin;
  berin_list    *ptinput;
  beout_list    *ptoutref;
  chain_list    *outrefchain = NULL;
  beaux_list    *ptauxref;
  chain_list    *auxrefchain = NULL;
  beaux_list    *ptdlyref;
  chain_list    *dlyrefchain = NULL;
  bebus_list    *ptbusref;
  chain_list    *busrefchain = NULL;
  bebux_list    *ptbuxref;
  chain_list    *buxrefchain = NULL;
  bereg_list    *ptregref;
  chain_list    *regrefchain = NULL;
  biabl_list    *ptbiabl;
  beaux_list    *ptnextbeaux;
  chain_list    *ptchain;
  chain_list    *ptinputchain, *tmp_expr;
  ht            *berinht;
  int            count = 0;

  if (ptcellbefig->ERRFLG == 1) return 1;

  for (ptberin = ptcellbefig->BERIN; ptberin; ptberin = ptberin->NEXT)
    count++;
  if (count != 0)
    berinht = addht(count*100);
  for (ptberin = ptcellbefig->BERIN; ptberin; ptberin = ptberin->NEXT)
    addhtitem(berinht, ptberin->NAME, (long)ptberin);

  for (ptbeaux = ptcellbefig->BEAUX; ptbeaux; ptbeaux = ptnextbeaux)
  {
    ptnextbeaux = ptbeaux->NEXT;
    if (ptbeaux->ABL == NULL) continue;
    ptberin     = (berin_list *)gethtitem(berinht, ptbeaux->NAME);
    if ( ptberin == (berin_list *)DELETEHT
         || ptberin == (berin_list *)EMPTYHT
         || ptbeaux->ABL == NULL)
    {
      ptcellbefig->ERRFLG = 1;
      return 1;
    }
    for (ptchain = ptberin->OUT_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptoutref       = (beout_list *)ptchain->DATA;
      ptoutref->ABL  = substExpr(tmp_expr=ptoutref->ABL, ptberin->NAME, ptbeaux->ABL);
      freeExpr(tmp_expr);
      ptoutref->TIME = ptoutref->TIME + ptbeaux->TIME;
      if (cbh_getchain(outrefchain, ptoutref) == NULL)
        outrefchain = addchain(outrefchain, ptoutref);
    }
    for (ptchain = ptberin->AUX_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptauxref       = (beaux_list *)ptchain->DATA;
      if (ptauxref->NAME == NULL)
      {
        ptcellbefig->ERRFLG = 1;
        continue;
      }
      ptauxref->ABL  = substExpr(tmp_expr=ptauxref->ABL, ptberin->NAME, ptbeaux->ABL);
      freeExpr(tmp_expr);
      ptauxref->TIME = ptauxref->TIME + ptbeaux->TIME;
      if (cbh_getchain(auxrefchain, ptauxref) == NULL)
        auxrefchain  = addchain(auxrefchain, ptauxref);
    }
    for (ptchain = ptberin->DLY_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptdlyref       = (beaux_list *)ptchain->DATA;
      ptdlyref->ABL  = substExpr(tmp_expr=ptdlyref->ABL, ptberin->NAME, ptbeaux->ABL);
      freeExpr(tmp_expr);
      if (cbh_getchain(dlyrefchain, ptdlyref) == NULL)
        dlyrefchain  = addchain(dlyrefchain, ptdlyref);
    }
    for (ptchain = ptberin->BUS_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptbusref       = (bebus_list *)ptchain->DATA;
      for (ptbiabl   = ptbusref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
      {
        ptbiabl->CNDABL = substExpr(tmp_expr=ptbiabl->CNDABL, ptberin->NAME, ptbeaux->ABL);
        freeExpr(tmp_expr);
        ptbiabl->VALABL = substExpr(tmp_expr=ptbiabl->VALABL, ptberin->NAME, ptbeaux->ABL);
        freeExpr(tmp_expr);
      }
      if (cbh_getchain(busrefchain, ptbusref) == NULL)
        busrefchain  = addchain(busrefchain, ptbusref);
    }
    for (ptchain = ptberin->BUX_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptbuxref       = (bebux_list *)ptchain->DATA;
      for (ptbiabl   = ptbuxref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
      {
        ptbiabl->CNDABL = substExpr(tmp_expr=ptbiabl->CNDABL, ptberin->NAME, ptbeaux->ABL);
        freeExpr(tmp_expr);
        ptbiabl->VALABL = substExpr(tmp_expr=ptbiabl->VALABL, ptberin->NAME, ptbeaux->ABL);
        freeExpr(tmp_expr);
      }
      if (cbh_getchain(buxrefchain, ptbuxref) == NULL)
        buxrefchain  = addchain(buxrefchain, ptbuxref);
    }
    for (ptchain = ptberin->REG_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptregref       = (bereg_list *)ptchain->DATA;
      for (ptbiabl   = ptregref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
      {
        ptbiabl->CNDABL = substExpr(tmp_expr=ptbiabl->CNDABL, ptberin->NAME, ptbeaux->ABL);
        freeExpr(tmp_expr);
        ptbiabl->VALABL = substExpr(tmp_expr=ptbiabl->VALABL, ptberin->NAME, ptbeaux->ABL);
        freeExpr(tmp_expr);
      }
      if (cbh_getchain(regrefchain, ptregref) == NULL)
        regrefchain  = addchain(regrefchain, ptregref);
    }
    ptinputchain     = supportChain_listExpr(ptbeaux->ABL);
    for (ptchain = ptinputchain; ptchain; ptchain = ptchain->NEXT)
    {
      ptinput = (berin_list *)gethtitem(berinht, (char *)ptchain->DATA);
      if ( ptinput == (berin_list *)DELETEHT
           || ptinput == (berin_list *)EMPTYHT)
      {
        ptcellbefig->ERRFLG = 1;
        return 1;
      }
      ptinput->AUX_REF = delchaindata(ptinput->AUX_REF, ptbeaux);
      ptinput->OUT_REF = cbh_unionchain(ptinput->OUT_REF, ptberin->OUT_REF);
      ptinput->AUX_REF = cbh_unionchain(ptinput->AUX_REF, ptberin->AUX_REF);
      ptinput->DLY_REF = cbh_unionchain(ptinput->DLY_REF, ptberin->DLY_REF);
      ptinput->BUS_REF = cbh_unionchain(ptinput->BUS_REF, ptberin->BUS_REF);
      ptinput->BUX_REF = cbh_unionchain(ptinput->BUX_REF, ptberin->BUX_REF);
      ptinput->REG_REF = cbh_unionchain(ptinput->REG_REF, ptberin->REG_REF);
    }
    freechain(ptinputchain);
    ptbeaux->NAME = NULL;
    if (auxrefchain != NULL)
      auxrefchain = delchaindata(auxrefchain, ptbeaux);
    delhtitem(berinht, ptberin->NAME);
    ptcellbefig->BERIN = beh_delberin(ptcellbefig->BERIN, ptberin);
  }
  for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT)
  {
    ptberin     = (berin_list *)gethtitem(berinht, ptbeout->NAME);
    if ( ptberin == (berin_list *)DELETEHT
         || ptberin == (berin_list *)EMPTYHT)
    {
      continue;
    }
    for (ptchain = ptberin->OUT_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptoutref       = (beout_list *)ptchain->DATA;
      ptoutref->ABL  = substExpr(ptoutref->ABL, ptberin->NAME, ptbeout->ABL);
      ptoutref->TIME = ptoutref->TIME + ptbeout->TIME;
      if (cbh_getchain(outrefchain, ptoutref) == NULL)
        outrefchain = addchain(outrefchain, ptoutref);
    }
    for (ptchain = ptberin->AUX_REF; ptchain; ptchain = ptchain->NEXT)
    {
      if (ptauxref->NAME == NULL)
      {
        ptcellbefig->ERRFLG = 1;
        continue;
      }
      ptauxref       = (beaux_list *)ptchain->DATA;
      ptauxref->ABL  = substExpr(ptauxref->ABL, ptberin->NAME, ptbeout->ABL);
      ptauxref->TIME = ptauxref->TIME + ptbeout->TIME;
      if (cbh_getchain(auxrefchain, ptauxref) == NULL)
        auxrefchain  = addchain(auxrefchain, ptauxref);
    }
    for (ptchain = ptberin->DLY_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptdlyref       = (beaux_list *)ptchain->DATA;
      ptdlyref->ABL  = substExpr(ptdlyref->ABL, ptberin->NAME, ptbeout->ABL);
      if (cbh_getchain(dlyrefchain, ptdlyref) == NULL)
        dlyrefchain  = addchain(dlyrefchain, ptdlyref);
    }
    for (ptchain = ptberin->BUS_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptbusref       = (bebus_list *)ptchain->DATA;
      for (ptbiabl   = ptbusref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
      {
        ptbiabl->CNDABL = substExpr(ptbiabl->CNDABL, ptberin->NAME, ptbeout->ABL);
        ptbiabl->VALABL = substExpr(ptbiabl->VALABL, ptberin->NAME, ptbeout->ABL);
      }
      if (cbh_getchain(busrefchain, ptbusref) == NULL)
        busrefchain  = addchain(busrefchain, ptbusref);
    }
    for (ptchain = ptberin->BUX_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptbuxref       = (bebux_list *)ptchain->DATA;
      for (ptbiabl   = ptbuxref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
      {
        ptbiabl->CNDABL = substExpr(ptbiabl->CNDABL, ptberin->NAME, ptbeout->ABL);
        ptbiabl->VALABL = substExpr(ptbiabl->VALABL, ptberin->NAME, ptbeout->ABL);
      }
      if (cbh_getchain(buxrefchain, ptbuxref) == NULL)
        buxrefchain  = addchain(buxrefchain, ptbuxref);
    }
    for (ptchain = ptberin->REG_REF; ptchain; ptchain = ptchain->NEXT)
    {
      ptregref       = (bereg_list *)ptchain->DATA;
      for (ptbiabl   = ptregref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
      {
        ptbiabl->CNDABL = substExpr(ptbiabl->CNDABL, ptberin->NAME, ptbeout->ABL);
        ptbiabl->VALABL = substExpr(ptbiabl->VALABL, ptberin->NAME, ptbeout->ABL);
      }
      if (cbh_getchain(regrefchain, ptregref) == NULL)
        regrefchain  = addchain(regrefchain, ptregref);
    }
    ptinputchain     = supportChain_listExpr(ptbeout->ABL);
    for (ptchain = ptinputchain; ptchain; ptchain = ptchain->NEXT)
    {
      ptinput = (berin_list *)gethtitem(berinht, (char *)ptchain->DATA);
      if ( ptinput == (berin_list *)DELETEHT
           || ptinput == (berin_list *)EMPTYHT)
      {
        ptcellbefig->ERRFLG = 1;
        return 1;
      }
      ptinput->OUT_REF = cbh_unionchain(ptinput->OUT_REF, ptberin->OUT_REF);
      ptinput->AUX_REF = cbh_unionchain(ptinput->AUX_REF, ptberin->AUX_REF);
      ptinput->DLY_REF = cbh_unionchain(ptinput->DLY_REF, ptberin->DLY_REF);
      ptinput->BUS_REF = cbh_unionchain(ptinput->BUS_REF, ptberin->BUS_REF);
      ptinput->BUX_REF = cbh_unionchain(ptinput->BUX_REF, ptberin->BUX_REF);
      ptinput->REG_REF = cbh_unionchain(ptinput->REG_REF, ptberin->REG_REF);
    }
    freechain(ptinputchain);
  }

  for (ptchain = outrefchain; ptchain; ptchain = ptchain->NEXT)
  {
    ptoutref      = (beout_list *)ptchain->DATA;
    ptoutref->ABL = cbh_reduceabl(ptoutref->ABL);
  }
  for (ptchain = auxrefchain; ptchain; ptchain = ptchain->NEXT)
  {
    ptauxref      = (beaux_list *)ptchain->DATA;
    ptauxref->ABL = cbh_reduceabl(ptauxref->ABL);
  }
  for (ptchain = dlyrefchain; ptchain; ptchain = ptchain->NEXT)
  {
    ptdlyref      = (beaux_list *)ptchain->DATA;
    ptdlyref->ABL = cbh_reduceabl(ptdlyref->ABL);
  }
  for (ptchain = busrefchain; ptchain; ptchain = ptchain->NEXT)
  {
    ptbusref      = (bebus_list *)ptchain->DATA;
    for (ptbiabl = ptbusref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
    {
      ptbiabl->CNDABL = cbh_reduceabl(ptbiabl->CNDABL);
      ptbiabl->VALABL = cbh_reduceabl(ptbiabl->VALABL);
    }
  }
  for (ptchain = buxrefchain; ptchain; ptchain = ptchain->NEXT)
  {
    ptbuxref      = (bebux_list *)ptchain->DATA;
    for (ptbiabl = ptbuxref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
    {
      ptbiabl->CNDABL = cbh_reduceabl(ptbiabl->CNDABL);
      ptbiabl->VALABL = cbh_reduceabl(ptbiabl->VALABL);
    }
  }
  for (ptchain = regrefchain; ptchain; ptchain = ptchain->NEXT)
  {
    ptregref      = (bereg_list *)ptchain->DATA;
    for (ptbiabl = ptregref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT)
    {
      ptbiabl->CNDABL = cbh_reducecndabl(ptbiabl->CNDABL);
      ptbiabl->VALABL = cbh_reduceabl(ptbiabl->VALABL);
    }
  }
  freechain(outrefchain);
  freechain(auxrefchain);
  freechain(dlyrefchain);
  freechain(busrefchain);
  freechain(buxrefchain);
  freechain(regrefchain);
  delht(berinht);
  
  for (ptbeaux = ptcellbefig->BEAUX; ptbeaux; ptbeaux = ptnextbeaux)
  {
    ptnextbeaux = ptbeaux->NEXT;
    if (ptbeaux->NAME == NULL)
    {
      ptcellbefig->BEAUX = beh_delbeaux(ptcellbefig->BEAUX, ptbeaux, 'Y');
    }
  }
  if (ptcellbefig->ERRFLG == 1) return 1;
  return 0;
}

/*}}}************************************************************************/
/*{{{                    cbh_reduceabl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *cbh_reduceabl(chain_list *expr)
{
  chain_list *support;
  pCircuit    circuit;
  chain_list *ptchain;
  pNode       bdd;
  chain_list *newexpr;

  support = supportChain_listExpr(expr);
  circuit = initializeCct("reduce", cbh_countchains(support), 10);

  for (ptchain = support; ptchain; ptchain = ptchain->NEXT)
    addInputCct(circuit, (char *)ptchain->DATA);
  bdd     = ablToBddCct(circuit, expr);
  newexpr = bddToAblCct(circuit, bdd);
  freeExpr(expr);
  freechain(support);
  destroyCct(circuit);

  return newexpr;
}

/*}}}************************************************************************/
/*{{{                    cbh_reducecndabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *cbh_reducecndabl(chain_list *expr)
{
  chain_list    *expr1;
  chain_list    *expr2;
  chain_list    *trigger    = NULL;
  chain_list    *condition  = NULL;
  chain_list    *trigexpr;

  if ( !ATOM(expr) && ( OPER(expr) == AND || lengthExpr(expr) == 2))
  {
    expr1 = CADR(expr);
    expr2 = CADR(CDR(expr));
    if ( !ATOM(expr2) && OPER(expr2) == NOT && !ATOM(CADR(expr2))
         && OPER(CADR(expr2)) == STABLE)
    {
      trigger     = CADR(CADR(expr2));
      condition   = expr1;
    }
    if ( trigger == NULL && !ATOM(expr1) && OPER(expr1) == NOT
         && !ATOM(CADR(expr1)) && OPER(CADR(expr1)) == STABLE)
    {
      trigger     = CADR(CADR(expr1));
      condition   = expr2;
    }
  }

  if (trigger == NULL)
    expr = cbh_reduceabl(expr);
  else
  {
    trigger   = cbh_reduceabl(copyExpr(trigger));
    condition = cbh_reduceabl(copyExpr(condition));
    freeExpr(expr);
    expr      = createExpr(AND);
    addQExpr(expr, condition);
    trigexpr  = createExpr(STABLE);
    addQExpr(trigexpr, trigger);
    addQExpr(expr, notExpr(trigexpr));
  }
  return expr;
}

/*}}}************************************************************************/
/*}}}                                                                          */
/*{{{                    Lofig application                                  */
/*                                                                          */
/*   written by anto april,the 29th 2002                                    */
/*                                                                          */
/****************************************************************************/
/*{{{                    Classifier                                         */
/****************************************************************************/
/*{{{             cbh_manageclassifier                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *cbh_manageclassifier(type,newlofig)
lofig_list   *newlofig;
int        type;
{
  chain_list    *chainx;
  chain_list    *funchead;
  chain_list    *lofhead;
  cbhcomb       *ptcomb;
  cbhseq        *ptseq;
  chain_list    *newinout[2];
  int            nbinout[2];
  int            func;
  float          fanout=0;
  char           buf[1024];
  int            pbuf;
  int            i;
  lofig_list   *reflofig;

  pbuf = sprintf(buf,"%c ",(type==CBH_CLASS)?'C':(type==CBH_GET)?'G':'D');
  cbh_getLoconFromLofig(newlofig,newinout,nbinout);

  ptseq  = cbh_getseqfromlofig(newlofig);
  if (newinout[CBH_OUT]) ptcomb = cbh_getcombfromlocon(newinout[CBH_OUT]->DATA);
  else ptcomb = NULL;
  if (ptcomb!=NULL) fanout = ptcomb->FANOUT;
  /* classement selon sequentiel ou combinatoire */
  if (ptseq!=NULL)
  {
    pbuf += sprintf(buf+pbuf,"seq    ");
    func = CBH_SEQ;
    chainx = CBH_CLASSIFIER->TYPE[CBH_SEQ];
    switch (ptseq->SEQTYPE)
    {
      case CBH_LATCH :
           break;
      case CBH_FLIPFLOP :
           if (!chainx->NEXT)
           {
             if (type==CBH_CLASS) chainx->NEXT=addchain(NULL,NULL);
             else goto erreur;
           }
           chainx=chainx->NEXT;
           break;
      default :
           CBH_PRINT("lofig de type inconnu %s\n",newlofig->NAME);
           cbh_freeInOutList(newinout);
           cbh_delcombtolofig(newlofig);
           cbh_delseqtolofig(newlofig);
           return NULL;
    }
  }
  else if (ptcomb!=NULL)
  {
    pbuf += sprintf(buf+pbuf,"comb   ");
    func = CBH_COMB;
    chainx = CBH_CLASSIFIER->TYPE[CBH_COMB];
  }
  else
  {
    pbuf += sprintf(buf+pbuf,"notype ");
    func = CBH_NOTYPE;
    chainx = CBH_CLASSIFIER->TYPE[CBH_NOTYPE];
  }
  if (chainx->DATA==NULL)
  {
    if (type==CBH_CLASS) chainx->DATA=addchain(NULL,NULL);
    else goto erreur;
  }
  chainx = chainx->DATA;

  while (pbuf<9) pbuf += sprintf(buf+pbuf," ");
  pbuf += sprintf(buf+pbuf,"\%s",newlofig->NAME);
  while (pbuf<9+CBH_MAXNL) pbuf += sprintf(buf+pbuf," ");
  pbuf += sprintf(buf+pbuf,"in:%2d ",nbinout[CBH_IN]);
  pbuf += sprintf(buf+pbuf,"out:%2d ",nbinout[CBH_OUT]);

  /* classement selon le nombre de connecteurs in */
  for (i=0;i<nbinout[CBH_IN];i++,chainx=chainx->NEXT)
    if (chainx->NEXT==NULL)
    {
      if (type==CBH_CLASS) chainx->NEXT=addchain(NULL,NULL);
      else goto erreur;
    }
  if (chainx->DATA==NULL)
  {
    if (type==CBH_CLASS) chainx->DATA=addchain(NULL,NULL);
    else goto erreur;
  }
  chainx = chainx->DATA;

  /* classement selon le nombre de connecteurs out */
  for (i=0;i<nbinout[CBH_OUT];i++,chainx=chainx->NEXT)
    if (chainx->NEXT==NULL)
    {
      if (type==CBH_CLASS) chainx->NEXT=addchain(NULL,NULL);
      else goto erreur;
    }
  if (chainx->DATA==NULL)
  {
    if (type==CBH_CLASS) chainx->DATA=addchain(NULL,NULL);
    else goto erreur;
  }
  funchead = chainx;
  chainx = chainx->DATA;

  /* classement selon la fonctionnalite */
  if (chainx->DATA==NULL)
  {
    if (type==CBH_CLASS)
    {
      chainx->DATA = addchain(NULL,newlofig);
      pbuf += sprintf(buf+pbuf,"new\n");
      CBH_PRINT(buf);
    }
    else goto erreur;
  }
  else
    for (;1;chainx=chainx->NEXT)
    {
      if ((type!=CBH_CLASS)&&(!chainx->DATA)) goto erreur;
      else
      {
        reflofig = (lofig_list*) ((chain_list*)(chainx->DATA))->DATA;
        if (cbh_cmplofig_IO(reflofig,newlofig,func,newinout,nbinout))
        {
          pbuf += sprintf(buf+pbuf,"matched to %s",reflofig->NAME);
          while (pbuf<33+2*CBH_MAXNL) pbuf += sprintf(buf+pbuf," ");
          pbuf += sprintf(buf+pbuf,"%5d ",CBH_PERMUT);
          pbuf += sprintf(buf+pbuf,"permuts");

          if (cbh_isclassedlofig(chainx->DATA,newlofig))
          {
            pbuf += sprintf(buf+pbuf," is already classed\n");
            CBH_PRINT(buf);
            cbh_freeInOutList(newinout);
            return NULL;
          }
          else
          {
            pbuf += sprintf(buf+pbuf," new classement\n");
            CBH_PRINT(buf);
          }

          lofhead = chainx;
          if (type==CBH_DEL)
          {
            if (lofhead)
            {
              chainx = lofhead->DATA;
              if (chainx->DATA==newlofig)
              {
                lofhead->DATA = delchain(chainx,chainx);
                cbh_freeInOutList(newinout);
                return NULL;
              }
              else
                for (;chainx->NEXT;chainx=chainx->NEXT)
                  if (chainx->NEXT->DATA==newlofig)
                  {
                    chainx->NEXT = delchain(chainx->NEXT,chainx->NEXT);
                    cbh_freeInOutList(newinout);
                    return NULL;
                  }
              goto erreur;
            }
            else goto erreur;
          }
          for (chainx=chainx->DATA;;chainx=chainx->NEXT)
          {
            if (cbh_cmpfanout((lofig_list*)chainx->DATA,fanout)<0)
            {
              if (type==CBH_CLASS)
              {
                chainx = addchain(chainx,newlofig);
                break;
              }
              else
              {
                cbh_freeInOutList(newinout);
                return chainx->DATA;
              }
            }
            else if (chainx->NEXT==NULL)
            {
              if (type==CBH_CLASS)
              {
                chainx->NEXT = addchain(NULL,newlofig);
                break;
              }
              else
              {
                cbh_freeInOutList(newinout);
                return chainx->DATA;
              }
            }
          }
          break;
        }
        else if (!chainx->NEXT)
        {
          if (type==CBH_CLASS)
          {
            funchead->DATA = addchain(funchead->DATA,addchain(NULL,newlofig));

            pbuf += sprintf(buf+pbuf,"new");
            while (pbuf<33+2*CBH_MAXNL) pbuf += sprintf(buf+pbuf," ");
            pbuf += sprintf(buf+pbuf,"%5d ",CBH_PERMUT);
            pbuf += sprintf(buf+pbuf,"permuts\n");
            CBH_PRINT(buf);

            break;
          }
          else goto erreur;
        }
      }
    }
  if (type!=CBH_CLASS)
    erreur :
      CBH_PRINT("lofig correspondante non trouvee\n");
  cbh_freeInOutList(newinout);
  return NULL;
}

/*}}}************************************************************************/
/*{{{             cbh_newclassifier                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
cbh_classifier *cbh_newclassifier()
{
  if (CBH_CLASSIFIER) return NULL;

  cbh_gettraceenv();
  CBH_CLASSIFIER = mbkalloc(sizeof(cbh_classifier));
  CBH_CLASSIFIER->TYPE[CBH_COMB]   = addchain(NULL,NULL);
  CBH_CLASSIFIER->TYPE[CBH_SEQ]    = addchain(NULL,NULL);
  CBH_CLASSIFIER->TYPE[CBH_NOTYPE] = addchain(NULL,NULL);
  CBH_CLASSIFIER->EXISTBDD         = ~CBH_EXISTBDD;

  return CBH_CLASSIFIER;
}

/*}}}************************************************************************/
/*{{{             cbh_freeclassifier                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_freeclassifier()
{
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_COMB]  ,3,CBH_COMB  |CBH_FREECLASS);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_SEQ]   ,3,CBH_SEQ   |CBH_FREECLASS);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_NOTYPE],3,CBH_NOTYPE|CBH_FREECLASS);

  mbkfree(CBH_CLASSIFIER);
  CBH_CLASSIFIER = NULL;
}

/*}}}************************************************************************/
/*{{{             cbh_displayclassifier                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_displayclassifier()
{
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_COMB]  ,3,CBH_COMB  |CBH_DISPLAY);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_SEQ]   ,3,CBH_SEQ   |CBH_DISPLAY);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_NOTYPE],3,CBH_NOTYPE|CBH_DISPLAY);
}

/*}}}************************************************************************/
/*{{{             cbh_isclassedlofig()                                      */
/*                                                                          */
/*                                                                          */
/* regarde si une lofig est classee pour une fonctionnalite precise head    */
/****************************************************************************/
int cbh_isclassedlofig(head,lofig)
chain_list  *head;
lofig_list  *lofig;
{
  chain_list    *chainx;

  for (chainx=head;chainx;chainx=chainx->NEXT)
  {
    if (lofig==chainx->DATA) return 1;
  }
  return 0;
}

/*}}}************************************************************************/
/*{{{             cbh_getclasslofig()                                       */
/*                                                                          */
/*                                                                          */
/* renvoie la chaine liste la lofig correspondante                          */
/****************************************************************************/
lofig_list *cbh_getclasslofig(lofig)
lofig_list  *lofig;
{
  chain_list    *res;

  res = cbh_manageclassifier(CBH_GET,lofig);
  return ((res)?((lofig_list*)res->DATA):((lofig_list*)res));
}

/*}}}************************************************************************/
/*{{{             cbh_delclasslofig()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_delclasslofig(lofig)
lofig_list  *lofig;
{
  cbh_manageclassifier(CBH_DEL,lofig);

  unlocklofig(lofig);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{             cbh_addcombtolocon                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_addcombtolocon(locon,ptcbhcomb)
locon_list   *locon;
cbhcomb     *ptcbhcomb;
{
  cbh_delcombtolocon(locon);
  locon->USER = addptype(locon->USER,CBH_TYPE_COMB,ptcbhcomb);
}

/*}}}************************************************************************/
/*{{{             cbh_delcombtolocon                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_delcombtolocon(locon)
locon_list   *locon;
{
  ptype_list   *ptype;

  if ((ptype = getptype(locon->USER,CBH_TYPE_COMB))!=NULL)
  {
    cbh_delcomb(ptype->DATA);
    locon->USER = delptype(locon->USER,CBH_TYPE_COMB);
  }
}

/*}}}************************************************************************/
/*{{{             cbh_getcombfromlocon                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
cbhcomb *cbh_getcombfromlocon(locon)
locon_list  *locon;
{
  ptype_list   *ptype;

  if ((ptype=getptype(locon->USER,CBH_TYPE_COMB))!=NULL)
    return ptype->DATA;
  else return NULL;
}

/*}}}************************************************************************/
/*{{{             cbh_addseqtolofig                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_addseqtolofig(lofig,ptcbhseq)
lofig_list  *lofig;
cbhseq      *ptcbhseq;
{
  cbh_delseqtolofig(lofig);
  lofig->USER = addptype(lofig->USER,CBH_TYPE_SEQ,ptcbhseq);
}
/*}}}************************************************************************/
/*{{{             cbh_delseqtolofig                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_delseqtolofig(lofig)
lofig_list   *lofig;
{
  ptype_list   *ptype;

  if ((ptype = getptype(lofig->USER,CBH_TYPE_SEQ))!=NULL)
  {
    cbh_delseq(ptype->DATA);
    lofig->USER = delptype(lofig->USER,CBH_TYPE_SEQ);
  }
}

/*}}}************************************************************************/
/*{{{             cbh_getseqfromlofig                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
cbhseq *cbh_getseqfromlofig(lofig)
lofig_list  *lofig;
{
  ptype_list   *ptype;

  if ((ptype =getptype(lofig->USER,CBH_TYPE_SEQ))!=NULL)
    return ptype->DATA;
  else return NULL;
}
/*}}}************************************************************************/
/*{{{             cbh_addccttolofig                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_addccttolofig(lofig,ptcct)
lofig_list  *lofig;
pCircuit    ptcct;
{
  cbh_delccttolofig(lofig);
  lofig->USER = addptype(lofig->USER,CBH_TYPE_CCT,ptcct);
}
/*}}}************************************************************************/
/*{{{             cbh_delccttolofig                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_delccttolofig(lofig)
lofig_list   *lofig;
{
  ptype_list   *ptype;

  if ((ptype = getptype(lofig->USER,CBH_TYPE_CCT))!=NULL)
  {
    destroyCct(ptype->DATA);
    lofig->USER = delptype(lofig->USER,CBH_TYPE_CCT);
  }
}

/*}}}************************************************************************/
/*{{{             cbh_getcctfromlofig                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
pCircuit cbh_getcctfromlofig(lofig)
lofig_list  *lofig;
{
  ptype_list   *ptype;

  if ((ptype = getptype(lofig->USER,CBH_TYPE_CCT)) != NULL)
    return ptype->DATA;
  else
    return cbh_docct(lofig);
}

/*}}}************************************************************************/
/*{{{             cbh_classlofig                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_classlofig(lofig)
lofig_list  *lofig;
{
  cbh_manageclassifier(CBH_CLASS,lofig);

  locklofig(lofig);
}

/*}}}************************************************************************/
/*{{{             cbh_gettraceenv                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_gettraceenv(void)
{
  char  *str;

  CBH_TRACE     = !CBH_DISPLAY;
  CBH_BDD       = 0;
  CBH_DEBUG     = 0;
  CBH_TEST      = 0;

  if ((str=getenv("CBH_TRACE_MODE")) && !strcasecmp(str,"yes"))
    CBH_TRACE   = CBH_DISPLAY;
  if ((str=getenv("CBH_BDD")) && !strcasecmp(str,"yes"))
    CBH_BDD     = 1;
  if ((str=getenv("CBH_DEBUG")) && !strcasecmp(str,"yes"))
    CBH_DEBUG   = 1;
  if ((str=getenv("CBH_TEST")) && !strcasecmp(str,"yes"))
  {
    CBH_DEBUG   = 1;
    CBH_TEST    = 1;
  }
}

/*}}}************************************************************************/
/*{{{             cbh_delcct()                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_delcct()
{
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_COMB]  ,3,CBH_COMB  |CBH_DELBDD);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_SEQ]   ,3,CBH_SEQ   |CBH_DELBDD);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_NOTYPE],3,CBH_NOTYPE|CBH_DELBDD);
  CBH_CLASSIFIER->EXISTBDD = ~CBH_EXISTBDD;

  destroyBdd(1);
}

/*}}}************************************************************************/
/*{{{             cbh_createcct()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_createcct()
{
  initializeBdd(SMALL_BDD);

  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_COMB]  ,3,CBH_COMB  |CBH_DOBDD);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_SEQ]   ,3,CBH_SEQ   |CBH_DOBDD);
  cbh_traversetree(CBH_CLASSIFIER->TYPE[CBH_NOTYPE],3,CBH_NOTYPE|CBH_DOBDD);
  CBH_CLASSIFIER->EXISTBDD = CBH_EXISTBDD;
}

/*}}}************************************************************************/
/*{{{             cbh_docct()                                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
pCircuit cbh_docct(lofig)
lofig_list  *lofig;
{
  chain_list    *inout[2];
  int            nbinout[2];
  int            loop[2];
  char          *namein[2];
  pCircuit       cct;
  chain_list    *mainabllist[CBH_MAXNBFUNC];
  int            i;
  chain_list    *chainx;
  chain_list    *abllist;
  char          *name;
  chain_list    *abl;

  if ((!CBH_CLASSIFIER) || (CBH_CLASSIFIER->EXISTBDD))
  {
    initializeBdd(SMALL_BDD);
    CBH_CLASSIFIER->EXISTBDD = CBH_EXISTBDD;
  }

  cbh_getLoconFromLofig(lofig,inout,nbinout);
  cbh_existloop(lofig,loop,namein);
  cct = cbh_initcct(lofig,nbinout[CBH_IN],nbinout[CBH_OUT],loop,0);
  cbh_addincct(cct,inout[CBH_IN],loop,namein);
  cbh_createabllist(lofig,CBH_SEQ,mainabllist,inout[CBH_OUT]);
  for (i = 0; i < CBH_MAXNBFUNC; i ++)
  {
    abllist = mainabllist[i];

    if (abllist)
      for (chainx = abllist; chainx; chainx = chainx->NEXT)
        /* ici les bdd doivent etre calcule pour chaque sorties de reflofig*/
        if (CAR(chainx))
        {
          name = VALUE_ATOM(CAR(chainx));
          abl  = CDAR(chainx);
          cbh_addoutcct(cct,ablToBddCct(cct,abl),name,i);
        }
  }
  cbh_freeabllist(mainabllist);
  cbh_freeInOutList(inout);
  cbh_addccttolofig(lofig,cct);
  return cct;
}

/*}}}************************************************************************/
/*{{{                    cbh_delcombtolofig()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_delcombtolofig(lofig)
lofig_list  *lofig;
{
  locon_list    *locon;

  for (locon = lofig->LOCON; locon; locon = locon->NEXT)
    cbh_delcombtolocon(locon);
}

/*}}}************************************************************************/
/*{{{                    cbh_freeLofig()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_freeLofig(lofig)
lofig_list  *lofig;
{
  cbh_delcombtolofig(lofig);
  cbh_delseqtolofig(lofig);
  cbh_delccttolofig(lofig);
  dellofig(lofig->NAME);
}

/*}}}************************************************************************/
/*{{{                    cbh_freechain2()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_freechain2(chain,mask)
chain_list  *chain;
int         mask;
{
  int       bdd = mask&~CBH_MASKBDD;

  if (!chain)
    return;
  if ((CBH_TRACE)||(mask&CBH_DISPLAY))
    printf("\t\t\t\t- %s\n",((lofig_list*)chain->DATA)->NAME);
  if (mask&CBH_FREECLASS)
  {
    unlocklofig((lofig_list*)chain->DATA);
    cbh_freeLofig((lofig_list*)chain->DATA);
    chain = delchain(chain,chain);
    cbh_freechain2(chain,mask);
  }
  if (bdd == CBH_DELBDD)
  {
    cbh_freechain2(chain->NEXT,mask);
    cbh_delccttolofig((lofig_list*)chain->DATA);
  }
  else if (bdd == CBH_DOBDD)
    cbh_docct((lofig_list*)chain->DATA);
}

/*}}}************************************************************************/
/*{{{             cbh_traversetree                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_traversetree(branch,n,mask)
chain_list  *branch;
int         n;
int         mask;
{
  chain_list    *chainx;
  int           i;
  int           type = mask&~CBH_MASKTYPE;

  if (!n)
  {
    if (branch)
      for (chainx = branch, i = 0; chainx; i ++)
      {
        CBH_PRINT("\t\t\t-func %d\n",i);
        cbh_freechain2(chainx->DATA,mask);
        if (mask&CBH_FREECLASS)
          chainx = delchain(chainx,chainx);
        else
          chainx = chainx->NEXT;
      }
  }
  else
    for (i = 0, chainx = branch; chainx; i ++)
    {
      if ((CBH_TRACE) || (mask&CBH_DISPLAY))
      {
        if (n == 1)
          printf("\t\t-out %d\n",i);
        else if (n == 2)
          printf("\t-in %d\n",i);
        else if (type == CBH_COMB)
          printf("-comb\n");
        else if (type == CBH_NOTYPE)
          printf("-notype\n");
        else if (type == CBH_SEQ)
        {
          if (i == 0)
            printf("-seq latch\n");
          else
            printf("-seq flipflop\n");
        }
      }
      cbh_traversetree(chainx->DATA,n-1,mask);
      if (mask&CBH_FREECLASS)
        chainx = delchain(chainx,chainx);
      else
        chainx = chainx->NEXT;
    }
}

/*}}}************************************************************************/
/*{{{             cbh_freechain                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_freechain(chain)
chain_list  *chain;
{
  if (!chain)
    return;
  else
    cbh_freechain(chain->NEXT);
  delchain(chain,chain);
}

/*}}}************************************************************************/
/*{{{             cbh_getLoconFromLofig()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_getLoconFromLofig(lofig,inout,nbinout)
lofig_list  *lofig;
chain_list  *inout[2];
int          nbinout[2];
{
  cbh_getLocon(lofig,inout,nbinout,CBH_LOFIG);
}

/*}}}************************************************************************/
/*{{{             cbh_getLoconFromLoins()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_getLoconFromLoins(loins,inout,nbinout)
loins_list  *loins;
chain_list  *inout[2];
int          nbinout[2];
{
  cbh_getLocon(loins,inout,nbinout,CBH_LOINS);
}

/*}}}************************************************************************/
/*{{{             cbh_getLocon()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_getLocon(lo_figins,inout,nbinout,type)
void        *lo_figins;
chain_list  *inout[2];
int          nbinout[2];
int          type;
{
  locon_list    *loconlist;

  if (!lo_figins)
    return;

  if (inout)
    inout[CBH_IN]    = NULL;
  if (inout)
    inout[CBH_OUT]   = NULL;
  if (nbinout)
    nbinout[CBH_IN]  = 0;
  if (nbinout)
    nbinout[CBH_OUT] = 0;

  switch (type)
  {
    case CBH_LOFIG :
         if (!(loconlist = ((lofig_list*)lo_figins)->LOCON))
           return;
         break;
    case CBH_LOINS :
         if (!(loconlist = ((loins_list*)lo_figins)->LOCON))
           return;
         break;
    default :
         return;
  }

  for (;loconlist;loconlist=loconlist->NEXT)
    switch (loconlist->DIRECTION)
    {
      case 'I' :
           if (nbinout)
             nbinout[CBH_IN]++;
           if (inout)
             inout[CBH_IN]  = addchain(inout[CBH_IN],loconlist);
           break;
      case 'O' : case 'Z' :
           if (nbinout)
             nbinout[CBH_OUT]++;
           if (inout)
             inout[CBH_OUT] = addchain(inout[CBH_OUT],loconlist);
           break;
      case 'T' : case 'B' :
           if (nbinout)
             nbinout[CBH_IN]++;
           if (inout)
             inout[CBH_IN]  = addchain(inout[CBH_IN],loconlist);
           if (nbinout)
             nbinout[CBH_OUT]++;
           if (inout)
             inout[CBH_OUT] = addchain(inout[CBH_OUT],loconlist);
           break;
           //  case 'X' : ;
    }
}

/*}}}************************************************************************/
/*{{{             cbh_freeInOutList()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_freeInOutList(inout)
chain_list  *inout[2];
{
  freechain(inout[CBH_IN]);
  freechain(inout[CBH_OUT]);
}

/*}}}************************************************************************/
/*{{{             cbh_behtocbh                                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
lofig_list *cbh_behtocbh(befig)
befig_list *befig ;
{
  lofig_list    *lofig ;
  locon_list    *locon ;
  losig_list    *losig ;
  cbhseq        *cbhseq ;
  cbhcomb       *cbhcomb;
  bepor_list    *ptbepor;
  int            index = 1 ;

  if((lofig = getloadedlofig(befig->NAME)) == NULL)
  {
    lofig = addlofig(befig->NAME) ;
    for (ptbepor = befig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT)
    {
      losig = addlosig(lofig,index++,addchain(NULL,ptbepor->NAME),EXTERNAL) ;
      addlocon(lofig,ptbepor->NAME,losig,ptbepor->DIRECTION) ;
    }
  }

  cbhseq = cbh_getseqfunc(befig,CBH_LIB_MODE) ;

  cbh_addseqtolofig(lofig,cbhseq) ;

  for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
    switch (locon->DIRECTION)
      case OUT : case INOUT : case TRISTATE : case TRANSCV :
           cbhcomb = cbh_getcombfunc(befig,cbhseq,locon->NAME);
           cbh_addcombtolocon(locon,cbhcomb);
  }
  return(lofig) ;
}


/*}}}************************************************************************/
/*{{{             cbh_befiglisttocbhlist                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_befiglisttocbhlist(befig)
befig_list *befig ;
{
  lofig_list *lofig ;

  cbh_newclassifier() ;

  while (befig)
  {
    lofig    = cbh_behtocbh(befig) ;
    cbh_classlofig(lofig) ;
    befig    = beh_delbefig (befig, befig, 'Y') ;
    befig    = befig->NEXT ;
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Lofig extension to Net-list                        */
/* written by anto february,the 11 th 2003                                  */
/*                                                                          */
/* purpose : use simulation on hierarchical net-list                        */
/****************************************************************************/
/*{{{                    cbh_applyBefToLof()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void cbh_applyBefToLof(lofig_list *lofig, befig_list *befig)
{
  cbhseq            *seq    = NULL, *seqx;
  befig_list        *copy;
  locon_list        *svlocon;
  bereg_list        *regx, *svreg;
  bebux_list        *buxx;

  copy          = beh_duplicate(befig);
  beh_biterize(copy);
  cbh_suppressaux(copy);
  // single register treatment
  for ( regx = copy->BEREG; regx; regx = regx->NEXT)
  {
    seqx        = cbh_newseq();
    seqx->NAME  = regx->NAME;
    seqx->NEXT  = seq;
    seq         = seqx;
  }
  // single bux treatment
  for ( buxx = copy->BEBUX; buxx; buxx = buxx->NEXT)
  {
    seqx        = cbh_newseq();
    seqx->NAME  = buxx->NAME;
    seqx->NEXT  = seq;
    seq         = seqx;
  }
  cbh_addseqtolofig(lofig,seq);

  svlocon       = lofig->LOCON;
  for (seqx = seq; seqx; seqx = seqx->NEXT)
    addlocon(lofig,seqx->NAME,NULL,'B') ;
  addptype(lofig->USER,CBH_TYPE_VLOCON,lofig->LOCON);
  lofig->LOCON  = svlocon;

  svreg         = copy->BEREG;
  copy->BEREG   = NULL;
  copy->NAME    = lofig->NAME;
  
  cbh_behtocbh(copy);
  
  copy->BEREG   = svreg;
  beh_frebefig(copy);
}

/*}}}************************************************************************/
/*{{{                    cbh_cbh2beh()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
befig_list *cbh_cbh2beh(lofig_list *lofig)
{
  befig_list    *res;
  locon_list    *loconx;
  cbhcomb       *comb;
  cbhseq        *seq;
  biabl_list    *biablx;
  ht            *dict;
  int            isBus;
  char           buf[1024], *master;
  chain_list    *one, *zero, *hz, *conflict, *abl;

  dict      = addht(10);
  isBus     = 0;
  one       = createBitStr("'1'");
  zero      = createBitStr("'0'");
  hz        = createBitStr("'z'");
  conflict  = createBitStr("'x'");
  
  beh_addbefig(NULL,lofig->NAME);
  
  for (loconx = lofig->LOCON; loconx; loconx = loconx->NEXT)
    res->BEPOR=beh_addbepor(res->BEPOR,loconx->NAME,loconx->DIRECTION,0);
  
  for (loconx = lofig->LOCON; loconx; loconx = loconx->NEXT)
  {
    addhtitem(dict,loconx->NAME,0);
    comb    = cbh_getcombfromlocon(loconx);
    biablx  = NULL;
    if (comb->CONFLICT)
    {
      isBus     = 1;
      if (biablx)
        biablx->FLAG |= BEH_CND_PRECEDE;
      biablx  = beh_addbiabl(biablx,NULL,comb->CONFLICT,conflict);
    }
    if (comb->HZFUNC)
    {
      isBus     = 1;
      if (biablx)
        biablx->FLAG |= BEH_CND_PRECEDE;
      biablx  = beh_addbiabl(biablx,NULL,comb->CONFLICT,hz);
    }
    if (isBus)
    {
      if (comb->FUNCTION)
        biablx  = beh_addbiabl(biablx,NULL,comb->FUNCTION,one);
      res->BEBUS=beh_addbebus(res->BEBUS,loconx->NAME,biablx,NULL,0,0);
    }
    else
      res->BEOUT=beh_addbeout(res->BEOUT,loconx->NAME,comb->FUNCTION,0,0);
  }
  
  if ( (seq = cbh_getseqfromlofig(lofig)) )
  {
    res->BEAUX=beh_addbeaux(res->BEAUX,seq->NEGNAME,notExpr(createAtom(seq->NAME)),0,0);

    abl    = seq->DATA;
    biablx = NULL;
    biablx = (seq->CLOCK) ? beh_addbiabl(biablx,NULL,seq->CLOCK,abl)  : biablx;
    biablx = (seq->SET)   ? beh_addbiabl(biablx,NULL,seq->SET,one)    : biablx;
    biablx = (seq->RESET) ? beh_addbiabl(biablx,NULL,seq->RESET,zero) : biablx;
    
    if (seq->SLAVECLOCK)
    {
      sprintf(buf,"%s_master",seq->NAME);
      master  = namealloc(buf);
      if ( gethtitem(dict,master) )
      {
        printf("erreur\n");
        EXIT(0);
      }
      res->BEREG=beh_addbereg(res->BEREG,master,biablx,0,0);
      biablx  = NULL;
      biablx  = beh_addbiabl(biablx,NULL,seq->SLAVECLOCK,createAtom(master));
      res->BEREG=beh_addbereg(res->BEREG,seq->NAME,biablx,0,0);
    }
    else
      res->BEREG=beh_addbereg(res->BEREG,seq->NAME,biablx,0,0);
  }
  
  return res;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
