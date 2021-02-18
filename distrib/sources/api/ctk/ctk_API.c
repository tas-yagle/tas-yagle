#include <stdio.h>
#include <string.h>
#ifdef Linux
#include <values.h>
#else
#include <float.h>
#endif
#include AVT_H
#include "api_communication.h"
#include GEN_H
#include STB_H
#include "api_common_structures.h"
//#include "gen_API.h"
//#define GENIUS_HEADER
#define API_USE_REAL_TYPES
#include "ctk_api_local.h"
#include "ctk_API.h"

#define STAT_NODE_TO_INDEX 0xfab70623

ctk_corresp sorteventparam [] = {
  { "SCORE",                  STB_CTK_SORT_SCORE_TOTAL      },
  { "SCORE_NOISE",            STB_CTK_SORT_SCORE_NOISE      },
  { "SCORE_ACTIVITY",         STB_CTK_SORT_SCORE_ACTIVITY   },
  { "SCORE_CTK",              STB_CTK_SORT_SCORE_CTK        },
  { "SCORE_INTERVAL",         STB_CTK_SORT_SCORE_INTERVAL   },
  { "NOISE_INSIDE_ALIM_MAX",  STB_CTK_SORT_INSIDE_ALIM_MAX  },
  { "NOISE_INSIDE_ALIM_REAL", STB_CTK_SORT_INSIDE_ALIM_REAL },
  { "NOISE_RISE_MAX",         STB_CTK_SORT_RISE_MAX         },
  { "NOISE_RISE_REAL",        STB_CTK_SORT_RISE_REAL        },
  { "NOISE_FALL_MAX",         STB_CTK_SORT_FALL_MAX         },
  { "NOISE_FALL_REAL",        STB_CTK_SORT_FALL_REAL        },
  { NULL,                     ' '                           }
} ;

ctk_corresp sortlineparam [] = {
  { "ABSOLUTE_DELAY",         STB_CTK_SORT_ABS_DELAY     },
  { "ABSOLUTE_MAX_DELAY",     STB_CTK_SORT_ABS_MAX_DELAY },
  { "ABSOLUTE_MIN_DELAY",     STB_CTK_SORT_ABS_MIN_DELAY },
  { "RELATIVE_DELAY",         STB_CTK_SORT_REL_DELAY     },
  { "RELATIVE_MAX_DELAY",     STB_CTK_SORT_REL_MAX_DELAY },
  { "RELATIVE_MIN_DELAY",     STB_CTK_SORT_REL_MIN_DELAY },
  { "ABSOLUTE_SLOPE",         STB_CTK_SORT_ABS_SLOPE     },
  { "ABSOLUTE_MAX_SLOPE",     STB_CTK_SORT_ABS_MAX_SLOPE },
  { "ABSOLUTE_MIN_SLOPE",     STB_CTK_SORT_ABS_MIN_SLOPE },
  { "RELATIVE_SLOPE",         STB_CTK_SORT_REL_SLOPE     },
  { "RELATIVE_MAX_SLOPE",     STB_CTK_SORT_REL_MAX_SLOPE },
  { "RELATIVE_MIN_SLOPE",     STB_CTK_SORT_REL_MIN_SLOPE },
  { NULL,                     ' '                        }
} ;

void ctk_LoadAggressionFile( stbfig_list *stbfig ) 
{
  if( !stbfig )
    return ;
    
  stb_ctk_parse_agression( stbfig );
}

void ctk_DriveStatCtk( stbfig_list *stbfig )
{
  stbfile      *report ;
  stb_ctk_stat *stat ;

  if( !stbfig )
    return ;
  
  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
//    fprintf( stderr, "No crosstalk statistics to drive in report.\n");
  }
  else {
    report = stb_info_open( stbfig );
    stb_ctk_drive_stat( report, stbfig, stat );
    stb_info_close( report );
  }
}

static void cleanstatindex(stb_ctk_stat *stat)
{
  int n;
  if( stat )
  {
    for( n=0 ; n<stat->NBELEM ; n++ ) {
       stat->TAB[n].NODE->USER=testanddelptype(stat->TAB[n].NODE->USER, STAT_NODE_TO_INDEX);
    }
  }
}

static void setstatindex(stb_ctk_stat *stat)
{
 int n;
 if( stat )
  {
    for( n=0 ; n<stat->NBELEM ; n++ ) {
       stat->TAB[n].NODE->USER=addptype(stat->TAB[n].NODE->USER, STAT_NODE_TO_INDEX, (void *)(long)(n+1));
    }
  } 
}

void ctk_BuildCtkStat( stbfig_list *stbfig )
{
  stb_ctk_stat *stat ;
  int n;
  if( !stbfig )
    return ;
  stat = stb_ctk_get_stat( stbfig );
  cleanstatindex(stat);
  if( stat )
    stb_ctk_clean_stat( stbfig );
  ttv_init_stm(stbfig->FIG);
  CtkMutexFree( stbfig->FIG );
  CtkMutexInit( stbfig->FIG );
  stb_ctk_fill_stat( stbfig, 1 );
  stat = stb_ctk_get_stat( stbfig );
  setstatindex(stat);
}

ptype_list* ctk_GetStatNodeProperty( stbfig_list *stbfig, int index, char *property )
{
  stb_ctk_stat *stat ;
  char          buf[128];
  
  if( !stbfig || !property )
    return addptype (NULL, TYPE_CHAR, strdup ("error_null_stabilityfigure_or_property_name"));

  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
    //fprintf( stderr, "No crosstalk statistics to drive in report.\n");
    return addptype (NULL, TYPE_CHAR, strdup ("No crosstalk statistics to drive in report"));
  }

  if( index < 1 || index > stat->NBELEM ) {
    avt_errmsg(CTK_API_ERRMSG, "002", AVT_WARNING);
    //fprintf( stderr, "No crosstalk statistics available for this event.\n" );
    return addptype (NULL, TYPE_CHAR, strdup ("No crosstalk statistics available for this event"));
  }

  index-- ;

  if( !strcasecmp( property, "SCORE_NOISE" ) ) {
    return addptype (NULL, TYPE_INT, (void*)(long)stb_ctk_get_score_noise( &stat->TAB[index] ) );
  }

  if( !strcasecmp( property, "SCORE_INTERVAL" ) ) {
    return addptype (NULL, TYPE_INT, (void*)(long)stb_ctk_get_score_interval( &stat->TAB[index] ) );
  }

  if( !strcasecmp( property, "SCORE_CTK" ) ) {
    return addptype (NULL, TYPE_INT, (void*)(long)stb_ctk_get_score_ctk( &stat->TAB[index] ) );
  }

  if( !strcasecmp( property, "SCORE_ACTIVITY" ) ) {
    return addptype (NULL, TYPE_INT, (void*)(long)stb_ctk_get_score_activity( &stat->TAB[index] ) );
  }

  if( !strcasecmp( property, "SCORE" ) ) {
    return addptype (NULL, TYPE_INT, (void*)(long)stb_ctk_get_score_total( &stat->TAB[index] ) );
  }

  if( !strcasecmp( property, "NOISE_RISE_REAL" ) ) {
    sprintf( buf, "%g", stat->TAB[index].RISE_PEAK_REAL );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "NOISE_FALL_REAL" ) ) {
    sprintf( buf, "%g", stat->TAB[index].FALL_PEAK_REAL );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "NOISE_RISE_MAX" ) ) {
    sprintf( buf, "%g", stat->TAB[index].RISE_PEAK_MAX );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "NOISE_FALL_MAX" ) ) {
    sprintf( buf, "%g", stat->TAB[index].FALL_PEAK_MAX );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "CAPA_GROUND" ) ) {
    sprintf( buf, "%g", stat->TAB[index].CNET_GND );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "CAPA_CTK" ) ) {
    sprintf( buf, "%g", stat->TAB[index].CNET_CC );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "CAPA_CGATE" ) ) {
    sprintf( buf, "%g", stat->TAB[index].CGATE );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "VOLTAGE_THRESHOLD" ) ) {
    sprintf( buf, "%g", stat->TAB[index].VTH );
    return addptype (NULL, TYPE_CHAR, strdup(buf) );
  }
  
  if( !strcasecmp( property, "EVENT" ) ) {
    return addptype (NULL, TYPE_TIMING_EVENT, stat->TAB[index].NODE );
  }

  avt_errmsg(CTK_API_ERRMSG, "003", AVT_ERROR, property);
//  sprintf( buf, "error : unknown property %s\n", property ); 
  fprintf( stderr, "%s\n", buf ); 
  return addptype (NULL, TYPE_CHAR, strdup (buf));

}

chain_list* ctk_GetAggressorList( stbfig_list *stbfig, ttvevent_list *event )
{
  stb_ctk_detail_agr_list *chain ;
  stb_ctk_detail          *ctkdetail ;
  chain_list              *head ;
  ctk_aggressor           *aggressor ;

  if( !stbfig || !event ) 
    return NULL ;

  ctkdetail = stb_ctk_get_detail( stbfig, event );
  head      = NULL ;
 
  if (ctkdetail!=NULL) {
    for( chain = ctkdetail->AGRLIST ; chain ; chain = chain->NEXT ) {
      aggressor = mbkalloc( sizeof( ctk_aggressor ) );
      aggressor->TTVSIG              = chain->TTVAGR ;
      aggressor->NETNAME             = strdup(chain->NETNAME) ;
      aggressor->ACT_WORST           = chain->ACT_WORST ;
      aggressor->ACT_BEST            = chain->ACT_BEST ;
      aggressor->ACT_MUTEX_WORST     = chain->ACT_MUTEX_WORST ;
      aggressor->ACT_MUTEX_BEST      = chain->ACT_MUTEX_BEST ;
      aggressor->NOISE_RISE_PEAK     = chain->NOISE_RISE_PEAK ;
      aggressor->NOISE_RISE_EXCLUDED = chain->NOISE_RISE_EXCLUDED ;
      aggressor->NOISE_FALL_PEAK     = chain->NOISE_FALL_PEAK ;
      aggressor->NOISE_FALL_EXCLUDED = chain->NOISE_FALL_EXCLUDED ;
      aggressor->CC                  = chain->CC ;
      head = addchain( head, aggressor );
    }
    
    stb_ctk_free_detail( stbfig, ctkdetail );
  }
  return head ;
}
static ptype_list *fitdouble(double val)
{
  ptype_list *pt;
  pt=addptype(NULL, TYPE_DOUBLE, NULL);
  *(float *)&pt->DATA=(float)val;
  return pt;
}

ptype_list* ctk_GetAggressorProperty( ctk_aggressor *agressor, char *property )
{
  char buf[128];

  if( !agressor || !property ) {
    return NULL ;
  }
  
  if( !strcasecmp( property, "SIGNAL" ) ) {
    return addptype( NULL, TYPE_TIMING_SIGNAL, agressor->TTVSIG );
  }
  
  if( !strcasecmp( property, "NETNAME" ) ) {
    return addptype( NULL, TYPE_CHAR, strdup(agressor->NETNAME) );
  }

  if( !strcasecmp( property, "CAPA_CTK" ) ) {
  //  sprintf( buf, "%g", agressor->CC );
    return fitdouble(agressor->CC*1e-12); //addptype( NULL, TYPE_CHAR, strdup(buf) );
  }

  if( !strcasecmp( property, "DELAYBESTAGR" ) ) {
    if( agressor->ACT_BEST  )
      sprintf( buf, "yes" );
    else {
      if( agressor->ACT_MUTEX_BEST ) 
        sprintf( buf, "excluded" );
      else
        sprintf( buf, "no" );
    }
    return addptype( NULL, TYPE_CHAR, strdup(buf) );
  }
  
  if( !strcasecmp( property, "DELAYWORSTAGR" ) ) {
    if( agressor->ACT_WORST  )
      sprintf( buf, "yes" );
    else {
      if( agressor->ACT_MUTEX_WORST ) 
        sprintf( buf, "excluded" );
      else
        sprintf( buf, "no" );
    }
    return addptype( NULL, TYPE_CHAR, strdup(buf) );
  }
  
  if( !strcasecmp( property, "NOISERISE" ) ) {
    if (agressor->NOISE_RISE_PEAK ) 
      sprintf( buf, "yes" );
    else {
      if( agressor->NOISE_RISE_EXCLUDED ) 
        sprintf( buf, "excluded" );
      else
        sprintf( buf, "no" );
    }
    return addptype( NULL, TYPE_CHAR, strdup(buf) );
  }
  
  if( !strcasecmp( property, "NOISEFALL" ) ) {
    if (agressor->NOISE_FALL_PEAK ) 
      sprintf( buf, "yes" );
    else {
      if( agressor->NOISE_FALL_EXCLUDED ) 
        sprintf( buf, "excluded" );
      else
        sprintf( buf, "no" );
    }
    return addptype( NULL, TYPE_CHAR, strdup(buf) );
  }
  
  fprintf( stderr, "error : unknown property %s\n", property ); 
  return NULL ;
}

void ctk_FreeAggressorList( chain_list *head )
{
  chain_list    *chain ;
  ctk_aggressor *aggressor ;
  for( chain = head ; chain ; chain = chain->NEXT ) {
    aggressor = (ctk_aggressor*)chain->DATA ;
    free( aggressor->NETNAME );
    mbkfree( chain->DATA );
  }
}

void ctk_SortCtkStatNode( stbfig_list *stbfig, char *criterion )
{
  stb_ctk_stat *stat ;
  int           n ;

  if( !stbfig || !criterion  )
    return ;

  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
//    fprintf( stderr, "No crosstalk statistics available.\n");
    return ;
  }

  cleanstatindex(stat);
  for( n = 0 ;
       sorteventparam[n].TOKEN && strcasecmp( sorteventparam[n].TOKEN, criterion ) ;
       n++ 
     );

  if( !sorteventparam[n].TOKEN ) {
    fprintf( stderr, "criterion %s not recognized.\n", criterion );
    return ;
  }

  stb_ctk_sort_stat( stat, sorteventparam[n].VALUE );
  setstatindex(stat);
}

int ctk_GetNumberOfCtkStatNode( stbfig_list *stbfig )
{
  stb_ctk_stat *stat ;

  if( !stbfig )
    return -1 ;

  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    fprintf( stderr, "No crosstalk statistics available.\n");
    return -1 ;
  }

  return stat->NBELEM ;
}

int ctk_GetCtkStatNodeFromEvent( stbfig_list *stbfig, ttvevent_list *event )
{
  stb_ctk_stat *stat ;
  int           n ;
  ptype_list *pt;
  
  if( !stbfig || !event )
    return -1 ;
    
  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
    //fprintf( stderr, "No crosstalk statistics available.\n");
    return -1 ;
  }

  if ((pt=getptype(event->USER, STAT_NODE_TO_INDEX))!=NULL) return n=(int)(long)pt->DATA;
  else {         
    for( n=0 ; n<stat->NBELEM ; n++ ) {
      if( stat->TAB[n].NODE == event )
        break ;
    }
  }
  if( n>=stat->NBELEM )
    return -1 ;
 
  return n+1 ;
}

void ctk_SortCtkStatLine( stbfig_list *stbfig, char *criterion )
{
  stb_ctk_stat *stat ;
  int           n ;

  if( !stbfig || !criterion  )
    return ;

  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
    //fprintf( stderr, "No crosstalk statistics available.\n");
    return ;
  }

  cleanstatindex(stat);
  for( n = 0 ;
       sortlineparam[n].TOKEN && strcasecmp( sortlineparam[n].TOKEN, criterion ) ;
       n++ 
     );

  if( !sortlineparam[n].TOKEN ) {
    fprintf( stderr, "criterion %s not recognized.\n", criterion );
    return ;
  }

  stb_ctk_sort_delay( stat, sortlineparam[n].VALUE, 0.0 );
  setstatindex(stat);
}

int ctk_GetNumberOfCtkStatLine( stbfig_list *stbfig )
{
  stb_ctk_stat *stat ;

  if( !stbfig  )
    return -1 ;

  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
    //fprintf( stderr, "No crosstalk statistics available.\n");
    return -1 ;
  }
  
  return stat->NBDELEM ;
}

ptype_list *ctk_GetStatLineProperty( stbfig_list *stbfig, int index, char *property )
{
  stb_ctk_stat *stat ;
  char          buf[128] ;
  
  if( !stbfig || !property )
    return NULL ;

  stat = stb_ctk_get_stat( stbfig );
  if( !stat ) {
    avt_errmsg(CTK_API_ERRMSG, "001", AVT_WARNING);
    //fprintf( stderr, "No crosstalk statistics to drive in report.\n");
    return NULL ;
  }

  if( index < 1 || index > stat->NBELEM ) {
    avt_errmsg(CTK_API_ERRMSG, "002", AVT_WARNING);
//    fprintf( stderr, "No crosstalk statistics available for this event.\n" );
    return NULL ;
  }

  index-- ;

  if( ! strcasecmp( property, "START_NODE" ) ) {
    return addptype( NULL, TYPE_TIMING_EVENT, stat->TABD[index].LINE->NODE ) ;
  }

  if( ! strcasecmp( property, "END_NODE" ) ) {
    return addptype( NULL, TYPE_TIMING_EVENT, stat->TABD[index].LINE->ROOT ) ;
  }

  if( ! strcasecmp( property, "DELAY_MAX_STA" ) ) {
    //sprintf( buf, "%g", ((float)stat->TABD[index].LINE->VALMAX)/TTV_UNIT );
    return fitdouble(((float)stat->TABD[index].LINE->VALMAX)*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "DELAY_MIN_STA" ) ) {
//    sprintf( buf, "%g", ((float)stat->TABD[index].LINE->VALMIN)/TTV_UNIT );
    return fitdouble(((float)stat->TABD[index].LINE->VALMIN)*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "SLOPE_MAX_STA" ) ) {
//    sprintf( buf, "%g", ((float)stat->TABD[index].LINE->FMAX)/TTV_UNIT );
    return fitdouble(((float)stat->TABD[index].LINE->FMAX)*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "SLOPE_MIN_STA" ) ) {
//    sprintf( buf, "%g", ((float)stat->TABD[index].LINE->FMIN)/TTV_UNIT );
    return fitdouble(((float)stat->TABD[index].LINE->FMIN)*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "DELAY_MAX_CTK" ) ) {
//    sprintf( buf, "%g", ((float)ttv_getdelaymax(stat->TABD[index].LINE))/TTV_UNIT );
    return fitdouble(((float)ttv_getdelaymax(stat->TABD[index].LINE))*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "DELAY_MIN_CTK" ) ) {
//    sprintf( buf, "%g", ((float)ttv_getdelaymin(stat->TABD[index].LINE))/TTV_UNIT );
    return fitdouble(((float)ttv_getdelaymin(stat->TABD[index].LINE))*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "SLOPE_MAX_CTK" ) ) {
//    sprintf( buf, "%g", ((float)ttv_getslopemax(stat->TABD[index].LINE))/TTV_UNIT );
    return fitdouble(((float)ttv_getslopemax(stat->TABD[index].LINE))*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  if( ! strcasecmp( property, "SLOPE_MIN_CTK" ) ) {
//    sprintf( buf, "%g", ((float)ttv_getslopemin(stat->TABD[index].LINE))/TTV_UNIT );
    return fitdouble(((float)ttv_getslopemin(stat->TABD[index].LINE))*1e-12/TTV_UNIT); //addptype( NULL, TYPE_CHAR, strdup(buf) ) ;
  }

  avt_errmsg(CTK_API_ERRMSG, "003", AVT_ERROR, property);
  return addptype (NULL, TYPE_CHAR, strdup ("error_unknown_property"));
//  fprintf( stderr, "error : unknown property %s\n", property ); 
//  return NULL ;
}
