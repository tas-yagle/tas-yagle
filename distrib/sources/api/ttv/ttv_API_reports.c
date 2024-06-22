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

#include AVT_H
#define API_USE_REAL_TYPES
#include "ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "ttv_API_display.h"
#include "ttv_API_util.h"
#include "../stb/stb_LOCAL.h"
#include "../genius/api_common_structures.h"
#include "../ctk/ctk_api_local.h"
#include "../ctk/ctk_API.h"

#define CTK_DIFF_MODE

//extern long ____stbpatchdelta(ttvpath_list *pth);
static int SIMULATE_MODE='n', TTV_API_NOSUPINFO_DETAIL=1 ;
static char *LP="";
double DISPLAY_time_unit=1e9, DISPLAY_capa_unit=1e12;
char *DISPLAY_time_string="ns", *DISPLAY_capa_string="pf", *DISPLAY_time_format="%.3f", *DISPLAY_signed_time_format="%+.3f", *DISPLAY_capa_format=NULL;
char DISPLAY_number_justify='r';
int DISPLAY_nodemode=1, DISPLAY_time_digits=-1, DISPLAY_hiderc=0;
char DISPLAY_errormode=1, probemode=1;
char DISPLAY_simdiffmode=0;
int ffdebug=0;

long detail_forced_mode=0;

typedef struct configentry {
  char *show;
  char *code;
  int column;
  int father;
  char *son[5];
} configentry;

char PL_CONFIG_SHOW[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

configentry PL_CONFIG[]=
  {
    {&PL_CONFIG_SHOW[COL_INDEX], "pl.index", COL_INDEX, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_STARTTIME], "pl.starttime", COL_STARTTIME, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_STARTSLOPE], "pl.startslope", COL_STARTSLOPE, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_PATHDELAY], "pl.pathdelay", COL_PATHDELAY, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_TOTALDELAY], "pl.totaldelay", COL_TOTALDELAY, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_DATALAG], "pl.datalag", COL_DATALAG, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_ENDSLOPE], "pl.endslope", COL_ENDSLOPE, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_STARTNODE], "pl.startnode", COL_STARTNODE, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&PL_CONFIG_SHOW[COL_ENDNODE], "pl.endnode", COL_ENDNODE, -1, {NULL, NULL, NULL, NULL, NULL}}
  };


char DT_CONFIG_SHOW[]={1,1,1,1,1, 1,1,1,1,1, 1, 1, 1, 1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 0,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1};

configentry DT_CONFIG[]=
  {
    {&DT_CONFIG_SHOW[COL_SIM_ACC], "dt.simacc", COL_SIM_ACC, GLOBAL_COL_SIM, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_SIM_DELTA], "dt.simdelta", COL_SIM_DELTA, GLOBAL_COL_SIM, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_SIM_SLOPE], "dt.simslope", COL_SIM_SLOPE, GLOBAL_COL_SIM, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_SIM_ERROR], "dt.simerror", COL_SIM_ERROR, GLOBAL_COL_SIM, {NULL, NULL, NULL, NULL, NULL}},

    {&DT_CONFIG_SHOW[COL_REF_ACC], "dt.refacc", COL_REF_ACC, GLOBAL_COL_REF, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_REF_DELTA], "dt.refdelta", COL_REF_DELTA, GLOBAL_COL_REF, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_REF_SLOPE], "dt.refslope", COL_REF_SLOPE, GLOBAL_COL_REF, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_REF_LAG_ACC], "dt.reflagacc", COL_REF_LAG_ACC, GLOBAL_COL_REF, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_REF_LAG_DELTA], "dt.reflagdelta", COL_REF_LAG_DELTA, GLOBAL_COL_REF, {NULL, NULL, NULL, NULL, NULL}},

    {&DT_CONFIG_SHOW[COL_CTK_ACC], "dt.ctkacc", COL_CTK_ACC, GLOBAL_COL_CTK, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_CTK_DELTA], "dt.ctkdelta", COL_CTK_DELTA, GLOBAL_COL_CTK, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_CTK_SLOPE], "dt.ctkslope", COL_CTK_SLOPE, GLOBAL_COL_CTK, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_CTK_LAG_ACC], "dt.ctklagacc", COL_CTK_LAG_ACC, GLOBAL_COL_CTK, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_CTK_LAG_DELTA], "dt.ctklagdelta", COL_CTK_LAG_DELTA,GLOBAL_COL_CTK, {NULL, NULL, NULL, NULL, NULL}},

    {&DT_CONFIG_SHOW[COL_CAPA], "dt.capa", COL_CAPA, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_NODETYPE], "dt.nodetype", COL_NODETYPE, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_NODENAME], "dt.nodename", COL_NODENAME, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_NETNAME], "dt.netname", COL_NETNAME, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_LINETYPE], "dt.linetype", COL_LINETYPE, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[COL_TRANSISTORS], "dt.transistors", COL_TRANSISTORS, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&DT_CONFIG_SHOW[INFO_CMD], "dt.clockinfo", INFO_CMD, -1, {NULL, NULL, NULL, NULL, NULL}},

    {&DT_CONFIG_SHOW[GLOBAL_COL_REF], "", GLOBAL_COL_REF, -1, {&DT_CONFIG_SHOW[COL_REF_ACC],
                                                               &DT_CONFIG_SHOW[COL_REF_DELTA],
                                                               &DT_CONFIG_SHOW[COL_REF_SLOPE],
                                                               NULL,
                                                               NULL}},

    {&DT_CONFIG_SHOW[GLOBAL_COL_REFLAG], "", GLOBAL_COL_REFLAG, -1, {&DT_CONFIG_SHOW[COL_REF_LAG_ACC],
                                                                     &DT_CONFIG_SHOW[COL_REF_LAG_DELTA],
                                                                     NULL,
                                                                     NULL,
                                                                     NULL}},

    {&DT_CONFIG_SHOW[GLOBAL_COL_CTK], "", GLOBAL_COL_CTK, -1, {&DT_CONFIG_SHOW[COL_CTK_ACC],
                                                               &DT_CONFIG_SHOW[COL_CTK_DELTA],
                                                               &DT_CONFIG_SHOW[COL_CTK_SLOPE],
                                                               NULL,
                                                               NULL}},
    
    {&DT_CONFIG_SHOW[GLOBAL_COL_CTKLAG], "", GLOBAL_COL_CTKLAG, -1, {&DT_CONFIG_SHOW[COL_CTK_LAG_ACC],
                                                                     &DT_CONFIG_SHOW[COL_CTK_LAG_DELTA],
                                                                     NULL,
                                                                     NULL,
                                                                     NULL}},

    {&DT_CONFIG_SHOW[GLOBAL_COL_SIM], "", GLOBAL_COL_SIM, -1, {&DT_CONFIG_SHOW[COL_SIM_ACC],
                                                               &DT_CONFIG_SHOW[COL_SIM_DELTA],
                                                               &DT_CONFIG_SHOW[COL_SIM_SLOPE],
                                                               &DT_CONFIG_SHOW[COL_SIM_ERROR],
                                                               NULL}}

  };

char STAB_CONFIG_SHOW[]={1,0,1,0,1, 1,1,1,1,1};

configentry STAB_CONFIG[]=
  {
    {&STAB_CONFIG_SHOW[COL_START], "stab.from", COL_START, -1, {NULL, NULL, NULL, NULL, NULL}},
    {&STAB_CONFIG_SHOW[COL_LATCH], "stab.thru", COL_LATCH, -1, {NULL, NULL, NULL, NULL, NULL}},
  };


PATH_MORE_INFO_TYPE PATH_MORE_INFO={0, {{0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0},
                                        {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}, {0, NULL, 0}}};

void ttv_DumpHeader(FILE *f, ttvfig_list *tvf)
{
  struct tm tms;
  char buf[1024], thedate[128];
  time_t mytime;

  if (tvf==NULL) return;

  tms.tm_sec=tvf->INFO->TTVSEC;
  tms.tm_min=tvf->INFO->TTVMIN;
  tms.tm_hour=tvf->INFO->TTVHOUR;
  tms.tm_mday=tvf->INFO->TTVDAY;
  tms.tm_mon=(tvf->INFO->TTVMONTH-1);
  tms.tm_year=tvf->INFO->TTVYEAR-1900;
  tms.tm_isdst=-1;

  mytime=mktime(&tms);

#ifdef Solaris
  ctime_r(&mytime, thedate, 52);
#else
  ctime_r(&mytime, thedate);
#endif
  thedate[strlen(thedate)-1] = '\0';
  sprintf(buf,
          "Timing figure: %s   generated on %s\n"
          "               supply: %g  temperature: %g\n"
          "               slope thresholds : %g / %g\n"
          "               delay threshold : %g\n",          
          tvf->INFO->FIGNAME, thedate,
          tvf->INFO->VDD, tvf->INFO->TEMP,
          tvf->INFO->STHLOW, tvf->INFO->STHHIGH,
          tvf->INFO->DTH
          );

  avt_printExecInfo(f, "#", buf, "");
}

static void settimeformat(int nb)
{
  char buf[20];
  
  sprintf(buf,"%%.%df",nb);
  DISPLAY_time_format=sensitive_namealloc(buf);
  sprintf(buf,"%%+.%df",nb);
  DISPLAY_signed_time_format=sensitive_namealloc(buf);
}

void _ttv_Board_SetValue(char *tab, int scol, Board *B, int col, char *val)
{
  if (tab[scol]) Board_SetValue(B, col, val);
}

void _ttv_Board_SetSize(char *tab, int scol, Board *B, int col, int size, char align)
{
  if (tab[scol]) Board_SetSize(B, col, size, align);
}

void _ttv_Board_SetSep(char *tab, int scol, Board *B, int col)
{
  if (tab[scol]) Board_SetSep(B, col);
}

void ttv_SetupReport(char *val)
{
  char buf[1024];
  char *tok, *c;

  strcpy(buf, val);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"ps")==0) 
        {
          DISPLAY_time_unit=1e12;
          DISPLAY_time_string="ps";
          if (DISPLAY_time_digits==-1) settimeformat(1);
        }
      else if (strcasecmp(tok,"ns")==0) 
        {
          DISPLAY_time_unit=1e9;
          DISPLAY_time_string="ns";
          if (DISPLAY_time_digits==-1) settimeformat(3);
        }
      else if (strcasecmp(tok,"pf")==0) {DISPLAY_capa_unit=1e12; DISPLAY_capa_string="pf";DISPLAY_capa_format=NULL;}
      else if (strcasecmp(tok,"ff")==0) {DISPLAY_capa_unit=1e15; DISPLAY_capa_string="ff";DISPLAY_capa_format="%.2f";}
      else if (strcasecmp(tok,"justifyright")==0) {DISPLAY_number_justify='r';}
      else if (strcasecmp(tok,"justifyleft")==0) {DISPLAY_number_justify='l';}
      else if (strcasecmp(tok,"onlynetname")==0) {DISPLAY_nodemode=0;}
      else if (strcasecmp(tok,"nodename")==0) {DISPLAY_nodemode=1;}
      else if (strcasecmp(tok,"hiderc")==0) {DISPLAY_hiderc=1;}
      else if (strcasecmp(tok,"showrc")==0) {DISPLAY_hiderc=0;}
      else if (strcasecmp(tok,"pathcomp")==0) DISPLAY_simdiffmode=1;
      else if (strcasecmp(tok,"!pathcomp")==0) DISPLAY_simdiffmode=0;
      else if (strcasecmp(tok,"!debug")==0) ffdebug=0;
      else if (strcasecmp(tok,"debug")==0) ffdebug=1;
      else if (strcasecmp(tok,"!supdetail")==0) TTV_API_NOSUPINFO_DETAIL=1;
      else if (strcasecmp(tok,"supdetail")==0) TTV_API_NOSUPINFO_DETAIL=0;
      else if (strcasecmp(&tok[1],"digits")==0) 
        {
          int nb=tok[0]-'0';
          if (nb<0 || nb>6)
            {
              nb=3;
              avt_errmsg(TTV_API_ERRMSG, "037", AVT_ERROR, nb);
//              avt_error("ttvapi", 502, AVT_ERR, "ttv_SetReportUnit: error, setting digits to %d\n", nb);
            }
          DISPLAY_time_digits=nb;
          settimeformat(DISPLAY_time_digits);
        }
      else if (strcasecmp(tok,"localerror")==0) {DISPLAY_errormode=0;}
      // ----- HIDDEN : FOR DEBUG PURPOSE ----
      else if (strcasecmp(tok,"probe-noprop")==0) {probemode=0;}
      else if (strcasecmp(tok,"probe-prop")==0) {probemode=1;}
      //----
      else //avt_error("ttvapi", 502, AVT_ERR, "ttv_SetReportUnit: unknown unit/setup '%s'\n", tok);
           avt_errmsg(TTV_API_ERRMSG, "038", AVT_ERROR, tok);
      
      tok=strtok_r(NULL, " ", &c);
    }
}

char *ttv_GetFullSignalName_COND(ttvfig_list *tf, ttvsig_list *sg)
{
  if (DISPLAY_nodemode) return ttv_GetFullSignalName(tf, sg);
  else return ttv_GetFullSignalNetName(tf, sg);
}

char *ttv_GetDetailSignalName_COND(ttvcritic_list *tc)
{
  if (DISPLAY_nodemode) return ttv_GetDetailNodeName(tc);
  else return ttv_GetDetailSignalName(tc);
}

void ttv_SetReportUnit(char *val)
{
  avt_errmsg(TTV_API_ERRMSG, "039", AVT_WARNING);
//  fprintf (stderr, "warning, ttv_SetReportUnit is obsolete, use ttv_SetupReport\n");
  ttv_SetupReport(val);
}

void ttvapi_setprefix(char *val)
{
  LP=val;
}

void ttvapi_setdislaytab(int nb)
{
  switch (nb)
  {
    case 0: LP=""; break;
    case 1: LP="    "; break;
    case 2: LP="        "; break;
    case 3: LP="            "; break;
    default:
    case 4: LP="                "; break;
  }
}

static char dirconv(char dir)
{
  if (dir=='u') return 'R';
  else if (dir=='d') return 'F';
  exit(5);
}

static int isclock(ttvsig_list *ts)
{
  if (getptype(ts->USER, TTV_SIG_CLOCK)!=NULL) return 1;
  return 0;
}

static char *typeconv(char *type, long sigtype, char *buf)
{
  int i=0;
  
  if (((sigtype & TTV_SIG_CZ) == TTV_SIG_CZ)
     || ((sigtype & TTV_SIG_CT) == TTV_SIG_CT))
   {
     buf[i++]='S'; buf[i++]='Z';
   }
  else
    if ((sigtype & TTV_SIG_CO) == TTV_SIG_CO) buf[i++]='S';

  if (strstr(type,"latch")!=NULL) buf[i++]='L';
  if (strstr(type,"precharge")!=NULL) buf[i++]='N';
  if (strstr(type,"flip-flop")!=NULL) buf[i++]='F';
  if (strstr(type,"breakpoint")!=NULL) buf[i++]='B';
  if (strstr(type,"command")!=NULL) buf[i++]='K';
  buf[i++]='\0';
  return buf;
}

char *FormaT(double val, char *buf, char *formatnum)
{
  if (formatnum==NULL)
    sprintf(buf,"%.3f",val);
  else
    sprintf(buf,formatnum,val);
  return buf;
}

void ttv_DisplayConfig(FILE *f, chain_list *tpl)
{
  ttvpath_list *tp;
  if (tpl==NULL || f==NULL) return;
  tp=(ttvpath_list *)tpl->DATA;

  if (ftello(f)==0)
    ttv_DumpHeader(f,tp->FIG);
            
//  avt_fprintf(f, "     Process :        ?\n");
  avt_fprintf(f, "     Voltage :        %gV\n", ttv_GetTimingFigureSupply(tp->FIG));
  avt_fprintf(f, "     Temperature :    %g degree C\n\n", ttv_GetTimingFigureTemperature(tp->FIG));
}

void ttv_DisplayActivateSimulation(char mode)
{
  mode=tolower(mode);
  if (mode!='y' && mode!='n') 
    {
      avt_errmsg(TTV_API_ERRMSG, "040", AVT_ERROR);
//      avt_error("ttvapi", 500, AVT_ERR, "ttv_DisplaySimulation: valid values for mode are 'y' or 'n'\n");
      return;
    }

  SIMULATE_MODE=mode;
}

void ttv_DisplayLegend(FILE *f)
{
  if (f==NULL) return;
  avt_fprintf(f, "Node type Index:\n");
  avt_fprintf(f, "   (C) : Clock node          (L) : Latch node         (F) : Flip-flop node\n");
  avt_fprintf(f, "   (B) : Breakpoint node     (K) : Latch command node (S) : Output connector node\n");
  avt_fprintf(f, "   (SZ): Output HZ connector (N) : Precharge node\n\n");
}




static void _ttv_DisplayPath(int number, ttvpath_list *tp, Board *b)
{
  double delay, slope, starttime, startslope, datalag;
  char *startsig, *endsig;
  ttvsig_list *sg;
  char startdir, enddir;
  char buf[20];

  sg=ttv_GetPathStartSignal(tp);
  startsig=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(sg), sg);

  sg=ttv_GetPathEndSignal(tp);
  endsig=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(sg), sg);

  startdir=dirconv(ttv_GetPathStartDirection(tp));
  enddir=dirconv(ttv_GetPathEndDirection(tp));
  delay=ttv_GetPathDelay(tp)*DISPLAY_time_unit;
  slope=ttv_GetPathSlope(tp)*DISPLAY_time_unit;
  startslope=ttv_GetPathStartSlope(tp)*DISPLAY_time_unit;
  starttime=ttv_GetPathStartTime(tp)*DISPLAY_time_unit;
  datalag=ttv_GetPathDataLag(tp)*DISPLAY_time_unit;

  sprintf(buf,"%d",number);
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_INDEX, b, COL_INDEX, buf);
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTTIME, b, COL_STARTTIME, FormaT(starttime, buf, DISPLAY_time_format));
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTSLOPE, b, COL_STARTSLOPE, FormaT(startslope, buf, DISPLAY_time_format));
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_PATHDELAY, b, COL_PATHDELAY, FormaT(delay, buf, DISPLAY_time_format));
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_TOTALDELAY, b, COL_TOTALDELAY, FormaT(delay+datalag, buf, DISPLAY_time_format));
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_DATALAG, b, COL_DATALAG, FormaT(datalag, buf, DISPLAY_time_format));
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_ENDSLOPE, b, COL_ENDSLOPE, FormaT(slope, buf, DISPLAY_time_format));

  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE, startsig);
  sprintf(buf,"(%c)",startdir);
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE_DIR, buf);

  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE, endsig);

  buf[0]='(';
  buf[1]=enddir;
  if (ttv_PathIsHZ(tp)) {strcpy(&buf[2],"Z)");}
  else {strcpy(&buf[2],")"); }

  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE_DIR, buf);
}

void ttv_DisplayPathList(FILE *f, chain_list *tpl)
{
  ttvpath_list *tp;
  int count=1;
  Board *b;

  if (f==NULL) return;

  b=Board_CreateBoard();

  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_INDEX, b, COL_INDEX, 5, 'r');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTTIME, b, COL_STARTTIME, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTSLOPE, b, COL_STARTSLOPE, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_PATHDELAY, b, COL_PATHDELAY, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_TOTALDELAY, b, COL_TOTALDELAY, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_DATALAG, b, COL_DATALAG, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_ENDSLOPE, b, COL_ENDSLOPE, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE, 20, 'l');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE_DIR, 3, 'l');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE, 20, 'l');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE_DIR, 3, 'l');

  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_STARTTIME, b, COL_PATH_SEP0);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_STARTSLOPE, b, COL_PATH_SEP1);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_PATHDELAY, b, COL_PATH_SEP2);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_TOTALDELAY, b, COL_PATH_SEP3);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_DATALAG, b, COL_PATH_SEP4);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_ENDSLOPE, b, COL_PATH_SEP5);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_PATH_SEP6);
  _ttv_Board_SetSep(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_PATH_SEP7);


  Board_NewLine(b);
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_INDEX, b, COL_INDEX, "Path");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTTIME, b, COL_STARTTIME, "Start");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTSLOPE, b, COL_STARTSLOPE, "Start");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_PATHDELAY, b, COL_PATHDELAY, "Path");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_TOTALDELAY, b, COL_TOTALDELAY, "Total");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_DATALAG, b, COL_DATALAG, "Data");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_ENDSLOPE, b, COL_ENDSLOPE, "Ending");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE, "Start");

  Board_NewLine(b);

  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTTIME, b, COL_STARTTIME, "time");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTSLOPE, b, COL_STARTSLOPE, "slope");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_PATHDELAY, b, COL_PATHDELAY, "delay");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_TOTALDELAY, b, COL_TOTALDELAY, "delay");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_DATALAG, b, COL_DATALAG, "lag");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_ENDSLOPE, b, COL_ENDSLOPE, "slope");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_STARTNODE,b, COL_STARTNODE, "From_node");
  _ttv_Board_SetValue(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE, "To_node");

  Board_NewSeparation(b);

  while (tpl!=NULL)
    {
      tp=(ttvpath_list *)tpl->DATA;
      Board_NewLine(b);
      _ttv_DisplayPath(count, tp, b);
      count++;
      tpl=tpl->NEXT;
    }

  Board_Display(f, b, LP);
  Board_FreeBoard(b);

  avt_fprintf(f, "\n");
  fflush(f);
}
/*
void ttv_DisplayPath(FILE *f, int number, ttvpath_list *tp)
{
  Board *b;

  if (f==NULL) return;

  b=Board_CreateBoard();

  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_INDEX, b, COL_INDEX, 5, 'r');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTTIME, b, COL_STARTTIME, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTSLOPE, b, COL_STARTSLOPE, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_PATHDELAY, b, COL_PATHDELAY, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_TOTALDELAY, b, COL_TOTALDELAY, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_DATALAG, b, COL_DATALAG, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_ENDSLOPE, b, COL_ENDSLOPE, 7, DISPLAY_number_justify);
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE, 20, 'l');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_STARTNODE, b, COL_STARTNODE_DIR, 3, 'l');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE, 20, 'l');
  _ttv_Board_SetSize(PL_CONFIG_SHOW, COL_ENDNODE, b, COL_ENDNODE_DIR, 3, 'l');

  _ttv_DisplayPath(number, tp, b);

  Board_Display(f, b, LP);
  Board_FreeBoard(b);
}
*/
static void checksetup(configentry *configtab, int nb, char *tok, int mode)
{
  int i, j;
  for (i=0; i<nb; i++)
    {
      if (strlen(configtab[i].code)==0)
        {
          *configtab[i].show=0;
          for (j=0; j<5 && configtab[i].son[j]!=NULL; j++)
            if (*(configtab[i].son[j])) {*configtab[i].show=1; break;}
        }
      else if (strcasecmp(tok,"all")==0 || mbk_TestREGEX(configtab[i].code, tok))
        {
          if (mode==0) *configtab[i].show=1;
          else *configtab[i].show=0;
        }
    }
}
  
void __ttv_DisplayPathDetailShowColumn(char *conf, int mode)
{
  char *c, *tok;
  char buf[1024];
  char oldsens;

  oldsens=CASE_SENSITIVE;
  CASE_SENSITIVE='N';


  strcpy(buf, conf);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"ref")==0) tok="dt.ref*";
      if (strcasecmp(tok,"reflag")==0) tok="dt.reflag*";
      if (strcasecmp(tok,"ctk")==0) tok="dt.ctk*";
      if (strcasecmp(tok,"ctklag")==0) tok="dt.ctklag*";
      if (strcasecmp(tok,"sim")==0) tok="dt.sim*";
      if (strcasecmp(tok,"trans")==0) tok="dt.transistors";
      if (strcasecmp(tok,"cmdinfo")==0) tok="dt.clockinfo";
//      if (strcasecmp(tok,"all")==0) tok="*";

      checksetup(PL_CONFIG, sizeof(PL_CONFIG)/sizeof(*PL_CONFIG), tok, mode);
      checksetup(DT_CONFIG, sizeof(DT_CONFIG)/sizeof(*DT_CONFIG), tok, mode);
      checksetup(STAB_CONFIG, sizeof(STAB_CONFIG)/sizeof(*STAB_CONFIG), tok, mode);
//      checksetup(C2L_CONFIG, sizeof(C2L_CONFIG)/sizeof(*C2L_CONFIG), tok, mode);

      tok=strtok_r(NULL, " ", &c);
    }

  CASE_SENSITIVE=oldsens;
}

void ttv_DisplayPathDetailShowColumn(char *conf)
{
  __ttv_DisplayPathDetailShowColumn(conf, 0);
}

void ttv_DisplayPathDetailHideColumn(char *conf)
{
  __ttv_DisplayPathDetailShowColumn(conf, 1);
}


double stb_periodmove(char dir, stbdebug_list *dbl)
{
  double val, v;
  char tdir='u';
  if (dbl==NULL) return 0;
  val=dbl->UPDELTA*1e-12/TTV_UNIT;
  v=dbl->DOWNDELTA*1e-12/TTV_UNIT;
  if (v>val) val=v, tdir='d';
  if (tdir!=dir) return val;
  return 0;
}

double stb_getperiodmove(char dir, stbdebug_list *dbl)
{
  if (dbl==NULL) return 0;
  if (dir=='u') return dbl->UPDELTA*1e-12/TTV_UNIT;
  if (dir=='d') return dbl->DOWNDELTA*1e-12/TTV_UNIT;
  return 0;
}

void ttv_GetNodeSpecout(ttvevent_list *latch, double unit, char *unitformat, char *res, stbdebug_list *dbl, ttvpath_list *tp, int setuphold, long dec)
{
  stbnode *node;
  double u=-1, d=-1;
  char bufu[64], bufd[64];
  stbpair_list *instab, *originstab;
  char index;
  long period;

  node=stb_getstbnode(latch);

  strcpy(res,"");

  if (node!=NULL && node->SPECOUT!=NULL)
    {
      if (node->CK!=NULL)
        {
          index = node->CK->CKINDEX;
          period = node->CK->PERIOD;
        }
      else
        {
          index = STB_NO_INDEX;
          period = STB_NO_TIME;
        }
      
      originstab = stb_nodeglobalpair (node->SPECOUT, period, node->NBINDEX, index);
      
      if (originstab!=NULL)
        {
          instab=originstab;
          if (instab!=NULL) d=instab->D;
          while (instab!=NULL)
            u=instab->U, instab=instab->NEXT;

          u=_LONG_TO_DOUBLE(u+dec);
          d=_LONG_TO_DOUBLE(d+dec);
          sprintf(bufu, unitformat, u*unit);
          sprintf(bufd, unitformat, d*unit);
          
          if (setuphold)
            sprintf(res,"Must be stable after %s", bufu);
          else
            sprintf(res,"Must be stable before %s", bufd);

          stb_freestbpair (originstab);
        }
    }
}


void ttv_GetNodeClock(ttvevent_list *tve, ttvevent_list *latch, double unit, char *unitstring, char *unitformat, char *res, stbdebug_list *dbl, ttvpath_list *tp)
{
  stbnode     *node;
  stbck *clock;
  char bufrise[64], buffall[64], bufperiod[64], format[64];
  double risemin, risemax, fallmin, fallmax, per;
  int inverted;
  char *rise="RISE", *fall="FALL";
  stbfig_list *sb;

  node=stb_getstbnode(latch);
  strcpy(res,"");
  if (node!=NULL)
    {
      clock=node->CK;
      sb= stb_getstbfig(ttv_GetTopTimingFigure(tve->ROOT->ROOT));
      clock=stb_getgoodclock_and_status(sb, clock, tve, latch, &inverted);
      if (inverted) rise="FALL", fall="RISE";
      if (clock!=NULL)
        {
          per=(clock->PERIOD/TTV_UNIT)*1e-12;
          risemin=((clock->SUPMIN/TTV_UNIT)*1e-12);
          risemax=((clock->SUPMAX/TTV_UNIT)*1e-12);
          fallmin=((clock->SDNMIN/TTV_UNIT)*1e-12);
          fallmax=((clock->SDNMAX/TTV_UNIT)*1e-12);

          risemin+=stb_getperiodmove('u', dbl);
          risemax+=stb_getperiodmove('u', dbl);
          fallmin+=stb_getperiodmove('d', dbl);
          fallmax+=stb_getperiodmove('d', dbl);

          if ((tp->TYPE & TTV_FIND_MAX)!=TTV_FIND_MAX) 
            {
              // hold
              risemin-=per;
              risemax-=per;
              fallmin-=per;
              fallmax-=per;
            }

          risemin+=stb_periodmove('u', dbl);
          risemax+=stb_periodmove('u', dbl);
          fallmin+=stb_periodmove('d', dbl);
          fallmax+=stb_periodmove('d', dbl);
          
          risemin*=unit;
          risemax*=unit;
          fallmin*=unit;
          fallmax*=unit;

          if (clock->PERIOD!=STB_NO_TIME)
            {
              sprintf(format, "PERIOD = %s", unitformat);
              sprintf(bufperiod, format, per*unit);
            }
          else
            strcpy(bufperiod, "PERIOD = NONE");
          
          if (clock->SUPMIN!=STB_NO_TIME)
            {
              if (clock->SUPMIN==clock->SUPMAX)
                {
                  sprintf(format, "%s = %s", rise, unitformat);
                  sprintf(bufrise, format, risemin);
                }
              else
                {
                  sprintf(format, "%s = (%s:%s)", rise, unitformat, unitformat);
                  sprintf(bufrise, format, risemin, risemax);
                }
            }
          else
            sprintf(bufrise, "%s = NONE", rise);

          if (clock->SDNMIN!=STB_NO_TIME)
            {
              if (clock->SDNMIN==clock->SDNMAX)
                {
                  sprintf(format, "%s = %s", fall, unitformat);
                  sprintf(buffall, format, fallmin);
                }
              else
                {
                  sprintf(format, "%s = (%s:%s)", fall, unitformat, unitformat);
                  sprintf(buffall, format, fallmin, fallmax);
                }
            }
          else
            sprintf(buffall, "%s = NONE", fall);

          if (clock->SUPMIN<clock->SDNMIN)
            sprintf(res,"%s %s ", bufrise, buffall);
          else
            sprintf(res,"%s %s ", buffall, bufrise);

          strcat(res, bufperiod);
        }
    }
}

static void ttv_GetNodeIntrinsicInfo(ttvevent_list *tve, ttvevent_list *latch, double unit, char *unitstring, char *unitformat, char *res, int access)
{
  stbnode     *node;
  stbck *clock;
  char bufrise[64], format[64];
  int inverted;
  stbfig_list *sb;

  node=stb_getstbnode(latch);
  strcpy(res,"");
  if (node!=NULL)
    {
      clock=node->CK;
      sb= stb_getstbfig(ttv_GetTopTimingFigure(tve->ROOT->ROOT));
      clock=stb_getgoodclock_and_status(sb, clock, tve, latch, &inverted);
      if (clock!=NULL)
        {
          if (access)
            {
              if (clock->ACCESSMIN!=STB_NO_TIME)
                {
                  sprintf(format, "  ACCESS_MIN = %s", unitformat);
                  sprintf(bufrise, format, ((clock->ACCESSMIN/TTV_UNIT)*1e-12)*unit);
                  strcat(res, bufrise);
                }
              if (clock->ACCESSMAX!=STB_NO_TIME)
                {
                  sprintf(format, "  ACCESS_MAX = %s", unitformat);
                  sprintf(bufrise, format, ((clock->ACCESSMAX/TTV_UNIT)*1e-12)*unit);
                  strcat(res, bufrise);
                }
            }
          else
            {
              if (clock->HOLD!=STB_NO_TIME)
                {
                  sprintf(format, "  HOLD_MARGIN = %s", unitformat);
                  sprintf(bufrise, format, ((clock->HOLD/TTV_UNIT)*1e-12)*unit);
                  strcat(res, bufrise);
                }       
              if (clock->SETUP!=STB_NO_TIME)
                {
                  sprintf(format, "  SETUP_MARGIN = %s", unitformat);
                  sprintf(bufrise, format, ((clock->SETUP/TTV_UNIT)*1e-12)*unit);
                  strcat(res, bufrise);
                }       
            }
        }
    }
}

int stb_fill_noise_on_detail(ttvfig_list *tvf, chain_list *detail)
{
  stbfig_list *sb;
  stb_ctk_stat *stat;
  ttvcritic_list *tc;
  PATH_NOISE_TYPE *pnt;
  int idx;

  if (tvf==NULL || (sb=stb_getstbfig(tvf))==NULL) return 0;  
  if ((stat = stb_ctk_get_stat(sb))==NULL) return 0;
  while (detail!=NULL)
    {
      tc=(ttvcritic_list *)detail->DATA;
      idx=ctk_GetCtkStatNodeFromEvent(sb, tc->NODE);
      if (idx>0)
        {
          idx--;
          pnt=(PATH_NOISE_TYPE *)mbkalloc(sizeof(PATH_NOISE_TYPE));
          pnt->N=stb_ctk_get_score_noise( &stat->TAB[idx] );
          pnt->I=stb_ctk_get_score_interval( &stat->TAB[idx] );
          pnt->A=stb_ctk_get_score_activity( &stat->TAB[idx] );
          pnt->T=stb_ctk_get_score_total( &stat->TAB[idx] );
          pnt->C=stb_ctk_get_score_ctk( &stat->TAB[idx] );
          pnt->P=stb_ctk_get_score_probability( &stat->TAB[idx] );
          tc->USER=addptype(tc->USER, PATH_NOISE_PTYPE, pnt);
        }
      detail=detail->NEXT;
    }
  return 1;
}

void stb_free_noise_on_detail(chain_list *detail)
{
  ptype_list *pt;
  ttvcritic_list *tc;
  while (detail!=NULL)
    {
      tc=(ttvcritic_list *)detail->DATA;
      if ((pt=getptype(tc->USER, PATH_NOISE_PTYPE))!=NULL)
        {
          mbkfree(pt->DATA);
          tc->USER=delptype(tc->USER, PATH_NOISE_PTYPE);
        }
      detail=detail->NEXT;
    }  
}

void ttv_DisplayCompletePathDetail(FILE *f, int num, ttvpath_list *tp, chain_list *detail)
{
  chain_list *detailbase;
  ttvcritic_list *tc, *tc1;
  double capa;
  double accsim=0, slopesim, deltasim;
  double accref=0, sloperef, deltaref;
  double accctk=0, slopectk, deltactk;
  double acclag=0, deltalag;
  double accreflag=0, deltareflag, start=0, error;
  char nodetype[20], *nodename, *netname, dir, *delaytype="";
  char *prevnetname=NULL, prevdir, ztag, *acclabel="Acc ", *deltalabel="Delta", *linelabel="Line";
  char buf[4096], buf1[20], format[256], *colcode="";
  int simerror=0;
  int hidereflag=(DT_CONFIG_SHOW[GLOBAL_COL_REFLAG]+1)&1, hidectklag=(DT_CONFIG_SHOW[GLOBAL_COL_CTKLAG]+1)&1, hidectk=(DT_CONFIG_SHOW[GLOBAL_COL_CTK]+1)&1, hideref=(DT_CONFIG_SHOW[GLOBAL_COL_REF]+1)&1, hidesim=(DT_CONFIG_SHOW[GLOBAL_COL_SIM]+1)&1;
  int tm, mergerc=0, addc=0, i, split=0, ideal_clock_path=1, has_stat;
  chain_list *trlist, *clock_tree;
  ttvevent_list *tve;
  char searchtype;
  ptype_list *pt=NULL, *pt_debug=NULL, *pt_more=NULL, *ptype, *idealptype=NULL;
  Board *b;
  double errorref, simval_cmp, tasval_cmp, start_cmp;
  stb_clock_latency_information *scli;
  stbfig_list *sb=NULL;
  ttv_line_ssta_info *tlsi=NULL;

  if (f==NULL) return;

  if (tp!=NULL && tp->FIG!=NULL)
    {
      sb=stb_getstbfig(tp->FIG);  
      tlsi=ttv_ssta_get_ssta_info(tp->FIG);
    }

  if (tp!=NULL)
    {
      pt=getptype(tp->USER, SLACK_PTYPE);
      pt_debug=getptype(tp->USER, DEBUG_PTYPE);
      pt_more=getptype(tp->USER, MORE_PTYPE);
      idealptype=getptype(tp->USER, STB_IDEAL_CLOCK);
    }

  if (num>=0) avt_fprintf(f, "%sPath (%d) : ", LP, num);

  if ((detail_forced_mode & NOSUPINFO_DETAIL)==0)
    {
      if (pt==NULL)
        {
          if (tp!=NULL && tp->LATCH!=NULL && ttv_GetPathDataLag(tp)!=0)
            {
              avt_fprintf(f, " %sData LAG on latch %s is %s", delaytype, tp->LATCH->ROOT->NAME, FormaT(ttv_GetPathDataLag(tp)*DISPLAY_time_unit, buf, DISPLAY_time_format));
              delaytype="// ";
            }
          if ((tve=ttv_GetPathAccessLatchCommand(tp))!=NULL)
            {
              avt_fprintf(f, " %sPath Access Command %s(%c)", delaytype, ttv_GetFullSignalName_COND(tp->FIG, tve->ROOT), dirconv(ttv_GetTimingEventDirection(tve)));
              delaytype="// ";
            }
          if (strcmp(delaytype,"")!=0 || num>=0) avt_fprintf(f, "\n\n");
          else avt_fprintf(f, "\n");
        }
      else
        {
          if (num>=0)
            {
              avt_fprintf(f, " Slack of %s", FormaT((((long)pt->DATA)*1e-13)*DISPLAY_time_unit, buf, DISPLAY_time_format));
              avt_fprintf(f, "\n",LP);
            }
        }
    }
  else if (num>=0) avt_fprintf(f, "\n");

  detailbase=detail;

  if (detail==NULL) return;

  has_stat=stb_fill_noise_on_detail(tp!=NULL?tp->FIG:NULL, detailbase);

  tc=(ttvcritic_list *)detail->DATA;
  if (ttv_GetDetailSimDelay(tc)==-1) hidesim=1;
  if (tp==NULL) split=0;
  else
    {
     // tm=(int)((ttv_GetPathDelay(tp)-ttv_GetPathRefDelay(tp))*1e12);
     if ((sb!=NULL && sb->CTKMODE & STB_CTK)/* || (tp!=NULL && (ttv_getloadedfigtypes(tp->FIG) & TTV_FILE_CTX)==TTV_FILE_CTX)*/) split=1;
     else split=0;
    }

  if (tp==NULL || ttv_GetPathDataLag(tp)==0) { hidectklag=1; hidereflag=1; }
  if (!split) hideref=1, hidereflag=1;
#ifdef CTK_DIFF_MODE
  else hidereflag=0;
#endif
  b=Board_CreateBoard();

 if (detail!=NULL && tp!=NULL)
   clock_tree=get_clock_latency_tree(tp->NODE, tp->TYPE, NULL);
 else
   clock_tree=NULL;

 if (clock_tree!=NULL) acclabel="Time ", deltalabel="Latency", linelabel="Period";

  if (DT_CONFIG_SHOW[COL_LINETYPE]==0 || DISPLAY_hiderc) mergerc=1;

  if (!hidesim)
    {
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_SIM_ACC, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_SIM_DELTA, b, COL_SIM_DELTA, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_SIM_SLOPE, b, COL_SIM_SLOPE, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_SIM_ERROR, b, COL_SIM_ERROR, 5, 'r');
      _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_DET_SEP_SIM);
    }

  if (!hideref)
    {
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_REF_ACC, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_REF_DELTA, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_SLOPE, b, COL_REF_SLOPE, 7, DISPLAY_number_justify);
      _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_DET_SEP_REF);
    }
  if (!hidereflag)
    {
#ifndef CTK_DIFF_MODE
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, 7, DISPLAY_number_justify);
#else
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, 5, 'r');
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, 5, 'r');
#endif
      _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_DET_SEP_REF_LAG);
    }
  
  if (!hidectk)
    {
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_CTK_SLOPE, b, COL_CTK_SLOPE, 7, DISPLAY_number_justify);
      _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_DET_SEP_CTK);
    }
  if (!hidectklag)
    {
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, 7, DISPLAY_number_justify);
      _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, 7, DISPLAY_number_justify);
      _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_DET_SEP_CTK_LAG);
    }
  
  _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_CAPA, b, COL_CAPA, 5, DISPLAY_number_justify);
  _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_NODETYPE, b, COL_NODETYPE, 4, 'r');
  _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_NODENAME, b, COL_NODENAME, 20, 'l');
  _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_NETNAME, b, COL_NETNAME, 20, 'l');
  _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, 4, 'r');
  if (!ffdebug)
    _ttv_Board_SetSize(DT_CONFIG_SHOW, COL_TRANSISTORS, b, COL_TRANSISTORS, 9, 'l');
  else
    Board_SetSize(b, COL_TRANSISTORS, 9, 'l');


  _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_CAPA, b, COL_DET_SEP_CAPA);
  _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_NODETYPE, b, COL_DET_SEP_NODETYPE);
  _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_NODENAME, b, COL_DET_SEP_NODENAME);
  _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_NETNAME, b, COL_DET_SEP_NETNAME);
  _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_DET_SEP_LINETYPE);
  if (!ffdebug)
    _ttv_Board_SetSep(DT_CONFIG_SHOW, COL_TRANSISTORS, b, COL_DET_SEP_TRANSISTORS);
  else
    Board_SetSep(b, COL_DET_SEP_TRANSISTORS);

  if (has_stat)
    {
      Board_SetSize(b, COL_CTK_SCORE_TOT, 2, 'r');
      Board_SetSize(b, COL_CTK_SCORE_NOISE, 2, 'r');
      Board_SetSize(b, COL_CTK_SCORE_CTK, 2, 'r');
      Board_SetSize(b, COL_CTK_SCORE_ACTIV, 2, 'r');
      Board_SetSize(b, COL_CTK_SCORE_INTERV, 2, 'r');
      Board_SetSize(b, COL_CTK_SCORE_PROBA, 2, 'r');
    }

  if (tlsi!=NULL)
    {
      Board_SetSize(b, COL_SSTA_MIN, 7, 'r');
      Board_SetSize(b, COL_SSTA_MOY, 7, 'r');
      Board_SetSize(b, COL_SSTA_MAX, 7, 'r');
      Board_SetSize(b, COL_SSTA_SIGMA, 5, 'r');
    }

  Board_NewLine(b);
  
  Board_SetValue(b, COL_SSTA_MIN, "éSSTA");
  Board_SetValue(b, COL_SSTA_MOY, "ç");
  Board_SetValue(b, COL_SSTA_MAX, "èInformation");
  Board_SetValue(b, COL_SSTA_SIGMA, "Sigma");
  
  sprintf(buf, "èDelay");
  sprintf(buf1, "éDelay[%s]", DISPLAY_time_string);
      
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_SIM_ACC, "éSim");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_DELTA, b, COL_SIM_DELTA, buf);
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_SLOPE, b, COL_SIM_SLOPE, "Sim_R/F");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ERROR, b, COL_SIM_ERROR, "% ");
  
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_REF_ACC, "éDelay");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_REF_DELTA, "è");

  if (split)
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, "éCtk");
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, buf);
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_SLOPE, b, COL_CTK_SLOPE, "Ctk_R/F");
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, "éCtk");
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, "èLAG");
    }
  else
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_CTK_ACC, "éDelay");
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_CTK_DELTA, "è");
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_CTK_LAG_ACC, "éLAG");
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_CTK_LAG_DELTA, "è");
    }
      
//  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODETYPE, b, COL_NODETYPE, "Node");
//  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, "Line");
//  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_TRANSISTORS, b, COL_TRANSISTORS, "Switching");

#ifndef CTK_DIFF_MODE
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, "éLAG");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, "è");
#else
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, "éCtk effect \%");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, "è");
#endif

  sprintf(buf, "time[%s]", DISPLAY_time_string);
  sprintf(buf1, "Cap[%s]", DISPLAY_capa_string);

  Board_SetValue(b, COL_CTK_SCORE_TOT, "éCTK");
  Board_SetValue(b, COL_CTK_SCORE_NOISE, "çScores");
  Board_SetValue(b, COL_CTK_SCORE_CTK, "ç");
  Board_SetValue(b, COL_CTK_SCORE_ACTIV, "ç");
  Board_SetValue(b, COL_CTK_SCORE_INTERV, "ç");
  Board_SetValue(b, COL_CTK_SCORE_PROBA, "è");

  
  Board_NewLine(b);

  Board_SetValue(b, COL_SSTA_MIN, "Min");
  Board_SetValue(b, COL_SSTA_MOY, "Mean");
  Board_SetValue(b, COL_SSTA_MAX, "Max");
  Board_SetValue(b, COL_SSTA_SIGMA, "Factor");

  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODENAME, b, COL_NODENAME, "Node_Name");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NETNAME, b, COL_NETNAME, "Net_Name");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CAPA, b, COL_CAPA, buf1);

  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_SIM_ACC, "Acc ");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_DELTA, b, COL_SIM_DELTA, "Delta");
//  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_SLOPE, b, COL_SIM_SLOPE, buf);
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_SLOPE, b, COL_SIM_SLOPE, "R/F  ");
  
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_REF_ACC, "Acc ");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_REF_DELTA, "Delta");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_SLOPE, b, COL_REF_SLOPE, "R/F  ");

  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, acclabel);
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, deltalabel);
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_SLOPE, b, COL_CTK_SLOPE, "R/F  ");
 
  if (!ffdebug)
    _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_TRANSISTORS, b, COL_TRANSISTORS, "Switching_transistors");
  else
    Board_SetValue(b, COL_TRANSISTORS, "DEBUG");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, linelabel);

  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, acclabel);
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, deltalabel);
#ifndef CTK_DIFF_MODE
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, "Acc ");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, "Delta");
#else
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, "Delay");
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, "Slope");
#endif
  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODETYPE, b, COL_NODETYPE, "Type");

  Board_SetValue(b, COL_CTK_SCORE_TOT, "T");
  Board_SetValue(b, COL_CTK_SCORE_NOISE, "N");
  Board_SetValue(b, COL_CTK_SCORE_CTK, "C");
  Board_SetValue(b, COL_CTK_SCORE_ACTIV, "A");
  Board_SetValue(b, COL_CTK_SCORE_INTERV, "I");
  Board_SetValue(b, COL_CTK_SCORE_PROBA, "P");

  Board_NewSeparation(b);

  if( DISPLAY_errormode==1 )
    {
/*      if (tp!=NULL)
        errorref=ttv_GetPathDelay(tp);
      else*/
        {
          detail=detailbase;
          errorref=0;
          while (detail!=NULL)
            {
              tc=(ttvcritic_list *)detail->DATA;
              if (ttv_GetDetailSimDelay(tc)!=-1)
                errorref+=ttv_GetDetailSimDelay(tc);
              //ttv_GetDetailRefDelay(tc);
              detail=detail->NEXT;
            }
        }
    }

  if (clock_tree!=NULL)
    {
      long lat=TTV_NOTIME, base=TTV_NOTIME;
      chain_list *cl;

      if (tp!=NULL) base=tp->DELAYSTART;
      else if (detailbase!=NULL) base=((ttvcritic_list *)detailbase->DATA)->DELAY;
      if (base!=TTV_NOTIME)
        {
          for (cl=clock_tree; cl!=NULL && cl->NEXT!=NULL; cl=cl->NEXT) ;
          if (cl!=NULL)
            base=base-((stb_clock_latency_information *)cl->DATA)->date;
        }
      else base=0;
      while (clock_tree!=NULL)
        {
          scli=(stb_clock_latency_information *)clock_tree->DATA;

          Board_NewLine(b);
          if (!hidectk)
            _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, FormaT(_LONG_TO_DOUBLE(scli->date+base)*DISPLAY_time_unit, buf, DISPLAY_time_format));
          if (!hidectklag)
            _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, FormaT(_LONG_TO_DOUBLE(scli->date+base)*DISPLAY_time_unit, buf, DISPLAY_time_format));
          sprintf(buf1, "%c ", dirconv(ttv_GetTimingEventDirection(scli->clock_event)));
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_SLOPE, b, COL_CTK_SLOPE, buf1);
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODENAME, b, COL_NODENAME, ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(scli->clock_event->ROOT), scli->clock_event->ROOT));
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NETNAME, b, COL_NETNAME, ttv_GetFullSignalNetName (ttv_GetSignalTopTimingFigure(scli->clock_event->ROOT), scli->clock_event->ROOT));  
          typeconv(ttv_GetSignalType(scli->clock_event->ROOT), scli->clock_event->ROOT->TYPE, buf);
          sprintf(nodetype, "(C%s)", buf);
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODETYPE, b, COL_NODETYPE, nodetype);  
          capa=ttv_GetSignalCapacitance(scli->clock_event->ROOT);
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CAPA, b, COL_CAPA, FormaT(capa*DISPLAY_capa_unit, buf, DISPLAY_capa_format)); 

          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, FormaT(_LONG_TO_DOUBLE(scli->period)*DISPLAY_time_unit, buf, DISPLAY_time_format));  

          if (lat!=TTV_NOTIME)
            {
              if (!hidectk)
                _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, FormaT(_LONG_TO_DOUBLE(lat)*DISPLAY_time_unit, buf, DISPLAY_time_format));
              if (!hidectklag)
                _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, FormaT(_LONG_TO_DOUBLE(lat)*DISPLAY_time_unit, buf, DISPLAY_time_format));
            }
          lat=scli->delay;
          mbkfree(scli);
          clock_tree=delchain(clock_tree, clock_tree);
        }

      
      Board_NewSeparation(b);
      Board_NewLine(b);
      acclabel="Acc ", deltalabel="Delta", linelabel="Line";
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, acclabel);
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, deltalabel);
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, acclabel);
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, deltalabel);
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, linelabel);

//      Board_NewSeparation(b);
    }

  tc1=NULL;
  detail=detailbase;
  while (detail!=NULL)
    {
      tc=(ttvcritic_list *)detail->DATA;
      
      nodename=ttv_GetDetailNodeName(tc);
      netname=ttv_GetDetailSignalName(tc);

      deltaref=ttv_GetDetailRefDelay(tc);
      sloperef=ttv_GetDetailRefSlope(tc);

      deltactk=ttv_GetDetailDelay(tc);
      slopectk=ttv_GetDetailSlope(tc);

      deltasim=ttv_GetDetailSimDelay(tc);
      slopesim=ttv_GetDetailSimSlope(tc);

      if (detail==detailbase)
        {
          if ((detail_forced_mode & RELATIVE_DETAIL)==0)
            {
              double stbdecal=0;
              stbdebug_list *dbl;
              if (pt_debug!=NULL)
                {
                  int dec;
                  Setup_Hold_Computation_Detail_INFO *detail_info;
                  dbl=(stbdebug_list *)pt_debug->DATA;
                  if (tp->ROOT->TYPE & TTV_NODE_UP) dec=1; else dec=0;
                  if (tp->TYPE & TTV_FIND_MAX) detail_info=&dbl->detail[dec].setup;
                  else detail_info=&dbl->detail[dec].hold;
                  if (detail_info->VALUE!=STB_NO_TIME) stbdecal=detail_info->datamoved+detail_info->misc+detail_info->mc_setup_period-detail_info->mc_hold_period;
                  stbdecal*=1e-12/TTV_UNIT;
                }
              deltaref=deltactk=deltasim=ttv_GetPathStartTime(tp)+stbdecal;
            }
          else
            deltaref=deltactk=deltasim=0;
        }

      if (deltasim==-1 || slopesim==-1) simerror=1;

      delaytype=ttv_GetDetailType(tc);

      if (tc1!=NULL)
        {
          deltaref+=ttv_GetDetailRefDelay(tc1);
          deltactk+=ttv_GetDetailDelay(tc1);
          deltasim+=ttv_GetDetailSimDelay(tc1);
        }

      if (mergerc && strcmp(delaytype,"rc")==0 && detail->NEXT!=NULL)
        {
          tc1=tc;
        }
      else tc1=NULL;

      if (tc1==NULL)
        {          

          dir=dirconv(ttv_GetDetailDirection(tc));
          capa=tc->CAPA*1e-15*DISPLAY_capa_unit;

          typeconv(ttv_GetDetailSignalType(tc), tc->SIGTYPE, buf);
          if (mergerc && detail->NEXT!=NULL && strcmp(ttv_GetDetailType((ttvcritic_list *)detail->NEXT->DATA),"rc")==0 && detail->NEXT->NEXT!=NULL)
            {
              typeconv(ttv_GetDetailSignalType((ttvcritic_list *)detail->NEXT->DATA), ((ttvcritic_list *)detail->NEXT->DATA)->SIGTYPE, buf1);
              strcat(buf,buf1);
            }

          if (tc->NODE_FLAG & TTV_NODE_FLAG_ISCLOCK) addc=1;
          if (tc->NODE_FLAG & TTV_NODE_FLAG_ISLATCH_ACCESS) { addc=0; ideal_clock_path=0; }
//          if ((tc->SIGTYPE & (TTV_SIG_L|TTV_SIG_R))!=0 && detail!=detailbase) addc=0;
          if (addc)
            sprintf(nodetype, "(C%s)", buf);
          else if (strcmp(buf,"")!=0)
            sprintf(nodetype, "(%s)", buf);
          else
            strcpy(nodetype,"");

          deltalag=deltactk+ttv_GetDetailDataLag(tc);
          deltareflag=deltaref+ttv_GetDetailDataLag(tc);

          if (addc && ideal_clock_path && (idealptype || (detail_forced_mode & IDEAL_CLOCK)!=0) && detail!=detailbase)
            deltactk=deltaref=deltalag=deltareflag=deltasim=0;

          accctk+=deltactk;
          accref+=deltaref;
          acclag+=deltalag;
          accreflag+=deltareflag;
          if (deltasim!=-1) accsim+=deltasim;

          if (detail==detailbase) 
            {
              start=deltaref;
              deltactk=0;
              deltaref=0;
              deltalag=0;
              deltasim=deltasim!=-1?0:-1;
              deltareflag=0;
            }

          if (strcmp(delaytype,"rc")!=0 && getenv("GATEMODE")) delaytype="gate";
/*            {
              char *gatename;
              int numinput;
              colcode="¤+";
              if (ttv_getgateinfo(((unsigned)tc->PROP)>>24, &gatename, &numinput))
                delaytype=gatename;
              if ((tc->SIGTYPE & TTV_SIG_L)!=0)
                {
                  if (tc->PROP & TTV_SIG_FLAGS_ISMASTER) delaytype="master";
                  else if (tc->PROP & TTV_SIG_FLAGS_ISSLAVE) delaytype="slave";
                }
            }
          else colcode="";*/

          if (ttv_DetailIsHZ(tc)) ztag='Z'; else ztag=' ';

          Board_NewLine(b);


          if (tlsi!=NULL)
            {
              ttv_line_delay_ssta_info *tldsi;
              int idx;
              if (tc->LINE!=NULL && (idx=ttv_ssta_getline_index(tc->LINE))!=-1)
                {
                  double val;
                  if (tp->TYPE & TTV_FIND_MAX)
                    tldsi=&tlsi[idx].delaymax;
                  else
                    tldsi=&tlsi[idx].delaymin;
                  Board_SetValue(b, COL_SSTA_MIN, FormaT(tldsi->min*DISPLAY_time_unit, buf, DISPLAY_time_format));
                  Board_SetValue(b, COL_SSTA_MOY, FormaT(tldsi->moy*DISPLAY_time_unit, buf, DISPLAY_time_format));
                  Board_SetValue(b, COL_SSTA_MAX, FormaT(tldsi->max*DISPLAY_time_unit, buf, DISPLAY_time_format));
                  val=sqrt(tldsi->var);
                  if (val>1e-15)
                    val=(ttv_GetDetailDelay(tc)-tldsi->moy)/val;
                  else
                    val=0;
                  sprintf(buf1, "%.1f", val);
                  Board_SetValue(b, COL_SSTA_SIGMA, buf1);
                }
            }

          if (!hidesim)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_SLOPE, b, COL_SIM_SLOPE, slopesim!=-1?FormaT(slopesim*DISPLAY_time_unit, buf, DISPLAY_time_format):"/");
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_DELTA, b, COL_SIM_DELTA, deltasim!=-1?FormaT(deltasim*DISPLAY_time_unit, buf, DISPLAY_time_format):"/");
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_SIM_ACC, !simerror?FormaT(accsim*DISPLAY_time_unit, buf, DISPLAY_time_format):"/");
          
              if (!simerror)
                {
                  if( DISPLAY_errormode==0) errorref=deltasim ;
  
                  if (!hidectk) error=(deltactk-deltasim)*100/errorref;
                  else error=(deltaref-deltasim)*100/errorref;
                  if (!finite(error)) strcpy(buf1,".....");
                  else if (fabs(error)>9999) sprintf(buf1,"¤6.....¤.%s", colcode);
                  else 
                    {
                      if (fabs(error)>=50) sprintf(buf1, "¤6%+.1f¤.%s", error, colcode);
                      else if (fabs(error)>10) sprintf(buf1, "¤4%+.1f¤.%s", error, colcode);
                      else sprintf(buf1, "%+.1f", error);
                    }
                }
              else
                sprintf(buf1, "%4s", "/ ");
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ERROR, b, COL_SIM_ERROR, buf1);
            }
          if (!hideref)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_REF_ACC, FormaT(accref*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_REF_DELTA, FormaT(deltaref*DISPLAY_time_unit, buf, DISPLAY_time_format));
              sprintf(buf1, "%s %c%c", FormaT(sloperef*DISPLAY_time_unit, buf, DISPLAY_time_format), dir, ztag);
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_SLOPE, b, COL_REF_SLOPE, buf1);
            }
          if (!hidereflag)
            {
#ifndef CTK_DIFF_MODE
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, FormaT(accreflag*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, FormaT(deltareflag*DISPLAY_time_unit, buf, DISPLAY_time_format));
#else
              if (detail!=detailbase)
              {
                error=(deltactk-deltaref)*100/deltaref;
                if (finite(error)) sprintf(buf1, "%+.1f", error);
                else sprintf(buf1, "....");
                _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, buf1);
                error=(slopectk-sloperef)*100/sloperef;
                if (finite(error)) sprintf(buf1, "%+.1f", error);
                else sprintf(buf1, "....");
                _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, buf1);
              }
#endif
            }

          if (!hidectk)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, FormaT(accctk*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, FormaT(deltactk*DISPLAY_time_unit, buf, DISPLAY_time_format));
              sprintf(buf1, "%s %c%c", FormaT(slopectk*DISPLAY_time_unit, buf, DISPLAY_time_format), dir, ztag);
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_SLOPE, b, COL_CTK_SLOPE, buf1);
            }
          if (!hidectklag)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, FormaT(acclag*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, FormaT(deltalag*DISPLAY_time_unit, buf, DISPLAY_time_format));          
            }

          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CAPA, b, COL_CAPA, FormaT(capa, buf, DISPLAY_capa_format));  

          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODETYPE, b, COL_NODETYPE, nodetype);  
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODENAME, b, COL_NODENAME, nodename);  
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NETNAME, b, COL_NETNAME, netname);  
          if (sb!=NULL)
          {
            char buf[30];
            char *tag0="", *tag="";
            if (stb_has_filter_directive(tc->NODE)) tag="[f] ";
            
            if (ffdebug && stb_getstbnode(tc->NODE)->CK!=NULL) tag0="*";

            sprintf(buf,"%s%s%s", tag0, tag, delaytype);
            _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, buf);  

            if (ffdebug)
            {
               long startmin, startmax, endmax, endmin, move;
               char filterstate;
               ttvevent_list *cs;
               char buf0[20], buf1[20], buf3[50];
               stbpair_list *pair, *opair;
               stbnode *node;
               strcpy(buf, "");
               node=stb_getstbnode(tc->NODE);
               pair=stb_globalstbtab(node->STBTAB,node->NBINDEX);
               pair=stb_globalstbpair(opair=pair);
               if (pair!=NULL)
               {
                 sprintf(buf3," STAB[%s,%s]", 
                                 FormaT(pair->D*1e-12/TTV_UNIT*DISPLAY_time_unit, buf0, DISPLAY_time_format),
                                 FormaT(pair->U*1e-12/TTV_UNIT*DISPLAY_time_unit, buf1, DISPLAY_time_format)
                                 );
                 strcat(buf,buf3);

               }
               stb_freestbpair(pair) ;
               stb_freestbpair(opair) ;
               if (stb_get_filter_directive_info(sb, tc->NODE, STB_NO_INDEX, &startmin, &startmax, &endmax, &endmin, &filterstate, &move, &cs, NULL))
               {
                 sprintf(buf3," (%c %s,%s)", (cs->TYPE & TTV_NODE_UP)!=0?'U':'D',
                                 FormaT(startmin*1e-12/TTV_UNIT*DISPLAY_time_unit, buf0, DISPLAY_time_format),
                                 FormaT(endmax*1e-12/TTV_UNIT*DISPLAY_time_unit, buf1, DISPLAY_time_format)
                                 );
                 strcat(buf,buf3);
               }
               Board_SetValue(b, COL_TRANSISTORS, buf);  
            }
          }
          else
            _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_LINETYPE, b, COL_LINETYPE, delaytype);  

          if (!ffdebug && DT_CONFIG_SHOW[COL_TRANSISTORS])
            {
              if (prevnetname!=NULL && tp!=NULL)
                {
                  chain_list *cl;
                  trlist=ttv_GetCrossedTransistorNames(tp->FIG, prevnetname, prevdir, netname, ttv_GetDetailDirection(tc), ztag=='Z'?1:0);
                  strcpy(buf,"");
                  for (cl=trlist; cl!=NULL; cl=cl->NEXT)
                    {
                      strcat(buf, (char *)cl->DATA);
                      if (cl->NEXT!=NULL) strcat(buf, ", ");
                    }
                  freechain(trlist);
                  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_TRANSISTORS, b, COL_TRANSISTORS, buf);  
                }

              prevnetname=netname;
              prevdir=ttv_GetDetailDirection(tc);
            }
        }

      if ((ptype=getptype(tc->USER, PATH_NOISE_PTYPE))!=NULL)
        {
          PATH_NOISE_TYPE *pnt;
          pnt=(PATH_NOISE_TYPE *)ptype->DATA;
          sprintf(buf,"%d%%", pnt->T);
          Board_SetValue(b, COL_CTK_SCORE_TOT, buf);
          sprintf(buf,"%d", pnt->N);
          Board_SetValue(b, COL_CTK_SCORE_NOISE, buf);
          sprintf(buf,"%d", pnt->C);
          Board_SetValue(b, COL_CTK_SCORE_CTK, buf);
          sprintf(buf,"%d", pnt->A);
          Board_SetValue(b, COL_CTK_SCORE_ACTIV, buf);
          sprintf(buf,"%d", pnt->I);
          Board_SetValue(b, COL_CTK_SCORE_INTERV, buf);
          sprintf(buf,"%d", pnt->P);
          Board_SetValue(b, COL_CTK_SCORE_PROBA, buf);
        }

      //if (tc1!=NULL) detail=detail->NEXT; // rc merged
      detail=detail->NEXT;
    }

  if (!hidesim) 
    {
      if (!hidectk) error=(accctk-accsim)*100/(accsim-start), tasval_cmp=accctk;
      else error=(accref-accsim)*100/(accsim-start), tasval_cmp=accref;
      simval_cmp=accsim;
      start_cmp=start;
    }

  if (PATH_MORE_INFO.enabled)
    {
      for (i=0; i<10 && PATH_MORE_INFO.add[i].label!=NULL; i++)
        {
          int ssim=0;
          deltaref=PATH_MORE_INFO.add[i].val;
          deltactk=PATH_MORE_INFO.add[i].val;
          deltasim=PATH_MORE_INFO.add[i].val;

          accctk+=deltactk;
          accref+=deltaref;
          acclag+=deltactk;
          accreflag+=deltaref;

          if (PATH_MORE_INFO.add[i].noacc) start+=PATH_MORE_INFO.add[i].val;
          if (!hidesim)
          {
             tasval_cmp+=PATH_MORE_INFO.add[i].val;
             if (DISPLAY_simdiffmode && strcmp(PATH_MORE_INFO.add[i].label,"[PATH MARGIN]")!=0)
             {
                simval_cmp+=PATH_MORE_INFO.add[i].val, ssim=1;
                accsim+=deltasim;
             }
          }

          Board_NewLine(b);

          if (!hidesim && ssim) 
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_SIM_ACC, FormaT(accsim*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_DELTA, b, COL_SIM_DELTA, FormaT(deltasim*DISPLAY_time_unit, buf, DISPLAY_signed_time_format));
            }
/*
          if (!hideref)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_REF_ACC, FormaT(accref*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_REF_DELTA, FormaT(deltaref*DISPLAY_time_unit, buf, DISPLAY_signed_time_format));
            }

          if (!hidereflag)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, FormaT(accreflag*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, FormaT(deltaref*DISPLAY_time_unit, buf, DISPLAY_signed_time_format));
            }*/
          if (!hidectk)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, FormaT(accctk*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, FormaT(deltactk*DISPLAY_time_unit, buf, DISPLAY_signed_time_format));
            }
          if (!hidectklag)
            {
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, FormaT(acclag*DISPLAY_time_unit, buf, DISPLAY_time_format));
              _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, FormaT(deltactk*DISPLAY_time_unit, buf, DISPLAY_signed_time_format));
            }
          _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_NODENAME, b, COL_NODENAME, PATH_MORE_INFO.add[i].label);
        }
    }

  Board_NewSeparation(b);

  
  Board_NewLine(b);
  
  if (!hidesim) 
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ACC, b, COL_SIM_ACC, FormaT(accsim*DISPLAY_time_unit, buf, DISPLAY_time_format));
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_DELTA, b, COL_SIM_DELTA, FormaT((accsim-start)*DISPLAY_time_unit, buf, DISPLAY_time_format));
      if (DISPLAY_simdiffmode) 
         {
           error=(tasval_cmp-simval_cmp)*100/(simval_cmp-start_cmp);
         }
      if (!finite(error)) strcpy(buf1,".....");
      else if (fabs(error)>9999) sprintf(buf1,"¤6.....¤.%s", colcode);
      else 
        {
          if (fabs(error)>=50) sprintf(buf1, "¤6%+.1f¤.%s", error, colcode);
          else if (fabs(error)>10) sprintf(buf1, "¤4%+.1f¤.%s", error, colcode);
          else sprintf(buf1, "%+.1f", error);                  
        }
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_SIM_ERROR, b, COL_SIM_ERROR, buf1);
    }
  if (!hideref)
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_ACC, b, COL_REF_ACC, FormaT(accref*DISPLAY_time_unit, buf, DISPLAY_time_format));
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_DELTA, b, COL_REF_DELTA, FormaT((accref-start)*DISPLAY_time_unit, buf, DISPLAY_time_format));
    }
#ifndef CTK_DIFF_MODE
  if (!hidereflag)
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_ACC, b, COL_REF_LAG_ACC, FormaT(accreflag*DISPLAY_time_unit, buf, DISPLAY_time_format));
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_REF_LAG_DELTA, b, COL_REF_LAG_DELTA, FormaT((accreflag-start)*DISPLAY_time_unit, buf, DISPLAY_time_format));
    }
#endif
  if (!hidectk)
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_ACC, b, COL_CTK_ACC, FormaT(accctk*DISPLAY_time_unit, buf, DISPLAY_time_format));
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_DELTA, b, COL_CTK_DELTA, FormaT((accctk-start)*DISPLAY_time_unit, buf, DISPLAY_time_format));
    }
  if (!hidectklag)
    {
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_ACC, b, COL_CTK_LAG_ACC, FormaT(acclag*DISPLAY_time_unit, buf, DISPLAY_time_format));
      _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CTK_LAG_DELTA, b, COL_CTK_LAG_DELTA, FormaT((acclag-start)*DISPLAY_time_unit, buf, DISPLAY_time_format));
    }

  _ttv_Board_SetValue(DT_CONFIG_SHOW, COL_CAPA, b, COL_CAPA, "(total)");  
      
  sprintf(buf,"%s   ", LP);
  Board_Display(f, b, buf);
  Board_FreeBoard(b);
  avt_fprintf(f, "%s\n",LP);

  stb_free_noise_on_detail(detailbase);

  if (TTV_API_NOSUPINFO_DETAIL==0 && (detail_forced_mode & NOSUPINFO_DETAIL)==0)
    {
      if (tp!=NULL)
        {
          ttvevent_list *revert_tve;

          if ((tve=ttv_GetPathCommand(tp))==NULL)
            if ((tp->ROOT->ROOT->TYPE & TTV_SIG_R)!=0) tve=tp->ROOT;

          if (tve!=NULL)
            {
              if (tp->TYPE & TTV_FIND_MAX) searchtype='m'; //min
              else searchtype='M'; // max
          
              if ((tp->ROOT->ROOT->TYPE & TTV_SIG_R)==0)
                revert_tve=ttv_opposite_event(tve);
              else
                {
                  if (!ttv_PathIsHZ(tp) || searchtype=='M') revert_tve=tve;
                  else revert_tve=ttv_opposite_event(tve);
                }
              
              if (DT_CONFIG_SHOW[INFO_CMD] && (tp->ROOT->ROOT->TYPE & TTV_SIG_R)==0)
                {
                  ttv_GetNodeIntrinsicInfo(tve, tp->ROOT, DISPLAY_time_unit, DISPLAY_time_string, DISPLAY_time_format, format, 1);
                  
                  if (strlen(format)!=0)
                    {
                      avt_fprintf(f, "%s    -> Intrinsic Info: %s(%c) -> %s(%c):%s\n", LP, ttv_GetFullSignalName_COND(tp->FIG, tve->ROOT), dirconv(ttv_GetTimingEventDirection(tve)), ttv_GetFullSignalName_COND(tp->FIG, tp->ROOT->ROOT), dirconv(ttv_GetTimingEventDirection(tp->ROOT)), format);
                    }
                  ttv_GetNodeIntrinsicInfo(tve, tp->ROOT, DISPLAY_time_unit, DISPLAY_time_string, DISPLAY_time_format, format, 0);
                  
                  if (strlen(format)!=0)
                    {
                      avt_fprintf(f, "%s    -> Intrinsic Info: %s(%c) -> %s(%c):%s\n", LP, ttv_GetFullSignalName_COND(tp->FIG, tp->ROOT->ROOT), dirconv(ttv_GetTimingEventDirection(tp->ROOT)), ttv_GetFullSignalName_COND(tp->FIG, revert_tve->ROOT), dirconv(ttv_GetTimingEventDirection(revert_tve)), format);
                    }
                  
                }

              if ((tp->ROOT->ROOT->TYPE & TTV_SIG_R)==0)
                avt_fprintf(f, "%s    -> Closing Command %s(%c)\n", LP, ttv_GetFullSignalName_COND(tp->FIG, revert_tve->ROOT), dirconv(ttv_GetTimingEventDirection(revert_tve)));

              if (DT_CONFIG_SHOW[INFO_CMD])
                {          
                  ttv_GetNodeClock(tve, tp->ROOT, DISPLAY_time_unit, DISPLAY_time_string, DISPLAY_time_format, format, pt_debug!=NULL?(stbdebug_list *)pt_debug->DATA:NULL, tp);
                  
                  if (strlen(format)!=0)
                    {
                      avt_fprintf(f, "%s    -> Closing Command Wave: %s\n", LP, format);
                    }
                  
                  if ((tp->ROOT->ROOT->TYPE & TTV_SIG_R)==0)
                  {            
                    avt_fprintf(f, "%s    -> Closing Clock Route: ", LP); ttv_DisplayRoute(f, revert_tve, searchtype, NULL, (tp->ROOT->ROOT->TYPE & TTV_SIG_R)!=0);
                    avt_fprintf(f, "\n");
                  }
                  if (pt_more!=NULL) ((more_func)pt_more->DATA)(LP, f);
                }
              avt_fprintf(f, "%s\n",LP);
            }
        }
      else
        {
          // specout
          if (tp!=NULL)
            {
              if (tp->TYPE & TTV_FIND_MAX) tm=1; else tm=0;
              ttv_GetNodeSpecout(tp->ROOT, DISPLAY_time_unit, DISPLAY_time_format, format, pt_debug!=NULL?(stbdebug_list *)pt_debug->DATA:NULL, tp, tm, 0);
              if (strlen(format)!=0)
                {
                  avt_fprintf(f, "%s    -> Specification: %s\n", LP, format);
                }          
              if (pt_more!=NULL) ((more_func)pt_more->DATA)(LP, f);
              avt_fprintf(f, "%s\n",LP);
            }
        }
    }
  fflush(f);
}

void ttv_DisplayPathDetail(FILE *f, int num, ttvpath_list *tp)
{
  chain_list *detail;

  if (tp==NULL) return;

  detail=ttv_GetPathDetail(tp);
  if (detail==NULL) return;

  if (SIMULATE_MODE=='y') ttv_SimulatePath_sub(tp->FIG, tp, "", 0);

  ttv_DisplayCompletePathDetail(f, num, tp, detail);

  ttv_FreePathDetail(detail);
  freechain(detail);
}

void ttv_DisplayPathListDetail(FILE *f, chain_list *tpl)
{
  int count=1;

  if (f==NULL) return;

  ttv_DisplayConfig(f, tpl);

  avt_fprintf(f, "      *** Path list (unit:[%s]) ***\n\n", DISPLAY_time_string);

  ttv_DisplayPathList(f, tpl);

  ttv_DisplayLegend(f);

  avt_fprintf(f, "      *** Path details (unit:[%s]) ***\n\n", DISPLAY_time_string);

  while (tpl!=NULL)
    {
      ttv_DisplayPathDetail(f, count, (ttvpath_list *)tpl->DATA);
      count++;
      tpl=tpl->NEXT;
    }
}

void ttv_DisplayRoute(FILE *f, ttvevent_list *tve, char type, chain_list *pthlist, int prech)
{
  chain_list *cl, *list;
  chain_list *detail, *detailbase;
  ttvcritic_list *tc;
  char *nodename, *col, *_type;
  ttvpath_list *pth;
  char dir[5];
  ttvsig_list *tvs;

  if (f==NULL) return;

  tvs=tve->ROOT;

  if (!prech)
    sprintf(dir,"?%c",ttv_GetTimingEventDirection(tve));
  else
    strcpy(dir,"??");

  if (pthlist==NULL)
    {
      if (type=='M') list=ttv_internal_GetCriticPath(NULL, tvs, dir, 100, 1);
      else list=ttv_internal_GetCriticPath(NULL, tvs, dir, 100, 0);
    } 
  else
    list=pthlist;

  cl=list;
  while (cl!=NULL)
    {
      pth=(ttvpath_list *)cl->DATA;
      tvs=ttv_GetPathStartSignal(pth);
      if (isclock(tvs))
        {
          if (!prech || (!ttv_PathIsHZ(pth) && type=='m' && tve==pth->ROOT)) break;
          if (type=='M' && ((!ttv_PathIsHZ(pth) && tve!=pth->ROOT) ||(ttv_PathIsHZ(pth) && tve==pth->ROOT))) break;
        }
      cl=cl->NEXT;
    }
  if (cl!=NULL)
    {
      pth=(ttvpath_list *)cl->DATA;
      detailbase=detail=ttv_GetPathDetail(pth);
      avt_fprintf(f, "(");
      while (detail!=NULL)
        {
          tc=(ttvcritic_list *)detail->DATA;
          if (detail==detailbase || !(DISPLAY_nodemode==0 && strcasecmp(ttv_GetDetailType(tc),"rc")==0))
            {
              if (detail!=detailbase) avt_fprintf(f, " - ");
              nodename=ttv_GetDetailSignalName_COND(tc);
              _type=ttv_GetDetailSignalType(tc);
              if (strstr(_type,"latch")!=NULL || strstr(_type,"flip-flop")!=NULL) col="¤4";
              else col="";
              avt_fprintf(f, "%s%s¤.(%c%s)", col, nodename, dirconv(ttv_GetDetailDirection(tc)), (ttv_PathIsHZ(pth) && detail->NEXT==NULL)?"Z":"");
            }
          detail=detail->NEXT;
        }
      avt_fprintf(f, ")");
     
      ttv_FreePathDetail(detailbase);
      freechain(detailbase);
    }
  else
    {
      if (list==NULL && (tvs->TYPE & TTV_SIG_C)!=0)
        {
          nodename=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(tvs), tvs);
          avt_fprintf(f, "%s(%c)", nodename, dirconv(ttv_GetTimingEventDirection(tve)));
        }
      else
        avt_fprintf(f, "¤6<no path to clock found>¤.");
    }

  if (pthlist==NULL)
    {
      ttv_FreePathList(list);
      freechain(list);
    }
}

static ttvcritic_list *findlastgate(ttvcritic_list * c)
{
  ttvcritic_list *lastg=c;
  while (c!=NULL)
    {
      if (strcmp(ttv_GetDetailType(c),"rc")!=0) lastg=c;
      c=c->NEXT;
    }
  return lastg;
}

static int getpos(ttvcritic_list *start, ttvcritic_list *end)
{
  int pos;
  if (ttv_GetDetailDirection(start)=='u') pos=0; else pos=2;
  if (ttv_GetDetailDirection(end)=='u') pos+=0; else pos+=1;
  return pos;
}

char *ttv_internal_getclockliststring(ttvfig_list *tf, char *clock)
{
  chain_list *cl, *list;
  char *key, *tmp;
  int size=4096, i, l;

  key=(char *)mbkalloc(size+10);
  if (strcmp(clock,"*")==0)
    {
      list=ttv_getclocklist(tf);
      i=0;
      strcpy(key,"");
      for (cl=list; cl!=NULL; cl=cl->NEXT)
        {
          l=strlen((char *)cl->DATA);
          if (i+l>size)
          {
            tmp=(char *)mbkalloc((size*2)+10);
            memcpy(tmp, key, i);
            mbkfree(key);
            key=tmp;
            size*=2;
          }
          strcpy(&key[i], (char *)cl->DATA);
          i+=l;
          key[i++]=' ';
        }
      key[i++]='\0';
      freechain(list);
    }
  else 
    strcpy(key, clock);

  return key;
}

static char *ttv_internal_getsigliststring(ttvfig_list *tf, chain_list *list)
{
  chain_list *cl;
  char *key, *tmp;
  char *nm;
  int size=4096, i, l;

  key=(char *)mbkalloc(size+10);
  i=0;
  strcpy(key,"");
  for (cl=list; cl!=NULL; cl=cl->NEXT)
    {
      nm=ttv_GetFullSignalName(tf, (ttvsig_list *)cl->DATA);
      l=strlen(nm);
      if (i+l>size)
        {
          tmp=(char *)mbkalloc((size*2)+10);
          memcpy(tmp, key, i);
          mbkfree(key);
          key=tmp;
          size*=2;
        }
      strcpy(&key[i], nm);
      i+=l;
      key[i++]=' ';
    }
  key[i++]='\0';
  return key;
}

chain_list *ttv_internal_getclockpaths(ttvfig_list *tf, char *clock, char *cmd, int number, char *minmax)
{
  chain_list *pth;
  char *key;
  long old0, old1;

  ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);

  key=ttv_internal_getclockliststring(tf, clock);

  if (strcmp(key,"")!=0)
    {
      ttv_setsearchexclude(0,TTV_SIG_LSL|TTV_SIG_LL|TTV_SIG_LF|TTV_SIG_B|TTV_SIG_R|TTV_SIG_C,&old0, &old1);
      
      pth=ttv_internal_GetPaths(tf, "*", key, cmd, "??", number, "critic", "path", minmax);
      
      ttv_setsearchexclude(old0, old1, &old0, &old1);
      
      ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);      
    }
  else
    pth=NULL;
  mbkfree(key);

  return pth;
}

void ttv_DisplayClockPathReport(FILE *f, ttvfig_list *tf, char *clock, char *minmax, int number)
{
  NameAllocator na;
  ht *all;
  char *nkey, key[4096], buf[20];
  chain_list *pth, *list, *cl, *detail, *list2, *ch;
  ttvpath_list *tpl;
  ttvcritic_list *start, *preend, *tc, **tab;
  struct {
    ttvcritic_list *critic, *first;
    ttvcritic_list *end;
    double addrc, total;
  } runc[4];
  long l;
  int ref, i;
  typedef struct 
  {
    chain_list *det[4];
    ht *ends;
  } PClass;
  PClass *pc;
  Board *b;

  if (!strcasecmp (minmax, "max")) l=1;
  else if (!strcasecmp (minmax, "min")) l=0;
  else {
    avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
//    avt_error("ttvapi", 504, AVT_ERR, "ttv_DisplayClockPathReport: available values for 'minmax' are 'min' or 'max'\n");
    return;
  }

  if (f==NULL) return;

  if (ftello(f)==0)
    ttv_DumpHeader(f, tf);
            
  avt_fprintf(f, "      *** Clock %s Path report (unit:[%s]) ***\n", l==0?"MIN":"MAX", DISPLAY_time_string);

  pth=ttv_internal_getclockpaths(tf, clock, "*", INT_MAX, minmax);

  CreateNameAllocator(1024, &na, 'y');
  all=addht(100);

  for (cl=pth, ref=0; cl!=NULL && (number<=0 || ref<number); cl=cl->NEXT)
    {
      tpl=(ttvpath_list *)cl->DATA;
//      if ((tpl->ROOT->ROOT->TYPE & TTV_SIG_Q)!=TTV_SIG_Q) continue;
      ref++;
      detail=ttv_GetPathDetail(tpl);
      start=(ttvcritic_list *)detail->DATA;
      preend=findlastgate(start);
      strcpy(key,"");
      /*for (tc=start; tc!=preend; tc=tc->NEXT)
        {
          if (tc==start || strcmp(ttv_GetDetailType(tc),"rc")!=0)
            strcat(key, ttv_GetDetailSignalName(tc));
        }*/
      strcat(key, ttv_GetDetailSignalName(start));
      strcat(key, ttv_GetDetailSignalName(preend));
      sprintf(buf, " %d", countchain(detail));
      strcat(key, buf);

      nkey=NameAlloc(&na, key);
      if ((l=gethtitem(all, nkey))==EMPTYHT)
        {
          pc=(PClass *)mbkalloc(sizeof(PClass));
          pc->det[0]=pc->det[1]=pc->det[2]=pc->det[3]=NULL;
          addhtitem(all, nkey, (long)pc);
          pc->ends=addht(100);
        }
      else
        pc=(PClass *)l;

      i=getpos(start, preend);
      pc->det[i]=addchain(pc->det[i], detail);

      if (preend->NEXT!=NULL)
        {
          nkey=NameAlloc(&na, ttv_GetDetailNodeName(preend->NEXT));
          if ((l=gethtitem(pc->ends, nkey))==EMPTYHT)
            {
              tab=(ttvcritic_list **)mbkalloc(sizeof(ttvcritic_list *)*4);
              for (l=0; l<4; l++)tab[l]=NULL;
              addhtitem(pc->ends, nkey, (long)tab);
            }
          else
            tab=(ttvcritic_list **)l;
          tab[i]=preend->NEXT;
        }
    }

  avt_fprintf(f, "          %d/%d paths from clock '%s' reported\n\n", ref, countchain((chain_list *)pth), clock);
  
  list=GetAllHTElems_sub(all, 1);

  delht(all);
  
  for (cl=list; cl!=NULL; cl=cl->NEXT)
    {
      pc=(PClass *)cl->DATA;
      b=Board_CreateBoard();

      Board_SetSize(b, COL_CP_NAME, 10, 'l');
      ref=-1;
      for (l=0;l<4; l++)
        {
          if (pc->det[l]!=NULL)
            {
              Board_SetSep(b, 10+(l*COLUMN_DISTANCE)-1);
              Board_SetSize(b, 10+(l*COLUMN_DISTANCE)+0, 3, 'l');
              Board_SetSize(b, 10+(l*COLUMN_DISTANCE)+1, 7, DISPLAY_number_justify);
              Board_SetSize(b, 10+(l*COLUMN_DISTANCE)+2, 7, DISPLAY_number_justify);
              runc[l].critic=runc[l].first=(ttvcritic_list *)((chain_list *)pc->det[l]->DATA)->DATA;
              runc[l].end=findlastgate(runc[l].critic);
              runc[l].addrc=0;
              runc[l].total=0;
              if (ref==-1) ref=l;
            }
        }
      Board_NewLine(b);
      Board_SetValue(b, COL_CP_NAME, "Name");
      for (l=0;l<4; l++)
        {
          if (pc->det[l]!=NULL)
            {
//              sprintf(key, "Delay", DISPLAY_time_string);
              Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+1, "Delay");
//              sprintf(key, "Slope", DISPLAY_time_string);
              Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+2, "Slope");
            }
        }

      Board_NewSeparation(b);

      while (runc[ref].critic!=runc[ref].end->NEXT)
        {
          if (runc[ref].critic==runc[ref].first || strcmp(ttv_GetDetailType(runc[ref].critic),"rc")!=0)
            {
              Board_NewLine(b);
              Board_SetValue(b, COL_CP_NAME, ttv_GetDetailSignalName(runc[ref].critic));
              for (l=0;l<4; l++)
                {
                  if (pc->det[l]!=NULL)
                    {
                      runc[l].total+=runc[l].addrc+ttv_GetDetailDelay(runc[l].critic);
                      runc[l].addrc=0;
                      sprintf(key,"  (%c)", dirconv(ttv_GetDetailDirection(runc[l].critic)));
                      Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+0, key);
                      Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+1, FormaT(runc[l].total*DISPLAY_time_unit, key, DISPLAY_time_format));
                      Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+2, FormaT(ttv_GetDetailSlope(runc[l].critic)*DISPLAY_time_unit, key, DISPLAY_time_format));

                      runc[l].critic=runc[l].critic->NEXT;
                    }
                } 
            }
          else 
            {
              for (l=0; l<4; l++)
                {
                  if (pc->det[l]!=NULL)
                    {
                      runc[l].addrc=ttv_GetDetailDelay(runc[l].critic);
                      runc[l].critic=runc[l].critic->NEXT;
                    }
                }
            }
        }

      list2=GetAllHTElems_sub(pc->ends, 1);
      if (list2!=NULL)
        {
          Board_NewLine(b);
          Board_SetValue(b, COL_CP_NAME, " -- ending rc nodes below --");
          for (ch=list2; ch!=NULL; ch=ch->NEXT)
            {
              tab=(ttvcritic_list **)ch->DATA;
              Board_NewLine(b);
              for (l=0;l<4; l++)
                {
                  if (tab[l]!=NULL)
                    {
                      ref=l;
                      sprintf(key,"  (%c)", dirconv(ttv_GetDetailDirection(tab[l])));
                      Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+0, key);
                      Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+1, FormaT((runc[l].total+ttv_GetDetailDelay(tab[l]))*DISPLAY_time_unit, key, DISPLAY_time_format));
                      Board_SetValue(b, 10+(l*COLUMN_DISTANCE)+2, FormaT(ttv_GetDetailSlope(tab[l])*DISPLAY_time_unit, key, DISPLAY_time_format));
                    }
                }
              Board_SetValue(b, COL_CP_NAME, ttv_GetDetailNodeName(tab[ref]));
              mbkfree(tab);
            }
        }
         
      freechain(list2);
      delht(pc->ends);
      for (l=0;l<4; l++)
        {
          for (ch=pc->det[l]; ch!=NULL; ch=ch->NEXT)
            {
              ttv_FreePathDetail((chain_list *)ch->DATA);
              freechain((chain_list *)ch->DATA);
            }
          freechain(pc->det[l]);
        }
      mbkfree(pc);

      Board_Display(f, b, "");
      Board_FreeBoard(b);
      avt_fprintf(f, "\n");
    }

  freechain(list);

  DeleteNameAllocator(&na);
  ttv_FreePathList(pth);
  freechain(pth);
}



#define MINCLOCKPATH_PTYPE 0xfab19105
#define MAXCLOCKPATH_PTYPE 0xfac19105
#define MINDATAPATH_PTYPE 0xfad19105
#define MAXDATAPATH_PTYPE 0xfae19105


/* added by Anto */

static stbck *
ttv_GetGoodStbClock(ttvevent_list *tve, ttvevent_list *latch)
{
  stbnode     *node;
  stbck *clock=NULL;

  if (tve==NULL) return NULL;
  node=stb_getstbnode(latch);
  if (node!=NULL)
    {
      for (clock=node->CK; clock!=NULL && clock->CMD!=tve; clock=clock->NEXT) ;
      if (clock==NULL) clock=node->CK;
    }
  return clock;
}
/* end added Anto */

static chain_list *associatepathtosig(ttvfig_list *tf, char *ck, char *in, char *out, char *minmax, long ptypenum, char enddir, char *path)
{
  chain_list *lst, *cl, *extr=NULL;
  ttvpath_list *tp;
  ttvsig_list *sig;
  ptype_list *pt;
  char dir[4]="??";

  dir[1]=enddir;
  lst=ttv_internal_GetPaths(tf, ck, in, out, dir, INT_MAX, "critic", path, minmax);

  for (cl=lst; cl!=NULL; cl=cl->NEXT)
    {
      tp=(ttvpath_list *)cl->DATA;
      sig=tp->ROOT->ROOT;
      if ((pt=getptype(sig->USER, ptypenum))==NULL)
        {
          pt=sig->USER=addptype(sig->USER, ptypenum, NULL);
          extr=addchain(extr, sig);
        }
      pt->DATA=addchain((chain_list *)pt->DATA, tp);
    }

  freechain(lst);
  return extr;
}

static void associatepathtosig_more(ttvfig_list *tf, chain_list *lst, long minmax, long ptypenum, chain_list **siglist)
{
  ptype_list *pt;
  ttvsig_list *tvs;

  while (lst!=NULL)
    {
      tvs=lst->DATA;
      if ((tvs->TYPE & TTV_SIG_C)!=0 && (tvs->TYPE & TTV_SIG_BYPASSIN)==0)
        {
          if ((pt=getptype(tvs->USER, ptypenum))==NULL)
            {
              pt=tvs->USER=addptype(tvs->USER, ptypenum, NULL);
              *siglist=addchain(*siglist, tvs);
            }
          pt->DATA=addchain((chain_list *)pt->DATA, ttv_create_one_node_path(tf, tf, &tvs->NODE[0], minmax));
          pt->DATA=addchain((chain_list *)pt->DATA, ttv_create_one_node_path(tf, tf, &tvs->NODE[1], minmax));
        }
      lst=lst->NEXT;
    }
}

static void cleanpathonsig(chain_list *cl, long ptypenum)
{
  ttvsig_list *sig;
  ptype_list *pt;
  while (cl!=NULL)
    {
      sig=(ttvsig_list *)cl->DATA;
      if ((pt=getptype(sig->USER, ptypenum))!=NULL)
        {
          
          ttv_FreePathList((chain_list *)pt->DATA);
          freechain((chain_list *)pt->DATA);
          sig->USER=delptype(sig->USER, ptypenum);
        }
      cl=cl->NEXT;
    }
}

#define BREAKCONSTRAINT 0xfab70809

static void ttv_setupbreakinfo(ttvfig_list *tvf, ttvevent_list *tve, long type)
{
 ttvline_list *line ;
 ptype_list *pt ;
 long delay ;


 if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    line = tve->INPATH ;
 else
    line = tve->INLINE ;

 for(; line != NULL ; line = line->NEXT) 
  {
   if(((line->TYPE & (TTV_LINE_D|TTV_LINE_T)) != 0) &&
      (line->FIG != tvf))
     continue ;

   if((line->NODE->ROOT->TYPE & TTV_SIG_B)!=0 && 
      ((line->TYPE & TTV_LINE_U) == TTV_LINE_U || (line->TYPE & TTV_LINE_O) == TTV_LINE_O))
     {
       if ((pt=getptype(line->NODE->USER, BREAKCONSTRAINT))==NULL)
         pt=line->NODE->USER=addptype(line->NODE->USER, BREAKCONSTRAINT, addptype(NULL, (long)NULL, NULL));

//       delay=ttv_getdelaymax(line);

       pt=(ptype_list *)pt->DATA;
       if((line->TYPE & TTV_LINE_U) == TTV_LINE_U)
         pt->TYPE=(long )addptype((ptype_list *)pt->TYPE, (long)line, line->ROOT);
       else
         pt->DATA=addptype((ptype_list *)pt->DATA, (long)line, line->ROOT);
     }
  }
}

static void ttv_removebreakinfo(ttvevent_list *tve)
{
  ptype_list *pt ;
  if ((pt=getptype(tve->USER, BREAKCONSTRAINT))!=NULL)
    {
      pt=(ptype_list *)pt->DATA;
      freeptype((ptype_list *)pt->TYPE);
      freeptype((ptype_list *)pt->DATA);
      freeptype(pt);
      tve->USER=delptype(tve->USER, BREAKCONSTRAINT);
    }
}

static ptype_list *ttv_getbreakinfo(ttvevent_list *tve, int setup)
{
  ptype_list *pt ;
  if ((pt=getptype(tve->USER, BREAKCONSTRAINT))!=NULL)
    {
      pt=(ptype_list *)pt->DATA;
      if (setup)
        return dupptypelst((ptype_list *)pt->TYPE);
      else
        return dupptypelst((ptype_list *)pt->DATA);
    }
  return NULL;
}

#define CONTRAINTOBJECT_REFCOUNT 0xfab70806
#define CONTRAINTOBJECT_GENE_CLOCK_PATH_MIN 0xfab70828
#define CONTRAINTOBJECT_GENE_CLOCK_PATH_MAX 0xfab70829


typedef struct
{
  double min;
  ConstraintObject *setup, *hold;
} BI_ConstraintObject;

double ttv_ComputeConstraintVALID(ConstraintObject *co)
{
  return ttv_GetPathDelay(co->datapath)+co->margedata+co->intrinsic_margin;
}
double ttv_ComputeConstraintREQUIRED(ConstraintObject *co)
{
  return ttv_GetPathDelay(co->clockpath)+co->margeclock+co->clocklatency;
}

double ttv_ComputeConstraintObjectMargin(ConstraintObject *co)
{
  double val;

  val=ttv_ComputeConstraintVALID(co)-ttv_ComputeConstraintREQUIRED(co);
  
  if (co->setup)
    return val;
  else
    return -val;
}
typedef int (*sortfunc)(const void *a0, const void *b0);

static int ConstraintObjectSort(const void *a0, const void *b0)
{
  ConstraintObject *a=*(ConstraintObject **)a0, *b=*(ConstraintObject **)b0;
  int cmp, cmp0;
  double m0, m1;
  cmp=strcmp(a->datapath->NODE->ROOT->NAME, b->datapath->NODE->ROOT->NAME);
  if (cmp<0) return -1;
  if (cmp>0) return 1;

  if (a->datapath->NODE->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
  if (b->datapath->NODE->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
  if (cmp<cmp0) return -1;
  if (cmp>cmp0) return 1;

  cmp=strcmp(a->clockpath->NODE->ROOT->NAME, b->clockpath->NODE->ROOT->NAME);
  if (cmp<0) return -1;
  if (cmp>0) return 1;

  if (a->clockpath->NODE->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
  if (b->clockpath->NODE->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
  if (cmp<cmp0) return -1;
  if (cmp>cmp0) return 1;

  m0=ttv_ComputeConstraintObjectMargin(a);
  m1=ttv_ComputeConstraintObjectMargin(b);

  if (m0<m1) return -1;
  if (m0>m1) return 1;

  return 0;
}

static int ConstraintObjectSort_for_C2L_cmp(ConstraintObject *a, ConstraintObject *b, int ids, int ide, int ics, int ice, int imargin)
{
  int cmp, cmp0;
  double m0, m1;

  if (!ids)
    {
      cmp=strcmp(a->datapath->NODE->ROOT->NAME, b->datapath->NODE->ROOT->NAME);
      if (cmp<0) return -1;
      if (cmp>0) return 1;
      
      if (a->datapath->NODE->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
      if (b->datapath->NODE->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
      if (cmp<cmp0) return -1;
      if (cmp>cmp0) return 1;
    }

  if (!ide)
    {
      cmp=strcmp(a->datapath->ROOT->ROOT->NAME, b->datapath->ROOT->ROOT->NAME);
      if (cmp<0) return -1;
      if (cmp>0) return 1;
      
      if (a->datapath->ROOT->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
      if (b->datapath->ROOT->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
      if (cmp<cmp0) return -1;
      if (cmp>cmp0) return 1;
    }

  if (!ics || ics==2)
    {
      cmp=strcmp(a->clockpath->NODE->ROOT->NAME, b->clockpath->NODE->ROOT->NAME);
      if (cmp<0) return -1;
      if (cmp>0) return 1;
      
      if (ics==0)
        {
          if (a->clockpath->NODE->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
          if (b->clockpath->NODE->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
          if (cmp<cmp0) return -1;
          if (cmp>cmp0) return 1;
        }
    }

  if (!ice)
    {
      cmp=strcmp(a->clockpath->ROOT->ROOT->NAME, b->clockpath->ROOT->ROOT->NAME);
      if (cmp<0) return -1;
      if (cmp>0) return 1;
      
      if (a->clockpath->ROOT->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
      if (b->clockpath->ROOT->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
      if (cmp<cmp0) return -1;
      if (cmp>cmp0) return 1;
    }

  if (!imargin)
    {
      m0=ttv_ComputeConstraintObjectMargin(a);
      m1=ttv_ComputeConstraintObjectMargin(b);
      
      if (m0>m1) return -1;
      if (m0<m1) return 1;
    }

  return 0;
}

static int cfgclock, cfgcmd;

static int ConstraintObjectSort_for_C2L(const void *a0, const void *b0)
{
  ConstraintObject *a=*(ConstraintObject **)a0, *b=*(ConstraintObject **)b0;
  return ConstraintObjectSort_for_C2L_cmp(a, b, 0, 0, cfgclock, cfgcmd, 0);
}


static int BI_ConstraintObjectSort(const void *a0, const void *b0)
{
  BI_ConstraintObject *a=(BI_ConstraintObject *)a0, *b=(BI_ConstraintObject *)b0;
  int cmp, cmp0;
  ConstraintObject *coa=a->setup, *cob=b->setup;

  if (coa==NULL) coa=a->hold;
  if (cob==NULL) cob=b->hold;
  

  cmp=strcmp(coa->datapath->NODE->ROOT->NAME, cob->datapath->NODE->ROOT->NAME);
  if (cmp<0) return -1;
  if (cmp>0) return 1;

  if (coa->datapath->NODE->TYPE & TTV_NODE_UP) cmp=1; else cmp=0;
  if (cob->datapath->NODE->TYPE & TTV_NODE_UP) cmp0=1; else cmp0=0;
  if (cmp<cmp0) return -1;
  if (cmp>cmp0) return 1;

  if (a->min<b->min) return 1;
  if (a->min>b->min) return -1;

  return 0;
}

ttvevent_list *ttv_GetClockPathCmd(ttvevent_list *node, ttvevent_list *cmd, int hzpath, int setup, int *mustbehz)
{
  ttvevent_list *tve;
  ttvevent_list *revert_tve;
  int prech=0, breakp=0, fflop=0;
  ptype_list *pt;

  if ((node->ROOT->TYPE & TTV_SIG_R)!=0) prech=1; 
  else if ((node->ROOT->TYPE & TTV_SIG_B)!=0) breakp=1; 
  else if ((node->ROOT->TYPE & TTV_SIG_LF)==TTV_SIG_LF) fflop=1; 
  if ((tve=cmd)==NULL)
    {
      if (prech) tve=node;
      else if (breakp)
        {
          if ((pt=getptype(node->ROOT->USER, STB_BREAK_TEST_EVENT))!=NULL)
            tve=(ttvevent_list *)pt->DATA;
        }
    }

  *mustbehz=0;
  if (tve!=NULL)
    {
      if (prech)
        {
          if (!hzpath)
            {
              revert_tve=tve, *mustbehz=1;
            }
          else
            {
              revert_tve=tve;
            }
        }
      else if (breakp || fflop)
        revert_tve=tve;
      else
        {
          if (!setup || !ttv_has_strict_setup(node))
            revert_tve=ttv_opposite_event(tve);
          else
            revert_tve=tve;
        }
      
      tve=revert_tve;
    }
  return tve;
}

static void ttv_sortaddedpaths(chain_list **paths, int max)
{
  ttvpath_list *pth;
  chain_list *cl, *ch;
  ptype_list *tp;

  if (*paths==NULL) return;

  for (cl=*paths; cl!=NULL; cl=cl->NEXT)
    {
      pth=(ttvpath_list *)cl->DATA;
      if (cl->NEXT!=NULL)
        pth->NEXT=cl->NEXT->DATA;
      else
        pth->NEXT=NULL;
    }    
  
  pth=(ttvpath_list *)(*paths)->DATA;
  freechain(*paths);

  pth=ttv_classpath(pth, max?TTV_FIND_MAX:TTV_FIND_MIN);

  cl=ch=NULL;
  while (pth!=NULL)
    {
      if ((tp=getptype(pth->NODE->USER, TTV_SIG_MARQUE))==NULL)
        tp=pth->NODE->USER=addptype(pth->NODE->USER, TTV_SIG_MARQUE, (void *)0);

      if ((((long)tp->DATA & (TTV_NODE_UP|TTV_NODE_DOWN)) & (pth->ROOT->TYPE & (TTV_NODE_UP|TTV_NODE_DOWN)))==0)
        {
          cl=addchain(cl, pth);
          tp->DATA=(void *)(((long)tp->DATA) | pth->ROOT->TYPE);
        }
      else
        ch=addchain(ch, pth);
      pth=pth->NEXT;
    }

  ttv_FreePathList(ch);
  freechain(ch);

  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      pth=(ttvpath_list *)ch->DATA;
      pth->NODE->USER=testanddelptype(pth->NODE->USER, TTV_SIG_MARQUE);
    }

  *paths=reverse(cl);
}
static chain_list *getcmdpaths_new(ttvfig_list *tf, ttvevent_list *cmd_ev, int max, int hz)
{
  long search;
  ptype_list *pt0;
  chain_list *last, *sp;
  ttvpath_list *pth;

  if (max) search=TTV_FIND_MAX, max=1;
  else search=TTV_FIND_MIN, max=0;

  search|=TTV_FIND_LINE;

  last=NULL;

  if (max) search=MAXCLOCKPATH_PTYPE;
  else search=MINCLOCKPATH_PTYPE;

  if ((pt0=getptype(cmd_ev->ROOT->USER, search))==NULL) sp=NULL;
  else sp=(chain_list *)pt0->DATA;
  
  if (sp==NULL && ttv_IsClock(cmd_ev->ROOT) && (cmd_ev->ROOT->TYPE & TTV_SIG_BYPASSIN)==0)
    {
      sp=addchain(sp, ttv_create_one_node_path(tf, tf, cmd_ev, (search & (TTV_FIND_MIN|TTV_FIND_MAX))));
      cmd_ev->ROOT->USER=addptype(cmd_ev->ROOT->USER, search, sp);
    }

  while (sp!=NULL)
    {
      pth=(ttvpath_list *)sp->DATA;
      if (pth->ROOT==cmd_ev
          && ((hz && (pth->TYPE & TTV_FIND_HZ)!=0)
              || (!hz && (pth->TYPE & TTV_FIND_HZ)==0))
          )
        {
          last=addchain(last, pth);
        }
      sp=sp->NEXT;
    }

  return last;
}

static chain_list *getcmdpaths_data_new(ttvfig_list *tf, ttvevent_list *cmd_ev, int max)
{
  long search;
  ptype_list *pt0;
  chain_list *last, *sp;
  ttvpath_list *pth;

  if (max) search=TTV_FIND_MAX, max=1;
  else search=TTV_FIND_MIN, max=0;

  search|=TTV_FIND_LINE;

  last=NULL;

  if (max) search=MAXDATAPATH_PTYPE;
  else search=MINDATAPATH_PTYPE;

  if ((pt0=getptype(cmd_ev->ROOT->USER, search))==NULL) sp=NULL;
  else sp=(chain_list *)pt0->DATA;
  
  if (sp==NULL && (cmd_ev->ROOT->TYPE & TTV_SIG_C)!=0)
    {
      sp=addchain(sp, ttv_create_one_node_path(tf, tf, cmd_ev, (search & (TTV_FIND_MIN|TTV_FIND_MAX))));
      cmd_ev->ROOT->USER=addptype(cmd_ev->ROOT->USER, search, sp);
    }

  while (sp!=NULL)
    {
      pth=(ttvpath_list *)sp->DATA;
      if (pth->ROOT==cmd_ev && !ttv_IsClock(pth->NODE->ROOT))
        {
          last=addchain(last, pth);
        }
      sp=sp->NEXT;
    }

  return last;
}

static void ttv_sort_cotab(chain_list *allobj, sortfunc sf)
{
  ConstraintObject **cotab;
  chain_list *cl;
  int i, tot=countchain(allobj);
  cotab=(ConstraintObject **)mbkalloc(sizeof(ConstraintObject *)*tot);
  for (cl=allobj, i=0; cl!=NULL; cl=cl->NEXT, i++) cotab[i]=(ConstraintObject *)cl->DATA;
  qsort(cotab, tot, sizeof(ConstraintObject *), sf);
  for (cl=allobj, i=0; cl!=NULL; cl=cl->NEXT, i++) cl->DATA=cotab[i];
  mbkfree(cotab);
}

chain_list *ttv_getnext_different_ConstraintObject(chain_list *lst, ConstraintObject *co, int ics, int ice)
{
  while (lst!=NULL)
    {
      if (ConstraintObjectSort_for_C2L_cmp(co, (ConstraintObject *)lst->DATA, 0, 0, ics, ice, 1)!=0) break;
      lst=delchain(lst, lst);
    }
  return lst;
}


static void ttv_GetDirectiveConstraintObject_SetupNodes(chain_list *directivenodes)
{
  chain_list *cl;
  ttvsig_list *tvs;

  for(cl=directivenodes; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if((tvs->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
        {
          tvs->NODE[0].TYPE |= TTV_NODE_STOP ;
          tvs->NODE[1].TYPE |= TTV_NODE_STOP ;
          if ((tvs->TYPE & TTV_SIG_N) != 0)
            TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
          TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_CROSS_STOP_NODE_IF_NONSTOP;
        }
    }
}

static void ttv_GetDirectiveConstraintObject_UnSetupNodes(chain_list *directivenodes)
{
  chain_list *cl;
  ttvsig_list *tvs;

  TTV_MORE_SEARCH_OPTIONS&=~(TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N|TTV_MORE_OPTIONS_CROSS_STOP_NODE_IF_NONSTOP);

  for(cl=directivenodes; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if((tvs->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
        {
          tvs->NODE[0].TYPE &= ~TTV_NODE_STOP ;
          tvs->NODE[1].TYPE &= ~TTV_NODE_STOP ;
        }
    }
}

static void ttv_GetDirectiveConstraintObject(chain_list *allnodes, chain_list **directivecmd, chain_list **directivedata)
{
  chain_list *allcmd=NULL, *cl, *alldirectivenode=NULL;
  ttvsig_list *tvs, *tvscmd;
  ptype_list *checkcmd, *pt;
  int i, cnt, j;

  for (cl=allnodes; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      cnt=0;
      for (j=0; j<2; j++)
        {
          for (i=0; i<2; i++)
            {
              checkcmd=ttv_get_directive_slopes(&tvs->NODE[i], j, -1, -1);
//              checkcmd=(ptype_list *)append((chain_list *)ttv_get_directive_slopes(&tvs->NODE[i], j, 1), (chain_list *)checkcmd);
              for (pt=checkcmd; pt!=NULL; pt=pt->NEXT)
                {
                  cnt++;
                  tvscmd=((ttvevent_list *)pt->DATA)->ROOT;
                  if (getptype(tvscmd->USER, TTV_SIG_MARQUE)==NULL)
                    {
                      allcmd=addchain(allcmd, tvscmd);
                      tvscmd->USER=addptype(tvscmd->USER, TTV_SIG_MARQUE, NULL);
                    }
                }
              freeptype(checkcmd);
            }
        }
      if (cnt!=0)
        alldirectivenode=addchain(alldirectivenode, tvs);
    }
  
  for (cl=allcmd; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      tvs->USER=testanddelptype(tvs->USER, TTV_SIG_MARQUE);
    }

  *directivecmd=allcmd;
  *directivedata=alldirectivenode;
}

static void ttv_FreeConstraintObjectManagement(ttvfig_list *tf, chain_list *allnodes)
{
  ptype_list *pt;
  ConstraintObjectManagement *com;
  chain_list *cl;
  ttvsig_list *tvs;

  if ((pt=getptype(tf->USER, CONTRAINTOBJECT_REFCOUNT))!=NULL)
    {
      com=(ConstraintObjectManagement *)pt->DATA;
      freechain(com->directivedata);
      freechain(com->connectordone);
      for (cl=allnodes; cl!=NULL; cl=cl->NEXT)
        {
          tvs=(ttvsig_list *)cl->DATA;
          if ((tvs->TYPE & TTV_SIG_Q)!=0)
            {
              ttv_removebreakinfo(&tvs->NODE[0]);
              ttv_removebreakinfo(&tvs->NODE[1]);
            }
        }  
      mbkfree(com);
      tf->USER=delptype(tf->USER, CONTRAINTOBJECT_REFCOUNT);
    }
}


static void ttv_SetupGeneratedClockPaths(ttvfig_list *tvf)
{
  chain_list *clks;
  ttvsig_list *tvs;
  int i;

  clks=ttv_GetClockList(tvf);
  while (clks!=NULL)
    {
      tvs=(ttvsig_list *)clks->DATA;
      for (i=0; i<2; i++)
        {
          tvs->NODE[i].USER=addptype(tvs->NODE[i].USER, CONTRAINTOBJECT_GENE_CLOCK_PATH_MIN, ttv_GetGeneratedClockPaths(tvf, &tvs->NODE[i], "min"));
          tvs->NODE[i].USER=addptype(tvs->NODE[i].USER, CONTRAINTOBJECT_GENE_CLOCK_PATH_MAX, ttv_GetGeneratedClockPaths(tvf, &tvs->NODE[i], "max"));
        }
      clks=delchain(clks, clks);
    }
}
static void ttv_UnSetupGeneratedClockPaths(ttvfig_list *tvf)
{
  chain_list *clks, *cl;
  ttvsig_list *tvs;
  int i;
  ptype_list *pt;

  clks=ttv_GetClockList(tvf);
  while (clks!=NULL)
    {
      tvs=(ttvsig_list *)clks->DATA;
      for (i=0; i<2; i++)
        {
          if ((pt=getptype(tvs->NODE[i].USER, CONTRAINTOBJECT_GENE_CLOCK_PATH_MIN))!=NULL)
            {
              cl=(chain_list *)pt->DATA;
              ttv_FreePathList(cl);
              freechain(cl);
            }
          if ((pt=getptype(tvs->NODE[i].USER, CONTRAINTOBJECT_GENE_CLOCK_PATH_MAX))!=NULL)
            {
              cl=(chain_list *)pt->DATA;
              ttv_FreePathList(cl);
              freechain(cl);
            }
        }
      clks=delchain(clks, clks);
    }
}

static void ttv_GetGeneratedClockPathsInfo(ttvevent_list *tve, int max, double *latency, ttvevent_list **master)
{
  chain_list *cl;
  ptype_list *pt;

  *latency=0;
  *master=NULL;

  if (max)
    pt=getptype(tve->USER, CONTRAINTOBJECT_GENE_CLOCK_PATH_MAX);
  else
    pt=getptype(tve->USER, CONTRAINTOBJECT_GENE_CLOCK_PATH_MIN);

  if (pt!=NULL && (cl=(chain_list *)pt->DATA)!=NULL)
    {
      *master=((ttvpath_list *)cl->DATA)->NODE;
      while (cl!=NULL)
        {
          *latency+=ttv_GetPathDelay((ttvpath_list *)cl->DATA);
          cl=cl->NEXT;
        }
    }
}


void ttv_FreeConstraintObject_sub(ConstraintObject *co, int freeclocks)
{
  ptype_list *pt;
  chain_list *allnodes;

  co->com->count--;
  if (co->com->count==0)
    {
      allnodes=ttv_getsigbytype(co->com->tf,NULL,TTV_SIG_TYPEALL,NULL);
      cleanpathonsig(allnodes, MINDATAPATH_PTYPE);
      cleanpathonsig(allnodes, MAXDATAPATH_PTYPE);

      if (freeclocks)
        {
          cleanpathonsig(allnodes, MINCLOCKPATH_PTYPE);
          cleanpathonsig(allnodes, MAXCLOCKPATH_PTYPE);
          ttv_UnSetupGeneratedClockPaths(co->com->tf);
          ttv_FreeConstraintObjectManagement(co->com->tf, allnodes);
        }
      freechain(allnodes);
    }
  mbkfree(co);
}

void ttv_FreeConstraints_sub(chain_list *allobj, ttvfig_list *tvf, int autoclean)
{
  chain_list *allnodes;
  while (allobj!=NULL)
    {
      ttv_FreeConstraintObject_sub((ConstraintObject *)allobj->DATA, autoclean);
      allobj=allobj->NEXT;
    }
  if (tvf!=NULL)
    {
      allnodes=ttv_getsigbytype(tvf,NULL,TTV_SIG_TYPEALL,NULL);
      cleanpathonsig(allnodes, MINCLOCKPATH_PTYPE);
      cleanpathonsig(allnodes, MAXCLOCKPATH_PTYPE);
      ttv_FreeConstraintObjectManagement(tvf, allnodes);
      freechain(allnodes);
    }
}

double ttv_GetLatchIntrinsicQuick(ttvfig_list *tvf, ttvevent_list *latch, ttvevent_list *cmd, int setup, ttvline_list **rline)
{
  long type, val;
  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;

  if (setup) type|=TTV_FIND_SETUP;
  else type|=TTV_FIND_HOLD;

  val=ttv_getconstraintquick(tvf,latch->ROOT->ROOT,latch,cmd,type|TTV_FIND_MAX,rline);
  if (val==TTV_NOTIME) val=0;
  return _LONG_TO_DOUBLE(val);
}

chain_list *ttv_GetConstraints(ttvfig_list *tf, char *inputconnector, char *towhat)
{
  ttvsig_list *latch, *tvs;
  ttvevent_list *in_ev, *out_ev, *checkcmd;
  ttvpath_list *tp;
  int i, tot, hz, j;
  chain_list *latchlist, *cl, *ch, *connectorlist;
  ptype_list *cmdlist;
  chain_list *pth, *cmdsigsmin, *cmdsigsmax, *datasigmin, *datasigmax, *allobj;
  long margedata, margeclock, overlapc, overlapd;
  long old0, old1;
  ptype_list *pt;
  int saveTTV_MAX_PATH_PERIOD;
  ConstraintObject *co;
  chain_list *directivecmd, *allnodes, *allclocks;
  ConstraintObjectManagement *com;
  char *tok, *c, buf1[1024];
  int latchh=0, prech=0, clockgating=0, breakp=0;
  ttvline_list *rline;

  API_TEST_TOKEN_SUB(TMA_API,"tma")

  allclocks=ttv_getclocksiglist(tf);
  
  if (allclocks==NULL)
    {
      avt_errmsg(TTV_API_ERRMSG, "042", AVT_ERROR);
      return NULL;
    }


  strcpy(buf1, towhat);
  tok=strtok_r(buf1, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"latch")==0) latchh=1;
      else if (strcasecmp(tok,"precharge")==0) prech=1;
      else if (strcasecmp(tok,"clockgating")==0) clockgating=1;
      else if (strcasecmp(tok,"breakpoint")==0) breakp=1;
      else if (strcasecmp(tok,"all")==0) latchh=prech=clockgating=breakp=1;
      else
         avt_errmsg(TTV_API_ERRMSG, "048", AVT_ERROR, tok);
      tok=strtok_r(NULL, " ", &c);
    }

  ttv_setsearchexclude(0, 0, &old0, &old1);

  if ((pt=getptype(tf->USER, CONTRAINTOBJECT_REFCOUNT))==NULL)
    {
      com=(ConstraintObjectManagement *)mbkalloc(sizeof(ConstraintObjectManagement));
      com->count=0;
      com->tf=tf;
      com->connectordone=NULL;
      tf->USER=addptype(tf->USER, CONTRAINTOBJECT_REFCOUNT, com);

      // retreiving clock path once
      allnodes=ttv_getsigbytype(tf,NULL,TTV_SIG_TYPEALL,NULL);
      ttv_GetDirectiveConstraintObject(allnodes, &directivecmd, &com->directivedata);
      // setup info for breakpoints
      for (cl=allnodes; cl!=NULL; cl=cl->NEXT)
        {
          tvs=(ttvsig_list *)cl->DATA;
          if ((tvs->TYPE & TTV_SIG_Q)!=0 || (tvs->TYPE & TTV_SIG_B)!=0)
            {
              ttv_setupbreakinfo(tf, &tvs->NODE[0], TTV_FIND_LINE);
              ttv_setupbreakinfo(tf, &tvs->NODE[1], TTV_FIND_LINE);
            }
        }
      freechain(allnodes);

      ttv_GetDirectiveConstraintObject_SetupNodes(directivecmd);
      
      cl=append(ttv_GetInterfaceCommandList(tf), ttv_GetInternalCommandList(tf));
      latchlist=append(cl, dupchainlst(directivecmd));
      cl=append(ttv_GetInterfaceBreakpointList(tf), ttv_GetInternalBreakpointList(tf));
      latchlist=append(cl, latchlist);
      cl=append(ttv_GetInterfacePrechargeList(tf), ttv_GetInternalPrechargeList(tf));
      latchlist=append(cl, latchlist);
      
      TTV_EXPLICIT_START_NODES=allclocks;
      TTV_EXPLICIT_END_NODES=latchlist;
      saveTTV_MAX_PATH_PERIOD=V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE;
      V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE=0;
      ttv_SetSearchMode("findcmd");
      ttv_SetSearchMode("dual");
      cmdsigsmin=associatepathtosig(tf, "*", "*", "*", "min", MINCLOCKPATH_PTYPE, '?', "path");
      freechain(cmdsigsmin);
      cmdsigsmax=associatepathtosig(tf, "*", "*", "*", "max", MAXCLOCKPATH_PTYPE, '?', "path");
      freechain(cmdsigsmax);
      ttv_SetSearchMode("!dual");
      ttv_SetSearchMode("!findcmd");
      V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE=saveTTV_MAX_PATH_PERIOD;
      TTV_EXPLICIT_START_NODES=TTV_EXPLICIT_END_NODES=NULL;

      ttv_GetDirectiveConstraintObject_UnSetupNodes(directivecmd);
      freechain(directivecmd);

      ttv_SetupGeneratedClockPaths(tf);

      if (V_BOOL_TAB[__TMA_ALLOW_ACCESS_AS_CLOCKPATH].VALUE)
        {
          ch=NULL;
          for (cl=latchlist; cl!=NULL; cl=cl->NEXT)
            {
              tvs=(ttvsig_list *)cl->DATA;

              if ((tvs->TYPE & TTV_SIG_Q)!=0)
                ch=addchain(ch, tvs);
            }

          if (ch!=NULL)
            {
              TTV_EXPLICIT_CLOCK_NODES=allclocks;
              TTV_EXPLICIT_END_NODES=ch;
              saveTTV_MAX_PATH_PERIOD=V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE;
              V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE=0;
              cmdsigsmin=associatepathtosig(tf, "*", "*", "*", "min", MINCLOCKPATH_PTYPE, '?', "access");
              freechain(cmdsigsmin);
              cmdsigsmax=associatepathtosig(tf, "*", "*", "*", "max", MAXCLOCKPATH_PTYPE, '?', "access");
              freechain(cmdsigsmax);
              V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE=saveTTV_MAX_PATH_PERIOD;
              TTV_EXPLICIT_CLOCK_NODES=TTV_EXPLICIT_END_NODES=NULL;
            }

          for (cl=ch; cl!=NULL; cl=cl->NEXT)
            {
              tvs=(ttvsig_list *)cl->DATA;
              if ((pt=getptype(tvs->USER, MINCLOCKPATH_PTYPE))!=NULL)
                ttv_sortaddedpaths((chain_list **)&pt->DATA, 0);
              if ((pt=getptype(tvs->USER, MAXCLOCKPATH_PTYPE))!=NULL)
                ttv_sortaddedpaths((chain_list **)&pt->DATA, 1);
            }

          freechain(ch);
        }
      
      freechain(latchlist);
    }
  else
    com=(ConstraintObjectManagement *)pt->DATA;

  freechain(allclocks);

  allobj=NULL;

  // selection des connecteurs sans recherche de datapath
  connectorlist=ttv_GetMatchingSignal(tf, inputconnector, "connector");
  for (cl=com->connectordone; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      tvs->USER=addptype(tvs->USER, CONTRAINTOBJECT_REFCOUNT, NULL);
    }

  cl=NULL;
  while (connectorlist!=NULL)
    {
      tvs=(ttvsig_list *)connectorlist->DATA;
      if (getptype(tvs->USER, CONTRAINTOBJECT_REFCOUNT)==NULL/* && !ttv_IsClock(tvs)*/)
        cl=addchain(cl, tvs);
      connectorlist=delchain(connectorlist, connectorlist);
    }
  connectorlist=cl;

  for (cl=com->connectordone; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      tvs->USER=delptype(tvs->USER, CONTRAINTOBJECT_REFCOUNT);
    }

  if (connectorlist==NULL) return NULL;

  // recuperation des datapaths
  latchlist=NULL;
  if (latchh)
    {
      cl=append(ttv_GetInterfaceLatchList(tf), ttv_GetInternalLatchList(tf));
      latchlist=append(cl, latchlist);
    }
  if (prech)
    {
      cl=append(ttv_GetInterfacePrechargeList(tf), ttv_GetInternalPrechargeList(tf));
      latchlist=append(cl, latchlist);
    }
  if (breakp)
    {
      cl=append(ttv_GetInterfaceBreakpointList(tf), ttv_GetInternalBreakpointList(tf));
      latchlist=append(cl, latchlist);
    }
  if (clockgating)
    {
      latchlist=append(dupchainlst(com->directivedata), latchlist);
      ttv_GetDirectiveConstraintObject_SetupNodes(com->directivedata);
    }

  TTV_EXPLICIT_START_NODES=connectorlist;
  TTV_EXPLICIT_END_NODES=latchlist;
  ttv_SetSearchMode("dual");
  datasigmin=associatepathtosig(tf, "*", "*", "*", "min", MINDATAPATH_PTYPE, '?', "path");
  associatepathtosig_more(tf, latchlist, TTV_FIND_MIN, MINDATAPATH_PTYPE, &datasigmin);
  datasigmax=associatepathtosig(tf, "*", "*", "*", "max", MAXDATAPATH_PTYPE, '?', "path");
  associatepathtosig_more(tf, latchlist, TTV_FIND_MAX, MAXDATAPATH_PTYPE, &datasigmax);
  ttv_SetSearchMode("!dual");
  TTV_EXPLICIT_START_NODES=TTV_EXPLICIT_END_NODES=NULL;

  freechain(datasigmin);
  freechain(datasigmax);

/*  for (cl=connectorlist; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if (getptype(tvs->USER, CONTRAINTOBJECT_REFCOUNT)==NULL)
        tvs->USER=addptype(tvs->USER, CONTRAINTOBJECT_REFCOUNT, NULL);
    }
*/
  com->connectordone=append(connectorlist, com->connectordone);

  if (clockgating)
    ttv_GetDirectiveConstraintObject_UnSetupNodes(com->directivedata);

  
  tot=0;
  for (cl=latchlist; cl!=NULL; cl=cl->NEXT)
    {
      latch=(ttvsig_list *)cl->DATA;

      for (i=0; i<2; i++)
        {
          if ((pt=getptype(latch->USER, i==0?MAXDATAPATH_PTYPE:MINDATAPATH_PTYPE))!=NULL)
            pth=(chain_list *)pt->DATA;
          else
            pth=NULL;

          while (pth!=NULL)
            {
              tp=(ttvpath_list *)pth->DATA;
              in_ev=tp->NODE;
              out_ev=tp->ROOT;
              
              if ((in_ev->ROOT->TYPE & TTV_SIG_C)!=0)
                {
                  if (mbk_TestREGEX(ttv_GetFullSignalName(tf, in_ev->ROOT), inputconnector))
                    {
                      ptype_list *pt;
                      if ((out_ev->ROOT->TYPE & TTV_SIG_L)!=0)
                        cmdlist = ttv_getlatchdatacommands(tf,out_ev,TTV_FIND_LINE|(i==0?TTV_FIND_MIN:TTV_FIND_MAX));
                      else if ((out_ev->ROOT->TYPE & TTV_SIG_B)!=0)
                        cmdlist = ttv_getbreakinfo(out_ev, i==0?1:0);
                      else if ((out_ev->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_R|TTV_SIG_B))!=0)
                        cmdlist = addptype(NULL, 0, out_ev);
                      else
                        cmdlist = NULL;
                      
                      pt=cmdlist;
                      while (cmdlist!=NULL)
                        {
                          checkcmd=ttv_GetClockPathCmd(out_ev, (ttvevent_list *)cmdlist->DATA, (tp->TYPE & TTV_FIND_HZ)!=0?1:0, i==0?1:0, &hz);
                          ch=getcmdpaths_new(tf, checkcmd, i, hz);
                          while (ch!=NULL)
                            {
                              co=(ConstraintObject *)mbkalloc(sizeof(ConstraintObject));
                              co->setup=(i==0?1:0);
                              co->datapath=tp;
                              co->clockpath=(ttvpath_list *)ch->DATA;
                              ttv_get_path_margins(tf, co->datapath, co->clockpath, &margedata, &margeclock, &overlapc, &overlapd,0,0);
                              co->margedata=_LONG_TO_DOUBLE(margedata);
                              co->margeclock=_LONG_TO_DOUBLE(margeclock);
                              co->overlapclock=_LONG_TO_DOUBLE(overlapc);
                              co->overlapdata=_LONG_TO_DOUBLE(overlapd);
                              co->intrinsic_margin_model=NULL;
                              if ((out_ev->ROOT->TYPE & TTV_SIG_L)!=0)
                                {
                                  rline=NULL;
                                  if (i==0)
                                    {
                                      if (!ttv_has_strict_setup(out_ev)) co->intrinsic_margin=ttv_GetLatchIntrinsicQuick(tf, out_ev, co->clockpath->ROOT, 1, &rline);
                                      else co->intrinsic_margin=0;
                                    }
                                  else
                                    {
                                      co->intrinsic_margin=-ttv_GetLatchIntrinsicQuick(tf, out_ev, co->clockpath->ROOT, 0, &rline);
                                    }
                                  co->intrinsic_margin_model=rline?rline->MDMAX:NULL;
                                }
                              else
                                {
                                  if ((out_ev->ROOT->TYPE & TTV_SIG_B)!=0)
                                    {
                                      co->intrinsic_margin=_LONG_TO_DOUBLE(ttv_getdelaymax((ttvline_list *)cmdlist->TYPE));
                                      co->intrinsic_margin_model=((ttvline_list *)cmdlist->TYPE)->MDMAX;
                                    }
                                  else
                                    co->intrinsic_margin=_LONG_TO_DOUBLE(cmdlist->TYPE);
                                }
                              ttv_GetGeneratedClockPathsInfo(co->clockpath->NODE, i, &co->clocklatency, &co->masterck);
                              if (co->masterck==NULL) co->masterck=co->clockpath->NODE;
                              co->com=com;
                              com->count++;
                              ch=delchain(ch, ch);
                              allobj=addchain(allobj, co);
                              tot++;
                            }
                          cmdlist=cmdlist->NEXT;
                        }
                      freeptype(pt);
                      
                      // directives
                      for (j=0; j<2; j++)
                        {
                          pt=cmdlist=ttv_get_directive_slopes(out_ev, i==0?1:0, j,ttv_IsClock(in_ev->ROOT)?1:0);
                          while (cmdlist!=NULL)
                            {
                              checkcmd=(ttvevent_list *)cmdlist->DATA;
                              if (j==0)
                                ch=getcmdpaths_new(tf, checkcmd, i, 0);
                              else
                                ch=getcmdpaths_data_new(tf, checkcmd, i);
                              while (ch!=NULL)
                                {
                                  co=(ConstraintObject *)mbkalloc(sizeof(ConstraintObject));
                                  co->setup=(i==0?1:0);
                                  co->datapath=tp;
                                  co->clockpath=(ttvpath_list *)ch->DATA;
                                  ttv_get_path_margins(tf, co->datapath, co->clockpath, &margedata, &margeclock, &overlapc, &overlapd,0,0);
                                  co->margedata=_LONG_TO_DOUBLE(margedata);
                                  co->margeclock=_LONG_TO_DOUBLE(margeclock);
                                  co->overlapclock=_LONG_TO_DOUBLE(overlapc);
                                  co->overlapdata=_LONG_TO_DOUBLE(overlapd);
                                  co->intrinsic_margin=_LONG_TO_DOUBLE(pt->TYPE);
                                  co->intrinsic_margin_model=NULL;
                                  ttv_GetGeneratedClockPathsInfo(co->clockpath->NODE, i, &co->clocklatency, &co->masterck);
                                  if (co->masterck==NULL) co->masterck=co->clockpath->NODE;
                                  co->com=com;                              
                                  com->count++;
                                  ch=delchain(ch, ch);
                                  allobj=addchain(allobj, co);
                                  tot++;
                                }
                              cmdlist=cmdlist->NEXT;
                            }
                          freeptype(pt);
                        }
                    }
                }
              pth=pth->NEXT;
            }
        }
    }            
  freechain(latchlist);
  ttv_setsearchexclude(old0, old1, &old0, &old1);
  return allobj;
}


void ttv_DisplayConnectorToLatchMargin (FILE *f, ttvfig_list *tf, char *inputconnector, char *mode)
{
  ttvpath_list *tp;
  char        buf1[1024], buf3[256], buf4[256];
  int          i, j, tot, dispall=0, split=0, margins=0, max;
  chain_list *cl, *connectorlist;
  chain_list *allobj, *allsetup, *allhold;
  Board *b;
  void *last;
  long old0, old1;
  char *tok, *c, *saveAUTO, savesimdiffmode=DISPLAY_simdiffmode, doDISPLAY_simdiffmode=0;
  char *towhat="latch";
  ConstraintObject *co;
  BI_ConstraintObject *bicotab;
  int filterclock=1, filtercmd=1, dosetup, dohold;
  chain_list *allnodes, *allclocks;

  if (f==NULL || tf==NULL) return;
  
  if (ftello(f)==0)
    ttv_DumpHeader(f, tf);
            
  strcpy(buf1, mode);
  tok=strtok_r(buf1, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"resume")==0 || strcasecmp(tok,"summary")==0) dispall=0;
      else if (strcasecmp(tok,"all")==0) dispall=1;
      else if (strcasecmp(tok,"split")==0) split=1;
      else if (strcasecmp(tok,"margins")==0) margins=1;
      else if (strcasecmp(tok,"pathcomp")==0) doDISPLAY_simdiffmode=1;
      else if (strcasecmp(tok,"allconstraints")==0) towhat="all", filterclock=2;
      else if (strcasecmp(tok,"full")==0) towhat="all", filterclock=0;
      else
         avt_errmsg(TTV_API_ERRMSG, "041", AVT_ERROR, tok);
      tok=strtok_r(NULL, " ", &c);
    }
    
  avt_fprintf(f, "      *** Connector to Latch report (unit:[%s]) ***\n\n", DISPLAY_time_string);
  cfgclock=filterclock;
  cfgcmd=filtercmd;
  if (split)
    {
      connectorlist=ttv_GetMatchingSignal(tf, inputconnector, "connector");
      for (cl=connectorlist; cl!=NULL; cl=cl->NEXT) cl->DATA=((ttvsig_list *)cl->DATA)->NETNAME;
    }
  else
    connectorlist=addchain(NULL, inputconnector);
    
  saveAUTO=ttv_AutomaticDetailBuild(dispall?"on":"noassoc");
  savesimdiffmode=DISPLAY_simdiffmode;
  DISPLAY_simdiffmode=doDISPLAY_simdiffmode;
  
  ttv_setsearchexclude(0, 0, &old0, &old1);

  while (connectorlist!=NULL)
    {
      allobj=ttv_GetConstraints(tf, (char *)connectorlist->DATA, towhat);

      if (allobj!=NULL)
        {
      
          tot=countchain(allobj);
          ttv_sort_cotab(allobj, ConstraintObjectSort_for_C2L);
      
          allsetup=allhold=NULL;
      
          for (cl=allobj; cl!=NULL; cl=cl->NEXT)
            {
              co=(ConstraintObject *)cl->DATA;
              if (co->setup) allsetup=addchain(allsetup, co);
              else allhold=addchain(allhold, co);
            }
      
          allsetup=reverse(allsetup);
          allhold=reverse(allhold);
      
          bicotab=(BI_ConstraintObject *)mbkalloc(sizeof(BI_ConstraintObject)*tot);
          i=0;
          while (allsetup!=NULL || allhold!=NULL)
            {
              bicotab[i].setup=bicotab[i].hold=NULL;
              dosetup=dohold=0;
          
              if (allhold==NULL) dosetup=1;
              else if (allsetup==NULL) dohold=1;
              else 
                {
                  int res;
                  res=ConstraintObjectSort_for_C2L_cmp((ConstraintObject *)allsetup->DATA, (ConstraintObject *)allhold->DATA, 0, 0, filterclock, filtercmd, 1);
                  if (res<0) dosetup=1;
                  else if (res>0) dohold=1;
                  else dosetup=dohold=1;
                }
      
              if (dosetup)
                {
                  co=(ConstraintObject *)allsetup->DATA;
                  bicotab[i].min=ttv_ComputeConstraintObjectMargin(co);
                  bicotab[i].setup=co;
                  allsetup=ttv_getnext_different_ConstraintObject(allsetup, co, filterclock, filtercmd);
                }
              if (dohold)
                {
                  co=(ConstraintObject *)allhold->DATA;
                  if (!dosetup || (dosetup && bicotab[i].min>ttv_ComputeConstraintObjectMargin(co)))
                    bicotab[i].min=ttv_ComputeConstraintObjectMargin(co);
                  bicotab[i].hold=co;
                  allhold=ttv_getnext_different_ConstraintObject(allhold, co, filterclock, filtercmd);
                }
              i++;
            }

          tot=i;
          if (tot>0)
            {
              qsort(bicotab, i, sizeof(BI_ConstraintObject), BI_ConstraintObjectSort);

              b=Board_CreateBoard();
      
              Board_SetSize(b, COL_INDEX, 5, 'r');
              Board_SetSize(b, COL_CONNECTOR, 10, 'l');
              Board_SetSize(b, COL_CONNECTOR_DIR, 3, 'l');
              Board_SetSize(b, COL_LATCH_C2L, 10, 'l');
              Board_SetSize(b, COL_LATCH_DIR, 3, 'l');
              Board_SetSize(b, COL_SETUP_C2L, 7, DISPLAY_number_justify);
              Board_SetSize(b, COL_HOLD_C2L, 7, DISPLAY_number_justify);
              Board_SetSize(b, COL_DATAMAX, 7, DISPLAY_number_justify);
              Board_SetSize(b, COL_DATAMIN, 7, DISPLAY_number_justify);
              Board_SetSize(b, COL_CLOCKMAX, 7, DISPLAY_number_justify);
              Board_SetSize(b, COL_CLOCKMIN, 7, DISPLAY_number_justify);
      
              if (margins)
                {
                  Board_SetSize(b, COL_DATAMAX_MARGIN, 5, DISPLAY_number_justify);
                  Board_SetSize(b, COL_CLOCKMIN_MARGIN, 5, DISPLAY_number_justify);
                  Board_SetSize(b, COL_DATAMIN_MARGIN, 5, DISPLAY_number_justify);
                  Board_SetSize(b, COL_CLOCKMAX_MARGIN, 5, DISPLAY_number_justify);
                }
      
              Board_SetSep(b, COL_C2L_SEP_CONNECTOR);
              Board_SetSep(b, COL_C2L_SEP_LATCH);
              Board_SetSep(b, COL_C2L_SEP_SETUPHOLD);
      
              Board_NewLine(b);
              Board_SetValue(b, COL_INDEX, "Path");
              Board_SetValue(b, COL_CONNECTOR, "Connector");
              Board_SetValue(b, COL_LATCH_C2L, "Latch");
              sprintf(buf1,"Setup");
              Board_SetValue(b, COL_SETUP_C2L, buf1);
              Board_SetValue(b, COL_DATAMAX, " DataMax");
              Board_SetValue(b, COL_CLOCKMIN, " ClockMin");
              sprintf(buf1,"Hold");
              Board_SetValue(b, COL_HOLD_C2L, buf1);
              Board_SetValue(b, COL_DATAMIN, " DataMin");
              Board_SetValue(b, COL_CLOCKMAX, " ClockMax");
      
              if (margins)
                {
                  Board_SetValue(b, COL_DATAMAX_MARGIN, "Margin");
                  Board_SetValue(b, COL_CLOCKMIN_MARGIN, "Margin");
                  Board_SetValue(b, COL_DATAMIN_MARGIN, "Margin");
                  Board_SetValue(b, COL_CLOCKMAX_MARGIN, "Margin");
                }
      
              Board_NewSeparation(b);
              last=NULL;
              for (i=0 ; i<tot ; i++) 
                {
                  Board_NewLine(b);
                  sprintf(buf1,"%d", i+1);
                  Board_SetValue(b, COL_INDEX, buf1);

                  co=bicotab[i].setup;
                  if (co==NULL) co=bicotab[i].hold;
          
                  if (last!=co->datapath->NODE)
                    {
                      Board_SetValue(b, COL_CONNECTOR, ttv_GetFullSignalName_COND(tf, co->datapath->NODE->ROOT));
                      last=co->datapath->NODE;
                      sprintf(buf1, "(%c)", dirconv(ttv_GetTimingEventDirection(co->datapath->NODE)));
                      Board_SetValue(b, COL_CONNECTOR_DIR, buf1);
                    }
          
                  Board_SetValue(b, COL_LATCH_C2L, ttv_GetFullSignalName_COND(tf, co->datapath->ROOT->ROOT));
                  sprintf(buf1, "(%c)", dirconv(ttv_GetTimingEventDirection(co->datapath->ROOT)));
                  Board_SetValue(b, COL_LATCH_DIR, buf1);
          
                  if (bicotab[i].setup==NULL) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintObjectMargin(bicotab[i].setup)*DISPLAY_time_unit);
                  Board_SetValue(b, COL_SETUP_C2L, buf1);
                  if (bicotab[i].hold==NULL) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintObjectMargin(bicotab[i].hold)*DISPLAY_time_unit);
                  Board_SetValue(b, COL_HOLD_C2L, buf1);
          
                  if (bicotab[i].setup==NULL) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintVALID(bicotab[i].setup)*DISPLAY_time_unit);
                  Board_SetValue(b, COL_DATAMAX, buf1);
                  if (bicotab[i].setup==NULL) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintREQUIRED(bicotab[i].setup)*DISPLAY_time_unit);
                  Board_SetValue(b, COL_CLOCKMIN, buf1);
                  if (bicotab[i].hold==NULL) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintVALID(bicotab[i].hold)*DISPLAY_time_unit);
                  Board_SetValue(b, COL_DATAMIN, buf1);
                  if (bicotab[i].hold==NULL) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintREQUIRED(bicotab[i].hold)*DISPLAY_time_unit);
                  Board_SetValue(b, COL_CLOCKMAX, buf1);

                  if (margins)
                    {
            
                      if (bicotab[i].setup)
                      {
                        sprintf(buf1,DISPLAY_time_format,bicotab[i].setup->margedata*DISPLAY_time_unit);
                        Board_SetValue(b, COL_DATAMAX_MARGIN, buf1);
                        sprintf(buf1,DISPLAY_time_format,bicotab[i].setup->margeclock*DISPLAY_time_unit);
                        Board_SetValue(b, COL_CLOCKMIN_MARGIN, buf1);
                      }
                      if (bicotab[i].hold)
                      {
                        sprintf(buf1,DISPLAY_time_format,bicotab[i].hold->margedata*DISPLAY_time_unit);
                        Board_SetValue(b, COL_DATAMIN_MARGIN, buf1);
                        sprintf(buf1,DISPLAY_time_format,bicotab[i].hold->margeclock*DISPLAY_time_unit);
                        Board_SetValue(b, COL_CLOCKMAX_MARGIN, buf1);
                      }
                    }
                }

              Board_NewSeparation(b);
      
              Board_Display(f, b, "");
              fflush(f);

              Board_FreeBoard(b);

              avt_fprintf(f, "\n\n");

              if (dispall==1)
                {
                  long oldmode;
                  oldmode=detail_forced_mode;
          
                  detail_forced_mode=NOSUPINFO_DETAIL|RELATIVE_DETAIL;

                  for (i=0 ; i<tot ; i++) 
                    {
                      co=bicotab[i].setup;
                      if (co==NULL) co=bicotab[i].hold;

                      if ((co->datapath->ROOT->ROOT->TYPE & TTV_SIG_L)!=0) strcpy(buf1,"LATCH");
                      else if ((co->datapath->ROOT->ROOT->TYPE & TTV_SIG_R)!=0) strcpy(buf1,"PRECHARGE");
                      else if ((co->datapath->ROOT->ROOT->TYPE & TTV_SIG_B)!=0) strcpy(buf1,"BREAKPOINT");
                      else strcpy(buf1,"NODE");

                      fprintf(f, "Path (%d): FROM CONNECTOR %s(%c) TO %s %s(%c)\n\n", i+1,
                              ttv_GetFullSignalName_COND(tf, co->datapath->NODE->ROOT),
                              dirconv(ttv_GetTimingEventDirection(co->datapath->NODE)),
                              buf1,
                              ttv_GetFullSignalName_COND(tf, co->datapath->ROOT->ROOT),
                              dirconv(ttv_GetTimingEventDirection(co->datapath->ROOT))
                              );

                      for (j=0; j<2; j++)
                        {
                          if (j==0) co=bicotab[i].setup; else co=bicotab[i].hold;

                          if (co!=NULL)
                            {
                              sprintf(buf1,DISPLAY_time_format,ttv_ComputeConstraintObjectMargin(co)*DISPLAY_time_unit);
                              fprintf(f, "  %s of %s\n\n", j==0?"SETUP":"HOLD", buf1);
                              max=0;
                              tp=co->datapath;
                              ttvapi_setprefix("     ");
                              PATH_MORE_INFO.enabled=1;
                              if (co->margedata!=0)
                                PATH_MORE_INFO.add[max].label="[PATH MARGIN]", PATH_MORE_INFO.add[max++].val=co->margedata;
                              if (co->intrinsic_margin!=0)
                                PATH_MORE_INFO.add[max].label=(j==0?"[INTRINSIC SETUP]":"[INTRINSIC HOLD]"), PATH_MORE_INFO.add[max++].val=co->intrinsic_margin;
                              PATH_MORE_INFO.add[max++].label=NULL;
                      
                              avt_fprintf(f, "     DATA PATH:\n");
                              ttv_DisplayPathDetail(f, -1, tp);
                      
                              PATH_MORE_INFO.enabled=0;
                      
                              tp=co->clockpath;
                              max=0;
                              PATH_MORE_INFO.enabled=1;
                              if (co->margeclock!=0)
                                PATH_MORE_INFO.add[max].label="[PATH MARGIN]", PATH_MORE_INFO.add[max++].val=co->margeclock;
                              if (co->clocklatency!=0)
                                {
                                  sprintf(buf3,"[LATENCY FROM %s]", ttv_GetFullSignalNetName(tf, co->masterck->ROOT));
                                  PATH_MORE_INFO.add[max].label=buf3, PATH_MORE_INFO.add[max++].val=co->clocklatency;
                                }
                              PATH_MORE_INFO.add[max++].label=NULL;
                      
                              avt_fprintf(f, "     CLOCK PATH:\n");
                              ttv_DisplayPathDetail(f, -1, tp);
                      
                              PATH_MORE_INFO.enabled=0;
                      
                              if (co->overlapdata!=0 || co->overlapclock!=0)
                                {
                                  sprintf(buf3,DISPLAY_signed_time_format, co->overlapdata*DISPLAY_time_unit);
                                  sprintf(buf4,DISPLAY_signed_time_format, co->overlapclock*DISPLAY_time_unit);
                                  avt_fprintf(f, "%s    -> Path Margins overlap: datapath:%s clockpath:%s\n", "     ", buf3, buf4);
                                }
                      
                              avt_fprintf(f, "\n");
                  
                              ttvapi_setprefix("");
                            }
                        }
                    }
                  detail_forced_mode=oldmode;
                }
            }

          mbkfree(bicotab);

          while (allobj!=NULL)
            {
              ttv_FreeConstraintObject_sub((ConstraintObject *)allobj->DATA, 0);
              allobj=delchain(allobj, allobj);
            }

          if (split && connectorlist->NEXT!=NULL)
            fprintf(f, "\n");
        }
      connectorlist=delchain(connectorlist, connectorlist); 
    }

  allnodes=ttv_getsigbytype(tf,NULL,TTV_SIG_TYPEALL,NULL);
  cleanpathonsig(allnodes, MINCLOCKPATH_PTYPE);
  cleanpathonsig(allnodes, MAXCLOCKPATH_PTYPE);
  ttv_FreeConstraintObjectManagement(tf, allnodes);
  freechain(allnodes);

  ttv_setsearchexclude(old0, old1, &old0, &old1);
  ttv_AutomaticDetailBuild(saveAUTO);
  DISPLAY_simdiffmode=savesimdiffmode;

}

chain_list *ttv_GetClockPath(ttvpath_list *tp, ttvevent_list **connector, ttvevent_list **pathcmd)
{
  chain_list *cl, *list, *ret, *allclocksig, *foundcl;
  ttvpath_list *pth;
  char dir[5];
  ttvsig_list *tvs;
  ttvevent_list *tve;
  ttvevent_list *revert_tve;
  int prech=0, mustbehz=0, breakp=0, fflop=0;
  char type;
  stbnode     *node;
  stbck *clock=NULL;
  double cur, this=-1;
  ptype_list *pt;


  *connector=*pathcmd=NULL;
  ret=NULL;
  
  if ((tp->ROOT->ROOT->TYPE & TTV_SIG_R)!=0) prech=1; 
  else if ((tp->ROOT->ROOT->TYPE & TTV_SIG_B)!=0) breakp=1; 
  else if ((tp->ROOT->ROOT->TYPE & TTV_SIG_LF)==TTV_SIG_LF) fflop=1; 
  if ((tve=ttv_GetPathCommand(tp))==NULL)
    {
      if (prech) tve=tp->ROOT;
      else if (breakp)
        {
          if ((pt=getptype(tp->ROOT->USER, STB_BREAK_TEST_EVENT))!=NULL)
            tve=(ttvevent_list *)pt->DATA;
        }
    }
  
  if (tve!=NULL)
    {
      node=stb_getstbnode(tve);

      if (tp->TYPE & TTV_FIND_MAX) type='m'; //min
      else type='M'; // max
      
      if (prech)
        {
          if (!ttv_PathIsHZ(tp))
            {
              if (node!=NULL && node->CK!=NULL)
                {
                  if ((type=='M' && (node->CK->FLAGS & STBCK_MAX_EDGE_NOT_HZ)!=0)
                      || (type=='m' && (node->CK->FLAGS & STBCK_MIN_EDGE_NOT_HZ)!=0))
                    revert_tve=ttv_opposite_event(tve);
                  else
                    revert_tve=tve, mustbehz=1;
                }
              else
                revert_tve=tve, mustbehz=1;
            }
          else
            {
              if (node!=NULL && node->CK!=NULL)
                {
                  if ((type=='M' && (node->CK->FLAGS & STBCK_MAX_EDGE_HZ)!=0)
                      || (type=='m' && (node->CK->FLAGS & STBCK_MIN_EDGE_HZ)!=0))
                    revert_tve=ttv_opposite_event(tve), mustbehz=1;
                  else
                    revert_tve=tve;
                }
              else
                revert_tve=tve;
            }
        }
      else if (breakp || fflop)
        revert_tve=tve;
      else
      {
        if (type=='M' || !ttv_has_strict_setup(tp->ROOT))
          revert_tve=ttv_opposite_event(tve);
        else
          revert_tve=tve;
      }
      
      *pathcmd=tve;
      tve=revert_tve;
      tvs=tve->ROOT;
      
      node=stb_getstbnode(tve);
      allclocksig=NULL;
      if (node==NULL || node->CK==NULL || node->CK->ORIGINAL_CLOCK==NULL || mustbehz)
        sprintf(dir,"?%c",ttv_GetTimingEventDirection(tve));
      else
        {
          /*if (!prech) revert_tve=ttv_opposite_event(node->CK->ORIGINAL_CLOCK);
          else*/ revert_tve=node->CK->ORIGINAL_CLOCK;
          sprintf(dir,"%c%c",ttv_GetTimingEventDirection(revert_tve), ttv_GetTimingEventDirection(tve));
          allclocksig=addchain(NULL, revert_tve->ROOT);
        }

      if (node!=NULL)
        {
          int checkdir;
          if ((tve->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
          else checkdir=0;
          if ((checkdir==0 && (node->CK->FLAGS & STBCK_FAKE_DOWN)!=0)
              || (checkdir==1 && (node->CK->FLAGS & STBCK_FAKE_UP)!=0))
            return NULL;
        }
      //      if (!prech)
      /*      else
              strcpy(dir,"??");*/
      
      if (!allclocksig) allclocksig=ttv_getclocksiglist(tp->FIG);
      ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK|TTV_MORE_OPTIONS_USE_CLOCK_START);

      if (allclocksig!=NULL)
        {
          cl=addchain(NULL, tvs);      
          list=ttv_internal_GetPaths_EXPLICIT(tp->FIG, NULL, allclocksig, cl, dir, -1, "critic", "path", type=='M'?"max":"min");
          /*
            if (type=='M') list=ttv_internal_GetCriticPath(NULL, tvs, dir, -1, 1);
            else list=ttv_internal_GetCriticPath(NULL, tvs, dir, -1, 0);
          */

          freechain(cl);
          freechain(allclocksig);
        }
      else 
        list=NULL;
      ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK|TTV_MORE_OPTIONS_USE_CLOCK_START);

      cl=list;
      foundcl=NULL;
      while (cl!=NULL)
        {
          pth=(ttvpath_list *)cl->DATA;
          tvs=ttv_GetPathStartSignal(pth);
          if (isclock(tvs))
            {
              if (!prech) break;
              if (!mustbehz && !ttv_PathIsHZ(pth)) break;
              if (mustbehz && ttv_PathIsHZ(pth))
                {
                  // to comply with stb hz path in stb_initcmd
                  cur=ttv_GetPathStartTime(pth)+ttv_GetPathDelay(pth);
                  if (this==-1 || (type=='M' && cur>this) || (type=='m' && cur<this))
                    {
                      this=cur;
                      foundcl=cl;
                    }
//                  break;
                }
            }
          cl=cl->NEXT;
        }
      if (foundcl!=NULL) cl=foundcl;
      if (cl!=NULL)
        {
          ret=addchain(ret, cl->DATA);
          cl->DATA=NULL;
        }
      else
        {
          if (list==NULL && (tve->ROOT->TYPE & TTV_SIG_C)!=0 && isclock(tve->ROOT))
            {
              *connector=tve;
            }
        }
      
      ttv_FreePathList(list);
      freechain(list);
    }
  return ret;
}

static ptype_list *fitdouble(double val)
{
  ptype_list *pt;
  pt=addptype(NULL, TYPE_DOUBLE, NULL);
  *(float *)&pt->DATA=(float)val;
  return pt;
}

Property *ttv_GetTimingConstraintProperty (ConstraintObject *co, char *property)
{
    char buf[256];

    if (!co) {
      sprintf (buf, "error_null_timing_constraint");
      return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "CLOCK_PATH")) {
      return addptype (NULL, TYPE_TIMING_PATH, co->clockpath);
    }

    if (!strcasecmp (property, "DATA_PATH")) {
      return addptype (NULL, TYPE_TIMING_PATH, co->datapath);
    }

    if (!strcasecmp (property, "INTRINSIC_MARGIN")) {
      return fitdouble(co->intrinsic_margin);
    }

    if (!strcasecmp (property, "INTRINSIC_MARGIN_MODEL")) {
      return addptype (NULL, TYPE_CHAR, strdup(co->intrinsic_margin_model?co->intrinsic_margin_model:"NULL"));
    }

    if (!strcasecmp (property, "TYPE")) {
      return addptype (NULL, TYPE_CHAR, strdup(co->setup?"setup":"hold"));
    }

    if (!strcasecmp (property, "CLOCK_PATH_MARGIN")) {
      return fitdouble(co->margeclock);
    }

    if (!strcasecmp (property, "DATA_PATH_MARGIN")) {
      return fitdouble(co->margedata);
    }

    if (!strcasecmp (property, "MASTER_CLOCK")) {
      return addptype (NULL, TYPE_TIMING_EVENT, co->masterck);
    }

    if (!strcasecmp (property, "MASTER_CLOCK_LATENCY")) {
      return fitdouble(co->clocklatency);
    }

    if (!strcasecmp (property, "VALUE")) {
      return fitdouble(ttv_ComputeConstraintObjectMargin(co));
    }

    avt_errmsg(TTV_API_ERRMSG, "043", AVT_ERROR, property);
    return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
//fprintf (stderr, "error: unknown property %s\n", property);
//    return NULL;
}

typedef struct wave {
    struct wave *NEXT;
    char *NAME;
    double TIME;
    double SLOPE;
    double VT;
    double SWING;
    double GROUND;
    char SENSE;
} wave;

double slope_convert(double F, double vdd, double vt, char sense, double vth_low, double vth_high)
{
    double v0, v1, x0, x1;
    
    v0 = vth_low * vdd;
    v1 = vth_high * vdd;

    if (sense == 'R') {
        if (v0 >= vt) {
            x0 = atanh((v0-vt)/(vdd-vt));
            x1 = atanh((v1-vt)/(vdd-vt));
        }
        else if (v1 >= vt) {
            x0 = (v0-vt)/(vdd-vt);
            x1 = atanh((v1-vt)/(vdd-vt));
        }
        else {
            x0 = (v0-vt)/(vdd-vt);
            x1 = (v1-vt)/(vdd-vt);
        }
        return (F/(x1-x0));
    }
    else {
        if (v1 < vdd-vt) {
            x1 = atanh((v1-(vdd-vt))/(-(vdd-vt)));
            x0 = atanh((v0-(vdd-vt))/(-(vdd-vt)));
        }
        else if (v0 < vdd-vt) {
            x1 = (v1-(vdd-vt))/(-(vdd-vt));
            x0 = atanh((v0-(vdd-vt))/(-(vdd-vt)));
        }
        else {
            x1 = (v1-(vdd-vt))/(-(vdd-vt));
            x0 = (v0-(vdd-vt))/(-(vdd-vt));
        }
        return (F/(x0-x1));
    }
}

double hitas_tanh(double t, double F, double vdd, double vt, char sense)
{
    double t0;
    double direction, threshold, initial;

    if (sense == 'F') {
        direction = -1;
        threshold = vdd-vt;
        initial = vdd;
    }
    else {
        direction = 1;
        threshold = vt;
        initial = 0;
    }
    
    t0 = F*vt/(vdd-vt);
    
    if (t <= 0) return initial;
    else if (t <= t0) return (threshold + (direction)*(vdd-vt)*((t-t0)/F));
    else return (threshold + (direction)*(vdd-vt)*tanh((t-t0)/F));
}

double get_threshold_time(double F, double vdd, double vt)
{
    if (vt >= vdd/2) return (F*vdd/(2*(vdd-vt)));
    else return ((F*vt)/(vdd-vt) + F * atanh((vdd-2*vt)/(2*(vdd-vt))));
}

void ttv_PlotCompletePathDetail(FILE *f, ttvpath_list *tp, chain_list *detailchain)
{
    chain_list *ptchain;
    ttvcritic_list *detail, *nextdetail;
    timing_model *model, *nextmodel;
    ttvsig_list *startsig;
    wave *ptwave, *headwave = NULL;
    double maxdelay, transient_time, time_step, startlag, timeshift, simtime;
    float low, high;
    
    startsig = tp->NODE->ROOT;
    for (ptchain = detailchain; ptchain; ptchain = ptchain->NEXT) {
        detail = (ttvcritic_list *)ptchain->DATA;
        if (detail->LINEMODELNAME) {
            model = stm_getmodel(detail->MODNAME, detail->LINEMODELNAME);
        }
        else model = NULL;
        nextmodel = NULL;
        if (ptchain->NEXT) {
            nextdetail = (ttvcritic_list *)ptchain->NEXT->DATA;
            if (nextdetail->LINEMODELNAME) {
                nextmodel = stm_getmodel(nextdetail->MODNAME, nextdetail->LINEMODELNAME);
            }
        }
        ptwave = malloc(sizeof(wave));
        ptwave->NEXT = headwave;
        ptwave->NAME = detail->NAME;
        ptwave->TIME = ((detail->DELAY + detail->DATADELAY)*1e-12/TTV_UNIT);
        if (headwave) ptwave->TIME += headwave->TIME;
        ptwave->SLOPE = detail->SLOPE*1e-12/TTV_UNIT;
        ptwave->GROUND = 0;
        if (model) ptwave->SWING = model->VDD;
        else {
            ptwave->SWING = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
            if (ptchain == detailchain) {
                if (ttv_get_signal_swing(tp->FIG, startsig, &low, &high) == 0) {
                    ptwave->SWING = high - low;
                    ptwave->GROUND = low;
                }
            }
        }
        if (nextmodel) ptwave->VT = nextmodel->VT;
        else ptwave->VT = STM_DEFAULT_VT;
        ptwave->SENSE = (detail->SNODE == TTV_UP)?'R':'F';
        ptwave->SLOPE = slope_convert(ptwave->SLOPE, ptwave->SWING, ptwave->VT, ptwave->SENSE, SIM_VTH_LOW, SIM_VTH_HIGH);
        headwave = ptwave;
    }

    maxdelay = 2*headwave->TIME;
    headwave = (wave *)reverse((chain_list *)headwave);
    startlag = get_threshold_time(headwave->SLOPE, headwave->SWING, headwave->VT);
    maxdelay += startlag;
    if (maxdelay > V_FLOAT_TAB[__SIM_TIME].VALUE) transient_time = maxdelay;
    else transient_time = V_FLOAT_TAB[__SIM_TIME].VALUE;
    time_step = V_FLOAT_TAB[__SIM_TRAN_STEP].VALUE;



    fputs("#TIME ", f);
    for (ptwave = headwave; ptwave; ptwave = ptwave->NEXT) {
        fputs(" ", f);
        fputs(ptwave->NAME, f);
    }
    fputs("\n", f);
    for (simtime = 0; simtime < transient_time + time_step; simtime += time_step) {
        fprintf(f, "% .5e", simtime);
        for (ptwave = headwave; ptwave; ptwave = ptwave->NEXT) {
            timeshift = startlag + ptwave->TIME - get_threshold_time(ptwave->SLOPE, ptwave->SWING, ptwave->VT);
            fprintf(f, " %- .5e", hitas_tanh(simtime-timeshift, ptwave->SLOPE, ptwave->SWING, ptwave->VT, ptwave->SENSE) + ptwave->GROUND);
        }
        fputs("\n", f);
    }
}

void ttv_PlotPathDetail(FILE *f, ttvpath_list *tp)
{
    chain_list *detail;

    if (tp==NULL) return;

    detail=ttv_GetPathDetail(tp);
    if (detail==NULL) return;

    ttv_PlotCompletePathDetail(f, tp, detail);

    ttv_FreePathDetail(detail);
    freechain(detail);
}

void ttv_ChangePathStartTime(ttvpath_list *tp, double time)
{
  tp->DELAYSTART=_DOUBLE_TO_LONG(time);
  if (tp->CRITIC)
  {
    tp->CRITIC->DELAY=tp->CRITIC->REFDELAY=tp->DELAYSTART;
    if (tp->CRITIC->SIMDELAY!=TTV_NOTIME) tp->CRITIC->SIMDELAY=tp->DELAYSTART;
  }
}
