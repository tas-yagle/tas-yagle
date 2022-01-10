
#include STM_H
#include TTV_H
#include GEN_H
#include MLU_H
#include CNS_H
#include INF_H
#include EFG_H
#include TAS_H
#include TRC_H
#include YAG_H
#include MCC_H

#define API_USE_REAL_TYPES
#include "api_communication.h"
#include "gen_API.h"
#include "ttv_API_LOCAL.h"
#include "ttv_API.h"

#define TTV_DUP_LOFIG_PTYPE 0x23450001
#define TTV_DONE_PTYPE 0x23450002
#define TTV_TEMP_PTYPE 0x0fab007
#define TTV_mark_IN 1
#define TTV_mark_OUT 2
#define TTV_mark_HZ 4

#define IFGNS(a) if (GENIUS_GLOBAL_LOFIG!=NULL) { a }

static int CALLED_TTV_FUNCTION;
static lofig_list *current_dup_lofig;
static ht *nodeht;
static ttvfig_list *current_ttvfig;
static lofig_list *TTV_BASE_LOFIG, editmodelofig;
static chain_list *ALL_ADDED_NODES;
static loins_list *loins_removed;
static losig_list *losig_removed;
static chain_list *ALL_MY_TTV=NULL;
static char *TTV_FUNCTIONS_FILE;
static double TTVSLOPE=200e-12, TTVCAPA=0;
static chain_list *ALL_LINES=NULL;
static int TTVFLAGS=0;
static char *TTV_FORCED_NAME=NULL;
static float ttv_low=-1, ttv_high=-1;

#define TTV_INSTANCE_MODE 1
#define TTV_DRIVE_DTX 2

static mbk_options_pack_struct ttv_opack[]=
  {
    {"ttvUseInstanceMode", TTV_INSTANCE_MODE},
    {"ttvDriveDTX", TTV_DRIVE_DTX}
  };


void ttv_SetOutputSwing(double vss, double vdd)
{
  ttv_low=vss;
  ttv_high=vdd;
}

void ttv_API_AtLoad_Initialize() // commentaire pour desactiver l'ajout de token
{
  TTVFLAGS=mbk_ReadFlags(__API_FLAGS, ttv_opack, sizeof(ttv_opack)/sizeof(*ttv_opack), 0, 0);
}

void ttv_API_Action_Initialize() // commentaire pour desactiver l'ajout de token
{
/*  TTVSLOPE=200e-12;
  TTVCAPA=0;*/
  CALLED_TTV_FUNCTION=0;
  current_dup_lofig=NULL;
  TTV_BASE_LOFIG=NULL;
  TTV_FUNCTIONS_FILE=NULL;
}

void ttv_SetBaseNetlist(lofig_list *lf)
{
  char buf[1024];

  TTV_BASE_LOFIG=lf;

  if (!TTV_FUNCTIONS_FILE && lf!=NULL) 
  {
    sprintf (buf, "%s_functions.c", lf->NAME);
    TTV_FUNCTIONS_FILE = namealloc (buf);
  }
}

void ttv_UseInstanceMode()
{
  TTV_FORCED_NAME=gen_getinstancename();
}

void ttv_SetFunctionsFile (char *functions_file)
{
  TTV_FUNCTIONS_FILE = namealloc (functions_file);
}

static void checkformoretags(ttvsig_list *ptsig)
{
  if((ptsig->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_Q | TTV_SIG_R | TTV_SIG_B |
                     TTV_SIG_N)) != 0)
    {
      ptsig->NODE[0].TYPE |= TTV_NODE_STOP;
      ptsig->NODE[1].TYPE |= TTV_NODE_STOP;
    }
}


static ttvsig_list *ttvapi_GetNode(char *node, char *root, float capa)
{
  long res;
  char temp[1024], *code;

  node=ttv_revect(gen_makesignalname(node));
  root=ttv_revect(gen_makesignalname(root));

  sprintf(temp,"*:%s",node);
  code=namealloc(temp);
  if ((res=gethtitem(nodeht, code))!=EMPTYHT) 
    {
      if (capa!=0.0 && ((ttvsig_list *)res)->CAPA==0.0)
        ((ttvsig_list *)res)->CAPA=capa;
      return (ttvsig_list *)res;
    }

  sprintf(temp,"%s:%s",root,node);
  code=namealloc(temp);
  if ((res=gethtitem(nodeht, code))!=EMPTYHT) 
    {
      if (capa!=0.0 && ((ttvsig_list *)res)->CAPA==0.0)
        ((ttvsig_list *)res)->CAPA=capa;
      return (ttvsig_list *)res;
    }

  ALL_ADDED_NODES=ttv_addrefsig(current_ttvfig, node, root, capa, 0, ALL_ADDED_NODES) ;
  addhtitem(nodeht, code, (long)ALL_ADDED_NODES->DATA);
  sprintf(temp,"*:%s",node);
  code=namealloc(temp);
  addhtitem(nodeht, code, (long)ALL_ADDED_NODES->DATA);
  return (ttvsig_list *)ALL_ADDED_NODES->DATA;
}

static void GrabCreatedNodes(chain_list *cl)
{
  char temp[1024], *code;  
  ttvsig_list *tvs;

  while (cl!=NULL)
    {
      tvs=(ttvsig_list *)cl->DATA;
      sprintf(temp,"%s:%s",tvs->NETNAME,tvs->NAME);
      code=namealloc(temp);
      if (gethtitem(nodeht, code)==EMPTYHT)
        {
          addhtitem(nodeht, code, (long)tvs);
          sprintf(temp,"*:%s",tvs->NAME);
          code=namealloc(temp);
          addhtitem(nodeht, code, (long)tvs);
          ALL_ADDED_NODES=addchain(ALL_ADDED_NODES, tvs);
        }
      cl=cl->NEXT;
    }
}

static double ttv_get_transistor_capa (locon_list *cir_ptcon)
{
    double trcapa = 0.0;
    lotrs_list *trs, *shrinked_trs;
    
    trs = (lotrs_list*)cir_ptcon->ROOT;
    shrinked_trs = trs;
//    shrinked_trs = duplotrs (trs);
    ELP_CALC_ONLY_CAPA = 1;
    elpLoadOnceElp();
    //elpLotrsShrink (shrinked_trs);
    if (trs->GRID == cir_ptcon)
        trcapa = elpGetCapaFromLocon (shrinked_trs->GRID,ELP_CAPA_TYPICAL,elpTYPICAL);
    else
    if (trs->DRAIN == cir_ptcon)
        trcapa = elpGetCapaFromLocon (shrinked_trs->DRAIN,ELP_CAPA_TYPICAL,elpTYPICAL);
    else
    if (trs->SOURCE == cir_ptcon)
        trcapa = elpGetCapaFromLocon (shrinked_trs->SOURCE,ELP_CAPA_TYPICAL,elpTYPICAL);

//    mbkfree (shrinked_trs);
    ELP_CALC_ONLY_CAPA = 0;

    return trcapa;
}

//--- copied directly from tas -----
static float ttvapi_tas_gettotalcapa(lofig_list *lofig, losig_list *losig)
{
    float capa = 0.0;
    chain_list *chain;
    
    if(!lofig)
        lofig = TAS_CONTEXT->TAS_LOFIG;
    if(elpIsCapaSig(lofig, losig, ELP_CAPA_TYPICAL)){
        return ((elpGetCapaSig(lofig, losig, ELP_CAPA_TYPICAL) + rcn_getcapa(lofig,losig)) * 1000.0);
    }else{
        for(chain = (chain_list*)getptype(losig->USER,LOFIGCHAIN)->DATA; chain; chain = chain->NEXT)
            capa += elpGetCapaFromLocon((locon_list*)chain->DATA,ELP_CAPA_TYPICAL,elpTYPICAL);
        elpAddCapaSig(lofig, losig, capa, ELP_CAPA_TYPICAL);
        return ((capa  + rcn_getcapa(lofig,losig)) * 1000.0);
    }
}
//---------------------------------

static void SetConCapa(lofig_list *lf, chain_list *cl)
{
  float capa;
  chain_list *ch;
  ttvsig_list *tvs;
  losig_list *ls;
  locon_list *lc;
  
  while (cl!=NULL)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if ((tvs->TYPE & TTV_SIG_CX)==TTV_SIG_CX) 
         tvs->USER=addptype(tvs->USER, TTV_TEMP_PTYPE, (void *)0);
      if ((ls=mbk_quickly_getlosigbyname(lf, tvs->NETNAME))!=NULL)
       {
        tvs->CAPA=ttvapi_tas_gettotalcapa(lf, ls);        
       }
      cl=cl->NEXT;
    }
}

static void UpdateRCXLocon(char *con_name, char *insname, char *rcxname)
{
  locon_list *lc;
  chain_list *lst, *cl;
  subinst_t *sins;
  losig_list *ls;
  sins=gen_get_hier_instance(LATEST_GNS_RUN, insname);
  if (sins->FLAGS & LOINS_IS_BLACKBOX) return;
  ls=gen_corresp_sig (con_name, sins->CRT);
  cl=lst=GrabAllConnectorsThruCorresp(con_name, ls, sins->CRT, NULL);
  while (cl!=NULL)
    {
      lc=(locon_list *)cl->DATA;
      addloconrcxname(lc, rcxname);
      cl=cl->NEXT;
    }
  freechain(lst);
}

static int CheckFirstCallForThisModel()
{
  ptype_list *p;
  loins_list *li, *prev, *next;
  ttvfig_list *tvf;
  timing_cell *cell;
  losig_list *ls, *prevls, *nextls;
  mbkContext tmpctx;
  
  IFGNS( if (TTV_BASE_LOFIG==NULL) TTV_BASE_LOFIG=__gns_GetNetlist (); )
  IFGNS( if ((TTVFLAGS & TTV_INSTANCE_MODE)==0 && TTV_FORCED_NAME==NULL && (p=getptype(TTV_BASE_LOFIG->USER, TTV_DONE_PTYPE))!=NULL) return 1; )
  
  if (current_dup_lofig!=NULL) return 0;

  CALLED_TTV_FUNCTION=1;
  ALL_LINES=NULL;
  ALL_ADDED_NODES=NULL;

  elpLoadOnceElp();
  rcx_env();
  //avt_savecontext();
  yagenv (tas_yaginit);
  tas_setenv();

  if (TTV_BASE_LOFIG==NULL) return 1;
  
  current_dup_lofig=rduplofig(TTV_BASE_LOFIG);
  
  if (TTV_FORCED_NAME!=NULL)
    current_dup_lofig->NAME=TTV_FORCED_NAME;
  else IFGNS( if (TTVFLAGS & TTV_INSTANCE_MODE) current_dup_lofig->NAME=gen_getinstancename(); )
  
  loins_removed=NULL;
  losig_removed=NULL;

  for (li=current_dup_lofig->LOINS, prev=NULL; li!=NULL; li=next)
    {
      next=li->NEXT;

      if (!(((tvf=ttv_gethtmodel(li->FIGNAME))!=NULL || (tvf=ttv_gethtmodel(li->INSNAME))!=NULL) && tvf->STATUS & TTV_STS_DTX))
        {
          if (prev==NULL) current_dup_lofig->LOINS=li->NEXT;
          else prev->NEXT=li->NEXT;
          li->NEXT=loins_removed; loins_removed=li;
        }
      else prev=li;
    }
  lofigchain(current_dup_lofig);
  
  for (ls=current_dup_lofig->LOSIG, prevls=NULL; ls!=NULL; ls=nextls)
    {
      nextls=ls->NEXT;
      if (getptype(ls->USER,LOFIGCHAIN)->DATA==NULL)
        {
          if (prevls==NULL) current_dup_lofig->LOSIG=ls->NEXT;
          else prevls->NEXT=ls->NEXT;
          ls->NEXT=losig_removed; losig_removed=ls;
        }
      else prevls=ls;
    }
  


   if ((tvf=ttv_gethtmodel(current_dup_lofig->NAME))!=NULL)
    {
      if((cell = stm_getcell(tvf->INFO->FIGNAME)) != NULL){
        stm_freecell(tvf->INFO->FIGNAME) ;
        cell = NULL;
      }
      ttv_unlockttvfig(tvf);
      ttv_freeallttvfig(tvf);
    }

  IFGNS( memcpy(&tmpctx, genius_external_getcontext(), sizeof(tmpctx));
         mbkSwitchContext(&tmpctx); // passage dans le context NORMAL pour les modeles de transistors 
       )

  tas_TechnoParameters();
  tas_capasig(current_dup_lofig);
  elpLofigAddCapas ( current_dup_lofig, elpTYPICAL );

  IFGNS( mbkSwitchContext(&tmpctx); )

  nodeht=addht(1024);

  current_ttvfig = tas_builtfig(current_dup_lofig);

  ttv_lockttvfig(current_ttvfig);

  tas_loconorient(current_dup_lofig, current_ttvfig);

  tas_setsigname(current_dup_lofig);
  
  IFGNS( memcpy(&tmpctx, genius_external_getcontext(), sizeof(tmpctx));
         mbkSwitchContext(&tmpctx); // passage dans le context NORMAL pour les modeles de transistors 
       )
          
  rcx_create(current_dup_lofig);
  
  tas_builtrcxview(current_dup_lofig, current_ttvfig);

  buildrcx(current_dup_lofig);

  IFGNS( mbkSwitchContext(&tmpctx); )

  tas_detecloconsig(current_dup_lofig, current_ttvfig);
  
  tas_calcrcxdelay(current_dup_lofig, current_ttvfig, TTV_FILE_DTX);

  tas_builtline(current_ttvfig, TTV_LINE_D);

  tas_cleanfig(current_dup_lofig, current_ttvfig);

  GrabCreatedNodes((chain_list *)current_ttvfig->ELCMDSIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->ELATCHSIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->EBREAKSIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->EPRESIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->EXTSIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->ESIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->CONSIG);
  SetConCapa(current_dup_lofig, (chain_list *)current_ttvfig->CONSIG);
  GrabCreatedNodes((chain_list *)current_ttvfig->NCSIG);
  current_ttvfig->NBCONSIG=current_ttvfig->NBELCMDSIG=current_ttvfig->NBELATCHSIG=
  current_ttvfig->NBEBREAKSIG=current_ttvfig->NBEPRESIG=current_ttvfig->NBEXTSIG=current_ttvfig->NBNCSIG=0;

  return 0;
}

static chain_list *tabtochain(ttvsig_list ***tab, int nb)
{
  chain_list *cl=NULL;
  int i;
  for (i=0;i<nb;i++) cl=addchain(cl, (*tab)[i]);
  mbkfree(*tab);
  //cl=reverse(cl);
  *tab=(ttvsig_list **)cl;
  return cl;
}
void ttv_EditTimingFigure(ttvfig_list *tvf)
{
  chain_list *uncache;
  long type, lev=0;
  if (TTV_BASE_LOFIG!=NULL || tvf==NULL)
  {
    return;       
  }

  ttv_API_Action_Initialize();

  CALLED_TTV_FUNCTION=1;
  ALL_LINES=NULL;
  ALL_ADDED_NODES=NULL;

  elpLoadOnceElp();
  rcx_env();
  //avt_savecontext();
  yagenv (tas_yaginit);
  tas_setenv();
  current_dup_lofig=TTV_BASE_LOFIG=&editmodelofig;
  current_ttvfig = tvf;

  ttv_lockttvfig(current_ttvfig);

  ttv_setcachesize(-1,-1);
  //ttv_disablecache(tvf) ; // desactivation du cache
 //ttv_setcachesize(TTV_ALLOC_MAX, TTV_ALLOC_MAX) ; // desactivation du cache
  type=ttv_getloadedfigtypes(tvf);

/*  if (type & TTV_FILE_DTX)
  {
   uncache=ttv_levelise(tvf,tvf->INFO->LEVEL,TTV_FIND_LINE);
   freechain(uncache);
  }
  if (type & TTV_FILE_TTX)
  {
   uncache=ttv_levelise(tvf,tvf->INFO->LEVEL,TTV_FIND_PATH);
   freechain(uncache);
  }
  */
  nodeht=addht(1024);
  loins_removed=NULL;
  losig_removed=NULL;

  GrabCreatedNodes(tabtochain(&current_ttvfig->CONSIG, current_ttvfig->NBCONSIG));
  GrabCreatedNodes(tabtochain(&current_ttvfig->ELCMDSIG, current_ttvfig->NBELCMDSIG));
  GrabCreatedNodes(tabtochain(&current_ttvfig->ELATCHSIG, current_ttvfig->NBELATCHSIG));
  GrabCreatedNodes(tabtochain(&current_ttvfig->EBREAKSIG, current_ttvfig->NBEBREAKSIG));
  GrabCreatedNodes(tabtochain(&current_ttvfig->EPRESIG, current_ttvfig->NBEPRESIG));
  GrabCreatedNodes(tabtochain(&current_ttvfig->EXTSIG, current_ttvfig->NBEXTSIG));
  GrabCreatedNodes(tabtochain(&current_ttvfig->NCSIG, current_ttvfig->NBNCSIG));
  SetConCapa(current_dup_lofig, (chain_list *)current_ttvfig->CONSIG);
  current_ttvfig->NBCONSIG=current_ttvfig->NBELCMDSIG=current_ttvfig->NBELATCHSIG=
  current_ttvfig->NBEBREAKSIG=current_ttvfig->NBEPRESIG=current_ttvfig->NBEXTSIG=current_ttvfig->NBNCSIG=0;

}

void ttv_CreateTimingFigure(lofig_list *lf)
{
  ttv_API_Action_Initialize();
  ttv_SetBaseNetlist(lf);
  
}

void ttv_dir_to_index(char *dir, int *a, int *b)
{
  if (strlen(dir)!=2) { *a=-1; return ; }
  if (dir[0]=='u' || dir[0]=='U') *a=1;
  else if (dir[0]=='d' || dir[0]=='D') *a=0;
  else { *a=-1; return ; }
  if (dir[1]=='u' || dir[1]=='U') *b=1;
  else if (dir[1]=='d' || dir[1]=='D') *b=0;
  else { *a=-1; return ; }
}

static char *getsignal(char *name, char *type, float *capa)
{
  losig_list *ls=NULL, *cls;
  loins_list *li;
  locon_list *lc;
  chain_list *arbo, *main_arbo;
  ht *sight;
  long l;

  arbo=main_arbo=gen_hierarchical_split(gen_makesignalname(name));
  if (GENIUS_GLOBAL_LOFIG!=NULL)
    {
      if (arbo->NEXT==NULL)
        {
          sight=gen_get_losig_ht(TTV_BASE_LOFIG);

          if ((l=gethtitem(sight, arbo->DATA))==EMPTYHT) ls=NULL;
          else ls=(losig_list *)l;

          if (ls!=NULL)
            {
              if (ls->TYPE=='E') *type='c';
              else *type='s';
            }
        }
      else
        {
          for (li=TTV_BASE_LOFIG->LOINS; li!=NULL && li->INSNAME!=arbo->DATA; li=li->NEXT) ;
          if (li!=NULL)
            {
              for (lc=li->LOCON; lc!=NULL && lc->NAME!=arbo->NEXT->DATA; lc=lc->NEXT) ;
              if (lc!=NULL)
                {
                  ls=lc->SIG;
                  if (ls->TYPE=='E') *type='n';
                  else *type='s';
                }
            }
          else
            {
              sight=gen_get_losig_ht(TTV_BASE_LOFIG);

              if ((l=gethtitem(sight, arbo->DATA))==EMPTYHT) ls=NULL;
              else ls=(losig_list *)l;

              if (ls!=NULL)
                {
                  if (ls->TYPE=='E') *type='n';
                  else *type='s';
                }
            }
        }

      cls=gen_corresp_sig (gen_makesignalname(name), CUR_CORRESP_TABLE);
      if (cls!=NULL)
        {
          //      rcn_refresh_signal(GENIUS_GLOBAL_LOFIG, cls);
          *capa=ttvapi_tas_gettotalcapa(GENIUS_GLOBAL_LOFIG, cls);
        }
      else
        *capa=0.0;
    }
  else if (TTV_BASE_LOFIG!=&editmodelofig)
    {
      ls=mbk_quickly_getlosigbyname(TTV_BASE_LOFIG, (char *)arbo->DATA);
      if (ls!=NULL)
        {
          *capa=rcn_getcapa(NULL, ls);
          if (ls->TYPE=='E') *type='c';
          else *type='s';
        }
      else
        {
          *capa=0;
        }
    }
  else
     *capa=0;
  freechain(arbo);
  if (ls==NULL) { *type='s'; return namealloc(name); }
  return gen_losigname(ls);
}

static void SetNodeType(ttvsig_list *tvs, char type)
{
  if (TTV_BASE_LOFIG==&editmodelofig && type=='s' && (tvs->TYPE & (TTV_SIG_C|TTV_SIG_Q|TTV_SIG_B|TTV_SIG_L|TTV_SIG_R))!=0) return;

  switch(type)
    {
    case 'c':
      if ((tvs->TYPE & TTV_SIG_N)==TTV_SIG_N
          ||(tvs->TYPE & TTV_SIG_S)==TTV_SIG_S)
        {
          avt_errmsg(TTV_API_ERRMSG, "003", AVT_WARNING, (tvs->TYPE & TTV_SIG_N)==TTV_SIG_N?'n':'s',type);
//          avt_error("ttvapi", 3, AVT_WAR, "conflicting node type old:'%c' new:'%c'\n",(tvs->TYPE & TTV_SIG_N)==TTV_SIG_N?'n':'s',type);
          return;
        }
      tvs->TYPE|=TTV_SIG_C;
      checkformoretags(tvs);
      break;
    case 'n':
      if ((tvs->TYPE & TTV_SIG_N)==TTV_SIG_C
          ||(tvs->TYPE & TTV_SIG_S)==TTV_SIG_S)
        {
          avt_errmsg(TTV_API_ERRMSG, "003", AVT_WARNING, (tvs->TYPE & TTV_SIG_C)==TTV_SIG_N?'c':'s',type);
//          avt_error("ttvapi", 3, AVT_WAR, "conflicting node type old:'%c' new:'%c'\n",(tvs->TYPE & TTV_SIG_C)==TTV_SIG_N?'c':'s',type);
          return;
        }
      tvs->TYPE|=TTV_SIG_N;
      checkformoretags(tvs);
      break;
    case 's':
      if ((tvs->TYPE & TTV_SIG_N)==TTV_SIG_C
          ||(tvs->TYPE & TTV_SIG_S)==TTV_SIG_N)
        {
          avt_errmsg(TTV_API_ERRMSG, "003", AVT_WARNING, (tvs->TYPE & TTV_SIG_C)==TTV_SIG_N?'c':'n',type);
//          avt_error("ttvapi", 3, AVT_WAR, "conflicting node type old:'%c' new:'%c'\n",(tvs->TYPE & TTV_SIG_C)==TTV_SIG_N?'c':'n',type);
          return;
        }
      if ((tvs->TYPE & (TTV_SIG_Q | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B))!=0) return;
      tvs->TYPE|=TTV_SIG_S;
      checkformoretags(tvs);
      break;
    case ' ':
      break;
    default:
      avt_errmsg(TTV_API_ERRMSG, "004", AVT_ERROR, type);
//      avt_error("ttvapi", 3, AVT_ERR, "type '%c' is unknown\n",type);
      return;
    }
}

char gettype(int type)
{
  if ((type & TTV_SIG_L)==TTV_SIG_L) return 'l';
  if ((type & TTV_SIG_R)==TTV_SIG_R) return 'r';
  if ((type & TTV_SIG_Q)==TTV_SIG_Q) return 'q';
  if ((type & TTV_SIG_B)==TTV_SIG_B) return 'b';
  return '?';
}

static int CompareSubtype(ttvsig_list *tvs0, ttvsig_list *tvs1)
{
  int t0, t1, res;
  if (tvs0->NETNAME!=tvs1->NETNAME) return 0;
  t0=(tvs0->TYPE & TTV_SIG_TYPE) & ~TTV_SIG_TYPECON;
  t1=(tvs1->TYPE & TTV_SIG_TYPE) & ~TTV_SIG_TYPECON;
  res=t0 | t1;
  if (res!=0)
    {
      tvs0->TYPE &= ~TTV_SIG_S;
      tvs1->TYPE &= ~TTV_SIG_S;
      tvs0->TYPE |= res;
      tvs1->TYPE |= res;
      checkformoretags(tvs0);
      checkformoretags(tvs1);
    }
  return 1;
}
static void SetNodeSubtype(ttvsig_list *tvs, char subtype)
{
  switch(subtype)
    {
    case 'q':
      if ((tvs->TYPE & (TTV_SIG_L | TTV_SIG_R /*| TTV_SIG_B*/))!=0)
        {
          avt_errmsg(TTV_API_ERRMSG, "005", AVT_WARNING, gettype(tvs->TYPE), subtype);
//          avt_error("ttvapi", 2, AVT_WAR, "overriding type '%c' with '%c'\n",gettype(tvs->TYPE), subtype);
        }
      tvs->TYPE&=~(TTV_SIG_R|TTV_SIG_L|/*TTV_SIG_B|*/TTV_SIG_S);
      tvs->TYPE|=TTV_SIG_Q;
      checkformoretags(tvs);
      break;
    case 'l':
      if ((tvs->TYPE & (TTV_SIG_Q | TTV_SIG_R | TTV_SIG_B))!=0)
        {
          avt_errmsg(TTV_API_ERRMSG, "005", AVT_WARNING, gettype(tvs->TYPE), subtype);
//          avt_error("ttvapi", 2, AVT_WAR, "overriding type '%c' with '%c'\n",gettype(tvs->TYPE), subtype);
        }
      tvs->TYPE&=~(TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B|TTV_SIG_S);
      tvs->TYPE|=TTV_SIG_LL;
      checkformoretags(tvs);
      break;
    case 'f':
      if ((tvs->TYPE & (TTV_SIG_Q | TTV_SIG_R | TTV_SIG_B))!=0)
        {
          avt_errmsg(TTV_API_ERRMSG, "005", AVT_WARNING, gettype(tvs->TYPE), subtype);
//          avt_error("ttvapi", 2, AVT_WAR, "overriding type '%c' with '%c'\n",gettype(tvs->TYPE), subtype);
        }
      tvs->TYPE&=~(TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B|TTV_SIG_S);
      tvs->TYPE|=TTV_SIG_LF;
      checkformoretags(tvs);
      break;
    case 'b':
      if ((tvs->TYPE & (TTV_SIG_L | TTV_SIG_R/* | TTV_SIG_Q*/))!=0)
        {
          avt_errmsg(TTV_API_ERRMSG, "005", AVT_WARNING, gettype(tvs->TYPE), subtype);
//          avt_error("ttvapi", 2, AVT_WAR, "overriding type '%c' with '%c'\n",gettype(tvs->TYPE), subtype);
        }
      tvs->TYPE|=TTV_SIG_B;
      tvs->TYPE&=~TTV_SIG_S;
      checkformoretags(tvs);
      break;
    case 'r':
      if ((tvs->TYPE & (TTV_SIG_L | TTV_SIG_Q | TTV_SIG_B))!=0)
        {
          avt_errmsg(TTV_API_ERRMSG, "005", AVT_WARNING, gettype(tvs->TYPE), subtype);
//         avt_error("ttvapi", 2, AVT_WAR, "overriding type '%c' with '%c'\n",gettype(tvs->TYPE), subtype);
        }
      tvs->TYPE&=~(TTV_SIG_L|TTV_SIG_Q|TTV_SIG_B|TTV_SIG_S);
      tvs->TYPE|=TTV_SIG_R;
      checkformoretags(tvs);
      break;
    case ' ':
      break;
    default:
      avt_errmsg(TTV_API_ERRMSG, "006", AVT_ERROR, subtype);
//      avt_error("ttvapi", 3, AVT_ERR, "subtype '%c' is unknown\n",subtype);
      return;
    }
}

static void SetAsBreakpointIfNeeded(ttvsig_list *tsig)
{
  if ((tsig->TYPE & TTV_SIG_L)!=TTV_SIG_L && (tsig->TYPE & TTV_SIG_Q)!=TTV_SIG_Q)
    {
      tsig->TYPE&=~(TTV_SIG_Q|TTV_SIG_S);
      tsig->TYPE|=TTV_SIG_B;
      checkformoretags(tsig);
    }
}

ttvsig_list *ttv_SetNodeSubtype(char *node, char subtype)
{
  char *sin;
  ttvsig_list *tsin;
  char it;
  float capa;

  if (CheckFirstCallForThisModel()) return NULL;
  if ((sin=getsignal(node, &it, &capa))==NULL) return NULL;
  tsin=ttvapi_GetNode(node, sin, capa);
  SetNodeType(tsin, it);
  SetNodeSubtype(tsin, subtype);
  return tsin;
}

ttvsig_list *ttv_AddConnector(char *name, char dir)
{
  ttvsig_list *tvs;
  long type;
  switch (dir)
  {
    case 'i': type=TTV_SIG_CI; break;
    case 'o': type=TTV_SIG_CO; break;
    case 'z': type=TTV_SIG_CZ; break;
    case 'b': type=TTV_SIG_CB; break;
    case 't': type=TTV_SIG_CT; break;
    case 'x': type=TTV_SIG_CX; break;
    default: type=TTV_SIG_CI;
      avt_errmsg(TTV_API_ERRMSG, "054", AVT_ERROR, dir);
  }
  tvs=ttv_AddCustomNode(name, name, 'c', ' ');
  tvs->TYPE&=~(TTV_SIG_CI|TTV_SIG_CO|TTV_SIG_CZ|TTV_SIG_CB|TTV_SIG_CT|TTV_SIG_CX);
  tvs->TYPE|=type;
  return tvs;
}

ttvsig_list *ttv_AddCommand(char *name)
{
  return ttv_SetNodeSubtype(name, 'q');
}

ttvsig_list *ttv_AddLatch(char *name)
{
  ttvsig_list *tsin;
  if (CheckFirstCallForThisModel()) return NULL;
  tsin=ttv_SetNodeSubtype(name, 'l');
  tsin->TYPE|=TTV_SIG_LL;
  checkformoretags(tsin);
  return tsin;
}

ttvsig_list *ttv_AddFlipFlop(char *name)
{
  ttvsig_list *tsin;
  if (CheckFirstCallForThisModel()) return NULL;
  tsin=ttv_SetNodeSubtype(name, 'l');
  tsin->TYPE|=TTV_SIG_LF;
  checkformoretags(tsin);
  return tsin;
}

ttvsig_list *ttv_AddPrecharge(char *name)
{
  return ttv_SetNodeSubtype(name, 'r');
}

ttvsig_list *ttv_AddBreakpoint(char *name)
{
  return ttv_SetNodeSubtype(name, 'b');
}

TimingSignal *ttv_GetNode(char *node, char *signal)
{
  ttvsig_list *tvs;
  if (CheckFirstCallForThisModel()) return NULL;
  tvs=ttvapi_GetNode(node, signal, 0.0);
  return tvs;
}

TimingSignal *ttv_AddCustomNode(char *node, char *signal, char type, char subtype)
{
  ttvsig_list *tvs;
  if (CheckFirstCallForThisModel()) return NULL;
  if (tolower(type)!='c' && tolower(type)!='n' && tolower(type)!='s')
    {
      IFGNS ( avt_errmsg(TTV_API_ERRMSG, "007", AVT_ERROR, gen_info(), type); )
      //IFGNS ( avt_error("ttvapi", 2, AVT_ERR, "%s: invalid node type '%c', ttv_AddCustomSignal ignored\n", gen_info(), type); )
    }
  tvs=ttvapi_GetNode(node, signal, 0.0);
  SetNodeType(tvs, type);
  if (subtype!=' ') SetNodeSubtype(tvs, subtype);
  return tvs;
}

void ttv_Config(double value, double out_capa)
{
  TTVSLOPE=value;
  TTVCAPA=out_capa;
}

char *ttv_getarcdef(ttvline_list *tvl)
{
  char temp[1024];
  char *type="mod";
  ttvsig_list *tvss, *tvsd;
  tvss=tvl->NODE->ROOT;
  tvsd=tvl->ROOT->ROOT;

  if ((tvl->TYPE & TTV_LINE_U)==TTV_LINE_U) type="setup";
  else if ((tvl->TYPE & TTV_LINE_O)==TTV_LINE_O) type="hold";
  else if ((tvl->TYPE & TTV_LINE_A)==TTV_LINE_A) type="access";
    
  if ((TTVFLAGS & TTV_INSTANCE_MODE)==0)
    sprintf(temp,"%s_%s_%s_%c%c", type, tvss->NAME, tvsd->NAME, 
            tvl->NODE==&tvss->NODE[1]?'u':'d',
            tvl->ROOT==&tvsd->NODE[1]?'u':'d');
  else
    sprintf(temp,"%s-%s_%s_%s_%c%c", gen_getinstancename(), type, tvss->NAME, tvsd->NAME, 
            tvl->NODE==&tvss->NODE[1]?'u':'d',
            tvl->ROOT==&tvsd->NODE[1]?'u':'d');
  return namealloc(temp);
}

static double ttv_tas_thr2scm (double fthr, int dir)
{
  return fthr ;
  if (dir==0)
    {
      // DOWN
      return elpThr2Scm (fthr, elpGeneral[elpGSLTHR],
                         elpGeneral[elpGSHTHR],
                         elpGetModelType (TRANSN)->elpModel[elpVT],
                         0.0, elpGeneral[elpGVDDMAX], elpFALL);
    }
  else
    {
      // UP
      return elpThr2Scm (fthr, elpGeneral[elpGSLTHR],
                         elpGeneral[elpGSHTHR],
                         elpGetModelType (TRANSP)->elpModel[elpVT],
                         elpGeneral[elpGVDDMAX], elpGeneral[elpGVDDMAX], elpRISE);

    }
}

void checkslope(char *type, char *input, char *output, char *dir, double *val)
{

  if (*val<=0)
    {
      *val=(TAS_CONTEXT->FRONT_CON/TTV_UNIT)*1e-12;
      avt_errmsg(TTV_API_ERRMSG, "008", AVT_ERROR, type, TAS_CONTEXT->FRONT_CON/TTV_UNIT, input, output, dir);
//      avt_error("ttvapi", 1, AVT_WAR, "nul or negative %sslope set to %gps for line %s" AVT_YELLOW "->" AVT_RESET "%s %s\n", type, TAS_CONTEXT->FRONT_CON/TTV_UNIT, input, output, dir);
    }
}

ttvline_list *ttv_AddCustomLine(TimingSignal *input, TimingSignal *output, double max_delay, double max_slope, double min_delay, double min_slope, char *dir, int type)
{
  int a, b;
  long vali, valo, LD, LE, LF, loaded;
  ptype_list *pti, *pto;
  ttvline_list *tvl;
  ttvevent_list *lnode, *cmdnode;

  if (CheckFirstCallForThisModel()) return NULL;
  ttv_dir_to_index(dir, &a, &b);
  if (a==-1) 
    {
      char *where="somewhere";
      IFGNS ( where=gen_info(); )
      avt_errmsg(TTV_API_ERRMSG, "009", AVT_ERROR, where, dir);
//      avt_error("ttvapi", 3, AVT_ERR, "%s: invalid direction '%s', timing line creation ignored\n", where, dir);
      return NULL;
    }

  if ((pti=getptype(input->USER, TTV_TEMP_PTYPE))!=NULL) vali=(long)pti->DATA;
  if ((pto=getptype(output->USER, TTV_TEMP_PTYPE))!=NULL) valo=(long)pto->DATA;

  LD=TTV_LINE_D;
  LE=TTV_LINE_E;
  LF=TTV_LINE_F;

  if (TTV_BASE_LOFIG==&editmodelofig)
  {
    loaded=ttv_getloadedfigtypes(current_ttvfig);
    if ((loaded & TTV_FILE_TTX)==TTV_FILE_TTX)
    {
      LD=TTV_LINE_T;
      LE=TTV_LINE_J;
      LF=TTV_LINE_P;
      
      if ((type & TTV_LINE_D)==TTV_LINE_D) { type&=~TTV_LINE_D; type|=LD; }
      if ((type & TTV_LINE_E)==TTV_LINE_E) { type&=~TTV_LINE_E; type|=LE; }
      if ((type & TTV_LINE_F)==TTV_LINE_F) { type&=~TTV_LINE_F; type|=LF; }
    }
  }
     
  switch (type)
    {
    case TTV_SETUP:
    case TTV_HOLD:
    case TTV_ACCESS:
      if (type==TTV_SETUP) type=TTV_LINE_U;
      else if (type==TTV_HOLD) type=TTV_LINE_O;
      else type=TTV_LINE_A;


      if (((input->TYPE|output->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
        type|=LD;     
      else
        type|=LE;

      break;
    case TTV_TIMING:
      if (((input->TYPE|output->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
        type=LD;
      else
        type=LF;
    }

  if ((type & (TTV_LINE_O|TTV_LINE_A|TTV_LINE_U))!=0)
    {
      ptype_list *pt;
      SetAsBreakpointIfNeeded(input);
      SetAsBreakpointIfNeeded(output);
      if ((type & TTV_LINE_A)!=TTV_LINE_A) vali|=TTV_mark_IN, valo|=TTV_mark_IN;
      else vali|=TTV_mark_IN, valo|=TTV_mark_OUT;

      if ((type & TTV_LINE_A)==TTV_LINE_A)
        lnode=&output->NODE[b], cmdnode=&input->NODE[a];
      else
        lnode=&input->NODE[a], cmdnode=&output->NODE[(b+1) & 1];
      if (lnode->ROOT->TYPE & TTV_SIG_L)
      {
        if ((pt=getptype(lnode->ROOT->USER, TTV_SIG_CMD))==NULL)
          pt=lnode->ROOT->USER=addptype(lnode->ROOT->USER, TTV_SIG_CMD, NULL);

        if (getchain(pt->DATA, cmdnode)==NULL)
          pt->DATA=addchain((chain_list *)pt->DATA, cmdnode);
      }
    }
  else
    {
      vali|=TTV_mark_IN;
      valo|=TTV_mark_OUT;
      if ((type & TTV_LINE_HZ)==TTV_LINE_HZ) valo|=TTV_mark_HZ;
    }

  if (pti!=NULL) pti->DATA=(void *)vali;
  if (pto!=NULL) pto->DATA=(void *)valo;

  if (CompareSubtype(input, output)) 
    {
      type|=TTV_LINE_RC;
    }

  tvl=ttv_addline(current_ttvfig,
                  &output->NODE[b], &input->NODE[a],
                  (long)(max_delay*1e12*TTV_UNIT),
                  (long)ttv_tas_thr2scm(max_slope*1e12*TTV_UNIT, b),
                  (long)(min_delay*1e12*TTV_UNIT),
                  (long)ttv_tas_thr2scm(min_slope*1e12*TTV_UNIT, b),
                  type);
  IFGNS (
           ALL_LINES=addchain(ALL_LINES, tvl);
        )
  return tvl;
}

void ttv_Associate_Model(ttvline_list *tvl, void *func)
{
  char *nt;
  timing_model *tmodel;
  HierLofigInfo *hli;
  ArcInfo *ai;
  double capa = 0.0, slope = STM_DEF_SLEW;
  float vt=STM_NOVALUE, vdd=STM_NOVALUE, vf=STM_NOVALUE, vth=STM_NOVALUE;

  if (func==NULL || TTV_BASE_LOFIG==&editmodelofig) return ;
  if (tvl==NULL || CheckFirstCallForThisModel()) { APIFreeCallFunc(func); return ;}

  nt=ttv_getarcdef(tvl);
  
  stm_addfctmodel (current_ttvfig->INSNAME, nt, "?", nt, func);
  
  tmodel=stm_getmodel(current_ttvfig->INSNAME, nt);

  stm_mod_setcaracslew (tmodel, 0.2 * slope);
  stm_mod_setcaracslew (tmodel, 1.8 * slope);
  // TODO get the real value of the capa
  capa=tvl->ROOT->ROOT->CAPA; // en Ff
  stm_mod_setcaracload (tmodel, 0.2 * capa);
  stm_mod_setcaracload (tmodel, 1.8 * capa);

  
  // vdd/2, vdd, vt, vfinal
  if (nt[strlen(nt)-1]=='d') vf=0.0;
  else if (ttv_high!=-1) vf=ttv_high-ttv_low;

  if (ttv_high!=-1)
    {
      vdd=ttv_high-ttv_low;
      vt=vdd*0.2;
      vth=vdd*V_FLOAT_TAB[__SIM_VTH].VALUE;
    }
  stm_mod_update(tmodel, vth, vdd, vt, vf);

  if ((tvl->TYPE & (TTV_LINE_O|TTV_LINE_U))==0
      || (tvl->TYPE & TTV_LINE_A)==TTV_LINE_A)
    ttv_addcaracline(tvl, nt, nt, nt, nt);
  else
    ttv_addcaracline(tvl, nt, nt, NULL, NULL);
  hli=gethierlofiginfo(TTV_BASE_LOFIG);  
  ai=getarcinfo(hli, nt);
  ai->MODEL=(APICallFunc *)func;
}


void ttv_Associate_Sim(ttvline_list *tvl, void *func)
{
  char *nt;
  HierLofigInfo *hli;
  ArcInfo *ai;

  if (func==NULL || TTV_BASE_LOFIG==&editmodelofig) return ;
  if (tvl==NULL || CheckFirstCallForThisModel()) {APIFreeCallFunc(func); return ;}

  nt=ttv_getarcdef(tvl);
  hli=gethierlofiginfo(TTV_BASE_LOFIG);  
  ai=getarcinfo(hli, nt);
  ai->SIM=(APICallFunc *)func;
}

void ttv_Associate_Env(ttvline_list *tvl, void *func)
{
  char *nt;
  HierLofigInfo *hli;
  ArcInfo *ai;

  if (func==NULL || TTV_BASE_LOFIG==&editmodelofig) return ;
  if (tvl==NULL || CheckFirstCallForThisModel()) {APIFreeCallFunc(func); return ;}

  nt=ttv_getarcdef(tvl);

  hli=gethierlofiginfo(TTV_BASE_LOFIG);  
  ai=getarcinfo(hli, nt);
  ai->ENV=(APICallFunc *)func;
}

void ttv_Associate_Ctk_Env(ttvline_list *tvl, void *func)
{
  char *nt;
  HierLofigInfo *hli;
  ArcInfo *ai;

  if (func==NULL || TTV_BASE_LOFIG==&editmodelofig) return ;
  if (tvl==NULL || CheckFirstCallForThisModel()) return ;

  nt=ttv_getarcdef(tvl);

  hli=gethierlofiginfo(TTV_BASE_LOFIG);  
  ai=getarcinfo(hli, nt);
  ai->CTK_ENV=(APICallFunc *)func;
}

ttvline_list *ttv_AddTiming(char *input, char *output, double max_delay, double max_slope, double min_delay, double min_slope, char *dir)
{
  char it, ot;
  char *sin, *sout;
  ttvsig_list *tsin, *tsout;
  long linetype=TTV_LINE_F;
  float capa0, capa1;

  if (CheckFirstCallForThisModel()) return NULL;

  if ((sin=getsignal(input, &it, &capa0))==NULL) return NULL;
  if ((sout=getsignal(output, &ot, &capa1))==NULL) return NULL;

  checkslope("min", input, output, dir, &min_slope);
  checkslope("max", input, output, dir, &max_slope);

  tsin=ttvapi_GetNode(input, sin, capa0);
  SetNodeType(tsin, it);
  tsout=ttvapi_GetNode(output, sout, capa1);
  SetNodeType(tsout, ot);

 if (((tsin->TYPE|tsout->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
    linetype=TTV_LINE_D;

  return ttv_AddCustomLine(tsin, tsout, max_delay, max_slope, min_delay, min_slope, dir, linetype);
}
ttvline_list *ttv_AddHZTiming(char *input, char *output, double max_delay, double min_delay, char *dir)
{
  char it, ot;
  char *sin, *sout;
  ttvsig_list *tsin, *tsout;
  long linetype=TTV_LINE_F|TTV_LINE_HZ;
  float capa0, capa1;

  if (CheckFirstCallForThisModel()) return NULL;

  if ((sin=getsignal(input, &it, &capa0))==NULL) return NULL;
  if ((sout=getsignal(output, &ot, &capa1))==NULL) return NULL;

  tsin=ttvapi_GetNode(input, sin, capa0);
  SetNodeType(tsin, it);
  tsout=ttvapi_GetNode(output, sout, capa1);
  SetNodeType(tsout, ot);

 if (((tsin->TYPE|tsout->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
    linetype=TTV_LINE_D|TTV_LINE_HZ;

  return ttv_AddCustomLine(tsin, tsout, max_delay, 0, min_delay, 0, dir, linetype);
}

ttvline_list *ttv_AddSetup(char *input, char *output, double delay, char *dir)
{
  char it, ot;
  char *sin, *sout;
  ttvsig_list *tsin, *tsout;
  long linetype=TTV_LINE_E;
  float capa0, capa1;

  if (CheckFirstCallForThisModel()) return NULL;

  if ((sin=getsignal(input, &it, &capa0))==NULL) return NULL;
  if ((sout=getsignal(output, &ot, &capa1))==NULL) return NULL;

  tsin=ttvapi_GetNode(input, sin, capa0);
  SetNodeType(tsin, it);
  tsout=ttvapi_GetNode(output, sout, capa1);
  SetNodeType(tsout, ot);

  if (((tsin->TYPE|tsout->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
    linetype=TTV_LINE_D;    

  return ttv_AddCustomLine(tsin, tsout, delay, 0, delay, 0, dir, linetype|TTV_LINE_U);
}

ttvline_list *ttv_AddHold(char *input, char *output, double delay, char *dir)
{
  char it, ot;
  char *sin, *sout;
  ttvsig_list *tsin, *tsout;
  long linetype=TTV_LINE_E;
  float capa0, capa1;

  if (CheckFirstCallForThisModel()) return NULL;

  if ((sin=getsignal(input, &it, &capa0))==NULL) return NULL;
  if ((sout=getsignal(output, &ot, &capa1))==NULL) return NULL;

  tsin=ttvapi_GetNode(input, sin, capa0);
  SetNodeType(tsin, it);
  tsout=ttvapi_GetNode(output, sout, capa1);
  SetNodeType(tsout, ot);

  if (((tsin->TYPE|tsout->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
    linetype=TTV_LINE_D;

  return ttv_AddCustomLine(tsin, tsout, delay, 0, delay, 0, dir, linetype|TTV_LINE_O);
}


ttvline_list *ttv_AddAccess(char *input, char *output, double max_delay, double max_slope, double min_delay, double min_slope, char *dir)
{
  char it, ot;
  char *sin, *sout;
  ttvsig_list *tsin, *tsout;
  ttvline_list *tl;
  long linetype=TTV_LINE_E;
  float capa0, capa1;
  char dir0[3]="  ";

  if (CheckFirstCallForThisModel()) return NULL;

  if ((sin=getsignal(input, &it, &capa0))==NULL) return NULL;
  if ((sout=getsignal(output, &ot, &capa1))==NULL) return NULL;

  tsin=ttvapi_GetNode(input, sin, capa0);
  SetNodeType(tsin, it);
  tsout=ttvapi_GetNode(output, sout, capa1);
  SetNodeType(tsout, ot);

  if (((tsin->TYPE|tsout->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
    linetype=TTV_LINE_D;

  tl=ttv_AddCustomLine(tsin, tsout, max_delay, max_slope, min_delay, min_slope, dir, linetype|TTV_LINE_A);
  dir0[0]=*dir;
  dir0[1]=*dir;
  ttv_SetLineCommand(tl, input, input, dir0);
  return tl;        
}

ttvline_list *ttv_AddHZAccess(char *input, char *output, double max_delay, double min_delay, char *dir)
{
  char it, ot;
  char *sin, *sout;
  ttvsig_list *tsin, *tsout;
  long linetype=TTV_LINE_E;
  float capa0, capa1;
  ttvline_list *tl;
  char dir0[3]="  ";

  if (CheckFirstCallForThisModel()) return NULL;

  if ((sin=getsignal(input, &it, &capa0))==NULL) return NULL;
  if ((sout=getsignal(output, &ot, &capa1))==NULL) return NULL;

  tsin=ttvapi_GetNode(input, sin, capa0);
  SetNodeType(tsin, it);
  tsout=ttvapi_GetNode(output, sout, capa1);
  SetNodeType(tsout, ot);

  if (((tsin->TYPE|tsout->TYPE) & (TTV_SIG_C|TTV_SIG_N))!=0)
    linetype=TTV_LINE_D|TTV_LINE_HZ;

  tl=ttv_AddCustomLine(tsin, tsout, max_delay, 0, min_delay, 0, dir, linetype|TTV_LINE_A);
  dir0[0]=*dir;
  dir0[1]=*dir;
  ttv_SetLineCommand(tl, input, input, dir0);
  return tl;
}

void ShareStmFCTModels()
{
  chain_list *cl;
  ttvline_list *tvl;
  HierLofigInfo *hli;
  ArcInfo *ai;
  char *nt, *key, *nt0, *nt1, *nt2, *nt3;
  ht *uniquemodels;
  NameAllocator na;
  char trans, temp[4096];
  long l;
  int cnt=0, total=0;
  timing_model *tmodel;

  CreateNameAllocator(1024, &na, CASE_SENSITIVE);
  uniquemodels=addht(1024);
  hli=gethierlofiginfo(TTV_BASE_LOFIG);

  for (cl=ALL_LINES, cnt=0; cl!=NULL; cl=cl->NEXT)
    {
      tvl=(ttvline_list *)cl->DATA;
      nt=ttv_getarcdef(tvl);
      ai=getarcinfo(hli, nt);

      strcpy(temp,"");
      if (ai->MODEL!=NULL)
        { APIPrintCallFunc(ai->MODEL, &temp[strlen(temp)]); strcat(temp,"|"); }
      if (ai->SIM!=NULL)
        { APIPrintCallFunc(ai->SIM, &temp[strlen(temp)]); strcat(temp,"|"); }
      if (ai->ENV!=NULL)
        { APIPrintCallFunc(ai->ENV, &temp[strlen(temp)]); strcat(temp,"|"); }
      if (ai->CTK_ENV!=NULL)
        { APIPrintCallFunc(ai->CTK_ENV, &temp[strlen(temp)]); strcat(temp,"|"); }

      if (strlen(temp)!=0)
        {
          key=NameAlloc(&na, temp);
          l=gethtitem(uniquemodels, key);
          if (l!=EMPTYHT)
            {
              cnt++;
              if ((long)nt!=l)
                {
                  stm_freemodel(current_ttvfig->INSNAME, nt);
                  nt=(char *)l;
                  if ((tvl->TYPE & (TTV_LINE_O|TTV_LINE_U))==0
                      || (tvl->TYPE & TTV_LINE_A)==TTV_LINE_A)
                    ttv_addcaracline(tvl, nt, nt, nt, nt);
                  else
                    ttv_addcaracline(tvl, nt, nt, NULL, NULL);
                }
            }
          else
            addhtitem(uniquemodels, key, (long)nt);
        }
      else
        {
          if (tvl->ROOT==&tvl->ROOT->ROOT->NODE[0]) trans='d'; else trans='u';
          sprintf(temp,"cst_%.1fps_%c", tvl->VALMAX/TTV_UNIT, trans); nt0=namealloc(temp);
          stm_addconstmodel(current_ttvfig->INSNAME, nt0, tvl->VALMAX/TTV_UNIT);

          sprintf(temp,"cst_%.1fps_%c", tvl->VALMIN/TTV_UNIT, trans); nt2=namealloc(temp);
          stm_addconstmodel(current_ttvfig->INSNAME, nt2, tvl->VALMIN/TTV_UNIT);

          if (trans=='d')
            {
              tmodel=stm_getmodel(current_ttvfig->INSNAME, nt0);
              stm_mod_update(tmodel, STM_NOVALUE, STM_NOVALUE, STM_NOVALUE, 0.0);
              tmodel=stm_getmodel(current_ttvfig->INSNAME, nt2);
              stm_mod_update(tmodel, STM_NOVALUE, STM_NOVALUE, STM_NOVALUE, 0.0);
            }

          if (!((tvl->TYPE & (TTV_LINE_O|TTV_LINE_U))==0
                || (tvl->TYPE & TTV_LINE_A)==TTV_LINE_A))
            {
              ttv_addcaracline(tvl, nt0, nt2, NULL, NULL);
            }
          else
            {
              sprintf(temp,"cst_%.1fps_%c", tvl->FMAX/TTV_UNIT, trans); nt1=namealloc(temp);
              stm_addconstmodel(current_ttvfig->INSNAME, nt1, tvl->FMAX/TTV_UNIT);
              sprintf(temp,"cst_%.1fps_%c", tvl->FMIN/TTV_UNIT, trans); nt3=namealloc(temp);
              stm_addconstmodel(current_ttvfig->INSNAME, nt3, tvl->FMIN/TTV_UNIT);

              if (trans=='d')
                {
                  tmodel=stm_getmodel(current_ttvfig->INSNAME, nt1);
                  stm_mod_update(tmodel, STM_NOVALUE, STM_NOVALUE, STM_NOVALUE, 0.0);
                  tmodel=stm_getmodel(current_ttvfig->INSNAME, nt3);
                  stm_mod_update(tmodel, STM_NOVALUE, STM_NOVALUE, STM_NOVALUE, 0.0);
                }

              ttv_addcaracline(tvl, nt0, nt2, nt1, nt3);
            }
        }
      total++;
    }
  
  if (cnt!=0)
    avt_fprintf(stdout,"[" AVT_MAGENTA "info" AVT_RESET "] %d/%d timing lines share their model\n",cnt,total);
  DeleteNameAllocator(&na);
  delht(uniquemodels);
  freechain(ALL_LINES);
}

static void ChangeConDir(ttvsig_list *tvs)
{
  ptype_list *pt;
  long type, val;

  if ((pt=getptype(tvs->USER, TTV_TEMP_PTYPE))!=NULL)
  {
    type=TTV_SIG_CX;
    val=(long)pt->DATA;
    if ((val & (TTV_mark_IN|TTV_mark_OUT))==(TTV_mark_IN|TTV_mark_OUT))
    {
      if (val & TTV_mark_HZ) type=TTV_SIG_CT;
      else type=TTV_SIG_CB;
    }
    else if ((val & TTV_mark_OUT)==TTV_mark_OUT)
    {
      if (val & TTV_mark_HZ) type=TTV_SIG_CZ;
      else type=TTV_SIG_CO;
    }
    else if ((val & TTV_mark_IN)==TTV_mark_IN)
    {
      type=TTV_SIG_CI;
    }
    tvs->TYPE&=~TTV_SIG_CX;
    tvs->TYPE|=type;
    tvs->USER=delptype(tvs->USER, TTV_TEMP_PTYPE);
  }

}
static void DispatchNodes()
{
  chain_list *cl;
  ttvsig_list *tvs;

  freechain((chain_list *)current_ttvfig->NCSIG); current_ttvfig->NCSIG=NULL;
  freechain((chain_list *)current_ttvfig->ELCMDSIG); current_ttvfig->ELCMDSIG=NULL;
  freechain((chain_list *)current_ttvfig->ELATCHSIG); current_ttvfig->ELATCHSIG=NULL;
  freechain((chain_list *)current_ttvfig->EPRESIG); current_ttvfig->EPRESIG=NULL;
  freechain((chain_list *)current_ttvfig->EBREAKSIG); current_ttvfig->EBREAKSIG=NULL;
  freechain((chain_list *)current_ttvfig->ESIG); current_ttvfig->ESIG=NULL;
  freechain((chain_list *)current_ttvfig->CONSIG); current_ttvfig->CONSIG=NULL;

  for (cl=ALL_ADDED_NODES; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if ((tvs->TYPE & TTV_SIG_C)==TTV_SIG_C)
       {
        ChangeConDir(tvs);
        current_ttvfig->CONSIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->CONSIG, tvs);
       }
      if ((tvs->TYPE & TTV_SIG_N)==TTV_SIG_N)
        current_ttvfig->NCSIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->NCSIG, tvs);
      if ((tvs->TYPE & TTV_SIG_Q)==TTV_SIG_Q)
        current_ttvfig->ELCMDSIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->ELCMDSIG, tvs);
      if ((tvs->TYPE & TTV_SIG_L)==TTV_SIG_L)
        current_ttvfig->ELATCHSIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->ELATCHSIG, tvs);
      if ((tvs->TYPE & TTV_SIG_R)==TTV_SIG_R)
        current_ttvfig->EPRESIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->EPRESIG, tvs);
      if ((tvs->TYPE & TTV_SIG_B)==TTV_SIG_B)
        current_ttvfig->EBREAKSIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->EBREAKSIG, tvs);
      if ((tvs->TYPE & TTV_SIG_S)==TTV_SIG_S)
        current_ttvfig->ESIG=(ttvsig_list **)addchain((chain_list *)current_ttvfig->ESIG, tvs);
    }
  freechain(ALL_ADDED_NODES);
}

void ttv_API_Action_Terminate() // commentaire pour desactiver l'ajout de token
{
  int delete;
  int editmode=0;
  long loaded, filetype;
  
  HierLofigInfo *hli;

  IFGNS (  
         if (CALLED_TTV_FUNCTION==0)
         {
           hli=gethierlofiginfo(CUR_HIER_LOFIG);
           if (hli->BUILD_TTV==NULL) return;
           else 
             if (CheckFirstCallForThisModel()) return;
         }  
         
         ShareStmFCTModels();
        )

  if (TTV_BASE_LOFIG==NULL) return;

  if (TTV_BASE_LOFIG==&editmodelofig) editmode=1;

  DispatchNodes();

  filetype=TTV_FILE_DTX;
  if (editmode)
  {
    loaded=ttv_getloadedfigtypes(current_ttvfig);          
    if ((loaded & TTV_FILE_TTX)==TTV_FILE_TTX) filetype=TTV_FILE_TTX;
  }
  
  current_ttvfig->STATUS |= TTV_STS_DTX ;

  delete = tas_deleteflatemptyfig(current_ttvfig, filetype, 'Y');

  tas_detectloop(current_ttvfig, filetype);
  
  ttv_builtrefsig(current_ttvfig);
  
  if(delete != 0)
    {
      ttv_setttvlevel(current_ttvfig) ;
      ttv_setsigttvfiglevel(current_ttvfig) ;
    }
    
  if (!editmode)
  {
    ttv_setttvdate (current_ttvfig, TTV_DATE_LOCAL);
    current_ttvfig->INFO->TOOLNAME = namealloc ("ttv_API");
    current_ttvfig->INFO->TOOLVERSION = namealloc ("1.0");
    current_ttvfig->INFO->TECHNONAME = namealloc ("spice");
    current_ttvfig->INFO->TECHNOVERSION = namealloc ("1.0");
    current_ttvfig->INFO->SLOPE = (long)(TTVSLOPE*1e12*TTV_UNIT);
    current_ttvfig->INFO->CAPAOUT = (long)(TTVCAPA*1e12);
    current_ttvfig->INFO->STHHIGH = V_FLOAT_TAB[__SIM_VTH_HIGH].VALUE;
    current_ttvfig->INFO->STHLOW = V_FLOAT_TAB[__SIM_VTH_LOW].VALUE;
    current_ttvfig->INFO->DTH = V_FLOAT_TAB[__SIM_VTH].VALUE;
    current_ttvfig->INFO->TEMP = V_FLOAT_TAB[__SIM_TEMP].VALUE;
    current_ttvfig->INFO->VDD = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;

    if (TTVFLAGS & TTV_DRIVE_DTX)
      {
        ttv_drittv (current_ttvfig, filetype, (long)0, NULL);
        stm_drivecell(current_ttvfig->INSNAME);
      }

    if ((TTVFLAGS & TTV_INSTANCE_MODE)==0 && TTV_FORCED_NAME==NULL)
      TTV_BASE_LOFIG->USER=addptype(TTV_BASE_LOFIG->USER, TTV_DONE_PTYPE, current_ttvfig);

    current_dup_lofig->LOINS=(loins_list *)append((chain_list *)current_dup_lofig->LOINS, (chain_list *)loins_removed);
    current_dup_lofig->LOSIG=(losig_list *)append((chain_list *)current_dup_lofig->LOSIG, (chain_list *)losig_removed);
  }
  
  CALLED_TTV_FUNCTION=0;
  TTV_BASE_LOFIG=NULL;
  TTV_FUNCTIONS_FILE = NULL;
  ALL_ADDED_NODES=NULL;
  current_dup_lofig=NULL;
  TTV_FORCED_NAME=NULL;
  delht(nodeht);

  IFGNS( ALL_MY_TTV=addchain(ALL_MY_TTV, current_ttvfig); )

  tas_restore();
  //avt_restorecontext ();

}

ttvfig_list *ttv_FinishTimingFigure()
{
  ttvfig_list *tvf;
  if (CheckFirstCallForThisModel()) return NULL;
  tvf=current_ttvfig;
  ttv_API_Action_Terminate();
  return tvf;
}

void ttv_DriveTimingFigure(ttvfig_list *tvf, char *filename, char *format)
{
  if (tvf==NULL) return;
  ttv_disablecache(tvf);
  if (strcasecmp(format,"dtx")==0)
    ttv_drittv (tvf, TTV_FILE_DTX, (long)0, filename);
  else if (strcasecmp(format,"ttx")==0)
    ttv_drittv (tvf, TTV_FILE_TTX, (long)0, filename);
  else
   {
     avt_errmsg(TTV_API_ERRMSG, "010", AVT_ERROR, format, filename);
//    avt_error("ttvapi", 4, AVT_ERR, "unknown file format '%s' for timing file '%s'\n", format, filename);
    return;
   }
   
  stm_drivecell(tvf->INFO->FIGNAME);
}
void ttv_BuildTiming()
{
  if (CheckFirstCallForThisModel()) return;
  ttv_API_Action_Terminate();
}

void ttv_TasBuiltTiming(lofig_list *lf)
{
  cnsfig_list *cnsf;
  ttvfig_list *tvf;
  
  elpLoadOnceElp();

  //avt_savecontext();
  tas_setenv();
  yagenv(tas_yaginit);

  YAG_CONTEXT->YAG_GENIUS=FALSE;
  cnsf=yagDisassemble(lf->NAME, lf, 0);

  TAS_CONTEXT->TAS_LOFIG=lf;
  TAS_CONTEXT->TAS_FIND_MIN = 'Y';
  tas_TechnoParameters();


  tas_capasig(lf);
  elpLofigAddCapas ( lf, elpTYPICAL );
  
  tas_detectinout (cnsf);

  tas_initcnsfigalloc (cnsf);
  tvf=tas_builtfig(lf);
  tas_loconorient(lf,tvf);
  tas_setsigname(lf);
  tas_timing (cnsf, lf, tvf);
  tvf=tas_cns2ttv(cnsf, tvf, lf);
  ttv_setttvdate (tvf, TTV_DATE_LOCAL);
  tvf->INFO->TOOLNAME = namealloc ("ttv_API_subtas");
  tvf->INFO->TOOLVERSION = namealloc ("1.0");
  tvf->INFO->TECHNONAME = namealloc ("tas");
  tvf->INFO->TECHNOVERSION = namealloc ("1.0");
  tvf->INFO->SLOPE = (long)(TTVSLOPE*1e12);
  tvf->INFO->CAPAOUT = (long)(TTVCAPA*1e12);

  if (TTVFLAGS & TTV_DRIVE_DTX)
    {
      ttv_drittv (tvf, TTV_FILE_DTX, (long)0, NULL);
      stm_drivecell(tvf->INSNAME);
    }
  CNS_HEADCNSFIG = delcnsfig(CNS_HEADCNSFIG, cnsf);

  ALL_MY_TTV=addchain(ALL_MY_TTV, tvf);
  yagrestore();
  tas_restore();
  //avt_restorecontext();
}

void ttv_API_TopLevel(long list) // commentaire pour desactiver l'ajout de token
{
  chain_list *cl=(chain_list *)list;
  chain_list *ch;
  ttvfig_list *tvf;
  timing_cell *cell;
  locon_list *lc;
  loins_list *li;
  ht *myttvfigs;
  int dir;
  
  if ((TTVFLAGS & TTV_INSTANCE_MODE)==0)
    {
      myttvfigs=addht(1024);
      for (ch=ALL_MY_TTV; ch!=NULL; ch=ch->NEXT)
        {
          tvf=(ttvfig_list *)ch->DATA;
          addhtitem(myttvfigs, tvf->INSNAME, (long)tvf);
        }
    }

  for (cl=(chain_list *)list; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      if ((TTVFLAGS & TTV_INSTANCE_MODE)!=0 || gethtitem(myttvfigs, li->INSNAME)!=EMPTYHT)
        {
          li->FIGNAME=li->INSNAME;
        }
    }

  if ((TTVFLAGS & TTV_INSTANCE_MODE)==0)
    {
      delht(myttvfigs);
    }

  for (ch=ALL_MY_TTV; ch!=NULL; ch=ch->NEXT)
    {
      tvf=(ttvfig_list *)ch->DATA;

      for (cl=(chain_list *)list; cl!=NULL; cl=cl->NEXT)
        {
          if (((loins_list *)cl->DATA)->FIGNAME==tvf->INSNAME) break;
        }
      if (cl==NULL)
        {
          ttv_unlockttvfig(tvf);
          if((cell = stm_getcell(tvf->INFO->FIGNAME)) != NULL){
            stm_freecell(tvf->INFO->FIGNAME) ;
            cell = NULL;
          }
          ttv_freeallttvfig(tvf);
        }
      else
        {
          for (cl=(chain_list *)list; cl!=NULL; cl=cl->NEXT)
            {
              li=(loins_list *)cl->DATA;
              for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
                {
                  dir=mbk_rc_are_internal(li, lc);
                  if (dir==1)
                    {
                      if (getptype(lc->SIG->USER, TAS_SIG_NORCDELAY)==NULL)
                        lc->SIG->USER=addptype(lc->SIG->USER, TAS_SIG_NORCDELAY, NULL);
                    }
                }
            }
        }
    }

  freechain(ALL_MY_TTV);
  ALL_MY_TTV=NULL;
}

int ttv_MinDelay()
{
  int state;
  if (TTV_COM==NULL) return 0;
  if (TTV_COM(TTV_getdelaytype, &state)) return 0;
  if (state==TTV_getdelaytype_min) return 1;
  return 0;
}

int ttv_MaxDelay()
{
  int state;
  if (TTV_COM==NULL) return 0;
  if (TTV_COM(TTV_getdelaytype, &state)) return 0;
  if (state==TTV_getdelaytype_max) return 1;
  return 0;
}


void ASSOCIATE_TIMING(void *func)
{
  APICallFunc *cf=(APICallFunc *)func;
  t_arg *ret;
  HierLofigInfo *hli=gethierlofiginfo(CUR_HIER_LOFIG);
  
  if (TTVFLAGS & TTV_INSTANCE_MODE || hli->BUILD_TTV==NULL)
    {
      // ajout a la lofig hierarchique de l'info
      hli->BUILD_TTV=cf;
      // 
      if (APIExecAPICallFunc(APIGetExecutionContext(), cf, &ret, 0)) EXIT(1);
      if (ret!=NULL)
        {
          APIFreeTARG(ret);
        }
    }
  else
    {
      APIFreeTARGS(cf->ARGS);
      mbkfree(cf);
    }
}

void ttv_SetLineCommand(ttvline_list *tl, char *max_command, char *min_command, char *dir)
{
  ttvsig_list *tvs;
  int a, b;

  if (tl==NULL) return;

  ttv_dir_to_index(dir, &a, &b);
  if (min_command==NULL) min_command=max_command, b=a;
  if (a==-1 || b==-1) 
    {
      avt_errmsg(TTV_API_ERRMSG, "009", AVT_ERROR, gen_info(), dir);
//      avt_error("ttvapi", 3, AVT_ERR, "%s: invalid direction '%s', timing line command creation ignored\n", gen_info(), dir);
      return;
    }
  if (tl->ROOT->ROOT->TYPE & TTV_SIG_L)
  {
    tvs=ttv_AddCommand(max_command);
    ttv_addcmd(tl, TTV_LINE_CMDMAX, &tvs->NODE[a]) ;
    tvs=ttv_AddCommand(min_command);
    ttv_addcmd(tl, TTV_LINE_CMDMIN, &tvs->NODE[b]) ;
  }
}

void ttv_SetLineModel(ttvline_list *tl, char *modelname, char *where)
{
  int slope, max;
  char *a[2][2];

  if (tl==NULL) return;

  if (strstr(where,"slope")!=NULL) slope=1;
  else slope=0;
  if (strstr(where,"max")!=NULL) max=1;
  else max=0;
  
  a[0][1]=tl->MDMAX;
  a[0][0]=tl->MDMIN;
  a[1][1]=tl->MFMAX;
  a[1][0]=tl->MFMIN;

  a[slope][max]=namealloc(modelname);

  if ((tl->TYPE & (TTV_LINE_O|TTV_LINE_U))==0
      || (tl->TYPE & TTV_LINE_A)==TTV_LINE_A)
    ttv_addcaracline(tl, a[0][1], a[0][0], a[1][1], a[1][0]);
  else
    ttv_addcaracline(tl, a[0][1], a[0][0], NULL, NULL);
}

void ttv_SetTimingFigureName( ttvfig_list *tvf, char *name )
{
  char *aname ;
  if( !tvf || !name ) return ;
  aname = namealloc( name );
  ttv_delhtmodel( tvf );
  if( !stm_renamecell( tvf->INFO->FIGNAME, aname ) ) {
    printf( "can't rename stm model\n" );
  }
  tvf->INFO->FIGNAME = aname ;
  tvf->INSNAME       = aname ;
  ttv_addhtmodel( tvf );
}

void ttv_CreateTimingTableModel(ttvfig_list *tvf, char *name, chain_list *input_slope, chain_list *output_capa, chain_list *values, char *type)
{
  int nbs, nbc, slope, i, j;
  timing_model *model = NULL;
  chain_list *cl, *ch;
  float u1, u2;
  long t1, t2;
  
  IFGNS( if (tvf==NULL) tvf=current_ttvfig; )

  if (tvf==NULL) return;

  nbs=countchain(input_slope);
  nbc=countchain(output_capa);
  if (strcasecmp(type,"slope-slope")==0) u1=u2=1e12, t1=t2=STM_INPUT_SLEW;
  else if (strcasecmp(type,"slope-ckslope")==0) u1=u2=1e12, t1=STM_INPUT_SLEW, t2=STM_CLOCK_SLEW;
  else if (strcasecmp(type,"slope-capa")==0) u1=1e12, u2=1e15, t1=STM_INPUT_SLEW, t2=STM_LOAD;
  else
  {
    avt_errmsg(TTV_API_ERRMSG, "050", AVT_ERROR, type);
    return;
  }

  if ((i=countchain(values))!=nbs)
  {
    avt_errmsg(TTV_API_ERRMSG, "049", AVT_ERROR, i, nbs);
    return;
  }

  for (ch=values; ch!=NULL; ch=ch->NEXT)
    if ((i=countchain((chain_list *)ch->DATA))!=nbc)
    {
       avt_errmsg(TTV_API_ERRMSG, "049", AVT_ERROR, i, nbc);
       return;
    }

  name=namealloc(name);
          
  if (nbs==1)
    {
      if (nbc==1)
        {
          stm_addconstmodel(tvf->INFO->FIGNAME, name, (*(float *)&((chain_list *)values->DATA)->DATA*1e12));
        }
      else
        {
          model = stm_addtblmodel (tvf->INFO->FIGNAME, name, nbc, 0, t2, STM_NOTYPE);
          for (cl=output_capa, i=0; i<nbc; i++, cl=cl->NEXT)
            {
              stm_modtbl_setXrangeval(model->UMODEL.TABLE, i, (*(float *)&cl->DATA)*u2);
            }
          for (ch=values; ch!=NULL; ch=ch->NEXT)
            for (cl=(chain_list *)ch->DATA, i=0; cl!=NULL; cl=cl->NEXT, i++)
              stm_modtbl_set1Dval (model->UMODEL.TABLE, i, (*(float *)&cl->DATA)*1e12);
       }
    }
  else if (nbc==1)
    {
      model = stm_addtblmodel (tvf->INFO->FIGNAME, name, nbs, 0, t1, STM_NOTYPE);
      for (cl=input_slope, i=0; i<nbs; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(model->UMODEL.TABLE, i, (*(float *)&cl->DATA)*u1);
        }
      for (cl=values, i=0; cl!=NULL; cl=cl->NEXT, i++)
        for (ch=(chain_list *)cl->DATA; ch!=NULL; ch=ch->NEXT)
          stm_modtbl_set1Dval (model->UMODEL.TABLE, i, (*(float *)&ch->DATA)*1e12);
    }
  else
    {
      model = stm_addtblmodel (tvf->INFO->FIGNAME, name, nbs, nbc, t1, t2);
      for (cl=input_slope, i=0; i<nbs; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(model->UMODEL.TABLE, i, (*(float *)&cl->DATA)*u1);
        }
      for (cl=output_capa, i=0; i<nbc; i++, cl=cl->NEXT)
        {
          stm_modtbl_setYrangeval(model->UMODEL.TABLE, i, (*(float *)&cl->DATA)*u2);
        }
      for (ch=values, i=0; i<nbs; i++, ch=ch->NEXT)
        for (j=0, cl=(chain_list *)ch->DATA; j<nbc; j++, cl=cl->NEXT)
          {
            stm_modtbl_set2Dval(model->UMODEL.TABLE, i, j, (*(float *)&cl->DATA)*1e12);
          }
    }
}

void ttv_CreateEnergyTableModel(ttvfig_list *tvf, char *name, chain_list *input_slope, chain_list *output_capa, chain_list *values, char *type)
{
  int nbs, nbc, slope, i, j;
  timing_model *model = NULL;
  chain_list *cl, *ch;
  float u1, u2;
  long t1, t2;
  
  IFGNS( if (tvf==NULL) tvf=current_ttvfig; )

  if (tvf==NULL) return;

  nbs=countchain(input_slope);
  nbc=countchain(output_capa);
  if (strcasecmp(type,"slope-slope")==0) u1=u2=1e12, t1=t2=STM_INPUT_SLEW;
  else if (strcasecmp(type,"slope-ckslope")==0) u1=u2=1e12, t1=STM_INPUT_SLEW, t2=STM_CLOCK_SLEW;
  else if (strcasecmp(type,"slope-capa")==0) u1=1e12, u2=1e15, t1=STM_INPUT_SLEW, t2=STM_LOAD;
  else
  {
    avt_errmsg(TTV_API_ERRMSG, "050", AVT_ERROR, type);
    return;
  }

  if ((i=countchain(values))!=nbs)
  {
    avt_errmsg(TTV_API_ERRMSG, "049", AVT_ERROR, i, nbs);
    return;
  }

  for (ch=values; ch!=NULL; ch=ch->NEXT)
    if ((i=countchain((chain_list *)ch->DATA))!=nbc)
    {
       avt_errmsg(TTV_API_ERRMSG, "049", AVT_ERROR, i, nbc);
       return;
    }

  name=namealloc(name);
  
  model=stm_getmodel(tvf->INFO->FIGNAME, name);
 
  if (nbs==1)
    {
      if (nbc==1)
        {
          model->ENERGYTYPE = STM_ENERGY_TABLE;
          model->ENERGYMODEL.ETABLE = stm_modtbl_create(0, 0, STM_NOTYPE, STM_NOTYPE);
          stm_modtbl_setconst (model->ENERGYMODEL.ETABLE, (*(float *)&((chain_list *)values->DATA)->DATA));
        }
      else
        {
          model->ENERGYTYPE = STM_ENERGY_TABLE;
          model->ENERGYMODEL.ETABLE = stm_modtbl_create(nbc, 0, t2, STM_NOTYPE);
          for (cl=output_capa, i=0; i<nbc; i++, cl=cl->NEXT)
            {
              stm_modtbl_setXrangeval(model->ENERGYMODEL.ETABLE, i, (*(float *)&cl->DATA)*u2);
            }
          for (ch=values; ch!=NULL; ch=ch->NEXT)
            for (cl=(chain_list *)ch->DATA, i=0; cl!=NULL; cl=cl->NEXT, i++)
              stm_modtbl_set1Dval (model->ENERGYMODEL.ETABLE, i, (*(float *)&cl->DATA));
       }
    }
  else if (nbc==1)
    {
      model->ENERGYTYPE = STM_ENERGY_TABLE;
      model->ENERGYMODEL.ETABLE = stm_modtbl_create(nbs, 0, t1, STM_NOTYPE);
      for (cl=input_slope, i=0; i<nbs; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(model->ENERGYMODEL.ETABLE, i, (*(float *)&cl->DATA)*u1);
        }
      for (cl=values, i=0; cl!=NULL; cl=cl->NEXT, i++)
        for (ch=(chain_list *)cl->DATA; ch!=NULL; ch=ch->NEXT)
          stm_modtbl_set1Dval (model->ENERGYMODEL.ETABLE, i, (*(float *)&ch->DATA));
    }
  else
    {
      model->ENERGYTYPE = STM_ENERGY_TABLE;
      model->ENERGYMODEL.ETABLE = stm_modtbl_create(nbs, nbc, t1, t2);
      for (cl=input_slope, i=0; i<nbs; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(model->ENERGYMODEL.ETABLE, i, (*(float *)&cl->DATA)*u1);
        }
      for (cl=output_capa, i=0; i<nbc; i++, cl=cl->NEXT)
        {
          stm_modtbl_setYrangeval(model->ENERGYMODEL.ETABLE, i, (*(float *)&cl->DATA)*u2);
        }
      for (ch=values, i=0; i<nbs; i++, ch=ch->NEXT)
        for (j=0, cl=(chain_list *)ch->DATA; j<nbc; j++, cl=cl->NEXT)
          {
            stm_modtbl_set2Dval(model->ENERGYMODEL.ETABLE, i, j, (*(float *)&cl->DATA));
          }
    }
}


chain_list *ttv_CharacTimingLineModel(ttvfig_list *tvf, char *name, chain_list *input_slope, chain_list *output_capa, char *type)
{
  int nbs, nbc, slope, i, j, cst=0;
  timing_model *model = NULL;
  chain_list *cl, *ch, *res, *res1;
  float si, sc, u1, u2, val;
  long t1, t2;
  char *cellname;
  
  IFGNS( if (tvf==NULL) tvf=current_ttvfig; )

  if (tvf==NULL) return NULL;

  nbs=countchain(input_slope);
  nbc=countchain(output_capa);
  if (strcasecmp(type,"slope-slope")==0) u1=u2=1e12, cst=1;
  else if (strcasecmp(type,"slope-capa:delay")==0) u1=u2=1e12, cst=2;
  else if (strcasecmp(type,"slope-capa:slope")==0) u1=1e12, u2=1e15;
  else
  {
    avt_errmsg(TTV_API_ERRMSG, "050", AVT_ERROR, type);
    return NULL;
  }

  name=namealloc(name);
  cellname=tvf->INFO->FIGNAME;

  res=NULL;
  for (ch=input_slope; ch!=NULL; ch=ch->NEXT)
  {
    si=(*(float *)&ch->DATA)*u1;
    res1=NULL;
    for (cl=output_capa; cl!=NULL; cl=cl->NEXT)
    {
      sc=(*(float *)&cl->DATA)*u2;
      if (cst==1)
        val=stm_getconstraint(cellname, name, si, sc);
      else if (cst==2)
        val=stm_getdelay(cellname, name, sc, si, NULL, NULL);
      else
        val=stm_getslew(cellname, name, sc, si, NULL, NULL, NULL);
      res1=addchain(res1, NULL);
      *(float *)&res1->DATA=val*1e-12;
    }
    res=addchain(res, reverse(res1));
  }
  res=reverse(res);
  return res;
}

int ttv_LoadSSTAResults(ttvfig_list *tvf, chain_list *filenames, chain_list *order)
{
  chain_list *cl;
  if (tvf==NULL) return 0;
  for (cl=order; cl!=NULL; cl=cl->NEXT)
  {
     if (countchain((chain_list *)cl->DATA)!=2)
     {
       avt_errmsg(TTV_API_ERRMSG, "053", AVT_ERROR);
       return 0;
     }
  }
  return ttv_ssta_loadresults(tvf, filenames, order);
}

int ttv_SetSSTARunNumber(ttvfig_list *tvf, int num)
{
  return ttv_ssta_set_run_number(tvf, num-1);
}

int ttv_BuildSSTALineStats(ttvfig_list *tvf)
{
  return ttv_ssta_buildlinestat(tvf);
}
