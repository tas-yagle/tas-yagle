#include <stdlib.h>
#include AVT_H
#include STM_H
#include TTV_H
#include TLF_H
#include CBH_H
#include MSL_H
#include LIB_H
#include SPF_H
#include SPE_H
#include BEH_H
#include BVL_H
#include BGL_H

#include <tcl.h>
#define API_USE_REAL_TYPES
#include "avt_API.h"
#include "../ttv/ttv_API_LOCAL.h"
#include "../ttv/ttv_API.h"

extern int shell_main(int argc, char **argv);
void   tclavt_exit (ClientData cd);

extern char *avtshell_RcFileName;

/****************************************************************************/
chain_list * avt_PinList(char *name)
{
  lofig_list *lf;
  locon_list *lc;
  chain_list *res;

  res = NULL;
  
  lf = getloadedlofig(name);
  for (lc = lf->LOCON; lc; lc = lc->NEXT)
    res = addchain(res,lc->NAME);
  
  return res;
}

/****************************************************************************/
chain_list * avt_SigList(char *name)
{
  lofig_list *lf;
  losig_list *ls;
  chain_list *res;

  res = NULL;
  
  lf = getloadedlofig(name);
  for (ls = lf->LOSIG; ls; ls = ls->NEXT)
    res = addchain(res,getsigname(ls));
  
  return res;
}

/*************************************************************************/

char *avt_gettcldistpath()
{
  char *e;
  char temp[4096];
  e=getenv("AVT_TOOLS_DIR");
  if (e==NULL)
    {
      e=getenv("AVERTEC_TOP");
      if (e==NULL) return NULL;
    }
  sprintf(temp,"%s/tcl",e);
  return sensitive_namealloc(temp);
}


int main (int argc, char **argv) 
{
    char pth[1024];

    avtenv ();
    mbkenv ();
    rcnenv ();

    sprintf(pth,"AVTDISTPATH=%s", avt_gettcldistpath());
    putenv(pth);
   
    atexit(avt_PrintErrorSummary);

    if( argc > 1 ) {
      if( mbk_activatecomslave( argv[ argc-1 ] ) )
        argc-- ;
    }
    
#ifdef AVERTEC_LICENSE
    if(avt_givetoken("AVT_SHELL_LICENSE_SERVER","avt_shell") != AVT_VALID_TOKEN )
    EXIT (1);
    Tcl_CreateExitHandler (tclavt_exit, (ClientData)0);
#endif


    libenv ();
    tlfenv ();
    cbhenv ();
    ttvenv ();

    shell_main (argc, argv);

    EXIT (0);
    return 0;
}

void avt_Config (char *var, char *val)
{
  avt_sethashvar_sub (var, val, 0, AVT_VAR_SET_BY_TCLCONFIG);
  if( strcasecmp( var, "avtLogFile"   ) == 0 ||
      strcasecmp( var, "avtLogEnable" ) == 0    )
    avt_initlog();
}

void avt_config (char *var, char *val)
{
  avt_Config( var, val );
}

char *avt_GetConfig(char *var)
{
  char *res=avt_gethashvar_sub(var, 1);
  return res;
}
char *avt_getconfig(char *var)
{
  return avt_GetConfig(var);
}
        
void    tclavt_exit (ClientData cd)
{
    cd = 0;
}

BehavioralFigure *avt_LoadBehavior(char *name, char *format)
{
    FILE       *behfile;

    initializeBdd (0);

    if (strcasecmp(format,"vhdl")==0) {
        avt_log(LOGFILEACCESS, 0, "Loading VHDL behavior \"%s\"\n", name);
        if ((behfile = mbkfopen (name, V_STR_TAB[__BVL_FILE_SUFFIX].VALUE, READ_TEXT))) { 
            fclose (behfile);
            return vhdloadbefig(NULL, name, BVL_KEEPAUX);
        }
    }
    else if (strcasecmp(format,"vbe")==0) {
        avt_log(LOGFILEACCESS, 0, "Loading Alliance VHDL behavior \"%s\"\n", name);
        if ((behfile = mbkfopen (name, V_STR_TAB[__BVL_FILE_SUFFIX].VALUE, READ_TEXT))) { 
            fclose (behfile);
            return vbeloadbefig(NULL, name, BVL_KEEPAUX);
        }
    }
    else if (strcasecmp(format,"verilog")==0) {
        avt_log(LOGFILEACCESS, 0, "Loading Verilog behavior \"%s\"\n", name);
        if ((behfile = mbkfopen (name, V_STR_TAB[__BGL_FILE_SUFFIX].VALUE, READ_TEXT))) { 
            fclose (behfile);
            return vlogloadbefig(NULL, name, BGL_KEEPAUX);
        }
    }
    else avt_errmsg(BEF_ERRMSG,"001", AVT_ERROR, format);
    return NULL;
}

void 
avt_DriveBehavior(befig_list *befig, char *format)
{
    if (strcasecmp(format,"vhdl")==0) {
        avt_log(LOGFILEACCESS, 0, "Driving VHDL behavior \"%s\"\n", befig->NAME);
        vhdsavebefig(befig, 0);
    }
    else if (strcasecmp(format,"vbe")==0) {
        avt_log(LOGFILEACCESS, 0, "Driving Alliance VHDL behavior \"%s\"\n", befig->NAME);
        vbesavebefig(befig, 0);
    }
    else if (strcasecmp(format,"verilog")==0) {
        avt_log(LOGFILEACCESS, 0, "Driving Verilog behavior \"%s\"\n", befig->NAME);
        vlogsavebefig(befig, 0);
    }
    else avt_errmsg(BEF_ERRMSG,"001", AVT_ERROR, format);
}

void avt_LoadFile(char *filename, char *format)
{
  chain_list *list, *cl;
  int enastat=0;
  chain_list *lasterr;
  static int first = 0;

  first++;
  if (first==2) { avt_errmsg (AVT_ERRMSG, "048", AVT_WARNING); }

  mbkenv() ;
  elpenv() ;
  mccenv() ;
  rcnenv();

#ifdef DELAY_DEBUG_STAT
  mbk_debugstat (NULL, 1);
#endif
  
  if (mbk_isregex_name(filename)) // juste pour eviter une difference de comportement par rapport a avant (temporaire?)
    {
      cl=addchain(NULL, filename);
      list=BuildFileList(NULL, cl, 1);
      freechain(cl);
    }
  else
    {
      list=addchain(NULL, mbkstrdup(filename));
    }

  for (cl=list; cl!=NULL; cl=cl->NEXT)
    {
      lasterr=MBK_ALL_PARSE_ERROR;
      filename=(char *)cl->DATA;
      if (strcasecmp(format,"spice")==0) 
        {
          avt_log(LOGFILEACCESS, 0, "Loading Spice netlist \"%s\"\n", filename);
          parsespice(filename);
          enastat=1;
        }
      else if (strcasecmp(format,"lib")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading LIB timing \"%s\"\n", filename);
          lib_parse_this_file(filename, NULL);
        }
      else if (strcasecmp(format,"tlf3")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading TLF3 timing \"%s\"\n", filename);
          parsetlf3(filename);
        }
      else if (strcasecmp(format,"tlf4")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading TLF4 timing \"%s\"\n", filename);
          parsetlf4(filename);
        }
      else if (strcasecmp(format,"ttv")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading TTV timing \"%s\"\n", filename);
          ttv_readcell(filename);
          if (ttv_readcell_TTVFIG!=NULL && (ttv_getloadedfigtypes(ttv_readcell_TTVFIG) & TTV_FILE_DTX)==TTV_FILE_DTX)
            ttv_LoadSpecifiedTimingFigure(ttv_readcell_TTVFIG->INFO->FIGNAME);
        }
      else if (strcasecmp(format,"vhdl")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading VHDL netlist \"%s\"\n", filename);
          parsevhdlnetlist(filename);
        }
      else if (strcasecmp(format,"verilog")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading Verilog netlist \"%s\"\n", filename);
          parseverilognetlist(filename);
        }
      else if (strcasecmp(format,"dspf")==0 || strcasecmp(format,"spf")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading DSPF parasitics \"%s\"\n", filename);
          parsespf(filename) ;
          enastat=1;
        }
      else if (strcasecmp(format,"spef")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading SPEF parasitics \"%s\"\n", filename);
          spef_quiet=1;
          parsespef(filename);
          spef_quiet=0;
          enastat=1;
        }
      else if (strcasecmp(format,"inf")==0)
        {
          avt_log(LOGFILEACCESS, 0, "Loading INF file \"%s\"\n", filename);
          _infRead(".unknown.", filename, 0);
        }
      else
          avt_errmsg(AVT_ERRMSG, "042", AVT_ERROR, format, filename);

      if( enastat ) {
        if (MBK_ALL_PARSE_ERROR!=lasterr)
          {
            mbk_parse_error *mpr=(mbk_parse_error *)MBK_ALL_PARSE_ERROR->DATA;
            avt_log( LOGSTATPRS, 1, "Devices ignored during parse of %s [%s]\n",
                     filename, format
                     );
            avt_log( LOGSTATPRS, 1, "Resistor  : %4lu\n", mpr->NB_RESI  );
            avt_log( LOGSTATPRS, 1, "Capacitor : %4lu\n", mpr->NB_CAPA  );
            avt_log( LOGSTATPRS, 1, "Diode     : %4lu\n", mpr->NB_DIODE  );
            avt_log( LOGSTATPRS, 1, "Net       : %4lu\n", mpr->NB_NET   );
            avt_log( LOGSTATPRS, 1, "Instance  : %4lu\n", mpr->NB_INSTANCE   );
            avt_log( LOGSTATPRS, 1, "Transistor: %4lu\n", mpr->NB_LOTRS );
            
          }
      }

      mbkfree(cl->DATA);
    }
  if (list==NULL)
    {
      avt_errmsg(AVT_ERRMSG, "045", AVT_ERROR, filename);
      exit(1);
    }
#ifdef DELAY_DEBUG_STAT
  mbk_debugstat ("load time:", 0);
#endif

  freechain(list);
}

void avt_EncryptSpice(char *inputname, char *outputname)
{
    mbkenv();
    if (mbk_ascii_encrypt(inputname, outputname) != 0) {
        avt_errmsg(AVT_API_ERRMSG, "006", AVT_ERROR, inputname);
    }
}

unsigned long avt_GetMemoryUsage()
{
  return mbkprocessmemoryusage();
}

typedef struct
{
  int flag;
  struct rusage start, end;
  time_t rstart, rend;
  char buf[92];
} avt_watch;

static ht *WATCH_HT=NULL;

static avt_watch *getwatch(char *name)
{
  long l;
  avt_watch *aw;

  name=sensitive_namealloc(name);
  if (WATCH_HT==NULL) WATCH_HT=addht(10);
  if ((l=gethtitem(WATCH_HT, name))!=EMPTYHT) return (avt_watch *)l;
  aw=(avt_watch *)mbkalloc(sizeof(avt_watch));
  addhtitem(WATCH_HT, name, (long)aw);
  aw->flag=0;
  return aw;
}

void avt_StartWatch(char *name)
{
  avt_watch *aw;
  aw=getwatch(name);
  getrusage(RUSAGE_SELF, &aw->start);
  time(&aw->rstart);
  memcpy(&aw->end, &aw->start, sizeof(struct rusage));
  aw->rend=aw->rstart;
  aw->flag=1;
}

void avt_StopWatch(char *name)
{
  avt_watch *aw;
  aw=getwatch(name);
  if ((aw->flag & 1)==0) return;
  getrusage(RUSAGE_SELF, &aw->end);
  time(&aw->rend);
  aw->flag|=2;
}

char *avt_PrintWatch(char *name)
{
  int flag;
  avt_watch *aw;
  unsigned long   temps;
  unsigned long   user, syst;
  unsigned long   userM, userS, userD;
  unsigned long   systM, systS, systD;
  

  aw=getwatch(name);

  flag=aw->flag;
  if (flag==0) {strcpy(aw->buf,"watch not started yet\n");return aw->buf;}
  if (flag==1) avt_StopWatch(name);

  temps = aw->rend - aw->rstart;
  user = (100 * aw->end.ru_utime.tv_sec + (aw->end.ru_utime.tv_usec / 10000))
    - (100 * aw->start.ru_utime.tv_sec + (aw->start.ru_utime.tv_usec / 10000));
  syst = (100 * aw->end.ru_stime.tv_sec + (aw->end.ru_stime.tv_usec / 10000))
    - (100 * aw->start.ru_stime.tv_sec + (aw->start.ru_stime.tv_usec / 10000));
  
  userM = user / 6000;
  userS = (user % 6000) / 100;
  userD = (user % 100) / 10;
  
  systM = syst / 6000;
  systS = (syst % 6000) / 100;
  systD = (syst % 100) / 10;
  
  sprintf(aw->buf, "system:+%02ldm%02ld.%ld user:+%02ldm%02ld.%ld", systM, systS, systD,userM, userS, userD);

  if (flag==1) aw->flag&=~2;
  return aw->buf;
}

chain_list *avt_GetCatalog()
{
  return getcataloglist();
}

void avt_SetCatalog(chain_list *list)
{
  setcataloglist(list);
}

int avt_BuildID()
{
  return (BUILDNUM);
}

int avt_RegexIsMatching(char *nametocheck, char *template)
{
  return mbk_TestREGEX(nametocheck, template);
}

void *_NULL_()
{
  return NULL;
}

int avt_CodeName(char *name)
{
  int i, j;
  if (name==NULL) return 0x12345;
  for (i=0, j=0; name[i]!='\0'; i++) j+=name[i]*(i+1);
  return j;
}

void avt_SetMainSeed(unsigned int value)
{
  eqt_initseed(0, value);
}

void avt_SetGlobalSeed(unsigned int value)
{
  mbk_init_montecarlo_global_seed(value);
}

unsigned int avt_GetMainSeed()
{
  return eqt_getinitseed();
}

unsigned int avt_GetGlobalSeed()
{
  return mbk_get_montecarlo_global_seed_init_saved();
}
