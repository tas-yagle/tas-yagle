
/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_critic.c                                                */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* recherche des chemins critique dans une ttvfig                           */
/****************************************************************************/

#define SPEED_UP_ACCESS_ALL
//#define SPEED_UP_ACCESS_CRITIC  //not done yet

#include "ttv.h"
#include "ttv_ssta.h"

// defines internes
#define SAVE_ALL_LATCH_TAG 1
#define SAVE_ALL_INPUT_TAG 2
#define SAVE_ALL_TAG (SAVE_ALL_LATCH_TAG|SAVE_ALL_INPUT_TAG)
// --------
#define TTV_LOW_RATIO 0.8
#define TTV_HIGH_RATIO 1.2
// --------
chain_list  *TTV_FIFO_HEAD = NULL ;
chain_list  *TTV_FIFO_UP = NULL ;
chain_list  *TTV_FIFO_B = NULL ;
chain_list  *TTV_FIFO_SAV = NULL ;
char TTV_FIFO_NUM = (char)0 ;
chain_list  *TTV_CHAIN_CLOCK = NULL ;
static chain_list  *TTV_CHAIN_FREE = NULL ;
static char  TTV_FIND_ALLPATH = 'N' ;
static char  TTV_FIND_BUILD = 'Y' ;
static long  TTV_EXCLUDE_TYPE_START=0, TTV_EXCLUDE_TYPE_END=0;
static chain_list *MARKED_SIG_LIST=NULL;
chain_list *TTV_EXPLICIT_CLOCK_NODES=NULL, *TTV_EXPLICIT_START_NODES=NULL, *TTV_EXPLICIT_END_NODES=NULL;
int TTV_MARK_MODE=0, TTV_QUIET_MODE=1;
static int TTV_SEARCH_INFORMATIONS;
static chain_list *TTV_CHARAC_CRITIC_SLOPES_LIST=NULL, *TTV_CHARAC_CRITIC_CAPAS_LIST=NULL;
static chain_list *TTV_CHARAC_CRITIC_RESULT_DELAYS=NULL, *TTV_CHARAC_CRITIC_RESULT_SLOPES=NULL, *TTV_CHARAC_CRITIC_RESULT_ENERGY=NULL;
static ttvevent_list *TTV_SEARCH_FIRST_NODE;
static chain_list *TTV_CRITIC_PARA_SEARCH=NULL;

long TTV_MORE_SEARCH_OPTIONS=0;

#define TTV_SEARCH_STEP_IN_CLOCK_PATH 1
static long TTV_SEARCH_STEP=0;

void ttv_allocpathmodel_propagate (ttvfig_list *ttvfig, ttvfig_list *insfig, ttvevent_list *node, ttvevent_list *root, ttvpath_list *path, long type);
        
typedef struct
{
  ttvpath_list *paths ;
  ttvpath_list *before_paths ;
} LOCAL_SORT_INFO;

static LOCAL_SORT_INFO inner_sort_info, upper_sort_info;

void ttv_set_ttv_getpath_clock_list(chain_list *lst)
{
  TTV_CHAIN_CLOCK=lst;
}

void ttv_reset_search_informations()
{
  TTV_SEARCH_INFORMATIONS=0;
}

long ttv_get_search_informations()
{
  return TTV_SEARCH_INFORMATIONS;
}

void ttv_search_mode(int set, long mask)
{
  if (set)
    TTV_MORE_SEARCH_OPTIONS|=mask;
  else
    TTV_MORE_SEARCH_OPTIONS&=~mask;
}

void ttv_activate_multi_cmd_critic_mode(int val)
{
  if (val)
    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS;
  else
    TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS;
}

void ttv_activate_multi_end_cmd_critic_mode(int val)
{
  if (val)
    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_DIFF_ENDING_CRITIC_COMMAND;
  else
    TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_DIFF_ENDING_CRITIC_COMMAND;
}

void ttv_activate_real_access_mode(int val)
{
  if (val)
    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_REAL_ACCESS;
  else
    TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_REAL_ACCESS;
}

void ttv_activate_path_and_access_mode(int val)
{
  if (val)
    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_PATH_AND_ACCESS;
  else
    TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_PATH_AND_ACCESS;
}

void ttv_activate_simple_critic(int val)
{
  if (val)
    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_SIMPLE_CRITIC;
  else
    TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_SIMPLE_CRITIC;
}

void ttv_activate_search_charac_mode(chain_list *slopes_in, chain_list *capas_in)
{
  TTV_CHARAC_CRITIC_SLOPES_LIST=slopes_in;
  TTV_CHARAC_CRITIC_CAPAS_LIST=capas_in;
  TTV_CHARAC_CRITIC_RESULT_DELAYS=NULL;
}

void ttv_retreive_search_charac_results(chain_list **delays, chain_list **slopes, chain_list **energy)
{
  *delays=TTV_CHARAC_CRITIC_RESULT_DELAYS;
  *slopes=TTV_CHARAC_CRITIC_RESULT_SLOPES;
  *energy=TTV_CHARAC_CRITIC_RESULT_ENERGY;
  TTV_CHARAC_CRITIC_SLOPES_LIST=NULL;
  TTV_CHARAC_CRITIC_CAPAS_LIST=NULL;
  TTV_CHARAC_CRITIC_RESULT_DELAYS=NULL;
}

/* fonction qui test L<O<=R en modulo */
static inline int greater_lowerequal(char L, char O, char R)
{
  if (R>L) return L<O && O<=R;
  return O>L || O<=R;
}

/* fonction qui test L>O>=R en modulo */
static inline int lower_greaterequal(char L, char O, char R)
{
  if (R<L) return R<=O && O<L;
  return O<L || O>=R;
}

void ttv_set_search_exclude_type(long typestart, long typeend, long *oldstart, long *oldend)
{
  *oldstart=TTV_EXCLUDE_TYPE_START;
  *oldend=TTV_EXCLUDE_TYPE_END;
  if (typestart!=-1) TTV_EXCLUDE_TYPE_START=typestart;
  if (typeend!=-1) TTV_EXCLUDE_TYPE_END=typeend;
}

static chain_list *namefromsig(ttvfig_list *tvf, chain_list *sigs)
{
  chain_list *cl=NULL;
  char buf[1024];
  while (sigs!=NULL)
    {
      ttv_getsigname(tvf,buf,(ttvsig_list *)sigs->DATA);
      cl=addchain(cl, namealloc(buf));
      sigs=sigs->NEXT;
    }
  return cl;
}

int ttv_must_be_excluded(ttvsig_list *tvs, long excludeflag, int exclude_not_clockcommand)
{
  if (exclude_not_clockcommand && (tvs->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q))==TTV_SIG_Q && getptype(tvs->USER, TTV_SIG_CLOCK)==NULL)
    return 1;
  if (excludeflag==0) return 0;
  if ((tvs->TYPE & excludeflag)!=0)
    {
      if ((tvs->TYPE & TTV_SIG_L)==0) return 1;
      excludeflag&=TTV_SIG_LL|TTV_SIG_LF|TTV_SIG_LR|TTV_SIG_LS|TTV_SIG_LSL;
      excludeflag&=~TTV_SIG_LL;
      if ((tvs->TYPE & excludeflag)!=0) return 1;
      // cas de ttv_sig_l tout seul
      if ((tvs->TYPE & TTV_SIG_LF)!=TTV_SIG_LF
          && (tvs->TYPE & TTV_SIG_LR)!=TTV_SIG_LR 
          && (tvs->TYPE & TTV_SIG_LS)!=TTV_SIG_LS
          && excludeflag==0) return 1; 
    }
  return 0;
}

chain_list *ttv_apply_exclude_flag(chain_list *cl, long excludeflag, int exclude_not_clockcommand)
{        
  chain_list *newchain;
  if (excludeflag==0 && exclude_not_clockcommand==0) return cl;
  newchain=NULL;
  while (cl!=NULL)
    {
      if (!ttv_must_be_excluded((ttvsig_list *)cl->DATA, excludeflag, exclude_not_clockcommand))
        newchain=addchain(newchain, cl->DATA);
      cl=delchain(cl, cl);
    }
  return newchain;
}

static chain_list *ttv_remove_connector_B(chain_list *cl)
{
  chain_list *newchain;
  ttvsig_list *tvs;
  newchain=NULL;
  while (cl!=NULL)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if ((tvs->TYPE & TTV_SIG_CB)!=TTV_SIG_CB ||
          (tvs->TYPE & TTV_SIG_L)==TTV_SIG_L ||
          (tvs->TYPE & TTV_SIG_R)==TTV_SIG_R)
        newchain=addchain(newchain, cl->DATA);
      cl=delchain(cl, cl);
    }
  return newchain;
}
                
static void ttv_clean_critic_para_nodes()
{
  ttvevent_list *tve;
  while (TTV_CRITIC_PARA_SEARCH!=NULL)
  {
    tve=(ttvevent_list *)TTV_CRITIC_PARA_SEARCH->DATA;
    tve->USER=testanddelptype(tve->USER, TTV_NODE_CRITIC_PARA_INFO);
    TTV_CRITIC_PARA_SEARCH=delchain(TTV_CRITIC_PARA_SEARCH, TTV_CRITIC_PARA_SEARCH);
  }
}

/*****************************************************************************/
/*                        function ttv_getinittime()                         */
/* parametres :                                                              */
/*                                                                           */
/* dit ce que doit faire la recherche pour un connecteur                     */
/*****************************************************************************/
long ttv_getinittime(ttvfig_list *ttvfig,ttvevent_list *event, char phaseevent, long type, char phasefilter, char *foundphase)
{
  long          time ;
  int mode=0;

  if (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_USE_DATA_START) mode|=1;
  if (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_USE_CLOCK_START) mode|=2;

  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    stb_getdelayedge(stb_getstbfig(ttvfig),event,phaseevent,&time,NULL, phasefilter, mode, foundphase, NULL) ;
  else
    stb_getdelayedge(stb_getstbfig(ttvfig),event,phaseevent,NULL,&time, phasefilter, mode, NULL, foundphase) ;

  if(time == TTV_NOTIME)
    return((long)0) ;

  return(time) ;
}

//----- fonction pour les marquages en vue de detection des boucles ----------
inline int ttv_isbypassednode(ttvevent_list *ev)
{
  if (ev->FIND==NULL || (ev->FIND->TYPE & TTV_FIND_BYPASS)==0) return 0;
  return 1;
}

inline void ttv_bypassnode(ttvevent_list *ev)
{
  ev->FIND->TYPE|=TTV_FIND_BYPASS;
}

inline void ttv_donotbypassnode(ttvevent_list *ev)
{
  ev->FIND->TYPE&=~TTV_FIND_BYPASS;
}

static int ttv_check_node_on_path(ttvfig_list *ttvfig, ttvevent_list *root, ttvevent_list *node, long type, ttvevent_list *nodetofind)
{
  ttvevent_list *nodex ;
  ptype_list *ptype ;

  nodex = node ;

  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
    {
      if (nodex==nodetofind) return 1;
      if((nodex->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
        {
          nodex = (ttvevent_list *)ptype->DATA ;
          if (nodex==nodetofind) return 1;
        }

      while(nodex->FIND->OUTLINE != NULL)
        {
          nodex = nodex->FIND->OUTLINE->ROOT ;
          if((nodex->FIND->OUTLINE == NULL) && 
             ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
            {
              if (nodex==nodetofind) return 1;
              nodex = (ttvevent_list *)ptype->DATA ;
            }
          if(nodex == root) break ;
          if (nodex==nodetofind) return 1;
        }
    }
  else
    {
      if (nodex==nodetofind) return 1;
      while(nodex->FIND->OUTLINE != NULL)
        {
          nodex = nodex->FIND->OUTLINE->NODE ;
          if(nodex == root) break ;
          if (nodex==nodetofind) return 1;
        }
    }

  return 0 ;
}

/*****************************************************************************/
/*                        function ttv_transfertnodefindinfo()               */
/* parametres :                                                              */
/*                                                                           */
/* propagation de la recherche                                               */
/*****************************************************************************/
#define ADDACCESS 1

#define THRU_FILTER_ACTIVE 1
//#define THRU_FILTER_NOCROSS 2

static int ttv_avoidmaxfilter(ttvevent_list *tve, int filter, char *newval)
{
  int ret=0;
  if ((TTV_IgnoreMaxFilter & 1)!=0 && (tve->ROOT->TYPE & TTV_SIG_L)!=0) ret=1;
  if ((TTV_IgnoreMaxFilter & 2)!=0 && (tve->ROOT->TYPE & TTV_SIG_R)!=0) ret=1;
  if ((TTV_IgnoreMaxFilter & 4)!=0 && filter) ret=1;
  if (tve->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_NOFILTERMAX) ret=1;
  if (ret==1) *newval|=TTV_FINDSTRUCT_FLAGS_NOFILTERMAX;
  return ret;
}

static int ttv_ckgenerated(long min, long max, stbck *ck, long dir, char state)
{
  long ckmin, ckmax;
  if ((dir & TTV_NODE_UP)!=0)
    {
      ckmin=ck->SUPMIN, ckmax=ck->SUPMAX;
      if (state=='d' && ck->SUPMAX<ck->SDNMAX) ckmin+=ck->PERIOD, ckmax+=ck->PERIOD;
    }
  else
    {
      ckmin=ck->SDNMIN, ckmax=ck->SDNMAX;
      if (state=='u' && ck->SDNMAX<ck->SUPMAX) ckmin+=ck->PERIOD, ckmax+=ck->PERIOD;
    }
  if (max<ckmin || min>ckmax) return 0;
  return 1;
}

static ttvfind_stb_stuff *ttv_getstb_stuff(ttvfind_stb_stuff *tss, int phase)
{
  while (tss!=NULL && tss->PHASE!=phase) tss=tss->NEXT;
  return tss;
}
inline int ttv_transfertnodefindinfo(ttvfig,node,root,line,delay,type)
     ttvfig_list *ttvfig ;
     ttvevent_list *root ;
     ttvevent_list *node ;
     ttvline_list *line ;
     long delay ;
     long type ;
{
  ttvevent_list *cmd, *start, *end, *filterclock ;
  stbfig_list *stbfig ;
  stbck origedge;
  long crossdelay=TTV_NOTIME, crossdelayend=TTV_NOTIME, mindelay, latchminslope, startmin, startmax, endmax, endmin;
  long time, latchmaxclose, tmpl, move=0, cdelay=delay ;
  char phase, filterstate, orgphase=TTV_NO_PHASE,cmdphase ;
  int period_inc, onfilternode=0, filternocross=0;
  char nofiltermax=0, forcedtransp=0, flag=0, aptss_flag;
  long ideal_crossdelay=TTV_NOTIME, ideal_crossdelayend=TTV_NOTIME;
  stbnode *n, *snode;
  int ideal=0;
  ttvfind_stb_stuff dummy[128], *activephase_tss, *newtss, *newtsslist;
 
  stbfig = stb_getstbfig(ttvfig) ;
  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
    start=node, end=root;
  else 
    {
      start=root, end=node;
//      if ((root->FIND->THRU_FILTER & THRU_FILTER_NOCROSS)!=0) return 0;
    }

  if ((root->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_LAST_CROSSED_IS_PRECH)!=0)
    {
      if (getptype(node->ROOT->USER, TTV_SIG_CLOCK)!=NULL) return 0;
      flag|=TTV_FINDSTRUCT_FLAGS_LAST_CROSSED_IS_PRECH;
    }

  if ((stbfig!=NULL && (stbfig->FLAGS & STB_CLOCK_INFO_READY)!=0 && stb_has_filter_directive(end))
      && ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE)==0 || TTV_SEARCH_FIRST_NODE!=end))
    {
      onfilternode=1;
    }

  if((root->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_FORCEDTRANSPARENCY)!= 0)
    {
      forcedtransp=TTV_FINDSTRUCT_FLAGS_FORCEDTRANSPARENCY;
      if (node->FIND != NULL && ttv_check_node_on_path(ttvfig,TTV_SEARCH_FIRST_NODE,root,type,node)) return(0) ;
    }

  if(forcedtransp==0 && ((root->FIND->FLAGS & TTV_FINDSTRUCT_CROSS_ENABLED)!=0 || (root->FIND->THRU_FILTER & THRU_FILTER_ACTIVE)!=0 || onfilternode))
    {
      flag|=TTV_FINDSTRUCT_CROSS_ENABLED;
      if(root->FIND!=NULL && node->FIND != NULL)
        {
          if((node->FIND->PERIODE < root->FIND->PERIODE) &&
             ttv_check_node_on_path(ttvfig,TTV_SEARCH_FIRST_NODE,root,type,node)
             )
            return(0) ;
        }

      if ((root->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_MUSTBECLOCK)!=0)
        {
          if (stb_getstbnode(node)->CK==NULL) return 0;
          else  flag|=TTV_FINDSTRUCT_FLAGS_MUSTBECLOCK;
          if ((root->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_ISIDEAL)!=0) cdelay=0, flag|=TTV_FINDSTRUCT_FLAGS_ISIDEAL;
        }

      period_inc=0;
      newtsslist=NULL;
      if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
        {
          if ((line->TYPE & TTV_LINE_HZ)!=0) return 0;
          snode=stb_getstbnode(node);
          ttv_initfindstb(&dummy[0]);

          for (phase=0; phase<stbfig->PHASENUMBER; phase++)
            {
              aptss_flag=0;
              move=0;
              stb_getdelayedge(stbfig,node,phase,&time,&mindelay, phase,0,NULL,NULL) ;
              if (time!=TTV_NOTIME)
                {
                  aptss_flag=0;
                  if((root->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL
                     || (root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                    {
                      if ((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                        {
                          stb_geteventphase(stbfig, root,&cmdphase,NULL,NULL,1,1) ;
                          cmd=NULL;
                        }
                      else 
                        {
                          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                            cmd = ttv_getlinecmd(ttvfig,line,TTV_LINE_CMDMAX) ;
                          else
                            cmd = ttv_getlinecmd(ttvfig,line,TTV_LINE_CMDMIN) ;
                          if (cmd!=NULL)
                            cmdphase=stb_getstbnode(cmd)->CK->CKINDEX;
                          else
                            stb_geteventphase(stbfig, root,&cmdphase,NULL,NULL,1,1) ;
                        }
                      //                          stb_geteventphase(stbfig,node,&phase,NULL,NULL,0,1) ;
                      stb_getlatchslope(stbfig,root,cmd,NULL,&latchminslope,&latchmaxclose,NULL,NULL,ADDACCESS,phase, &move) ;
                        
                      crossdelay = latchminslope - cdelay ;
                      crossdelayend = latchmaxclose - cdelay ;
                      ideal_crossdelay = latchminslope;
                      ideal_crossdelayend = latchmaxclose;
                      if ((activephase_tss=ttv_getstb_stuff(root->FIND->STB, cmdphase))==NULL)
                        activephase_tss=&dummy[0];
                    }
                  else if (onfilternode)
                    {
                      long tmptime;
                      stb_get_filter_directive_info(stbfig, end, phase, &startmin, &startmax, &endmax, &endmin, &filterstate, &move, &filterclock, &origedge);
                      if (root->FIND->ORGPHASE==TTV_NO_PHASE)
                        stb_get_directive_setup_slope(stbfig, stb_getstbnode(end), &orgphase, &tmptime, 1);
                      ideal_crossdelay=startmin;
                      ideal_crossdelayend=endmax;
                      if ((activephase_tss=ttv_getstb_stuff(root->FIND->STB, origedge.CTKCKINDEX))==NULL)
                        activephase_tss=&dummy[0];
                      if (activephase_tss->IDEAL_CROSSDELAY!=TTV_NOTIME)
                        {
                          if (ideal_crossdelay<activephase_tss->IDEAL_CROSSDELAY+move) ideal_crossdelay=activephase_tss->IDEAL_CROSSDELAY+move;
                          if (ideal_crossdelayend>activephase_tss->IDEAL_NOMORECROSSDELAY+move) ideal_crossdelayend=activephase_tss->IDEAL_NOMORECROSSDELAY+move;
                        }
                      if (activephase_tss->CROSSDELAY!=TTV_NOTIME)
                        {
                          if (startmin<activephase_tss->CROSSDELAY+move) startmin=activephase_tss->CROSSDELAY+move;
                          if (endmax>activephase_tss->NOMORECROSSDELAY+move) endmax=activephase_tss->NOMORECROSSDELAY+move;
                        }
                      crossdelay = startmin - cdelay ;
                      crossdelayend = endmax - cdelay ;
                      latchminslope=startmin;
                      latchmaxclose=endmax;
                    }
                  else
                    {
                      activephase_tss=ttv_getstb_stuff(root->FIND->STB, phase);
                      if (activephase_tss==NULL) continue;
                      crossdelay = activephase_tss->CROSSDELAY - cdelay ;
                      crossdelayend = activephase_tss->NOMORECROSSDELAY - cdelay ;
                      latchminslope=activephase_tss->CROSSDELAY;
                      latchmaxclose=activephase_tss->NOMORECROSSDELAY;
                      ideal_crossdelay=activephase_tss->IDEAL_CROSSDELAY;
                      ideal_crossdelayend=activephase_tss->IDEAL_NOMORECROSSDELAY;
                    }
                  
                  if(time != TTV_NOTIME && (time < crossdelay || (!ttv_avoidmaxfilter(root, onfilternode,&nofiltermax) && mindelay>crossdelayend)))
                    {
                      if (onfilternode && (n=stb_getstbnode(node))->CK!=NULL)
                        {
                          if (n->CK->ORIGINAL_CLOCK!=NULL && getptype(n->CK->ORIGINAL_CLOCK->ROOT->USER, STB_IDEAL_CLOCK)!=NULL)
                            ideal=1, cdelay=0;
                          if (ttv_ckgenerated(mindelay+cdelay, time+cdelay, &origedge, root->TYPE, ' ')
                              && ((root->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_MUSTBECLOCK)!=0
                                  || (!ideal && ttv_ckgenerated(crossdelay+cdelay-move, crossdelayend+cdelay-move, &origedge, root->TYPE, filterstate))
                                  || (ideal && ttv_ckgenerated(ideal_crossdelay-move, ideal_crossdelayend-move, &origedge, root->TYPE, filterstate)))
                              )
                            {
                              if ((node->TYPE & TTV_NODE_UP)!=0) mindelay=n->CK->SUPMIN, time=n->CK->SUPMAX;
                              else mindelay=n->CK->SDNMIN, time=n->CK->SDNMAX;
                              crossdelay=mindelay;
                              crossdelayend=time;
                              ideal_crossdelay=mindelay;
                              ideal_crossdelayend=time;
                              latchminslope=crossdelay+cdelay;
                              latchmaxclose=crossdelayend+cdelay;
                              flag|=TTV_FINDSTRUCT_FLAGS_MUSTBECLOCK;
                              if (ideal) flag|=TTV_FINDSTRUCT_FLAGS_ISIDEAL;
                              move=0;
                            }
                          else 
                            continue;
                        }
                      else continue;
                    }

                  // cas min : on cross un latch mais le delai min ne crosse pas
                  // on calcul un nouveau delai min qui cross
                  if((type & TTV_FIND_MIN) == TTV_FIND_MIN)
                    {
                      if((root->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL
                         || (root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R || onfilternode)
                        {
                          if (mindelay+cdelay<latchminslope) mindelay=latchminslope-cdelay;
                        }
                      else mindelay=activephase_tss->CROSSDELAY_MIN-cdelay;
                    }
                  else
                    {
                      if((root->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL
                         || (root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R || onfilternode)
                        {
                          if (time+cdelay>latchmaxclose && !ttv_avoidmaxfilter(root, onfilternode,&nofiltermax)) mindelay=latchmaxclose-cdelay;
                          else mindelay=time;
                        }
                      else mindelay=activephase_tss->CROSSDELAY_MIN-cdelay;
                    }

                  if((node->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L
                     || (node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                    {
//                      stb_geteventphase(stbfig,node,&phase,NULL,NULL,1,1) ;
                      
                      if (!(activephase_tss->PHASE==phase && (stbfig!=NULL && stbfig->STABILITYMODE & STB_STABILITY_LT) !=0))
                        {
                          if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
                            {
                              if(greater_lowerequal(activephase_tss->PHASE, root->FIND->ORGPHASE, phase))
                                period_inc=1 ;
                            }
                          else
                            {
                              if(lower_greaterequal(activephase_tss->PHASE, root->FIND->ORGPHASE, phase))
                                period_inc=1 ;
                            }
                        }
                    }
                  
                  newtss=ttv_allocfindstb();
                  newtss->PERIOD_CHANGE=activephase_tss->PERIOD_CHANGE+move;
                  newtss->PHASE = phase ;
                  newtss->STARTPHASE = activephase_tss->STARTPHASE ; // don't care
                  newtss->FLAGS = aptss_flag ;
                  newtss->CROSSDELAY = crossdelay ;
                  newtss->NOMORECROSSDELAY = crossdelayend ;
                  newtss->IDEAL_CROSSDELAY = ideal_crossdelay ;
                  newtss->IDEAL_NOMORECROSSDELAY = ideal_crossdelayend ;
                  newtss->CROSSDELAY_MIN = mindelay ;
                  newtss->NEXT=newtsslist;
                  newtsslist=newtss;
                }
            }
        }
      else // ----------------DUAL ---------------
        {
          ttvfind_stb_stuff *activephase_tsslist;
          if (root->FIND->OUTLINE!=NULL && (root->FIND->OUTLINE->TYPE & TTV_LINE_HZ)!=0) return 0;
          if ((root->FIND->FLAGS & TTV_FINDSTRUCT_FLAGS_ISIDEAL)!=0) cdelay=0, flag|=TTV_FINDSTRUCT_FLAGS_ISIDEAL;

          if (root->FIND->STB==NULL)
            {
              int i, j;
              n=stb_getstbnode(TTV_SEARCH_FIRST_NODE);
              for (i=0, j=0; i<stbfig->PHASENUMBER; i++)
                {
                  stb_getdelayedge(stbfig,TTV_SEARCH_FIRST_NODE,i,&time,&mindelay, i,0,NULL,NULL) ;
                  if (time!=TTV_NOTIME)
                  {
                    ttv_initfindstb(&dummy[j]);
                    dummy[j].PHASE=dummy[j].STARTPHASE=i;
                    if (j>0) dummy[j-1].NEXT=&dummy[j];
                    j++;
                  }
                }
              if (j==0) activephase_tsslist=NULL;
              else activephase_tsslist=&dummy[0];
            }
          else
            activephase_tsslist=root->FIND->STB;

          for (activephase_tss=activephase_tsslist; activephase_tss!=NULL; activephase_tss=activephase_tss->NEXT)
            {
              if (activephase_tss->FLAGS & TTV_TPS_FINDSTRUCT_THRU_FILTER_NOCROSS) continue;
              aptss_flag=0;
              phase=activephase_tss->PHASE;
              if((node->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL
                 || (node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                {
                  if (activephase_tss->CROSSDELAY!=TTV_NOTIME)
                    crossdelay=activephase_tss->CROSSDELAY, crossdelayend=activephase_tss->NOMORECROSSDELAY;
                  else
                    {
                      stb_getdelayedge(stbfig,TTV_SEARCH_FIRST_NODE,activephase_tss->STARTPHASE,&time,&mindelay, activephase_tss->STARTPHASE,0,NULL,NULL) ;
//                      if (phase==TTV_NO_PHASE) stb_geteventphase(stbfig,TTV_SEARCH_FIRST_NODE,&phase,NULL,NULL,1,1) ;
                      if (getptype(TTV_SEARCH_FIRST_NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) ideal=1, cdelay=0, flag|=TTV_FINDSTRUCT_FLAGS_ISIDEAL;
                      if (!ideal)
                        {
                          crossdelay=mindelay+root->FIND->DELAY;
                          crossdelayend=time+root->FIND->DELAY;
                        }
                      else
                        {
                          crossdelay=mindelay;
                          crossdelayend=time;
                        }
                    }

                  if ((node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                    {
                      stb_geteventphase(stbfig, node,&cmdphase,NULL,NULL,1,1) ;
                      cmd=NULL;
                    }
                  else 
                    {
                      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                        cmd = ttv_getlinecmd(ttvfig,line,TTV_LINE_CMDMAX) ;
                      else
                        cmd = ttv_getlinecmd(ttvfig,line,TTV_LINE_CMDMIN) ;
                      if (cmd!=NULL)
                        cmdphase=stb_getstbnode(cmd)->CK->CKINDEX;
                      else
                        stb_geteventphase(stbfig, node,&cmdphase,NULL,NULL,1,1) ;
                    }

                  stb_getlatchslope(stbfig,node,cmd,NULL,&time,&latchmaxclose,NULL,NULL,ADDACCESS,phase,&move) ;
//                  if (cmd!=NULL && (n=stb_getstbnode(cmd))!=NULL && n->CK!=NULL)
                    orgphase= cmdphase;
/*                  else
                    orgphase=phase;*/
                  crossdelay += cdelay ;
                  crossdelayend += cdelay;
                  if(crossdelayend<=time || (!ttv_avoidmaxfilter(root, onfilternode,&nofiltermax) && crossdelay>=latchmaxclose))
                    {
                      aptss_flag|=TTV_TPS_FINDSTRUCT_THRU_FILTER_NOCROSS;
                      //return(0) ;
                    }
                  tmpl=activephase_tss->CROSSDELAY_MIN;
                  if ((type & TTV_FIND_MIN) == TTV_FIND_MIN)
                    {
                      if (crossdelay<time)
                        {
                          if (tmpl==TTV_NOTIME)
                            tmpl=0;
                          tmpl+=time-crossdelay;
                          crossdelay=time;
                        }
                    }
                  else
                    {
                      if (crossdelayend>latchmaxclose && !ttv_avoidmaxfilter(root, onfilternode,&nofiltermax))
                        {
                          if (tmpl==TTV_NOTIME)
                            tmpl=0;
                          tmpl+=latchmaxclose-crossdelayend;
                          crossdelay=latchmaxclose;
                        }
                    }
                  mindelay=tmpl;
                }
              else if (onfilternode)
                {
                  stb_get_filter_directive_info(stbfig, end, phase, &startmin, &startmax, &endmax, &endmin, &filterstate, &move, &filterclock, &origedge);
                  stb_get_directive_setup_slope(stbfig, stb_getstbnode(end), &orgphase, &time, 1);
                  if (activephase_tss->CROSSDELAY!=TTV_NOTIME)
                    crossdelay=activephase_tss->CROSSDELAY, crossdelayend=activephase_tss->NOMORECROSSDELAY;
                  else
                    {
                      stb_getdelayedge(stbfig,TTV_SEARCH_FIRST_NODE,activephase_tss->STARTPHASE,&time,&mindelay, activephase_tss->STARTPHASE,0,NULL,NULL) ;
                      if (getptype(TTV_SEARCH_FIRST_NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) ideal=1, cdelay=0, flag|=TTV_FINDSTRUCT_FLAGS_ISIDEAL;
                      if (!ideal)
                        {
                          crossdelay=mindelay+root->FIND->DELAY;
                          crossdelayend=time+root->FIND->DELAY;
                        }
                      else
                        {
                          crossdelay=mindelay;
                          crossdelayend=time;
                        }
                    }
                  crossdelay+=cdelay;
                  crossdelayend+=cdelay;
                  if(crossdelayend<startmin || (!ttv_avoidmaxfilter(root, onfilternode,&nofiltermax) && crossdelay>endmax))
                    {
                      if ((n=stb_getstbnode(root))->CK!=NULL && ttv_ckgenerated(crossdelay, crossdelayend, &origedge, node->TYPE, ' '))
                        {
                          flag|=TTV_FINDSTRUCT_FLAGS_MUSTBECLOCK;
                          startmin=crossdelay;
                          endmax=crossdelayend;
                          move=0;
                        }
                      else
                        {
                          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE)==0)
                            continue;
                          else
                            aptss_flag|=TTV_TPS_FINDSTRUCT_THRU_FILTER_NOCROSS;
                        }
                    }
                  tmpl=activephase_tss->CROSSDELAY_MIN;
                  if ((type & TTV_FIND_MIN) == TTV_FIND_MIN)
                    {
                      if (crossdelay<startmin)
                        {
                          if (tmpl==TTV_NOTIME)
                            tmpl=0;
                          tmpl+=startmin-crossdelay;
                          crossdelay=startmin;
                        }
                    }
                  else
                    {
                      if (crossdelayend>endmax && !ttv_avoidmaxfilter(root, onfilternode,&nofiltermax))
                        {
                          if (tmpl==TTV_NOTIME)
                            tmpl=0;
                          tmpl+=endmax-crossdelayend;
                          crossdelayend=endmax;
                        }
                    }
                  mindelay=tmpl;
                }
              else
                {
                  orgphase=phase;
                  move=0;
                  mindelay=activephase_tss->CROSSDELAY_MIN;
                  if (activephase_tss->CROSSDELAY!=TTV_NOTIME)
                    crossdelay=activephase_tss->CROSSDELAY+cdelay, crossdelayend=activephase_tss->NOMORECROSSDELAY+cdelay;
                }
      
              if((node->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L
                 || (node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                {
                  //              stb_geteventphase(stbfig,node,&phase,NULL,NULL,1,1) ;
                  
                  if (!(activephase_tss->PHASE==phase && (stbfig!=NULL && stbfig->STABILITYMODE & STB_STABILITY_LT) !=0))
                    {
                      if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
                        {
                          if(greater_lowerequal(activephase_tss->PHASE, root->FIND->ORGPHASE, phase))
                            period_inc=1 ;
                        }
                      else
                        {
                          if(lower_greaterequal(activephase_tss->PHASE, root->FIND->ORGPHASE, phase))
                            period_inc=1 ;
                        }
                    }
                }
              
              newtss=ttv_allocfindstb();
              newtss->PERIOD_CHANGE=activephase_tss->PERIOD_CHANGE+move;
              newtss->PHASE = orgphase ;
              newtss->STARTPHASE = activephase_tss->STARTPHASE ;
              newtss->FLAGS = aptss_flag ;
              newtss->CROSSDELAY = crossdelay - move ;
              newtss->NOMORECROSSDELAY = crossdelayend - move;
              newtss->IDEAL_CROSSDELAY = ideal_crossdelay ;
              newtss->IDEAL_NOMORECROSSDELAY = ideal_crossdelayend ;
              newtss->CROSSDELAY_MIN = mindelay ;
              newtss->NEXT=newtsslist;
              newtsslist=newtss;
            }
        }

      if (newtsslist!=NULL)
        {          
          ttv_fifopush(node) ; 
          node->FIND->DELAY = root->FIND->DELAY + delay ;
          node->FIND->OUTLINE = line ;
          node->FIND->PERIODE = root->FIND->PERIODE + period_inc;
          node->FIND->THRU_FILTER=filternocross | (root->FIND->THRU_FILTER || onfilternode?THRU_FILTER_ACTIVE:0);
          node->FIND->FLAGS|=flag|nofiltermax|forcedtransp;
          node->FIND->ORGPHASE=orgphase!=TTV_NO_PHASE?orgphase:root->FIND->ORGPHASE;
          ttv_freefindstblist(node->FIND->STB);
          node->FIND->STB=newtsslist;
        }
      else
        return 0;
    }
  else
    {
      ttv_fifopush(node) ; 
      node->FIND->DELAY = root->FIND->DELAY + delay ;
      node->FIND->OUTLINE = line ;
      node->FIND->FLAGS|=flag|forcedtransp;
      ttv_freefindstblist(node->FIND->STB);
      node->FIND->STB=NULL;
    }


  return(1) ;
}

static ttvevent_list *ttv_findlatchcommand_notdual(ttvevent_list *root, ttvevent_list *node)
{
  ttvevent_list *nodex ;
  ptype_list *ptype ;

  nodex = node ;

  if((nodex->FIND->OUTLINE == NULL) && 
     ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL)) return nodex;

  while(nodex->FIND->OUTLINE != NULL)
    {
      node=nodex;
      nodex = nodex->FIND->OUTLINE->ROOT ;
      if((nodex->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL)) return nodex;

      if((nodex->ROOT->TYPE & TTV_SIG_L)!=0) return node;
      if (nodex == root || (nodex->ROOT->TYPE & TTV_SIG_R)!=0) return nodex;
    }

  return NULL;
}

/*****************************************************************************/
/*                        function ttv_connectorsearchtype()                 */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/* ptsig : latch d'erreur                                                    */
/* type : type de recheche                                                   */
/*                                                                           */
/* dit ce que doit faire la recherche pour un connecteur                     */
/*****************************************************************************/
inline long ttv_connectorsearchtype(ttvfig,node,type)
     ttvfig_list *ttvfig ;
     ttvevent_list *node ;
     long type ;
{
  long res = (long)0 ;

  switch(node->ROOT->TYPE & TTV_SIG_TYPECON)
    {
    case TTV_SIG_CI :  
      res = ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) ? 
        TTV_SIG_BEGINSEARCH : TTV_SIG_ENDSEARCH ;
      res |= TTV_SIG_BEGINPATH ;
      break ;
    case TTV_SIG_CO :  
    case TTV_SIG_CZ : 
      res = ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) ? 
        TTV_SIG_ENDSEARCH : TTV_SIG_BEGINSEARCH ;
      res |= TTV_SIG_ENDPATH ;
      break ;
    case TTV_SIG_CB :  
      res = ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) ? 
        TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
        TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH :
        TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
        TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH;
      if(ttv_islocononlyend(ttvfig,node,type) == 0)
        res |= TTV_SIG_CROSS ;
      break ;
    case TTV_SIG_CT :   
      res = TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
        TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH ;
      if(ttv_islocononlyend(ttvfig,node,type) == 0)
        res |= TTV_SIG_CROSS ;
      break ;
    case TTV_SIG_CX :  ;
      break ;
    }

  return(res) ;
}

/*****************************************************************************/
/*                        function ttv_signalsearchtype()                    */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/* ptsig : latch d'erreur                                                    */
/* type : type de recheche                                                   */
/*                                                                           */
/* dit ce que doit faire la recherche pour un signal donnee                  */
/*****************************************************************************/
inline long ttv_signalsearchtype(ttvfig,node,type)
     ttvfig_list *ttvfig ;
     ttvevent_list *node ;
     long type ;
{
  long res = (long)0 ;

  switch(node->ROOT->TYPE & TTV_SIG_TYPEALL)
    {
    case TTV_SIG_C  : 
      res = ttv_connectorsearchtype(ttvfig,node,type) ;
      break ;
    case TTV_SIG_CQ : 
      res = ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) ? 
        (TTV_SIG_ENDSEARCH|TTV_SIG_CROSS|TTV_SIG_ENDPATH) : 
        (TTV_SIG_CROSS|TTV_SIG_BEGINSEARCH|TTV_SIG_ENDPATH) ; 
      res |= ttv_connectorsearchtype(ttvfig,node,type) ;
      break ;
    case TTV_SIG_N  : 
      res = TTV_SIG_CROSS ;
      break ;
    case TTV_SIG_NQ : 
      res = ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) ? 
        (TTV_SIG_ENDSEARCH|TTV_SIG_CROSS|TTV_SIG_ENDPATH) : 
        (TTV_SIG_CROSS|TTV_SIG_BEGINSEARCH|TTV_SIG_ENDPATH) ; 
      break ;
    case (TTV_SIG_C|TTV_SIG_B) : 
    case (TTV_SIG_N|TTV_SIG_B) : 
      res = TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
        TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH ;
      if((type & TTV_FIND_HIER) != TTV_FIND_HIER)
        res |= TTV_SIG_CROSS ;
      break ;
    case TTV_SIG_Q  : 
      res = ((type & (TTV_FIND_DUAL|TTV_FIND_CMD))!=0) ? 
        (TTV_SIG_ENDSEARCH|TTV_SIG_CROSS|TTV_SIG_ENDPATH) : 
        (TTV_SIG_CROSS|TTV_SIG_BEGINSEARCH|TTV_SIG_ENDPATH) ; 
      break ;
    case TTV_SIG_L  : 
    case TTV_SIG_R  : 
    case TTV_SIG_CL : 
    case TTV_SIG_CR : 
    case TTV_SIG_NL : 
    case TTV_SIG_NR : 
      res = TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
        TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH ;
      if(ttv_iscrosslatch(ttvfig,node,type) == 1)
        res |= TTV_SIG_CROSS ;
      break ;
    case TTV_SIG_B  : 
      res = TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
        TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH|TTV_SIG_CROSS ;
      break ;
    case TTV_SIG_I  : 
      res = TTV_SIG_CROSS ;
    case TTV_SIG_S  : 
      res = TTV_SIG_CROSS ;
      break ;
    }
  if((node->TYPE & TTV_NODE_STOP) == TTV_NODE_STOP
     && (node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B))==0)
    res|=TTV_SIG_BEGINSEARCH|TTV_SIG_ENDSEARCH|
      TTV_SIG_BEGINPATH|TTV_SIG_ENDPATH;


  return(res) ;
}

/*****************************************************************************/
/*                        function ttv_getnodeslope()                        */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/* ptsig : latch d'erreur                                                    */
/* type : type de recheche                                                   */
/*                                                                           */
/* recherche le front d'entree d'yun chemin                                  */
/*****************************************************************************/
long ttv_getnodeslope(ttvfig,ttvins,node,sloperef,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *ttvins ;
     ttvevent_list *node ;
     long *sloperef ;
     long type ;
{
  ttvline_list *line ;
  long slope = TTV_NOSLOPE ;
  long slopex ;

  *sloperef = TTV_NOSLOPE ;

  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1 && TTV_PROPAGATE_FORCED_SLOPE!=TTV_NOTIME)
    {
      *sloperef=TTV_PROPAGATE_FORCED_SLOPE;
      return *sloperef;
    }

  if(((node->ROOT->TYPE & TTV_SIG_CI) == TTV_SIG_CI) && 
     ((node->ROOT->TYPE & TTV_SIG_CB) != TTV_SIG_CB))
    {
      *sloperef = (long)ttv_getnodeslew(node,type);
      return *sloperef ;
    }
  else
    {
      if(ttvins == NULL)
        ttvins = node->ROOT->ROOT ;
      if((type & TTV_FIND_PATH) ==  TTV_FIND_PATH)
        {
          ttv_expfigsig(ttvfig,node->ROOT,ttvins->INFO->LEVEL,ttvfig->INFO->LEVEL,
                        TTV_STS_CL_PJT, TTV_FILE_TTX);
          line = node->INPATH;
        }
      else
        {
          ttv_expfigsig(ttvfig,node->ROOT,ttvins->INFO->LEVEL,ttvfig->INFO->LEVEL,
                        TTV_STS_CLS_FED, TTV_FILE_DTX);
          line = node->INLINE;
        }
      for(; line; line = line->NEXT)
        {
          if((((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) &&
              ((line->TYPE & TTV_LINE_A) != TTV_LINE_A)) ||
             (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
              (line->FIG != ttvfig)) ||
             (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
              (line->FIG->INFO->LEVEL < ttvins->INFO->LEVEL)))
            continue ;
          if((type & TTV_FIND_MAX) ==  TTV_FIND_MAX)
            {
              slopex = ttv_getslopemax(line) ;
              if((slopex > slope) || (slope == TTV_NOSLOPE))
                slope = slopex ;
              slopex = line->FMAX ;
              if((slopex > *sloperef) || (*sloperef == TTV_NOSLOPE))
                *sloperef = slopex ;
            }
          else
            {
              slopex = ttv_getslopemin(line) ;
              if((slopex < slope) || (slope == TTV_NOSLOPE))
                slope = slopex ;
              slopex = line->FMIN ;
              if((slopex < *sloperef) || (*sloperef == TTV_NOSLOPE))
                *sloperef = slopex ;
            }
        }
    }

  if (*sloperef == TTV_NOSLOPE && slope==TTV_NOSLOPE
      && ((node->ROOT->TYPE & TTV_SIG_B)!=0
          || (node->ROOT->TYPE & TTV_SIG_L)!=0
          || (node->ROOT->TYPE & TTV_SIG_R)!=0
          )
      )
    slope=*sloperef=ttvfig->INFO->SLOPE;

  if(*sloperef == TTV_NOSLOPE)
    *sloperef = (long)0 ;
  return((slope == TTV_NOSLOPE) ? (long)0 : slope) ;
}

static void ttv_uncachenodelines(ttvfig_list *ttvfig,ttvfig_list *ttvins,ttvevent_list *node,long type)
{
  if(ttvins == NULL)
    ttvins = node->ROOT->ROOT ;
  if((type & TTV_FIND_PATH) ==  TTV_FIND_PATH)
    {
      ttv_expfigsig(ttvfig,node->ROOT,ttvins->INFO->LEVEL,ttvfig->INFO->LEVEL,
                    TTV_STS_CL_PJT, TTV_FILE_TTX);
    }
  else
    {
      ttv_expfigsig(ttvfig,node->ROOT,ttvins->INFO->LEVEL,ttvfig->INFO->LEVEL,
                    TTV_STS_CLS_FED, TTV_FILE_DTX);
    }
}

/*****************************************************************************/

/*                        function ttv_islocononlyend()                      */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/* ptsig : latch d'erreur                                                    */
/* type : type de recheche                                                   */
/*                                                                           */
/* dit si un connecteur ne doit pas être traversé                            */
/*****************************************************************************/
int ttv_islocononlyend(ttvfig,node,type)
     ttvfig_list *ttvfig ;
     ttvevent_list *node ;
     long type ;
{
  if(((node->ROOT->TYPE & TTV_SIG_CT) != TTV_SIG_CT) ||
     ((node->TYPE & TTV_NODE_ONLYEND) != TTV_NODE_ONLYEND))
    return 0 ;

  if(((node->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT) &&
     ((node->TYPE & TTV_NODE_ONLYEND) == TTV_NODE_ONLYEND))
    return 1 ;

  return 0 ;
#ifndef __ALL__WARNING_
  ttvfig = NULL;
  type = 0;
#endif
}

/*****************************************************************************/
/*                        function ttv_islineonlyend()                       */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/* ptsig : latch d'erreur                                                    */
/* type : type de recheche                                                   */
/*                                                                           */
/* dit si un connecteur ne doit pas être traversé                            */
/*****************************************************************************/
int ttv_islineonlyend(ttvfig,line,type)
     ttvfig_list *ttvfig ;
     ttvline_list *line ;
     long type ;
{
  if(((line->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N) &&
     ((line->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N) &&
     ((line->NODE->TYPE & TTV_NODE_ONLYEND) == TTV_NODE_ONLYEND) &&
     ((line->ROOT->TYPE & TTV_NODE_ONLYEND) == TTV_NODE_ONLYEND))
    return 1 ;
  else
    return 0 ;

#ifndef __ALL__WARNING_
  ttvfig = NULL;
  type = 0;
#endif
}

/*****************************************************************************/
/*                        function ttv_monolatcherror()                      */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/* ptsig : latch d'erreur                                                    */
/*                                                                           */
/* erreur d'une chaine entre un meme latch                                   */
/*****************************************************************************/
void ttv_monolatcherror(ttvfig,ptsig)
     ttvfig_list *ttvfig ;
     ttvsig_list *ptsig ;
{
  char message[1024] ;
  char name[1024] ;

  sprintf(message,"%s for latch %s",ttvfig->INFO->FIGNAME,
          ttv_getsigname(ttvfig,name,ptsig)) ;
  ttv_error(30,message,TTV_WARNING) ;
}

/*****************************************************************************/
/*                        function ttv_addhtpath()                           */
/* parametres :                                                              */
/*                                                                           */
/* ajoute et recherche un chemin dans une table                              */
/*****************************************************************************/
void ttv_addhtpath(htab,node,root)
     ht **htab ;
     ttvevent_list *node ;
     ttvevent_list *root ;
{
  unsigned long key ;
  unsigned long keyx ;
  ptype_list *ptype ;
  int size = (sizeof(void *)/2)*8 ;

  key = (((unsigned long)root) << size) ;
  keyx = (((unsigned long)node) << size) ;
  keyx = (keyx >> size) ;
  key |= keyx ;

  if(*htab == NULL)
    {
      *htab = addht(100) ;
      ptype = NULL ;
    }
  else if((ptype = ttv_gethtpath(*htab,node,root)) == NULL)
    {
      ptype = (ptype_list *)gethtitem(*htab,(void *)key) ;
      if(ptype == (ptype_list *)EMPTYHT)
        ptype = NULL ;
    }

  ptype = addptype(ptype,(long)node,root) ;
  addhtitem(*htab,(void *)key,(long)ptype) ;
}

/*****************************************************************************/
/*                        function ttv_gethtpath()                           */
/* parametres :                                                              */
/*                                                                           */
/* recherche un chemin dans une table                                        */
/*****************************************************************************/
ptype_list *ttv_gethtpath(htab,node,root)
     ht *htab ;
     ttvevent_list *node ;
     ttvevent_list *root ;
{
  ptype_list *ptype ;
  unsigned long key ;
  unsigned long keyx ;
  int size = (sizeof(void *)/2)*8 ;

  key = (((unsigned long)root) << size) ;
  keyx = (((unsigned long)node) << size) ;
  keyx = (keyx >> size) ;
  key |= keyx ;

  if(htab == NULL)
    return(NULL) ;

  ptype = (ptype_list *)gethtitem(htab,(void *)key) ;

  if(ptype == (ptype_list *)EMPTYHT)
    return(NULL) ;

  for(; ptype != NULL ; ptype = ptype->NEXT)
    {
      if((ptype->TYPE == (long)node) && (ptype->DATA == (void *)root)) 
        return(ptype) ;
    }

  return(NULL) ;
}

/*****************************************************************************/
/*                        function ttv_delhtpath()                           */
/* parametres :                                                              */
/*                                                                           */
/* supprime une table de chemin                                              */
/*****************************************************************************/
void ttv_delhtpath(htab)
     ht *htab ;
{
  long  nextitem ;
  void *nextkey ;

  if(htab == NULL)
    return ;

  scanhtkey( htab, 1, &nextkey, &nextitem ) ;
  while( nextitem != EMPTYHT ) {
    freeptype( (ptype_list *)nextitem ) ;
    scanhtkey( htab, 0, &nextkey, &nextitem ) ;
  }
 
  delht(htab) ;
}

/*****************************************************************************/
/*                        function ttv_filterpathhz()                        */
/* parametres :                                                              */
/* path : liste de chemin                                                    */
/*                                                                           */
/* enleve les chemin qui ne sont pas des hz                                  */
/*****************************************************************************/
ttvpath_list *ttv_filterpathhz(path,nbpath,nb,lsi,type)
     ttvpath_list *path ;
     int *nbpath;
     int nb;
     LOCAL_SORT_INFO *lsi;
     long type;
{
  ttvpath_list *ptpath, *limit=NULL;
  ttvpath_list *ptpathx ;
  ttvpath_list *ptpaths ;
  ttvevent_list *node ;
  ttvevent_list *root ;
  ttvsig_list *tvs;
  ht *htab = NULL ;
  int beforepath=1, sortall=0;
 
  if (lsi!=NULL && *nbpath>nb)
    {
      if (lsi->before_paths!=NULL && lsi->before_paths!=(void *)1)
        limit=lsi->before_paths;
      else 
        limit=lsi->paths;
    }
 
  for(ptpath = path ; ptpath != NULL ; ptpath = ptpath->NEXT)
    {
      tvs=ptpath->ROOT->ROOT;
      if((ptpath->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ)
        {
          if (getptype(tvs->USER, TTV_SIG_DONE_PTYPE)==NULL)
            tvs->USER=addptype(tvs->USER, TTV_SIG_DONE_PTYPE, NULL);
        }
    }     

  for(ptpath = path ; ptpath != NULL ; ptpath = ptpath->NEXT)
    {
      tvs=ptpath->ROOT->ROOT;
      if((ptpath->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ
         && getptype(tvs->USER, TTV_SIG_DONE_PTYPE)!=NULL)
        ttv_addhtpath(&htab,ptpath->NODE,ptpath->ROOT) ;
    }

  for(ptpath = path ; ptpath != NULL ; ptpath = ptpathx)
    {
      tvs=ptpath->ROOT->ROOT;
      tvs->USER=testanddelptype(tvs->USER, TTV_SIG_DONE_PTYPE);
    
      ptpathx = ptpath->NEXT ;
      node = ptpath->NODE ; 
      root = ((ptpath->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
        ? ptpath->ROOT->ROOT->NODE
        : ptpath->ROOT->ROOT->NODE +1   ;

      if(((ptpath->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) &&
         ((path->ROOT->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R) &&
         (ttv_gethtpath(htab,node,root) != NULL))
        {
          if(ptpath == path)
            {
              path = path->NEXT ;
            }
          else
            {
              ptpaths->NEXT = ptpath->NEXT ;
            }
          ptpath->NEXT = NULL ;
          ttv_freepathlist(ptpath) ;
          if (nbpath!=NULL) (*nbpath)--;
          if (lsi!=NULL && beforepath==1) sortall=1;
        }
      else
        ptpaths = ptpath ;
      if (lsi!=NULL && lsi->paths==limit) beforepath=0;
    }

  ttv_delhtpath(htab) ;
 
  path = ttv_filterpath(path,type,nbpath) ;

  if (sortall || (lsi!=NULL && *nbpath>nb))
    {
      int i;
      path = ttv_classpath(path,type) ;
   
      for(ptpath=path, i=1; i<nb && ptpath != NULL ; ptpath = ptpath->NEXT , i++) ;

      if(ptpath != NULL)
        {
          ttv_freepathlist(ptpath->NEXT) ;
          ptpath->NEXT = NULL ;
          path=(ttvpath_list *)reverse((chain_list *)path);
          *nbpath=nb;
        }
      else
        *nbpath=i-1;
    }
 
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_filterpath()                          */
/* parametres :                                                              */
/* path : liste de chemin                                                    */
/* type : type de chemin a filtrer                                           */
/*                                                                           */
/* enleve les chemin qui ne sont pas dans le masque des fronts               */
/*****************************************************************************/
ttvpath_list *ttv_filterpath(path,type,nbpath)
     ttvpath_list *path ;
     long type ;
     int *nbpath;
{
  ttvpath_list *patht ;
  ttvpath_list *pathtx ;
  ttvpath_list *pathts ;
  ttvpath_list *pathtss ;

  patht = path ;
  pathts = NULL ;
  pathtss = NULL ;
  while(patht != NULL)
    {
      if((((patht->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
          ((patht->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
          ((type & TTV_FIND_NOT_UPUP) == TTV_FIND_NOT_UPUP)) ||
         (((patht->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
          ((patht->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
          ((type & TTV_FIND_NOT_UPDW) == TTV_FIND_NOT_UPDW)) ||
         (((patht->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
          ((patht->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
          ((type & TTV_FIND_NOT_DWUP) == TTV_FIND_NOT_DWUP)) ||
         (((patht->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
          ((patht->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
          ((type & TTV_FIND_NOT_DWDW) == TTV_FIND_NOT_DWDW)))
        {
          if(patht == path)
            {
              pathtx = path ;
              path = path->NEXT ;
              patht = path ;
              pathtx->NEXT = pathts ;
              pathts = pathtx ;
            }
          else
            {
              pathtx = patht ;
              pathtss->NEXT = patht->NEXT ;
              patht = patht->NEXT ;
              pathtx->NEXT = pathts ;
              pathts = pathtx ;
            }
          if (nbpath!=NULL) (*nbpath)--;
        }
      else
        {
          pathtss = patht ;
          patht = patht->NEXT ;
        }
    }

  ttv_freepathlist(pathts) ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_pathcompar()                          */
/* parametres :                                                              */
/* path1 : premier chemin                                                    */
/* path2 : deuxieme chemin                                                   */
/*                                                                           */
/* compare deux chemin et retourne selon qsort                               */
/*****************************************************************************/
int ttv_pathcompar(path1,path2)
     ttvpath_list **path1 ;
     ttvpath_list **path2 ;
{
  if((*path1)->DELAY < (*path2)->DELAY)
    return 1;
  else if((*path1)->DELAY > (*path2)->DELAY)
    return -1;
  else
    return 0;
}

/*****************************************************************************/
/*                        function ttv_classpath()                           */
/* parametres :                                                              */
/* path : liste des chemins a classer                                        */
/* type : type de classement croissant ou decroissant                        */
/*                                                                           */
/* classe la liste des chemins dans l'ordre croissant suivant le type        */
/*****************************************************************************/
ttvpath_list *ttv_classpath(path,type)
     ttvpath_list *path ;
     long type ;
{
  ttvpath_list *pathx ;
  ttvpath_list **pattab ;
  long nbpath = 0 ;
  long nbpathx ;
 
  if((path == NULL) || ((type & TTV_FIND_NOTCLASS) == TTV_FIND_NOTCLASS))
    return(path) ;

  for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT)
    nbpath++ ;

  pattab = (ttvpath_list**)mbkalloc(nbpath * sizeof(ttvpath_list *)) ;

  pathx = path ;

  for(nbpathx = (long)0 ; nbpathx < nbpath ; nbpathx++)
    {
      *(pattab + nbpathx) = pathx ;
      pathx = pathx->NEXT ;
    }
  
  qsort(pattab,nbpath,sizeof(ttvpath_list *),(int (*)(const void*,const void*))ttv_pathcompar)  ;
 
  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    path = *pattab ;
  else
    path = *(pattab + nbpath -1) ;
  pathx = path ;

  for(nbpathx = (long)1 ; nbpathx < nbpath ; nbpathx++)
    {
      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
        pathx->NEXT = *(pattab + nbpathx) ;
      else
        pathx->NEXT = *(pattab + (nbpath - nbpathx - 1)) ;
      pathx = pathx->NEXT ;
    }

  pathx->NEXT = NULL ;

  mbkfree(pattab) ;

  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_fifopush()                            */
/* parametres :                                                              */
/* node : noeud                                                              */
/*                                                                           */
/* ajoute un noeud dans une fifo et le type TTV_FIND_INFIFO ne fait rien     */
/* si il est deja dans la fifo                                               */
/*****************************************************************************/
void ttv_fifopush(node)
     ttvevent_list *node ;
{
  if(node->FIND == NULL)
    {
      ttv_allocfind(node) ;
      node->FIND->FIFO = TTV_FIFO_NUM ;
    }
  else if(node->FIND->FIFO != TTV_FIFO_NUM)
    {
      ttv_allocfind(node) ;
      node->FIND->FIFO = TTV_FIFO_NUM ;
    }
 
  if((node->FIND->TYPE & TTV_FIND_INFIFO) == TTV_FIND_INFIFO) return ;

  node->FIND->TYPE |= TTV_FIND_INFIFO ;

  if(TTV_FIFO_HEAD == NULL)
    {
      TTV_FIFO_HEAD = TTV_FIFO_UP = TTV_FIFO_B = addchain(TTV_FIFO_HEAD,node) ;
    }
  else if((TTV_FIFO_HEAD == TTV_FIFO_B) && (TTV_FIFO_HEAD->DATA == NULL)) 
    {
      TTV_FIFO_B->DATA = (void *)node ;
    }
  else
    {
      if(TTV_FIFO_B->NEXT != NULL)
        {
          TTV_FIFO_B = TTV_FIFO_B->NEXT ;
          TTV_FIFO_B->DATA = (void *)node ;
        }
      else
        {
          TTV_FIFO_B->NEXT = addchain(NULL,node) ;
          TTV_FIFO_B = TTV_FIFO_B->NEXT ;
        }
    }
  if(TTV_FIFO_UP == NULL) TTV_FIFO_UP = TTV_FIFO_B ;
}

/*****************************************************************************/
/*                        function ttv_fifopop()                             */
/* renvoie le noeud dans la fifo                                             */
/*****************************************************************************/
ttvevent_list *ttv_fifopop()
{
  ttvevent_list *node ;

  if(TTV_FIFO_UP == NULL) return((ttvevent_list *)NULL);
  node = (ttvevent_list *)TTV_FIFO_UP->DATA ;
  if(TTV_FIFO_UP == TTV_FIFO_B) TTV_FIFO_UP = NULL ;
  else TTV_FIFO_UP = TTV_FIFO_UP->NEXT ;
 
  node->FIND->TYPE &= TTV_FIND_OUTFIFO ;
  return(node) ;
}

/*****************************************************************************/
/*                        function ttv_fifoclean()                           */
/* initialise la fifo                                                        */
/*****************************************************************************/
void ttv_fifoclean()
{
  chain_list *chain ;
  ttvevent_list *node ;
  ttvfind_list *find ;

 
  for(chain = TTV_FIFO_HEAD ; chain != TTV_FIFO_B ; chain = chain->NEXT)
    {
      node = (ttvevent_list *)chain->DATA ;
      if(node->FIND != NULL)
        if(node->FIND->FIFO == TTV_FIFO_NUM)
          {
            find = node->FIND ;
            node->FIND = node->FIND->NEXT ;
            find->NEXT = NULL ;
            ttv_freefindlist(find) ;
          }
      if((node->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_B|TTV_SIG_R)) != 0)
        {
          if(getptype(node->USER,TTV_NODE_LATCH) != NULL)
            node->USER = delptype(node->USER,TTV_NODE_LATCH) ;
        }
      chain->DATA = NULL ;
    }
 
  if(chain == NULL) 
    return ;
  else if(chain->DATA == NULL)
    return ;

  node = (ttvevent_list *)chain->DATA ;
  if(node->FIND != NULL)
    if(node->FIND->FIFO == TTV_FIFO_NUM)
      {
        find = node->FIND ;
        node->FIND = node->FIND->NEXT ;
        find->NEXT = NULL ;
        ttv_freefindlist(find) ;
      }
  if((node->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_B|TTV_SIG_R)) != 0)
    {
      if(getptype(node->USER,TTV_NODE_LATCH) != NULL)
        node->USER = delptype(node->USER,TTV_NODE_LATCH) ;
    }
  chain->DATA = NULL ;
 
  TTV_FIFO_B = TTV_FIFO_HEAD ;
  TTV_FIFO_UP = NULL ;
}

/*****************************************************************************/
/*                        function ttv_fifodelete()                          */
/* detruit la fifo                                                           */
/*****************************************************************************/
void ttv_fifodelete()
{
  ttv_fifoclean() ;
  freechain(TTV_FIFO_HEAD) ;
  TTV_FIFO_HEAD = NULL ;
  TTV_FIFO_UP = NULL ;
  TTV_FIFO_B = NULL ;
}

/*****************************************************************************/
/*                        function ttv_fifosave()                            */
/* detruit la fifo                                                           */
/*****************************************************************************/
void ttv_fifosave()
{
  TTV_FIFO_SAV = addchain(TTV_FIFO_SAV,TTV_FIFO_HEAD) ;
  TTV_FIFO_SAV = addchain(TTV_FIFO_SAV,TTV_FIFO_UP) ;
  TTV_FIFO_SAV = addchain(TTV_FIFO_SAV,TTV_FIFO_B) ;
  TTV_FIFO_HEAD = NULL ;
  TTV_FIFO_UP = NULL ;
  TTV_FIFO_B = NULL ;
  TTV_FIFO_NUM++ ;
}

/*****************************************************************************/
/*                        function ttv_fiforestore()                         */
/* detruit la fifo                                                           */
/*****************************************************************************/
void ttv_fiforestore()
{
  ttv_fifodelete() ;
  TTV_FIFO_B = (chain_list *)TTV_FIFO_SAV->DATA ;
  TTV_FIFO_SAV = delchain(TTV_FIFO_SAV,TTV_FIFO_SAV) ;
  TTV_FIFO_UP = (chain_list *)TTV_FIFO_SAV->DATA ;
  TTV_FIFO_SAV = delchain(TTV_FIFO_SAV,TTV_FIFO_SAV) ;
  TTV_FIFO_HEAD = (chain_list *)TTV_FIFO_SAV->DATA ;
  TTV_FIFO_SAV = delchain(TTV_FIFO_SAV,TTV_FIFO_SAV) ;
  TTV_FIFO_NUM-- ;
}

/*****************************************************************************/
/*                        function ttv_getlatchaccess()                      */
/* parametres :                                                              */
/* latch : latch de recherche                                                */
/*                                                                           */
/* recherche les commandes d'un latch pour les access                        */
/*****************************************************************************/
ptype_list *ttv_getlatchaccess_sub(ttvfig_list *ttvfig,ttvevent_list *latch,long type, int withdataonly)
{
  ttvline_list *line ;
  ttvline_list *ptline ;
  ttvevent_list *event ;
  ptype_list *cmdlist = NULL ;
  ptype_list *ptype ;
  long delay ;


  if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    line = latch->INPATH ;
  else
    line = latch->INLINE ;

  for(; line != NULL ; line = line->NEXT) 
    {
      if(((line->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
         (line->FIG != ttvfig))
        continue ;

      if((line->TYPE & TTV_LINE_A) == TTV_LINE_A)
        {
          if (withdataonly==0)
            {
              if((line->NODE->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
                {
                  for(ptype = cmdlist ; ptype != NULL ; ptype = ptype->NEXT)
                    {
                      if(ptype->DATA == line->NODE)
                        {
                          if(ptype->TYPE != (long)0)
                            {
                              ptline = (ttvline_list *)ptype->TYPE ;
                              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                                {
                                  if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 1))==TTV_NOTIME)
                                    delay = ttv_getdelaymax(ptline) ;
                                }
                              else
                                {                         
                                  if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 0))==TTV_NOTIME)
                                    delay = ttv_getdelaymin(ptline) ;
                                }
                            }
                          else
                            {
                              ptline = NULL ;
                              delay = (long)0 ;
                            }
                          if(((type & TTV_FIND_MAX) == TTV_FIND_MAX) && 
                             ((delay < ttv_getdelaymax(line)) || (ptline == NULL)))
                            {
                              ptype->TYPE = (long)line ; 
                            }
                          else if(((type & TTV_FIND_MIN) == TTV_FIND_MIN) &&
                                  ((delay < ttv_getdelaymin(line)) || (ptline == NULL)))
                            {
                              ptype->TYPE = (long)line ; 
                            }
                          break ;
                        }
                    }
                }
              else
                {
                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                    cmdlist = addptype(cmdlist,(long)line,line->NODE) ;
                  else
                    cmdlist = addptype(cmdlist,(long)line,line->NODE) ;
                  line->NODE->TYPE |= TTV_NODE_MARQUE ;
                }
            }
        }
      else
        {
          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            event =  ttv_getlinecmd(ttvfig,line,TTV_LINE_CMDMAX) ;
          else
            event =  ttv_getlinecmd(ttvfig,line,TTV_LINE_CMDMIN) ;
          if(event != NULL)
            {
              if((event->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)
                {
                  cmdlist = addptype(cmdlist,(long)0,event) ;
                  event->TYPE |= TTV_NODE_MARQUE ;
                }
            }
        }
    }
 
  for(ptype = cmdlist ; ptype != NULL ; ptype = ptype->NEXT)
    {
      ((ttvevent_list *)ptype->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
    }

  return(cmdlist) ;
}
ptype_list *ttv_getlatchaccess(ttvfig_list *ttvfig,ttvevent_list *latch,long type)
{
  return ttv_getlatchaccess_sub(ttvfig, latch, type, 0);
}
ptype_list *ttv_getlatchdatacommands(ttvfig_list *ttvfig,ttvevent_list *latch,long type)
{
  return ttv_getlatchaccess_sub(ttvfig, latch, type, 1);
}

ptype_list *ttv_getlatchasynccmd(ttvfig_list *ttvfig,ttvevent_list *latch,long type)
{
  ttvline_list *line ;
  ttvline_list *ptline ;
  ttvevent_list *event ;
  ptype_list *cmdlist = NULL ;
  ptype_list *ptype ;
  long delay ;

  if ((latch->ROOT->TYPE & TTV_SIG_L)==0) return NULL;

  if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    line = latch->INPATH ;
  else
    line = latch->INLINE ;

  for(; line != NULL ; line = line->NEXT) 
    {
      if((line->TYPE & TTV_LINE_CONT) == 0)
      {
         if (ttv_getlinecmd(ttvfig,line,((type & TTV_FIND_MAX) == TTV_FIND_MAX)?TTV_LINE_CMDMAX:TTV_LINE_CMDMIN)==NULL)
         {
            cmdlist = addptype(cmdlist,(long)line,line->NODE) ;
         }
      }
    }
  return(cmdlist) ;
}

/*****************************************************************************/
/*                        function ttv_getcommandaccess()                    */
/* parametres :                                                              */
/* latch : latch de recherche                                                */
/*                                                                           */
/* recherche les commandes d'un latch pour les access                        */
/*****************************************************************************/
ptype_list *ttv_getcommandaccess(ttvfig,command,type)
     ttvfig_list *ttvfig ;
     ttvevent_list *command ;
     long type ;
{
  ttvline_list *line ;
  ttvline_list *ptline ;
  ptype_list *latchlist = NULL ;
  ptype_list *ptype ;
  chain_list *chain ;
  long delay ;


  if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    ptype = getptype(command->USER,TTV_NODE_DUALPATH) ;
  else
    ptype = getptype(command->USER,TTV_NODE_DUALLINE) ;

  if(ptype == NULL)
    return(NULL) ;

  chain = (chain_list *)ptype->DATA ;

  for(; chain != NULL ; chain = chain->NEXT) 
    {
      line = (ttvline_list *)chain->DATA ;

      if(((line->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
         (line->FIG != ttvfig))
        continue ;

      if((line->TYPE & TTV_LINE_A) == TTV_LINE_A)
        {
          if((line->ROOT->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
            {
              for(ptype = latchlist ; ptype != NULL ; ptype = ptype->NEXT)
                {
                  if(ptype->DATA == line->ROOT)
                    {
                      if(ptype->TYPE != (long)0)
                        {
                          ptline = (ttvline_list *)ptype->TYPE ;
                          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                            {
                              if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 1))==TTV_NOTIME)
                                delay = ttv_getdelaymax(ptline) ;
                            }
                          else
                            {                         
                              if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 0))==TTV_NOTIME)
                                delay = ttv_getdelaymin(ptline) ;
                            }
                        }
                      else
                        {
                          ptline = NULL ;
                          delay = (long)0 ;
                        }
                      if(((type & TTV_FIND_MAX) == TTV_FIND_MAX) && 
                         ((delay < ttv_getdelaymax(line)) || (ptline == NULL)))
                        {
                          ptype->TYPE = (long)line ; 
                        }
                      else if(((type & TTV_FIND_MIN) == TTV_FIND_MIN) &&
                              ((delay < ttv_getdelaymin(line)) || (ptline == NULL)))
                        {
                          ptype->TYPE = (long)line ; 
                        }
                      break ;
                    }
                }
            }
          else
            {
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                latchlist = addptype(latchlist,(long)line,line->ROOT) ;
              else
                latchlist = addptype(latchlist,(long)line,line->ROOT) ;
              line->ROOT->TYPE |= TTV_NODE_MARQUE ;
            }
        }
    }
 
  for(ptype = latchlist ; ptype != NULL ; ptype = ptype->NEXT)
    {
      ((ttvevent_list *)ptype->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
    }

  return(latchlist) ;
}


/*****************************************************************************/
/*                        function ttv_findaccess()                          */
/* parametres :                                                              */
/* head : ttvfig pere                                                        */
/* ttvfig : ttvfig courante                                                  */
/* root : noeud sortie                                                       */
/* cmd : noeud command                                                       */
/* type : type de recherche                                                  */
/* level : niveau de la recherche                                            */
/*                                                                           */
/* parcours en largeur pour retrouver les chemins finissant sur root         */
/* si level = 0 il faut chercher dans le niveau de ttvfig qui n'est pas la   */
/* figure pere                                                               */
/* si level = ttvfig->LEVEL la recherche est au niveau de la figure pere     */
/* si level < ttvfig->LEVEL la recherche est entre level et le figure pere   */
/* si level > ttvfig->LEVEL la recherche est entre ttvfig et level sans la   */
/* figure pere                                                               */
/*****************************************************************************/
chain_list *ttv_findaccess(head,ttvfig,root,cmd,type,level)
     ttvfig_list *head ;
     ttvfig_list *ttvfig ;
     ttvevent_list *root ; 
     ttvevent_list *cmd ; 
     long type ;
     long level ;
{
  chain_list *chainin ;
  chain_list *chainclock ;

  chainclock = TTV_CHAIN_CLOCK ;
  TTV_CHAIN_CLOCK = NULL ;
  TTV_FIND_BUILD = 'N' ;

  chainin = ttv_findpath(head,ttvfig,root,cmd,type,level) ;

  TTV_CHAIN_CLOCK = chainclock ;

  TTV_FIND_BUILD = 'Y' ;

  return(chainin) ;
}

//----- fonction de marquage des noeuds sur le chemin de donnees des access ---
// action==0 => demarquer , action==1 => marquer
void ttv_markdatapath(ttvevent_list *start, ttvevent_list *end, long type, int action)
{
  ttvevent_list *nodex;
  if ((type & TTV_FIND_DUAL)!=TTV_FIND_DUAL)
    {
      nodex=start;
      if (action==0) ttv_donotbypassnode(nodex);
      else ttv_bypassnode(nodex);
      while(nodex->FIND->OUTLINE != NULL)
        {
          nodex = nodex->FIND->OUTLINE->ROOT ;
          if (action==0) ttv_donotbypassnode(nodex);
          else ttv_bypassnode(nodex);
          if(nodex == end) break ;
        }
    }
  else
    {
      nodex=end;
      if (action==0) ttv_donotbypassnode(nodex);
      else ttv_bypassnode(nodex);
      while(nodex->FIND->OUTLINE != NULL)
        {
          nodex = nodex->FIND->OUTLINE->NODE ;
          if (action==0) ttv_donotbypassnode(nodex);
          else ttv_bypassnode(nodex);
          if(nodex == end) break ;
        }
    }
}

/*****************************************************************************/
/*                        function ttv_findclockpath()                       */
/* parametres :                                                              */
/* head : ttvfig pere                                                        */
/* ttvfig : ttvfig courante                                                  */
/* root : noeud sortie                                                       */
/* type : type de recherche                                                  */
/* level : niveau de la recherche                                            */
/*                                                                           */
/* parcours en largeur pour retrouver les chemins finissant sur root         */
/* si level = 0 il faut chercher dans le niveau de ttvfig qui n'est pas la   */
/* figure pere                                                               */
/* si level = ttvfig->LEVEL la recherche est au niveau de la figure pere     */
/* si level < ttvfig->LEVEL la recherche est entre level et le figure pere   */
/* si level > ttvfig->LEVEL la recherche est entre ttvfig et level sans la   */
/* figure pere                                                               */
/*****************************************************************************/
chain_list *ttv_findclockpath(ttvfig_list *head,ttvfig_list *ttvfig,ttvevent_list *root,ttvevent_list *end,long type,long level, ttvevent_list *thru_command)
{
  chain_list *chainclock = NULL ;
  chain_list *chainnext ;
  chain_list *chainsav ;
  chain_list *chain ;
  ptype_list *cmdlist ;
  ptype_list *ptype ;
  ptype_list *ptypex ;
  ttvevent_list *event ; 
  ttvevent_list *cmdevent ; 
  ttvsig_list *ptsig ; 
  ttvline_list *ptline ;
  long delay ;
  char find = 'N' ;

  if(TTV_CHAIN_CLOCK == NULL)
    return(NULL) ;
  
  if((((root->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_B)) != 0) &&
      ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
     (((root->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_B)) != 0) &&
      ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)))
    {

      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        cmdlist = ttv_getcommandaccess(ttvfig,root,type) ;
      else if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS)!=0)
        cmdlist=ttv_getlatchasynccmd(ttvfig,root,type);
      else
        cmdlist = ttv_getlatchaccess(ttvfig,root,type) ;

      if(cmdlist == NULL)
        return(NULL) ;

      ttv_markdatapath(root, end, type, 1);

      if(V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE!=0 || (type & TTV_FIND_DUAL) == TTV_FIND_DUAL || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS)!=0)
        find = 'Y' ;
      else 
        {
          if ((type & TTV_FIND_PATHDET)==TTV_FIND_PATHDET
              || (type & TTV_FIND_CRITIC)==TTV_FIND_CRITIC)
            find='Y';
          else
            {
              for(ptype = cmdlist ; ptype != NULL ; ptype = ptype->NEXT)
                {
                  cmdevent = (ttvevent_list *)ptype->DATA ;

                  if (thru_command!=NULL && cmdevent!=thru_command) continue;
                  if (ttv_isbypassednode(cmdevent)) continue;
            
                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                    stb_getstblatchaccess(stb_getstbfig(head),root,cmdevent,NULL,&delay,&event) ;
                  else
                    stb_getstblatchaccess(stb_getstbfig(head),root,cmdevent,&delay,NULL,&event) ;
     
                  if((delay <= (long)0) || (event == NULL) || ttv_canbeinfalsepath(cmdevent, 'o')|| ttv_canbeinfalsepath(end, 'o'))
                    {
                      find='Y';
                      break;
                    }
                }
            }
        }

      for(ptype = cmdlist ; ptype != NULL ; ptype = ptype->NEXT)
        {
          cmdevent = (ttvevent_list *)ptype->DATA ;

          if (thru_command!=NULL && cmdevent!=thru_command) continue;
          if (ttv_isbypassednode(cmdevent)) continue;
            
          ttv_fifopush(cmdevent) ;
          if(ptype->TYPE != (long)0)
            {
              ptline = (ttvline_list *)ptype->TYPE ;
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                {
                  if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE,1))==TTV_NOTIME)
                    delay = ttv_getdelaymax(ptline) ;
                }
              else
                {                         
                  if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 0))==TTV_NOTIME)
                    delay = ttv_getdelaymin(ptline) ;
                }
            }
          else
            {
              ptline = NULL ;
              delay = (long)0 ;
            }
          if((ptypex = getptype(cmdevent->USER,TTV_NODE_LATCH)) == NULL)
            {
              if(cmdevent->FIND->DELAY == TTV_NOTIME)
                {
                  cmdevent->USER = addptype(cmdevent->USER,TTV_NODE_LATCH,root) ;
                  cmdevent->FIND->DELAY = delay + root->FIND->DELAY ;
                  cmdevent->FIND->OUTLINE = ptline ;
                }
              else
                {
                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                    {
                      if(cmdevent->FIND->DELAY < (delay + root->FIND->DELAY))
                        {
                          cmdevent->FIND->DELAY = delay + root->FIND->DELAY ;
                          cmdevent->FIND->OUTLINE = ptline ;
                          cmdevent->USER = addptype(cmdevent->USER,TTV_NODE_LATCH,root) ;
                        }
                    }
                  else
                    {
                      if(cmdevent->FIND->DELAY > (delay + root->FIND->DELAY))
                        {
                          cmdevent->FIND->DELAY = delay + root->FIND->DELAY ;
                          cmdevent->FIND->OUTLINE = ptline ;
                          cmdevent->USER = addptype(cmdevent->USER,TTV_NODE_LATCH,root) ;
                        }
                    }
                }
            }
          else
            {
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                {
                  if(cmdevent->FIND->DELAY < (delay + root->FIND->DELAY))
                    {
                      cmdevent->FIND->DELAY = delay + root->FIND->DELAY ;
                      cmdevent->FIND->OUTLINE = ptline ;
                      ptypex->DATA = root ;
                    }
                }
              else
                {
                  if(cmdevent->FIND->DELAY > (delay + root->FIND->DELAY))
                    {
                      cmdevent->FIND->DELAY = delay + root->FIND->DELAY ;
                      cmdevent->FIND->OUTLINE = ptline ;
                      ptypex->DATA = root ;
                    }
                }
            }

          if (find=='Y') continue;

          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            stb_getstblatchaccess(stb_getstbfig(head),root,cmdevent,NULL,&delay,&event) ;
          else
            stb_getstblatchaccess(stb_getstbfig(head),root,cmdevent,&delay,NULL,&event) ;

          if((delay > (long)0) && (event != NULL))
            {
              ttv_fifopush(event) ;
              if((ptypex = getptype(event->USER,TTV_NODE_CLOCK)) == NULL)
                {
                  chainclock = addchain(chainclock,(void *)event) ;
                  if(event->FIND->DELAY == TTV_NOTIME)
                    {
                      event->USER = addptype(event->USER,TTV_NODE_CLOCK,cmdevent) ;
                      event->FIND->DELAY = delay + cmdevent->FIND->DELAY ;
                      event->FIND->OUTLINE = NULL ;
                    }
                  else
                    {
                      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                        {
                          if(event->FIND->DELAY < (delay + cmdevent->FIND->DELAY))
                            {
                              event->FIND->DELAY = delay + cmdevent->FIND->DELAY ;
                              event->FIND->OUTLINE = NULL ;
                              event->USER = addptype(event->USER,TTV_NODE_CLOCK,root) ;
                            }
                        }
                      else
                        {
                          if(event->FIND->DELAY > (delay + cmdevent->FIND->DELAY))
                            {
                              event->FIND->DELAY = delay + cmdevent->FIND->DELAY ;
                              event->FIND->OUTLINE = NULL ;
                              event->USER = addptype(event->USER,TTV_NODE_CLOCK,root) ;
                            }
                        }
                    }
                }
              else
                {
                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                    {
                      if(event->FIND->DELAY < (delay + cmdevent->FIND->DELAY))
                        {
                          event->FIND->DELAY = delay + cmdevent->FIND->DELAY ;
                          event->FIND->OUTLINE = ptline ;
                          ptypex->DATA = cmdevent ;
                        }
                    }
                  else
                    {
                      if(event->FIND->DELAY > (delay + cmdevent->FIND->DELAY))
                        {
                          event->FIND->DELAY = delay + cmdevent->FIND->DELAY ;
                          event->FIND->OUTLINE = ptline ;
                          ptypex->DATA = cmdevent ;
                        }
                    }
                }
            }
          else
            {
              find = 'Y' ;
            }
        }
      freeptype(cmdlist) ;
    }
  else if((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
    {

      ttv_markdatapath(root, end, type, 1);

      find='Y';
      ttv_fifopush(root) ;
      if((ptype = getptype(root->USER,TTV_NODE_LATCH)) == NULL)
        root->USER = addptype(root->USER,TTV_NODE_LATCH,root) ;
      else
        ptype->DATA = (void *)root ;

      if(root->FIND->NEXT != NULL)
        {
          if(root->FIND->DELAY == TTV_NOTIME)
            {
              if(root->FIND->NEXT->DELAY != TTV_NOTIME)
                {
                  root->FIND->DELAY = root->FIND->NEXT->DELAY ;
                  root->FIND->OUTLINE = root->FIND->NEXT->OUTLINE ;
                }
            }
        }
    }
  else return(NULL) ;

  if(find == 'Y')
    {
      TTV_SEARCH_STEP|=TTV_SEARCH_STEP_IN_CLOCK_PATH;
      chainclock = ttv_findpath(head,ttvfig,root,NULL,type & ~TTV_FIND_HZ,level) ;
      TTV_SEARCH_STEP&=~TTV_SEARCH_STEP_IN_CLOCK_PATH;
    }

  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
    {
      for(chain = TTV_CHAIN_CLOCK ; chain != NULL ; chain = chain->NEXT)
        {
          ptsig = (ttvsig_list *)chain->DATA ;
          ptsig->NODE->TYPE |= TTV_NODE_MARQUE ;
          (ptsig->NODE+1)->TYPE |= TTV_NODE_MARQUE ;
        }
  
      chainsav = NULL ;
      for(chain = chainclock ; chain != NULL ; chain = chainnext)
        {
          chainnext = chain->NEXT ;
          event = (ttvevent_list *)chain->DATA ;
          if((event->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)
            {
              event->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
              if(chainsav == NULL)
                {
                  chainclock = chainnext ;
                  chain->NEXT = NULL ;
                  freechain(chain) ;
                  event->USER=testanddelptype(event->USER,TTV_NODE_CLOCK);
                  continue ;
                }
              else
                {
                  chainsav->NEXT = chain->NEXT ;
                  chain->NEXT = NULL ;
                  freechain(chain) ;
                  continue ;
                }
            }
          chainsav = chain ;
        }
  
      for(chain = TTV_CHAIN_CLOCK ; chain != NULL ; chain = chain->NEXT)
        {
          ptsig = (ttvsig_list *)chain->DATA ;
          ptsig->NODE->TYPE &= ~(TTV_NODE_MARQUE) ;
          (ptsig->NODE+1)->TYPE &= ~(TTV_NODE_MARQUE) ;
        }
    }


  for(chain = TTV_CHAIN_FREE ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_NOT_FREE) ;

  freechain(TTV_CHAIN_FREE) ;
  TTV_CHAIN_FREE = NULL ;

  ttv_markdatapath(root, end, type, 0);
    
  return(chainclock) ;
}

/*****************************************************************************/
/*                        function ttv_iscrosslatch()                        */
/* parametres :                                                              */
/* ttvfig : figure principale                                                */
/*                                                                           */
/* doit-on traverser un latch                                                */
/*****************************************************************************/
int ttv_iscrosslatch(ttvfig,latch,type)
     ttvfig_list *ttvfig ; 
     ttvevent_list *latch ;
     long type ;
{
  stbfig_list *stbfig ;
  long datamin ;
  long datamax ;
  char phase ;
  int maxpathpd=V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE;
  char flags=0;

  if((((latch->ROOT->TYPE & TTV_SIG_LL) !=  TTV_SIG_LL) &&
      ((latch->ROOT->TYPE & TTV_SIG_R) !=  TTV_SIG_R)) ||
     (((type & TTV_FIND_HZ) == TTV_FIND_HZ) && 
      ((latch->ROOT->TYPE & TTV_SIG_R) ==  TTV_SIG_R)))
    return(0) ;

  if (TTV_MaxPathPeriodPrecharge!=0 && (latch->ROOT->TYPE & TTV_SIG_R) !=0)
    {
      maxpathpd=TTV_MaxPathPeriodPrecharge;
      flags=TTV_FINDSTRUCT_FLAGS_LAST_CROSSED_IS_PRECH;
    }

  if (ttv_testsigflag(latch->ROOT, TTV_SIG_FLAGS_DONTCROSS))
    return 0;
  
  if (ttv_testsigflag(latch->ROOT, TTV_SIG_FLAGS_TRANSPARENT))
    {
      latch->FIND->FLAGS|=TTV_FINDSTRUCT_FLAGS_FORCEDTRANSPARENCY;
      latch->FIND->FLAGS|=flags;
      return 1;
    }
  
  if(latch->FIND->PERIODE >= maxpathpd)
    return(0) ;

  if((stbfig = stb_getstbfig(ttvfig)) == NULL)
    return(0) ;

  stb_geteventphase(stbfig,latch,&phase,NULL,NULL,1,1) ;

  if(phase == STB_NO_INDEX)
    {
      latch->FIND->FLAGS|=flags;
      return(1) ;
    }
 
  stb_getstbdelta(stbfig, latch, &datamin, &datamax, NULL);

  if(datamax <= 0)
    {
      return(0) ;
    }

  /* if(latch->FIND->ORGPHASE == TTV_NO_PHASE)
     {
     latch->FIND->ORGPHASE = phase ;
     }
  */
  // latch->FIND->PHASE = phase ;
  latch->FIND->FLAGS|=flags|TTV_FINDSTRUCT_CROSS_ENABLED;

  return(1) ;
}

/*****************************************************************************/
/*                        function ttv_findpath()                            */
/* parametres :                                                              */
/* head : ttvfig pere                                                        */
/* ttvfig : ttvfig courante                                                  */
/* root : noeud sortie                                                       */
/* cmd : noeud command                                                       */
/* type : type de recherche                                                  */
/* level : niveau de la recherche                                            */
/*                                                                           */
/* parcours en largeur pour retrouver les chemins finissant sur root         */
/* si level = 0 il faut chercher dans le niveau de ttvfig qui n'est pas la   */
/* figure pere                                                               */
/* si level = ttvfig->LEVEL la recherche est au niveau de la figure pere     */
/* si level < ttvfig->LEVEL la recherche est entre level et le figure pere   */
/* si level > ttvfig->LEVEL la recherche est entre ttvfig et level sans la   */
/* figure pere                                                               */
/*****************************************************************************/
chain_list *ttv_findpath(head,ttvfig,root,cmd,type,level)
     ttvfig_list *head ;
     ttvfig_list *ttvfig ;
     ttvevent_list *root ; 
     ttvevent_list *cmd ; 
     long type ;
     long level ;
{
  ttvfig_list *ttvfigx ;
  ttvfig_list *ttvfigp ;
  ttvevent_list *node ;
  ttvevent_list *nodex ;
  ttvevent_list *rootx ;
  ptype_list *ptype ;
  chain_list *in_gi = NULL ;
  chain_list *chain ;
  chain_list *chainnfree = NULL ;
  long status ;
  long filetype ;
  long delay ;
  char typesigroot = 'X' ;

  ttv_ssta_set_checkloadedstore(head);
 
  if(((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
     ((root->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
     ((root->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
    typesigroot = 'R' ; 
  if(((root->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) || 
     ((root->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
    typesigroot = 'Z' ; 

  if((root->FIND == NULL) || 
     (root->FIND->DELAY == TTV_NOTIME) || 
     (TTV_CHAIN_CLOCK == NULL))
    {
      ttv_fifopush(root) ;
      root->FIND->DELAY = (long)0 ;
    }

  if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    {
      status = TTV_STS_L_J ;
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        status |= TTV_STS_DUAL_J ;
      filetype = TTV_FILE_TTX ;
    }
  else
    {
      status = TTV_STS_LS_E ;
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        status |= TTV_STS_DUAL_E ;
      filetype = TTV_FILE_DTX ;
    }

  if((ttvfig->INFO->LEVEL >= level) && (level != (long)0))
    {
      long statusx = status ;

      if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
        {
          statusx |= TTV_STS_CL_PJT ;
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            statusx |= (TTV_STS_DUAL_T|TTV_STS_DUAL_P|TTV_STS_DUAL_J) ;
        }
      else
        {
          statusx |= TTV_STS_CLS_FED ;
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            statusx |= (TTV_STS_DUAL_D|TTV_STS_DUAL_F|TTV_STS_DUAL_E) ;
        }
      if((ttvfig->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE)
        {
          chainnfree = addchain(chainnfree,(void *)ttvfig) ;
          ttvfig->STATUS |= TTV_STS_NOT_FREE ;
        }
      ttv_parsttvfig(ttvfig,statusx,filetype) ;
      ttvfigp = ttvfig ;
      if(ttvfig->INFO->LEVEL > level)
        {
          ttvfig = root->ROOT->ROOT ;
          while(ttvfig != NULL)
            {
              if(ttvfig->INFO->LEVEL >= level) break ;
              ttvfig = ttvfig->ROOT ;
            }
          level = ttvfigp->INFO->LEVEL ;
        }
    }
  else
    {
      if(level == (long)0)
        {
          level = ttvfig->INFO->LEVEL ;
        }
      ttvfigp = NULL ;
    }

  ttvfigx = root->ROOT->ROOT ;
  while(ttvfigx != NULL)
    {
      long statusx = status ;
      if(ttvfigx->INFO->LEVEL > level) break ;
      if(ttvfigx->INFO->LEVEL >= ttvfig->INFO->LEVEL)
        {
          if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
            {
              statusx |= TTV_STS_P ;
              if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                statusx |= TTV_STS_DUAL_P ;
            }
          else
            {
              statusx |= TTV_STS_F ;
              if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                statusx |= TTV_STS_DUAL_F ;
            }
        }
      if((ttvfigx->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE)
        {
          chainnfree = addchain(chainnfree,(void *)ttvfigx) ;
          ttvfigx->STATUS |= TTV_STS_NOT_FREE ;
        }
      ttv_parsttvfig(ttvfigx,statusx,filetype) ;
      ttvfigx = ttvfigx->ROOT ;
    }

  while((node = ttv_fifopop()) != NULL)
    {
      ttvline_list  *in ;

      //   if (TTV_MARK_MODE==TTV_MARK_MODE_USE && ??inf->CLOCK!=0?? && getptype(node->ROOT->USER, TTV_SIG_MARKED)==NULL) continue;
      if (((TTV_SEARCH_STEP & TTV_SEARCH_STEP_IN_CLOCK_PATH)!=0 || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MUST_BE_CLOCK)!=0) && ttv_testsigflag(node->ROOT, TTV_SIG_FLAGS_ISDATA)) continue;

      if((node->TYPE & TTV_NODE_STOP) == TTV_NODE_STOP) 
        {
          if(node != root &&  
             (((type & (TTV_FIND_HIER|TTV_FIND_DUAL)) != 0 || (node->ROOT->TYPE & (TTV_SIG_CB|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_Q)) != TTV_SIG_CB)
              && ((type & TTV_FIND_HIER) == TTV_FIND_HIER || ((node->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_Q)) != TTV_SIG_N || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N)!=0))
              && ((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_Q)) != TTV_SIG_Q || (type & (TTV_FIND_DUAL|TTV_FIND_CMD)) != 0)))
            {
              if((node->FIND->TYPE & TTV_FIND_BEGIN) != TTV_FIND_BEGIN)
                {
                  in_gi = addchain(in_gi,(char *)node) ;
                  node->FIND->TYPE |= TTV_FIND_BEGIN ;
                }
              if(  ((((node->ROOT->TYPE & (TTV_SIG_CB|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_CB)
                     && ((node->ROOT->TYPE & (TTV_SIG_CT|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_CT)
                     && ((node->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_N))
                    || 
                    ((type & TTV_FIND_HIER) == TTV_FIND_HIER)
                    ||(ttv_islocononlyend(ttvfig,node,type) != 0))
                   &&
                   (((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_Q)) != TTV_SIG_Q)
                    || ((type & TTV_FIND_CMD) != TTV_FIND_CMD))
                   //|| ((type & (TTV_FIND_DUAL|TTV_FIND_CMD)) != TTV_FIND_DUAL))
                   && ((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_N | TTV_SIG_B)) != TTV_SIG_B))
                if((TTV_CHAIN_CLOCK != NULL) || (TTV_FIND_BUILD == 'Y') ||
                   (ttv_iscrosslatch(head,node,type) == 0))
                  {
                    if (!((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_CROSS_STOP_NODE_IF_NONSTOP)!=0 && 
                          (node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B))==0))
                      continue ;
                  }
            }
        }

      if(((((node->ROOT->TYPE & TTV_SIG_BYPASSOUT) != TTV_SIG_BYPASSOUT) &&
           ((node->TYPE & TTV_NODE_BYPASSOUT) != TTV_NODE_BYPASSOUT)) ||
          ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)) &&
         ((((node->ROOT->TYPE & TTV_SIG_BYPASSIN) != TTV_SIG_BYPASSIN) &&
           ((node->TYPE & TTV_NODE_BYPASSIN) != TTV_NODE_BYPASSIN)) ||
          ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)))
        {
          if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
            {
              if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                {
                  ptype = getptype(node->USER,TTV_NODE_DUALPATH) ;
                  if(ptype == NULL) in = NULL ;
                  else
                    {
                      chain = (chain_list *)ptype->DATA ;
                      if(chain == NULL) in = NULL ;
                      else in = (ttvline_list *)chain->DATA ;
                    }
                }
              else in = node->INPATH ;
            }
          else
            {
              if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                {
                  ptype = getptype(node->USER,TTV_NODE_DUALLINE) ;
                  if(ptype == NULL) in = NULL ;
                  else
                    {
                      chain = (chain_list *)ptype->DATA ;
                      if(chain == NULL) in = NULL ;
                      else in = (ttvline_list *)chain->DATA ;
                    }
                }
              else in = node->INLINE ;
            }

          while(in != NULL)
            {
              if(((in->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
                 (((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                  ((in->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
                  (ttv_islineonlyend(ttvfig,in,type) == 1)))
                {
                  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                    {
                      chain = chain->NEXT ;
                      if(chain == NULL) in = NULL ;
                      else in = (ttvline_list *)chain->DATA ;
                      continue ;
                    }
                  else
                    {
                      in = in->NEXT ;
                      continue ;
                    }
                }
              if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                {
                  nodex = in->ROOT ;
                  rootx = in->NODE ;
                  if(((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) && 
                     (node->FIND->OUTLINE != NULL)) 
                    {
                      if((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
                        {
                          chain = chain->NEXT ;
                          if(chain == NULL) in = NULL ;
                          else in = (ttvline_list *)chain->DATA ;
                          continue ;
                        }
                    }
                  if(((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
                     ((nodex->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
                     ((nodex->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
                    {
                      if((((in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) &&
                          ((type & TTV_FIND_HZ) != TTV_FIND_HZ)) || 
                         (((in->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ) &&
                          ((type & TTV_FIND_HZ) == TTV_FIND_HZ)))
                        {
                          chain = chain->NEXT ;
                          if(chain == NULL) in = NULL ;
                          else in = (ttvline_list *)chain->DATA ;
                          continue ;
                        }
                    }
                  else if((in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ)
                    {
                      chain = chain->NEXT ;
                      if(chain == NULL) in = NULL ;
                      else in = (ttvline_list *)chain->DATA ;
                      continue ;
                    }
                  if((cmd != NULL) && ((nodex->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L))
                    {
                      if(cmd != ttv_getlinecmd(head,in,((type & TTV_FIND_MAX) == TTV_FIND_MAX)?TTV_LINE_CMDMAX:TTV_LINE_CMDMIN))
                        {
                          chain = chain->NEXT ;
                          if(chain == NULL) in = NULL ;
                          else in = (ttvline_list *)chain->DATA ;
                          continue ;
                        }
                    }
                  if(((nodex->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
                     ((nodex->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
                    {
                      if((((in->TYPE & TTV_LINE_R) == TTV_LINE_R) &&
                          ((type & TTV_FIND_S) == TTV_FIND_S)) ||
                         (((in->TYPE & TTV_LINE_S) == TTV_LINE_S) &&
                          ((type & TTV_FIND_R) == TTV_FIND_R)))
                        {
                          chain = chain->NEXT ;
                          if(chain == NULL) in = NULL ;
                          else in = (ttvline_list *)chain->DATA ;
                          continue ;
                        }
                    }
                }
              else 
                {
                  nodex = in->NODE ;
                  rootx = in->ROOT ;
                  if(((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) && 
                     (node->FIND->OUTLINE != NULL)) 
                    {
                      if((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
                        {
                          in = in->NEXT ;
                          continue ;
                        }
                    }
                  if((typesigroot == 'R') ||
                     ((rootx->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
                     ((rootx->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
                    {
                      if(((((in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) &&
                           ((type & TTV_FIND_HZ) != TTV_FIND_HZ)) || 
                          (((in->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ) &&
                           ((type & TTV_FIND_HZ) == TTV_FIND_HZ))) && 
                         ((rootx == root) ||
                          ((rootx->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
                          ((rootx->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))) 
                        {
                          in = in->NEXT ;
                          continue ;
                        }
                    }
                  else if((in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ)
                    {
                      in = in->NEXT ;
                      continue ;
                    }
                  if((cmd != NULL || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS)!=0)
                     && ((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L) 
                     && (rootx == root))
                    {
                      if(cmd != ttv_getlinecmd(head,in,((type & TTV_FIND_MAX) == TTV_FIND_MAX)?TTV_LINE_CMDMAX:TTV_LINE_CMDMIN))
                        {
                          in = in->NEXT ;
                          continue ;
                        }
                    }
                  if(typesigroot == 'Z')
                    {
                      if(((((in->TYPE & TTV_LINE_R) == TTV_LINE_R) &&
                           ((type & TTV_FIND_S) == TTV_FIND_S)) || 
                          (((in->TYPE & TTV_LINE_S) == TTV_LINE_S) &&
                           ((type & TTV_FIND_R) == TTV_FIND_R))) && (rootx == root))
                        {
                          in = in->NEXT ;
                          continue ;
                        }
                    }
                }

              if((((in->TYPE & TTV_LINE_DENPTMAX) == TTV_LINE_DENPTMAX) &&
                  ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                 (((in->TYPE & TTV_LINE_DENPTMIN) == TTV_LINE_DENPTMIN) &&
                  ((type & TTV_FIND_MIN) == TTV_FIND_MIN)) ||
                 (((in->TYPE & (TTV_LINE_T | TTV_LINE_D)) != 0) && 
                  (ttvfigp != in->FIG)) ||
                 (((in->TYPE & (TTV_LINE_J | TTV_LINE_E)) != 0) && 
                  (level < in->FIG->INFO->LEVEL)) ||
                 (((in->TYPE & (TTV_LINE_F | TTV_LINE_P)) != 0) && 
                  ((level < in->FIG->INFO->LEVEL) || (ttvfig->INFO->LEVEL > in->FIG->INFO->LEVEL))))
                {
                  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                    {
                      chain = chain->NEXT ;
                      if(chain == NULL) in = NULL ;
                      else in = (ttvline_list *)chain->DATA ;
                    }
                  else in = in->NEXT ;
                  continue ;
                }

              if(node->ROOT->ROOT != nodex->ROOT->ROOT)
                {
                  ttvfigx = nodex->ROOT->ROOT ;
                  while((ttvfigx != ttvfig) && ((ttvfigx->STATUS & 
                                                 TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE))
                    {
                      if((ttvfigx->STATUS & TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE)
                        {
                          chainnfree = addchain(chainnfree,(void *)ttvfigx) ;
                          ttvfigx->STATUS |= TTV_STS_NOT_FREE ;
                        }
                      ttv_parsttvfig(ttvfigx,status,filetype) ;
                      ttvfigx = ttvfigx->ROOT ;
                    }
                }
           
              if(nodex == root)
                {
                  if (TTV_CHAIN_CLOCK==NULL)
                    {
                      if(((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
                         || ((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R))
                        {
                          if(((((nodex->ROOT->TYPE & TTV_SIG_BYPASSIN) != TTV_SIG_BYPASSIN) &&
                               ((nodex->TYPE & TTV_NODE_BYPASSIN) != TTV_NODE_BYPASSIN)) ||
                              ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)) &&
                             ((((nodex->ROOT->TYPE & TTV_SIG_BYPASSOUT) != TTV_SIG_BYPASSOUT) &&
                               ((nodex->TYPE & TTV_NODE_BYPASSOUT) != TTV_NODE_BYPASSOUT)) ||
                              ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)))
                            {
                              if((ptype = getptype(root->USER,TTV_NODE_DELAYLOOP)) == NULL)
                                {
                                  ptype = root->USER = addptype(root->USER,
                                                                TTV_NODE_DELAYLOOP,(void*)TTV_NOTIME) ;
                                }
                              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                                {
                                  delay =  ttv_getdelaymax(in) ;
                                  if(((node->FIND->DELAY + delay) > (long)ptype->DATA) &&
                                     (delay != TTV_NOTIME))
                                    {
                                      ptype->DATA = (void *)(node->FIND->DELAY + delay) ;
                                      nodex->FIND->OUTLINE = in ;
                                      if((root->FIND->TYPE & TTV_FIND_BEGIN) != TTV_FIND_BEGIN)
                                        {
                                          in_gi = addchain(in_gi,(char *)root) ;
                                          root->FIND->TYPE |= TTV_FIND_BEGIN ;
                                        }
                                    }
                                }
                              else
                                {
                                  delay =  ttv_getdelaymin(in) ;
                                  if((((node->FIND->DELAY + delay) < (long)ptype->DATA) ||
                                      ((long)ptype->DATA == TTV_NOTIME)) 
                                     && (delay != TTV_NOTIME))
                                    {
                                      ptype->DATA = (void *)(node->FIND->DELAY + delay) ;
                                      nodex->FIND->OUTLINE = in ;
                                      if((root->FIND->TYPE & TTV_FIND_BEGIN) != TTV_FIND_BEGIN)
                                        {
                                          in_gi = addchain(in_gi,(char *)root) ;
                                          root->FIND->TYPE |= TTV_FIND_BEGIN ;
                                        }
                                    }
                                }
                              if((long)ptype->DATA == TTV_NOTIME)
                                ptype = root->USER = delptype(root->USER,
                                                              TTV_NODE_DELAYLOOP) ;
                            }
                        }
                      else
                        ttv_monolatcherror(ttvfig,root->ROOT) ;
                    }
                  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                    {
                      chain = chain->NEXT ;
                      if(chain == NULL) in = NULL ;
                      else in = (ttvline_list *)chain->DATA ;
                    }
                  else in = in->NEXT ;
                  continue ;
                }
          
              if(((((nodex->ROOT->TYPE & TTV_SIG_BYPASSIN) != TTV_SIG_BYPASSIN) &&
                   ((nodex->TYPE & TTV_NODE_BYPASSIN) != TTV_NODE_BYPASSIN)) ||
                  ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)) &&
                 ((((nodex->ROOT->TYPE & TTV_SIG_BYPASSOUT) != TTV_SIG_BYPASSOUT) &&
                   ((nodex->TYPE & TTV_NODE_BYPASSOUT) != TTV_NODE_BYPASSOUT)) ||
                  ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)))
                {
                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)  
                    {
                      delay =  ttv_getdelaymax(in) ;
                      if(delay != TTV_NOTIME)
                        {
                          if((nodex->FIND == NULL) ||
                             (!ttv_isbypassednode(nodex) && ((nodex->FIND->FIFO < node->FIND->FIFO) ||
                                                             ((node->FIND->DELAY + delay) > nodex->FIND->DELAY)))
                             )
                            {
                              ttv_transfertnodefindinfo(head,nodex,node,in,delay,type) ;
                            }
                        }
                    }
                  else 
                    {
                      delay =  ttv_getdelaymin(in) ;
                      if(delay != TTV_NOTIME)
                        {
                          if((nodex->FIND == NULL) || 
                             (!ttv_isbypassednode(nodex) && ((nodex->FIND->FIFO < node->FIND->FIFO) ||
                                                             ((node->FIND->DELAY + delay) < nodex->FIND->DELAY)))
                             )
                            {
                              ttv_transfertnodefindinfo(head,nodex,node,in,delay,type) ;
                            }
                        }
                    }
                }
              if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                {
                  chain = chain->NEXT ;
                  if(chain == NULL) in = NULL ;
                  else in = (ttvline_list *)chain->DATA ;
                }
              else in = in->NEXT ;
            }
        }
    }

  if(TTV_CHAIN_CLOCK == NULL)
    {
      for(chain = chainnfree ; chain != NULL ; chain = chain->NEXT)
        ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_NOT_FREE) ;

      freechain(chainnfree) ;
      TTV_CHAIN_FREE = NULL ;
    }
  else
    {
      TTV_CHAIN_FREE = chainnfree ;
    }

  if((type & (TTV_FIND_DUAL|TTV_FIND_HZ)) == (TTV_FIND_DUAL|TTV_FIND_HZ)) 
    {
      chainnfree = NULL ;
      for(chain = in_gi ; chain != NULL ; chain = chain->NEXT)
        {
          node = (ttvevent_list *)chain->DATA ;
          if(((node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
             ((node->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
             ((node->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
            chainnfree = addchain(chainnfree,(void*)node) ;
          else
            node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
        }
      freechain(in_gi) ;
      in_gi = chainnfree ;
      if((ptype = getptype(root->USER,TTV_NODE_DELAYLOOP)) != NULL)
        {
          root->FIND->DELAY = (long)ptype->DATA ;
          root->USER = delptype(root->USER,TTV_NODE_DELAYLOOP) ;
        }
    }

  if((type & (TTV_FIND_DUAL|TTV_FIND_S)) == (TTV_FIND_DUAL|TTV_FIND_S))
    {
      chainnfree = NULL ;
      for(chain = in_gi ; chain != NULL ; chain = chain->NEXT)
        {
          node = (ttvevent_list *)chain->DATA ;
          if(((node->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
             ((node->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
            chainnfree = addchain(chainnfree,(void*)node) ;
          else
            node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
        }
      freechain(in_gi) ;
      in_gi = chainnfree ;
      if((ptype = getptype(root->USER,TTV_NODE_DELAYLOOP)) != NULL)
        {
          root->FIND->DELAY = (long)ptype->DATA ;
          root->USER = delptype(root->USER,TTV_NODE_DELAYLOOP) ;
        }
    }


  if((root->FIND->TYPE & TTV_FIND_BEGIN) == TTV_FIND_BEGIN)
    {
      if((ptype = getptype(root->USER,TTV_NODE_DELAYLOOP)) != NULL)
        {
          root->FIND->DELAY = (long)ptype->DATA ;
          root->USER = delptype(root->USER,TTV_NODE_DELAYLOOP) ;
        }
    }

  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      if(ttv_canbeinfalsepath(root, 'i'))
        {
          chainnfree = NULL ;
          for(chain = in_gi ; chain != NULL ; chain = chain->NEXT)
            {
              node = (ttvevent_list *)chain->DATA ;
              if(ttv_canbeinfalsepath(node, 'o'))
                {
                  int ret;
                  if((ret=ttv_isfalsepath(head,root,node,type,0,NULL)) == 0)
                    chainnfree = addchain(chainnfree,(void*)node) ;
                  else
                    {
                      node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
                      if (ret==1) TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
                    }
                }
              else
                {
                  chainnfree = addchain(chainnfree,(void*)node) ;
                }
            }
          freechain(in_gi) ;
          in_gi = chainnfree ;
        }
    }
  else
    {
      if (!(TTV_SEARCH_STEP & TTV_SEARCH_STEP_IN_CLOCK_PATH))
        {
          if(ttv_canbeinfalsepath(root, 'o'))
            {
              chainnfree = NULL ;
              for(chain = in_gi ; chain != NULL ; chain = chain->NEXT)
                {
                  node = (ttvevent_list *)chain->DATA ;
                  if(ttv_canbeinfalsepath(node, 'i'))
                    {
                      int ret;
                      if((ret=ttv_isfalsepath(head,node,root,type,0,NULL)) == 0)
                        chainnfree = addchain(chainnfree,(void*)node) ;
                      else
                        {
                          node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
                          if (ret==1) TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
                        }
                    }
                  else
                    {
                      chainnfree = addchain(chainnfree,(void*)node) ;
                    }
                }
              freechain(in_gi) ; 
              in_gi = chainnfree ;
            }
        }
      else
        {
          ttvevent_list *cmdev;
          int isfalse;
          chainnfree = NULL ;
          for(chain = in_gi ; chain != NULL ; chain = chain->NEXT)
            {
              isfalse=0;
              node = (ttvevent_list *)chain->DATA ;
              if(ttv_canbeinfalsepath(node, 'i'))
                {
                  // retreive the latch command
                  cmdev=ttv_findlatchcommand_notdual(root, node);
                  if(cmdev!=NULL && ttv_canbeinfalsepath(cmdev, 'o'))
                    {
                      int ret;
                      if((ret=ttv_isfalsepath(head,node,cmdev,type,0,NULL))!=0)
                        {
                          node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
                          if (ret==1) TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
                          isfalse=1;
                        }
                    }
                }
              if (!isfalse)
                chainnfree = addchain(chainnfree,(void*)node) ;
            }
          freechain(in_gi) ; 
          in_gi = chainnfree ;
        }
    }

  return(in_gi) ;
}

/*****************************************************************************/
/*                        function ttv_getcritic()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : ttvfig ou se trouve le chemin                                   */
/* root : noeud de depart                                                    */
/* node : noeud d'arrive                                                     */
/* type : type de recherche                                                  */
/*                                                                           */
/* construit le detail d'un chemin en fonction du type de temps max ou min   */
/*****************************************************************************/
ttvcritic_list *ttv_getcritic(ttvfig,figpath,root,node,latch,cmdlatch,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvevent_list *root ;
     ttvevent_list *node ;
     ttvevent_list *latch ;
     ttvevent_list *cmdlatch ;
     long type ;
{
  ttvfig_list *ttvfigx ;
  ttvevent_list *nodex ;
  ttvfig_list *ttvfigsav ;
  chain_list *chain ;
  chain_list *chainx ;
  ttvcritic_list *critic = NULL ;
  long levelroot ;
  long levelnode ;
  long level ;

  if(((type & TTV_FIND_PATH) != TTV_FIND_PATH) || 
     ((type & TTV_FIND_LINE) == TTV_FIND_LINE)) 
    {
      type |= TTV_FIND_LINE ;
      type &= ~(TTV_FIND_PATH) ;
    }

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      type &= ~(TTV_FIND_DUAL) ;
      TTV_CHAIN_CLOCK = addchain(TTV_CHAIN_CLOCK,node->ROOT) ;
    }

  if(figpath == NULL)
    {
      levelroot = ttv_getsiglevel(root->ROOT) ;
      if((latch != NULL) && (node != latch))
        levelnode = ttv_getsiglevel(latch->ROOT) ;
      else
        levelnode = ttv_getsiglevel(node->ROOT) ;

      if(levelroot > ttvfig->INFO->LEVEL) levelroot = ttvfig->INFO->LEVEL ;
      if(levelnode > ttvfig->INFO->LEVEL) levelnode = ttvfig->INFO->LEVEL ;

      if(levelroot > levelnode) level = levelnode ;
      else level = levelroot ;

      for(ttvfigx = root->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS |= TTV_STS_MARQUE ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }

      if((latch != NULL) && (node != latch))
        ttvfigx = latch->ROOT->ROOT ;
      else
        ttvfigx = node->ROOT->ROOT ;

      for(; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        if((ttvfigx->STATUS & TTV_STS_MARQUE) == TTV_STS_MARQUE)
          break ;

      ttvfigsav = ttvfigx ;

      for(ttvfigx = root->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS &= ~(TTV_STS_MARQUE) ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }

      if(ttvfigsav == NULL)
        {
          if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
            {
              freechain(TTV_CHAIN_CLOCK) ;
              TTV_CHAIN_CLOCK = NULL ;
            }
          return(NULL) ;
        }

      if((level < ttvfig->INFO->LEVEL) && (level == ttvfigsav->INFO->LEVEL))
        {
          level = (long)0 ;
        }
      else if((level == ttvfig->INFO->LEVEL) && (level > ttvfigsav->INFO->LEVEL))
        {
          level = ttvfigsav->INFO->LEVEL ;
          ttvfigsav = ttvfig ;
        }
    }
  else
    {
      ttvfigsav = figpath ;
      if(figpath == ttvfig)
        {
          level = figpath->INFO->LEVEL ;
        }
      else
        {
          level = (long)0 ;
        }
    }

  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      nodex = node ;
      node = root ;
      root = nodex ;
    }

  TTV_SEARCH_FIRST_NODE=root;

  if((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_Q | TTV_SIG_R |
                          TTV_SIG_N|TTV_SIG_B)) == 0)
    node->TYPE |= TTV_NODE_STOP ;

  if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0 || (root->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_B))==0)
    chain = ttv_findaccess(ttvfig,ttvfigsav,root,NULL,type,level) ;
  else
    {
      ttv_fifopush(root) ;
      ttv_uncachenodelines(ttvfig,ttvfigsav,root,type);
      root->FIND->DELAY = 0 ;
      chain=addchain(NULL, root);
    }


  for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
      ((ttvevent_list *)chainx->DATA)->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
    }

  freechain(chain) ;

  if((latch != NULL) && (node != latch))
    {
      int found=0;
      // BLINDAGE CORE si latch non trouve
      if (latch->FIND==NULL) 
        {
          char buf0[128], buf1[128], buf2[128], buf[1024];
          if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
            {
              freechain(TTV_CHAIN_CLOCK) ;
              TTV_CHAIN_CLOCK = NULL ;
            }
          sprintf(buf, "could not reach latch '%s' for path from '%s' to '%s'",
                  ttv_getsigname(ttvfig,buf2,latch->ROOT),
                  ttv_getsigname(ttvfig,buf0,node->ROOT),
                  ttv_getsigname(ttvfig,buf1,root->ROOT));
                          
          ttv_error(55,buf,TTV_WARNING) ;
          ttv_fifoclean() ;

          return NULL;
        }
      //

      ttv_fifosave() ;
      chain = ttv_findclockpath(ttvfig,ttvfig,latch,root,type|TTV_FIND_CRITIC,1, cmdlatch) ;

    
      for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
        {
          if (chainx->DATA==node) found=1;
          ((ttvevent_list *)chainx->DATA)->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
        }
      freechain(chain) ;

      if (!found)
        {
          char buf0[128], buf1[128], buf[1024], buf3[128];
          ttv_fiforestore() ;
          ttv_fifoclean() ;
          sprintf(buf3," thru latch '%s'",ttv_getsigname(ttvfig,buf0,latch->ROOT));
          sprintf(buf, "could not reach clock '%s' for path from '%s' to '%s'%s",
                  ttv_getsigname(ttvfig,buf0,node->ROOT),
                  ttv_getsigname(ttvfig,buf0,node->ROOT),
                  ttv_getsigname(ttvfig,buf1,root->ROOT),
                  buf3);

          ttv_error(55,buf,TTV_WARNING) ;

          return NULL;
        }
    }

  if((node->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_Q | TTV_SIG_R |
                          TTV_SIG_N|TTV_SIG_B)) == 0)
    node->TYPE &= ~(TTV_NODE_STOP) ;

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      freechain(TTV_CHAIN_CLOCK) ;
      TTV_CHAIN_CLOCK = NULL ;
    }

  // BLINDAGE CORE si node non trouve
  if (node->FIND==NULL)
    {
      char buf0[128], buf1[128], buf[1024], buf3[128];
      if((latch != NULL) && (node != latch))
        {
          ttv_fiforestore() ;
        }
      ttv_fifoclean() ;
      if (latch==NULL) strcpy(buf3,"");
      else sprintf(buf3," thru latch '%s'",ttv_getsigname(ttvfig,buf0,latch->ROOT));
      sprintf(buf, "could not reach clock '%s' for path from '%s' to '%s'%s",
              ttv_getsigname(ttvfig,buf0,node->ROOT),
              ttv_getsigname(ttvfig,buf0,node->ROOT),
              ttv_getsigname(ttvfig,buf1,root->ROOT),
              buf3);

      ttv_error(55,buf,TTV_WARNING) ;

      return NULL;
    }
  //

  if(node->FIND->OUTLINE == NULL && getptype(node->USER,TTV_NODE_LATCH)==NULL) 
    {
      if((latch != NULL) && (node != latch))
        {
          ttv_fiforestore() ;
        }
      ttv_fifoclean() ;
      return(NULL) ;
    }
 
  if (TTV_CHARAC_CRITIC_SLOPES_LIST!=NULL || TTV_CHARAC_CRITIC_CAPAS_LIST!=NULL)
    {
      if (TTV_CHARAC_CRITIC_RESULT_DELAYS==NULL)
        ttv_DoCharacPropagation(ttvfig,figpath,node,root,type,TTV_CHARAC_CRITIC_SLOPES_LIST,TTV_CHARAC_CRITIC_CAPAS_LIST, &TTV_CHARAC_CRITIC_RESULT_DELAYS, &TTV_CHARAC_CRITIC_RESULT_SLOPES, &TTV_CHARAC_CRITIC_RESULT_ENERGY,NULL);
      critic = NULL;
    }
  else
    critic = ttv_savcritic(ttvfig,figpath,root,node,type,0) ;

  if((latch != NULL) && (node != latch))
    {
      ttv_fiforestore() ;
    }

  ttv_fifoclean() ;

  if(critic == NULL)
    return(NULL) ;
  return(critic) ;
}

static ttvpath_stb_stuff *ttv_getcrossvalues_notdual(ttvfig_list *tvf, ttvevent_list *tve, long type, int checkstart)
{
  ttvfind_stb_stuff *tss;
  long startg, start=TTV_NOTIME;
  char startphase;
  ttvpath_stb_stuff *tps, *tpslist=NULL;

  for (tss=tve->FIND->STB; tss!=NULL; tss=tss->NEXT)
    {
      startg = ttv_getinittime(tvf,tve,STB_NO_INDEX,type,startphase=tss->PHASE,NULL) ;
      start=tss->CROSSDELAY_MIN;
      if (tss->CROSSDELAY_MIN!=TTV_NOTIME
          && ((checkstart && getptype(tve->ROOT->USER, STB_IDEAL_CLOCK)!=NULL)
              || ((type & TTV_FIND_MAX)==TTV_FIND_MAX && start>=startg)
              || ((type & TTV_FIND_MIN)==TTV_FIND_MIN && start<=startg))          
          ) start=TTV_NOTIME;

      if (start==TTV_NOTIME)
        start=startg;

      tps=ttv_allocpath_stb_stuff();
      tps->NEXT=tpslist;
      tpslist=tps;
      tps->STARTPHASE=tss->PHASE;
      tps->STARTTIME=start;
      tps->PERIOD_CHANGE=tss->PERIOD_CHANGE;
    }
  
  return tpslist;
}

static ttvpath_stb_stuff *ttv_getcrossvalues_dual(ttvfig_list *tvf, ttvevent_list *tve, ttvevent_list *startnode, long type, int *failed)
{
  ttvfind_stb_stuff *tss;
  long startg, crossmin;
  char startphase;
  ttvpath_stb_stuff *tps, *tpslist=NULL;
  ttvevent_list *tvecheckev;

  *failed=0;

  if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE)==0 && stb_has_filter_directive(tve))
    tvecheckev=tve;
  else
    tvecheckev=tve->FIND->OUTLINE->NODE;

  for (tss=tvecheckev->FIND->STB; tss!=NULL; tss=tss->NEXT)
    {
      if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE)==0
          && (tss->FLAGS & TTV_TPS_FINDSTRUCT_THRU_FILTER_NOCROSS)!=0) continue;
      startg = ttv_getinittime(tvf,startnode,STB_NO_INDEX,type,startphase=tss->STARTPHASE,NULL) ;

      crossmin=tss->CROSSDELAY_MIN;
  
      if (crossmin!=TTV_NOTIME)
        {
          crossmin=startg+crossmin;
          if ((type & TTV_FIND_MAX)==TTV_FIND_MAX)
            {
              if (crossmin>=startg) crossmin=TTV_NOTIME;
              if (crossmin!=TTV_NOTIME && crossmin<ttv_getinittime(tvf,startnode,STB_NO_INDEX,TTV_FIND_MIN,tss->STARTPHASE,NULL)) continue;
            }
          else
            {
              if (crossmin<=startg) crossmin=TTV_NOTIME;
              if (crossmin!=TTV_NOTIME && crossmin>ttv_getinittime(tvf,startnode,STB_NO_INDEX,TTV_FIND_MAX,tss->STARTPHASE,NULL)) continue;
            }
        }
      if (crossmin==TTV_NOTIME) crossmin=startg;
      tps=ttv_allocpath_stb_stuff();
      tps->NEXT=tpslist;
      tpslist=tps;
      tps->STARTPHASE=tss->STARTPHASE;
      tps->STARTTIME=crossmin;
      tps->PERIOD_CHANGE=tss->PERIOD_CHANGE;
    }
  if (tvecheckev->FIND->STB!=NULL && tpslist==NULL) *failed=1;
  return tpslist;
}


static ttvpath_stb_stuff *ttv_getbestcrossvalues(ttvpath_stb_stuff *tps, long type)
{
  ttvpath_stb_stuff *best=NULL;

  while (tps!=NULL)
    {
      if (best==NULL
          || ((type & TTV_FIND_MAX)==TTV_FIND_MAX && best->STARTTIME<tps->STARTTIME)
          || ((type & TTV_FIND_MIN)==TTV_FIND_MIN && best->STARTTIME>tps->STARTTIME)
          ) best=tps;
      tps=tps->NEXT;
    }
  return best;
}

static ttvpath_stb_stuff *ttv_getphasecrossvalues(ttvpath_stb_stuff *tps, ttvevent_list *cmd)
{
  char cmdphase;
  stbnode *n;

  if (cmd!=NULL && (n=stb_getstbnode(cmd))!=NULL && n->CK!=NULL)
    cmdphase=n->CK->CKINDEX;
  else
    cmdphase=STB_NO_INDEX;

  while (tps!=NULL && tps->STARTPHASE!=cmdphase) tps=tps->NEXT;

  return tps;
}

/*****************************************************************************/
/*                        function ttv_savpath()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ttvins : figure instance                                                  */
/* path : liste des chemins                                                  */
/* root : signal source                                                      */
/* node : signal destination                                                 */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les chemins entre ptsig et les signaux de mask             */
/*****************************************************************************/
char ttv_getcmdphase(ttvevent_list *cmd)
{
  stbnode *stbnode;
  char phase;
  if (cmd!=NULL && (stbnode=stb_getstbnode(cmd))!=NULL && stbnode->CK!=NULL)
    return stbnode->CK->CKINDEX;

  return STB_NO_INDEX;
}

ttvpath_list *ttv_savpath(ttvfig,ttvins,path,root,node,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *ttvins ;
     ttvpath_list *path ;
     ttvevent_list *root ;
     ttvevent_list *node ;
     long type ;
{
  ttvevent_list *nodex ; 
  ptype_list *ptype ;
  ttvfig_list *ttvfigx ;
  ttvevent_list *cmd ;
  ttvevent_list *previous_node ;
  long typer ;
  long delayref = (long)0 ;
  long access = (long)0 ;
  long refaccess = (long)0 ;
  long start = 0 ;
  long starts = 0 ;
  long startsref = 0 ;
  long data ;
  long datamax ;
  long datamin, crossmin, per_change=0, clockpathdelay=0;
  ttvevent_list *latch = NULL ;
  ttvevent_list *cmdlatch = NULL ;
  ptype_list *latchlist = NULL, crossvalues ;
  chain_list *chainnode ;
  chain_list *chain ;
  ttvsig_list *tvs;
  stbfig_list *sb;
  int checkloop=0;
  char phase;
  long lcrossmin, oslope;
  ttvline_list *prevline=NULL;
  ttvpath_stb_stuff *tps, *best, *tpslat, *ptps, *phaseinfo;
 
  ttvfigx = node->ROOT->ROOT ;
  sb=stb_getstbfig(ttvfig);

  phaseinfo=NULL;

  nodex = node ;
  chainnode = NULL ;
  if ((V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE>0 || TTV_MaxPathPeriodPrecharge>0) && (type & TTV_FIND_ALL)==0) checkloop=1;
  if((nodex->FIND->OUTLINE == NULL) && 
     ((ptype = getptype(nodex->USER,TTV_NODE_CLOCK)) != NULL))
    {
      delayref += ttv_GET_FINDDELAY(nodex, 0) - ttv_GET_FINDDELAY((ttvevent_list *)ptype->DATA, 0) ;
      nodex = (ttvevent_list *)ptype->DATA ;
      ttvfigx = nodex->ROOT->ROOT ;
    }

  if((nodex->FIND->OUTLINE == NULL) && 
     ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
    {
      cmdlatch = nodex ;
      delayref += ttv_GET_FINDDELAY(nodex, 0) - ttv_GET_FINDDELAY((ttvevent_list *)ptype->DATA, 0) ;
      nodex = (ttvevent_list *)ptype->DATA ;
      latch = nodex ;
      ttvfigx = nodex->ROOT->ROOT ;
    }

  while(nodex->FIND->OUTLINE != NULL)
    {
      if(nodex->FIND->OUTLINE->FIG->INFO->LEVEL > ttvfigx->INFO->LEVEL)
        ttvfigx = nodex->FIND->OUTLINE->FIG ;

      delayref += ttv_GET_LINE_DELAY(nodex->FIND->OUTLINE, type, 1);
      
      if (checkloop)
        {
          nodex->TYPE|=TTV_NODE_MARQUE;
          chainnode=addchain(chainnode, nodex);
        }
      if (TTV_MARK_MODE==TTV_MARK_MODE_DO)
        {
          tvs=nodex->ROOT;
          if (getptype(tvs->USER, TTV_SIG_MARKED)==NULL)
            {
              tvs->USER=addptype(tvs->USER, TTV_SIG_MARKED, NULL);
              MARKED_SIG_LIST=addchain(MARKED_SIG_LIST, tvs);
            }
        }
      if((nodex->FIND->OUTLINE->TYPE & TTV_LINE_A) == TTV_LINE_A
         || (latch==NULL && (nodex->FIND->OUTLINE->ROOT->ROOT->TYPE & TTV_SIG_L)!=0 && (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS)!=0))
        {
          cmdlatch = nodex->FIND->OUTLINE->NODE ;
          latch = nodex->FIND->OUTLINE->ROOT ;
          ttvfigx = latch->ROOT->ROOT ;
          refaccess = ttv_GET_LINE_DELAY(nodex->FIND->OUTLINE, type, 1);
          access = ttv_GET_FINDDELAY(nodex->FIND->OUTLINE->NODE, 0) - ttv_GET_FINDDELAY(nodex->FIND->OUTLINE->ROOT, 0);
        }
      
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          if(nodex->FIND->OUTLINE->NODE == root) break ;
          if((nodex->FIND->OUTLINE->NODE->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)  
            {
              TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
              for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
                ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
              freechain(chainnode) ;
              return(path) ;
            }
          nodex = nodex->FIND->OUTLINE->NODE ;
        }
      else
        {
          if(nodex->FIND->OUTLINE->ROOT == root) break ;
          if((nodex->FIND->OUTLINE->ROOT->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
            {
              TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
              for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
                ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
              freechain(chainnode) ;
              return(path) ;
            }
          prevline=nodex->FIND->OUTLINE;
          nodex = nodex->FIND->OUTLINE->ROOT ;
        }

      if(((nodex->FIND->OUTLINE == NULL) || 
          ((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
        {
          cmdlatch = nodex ;
          delayref += ttv_GET_FINDDELAY(nodex, 0) - ttv_GET_FINDDELAY((ttvevent_list *)ptype->DATA, 0) ;
          nodex = (ttvevent_list *)ptype->DATA ;
          latch = nodex ;
          ttvfigx = latch->ROOT->ROOT ;
        }
      if((((nodex->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) ||
          ((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)) && (nodex != latch))
        {
          char phase;

          if ((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
            stb_geteventphase(sb, nodex,&phase,NULL,NULL,1,1) ;
          else
            {
              if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL && node->FIND->OUTLINE!=NULL)
                cmd = ttv_getlinecmd(ttvfig,node->FIND->OUTLINE,TTV_LINE_CMDMAX) ;
              else if ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL && prevline!=NULL)
                cmd = ttv_getlinecmd(ttvfig,prevline,TTV_LINE_CMDMAX) ;
              else
                cmd=NULL;
              
              phase=ttv_getcmdphase(cmd);
            }

          latchlist = addptype(latchlist,(long)phase,nodex) ;
        }
    }

  if (checkloop)
    {
      for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
        ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
      freechain(chainnode) ;
    }
 
  if (TTV_MARK_MODE==TTV_MARK_MODE_DO) return path;

  previous_node=NULL;

  if((ttvfigx != ttvins) && (ttvins != NULL))
    return(path) ;

  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      int failed;
      typer = (long)0 ;
      if((node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
        {
          previous_node=node->FIND->OUTLINE->NODE;
          if(node->FIND->OUTLINE->TYPE & TTV_LINE_PR) typer = TTV_FIND_PR ;
          else if(node->FIND->OUTLINE->TYPE & TTV_LINE_EV) typer = TTV_FIND_EV ;
        }
      if(((node->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS) ||
         ((node->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR))
        {
          if(node->FIND->OUTLINE->TYPE & TTV_LINE_R) typer = TTV_FIND_R ;
          else if(node->FIND->OUTLINE->TYPE & TTV_LINE_S) typer = TTV_FIND_S ;
        }

      tps=ttv_getcrossvalues_dual(ttvfig, node, root, type, &failed);
      if (failed) return path;
      best=ttv_getbestcrossvalues(tps, type);
      if (best==NULL)
        {
          start = ttv_getinittime(ttvfig,root,STB_NO_INDEX,type,STB_NO_INDEX,&phase) ;
          crossmin=TTV_NOTIME;
        }
      else
        {
          start = ttv_getinittime(ttvfig,root,STB_NO_INDEX,type,best->STARTPHASE,&phase) ;
          crossmin=best->STARTTIME;
        }
      starts = ttv_getnodeslope(ttvfig,ttvins,root,&startsref,type) ;

      if(latch != NULL)
        {
          stb_getstbdelta(sb, latch, &datamin, &datamax, cmdlatch); 
          if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            data = datamax ;
          else
            data = datamin;
          clockpathdelay=ttv_GET_FINDDELAY(latch, 0);
          // modif pour le crossmin non mis en dual raison: pas d'access en dual
        }
      else data = (long)0 ;

      if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_KEEP_PHASE_INFO)!=0)
        phaseinfo=tps;
      else
        ttv_freepathstblist(tps);

      cmd=NULL;
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
        {
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0)
            cmd = ttv_getlinecmd(ttvfig,node->FIND->OUTLINE,TTV_LINE_CMDMAX) ;
          oslope=ttv_getslopemax(node->FIND->OUTLINE);
        }
      else
        {
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0)
            cmd = ttv_getlinecmd(ttvfig,node->FIND->OUTLINE,TTV_LINE_CMDMAX) ;
          oslope=ttv_getslopemin(node->FIND->OUTLINE);
        }


//          start = ttv_getinittime(ttvfig,root,STB_NO_INDEX,type,STB_NO_INDEX,&phase) ;

/*          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE)==0 && stb_has_filter_directive(node))
            crossmin=node->FIND->CROSSDELAY_MIN;
          else
            crossmin=node->FIND->OUTLINE->NODE->FIND->CROSSDELAY_MIN;
*/
//          per_change=node->FIND->PERIOD_CHANGE;
/*
          if (crossmin!=TTV_NOTIME)
            {
              if (crossmin>=start) crossmin=TTV_NOTIME;
              else crossmin=start+crossmin;
              if (crossmin!=TTV_NOTIME && crossmin<ttv_getinittime(ttvfig,root,STB_NO_INDEX,TTV_FIND_MIN,STB_NO_INDEX,NULL)) return path;
            }
  */   
          path = ttv_allocpath(path,ttvfigx,node,root,cmd,
                               latch,cmdlatch,latchlist,data,access,refaccess,
                               type|typer,delayref,ttv_GET_LINE_SLEW(node->FIND->OUTLINE, type, 1),
                               ttv_GET_FINDDELAY(node, 0),oslope,
                               start,starts,NULL,NULL, crossmin,phase,clockpathdelay) ;
    }
  else
    {
      for(nodex = node ; nodex->FIND->OUTLINE != NULL ; nodex = nodex->FIND->OUTLINE->ROOT)
        if(nodex->FIND->OUTLINE->ROOT == root) break ;

      if(latchlist != NULL)
        latchlist = (ptype_list *)reverse((chain_list *)latchlist) ;
      if((nodex->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_CLOCK)) != NULL))
        {
          nodex = (ttvevent_list *)ptype->DATA ;
          for(; nodex->FIND->OUTLINE != NULL ; nodex = nodex->FIND->OUTLINE->ROOT)
            if(nodex->FIND->OUTLINE->ROOT == root) break ;
        }
      if((nodex->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
        {
          nodex = (ttvevent_list *)ptype->DATA ;
          for(; nodex->FIND->OUTLINE != NULL ; nodex = nodex->FIND->OUTLINE->ROOT)
            if(nodex->FIND->OUTLINE->ROOT == root) break ;
        }

      if(nodex->FIND->OUTLINE == NULL) nodex = node ;
      typer = (long)0 ;
      if((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
        {
          previous_node=nodex;
          if(nodex->FIND->OUTLINE->TYPE & TTV_LINE_PR) typer = TTV_FIND_PR ;
          else if(nodex->FIND->OUTLINE->TYPE & TTV_LINE_EV) typer = TTV_FIND_EV ;
        }

      if(((root->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS) ||
         ((root->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR))
        {
          if(nodex->FIND->OUTLINE->TYPE & TTV_LINE_R) typer = TTV_FIND_R ;
          else if(nodex->FIND->OUTLINE->TYPE & TTV_LINE_S) typer = TTV_FIND_S ;
        }
//      per_change=node->FIND->PERIOD_CHANGE;
      tps=ttv_getcrossvalues_notdual(ttvfig, node, type, 1);
      best=ttv_getbestcrossvalues(tps, type);
      if(latch != NULL)
        {
          long oldmode;
          oldmode=TTV_MORE_SEARCH_OPTIONS;
          TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_USE_CLOCK_START;
          TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_USE_DATA_START;

          stb_getstbdelta(sb, latch, &datamin, &datamax, cmdlatch); 


          if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            data = datamax ;
          else
            data=datamin;
          clockpathdelay=ttv_GET_FINDDELAY(node, 0)-ttv_GET_FINDDELAY(latch, 0);
          tpslat=ttv_getcrossvalues_notdual(ttvfig, latch, type, 0);
          ptps=ttv_getphasecrossvalues(tpslat, cmdlatch);
          if (ptps!=NULL)
            lcrossmin=ptps->STARTTIME;
          else
            lcrossmin=TTV_NOTIME;
          if (lcrossmin!=TTV_NOTIME)
            {
              phase=ttv_getcmdphase(cmdlatch);
              start = ttv_getinittime(ttvfig,latch,STB_NO_INDEX,type,phase,NULL) ;
              if (lcrossmin>=start)
                lcrossmin=TTV_NOTIME;
              if (lcrossmin!=TTV_NOTIME)
                data=data-(start-lcrossmin); // lag qui cross
            }
          TTV_MORE_SEARCH_OPTIONS=oldmode;
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_KEEP_PHASE_INFO)!=0)
            phaseinfo=tpslat;
          else
            ttv_freepathstblist(tpslat);
        }
      else
        data=0;
      starts = ttv_getnodeslope(ttvfig,ttvins,node,&startsref,type) ;
      if (best==NULL)
        {
          start = ttv_getinittime(ttvfig,node,STB_NO_INDEX,type,STB_NO_INDEX,&phase) ;
          crossmin=TTV_NOTIME;
        }
      else
        {
          start = ttv_getinittime(ttvfig,node,STB_NO_INDEX,type,best->STARTPHASE,&phase) ;
          crossmin=best->STARTTIME;
        }

      if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_KEEP_PHASE_INFO)!=0 && latch==NULL)
        phaseinfo=tps;
      else
        ttv_freepathstblist(tps);
/*
          if (node->FIND->CROSSDELAY_MIN!=TTV_NOTIME
              && (node->FIND->CROSSDELAY_MIN>=start
                  || getptype(node->ROOT->USER, STB_IDEAL_CLOCK)!=NULL)
              ) node->FIND->CROSSDELAY_MIN=TTV_NOTIME;
*/
      
      cmd=NULL;
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
        {
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0)
            cmd = ttv_getlinecmd(ttvfig,nodex->FIND->OUTLINE,TTV_LINE_CMDMAX) ;
          oslope=ttv_getslopemax(nodex->FIND->OUTLINE);
        }
      else
        {
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0)
            cmd = ttv_getlinecmd(ttvfig,nodex->FIND->OUTLINE,TTV_LINE_CMDMAX) ;
          oslope=ttv_getslopemin(nodex->FIND->OUTLINE);
        }
      path = ttv_allocpath(path,ttvfigx,root,node,cmd,
                           latch,cmdlatch,latchlist,data,access,refaccess,
                           type|typer,delayref,ttv_GET_LINE_SLEW(nodex->FIND->OUTLINE, type, 1),
                           ttv_GET_FINDDELAY(node, 0),oslope,
                           start,starts,NULL,NULL, crossmin,phase,clockpathdelay) ;
    }

  path->TTV_MORE_SEARCH_OPTIONS=TTV_MORE_SEARCH_OPTIONS;
  if (node->FIND->THRU_FILTER) path->TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_FLAG_THRU_FILTER_FOUND;
  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE!=0) path->TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_FLAG_PROGATE_USED;

  if (phaseinfo!=NULL)
    path->USER=addptype(path->USER, TTV_PATH_PHASE_INFO, phaseinfo);

  if (previous_node!=NULL)
    path->USER=addptype(path->USER, TTV_PATH_PREVIOUS_NODE, previous_node);
/*  if (per_change!=0)
    path->USER=addptype(path->USER, TTV_PATH_PERIOD_CHANGE, (void *)per_change);*/
  if (node->FIND->ORGPHASE!=TTV_NO_PHASE)
    path->USER=addptype(path->USER, TTV_PATH_OUTPUT_PHASE, (void *)(long)node->FIND->ORGPHASE);
  if((type & TTV_FIND_CARAC) == TTV_FIND_CARAC) 
    {
      if (V_BOOL_TAB[__TMA_CHARAC_PRECISION].VALUE)
        ttv_allocpathmodel_propagate(ttvfig, ttvins, node, root,path,type);
      else
        ttv_allocpathmodel(ttvfig,path,type) ;
    }
  if((type & TTV_FIND_PATHDET) == TTV_FIND_PATHDET || (type & TTV_FIND_ALL)!=0)
    path->CRITIC = ttv_savcritic(ttvfig,ttvins,root,node,type,1) ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_savcritic()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* path : liste des chemins                                                  */
/* root : signal source                                                      */
/* node : signal destination                                                 */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les chemins entre ptsig et les signaux de mask             */
/*****************************************************************************/
static inline char *getlinemodel(ttvline_list *tl, long type)
{
  if (type & TTV_FIND_MAX)
    return tl->MDMAX;
  return tl->MDMIN;
}

ttvcritic_list *ttv_savcritic(ttvfig,ttvins,root,node,type,ignorepropagate)
     ttvfig_list *ttvfig ;
     ttvfig_list *ttvins ;
     ttvevent_list *root ;
     ttvevent_list *node ;
     long type ;
     int ignorepropagate;
{
  ttvfig_list *ttvfigx ;
  ttvevent_list *nodex, *cmdlatch, *cmd ;
  ttvcritic_list *critic = NULL ;
  ttvline_list *ptline ;
  ptype_list *ptype ;
  long slope ;
  long newslope ;
  long delay ;
  long olddelay ;
  long typeline ;
  long start ;
  long data = 0 ;
  long datamax ;
  long datamin, lcrossmin ;
  long start0;
  char nodeflags=0;
  chain_list *line_list;
  stbfig_list *sb;
  char *lastlinemodel=NULL;
  ttvline_list *lastline;
  long oldmode;
  ttvpath_stb_stuff *tpslat, *ptps;
  char phase;

  sb=stb_getstbfig(ttvfig);
  type&=~TTV_FIND_PATHDET;

  if (!ignorepropagate && V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
    line_list=ttv_DoPropagation_PRECISION_LEVEL1(ttvfig, ttvins, node, root, type);

  nodex = node ;

  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
    {
      newslope = (long)ttv_getnodeslope(ttvfig,ttvins,node,&slope,type) ;
      typeline = (long) 0 ;
      ttvfigx = NULL ;
      delay = ttv_GET_FINDDELAY(node, 0);
      olddelay = (long) 0 ;
      start = ttv_getinittime(ttvfig,node,STB_NO_INDEX,type,STB_NO_INDEX,NULL) ;
      lastline=NULL;
      if((nodex->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
        {
          critic = ttv_alloccritic(critic,ttvfig,ttvfigx,nodex,type|typeline,
                                   data,olddelay + start,slope,
                                   delay - ttv_GET_FINDDELAY(nodex, 0) + start,newslope,nodeflags,lastlinemodel,NULL) ;
          start = (long)0 ;
          nodeflags=0;
          olddelay = ttv_GET_FINDDELAY(nodex, 0) - ttv_GET_FINDDELAY((ttvevent_list *)ptype->DATA, 0) ;
          cmdlatch=nodex;
          nodex = (ttvevent_list *)ptype->DATA ;


          stb_getstbdelta(sb, nodex, &datamin, &datamax, cmdlatch);
          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            data = datamax ;
          else
            data = datamin ;

          oldmode=TTV_MORE_SEARCH_OPTIONS;
          TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_USE_CLOCK_START;
          TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_USE_DATA_START;

          tpslat=ttv_getcrossvalues_notdual(ttvfig, nodex, type, 0);
          ptps=ttv_getphasecrossvalues(tpslat, cmdlatch);
          if (ptps!=NULL)
            lcrossmin=ptps->STARTTIME;
          else
            lcrossmin=TTV_NOTIME;

          ttv_freepathstblist(tpslat);
//lcrossmin=nodex->FIND->CROSSDELAY_MIN;
          if (lcrossmin!=TTV_NOTIME)
            {
              phase=ttv_getcmdphase(cmdlatch);

              start0 = ttv_getinittime(ttvfig,nodex,STB_NO_INDEX,type,phase,NULL) ;
              if (((type & TTV_FIND_MAX)!=0 && lcrossmin>=start0)
                  || ((type & TTV_FIND_MIN)!=0 && lcrossmin<=start0))
                lcrossmin=TTV_NOTIME;
              if (lcrossmin!=TTV_NOTIME)
                data=data-(start0-lcrossmin); // lag qui cross
            }
          TTV_MORE_SEARCH_OPTIONS=oldmode;
          nodeflags=TTV_NODE_FLAG_ISLATCH_ACCESS;
        }
      ptline = nodex->FIND->OUTLINE ;
      while(nodex->FIND->OUTLINE != NULL)
        {
          if(((type & TTV_FIND_LINE) == TTV_FIND_LINE) || (nodex == node))
            {
              critic = ttv_alloccritic(critic,ttvfig,ttvfigx,nodex,type|typeline,
                                       data,olddelay + start,slope,
                                       delay - ttv_GET_FINDDELAY(nodex, 0) + start,newslope,nodeflags,lastlinemodel,lastline) ;
              nodeflags=0;
              data = (long)0 ;
              start = (long)0 ;
            }
          ttv_getmodelline(ptline,&critic->MODNAME,&critic->INSNAME) ;
          ptline = nodex->FIND->OUTLINE ;
          lastline = ptline;
          lastlinemodel=getlinemodel(ptline, type);
          typeline = TTV_LINETYPE(nodex->FIND->OUTLINE) ;
          slope = ttv_GET_LINE_SLEW(nodex->FIND->OUTLINE, type, 1) ;
          olddelay = ttv_GET_LINE_DELAY(nodex->FIND->OUTLINE, type, 1) ;
          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            newslope = ttv_getslopemax(nodex->FIND->OUTLINE) ;
          else
            newslope = ttv_getslopemin(nodex->FIND->OUTLINE) ;

          if((nodex->FIND->OUTLINE->TYPE & TTV_LINE_A) == TTV_LINE_A
             || ((nodex->FIND->OUTLINE->ROOT->ROOT->TYPE & TTV_SIG_R)!=0 && getptype(nodex->FIND->OUTLINE->ROOT->USER,TTV_NODE_LATCH)!=NULL))
            {
              stb_getstbdelta(sb, nodex->FIND->OUTLINE->ROOT, &datamin, &datamax, nodex);
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                data = datamax ;
              else
                data = datamin ;

              oldmode=TTV_MORE_SEARCH_OPTIONS;
              TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_USE_CLOCK_START;
              TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_USE_DATA_START;

              tpslat=ttv_getcrossvalues_notdual(ttvfig, nodex->FIND->OUTLINE->ROOT, type, 0);
              ptps=ttv_getphasecrossvalues(tpslat, nodex);
              if (ptps!=NULL)
                lcrossmin=ptps->STARTTIME;
              else
                lcrossmin=TTV_NOTIME;
              
              ttv_freepathstblist(tpslat);

//              lcrossmin=nodex->FIND->OUTLINE->ROOT->FIND->CROSSDELAY_MIN;
              if (lcrossmin!=TTV_NOTIME)
                {

                  phase=ttv_getcmdphase(nodex);
                  start0 = ttv_getinittime(ttvfig,nodex->FIND->OUTLINE->ROOT,STB_NO_INDEX,type,phase,NULL) ;
                  if (((type & TTV_FIND_MAX)!=0 && lcrossmin>=start0)
                      || ((type & TTV_FIND_MIN)!=0 && lcrossmin<=start0))
                    lcrossmin=TTV_NOTIME;
                  if (lcrossmin!=TTV_NOTIME)
                    data=data-(start0-lcrossmin); // lag qui cross
                }
              TTV_MORE_SEARCH_OPTIONS=oldmode;
              nodeflags=TTV_NODE_FLAG_ISLATCH_ACCESS;
            }
      
          delay = ttv_GET_FINDDELAY(nodex, 0);
          ttvfigx = nodex->FIND->OUTLINE->FIG ;
          nodex = nodex->FIND->OUTLINE->ROOT ;
          if((nodex->FIND->OUTLINE == NULL) && 
             ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
            {
              critic = ttv_alloccritic(critic,ttvfig,ttvfigx,nodex,type|typeline,
                                       data,olddelay + start,slope,
                                       delay - ttv_GET_FINDDELAY(nodex, 0) + start,newslope,nodeflags,lastlinemodel,lastline) ;
              typeline=0;
              start = (long)0 ;
              nodeflags=0;
              lastlinemodel=NULL;
              lastline=NULL;
              olddelay = ttv_GET_FINDDELAY(nodex, 0) - ttv_GET_FINDDELAY((ttvevent_list *)ptype->DATA, 0) ;
              delay = ttv_GET_FINDDELAY(nodex, 0) ;
              cmdlatch=nodex;
              nodex = (ttvevent_list *)ptype->DATA ;
              stb_getstbdelta(sb, nodex, &datamin, &datamax, cmdlatch);
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                data = datamax ;
              else
                data = datamin ;

              oldmode=TTV_MORE_SEARCH_OPTIONS;
              TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_USE_CLOCK_START;
              TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_USE_DATA_START;

              tpslat=ttv_getcrossvalues_notdual(ttvfig, nodex, type, 0);
              ptps=ttv_getphasecrossvalues(tpslat, cmdlatch);
              if (ptps!=NULL)
                lcrossmin=ptps->STARTTIME;
              else
                lcrossmin=TTV_NOTIME;
              
              ttv_freepathstblist(tpslat);
//              lcrossmin=nodex->FIND->CROSSDELAY_MIN;
              if (lcrossmin!=TTV_NOTIME)
                {
                  phase=ttv_getcmdphase(cmdlatch);
                  start0 = ttv_getinittime(ttvfig,nodex,STB_NO_INDEX,type,phase,NULL) ;
                  if (((type & TTV_FIND_MAX)!=0 && lcrossmin>=start0)
                      || ((type & TTV_FIND_MIN)!=0 && lcrossmin<=start0))
                    lcrossmin=TTV_NOTIME;
                  if (lcrossmin!=TTV_NOTIME)
                    data=data-(start0-lcrossmin); // lag qui cross
                }
              TTV_MORE_SEARCH_OPTIONS=oldmode;
              nodeflags=TTV_NODE_FLAG_ISLATCH_ACCESS;
            }
          if(nodex == root) break ;
        }
      critic = ttv_alloccritic(critic,ttvfig,ttvfigx,nodex,type|typeline,
                               data,olddelay + start,slope,delay + start,newslope,nodeflags,lastlinemodel, ptline) ;   
      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)  cmd = ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMAX) ;
      else cmd = ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMIN) ;
   
      nodeflags=0;
      start = (long)0 ;
      data = (long)0 ;
      ttv_getmodelline(ptline,&critic->MODNAME,&critic->INSNAME) ;
      critic = ((ttvcritic_list *)reverse((chain_list *)critic));
    }
  else
    {
      start = ttv_getinittime(ttvfig,root,STB_NO_INDEX,type,STB_NO_INDEX,NULL) ;
      while(nodex->FIND->OUTLINE != NULL)
        {
          slope = ttv_GET_LINE_SLEW(nodex->FIND->OUTLINE, type, 1) ;
          olddelay = ttv_GET_LINE_DELAY(nodex->FIND->OUTLINE, type, 1) ;
          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            newslope = ttv_getslopemax(nodex->FIND->OUTLINE) ;
          else
            newslope = ttv_getslopemin(nodex->FIND->OUTLINE) ;
          delay = ttv_GET_FINDDELAY(nodex->FIND->OUTLINE->NODE, 0) ;
          ttvfigx = nodex->FIND->OUTLINE->FIG ;
          typeline = TTV_LINETYPE(nodex->FIND->OUTLINE) ;
          ptline = nodex->FIND->OUTLINE ;
          if(((type & TTV_FIND_LINE) == TTV_FIND_LINE) || (nodex == node)) 
            {
              if(root == nodex->FIND->OUTLINE->NODE)
                critic = ttv_alloccritic(critic,ttvfig,ttvfigx,nodex,type|typeline,
                                         (long)0,olddelay,slope,ttv_GET_FINDDELAY(nodex, 0),newslope,nodeflags,getlinemodel(ptline, type),ptline) ;
              else
                critic = ttv_alloccritic(critic,ttvfig,ttvfigx,nodex,type|typeline,
                                         (long)0,olddelay,slope,ttv_GET_FINDDELAY(nodex, 0)-delay,newslope,nodeflags,getlinemodel(ptline, type),ptline) ;
              if (node==nodex)
                {
                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)  cmd = ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMAX) ;
                  else cmd = ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMIN) ;
                }

            }
          nodeflags=0;
          ttv_getmodelline(ptline,&critic->MODNAME,&critic->INSNAME) ;
          nodex = nodex->FIND->OUTLINE->NODE ;
          if(nodex == root) break ;
        }
      newslope = ttv_getnodeslope(ttvfig,ttvins,nodex,&slope,type) ;
      critic = ttv_alloccritic(critic,ttvfig,NULL,nodex,type,
                               (long)0,start,slope,ttv_GET_FINDDELAY(nodex, 0) - delay + start,newslope,nodeflags,getlinemodel(ptline, type),NULL) ;
      nodeflags=0;
      ttv_getmodelline(ptline,&critic->MODNAME,&critic->INSNAME) ;
    }

  if (!ignorepropagate && V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
    ttv_CleanPropagation_PRECISION_LEVEL1(line_list);

  if(nodex != root) 
    {
      ttv_freecriticlist(critic) ;
      return(NULL) ;
    }

  if (critic!=NULL) critic->NODE_FLAG|=TTV_NODE_FLAG_FIRST_OF_DETAIL;
  return(critic) ;
}

/*****************************************************************************/
/*                        function ttv_keepnbpath()                          */
/* parametres :                                                              */
/* path : liste des chemins                                                  */
/* nb : nombre de chemin a garder                                            */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche le chemin le plus court ou le plus long d'une ttvfig            */
/*****************************************************************************/
ttvpath_list *ttv_keepnbpath(ttvpath_list *path,int nb,int *nbpath,long type,LOCAL_SORT_INFO *sort_info, int dontcarehz)
{
  ttvpath_list *pathx ;
  ttvpath_list *pathh ;
  ttvpath_list *pathr ;
  ttvpath_list *pathxx ;
  int nx ;
  int nbe ;

  if(nb < 100)
    nx = 0 ;
  else
    nx = nb ;

  if((type & (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_UPDW|
              TTV_FIND_NOT_DWUP|TTV_FIND_NOT_DWDW)) != 0 && 
     (!dontcarehz || !((path->ROOT->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ || (path->ROOT->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT)))
    {
      pathxx = path->NEXT ;
      path->NEXT = NULL ;
      if((path = ttv_filterpath(path,type,nbpath)) != NULL)
        {
          path->NEXT = pathxx ;
        }
      else
        {
          path = pathxx ;
          return(path) ;
        }
    }
  if(path == NULL)
    return(NULL) ;
  if(path->NEXT != NULL)
    {
      if(*nbpath == nb)
        {
          if(nx == 0)
            {
              pathx = path->NEXT ;
              path->NEXT = pathx->NEXT ;
              pathx->NEXT = NULL ;
              ttv_freepathlist(pathx) ;
              pathxx = path ;
              for(pathx = path->NEXT ; pathx != NULL ; pathx = pathx->NEXT)
                {
                  if(((path->DELAY <= pathx->DELAY) &&
                      ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                     ((path->DELAY >= pathx->DELAY) &&
                      ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
                    break ;
                  pathxx = pathx ;
                }
              if(pathxx != path)
                {
                  pathx = path ;
                  path = path->NEXT ;
                  pathx->NEXT = pathxx->NEXT ;
                  pathxx->NEXT = pathx ;
                }

              return(path) ;
            }
          else
            {
              (*nbpath)++ ;
              if(((path->DELAY > path->NEXT->DELAY) &&
                  ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                 ((path->DELAY < path->NEXT->DELAY) &&
                  ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
                {
                  pathx = path->NEXT ;
                  path->NEXT = path->NEXT->NEXT ;
                  pathx->NEXT = path ;
                  path = pathx ;
                  sort_info->before_paths=(void *)1;
                }
              else
                sort_info->before_paths=NULL;
              sort_info->paths = path ;
              return(path) ;
            }
        }
      else if(*nbpath < nb)
        {
          (*nbpath)++ ;
          if(*nbpath == nb)
            path = ttv_classpath(path,((type & TTV_FIND_MAX) == TTV_FIND_MAX) ? 
                                 TTV_FIND_MIN : TTV_FIND_MAX) ;
          return(path) ;
        }
      else if(*nbpath < (nb + nx - 1))
        {
          (*nbpath)++ ;
          if(((path->DELAY > path->NEXT->DELAY) &&
              ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
             ((path->DELAY < path->NEXT->DELAY) &&
              ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
            {
              pathx = path->NEXT ;
              path->NEXT = path->NEXT->NEXT ;
              pathx->NEXT = path ;
              path = pathx ;
              if (path==sort_info->paths) sort_info->paths=path->NEXT;
              if (path==sort_info->before_paths) sort_info->before_paths=path->NEXT;
            }
          if (sort_info->before_paths==(void *)1) sort_info->before_paths=path;
          return(path) ;
        }
      else
        {
          (*nbpath)++ ;
          if(((path->DELAY > path->NEXT->DELAY) &&
              ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
             ((path->DELAY < path->NEXT->DELAY) &&
              ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
            {
              pathx = path->NEXT ;
              path->NEXT = path->NEXT->NEXT ;
              pathx->NEXT = path ;
              path = pathx ;
              if (path==sort_info->paths) sort_info->paths=path->NEXT;
              if (path==sort_info->before_paths) sort_info->before_paths=path->NEXT;
            }
          if (sort_info->before_paths!=NULL)
            {
              pathxx = path->NEXT;
              path->NEXT = sort_info->paths->NEXT->NEXT;
              sort_info->paths->NEXT->NEXT = pathxx;
              pathxx =  sort_info->paths->NEXT;
              sort_info->paths->NEXT = path;
              path = pathxx;
            }

          pathx = sort_info->paths->NEXT ;
          sort_info->paths->NEXT = NULL ;
          path = ttv_classpath(path,((type & TTV_FIND_MAX) == TTV_FIND_MAX) ?
                               TTV_FIND_MIN : TTV_FIND_MAX) ;
          nbe = 0 ;
          pathr = NULL ;
          while(pathx != NULL || path != NULL)
            {
              if(nbe < nx)
                {
                  nbe ++ ;
                  if(((path->DELAY > pathx->DELAY) &&
                      ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                     ((path->DELAY < pathx->DELAY) &&
                      ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
                    {
                      pathxx = pathx ;
                      pathx = pathx->NEXT ;
                      pathxx->NEXT = NULL ;
                      ttv_freepathlist(pathxx) ;
                    }
                  else
                    {
                      pathxx = path ;
                      path = path->NEXT ;
                      pathxx->NEXT = NULL ;
                      ttv_freepathlist(pathxx) ;
                    }
                }
              else
                {
                  if((path!=NULL) && ((pathx == NULL) || (((path->DELAY < pathx->DELAY) &&
                                                           ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                                                          ((path->DELAY > pathx->DELAY) &&
                                                           ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))))
                    {
                      pathxx = pathx ;
                      pathx = path ;
                      path = pathxx ;
                    }

                  if(pathr == NULL)
                    {
                      pathr = pathx ;
                      pathh = pathr ;
                      pathx = pathx->NEXT ;
                      pathr->NEXT = NULL ;
                    }
                  else
                    {
                      pathr->NEXT = pathx ;
                      pathxx = pathx->NEXT ;
                      pathx->NEXT = NULL ;
                      pathr = pathx ;
                      pathx = pathxx ;
                    }
                }
            }
          *nbpath = nb ;
          return(pathh) ;
        }
    }
  else
    {
      *nbpath = 1;
      return(path) ;
    }
}

/*****************************************************************************/
/*                        function ttv_savpathlist()                         */
/* parametres :                                                              */
/* path : list des chemins                                                   */
/* root : evenement de debut                                                 */
/* type : type de recherche                                                  */
/* chainnode : liste des fins                                                */
/*                                                                           */
/* construit la liste des chemins                                            */
/*****************************************************************************/
ttvpath_list *ttv_savpathlist(ttvfig,figpath,path,root,type,level,chainnode,delaymax,delaymin,savall,nb,nbcur)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvpath_list *path ;
     ttvevent_list *root ;
     long type ;
     long level ;
     chain_list *chainnode ;
     long delaymax ;
     long delaymin ;
     int savall ;
     int nb;
     int *nbcur;
{
  chain_list *chain ;
  chain_list *chainx ;
  chain_list *chainin ;
  chain_list *chainx2, *line_list, *thru_command_list ;
  ptype_list *pt, *ptype, *ptypelatch;
  ttvline_list *line;
  long delay;
  ttvevent_list *firstnode, *possiblelatchnode;
  int ret;
 
  if (((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ONLYNOTHZ)!=0 && (type & TTV_FIND_HZ)!=0)
      || ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ONLYHZ)!=0 && (type & TTV_FIND_HZ)==0)) return path;

  ttv_fifosave() ;                                                                                     
                                                                                                      
  for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)                                         
    {                                                                                                 
      ttvevent_list *node = (ttvevent_list *)chain->DATA ;
      possiblelatchnode=node;
      node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;

      if((node->ROOT->TYPE & (TTV_SIG_MASKOUT | TTV_SIG_MASKIN)) == 0)
        {  
          if ((node->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_L|TTV_SIG_B))!=0 && ((savall & SAVE_ALL_LATCH_TAG)==0))
            continue ;
          if ((node->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_L|TTV_SIG_B))==0 && ((savall & SAVE_ALL_INPUT_TAG)==0))
            continue ;
        }

      if(TTV_CHAIN_CLOCK==NULL || (type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
        {
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS)!=0 && TTV_CHAIN_CLOCK!=NULL && (node->ROOT->TYPE & TTV_SIG_R)==0)
            {
              pt=ttv_getlatchaccess(ttvfig,node,type);
              thru_command_list=NULL;
              for (ptype=pt; ptype!=NULL; ptype=ptype->NEXT)
                thru_command_list=addchain(thru_command_list, ptype->DATA);
              freeptype(pt);
            }
          else thru_command_list=addchain(NULL, NULL);

          while (thru_command_list!=NULL)
            {
              if (TTV_CHAIN_CLOCK!=NULL)
                {
                  if ((node->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_L|TTV_SIG_B))!=0)
                    chainin = ttv_findclockpath(ttvfig,ttvfig,node,root,type,1,(ttvevent_list *)thru_command_list->DATA) ;
                  else
                    {
                      if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS)==0/*(savall & SAVE_ALL_LATCH_TAG)==0*/ || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_PATH_AND_ACCESS)!=0) // pas de vrai access
                        chainin = addchain(NULL,node) ;
                      else
                        chainin = NULL;
                    }
                }
              else if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS)==0)
                chainin = addchain(NULL,node) ;
              else
                chainin = NULL;

              if (TTV_QUIET_MODE==0 && chainin==NULL && (node->ROOT->TYPE & (TTV_SIG_B|TTV_SIG_C|TTV_SIG_Q))==0)
                {
                  char buf0[128];
                  ttv_error(56,ttv_getsigname(ttvfig,buf0,node->ROOT),TTV_WARNING);
                }

              for(chainx = chainin ; chainx != NULL ; chainx = chainx->NEXT)
                {
                  ttvevent_list *node = (ttvevent_list *)chainx->DATA ;
                  node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;

                  if (TTV_CHAIN_CLOCK!=NULL && (ret=ttv_isfalsepath(ttvfig,node,root,type,0,possiblelatchnode!=node?possiblelatchnode:NULL))!=0)
                    {
                      if (ret==1) TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
                      continue;
                    }

                  // repropagate delay
            
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && nbcur!=NULL && nb!=-1 && path!=NULL && *nbcur>=nb)
                    {
                      if((path->DELAY >= node->FIND->DELAY*TTV_HIGH_RATIO && 
                          (type & TTV_FIND_MAX) == TTV_FIND_MAX) ||
                         (path->DELAY <= node->FIND->DELAY*TTV_LOW_RATIO && 
                          (type & TTV_FIND_MIN) == TTV_FIND_MIN))
                        {
                          if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                            node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                          continue;
                        }
                    }

                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    line_list=ttv_DoPropagation_PRECISION_LEVEL1(ttvfig, figpath, node, root, type);
                  /*            
                                if (nb!=-1 && nbcur!=NULL && path!=NULL && *nbcur>=nb)
                                {
                                if((path->DELAY >= ttv_GET_FINDDELAY(node, 0) && 
                                (type & TTV_FIND_MAX) == TTV_FIND_MAX) ||
                                (path->DELAY <= ttv_GET_FINDDELAY(node, 0) && 
                                (type & TTV_FIND_MIN) == TTV_FIND_MIN))
                                {
                                if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                                node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                                continue;
                                }
                                }
                  */

                  if((ttv_GET_FINDDELAY(node, 0) < delaymin) || (ttv_GET_FINDDELAY(node, 0) > delaymax))
                    {
                      if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                        node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                      if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                        ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                      continue ;
                    }
            
                  path = ttv_savpath(ttvfig,figpath,path,root,node,type) ;
                  if (nbcur!=NULL) (*nbcur)++;

                  if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                    node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                }
              freechain(chainin); 
              thru_command_list=delchain(thru_command_list, thru_command_list);
              if (thru_command_list!=NULL) ttv_fifoclean() ;
            }
        }
      else
        {
          firstnode=node;
          if ((firstnode->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_B)) != 0)
            ptypelatch=ttv_getcommandaccess(ttvfig,firstnode,type) ;
          else if((firstnode->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
            ptypelatch=addptype(NULL, (long)NULL, firstnode);
          else
            ptypelatch=NULL;

          ttv_markdatapath(root, node, type, 1);

          for(pt = ptypelatch ; pt != NULL ; pt = pt->NEXT)
            {
              ttvevent_list *node = (ttvevent_list *)pt->DATA ;

              ttv_fifopush(node);          
          
              if ((firstnode->ROOT->TYPE & TTV_SIG_R) != 0)
                {
                  // precharge
                  if((ptype = getptype(node->USER,TTV_NODE_LATCH)) == NULL)
                    node->USER = addptype(node->USER,TTV_NODE_LATCH,firstnode) ;
                  else
                    ptype->DATA = (void *)node ;

                  if(node->FIND->NEXT != NULL)
                    {
                      if(node->FIND->DELAY == TTV_NOTIME)
                        {
                          if(node->FIND->NEXT->DELAY != TTV_NOTIME)
                            {
                              node->FIND->DELAY = node->FIND->NEXT->DELAY ;
                              node->FIND->OUTLINE = node->FIND->NEXT->OUTLINE ;
                            }
                        }
                    }
                }
              else 
                {
                  if(pt->TYPE != (long)0)
                    {
                      line = (ttvline_list *)pt->TYPE ;
                      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                        {
                          if ((delay=stb_gettruncatedaccess(line->ROOT, line->NODE, 1))==TTV_NOTIME)
                            delay = ttv_getdelaymax(line) ;
                        }
                      else
                        {                         
                          if ((delay=stb_gettruncatedaccess(line->ROOT, line->NODE, 0))==TTV_NOTIME)
                            delay = ttv_getdelaymin(line) ;
                        }
                    }
                  else
                    {
                      line = NULL ;
                      delay = (long)0 ;
                    }
                  if((ptype = getptype(node->USER,TTV_NODE_LATCH)) == NULL)
                    node->USER = addptype(node->USER,TTV_NODE_LATCH,firstnode) ;
                  else
                    ptype->DATA = (void *)firstnode ;
                  node->FIND->DELAY = delay + firstnode->FIND->DELAY ;
                  node->FIND->OUTLINE = line ;              
                }
          
              chainin = ttv_findpath(ttvfig,ttvfig,node,NULL,type,1) ;
          
              for(chainx2 = chainin ; chainx2 != NULL ; chainx2 = chainx2->NEXT)
                {
                  ttvevent_list *node = (ttvevent_list *)chainx2->DATA ;
                  node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;

                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && nbcur!=NULL && nb!=-1 && path!=NULL && *nbcur>=nb)
                    {
                      if((path->DELAY >= node->FIND->DELAY*TTV_HIGH_RATIO && 
                          (type & TTV_FIND_MAX) == TTV_FIND_MAX) ||
                         (path->DELAY <= node->FIND->DELAY*TTV_LOW_RATIO && 
                          (type & TTV_FIND_MIN) == TTV_FIND_MIN))
                        {
                          continue;
                        }
                    }

                  // repropagate delay
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    line_list=ttv_DoPropagation_PRECISION_LEVEL1(ttvfig, figpath, node, root, type);

                  if((ttv_GET_FINDDELAY(node, 0) < delaymin) || (ttv_GET_FINDDELAY(node, 0) > delaymax))
                    { 
                      if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                        ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                      continue ;
                    }
             
                  path = ttv_savpath(ttvfig,figpath,path,root,node,type) ;
                  if (nbcur!=NULL) (*nbcur)++;
             
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                }
            }

          ttv_markdatapath(root, node, type, 0);
          freeptype(ptypelatch);
        }
      ttv_fifoclean() ;
    }

  ttv_fiforestore() ;
  level=0; // avoid warning

  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_savallpath()                          */
/* parametres :                                                              */
/* path : list des chemins                                                   */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps minimum des chemin                                       */
/* nb : nombre de chemin a garder                                            */
/* nb path : nombre de chemin Ren cours                                      */
/* type : type de recherche                                                  */
/* chainnode : liste des fins                                                */
/*                                                                           */
/* construit la liste des chemins                                            */
/*****************************************************************************/
ttvpath_list *ttv_savallpath(path,nb,nbpath,type,rpth,savall)
     ttvpath_list *path ;
     int nb ;
     int *nbpath ;
     long type ;
     ttvpath_list *rpth ;
     int savall ;
{
  ttvpath_list *pathx, *npath ;
  // chain_list *chain ;

  // if (path==NULL) { *nbpath=countchain((chain_list *)rpth); return rpth;}
 
  for(pathx = rpth ; pathx != NULL ; pathx = npath)
    {
      npath=pathx->NEXT;
      if((path != NULL) && (nbpath != NULL))
        {
          if(*nbpath >= nb)
            {
              if(((path->DELAY >= pathx->DELAY) && 
                  ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                 ((path->DELAY <= pathx->DELAY) && 
                  ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
                {
                  pathx->NEXT = NULL ;
                  ttv_freepathlist(pathx) ;
                  continue ;
                }
            }
        }

      pathx->NEXT = path ;
      path = pathx ;
      if(nbpath != NULL)
        path = ttv_keepnbpath(path,nb,nbpath,type,&upper_sort_info,1) ;
    }
  savall=0; // avoid warning
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_savcriticpath()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ttvins : ttvfig instance                                                  */
/* path : list des chemins                                                   */
/* root : evenement de debut                                                 */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps minimum des chemin                                       */
/* nb : nombre de chemin a garder                                            */
/* nb path : nombre de chemin Ren cours                                      */
/* type : type de recherche                                                  */
/* chainnode : liste des fins                                                */
/*                                                                           */
/* construit la liste des chemins critic                                     */
/*****************************************************************************/
ttvpath_list *ttv_savcriticpath(ttvfig,ttvins,path,root,delaymax,delaymin,nb,nbpath,type,level,chainnode,savall)
     ttvfig_list *ttvfig ;
     ttvfig_list *ttvins ;
     ttvpath_list *path ;
     ttvevent_list *root ;
     long delaymax ;
     long delaymin ;
     int nb ;
     int *nbpath ;
     long type ;
     long level ;
     chain_list *chainnode ;
     int savall;
{
  ttvpath_list *pathx ;
  ttvevent_list *node, *firstnode, *node1, *possiblelatchnode;
  chain_list *chain ;
  chain_list *chainx ;
  chain_list *chainin ;
  chain_list *chainx2, *line_list, *thru_command_list;
  ptype_list *pt, *ptype, *ptypelatch;
  ttvline_list *line;
  long delay;
  int abort,ret;
 
  if (((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ONLYNOTHZ)!=0 && (type & TTV_FIND_HZ)!=0)
      || ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ONLYHZ)!=0 && (type & TTV_FIND_HZ)==0)) return path;

  ttv_fifosave() ;

  for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
    {
      node = (ttvevent_list *)chain->DATA ;
      possiblelatchnode=node;
      node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;

      if(((((node->ROOT->TYPE & TTV_SIG_MASKIN) != TTV_SIG_MASKIN) &&
           ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
          (((node->ROOT->TYPE & TTV_SIG_MASKOUT) != TTV_SIG_MASKOUT) &&
           ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL))))
        {
          if ((node->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_L|TTV_SIG_B))!=0 && ((savall & SAVE_ALL_LATCH_TAG)==0))
            continue ;
          if ((node->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_L|TTV_SIG_B))==0 && ((savall & SAVE_ALL_INPUT_TAG)==0))
            continue ;
        }

      if(TTV_CHAIN_CLOCK==NULL || (type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
        {
          if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS)!=0 && TTV_CHAIN_CLOCK!=NULL && (node->ROOT->TYPE & TTV_SIG_R)==0)
            {
              pt=ttv_getlatchaccess(ttvfig,node,type);
              thru_command_list=NULL;
              for (ptype=pt; ptype!=NULL; ptype=ptype->NEXT)
                thru_command_list=addchain(thru_command_list, ptype->DATA);
              freeptype(pt);
            }
          else thru_command_list=addchain(NULL, NULL);

          node1=node;
        
          while (thru_command_list!=NULL)
            {
              if (TTV_CHAIN_CLOCK!=NULL)
                {
                  if ((node1->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_L|TTV_SIG_B))!=0)
                    chainin = ttv_findclockpath(ttvfig,ttvfig,node1,root,type,1,(ttvevent_list *)thru_command_list->DATA) ;
                  else
                    {
                      if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS)==0/*(savall & SAVE_ALL_LATCH_TAG)==0*/ || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_PATH_AND_ACCESS)!=0) // pas de vrai access
                        chainin = addchain(NULL,node1) ;
                      else
                        chainin = NULL;
                    }
                }
              else if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS)==0)
                {
                  // --------- zinaps + ------------
                  if(((((node1->ROOT->TYPE & TTV_SIG_MASKIN) != TTV_SIG_MASKIN) &&
                       ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
                      (((node1->ROOT->TYPE & TTV_SIG_MASKOUT) != TTV_SIG_MASKOUT) &&
                       ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL))))
                    {
                      thru_command_list=delchain(thru_command_list, thru_command_list);
                      continue;
                    }
                  // -------------------------------
                  chainin = addchain(NULL,node1) ;
                }
              else chainin=NULL;
          
              if (TTV_QUIET_MODE==0 && chainin==NULL && (node1->ROOT->TYPE & (TTV_SIG_B|TTV_SIG_C|TTV_SIG_Q))==0)
                {
                  char buf0[128];
                  ttv_error(56,ttv_getsigname(ttvfig,buf0,node1->ROOT),TTV_WARNING);
                }

              for(chainx = chainin ; chainx != NULL ; chainx = chainx->NEXT)
                {
                  ttvevent_list *node = (ttvevent_list *)chainx->DATA ;
                  node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;

                  // test des falsepaths pour les critic access
                  abort=0;
                  /*            if (TTV_CHAIN_CLOCK!=NULL)
                                {
                                if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                                abort=ttv_isfalsepath(ttvfig,root,node,type);
                                else
                                abort=ttv_isfalsepath(ttvfig,node,root,type);    
                                }        
                  */
                  if (TTV_CHAIN_CLOCK!=NULL && (ret=ttv_isfalsepath(ttvfig,node,root,type,0,possiblelatchnode!=node?possiblelatchnode:NULL))!=0)
                    {
                      if (ret==1) TTV_SEARCH_INFORMATIONS|=TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
                      continue;
                    }

                  if(abort==0 && V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && path != NULL && *nbpath >= nb)
                    {
                      if((path->DELAY >= node->FIND->DELAY*TTV_HIGH_RATIO && 
                          (type & TTV_FIND_MAX) == TTV_FIND_MAX) ||
                         (path->DELAY <= node->FIND->DELAY*TTV_LOW_RATIO && 
                          (type & TTV_FIND_MIN) == TTV_FIND_MIN))
                        {
                          if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                            node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                          continue ;
                        }
                    }

                  // repropagate delay
                  if (!abort && V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    line_list=ttv_DoPropagation_PRECISION_LEVEL1(ttvfig, ttvins, node, root, type);
            
                  if(abort || (ttv_GET_FINDDELAY(node, 0) > delaymax) || (ttv_GET_FINDDELAY(node, 0) < delaymin))
                    { 
                      if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                        node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                      if (!abort && V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                        ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                      continue ;
                    }
            
                  if(path != NULL)
                    {
                      if(*nbpath >= nb)
                        {
                          if(((path->DELAY >= ttv_GET_FINDDELAY(node, 0)) && 
                              ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                             ((path->DELAY <= ttv_GET_FINDDELAY(node, 0)) && 
                              ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
                            {
                              if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                                node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                              if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                                ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                              continue ;
                            }
                        }
                    }
       
                  pathx = path ;
                  path = ttv_savpath(ttvfig,ttvins,path,root,node,type) ;
   
                  if(getptype(node->USER,TTV_NODE_CLOCK) != NULL)
                    node->USER = delptype(node->USER,TTV_NODE_CLOCK) ;
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    ttv_CleanPropagation_PRECISION_LEVEL1(line_list);

                  if(pathx == path)
                    continue ;
    
                  path = ttv_keepnbpath(path,nb,nbpath,type,&upper_sort_info,1) ;
                }
              freechain(chainin) ;
              thru_command_list=delchain(thru_command_list, thru_command_list);
              if (thru_command_list!=NULL) ttv_fifoclean() ;
            }
        }
      else
        {
          firstnode=node;
          if ((firstnode->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_B)) != 0)
            ptypelatch=ttv_getcommandaccess(ttvfig,firstnode,type) ;
          else if((firstnode->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
            ptypelatch=addptype(NULL, (long)NULL, firstnode);
          else
            ptypelatch=NULL;

          for(pt = ptypelatch ; pt != NULL ; pt = pt->NEXT)
            {
              ttvevent_list *node = (ttvevent_list *)pt->DATA ;

              ttv_fifopush(node);          
          
              if ((firstnode->ROOT->TYPE & TTV_SIG_R) != 0)
                {
                  // precharge
                  if((ptype = getptype(node->USER,TTV_NODE_LATCH)) == NULL)
                    node->USER = addptype(node->USER,TTV_NODE_LATCH,firstnode) ;
                  else
                    ptype->DATA = (void *)node ;

                  if(node->FIND->NEXT != NULL)
                    {
                      if(node->FIND->DELAY == TTV_NOTIME)
                        {
                          if(node->FIND->NEXT->DELAY != TTV_NOTIME)
                            {
                              node->FIND->DELAY = node->FIND->NEXT->DELAY ;
                              node->FIND->OUTLINE = node->FIND->NEXT->OUTLINE ;
                            }
                        }
                    }
                }
              else 
                {
                  if(pt->TYPE != (long)0)
                    {
                      line = (ttvline_list *)pt->TYPE ;
                      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                        {
                          if ((delay=stb_gettruncatedaccess(line->ROOT, line->NODE, 1))==TTV_NOTIME)
                            delay = ttv_getdelaymax(line) ;
                        }
                      else
                        {                         
                          if ((delay=stb_gettruncatedaccess(line->ROOT, line->NODE, 0))==TTV_NOTIME)
                            delay = ttv_getdelaymin(line) ;
                        }
                    }
                  else
                    {
                      line = NULL ;
                      delay = (long)0 ;
                    }
                  if((ptype = getptype(node->USER,TTV_NODE_LATCH)) == NULL)
                    node->USER = addptype(node->USER,TTV_NODE_LATCH,firstnode) ;
                  else
                    ptype->DATA = (void *)firstnode ;
                  node->FIND->DELAY = delay + firstnode->FIND->DELAY ;
                  node->FIND->OUTLINE = line ;              
                }
          
              chainin = ttv_findpath(ttvfig,ttvfig,node,NULL,type,1) ;
          
              for(chainx2 = chainin ; chainx2 != NULL ; chainx2 = chainx2->NEXT)
                {
                  ttvevent_list *node = (ttvevent_list *)chainx2->DATA ;
                  node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;

                  if(V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && path != NULL && *nbpath >= nb)
                    {
                      if((path->DELAY >= node->FIND->DELAY*TTV_HIGH_RATIO && 
                          (type & TTV_FIND_MAX) == TTV_FIND_MAX) ||
                         (path->DELAY <= node->FIND->DELAY*TTV_LOW_RATIO && 
                          (type & TTV_FIND_MIN) == TTV_FIND_MIN))
                        {
                          continue ;
                        }
                    }

                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    line_list=ttv_DoPropagation_PRECISION_LEVEL1(ttvfig, ttvins, node, root, type);
             
                  if((ttv_GET_FINDDELAY(node, 0) > delaymax) || (ttv_GET_FINDDELAY(node, 0) < delaymin))
                    { 
                      if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                        ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                      continue ;
                    }
          
                  if(path != NULL)
                    {
                      if(*nbpath >= nb)
                        {
                          if(((path->DELAY >= node->FIND->DELAY) && 
                              ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                             ((path->DELAY <= node->FIND->DELAY) && 
                              ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))
                            {
                              if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                                ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                              continue ;
                            }
                        }
                    }
       
                  pathx = path ;
                  path = ttv_savpath(ttvfig,ttvins,path,root,node,type) ;
      
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    ttv_CleanPropagation_PRECISION_LEVEL1(line_list);

                  if(pathx == path)
                    continue ;
       
                  path = ttv_keepnbpath(path,nb,nbpath,type,&upper_sort_info,1) ;
                }
            }
          freeptype(ptypelatch);
        }
      ttv_fifoclean() ;
    }

  ttv_fiforestore() ;
  level=0; // avoid warning
  return(path) ;
}

static int computesavealltag(chain_list *inputs,int allinterm)
{
  chain_list *cl;

  //  if (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS) return SAVE_ALL_TAG;

  if (allinterm && inputs==NULL) return SAVE_ALL_TAG;
  /* 
     if (TTV_CHAIN_CLOCK==NULL) return 0;

     while (inputs!=NULL)
     {
     for (cl=TTV_CHAIN_CLOCK; cl!=NULL && cl->DATA!=inputs->DATA; cl=cl->NEXT) ;
     if (cl!=NULL) return SAVE_ALL_LATCH_TAG;
     inputs=inputs->NEXT;
     }
  */
  return 0;
}

/*****************************************************************************/
/*                        function ttv_findsigpath()                         */
/* parametres :                                                              */
/*****************************************************************************/
ttvpath_list *ttv_findsigpath(ttvfig,ttvins,figsearch,begin,end,cmd,delaymax,delaymin,type,level,path,searchtype,nb,nbcur,savall,pthz)
     ttvfig_list *ttvfig ;
     ttvfig_list *ttvins ;
     ttvfig_list *figsearch ;
     ttvevent_list *begin ;
     ttvevent_list *end ;
     ttvevent_list *cmd ;
     long delaymax ;
     long delaymin ;
     long type ;
     long level ;
     ttvpath_list *path ;
     char searchtype ;
     int nb ;
     int *nbcur ;
     int savall ;
     char *pthz ;
{
  chain_list *chain ;
  chain_list *chainpath ;
  ttvpath_list *pth, *rpth;
  char typehz ;
  char typers, filter=0 ;
  ttvsig_list *tvs;

  mbk_comcheck( 0, 0, ttv_signtimingfigure(ttvfig) );

  TTV_SEARCH_FIRST_NODE=begin;
         
  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      if(TTV_FIND_ALLPATH == 'N')
        chainpath = ttv_findaccess(ttvfig,figsearch,begin,cmd,type,level) ;
      else
        pth = (ttvpath_list *)ttv_findpara(ttvfig,ttvins,begin,end,delaymax,delaymin,type,level,NULL,TTV_MAX_PATHNB,savall) ;

      typehz = 'N' ;
      typers = 'N' ;

      if(TTV_FIND_ALLPATH == 'N')
        for(chain = chainpath ; chain != NULL ; chain = chain->NEXT)
          {
            tvs=((ttvevent_list *)chain->DATA)->ROOT;
            if(((((tvs->TYPE & TTV_SIG_MASKIN) != TTV_SIG_MASKIN) &&
                 ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
                (((tvs->TYPE & TTV_SIG_MASKOUT) != TTV_SIG_MASKOUT) &&
                 ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)))) continue;
              
            if((tvs->TYPE & TTV_SIG_R) == TTV_SIG_R || (tvs->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ || (tvs->TYPE & TTV_SIG_CT) == TTV_SIG_CT)
              {
                if ((tvs->TYPE & TTV_SIG_R) != TTV_SIG_R) filter=1;
                typehz = 'Y' ;
              }
            if((tvs->TYPE & TTV_SIG_LR)== TTV_SIG_LR || 
               (tvs->TYPE & TTV_SIG_LS)== TTV_SIG_LS)
              typers = 'Y' ;
          }
      else
        for(rpth = pth ; rpth != NULL ; rpth = rpth->NEXT)
          {
            if(((rpth->ROOT->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) || ((rpth->ROOT->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) || ((rpth->ROOT->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
              {
                if ((rpth->ROOT->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R) filter=1;
                typehz = 'Y' ;
              }
            if(((rpth->ROOT->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) || 
               ((rpth->ROOT->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
              typers = 'Y' ;
          }

      if(typers == 'Y')
        {
          if(TTV_FIND_ALLPATH == 'N')
            {
              for(chain = chainpath ; chain != NULL ; chain = chain->NEXT)
                {
                  ttvevent_list *node = (ttvevent_list *)chain->DATA ;
                  node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
                }
              freechain(chainpath) ;
            }
          else
            {
              ttv_freepathlist(rpth) ;
            }
          ttv_fifoclean() ;
          if(TTV_FIND_ALLPATH == 'N')
            chainpath = ttv_findaccess(ttvfig,figsearch,begin,cmd,type|TTV_FIND_R,level) ;
          else
            pth = (ttvpath_list *)ttv_findpara(ttvfig,ttvins,begin,end,delaymax,delaymin,type|TTV_FIND_R,level,NULL,(typehz=='Y')?TTV_MAX_PATHNB:nb,savall) ;
        }
    }
  else
    {
      if(((begin->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
         ((begin->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
         ((begin->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
        {
          if ((begin->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R) filter=1;
          typehz = 'Y' ;
        }
      else
        typehz = 'N' ;
 
      if(((begin->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS) ||
         ((begin->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR))
        {
          typers = 'Y' ;
          if(TTV_FIND_ALLPATH == 'N')
            {
              if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0)
                chainpath = ttv_findaccess(ttvfig,figsearch,begin,cmd,type|TTV_FIND_R,level) ;
              else
                {
                  ttv_fifopush(begin) ;
                  ttv_uncachenodelines(ttvfig,figsearch,begin, type);
                  begin->FIND->DELAY = 0 ;
                  chainpath=addchain(NULL, begin);
                }
            }
          else
            pth = (ttvpath_list *)ttv_findpara(ttvfig,ttvins,begin,end,delaymax,delaymin,type|TTV_FIND_R,level,NULL,(filter)?TTV_MAX_PATHNB:nb,savall) ;
        }
      else
        {
          if(TTV_FIND_ALLPATH == 'N')
            {
              if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0 || (begin->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_B))==0)
                chainpath = ttv_findaccess(ttvfig,figsearch,begin,cmd,type,level) ;
              else
                {
                  ttv_fifopush(begin) ;
                  ttv_uncachenodelines(ttvfig,figsearch,begin, type);
                  begin->FIND->DELAY = 0 ;
                  chainpath=addchain(NULL, begin);
                }
            }
          else  
            pth = (ttvpath_list *)ttv_findpara(ttvfig,ttvins,begin,end,delaymax,delaymin,type,level,NULL,(filter)?TTV_MAX_PATHNB:nb,savall) ;
          typers = 'N' ;
        }
    }

  if(TTV_FIND_ALLPATH == 'N')
    {
      if(searchtype == TTV_SEARCH_PATH)
        path = ttv_savpathlist(ttvfig,ttvins,path,begin,(typers == 'Y') ? type|TTV_FIND_R : type,level,chainpath,delaymax,delaymin,savall,((type & TTV_FIND_DUAL) == TTV_FIND_DUAL || typehz=='Y')?TTV_MAX_PATHNB:nb,nbcur) ;
      else
        path = ttv_savcriticpath(ttvfig,ttvins,path,begin,delaymax,delaymin,(filter)?TTV_MAX_PATHNB:nb,nbcur,(typers == 'Y') ? type|TTV_FIND_R : type,level,chainpath,savall) ;
      freechain(chainpath) ;
    }
  else
    {
      path = ttv_savallpath(path,(filter)?TTV_MAX_PATHNB:nb,nbcur,(typers == 'Y') ? type|TTV_FIND_R : type,pth,savall) ;
    }

  ttv_fifoclean() ;

  if(typers == 'Y')
    {
      if(TTV_FIND_ALLPATH == 'N')
        chainpath = ttv_findaccess(ttvfig,figsearch,begin,cmd,type|TTV_FIND_S,level) ;
      else
        pth = (ttvpath_list *)ttv_findpara(ttvfig,ttvins,begin,end,delaymax,delaymin,type|TTV_FIND_S,level,NULL,(filter)?TTV_MAX_PATHNB:nb,savall) ;
      if(TTV_FIND_ALLPATH == 'N')
        {
          if(searchtype == TTV_SEARCH_PATH)
            path = ttv_savpathlist(ttvfig,ttvins,path,begin,type|TTV_FIND_S,level,chainpath,delaymax,delaymin,savall,((type & TTV_FIND_DUAL) == TTV_FIND_DUAL || filter)?TTV_MAX_PATHNB:nb,nbcur) ;
          else
            path = ttv_savcriticpath(ttvfig,ttvins,path,begin,delaymax,delaymin,(filter)?TTV_MAX_PATHNB:nb,nbcur,type|TTV_FIND_S,level,chainpath,savall) ;
          freechain(chainpath) ;
        }
      else
        {
          path = ttv_savallpath(path,(filter)?TTV_MAX_PATHNB:nb,nbcur,type|TTV_FIND_S,pth,savall) ;
        }
      ttv_fifoclean() ;
    }

  if(typehz == 'Y')
    {
      if (filter) *pthz = 'Y' ;
      if(TTV_FIND_ALLPATH == 'N')
        {
          chainpath = ttv_findaccess(ttvfig,figsearch,begin,cmd,type|TTV_FIND_HZ,level) ;
        }
      else
        pth = (ttvpath_list *)ttv_findpara(ttvfig,ttvins,begin,end,delaymax,delaymin,type|TTV_FIND_HZ,level,NULL,(filter)?TTV_MAX_PATHNB:nb,savall) ;
      if(TTV_FIND_ALLPATH == 'N')
        {
          if(searchtype == TTV_SEARCH_PATH)
            path = ttv_savpathlist(ttvfig,ttvins,path,begin,type|TTV_FIND_HZ,level,chainpath,delaymax,delaymin,savall,((type & TTV_FIND_DUAL) == TTV_FIND_DUAL || typehz=='Y')?TTV_MAX_PATHNB:nb,nbcur) ;
          else
            path = ttv_savcriticpath(ttvfig,ttvins,path,begin,delaymax,delaymin,(filter)?TTV_MAX_PATHNB:nb,nbcur,type|TTV_FIND_HZ,level,chainpath,savall) ;
          freechain(chainpath) ;
        }
      else
        {
          path = ttv_savallpath(path,(filter)?TTV_MAX_PATHNB:nb,nbcur,type|TTV_FIND_HZ,pth,savall) ;
        }
      ttv_fifoclean() ;
    }

  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getcriticpath()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* maskin : list de masques pour les entrees                                 */
/* maskout : list de masques pour les sorties                                */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* nb : nombre de chemin a garder                                            */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche le chemin le plus court ou le plus long d'une ttvfig            */
/*****************************************************************************/
ttvpath_list *ttv_getcriticpath(ttvfig,figpath,maskin,maskout,delaymax,delaymin,nb,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     chain_list *maskin ;
     chain_list *maskout ;
     long delaymax ;
     long delaymin ;
     int nb ;
     long type ;
{
  ttvfig_list *ttvins ;
  ttvsig_list *ptsig ;
  chain_list *chainx, *event_to_retry, *chc, *chainc ;
  chain_list *chaininx ;
  chain_list *chaininsav ;
  chain_list *chainfig ;
  chain_list *chainfigsav ;
  chain_list *chainfigdel ;
  ttvpath_list *path = NULL, *pathx, *npathx, *prevpathx ;
  chain_list *chain ;
  long level, searchsigtype ;
  // int nbx = 1 ;  changed zinaps
  int nbx = 0 ;
  int i, savealltag, allfailed ;
  char typehz ;

  if(((type & TTV_FIND_PATH) == TTV_FIND_PATH) ||
     ((type & TTV_FIND_LINE) != TTV_FIND_LINE))
    {
      type &= ~(TTV_FIND_LINE | TTV_FIND_HIER) ;
      type |= TTV_FIND_PATH ;
    }
  else
    type &= ~(TTV_FIND_HIER) ;

  if(nb <= 0) nb = 1 ;

  if(figpath == NULL)
    chainfigsav = reverse(ttv_getttvfiglist(ttvfig)) ;
  else
    chainfigsav = addchain(NULL,figpath) ;

  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
    {
      if (TTV_EXPLICIT_START_NODES==NULL)
        chaininsav = ttv_getsigbytype_and_netname(ttvfig,figpath,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B/*|TTV_SIG_Q*/,maskin) ;
      else
        chaininsav = dupchainlst(TTV_EXPLICIT_START_NODES);
      chaininsav = ttv_apply_exclude_flag(chaininsav, TTV_EXCLUDE_TYPE_START, 0/*1*/);
      chaininsav = ttv_remove_connector_B(chaininsav);
    }
  else
    {
      if (TTV_EXPLICIT_END_NODES==NULL)
        chaininsav = ttv_getsigbytype_and_netname(ttvfig,figpath,TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B,maskout) ;
      else
        chaininsav = dupchainlst(TTV_EXPLICIT_END_NODES);
      chaininsav = ttv_apply_exclude_flag(chaininsav, TTV_EXCLUDE_TYPE_END, 0);

      if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE)!=0 && stb_getstbfig(ttvfig)!=NULL)
        { 
          // not possible for filter in dual mode
          for (chain=chaininsav; chain!=NULL; chain=chain->NEXT)
            {
              if (stb_has_filter_directive(((ttvsig_list *)chain->DATA)->NODE)
                  || stb_has_filter_directive(((ttvsig_list *)chain->DATA)->NODE+1)) break;
            }
          if (chain!=NULL)
            {
              // revert back to not dual mode
              freechain(chaininsav);
              if (TTV_EXPLICIT_START_NODES==NULL)
                chaininsav = ttv_getsigbytype_and_netname(ttvfig,figpath,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B/*|TTV_SIG_Q*/,maskin) ;
              else
                chaininsav = dupchainlst(TTV_EXPLICIT_START_NODES);
              chaininsav = ttv_apply_exclude_flag(chaininsav, TTV_EXCLUDE_TYPE_START, 0/*1*/);
              chaininsav = ttv_remove_connector_B(chaininsav);
              type&=~TTV_FIND_DUAL;
            }
        }
    }

  // calcul si on doit ignorer le maskin pour les latchs des access
  savealltag=computesavealltag(chaininsav, 0);

  for(chaininx = chaininsav ; chaininx != NULL ; chaininx = chaininx->NEXT)
    {
      if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
        ((ttvsig_list *)chaininx->DATA)->TYPE |= TTV_SIG_MASKIN ;
      else
        ((ttvsig_list *)chaininx->DATA)->TYPE |= TTV_SIG_MASKOUT ;
    }

  chainfigdel = chainfigsav ;
  for(chainfig = chainfigsav ; chainfig != NULL ; chainfig = chainfig->NEXT)
    {
      ttvins = (ttvfig_list *)chainfig->DATA ;
      if(((ttvins->STATUS & TTV_STS_MODEL) != TTV_STS_MODEL) &&
         (maskin == NULL) && (maskout == NULL))
        continue ;

      if(ttvins == ttvfig)
        level = ttvfig->INFO->LEVEL ;
      else
        level = (long)0 ;

      if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
        {
          if (TTV_EXPLICIT_END_NODES==NULL)
            chain = ttv_getsigbytype_and_netname(ttvfig,ttvins,TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B,maskout) ;
          else
            chain=dupchainlst(TTV_EXPLICIT_END_NODES);
          chain = ttv_apply_exclude_flag(chain, TTV_EXCLUDE_TYPE_END, 0);
        }
      else
        {
          searchsigtype=TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B/*|TTV_SIG_Q*/;
          //     if (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ENABLE_START_ON_TTV_SIG_Q) searchsigtype|=TTV_SIG_Q;
          if (TTV_EXPLICIT_START_NODES==NULL)
            chain = ttv_getsigbytype_and_netname(ttvfig,ttvins,searchsigtype,maskin) ;
          else
            chain=dupchainlst(TTV_EXPLICIT_START_NODES);
          chain = ttv_apply_exclude_flag(chain, TTV_EXCLUDE_TYPE_START, 0/*1*/);
          chain = ttv_remove_connector_B(chain);
        }

      event_to_retry=NULL;

      for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
        {
          ptsig = (ttvsig_list *)chainx->DATA ;
          if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
            if((((ptsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO) &&
                ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
               (((ptsig->TYPE & TTV_SIG_CI) != TTV_SIG_CI) &&
                ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)))
              continue ;

          if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL && (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DIFF_ENDING_CRITIC_COMMAND))
            chainc = ttv_getlrcmd(ttvfig,ptsig) ;
          else
            chainc = NULL;
      
          if(chainc == NULL)
            chainc = addchain(chainc,NULL) ;
      
          allfailed=0;
          for(chc = chainc ; chc != NULL ; chc = chc->NEXT)
            {
              typehz = 'N' ;
   
              for(i = 0 ; i < 2 ; i++)
                {
                  if((((((type & (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_UPDW)) == (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_UPDW)) && (i == 1)) || 
                       (((type & (TTV_FIND_NOT_DWUP|TTV_FIND_NOT_DWDW)) == (TTV_FIND_NOT_DWUP|TTV_FIND_NOT_DWDW)) && (i == 0)))
                      && ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)) ||
                     (((((type & (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_DWUP)) == (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_DWUP)) && (i == 1)) || 
                       (((type & (TTV_FIND_NOT_UPDW|TTV_FIND_NOT_DWDW)) == (TTV_FIND_NOT_UPDW|TTV_FIND_NOT_DWDW)) && (i == 0)))
                      && ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
                      && (ptsig->TYPE & (TTV_SIG_CT|TTV_SIG_CZ))==0 // pour le filtrage HZ                    
                      ))
                    continue ;
                  TTV_SEARCH_INFORMATIONS&=~TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
                  path = (ttvpath_list *)ttv_findsigpath(ttvfig,ttvins,ttvins,ptsig->NODE+i,NULL,(ttvevent_list *)chc->DATA,delaymax,delaymin,type,level,path,TTV_SEARCH_CRITICPATH,nb,&nbx,savealltag,&typehz) ;
                  if (TTV_SEARCH_INFORMATIONS & TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH)
                    {
                      event_to_retry=addchain(event_to_retry, ptsig->NODE+i);
                      allfailed|=1<i;
                      if (typehz=='Y')
                        {
                          // pour le filtre des HZ il faut les 2 events, donc on recherhera les // pour les 2 events
                          allfailed=3;
                          if (i==0)
                            {
                              event_to_retry=addchain(event_to_retry, ptsig->NODE+1);
                              break;
                            }
                          else
                            {
                              event_to_retry=addchain(event_to_retry, ptsig->NODE);
                            }
                        }
                    }
                }
              if (allfailed==3) break;
            }

          freechain(chainc);

          if(typehz == 'Y')
            path = ttv_filterpathhz(path,&nbx, nb,&upper_sort_info, type) ;

          typehz = 'N' ;
        }
      freechain(chain) ;

      if (event_to_retry!=NULL)
        {
          int r_nbx, retried_nbx=0;
          ttvpath_list *retried_list=NULL;
          ttvevent_list *tve;


          // remove signal to retry from the list
          for (chain=event_to_retry; chain!=NULL; chain=chain->NEXT) 
            ((ttvevent_list *)chain->DATA)->TYPE|=TTV_NODE_MARQUE;
     
          for (pathx=path; pathx!=NULL; pathx=npathx)
            {
              npathx=pathx->NEXT;
              if ((type & TTV_FIND_DUAL)!=0) tve=pathx->NODE; else tve=pathx->ROOT;
              if (tve->TYPE & TTV_NODE_MARQUE)
                {
                  if (pathx==path) path=npathx; else prevpathx->NEXT=npathx;
                  pathx->NEXT=NULL;
                  ttv_freepathlist(pathx);
                  nbx--;
                }
              else prevpathx=pathx;
            }
          for (chain=event_to_retry; chain!=NULL; chain=chain->NEXT) 
            ((ttvevent_list *)chain->DATA)->TYPE&=~TTV_NODE_MARQUE;

          TTV_FIND_ALLPATH='Y';
          if ((type & TTV_FIND_ACCESS)==0) TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH;

          while (event_to_retry!=NULL)
            {
              tve = (ttvevent_list *)event_to_retry->DATA ;

              if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL && (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_DIFF_ENDING_CRITIC_COMMAND))
                chainc = ttv_getlrcmd(ttvfig,tve->ROOT) ;
              else
                chainc = NULL;
        
              if(chainc == NULL)
                chainc = addchain(chainc,NULL) ;
        
              typehz = 'N' ;
              for(chc = chainc ; chc != NULL ; chc = chc->NEXT)
                {
                  r_nbx=0;
                  pathx=NULL;
                  pathx = (ttvpath_list *)ttv_findsigpath(ttvfig,ttvins,ttvins,tve,NULL,(ttvevent_list *)chc->DATA,delaymax,delaymin,type|TTV_FIND_ALL,level,pathx,TTV_SEARCH_CRITICPATH,nb,&r_nbx,savealltag,&typehz) ;
                  if(typehz == 'Y')
                    {
                      if (event_to_retry->NEXT!=NULL && tve->ROOT==((ttvevent_list *)event_to_retry->NEXT->DATA)->ROOT) // test normalement vrai
                        {
                          tve = (ttvevent_list *)event_to_retry->NEXT->DATA ;
                          pathx = (ttvpath_list *)ttv_findsigpath(ttvfig,ttvins,ttvins,tve,(ttvevent_list *)chc->DATA,NULL,delaymax,delaymin,type|TTV_FIND_ALL,level,pathx,TTV_SEARCH_CRITICPATH,nb,&r_nbx,savealltag,&typehz) ;
                          event_to_retry=delchain(event_to_retry, event_to_retry);
                        }

                      pathx = ttv_filterpathhz(pathx,&r_nbx, nb, NULL, type) ;
                    }
                  // keep critic only
           
                  pathx=ttv_keep_critic_paths(pathx, &r_nbx, type, (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS)!=0?1:0);

                  // merge to list

                  retried_list=ttv_mergepathlists(retried_list, retried_nbx, pathx, r_nbx, nb, type, &retried_nbx);
                }

              freechain(chainc);

              if (typehz == 'Y' && event_to_retry->NEXT!=NULL && tve->ROOT==((ttvevent_list *)event_to_retry->NEXT->DATA)->ROOT)
                event_to_retry=delchain(event_to_retry, event_to_retry);
              event_to_retry=delchain(event_to_retry, event_to_retry);
            }
          TTV_FIND_ALLPATH='N';
          TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH;

          // merge to previous list
          path=ttv_mergepathlists(path, nbx, retried_list, retried_nbx, nb, type, &nbx);
        }
      TTV_SEARCH_INFORMATIONS&=~TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;

   
      if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
        ttv_addinfreelist(ttvins,TTV_STS_P) ;
      else
        ttv_addinfreelist(ttvins,TTV_STS_F) ;
    }
  
  for(chaininx = chaininsav ; chaininx != NULL ; chaininx = chaininx->NEXT)
    {
      ((ttvsig_list *)chaininx->DATA)->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
    }
  freechain(chaininsav) ;
  freechain(chainfigsav) ;

  ttv_delinfreelist(NULL,NULL) ;

  pathx=ttv_classpath(path,type);
  for(path=pathx, i=1; i<nb && path != NULL ; path = path->NEXT , i++) ;

  if(path != NULL)
    {
      ttv_freepathlist(path->NEXT) ;
      path->NEXT = NULL ;
    }
  return(pathx) ;
}

/*****************************************************************************/
/*                        function ttv_getcriticaccess()                     */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* maskin : list de masques pour les entrees                                 */
/* maskout : list de masques pour les sorties                                */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* nb : nombre de chemin a garder                                            */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche le temps d'acces le plus court ou le plus long d'une ttvfig     */
/*****************************************************************************/
ttvpath_list *ttv_getcriticaccess(ttvfig,figpath,maskin,maskout,maskclock,delaymax,delaymin,nb,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     chain_list *maskin ;
     chain_list *maskout ;
     chain_list *maskclock ;
     long delaymax ;
     long delaymin ;
     int nb ;
     long type ;
{
  ttvpath_list *path ;
#ifdef SPEED_UP_ACCESS_CRITIC
  long subtype;
  chain_list *clknames;
#endif 
  chain_list *chainclock;
 
  if (TTV_EXPLICIT_CLOCK_NODES==NULL)
    {
      if(maskclock == NULL)
        chainclock = ttv_getclocksiglist(ttvfig) ;
      else
        chainclock = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B,maskclock) ;
    }
  else
    chainclock=dupchainlst(TTV_EXPLICIT_CLOCK_NODES);

#ifdef SPEED_UP_ACCESS_CRITIC
  // speed up all access
  clknames=namefromsig(ttvfig, chainclock);
  if (clknames!=NULL)
    {
      long save0, save1, save2;
      ttv_set_search_exclude_type(0, 0, &save0, &save1);
      save2=TTV_MORE_SEARCH_OPTIONS;
      TTV_MORE_SEARCH_OPTIONS=0;
      subtype=type & (TTV_FIND_PATH|TTV_FIND_LINE|TTV_FIND_MIN|TTV_FIND_MAX);
      TTV_MARK_MODE=TTV_MARK_MODE_DO;
      ttv_getcriticpath(ttvfig,figpath,clknames,NULL,delaymax,delaymin,LONG_MAX,TTV_FIND_DUAL|subtype) ;
 
      TTV_MARK_MODE=TTV_MARK_MODE_USE;
      freechain(clknames);
      ttv_set_search_exclude_type(save0, save1, &save0, &save1);
      TTV_MORE_SEARCH_OPTIONS=save2;
    }
#endif 
  TTV_CHAIN_CLOCK = chainclock;
  path = ttv_getcriticpath(ttvfig,figpath,maskin,maskout,delaymax,delaymin,nb,type|TTV_FIND_ACCESS) ;

#ifdef SPEED_UP_ACCESS_CRITIC
  ttv_FreeMarkedSigList();
  TTV_MARK_MODE=0;
#endif

  freechain(TTV_CHAIN_CLOCK) ;
  TTV_CHAIN_CLOCK = NULL ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getpath()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* ptsig : signal de depart de la recherche                                  */
/* mask : list de masques pour les entrees ou sorties                        */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les chemins entre ptsig et les signaux de mask             */
/*****************************************************************************/
ttvpath_list *ttv_getpath(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvfig_list *ttvfigx ;
  ttvfig_list *ttvfigsav ;
  ttvpath_list *path = NULL ;
  chain_list *chainin ;
  chain_list *chain ;
  chain_list *chainpath ;
  chain_list *chainc, *event_to_retry ;
  chain_list *chc ;
  long levelsig ;
  long levelend ;
  long level = (long)0 ;
  long typesig ;
  int i, savealltag ;
  char typehz ;

  if(((type & TTV_FIND_PATH) == TTV_FIND_PATH) ||
     ((type & TTV_FIND_LINE) != TTV_FIND_LINE))
    {
      type &= ~(TTV_FIND_LINE | TTV_FIND_HIER) ;
      type |= TTV_FIND_PATH ;
    }
  else
    type &= ~(TTV_FIND_HIER) ;

  if(mask != NULL)
    {
      levelsig = ttv_getsiglevel(ptsig) ;

      if(levelsig == ttvfig->INFO->LEVEL)
        {
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            typesig = TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B ;
          else
            typesig = TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_Q ;
        }
      else
        {
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            typesig = TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B ;
          else
            typesig = TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_Q ;
        }

      if((type & TTV_FIND_SIG) != TTV_FIND_SIG)
        {
          long excludemask;
          int exclQK=0;
          if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) excludemask=TTV_EXCLUDE_TYPE_END; else excludemask=TTV_EXCLUDE_TYPE_START, exclQK=1;
          chainin = ttv_getsigbytype_and_netname(ttvfig,NULL,typesig,mask) ;
          chainin = ttv_apply_exclude_flag(chainin, excludemask, exclQK);
        }
      else
        {
          chainin = NULL ; 
          for(chain = mask ; chain != NULL ; chain = chain->NEXT) 
            {
              chainin = addchain(chainin,chain->DATA) ;
            }
        }

      if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
        chainin = ttv_remove_connector_B(chainin);
   
      for(chain = chainin ; chain != NULL ; chain = chain->NEXT) 
        {
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            ((ttvsig_list *)chain->DATA)->TYPE |= TTV_SIG_MASKOUT ;
          else
            ((ttvsig_list *)chain->DATA)->TYPE |= TTV_SIG_MASKIN ;
          if((levelend = ttv_getsiglevel((ttvsig_list *)chain->DATA)) > level) 
            level = levelend ;
        }

      levelend = level ;

      // calcul si on doit ignorer le maskin pour les latchs des access
      savealltag=computesavealltag(chainin, 0);

      if((levelend < ttvfig->INFO->LEVEL) && (levelsig == ttvfig->INFO->LEVEL)) 
        {
          chainpath = NULL ;
          for(chain = chainin ; chain != NULL ; chain = chain->NEXT)
            {
              if((((ttvsig_list *)chain->DATA)->TYPE & TTV_SIG_C) != TTV_SIG_C)
                chainpath = addchain(chainpath,chain->DATA) ;
              else 
                {
                  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                    ((ttvsig_list *)chain->DATA)->TYPE &= ~(TTV_SIG_MASKOUT) ;
                  else
                    ((ttvsig_list *)chain->DATA)->TYPE &= ~(TTV_SIG_MASKIN) ;
                }
            }
          freechain(chainin) ;
          chainin = chainpath ;
        }
  
      if(levelsig > ttvfig->INFO->LEVEL) levelsig = ttvfig->INFO->LEVEL ;
      if(levelend > ttvfig->INFO->LEVEL) levelend = ttvfig->INFO->LEVEL ;
  
      if(levelsig > levelend) level = levelend ;
      else level = levelsig ;

      for(ttvfigx = ptsig->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS |= TTV_STS_MARQUE ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }

      ttvfigsav = ttvfig ;

      for(chain = chainin ; chain != NULL ; chain = chain->NEXT)
        {
          for(ttvfigx = ((ttvsig_list *)chain->DATA)->ROOT ; 
              ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
            {
              if((ttvfigx->STATUS & TTV_STS_MARQUE) == TTV_STS_MARQUE)
                break ;
            }
          if(ttvfigx != NULL)
            if(ttvfigx->INFO->LEVEL < ttvfigsav->INFO->LEVEL) ttvfigsav = ttvfigx ;
          if(ttvfigsav == ptsig->ROOT)
            break ;
        }

      for(ttvfigx = ptsig->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS &= ~(TTV_STS_MARQUE) ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }
    }
  else
    {
      chainin = NULL ;
      savealltag=computesavealltag(chainin, 1);
    }

  if((figpath == NULL) && (mask != NULL))
    {
      if((level < ttvfig->INFO->LEVEL) && (level == ttvfigsav->INFO->LEVEL))
        {
          level = (long)0 ;
        }
      else if((level == ttvfig->INFO->LEVEL) && (level > ttvfigsav->INFO->LEVEL))
        {
          level = ttvfigsav->INFO->LEVEL ;
          ttvfigsav = ttvfig ;
        }
    }
  else if((figpath == NULL) && (mask == NULL))
    {
      level = ptsig->ROOT->INFO->LEVEL ;
      ttvfigsav = ttvfig ;
    }
  else
    {
      ttvfigsav = figpath ;
      if(figpath == ttvfig)
        {
          level = figpath->INFO->LEVEL ;
        }
      else
        {
          level = (long)0 ;
        }
    }

  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
    chainc = ttv_getlrcmd(ttvfig,ptsig) ;
  else
    chainc = NULL;

  if(chainc == NULL || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS)!=0)
    chainc = addchain(chainc,NULL) ;

  typehz = 'N' ;
  for(chc = chainc ; chc != NULL ; chc = chc->NEXT)
    {
      event_to_retry=NULL;
      if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL && (ptsig->TYPE & TTV_SIG_CB)==TTV_SIG_CB) continue;
      for(i = 0 ; i < 2 ; i++)
        {
          if((((((type & (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_UPDW)) == (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_UPDW)) && (i == 1)) || 
               (((type & (TTV_FIND_NOT_DWUP|TTV_FIND_NOT_DWDW)) == (TTV_FIND_NOT_DWUP|TTV_FIND_NOT_DWDW)) && (i == 0)))
              && ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)) ||
             (((((type & (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_DWUP)) == (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_DWUP)) && (i == 1)) || 
               (((type & (TTV_FIND_NOT_UPDW|TTV_FIND_NOT_DWDW)) == (TTV_FIND_NOT_UPDW|TTV_FIND_NOT_DWDW)) && (i == 0)))
              && ((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
              && (ptsig->TYPE & (TTV_SIG_CT|TTV_SIG_CZ))==0 // pour le filtrage HZ
              ))
            continue ;
          TTV_SEARCH_INFORMATIONS&=~TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
          path = (ttvpath_list *)ttv_findsigpath(ttvfig,figpath,ttvfigsav,ptsig->NODE+i,NULL,(ttvevent_list *)chc->DATA,delaymax,delaymin,type,level,path,TTV_SEARCH_PATH,TTV_MAX_PATHNB,NULL, savealltag,&typehz) ;
          if (TTV_SEARCH_INFORMATIONS & TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH)
            {
              event_to_retry=addchain(event_to_retry, ptsig->NODE+i);
              if (typehz=='Y')
                {
                  // pour le filtre des HZ il faut les 2 events, donc on recherhera les // pour les 2 events
                  if (i==0)
                    {
                      event_to_retry=addchain(event_to_retry, ptsig->NODE+1);
                      break;
                    }
                  else
                    {
                      event_to_retry=addchain(event_to_retry, ptsig->NODE);
                    }
                }
            }
        }
      if (event_to_retry!=NULL)
        {
          int r_nbx, retried_nbx=0;
          ttvpath_list *retried_list=NULL;
          ttvevent_list *tve;
          ttvpath_list *pathx, *npathx, *prevpathx;

          // remove signal to retry from the list
          for (chain=event_to_retry; chain!=NULL; chain=chain->NEXT) 
            ((ttvevent_list *)chain->DATA)->TYPE|=TTV_NODE_MARQUE;
     
          for (pathx=path; pathx!=NULL; pathx=npathx)
            {
              npathx=pathx->NEXT;
              if ((type & TTV_FIND_DUAL)!=0) tve=pathx->NODE; else tve=pathx->ROOT;
              if (pathx->CMD==chc->DATA && tve->TYPE & TTV_NODE_MARQUE)
                {
                  if (pathx==path) path=npathx; else prevpathx->NEXT=npathx;
                  pathx->NEXT=NULL;
                  ttv_freepathlist(pathx);
                }
              else prevpathx=pathx;
            }
          for (chain=event_to_retry; chain!=NULL; chain=chain->NEXT) 
            ((ttvevent_list *)chain->DATA)->TYPE&=~TTV_NODE_MARQUE;

          TTV_FIND_ALLPATH='Y';
          if ((type & TTV_FIND_ACCESS)==0) TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH;
          while (event_to_retry!=NULL)
            {
              tve = (ttvevent_list *)event_to_retry->DATA ;

              typehz = 'N' ;
              r_nbx=0;
              pathx=NULL;
              pathx = (ttvpath_list *)ttv_findsigpath(ttvfig,figpath,ttvfigsav,tve,NULL,(ttvevent_list *)chc->DATA,delaymax,delaymin,type|TTV_FIND_ALL,level,pathx,TTV_SEARCH_PATH,TTV_MAX_PATHNB,NULL, savealltag,&typehz) ;
        
              if(typehz == 'Y')
                {
                  if (event_to_retry->NEXT!=NULL && tve->ROOT==((ttvevent_list *)event_to_retry->NEXT->DATA)->ROOT) // test normalement vrai
                    {
                      event_to_retry=delchain(event_to_retry, event_to_retry);
                      tve = (ttvevent_list *)event_to_retry->DATA ;
                      pathx = (ttvpath_list *)ttv_findsigpath(ttvfig,figpath,ttvfigsav,tve,NULL,(ttvevent_list *)chc->DATA,delaymax,delaymin,type|TTV_FIND_ALL,level,pathx,TTV_SEARCH_PATH,TTV_MAX_PATHNB,NULL, savealltag,&typehz) ;
                    }

                  pathx = ttv_filterpathhz(pathx,NULL, TTV_MAX_PATHNB, NULL, type) ;
                }
              // keep critic only
        
              pathx=ttv_keep_critic_paths(pathx, &r_nbx, type, (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS)!=0?1:0);

              // merge to list

              retried_list=ttv_mergepathlists(retried_list, retried_nbx, pathx, r_nbx, TTV_MAX_PATHNB, type, &retried_nbx);
        
              event_to_retry=delchain(event_to_retry, event_to_retry);
            }
          TTV_FIND_ALLPATH='N';
          TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH;

          // merge to previous list
          path=ttv_mergepathlists(path, countchain((chain_list *)path), retried_list, retried_nbx, TTV_MAX_PATHNB, type, &r_nbx);
        }
      TTV_SEARCH_INFORMATIONS&=~TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH;
    }
  freechain(chainc) ;

  for(chain = chainin ; chain != NULL ; chain = chain->NEXT) 
    if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      ((ttvsig_list *)chain->DATA)->TYPE &= ~(TTV_SIG_MASKOUT) ;
    else
      ((ttvsig_list *)chain->DATA)->TYPE &= ~(TTV_SIG_MASKIN) ;

  freechain(chainin) ;

  if(typehz == 'Y')
    path = ttv_filterpathhz(path,NULL,0,NULL,0) ;

  if((type & (TTV_FIND_NOT_UPUP|TTV_FIND_NOT_UPDW|
              TTV_FIND_NOT_DWUP|TTV_FIND_NOT_DWDW)) != 0)
    path = ttv_filterpath(path,type,NULL) ;

  if((type & TTV_FIND_NOTCLASS) == TTV_FIND_NOTCLASS)
    return(path) ;
  else
    return(ttv_classpath(path,type)) ;
}

/*****************************************************************************/
/*                        function ttv_getpathnocross()                      */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* ptsig : signal de depart de la recherche                                  */
/* mask : list de masques pour les entrees ou sorties                        */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les chemins entre ptsig et les signaux de mask             */
/*****************************************************************************/
ttvpath_list *ttv_getpathnocross(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;
  int maxperiod, savmpd;

  maxperiod = V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE ;
  savmpd=TTV_MaxPathPeriodPrecharge;
  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = 0 ;
  TTV_MaxPathPeriodPrecharge=0;
  
  path = ttv_getpath(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type) ;

  TTV_MaxPathPeriodPrecharge=savmpd;
  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = maxperiod ;

  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getaccessnocross()                    */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* ptsig : signal de depart de la recherche                                  */
/* mask : list de masques pour les entrees ou sorties                        */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les chemins entre ptsig et les signaux de mask             */
/*****************************************************************************/
ttvpath_list *ttv_getaccessnocross(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;
  int maxperiod, savmpd;

  maxperiod = V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE ;
  savmpd=TTV_MaxPathPeriodPrecharge;
  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = 0 ;
  TTV_MaxPathPeriodPrecharge=0;
  
  path = ttv_getaccess(ttvfig,figpath,ptsig,mask,NULL,delaymax,delaymin,type) ;

  TTV_MaxPathPeriodPrecharge=savmpd;
  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = maxperiod ;

  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getpathsig()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* ptsig : signal de depart de la recherche                                  */
/* mask : list de masques pour les entrees ou sorties                        */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les chemins pour un signal                                 */
/*****************************************************************************/
ttvpath_list *ttv_getpathsig(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;

  TTV_FIND_ALLPATH = 'Y' ;
  path = ttv_getpath(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type|TTV_FIND_ALL) ;
  TTV_FIND_ALLPATH = 'N' ;
  return(path) ;
}

ttvpath_list *ttv_getpathsignocross(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;
  int maxperiod, savmpd ;

  maxperiod = V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE ;
  savmpd=TTV_MaxPathPeriodPrecharge;

  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = 0 ;
  TTV_MaxPathPeriodPrecharge=0;

  TTV_FIND_ALLPATH = 'Y' ;
  path = ttv_getpath(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type|TTV_FIND_ALL) ;
  TTV_FIND_ALLPATH = 'N' ;

  TTV_MaxPathPeriodPrecharge=savmpd;

  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = maxperiod ;

  return(path) ;
}

ttvpath_list *ttv_getaccesssignocross(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;
  int maxperiod, savmpd ;

  maxperiod = V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE ;
  savmpd=TTV_MaxPathPeriodPrecharge;

  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = 0 ;
  TTV_MaxPathPeriodPrecharge=0;

  TTV_FIND_ALLPATH = 'Y' ;
  path = ttv_getaccess(ttvfig,figpath,ptsig,mask,NULL,delaymax,delaymin,type|TTV_FIND_ALL) ;
  TTV_FIND_ALLPATH = 'N' ;

  TTV_MaxPathPeriodPrecharge=savmpd;

  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = maxperiod ;

  return(path) ;
}


/*****************************************************************************/
/*                        function ttv_getaccess()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* ptsig : signal de depart de la recherche                                  */
/* mask : list de masques pour les entrees ou sorties                        */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les temps d'acces pour les extramitees                     */
/*****************************************************************************/
ttvpath_list *ttv_getaccess(ttvfig,figpath,ptsig,mask,maskclock,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     chain_list *maskclock ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;
#ifdef SPEED_UP_ACCESS_CRITIC
  long subtype;
  chain_list *clknames;
#endif 
  chain_list *chainclock;
 
  if (TTV_EXPLICIT_CLOCK_NODES==NULL)
    {
      if(maskclock == NULL)
        chainclock = ttv_getclocksiglist(ttvfig) ;
      else
        {
          if((type & TTV_FIND_SIG) != TTV_FIND_SIG)
            chainclock = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B,maskclock) ;
          else
            chainclock = maskclock ;
        }
    }
  else
    chainclock=dupchainlst(TTV_EXPLICIT_CLOCK_NODES);

#ifdef SPEED_UP_ACCESS_CRITIC
  // speed up access
  clknames=namefromsig(ttvfig, chainclock);
  if (clknames!=NULL)
    {
      long save0, save1, save2;
      ttv_set_search_exclude_type(0, 0, &save0, &save1);
      save2=TTV_MORE_SEARCH_OPTIONS;
      TTV_MORE_SEARCH_OPTIONS=0;
      subtype=type & (TTV_FIND_PATH|TTV_FIND_LINE|TTV_FIND_MIN|TTV_FIND_MAX);
      TTV_MARK_MODE=TTV_MARK_MODE_DO;
      ttv_getcriticpath(ttvfig,figpath,clknames,NULL,delaymax,delaymin,LONG_MAX,TTV_FIND_DUAL|subtype) ;
 
      TTV_MARK_MODE=TTV_MARK_MODE_USE;
      freechain(clknames);
      ttv_set_search_exclude_type(save0, save1, &save0, &save1);
      TTV_MORE_SEARCH_OPTIONS=save2;
    }
#endif 
  TTV_CHAIN_CLOCK = chainclock;

  path = ttv_getpath(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type|TTV_FIND_ACCESS) ;

#ifdef SPEED_UP_ACCESS_CRITIC
  ttv_FreeMarkedSigList();
  TTV_MARK_MODE=0;
#endif

  if(maskclock==NULL || (type & TTV_FIND_SIG) != TTV_FIND_SIG)
    freechain(TTV_CHAIN_CLOCK) ;
  TTV_CHAIN_CLOCK = NULL ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getsigaccess()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* ptsig : signal de depart de la recherche                                  */
/* mask : list de masques pour les entrees ou sorties                        */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tous les temps d'acces pour les latchs                          */
/*****************************************************************************/
ttvpath_list *ttv_getsigaccess(ttvfig,figpath,ptsig,mask,maskclock,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     chain_list *maskclock ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvpath_list *path ;
  long subtype;
  chain_list *clknames, *chainclock;
 
  TTV_FIND_ALLPATH = 'Y' ;
  if (TTV_EXPLICIT_CLOCK_NODES==NULL)
    {
      if(maskclock == NULL)
        chainclock = ttv_getclocksiglist(ttvfig) ;
      else
        {
          if((type & TTV_FIND_SIG) != TTV_FIND_SIG)
            chainclock = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B,maskclock) ;
          else
            chainclock = maskclock ;
        }
    }
  else
    chainclock=dupchainlst(TTV_EXPLICIT_CLOCK_NODES);

#ifdef SPEED_UP_ACCESS_ALL
  // speed up access
  clknames=namefromsig(ttvfig, chainclock);
  if (clknames!=NULL)
    {
      long saveprec=V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE;
      long save0, save1, save2;
      chain_list *savestartnode=TTV_EXPLICIT_START_NODES;
      ttv_set_search_exclude_type(0, 0, &save0, &save1);
      save2=TTV_MORE_SEARCH_OPTIONS;
      TTV_MORE_SEARCH_OPTIONS=0;
      V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=0;
      subtype=type & (TTV_FIND_PATH|TTV_FIND_LINE|TTV_FIND_MIN|TTV_FIND_MAX);
      TTV_MARK_MODE=TTV_MARK_MODE_DO;
      TTV_EXPLICIT_START_NODES=chainclock;
      ttv_getcriticpath(ttvfig,figpath,clknames,NULL,delaymax,delaymin,INT_MAX,TTV_FIND_DUAL|TTV_FIND_ALL|subtype) ;
      TTV_EXPLICIT_START_NODES=savestartnode;
      freechain(clknames);
      ttv_set_search_exclude_type(save0, save1, &save0, &save1);
      TTV_MORE_SEARCH_OPTIONS=save2;
      V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=saveprec;
      TTV_MARK_MODE=TTV_MARK_MODE_USE;
    }
#endif 
 
  TTV_CHAIN_CLOCK = chainclock;
 
  path = ttv_getpath(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type|TTV_FIND_ALL|TTV_FIND_ACCESS) ;

#ifdef SPEED_UP_ACCESS_ALL
  ttv_FreeMarkedSigList();
  TTV_MARK_MODE=0;
#endif

  if(maskclock==NULL || (type & TTV_FIND_SIG) != TTV_FIND_SIG)
    freechain(TTV_CHAIN_CLOCK) ;
  TTV_CHAIN_CLOCK = NULL ;
  TTV_FIND_ALLPATH = 'N' ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getallpath     ()                     */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* maskin : list de masques pour les entrees                                 */
/* maskout : list de masques pour les sorties                                */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* nb : nombre de chemin a garder                                            */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tout les chemins d'une ttvfig                                   */
/*****************************************************************************/
ttvpath_list *ttv_getallpath(ttvfig,figpath,maskin,maskout,delaymax,delaymin,nb,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     chain_list *maskin ;
     chain_list *maskout ;
     long delaymax ;
     long delaymin ;
     int nb ;
     long type ;
{
  ttvpath_list *path ;

  TTV_FIND_ALLPATH = 'Y' ;
  path = ttv_getcriticpath(ttvfig,figpath,maskin,maskout,delaymax,delaymin,nb,type|TTV_FIND_ALL) ;
  TTV_FIND_ALLPATH = 'N' ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_getallaccess()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* maskin : list de masques pour les entrees                                 */
/* maskout : list de masques pour les sorties                                */
/* delaymax : temps maximum des chemin                                       */
/* delaymin : temps maximum des chemin                                       */
/* nb : nombre de chemin a garder                                            */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche tout les temps d'acces d'une ttvfig                             */
/*****************************************************************************/
ttvpath_list *ttv_getallaccess(ttvfig,figpath,maskin,maskout,maskclock,delaymax,delaymin,nb,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     chain_list *maskin ;
     chain_list *maskout ;
     chain_list *maskclock ;
     long delaymax ;
     long delaymin ;
     int nb ;
     long type ;
{
  ttvpath_list *path ;
  long subtype;
  chain_list *clknames, *chainclock;
 
  TTV_FIND_ALLPATH = 'Y' ;

  if (TTV_EXPLICIT_CLOCK_NODES==NULL)
    {
      if(maskclock == NULL)
        chainclock = ttv_getclocksiglist(ttvfig) ;
      else
        chainclock = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B,maskclock) ;
    }
  else
    chainclock=dupchainlst(TTV_EXPLICIT_CLOCK_NODES);

#ifdef SPEED_UP_ACCESS_ALL
  // speed up access
  clknames=namefromsig(ttvfig, chainclock);
  if (clknames!=NULL)
    {
      long saveprec=V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE;
      long save0, save1, save2;
      chain_list *savestartnode=TTV_EXPLICIT_START_NODES;
      ttv_set_search_exclude_type(0, 0, &save0, &save1);
      save2=TTV_MORE_SEARCH_OPTIONS;
      TTV_MORE_SEARCH_OPTIONS=0;
      V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=0;
      subtype=type & (TTV_FIND_PATH|TTV_FIND_LINE|TTV_FIND_MIN|TTV_FIND_MAX);
      TTV_MARK_MODE=TTV_MARK_MODE_DO;
      TTV_EXPLICIT_START_NODES=chainclock;
      ttv_getcriticpath(ttvfig,figpath,clknames,NULL,delaymax,delaymin,INT_MAX,TTV_FIND_DUAL|TTV_FIND_ALL|subtype) ;
      TTV_EXPLICIT_START_NODES=savestartnode;
      freechain(clknames);
      ttv_set_search_exclude_type(save0, save1, &save0, &save1);
      TTV_MORE_SEARCH_OPTIONS=save2;
      V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=saveprec;
      TTV_MARK_MODE=TTV_MARK_MODE_USE;
    }
#endif 
  TTV_CHAIN_CLOCK = chainclock;
  path = ttv_getcriticpath(ttvfig,figpath,maskin,maskout,delaymax,delaymin,nb,type|TTV_FIND_ALL|TTV_FIND_ACCESS) ;

#ifdef SPEED_UP_ACCESS_ALL
  ttv_FreeMarkedSigList();
  TTV_MARK_MODE=0;
#endif

  freechain(TTV_CHAIN_CLOCK) ;
  TTV_CHAIN_CLOCK = NULL ;
  TTV_FIND_ALLPATH = 'N' ;
  return(path) ;
}

/*****************************************************************************/
/*                        function ttv_testparamask()                        */
/* parametres :                                                              */
/* inf : argument de la recherche de chemin parallele                        */
/*                                                                           */
/* test le mask pour la recherche de chemin parallele                        */
/*****************************************************************************/
int ttv_testparamask(inf)
     ttvinfopara *inf ;
{
  ttvevent_list *nodex ;
  chain_list *chain ;
  chain_list *chainx ;
  chain_list *chainxx ;

  if (((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ONLYNOTHZ)!=0 && (inf->TYPE & TTV_FIND_HZ)!=0)
      || ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ONLYHZ)!=0 && (inf->TYPE & TTV_FIND_HZ)==0)) return 0;

  if((ttv_GET_FINDDELAY(inf->CURNODE, 0) > inf->MAX) || (ttv_GET_FINDDELAY(inf->CURNODE, 0) < inf->MIN))
    return(0) ;

  if((inf->CLOCK == 1) && (TTV_CHAIN_CLOCK != NULL))
    {
      for(chain = TTV_CHAIN_CLOCK ; chain != NULL ; chain = chain->NEXT)
        {
          if((ttvsig_list *)chain->DATA == inf->CURNODE->ROOT)
            break ;
        }
      if(chain == NULL)
        return(0) ;
    }

  if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      if(ttv_isfalsepath(inf->FIG,inf->ROOT,inf->CURNODE,inf->TYPE,0,inf->LATCH) != 0)
        return(0) ;
    }
  else
    {
      if(ttv_isfalsepath(inf->FIG,inf->CURNODE,inf->ROOT,inf->TYPE,0,inf->LATCH) != 0)
        {
          return(0) ;
        }
    }
 
  if (inf->CLOCK)
    {
      ttvevent_list *cmdev;
      // on check si le chemin de clock est valid : on est en mode non dual normalement
      cmdev=ttv_findlatchcommand_notdual(inf->ROOT,inf->CURNODE);
      if (cmdev!=NULL && ttv_isfalsepath(inf->FIG,inf->CURNODE,cmdev,inf->TYPE & ~TTV_FIND_HZ,0,NULL) != 0)
        return(0) ;
    }

  if((inf->MASK == NULL) || 
     ((inf->TYPE & (TTV_FIND_MASKAND|TTV_FIND_MASKOR|TTV_FIND_MASKNOT)) == 0))
    return(1) ;

  if((inf->TYPE & TTV_FIND_MASKAND) == TTV_FIND_MASKAND)
    {
      chain = NULL ;
      for(chainx = inf->MASK ; chainx != NULL ; chainx = chainx->NEXT)
        chain = addchain(chain,chainx->DATA) ;
    }

  nodex = inf->CURNODE ;

  while(nodex->FIND->OUTLINE != NULL)
    {
      if((inf->TYPE & TTV_FIND_MASKAND) == TTV_FIND_MASKAND)
        {
          for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
            {
              chainxx = chainx->NEXT ;
              chainx->NEXT = NULL ;
              if(chainx->DATA != NULL)
                if(ttv_testmask(inf->FIG,nodex->ROOT,chainx) == 1)
                  chainx->DATA = NULL ;
              chainx->NEXT = chainxx ;
            }
        }
      else if((inf->TYPE & TTV_FIND_MASKOR) == TTV_FIND_MASKOR)
        {
          if(ttv_testmask(inf->FIG,nodex->ROOT,inf->MASK) == 1)
            return(1) ;
        }
      else if((inf->TYPE & TTV_FIND_MASKNOT) == TTV_FIND_MASKNOT)
        {
          if(ttv_testmask(inf->FIG,nodex->ROOT,inf->MASK) == 1)
            return(0) ;
        }
      if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          nodex = nodex->FIND->OUTLINE->NODE ;
          if(nodex == inf->ROOT) break ;
        }
      else
        {
          nodex = nodex->FIND->OUTLINE->ROOT ;
          if(nodex == inf->ROOT) break ;
        }
    }

  if((inf->TYPE & TTV_FIND_MASKAND) == TTV_FIND_MASKAND)
    {
      for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
        {
          chainxx = chainx->NEXT ;
          chainx->NEXT = NULL ;
          if(chainx->DATA != NULL)
            if(ttv_testmask(inf->FIG,nodex->ROOT,chainx) == 1)
              chainx->DATA = NULL ;
          chainx->NEXT = chainxx ;
        }
      for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
        {
          if(chainx->DATA != NULL)
            break ;
        }
      freechain(chain) ;
      if(chainx == NULL)
        return(1) ;
    }
  else if((inf->TYPE & TTV_FIND_MASKOR) == TTV_FIND_MASKOR)
    {
      if(ttv_testmask(inf->FIG,nodex->ROOT,inf->MASK) == 1)
        return(1) ;
    }
  else if((inf->TYPE & TTV_FIND_MASKNOT) == TTV_FIND_MASKNOT)
    {
      if(ttv_testmask(inf->FIG,nodex->ROOT,inf->MASK) == 1)
        return(0) ;
    }

  if((inf->TYPE & TTV_FIND_MASKNOT) == TTV_FIND_MASKNOT)
    return(1) ;
  else
    return(0) ;
}

/*****************************************************************************/
/*                        function ttv_findpara()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : niveau minimum de recherche                                     */
/* root : noeud de depart de la recherche                                    */
/* node : noeud d'arrivee de la recherche                                    */
/* max : borne superieur de recherche                                        */
/* min : borne inferieur de recherche                                        */
/* type : type de recherche                                                  */
/* level : niveau de la recherche                                            */
/* mask : mask de rechercher                                                 */
/*                                                                           */
/* fonction de chemin parallele                                              */
/* renvoie le liste des details ou la liste des chemins suivant le type      */
/*****************************************************************************/
void *ttv_findpara(ttvfig,figpath,root,node,max,min,type,level,mask,nb,saveall)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvevent_list *root ;
     ttvevent_list *node ;
     long max ;
     long min ;
     long type ;
     long level ;
     chain_list *mask ;
     int nb ;
     int saveall;
{
  ttvfig_list *ttvfigx ;
  chain_list *chain ;
  ttvinfopara info ;

  ttv_ssta_set_checkloadedstore(ttvfig);

  if(figpath == NULL)
    figpath = root->ROOT->ROOT ;

  info.FIG = ttvfig ;
  info.INS = figpath ;
  info.ROOT = root ;
  info.NODE = node ;
  info.LATCH = NULL ;
  info.CURNODE = NULL ;
  info.MASK = mask ;
  info.MAX = max ; 
  info.MIN = min ; 
  info.LEVEL = level ;
  info.TYPE = type ;
  info.NBPATH = 0 ;
  info.NBMAX = nb ;
  info.LIST.CHAIN = NULL ; // initialise aussi info.LIST.PATHS
  info.CHAINFIG = NULL ;
  info.CLOCK = 0 ;
  info.CLOCKTAB = NULL ;
  info.SAVEALL = saveall;
 
  if(level == ttvfig->INFO->LEVEL)
    {
      ttvfig->STATUS |= TTV_STS_NOT_FREE ;
      info.CHAINFIG = addchain(info.CHAINFIG,ttvfig) ;
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        ttv_parsttvfig(ttvfig,TTV_STS_DENOTINPT|TTV_STS_CLS_FED|TTV_STS_DUAL_D|
                       TTV_STS_DUAL_E|TTV_STS_DUAL_F,TTV_FILE_DTX) ;
      else
        ttv_parsttvfig(ttvfig,TTV_STS_DENOTINPT|TTV_STS_CLS_FED,TTV_FILE_DTX) ;
    }

  ttvfigx = root->ROOT->ROOT ;
  while(ttvfigx != NULL)
    {
      ttvfigx->STATUS |= TTV_STS_NOT_FREE ;
      info.CHAINFIG = addchain(info.CHAINFIG,ttvfigx) ;
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          if(ttvfigx->INFO->LEVEL >= figpath->INFO->LEVEL)
            ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_FE|TTV_STS_DUAL_E|
                           TTV_STS_DUAL_F,TTV_FILE_DTX) ;
          else
            ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_E|TTV_STS_DUAL_E,
                           TTV_FILE_DTX) ;
        }
      else
        {
          if(ttvfigx->INFO->LEVEL >= figpath->INFO->LEVEL)
            ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_FE,TTV_FILE_DTX) ;
          else
            ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_E,TTV_FILE_DTX) ;
        }
      if(ttvfigx->INFO->LEVEL >= level) break ;
      ttvfigx = ttvfigx->ROOT ;
    }

  ttv_fifopush(root) ;
  root->FIND->DELAY = (long)0 ;

  ttv_bypassnode(root);
 
  if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS)==0 || (root->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_B))==0)
    ttv_findp(root,&info) ;
  else
    ttv_findpclock(root,&info) ;
  
  ttv_clean_critic_para_nodes();
  ttv_donotbypassnode(root);

  if((info.NBPATH > info.NBMAX) && 
     ((info.TYPE & TTV_FIND_CRITIC) != TTV_FIND_CRITIC))
    ttv_reduceparapath(&info,1) ;

  for(chain = info.CHAINFIG ; chain != NULL ; chain = chain->NEXT)
    ((ttvfig_list *)chain->DATA)->STATUS &= ~(TTV_STS_NOT_FREE) ;

  freechain(info.CHAINFIG) ;
  /*
    if ((info.TYPE & TTV_FIND_CRITIC) != TTV_FIND_CRITIC)
    {
    for (pth=info.LIST.PATHS, info.LIST.CHAIN=NULL; pth!=NULL; pth=npth)
    {
    npth=pth->NEXT;
    pth->NEXT=NULL;
    info.LIST.CHAIN=addchain(info.LIST.CHAIN, pth);
    }
    info.LIST.CHAIN=reverse(info.LIST.CHAIN);
    }
  */
  return(info.LIST.CHAIN) ;
}

/*****************************************************************************/
/*                        function ttv_reduceparapath()                      */
/* parametres :                                                              */
/* root : noeud courant de la recherche                                      */
/* inf : information sur la recherche                                        */
/* force=1 => pas d'ajout de path, juste reduire                             */
/* force=0 => ajout de path et reduction si necessaire                       */
/*                                                                           */
/* fonction recursive de construction de chemin parallele                    */
/* renvoie le liste des details ou la liste des chemins suivant le type      */
/*****************************************************************************/
ttvpath_list *ttv_reduceparapath(inf,force)
     ttvinfopara *inf ;
     int force;
{
  ttvpath_list *pathx, *npathx ;

 
  if (force)
    {
      ttvpath_list *path ;
      ttvpath_list *pathhead ;
      int nb ;
 
      pathhead = ttv_classpath(inf->LIST.PATHS,inf->TYPE) ;

      for(path = pathhead , nb = 1 ; nb < inf->NBMAX && path != NULL ; path = path->NEXT , nb++) ;

      if(path != NULL)
        {
          ttv_freepathlist(path->NEXT) ;
          path->NEXT = NULL ;

          if((inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
            {
              if(inf->MIN < path->DELAY)
                inf->MIN = path->DELAY ;
            }
          else
            {
              if(inf->MAX > path->DELAY)
                inf->MAX = path->DELAY ;
            }
        }

      inf->LIST.PATHS = pathhead;
      inf->NBPATH = inf->NBMAX ;
    }
  else
    {
      pathx=inf->LIST.PATHS;
      npathx=pathx->NEXT;
 
      if(pathx->LATCH == NULL)
        {
          if(((((pathx->NODE->ROOT->TYPE & TTV_SIG_MASKIN) != TTV_SIG_MASKIN) &&
               ((inf->TYPE & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
              (((pathx->ROOT->ROOT->TYPE & TTV_SIG_MASKOUT) != TTV_SIG_MASKOUT) &&
               ((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL))) && ((inf->SAVEALL & SAVE_ALL_INPUT_TAG)==0))
            {
              pathx->NEXT = NULL ;
              ttv_freepathlist(pathx) ;
              inf->LIST.PATHS=npathx;
              return inf->LIST.PATHS;
            }
        }
      else
        {
          if(((pathx->LATCH->ROOT->TYPE & TTV_SIG_MASKIN) != TTV_SIG_MASKIN) &&
             ((inf->SAVEALL & SAVE_ALL_LATCH_TAG)== 0))
            {
              pathx->NEXT = NULL ;
              ttv_freepathlist(pathx) ;
              inf->LIST.PATHS=npathx;
              return inf->LIST.PATHS;
            }
        }

      if(inf->NBPATH >= inf->NBMAX &&
         (((npathx->DELAY >= pathx->DELAY) && 
           ((inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
          ((npathx->DELAY <= pathx->DELAY) && 
           ((inf->TYPE & TTV_FIND_MIN) == TTV_FIND_MIN))))
        {
          pathx->NEXT = NULL ;
          ttv_freepathlist(pathx) ;
          inf->LIST.PATHS = npathx;
        }
      else
        inf->LIST.PATHS = ttv_keepnbpath(inf->LIST.PATHS,inf->NBMAX, &inf->NBPATH,inf->TYPE, &inner_sort_info,1) ;
    }
 
  return(inf->LIST.PATHS) ;
}

int ttv_check_insertability(ttvevent_list *event, ttvinfopara *inf)
{
  ttvfig_list *ttvfigx ;
  ttvfigx = event->ROOT->ROOT ;
  if((ttvfigx->INFO->LEVEL >= inf->INS->INFO->LEVEL) && (ttvfigx->INFO->LEVEL <= inf->LEVEL))
    {
      if((inf->TYPE & TTV_FIND_CRITIC) != TTV_FIND_CRITIC)
        {
          if(inf->NBPATH >= inf->NBMAX &&
             ((inf->LIST.PATHS->DELAY >= inf->CURNODE->FIND->DELAY*TTV_HIGH_RATIO && 
               (inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX) ||
              (inf->LIST.PATHS->DELAY <= inf->CURNODE->FIND->DELAY*TTV_LOW_RATIO && 
               (inf->TYPE & TTV_FIND_MIN) == TTV_FIND_MIN)))
            {
              return 0;
            }          
        }
    }

  return 1;
}
/*****************************************************************************/
/*                        function ttv_savparapath()                         */
/* parametres :                                                              */
/* root : noeud courant de la recherche                                      */
/* inf : information sur la recherche                                        */
/*                                                                           */
/* recherche les access                                                      */
/*****************************************************************************/
void ttv_savparapath(event,inf)
     ttvevent_list *event ;
     ttvinfopara *inf ;
{
  ttvfig_list *ttvfigx ;
  ttvcritic_list *critic ;
  ptype_list *ptype ;
  chain_list *chainnode, *chain ;
  ttvevent_list *eventstart=event;
  ttvsig_list *tvs;

  if (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH)
  {
    if ((ptype=getptype(inf->CURNODE->USER, TTV_NODE_CRITIC_PARA_INFO))!=NULL)
      {
        if (((inf->TYPE & TTV_FIND_MAX)!=0 && (long)ptype->DATA>inf->CURNODE->FIND->DELAY)
            || ((inf->TYPE & TTV_FIND_MIN)!=0 && (long)ptype->DATA<inf->CURNODE->FIND->DELAY))
        return;
      }
  }
                  
  ttvfigx = event->ROOT->ROOT ;
  if((event->FIND->OUTLINE == NULL) && 
     ((ptype = getptype(event->USER,TTV_NODE_LATCH)) != NULL))
    {
      event = (ttvevent_list *)ptype->DATA ;
    }
 
  chainnode = NULL;
  while(event->FIND->OUTLINE != NULL)
    {
      if(event->FIND->OUTLINE->FIG->INFO->LEVEL > ttvfigx->INFO->LEVEL)
        ttvfigx = event->FIND->OUTLINE->FIG ;

      if (TTV_MARK_MODE==TTV_MARK_MODE_DO)
        {
          tvs=event->ROOT;
          if (getptype(tvs->USER, TTV_SIG_MARKED)==NULL)
            {
              tvs->USER=addptype(tvs->USER, TTV_SIG_MARKED, NULL);
              MARKED_SIG_LIST=addchain(MARKED_SIG_LIST, tvs);
            }
        }
   
      if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          if(event->FIND->OUTLINE->NODE == inf->ROOT) break ;
          // zinaps : pour eviter les boucles
          if((event->FIND->OUTLINE->NODE->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)  
            {
              char buf0[128], buf1[128], buf2[128], buf[1024];
              sprintf(buf, "from '%s' to '%s' on node '%s'",
                      ttv_getsigname(inf->FIG,buf0,inf->ROOT->ROOT),
                      ttv_getsigname(inf->FIG,buf1,eventstart->ROOT),
                      ttv_getsigname(inf->FIG,buf2,event->FIND->OUTLINE->NODE->ROOT));

              ttv_error(54,buf,TTV_WARNING) ;
              for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
                ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
              freechain(chainnode) ;
              return;
            }
          event = event->FIND->OUTLINE->NODE ;
        }
      else
        {
          if(event->FIND->OUTLINE->ROOT == inf->ROOT) break ;

          // zinaps : pour eviter les boucles
          if((event->FIND->OUTLINE->ROOT->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE)
            {
              char buf0[128], buf1[128], buf2[128], buf[1024];
              sprintf(buf, "from '%s' to '%s' on node '%s'",
                      ttv_getsigname(inf->FIG,buf0,eventstart->ROOT),
                      ttv_getsigname(inf->FIG,buf1,inf->ROOT->ROOT),
                      ttv_getsigname(inf->FIG,buf2,event->FIND->OUTLINE->ROOT->ROOT));
              ttv_error(54,buf,TTV_WARNING) ;
              for(chain = chainnode ; chain != NULL ; chain = chain->NEXT)
                ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
              freechain(chainnode) ;
              return;
            }

          event = event->FIND->OUTLINE->ROOT ;
        }
      if((event->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(event->USER,TTV_NODE_LATCH)) != NULL))
        {
          event = (ttvevent_list *)ptype->DATA ;
        }
    }

  if (TTV_MARK_MODE==TTV_MARK_MODE_DO) return;

  if((ttvfigx->INFO->LEVEL >= inf->INS->INFO->LEVEL) && (ttvfigx->INFO->LEVEL <= inf->LEVEL))
    {
      if((inf->TYPE & TTV_FIND_CRITIC) == TTV_FIND_CRITIC)
        {
          if (TTV_CHARAC_CRITIC_SLOPES_LIST!=NULL || TTV_CHARAC_CRITIC_CAPAS_LIST!=NULL)
            {
              if (TTV_CHARAC_CRITIC_RESULT_DELAYS==NULL)
                ttv_DoCharacPropagation(inf->FIG,inf->INS,inf->CURNODE,inf->ROOT,inf->TYPE,TTV_CHARAC_CRITIC_SLOPES_LIST,TTV_CHARAC_CRITIC_CAPAS_LIST, &TTV_CHARAC_CRITIC_RESULT_DELAYS, &TTV_CHARAC_CRITIC_RESULT_SLOPES, &TTV_CHARAC_CRITIC_RESULT_ENERGY, NULL);
              critic = NULL;
            }
          else
            critic = ttv_savcritic(inf->FIG,inf->INS,inf->ROOT,inf->CURNODE,inf->TYPE,1) ;

          if(critic != NULL)
            {
              inf->LIST.CHAIN = addchain(inf->LIST.CHAIN,critic) ;
            }
        }
      else
        {
          // est-ce que l'on doit garder le path?
          if(!(inf->NBPATH >= inf->NBMAX &&
               (((inf->LIST.PATHS->DELAY >= ttv_GET_FINDDELAY(inf->CURNODE, 0)) && 
                 ((inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                ((inf->LIST.PATHS->DELAY <= ttv_GET_FINDDELAY(inf->CURNODE, 0)) && 
                 ((inf->TYPE & TTV_FIND_MIN) == TTV_FIND_MIN)))))
            {
              void *old=inf->LIST.PATHS;
              inf->LIST.PATHS=ttv_savpath(inf->FIG,NULL,inf->LIST.PATHS,inf->ROOT,inf->CURNODE,inf->TYPE);
              if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH)!=0 && old!=inf->LIST.PATHS)
              {
                 if ((ptype=getptype(inf->CURNODE->USER, TTV_NODE_CRITIC_PARA_INFO))!=NULL)
                   ptype->DATA=(void *)inf->CURNODE->FIND->DELAY;
                 else
                 {
                   inf->CURNODE->USER=addptype(inf->CURNODE->USER, TTV_NODE_CRITIC_PARA_INFO, (void *)inf->CURNODE->FIND->DELAY);
                   TTV_CRITIC_PARA_SEARCH=addchain(TTV_CRITIC_PARA_SEARCH, inf->CURNODE);
                 }
              }
              ttv_reduceparapath(inf,0) ;
            }
        }
    }
}

/*****************************************************************************/
/*                        function ttv_findpclock()                          */
/* parametres :                                                              */
/* root : noeud courant de la recherche                                      */
/* inf : information sur la recherche                                        */
/*                                                                           */
/* recherche les access                                                      */
/*****************************************************************************/

static int TTV_NB_FOUND=0;

void ttv_findpclock(event,inf)
     ttvevent_list *event ;
     ttvinfopara *inf ;
{
  ptype_list *cmdlist ;
  ptype_list *ptype ;
  ptype_list *ptypex ;
  ttvevent_list *cmdevent ; 
  ttvline_list *ptline ;
  chain_list *line_list;
  int condtrue;
  long delay ;
  long search, insert ;

  TTV_NB_FOUND=0;

  if ((TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS)!=0)
    cmdlist=ttv_getlatchasynccmd(inf->FIG,event,inf->TYPE);
  else
    cmdlist = ttv_getlatchaccess(inf->FIG,event,inf->TYPE) ;

  for(ptype = cmdlist ; ptype != NULL ; ptype = ptype->NEXT)
    {
      cmdevent = (ttvevent_list *)ptype->DATA ;

      if (ttv_isbypassednode(cmdevent)) continue;

      ttv_fifosave();

      if(ptype->TYPE != (long)0)
        {
          ptline = (ttvline_list *)ptype->TYPE ;
          if((inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
            {
              if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 1))==TTV_NOTIME)
                delay = ttv_getdelaymax(ptline);
            }
          else
            {                         
              if ((delay=stb_gettruncatedaccess(ptline->ROOT, ptline->NODE, 0))==TTV_NOTIME)
                delay = ttv_getdelaymin(ptline) ;
            }
        }
      else
        {
          ptline = NULL ;
          delay = (long)0 ;
        }
      ttv_fifopush(cmdevent) ;
      if((ptypex = getptype(cmdevent->USER,TTV_NODE_LATCH)) == NULL)
        {
          cmdevent->USER = addptype(cmdevent->USER,TTV_NODE_LATCH,event) ;
          cmdevent->FIND->DELAY = delay + event->FIND->DELAY ;
        }
      else
        {
          ptypex->DATA = event ;
          cmdevent->FIND->DELAY = delay + event->FIND->DELAY ;
        }
      cmdevent->FIND->OUTLINE = ptline ;
      inf->CURNODE = cmdevent ;
      inf->LATCH=event;
      inf->CLOCK = 1 ;
      condtrue=0;
      if(((cmdevent == inf->NODE) || 
          ((inf->NODE == NULL) && 
           (((search = ttv_signalsearchtype(inf->FIG,cmdevent,inf->TYPE)) & TTV_SIG_ENDSEARCH) == TTV_SIG_ENDSEARCH))))
        {
          if(!(V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && (insert=ttv_check_insertability(cmdevent, inf))==0))
            {
              if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                line_list=ttv_DoPropagation_PRECISION_LEVEL1(inf->FIG, NULL, inf->CURNODE, inf->ROOT, inf->TYPE);
              
              if (ttv_testparamask(inf) != 0)
                {
                  TTV_NB_FOUND++;
                  ttv_savparapath(cmdevent,inf) ;
                  condtrue=1;
                }
              
              if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
            }
          else if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && insert==0 && ttv_testparamask(inf)!=0) TTV_NB_FOUND++;
        }
      if(condtrue==0 && (((inf->CURNODE != inf->NODE) && (inf->NODE != NULL)) || 
                         ((inf->NODE == NULL) && ((search & TTV_SIG_CROSS) == TTV_SIG_CROSS))))
        {
          ttv_findp(cmdevent,inf) ;
        }
      inf->CLOCK = 0 ;
      inf->LATCH = NULL;
      ttv_fiforestore();
    }
 
  if (TTV_QUIET_MODE==0 && (inf->TYPE & TTV_FIND_CRITIC)!=TTV_FIND_CRITIC && TTV_NB_FOUND==0 && (event->ROOT->TYPE & (TTV_SIG_B|TTV_SIG_C|TTV_SIG_Q))==0)
    {
      char buf0[128];
      ttv_error(56,ttv_getsigname(inf->FIG,buf0,event->ROOT),TTV_WARNING);
    }

  freeptype(cmdlist) ; 
}

/*****************************************************************************/
/*                        function ttv_findp()                               */
/* parametres :                                                              */
/* root : noeud courant de la recherche                                      */
/* inf : information sur la recherche                                        */
/*                                                                           */
/* fonction recursive de construction de chemin parallele                    */
/* renvoie le liste des details ou la liste des chemins suivant le type      */
/*****************************************************************************/
void ttv_findp(root,inf)
     ttvevent_list *root ;
     ttvinfopara *inf ;
{
  static ttvfig_list *ttvfigx ;
  static ptype_list *ptype ;
  static long delay ;
  static int abort;
  static chain_list *line_list;
  long search ;
  ttvevent_list *nodex ;
  ttvline_list *in ;
  chain_list *chain ;

  if (TTV_MARK_MODE==TTV_MARK_MODE_USE && inf->CLOCK!=0 && getptype(root->ROOT->USER, TTV_SIG_MARKED)==NULL) return;
  if ((inf->CLOCK!=0 || (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MUST_BE_CLOCK)!=0) && ttv_testsigflag(root->ROOT, TTV_SIG_FLAGS_ISDATA)) return;
 
  if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      if(((root->ROOT->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN) ||
         ((root->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
        return ;
      ptype = getptype(root->USER,TTV_NODE_DUALLINE) ;
      if(ptype == NULL) in = NULL ;
      else
        {
          chain = (chain_list *)ptype->DATA ;
          if(chain == NULL) in = NULL ;
          else in = (ttvline_list *)chain->DATA ;
        }
    }
  else
    {
      if(((root->ROOT->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT) ||
         ((root->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT))
        return ;
      in =  root->INLINE ;
    }
 
  while(in != NULL)
    {
      if(((in->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
         (((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
          ((in->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
          (ttv_islineonlyend(inf->FIG,in,inf->TYPE) == 1)))
        {
          if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            {
              chain = chain->NEXT ;
              if(chain == NULL) in = NULL ;
              else in = (ttvline_list *)chain->DATA ;
            }
          else
            in = in->NEXT ;
          continue ;
        }
      if((((inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX) && (in->VALMAX == TTV_NOTIME))
         ||(((inf->TYPE & TTV_FIND_MIN) == TTV_FIND_MIN) && (in->VALMIN == TTV_NOTIME)))
     
        {
          if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            {
              chain = chain->NEXT ;
              if(chain == NULL) in = NULL ;
              else in = (ttvline_list *)chain->DATA ;
            }
          else
            in = in->NEXT ;
          continue ;
        }
      if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          nodex = in->ROOT ;
          if(((nodex->ROOT->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT) ||
             ((nodex->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT))
            {
              chain = chain->NEXT ;
              if(chain == NULL) in = NULL ;
              else in = (ttvline_list *)chain->DATA ;
              continue ;
            }
          if(((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
             (root->FIND->OUTLINE != NULL))
            {
              if((root->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
                {
                  chain = chain->NEXT ;
                  if(chain == NULL) in = NULL ;
                  else in = (ttvline_list *)chain->DATA ;
                  continue ;
                }
            }
          if(((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
             ((nodex->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
             ((nodex->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
            {
              if(inf->CLOCK || ((((in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) &&
                                 ((inf->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)) ||
                                (((in->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ) &&
                                 ((inf->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ))))
                {
                  chain = chain->NEXT ;
                  if(chain == NULL) in = NULL ;
                  else in = (ttvline_list *)chain->DATA ;
                  continue ;
                }
            }
          if(((nodex->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
             ((nodex->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
            {
              if((((in->TYPE & TTV_LINE_R) == TTV_LINE_R) &&
                  ((inf->TYPE & TTV_FIND_S) == TTV_FIND_S)) ||
                 (((in->TYPE & TTV_LINE_S) == TTV_LINE_S) &&
                  ((inf->TYPE & TTV_FIND_R) == TTV_FIND_R)))
                {
                  chain = chain->NEXT ;
                  if(chain == NULL) in = NULL ;
                  else in = (ttvline_list *)chain->DATA ;
                  continue ;
                }
            }
        }
      else
        {
          nodex = in->NODE ;
          if(((nodex->ROOT->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN) ||
             ((nodex->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
            {
              in = in->NEXT ;
              continue ;
            }
          if(((in->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
             (root->FIND->OUTLINE != NULL))
            {
              if((root->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
                {
                  in = in->NEXT ;
                  continue ;
                }
            }
          if(((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
             ((root->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
             ((root->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT))
            {
              if (inf->CLOCK && (in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ)
                {
                  in = in->NEXT ;
                  continue ;
                }
              if(!inf->CLOCK && ((((in->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) &&
                                  ((inf->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)) ||
                                 (((in->TYPE & TTV_LINE_HZ) != TTV_LINE_HZ) &&
                                  ((inf->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ))))
                {
                  in = in->NEXT ;
                  continue ;
                }
            }
          if(((root->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
             ((root->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
            {
              if((((in->TYPE & TTV_LINE_R) == TTV_LINE_R) &&
                  ((inf->TYPE & TTV_FIND_S) == TTV_FIND_S)) ||
                 (((in->TYPE & TTV_LINE_S) == TTV_LINE_S) &&
                  ((inf->TYPE & TTV_FIND_R) == TTV_FIND_R)))
                {
                  in = in->NEXT ;
                  continue ;
                }
            }
        }
 
      if((((in->TYPE & TTV_LINE_D) == TTV_LINE_D) && 
          ((inf->FIG != in->FIG) || (inf->FIG->INFO->LEVEL > inf->LEVEL))) ||
         (((in->TYPE & TTV_LINE_E) == TTV_LINE_E) && (inf->LEVEL < in->FIG->INFO->LEVEL)) ||
         (((in->TYPE & TTV_LINE_F) == TTV_LINE_F) && ((inf->LEVEL < in->FIG->INFO->LEVEL) ||
                                                      (inf->INS->INFO->LEVEL > in->FIG->INFO->LEVEL)))) 
        {
          if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            {
              chain = chain->NEXT ;
              if(chain == NULL) in = NULL ;
              else in = (ttvline_list *)chain->DATA ;
            }
          else in = in->NEXT ;
          continue ;
        }
 
      /*
        if((((nodex->TYPE & TTV_NODE_STOP) == TTV_NODE_STOP) &&
        (((((((nodex->ROOT->TYPE & (TTV_SIG_CB|TTV_SIG_L|TTV_SIG_R)) 
        != TTV_SIG_CB) &&
        ((nodex->ROOT->TYPE & (TTV_SIG_CT|TTV_SIG_L|TTV_SIG_R)) 
        != TTV_SIG_CT)) ||
        (ttv_islocononlyend(inf->FIG,nodex,inf->TYPE) != 0))) &&
        ((nodex->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_L|TTV_SIG_R)) != TTV_SIG_N)) &&
        ((nodex->ROOT->TYPE & TTV_SIG_Q) != TTV_SIG_Q) &&
        ((nodex->ROOT->TYPE & TTV_SIG_B) != TTV_SIG_B))) && 
        ((nodex != inf->NODE) && (inf->NODE != NULL) && 
        (TTV_CHAIN_CLOCK == NULL) && 
        (ttv_iscrosslatch(inf->FIG,nodex,inf->TYPE) == 0)))
        {
        if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
        chain = chain->NEXT ;
        if(chain == NULL) in = NULL ;
        else in = (ttvline_list *)chain->DATA ;
        }
        else in = in->NEXT ;
        continue ;
        }
      */

      if(root->ROOT->ROOT != nodex->ROOT->ROOT)
        {
          ttvfigx = nodex->ROOT->ROOT ;
          while((ttvfigx != inf->FIG) && ((ttvfigx->STATUS &
                                           TTV_STS_NOT_FREE) != TTV_STS_NOT_FREE))
            {
              ttvfigx->STATUS |= TTV_STS_NOT_FREE ;
              inf->CHAINFIG = addchain(inf->CHAINFIG,ttvfigx) ;
              if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_E|TTV_STS_DUAL_E,
                               TTV_FILE_DTX) ;
              else
                ttv_parsttvfig(ttvfigx,TTV_STS_DENOTINPT|TTV_STS_LS_E,TTV_FILE_DTX) ;
              ttvfigx = ttvfigx->ROOT ;
            }
        }

      if((inf->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
        delay = ttv_getdelaymax(in) ;
      else
        delay = ttv_getdelaymin(in) ;

      if (/*inf->CLOCK!=0 && */ttv_isbypassednode(nodex)) abort=1; else abort=0;

      if(abort || ttv_transfertnodefindinfo(inf->FIG,nodex,root,in,delay,inf->TYPE) == 0)
        {
          if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            {
              chain = chain->NEXT ;
              if(chain == NULL) in = NULL ;
              else in = (ttvline_list *)chain->DATA ;
            }
          else in = in->NEXT ;
          continue ;
        }

      search = ttv_signalsearchtype(inf->FIG,nodex,inf->TYPE) ;

      inf->CURNODE = nodex ;

      if((TTV_CHAIN_CLOCK != NULL) && 
         ((nodex->ROOT->TYPE & (TTV_SIG_B|TTV_SIG_L|TTV_SIG_R)) != 0) && (inf->CLOCK == 0))
        {
          int isfalse=0;
          // on check si le chemin de data est un falsepath
          if (ttv_canbeinfalsepath(nodex, 'i') && ttv_canbeinfalsepath(inf->ROOT, 'o') && ttv_isfalsepath(inf->FIG,nodex, inf->ROOT,inf->TYPE,0,NULL))
            isfalse=1;

          if (!isfalse)
            {
              ttv_bypassnode(nodex);
              if((nodex->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                {
                  long oldphase;
                  char oldflags;
                  ttvfind_stb_stuff *savetss;
                  inf->CLOCK = 1 ;
                  savetss=inf->CURNODE->FIND->STB;
                  oldphase=inf->CURNODE->FIND->ORGPHASE;
                  oldflags=inf->CURNODE->FIND->FLAGS;
                  inf->CURNODE->FIND->FLAGS&=~TTV_FINDSTRUCT_CROSS_ENABLED;
                  inf->CURNODE->FIND->STB=NULL;
                  inf->CURNODE->FIND->ORGPHASE=TTV_NO_PHASE;
                  nodex->USER = addptype(nodex->USER,TTV_NODE_LATCH,nodex) ;
                  ttv_findp(inf->CURNODE,inf) ;
                  nodex->USER = delptype(nodex->USER,TTV_NODE_LATCH) ;
                  inf->CURNODE = nodex ;
                  inf->CURNODE->FIND->ORGPHASE=oldphase;
                  inf->CURNODE->FIND->STB=savetss;
                  inf->CURNODE->FIND->FLAGS=oldflags;
                  inf->CLOCK = 0 ;
                }
              else
                {
                  ttv_findpclock(nodex,inf) ;
                  inf->CURNODE = nodex ;
                }
              ttv_donotbypassnode(nodex);
            }
        }
      else if(((nodex == inf->NODE) || 
               ((inf->NODE == NULL) && 
                ((search & TTV_SIG_ENDSEARCH) == TTV_SIG_ENDSEARCH))))
        {
          if ((TTV_CHAIN_CLOCK == NULL && (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS)==0)
              || inf->CLOCK
              || (inf->CLOCK==0 && (TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_REAL_ACCESS)==0))
            {
              // PS: j'utilise 'abort' dans la fonction recursive mais ca
              // aurait plutot du etre une nouvelle variable 'insert'
              if(!(V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && (abort=ttv_check_insertability(nodex, inf))==0))
                {

                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    line_list=ttv_DoPropagation_PRECISION_LEVEL1(inf->FIG, NULL, inf->CURNODE, inf->ROOT, inf->TYPE);
             
                  if (ttv_testparamask(inf) != 0)
                    {
                      TTV_NB_FOUND++;
                      ttv_savparapath(nodex,inf) ;
                      inf->ROOT->FIND->DELAY = (long)0 ;
                    }
             
                  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE>=1)
                    ttv_CleanPropagation_PRECISION_LEVEL1(line_list);
                }
              else if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE==1 && abort==0 && ttv_testparamask(inf)!=0) TTV_NB_FOUND++;
            }
        }
      if((((inf->CURNODE != inf->NODE) && (inf->NODE != NULL)) || 
          (inf->NODE == NULL)) && ((search & TTV_SIG_CROSS) == TTV_SIG_CROSS))
        {
          /*     if (inf->CLOCK==0)*/ ttv_bypassnode(nodex);
          ttv_findp(inf->CURNODE,inf) ;
          /*     if (inf->CLOCK==0)*/ ttv_donotbypassnode(nodex);
        }
      else
        {
          inf->ROOT->FIND->DELAY = (long)0 ;
        }
      if((inf->TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          chain = chain->NEXT ;
          if(chain == NULL) in = NULL ;
          else in = (ttvline_list *)chain->DATA ;
        }
      else in = in->NEXT ;
    }
}

/*****************************************************************************/
/*                        function ttv_getcriticpara()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* root : noeud de depart de la recherche                                    */
/* root : noeud d'arrivee de la recherche                                    */
/* delay : borne superieur de recherche                                      */
/* type : type de recherche                                                  */
/* mask : mask de rechercher                                                 */
/*                                                                           */
/* recherche les detail d'un chemin parallele entre deux noeuds              */
/* qui ont un delai particuliers                                             */
/*****************************************************************************/
chain_list *ttv_getcriticpara(ttvfig_list *ttvfig,ttvfig_list *figpath,ttvevent_list *root,ttvevent_list *node,long delay, long type, chain_list *mask, long moreopt)
{
  ttvfig_list *ttvfigx ;
  ttvevent_list *nodex ;
  chain_list *chain ;
  long level ;
  long levelroot ;
  long levelnode, comptag ;

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      type &= ~(TTV_FIND_DUAL) ;
      if (node==NULL)
        TTV_CHAIN_CLOCK = ttv_getclocksiglist(ttvfig) ;
      else 
        TTV_CHAIN_CLOCK = addchain(TTV_CHAIN_CLOCK,node->ROOT) ;
    }

  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      nodex = node ;
      node = root ;
      root = nodex ; 
    }

  if(figpath != NULL)
    {
      level = figpath->INFO->LEVEL ;
    }
  else
    {
      levelroot = ttv_getsiglevel(root->ROOT) ;
      levelnode = ttv_getsiglevel(node->ROOT) ;
      if(levelroot > ttvfig->INFO->LEVEL) levelroot = ttvfig->INFO->LEVEL ;
      if(levelnode > ttvfig->INFO->LEVEL) levelnode = ttvfig->INFO->LEVEL ;
      if(levelroot > levelnode) level = levelnode ;
      else level = levelroot ;

      for(ttvfigx = root->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS |= TTV_STS_MARQUE ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }
 
      for(ttvfigx = node->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        if((ttvfigx->STATUS & TTV_STS_MARQUE) == TTV_STS_MARQUE)
          break ;
 
      figpath = ttvfigx ;
 
      for(ttvfigx = root->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS &= ~(TTV_STS_MARQUE) ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }
    }

  if (node!=NULL) chain=addchain(NULL, node);
  else chain=NULL;
  comptag=computesavealltag(chain, 0);
  freechain(chain);

  if (moreopt & TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH)
    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH;
  TTV_SEARCH_FIRST_NODE=root;
  chain = (chain_list *)ttv_findpara(ttvfig,figpath,root,node,delay,delay,
                                     type|TTV_FIND_CRITIC,level,mask,0,comptag) ;
  ttv_fifoclean() ;
  TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH;

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      freechain(TTV_CHAIN_CLOCK) ;
      TTV_CHAIN_CLOCK = NULL ;
    }

  return(chain) ;
}

/*****************************************************************************/
/*                        function ttv_freecriticpara()                      */
/* parametres :                                                              */
/* pthead : liste des details de chemin parallele                            */
/*                                                                           */
/* libere les details d'un chemin parallele                                  */
/*****************************************************************************/
int ttv_freecriticpara(pthead)
     chain_list *pthead ;
{
  chain_list *chain ;

  if(pthead == NULL) return(0) ;
 
  for(chain = pthead ; chain != NULL ; chain = chain->NEXT)
    {
      ttv_freecriticlist((ttvcritic_list *)chain->DATA) ;
    }

  freechain(pthead) ;

  return(1) ;
}

/*****************************************************************************/
/*                        function ttv_getpara()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure de recherche d'un chemin                                 */
/* root : noeud de depart de la recherche                                    */
/* root : noeud d'arrivee de la recherche                                    */
/* delaymax : borne superieur de recherche                                   */
/* delaymin : borne inferieur de recherche                                   */
/* type : type de recherche                                                  */
/* mask : mask de rechercher                                                 */
/*                                                                           */
/* recherche de chemin parrallele entre deux noeuds particuliers             */
/*****************************************************************************/
ttvpath_list *ttv_getpara(ttvfig,figpath,root,node,max,min,type,mask,nb)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvevent_list *root ;
     ttvevent_list *node ;
     long max ;
     long min ;
     long type ;
     chain_list *mask ;
     int nb ;
{
  ttvfig_list *ttvfigx ;
  ttvevent_list *nodex ;
  ttvpath_list *path ;
  // chain_list *chain ;
  // chain_list *chainx ;
  long level ;
  long levelroot ;
  long levelnode ;

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      type &= ~(TTV_FIND_DUAL) ;
      TTV_CHAIN_CLOCK = addchain(TTV_CHAIN_CLOCK,node->ROOT) ;
    }

  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      nodex = node ;
      node = root ;
      root = nodex ; 
    }

  if(figpath != NULL)
    {
      level = figpath->INFO->LEVEL ;
    }
  else
    {
      levelroot = ttv_getsiglevel(root->ROOT) ;
      levelnode = ttv_getsiglevel(node->ROOT) ;
      if(levelroot > ttvfig->INFO->LEVEL) levelroot = ttvfig->INFO->LEVEL ;
      if(levelnode > ttvfig->INFO->LEVEL) levelnode = ttvfig->INFO->LEVEL ;
      if(levelroot > levelnode) level = levelnode ;
      else level = levelroot ;
 
      for(ttvfigx = root->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS |= TTV_STS_MARQUE ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }
 
      for(ttvfigx = node->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        if((ttvfigx->STATUS & TTV_STS_MARQUE) == TTV_STS_MARQUE)
          break ;
 
      figpath = ttvfigx ;
 
      for(ttvfigx = root->ROOT->ROOT ; ttvfigx != NULL ; ttvfigx = ttvfigx->ROOT)
        {
          ttvfigx->STATUS &= ~(TTV_STS_MARQUE) ;
          if((ttvfigx->INFO->LEVEL >= level) || (ttvfigx == ttvfig)) break ;
        }
    }

  if(figpath == NULL)
    {
      if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
        {
          freechain(TTV_CHAIN_CLOCK) ;
          TTV_CHAIN_CLOCK = NULL ;
        }
      return(NULL) ;
    }

  path = (ttvpath_list *)ttv_findpara(ttvfig,figpath,root,node,
                                      max,min,type,level,mask,nb,SAVE_ALL_TAG) ;
  ttv_fifoclean() ;

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      freechain(TTV_CHAIN_CLOCK) ;
      TTV_CHAIN_CLOCK = NULL ;
    }

  if((type & TTV_FIND_NOTCLASS) == TTV_FIND_NOTCLASS)
    return(path) ;
  else
    return(ttv_classpath(path,type)) ;
}

/*****************************************************************************/
/*                        function ttv_getdelay()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* maskin : list de masques pour les entrees                                 */
/* maskout : list de masques pour les sorties                                */
/* delaymax : temps maximum des delais                                       */
/* delaymin : temps maximum des delais                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* recherche les delais d'une figure                                         */
/*****************************************************************************/
chain_list *ttv_getdelay(ttvfig,figpath,maskin,maskout,delaymax,delaymin,type)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     chain_list *maskin ;
     chain_list *maskout ;
     long delaymax ;
     long delaymin ;
     long type ;
{
  ttvlbloc_list *ptlbloc[3] ;
  ttvline_list *ptline ;
  ttvpath_list *path = NULL ;
  ttvpath_list *pathx ;
  ttvcritic_list *critic ;
  ttvevent_list *cmd ;
  chain_list *delaylist = NULL ;
  long typer ;
  long i ;
  long j ;
  long delay ;
  long newdelay ;
  long slope ;
  long newslope ;

  if(figpath == NULL)
    figpath = ttvfig ;

  if(figpath == ttvfig)
    ttv_parsttvfig(figpath,TTV_STS_CLS_FED,TTV_FILE_DTX) ;
  else
    ttv_parsttvfig(figpath,TTV_STS_LS_FE,TTV_FILE_DTX) ;

  ptlbloc[0] = figpath->FBLOC ;
  ptlbloc[1] = figpath->EBLOC ;

  if(figpath == ttvfig)
    ptlbloc[2] = figpath->DBLOC ;
  else
    ptlbloc[2] = NULL ;

  for(i = 0 ; i < 3 ; i++)
    for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
      {
        for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
          {
            ptline = ptlbloc[i]->LINE + j ;
            if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
              continue ;

            if((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
              continue ;

            if((((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                ((type & TTV_FIND_RC) != TTV_FIND_RC)) ||
               (((ptline->TYPE & TTV_LINE_RC) != TTV_LINE_RC) &&
                ((type & TTV_FIND_GATE) != TTV_FIND_GATE)))
              continue ;

            if((((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
                ((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
                ((type & TTV_FIND_NOT_UPUP) == TTV_FIND_NOT_UPUP)) ||
               (((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
                ((ptline->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
                ((type & TTV_FIND_NOT_UPDW) == TTV_FIND_NOT_UPDW)) ||
               (((ptline->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
                ((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
                ((type & TTV_FIND_NOT_DWUP) == TTV_FIND_NOT_DWUP)) ||
               (((ptline->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
                ((ptline->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) &&
                ((type & TTV_FIND_NOT_DWDW) == TTV_FIND_NOT_DWDW)))
              continue ;
 
            if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
              {
                if((delay = ptline->VALMAX) == TTV_NOTIME)
                  continue ; ;
                newdelay = ttv_getdelaymax(ptline) ;
                slope = ptline->FMAX ;
                newslope = ttv_getslopemax(ptline) ;
                cmd = ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMAX) ;
              }
            else
              {
                if((delay = ptline->VALMIN) == TTV_NOTIME)
                  continue ; ;
                newdelay = ttv_getdelaymin(ptline) ;
                slope = ptline->FMIN ;
                newslope = ttv_getslopemin(ptline) ;
                cmd = ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMIN) ;
              }

            if((newdelay >= delaymin) && (newdelay <= delaymax) &&
               ((ttv_testmask(ttvfig,ptline->NODE->ROOT,maskin) == 1) ||
                (maskin == NULL)) &&
               ((ttv_testmask(ttvfig,ptline->ROOT->ROOT,maskout) == 1) ||
                (maskout == NULL)))
              {
                if((ptline->TYPE & TTV_LINE_R) == TTV_LINE_R) 
                  typer = TTV_FIND_R ;
                else if((ptline->TYPE & TTV_LINE_S) == TTV_LINE_S)
                  typer = TTV_FIND_S ;
                else if((ptline->TYPE & TTV_LINE_PR) == TTV_LINE_PR) 
                  typer = TTV_FIND_PR ;
                else if((ptline->TYPE & TTV_LINE_EV) == TTV_LINE_EV) 
                  typer = TTV_FIND_EV ;
                else typer = (long)0 ;
                if((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
                  typer |= TTV_FIND_RC ;
                if((ptline->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ)
                  typer |= TTV_FIND_HZ ;
                if((ptline->TYPE & TTV_LINE_R) == TTV_LINE_R)
                  typer |= TTV_FIND_R ;
                if((ptline->TYPE & TTV_LINE_S) == TTV_LINE_S)
                  typer |= TTV_FIND_S ;
                path = ttv_allocpath(path,figpath,ptline->ROOT,ptline->NODE,cmd,
                                     NULL,NULL,NULL,(long)0,(long)0,(long)0,type|typer,
                                     delay,slope,
                                     newdelay,newslope,(long)0,(long)0,NULL,NULL,0,STB_NO_INDEX,0) ;
                ttv_getmodelline(ptline,(char **)(&path->MD),(char **)(&path->MF)) ;
              }
          }
      }

  if((type & TTV_FIND_NOTCLASS) != TTV_FIND_NOTCLASS)
    path = ttv_classpath(path,type) ;

  for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT)
    {
      critic = NULL ;
      critic = ttv_alloccritic(critic,ttvfig,pathx->FIG,pathx->ROOT,pathx->TYPE,
                               (long)0,pathx->REFDELAY,pathx->REFSLOPE,pathx->DELAY,pathx->SLOPE,0,NULL,NULL) ;
      critic->MODNAME = (char *)pathx->MD ;
      critic->INSNAME = (char *)pathx->MF ;
      critic = ttv_alloccritic(critic,ttvfig,pathx->FIG,pathx->NODE,pathx->TYPE,
                               (long)0,(long)0,(long)0,(long)0,(long)0,0,NULL,NULL) ;
      critic->MODNAME = (char *)pathx->MD ;
      critic->INSNAME = (char *)pathx->MF ;
      delaylist = addchain(delaylist,(void *)critic) ;
      pathx->MD = NULL;
      pathx->MF = NULL;
    }

  ttv_freepathlist(path) ;

  return(reverse(delaylist)) ;
}

/*****************************************************************************/
/*                        function ttv_getconstraint()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* figpath : figure hierarchique ou null                                     */
/* ptsig : signal ou l'on veut la contraintes L ou B                         */
/* type : max min access or setup ou hold                                    */
/* file : fichier dtx ou ttx elais                                           */
/*                                                                           */
/* recherche les delais d'une figure                                         */
/*****************************************************************************/
chain_list *ttv_getconstraint(ttvfig_list *ttvfig,ttvfig_list *figpath,ttvsig_list *ptsig,long type,long file,int avoiddir)
{
  ttvline_list *ptline ;
  long level ;
  long dual ;
  ttvpath_list *path = NULL ;
  ttvpath_list *pathx ;
  ttvcritic_list *critic ;
  chain_list *delaylist = NULL ;
  ptype_list *ptype ;
  chain_list *chain ;
  long i ;
  long delay ;
  long newdelay ;
  long slope ;
  long newslope ;
 
  if(figpath == NULL)
    level = 1 ;
  else
    level = figpath->INFO->LEVEL ;

  if((type & (TTV_FIND_SETUP|TTV_FIND_HOLD)) != 0)
    {
      if((file & TTV_FILE_DTX) == TTV_FILE_DTX)
        dual = TTV_STS_DUAL_F|TTV_STS_DUAL_E|TTV_STS_DUAL_D ;
      else
        dual = TTV_STS_DUAL_P|TTV_STS_DUAL_J|TTV_STS_DUAL_T ;
    }
  else 
    dual = (long)0 ;

  if((file & TTV_FILE_DTX) == TTV_FILE_DTX)
    ttv_expfigsig(ttvfig,ptsig,level,ttvfig->INFO->LEVEL,
                  TTV_STS_CLS_FED|dual, TTV_FILE_DTX);
  else
    ttv_expfigsig(ttvfig,ptsig,level,ttvfig->INFO->LEVEL,
                  TTV_STS_CL_PJT|dual, TTV_FILE_TTX);


  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    for(i = 0 ; i < 2 ; i++)
      {
        if (i==avoiddir) continue;
        if((file & TTV_FILE_DTX) == TTV_FILE_DTX)
          ptline = (ptsig->NODE + i)->INLINE ;
        else
          ptline = (ptsig->NODE + i)->INPATH ;

        for(; ptline != NULL ; ptline = ptline->NEXT)
          {
            if((ptline->TYPE & TTV_LINE_A) != TTV_LINE_A)
              continue ;
  
            if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
              {
                if((delay = ptline->VALMAX) == TTV_NOTIME)
                  continue ; ;
                newdelay = ttv_getdelaymax(ptline) ;
                slope = ptline->FMAX ;
                newslope = ttv_getslopemax(ptline) ;
              }
            else
              {
                if((delay = ptline->VALMIN) == TTV_NOTIME)
                  continue ; ;
                newdelay = ttv_getdelaymin(ptline) ;
                slope = ptline->FMIN ;
                newslope = ttv_getslopemin(ptline) ;
              }
  
            path = ttv_allocpath(path,figpath,ptline->ROOT,ptline->NODE,NULL,
                                 NULL,NULL,NULL,(long)0,(long)0,(long)0,
                                 type &= ~(TTV_FIND_SETUP|TTV_FIND_HOLD),
                                 delay,slope,
                                 newdelay,newslope,(long)0,(long)0,NULL,NULL,0,STB_NO_INDEX,0) ;
          }
      }

  if((type & (TTV_FIND_SETUP|TTV_FIND_HOLD)) != 0)
    for(i = 0 ; i < 2 ; i++)
      {
        if (i==avoiddir) continue;
        if((file & TTV_FILE_DTX) == TTV_FILE_DTX)
          ptype = getptype((ptsig->NODE+i)->USER,TTV_NODE_DUALLINE) ;
        else
          ptype = getptype((ptsig->NODE+i)->USER,TTV_NODE_DUALPATH) ;

        if(ptype == NULL) chain = NULL ;
        else
          {
            chain = (chain_list *)ptype->DATA ;
          }

        for(; chain != NULL ; chain = chain->NEXT)
          {
            ptline = (ttvline_list *)chain->DATA ;

            if((ptline->TYPE & (TTV_LINE_U|TTV_LINE_O)) == 0)
              continue ;

            if((((ptline->TYPE & TTV_LINE_U) == TTV_LINE_U) &&
                ((type & TTV_FIND_SETUP) != TTV_FIND_SETUP)) ||
               (((ptline->TYPE & TTV_LINE_O) == TTV_LINE_O) &&
                ((type & TTV_FIND_HOLD) != TTV_FIND_HOLD)))
              continue ;
  
            if((delay = ptline->VALMAX) == TTV_NOTIME)
              continue ; ;

            newdelay = ttv_getdelaymax(ptline) ;

            slope = ptline->FMAX ;
            newslope = ttv_getslopemax(ptline) ;
  
            path = ttv_allocpath(path,figpath,ptline->ROOT,ptline->NODE,NULL,
                                 NULL,NULL,NULL,(long)0,(long)0,(long)0,
                                 ((ptline->TYPE & TTV_LINE_U) == TTV_LINE_U) ? 
                                 (type &= ~(TTV_FIND_ACCESS|TTV_FIND_HOLD)):
                                 (type &= ~(TTV_FIND_ACCESS|TTV_FIND_SETUP)),
                                 delay,slope,
                                 newdelay,newslope,(long)0,(long)0,NULL,NULL,0,STB_NO_INDEX,0) ;
          }
      }

  if((type & TTV_FIND_NOTCLASS) != TTV_FIND_NOTCLASS)
    path = ttv_classpath(path,type) ;

  for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT)
    {
      critic = NULL ;
      critic = ttv_alloccritic(critic,ttvfig,pathx->FIG,pathx->ROOT,pathx->TYPE,
                               (long)0,pathx->REFDELAY,pathx->REFSLOPE,pathx->DELAY,pathx->SLOPE,0,NULL,NULL) ;
      critic = ttv_alloccritic(critic,ttvfig,pathx->FIG,pathx->NODE,pathx->TYPE,
                               (long)0,(long)0,(long)0,(long)0,(long)0,0,NULL,NULL) ;
      delaylist = addchain(delaylist,(void *)critic) ;
    }

  ttv_freepathlist(path) ;

  return(reverse(delaylist)) ;
}

long ttv_getconstraintquick(ttvfig_list *ttvfig,ttvfig_list *figpath,ttvevent_list *tve,ttvevent_list *cmd,long type, ttvline_list **rline)
{
  ttvline_list *ptline ;
  long level ;
  chain_list *chain ;
 
  if(figpath == NULL)
    level = 1 ;
  else
    level = figpath->INFO->LEVEL ;

  *rline=NULL;

  if((type & TTV_FIND_ACCESS) == TTV_FIND_ACCESS)
    {
      if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
        ttv_expfigsig(ttvfig,tve->ROOT,level,ttvfig->INFO->LEVEL,
                      TTV_STS_CLS_FED, TTV_FILE_DTX);
      else
        ttv_expfigsig(ttvfig,tve->ROOT,level,ttvfig->INFO->LEVEL,
                      TTV_STS_CL_PJT, TTV_FILE_TTX);
      if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
        ptline = tve->INLINE ;
      else
        ptline = tve->INPATH ;

      for(; ptline != NULL ; ptline = ptline->NEXT)
        {
          if((ptline->TYPE & TTV_LINE_A) == TTV_LINE_A && ptline->NODE==cmd)
            {
              if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                return ttv_getdelaymax(ptline) ;
              else
                return ttv_getdelaymin(ptline) ;
            }
        }
    }

  if((type & (TTV_FIND_SETUP|TTV_FIND_HOLD)) != 0)
    {
      if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
        ttv_expfigsig(ttvfig,cmd->ROOT,level,ttvfig->INFO->LEVEL,
                      TTV_STS_CLS_FED, TTV_FILE_DTX);
      else
        ttv_expfigsig(ttvfig,cmd->ROOT,level,ttvfig->INFO->LEVEL,
                      TTV_STS_CL_PJT, TTV_FILE_TTX);

      if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
        ptline = cmd->INLINE ;
      else
        ptline = cmd->INPATH ;

      for(; ptline != NULL ; ptline = ptline->NEXT)
        {
          if((((ptline->TYPE & TTV_LINE_U) == TTV_LINE_U) &&
              ((type & TTV_FIND_SETUP) != TTV_FIND_SETUP)) ||
             (((ptline->TYPE & TTV_LINE_O) == TTV_LINE_O) &&
              ((type & TTV_FIND_HOLD) != TTV_FIND_HOLD)))
            continue ;

          if(ptline->NODE==tve)
            {
              *rline=ptline;
              if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                return ttv_getdelaymax(ptline) ;
              else
                return ttv_getdelaymin(ptline) ;
            }
        }
    }

  return TTV_NOTIME ;
}

float ttv_getline_vt(ttvfig_list *ttvfig,ttvfig_list *figpath,ttvevent_list *tve,ttvevent_list *cmd,long type)
{
  ttvline_list *ptline ;
  long level ;
  chain_list *chain ;
  char *stm_modelname=NULL;
  float vth = -1.0;
  timing_model *tmg_model;

  if(figpath == NULL)
    level = 1 ;
  else
    level = figpath->INFO->LEVEL ;

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
    ttv_expfigsig(ttvfig,tve->ROOT,level,ttvfig->INFO->LEVEL,
                  TTV_STS_CLS_FED, TTV_FILE_DTX);
  else
    ttv_expfigsig(ttvfig,tve->ROOT,level,ttvfig->INFO->LEVEL,
                  TTV_STS_CL_PJT, TTV_FILE_TTX);

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
    ptline = tve->INLINE ;
  else
    ptline = tve->INPATH ;

  for(; ptline != NULL ; ptline = ptline->NEXT)
    {
      if((ptline->TYPE & TTV_LINE_A) == TTV_LINE_A && ptline->NODE==cmd)
        {
          if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            stm_modelname=ptline->MDMAX;
          else
            stm_modelname=ptline->MDMIN;
          break;
        }
    }
 
  if (stm_modelname!=NULL && (tmg_model = stm_getmodel (ptline->FIG->INSNAME,stm_modelname))) {
    vth = stm_mod_vt (tmg_model);
  }

  return vth;
}

/*****************************************************************************/
/*                        function ttv_freedelaylist()                       */
/* parametres :                                                              */
/* pthead : liste des delais                                                 */
/*                                                                           */
/* libere les delais                                                         */
/*****************************************************************************/
int ttv_freedelaylist(pthead)
     chain_list *pthead ;
{
  return(ttv_freecriticpara(pthead)) ;
}

/*****************************************************************************/
/*                        function ttv_getnodeslew()                         */
/*****************************************************************************/
long ttv_getnodeslew(node, type)
     ttvevent_list * node;
     long type;
{
  ttvdelay_list *delay;
  long slew = TTV_NOSLOPE;
  ptype_list *pt;
    
  if((delay = ttv_getnodedelay(node)) != NULL){
    if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
      slew = delay->FMAX;
    else
      slew = delay->FMIN;
  }
  else
    {
      if ((pt=getptype(node->USER, TTV_NODE_UTD_INPUT_SLOPE))!=NULL)
        slew = (long)pt->DATA;      
      else 
        slew = node->ROOT->ROOT->INFO->SLOPE ;
    }
  return slew;
}

/*****************************************************************************/
/*                        function ttv_allocpathmodel()                      */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* inchain : liste des entrees                                               */
/* goal : noeud sortie                                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* construit les chemins a partir du parcours en largeur du graphe           */
/*****************************************************************************/
//#define CAPASUPMODE
static float ttv_getnodecapa(lofig_list *lofig, ttvevent_list *tve, long type)
{
  float capa;
  char typemax ;
  char typemin ;
  double rmin, c1min, c2min, rmax, c1max, c2max ;
  locon_list *locon=NULL ;
  rcx_list *ptrcx ;
  rcx_slope     slope;
  long          search;
  if(lofig == NULL)
    {
      c1max = ttv_get_signal_capa(tve->ROOT) / 1000.0 ;
      c2max = 0.0 ;
      rmax = -1.0;
      c1min = ttv_get_signal_capa(tve->ROOT) / 1000.0 ;
      c2min = 0.0 ;
      rmin = -1.0;
      typemax = RCX_CAPALOAD ;
      typemin = RCX_CAPALOAD ;
    }
  else
    {
      locon = rcx_gethtrcxcon(NULL,lofig,tve->ROOT->NAME) ;
    
      if((locon == NULL) || (locon->PNODE == NULL) || 
         ((ptrcx = getrcx(locon->SIG)) == NULL))
        {
          c1max = ttv_get_signal_capa(tve->ROOT) / 1000.0 ;
          c2max = 0.0 ;
          rmax = -1.0 ;
          c1min = ttv_get_signal_capa(tve->ROOT) / 1000.0 ;
          c2min = 0.0 ;
          rmin = -1.0;
          typemax = RCX_CAPALOAD ;
          typemin = RCX_CAPALOAD ;
        }
      else
        {
          if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
            slope.F0MAX  = ttv_getslopenode( tve->ROOT->ROOT,
                                             tve->ROOT->ROOT->INFO->LEVEL,
                                             tve,
                                             search        | 
                                             TTV_FIND_GATE | 
                                             TTV_FIND_RC   | 
                                             TTV_FIND_MAX,
                                             TTV_MODE_LINE
                                             )/TTV_UNIT;
            slope.FCCMAX = ttv_getslopenode( tve->ROOT->ROOT,
                                             tve->ROOT->ROOT->INFO->LEVEL,
                                             tve,
                                             search        | 
                                             TTV_FIND_GATE | 
                                             TTV_FIND_RC   | 
                                             TTV_FIND_MAX,
                                             TTV_MODE_DELAY
                                             )/TTV_UNIT;
          }
          else {
            slope.F0MAX  = -1.0 ;
            slope.FCCMAX = -1.0 ;
          }
            
          slope.SENS   = ((tve->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
            ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
          slope.CCA   = -1.0 ;
    
          if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
            typemax = rcx_rcnload( lofig, 
                                   locon->SIG, 
                                   locon->PNODE,
                                   &rmax, &c1max, &c2max, 
                                   RCX_BESTLOAD, 
                                   0.0,
                                   &slope, 
                                   RCX_MAX 
                                   ) ;
          } else {
            typemin = rcx_rcnload( lofig, 
                                   locon->SIG, 
                                   locon->PNODE,
                                   &rmin, &c1min, &c2min, 
                                   RCX_BESTLOAD, 
                                   0.0,
                                   &slope, 
                                   RCX_MIN 
                                   );
          }
        }
    }
  if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
    capa = ((typemax==RCX_CAPALOAD) ? c1max*1000.0 : (c1max+c2max)*1000.0);
  }else{
    capa = ((typemin==RCX_CAPALOAD) ? c1min*1000.0 : (c1min+c2min)*1000.0);
  }
        

  return capa;
}

static void ttv_getaxisvalues(ht *htslope, ht *htcapa, char *namein, char *nameout, chain_list **slope_axis, chain_list **capa_axis)
{
  int i;
  stm_carac     *caracslope = NULL ;
  stm_carac     *caraccapa = NULL ;

  *slope_axis=*capa_axis=NULL ;
  if(namein && htslope)
    {
      caracslope = (stm_carac*)gethtitem (htslope, namein) ;
      if ((long)caracslope == EMPTYHT)
        caracslope = (stm_carac*)gethtitem (htslope, namealloc ("default")) ;
      if ((long)caracslope == EMPTYHT)
        *slope_axis=addchain(*slope_axis, (void *)TTV_NOTIME) ;
      else
        {
          for (i = 0 ; i < caracslope->NVALUES ; i++)
            *slope_axis=addchain(*slope_axis, (void *)mbk_long_round(caracslope->VALUES[i]*TTV_UNIT));
        }
    }else
      *slope_axis=NULL; //addchain(*slope_axis, (void *)TTV_NOTIME) ;

  if(nameout && htcapa)
    {
      caraccapa = (stm_carac*)gethtitem (htcapa, nameout) ;
      if ((long)caraccapa == EMPTYHT)
        caraccapa = (stm_carac*)gethtitem (htcapa, namealloc ("default")) ;
      if ((long)caraccapa == EMPTYHT)
        *capa_axis=addchain(*capa_axis, NULL), *(float *)&(*capa_axis)->DATA=-1;
      else
        {
          for (i = 0 ; i < caraccapa->NVALUES ; i++)
            *capa_axis=addchain(*capa_axis, NULL), *(float *)&(*capa_axis)->DATA=caraccapa->VALUES[i];
        }
    }else
      *capa_axis=NULL; //addchain(*capa_axis, NULL), *(float *)&(*capa_axis)->DATA=-1;

  *slope_axis=reverse(*slope_axis);
  *capa_axis=reverse(*capa_axis);
}

chain_list *ttv_getconnectoraxis(ttvfig_list *ttvfig, int slope, char *name)
{
  ht *htslope_axis = NULL;
  ht *htcapa_axis = NULL;
  chain_list *res0, *res1;
  ptype_list *ptype;
 
  if ((ptype = getptype(ttvfig->USER, TTV_FIG_SAXIS)) != NULL)
    htslope_axis = (ht*)ptype->DATA; 
  if ((ptype = getptype(ttvfig->USER, TTV_FIG_CAXIS)) != NULL)
    htcapa_axis = (ht*)ptype->DATA; 

  ttv_getaxisvalues(htslope_axis, htcapa_axis, slope?name:NULL, slope?NULL:name, &res0, &res1);
  if (slope) return res0;
  return res1;
}
  
void ttv_allocpathmodel_propagate (ttvfig_list *ttvfig, ttvfig_list *insfig, ttvevent_list *node, ttvevent_list *root, ttvpath_list *path, long type)
{
  chain_list *input_slope, *output_capa, *real_output_capa, *res_slope, *res_delay, *res_energy, *cl, *ch;
  lofig_list *lofig;
  ht *htslope_axis = NULL;
  ht *htcapa_axis = NULL;
  int nbs, nbc, i, j, doref;
  timing_model *delay_model = NULL, *md;
  timing_model *slope_model = NULL;
  ptype_list *ptype;
  ttvline_list *line, *tl;
  long inslope, delta;
  float *ft, sigcapa=0, fl, factor;
 
  if ((ptype = getptype(ttvfig->USER, TTV_FIG_SAXIS)) != NULL)
    htslope_axis = (ht*)ptype->DATA; 
  if ((ptype = getptype(ttvfig->USER, TTV_FIG_CAXIS)) != NULL)
    htcapa_axis = (ht*)ptype->DATA; 

  ttv_getaxisvalues(htslope_axis, htcapa_axis, path->NODE->ROOT->NETNAME, path->ROOT->ROOT->NETNAME, &input_slope, &output_capa);

  ch=ttv_BuildLineList(ttvfig, root, node, type, &doref);
  line=(ttvline_list *)ch->DATA;
  if (input_slope==NULL)
    {
      if ((line->NODE->ROOT->TYPE & TTV_SIG_C)!=0)
        {
          ttv_getnodeslope(ttvfig, insfig, line->NODE, &inslope, type);
          ft = stm_dyna_slews (STM_DEF_AXIS_BASE, inslope);
          nbs = STM_DEF_AXIS_BASE * 2 + 1;
          for (i=0; i<nbs; i++) input_slope=addchain(input_slope, (void *)mbk_long_round(ft[i]));
          mbkfree(ft);
          input_slope=reverse(input_slope);
        }
      else input_slope=addchain(input_slope, (void *)TTV_NOTIME) ;
    }  
  else if ((line->NODE->ROOT->TYPE & TTV_SIG_C)==0)
    {
      freechain(input_slope);
      input_slope=addchain(NULL, (void *)TTV_NOTIME) ;
    }

  line=NULL;
  for (cl=ch; cl!=NULL; cl=cl->NEXT)
    if (((tl=(ttvline_list *)cl->DATA)->TYPE & TTV_LINE_RC)==0) line=tl;

  if ((tl->ROOT->ROOT->TYPE & TTV_SIG_C)!=0)
    {
      lofig = ttv_getrcxlofig (ttvfig);
      sigcapa=ttv_get_signal_capa(tl->ROOT->ROOT); //ttv_getnodecapa(lofig,tl->ROOT,type);
    }
  if (output_capa==NULL)
    {
      if ((tl->ROOT->ROOT->TYPE & TTV_SIG_C)!=0)
        {
          if (line!=NULL) md=stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMAX); else md=NULL;
          ft = stm_dyna_loads (STM_DEF_AXIS_BASE, stm_mod_vf(md)/stm_mod_imax(md), sigcapa);
          nbc = STM_DEF_AXIS_BASE * 2 + 1;
          for (i=0; i<nbc; i++) output_capa=addchain(output_capa, NULL), *(float *)&output_capa->DATA=ft[i];
          mbkfree(ft);
          output_capa=reverse(output_capa);
        }
      else output_capa=addchain(output_capa, NULL), *(float *)&output_capa->DATA=-1;
    }
  else if ((tl->ROOT->ROOT->TYPE & TTV_SIG_C)==0)
    {
      freechain(output_capa);
      output_capa=addchain(NULL, NULL), *(float *)&output_capa->DATA=-1;
    }


  real_output_capa=dupchainlst(output_capa);
  if (V_BOOL_TAB[__TMA_DRIVECAPAOUT].VALUE)
    {
      for (cl=real_output_capa; cl!=NULL; cl=cl->NEXT)
        {
          fl=*(float *)&cl->DATA;
          if (fl!=-1)
            {
              fl=fl-sigcapa;
              if (fl<0)
                {
                  avt_errmsg( TTV_ERRMSG, "059", AVT_WARNING, *(float *)&cl->DATA, tl->ROOT->ROOT->NAME, sigcapa);
                  fl=0;
                }
              *(float *)&cl->DATA=fl;
            }
        }
    }
  //  ttv_experim_setstopaftergate1(1);
  ttv_DoCharacPropagation(ttvfig, insfig, node, root, type, input_slope, real_output_capa, &res_delay, &res_slope, &res_energy, ch);
  //  ttv_experim_setstopaftergate1(0);

  freechain(real_output_capa);

  nbs=countchain(input_slope);
  nbc=countchain(output_capa);

  if (nbs==1)
    {
      delay_model = stm_mod_create_table (NULL, nbc, 0, STM_LOAD, STM_NOTYPE);
      slope_model = stm_mod_create_table (NULL, nbc, 0, STM_LOAD, STM_NOTYPE);
      for (cl=output_capa, i=0; i<nbc; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(delay_model->UMODEL.TABLE, i, *(float *)&cl->DATA);
          stm_modtbl_setXrangeval(slope_model->UMODEL.TABLE, i, *(float *)&cl->DATA);
        }
      for (cl=res_delay, i=0; cl!=NULL; cl=cl->NEXT, i++) stm_modtbl_set1Dval (delay_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
      for (cl=res_slope, i=0; cl!=NULL; cl=cl->NEXT, i++) stm_modtbl_set1Dval (slope_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
    }
  else if (nbc==1)
    {
      delay_model = stm_mod_create_table (NULL, nbs, 0, STM_INPUT_SLEW, STM_NOTYPE);
      slope_model = stm_mod_create_table (NULL, nbs, 0, STM_INPUT_SLEW, STM_NOTYPE);
      for (cl=input_slope, i=0; i<nbs; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(delay_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
          stm_modtbl_setXrangeval(slope_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
        }
      for (cl=res_delay, i=0; cl!=NULL; cl=cl->NEXT, i++) stm_modtbl_set1Dval (delay_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
      for (cl=res_slope, i=0; cl!=NULL; cl=cl->NEXT, i++) stm_modtbl_set1Dval (slope_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
    }
  else
    {
      delay_model = stm_mod_create_table (NULL, nbs, nbc, STM_INPUT_SLEW, STM_LOAD);
      slope_model = stm_mod_create_table (NULL, nbs, nbc, STM_INPUT_SLEW, STM_LOAD);
      for (cl=input_slope, i=0; i<nbs; i++, cl=cl->NEXT)
        {
          stm_modtbl_setXrangeval(delay_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
          stm_modtbl_setXrangeval(slope_model->UMODEL.TABLE, i, ((long)cl->DATA)/TTV_UNIT);
        }
      for (cl=output_capa, i=0; i<nbc; i++, cl=cl->NEXT)
        {
          stm_modtbl_setYrangeval(delay_model->UMODEL.TABLE, i, *(float *)&cl->DATA);
          stm_modtbl_setYrangeval(slope_model->UMODEL.TABLE, i, *(float *)&cl->DATA);
        }
      for (cl=res_delay, ch=res_slope, i=0; i<nbs; i++)
        for (j=0; j<nbc; j++, cl=cl->NEXT, ch=ch->NEXT)
          {
            stm_modtbl_set2Dval(delay_model->UMODEL.TABLE, i, j, ((long)cl->DATA)/TTV_UNIT);
            stm_modtbl_set2Dval(slope_model->UMODEL.TABLE, i, j, ((long)ch->DATA)/TTV_UNIT);
          }
    }

  if (ttv_get_path_margins_info(ttvfig, path, &factor, &delta))
    stm_modtbl_scale_and_add_val(delay_model->UMODEL.TABLE, delta/TTV_UNIT, factor);
  path->MF=slope_model;
  path->MD=delay_model;

  freechain(input_slope);
  freechain(output_capa);
  freechain(res_delay);
  freechain(res_slope);
  freechain(res_energy);

}



void ttv_allocpathmodel (ttvfig, path, type)
     ttvfig_list *ttvfig;
     ttvpath_list *path;
     long type;
{
  ttvevent_list *start;
  ttvevent_list *end;
  ttvline_list *start_line;
  ttvline_list *start_line_rc;
  ttvline_list *end_line;
  ttvline_list *end_line_rc;
  ttvline_list *line;
  ttvline_list *ptlineforslew = NULL;
  timing_model *mindelay = NULL;
  timing_model *moutdelay = NULL;
  timing_model *moutslope = NULL;
  timing_model *minslope = NULL;
  timing_model *modelindelay;
  timing_model *modelinslope;
  timing_model *modeloutdelay;
  timing_model *modeloutslope;
  long delay;
  int i, /*j,*/ ns, nl, ns_slew;
  float inslew_min, inslew_max, slew, load, capa, nodeslew, factor;
  long dmin, dmax, fmin, fmax;
  int singlerc = 0;
  lofig_list *lofig;
  char ttype;
  char typemax ;
  char typemin ;
  double rmin, c1min, c2min, rmax, c1max, c2max ;
  locon_list *locon=NULL ;
  rcx_list *ptrcx ;
  rcx_slope     slope;
  long          search, delta;
  ht *htslope_axis = NULL;
  ht *htcapa_axis = NULL;
  ptype_list *ptype;
    

  if ((ptype = getptype(ttvfig->USER, TTV_FIG_SAXIS)) != NULL)
    htslope_axis = (ht*)ptype->DATA; 
  if ((ptype = getptype(ttvfig->USER, TTV_FIG_CAXIS)) != NULL)
    htcapa_axis = (ht*)ptype->DATA; 


  lofig = ttv_getrcxlofig (ttvfig);

  start = path->NODE;
  end = path->ROOT;

  nodeslew = (float)ttv_getnodeslew(start, type);

  /* start_line_rc, start_line, end_line, end_line_rc, delay */

  if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) {
    line = end->FIND->OUTLINE;

    if ((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) && ((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) && ((line->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C)) {
      if (line->NODE->FIND->OUTLINE) {
        end_line_rc = line;
        line = line->NODE->FIND->OUTLINE;
      }
      else {
        end_line_rc = line;
        line = NULL;
      }
    }
    else
      end_line_rc = NULL;

    start_line_rc = NULL;
    start_line = NULL;
    end_line = line;
    if(end_line)
      ptlineforslew = end_line->NODE->FIND->OUTLINE;
  }
  else {
    line = start->FIND->OUTLINE;

    if ((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) && ((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) && ((line->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C)) {
      if (line->ROOT->FIND->OUTLINE) {
        start_line_rc = line;
        line = line->ROOT->FIND->OUTLINE;
      }
      else {
        start_line_rc = line;
        line = NULL;
      }
    }
    else
      start_line_rc = NULL;

    end_line_rc = NULL;
    end_line = NULL;
    start_line = line;
  }

  delay = 0;

  while (line) {
    if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) {
      if (line->NODE == start) {
        if (line->TYPE & (TTV_LINE_D | TTV_LINE_T)) start_line = line;
        break;
      }

      if ((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) && line->NODE->FIND->OUTLINE) {
        if ((line->NODE->FIND->OUTLINE->NODE == start) && ((line->NODE->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)) {
          start_line_rc = line->NODE->FIND->OUTLINE;
          start_line = line;
          break;
        }
      }
      if ((line->ROOT != end) || ((line->ROOT == end) && ((line->ROOT->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C))) {
        if(!end_line_rc || (end_line_rc && (end_line_rc->NODE->FIND->OUTLINE != line))){
          if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
            delay += ttv_getdelaymax (line);
            inslew_max = ttv_getslopemax (line);
          }
          else {
            delay += ttv_getdelaymin (line);
            inslew_min = ttv_getslopemin (line);
          }
        }
      }
    }
    else {
      if (line->ROOT == end) {
        if (line->TYPE & (TTV_LINE_D | TTV_LINE_T)) end_line = line;
        if ((line->ROOT->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C){
          if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
            delay += ttv_getdelaymax (line);
            inslew_max = ttv_getslopemax (line);
          }
          else {
            delay += ttv_getdelaymin (line);
            inslew_min = ttv_getslopemin (line);
          }
        }
        break;
      }

      if ((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) && line->ROOT->FIND->OUTLINE) {
        if ((line->ROOT->FIND->OUTLINE->ROOT == end) && ((line->ROOT->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC)) {
          end_line_rc = line->ROOT->FIND->OUTLINE;
          end_line = line;
          if ((end_line_rc->ROOT->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C){
            if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
              delay += ttv_getdelaymax (line);
              inslew_max = ttv_getslopemax (line);
            }
            else {
              delay += ttv_getdelaymin (line);
              inslew_min = ttv_getslopemin (line);
            }
          }
          break;
        }
      }
      /*            if (!(line->TYPE & (TTV_LINE_D | TTV_LINE_T)))*/
      ptlineforslew = line;
      if ((line->NODE != start) || ((line->NODE == start) && ((line->NODE->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C))) {
        if(!start_line_rc || (start_line_rc && (start_line_rc->ROOT->FIND->OUTLINE != line))){
          if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
            delay += ttv_getdelaymax (line);
            inslew_max = ttv_getslopemax (line);
          }
          else {
            delay += ttv_getdelaymin (line);
            inslew_min = ttv_getslopemin (line);
          }
        }
      }
    }

    /*        if (!(line->TYPE & (TTV_LINE_D | TTV_LINE_T))) {
              if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
              delay += ttv_getdelaymax (line);
              inslew_max = ttv_getslopemax (line);
              }
              else {
              delay += ttv_getdelaymin (line);
              inslew_min = ttv_getslopemin (line);
              }
              }
    */
    if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL){
      line = line->NODE->FIND->OUTLINE;
    }else {
      line = line->ROOT->FIND->OUTLINE;
    }
  }

  if (line && !(line->TYPE & (TTV_LINE_D | TTV_LINE_T))) {
    /*        if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
              delay += ttv_getdelaymax (line);
              inslew_max = ttv_getslopemax (line);
              }
              else {
              delay += ttv_getdelaymin (line);
              inslew_min = ttv_getslopemin (line);
              }*/

    if ((type & TTV_FIND_DUAL) == TTV_FIND_DUAL) start_line = line;
    else end_line = line;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // No models to create

  singlerc = (start_line_rc || end_line_rc) && (start_line_rc == end_line_rc);

  if (!singlerc) {
    if (!start_line && !end_line) {
      path->MD = NULL;
      path->MF = NULL;
      return;
    }
    if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
      if (!start_line && !end_line->MDMAX && !end_line->MFMAX) {
        path->MD = NULL;
        path->MF = NULL;
        return;
      }
      if (!end_line && !start_line->MDMAX && !start_line->MFMAX) {
        path->MD = NULL;
        path->MF = NULL;
        return;
      }
      if (!start_line->MDMAX && !end_line->MDMAX && !start_line->MFMAX && !end_line->MFMAX) {
        path->MD = NULL;
        path->MF = NULL;
        return;
      }
    }
    else {
      if (!start_line && !end_line->MDMIN && !end_line->MFMIN) {
        path->MD = NULL;
        path->MF = NULL;
        return;
      }
      if (!end_line && !start_line->MDMIN && !start_line->MFMIN) {
        path->MD = NULL;
        path->MF = NULL;
        return;
      }
      if (!start_line->MDMIN && !end_line->MDMIN && !start_line->MFMIN && !end_line->MFMIN) {
        path->MD = NULL;
        path->MF = NULL;
        return;
      }
    }
  }
  if (!start_line) {
    capa = STM_DONTCARE;
  }
  else {
    //        capa = start_line->ROOT->ROOT->CAPA;
        
    if(lofig == NULL)
      {
        c1max = ttv_get_signal_capa(start_line->ROOT->ROOT) / 1000.0 ;
        c2max = 0.0 ;
        rmax = -1.0;
        c1min = ttv_get_signal_capa(start_line->ROOT->ROOT) / 1000.0 ;
        c2min = 0.0 ;
        rmin = -1.0;
        typemax = RCX_CAPALOAD ;
        typemin = RCX_CAPALOAD ;
      }
    else
      {
        locon = rcx_gethtrcxcon(NULL,lofig,start_line->ROOT->ROOT->NAME) ;
    
        if((locon == NULL) || (locon->PNODE == NULL) || 
           ((ptrcx = getrcx(locon->SIG)) == NULL))
          {
            c1max = ttv_get_signal_capa(start_line->ROOT->ROOT) / 1000.0 ;
            c2max = 0.0 ;
            rmax = -1.0 ;
            c1min = ttv_get_signal_capa(start_line->ROOT->ROOT) / 1000.0 ;
            c2min = 0.0 ;
            rmin = -1.0;
            typemax = RCX_CAPALOAD ;
            typemin = RCX_CAPALOAD ;
          }
        else
          {
            if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
              slope.F0MAX  = ttv_getslopenode( start_line->ROOT->ROOT->ROOT,
                                               start_line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                               start_line->ROOT,
                                               search        | 
                                               TTV_FIND_GATE | 
                                               TTV_FIND_RC   | 
                                               TTV_FIND_MAX,
                                               TTV_MODE_LINE
                                               )/TTV_UNIT;
              slope.FCCMAX = ttv_getslopenode( start_line->ROOT->ROOT->ROOT,
                                               start_line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                               start_line->ROOT,
                                               search        | 
                                               TTV_FIND_GATE | 
                                               TTV_FIND_RC   | 
                                               TTV_FIND_MAX,
                                               TTV_MODE_DELAY
                                               )/TTV_UNIT;
            }
            else {
              slope.F0MAX  = -1.0 ;
              slope.FCCMAX = -1.0 ;
            }
            
            slope.SENS   = ((start_line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
              ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
            slope.CCA   = -1.0 ;
    
            if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
              typemax = rcx_rcnload( lofig, 
                                     locon->SIG, 
                                     locon->PNODE,
                                     &rmax, &c1max, &c2max, 
                                     RCX_BESTLOAD, 
                                     0.0,
                                     &slope, 
                                     RCX_MAX 
                                     ) ;
            } else {
              typemin = rcx_rcnload( lofig, 
                                     locon->SIG, 
                                     locon->PNODE,
                                     &rmin, &c1min, &c2min, 
                                     RCX_BESTLOAD, 
                                     0.0,
                                     &slope, 
                                     RCX_MIN 
                                     );
            }
          }
      }
    if((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
      capa = ((typemax==RCX_CAPALOAD) ? c1max*1000.0 : (c1max+c2max)*1000.0);
    }else{
      capa = ((typemin==RCX_CAPALOAD) ? c1min*1000.0 : (c1min+c2min)*1000.0);
    }
        
  }

  //----------------------------------------------------------------------------
  // models to create
  if((((path->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((path->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))
     &&(((path->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((path->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))){
    stm_addaxisvalues(htslope_axis, htcapa_axis, path->NODE->ROOT->NETNAME, path->ROOT->ROOT->NETNAME);
    if (!start_line) {
      modelindelay = NULL;
      modelinslope = NULL;
    }
    else {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
        modelindelay = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MDMAX);
        if (STM_CACHE && modelindelay) 
          mbk_cache_lock (STM_CACHE, modelindelay);
        modelinslope = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MFMAX);
        if (STM_CACHE && modelinslope) 
          mbk_cache_lock (STM_CACHE, modelinslope);
      } else {
        modelindelay = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MDMIN);
        if (STM_CACHE && modelindelay) 
          mbk_cache_lock (STM_CACHE, modelindelay);
        modelinslope = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MFMIN);
        if (STM_CACHE && modelinslope) 
          mbk_cache_lock (STM_CACHE, modelinslope);
      }
    }
    
    if (!end_line) {
      modeloutdelay = NULL;
      modeloutslope = NULL;
    }
    else {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
        modeloutdelay = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MDMAX);
        if (STM_CACHE && modeloutdelay) 
          mbk_cache_lock (STM_CACHE, modeloutdelay);
        modeloutslope = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MFMAX);
        if (STM_CACHE && modeloutslope) 
          mbk_cache_lock (STM_CACHE, modeloutslope);
      } else {
        modeloutdelay = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MDMIN);
        if (STM_CACHE && modeloutdelay) 
          mbk_cache_lock (STM_CACHE, modeloutdelay);
        modeloutslope = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MFMIN);
        if (STM_CACHE && modeloutslope) 
          mbk_cache_lock (STM_CACHE, modeloutslope);
      }
    }
  }else if(((path->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((path->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N)){
    stm_addaxisvalues(htslope_axis, htcapa_axis, path->NODE->ROOT->NETNAME, NULL);
    if (!start_line) {
      modelindelay = NULL;
      modelinslope = NULL;
    }
    else {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
        modelindelay = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MDMAX);
        if (STM_CACHE && modelindelay) 
          mbk_cache_lock (STM_CACHE, modelindelay);
        modelinslope = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MFMAX);
        if (STM_CACHE && modelinslope) 
          mbk_cache_lock (STM_CACHE, modelinslope);
      } else {
        modelindelay = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MDMIN);
        if (STM_CACHE && modelindelay) 
          mbk_cache_lock (STM_CACHE, modelindelay);
        modelinslope = stm_getmodel (ttvfig->INFO->FIGNAME, start_line->MFMIN);
        if (STM_CACHE && modelinslope) 
          mbk_cache_lock (STM_CACHE, modelinslope);
      }
    }
    if(start_line != end_line){
      modeloutdelay = NULL;
      modeloutslope = NULL;
    }else{
      modeloutdelay = modelindelay;
      modeloutslope = modelinslope;
    }
  }else if(((path->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((path->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N)){
    stm_addaxisvalues(htslope_axis, htcapa_axis, NULL, path->ROOT->ROOT->NETNAME);
    if (!end_line) {
      modeloutdelay = NULL;
      modeloutslope = NULL;
    }
    else {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
        modeloutdelay = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MDMAX);
        if (STM_CACHE && modeloutdelay) 
          mbk_cache_lock (STM_CACHE, modeloutdelay);
        modeloutslope = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MFMAX);
        if (STM_CACHE && modeloutslope) 
          mbk_cache_lock (STM_CACHE, modeloutslope);
      } else {
        modeloutdelay = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MDMIN);
        if (STM_CACHE && modeloutdelay) 
          mbk_cache_lock (STM_CACHE, modeloutdelay);
        modeloutslope = stm_getmodel (ttvfig->INFO->FIGNAME, end_line->MFMIN);
        if (STM_CACHE && modeloutslope) 
          mbk_cache_lock (STM_CACHE, modeloutslope);
      }
    }
    if(start_line != end_line){
      modelindelay = NULL;
      modelinslope = NULL;
    }else{
      modelindelay = modeloutdelay;
      modelinslope = modeloutslope;
    }
  }else{
    modeloutdelay = NULL;
    modeloutslope = NULL;
    modelindelay = NULL;
    modelinslope = NULL;
  }        
    
  // creation du modele rc + gate  start ------------------------------------------------------------------------
  if (start_line_rc && start_line && modelindelay && modelinslope && (start_line != end_line)) {

    // delay model
    mindelay = stm_mod_reduce(NULL, modelindelay, nodeslew/TTV_UNIT, capa, STM_LOADFIX_RED, STM_DELAY);
    ns = stm_modtbl_getnslew (mindelay->UMODEL.TABLE);
    if (!ns) {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
        ttv_calcgatercdelayslope (start_line_rc, start_line, nodeslew, nodeslew, 
                                  capa, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
        stm_modtbl_setconst (mindelay->UMODEL.TABLE, dmax/TTV_UNIT);
      }else{
        ttv_calcgatercdelayslope (start_line_rc, start_line, nodeslew, nodeslew, 
                                  capa, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
        stm_modtbl_setconst (mindelay->UMODEL.TABLE, dmin/TTV_UNIT);
      }
    }
    else
      for (i = 0; i < ns; i++) {
        slew = stm_modtbl_getslewaxisval (mindelay->UMODEL.TABLE, i);

        if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
          ttv_calcgatercdelayslope (start_line_rc, start_line, slew*TTV_UNIT, slew*TTV_UNIT, capa, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
          stm_modtbl_set1Dval (mindelay->UMODEL.TABLE, i, dmax/TTV_UNIT);
        }else{
          ttv_calcgatercdelayslope (start_line_rc, start_line, slew*TTV_UNIT, slew*TTV_UNIT, capa, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
          stm_modtbl_set1Dval (mindelay->UMODEL.TABLE, i, dmin/TTV_UNIT);
        }
      }

    // slope model
    minslope = stm_mod_reduce(NULL, modelinslope, nodeslew/TTV_UNIT, capa, STM_LOADFIX_RED, STM_SLEW);
    ns = stm_modtbl_getnslew (minslope->UMODEL.TABLE);
    if (!ns) {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
        ttv_calcgatercdelayslope (start_line_rc, start_line, nodeslew, nodeslew, 
                                  capa, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
        stm_modtbl_setconst (minslope->UMODEL.TABLE, fmax/TTV_UNIT);
      }else{
        ttv_calcgatercdelayslope (start_line_rc, start_line, nodeslew, nodeslew, 
                                  capa, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
        stm_modtbl_setconst (minslope->UMODEL.TABLE, fmin/TTV_UNIT);
      }
    }
    for (i = 0; i < ns; i++) {
      slew = stm_modtbl_getslewaxisval (minslope->UMODEL.TABLE, i);
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
        ttv_calcgatercdelayslope (start_line_rc, start_line, slew*TTV_UNIT, slew*TTV_UNIT, capa, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
        stm_modtbl_set1Dval (minslope->UMODEL.TABLE, i, fmax/TTV_UNIT);
      }else{
        ttv_calcgatercdelayslope (start_line_rc, start_line, slew*TTV_UNIT, slew*TTV_UNIT, capa, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
        stm_modtbl_set1Dval (minslope->UMODEL.TABLE, i, fmin/TTV_UNIT);
      }
    }
    if (STM_CACHE)  {
      mbk_cache_unlock (STM_CACHE, modelinslope);
      mbk_cache_unlock (STM_CACHE, modelindelay);
    }
    modelinslope = minslope;
    modelindelay = mindelay;
  }

  // creation du modele rc + gate  end ------------------------------------------------------------------------
  if (end_line_rc && end_line && modeloutdelay && modeloutslope && (start_line != end_line)) {
    if (ptlineforslew) {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
        slew = ttv_getslopemax (ptlineforslew);
      else
        slew = ttv_getslopemin (ptlineforslew);
    }
    else slew = STM_DEF_SLEW * TTV_UNIT ;
        
    load = ttv_get_signal_capa(end_line->ROOT->ROOT);
    moutdelay = stm_mod_reduce(NULL, modeloutdelay, slew/TTV_UNIT, load, STM_SLEWFIX_RED, STM_DELAY);
    moutslope = stm_mod_reduce(NULL, modeloutslope, slew/TTV_UNIT, load, STM_SLEWFIX_RED, STM_SLEW);
    nl = stm_modtbl_getnload (moutdelay->UMODEL.TABLE);
    /*        for (i = 0; i < nl; i++) {
              load = stm_modtbl_getloadaxisval (moutdelay->UMODEL.TABLE, i);

              if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
              ttv_calcgatercdelayslope (end_line, end_line_rc, slew, slew, load, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
              stm_modtbl_set1Dval (moutdelay->UMODEL.TABLE, i, dmax/TTV_UNIT);
              stm_modtbl_set1Dval (moutslope->UMODEL.TABLE, i, fmax/TTV_UNIT);
              }
              else {
              ttv_calcgatercdelayslope (end_line, end_line_rc, slew, slew, load, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
              stm_modtbl_set1Dval (moutdelay->UMODEL.TABLE, i, dmin/TTV_UNIT);
              stm_modtbl_set1Dval (moutslope->UMODEL.TABLE, i, fmin/TTV_UNIT);
              }
              }*/
    if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
      stm_modtbl_scale_and_add_val (moutdelay->UMODEL.TABLE, (float)end_line_rc->VALMAX/TTV_UNIT, 1);
    }else{
      stm_modtbl_scale_and_add_val (moutdelay->UMODEL.TABLE, (float)end_line_rc->VALMIN/TTV_UNIT, 1);
    }
    if (STM_CACHE)  {
      mbk_cache_unlock (STM_CACHE, modeloutslope);
      mbk_cache_unlock (STM_CACHE, modeloutdelay);
    }
    modeloutslope = moutslope;
    modeloutdelay = moutdelay;
  }

  if (end_line_rc && end_line && modelindelay && modelinslope && (start_line == end_line)) {
    mindelay = stm_mod_reduce(NULL, modelindelay, nodeslew/TTV_UNIT, capa, STM_COPY, STM_DELAY);
    minslope = stm_mod_reduce(NULL, modelinslope, nodeslew/TTV_UNIT, capa, STM_COPY, STM_SLEW);
    ns = stm_modtbl_getnslew (mindelay->UMODEL.TABLE);
    nl = stm_modtbl_getnload (mindelay->UMODEL.TABLE);
    ns_slew = stm_modtbl_getnslew (minslope->UMODEL.TABLE);
    /*        for (j = 0; j < nl; j++) {
              for (i = 0; i < ns; i++) {
              slew = stm_modtbl_getslewaxisval (mindelay->UMODEL.TABLE, i);
              load = stm_modtbl_getloadaxisval (mindelay->UMODEL.TABLE, j);
              if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
              ttv_calcgatercdelayslope (end_line, end_line_rc, slew*TTV_UNIT, slew*TTV_UNIT, load, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
              stm_modtbl_set2Dval (mindelay->UMODEL.TABLE, i, j, dmax/TTV_UNIT);
              if(ns_slew)
              stm_modtbl_set2Dval (minslope->UMODEL.TABLE, i, j, fmax/TTV_UNIT);
              else
              stm_modtbl_set1Dval (minslope->UMODEL.TABLE, j, fmax/TTV_UNIT);
              }else{
              ttv_calcgatercdelayslope (end_line, end_line_rc, slew*TTV_UNIT, slew*TTV_UNIT, load, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
              stm_modtbl_set2Dval (mindelay->UMODEL.TABLE, i, j, dmin/TTV_UNIT);
              if(ns_slew)
              stm_modtbl_set2Dval (minslope->UMODEL.TABLE, i, j, fmin/TTV_UNIT);
              else
              stm_modtbl_set1Dval (minslope->UMODEL.TABLE, j, fmin/TTV_UNIT);
              }
              }
              }*/
    if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
      stm_modtbl_scale_and_add_val (mindelay->UMODEL.TABLE, (float)end_line_rc->VALMAX/TTV_UNIT, 1);
    }else{
      stm_modtbl_scale_and_add_val (mindelay->UMODEL.TABLE, (float)end_line_rc->VALMIN/TTV_UNIT, 1);
    }
    if(start_line_rc){
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX){
        stm_modtbl_scale_and_add_val (mindelay->UMODEL.TABLE, (float)start_line_rc->VALMAX/TTV_UNIT, 1);
      }else{
        stm_modtbl_scale_and_add_val (mindelay->UMODEL.TABLE, (float)start_line_rc->VALMIN/TTV_UNIT, 1);
      }
    }
    if (STM_CACHE)  {
      mbk_cache_unlock (STM_CACHE, modelinslope);
      mbk_cache_unlock (STM_CACHE, modelindelay);
    }
    modelinslope = minslope;
    modelindelay = mindelay;
  }
  // creation du modele rc + gate  singlerc ------------------------------------------------------------------------
  if (singlerc) {
    load = ttv_get_signal_capa(start_line_rc->NODE->ROOT);
    mindelay = stm_mod_create_table (NULL, 3, 0, STM_INPUT_SLEW, STM_NOTYPE);
    minslope = stm_mod_create_table (NULL, 3, 0, STM_INPUT_SLEW, STM_NOTYPE);

    for (i = 0; i < 3; i++) {
      switch (i) {
      case 0: slew = nodeslew / 2; break;
      case 1: slew = nodeslew; break;
      case 2: slew = nodeslew * 2; break;
      }

      mindelay->UMODEL.TABLE->XRANGE[i] = slew/TTV_UNIT;
      minslope->UMODEL.TABLE->XRANGE[i] = slew/TTV_UNIT;
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX) {
        ttv_calcgatercdelayslope (end_line_rc, NULL, slew, slew, load, NULL, NULL, &dmax, &fmax, TTV_MODE_NOTSTORE);
        stm_modtbl_set1Dval (mindelay->UMODEL.TABLE, i, dmax/TTV_UNIT);
        stm_modtbl_set1Dval (minslope->UMODEL.TABLE, i, fmax/TTV_UNIT);
      }
      else {
        ttv_calcgatercdelayslope (end_line_rc, NULL, slew, slew, load, &dmin, &fmin, NULL, NULL, TTV_MODE_NOTSTORE);
        stm_modtbl_set1Dval (mindelay->UMODEL.TABLE, i, dmin/TTV_UNIT);
        stm_modtbl_set1Dval (minslope->UMODEL.TABLE, i, fmin/TTV_UNIT);
      }
    }

    modelinslope = minslope;
    modelindelay = mindelay;
  }

  // models assignation ---------------------------------------------------------
  if (start_line != end_line) {
    if (ptlineforslew) {
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
        slew = ttv_getslopemax (ptlineforslew);
      else
        slew = ttv_getslopemin (ptlineforslew);
    }
    else slew = STM_DEF_SLEW * TTV_UNIT ;
        
    path->MD = stm_mod_merge (NULL, modelindelay, modeloutdelay, (float)delay/TTV_UNIT, (float)capa, (float)slew/TTV_UNIT);
    if(modeloutslope){
      path->MF = stm_mod_reduce (NULL, modeloutslope, (float)slew/TTV_UNIT, (float)capa, STM_SLEWFIX_RED, STM_SLEW);
    }else{
      if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
        path->MF = stm_mod_create_fcst (NULL, ttv_getslopemax (end_line)/TTV_UNIT, STM_MOD_MODTBL);
      else
        path->MF = stm_mod_create_fcst (NULL, ttv_getslopemin (end_line)/TTV_UNIT, STM_MOD_MODTBL);
    }
  }
  else {
    if((((start_line->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((start_line->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))
       &&(((start_line->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((start_line->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))){
      path->MD = stm_mod_reduce(NULL, modelindelay, nodeslew/TTV_UNIT, capa, STM_COPY, STM_DELAY);
      path->MF = stm_mod_reduce(NULL, modelinslope, nodeslew/TTV_UNIT, capa, STM_COPY, STM_SLEW);
    }else if(((start_line->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((start_line->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N)){
      path->MD = stm_mod_reduce(NULL, modelindelay, nodeslew/TTV_UNIT, capa, STM_LOADFIX_RED, STM_DELAY);
      path->MF = stm_mod_reduce(NULL, modelinslope, nodeslew/TTV_UNIT, capa, STM_LOADFIX_RED, STM_SLEW);
    }else if(((start_line->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((start_line->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N)){
      path->MD = stm_mod_reduce(NULL, modelindelay, STM_DEF_SLEW, capa, STM_SLEWFIX_RED, STM_DELAY);
      path->MF = stm_mod_reduce(NULL, modelinslope, STM_DEF_SLEW, capa, STM_SLEWFIX_RED, STM_SLEW);
    }
  }

  if(((path->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && ((path->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
    ttype = STM_HH;
  else if(((path->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && ((path->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
    ttype = STM_HL;
  else if(((path->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) && ((path->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
    ttype = STM_LH;
  else if(((path->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) && ((path->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
    ttype = STM_LL;
  stm_mod_update_transition(path->MD, ttype);
  stm_mod_update_transition(path->MF, ttype);
    
  // temp models destruction ----------------------------------------------------
  if (STM_CACHE) {
    if (mbk_cache_islock (STM_CACHE, modelinslope) == YES)
      mbk_cache_unlock (STM_CACHE, modelinslope);
    if (mbk_cache_islock (STM_CACHE, modelindelay) == YES)
      mbk_cache_unlock (STM_CACHE, modelindelay);
    if (mbk_cache_islock (STM_CACHE, modeloutdelay) == YES)
      mbk_cache_unlock (STM_CACHE, modeloutdelay);
    if (mbk_cache_islock (STM_CACHE, modeloutslope) == YES)
      mbk_cache_unlock (STM_CACHE, modeloutslope);
  }
  if (mindelay) stm_mod_destroy (mindelay);
  if (minslope) stm_mod_destroy (minslope);
  if (moutdelay) stm_mod_destroy (moutdelay);
  if (moutslope) stm_mod_destroy (moutslope);
  stm_delaxisvalues();
    
  if (path->MD!=NULL && ttv_get_path_margins_info(ttvfig, path, &factor, &delta))
    stm_modtbl_scale_and_add_val(path->MD->UMODEL.TABLE, delta/TTV_UNIT, factor);
}

/*****************************************************************************/
/*                        function ttv_allocnewmodel()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* inchain : liste des entrees                                               */
/* goal : noeud sortie                                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* construit les chemins a partir du parcours en largeur du graphe           */
/*****************************************************************************/
void ttv_allocnewmodel(ttvfig,node,goal,ptline,ptend,caracnode,caracgoal,type)
     ttvfig_list *ttvfig ;
     ttvevent_list *node ;
     ttvevent_list *goal ;
     ttvline_list *ptline ;
     ttvline_list *ptend ;
     ttvline_list *caracnode ;
     ttvline_list *caracgoal ;
     long type ;
{
  ttvline_list *ptlinex ;
  char *modelindelay ;
  char *modeloutdelay ;
  char *modeloutslope ;
  long delay, slew = STM_DEF_SLEW * TTV_UNIT ;
  float capa, vt, vth, vdd, vend ;
  timing_model *tmodel;
  char ttype;
  ht *htslope_axis = NULL;
  ht *htcapa_axis = NULL;
  ptype_list *ptype;

  if ((ptype = getptype(ttvfig->USER, TTV_FIG_SAXIS)) != NULL)
    htslope_axis = (ht*)ptype->DATA; 
  if ((ptype = getptype(ttvfig->USER, TTV_FIG_CAXIS)) != NULL)
    htcapa_axis = (ht*)ptype->DATA; 
 
  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    {
      if(caracnode == NULL) 
        {
          if((caracgoal->MDMAX == NULL) && (caracgoal->MFMAX == NULL))
            return ;
        }
      else if(caracgoal == NULL)
        {
          if((caracnode->MDMAX == NULL) && (caracnode->MFMAX == NULL))
            return ;
        }
      else if((caracnode->MDMAX == NULL) && (caracgoal->MDMAX == NULL) &&
              (caracnode->MFMAX == NULL) && (caracgoal->MFMAX == NULL))
        return ;
    }
  else
    {
      if(caracnode == NULL) 
        {
          if((caracgoal->MDMIN == NULL) && (caracgoal->MFMIN == NULL))
            return ;
        }
      else if(caracgoal == NULL)
        {
          if((caracnode->MDMIN == NULL) && (caracnode->MFMIN == NULL))
            return ;
        }
      else if((caracnode->MDMIN == NULL) && (caracgoal->MDMIN == NULL) &&
              (caracnode->MFMIN == NULL) && (caracgoal->MFMIN == NULL))
        return ;
    }

  if(node->FIND->OUTLINE == ptline)
    {
      return ;
    } 

  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    {
      if(caracnode == NULL)
        {
          modelindelay = NULL ;
        }
      else
        {
          modelindelay = caracnode->MDMAX ;
        }
      if(caracgoal == NULL)
        {
          modeloutdelay = NULL ;
          modeloutslope = NULL ;
        }
      else
        {
          modeloutdelay = caracgoal->MDMAX ;
          modeloutslope = caracgoal->MFMAX ;
        }
    }
  else
    {
      if(caracnode == NULL)
        {
          modelindelay = NULL ;
        }
      else
        {
          modelindelay = caracnode->MDMIN ;
        }
      if(caracgoal == NULL)
        {
          modeloutdelay = NULL ;
          modeloutslope = NULL ;
        }
      else
        {
          modeloutdelay = caracgoal->MDMIN ;
          modeloutslope = caracgoal->MFMIN ;
        }
    }

  delay = (long)0 ;
  ptlinex = node->FIND->OUTLINE ;
  if(ptlinex){
    if((type & TTV_FIND_MAX) == TTV_FIND_MAX){
      capa = ttv_getcapamax(ptlinex);
      slew = ttv_getslopemax(ptlinex);
    }else{
      capa = ttv_getcapamin(ptlinex);
      slew = ttv_getslopemin(ptlinex);
    }
  }else
    capa = STM_DONTCARE;
  for(ptlinex = node->FIND->OUTLINE ; ptlinex != NULL && ptlinex->ROOT != goal ; 
      ptlinex = ptlinex->ROOT->FIND->OUTLINE)
    {
      if((ptlinex->TYPE & (TTV_LINE_D|TTV_LINE_T)) == 0)
        {
          if((type & TTV_FIND_MAX) == TTV_FIND_MAX){
            delay += ttv_getdelaymax(ptlinex) ;
            slew = ttv_getslopemax(ptlinex);
          }else{
            delay += ttv_getdelaymin(ptlinex) ;
            slew = ttv_getslopemin(ptlinex);
          }
        }
    }

  if((ptlinex != NULL) && ((ptlinex->TYPE & (TTV_LINE_D|TTV_LINE_T)) == 0))
    {
      if((type & TTV_FIND_MAX) == TTV_FIND_MAX){
        delay += ttv_getdelaymax(ptlinex) ;
        slew = ttv_getslopemax(ptlinex);
      }else{
        delay += ttv_getdelaymin(ptlinex) ;
        slew = ttv_getslopemin(ptlinex);
      }
    }

  if((((ptline->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((ptline->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))
     &&(((ptline->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((ptline->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))){
    stm_addaxisvalues(htslope_axis, htcapa_axis, ptline->NODE->ROOT->NETNAME, ptline->ROOT->ROOT->NETNAME);
  }else if(((ptline->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((ptline->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N)){
    stm_addaxisvalues(htslope_axis, htcapa_axis, ptline->NODE->ROOT->NETNAME, NULL);
  }else if(((ptline->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((ptline->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N)){
    stm_addaxisvalues(htslope_axis, htcapa_axis, NULL, ptline->ROOT->ROOT->NETNAME);
  }

  if(((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && ((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
    ttype = STM_HH;
  else if(((ptline->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && ((ptline->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
    ttype = STM_HL;
  else if(((ptline->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) && ((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP))
    ttype = STM_LH;
  else if(((ptline->NODE->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) && ((ptline->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN))
    ttype = STM_LL;
  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    {
      ptline->MDMAX = ttv_getstmmodelname(ttvfig,ptline,STM_DELAY,STM_MAX) ;
      ptline->MFMAX = ttv_getstmmodelname(ttvfig,ptline,STM_SLEW,STM_MAX) ;
      if(caracnode != caracgoal)
        stm_addmergedmodel(ttvfig->INFO->FIGNAME,ptline->MDMAX,ttvfig->INFO->FIGNAME,
                           modelindelay,ttvfig->INFO->FIGNAME,modeloutdelay,
                           (float)delay/TTV_UNIT,(float)capa,(float)slew/TTV_UNIT) ;
      else
        stm_addreducedmodel(ttvfig->INFO->FIGNAME,ptline->MDMAX,ttvfig->INFO->FIGNAME,
                            modeloutdelay,(float)slew/TTV_UNIT,(float)capa,STM_COPY,STM_DELAY) ;

      if(modeloutslope != NULL){
        if(caracnode != caracgoal)
          stm_addreducedmodel(ttvfig->INFO->FIGNAME,ptline->MFMAX,ttvfig->INFO->FIGNAME,
                              modeloutslope,(float)slew/TTV_UNIT,(float)capa,STM_SLEWFIX_RED,STM_SLEW) ;
        else
          stm_addreducedmodel(ttvfig->INFO->FIGNAME,ptline->MFMAX,ttvfig->INFO->FIGNAME,
                              modeloutslope,(float)slew/TTV_UNIT,(float)capa,STM_COPY,STM_SLEW) ;
      }else
        stm_addconstmodel(ttvfig->INFO->FIGNAME,ptline->MFMAX,ptend->FMAX/TTV_UNIT) ;

      if (!stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MDMAX)) ptline->MDMAX = NULL;
      if (!stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MFMAX)) ptline->MFMAX = NULL;

      if(ptline->MDMAX){
        tmodel = stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MDMAX);
        if((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        else if((ptline->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        vt   = stm_mod_vt(tmodel);
        vth  = stm_mod_vth(tmodel);
        vdd  = stm_mod_vdd(tmodel);
        vend = stm_mod_vf(tmodel);
        stm_mod_update_transition(tmodel, ttype);
      }
      if(ptline->MFMAX){
        tmodel = stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MFMAX);
        stm_mod_update(tmodel, vth, vdd, vt, vend);
        stm_mod_update_transition(tmodel, ttype);
      }
    }
  else
    {
      ptline->MDMIN = ttv_getstmmodelname(ttvfig,ptline,STM_DELAY,STM_MIN) ;
      ptline->MFMIN = ttv_getstmmodelname(ttvfig,ptline,STM_SLEW,STM_MIN) ;
      if(caracnode != caracgoal)
        stm_addmergedmodel(ttvfig->INFO->FIGNAME,ptline->MDMIN,ttvfig->INFO->FIGNAME,
                           modelindelay,ttvfig->INFO->FIGNAME,modeloutdelay,
                           (float)delay/TTV_UNIT,(float)capa,(float)slew/TTV_UNIT) ;
      else
        stm_addreducedmodel(ttvfig->INFO->FIGNAME,ptline->MDMIN,ttvfig->INFO->FIGNAME,
                            modeloutdelay,(float)slew/TTV_UNIT,(float)capa,STM_COPY,STM_DELAY) ;

      if(modeloutslope != NULL){
        if(caracnode != caracgoal)
          stm_addreducedmodel(ttvfig->INFO->FIGNAME,ptline->MFMIN,ttvfig->INFO->FIGNAME,
                              modeloutslope,(float)slew/TTV_UNIT,(float)capa,STM_SLEWFIX_RED,STM_SLEW) ;
        else
          stm_addreducedmodel(ttvfig->INFO->FIGNAME,ptline->MFMIN,ttvfig->INFO->FIGNAME,
                              modeloutslope,(float)slew/TTV_UNIT,(float)capa,STM_COPY,STM_SLEW) ;
      }else
        stm_addconstmodel(ttvfig->INFO->FIGNAME,ptline->MFMIN,ptend->FMIN/TTV_UNIT) ;
   
      if (!stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MDMIN)) ptline->MDMIN = NULL;
      if (!stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MFMIN)) ptline->MFMIN = NULL;

      if(ptline->MDMIN){
        tmodel = stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MDMIN);
        if((ptline->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        else if((ptline->ROOT->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        vt   = stm_mod_vt(tmodel);
        vth  = stm_mod_vth(tmodel);
        vdd  = stm_mod_vdd(tmodel);
        vend = stm_mod_vf(tmodel);
        stm_mod_update_transition(tmodel, ttype);
      }
      if(ptline->MFMIN){
        tmodel = stm_getmodel(ttvfig->INFO->FIGNAME, ptline->MFMIN);
        stm_mod_update(tmodel, vth, vdd, vt, vend);
        stm_mod_update_transition(tmodel, ttype);
      }
    }
  stm_delaxisvalues();
}

/*****************************************************************************/
/*                        function ttv_addpath()                             */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* inchain : liste des entrees                                               */
/* goal : noeud sortie                                                       */
/* type : type de recherche                                                  */
/*                                                                           */
/* construit les chemins a partir du parcours en largeur du graphe           */
/*****************************************************************************/
void ttv_addpath(ttvfig,inchain,goal,cmd,type)
     ttvfig_list *ttvfig ;
     chain_list *inchain ;
     ttvevent_list *goal ;
     ttvevent_list *cmd ;
     long type ;
{
  ttvevent_list *nodex ;
  ttvevent_list *avnodex ;
  ttvevent_list *node ;
  ttvevent_list *goalx ;
  ttvline_list *line ;
  chain_list *chainout ;
  chain_list *chainend ;
  chain_list *chainsav ;
  chain_list *chain ;
  ptype_list *ptype ;
  long delay ;
  long typeline ;
  char flag ;
  char first ;

  if((inchain == NULL) || 
     ((goal->ROOT->TYPE & TTV_SIG_BYPASSOUT) == TTV_SIG_BYPASSOUT) ||
     ((goal->TYPE & TTV_NODE_BYPASSOUT) == TTV_NODE_BYPASSOUT))
    {
      for(chain = inchain ; chain != NULL ; chain = chain->NEXT)
        ((ttvevent_list *)chain->DATA)->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
      ttv_fifoclean() ;
      return ;
    }

  chainout = inchain ;
  chainend = inchain ;
  chain = inchain->NEXT ;
  inchain->NEXT = NULL ;

  while(chain != NULL)
    {
      chainsav = chain->NEXT ;
      if((((ttvevent_list *)chain->DATA)->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
        {
          chain->NEXT = chainout ;
          chainout = chain ;
        }
      else
        {
          chain->NEXT = NULL ;
          chainend->NEXT = chain ;
          chainend = chain ;
        }
      chain = chainsav ;
    }

  inchain = chainout ;
  chainout = NULL ;
  chainend = NULL ;

  for(chain = inchain ; chain != NULL ; chain = chain->NEXT)
    {
      node = (ttvevent_list *)chain->DATA ;
      node->FIND->TYPE  &= ~(TTV_FIND_BEGIN) ;
      if(((node->ROOT->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN) ||
         ((node->TYPE & TTV_NODE_BYPASSIN) == TTV_NODE_BYPASSIN))
        continue ;
      flag = 'N' ;
      if(node == goal)
        first = 'Y' ;
      else
        first = 'N' ;
      goalx = node ;

      if(((goal->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) != 0)
         && ((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
        {
          node->TYPE |= TTV_NODE_MARQUE ;
        }
      else if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) != 0)
              && ((goal->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
        {
          goal->TYPE |= TTV_NODE_MARQUE ;
        }

      if((goal->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) == TTV_SIG_Q)
        {
          goal->TYPE |= TTV_NODE_MARQUE ;
        }

      if((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) == TTV_SIG_Q)
        {
          node->TYPE |= TTV_NODE_MARQUE ;
        }

      for(nodex = node ; nodex->FIND->OUTLINE != NULL && 
            ((nodex != goal) || (first == 'Y')) ;
          nodex = nodex->FIND->OUTLINE->ROOT)
        {
          first = 'N' ;
          if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
            {
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                nodex->FIND->OUTLINE->TYPE |= TTV_LINE_DEPTMAX ;
              else
                nodex->FIND->OUTLINE->TYPE |= TTV_LINE_DEPTMIN ;
            }
     
          if(((((nodex->ROOT->TYPE & TTV_SIG_I) == TTV_SIG_I) && 
               ((((nodex->TYPE & TTV_NODE_IMAX) == TTV_NODE_IMAX) &&
                 ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
                (((nodex->TYPE & TTV_NODE_IMIN) == TTV_NODE_IMIN) &&
                 ((type & TTV_FIND_MIN) == TTV_FIND_MIN))))) && (nodex != node))
            {
              if(flag == 'Y')
                {
                  flag = 'N' ;
                  if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) != 0) ||
                     ((goal->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) != 0))
                    {
                      if((nodex->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)
                        {
                          nodex->TYPE |= TTV_NODE_MARQUE ;
                          chainend = addchain(chainend,nodex) ;
                        }
                      if((goalx->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)
                        {
                          goalx->TYPE |= TTV_NODE_MARQUE ;
                          chainend = addchain(chainend,goalx) ;
                        }
                    }
                  if((goalx->FIND->TYPE & TTV_FIND_BEGIN) != TTV_FIND_BEGIN)
                    {
                      goalx->FIND->TYPE |= TTV_FIND_BEGIN ;
                      if((nodex->TYPE & TTV_NODE_END) != TTV_NODE_END)
                        {
                          chainout = addchain(chainout,(void*)nodex) ;
                          nodex->TYPE |= TTV_NODE_END ;
                        }
                      goalx->USER = addptype(goalx->USER,TTV_NODE_AVNODE,(void *)avnodex) ;
                      goalx->USER = addptype(goalx->USER,TTV_NODE_IN,(void*)nodex) ;
                      if((ptype = getptype(nodex->USER,TTV_NODE_INCHAIN)) == NULL)
                        nodex->USER = addptype(nodex->USER,TTV_NODE_INCHAIN,
                                               (void*)addchain(NULL,(void*)goalx)) ;
                      else
                        ptype->DATA = (chain_list *)addchain((chain_list *)ptype->DATA,
                                                             (void*)goalx) ;
                    }
                }
              goalx = nodex ;
            } 
          if(nodex->FIND->OUTLINE->FIG == ttvfig)
            {
              flag = 'Y' ;
            }
          avnodex = nodex ;
        }

      if(nodex == goal)
        {
          if(flag == 'Y')
            {
              if((goalx->FIND->TYPE & TTV_FIND_BEGIN) != TTV_FIND_BEGIN)
                {
                  goalx->FIND->TYPE |= TTV_FIND_BEGIN ; 
                  if((goalx != node) && 
                     ((((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) != 0) ||
                       ((goal->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N|TTV_SIG_Q)) != 0)) &&
                      ((goalx->TYPE & TTV_NODE_MARQUE) != TTV_NODE_MARQUE)))
                    {
                      goalx->TYPE |= TTV_NODE_MARQUE ;
                      chainend = addchain(chainend,goalx) ;
                    }
                  if((nodex->TYPE & TTV_NODE_END) != TTV_NODE_END)
                    {
                      chainout = addchain(chainout,(void*)nodex) ;
                      nodex->TYPE |= TTV_NODE_END ;
                    }
                  goalx->USER = addptype(goalx->USER,TTV_NODE_IN,(void*)nodex) ;
                  goalx->USER = addptype(goalx->USER,TTV_NODE_AVNODE,(void *)avnodex) ;
                  if((ptype = getptype(nodex->USER,TTV_NODE_INCHAIN)) == NULL)
                    nodex->USER = addptype(nodex->USER,TTV_NODE_INCHAIN,
                                           (void*)addchain(NULL,(void*)goalx)) ;
                  else
                    ptype->DATA = (chain_list *)addchain((chain_list *)ptype->DATA,
                                                         (void*)goalx) ;
                }
            }
        }
    }
 
  inchain = append(chainend,inchain) ;

  for(chainend = chainout ; chainend != NULL ; chainend = chainend->NEXT)
    {
      goalx = (ttvevent_list *)chainend->DATA ;
      goalx->TYPE &= ~(TTV_NODE_END) ;
      if(((type & TTV_FIND_HZ) == TTV_FIND_HZ) &&
         (((goalx->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) ||
          ((goalx->ROOT->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ) ||
          ((goalx->ROOT->TYPE & TTV_SIG_CT) == TTV_SIG_CT)))
        typeline = TTV_LINE_HZ ;
      else if(((goalx->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
              ((goalx->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
        {
          if((type & TTV_FIND_R) == TTV_FIND_R)
            typeline = TTV_LINE_R ;
          else
            typeline = TTV_LINE_S ;
        }
      else typeline = (long)0 ;
      chainsav = NULL ;
      for(line = goalx->INPATH ; line != NULL ; line = line->NEXT)
        {
          if(((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
             (line->NODE->FIND == NULL))
            continue ;
          if((((node = line->NODE)->FIND->TYPE & TTV_FIND_BEGIN) == TTV_FIND_BEGIN) &&
             (line->FIG == ttvfig))
            if((ptype = getptype(node->USER,TTV_NODE_IN)) != NULL)
              if((ttvevent_list *)ptype->DATA == goalx)
                {
                  if((((goalx->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE) &&
                      ((line->NODE->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE) &&
                      (line->NODE != goalx)) &&
                     ((line->TYPE & TTV_LINE_P) == TTV_LINE_P))
                    {
                      chainsav = addchain(chainsav,line) ;
                    }

                  if((typeline & TTV_LINE_HZ) != (line->TYPE & TTV_LINE_HZ))
                    continue ; 
            
                  if((typeline & TTV_LINE_R) != (line->TYPE & TTV_LINE_R))
                    continue ; 

                  if((typeline & TTV_LINE_S) != (line->TYPE & TTV_LINE_S))
                    continue ; 

                  if((cmd != NULL) && ((goalx->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L))
                    if(cmd != ttv_getlinecmd(ttvfig,line,((type & TTV_FIND_MAX) == TTV_FIND_MAX)?TTV_LINE_CMDMAX:TTV_LINE_CMDMIN))
                      continue ;
            
                  avnodex = (ttvevent_list *)getptype(node->USER,TTV_NODE_AVNODE)->DATA ;
                  node->USER = delptype(node->USER,TTV_NODE_AVNODE) ;

                  node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
                  node->USER = delptype(node->USER,TTV_NODE_IN) ;

                  if(line->ROOT == goal)
                    delay = line->NODE->FIND->DELAY ;
                  else
                    delay = line->NODE->FIND->DELAY - line->ROOT->FIND->DELAY ;
                  line->TYPE |= TTV_LINE_INFIG ;

                  if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                    {
                      if((line->VALMAX != TTV_NOTIME) && (ttv_getdelaymax(line) >= delay))
                        continue ;
                      if((((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) ||
                          ((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)) &&
                         ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                        {
                          ttvline_list *caracin ;
                          ttvline_list *caracout ;

                          if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                             ((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                            {
                              caracin = node->FIND->OUTLINE ;
                            }
                          else
                            {
                              caracin = NULL ;
                            }
                          if(((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                             ((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                            {
                              caracout = avnodex->FIND->OUTLINE ;
                            }
                          else
                            {
                              caracout = NULL ;
                            }
                          if((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC)
                            ttv_allocnewmodel(ttvfig,node,goalx,line,avnodex->FIND->OUTLINE,
                                              caracin,caracout,TTV_FIND_MAX) ;
                        }

                      line->VALMAX = delay ;
                      line->FMAX = ttv_getslopemax(avnodex->FIND->OUTLINE) ;
                    }
                  else
                    {
                      if((line->VALMIN != TTV_NOTIME) && (ttv_getdelaymin(line) <= delay))
                        continue ;
                      if((((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) ||
                          ((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)) &&
                         ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                        {
                          ttvline_list *caracin ;
                          ttvline_list *caracout ;

                          if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                             ((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                            {
                              caracin = node->FIND->OUTLINE ;
                            }
                          else
                            {
                              caracin = NULL ;
                            }
                          if(((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                             ((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                            {
                              caracout = avnodex->FIND->OUTLINE ;
                            }
                          else
                            {
                              caracout = NULL ;
                            }
                          if((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC)
                            ttv_allocnewmodel(ttvfig,node,goalx,line,avnodex->FIND->OUTLINE,
                                              caracin,caracout,TTV_FIND_MIN) ;
                        }

                      line->VALMIN = delay ;
                      line->FMIN = ttv_getslopemin(avnodex->FIND->OUTLINE) ;
                    }

                  if((line->ROOT->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
                    {
                      if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_EV) == TTV_LINE_EV)
                        line->TYPE |= TTV_LINE_EV ;
                      if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
                        line->TYPE |= TTV_LINE_PR ;
                    }

                  if(((line->ROOT->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
                     ((line->ROOT->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
                    {
                      if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_S) == TTV_LINE_S)
                        line->TYPE |= TTV_LINE_S ;
                      if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_R) == TTV_LINE_R)
                        line->TYPE |= TTV_LINE_R ;
                    }

                  if((goalx->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
                    {
                      ptype_list *ptypecmd ;

                      if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                        {
                          ptypecmd = ttv_addcmd(line,TTV_LINE_CMDMAX,
                                                ttv_getlinecmd(ttvfig,avnodex->FIND->OUTLINE,TTV_LINE_CMDMAX)) ;
                        }
                      else
                        {
                          ptypecmd = ttv_addcmd(line,TTV_LINE_CMDMIN, 
                                                ttv_getlinecmd(ttvfig,avnodex->FIND->OUTLINE,TTV_LINE_CMDMIN)) ;
                        }
                    }
                }
        }

      for(chain = chainsav ; chain != NULL ; chain = chain->NEXT)
        {
          line = (ttvline_list *)chain->DATA ;
          line = ttv_addline(ttvfig,line->ROOT,line->NODE,line->VALMAX,line->FMAX,
                             line->VALMIN,line->FMIN,TTV_LINE_J|TTV_LINE_INFIG) ;
          line->TYPE |= (((ttvline_list *)chain->DATA)->TYPE 
                         & (TTV_LINE_HZ|TTV_LINE_EV|TTV_LINE_PR|TTV_LINE_RC|
                            TTV_LINE_R|TTV_LINE_S)) ;
          if((ptype = getptype(((ttvline_list *)chain->DATA)->USER,
                               TTV_LINE_CMDMAX))!= NULL) 
            ttv_addcmd(line,TTV_LINE_CMDMAX,(ttvevent_list *)ptype->DATA) ;
          if((ptype = getptype(((ttvline_list *)chain->DATA)->USER,
                               TTV_LINE_CMDMIN))!= NULL) 
            ttv_addcmd(line,TTV_LINE_CMDMIN,(ttvevent_list *)ptype->DATA) ;
          ttv_delline((ttvline_list *)chain->DATA) ;
        }
   
      freechain(chainsav) ;

      ptype = getptype(goalx->USER,TTV_NODE_INCHAIN) ;
      for(chain = (chain_list *)ptype->DATA ;
          chain != NULL ; chain = chain->NEXT)
        {
          node = (ttvevent_list *)chain->DATA ;

          if((node->FIND->TYPE & TTV_FIND_BEGIN) != TTV_FIND_BEGIN)
            {
              continue ;
            }

          avnodex = (ttvevent_list *)getptype(node->USER,TTV_NODE_AVNODE)->DATA ;
          node->USER = delptype(node->USER,TTV_NODE_AVNODE) ;

          if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
            {
              if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) ||
                 ((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0))
                {
                  ttvline_list *caracin ; 
                  ttvline_list *caracout ; 

                  if(goalx == goal)
                    delay = node->FIND->DELAY ;
                  else
                    delay = node->FIND->DELAY - goalx->FIND->DELAY ;
                  line = ttv_addline(ttvfig,goalx,node,delay,
                                     ttv_getslopemax(avnodex->FIND->OUTLINE),TTV_NOTIME,TTV_NOSLOPE,
                                     typeline|TTV_LINE_T|TTV_LINE_INFIG) ;
 
                  if(((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                     (node->FIND->OUTLINE->ROOT == goalx))
                    line->TYPE |= TTV_LINE_RC ;

                  if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                     ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) &&
                     ((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                    {
                      caracin = node->FIND->OUTLINE ;
                    }
                  else
                    caracin = NULL ;
                  if(((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                     ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) && 
                     ((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                    {
                      caracout = avnodex->FIND->OUTLINE ;
                    }
                  else
                    caracout = NULL ;
                  if((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC)
                    ttv_allocnewmodel(ttvfig,node,goalx,line,avnodex->FIND->OUTLINE,caracin,
                                      caracout,TTV_FIND_MAX) ;
                }
              else
                {
                  if(((goalx->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE) &&
                     ((node->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE) && 
                     (goalx != node))
                    {
                      if(goalx == goal)
                        delay = node->FIND->DELAY ;
                      else
                        delay = node->FIND->DELAY - goalx->FIND->DELAY ;
                      line = ttv_addline(ttvfig,goalx,node,delay,
                                         ttv_getslopemax(avnodex->FIND->OUTLINE),TTV_NOTIME,TTV_NOSLOPE,
                                         typeline|TTV_LINE_J|TTV_LINE_INFIG) ;
                    }
                  else
                    {
                      if(goalx == goal)
                        delay = node->FIND->DELAY ;
                      else
                        delay = node->FIND->DELAY - goalx->FIND->DELAY ;
                      line = ttv_addline(ttvfig,goalx,node,delay,
                                         ttv_getslopemax(avnodex->FIND->OUTLINE),TTV_NOTIME,TTV_NOSLOPE,
                                         typeline|TTV_LINE_P|TTV_LINE_INFIG) ;
                    }
 
                  if(((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                     (node->FIND->OUTLINE->ROOT == goalx))
                    line->TYPE |= TTV_LINE_RC ;
                }
            }
          else
            {
              if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) ||
                 ((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0))
                {
                  ttvline_list *caracin ; 
                  ttvline_list *caracout ; 
 
                  if(goalx == goal)
                    delay = node->FIND->DELAY ;
                  else
                    delay = node->FIND->DELAY - goalx->FIND->DELAY ;
                  line = ttv_addline(ttvfig,goalx,node,TTV_NOTIME,TTV_NOSLOPE,
                                     delay,ttv_getslopemin(avnodex->FIND->OUTLINE),
                                     typeline|TTV_LINE_T|TTV_LINE_INFIG) ;

                  if(((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                     (node->FIND->OUTLINE->ROOT == goalx))
                    line->TYPE |= TTV_LINE_RC ;
 
                  if(((node->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                     ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) &&
                     ((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                    {
                      caracin = node->FIND->OUTLINE ;
                    }
                  else
                    caracin = NULL ;
                  if(((goalx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
                     ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) &&
                     ((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
                    {
                      caracout = avnodex->FIND->OUTLINE ;
                    }
                  else
                    caracout = NULL ;
                  if((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC)
                    ttv_allocnewmodel(ttvfig,node,goalx,line,avnodex->FIND->OUTLINE,caracin,
                                      caracout,TTV_FIND_MIN) ;
                }
              else
                {
                  if(((goalx->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE) &&
                     ((node->TYPE & TTV_NODE_MARQUE) == TTV_NODE_MARQUE) &&
                     (goalx != node))
                    {
                      if(goalx == goal)
                        delay = node->FIND->DELAY ;
                      else
                        delay = node->FIND->DELAY - goalx->FIND->DELAY ;
                      line = ttv_addline(ttvfig,goalx,node,TTV_NOTIME,TTV_NOSLOPE,
                                         delay,ttv_getslopemin(avnodex->FIND->OUTLINE),
                                         typeline|TTV_LINE_J|TTV_LINE_INFIG) ;
                    }
                  else
                    {
                      if(goalx == goal)
                        delay = node->FIND->DELAY ;
                      else
                        delay = node->FIND->DELAY - goalx->FIND->DELAY ;
                      line = ttv_addline(ttvfig,goalx,node,TTV_NOTIME,TTV_NOSLOPE,
                                         delay,ttv_getslopemin(avnodex->FIND->OUTLINE),
                                         typeline|TTV_LINE_P|TTV_LINE_INFIG) ;
                    }

                  if(((node->FIND->OUTLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC) &&
                     (node->FIND->OUTLINE->ROOT == goalx))
                    line->TYPE |= TTV_LINE_RC ;
                }
            }
 
          if((line->ROOT->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
            {
              if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_EV) == TTV_LINE_EV)
                line->TYPE |= TTV_LINE_EV ;
              if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
                line->TYPE |= TTV_LINE_PR ;
            }

          if(((line->ROOT->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
             ((line->ROOT->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
            {
              if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_S) == TTV_LINE_S)
                line->TYPE |= TTV_LINE_S ;
              if((avnodex->FIND->OUTLINE->TYPE & TTV_LINE_R) == TTV_LINE_R)
                line->TYPE |= TTV_LINE_R ;
            }
 
          if((goalx->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
            {
              ptype_list *ptypecmd ;
 
              if((type & TTV_FIND_MAX) == TTV_FIND_MAX)
                {
                  ptypecmd = ttv_addcmd(line,TTV_LINE_CMDMAX,
                                        ttv_getlinecmd(ttvfig,avnodex->FIND->OUTLINE,TTV_LINE_CMDMAX)) ;
                }
              else
                {
                  ptypecmd = ttv_addcmd(line,TTV_LINE_CMDMIN,
                                        ttv_getlinecmd(ttvfig,avnodex->FIND->OUTLINE,TTV_LINE_CMDMIN)) ;
                }
            }

          node->FIND->TYPE &= ~(TTV_FIND_BEGIN) ;
          node->USER = delptype(node->USER,TTV_NODE_IN) ;
        }
      freechain((chain_list *)ptype->DATA) ;
      goalx->USER = delptype(goalx->USER,TTV_NODE_INCHAIN) ;
    }
  for(chain = inchain ; chain != NULL ; chain = chain->NEXT)
    {
      ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_MARQUE) ;
    }
  goal->TYPE &= ~(TTV_NODE_MARQUE) ;
  ttv_fifoclean() ;
  freechain(chainout);
  freechain(inchain);
}

/*****************************************************************************/
/*                        function ttv_builtpath()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* type : type de recherche                                                  */
/*                                                                           */
/* construit tout les chemins d'une ttvfig a partir des arcs elementaires    */
/*****************************************************************************/
void ttv_builtpath(ttvfig,type)
     ttvfig_list *ttvfig ;
     long type ;
{
  ttvsig_list  *sig ;
  ttvlbloc_list *ptlbloc[3] ;
  ttvline_list *ptline ;
  ttvline_list *ptlinex ;
  chain_list *chainfree = NULL ;
  chain_list *chain ;
  chain_list *chainc ;
  long i ;
  long j ;
  long typers ;

  if(ttvfig == NULL)
    return ;

  if((ttvfig->STATUS & TTV_STS_DUAL) != (long)0)
    ttv_freedualline(ttvfig,TTV_STS_DUAL) ;
 
  type &= ~(TTV_FIND_MAX | TTV_FIND_MIN | TTV_FIND_DUAL | TTV_FIND_HZ) ;

  if((((ttvfig->STATUS & TTV_STS_DTX) != TTV_STS_DTX) &&
      ((type & TTV_FIND_LINE) == TTV_FIND_LINE)) ||
     (((ttvfig->STATUS & TTV_STS_TTX) != TTV_STS_TTX) &&
      ((type & TTV_FIND_PATH) == TTV_FIND_PATH)))
    
    {
      ttv_error(31,ttvfig->INFO->FIGNAME,TTV_WARNING) ;
      return ;
    }

  for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
    {
      sig = *(ttvfig->CONSIG + i) ;
      if(((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
         ((sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
        continue ;
      if((sig->TYPE & TTV_SIG_CO) == TTV_SIG_CO)
        {
          if((sig->TYPE & TTV_SIG_L) == TTV_SIG_L)
            chainc = ttv_getlrcmd(ttvfig,sig) ;
          else chainc = NULL ;
          if(chainc == NULL)
            chainc = addchain(chainc,NULL) ;
          for(chain = chainc ; chain != NULL ; chain = chain->NEXT)
            {
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,(ttvevent_list *)chain->DATA,type|TTV_FIND_MAX,
                                              ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MAX) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,(ttvevent_list *)chain->DATA,type|TTV_FIND_MAX,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MAX) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,(ttvevent_list *)chain->DATA,type|TTV_FIND_MIN,
                                              ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MIN) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,(ttvevent_list *)chain->DATA,type|TTV_FIND_MIN,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MIN) ;
            }
          freechain(chainc) ;
          if(((sig->TYPE & TTV_SIG_CT) == TTV_SIG_CT) ||
             ((sig->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ))
            {
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MAX|TTV_FIND_HZ,
                                              ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MAX|TTV_FIND_HZ) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MAX|TTV_FIND_HZ,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MAX|TTV_FIND_HZ) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MIN|TTV_FIND_HZ,
                                              ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MIN|TTV_FIND_HZ) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MIN|TTV_FIND_HZ,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MIN|TTV_FIND_HZ) ;
            }
        }
    }

  if((type & TTV_FIND_HIER) == TTV_FIND_HIER)
    for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
      {
        sig = *(ttvfig->NCSIG + i) ;
        if(((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
           ((sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
          continue ;
        if((sig->TYPE & TTV_SIG_L) == TTV_SIG_L)
          chainc = ttv_getlrcmd(ttvfig,sig) ;
        else chainc = NULL ;
        if(chainc == NULL)
          chainc = addchain(chainc,NULL) ;
        for(chain = chainc ; chain != NULL ; chain = chain->NEXT)
          {
            ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,(ttvevent_list *)chain->DATA,type|TTV_FIND_MAX,
                                            ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MAX) ;
            ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,(ttvevent_list *)chain->DATA,type|TTV_FIND_MAX,
                                            ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MAX) ;
            ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,(ttvevent_list *)chain->DATA,type|TTV_FIND_MIN,
                                            ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MIN) ;
            ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,(ttvevent_list *)chain->DATA,type|TTV_FIND_MIN,
                                            ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MIN) ;
          }
        freechain(chainc) ;
      }

  for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
    {
      sig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i) 
        : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
      if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
        {
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MAX,
                                          ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MAX) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MAX,
                                          ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MAX) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MIN,
                                          ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MIN) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MIN,
                                          ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MIN) ;
        }
    }

  for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
    {
      sig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i) 
        : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
      chainc = ttv_getlrcmd(ttvfig,sig) ;
      if(chainc == NULL)
        chainc = addchain(chainc,NULL) ;
      for(chain = chainc ; chain != NULL ; chain = chain->NEXT)
        {
          if(((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
             ((sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
            {
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,
                                              (ttvevent_list *)chain->DATA,type|TTV_FIND_MAX|TTV_FIND_R,
                                              ttvfig->INFO->LEVEL),sig->NODE,(ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MAX|TTV_FIND_R) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,
                                              (ttvevent_list *)chain->DATA,type|TTV_FIND_MAX|TTV_FIND_R,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,(ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MAX|TTV_FIND_R) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,
                                              (ttvevent_list *)chain->DATA,type|TTV_FIND_MIN|TTV_FIND_R,
                                              ttvfig->INFO->LEVEL),sig->NODE,(ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MIN|TTV_FIND_R) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,
                                              (ttvevent_list *)chain->DATA,type|TTV_FIND_MIN|TTV_FIND_R,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,(ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MIN|TTV_FIND_R) ;
              typers = TTV_FIND_S ;
            }
          else
            typers = (long)0 ; ;
          if(((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) || (typers == TTV_FIND_S))
            {
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,(ttvevent_list *)chain->DATA,type|TTV_FIND_MAX|typers,
                                              ttvfig->INFO->LEVEL),sig->NODE,
                          (ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MAX|typers) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,(ttvevent_list *)chain->DATA,type|TTV_FIND_MAX|typers,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,
                          (ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MAX|typers) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,(ttvevent_list *)chain->DATA,type|TTV_FIND_MIN|typers,
                                              ttvfig->INFO->LEVEL),sig->NODE,
                          (ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MIN|typers) ;
              ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,(ttvevent_list *)chain->DATA,type|TTV_FIND_MIN|typers,
                                              ttvfig->INFO->LEVEL),sig->NODE+1,
                          (ttvevent_list *)chain->DATA,
                          type|TTV_FIND_MIN|typers) ;
            }
        }
      freechain(chainc) ;
    }

  for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
    {
      sig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i) 
        : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
      if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
        {
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MAX,
                                          ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MAX) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MAX,
                                          ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MAX) ;
        }
      ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,
                                      NULL,type|TTV_FIND_MAX|TTV_FIND_HZ,ttvfig->INFO->LEVEL),sig->NODE,
                  NULL,type|TTV_FIND_MAX|TTV_FIND_HZ) ;
      ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,
                                      NULL,type|TTV_FIND_MAX|TTV_FIND_HZ,ttvfig->INFO->LEVEL),sig->NODE+1,
                  NULL,type|TTV_FIND_MAX|TTV_FIND_HZ) ;
      if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
        {
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MIN,
                                          ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MIN) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MIN,
                                          ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MIN) ;
        }
      ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,
                                      type|TTV_FIND_MIN|TTV_FIND_HZ,ttvfig->INFO->LEVEL),sig->NODE,
                  NULL,type|TTV_FIND_MIN|TTV_FIND_HZ) ;
      ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,
                                      type|TTV_FIND_MIN|TTV_FIND_HZ,ttvfig->INFO->LEVEL),sig->NODE+1,
                  NULL,type|TTV_FIND_MIN|TTV_FIND_HZ) ;
    }

  for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
    {
      sig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i) 
        : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
      if((sig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0)
        {
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MAX,
                                          ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MAX) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MAX,
                                          ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MAX) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE,NULL,type|TTV_FIND_MIN,
                                          ttvfig->INFO->LEVEL),sig->NODE,NULL,type|TTV_FIND_MIN) ;
          ttv_addpath(ttvfig,ttv_findpath(ttvfig,ttvfig,sig->NODE+1,NULL,type|TTV_FIND_MIN,
                                          ttvfig->INFO->LEVEL),sig->NODE+1,NULL,type|TTV_FIND_MIN) ;
        }
    }


  ttvfig->STATUS |= TTV_STS_TTX ;

  ttv_fifodelete() ;

  ptlbloc[0] = ttvfig->FBLOC ;
  ptlbloc[1] = ttvfig->EBLOC ;
  ptlbloc[2] = ttvfig->DBLOC ;
  for(i = 0 ; i < 3 ; i++)
    for( ; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
      {
        for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
          {
            ptline = ptlbloc[i]->LINE + j ;
            if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
              continue ;

            if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
              {
                if((ptline->TYPE & TTV_LINE_DEPTMAX) == TTV_LINE_DEPTMAX)
                  ptline->TYPE &= ~(TTV_LINE_DEPTMAX) ;
                else
                  ptline->TYPE |= TTV_LINE_DENPTMAX ;
                if((ptline->TYPE & TTV_LINE_DEPTMIN) == TTV_LINE_DEPTMIN)
                  ptline->TYPE &= ~(TTV_LINE_DEPTMIN) ;
                else
                  ptline->TYPE |= TTV_LINE_DENPTMIN ;

                if((ptline->TYPE & TTV_LINE_CONT) ==  TTV_LINE_CONT)
                  {
                    if((ptline->TYPE & TTV_LINE_D) ==  TTV_LINE_D)
                      {
                        ptlinex = ttv_addline(ttvfig,ptline->ROOT,ptline->NODE,
                                              ttv_getdelaymax(ptline),ttv_getslopemax(ptline),
                                              ttv_getdelaymin(ptline),ttv_getslopemin(ptline),
                                              TTV_LINE_T|TTV_LINE_CONT|
                                              (ptline->TYPE & (TTV_LINE_A|TTV_LINE_U|TTV_LINE_O))) ;
                        ttv_addcaracline(ptlinex,ptline->MDMAX,ptline->MDMIN,
                                         ptline->MFMAX,ptline->MFMIN) ;
                      }
                    else
                      {
                        ptlinex = ttv_addline(ttvfig,ptline->ROOT,ptline->NODE,
                                              ttv_getdelaymax(ptline),ttv_getslopemax(ptline),
                                              ttv_getdelaymin(ptline),ttv_getslopemin(ptline),
                                              TTV_LINE_J|TTV_LINE_CONT|
                                              (ptline->TYPE & (TTV_LINE_A|TTV_LINE_U|TTV_LINE_O))) ;
                      }
                    if((ptline->ROOT->ROOT->TYPE & TTV_SIG_L) ==  TTV_SIG_L)
                      {
                        ttv_addcmd(ptlinex,TTV_LINE_CMDMAX,
                                   ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMAX)) ;
                        ttv_addcmd(ptlinex,TTV_LINE_CMDMIN, 
                                   ttv_getlinecmd(ttvfig,ptline,TTV_LINE_CMDMIN)) ;
                      }
                  }
              }
          }
      } 

  if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    {
      ptlbloc[2] = ttvfig->TBLOC ;
      ptlbloc[1] = ttvfig->PBLOC ;
      ptlbloc[0] = ttvfig->JBLOC ;
      for(i = 0 ; i < 3 ; i++)
        for(; ptlbloc[i] != NULL ; ptlbloc[i] = ptlbloc[i]->NEXT)
          {
            for(j = 0 ; j < TTV_MAX_LBLOC ; j++)
              {
                ptline = ptlbloc[i]->LINE + j ;
                if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
                  continue ;

                if(((ptline->NODE->ROOT->TYPE & TTV_SIG_S) == TTV_SIG_S) || 
                   ((ptline->ROOT->ROOT->TYPE & TTV_SIG_S) == TTV_SIG_S) ||
                   /* pour le pb des commandes
                      ((ptline->NODE->ROOT->TYPE & (TTV_SIG_Q | TTV_SIG_C | TTV_SIG_N)) 
                      == TTV_SIG_Q) ||
                   */
                   (((ptline->TYPE & TTV_LINE_INFIG) != TTV_LINE_INFIG) &&
                    ((ptline->TYPE & TTV_LINE_CONT) != TTV_LINE_CONT)))
                  {
                    if((ptline->TYPE & TTV_LINE_T) == TTV_LINE_T)
                      {
                        if(ptline->MDMAX != NULL)
                          chainfree = addchain(chainfree,ptline->MDMAX) ;
                        if(ptline->MFMAX != NULL)
                          chainfree = addchain(chainfree,ptline->MFMAX) ;
                        if(ptline->MDMIN != NULL)
                          chainfree = addchain(chainfree,ptline->MDMIN) ;
                        if(ptline->MFMIN != NULL)
                          chainfree = addchain(chainfree,ptline->MFMIN) ;
                      }
         
                    ttv_delline(ptline) ;
                    continue ;
                  }

                ptline->TYPE &= ~(TTV_LINE_INFIG) ;
              } 
          }
    }

  if(chainfree != NULL)
    {
      stm_freemodellist(ttvfig->INFO->FIGNAME,chainfree) ;
      freechain(chainfree) ;
    }
}

void ttv_FreeMarkedSigList()
{
  ttvsig_list *tvs;
  while (MARKED_SIG_LIST!=NULL)
    {
      tvs=(ttvsig_list *)MARKED_SIG_LIST->DATA;
      if (getptype(tvs->USER, TTV_SIG_MARKED)==NULL)
        {
          tvs->USER=delptype(tvs->USER, TTV_SIG_MARKED);
        }
      MARKED_SIG_LIST=delchain(MARKED_SIG_LIST, MARKED_SIG_LIST);
    }
}

static int marked_for(ttvevent_list *evi, ttvevent_list *evo, int hz)
{
  ptype_list *pt;
  long mask;
  if (evo->TYPE & TTV_NODE_UP) mask=1; else mask=2;
  if (hz) mask=mask<<8;
  if ((pt=getptype(evi->USER, TTV_SIG_MARKED))!=NULL && ((long)pt->DATA & mask)!=0) return 1;
  return 0;
}

static void mark_for(ttvevent_list *evi, ttvevent_list *evo, int hz)
{
  ptype_list *pt;
  long mask;
  if (evo->TYPE & TTV_NODE_UP) mask=1; else mask=2;
  if (hz) mask=mask<<8;
  if ((pt=getptype(evi->USER, TTV_SIG_MARKED))==NULL)
    pt=evi->USER=addptype(evi->USER, TTV_SIG_MARKED, (void *)0);
  mask|=(long)pt->DATA;
  pt->DATA=(void *)mask;
}

ttvpath_list *filter_parallel_to_node(ttvpath_list *pth)
{
  ttvpath_list *path, *next;
  chain_list *cl=NULL, *ch;
  int hz;
  for (path=pth; path!=NULL; path=next)
    {
      next=path->NEXT;
      if ((path->TYPE & TTV_FIND_HZ)==TTV_FIND_HZ) hz=1;
      else hz=0;
      if (!marked_for(path->NODE, path->ROOT, hz))
        {
          cl=addchain(cl, path);
          mark_for(path->NODE, path->ROOT, hz);
        }
      else
        {
          path->NEXT=NULL;
          ttv_freepathlist(path);
        }
    }

  path=NULL;
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      pth=(ttvpath_list *)ch->DATA;
      pth->NEXT=path;
      pth->NODE->USER=testanddelptype(pth->NODE->USER, TTV_SIG_MARKED);

      path=pth;
    }
  
  freechain(cl);

  return path;
}

ttvpath_list *ttv_getpathnocross_v2(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type,addasync)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
     int addasync;
{
  ttvpath_list *path;

  ttv_reset_search_informations();
  if (addasync) ttv_search_mode(1, TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS);

  path = ttv_getpathnocross (ttvfig, figpath, ptsig, mask, delaymax, delaymin, type);
  
  if (ttv_get_search_informations() & TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH)
    {
      ttv_freepathlist(path);
      path = ttv_getpathsignocross (ttvfig, figpath, ptsig, mask, delaymax, delaymin, type);
      path=filter_parallel_to_node(path);
    }
  if (addasync) ttv_search_mode(0, TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS);
  return path;
}

ttvpath_list *ttv_getaccessnocross_v2(ttvfig,figpath,ptsig,mask,delaymax,delaymin,type,addasync)
     ttvfig_list *ttvfig ;
     ttvfig_list *figpath ;
     ttvsig_list *ptsig ;
     chain_list *mask ;
     long delaymax ;
     long delaymin ;
     long type ;
     int addasync;
{
  ttvpath_list *path;

  ttv_reset_search_informations();
  if (addasync) ttv_search_mode(1, TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS);

  path = ttv_getaccessnocross (ttvfig, figpath, ptsig, mask, delaymax, delaymin, type);
  
  if (ttv_get_search_informations() & TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH)
    {
      ttv_freepathlist(path);
      path = ttv_getaccesssignocross (ttvfig, figpath, ptsig, mask, delaymax, delaymin, type);
      path=filter_parallel_to_node(path);
    }
  if (addasync) ttv_search_mode(0, TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS);
  return path;
}

chain_list *ttv_getendpoints(ttvfig_list *tvf, chain_list *sigl, long type)
{
  chain_list *sub, *cl, *lst, *ret=NULL;
  ttvevent_list *tve;
  ttvsig_list *sig;
  int maxperiod, savmpd;

  if ((type & TTV_FIND_DUAL)!=0) type|=TTV_FIND_CMD;

  maxperiod = V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE ;
  savmpd=TTV_MaxPathPeriodPrecharge;

  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = 0 ;
  TTV_MaxPathPeriodPrecharge=0;

  for (lst=sigl; lst!=NULL; lst=lst->NEXT)
    {
      sig=(ttvsig_list *)lst->DATA;
      sub = ttv_findpath(tvf,tvf,sig->NODE,NULL,type,tvf->INFO->LEVEL) ;
      for(cl= sub ; cl != NULL ; cl = cl->NEXT)
        {
          tve=(ttvevent_list *)cl->DATA;
          tve->FIND->TYPE &= ~TTV_FIND_BEGIN ;
          if ((tve->ROOT->TYPE & TTV_SIG_MARQUE)==0) ret=addchain(ret, tve->ROOT);
          tve->ROOT->TYPE|=TTV_SIG_MARQUE;
        }
      ttv_fifoclean();
      freechain(sub);
      sub = ttv_findpath(tvf,tvf,sig->NODE+1,NULL,type,tvf->INFO->LEVEL) ;
      for(cl= sub ; cl != NULL ; cl = cl->NEXT)
        {
          tve=(ttvevent_list *)cl->DATA;
          tve->FIND->TYPE &= ~TTV_FIND_BEGIN ;
          if ((tve->ROOT->TYPE & TTV_SIG_MARQUE)==0) ret=addchain(ret, tve->ROOT);
          tve->ROOT->TYPE|=TTV_SIG_MARQUE;
        }
      ttv_fifoclean();
      freechain(sub);
    }
  for (cl=ret; cl!=NULL; cl=cl->NEXT)
    ((ttvsig_list *)cl->DATA)->TYPE&=~TTV_SIG_MARQUE;
  V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = maxperiod ;
  TTV_MaxPathPeriodPrecharge=savmpd;
  return ret;
}

static ttvcritic_list *ttv_create_one_node_critic(ttvfig_list *ttvfig,ttvfig_list *ttvins, ttvevent_list *node, long type)
{
  ttvcritic_list *critic;
  long slope ;
  long newslope ;
  long start ;

  newslope = (long)ttv_getnodeslope(ttvfig,ttvins,node,&slope,type) ;
  start = ttv_getinittime(ttvfig,node,STB_NO_INDEX,type,STB_NO_INDEX,NULL) ;
  critic = ttv_alloccritic(NULL,ttvfig,NULL,node,type,0,start,slope,start,newslope,0,NULL,NULL) ;

  critic->NODE_FLAG|=TTV_NODE_FLAG_FIRST_OF_DETAIL;
  return(critic) ;
}

ttvpath_list *ttv_create_one_node_path(ttvfig_list *ttvfig,ttvfig_list *ttvins,ttvevent_list *node,long type)
{
  ttvfig_list *ttvfigx ;
  long startsref = 0, starts, start ;
  ttvpath_list *path;
  char phase;

  ttvfigx = node->ROOT->ROOT ;

  starts = ttv_getnodeslope(ttvfig,ttvins,node,&startsref,type) ;
  start = ttv_getinittime(ttvfig,node,STB_NO_INDEX,type,STB_NO_INDEX,&phase) ;
  path = ttv_allocpath(NULL,ttvfigx,node,node,NULL,
                       NULL,NULL,NULL,0,0,0,
                       type,0,startsref,
                       0,starts,
                       start,starts,NULL,NULL, TTV_NOTIME,phase,0) ;

  path->CRITIC = ttv_create_one_node_critic(ttvfig,ttvins,node,type) ;
  return(path) ;
}

