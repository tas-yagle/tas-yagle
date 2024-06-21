/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Fichier : stm_API_cache.c                                             */
/*                                                                          */
/*    © copyright 2003 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "stm_API_cache.h"

static double            THRESHOLD      = 120.0;
static stm_tree4cache   *APISTM_TREE    = NULL;
static stm_current      *CURRENT        = NULL;

static int stm_gettimingfunction_and_signalcapa(char *out, timing_function **fct, double *capa);

static stm_current *stm_getCurrent(timing_function *fct);

void             stm_createCache(void);
double           stm_getSlope   (double inslope, double outload);
double           stm_getDelay   (double inslope, double outload);
void             stm_storeSlope (double inslope, double outload, double slope);
void             stm_storeDelay (double inslope, double outload, double delay);
int              stm_delayThresholdOK(double inslope, double outload, double threshold);
int              stm_slopeThresholdOK(double inslope, double outload, double threshold);

void             stm_displayRange(timing_table *table);

static void      stm_storeInTable(timing_table **table, double inslope, double outload, double delay, int mode);
static int       stm_thresholdOK(timing_table *table, double inslope, double outload, double threshold);
static int       stm_getBeforeAndAfter(float *range, int max, double value, int *before, int *after);
static double    stm_thresholdTest(double x0, double x1, double x, double y0, double y1);
static void      stm_getVar(timing_table *table, double *x0, double *x1, double *y0, double *y1, int bx, int ax, int by, int ay, int mode) ;
static int       stm_findIndex(float *range, double value, int size);
static int       stm_copyRangeAndInsert(float *range, float *new, double value, int size);

static int       isEqual(float a, float b);
static int       isInf(float a, float b);

static stm_tree4cache   *stm_newTree(stm_tree4cache *head);
static stm_tree4cache   *stm_freeTree(stm_tree4cache *tree);
static stm_tree4cache   *stm_freeAllTree(stm_tree4cache *tree);
static stm_tree4cache   *stm_addTree(stm_tree4cache *current, char *orig, char *dest, char *type);

static void    stm_setDelayNSlope(stm_tree4cache *tree, double delay, double slope);
static void    stm_updateCacheFromTree(stm_tree4cache *tree, double inslope);
static double  stm_getTreeSlope(stm_tree4cache *tree, char *orig);
static void    stm_applyTree(double inslope);
static double  stm_interpol(timing_table *table, double inslope, double outload, float (*func)(timing_table *, float, float), long type);
static double  stm_getValue(timing_table *table, double inslope, double outload, float (*func)(timing_table *, float, float));

static void stm_fillFindUpnDown(timing_table *tbl, int xind, int yind, int *up, int *down);
static void stm_fillCol(timing_table *tbl, int up, int down, int xind);
static void stm_fillNewLine(timing_table *tbl,int yind);
static void stm_fillFindLeftnRight(timing_table *tbl, int xind, int yind, int *beg, int *end);
static void stm_fillLine(timing_table *tbl, int beg, int end, int yind);
static void stm_fillNewCol(timing_table *tbl,int xind);
static void stm_fillTbl(timing_table *tbl, int xind, int yind, char type);

void             stm_test_threshold(void);

/****************************************************************************/
/*{{{                    stm_API_ActionTerminate()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_API_ActionTerminate() // commentaire pour desactiver l'ajout de token
{
  APISTM_TREE   = stm_freeAllTree(APISTM_TREE);
}
/*}}}************************************************************************/
/*{{{                    Tree Management                                    */
/****************************************************************************/
/*{{{                    stm_newTree()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static stm_tree4cache *stm_newTree(stm_tree4cache *head)
{
  stm_tree4cache    *res;

  res   = mbkalloc(sizeof(struct stm_tree4cache));
  
  res->FLAG     = 0;
  res->NEXT     = head;

  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_freeTree()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static stm_tree4cache *stm_freeTree(stm_tree4cache *tree)
{
  mbkfree(tree);

  return NULL;
}

/*}}}************************************************************************/
/*{{{                    stm_freeAllTree()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static stm_tree4cache *stm_freeAllTree(stm_tree4cache *tree)
{
  if (tree)
  {
    tree->NEXT  = stm_freeAllTree(tree->NEXT);
    tree        = stm_freeTree(tree);
  }
  return tree;
}

/*}}}************************************************************************/
/*{{{                    stm_addTree()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static stm_tree4cache *stm_addTree(stm_tree4cache *current, char *orig,
                                   char *dest, char *type)
{
  stm_tree4cache     treex, *res = NULL;

  orig              = namealloc(orig);
  dest              = namealloc(dest);
  type              = namealloc(type);
  treex.NEXT        = current;
  treex.ORIG        = NULL;
  
  for ( ; treex.NEXT; treex = *(treex.NEXT))
    if (treex.ORIG == orig && treex.DEST == dest && treex.DIRECTION == type)
      res           = &treex;

  if (res)
    return res;
  else
  {
    res             = stm_newTree(treex.NEXT);
    treex.NEXT      = res;
    res->NEXT       = current;
    res->ORIG       = orig;
    res->DEST       = dest;
    res->DIRECTION  = type;
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_setDelayNSlope()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_setDelayNSlope(stm_tree4cache *tree, double delay,
                               double slope)
{
  tree->DELAY   = delay;
  tree->SLOPE   = slope;
}

/*}}}************************************************************************/
/*{{{                    stm_updateCacheFromTree()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_updateCacheFromTree(stm_tree4cache *tree,double inslope)
{
  stm_tree4cache    *treex;

  for (treex = tree; tree; tree = tree->NEXT)
  {
    if (!treex->FLAG)
    {
      timing_function   *fct;
      timing_table      *tbl;
      stm_current       *cur;
      double             capa, slope;
      int                mode;

      switch (stm_gettimingfunction_and_signalcapa(treex->DEST,&fct,&capa))
      {
        case 0 :
        case 1 :
             cur            = stm_getCurrent(fct);
             slope          = stm_getTreeSlope(tree,treex->ORIG);
             inslope        = (slope > 0.0) ? slope : inslope;
             tbl            = cur->DELAY;
             mode           = stm_thresholdOK(tbl,inslope,capa, 20);
             stm_storeInTable(&tbl, inslope, capa, treex->DELAY, mode);
             cur->DELAY     = tbl;
             tbl            = cur->SLOPE;
             stm_storeInTable(&tbl, inslope, capa, treex->SLOPE, mode);
             cur->SLOPE     = tbl;
        default :
             treex->FLAG    = 1;
      }
    }
  }
}

/*}}}************************************************************************/
/*{{{                    stm_getTreeSlope()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static double stm_getTreeSlope(stm_tree4cache *tree, char *orig)
{
  for ( ; tree; tree = tree->NEXT)
    if (tree->DEST == orig)
      return tree->SLOPE;
  return -1.0;
}

/*}}}************************************************************************/
/*{{{                    SET_DELAY_AND_SLOPE()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void SET_DELAY_AND_SLOPE(char *in, char *out, char *direction, double delay,
                         double slope)
{
  stm_tree4cache    *res;

  res           = stm_addTree(APISTM_TREE,in,out,direction);
  stm_setDelayNSlope(res,delay,slope);
  if (!APISTM_TREE)
    APISTM_TREE = res;
}

/*}}}************************************************************************/
/*{{{                    stm_applyTree()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_applyTree(double inslope)
{
  stm_updateCacheFromTree(APISTM_TREE,inslope);
  APISTM_TREE   = stm_freeAllTree(APISTM_TREE);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Extern Funtions                                    */
/****************************************************************************/
/*{{{                    High Level Functions                               */
/*                                                                          */
/* CURRENT VSLOPE represents input slope setted by stm_thresholdOK          */
/* CURRENT VLOAD represents output capa setted by stm_thresholdOK           */
/*                                                                          */
/****************************************************************************/
void stm_sup_storeDelay(double delay)
{
  stm_storeDelay(CURRENT->VSLOPE,CURRENT->VLOAD,delay);
  stm_applyTree(CURRENT->VSLOPE);
}

void stm_sup_storeSlope(double slope)
{
  stm_storeSlope(CURRENT->VSLOPE,CURRENT->VLOAD,slope);
  stm_applyTree(CURRENT->VSLOPE);
}

double stm_sup_getDelay()
{
  return stm_getDelay(CURRENT->VSLOPE,CURRENT->VLOAD);
}

double stm_sup_getSlope()
{
  return stm_getSlope(CURRENT->VSLOPE,CURRENT->VLOAD);
}

/*}}}************************************************************************/

/*{{{                    stm_freeCurrent()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_freeCurrent(void *x)
{
  stm_current   *current;

  current   = x;
  if (current)
  {
    stm_freeCurrent(current->NEXT);
    stm_modtbl_destroy(current->DELAY);
    stm_modtbl_destroy(current->SLOPE);
    current->NEXT   = NULL;
    mbkfree(current);
  }
}

/*}}}************************************************************************/
/*{{{                    stm_newCurrent()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
stm_current *stm_newCurrent(stm_current *head, char *insname)
{
  stm_current   *current;

  current           = mbkalloc(sizeof(struct stm_current));
  current->NEXT     = head;
  current->INS      = insname;
  current->DELAY    = NULL;
  current->SLOPE    = NULL;
  current->VSLOPE   = 0.0;
  current->VLOAD    = 0.0;
  current->VDELAY   = 0.0;
  current->TRESRES  = 0;

  return current;
}

/*}}}************************************************************************/
/*{{{                    stm_getCurrentByName()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
stm_current *stm_getCurrentByName(ptype_list *ptype, char *name)
{
  stm_current   *cur;
  
  for (cur = ptype->DATA; cur && cur->INS != name; cur = cur->NEXT)
    ;
  if (!cur)
  {
    ptype->DATA   = stm_newCurrent(ptype->DATA,name);
    cur           = ptype->DATA;
  }

  return cur;
}

/*}}}************************************************************************/
/*{{{                    stm_getCurrent()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
stm_current *stm_getCurrent(timing_function *fct)
{
  ptype_list    *ptypex;
  stm_current   *cur;

  ptypex    = stm_modfct_getSharedType(fct,STM_CURRENT_TYPE);
  
  if (!ptypex)
    cur     = stm_modfct_addSharedType(fct,STM_CURRENT_TYPE,
                                       stm_newCurrent(NULL,fct->INS),
                                       stm_freeCurrent) -> DATA;
  else
    cur     = stm_getCurrentByName(ptypex,fct->INS);

  return cur;
}

/*}}}************************************************************************/
/*{{{                    stm_createCache()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_createCache()
{
  CURRENT   = stm_getCurrent(stm_get_current_arc());
#ifdef _PRINT
  printf("instance : %s\n",CURRENT->INS);
  stm_displayTable("slope",CURRENT->SLOPE);
  stm_displayTable("delay",CURRENT->DELAY);
#endif
}

/*}}}************************************************************************/
/*{{{                    stm_delayThresholdOK()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int stm_delayThresholdOK(double inslope, double outload, double threshold)
{
  stm_createCache();

#ifdef _PRINT
  printf("computing delay for slope %4.3g, load %4.3g\n",inslope,outload);
#endif
  CURRENT->TYPE     = STM_MODE_DELAY;
  CURRENT->VSLOPE   = inslope;
  CURRENT->VLOAD    = outload;
  CURRENT->TRESRES  = stm_thresholdOK(CURRENT->DELAY,inslope,outload,
                                      threshold);

  if (CURRENT->TRESRES&STM_THRESHOLD_NOMATCH)
    return 0;
  else
  {
#ifdef _PRINT
    //    printf("delai ok\n");
#endif
    return CURRENT->TRESRES;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_slopeThresholdOK()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int stm_slopeThresholdOK(double inslope, double outload, double threshold)
{
  stm_createCache();

#ifdef _PRINT
  printf("computing slope for slope %g, load %g\n",inslope,outload);
#endif
  CURRENT->TYPE     = STM_MODE_SLOPE;
  CURRENT->VSLOPE   = inslope;
  CURRENT->VLOAD    = outload;
  CURRENT->TRESRES  = stm_thresholdOK(CURRENT->SLOPE,inslope,outload,
                                      threshold);

  if (CURRENT->TRESRES & STM_THRESHOLD_NOMATCH)
    return 0;
  else
  {
#ifdef _PRINT
    printf("slope ok\n");
#endif
    return CURRENT->TRESRES;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_storeDelay()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_storeDelay(double inslope, double outload, double delay)
{
  stm_storeInTable( &(CURRENT->DELAY),inslope,outload,delay,
                    (CURRENT->TRESRES)&STM_THRESHOLD_MATCH_XY);
#ifdef _PRINT
  stm_displayTable(CURRENT->DELAY);
#endif
}

/*}}}************************************************************************/
/*{{{                    stm_storeSlope()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_storeSlope(double inslope, double outload, double slope)
{
#ifdef _PRINT
  printf("inserting %g,%g,%g\n",inslope,outload,slope);
#endif
  stm_storeInTable(&(CURRENT->SLOPE),inslope,outload,slope,
                   (CURRENT->TRESRES)&STM_THRESHOLD_MATCH_XY);
#ifdef _PRINT
  stm_displayTable(CURRENT->SLOPE);
#endif
}

/*}}}************************************************************************/
/*{{{                    stm_getDelay()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double stm_getDelay(double inslope, double outload)
{
  return stm_getValue(CURRENT->DELAY,inslope,outload,stm_modtbl_delay);
}

/*}}}************************************************************************/
/*{{{                    stm_getSlope()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double stm_getSlope(double inslope, double outload)
{
  return stm_getValue(CURRENT->SLOPE,inslope,outload,stm_modtbl_slew);
}

/*}}}************************************************************************/
/*{{{                    stm_printVariation()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_printVariation(FILE *fp, double real, double found)
{
  fprintf(fp,"real : %g",real);
  fprintf(fp,",found : %g",found);
  fprintf(fp,",variation : %3.1f%%\n",100*(real-found)/real);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Static functions                                   */
/****************************************************************************/
/*{{{                    stm_getValue()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static double stm_getValue(timing_table *table, double inslope,
                           double outload,
                           float (*func)(timing_table *, float, float))
{
//  stm_displayTable(table);
  if (CURRENT->TRESRES == STM_THRESHOLD_MATCH_XY)
  {
    int      x, y;
    x   = stm_findIndex(table->XRANGE,inslope,table->NX);
    y   = stm_findIndex(table->YRANGE,outload,table->NY);
    return table->SET2D[x][y];
  }
  else
    return stm_interpol(table,inslope,outload,func,CURRENT->TRESRES);
}

/*}}}************************************************************************/
/*{{{                    stm_interpol()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static double stm_interpol(timing_table *table, double inslope,
                           double outload,
                           float (*func)(timing_table *, float, float),
                           long type)
{
  float    **set2D, set1D[1024], res, *xrange;
  int        nx, x = -1, y = -1, i;
  long       xt;
  
  set2D             = table->SET2D;
  nx                = table->NX;
  xrange            = table->XRANGE;
  xt                = table->XTYPE;
  
  y = (table->NY == 1) ? 0 : (type&STM_THRESHOLD_MATCH_Y)
    ? stm_findIndex(table->YRANGE,outload,table->NY): y;
  x = (table->NX == 1) ? 0 : (type&STM_THRESHOLD_MATCH_X)
    ? stm_findIndex(table->XRANGE,inslope,table->NX) : x;
  
  if (y >= 0)
  {
    for (i = 0; i < table->NX; i ++)
      set1D[i]      = set2D[i][y];
    table->SET2D    = NULL;
    table->SET1D    = set1D;
  }
  else if (x >= 0)
  {
    table->SET2D    = NULL;
    table->SET1D    = set2D[x];
    table->NX       = table->NY;
    table->XRANGE   = table->YRANGE;
    table->XTYPE    = STM_LOAD;
  }
  
  res               = (*func)(table,outload,inslope);
  table->SET2D      = set2D;
  table->SET1D      = NULL;
  table->NX         = nx;
  table->XRANGE     = xrange;
  table->XTYPE      = xt;
  /*
  if (table->NY == 1)
  {
    int      i;

    for (i = 0; i < table->NX; i ++)
      set1D[i]      = set2D[i][0];
    table->SET2D    = NULL;
    table->SET1D    = set1D;
  }
  else if (table->NX == 1)
  {
    table->SET2D    = NULL;
    table->SET1D    = set2D[0];
    table->NX       = table->NY;
  }
  res           = (*func)(table,outload,inslope);
  table->SET2D  = set2D;
  table->SET1D  = NULL;
  table->NX     = nx;
  */
  

  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_thresholdOK()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int stm_thresholdOK(timing_table *table, double inslope,
                           double outload, double threshold)
{
  int        bx, ax, by, ay, x, y, res;
  double     x0, x1, y0, y1; 

  if (table)
  {
    x       = stm_getBeforeAndAfter(table->XRANGE,table->NX,inslope,&bx,&ax);
    y       = stm_getBeforeAndAfter(table->YRANGE,table->NY,outload,&by,&ay);
    res     = 0;
    if (x || y)
    {
      if (x && y)
      {
        res          = STM_THRESHOLD_MATCH_XY;
        if (table->SET2D[bx][by] == STM_NOVALUE)
          res       |= STM_THRESHOLD_NOMATCH;
      }
      else if (x)
      {
        res         |= STM_THRESHOLD_MATCH_X;
        if (ay == table->NY || by == -1)
          res       |= STM_THRESHOLD_NOMATCH;
        else
        {
          stm_getVar(table,&x0,&x1,&y0,&y1,bx,ax,by,ay,0);
          if ( stm_thresholdTest(x0,x1,outload,y0,y1) > threshold )
            res     |= STM_THRESHOLD_NOMATCH;
          else
            res     |= STM_THRESHOLD_MATCH;
        }
      }
      else
      {
        res         |= STM_THRESHOLD_MATCH_Y;
        if (ax == table->NX || bx == -1)
          res       |= STM_THRESHOLD_NOMATCH;
        else
        {
          stm_getVar(table,&x0,&x1,&y0,&y1,bx,ax,by,ay,1);
          if ( stm_thresholdTest(x0,x1,inslope,y0,y1) > threshold )
            res     |= STM_THRESHOLD_NOMATCH;
          else
            res     |= STM_THRESHOLD_MATCH;
        }
      }
    }
    else
    {
      if (ax == table->NX || ay == table->NY || by == -1 || bx == -1)
        res          = STM_THRESHOLD_NOMATCH;
      else 
      {
        stm_getVar(table,&x0,&x1,&y0,&y1,bx,ax,by,ay,0);
        if ( stm_thresholdTest(x0,x1,outload,y0,y1) > threshold )
          res        = STM_THRESHOLD_NOMATCH;
        else
        {
          stm_getVar(table,&x0,&x1,&y0,&y1,bx,ax,by,ay,1);
          if ( stm_thresholdTest(x0,x1,inslope,y0,y1) > threshold )
            res      = STM_THRESHOLD_NOMATCH;
          else
          {
            stm_getVar(table,&x0,&x1,&y0,&y1,bx,ax,by,ay,2);
            if ( stm_thresholdTest(x0,x1,outload,y0,y1) > threshold )
              res    = STM_THRESHOLD_NOMATCH;
            else
            {
              stm_getVar(table,&x0,&x1,&y0,&y1,bx,ax,by,ay,3);
              if ( stm_thresholdTest(x0,x1,inslope,y0,y1) > threshold )
                res  = STM_THRESHOLD_NOMATCH;
              else
                res |= STM_THRESHOLD_MATCH;
            }
          }
        }
      }
    }
  }
  else
    res              = STM_THRESHOLD_NOMATCH;

#ifdef _PRINT
  printf("test res = %x\n",res);
#endif
  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_getVar()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_getVar(timing_table *table, double *x0, double *x1,
                       double *y0, double *y1, int bx, int ax, int by,
                       int ay, int mode) 
{
  switch (mode)
  {
    case 0 :
         *x0    = table->YRANGE[by];
         *x1    = table->YRANGE[ay];
         *y0    = table->SET2D[bx][by];
         *y1    = table->SET2D[bx][ay];
         break;
    case 1 :
         *x0    = table->XRANGE[bx];
         *x1    = table->XRANGE[ax];
         *y0    = table->SET2D[bx][by];
         *y1    = table->SET2D[ax][by];
         break;
    case 2 :
         *x0    = table->YRANGE[by];
         *x1    = table->YRANGE[ay];
         *y0    = table->SET2D[ax][by];
         *y1    = table->SET2D[ax][ay];
         break;
    case 3 :
         *x0    = table->XRANGE[bx];
         *x1    = table->XRANGE[ax];
         *y0    = table->SET2D[bx][ay];
         *y1    = table->SET2D[ax][ay];
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_thresholdTest()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static double stm_thresholdTest(double x0, double x1, double x,
                                double y0, double y1)
{
  double      res, angle,X0 , Y0, X, Y, a;
  
  if (y0 == STM_NOVALUE || y1 == STM_NOVALUE)
    return STM_NOVALUE;
  else if (y0 == y1)
    return 0.0;
  else
  {
    X0      = x0/x1;
    Y0      = y0/y1;
    a       = (1-Y0)/(1-X0);
    angle   = atan(a)/(M_PI/2);
    Y       = log(pow(angle,2));
    X       = (2*(x-x0)/(x1-x0) - 1) * Y;
    res     = exp(-pow(X,2)) * 100 * sqrt(angle);
#ifdef _PRINT
    printf("angle : %4.2f° ",angle*90);
    printf("threshold : %4.2f%%\n",res);
#endif
    return res;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_getBeforeAndAfter()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int stm_getBeforeAndAfter(float *range, int max, double value,
                                 int *before, int *after)
{
  int        i, res = 0;

  *before   = -1;
  *after    = -1;

  for (i = 0; i < max ; i ++)
  {
    if (isInf(value,range[i]))
      break;
    *before = i;
    if ((res = isEqual(range[i],value)))
      break;
  }
  *after    = i;

#ifdef _PRINT
    printf("value %g, before %d %g, after %d %g %s\n",value,
           *before,range[*before],*after,range[*after],(res)?"OK":"NOT OK");
#endif

  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_storeInTable()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_storeInTable(timing_table **curtable, double inslope,
                             double outload, double delay, int mode)
{
  timing_table  *restable, *table;
  int            i, j, x, y, dx, dy;
  char           type = 0;

  table     = *curtable;

 /*
  if (table == CURRENT->SLOPE)
  {
    printf("before adding------------------------------------------------\n");
    stm_displayTable(table);
  }
  */
  if (mode == STM_THRESHOLD_MATCH_XY)
  {
    x                   = stm_findIndex(table->XRANGE,inslope, table->NX);
    y                   = stm_findIndex(table->YRANGE,outload, table->NY);
    table->SET2D[x][y]  = delay;
    type                = 'M';
    restable            = table;
//    stm_fillTbl(restable,x,y,type);
  }
  else if (table)
  {
    switch (mode)
    {
      case STM_THRESHOLD_MATCH_X :
           restable = stm_modtbl_create(table->NX,table->NY + 1,
                                        STM_INPUT_SLEW,STM_LOAD);
           x        = stm_findIndex(table->XRANGE,inslope, table->NX);
           y        = stm_copyRangeAndInsert(table->YRANGE, restable->YRANGE,
                                             outload,restable->NY);
           type     = 'Y';
           restable->XRANGE = table->XRANGE;
           table->XRANGE   = NULL;
           break;
      case STM_THRESHOLD_MATCH_Y :
           restable = stm_modtbl_create(table->NX + 1,table->NY,
                                        STM_INPUT_SLEW,STM_LOAD);
           x        = stm_copyRangeAndInsert(table->XRANGE, restable->XRANGE,
                                             inslope,restable->NX);
           y        = stm_findIndex(table->YRANGE,outload, table->NY);
           type     = 'X';
           restable->YRANGE = table->YRANGE;
           table->YRANGE   = NULL;
           break;
      default :
           restable = stm_modtbl_create(table->NX + 1,table->NY + 1,
                                        STM_INPUT_SLEW,STM_LOAD);
           x        = stm_copyRangeAndInsert(table->XRANGE,
                                             restable->XRANGE,
                                             inslope,restable->NX);
           y        = stm_copyRangeAndInsert(table->YRANGE,
                                             restable->YRANGE,
                                             outload,restable->NY);
           type     = 'N';
           break;
    }
    dx      = 0;
    dy      = 0;
    for (j = 0; j < restable->NY; j ++)
    {
      dx   = 0;
      if (j == y && mode != STM_THRESHOLD_MATCH_Y)
      {
        dy    = 1;
        continue;
      }
      for (i = 0; i < restable->NX; i ++)
      {
        if (i == x && mode != STM_THRESHOLD_MATCH_X)
        {
          dx    = 1;
          continue;
        }
        restable->SET2D[i][j] = table->SET2D[i - dx][j - dy];
      }
    }
    restable->SET2D[x][y] = delay; 
//    stm_fillTbl(restable,x,y,type);
    stm_modtbl_destroy(table);
  }
  else
  {
    restable    = stm_modtbl_create(STM_CACHE_SIZE,STM_CACHE_SIZE,
                                    STM_INPUT_SLEW,STM_LOAD);
    stm_modtbl_setXrangeval(restable,0,inslope);
    stm_modtbl_setYrangeval(restable,0,outload);
    stm_modtbl_set2Dval(restable,0,0,delay);
  }
  *curtable     = restable;   

/*  
  if (table == CURRENT->SLOPE)
  {
    printf("after complete\n");
    stm_displayTable(restable);
    printf("-------------------------------------------------------------\n");
  }
 */
}

/*}}}************************************************************************/
/*{{{                    stm_findIndex()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int stm_findIndex(float *range, double value, int size)
{
  int       i;

  for (i = 0; i < size; i ++)
    if (isEqual(range[i],value))
      return i;

  return -1;
}

/*}}}************************************************************************/
/*{{{                    stm_copyRangeAndInsert()                           */
/*                                                                          */
/* return index of new value                                                */
/*                                                                          */
/****************************************************************************/
static int stm_copyRangeAndInsert(float *range, float *new, double value,
                                  int size)
{
  int    i, res;

  for (i = 0; i < size-1 && range[i] < value; i ++)
    new[i]  = range[i];
  new[i]    = value;
  res       = i;
  for (i = res + 1; i < size; i ++)
    new[i]  = range[i-1];

  return res;
}

/*}}}************************************************************************/
/*{{{                    stm_getarcdef()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *stm_getarcdef(char *ins, char *local)
{
  char   temp[1024];
  
  sprintf(temp,"%s.%s",ins, local);
  
  return namealloc(temp);
}

/*}}}************************************************************************/
/*{{{                    stm_gettimingfunction_and_signalcapa()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int stm_gettimingfunction_and_signalcapa(char *output,
                                                timing_function **tf,
                                                double *capa)
{
  timing_function   *fct;
  char              *modelname;
  timing_model      *tmodel;
  losig_list        *ls;

  fct       = stm_get_current_arc();
  modelname = stm_getarcdef(fct->INS, fct->LOCALNAME);

  tmodel    = stm_getmodel (LATEST_GNS_RUN->FIGNAME, modelname);
  if (tmodel == NULL)
  {
    avt_errmsg(STM_API_ERRMSG, "001", AVT_ERROR, modelname);
    //fprintf(stderr,"stm model '%s' does not exist\n",modelname);
    return -1;
  }
  if (tmodel->UTYPE != STM_MOD_MODFCT)
  {
    avt_errmsg(STM_API_ERRMSG, "002", AVT_ERROR, modelname);
//    fprintf(stderr,"stm model '%s' is not a function\n",modelname);
    return -1;
  }
  *tf       = tmodel->UMODEL.FUNCTION;
  ls        = gen_corresp_sig(gen_makesignalname(output),CUR_CORRESP_TABLE);
  if (ls == NULL)
  {
    *capa   = 0.0;
    return 1;
  }
  if (ls->PRCN != NULL)
    *capa   = ls->PRCN->RCCAPA*1e3;
  else
    *capa   = 0.0;
  
  return 0;
}

/*}}}************************************************************************/
/*{{{                    Filling tables utilities                           */
/****************************************************************************/
/*{{{                    stm_fillTbl()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_fillTbl(timing_table *tbl, int xind, int yind, char type)
{
  int        beg, end, up ,down;

  switch (type)
  {
    case 'X' :
         stm_fillFindLeftnRight(tbl,xind,yind,&beg,&end);
         stm_fillLine(tbl,beg,xind,yind);
         stm_fillLine(tbl,xind,end,yind);
         stm_fillNewCol(tbl,xind);
         break;
    case 'Y' : 
         stm_fillFindUpnDown(tbl,xind,yind,&up,&down);
         stm_fillCol(tbl,up,yind,xind);
         stm_fillCol(tbl,yind,down,xind);
         stm_fillNewLine(tbl,yind);
         break;
    case 'N' : // New
         stm_fillNewCol(tbl,xind);
         stm_fillNewLine(tbl,yind);
         break;
    case 'M' : // Match
         stm_fillFindLeftnRight(tbl,xind,yind,&beg,&end);
         stm_fillLine(tbl,beg,xind,yind);
         stm_fillLine(tbl,xind,end,yind);
         stm_fillFindUpnDown(tbl,xind,yind,&up,&down);
         stm_fillCol(tbl,up,yind,xind);
         stm_fillCol(tbl,yind,down,xind);
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_fillFindUpnDown()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_fillFindUpnDown(timing_table *tbl, int xind, int yind, 
                                int *up, int *down)
{
  int        i;
  
  *up       = -1;
  *down     = -1;
  for (i = 0; i < tbl->NY; i ++)
    if (tbl->SET2D[xind][i] != STM_NOVALUE)
    {
      if (i < yind)
        *up  = i;
      else if (i > yind)
      {
        *down  = i;
        break;
      }
    }
}

/*}}}************************************************************************/
/*{{{                    stm_fillCol()                                      */
/*                                                                          */
/* fill a line like this :     X                                            */
/*                             .                                            */
/*                             .                                            */
/*                             X                                            */
/*                                                                          */
/****************************************************************************/
static void stm_fillCol(timing_table *tbl, int up, int down, int xind)
{
  if (up != -1 && down != -1)
  {
    float        x0, x1, y0, y1, x;
    int          i;
    
    x0       = tbl->YRANGE[up];
    y0       = tbl->SET2D[xind][up];
    x1       = tbl->YRANGE[down];
    y1       = tbl->SET2D[xind][down];
    for (i = up; i < down; i ++)
    {
      x      = tbl->YRANGE[i];
      if (stm_thresholdTest(x0,x1,x,y0,y1) < THRESHOLD)
        tbl->SET2D[xind][i] = stm_modtbl_interpol1DCalc(x0,x1,y0,y1,x);
    }

  }
}

/*}}}************************************************************************/
/*{{{                    stm_fillNewLine()                                  */
/*                                                                          */
/* fill a new column like this : XXX                                        */
/*                               ...                                        */
/*                               XXX                                        */
/*                                                                          */
/****************************************************************************/
static void stm_fillNewLine(timing_table *tbl,int yind)
{
  if (yind != 0 && yind != tbl->NY-1)
  {
    float          x0, x1, y0, y1, x;
    int            i;

    for (i = 0; i < tbl->NX; i ++)
      if ( tbl->SET2D[i][yind-1] != STM_NOVALUE
           && tbl->SET2D[i][yind+1] != STM_NOVALUE)
      {
        x0     = tbl->YRANGE[yind-1];
        y0     = tbl->SET2D[i][yind-1];
        x1     = tbl->YRANGE[yind+1];
        y1     = tbl->SET2D[i][yind+1];
        x      = tbl->YRANGE[i];
        if (stm_thresholdTest(x0,x1,x,y0,y1) < THRESHOLD)
          tbl->SET2D[i][yind] = stm_modtbl_interpol1DCalc(x0,x1,y0,y1,x);
      }
  }
}

/*}}}************************************************************************/
/*{{{                    stm_fillFindLeftnRight()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void stm_fillFindLeftnRight(timing_table *tbl, int xind, int yind, 
                                   int *beg, int *end)
{
  int        i;
  
  *beg      = -1;
  *end      = -1;
  for (i = 0; i < tbl->NX; i ++)
    if (tbl->SET2D[i][yind] != STM_NOVALUE)
    {
      if (i < xind)
        *beg  = i;
      else if (i > xind)
      {
        *end  = i;
        break;
      }
    }
}

/*}}}************************************************************************/
/*{{{                    stm_fillLine()                                     */
/*                                                                          */
/* fill a line like this :     X.....X                                      */
/*                                                                          */
/****************************************************************************/
static void stm_fillLine(timing_table *tbl, int beg, int end, int yind)
{
  if (beg != -1 && end != -1)
  {
    float        x0, x1, y0, y1, x;
    int          i;
    
    x0       = tbl->XRANGE[beg];
    y0       = tbl->SET2D[beg][yind];
    x1       = tbl->XRANGE[end];
    y1       = tbl->SET2D[end][yind];
    for (i = beg; i < end; i ++)
    {
      x      = tbl->XRANGE[i];
      if (stm_thresholdTest(x0,x1,x,y0,y1) < THRESHOLD)
        tbl->SET2D[i][yind] = stm_modtbl_interpol1DCalc(x0,x1,y0,y1,x);
    }

  }
}

/*}}}************************************************************************/
/*{{{                    stm_fillNewCol()                                   */
/*                                                                          */
/* fill a new column like this : X.X                                        */
/*                               X.X                                        */
/*                                                                          */
/****************************************************************************/
static void stm_fillNewCol(timing_table *tbl,int xind)
{
  if (xind != 0 && xind != tbl->NX-1)
  {
    float          x0, x1, y0, y1, x;
    int            i;

    for (i = 0; i < tbl->NY; i ++)
      if ( tbl->SET2D[xind-1][i] != STM_NOVALUE
           && tbl->SET2D[xind+1][i] != STM_NOVALUE)
      {
        x0     = tbl->XRANGE[xind-1];
        y0     = tbl->SET2D[xind-1][i];
        x1     = tbl->XRANGE[xind+1];
        y1     = tbl->SET2D[xind+1][i];
        x      = tbl->XRANGE[i];
        if (stm_thresholdTest(x0,x1,x,y0,y1) < THRESHOLD)
          tbl->SET2D[xind][i] = stm_modtbl_interpol1DCalc(x0,x1,y0,y1,x);
      }
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    stm_displayRange()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_displayRange(timing_table *table)
{
  if (table)
  {
    int        i, j, k, nb = 8, x;

    for (k = 0; k <= (table->NX)/nb ; k ++)
    {
      x = k*nb + ((k < (table->NX)/nb) ? nb : (table->NX - k*nb)%nb);
      printf("| delay |");
      for (i = k*nb; i < x; i ++)
        printf(" %+e",table->XRANGE[i]);
      printf("\n");

      printf("|  capa |");
      for (j = 0; j < table->NY; j ++)
        printf(" %+e",table->YRANGE[j]);
      printf("\n");
    }
  }
}

/*}}}************************************************************************/
/*{{{                    stm_test_threshold()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_test_threshold()
{
  double      res,  y, x0, x1, y0, y1, x, X, Y, nb;
  FILE       *f;

  f = fopen("plot.dat","wt");
  x0 = 0;
  x1 = 100;
  y0 = 0;
  y1 = 90;
  nb = 50;

  for (y = y0; y <= y1; y += (y1-y0)/nb)
  {
    for (x = x0; x <= x1; x += (x1-x0)/nb) 
    {
      Y   = y/90;
      //      Y   = log(pow(Y,(1.0-1/y)));
      Y   = log(pow(Y,2));
      X   = (2*(x-x0)/(x1-x0) - 1);
      X   = X*Y;
      res = exp(-pow(X,2)) * 100 * pow(y/90,1.0/3);
      //      fprintf(f,"%4.3g %4.1f %7.2f\n",x,y,res);
      //      fprintf(f,"\n");
    }
    fprintf(f,"\n");
  }
  EXIT(0);

}

/*}}}************************************************************************/
/*{{{                    Math Function                                      */
/****************************************************************************/
/*{{{                    stm_arround()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double stm_arround(double a, double pres)
{
  double     res;
  long       cut;

  res   = a / pres;
  cut   = (long)(res + 0.5);
  res   = cut * pres;

  return res;
}

/*}}}************************************************************************/
/*{{{                    isEqual()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int isEqual(float a, float b)
{
  if (b == 0)
    if (a == 0)
      return 1;
    else
      return ( fabs((b/a) - 1) < STM_ACCURACY);
  else
    return ( fabs((a/b) - 1) < STM_ACCURACY);
}

/*}}}************************************************************************/
/*{{{                    isInf()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int isInf(float a, float b)
{
  if (isEqual(a,b))
    return 0;
  else
    return a < b;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
