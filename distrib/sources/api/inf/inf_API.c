#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#include AVT_H
#include MUT_H
#include INF_H

#define API_USE_REAL_TYPES
#include "inf_API.h"

static char *inffigurename=".unknown.", warned=0;


void inf_DumpRegistry_i(char *name)
{
  inffig_list *ifl=getloadedinffig(name);
  inf_DumpRegistry(stdout,ifl,0);
}

void inf_CheckRegistry_i(char *name, int level, chain_list *data)
{
  inffig_list *ifl=getloadedinffig(name);
  inf_CheckRegistry(stdout,ifl,level,data);
}

inffig_list *__giveinf()
{
  inffig_list *ifl;
  if (warned==0 && inffigurename==".unknown.")
    {
      avt_errmsg(INF_API_ERRMSG, "001", AVT_FATAL);
//      avt_error("infapi", AVT_WAR, 1, "you must first use 'inf_SetFigureName' prior to use other functions\n");
      warned=1;
    }
  if ((ifl=getloadedinffig(inffigurename))!=NULL) return ifl;
  ifl=addinffig(inffigurename);
  return ifl;
}

void inf_SetFigureName(char *name)
{
  inffigurename=namealloc(name);
}

void inf_LoadInformationFile(char *figname)
{
  inffig_list *ifl;
  inf_Dirty(figname);
  if ((ifl=getinffig(figname))==NULL)
    {
      avt_errmsg(INF_API_ERRMSG, "002", AVT_WARNING, figname);
//      avt_error("infapi", AVT_WAR, 2, "figure '%s' could not be found\n",figname);
      ifl=addinffig(figname);
    }
  else
  inf_SetFigureName(figname);
}

void inf_LoadAndMergeInformationFile(char *figname)
{
  infRead(figname, 1);
  inf_SetFigureName(figname);
}

void inf_AddFile(char *filename, char *figname)
{
  inffig_list *ret;
  inf_ignorename=1;
  ret=_infRead(figname, filename, 0);
  avt_log(LOGFILEACCESS, 0, "Loading INF file \"%s\" for figure \"%s\"%s\n", filename, figname,ret!=NULL?" done.":" failed.");
//  avt_error("infapi", -1, AVT_INFO, "load INF '" AVT_BLUE "%s" AVT_RESET "' for figure '%s'%s\n", filename, figname,
//            ret!=NULL ? AVT_GREEN " done" AVT_RESET : AVT_RED " failed" AVT_RESET);
  inf_ignorename=0;
}

void inf_Drive(char *outputname)
{
  inffig_list *ifl;
  ifl=__giveinf();
  infDrive(ifl, outputname, INF_LOADED_LOCATION, NULL);
}

void inf_ExportSections(char *outputname, char *section)
{
  inffig_list *ifl;
  ifl=__giveinf();
  infDrive_filtered(ifl, outputname, INF_LOADED_LOCATION, NULL, section);
}

void inf_DefineSlew(char *name, double slope_rise, double slope_fall)
{
  inffig_list *ifl;
  ifl=__giveinf();
  if (slope_rise<1e-12) slope_rise=1e-12;
  if (slope_fall<1e-12) slope_fall=1e-12;
  inf_AddDouble(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_PIN_RISING_SLEW, slope_rise, NULL);
  inf_AddDouble(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_PIN_FALLING_SLEW, slope_fall, NULL);
}

void inf_DefineSlope(char *name, double slope_rise, double slope_fall)
{
  inf_DefineSlew(name, slope_rise, slope_fall);
}

void inf_DefineDelay(char *origin, char *destination, double delay)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_DELAY, "", mbk_decodeanyvector(origin), mbk_decodeanyvector(destination), 0, delay, NULL);
}

void inf_DefineStop(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_STOP, NULL, NULL);
}

void inf_DefineRename(char *origin, char *destination)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_RENAME, "", mbk_decodeanyvector(origin), mbk_decodeanyvector(destination), 0, 0, NULL);
}

void inf_DefineMemsym(char *origin, char *destination)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_MEMSYM, "", mbk_decodeanyvector(origin), mbk_decodeanyvector(destination), 0, 0, NULL);
}

void inf_DefineSensitive(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_SENSITIVE, NULL, NULL);
}

void inf_DefineSuppress(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_SUPPRESS, NULL, NULL);
}

void inf_DefineInputs(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_INPUTS, NULL, NULL);
}

void inf_DefineNotLatch(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_NOTLATCH, NULL, NULL);
}

void inf_DefineKeepTristateBehaviour(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_KEEP_TRISTATE_BEHAVIOUR, NULL, NULL);
}

void inf_DefineRC(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_RC, INF_YES, NULL);
}

void inf_DefineNORC(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_RC, INF_NO, NULL);
}

void inf_DefineAsynchron(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_ASYNCHRON, NULL, NULL);
}

void inf_DefineDoNotCross(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_DONTCROSS, NULL, NULL);
}

void inf_DefineTransparent(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_TRANSPARENT, NULL, NULL);
}

void inf_DefineInter(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_INTER, NULL, NULL);
}

void inf_DefineBreak(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_BREAK, NULL, NULL);
}

void inf_DefineStrictSetup(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_STRICT_SETUP, NULL, NULL);
}

void inf_DefineCkPrech(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_CKPRECH, NULL, NULL);
}

void inf_DefineNoFalling(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_NOFALLING, NULL, NULL);
}

void inf_DefineNoRising(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_NORISING, NULL, NULL);
}

void inf_SetPeriod(double value)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddDouble(ifl, INF_LOADED_LOCATION, INF_STB_HEADER, INF_DEFAULT_PERIOD, value, NULL);
}

void inf_SetSetupMargin(double value)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddDouble(ifl, INF_LOADED_LOCATION, INF_STB_HEADER, INF_SETUPMARGIN, value, NULL);
}

void inf_SetHoldMargin(double value)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddDouble(ifl, INF_LOADED_LOCATION, INF_STB_HEADER, INF_HOLDMARGIN, value, NULL);
}

void inf_DefineConstraint(char *name, int value)
{
  inffig_list *ifl;
  ifl=__giveinf();
   inf_AddInt(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_STUCK, value?1:0, NULL);
}

void inf_DefineConditionedCommandStates(char *name, char *state)
{
  inffig_list *ifl;
  ifl=__giveinf();
  
  if (strcasecmp(state,"up")==0) state=INF_VERIFUP;
  else if (strcasecmp(state,"down")==0) state=INF_VERIFDOWN;
  else if (strcasecmp(state,"rising")==0) state=INF_VERIFRISE;
  else if (strcasecmp(state,"falling")==0) state=INF_VERIFFALL;
  else if (strcasecmp(state,"noverif")==0) state=INF_NOVERIF;
  else
    {
      avt_errmsg(INF_API_ERRMSG, "003", AVT_ERROR, state);
//      avt_error("infapi", AVT_ERR, 1, "invalid  command state given: '%s'\n", state);
      return;
    }
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_VERIF_STATE, state, NULL);
}

void inf_DefineClockPriority(char *name, char *clock)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_PREFERED_CLOCK, infTasVectName(clock), NULL);
}

void inf_DefineDLatch(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_DLATCH, INF_YES, NULL);
}

void inf_DefineFlipFlop(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_FLIPFLOP, NULL, NULL);
}

void inf_DefineNotDLatch(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_DLATCH, INF_NO, NULL);
}

void inf_DefinePrecharge(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_PRECHARGE, INF_YES, NULL);
}

void inf_DefineNotPrecharge(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_PRECHARGE, INF_NO, NULL);
}

void inf_DefineModelLatchLoop(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_MODELLOOP, INF_YES, NULL);
}

void inf_DefineCkLatch(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_CKLATCH, INF_YES, NULL);
}

void inf_DefineNotCkLatch(char *name)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddString(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_CKLATCH, INF_NO, NULL);
}

void inf_DefineDirout(char *name, int level)
{
  inffig_list *ifl;
  ifl=__giveinf();
  inf_AddInt(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_DIROUT, level, NULL);
}

void inf_DefineClock(chain_list *elems)
{
  inffig_list *ifl;
  char *aname, *bname;
  int mode, val;
  double minup, maxup, mindown, maxdown, period, mult, delta;

  if (elems!=NULL)
    {
      if (strcasecmp(elems->DATA,"virtual")==0) mode=INF_CLOCK_VIRTUAL, elems=elems->NEXT;
      else if (strcasecmp(elems->DATA,"ideal")==0) mode=INF_CLOCK_IDEAL, elems=elems->NEXT;
      else mode=0;

      if (elems!=NULL && strcasecmp(elems->DATA,"!")==0) mode|=INF_CLOCK_INVERTED, elems=elems->NEXT;

      if (elems!=NULL)
        {
          aname=infTasVectName((char *)elems->DATA);
          elems=elems->NEXT;
          
          ifl=__giveinf();  
          inf_AddInt(ifl, INF_LOADED_LOCATION, aname, INF_CLOCK_TYPE, mode, NULL);
          
          if (elems!=NULL)
            {
              if (strcasecmp(elems->DATA,"clock")!=0)
                {
                  if (countchain(elems)>=5)
                    {
                      minup=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      maxup=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      mindown=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      maxdown=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      period=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                    }
                  else
                    avt_errmsg(INF_API_ERRMSG, "004", AVT_ERROR);
//                    avt_error("infapi", 6, AVT_ERR, "missing clock parameter\n");          
                }
              else
                {
                  elems=elems->NEXT;
                  if (countchain(elems)>=3)
                    {
                      bname=infTasVectName((char *)elems->DATA); elems=elems->NEXT;
                      mult=atof((char *)elems->DATA); elems=elems->NEXT;
                      delta=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      
                      if (inf_GetInt(ifl, bname, INF_CLOCK_TYPE, &val)==0)
                        avt_errmsg(INF_API_ERRMSG, "005", AVT_ERROR, bname);
//                        avt_error("infapi", 11, AVT_ERR, "clock '%s' not defined yet\n", bname);
                      else
                        {
                          double per, defper, min;
                          double minr=0, maxr=0, minf=1e-12, maxf=1e-12;
                          if (!inf_GetDouble(ifl, INF_STB_HEADER, INF_DEFAULT_PERIOD, &defper)) defper=-1;
                          if (!inf_GetDouble(ifl, bname, INF_CLOCK_PERIOD, &per)) per=-1;
                          
                          if (defper==-1 && per==-1 && fabs(mult-1)<=1e-15)
                            avt_errmsg(INF_API_ERRMSG, "006", AVT_ERROR, bname);
//                            avt_error("infapi", 12, AVT_ERR, "no period specified for clock '%s' or no default period\n", bname);
                          else
                            {
                              if (per!=-1)
                                period=per*mult;
                              else if (defper!=-1)
                                period=defper*mult;
                              else
                                period=0;
                              
                              inf_GetDouble(INF_FIG, bname, INF_MIN_RISE_TIME, &minr);
                              inf_GetDouble(INF_FIG, bname, INF_MAX_RISE_TIME, &maxr);
                              inf_GetDouble(INF_FIG, bname, INF_MIN_FALL_TIME, &minf);
                              inf_GetDouble(INF_FIG, bname, INF_MAX_FALL_TIME, &maxf);
                              
                              if (minr<minf) min=minr; else min=minf;
                              
                              minup=min+(minr-min)*mult+delta;
                              maxup=min+(maxr-min)*mult+delta;
                              mindown=min+(minf-min)*mult+delta;
                              maxdown=min+(maxf-min)*mult+delta;
                              
                              if ((val & INF_CLOCK_INVERTED)!=0)
                                {
                                  min=minup; minup=mindown; mindown=min;
                                  min=maxup; maxup=maxdown; maxdown=min;              
                                }
                            }
                        }
                    }
                  else
                    avt_errmsg(INF_API_ERRMSG, "007", AVT_ERROR);
 //                   avt_error("infapi", 6, AVT_ERR, "missing clock parameter\n");
                }
              
              if (mode & INF_CLOCK_IDEAL)
                {
                  if (countchain(elems)>=4)
                    {
                      // latencies
                      minup+=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      maxup+=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      mindown+=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                      maxdown+=avt_parse_unit((char *)elems->DATA, 't'); elems=elems->NEXT;
                    }
                  else if (countchain(elems)!=0)
                    avt_errmsg(INF_API_ERRMSG, "007", AVT_ERROR);
//                    avt_error("infapi", 6, AVT_ERR, "missing clock parameter\n");
                }
              
              if (elems!=NULL)
                 avt_errmsg(INF_API_ERRMSG, "008", AVT_ERROR, (char *)elems->DATA);
                 //avt_error("infapi", 5, AVT_WAR, "too many parameters starting at '%s'\n", (char *)elems->DATA);
              
              inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MIN_RISE_TIME, minup, NULL);
              inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MAX_RISE_TIME, maxup, NULL);
              
              inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MIN_FALL_TIME, mindown, NULL);
              inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MAX_FALL_TIME, maxdown, NULL);
              
              if (period>0)
                inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_CLOCK_PERIOD, period, NULL);
            }
        }
      else
        avt_errmsg(INF_API_ERRMSG, "007", AVT_ERROR);
//        avt_error("infapi", 6, AVT_ERR, "missing clock parameter\n");  
    }
  else
    avt_errmsg(INF_API_ERRMSG, "007", AVT_ERROR);
//    avt_error("infapi", 6, AVT_ERR, "missing clock parameter\n");          
}

/*
void inf_DefineClock(char *name, double minup, double maxup, double mindown, double maxdown, double period)
{
  inffig_list *ifl;
  char *aname;

  ifl=__giveinf();
  aname=infTasVectName(name);
  inf_AddInt(ifl, INF_LOADED_LOCATION, aname, INF_CLOCK_TYPE, 0, NULL);

  inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MIN_RISE_TIME, minup, NULL);
  inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MAX_RISE_TIME, maxup, NULL);

  inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MIN_FALL_TIME, mindown, NULL);
  inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_MAX_FALL_TIME, maxdown, NULL);
  
  if (period>0)
    inf_AddDouble(ifl, INF_LOADED_LOCATION, aname, INF_CLOCK_PERIOD, period, NULL);
}
*/

void inf_DefineBypass(char *name, char *where)
{
  inffig_list *ifl;
  ifl=__giveinf();

  if (strcasecmp(where,"<")==0) where=INF_IN;
  else if (strcasecmp(where,">")==0) where=INF_OUT;
  else if (strcasecmp(where,"!")==0) where=INF_ONLYEND;
  else if (strcasecmp(where,"")==0) where=INF_ALL;
  else
    {
      avt_errmsg(INF_API_ERRMSG, "009", AVT_ERROR, where);
//      avt_error("infapi", AVT_ERR, 2, "invalid bypass location given: '%s'\n", where);
      return;
    }
  inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_BYPASS, where, NULL);
}

void inf_DefineMutex(char *type, chain_list *list)
{
  inffig_list *ifl;
  chain_list *cl, *ch;

  ifl=__giveinf();
  
  if (strcasecmp(type,"muxup")==0) type=INF_MUXU;
  else if (strcasecmp(type,"muxdn")==0) type=INF_MUXD;
  else if (strcasecmp(type,"cmpup")==0) type=INF_CMPU;
  else if (strcasecmp(type,"cmpdn")==0) type=INF_CMPD;
  else
    {
      avt_errmsg(INF_API_ERRMSG, "010", AVT_ERROR, type);
//      avt_error("infapi", AVT_ERR, 3, "unknown mutex type given: '%s'\n", type);
      return;
    }

  cl=dupchainlst(list);
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    ch->DATA=mbk_decodeanyvector((char *)ch->DATA);
  inf_AddList(ifl, INF_LOADED_LOCATION, type, "", cl, NULL);
}

void inf_DefineCrosstalkMutex(char *type, chain_list *list)
{
  inffig_list *ifl;
  chain_list *cl, *ch;

  ifl=__giveinf();
  
  if (strcasecmp(type,"muxup")==0) type=INF_CROSSTALKMUXU;
  else if (strcasecmp(type,"muxdn")==0) type=INF_CROSSTALKMUXD;
  else
    {
      avt_errmsg(INF_API_ERRMSG, "011", AVT_ERROR, type);
//      avt_error("infapi", AVT_ERR, 3, "unknown crosstalk mutex type given: '%s'\n", type);
      return;
    }

  cl=dupchainlst(list);
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    ch->DATA=mbk_decodeanyvector((char *)ch->DATA);
  inf_AddList(ifl, INF_LOADED_LOCATION, type, "", cl, NULL);
}

void inf_DefineDisable(char *origin, char *destination)
{
  inffig_list *ifl;
  char *o, *d;

  ifl=__giveinf();
  
  if (origin==NULL || strcmp(origin,"")==0) o=NULL;
  else o=infTasVectName(origin);
  if (destination==NULL || strcmp(destination,"")==0) d=NULL;
  else d=infTasVectName(destination);

  if (o==NULL && d==NULL) return;

  inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_DISABLE_PATH, "", o, d, 0, 0, NULL);
}

void inf_CleanFigure()
{
  inffig_list *ifl;
  ifl=__giveinf();
  infClean(ifl, INF_LOADED_LOCATION);
}

void inf_DefineAsynchronousClockGroup(char *domain, chain_list *list, double period)
{
  inffig_list *ifl;
  chain_list *ch;

  ifl=__giveinf();
  
  for (ch=list; ch!=NULL; ch=ch->NEXT)
    inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName((char *)ch->DATA), INF_ASYNC_CLOCK_GROUP, infTasVectName(domain), NULL);

  if (period>0)
    inf_AddDouble(ifl, INF_LOADED_LOCATION, INF_ASYNC_CLOCK_GROUP_PERIOD, infTasVectName(domain), period, NULL);
}

void inf_DefineEquivalentClockGroup(char *domain, chain_list *list)
{
  inffig_list *ifl;
  chain_list *ch;

  ifl=__giveinf();
  
  for (ch=list; ch!=NULL; ch=ch->NEXT)
    inf_AddString(ifl, INF_LOADED_LOCATION, infTasVectName((char *)ch->DATA), INF_EQUIV_CLOCK_GROUP, infTasVectName(domain), NULL);
}

void inf_DefinePower(char *name, double voltage)
{
  inffig_list *ifl;

  ifl=__giveinf();
  inf_AddDouble(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_POWER, voltage, NULL);
}

void inf_DefineTemperature(double temp)
{
  inffig_list *ifl;

  ifl=__giveinf();
  inf_AddDouble(ifl, INF_LOADED_LOCATION, INF_OPERATING_CONDITION, INF_TEMPERATURE,temp,NULL);
}

void inf_DefineFalsePath(chain_list *elems)
{
  ptype_list *pt=NULL;
  char *name;
  long type;
  inffig_list *ifl;

  while (elems!=NULL && strcmp(elems->DATA,":")!=0)
    {
      if (strcasecmp(elems->DATA,"<void>")==0)
        name=NULL, type=INF_NOTHING;
      else
        {
          name=infTasVectName(elems->DATA);
          type=INF_UPDOWN;
          if (elems->NEXT!=NULL)
            {
              if (strcasecmp(elems->NEXT->DATA,"<up>")==0 || strcasecmp(elems->NEXT->DATA,"<rise>")==0) type=INF_UP, elems=elems->NEXT;
              else if (strcasecmp(elems->NEXT->DATA,"<down>")==0 || strcasecmp(elems->NEXT->DATA,"<fall>")==0) type=INF_DOWN, elems=elems->NEXT;
            }
        }
      pt=addptype(pt, type, name);
      elems=elems->NEXT;
    }

  while (pt!=NULL && pt->TYPE==INF_NOTHING) pt=delptype(pt, INF_NOTHING);
  pt=(ptype_list *)reverse((chain_list *)pt);
  while (pt!=NULL && pt->TYPE==INF_NOTHING) pt=delptype(pt, INF_NOTHING);
  pt=(ptype_list *)reverse((chain_list *)pt);

  if (pt==NULL) // { avt_error("infapi", 6, AVT_WAR, "no signal elements in falsepath\n"); }
    avt_errmsg(INF_API_ERRMSG, "012", AVT_ERROR);
  else
    {
      if (elems!=NULL)
        {
          if (elems->NEXT==NULL)
             avt_errmsg(INF_API_ERRMSG, "007", AVT_ERROR);
//            { avt_error("infapi", 6, AVT_ERR, "missing clock parameter\n"); }
          else if (elems->NEXT->NEXT!=NULL)
             avt_errmsg(INF_API_ERRMSG, "008", AVT_ERROR, (char *)elems->NEXT->NEXT->DATA);
//            { avt_error("infapi", 5, AVT_WAR, "too many parameters starting at '%s'\n", (char *)elems->NEXT->NEXT->DATA); }
          else
            {
              if (strcasecmp((char *)elems->NEXT->DATA, "<hz>")==0)
                pt=addptype(pt, INF_CK, infTasVectName("^"));
              else if (strcasecmp((char *)elems->NEXT->DATA, "<nothz>")==0)
                pt=addptype(pt, INF_CK, infTasVectName("°"));
              else
                pt=addptype(pt, INF_CK, infTasVectName(elems->NEXT->DATA));
            }        
        }
    }

  if (pt!=NULL)
    {
      ifl=__giveinf();
      pt=(ptype_list *)reverse((chain_list *)pt);
      ifl->LOADED.INF_FALSEPATH=addchain(ifl->LOADED.INF_FALSEPATH, pt);
    }
}

void inf_DefinePathDelayMargin(char *type, char *name, double factor, double delta, char *pathtype_s)
{
  int sigtype, pathtype, i, j, k;
  char *c, *tok;
  char buf[1024];
  inffig_list *ifl;
  struct
  {
    int t;
    char *s;
  } run[]=
    {
      {INF_MARGIN_ON_MINDELAY, INF_PATHDELAYMARGINMIN},
      {INF_MARGIN_ON_MAXDELAY, INF_PATHDELAYMARGINMAX},
      {INF_MARGIN_ON_CLOCKPATH, INF_PATHDELAYMARGINCLOCK},
      {INF_MARGIN_ON_DATAPATH, INF_PATHDELAYMARGINDATA},
      {INF_MARGIN_ON_RISEDELAY, INF_PATHDELAYMARGINRISE},
      {INF_MARGIN_ON_FALLDELAY, INF_PATHDELAYMARGINFALL}
    };
  char section[128];

  if (strcasecmp(type,"any")==0) sigtype=INF_MARGIN_ON_LATCH|INF_MARGIN_ON_BREAK|INF_MARGIN_ON_CONNECTOR|INF_MARGIN_ON_PRECHARGE|INF_MARGIN_ON_CMD;
  else if (strcasecmp(type,"latch")==0) sigtype=INF_MARGIN_ON_LATCH;
  else if (strcasecmp(type,"prech")==0) sigtype=INF_MARGIN_ON_PRECHARGE;
  else if (strcasecmp(type,"break")==0) sigtype=INF_MARGIN_ON_BREAK;
  else if (strcasecmp(type,"cmd")==0) sigtype=INF_MARGIN_ON_CMD;
  else if (strcasecmp(type,"con")==0) sigtype=INF_MARGIN_ON_CONNECTOR;
  else 
    {
      avt_errmsg(INF_API_ERRMSG, "013", AVT_ERROR, type);
//      avt_error("infapi", 7, AVT_ERR, "invalid signal type '%s'\n", type);
      return;
    }

  pathtype=0;
  strcpy(buf, pathtype_s);
  tok=strtok_r(buf, " _", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"min")==0) pathtype|=INF_MARGIN_ON_MINDELAY;
      else if (strcasecmp(tok,"max")==0) pathtype|=INF_MARGIN_ON_MAXDELAY;
      else if (strcasecmp(tok,"rise")==0) pathtype|=INF_MARGIN_ON_RISEDELAY;
      else if (strcasecmp(tok,"fall")==0) pathtype|=INF_MARGIN_ON_FALLDELAY;
      else if (strcasecmp(tok,"clockpath")==0) pathtype|=INF_MARGIN_ON_CLOCKPATH;
      else if (strcasecmp(tok,"datapath")==0) pathtype|=INF_MARGIN_ON_DATAPATH;
      else
        {
          avt_errmsg(INF_API_ERRMSG, "014", AVT_ERROR, tok);
//          avt_error("infapi", 8, AVT_ERR, "unrecognized token '%s'\n", tok);
        }
      tok=strtok_r(NULL, " ", &c);
    }

  ifl=__giveinf();

  if (fabs(delta)>=1) delta*=1e-12;

  if ((pathtype & (INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_MAXDELAY))==0)
    pathtype|=INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_MAXDELAY;
  if ((pathtype & (INF_MARGIN_ON_CLOCKPATH|INF_MARGIN_ON_DATAPATH))==0)
    pathtype|=INF_MARGIN_ON_CLOCKPATH|INF_MARGIN_ON_DATAPATH;
  if ((pathtype & (INF_MARGIN_ON_RISEDELAY|INF_MARGIN_ON_FALLDELAY))==0)
    pathtype|=INF_MARGIN_ON_RISEDELAY|INF_MARGIN_ON_FALLDELAY;

  for (i=0; i<2; i++)
    for (j=2; j<4; j++)
      for (k=4; k<6; k++)
        {
          if ((pathtype & (run[i].t|run[j].t|run[k].t))==(run[i].t|run[j].t|run[k].t))
            {
              sprintf(section,INF_PATHDELAYMARGINPREFIX"|%s,%s,%s",run[i].s, run[j].s, run[k].s);
              inf_AddMiscData(ifl, INF_LOADED_LOCATION, infTasVectName(name), section, NULL, NULL, pathtype|sigtype, factor, delta, NULL);
            }

        }

/*
  if ((pathtype & (INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_CLOCKPATH))==(INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_CLOCKPATH))
    inf_AddMiscData(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_PATHDELAYMARGINCLOCKMIN, NULL, NULL, pathtype|sigtype, factor, delta, NULL);
  
  if ((pathtype & (INF_MARGIN_ON_MAXDELAY|INF_MARGIN_ON_CLOCKPATH))==(INF_MARGIN_ON_MAXDELAY|INF_MARGIN_ON_CLOCKPATH))
    inf_AddMiscData(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_PATHDELAYMARGINCLOCKMAX, NULL, NULL, pathtype|sigtype, factor, delta, NULL);

  if ((pathtype & (INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_DATAPATH))==(INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_DATAPATH))
    inf_AddMiscData(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_PATHDELAYMARGINDATAMIN, NULL, NULL, pathtype|sigtype, factor, delta, NULL);
  
  if ((pathtype & (INF_MARGIN_ON_MAXDELAY|INF_MARGIN_ON_DATAPATH))==(INF_MARGIN_ON_MAXDELAY|INF_MARGIN_ON_DATAPATH))
    inf_AddMiscData(ifl, INF_LOADED_LOCATION, infTasVectName(name), INF_PATHDELAYMARGINDATAMAX, NULL, NULL, pathtype|sigtype, factor, delta, NULL);
*/

}


void inf_DefineIgnore(char *type, chain_list *list)
{
  inffig_list *ifl;
  chain_list *ch;
  char *section_type;

  ifl=__giveinf();

  if (strcasecmp(type,"instance")==0 || strcasecmp(type,"instances")==0)
    section_type=INF_IGNORE_INSTANCE;
  else if (strcasecmp(type,"transistor")==0 || strcasecmp(type,"transistors")==0)
    section_type=INF_IGNORE_TRANSISTOR;
  else if (strcasecmp(type,"resistance")==0 || strcasecmp(type,"resistances")==0)
    section_type=INF_IGNORE_RESISTANCE;
  else if (strcasecmp(type,"diode")==0 || strcasecmp(type,"diodes")==0)
    section_type=INF_IGNORE_DIODE;
  else if (strcasecmp(type,"capacitance")==0 || strcasecmp(type,"capacitances")==0)
    section_type=INF_IGNORE_CAPACITANCE;
  else if (strcasecmp(type,"signalname")==0 || strcasecmp(type,"signalnames")==0)
    section_type=INF_IGNORE_NAMES;
  else if (strcasecmp(type,"parasitics")==0)
    section_type=INF_IGNORE_PARASITICS;
  else
   {
     avt_errmsg(INF_API_ERRMSG, "015", AVT_ERROR, type);
//     avt_error("infapi", 9, AVT_ERR, "unknown section '%s', ignoring section.\n", type);
     section_type=NULL;
   }

  if (section_type!=NULL)
    for (ch=list; ch!=NULL; ch=ch->NEXT)
      inf_AddAssociation(ifl, INF_LOADED_LOCATION, section_type, "", namealloc((char *)ch->DATA), NULL, 0, 0, NULL);
}

void inf_DefineConnectorDirections(char *type, chain_list *list)
{
  inffig_list *ifl;
  chain_list *ch;
  char *val;

  ifl=__giveinf();

  if (strcasecmp(type,"input")==0) val="I";
  else if (strcasecmp(type,"output")==0) val="O";
  else if (strcasecmp(type,"inout")==0) val="B";
  else if (strcasecmp(type,"tristate")==0) val="T";
  else if (strcasecmp(type,"hz")==0) val="Z";
  else if (strcasecmp(type,"unknown")==0) val="X";
  else
   {
     avt_errmsg(INF_API_ERRMSG, "015", AVT_ERROR, type);
//     avt_error("infapi", 10, AVT_ERR, "unknown direction '%s', ignoring section.\n", type);
     val=NULL;
   }

  if (val!=NULL)
    for (ch=list; ch!=NULL; ch=ch->NEXT)
      inf_AddString(ifl, INF_LOADED_LOCATION, namealloc((char *)ch->DATA), INF_CONNECTOR_DIRECTION, val, NULL);

}

void inf_DefineConnectorSwing(char *name, double lowlevel, double highlevel)
{
  inffig_list *ifl;
  char *name0;
  ifl=__giveinf();
  
  name0=infTasVectName (name);

  if (lowlevel!=0) 
    inf_AddDouble(ifl, INF_LOADED_LOCATION, name0, INF_PIN_LOW_VOLTAGE, lowlevel, NULL);
  inf_AddDouble(ifl, INF_LOADED_LOCATION, name0, INF_PIN_HIGH_VOLTAGE, highlevel, NULL);
}

static void convert_data(inffig_list *ifl, char *name, char *section, chain_list *data, int mode, char tm)
{
  chain_list *cl, *ch;
  double val, *al;

  for (cl=data, ch=NULL; cl!=NULL; cl=cl->NEXT)
    {
      val=avt_parse_unit((char *)cl->DATA, tm);
      al=(double *)mbkalloc(sizeof(double));
      *al=val;
      ch=addchain(ch, al);
    }

  ch=reverse(ch);
  if (mode==0)
    {
      inf_RemoveKey(ifl, INF_LOADED_LOCATION, name, section);  
            
      for (cl=ch; cl!=NULL; cl=cl->NEXT)
        {
          inf_AddAssociation(ifl, INF_LOADED_LOCATION, name, section, NULL, NULL, 0, *(double *)cl->DATA, NULL);
          mbkfree(cl->DATA);
        }
    }
  else
    {
      if (countchain(ch)!=3)
         avt_errmsg(INF_API_ERRMSG, "017", AVT_ERROR);
        //avt_error("infapi", AVT_ERR, 11, "too much values for slope or capa generation\n");
      else
        {
          double a=*(double *)ch->DATA, b=*(double *)ch->NEXT->DATA, s=*(double *)ch->NEXT->NEXT->DATA;
          inf_RemoveKey(ifl, INF_LOADED_LOCATION, name, section);  
          for (val = 0 ; val <= (b - a+1e-18) / s ; val++) 
            {
              inf_AddAssociation(ifl, INF_LOADED_LOCATION, name, section, NULL, NULL, 0, a + s * val, NULL);
            }
        }
      for (cl=ch; cl!=NULL; cl=cl->NEXT)
        {
          mbkfree(cl->DATA);
        }
    }
   freechain(ch);
}

static void inf_DefineAxis(char *name, chain_list *list, char *type, char *section, char tm)
{
  inffig_list *ifl;

  ifl=__giveinf();
  
  if (strcasecmp(type,"linear")==0)
    convert_data(ifl, infTasVectName(name), section, list, 1, tm);
  else if (strcasecmp(type,"custom")==0)
    convert_data(ifl, infTasVectName(name), section, list, 0, tm);
  else 
    avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, type);
//    avt_error("infapi", AVT_ERR, 13, "unknown type given: '%s'\n", type);
}

void inf_DefineSlopeRange(char *name, chain_list *list, char *type)
{
  inf_DefineAxis(name, list, type, INF_SLOPEIN, 't');
}

void inf_DefineCapacitanceRange(char *name, chain_list *list, char *type)
{
  inf_DefineAxis(name, list, type, INF_CAPAOUT, 'c');
}

void inf_MarkSignal(char *name, char *marks)
{
    inf_AddInt(__giveinf(), INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_MARKSIG, inf_code_marksig(marks), NULL);
}

void inf_MarkTransistor(char *name, char *marks)
{
    inf_AddInt(__giveinf(), INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_MARKTRANS, inf_code_marktrans(marks), NULL);
}

void inf_DisableTimingArc(char *input, char *output, char *direction)
{
  int dir;
  if (strlen(direction)==2)
    {
      if (tolower(direction[0])=='u' || tolower(direction[0])=='r') dir=0x20;
      else if (tolower(direction[0])=='d' || tolower(direction[0])=='f') dir=0x10;
      else if (direction[0]=='?') dir=0x30;
      else
        {
          avt_errmsg(INF_API_ERRMSG, "019", AVT_ERROR, direction);
//          avt_error("infapi", AVT_ERR, 14, "inf_DisableTimingArc: invalid direction '%s'\n", direction);
          return;
        }

      if (tolower(direction[1])=='u' || tolower(direction[1])=='r') dir|=0x02;
      else if (tolower(direction[1])=='d' || tolower(direction[1])=='f') dir|=0x01;
      else if (direction[1]=='?') dir|=0x03;
      else
        {
          avt_errmsg(INF_API_ERRMSG, "019", AVT_ERROR, direction);
//          avt_error("infapi", AVT_ERR, 14, "inf_DisableTimingArc: invalid direction '%s'\n", direction);
          return;
        }

      inf_AddAssociation(__giveinf(), INF_LOADED_LOCATION, infTasVectName(output), INF_DISABLE_GATE_DELAY, infTasVectName(input), NULL, dir, 0, NULL);
    }
  else
    avt_errmsg(INF_API_ERRMSG, "019", AVT_ERROR, direction);
//    avt_error("infapi", AVT_ERR, 14, "inf_DisableTimingArc: invalid direction '%s'\n", direction);
}

void inf_DefineCharacteristics(char *type, chain_list *list)
{
  inffig_list *ifl;
  chain_list *ch;
  char *val;

  ifl=__giveinf();

  if (strcasecmp(type,"data")==0) val=INF_CHARAC_DATA;
  else
   {
     avt_errmsg(INF_API_ERRMSG, "020", AVT_ERROR, type);
//     avt_error("infapi", 15, AVT_ERR, "unknown characteristic '%s', ignoring section.\n", type);
     val=NULL;
   }

  if (val!=NULL)
    for (ch=list; ch!=NULL; ch=ch->NEXT)
      inf_AddString(ifl, INF_LOADED_LOCATION, namealloc((char *)ch->DATA), val, type, NULL);

}
//chain_list *list)
void inf_DefineDirective_sub(chain_list *list)
{
  int val, delaymode;
  char temp, temp1, act;
  char *nom1, *nom2;
  splitint *si=(splitint *)&val;
  inffig_list *ifl;
  double margin;

  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  ifl=__giveinf();

  if (strcasecmp((char *)list->DATA, "filter")==0) si->cval.a=INF_DIRECTIVE_FILTER;
  else if (strcasecmp((char *)list->DATA, "check")==0) si->cval.a=0;
  else { avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, (char *)list->DATA); return; }

  list=list->NEXT;
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }
    
  if (strcasecmp((char *)list->DATA, "clock")==0) temp=INF_DIRECTIVE_CLOCK, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "data")==0) temp=0, list=list->NEXT;
  else temp=0; //{ avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, sig1type); return; }

  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  nom1=(char *)list->DATA;
  
  list=list->NEXT;
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  if (!strcasecmp((char *)list->DATA,"rising")) temp|=INF_DIRECTIVE_RISING, list=list->NEXT;
  else if (!strcasecmp((char *)list->DATA,"falling")) temp|=INF_DIRECTIVE_FALLING, list=list->NEXT;
//  else temp|=INF_DIRECTIVE_FALLING|INF_DIRECTIVE_RISING;
//  else { avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, dir1); return; }

  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  delaymode=0;
  if (strcasecmp((char *)list->DATA, "before")==0) act=INF_DIRECTIVE_BEFORE;
  else if (strcasecmp((char *)list->DATA, "after")==0) act=INF_DIRECTIVE_AFTER;
  else if (strcasecmp((char *)list->DATA, "with")==0) act=0;
  else { avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, (char *)list->DATA); return; }

  list=list->NEXT;
  if (list==NULL){ avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  if (strcasecmp((char *)list->DATA, "clock")==0) temp1=INF_DIRECTIVE_CLOCK, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "data")==0) temp1=0, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "delay")==0) delaymode=1;
  else temp1=0;

  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }
    
  if (delaymode==0)
  {

    nom2=(char *)list->DATA;

    list=list->NEXT;
    if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

    if (act==0)
      {
        if ((temp1 & INF_DIRECTIVE_CLOCK)==0)
          { avt_errmsg(INF_API_ERRMSG, "026", AVT_ERROR); return; }
        if (!strcasecmp((char *)list->DATA,"up")) temp1|=INF_DIRECTIVE_UP;
        else if (!strcasecmp((char *)list->DATA,"down")) temp1|=INF_DIRECTIVE_DOWN;
        else { avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, (char *)list->DATA); return; }
      }
    else
      {
        if (!strcasecmp((char *)list->DATA,"rising")) temp1|=INF_DIRECTIVE_RISING;
        else if (!strcasecmp((char *)list->DATA,"falling")) temp1|=INF_DIRECTIVE_FALLING;
        else temp1|=INF_DIRECTIVE_FALLING|INF_DIRECTIVE_RISING;
//        else { avt_errmsg(INF_API_ERRMSG, "018", AVT_ERROR, dir2); return; }
      }

    if ((si->cval.a & INF_DIRECTIVE_FILTER)!=0 && ((temp & INF_DIRECTIVE_CLOCK)!=0 || (temp1 & INF_DIRECTIVE_CLOCK)==0 || act!=0))
      { avt_errmsg(INF_ERRMSG, "027", AVT_ERROR); return; }

    list=list->NEXT;
    if (list!=NULL)
      {
        margin=avt_parse_unit((char *)list->DATA, 't');
        if (list->NEXT!=NULL) avt_errmsg(INF_API_ERRMSG, "008", AVT_ERROR, (char *)list->NEXT->DATA);
      }
    else margin=0;
    si->cval.d=temp1;
  }
  else
  {          
    nom2="*"; 
    si->cval.d=INF_DIRECTIVE_DELAY;
    list=list->NEXT;
    if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }
    margin=avt_parse_unit((char *)list->DATA, 't');
    list=list->NEXT;
    if (list!=NULL && !strcasecmp((char *)list->DATA,"from"))
    {
      list=list->NEXT;
      if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }
      nom2=(char *)list->DATA;
      list=list->NEXT;
      if (list!=NULL)
      {
        temp1=0;
        if (!strcasecmp((char *)list->DATA,"rising")) temp1|=INF_DIRECTIVE_RISING;
        else if (!strcasecmp((char *)list->DATA,"falling")) temp1|=INF_DIRECTIVE_FALLING;
        si->cval.d|=temp1;
        list=list->NEXT;
      }
    }
    if (list!=NULL) avt_errmsg(INF_API_ERRMSG, "008", AVT_ERROR, (char *)list->NEXT->DATA);
  }
  si->cval.b=temp;
  si->cval.c=act;

  inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_DIRECTIVES, "", infTasVectName(nom1), infTasVectName(nom2), val, margin, NULL);
}

void inf_DefineRS(char *name, char *type)
{
  int mark=inf_code_markRS(type);
  if (mark!=0) inf_AddInt(__giveinf(), INF_LOADED_LOCATION, mbk_decodeanyvector(name), INF_MARKRS, mark, NULL);
  else avt_errmsg(INF_ERRMSG, "028", AVT_WARNING, type);
}

void inf_DefineFalseSlack_sub(chain_list *list)
{
  char *nom1, *nom2, *nom3, *nom4;
  char dir1, dir2, dir3, dir4, what;
  ptype_list *pt;
  inffig_list *ifl;
  

  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }
  
  ifl=__giveinf();

  what=INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD;
  nom1=(char *)list->DATA;
  nom2=strrchr(nom1,':');
  if (nom2!=NULL && *(nom2+1)=='\0')
  {
    char buf[1024], *tok, *c;
    char val=0;
    strcpy(buf, nom1);
    *strrchr(buf,':')='\0';
    tok=strtok_r(buf, "-", &c);
    while (tok!=NULL)
    {
      if (strcasecmp(tok, "setup")==0) val|=INF_FALSESLACK_SETUP;
      else if (strcasecmp(tok, "hold")==0) val|=INF_FALSESLACK_HOLD;
      else if (strcasecmp(tok, "latch")==0) val|=INF_FALSESLACK_LATCH;
      else if (strcasecmp(tok, "prech")==0) val|=INF_FALSESLACK_PRECH;
      else 
        {
          avt_errmsg(INF_API_ERRMSG, "025", AVT_ERROR, tok);
        }
      tok=strtok_r(NULL, " ", &c);
    }
    if ((val & (INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD))==0)
      val|=INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD;
    what=val;
    list=list->NEXT;
  }
  
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }
  
  nom1=(char *)list->DATA, list=list->NEXT;
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  if (strcasecmp((char *)list->DATA, "<up>")==0 || strcasecmp((char *)list->DATA, "<rise>")==0) dir1=INF_FALSESLACK_UP, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "<down>")==0 || strcasecmp((char *)list->DATA, "<fall>")==0) dir1=INF_FALSESLACK_DOWN, list=list->NEXT;
  else dir1=INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
  
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  nom2=(char *)list->DATA, list=list->NEXT;
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  if (strcasecmp((char *)list->DATA, "<up>")==0 || strcasecmp((char *)list->DATA, "<rise>")==0) dir2=INF_FALSESLACK_UP, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "<down>")==0 || strcasecmp((char *)list->DATA, "<fall>")==0) dir2=INF_FALSESLACK_DOWN, list=list->NEXT;
  else dir2=INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
  
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  nom3=(char *)list->DATA, list=list->NEXT;
  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  if (strcasecmp((char *)list->DATA, "<up>")==0 || strcasecmp((char *)list->DATA, "<rise>")==0) dir3=INF_FALSESLACK_UP, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "<down>")==0 || strcasecmp((char *)list->DATA, "<fall>")==0) dir3=INF_FALSESLACK_DOWN, list=list->NEXT;
  else dir3=INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
  
  if (strcasecmp((char *)list->DATA, "<hz>")==0) dir3|=INF_FALSESLACK_HZ, list=list->NEXT;
  else if (strcasecmp((char *)list->DATA, "<nothz>")==0) dir3|=INF_FALSESLACK_NOTHZ, list=list->NEXT;
  else dir3|=INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ;

  if (list==NULL) { avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); return; }

  nom4=(char *)list->DATA, list=list->NEXT;
  if (list!=NULL) 
  { 
    if (strcasecmp((char *)list->DATA, "<up>")==0 || strcasecmp((char *)list->DATA, "<rise>")==0) dir4=INF_FALSESLACK_UP, list=list->NEXT;
    else if (strcasecmp((char *)list->DATA, "<down>")==0 || strcasecmp((char *)list->DATA, "<fall>")==0) dir4=INF_FALSESLACK_DOWN, list=list->NEXT;
    else dir4=INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
  }
  else dir4=INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
  
  if (list!=NULL) avt_errmsg(INF_API_ERRMSG, "008", AVT_ERROR, (char *)list->DATA);

  pt=addptype(NULL, dir4, infTasVectName(nom4));
  pt=addptype(pt, dir3, infTasVectName(nom3));
  pt=addptype(pt, dir2, infTasVectName(nom2));
  pt=addptype(pt, dir1|what, infTasVectName(nom1));
  ifl->LOADED.INF_FALSESLACK=addchain(ifl->LOADED.INF_FALSESLACK, pt);
}

void inf_DefineSwitchingProbability(char *name, double val)
{
  inffig_list *ifl;

  ifl=__giveinf();
  if (val>=0 && val<=1)
  {
    name=infTasVectName (name);  
    inf_AddDouble(ifl, INF_LOADED_LOCATION, name, INF_SWITCHING_PROBABILITY, val, NULL);
  }
  else
    avt_errmsg(INF_API_ERRMSG, "029", AVT_ERROR, val);
}

