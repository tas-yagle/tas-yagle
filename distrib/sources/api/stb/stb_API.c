#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include MSL_H
#include AVT_H
#include INF_H
#include ELP_H
#include CNS_H
#include YAG_H
#include TLC_H
#include TRC_H
#include STM_H
#include TTV_H
#include MCC_H
#include SIM_H
#include TAS_H
#include FCL_H
#include BEH_H
#include CBH_H
#include LIB_H
#include TLF_H
#include STB_H

#define API_USE_REAL_TYPES
#include "stb_LOCAL.h"
#include "../ttv/ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "stb_API.h"
#include "../ttv/ttv_API_display.h"
#include "../../tas/stb/stb_util.h"

#define RISING 'u'
#define FALLING 'd'


static stbdebug_list *LATEST_INPUT=NULL;
static stbdebug_list *LATEST_DEBUGLIST=NULL;
static long STBDEBUG_FLAG=0;
static int stbforceupdate=0;

long stbsetdebugflag(long val)
{
  long last;
  last=STBDEBUG_FLAG;
  STBDEBUG_FLAG=val;
  stbforceupdate=1;
  return last;
}

static char *
vecname(char *name)
{
        return mbk_vect(mbk_decodeanyvector(name), '[', ']');
}


stbfig_list *stb (TimingFigure *ttvfig)
{
    stbfig_list *stbfig;
    char graphmode;

    STB_MODE_VAR = STB_STABILITY_FF;
    STB_CTK_VAR = STB_CTK_NOT|STB_CTK_REPORT;
        

    if (!ttvfig) {
        fprintf (stderr, "STB ERROR: NULL ttvfig\n");
        return NULL;
    }

    if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)
      graphmode=STB_DET_GRAPH;
    else
      graphmode=STB_RED_GRAPH;
    
    mbkenv ();
    cnsenv ();
    yagenv (tas_yaginit);
    fclenv ();
    elpenv ();
    mccenv ();
    tlcenv ();
    rcnenv ();
    rcx_env ();
    stb_env ();
    stb_ctk_env ();
    tas_setenv ();
    tas_version ();
    libenv ();
    tlfenv ();
    cbhenv ();
    ttvenv ();

    STB_CTK_VAR |= STB_CTX_REPORT;

    if (LATEST_DEBUGLIST) {
        stb_freestbdebuglist(LATEST_DEBUGLIST);
        LATEST_DEBUGLIST = NULL;
        LATEST_INPUT = NULL;
    }
    if ((stbfig=stb_getstbfig (ttvfig))) {
        stb_delstbfig (stbfig);
        
    }
        
    ttv_init_stm(ttvfig);

    ttv_setcachesize(STB_LIMITS,STB_LIMITL) ;
    TTV_MAX_SIG = TTV_ALLOC_MAX;
    
    return (stb_analysis (ttvfig, STB_ANALYSIS_VAR, graphmode, STB_MODE_VAR, STB_CTK_VAR));
}


chain_list *stb_GetErrorList (stbfig_list *stbfig, double margin, int nberror)
{
    long         minsetup  ;
    long         minhold  ;
    int          errnum;

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetErrorList");
    if (stbfig==NULL) return NULL;

    if (margin>1e-3) margin=1e-3;
    margin*=TTV_UNIT;
    return (stb_geterrorlist (stbfig, (long)(margin*1e12), nberror, &minsetup, &minhold, &errnum));
}

long stb_SUPMODE=0;

static stbdebug_list* CHECK_STBDEBUGLIST(ttvsig_list *input, ttvsig_list *output, long moreopt)
{
    stbdebug_list   *debug;
    stbfig_list     *stbfig;

    if (moreopt) stbforceupdate=1;
    if (!stbforceupdate && LATEST_DEBUGLIST && (LATEST_DEBUGLIST->SIG2 == output)) {
        if (input == NULL)
            return LATEST_DEBUGLIST;
        if (LATEST_INPUT) {
            if (LATEST_INPUT->SIG1 == input)
                return LATEST_INPUT;
        }
        for (debug=LATEST_DEBUGLIST ; debug ; debug=debug->NEXT) {
            if (debug->SIG1 == input) 
                break;     
        }
        LATEST_INPUT = debug;
        return LATEST_INPUT;
    }
    stbforceupdate=0;
    if ((stbfig = stb_getstbfig (ttv_GetTopTimingFigure(output->ROOT)))) {
        if (LATEST_DEBUGLIST) stb_freestbdebuglist(LATEST_DEBUGLIST);
        LATEST_DEBUGLIST = stb_debugstberror (stbfig, output, LONG_MAX, STB_COMPUTE_DEBUG_CHRONO|STBDEBUG_FLAG|stb_SUPMODE|moreopt) ;
        LATEST_DEBUGLIST = stb_sortstbdebug (LATEST_DEBUGLIST) ;
        LATEST_INPUT = NULL;
        if (input == NULL)
            return LATEST_DEBUGLIST;
        for (debug=LATEST_DEBUGLIST ; debug ; debug=debug->NEXT) {
            if (debug->SIG1 == input) 
                break;     
        }
        LATEST_INPUT = debug;
        return LATEST_INPUT;
    }
    return NULL;
}

chain_list *stb_GetSignalStabilityPaths(stbfig_list *stbfig, char *output) 
{
  stbdebug_list   *debug, *RET=NULL;
  chain_list      *chain=NULL;
  ttvsig_list     *output_sig;

  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetSignalStabilityPaths");
  if (output==NULL) return NULL;
  else
      output_sig = ttv_getsig(stbfig->FIG, vecname(output));
  if (stbfig) {
    RET = stb_debugstberror (stbfig, output_sig, LONG_MAX, STB_COMPUTE_DEBUG_CHRONO|STBDEBUG_FLAG) ;
    RET = stb_sortstbdebug (RET) ;
  }

  for (debug=RET; debug ; debug=debug->NEXT) 
    chain = addchain (chain, debug);
  
  return reverse (chain);
}

chain_list *stb_internal_GetSignalStabilityPaths(ttvsig_list *output, long moreopt) 
{
    stbdebug_list   *debug;
    chain_list      *chain=NULL;

    if (output==NULL) return NULL;

    for (debug=CHECK_STBDEBUGLIST(NULL, output, moreopt); debug ; debug=debug->NEXT) 
      chain = addchain (chain, debug);
    
    return reverse (chain);
}

ttvsig_list *stb_GetPathInputSignal(stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetPathInputSignal");
  if (path==NULL) return NULL;
  return path->SIG1;
}

ttvsig_list *stb_GetPathOutputSignal(stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetPathOutputSignal");
  if (path==NULL) return NULL;
  return path->SIG2;
}

char    *stb_GetClockName (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetClockName");
  if (path==NULL) return "";
  return path->CKNAME;
}

char    *stb_GetCommandName (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetCommandName");
  if (path==NULL) return "";
  return path->CMDNAME;
}

double stb_GetClockTime(stbdebug_list *path, int clock, char dir)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetClockTime");
    switch (clock) 
      {
      case INPUT_CLOCK    :
        if ((dir==RISING)&&(path->CKORGUP_MIN!=STB_NO_TIME))    return path->CKORGUP_MIN*1e-12/TTV_UNIT;
        else if ((dir==FALLING)&&(path->CKORGDN_MIN!=STB_NO_TIME))     return path->CKORGDN_MIN*1e-12/TTV_UNIT;
        break;
      case OUTPUT_CLOCK   :
        if ((dir==RISING)&&(path->CKUP_MIN!=STB_NO_TIME))   return path->CKUP_MIN*1e-12/TTV_UNIT;
        else if ((dir==FALLING)&&(path->CKDN_MIN!=STB_NO_TIME))    return path->CKDN_MIN*1e-12/TTV_UNIT;
        break;
      case MAIN_CLOCK     : 
        if ((dir==RISING)&&(path->CKREFUP_MIN!=STB_NO_TIME))    return path->CKREFUP_MIN*1e-12/TTV_UNIT;
        else if ((dir==FALLING)&&(path->CKREFDN_MIN!=STB_NO_TIME))    return path->CKREFDN_MIN*1e-12/TTV_UNIT;
        break;
    }
    return -1;
}

double stb_GetClockDelta(stbdebug_list *path, int clock, char dir)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetClockDelta");
  if (path==NULL) return -1;
    switch (clock) {
    case INPUT_CLOCK    :
      if ((dir==RISING)&&(path->CKORGUP_MAX!=STB_NO_TIME)&&(path->CKORGUP_MIN!=STB_NO_TIME))    return (path->CKORGUP_MAX - path->CKORGUP_MIN)*1e-12/TTV_UNIT;
      else if ((dir==FALLING)&&(path->CKORGDN_MAX!=STB_NO_TIME)&&(path->CKORGDN_MIN!=STB_NO_TIME))  return (path->CKORGDN_MAX - path->CKORGDN_MIN)*1e-12/TTV_UNIT;
      break;
    case OUTPUT_CLOCK   :
      if ((dir==RISING)&&(path->CKUP_MAX!=STB_NO_TIME)&&(path->CKUP_MIN!=STB_NO_TIME))    return (path->CKUP_MAX - path->CKUP_MIN)*1e-12/TTV_UNIT;
      else if ((dir==FALLING)&&(path->CKDN_MAX!=STB_NO_TIME)&&(path->CKDN_MIN!=STB_NO_TIME))  return (path->CKDN_MAX - path->CKDN_MIN)*1e-12/TTV_UNIT;
      break;
    case MAIN_CLOCK     :
      if ((dir==RISING)&&(path->CKREFUP_MAX!=STB_NO_TIME)&&(path->CKREFUP_MIN!=STB_NO_TIME))    return (path->CKREFUP_MAX - path->CKREFUP_MIN)*1e-12/TTV_UNIT;
      else if ((dir==FALLING)&&(path->CKREFDN_MAX!=STB_NO_TIME)&&(path->CKREFDN_MIN!=STB_NO_TIME))  return (path->CKREFDN_MAX - path->CKREFDN_MIN)*1e-12/TTV_UNIT;
      break;
    }
    return -1;
}

double stb_GetClockPeriod (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetClockPeriod");
  if (path==NULL) return -1;
  return path->PERIODE*1e-12/TTV_UNIT;
}

double stb_GetSetup (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetSetup");
  if (path==NULL || path->SETUP==STB_NO_TIME) return -1;
  return path->SETUP*1e-12/TTV_UNIT;
}

double stb_GetHold (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetHold");
  if (path==NULL || path->HOLD==STB_NO_TIME) return -1;
  return path->HOLD*1e-12/TTV_UNIT;
}

double stb_GetSetupMargin (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetSetupMargin");
  if (path==NULL || path->MARGESETUP==STB_NO_TIME) return -1;
  return path->MARGESETUP*1e-12/TTV_UNIT;
}

double stb_GetHoldMargin (stbdebug_list *path)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetHoldMargin");
  if (path==NULL || path->MARGEHOLD==STB_NO_TIME) return -1;
  return path->MARGEHOLD*1e-12/TTV_UNIT;
}

chain_list *stb_GetInputInstabilityRanges(stbdebug_list *path, char dir)
{
  chain_list      *chain=NULL;
  stbpair_list    *ppair, *head;
  
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetInputInstabilityRanges");
  if (path==NULL || path->CHRONO==NULL) return NULL;
  
  switch(tolower(dir))
  {
    case 'u': head=path->CHRONO->SIG1S_U; break;
    case 'd': head=path->CHRONO->SIG1S_D; break;
    case 'm':
    default: head=path->CHRONO->SIG1S; break;
  }
  
  for (ppair=head ; ppair ; ppair=ppair->NEXT)
    chain = addchain (chain,ppair);
  return reverse(chain);
}

chain_list *stb_GetOutputInstabilityRanges(stbdebug_list *path, char dir)
{
  chain_list      *chain=NULL;
  stbpair_list    *ppair, *head;
  
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetOutputInstabilityRanges");
  if (path==NULL || path->CHRONO==NULL) return NULL;
  
  switch(tolower(dir))
  {
    case 'u': head=path->CHRONO->SIG2S_U; break;
    case 'd': head=path->CHRONO->SIG2S_D; break;
    case 'm':
    default: head=path->CHRONO->SIG2S; break;
  }
  
  for (ppair=head ; ppair ; ppair=ppair->NEXT)
    chain = addchain (chain,ppair);
  return reverse(chain);
}

chain_list *stb_GetOutputSpecificationRanges(stbdebug_list *path, char dir)
{
  chain_list      *chain=NULL;
  stbpair_list    *ppair, *head;
  
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetOutputSpecificationRanges");
  if (path==NULL || path->CHRONO==NULL) return NULL;
  
  switch(tolower(dir))
  {
    case 'u': head=path->CHRONO->SPECS_U; break;
    case 'd': head=path->CHRONO->SPECS_D; break;
    case 'm':
    default: head=path->CHRONO->SPECS; break;
  }
  
  for (ppair=head ; ppair ; ppair=ppair->NEXT)
    chain = addchain (chain,ppair);
  return reverse(chain);
}

double stb_GetInstabilityRangeStart(stbpair_list *range)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetInstabilityRangeStart");
  if (range==NULL) return -1;
  return range->D*1e-12/TTV_UNIT;
}

double stb_GetInstabilityRangeEnd(stbpair_list *range)
{
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetInstabilityRangeEnd");
  if (range==NULL) return -1;
  return range->U*1e-12/TTV_UNIT;
}

void   stb_FreeStabilityFigure (stbfig_list *sf)
{
    stb_delstbfig (sf);
}

void   stb_FreeStabilityPathList (chain_list *path_list)
{
  if (path_list==NULL) return;

  if (LATEST_DEBUGLIST==path_list->DATA)
    {
      LATEST_DEBUGLIST=NULL;
      LATEST_INPUT = NULL;
    }

    stb_freestbdebuglist ((stbdebug_list *)path_list->DATA);
}

Property *stb_GetStabilityPathProperty (StabilityPath *path, char *property)
{
    char buf[256];

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_GetStabilityPathProperty");
    if (!strcasecmp (property, "SOURCE_SIG"))
        return addptype (NULL, TYPE_TIMING_SIGNAL, stb_GetPathInputSignal (path));

    if (!strcasecmp (property, "ERROR_SIG")) 
        return addptype (NULL, TYPE_TIMING_SIGNAL, stb_GetPathOutputSignal (path));

    if (!strcasecmp (property, "CK_NAME")) 
        return addptype (NULL, TYPE_CHAR, stb_GetClockName (path));

    if (!strcasecmp (property, "CK_PERIOD")) {
        sprintf (buf, "%g", stb_GetClockPeriod (path));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "SETUP")) {
        sprintf (buf, "%g", stb_GetSetup (path));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "HOLD")) {
        sprintf (buf, "%g", stb_GetHold (path));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "SETUP_MARGIN")) {
        sprintf (buf, "%g", stb_GetSetupMargin (path));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "HOLD_MARGIN")) {
        sprintf (buf, "%g", stb_GetHoldMargin (path));
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "CMD_NAME")) 
        return addptype (NULL, TYPE_CHAR, stb_GetCommandName (path));

    fprintf (stderr, "error: unknown property %s\n", property);
    return NULL;
}

stbfig_list *stb_LoadSwitchingWindows(ttvfig_list *tvf, char *filename)
{
  stbfig_list *stbfig;
  
  if (!tvf) {
    fprintf (stderr, "STB ERROR: NULL ttvfig\n");
    return NULL;
  }
    
  mbkenv ();
  cnsenv ();
  yagenv (tas_yaginit);
  fclenv ();
  elpenv ();
  mccenv ();
  tlcenv ();
  rcnenv ();
  rcx_env ();
  stb_env ();
  stb_ctk_env ();
  tas_setenv ();
  tas_version ();
  libenv ();
  tlfenv ();
  cbhenv ();
  ttvenv ();

  if (LATEST_DEBUGLIST) {
    stb_freestbdebuglist(LATEST_DEBUGLIST);
    LATEST_DEBUGLIST = NULL;
    LATEST_INPUT = NULL;
  }
  if ((stbfig=stb_getstbfig (tvf))) {
    stb_delstbfig (stbfig);    
  }

  stbfig=stb_parseSTO(tvf, filename);
  if (stbfig && (ttv_getloadedfigtypes(tvf) & TTV_FILE_CTX)==TTV_FILE_CTX)
    stbfig->CTKMODE|=STB_CTK;
  
  return stbfig;
}

double getsetupholdinstab(stbdebug_list *dbl, int setup, int dir)
{
  double dv;
  stbchrono_list  *chro;
  stbpair_list *instab;
  double datavalid=1;
 
  if (dbl==NULL) return 1;

  if ((chro = dbl->CHRONO)!=NULL)
    {
      
      if (dir==1) instab=chro->SIG2S_U;
      else if (dir==0) instab=chro->SIG2S_D;
      else instab=chro->SIG2S;
      
      if (setup) dv=-1;
      else dv=1;
      
      if (instab==NULL) dv=1;
      
      while (instab!=NULL)
        {
          if (setup && stb_GetInstabilityRangeEnd(instab)>dv) dv=stb_GetInstabilityRangeEnd(instab);
          else if (!setup && stb_GetInstabilityRangeStart(instab)<dv) dv=stb_GetInstabilityRangeStart(instab);
          instab=instab->NEXT;
        }
      
      datavalid=dv;
    }
  return datavalid;
}

double getsetuphold(ttvsig_list *sig, int setup, int dir, ttvsig_list *siginput, ttvevent_list *in_ev, ttvevent_list *cmd, SH_info *shi)
{
  stbdebug_list *dbl;
  chain_list *lst, *ch;
  double setuphold=-1;
  char input_ev, latch_ev;
  stbnode *sn;
  int i;
  long min=STB_NO_TIME;
  Setup_Hold_Computation_Detail_INFO *detail;

  if (shi!=NULL) shi->dbl=NULL;
  input_ev='?';
  latch_ev='?';

  sn=stb_getstbnode(sig->NODE);
  if (sn==NULL) sn=stb_getstbnode(sig->NODE+1);
  if (sn==NULL || (setup && (sn->FLAG & STB_NODE_NOSETUP)!=0)
      || (!setup && (sn->FLAG & STB_NODE_NOHOLD)!=0)) return setuphold;

  lst=stb_internal_GetSignalStabilityPaths(sig, setup?STB_COMPUTE_SETUP_ONLY:STB_COMPUTE_HOLD_ONLY);
  
  for (ch=lst; ch!=NULL; ch=ch->NEXT)
    {
      dbl=(stbdebug_list *)ch->DATA;
      if (siginput!=NULL && stb_GetPathInputSignal(dbl)!=siginput) continue;
      if (in_ev!=NULL && dbl->SIG1_EVENT!=in_ev) continue;
      if (cmd!=NULL && dbl->CMD_EVENT!=cmd) continue;

      if (dbl->SIG1_EVENT!=NULL)
        {
          if (dbl->SIG1_EVENT->TYPE & TTV_NODE_UP) input_ev='u';
          else input_ev='d';
        }
      else input_ev='?';

      for (i=0;i<2;i++)
        {
          if (dir==2 || dir==i)
            {
              if (setup) detail=&dbl->detail[i].setup;
              else detail=&dbl->detail[i].hold;
              if (detail->VALUE<min)
                {
                  min=detail->VALUE;
                  if (shi!=NULL)
                    {
                      shi->dbl=dbl;
                      shi->input_event=input_ev;
                      shi->latch_event=latch_ev;
                    }
                }
            }
        }
    }
  if (min!=STB_NO_TIME)
    setuphold=(min/TTV_UNIT)*1e-12;

  freechain(lst);
  return setuphold;
}


double __stb_GetSignalHold(ttvsig_list *tvs, char dir)
{
  long lastmode;
  if (tvs==NULL) return -1;
  lastmode=stbsetdebugflag(STB_DIFFERENTIATE_INPUT_EVENTS);
  if (tolower(dir)=='u') return getsetuphold(tvs, 0, 1, NULL, NULL, NULL, NULL);
  else if (tolower(dir)=='d') return getsetuphold(tvs, 0, 0, NULL, NULL, NULL, NULL);
  else if (dir=='?') return getsetuphold(tvs, 0, 2, NULL, NULL, NULL, NULL);
  else //avt_error("stbapi", 1, AVT_ERR, "stb_GetSignalHold: unknown direction '%c'\n", dir);
     avt_errmsg(STB_API_ERRMSG, "001", AVT_ERROR, dir);
  stbsetdebugflag(lastmode);
  return -1;
}

double __stb_GetSignalSetup(ttvsig_list *tvs, char dir)
{
  long lastmode;
  if (tvs==NULL) return -1;
  lastmode=stbsetdebugflag(STB_DIFFERENTIATE_INPUT_EVENTS);
  if (tolower(dir)=='u') return getsetuphold(tvs, 1, 1, NULL, NULL, NULL, NULL);
  else if (tolower(dir)=='d') return getsetuphold(tvs, 1, 0, NULL, NULL, NULL, NULL);
  else if (dir=='?') return getsetuphold(tvs, 1, 2, NULL, NULL, NULL, NULL);
  else //avt_error("stbapi", 1, AVT_ERR, "stb_GetSignalSetup: unknown direction '%c'\n", dir);
     avt_errmsg(STB_API_ERRMSG, "001", AVT_ERROR, dir);
  stbsetdebugflag(lastmode);
  return -1;
}

double stb_GetSetupSlack (stbfig_list *fig, char *signame, char dir)
{
    ttvsig_list *sig;

    if (!fig) return -1;
    sig = ttv_getsig (fig->FIG, vecname (signame));

    return __stb_GetSignalSetup (sig, dir);
}

double stb_GetHoldSlack (stbfig_list *fig, char *signame, char dir)
{
    ttvsig_list *sig;

    if (!fig) return -1;
    sig = ttv_getsig (fig->FIG, vecname (signame));

    return __stb_GetSignalHold (sig, dir);
}

void stb_UpdateSlacks (stbfig_list *stbfig)
{
  chain_list *chainsig, *chain;
  stbdebug_list *sdl;
  ttvsig_list *tvs;
  int oldprecisionlevel;

  if (!stbfig) return;

  oldprecisionlevel=ttv_SetPrecisionLevel(0);
  stbfig->STABILITYFLAG = STB_STABLE;
  chainsig = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B, NULL) ;
  for(chain = chainsig; chain ; chain = chain->NEXT)
  {
      tvs=(ttvsig_list*)chain->DATA;
      
      sdl = stb_debugstberror(stbfig, tvs, 0, STB_UPDATE_SETUP_HOLD);
      if(sdl){
          if((sdl->SETUP <= 0) || (sdl->HOLD <= 0))
              stbfig->STABILITYFLAG = STB_UNSTABLE;
          stb_freestbdebuglist(sdl);
      }
  }
  ttv_SetPrecisionLevel(oldprecisionlevel);
}

void stb_DriveReport (stbfig_list *stbfig, char *filename)
{
  int oldprecisionlevel;
  if (!stbfig) return;
  oldprecisionlevel=ttv_SetPrecisionLevel(0);
  stb_report(stbfig, filename);
  ttv_SetPrecisionLevel(oldprecisionlevel);
}

Property *stb_GetStabilityFigureProperty (StabilityFigure *sf, char *property)
{
    char buf[256];

    if (!sf) {
        sprintf (buf, "error_null_stability_figure");
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "TIMING_FIGURE")) 
        return addptype (NULL, TYPE_TIMING_FIGURE, sf->FIG);

    fprintf (stderr, "error: unknown property %s\n", property);
    return NULL;
}

double stb_synchronized_slopes_move(stbfig_list *ptstbfig, ttvevent_list *startnode, ttvevent_list *endclock)
{
  stbnode *node;
  char startphase, endphase;
  long period, dec;
  int i;
  if (getptype(endclock->ROOT->USER, TTV_SIG_CLOCK)==NULL) return 0;
  node=stb_getstbnode(endclock);
  endphase=node->CK->CKINDEX;
  period=node->CK->PERIOD;
  node=stb_getstbnode(startnode);
  if (getptype(startnode->ROOT->USER, TTV_SIG_CLOCK)!=NULL)
  {
    startphase=node->CK->CKINDEX;
  }
  else
  {
    for (i=0; i<node->NBINDEX && node->STBTAB[i]==NULL; i++) ;
    if (i>=node->NBINDEX) return 0;
    startphase=i;
  }
  dec=stb_synchronize_slopes(ptstbfig, startphase, endphase, period, 0);
  return _LONG_TO_DOUBLE(dec);
}
