/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_hier.c                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#include "trc.h"

/* CVS informations :

Revision : $Revision: 1.29 $
Author   : $Author: fabrice $
Date     : $Date: 2008/03/20 16:23:04 $

*/

int RCX_NB_CAPA_L0      = -1 ;
int RCX_NB_CAPA_L1      = -1 ;
int RCX_NB_CAPA_L2      = -1 ;
int RCX_CAPA_NOM_MIN    = -1 ;
int RCX_CAPA_NOM_MAX    = -1 ;
int RCX_CAPA_UP_NOM_MIN = -1 ;
int RCX_CAPA_UP_NOM_MAX = -1 ;
int RCX_CAPA_DW_NOM_MIN = -1 ;
int RCX_CAPA_DW_NOM_MAX = -1 ;
int RCX_CAPA_UP_MIN     = -1 ;
int RCX_CAPA_UP_MAX     = -1 ;
int RCX_CAPA_DW_MIN     = -1 ;
int RCX_CAPA_DW_MAX     = -1 ;
int RCX_CAPA_NOMF_MIN   = -1 ;
int RCX_CAPA_NOMF_MAX   = -1 ;
int RCX_CAPA_UP_NOMF_MIN = -1 ;
int RCX_CAPA_UP_NOMF_MAX = -1 ;
int RCX_CAPA_DW_NOMF_MIN = -1 ;
int RCX_CAPA_DW_NOMF_MAX = -1 ;

/******************************************************************************\

Renvoie la capacité associée à un locon.

slope  : TRC_SLOPE_UP, TRC_SLOPE_DOWN, TRC_SLOPE_UNK.
type   : TRC_CAPA_MIN, TRC_CAPA_NOM, TRC_CAPA_NOM_MIN, 
         TRC_CAPA_NOM_MAX, TRC_CAPA_MAX.
domain : TRC_HALF, TRC_END

status (peut être NULL) : indique quelle est la valeur de la capacité.
  RCX_CAPA_OK :    la capacité a été exactement trouvée.
  RCX_CAPA_EQUIV : la capacité a été déduite.                     

\******************************************************************************/

RCXFLOAT rcx_getloconcapa( locon_list *locon, 
                           char slope, 
                           char type, 
                           char domain,
                           char *status 
                         )
{
  float *ptspace ;
  RCXFLOAT capa ;
  
  ptspace = rcx_loconcapa_getspace( locon ) ;
  if( !ptspace ) {
    if( status )
      *status = RCX_CAPA_EQUIV ;
    return 0.0 ;
  }
  if( status ) *status = RCX_CAPA_OK ;
  
  switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {

  case ELP_CAPA_LEVEL0 :

    switch( type ) {
    
    case TRC_CAPA_NOM :
      if( domain == TRC_HALF )
        capa = ( ptspace[RCX_CAPA_NOM_MIN] + ptspace[RCX_CAPA_NOM_MAX] ) / 2.0 ;
      else
        capa = ( ptspace[RCX_CAPA_NOMF_MIN] + ptspace[RCX_CAPA_NOMF_MAX] ) / 2.0 ;
      break ;
      
    case TRC_CAPA_NOM_MIN :
      if( domain == TRC_HALF )
        capa = ptspace[RCX_CAPA_NOM_MIN] ;
      else
        capa = ptspace[RCX_CAPA_NOMF_MIN] ;
      break ;
      
    case TRC_CAPA_NOM_MAX :
      if( domain == TRC_HALF )
        capa = ptspace[RCX_CAPA_NOM_MAX] ;
      else
        capa = ptspace[RCX_CAPA_NOMF_MAX] ;
      break ;
      
    default :
      if( domain == TRC_HALF )
        capa = ( ptspace[RCX_CAPA_NOM_MIN] + ptspace[RCX_CAPA_NOM_MAX] ) / 2.0 ;
      else
        capa = ( ptspace[RCX_CAPA_NOMF_MIN] + ptspace[RCX_CAPA_NOMF_MAX] ) / 2.0 ;
      if( status ) *status = RCX_CAPA_EQUIV ;
      break ;
    }

    break;
    
  case ELP_CAPA_LEVEL1 :

    switch( type ) {
    
    case TRC_CAPA_NOM :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_UP_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_UP_NOMF_MAX] ) / 2.0 ;
          break ;
        
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_DW_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_DW_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MAX] ) / 2.0 ;
          break ;
        
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_UP_NOM_MAX] +
                     ptspace[RCX_CAPA_DW_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MAX]   ) / 4.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_UP_NOMF_MAX] +
                     ptspace[RCX_CAPA_DW_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MAX]   ) / 4.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }
      break ;
      
    case TRC_CAPA_NOM_MIN :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_UP_NOM_MIN] ;
          else
            capa = ptspace[RCX_CAPA_UP_NOMF_MIN] ;
          break ;
        
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_DW_NOM_MIN] ;
          else
            capa = ptspace[RCX_CAPA_DW_NOMF_MIN] ;
          break ;
        
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MIN] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MIN] ) / 2.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }
      break ;
      
    case TRC_CAPA_NOM_MAX :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_UP_NOM_MAX] ;
          else
            capa = ptspace[RCX_CAPA_UP_NOMF_MAX] ;
          break ;
        
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_DW_NOM_MAX] ;
          else
            capa = ptspace[RCX_CAPA_DW_NOMF_MAX] ;
          break ;
        
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MAX] + ptspace[RCX_CAPA_DW_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MAX] + ptspace[RCX_CAPA_DW_NOMF_MAX] ) / 2.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }
      break ;
      
    default :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_UP_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_UP_NOMF_MAX] ) / 2.0 ;
          break ;
        
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_DW_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_DW_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MAX] ) / 2.0 ;
          break ;
        
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_UP_NOM_MAX] +
                     ptspace[RCX_CAPA_DW_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MAX]   ) / 4.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_UP_NOMF_MAX] +
                     ptspace[RCX_CAPA_DW_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MAX]   ) / 4.0 ;
          break ;
      }
      if( status ) *status = RCX_CAPA_EQUIV ;
    }
    break;

  case ELP_CAPA_LEVEL2 :
  
    switch( type ) {
    
    case TRC_CAPA_MIN :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          capa = ptspace[RCX_CAPA_UP_MIN];
          break ;
          
        case TRC_SLOPE_DOWN :
          capa = ptspace[RCX_CAPA_DW_MIN];
          break ;
          
        case TRC_SLOPE_UNK :
          capa = ( ptspace[RCX_CAPA_UP_MIN] + ptspace[RCX_CAPA_DW_MIN] ) / 2.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }

      break ;
      
    case TRC_CAPA_NOM :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_UP_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_UP_NOMF_MAX] ) / 2.0 ;
          break ;
          
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_DW_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_DW_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MAX] ) / 2.0 ;
          break ;
          
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_UP_NOM_MAX] +
                     ptspace[RCX_CAPA_DW_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MAX]   ) / 4.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_UP_NOMF_MAX] +
                     ptspace[RCX_CAPA_DW_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MAX]   ) / 4.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }

      break ;
      
    case TRC_CAPA_NOM_MIN :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_UP_NOM_MIN] ;
          else
            capa = ptspace[RCX_CAPA_UP_NOMF_MIN] ;
          break ;
          
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_DW_NOM_MIN] ;
          else
            capa = ptspace[RCX_CAPA_DW_NOMF_MIN] ;
          break ;
          
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MIN] + ptspace[RCX_CAPA_DW_NOM_MIN] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MIN] + ptspace[RCX_CAPA_DW_NOMF_MIN] ) / 2.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }

      break ;
      
    case TRC_CAPA_NOM_MAX :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_UP_NOM_MAX] ;
          else
            capa = ptspace[RCX_CAPA_UP_NOMF_MAX] ;
          break ;
          
        case TRC_SLOPE_DOWN :
          if( domain == TRC_HALF )
            capa = ptspace[RCX_CAPA_DW_NOM_MAX] ;
          else
            capa = ptspace[RCX_CAPA_DW_NOMF_MAX] ;
          break ;
          
        case TRC_SLOPE_UNK :
          if( domain == TRC_HALF )
            capa = ( ptspace[RCX_CAPA_UP_NOM_MAX] + ptspace[RCX_CAPA_DW_NOM_MAX] ) / 2.0 ;
          else
            capa = ( ptspace[RCX_CAPA_UP_NOMF_MAX] + ptspace[RCX_CAPA_DW_NOMF_MAX] ) / 2.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }

      break ;
      
    case TRC_CAPA_MAX :
    
      switch( slope ) {
      
        case TRC_SLOPE_UP :
          capa = ptspace[RCX_CAPA_UP_MAX] ;
          break ;
          
        case TRC_SLOPE_DOWN :
          capa = ptspace[RCX_CAPA_DW_MAX] ;
          break ;
          
        case TRC_SLOPE_UNK :
          capa = ( ptspace[RCX_CAPA_UP_MAX] + ptspace[RCX_CAPA_DW_MAX] ) / 2.0 ;
          if( status ) *status = RCX_CAPA_EQUIV ;
          break ;
      }

      break ;
    }
    
    break;
  }
  return capa ;
}

float rcx_get_all_locon_capa( losig_list *losig, char slope, char type, char domain )
{
  ptype_list *ptl;
  chain_list *scancon;
  locon_list *locon;
  float      sumcapa=0.0;
  
  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( ptl ) {
    for( scancon = (chain_list*)ptl->DATA ; 
         scancon ; 
         scancon = scancon->NEXT ) {
      locon = (locon_list*)scancon->DATA;
      if( !rcx_isvalidlocon( locon ) )
        continue;
      sumcapa = sumcapa + rcx_getloconcapa( locon, slope, type, domain, NULL ) ;
    }
  }

  return sumcapa;
}
/******************************************************************************\

Initialise la capacité sur un locon.
Seul l'un des deux arguments lofig ou loins doit être positionné. Il s'agit de
l'endroit où est stocké le tableau des capacités allouées par tat.

\******************************************************************************/

void rcx_setloconcapa_l0( lofig_list *lofig, 
                          loins_list *loins, 
                          locon_list *locon, 
                          char domain,
                          RCXFLOAT capamin, 
                          RCXFLOAT capamax 
                        )
{
  float *ptspace;

  if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL0 ) {
    avt_errmsg( TRC_ERRMSG, "004", AVT_FATAL, 1 );
  }
  
  ptspace = rcx_loconcapa_getspace( locon );
  if( !ptspace ) 
    ptspace = rcx_loconcapa_addspace( lofig, loins, locon );

  switch( domain ) {
  case TRC_HALF :
    ptspace[RCX_CAPA_NOM_MIN] = capamin ;
    ptspace[RCX_CAPA_NOM_MAX] = capamax ;
    break ;
  case TRC_END :
    ptspace[RCX_CAPA_NOMF_MIN] = capamin ;
    ptspace[RCX_CAPA_NOMF_MAX] = capamax ;
    break ;
  case TRC_FULL :
    ptspace[RCX_CAPA_NOM_MIN]  = capamin ;
    ptspace[RCX_CAPA_NOM_MAX]  = capamax ;
    ptspace[RCX_CAPA_NOMF_MIN] = capamin ;
    ptspace[RCX_CAPA_NOMF_MAX] = capamax ;
  }
}

void rcx_setloconcapa_l1( lofig_list *lofig, 
                          loins_list *loins, 
                          locon_list *locon, 
                          char domain,
                          RCXFLOAT capa_up_min, 
                          RCXFLOAT capa_up_max, 
                          RCXFLOAT capa_dw_min,
                          RCXFLOAT capa_dw_max
                        )
{
  float *ptspace;
  
  if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL1 ) {
    avt_errmsg( TRC_ERRMSG, "004", AVT_FATAL, 2 );
  }

  ptspace = rcx_loconcapa_getspace( locon );
  if( !ptspace ) 
    ptspace = rcx_loconcapa_addspace( lofig, loins, locon );

  switch( domain ) {
  case TRC_HALF :
    ptspace[RCX_CAPA_UP_NOM_MIN]  = capa_up_min;
    ptspace[RCX_CAPA_UP_NOM_MAX]  = capa_up_max;
    ptspace[RCX_CAPA_DW_NOM_MIN]  = capa_dw_min;
    ptspace[RCX_CAPA_DW_NOM_MAX]  = capa_dw_max;
    break ;
  case TRC_END :
    ptspace[RCX_CAPA_UP_NOMF_MIN] = capa_up_min;
    ptspace[RCX_CAPA_UP_NOMF_MAX] = capa_up_max;
    ptspace[RCX_CAPA_DW_NOMF_MIN] = capa_dw_min;
    ptspace[RCX_CAPA_DW_NOMF_MAX] = capa_dw_max;
    break ;
  case TRC_FULL :
    ptspace[RCX_CAPA_UP_NOM_MIN]  = capa_up_min;
    ptspace[RCX_CAPA_UP_NOM_MAX]  = capa_up_max;
    ptspace[RCX_CAPA_DW_NOM_MIN]  = capa_dw_min;
    ptspace[RCX_CAPA_DW_NOM_MAX]  = capa_dw_max;
    ptspace[RCX_CAPA_UP_NOMF_MIN] = capa_up_min;
    ptspace[RCX_CAPA_UP_NOMF_MAX] = capa_up_max;
    ptspace[RCX_CAPA_DW_NOMF_MIN] = capa_dw_min;
    ptspace[RCX_CAPA_DW_NOMF_MAX] = capa_dw_max;
    break ;
  }
}

/* les capacités capa_up_min, capa_up_max, capa_dw_min, capa_dw_max sont prise en 
compte uniquement si domain=TRC_HALF */
void rcx_setloconcapa_l2( lofig_list *lofig, 
                          loins_list *loins, 
                          locon_list *locon, 
                          char domain,
                          RCXFLOAT capa_up_min,
                          RCXFLOAT capa_up_nom_min,
                          RCXFLOAT capa_up_nom_max,
                          RCXFLOAT capa_up_max,
                          RCXFLOAT capa_dw_min,
                          RCXFLOAT capa_dw_nom_min,
                          RCXFLOAT capa_dw_nom_max,
                          RCXFLOAT capa_dw_max
                        )
{
  float *ptspace;
  
  if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL2 ) {
    avt_errmsg( TRC_ERRMSG, "004", AVT_FATAL, 3 );
  }

  ptspace = rcx_loconcapa_getspace( locon );
  if( !ptspace ) 
    ptspace = rcx_loconcapa_addspace( lofig, loins, locon );

  switch( domain ) {
  case TRC_HALF :
    ptspace[RCX_CAPA_UP_MIN]      = capa_up_min;
    ptspace[RCX_CAPA_UP_MAX]      = capa_up_max;
    ptspace[RCX_CAPA_DW_MIN]      = capa_dw_min;
    ptspace[RCX_CAPA_DW_MAX]      = capa_dw_max;
    ptspace[RCX_CAPA_UP_NOM_MIN]  = capa_up_nom_min;
    ptspace[RCX_CAPA_UP_NOM_MAX]  = capa_up_nom_max;
    ptspace[RCX_CAPA_DW_NOM_MIN]  = capa_dw_nom_min;
    ptspace[RCX_CAPA_DW_NOM_MAX]  = capa_dw_nom_max;
    break ;
  case TRC_END :
    ptspace[RCX_CAPA_UP_NOMF_MIN] = capa_up_nom_min;
    ptspace[RCX_CAPA_UP_NOMF_MAX] = capa_up_nom_max;
    ptspace[RCX_CAPA_DW_NOMF_MIN] = capa_dw_nom_min;
    ptspace[RCX_CAPA_DW_NOMF_MAX] = capa_dw_nom_max;
    break ;
  case TRC_FULL :
    ptspace[RCX_CAPA_UP_MIN]      = capa_up_min;
    ptspace[RCX_CAPA_UP_MAX]      = capa_up_max;
    ptspace[RCX_CAPA_DW_MIN]      = capa_dw_min;
    ptspace[RCX_CAPA_DW_MAX]      = capa_dw_max;
    ptspace[RCX_CAPA_UP_NOM_MIN]  = capa_up_nom_min;
    ptspace[RCX_CAPA_UP_NOM_MAX]  = capa_up_nom_max;
    ptspace[RCX_CAPA_DW_NOM_MIN]  = capa_dw_nom_min;
    ptspace[RCX_CAPA_DW_NOM_MAX]  = capa_dw_nom_max;
    ptspace[RCX_CAPA_UP_NOMF_MIN] = capa_up_nom_min;
    ptspace[RCX_CAPA_UP_NOMF_MAX] = capa_up_nom_max;
    ptspace[RCX_CAPA_DW_NOMF_MIN] = capa_dw_nom_min;
    ptspace[RCX_CAPA_DW_NOMF_MAX] = capa_dw_nom_max;
    break ;
  }

}

/******************************************************************************\

Ajoute la capacité sur un locon.
Seul l'un des deux arguments lofig ou loins doit être positionné. Il s'agit de
l'endroit où est stocké le tableau des capacités allouées par tat.

\******************************************************************************/

void rcx_addloconcapa_l0( lofig_list *lofig, 
                          loins_list *loins, 
                          locon_list *locon, 
                          char domain,
                          RCXFLOAT capamin, 
                          RCXFLOAT capamax 
                        )
{
  float *ptspace;
  
  if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL0 ) {
    avt_errmsg( TRC_ERRMSG, "004", AVT_FATAL, 4 );
  }

  ptspace = rcx_loconcapa_getspace( locon );
  if( !ptspace ) 
    ptspace = rcx_loconcapa_addspace( lofig, loins, locon );

  if( domain == TRC_HALF ) {
    if( RCX_CAPA_NOM_MIN != RCX_CAPA_NOM_MIN ) {
      ptspace[RCX_CAPA_NOM_MIN] = ptspace[RCX_CAPA_NOM_MIN] + capamin ;
      ptspace[RCX_CAPA_NOM_MAX] = ptspace[RCX_CAPA_NOM_MAX] + capamax ;
    }
    else 
      ptspace[RCX_CAPA_NOM_MIN] = ptspace[RCX_CAPA_NOM_MIN] + (capamax+capamin)/2.0 ;
  }
  else {
    if( RCX_CAPA_NOMF_MIN != RCX_CAPA_NOMF_MIN ) {
      ptspace[RCX_CAPA_NOMF_MIN] = ptspace[RCX_CAPA_NOMF_MIN] + capamin ;
      ptspace[RCX_CAPA_NOMF_MAX] = ptspace[RCX_CAPA_NOMF_MAX] + capamax ;
    }
    else 
      ptspace[RCX_CAPA_NOMF_MIN] = ptspace[RCX_CAPA_NOMF_MIN] + (capamax+capamin)/2.0 ;
  }
}

void rcx_addloconcapa_l1( lofig_list *lofig, 
                          loins_list *loins, 
                          locon_list *locon, 
                          char domain,
                          RCXFLOAT capa_up_min, 
                          RCXFLOAT capa_up_max, 
                          RCXFLOAT capa_dw_min,
                          RCXFLOAT capa_dw_max
                        )
{
  float *ptspace;

  if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL1 ) {
    avt_errmsg( TRC_ERRMSG, "004", AVT_FATAL, 5 );
  }
  
  ptspace = rcx_loconcapa_getspace( locon );
  if( !ptspace ) 
    ptspace = rcx_loconcapa_addspace( lofig, loins, locon );

  if( domain == TRC_HALF ) {
    if( RCX_CAPA_UP_NOM_MIN != RCX_CAPA_UP_NOM_MAX ) {
      ptspace[RCX_CAPA_UP_NOM_MIN] = ptspace[RCX_CAPA_UP_NOM_MIN] + capa_up_min;
      ptspace[RCX_CAPA_UP_NOM_MAX] = ptspace[RCX_CAPA_UP_NOM_MAX] + capa_up_max;
      ptspace[RCX_CAPA_DW_NOM_MIN] = ptspace[RCX_CAPA_DW_NOM_MIN] + capa_dw_min;
      ptspace[RCX_CAPA_DW_NOM_MAX] = ptspace[RCX_CAPA_DW_NOM_MAX] + capa_dw_max;
    }
    else {
      ptspace[RCX_CAPA_UP_NOM_MIN] = ptspace[RCX_CAPA_UP_NOM_MIN] + (capa_up_max+capa_up_min)/2.0;
      ptspace[RCX_CAPA_DW_NOM_MIN] = ptspace[RCX_CAPA_DW_NOM_MIN] + (capa_dw_max+capa_dw_min)/2.0;
    }
  }
  else {
    if( RCX_CAPA_UP_NOMF_MIN != RCX_CAPA_UP_NOMF_MAX ) {
      ptspace[RCX_CAPA_UP_NOMF_MIN] = ptspace[RCX_CAPA_UP_NOMF_MIN] + capa_up_min;
      ptspace[RCX_CAPA_UP_NOMF_MAX] = ptspace[RCX_CAPA_UP_NOMF_MAX] + capa_up_max;
      ptspace[RCX_CAPA_DW_NOMF_MIN] = ptspace[RCX_CAPA_DW_NOMF_MIN] + capa_dw_min;
      ptspace[RCX_CAPA_DW_NOMF_MAX] = ptspace[RCX_CAPA_DW_NOMF_MAX] + capa_dw_max;
    }
    else {
      ptspace[RCX_CAPA_UP_NOMF_MIN] = ptspace[RCX_CAPA_UP_NOMF_MIN] + (capa_up_max+capa_up_min)/2.0;
      ptspace[RCX_CAPA_DW_NOMF_MIN] = ptspace[RCX_CAPA_DW_NOMF_MIN] + (capa_dw_max+capa_dw_min)/2.0;
    }
  }
}

/* les capacités capa_up_min, capa_up_max, capa_dw_min, capa_dw_max sont prise en 
compte uniquement si domain=TRC_HALF */
void rcx_addloconcapa_l2( lofig_list *lofig, 
                          loins_list *loins, 
                          locon_list *locon, 
                          char domain,
                          RCXFLOAT capa_up_min,
                          RCXFLOAT capa_up_nom_min,
                          RCXFLOAT capa_up_nom_max,
                          RCXFLOAT capa_up_max,
                          RCXFLOAT capa_dw_min,
                          RCXFLOAT capa_dw_nom_min,
                          RCXFLOAT capa_dw_nom_max,
                          RCXFLOAT capa_dw_max
                        )
{
  float *ptspace;
  
  if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL2 ) {
    avt_errmsg( TRC_ERRMSG, "004", AVT_FATAL, 6 );
  }
  
  ptspace = rcx_loconcapa_getspace( locon );
  if( !ptspace ) 
    ptspace = rcx_loconcapa_addspace( lofig, loins, locon );


  if( domain == TRC_HALF ) {

    ptspace[RCX_CAPA_UP_MIN]     = ptspace[RCX_CAPA_UP_MIN]     + capa_up_min;
    ptspace[RCX_CAPA_UP_MAX]     = ptspace[RCX_CAPA_UP_MAX]     + capa_up_max;
    ptspace[RCX_CAPA_DW_MIN]     = ptspace[RCX_CAPA_DW_MIN]     + capa_dw_min;
    ptspace[RCX_CAPA_DW_MAX]     = ptspace[RCX_CAPA_DW_MAX]     + capa_dw_max;
    
    if( RCX_CAPA_UP_NOM_MIN != RCX_CAPA_UP_NOM_MAX ) {
      ptspace[RCX_CAPA_UP_NOM_MIN] = ptspace[RCX_CAPA_UP_NOM_MIN] + capa_up_nom_min;
      ptspace[RCX_CAPA_UP_NOM_MAX] = ptspace[RCX_CAPA_UP_NOM_MAX] + capa_up_nom_max;
      ptspace[RCX_CAPA_DW_NOM_MIN] = ptspace[RCX_CAPA_DW_NOM_MIN] + capa_dw_nom_min;
      ptspace[RCX_CAPA_DW_NOM_MAX] = ptspace[RCX_CAPA_DW_NOM_MAX] + capa_dw_nom_max;
    }
    else {
      ptspace[RCX_CAPA_UP_NOM_MIN] = ptspace[RCX_CAPA_UP_NOM_MIN] + (capa_up_nom_min+capa_up_nom_max)/2.0;
      ptspace[RCX_CAPA_DW_NOM_MIN] = ptspace[RCX_CAPA_DW_NOM_MIN] + (capa_dw_nom_min+capa_dw_nom_max)/2.0;
    }
  }
  else {
    if( RCX_CAPA_UP_NOMF_MIN != RCX_CAPA_UP_NOMF_MAX ) {
      ptspace[RCX_CAPA_UP_NOMF_MIN] = ptspace[RCX_CAPA_UP_NOMF_MIN] + capa_up_nom_min;
      ptspace[RCX_CAPA_UP_NOMF_MAX] = ptspace[RCX_CAPA_UP_NOMF_MAX] + capa_up_nom_max;
      ptspace[RCX_CAPA_DW_NOMF_MIN] = ptspace[RCX_CAPA_DW_NOMF_MIN] + capa_dw_nom_min;
      ptspace[RCX_CAPA_DW_NOMF_MAX] = ptspace[RCX_CAPA_DW_NOMF_MAX] + capa_dw_nom_max;
    }
    else {
      ptspace[RCX_CAPA_UP_NOMF_MIN] = ptspace[RCX_CAPA_UP_NOMF_MIN] + (capa_up_nom_min+capa_up_nom_max)/2.0;
      ptspace[RCX_CAPA_DW_NOMF_MIN] = ptspace[RCX_CAPA_DW_NOMF_MIN] + (capa_dw_nom_min+capa_dw_nom_max)/2.0;
    }
  }
}

/******************************************************************************\

Ajoute les capacités de loconb dans celles de locona.

\******************************************************************************/

void rcx_add_capa_locon( lofig_list *lofig, 
                         locon_list *locona, 
                         locon_list *loconb
                       )
{
  char domain[] = { TRC_HALF, TRC_END } ;
  unsigned int i;

  for( i=0 ; i<sizeof(domain)/sizeof(domain[0]) ; i++ ) {
    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    case ELP_CAPA_LEVEL0 :
      rcx_addloconcapa_l0( lofig, 
                           NULL, 
                           locona, 
                           domain[i],
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UNK, 
                                             TRC_CAPA_NOM_MIN,
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UNK, 
                                             TRC_CAPA_NOM_MAX, 
                                             domain[i],
                                             NULL 
                                           )
                         );
      break ;
    case ELP_CAPA_LEVEL1 :
      rcx_addloconcapa_l1( lofig, 
                           NULL, 
                           locona, 
                           domain[i],
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UP, 
                                             TRC_CAPA_NOM_MIN, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UP, 
                                             TRC_CAPA_NOM_MAX, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_DOWN,
                                             TRC_CAPA_NOM_MIN, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_DOWN,
                                             TRC_CAPA_NOM_MAX, 
                                             domain[i],
                                             NULL 
                                           )
                         );
      break ;
    case ELP_CAPA_LEVEL2 :
      rcx_addloconcapa_l2( lofig, 
                           NULL, 
                           locona, 
                           domain[i],
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UP, 
                                             TRC_CAPA_MIN, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UP, 
                                             TRC_CAPA_NOM_MIN, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UP,
                                             TRC_CAPA_NOM_MAX,
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_UP, 
                                             TRC_CAPA_MAX, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_DOWN, 
                                             TRC_CAPA_MIN, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_DOWN, 
                                             TRC_CAPA_NOM_MIN, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_DOWN, 
                                             TRC_CAPA_NOM_MAX, 
                                             domain[i],
                                             NULL 
                                           ),
                           rcx_getloconcapa( loconb, 
                                             TRC_SLOPE_DOWN, 
                                             TRC_CAPA_MAX, 
                                             domain[i],
                                             NULL 
                                           )
                         );
      break ;
    }
  }
}

/******************************************************************************\

Efface une capacité associée à un locon.

\******************************************************************************/

void rcx_freeloconcapa( lofig_list *lofig,
                        loins_list *loins,
                        locon_list *locon
                      )
{
  if( rcx_loconcapa_freespace( lofig, loins, locon ) )
    locon->USER = delptype( locon->USER, RCX_LOCONCAPA_SPACE );
}

/******************************************************************************\

Ajoute un emplacement pour stocker une capacité sur un locon.
Seul l'un des deux arguments lofig ou loins doit être positionné. Il s'agit de
l'endroit où est stocké le tableau des capacités allouées par tat.

\******************************************************************************/

float* rcx_loconcapa_addspace( lofig_list *lofig,
                                  loins_list *loins,
                                  locon_list *locon 
                                )
{
  HeapAlloc *heap;
  float     *space;
  int        n, i;

  heap  = rcx_loconcapa_giveheap( lofig, loins );
  space = AddHeapItem( heap );

  locon->USER = addptype( locon->USER, RCX_LOCONCAPA_SPACE, space );
  switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
  case ELP_CAPA_LEVEL0 : n = RCX_NB_CAPA_L0 ; break ;
  case ELP_CAPA_LEVEL1 : n = RCX_NB_CAPA_L1 ; break ;
  case ELP_CAPA_LEVEL2 : n = RCX_NB_CAPA_L2 ; break ;
  }

  for( i=0 ; i < n ; i++ )
    space[i] = 0.0 ;
    
  return space;
}

/******************************************************************************\

Renvoie l'emplacement pour stocker une capacité sur un locon, ou NULL si cet
emplacement n'a jamais été créé par rcx_loconcapa_addspace().

\******************************************************************************/

float* rcx_loconcapa_getspace( locon_list *locon )
{
  ptype_list *ptl;

  ptl = getptype( locon->USER, RCX_LOCONCAPA_SPACE );
  if( !ptl ) return NULL;
  return (float*)ptl->DATA;
}

/******************************************************************************\

Libère l'emplacement pour stocker une capacité.

\******************************************************************************/

char rcx_loconcapa_freespace( lofig_list *lofig, 
                              loins_list *loins,
                              locon_list *locon )
{
  ptype_list *ptl;
  HeapAlloc  *heap;
  float      *space;
  
  ptl = getptype( locon->USER, RCX_LOCONCAPA_SPACE );
  if( !ptl ) return 0;
  space = (float*)ptl->DATA;

  heap = rcx_loconcapa_giveheap( lofig, loins );
  DelHeapItem( heap, space );

  return 1;
}

/******************************************************************************\

Renvoie un HeapAlloc associé soit à une lofig, soit à une loins.

\******************************************************************************/

HeapAlloc* rcx_loconcapa_giveheap( lofig_list *lofig, loins_list *loins )
{
  HeapAlloc  *heap;
  ptype_list *ptl;
  int         n;

  switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
  case ELP_CAPA_LEVEL0 : n = RCX_NB_CAPA_L0 * sizeof( float ) ; break ;
  case ELP_CAPA_LEVEL1 : n = RCX_NB_CAPA_L1 * sizeof( float ) ; break ;
  case ELP_CAPA_LEVEL2 : n = RCX_NB_CAPA_L2 * sizeof( float ) ; break ;
  default :
    rcx_error(44,NULL,AVT_FATAL);
  }
  
  if( lofig ) {
    ptl = getptype( lofig->USER, RCX_LOCONCAPA_HEAP );
    if( !ptl ) {
      heap = mbkalloc( sizeof( HeapAlloc ) );
      CreateHeap( n, 4096, heap );
      lofig->USER = addptype( lofig->USER, RCX_LOCONCAPA_HEAP, heap );
    }
    else {
      heap = (HeapAlloc*)ptl->DATA;
    }
  }
  else {
    ptl = getptype( loins->USER, RCX_LOCONCAPA_HEAP );
    if( !ptl ) {
      heap = mbkalloc( sizeof( HeapAlloc ) );
      CreateHeap( n, 32, heap );
      loins->USER = addptype( loins->USER, RCX_LOCONCAPA_HEAP, heap );
    }
    else {
      heap = (HeapAlloc*)ptl->DATA;
    }
  }

  return heap;
}

/******************************************************************************\

Libère le HeapAlloc

\******************************************************************************/

void rcx_loconcapa_freeheap( lofig_list *lofig, loins_list *loins )
{
  HeapAlloc  *heap;
  ptype_list *ptl;

  if( lofig ) 
    ptl = getptype( lofig->USER, RCX_LOCONCAPA_HEAP );
  if( loins )
    ptl = getptype( loins->USER, RCX_LOCONCAPA_HEAP );

  if( ptl ) {
    heap = (HeapAlloc*)ptl->DATA;
    DeleteHeap( heap );
    if( lofig ) 
      lofig->USER = delptype( lofig->USER, RCX_LOCONCAPA_HEAP );
    if( loins ) 
      loins->USER = delptype( loins->USER, RCX_LOCONCAPA_HEAP );
    mbkfree( heap );
  }
}

/******************************************************************************\



\******************************************************************************/

RCXFLOAT rcx_getnodecapa( losig_list  *ptsig, 
                          lonode_list *node, 
                          RCXFLOAT     extcapa,
                          rcx_slope   *slope,
                          char         type,
                          char         domain,
                          RCXFLOAT     coefctc
                        )
{
  RCXFLOAT         capa=0.0, c ;
  loctc_list      *ctc  ;
  lowire_list     *lowire ;
  chain_list      *chain ;
  num_list        *sc;
  int              nb;
  locon_list      *ptlocon;
  float            capalocon;
  rcx_list        *ptrcx;
  char             capatype;

  ptrcx = getrcx( ptsig );
  
  // Capacité due aux capacités couplées.
  c = 0.0 ;
  for( chain = node->CTCLIST ; chain ; chain = chain->NEXT ) {
    ctc = (loctc_list*)chain->DATA;
    c = c + rcx_capamiller( ptsig, ctc, slope, type );
  }
  capa = capa + c;

  // Capacité due aux fils.
  c = 0.0 ;
  for( chain = node->WIRELIST ; chain ; chain = chain->NEXT ) {
    lowire = (lowire_list*)(chain->DATA);
    c = c + lowire->CAPA/2.0 ;
  }
  capa = capa + c;

  // Capacité due aux connecteurs.
  c = 0.0 ;
  for( chain = getloconnode( node ) ; chain ; chain = chain->NEXT ) {
    
    ptlocon = (locon_list*)chain->DATA ;
    if( !rcx_isvalidlocon( ptlocon ) )
      continue;

    nb=0;

    if( type == RCX_MIN )
      capatype = TRC_CAPA_NOM_MIN ;
    else
      capatype = TRC_CAPA_NOM_MAX ;
    capalocon = rcx_getloconcapa( ptlocon, slope->SENS, capatype, domain, NULL );

    if( capalocon > 0.0 ) {
      for( sc = ptlocon->PNODE, nb=0 ; sc ; sc=sc->NEXT, nb++ );
      c = c + capalocon / nb ;
    }

    if( rcx_isloconexternal( ptlocon ) ) {
      if( nb == 0 )
        for( sc = ptlocon->PNODE, nb=0 ; sc ; sc=sc->NEXT, nb++ );
      
      c = c + extcapa / nb ;
    }
  }
  capa = capa + c ;

  // Application du correctif pour les capacités de couplage globales.

  capa = capa * coefctc ;
  
  // Redimensionnement des capacités

  capa = capa * tlc_getcapafactor();

  return(capa);
}

/******************************************************************************\

Récupère différents type de capacité :

   capatype : flag
     RCX_SIGCAPA_GROUND : Renvoie les capacités à la masse.
     RCX_SIGCAPA_CTK :    Renvoie les capacités de couplage.
   capapos : flag
     RCX_SIGCAPA_LOCAL :   Prend en compte les capacités localisées.
     RCX_SIGCAPA_GLOBAL :  Prend en compte les capacités globales.
   capadyna : valeur
     RCX_SIGCAPA_NORM :   CTC à leurs valeurs nominales.
     RCX_SIGCAPA_MILLER : Modifie la valeurs des CTC avec l'effet Miller.
     RCX_SIGCAPA_DELTA :  Calcule la VARIATION de capacité due à l'effet Miller.

   slope : utilisé seulement si capadyna différent de RCX_SIGCAPA_NORM.
   type :  RCX_MIN | RCX_MAX. utilisé seulement si capadyna différent de 
           RCX_SIGCAPA_NORM.
   
\******************************************************************************/

RCXFLOAT rcx_getsigcapa( lofig_list    *lofig,
                         losig_list    *ptsig,
                         char          capatype,
                         char          capapos,
                         char          capadyna,
                         rcx_slope     *slope,
                         char          type,
                         char          domain
                       )
{
  RCXFLOAT     sumcapa = 0.0 ;
  RCXFLOAT     capa;
  long         node;
  char         thiscapatype;
  char         thiscapapos;
  chain_list  *scanctc;
  loctc_list  *ctc;
  lowire_list *scanwire;
  char         hasglobal = 'N';
  rcx_list    *ptrcx=NULL;
  ptype_list  *ptl;
  locon_list  *locon;
  chain_list  *scancon;
  char         slopetype;
  char         ctype;

  ptrcx = getrcx( ptsig );

  if( capapos == RCX_SIGCAPA_GLOBAL ) {
    if( GETFLAG( ptrcx->FLAG, RCXHASNOGCTC ) )
      return 0.0;
  }

  if( lofig )
    rcn_lock_signal( lofig, ptsig );

  if( ( capatype & RCX_SIGCAPA_GROUND ) == RCX_SIGCAPA_GROUND  &&
      ( capapos  & RCX_SIGCAPA_LOCAL  ) == RCX_SIGCAPA_LOCAL   &&
      ( capadyna == RCX_SIGCAPA_NORM || capadyna == RCX_SIGCAPA_MILLER )
    ) {
    ptl = getptype( ptsig->USER, LOFIGCHAIN );
    if( ptl ) {
      for( scancon = (chain_list*)ptl->DATA ; 
           scancon ; 
           scancon = scancon->NEXT ) {
        locon = (locon_list*)scancon->DATA;
        if( !rcx_isvalidlocon( locon ) )
          continue;
        if ( !slope ) 
          slopetype = TRC_SLOPE_UNK;
        else
          slopetype = slope->SENS;
        if( type == RCX_MIN )
          ctype = TRC_CAPA_NOM_MIN ;
        else
          ctype = TRC_CAPA_NOM_MAX ;
        capa = rcx_getloconcapa( locon, slopetype, ctype, domain, NULL );
        sumcapa = sumcapa + capa ;
      }
    }
  }
 
  for( scanctc = ptsig->PRCN->PCTC ; scanctc ;scanctc = scanctc->NEXT ) {
  
    ctc   = (loctc_list*)scanctc->DATA;
    
    node = rcn_ctcnode( ctc, ptsig );
    
    thiscapatype = 0;
    thiscapapos = 0;
    
    if( rcx_iscrosstalkcapa( ctc, ptsig ) )
      thiscapatype = thiscapatype | RCX_SIGCAPA_CTK ;
    else
      thiscapatype = thiscapatype | RCX_SIGCAPA_GROUND ;

    if( node )
      thiscapapos = thiscapapos | RCX_SIGCAPA_LOCAL ;
    else {
      thiscapapos = thiscapapos | RCX_SIGCAPA_GLOBAL ;
      hasglobal = 'Y';
    }

    if( ( thiscapatype & capatype ) && ( thiscapapos & capapos ) ) {
    
      capa = 0.0;
      
      switch( capadyna ) {
      case RCX_SIGCAPA_NORM :
        capa = ctc->CAPA ;
        break;
      case RCX_SIGCAPA_MILLER :
        capa = rcx_capamiller( ptsig, ctc, slope, type );
        break;
      case RCX_SIGCAPA_DELTA :
        capa = rcx_capamiller( ptsig, ctc, slope, type ) 
               - ctc->CAPA;
        break;
      }

      sumcapa = sumcapa + capa;
    }
  }

  if( ( capatype & RCX_SIGCAPA_GROUND ) == RCX_SIGCAPA_GROUND  &&
      ( capapos  & RCX_SIGCAPA_LOCAL  ) == RCX_SIGCAPA_LOCAL   &&
      ( capadyna == RCX_SIGCAPA_NORM || capadyna == RCX_SIGCAPA_MILLER )
    ) {

    for( scanwire = ptsig->PRCN->PWIRE ; scanwire ; scanwire = scanwire->NEXT )
      sumcapa = sumcapa + scanwire->CAPA ;
  }

  if( hasglobal=='N' && ptrcx )
    SETFLAG( ptrcx->FLAG, RCXHASNOGCTC );

  if( lofig )
    rcn_unlock_signal( lofig, ptsig );
  return( sumcapa );
  
}

/******************************************************************************\
Renvoie la capacité vue par un net RCX
\******************************************************************************/
float rcx_getcapa( lofig_list *lofig, losig_list *losig )
{
  float       capa;
  float      sumcapa=0.0;
  
  capa = rcx_get_all_locon_capa( losig, TRC_SLOPE_UNK, TRC_CAPA_NOM, TRC_HALF ) ; 
  sumcapa = sumcapa + capa ;
  capa = rcn_getcapa( lofig, losig ) ;
  sumcapa = sumcapa + capa ;

  return sumcapa;
}

void rcx_add_capa_from_instance_to_figure( loins_list *loins,
                                           lofig_list *lofig,
                                           locon_list *loconint,
                                           locon_list *loconext
                                         )
{
  RCXFLOAT cmin,       cmax ;
  RCXFLOAT cminf,      cmaxf ;
  RCXFLOAT cupmin,     cupmax ;
  RCXFLOAT cdwmin,     cdwmax ;
  RCXFLOAT cupminf,    cupmaxf ;
  RCXFLOAT cdwminf,    cdwmaxf ;
  RCXFLOAT cupnommin,  cupnommax ;
  RCXFLOAT cdwnommin,  cdwnommax ;
  RCXFLOAT cupnomminf, cupnommaxf ;
  RCXFLOAT cdwnomminf, cdwnommaxf ;
  
  switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
  case ELP_CAPA_LEVEL0 :
    cmin  = rcx_getloconcapa( loconint, TRC_SLOPE_UNK, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cmax  = rcx_getloconcapa( loconint, TRC_SLOPE_UNK, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cminf = rcx_getloconcapa( loconint, TRC_SLOPE_UNK, TRC_CAPA_NOM_MIN, TRC_END,  NULL );
    cmaxf = rcx_getloconcapa( loconint, TRC_SLOPE_UNK, TRC_CAPA_NOM_MAX, TRC_END,  NULL );
    rcx_freeloconcapa( NULL, loins, loconint );
    if( cmin>0.0 || cmax>0.0 || cminf || cmaxf ) {
      rcx_addloconcapa_l0( lofig, NULL, loconext, TRC_HALF, cmin, cmax );
      rcx_addloconcapa_l0( lofig, NULL, loconext, TRC_HALF, cminf, cmaxf );
    }
    break ;
  case ELP_CAPA_LEVEL1 :
    cupmin  = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cupmax  = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cdwmin  = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cdwmax  = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cupminf = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cupmaxf = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_END, NULL );
    cdwminf = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cdwmaxf = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_END, NULL );
    rcx_freeloconcapa( NULL, loins, loconint );
    if( cupmin>0.0  || cupmax>0.0  || cdwmin>0.0  || cdwmax>0.0 ||
        cupminf>0.0 || cupmaxf>0.0 || cdwminf>0.0 || cdwmaxf>0.0    ) {
      rcx_addloconcapa_l1( lofig, NULL, loconext, TRC_HALF, cupmin, cupmax, cdwmin, cdwmax );
      rcx_addloconcapa_l1( lofig, NULL, loconext, TRC_END, cupmin, cupmax, cdwmin, cdwmax );
    }
    break ;
  case ELP_CAPA_LEVEL2 :
    cupmin     = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_MIN,     TRC_HALF, NULL );
    cupmax     = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_MAX,     TRC_HALF, NULL );
    cdwmin     = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_MIN,     TRC_HALF, NULL );
    cdwmax     = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_MAX,     TRC_HALF, NULL );
    cupnommin  = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cupnommax  = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cdwnommin  = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cdwnommax  = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cupnomminf = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cupnommaxf = rcx_getloconcapa( loconint, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_END, NULL );
    cdwnomminf = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cdwnommaxf = rcx_getloconcapa( loconint, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_END, NULL );
    rcx_freeloconcapa( NULL, loins, loconint );
    if( cupmin>0.0  || cdwnommin>0.0  || cdwnommax>0.0  || cupmax>0.0  || 
        cdwmin>0.0  || cupnommin>0.0  || cupnommax>0.0  || cdwmax>0.0  ||
        cdwminf>0.0 || cupnomminf>0.0 || cupnommaxf>0.0 || cdwmaxf>0.0    ) {
      rcx_addloconcapa_l2( lofig, NULL, loconext, TRC_HALF,
                           cupmin, cupnommin, cupnommax, cupmax, 
                           cdwmin, cdwnommin, cdwnommax, cdwmax 
                         );
      rcx_addloconcapa_l2( lofig, NULL, loconext, TRC_END,
                           cupmin, cupnomminf, cupnommaxf, cupmax, 
                           cdwmin, cdwnomminf, cdwnommaxf, cdwmax 
                         );
    }
    break ;
  }
}

void rcx_transfert_capa_from_instance_to_figure( loins_list *loins,
                                                 lofig_list *lofig,
                                                 locon_list *locon
                                               )
{
  RCXFLOAT cmin,       cmax ;
  RCXFLOAT cminf,      cmaxf ;
  RCXFLOAT cupmin,     cupmax ;
  RCXFLOAT cdwmin,     cdwmax ;
  RCXFLOAT cupminf,    cupmaxf ;
  RCXFLOAT cdwminf,    cdwmaxf ;
  RCXFLOAT cupnommin,  cupnommax ;
  RCXFLOAT cdwnommin,  cdwnommax ;
  RCXFLOAT cupnomminf, cupnommaxf ;
  RCXFLOAT cdwnomminf, cdwnommaxf ;
  
  switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
  case ELP_CAPA_LEVEL0 :
    cmin  = rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cmax  = rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cminf = rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MIN, TRC_END,  NULL );
    cmaxf = rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MAX, TRC_END,  NULL );
    rcx_freeloconcapa( NULL, loins, locon );
    if( cmin>0.0 || cmax>0.0 || cminf || cmaxf ) {
      rcx_setloconcapa_l0( lofig, NULL, locon, TRC_HALF, cmin, cmax );
      rcx_setloconcapa_l0( lofig, NULL, locon, TRC_HALF, cminf, cmaxf );
    }
    break ;
  case ELP_CAPA_LEVEL1 :
    cupmin  = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cupmax  = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cdwmin  = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cdwmax  = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cupminf = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cupmaxf = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_END, NULL );
    cdwminf = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cdwmaxf = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_END, NULL );
    rcx_freeloconcapa( NULL, loins, locon );
    if( cupmin>0.0  || cupmax>0.0  || cdwmin>0.0  || cdwmax>0.0 ||
        cupminf>0.0 || cupmaxf>0.0 || cdwminf>0.0 || cdwmaxf>0.0    ) {
      rcx_setloconcapa_l1( lofig, NULL, locon, TRC_HALF, cupmin, cupmax, cdwmin, cdwmax );
      rcx_setloconcapa_l1( lofig, NULL, locon, TRC_END, cupmin, cupmax, cdwmin, cdwmax );
    }
    break ;
  case ELP_CAPA_LEVEL2 :
    cupmin     = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_MIN,     TRC_HALF, NULL );
    cupmax     = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_MAX,     TRC_HALF, NULL );
    cdwmin     = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_MIN,     TRC_HALF, NULL );
    cdwmax     = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_MAX,     TRC_HALF, NULL );
    cupnommin  = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cupnommax  = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cdwnommin  = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
    cdwnommax  = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
    cupnomminf = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cupnommaxf = rcx_getloconcapa( locon, TRC_SLOPE_UP,   TRC_CAPA_NOM_MAX, TRC_END, NULL );
    cdwnomminf = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_END, NULL );
    cdwnommaxf = rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_END, NULL );
    rcx_freeloconcapa( NULL, loins, locon );
    if( cupmin>0.0  || cdwnommin>0.0  || cdwnommax>0.0  || cupmax>0.0  || 
        cdwmin>0.0  || cupnommin>0.0  || cupnommax>0.0  || cdwmax>0.0  ||
        cdwminf>0.0 || cupnomminf>0.0 || cupnommaxf>0.0 || cdwmaxf>0.0    ) {
      rcx_setloconcapa_l2( lofig, NULL, locon, TRC_HALF,
                           cupmin, cupnommin, cupnommax, cupmax, 
                           cdwmin, cdwnommin, cdwnommax, cdwmax 
                         );
      rcx_setloconcapa_l2( lofig, NULL, locon, TRC_END,
                           cupmin, cupnomminf, cupnommaxf, cupmax, 
                           cdwmin, cdwnomminf, cdwnommaxf, cdwmax 
                         );
    }
    break ;
  }
 
}
