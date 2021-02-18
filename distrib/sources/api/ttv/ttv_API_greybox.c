#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include STM_H
#include TTV_H
#include MLU_H
#include MUT_H
#include INF_H
#include EFG_H
#include TAS_H
#include TRC_H
#include YAG_H
#include MCC_H
#include STB_H
#include BCK_H

#include AVT_H

#define API_USE_REAL_TYPES
#include "ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "ttv_API_util.h"

#define IFGNS(a) if (GENIUS_GLOBAL_LOFIG!=NULL) { a }

static ht      *ALL_TTVFIGS = NULL;

static long my_search_exclude_start=0, my_search_exclude_end=0;
static int sigall=0;

#define FLAG_NOSORT     1
#define FLAG_REALACCESS 2

//#define TTV_FIG_TYPE 0x1000

static long DETAIL_MODE=0, SEARCH_MODE=0;

static char statictempbuf[16][256];
static int statictempbufindex=0, assocdetail=1, ckonly=0;

long ttv_setsearchmode(long val)
{
 long old=SEARCH_MODE;
 SEARCH_MODE=val;
 return old;
}
void ttv_setsearchexclude(long valstart, long valend, long *oldvalstart, long *oldvalend)
{
  *oldvalstart=my_search_exclude_start;
  *oldvalend=my_search_exclude_end;
  if (valend!=-1) my_search_exclude_end=valend;
  if (valstart!=-1) my_search_exclude_start=valstart;
}

static int ttv_checkenabledualmode(chain_list *in, chain_list *out)
{
  int cnt1=0, cnt2=0;
  if (out==NULL) cnt2=INT_MAX;
  else
    while (out!=NULL)
    {
      if (mbk_isregex_name((char *)out->DATA)) cnt2++; 
      out=out->NEXT;
    }
  if (in==NULL) cnt1=INT_MAX;
  else
    while (in!=NULL)
    {
      if (mbk_isregex_name((char *)in->DATA)) cnt1++; 
      in=in->NEXT;
    }
  if (cnt1<cnt2) return 1;
  return 0;
}
void ttv_SearchExcludeNodeType(char *conf)
{
  long val=0;
  char *c, *tok;
  char buf[1024];
  int start=0, end=0;

  strcpy(buf, conf);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"command")==0) val|=TTV_SIG_Q;
      else if (strcasecmp(tok,"connector")==0) val|=TTV_SIG_C;
      else if (strcasecmp(tok,"breakpoint")==0) val|=TTV_SIG_B;
      else if (strcasecmp(tok,"precharge")==0) val|=TTV_SIG_R;
      else if (strcasecmp(tok,"latch")==0) val|=TTV_SIG_LL|TTV_SIG_LF|TTV_SIG_LR|TTV_SIG_LS|TTV_SIG_LSL;
      else if (strcasecmp(tok,"start")==0) start=1;
      else if (strcasecmp(tok,"end")==0) end=1;
      else 
        avt_errmsg(TTV_API_ERRMSG, "011", AVT_ERROR, tok);

      tok=strtok_r(NULL, " ", &c);
    }
  if (end || (!end && !start)) my_search_exclude_end=val;
  if (start || (!end && !start)) my_search_exclude_start=val;
}

chain_list *ttv_GetMatchingSignal(ttvfig_list *tvf, char *name, char *type)
{
  chain_list *ret, *cl=addchain(NULL, name);
  char *tok, *c, buf[1024];
  long val=0;

  strcpy(buf, type);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"command")==0) val|=TTV_SIG_Q;
      else if (strcasecmp(tok,"connector")==0) val|=TTV_SIG_C;
      else if (strcasecmp(tok,"breakpoint")==0) val|=TTV_SIG_B;
      else if (strcasecmp(tok,"precharge")==0) val|=TTV_SIG_R;
      else if (strcasecmp(tok,"latch")==0) val|=TTV_SIG_L;
      else if (strcasecmp(tok,"any")==0) val|=TTV_SIG_TYPEALL;
      else 
        avt_errmsg(TTV_API_ERRMSG, "012", AVT_ERROR, tok);
      
      tok=strtok_r(NULL, " ", &c);
    }
  if (val==0) val=TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R|TTV_SIG_Q;

  ret=ttv_getsigbytype_and_netname(tvf,NULL,val,cl);
  freechain(cl);
  return ret;
}
/* return 1 if two critic are identical, 0 else */
static int CompareDetail( ttvcritic_list *critic1, ttvcritic_list *critic2 )
{
  if( critic1->FIG != critic2->FIG ) return 0 ;
  if( critic1->DELAY != critic2->DELAY ) return 0 ;
  if( critic1->SLOPE != critic2->SLOPE ) return 0 ;
  if( critic1->SNODE != critic2->SNODE ) return 0 ;
  if( strcmp( critic1->NAME, critic2->NAME ) ) return 0 ;
  if( strcmp( critic1->NETNAME, critic2->NETNAME ) ) return 0 ;

  return 1 ;
}

static char *getstatictempbuf()
{
  char *c;
  c=statictempbuf[statictempbufindex];
  statictempbufindex++;
  if (statictempbufindex>=16) statictempbufindex=0;
  return c;
}


static char *
vecname(char *name)
{
    return mbk_vect(mbk_decodeanyvector(name), '[', ']');
}

static chain_list *split_mask(char *mask)
{
  char *buf;
  char *start, *mark;
  chain_list *cl=NULL;

  if (mask==NULL) return NULL;
//  if (mask[0]=='\0') return addchain(NULL, namealloc("-"));
  buf=(char *)mbkalloc(strlen(mask)+10);
  strcpy(buf, mask);

  start=strtok_r(buf, " ", &mark);
  while (start!=NULL)
    {
      cl=addchain(cl, vecname(start));
      start=strtok_r(NULL, " ", &mark);
    }
  mbkfree(buf);
  return reverse(cl);
}

static ttvfig_list *getattvfig(ttvsig_list *a, ttvsig_list *b)
{
  if (a!=NULL) return ttv_GetSignalTopTimingFigure(a);
  if (b!=NULL) return ttv_GetSignalTopTimingFigure(b);
  return NULL;
}


chain_list* ttv_RemoveDuplicatedPath( chain_list *headpathlist ) 
{
  NameAllocator   na ;
  ht             *same ;
  ttvpath_list   *path ;
  char           *name ;
  chain_list     *scan ;
  chain_list     *scanpath ;
  chain_list     *headsamelist ;
  chain_list     *chainlistpath ;
  chain_list     *returnedpathlist ;
  chain_list     *scannextpath ;
  ttvpath_list   *nextpath ;
  chain_list *pathdetail ;
  chain_list *scandetail ;
  chain_list *nextpathdetail ;
  chain_list *nextscandetail ;
  char            buf[2000] ;
  char            dummy ;

  CreateNameAllocator( 1000, &na, 'y' );
  
  same = addht( 1000 );
  
  for( scanpath = headpathlist ; scanpath ; scanpath = scanpath->NEXT ) {
  
    path = (ttvpath_list*)scanpath->DATA ;
    
    sprintf( buf, "%p,%p,%p,%ld,%ld", path->NODE, path->ROOT, path->LATCH, path->DELAY, path->SLOPE );
    name = NameAlloc( &na, buf );

    chainlistpath = (chain_list*)gethtitem( same, name );
    if( chainlistpath == (chain_list*)EMPTYHT )
      chainlistpath = NULL ;
    chainlistpath = addchain( chainlistpath, path );
    addhtitem( same, name, (long)chainlistpath );
  }
 
  headsamelist = GetAllHTElems( same );

  for( scan = headsamelist ; scan ; scan = scan->NEXT ) {
  
    chainlistpath = (chain_list*)scan->DATA ;

    if( chainlistpath ) {
    
      if( chainlistpath->NEXT ) {
        
        for( scanpath = chainlistpath ; scanpath ; scanpath = scanpath->NEXT ) {
        
          path = (ttvpath_list*)scanpath->DATA ;
          pathdetail = ttv_GetPathDetail( path );
          
          for( scannextpath = scanpath->NEXT ; scannextpath ; scannextpath = scannextpath->NEXT ) {
          
            nextpath = (ttvpath_list*)scannextpath->DATA ;
            nextpathdetail = ttv_GetPathDetail( nextpath );
            
            for( scandetail = pathdetail, nextscandetail = nextpathdetail ;
                 scandetail && nextscandetail ;
                 scandetail = scandetail->NEXT, nextscandetail = nextscandetail->NEXT ) {

              if( !CompareDetail( (ttvcritic_list*)scandetail->DATA, (ttvcritic_list*)nextscandetail->DATA ) )
                break ;
            }

            freechain( nextpathdetail );

            if( !scandetail && !nextscandetail )
              break ;
          }

          freechain( pathdetail );
          if( scannextpath )
            path->NEXT = (ttvpath_list*)&dummy ;
        }
      }
    }
    
    freechain( chainlistpath );
  }

  returnedpathlist = NULL ;
  for( scanpath = headpathlist ; scanpath ; scanpath = scanpath->NEXT ) {
    path = (ttvpath_list*)scanpath->DATA ;
    if( path->NEXT != (ttvpath_list*)&dummy ) {
      returnedpathlist = addchain( returnedpathlist, path );
    }
    else {
      path->NEXT = NULL ;
      ttv_freepathlist( path );
    }
  }
  
  freechain( headsamelist );
  delht( same );
  DeleteNameAllocator(&na);
  
  return reverse( returnedpathlist );
}

static void AssociateDetailIfNeeded(ttvpath_list *pth)
{
  NameAllocator na;
  ht *same;
  char buf[2048], *name;
  long l;
  chain_list *lst, *elems, *cl, *ch, *detail, *rundetail/*, *detailchain*/;
  ttvpath_list *ta;
  ttvcritic_list *tc, *prev;
  ht *diffcmd;
  chain_list *cl0;

  if (pth!=NULL && ((pth->TYPE & TTV_FIND_ALL)==0 || pth->CRITIC!=NULL)) return;

  CreateNameAllocator(1000, &na, 'y');
  same=addht(10000);
  for (; pth; pth = pth->NEXT) 
    {
      if ((pth->TYPE & TTV_FIND_ALL)!=0) // on peut avoir des melanges FINDALL & CRITIC
        {
          sprintf(buf, "%p,%p,%p,%ld", pth->NODE, pth->ROOT, pth->LATCH, pth->DELAY);
          name=NameAlloc(&na, buf);
          if ((l=gethtitem(same, name))==EMPTYHT) lst=NULL;
          else lst=(chain_list *)l;
          lst=addchain(lst, pth);
          addhtitem(same, name, (long)lst);
        }
    }
  
  elems=GetAllHTElems(same);
  for (cl=elems; cl!=NULL; cl=cl->NEXT)
    {
      chain_list *tch;
      lst=(chain_list *)cl->DATA;
      if (lst->NEXT!=NULL) // au moins 2 path avec le meme delai
        {
          detail=NULL;
          diffcmd=NULL;
          for (ch=lst; ch!=NULL; ch=ch->NEXT)
            {
              ta=(ttvpath_list *)ch->DATA;
              if (ch==lst) 
                {
                  detail=rundetail=ttv_getcriticpara(ttv_GetTopTimingFigure(ta->FIG), ta->FIG, ta->ROOT, ta->NODE, ta->DELAY, (ta->TYPE & ~(TTV_FIND_PATH))|TTV_FIND_LINE, NULL, ta->TTV_MORE_SEARCH_OPTIONS);
                  if (ta->LATCH)
                    {
                      diffcmd=addht(120);
                      for (cl0=rundetail; cl0!=NULL; cl0=cl0->NEXT)
                        {
                          for (tc=(ttvcritic_list *)cl0->DATA, prev=NULL; tc!=NULL && tc->NODE!=ta->LATCH; prev=tc, tc=tc->NEXT) ;
                          if ((l=gethtitem(diffcmd, prev->NODE))==EMPTYHT) tch=NULL;
                          else tch=(chain_list *)l;
                          addhtitem(diffcmd, prev->NODE, (long)addchain(tch, cl0->DATA));
                        }
                    }
                }
              if (rundetail==NULL) break; // erreur?
              
              if (diffcmd)
                {
                  if ((l=gethtitem(diffcmd, ta->CMDLATCH))==EMPTYHT || (chain_list *)l==NULL) break; // erreur
                  tch=(chain_list *)l;
                  ta->CRITIC=(ttvcritic_list *)tch->DATA;
                  tch=delchain(tch, tch);
                  addhtitem(diffcmd, ta->CMDLATCH, (long)tch);
                }
              else
                ta->CRITIC=(ttvcritic_list *)rundetail->DATA;
              rundetail=rundetail->NEXT;
            }
          if (ta->LATCH) delht(diffcmd);
          freechain(detail);
        }
      freechain(lst);
    }
  freechain(elems);
  delht(same);
  DeleteNameAllocator(&na);
}

static chain_list *
pathchain(ttvpath_list *pth)
{
    chain_list     *reschain = NULL;

    if (assocdetail) AssociateDetailIfNeeded(pth);

    for (; pth; pth = pth->NEXT) {
        reschain = addchain(reschain, pth);
    }
    return reverse(reschain);
}

static ttvpath_list *
ttv_GetPathBySignal( ttvfig_list *tf, ttvsig_list *source, ttvsig_list *dest, int a, int b, int minmax)
{
    ttvpath_list   *pth;
    chain_list      mask;
    chain_list     *a_mask;
    ttvsig_list    *sig;
    long            type = TTV_FIND_NOTCLASS|DETAIL_MODE;
    long old0, old1;

    if (tf==NULL) return NULL;

//    ttv_checkinfchange(tf);

    if ((ttv_getloadedfigtypes(tf) & TTV_FILE_DTX)==TTV_FILE_DTX)
      type |= TTV_FIND_LINE;
    else
      type |= TTV_FIND_PATH;

    if (!source && dest) {
        a_mask = NULL;
        type |= minmax | TTV_FIND_SIG | TTV_FIND_CARAC;
        sig = dest;
        if (b == TTVAPI_UP)
            type |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPDW;
        if (b == TTVAPI_DN)
            type |= TTV_FIND_NOT_DWUP | TTV_FIND_NOT_UPUP;
    }
    else if (source && !dest) {
        a_mask = NULL;
        type |= minmax | TTV_FIND_SIG | TTV_FIND_DUAL | TTV_FIND_CARAC;
        sig = source;
        if (a == TTVAPI_UP)
            type |= TTV_FIND_NOT_DWUP | TTV_FIND_NOT_DWDW;
        if (a == TTVAPI_DN)
            type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_UPUP;
    }
    else if (source && dest) {
        mask.DATA = source;
        mask.NEXT = NULL;
        a_mask = &mask;
        type |= minmax | TTV_FIND_SIG | TTV_FIND_CARAC;
        sig = dest;
        if (a == TTVAPI_UP)
            type |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_DWUP;
        else if (a == TTVAPI_DN)
            type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW;
        if (b == TTVAPI_UP)
            type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
        else if (b == TTVAPI_DN)
            type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP;
    }
    else return NULL;

    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);
    if (sigall==0)
      pth = ttv_getpath(tf, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type);
    else
      pth = ttv_getpathsig(tf, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type);
    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);
    return pth;
}

//------------------
void
ttv_TerminateRun()
{
    ttvfig_list    *tvf;
    timing_cell    *cell;

    if (ALL_TTVFIGS != NULL) {
        chain_list     *cl, *ch;

        ch = cl = GetAllHTElems(ALL_TTVFIGS);
        while (cl != NULL) {
            tvf = (ttvfig_list *) cl->DATA;
            ttv_unlockttvfig(tvf);

            if ((cell = stm_getcell(tvf->INFO->FIGNAME)) != NULL)
                stm_freecell(tvf->INFO->FIGNAME);

            ttv_freeallttvfig(tvf);

            cl = cl->NEXT;
        }
        freechain(ch);
        delht(ALL_TTVFIGS);
        ALL_TTVFIGS = NULL;
    }
}

static int      initfirsttime = 0;

void
ttv_InitializeNewRun()
{
    if (initfirsttime == 0) {
        initfirsttime = 1;
        avtenv();
        mbkenv();
        elpenv();
        mccenv();
        stmenv();
        rcx_env();
    }
    ttv_TerminateRun();
    ALL_TTVFIGS = addht(64);
}


void
CHECK_INIT_CALL()
{
    if (ALL_TTVFIGS == NULL) {
      ttv_InitializeNewRun();
    }
    ttvenv();
}

static void
ttvgetparams(char *username, char *filename, float *temper, float *alim)
{
    char            temp[1024], *c, *end;
    float           t;

    strcpy(temp, username);
    *temper = 0;
    *alim = 0;
    //temperature
        c = strrchr(temp, '_');
    if (c == NULL) {
        strcpy(filename, temp);
        return;
    }
    t = strtod(&c[1], &end);
    if (*end == '\0')
        *temper = t;
    else {
        strcpy(filename, temp);
        return;
    }
    *c = '\0';
    //alimentation
        c = strrchr(temp, '_');
    if (c == NULL) {
        strcpy(filename, temp);
        return;
    }
    t = strtod(&c[1], &end);
    if (*end == '\0')
        *alim = t;
    else {
        strcpy(filename, temp);
        return;
    }
    *c = '\0';
    strcpy(filename, temp);
}

static void
ttvaddfigure(char *name, float temp, float alim, ttvfig_list * tf)
{
    char            nm[1024];

    sprintf(nm, "%s|temp=%.3g|alim=%.3g", name, temp, alim);
    addhtitem(ALL_TTVFIGS, namealloc(nm), (long) tf);
}

static ttvfig_list *
ttvgetfigure(char *name, float temp, float alim)
{
    char            nm[1024];
    long            l;

    CHECK_INIT_CALL();

    if (temp<0 && alim<0)
      return ttv_getttvfig( name, TTV_FILE_DTX|TTV_FILE_TTX);

    sprintf(nm, "%s|temp=%.3g|alim=%.3g", name, temp, alim);
    if ((l = gethtitem(ALL_TTVFIGS, namealloc(nm))) == EMPTYHT)
        return NULL;
    return (ttvfig_list *) l;
}

static chain_list *
getfilelist()
{
    DIR            *dp;
    struct dirent  *ep;
    chain_list     *cl = NULL;

    dp = opendir("./");
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL)
            cl = addchain(cl, strdup(ep->d_name));
        closedir(dp);
    }
    else
      avt_errmsg(TTV_API_ERRMSG, "013", AVT_ERROR);

    return cl;
}

static int samefloat(float v1, float v2)
{
  if (fabs(v1-v2)<1e-5) return 1;
  return 0;
}
ttvfig_list *
ttv_LoadSpecifiedTimingFigure(char *name)
{
  ttvfig_list *tf;
  inffig_list *ifl;
  
  CHECK_INIT_CALL();

  ifl=getloadedinffig(name);
  tf = ttv_getttvfig( name, TTV_FILE_DTX|TTV_FILE_INF);
  if( tf==NULL )
     avt_errmsg(TTV_API_ERRMSG, "014", AVT_ERROR, name);
  else
  {
    if (!samefloat(tf->INFO->TEMP,V_FLOAT_TAB[__SIM_TEMP].VALUE))
    {
      MCC_TEMP = tf->INFO->TEMP;
      V_FLOAT_TAB[__SIM_TEMP].VALUE=tf->INFO->TEMP;
      elpGeneral[elpTEMP] = tf->INFO->TEMP ;
      if (V_FLOAT_TAB[__SIM_TEMP].SET)
        avt_errmsg(TTV_API_ERRMSG, "045", AVT_WARNING, "simTemperature", V_FLOAT_TAB[__SIM_TEMP].VALUE);
    }
    if (tf->INFO->TNOM>-500 && !samefloat(tf->INFO->TNOM,V_FLOAT_TAB[__SIM_TNOM].VALUE))
    {
      V_FLOAT_TAB[__SIM_TNOM].VALUE=tf->INFO->TNOM;
      if (V_FLOAT_TAB[__SIM_TNOM].SET)
        avt_errmsg(TTV_API_ERRMSG, "045", AVT_WARNING, "simNominalTemperature", V_FLOAT_TAB[__SIM_TNOM].VALUE);
    }

    if (!samefloat(tf->INFO->VDD,V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE))
    {
      V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE=tf->INFO->VDD;
      elpGeneral[elpGVDDMAX] = tf->INFO->VDD ;
      MCC_VDDmax = tf->INFO->VDD ;
      if (V_FLOAT_TAB[__SIM_POWER_SUPPLY].SET)
        avt_errmsg(TTV_API_ERRMSG, "045", AVT_WARNING, "simPowerSupply", V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
    }
    if (!samefloat(tf->INFO->DTH,SIM_VTH) || !samefloat(tf->INFO->STHLOW,SIM_VTH_LOW) || !samefloat(tf->INFO->STHHIGH,SIM_VTH_HIGH))
    {
      V_FLOAT_TAB[__SIM_VTH].VALUE=SIM_VTH=MCC_VTH=tf->INFO->DTH;
      V_FLOAT_TAB[__SIM_VTH_LOW].VALUE=SIM_VTH_LOW=STM_DEFAULT_SMINR=STM_DEFAULT_SMINF=tf->INFO->STHLOW;
      V_FLOAT_TAB[__SIM_VTH_HIGH].VALUE=SIM_VTH_HIGH=STM_DEFAULT_SMAXR=STM_DEFAULT_SMAXF=tf->INFO->STHHIGH;
      avt_errmsg(TTV_API_ERRMSG, "046", AVT_WARNING);
    }
  }
  if (tf!=NULL && ifl!=NULL && (ifl->changed & INF_CHANGED_RECOMP)!=0)
    {
      if (ttv_check_update_slope_and_output_capa(tf))
      {
        avt_error("ttvapi", -1, AVT_INFO, "new configuration may affect UTD '%s', recomputing values...\n",name);
        ttv_RecomputeDelays(tf);
      }
      ifl->changed&=~INF_CHANGED_RECOMP;
    }



  return tf ;
}

int ttv_LoadCrosstalkFile(ttvfig_list *tvf)
{
  if( tvf==NULL )
     avt_errmsg(TTV_API_ERRMSG, "015", AVT_ERROR);
  return ttv_ctxparse(tvf);
}


ttvfig_list *
ttv_LoadTimingFigure(char *name)
{
    char            name0[1024];
    float           alim, temp;
    ttvfig_list    *tf = NULL;
    chain_list     *cl, *ch;
    char           *c, *n;

    CHECK_INIT_CALL();

    ch = cl = getfilelist();
    while (cl != NULL) {
      n = (char *) cl->DATA;
      if ((c = strrchr(n, '.')) != NULL && (strcmp(c, ".dtx") == 0 || strcmp(c, ".ttx") == 0) && strncmp(n, name, strlen(name)) == 0) {
        *c = '\0';
        ttvgetparams(n, name0, &temp, &alim);
        
        if ((tf = ttvgetfigure(name0, temp, alim)) != NULL)
          {
          }
        else {
          tf = ttv_getttvfig(n, TTV_FILE_INF);
          if (tf->INFO->TEMP>-1000.0) temp=tf->INFO->TEMP;
          if (tf->INFO->VDD>-1.0) alim=tf->INFO->VDD;
          ttvaddfigure(name0, temp, alim, tf);
        }
      }
      free(cl->DATA);
      cl = cl->NEXT;
    }
    freechain(ch);
    if (tf == NULL)
       avt_errmsg(TTV_API_ERRMSG, "014", AVT_ERROR, name);
    return tf;
}

ttvfig_list *
ttv_LoadSpecifiedTimingPathFigure(char *name)
{
  ttvfig_list *tf;
  
  CHECK_INIT_CALL();

  tf = ttv_getttvfig( name, TTV_FILE_TTX|TTV_FILE_INF);
  if( tf==NULL )
    avt_errmsg(TTV_API_ERRMSG, "014", AVT_ERROR, name);
  return tf ;
}

ttvfig_list *
ttv_LoadTimingPathFigure(char *name)
{
    char            name0[1024];
    float           alim, temp;
    ttvfig_list    *tf = NULL;
    chain_list     *cl, *ch;
    char           *c, *n;

    CHECK_INIT_CALL();

    ch = cl = getfilelist();
    while (cl != NULL) {
        n = (char *) cl->DATA;
        if ((c = strrchr(n, '.')) != NULL && (strcmp(c, ".dtx") == 0 || strcmp(c, ".ttx") == 0) && strncmp(n, name, strlen(name)) == 0) {
            *c = '\0';
            ttvgetparams(n, name0, &temp, &alim);

            if ((tf = ttvgetfigure(name0, temp, alim)) != NULL)
                avt_errmsg(TTV_API_ERRMSG, "016", AVT_WARNING, n);
            else {
                tf = ttv_getttvfig(n, TTV_FILE_INF);
                ttvaddfigure(name0, temp, alim, tf);
            }
        }
        free(cl->DATA);
        cl = cl->NEXT;
    }
    freechain(ch);
    if (tf == NULL)
      avt_errmsg(TTV_API_ERRMSG, "014", AVT_ERROR, name);
    return tf;
}

ttvfig_list    *
ttv_GetTimingFigure(char *name, float temp, float alim)
{
  ttvfig_list    *tf;
  
  CHECK_INIT_CALL();
  
  tf = ttvgetfigure(name, temp, alim);
  if (tf == NULL)
  {
    if (temp<0 && alim<0)
      avt_errmsg(TTV_API_ERRMSG, "052", AVT_ERROR, name);
    else
      avt_errmsg(TTV_API_ERRMSG, "017", AVT_ERROR, name, temp, alim);
  }
  return tf;
}

double ttv_GetTimingFigureTemperature(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->TEMP;
}

double ttv_GetTimingFigureSupply(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->VDD;
}

double ttv_GetTimingFigureOutputCapacitance(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->CAPAOUT*1e-15;
}

double ttv_GetTimingFigureInputSlope(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->SLOPE*1e-12/TTV_UNIT;
}

double ttv_GetTimingFigureLowThreshold(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->STHLOW;
}

double ttv_GetTimingFigureHighThreshold(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->STHHIGH;
}

double ttv_GetTimingFigureThreshold(ttvfig_list *tf)
{
  if (tf==NULL) return 0;
  return tf->INFO->DTH;
}

char *ttv_GetTimingFigureTechno(ttvfig_list *tf)
{
  char buf[1024];
  if (tf==NULL) return "";
  sprintf(buf, "%s v%s", tf->INFO->TECHNONAME, tf->INFO->TECHNOVERSION);
  return namealloc(buf);
}

char *ttv_GetTimingFigureGenerationDate(ttvfig_list *tf)
{
  char buf[1024];
  if (tf==NULL) return "";
  sprintf(buf, "%02d/%2d/%02d", tf->INFO->TTVYEAR, tf->INFO->TTVMONTH, tf->INFO->TTVDAY);
  return namealloc(buf);
}

char *ttv_GetTimingFigureGenerationTime(ttvfig_list *tf)
{
  char buf[1024];
  if (tf==NULL) return "";
  sprintf(buf, "%dh%dm%ds", tf->INFO->TTVHOUR, tf->INFO->TTVMIN, tf->INFO->TTVSEC);
  return namealloc(buf);
}

static ttvpath_list *
ttv_GetPathByName(ttvfig_list * tf, char *start, char *end, char *dir, int minmax)
{
    int             a, b;
    ttvsig_list    *startsig, *endsig;

//    ttv_checkinfchange(tf);
    ttv_DirectionStringToIndices(dir, &a, &b);
    if (a == -1 || b == -1) {
      avt_errmsg(TTV_API_ERRMSG, "018", AVT_ERROR, dir);
      return NULL;
    }
    startsig = ttv_getsig(tf, vecname(start));
    if (startsig == NULL) {
        avt_errmsg(TTV_API_ERRMSG, "019", AVT_ERROR, start);
        return NULL;
    }
    endsig = ttv_getsig(tf, vecname(end));
    if (endsig == NULL) {
        avt_errmsg(TTV_API_ERRMSG, "019", AVT_ERROR, end);
        return NULL;
    }
    return ttv_GetPathBySignal(tf, startsig, endsig, a, b, minmax);
}


static ttvpath_list *
ttv_GetPathByConditions(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature, int minmax, ttvfig_list ** tvf)
{
    ttvfig_list    *tv;
    ttvpath_list   *pth;

    tv = ttvgetfigure(figure, temperature, VDD);
    *tvf = tv;

    if (tv == NULL) {
        avt_errmsg(TTV_API_ERRMSG, "017", AVT_ERROR, figure, temperature, VDD);
        return NULL;
    }

//    ttv_checkinfchange(tvf);
    pth = ttv_GetPathByName(tv, namealloc(start), namealloc(end), dir, minmax);
    if (pth == NULL) {
        avt_errmsg(TTV_API_ERRMSG, "020", AVT_ERROR, (minmax &TTV_FIND_MAX)==TTV_FIND_MAX?"max":"min",start, end, dir);
      return NULL;
    }

    return pth;
}

static double
INSLOPE(double inslope, ttvpath_list * pth, timing_model * tm)
{
    if (tm == NULL) {
        return inslope * 1e12;
    }
    else {
        return elpThr2Scm(inslope * 1e12, SIM_VTH_LOW, SIM_VTH_HIGH, stm_mod_vt(tm), ttv_GetPathStartDirection(pth) == 'u' ? pth->FIG->INFO->VDD : stm_mod_default_vfd(), pth->FIG->INFO->VDD, ttv_GetPathStartDirection(pth) == 'u' ? elpRISE : elpFALL);
    }
}

ttvpath_list *
ttv_getcritic_pathlist (ttvfig_list *tf, void *start, void *end, char *dir, long number, int minmax, int no_class, int names, int all)
{
    int             a, b, i;
    chain_list     *mask_in=NULL, *mask_out=NULL;
    ttvpath_list   *path_list, *path;
    long            type, old0, old1;
    chain_list      mask;
    chain_list     *a_mask;
    ttvsig_list *sig;
    int forced;
            
    if (tf==NULL) return NULL;

    if (number<=0) number=INT_MAX;

    ttv_DirectionStringToIndices(dir, &a, &b);
    if (a == -1 || b == -1) {
      avt_errmsg(TTV_API_ERRMSG, "018", AVT_ERROR, dir);
      return NULL;
    }
    forced=ttv_DirectionStringToIndicesHZ(dir);
    if (forced==TTV_API_ONLY_HZ) ttv_search_mode(1, TTV_MORE_OPTIONS_ONLYHZ);
    else if (forced==TTV_API_ONLY_NOTHZ) ttv_search_mode(1, TTV_MORE_OPTIONS_ONLYNOTHZ);
    
    if ((ttv_getloadedfigtypes(tf) & TTV_FILE_DTX)==TTV_FILE_DTX)
      type = minmax | TTV_FIND_LINE|DETAIL_MODE;
    else
      {
        // here
        if (all)
          {
            avt_errmsg(TTV_API_ERRMSG, "021", AVT_ERROR,tf->INFO->FIGNAME);
            return NULL;
          }
        type = minmax | TTV_FIND_PATH|DETAIL_MODE;
      }
    if (no_class != 0) type |= TTV_FIND_NOTCLASS;

    if (a == TTVAPI_UP)
        type |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_DWUP;
    else if (a == TTVAPI_DN)
        type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW;
    if (b == TTVAPI_UP)
        type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
    else if (b == TTVAPI_DN)
        type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP;

    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);

    type|=SEARCH_MODE;
    if (SEARCH_MODE & TTV_FIND_DUAL) type|=TTV_FIND_CMD;

    if (names)
      {
        if (start==NULL || strcmp((char *)start, "*") != 0) mask_in = split_mask((char *)start); //addchain(NULL, start);
        if (end==NULL || strcmp((char *)end, "*") != 0) mask_out = split_mask((char *)end); //addchain(NULL, end);

        if (ttv_checkenabledualmode(mask_in, mask_out)) type |= TTV_FIND_DUAL|TTV_FIND_CMD;

        if (!all)
          path_list = ttv_getcriticpath(tf, NULL, mask_in, mask_out, TTV_DELAY_MAX, TTV_DELAY_MIN, (int)number, type);
        else
            path_list = ttv_getallpath(tf, NULL, mask_in, mask_out, TTV_DELAY_MAX, TTV_DELAY_MIN, (int)number, type);
        freechain(mask_in);
        freechain(mask_out);
      }
    else
      {
        type |= TTV_FIND_SIG;
        if (!start && end) { a_mask = NULL; sig = end; }
        else if (start && !end) { a_mask = NULL; type |= TTV_FIND_DUAL|TTV_FIND_CMD; sig = start; }
        else 
        {
          if ((type & TTV_FIND_DUAL)!=TTV_FIND_DUAL)
          {
             mask.DATA = start; mask.NEXT = NULL; a_mask = &mask; sig = end; 
          }
          else
          {
             mask.DATA = end; mask.NEXT = NULL; a_mask = &mask; sig = start; 
          }
        }

        if (!all)
          path_list = ttv_getpath(tf, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type);
        else
          path_list = ttv_getpathsig(tf, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type);

        for(path=path_list, i=1; i<number && path != NULL ; path = path->NEXT , i++) ;
        if(path != NULL)
          {
            ttv_freepathlist(path->NEXT) ;
            path->NEXT = NULL ;
          }

      }
    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);
    ttv_search_mode(0, TTV_MORE_OPTIONS_ONLYHZ|TTV_MORE_OPTIONS_ONLYNOTHZ);
    return path_list;
}

/*****************************************************************/
static ttvpath_list *
ttv_GetAccessList (ttvfig_list *tf, void *start, void *end, char *clk, char *dir, int number, int minmax, int no_class, int names, int all)
{
    int             a, b;
    chain_list      *mask_out=NULL, *mask_clk=NULL, *mask_in=NULL;
    ttvpath_list   *path_list;
    long            type, old0, old1;
    ttvsig_list *sig;
    chain_list      mask;
    int forced;

    if (tf==NULL) return NULL;

    if (number<=0) number=INT_MAX;

    ttv_DirectionStringToIndices(dir, &a, &b);
    if (a == -1 || b == -1) {
      avt_errmsg(TTV_API_ERRMSG, "018", AVT_ERROR, dir);
      return NULL;
    }
    forced=ttv_DirectionStringToIndicesHZ(dir);
    if (forced==TTV_API_ONLY_HZ) ttv_search_mode(1, TTV_MORE_OPTIONS_ONLYHZ);
    else if (forced==TTV_API_ONLY_NOTHZ) ttv_search_mode(1, TTV_MORE_OPTIONS_ONLYNOTHZ);

    if ((ttv_getloadedfigtypes(tf) & TTV_FILE_DTX)==TTV_FILE_DTX)
      type = minmax | TTV_FIND_LINE|DETAIL_MODE;
    else
      {
        if (all)
          {
            avt_errmsg(TTV_API_ERRMSG, "021", AVT_ERROR,tf->INFO->FIGNAME);
            return NULL;
          }
        type = minmax | TTV_FIND_PATH|DETAIL_MODE;
      }
    type|=(SEARCH_MODE & ~TTV_FIND_DUAL)/*|TTV_FIND_CMD*/; // pas de mode dual en access pour l'instant
    // TTV_FIND_CMD

    if ((no_class & FLAG_NOSORT)!=0) type |= TTV_FIND_NOTCLASS;

    if (a == TTVAPI_UP)
        type |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_DWUP;
    else if (a == TTVAPI_DN)
        type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW;
    if (b == TTVAPI_UP)
        type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
    else if (b == TTVAPI_DN)
        type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP;

    if (clk!=NULL)
      {
        if (strcmp(clk, "*") != 0) mask_clk = split_mask(clk);
        else mask_clk = ttv_getclocklist(tf);
      }
    
    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);

    if (no_class & FLAG_REALACCESS) ttv_activate_real_access_mode(1);

    if (names)
      {
        mask_in = split_mask((char *)start);
        mask_out = split_mask((char *)end);
        if (!all)
          path_list = ttv_getcriticaccess(tf, NULL, mask_in, mask_out, mask_clk, TTV_DELAY_MAX, TTV_DELAY_MIN, (int)number, type);
        else
          path_list = ttv_getallaccess(tf, NULL, mask_in, mask_out, mask_clk, TTV_DELAY_MAX, TTV_DELAY_MIN, (int)number, type);
        freechain(mask_in);
        freechain(mask_out);
        freechain(mask_clk);
      }
    else
      {
        type |= TTV_FIND_SIG;
        if (!start && end) { mask_out = NULL; sig = end; }
        // -->> le cas ci-dessous ne devrait pas arriver <<--
        else if (start && !end) { mask_out = NULL; type |= TTV_FIND_DUAL|TTV_FIND_CMD; sig = start; }
        // --
        else { mask.DATA = start; mask.NEXT = NULL; mask_out = &mask; sig = end; }

        if (mask_clk!=NULL) 
          {
            chain_list *clklist;
            clklist=ttv_getsigbytype(tf,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B,mask_clk) ;
            freechain(mask_clk);
            mask_clk = clklist;
          }

        if (!all)
          path_list = ttv_getaccess(tf, NULL, sig, mask_out, mask_clk, TTV_DELAY_MAX, TTV_DELAY_MIN, type);
        else
          path_list = ttv_getsigaccess(tf, NULL, sig, mask_out, mask_clk, TTV_DELAY_MAX, TTV_DELAY_MIN, type);
        freechain(mask_clk);
      }

    if (no_class & FLAG_REALACCESS) ttv_activate_real_access_mode(0);
    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);
    ttv_search_mode(0, TTV_MORE_OPTIONS_ONLYHZ|TTV_MORE_OPTIONS_ONLYNOTHZ);

    return path_list;
}

ttvpath_list * ttv_getsigaccesslist (ttvfig_list *tf, void *start, void *end, void *clk, char *dir, int number, int minmax, int no_class, int all)
{
    int             a, b;
    chain_list      *mask_out=NULL, *mask_clk=NULL;
    ttvpath_list   *path_list;
    long            type, old0, old1;
    ttvsig_list *sig;
    chain_list      mask;

    if (tf==NULL) return NULL;

    if (number<=0) number=INT_MAX;

    ttv_DirectionStringToIndices(dir, &a, &b);
    if (a == -1 || b == -1) {
      avt_errmsg(TTV_API_ERRMSG, "018", AVT_ERROR, dir);
      return NULL;
    }

    if ((ttv_getloadedfigtypes(tf) & TTV_FILE_DTX)==TTV_FILE_DTX)
      type = minmax | TTV_FIND_LINE|DETAIL_MODE;
    else
      {
        if (all)
          {
            avt_errmsg(TTV_API_ERRMSG, "021", AVT_ERROR,tf->INFO->FIGNAME);
            return NULL;
          }
        type = minmax | TTV_FIND_PATH|DETAIL_MODE;
      }
    type|=(SEARCH_MODE & ~TTV_FIND_DUAL)/*|TTV_FIND_CMD*/; // pas de mode dual en access pour l'instant

    if ((no_class & FLAG_NOSORT)!=0) type |= TTV_FIND_NOTCLASS;

    if (a == TTVAPI_UP)
        type |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_DWUP;
    else if (a == TTVAPI_DN)
        type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW;
    if (b == TTVAPI_UP)
        type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
    else if (b == TTVAPI_DN)
        type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP;

    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);

    type |= TTV_FIND_SIG;
    if (!start && end) { mask_out = NULL; sig = end; }
    // -->> le cas ci-dessous ne devrait pas arriver <<--
    else if (start && !end) { mask_out = NULL; type |= TTV_FIND_DUAL|TTV_FIND_CMD; sig = start; }
    // --
    else { mask.DATA = start; mask.NEXT = NULL; mask_out = &mask; sig = end; }

    mask_clk = addchain(NULL, clk);

    if (!all)
      path_list = ttv_getaccess(tf, NULL, sig, mask_out, mask_clk, TTV_DELAY_MAX, TTV_DELAY_MIN, type);
    else
      path_list = ttv_getsigaccess(tf, NULL, sig, mask_out, mask_clk, TTV_DELAY_MAX, TTV_DELAY_MIN, type);

    freechain(mask_clk);

    ttv_set_search_exclude_type(my_search_exclude_start, my_search_exclude_end, &old0, &old1);
    return path_list;
}

/*****************************************************************/
/*****************************************************************/
/* GLOBAL Path Browsing function                                 */
/*****************************************************************/
/*****************************************************************/

chain_list *
ttv_GetPaths_sub(ttvfig_list *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax)
{
    int minmax_i, all_i;
    chain_list *res;

//    ttv_checkinfchange(tf);

    if (!strcasecmp (minmax, "max")) minmax_i = TTV_FIND_MAX;
    else
    if (!strcasecmp (minmax, "min")) minmax_i = TTV_FIND_MIN;
    else {
        avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
//        fprintf (stderr, "ttv_GetPaths: available values for 'minmax' are 'min' or 'max'\n");
        return NULL;
    }

    if (!strcasecmp (all, "critic")) all_i = 0;
    else
    if (!strcasecmp (all, "all")) all_i = 1;
    else {
        avt_errmsg(TTV_API_ERRMSG, "023", AVT_ERROR);
//        fprintf (stderr, "error: available values for 'all' are 'critic' or 'all'\n");
        return NULL;
    }

    if (ckonly) ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);

    if (!strcasecmp (path, "path"))
      res=pathchain (ttv_getcritic_pathlist (tf, start, end, dir, number, minmax_i, 0, 1, all_i));
    else
    if (!strcasecmp (path, "path_v2"))
      res=pathchain (filter_parallel_to_node(ttv_getcritic_pathlist (tf, start, end, dir, number, minmax_i, 0, 1, 1)));
    else
    if (!strcasecmp (path, "access"))
      res=pathchain (ttv_GetAccessList (tf, start, end, clock, dir, number, minmax_i, FLAG_REALACCESS, 1, all_i));
    else {
        avt_errmsg(TTV_API_ERRMSG, "024", AVT_ERROR);
//        fprintf (stderr, "error: available values for 'path' are 'path' or 'access'\n");
    }

    if (ckonly) ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
    return res;
}
chain_list *ttv_internal_GetPaths(ttvfig_list *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax)
{
    int minmax_i, all_i;

    if (!strcasecmp (minmax, "max")) minmax_i = TTV_FIND_MAX;
    else
    if (!strcasecmp (minmax, "min")) minmax_i = TTV_FIND_MIN;
    else {
        avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
        //fprintf (stderr, "error: available values for 'minmax' are 'min' or 'max'\n");
        return NULL;
    }

    if (!strcasecmp (all, "critic")) all_i = 0;
    else
    if (!strcasecmp (all, "all")) all_i = 1;
    else {
        avt_errmsg(TTV_API_ERRMSG, "023", AVT_ERROR);
//        fprintf (stderr, "error: available values for 'all' are 'critic' or 'all'\n");
        return NULL;
    }

    if (!strcasecmp (path, "path"))
        return pathchain (ttv_getcritic_pathlist (tf, start, end, dir, number, minmax_i, 0, 1, all_i));
    else
/*    if (!strcasecmp (path, "access"))
        return pathchain (ttv_GetAccessList (tf, start, end, clock, dir, number, minmax_i, 0, 1, all_i));
    else*/
      if (!strcasecmp (path, "access"))
        return pathchain (ttv_GetAccessList (tf, start, end, clock, dir, number, minmax_i, FLAG_REALACCESS, 1, all_i));
    else {
        avt_errmsg(TTV_API_ERRMSG, "024", AVT_ERROR);
//        fprintf (stderr, "error: available values for 'path' are 'path' or 'access'\n");
        return NULL;
    }
}

chain_list *ttv_internal_GetPaths_EXPLICIT(ttvfig_list *tf, chain_list *clock, chain_list *start, chain_list *end, char *dir, long number, char *all, char *path, char *minmax)
{
  chain_list *allpaths;
  char *din=NULL/*"*"*/, *dout=NULL/*"*"*/;
  TTV_EXPLICIT_CLOCK_NODES=clock;
  TTV_EXPLICIT_START_NODES=start;
  TTV_EXPLICIT_END_NODES=end;

  if (TTV_EXPLICIT_CLOCK_NODES==NULL)
    {
      int a, b;
      a=countchain(TTV_EXPLICIT_START_NODES);
      b=countchain(TTV_EXPLICIT_END_NODES);
      if (a!=0 || b!=0)
        {
          if (a==0 && b!=0) dout="*"; // just pour activer le mode non dual
          else if (b==0 || a!=0) din="*"; // just pour activer le mode dual
          else if (a==0 || a>=b) dout="*"; // just pour activer le mode non dual
          else if (b==0 || a<b) din="*"; // just pour activer le mode dual
        }
    }

  allpaths=ttv_internal_GetPaths(tf, "*", din, dout, dir, number, all , path, minmax);
  
  TTV_EXPLICIT_START_NODES=NULL;
  TTV_EXPLICIT_END_NODES=NULL;
  TTV_EXPLICIT_CLOCK_NODES=NULL;
  return allpaths;
}

/*****************************************************************/
/*****************************************************************/
/* OBSOLETE Path Browsing function                               */
/*****************************************************************/
/*****************************************************************/

chain_list *
ttv_GetCriticPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number, char *minmax)
{
    avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticPathList");
//    fprintf (stderr, "warning: ttv_GetCriticPathList is obsolete, use ttv_GetPaths\n");
    if (!strcasecmp (minmax, "max"))
    return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MAX, 0, 1, 0));
    else
    if (!strcasecmp (minmax, "min"))
    return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MIN, 0, 1, 0));

    avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
//    fprintf (stderr, "error: available values for minmax are 'min' or 'max'\n");
    return NULL;
}

/*****************************************************************/
chain_list *
ttv_GetCriticMaxPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number)
{
    avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticMaxPathList");
//    fprintf (stderr, "warning: ttv_GetCriticMaxPathList is obsolete, use ttv_GetPaths\n");
    return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MAX, 0, 1, 0));
}

/*****************************************************************/
chain_list *
ttv_GetCriticMinPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number)
{
    avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticMinPathList");
//    fprintf (stderr, "warning: ttv_GetCriticMinPathList is obsolete, use ttv_GetPaths\n");
    return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MIN, 0, 1, 0));
}

/*****************************************************************/
chain_list *ttv_internal_GetCriticPath(TimingSignal *start, TimingSignal *end, char *dir, long number, int max)
{
  ttvfig_list *tf=getattvfig(start, end);
  if (tf==NULL) return NULL;
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, max?TTV_FIND_MAX:TTV_FIND_MIN, 0, 0, 0));
}

chain_list *
ttv_GetCriticMaxPath(TimingSignal *start, TimingSignal *end, char *dir, long number)
{
  ttvfig_list *tf=getattvfig(start, end);
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticMaxPath");
//    fprintf (stderr, "warning: ttv_GetCriticMaxPath is obsolete, use ttv_GetPaths\n");
  if (tf==NULL) return NULL;
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MAX, 0, 0, 0));
}

/*****************************************************************/
chain_list *
ttv_GetCriticMinPath(TimingSignal *start, TimingSignal *end, char *dir, long number)
{
  ttvfig_list *tf=getattvfig(start, end);
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticMinPath");
//  fprintf (stderr, "warning: ttv_GetCriticMinPath is obsolete, use ttv_GetPaths\n");
  if (tf==NULL) return NULL;
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MIN, 0, 0, 0));
}

/*****************************************************************/
chain_list *
ttv_GetAllMaxPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number)
{
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMaxPathList");
//  fprintf (stderr, "warning: ttv_GetAllMaxPathList is obsolete, use ttv_GetPaths\n");
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MAX, 0, 1, 1));
}

/*****************************************************************/
chain_list *
ttv_GetAllMinPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number)
{
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMinPathList");
//    fprintf (stderr, "warning: ttv_GetAllMinPathList is obsolete, use ttv_GetPaths\n");
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MIN, 0, 1, 1));
}

/*****************************************************************/
chain_list *
ttv_GetAllMaxPath(TimingSignal *start, TimingSignal *end, char *dir, long number)
{
  ttvfig_list *tf ;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMaxPath");
//    fprintf (stderr, "warning: ttv_GetAllMaxPath is obsolete, use ttv_GetPaths\n");
  tf=getattvfig(start, end);
  if (tf==NULL) return NULL;
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MAX, 0, 0, 1));
}

/*****************************************************************/
chain_list *
ttv_GetAllMinPath(TimingSignal *start, TimingSignal *end, char *dir, long number)
{
  ttvfig_list *tf ;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMinPath");
//    fprintf (stderr, "warning: ttv_GetAllMinPath is obsolete, use ttv_GetPaths\n");
  tf=getattvfig(start, end);
  if (tf==NULL) return NULL;
  return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MIN, 0, 0, 1));
}

/*****************************************************************/
chain_list *
ttv_GetUnsortedCriticMaxPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number)
{
    return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MAX, 1, 1, 0));
}

/*****************************************************************/

chain_list *
ttv_GetUnsortedCriticMinPathList(ttvfig_list *tf, char *start, char *end, char *dir, long number)
{
    return pathchain(ttv_getcritic_pathlist(tf, start, end, dir, number, TTV_FIND_MIN, 1, 1, 0));
}

/*****************************************************************/
chain_list*
ttv_GetMaxTimingPathFrom(ttvsig_list * source, char dir)
{
  int             sens;
  ttvfig_list *fig;

  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetMaxTimingPathFrom");
  //fprintf (stderr, "warning: ttv_GetMaxTimingPathFrom is obsolete, use ttv_GetPaths\n");
  if (source==NULL) return NULL;
  fig=getattvfig(source, NULL);
  sens = ttv_DirectionToIndex(dir);
  return pathchain(ttv_GetPathBySignal( fig, source, NULL, sens, -1, TTV_FIND_MAX));
}

/*****************************************************************/
chain_list *
ttv_GetMaxTimingPathTo(ttvsig_list * dest, char dir)
{
  int             sens;
  ttvfig_list *fig;

  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetMaxTimingPathTo");
  //fprintf (stderr, "warning: ttv_GetMaxTimingPathTo is obsolete, use ttv_GetPaths\n");
  if (dest==NULL) return NULL;
  fig=getattvfig(dest, NULL);
  sens = ttv_DirectionToIndex(dir);
  return pathchain(ttv_GetPathBySignal( fig, NULL, dest, -1, sens, TTV_FIND_MAX));
}

/*****************************************************************/
chain_list *
ttv_GetMinTimingPathFrom(ttvsig_list * source, char dir)
{
  int             sens;
  ttvfig_list *fig;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetMinTimingPathFrom");
  //fprintf (stderr, "warning: ttv_GetMinTimingPathFrom is obsolete, use ttv_GetPaths\n");
  if (source==NULL) return NULL;
  fig=getattvfig(source, NULL);
  sens = ttv_DirectionToIndex(dir);
  return pathchain(ttv_GetPathBySignal( fig, source, NULL, sens, -1, TTV_FIND_MIN));
}

/*****************************************************************/
chain_list *
ttv_GetMinTimingPathTo( ttvsig_list * dest, char dir)
{
  int             sens;
  ttvfig_list *fig;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetMinTimingPathTo");
  //fprintf (stderr, "warning: ttv_GetMinTimingPathTo is obsolete, use ttv_GetPaths\n");
  if (dest==NULL) return NULL;
  fig=getattvfig(dest, NULL);
  sens = ttv_DirectionToIndex(dir);
  return pathchain(ttv_GetPathBySignal( fig, NULL, dest, -1, sens, TTV_FIND_MIN));
}

/*****************************************************************/
chain_list *
ttv_GetMaxTimingPath( ttvsig_list * source, ttvsig_list * dest, char *dir)
{
    int             sensa, sensb;
    ttvfig_list *fig;
    fig=getattvfig(source, dest);
    if (fig==NULL) return NULL;
    ttv_DirectionStringToIndices(dir, &sensa, &sensb);
    return pathchain(ttv_GetPathBySignal( fig, source, dest, sensa, sensb, TTV_FIND_MAX));
}

/*****************************************************************/
chain_list *
ttv_GetMinTimingPath( ttvsig_list * source, ttvsig_list * dest, char *dir)
{
    int             sensa, sensb;
    ttvfig_list *fig;
    fig=getattvfig(source, dest);
    if (fig==NULL) return NULL;
    ttv_DirectionStringToIndices(dir, &sensa, &sensb);
    return pathchain(ttv_GetPathBySignal( fig, source, dest, sensa, sensb, TTV_FIND_MIN));
}

/*****************************************************************/
chain_list*
ttv_GetCriticMaxAccessList(ttvfig_list *tf, char *start, char *end, char *clock, char *dir, long number)
{
//    fprintf (stderr, "warning: ttv_GetCriticMaxAccessList is obsolete, use ttv_GetPaths\n");
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticMaxAccessList");
  return pathchain(ttv_GetAccessList( tf, start, end, clock, dir, number, TTV_FIND_MAX, 0, 1, 0) );
}

/*****************************************************************/
chain_list*
ttv_GetCriticMinAccessList(ttvfig_list *tf, char *start, char *end, char *clock, char *dir, long number)
{
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetCriticMinAccessList");
//    fprintf (stderr, "warning: ttv_GetCriticMinAccessList is obsolete, use ttv_GetPaths\n");
  return pathchain( ttv_GetAccessList( tf, start, end, clock, dir, number, TTV_FIND_MIN, 0, 1, 0 ) );
}

/*****************************************************************/
chain_list*
ttv_GetAllMaxAccessList(ttvfig_list *tf, char *start, char *end, char *clock, char *dir, long number)
{
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMaxAccessList");
//    fprintf (stderr, "warning: ttv_GetMaxAccessList is obsolete, use ttv_GetPaths\n");
  return pathchain( ttv_GetAccessList( tf, start, end, clock, dir, number, TTV_FIND_MAX, 0, 1, 1) );
}

/*****************************************************************/
chain_list*
ttv_GetAllMinAccessList(ttvfig_list *tf, char *start, char *end, char *clock, char *dir, long number)
{
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMinAccessList");
//    fprintf (stderr, "warning: ttv_GetMinAccessList is obsolete, use ttv_GetPaths\n");
  return pathchain( ttv_GetAccessList( tf, start, end, clock, dir, number, TTV_FIND_MIN, 0, 1, 1 ) );
}

/*****************************************************************/
chain_list*
ttv_internal_GetAccess(ttvsig_list *sig, ttvsig_list *end, char *clock, char *dir, int max )
{
  ttvfig_list *tf=getattvfig(sig, end);
  if (tf==NULL/* || end==NULL*/) return NULL;
  return pathchain( ttv_GetAccessList( tf, sig, end, clock, dir, -1, max?TTV_FIND_MAX:TTV_FIND_MIN, 0, 0, 0 ) );
}

chain_list*
ttv_GetMaxAccess(ttvsig_list *sig, ttvsig_list *end, char *clock, char *dir )
{
  ttvfig_list *tf;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetMaxAccess");
//  fprintf (stderr, "warning, ttv_GetMaxAccess is obsolete, use ttv_GetPaths\n");
  tf=getattvfig(sig, end);
  if (tf==NULL/* || end==NULL*/) return NULL;
  return pathchain( ttv_GetAccessList( tf, sig, end, clock, dir, -1, TTV_FIND_MAX, 0, 0, 0 ) );
}

/*****************************************************************/
chain_list*
ttv_GetMinAccess(ttvsig_list *sig, ttvsig_list *end, char *clock, char *dir )
{
  ttvfig_list *tf;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetMinAccess");
//  fprintf (stderr, "warning, ttv_GetMinAccess is obsolete, use ttv_GetPaths\n");
  tf=getattvfig(sig, end);
  if (tf==NULL/* || end==NULL*/) return NULL;
  return pathchain( ttv_GetAccessList( tf, sig, end, clock, dir, -1, TTV_FIND_MIN, 0, 0, 0 ) );
}

/*****************************************************************/
chain_list*
ttv_GetAllMaxAccess(ttvsig_list *sig, ttvsig_list *end, char *clock, char *dir, long number)
{
  ttvfig_list *tf;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMaxAccess");
//  fprintf (stderr, "warning, ttv_GetAllMaxAccess is obsolete, use ttv_GetPaths\n");
  tf=getattvfig(sig, end);
  if (tf==NULL/* || end==NULL*/) return NULL; // zinaps
  return pathchain( ttv_GetAccessList( tf, sig, end, clock, dir, number, TTV_FIND_MAX, 0, 0, 1 ) );
}

/*****************************************************************/
chain_list*
ttv_GetAllMinAccess(ttvsig_list *sig, ttvsig_list *end, char *clock, char *dir, long number )
{
  ttvfig_list *tf;
  avt_errmsg(TTV_API_ERRMSG, "025", AVT_WARNING, "ttv_GetAllMinAccess");
//  fprintf (stderr, "warning, ttv_GetAllMinAccess is obsolete, use ttv_GetPaths\n");
  tf=getattvfig(sig, end);
  if (tf==NULL/* || end==NULL*/) return NULL; // zinaps
  return pathchain( ttv_GetAccessList( tf, sig, end, clock, dir, number, TTV_FIND_MIN, 0, 0, 1 ) );
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

double
ttv_ComputeMaxPathDelay(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;


    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, input_slope, output_capa, VDD, temperature, TTV_FIND_MAX, &tvf)) == NULL)
        return 0.0;

    if (pth->MD != NULL) {
        val = stm_mod_delay(pth->MD, output_capa * 1e15, INSLOPE(input_slope, pth, pth->MD), NULL, "unknown");
    }
    else {
        val = pth->DELAY/TTV_UNIT;
    }
    ttv_freepathlist(pth);
    return val * 1e-12;
}

/*****************************************************************/
double
ttv_ComputeMaxPathSlope(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, input_slope, output_capa, VDD, temperature, TTV_FIND_MAX, &tvf)) == NULL)
        return 0.0;

    if (pth->MF != NULL) {
        val = stm_mod_slew(pth->MF, output_capa * 1e15, INSLOPE(input_slope, pth, pth->MF), NULL, NULL, "unknown");
    }
    else {
        val = pth->SLOPE/TTV_UNIT;
    }
    ttv_freepathlist(pth);
    return val*1e-12;
}

/*****************************************************************/
double
ttv_ComputeMinPathDelay(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, input_slope, output_capa, VDD, temperature, TTV_FIND_MIN, &tvf)) == NULL)
        return 0.0;

    if (pth->MD != NULL) {
        val = stm_mod_delay(pth->MD, output_capa * 1e15, INSLOPE(input_slope, pth, pth->MD), NULL, "unknown");
    }
    else {
        val = pth->DELAY/TTV_UNIT;
    }
    ttv_freepathlist(pth);
    return val * 1e-12;
}

/*****************************************************************/
double
ttv_ComputeMinPathSlope(char *figure, char *start, char *end, char *dir, double input_slope, double output_capa, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, input_slope, output_capa, VDD, temperature, TTV_FIND_MIN, &tvf)) == NULL)
        return 0.0;

    if (pth->MF != NULL) {
        val = stm_mod_slew(pth->MF, output_capa * 1e15, INSLOPE(input_slope, pth, pth->MF), NULL, NULL, "unknown");
    }
    else {
        val = pth->SLOPE/TTV_UNIT;
    }
    ttv_freepathlist(pth);
    return val*1e-12;
}

/*****************************************************************/
double
ttv_GetPathMinSlope(char *figure, char *start, char *end, char *dir, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, 0.0, 0.0, VDD, temperature, TTV_FIND_MIN, &tvf)) == NULL)
        return 0.0;

    val = pth->SLOPE;

    ttv_freepathlist(pth);
    return val*1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetPathMinDelay(char *figure, char *start, char *end, char *dir, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, 0.0, 0.0, VDD, temperature, TTV_FIND_MIN, &tvf)) == NULL)
        return 0.0;

    val = pth->DELAY;

    ttv_freepathlist(pth);
    return val * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetPathMaxSlope(char *figure, char *start, char *end, char *dir, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, 0.0, 0.0, VDD, temperature, TTV_FIND_MAX, &tvf)) == NULL)
        return 0.0;

    val = pth->SLOPE;

    ttv_freepathlist(pth);
    return val*1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetPathMaxDelay(char *figure, char *start, char *end, char *dir, double VDD, double temperature)
{
    ttvpath_list   *pth;
    ttvfig_list    *tvf;
    double          val;

    if ((pth = ttv_GetPathByConditions(figure, start, end, dir, 0.0, 0.0, VDD, temperature, TTV_FIND_MAX, &tvf)) == NULL)
        return 0.0;

    val = pth->DELAY;

    ttv_freepathlist(pth);
    return val * 1e-12/TTV_UNIT;
}

/*****************************************************************/
static chain_list *
ttv_GetSigList(ttvsig_list ** sig, int nb)
{
    int             i;
    chain_list     *cl = NULL;

    for (i = 0; i < nb; i++)
        cl = addchain(cl, sig[i]);
    return reverse(cl);
}

/*****************************************************************/

chain_list *ttv_GetTimingSignalList(ttvfig_list * tvf, char *type, char *location)
{
    if (!strcmp (type, "connector"))
        return ttv_GetConnectorList (tvf);

    if (!strcmp (type, "breakpoint")) {
        if (!strcmp (location, "interface")) 
            return ttv_GetInterfaceBreakpointList (tvf);
        if (!strcmp (location, "internal")) 
            return ttv_GetInternalBreakpointList (tvf);
        if (!strcmp (location, "all")) 
            return append (ttv_GetInternalBreakpointList (tvf), ttv_GetInterfaceBreakpointList (tvf));
    }

    if (!strcmp (type, "precharge")) {
        if (!strcmp (location, "interface")) 
            return ttv_GetInterfacePrechargeList (tvf);
        if (!strcmp (location, "internal")) 
            return ttv_GetInternalPrechargeList (tvf);
        if (!strcmp (location, "all")) 
            return append (ttv_GetInternalPrechargeList (tvf), ttv_GetInterfacePrechargeList (tvf));
    }

    if (!strcmp (type, "latch")) {
        if (!strcmp (location, "interface")) 
            return ttv_GetInterfaceLatchList (tvf);
        if (!strcmp (location, "internal")) 
            return ttv_GetInternalLatchList (tvf);
        if (!strcmp (location, "all")) 
            return append (ttv_GetInternalLatchList (tvf), ttv_GetInterfaceLatchList (tvf));
    }

    if (!strcmp (type, "command")) {
        if (!strcmp (location, "interface")) 
            return ttv_GetInterfaceCommandList (tvf);
        if (!strcmp (location, "internal")) 
            return ttv_GetInternalCommandList (tvf);
        if (!strcmp (location, "all")) 
            return append (ttv_GetInternalCommandList (tvf), ttv_GetInterfaceCommandList (tvf));
    }
    return NULL;
}


/*****************************************************************/

chain_list     *
ttv_GetConnectorList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->CONSIG, tvf->NBCONSIG);
}

/*****************************************************************/
char           *
ttv_GetConnectorName(ttvsig_list * tvs)
{
  if (tvs==NULL) return "";
  return tvs->NETNAME;
}

/*****************************************************************/
char
ttv_GetConnectorDirection(ttvsig_list * tvs)
{
  if (tvs==NULL) return '?';
  if ((tvs->TYPE & TTV_SIG_CT) == TTV_SIG_CT)
    return 't';
  if ((tvs->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ)
    return 'z';
  if ((tvs->TYPE & TTV_SIG_CX) == TTV_SIG_CX)
    return 'u';
  if ((tvs->TYPE & TTV_SIG_CB) == TTV_SIG_CB)
    return 'b';
  if ((tvs->TYPE & TTV_SIG_CI) == TTV_SIG_CI)
    return 'i';
  if ((tvs->TYPE & TTV_SIG_CO) == TTV_SIG_CO)
    return 'o';
  return '?';
}

/*****************************************************************/
double
ttv_GetSignalCapacitance(ttvsig_list * tvs)
{
  float add=0;
  if (tvs==NULL) return -1;
  if ((tvs->TYPE & TTV_SIG_CO)==TTV_SIG_CO
     || (tvs->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ
     || (tvs->TYPE & TTV_SIG_CB)==TTV_SIG_CB
     || (tvs->TYPE & TTV_SIG_CT)==TTV_SIG_CT
     || (tvs->TYPE & TTV_SIG_N)==TTV_SIG_N)
   add=ttv_get_signal_output_capacitance(ttv_GetSignalTopTimingFigure(tvs), tvs);

  return (tvs->CAPA+add) * 1e-15;
}

double
ttv_GetSignalLoad(ttvsig_list * tvs)
{
  float add=0;
  if (tvs==NULL) return -1;
  if ((tvs->TYPE & TTV_SIG_CO)==TTV_SIG_CO
     || (tvs->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ
     || (tvs->TYPE & TTV_SIG_CB)==TTV_SIG_CB
     || (tvs->TYPE & TTV_SIG_CT)==TTV_SIG_CT
     || (tvs->TYPE & TTV_SIG_N)==TTV_SIG_N)
   add=ttv_get_signal_output_capacitance(ttv_GetSignalTopTimingFigure(tvs), tvs);

  return add * 1e-15;
}

/*****************************************************************/
double
ttv_GetConnectorCapacitance(ttvsig_list * tvs)
{
  if (tvs==NULL) return -1;
  return ttv_GetSignalCapacitance(tvs);
}

/*****************************************************************/
int
ttv_GetNameIndex(char *sig)
{
    sig = mbk_devect(sig, "[", "]");
    return vectorindex(sig);
}

/*****************************************************************/
char           *
ttv_GetNameRadical(char *sig)
{
    sig = mbk_devect(sig, "[", "]");
    return vectorradical(sig);
}

/*****************************************************************/
void
ttv_GetVectorConnectorRange(ttvsig_list * tvs, int *left_bound, int *right_bound)
{
    int             i, idx;
    int             min = 100000, max = -100000;
    char           *rad;

    rad = ttv_GetNameRadical(ttv_GetConnectorName(tvs));

    for (i = 0; i < tvs->ROOT->NBCONSIG; i++) {
        if (rad == ttv_GetNameRadical(ttv_GetConnectorName(tvs->ROOT->CONSIG[i]))) {
            idx = ttv_GetNameIndex(ttv_GetConnectorName(tvs->ROOT->CONSIG[i]));
            if (idx > max)
                max = idx;
            if (idx < min)
                min = idx;
        }
    }
    *left_bound = min;
    *right_bound = max;
}

/*****************************************************************/
int
ttv_GetVectorConnectorLeftBound(ttvsig_list * tvs)
{
    int             l, r;

    if (tvs==NULL) return -1;
    ttv_GetVectorConnectorRange(tvs, &l, &r);
    return l;
}

/*****************************************************************/
int
ttv_GetVectorConnectorRightBound(ttvsig_list * tvs)
{
    int             l, r;

    if (tvs==NULL) return -1;
    ttv_GetVectorConnectorRange(tvs, &l, &r);
    return r;
}

/*****************************************************************/
chain_list     *
ttv_GetInterfaceBreakpointList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->EBREAKSIG, tvf->NBEBREAKSIG);
}

/*****************************************************************/
chain_list     *
ttv_GetInterfaceLatchList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->ELATCHSIG, tvf->NBELATCHSIG);
}

/*****************************************************************/
chain_list     *
ttv_GetInterfaceCommandList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->ELCMDSIG, tvf->NBELCMDSIG);
}

/*****************************************************************/
chain_list     *
ttv_GetInterfacePrechargeList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->EPRESIG, tvf->NBEPRESIG);
}

/*****************************************************************/
chain_list * ttv_GetInternalBreakpointList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->IBREAKSIG, tvf->NBIBREAKSIG);
}

/*****************************************************************/
chain_list     *
ttv_GetInternalLatchList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->ILATCHSIG, tvf->NBILATCHSIG);
}

/*****************************************************************/
chain_list     *
ttv_GetInternalCommandList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->ILCMDSIG, tvf->NBILCMDSIG);
}

/*****************************************************************/
chain_list     *
ttv_GetInternalPrechargeList(ttvfig_list * tvf)
{
  if (tvf==NULL) return NULL;
  return ttv_GetSigList(tvf->IPRESIG, tvf->NBIPRESIG);
}

/*****************************************************************/
char           *
ttv_GetSignalNetName(ttvsig_list * tvs)
{
  if (tvs==NULL) return "";
  return tvs->NETNAME;
}

/*****************************************************************/
char           *
ttv_GetSignalName(ttvsig_list * tvs)
{
  if (tvs==NULL) return "";
  return tvs->NAME;
}

/*****************************************************************/
char           *
ttv_GetFullSignalNetName(ttvfig_list * tvf, ttvsig_list * tvs)
{
    char buf[1024], *c;
    if (tvs==NULL) return "";
    c=getstatictempbuf();
    strcpy(c, ttv_getnetname(tvf, buf, tvs));
    return c;
}

/*****************************************************************/
char           *
ttv_GetFullSignalName(ttvfig_list * tvf, ttvsig_list * tvs)
{
    char buf[1024], *c;
    if (tvs==NULL) return "";
    c=getstatictempbuf();
    strcpy(c, ttv_getsigname(tvf, buf, tvs));
    return c;
}

/*****************************************************************/
static char           *
__ttv_GetSignalType(long TYPE)
{
    if ((TYPE & TTV_SIG_C) == TTV_SIG_C) {
        if ((TYPE & TTV_SIG_Q) == TTV_SIG_Q)
            return ("connector command");
        else if ((TYPE & TTV_SIG_R) == TTV_SIG_R)
            return ("connector precharge");
        else if ((TYPE & TTV_SIG_B) == TTV_SIG_B)
            return ("connector breakpoint");
        else
            return ("connector");
    }

    if ((TYPE & TTV_SIG_N) == TTV_SIG_N) {
        if ((TYPE & TTV_SIG_Q) == TTV_SIG_Q)
            return ("internal connector command");
        else if ((TYPE & TTV_SIG_R) == TTV_SIG_R)
            return ("internal connector precharge");
        else if ((TYPE & TTV_SIG_B) == TTV_SIG_B)
            return ("internal connector breakpoint");
        else
            return ("internal connector");
    }

    if ((TYPE & TTV_SIG_Q) == TTV_SIG_Q)
        return ("command");

    if ((TYPE & TTV_SIG_L) == TTV_SIG_L) {
        if ((TYPE & TTV_SIG_LL) == TTV_SIG_LL)
            return ("latch");
        else
            return ("flip-flop");
    }

    if ((TYPE & TTV_SIG_R) == TTV_SIG_R)
        return ("precharge");
    if ((TYPE & TTV_SIG_I) == TTV_SIG_I)
        return ("factorisation signal");

    if ((TYPE & TTV_SIG_S) == TTV_SIG_S)
        return ("internal signal");
    if ((TYPE & TTV_SIG_B) == TTV_SIG_B)
        return ("breakpoint");

    return "";
}

/*****************************************************************/
char           *
ttv_GetSignalType(ttvsig_list * tvs)
{
  if (tvs==NULL) return NULL;
  return __ttv_GetSignalType(tvs->TYPE);
}

/*****************************************************************/
chain_list *
ttv_GetLatchCommands(ttvsig_list * tvs)
{
  if (tvs==NULL) return NULL;
  return ttv_getlrcmd(ttv_GetSignalTopTimingFigure(tvs), tvs);
}

static chain_list *ttv_api_ptype2chain(ptype_list *pt)
{
  chain_list *cl=NULL;
  ptype_list *pt1;
  for (pt1=pt; pt1!=NULL; pt1=pt1->NEXT) cl=addchain(cl, pt1->DATA);
  freeptype(pt);
  return cl;
}

chain_list *
ttv_GetLatchEventCommands(ttvsig_list * tvs, char dir)
{
  int ndir;
  ttvfig_list *tvf;
  long type;

  if (tvs==NULL) return NULL;
  ndir=ttv_DirectionToIndex(dir);

  if (ndir==TTVAPI_NONE)
    {
      avt_errmsg(TTV_API_ERRMSG, "018", AVT_ERROR, dir);
      return NULL;
    }
  tvf=ttv_GetSignalTopTimingFigure(tvs);
  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;

  if (ndir==TTVAPI_EITHER)
    return ttv_getlrcmd(ttv_GetSignalTopTimingFigure(tvs), tvs);
  else if (ndir==TTVAPI_UP)
    return ttv_api_ptype2chain(ttv_getlatchaccess(tvf, &tvs->NODE[1], type));
  else
    return ttv_api_ptype2chain(ttv_getlatchaccess(tvf, &tvs->NODE[0], type));
}

/*****************************************************************/
ttvsig_list *
ttv_GetTimingEventSignal(ttvevent_list * tve)
{
  if (tve==NULL) return NULL;
  return tve->ROOT;
}

/*****************************************************************/
// deprecated 
ttvsig_list *ttv_GetLatchCommandSignal(ttvevent_list *tve)
{
  return ttv_GetTimingEventSignal(tve);
}

/*****************************************************************/
char ttv_GetLatchCommandDirection(ttvevent_list *tve)
{
  return ttv_GetTimingEventDirection(tve);
}

/*****************************************************************/
char
ttv_GetTimingEventDirection(ttvevent_list * tve)
{
  if (tve==NULL) return '?';
  if (tve == &tve->ROOT->NODE[1])
    return 'u';
  if (tve == &tve->ROOT->NODE[0])
    return 'd';
  return '?';
}

/*****************************************************************/
int ttv_PathIsHZ(ttvpath_list *ta)
{
  return (ta->TYPE & TTV_FIND_HZ ) == TTV_FIND_HZ;
}

/*****************************************************************/
ttvsig_list *
ttv_GetPathStartSignal(ttvpath_list * ta)
{
  if (ta==NULL) return NULL;
  return ta->NODE->ROOT;
}

/*****************************************************************/
ttvsig_list *
ttv_GetPathEndSignal(ttvpath_list * ta)
{
if (ta==NULL) return NULL;
 return ta->ROOT->ROOT;
}

/*****************************************************************/
ttvevent_list  *
ttv_GetPathCommand(ttvpath_list * ta)
{
  if (ta==NULL) return NULL;
  return ta->CMD;
}

/*****************************************************************/
ttvevent_list  *
ttv_GetPathAccessLatchCommand(ttvpath_list * ta)
{
  if (ta==NULL) return NULL;
  return ta->CMDLATCH;
}
/*****************************************************************/
ttvevent_list  *
ttv_GetPathAccessLatch(ttvpath_list * ta)
{
  if (ta==NULL) return NULL;
  return ta->LATCH;
}

/*****************************************************************/
char
ttv_GetPathStartDirection(ttvpath_list * ta)
{
  if (ta==NULL) return ' ';
  if (ta->NODE == &ta->NODE->ROOT->NODE[1])
    return 'u';
  if (ta->NODE == &ta->NODE->ROOT->NODE[0])
    return 'd';
  return '?';
}

/*****************************************************************/
char
ttv_GetPathEndDirection(ttvpath_list * ta)
{
  if (ta==NULL) return ' ';
  if (ta->ROOT == &ta->ROOT->ROOT->NODE[1])
    return 'u';
  if (ta->ROOT == &ta->ROOT->ROOT->NODE[0])
    return 'd';
  return '?';
}

/*****************************************************************/
ttvfig_list *ttv_GetSignalTopTimingFigure(ttvsig_list *tvs)
{
  ttvfig_list *tvf;
  if (tvs==NULL) return NULL;
  for (tvf=tvs->ROOT; tvf->ROOT!=NULL; tvf=tvf->ROOT) ;
  return tvf;
}

/*****************************************************************/
ttvfig_list *ttv_GetTopTimingFigure(ttvfig_list *tvf)
{
  if (tvf==NULL) return NULL;
  for (; tvf->ROOT!=NULL; tvf=tvf->ROOT) ;
  return tvf;
}

/*****************************************************************/
ttvsig_list    *
ttv_GetTimingSignal(ttvfig_list * tvf, char *name)
{
  if (tvf==NULL) return NULL;
  return ttv_getsig(tvf, vecname(name));
}

/*****************************************************************/
double
ttv_GetPathDelay(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  return ta->DELAY*1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetPathRefDelay(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  return ta->REFDELAY*1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetPathDataLag(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  return ta->DATADELAY * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetPathSlope(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  return ta->SLOPE*1e-12/TTV_UNIT; //OUTSLOPE(ta->SLOPE, ta, ta->MF);
}

/*****************************************************************/
double
ttv_GetPathRefSlope(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  return ta->REFSLOPE*1e-12/TTV_UNIT; //OUTSLOPE(ta->REFSLOPE, ta, ta->MF);
}

/*****************************************************************/
double ttv_GetPathStartTime(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  if (ta->CROSSMINDELAY!=TTV_NOTIME)
    return ta->CROSSMINDELAY * 1e-12/TTV_UNIT;
  return ta->DELAYSTART * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double ttv_GetPathStartSlope(ttvpath_list * ta)
{
  if (ta==NULL) return 0;
  return ta->SLOPESTART * 1e-12/TTV_UNIT;
}

/*****************************************************************/
chain_list *
ttv_GetParallelPaths(ttvpath_list * ta, long num)
{
    long            type = ta->TYPE|TTV_FIND_ALL;
    ttvpath_list *tpl;

    if (num <= 0) num = 10000;

    tpl=ttv_getpara(ttv_GetTopTimingFigure(ta->FIG), ta->FIG, ta->ROOT, ta->NODE, TTV_DELAY_MAX, TTV_DELAY_MIN, type, NULL, num);
    
    return pathchain(tpl);
}

/*****************************************************************/
chain_list *
ttv_GetParallelPathDetail(ttvpath_list * ta)
{
    chain_list     *detailchain = NULL, *cl;
    ttvcritic_list *detail, *ptd;
    //ptype_list *pt;
    long            type = (ta->TYPE & (~TTV_FIND_PATH)) | TTV_FIND_LINE;
    long sTTV_MORE_SEARCH_OPTIONS;
    int sPRECLEV;
    
    if ((ta->TYPE & TTV_FIND_ALL)==0) return ttv_GetPathDetail(ta);
    
    sTTV_MORE_SEARCH_OPTIONS=TTV_MORE_SEARCH_OPTIONS;
    sPRECLEV=V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE;
    TTV_MORE_SEARCH_OPTIONS=ta->TTV_MORE_SEARCH_OPTIONS;
    if (ta->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_FLAG_PROGATE_USED) V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=1;

    if (ta->CRITIC==NULL)
      {
        cl = ttv_getcriticpara(ttv_GetTopTimingFigure(ta->FIG), ta->FIG, ta->ROOT, ta->NODE, ta->DELAY, type, NULL, ta->TTV_MORE_SEARCH_OPTIONS);
        
        if (cl==NULL) return NULL;
        
        if (cl->NEXT!=NULL && ta->LATCH!=NULL)
          {
            chain_list *ch;
            char *name;
            
            name=ttv_GetFullSignalName(ttv_GetSignalTopTimingFigure(ta->LATCH->ROOT), ta->LATCH->ROOT);
            
            for (ch=cl; ch!=NULL; ch=ch->NEXT)
              {
                detail = ch->DATA;
                for (ptd = detail; ptd; ptd = ptd->NEXT) {
                  if (strcmp(name, ttv_GetDetailNodeName(ptd))==0) break;
                }
                if (ptd!=NULL) break;
              }
            if (ch==NULL) 
            {
              ta->CRITIC= detail = cl->DATA; cl->DATA=NULL;
              ta->CRITIC->DELAY=ta->CRITIC->REFDELAY=ta->DELAYSTART;
            }
            else
            {
              ta->CRITIC= detail = ch->DATA; ch->DATA=NULL;
              ta->CRITIC->DELAY=ta->CRITIC->REFDELAY=ta->DELAYSTART;
            }
          }
        else 
          {
            ta->CRITIC=detail = cl->DATA;
            ta->CRITIC->DELAY=ta->CRITIC->REFDELAY=ta->DELAYSTART;
            cl->DATA=NULL;
          }
        ttv_freecriticpara(cl);        
      }
    else
      detail=ta->CRITIC;

    for (ptd = detail; ptd; ptd = ptd->NEXT) {
        detailchain = addchain(detailchain, ptd);
    }

    V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=sPRECLEV;
    TTV_MORE_SEARCH_OPTIONS=sTTV_MORE_SEARCH_OPTIONS;
    detailchain=reverse(detailchain);
    return detailchain;
}

/*****************************************************************/
TimingDetailList *
ttv_GetPathDetail(ttvpath_list * ta)
{
    chain_list     *detailchain = NULL;
    ttvcritic_list *detail, *ptd;
    long            type;
    long sTTV_MORE_SEARCH_OPTIONS;
    int sPRECLEV;

    if (ta==NULL) return NULL;

    type=(ta->TYPE & (~TTV_FIND_PATH)) | TTV_FIND_LINE;

    if ((ta->TYPE & TTV_FIND_ALL)!=0) return ttv_GetParallelPathDetail(ta);

    sTTV_MORE_SEARCH_OPTIONS=TTV_MORE_SEARCH_OPTIONS;
    sPRECLEV=V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE;
    TTV_MORE_SEARCH_OPTIONS=ta->TTV_MORE_SEARCH_OPTIONS;
    if (ta->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_FLAG_PROGATE_USED) V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=1;

    if (ta->CRITIC==NULL)
      {
        detail = ttv_getcritic(ttv_GetTopTimingFigure(ta->FIG),ta->FIG, ta->ROOT, ta->NODE, ta->LATCH, ta->CMDLATCH, type);
        ta->CRITIC = detail;
        ta->CRITIC->DELAY=ta->CRITIC->REFDELAY=ta->DELAYSTART;
      }
    else
      detail=ta->CRITIC;

    for (ptd = detail; ptd; ptd = ptd->NEXT) {
      detailchain = addchain(detailchain, ptd);
    }
    
    V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE=sPRECLEV;
    TTV_MORE_SEARCH_OPTIONS=sTTV_MORE_SEARCH_OPTIONS;

    detailchain=reverse(detailchain);
    return detailchain;
}

/*****************************************************************/
void ttv_FreePathDetail(chain_list *cl)
{
  ttvcritic_list *detail;

  return;
  if (cl==NULL) return;

  detail=(ttvcritic_list *)cl->DATA;
  if ((detail->TYPE & TTV_FIND_ALL)!=0) 
    ttv_FreeParallelPathDetail(cl);
  else
    {
      ttv_freecriticlist( (ttvcritic_list *)detail->MODNAME);
      freechain(cl);
    }
}

/*****************************************************************/
void __ttv_FreeParallelPathDetail(chain_list *cl);

/*****************************************************************/
void __ttv_FreePathDetail(chain_list *cl)
{
  ttvcritic_list *detail;
  
  if (cl==NULL) return;

  detail=(ttvcritic_list *)cl->DATA;
  if ((detail->TYPE & TTV_FIND_ALL)!=0) 
    __ttv_FreeParallelPathDetail(cl);
  else
    {
      ttv_freecriticlist( (ttvcritic_list *)detail->MODNAME);
      freechain(cl);
    }
}

/*****************************************************************/
void __ttv_FreeParallelPathDetail(chain_list *cl)
{
  ttvcritic_list *detail;

  return;
  if (cl==NULL) return;

  detail=(ttvcritic_list *)cl->DATA;
  if ((detail->TYPE & TTV_FIND_ALL)==0)
    __ttv_FreePathDetail(cl);
  else
    {
      ttv_freecriticpara( (chain_list *)detail->MODNAME);
      freechain(cl);
    }
}

/*****************************************************************/
void ttv_FreePathList(chain_list *cl)
{
  ttvpath_list *ta;
  //ptype_list *pt;
  while (cl!=NULL)
    {
      ta=(ttvpath_list *)cl->DATA;
      if (ta!=NULL)
        {
          ta->NEXT=NULL;
          ttv_freepathlist(ta);
        }
      cl=cl->NEXT;
    }
}

/*****************************************************************/
ttvpath_list *ttv_List2Path(chain_list *cl)
{
  if (cl==NULL) return NULL;
  return (ttvpath_list *)cl->DATA;
}

/*****************************************************************/
ttvcritic_list *ttv_List2Detail(chain_list *cl)
{
  if (cl==NULL) return NULL;
  return (ttvcritic_list *)cl->DATA;
}

/*****************************************************************/
void ttv_FreeParallelPathDetail(chain_list *cl)
{
  ttvcritic_list *detail;
  return;
  if (cl==NULL) return;

  detail=(ttvcritic_list *)cl->DATA;
  if ((detail->TYPE & TTV_FIND_ALL)==0)
    ttv_FreePathDetail(cl);
  else
    {
      ttv_freecriticpara( (chain_list *)detail->MODNAME);
      freechain(cl);
    }
}

/*****************************************************************/
char *
ttv_GetDetailNodeName(ttvcritic_list * td)
{
  if (td==NULL) return "";
  return td->NAME;
}

/*****************************************************************/
char *
ttv_GetDetailSignalName(ttvcritic_list * td)
{
  if (td==NULL) return "";
  return td->NETNAME;
}

/*****************************************************************/
char *ttv_GetDetailSignalType(ttvcritic_list * td)
{
  if (td==NULL) return "";
  return __ttv_GetSignalType(td->SIGTYPE);
}

/*****************************************************************/
int ttv_DetailIsHZ(ttvcritic_list * td)
{
  return td->NEXT==NULL && (td->TYPE & TTV_FIND_HZ)==TTV_FIND_HZ;
}

/*****************************************************************/
double
ttv_GetDetailDelay(ttvcritic_list * td)
{
  if (td==NULL || (td->NODE_FLAG & TTV_NODE_FLAG_FIRST_OF_DETAIL)) return 0;
  return td->DELAY * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetDetailSlope(ttvcritic_list * td)
{
  if (td==NULL) return 0;
  return td->SLOPE * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetDetailRefDelay(ttvcritic_list * td)
{
  if (td==NULL || (td->NODE_FLAG & TTV_NODE_FLAG_FIRST_OF_DETAIL)) return 0;
  return td->REFDELAY * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetDetailRefSlope(ttvcritic_list * td)
{
  if (td==NULL) return 0;
  return td->REFSLOPE * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double
ttv_GetDetailSimDelay(ttvcritic_list * td)
{
  if (td==NULL) return 0;
  if (td->SIMDELAY==TTV_NOTIME) return -1;
  if (td->NODE_FLAG & TTV_NODE_FLAG_FIRST_OF_DETAIL) return 0;
  return td->SIMDELAY * 1e-12/TTV_UNIT;
}
/*****************************************************************/

double
ttv_GetDetailSimSlope(ttvcritic_list * td)
{
  if (td==NULL) return 0;
  if (td->SIMSLOPE==TTV_NOSLOPE) return -1;
  return td->SIMSLOPE * 1e-12/TTV_UNIT;
}

/*****************************************************************/
double ttv_GetDetailDataLag(ttvcritic_list * td)
{
  if (td==NULL) return 0;
  return td->DATADELAY * 1e-12/TTV_UNIT;
}

/*****************************************************************/
char           *
ttv_GetDetailType(ttvcritic_list * td)
{
  if (td==NULL) return "";
  if (td->FIG==NULL) // assume it's the first chain
    return "";
  if ((td->TYPE & TTV_FIND_RC) == TTV_FIND_RC)
    return namealloc("rc");
  else //if ((td->TYPE & TTV_FIND_GATE) == TTV_FIND_GATE)
  {
    char *gatename;
    int numinput;
    char *delaytype="gate";
    if (ttv_getgateinfo(((unsigned)td->PROP)>>24, &gatename, &numinput))
      delaytype=gatename;
    if ((td->SIGTYPE & TTV_SIG_L)!=0)
      {
        if (td->PROP & TTV_SIG_FLAGS_ISMASTER) delaytype="master";
        else if (td->PROP & TTV_SIG_FLAGS_ISSLAVE) delaytype="slave";
      }

    return namealloc(delaytype);
  }
/*  else 
    return "";*/
}

/*****************************************************************/
char
ttv_GetDetailDirection(ttvcritic_list * td)
{
  if (td==NULL) return ' ';
  if (td->SNODE == TTV_UP)
    return 'u';
  else
    return 'd';
}

/*****************************************************************/
chain_list *
ttv_GetSignalCapaList(ttvsig_list * tvs)
{
    float *cu;
    float *cumin;
    float *cumax;
    float *cd;
    float *cdmin;
    float *cdmax;
    chain_list *capachain = NULL;

    if (tvs == NULL) return NULL;

    cu = (float *)mbkalloc(sizeof(float));
    cumin = (float *)mbkalloc(sizeof(float));
    cumax = (float *)mbkalloc(sizeof(float));
    cd = (float *)mbkalloc(sizeof(float));
    cdmin = (float *)mbkalloc(sizeof(float));
    cdmax = (float *)mbkalloc(sizeof(float));

    ttv_getallsigcapas(tvs, cu, cumin, cumax, cd, cdmin, cdmax);

    *cu = *cu * 1e-12;
    *cumin = *cumin * 1e-12;
    *cumax = *cumax * 1e-12;
    *cd = *cd * 1e-12;
    *cdmin = *cdmin * 1e-12;
    *cdmax = *cdmax * 1e-12;

    capachain = addchain(capachain, cdmax);
    capachain = addchain(capachain, cdmin);
    capachain = addchain(capachain, cd);
    capachain = addchain(capachain, cumax);
    capachain = addchain(capachain, cumin);
    capachain = addchain(capachain, cu);

    return capachain;
}

/*****************************************************************/
chain_list*
ttv_GetClockList( ttvfig_list *tvf )
{
  return ttv_getclocksiglist( tvf );
}

/* ------------------------------------------------------- */

int ttv_IsClock(ttvsig_list *ts)
{
  if (getptype(ts->USER, TTV_SIG_CLOCK)!=NULL) return 1;
  return 0;
}
int ttv_IsAsynchronous(ttvsig_list *ts)
{
  if (getptype(ts->USER, TTV_SIG_ASYNCHRON)!=NULL) return 1;
  return 0;
}
/* ------------------------------------------------------- */

chain_list*
ttv_SortPathList( chain_list *list, char *order )
{
  long type = 0 ;
  chain_list *scan;
  ttvpath_list *headpath ;
  
  if( strcasecmp( order, "ascending" )==0 )
    type = TTV_FIND_MIN ;

  if( strcasecmp( order, "descending" )==0 )
    type = TTV_FIND_MAX ;

  if( type == 0 ) {
      avt_errmsg(TTV_API_ERRMSG, "026", AVT_ERROR);
    return list ;
  }
  
  for( scan = list ; scan ; scan = scan->NEXT ) {
    if( scan->NEXT ) 
      ((ttvpath_list*)scan->DATA)->NEXT = ((ttvpath_list*)scan->NEXT->DATA);
    else
      ((ttvpath_list*)scan->DATA)->NEXT = NULL ;
  }

  headpath = ((ttvpath_list*)list->DATA);
  freechain( list );
  
  return pathchain(ttv_classpath( headpath, type ));
}

/* ------------------------------------------------------- */

char *ttv_AutomaticDetailBuild(char *mode)
{
  long old=DETAIL_MODE;
  if (strcasecmp(mode,"on")==0)
    DETAIL_MODE=TTV_FIND_PATHDET;
  else if (strcasecmp(mode,"off")==0)
    DETAIL_MODE=0;
  else if (strcasecmp(mode,"noassoc")==0)
    assocdetail=0, DETAIL_MODE=0;
  else
    avt_errmsg(TTV_API_ERRMSG, "027", AVT_ERROR);
  
  if (old==TTV_FIND_PATHDET) return "on";
  return "off";
}

/* ------------------------------------------------------- */

static double _ttv_GetLatchINFO(ttvfig_list *tvf, ttvsig_list *latch, ttvsig_list *command, long type, char dir)
{
  ttvcritic_list *tc;
  chain_list *cl, *ch;
  char *name;
  double val=0;
  int vdir;

  if (latch==NULL) return 0;

  switch(ttv_DirectionToIndex(dir))
    {
    case TTVAPI_UP : vdir=0; break;
    case TTVAPI_DN : vdir=1; break;
    default: vdir=-1; break;
    }

  cl=ttv_getconstraint(tvf, latch->ROOT, latch, type, ttv_getloadedfigtypes(tvf), vdir);
  if (cl==NULL) return 0;
  if (command!=NULL)
    {
      name=ttv_GetFullSignalNetName(tvf, command);
      for (ch=cl; ch!=NULL; ch=ch->NEXT)
        {
          tc=(ttvcritic_list *)ch->DATA;
          if (((type & TTV_FIND_ACCESS)==TTV_FIND_ACCESS && strcmp(ttv_GetDetailSignalName(tc), name)==0)
              || ((type & TTV_FIND_ACCESS)!=TTV_FIND_ACCESS && strcmp(ttv_GetDetailSignalName(tc->NEXT), name)==0))
            {
              val=ttv_GetDetailDelay(tc->NEXT);
              break;
            }
          
        }
    }
  else
    {
      tc=(ttvcritic_list *)cl->DATA;
      val=ttv_GetDetailDelay(tc->NEXT);
    }
  
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      ttv_freecriticlist((ttvcritic_list *)ch->DATA);
    }
  freechain(cl);
  return val;
}

/* ------------------------------------------------------- */

double ttv_GetLatchAccess(ttvfig_list *tvf, ttvsig_list *latch, char dir, ttvsig_list *command, char *minmax)
{
  if (!strcasecmp (minmax, "min"))
    return _ttv_GetLatchINFO(tvf, latch, command, TTV_FIND_ACCESS|TTV_FIND_MIN, dir);
  else
  if (!strcasecmp (minmax, "max"))
    return _ttv_GetLatchINFO(tvf, latch, command, TTV_FIND_ACCESS|TTV_FIND_MAX, dir);
  else {
      fprintf (stderr, "error: available values for minmax are 'min' or 'max'\n");
      return 0.0;
  }
}

/* ------------------------------------------------------- */

double ttv_GetLatchMaxAccess(ttvfig_list *tvf, ttvsig_list *latch, char dir, ttvsig_list *command)
{
  fprintf (stderr, "warning, ttv_GetLatchMaxAccess is obsolete, use ttv_GetLatchAccess\n");
  return _ttv_GetLatchINFO(tvf, latch, command, TTV_FIND_ACCESS|TTV_FIND_MAX, dir);
}

/* ------------------------------------------------------- */

double ttv_GetLatchMinAccess(ttvfig_list *tvf, ttvsig_list *latch, char dir, ttvsig_list *command)
{
  fprintf (stderr, "warning, ttv_GetLatchMinAccess is obsolete, use ttv_GetLatchAccess\n");
  return _ttv_GetLatchINFO(tvf, latch, command, TTV_FIND_ACCESS|TTV_FIND_MIN, dir);
}

/* ------------------------------------------------------- */

double ttv_GetLatchSetup(ttvfig_list *tvf, ttvsig_list *latch, char dir, ttvsig_list *command)
{
  return _ttv_GetLatchINFO(tvf, latch, command, TTV_FIND_SETUP|TTV_FIND_MIN, dir);
}

/* ------------------------------------------------------- */

double ttv_GetLatchHold(ttvfig_list *tvf, ttvsig_list *latch, char dir, ttvsig_list *command)
{
  return _ttv_GetLatchINFO(tvf, latch, command, TTV_FIND_HOLD|TTV_FIND_MIN, dir);
}

/* ------------------------------------------------------- */

void ttv_SetSearchMode(char *mode)
{
  if (strcasecmp(mode,"dual")==0) SEARCH_MODE|=TTV_FIND_DUAL;
  else if (strcasecmp(mode,"!dual")==0) SEARCH_MODE&=~TTV_FIND_DUAL;
  else if (strcasecmp(mode,"findcmd")==0) SEARCH_MODE|=TTV_FIND_CMD;
  else if (strcasecmp(mode,"!findcmd")==0) SEARCH_MODE&=~TTV_FIND_CMD;
  else if (strcasecmp(mode,"ckonly")==0) ckonly=1;
  else if (strcasecmp(mode,"!ckonly")==0) ckonly=0;
  else if (strcasecmp(mode,"noendnodefilter")==0) ttv_search_mode(1, TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE);
  else if (strcasecmp(mode,"!noendnodefilter")==0) ttv_search_mode(0, TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE);
  else if (strcasecmp(mode,"sigall")==0) sigall=1;
  else if (strcasecmp(mode,"!sigall")==0) sigall=0;
  else if (strcasecmp(mode,"directaccess")==0) ttv_search_mode(1, TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS);
  else if (strcasecmp(mode,"!directaccess")==0) ttv_search_mode(0, TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS);
  else if (strcasecmp(mode,"usenodenameonly")==0) ttv_search_mode(1, TTV_MORE_OPTIONS_USE_NODE_NAME_ONLY);
  else if (strcasecmp(mode,"!usenodenameonly")==0) ttv_search_mode(0, TTV_MORE_OPTIONS_USE_NODE_NAME_ONLY);
  else if (strcasecmp(mode,"multicmd")==0) ttv_activate_multi_cmd_critic_mode(1);
  else if (strcasecmp(mode,"!multicmd")==0) ttv_activate_multi_cmd_critic_mode(0);
  else if (strcasecmp(mode,"findasync")==0) ttv_search_mode(1, TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS);
  else if (strcasecmp(mode,"!findasync")==0) ttv_search_mode(0, TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS);
  else printf("ttv_SetSearchMode: unknown search mode '%s'\n",mode);
}

/* ------------------------------------------------------- */
  
Property *ttv_GetTimingDetailProperty (TimingDetail *det, char *property)
{
    char         buf[256];
    ttvsig_list *sig;
    ttvcriticmc *mc ;
    chain_list  *head ;
    int          i ;

    if (!det) {
        sprintf (buf, "error_null_timing_detail");
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "TRANS")) {
        sprintf (buf, "%c", ttv_GetDetailDirection (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "EVENT")) {
        ttvfig_list *tvf;
        if (det->FIG) tvf=det->FIG;
        else if (det->NEXT) tvf=det->NEXT->FIG;
        else tvf=NULL;

        if (stb_getstbfig(tvf))
           return addptype (NULL, TYPE_TIMING_EVENT, det->NODE);
        
        sig = ttv_GetTimingSignal (tvf, det->NAME);
        if (det->SNODE == TTV_UP)
            return addptype (NULL, TYPE_TIMING_EVENT, (void*)(&(sig->NODE[1])));
        else
            return addptype (NULL, TYPE_TIMING_EVENT, (void*)(&(sig->NODE[0])));
    }

    if (!strcasecmp (property, "NODE_NAME")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetDetailNodeName (det)));

    if (!strcasecmp (property, "SIGNAL_NAME") || !strcasecmp (property, "NET_NAME")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetDetailSignalName (det)));

    if (!strcasecmp (property, "SIGNAL_TYPE")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetDetailSignalType (det)));

    if (!strcasecmp (property, "TYPE")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetDetailType (det)));

    if (!strcasecmp (property, "SLOPE")) {
        sprintf (buf, "%g", ttv_GetDetailSlope (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "REF_SLOPE")) {
        sprintf (buf, "%g", ttv_GetDetailRefSlope (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "SIM_SLOPE")) {
        sprintf (buf, "%g", ttv_GetDetailSimSlope (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "DELAY")) {
        sprintf (buf, "%g", ttv_GetDetailDelay (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "REF_DELAY")) {
        sprintf (buf, "%g", ttv_GetDetailRefDelay (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "SIM_DELAY")) {
        sprintf (buf, "%g", ttv_GetDetailSimDelay (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "DATA_LAG")) {
        sprintf (buf, "%g", ttv_GetDetailDataLag (det));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "HZ")) 
        return addptype (NULL, TYPE_INT, (void*)(long)ttv_DetailIsHZ (det));

    if (!strcasecmp (property, "MCDELAY")) {
        mc   = ttv_getassociatedcriticmc( det );
        head = NULL ;
        if( mc ) {
          for( i=0 ; i<mc->NB ; i++ ) {
            head = addchain( head, NULL );
            *((float*)(&(head->DATA))) = mc->DELAY[i]*1e-12/TTV_UNIT ;
          }
        }
        return addptype (NULL, TYPE_TABLE, reverse(head) );
    }

    avt_errmsg(TTV_API_ERRMSG, "043", AVT_ERROR, property);
    return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
//fprintf (stderr, "error: unknown property %s\n", property);
//    return NULL;
}

/*******************************************************************************/

static chain_list *getevstab(TimingEvent *ev, int obs)
{
  stbnode *node;
  stbpair_list *pair, *p;
  chain_list *ch=NULL, *cl;
  if ((node=stb_getstbnode(ev))!=NULL)
  {
    if (!obs)
      pair=stb_getpairnode(stb_getstbfig(ttv_GetTopTimingFigure(ev->ROOT->ROOT)),ev, 0);
    else
      pair=stb_getlimitpairnode(stb_getstbfig(ttv_GetTopTimingFigure(ev->ROOT->ROOT)),ev, 0);
//    pair=stb_globalstbtab(node->STBTAB, node->NBINDEX);
    for (p=pair; p!=NULL; p=p->NEXT)
    {
      cl=addchain(NULL, NULL); *(float *)&cl->DATA=p->U*1e-12/TTV_UNIT;
      cl=addchain(cl, NULL); *(float *)&cl->DATA=p->D*1e-12/TTV_UNIT;
      ch=addchain(ch, cl);
    }
    stb_freestbpair(pair);
  }
  return reverse(ch);
}

static ptype_list *fitdouble(double val)
{
  ptype_list *pt;
  pt=addptype(NULL, TYPE_DOUBLE, NULL);
  *(float *)&pt->DATA=(float)val;
  return pt;
}

Property *ttv_GetTimingEventProperty (TimingEvent *ev, char *property)
{
    char buf[256];

    if (!ev) {
        sprintf (buf, "error_null_timing_event");
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "TRANS")) {
        sprintf (buf, "%c", ttv_GetTimingEventDirection (ev));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "SIGNAL")) 
        return addptype (NULL, TYPE_TIMING_SIGNAL, ttv_GetTimingEventSignal (ev));

    if (!strcasecmp (property, "STABILITY_OBS")) 
        return addptype (NULL, TYPE_DOUBLE_TABLE, getevstab(ev, 1));
    if (!strcasecmp (property, "STABILITY")) 
        return addptype (NULL, TYPE_DOUBLE_TABLE, getevstab(ev, 0));

    avt_errmsg(TTV_API_ERRMSG, "043", AVT_ERROR, property);
    return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
    //fprintf (stderr, "error: unknown property %s\n", property);
    //return NULL;
}

/*******************************************************************************/

static double ttv_getsignodeslope(TimingEvent *tev, int max)
{
  long sloperef, slope;
  slope=ttv_getnodeslope(ttv_GetSignalTopTimingFigure(tev->ROOT),NULL,tev,&sloperef,max?TTV_FIND_MAX:TTV_FIND_MIN);
  return slope*1e-12/TTV_UNIT;
}

static float ttv_getleakage(TimingSignal *sig, int state, int max)
{
  ptype_list *ptypeleakage;
  leakage_list *leakage;
  
  if((ptypeleakage = getptype(sig->USER, TAS_LEAKAGE))){
      leakage = ptypeleakage->DATA;
      if(state && max) return leakage->L_UP_MAX;
      else if(state && !max) return leakage->L_UP_MIN;
      else if(!state && max) return leakage->L_DN_MAX;
      else if(!state && !max) return leakage->L_DN_MIN;
  }
  return 0.0;
}

Property *ttv_GetTimingSignalProperty (TimingSignal *sig, char *property)
{
    char buf[256];
    double val;
    float leakage;

    if (!sig) {
        sprintf (buf, "error_null_timing_signal");
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "CLOCK")) 
        return addptype (NULL, TYPE_INT, (void*)(long)ttv_IsClock (sig));

    if (!strcasecmp (property, "IS_CLOCK")) 
    {
        if (ttv_IsClock (sig))
           strcpy(buf,"yes");
        else
           strcpy(buf,"no");

        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }
    if (!strcasecmp (property, "IS_ASYNCHRONOUS")) 
    {
      if (ttv_IsAsynchronous (sig))
        strcpy(buf,"yes");
      else
        strcpy(buf,"no");
      
      return addptype (NULL, TYPE_CHAR, strdup (buf));
    }


    if (!strcasecmp (property, "DIR")) {
        sprintf (buf, "%c", ttv_GetConnectorDirection (sig));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "CAPA")) {
        sprintf (buf, "%g", ttv_GetSignalCapacitance (sig));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "LOAD")) {
        sprintf (buf, "%g", ttv_GetSignalLoad (sig));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "RIGHT_BOUND")) 
        return addptype (NULL, TYPE_INT, (void*)(long)ttv_GetVectorConnectorRightBound (sig));

    if (!strcasecmp (property, "LEFT_BOUND")) 
        return addptype (NULL, TYPE_INT, (void*)(long)ttv_GetVectorConnectorLeftBound (sig));

    if (!strcasecmp (property, "TOP_LEVEL"))
        return addptype (NULL, TYPE_TIMING_FIGURE, ttv_GetSignalTopTimingFigure (sig));

    if (!strcasecmp (property, "NET_NAME")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetSignalNetName (sig)));

    if (!strcasecmp (property, "NAME")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetSignalName (sig)));

    if (!strcasecmp (property, "TYPE")) 
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetSignalType (sig)));

    if (!strcasecmp (property, "EVENT_UP")) 
        return addptype (NULL, TYPE_TIMING_EVENT, (void*)(&(sig->NODE[1])));

    if (!strcasecmp (property, "EVENT_DOWN")) 
        return addptype (NULL, TYPE_TIMING_EVENT, (void*)(&(sig->NODE[0])));

    if (!strcasecmp (property, "RISING_SLOPE")) { 
        if ((sig->TYPE & TTV_SIG_CX) != TTV_SIG_CX && (sig->TYPE & TTV_SIG_CI) != TTV_SIG_CI && (sig->TYPE & TTV_SIG_CT)!=TTV_SIG_CT) {
            sprintf (buf, "error_not_input_connector");
            return addptype (NULL, TYPE_CHAR, strdup (buf));
        }
        val=ttv_getsignodeslope(&sig->NODE[1], 1);
        sprintf (buf, "%g", val);
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "FALLING_SLOPE")) { 
        if ((sig->TYPE & TTV_SIG_CX) != TTV_SIG_CX && (sig->TYPE & TTV_SIG_CI) != TTV_SIG_CI && (sig->TYPE & TTV_SIG_CT)!=TTV_SIG_CT) {
            sprintf (buf, "error_not_input_connector");
            return addptype (NULL, TYPE_CHAR, strdup (buf));
        }
        val=ttv_getsignodeslope(&sig->NODE[0], 1);
        sprintf (buf, "%g", val);
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }
    if (!strcasecmp (property, "LEAKAGE_UP_MAX")) {
        leakage=ttv_getleakage(sig,1,1);
        return fitdouble(leakage);
    }
    if (!strcasecmp (property, "LEAKAGE_UP_MIN")) {
        leakage=ttv_getleakage(sig,1,0);
        return fitdouble(leakage);
    }
    if (!strcasecmp (property, "LEAKAGE_DN_MAX")) {
        leakage=ttv_getleakage(sig,0,1);
        return fitdouble(leakage);
    }
    if (!strcasecmp (property, "LEAKAGE_DN_MIN")) {
        leakage=ttv_getleakage(sig,0,0);
        return fitdouble(leakage);
    }
    if (!strcasecmp (property, "GATE_TYPE")) {
        int prop=ttv_testsigflag(sig, 0xffffffff), numinput;
        char *gatename;
        if (!ttv_getgateinfo(((unsigned)prop)>>24, &gatename, &numinput))
          gatename="";
        if ((sig->TYPE & TTV_SIG_L)!=0)
         {
           if (prop & TTV_SIG_FLAGS_ISMASTER) gatename="master";
           else if (prop & TTV_SIG_FLAGS_ISSLAVE) gatename="slave";
         }
        return addptype (NULL, TYPE_CHAR, strdup (gatename));
    }
    if (!strcasecmp (property, "VOLTAGE_SWING")) {
      float low, high;
      ttv_get_signal_swing(ttv_GetSignalTopTimingFigure(sig), sig, &low, &high);
      return fitdouble(high-low);
    }
    avt_errmsg(TTV_API_ERRMSG, "043", AVT_ERROR, property);
    return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
//    fprintf (stderr, "error: unknown property %s\n", property);
//    return NULL;
}

/*******************************************************************************/
static double ttv_getsimdelayslope(TimingPath *path, double *slope)
{
  double delay=0;
  ttvcritic_list *tc;
  tc=path->CRITIC;
  if (tc==NULL) return -1;
  while (tc!=NULL)
  {
    if (tc->SIMSLOPE==TTV_NOTIME) return -1;
    if (tc!=path->CRITIC) delay+=_LONG_TO_DOUBLE(tc->SIMDELAY);
    if (tc->NEXT==NULL) 
    {
       if (tc->SIMSLOPE==TTV_NOTIME) *slope=-1;
       else *slope=_LONG_TO_DOUBLE(tc->SIMSLOPE);
    }
    tc=tc->NEXT;
  }
  return delay;
}

double ttv_GetLoopFeedBackDelay(TimingPath *path)
{
  ptype_list *pt;
  long val=0;
  if ((pt=getptype(path->USER, SIM_LOOP_FEEDBACK_DELAY_PTYPE))!=NULL) val=(long)pt->DATA;
  return _LONG_TO_DOUBLE(val);    
}

double ttv_GetCmdDelayToVT(TimingPath *path)
{
  ptype_list *pt;
  long val=0;
  if ((pt=getptype(path->USER, SIM_DELAY_TO_VT_PTYPE))!=NULL) val=(long)pt->DATA;
  return _LONG_TO_DOUBLE(val);    
}

Property *ttv_GetTimingPathProperty (TimingPath *path, char *property)
{
  char buf[256];
  double slope, delay;
  float factor;
  long delta;

  if (!path) {
    sprintf (buf, "error_null_timing_path");
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "DELAY")) {
    sprintf (buf, "%g", ttv_GetPathDelay (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "DATA_LAG")) {
    sprintf (buf, "%g", ttv_GetPathDataLag (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "SIM_DELAY")) {
    delay=ttv_getsimdelayslope(path, &slope);
    return fitdouble(delay);
  }

  if (!strcasecmp (property, "SIM_LOOP_DELAY")) {
    delay=ttv_GetLoopFeedBackDelay(path);
    return fitdouble(delay);
  }

  if (!strcasecmp (property, "SIM_DELAY_TO_VT")) {
    delay=ttv_GetCmdDelayToVT(path);
    return fitdouble(delay);
  }

  if (!strcasecmp (property, "REF_DELAY")) {
    sprintf (buf, "%g", ttv_GetPathRefDelay (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "SLOPE")) {
    sprintf (buf, "%g", ttv_GetPathSlope (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "REF_SLOPE")) {
    sprintf (buf, "%g", ttv_GetPathRefSlope (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "SIM_SLOPE")) {
    delay=ttv_getsimdelayslope(path, &slope);
    if (delay==-1) slope=-1;
    return fitdouble(slope);
  }

  if (!strcasecmp (property, "START_TIME")) {
    sprintf (buf, "%g", ttv_GetPathStartTime (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "START_SLOPE")) {
    sprintf (buf, "%g", ttv_GetPathStartSlope (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "START_SIG")) 
    return addptype (NULL, TYPE_TIMING_SIGNAL, ttv_GetPathStartSignal (path));

  if (!strcasecmp (property, "END_SIG")) 
    return addptype (NULL, TYPE_TIMING_SIGNAL, ttv_GetPathEndSignal (path));

  if (!strcasecmp (property, "COMMAND")) 
    return addptype (NULL, TYPE_TIMING_EVENT, ttv_GetPathCommand (path));

  if (!strcasecmp (property, "ACCESS_COMMAND")) 
    return addptype (NULL, TYPE_TIMING_EVENT, ttv_GetPathAccessLatchCommand (path));

  if (!strcasecmp (property, "ACCESS_LATCH")) 
    return addptype (NULL, TYPE_TIMING_EVENT, ttv_GetPathAccessLatch (path));

  if (!strcasecmp (property, "START_EVENT")) 
    return addptype (NULL, TYPE_TIMING_EVENT, path->NODE);

  if (!strcasecmp (property, "END_EVENT")) 
    return addptype (NULL, TYPE_TIMING_EVENT, path->ROOT);

  if (!strcasecmp (property, "START_TRAN") || !strcasecmp (property, "START_TRANS")) {
    sprintf (buf, "%c", ttv_GetPathStartDirection (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "END_TRAN") || !strcasecmp (property, "END_TRANS")) {
    sprintf (buf, "%c", ttv_GetPathEndDirection (path));
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "IS_HZ")) {
    if (ttv_PathIsHZ(path)) strcpy(buf,"yes");
    else strcpy(buf,"no");
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }
  if (!strcasecmp (property, "TYPE")) {
    if (path->TYPE & TTV_FIND_MAX) strcpy(buf,"max");
    else strcpy(buf,"min");
    return addptype (NULL, TYPE_CHAR, strdup (buf));
  }

  if (!strcasecmp (property, "PATH_MARGIN_FACTOR")) {
    if (!ttv_get_path_margins_info(path->FIG, path, &factor, &delta)) factor=1;
    return fitdouble(factor);
  }
  if (!strcasecmp (property, "PATH_MARGIN_DELTA")) {
    if (!ttv_get_path_margins_info(path->FIG, path, &factor, &delta)) delta=0;
    return fitdouble(_LONG_TO_DOUBLE(delta));
  }

  avt_errmsg(TTV_API_ERRMSG, "043", AVT_ERROR, property);
  return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
  //fprintf (stderr, "error: unknown property %s\n", property);
  //    return NULL;
}

/*******************************************************************************/

Property *ttv_GetTimingFigureProperty (TimingFigure *fig, char *property)
{
    char temp[256], def_supply[256], def_load[256], def_slope[256], th_low[256], th_high[256];
    char buf[256];

    if (!fig) {
        sprintf (buf, "error_null_timing_figure");
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "TOP_LEVEL"))
        return addptype (NULL, TYPE_TIMING_FIGURE, ttv_GetTopTimingFigure (fig));

    if (!strcasecmp (property, "TEMP")) {
        sprintf (temp, "%g", ttv_GetTimingFigureTemperature (fig));
        return addptype (NULL, TYPE_CHAR, strdup (temp));
    }

    if (!strcasecmp (property, "DEF_SUPPLY")) {
        sprintf (def_supply, "%g", ttv_GetTimingFigureSupply (fig));
        return addptype (NULL, TYPE_CHAR, strdup (def_supply));
    }

    if (!strcasecmp (property, "DEF_LOAD")) {
        sprintf (def_load, "%g", ttv_GetTimingFigureOutputCapacitance (fig));
        return addptype (NULL, TYPE_CHAR, strdup (def_load));
    }

    if (!strcasecmp (property, "DEF_SLOPE")) {
    sprintf (def_slope, "%g", ttv_GetTimingFigureInputSlope (fig));
        return addptype (NULL, TYPE_CHAR, strdup (def_slope));
    }

    if (!strcasecmp (property, "TH_LOW")) {
    sprintf (th_low, "%g", ttv_GetTimingFigureLowThreshold (fig));
        return addptype (NULL, TYPE_CHAR, strdup (th_low));
    }

    if (!strcasecmp (property, "TH_HIGH")) {
    sprintf (th_high, "%g", ttv_GetTimingFigureHighThreshold (fig));
        return addptype (NULL, TYPE_CHAR, strdup (th_high));
    }

    if (!strcasecmp (property, "TECH_NAME"))
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetTimingFigureTechno (fig)));

    if (!strcasecmp (property, "DATE"))
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetTimingFigureGenerationDate (fig)));

    if (!strcasecmp (property, "TIME"))
        return addptype (NULL, TYPE_CHAR, strdup(ttv_GetTimingFigureGenerationTime (fig)));

    if (!strcasecmp (property, "FIGNAME") || !strcasecmp (property, "NAME"))
        return addptype (NULL, TYPE_CHAR, strdup(fig->INFO->FIGNAME));

    if (!strcasecmp (property, "STABILITY_FIGURE"))
        return addptype (NULL, TYPE_STABILITY_FIGURE, stb_getstbfig(fig));


    avt_errmsg(TTV_API_ERRMSG, "043", AVT_ERROR, property);
    return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
    //fprintf (stderr, "error: unknown property %s\n", property);
    //return NULL;
}

/* ------------------------------------------------------- */
void ttv_RemoveTimingFigure(ttvfig_list *tf)
{
  if (tf!=NULL)
    ttv_freettvfig(tf);
}

/* ------------------------------------------------------- */
chain_list* ttv_internal_filterpathbycommand( chain_list *pathl, ttvevent_list *cmd, ttvevent_list *inev, int free) 
{
  ttvpath_list   *path ;
  chain_list *lst;
  chain_list *scanpath ;
  ttvevent_list *piev;

  for( scanpath = pathl, lst=NULL ; scanpath ; scanpath = scanpath->NEXT ) 
    {
      
      path = (ttvpath_list*)scanpath->DATA ;
      path->NEXT = NULL ;
      if (path->LATCH!=NULL) piev=path->LATCH; else piev=path->NODE;
      if (path->CMD==cmd && (inev==NULL || inev==piev))
        {
          lst=addchain(lst, path);
        }
      else
        {
          ttv_freepathlist( path );
        }
    }
 
  if (free) freechain(pathl);
  lst=reverse(lst);
  for( scanpath = lst ; scanpath!=NULL && scanpath->NEXT!=NULL ; scanpath = scanpath->NEXT ) 
    ((ttvpath_list*)scanpath->DATA)->NEXT=(ttvpath_list*)scanpath->NEXT->DATA;

  return lst;
}

/* ------------------------------------------------------- */

ttvfig_list *ttv_LoadSDF (lofig_list *fig, char *sdf_file)
{
    ttvfig_list *ttvfig;
    char technoversion[64];

    bck_sdfparse (sdf_file, fig);
    if (!fig) return NULL;
    ttvfig = bck_bck2ttv (fig, fig->NAME);

    if (elpTechnoName[0] == '\0') {
        ttvfig->INFO->TECHNONAME = namealloc ("unknown");
        sprintf (technoversion,"%.2f",0.0);
    } else {
        ttvfig->INFO->TECHNONAME = namealloc (elpTechnoName);
        sprintf (technoversion, "%.2f", elpTechnoVersion);
    }

    ttvfig->INFO->TECHNOVERSION = namealloc (technoversion);
    ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL);

    return ttvfig;
}

void ttv_RecomputeDelays(ttvfig_list *tvf)
{
  chain_list *cl;
  long type;
  inffig_list *ifl;
  if (tvf==NULL) return;

  ttv_setcachesize(10000, 10000) ; // desactivation du cache

  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;

  if (V_BOOL_TAB[__TAS_DELAY_PROP].VALUE) type|=TTV_FIND_DELAY;

  type|=TTV_FIND_LINETOP;

  ifl=getloadedinffig(tvf->INFO->FIGNAME);

  ttv_getinffile(tvf);

  cl = ttv_calcfigdelay(tvf,NULL,tvf->INFO->LEVEL,type|TTV_FIND_MIN|TTV_FIND_MAX,TTV_MODE_LINE,NULL,NULL,NULL,0) ;

  ttv_update_slope_and_output_capa(tvf);
  ttv_freefigdelay(cl) ;
  freechain(cl);

  if (ifl!=NULL) ifl->changed&=~INF_CHANGED_RECOMP;
}

chain_list* ttv_GetTimingSignalListByNet( ttvfig_list *fig, char *name )
{
  chain_list *ch ;
  chain_list *mask ;
  ch = ttv_getsigbytype_and_netname(fig,NULL,TTV_SIG_TYPEALL,mask=addchain(NULL,name));
  freechain( mask );
  return ch ;
}

chain_list *ttv_GetGeneratedClockPaths(ttvfig_list *tvf, ttvevent_list *tve, char *minmax)
{
  inffig_list *ifl;
  chain_list *res=NULL;
  int stop, edges, msk;
  char *name, *master;
  char dir[3]="  ";
  chain_list *clks, *cl, *paths, *clist, *elist;
  ttvsig_list *mck;
  int saveTTV_MAX_PATH_PERIOD;

  if (tvf==NULL || tve==NULL) return NULL;

  if (strcasecmp (minmax, "max") && strcasecmp (minmax, "min"))
   {
     avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
     return NULL;
   }

  clks=ttv_GetClockList(tvf);

  if (ttv_IsClock(tve->ROOT) && (ifl=getloadedinffig(tvf->INFO->FIGNAME))!=NULL)
    {
      stop=0;
      while (!stop)
        {
          stop=1;
          name=ttv_GetFullSignalNetName(tvf, tve->ROOT);
          if (inf_GetString (ifl, name, INF_MASTER_CLOCK, &master) && master!=NULL)
            {
              if (!inf_GetInt (ifl, name, INF_MASTER_CLOCK_EDGES, &edges)) edges=0;
              if (tve->TYPE & TTV_NODE_UP)
                {
                  dir[1]='u';
                  if (edges & 2) dir[0]='u'; else dir[0]='d';
                }
              else
                {
                  dir[1]='d';
                  if (edges & 1) dir[0]='u'; else dir[0]='d';
                }
              
              for (cl=clks; cl!=NULL && mbk_casestrcmp(ttv_GetFullSignalNetName(tvf, mck=(ttvsig_list *)cl->DATA), master)!=0; cl=cl->NEXT) ;

              if (cl!=NULL)
                {
                   clist=addchain(NULL, mck);
                   elist=addchain(NULL, tve->ROOT);
                   saveTTV_MAX_PATH_PERIOD=V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE;
                   V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE=0;
                   ttv_search_mode(1, TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS);
                   paths=ttv_internal_GetPaths_EXPLICIT(tvf, clist, NULL, elist, dir, 1, "critic", "access", minmax);
                   ttv_search_mode(0, TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS);
                   V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE=saveTTV_MAX_PATH_PERIOD;
                   freechain(clist);
                   freechain(elist);
                   if (paths!=NULL)
                     {
                       res=append(res, paths);
                       tve=((ttvpath_list *)paths->DATA)->NODE;
                       stop=0;
                     }
                }
            }
        }
    }
  freechain(clks);
  return reverse(res);
}

void ttv_SetFigureFlag(ttvfig_list *tvf, char *mode)
{
  if (tvf!=NULL) 
    {
      if (!strcasecmp(mode,"tmaDriveCapaout_Is_Handled"))
        tvf->USER=addptype(tvf->USER, TTV_FIG_CAPAOUT_HANDLED, NULL);
      else
        avt_errmsg(TTV_API_ERRMSG, "051", AVT_ERROR, mode);
    }
}

char *ttv_GetClockInfos(ttvfig_list *tvf, char *name, char *minmax)
{
  inffig_list *ifl;
  char *master;
  double rise=0, fall=0, per=0, mrise=0, mfall=0, mper=0;
  char buf[1024];
  chain_list *cl, *lst;

  if (tvf==NULL) return NULL;

  if (strcasecmp (minmax, "max") && strcasecmp (minmax, "min"))
   {
     avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
     return NULL;
   }

  if ((ifl=getloadedinffig(tvf->INFO->FIGNAME))!=NULL)
    {
      name=namealloc(name);
      lst=inf_GetEntriesByType(ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
      for (cl=lst; cl; cl=cl->NEXT)
        if (mbk_TestREGEX(name, (char *)cl->DATA)) break;
      if (cl!=NULL) name=(char *)cl->DATA;
      freechain(lst);
      
      if (strcasecmp (minmax, "max")==0)
       {
         inf_GetDouble (ifl, name, INF_MAX_RISE_TIME, &rise);
         inf_GetDouble (ifl, name, INF_MAX_FALL_TIME, &fall);
       }
      else
       {
         inf_GetDouble (ifl, name, INF_MIN_RISE_TIME, &rise);
         inf_GetDouble (ifl, name, INF_MIN_FALL_TIME, &fall);
       }

      inf_GetDouble (ifl, name, INF_CLOCK_PERIOD, &per);
      
      master=NULL;
      if (inf_GetString (ifl, name, INF_MASTER_CLOCK, &master) && master!=NULL)
      {
       if (strcasecmp (minmax, "max")==0)
        {
          inf_GetDouble (ifl, master, INF_MAX_RISE_TIME, &mrise);
          inf_GetDouble (ifl, master, INF_MAX_FALL_TIME, &mfall);
        }
       else
        {
          inf_GetDouble (ifl, master, INF_MIN_RISE_TIME, &mrise);
          inf_GetDouble (ifl, master, INF_MIN_FALL_TIME, &mfall);
        }
       inf_GetDouble (ifl, master, INF_CLOCK_PERIOD, &mper);
      }
      sprintf(buf,"rise %g fall %g period %g master %s rise %g fall %g period %g", rise, fall, per, master==NULL?"none":master, mrise, mfall, mper);
      return strdup(buf);
    }
  return NULL;
}

void ttv_UpdateInf(ttvfig_list *tvf)
{
  ttv_getinffile(tvf);
}

