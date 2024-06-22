/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEG_GEN Version 1.00                                        */
/*    Fichier : beg_gen.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "beg_gen.h"

// #define BEG_ERR_SING_N_VECT   "001"
// #define BEG_ERR_BETYPE        "002"
// #define BEG_ERR_OUT_AS_IN     "003"
// #define BEG_ERR_NULCND        "004"
// #define BEG_ERR_CONFLICT_VECT "005"
// #define BEG_ERR_SIZE          "006"
// #define BEG_ERR_PARSER        "007"
// #define BEG_ERR_POR           "008"
// #define BEG_ERR_BIBUS         "009"
// #define BEG_ERR_WAY           "010"
// #define BEG_ERR_SELECT        "011"
// #define BEG_ERR_EXISTSIG      "012"
// #define BEG_ERR_UNKNOWN       "016"
// 
// #define BEG_WAR_UNDEF_SIG     "013"
// #define BEG_WAR_INOUTSIG      "014"
// #define BEG_WAR_WAYENV        "015"
// #define BEG_WAR_UNKNOWN       "016"
    
// error and warning
//long BEG_ERR_SING_N_VECT     ;
//long BEG_ERR_BETYPE          ;
//long BEG_ERR_OUT_AS_IN       ;
//long BEG_ERR_NULCND          ;
//long BEG_ERR_CONFLICT_VECT   ;
//long BEG_ERR_SIZE            ;
//long BEG_ERR_PARSER          ;
//long BEG_ERR_POR             ;
//long BEG_ERR_BIBUS           ;
//long BEG_ERR_WAY             ;
//long BEG_ERR_SELECT          ;
//long BEG_ERR_EXISTSIG        ;
//long BEG_ERR_UNKNOWN         ;
//
//long BEG_WAR_UNDEF_SIG       ;
//long BEG_WAR_INOUTSIG        ;
//long BEG_WAR_WAYENV          ;
//long BEG_WAR_UNKNOWN         ;
    
/* environment variables    */
static int               BEG_ALLOW_SELECT;
static int               BEG_TRACE;
static int               BEG_USER_WAY;
static int               BEG_COMPRESS_MAX;

/* global variables         */
static befig_list       *BEG_HEAD_BEFIG = NULL;
static befig_list       *BEG_WORK_BEFIG = NULL;
static BEG_OBJ           BEG_WORKLIST   = NULL;
static ht               *BEG_POR_TABLE  = NULL;
static ht               *BEG_SIG_TABLE  = NULL;
static ht               *BEG_GUE_TABLE  = NULL; /* found from loop          */
static ht               *BEG_RIN_ADJUS  = NULL; /* adjusted Rin             */
static ht               *BEG_RIN_TABLE  = NULL;
static ht               *BEG_CON_TABLE  = NULL; /* conflict vector ht       */
static ptype_list       *BEG_CON_LIST   = NULL; /* conflict vector ht       */
static ht               *BEG_DEL_TABLE  = NULL; /* deleted biabl list       */
static ht               *BEG_FIG_TABLE  = NULL; /* ht for befig             */

static beg_obj_list     *BEG_HEAD_OBJ   = NULL;
static int               BEG_COMPACT    = 0;


static int               BEG_RIN_TREAT  = 0;
//static int               BEG_DONT_TOUCH = 0;
static int               BEG_BLOCK_INV  = 0;
static int               BEG_PHASE_1;
static int               BEG_PHASE_2;
static int               BEG_IS_CHECKED;
static int               BEG_ADD_INDEX  = 1;
static int               BEG_SORT_DONE;
static int               BEG_CONF_CPT;

/****************************************************************************/
/*{{{                    Functions declaration                              */
/****************************************************************************/
/*{{{                    Global Functions                                   */
/****************************************************************************/
int         beg_isDeletedBiabl             (biabl_list *biabl);
void        beg_resolutionFunc             (char *name, int index, biabl_list *new);
void        beg_testBerin                  (void);
void        rtest                          (void);
void        beg_delBefig                   (befig_list *fig);

/*}}}************************************************************************/
/*{{{                    Static Functions                                   */
/****************************************************************************/
static BEG_OBJ           beg_def_bepor                  (char *name, char  direction, char  type);
static BEG_OBJ           beg_def_beout                  (char *name, chain_list *abl, unsigned int time,int,int, long flags);
static BEG_OBJ           beg_def_beaux                  (char *name, chain_list *abl, unsigned int time,int,int, long flags);
static BEG_OBJ           beg_def_bebux                  (char *name, biabl_list *biabl, char  type, long flags, long biflags);
static BEG_OBJ           beg_def_bebus                  (char *name, biabl_list *biabl, char  type, long flags, long biflags);
static BEG_OBJ           beg_def_bereg                  (char *name, biabl_list *biabl, char  type, long flags, long biflags);
static biabl_list       *beg_getBiabl                   (BEG_OBJ obj);
static int               beg_getMaxObj                  (BEG_OBJ sig, int lt, int rt);
static int               beg_getMinObj                  (BEG_OBJ sig, int lt, int rt);
static BEG_OBJ           beg_isPor                      (char *name);
static BEG_OBJ           beg_isInternalSig              (char *name);
static void              beg_newPor                     (char *name, BEG_OBJ obj);
static void              beg_newSig                     (char *name, BEG_OBJ obj);
//static chain_list       *beg_isAllDeclared              (chain_list *expr,char *var1,char *var2);
static void              beg_addBiablToVectBiabl        (BEG_OBJ obj, int left, int right, biabl_list *biabl);
static void              beg_adjustDomain               (BEG_OBJ obj, int left, int right);
static int               beg_chkCovering                (vectbiabl_list *vbiabl, int left, int right);
static void              beg_resolveExistingSig         (int *oldleft, int *oldright, chain_list **oldabl, int left, int right, chain_list *abl);
static int               beg_genPutBinInBuf             (char *var, char *buf);
static char             *beg_mkName                     (char *opname, int left, int right);
static int               beg_doITreatThisOne            (BEG_OBJ sig, int left, int right);
static void              beg_verifyAux                  (void);
static void              beg_convertAuxInOut            (befig_list *befig, void *sig, long mode);
static void              beg_addBiablToBEG_OBJ          (BEG_OBJ obj,biabl_list *biabl);
static biabl_list       *beg_addBiablToBiablList        (biabl_list *list,biabl_list *biabl);
static void              beg_addbiloop                  (biabl_list *biabl,char *index,char *var1,char *var2);
static void              beg_addBerins                  (chain_list *berins,BEG_OBJ obj);
static inline void       beg_addBerinsInt(char *berin,BEG_OBJ obj);
static void              beg_addBerin                   (berin_list *berin,BEG_OBJ obj);
static void              beg_addUniqChain               (chain_list **head,void *data);
static void              beg_chk_befig                  (void);
static char             *beg_get_loopvar                (char *signame, char **name, char mode);
//static char             *beg_guessRange                 (char *name,char **opname,int *left,int *right,char *var1,char *var2,char mode);
static int               beg_getMin                     (int nb, ... );
static int               beg_getMax                     (int nb, ... );
static void              beg_invertBound                (BEG_OBJ obj);
static void              beg_invertIntArg               (int *a, int *b);
static void              beg_invertLongArg              (long *a, long *b);
static int               beg_isSameWay                  (BEG_OBJ obj,int left,int right);
static char             *beg_mkAblName                  (char *opname,int left,int right);
static char             *beg_changeName                 (char *bname, char *name, char *loopvar, int curindex, int step);
static inline void       eatRenamed                     (befig_list *eaten);
static void              beg_eatPor                     (bepor_list *por);
static void              beg_eatOut                     (beout_list *out);
static void              beg_eatAux                     (beaux_list *aux);
static void              beg_eatBus                     (bebus_list *bus);
static void              beg_eatReg                     (bereg_list *reg);
static void              beg_eatBux                     (bebux_list *bux);
static void              beg_eatVectPor                 (bevectpor_list *vpor);
static void              beg_eatVectOut                 (bevectout_list *vout);
static void              beg_eatVectAux                 (bevectaux_list *vaux);
static void              beg_eatVectBus                 (bevectbus_list *vbus);
static void              beg_eatVectBux                 (bevectbux_list *vbux);
static void              beg_eatVectReg                 (bevectreg_list *vreg);
static void              beg_eatBiabl                   (BEG_OBJ (*func)(char*,biabl_list*,char,long,long),char *name,biabl_list *biabl,long flags);
static void              beg_eatVectBiabl               (BEG_OBJ (*func)(char*,biabl_list*,char,long,long),char *name,vectbiabl_list *vbiabl,long flags);
static BEG_OBJ           beg_defSig                     (char *name,chain_list *abl,unsigned int time,int,int,long);
static BEG_OBJ           beg_defBus                     (char *name,biabl_list *biabl,char type, long flags, long biflags);
//static vectbiabl_list   *beg_sortVectBiablGetSortedTable(vectbiabl_list **vbtable);
//static vectbiabl_list  **beg_sortVectBiablDoTable       (vectbiabl_list *vbiabl, int cpt, int *way, int *nb);
static int               beg_sortVectBiablCompare       (const void *vb1, const void *vb2);
static vectbiabl_list   *beg_sortVectBiabl              (vectbiabl_list *vbiabl);
static int               beg_way                        (int left,int right);

static void              beg_freeLD                     (beg_ld_list *ld);
static beg_ld_list      *beg_addLD                      (beg_ld_list *next, biabl_list *base, chain_list *bascs, chain_list *basvs, vectbiabl_list *vbase, int step, loop_list *loop);
static int               beg_applyLD                    (beg_ld_list *ld);
static int               beg_isInt                      (char *str);
static beg_obj_list     *beg_allocEnv                   (beg_obj_list *next);
static void              beg_newEnv                     (char *name);
static beg_obj_list     *beg_freeEnv                    (beg_obj_list *obj);
static void              beg_restoreEnv                 (beg_obj_list *obj);
static void              beg_saveEnv                    (beg_obj_list *obj);
static beg_obj_list     *beg_getEnvByFigName            (char *figname);
static chain_list       *beg_cmpSupport                 (chain_list *bass, chain_list *s, char *loopvar, int cindex, int step);
static void              beg_freeSupport                (chain_list *support);
static void              beg_cleanFreeSupport           (chain_list *basesupport,chain_list *support);
static void              beg_modifyAblLoop              (chain_list *abl, chain_list *support);
static int               beg_isSameByLoopVar            (biabl_list *bbiabl, biabl_list *biabl, chain_list *bascs , chain_list *basvs,chain_list **refcs, chain_list **refvs,char *lvar, int cpt, int step);
static char             *beg_getLongName                (BEG_OBJ obj);
static void              beg_eatMsg                     (bemsg_list *msg);
static void              beg_eatRin                     (berin_list **rin);
static void              beg_performVReg                (bevectreg_list *vreg, vectbiabl_list *(*func)(vectbiabl_list*));
static void              beg_performVBux                (bevectbux_list *vbux, vectbiabl_list *(*func)(vectbiabl_list*));
static void              beg_performVBus                (bevectbus_list *vbus, vectbiabl_list *(*func)(vectbiabl_list*));
static void              beg_perform                    (vectbiabl_list *(*func)(vectbiabl_list*));
//static void              beg_getRangeFLoop              (char **opname,int *left,int *right,char *expr, char *var1,char *var2);
static vectbiabl_list   *beg_detectLoopI                (vectbiabl_list *vbiabl);
static biabl_list       *beg_detectLoopII               (biabl_list *biabl);
static void              beg_addGuessedRange            (char *opname, int left);
static int               beg_mergeAtom                  (chain_list *atm1,chain_list *atm2);
static void              beg_invertObj                  (BEG_OBJ obj);
static chain_list       *beg_getAbl                     (BEG_OBJ obj);
static void              beg_newVectBiabl               (BEG_OBJ obj, vectbiabl_list *vbiabl);
static int               beg_doITreatThisRin            (BEG_OBJ obj, int left, int right);

static int               beg_wayStrict                  (int left,int right);
static void              beg_treatConflictError         (BEG_OBJ obj,int left,int right,biabl_list *biabl);
static void              beg_reloadConflictError        (void);
static vectbiabl_list   *beg_detectLoop                 (vectbiabl_list *vbiabl);
static vectbiabl_list   *beg_detectLoopI                (vectbiabl_list *vbiabl);

static chain_list       *beg_getAbl                     (BEG_OBJ obj);
static char             *beg_doLoopVar                  (loop_list *loop);
//static void              beg_verifyVectPor            (befig_list *befig);
//static chain_list       *beg_genAblFBitStr            (char *bitstr);
//static int               beg_canInsertAbl             (int ol,int or,int l,int r,chain_list *abl);
//static void              beg_eatFigure                (befig_list *eater,befig_list *eaten);
//static beaux_list       *beg_convertOutToAux          (beaux_list *next,beout_list *out);
//static bevectaux_list   *beg_convertVectOutToAux      (bevectaux_list *next,bevectout_list *vout);
//static bebux_list       *beg_convertBusToBux          (bebux_list *next,bebus_list *bus);
//static bevectbux_list   *beg_convertVectBusToBux      (bevectbux_list *next,bevectbus_list *vbus);

//static vectbiabl_list   *beg_holeDetection            (vectbiabl_list *vbiabl);
static void              beg_specialFreVBiabl         (vectbiabl_list *head, vectbiabl_list *last);
//static void              beg_newAbl                   (BEG_OBJ obj, chain_list *abl);
static void              beg_calcRange(eqt_ctx *ctx, char *expr, loop_list *loop, long *left, long *right);
static chain_list       *beg_IsAllDeclared(chain_list *expr,loop_list *loop);

static biabl_list       *beg_freeBiabl(biabl_list *biabl);
static vectbiabl_list   *beg_delVbiablHead(vectbiabl_list *vbiabl);
static int               beg_isBiablHZ(biabl_list *biabl);
static int               beg_addBiablToBiablListInt(biabl_list *list,biabl_list *biabl);
static biabl_list       *beg_sortElseB(biabl_list *biabl);
static vectbiabl_list   *beg_sortElseVB(vectbiabl_list *vbiabl);
static void              beg_sortElse();
static void              beg_newBiabl(BEG_OBJ obj, biabl_list *biabl);
static int               beg_hasPrecede(biabl_list *biabl);

static void beg_freeAllEnv();

static void delFig(void *key);
static beg_obj_list *getFig(void *key);
static void newFigList(void);
static void addFigInList(void *key, beg_obj_list *obj);

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    BEG message                                        */
/****************************************************************************/
/*{{{                    beg_initError()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_initError(void)
{
/*  static int     init       = 0;

  if (!init)
  {
    BEG_ERR_SING_N_VECT   = (long)"Attempt to merge bit vector and single bit";
    BEG_ERR_BETYPE        = (long)"Internal error";
    BEG_ERR_OUT_AS_IN     = (long)"Connector declared in, used as out";
    BEG_ERR_NULCND        = (long)"Null condition";
    BEG_ERR_CONFLICT_VECT = (long)"Conflicting vector";
    BEG_ERR_SIZE          = (long)"Expression and variable has different size";
    BEG_ERR_PARSER        = (long)"Parser Failure";
    BEG_ERR_POR           = (long)"Conflicting declaration and use";
    BEG_ERR_BIBUS         = (long)"Conflicting bus use";
    BEG_ERR_WAY           = (long)"Direction of declaration conflicts with use";
    BEG_ERR_SELECT        = (long)"Selected signal, vector expression not allowed";
    BEG_ERR_EXISTSIG      = (long)"Attempt to insert a signal into a defined signal";
    BEG_ERR_UNKNOWN       = (long)"Unknown error";

    BEG_WAR_UNDEF_SIG     = (long)"Vector incompletely defined, made external";
    BEG_WAR_INOUTSIG      = (long)"Connector declared out, used as in";
    BEG_WAR_WAYENV        = (long)"Bad value for 'BEG_USER_WAY', accepted 'to' or 'downto'";
    BEG_WAR_UNKNOWN       = (long)"Unknown warning";

    init                  = 1;
  }*/
  ;
}

/*}}}************************************************************************/
/*{{{                    beg_error()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_error(char * error, char *text, ... )
{
  va_list    arg;
  char       buf[1024];

  if (!error || !BEG_WORK_BEFIG->NAME)
    avt_errmsg(BEG_ERRMSG,error,AVT_ERROR);
    
  va_start(arg,text);
  if (text)
  {
    vsprintf(buf,text,arg);
    avt_errmsg(BEG_ERRMSG,error,AVT_ERROR,BEG_WORK_BEFIG->NAME,buf);
  }
  else
    avt_errmsg(BEG_ERRMSG,error,AVT_ERROR,BEG_WORK_BEFIG->NAME,"");
  va_end(arg);

  EXIT (0);

}

/*}}}************************************************************************/
/*{{{                    beg_warning()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_warning(char * warning, char *text, ... )
{
  va_list    arg;
  char       buf[1024];

  va_start(arg,text);
  if (text)
  {
    vsprintf(buf,text,arg);
    avt_errmsg(BEG_ERRMSG,warning,AVT_WARNING,BEG_WORK_BEFIG->NAME,buf);
  }
  else
    avt_errmsg(BEG_ERRMSG,warning,AVT_ERROR,BEG_WORK_BEFIG->NAME,"");
  va_end(arg);
}

/*}}}************************************************************************/
/*{{{                    beg_trace()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_trace(char *text, ... )
{
  if (BEG_TRACE)
  {
    va_list    arg;
    char       buf[1024];

    va_start(arg,text);
    if (text)
    {
      vsprintf(buf,text,arg);
      avt_errmsg(BEG_ERRMSG,BEG_TRA_X,AVT_WARNING,BEG_WORK_BEFIG->NAME,buf);
    }
    else
      avt_errmsg(BEG_ERRMSG,BEG_TRA_X,AVT_WARNING,BEG_WORK_BEFIG->NAME,"");
    va_start(arg,text);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    BEG environment                                    */
/****************************************************************************/
/*{{{                    HT management                                      */
/****************************************************************************/
/*{{{                    addFigInList()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void addFigInList(void *key, beg_obj_list *obj)
{
  addhtitem(BEG_FIG_TABLE,key,(long)obj);
}

/*}}}************************************************************************/
/*{{{                    newFigList()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void newFigList(void)
{
  if (!BEG_FIG_TABLE)
    BEG_FIG_TABLE   = addht(50);
}

/*}}}************************************************************************/
/*{{{                    getFig()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beg_obj_list *getFig(void *key)
{
  beg_obj_list  *res;
  
  newFigList();
  
  res   = (beg_obj_list*)gethtitem(BEG_FIG_TABLE,key);
  if (res == (beg_obj_list*)EMPTYHT || res == (beg_obj_list*)DELETEHT)
    return NULL;
  else
    return res;
}

/*}}}************************************************************************/
/*{{{                    delFig()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void delFig(void *key)
{
  delhtitem(BEG_FIG_TABLE,key);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    beg_allocEnv()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beg_obj_list *beg_allocEnv(beg_obj_list *next)
{
  beg_obj_list  *new;

  new = mbkalloc(sizeof(struct beg_obj));
  
  new->NEXT     = next;
  new->FIG      = NULL;
  new->WORKLIST = NULL;
  new->POR      = addht(BEG_HTSIZE);
  new->SIG      = addht(BEG_HTSIZE);
  new->GUE      = addht(BEG_HTSIZE);
  new->RINADJUS = addht(BEG_HTSIZE);
  new->RIN      = addht(BEG_HTSIZE);
  new->CON      = addht(BEG_HTSIZE);
  new->DEL      = addht(BEG_HTSIZE);
  new->CONLIST  = NULL;
  new->RINTREAT = 0;

  return new;
}

/*}}}************************************************************************/
/*{{{                    beg_freeEnv()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beg_obj_list *beg_freeEnv(beg_obj_list *obj)
{
  beg_obj_list  *res;

  res = obj->NEXT;

  freeptype (obj->WORKLIST);
  delht     (obj->POR);
  delht     (obj->SIG);
  delht     (obj->GUE);
  delht     (obj->RIN);
  delht     (obj->CON);
  freeptype (obj->CONLIST);
  delht     (obj->RINADJUS);
  delht     (obj->DEL);
  mbkfree   (obj);

  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_terminate()                                    */
/*                                                                          */
/* same as beg_freeAllEnv but keep the behavioral figure fig                */
/*                                                                          */
/****************************************************************************/
void beg_terminate(befig_list *fig)
{
  if (BEG_HEAD_OBJ)
  {
    delFig(BEG_HEAD_OBJ->FIG);
    delFig(BEG_HEAD_OBJ->FIG->NAME);

    if (BEG_HEAD_OBJ->FIG != fig)
    {
      beg_trace("unused genius behavioral figure",BEG_HEAD_OBJ->FIG->NAME);
      beh_frebefig(BEG_HEAD_OBJ->FIG);
    }
    BEG_HEAD_OBJ    = beg_freeEnv(BEG_HEAD_OBJ);
    beg_terminate(fig);
    if (BEG_FIG_TABLE)
    {
      delht(BEG_FIG_TABLE);
      BEG_FIG_TABLE = NULL;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    beg_freeAllEnv()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_freeAllEnv()
{
  if (BEG_HEAD_OBJ)
  {
    delFig(BEG_HEAD_OBJ->FIG);
    delFig(BEG_HEAD_OBJ->FIG->NAME);

    beh_frebefig(BEG_HEAD_OBJ->FIG);
    BEG_HEAD_OBJ    = beg_freeEnv(BEG_HEAD_OBJ);
    beg_freeAllEnv();
    if (BEG_FIG_TABLE)
    {
      delht(BEG_FIG_TABLE);
      BEG_FIG_TABLE = NULL;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    beg_restoreEnv()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_restoreEnv(beg_obj_list *obj)
{
  if (obj)
  {
    BEG_WORK_BEFIG  = obj->FIG;
    BEG_WORKLIST    = obj->WORKLIST;
    BEG_POR_TABLE   = obj->POR;
    BEG_SIG_TABLE   = obj->SIG;
    BEG_GUE_TABLE   = obj->GUE;
    BEG_RIN_TABLE   = obj->RIN;
    BEG_CON_TABLE   = obj->CON;
    BEG_CON_LIST    = obj->CONLIST;
    BEG_RIN_ADJUS   = obj->RINADJUS;
    BEG_RIN_TREAT   = obj->RINTREAT;
    BEG_DEL_TABLE   = obj->DEL;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_saveEnv()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_saveEnv(beg_obj_list *obj)
{
  obj->FIG      = BEG_WORK_BEFIG;
  obj->WORKLIST = BEG_WORKLIST;
  obj->POR      = BEG_POR_TABLE;
  obj->SIG      = BEG_SIG_TABLE;
  obj->GUE      = BEG_GUE_TABLE;
  obj->RIN      = BEG_RIN_TABLE;
  obj->CON      = BEG_CON_TABLE;
  obj->CONLIST  = BEG_CON_LIST;
  obj->RINADJUS = BEG_RIN_ADJUS;
  obj->RINTREAT = BEG_RIN_TREAT;
  obj->DEL      = BEG_DEL_TABLE;
}

/*}}}************************************************************************/
/*{{{                    beg_init()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_init(void)
{
  BEG_HEAD_BEFIG      = NULL;
  BEG_WORK_BEFIG      = NULL;
  BEG_WORKLIST        = NULL;
  BEG_POR_TABLE       = NULL;
  BEG_SIG_TABLE       = NULL;
  BEG_GUE_TABLE       = NULL;
  BEG_RIN_ADJUS       = NULL;
  BEG_RIN_TABLE       = NULL;
  BEG_CON_TABLE       = NULL;
  BEG_CON_LIST        = NULL;
  BEG_DEL_TABLE       = NULL;
  BEG_RIN_TREAT       = 0;
  BEG_BLOCK_INV       = 0;
  BEG_ADD_INDEX       = 1;
  
  BEG_HEAD_OBJ        = NULL;
  BEG_SIG_TABLE       = NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_delEnv()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_delEnv(befig_list *fig)
{
  beg_obj_list  *obj;
#if 0
  beg_obj_list  *prev, head;
  head.NEXT     = BEG_HEAD_OBJ;
  head.FIG      = NULL;
#endif

  newFigList();

#if 0
  for (prev = obj = &head; obj && (obj->FIG != fig); obj = obj->NEXT)
    prev        = obj;
  if (obj)
    prev->NEXT  = beg_freeEnv(obj);
  BEG_HEAD_OBJ  = head.NEXT;
#else
  obj=getFig(fig);
  if (obj)
    beg_freeEnv(obj);
  BEG_HEAD_OBJ  =NULL;
#endif

  delFig(fig);
  delFig(fig->NAME);
}

/*}}}************************************************************************/
/*{{{                    beg_newEnv()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_newEnv(char *name)
{
  BEG_HEAD_OBJ = beg_allocEnv(BEG_HEAD_OBJ);
  beg_restoreEnv(BEG_HEAD_OBJ);
  
  BEG_WORK_BEFIG      = beh_addbefig(NULL,name);
  BEG_HEAD_OBJ->FIG   = BEG_WORK_BEFIG;

  newFigList();
  addFigInList(name,BEG_HEAD_OBJ);
  addFigInList(BEG_WORK_BEFIG,BEG_HEAD_OBJ);
}

/*}}}************************************************************************/
/*{{{                    beg_getOneEnvFromFigName()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_getOneEnvFromFigName(char *name)
{
  beg_obj_list  *obj;

  obj               = beg_getEnvByFigName(name);

  if (obj)
    beg_restoreEnv(obj);
  else
    beg_newEnv(name);
}

/*}}}************************************************************************/
/*{{{                    beg_getEnv()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beg_obj_list *beg_getEnv(befig_list *fig)
{
  beg_obj_list  *obj;


#if 0
  if (!(obj = getFig(fig)))
  {
    for (obj = BEG_HEAD_OBJ; obj && (obj->FIG != fig); obj = obj->NEXT)
      ;
    if (obj)
      addFigInList(fig,obj);
  }
#else
  obj = getFig(fig);
#endif
  return obj;
}

/*}}}************************************************************************/
/*{{{                    beg_getEnvByFigName()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beg_obj_list *beg_getEnvByFigName(char *figname)
{
  beg_obj_list  *obj;

#if 0
  if (!(obj = getFig(figname)))
  {
    for (obj = BEG_HEAD_OBJ; obj; obj = obj->NEXT)
      if (obj->FIG && obj->FIG->NAME == figname)
        break;
    if (obj)
      addFigInList(figname,obj);
  }
#else
  obj = getFig(figname);
#endif
  return obj;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    BEG_OBJ manipulation                               */
/****************************************************************************/
/*{{{                    makeConIn()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
makeConIn(char *org, BEG_OBJ obj)
{
  switch (*org)
  {
    case 'O':
         *org   = 'B';
         if (obj)
           beg_warning(BEG_WAR_INOUTSIG,beg_getName(obj)) ;
         break ;
    case 'X':
    default:
         *org   = 'I';
    case 'I': 
    case 'B': 
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    makeConOut()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
makeConOut(char *org, BEG_OBJ obj)
{
  switch (*org)
  {
    case 'I':
         *org   = 'B';
         if (obj)
           beg_warning(BEG_WAR_INOUTSIG,beg_getName(obj)) ;
         break ;
    case 'X':
    default:
         *org   = 'O';
         break;
    case 'O': 
    case 'B':
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_isDeletedBiabl()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beg_isDeletedBiabl(biabl_list *biabl)
{
  return (gethtitem(BEG_DEL_TABLE,biabl) == EMPTYHT) ? 0 : 1;
}

/*}}}************************************************************************/
/*{{{                    beg_addBerin()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_addBerin(berin_list *berin,BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTOUT :
         beg_addUniqChain(&berin->OUT_VEC,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_SINGOUT :
         beg_addUniqChain(&berin->OUT_REF,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_VECTAUX :
         beg_addUniqChain(&berin->AUX_VEC,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_SINGAUX :
         beg_addUniqChain(&berin->AUX_REF,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_VECTBUS :
         beg_addUniqChain(&berin->BUS_VEC,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_SINGBUS :
         beg_addUniqChain(&berin->BUS_REF,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_VECTBUX :
         beg_addUniqChain(&berin->BUX_VEC,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_SINGBUX :
         beg_addUniqChain(&berin->BUX_REF,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_VECTREG :
         beg_addUniqChain(&berin->REG_VEC,BEG_OBJ_DATA(obj)); break;
    case BEG_TYPE_SINGREG :
         beg_addUniqChain(&berin->REG_REF,BEG_OBJ_DATA(obj)); break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_addUniqChain()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_addUniqChain(chain_list **head,void *data)
{
  chain_list    *chain = *head;

  for (; chain; chain = chain->NEXT)
    if (chain->DATA == data)
      break;
  if (!chain)
    *head = addchain(*head,data);
}

/*}}}************************************************************************/
/*{{{                    beg_getLeft()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beg_getLeft(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTPOR :
         return ((bevectpor_list *)BEG_OBJ_DATA(obj))->LEFT;
    case BEG_TYPE_VECTOUT :
         return ((bevectout_list *)BEG_OBJ_DATA(obj))->LEFT;
    case BEG_TYPE_VECTAUX :
         return ((bevectaux_list *)BEG_OBJ_DATA(obj))->LEFT;
    case BEG_TYPE_VECTBUS :
         return ((bevectbus_list *)BEG_OBJ_DATA(obj))->LEFT;
    case BEG_TYPE_VECTBUX :
         return ((bevectbux_list *)BEG_OBJ_DATA(obj))->LEFT;
    case BEG_TYPE_VECTREG :
         return ((bevectreg_list *)BEG_OBJ_DATA(obj))->LEFT;
    case BEG_MASK_RIN :
         return     ((berin_list *)BEG_OBJ_DATA(obj))->LEFT;
    default : return -1;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_getRight()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beg_getRight(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTPOR :
         return ((bevectpor_list *)BEG_OBJ_DATA(obj))->RIGHT;
    case BEG_TYPE_VECTOUT :
         return ((bevectout_list *)BEG_OBJ_DATA(obj))->RIGHT;
    case BEG_TYPE_VECTAUX :
         return ((bevectaux_list *)BEG_OBJ_DATA(obj))->RIGHT;
    case BEG_TYPE_VECTBUS :
         return ((bevectbus_list *)BEG_OBJ_DATA(obj))->RIGHT;
    case BEG_TYPE_VECTBUX :
         return ((bevectbux_list *)BEG_OBJ_DATA(obj))->RIGHT;
    case BEG_TYPE_VECTREG :
         return ((bevectreg_list *)BEG_OBJ_DATA(obj))->RIGHT;
    case BEG_MASK_RIN :
         return     ((berin_list *)BEG_OBJ_DATA(obj))->RIGHT;
    default : return -1;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_newRight()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_newRight(BEG_OBJ obj,
                  int right)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTPOR :
         ((bevectpor_list *)BEG_OBJ_DATA(obj))->RIGHT = right; break;
    case BEG_TYPE_VECTOUT :
         ((bevectout_list *)BEG_OBJ_DATA(obj))->RIGHT = right; break;
    case BEG_TYPE_VECTAUX :
         ((bevectaux_list *)BEG_OBJ_DATA(obj))->RIGHT = right; break;
    case BEG_TYPE_VECTBUS :
         ((bevectbus_list *)BEG_OBJ_DATA(obj))->RIGHT = right; break;
    case BEG_TYPE_VECTBUX :
         ((bevectbux_list *)BEG_OBJ_DATA(obj))->RIGHT = right; break;
    case BEG_TYPE_VECTREG :
         ((bevectreg_list *)BEG_OBJ_DATA(obj))->RIGHT = right; break;
    case BEG_MASK_RIN :
         ((berin_list *)BEG_OBJ_DATA(obj))->RIGHT     = right; break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_newLeft()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_newLeft(BEG_OBJ obj,
                 int left)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTPOR :
         ((bevectpor_list *)BEG_OBJ_DATA(obj))->LEFT = left; break;
    case BEG_TYPE_VECTOUT :
         ((bevectout_list *)BEG_OBJ_DATA(obj))->LEFT = left; break;
    case BEG_TYPE_VECTAUX :
         ((bevectaux_list *)BEG_OBJ_DATA(obj))->LEFT = left; break;
    case BEG_TYPE_VECTBUS :
         ((bevectbus_list *)BEG_OBJ_DATA(obj))->LEFT = left; break;
    case BEG_TYPE_VECTBUX :
         ((bevectbux_list *)BEG_OBJ_DATA(obj))->LEFT = left; break;
    case BEG_TYPE_VECTREG :
         ((bevectreg_list *)BEG_OBJ_DATA(obj))->LEFT = left; break;
    case BEG_MASK_RIN :
         ((berin_list *)BEG_OBJ_DATA(obj))->LEFT     = left; break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_adjustDirection()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
void beg_adjustDirection(BEG_OBJ obj, char direction)
{
  char      *dir;
  
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTPOR :
         dir    = &((bevectpor_list *)BEG_OBJ_DATA(obj))->DIRECTION;
         break;
    case BEG_TYPE_SINGPOR :
         dir    = &((bepor_list *)BEG_OBJ_DATA(obj))->DIRECTION;
         break;
    default : ;
  }
  
  switch (direction)
  {
    case 'I':
         makeConIn(dir,NULL);
         break;
    case 'O':
         makeConOut(dir,NULL);
         break;
    case 'B':
         *dir   = direction;
    case 'X':
    default:
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_getName()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *beg_getName(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTPOR :
         return ((bevectpor_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_SINGPOR :
         return     ((bepor_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_VECTOUT :
         return ((bevectout_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_SINGOUT :
         return     ((beout_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_VECTAUX :
         return ((bevectaux_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_SINGAUX :
         return     ((beaux_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_VECTBUS :
         return ((bevectbus_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_SINGBUS :
         return     ((bebus_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_VECTBUX :
         return ((bevectbux_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_SINGBUX :
         return     ((bebux_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_VECTREG :
         return ((bevectreg_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_TYPE_SINGREG :
         return     ((bereg_list *)BEG_OBJ_DATA(obj))->NAME;
    case BEG_MASK_RIN :
         return     ((berin_list *)BEG_OBJ_DATA(obj))->NAME;
    default : return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_newSig()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_newSig(char *name, BEG_OBJ obj)
{
  addhtitem(BEG_SIG_TABLE,name,(long)obj);
}

/*}}}************************************************************************/
/*{{{                    beg_getLongName()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *beg_getLongName(BEG_OBJ obj)
{
  char       buf[255];

  sprintf(buf,"%s %d:%d",beg_getName(obj),beg_getLeft(obj),beg_getRight(obj));
  return mbkstrdup(buf);
}

/*}}}************************************************************************/
/*{{{                    beg_newPor()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_newPor(char *name, BEG_OBJ obj)
{
  addhtitem(BEG_POR_TABLE,name,(long)obj);
}

/*}}}************************************************************************/
/*{{{                    beg_newVectBiabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_newVectBiabl(BEG_OBJ obj, vectbiabl_list *vbiabl)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTBUS :
         ((bevectbus_list *)BEG_OBJ_DATA(obj))->VECTBIABL = vbiabl; break;
    case BEG_TYPE_VECTBUX :
         ((bevectbux_list *)BEG_OBJ_DATA(obj))->VECTBIABL = vbiabl; break;
    case BEG_TYPE_VECTREG :
         ((bevectreg_list *)BEG_OBJ_DATA(obj))->VECTBIABL = vbiabl; break;
    default : ;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_getVectBiabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
vectbiabl_list *beg_getVectBiabl(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTBUS :
         return ((bevectbus_list *)BEG_OBJ_DATA(obj))->VECTBIABL;
    case BEG_TYPE_VECTBUX :
         return ((bevectbux_list *)BEG_OBJ_DATA(obj))->VECTBIABL;
    case BEG_TYPE_VECTREG :
         return ((bevectreg_list *)BEG_OBJ_DATA(obj))->VECTBIABL;
    case BEG_TYPE_CONFLIT :
         return BEG_OBJ_DATA(obj);
    default : return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_isVect()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beg_isVect(BEG_OBJ obj)
{
  return (BEG_OBJ_TYPE(obj)&BEG_MASK_VECT);
}

/*}}}************************************************************************/
/*{{{                    beg_getBiabl()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static biabl_list *beg_getBiabl(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_SINGBUS :
         return     ((bebus_list *)BEG_OBJ_DATA(obj))->BIABL;
    case BEG_TYPE_SINGBUX :
         return     ((bebux_list *)BEG_OBJ_DATA(obj))->BIABL;
    case BEG_TYPE_SINGREG :
         return     ((bereg_list *)BEG_OBJ_DATA(obj))->BIABL;
    default : return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_newBiabl()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_newBiabl(BEG_OBJ obj, biabl_list *biabl)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_SINGBUS :
         ((bebus_list *)BEG_OBJ_DATA(obj))->BIABL = biabl;
    case BEG_TYPE_SINGBUX :
         ((bebux_list *)BEG_OBJ_DATA(obj))->BIABL = biabl;
    case BEG_TYPE_SINGREG :
         ((bereg_list *)BEG_OBJ_DATA(obj))->BIABL = biabl;
    default : ;
  }
}

static void beg_updateflags(BEG_OBJ obj, long flags, long biflags)
{
  biabl_list *biabl=NULL;
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_SINGBUS :
         ((bebus_list *)BEG_OBJ_DATA(obj))->FLAGS |= flags;
         biabl=((bebus_list *)BEG_OBJ_DATA(obj))->BIABL;
    case BEG_TYPE_SINGBUX :
         ((bebux_list *)BEG_OBJ_DATA(obj))->FLAGS |= flags;
         biabl=((bebux_list *)BEG_OBJ_DATA(obj))->BIABL;
    case BEG_TYPE_SINGREG :
         ((bereg_list *)BEG_OBJ_DATA(obj))->FLAGS |= flags;
         biabl=((bereg_list *)BEG_OBJ_DATA(obj))->BIABL;
    default : ;
  }
  if (biflags!=0 && biabl!=NULL)
  {
    while (biabl->NEXT!=NULL) biabl=biabl->NEXT;
    biabl->FLAG|=biflags;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_getMaxObj()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_getMaxObj(BEG_OBJ sig,int lt,int rt)
{
  int         sl,sr;

  sl = beg_getLeft(sig);
  sr = beg_getRight(sig);

  if (sl > sr)
    if (lt > rt)
      if (sl > lt)
        return sl;
      else
        return lt;
    else
      if (sl > rt)
        return sl;
      else
        return rt;
  else
    if (lt > rt)
      if (sr > lt)
        return sr;
      else
        return lt;
    else
      if (sr > rt)
        return sr;
      else
        return rt;
}

/*}}}************************************************************************/
/*{{{                    beg_getMinObj()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_getMinObj(BEG_OBJ sig,int lt,int rt)
{
  int         sl,sr;

  sl = beg_getLeft(sig);
  sr = beg_getRight(sig);

  if (sl < sr)
    if (lt < rt)
      if (sl < lt)
        return sl;
      else
        return lt;
    else
      if (sl < rt)
        return sl;
      else
        return rt;
  else
    if (lt < rt)
      if (sr < lt)
        return sr;
      else
        return lt;
    else
      if (sr < rt)
        return sr;
      else
        return rt;
}

/*}}}************************************************************************/
/*{{{                    beg_wayUp()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_wayUp(BEG_OBJ obj)
{
  return beg_way(beg_getRight(obj),beg_getLeft(obj));
}

/*}}}************************************************************************/
/*{{{                    beg_wayDown()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_wayDown(BEG_OBJ obj)
{
  return beg_way(beg_getLeft(obj),beg_getRight(obj));
}

/*}}}************************************************************************/
/*{{{                    beg_way()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_way(int left,int right)
{
  return (left >= right);
}

/*}}}************************************************************************/
/*{{{                    beg_wayStrict()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_wayStrict(int left,int right)
{
  return (BEG_USER_WAY) ? (left > right) : beg_way(left,right);
}

/*}}}************************************************************************/
/*{{{                    beg_isSameWay()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_isSameWay(BEG_OBJ obj,int left,int right)
{
  if (!beg_way(right,left))
    return beg_wayDown(obj);
  else if (!beg_way(left,right))
    return beg_wayUp(obj);
  else
    return 1;
}

/*}}}************************************************************************/
/*{{{                    beg_chkWay()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_chkWay(BEG_OBJ obj,int left,int right)
{
  if (left != right)
    if (!beg_isSameWay(obj,left,right))
    {
      if (gethtitem(BEG_GUE_TABLE,beg_getName(obj)) != EMPTYHT)
      {
        beg_invertBound(obj);
        delhtitem(BEG_GUE_TABLE,beg_getName(obj));
      }
      else
        beg_error(BEG_ERR_WAY,beg_getName(obj));
    }
}

/*}}}************************************************************************/
/*{{{                    beg_invertBound()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_invertBound(BEG_OBJ obj)
{
  int        tmp;

  if (BEG_RIN_TREAT && gethtitem(BEG_RIN_ADJUS,beg_getName(obj)) != EMPTYHT)
    return;
  tmp = beg_getRight(obj);
  beg_newRight(obj,beg_getLeft(obj));
  beg_newLeft (obj,tmp);
  beg_invertObj(obj);
}

/*}}}************************************************************************/
/*{{{                    beg_invertObj()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_invertObj(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj) & ~BEG_MASK_TYPE)
  {
    case BEG_MASK_OUT : case BEG_MASK_AUX :
         invertVectAbl(beg_getAbl(obj));
    case BEG_MASK_REG : case BEG_MASK_BUX : case BEG_MASK_BUS :
         beh_invertvectbiabl(beg_getVectBiabl(obj));
  }
}

/*}}}************************************************************************/
/*{{{                    beg_getAbl()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *beg_getAbl(BEG_OBJ obj)
{
  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_VECTOUT :
         return ((bevectout_list *)BEG_OBJ_DATA(obj))->ABL;
    case BEG_TYPE_VECTAUX :
         return ((bevectaux_list *)BEG_OBJ_DATA(obj))->ABL;
    default : return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_makeInPor()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_makeInPor(BEG_OBJ obj)
{
  char              *dir;

  switch (BEG_OBJ_TYPE(obj))
  {
    case BEG_TYPE_SINGPOR :
         dir = &((bepor_list*)BEG_OBJ_DATA(obj))->DIRECTION;
         break;
    case BEG_TYPE_VECTPOR :
         dir = &((bevectpor_list*)BEG_OBJ_DATA(obj))->DIRECTION;
         break;
  }
  makeConIn(dir,obj);
}

/*}}}************************************************************************/
/*{{{                    beg_isPor()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_isPor(char *name)
{
  long         sig;

  sig = gethtitem(BEG_POR_TABLE,name);

  if (sig == EMPTYHT)
    return NULL;
  else
    return (BEG_OBJ)sig;
}

/*}}}************************************************************************/
/*{{{                    beg_isInternalSig()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_isInternalSig(char *name)
{
  long         sig;

  sig = gethtitem(BEG_SIG_TABLE,name);

  if (sig == EMPTYHT)
    return NULL;
  else
    return (BEG_OBJ)sig;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Others utilities                                   */
/****************************************************************************/
/*{{{                    beg_getRangeFLoop()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_GetRangeFLoop(char **opname,long *left,long *right,char *expr,
                              loop_list *loop)
{
  BEG_OBJ    obj;
  eqt_ctx   *ctx = eqt_init(2);
  
  beg_calcRange(ctx,expr,loop,left,right);
  eqt_term(ctx);

  if ((obj = beg_isPor(*opname)) || (obj = beg_isInternalSig(*opname)))
  {
    if (!beg_isSameWay(obj,*left,*right) && !BEG_BLOCK_INV)
      beg_invertLongArg(left,right);
  }
  else
    beg_addGuessedRange(*opname,(int)*left);
}

/*}}}************************************************************************/
/*{{{                    beg_guessRange()                                   */
/*                                                                          */
/* guess the undefined range of a signal because of variable index          */
/*                                                                          */
/* return index                                                             */
/****************************************************************************/
static char *beg_GuessRange(char *name,char **opname,long *left,long *right,
                            loop_list *loop,char mode)
{
  char      *expr;

  beg_get_vectname(name,opname,left,right,mode);
  if (*left != -2)
    return NULL;
  else
  {
    if (mode == BEG_SEARCH_NAME)
      mode = BEG_SEARCH_LOOP;
    else if (mode == BEG_SEARCH_ABL)
      mode = BEG_SEARCH_ALOOP;

    expr = beg_get_loopvar(name,opname,mode);
    if (expr)
      beg_GetRangeFLoop(opname,left,right,expr,loop);
    else
    {
      *left  = -1;
      *right = -1;
    }
    return expr;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_calcRange()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static 
void beg_calcRange(eqt_ctx *ctx, char *expr, loop_list *loop, long *left,
                   long *right)
{
  if (!loop)
  {
    *left   = (int)eqt_eval(ctx, expr,EQTFAST);
    *right  = (int)eqt_eval(ctx, expr,EQTFAST);
  }
  else
  {
    long         left1, right1, left2, right2;

    eqt_addvar(ctx, loop->VARNAME,loop->LEFT);
    beg_calcRange(ctx, expr,loop->NEXT,&left1,&right1);
    eqt_addvar(ctx, loop->VARNAME,loop->RIGHT);
    beg_calcRange(ctx, expr,loop->NEXT,&left2,&right2);
    *left   = beg_getMax(4,left1,left2,right1,right2);
    *right  = beg_getMin(4,left1,left2,right1,right2);
    if (BEG_USER_WAY)
      beg_invertLongArg(left,right);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_addGuessedRange()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_addGuessedRange(char *opname, int left)
{
  addhtitem(BEG_GUE_TABLE,opname,left);
}

/*}}}************************************************************************/
/*{{{                    beg_getMin()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_getMin(int nb, ... )
{
  va_list    arg;
  int        i;
  int        res;
  int        tmp;

  va_start(arg,nb);
  res = va_arg(arg,int);
  for (i = 1; i < nb; i ++)
    if ( (tmp = va_arg(arg,int)) < res)
      res = tmp;
  va_end(arg);
  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_getMax()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_getMax(int nb, ... )
{
  va_list    arg;
  int        i;
  int        res;
  int        tmp;

  va_start(arg,nb);
  res = va_arg(arg,int);
  for (i = 1; i < nb; i ++)
    if ( (tmp = va_arg(arg,int)) > res)
      res = tmp;
  va_end(arg);
  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_invertIntArg()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_invertIntArg(int *a, int *b)
{
  int        tmp;

  tmp = *a;
  *a  = *b;
  *b  = tmp;
}

/*}}}************************************************************************/
/*{{{                    beg_invertLongArg()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_invertLongArg(long *a, long *b)
{
  long       tmp;

  tmp = *a;
  *a  = *b;
  *b  = tmp;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Perform action                                     */
/****************************************************************************/
/*{{{                    beg_performVReg()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_performVReg(bevectreg_list *vreg,
                            vectbiabl_list *(*func)(vectbiabl_list*))
{
  if (vreg)
  {
    beg_performVReg(vreg->NEXT,func);
    vreg->VECTBIABL = (*func) (vreg->VECTBIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_performVBux()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_performVBux(bevectbux_list *vbux,
                            vectbiabl_list *(*func)(vectbiabl_list*))
{
  if (vbux)
  {
    beg_performVBux(vbux->NEXT,func);
    vbux->VECTBIABL = (*func) (vbux->VECTBIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_performVBus()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_performVBus(bevectbus_list *vbus,
                            vectbiabl_list *(*func)(vectbiabl_list*))
{
  if (vbus)
  {
    beg_performVBus(vbus->NEXT,func);
    vbus->VECTBIABL = (*func) (vbus->VECTBIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_perform()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_perform(vectbiabl_list *(*func)(vectbiabl_list*))
{
  beg_performVReg(BEG_WORK_BEFIG->BEVECTREG,func);
  beg_performVBux(BEG_WORK_BEFIG->BEVECTBUX,func);
  beg_performVBus(BEG_WORK_BEFIG->BEVECTBUS,func);
}


/*}}}************************************************************************/
/*{{{                    beg_performReg()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_performReg(bereg_list *reg, biabl_list *(*func)(biabl_list*))
{
  if (reg)
  {
    beg_performReg(reg->NEXT,func);
    reg->BIABL = (*func) (reg->BIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_performBux()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_performBux(bebux_list *bux, biabl_list *(*func)(biabl_list*))
{
  if (bux)
  {
    beg_performBux(bux->NEXT,func);
    bux->BIABL = (*func) (bux->BIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_performBus()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_performBus(bebus_list *bus, biabl_list *(*func)(biabl_list*))
{
  if (bus)
  {
    beg_performBus(bus->NEXT,func);
    bus->BIABL = (*func) (bus->BIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_singPerform()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_singPerform(biabl_list *(*func)(biabl_list*))
{
  beg_performReg(BEG_WORK_BEFIG->BEREG,func);
  beg_performBux(BEG_WORK_BEFIG->BEBUX,func);
  beg_performBus(BEG_WORK_BEFIG->BEBUS,func);
}


/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Sort function                                      */
/****************************************************************************/
/*{{{                    beg_sortVectBiablDoTable()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list **beg_sortVectBiablDoTable(vectbiabl_list *vbiabl,
                                                 int cpt, int *nb)
{
  vectbiabl_list   **vbtable = NULL;

  if (! vbiabl)
    if (cpt)
    {
      int     eltsize;

      eltsize        = sizeof(vectbiabl_list*);
      vbtable        = (vectbiabl_list**)mbkalloc(cpt*eltsize);
      *nb            = cpt;
      
      return vbtable;
    }
    else
      return NULL;
  else
  {
    vbtable      = beg_sortVectBiablDoTable(vbiabl->NEXT,cpt+1,nb);
    vbtable[cpt] = vbiabl;
    
    return vbtable;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_sortVectBiablCompare()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_sortVectBiablCompare(const void *vb1,const void *vb2)
{
//  int              res;
  vectbiabl_list  *vbiabl1 = *(vectbiabl_list **)vb1;
  vectbiabl_list  *vbiabl2 = *(vectbiabl_list **)vb2;

  /*
  if (vbiabl1->RIGHT == vbiabl2->RIGHT && vbiabl1->LEFT == vbiabl2->LEFT)
    res     = 0;
  if (beg_way(vbiabl1->LEFT,vbiabl1->RIGHT))
    res     = (vbiabl1->LEFT < vbiabl2->RIGHT) ? -1 : 1;
  else
    res     = (vbiabl1->RIGHT < vbiabl2->LEFT) ? -1 : 1;
  
  return res;
*/
  return vbiabl1->RIGHT - vbiabl2->RIGHT;
}

/*}}}************************************************************************/
/*{{{                    beg_sortVectBiablGetSortedTable()                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list *beg_sortVectBiablGetSortedTable(vectbiabl_list **vbtable,
                                                       int nb)
{
  int                i;
  vectbiabl_list    *res;

  for (i = 0; i < nb - 1; i ++)
    vbtable[i]->NEXT = vbtable[i + 1];
  vbtable[nb-1]->NEXT   = NULL;
  res = vbtable[0];
  mbkfree(vbtable);

  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_sortVectBiabl()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list *beg_sortVectBiabl(vectbiabl_list *vbiabl)
{
  vectbiabl_list   **vbtable     = NULL;
  //int                 way        = 1;
  //int                 eltsize;
  int                 nbelt;

  vbtable   = beg_sortVectBiablDoTable(vbiabl,0,&nbelt);
  //  eltsize   = sizeof(vectbiabl_list*);
  qsort(vbtable,nbelt,sizeof(vectbiabl_list*),beg_sortVectBiablCompare);
  vbiabl    = beg_sortVectBiablGetSortedTable(vbtable,nbelt);

  return vbiabl;
}

/*}}}************************************************************************/
/*{{{                    beg_sort()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_sort()
{
  beg_perform(beg_sortVectBiabl);
  beg_sortElse();
}

/*}}}************************************************************************/
/*{{{                    beg_forceZSort()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_forceZSort(void)
{
  beg_performVReg(BEG_WORK_BEFIG->BEVECTREG,beg_sortElseVB);
  beg_performReg(BEG_WORK_BEFIG->BEREG,beg_sortElseB);
  BEG_SORT_DONE   = 1;
}

/*}}}************************************************************************/
/*{{{                    beg_sortElse()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_sortElse()
{
  if ( !BEG_SORT_DONE )
  {
    beg_performVReg(BEG_WORK_BEFIG->BEVECTREG,beg_sortElseVB);
    beg_performReg(BEG_WORK_BEFIG->BEREG,beg_sortElseB);
    BEG_SORT_DONE   = 1;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_sortElseVB()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list *beg_sortElseVB(vectbiabl_list *vbiabl)
{
  vectbiabl_list    *vbiablx;

  for (vbiablx = vbiabl; vbiablx; vbiablx = vbiablx->NEXT)
    vbiablx->BIABL  = beg_sortElseB(vbiablx->BIABL);
  return vbiabl;
}

/*}}}************************************************************************/
/*{{{                    beg_sortElseB()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static biabl_list *beg_sortElseB(biabl_list *biabl)
{
#if 0
  biabl_list    *newH, *lastH, *lastQ, *biablx, *last;
  int            needtodo, isHZ;

  newH              = biabl;
  biablx            = biabl;
  needtodo          = 0;
  last              = NULL;

  while (1)
  {
    if (biablx)
      isHZ          = beg_isBiablHZ(biablx);
    if ( ! biablx || (biablx && ! (biablx->FLAG&BEH_CND_PRECEDE)) )
    {
      if (needtodo && lastQ)
      {
        lastQ->NEXT = biablx;
        last->NEXT  = newH;
        newH        = lastH;
        needtodo    = 0;
      }
      lastH         = biablx;
      lastQ         = last;
      if ( ! biablx )
        break;
    }
    if ( ! biablx )
      break;
    /*if ( !(biablx->FLAG&BEH_CND_PRECEDE) && needtodo && lastQ && !isHZ )
    {
      biablx->FLAG &= ~BEH_CND_PRECEDE;
      lastH->FLAG  |= BEH_CND_PRECEDE;
      last->NEXT    = biablx->NEXT;
      biablx->NEXT  = lastH;
      lastH         = biablx;
      lastQ->NEXT   = biablx;
      biablx        = last;
    }*/
    needtodo        = isHZ;
    last            = biablx;
    biablx          = biablx->NEXT;
  }

  return newH;
#else
  /* param : biabl */

  chain_list    *pH, *hl, *chainx, *Z, *nZ, *lZ, *lnZ, *x;
  biabl_list    *biablx, *res;
  int            hasZ;
  
  hl                    = NULL;
  for (biablx = biabl; biablx; biablx = biablx->NEXT)
  {
    if (!(biablx->FLAG&BEH_CND_PRECEDE))
      hl                = addchain(hl,NULL);
    hl->DATA            = addchain(hl->DATA,biablx);
  }
  
  lZ                    = NULL;
  lnZ                   = NULL;
  for (chainx = hl; chainx; chainx = delchain(chainx,chainx))
  {
    hasZ                = 0;
    pH                  = chainx->DATA;
    biablx              = pH->DATA;
    if (biablx->CNDABL && !(biablx->FLAG&BEH_CND_LAST))
    { 
      Z                 = NULL;
      nZ                = NULL;
      for (; pH; pH = delchain(pH,pH))
      {
        biablx          = pH->DATA;
        if (beg_isBiablHZ(biablx))
        {
          Z             = addchain(Z,biablx);
          hasZ ++;
        }
        else
          nZ            = addchain(nZ,biablx);
      }
      pH                = append(Z,nZ);
      biablx            = pH->DATA;
      biablx->FLAG     &= ~BEH_CND_PRECEDE;
      for (x = pH->NEXT; x; x = x->NEXT)
      {
        biablx          = x->DATA;
        biablx->FLAG   |= BEH_CND_PRECEDE;
      }
        
    }
    if (hasZ)
      lZ                = addchain(lZ,pH);
    else
      lnZ               = addchain(lnZ,pH);
  }
  hl                    = append(lZ,lnZ);
  
  res                   = ((chain_list*)hl->DATA)->DATA;
  biablx                = NULL;
  for (chainx = hl; chainx; chainx = delchain(chainx,chainx))
    for (x = chainx->DATA; x; x = delchain(x,x))
      if (!biablx)
        biablx          = (biabl_list*)x->DATA;
      else
      {
        biablx->NEXT    = (biabl_list*)x->DATA;
        biablx          = biablx->NEXT;
      }
  biablx->NEXT          = NULL;
  
  return res;
#endif
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Figure construction                                */
/****************************************************************************/
/*{{{                    Api level functions                                */
/****************************************************************************/
/*{{{                    beg_def_befig()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_def_befig(char *name)
{
  static int     init       = 0;
  char          *way;
  
  if (!init)
  {
    init                = 1;
    beg_initError();

    BEG_ALLOW_SELECT    = (int)(long)getenv("BEG_ALLOW_SELECT");
    BEG_COMPRESS_MAX    = 1;
    BEG_TRACE           = (int)(long)getenv("BEG_TRACE");
    
    BEG_USER_WAY        = 0; // <= downto
    if ((way=V_STR_TAB[__BEG_USER_WAY].VALUE)!=NULL)
    {
      if (!strcasecmp(way,"to"))
        BEG_USER_WAY    = 1;
      else if (strcasecmp(way,"downto"))
        beg_warning(BEG_WAR_WAYENV,", actual: %s",way);
    }
  }

  BEG_SORT_DONE       = 0;
  BEG_PHASE_1         = 1;
  BEG_IS_CHECKED      = 0;

  beg_getOneEnvFromFigName(name);
}

/*}}}************************************************************************/
/*{{{                    beg_duplicate()                                    */
/*                                                                          */
/* duplicate the model to create an instance                                */
/*                                                                          */
/****************************************************************************/
befig_list *beg_duplicate(befig_list *model, char *insname)
{
  char          *curfigname;
  befig_list    *dup, *res;

  curfigname    = BEG_WORK_BEFIG->NAME;

  beg_def_befig(insname);
  dup           = beh_duplicate(model);
  dup->NAME     = "tmpNameForduplication";
  beg_eat_por(dup);
  beg_eat_figure(dup);
  res           = beg_get_befig(BEG_NOVERIF);
  
  beg_def_befig(curfigname);
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    updateCondir()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
updateCondir(char *old, char dir)
{
  switch (dir)
  {
    case 'I':
         makeConIn(old,NULL);
         break;
    case 'O':
         makeConOut(old,NULL);
         break;
    case 'X':
    case 'B':
    default:
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_updateConsDir()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
beg_updateConsDir(befig_list *befig)
{
  static ht         *conht   = NULL;
  static ht         *convht   = NULL;
  bevectpor_list    *vpor, *vx;
  bepor_list        *por, *x;

  if (!befig)
  {
    delht(conht);
    conht       = NULL;
    delht(convht);
    convht       = NULL;
  }
  else
  {
    // hash table contains yagle pins
    if (!conht || !convht)
    {
      conht     = addht(50);
      for (por = BEG_WORK_BEFIG->BEPOR; por; por = por->NEXT)
        addhtitem(conht,por->NAME,(long)por);

      convht    = addht(50);
      for (vpor = BEG_WORK_BEFIG->BEVECTPOR; vpor; vpor = vpor->NEXT)
        addhtitem(convht,vpor->NAME,(long)vpor);
    }

    for (por = befig->BEPOR; por; por = por->NEXT)
    {
      x         = (bepor_list *)gethtitem(conht,por->NAME);
      if (x != (bepor_list*)EMPTYHT)
        updateCondir(&x->DIRECTION,por->DIRECTION);
    }
    
    for (vpor = befig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    {
      vx        = (bevectpor_list *)gethtitem(conht,vpor->NAME);
      if (vx != (bevectpor_list*)EMPTYHT)
        updateCondir(&vx->DIRECTION,vpor->DIRECTION);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    beg_def_por()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_def_por(char *name, char  direction)
{
  beg_def_bepor(name,direction,0);
}
/*}}}************************************************************************/
/*{{{                    beg_def_mux()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list *beg_def_mux(char *name, char *cndexpr, char *bitstr, char *valexpr,
                        int time, long flags, long biflags)
{
  BEG_OBJ            sig;
  chain_list        *cnd;
  chain_list        *val;
  chain_list        *bit;
  char              *opname;
  biabl_list        *biabl;
  long               left,right;
  BEG_OBJ            obj;
  chain_list        *berins = NULL;
  chain_list        *berinx = NULL;
  char              *signame;

  beg_get_vectname(name,&opname,&left,&right,BEG_SEARCH_NAME);

  val         = beg_str2Abl(valexpr);
  berins      = beg_IsAllDeclared(val,NULL);

  if (cndexpr)
  {
    cnd       = beg_str2Abl(cndexpr);
    if (!BEG_ALLOW_SELECT && !ATOM(cnd))
      beg_error(BEG_ERR_SELECT,name);
    berinx    = beg_IsAllDeclared(cnd,NULL);
  }
  else
    cnd       = NULL;

  if (!beg_isInternalSig(opname) && !cndexpr)
    beg_error(BEG_ERR_NULCND,NULL);
  biabl       = beh_addbiabl(NULL,NULL,cnd,val);
  biabl->TIME = time;

  if (bitstr)
  {
    if ( !strcmp(bitstr,"default") || !strcmp(bitstr,"others"))
      beh_addcnddefault2biabl(biabl);
    else
    {
      bit     = beg_str2Abl(bitstr);
      bitstr  = bit->DATA;
      freeExpr(bit);
      beh_addcndvect2biabl(biabl,bitstr);
    }
  }

  // it's an external bus (case bebus)
  if ((sig = beg_isPor(opname)))
  {
    beg_chkWay(sig,left,right);
    obj       = beg_def_bebus(name,biabl,0,flags,biflags);
    if (beg_doITreatThisOne(sig,beg_getLeft(obj),beg_getRight(obj)))
    {
      signame = beg_getLongName(sig);
      beg_error(BEG_ERR_POR,": %s defined %s",name,beg_getLongName(sig));
      mbkfree(signame);
    }
  }
  else // (case bebux)
    obj       = beg_def_bebux(name,biabl,0,flags,biflags);

  beg_addBerins(berins,obj);
  beg_addBerins(berinx,obj);

  return biabl;
}

/*}}}************************************************************************/
/*{{{                    beg_def_process()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list *beg_def_process(char *name,char *cndexpr,char *valexpr,int time, int timer, int timef,
                            long flag, long flags,long biflags)
{
  chain_list        *cnd    = NULL;
  chain_list        *val    = NULL;
  biabl_list        *biabl;
  BEG_OBJ            obj;
  chain_list        *berins = NULL;
  chain_list        *berinx = NULL;

  val            = beg_str2Abl(valexpr);
  berins         = beg_IsAllDeclared(val,NULL);
  if (cndexpr)
  {
    cnd          = beg_str2Abl(cndexpr);
    berinx       = beg_IsAllDeclared(cnd,NULL);
  }

  // biabl construction
  biabl          = beh_addbiabl(NULL,NULL,cnd,val);
  biabl->TIME    = time;
  biabl->TIMER   = timer;
  biabl->TIMEF   = timef;
  if (flag&BEG_PRE || flag&BEG_LAST)
    biabl->FLAG |= BEH_CND_PRECEDE;
  if (flag&BEG_LAST)
    biabl->FLAG |= BEH_CND_LAST;
  if (!BEG_ALLOW_SELECT || flag&BEG_CMPLX)
    biabl->FLAG |= BEH_CND_CMPLX;

  if (flag&BEG_REG)
    obj          = beg_def_bereg(name,biabl,0,flags,biflags);
  else
    obj          = beg_defBus(name,biabl,0,flags,biflags);

  beg_addBerins(berins,obj);
  beg_addBerins(berinx,obj);

  return biabl;
}

/*}}}************************************************************************/
/*{{{                    beg_def_loop()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list * beg_def_loop(char *name, char *cndexpr, char *valexpr,
                          char *var, int   time, long  flag,  long flags, long biflags)
{
  return beg_def_biloop(name,cndexpr,valexpr,var,NULL,time,flag,flags,biflags);
}

/*}}}************************************************************************/
/*{{{                    beg_def_biloop()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list * beg_def_biloop(char *name,char *cndexpr,char *valexpr,char *var1,
                            char *var2,int   time,long  flag, long flags, long biflags)
{
  chain_list    *cnd;
  chain_list    *val;
  char          *opname;
  biabl_list    *biabl;
  long           left,right;
  BEG_OBJ        obj;
  chain_list    *berins = NULL;
  chain_list    *berinx = NULL;
  char          *index;
  char          *varname;
  long           varbegin,varend;
  loop_list     *loop   = NULL, *loopx;

  beg_get_vectname(var1,&varname,&varbegin,&varend,BEG_SEARCH_NAME);
  loop = beh_newloop(loop,varname,NULL,varbegin,varend);
  if (var2)
  {
    beg_get_vectname(var2,&varname,&varbegin,&varend,BEG_SEARCH_NAME);
    loop = beh_newloop(loop,varname,NULL,varbegin,varend);
  }

  index = beg_GuessRange(name,&opname,&left,&right,loop,BEG_SEARCH_NAME);

  name = beg_mkName(opname,left,right);
  val = beg_str2Abl(valexpr);
  cnd = beg_str2Abl(cndexpr);

  berins = beg_IsAllDeclared(val,loop);
  berinx = beg_IsAllDeclared(cnd,loop);

  for (loopx = loop; loopx; )
  {
    loop    = loopx;
    loopx   = loopx->NEXT;
    mbkfree(loop);
  }

  if ( !beg_isInternalSig(opname)
       &&
       !beg_isPor(opname)
       &&
       !cndexpr)
    beg_error(BEG_ERR_NULCND,NULL);

  // biabl construction
  biabl = beh_addbiabl(NULL,NULL,cnd,val);
  biabl->TIME = time;
  beg_addbiloop(biabl,index,var1,var2);

  if (flag&BEG_REG)
    obj = beg_def_bereg(name,biabl,0,flags,biflags);
  else
    obj = beg_defBus(name,biabl,0,flags,biflags);

  beg_addBerins(berins,obj);
  beg_addBerins(berinx,obj);

  return biabl;
}

/*}}}************************************************************************/
/*{{{                    beg_assertion()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_assertion(char *expr, char *msg, char level, char *label)
{
  bemsg_list    *bemsg;
  chain_list    *abl;


  abl                   = beg_str2Abl(expr);
  bemsg                 = BEG_WORK_BEFIG->BEMSG;
  bemsg                 = beh_addbemsg(bemsg,label,level,msg,abl,NULL);
  BEG_WORK_BEFIG->BEMSG = bemsg;
}

/*}}}************************************************************************/
/*{{{                    beg_get_befigByName()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
befig_list *beg_get_befigByName(char *name)
{
//  befig_list    *ptbefig;
//
//  for (ptbefig = BEG_HEAD_BEFIG; ptbefig; ptbefig = ptbefig->NEXT)
//    if (ptbefig->NAME == name)
//      break;
//  return ptbefig;
  beg_obj_list  *obj;

  return ((obj = beg_getEnvByFigName(name))) ? obj->FIG : NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_get_befig()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
befig_list *beg_get_befig(int verif)
{
  if (!BEG_IS_CHECKED)
  {
    beg_verifyAux();
    BEG_CONF_CPT    = 0;
    beg_reloadConflictError();
    beg_sortElse();

    if (! (verif&BEG_NOVERIF))
      beg_chk_befig();
    beg_saveEnv(beg_getEnv(BEG_WORK_BEFIG));
    BEG_IS_CHECKED  = 1;
  }
  return BEG_WORK_BEFIG;
}

/*}}}************************************************************************/
/*{{{                    beg_get_currentName()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *beg_get_currentName(void)
{
  return BEG_WORK_BEFIG->NAME;
}

/*}}}************************************************************************/
/*{{{                    beg_getObjByName()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
BEG_OBJ beg_getObjByName(befig_list *fig, char *name, BEG_OBJ *por)
{
  beg_obj_list  *work, *current;
  BEG_OBJ        obj;

  work      = beg_getEnv(fig);
  if (work)
  {
    current = beg_getEnv(BEG_WORK_BEFIG);
    beg_saveEnv(current);
    beg_restoreEnv(work);
    *por    = beg_isPor(name);
    obj     = beg_isInternalSig(name);
    beg_restoreEnv(current);

    return obj;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_getBerinByName()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
berin_list *beg_getBerinByName(befig_list *fig, char *name)
{
  beg_obj_list  *work, *current;
  berin_list    *rinx;

  work      = beg_getEnv(fig);
  if (work)
  {
    current = beg_getEnv(BEG_WORK_BEFIG);
    beg_saveEnv(current);
    beg_restoreEnv(work);
    
    rinx    = (berin_list*)gethtitem(BEG_RIN_TABLE,name);
    if (rinx == (berin_list*)EMPTYHT)
      rinx  = NULL; 
    
    beg_restoreEnv(current);

    return rinx;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_def_sig()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_def_sig(char *name, char *expr, unsigned int time, int timer, int timef, long flags)
{
  chain_list    *abl;
  BEG_OBJ        obj;
  chain_list    *berins;

  abl = beg_str2Abl(expr);
  berins = beg_IsAllDeclared(abl,NULL);

  obj = beg_defSig(name,abl,time,timer,timef,flags);

  beg_addBerins(berins,obj);
}

/*}}}************************************************************************/
/*{{{                    beg_add_pors()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_add_pors(befig_list *fig)
{
  if (fig)
  {
    bevectpor_list  *vpor;
    bepor_list      *por;
    BEG_OBJ          obj;

    for (vpor = fig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    {
      obj   = BEG_OBJ_NEW(vpor,BEG_TYPE_VECTPOR);
      beg_newPor(vpor->NAME,obj);
    }

    for (por = fig->BEPOR; por; por = por->NEXT)
    {
      obj   = BEG_OBJ_NEW(por,BEG_TYPE_SINGPOR);
      beg_newPor(por->NAME,obj);
    }
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Low level functions                                */
/****************************************************************************/
/*{{{                    beg_def_bepor()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_def_bepor(char *name, char  direction, char  type)
{
  char              *shortname;
  long               left, right;
  BEG_OBJ            por        = NULL;
  bevectpor_list    *ptvectpor  = NULL;
  bepor_list        *ptpor      = NULL;

  beg_get_vectname(name,&shortname,&left,&right,BEG_SEARCH_NAME);
  if ((por = beg_isPor(shortname)))
  {
    switch (BEG_OBJ_TYPE(por))
    {
      case BEG_TYPE_VECTPOR :
           if (left == -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           ptvectpor = BEG_OBJ_DATA(por);
           beg_adjustDomain(por,left,right);
           beg_adjustDirection(por,direction);
           break;
      case BEG_TYPE_SINGPOR :
           if ( left != -1 || right != -1 )
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           break;
      default :
           beg_error(BEG_ERR_BETYPE,NULL);
    }
  }
  else
  {
    if (left == -1)
    {
      ptpor = beh_addbepor(BEG_WORK_BEFIG->BEPOR,shortname,direction,type);
      BEG_WORK_BEFIG->BEPOR = ptpor;

      por = BEG_OBJ_NEW(ptpor,BEG_TYPE_SINGPOR);
      beg_newPor(shortname,por);
    }
    else
    {
      ptvectpor = beh_addbevectpor(BEG_WORK_BEFIG->BEVECTPOR,shortname,
                                   direction,type,left,right);
      beg_addGuessedRange(shortname,left);
      por = BEG_OBJ_NEW(ptvectpor,BEG_TYPE_VECTPOR);
      BEG_WORK_BEFIG->BEVECTPOR = ptvectpor;
      beg_newPor(shortname,por);
    }
  }

  return por;
}

/*}}}************************************************************************/
/*{{{                    beg_def_beout()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_def_beout(char *name, chain_list *abl, unsigned int time, int timer, int timef, long flags)
{
  char              *shortname;
  long               left, right;
  BEG_OBJ            out        = NULL;
  BEG_OBJ            por;
  bevectout_list    *vectout    = NULL;
  beout_list        *ptout      = NULL;

  beg_get_vectname(name,&shortname,&left,&right,BEG_SEARCH_NAME);
  if (abs(left-right)+1 != verifyVectAbl(abl))
    beg_error(BEG_ERR_SIZE,name);

  if ((out = beg_isInternalSig(shortname)))
  {
    switch (BEG_OBJ_TYPE(out))
    {
      case BEG_TYPE_VECTOUT :
           if (left == -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           vectout = BEG_OBJ_DATA(out);
           beg_resolveExistingSig((int *)&vectout->LEFT,
                                  (int *)&vectout->RIGHT,
                                  &vectout->ABL,left,right,abl);
           if (vectout->TIME < time) vectout->TIME = time;
           break;
      case BEG_TYPE_SINGOUT :
           if ((left != -1)||(right != -1))
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           ptout = BEG_OBJ_DATA(out);
           if (ptout->TIME < time) ptout->TIME = time;
           break;
      default : beg_error(BEG_ERR_BETYPE,NULL);
    }
  }
  else
  {
    if (left == -1)
    {
      ptout = beh_addbeout(BEG_WORK_BEFIG->BEOUT,shortname,abl,NULL,flags);
      ptout->TIME           = time;
      ptout->TIMER           = timer;
      ptout->TIMEF           = timef;
      BEG_WORK_BEFIG->BEOUT = ptout;

      out = BEG_OBJ_NEW(ptout,BEG_TYPE_SINGOUT);
      beg_newSig(shortname,out);
    }
    else
    {
      vectout = beh_addbevectout(BEG_WORK_BEFIG->BEVECTOUT,shortname,
                                 abl,left,right,flags);
      vectout->TIME = time;
      BEG_WORK_BEFIG->BEVECTOUT = vectout;

      beg_addGuessedRange(shortname,left);
      out = BEG_OBJ_NEW(vectout,BEG_TYPE_VECTOUT);
      beg_newSig(shortname,out);
    }
    if ((por = beg_isPor(shortname)))
      beg_adjustDirection(por,'O');
  }
  return out;
}

/*}}}************************************************************************/
/*{{{                    beg_def_beaux()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_def_beaux(char *name, chain_list *abl, unsigned int time, int timer, int timef, long flags)
{
  char              *shortname;
  long               left, right;
  BEG_OBJ            aux        = NULL;
  bevectaux_list    *vectaux    = NULL;
  beaux_list        *ptaux      = NULL;

  beg_get_vectname(name,&shortname,&left,&right,BEG_SEARCH_NAME);
  if (abs(left - right) + 1 != verifyVectAbl(abl))
    beg_error(BEG_ERR_SIZE,name);

  if ((aux = beg_isInternalSig(shortname)))
  {
    switch (BEG_OBJ_TYPE(aux))
    {
      case BEG_TYPE_VECTAUX :
           if (left == -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           vectaux = BEG_OBJ_DATA(aux);
           beg_resolveExistingSig((int *)&vectaux->LEFT,
                                  (int *)&vectaux->RIGHT,
                                  &vectaux->ABL,left,right,abl);
           if (vectaux->TIME < time)
             vectaux->TIME = time;
           break;
      case BEG_TYPE_SINGAUX :
           if ((left != -1)||(right != -1))
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           ptaux = BEG_OBJ_DATA(aux);
           if (ptaux->TIME < time)
             ptaux->TIME = time;
           break;
      default : beg_error(BEG_ERR_BETYPE,NULL);
    }
  }
  else
  {
    if (left == -1)
    {
      ptaux = beh_addbeaux(BEG_WORK_BEFIG->BEAUX,shortname,abl,NULL,flags);
      ptaux->TIME = time;
      ptaux->TIMER = timer;
      ptaux->TIMEF = timef;
      BEG_WORK_BEFIG->BEAUX = ptaux;

      aux = BEG_OBJ_NEW(ptaux,BEG_TYPE_SINGAUX);
      beg_newSig(shortname,aux);
    }
    else
    {
      vectaux = beh_addbevectaux(BEG_WORK_BEFIG->BEVECTAUX,shortname,
                                 abl,left,right,flags);
      vectaux->TIME = time;
      BEG_WORK_BEFIG->BEVECTAUX = vectaux;

      beg_addGuessedRange(shortname,left);
      aux = BEG_OBJ_NEW(vectaux,BEG_TYPE_VECTAUX);
      beg_newSig(shortname,aux);
    }
  }
  return aux;
}

/*}}}************************************************************************/
/*{{{                    beg_def_bebux()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_def_bebux(char *name,
                             biabl_list *biabl,
                             char  type, long flags, long biflags)
{
  char              *shortname;
  long               left, right;
  vectbiabl_list    *vectbiabl;
  BEG_OBJ            bux        = NULL;
  bevectbux_list    *vectbux      = NULL;
  bebux_list        *ptbux      = NULL;

  beg_get_vectname(name,&shortname,&left,&right,BEG_SEARCH_NAME);
  if (!biabl->FLAG & BEH_CND_LOOP)
    if (abs(left - right) + 1 != verifyVectAbl(biabl->VALABL))
      beg_error(BEG_ERR_SIZE,name);

  if ((bux = beg_isInternalSig(shortname)))
  {
    switch (BEG_OBJ_TYPE(bux))
    {
      case BEG_TYPE_VECTBUX :
           if (left == -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           beg_addBiablToVectBiabl(bux,left,right,biabl);
           break;
      case BEG_TYPE_SINGBUX :
           if ( left != -1 || right != -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           beg_addBiablToBEG_OBJ(bux,biabl);
           break;
      default :
           beg_error(BEG_ERR_BETYPE,NULL);
    }
  }
  else
  {
    if (left == -1)
    {
      ptbux = beh_addbebux(BEG_WORK_BEFIG->BEBUX,shortname,biabl,NULL,type,flags);
      BEG_WORK_BEFIG->BEBUX = ptbux;

      bux = BEG_OBJ_NEW(ptbux,BEG_TYPE_SINGBUX);
      beg_newSig(shortname,bux);
    }
    else
    {
      vectbiabl = beh_addvectbiabl(NULL,left,right,biabl);
      vectbux   = beh_addbevectbux(BEG_WORK_BEFIG->BEVECTBUX,shortname,
                                   vectbiabl,left,right,type,flags);
      BEG_WORK_BEFIG->BEVECTBUX = vectbux;

      beg_addGuessedRange(shortname,left);
      bux       = BEG_OBJ_NEW(vectbux,BEG_TYPE_VECTBUX);
      beg_newSig(shortname,bux);
    }
  }
  beg_updateflags(bux, flags, biflags);
  return bux;
}

/*}}}************************************************************************/
/*{{{                    beg_def_bebus()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_def_bebus(char *name,
                             biabl_list *biabl,
                             char  type,long flags, long biflags)
{
  char              *shortname;
  long               left, right;
  vectbiabl_list    *vectbiabl;
  BEG_OBJ            bus        = NULL;
  BEG_OBJ            por;
  bevectbus_list    *vectbus    = NULL;
  bebus_list        *ptbus      = NULL;

  beg_get_vectname(name,&shortname,&left,&right,BEG_SEARCH_NAME);
  if (!biabl->FLAG&BEH_CND_LOOP)
    if (abs(left-right)+1 != verifyVectAbl(biabl->VALABL))
      beg_error(BEG_ERR_SIZE,name);

  if ((bus = beg_isInternalSig(shortname)))
  {
    switch (BEG_OBJ_TYPE(bus))
    {
      case BEG_TYPE_VECTBUS :
           if (left == -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           beg_addBiablToVectBiabl(bus,left,right,biabl);
           break;
      case BEG_TYPE_SINGBUS :
           if ( left != -1 || right != -1 )
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           beg_addBiablToBEG_OBJ(bus,biabl);
           break;
      default : beg_error(BEG_ERR_BETYPE,NULL);
    }
  }
  else
  {
    if (left == -1)
    {
      ptbus = beh_addbebus(BEG_WORK_BEFIG->BEBUS,shortname,biabl,NULL,type,flags);
      BEG_WORK_BEFIG->BEBUS = ptbus;
      bus   = BEG_OBJ_NEW(ptbus,BEG_TYPE_SINGBUS);
      beg_newSig(shortname,bus);
    }
    else
    {
      vectbiabl = beh_addvectbiabl(NULL,left,right,biabl);
      vectbus   = beh_addbevectbus(BEG_WORK_BEFIG->BEVECTBUS,shortname,
                                   vectbiabl,left,right,type,flags);
      BEG_WORK_BEFIG->BEVECTBUS = vectbus;

      beg_addGuessedRange(shortname,left);
      bus       = BEG_OBJ_NEW(vectbus,BEG_TYPE_VECTBUS);
      beg_newSig(shortname,bus);
    }
    if ((por = beg_isPor(shortname)))
      beg_adjustDirection(por,'O');
  }
  beg_updateflags(bus, flags, biflags);
  return bus;
}

/*}}}************************************************************************/
/*{{{                    beg_def_bereg()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_def_bereg(char *name,
                             biabl_list *biabl,
                             char type,long flags, long biflags)
{
  char              *shortname;
  long               left, right;
  vectbiabl_list    *vectbiabl;
  BEG_OBJ            reg        = NULL;
  BEG_OBJ            por;
  bevectreg_list    *vectreg    = NULL;
  bereg_list        *ptreg      = NULL;

  // type is declared to get compatibility with the other functions
  // there is no use to type
  type      = '®';
  
  
  beg_get_vectname(name,&shortname,&left,&right,BEG_SEARCH_NAME);
  if (!beh_isloop(biabl))
    if (abs(left-right)+1 != verifyVectAbl(biabl->VALABL))
      beg_error(BEG_ERR_SIZE,name);

  if ((reg = beg_isInternalSig(shortname)))//||(reg = beg_isPor(shortname)))
  {
    switch (BEG_OBJ_TYPE(reg)&BEG_MASK_TYPE)
    {
      case BEG_MASK_VECT :
           if (left == -1)
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           beg_addBiablToVectBiabl(reg,left,right,biabl);
           break;
      case BEG_MASK_SING :
           if ( left != -1 || right != -1 )
             beg_error(BEG_ERR_SING_N_VECT,NULL);
           beg_addBiablToBEG_OBJ(reg,biabl);
           break;
      default :
           beg_error(BEG_ERR_BETYPE,NULL);
    }
  }
  else
  {
    if (left == -1)
    {
      ptreg = beh_addbereg(BEG_WORK_BEFIG->BEREG,shortname,biabl,NULL,flags);
      BEG_WORK_BEFIG->BEREG = ptreg;

      reg = BEG_OBJ_NEW(ptreg,BEG_TYPE_SINGREG);
      beg_newSig(shortname,reg);
    }
    else
    {
      vectbiabl = beh_addvectbiabl(NULL,left,right,biabl);
      vectreg   = beh_addbevectreg(BEG_WORK_BEFIG->BEVECTREG,shortname,
                                   vectbiabl,left,right,flags);
      BEG_WORK_BEFIG->BEVECTREG = vectreg;

      beg_addGuessedRange(shortname,left);
      reg = BEG_OBJ_NEW(vectreg,BEG_TYPE_VECTREG);
      beg_newSig(shortname,reg);
    }
    if ((por = beg_isPor(shortname)))
      beg_adjustDirection(por,'O');
  }
  beg_updateflags(reg, flags, biflags);
  return reg;
}

/*}}}************************************************************************/
/*{{{                    beg_defSig()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_defSig(char *name,chain_list *abl,unsigned int time,int timer, int timef, long flags)
{
  BEG_OBJ            sig;
  BEG_OBJ            obj;
  bevectout_list    *vectout;
  char              *opname;
  long               left, right;
  char              *signame;

  beg_get_vectname(name,&opname,&left,&right,BEG_SEARCH_NAME);
  // it's an external signal (case beout)
  if ((sig = beg_isPor(opname)))
  {
    beg_chkWay(sig,left,right);
    obj = beg_def_beout(name,abl,time,timer,timef,flags);
    vectout = BEG_OBJ_DATA(obj);
    if ((BEG_OBJ_TYPE(sig)&BEG_MASK_VECT))
      if (beg_doITreatThisOne(sig,vectout->LEFT,vectout->RIGHT))
      {
        signame = beg_getLongName(sig);
        beg_error(BEG_ERR_POR,": %s defined as %s",name,signame);
        mbkfree(signame);
      }
  }
  // (case beaux)
  else
    obj = beg_def_beaux(name,abl,time,timer,timef,flags);
  return obj;
}

/*}}}************************************************************************/
/*{{{                    beg_defBus()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static BEG_OBJ beg_defBus(char *name, biabl_list *biabl,char type,long flags, long biflags)
{
  BEG_OBJ            por;
  BEG_OBJ            res;
  char              *opname;
  long               left, right;
  char              *signame;

  beg_get_vectname(name,&opname,&left,&right,BEG_SEARCH_NAME);

  if ((por = beg_isPor(opname)))
  {
    beg_chkWay(por,left,right);
    if ( (BEG_OBJ_TYPE(por)&BEG_MASK_VECT)
         &&
         (beg_doITreatThisOne(por,left,right)))
    {
      signame = beg_getLongName(por);
      beg_error(BEG_ERR_POR,": %s defined %s",name,signame);
      mbkfree(signame);
    }
    res = beg_def_bebus(name,biabl,type,flags,biflags);
  }
  else
    res = beg_def_bebux(name,biabl,type,flags,biflags);

  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_addbiloop()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_addbiloop(biabl_list *biabl,char *index,char *var1,char *var2)
{
  char          *varname;
  long           varbegin,varend;

  beg_get_vectname(var1,&varname,&varbegin,&varend,BEG_SEARCH_NAME);
  beh_addloop(biabl,index,varname,varbegin,varend);
  if (var2)
  {
    beg_get_vectname(var2,&varname,&varbegin,&varend,BEG_SEARCH_NAME);
    beh_addloop(biabl,index,varname,varbegin,varend);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_addBiablToVectBiabl()                          */
/*                                                                          */
/* trouve un vectbiabl correspondant au domaine                             */
/****************************************************************************/
static void beg_addBiablToVectBiabl(BEG_OBJ obj,int left,int right,
                                    biabl_list *biabl)
{
  vectbiabl_list    *ptvbiabl;
  vectbiabl_list    *vbiabl = beg_getVectBiabl(obj);

  beg_adjustDomain(obj,left,right);
  for (ptvbiabl = vbiabl; ptvbiabl; ptvbiabl = ptvbiabl->NEXT)
  {
    switch (beg_chkCovering(ptvbiabl,left,right))
    {
      case 3 :
           invertVectAbl(biabl->VALABL);
      case 2 :
           ptvbiabl->BIABL  = beg_addBiablToBiablList(ptvbiabl->BIABL,biabl);
           return ;
      case 0 :
           beg_treatConflictError(obj,left,right,biabl);
           return;
      default: ;
    }
  }
  beg_newVectBiabl(obj,beh_addvectbiabl(vbiabl,left,right,biabl));
}

/*}}}************************************************************************/
/*{{{                    beg_addBiablToBEG_OBJ()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_addBiablToBEG_OBJ(BEG_OBJ obj,biabl_list *biabl)
{
  beg_newBiabl(obj,beg_addBiablToBiablList(beg_getBiabl(obj),biabl));
}

/*}}}************************************************************************/
/*{{{                    beg_addBiablToBiablList()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static biabl_list *beg_addBiablToBiablList(biabl_list *list,biabl_list *biabl)
{
  biabl_list     biablx;

  if (!biabl->CNDABL)
    biabl->CNDABL   = copyExpr(list->CNDABL);

  biablx.NEXT     = list;
  biablx.FLAG     = 0;
  biablx.VALABL   = NULL;
  beg_addBiablToBiablListInt(&biablx,biabl);

  return biablx.NEXT;
}

/*}}}************************************************************************/
/*{{{                    beg_addBiablToBiablListInt()                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_addBiablToBiablListInt(biabl_list *list,biabl_list *biabl)
{
  int        res;

  if (list)
    switch ( beg_addBiablToBiablListInt(list->NEXT,biabl))
    {
      case 4 :
           if ( beg_isBiablHZ(biabl) ||  !(biabl->FLAG&BEH_CND_PRECEDE) )
           {
             list->NEXT     = biabl;
             return 1;
           }
      case 3 :
           if ( beg_isBiablHZ(list) )
           {
             res            = (list->FLAG&BEH_CND_PRECEDE) ? 3 : 0;
             list->FLAG    |= BEH_CND_PRECEDE;
             return res;
           }
           else
           {
             biabl->NEXT    = list->NEXT;
             list->NEXT     = biabl;
             return 1;
           }
      case 0 :
           biabl->FLAG     &= ~BEH_CND_PRECEDE;
           biabl->NEXT      = list->NEXT;
           list->NEXT       = biabl;
      case 1 : default :
           return 1;
  }
  else
    return 4;
}

/*}}}************************************************************************/
/*{{{                    beg_adjustDomain()                                 */
/*                                                                          */
/* reajuste le domaine en fonction de left et right                         */
/****************************************************************************/
static void beg_adjustDomain(BEG_OBJ obj,int left,int right)
{
  if ((BEG_OBJ_TYPE(obj) & BEG_MASK_SING) || left == -2 || right == -2)
    return; // nothing to do
  else if (right == -1 || left == -1)
    beg_error(BEG_ERR_SING_N_VECT,beg_getName(obj));
  else if (beg_getLeft(obj) == beg_getRight(obj))
  {
    if (beg_wayStrict(left,right))
    {
      beg_newLeft (obj,beg_getMaxObj(obj,left,right));
      beg_newRight(obj,beg_getMinObj(obj,left,right));
    }
    else
    {
      beg_newLeft (obj,beg_getMinObj(obj,left,right));
      beg_newRight(obj,beg_getMaxObj(obj,left,right));
    }
  }
  else
    if (!beg_wayStrict(beg_getRight(obj),beg_getLeft(obj)))
    {
      beg_newLeft (obj,beg_getMaxObj(obj,left,right));
      beg_newRight(obj,beg_getMinObj(obj,left,right));
    }
    else
    {
      beg_newLeft (obj,beg_getMinObj(obj,left,right));
      beg_newRight(obj,beg_getMaxObj(obj,left,right));
    }
}

/*}}}************************************************************************/
/*{{{                    beg_chkCovering()                                  */
/*                                                                          */
/* test le recouvrement de deux vectbiabl                                   */
/*                                                                          */
/****************************************************************************/
static int beg_chkCovering(vectbiabl_list *vbiabl,int left,int right)
{
  int          sl,sr;

  sl = vbiabl->LEFT;
  sr = vbiabl->RIGHT;

  if ((sl == left) && (sr == right))
    return 2;
  else if ((sl == right) && (sr == left))
    return 3;
  else if (!beg_way(sr,sl))
    if (!beg_way(right,left))
      return ((!beg_way(left,sr))  || (!beg_way(sl,right)));
    else
      return ((!beg_way(right,sr)) || (!beg_way(sl,left)));
  else
    if (!beg_way(right,left))
      return ((!beg_way(left,sl))  || (!beg_way(sr,right)));
    else
      return ((!beg_way(right,sl)) || (!beg_way(sr,left)));
}

/*}}}************************************************************************/
/*{{{                    beg_resolveExistingSig()                           */
/*                                                                          */
/* resout la complexite de traitement du a l'existence du signal            */
/*                                                                          */
/****************************************************************************/
static void beg_resolveExistingSig(int *oldleft,
                                   int *oldright,
                                   chain_list **oldabl,
                                   int left,int right, chain_list *abl)
{
  int        diff;

  if ( beg_way(*oldleft,*oldright) != beg_way(left,right))
  {
    invertVectAbl(abl);
    beg_invertIntArg(&left,&right);
  }

  if (beg_wayStrict(*oldleft,*oldright)) // downto
  {
    // l > r,ol > or
    if ( beg_way((diff = right-*oldleft-1),0))
      goto solLeft;
    // ol > or,l > r
    else if ( beg_way((diff = *oldright-1-left), 0))
      goto solRight;
    else if ( beg_wayStrict(left,*oldleft) || beg_wayStrict(*oldright,right))
      printf("cas non traiter(1), erreur\n");
    else
      goto solIn;
  }
  else // to
  {
    // l < r,ol < or
    if ( beg_way((diff = *oldleft-1-right),0))
      goto solLeft;
    // ol < or,l < r
    else if ( beg_way((diff = left-*oldright-1),0))
      goto solRight;
    else if (beg_way(right,*oldright) || beg_way(*oldleft,left))
      printf("cas non traiter(2), erreur\n");
    else
      goto solIn;
  }

solLeft :
  if (diff)
    *oldabl = concatAbl(createBitStr(genUBitStr(diff)),*oldabl);
  *oldabl   = concatAbl(abl,*oldabl);
  *oldleft  = left;
  return;

solRight :
  if (diff)
    *oldabl = concatAbl(*oldabl,createBitStr(genUBitStr(diff)));
  *oldabl   = concatAbl(*oldabl,abl);
  *oldright = right;
  return;

solIn :
  left  = abs(left - *oldleft);
  right = abs(right- *oldleft);
  if (!changeAblAtRange(*oldabl,left,right,abl,1))
    beg_error(BEG_ERR_EXISTSIG,NULL);
}

/*}}}************************************************************************/
/*{{{                    beg_freeBefig()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_freeBefig()
{
  beh_frebefig(BEG_HEAD_BEFIG);
}

/*}}}************************************************************************/
/*{{{                    beg_delBefig()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_delBefig(befig_list *fig)
{
  if (fig->NAME)
    beg_delEnv(fig);
  fig->NEXT         = NULL;
  begFreeCorrespHT(fig, fig->NAME);
  beh_frebefig(fig);
}

/*}}}************************************************************************/
/*{{{                    beg_canInsertAbl()                                 */
/*                                                                          */
/* test if we can insert an abl into another whitout conflicting            */
/*                                                                          */
/****************************************************************************/
/*static int beg_canInsertAbl(int ol,int or,int l,int r,chain_list *abl)
  {
  int            i;
  int            res = 1;
  chain_list    *x;
  static char   *str_u;

  str_u = namealloc("'u'");
  if (beg_way(ol,or))
  for (i = l; i >= r; i --)
  {
  x = getAblAtIndex(abl,ol,or,i);
  res *= (x->DATA == str_u);
  freeExpr(x);
  }
  else
  for (i = l; i <= r; i ++)
  {
  x = getAblAtIndex(abl,ol,or,i);
  res *= (x->DATA == str_u);
  freeExpr(x);
  }
  return res;
  }*/

/*}}}************************************************************************/
/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Text manipulation                                  */
/****************************************************************************/
/*{{{                    beg_genBitStr()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
       char *beg_genBitStr(char *text)
{
  char    buf[3072];
  int    i;
  int    j    = 0;

  buf[j++] = '"';
  switch (text[1])
  {
    case 'b' : case 'B' :
         for(i = 2; *(text+i) != '\0'; i ++)
           switch (*(text+i))
           {
             case '0' :            j += beg_genPutBinInBuf("0",&buf[j]);break;
             case '1' :            j += beg_genPutBinInBuf("1",&buf[j]);break;
             case 'z' : case 'Z' : j += beg_genPutBinInBuf("z",&buf[j]);break;
             case 'x' : case 'X' : j += beg_genPutBinInBuf("x",&buf[j]);break;
             case 'u' : case 'U' : j += beg_genPutBinInBuf("u",&buf[j]);break;
             default : break;
           }
         break;
    case 'o' : case 'O' :
         for(i = 2; *(text+i) != '\0'; i ++)
           switch (*(text+i))
           {
             case '0' :            j += beg_genPutBinInBuf("000",&buf[j]);break;
             case '1' :            j += beg_genPutBinInBuf("001",&buf[j]);break;
             case '2' :            j += beg_genPutBinInBuf("010",&buf[j]);break;
             case '3' :            j += beg_genPutBinInBuf("011",&buf[j]);break;
             case '4' :            j += beg_genPutBinInBuf("100",&buf[j]);break;
             case '5' :            j += beg_genPutBinInBuf("101",&buf[j]);break;
             case '6' :            j += beg_genPutBinInBuf("110",&buf[j]);break;
             case '7' :            j += beg_genPutBinInBuf("111",&buf[j]);break;
             case 'z' : case 'Z' : j += beg_genPutBinInBuf("zzz",&buf[j]);break;
             case 'x' : case 'X' : j += beg_genPutBinInBuf("xxx",&buf[j]);break;
             case 'u' : case 'U' : j += beg_genPutBinInBuf("uuu",&buf[j]);break;
             default : break;
           }
         break;
    case 'x' : case 'X' :
         for(i = 2; *(text+i) != '\0'; i ++)
           switch (*(text+i))
           {
             case '0' :            j += beg_genPutBinInBuf("0000",&buf[j]);break;
             case '1' :            j += beg_genPutBinInBuf("0001",&buf[j]);break;
             case '2' :            j += beg_genPutBinInBuf("0010",&buf[j]);break;
             case '3' :            j += beg_genPutBinInBuf("0011",&buf[j]);break;
             case '4' :            j += beg_genPutBinInBuf("0100",&buf[j]);break;
             case '5' :            j += beg_genPutBinInBuf("0101",&buf[j]);break;
             case '6' :            j += beg_genPutBinInBuf("0110",&buf[j]);break;
             case '7' :            j += beg_genPutBinInBuf("0111",&buf[j]);break;
             case '8' :            j += beg_genPutBinInBuf("1000",&buf[j]);break;
             case '9' :            j += beg_genPutBinInBuf("1001",&buf[j]);break;
             case 'a' : case 'A' : j += beg_genPutBinInBuf("1010",&buf[j]);break;
             case 'b' : case 'B' : j += beg_genPutBinInBuf("1011",&buf[j]);break;
             case 'c' : case 'C' : j += beg_genPutBinInBuf("1100",&buf[j]);break;
             case 'd' : case 'D' : j += beg_genPutBinInBuf("1101",&buf[j]);break;
             case 'e' : case 'E' : j += beg_genPutBinInBuf("1110",&buf[j]);break;
             case 'f' : case 'F' : j += beg_genPutBinInBuf("1111",&buf[j]);break;
             case 'z' : case 'Z' : j += beg_genPutBinInBuf("zzzz",&buf[j]);break;
             case 'x' : case 'X' : j += beg_genPutBinInBuf("xxxx",&buf[j]);break;
             case 'u' : case 'U' : j += beg_genPutBinInBuf("uuuu",&buf[j]);break;
             default : break;
           }
         break;
    default : break;
  }
  if (j == 2)
  {
    buf[0]   = '\'';
    buf[j++] = '\'';
  }
  else
    buf[j++] = '"';
  buf[j++]   = '\0';
  return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                    beg_genPutBinInBuf()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_genPutBinInBuf(char *var,char *buf)
{
  int         i;

  for (i = 0; *(var+i) != '\0'; i++)
    buf[i] = *(var+i);
  return i;
}

/*}}}************************************************************************/
/*{{{                    beg_mkName()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *beg_mkName(char *opname,int left,int right)
{
  char          buf[256];

  if (left == right)
    if (left == -1)
      sprintf(buf,"%s",opname);
    else
      sprintf(buf,"%s(%d)",opname,left);
  else
    sprintf(buf,"%s(%d:%d)",opname,left,right);

  return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                    beg_mkAblName()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *beg_mkAblName(char *opname,int left,int right)
{
  char          buf[256];

  if (left == right)
    if (left == -1)
      sprintf(buf,"%s",opname);
    else
      sprintf(buf,"%s %d",opname,left);
  else
    sprintf(buf,"%s %d:%d",opname,left,right);

  return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                    beg_get_vectname()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_get_vectname(char *signame,char **name,long *left,long *right,
                      char modeext)
{
  char       shrtname[1024];
  char      *leftparent;
  char      *colon;
  int        pos;
  char       mode;

  if (modeext == BEG_SEARCH_LOOP)
    mode = BEG_SEARCH_NAME;
  else if (modeext == BEG_SEARCH_ALOOP)
    mode = BEG_SEARCH_ABL;
  else
    mode = modeext;

  sprintf(shrtname,"%s",signame);
  leftparent = strchr(shrtname,mode);

  if (leftparent)
  {
    pos = leftparent - shrtname;
    shrtname[pos] = '\0';
    if (modeext == BEG_SEARCH_LOOP)
    {
      (leftparent+1)[strlen(leftparent+1)-1] = '\0';
      *left = (long)namealloc(leftparent+1);
    }
    else if (modeext == BEG_SEARCH_ALOOP)
      *left = (long)namealloc(leftparent+1);
    else if (left)
    {
      if ((colon = strchr(shrtname+pos+1,':')))
      {
        *left  = atoi(shrtname+pos+1);
        *right = atoi(colon+1);
      }
      else
      {
        *left  = atoi(shrtname+pos+1);
        *right = atoi(shrtname+pos+1);
        if (!*left && strchr(shrtname+pos+1,'0') != shrtname+pos+1)
        {
          *left  = -2;
          *right = -2;
        }
      }
    }
  }
  else
    if (left)
    {
      *left  = -1;
      *right = -1;
    }

  if (name)
    *name = namealloc(shrtname);
}

/*}}}************************************************************************/
/*{{{                    beg_get_loopvar()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *beg_get_loopvar(char *signame,char **name,char mode)
{
  char      *res;
  long       test = 0;

  beg_get_vectname(signame,name,(long *)&res,&test,mode);
  return (test != -1) ? res : NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_str2Abl()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *beg_str2Abl(char *str)
{
  return beg_genStr2Abl(str);
}

/*}}}************************************************************************/
/*{{{                    beg_isInt()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_isInt(char *str)
{
  char      *end;
  long       res;

  res = strtol(str,&end,10);
  if (*end == '\0' && res < (long)(((unsigned int)-1)>>1) )
    return (int)res;
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    beg_isBiablHZ()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_isBiablHZ(biabl_list *biabl)
{
  chain_list    *abl;
  unsigned int   i;

  abl   = biabl->VALABL;
  if ( !abl || ! ATOM(abl) || !isBitStr(VALUE_ATOM(abl)))
    return 0;
  else
  {
    for (i = 1; i < strlen(VALUE_ATOM(abl)) - 1; i ++)
      if (*(VALUE_ATOM(abl)+i) != 'z' && *(VALUE_ATOM(abl)+i) != 'Z')
        return 0;
    return 1;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_doLoopVar()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *beg_doLoopVar(loop_list *loop)
{
  loop_list *loopx;
  char       list[26], buf[8];
  int        i;

  for (i = 0; i < 26; i ++)
    list[i] = 1;
  for (loopx = loop; loopx; loopx = loopx->NEXT)
    if (!loopx->VARNAME[1])
      list[tolower(loopx->VARNAME[0]) - 'a'] = 0;
  for (i = 'i' - 'a'; i < 26; i ++)
    if (list[i])
    {
      sprintf(buf,"%c",'a'+i);
      
      return namealloc(buf);
    }
  for (i = 0; i < 'i' - 'a'; i ++)
    if (list[i])
    {
      sprintf(buf,"%c",'a'+i);
      
      return namealloc(buf);
    }
  
  return NULL;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Figure verification                                */
/****************************************************************************/
/*{{{                    beg_reloadConflictError()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_reloadConflictError()
{
  vectbiabl_list    *vbiabl;
  biabl_list        *biabl;
  ptype_list        *obj;
  BEG_OBJ            ref;

  BEG_PHASE_1 = 0;
  
  for (obj = BEG_CON_LIST; obj; obj = delptype(obj,obj->TYPE))
  {
    BEG_PHASE_2 = 1;
    beg_trace("Resolving conflict on %s (phase 1)",(char*)obj->TYPE);
    ref    = beg_isInternalSig((char*)(obj->TYPE));
    vbiabl = (vectbiabl_list *)reverse(obj->DATA);
    vbiabl = beg_sortVectBiabl(vbiabl);
    vbiabl = beg_detectLoopI  (vbiabl);
    vbiabl = beg_detectLoop   (vbiabl);
    for ( ; vbiabl; vbiabl = beg_delVbiablHead(vbiabl))
    {
      for (biabl = vbiabl->BIABL; biabl; biabl = biabl->NEXT)
        beg_addBiablToVectBiabl(ref,vbiabl->LEFT,vbiabl->RIGHT,biabl);
      vbiabl->BIABL = NULL;
    }
  }
  BEG_CON_LIST  = NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_freeBiabl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static biabl_list *beg_freeBiabl(biabl_list *biabl)
{
  while (biabl)
  {
    addhtitem(BEG_DEL_TABLE,biabl,0);
    biabl    = beh_delbiabl(biabl,biabl,'Y');
  }
  return NULL;
}

/*}}}************************************************************************/
/*{{{                    beg_delVbiablHead()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list *beg_delVbiablHead(vectbiabl_list *vbiabl)
{
  vectbiabl_list    *res    = vbiabl->NEXT;

  if ( vbiabl )
  {
    beg_freeBiabl(vbiabl->BIABL);
    mbkfree(vbiabl);
  }
  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_treatConflictError()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_treatConflictError(BEG_OBJ obj,int left,int right,
                                   biabl_list *biabl)
{
  char          *name;

  name      = beg_getName(obj);
  
  if (BEG_CONF_CPT > 1000)
    beg_error(BEG_ERR_CONFLICT_VECT,": %s\n",name);
  BEG_CONF_CPT ++;

  if (BEG_PHASE_1)
  {
    ptype_list      *newobj;
    //vectbiabl_list  *vbx, *vb;

    if ((long)(newobj = (ptype_list *)gethtitem(BEG_CON_TABLE,obj)) == EMPTYHT)
    {
      newobj        = addptype(BEG_CON_LIST,(long)name,NULL);
      BEG_CON_LIST  = newobj;
    }
    else
      newobj        = getptype(BEG_CON_LIST,(long)name);
    /*
    vb              = beh_addvectbiabl(NULL,left,right,biabl);
    for (vbx = newobj->DATA; vbx && vbx->NEXT; vbx = vbx->NEXT)
      ;
    if (vbx)
      vbx->NEXT     = vb;
    else
      newobj->DATA  = vb;
    */
    newobj->DATA    = beh_addvectbiabl(newobj->DATA,left,right,biabl);
    sethtitem(BEG_CON_TABLE,obj,(long)newobj);
  }
  else if (BEG_PHASE_2)
  {
    beg_trace("Resolving conflict on %s (phase 2)",name);
    beg_newVectBiabl(obj,beg_sortVectBiabl(beg_getVectBiabl(obj)));
    beg_newVectBiabl(obj,beg_detectLoopI  (beg_getVectBiabl(obj)));
    beg_newVectBiabl(obj,beg_detectLoop   (beg_getVectBiabl(obj)));
    BEG_PHASE_2 = 0;
    beg_addBiablToVectBiabl(obj,left,right,biabl);
  }
  else
  {
    int          way    , i;
    biabl_list  *new;

    beg_trace("Resolving conflict on %s by decompressing (phase 3)",name);
    way     = left - right;
    if (way)
    {
      way   = way / abs(way);
      for ( i = left - way ; way*(i - right) >= 0; i -= way )
      {
        new = beh_explodeBiabl(biabl,i,name,left,right,beg_resolutionFunc);
        if (new)
          beg_addBiablToVectBiabl(obj,i,i,new);
        else
          break;
      }
    }
    else
      beg_addBiablToVectBiabl(obj,left,right,biabl);
  }

  BEG_CONF_CPT --;
}

/*}}}************************************************************************/
/*{{{                    beg_resolutionFunc()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_resolutionFunc(char *name, int index, biabl_list *new)
{
  BEG_OBJ      ref;

  ref = beg_isInternalSig(name);
  beg_addBiablToVectBiabl(ref,index,index,new);
}

/*}}}************************************************************************/
/*{{{                    beg_verifyVectPor()                                */
/*                                                                          */
/* verify that we only have scalar por on scalar list                       */
/* ---- 29 10 2002 : declared obsolete                                      */
/*                   if declared vector, remain vector                      */
/*                                                                          */
/****************************************************************************/
/*static void beg_verifyVectPor(befig_list *befig)
  {
  bevectpor_list    *vectpor;
  char              *name;

  for (vectpor = befig->BEVECTPOR; vectpor; vectpor = vectpor->NEXT)
  if (vectpor->LEFT == vectpor->RIGHT)
  {
  name = beg_mkName(vectpor->NAME,vectpor->LEFT,vectpor->LEFT);
  beg_def_bepor(name,vectpor->DIRECTION,vectpor->TYPE);
  beh_delbevectpor(befig->BEVECTPOR,vectpor);
  }
  }*/

/*}}}************************************************************************/
/*{{{                    beg_doITreatThisOne()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_doITreatThisOne(BEG_OBJ sig,int left,int right)
{
  int         sl, sr;

  sl = beg_getLeft(sig);
  sr = beg_getRight(sig);
  if ( left == -2 || sl == -2)
    return 0;

  beg_chkWay(sig,left,right);

  if ( !beg_way(sr,sl))
    return ( (!beg_way(sl,left))  || (!beg_way(right,sr)));
  else
    return ( (!beg_way(sr,right)) || (!beg_way(left,sl)));
}

/*}}}************************************************************************/
/*{{{                    beg_verifyAux()                                    */
/*                                                                          */
/* verify that all auxilliary has not been declared external                */
/*                                                                          */
/****************************************************************************/
static void beg_verifyAux()
{
  beaux_list        *aux;
  beaux_list        *naux;
  bevectaux_list    *vectaux;
  bevectaux_list    *nvectaux;

  for (aux = BEG_WORK_BEFIG->BEAUX; aux; )
  {
    if (beg_isPor(aux->NAME))
    {
      naux     = aux->NEXT;
      beg_convertAuxInOut(BEG_WORK_BEFIG,aux,BEG_TYPE_SINGAUX);
      aux      = naux;
    }
    else
      aux      = aux->NEXT;
  }
  for (vectaux = BEG_WORK_BEFIG->BEVECTAUX; vectaux; )
  {
    if (beg_isPor(vectaux->NAME))
    {
      nvectaux = vectaux->NEXT;
      beg_convertAuxInOut(BEG_WORK_BEFIG,vectaux,BEG_TYPE_VECTAUX);
      vectaux  = nvectaux;
    }
    else
      vectaux  = vectaux->NEXT;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_isAllDeclared()                                */
/*                                                                          */
/* detect if all entry of expr are declared otherwise it'll be maid         */
/*                                                                          */
/****************************************************************************/
static chain_list *beg_IsAllDeclared(chain_list *expr,loop_list *loop)
{
  char          *name;
  char          *opname;
  long           left, right;
  BEG_OBJ        obj;
  chain_list    *res        = NULL;
  char          *signame;
  chain_list    *support;

  for ( support = supportChain_listExpr(expr); support;
        support = delchain(support,support))
  {
    name = support->DATA;
    beg_get_vectname(name,&opname,&left,&right,BEG_SEARCH_ABL);

    if (left == -2)
    {
      beg_GuessRange(name,&opname,&left,&right,loop,BEG_SEARCH_ABL);
      name  = beg_mkAblName(opname,left,right);
    }
    if ( (obj = beg_isPor(opname)) )
    {
      beg_chkWay(obj,left,right);
      beg_makeInPor(obj);
      if (beg_doITreatThisOne(obj,left,right))
      {
        signame = beg_getLongName(obj);
        beg_error(BEG_ERR_POR,": use as %s defined %s", name,signame);
        mbkfree(signame);
      }
    }
    else if ((obj = beg_isInternalSig(opname)))
      beg_chkWay(obj,left,right);
    res     = addchain(res,name);
  }
  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_addBerinsInt()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void beg_addBerinsInt(char *berin,BEG_OBJ obj)
{
  char          *name;
  long           left,right;
  berin_list    *rin;

  beg_get_vectname(berin,&name,&left,&right,BEG_SEARCH_ABL);
  if ((long)(rin = (berin_list *)gethtitem(BEG_RIN_TABLE,name)) == EMPTYHT)
  {
    rin = beh_addberin(BEG_WORK_BEFIG->BERIN,name);
    addhtitem(BEG_RIN_TABLE,name,(long)rin);
    rin->LEFT  = left;
    rin->RIGHT = right;
    BEG_WORK_BEFIG->BERIN = rin;
    if (rin->LEFT == rin->RIGHT)
      if (gethtitem(BEG_RIN_ADJUS,name) == EMPTYHT)
        addhtitem(BEG_RIN_ADJUS,name,0);
  }
  else
  {
    if (rin->LEFT != -1)
    {
      if ( gethtitem(BEG_RIN_ADJUS,name) != EMPTYHT && left != right )
        delhtitem(BEG_RIN_ADJUS,name);
      // ne sert a rien?
      //beg_adjustDomain(BEG_OBJ_NEW(rin,BEG_MASK_RIN),left,right);
    }
  }
  if (obj)
    beg_addBerin(rin,obj);
}

/*}}}************************************************************************/
/*{{{                    beg_addBerins()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_addBerins(chain_list *berins,BEG_OBJ obj)
{
  chain_list    *chainx;

  for (chainx = berins; chainx; chainx = delchain(chainx,chainx))
    beg_addBerinsInt(chainx->DATA,obj);
}

/*}}}************************************************************************/
/*{{{                    beg_genAblFBitStr()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*static chain_list *beg_genAblFBitStr(char *bitstr)
  {
  int            i;
  chain_list    *abl    = NULL;
  chain_list    *expr1  = NULL;

  for (i = 0; *(bitstr+i) != '\0'; i ++)
  {
  switch (*(bitstr+i))
  {
  case '0' : expr1 = createAtom("'0'"); break;
  case '1' : expr1 = createAtom("'1'"); break;
  default  : break ;
  }
  abl = (abl) ? concatAbl(abl,expr1) : expr1;
  }
  return abl;
  }*/

/*}}}************************************************************************/
/*{{{                    beg_chk_befig()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_chk_befig()
{
  berin_list    *rin;
  BEG_OBJ        obj;

  for (rin = BEG_WORK_BEFIG->BERIN; rin; rin = rin->NEXT)
    if ( (obj = beg_isPor(rin->NAME)) )
    {
      beg_makeInPor(obj);
      if (beg_isVect(obj))
      {
        if (rin->LEFT != -1)
          beg_adjustDomain(obj,rin->LEFT,rin->RIGHT);
        else
          beg_error(BEG_ERR_SING_N_VECT,rin->NAME);
      }
    }
    else if ( (obj = beg_isInternalSig(rin->NAME)) )
    {
      if (beg_doITreatThisRin(obj,rin->LEFT,rin->RIGHT))
      {
        beg_convertAuxInOut(BEG_WORK_BEFIG,
                            BEG_OBJ_DATA(obj),BEG_OBJ_TYPE(obj));
        beg_def_por(beg_mkName(rin->NAME,rin->LEFT,rin->RIGHT),'B');
      }
    }
    else
      beg_def_por(beg_mkName(rin->NAME,rin->LEFT,rin->RIGHT),'I');
}

/*}}}************************************************************************/
/*{{{                    beg_doITreatThisRin()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_doITreatThisRin(BEG_OBJ obj, int left, int right)
{
  int        res;
  BEG_RIN_TREAT = 1;
  res           = beg_doITreatThisOne(obj,left,right);
  BEG_RIN_TREAT = 0;

  return res;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Special feature detection                          */
/****************************************************************************/
///*{{{                    beg_holeDetection()                                */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static vectbiabl_list *beg_holeDetection(vectbiabl_list *vbiabl)
//{
//  vectbiabl_list    *vbiablx    = vbiabl;
//  vectbiabl_list    *res         = NULL;
//  int                 way        = 1;
//
//  for (; vbiablx; vbiablx = res)
//  {
//    if (!(res = vbiablx->NEXT))
//      break;
//    else
//      if ((way &= beg_way(vbiablx->LEFT,vbiablx->RIGHT)))
//      {
//        if (vbiablx->LEFT != (vbiablx->NEXT->RIGHT - 1))
//          break;
//      }
//      else
//      {
//        if (vbiablx->RIGHT != (vbiablx->NEXT->LEFT - 1))
//          break;
//      }
//  }
//  return res;
//}
//
///*}}}************************************************************************/
/*{{{                    beg_compactAbl()                                   */
/*                                                                          */
/* abl compactage                                                           */
/****************************************************************************/
chain_list *beg_compactAbl(chain_list *expr)
{
  chain_list    *ssabl;
  chain_list    *abl;
  int            done   = 0;

  if (OPER(expr) == CAT)
  {
    abl = CDR(expr);
    while (abl && CDR(abl))
    {
      ssabl = CAR(abl);
      if (ATOM(ssabl))
        if (ATOM(CADR(abl)) && beg_mergeAtom(ssabl,CADR(abl)))
          CDR(abl) = delchain(CDR(abl),CDR(abl));
        else
        {
          abl  = CDR(abl);
          done = 1;
        }
      else
      {
        ssabl  = beg_compactAbl(ssabl);
        abl    = CDR(abl);
      }
    }
    if (!done)
    {
      abl       = CADR(expr);
      CADR(expr) = NULL;
      freeExpr(expr);
      return abl;
    }
    else
      return expr;
  }
  else
  {
    abl = expr;
    while ((abl = CDR(abl)))
      CAR(abl) = beg_compactAbl(CAR(abl));
    return expr;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_mergeAtom()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_mergeAtom(chain_list *atm1,chain_list *atm2)
{
  char      *name1  = VALUE_ATOM(atm1);
  char      *name2  = VALUE_ATOM(atm2);
  char      *rad1, *rad2;
  long       left1, left2, right1, right2;

  beg_get_vectname(name1,&rad1,&left1,&right1,BEG_SEARCH_ABL);
  beg_get_vectname(name2,&rad2,&left2,&right2,BEG_SEARCH_ABL);
  if (rad1 == rad2)
  {
    if (left2 == right2)
    {
      if (left1 == right1)
        if (abs(right1-left2) == 1)
        {
          delchain(atm2,atm2);
          VALUE_ATOM(atm1) = createVectAtomName(rad1,left1,right2);
          return 1;
        }
        else
          return 0;
      else if (left1 > right1)
        if (right1 == left2+1)
        {
          delchain(atm2,atm2);
          VALUE_ATOM(atm1) = createVectAtomName(rad1,left1,right2);
          return 1;
        }
        else
          return 0;
      else
        if (right1 == left2-1)
        {
          delchain(atm2,atm2);
          VALUE_ATOM(atm1) = createVectAtomName(rad1,left1,right2);
          return 1;
        }
        else
          return 0;
    }
    else if (beg_way(left1,right1) == beg_way(left2,right2)
             && abs(right1-left2) == 1)
    {
      delchain(atm2,atm2);
      VALUE_ATOM(atm1) = createVectAtomName(rad1,left1,right2);
      return 1;
    }
    else
      return 0;
  }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    beg_boolDelNot()                                   */
/*                                                                          */
/* boolean compactage for driver                                            */
/****************************************************************************/
char *beg_boolDelNot(chain_list *expr, chain_list **res)
{
  if (OPER(expr) == CAT)
  {
    chain_list      *tmpx;
    chain_list      *abl;
    char             tmp[255];
    int              i, j;
    int              not;

    *res            = copyExpr(expr);
    for (i = 1, tmpx = (*res)->NEXT; tmpx; tmpx = tmpx->NEXT)
    {
      abl           = tmpx->DATA;
      not           = 0;
      if (!ATOM(abl) && OPER(abl) == NOT)
      {
        abl         = notExpr(abl);
        not         = 1;
      }
      tmpx->DATA    = abl;
      for (j = verifyVectAbl(abl); j; j--)
        tmp[i++]    = (not) ? '0' : '1' ;
    }
    tmp[0]          = (i > 2) ? '"' : '\'';
    tmp[i]          = (i > 2) ? '"' : '\'';
    tmp[i+1]        = '\0';
    
    return makeBitStr(tmp);
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    Loop & Vector detection                            */
/****************************************************************************/
/*{{{                    beg_compact()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_compact()
{
//  BEG_DONT_TOUCH    = 0;
  BEG_COMPACT   = 1;
  
  do
  {
    //  beg_compactAbl(BEG_WORK_BEFIG->BEVECTAUX->ABL);
    beg_singPerform(beg_detectLoopII);
    beg_perform(beg_detectLoopI);
    beg_perform(beg_detectLoop);
  }
  //while (BEG_COMPACT);
  while (0);
}

/*}}}************************************************************************/
/*{{{                    beg_detectLoopI()                                  */
/*                                                                          */
/* detect loop on biabl with beg_detectLoopII() function                    */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list *beg_detectLoopI(vectbiabl_list *vbiabl)
{
  biabl_list        *biabl;

  if (vbiabl)
  {
//    BEG_DONT_TOUCH      = vbiabl->BIABL->FLAG & BEH_CND_SELECT;
    // loop detection on biabl
    if ((biabl = beg_detectLoopII(vbiabl->BIABL)))
      vbiabl->BIABL     = biabl;
    else
    {
      return vbiabl;
    }
    beg_detectLoopI(vbiabl->NEXT);
  }
  return vbiabl;
}

/*}}}************************************************************************/
/*{{{                    beg_hasPrecede()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_hasPrecede(biabl_list *biabl)
{
  return ((biabl->FLAG&BEH_CND_PRECEDE) == BEH_CND_PRECEDE);
}

/*}}}************************************************************************/
/*{{{                    beg_detectLoopII()                                 */
/*                                                                          */
/* loop detection on biabl                                                  */
/*                                                                          */
/****************************************************************************/
static biabl_list *beg_detectLoopII(biabl_list *biabl)
{
  // with select are strictly not compacted
  if ( ! (biabl->FLAG&BEH_CND_SELECT) || !biabl->NEXT )
  {
    vectbiabl_list  *vbiabl;
    biabl_list      *biablh, *biablx, *last;
    int              i;

    last        = NULL;
    vbiabl      = NULL;
    i           = 0;
    for (biablx = biabl; biablx; biablx = biablx->NEXT)
    {
      if ( !beg_hasPrecede(biablx) )
      {
        if (last)
          last->NEXT    = NULL;
        biablh  = biablx;
        vbiabl  = beh_addvectbiabl(vbiabl,i,i,biablh);
        i ++;
      }
      last      = biablx;
    }

    if (vbiabl==NULL) return biabl;
    BEG_ADD_INDEX   = 0;
    vbiabl      = (vectbiabl_list *)reverse((chain_list *)vbiabl);
    vbiabl      = beg_detectLoop(vbiabl);
    BEG_ADD_INDEX   = 1;

    biablh      = vbiabl->BIABL;
    for (last = NULL; vbiabl; vbiabl = beh_delvectbiabl(vbiabl,vbiabl,'Y'))
    {
      if (last)
        last->NEXT  = vbiabl->BIABL;
      for (biablx = vbiabl->BIABL; biablx; biablx = biablx->NEXT)
        last        = ( ! biablx->NEXT) ? biablx : NULL;
      vbiabl->BIABL = NULL;
    }
    return biablh;
  }
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    guessMinLoopDetected()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
void guessMinLoopDetected(beg_ld_list *ld)
{
  vectbiabl_list    *vb, *vbx, *minvb;
  beg_ld_list       *ldx;
  int                i, min, step, cpt;

  if (ld->NEXT)
  {
    vb              = ld->VBIABL;
    min             = ((unsigned int)(-1)>>1);

    for (ldx = ld; ldx; ldx = ldx->NEXT)
      if (vb != ldx->VBIABL)
        return;
      else
        for (vbx = vb, i = 0; vbx; vbx = vbx->NEXT, i ++)
          if (vbx == ldx->LAST)
          {
            min     = (i < min) ? i : min;
            step    = ldx->STEP;
            cpt     = ldx->CPT;
            break;
          }

    minvb           = NULL;
    // get the min
    for (vbx = vb, i = 0; vbx; vbx = vbx->NEXT, i ++)
      if (i == min)
        minvb       = vbx;

    // once min found apply it to everything
    if (minvb)
      for (ldx = ld; ldx; ldx = ldx->NEXT)
      {
        ldx->LAST   = minvb;
        ldx->CPT    = cpt/step * ldx->STEP;
      }
  }
}

/*}}}************************************************************************/
/*{{{                    beg_detectLoop()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static vectbiabl_list *beg_detectLoop(vectbiabl_list *vbiabl)
{
  beg_ld_list       *loopdetect;
  chain_list        *cnd, *val, *bascs, *basvs, *refcs, *refvs;
  biabl_list        *base, *biablx;
  vectbiabl_list    *vbiablx;
  unsigned int       time;
  char              *lvar;
  int                step, cpt, left, right, vectorize, onepass, whichbiabl;
  int                i, leftx, rightx;
  loop_list         *loop;

  if (vbiabl && ! (vbiabl->BIABL->FLAG&BEH_CND_SELECT) )
  {
    loopdetect      = NULL;
    vectorize       = (BEG_COMPRESS_MAX) ? 0 : 1;
    onepass         = 0;
    whichbiabl      = 0;
    left            = vbiabl->LEFT;
    right           = vbiabl->RIGHT;
    step            = abs(left - right) + 1;

    for (base = vbiabl->BIABL; base; base = base->NEXT, whichbiabl ++)
    {
      refcs         = NULL;
      refvs         = NULL;
      loop          = beh_getloop(base);
      lvar          = beg_doLoopVar(loop);
      time          = base->TIME;
      cnd           = base->CNDABL;
      val           = base->VALABL;
      bascs         = fullSupportChain_listExpr(cnd);
      basvs         = fullSupportChain_listExpr(val);
      loopdetect    = beg_addLD(loopdetect,base,bascs,basvs,vbiabl,step,loop);
      cpt           = loopdetect->CPTINIT;
      // first step
      // detecting loop
      for (vbiablx = vbiabl->NEXT; vbiablx; vbiablx = vbiablx->NEXT)
      {
        biablx      = vbiablx->BIABL;
        leftx       = vbiablx->LEFT;
        rightx      = vbiablx->RIGHT;

        for (i = 0; i < whichbiabl && biablx; i ++)
          biablx    = biablx->NEXT;
        if (!biablx)
          break;
        if ( abs(leftx - rightx) == step-1 && leftx == (cpt+1)*step + left)
        {
          onepass   = 1;
          if (!beg_isSameByLoopVar(base,biablx,bascs,basvs,&refcs,&refvs,
                                   lvar,cpt,step))
          {
            if (BEG_COMPRESS_MAX && vbiablx != vbiabl->NEXT)
              break;
            else
            {
              vbiabl->NEXT  = beg_detectLoop(vbiabl->NEXT);
              beg_freeLD(loopdetect);
              return vbiabl;
            }
          }
          if (vectorize)
            vectorize  &= equalExpr(cnd,biablx->CNDABL);
          time          = (time > biablx->TIME) ? time : biablx->TIME;
        }
        cpt ++;
      }

      // loop detected
      // processing modification
      loopdetect->LAST      = vbiablx;
      loopdetect->REFVS     = refvs;
      loopdetect->REFCS     = refcs;
      loopdetect->VECTORIZE = vectorize;
      loopdetect->CPT       = cpt;
      loopdetect->TIME      = time;
      loopdetect->LVAR      = lvar;

      // detection failed cleanning structure
      if (!onepass)
      {
        beg_cleanFreeSupport(bascs,refcs);
        beg_cleanFreeSupport(basvs,refvs);
        freechain(bascs);
        freechain(basvs);
        beg_freeLD(loopdetect);
        BEG_COMPACT = 0;

        return vbiabl;
      }
    }
    guessMinLoopDetected(loopdetect);
    if (beg_applyLD(loopdetect))
      vbiabl->NEXT          = beg_detectLoop(loopdetect->LAST);
    beg_freeLD(loopdetect);

    return vbiabl;
  }
  else
    return vbiabl;
}

/*}}}************************************************************************/
/*{{{                    beg_changeName()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *beg_changeName(char *bname, char *name, char *loopvar, 
                            int curindex, int step)
{
  long       left, bleft, right, bright;
  int        diff, isint, mul;
  char      *opname, *bopname;
  char       signe;
  char       buf    [2048];
  char      *varname, *bvarname;
  char      *eqt;

  if (bname == name)
    return mbkstrdup(name);
  else
  {
    beg_get_vectname(name ,&opname ,&left ,&right ,BEG_SEARCH_ABL);
    beg_get_vectname(bname,&bopname,&bleft,&bright,BEG_SEARCH_ABL);
    if (opname == bopname)
    {
      if (left == -2 && bleft == -2)
      { // loop
        varname  = beg_get_loopvar(name  ,&opname  ,BEG_SEARCH_ALOOP);
        bvarname = beg_get_loopvar(bname ,&bopname ,BEG_SEARCH_ALOOP);

        sprintf(buf,"(%s)-(%s)",varname,bvarname);
        eqt      = eqt_getSimpleEquation(buf);
        diff     = beg_isInt(eqt);
        isint    = (!diff) ? ((strlen(eqt) == 1) ? 1 : 0 ) : 1;
        mbkfree(eqt);

        if (isint)
        {
          diff   = diff/(curindex + 1);// + step);
          sprintf(buf,"%s+%d*%s",bvarname,diff,loopvar);
          eqt    = eqt_getSimpleEquation(buf);
          sprintf(buf,"%s %s",opname,eqt);
          mbkfree(eqt);
        }
        else
        {
          sprintf(buf,"%s-%d+%d*%s",bvarname,curindex,step,loopvar);
          eqt    = eqt_getSimpleEquation(buf);
          sprintf(buf,"%s %s",opname,eqt);
          mbkfree(eqt);
        }
        return mbkstrdup(buf);
      }
      else if (left == right && bleft == bright)
      {
        mul      = (left - bleft) / (curindex + 1);
        diff     = left - (curindex + 1) * mul; // try to put step
//        diff     = left - (curindex + step);
        signe    = (diff > 0) ? '+' :(diff < 0) ? '-' : '\0';
        if (mul - 1)
          sprintf(buf,"%s*%d%c%d",loopvar,mul,signe,abs(diff));
        else
          sprintf(buf,"%s%c%d",loopvar,signe,abs(diff));
        eqt    = eqt_getSimpleEquation(buf);
        sprintf(buf,"%s %s",bopname,eqt);
        mbkfree(eqt);

        return mbkstrdup(buf);
      }
      else/* if ((left - right) == (bleft - right))
      {
        diff     = left - (curindex + 1); // try to put step
        signe    = (diff > 0) ? '+' :(diff < 0) ? '-' : '\0';
        sprintf(buf,"%s %s%c%d:%s%c%d",bopname,loopvar,signe,abs(diff),
                loopvar,signe,right+diff);
        return mbkstrdup(buf);
      }
      else*/
        return NULL;
    }
    else
      return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_isSameSupport()                                */
/*                                                                          */
/* refs   : reference support                                               */
/****************************************************************************/
static int beg_isSameSupport(chain_list *refs, chain_list *s)
{
  while (1)
  {
    if (!refs && !s)
      return 1;
    else if (!refs || !s || (strcmp(refs->DATA,s->DATA)))
      return 0;
    else
    {
      refs = refs->NEXT;
      s    = s->NEXT;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    beg_isSameByLoopVar()                              */
/*                                                                          */
/* bcs : base  condition support                                            */
/* bvs : base  value     support                                            */
/****************************************************************************/
static int beg_isSameByLoopVar(biabl_list *bbiabl, biabl_list *biabl,
                               chain_list *bascs , chain_list *basvs,
                               chain_list **refcs, chain_list **refvs,
                               char *lvar, int cpt, int step)
{
  chain_list    *curcs;
  chain_list    *curvs;
  chain_list    *rescs;       // res support
  chain_list    *resvs;       // res support
  int            res        = 1;

  if ( bbiabl && biabl &&
       equalVarExpr(biabl->CNDABL,bbiabl->CNDABL) &&
       equalVarExpr(biabl->VALABL,bbiabl->VALABL))
  {
    // condition
    curcs    = fullSupportChain_listExpr(biabl->CNDABL);
    rescs    = beg_cmpSupport(bascs,curcs,lvar,cpt,step);
    if (*refcs)
    {
      res   &= (beg_isSameSupport(*refcs,rescs)) ? 1 : 0 ;
      beg_freeSupport(rescs);
    }
    else if (rescs)
      *refcs = rescs;
    else
      res = 0;
    freechain(curcs);
    // value
    curvs    = fullSupportChain_listExpr(biabl->VALABL);
    resvs    = beg_cmpSupport(basvs,curvs,lvar,cpt,step);
    if (*refvs)
    {
      res   &= (beg_isSameSupport(*refvs,resvs)) ? 1 : 0 ;
      beg_freeSupport(resvs);
    }
    else if (resvs)
      *refvs = resvs;
    else
      res = 0;
    freechain(curvs);

    return res;
  }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    beg_cmpSupport()                                   */
/*                                                                          */
/* bass   : base support                                                    */
/* cindex : curent index                                                    */
/****************************************************************************/
static chain_list *beg_cmpSupport_recursive(chain_list *bass, chain_list *s,
                                  char *loopvar, int cindex, int step, int *failed)
{
  chain_list    *chainx     = NULL;
  char          *newname;

  if (bass)
  {
    chainx      = beg_cmpSupport_recursive(bass->NEXT,s->NEXT,loopvar,cindex,step,failed);
    if (*failed) return NULL;
    newname     = beg_changeName(bass->DATA,s->DATA,loopvar,cindex,step);
    if (newname)
      chainx    = addchain(chainx,newname);
    else
    {
      freechain(chainx);
      chainx    = NULL;
      *failed=1;
    }
    
    return chainx;
  }
  else
    return NULL;
}

static chain_list *beg_cmpSupport(chain_list *bass, chain_list *s,
                                  char *loopvar, int cindex, int step)
{
  int failed=0;
  return beg_cmpSupport_recursive(bass, s, loopvar, cindex, step, &failed);
}
/*}}}************************************************************************/
/*{{{                    beg_freeSupport()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_freeSupport(chain_list *support)
{
  if (support)
  {
    beg_freeSupport(support->NEXT);
    mbkfree(support->DATA);
    delchain(support,support);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_specialFreVBiabl()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_specialFreVBiabl(vectbiabl_list *head, vectbiabl_list *last)
{
  while (head && head != last)
    head = beg_delVbiablHead(head);
}

/*}}}************************************************************************/
/*{{{                    beg_cleanFreeSupport()                             */
/*                                                                          */
/* Warning : u should not suppress                                          */
/*                nameallocated name (contained by base support)            */
/****************************************************************************/
static void beg_cleanFreeSupport(chain_list *basesupport,chain_list *support)
{
  if (support)
  {
    beg_cleanFreeSupport(basesupport->NEXT,support->NEXT);
    if (basesupport->DATA != support->DATA)
      mbkfree(support->DATA);
    delchain(support,support);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_modifyAblVect()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_modifyAblVect(chain_list *abl, chain_list *sup, char *lvar)
{
  chain_list    *abls;
  chain_list    *chainx;
  char          *vname;
  char          *name;
  long           left,right;
  loop_list     *loop;

  abls = fullSupportChain_listExpr(abl);

  BEG_BLOCK_INV  = 1;
  for (chainx = abls; chainx && sup; chainx = chainx->NEXT, sup = sup->NEXT)
  {
    beg_get_vectname(lvar,&name,&left,&right,BEG_SEARCH_NAME);
    loop    = beh_newloop(NULL,name,NULL,left,right);
    if ( beg_GuessRange(sup->DATA,&name,&left,&right,loop,BEG_SEARCH_ABL)
         && left != right)
    {
      vname = beg_mkAblName(name,left,right);
      replaceAtomExpr(abl,chainx->DATA,namealloc(vname));
    }
    else
    {

      chain_list  *expr;

      expr = replicateAbl(createAtom(chainx->DATA),abs(left-right) + 1);
      replaceAtomByExpr(abl,chainx->DATA,expr);
      freeExpr(expr);
    }
    mbkfree(loop);
  }
  BEG_BLOCK_INV  = 0;
  freechain(abls);
}

/*}}}************************************************************************/
/*{{{                    beg_modifyAblLoop()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_modifyAblLoop(chain_list *abl, chain_list *support)
{
  chain_list    *abls;
  chain_list    *chainx;

  abls = fullSupportChain_listExpr(abl);

  for (chainx = abls; ; chainx = chainx->NEXT, support = support->NEXT)
    if (!chainx || ! support)
      break;
    else
      replaceAtomExpr (abl,chainx->DATA,namealloc(support->DATA));

  freechain(abls);
}

/*}}}************************************************************************/
/*{{{                    beg_addLD()                                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static beg_ld_list *beg_addLD(beg_ld_list *next, biabl_list *base,
                              chain_list *bascs, chain_list *basvs,
                              vectbiabl_list *vbase, int step,
                              loop_list *loop)
{
  beg_ld_list   *res;

  res            = (beg_ld_list*)mbkalloc(sizeof(struct beg_ld));
  res->NEXT      = next;
  res->BASE      = base;
  res->REFCS     = NULL;
  res->REFVS     = NULL;
  res->BASCS     = bascs;
  res->BASVS     = basvs;
  res->LOOP      = loop;
  //  res->CPTINIT  = vbase->LEFT;
  res->CPTINIT   = 0;
  res->VECTORIZE = 0;
  res->STEP      = step;
  res->TIME      = 0;
  res->VBIABL    = vbase;
  res->LAST      = NULL;
  res->LVAR      = NULL;
  return res;
}

/*}}}************************************************************************/
/*{{{                    beg_freeLD()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_freeLD(beg_ld_list *ld)
{
  if (ld)
  {
    beg_freeLD(ld->NEXT);
    mbkfree(ld);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_applyLD()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int beg_applyLD(beg_ld_list *ld)
{
  char          *lvar;
  beg_ld_list   *ldx;
  char          *var;
  long           left, right;
  loop_list     *loop;
  int            loopleft, loopright;
  char           loopindex[255];
  int            tmp;
  eqt_ctx       *ctx;

  for (ldx = ld; ldx; ldx = ldx->NEXT)
    if (ldx->LAST != ld->LAST)
      return 0;
  for (ldx = ld; ldx; ldx = ldx->NEXT)
  {
    lvar                = ldx->LVAR;
    loop                = NULL;
    left                = ldx->VBIABL->LEFT;
    right               = ldx->VBIABL->RIGHT;
    ldx->BASE->TIME     = ldx->TIME;
    if (ldx->VECTORIZE && !ldx->LOOP)
    { // vectorisation
      var               = beg_mkName(lvar,ldx->CPTINIT,
                                     ldx->CPT + ldx->CPTINIT);
      beg_modifyAblVect(ldx->BASE->VALABL,ldx->REFVS,var);
    }
    else
    { // loop
      if (ldx->LOOP && ldx->LOOP->INDEX)
      {
        sprintf(loopindex,"%s+%s*%d",ldx->LOOP->INDEX,
                (BEG_ADD_INDEX) ? lvar : "1",ldx->STEP);
        var             = eqt_getSimpleEquation(loopindex);
        sprintf(loopindex,"%s",var);
        mbkfree(var);
      }
      else if (left != 0)
        sprintf(loopindex,"%s+%ld",(BEG_ADD_INDEX) ? lvar : "0",left);
      else
        sprintf(loopindex,"%s",(BEG_ADD_INDEX) ? lvar : "0");

      beg_modifyAblLoop(ldx->BASE->CNDABL,ldx->REFCS);
      beg_modifyAblLoop(ldx->BASE->VALABL,ldx->REFVS);

      loopleft          = ldx->CPTINIT;
      loopright         = ldx->CPT + ldx->CPTINIT;

      beh_addloop( ldx->BASE,(BEG_ADD_INDEX) ? loopindex :
                   (ldx->LOOP) ? ldx->LOOP->INDEX : NULL,
                   lvar,loopleft,loopright);
    }
    loop                = beh_getloop(ldx->BASE);
    tmp                 = BEG_USER_WAY;
    
    BEG_USER_WAY        = (left > right) ? 0 : 1 ;
    if (loop->INDEX)
    {
      ctx = eqt_init(2);
      beg_calcRange(ctx,loop->INDEX,loop,&left,&right);
      eqt_term(ctx);
    }
    BEG_USER_WAY        = tmp;
    
    ldx->VBIABL->LEFT   = left;
    ldx->VBIABL->RIGHT  = right;
    // a little shower
    beg_cleanFreeSupport(ldx->BASCS,ldx->REFCS);
    beg_cleanFreeSupport(ldx->BASVS,ldx->REFVS);
    freechain(ldx->BASCS);
    freechain(ldx->BASVS);
    beg_specialFreVBiabl(ldx->VBIABL->NEXT,ldx->LAST);
    ldx->VBIABL->NEXT   = ldx->LAST;
  }
  
  return 1;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Figure inclusion                                   */
/****************************************************************************/
/*{{{                    beg_eatVectBiabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectBiabl(BEG_OBJ(*func)(char*,biabl_list*,char,long,long),
                             char *name,vectbiabl_list *vbiabl,long flags)
{
  char                *vname;

  if (vbiabl)
  {
    beg_eatVectBiabl(func,name,vbiabl->NEXT,flags);
    vname = beg_mkName(name,vbiabl->LEFT,vbiabl->RIGHT);
    beg_eatBiabl(func,vname,vbiabl->BIABL, flags);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatBiabl()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatBiabl(BEG_OBJ (*func)(char*,biabl_list*,char,long,long),
                         char *name,biabl_list *biabl,long flags)
{
  biabl_list        *next;
  BEG_OBJ            obj;
  chain_list        *berins = NULL;
  chain_list        *berinx = NULL;
  loop_list         *loop;
  char              *var1   = NULL;
  char              *var2   = NULL;

  for (; biabl; biabl = next)
  {
    loop        = beh_getloop(biabl);
    berinx      = (biabl->VALABL) ?
      beg_IsAllDeclared(biabl->VALABL,loop) : NULL;
    berins      = (biabl->CNDABL) ?
      beg_IsAllDeclared(biabl->CNDABL,loop) : NULL;
    next        = biabl->NEXT;
    biabl->NEXT = NULL;
    obj         = (*func) (name,biabl,0,flags,biabl->FLAG);
    var1        = NULL;
    var2        = NULL;
    beg_addBerins(berins,obj);
    beg_addBerins(berinx,obj);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatRin()                                       */
/*                                                                          */
/* only eat special berin which don't drive anything                        */
/*                                                                          */
/****************************************************************************/
static void beg_eatRin(berin_list **rin)
{
  berin_list    *rinx;
  char          *name;
  
  for (rinx = *rin; rinx; rinx = rinx->NEXT)
  {
    beg_get_vectname(rinx->NAME,&name,NULL,NULL,BEG_SEARCH_ABL);
    if (!(rinx->OUT_REF || rinx->OUT_VEC || rinx->BUS_REF || rinx->BUS_VEC ||
          rinx->AUX_REF || rinx->AUX_VEC || rinx->REG_REF || rinx->REG_VEC ||
          rinx->BUX_REF || rinx->BUX_VEC))
      beg_addBerinsInt(rinx->NAME,NULL);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatMsg()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatMsg(bemsg_list *msg)
{
  if (msg)
  {
    if (msg->NEXT)
      beg_eatMsg(msg->NEXT);
    else
      msg->NEXT = BEG_WORK_BEFIG->BEMSG;
    BEG_WORK_BEFIG->BEMSG = msg;
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatOut()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatOut(beout_list *out)
{
  if (out)
  {
    chain_list  *berins = NULL;
    BEG_OBJ      obj;
    char        *opname;
    char        *name;
    long         index;

    beg_eatOut(out->NEXT);
    berins  = beg_IsAllDeclared(out->ABL,NULL);
    name    = out->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name  = beg_mkName(opname,index,index);
    obj     = beg_defSig(name,out->ABL,out->TIME,0,0,out->FLAGS);
    beg_addBerins(berins,obj);
    mbkfree(out);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatAux()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatAux(beaux_list *aux)
{
#if 0
  if (aux)
  {
    chain_list  *berins = NULL;
    BEG_OBJ      obj;
    char        *opname;
    char        *name;
    int          index;

    beg_eatAux(aux->NEXT);
    berins  = beg_IsAllDeclared(aux->ABL,NULL);
    name    = aux->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name = beg_mkName(opname,index,index);
    obj     = beg_defSig(name,aux->ABL,aux->TIME);
    beg_addBerins(berins,obj);
    mbkfree(aux);
  }
#else
  beaux_list    *auxx, *next;
  chain_list    *berins = NULL;
  BEG_OBJ        obj;
  char          *opname;
  char          *name;
  long           index;
  int            i;

  i = 0;
  auxx      = (beaux_list*)reverse((chain_list *)aux);
  for (; auxx; auxx = next)
  {
    next=auxx->NEXT;
    i ++;
    berins  = beg_IsAllDeclared(auxx->ABL,NULL);
    name    = auxx->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name  = beg_mkName(opname,index,index);
    obj     = beg_defSig(name,auxx->ABL,auxx->TIME,auxx->TIMER,auxx->TIMEF,auxx->FLAGS);
    beg_addBerins(berins,obj);
    mbkfree(auxx);
  }
#endif
}

/*}}}************************************************************************/
/*{{{                    beg_eatBus()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatBus(bebus_list *bus)
{
  if (bus)
  {
    char    *opname;
    char    *name;
    long     index;

    beg_eatBus(bus->NEXT);
    name     = bus->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name   = beg_mkName(opname,index,index);
    beg_eatBiabl(beg_defBus,name,bus->BIABL,bus->FLAGS);
    mbkfree(bus);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatBux()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatBux(bebux_list *bux)
{
  if (bux)
  {
    char    *opname;
    char    *name;
    long    index;

    beg_eatBux(bux->NEXT);
    name     = bux->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name   = beg_mkName(opname,index,index);
    beg_eatBiabl(beg_defBus,name,bux->BIABL,bux->FLAGS);
    mbkfree(bux);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatReg()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatReg(bereg_list *reg)
{
  if (reg)
  {
    char    *opname;
    char    *name;
    long     index;

    beg_eatReg(reg->NEXT);
    name     = reg->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name   = beg_mkName(opname,index,index);
    beg_eatBiabl(beg_def_bereg,name,reg->BIABL,reg->FLAGS);
    mbkfree(reg);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatVectOut()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectOut(bevectout_list *vout)
{
  if (vout)
  {
    chain_list  *berins = NULL;
    BEG_OBJ      obj;
    char        *vname;

    vname   = beg_mkName(vout->NAME,vout->LEFT,vout->RIGHT);
    berins  = beg_IsAllDeclared(vout->ABL,NULL);
    beg_eatVectOut(vout->NEXT);
    obj     = beg_defSig(vname,vout->ABL,vout->TIME,0,0,vout->FLAGS);
    beg_addBerins(berins,obj);
    mbkfree(vout);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatVectAux()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectAux(bevectaux_list *vaux)
{
  if (vaux)
  {
    chain_list  *berins = NULL;
    BEG_OBJ      obj;
    char        *vname;

    vname   = beg_mkName(vaux->NAME,vaux->LEFT,vaux->RIGHT);
    berins  = beg_IsAllDeclared(vaux->ABL,NULL);
    beg_eatVectAux(vaux->NEXT);
    obj     = beg_defSig(vname,vaux->ABL,vaux->TIME,0,0,vaux->FLAGS);
    beg_addBerins(berins,obj);
    mbkfree(vaux);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatVectBus()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectBus(bevectbus_list *vbus)
{
  if (vbus)
  {
    beg_eatVectBus(vbus->NEXT);
    beg_eatVectBiabl(beg_defBus,vbus->NAME,vbus->VECTBIABL,vbus->FLAGS);
    mbkfree(vbus);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatVectBux()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectBux(bevectbux_list *vbux)
{
  if (vbux)
  {
    beg_eatVectBux(vbux->NEXT);
    beg_eatVectBiabl(beg_defBus,vbux->NAME,vbux->VECTBIABL,vbux->FLAGS);
    mbkfree(vbux);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatVectReg()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectReg(bevectreg_list *vreg)
{
  if (vreg)
  {
    beg_eatVectReg(vreg->NEXT);
    beg_eatVectBiabl(beg_def_bereg,vreg->NAME,vreg->VECTBIABL,vreg->FLAGS);
    mbkfree(vreg);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatVectPor()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatVectPor(bevectpor_list *vpor)
{
  if (vpor)
  {
    char        *vname;

    beg_eatVectPor(vpor->NEXT);
    vpor->NEXT  = NULL;
    vname       = beg_mkName(vpor->NAME,vpor->LEFT,vpor->RIGHT);
    beg_def_por(vname,vpor->DIRECTION);
    beh_frebevectpor(vpor);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eatPor()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_eatPor(bepor_list *por)
{
  if (por)
  {
    char        *opname, *name;
    long         index;

    beg_eatPor(por->NEXT);
    por->NEXT   = NULL;
    name        = por->NAME;
    beg_get_vectname(name,&opname,&index,&index,BEG_SEARCH_ABL);
    if (index != -1)
      name      = beg_mkName(opname,index,index);
    beg_def_por(name,por->DIRECTION);
    beh_frebepor(por);
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eat_por()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_eat_por(befig_list *eaten)
{
  beg_eatPor    (eaten->BEPOR);
  beg_eatVectPor(eaten->BEVECTPOR);
  eaten->BEPOR      = NULL;
  eaten->BEVECTPOR  = NULL;
}

/*}}}************************************************************************/
/*{{{                    eatRenamed()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void eatRenamed(befig_list *eaten)
{
  ptype_list    *ptx, *list;

  if (eaten->USER)
  {
    list            = getptype(eaten->USER,BEG_RENAME_LIST);
    if (list)
    {
      for (ptx = list->DATA; ptx; ptx = ptx->NEXT)
        beg_def_sig((char*)ptx->TYPE,ptx->DATA,0,0,0,0);
      freeptype(list->DATA);
      eaten->USER   = delptype(eaten->USER,BEG_RENAME_LIST);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    beg_eat_figure()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_eat_figure(befig_list *eaten)
{
  // traitement des out
  beg_eatOut    (eaten->BEOUT);
  beg_eatVectOut(eaten->BEVECTOUT);
  eaten->BEOUT     = NULL;
  eaten->BEVECTOUT = NULL;
  // traitement des aux
  beg_eatAux    (eaten->BEAUX);
  beg_eatVectAux(eaten->BEVECTAUX);
  eaten->BEAUX     = NULL;
  eaten->BEVECTAUX = NULL;
  // traitement des reg
  beg_eatReg    (eaten->BEREG);
  beg_eatVectReg(eaten->BEVECTREG);
  eaten->BEREG     = NULL;
  eaten->BEVECTREG = NULL;
  //  traitement des assertions
  beg_eatMsg    (eaten->BEMSG);
  eaten->BEMSG     = NULL;
  // traitement des bux
  beg_eatBux    (eaten->BEBUX);
  beg_eatVectBux(eaten->BEVECTBUX);
  eaten->BEBUX     = NULL;
  eaten->BEVECTBUX = NULL;
  // traitement des bus
  beg_eatBus    (eaten->BEBUS);
  beg_eatVectBus(eaten->BEVECTBUS);
  eaten->BEBUS     = NULL;
  eaten->BEVECTBUS = NULL;

  // traitement des rin
  beg_eatRin    (&(eaten->BERIN));

  // traitement des renamed
  eatRenamed    (eaten);
  
  beg_delBefig(eaten);
}

/*}}}************************************************************************/
/*{{{                    beg_eatFigure()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*static void beg_eatFigure(befig_list *eater,befig_list *eaten)
  {
  bepor_list        *por;
  bevectpor_list    *vpor;
//  beout_list        *out;
//  bevectout_list    *vout;
//  beaux_list        *aux;
//  bevectaux_list    *vaux;
//  bereg_list        *reg;
//  bevectreg_list    *vreg;
bebux_list        *bux;
bevectbux_list    *vbux;
bebus_list        *bus;
bevectbus_list    *vbus;
//  berin_list        *rin;
char                *opname;
//  BEG_OBJ             obj;
//  void              *next;
//biabl_list          *biabl;
//vectbiabl_list      *vbiabl;

if (eater != BEG_WORK_BEFIG)
{
printf ("******************************\n\n\n");
printf ("pas actualisee beg_eatFigure()\n\n\n");
printf ("******************************\n");
return;
beg_saveEnv(BEG_HEAD_OBJ);

for (por = eater->BEPOR; por; por = por->NEXT)
beg_newPor(por->NAME,BEG_OBJ_NEW(por,BEG_TYPE_SINGPOR));
for (vpor = eater->BEVECTPOR; por; por = por->NEXT)
{
beg_get_vectname(vpor->NAME,&opname,NULL,NULL,BEG_SEARCH_NAME);
beg_newPor(vpor->NAME,BEG_OBJ_NEW(vpor,BEG_TYPE_VECTPOR));
}
for (bus = eater->BEBUS; bus; bus = bus->NEXT)
beg_newSig(bus->NAME,BEG_OBJ_NEW(bus,BEG_TYPE_SINGBUS));
for (vbus = eater->BEVECTBUS; bus; bus = bus->NEXT)
{
beg_get_vectname(vbus->NAME,&opname,NULL,NULL,BEG_SEARCH_NAME);
beg_newSig(vbus->NAME,BEG_OBJ_NEW(vbus,BEG_TYPE_VECTBUS));
}
for (bux = eater->BEBUX; bux; bux = bux->NEXT)
beg_newSig(bux->NAME,BEG_OBJ_NEW(bux,BEG_TYPE_SINGBUX));
for (vbux = eater->BEVECTBUX; bux; bux = bux->NEXT)
{
beg_get_vectname(vbux->NAME,&opname,NULL,NULL,BEG_SEARCH_NAME);
beg_newSig(vbux->NAME,BEG_OBJ_NEW(vbux,BEG_TYPE_VECTBUX));
}
}

beg_eat_figure(eaten);

if (eater != BEG_WORK_BEFIG)
beg_restoreEnv(BEG_HEAD_OBJ);
}*/

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Structure conversion                               */
/****************************************************************************/
///*{{{                    beg_convertOutToAux()                              */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static beaux_list *beg_convertOutToAux(beaux_list *next,beout_list *out)
//{
//  beaux_list *aux;
//
//  aux = beh_addbeaux(next,out->NAME,NULL,NULL);
//  aux->ABL  = out->ABL;
//  aux->NODE = out->NODE;
//  aux->TIME = out->TIME;
//
//  out->ABL  = NULL;
//
//  return aux;
//}
//
///*}}}************************************************************************/
///*{{{                    beg_convertVectOutToAux()                          */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static bevectaux_list *beg_convertVectOutToAux(bevectaux_list *next,
//                                        bevectout_list *vout)
//{
//  bevectaux_list *vaux;
//
//  vaux = beh_addbevectaux(next,vout->NAME,0,0);
//  vaux->ABL  = vout->ABL;
//  vaux->LEFT = vout->LEFT;
//  vaux->RIGHT= vout->RIGHT;
//  vaux->TIME = vout->TIME;
//
//  vout->ABL  = NULL;
//
//  return vaux;
//}
//
///*}}}************************************************************************/
///*{{{                    beg_convertBusToBux()                              */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static bebux_list *beg_convertBusToBux(bebux_list *next,bebus_list *bus)
//{
//  bebux_list *bux;
//
//  bux = beh_addbebux(next,bus->NAME,NULL,NULL);
//  bux->BIABL  = bus->BIABL;
//  bux->BINODE = bus->BINODE;
//
//  bus->BIABL  = NULL;
//
//  return bux;
//}
//
///*}}}************************************************************************/
///*{{{                    beg_convertVectBusToBux()                          */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static bevectbux_list *beg_convertVectBusToBux(bevectbux_list *next,
//                                        bevectbus_list *vbus)
//{
//  bevectbux_list *vbux;
//
//  vbux = beh_addbevectbux(next,vbus->NAME,0,0);
//  vbux->VECTBIABL = vbus->VECTBIABL;
//  vbux->LEFT      = vbus->LEFT;
//  vbux->RIGHT     = vbus->RIGHT;
//
//  vbus->VECTBIABL = NULL;
//
//  return vbux;
//}
//
///*}}}************************************************************************/
/*{{{                    beg_convertAuxInOut()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void beg_convertAuxInOut(befig_list *befig,void *sig,long mode)
{
  beaux_list        *aux;
  bevectaux_list    *vectaux;
  char              *errmsg;

  switch (mode)
  {
    case BEG_TYPE_SINGAUX :
         aux = sig;
         befig->BEOUT = beh_addbeout(befig->BEOUT,aux->NAME,aux->ABL,NULL,aux->FLAGS);
         befig->BEOUT->TIME = aux->TIME;
         aux->ABL = NULL;
         befig->BEAUX = beh_delbeaux(befig->BEAUX,aux,'Y');
         break;
    case BEG_TYPE_VECTAUX :
         vectaux = sig;
         befig->BEVECTOUT = beh_addbevectout(befig->BEVECTOUT, vectaux->NAME,
                                             vectaux->ABL, vectaux->LEFT,
                                             vectaux->RIGHT, vectaux->FLAGS);
         vectaux->ABL = NULL;
         befig->BEVECTOUT->TIME = vectaux->TIME;
         befig->BEVECTAUX = beh_delbevectaux(befig->BEVECTAUX,vectaux,'Y');
         break;
    default :
         beg_error(BEG_ERR_IN_TO_OUT,beg_getLongName(sig));
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Test function                                      */
/****************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_testCalcRange()
{
  loop_list     *loop   = NULL;
  loop_list     *loopx;
  int            i;
  char           buf[5];
  long           left, right;
  eqt_ctx       *ctx;

  for (i = 0; i < 4; i ++)
  {
    sprintf(buf,"%c",'i'+ i);
    loopx           = mbkalloc(sizeof(struct beh_loop));
    loopx->NEXT     = loop;
    loopx->RIGHT    = 0;
    loopx->LEFT     = i+3;
    loopx->VARNAME  = namealloc(buf);
    loop            = loopx;
    
    //printf("%s(%d,%d)\n",loop->VARNAME,loop->LEFT,loop->RIGHT);
  }
  ctx = eqt_init(2);
  beg_calcRange(ctx,namealloc("2+i+6*j+k+l*4"),loop,&left,&right);
  sprintf(buf,"%ld,%ld",left,right);
  eqt_term(ctx);

  if (!strcmp("58,2",buf))
    printf("OK\n");
  else
    printf("Test FAILED\n");
}

/*}}}************************************************************************/
/*{{{                   beg_testBerin()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_testBerin()
{
  berin_list    *berin  = BEG_WORK_BEFIG->BERIN;

  for (; berin; berin = berin->NEXT)
  {
    chain_list  *chainx;

    printf("berin %s:",berin->NAME);
    for (chainx = berin->OUT_REF; chainx; chainx = chainx->NEXT)
      printf(" %s",((beout_list *)chainx->DATA)->NAME);
    for (chainx = berin->OUT_VEC; chainx; chainx = chainx->NEXT)
      printf(" %s",((beout_list *)chainx->DATA)->NAME);

    for (chainx = berin->BUS_REF; chainx; chainx = chainx->NEXT)
      printf(" %s",((bebus_list *)chainx->DATA)->NAME);
    for (chainx = berin->BUS_VEC; chainx; chainx = chainx->NEXT)
      printf(" %s",((bebus_list *)chainx->DATA)->NAME);

    for (chainx = berin->BUX_REF; chainx; chainx = chainx->NEXT)
      printf(" %s",((bebux_list *)chainx->DATA)->NAME);
    for (chainx = berin->BUX_VEC; chainx; chainx = chainx->NEXT)
      printf(" %s",((bebux_list *)chainx->DATA)->NAME);

    for (chainx = berin->AUX_REF; chainx; chainx = chainx->NEXT)
      printf(" %s",((beaux_list *)chainx->DATA)->NAME);
    for (chainx = berin->AUX_VEC; chainx; chainx = chainx->NEXT)
      printf(" %s",((beaux_list *)chainx->DATA)->NAME);

    for (chainx = berin->REG_REF; chainx; chainx = chainx->NEXT)
      printf(" %s",((bereg_list *)chainx->DATA)->NAME);
    for (chainx = berin->REG_VEC; chainx; chainx = chainx->NEXT)
      printf(" %s",((bereg_list *)chainx->DATA)->NAME);
    printf("\n");
  }
}

/*}}}************************************************************************/
/*{{{                    beg_testEat()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beg_testEat(befig_list *befig)
{
  beg_eat_figure(befig);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
