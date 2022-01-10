#include <stdlib.h>
#include "avt_lib.h"


char *ANNOT_T_D=NULL, *ANNOT_T_S=NULL, *ANNOT_T_G=NULL, *ANNOT_T_B=NULL;
char *ANNOT_R_POS=NULL, *ANNOT_R_NEG=NULL;
char *ANNOT_D_POS=NULL, *ANNOT_D_NEG=NULL;
char *ANNOT_C_POS=NULL, *ANNOT_C_NEG=NULL;
unsigned long int RCN_CACHE_SIZE=10*1024*1024;

int avtAnnotationDeviceConnectorSetting_init(const char *var, const char *val, char *result)
{
  char * *items[]={&ANNOT_T_S, &ANNOT_T_G, &ANNOT_T_D, &ANNOT_T_B,
                  &ANNOT_R_POS, &ANNOT_R_NEG,
                  &ANNOT_C_POS, &ANNOT_C_NEG,
                  &ANNOT_D_POS, &ANNOT_D_NEG};
  char buf[1024];
  char *c, *tok;
  unsigned int i=0;
  
  strcpy(buf, val);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL && i<sizeof(items)/sizeof(*items))
    {
      if (*items[i]!=NULL) free(*items[i]);
      if (strcmp(tok,"-")!=0) *items[i]=strdup(tok);
      tok=strtok_r(NULL, " ", &c);
      i++;
    }
    
  if (i!=sizeof(items)/sizeof(*items))
  {
    avt_errmsg(AVT_ERRMSG, "041", AVT_ERROR, val, var);
    return 0;
  }

  return 1;
}

// -----------------------------------------
char env_SIMUINV=' ';
char *SIMUINVCONENAME=NULL, *SIMUINV_PREFIX=NULL;
int tpiv_inverter_config_reverse=1;
float tpiv_inverter_config_t0r=0, tpiv_inverter_config_t0f=0;
float tpiv_inverter_config_tmax=100e-12;

int tasSimulateInverter_init(const char *var, const char *env, char *result)
{
  const char *tmp;
  const char *e = env;
  int len = strlen(env); //largest it can be
  if (SIMUINV_PREFIX!=NULL) free(SIMUINV_PREFIX);
  if (SIMUINVCONENAME!=NULL) free(SIMUINVCONENAME);
  
  SIMUINV_PREFIX=NULL;
  
  if (!env)
  {
    env_SIMUINV = ' ';
  }
  else {
    if (*e == 'S')
      env_SIMUINV = 'S';
    else
      env_SIMUINV = 'T';
    e++;
    while (*e == ' ' && *e != '\0')
      e++;
    tmp=e;
    while (*e != ' ' && *e != '\0')
      e++;
    if (*e==' ') {
      len = e-env;
      e++;
    }
    if (*tmp != '\0')
      SIMUINVCONENAME = strndup(tmp, len);
    while (*e == ' ' && *e != '\0')
      e++;
    tmp=e;
    if (*tmp!='\0') SIMUINV_PREFIX = strdup(tmp);


    printf
        ("*** inverter simulator for cone %s with model '%c' ***\n",
         SIMUINVCONENAME, env_SIMUINV);
  }
  return 1;
}

int tpiv_inverter_config_init(const char *var, const char *config, char *result)
{
  char  *tmp ;
  char  *tok ;
  int    error ;
  char  *eq ;
  double value ;
  char  *endval ;
  char   token[256] ;

  /* default config */
  tpiv_inverter_config_reverse = 1 ;
  tpiv_inverter_config_t0r=tpiv_inverter_config_t0f= 0.0 ;
  
  if( !config )
    return 1;

  tmp = alloca( sizeof( char ) * ( strlen( config )+1 ) );
  strcpy( tmp, config );

  error = 0 ;
  tok = strtok( tmp, " "  );
  while( tok ) {
  
    eq=strchr( tok, '=' );
    if( !eq ) { error=1 ; break ; }
    strncpy( token, tok, eq-tok ) ;
    token[ eq-tok ] = '\0' ;
    value = strtod( eq+1, &endval );
    if( ! ( *endval == ' ' || *endval == '\0' )  ) { error=1 ; break ; }
   
    error = 1 ;
    if( !strcmp( token, "reverse" ) ) { tpiv_inverter_config_reverse = mbk_long_round(value) ; error = 0 ; }
    if( !strcmp( token, "t0"      ) ) { tpiv_inverter_config_t0r=tpiv_inverter_config_t0f=value ; error = 0 ; }
    if( !strcmp( token, "t0r"      ) ) { tpiv_inverter_config_t0r=value ; error = 0 ; }
    if( !strcmp( token, "t0f"      ) ) { tpiv_inverter_config_t0f=value ; error = 0 ; }
    if( !strcmp( token, "tmax"      ) ) { tpiv_inverter_config_tmax=value ; error = 0 ; }
    if( error )
      break ;

    tok = strtok( NULL, " "  );
  }

  if( error ) {
    printf( "bad configuration for %s\n", tok );
    return 0;
  }
  return 1;
}


int yagSimpleLatchDetection_init(const char *var, const char *val, char *result)
{ 
  char buf[1024];
  char *c, *tok;
  
  strcpy(buf, val);
  tok=strtok_r(buf, "+", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"latch")!=0 && strcasecmp(tok,"memsym")!=0 && strcasecmp(tok,"levelhold")!=0 && strcasecmp(tok,"strictlevelhold")!=0)
       {
         avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, tok, var);
         return 0;
      }
      tok=strtok_r(NULL, "+", &c);
    }
  return 1;
}

int yagDetectClockGating_init(const char *var, const char *val, char *result)
{ 
  char buf[1024];
  char *c, *tok;
  
  strcpy(buf, val);
  tok=strtok_r(buf, "+", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"yes")!=0 && strcasecmp(tok,"no")!=0 && strcasecmp(tok,"check")!=0 && strcasecmp(tok,"filter")!=0)
       {
         avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, tok, var);
         return 0;
      }
      tok=strtok_r(NULL, "+", &c);
    }
  return 1;
}


// -----------------------------------------
int TTV_MaxPathPeriodPrecharge=0;
// mask: 1=latch, 2=precharge, 4=filter
int TTV_IgnoreMaxFilter=0;

int avtTransparentPrecharge_init(const char *var, const char *val, int *result)
{ 
  TTV_IgnoreMaxFilter&=~2;
  if (!strcasecmp (val, "yes")) *result=1, TTV_MaxPathPeriodPrecharge=1;
  else if (!strcasecmp (val, "no")) *result=0, TTV_MaxPathPeriodPrecharge=0;
  else if (!strcasecmp (val, "unfiltered")) *result=1, TTV_MaxPathPeriodPrecharge=1, TTV_IgnoreMaxFilter|=2;
  else {
      avt_errmsg (AVT_ERRMSG, "040", AVT_WARNING, val, var);
      return 0;
  }
  return 1;
}

// -----------------------------------------

int ttvIgnoreMaxFilter_init(const char *var, const char *val, char *result)
{
  char buf[1024];
  char *c, *tok;
  
  TTV_IgnoreMaxFilter=0;
  strcpy(buf, val);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"latch")==0) TTV_IgnoreMaxFilter|=1;
      else if (strcasecmp(tok,"precharge")==0) TTV_IgnoreMaxFilter|=2;
      else if (strcasecmp(tok,"filter")==0) TTV_IgnoreMaxFilter|=4;
      else 
      {
         avt_errmsg (AVT_ERRMSG, "040", AVT_WARNING, var, tok);
         return 0;
      }
      tok=strtok_r(NULL, " ", &c);
    }
  return 1;
}

int avtParasiticCacheSize_init(const char *var, const char *val, char *result)
{
  long oldval=RCN_CACHE_SIZE;
  char *ptend;
  
  RCN_CACHE_SIZE = strtol( val, &ptend, 10 );

  if( *ptend != '\0' ) {
    if( strcasecmp( ptend, "kb" )==0 )
      RCN_CACHE_SIZE = RCN_CACHE_SIZE * 1024;
    else {
      if( strcasecmp( ptend, "mb" )==0 )
        RCN_CACHE_SIZE = RCN_CACHE_SIZE * 1048576;
      else {
        if( strcasecmp( ptend, "gb" )==0 )
          RCN_CACHE_SIZE = RCN_CACHE_SIZE * 1073741824;
        else {
          avt_errmsg (AVT_ERRMSG, "040", AVT_WARNING, var, val);
          RCN_CACHE_SIZE = oldval;
        }
      }
    }
  }
  return 1;
}

const struct {
  char *unit;
  double mult;
} time_unit_tab[]={{"", 1},
              {"f", 1e-15},
              {"p", 1e-12},
              {"n", 1e-9},
              {"u", 1e-6},
              {"m", 1e-3},
              {"fs", 1e-15},
              {"ps", 1e-12},
              {"ns", 1e-9},
              {"us", 1e-6},
              {"ms", 1e-3},
              {"s", 1}};
const struct {
  char *unit;
  double mult;
} capa_unit_tab[]={{"", 1},
              {"ff", 1e-15},
              {"pf", 1e-12},
              {"f", 1e-15},
              {"p", 1e-12},
              {"n", 1e-9},
              {"u", 1e-6},
              {"m", 1e-3},
              {"nf", 1e-9},
              {"uf", 1e-6},
              {"mf", 1e-3},
              {"f", 1}};
const struct {
  char *unit;
  double mult;
} mem_unit_tab[]={{"", 1},
              {"b", 1},
              {"kb", 1024},
              {"mb", 1024*1024},
              {"gb", 1024*1024*1024}};

const struct {
  char *unit;
  double mult;
} voltage_unit_tab[]={{"", 1},
              {"fv", 1e-15},
              {"pv", 1e-12},
              {"f", 1e-15},
              {"p", 1e-12},
              {"n", 1e-9},
              {"u", 1e-6},
              {"m", 1e-3},
              {"nv", 1e-9},
              {"uv", 1e-6},
              {"mv", 1e-3},
              {"v", 1}};

double avt_parse_unit(const char *str, char type)
{
  char    *nxt;
  unsigned int i;
  double val;
  if (type=='t')
  {

    val=strtod(str, &nxt);
    if (*nxt!=';')
    {
      for (i=0; i<sizeof(time_unit_tab)/sizeof(*time_unit_tab); i++)
        if (strcasecmp(time_unit_tab[i].unit, nxt)==0) break;

      if (i>=sizeof(time_unit_tab)/sizeof(*time_unit_tab))
      {
        avt_errmsg(AVT_ERRMSG, "049", AVT_FATAL, str);
      }
      else val*=time_unit_tab[i].mult;
    }
    else
      avt_errmsg(AVT_ERRMSG, "049", AVT_WARNING, str);
  } 
  else if (type=='c')
  {
    val=strtod(str, &nxt);
    if (*nxt!=';')
    {
      for (i=0; i<sizeof(capa_unit_tab)/sizeof(*capa_unit_tab); i++)
        if (strcasecmp(capa_unit_tab[i].unit, nxt)==0) break;

      if (i>=sizeof(capa_unit_tab)/sizeof(*capa_unit_tab))
      {
        avt_errmsg(AVT_API_ERRMSG, "006", AVT_FATAL, str);
      }
      else val*=capa_unit_tab[i].mult;
    }
    else
      avt_errmsg(AVT_ERRMSG, "049", AVT_WARNING, str);
  }
  else if (type=='m')
  {
    val=strtod(str, &nxt);
    if (*nxt!=';')
    {
      for (i=0; i<sizeof(mem_unit_tab)/sizeof(*mem_unit_tab); i++)
        if (strcasecmp(mem_unit_tab[i].unit, nxt)==0) break;

      if (i>=sizeof(mem_unit_tab)/sizeof(*mem_unit_tab))
      {
        avt_errmsg(AVT_API_ERRMSG, "006", AVT_FATAL, str);
      }
      else val*=mem_unit_tab[i].mult;
    }
    else
      avt_errmsg(AVT_ERRMSG, "049", AVT_WARNING, str);
  }
  else if (type=='v')
  {
    val=strtod(str, &nxt);
    if (*nxt!=';')
    {
      for (i=0; i<sizeof(voltage_unit_tab)/sizeof(*voltage_unit_tab); i++)
        if (strcasecmp(voltage_unit_tab[i].unit, nxt)==0) break;

      if (i>=sizeof(voltage_unit_tab)/sizeof(*voltage_unit_tab))
      {
        avt_errmsg(AVT_API_ERRMSG, "006", AVT_FATAL, str);
      }
      else val*=voltage_unit_tab[i].mult;
    }
    else
      avt_errmsg(AVT_ERRMSG, "049", AVT_WARNING, str);
  }
  else
    avt_errmsg(AVT_API_ERRMSG, "006", AVT_FATAL, str);

  return val;
}

int avt_parse_time (const char *var, const char *val, float *result) 
{
  *result=avt_parse_unit(val, 't');
  return 1;
}

int avt_parse_capa (const char *var, const char *val, float *result) 
{
  *result=avt_parse_unit(val, 'c');
  return 1;
}
int avt_parse_mem (const char *var, const char *val, float *result) 
{
  *result=avt_parse_unit(val, 'm');
  return 1;
}

void avt_init_model(chain_list **list, const char *str)
{
  char buf[1024];
  char *tmp, *pttok;
  freechain(*list); *list=NULL;
  if (namealloc_ok() && str!=NULL && strlen(str)>0) 
  {
    strcpy(buf, str);
    *list = addchain(*list, namealloc(strtok_r(buf, ":", &tmp)));
    while ((pttok = strtok_r(NULL, ":", &tmp)))
      *list = addchain(*list, namealloc(pttok));
  }
}

int avt_init_model_tn (const char *var, const char *val, char *result)
{
  avt_init_model(&TNMOS, val);
  result=NULL; var=NULL;
  return 1;
}
int avt_init_model_tp (const char *var, const char *val, char *result)
{
  avt_init_model(&TPMOS, val);
  result=NULL; var=NULL;
  return 1;
}
int avt_init_model_dn (const char *var, const char *val, char *result)
{
  avt_init_model(&DNMOS, val);
  result=NULL; var=NULL;
  return 1;
}
int avt_init_model_dp (const char *var, const char *val, char *result)
{
  avt_init_model(&DPMOS, val);
  result=NULL; var=NULL;
  return 1;
}

