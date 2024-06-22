/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_ctk_noise.c                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

#include "stb_util.h"
#include "stb_init.h"
#include "stb_error.h"
#include "stb_transfer.h"
#include "stb_relaxation.h"
#include "stb_overlap.h"
#include "stb_ctk.h"
#include "stb_ctk_noise.h"
#include "stb_ctk_score.h"
#include "stb_ctk_mutex.h"
#include "stb_ctk_debug.h"

RCXFLOAT STB_NOISE_DEFAULT_RESI=1000.0;
char STB_CTK_NOISE_COMPARE = STB_CTK_SORT_INSIDE_ALIM_REAL ;

chain_list* stb_ctk_noise_event_list( stbfig_list       *stbfig,
                                      ttvevent_list     *evtvic,
                                      chain_list        *headevent,
                                      rcx_signal_noise  *tab,
                                      char               calcul,
                                      RCXFLOAT          *v
                                    )
{
  losig_list      *losig;
  ttvevent_list   *event;
  rcxparam        *param;
  int              index;
  chain_list      *chain;
  chain_list      *nomutex;
  RCXFLOAT         v_max;
  sortedmutex     *mutex ;
  sortedmutexlist *ml;
  chain_list      *excluded;
  ttvevent_list   *event_max;

  *v  = 0.0;
  excluded = NULL ;
 
  mutex = stb_ctk_sort_by_mutex( stbfig->FIG, evtvic, headevent );
  if( mutex )
    nomutex = mutex->NOMUTEX ;
  else
    nomutex = headevent ;
    
  for( chain = nomutex ; chain ; chain = chain->NEXT ) {
  
    event = (ttvevent_list*)chain->DATA;
    param = stb_getrcxparamfromevent( event );
    losig = param->SIGNAL;
    
    index = rcx_gettabagrindex( losig );
    if( index < 0 ) stb_ctk_error( 2, "internal error : negative index.\n" );
    
    if( calcul == TTV_NODE_UP )
      *v = *v + tab[index].NOISE_OVER  ;
    else
      *v = *v + tab[index].NOISE_UNDER ;
  }

  if( mutex ) {
  
    for( ml = mutex->LIST ; ml ; ml = ml->NEXT ) {
    
      v_max = 0.0 ;
      event_max = NULL ;
      
      for( chain = ml->LIST ; chain ; chain = chain->NEXT ) {
      
        event = (ttvevent_list*)chain->DATA;
        param = stb_getrcxparamfromevent( event );
        losig = param->SIGNAL;
        
        index = rcx_gettabagrindex( losig );
        if( index < 0 ) 
                     stb_ctk_error( 2, "internal error : negative index.\n" );
     
        if( calcul == TTV_NODE_UP ) {
          if( tab[index].NOISE_OVER > v_max ) {
            v_max = tab[index].NOISE_OVER ;
            event_max = event ;
          }
        }
        else {
          if( tab[index].NOISE_UNDER > v_max ) {
            v_max = tab[index].NOISE_UNDER ;
            event_max = event ;
          }
        }
      }
      
      for( chain = ml->LIST ; chain ; chain = chain->NEXT ) {
        event = (ttvevent_list*)chain->DATA;
        if( event != event_max )
          excluded = addchain( excluded, event );
      }

      *v  = *v + v_max ;
    }

    stb_ctk_free_sortedmutex( stbfig->FIG, mutex );
  }
  stbfig=NULL; // unused

  return excluded ;
}

timing_model* stb_ctk_noise_get_model( ttvfig_list *ttvfig, 
                                       ttvevent_list *event,
                                       long level,
                                       long type
                                     )
{
  ttvline_list  *line;
  timing_model  *model;

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE) {
  
    ttv_expfigsig( ttvfig, 
                   event->ROOT, 
                   level, 
                   ttvfig->INFO->LEVEL, 
                   TTV_STS_CLS_FED|TTV_STS_DUAL_FED, 
                   TTV_FILE_DTX
                 );
    line = event->INLINE ;
  }
  else {
    ttv_expfigsig( ttvfig,
                   event->ROOT, 
                   level, 
                   ttvfig->INFO->LEVEL, 
                   TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, 
                   TTV_FILE_TTX
                 );
    line = event->INPATH ;
  }

  for( line = line ; line ; line = line->NEXT ) {
    if( !ttv_islinelevel( ttvfig, line, level ) )
      continue;
    model = stm_getmodel( ttvfig->INFO->FIGNAME, line->MDMAX );
    if( model ) return model;
    model = stm_getmodel( ttvfig->INFO->FIGNAME, line->MDMIN );
    if( model ) return model;
  }
  return NULL;
}

void stb_ctk_noise_node_gaplist( stbfig_list   *stbfig, 
                                 ttvevent_list *ttv_victime,
                                 lofig_list    *figvic,
                                 losig_list    *lo_victime,
                                 stbgap_list   *gaplist,
                                 chain_list    *alwaysactif,
                                 long           level,
                                 long           type,
                                 long           calcul,
                                 char         **mod,
                                 float         *max,
                                 float         *real
                               )
{
  timing_model     *model;
  noise_scr        *modscr;
  rcx_signal_noise *agresseurs;
  RCXFLOAT          v;
  RCXFLOAT          vgap;
  RCXFLOAT          valw;
  RCXFLOAT          vmax;
  int               i;
  stbgap_list      *gap;
  stbgap_list      *bestgap;
  int               nbagr;
  char             *modname;
  rcxparam         *param;
  chain_list       *excluded;
  chain_list       *bestexcluded;
  RCXFLOAT          resi;
  chain_list       *chain;
  int               n1, n2;

  modname = "none";
  
  model = stb_ctk_noise_get_model( stbfig->FIG, ttv_victime, level, type );

  switch( stm_noise_getmodeltype( model ) ) {
  
  case STM_NOISE_SCR :
  default :
  
    if( stm_noise_getmodeltype( model ) == STM_NOISE_SCR ) {
      modname = "scr ";
      modscr     = stm_noise_getmodel_scr( model );
      resi = stm_noise_scr_resi( modscr );
    }
    else {
      modname = "cc  ";
      resi = STB_NOISE_DEFAULT_RESI ;
    }
    
    agresseurs = rcx_noise_scr_detail( figvic,
                                       lo_victime, 
                                       resi,
                                       stm_mod_vdd( model ),
                                       &nbagr
                                     );
    vmax  = 0.0;
    for( i=0 ; i < nbagr ; i++ ) {
      if( calcul == TTV_NODE_UP )
        vmax = vmax  + agresseurs[i].NOISE_OVER;
      else
        vmax = vmax + agresseurs[i].NOISE_UNDER;
    }

    excluded = stb_ctk_noise_event_list( stbfig,
                                         ttv_victime,
                                         alwaysactif,
                                         agresseurs,
                                         calcul,
                                         &valw
                                       );
    /* Ajoute les agresseurs pour lesquels on a pas de ttv */
    for( i=0 ; i < nbagr ; i++ ) {
      param = agresseurs[i].PARAM ;
      if( !stb_getttvsigrcxparam( param ) ) {
        if( calcul == TTV_NODE_UP )
          valw = valw + agresseurs[i].NOISE_OVER;
        else
          valw = valw + agresseurs[i].NOISE_UNDER;
      }
    }

    stb_ctk_report_detail( stbfig, 
                           ttv_victime, 
                           alwaysactif, 
                           calcul, 
                           excluded
                         );
    freechain( excluded );
    
    bestgap      = NULL;
    bestexcluded = NULL ;
    v            = 0.0;
    
    for( gap = gaplist ; gap ; gap = gap->NEXT ) {
      if( CTK_LOGFILE ) {
        stb_ctkprint( 1, "  gap [%.1f;%.1f]\n", 
                      gap->START/TTV_UNIT, gap->END/TTV_UNIT 
                    );
        for( chain = gap->SIGNALS ; chain ; chain = chain->NEXT ) 
          stb_ctkprint( 1, "    %s\n", 
                        ((ttvevent_list*)chain->DATA)->ROOT->NAME 
                      );
      }
      if( gap->START == gap->END ) {
        continue ;
      }

      excluded = stb_ctk_noise_event_list( stbfig, 
                                           ttv_victime,
                                           gap->SIGNALS,
                                           agresseurs,
                                           calcul,
                                           &vgap
                                         );
      stb_ctkprint( 1, "    -> noise=%f\n", vgap );
      if( vgap > v ) {
        v = vgap ;
        bestgap = gap ;
        freechain( bestexcluded );
        bestexcluded = excluded ;
      }
      else
        if( vgap == v ) {
          for( chain = bestexcluded, n1=0 ; chain ; chain = chain->NEXT, n1++ );
          for( chain = excluded, n2=0 ; chain ; chain = chain->NEXT, n2++ );
          if( n2 > n1 ) {
            v = vgap ;
            bestgap = gap ;
            freechain( bestexcluded );
            bestexcluded = excluded ;
          }
          else
            freechain( excluded );
        }
        else
          freechain( excluded );
    }
        
    if( bestgap ) {
      if( CTK_LOGFILE ) {
        stb_ctkprint( 1, "  -> retained gap is [%.1f;%.1f]\n", bestgap->START/TTV_UNIT, bestgap->END/TTV_UNIT );
        stb_ctkprint( 1, "     excluded event are :\n" );
        for( chain = bestexcluded ; chain ; chain = chain->NEXT ) {
          stb_ctkprint( 1, 
                        "       %s\n",
                        ((ttvevent_list*)chain->DATA)->ROOT->NAME 
                      );
        }
      }
      stb_ctk_report_detail( stbfig,
                             ttv_victime,
                             bestgap->SIGNALS,
                             calcul,
                             bestexcluded
                           );
    }
    freechain( bestexcluded );
    stb_ctkprint( 1, "  ----------------\n" );
    v = v + valw ;
    rcx_freetabagr( agresseurs, sizeof( rcx_signal_noise ), nbagr );
    break;
    
  }

  *max  = vmax;
  *real = v;
  *mod = modname;
}

int  stb_ctk_noise_node( stbfig_list    *stbfig, 
                         lofig_list     *figvic,
                         stb_ctk_detail *detail,
                         chain_list     *headagr,
                         long            level, 
                         long            type,
                         char          **ovr_mod,
                         float          *ovr_max,
                         float          *ovr_real,
                         char          **und_mod,
                         float          *und_max,
                         float          *und_real
                       )
{
  int            i;
  long           calcul[2]={TTV_NODE_UP,TTV_NODE_DOWN};
  chain_list    *chain ;
  ttvevent_list *event ;
  stbpair_list  *pair, *scanpair ;
  stb_ctk_gap   *ctkgap ;
  chain_list    *headtoclean ;

  *ovr_mod = "";
  *und_mod = "";
  *ovr_max = 0.0;
  *ovr_real = 0.0;
  *und_max = 0.0;
  *und_real = 0.0;

  if( CTK_LOGFILE ) {
    stb_ctkprint( 0, "noise for %s %s (domain %s)\n",
                  ( detail->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" : "DW",
                  detail->NODE->ROOT->NAME,
                  stb_geteventdomainnamelist( stbfig, detail->NODE )
                );
  }

  for( i=0; i<2 ; i++ ) { 
 
    headtoclean = stb_createchainevent( headagr, calcul[i] );
    ctkgap = stb_fill_gap( stbfig, detail, calcul[i] );

    if( CTK_LOGFILE ) {
      for( chain = ctkgap->OTHERDOMAIN ; chain ; chain = chain->NEXT ) { 
        event = (ttvevent_list*)chain->DATA ;
        stb_ctkprint( 1, "dift dom : %s (domain %s)\n", 
                      event->ROOT->NAME,
                      stb_geteventdomainnamelist( stbfig, event )
                    );
      }
      for( chain = ctkgap->CONDACTIF ; chain ; chain = chain->NEXT ) {
        event = (ttvevent_list*)chain->DATA ;
        stb_ctkprint( 1, 
                      "%s %s : ", 
                      ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP": "DW",
                      event->ROOT->NAME
                    );
        pair = stb_getpairnode( stbfig, event, STB_CTK_MARGIN );
        for( scanpair = pair ; scanpair ; scanpair = scanpair->NEXT ) {
          stb_ctkprint( 1,"[%d;%d] ", scanpair->D, scanpair->U );
        }
        stb_ctkprint( 1, "\n" );
        stb_freestbpair( pair );
      }
    }

    if( calcul[i]==TTV_NODE_UP )
      stb_ctk_noise_node_gaplist( stbfig, 
                                  detail->NODE, 
                                  figvic,
                                  detail->LOSIG, 
                                  ctkgap->GAPACTIF,
                                  ctkgap->OTHERDOMAIN,
                                  level, 
                                  type,
                                  calcul[i],
                                  ovr_mod,
                                  ovr_max,
                                  ovr_real
                                );
    else
      stb_ctk_noise_node_gaplist( stbfig, 
                                  detail->NODE,
                                  figvic,
                                  detail->LOSIG, 
                                  ctkgap->GAPACTIF, 
                                  ctkgap->OTHERDOMAIN,
                                  level, 
                                  type,
                                  calcul[i],
                                  und_mod,
                                  und_max,
                                  und_real
                                );

    stb_freechainevent( headtoclean );
  }

  return 1;
}

/******************************************************************************\
Fonction de comparaison pour trier les tableaux de valeur de bruit. Le critère
de trie doit être mis dans la variable globale STB_CTK_NOISE_COMPARE.
\******************************************************************************/
int stb_ctk_noise_compare( stb_ctk_tab_stat *n1, stb_ctk_tab_stat *n2 )
{
  float noise1, noise2 ;
  int r, trs1, trs2 ;

  switch( STB_CTK_NOISE_COMPARE ) {
  case STB_CTK_SORT_INSIDE_ALIM_REAL:
    if( ( n1->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
      noise1 = n1->RISE_PEAK_REAL;
    else
      noise1 = n1->FALL_PEAK_REAL;
    if( ( n2->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
      noise2 = n2->RISE_PEAK_REAL;
    else
      noise2 = n2->FALL_PEAK_REAL;
    break;
  case STB_CTK_SORT_INSIDE_ALIM_MAX:
    if( ( n1->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
      noise1 = n1->RISE_PEAK_MAX;
    else
      noise1 = n1->FALL_PEAK_MAX;
    if( ( n2->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
      noise2 = n2->RISE_PEAK_MAX;
    else
      noise2 = n2->FALL_PEAK_MAX;
    break;
  case STB_CTK_SORT_RISE_MAX:
    noise1 = n1->RISE_PEAK_MAX;
    noise2 = n2->RISE_PEAK_MAX;
    break;
  case STB_CTK_SORT_RISE_REAL:
    noise1 = n1->RISE_PEAK_REAL;
    noise2 = n2->RISE_PEAK_REAL;
    break;
  case STB_CTK_SORT_FALL_MAX:
    noise1 = n1->FALL_PEAK_MAX;
    noise2 = n2->FALL_PEAK_MAX;
    break;
  case STB_CTK_SORT_FALL_REAL:
    noise1 = n1->FALL_PEAK_REAL;
    noise2 = n2->FALL_PEAK_REAL;
    break;
  default:
    noise1 = 0.0;
    noise2 = 0.0;
  }
  if( noise1 < noise2 ) return  1;
  if( noise1 > noise2 ) return -1;

  r =strcmp( n1->NODE->ROOT->NAME, n2->NODE->ROOT->NAME );
  if( r )
    return r ;

  if( ( n1->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
    trs1=1;
  else
    trs1=0;

  if( ( n2->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
    trs2=1;
  else
    trs2=0;

  if( trs1>trs2 ) return  1;
  if( trs2>trs1 ) return -1;
  return 0;
}

void stb_ctk_noise_report( stbfile *report, stb_ctk_stat *tabnoise )
{
  int                 curnoise;

  if( !CTK_PROGRESSBAR ) {
    printf( "Driving noises in report file\n" );
    stb_progressbar( tabnoise->NBDISPLAY, 0 );
  }
  else
    CTK_PROGRESSBAR( 0, 1, 0, "Driving noises in report file" );

  stb_info( report, "# Noise analysis :\n" );
  stb_info( report, "# ----------------\n\n" );

  stb_info( report, "# %-50s %14s %14s %24s\n", 
                    "", 
                    "Rise peak", 
                    "Fall peak",
                    "Scores"
          );
  stb_info( report,
            "# %-50s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s\n\n",
            "Node",
            "Vth",
            "Mod",
            "Max",
            "Real",
            "Mod",
            "Max",
            "Real",
            "Tot",
            "Nois",
            "Intv",
            "Ctk",
            "Acty"
          );

  stb_info( report, "BeginNoise\n" );
  for( curnoise=0 ; curnoise < tabnoise->NBDISPLAY ; curnoise++ ) {

    if( !CTK_PROGRESSBAR )
      stb_progressbar( tabnoise->NBDISPLAY, curnoise );
    else
      CTK_PROGRESSBAR( 0, tabnoise->NBDISPLAY, curnoise, NULL );
      
    if( tabnoise->TAB[curnoise].RISE_PEAK_MAX == 0.0 && 
        tabnoise->TAB[curnoise].FALL_PEAK_MAX == 0.0    )
      continue;
      
    if( tabnoise->TAB[curnoise].RISE_PEAK_MAX*1000.0 >= 
        (long)CTK_REPORT_NOISE_MIN ||
        tabnoise->TAB[curnoise].FALL_PEAK_MAX*1000.0 >= 
        (long)CTK_REPORT_NOISE_MIN                       ) 
    {
      stb_info( report,
          "  %-50s %4.0f %4s %4.0f %4.0f %4s %4.0f %4.0f %4d %4d %4d %4d %4d\n",
             stb_display_ttvevent_noise( report->STBFIG->FIG, 
                                         tabnoise->TAB[curnoise].NODE 
                                       ),
                tabnoise->TAB[curnoise].VTH*1000.0,
                tabnoise->TAB[curnoise].NOISE_MODEL,
                tabnoise->TAB[curnoise].RISE_PEAK_MAX*1000.0,
                tabnoise->TAB[curnoise].RISE_PEAK_REAL*1000.0,
                tabnoise->TAB[curnoise].NOISE_MODEL,
                tabnoise->TAB[curnoise].FALL_PEAK_MAX*1000.0,
                tabnoise->TAB[curnoise].FALL_PEAK_REAL*1000.0,
                stb_ctk_get_score_total( & tabnoise->TAB[curnoise] ),
                stb_ctk_get_score_noise( & tabnoise->TAB[curnoise] ),
                stb_ctk_get_score_interval( & tabnoise->TAB[curnoise] ),
                stb_ctk_get_score_ctk( & tabnoise->TAB[curnoise] ),
                stb_ctk_get_score_activity( & tabnoise->TAB[curnoise] )
              );
      
    }
  }
  stb_info( report, "EndNoise\n\n\n" );

  if( !CTK_PROGRESSBAR ) {
    stb_progressbar( tabnoise->NBDISPLAY, tabnoise->NBDISPLAY );
    printf( "\n" );
  }
  else
    CTK_PROGRESSBAR( 0, tabnoise->NBDISPLAY, tabnoise->NBDISPLAY, NULL );
}

void stb_ctk_sort_stat( stb_ctk_stat *stat, 
                         char criterion
                       )
{
  stb_ctk_fix_min( stat );

  if( criterion == STB_CTK_SORT_INSIDE_ALIM_MAX  ||
      criterion == STB_CTK_SORT_INSIDE_ALIM_REAL ||
      criterion == STB_CTK_SORT_RISE_MAX         ||
      criterion == STB_CTK_SORT_RISE_REAL        ||
      criterion == STB_CTK_SORT_FALL_MAX         ||
      criterion == STB_CTK_SORT_FALL_REAL           ) {
    STB_CTK_NOISE_COMPARE = criterion ;
    qsort( stat->TAB, 
           stat->NBDISPLAY, 
           sizeof( stb_ctk_tab_stat ), 
           (int(*)(const void*,const void*)) stb_ctk_noise_compare 
         );
    STB_CTK_NOISE_COMPARE = STB_CTK_SORT_INSIDE_ALIM_REAL ;
  }
  else {
    STB_CTK_STAT_COMPARE = criterion ;
    qsort( stat->TAB, 
           stat->NBDISPLAY, 
           sizeof( stb_ctk_tab_stat ), 
           (int(*)(const void*,const void*)) stb_ctk_score_compare 
         );
    STB_CTK_STAT_COMPARE = STB_CTK_SORT_SCORE_TOTAL ;
  }

}

/*

Met à jour la structure stb_ctk_detail si elle est présente.
La chain_list head contient une list d'event agresseur du node.

*/

void stb_ctk_report_detail( stbfig_list   *stbfig, 
                            ttvevent_list *victim, 
                            chain_list    *head, 
                            long           calcul,
                            chain_list    *excluded
                          )
{
  chain_list                    *chain ;
  stb_ctk_detail_agr_list       *detail ;
  ttvsig_list                   *aggr;

  for( chain = excluded ; chain ; chain = chain->NEXT ) {
    aggr = ((ttvevent_list*)chain->DATA)->ROOT ;
    aggr->USER = addptype( aggr->USER, STB_CTK_EXCLUDED, NULL );
  }

  for( chain = head ; chain ; chain = chain->NEXT ) {
    aggr = ((ttvevent_list*)chain->DATA)->ROOT ;
    if( ! getptype( aggr->USER, STB_CTK_EXCLUDED ) ) {
      detail = stb_ctk_get_node_detail( aggr );
      if( detail ) {
        if( calcul == TTV_NODE_UP )
          detail->NOISE_RISE_PEAK = 1;
        if( calcul == TTV_NODE_DOWN )
          detail->NOISE_FALL_PEAK = 1;
      }
    }
  }

  for( chain = excluded ; chain ; chain = chain->NEXT ) {
    aggr = ((ttvevent_list*)chain->DATA)->ROOT ;
    aggr->USER = delptype( aggr->USER, STB_CTK_EXCLUDED );
    detail = stb_ctk_get_node_detail( aggr );
    if( detail ) {
      if( calcul == TTV_NODE_UP )
        detail->NOISE_RISE_EXCLUDED = 1;
      if( calcul == TTV_NODE_DOWN )
        detail->NOISE_FALL_EXCLUDED = 1;
    }
  }

  stbfig=NULL;
  victim=NULL ;
}

char* stb_geteventdomainnamelist( stbfig_list *stbfig, ttvevent_list *event )
{
  int              phase ;
  stbck           *cklist ;
  char             bufckname[1024] ;
  char             bufckfullname[1024] ;
  static char      buffer[2048] ;
  stbnode         *node ;
  char             edge ;

  node = stb_getstbnode( event );
  if( !node || node->NBINDEX == 0 )
    return "" ;

  buffer[0]='\0';

  for( cklist = node->CK ; cklist ; cklist = cklist->NEXT ) {

    if( cklist->CKINDEX == STB_NO_INDEX ) {
      strcat( buffer, "ckpth no index " );
      continue ;
    }

    stb_getclock( stbfig, cklist->CKINDEX, bufckname, &edge, cklist);
    sprintf( bufckfullname, 
             "ckpth %s %s ", 
             edge == STB_SLOPE_UP ? "UP" : "DOWN",
             bufckname
           );
    strcat( buffer, bufckfullname );
  }

  for( phase = 0 ; phase < node->NBINDEX ; phase++ ) {
  
    if( node->STBTAB[phase] ) {
      stb_getclock( stbfig, phase, bufckname, &edge, NULL);
      sprintf( bufckfullname, 
               "%s %s ", 
               edge == STB_SLOPE_UP ? "UP" : "DOWN",
               bufckname
             );
      strcat( buffer, bufckfullname );
    }
  }
  
  return buffer;
}
