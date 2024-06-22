/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_obj.c                                                    */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/
#include <semaphore.h>

#include SIM_H
#include "sim.h"
#include "sim_util.h"

int    SIM_OUTLOAD = SIM_TRANSISTOR_OUTLOAD;
double SIM_INPUT_START = 0.0;
double SIM_OUT_CAPA_VAL= -1.0;
double SIM_TECSIZE     = 1.0;
double SIM_VTH         = 0.5;
double SIM_VTH_HIGH    = 0.8;
double SIM_VTH_LOW     = 0.2;
double SIM_SLOP        = 200.0;
double SIM_SIMU_STEP   = SIM_UNDEF;
double SIM_DC_STEP     = 0.001;
char  *SIM_SPICE_OPTIONS = NULL;
char  *SIM_SPICESTRING = "spice $";
char  *SIM_SPICENAME   = "spice";
char  *SIM_SPICEOUT    = "$.out";
char  *SIM_SPICESTDOUT = "$.out";
char  *SIM_TECHFILE    = "avtdefault.tec";
char   SIM_TR_AS_INS   = SIM_NO;
void  *SIM_EXTRACT_RULE= NULL;
char   SIM_DEFAULT_DRIVE_NETLIST   = SIM_NO;
char   SIM_DEFAULT_OVR_FILES       = SIM_YES;
char   SIM_DEFAULT_REMOVE_FILES    = SIM_NO;
float  SIM_VT_SLOPEIN              = -1.0;


extern char *spi_devect (char *name);
static inline locon_list *findLoconInFig(lofig_list *fig, char *loconName);
static inline losig_list *findLosigInFig(lofig_list *fig, char *losigName);
static inline loins_list *findLoinsInFig(lofig_list *fig, char *loinsName);
static inline lotrs_list *findLotrsInFig(lofig_list *fig, char *lotrsName);
static inline locon_list *findLoconInLoins(loins_list *ins, char *loconName,char *loconName2);
static inline locon_list *findLoconInTrs(lotrs_list *trs, char *loconName,char *loconName2);
static inline locon_list* sim_find_by_nameInt(char        *name, lofig_list  *topfig, locon_list **loconfound, losig_list **losigfound, chain_list **listins, chain_list  *hiername);

int sim_SetPowerSupply (lofig_list *lofig)
{
    float supply;

    if (V_FLOAT_TAB[__SIM_POWER_SUPPLY].SET==0) 
        if (mbk_GetPowerSupply (lofig, &supply)) {
            V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = supply;
            elpGeneral[elpGVDDMAX] = supply;
            return 1;
        }
    return 0;
}

static int donesimInFork=0;
mbk_sem_t simInFork;

/*static void sim_destroy_sem()
{
  mbk_sem_destroy(simInFork, &simInForkStatic, "simvfork");
}*/
void simenv ()
{
    char *env;
    int stoprequest=0;

    if (!donesimInFork) {
        mbk_sem_init(&simInFork, 1);
        donesimInFork++;
    }

    if ((env = V_STR_TAB[__SIM_OUTLOAD].VALUE)) {
        if (!strcasecmp (env,"dynamic"))         SIM_OUTLOAD = SIM_DYNAMIC_OUTLOAD;
        else if (!strcasecmp (env,"transistor")) SIM_OUTLOAD = SIM_TRANSISTOR_OUTLOAD;
        else if (!strcasecmp (env,"none"))       SIM_OUTLOAD = SIM_NO_OUTLOAD;
    }
    else SIM_OUTLOAD = SIM_TRANSISTOR_OUTLOAD;

    //Simulator related options
    if (V_INT_TAB[__SIM_TOOLMODEL].SET && !V_INT_TAB[__SIM_TOOL].SET) {
        switch (V_INT_TAB[__SIM_TOOLMODEL].VALUE) {
            case SIM_TOOLMODEL_SPICE: V_INT_TAB[__SIM_TOOL].VALUE = SIM_TOOL_SPICE; break;
            case SIM_TOOLMODEL_HSPICE: V_INT_TAB[__SIM_TOOL].VALUE = SIM_TOOL_HSPICE; break;
            case SIM_TOOLMODEL_ELDO: V_INT_TAB[__SIM_TOOL].VALUE = SIM_TOOL_ELDO; break;
            case SIM_TOOLMODEL_TITAN: V_INT_TAB[__SIM_TOOL].VALUE = SIM_TOOL_TITAN; break;
        }
    }
    if (V_INT_TAB[__SIM_TOOL].SET && !V_INT_TAB[__SIM_TOOLMODEL].SET) {
        switch (V_INT_TAB[__SIM_TOOL].VALUE) {
            case SIM_TOOL_SPICE: 
            case SIM_TOOL_LTSPICE: 
            case SIM_TOOL_NGSPICE: 
                V_INT_TAB[__SIM_TOOLMODEL].VALUE = SIM_TOOLMODEL_SPICE;
                break;
            case SIM_TOOL_HSPICE:
            case SIM_TOOL_MSPICE:
                V_INT_TAB[__SIM_TOOLMODEL].VALUE = SIM_TOOLMODEL_HSPICE;
                break;
            case SIM_TOOL_ELDO:
                V_INT_TAB[__SIM_TOOLMODEL].VALUE = SIM_TOOLMODEL_ELDO;
                break;
            case SIM_TOOL_TITAN:
                V_INT_TAB[__SIM_TOOLMODEL].VALUE = SIM_TOOLMODEL_TITAN;
                break;
        }
    }

    if (V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE) {
        if (!V_FLOAT_TAB[__SIM_TNOM].SET) V_FLOAT_TAB[__SIM_TNOM].VALUE=25;
    }

    switch (V_INT_TAB[__SIM_TOOL].VALUE) {
        case SIM_TOOL_ELDO:
            sim_decodeMeasCmdFree();
            sim_decodeMeasCmd (".meas tran %l TRIG v(%s1) val=%v1 %t1{rise,fall}=%n1{LAST}"
                               " TARG v(%s2) val=%v2 %t2{rise,fall}=%n2{LAST}");
            mfe_freerules(&SIM_EXTRACT_RULE);
            mfe_addrule (&SIM_EXTRACT_RULE, "line:/EXTRACT INFORMATION/1****/:* %l = %v targ= %0 trig= %0");
            SIM_SPICE_OPTIONS = sensitive_namealloc ("hmax=1e-12 eps=10u");
            SIM_SPICEOUT      = sensitive_namealloc ("$.chi");
            SIM_SPICESTDOUT   = sensitive_namealloc ("$.out");
            SIM_SPICESTRING   = sensitive_namealloc ("eldo $");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=1;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=0;
            break;
        case SIM_TOOL_TITAN:
        case SIM_TOOL_TITAN7:
            sim_decodeMeasCmdFree();
            sim_decodeMeasCmd (".measure tran %l TRIG v(%s1) val=%v1 %t1{rise,fall}=%n1{LAST}"
                               " TARG v(%s2) val=%v2 %t2{rise,fall}=%n2{LAST}");
            mfe_freerules(&SIM_EXTRACT_RULE);
            mfe_addrule (&SIM_EXTRACT_RULE, "line:/--- Nominal//: %l | %v | %0 | %0");
            SIM_SPICE_OPTIONS = sensitive_namealloc ("reltol=1e-4 tritol=5");
            if (V_BOOL_TAB[__SIM_USE_PRINT].VALUE == 1) SIM_SPICEOUT = sensitive_namealloc ("$.tr.t1.ppr");
            else SIM_SPICEOUT = sensitive_namealloc ("$.tr.measure");
            SIM_SPICESTDOUT   = sensitive_namealloc ("$.out");
            SIM_SPICESTRING   = sensitive_namealloc ("titan $");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=1;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=0;
            break;
        case SIM_TOOL_MSPICE:
            V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE=0;
            SIM_SPICE_OPTIONS = sensitive_namealloc ("COMPATIBILITY=hspice");
            mfe_freerules(&SIM_EXTRACT_RULE);
            mfe_addrule (&SIM_EXTRACT_RULE, "table:/.TITLE//:%l %v");
            sim_decodeMeasCmdFree();
            sim_decodeMeasCmd (".measure tran %l TRIG v(%s1) val=%v1 %t1{rise,fall}=%n1{LAST}"
                               " TARG v(%s2) val=%v2 %t2{rise,fall}=%n2{LAST}");
            SIM_SPICEOUT      = sensitive_namealloc ("$.mt0");
            SIM_SPICESTRING   = sensitive_namealloc ("mspice $");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=1;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=0;
            break;
        case SIM_TOOL_HSPICE:
            SIM_SPICE_OPTIONS = NULL;
            mfe_freerules(&SIM_EXTRACT_RULE);
            mfe_addrule (&SIM_EXTRACT_RULE, "table:/.TITLE//:%l %v");
            sim_decodeMeasCmdFree();
            sim_decodeMeasCmd (".measure tran %l TRIG v(%s1) val=%v1 %t1{rise,fall}=%n1{LAST}"
                               " TARG v(%s2) val=%v2 %t2{rise,fall}=%n2{LAST}");
            SIM_SPICEOUT      = sensitive_namealloc ("$.mt0");
            SIM_SPICESTRING   = sensitive_namealloc ("hspice $");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=1;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=0;
            break;
        case SIM_TOOL_LTSPICE:
            sim_decodeMeasCmdFree();
            sim_decodeMeasCmd (".measure tran %l TRIG v(%s1) val=%v1 %t1{rise,fall}=%n1{LAST}"
                               " TARG v(%s2) val=%v2 %t2{rise,fall}=%n2{LAST}");
            mfe_freerules(&SIM_EXTRACT_RULE);
            mfe_addrule (&SIM_EXTRACT_RULE, "line:/Direct Newton/Date:/:%l = %v FROM %0 TO %0");
            SIM_SPICE_OPTIONS = NULL;
            SIM_SPICEOUT      = sensitive_namealloc ("$.log");
            SIM_SPICESTDOUT   = sensitive_namealloc ("$.out");
            SIM_SPICESTRING   = sensitive_namealloc ("ltspice $");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=1;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=0;
            break;
        case SIM_TOOL_NGSPICE:
            SIM_SPICE_OPTIONS = NULL;
            mfe_freerules(&SIM_EXTRACT_RULE);
            mfe_addrule (&SIM_EXTRACT_RULE, "line:/Transient Analysis//:%l = %v targ= %0 trig= %0");
            sim_decodeMeasCmdFree();
            sim_decodeMeasCmd (".measure tran %l TRIG v(%s1) val=%v1 %t1{rise,fall}=%n1{LAST}"
                               " TARG v(%s2) val=%v2 %t2{rise,fall}=%n2{LAST}");
            SIM_SPICEOUT      = sensitive_namealloc ("$.out");
            SIM_SPICESTDOUT   = sensitive_namealloc ("$.out");
            SIM_SPICESTRING   = sensitive_namealloc ("ngspice -b $");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=1;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=0;
            break;
        default:
            SIM_SPICE_OPTIONS = NULL;
            SIM_SPICESTRING = sensitive_namealloc ("spice -b $");
            SIM_SPICEOUT      = sensitive_namealloc ("$.out");
            SIM_SPICESTDOUT   = sensitive_namealloc ("$.out");
            if (!V_BOOL_TAB[__SIM_USE_MEAS].SET) V_BOOL_TAB[__SIM_USE_MEAS].VALUE=0;
            if (!V_BOOL_TAB[__SIM_USE_PRINT].SET) V_BOOL_TAB[__SIM_USE_PRINT].VALUE=1;
    }
            
    if (V_BOOL_TAB[__SIM_USE_MEAS].VALUE == 1 && V_BOOL_TAB[__SIM_USE_PRINT].VALUE == 1) {
        if (V_BOOL_TAB[__SIM_USE_PRINT].SET && !V_BOOL_TAB[__SIM_USE_MEAS].SET) {
            V_BOOL_TAB[__SIM_USE_MEAS].VALUE = 0;
        }
        else V_BOOL_TAB[__SIM_USE_PRINT].VALUE = 0;
    }

    SIM_OUT_CAPA_VAL= V_FLOAT_TAB[__SIM_OUT_CAPA_VAL].VALUE;
    SIM_INPUT_START = V_FLOAT_TAB[__SIM_INPUT_START].VALUE;
    SIM_TECSIZE     = V_FLOAT_TAB[__SIM_TECHNO_SIZE].VALUE;
    SIM_VTH         = V_FLOAT_TAB[__SIM_VTH].VALUE;
    SIM_VTH_HIGH    = V_FLOAT_TAB[__SIM_VTH_HIGH].VALUE;
    SIM_VTH_LOW     = V_FLOAT_TAB[__SIM_VTH_LOW].VALUE;
    if ((env = getenv ("SIM_SLOPE_INTERNAL_THRESHOLDS")) && (!strcasecmp (env, "yes"))) {
        SIM_VTH_HIGH    = ELPINITTHR ;
        SIM_VTH_LOW     = ELPINITTHR ;
    }

    SIM_SLOP        = V_FLOAT_TAB[__SIM_INPUT_SLOPE].VALUE * 1.0E12;
    SIM_SIMU_STEP   = V_FLOAT_TAB[__SIM_SIMU_STEP].VALUE * 1.0E9;
    SIM_DC_STEP     = V_FLOAT_TAB[__SIM_DC_STEP].VALUE;

    if ((env = V_STR_TAB[__SIM_MEAS_CMD].VALUE))
    {
      sim_decodeMeasCmdFree();
      sim_decodeMeasCmd(env);
    }
    if ((env = V_STR_TAB[__SIM_EXTRACT_RULE].VALUE)) 
    {
      mfe_freerules(&SIM_EXTRACT_RULE);
      mfe_addrule(&SIM_EXTRACT_RULE,env);
    }

    if (V_STR_TAB[__SIM_SPICE_OPTIONS].SET) SIM_SPICE_OPTIONS = sensitive_namealloc (V_STR_TAB[__SIM_SPICE_OPTIONS].VALUE);
    if (V_STR_TAB[__SIM_SPICE_STRING].SET) SIM_SPICESTRING = sensitive_namealloc (V_STR_TAB[__SIM_SPICE_STRING].VALUE);
    if (V_STR_TAB[__SIM_SPICE_OUT].SET) SIM_SPICEOUT = sensitive_namealloc (V_STR_TAB[__SIM_SPICE_OUT].VALUE);
    if (V_STR_TAB[__SIM_SPICE_STDOUT].SET) SIM_SPICESTDOUT = sensitive_namealloc (V_STR_TAB[__SIM_SPICE_STDOUT].VALUE);
    if (V_STR_TAB[__SIM_TECH_FILE].SET) SIM_TECHFILE = sensitive_namealloc (V_STR_TAB[__SIM_TECH_FILE].VALUE);
    SIM_TR_AS_INS = V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE?SIM_YES:SIM_NO;
    SIM_DEFAULT_OVR_FILES = V_BOOL_TAB[__SIM_OVR_FILES].VALUE?SIM_YES:SIM_NO;
    
    SIM_DEFAULT_REMOVE_FILES = V_BOOL_TAB[__SIM_REMOVE_FILES].VALUE?SIM_YES:SIM_NO;

    // zinaps le 12/10/2004
    if ((SIM_VTH<0 || SIM_VTH>1) && SIM_VTH!=-1)
     { avt_error("sim", 1, AVT_ERR, "invalid value for variable 'simVth', it should range from 0 to 1\n"); stoprequest=1; }
    if ((SIM_VTH_HIGH<0 || SIM_VTH_HIGH>1) && SIM_VTH_HIGH!=-1)
     { avt_error("sim", 2, AVT_ERR, "invalid value for variable 'simVthHigh', it should range from 0 to 1\n"); stoprequest=1; }
    if ((SIM_VTH_LOW<0 || SIM_VTH_LOW>1) && SIM_VTH_LOW!=-1)
     { avt_error("sim", 3, AVT_ERR, "invalid value for variable 'simVthLow', it should range from 0 to 1\n"); stoprequest=1; }
    
    if (stoprequest) EXIT(1);

    if (SIM_VTH_HIGH<SIM_VTH && SIM_VTH!=-1 && SIM_VTH_HIGH!=-1)
     { avt_error("sim", 4, AVT_ERR, "'simVthHigh' value must be higher than 'simVth'\n"); stoprequest=1; }
    if (SIM_VTH_LOW>SIM_VTH && SIM_VTH!=-1 && SIM_VTH_LOW!=-1)
     { avt_error("sim", 5, AVT_ERR, "'simVthLow' value must be lower than 'simVth'\n"); stoprequest=1; }
 
    if (stoprequest) EXIT(1);
    // ---------------------
    spi_env();
}

void sim_error( char *msg, ... )
{
  va_list arg;

  va_start( arg, msg );

  fflush( stdout );
  fprintf( stderr, "\n\n *** Fatal error in sim library ***\n" );
  vfprintf( stderr, msg, arg );

  EXIT(1);
}

void sim_warning( char *msg, ... )
{
  va_list arg;
  char buf[2048];
  
  va_start( arg, msg );

//  fflush( stdout );
//  fprintf( stderr, "\n\n *** Warning in sim library ***\n" );
  vsprintf( buf, msg, arg );
  avt_error("sim", -1, AVT_WAR, buf);
}

/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
locon_list *sim_find_locon(char *name, lofig_list *topfig,
                           locon_list **loconfound, chain_list **listins )
{
  locon_list    *locon;
  
  if (loconfound)
  {
    sim_find_by_name(name,topfig,loconfound,NULL,listins);
    locon   = *loconfound;
  }
  else
    sim_find_by_name(name,topfig,&locon,NULL,listins);

  return locon;
}

/*}}}************************************************************************/
/*{{{                    sim_get_hier_list()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list* sim_get_hier_list(char *hiername)
{
  chain_list    *decomp = NULL;
  char           word[1024];
  int            pos, destpos;

  pos       = 0;
  destpos   = 0;

  while (hiername[pos] != '\0')
  {
    if( hiername[pos] != SEPAR )
      word[destpos++]   = hiername[pos];
    else
    {
      word[destpos]     = '\0';
      destpos           = 0;
      decomp            = addchain(decomp,namealloc(word));
    }
    pos ++;
  }

  if (destpos)
  {
    word[destpos]       = '\0';
    decomp              = addchain(decomp,namealloc(word));
  }

  return reverse(decomp);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/* Recherche un locon ou un losig d'après son nom hiérarchique. Renvoie la liste
chaînée des instances où il se trouve depuis topfig.

      Les noms doivent être sous la forme :
      Z                 -> Locon sur l'interface de lofig.
      tr.Z              -> Locon sur un transistor de la lofig.
      i1.Z              -> Locon sur une instance de la lofig.
      i1.tr.Z           -> Locon sur un transistor de l'instance i1
      i1.i2.Z           -> Locon sur une instance i2 de l'instance i1
      i1.i2.tr.Z        -> et ainsi de suite.
    OU
      i1.i2.tr.Z        -> la netlist est à plat, le trs s'appelle i1.i2.tr
      ...
*/
/****************************************************************************/
void sim_find_by_name( char *name,
                       lofig_list *topfig,
                       locon_list **loconfound,
                       losig_list **losigfound,
                       chain_list **listins
                     )
{
  chain_list    *hiername;
  locon_list    *locon;
  losig_list    *losig;
  
  hiername      = sim_get_hier_list(name);
  
  if (loconfound)
    *loconfound = NULL;
  if (losigfound)
    *losigfound = NULL;
  if (listins)
    *listins    = NULL;
  
  // recherche d'abord sur l interface
  locon         = findLoconInFig(topfig,name);
  // puis les signaux
  losig         = findLosigInFig(topfig,name);
    
  if (!locon && !losig)
  {
    sim_find_by_nameInt(name,topfig,loconfound,losigfound,listins,hiername);
  }
  else
  {
    if (loconfound)
      *loconfound   = locon;
    if (losigfound)
      *losigfound   = losig;
  }
  
  freechain(hiername);

}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline locon_list *findLoconInFig(lofig_list *fig, char *loconName)
{
  locon_list    *locon = fig->LOCON;
  
  while (locon && locon->NAME != loconName)
    locon       = locon->NEXT;

  return locon;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline locon_list *findLoconInLoins(loins_list *ins, char *loconName,
                                           char *loconName2)
{
  locon_list    *locon = ins->LOCON;
  
  if (loconName2)
    while (locon && locon->NAME != loconName && locon->NAME != loconName2)
      locon     = locon->NEXT;
  else
    while (locon && locon->NAME != loconName)
      locon     = locon->NEXT;

  return locon;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline losig_list *findLosigInFig(lofig_list *fig, char *losigName)
{
  losig_list    *losig = fig->LOSIG;
  
  while (losig && getsigname(losig) != losigName)
    losig       = losig->NEXT;

  return losig;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline loins_list *findLoinsInFig(lofig_list *fig, char *loinsName)
{
  loins_list    *loins = fig->LOINS;
  
  while (loins && loins->INSNAME != loinsName)
    loins       = loins->NEXT;

  return loins;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline lotrs_list *findLotrsInFig(lofig_list *fig, char *lotrsName)
{
  lotrs_list    *lotrs = fig->LOTRS;
  
  while (lotrs && (!lotrs->TRNAME || lotrs->TRNAME != lotrsName))
    lotrs       = lotrs->NEXT;

  return lotrs;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline locon_list *findLoconInTrs(lotrs_list *trs, char *loconName,
                                         char *loconName2)
{
  locon_list    *locon;
 
  if (!trs)
    locon       = NULL;
  else if (trs->GRID->NAME == loconName || trs->GRID->NAME == loconName2)
    locon       = trs->GRID;
  else if (trs->SOURCE->NAME == loconName || trs->SOURCE->NAME == loconName2)
    locon       = trs->SOURCE;
  else if (trs->DRAIN->NAME == loconName || trs->DRAIN->NAME == loconName2)
    locon       = trs->DRAIN;
  else if (trs->BULK &&
           (trs->BULK->NAME == loconName || trs->BULK->NAME == loconName2))
    locon       = trs->BULK;
  else
    locon       = NULL;
      
  return locon;
}



/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
locon_list* sim_find_by_nameInt(char        *name,
                                lofig_list  *topfig,
                                locon_list **loconfound,
                                losig_list **losigfound,
                                chain_list **listins,
                                chain_list  *hiername)
{
  loins_list    *loins;
  locon_list    *locon = NULL;
  losig_list    *losig = NULL;
  lotrs_list    *lotrs;
  char           tmpbuf[BUFSIZ], nomtrs[BUFSIZ], *tmp, *insname, *signame;
  chain_list    *chainx, *ins = NULL, *headTrs;
  lofig_list    *figx;
  
  // descend la hiérarchie. Les noms sont de la forme
  // A.B.locon
  // A.tr.locon
  // A.losig
  for (figx = topfig, chainx = hiername; chainx; chainx = chainx->NEXT)
  {
    insname         = (char*)chainx->DATA;
    if ((loins = findLoinsInFig(figx,insname)))
    {
      ins           = addchain(ins,loins);
      figx          = getlofig(loins->FIGNAME,'A');
    }
    else
      break;
  }
  
  headTrs           = chainx;
  signame           = insname; // the last visited is a signal name
  
  /* C'est sur une instance */
  if (ins)
  {
    locon           = findLoconInLoins(ins->DATA,insname,name);
    /* Greg, le 13/10/03 : il faut retirer la dernière instance de la liste 
       puisque le locon ne se situe pas DANS l'instance */
    chainx          = ins ;
    ins             = ins->NEXT ;
    chainx->NEXT    = NULL ;
    freechain( chainx );
    
  }

  /* C'est sur un transistor : on gère le cas où le nom du transistor 
     correspond à un nom hiérarchique. */
  if (!locon && headTrs->NEXT)
  {
    // creation of transistor name
    strcpy(nomtrs,(char*)headTrs->DATA);
    for (chainx = headTrs->NEXT; chainx->NEXT; chainx = chainx->NEXT)
    {
      sprintf(tmpbuf,"%s%c%s",nomtrs,SEPAR,(char*)chainx->DATA);
      strcpy(nomtrs,tmpbuf);
    }

    if ((tmp = namefind(nomtrs)))
      lotrs         = findLotrsInFig(figx,tmp);
    else
      lotrs         = NULL;
    locon           = findLoconInTrs(lotrs,chainx->DATA,name);
  }

  // C'est peut etre un signal
  if (!locon)
    losig           = findLosigInFig(figx,signame);

  if (locon)
  {
    if (loconfound)
      *loconfound   = locon;
    if (listins)
      *listins      = reverse(ins);
    else
      freechain(ins);
  }
  else if (listins)
  {
    freechain(ins);
    *listins      = NULL;
  }
  
  if (losigfound)
    *losigfound     = losig;
  

  return locon;
}

/*}}}************************************************************************/
/* Renvoie la liste chainée des figures nécessaires pour lancer une simulation.
Cette liste est ordonnée des instances les plus basses vers les instances les
plus hautes. */

chain_list *sim_getfiglist( lofig_list *lofig )
{
  chain_list *last;
  chain_list *head;
  chain_list *done;
  chain_list *new;
  loins_list *loins;
  lofig_list *figins;

  head = addchain( NULL, lofig );
  last = head;
  done = head;

  while( done ) {

    lofig = (lofig_list*)(done->DATA);
    for( loins = lofig->LOINS ; loins ; loins = loins->NEXT ) {
      if (incatalog (loins->FIGNAME)) continue;
      figins = getlofig( loins->FIGNAME, 'A' );
      if( !getptype( figins->USER, SIM_MARKED ) ) {
        figins->USER = addptype( figins->USER, SIM_MARKED, NULL );
        new = addchain( NULL, figins );
        last->NEXT = new;
        last = new;
      }
    }
    done = done->NEXT;
  }

  for( done = head->NEXT ; done ; done = done->NEXT ) {
    lofig = (lofig_list*)(done->DATA);
    lofig->USER = delptype( lofig->USER, SIM_MARKED );
  }

  return reverse( head );
}

static chain_list *_sim_getallfiglist(ht *fight, chain_list *head, lofig_list *lofig )
{
  loins_list *loins;
  lofig_list *figins;

  for( loins = lofig->LOINS ; loins!=NULL ; loins = loins->NEXT ) 
   {
     if (gethtitem(fight, loins->FIGNAME)!=EMPTYHT || incatalog (loins->FIGNAME)) continue;
     figins = getlofig( loins->FIGNAME, 'A' );
     head=addchain(head, figins );
     head=_sim_getallfiglist(fight, head, figins);
   }

  return head;
}

chain_list *sim_getallfiglist(lofig_list *lofig )
{
  chain_list *head;
  ht *fight;
  fight=addht(10);

  head=_sim_getallfiglist(fight, NULL, lofig);

  delht(fight);
  return head;
}


/*---------------------------------------------------------------------------*/

char *sim_vect (char *v) /* a 0 -> a[0] */
{
    char sv[1024];
    int i = 0, isvector = 0;

    while (v[i] != '\0')
        if (v[i] != ' ') {
            sv[i] = v[i];
            i++;
        }
        else {
            isvector = 1;
            sv[i++] = '[';
        }

    if (isvector)
        sv[i++] = ']';

    sv[i] = '\0';
    return namealloc (sv);
}


/*---------------------------------------------------------------------------*/

char *sim_devect (char *v) /* a[0] -> a 0 */
{
    char sv[1024];
    int i = 0;

    return spi_devect (v);
    while (v[i] != '\0')
        switch (v[i]) {
            case '[':
            case '(':
                sv[i++] = ' ';
                break;
            case ']':
            case ')':
                sv[i++] = '\0';
                break;
            default:
                sv[i] = v[i];
                i++;
        }

    sv[i] = '\0';
    return namealloc (sv);
}

/*---------------------------------------------------------------------------*/

char *sim_spivect (char *v) /* a 0 -> a_0 */
{
    char sv[1024];
    int i = 0;

    while (v[i] != '\0')
        if (v[i] != ' ') {
            sv[i] = v[i];
            i++;
        }
        else
            sv[i++] = '_';

    sv[i] = '\0';
    return namealloc (sv);
}

/*---------------------------------------------------------------------------*/

char *sim_spidevect (char *v) /* a_0 -> a 0 */
{
    char sv[1024];
    int i = 0;

    while (v[i] != '\0') {
        sv[i] = v[i];
        i++;
    }
    sv[i--] = '\0';

    while (isdigit ((int)v[i])) i--;

    if (v[i] == '_')
        sv[i] = ' ';

    return namealloc (sv);
}

/*--------------------------------------------------------------*/
/*--------- useful functions -----------------------------------*/
/*--------------------------------------------------------------*/
#if 0
/*--------- log2 -----------------------------------------------*/

unsigned long log2 (unsigned long n)
{
    unsigned long log = 0;
    while (n /= 2)
        log++;
    return log;
}

/*--------- exp2 -----------------------------------------------*/

unsigned long exp2 (unsigned long n)
{
    long exp = 1;
    while (n--)
        exp *= 2;
    return exp;
}
#endif
/*--------- dec_to_bin -----------------------------------------*/

void reverse_endianess (char *bin)
{
    int i, l;
    char *buf;

    l = strlen (bin);
    buf = (char*)mbkalloc (l * sizeof (char));

    for (i = 0; i < l; i++)
        buf[l - i - 1] = bin[i];
    for (i = 0; i < l; i++)
        bin[i] = buf[i];

    mbkfree(buf);
}

/*--------- dec_to_bin -----------------------------------------*/


void sim_hex2bin (char *bin, char *hex)
{
    int i;
    int l = strlen (hex);

    for (i = 0; i < l; i++)
        switch (hex[i]) {
            case '0':
                strcpy (&bin[4 * i], "0000");
                break;
            case '1':
                strcpy (&bin[4 * i], "0001");
                break;
            case '2':
                strcpy (&bin[4 * i], "0010");
                break;
            case '3':
                strcpy (&bin[4 * i], "0011");
                break;
            case '4':
                strcpy (&bin[4 * i], "0100");
                break;
            case '5':
                strcpy (&bin[4 * i], "0101");
                break;
            case '6':
                strcpy (&bin[4 * i], "0110");
                break;
            case '7':
                strcpy (&bin[4 * i], "0111");
                break;
            case '8':
                strcpy (&bin[4 * i], "1000");
                break;
            case '9':
                strcpy (&bin[4 * i], "1001");
                break;
            case 'a': case 'A':
                strcpy (&bin[4 * i], "1010");
                break;
            case 'b': case 'B':
                strcpy (&bin[4 * i], "1011");
                break;
            case 'c': case 'C':
                strcpy (&bin[4 * i], "1100");
                break;
            case 'd': case 'D':
                strcpy (&bin[4 * i], "1101");
                break;
            case 'e': case 'E':
                strcpy (&bin[4 * i], "1110");
                break;
            case 'f': case 'F':
                strcpy (&bin[4 * i], "1111");
                break;
            default:
                fprintf (stderr, "malformed hex value `%s'\n", hex);
                strcpy (&bin[4 * i], "0000");
        }
}

char *sim_vectorize (char *radix, int index)
{
        char buf[2048];

        sprintf (buf, "%s %d", radix, index);
        return namealloc (buf);
}

/* sim_node_clean permet d'effacer les ic, pwl, addmeaure du node */
void sim_node_clean(sim_model *model, char *name)
{
  locon_list    *locon ;
  losig_list    *losig ;

  sim_find_by_name( name,
                    sim_model_get_lofig( model ),
                    &locon,
                    &losig,
                    NULL
                  );
  if( locon )
    sim_ic_clear( model, name, SIM_IC_LOCON );
  if( losig )
    sim_ic_clear( model, name, SIM_IC_SIGNAL );

  sim_input_clear( model, name);
}


static mbkContext *mc=NULL;

lofig_list *sim_GetNetlist(char *name)
{
  lofig_list *lf;
  if (mc==NULL)
    {
      mc=mbkCreateContext();
    }
  mbkSwitchContext(mc);
  lf=getloadedlofig(name);
  mbkSwitchContext(mc);
  return lf;
}

sim_model *simDuplicate(sim_model *src)
{
  sim_model *sm;
  sim_ic *ic, *ic0, *icprev;
  sim_input *si, *si0, *siprev;
  sim_measure *sme, *sme0, *smeprev;
  sim_measure_detail *smd, *smd0, *smdprev;
  sim_translate *smt;

  // base
  sm=(sim_model *)mbkalloc(sizeof(sim_model));
  memcpy(sm, src, sizeof(sim_model));

  sm->FIG=sim_GetNetlist(src->FIG->NAME);
  if (sm->FIG==NULL)
    {
      sm->FIG=rduplofig(src->FIG);
      lofigchain(sm->FIG);
      // renommage ?
      mbkSwitchContext(mc);
      addhtitem(HT_LOFIG, sm->FIG->NAME, (long)sm->FIG);
      sm->FIG->NEXT=HEAD_LOFIG;
      HEAD_LOFIG=sm->FIG;
      mbkSwitchContext(mc);
    }
  sm->PARAMETER.TECHNOFILELIST=dupchainlst(sm->PARAMETER.TECHNOFILELIST);

  if (sm->PARAMETER.TOOL_CMD!=NULL)
    sm->PARAMETER.TOOL_CMD=mbkstrdup(sm->PARAMETER.TOOL_CMD);
  if (sm->PARAMETER.TOOL_OUTFILE!=NULL)
    sm->PARAMETER.TOOL_OUTFILE=mbkstrdup(sm->PARAMETER.TOOL_OUTFILE);
  if (sm->OUTPUT_FILE!=NULL)
    sm->OUTPUT_FILE=mbkstrdup(sm->OUTPUT_FILE);
  // sim_ic
  for (ic=src->LIC, icprev=NULL; ic!=NULL; icprev=ic0, ic=ic->NEXT)
    {
      ic0=(sim_ic *)mbkalloc(sizeof(sim_ic));
      memcpy(ic0, ic, sizeof(sim_ic));
      if (icprev!=NULL) icprev->NEXT=ic0; else sm->LIC=ic0;
      ic0->NODENAME=dupchainlst(ic->NODENAME);
    }

  // sim_input
  sm->HTINPUT=addht(10);
  for (si=src->LINPUT, siprev=NULL; si!=NULL; siprev=si0, si=si->NEXT)
    {
      si0=(sim_input *)mbkalloc(sizeof(sim_input));
      memcpy(si0, si, sizeof(sim_input));
      if (siprev!=NULL) siprev->NEXT=si0; else  sm->LINPUT=si0;
      addhtitem(sm->HTINPUT, si0->LOCON_NAME, (long)si0);
    }

  // sim_measure
  for (sme=src->LMEASURE, smeprev=NULL; sme!=NULL; smeprev=sme0, sme=sme->NEXT)
    {
      sme0=(sim_measure *)mbkalloc(sizeof(sim_measure));
      memcpy(sme0, sme, sizeof(sim_measure));
      if (smeprev!=NULL) smeprev->NEXT=sme0; else sm->LMEASURE=sme0;
      sme0->NODENAME=dupchainlst(sme->NODENAME);

      for (smd=sme->DETAIL, smdprev=NULL; smd!=NULL; smdprev=smd0, smd=smd->NEXT)
        {
          smd0=(sim_measure_detail *)mbkalloc(sizeof(sim_measure_detail));
          memcpy(smd0, smd, sizeof(sim_measure_detail));
          if (smdprev!=NULL) smdprev->NEXT=smd0; else sme0->DETAIL=smd0;
        }
    }

  // sim_translate
  for (smt=src->TRANSLATION; smt!=NULL; smt=smt->NEXT)
    {
      sim_input_add_alias(sm, smt->signal, smt->equiv);
    }

  // sim extraction
  mfe_duprules(&(sm->LRULES),src->LRULES);
  mfe_duplabels(&(sm->LLABELS),src->LLABELS);

  return sm;
}

#if 0
static int count=1;

sim_model *sim_SaveContext(sim_model *src, char *label)
{
  sim_model *sm;
  sm=simDuplicate(src);
  sm->NUMBER=count++;
  if (label!=NULL)
    {
      if (src->CONTEXT_HT==NULL) src->CONTEXT_HT=addht(15);
      addhtitem(src->CONTEXT_HT, namealloc(label), (long)sm);
    }
  return sm;
}

sim_model *sim_GetSavedContext(sim_model *src, char *label)
{
  long l;
  if (label!=NULL && src->CONTEXT_HT!=NULL)
    {
      if ((l=gethtitem(src->CONTEXT_HT, namealloc(label)))!=EMPTYHT) return (sim_model *)l;
    }
  return NULL;
}

static void _simDriveIC(sim_ic *si, FILE *f)
{
  char *name, *func, val[100];
  switch(si->TYPE)
    {
    case SIM_IC_VOLTAGE:
      func="sim_AddInitVoltage";
      sprintf(val, "%e", si->UIC.VOLTAGE.VOLTAGE);
      break;
    case SIM_IC_LEVEL:
      func="sim_AddInitLevel";
      sprintf(val, "%d", si->UIC.LEVEL.LEVEL==SIM_ZERO?0:1);
      break;
    default: EXIT(2);
    }
  switch(si->LOCATE)
    {
    case SIM_IC_LOCON: name=si->WHERE.LOCON_NAME; break;
    case SIM_IC_SIGNAL: name=si->WHERE.SIGNAL_NAME; break;
    case SIM_IC_SIGNAL_NODE: name=si->WHERE.SIGNAL_NODE_NAME; break;
    default: EXIT(3);
    }
  fprintf(f,"  %s ( SC, \"%s\", %s);\n", func, name, val);
}

static void _simDriveDC(sim_input *si, FILE *f)
{
  char *func, val[100];

  switch(si->TYPE)
    {
    case SIM_STUCK:
      switch(si->UINPUT.INPUT_STUCK.TYPE)
        {
        case SIM_STUCK_VALUE:
          func="sim_AddStuckVoltage";
          sprintf(val, "%e", si->UINPUT.INPUT_STUCK.MODEL.STUCK_VOLTAGE.VALUE);
          break;
        case SIM_STUCK_LEVEL:
          func="sim_AddStuckLevel";
          sprintf(val, "%d", si->UINPUT.INPUT_STUCK.MODEL.STUCK_LEVEL.VALUE==SIM_ZERO?0:1);
          break;
        default: EXIT(2);
        }
      fprintf(f,"  %s ( SC, \"%s\", %s);\n", func, si->LOCON_NAME, val);
      break;

    case SIM_SLOPE:
      switch(si->UINPUT.INPUT_SLOPE.TYPE)
        {
        case SIM_SLOPE_SINGLE:
          fprintf(f,"  sim_AddSlope ( SC, \"%s\", %e, %e, '%c');\n",
                  si->LOCON_NAME,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TSTART,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TRISE,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TRANSITION==SIM_RISE?'u':'d'
                  );
          break;
        case SIM_SLOPE_PATTERN:
          fprintf(f,"  sim_AddWaveForm ( SC, \"%s\", %e, %e, %e, \"%s\");\n",
                  si->LOCON_NAME,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.TRISE,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.TFALL,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PERIOD,
                  si->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PATTERN
                  );
          break;
        default: EXIT(4);
        }
      break;
    case SIM_FUNC:
      fprintf(stderr,"can't drive pointed functions used in ...\n");
      break;
    default: EXIT(5);
    }
}

static void _simDriveMEAS(sim_measure *sm, FILE *f)
{
  char *name, *func;
  switch(sm->WHAT)
    {
    case SIM_MEASURE_CURRENT:
      func="sim_AddMeasureCurrent";
      break;
    case SIM_MEASURE_VOLTAGE:
      func="sim_AddMeasure";
      break;
    default: EXIT(2);
    }
  switch(sm->TYPE)
    {
    case SIM_MEASURE_LOCON: name=sm->WHERE.LOCON_NAME; break;
    case SIM_MEASURE_SIGNAL: name=sm->WHERE.SIGNAL_NAME; break;
    case SIM_MEASURE_SIGNAL_NODE: name=sm->WHERE.SIGNAL_NODE_NAME; break;
    default: EXIT(3);
    }
  fprintf(f,"  %s ( SC, \"%s\");\n", func, name);
}

void _simDrivePARAMS(sim_model *sm, FILE *f)
{
  char *tool;
  chain_list *cl;

  switch(sm->PARAMETER.TOOL)
    {
    case SIM_TOOL_NGSPICE: tool="NGSPICE"; break;
    case SIM_TOOL_LTSPICE: tool="LTSPICE"; break;
    case SIM_TOOL_HSPICE: tool="HSPICE"; break;
    case SIM_TOOL_MSPICE: tool="MSPICE"; break;
    case SIM_TOOL_TITAN: tool="TITAN"; break;
    case SIM_TOOL_TITAN7: tool="TITANv7"; break;
    case SIM_TOOL_ELDO: tool="ELDO"; break;
    case SIM_TOOL_DEFAULT: tool="DEFAULT"; break;
    default: EXIT(4);
    }

  fprintf(f,"  sim_SetSimulatorType ( SC, \"%s\");\n",tool);
  fprintf(f,"  sim_SetSimulationOutputFile ( SC, \"%s\");\n", sm->PARAMETER.TOOL_OUTFILE);
  fprintf(f,"  sim_SetSimulationCall ( SC, \"%s\");\n", sm->PARAMETER.TOOL_CMD);
  fprintf(f,"  sim_SetSimulationStep ( SC, %e);\n", sm->PARAMETER.TRANSIANT_STEP);
  fprintf(f,"  sim_SetSimulationTime ( SC, %e);\n", sm->PARAMETER.TRANSIANT_TMAX);

  fprintf(f,"  sim_SetSimulationTemp ( SC, %e);\n", sm->PARAMETER.TEMP);
  fprintf(f,"  sim_SetSimulationSupply ( SC, %e);\n", sm->PARAMETER.ALIM_VOLTAGE);
  fprintf(f,"  sim_SetSimulationSlope ( SC, %e);\n", sm->PARAMETER.SLOPE);
  fprintf(f,"  sim_SetSlopeVTH ( SC, %e, %e);\n", sm->PARAMETER.VTH_LOW, sm->PARAMETER.VTH_HIGH);
  fprintf(f,"  sim_SetDelayVTH ( SC, %e);\n", sm->PARAMETER.VTH);
  fprintf(f,"  sim_DriveTransistorAsInstance ( SC, '%c');\n", sm->PARAMETER.TRANSISTOR_AS_INSTANCE?'y':'n');

  if (sm->PARAMETER.TECHNOFILELIST!=NULL) fprintf(f,"\n");
  for (cl=sm->PARAMETER.TECHNOFILELIST; cl!=NULL; cl=cl->NEXT)
    fprintf(f,"  sim_AddSimulationTechnoFile ( SC, \"%s\");\n", (char *)cl->DATA);

  fprintf(f,"\n");
}

void _simDriveALIASES(sim_model *sm, FILE *f)
{
  sim_translate *st;

  if (sm->TRANSLATION!=NULL) fprintf(f,"\n");
  for (st=sm->TRANSLATION; st!=NULL; st=st->NEXT)
    {
      fprintf(f,"  sim_AddAlias ( SC, \"%s\", \"%s\");\n",st->signal, st->equiv);
    }
}
void sim_DriveContext(sim_model *sm, FILE *f)
{
  sim_ic *si;
  sim_input *sin;
  sim_measure *sme;

  fprintf(f,"void SimModel_%ld()\n{\n",sm->NUMBER);
  fprintf(f,"  SC = sim_CreateContext(sim_GetNetlist(\"%s\"));\n\n",sm->FIG->NAME);
  _simDrivePARAMS(sm, f);
  for (si=sm->LIC; si!=NULL; si=si->NEXT) _simDriveIC(si, f);
  for (sin=sm->LINPUT; sin!=NULL; sin=sin->NEXT) _simDriveDC(sin, f);
  for (sme=sm->LMEASURE; sme!=NULL; sme=sme->NEXT) _simDriveMEAS(sme, f);
  _simDriveALIASES(sm, f);
  fprintf(f,"}\n\n");
}
#endif

static sim_model *__SIM_CTX__;

void sim_set_ctx(sim_model *ctx)
{
  __SIM_CTX__=ctx;
}

sim_model *sim_get_ctx()
{
  return __SIM_CTX__;
}

// Renvoie le nom hierarchique du signal en fonction du simulateur utilise
char *sim_get_hier_signame (sim_model *model, char *signal)
{
  char buf[BUFSIZ];

  if (!model || !signal) return NULL;
  switch( sim_parameter_get_tool( model ) ) {
    case SIM_TOOL_ELDO:
    case SIM_TOOL_TITAN:
    case SIM_TOOL_TITAN7:
    case SIM_TOOL_HSPICE:
    case SIM_TOOL_MSPICE:
    case SIM_TOOL_NGSPICE:
      sprintf( buf, "x%s.%s", sim_model_get_lofig( model )->NAME,
                              signal);
      break;
    case SIM_TOOL_LTSPICE:
    case SIM_TOOL_SPICE:
      sprintf( buf, "%s:%s", sim_model_get_lofig( model )->NAME,
                             signal);
      break;
  }
  return namealloc (buf);
}

void sim_get_THR(sim_model *model, SIM_FLOAT *th_in, SIM_FLOAT *th_out, SIM_FLOAT *slopelow, SIM_FLOAT *slopehigh)
{
  if (th_in!=NULL) *th_in=sim_parameter_get_vss(model, 'i')+(sim_parameter_get_alim(model, 'i')-sim_parameter_get_vss(model, 'i'))*model->PARAMETER.VTHSTART;
  if (th_out!=NULL) *th_out=sim_parameter_get_vss(model, 'o')+(sim_parameter_get_alim(model, 'o')-sim_parameter_get_vss(model, 'o'))*model->PARAMETER.VTHEND;
  if (slopelow!=NULL) *slopelow = sim_parameter_get_vss(model, 'o')+(sim_parameter_get_alim(model, 'o')-sim_parameter_get_vss(model, 'o'))*model->PARAMETER.VTH_LOW;
  if (slopehigh!=NULL) *slopehigh = sim_parameter_get_vss(model, 'o')+(sim_parameter_get_alim(model, 'o')-sim_parameter_get_vss(model, 'o'))*model->PARAMETER.VTH_HIGH;
}


void sim_get_multivoltage_values (cone_list *cone, losig_list *ls, double *vssl, double *vssh, double *vddl, double *vddh)
{
    alim_list *power;
    if (cone!=NULL)
      power=cns_get_multivoltage(cone);
    else 
      power=cns_get_signal_multivoltage(ls);

    if(power){
        *vssl=power->VSSMIN;
        *vssh=power->VSSMAX;
        *vddl=power->VDDMIN;
        *vddh=power->VDDMAX;
    }else{
        *vssl=*vssl=0;
        *vddl=*vddh=V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    }
}

void sim_set_result_file_extension(char mode, int dc, char **var)
{
    switch (V_INT_TAB[__SIM_TOOL].VALUE) {
        case SIM_TOOL_ELDO:
            *var      = SIM_SPICEOUT;
            break;
        case SIM_TOOL_TITAN:
        case SIM_TOOL_TITAN7:
            if (!dc)
            {
              if (mode=='m')
                 *var      = sensitive_namealloc ("$.tr.t1.measure");
              else if (mode=='p')
                 *var      = sensitive_namealloc ("$.tr.t1.ppr");
            }
            else
            {
              if (mode=='m')
                 *var      = sensitive_namealloc ("$.dc.t1.measure");
              else if (mode=='p')
                 *var      = sensitive_namealloc ("$.dc.t1.ppr");
            }
            break;
        case SIM_TOOL_MSPICE:
        case SIM_TOOL_HSPICE:
            *var=SIM_SPICEOUT;
            break;
        case SIM_TOOL_LTSPICE:
            *var      = sensitive_namealloc ("$.log");
            break;
        case SIM_TOOL_NGSPICE:
        default:
            *var      = SIM_SPICEOUT;
    }
}
