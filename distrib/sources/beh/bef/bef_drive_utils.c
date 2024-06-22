/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : bef_drive_utils.c                                           */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include AVT_H
#include "bef_drive_utils.h"
#include "bef_lib.h"

#define     BEF_INDENT      "  "

static int         CKTYPE;
static int         MAXNAME;

static chain_list  *strip_ckedge_int(chain_list *ptabl, char *clock);
static void drive_signals_bereg(bef_driver *driver);
static void drive_signals_bevectreg(bef_driver *driver, bevectreg_list *vreg);
static void drive_signals_bebux(bef_driver *driver);
static void drive_signals_bevectbux(bef_driver *driver, bevectbux_list *vbux);
static void drive_signals_beaux(bef_driver *driver);
static void drive_signals_bevectaux(bef_driver *driver, bevectaux_list *vaux);

static inline char *getName(void *be, long type);
static inline void *getNext(void *be, long type);
static inline void *vectnam(void *be, int *left, int *right, char **name, char type);
static inline void getBeobjField(void *be, long type, chain_list **abl, long *time, biabl_list **biabl, vectbiabl_list **vbiabl, char **name, char *head, int *left, int *right,int *timer, int *timef, char *flags, long *moreflags);
static void drive_one_vpor(bef_driver *driver, bevectpor_list *vpor);
static bepor_list *drive_one_por(bef_driver *driver, bepor_list *por);
static void drive_vpor(bef_driver *driver, bevectpor_list *vpor,int cpt);
static void drive_por(bef_driver *driver, bepor_list *por);
static void print_biabl(bef_driver *driver, biabl_list *biabl, char *name, char *vname, int vsize, int *np, char *head, int type, char flags);

static void drive_process(bef_driver *driver, biabl_list *biabl, char *name, char *vname, int vsize, int *np, char *head, int type, char flags);
static char *getStrAblReduce(bef_driver *driver, chain_list *abl, char *buf, int *bufsize);

static chain_list *porreg_l = NULL;
ht *BEH_TIMEVARS=NULL;

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    Declaration des signaux (compatible)               */
/****************************************************************************/
///*{{{                    reverse_takeBiggerNameReg()                        */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static bereg_list *reverse_takeBiggerNameReg(bereg_list *reg)
//{
//  bereg_list    *p, *q = NULL;
//  int            len;
//
//  if (!reg)
//    return NULL;
//  while ((p = reg->NEXT))
//  {
//    len         = strlen(reg->NAME);
//    if (MAXNAME < len)
//      MAXNAME   = len;
//    reg->NEXT   = q;
//    q           = reg;
//    reg         = p;
//  }
//  reg->NEXT     = q;
//
//  return reg;
//}
//
//
///*}}}************************************************************************/
///*{{{                    reverse_takeBiggerNameBux()                        */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static bebux_list *reverse_takeBiggerNameBux(bebux_list *bux)
//{
//  bebux_list    *p, *q = NULL;
//  int            len;
//
//  if (!bux)
//    return NULL;
//  while ((p = bux->NEXT))
//  {
//    len         = strlen(bux->NAME);
//    if (MAXNAME < len)
//      MAXNAME   = len;
//    bux->NEXT   = q;
//    q           = bux;
//    bux         = p;
//  }
//  bux->NEXT     = q;
//
//  return bux;
//}
//
//
///*}}}************************************************************************/
///*{{{                    reverse_takeBiggerNameAux()                        */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static beaux_list *reverse_takeBiggerNameAux(beaux_list *aux)
//{
//  beaux_list    *p, *q = NULL;
//  int            len;
//
//  if (!aux)
//    return NULL;
//  while ((p = aux->NEXT))
//  {
//    len         = strlen(aux->NAME);
//    if (MAXNAME < len)
//      MAXNAME   = len;
//    aux->NEXT   = q;
//    q           = aux;
//    aux         = p;
//  }
//  aux->NEXT     = q;
//
//  return aux;
//}
//
//
//
static int hasevents(biabl_list *biabl)
{
  biabl_list    *biablx;
  char *clk;
  
  for (biablx = biabl; biablx; biablx = biablx->NEXT)
     if ((bef_search_stable(biablx->CNDABL,&clk)) > 0) return 1;
  return 0;
}

static char *insert_name(char *name, char *postfix, char *buf)
{
  char *c;
  
  if ((c=strchr(name, ' '))!=NULL)
  {
    *c='\0';
    sprintf(buf, "%s%s %s",name,postfix,c+1);
    *c=' ';
  }
  else
    sprintf(buf, "%s%s", name, postfix);
  return buf;
}
///*}}}************************************************************************/
/*{{{                    drive_signals_bereg()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_bereg(bef_driver *driver)
{
  int            left, right;
  bereg_list    *reg;
  char          *name = NULL;
  ht            *porht = driver->POR.PORHT;
  int verilog, inertial;
  char buf[2048];

  driver->FIG->BEREG = (bereg_list*)reverse((chain_list*)driver->FIG->BEREG);

  for (reg = driver->FIG->BEREG; reg; reg = reg->NEXT)
  {
    reg              = vectnam(reg,&left,&right,&name,BEG_TYPE_SINGREG);
    inertial=V_BOOL_TAB[__MGL_INERTIAL_MEMORY].VALUE && (reg->FLAGS & BEH_FLAG_NORMAL)==0;
    verilog=(inertial && driver->VLG && !hasevents(reg->BIABL) && !beh_isloop(reg->BIABL));
    if (verilog)
    {
      insert_name(name, "_prereg", buf);
      driver->SIGNAL(driver->FP,buf,'W',left,right);
    }
    if (porht && gethtitem(porht,namealloc(name)) != EMPTYHT)
    {
      porreg_l = addchain(porreg_l,reg);
      driver->SIGNAL(driver->FP,name,'P',left,right);
      mbkfree(name);
      continue;
    }
    driver->SIGNAL(driver->FP,name,'R',left,right);
    mbkfree(name);
  }
  
  driver->FIG->BEREG = (bereg_list*)reverse((chain_list*)driver->FIG->BEREG);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_bevectreg()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_bevectreg(bef_driver *driver, bevectreg_list *vreg)
{

  int verilog, inv, inertial;
  char buf[2048];
  bevectreg_list *vregx;

  vreg=(bevectreg_list *)reverse((chain_list *)vreg);
  for(vregx=vreg; vregx; vregx=vregx->NEXT)
  {
    ht          *porht = driver->POR.PORHT;
//    drive_signals_bevectreg(driver,vreg->NEXT);

    inertial=V_BOOL_TAB[__MGL_INERTIAL_MEMORY].VALUE && (vregx->FLAGS & BEH_FLAG_NORMAL)==0;
    verilog=(inertial && driver->VLG && !hasevents(vregx->VECTBIABL->BIABL) && !beh_isloop(vregx->VECTBIABL->BIABL));

    if (verilog)
    {
      insert_name(vregx->NAME, "_prereg", buf);
      driver->SIGNAL(driver->FP,buf,'W',vregx->LEFT,vregx->RIGHT);
    }
    
    if (!(porht && (gethtitem(porht,vregx->NAME) != EMPTYHT)))
      driver->SIGNAL(driver->FP,vregx->NAME,'R',vregx->LEFT,vregx->RIGHT);
    else
    {
      driver->SIGNAL(driver->FP,vregx->NAME,'P',vregx->LEFT,vregx->RIGHT);
      porreg_l = addchain(porreg_l,vregx);
    }
  }
  vreg=(bevectreg_list *)reverse((chain_list *)vreg);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_bebux()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_bebux(bef_driver *driver)
{
  int             left, right;
  struct bebux   *bux;
  char           *name = NULL;
  int verilog, oldstyle;
  char buf[2048];

  driver->FIG->BEBUX = (bebux_list*)reverse((chain_list*)driver->FIG->BEBUX);

  for (bux = driver->FIG->BEBUX; bux; bux = bux->NEXT)
  {
    bux              = vectnam(bux,&left,&right,&name,BEG_TYPE_SINGBUX);
    oldstyle=V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (bux->FLAGS & BEH_FLAG_NORMAL)!=0;
    verilog=(!oldstyle && V_BOOL_TAB[__MGL_TRISTATE_IS_MEMORY].VALUE && (bux->FLAGS & BEH_FLAG_FORCEBUS)==0 && driver->VLG && !hasevents(bux->BIABL) && !beh_isloop(bux->BIABL));
    if (verilog)
    {
      insert_name(name, "_prereg", buf);
      driver->SIGNAL(driver->FP,buf,'r',left,right);
      driver->SIGNAL(driver->FP,name,'R',left,right);
    }
    else
      driver->SIGNAL(driver->FP,name,oldstyle?'R':'r',left,right);
    mbkfree(name);
  }
  driver->FIG->BEBUX = (bebux_list*)reverse((chain_list*)driver->FIG->BEBUX);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_bevectbux()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_bevectbux(bef_driver *driver, bevectbux_list *vbux)
{
  int verilog, oldstyle;
  char buf[2048];
  bevectbux_list *vbuxx;
  
  vbux=(bevectbux_list *)reverse((chain_list *)vbux);
  for (vbuxx=vbux; vbuxx; vbuxx=vbuxx->NEXT)
  {
//    drive_signals_bevectbux(driver,vbux->NEXT);
    
    oldstyle=V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (vbuxx->FLAGS & BEH_FLAG_NORMAL)!=0;
    verilog=(!oldstyle && V_BOOL_TAB[__MGL_TRISTATE_IS_MEMORY].VALUE && (vbuxx->FLAGS & BEH_FLAG_FORCEBUS)==0 && driver->VLG && !beh_isloop(vbuxx->VECTBIABL->BIABL) && !hasevents(vbuxx->VECTBIABL->BIABL));
    if (verilog)
    {
      insert_name(vbuxx->NAME, "_prereg", buf);
      driver->SIGNAL(driver->FP,buf,'r',vbuxx->LEFT,vbuxx->RIGHT);
      driver->SIGNAL(driver->FP,vbuxx->NAME,'R',vbuxx->LEFT,vbuxx->RIGHT);
    }
    else
      driver->SIGNAL(driver->FP,vbuxx->NAME,oldstyle?'R':'r',vbuxx->LEFT,vbuxx->RIGHT);
  }
  vbux=(bevectbux_list *)reverse((chain_list *)vbux);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_bebus()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

static void drive_signals_bebus(bef_driver *driver)
{
  int             left, right;
  struct bebus   *bus;
  char           *name = NULL;
  int verilog, oldstyle;
  char buf[2048];

  driver->FIG->BEBUS = (bebus_list*)reverse((chain_list*)driver->FIG->BEBUS);

  for (bus = driver->FIG->BEBUS; bus; bus = bus->NEXT)
  {
    bus              = vectnam(bus,&left,&right,&name,BEG_TYPE_SINGBUX);
    oldstyle=V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (bus->FLAGS & BEH_FLAG_NORMAL)!=0;
    verilog=(!oldstyle && V_BOOL_TAB[__MGL_TRISTATE_IS_MEMORY].VALUE && (bus->FLAGS & BEH_FLAG_FORCEBUS)==0 && driver->VLG && !hasevents(bus->BIABL));
    if (verilog)
    {
      insert_name(name, "_prereg", buf);
      driver->SIGNAL(driver->FP,buf,'r',left,right);
      driver->SIGNAL(driver->FP,name,'b',left,right);
    }
    else if (oldstyle)
      driver->SIGNAL(driver->FP,name,'b',left,right);
    mbkfree(name);
  }
  driver->FIG->BEBUS = (bebus_list*)reverse((chain_list*)driver->FIG->BEBUS);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_bevectbus()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_bevectbus(bef_driver *driver, bevectbus_list *vbus)
{
  int verilog, oldstyle;
  char buf[2048];
  bevectbus_list *vbusx;
  
  vbus=(bevectbus_list *)reverse((chain_list *)vbus);
  for (vbusx=vbus; vbusx; vbusx=vbusx->NEXT)
  {
//    drive_signals_bevectbus(driver,vbus->NEXT);
    oldstyle=V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (vbus->FLAGS & BEH_FLAG_NORMAL)!=0;
    verilog=(!oldstyle && V_BOOL_TAB[__MGL_TRISTATE_IS_MEMORY].VALUE && (vbus->FLAGS & BEH_FLAG_FORCEBUS)==0 && driver->VLG && !beh_isloop(vbusx->VECTBIABL->BIABL) && !hasevents(vbusx->VECTBIABL->BIABL));
    if (verilog)
    {
      insert_name(vbusx->NAME, "_prereg", buf);
      driver->SIGNAL(driver->FP,buf,'r',vbusx->LEFT,vbusx->RIGHT);
      driver->SIGNAL(driver->FP,vbusx->NAME,'b',vbusx->LEFT,vbusx->RIGHT);
    }
    else if (oldstyle)
      driver->SIGNAL(driver->FP,vbusx->NAME,'b',vbusx->LEFT,vbusx->RIGHT);
  }
  vbus=(bevectbus_list *)reverse((chain_list *)vbus);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_beaux()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_beaux(bef_driver *driver)
{
  int             left, right;
  beaux_list     *aux;
  char           *name = NULL;

  driver->FIG->BEAUX = (beaux_list*)reverse((chain_list*)driver->FIG->BEAUX);

  for (aux = driver->FIG->BEAUX; aux; aux = aux->NEXT)
  {
    aux              = vectnam(aux,&left,&right,&name,BEG_TYPE_SINGAUX);
    driver->SIGNAL(driver->FP,name,'W',left,right);
    mbkfree(name);
  }
  driver->FIG->BEAUX = (beaux_list*)reverse((chain_list*)driver->FIG->BEAUX);
}

/*}}}************************************************************************/
/*{{{                    drive_signals_bevectaux()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_signals_bevectaux(bef_driver *driver, bevectaux_list *vaux)
{
  bevectaux_list *vauxx;
  
  vaux=(bevectaux_list *)reverse((chain_list *)vaux);
  for (vauxx=vaux; vauxx; vauxx=vauxx->NEXT)
  {
//    drive_signals_bevectaux(driver,vaux->NEXT);
    driver->SIGNAL(driver->FP,vauxx->NAME,'W',vauxx->LEFT,vauxx->RIGHT);
  }
  vaux=(bevectaux_list *)reverse((chain_list *)vaux);
}

/*}}}************************************************************************/
/*{{{                    print_biabl()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_biabl(bef_driver *driver, biabl_list *biabl, char *name,
                        char *vname, int vsize, int *np, char *head, int type, char flags)
{
  if (!biabl)
    beh_error(40, name);
  else if (!biabl->CNDABL)
    beh_toolbug(19,"print_biabl",name,0);
  else if (!biabl->VALABL)
    beh_toolbug(20,"print_biabl",name,0);
  else
  {
    // case of  with ... select
    if ((type != BEG_MASK_REG) && (beh_simplifybiabl(biabl)))
      driver->PROC.WITHSELECT(driver->FP,biabl,vname,vsize,driver->ABUFF,
                              driver->ABUFS);
    // case of process
    else
      drive_process(driver,biabl,name,vname,vsize,np,head,type,flags);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Utilities (compatible)                             */
/****************************************************************************/
/*{{{                    getName()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline char *getName(void *be, long type)
{
  char      *res = NULL;

  switch (type)
  {
    case BEG_TYPE_SINGPOR :
         res        = ((bepor_list *)be)->NAME;
         break;
    case BEG_TYPE_SINGBUX :
         res        = ((bebux_list *)be)->NAME;
         break;
    case BEG_TYPE_SINGREG :
         res        = ((bereg_list *)be)->NAME;
         break;
    case BEG_TYPE_SINGAUX :
         res        = ((beaux_list *)be)->NAME;
         break;
    case BEG_TYPE_SINGBUS :
         res        = ((bebus_list *)be)->NAME;
         break;
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    getNext()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void *getNext(void *be, long type)
{
  void      *res = NULL;

  switch (type)
  {
    case BEG_TYPE_SINGPOR :
         res        = ((bepor_list *)be)->NEXT;
         break;
    case BEG_TYPE_SINGBUX :
         res        = ((bebux_list *)be)->NEXT;
         break;
    case BEG_TYPE_SINGREG :
         res        = ((bereg_list *)be)->NEXT;
         break;
    case BEG_TYPE_SINGAUX :
         res        = ((beaux_list *)be)->NEXT;
         break;
    case BEG_TYPE_SINGBUS :
         res        = ((bebus_list *)be)->NEXT;
         break;
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    vectnam()                                          */
/*                                                                          */
/*  analyze a list of signal and return the bounds of the vectorised        */
/*    signals, if they occure.                                              */
/*                                                                          */
/****************************************************************************/
static inline void *vectnam(void *be, int *left, int *right, char **name,
                            char type)
{
  char          *blank_space, *sig_name, name_tmp[200], number[200];
  char           END = 0;

  *left                 = -1;
  *right                = -1;
  
  sig_name              = getName(be,type);
  if (sig_name)
  {
    *name               = (char*)mbkalloc(strlen(sig_name) + 1);
    strcpy(*name, sig_name);
    blank_space         = strchr(*name,' ');
    if (blank_space)
    {
      strcpy(number, blank_space);
      *left             = atoi(number);
      *right            = *left;
      *blank_space      = '\0';
    }

    while (!END)
    {
      if (getNext(be,type))
      {
        strcpy(name_tmp,getName(getNext(be,type),type));
        blank_space     = strchr(name_tmp,' ');
        if (blank_space)
        {
          strcpy(number,blank_space);
          *blank_space  = '\0';
          if (!strcmp(*name,name_tmp))
          {
            *right      = atoi(number);
            be          = getNext(be,type);
          }
          else
            END         = 1;
        }
        else
          END           = 1;
      }
      else
        END             = 1;
    }
    return (be);
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    preprocess_abl()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *bef_preprocess_abl(chain_list *abl)
{
  chain_list     *resabl;
  chain_list     *devabl;
  chain_list     *simpabl;

  devabl    = devXorExpr(abl);
  simpabl   = simplif10Expr(devabl);
  resabl    = flatPolarityExpr(simpabl, 1);

  freeExpr(devabl);
  freeExpr(simpabl);

  return (resabl);
}

/*}}}************************************************************************/
/*{{{                    iprint()                                           */
/*                                                                          */
/* print with indentation, keep coherance                                   */
/****************************************************************************/
void iprint(FILE *fp,char mode,char *str, ... )
{
  va_list        arg;
  static int     indent = 0;
  static int     init = 0;
  int            i, usebuf, par, old, hd, j, l;
  char           buf[2048];

  va_start(arg,str);

  usebuf        = 0;
  switch (mode)
  {
    case '+' :
         init   = 1;
    case '>' :
         indent ++;
         break;
    case '-' :
         init   = -1;
    case '<' :
         indent --;
         break;
    case 'i' :
         indent = 1;
         init   = 1;
         break;
    case 'd' :
         usebuf = 1;
  }

  if (str)
  {
    if (usebuf)
    {
      l = strlen(BEF_INDENT);
      vsprintf(buf,str,arg);
      for (i = 0, hd = 0, par = 0, old = 0; buf[i] != '\0'; i++)
        switch (buf[i])
        {
          case ':'  :
                 if (MAXNAME)
                   for (j = i; j < indent*l - init + MAXNAME + 2; j ++)
                     fprintf(fp," ");
                 break;
        }

      for (i = init; i < indent + par; i ++)
        fprintf(fp,"%s",BEF_INDENT);
      fprintf(fp,"%s",buf+hd);
    }
    else
    {
      for (i = init; i < indent; i ++)
        fprintf(fp,"%s",BEF_INDENT);
      vfprintf(fp,str,arg);
    }
  }

  init          = 0;
  va_end(arg);
}

/*}}}************************************************************************/
/*{{{                    strip_ckedge_int()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *strip_ckedge_int(chain_list *ptabl, char *clock)
{
  if (ATOM(ptabl))
    if (VALUE_ATOM(ptabl) == clock)
      if (!CKTYPE)
        return createAtom("'1'");
      else
        return createAtom("'0'");
    else
      return addchain(NULL,ptabl->DATA);
  else if (OPER(ptabl) == STABLE)
    return createAtom("'0'");
  else
  {
    chain_list  *auxExpr, *oldExpr, *expr1;

    oldExpr     = ptabl;
    auxExpr     = createExpr (OPER (ptabl));
    expr1       = auxExpr;

    while ((ptabl = CDR (ptabl)))
      addQExpr(auxExpr,bef_strip_ckedge(CAR(ptabl),clock));

    /* on sauvegarde la poubelle */
    ((chain_list *)auxExpr->DATA)->NEXT = CDR(CAR(oldExpr));

    return (auxExpr);
  }
}

/*}}}************************************************************************/
/*{{{                    getBeobjField()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void getBeobjField(void *be, long type, chain_list **abl,
                                 long *time, biabl_list **biabl,
                                 vectbiabl_list **vbiabl, char **name,
                                 char *head, int *left, int *right, int *timer, int *timef, char *flags, long *moreflags)
{
  long l;
  *timer=*timef=0;
  *flags=0;
  *moreflags=0;
  switch (type)
  {
    case BEG_TYPE_SINGAUX :
         if (BEH_TIMEVARS!=NULL && (l=gethtitem(BEH_TIMEVARS, ((beaux_list *)be)->TIMEVAR))!=EMPTYHT)
           *time   = (int)l;
         else
           *time   = ((beaux_list *)be)->TIME;
         *abl    = ((beaux_list *)be)->ABL;
         *name   = ((beaux_list *)be)->NAME;
         *timer=((beaux_list *)be)->TIMER;
         *timef=((beaux_list *)be)->TIMEF;
         *moreflags=((beaux_list *)be)->FLAGS;
         break;
    case BEG_TYPE_VECTAUX :
         if (BEH_TIMEVARS!=NULL && (l=gethtitem(BEH_TIMEVARS, ((bevectaux_list *)be)->TIMEVAR))!=EMPTYHT)
           *time   = (int)l;
         else
           *time   = ((bevectaux_list *)be)->TIME;
         *abl    = ((bevectaux_list *)be)->ABL;
         *name   = ((bevectaux_list *)be)->NAME;
         *left   = ((bevectaux_list *)be)->LEFT;
         *right  = ((bevectaux_list *)be)->RIGHT;
         *moreflags=((bevectaux_list *)be)->FLAGS;
         break;
    case BEG_TYPE_SINGOUT :
         if (BEH_TIMEVARS!=NULL && (l=gethtitem(BEH_TIMEVARS, ((beout_list *)be)->TIMEVAR))!=EMPTYHT)
           *time   = (int)l;
         else
           *time   = ((beout_list *)be)->TIME;
         *timer=((beout_list *)be)->TIMER;
         *timef=((beout_list *)be)->TIMEF;
         *abl    = ((beout_list *)be)->ABL;
         *name   = ((beout_list *)be)->NAME;
         *moreflags=((beout_list *)be)->FLAGS;
         break;
    case BEG_TYPE_VECTOUT :
         if (BEH_TIMEVARS!=NULL && (l=gethtitem(BEH_TIMEVARS, ((bevectout_list *)be)->TIMEVAR))!=EMPTYHT)
           *time   = (int)l;
         else
           *time   = ((bevectout_list *)be)->TIME;
         *abl    = ((bevectout_list *)be)->ABL;
         *name   = ((bevectout_list *)be)->NAME;
         *left   = ((bevectout_list *)be)->LEFT;
         *right  = ((bevectout_list *)be)->RIGHT;
         *moreflags=((bevectout_list *)be)->FLAGS;
         break;
    case BEG_TYPE_VECTBUX :
         *vbiabl = ((bevectbux_list *)be)->VECTBIABL;
         *name   = ((bevectbux_list *)be)->NAME;
         *flags=((bevectbux_list *)be)->FLAGS;
         sprintf(head,"VBUX");
         break;
    case BEG_TYPE_SINGBUX :
         *biabl  = ((bebux_list *)be)->BIABL;
         *name   = ((bebux_list *)be)->NAME;
         *flags=((bebux_list *)be)->FLAGS;
         sprintf(head,"BUX");
         break;
    case BEG_TYPE_VECTBUS :
         *vbiabl = ((bevectbus_list *)be)->VECTBIABL;
         *name   = ((bevectbus_list *)be)->NAME;
         *flags=((bevectbus_list *)be)->FLAGS;
         sprintf(head,"VBUS");
         break;
    case BEG_TYPE_SINGBUS :
         *biabl  = ((bebus_list *)be)->BIABL;
         *name   = ((bebus_list *)be)->NAME;
         *flags=((bebus_list *)be)->FLAGS;
         sprintf(head,"BUS");
         break;
    case BEG_TYPE_VECTREG :
         *vbiabl = ((bevectreg_list *)be)->VECTBIABL;
         *name   = ((bevectreg_list *)be)->NAME;
         *flags=((bevectreg_list *)be)->FLAGS;
         sprintf(head,"VREG");
         break;
    case BEG_TYPE_SINGREG :
         *biabl  = ((bereg_list *)be)->BIABL;
         *name   = ((bereg_list *)be)->NAME;
         *flags=((bereg_list *)be)->FLAGS;
         sprintf(head,"REG");
         break;
    default :
         beh_toolbug(19,"print_one_beobj type",NULL,0);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Driver initialisation (compatible)                 */
/****************************************************************************/
/*{{{                    bef_driverSet()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_driverSet(bef_driver   *driver,
                   FILE         *fp,
                   befig_list   *fig,
                   char        **buffer,
                   int          *bufsize,
                   void          (*time)        (FILE*,int,int,int),
                   void          (*end_loop)    (FILE*,char,char*,int,int,int,loop_list*),
                   void          (*looplist)    (FILE*,char,char*,int,int*,int,loop_list*),
                   void          (*loop)        (FILE*,char,char*,int,int*,loop_list*),
                   void          (*value)       (FILE*,chain_list*,char,int,int,int,char**,int*),
                   void          (*affected)    (FILE*,char,char,int,int,int,char*,char*,loop_list*,long),
                   void          (*signal)      (FILE*,char*,char,int,int),
                   void          (*bebus_ass)   (FILE*,char*,char*,int),
                   int           (*isone)       (char*))
{
  driver->FP                = fp;
  driver->FIG               = fig;
  driver->ABUFF             = buffer;
  driver->ABUFS             = bufsize;
  driver->TIME              = time;
  driver->END_LOOP          = end_loop;
  driver->LOOP_LIST         = looplist;
  driver->LOOP              = loop;
  driver->VALUE             = value;
  driver->AFFECTED          = affected;
  driver->SIGNAL            = signal;
  driver->INTER_ASS         = bebus_ass;
  driver->ISONE             = isone;
}

/*}}}************************************************************************/
/*{{{                    bef_driverSetAbl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_driverSetAbl(bef_driver *driver,
                      char *(*abl2strBool)(chain_list*,char*,int*))
{
  driver->ABL.TOBOOLSTR     = abl2strBool;
}

/*}}}************************************************************************/
/*{{{                    bef_driverSetProcess()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_driverSetProcess(bef_driver    *driver,
                          void           (*withselect)  (FILE*,biabl_list*,char*,int,char**,int*),
                          chain_list    *(*declar)      (FILE*,biabl_list*,char*,int,char*,char**,ptype_list*clock),
                          void           (*begin)       (FILE*,chain_list*),
                          void           (*end)         (FILE*),
                          void           (*if_)         (FILE*,char*),
                          void           (*elsif)       (FILE*,char*),
                          int            (*else_)       (FILE*),
                          int            (*endif)       (FILE*,char),
                          int            (*edge)        (char*,char*,char,char*))
{
  driver->PROC.WITHSELECT   = withselect;
  driver->PROC.DECLAR       = declar;
  driver->PROC.BEGIN        = begin;
  driver->PROC.END          = end;
  driver->PROC.IF           = if_;
  driver->PROC.ELSIF        = elsif;
  driver->PROC.ELSE         = else_;
  driver->PROC.ENDIF        = endif;
  driver->PROC.EDGE         = edge;
}

/*}}}************************************************************************/
/*{{{                    bef_driverSetStr()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_driverSetStr(bef_driver    *driver,
                      char          *(*name)        (char*),
                      char          *(*vname)       (char*,int,int),
                      char          *(*vector)      (char*, char *),
                      char          *(*bitstr)      (char*,char*),
                      char          *(*bebus)       (char*),
                      char          *(*bereg)       (char*))
{
  driver->STR.NAME          = name;
  driver->STR.VNAME         = vname;
  driver->STR.VECTOR        = vector;
  driver->STR.BITSTR        = bitstr;
  driver->STR.BEBUS         = bebus;
  driver->STR.BEREG         = bereg;
}

/*}}}************************************************************************/
/*{{{                    bef_driverSetPor()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_driverSetPor(bef_driver    *driver,
                      void           (*por)     (FILE*,char*,char,int,int),
                      void           (*end)     (FILE*),
                      void           (*start)   (FILE*),
                      ht            *porht
                      )
{
  driver->POR.POR           = por;
  driver->POR.END           = end;
  driver->POR.START         = start;
  driver->POR.PORHT         = porht;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Enable from outside (compatible)                   */
/****************************************************************************/
/*{{{                    bef_gettimeunit()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *bef_gettimeunit (unsigned char time_unit_char)
{
  switch (time_unit_char)
  {
    case BEH_TU__FS:
         return(namealloc("fs"));
    case BEH_TU__PS:
         return(namealloc("ps"));
    case BEH_TU__NS:
         return(namealloc("ns"));
    case BEH_TU__US:
         return(namealloc("us"));
    case BEH_TU__MS:
         return(namealloc("ms"));
    default:
         beh_error (200, NULL);
         EXIT (1);
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    bef_strip_ckedge()                                 */
/*                                                                          */
/* strip the clock edge condition                                           */
/****************************************************************************/
chain_list *bef_strip_ckedge(chain_list *ptabl, char *clock)
{
  chain_list    *temp_expr, *res_expr;

  temp_expr     = strip_ckedge_int(ptabl,clock);
  res_expr      = simplif10Expr(temp_expr);
  freeExpr(temp_expr);

  return (res_expr);
}

/*}}}************************************************************************/
/*{{{                    bef_search_stable()                                */
/*                                                                          */
/* detecte la presence de STABLE                                            */
/****************************************************************************/
int bef_search_stable(chain_list *ptabl, char **clock)
{
  int             num = 0;

  if (ATOM(ptabl))
    return 0;
  if (OPER(ptabl) == STABLE)
  {
    *clock      = VALUE_ATOM(CADR(ptabl));

    return 1;
  }
  else
    while ((ptabl = CDR(ptabl)) != NULL)
      num      += bef_search_stable(CAR(ptabl),clock);

  return num;
}

/*}}}************************************************************************/
/*{{{                    bef_get_stable_attribute()                         */
/*                                                                          */
/* recupere l'attribut de STABLE                                            */
/****************************************************************************/
int bef_get_stable_attribute(chain_list *ptabl, char **clock)
{
  int            num    = 0;

  if (ATOM(ptabl))
    return 0;
  
  if (OPER(ptabl) == STABLE)
  {
    *clock  = VALUE_ATOM(CADR(ptabl));
    if (ATTRIB(ptabl))
      num   = VALUE_ATTRIB(ptabl);
  }
  else
    while ((ptabl = CDR(ptabl)))
      if ((num = bef_get_stable_attribute(CAR(ptabl),clock)))
        return num;

  return num;
}

/*}}}************************************************************************/
/*{{{                    search_notck_int()                                 */
/*                                                                          */
/* detecte la presence not ck ou ck                                         */
/****************************************************************************/
static int search_notck_int(chain_list * ptabl, char *clock)
{
  int        num = 0;

  if (ATOM(ptabl))
    return 0;

  /* on verifie si le prochain signal est la clock */
  if (OPER(ptabl) == NOT)
    if (VALUE_ATOM(CADR(ptabl)) == clock)
 /*   if (CDR(CADR(ptabl)) &&
        VALUE_ATOM(CADR(CADR(ptabl))) == clock)*/
    {
      CKTYPE    = 1;
      return 1;
    }
    else
      return 0;
  else
    while ((ptabl = CDR(ptabl)) != NULL)
      num      += search_notck_int(CAR(ptabl),clock);

  return num;
}

/*}}}************************************************************************/
/*{{{                    bef_search_notck()                                 */
/*                                                                          */
/* detecte la presence not ck ou ck                                         */
/****************************************************************************/
int bef_search_notck(chain_list * ptabl, char *clock)
{
  CKTYPE        = 0;

  return search_notck_int(ptabl,clock);
}

/*}}}************************************************************************/
/*{{{                    bef_drive_sigDeclaration()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_sigDeclaration(bef_driver *driver)
{
  drive_signals_bevectbus (driver,driver->FIG->BEVECTBUS);
  drive_signals_bebus     (driver);
  drive_signals_bevectreg (driver,driver->FIG->BEVECTREG);
  drive_signals_bereg     (driver);
  drive_signals_bevectbux (driver,driver->FIG->BEVECTBUX);
  drive_signals_bebux     (driver);
  drive_signals_bevectaux (driver,driver->FIG->BEVECTAUX);
  drive_signals_beaux     (driver);
}

/*}}}************************************************************************/
/*{{{                    bef_print_one_beobj()                              */
/*                                                                          */
/* print a process for a single signal                                      */
/*                                                                          */
/****************************************************************************/

static void bef_print_reg_case(bef_driver *driver, char *input, char *output, char *outputv)
{
  char *name0, *stg="1'b0, 1'b1: ";
  chain_list *temp;
  if (V_INT_TAB[__MGL_DRIVE_CONFLICT].VALUE)
      stg="1'b0, 1'b1, 1'bx: ";
  name0=driver->STR.VECTOR(input, NULL);
  iprint(driver->FP,'i',"always @( %s )\n",name0);
  driver->PROC.BEGIN(driver->FP,NULL);
  iprint(driver->FP,0,"case ( %s )\n", name0);
  iprint(driver->FP,'>',stg);
//  iprint(driver->FP,'>',"1'b0, 1'b1: ");
  driver->AFFECTED(driver->FP,0,'R',0,0,0,output,outputv,NULL,0);
  temp=createAtom(input);
  driver->VALUE   (driver->FP,temp,'R',0,0,0,driver->ABUFF, driver->ABUFS);
  freeExpr(temp);
  iprint(driver->FP, '<' ,"endcase\n");
  iprint(driver->FP,'-',NULL);
  driver->PROC.END(driver->FP);
}

                  
void bef_print_one_beobj(bef_driver *driver, void *be, int *np,long type)
{
  chain_list        *abl;
  long               time;
  biabl_list        *biabl;
  vectbiabl_list    *vbiabl, *vbiablx;
  char              *vname;
  char              *name;
  int                vectsize;
  char               head[16];
  long               singorvect, moreflags;
  int                left, right;
  char              *save_name, flags;
  int timer, timef, verilogbus, verilogreg;
  char buf[1024];

  getBeobjField(be,type,&abl,&time,&biabl,&vbiabl,&name,head,&left,&right,&timer,&timef,&flags,&moreflags);

  singorvect = type &  BEG_MASK_TYPE;
  type       = type & ~BEG_MASK_TYPE;
  
  verilogbus=(!(V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (flags & BEH_FLAG_NORMAL)!=0) && V_BOOL_TAB[__MGL_TRISTATE_IS_MEMORY].VALUE && (flags & BEH_FLAG_FORCEBUS)==0 && driver->VLG);
  verilogreg=(V_BOOL_TAB[__MGL_INERTIAL_MEMORY].VALUE && (flags & BEH_FLAG_NORMAL)==0 && driver->VLG);
  
  switch (singorvect)
  {
    case BEG_MASK_VECT :
         switch (type)
         {
           case BEG_MASK_BUS :
           case BEG_MASK_BUX :
                for (vbiablx=vbiabl; vbiablx; vbiablx = vbiablx->NEXT)
                  if (hasevents(vbiablx->BIABL) || beh_isloop(vbiablx->BIABL)) break;
                if (verilogbus && vbiablx==NULL && vbiabl!=NULL && vbiabl->LEFT==vbiabl->RIGHT && type!=BEG_MASK_BUS)
                {
                  chain_list *temp;
                  char buf0[1024];
                  char *name0, *oname=name;
                  insert_name(name, "_prereg", buf);
                  name        = driver->STR.NAME(buf);
                  save_name   = oname;
                  iprint(driver->FP,'i',"\n");

                  if (type==BEG_MASK_BUS)
                    save_name = driver->STR.BEBUS(oname);
                    
                  for (; vbiabl; vbiabl = vbiabl->NEXT)
                  {
                    iprint(driver->FP,'i',"\n");
                    left      = vbiabl->LEFT;
                    right     = vbiabl->RIGHT;
                    vname     = driver->STR.VNAME(name,left,right);
                    vectsize  = abs(left - right) + 1;
                    biabl     = vbiabl->BIABL;

                    name0     = driver->STR.VNAME(name,left,right);
                    print_biabl(driver,biabl, name, name0,vectsize,np,head,type,flags);

                    mbkfree(name0);

                    name0        = driver->STR.NAME(save_name);
                    vname     = driver->STR.VNAME(name0,left,right);
                    sprintf(buf0,"%s %d", buf, vbiabl->LEFT);
                    bef_print_reg_case(driver,buf0,name0,vname);
  
                    mbkfree(vname);
                  }
                  if (type==BEG_MASK_BUS)
                  {
                    driver->INTER_ASS(driver->FP,oname,save_name,0);
                  }
                }
                else
                {
                  name        = driver->STR.NAME(name);
                  if ((V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (flags & BEH_FLAG_NORMAL)!=0) && type==BEG_MASK_BUS)
                  {
                    save_name   = name;
                    name        = driver->STR.BEBUS(name);
                  }
                  for (; vbiabl; vbiabl = vbiabl->NEXT)
                  {
                    iprint(driver->FP,'i',"\n");
                    left      = vbiabl->LEFT;
                    right     = vbiabl->RIGHT;
                    vname     = driver->STR.VNAME(name,left,right);
                    vectsize  = abs(left - right) + 1;
                    biabl     = vbiabl->BIABL;

                    print_biabl(driver,biabl,name,vname,vectsize,np,head,type,flags);
                    mbkfree(vname);
                  }
                  if ((V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (flags & BEH_FLAG_NORMAL)!=0) && type==BEG_MASK_BUS)
                    driver->INTER_ASS(driver->FP,save_name,name,0);
                }
                break;
           case BEG_MASK_REG :// case BEG_MASK_BUX :
                for (vbiablx=vbiabl; vbiablx; vbiablx = vbiablx->NEXT)
                  if (hasevents(vbiablx->BIABL) || beh_isloop(vbiablx->BIABL)) break;
                if (verilogreg && vbiablx==NULL && vbiabl!=NULL && vbiabl->LEFT==vbiabl->RIGHT)
                {
                  chain_list *temp;
                  char buf0[1024];
                  char *name0, *oname=name;
                  insert_name(name, "_prereg", buf);
                  name        = driver->STR.NAME(buf);
                  save_name   = oname;
                  iprint(driver->FP,'i',"\n");
                  if (porreg_l && porreg_l->DATA == be)
                  {
                    save_name     = driver->STR.BEREG(name);
                  }
                  for (; vbiabl; vbiabl = vbiabl->NEXT)
                  {
                    iprint(driver->FP,'i',"\n");
                    left      = vbiabl->LEFT;
                    right     = vbiabl->RIGHT;
//                    vname     = driver->STR.VNAME(name,left,right);
                    vectsize  = abs(left - right) + 1;
                    biabl     = vbiabl->BIABL;

                    name0     = driver->STR.VNAME(name,left,right);

                    print_biabl(driver,biabl,name,name0,vectsize,np,head,type,flags);
                    mbkfree(name0);

                    name0        = driver->STR.NAME(save_name);
                    vname     = driver->STR.VNAME(name0,left,right);
                    sprintf(buf0,"%s %d", buf, vbiabl->LEFT);

                    bef_print_reg_case(driver,buf0,name0,vname);
                    
                    mbkfree(vname);
                  }

                }
                else
                {
                  name        = driver->STR.NAME(name);
                  if (porreg_l && porreg_l->DATA == be)
                  {
                    save_name = name;
                    name      = driver->STR.BEREG(name);
                  }
                  for (; vbiabl; vbiabl = vbiabl->NEXT)
                  {
                    iprint(driver->FP,'i',"\n");
                    left      = vbiabl->LEFT;
                    right     = vbiabl->RIGHT;
                    vname     = driver->STR.VNAME(name,left,right);
                    vectsize  = abs(left - right) + 1;
                    biabl     = vbiabl->BIABL;
                    print_biabl(driver,biabl,name,vname,vectsize,np,head,type,flags);
                    mbkfree(vname);
                  }
                  if (porreg_l && porreg_l->DATA == be)
                  {
                    porreg_l  = delchain(porreg_l,porreg_l);
                    driver->INTER_ASS(driver->FP,save_name,name,0);
                  }
                }
                break;
           case BEG_MASK_OUT : case BEG_MASK_AUX :
                if (!abl)
                  beh_error(40, vname);
              
                /* added by zinaps */
                if (abs(right-left)<128)
                {                    
                  name      = driver->STR.NAME(name);
                  vname     = driver->STR.VNAME(name,left,right);
                  driver->AFFECTED(driver->FP,'0','W',time,timer,timef,NULL,vname,NULL,moreflags);
                  driver->VALUE   (driver->FP,abl,'W',time,timer,timef,driver->ABUFF,
                                   driver->ABUFS);
                  mbkfree(vname);
                }
                else
                {
                  int    i, w = -1;
                  chain_list *cl;
                  if (left>right)
                  {
                    w       = right;
                    right   = left;
                    left    = w;
                  }
                  name        = driver->STR.NAME(name);
                  for (i = left; i <= right; i ++)
                  {                        
                    vname   = driver->STR.VNAME(name,i,i);
                    driver->AFFECTED(driver->FP,'0','W',time,timer,timef,NULL,vname,NULL,moreflags);
                    if (w == -1) 
                      cl    = getAblAtPos(abl, i-left);
                    else
                      cl    = getAblAtPos(abl, right-i);
                    driver->VALUE(driver->FP,cl,'W',time,timer,timef,driver->ABUFF,driver->ABUFS);
                    freeExpr(cl);
                    mbkfree(vname);
                  }
                }
                /* ^^^^^^^^^^^^^^ */
                break;
           default : ;
         }
         break;
    case BEG_MASK_SING :
         switch (type)
         {
           case BEG_MASK_BUS :
           case BEG_MASK_BUX :
                if (verilogbus && type!=BEG_MASK_BUS && !beh_isloop(biabl) && !hasevents(biabl))
                {
                  chain_list *temp;
                  char buf0[1024];
                  char *name0;

                  iprint(driver->FP,'i',"\n");
                  insert_name(name, "_prereg", buf);

                  if (type==BEG_MASK_BUS)
                  {
                    save_name   = name;
                    name        = driver->STR.BEBUS(name);
                  }
                  
                  print_biabl(driver,biabl,buf,buf,1,np,head,type,flags);

                  bef_print_reg_case(driver,buf,name,name);

                  if (type==BEG_MASK_BUS)
                    driver->INTER_ASS(driver->FP,save_name,name,1);
                }
                else
                {
                   if ((V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (flags & BEH_FLAG_NORMAL)!=0) && type==BEG_MASK_BUS)
                   {
                     save_name   = name;
                     name        = driver->STR.BEBUS(name);
                   }
                   iprint(driver->FP,'i',"\n");
                   print_biabl(driver,biabl,name,name,1,np,head,type,flags);
                   if ((V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (flags & BEH_FLAG_NORMAL)!=0) && type==BEG_MASK_BUS)
                     driver->INTER_ASS(driver->FP,save_name,name,1);
                }
                break;
           case BEG_MASK_REG :
                save_name   = name;
                if (porreg_l && porreg_l->DATA == be)
                {
                  name        = driver->STR.BEREG(name);
                }
                if (verilogreg && !beh_isloop(biabl) && !hasevents(biabl))
                {
                  insert_name(save_name, "_prereg", buf);
                  iprint(driver->FP,'i',"\n");
                  print_biabl(driver,biabl,buf,buf,1,np,head,type,flags);
                  bef_print_reg_case(driver,buf,name,name);
                }
                else
                {
                 iprint(driver->FP,'i',"\n");
                 print_biabl(driver,biabl,name,name,1,np,head,type,flags);
                }
                if (porreg_l && porreg_l->DATA == be)
                {
                  porreg_l    = delchain(porreg_l,porreg_l);
                  driver->INTER_ASS(driver->FP,save_name,name,1);
                }
                break;
           case BEG_MASK_OUT : case BEG_MASK_AUX :
                if (!abl)
                  beh_error(40, name);
                driver->AFFECTED(driver->FP,'0','W',time,timer,timef,name,name,NULL,moreflags);
                driver->VALUE   (driver->FP,abl,'W',time,timer,timef,driver->ABUFF,
                                 driver->ABUFS);
                break;
           default : ;
         }
         break;
    default : ;
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitements des signaux simples (compatible)       */
/****************************************************************************/
/*{{{                    bef_driver_aux()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_aux(bef_driver *driver)
{
  beaux_list        *aux  = driver->FIG->BEAUX;
  bevectaux_list    *vaux = driver->FIG->BEVECTAUX;

  if (aux || vaux)
    iprint(driver->FP,'i',"\n");

  for (; aux; aux = aux->NEXT)
    bef_print_one_beobj(driver,  aux, NULL, BEG_TYPE_SINGAUX);
  for (; vaux; vaux = vaux->NEXT)
    bef_print_one_beobj(driver, vaux, NULL, BEG_TYPE_VECTAUX);
}

/*}}}************************************************************************/
/*{{{                    bef_drive_out()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_out(bef_driver *driver)
{
  bevectout_list    *vout = driver->FIG->BEVECTOUT;
  beout_list        *out  = driver->FIG->BEOUT;

  if (out || vout)
    iprint(driver->FP,'i',"\n");

  for (; out; out  = out->NEXT)
    bef_print_one_beobj(driver,  out, NULL, BEG_TYPE_SINGOUT);
  for (; vout; vout = vout->NEXT)
    bef_print_one_beobj(driver, vout, NULL, BEG_TYPE_VECTOUT);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitement des bus (compatible)                    */
/****************************************************************************/
/*{{{                    bef_drive_bux()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_bux(bef_driver *driver)
{
  bebux_list     *bux;
  bevectbux_list *vbux;
  int             numprocess = 0;

  for (bux = driver->FIG->BEBUX; bux; bux = bux->NEXT)
    bef_print_one_beobj(driver,bux,&numprocess,BEG_TYPE_SINGBUX);

  numprocess = 0;
  for(vbux=driver->FIG->BEVECTBUX;vbux;vbux=vbux->NEXT)
    bef_print_one_beobj(driver,vbux,&numprocess,BEG_TYPE_VECTBUX);
}

/*}}}************************************************************************/
/*{{{                    bef_drive_bus()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_bus(bef_driver *driver)
{
  bebus_list     *bus;
  bevectbus_list *vbus;
  int             numprocess = 0;

  for (bus = driver->FIG->BEBUS; bus; bus = bus->NEXT)
    bef_print_one_beobj(driver,bus,&numprocess,BEG_TYPE_SINGBUS);

  numprocess = 0;
  for(vbus = driver->FIG->BEVECTBUS;vbus;vbus=vbus->NEXT)
    bef_print_one_beobj(driver,vbus,&numprocess,BEG_TYPE_VECTBUS);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitement des registres  (compatible)             */
/****************************************************************************/
/*{{{                    bef_drive_reg()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_reg(bef_driver *driver)
{
  bereg_list     *reg;
  bevectreg_list *vreg;
  int             numprocess    = 0;

  for (reg = driver->FIG->BEREG; reg; reg = reg->NEXT)
    bef_print_one_beobj(driver,reg,&numprocess,BEG_TYPE_SINGREG);

  numprocess = 0;
  for(vreg=driver->FIG->BEVECTREG;vreg;vreg=vreg->NEXT)
    bef_print_one_beobj(driver,vreg,&numprocess,BEG_TYPE_VECTREG);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Declaration des connecteurs                        */
/****************************************************************************/
/*{{{                    reverse_takeBiggerNamePor()                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bepor_list *reverse_takeBiggerNamePor(bepor_list *por)
{
  bepor_list    *p, *q = NULL;
  int            len;

  if (!por)
    return NULL;
  while ((p = por->NEXT))
  {
    len         = strlen(por->NAME);
    if (MAXNAME < len)
      MAXNAME   = len;
    por->NEXT   = q;
    q           = por;
    por         = p;
  }
  por->NEXT     = q;

  return por;
}

/*}}}************************************************************************/
/*{{{                    porGetDir()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
char porGetDir(bepor_list *st, bepor_list *ed)
{
  char       res;

  res       = 'X';

  for (; st != ed->NEXT; st = st->NEXT)
    if (st->DIRECTION != 'X')
      /*
      if (res != 'X' && res != st->DIRECTION)
        res     = 'Z';
      */
      res   = st->DIRECTION;
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    drive_one_por()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bepor_list *drive_one_por(bef_driver *driver, bepor_list *por)
{
  bepor_list    *porx;
  int            left, right;
  char          *name, dir;

  porx      = vectnam(por,&left,&right,&name,BEG_TYPE_SINGPOR);
  dir       = porGetDir(por,porx);
  if (driver->POR.PORHT)
    addhtitem(driver->POR.PORHT,namealloc(name),BEG_TYPE_SINGPOR);
  driver->POR.POR(driver->FP,name,dir,left,right);

  mbkfree(name);
  
  return porx;
}

/*}}}************************************************************************/
/*{{{                    drive_one_vpor()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_one_vpor(bef_driver *driver, bevectpor_list *vpor)
{
  driver->POR.POR(driver->FP,vpor->NAME,vpor->DIRECTION,
                  vpor->LEFT,vpor->RIGHT);
  if (driver->POR.PORHT)
    addhtitem(driver->POR.PORHT,vpor->NAME,0);
}

/*}}}************************************************************************/
/*{{{                    bef_initMaxname()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_initMaxname(void)
{
  MAXNAME       = 0;
}

/*}}}************************************************************************/
/*{{{                    bef_drive_vpor()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_vpor(bef_driver *driver, bevectpor_list *vpor,int cpt)
{
  if (vpor)
  {
    int      len;
    
    len         = strlen(vpor->NAME);
    if (MAXNAME < len)
      MAXNAME   = len;
    drive_vpor(driver,vpor->NEXT,cpt+1);
    drive_one_vpor(driver,vpor);
    if (cpt)
      fprintf(driver->FP,";\n");
  }
}


/*}}}************************************************************************/
/*{{{                    bef_drive_por()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_por(bef_driver *driver, bepor_list *por)
{
  if (por)
  {
    bepor_list  *porx;

    por     = reverse_takeBiggerNamePor(por);

    for (porx = por; porx; )
      if ((porx = drive_one_por(driver, porx)->NEXT ))
        fprintf(driver->FP,";\n");

    por     = (bepor_list *)reverse((chain_list *)por);
  }
}

/*}}}************************************************************************/
/*{{{                    port_declaration()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bef_drive_porDeclaration(bef_driver *driver)
{
  MAXNAME       = 0;
  driver->POR.START(driver->FP);
  drive_vpor (driver,driver->FIG->BEVECTPOR,driver->FIG->BEPOR ? 1 : 0);
  drive_por  (driver,driver->FIG->BEPOR);
  driver->POR.END(driver->FP);
  MAXNAME       = 0;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Drive structures of behavior                       */
/****************************************************************************/
/*{{{                    drive_cnd_abl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_cnd_abl(bef_driver *driver, char *buffer, int *done_endif,
                          int *done_else, int first, int sameckcnd,
                          long flag, void *next, int drive_edge)
{
  // condition drive
  // the first
  if (first)
  {
    if (drive_edge)
      driver->PROC.IF(driver->FP,buffer);
  }
  // the last for else
  else if (!next && driver->ISONE(buffer))
  {
    *done_else      = driver->PROC.ELSE(driver->FP);
    *done_endif     = 1;
  }
  // precedence
  else if (!(flag&BEH_CND_PRECEDE))
  {
    if (!sameckcnd)
    {
      if (!done_endif)
        *done_endif = driver->PROC.ENDIF(driver->FP,'0');
      if (drive_edge)
        driver->PROC.IF(driver->FP,buffer);
    }
    else
      iprint(driver->FP,'+',NULL);
    *done_else      = 0;
  }
  else if (flag&BEH_CND_LAST)
    *done_else      = driver->PROC.ELSE(driver->FP);
  else if (!sameckcnd)
    driver->PROC.ELSIF(driver->FP,buffer);
}

/*}}}************************************************************************/
/*{{{                    drive_end_of_process()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_end_of_process(bef_driver *driver, loop_list *lastloop,
                                 int done_endif, int done_else, int cpt,
                                 char *name, char *vname, int vsize,
                                 int np, char *head, int type)
{
  char           buf[4096];
  
  if ( (type != BEG_MASK_REG) && !done_else)
  {
    driver->PROC.ELSE(driver->FP);
    driver->AFFECTED(driver->FP,'-','R',0,0,0,name,vname,lastloop,0);
    if (lastloop && lastloop->INDEX)
      fprintf(driver->FP,"%s;\n",driver->STR.BITSTR(genHZBitStr(1),buf));
    else
      fprintf(driver->FP,"%s;\n",driver->STR.BITSTR(genHZBitStr(vsize),buf));
  }
  if (!done_endif)
    driver->PROC.ENDIF(driver->FP,'-');
  driver->END_LOOP(driver->FP,'-',head,np,cpt,0,lastloop);
}

/*}}}************************************************************************/
/*{{{                    drive_one_biabl()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void drive_one_biabl(bef_driver *driver, biabl_list *biabl,
                            loop_list **lastloop, char **oldckcnd,
                            int *done_endif, int *done_else, int *cpt,
                            int first, char *name,
                            char *vname, int vsize, int *np, char *head,
                            int type, char **buffer, int *bsize)
{
  long           flag = biabl->FLAG;
  FILE          *fp = driver->FP;
  bef_process    proc = driver->PROC;
  int            isEdgeCondition;
  int            sameloop;
  int            drive_edge;
  loop_list     *loop;
  int            sndckcnd;
  char          *clock, buf[4096];
  int            sameckcnd;
  biabl_list    *next;
  
  next                  = biabl->NEXT;
  
  // detect edge condition
  if (type == BEG_MASK_REG)
    isEdgeCondition     = bef_search_stable(biabl->CNDABL,&clock);
  else
  {
    isEdgeCondition     = 0;
    flag|=BEH_CND_PRECEDE;
  }
  // case of edge condition
  if (isEdgeCondition)
  {
    char       ckstate, *ckname, *signame;

    if (bef_search_notck(biabl->CNDABL,clock) > 0)
      ckstate           = '0';
    else
      ckstate           = '1';
    beg_get_vectname(clock,&signame,NULL,NULL,BEG_SEARCH_ABL);
    signame             = driver->STR.NAME(signame);
    ckname              = driver->STR.VECTOR(clock,buf);

    drive_edge          = proc.EDGE(*buffer,ckname,ckstate,signame);
    
    sameckcnd           = *oldckcnd && !strcmp(*oldckcnd,*buffer);
    if (*oldckcnd)
      mbkfree(*oldckcnd);
    *oldckcnd           = mbkstrdup(*buffer);
  }
  else
  // default
  {
    *buffer             = getStrAblReduce(driver,biabl->CNDABL,*buffer,bsize);
    if (*oldckcnd)
      mbkfree(*oldckcnd);
    *oldckcnd           = NULL;
    sameckcnd           = 0;
    drive_edge          = 1;
  }

  if (!*done_endif && !(flag&BEH_CND_PRECEDE) && !(flag&BEH_CND_LAST))
  {
    if (type != BEG_MASK_REG && !*done_else)
    {
/*      proc.ELSE(fp);
      *done_else        = 1;
      driver->AFFECTED(fp,'-','R',0,0,0,name,vname,*lastloop);
      if (*lastloop && (*lastloop)->INDEX)
        fprintf(fp,"%s;\n",driver->STR.BITSTR(genHZBitStr(1),buf));
      else
        fprintf(fp,"%s;\n",driver->STR.BITSTR(genHZBitStr(vsize),buf));
        */
    }
    if (!sameckcnd)
      *done_endif       = proc.ENDIF(fp,'0');
  }

  loop                  = beh_getloop(biabl);
  sameloop              = beh_isSameLoop(loop,*lastloop);
  driver->END_LOOP (fp,'<',head,*np,*cpt,sameloop,*lastloop);
  driver->LOOP_LIST(fp,'+',head,*np,cpt,sameloop,loop);
  
  drive_cnd_abl(driver,*buffer,done_endif,done_else,first,sameckcnd,
                flag,next,drive_edge);

  // supplementary condition in case of register
  if (isEdgeCondition)
  {
    chain_list    *cndabl;

    cndabl              = bef_strip_ckedge(biabl->CNDABL,clock);
    sndckcnd            = (!(ATOM(cndabl) &&
                             !(strcmp(VALUE_ATOM(cndabl),"'1'") &&
                               strcmp(VALUE_ATOM(cndabl),"'0'"))));
    if (sndckcnd)
    {
      *buffer           = getStrAblReduce(driver,cndabl,*buffer,bsize);
      drive_cnd_abl(driver,*buffer,done_endif,done_else,first,0,flag,next,1);
    }
    freeExpr(cndabl);
  }
  else
    sndckcnd            = 0;

  // value
  if (biabl->VALABL)
  {
    int time;
    long l;
    if (BEH_TIMEVARS!=NULL && (l=gethtitem(BEH_TIMEVARS, biabl->TIMEVAR))!=EMPTYHT)
      time=(int)l;
    else
      time=biabl->TIME;
    if (biabl->TIMER!=0 || biabl->TIMEF!=0)
      time=(int)((biabl->TIMER+biabl->TIMEF)/2.0+0.5);
    driver->AFFECTED(fp,'-','R',time,0,0,name,vname,loop,biabl->FLAG);
    driver->VALUE   (fp,biabl->VALABL,'R',time,0,0,driver->ABUFF,
                     driver->ABUFS);
  }
  else
    beh_toolbug(20, "drive_process",name, 0);

  
  if (!drive_edge && !sndckcnd)
    *done_endif           = 1;
  else
    *done_endif           = 0;
  

  if (sndckcnd && !(next && next->FLAG & BEH_CND_PRECEDE))
    if (drive_edge)
      proc.ENDIF(fp,'-');
  *lastloop             = loop;
}

/*}}}************************************************************************/
/*{{{                    getStrAblReduce()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *getStrAblReduce(bef_driver *driver, chain_list *abl,
                             char *buffer, int *bsize) 
{
  chain_list    *red;

  red               = bef_preprocess_abl(abl);
  buffer[0]         = '\0';
  buffer            = driver->ABL.TOBOOLSTR(red,buffer,bsize);
  freeExpr(red);

  return buffer;
}

/*}}}************************************************************************/
/*{{{                    drive_process()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static ptype_list *getevents(biabl_list *biabl, int *others)
{
  biabl_list    *biablx;
  char *clk;
  ptype_list *all=NULL, *pt;
  long dir;
  
  *others=0;
  for (biablx = biabl; biablx; biablx = biablx->NEXT)
     if ((bef_search_stable(biablx->CNDABL,&clk)) > 0)
     {
        if (bef_search_notck(biablx->CNDABL,clk) > 0) dir=0;
        else dir=1;

        for (pt=all; pt!=NULL && !(pt->DATA==clk && pt->TYPE==dir); pt=pt->NEXT) ;
        if (pt==NULL)
          all=addptype(all, dir,clk);        
     }
     else (*others)++;
  return (ptype_list *)reverse((chain_list *)all);
}

static void drive_process(bef_driver *driver, biabl_list *biabl, char *name,
                          char *vname, int vsize, int *np, char *head,
                          int type, char flags)
{
  chain_list    *loopchain;
  char          *oldckcnd;
  int            done_endif, done_else, cpt, first, go, others;
  biabl_list    *biablx;
  loop_list     *lastloop;
  ptype_list *events, *pt;
  char buf[4096];
  char *pbuf;
  int bsize, time;
  long l;
  
  driver->PROC.EDGE(buf,"",0,"");

  if (driver->VLG) // verilog
    events=getevents(biabl, &others);
  else
    events=NULL;  

  if ((!(V_BOOL_TAB[__MGL_OLD_STYLE_BUS].VALUE || (flags & BEH_FLAG_NORMAL)!=0) &&
      ((type & ~BEG_MASK_TYPE)== BEG_MASK_BUS || (type & ~BEG_MASK_TYPE)==BEG_MASK_BUX) && driver->VLG && vsize==1 && !beh_isloop(biabl))
      ||
      (V_BOOL_TAB[__MGL_INERTIAL_MEMORY].VALUE && (flags & BEH_FLAG_NORMAL)==0 &&
      ((type & ~BEG_MASK_TYPE)== BEG_MASK_REG) && driver->VLG && vsize==1 && !beh_isloop(biabl) && events==NULL)
     )
  {
    iprint(driver->FP,'i',"/* %s '%s' */\n",(type & ~BEG_MASK_TYPE)== BEG_MASK_REG?"REGISTER":"BUS",vname);
    for (biablx = biabl; biablx; biablx = biablx->NEXT)
      {
        if (BEH_TIMEVARS!=NULL && (l=gethtitem(BEH_TIMEVARS, biablx->TIMEVAR))!=EMPTYHT)
          time=(int)l;
        else
          time=biablx->TIME;
        *driver->ABUFF = getStrAblReduce(driver,biablx->CNDABL,*driver->ABUFF, driver->ABUFS);
        if (driver->ISONE(*driver->ABUFF)) l=BEH_CND_WEAK;
        else l=0;
        driver->AFFECTED(driver->FP,'0','w',time, biablx->TIMER, biablx->TIMEF, name, vname, NULL,biablx->FLAG | l);
        if (l==0)
        {
          fprintf(driver->FP, "(%s) ? ", *driver->ABUFF);
        }
        driver->VALUE   (driver->FP,biablx->VALABL,'w',biablx->TIME, biablx->TIMER, biablx->TIMEF,driver->ABUFF, driver->ABUFS);
        if (l==0)
          fprintf(driver->FP, " : 1'bz;\n");
        else
          fprintf(driver->FP, " ;\n");
      }
    iprint(driver->FP,'i',"/* ---------------------------- */\n",name);
  }
  else
  {
    if (events==NULL) events=addptype(events, 0, NULL);
    else if (others>0) events=addptype(events, 1, NULL);

    if (driver->VLG && events!=NULL && events->NEXT!=NULL)
      iprint(driver->FP,'i',"/* Event split for '%s' */\n",vname);

    for (pt=events; pt!=NULL; pt=pt->NEXT)
    {
      // sensibility list
      if (pt->TYPE==0 && pt->DATA==NULL)
        loopchain     = driver->PROC.DECLAR(driver->FP,biabl,name,*np,head,
                                            driver->ABUFF,NULL);
      else
        loopchain     = driver->PROC.DECLAR(driver->FP,biabl,name,*np,head,
                                            driver->ABUFF,pt);

      // process
      lastloop      = NULL;
      oldckcnd      = NULL;
      done_else     = 0;
      cpt           = 0;
      done_endif    = 1;
      first         = 1;

      driver->PROC.BEGIN(driver->FP,loopchain);
      for (biablx = biabl; biablx; biablx = biablx->NEXT, first = 0)
      {
        go=0;
        if (pt->TYPE==0 && pt->DATA==NULL) go=1;
        else
           {
             int hasclk;
             char *clk;
             long dir;
             hasclk=bef_search_stable(biablx->CNDABL,&clk);
             if (pt->TYPE==1 && pt->DATA==NULL && hasclk<=0) go=1;
             else if (hasclk>0 && clk==pt->DATA)
               {
                  if (bef_search_notck(biablx->CNDABL,clk) > 0) dir=0;
                  else dir=1;
                  if (dir==pt->TYPE) go=1;
               }
           }
       if (go)
         drive_one_biabl(driver,biablx,&lastloop,&oldckcnd,&done_endif,&done_else,
                        &cpt,first,name,vname,vsize,np,head,type,
                        driver->ABUFF,driver->ABUFS);
      }

      // end of process
      drive_end_of_process(driver,lastloop,done_endif,done_else,cpt,name,vname,
                           vsize,*np,head,type);
      driver->PROC.END(driver->FP);
      (*np) ++;
      
      if (oldckcnd)
        mbkfree(oldckcnd);
      if (pt->NEXT!=NULL) iprint(driver->FP,'i',"\n");
    }
    if (driver->VLG && events!=NULL && events->NEXT!=NULL)
      iprint(driver->FP,'i',"/* ---------------------------- */\n");
  }
  freeptype(events);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*}}}************************************************************************/
