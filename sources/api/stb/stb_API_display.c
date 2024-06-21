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

#define  API_USE_REAL_TYPES

#include "../ttv/ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "../ttv/ttv_API_display.h"
#include "../ttv/ttv_API_util.h"
#include "stb_LOCAL.h"
#include "stb_API.h"

#define DEC  "   "
#define DEC0 "  "

static char *
vecname(char *name)
{
  return mbk_vect(mbk_decodeanyvector(name), '[', ']');
}


static char dirconv(char dir)
{
  if (dir=='u') return 'R';
  else if (dir=='d') return 'F';
  exit(5);
}

static void stb_get_path_info(ttvsig_list *tvs, int setup, ttvsig_list **start, ttvsig_list **latch)
{
  ttvsig_list *inssig;
  chain_list *list;
  ttvpath_list *pth;
  stbdebug_list *max;
  double val;
  long lastmode;
  SH_info shi;
  char search[4];

  *latch=*start=NULL;
  lastmode=stbsetdebugflag(STB_DIFFERENTIATE_INPUT_EVENTS);

  val=getsetuphold(tvs,setup,2,NULL, NULL, NULL, &shi);
  max=shi.dbl;
/*
  if (setup)
    val=___findworst(tvs, &max, stb_GetSetup);
  else
    val=___findworst(tvs, &max, stb_GetHold);
*/
  if (max!=NULL)
    {          
      inssig=stb_GetPathInputSignal(max);
    
      sprintf(search, "?%c", shi.latch_event);
      ttv_activate_path_and_access_mode(1);
      if (setup) list=ttv_internal_GetAccess(inssig, tvs, "*", search, 1); //max
      else list=ttv_internal_GetAccess(inssig, tvs, "*", search, 0); // min
      ttv_activate_path_and_access_mode(0);
      
      list=ttv_internal_filterpathbycommand(list, max->CMD_EVENT, max->SIG1_EVENT, 1);

      if (list!=NULL)
        {
          pth=(ttvpath_list *)list->DATA;
          if (pth->LATCH!=NULL) *latch=pth->LATCH->ROOT;
          *start=pth->NODE->ROOT;
        }
      ttv_FreePathList(list);
      freechain(list);
    }
  stbsetdebugflag(lastmode);
}


void    stb_DisplayErroneousSignals (FILE *f, chain_list *errorlist)
{
  chain_list  *ch;
  ttvsig_list *sig, *start, *latch;
  char        *name, buf1[15];
  double       setup, hold;
  int          i, who=-1;
  Board *b;
  
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayErroneousSignals");

  if (f==NULL) return;

  b=Board_CreateBoard();

  Board_SetSize(b, COL_INDEX, 5, 'r');
  _ttv_Board_SetSize(STAB_CONFIG_SHOW, COL_START, b, COL_START, 10, 'l');
  _ttv_Board_SetSize(STAB_CONFIG_SHOW, COL_LATCH, b, COL_LATCH, 10, 'l');
  _ttv_Board_SetSep(STAB_CONFIG_SHOW, COL_START, b, COL_ERRL_SEP0);
  _ttv_Board_SetSep(STAB_CONFIG_SHOW, COL_LATCH, b, COL_ERRL_SEP1);
  Board_SetSize(b, COL_NAME, 10, 'l');
  Board_SetSize(b, COL_SETUP, 7, DISPLAY_number_justify);
  Board_SetSize(b, COL_HOLD, 7, DISPLAY_number_justify);
  Board_SetSep(b, COL_ERRL_SEP2);
  Board_SetSep(b, COL_ERRL_SEP3);

  Board_NewLine(b);
  Board_SetValue(b, COL_NAME, "To Node");
  sprintf(buf1,"Setup");
  Board_SetValue(b, COL_SETUP, buf1);
  sprintf(buf1,"Hold");
  Board_SetValue(b, COL_HOLD, buf1);
  _ttv_Board_SetValue(STAB_CONFIG_SHOW, COL_START, b, COL_START, "From node");
  _ttv_Board_SetValue(STAB_CONFIG_SHOW, COL_LATCH, b, COL_LATCH, "Thru node");

  Board_NewSeparation(b);

  for (i=0,ch=errorlist ; ch ; ch=ch->NEXT, i++) 
    {
      Board_NewLine(b);
      sig = (ttvsig_list*)ch->DATA;
      name = ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(sig), sig);
      setup = __stb_GetSignalSetup(sig, '?'); //stb_GetErroneousSignalSetup (sig);
      hold = __stb_GetSignalHold(sig, '?'); //stb_GetErroneousSignalHold (sig);

      sprintf(buf1,"%d",i+1);
      Board_SetValue(b, COL_INDEX, buf1);

      if (setup==-1) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,setup*DISPLAY_time_unit);
      Board_SetValue(b, COL_SETUP, buf1);

      if (hold==-1) strcpy(buf1,"none"); else sprintf(buf1,DISPLAY_time_format,hold*DISPLAY_time_unit);
      Board_SetValue(b, COL_HOLD, buf1);
 
      Board_SetValue(b, COL_NAME, name);

      if (setup==-1) { if (hold!=-1) who=0; }
      else if (hold==-1) { if (setup!=-1) who=1; }
      else if (setup!=-1 && hold!=-1) { if (hold<setup) who=0; else who=1; }

      if (who!=-1 && (STAB_CONFIG_SHOW[COL_START] || STAB_CONFIG_SHOW[COL_LATCH]))
        {
          stb_get_path_info(sig, who, &start, &latch);
         _ttv_Board_SetValue(STAB_CONFIG_SHOW, COL_START, b, COL_START, ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(start), start));
          if (latch!=NULL)
            _ttv_Board_SetValue(STAB_CONFIG_SHOW, COL_LATCH, b, COL_LATCH, ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(latch), latch));
        }
    }

  Board_NewSeparation(b);

  avt_fprintf(f, "      *** Error report (unit:[%s]) ***\n\n", DISPLAY_time_string);

  Board_Display(f, b, "");
  Board_FreeBoard(b);

  avt_fprintf(f, "\n");
}

void stb_DisplayErrorList (FILE *f, stbfig_list *stbfig, double margin, int nberror)
{
    long         minsetup  ;
    long         minhold  ;
    int          errnum;
    chain_list  *errlist;

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayErrorList");
    if (stbfig==NULL) return;

    if (margin>1e-3) margin=1e-3;
    margin*=TTV_UNIT;
    errlist  = stb_geterrorlist (stbfig, (long)(margin*1e12), nberror, &minsetup, &minhold, &errnum);
    stb_DisplayErroneousSignals (f, errlist);
    freechain(errlist);
}



static void    stb_DisplayRanges (FILE *f, chain_list *ranges, double period)
{
    chain_list  *ch;
    double       start, end;
    char buf[20], buf1[20];

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayRanges");
    if (f==NULL) return;

    for (ch=ranges ; ch ; ch=ch->NEXT) {
        start = stb_GetInstabilityRangeStart((stbpair_list*)ch->DATA);
/*        while (start < 0)
            start += period;*/
        end = stb_GetInstabilityRangeEnd((stbpair_list*)ch->DATA);
/*        while (end < 0)
            end += period;*/
        fprintf (f, "___[%s   %s]___", FormaT(start*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT(end*DISPLAY_time_unit, buf1, DISPLAY_time_format));
    }
}

void    stb_DisplayInputInstabilityRanges (FILE *f, stbdebug_list *input)
{
    chain_list  *range;
    double       period;
    ttvsig_list *sg;

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayInputInstabilityRanges");
    if (f==NULL) return;

    period = stb_GetClockPeriod (input);
    range = stb_GetInputInstabilityRanges(input, 'm');
    sg=stb_GetPathInputSignal(input);
    fprintf (f, DEC"Input (%s) : ", ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(sg), sg));
    stb_DisplayRanges (f, range, period);
    freechain (range);
    fprintf (f, "\n");
}

void    stb_DisplayOutputInstabilityRanges (FILE *f, stbdebug_list *input)
{
    chain_list  *range;
    double       period;
    ttvsig_list *sg;

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayOutputInstabilityRanges");
    if (f==NULL) return;

    period = stb_GetClockPeriod (input);
    range = stb_GetOutputInstabilityRanges (input, 'm');
    sg=stb_GetPathOutputSignal(input);
    fprintf (f, DEC"Output (%s) : ", ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(sg), sg));
    stb_DisplayRanges (f, range, period);
    freechain (range);
    fprintf (f, "\n");
}

void    stb_DisplayClock (FILE *f, stbdebug_list *input, int clock)
{
    double  ckup_time, ckdn_time, ckup_delta, ckdn_delta;
    char buf[20], buf1[20];

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayClock");
    if (f==NULL) return;

    ckup_time   = stb_GetClockTime (input, clock, 'u');
    ckdn_time   = stb_GetClockTime (input, clock, 'd');
    if ((ckup_time==-1)||(ckdn_time==-1))
        return;
    ckup_delta  = stb_GetClockDelta (input, clock, 'u');
    ckdn_delta  = stb_GetClockDelta (input, clock, 'd');
   
    switch (clock) {
        case INPUT_CLOCK    :   fprintf (f, DEC0"input clock : "); break;
        case OUTPUT_CLOCK   :   fprintf (f, DEC0"output clock : "); break;
        case MAIN_CLOCK     :   fprintf (f, DEC0"clock (%s) : ", stb_GetClockName (input)); break;
    }

    if (ckup_delta > 0)
        fprintf (f, "RISE (%s:%s), ", FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT((ckup_time+ckup_delta)*DISPLAY_time_unit, buf1, DISPLAY_time_format));
    else
        fprintf (f, "RISE %s, ", FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format));

    if (ckdn_delta > 0)
        fprintf (f, "FALL (%s:%s) ", FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT((ckdn_time+ckdn_delta)*DISPLAY_time_unit, buf1, DISPLAY_time_format));
    else
        fprintf (f, "FALL %s ", FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format));

    
    if (clock == MAIN_CLOCK)
        fprintf (f, ", PERIOD = %s", FormaT(stb_GetClockPeriod (input)*DISPLAY_time_unit, buf, DISPLAY_time_format));
    
    fprintf (f, "\n");
}

void    stb_DisplayOutputSpecificationRanges (FILE *f, stbdebug_list *input)
{
    chain_list  *range;
    double       period;
    
    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayOutputSpecificationRanges");
    if (f==NULL) return;

    if ((range=stb_GetOutputSpecificationRanges (input, 'm'))) {
        period = stb_GetClockPeriod (input);
        fprintf (f, DEC"Specs : ");
        stb_DisplayRanges (f, range, period);
        freechain (range);
        fprintf (f, "\n");
    }
}

void    stb_DisplayInfos (FILE *f, stbdebug_list *input)
{
    double   margin;
    char     buf[1024], *str;
    double   s;
    ttvsig_list *isg, *osg;

    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayInfos");
    if (f==NULL) return;

    if ((str = stb_GetCommandName (input)))
        sprintf (buf, " (%s) ", str);
    else
        strcpy (buf," ");

    isg=stb_GetPathInputSignal(input);
    osg=stb_GetPathOutputSignal(input);
    fprintf (f, "%s%s-> %s :", ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(isg), isg), buf, ttv_GetFullSignalName_COND (ttv_GetSignalTopTimingFigure(osg), osg));
    if ((s=stb_GetSetup (input))!=-1) fprintf (f, " setup = %s", FormaT(s*DISPLAY_time_unit, buf, DISPLAY_time_format));
    if ((s=stb_GetHold (input))!=-1) fprintf (f, "  hold = %s", FormaT(s*DISPLAY_time_unit, buf, DISPLAY_time_format));
    if ((margin = stb_GetSetupMargin (input)) > 0)
        fprintf (f, "  setup margin = %s", FormaT(margin*DISPLAY_time_unit, buf, DISPLAY_time_format));
    if ((margin = stb_GetHoldMargin (input)) > 0)
        fprintf (f, "  hold margin = %s", FormaT(margin*DISPLAY_time_unit, buf, DISPLAY_time_format));

    fprintf (f, "\n");
}

void    stb_DisplayInputInfos (FILE *f, stbdebug_list *input)
{
    avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayInputInfos");
    stb_DisplayInfos (f, input);
    stb_DisplayClock (f, input, MAIN_CLOCK);
    stb_DisplayClock (f, input, INPUT_CLOCK);
    stb_DisplayInputInstabilityRanges (f, input);
    stb_DisplayClock (f, input, OUTPUT_CLOCK);
    stb_DisplayOutputInstabilityRanges (f, input);
    stb_DisplayOutputSpecificationRanges (f, input);
}


void stb_DisplaySetupTimeReport (FILE *f, stbdebug_list *max, char datadir)
{
  double  ckup_time, ckdn_time, ckup_delta, ckdn_delta, val=DBL_MAX, v, add;
  double maxdd=-DBL_MAX;
  chain_list *cl, *ch;
  ttvsig_list *tvs;
  char buf[20], buf1[20];
  char *rise="RISE", *fall="FALL", *ckname;
  stbnode *node;
  stbfig_list *sb;
  ttvfig_list *tvf;
  ttvevent_list *tve;
  int inverted=0;

  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplaySetupTimeReport");
  if (max==NULL || f==NULL) return;

  tvs=stb_GetPathOutputSignal(max);

  if ((val=stb_GetSetup(max))==-1) val=DBL_MAX; //val=___findworst(tvs, &max, stb_GetSetup);

  cl=stb_GetOutputInstabilityRanges(max,'m');
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      v=stb_GetInstabilityRangeEnd((stbpair_list *)ch->DATA);
      if (v>maxdd) { maxdd=v; }
    }
  freechain(cl);
  
  if (maxdd==-DBL_MAX) return;

  v=stb_GetClockPeriod (max);
  
  ckup_time   = stb_GetClockTime (max, MAIN_CLOCK, 'u');
  ckdn_time   = stb_GetClockTime (max, MAIN_CLOCK, 'd');
  
  if (!(ckup_time==-1 || ckdn_time==-1))
    {
      ckup_time+=stb_periodmove('x', max);
      ckdn_time+=stb_periodmove('x', max);
      ckup_delta  = stb_GetClockDelta (max, MAIN_CLOCK, 'u');
      ckdn_delta  = stb_GetClockDelta (max, MAIN_CLOCK, 'd');
      
      avt_fprintf(f, DEC"CLOCK %-5s  :", stb_GetClockName (max));
      if (ckup_delta>0) avt_fprintf (f, " RISE = (%s:%s)", FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT((ckup_time+ckup_delta)*DISPLAY_time_unit, buf1, DISPLAY_time_format));
      else avt_fprintf (f, " RISE = %s", FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format));
      if (ckdn_delta>0) avt_fprintf (f, " FALL = (%s:%s)", FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT((ckdn_time+ckdn_delta)*DISPLAY_time_unit, buf1, DISPLAY_time_format));
      else avt_fprintf (f, " FALL = %s", FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf (f, " PERIOD = %s", FormaT(stb_GetClockPeriod(max)*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf(f,"\n");
    }

  ckup_time   = stb_GetClockTime (max, OUTPUT_CLOCK, 'u');
  ckdn_time   = stb_GetClockTime (max, OUTPUT_CLOCK, 'd');

  if (!(ckup_time==-1 || ckdn_time==-1))
    {
      ckup_time+=stb_periodmove('u', max);
      ckdn_time+=stb_periodmove('d', max);
      ckup_delta  = stb_GetClockDelta (max, OUTPUT_CLOCK, 'u');
      ckdn_delta  = stb_GetClockDelta (max, OUTPUT_CLOCK, 'd');

      if (max->CMD_EVENT!=NULL)
        {
          tvf=ttv_GetSignalTopTimingFigure(max->CMD_EVENT->ROOT);
          if (datadir=='d') tve=stb_GetPathOutputSignal(max)->NODE;
          else tve=stb_GetPathOutputSignal(max)->NODE+1;
          ckname=ttv_GetFullSignalName_COND(tvf, max->CMD_EVENT->ROOT);
          node=stb_getstbnode(tve);
          if (node!=NULL)
            {
              sb= stb_getstbfig(tvf);
              stb_getgoodclock_and_status(sb, node->CK, max->CMD_EVENT, tve, &inverted);
              if (inverted) rise="FALL", fall="RISE";
            }
        }
      else ckname="";
      avt_fprintf(f, DEC"CLOCK %-5s :", ckname);
      avt_fprintf (f, " %s = %s [MIN]", rise, FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf (f, " %s = %s [MIN]", fall, FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf(f,"\n");
    }

  if (stb_GetSetupMargin(max)!=-1) add=stb_GetSetupMargin(max); else add=0;
  avt_fprintf(f, DEC"DATA DELAY   : %s [MAX]", FormaT((maxdd+add)*DISPLAY_time_unit, buf, DISPLAY_time_format));
  if (add!=0)
    avt_fprintf(f, " (%s + %s)",  FormaT((maxdd)*DISPLAY_time_unit, buf1, DISPLAY_time_format), FormaT(add*DISPLAY_time_unit, buf, DISPLAY_time_format));
  avt_fprintf(f, "\n");

  if (val!=DBL_MAX)
    avt_fprintf(f, DEC"SETUP        : %s%s" AVT_RESET "\n", val<0? AVT_RED :"", FormaT(val*DISPLAY_time_unit, buf, DISPLAY_time_format));
  else
    avt_fprintf(f, DEC"SETUP        : none\n");
}

void stb_DisplayHoldTimeReport (FILE *f, stbdebug_list *max, char datadir)
{
  double  ckup_time, ckdn_time, ckup_delta, ckdn_delta, val=DBL_MAX, v, sub;
  double mindd=DBL_MAX;
  chain_list *cl, *ch;
  ttvsig_list *tvs;
  char buf[20], buf1[20];
  char *rise="RISE", *fall="FALL", *ckname;
  stbnode *node;
  stbfig_list *sb;
  ttvfig_list *tvf;
  ttvevent_list *tve;
  int inverted=0;

  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayHoldTimeReport");
  if (max==NULL || f==NULL) return;

  tvs=stb_GetPathOutputSignal(max);

  if ((val=stb_GetHold(max))==-1) val=DBL_MAX; //  val=___findworst(tvs, &max, stb_GetHold);

  cl=stb_GetOutputInstabilityRanges(max, 'm');
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      v=stb_GetInstabilityRangeStart((stbpair_list *)ch->DATA);
      if (v<mindd) { mindd=v; }
    }
  freechain(cl);

  if (mindd==DBL_MAX) return;

  v=stb_GetClockPeriod (max);
      
  ckup_time   = stb_GetClockTime (max, MAIN_CLOCK, 'u');
  ckdn_time   = stb_GetClockTime (max, MAIN_CLOCK, 'd');
  if (!(ckup_time==-1 || ckdn_time==-1))
    {
      ckup_time+=-v+stb_periodmove('x', max);
      ckdn_time+=-v+stb_periodmove('x', max);

      ckup_delta  = stb_GetClockDelta (max, MAIN_CLOCK, 'u');
      ckdn_delta  = stb_GetClockDelta (max, MAIN_CLOCK, 'd');
      
      avt_fprintf(f, DEC"CLOCK %-5s  :", stb_GetClockName (max));
      if (ckup_delta>0) avt_fprintf (f, " RISE = (%s:%s)", FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT((ckup_time+ckup_delta)*DISPLAY_time_unit, buf1, DISPLAY_time_format));
      else avt_fprintf (f, " RISE = %s", FormaT(ckup_time*DISPLAY_time_unit, buf, DISPLAY_time_format));
      if (ckdn_delta>0) avt_fprintf (f, " FALL = (%s:%s)", FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format), FormaT((ckdn_time+ckdn_delta)*DISPLAY_time_unit, buf1, DISPLAY_time_format));
      else avt_fprintf (f, " FALL = %s", FormaT(ckdn_time*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf (f, " PERIOD = %s", FormaT(stb_GetClockPeriod(max)*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf(f,"\n");
    }

  ckup_time   = stb_GetClockTime (max, OUTPUT_CLOCK, 'u');
  ckdn_time   = stb_GetClockTime (max, OUTPUT_CLOCK, 'd');

  if (!(ckup_time==-1 || ckdn_time==-1))
    {
      ckup_time+=-v+stb_periodmove('u', max);
      ckdn_time+=-v+stb_periodmove('d', max);
      ckup_delta  = stb_GetClockDelta (max, OUTPUT_CLOCK, 'u');
      ckdn_delta  = stb_GetClockDelta (max, OUTPUT_CLOCK, 'd');
      
      if (max->CMD_EVENT!=NULL)
        {
          tvf=ttv_GetSignalTopTimingFigure(max->CMD_EVENT->ROOT);
          if (datadir=='d') tve=stb_GetPathOutputSignal(max)->NODE;
          else tve=stb_GetPathOutputSignal(max)->NODE+1;
          ckname=ttv_GetFullSignalName_COND(tvf, max->CMD_EVENT->ROOT);
          node=stb_getstbnode(tve);
          if (node!=NULL)
            {
              sb= stb_getstbfig(tvf);
              stb_getgoodclock_and_status(sb, node->CK, max->CMD_EVENT, tve, &inverted);
              if (inverted) rise="FALL", fall="RISE";
            }
        }
      else ckname="";
      avt_fprintf(f, DEC"CLOCK %-5s :", ckname);
      avt_fprintf (f, " %s = %s [MAX]", rise, FormaT((ckup_time+ckup_delta)*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf (f, " %s = %s [MAX]", fall, FormaT((ckdn_time+ckdn_delta)*DISPLAY_time_unit, buf, DISPLAY_time_format));
      avt_fprintf(f,"\n");
    }

  if (stb_GetHoldMargin(max)!=-1) sub=stb_GetHoldMargin(max); else sub=0;
  avt_fprintf(f, DEC"DATA DELAY   : %s [MIN]", FormaT((mindd-sub)*DISPLAY_time_unit, buf, DISPLAY_time_format));
  if (sub!=0)
    avt_fprintf(f, " (%s - %s)", FormaT(mindd*DISPLAY_time_unit, buf1, DISPLAY_time_format), FormaT(sub*DISPLAY_time_unit, buf, DISPLAY_time_format));
  avt_fprintf(f, "\n");
  if (val!=DBL_MAX)
    avt_fprintf(f, DEC"HOLD         : %s%s" AVT_RESET "\n",  val<0?AVT_RED:"", FormaT(val*DISPLAY_time_unit, buf, DISPLAY_time_format));
  else
    avt_fprintf(f, DEC"HOLD         : none\n");
}

void stb_DisplayClockRoute(FILE *f, stbdebug_list *max, char type)
{
  ttvevent_list *tve;
  ttvsig_list *tvs;
 
  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayClockRoute");
  type=tolower(type);
  if (type!='s' && type!='h')
    {
      avt_errmsg(STB_API_ERRMSG, "005", AVT_ERROR);
      //avt_fprintf(f, "valid values for type is 's' (setup) or 'h' (hold)");
      return;
    }

  if (f==NULL) return;

  tvs=stb_GetPathOutputSignal(max);

  if (max->CMD_EVENT==NULL) {avt_fprintf(f, AVT_RED "<no path to clock found, unknown command node>" AVT_RESET); return; }

  if (max->CMD_EVENT->TYPE & TTV_NODE_UP) tve=max->CMD_EVENT->ROOT->NODE;
  else tve=max->CMD_EVENT->ROOT->NODE+1;

  ttv_DisplayRoute(f, tve, type=='s'?'m':'M', NULL, 0);
}

void stb_DisplayDataRoute(FILE *f, stbdebug_list *max, char type, char datadir)
{
  ttvsig_list *inssig, *tvs;
  chain_list *cl, *list;
  chain_list *detail, *detailbase;
  ttvcritic_list *tc;
  char *nodename, *col, *_type;
  ttvpath_list *pth;
  char search[4];

  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplayDataRoute");
  if (max==NULL || f==NULL) return;

  type=tolower(type);

  tvs=stb_GetPathOutputSignal(max);

  if (type!='s' && type!='h')
    {
      max=NULL;
      avt_errmsg(STB_API_ERRMSG, "005", AVT_ERROR);
//      avt_fprintf(f, "valid values for type is 's' (setup) or 'h' (hold), ");
    }

  if (max!=NULL)
    {          
      inssig=stb_GetPathInputSignal(max);
      
      sprintf(search,"?%c",datadir);

      ttv_activate_path_and_access_mode(1);
      if (type=='s') list=ttv_internal_GetAccess(inssig, tvs, "*", search, 1); //max
      else list=ttv_internal_GetAccess(inssig, tvs, "*", search, 0); // min
      ttv_activate_path_and_access_mode(0);
      
      list=ttv_internal_filterpathbycommand(list, max->CMD_EVENT, max->SIG1_EVENT, 1);

      cl=list;

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
                  if (strstr(_type,"latch")!=NULL || strstr(_type,"flip-flop")!=NULL) col=AVT_YELLOW;
                  else col="";
                  avt_fprintf(f, "%s%s" AVT_RESET "(%c)", col, nodename, dirconv(ttv_GetDetailDirection(tc)));
                }
              detail=detail->NEXT;
            }
          avt_fprintf(f, ")");

          ttv_FreePathDetail(detailbase);
          freechain(detailbase);
        }
      else
        avt_fprintf(f, AVT_RED "<access not found>" AVT_RESET);
      ttv_FreePathList(list);
      freechain(list);
    }
  else avt_fprintf(f, AVT_RED "<not found>" AVT_RESET);
}


void stb_DisplaySignalStabilityReport (FILE *f, stbfig_list *stbfig, char *name)
{
  stbdebug_list *max;
  double val;
  long lastmode;
  SH_info shi;
  ttvsig_list *tvs;

  avt_errmsg(STB_API_ERRMSG, "007", AVT_WARNING, "stb_DisplaySignalStabilityReport");
  if (!stbfig) return;

  tvs = ttv_getsig(stbfig->FIG, vecname(name));

  if (tvs==NULL || f==NULL) return;
  lastmode=stbsetdebugflag(STB_DIFFERENTIATE_INPUT_EVENTS);

  avt_fprintf(f, AVT_BLUE "Signal '%s'" AVT_RESET " (unit:[%s])\n", ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(tvs), tvs), DISPLAY_time_string);

  val=getsetuphold(tvs,1,2,NULL, NULL, NULL, &shi);
  max=shi.dbl;
//  val=___findworst(tvs, &max, stb_GetSetup);
  if (stb_GetSetup(max)==-1)
    avt_fprintf(f, " [" AVT_BOLD "No Setup Time Available" AVT_RESET "]\n");
  else if (val!=DBL_MAX && val>=0)
    avt_fprintf(f," [Setup Time]\n");
  else
    avt_fprintf(f," [" AVT_RED "Setup Time Error" AVT_RESET "]\n");
   
  //display setup path
  avt_fprintf(f, DEC0"Data route : ");
  if (max!=NULL) { stb_DisplayDataRoute(f, max, 's', shi.latch_event); avt_fprintf(f, "\n"); }
  avt_fprintf(f, DEC0"Closing Clock route : ");
  if (max!=NULL) { stb_DisplayClockRoute(f, max, 's'); avt_fprintf(f, "\n"); }
  avt_fprintf(f, "\n");
  stb_DisplaySetupTimeReport (f, max, shi.latch_event);

  avt_fprintf(f, "\n");

  val=getsetuphold(tvs,0,2,NULL, NULL, NULL, &shi);
  max=shi.dbl;
//  val=___findworst(tvs, &max, stb_GetHold);
  if (stb_GetHold(max)==-1)
    avt_fprintf(f," [" AVT_BOLD "No Hold Time Available" AVT_RESET "]\n");
  else if (val!=DBL_MAX && val>=0)
    avt_fprintf(f," [Hold Time]\n");
  else
    avt_fprintf(f," [" AVT_RED "Hold Time Error" AVT_RESET "]\n");

  //display hold path
  avt_fprintf(f, DEC0"Data route : ");
  if (max!=NULL) { stb_DisplayDataRoute(f, max, 'h', shi.latch_event); avt_fprintf(f, "\n"); }
  avt_fprintf(f, DEC0"Closing Clock route : ");
  if (max!=NULL) { stb_DisplayClockRoute(f, max, 'h'); avt_fprintf(f, "\n"); }
  avt_fprintf(f, "\n");
  stb_DisplayHoldTimeReport (f, max, shi.latch_event);
  avt_fprintf(f, "\n");
  stbsetdebugflag(lastmode);
}

static int isclock(ttvsig_list *ts)
{
  if (getptype(ts->USER, TTV_SIG_CLOCK)!=NULL) return 1;
  return 0;
}

void ttv_CheckTransformToIdeal(ttvpath_list * tp)
{
  chain_list *cl, *ch;
  long sub, subr;
  ttvcritic_list *tc;
  int latchideal=0, clockideal=0, mode=0;

  if (tp==NULL) return;

  if (tp->LATCH && getptype(tp->LATCH->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) latchideal=1;
  if (getptype(tp->NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) clockideal=1;

  ch=cl=ttv_GetPathDetail(tp);
  sub=subr=0;
  while (cl!=NULL)
    {
      tc=(ttvcritic_list *)cl->DATA;
      if (cl!=ch)
        {
          if (tc->NODE_FLAG & TTV_NODE_FLAG_ISLATCH_ACCESS)
          {
            if (!latchideal) break;
            mode=1;
          }
          else if ((mode==0 && clockideal) || (mode==1 && latchideal))
          {
            sub+=tc->DELAY; tc->DELAY=0;
            subr+=tc->REFDELAY; tc->REFDELAY=0;
            if (tc->SIMDELAY!=TTV_NOTIME) tc->SIMDELAY=0;
          }
        }
      cl=delchain(cl,cl);
    }
  freechain(cl);
  tp->DELAY-=sub;
  tp->REFDELAY-=subr;
  //tp->USER=addptype(tp->USER, STB_IDEAL_CLOCK, NULL);
}


static char *typeconv(char *type)
{
  if (strstr(type,"connector")!=NULL) return "S";
  if (strstr(type,"latch")!=NULL) return "L";
  if (strstr(type,"precharge")!=NULL) return "N";
  if (strstr(type,"flip-flop")!=NULL) return "F";
  if (strstr(type,"breakpoint")!=NULL) return "B";
  if (strstr(type,"command")!=NULL) return "K";
  return "";
}

static int slacksort(const void *a0, const void *b0)
{
  int res;
  slackinfo *a=(slackinfo *)a0, *b=(slackinfo *)b0;
  if (a->slack<b->slack) return -1;
  if (a->slack>b->slack) return 1;
  if ((res=mbk_casestrcmp(a->debugl.SIG2->NAME, b->debugl.SIG2->NAME))<0) return -1;
  else if (res>0) return 1;
  else if ((res=mbk_casestrcmp(a->debugl.SIG1->NAME, b->debugl.SIG1->NAME))<0) return -1;
  else if (res>0) return 1;
  return 0;
}

static ttvevent_list *stb_getpathstart(ttvfig_list *tvf, ttvevent_list *inev, char *start, int dirin, int *access, unsigned char phase, ttvevent_list *startcmd, int notanaccess, ttvevent_list *startclock)
{
  ttvevent_list *input=NULL, *testev;
  stbck *ck;
  stbnode *node;
  int checkdir;
  ptype_list *pt, *pt0;

  if (notanaccess || (inev->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_R|TTV_SIG_B))==0 || (startcmd==NULL && ttv_IsClock(inev->ROOT)))
    {
      *access=0;
      if ((inev->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
      else checkdir=0;
      if ((dirin==2 || checkdir==dirin) && mbk_TestREGEX(ttv_GetFullSignalName(tvf, inev->ROOT), start))
        input=inev;
    }
  else if ((inev->ROOT->TYPE & TTV_SIG_B)!=0)
    {
       pt=ttv_getlatchaccess(tvf, inev, TTV_FIND_MAX);
       for (pt0=pt; pt0!=NULL; pt0=pt0->NEXT)
       {
         node=stb_getstbnode((ttvevent_list *)pt0->DATA);
         testev=NULL;
         ck=node->CK;
         if (ck!=NULL)
         {
            if (ck->ORIGINAL_CLOCK!=NULL) testev=ck->ORIGINAL_CLOCK;
            else if (stb_getclocknode(stb_getstbfig(tvf), ck->CKINDEX, NULL, &testev, NULL)==NULL)
            testev=NULL;
         }
         if (testev!=NULL)
         {
           if ((testev->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
           else checkdir=0;
           if ((dirin==2 || checkdir==dirin) && mbk_TestREGEX(ttv_GetFullSignalName(tvf, testev->ROOT), start))
             break;
         }
       }
       freeptype(pt);
       if (pt0!=NULL && testev!=NULL) input=testev, *access=1;
       else input=inev, *access=0;
    }
  else
    {
      // recherche de la clock
      *access=1;
      if (startclock!=NULL && (inev->ROOT->TYPE & TTV_SIG_R)==0)
      {
        if ((startclock->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
        else checkdir=0;
        if ((dirin==2 || checkdir==dirin) && mbk_TestREGEX(ttv_GetFullSignalName(tvf, startclock->ROOT), start))
          input=startclock;
        else
          input=NULL;
      }
      else
      {
        node=stb_getstbnode(inev);
        for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
          {
            if (phase!=STB_NO_INDEX && ck->CKINDEX!=phase) continue;
            if (ck->ORIGINAL_CLOCK!=NULL)
              testev=ck->ORIGINAL_CLOCK;
            else if (stb_getclocknode(stb_getstbfig(tvf), ck->CKINDEX, NULL, &testev, NULL)==NULL)
              testev=NULL;

            if (testev!=NULL)
              {
                if ((testev->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
                else checkdir=0;
                if ((dirin==2 || checkdir==dirin) && mbk_TestREGEX(ttv_GetFullSignalName(tvf, testev->ROOT), start))
                  {
                    if ((ck->FLAGS & (STBCK_MAX_EDGE_HZ|STBCK_MIN_EDGE_HZ))!=0)
                      *access=0, testev=inev;
                    else if ((ck->FLAGS & (STBCK_FAKE_DOWN|STBCK_FAKE_UP))!=0)
                      {
                        ttvevent_list *tve;
                        if (ck->CMD!=NULL) tve=ck->CMD;
                        else 
                          {
                            if ((pt=getptype(node->EVENT->USER, STB_BREAK_TEST_EVENT))!=NULL)
                              tve=(ttvevent_list *)pt->DATA;
                            else
                              tve=node->EVENT;
                          }
                        if ((inev->ROOT->TYPE & TTV_SIG_L)!=0 && (tve->TYPE & TTV_NODE_UP)!=(ck->ORIGINAL_CLOCK->TYPE & TTV_NODE_UP)) 
                          checkdir=(checkdir+1) & 1;
                        if ((checkdir==0 && (ck->FLAGS & STBCK_FAKE_DOWN)!=0)
                            || (checkdir==1 && (ck->FLAGS & STBCK_FAKE_UP)!=0))
                            *access=0, testev=inev;
                      }
                    break;
                  }
              }
          }
        if (ck!=NULL)
          input=testev;
      }
    }
  return input;
}


static inline int stb_could_enter_range(ttvsig_list *tvs, int setuphold, double top, int nb, int max)
{
  int i;
  stbnode *ptnode;
  double val;

  if (!V_BOOL_TAB[__STB_SETUP_HOLD_UPDATE].VALUE) return 1;
  if (nb<max) return 1;

  for(i = 0  ; i < 2  ; i++)
   {
    if((ptnode = stb_getstbnode(tvs->NODE+i)) == NULL)
      continue  ;
    val=-1;
    if (setuphold && ptnode->SETUP!=STB_NO_TIME)
      {
        val=ptnode->SETUP*1e-12/TTV_UNIT;
      }
    else if (!setuphold && ptnode->HOLD!=STB_NO_TIME)
      {
        val=ptnode->HOLD*1e-12/TTV_UNIT;
      }

    if (val!=-1 && val<top) return 1;
   }
  return 0;
}

static int checkkeepslack(int flag, int forced)
{
  if ((flag & STB_DEBUG_HZPATH)==0 && forced==TTV_API_ONLY_HZ) return 0;
  if ((flag & STB_DEBUG_HZPATH)!=0 && forced==TTV_API_ONLY_NOTHZ) return 0;
  return 1;
}

static chain_list *getslackinfo_grab(ttvfig_list *tvf, ttvsig_list *sig, int setup, char *start, char *end, int dirin, int dirout, double margin, chain_list *old, double *top, int *nb, int max, int forced, char *thru)
{
  stbdebug_list *dbl, *savedbl;
  double val, ckt;
  char ckdir;
  Setup_Hold_Computation_Detail_INFO *detail;
  long d_req, d_valid;
  ttvsig_list *inputsig;
  ttvevent_list *path_start_sig;
  slackinfo *si;
  int access, i;
  chain_list *lst, *ch;

  if (!stb_could_enter_range(sig, setup, *top, *nb, max)) return old;

  if (mbk_TestREGEX(ttv_GetFullSignalName(tvf, sig), end) || mbk_TestREGEX(ttv_GetFullSignalNetName(tvf, sig), end))
    {
      lst=stb_internal_GetSignalStabilityPaths(sig, setup?STB_COMPUTE_SETUP_ONLY:STB_COMPUTE_HOLD_ONLY);
      for (ch=lst; ch!=NULL; ch=ch->NEXT)
        {
          dbl=(stbdebug_list *)ch->DATA;
          for (i=0;i<2;i++)
            {
              if (dirout==2 || dirout==i)
                {
                  if (setup) detail=&dbl->detail[i].setup;
                  else detail=&dbl->detail[i].hold;
              
                  val=detail->VALUE*1e-12/TTV_UNIT;
                  if (detail->VALUE!=STB_NO_TIME && val<=margin && (*nb<max || val<*top) && checkkeepslack(detail->flags, forced))
                    {
                      inputsig=dbl->SIG1;
                      if ((path_start_sig=stb_getpathstart(tvf, dbl->SIG1_EVENT, start, dirin, &access, detail->phase_origin, dbl->START_CMD_EVENT, 0, dbl->START_CLOCK))!=NULL)
                        {
                          if (thru==NULL
                              || (!access && (mbk_TestREGEX(ttv_GetFullSignalName(tvf, path_start_sig->ROOT), thru) || mbk_TestREGEX(ttv_GetFullSignalNetName(tvf, path_start_sig->ROOT), thru)))
                              || (access && (mbk_TestREGEX(ttv_GetFullSignalName(tvf, dbl->SIG1), thru) || mbk_TestREGEX(ttv_GetFullSignalNetName(tvf, dbl->SIG1), thru)))
                             )
                            {
                              d_valid=detail->instab+detail->data_margin;
                              if (setup)
                                d_req=detail->clock+detail->clock_margin-detail->margin+detail->skew-detail->uncertainty;
                              else
                                {
                                  d_req=detail->clock+detail->clock_margin+detail->margin-detail->period-detail->skew+detail->uncertainty;
                                  d_valid+=detail->misc;
                                }

                              // correction pour multicycle sur le required
                              /*d_valid+=detail->mc_setup_period-detail->mc_hold_period;
                                d_req+=detail->mc_setup_period-detail->mc_hold_period;*/
                              d_valid+=-detail->datamoved-detail->mc_hold_period;
                              d_req+=-detail->datamoved-detail->mc_hold_period;

                              if (!setup)
                                {
                                  if (dbl->CKREFUP_MAX<dbl->CKREFDN_MAX)
                                    ckt=(dbl->CKREFUP_MAX/TTV_UNIT)*1e-12, ckdir='u';
                                  else
                                    ckt=(dbl->CKREFDN_MAX/TTV_UNIT)*1e-12, ckdir='d';
                                }
                              else
                                {
                                  if (dbl->CKREFUP_MIN<dbl->CKREFDN_MIN)
                                    ckt=(dbl->CKREFUP_MIN/TTV_UNIT)*1e-12, ckdir='u';
                                  else
                                    ckt=(dbl->CKREFDN_MIN/TTV_UNIT)*1e-12, ckdir='d';
                                }
                          
                              si=(slackinfo *)mbkalloc(sizeof(slackinfo));

                              si->slack=detail->VALUE*1e-12/TTV_UNIT;

                              si->datavalid=(d_valid/TTV_UNIT)*1e-12;
/*                              if ((detail->flags & STB_DEBUG_DIRECTIVE)==0)
                                {
                                  if (dbl->CKREFUP_MAX==STB_NO_TIME)
                                    si->datavalidclock=-1;
                                  else
                                    si->datavalidclock=ckt;
                                  si->datavalidclockdir=ckdir;
                                }
                              else
                                {
                                  si->datavalidclock=0; // a faire
                                  si->datavalidclockdir='u';
                                }
                          */
                              if (dbl->PERIODE==STB_NO_TIME)
                                si->period=-1;
                              else
                                si->period=(dbl->PERIODE/TTV_UNIT)*1e-12;
                              si->datareq=(d_req/TTV_UNIT)*1e-12;
                              si->path=NULL;
                              si->dirout=i;
                              si->lag=(detail->lag/TTV_UNIT)*1e-12;
/*                              if (detail->flags & STB_DEBUG_DIRECTIVE_FALLING) si->dirin=0;
                              else if (detail->flags & STB_DEBUG_DIRECTIVE_RISING) si->dirin=1;
                              else */si->dirin=(path_start_sig->TYPE & TTV_NODE_UP)==TTV_NODE_UP?1:0;
                              si->access=access;
                              si->hzpath=(detail->flags & STB_DEBUG_HZPATH)!=0?1:0;
                              si->phase=detail->phase_origin;
                              si->flags=detail->flags;
                              si->jump=1;

                              memcpy(savedbl=&si->debugl, dbl, sizeof(stbdebug_list));

                              /*                          if (savedbl->CHRONO!=NULL)
                                                          {
                                                          savedbl->CHRONO=mbkalloc(sizeof(stbchrono_list));
                                                          memcpy(savedbl->CHRONO, dbl->CHRONO, sizeof(stbchrono_list));
                                                          savedbl->CHRONO->SPECS=stb_dupstbpairlist(dbl->CHRONO->SPECS);
                                                          savedbl->CHRONO->SPECS_U=stb_dupstbpairlist(dbl->CHRONO->SPECS_U);
                                                          savedbl->CHRONO->SPECS_D=stb_dupstbpairlist(dbl->CHRONO->SPECS_D);
                                                          }*/

                              old=addchain(old, si);
                              (*nb)++;
                              if (val>*top) *top=val;
                            }
                        }
                    }
                }
            }
        }
      freechain(lst);
    }
  return old;
}

static chain_list *stb_get_directive_paths(ttvfig_list *tf, int max, ttvsig_list *sig, int sigdir, int clock, int retrypath, long thisarrival, ttvevent_list **tve, chain_list *speedinput)
{
  chain_list *cl, *ch, *clks, *ret=NULL;
  int checkdir;
  ttvpath_list *tp;
  long arrival;

  *tve=NULL;
  ttv_activate_multi_cmd_critic_mode(1);
  ttv_activate_multi_end_cmd_critic_mode(1);
  ch=addchain(NULL, sig);
  if (clock) 
    {
      clks=ttv_getclocksiglist(tf);
      ttv_search_mode(1, TTV_MORE_OPTIONS_USE_CLOCK_START);
    }
  else
    {
      ttv_search_mode(1, TTV_MORE_OPTIONS_USE_DATA_START);
      clks=speedinput;
    }
  cl=ttv_internal_GetPaths_EXPLICIT(tf, NULL, clks, ch, "??", 0, "critic", clock?"path":retrypath?"path":"access", max?"max":"min");
  if (clks!=speedinput) freechain(clks);
  freechain(ch);
  ttv_search_mode(0, TTV_MORE_OPTIONS_USE_CLOCK_START|TTV_MORE_OPTIONS_USE_DATA_START);
  ttv_activate_multi_end_cmd_critic_mode(0);
  ttv_activate_multi_cmd_critic_mode(0);
  
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      tp=(ttvpath_list *)ch->DATA;
      if ((tp->ROOT->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
      else checkdir=0;
      if (retrypath &&
          ((tp->NODE->ROOT->TYPE & TTV_SIG_C)==0
           || ttv_IsClock(tp->NODE->ROOT))                      
          ) continue;
      if (ttv_PathIsHZ(tp)) continue;
      
      if (checkdir==sigdir)
        {
          arrival=tp->DELAY+tp->DATADELAY;
          if (tp->CROSSMINDELAY!=TTV_NOTIME)
              arrival+=tp->CROSSMINDELAY;
            else
              arrival+=tp->DELAYSTART;
          if ((clock && getptype(tp->NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL) || arrival==thisarrival)
            {
              ret=addchain(ret, tp);
              ch->DATA=NULL;
              break;
            }
        }
    }
  
  if (!retrypath && !clock && ret==NULL)
    ret=stb_get_directive_paths(tf, max, sig, sigdir, clock, 1, thisarrival, tve, speedinput);
    
  if (!retrypath && ret==NULL && (sig->TYPE & TTV_SIG_C)!=0)
    {
      *tve=&sig->NODE[sigdir];
    }

  ttv_FreePathList(cl);
  freechain(cl);
  return ret;
}


static ttvpath_list *change_path_start_considering_start_phase(ttvfig_list *tf, ttvpath_list *tp, char phase, chain_list **list)
{
  long start;
  start=ttv_getinittime(tf,tp->NODE,STB_NO_INDEX,tp->TYPE,phase,NULL) ;
  if (start!=tp->DELAYSTART)
    {
      tp=ttv_duppath(tp);
      tp->DELAYSTART=start;
      *list=addchain(*list, tp);
    }
  return tp;
}

static int stb_assoc_start_mode(slackinfo *a)
{
  if (a->access) return 2; // clock mode
  if (ttv_IsClock(a->debugl.SIG1))
    {
      if (a->debugl.START_CMD_EVENT==NULL) return 2; // clock mode
      else return 1; //data mode
    }
  return 0;
}
static chain_list *stb_associate_paths(ttvfig_list *tf, char *dir, int setuphold, char *start, int dirin, slackinfo *tab, int cur, int tot, chain_list *allpaths, chain_list *speedinput)
{
  chain_list *cl, *ch, *clks;
  int i, access, checkdir, specinmode=0, sm;
  char checkhz;
  ttvpath_list *tp;
  ttvevent_list *path_start_ev;
  stbnode *node;
  if (tab[cur].path==NULL)
    {
      ttv_activate_multi_cmd_critic_mode(1);
      ttv_activate_multi_end_cmd_critic_mode(1);
      ttv_search_mode(1, TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE);
      ch=addchain(NULL, tab[cur].debugl.SIG2);
      if (tab[cur].access)
        {
          clks=ttv_getclocksiglist(tf);
          ttv_search_mode(1, TTV_MORE_OPTIONS_USE_CLOCK_START);
          if (tab[cur].debugl.SIG2==tab[cur].debugl.SIG1 && (tab[cur].debugl.SIG2->TYPE & TTV_SIG_C)!=0)
            ttv_search_mode(1, TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS);
        }
      else clks=NULL;
      sm=stb_assoc_start_mode(&tab[cur]);
      if (sm==1) ttv_search_mode(1, TTV_MORE_OPTIONS_USE_DATA_START);
      else if (sm==2) ttv_search_mode(1, TTV_MORE_OPTIONS_USE_CLOCK_START);

      cl=ttv_internal_GetPaths_EXPLICIT(tf, clks, speedinput, ch, dir, 0, "critic", tab[cur].access?"access":"path", setuphold?"max":"min");
      freechain(clks);
      freechain(ch);
      ttv_search_mode(0, TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE);
      ttv_search_mode(0, TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS);

      if (cl==NULL)
        {
          node=stb_getstbnode(tab[cur].debugl.SIG1_EVENT);
          if (node->SPECIN!=NULL)
            {
              cl=addchain(NULL, ttv_create_one_node_path(tf, tf, tab[cur].debugl.SIG1_EVENT, setuphold?TTV_FIND_MAX:TTV_FIND_MIN));
              specinmode=1;
            }
        }
      ttv_search_mode(0, TTV_MORE_OPTIONS_USE_CLOCK_START|TTV_MORE_OPTIONS_USE_DATA_START);
      ttv_activate_multi_end_cmd_critic_mode(0);
      ttv_activate_multi_cmd_critic_mode(0);      
/*
      sort_by_absolute_time(cl);
      if (setuphold==0) cl=reverse(cl);
*/
      for (i=cur; i<tot; i+=tab[i].jump)
        {
          if (tab[i].path!=NULL) continue;
//          if (tab[i].flags & STB_DEBUG_DIRECTIVE) continue;
          if (tab[i].access==tab[cur].access && tab[i].debugl.SIG2==tab[cur].debugl.SIG2 && stb_assoc_start_mode(&tab[i])==sm)
            {
              path_start_ev=stb_getpathstart(tf, tab[i].debugl.SIG1_EVENT, start, dirin, &access, tab[i].phase, tab[i].debugl.START_CMD_EVENT, !tab[i].access, tab[i].debugl.START_CLOCK);
              for (ch=cl; ch!=NULL; ch=ch->NEXT)
                {
                  tp=(ttvpath_list *)ch->DATA;
                  if ((tp->ROOT->TYPE & TTV_NODE_UP)==TTV_NODE_UP) checkdir=1;
                  else checkdir=0;
                  if (ttv_PathIsHZ(tp)) checkhz=1; else checkhz=0;

                  if ((specinmode || checkdir==tab[i].dirout) && checkhz==tab[i].hzpath)
                    {
                      if (tab[cur].access==0)
                        {
                          if (tp->NODE==tab[i].debugl.SIG1_EVENT) 
                            {
                              tab[i].path=change_path_start_considering_start_phase(tf, tp, tab[i].phase, &cl);
                              tab[i].path->MD++;
                              break;
                            }
                        }
                      else
                        {
                          if (tp->LATCH==tab[i].debugl.SIG1_EVENT && tp->NODE==path_start_ev
                              && ((tab[i].flags & STB_DEBUG_DIRECTIVE)!=0 || tp->CMD==tab[i].debugl.CMD_EVENT)
                              && ((tp->LATCH->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_B))!=0 || tp->CMDLATCH==tab[i].debugl.START_CMD_EVENT)
                              ) 
                            {
                              tab[i].path=change_path_start_considering_start_phase(tf, tp, tab[i].phase, &cl);
                              tab[i].path->MD++;
                              break;
                            }
                        }
                    }
                }
            }
        }
      for (ch=cl; ch!=NULL; ch=ch->NEXT)
      {
        tp=(ttvpath_list *)ch->DATA;
        if (tp->MD!=NULL) allpaths=addchain(allpaths, tp), ch->DATA=NULL;
      }
      ttv_FreePathList(cl);
      freechain(cl);
    }
  return allpaths;
}

#define ZINAPS_TEMP_TYPE  0xfab70228

static slackinfo *__stb_getslacklist(stbfig_list *sbf, int setuphold, char *start, char *end, int dirin, int dirout, double margin, int maxnb, int *total, int forced, char *thru)
{
  int tot, max, nbtot=0;
  slackinfo *tab;
  chain_list *cl, *chainsig, *chain, *all, *prev;
  double topval=1e10;
  ptype_list *pt;
  ttvsig_list *tvs;

  if (sbf==NULL) { *total=0; return NULL; }

  all=NULL;
  chainsig = ttv_getsigbytype(sbf->FIG, NULL, TTV_SIG_TYPEALL, NULL) ;
  for(chain = chainsig; chain ; chain = chain->NEXT)
  {
    tvs=(ttvsig_list *)chain->DATA;
    all=getslackinfo_grab(sbf->FIG, tvs, setuphold, start, end, dirin, dirout, margin, all, &topval, &nbtot, maxnb, forced, thru);
    if (maxnb!=INT_MAX && nbtot>maxnb*2)
      {
        tab=(slackinfo *)mbkalloc(sizeof(slackinfo)*nbtot);
        for (tot=0, cl=all; tot<nbtot && cl!=NULL; tot++, cl=cl->NEXT)
          {
            memcpy(&tab[tot], cl->DATA, sizeof(slackinfo));
          }

        qsort(tab, tot, sizeof(slackinfo), slacksort);

        for (tot=0, cl=all; tot<maxnb; tot++, cl=cl->NEXT)
          {
            prev=cl;
            memcpy(cl->DATA, &tab[tot], sizeof(slackinfo));
          }
        prev->NEXT=NULL;
        while (cl!=NULL)
          {
            mbkfree(cl->DATA);
            cl=delchain(cl, cl);
          }
        topval=tab[maxnb-1].slack;
        nbtot=maxnb;
        mbkfree(tab);
      }
  }
  freechain(chainsig);

  max=countchain(all);
  tab=(slackinfo *)mbkalloc(sizeof(slackinfo)*max);
  for (tot=0; all!=NULL; all=delchain(all, all))
    {
      memcpy(&tab[tot], all->DATA, sizeof(slackinfo));
      mbkfree(all->DATA);
      tot++;
    }

  qsort(tab, tot, sizeof(slackinfo), slacksort);

  *total=tot;

  for (max=0; max<tot; max++)
    {
      if ((pt=getptype(tab[max].debugl.SIG2->USER, ZINAPS_TEMP_TYPE))==NULL)
        tab[max].debugl.SIG2->USER=addptype(tab[max].debugl.SIG2->USER, ZINAPS_TEMP_TYPE, (void *)(long)max);
      else
        {
          tab[(int)(long)pt->DATA].jump=max-(int)(long)pt->DATA;
          pt->DATA=(void *)(long)max;
        }
    }

  for (max=0; max<tot; max++)
    {
      if ((pt=getptype(tab[max].debugl.SIG2->USER, ZINAPS_TEMP_TYPE))!=NULL)
        {
          tab[(int)(long)pt->DATA].jump=tot-(int)(long)pt->DATA;
          tab[max].debugl.SIG2->USER=delptype(tab[max].debugl.SIG2->USER, ZINAPS_TEMP_TYPE);
        }
    }

  return tab;
}

static chain_list *__stb_getslackpaths(ttvfig_list *tvf, slackinfo *tab, char *start, int dirin, char *direction, int setuphold, int i, int tot, chain_list *allpaths, chain_list **requiredpath, chain_list *speedinput)
{
  Setup_Hold_Computation_Detail_INFO *detail;
  chain_list *chain;
  int dir;
  ttvevent_list *clockcon, *pathcmd;
  *requiredpath=NULL;

/*  if (tab[i].flags & STB_DEBUG_DIRECTIVE)
    allpaths=stb_associate_directive_paths(tvf, setuphold, tab, i, tot, allpaths, (tab[i].flags & STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK)?1:0, 0, speedinput);
  else*/
  allpaths=stb_associate_paths(tvf, direction, setuphold, start, dirin, tab, i, tot, allpaths, speedinput);
  if (tab[i].path)
    {
      if (ttv_GetPathEndDirection(tab[i].path)=='u') dir=1; else dir=0;
      if (setuphold) detail=&tab[i].debugl.detail[dir].setup;
      else detail=&tab[i].debugl.detail[dir].hold;

      tab[i].path->CMD=tab[i].debugl.CMD_EVENT;
      
      if (!(tab[i].flags & STB_DEBUG_SPECOUTMODE) && !(tab[i].flags & STB_DEBUG_DIRECTIVE_DELAYMODE))
      {
        if (tab[i].flags & STB_DEBUG_DIRECTIVE)
          {
            chain=stb_get_directive_paths(tvf, !setuphold, tab[i].debugl.CMD_EVENT->ROOT, (tab[i].flags & STB_DEBUG_DIRECTIVE_RISING)?1:0,(tab[i].flags & STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA)?0:1, 0, detail->clock-detail->moved_clock_period, &clockcon, speedinput);
          }
        else
          chain=ttv_GetClockPath(tab[i].path, &clockcon, &pathcmd);
        if (chain==NULL && clockcon!=NULL)
          {
            if ((tab[i].flags & STB_DEBUG_DIRECTIVE)==0 || (tab[i].flags & STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA)==0)
              ttv_search_mode(1, TTV_MORE_OPTIONS_USE_CLOCK_START);
            else
              ttv_search_mode(1, TTV_MORE_OPTIONS_USE_DATA_START);

            chain=addchain(NULL, ttv_create_one_node_path(tab[i].path->FIG, tab[i].path->FIG, clockcon, (tab[i].path->TYPE & TTV_FIND_MAX)!=0?TTV_FIND_MIN:TTV_FIND_MAX));
            ttv_search_mode(0, TTV_MORE_OPTIONS_USE_CLOCK_START|TTV_MORE_OPTIONS_USE_DATA_START);
          }
      }
      else chain=NULL;
      *requiredpath=chain;
    }

  return allpaths;
}

static ttvpath_list *stb_GetSlackPath(slackobject *so, char which)
{
  // which=0 data_valid else data_required
  chain_list *allpaths, *chain;
  Setup_Hold_Computation_Detail_INFO *detail;

  if (so->SI.path==NULL)
    {
      int oldttvmode=TTV_QUIET_MODE, oldprecisionlevel;
      TTV_QUIET_MODE=1;
      oldprecisionlevel=ttv_SetPrecisionLevel(0);

      allpaths=__stb_getslackpaths(ttv_GetSignalTopTimingFigure(so->SI.debugl.SIG2), &so->SI, "*", 2, "??", so->setuphold, 0, 1, NULL, &chain, NULL);
      so->data_required=chain!=NULL?chain->DATA:NULL;
      freechain(chain);
      so->data_valid=so->SI.path;
      for (chain=allpaths; chain!=NULL && chain->DATA!=so->SI.path; chain=chain->NEXT) ;
      if (chain!=NULL) chain->DATA=NULL;
      ttv_FreePathList(allpaths);
      freechain(allpaths);

      ttv_SetPrecisionLevel(oldprecisionlevel);
      TTV_QUIET_MODE=oldttvmode;

      if (so->setuphold) detail=&so->SI.debugl.detail[(int)so->SI.dirout].setup;
      else detail=&so->SI.debugl.detail[(int)so->SI.dirout].hold;

//      if (so->data_valid!=NULL) so->data_valid->DELAYSTART+=detail->datamoved+detail->misc;

      if (so->data_required!=NULL)
        {
/*          long pm;
          pm=detail->moved_clock_period;
          if (so->setuphold==0) pm-=detail->period;
          so->data_required->DELAYSTART+=pm;*/
          so->data_required->DELAYSTART+=-detail->datamoved-detail->misc+detail->moved_clock_period;
          if (so->setuphold==0) so->data_required->DELAYSTART-=detail->period;
        }
      if (so->data_valid) ttv_CheckTransformToIdeal(so->data_valid);
      if (so->data_required) ttv_CheckTransformToIdeal(so->data_required);
    }
  if (which=='v') return so->data_valid;
  return so->data_required;
}

static void stb_GetAllSlackPath(chain_list *sl)
{
  chain_list *cl, *chain, *allpaths, *speedinput;
  slackobject *so;
  slackinfo *tab;
  int nb, setuphold, i;
  ttvpath_list *pth;
  ttvfig_list *tvf;
  for (nb=0, cl=sl; cl!=NULL; cl=cl->NEXT)
  {
    so=(slackobject *)cl->DATA;
    setuphold=so->setuphold;
    tvf=so->SI.debugl.SIG2->ROOT;
    if (so->SI.path==NULL) nb++;
  }

  if (nb>0)
  {
    tab=(slackinfo *)mbkalloc(nb*sizeof(slackinfo));
    for (nb=0, cl=sl; cl!=NULL; cl=cl->NEXT)
    {
      so=(slackobject *)cl->DATA;
      if (so->SI.path==NULL)
        memcpy(&tab[nb++], &so->SI, sizeof(slackinfo));
    }
    tvf=ttv_GetTopTimingFigure(tvf);
    speedinput = ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B,NULL) ;
    allpaths=NULL;
    for (i=0, cl=sl; cl!=NULL; cl=cl->NEXT)
    {
      so=(slackobject *)cl->DATA;
      if (so->SI.path==NULL)
      {
        allpaths=__stb_getslackpaths(tvf, tab, "*", 2, "??", setuphold, i, nb, allpaths, &chain, speedinput);
        so->data_required=chain!=NULL?ttv_duppath((ttvpath_list *)chain->DATA):NULL;
        freechain(chain);
        so->data_valid=ttv_duppath(tab[i].path);
        i++;
      }
    }
    freechain(speedinput);
    for (chain=allpaths; chain!=NULL; chain=chain->NEXT)
      {
        pth=(ttvpath_list *)chain->DATA;
        if (pth!=NULL) pth->MD=NULL;
      }
    ttv_FreePathList(allpaths);
    freechain(allpaths);
    mbkfree(tab);
  }
}

static void __stb_DisplaySlackReport_grab(FILE *f, stbfig_list *stbfig, char *start, char *end, char *direction, int setuphold, double margin, int sumaonly, int maxnb, int addit, char *thru, chain_list *given_objects, int simple)
{
  char *startsig, *endsig;
  ttvsig_list *sg;
  char startdir, enddir;
  char buf[1024];
  int count=1, tot, max, i, dir, dirin, dirout, access, countfree;
  Board *b;
  slackinfo *tab;
  slackobject **sotab=NULL;
  long lastmode;
  Setup_Hold_Computation_Detail_INFO *detail;
  chain_list *chain, *allpaths, *speedinput=NULL;
  ttvevent_list *path_start_ev;
  ttvpath_list *pth;
  long old0, old1, save_req;
  float pm, sh_m;
  int forced;
  

  ttv_DirectionStringToIndices(direction, &dirin, &dirout);
  forced=ttv_DirectionStringToIndicesHZ(direction);

  lastmode=stbsetdebugflag(STB_DIFFERENTIATE_INPUT_EVENTS);
  ttv_setsearchexclude(0,0,&old0, &old1);

  b=Board_CreateBoard();

  Board_SetSize(b, COL_INDEX, 5, 'r');
  Board_SetSize(b, COL_SLACK, 7, DISPLAY_number_justify);
  Board_SetSize(b, COL_DV_TIME, 7, DISPLAY_number_justify);
  Board_SetSize(b, COL_DV_REL, 7, DISPLAY_number_justify);
  Board_SetSize(b, COL_DR_TIME, 7, DISPLAY_number_justify);
  Board_SetSize(b, COL_DR_REL, 7, DISPLAY_number_justify);
  Board_SetSize(b, COL_STARTEND, 3, 'c');
  Board_SetSize(b, COL_FROMNODE, 20, 'l');
  Board_SetSize(b, COL_FROMNODEDIR, 3, 'l');
  Board_SetSize(b, COL_TONODE, 20, 'l');
  Board_SetSize(b, COL_TONODEDIR, 4, 'l');

  if (addit & 2)
    Board_SetSize(b, COL_THRUDEBUG, 20, 'l');

  if (addit & 1)
    {
      Board_SetSize(b, COL_DATAPATHMARGIN, 6, DISPLAY_number_justify);
      Board_SetSize(b, COL_CLOCKPATHMARGIN, 6, DISPLAY_number_justify);
      Board_SetSize(b, COL_INTRINSIC_MARGIN, 10, DISPLAY_number_justify);
      Board_SetSep(b, COL_SLACK_SEP6);
    }

  Board_SetSep(b, COL_SLACK_SEP0);
  Board_SetSep(b, COL_SLACK_SEP1);
  Board_SetSep(b, COL_SLACK_SEP2);
  Board_SetSep(b, COL_SLACK_SEP3);
  Board_SetSep(b, COL_SLACK_SEP4);
  Board_SetSep(b, COL_SLACK_SEP5);

  Board_NewLine(b);

  Board_SetValue(b, COL_INDEX, "Path");
  Board_SetValue(b, COL_SLACK, "Slack");
  Board_SetValue(b, COL_DV_TIME, "éData");
  Board_SetValue(b, COL_DV_REL, "èvalid");
  Board_SetValue(b, COL_DR_TIME, "éData");
  Board_SetValue(b, COL_DR_REL, "èrequired");
  Board_SetValue(b, COL_FROMNODE, "From_node");
  Board_SetValue(b, COL_TONODE, "To_node");

  if (addit & 1)
    {
      Board_SetValue(b, COL_DATAPATHMARGIN, "D-Margin");
      Board_SetValue(b, COL_CLOCKPATHMARGIN, "C-Margin");
      Board_SetValue(b, COL_INTRINSIC_MARGIN, "IntrMargin");
    }
  if (addit & 2)
    Board_SetValue(b, COL_THRUDEBUG, "Thru_node");

  Board_NewSeparation(b);

  if (!given_objects)
    tab=__stb_getslacklist(stbfig, setuphold, start, end, dirin, dirout, margin, maxnb, &tot, forced,thru);
  else
    {
      tot=countchain(given_objects);
      if (tot>0)
        {
          tab=mbkalloc(sizeof(slackinfo)*tot);
          sotab=mbkalloc(sizeof(slackobject *)*tot);
          for (chain=given_objects, i=0; chain!=NULL; chain=chain->NEXT, i++)
            { 
              sotab[i]=(slackobject *)chain->DATA;
              memcpy(&tab[i], &((slackobject *)chain->DATA)->SI, sizeof(slackinfo));
            }
        }
    }

  for (i=0; i<tot && i<maxnb; i++)
    {
      Board_NewLine(b);
      sprintf(buf,"%d", count);
      Board_SetValue(b, COL_INDEX, buf);
      if (sotab)
        {
          if (sotab[i]->SI.path!=NULL)
            path_start_ev=sotab[i]->SI.path->NODE;
          else
            path_start_ev=stb_getpathstart(ttv_GetSignalTopTimingFigure(sotab[i]->SI.debugl.SIG2), sotab[i]->SI.debugl.SIG1_EVENT, "*", (int)sotab[i]->SI.dirin, &access, sotab[i]->SI.phase, sotab[i]->SI.debugl.START_CMD_EVENT, !sotab[i]->SI.access, sotab[i]->SI.debugl.START_CLOCK);
        }
      else
        path_start_ev=stb_getpathstart(stbfig->FIG, tab[i].debugl.SIG1_EVENT, start, dirin, &access, tab[i].phase,tab[i].debugl.START_CMD_EVENT, !tab[i].access, tab[i].debugl.START_CLOCK);

      sg=path_start_ev->ROOT;
      startsig=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(sg), sg);
      
      sg=tab[i].debugl.SIG2;
      endsig=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(sg), sg);

      startdir=dirconv(tab[i].dirin?'u':'d');
      enddir=dirconv(tab[i].dirout?'u':'d');
      sprintf(buf,"(%c)",startdir);
      Board_SetValue(b, COL_FROMNODEDIR, buf);
      buf[0]='(';
      buf[1]=enddir;
      if (tab[i].hzpath) {strcpy(&buf[2],"Z)");}
      else {strcpy(&buf[2],")"); }
      Board_SetValue(b, COL_TONODEDIR, buf);
      Board_SetValue(b, COL_FROMNODE, startsig);
      Board_SetValue(b, COL_TONODE, endsig);

      if (/*(tab[i].flags & STB_DEBUG_DIRECTIVE)==0 && */(tab[i].access || (tab[i].debugl.SIG1_EVENT->ROOT->TYPE & TTV_SIG_C)==0))
        {
          char buf1[1024];
          
          if (tab[i].debugl.START_CMD_EVENT!=NULL)
            {
              startsig=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(tab[i].debugl.START_CMD_EVENT->ROOT), tab[i].debugl.START_CMD_EVENT->ROOT);
              sprintf(buf1, " cmd: %s (%c)", startsig, dirconv(ttv_GetTimingEventDirection(tab[i].debugl.START_CMD_EVENT)));
            }
          else strcpy(buf1,"");
          startsig=ttv_GetFullSignalName_COND(ttv_GetSignalTopTimingFigure(tab[i].debugl.SIG1_EVENT->ROOT), tab[i].debugl.SIG1_EVENT->ROOT);
          sprintf(buf, "%s (%c)%s", startsig, dirconv(ttv_GetTimingEventDirection(tab[i].debugl.SIG1_EVENT)), buf1);
          Board_SetValue(b, COL_THRUDEBUG, buf);
        }

      if (tab[i].datavalid<1)
        {
          Board_SetValue(b, COL_DV_TIME, FormaT(tab[i].datavalid*DISPLAY_time_unit, buf, DISPLAY_time_format));
          if ((tab[i].flags & STB_DEBUG_DIRECTIVE)==0)
            {
              char buf0[1024];
              if (tab[i].lag>0)
                sprintf(buf, "[%s lag]", FormaT(tab[i].lag*DISPLAY_time_unit, buf0, DISPLAY_time_format));
              else
                strcpy(buf,"");
              Board_SetValue(b, COL_DV_REL, buf);
            }
        }
      else
        {
          Board_SetValue(b, COL_DV_TIME, "void");
//          Board_SetValue(b, COL_DV_REL, "void");
        }
      Board_SetValue(b, COL_DR_TIME, FormaT(tab[i].datareq*DISPLAY_time_unit, buf, DISPLAY_time_format));
/*      if ((tab[i].flags & STB_DEBUG_DIRECTIVE)==0)
        {
          if (tab[i].period>0)
            sprintf(buf, "%s B%c", FormaT(fmod(tab[i].datavalidclock-tab[i].datareq, tab[i].period)*DISPLAY_time_unit, buf, DISPLAY_time_format), tab[i].datavalidclockdir=='u'?'R':'F');
          else
            strcpy(buf,"");
          Board_SetValue(b, COL_DR_REL, buf);
        }*/
      if (tab[i].datavalid<1)
        {
          Board_SetValue(b, COL_SLACK, FormaT(tab[i].slack*DISPLAY_time_unit, buf, DISPLAY_time_format));
        }
      else Board_SetValue(b, COL_SLACK, "void");

      if ((tab[i].flags & STB_DEBUG_DIRECTIVE)==0)
        {
          if (isclock(path_start_ev->ROOT))
            startsig="C";
          else 
            {
              startsig=typeconv(ttv_GetSignalType(path_start_ev->ROOT));
              //          if (strcmp(startsig, "S")==0) startsig="s";
            }
          
          if (isclock(tab[i].debugl.SIG2))
            endsig="C";
          else 
            endsig=typeconv(ttv_GetSignalType(tab[i].debugl.SIG2));
        }
      else
        {
          if (tab[i].flags & STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK)
            startsig="CP";
          else
            startsig="DP";
          if (tab[i].flags & STB_DEBUG_DIRECTIVE_DELAYMODE)
            endsig="CST";
          else if (tab[i].flags & STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA)
            endsig="DP";
          else
            endsig="CP";
        }
      
      sprintf(buf,"%s-%s", startsig, endsig);
          
      Board_SetValue(b, COL_STARTEND, buf);

      if (addit & 1)
        {
          if (setuphold) detail=&tab[i].debugl.detail[(int)tab[i].dirout].setup;
          else detail=&tab[i].debugl.detail[(int)tab[i].dirout].hold;
          Board_SetValue(b, COL_DATAPATHMARGIN, FormaT((detail->data_margin*1e-12/TTV_UNIT)*DISPLAY_time_unit, buf, DISPLAY_time_format));
          Board_SetValue(b, COL_CLOCKPATHMARGIN, FormaT((detail->clock_margin*1e-12/TTV_UNIT)*DISPLAY_time_unit, buf, DISPLAY_time_format));
          Board_SetValue(b, COL_INTRINSIC_MARGIN, FormaT((detail->margin*1e-12/TTV_UNIT)*DISPLAY_time_unit, buf, DISPLAY_time_format));
        }
      
      count++;
    }

  avt_fprintf(f, "      *** %s Slack report (unit:[%s]) ***\n\n",setuphold?"Setup":"Hold", DISPLAY_time_string);

  Board_Display(f, b, "");
  Board_FreeBoard(b);

  avt_fprintf(f, "\n\n");

  if (!sumaonly)
    {
      char *olddetmode;
      olddetmode=ttv_AutomaticDetailBuild("on");
      countfree=0;
      allpaths=NULL;
      if (!sotab)
        speedinput = ttv_getsigbytype_and_netname(stbfig->FIG,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B,NULL) ;
      else
        stb_GetAllSlackPath(given_objects);

      for (i=0; i<tot && i<maxnb; i++)
        {
  
          if (!sotab)
            allpaths=__stb_getslackpaths(stbfig->FIG, tab, start, dirin, direction, setuphold, i, tot, allpaths, &chain, speedinput);
          else
            {
              if (sotab[i]->SI.path==NULL)
                stb_GetSlackPath(sotab[i], 'v');
              tab[i].path=sotab[i]->SI.path;
              chain=NULL;
            }
          
          if (tab[i].path)
            {
              int oldmode=detail_forced_mode;

              if (ttv_GetPathEndDirection(tab[i].path)=='u') dir=1; else dir=0;
              if (setuphold) detail=&tab[i].debugl.detail[dir].setup;
              else detail=&tab[i].debugl.detail[dir].hold;
              
              detail_forced_mode=NOSUPINFO_DETAIL;
//              if (getptype(tab[i].path->NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL)
                {
                  if (!sotab || sotab[i]->custom==0)
                    ttv_CheckTransformToIdeal(tab[i].path);
//                    detail_forced_mode|=IDEAL_CLOCK;
                }
              
              avt_fprintf(f, "Path (%d) :  Slack of %s\n", i+1, FormaT(tab[i].slack*DISPLAY_time_unit, buf, DISPLAY_time_format));
              avt_fprintf(f, "     DATA VALID:\n");
              
              tab[i].path->CMD=tab[i].debugl.CMD_EVENT;
/*              if (!sotab)
                tab[i].path->USER=addptype(tab[i].path->USER, DEBUG_PTYPE, &tab[i].debugl);*/
              max=0;

              if (detail->data_margin!=0)
                {
                  PATH_MORE_INFO.add[max].label="[PATH MARGIN]";
                  PATH_MORE_INFO.add[max].val=(detail->data_margin*1e-12/TTV_UNIT);
                  PATH_MORE_INFO.add[max].noacc=0;
                  max++;
                }

              PATH_MORE_INFO.add[max].label=NULL;
              PATH_MORE_INFO.enabled=1;
              ttv_DisplayPathDetail(f, -1, tab[i].path);
              PATH_MORE_INFO.enabled=0;
//              tab[i].path->USER=testanddelptype(tab[i].path->USER, DEBUG_PTYPE);
              tab[i].path->USER=testanddelptype(tab[i].path->USER, MORE_PTYPE);

//              detail_forced_mode&=~IDEAL_CLOCK;

              if (chain!=NULL || (sotab && (detail->flags & STB_DEBUG_SPECOUTMODE)==0 && (tab[i].flags & STB_DEBUG_DIRECTIVE_DELAYMODE)==0))
                {
                  if (!sotab)
                    pth=(ttvpath_list *)chain->DATA;
                  else
                    pth=stb_GetSlackPath(sotab[i], 'r');

                  max=0;

                  sh_m=(detail->margin*1e-12/TTV_UNIT);
                  if (detail->clock_margin!=0)
                    {                      
                      PATH_MORE_INFO.add[max].label="[PATH MARGIN]";
                      PATH_MORE_INFO.add[max].val=(detail->clock_margin*1e-12/TTV_UNIT);
                      PATH_MORE_INFO.add[max].noacc=0;
                      max++;
                    }
                  if (detail->margin!=0)
                    {
                      char *labelsetup, *labelhold;
                      if ((tab[i].flags & STB_DEBUG_DIRECTIVE)==0)
                        labelsetup="[INTRINSIC SETUP]", labelhold="[INTRINSIC HOLD]";
                      else
                        labelsetup="[SETUP MARGIN]", labelhold="[HOLD MARGIN]";
                                    
                      if (setuphold)
                        PATH_MORE_INFO.add[max].label=labelsetup, PATH_MORE_INFO.add[max].val=-(detail->margin*1e-12/TTV_UNIT);
                      else
                        PATH_MORE_INFO.add[max].label=labelhold,  PATH_MORE_INFO.add[max].val=(detail->margin*1e-12/TTV_UNIT);
                      PATH_MORE_INFO.add[max].noacc=0;
                      max++;
                    }
                  if (detail->skew!=0)
                    {
                      PATH_MORE_INFO.add[max].label="[SKEW COMPENSATION]";
                      if (setuphold)
                        PATH_MORE_INFO.add[max].val=detail->skew/TTV_UNIT*1e-12;
                      else
                        PATH_MORE_INFO.add[max].val=-detail->skew/TTV_UNIT*1e-12;
                      PATH_MORE_INFO.add[max].noacc=0;
                      max++;
                    }
                   if (detail->uncertainty!=0)
                    {
                      PATH_MORE_INFO.add[max].label="[CLOCK UNCERTAINTY]";
                      if (setuphold)
                        PATH_MORE_INFO.add[max].val=-detail->uncertainty/TTV_UNIT*1e-12;
                      else
                        PATH_MORE_INFO.add[max].val=detail->uncertainty/TTV_UNIT*1e-12;
                      PATH_MORE_INFO.add[max].noacc=0;
                      max++;
                    }
                  if (!simple)
                    {
                      if (detail->mc_setup_period!=0)
                        {
                          sprintf(buf, "[SETUP MULTICYCLE %d]", detail->nb_setup_cycle);
                          PATH_MORE_INFO.add[max].label=sensitive_namealloc(buf);
                          PATH_MORE_INFO.add[max].val=(detail->mc_setup_period*1e-12/TTV_UNIT);
                          PATH_MORE_INFO.add[max].noacc=1;
                          max++;
                        }
                      if (!setuphold && detail->mc_hold_period!=0)
                        {
                          sprintf(buf, "[HOLD MULTICYCLE %d]", detail->nb_hold_cycle);
                          PATH_MORE_INFO.add[max].label=sensitive_namealloc(buf);
                          PATH_MORE_INFO.add[max].val=-(detail->mc_hold_period*1e-12/TTV_UNIT);
                          PATH_MORE_INFO.add[max].noacc=1;
                          max++;
                        }
                      
                      pm=-(detail->datamoved+detail->mc_setup_period)+detail->moved_clock_period;
                      if (setuphold==0) pm-=detail->period;
                      if (pm!=0)
                        {
                          if (pm>0)
                            PATH_MORE_INFO.add[max].label="[NEXT PERIOD]";
                          else
                            PATH_MORE_INFO.add[max].label="[PREVIOUS PERIOD]";
                          PATH_MORE_INFO.add[max].val=pm/TTV_UNIT*1e-12;
                          PATH_MORE_INFO.add[max].noacc=1;
                          max++;
                        }
                    }

                  PATH_MORE_INFO.add[max].label=NULL;
                  if (max!=0) PATH_MORE_INFO.enabled=1;
                  avt_fprintf(f, "     DATA REQUIRED:\n");
//                  if (getptype(pth->NODE->ROOT->USER, STB_IDEAL_CLOCK)!=NULL)
                    {
                      if (!sotab || sotab[i]->custom==0)
                        ttv_CheckTransformToIdeal(pth);
//                        detail_forced_mode|=IDEAL_CLOCK;
                    }
                  
                  save_req=pth->DELAYSTART;
                  if (sotab && !simple)
                    {
                      pth->DELAYSTART-=detail->moved_clock_period-detail->datamoved+detail->misc;
                      if (setuphold==0) pth->DELAYSTART+=detail->period;
                    }
                  else if (!sotab && simple)
                    {
                      pth->DELAYSTART+=-detail->datamoved-detail->misc+detail->moved_clock_period;
                      if (!setuphold) pth->DELAYSTART-=detail->period;
                    }

                    
                  ttv_DisplayPathDetail(f, -1, pth);

                  pth->DELAYSTART=save_req;

                  PATH_MORE_INFO.enabled=0;
                }
              else if (tab[i].flags & STB_DEBUG_DIRECTIVE_DELAYMODE)
                {
                  avt_fprintf(f, "    -> Specification: Must be stable  %s %s\n\n", setuphold?"after":"before", FormaT(tab[i].datareq*DISPLAY_time_unit, buf, DISPLAY_time_format));
                }
              else
                {
                  ttv_GetNodeSpecout(tab[i].path->ROOT, DISPLAY_time_unit, DISPLAY_time_format, buf, &tab[i].debugl, tab[i].path, setuphold, -detail->datamoved-detail->misc+detail->moved_clock_period);
                  if (strlen(buf)!=0)
                    {
                      avt_fprintf(f, "    -> Specification: %s\n\n", buf);
                    }          
                }
              ttv_FreePathList(chain);
              freechain(chain);
              detail_forced_mode=oldmode;

              if (!sotab)
                {
                  tab[i].path->MD--;
                  if (tab[i].path->MD==NULL) 
                    {
                      countfree++;
                      if (countfree>50)
                        {
                          chain_list *temp=allpaths;
                          allpaths=NULL;
                          for (chain=temp; chain!=NULL; chain=chain->NEXT)
                            {
                              pth=(ttvpath_list *)chain->DATA;
                              if (pth->MD==NULL)
                                {
                                  pth->NEXT=NULL;
                                  ttv_freepathlist(pth);
                                }
                              else allpaths=addchain(allpaths, pth);
                            }
                          countfree=0;
                          freechain(temp);
                        }
                    }
                }
            }
          else
            {
              avt_fprintf(f, "Path (%d) :  Slack of %s\n", i+1, FormaT(tab[i].slack*DISPLAY_time_unit, buf, DISPLAY_time_format));
              avt_fprintf(f, "     DATA VALID: *** FAILED TO RETRIEVE PATH ***\n\n");
            }
        }

      if (!sotab)
        {
          freechain(speedinput);
          for (chain=allpaths; chain!=NULL; chain=chain->NEXT)
            {
              pth=(ttvpath_list *)chain->DATA;
              if (pth!=NULL) pth->MD=NULL;
            }
          ttv_FreePathList(allpaths);
          freechain(allpaths);
        }
      ttv_AutomaticDetailBuild(olddetmode);
    }

 
/*  for (i=0; i<tot; i++)
    {
      if (tab[i].debugl.CHRONO!=NULL)
        {
          
          stb_freestbpair(tab[i].debugl.CHRONO->SPECS);
          stb_freestbpair(tab[i].debugl.CHRONO->SPECS_U);
          stb_freestbpair(tab[i].debugl.CHRONO->SPECS_D);
          mbkfree(tab[i].debugl.CHRONO);
          tab[i].debugl.CHRONO = NULL;
        }
    }
*/

  mbkfree(tab);
  if (sotab) mbkfree(sotab);

  ttv_setsearchexclude(old0, old1, &old0, &old1);
  stbsetdebugflag(lastmode);
  fflush(f);
}

void stb_DisplaySlackReport_sub(FILE *f, stbfig_list *stbfig, char *start, char *end, char *dir, int number, char *mode, double margin)
{
  int setup=0, hold=0, sum=0;
  char *tok, *c;
  char buf[256];
  int oldttvmode=TTV_QUIET_MODE;
  int oldprecisionlevel, addit=0, simple=0;
  long stbmode=stb_SUPMODE;
  char *thru=NULL;

  strcpy(buf, mode);
  tok=strtok_r(buf, " ", &c);

  while (tok!=NULL)
    {
      if (strcasecmp(tok,"setup")==0) setup=1;
      else if (strcasecmp(tok,"hold")==0) hold=1;
      else if (strcasecmp(tok,"all")==0) {setup=hold=1;}
      else if (strcasecmp(tok,"summary")==0) sum=1;
      else if (strcasecmp(tok,"margins")==0) addit|=1;
      else if (strcasecmp(tok,"thru")==0) addit|=2;
      else if (strcasecmp(tok,"simple")==0) simple=1;
      else if (strcasecmp(tok,"noprechlag")==0) stb_SUPMODE|=STB_REMOVE_LAGGING_PRECHARGES;
      else if (strncasecmp(tok,"thru=",5)==0) thru=&tok[5];
      else //avt_error("stbapi", 2, AVT_ERR, "stb_DisplaySlackReport: unknown mode '%s'\n", tok);      
        avt_errmsg(STB_API_ERRMSG, "002", AVT_ERROR, tok);
      tok=strtok_r(NULL, " ", &c);
    }

  if (f==NULL) return;

  if (ftello(f)==0)
    ttv_DumpHeader(f, stbfig->FIG);

  if (!stbfig) {
      avt_errmsg(STB_API_ERRMSG, "003", AVT_ERROR);
//      avt_error("stbapi", 2, AVT_ERR, "stb_DisplaySlackReport: NULL stbfig\n");      
      return;
  }

  TTV_QUIET_MODE=1;

  oldprecisionlevel=ttv_SetPrecisionLevel(0);

  if (number<=0) number=INT_MAX;

  if (setup)
    {
      __stb_DisplaySlackReport_grab(f, stbfig, start, end, dir, 1, margin, sum, number, addit,thru, NULL, simple);
    }
  if (hold)
    {
      __stb_DisplaySlackReport_grab(f, stbfig, start, end, dir, 0, margin, sum, number, addit,thru, NULL, simple);
    }
  
  ttv_SetPrecisionLevel(oldprecisionlevel);

  TTV_QUIET_MODE=oldttvmode;
  stb_SUPMODE=stbmode;
  if (setup==0 && hold==0)
    avt_errmsg(STB_API_ERRMSG, "004", AVT_ERROR, mode);
//    avt_error("stbapi", 1, AVT_ERR, "stb_DisplaySlackReport: unknown or incomplete mode '%s'. Required: 'setup', 'hold' or 'all'\n", mode);
}

void stb_FreeSlackList(chain_list *cl)
{
  slackobject *so;
  while (cl!=NULL)
    {
      so=(slackobject *)cl->DATA;
      if (so->data_valid!=NULL) { so->data_valid->NEXT=NULL; so->data_valid->MD=NULL; ttv_freepathlist(so->data_valid); }
      if (so->data_required!=NULL) { so->data_required->NEXT=NULL; so->data_required->MD=NULL; ttv_freepathlist(so->data_required); }
      mbkfree(so);
      cl=cl->NEXT;
    }
}

chain_list *stb_GetSlacks_sub(stbfig_list *stbfig, char *start, char *end, char *dir, int number, char *mode, double margin, char *thru, int nolagprech)
{
  int setuphold;
  int oldprecisionlevel;
  int oldttvmode=TTV_QUIET_MODE;
  int tot, i, dirin, dirout;
  slackinfo *tab;
  slackobject *so;
  chain_list *cl, *ch;
  int forced;
  long stbmode=stb_SUPMODE;

  ttv_DirectionStringToIndices(dir, &dirin, &dirout);
  forced=ttv_DirectionStringToIndicesHZ(dir);
  
  if (strcasecmp(mode,"setup")==0) setuphold=1;
  else if (strcasecmp(mode,"hold")==0) setuphold=0;
  else 
    {
      avt_errmsg(STB_API_ERRMSG, "002", AVT_ERROR, mode);
      return NULL;
    }

  if (nolagprech) stb_SUPMODE|=STB_REMOVE_LAGGING_PRECHARGES;
  if (number<=0) number=INT_MAX;
  if (thru==NULL || strcmp(thru,"*")==0) thru=NULL;

  TTV_QUIET_MODE=1;
  oldprecisionlevel=ttv_SetPrecisionLevel(0);
  tab=__stb_getslacklist(stbfig, setuphold, start, end, dirin, dirout, margin, number, &tot, forced, thru);

  cl=NULL; ch=NULL;
  for (i=0; i<tot; i++)
    {
      so=(slackobject *)mbkalloc(sizeof(slackobject));
      memcpy(&so->SI, &tab[i], sizeof(slackinfo));
      so->setuphold=setuphold;
      so->custom=0;
      so->data_valid=so->data_required=NULL;
      if (i<number)
        cl=addchain(cl, so);
      else
        ch=addchain(ch, so);
    }
  mbkfree(tab);
  stb_FreeSlackList(ch);
  freechain(ch);
  ttv_SetPrecisionLevel(oldprecisionlevel);
  TTV_QUIET_MODE=oldttvmode;
  stb_SUPMODE=stbmode;
  return reverse(cl);
}


static ptype_list *fitdouble(double val)
{
  ptype_list *pt;
  pt=addptype(NULL, TYPE_DOUBLE, NULL);
  *(float *)&pt->DATA=(float)val;
  return pt;
}
Property *stb_GetSlackProperty (slackobject *so, char *property)
{
    char buf[256];
    Setup_Hold_Computation_Detail_INFO *detail;

    if (!so) {
        sprintf (buf, "error_null_stabilityslack");
        return addptype (NULL, TYPE_CHAR, strdup (buf));
    }

    if (!strcasecmp (property, "TYPE"))
      return addptype (NULL, TYPE_CHAR, strdup(so->setuphold?"setup":"hold"));

    if (!strcasecmp (property, "DATA_REQUIRED_PATH"))
      return addptype (NULL, TYPE_TIMING_PATH, stb_GetSlackPath(so, 'r'));

    if (!strcasecmp (property, "DATA_VALID_PATH"))
      return addptype (NULL, TYPE_TIMING_PATH, stb_GetSlackPath(so, 'v'));

    if (!strcasecmp (property, "DATA_REQUIRED")) 
      return fitdouble(so->SI.datareq);

    if (!strcasecmp (property, "DATA_VALID")) 
      return fitdouble(so->SI.datavalid);

    if (!strcasecmp (property, "START_EVENT"))
      {
        ttvevent_list *path_start_ev;
        int access;
        path_start_ev=stb_getpathstart(ttv_GetSignalTopTimingFigure(so->SI.debugl.SIG2), so->SI.debugl.SIG1_EVENT, "*", (int)so->SI.dirin, &access, so->SI.phase, so->SI.debugl.START_CMD_EVENT, !so->SI.access, so->SI.debugl.START_CLOCK);
        return addptype (NULL, TYPE_TIMING_EVENT, path_start_ev);
      }

    if (!strcasecmp (property, "THRU_EVENT"))
      {
        ttvevent_list *thru=NULL;
        if (so->SI.access) thru=so->SI.debugl.SIG1_EVENT;
        return addptype (NULL, TYPE_TIMING_EVENT, thru);
      }

    if (!strcasecmp (property, "THRU_COMMAND_EVENT"))
      {
        ttvevent_list *thrucmd=NULL;
        if (so->SI.access) thrucmd=so->SI.debugl.START_CMD_EVENT;
        return addptype (NULL, TYPE_TIMING_EVENT, thrucmd);
      }

    if (!strcasecmp (property, "END_EVENT")) 
      return addptype (NULL, TYPE_TIMING_EVENT, &so->SI.debugl.SIG2->NODE[(int)so->SI.dirout]);

    if (!strcasecmp (property, "IS_HZ")) 
    {
      return addptype (NULL, TYPE_CHAR, strdup(so->SI.hzpath?"yes":"no"));
    }

    if (!strcasecmp (property, "VALUE"))
      return fitdouble(so->SI.slack);

    if (so->setuphold) detail=&so->SI.debugl.detail[(int)so->SI.dirout].setup;
    else detail=&so->SI.debugl.detail[(int)so->SI.dirout].hold;

    if (!strcasecmp (property, "DATA_REQUIRED_PATH_MARGIN"))
        return fitdouble(detail->clock_margin*1e-12/TTV_UNIT);

    if (!strcasecmp (property, "DATA_VALID_PATH_MARGIN"))
      return fitdouble(detail->data_margin*1e-12/TTV_UNIT);

    if (!strcasecmp (property, "INTRINSIC_MARGIN"))
      {
        if (so->setuphold)
          return fitdouble(-detail->margin*1e-12/TTV_UNIT);
        else
          return fitdouble(detail->margin*1e-12/TTV_UNIT);
      }

    if (!strcasecmp (property, "UNCERTAINTY"))
      {
        if (so->setuphold)
          return fitdouble(-detail->uncertainty*1e-12/TTV_UNIT);
        else
          return fitdouble(detail->uncertainty*1e-12/TTV_UNIT);
      }

    if (!strcasecmp (property, "SKEW_COMPENSATION"))
      {
        if (so->setuphold)
          return fitdouble(detail->skew/TTV_UNIT*1e-12);
        else
          return fitdouble(-detail->skew/TTV_UNIT*1e-12);
      }

    avt_errmsg(STB_API_ERRMSG, "006", AVT_FATAL, property);
//    fprintf (stderr, "error: unknown property %s\n", property);
    return NULL;
}

static int stb_path_is_clockpath(ttvpath_list *pth)
{
  stbnode *n;
  ttvevent_list *ev;
  stbpair_list *pair;

  if ((pth->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_MUST_BE_CLOCK)!=0
      || (pth->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_USE_CLOCK_START)!=0
      ) return 1;
  if ((pth->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_USE_DATA_START)!=0) return 1;
  if (pth->LATCH==NULL) ev=pth->NODE;
  else ev=pth->LATCH;

  if (ttv_IsClock(ev->ROOT)) return 1;
  n=stb_getstbnode(ev);
  pair=stb_globalstbtab(n->STBTAB, n->NBINDEX);
  stb_freestbpair(pair) ;
  if (pair) return 0;
  if (n->CK!=NULL) return 1;
  return 0;
}

slackobject *stb_ComputeOneSlack(int setup, ttvpath_list *data, ttvpath_list *clock, double margin, int nextcycle, int nosync)
{
  char phase_data, phase_clock;
  int clockisdata=0, dataisclock=0,dirout;
  long oc,od, cs, ds, val, lmargin, d_valid, d_req, css, dss, movedck,pper;
  inffig_list *ifl;
  ttvfig_list *tf;
  slackobject *so;
  stbfig_list *sf;
  Setup_Hold_Computation_Detail_INFO *detail;
  Extended_Setup_Hold_Computation_Data_Item eshdi;
  int flags=STB_DEBUG_DIRECTIVE;
  slackinfo *si;

  tf=ttv_GetSignalTopTimingFigure(data->NODE->ROOT);

  sf=stb_getstbfig(tf);

  if (sf==NULL) return NULL;

  ifl=getloadedinffig(tf->INFO->FIGNAME);

  phase_data=data->PHASE;
  phase_clock=clock->PHASE;
  if (stb_path_is_clockpath(data)) dataisclock=1, flags|=STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK;
  if (!stb_path_is_clockpath(clock)) clockisdata=1, flags|=STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA;

  if (ifl!=NULL && ifl->LOADED.INF_FALSESLACK!=NULL)
    {
      int res;
      res=stb_isfalseslack(sf, data->LATCH?data->NODE:NULL, data->LATCH?data->LATCH:data->NODE, data->ROOT, clock->NODE, setup?INF_FALSESLACK_SETUP:INF_FALSESLACK_HOLD);
      if ((ttv_PathIsHZ(data) && (res & INF_FALSESLACK_HZ)!=0) || (!ttv_PathIsHZ(data) && (res & INF_FALSESLACK_NOTHZ)!=0)) return NULL;
    }

  so=mbkalloc(sizeof(slackobject));
  so->setuphold=setup;
  so->custom=1;
  so->data_valid=ttv_duppath(data);
  so->data_required=ttv_duppath(clock);
  ttv_CheckTransformToIdeal(so->data_valid);
  ttv_CheckTransformToIdeal(so->data_required);
  if (so->data_valid->ROOT->TYPE & TTV_NODE_UP) dirout=1; else dirout=0;
  si=&so->SI;
  if (setup) detail=&si->debugl.detail[dirout].setup;
  else detail=&si->debugl.detail[dirout].hold;
  
  // path margins
  ttv_get_path_margins(tf, so->data_valid, so->data_required, &eshdi.data_margin, &eshdi.clock_margin, &oc, &od, clockisdata, dataisclock);
  eshdi.common=NULL; eshdi.same_origin=0;
  eshdi.value=abs(oc-od);
  movedck=0;

  // to do
  eshdi.clock_uncertainty=0;

  // multicycle
  if (phase_data!=STB_NO_INDEX) od=stb_getperiod(sf, phase_data); else od=0;
  if (phase_clock!=STB_NO_INDEX) oc=stb_getperiod(sf, phase_clock); else oc=0;

  stb_getmulticycleperiod(so->data_valid->LATCH?so->data_valid->LATCH:so->data_valid->NODE, so->data_valid->ROOT, od, oc, &detail->mc_setup_period, &detail->mc_hold_period, &detail->nb_setup_cycle, &detail->nb_hold_cycle);
  if (setup) detail->mc_hold_period=0;

  detail->datamoved=-detail->mc_setup_period;
  pper=0;
  if (!nosync)
    {
      pper=oc;
      if (phase_data!=STB_NO_INDEX && phase_clock!=STB_NO_INDEX)
        {
          if (stb_cmpphase(sf, phase_clock, phase_data)<=0/*phase_clock<=phase_data*/)
          {
            detail->datamoved-=stb_synchronize_slopes(sf, phase_data, phase_clock, oc, 0);
           // detail->datamoved-=oc;
          }
          else if (phase_clock==phase_data && nextcycle)
            movedck=oc;
        }
    }

  if (setup && -detail->datamoved+movedck!=0) eshdi.value=0;
  else if (!setup && -detail->datamoved+detail->mc_hold_period+movedck!=0) eshdi.value=0;

  if (margin<0)
    {
      if (((so->data_valid->ROOT->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_B))!=0) && ((so->data_required->ROOT->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_B))!=0)
          && ((so->data_valid->TYPE & (TTV_FIND_MAX|TTV_FIND_MIN))|(so->data_required->TYPE & (TTV_FIND_MAX|TTV_FIND_MIN)))==(TTV_FIND_MAX|TTV_FIND_MIN))
        {
          margin=ttv_GetLatchIntrinsicQuick(tf, so->data_valid->ROOT, so->data_required->ROOT, setup,NULL);
        }
      else
        margin=0;
    }

  lmargin=_DOUBLE_TO_LONG(margin);

  if (so->data_required->CROSSMINDELAY!=TTV_NOTIME) cs=so->data_required->CROSSMINDELAY; else cs=so->data_required->DELAYSTART;
  css=cs;
  cs+=so->data_required->DELAY+so->data_required->DATADELAY+movedck;
  if (so->data_valid->CROSSMINDELAY!=TTV_NOTIME) ds=so->data_valid->CROSSMINDELAY; else ds=so->data_valid->DELAYSTART;
  dss=ds;
  ds+=so->data_valid->DELAY+so->data_valid->DATADELAY+detail->datamoved;
  if (setup)
    val=(cs+eshdi.clock_margin-eshdi.clock_uncertainty)-(ds+eshdi.data_margin)-lmargin+eshdi.value;
  else
    val=(ds+eshdi.data_margin)+detail->mc_hold_period-(cs+eshdi.clock_margin+lmargin-pper+eshdi.clock_uncertainty)+eshdi.value;

  stb_setdetail(detail, val, ds, cs, lmargin, setup?0:pper, setup?0:detail->mc_hold_period, &eshdi, flags, phase_data, movedck);

  si->debugl.SIG1_EVENT=so->data_valid->LATCH?so->data_valid->LATCH:so->data_valid->NODE;
  si->debugl.SIG1=si->debugl.SIG1_EVENT->ROOT;
  si->debugl.SIG2=so->data_valid->ROOT->ROOT;
  si->debugl.PERIODE=oc;
  si->debugl.CKNAME=si->debugl.CMDNAME=NULL;
  si->debugl.CMD_EVENT=so->data_valid->CMD;
  si->debugl.MARGESETUP=si->debugl.MARGEHOLD=lmargin;
  si->debugl.SETUP=setup?val:STB_NO_TIME;
  si->debugl.HOLD=!setup?val:STB_NO_TIME;
  si->debugl.START_CMD_EVENT=so->data_valid->CMDLATCH?so->data_valid->CMDLATCH:NULL;
  si->debugl.HOLDPERIOD=detail->mc_hold_period+oc;
  
  d_valid=detail->instab+detail->data_margin;
  if (setup)
    d_req=detail->clock+detail->clock_margin-detail->margin+detail->skew;
  else
    {
      d_req=detail->clock+detail->clock_margin+detail->margin-detail->period-detail->skew;
      d_valid+=detail->misc;
    }

  // correction pour multicycle sur le required
  d_valid+=-detail->datamoved-detail->mc_hold_period;
  d_req+=-detail->datamoved-detail->mc_hold_period;

  si->slack=_LONG_TO_DOUBLE(detail->VALUE);

  si->datavalid=_LONG_TO_DOUBLE(d_valid);
  si->lag=_LONG_TO_DOUBLE(data->DATADELAY);
/*  if (ttv_IsClock(so->data_valid->NODE->ROOT)) si->datavalidclock=dss; else si->datavalidclock=-1;
  si->datavalidclockdir=(so->data_valid->NODE->TYPE==TTV_NODE_UP)?1:0;*/
                          
  if (si->debugl.PERIODE==STB_NO_TIME) si->period=-1; else si->period=_LONG_TO_DOUBLE(si->debugl.PERIODE);
  si->datareq=_LONG_TO_DOUBLE(d_req);
  si->path=so->data_valid;
  si->dirout=dirout;
  si->dirin=(so->data_valid->NODE->TYPE & TTV_NODE_UP)?1:0;
  si->access=so->data_valid->LATCH?1:0;
  si->hzpath=ttv_PathIsHZ(so->data_valid)?1:0;
  si->phase=phase_data;
  si->flags=detail->flags;

  so->data_required->DELAYSTART+=-detail->datamoved-detail->misc+detail->moved_clock_period;
  if (!setup) so->data_required->DELAYSTART-=detail->period;
  return so;
}

void stb_DisplaySlackReport_sub2(FILE *f, chain_list *solist, int number, char *mode)
{
  int setup=1, hold=1, sum=0, simple=0;
  char *tok, *c;
  char buf[256];
  int addit=0;
  chain_list *cl, *ch;  
  
  if (f==NULL) return;
  
  if (number<=0) number=INT_MAX;

  if (ftello(f)==0 && solist!=NULL)
    {
      ttvfig_list *tvf;
      tvf=ttv_GetSignalTopTimingFigure(((slackobject *)solist->DATA)->SI.debugl.SIG2);
      ttv_DumpHeader(f, tvf);
    }

  strcpy(buf, mode);
  tok=strtok_r(buf, " ", &c);

  while (tok!=NULL)
    {
      if (strcasecmp(tok,"setup")==0) hold=0;
      else if (strcasecmp(tok,"hold")==0) setup=0;
      else if (strcasecmp(tok,"all")==0) setup=hold=1;
      else if (strcasecmp(tok,"summary")==0) sum=1;
      else if (strcasecmp(tok,"margins")==0) addit|=1;
      else if (strcasecmp(tok,"thru")==0) addit|=2;
      else if (strcasecmp(tok,"simple")==0) simple=1;
      else //avt_error("stbapi", 2, AVT_ERR, "stb_DisplaySlackReport: unknown mode '%s'\n", tok);      
        avt_errmsg(STB_API_ERRMSG, "002", AVT_ERROR, tok);
      tok=strtok_r(NULL, " ", &c);
    }
  
  if (setup)
    {
      ch=NULL;
      for (cl=solist; cl!=NULL; cl=cl->NEXT)
        if (((slackobject *)cl->DATA)->setuphold) ch=addchain(ch, cl->DATA);
      ch=reverse(ch);
      if (ch!=NULL)
        __stb_DisplaySlackReport_grab(f, NULL, NULL, NULL, "??", 1, 0, sum, number, addit,0, ch, simple);
      freechain(ch);
    }

  if (hold)
    {
      ch=NULL;
      for (cl=solist; cl!=NULL; cl=cl->NEXT)
        if (!((slackobject *)cl->DATA)->setuphold) ch=addchain(ch, cl->DATA);
      ch=reverse(ch);
      if (ch!=NULL)
        __stb_DisplaySlackReport_grab(f, NULL, NULL, NULL, "??", 0, 0, sum, number, addit,0, ch, simple);
      freechain(ch);
    }
}

void stb_DisplayCoverage_sub(FILE *f, stbfig_list *sf, int detail)
{
  enum
    {
      ___LATCH,
      ___PRECH,
      ___DIRECT,
      ___BREAK,
      ___OUTPUT,
      ___LAST
    };

  int a, i, k;
  char *name, dir;
  Board *b;
  struct
  {
    int totsig, totsetup, tothold, totevent;
    int totrise, totfall, minus, realtot;
  } res[___LAST];

  char *label[___LAST]={"Latchs","Precharges","Directives","Breakpoints","Outputs"};

  chain_list *cl;
  ttvevent_list *tve;
  stbnode *sn;
  ttv_directive *sd;

#define DC_COL_DIR 0
#define DC_COL_NAME 1
#define DC_COL_SETUP 2
#define DC_COL_HOLD 3
#define DC_COL_DATA 4
#define DC_COL_CLOCK 5
#define DC_COL_SPECOUT 6

  if (f==NULL) return;

  if (ftello(f)==0)
    ttv_DumpHeader(f, sf->FIG);

  fprintf(f,"      *** Coverage Report ***\n\n");

  for (a=0; a<___LAST; a++)
    res[a].totsig=res[a].totsetup=res[a].tothold=res[a].totevent=res[a].totrise=res[a].totfall=res[a].minus=res[a].realtot=0;

  for (cl=sf->NODE; cl!=NULL; cl=cl->NEXT)
    {
      tve=(ttvevent_list *)cl->DATA;
      sn=stb_getstbnode(tve);
      a=-1;
      if (tve->ROOT->TYPE & TTV_SIG_R)
        a=___PRECH;
      else if (tve->ROOT->TYPE & TTV_SIG_L)
        a=___LATCH;
      else if ((tve->ROOT->TYPE & TTV_SIG_C)!=0 
               && (tve->ROOT->TYPE & TTV_SIG_CI)!=TTV_SIG_CI
               && (tve->ROOT->TYPE & TTV_SIG_CX)!=TTV_SIG_CX
               )
        a=___OUTPUT;
      else if ((tve->ROOT->TYPE & TTV_SIG_B)!=0)
        a=___BREAK;

      if (a!=-1 && (tve->TYPE & TTV_NODE_UP)!=0)
        res[a].realtot++;

      if (a==___OUTPUT)
        {
          if (sn->SPECOUT==NULL) k=sn->NBINDEX;
          else
            for (k=0; k<sn->NBINDEX && sn->SPECOUT[k]==NULL; k++) ;
          if (k>=sn->NBINDEX)
            a=-1;
        }
      else if (a==___BREAK)
        {
          if (getptype(tve->ROOT->NODE[0].USER, STB_BREAK_TEST_EVENT)==NULL
              && getptype(tve->ROOT->NODE[1].USER, STB_BREAK_TEST_EVENT)==NULL)
            a=-1;
        }


      if (a!=-1)
        {
          if (tve->TYPE & TTV_NODE_UP) res[a].totsig++;

          if (sn->SETUP!=STB_NO_TIME && sn->HOLD!=STB_NO_TIME)
            {
              if (tve->TYPE & TTV_NODE_UP) res[a].totrise++;
              else res[a].totfall++;
            }

          if (sn->SETUP!=STB_NO_TIME || sn->HOLD!=STB_NO_TIME)
            {
              if (sn->SETUP!=STB_NO_TIME) res[a].totsetup++;
              if (sn->HOLD!=STB_NO_TIME) res[a].tothold++;
              res[a].totevent++;
            }
        }

      a=-1;
      if ((sd=ttv_get_directive(tve->ROOT))!=NULL)
      {
        while (sd!=NULL && sd->filter) sd=sd->next;
        if (sd!=NULL) a=___DIRECT;
      }
      
      if (a!=-1)
        {
          if (tve->TYPE & TTV_NODE_UP) res[a].totsig++;

          if (sn->SETUP!=STB_NO_TIME && sn->HOLD!=STB_NO_TIME)
            {
              if (tve->TYPE & TTV_NODE_UP) res[a].totrise++;
              else res[a].totfall++;
            }
          
          if (sn->SETUP!=STB_NO_TIME || sn->HOLD!=STB_NO_TIME)
            {
              if (sn->SETUP!=STB_NO_TIME) res[a].totsetup++;
              if (sn->HOLD!=STB_NO_TIME) res[a].tothold++;
              res[a].totevent++;
            }
        }
    }
  
  for (i=0; i<___LAST; i++)
    {
      int nbev;
      fprintf(f,"\n_________ %s coverage\n", label[i]);
      if (i==___PRECH) nbev=res[i].totsig;
      else nbev=res[i].totsig*2;
      fprintf(f,"  total = %d signals\n", res[i].realtot);
      fprintf(f,"  total to check = %d signals (%d events)\n", res[i].totsig, nbev);
      if (res[i].totsig>0)
        {
          fprintf(f,"  event setups  = %d (%.1f%%)\n", res[i].totsetup, res[i].totsetup*100.0/nbev);
          fprintf(f,"  event holds   = %d (%.1f%%)\n", res[i].tothold, res[i].tothold*100.0/nbev);
          if (i!=___PRECH)
            fprintf(f,"  event rising  = %d (%.1f%%)\n", res[i].totrise, res[i].totrise*100.0/res[i].totsig);

          fprintf(f,"  event falling = %d (%.1f%%)\n", res[i].totfall, res[i].totfall*100.0/res[i].totsig);
          
          if (detail)
            {
              fprintf(f,"\n  ** DETAIL **\n");
              b=Board_CreateBoard();
              Board_SetSize(b, DC_COL_DIR, 4, 'l');
              Board_SetSize(b, DC_COL_NAME, 5, 'l');
              Board_SetSize(b, DC_COL_SETUP, 8, 'r');
              Board_SetSize(b, DC_COL_HOLD, 8, 'r');
              Board_SetSize(b, DC_COL_DATA, 8, 'r');
              Board_SetSize(b, DC_COL_CLOCK, 8, 'r');
              if (i==___OUTPUT || i==___BREAK) Board_SetSize(b, DC_COL_SPECOUT, 8, 'r');
              //          Board_SetValue(b, DC_COL_DIR, 4, 'l');
              Board_NewLine(b);
              Board_SetValue(b, DC_COL_NAME, "Node Name");
              Board_SetValue(b, DC_COL_SETUP, "Setup");
              Board_SetValue(b, DC_COL_HOLD, "Hold");
              Board_SetValue(b, DC_COL_DATA, "Data");
              Board_SetValue(b, DC_COL_CLOCK, "Clock");
              Board_SetValue(b, DC_COL_SPECOUT, i==___OUTPUT?"OutSpec":"Constraint");
                
              Board_NewSeparation(b);
              for (cl=sf->NODE; cl!=NULL; cl=cl->NEXT)
                {
                  tve=(ttvevent_list *)cl->DATA;
                  if (i==___PRECH && (tve->TYPE & TTV_NODE_UP)!=0) continue;
                  sn=stb_getstbnode(tve);
                  a=-1;
                  if (tve->ROOT->TYPE & TTV_SIG_R)
                    a=___PRECH;
                  else if (tve->ROOT->TYPE & TTV_SIG_L)
                    a=___LATCH;
                  else if ((tve->ROOT->TYPE & TTV_SIG_C)!=0
                           && (tve->ROOT->TYPE & TTV_SIG_CI)!=TTV_SIG_CI
                           && (tve->ROOT->TYPE & TTV_SIG_CX)!=TTV_SIG_CX
                           )
                    a=___OUTPUT;
                  else if (tve->ROOT->TYPE & TTV_SIG_B &&
                           (getptype(tve->ROOT->NODE[0].USER, STB_BREAK_TEST_EVENT)!=NULL
                            || getptype(tve->ROOT->NODE[1].USER, STB_BREAK_TEST_EVENT)!=NULL))
                    a=___BREAK;
                  
                  if (i==___DIRECT && ttv_get_directive(tve->ROOT)!=NULL)
                    a=___DIRECT;
                  
                  if (a==i && (sn->SETUP==STB_NO_TIME || sn->HOLD==STB_NO_TIME))
                    {
                      char buf[5];
                      dir=dirconv(tve->TYPE & TTV_NODE_UP?'u':'d');
                      sprintf(buf,"(%c)",dir);
                      Board_NewLine(b);
                      Board_SetValue(b, DC_COL_DIR, buf);
                      Board_SetValue(b, DC_COL_NAME, ttv_GetFullSignalName_COND (sf->FIG, tve->ROOT));
                      Board_SetValue(b, DC_COL_SETUP, sn->SETUP!=STB_NO_TIME?"yes":"no");
                      Board_SetValue(b, DC_COL_HOLD, sn->HOLD!=STB_NO_TIME?"yes":"no");
                      Board_SetValue(b, DC_COL_CLOCK, sn->CK!=NULL?"yes":"no");
                      if (sn->STBTAB==NULL) k=sn->NBINDEX;
                      else
                        for (k=0; k<sn->NBINDEX && sn->STBTAB[k]==NULL; k++) ;
                      Board_SetValue(b, DC_COL_DATA, k>=sn->NBINDEX?"no":"yes");

                      if (a==___OUTPUT)
                        {
                          if (sn->SPECOUT==NULL) k=sn->NBINDEX;
                          else
                            for (k=0; k<sn->NBINDEX && sn->SPECOUT[k]==NULL; k++) ;
                          Board_SetValue(b, DC_COL_SPECOUT, k>=sn->NBINDEX?"no":"yes");
                        }
                      else
                        {
                          Board_SetValue(b, DC_COL_SPECOUT, getptype(tve->USER, STB_BREAK_TEST_EVENT)==NULL?"no":"yes");
                        }
                    }
                }
              Board_NewSeparation(b);
              Board_Display(f, b, "  ");
              Board_FreeBoard(b);
            }
        }
      fprintf(f,"\n");
    }

}
