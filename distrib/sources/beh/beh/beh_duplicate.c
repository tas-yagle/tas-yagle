/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEHVECT Version 1.00                                        */
/*    Fichier : beh_dup.c                                                   */
/*                                                                          */
/*    (c) copyright 2003 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include MUT_H
#include LOG_H
#include EQT_H
#include BEH_H

#define XOU(a,b)        (!(a) && (b)) || ((a) && !(b))

static chain_list       *BEH_EXP_LIST   = NULL;
static ht               *BEH_EXP_TABLE  = NULL;

//static vectbiabl_list   *beh_explodeVectBiabl   (vectbiabl_list *vbiabl,char *name);
//static vectbiabl_list   *beh_explodeVect        (vectbiabl_list *vbiabl,char *name);
       biabl_list       *beh_explodeBiabl       (biabl_list *biabl,int index, char *name,int left, int right,void (*func)(char*,int,biabl_list*));
static void              beh_explodeLoopInt     (eqt_ctx *ctx, loop_list *loop, biabl_list *biabl, char *name,void (*func)(char*,int,biabl_list*));
       void              beh_explodeLoop        (loop_list *loop, biabl_list *biabl,char *name,void (*func)(char*,int,biabl_list*));
static chain_list       *beh_explodeLoopAbl     (eqt_ctx *ctx, chain_list *abl);

       befig_list       *beh_duplicate          (befig_list *befig);

static bereg_list       *beh_dupreg             (bereg_list *reg);
static bevectreg_list   *beh_dupvreg            (bevectreg_list *vreg);
static bebus_list       *beh_dupbus             (bebus_list *bus);
static bevectbus_list   *beh_dupvbus            (bevectbus_list *vbus);
static bebux_list       *beh_dupbux             (bebux_list *bux);
static bevectbux_list   *beh_dupvbux            (bevectbux_list *vbux);
static beaux_list       *beh_dupaux             (beaux_list *aux);
static bevectaux_list   *beh_dupvaux            (bevectaux_list *vaux);
static beout_list       *beh_dupout             (beout_list *out);
static bevectout_list   *beh_dupvout            (bevectout_list *vout);
static bepor_list       *beh_duppor             (bepor_list *por);
static bevectpor_list   *beh_dupvpor            (bevectpor_list *vpor);
static berin_list       *beh_duprin             (berin_list *rin);
static bemsg_list       *beh_dupmsg             (bemsg_list *msg);

static vectbiabl_list   *beh_dupvbiabl          (vectbiabl_list *vbiabl);
static loop_list        *beh_duploop            (loop_list *loop);

static berin_list       *beh_bitrzrin           (berin_list *rin);
static bereg_list       *beh_bitrzreg           (bereg_list *reg);
static bebux_list       *beh_bitrzbux           (bebux_list *bux);
static bebus_list       *beh_bitrzbus           (bebus_list *bus);
static bemsg_list       *beh_bitrzmsg           (bemsg_list *msg);
static bereg_list       *beh_bitrzvreg          (bevectreg_list *vreg,bereg_list *reg);
static bebux_list       *beh_bitrzvbux          (bevectbux_list *vbux,bebux_list *bux);
static bebus_list       *beh_bitrzvbus          (bevectbus_list *vbus,bebus_list *bus);
static beaux_list       *beh_bitrzvaux          (bevectaux_list *vaux,beaux_list *aux);
static beout_list       *beh_bitrzvout          (bevectout_list *vout,beout_list *out);
static bepor_list       *beh_bitrzvpor          (bevectpor_list *vpor,bepor_list *por);

static int beh_explodews(biabl_list *biabl);
static chain_list *beh_bitrzcndws(chain_list *cnd, char *bitstr);
static void beh_bitrzcnd(chain_list *cnd);
static void beh_explodec(biabl_list *biabl);

static void beh_resolvingfunc(char *name,int index,biabl_list *biabl);

static bereg_list *beh_addloopreg(bereg_list *reg);
static bebus_list *beh_addloopbus(bebus_list *bus);
static bebux_list *beh_addloopbux(bebux_list *bux);
static biabl_list *beh_expgetbiabl(chain_list *list);

/****************************************************************************/
/*{{{                    Biterize functions                                 */
/****************************************************************************/
/*{{{                    beh_biterize()                                     */
/*                                                                          */
/* suppress vectorisation, loop and with select                             */
/*                                                                          */
/****************************************************************************/
void beh_biterize(befig_list *befig)
{
  BEH_EXP_TABLE     = addht(100);
  
  befig->BEREG      = beh_bitrzreg     (befig->BEREG);
  befig->BEBUX      = beh_bitrzbux     (befig->BEBUX);
  befig->BEBUS      = beh_bitrzbus     (befig->BEBUS);
  befig->BEAUX      =                   befig->BEAUX ;
  befig->BEOUT      =                   befig->BEOUT ;
  befig->BEPOR      =                   befig->BEPOR ;
  
  befig->BEREG      = beh_bitrzvreg    (befig->BEVECTREG,befig->BEREG);
  befig->BEVECTREG  = beh_delbevectreg (befig->BEVECTREG,befig->BEVECTREG,'Y');
  befig->BEBUX      = beh_bitrzvbux    (befig->BEVECTBUX,befig->BEBUX);
  befig->BEVECTBUX  = beh_delbevectbux (befig->BEVECTBUX,befig->BEVECTBUX,'Y');
  befig->BEBUS      = beh_bitrzvbus    (befig->BEVECTBUS,befig->BEBUS);
  befig->BEVECTBUS  = beh_delbevectbus (befig->BEVECTBUS,befig->BEVECTBUS,'Y');
  befig->BEAUX      = beh_bitrzvaux    (befig->BEVECTAUX,befig->BEAUX);
  befig->BEVECTAUX  = beh_delbevectaux (befig->BEVECTAUX,befig->BEVECTAUX,'Y');
  befig->BEOUT      = beh_bitrzvout    (befig->BEVECTOUT,befig->BEOUT);
  befig->BEVECTOUT  = beh_delbevectout (befig->BEVECTOUT,befig->BEVECTOUT,'Y');
  befig->BEPOR      = beh_bitrzvpor    (befig->BEVECTPOR,befig->BEPOR);
  befig->BEVECTPOR  = beh_delbevectpor (befig->BEVECTPOR,befig->BEVECTPOR);

  befig->BERIN      = beh_bitrzrin     (befig->BERIN);
  befig->BEMSG      = beh_bitrzmsg     (befig->BEMSG);

  delht(BEH_EXP_TABLE);
  BEH_EXP_TABLE     = NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzrin()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static berin_list *beh_bitrzrin(berin_list *rin)
{
  if (rin)
  {
    int          min, max, i;
    char         buf[1024];

    rin->NEXT   = beh_bitrzrin(rin->NEXT);
    min         = (rin->LEFT >= rin->RIGHT) ? rin->RIGHT : rin->LEFT  ;
    max         = (rin->LEFT >= rin->RIGHT) ? rin->LEFT  : rin->RIGHT ;
    for (i = min; i <= max; i ++ )
    {
      sprintf(buf,"%s %d",rin->NAME,i);
      rin->NEXT = beh_addberin(rin->NEXT,namealloc(buf));
    }

    return beh_delberin(rin,rin);
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzmsg()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bemsg_list *beh_bitrzmsg(bemsg_list *msg)
{
  if (msg)
  {
    int          max, i;

    msg->NEXT   = beh_bitrzmsg(msg->NEXT);
    max         = verifyVectAbl(msg->ABL);
    for (i = 0; i < max; i ++ )
      msg->NEXT = beh_addbemsg(msg->NEXT,msg->LABEL,msg->LEVEL,msg->MESSAGE,
                               getAblAtPos(msg->ABL,i),NULL);

    return beh_delbemsg(msg,msg,'Y');
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzvpor()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bepor_list *beh_bitrzvpor(bevectpor_list *vpor,bepor_list *por)
{
  if (vpor)
  {
    int         i, way;
    char        buf[1024];

    way         = (vpor->LEFT > vpor->RIGHT);
    por         = beh_bitrzvpor(vpor->NEXT,por);
    vpor->NEXT  = beh_delbevectpor(vpor->NEXT,vpor->NEXT);
    for (i = vpor->LEFT; XOU(way, i <= vpor->RIGHT-way ); (way) ? i-- : i++ )
    {
      sprintf(buf,"%s %d",vpor->NAME,i);
      por       = beh_addbepor(por,namealloc(buf),vpor->DIRECTION,vpor->TYPE);
    }
    return por;
  }
  else
    return por;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzreg()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bereg_list *beh_bitrzreg(bereg_list *reg)
{
  if (reg)
  {
    beh_bitrzreg(reg->NEXT);
    beh_explodec(reg->BIABL);
  }
  return reg;
}
/*}}}************************************************************************/
/*{{{                    beh_bitrzbux()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebux_list *beh_bitrzbux(bebux_list *bux)
{
  if (bux)
  {
    beh_bitrzbux(bux->NEXT);
    if (!beh_explodews(bux->BIABL))
      beh_explodec(bux->BIABL);
  }
  return bux;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzbus()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebus_list *beh_bitrzbus(bebus_list *bus)
{
  if (bus)
  {
    beh_bitrzbus(bus->NEXT);
    if (!beh_explodews(bus->BIABL))
      beh_explodec(bus->BIABL);
  }
  return bus;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzvreg()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bereg_list *beh_bitrzvreg(bevectreg_list *vreg,bereg_list *reg)
{
  if (vreg)
  {
    vectbiabl_list  *vbiablx;
    biabl_list      *biablx;
    int              i, way, cnddone;
    char             buf[1024];

    reg             = beh_bitrzvreg(vreg->NEXT,reg);
    vreg->NEXT      = beh_delbevectreg(vreg->NEXT,vreg->NEXT,'Y');
    while ( vreg->VECTBIABL)
    {
      vbiablx = vreg->VECTBIABL;
      cnddone = beh_explodews(vbiablx->BIABL);
      way     = (vbiablx->LEFT > vbiablx->RIGHT);
      for ( i = vbiablx->LEFT; XOU(way, i <= vbiablx->RIGHT-way);
            (way) ? i -- : i ++ )
      {
        biablx = beh_explodeBiabl(vbiablx->BIABL,i,vreg->NAME,vbiablx->LEFT,
                                  vbiablx->RIGHT,beh_resolvingfunc);
        if (biablx)
        {
          sprintf(buf,"%s %d",vreg->NAME,i);
          reg  = beh_addbereg(reg,namealloc(buf),biablx,NULL,vreg->FLAGS);
          if (!cnddone)
            beh_explodec(reg->BIABL);
        }
        else
        {
          reg = beh_addloopreg(reg);
          break;
        }
      }
      vreg->VECTBIABL = beh_delvectbiabl(vbiablx,vbiablx,'Y');
    }
    return reg;
  }
  else
    return reg;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzvbus()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebus_list *beh_bitrzvbus(bevectbus_list *vbus,bebus_list *bus)
{
  if (vbus)
  {
    vectbiabl_list  *vbiablx;
    int              i, way, cnddone;
    char             buf[1024];
    biabl_list      *biablx;

    bus             = beh_bitrzvbus(vbus->NEXT,bus);
    vbus->NEXT      = beh_delbevectbus(vbus->NEXT,vbus->NEXT,'Y');
    while ( vbus->VECTBIABL)
    {
      vbiablx = vbus->VECTBIABL;
      cnddone = beh_explodews(vbiablx->BIABL);
      way     = (vbiablx->LEFT > vbiablx->RIGHT);
      for ( i = vbiablx->LEFT; XOU(way, i <= vbiablx->RIGHT-way);
            (way) ? i -- : i ++ )
      {
        biablx = beh_explodeBiabl(vbiablx->BIABL,i,vbus->NAME,vbiablx->LEFT,
                                  vbiablx->RIGHT,beh_resolvingfunc);
        if (biablx)
        {
          sprintf(buf,"%s %d",vbus->NAME,i);
          bus  = beh_addbebus(bus,namealloc(buf),biablx,NULL,0,vbus->FLAGS);
          if (!cnddone)
            beh_explodec(bus->BIABL);
        }
        else
        {
          bus = beh_addloopbus(bus);
          break;
        }
      }
      vbus->VECTBIABL = beh_delvectbiabl(vbiablx,vbiablx,'Y');
    }
    return bus;
  }
  else
    return bus;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzvbux()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebux_list *beh_bitrzvbux(bevectbux_list *vbux,bebux_list *bux)
{
  if (vbux)
  {
    vectbiabl_list  *vbiablx;
    int              i, way, cnddone;
    char             buf[1024];
    biabl_list      *biablx;

    bux             = beh_bitrzvbux(vbux->NEXT,bux);
    vbux->NEXT      = beh_delbevectbux(vbux->NEXT,vbux->NEXT,'Y');
    while ( vbux->VECTBIABL)
    {
      vbiablx = vbux->VECTBIABL;
      cnddone = beh_explodews(vbiablx->BIABL);
      way     = (vbiablx->LEFT > vbiablx->RIGHT);
      for ( i = vbiablx->LEFT; XOU(way, i <= vbiablx->RIGHT-way);
            (way) ? i -- : i ++ )
      {
        biablx = beh_explodeBiabl(vbiablx->BIABL,i,vbux->NAME,vbiablx->LEFT,
                                  vbiablx->RIGHT,beh_resolvingfunc);
        if (biablx)
        {
          sprintf(buf,"%s %d",vbux->NAME,i);
          bux  = beh_addbebux(bux,namealloc(buf),biablx,NULL,0,vbux->FLAGS);
          if (!cnddone)
            beh_explodec(bux->BIABL);
        }
        else
        {
          bux = beh_addloopbux(bux);
          break;
        }
      }
      vbux->VECTBIABL = beh_delvectbiabl(vbiablx,vbiablx,'Y');
    }
    return bux;
  }
  else
    return bux;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzvaux()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beaux_list *beh_bitrzvaux(bevectaux_list *vaux,beaux_list *aux)
{
  if (vaux)
  {
    int          i, way;
    char         buf[1024];
    chain_list  *abl;

    way         = (vaux->LEFT > vaux->RIGHT);
    aux         = beh_bitrzvaux(vaux->NEXT,aux);
    vaux->NEXT  = beh_delbevectaux(vaux->NEXT,vaux->NEXT,'Y');
    for (i = vaux->LEFT; XOU(way, i <= vaux->RIGHT-way); (way) ? i-- : i++ )
    {
      sprintf(buf,"%s %d",vaux->NAME,i);
      abl       = getAblAtIndex(vaux->ABL,vaux->LEFT,vaux->RIGHT,i);
      aux       = beh_addbeaux(aux,namealloc(buf),abl,NULL,aux->FLAGS);
      aux->TIME = vaux->TIME;
      aux->TIMEVAR = vaux->TIMEVAR;
    }
    return aux;
  }
  return aux;
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzvout()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beout_list *beh_bitrzvout(bevectout_list *vout,beout_list *out)
{
  if (vout)
  {
    int          i, way;
    char         buf[1024];
    chain_list  *abl;

    way         = (vout->LEFT > vout->RIGHT);
    out         = beh_bitrzvout(vout->NEXT,out);
    vout->NEXT  = beh_delbevectout(vout->NEXT,vout->NEXT,'Y');
    for (i = vout->LEFT; XOU(way, i <= vout->RIGHT-way); (way) ? i-- : i++ )
    {
      sprintf(buf,"%s %d",vout->NAME,i);
      abl       = getAblAtIndex(vout->ABL,vout->LEFT,vout->RIGHT,i);
      out       = beh_addbeout(out,namealloc(buf),abl,NULL,out->FLAGS);
      out->TIME = vout->TIME;
      out->TIMEVAR = vout->TIMEVAR;
    }
    return out;
  }
  return out;
}

/*}}}************************************************************************/
/*{{{                    beh_explodews()                                    */
/*                                                                          */
/* destroy with select                                                      */
/*                                                                          */
/****************************************************************************/
static int beh_explodews(biabl_list *biabl)
{
  if (biabl && biabl->FLAG & BEH_CND_SELECT)
  {
    biabl_list  *biablx;
    chain_list  *abl;
    long         flag;

    for (biablx = biabl; biablx; biablx = biablx->NEXT)
    {
      if (biablx->FLAG & BEH_CND_SELECT)
      {
        abl  = beh_bitrzcndws(biablx->CNDABL,beh_getcndvectfbiabl(biablx));
        flag = (biablx->FLAG | BEH_CND_PRECEDE) & ~BEH_CND_SELECT;
      }
      else if (biablx->FLAG & BEH_CND_SEL_OTHERS)
      {
        abl  = createBitStr("1");
        flag = (biablx->FLAG | BEH_CND_PRECEDE) & ~BEH_CND_SEL_OTHERS;
      }
      else
        beh_toolbug(20,"beh_explodews",NULL,0);
      beh_delcndvectfbiabl(biabl);
      biablx->CNDABL = abl;
      biablx->FLAG   = flag;
    }
    return 1;
  }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    beh_explodec()                                     */
/*                                                                          */
/* destroy vectorized conditions                                            */
/*                                                                          */
/****************************************************************************/
static void beh_explodec(biabl_list *biabl)
{
  if (biabl)
  {
    biabl_list  *biablx;

    for (biablx = biabl; biablx; biablx = biablx->NEXT)
      beh_bitrzcnd(biablx->CNDABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzcnd()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beh_bitrzcnd(chain_list *cnd)
{
  if (!ATOM(cnd))
  {
    switch (OPER(cnd))
    {
      case CAT :
           OPER(cnd)    = AND;
           VECT_S(cnd)  = 1L;
    }
    while ((cnd = CDR(cnd)))
      beh_bitrzcnd(CAR(cnd));
  }
}

/*}}}************************************************************************/
/*{{{                    beh_bitrzcndws()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *beh_bitrzcndws(chain_list *cnd, char *bitstr)
{
  int            i, max;
  chain_list    *res, *tmp;

  res   = createExpr(AND);
  max   = verifyVectAbl(cnd);
  for (i = 0; i < max; i ++)
  {
    tmp = getAblAtPos(cnd,i);
    if (bitstr)
      switch (*(bitstr+1+i))
      {
        case '0' :
             tmp  = notExpr(tmp);
        case '1' :
             addHExpr(res,tmp);
      }
    else
      addHExpr(res,tmp);
  }
  freeExpr(cnd);

  return res;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Duplication functions                              */
/****************************************************************************/
/*{{{                    beh_duplicate()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
befig_list *beh_duplicate(befig_list *befig)
{
  befig_list    *res;

  res               = beh_addbefig(NULL,befig->NAME);
  res->BEREG        = beh_dupreg  (befig->BEREG);
  res->BEBUX        = beh_dupbux  (befig->BEBUX);
  res->BEBUS        = beh_dupbus  (befig->BEBUS);
  res->BEAUX        = beh_dupaux  (befig->BEAUX);
  res->BEOUT        = beh_dupout  (befig->BEOUT);
  res->BEPOR        = beh_duppor  (befig->BEPOR);
  res->BEVECTREG    = beh_dupvreg (befig->BEVECTREG);
  res->BEVECTBUX    = beh_dupvbux (befig->BEVECTBUX);
  res->BEVECTBUS    = beh_dupvbus (befig->BEVECTBUS);
  res->BEVECTAUX    = beh_dupvaux (befig->BEVECTAUX);
  res->BEVECTOUT    = beh_dupvout (befig->BEVECTOUT);
  res->BEVECTPOR    = beh_dupvpor (befig->BEVECTPOR);

  res->BERIN        = beh_duprin  (befig->BERIN);
  res->BEMSG        = beh_dupmsg  (befig->BEMSG);

  return res;
}

/*}}}************************************************************************/
/*{{{                    beh_dupbiabl()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list *beh_dupbiabl(biabl_list *biabl)
{
  if (biabl)
  {
    biabl_list  *new;

    new         = beh_addbiabl(NULL,biabl->LABEL,NULL,NULL);
    new->TIME   = biabl->TIME;
    new->TIMER   = biabl->TIMER;
    new->TIMEF   = biabl->TIMEF;
    new->TIMEVAR   = biabl->TIMEVAR;
    new->FLAG   = biabl->FLAG;
    new->CNDABL = copyExpr    (biabl->CNDABL);
    new->VALABL = copyExpr    (biabl->VALABL);
    new->LOOP   = beh_duploop (biabl->LOOP);
    new->NEXT   = beh_dupbiabl(biabl->NEXT);
    new->USER   = dupptypelst (biabl->USER);
    
    return new;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvbiabl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
vectbiabl_list *beh_dupvbiabl(vectbiabl_list *vbiabl)
{
  if (vbiabl)
  {
    vectbiabl_list   *new;

    new        = beh_addvectbiabl(NULL,vbiabl->LEFT,vbiabl->RIGHT,NULL);
    new->BIABL = beh_dupbiabl    (vbiabl->BIABL);
    new->NEXT  = beh_dupvbiabl   (vbiabl->NEXT);
    
    return new;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_duploop()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
loop_list *beh_duploop(loop_list *loop)
{
  if (loop)
  {
    loop_list   *res;

    res = beh_duploop(loop->NEXT);
    res = beh_newloop(res,loop->VARNAME,loop->INDEX,loop->LEFT,loop->RIGHT);
    
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupreg()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bereg_list *beh_dupreg(bereg_list *reg)
{
  if (reg)
    return beh_addbereg(beh_dupreg(reg->NEXT),reg->NAME,
                        beh_dupbiabl(reg->BIABL),NULL,reg->FLAGS);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvreg()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bevectreg_list *beh_dupvreg(bevectreg_list *vreg)
{
  if (vreg)
    return beh_addbevectreg(beh_dupvreg(vreg->NEXT),vreg->NAME,
                            beh_dupvbiabl(vreg->VECTBIABL),
                            vreg->LEFT,vreg->RIGHT,vreg->FLAGS);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupbus()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebus_list *beh_dupbus(bebus_list *bus)
{
  if (bus)
    return beh_addbebus(beh_dupbus(bus->NEXT),bus->NAME,
                        beh_dupbiabl(bus->BIABL),NULL,bus->TYPE,bus->FLAGS);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvbus()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bevectbus_list *beh_dupvbus(bevectbus_list *vbus)
{
  if (vbus)
    return beh_addbevectbus(beh_dupvbus(vbus->NEXT),vbus->NAME,
                            beh_dupvbiabl(vbus->VECTBIABL),
                            vbus->LEFT,vbus->RIGHT,vbus->TYPE,vbus->FLAGS);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupbux()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebux_list *beh_dupbux(bebux_list *bux)
{
  if (bux)
    return beh_addbebux(beh_dupbux(bux->NEXT),bux->NAME,
                        beh_dupbiabl(bux->BIABL),NULL,bux->TYPE,bux->FLAGS);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvbux()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bevectbux_list *beh_dupvbux(bevectbux_list *vbux)
{
  if (vbux)
    return beh_addbevectbux(beh_dupvbux(vbux->NEXT),vbux->NAME,
                            beh_dupvbiabl(vbux->VECTBIABL),
                            vbux->LEFT,vbux->RIGHT,vbux->TYPE,vbux->FLAGS);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupaux()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beaux_list *beh_dupaux(beaux_list *aux)
{
  if (aux)
  {
    beaux_list  *res;

    res         = beh_addbeaux(beh_dupaux(aux->NEXT),aux->NAME,
                               copyExpr(aux->ABL),NULL,aux->FLAGS);
    res->TIME   = aux->TIME;
    res->TIMER   = aux->TIMER;
    res->TIMEF   = aux->TIMEF;
    res->TIMEVAR   = aux->TIMEVAR;
    
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvaux()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bevectaux_list *beh_dupvaux(bevectaux_list *vaux)
{
  if (vaux)
  {
    bevectaux_list  *res;

    res         = beh_addbevectaux(beh_dupvaux(vaux->NEXT),vaux->NAME,
                                   copyExpr(vaux->ABL),vaux->LEFT,
                                   vaux->RIGHT, vaux->FLAGS);
    res->TIME   = vaux->TIME;
    res->TIMEVAR   = vaux->TIMEVAR;
    
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupout()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beout_list *beh_dupout(beout_list *out)
{
  if (out)
  {
    beout_list  *res;

    res         = beh_addbeout(beh_dupout(out->NEXT),out->NAME,
                               copyExpr(out->ABL),NULL,out->FLAGS);
    res->TIME   = out->TIME;
    res->TIMER   = out->TIMER;
    res->TIMEF   = out->TIMEF;
    res->TIMEVAR   = out->TIMEVAR;
    
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvout()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bevectout_list *beh_dupvout(bevectout_list *vout)
{
  if (vout)
  {
    bevectout_list  *res;

    res         = beh_addbevectout(beh_dupvout(vout->NEXT),vout->NAME,
                                   copyExpr(vout->ABL),vout->LEFT,
                                   vout->RIGHT, vout->FLAGS);
    res->TIME   = vout->TIME;
    res->TIMEVAR   = vout->TIMEVAR;
    
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_duppor()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bepor_list *beh_duppor(bepor_list *por)
{
  if (por)
    return beh_addbepor(beh_duppor(por->NEXT),por->NAME,por->DIRECTION,
                        por->TYPE);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupvpor()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bevectpor_list *beh_dupvpor(bevectpor_list *vpor)
{
  if (vpor)
    return beh_addbevectpor( beh_dupvpor(vpor->NEXT),vpor->NAME,
                             vpor->DIRECTION,vpor->TYPE,
                             vpor->LEFT,vpor->RIGHT);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_duprin()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static berin_list *beh_duprin(berin_list *rin)
{
  if (rin)
  {
    berin_list  *res;

    res             = beh_duprin(rin->NEXT);
    if (!(rin->OUT_REF || rin->OUT_VEC || rin->BUS_REF || rin->BUS_VEC ||
          rin->AUX_REF || rin->AUX_VEC || rin->REG_REF || rin->REG_VEC ||
          rin->BUX_REF || rin->BUX_VEC))
    {
      res           = beh_addberin(res,rin->NAME);
      res->LEFT     = rin->LEFT;
      res->RIGHT    = rin->RIGHT;
    }
    
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_dupmsg()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bemsg_list *beh_dupmsg(bemsg_list *msg)
{
  if (msg)
    return beh_addbemsg(beh_dupmsg(msg->NEXT),msg->LABEL,msg->LEVEL,
                        msg->MESSAGE,copyExpr(msg->ABL),NULL);
  else
    return NULL;

}
/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Explosion functions                                */
/****************************************************************************/
///*{{{                    beh_bitrzbiabl()                                   */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//biabl_list *beh_bitrzbiabl(biabl_list *biabl, int index)
//{
//  if (biabl)
//  {
//    biabl_list  *res;
//    loop_list   *loop;
//
//    res = beh_explodeBiabl(biabl->NEXT,index,name,func);
//    if ((loop = beh_getloop(biabl)))
//    {
//      if (loop->INDEX)
//      {
//        beh_explodeLoop(loop,biabl,name,func);
//        return NULL;
//      }
//    }
//    else
//    {
//      ptype_list    *ptx;
//
//      res           = beh_addbiabl(res,biabl->LABEL,biabl->CNDABL,NULL);
//      res->VALABL   = getAblAtPos(biabl->VALABL,index);
//      res->TIME     = biabl->TIME;
//      res->FLAG     = biabl->FLAG;
//      res->USER     = dupptypelst(biabl->USER);
//    }
//    return res;
//  }
//  else
//    return biabl;
//}
//
///*}}}************************************************************************/
/*{{{                    beh_resolvingfunc()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beh_resolvingfunc(char *name,int index,biabl_list *biabl)
{
  char           buf[1024], *vname;
  chain_list    *explist;
  
  sprintf(buf,"%s %d",name, index);
  vname   = namealloc(buf);
  explist = (chain_list *)gethtitem(BEH_EXP_TABLE,vname);
  if ((long)explist == EMPTYHT)
  {
    explist = NULL;
    BEH_EXP_LIST = addchain(BEH_EXP_LIST,vname);
  }
  addhtitem(BEH_EXP_TABLE,vname,(long)addchain(explist,biabl));
}

/*}}}************************************************************************/
/*{{{                    beh_addloopreg()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bereg_list *beh_addloopreg(bereg_list *reg)
{
  chain_list    *loopreg, *chainx;

  for (chainx = BEH_EXP_LIST; chainx; chainx = delchain(chainx,chainx))
  {
    loopreg = (chain_list *)gethtitem(BEH_EXP_TABLE,chainx->DATA);
    reg     = beh_addbereg(reg,chainx->DATA,beh_expgetbiabl(loopreg),NULL,0);
    freechain(loopreg);
  }

  BEH_EXP_LIST = NULL;
  return reg;
}


/*}}}************************************************************************/
/*{{{                    beh_addloopbus()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebus_list *beh_addloopbus(bebus_list *bus)
{
  chain_list    *loopbus, *chainx;

  for (chainx = BEH_EXP_LIST; chainx; chainx = delchain(chainx,chainx))
  {
    loopbus = (chain_list *)gethtitem(BEH_EXP_TABLE,chainx->DATA);
    bus     = beh_addbebus(bus,chainx->DATA,beh_expgetbiabl(loopbus),NULL,0,0);
    freechain(loopbus);
  }

  BEH_EXP_LIST = NULL;
  return bus;
}


/*}}}************************************************************************/
/*{{{                    beh_addloopbux()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bebux_list *beh_addloopbux(bebux_list *bux)
{
  chain_list    *loopbux, *chainx;

  for (chainx = BEH_EXP_LIST; chainx; chainx = delchain(chainx,chainx))
  {
    loopbux = (chain_list *)gethtitem(BEH_EXP_TABLE,chainx->DATA);
    bux     = beh_addbebux(bux,chainx->DATA,beh_expgetbiabl(loopbux),NULL,0,0);
    freechain(loopbux);
  }

  BEH_EXP_LIST = NULL;
  return bux;
}


/*}}}************************************************************************/
/*{{{                    beh_expgetbiabl()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static biabl_list *beh_expgetbiabl(chain_list *list)
{
  if (list)
  {
    ((biabl_list *)(list->DATA))->NEXT = beh_expgetbiabl(list->NEXT);
    return list->DATA;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
///*{{{                    beh_explodeVect()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static vectbiabl_list *beh_explodeVect(vectbiabl_list *vbiabl, char *name)
//{
//  if (vbiabl)
//  {
//    vbiabl->NEXT = beh_explodeVect(vbiabl->NEXT,name);
//    return beh_explodeVectBiabl(vbiabl,name);
//  }
//  else
//    return vbiabl;
//}
//
///*}}}************************************************************************/
///*{{{                    beh_explodeVectBiabl()                             */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static vectbiabl_list *beh_explodeVectBiabl(vectbiabl_list *vbiabl, char *name)
//{
//  vectbiabl_list    *next   = vbiabl->NEXT;
//  biabl_list        *biabl  = vbiabl->BIABL;
//  int                left   = vbiabl->LEFT;
//  int                right  = vbiabl->RIGHT;
//  int                i;
//  vectbiabl_list    *vbiablx;
//  biabl_list        *new;
//  int                way;
//
//  if (left == right)
//    return vbiabl;
//  else
//  {
//    way             = (left - right) / abs(left - right);
//    vbiabl->RIGHT   = left;
//    for ( i = left - way ; way*(i - right) >= 0; i -= way )
//    {
//      if (i == left)
//        vbiablx     = vbiabl;
//      else
//        vbiablx     = beh_addvectbiabl(vbiablx,i,i,NULL);
//      if (( new = beh_explodeBiabl(biabl,abs(i-left),name,NULL) ))
//        vbiablx->BIABL    = new;
//    }
//    return next;
//  }
//}
//
///*}}}************************************************************************/
/*{{{                    beh_explodeBiabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list *beh_explodeBiabl(biabl_list *biabl,int index, char *name,int left,
                             int right,void (*func)(char*,int,biabl_list*))
{
  if (biabl)
  {
    biabl_list  *res;
    loop_list   *loop;

    res = beh_explodeBiabl(biabl->NEXT,index,name,left,right,func);
    if ((loop = beh_getloop(biabl)))
    {
      if (loop->INDEX)
      {
        beh_explodeLoop(loop,biabl,name,func);
        return NULL;
      }
    }
    else
    {
      res           = beh_addbiabl(res,biabl->LABEL,NULL,NULL);
      res->CNDABL   = copyExpr(biabl->CNDABL);
      res->VALABL   = getAblAtIndex(biabl->VALABL,left,right,index);
      res->TIME     = biabl->TIME;
      res->TIMEVAR     = biabl->TIMEVAR;
      res->FLAG     = biabl->FLAG;
      res->USER     = dupptypelst(biabl->USER);
    }
    return res;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_explodeLoop()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_explodeLoop(loop_list *loop, biabl_list *biabl,char *name,
                     void (*func)(char*,int,biabl_list*))
{
  eqt_ctx *ctx = eqt_init(3);
  beh_explodeLoopInt(ctx,loop,biabl,name,func);
  eqt_term(ctx);
}

/*}}}************************************************************************/
/*{{{                    beh_explodeLoopInt()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beh_explodeLoopInt(eqt_ctx *ctx,loop_list *loop,biabl_list *biabl,char *name,
                               void (*func)(char*,int,biabl_list*))
{
  static char    ind[255];

  if (loop)
  {
    int      i  , way;

    way    = (loop->LEFT - loop->RIGHT) / abs(loop->LEFT - loop->RIGHT);
    for (i = loop->LEFT; way*(i - (int)loop->RIGHT) >= 0; i -= way)
    {
      eqt_addvar(ctx,loop->VARNAME,i);
      sprintf(ind,"%s",loop->INDEX);
      beh_explodeLoopInt(ctx,loop->NEXT,biabl,name,func);
    }
  }
  else
  {
    biabl_list  *new;
    ptype_list  *ptx;
    char        *eqtres;
    int          index;

    eqtres          = eqt_getEvaluedEquation(ctx,ind);
    index           = atoi(eqtres);
    mbkfree(eqtres);
    new             = beh_addbiabl(NULL,biabl->LABEL,NULL,NULL);
    new->VALABL     = beh_explodeLoopAbl(ctx,copyExpr(biabl->VALABL));
    new->CNDABL     = beh_explodeLoopAbl(ctx,copyExpr(biabl->CNDABL));
    new->TIME       = biabl->TIME;
    new->TIMEVAR       = biabl->TIMEVAR;
    new->FLAG       = biabl->FLAG & ~BEH_CND_LOOP;
    for ( ptx = biabl->USER; ptx; ptx = ptx->NEXT)
      if (ptx->TYPE != BEH_TYPE_LOOP)
        new->USER   = addptype(new->USER,ptx->TYPE,ptx->DATA);
    if (func)
      (*func) (name,index,new);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_explodeLoopAbl()                               */
/*                                                                          */
/* eqt variables positionned                                                */
/*                                                                          */
/****************************************************************************/
static chain_list *beh_explodeLoopAbl(eqt_ctx *ctx,chain_list *abl)
{
  if (ATOM(abl))
  {
    char     name[256], index[256];
    char    *eqtres;

    switch (sscanf(VALUE_ATOM(abl),"%s %s",name,index))
    {
      case 2 :
           eqtres    = eqt_getEvaluedEquation(ctx,index);
           sprintf(name,"%s %s",name,eqtres);
           abl->DATA = namealloc(name);
           mbkfree(eqtres);
      default :
           return abl;
    }
  }
  else
  {
    chain_list  *ablx   = abl;

    while ((ablx = CDR(ablx)))
      CAR(ablx) = beh_explodeLoopAbl(ctx,CAR(ablx));
    return abl;
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
