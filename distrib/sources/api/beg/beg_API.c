#include <string.h>

#include AVT_H
#include MUT_H
#include MLO_H
#include BEH_H
#include BVL_H
#include BHL_H
#include BEG_H
#include BEF_H
#include GEN_H
#include LOG_H

#define API_USE_REAL_TYPES
#include "gen_API.h"
#include "beg_API.h"
#define AVTWIG_AVOID_CONFLICT
#include "mbk_API.h"


static ht                       *BEG_MODELHT        = NULL;

static char                     *BEGAPI_PREFIX      = NULL;
static int                       BEGAPI_INDEX       = -1;
static int                       BEG_TRACE          = 0;
static befig_list               *BEGAPI_BEFIG       = NULL;
static ht                       *BEGAPI_DONEHT      = NULL;

static int                       BEGAPI_USEFILES    = 0;

static int                       BEGAPI_DRIVE_CORRESP;
static int                       BEGAPI_DRIVE_ALL_BEH;
static ht                       *BEGAPI_MODELTABLE  = NULL;

static int   DEACTIVATE_BEG = 0, BEG_QUICK_MODE=1;
static char *BEG_FORCED_NAME=NULL;
static chain_list *RECUR_STACK=NULL;
void         begRenameSignalsFromModel();
static void begAutoCreateCorrespOnBlackbox(befig_list *bf, lofig_list *lf);

#include "beg_API_correspondance.h"


static long getbiablflag(long val)
{
  long nval=0;
  if (val & 2) nval|=BEH_CND_WEAK; 
  if (val & 4) nval|=BEH_CND_STRONG; 
  if (val & 8) nval|=BEH_CND_PULL; 
  return nval;
}

static long getsigflag(long val)
{
  long nval=0;
  if (val & 1) nval|=BEH_FLAG_NORMAL; 
  return nval;
}

/**************************************************************************************/

void beg_API_AtLoad_Initialize() // commentaire pour desactiver l'ajout de token
{
  char    *str;

  if (V_BOOL_TAB[__API_DRIVE_CORRESP].VALUE)
  {
    mbv_useAdvancedNameAllocator();
    begInitCorrespondance();
    BEGAPI_DRIVE_CORRESP    = 1;
  }
  else
    BEGAPI_DRIVE_CORRESP    = 0;
  
  str                       = V_STR_TAB[__API_DRIVE_ALL_BEH].VALUE;
  if (str && !strcasecmp(str,"yes"))
    BEGAPI_DRIVE_ALL_BEH    = 1;
  else if (str && !strcasecmp(str,"onlymodel"))
  {
    BEGAPI_DRIVE_ALL_BEH    = 1;
    BEGAPI_MODELTABLE       = addht(50);
  }
  else
    BEGAPI_DRIVE_ALL_BEH    = 0;
}

/**************************************************************************************/

void
beg_API_Action_Initialize() // commentaire pour desactiver l'ajout de token
{
  BEG_FORCED_NAME           = NULL;
  DEACTIVATE_BEG = 0;
}


void beg_API_TopLevel(long list) // commentaire pour desactiver l'ajout de token
{
  int res;
  char buf[2048];

  strcpy(buf,"");

  res=check_instances_connections(GENIUS_GLOBAL_LOFIG, buf);
/*  if (res)
    {
      avt_error("begapi", 1, AVT_WAR,
                  "Functional errors could appear if using the beg_API\n"
                  "reason: %s\n",
                  buf
                  );      
    }*/
  DEACTIVATE_BEG=(long)RECUR_STACK->DATA; RECUR_STACK=delchain(RECUR_STACK, RECUR_STACK);
  BEG_FORCED_NAME = (char *)RECUR_STACK->DATA; RECUR_STACK=delchain(RECUR_STACK, RECUR_STACK);
}
/*
void
beg_API_Action_Terminate() // commentaire pour desactiver l'ajout de token
{

}
*/
/**************************************************************************************/

void
beg_API_Restart() // commentaire pour desactiver l'ajout de token
{
    chain_list *ptchain, *refchain;
    char       *varname;

    RECUR_STACK=addchain(RECUR_STACK, BEG_FORCED_NAME);
    RECUR_STACK=addchain(RECUR_STACK, (void *)(long)DEACTIVATE_BEG);

    if (BEG_MODELHT != NULL) {
        delht(BEG_MODELHT);
        BEG_MODELHT = NULL;
    }
#if 0
    if (BEGAPI_INTVARLIST != NULL) {
        for (ptchain = BEGAPI_INTVARLIST; ptchain; ptchain = ptchain->NEXT) {
            varname = (char *)ptchain->DATA;
            refchain = (chain_list *)gethtitem(BEGAPI_INTVARHT, varname);
            freechain(refchain);
        }
        freechain(BEGAPI_INTVARLIST);
        BEGAPI_INTVARLIST = NULL;
        delht(BEGAPI_INTVARHT);
        BEGAPI_INTVARHT = NULL;
    }
#endif
}

/**************************************************************************************/

static befig_list * begGetModel(char *name)
{
    befig_list *ptbefig = NULL;

    if (BEG_MODELHT != NULL) {
        ptbefig = (befig_list *)gethtitem(BEG_MODELHT, name);
        if (ptbefig == (befig_list *)EMPTYHT) ptbefig = NULL;
    }
    return ptbefig;
}

/**************************************************************************************/

static int BEG_CHECK_EXISTENCE(char *name)
{
  
  if (begGetModel(namealloc(name))!=NULL) 
    DEACTIVATE_BEG=1;
  else
    DEACTIVATE_BEG=0;
  return DEACTIVATE_BEG;
}

befig_list *begModelToInstance(char *instance, char *model)
{
  befig_list *ptinsbefig;

  ptinsbefig    = begGetModel(instance);
  if (ptinsbefig == NULL)
    {
      ptinsbefig    = begGetModel(model);
      if (ptinsbefig != NULL)
        {
          ptinsbefig    = beg_duplicate(ptinsbefig, instance);
        }
    }
  return ptinsbefig;
}

static char *_BegGetName()
{
  if (BEG_FORCED_NAME!=NULL) return BEG_FORCED_NAME;
  if (!BEG_QUICK_MODE)
    return gns_GetCurrentInstance();
  else
    return gns_GetCurrentModel();
}

/**************************************************************************************/

static void
begRefDriver(char *varname, biabl_list *ptdriver)
{
    chain_list *varchain;
    ptdriver->TIMEVAR=namealloc(varname);
#if 0
    if (BEGAPI_INTVARHT == NULL) BEGAPI_INTVARHT = addht(20);
    if ((varchain = (chain_list *)gethtitem(BEGAPI_INTVARHT, namealloc(varname))) != (void *)EMPTYHT) {
        varchain = addchain(varchain, ptdriver);
        sethtitem(BEGAPI_INTVARHT, namealloc(varname), (long)varchain);
    }
    else {
        addhtitem(BEGAPI_INTVARHT, namealloc(varname), (long)addchain(NULL, ptdriver));
        BEGAPI_INTVARLIST = addchain(BEGAPI_INTVARLIST, namealloc(varname));
    }
#endif
}

/**************************************************************************************/

static char *
begRadical(char *name)
{
    char buf[1024];
    char *pt;

    strcpy(buf, name);
    if ((pt = strchr(buf, ' ')) != NULL) *pt = 0;
    return namealloc(buf);
}

/**************************************************************************************/

static void
begTranslate(BEG_OBJ ptbegobj, BEG_OBJ ptportobj, int shift, char *name)
{
    vectbiabl_list *ptvecbiabl;
    biabl_list     *ptbiabl;
    loop_list      *ptloop;
    berin_list     *ptberin;

    if (ptbegobj && gethtitem(BEGAPI_DONEHT, ptbegobj) == EMPTYHT) {
        addhtitem(BEGAPI_DONEHT, ptbegobj, 0);
        ptvecbiabl = beg_getVectBiabl(ptbegobj);
        beg_newLeft(ptbegobj, beg_getLeft(ptbegobj)+shift);
        beg_newRight(ptbegobj, beg_getRight(ptbegobj)+shift);
        for (;ptvecbiabl; ptvecbiabl = ptvecbiabl->NEXT) {
            if (ptvecbiabl->LEFT >= 0) ptvecbiabl->LEFT += shift;
            if (ptvecbiabl->RIGHT >= 0) ptvecbiabl->RIGHT += shift;
            for (ptbiabl = ptvecbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                ptloop = beh_getloop(ptbiabl);
                for (;ptloop; ptloop = ptloop->NEXT) {
                    if (ptloop->INDEX != NULL) {
                        ptloop->INDEX = shiftExprStr(ptloop->INDEX, shift);
                    }
                }
            }
        }
    }
    if (ptportobj && gethtitem(BEGAPI_DONEHT, ptportobj) == EMPTYHT) {
        addhtitem(BEGAPI_DONEHT, ptportobj, 0);
        ptvecbiabl = beg_getVectBiabl(ptportobj);
        beg_newLeft(ptportobj, beg_getLeft(ptportobj)+shift);
        beg_newRight(ptportobj, beg_getRight(ptportobj)+shift);
        for (;ptvecbiabl; ptvecbiabl = ptvecbiabl->NEXT) {
            if (ptvecbiabl->LEFT >= 0) ptvecbiabl->LEFT += shift;
            if (ptvecbiabl->RIGHT >= 0) ptvecbiabl->RIGHT += shift;
            for (ptbiabl = ptvecbiabl->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                ptloop = beh_getloop(ptbiabl);
                for (;ptloop; ptloop = ptloop->NEXT) {
                    if (ptloop->INDEX != NULL) {
                        ptloop->INDEX = shiftExprStr(ptloop->INDEX, shift);
                    }
                }
            }
        }
    }
    ptberin = beg_getBerinByName(BEGAPI_BEFIG, name);
    if (ptberin && gethtitem(BEGAPI_DONEHT, ptberin) == EMPTYHT) {
        addhtitem(BEGAPI_DONEHT, ptberin, 0);
        ptberin->LEFT += shift;
        ptberin->RIGHT += shift;
    }
}

/**************************************************************************************/
static int mode=0;

void begSwitchMode()
{
  mode=(mode+1) & 1;
}

static char *
begNameWrap(char *name)
{
    char buf[1024];
    BEG_OBJ ptbegobj;
    BEG_OBJ ptportobj;
    int     shift;

    if (strncasecmp(name,"ext@",4)==0) return name;

    buf[0] = 0;
    if (strchr(name, ' ') == NULL) {
        ptbegobj = beg_getObjByName(BEGAPI_BEFIG, name, &ptportobj);
        if (((ptbegobj != NULL && beg_isVect(ptbegobj)) || (ptportobj != NULL && beg_isVect(ptportobj))) && BEGAPI_INDEX >= 0) {
            if (ptbegobj) shift = BEGAPI_INDEX * (abs(beg_getLeft(ptbegobj) - beg_getRight(ptbegobj)) + 1);
            else shift = BEGAPI_INDEX * (abs(beg_getLeft(ptportobj) - beg_getRight(ptportobj)) + 1);
            if (shift > 0) begTranslate(ptbegobj, ptportobj, shift, name);
            sprintf(buf, "%s.%s", BEGAPI_PREFIX, name);
            return namealloc(buf);
        }
        else {
            if (ptbegobj == NULL && ptportobj == NULL && name != BEGAPI_BEFIG->NAME) {
              avt_errmsg(BEG_API_ERRMSG, "001", AVT_WARNING, name, BEGAPI_BEFIG->NAME);
//            avt_error("begapi", 1, AVT_WAR, "name '%s' does not exist in figure '%s'\n", name, BEGAPI_BEFIG->NAME);
            }
            if (BEGAPI_INDEX >= 0) {
                sprintf(buf, "%s.%s %d", BEGAPI_PREFIX, name, BEGAPI_INDEX);
            }
            else {
                sprintf(buf, "%s.%s", BEGAPI_PREFIX, name);
            }
            return namealloc(buf);
        }
    }
    else {
        if (BEGAPI_INDEX >= 0) {
            sprintf(buf, "%s.%s", BEGAPI_PREFIX, name);
            ptbegobj = beg_getObjByName(BEGAPI_BEFIG, begRadical(name), &ptportobj);
            if (ptbegobj != NULL || ptportobj != NULL) {
                if (ptbegobj) shift = BEGAPI_INDEX * (abs(beg_getLeft(ptbegobj) - beg_getRight(ptbegobj)) + 1);
                else shift = BEGAPI_INDEX * (abs(beg_getLeft(ptportobj) - beg_getRight(ptportobj)) + 1);
                if (shift > 0) return renameVectAtom(buf, NULL, shift);
            }
            else  if (ptbegobj == NULL && ptportobj == NULL && name != BEGAPI_BEFIG->NAME) {
                avt_errmsg(BEG_API_ERRMSG, "001", AVT_WARNING, name, BEGAPI_BEFIG->NAME);
//                avt_error("begapi", 1, AVT_WAR, "name '%s' does not exist in figure '%s'\n", name, BEGAPI_BEFIG->NAME);
            }
            return namealloc(buf);
        }
        else {
            sprintf(buf, "%s.%s", BEGAPI_PREFIX, name);
            return namealloc(buf);
        }
    }
}

static inline int inrange(int num, int left, int right)
{
  if (left<right && num>=left && num<=right)
    return 1;
  else if (left>=right && num>=right && num<=left)
    return 1;
  return 0;
}

static void begAutoCreateCorrespOnBlackbox(befig_list *bf, lofig_list *lf)
{
  BEG_OBJ        ptbegobj;
  BEG_OBJ        ptportobj;
  losig_list    *ls;
  char          *name;
  ht            *corresp_ht;
  int            index;

  if (BEGAPI_DRIVE_CORRESP)
  {
    corresp_ht      = begGetCorrespHTCreate(gns_GetCurrentInstance()/*bf->NAME*/);
    
    for (ls = lf->LOSIG; ls != NULL; ls = ls->NEXT)
    {
      name          = gen_losigname(ls);
      if (strchr(name, ' ') == NULL) 
      {
        ptbegobj    = beg_getObjByName(bf, name, &ptportobj);
        if (ptbegobj != NULL)
          begAddCorrespName(corresp_ht,name, name);
      }
      else
      {
        index       = vectorindex(name);
        ptbegobj    = beg_getObjByName(bf,vectorradical(name),&ptportobj);
        if (ptbegobj != NULL &&
            inrange(index,beg_getLeft(ptbegobj),beg_getRight(ptbegobj)))
          begAddCorrespName(corresp_ht,name,name);
      }
    }
    begUpdateCorresp(corresp_ht);
  }
}

/**************************************************************************************/

befig_list *begLoadBefig(char *name)
{
  name      = NULL; // WARNING PREVENTION
  
  return NULL;
}

/**************************************************************************************/

void
begSaveModel(void)
{
    befig_list *ptbefig;
    char *name, *insname;

    name=_BegGetName();
   /* 
    if (BEGAPI_DRIVE_CORRESP)
      {
        insname=gns_GetCurrentInstance();
        if (name!=insname)
          {
            if ((ptbefig = begGetModel(insname))==NULL)
              if ((ptbefig = begGetModel(name))==NULL)
                ptbefig = beg_get_befig(BEG_NOVERIF);
            bvl_drivecorresp(insname, ptbefig);
          }
      }
    */
    if (DEACTIVATE_BEG) return;
    
    ptbefig = beg_get_befig(BEG_NOVERIF);
    if (ptbefig != NULL) savebefig(ptbefig, 0);
}

/**************************************************************************************/

static void _begKeepModel(int mode)
{
  befig_list    *ptbefig;
  lofig_list    *lf;

  if (BEGAPI_DRIVE_ALL_BEH)
  {
    if (!BEGAPI_MODELTABLE)
      begSaveModel();
    else if (gethtitem(BEGAPI_MODELTABLE,CUR_CORRESP_TABLE->GENIUS_FIGNAME)
             == EMPTYHT)
    {
      addhtitem(BEGAPI_MODELTABLE,CUR_CORRESP_TABLE->GENIUS_FIGNAME,0);
      begSaveModel();
    }
  }
  
  if (!DEACTIVATE_BEG)
  {
    ptbefig         = beg_get_befig(BEG_NOVERIF);

    if (BEG_MODELHT == NULL)
      BEG_MODELHT   = addht(20);
    addhtitem(BEG_MODELHT,ptbefig->NAME,(long)ptbefig);
  }
  
  if (mode==1 && CUR_CORRESP_TABLE->FLAGS & LOINS_IS_BLACKBOX)
  {
    lf          = gns_GetBlackboxNetlist(CUR_CORRESP_TABLE->GENIUS_FIGNAME);
    begAutoCreateCorrespOnBlackbox(ptbefig, lf);
  }

  if (gns_IsTopLevel())
    begRenameSignalsFromModel();

}

void
begKeepModel(void)
{
  _begKeepModel(1);
}

/**************************************************************************************/

/**************************************************************************************/

void
begDestroyModel(void)
{
    if (DEACTIVATE_BEG) return;
    beg_freeBefig();
}

/**************************************************************************************/

char *
begVectorize(char *radical, int index)
{
    char temp[1024];

    sprintf(temp, "%s(%d)", radical, index);
    return namealloc(temp);
}

/**************************************************************************************/

char *
begVarVectorize(char *radical, char *var)
{
    char temp[1024];

    sprintf(temp, "%s(%s)", radical, var);
    return namealloc(temp);
}

/**************************************************************************************/

char *
begVectorRange(char *radical, int left, int right)
{
    char temp[1024];

    sprintf(temp, "%s(%d:%d)", radical, left, right);
    return namealloc(temp);
}

/**************************************************************************************/

void
begCreateModel(char *name)
{  
  if (BEG_CHECK_EXISTENCE(name)) return;
  BEG_FORCED_NAME=namealloc(name);
  beg_def_befig(name);
}

/**************************************************************************************/

void
begCreatePort(char *name, char direction)
{
    if (DEACTIVATE_BEG) return;
    beg_def_por(name, direction);
}

/**************************************************************************************/

void
begCreateModelFromConnectors(char *name, chain_list *ptloconlist)
{
  befig_list    *ptbefig;
  locon_list    *ptlocon;
  chain_list    *ptchain;
  chain_list    *ptprevchain;
  char          *basename;
  char           temp[1024];
  char           direction;
  int            left, right, index;
  ht            *corresp_ht;

  if (BEG_CHECK_EXISTENCE(name) && BEGAPI_DRIVE_CORRESP==0) return;

  if (BEG_TRACE)
    fprintf (stdout, "\nbegCreateModelFromConnectors ('%s')\n", name);

  if (!DEACTIVATE_BEG)
    {
      if ((ptbefig = beg_get_befigByName(name))!=NULL)
         beg_delBefig(ptbefig);
      beg_def_befig(name);
      ptbefig   = beg_get_befig(BEG_NOVERIF);
    }

  if (BEGAPI_DRIVE_CORRESP)
    corresp_ht  = begGetCorrespHTCreate(gns_GetCurrentInstance()/*ptbefig->NAME*/);
  else
    corresp_ht  = NULL;

  for (ptchain = ptloconlist; ptchain; ptchain = ptchain->NEXT) {
    ptlocon = (locon_list *)ptchain->DATA;
    if ((index = vectorindex(ptlocon->NAME)) == -1) {
      gns_GetModelSignalRange(ptlocon->NAME, &left, &right);
      if (left != -1)
      {
        sprintf(temp,"%s(%d:%d)",ptlocon->NAME,left,right);
        begTraceAndCorresp(corresp_ht,ptlocon->NAME,left,right,
                           BEG_TRACE,BEGAPI_DRIVE_CORRESP);
      }
      else
      {
        sprintf(temp, "%s", ptlocon->NAME);
        begTraceAndCorresp(corresp_ht,ptlocon->NAME,-1,-1,
                           BEG_TRACE,BEGAPI_DRIVE_CORRESP);
      }
    }
    else
    {
      ptprevchain   = ptchain;
      basename      = vectorradical(ptlocon->NAME);
      left          = index;
      
      for (; ptchain; ptchain = ptchain->NEXT)
      {
        ptlocon     = (locon_list *)ptchain->DATA;
        if (vectorradical(ptlocon->NAME) != basename)
          break;
        right       = vectorindex(ptlocon->NAME);
        ptprevchain = ptchain;
        begTraceAndCorresp(corresp_ht,vectorradical(ptlocon->NAME),
                           right,right,
                           BEG_TRACE,BEGAPI_DRIVE_CORRESP);
      }
      ptchain       = ptprevchain;
      sprintf(temp, "%s(%d:%d)", basename, left, right);
    }
    
    if (ptlocon->DIRECTION == 'D' || ptlocon->DIRECTION == 'S')
      direction     = 'I';
    else
      direction     = ptlocon->DIRECTION;

    if (!DEACTIVATE_BEG)
      beg_def_por(namealloc(temp), direction);
  }
  if (BEGAPI_DRIVE_CORRESP) {
    for (ptchain = ptloconlist; ptchain; ptchain = ptchain->NEXT) {
      ptlocon = (locon_list *)ptchain->DATA;
      begAddCorresp(gns_GetCurrentInstance(), ptlocon->NAME);
    }
  }
}

/**************************************************************************************/

void begCreateModelInterface(char *name)
{
    chain_list *connectors;    

    if (BEG_CHECK_EXISTENCE(name)==0)
      {
        begCreateModel(name);
        name=_BegGetName();
        connectors = gns_GetModelConnectorList();
        begCreateModelFromConnectors(name, connectors);
        freechain(connectors);
      }
}

void begCreateInterface()
{
    chain_list *connectors;    
    char *name;

    name=_BegGetName();
    connectors = gns_GetModelConnectorList();
    begCreateModelFromConnectors(name, connectors);
    freechain(connectors);
}

/**************************************************************************************/

void
begAddInstanceModel(loins_list *instance, ht *corTbl)
{
  befig_list     *befig;
  locon_list     *ptlocon;
  bepor_list     *ptbepor;
  berin_list     *ptberin;
  bevectpor_list *ptbevectpor;
  lofig_list     *pthierlofig;
  ht             *berinht;
  ht             *loconht;
  int             left, right, shift;
  char           *insname;
  char           *newname;
  char           *leftsigname;
  chain_list     *before_change, *after_change;
  ptype_list     *ptype;
  ht             *corTblIns = NULL;
  CorrespondingInstance *ci;

  if (DEACTIVATE_BEG && BEGAPI_DRIVE_CORRESP==0) return;

  ci=gns_GetCorrespondingInstance(instance->INSNAME);
  insname       = gns_GetCorrespondingInstanceName(ci);
  befig    = begModelToInstance(insname, ci->CRT->GENIUS_FIGNAME);
  if (befig == NULL)
    {
      fprintf(stderr,"%s: BegApi: Model for instance '%s' does not exist\n", gen_info(), insname);
      return;
    }

  corTblIns       = begGetCorrespHT(befig->NAME);
/*
  if ((ptype = getptype (befig->USER, BEG_CORRESP_PTYPE)))
    corTblIns       = (ht*)((chain_list*)ptype->DATA)->DATA;
*/
  pthierlofig   = CUR_HIER_LOFIG; //gns_GetNetlist();
  if (befig == NULL && BEGAPI_USEFILES)
  {
    befig  = begLoadBefig(instance->FIGNAME);
    if (befig != NULL)
      befig->NAME = insname;
  }
  BEGAPI_BEFIG  = befig;
  BEGAPI_INDEX  = gns_GetInstanceLoopIndex(instance, &BEGAPI_PREFIX);
  if (BEGAPI_INDEX != -1)
  {
    gns_GetInstanceLoopRange(pthierlofig, instance, &left, &right);
    if (left != -1 && left == right)
      BEGAPI_INDEX = -1;
  }
  if (mode==1)
  {
    BEGAPI_PREFIX = instance->INSNAME;
  }

  berinht       = addht(20);
  loconht       = addht(20);
  
  if (befig)
  {
    for (ptbepor = befig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT)
    {
      ptlocon   = gns_GetInstanceConnector(instance, ptbepor->NAME);
      addhtitem(loconht, ptbepor, (long)ptlocon);
    }
    for (ptbevectpor = befig->BEVECTPOR; ptbevectpor; ptbevectpor = ptbevectpor->NEXT)
    {
      ptlocon   = gns_GetInstanceConnector(instance, gns_Vectorize(ptbevectpor->NAME, ptbevectpor->LEFT));
      addhtitem(loconht, ptbevectpor, (long)ptlocon);
    }

    /* prefix internal nodes with instance name and rename ports according to rule port map */

    if (BEGAPI_DRIVE_CORRESP) /* trace changes */
      before_change     = begGetNames(befig);
    else
      before_change     = NULL;

    BEGAPI_DONEHT   = addht(40);
    beh_namewrap(befig,begNameWrap);
    delht(BEGAPI_DONEHT);
    BEGAPI_DONEHT   = NULL;

    for (ptberin = befig->BERIN; ptberin; ptberin = ptberin->NEXT)
      addhtitem(berinht,ptberin->NAME,(long)ptberin);
    
    for (ptbepor = befig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT)
    {
      if (!(ptlocon = (locon_list *)gethtitem(loconht, ptbepor)))
      {
        char         buf[1024];
        
        sscanf(ptbepor->NAME,"%*[^.].%s",buf);
        newname     = namealloc(buf);
      }
      else
        newname     = (char *)ptlocon->SIG->NAMECHAIN->DATA;
      ptberin       = (berin_list *)gethtitem(berinht, ptbepor->NAME);
      if (ptberin == (berin_list *)EMPTYHT)
        ptberin     = NULL;
      beh_renameport(befig,ptbepor,ptberin,newname);
    }
    
    for (ptbevectpor = befig->BEVECTPOR; ptbevectpor; ptbevectpor = ptbevectpor->NEXT)
    {
      left          = ptbevectpor->LEFT;
      ptlocon       = (locon_list *)gethtitem(loconht, ptbevectpor);
      leftsigname   = (char *)ptlocon->SIG->NAMECHAIN->DATA;
      shift         = vectorindex(leftsigname) - left;
      ptberin       = (berin_list *)gethtitem(berinht, ptbevectpor->NAME);
      if (ptberin == (berin_list *)EMPTYHT)
        ptberin     = NULL;
      newname       = vectorradical(leftsigname);
      beh_renamevectport(befig, ptbevectpor, ptberin, newname, shift);
    }

    if (BEGAPI_DRIVE_CORRESP)
    {
      after_change  = begGetNames(befig);
      if (corTblIns!=NULL)
        begUpdateChanges(corTblIns,corTbl,before_change,after_change);
      else
        avt_errmsg(BEG_API_ERRMSG, "002", AVT_WARNING, insname);
//        avt_error("begapi", 6, AVT_WAR,"could not find correspondance for instance '%s'\n",insname);
      before_change = NULL;
      after_change  = NULL;
    }
    else
      after_change  = NULL;

    if (corTblIns)
      {
        mbv_freeht(corTblIns); //delht(corTblIns);
        begGetCorrespHTUnlink(befig->NAME);
      }
/*    if (befig->USER)
      befig->USER  = delptype(befig->USER,BEG_CORRESP_PTYPE);
  */  
    freechain(before_change);
    freechain(after_change);

    /* merge into figure under construction */
    if (!DEACTIVATE_BEG)
      beg_eat_figure(befig);
    else
      beh_frebefig(befig);
  }
  delht(berinht);
  delht(loconht);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void addInstance(loins_list *loins, ht *corht, ht *fight, ht *sight)
{
  // in this case fight and sight are unused and should be NULL so
  fight             = NULL;
  sight             = NULL;
  begAddInstanceModel(loins,corht);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begAddAllInstanceModels(int compact)
{
  lofig_list    *hierlofig;
  befig_list    *befig;
  int mode=1;
  char *name, *insname;

  if (DEACTIVATE_BEG && BEGAPI_DRIVE_CORRESP==0) return;

  if (DEACTIVATE_BEG) 
    mode|=DISABLE_CREATE_BEH;

  if (BEGAPI_DRIVE_CORRESP==0)
    mode|=DISABLE_CREATE_COR;
      
  if (!compact)
    mode|=DISABLE_COMPACT;

  name=_BegGetName();
  if ((befig = begGetModel(name))==NULL)
    befig = beg_get_befig(BEG_NOVERIF);

  hierlofig         = CUR_HIER_LOFIG; //gns_GetNetlist();
//  befig             = beg_get_befig(BEG_NOVERIF);
  beg_addAllInstances(gns_GetCurrentInstance(), befig,hierlofig->LOINS,NULL,NULL,mode,addInstance);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *createName(char *name, int i, char *prefix)
{
  char       buf[1024];
  
  if (strncasecmp(name,"ext@",4)==0) return namealloc(name);
  if (prefix)
    if (i > -1)
      sprintf(buf,"%s.%s %d",prefix,name,i);
    else
      sprintf(buf,"%s.%s",prefix,name);
  else if (i > -1)
    sprintf(buf,"%s %d",name,i);
  else
    sprintf(buf,"%s",name);

  return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void putCorList4Yagle(befig_list *befig, char *prefix)
{
  locon_list    *locon;
  losig_list    *sig;
  char          *unprefixed_name, *model_name, *circuit_name;
  int            left, right, inc, i;
  chain_list    *loconlist, *chainx;
  ptype_list    *renamed;
  
  loconlist             = gns_GetModelConnectorList();
  renamed               = NULL;
  for (chainx = loconlist; chainx; chainx = chainx->NEXT)
  {
    locon               = (locon_list *)chainx->DATA;
    gns_GetModelSignalRange(locon->NAME,&left,&right);
    if (left != -1)
    {
      inc               = (right >= left) ? 1 : -1;
      for (i = left; i != right+inc; i += inc)
      {
        unprefixed_name = createName(locon->NAME,i,NULL);
        
        sig             = gns_GetCorrespondingSignal(unprefixed_name);
        if (prefix != NULL)
          model_name    = createName(locon->NAME,i,prefix);
        else
          model_name    = unprefixed_name;
        
        circuit_name    = sig->NAMECHAIN->DATA;
        renamed         = addptype(renamed,(long)model_name,circuit_name);
      }
    }
    else
    {
      sig               = gns_GetCorrespondingSignal(locon->NAME);
      if (prefix != NULL)
        model_name      = createName(locon->NAME,-1,prefix);
      else
        model_name      = locon->NAME;
      
      circuit_name      = sig->NAMECHAIN->DATA;
      renamed           = addptype(renamed,(long)model_name,circuit_name);
    }
  }
  befig->USER           = addptype(befig->USER,BEG_RENAME_LIST,renamed);
  
  freechain(loconlist);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begRenameSignalsFromModel(void)
{
  chain_list    *before_change, *after_change;
  befig_list    *ptbefig;
  ptype_list    *ptype;
  char          *prefix = NULL;
  ht            *corresp_ht, *new_corresp_ht;

  ptbefig   = begModelToInstance(gns_GetCurrentInstance(),
                                 gns_GetCurrentModel());
  if (ptbefig == NULL)
    return;
 
  addhtitem(BEG_MODELHT, ptbefig->NAME, (long)ptbefig);

  if (BEGAPI_DRIVE_CORRESP)
  {
    corresp_ht=begGetCorrespHT(ptbefig->NAME);
    new_corresp_ht    = addht(BEGAPI_HT_BASE);
    begGetCorrespHTRelink(ptbefig->NAME, new_corresp_ht);
/*    if ((ptype = getptype(ptbefig->USER,BEG_CORRESP_PTYPE)))
    {
      corresp_ht        = (ht*)((chain_list*)ptype->DATA)->DATA;
      new_corresp_ht    = addht(BEGAPI_HT_BASE);
      ((chain_list*)ptype->DATA)->DATA = new_corresp_ht;
    }
*/
    before_change       = begGetNames(ptbefig);
  }

  if (ptbefig != NULL)
  {
    prefix              = ptbefig->NAME;
    BEGAPI_PREFIX       = prefix;
    BEGAPI_INDEX        = -1;
    BEGAPI_BEFIG        = ptbefig;
    beh_namewrap(ptbefig,begNameWrap);
    ptbefig->NAME       = prefix;
  }

  if (BEGAPI_DRIVE_CORRESP)
  {
    after_change        = begGetNames(ptbefig);
    begUpdateChanges(corresp_ht,new_corresp_ht,before_change,after_change);
    before_change       = NULL;
    after_change        = NULL;
    mbv_freeht(corresp_ht); //delht(corresp_ht);
  }
  
  putCorList4Yagle(ptbefig,prefix);
}

/**************************************************************************************/

#define tops(x) (int)(x>=0.9?x+0.5:x*1e12+0.5)
void
begAssign_sub(char *name, char *expr, double delay, double delayr, double delayf, char *delayvar, int flags)
{
    delayvar    = NULL; // WARNING PREVENTION
  
    if (DEACTIVATE_BEG && BEGAPI_DRIVE_CORRESP==0) return;

    if (BEG_TRACE)
        fprintf(stdout,"    signal %s <%s>\n",name,
                gns_GetSignalName(gns_GetCorrespondingSignal(name)));

    if (BEGAPI_DRIVE_CORRESP)
        begAddCorresp(gns_GetCurrentInstance(), name);

    if (!DEACTIVATE_BEG)
      beg_def_sig(name, expr, tops(delay),tops(delayr),tops(delayf), getbiablflag(flags));
}
void
begAssign(char *name, char *expr, int delay, char *delayvar)
{
   begAssign_sub(name, expr, delay*1e-12, 0, 0, delayvar, 0);
}

/**************************************************************************************/

void
begAddBusDriver_sub(char *name, char *condition, char *value, double delay, double delayr, double delayf, char *delayvar, int flags)
{
    biabl_list *ptbiabl;

    if (BEG_TRACE)
        fprintf(stdout, "    signal %s <%s>\n",name,
                gns_GetSignalName(gns_GetCorrespondingSignal(name)));

    if (BEGAPI_DRIVE_CORRESP)
        begAddCorresp(gns_GetCurrentInstance(), name);

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_process(name, condition, value, tops(delay),  tops(delayr), tops(delayf), BEG_BUS,getsigflag(flags)|BEH_FLAG_FORCEBUS,getbiablflag(flags));
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
    /*    biabl_list *ptbiabl;

          ptbiabl = beg_def_mux(name, condition, NULL, value, delay);
          if (delayvar != NULL) {
          begRefDriver(delayvar, ptbiabl);
          }*/
}
void
begAddBusDriver(char *name, char *condition, char *value, int delay, char *delayvar)
{
   begAddBusDriver_sub(name, condition, value, delay*1e-12, 0, 0, delayvar, 0);
}

/**************************************************************************************/

void
begAddBusElse_sub(char *name, char *condition, char *value, double delay, double delayr, double delayf, char *delayvar, int flags)
{
    biabl_list *ptbiabl;

    if (BEG_TRACE)
        fprintf(stdout, "    signal %s <%s>\n",name,
                gns_GetSignalName(gns_GetCorrespondingSignal(name)));

    if (BEGAPI_DRIVE_CORRESP)
        begAddCorresp(gns_GetCurrentInstance(), name);

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_process(name, condition, value, tops(delay),  tops(delayr), tops(delayf), BEG_BUS|BEG_PRE,getsigflag(flags)|BEH_FLAG_FORCEBUS,getbiablflag(flags));
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}
void
begAddBusElse(char *name, char *condition, char *value, int delay, char *delayvar)
{
  begAddBusElse_sub(name, condition, value, delay*1e-12, 0, 0, delayvar, 0);
}
/**************************************************************************************/

void
begAddSelectDriver(char *name, char *select, char *when ,char *value, int delay, char *delayvar)
{
    biabl_list *ptbiabl;

    if (DEACTIVATE_BEG) return;

    ptbiabl     = beg_def_mux(name, select, when, value, delay,0,0);
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

/**************************************************************************************/
void
begAddBusDriverLoop(char *name, char *condition, char *value, char *loopvar, int delay, char *delayvar)
{
    biabl_list *ptbiabl;

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_loop(name, condition, value, loopvar, delay, 0,0,0);
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

/**************************************************************************************/
void
begAddBusDriverDoubleLoop(char *name, char *condition, char *value, char *loopvar1, char *loopvar2, int delay, char *delayvar)
{
    biabl_list *ptbiabl;

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_biloop(name, condition, value, loopvar1, loopvar2, delay, 0,0,0);
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

/**************************************************************************************/

void
begAddMemDriver_sub(char *name, char *condition, char *value, double delay, double delayr, double delayf, char *delayvar,int flags)
{
    biabl_list *ptbiabl;

    if (BEG_TRACE)
        fprintf(stdout, "    signal %s <%s>\n",name,
                gns_GetSignalName(gns_GetCorrespondingSignal(name)));

    if (BEGAPI_DRIVE_CORRESP)
        begAddCorresp(gns_GetCurrentInstance(), name);

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_process(name, condition, value, tops(delay),  tops(delayr), tops(delayf), BEG_REG,getsigflag(flags),getbiablflag(flags));
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

void
begAddMemDriver(char *name, char *condition, char *value, int delay, char *delayvar)
{
   begAddMemDriver_sub(name, condition, value, delay*1e-12, 0, 0, delayvar, 0);
}
/**************************************************************************************/

void
begAddMemDriverLoop(char *name, char *condition, char *value, char *loopvar, int delay, char *delayvar)
{
    biabl_list *ptbiabl;

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_loop(name, condition, value, loopvar, delay, BEG_REG,0,0);
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

/**************************************************************************************/

void
begAddMemDriverDoubleLoop(char *name, char *condition, char *value, char *loopvar1, char *loopvar2, int delay, char *delayvar)
{
    biabl_list *ptbiabl;

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_biloop(name, condition, value, loopvar1, loopvar2, delay, BEG_REG,0,0);
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

/**************************************************************************************/

void
begAddMemElse_sub(char *name, char *condition, char *value, double delay, double delayr, double delayf, char *delayvar, int flags)
{
    biabl_list *ptbiabl;

    if (BEG_TRACE)
        fprintf(stdout, "    signal %s <%s>\n",name,
                gns_GetSignalName(gns_GetCorrespondingSignal(name)));

    if (BEGAPI_DRIVE_CORRESP)
        begAddCorresp(gns_GetCurrentInstance(), name);

    if (DEACTIVATE_BEG) return;

    ptbiabl = beg_def_process(name, condition, value, tops(delay), tops(delayr), tops(delayf), BEG_REG|BEG_PRE,getsigflag(flags),getbiablflag(flags));
    if (delayvar != NULL && strcmp(delayvar,"NULL")!=0) {
        begRefDriver(delayvar, ptbiabl);
    }
}

void
begAddMemElse(char *name, char *condition, char *value, int delay, char *delayvar)
{

  begAddMemElse_sub(name, condition, value, delay*1e-12, 0, 0, delayvar, 0);
}
/**************************************************************************************/

void
begAddWarningCheck(char *testexpr, char *message)
{
    if (DEACTIVATE_BEG) return;

    beg_assertion(testexpr, message, 'W', NULL);
}

/**************************************************************************************/

void
begAddErrorCheck(char *testexpr, char *message)
{
    if (DEACTIVATE_BEG) return;

    beg_assertion(testexpr, message, 'E', NULL);
}

/**************************************************************************************/

void
begSort()
{
    if (DEACTIVATE_BEG) return;

    beg_sort();
}

/**************************************************************************************/

void
begCompact()
{
    chain_list *ptchain, *ptchain0;
    chain_list *refchain, *newrefchain;
    biabl_list *ptdriver;
    char       *varname;

    if (DEACTIVATE_BEG) return;

    beg_compact();
#if 0
    for (ptchain = BEGAPI_INTVARLIST; ptchain; ptchain = ptchain->NEXT) {
        varname = (char *)ptchain->DATA;
        refchain = (chain_list *)gethtitem(BEGAPI_INTVARHT, varname);
        newrefchain = NULL;
        for (ptchain0 = refchain; ptchain0; ptchain0 = ptchain0->NEXT) {
            ptdriver = (biabl_list *)ptchain0->DATA;
            if (!beg_isDeletedBiabl(ptdriver)) newrefchain = addchain(newrefchain, ptdriver);
        }
        freechain(refchain);
        sethtitem(BEGAPI_INTVARHT, varname, (long)newrefchain);
    }
#endif
}

/**************************************************************************************/

void
begSetDelay(char *varname, int value)
{
    chain_list *varchain;
    biabl_list *ptdriver;

    if (DEACTIVATE_BEG) return;

    if (BEH_TIMEVARS==NULL) BEH_TIMEVARS=addht(32);
    if (value<0) value=0;
    addhtitem(BEH_TIMEVARS, namealloc(varname), (long)value);
    
#if 0
    if ((varchain = (chain_list *)gethtitem(BEGAPI_INTVARHT, namealloc(varname))) != (void *)EMPTYHT) {
        for (;varchain; varchain = varchain->NEXT) {
            ptdriver  = (biabl_list *)varchain->DATA;
            ptdriver->TIME = value;
        }
    }
    else {
        avt_errmsg(BEG_API_ERRMSG, "003", AVT_WARNING, varname);
//        avt_error("begapi", 2, AVT_WAR, "delay variable '%s' does not exist\n", varname);
    }
#endif
}

/**************************************************************************************/

void
begBuildModel()
{
  char *name;

  name=_BegGetName();
  if (BEG_CHECK_EXISTENCE(name)==0)
    {
      begCreateModelInterface(name);
      begAddAllInstanceModels(0);
    }
  else
    begAddAllInstanceModels(0);
  begKeepModel();
}

/**************************************************************************************/

void
begBuildModelUsingFiles(char *name)
{
    BEGAPI_USEFILES = 1;
    begBuildModel(name);
    BEGAPI_USEFILES = 0;
}

/**************************************************************************************/

void
begBuildCompactModel()
{
  char *name;
  name=_BegGetName();
  if (BEG_CHECK_EXISTENCE(name)==0)
    {
      begCreateModelInterface(name);
      begAddAllInstanceModels(1);
      begSort();
      begCompact();
    }
  else
    begAddAllInstanceModels(1);
  begKeepModel();
}

/**************************************************************************************/

void
begBuildCompactModelUsingFiles(char *name)
{
    BEGAPI_USEFILES = 1;
    begBuildCompactModel(name);
    BEGAPI_USEFILES = 0;
}

/**************************************************************************************/

void
begBiterize()
{
    befig_list *ptbefig;
    char        figname[1024], *name;

    if (DEACTIVATE_BEG) return;

    ptbefig = beg_get_befig(BEG_NOVERIF);
    if (ptbefig != NULL)
    {
        name          = ptbefig->NAME;
        ptbefig       = beh_duplicate(ptbefig);
        sprintf(figname,"%s_duplicated",name);
        ptbefig->NAME = namealloc(figname);
        savebefig(ptbefig, 0);

        beh_biterize(ptbefig);
        sprintf(figname,"%s_biterized",name);
        ptbefig->NAME = namealloc(figname);
        savebefig(ptbefig, 0);

        beh_frebefig(ptbefig);
    }
}

/**************************************************************************************/

void begExport(char *name)
{
    befig_list *bf, *bf0;

    name=namealloc(name);

    if (begGetModel(name)==NULL) {
        bf0=beg_get_befig(BEG_NOVERIF);
        if (bf0!=NULL) {
            bf=beh_duplicate(bf0);
            if (BEGAPI_DRIVE_CORRESP)
                begTransfertCorresp(name, gns_GetCurrentInstance(), 0);

            bf->NAME=name;
            //          savebefig(bf, 0);
            addhtitem(BEG_MODELHT, bf->NAME, (long)bf);
        }
        else
          avt_errmsg(BEG_API_ERRMSG, "004", AVT_WARNING, name);
//          avt_error("begapi", 5, AVT_WAR,"could not export behaviour '%s'. Current instance behaviour is not created yet\n",name);
    }
    else
      avt_errmsg(BEG_API_ERRMSG, "005", AVT_WARNING, name);
//      avt_error("begapi", 4, AVT_WAR,"behaviour '%s' already exists\n",name);
}

/**************************************************************************************/

void begImport(char *name)
{
    befig_list *bf, *bf0;
    int update=0;
    char *mname;

    name=namealloc(name);
    mname=_BegGetName();

    if (BEGAPI_DRIVE_CORRESP)
      {
        if (CUR_CORRESP_TABLE->FLAGS & LOINS_IS_BLACKBOX) update=1;
        begTransfertCorresp(gns_GetCurrentInstance(), name, update);
      }

    if (BEG_CHECK_EXISTENCE(mname))
    {
      _begKeepModel(0);
      return;
    }

    if ((bf0=begGetModel(name))!=NULL)
      {
        begCreateInterface();
        bf=beh_duplicate(bf0);
        // to avoid destruction of the model correspondance table
        bf->NAME=namealloc("temporary_figure_for_import");
        //      savebefig(bf, 0);
        beg_eat_figure(bf);
        _begKeepModel(0);
    }
    else
      avt_errmsg(BEG_API_ERRMSG, "006", AVT_WARNING, name);
//      avt_error("begapi", 3, AVT_WAR,"could not import behaviour '%s'. It does not exist\n",name);
}

/**************************************************************************************/
