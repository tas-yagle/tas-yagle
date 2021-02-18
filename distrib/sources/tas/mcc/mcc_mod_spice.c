/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_spice.c                                               */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2001 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/

#include MCC_H
#include "mcc_util.h"
#include "mcc_mod_util.h"
#include "mcc_mod_bsim3v3.h"
#include "mcc_mod_bsim4.h"
#include "mcc_mod_psp.h"
#include "mcc_mod_ext.h"
#include "mcc_mod_mos2.h"
#include "mcc_mod_mm9.h"
#include "mcc_mod_spice.h"
#include "mcc_genspi.h"
#include "mcc_debug.h"
#include "mcc_curv.h"
#include "mcc_parse_cfg.h"

/******************************************************************************/
/* extern globals                                                             */
/******************************************************************************/

/******************************************************************************/
/* extern fonctions                                                           */
/******************************************************************************/

/******************************************************************************/
/* FONCTIONS                                                                  */
/******************************************************************************/

/****************************************************************************\
 FUNCTION : mcc_integfordw()
 Return voltage dependant capacitance value
\****************************************************************************/

double mcc_integfordw_q( double c, double p, double m, double vbd )
{
  double q ;
  double epsilon     = 1e-3 ;
  
  if( vbd < 0.0 ) {
    if( m < 1.0-epsilon || m > 1.0+epsilon ) 
      q = c*p/(m-1.0) * ( pow( 1.0-vbd/p, 1.0-m ) - 1.0 );
    else
      q = -c*p * log( 1.0-vbd/p );
  }
  else
    q = c * ( vbd + m/(p+p)*vbd*vbd );

  return q ;
}

double mcc_integfordw( double c, double vbd0, double vbd1, double p, double m)
{
  double q0 ;
  double q1 ;
  double cx ;
  
  q0 = mcc_integfordw_q( c, p, m, vbd0 );
  q1 = mcc_integfordw_q( c, p, m, vbd1 );
  cx = fabs( (q1 - q0) / (vbd1 - vbd0) ) ;

  return cx ;
}

/******************************************************************************/
/* Obtention de fichier technologique                                         */
/******************************************************************************/
mcc_technolist *mcc_gettechnofile (char *technoname)
{
    mcc_technolist *pttechnofile ;

    if ((pttechnofile = mcc_gettechno(technoname)) != NULL) 
      return ( pttechnofile ) ;

    if ( MCC_USE_SPI_PARSER )
      parsespice ( technoname );
    else
      mcc_parserfile (technoname) ;
    
    if ((pttechnofile = mcc_gettechno(technoname)) != NULL) {
      mcc_initallparam ( technoname ) ;
      return pttechnofile;
    }

    avt_errmsg(MCC_ERRMSG, "012", AVT_ERROR, technoname);
    return NULL ;
}

/******************************************************************************/
/* Destruction de fichier technologique                                       */
/******************************************************************************/
void mcc_deltechnofile(char *technoname)
{
 mcc_technolist *pttechnofile ;
 mcc_technolist *pttechnofileprev ;
 mcc_technolist *pttechnofilenext ;

 pttechnofile = MCC_HEADTECHNO ;

 if (pttechnofile->NAME == namealloc(technoname))
	{
	pttechnofileprev = pttechnofile ; 
	pttechnofile = pttechnofile->NEXT ;
    free_all_model(pttechnofileprev) ;
	mbkfree(pttechnofileprev) ;            
	MCC_HEADTECHNO = pttechnofile ;
	return ;
	}
 else
	while (pttechnofile->NEXT != NULL)
	{
		pttechnofileprev = pttechnofile ;
		pttechnofile = pttechnofile->NEXT ;
		pttechnofilenext = pttechnofile->NEXT ;

		if (pttechnofile->NAME == namealloc(technoname))
		{
			pttechnofileprev->NEXT = pttechnofilenext ;
            free_all_model(pttechnofile) ;
			mbkfree(pttechnofile) ;
			return ;
		}

	}	
 if (pttechnofile->NAME == namealloc(technoname))
	{
	pttechnofileprev->NEXT = NULL ;
    free_all_model(pttechnofile) ; 
	mbkfree(pttechnofile) ;	
	return ;
	}
  else 
	{
    avt_errmsg(MCC_ERRMSG, "013", AVT_ERROR, technoname);
	return ;
	}
}   

/******************************************************************************\

  FUNCT mcc_get_modeltype
  
\******************************************************************************/
int mcc_get_modeltype ( mcc_modellist *ptmodel, char *name )
{
  int            modeltype = MCC_NOMODEL ;
  modcfglist    *m ;
  modmodellist  *md ;
  moddeflist    *d ;
  int            value ;

  if( ptmodel->MODELTYPE >=0 )
    return ptmodel->MODELTYPE ;

  if( ptmodel->TYPE == MCC_DIODE )
    return modeltype ;

  for( m = mcc_getmodcfg() ; m ; m = m->NEXT ) {
    if( strcasecmp(V_INT_TAB[__SIM_TOOLMODEL].ENUMSTR_FUNC(V_INT_TAB[__SIM_TOOLMODEL].VALUE), m->NAME ) == 0 )
      break ;
  }
 
  md = NULL ;

  if( m ) {
  
    for( md = m->MODLIST ; md ; md = md->NEXT ) {
    
      if( md->CRITERION == MCC_CRIT_PARAM ) {
      
        if( mcc_getparamtype( ptmodel, md->VALUE.PARAM.PARAM) == MCC_SETVALUE ) {
        
          value = (int)(mcc_getparam( ptmodel, md->VALUE.PARAM.PARAM ) + 0.5 );
          
          if( value == (int)(md->VALUE.PARAM.VALUE+0.5) ) {
            modeltype = md->MCCTYPE ;
            break ;
          }
        }
      }

      if( md->CRITERION == MCC_CRIT_MODEL && name ) {
      
        if( strcasecmp( md->VALUE.MODEL, name ) == 0 ) {
          modeltype = md->MCCTYPE ;
          break ;
        }
      }
    }
  }
  
  if( !md ) {
    /* cas ou on a pas trouvé : peut etre que le simtoolmodel n'est pas
       positionné. dans ce cas on essaye de trouver une correspondance. */
    for( m = mcc_getmodcfg() ; m ; m = m->NEXT ) {
      for( md = m->MODLIST ; md ; md = md->NEXT ) {
        if( md->CRITERION == MCC_CRIT_PARAM ) {
        
          if( mcc_getparamtype( ptmodel, md->VALUE.PARAM.PARAM) == MCC_SETVALUE ) {
          
            value = (int)(mcc_getparam( ptmodel, md->VALUE.PARAM.PARAM ) + 0.5 );
            
            if( value == (int)(md->VALUE.PARAM.VALUE+0.5) ) {
              modeltype = md->MCCTYPE ;
              break ;
            }
          }
        }

        if( md->CRITERION == MCC_CRIT_MODEL && name ) {
        
          if( strcasecmp( md->VALUE.MODEL, name ) == 0 ) {
            modeltype = md->MCCTYPE ;
            break ;
          }
        }
      }
      if( md )
        break ;
    }
    if( modeltype != MCC_NOMODEL ) {
      avt_errmsg( MCC_ERRMSG, "042", AVT_ERROR, ptmodel->NAME, V_INT_TAB[__SIM_TOOLMODEL].ENUMSTR_FUNC(V_INT_TAB[__SIM_TOOLMODEL].VALUE), MCC_MOD_NAME[modeltype], m->NAME ) ;
    }
  }
  
  if( md ) {
    if (md->EXTNAME) ptmodel->USER = addptype(ptmodel->USER, MCC_MODEL_EXTNAME, md->EXTNAME);
    for( d = md->DEFAULT ; d ; d = d->NEXT ) {
      if( mcc_getparamtype( ptmodel, d->NAME ) != MCC_SETVALUE ) 
        mcc_addparam( ptmodel, d->NAME, d->VALUE, MCC_SETVALUE );
    }
  }
  
  return modeltype ;
}

/******************************************************************************/
/* Obtention du type de modele de transistor a partir d'un technofile         */  
/* On suppose que les transistors sont tous du meme model dans le technofile  */
/* C'est pourquoi on ne compare que le parametre LEVEL d'un seul transistor   */
/******************************************************************************/
int mcc_getmodeltype(char *technoname)
{
    int modeltype = MCC_NOMODEL ;
    mcc_technolist *pttechnolist ;
    mcc_modellist  *ptmodel ;

    if(!(pttechnolist = mcc_gettechnofile(technoname)))
        return(MCC_NOMODEL) ;

    ptmodel = pttechnolist->MODEL ;
    if(ptmodel == NULL)
       modeltype = MCC_MOS2 ;
    else {
     while (ptmodel) {
        if( ptmodel->TYPE != MCC_UNKNOWN && 
            mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LEVEL) == MCC_SETVALUE )
            break ;
        else
            ptmodel = ptmodel->NEXT ;
     }
     if (ptmodel)
       modeltype = mcc_get_modeltype( ptmodel, NULL );
    }
    
    return(modeltype) ;
}

/******************************************************************************/
/* initialisation des parametres de tous les modeles d'un technofile          */
/******************************************************************************/
void mcc_initallparam ( char *technoname )
{
  mcc_technolist *pttechnolist ;
  mcc_modellist  *ptmodel ;

  if(!(pttechnolist = mcc_gettechno(technoname)))
        return ;
  
  for(ptmodel = pttechnolist->MODEL ; ptmodel ; ptmodel = ptmodel->NEXT)
  {
    if ( ptmodel->TYPE == MCC_DIODE )
      mcc_initparam_diode(ptmodel);
    else
      switch ( ptmodel->MODELTYPE ) {
           case MCC_BSIM3V3  : mcc_initparam_bsim3v3(ptmodel) ;
                               break ;
           case MCC_BSIM4    : 
                               mcc_initparam_bsim4 (ptmodel) ;
                               break ;
           case MCC_MM9      : mcc_initparam_mm9 (ptmodel) ;
                               break ;
           case MCC_MOS2     : mcc_initparam_mos2 (ptmodel) ;
                               break ;
           case MCC_MPSPB    :
           case MCC_MPSP     : mcc_initparam_psp( ptmodel );
                               break ;
           case MCC_EXTMOD   : mcc_initparam_ext(ptmodel);
                               break ;
      }
  }
}

/******************************************************************************/
/* initialisation des parametres d'un modele d'un technofile                  */
/******************************************************************************/
void mcc_initmodel ( mcc_modellist *ptmodel )
{
  if (ptmodel->TYPE == MCC_DIODE)
    mcc_initparam_diode (ptmodel);
#ifdef MCC_RESI_CODE
  else if (ptmodel->TYPE == MCC_MODEL_RESI)
    mcc_initparam_resi (ptmodel);
#endif
  else
    switch ( ptmodel->MODELTYPE ) {
            case MCC_BSIM3V3  : mcc_initparam_bsim3v3(ptmodel) ;
                                break ;
            case MCC_BSIM4    :
                                mcc_initparam_bsim4 (ptmodel) ; 
                                break ;
            case MCC_MM9      : mcc_initparam_mm9 (ptmodel) ;
                                break ;
            case MCC_MOS2     : mcc_initparam_mos2 (ptmodel) ;
                                break ;
            case MCC_MPSPB    :
            case MCC_MPSP     : mcc_initparam_psp( ptmodel );
                                break ;
            case MCC_EXTMOD      : mcc_initparam_ext( ptmodel );
                                break ;
    }
}

/******************************************************************************/
/* Obtention du parametre LMIN d'un modele                                    */
/******************************************************************************/
double mcc_getLMIN (char *technoname, char *transname,
                    int transtype, int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    double lmin = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else 
        lmin = mcc_getparam_quick(ptmodel, __MCC_QUICK_LMIN) ;
    
    return lmin ;
}

/******************************************************************************/
/* Obtention du parametre LMAX d'un modele                                    */
/******************************************************************************/
double mcc_getLMAX (char *technoname, char *transname,
                    int transtype, int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    double lmax = MCC_D_LWMAX ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else 
        lmax = mcc_getparam_quick(ptmodel, __MCC_QUICK_LMAX) ;
    
    return lmax ;
}

/******************************************************************************/
/* Obtention du parametre WMIN d'un modele                                    */
/******************************************************************************/
double mcc_getWMIN (char *technoname, char *transname,
                    int transtype, int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    double wmin = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else
        wmin = mcc_getparam_quick(ptmodel, __MCC_QUICK_WMIN) ;

    return wmin ;
}

/******************************************************************************/
/* Obtention du parametre WMAX d'un modele                                    */
/******************************************************************************/
double mcc_getWMAX (char *technoname, char *transname,
                    int transtype, int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    double wmax = MCC_D_LWMAX ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else 
        wmax = mcc_getparam_quick(ptmodel, __MCC_QUICK_WMAX) ;

    return wmax ;
}

/******************************************************************************/
/* Calcul le parametre de shrink DL d'un modele                               */
/******************************************************************************/
double mcc_calcDL (char *technoname, char *transname,
                   int transtype, int transcase, 
                   double L, double W, elp_lotrs_param *lotrsparam) 
{ 
    mcc_modellist *ptmodel ;
    double dl = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM4    : dl = mcc_calcDL_bsim4 (ptmodel, lotrsparam, L, W) ;
                                  break;
              case MCC_BSIM3V3  : dl = mcc_calcDL_bsim3v3(ptmodel, L, W) ;
                                  break ;
              case MCC_MM9      : 
              case MCC_MOS2     : dl = mcc_calcDL_com(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : dl = 0.0 ;
                                  break ;
              case MCC_EXTMOD   : dl = 0.0 ;
                                  break ;
              }
    }
    return(dl) ;
}

/******************************************************************************/
/* Calcul le parametre de shrink DW d'un modele                               */
/******************************************************************************/
double mcc_calcDW (char *technoname, char *transname,
                   int transtype, int transcase, 
                   double L, double W, elp_lotrs_param *lotrsparam) 
{ 
    mcc_modellist *ptmodel ;
    double dw = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : dw = mcc_calcDW_bsim3v3(ptmodel, L, W) ;
                                  break ;
              case MCC_BSIM4    : 
                                  dw = mcc_calcDW_bsim4(ptmodel, lotrsparam, L, W) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     : dw = mcc_calcDW_com(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : dw = 0.0 ;
                                  break ;
              case MCC_EXTMOD   : dw = 0.0 ;
                                  break ;
        }
    }
    return(dw) ;
}

/******************************************************************************/
/* Calcul le parametre de shrink active DLC d'un modele                       */
/******************************************************************************/
double mcc_calcDLC (char *technoname, char *transname,
                    int transtype, int transcase, 
                    double L, double W, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double dlc = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM4    : dlc = mcc_calcDLC_bsim4 (ptmodel, lotrsparam, L, W) ;
                                  break ;
              case MCC_BSIM3V3  : dlc = mcc_calcDLC_bsim3v3(ptmodel, L, W) ;
                                  break ;
              case MCC_MM9      : 
              case MCC_MOS2     : dlc = mcc_calcDL_com(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : dlc = 0.0 ;
                                  break ;
              case MCC_EXTMOD   : dlc = 0.0 ;
                                  break ;
              }
    }
    return(dlc) ;
}

/******************************************************************************/
/* Calcul le parametre de shrink active DWC d'un modele                       */
/******************************************************************************/
double mcc_calcDWC (char *technoname, char *transname,
                    int transtype, int transcase, 
                    double L, double W, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double dwc = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : dwc = mcc_calcDWC_bsim3v3(ptmodel, L, W) ;
                                  break ;
              case MCC_BSIM4    :
                                  dwc = mcc_calcDWC_bsim4(ptmodel, lotrsparam, L, W) ;
                                  break ;
              case MCC_MM9      : 
              case MCC_MOS2     : dwc = mcc_calcDW_com(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : dwc = 0.0 ;
                                  break ;
              case MCC_EXTMOD   : dwc = 0.0 ;
                                  break ;
        }
    }
    return(dwc) ;
}

/******************************************************************************/
/* Calcul de la tension de seuil VTH d'un modele                              */
/******************************************************************************/
double mcc_calcVTH(char *technoname, char *transname,
                   int transtype, int transcase, double L, double W, 
                   double temp, double vbs, double vds,
                   elp_lotrs_param *lotrsparam, int mcclog) 
{
    mcc_modellist *ptmodel ;
    double vth = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : vth = mcc_calcVTH_bsim3v3(ptmodel, L, W, temp, 
                                                            vbs, vds,lotrsparam,mcclog) ;
                                  break ;
              case MCC_BSIM4    : 
                                  vth = mcc_calcVTH_bsim4 (ptmodel, L, W, temp,
                                                           vbs, vds,0,lotrsparam,mcclog) ;
                                  break ;
              case MCC_MM9      : vth = mcc_calcVTH_mm9(ptmodel, L, W, temp, vbs, vds, 0.0) ;
                                  break ;
              case MCC_MOS2     : vth = mcc_calcVTH_mos2(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : vth = mcc_calcVTH_psp(ptmodel, L, W, temp, vbs, vds,lotrsparam);
                                  break;
              case MCC_EXTMOD   : vth = mcc_calcVTH_ext(ptmodel, L, W, temp, vbs, vds,lotrsparam);
                                  break ;
        }
    }
    return vth ;
}

/******************************************************************************/
/* Calcul du courant IDS d'un modele                                          */
/******************************************************************************/
double mcc_calcIDS(char *technoname, char *transname,
                   int transtype, int transcase, double vbs, double vgs, double vds, 
                   double L, double W, 
                   double temp,elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double ids = 0.0;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : ids = mcc_calcIDS_bsim3v3 (ptmodel, vbs, vgs,
                                        vds, W, L, 
                                        temp,lotrsparam) ;
                                  break ;
              case MCC_BSIM4    :
                                 ids = mcc_calcIDS_bsim4 (ptmodel, vbs, vgs,
                                                          vds, W, L, 
                                                          temp,lotrsparam) ;
                                 break ;
              case MCC_MPSPB    :
              case MCC_MPSP   :
                                 ids = mcc_calcIDS_psp (ptmodel, vbs, vgs,
                                                        vds, W, L, 
                                                        temp,lotrsparam) ;
                                 break ;
              case MCC_EXTMOD   :
                                 ids = mcc_calcIDS_ext(ptmodel, vbs, vgs,
                                                        vds, W, L, 
                                                        temp,lotrsparam) ;
                                 break ;
        }
    }
    return ids;
}

/******************************************************************************/
/* Calcul de la tension degradee VDDDEG d'un modele de transistor             */
/******************************************************************************/
double mcc_calcVDDDEG(char *technoname, char *transname, 
                      int transtype, int transcase, double L, double W, 
                      double vdd, double temp, 
                      double step, elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double vdeg = 0.0 ;

    if (!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return(0.0) ;
    else 
      vdeg = mcc_calcVDDDEG_com (ptmodel, transname, L, W, 
                                 vdd, temp, step, 
                                 lotrsparam) ; 

    return vdeg ;
}

/******************************************************************************/
/* Calcul de la tension degradee VSSDEG d'un modele de transistor             */
/******************************************************************************/
double mcc_calcVSSDEG(char *technoname, char *transname, 
                      int transtype, int transcase, double L, double W, 
                      double vdd, double temp,
                      double step,elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double vdeg = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else 
      vdeg = mcc_calcVSSDEG_com (ptmodel, transname,L, W, 
                                 vdd, temp,
                                 step,
                                 lotrsparam) ; 
    return vdeg ;
}

/*******************************************************************************/
/* Calcul de capacite de grille CGD d'un transistor                            */
/* CGD represente la capacite d'oxyde par unite de surface                     */
/* Unite:  F/m^2                                                               */
/*******************************************************************************/
double mcc_calcCGD( char *technoname, 
                    char *transname, 
                    int transtype, 
                    int transcase, 
                    double L, 
                    double W, 
                    double temp, 
                    double vgs0, 
                    double vgs1, 
                    double vbs, 
                    double vds,
                    elp_lotrs_param *lotrsparam
                  )
{
    mcc_modellist *ptmodel ;
    double cgd = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : cgd = mcc_calcCGD_bsim3v3( ptmodel, L, W, temp, vgs0, vgs1, vbs, vds,lotrsparam) ;
                                  break ;
              case MCC_BSIM4    : cgd = mcc_calcCGD_bsim4( ptmodel, L, W, temp, vgs0, vgs1, vbs, vds,lotrsparam) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     : cgd = 0.0 ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : cgd = mcc_calcCGD_psp( ptmodel, L, W, temp, vgs0, vgs1, vbs, vds,lotrsparam) ;
                                  break ;
              case MCC_EXTMOD   : cgd = mcc_calcCGD_ext( ptmodel, L, W, temp, vgs0, vgs1, vbs, vds,lotrsparam) ;
                                  break ;
        }
    }
    return cgd ;
}

/*******************************************************************************\
 Calcul de capacite de grille CGSI d'un transistor
 lie au charges intrinseques de la source
 Unite:  F/m^2                                                            
\*******************************************************************************/
double mcc_calcCGSI(char *technoname, char *transname, 
                    int transtype, int transcase, double L, double W, double temp, 
                    double vgs, double vbs, double vds,elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double cgsi = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return 0.0 ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : cgsi = mcc_calcCGSI_bsim3v3 (ptmodel, L, W, temp, 
                                                               vgs, vbs, vds,lotrsparam) ;
                                  break ;
              case MCC_BSIM4    :
                                  cgsi= mcc_calcCGSI_bsim4 (ptmodel, L, W, temp, 
                                                           vgs, vbs, vds,lotrsparam) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     : cgsi= 0.0 ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : cgsi= mcc_calcCGSI_psp( ptmodel, L, W, temp, 
                                                          vgs, vbs, vds,lotrsparam) ;
              case MCC_EXTMOD   : cgsi= mcc_calcCGSI_ext( ptmodel, L, W, temp, 
                                                          vgs, vbs, vds,lotrsparam) ;
                                  break ;
        }
    }
    return cgsi ;
}

/*******************************************************************************\
 * mcc_calcCGPU
\*******************************************************************************/
double mcc_calcCGPU (char *technoname, char *transname, 
                     int transtype, int transcase,
                     double L, double W, double vdd) 
{
  double vg1,vg2,vd1,vd2,vs1,vs2;
  double cgp;

  vg1 = 0.0;
  vg2 = vdd/2.0;
  vd1 = vd2 = vdd;
  if ( transtype == MCC_NMOS )
    vs1 = vs2 = 0.0;
  else
    vs1 = vs2 = vdd;
  mcc_GetInputCapa ( technoname, transname,
                     transtype, transcase, L, W, 
                     0.0, vg1, vg2, 
                     vd1, vd2, vs1, vs2,
                     NULL, 
                     NULL, NULL, &cgp);
  return cgp*1.0e6;
}

/*******************************************************************************\
 * mcc_calcCGPD
\*******************************************************************************/
double mcc_calcCGPD (char *technoname, char *transname, 
                     int transtype, int transcase,
                     double L, double W, double vdd) 
{
  double vg1,vg2,vd1,vd2,vs1,vs2;
  double cgp;

  vg1 = vdd;
  vg2 = vdd/2.0;
  vd1 = vd2 = 0.0;
  if ( transtype == MCC_NMOS )
    vs1 = vs2 = 0.0;
  else
    vs1 = vs2 = vdd;
  mcc_GetInputCapa ( technoname, transname,
                     transtype, transcase, L, W, 
                     0.0, vg1, vg2, 
                     vd1, vd2, vs1, vs2,
                     NULL, 
                     NULL, NULL, &cgp);
  return cgp*1.0e6;
}

/*******************************************************************************/
/* Calcul de capacite de grille CGP d'un transistor                            */
/* CGP represente la capacite de recouvrement (overlap) entre G/D ou G/S       */
/* Unite:  F/m                                                                 */
/* Methode de calcul(approximative): CGP = CGDO ou CGP = CGSO                  */
/*******************************************************************************/
double mcc_calcCGP(char *technoname, char *transname, 
                   int transtype, int transcase,
                   double L, double W, double vgx, double *ptQov, 
                   elp_lotrs_param *lotrsparam, double temp )
{
    mcc_modellist *ptmodel ;
    double cgp = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : cgp = mcc_calcCGP_bsim3v3(ptmodel, vgx, L, W, ptQov) ;
                                  break ;
              case MCC_BSIM4    :
                                  cgp = mcc_calcCGP_bsim4 (ptmodel, lotrsparam, vgx, L, W, ptQov) ;
                                  break ;
              case MCC_MM9      : 
              case MCC_MOS2     : cgp = mcc_calcCGP_com(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     :
                                  cgp = mcc_calcCGP_psp (ptmodel, lotrsparam, vgx, L, W, temp, ptQov) ;
              case MCC_EXTMOD   :
                                  cgp = mcc_calcCGP_ext(ptmodel, lotrsparam, vgx, L, W, temp, ptQov);
                                  break ;
        }
    }
    return cgp ;
}

/*******************************************************************************/
/* Calcul de capacite de drain CDS d'un transistor                             */
/* CDS represente la capacite de jonction par unite de surface                 */
/* Unite:  F/m^2                                                               */
/* Methode de calcul(approximative): CDS = CJ                                  */
/*******************************************************************************/
double mcc_calcCDS(char *technoname, char *transname, 
                   int transtype, int transcase, double L, double W, double temp, 
                   double vbx1, double vbx2, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double cds = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : 
              case MCC_MM9      :
              case MCC_MOS2     : cds = mcc_calcCDS_com(ptmodel, temp, vbx1, vbx2) ;
                                  break ;
              case MCC_BSIM4    : cds = mcc_calcCDS_bsim4 (ptmodel, temp, vbx1, vbx2) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : cds = mcc_calcCDS_psp( ptmodel, lotrsparam, temp, vbx1, vbx2, L, W );
                                  break ;
              case MCC_EXTMOD   : cds = mcc_calcCDS_ext( ptmodel, lotrsparam, temp, vbx1, vbx2, L, W );
                                  break ;
        }
    }
    return cds ;
}

/*******************************************************************************/
/* Calcul de capacite de drain CDP d'un transistor                             */
/* CDP represente la capacite de jonction (sidewall junction)                  */
/* Unite:  F/m                                                                 */
/* Methode de calcul(approximative): CDP = CJSW                                */
/*******************************************************************************/
double mcc_calcCDP(char *technoname, char *transname, 
                   int transtype, int transcase, double L, double W,
                   double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double cdp = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  :
              case MCC_MM9      :
              case MCC_MOS2     : cdp = mcc_calcCDP_com(ptmodel, temp, vbx1, vbx2) ;
                                  break ;
              case MCC_BSIM4    : cdp = mcc_calcCDP_bsim4( ptmodel, lotrsparam,temp, vbx1, vbx2 );
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : cdp = mcc_calcCDP_psp( ptmodel, lotrsparam,temp, vbx1, vbx2, L, W );
                                  break ;
              case MCC_EXTMOD   : cdp = mcc_calcCDP_ext( ptmodel, lotrsparam,temp, vbx1, vbx2, L, W );
                                  break ;
        }
    }
    return cdp ;
}

/*******************************************************************************/
/* Calcul de capacite de drain CDW d'un transistor                             */
/* CDW represente la capacite de jonction (sidewall junction facing gate)      */
/* Unite:  F/m                                                                 */
/* Methode de calcul(approximative): CDW = CJSWG (bsim3v3)                     */
/* Methode de calcul(approximative): CDW = CGP   (mos level 2)                 */
/*******************************************************************************/
double mcc_calcCDW(char *technoname, char *transname, 
                   int transtype, int transcase, double L, double W, double temp, 
                   double vbx1, double vbx2, double vgx, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double cdw = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  :
              case MCC_MM9      :
              case MCC_MOS2     : cdw = mcc_calcCDW_com(ptmodel, temp, vbx1, vbx2, vgx, L, W) ; 
                                  break ;
              case MCC_BSIM4    : cdw = mcc_calcCDW_bsim4 (ptmodel, lotrsparam,temp, vbx1, vbx2, vgx, L, W) ; 
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : cdw = mcc_calcCDW_psp (ptmodel, lotrsparam,temp, vbx1, vbx2, L, W) ; 
                                  break ;
              case MCC_EXTMOD   : cdw = mcc_calcCDW_ext(ptmodel, lotrsparam,temp, vbx1, vbx2, L, W) ; 
                                  break ;
        }
    }
    return cdw ;
}

/******************************************************************************\
Function : mcc_calcDWCJ
\******************************************************************************/
double mcc_calcDWCJ (char *technoname, char *transname,
                     int transtype, int transcase,
                     double L, double W, 
                     elp_lotrs_param *lotrsparam, double temp) 
{ 
    mcc_modellist *ptmodel ;
    double dwcj = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : dwcj = mcc_calcDW_bsim3v3(ptmodel, L, W) ;
                                  break ;
              case MCC_BSIM4    : dwcj = mcc_calcDWCJ_bsim4(ptmodel, lotrsparam,L, W) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     : dwcj = mcc_calcDW_com(ptmodel) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : dwcj = mcc_calcDWCJ_psp(ptmodel, lotrsparam, temp, L, W) ;
                                  break ;
              case MCC_EXTMOD   : dwcj = mcc_calcDWCJ_ext(ptmodel, lotrsparam, temp, L, W) ;
                                  break ;
        }
    }
    return dwcj;
}


/*******************************************************************************/
/* Les valeurs des capacites de source sont egales aux capacites de drain      */
/*******************************************************************************/
/* Calcul de capacite de source CSS d'un transistor                            */
/* CSS represente la capacite de jonction par unite de surface                 */
/* Unite:  F/m^2                                                               */
/* Methode de calcul(approximative): CSS = CJ = CDS                            */
/*******************************************************************************/
double mcc_calcCSS(char *technoname, char *transname, 
                   int transtype, int transcase, double L, double W,
                   double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam )
{
    mcc_modellist *ptmodel ;
    double css = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  :
              case MCC_MM9      :
              case MCC_MOS2     : css = mcc_calcCDS_com(ptmodel, temp, vbx1, vbx2) ;
                                  break ;
              case MCC_BSIM4    : css = mcc_calcCDS_bsim4 (ptmodel, temp, vbx1, vbx2) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : css = mcc_calcCDS_psp( ptmodel, lotrsparam, temp, vbx1, vbx2, L, W );
                                  break ;
              case MCC_EXTMOD   : css = mcc_calcCDS_ext( ptmodel, lotrsparam, temp, vbx1, vbx2, L, W );
                                  break ;
        }
    }
    return css ;
}

/*******************************************************************************/
/* Calcul de capacite de source CSP d'un transistor                            */
/* CSP represente la capacite de jonction (sidewall junction)                  */
/* Unite:  F/m                                                                 */
/* Methode de calcul(approximative): CSP = CJSW = CDP                          */
/*******************************************************************************/
double mcc_calcCSP(char *technoname, char *transname, 
                   int transtype, int transcase, double L, double W,
                   double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double csp = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  :
              case MCC_MM9      :
              case MCC_MOS2     : csp = mcc_calcCDP_com(ptmodel, temp, vbx1,vbx2) ;
                                  break ;
              case MCC_BSIM4    : csp = mcc_calcCDP_bsim4 (ptmodel, lotrsparam,temp, vbx1,vbx2) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : csp = mcc_calcCDP_psp( ptmodel, lotrsparam,temp, vbx1, vbx2, L, W ) ;
                                  break ;
              case MCC_EXTMOD   : csp = mcc_calcCDP_ext( ptmodel, lotrsparam,temp, vbx1, vbx2, L, W ) ;
                                  break ;
        }
    }
    return csp ;
}

/*******************************************************************************/
/* Calcul de capacite de drain CSW d'un transistor                             */
/* CSW represente la capacite de jonction (sidewall junction facing gate)      */
/* Unite:  F/m                                                                 */
/* Methode de calcul(approximative): CSW = CJSWG (bsim3v3) = CDW               */
/* Methode de calcul(approximative): CSW = CGP = CDW  (mos2)                   */
/*******************************************************************************/
double mcc_calcCSW(char *technoname, char *transname, 
                   int transtype, int transcase, double L, double W, double temp, 
                   double vbx1, double vbx2, double vgx, elp_lotrs_param *lotrsparam)
{
    mcc_modellist *ptmodel ;
    double csw = 0.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  :
              case MCC_MM9      :
              case MCC_MOS2     : csw = mcc_calcCDW_com(ptmodel, temp, vbx1, vbx2, vgx, L, W) ; 
                                  break ;
              case MCC_BSIM4    : csw = mcc_calcCDW_bsim4 (ptmodel, lotrsparam, temp, vbx1, vbx2, vgx, L, W) ; 
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : csw = mcc_calcCDW_psp (ptmodel, lotrsparam, temp, vbx1, vbx2, L, W) ; 
                                  break ;
              case MCC_EXTMOD   : csw = mcc_calcCDW_ext(ptmodel, lotrsparam, temp, vbx1, vbx2, L, W);
                                  break ;
        }
    }
    return csw ;
}

/******************************************************************************/
/* Obtention du level du modele                                               */
/******************************************************************************/
double mcc_gettechnolevel(char *technoname)
{
    double level = 0.0 ;
    mcc_technolist *pttechnofile ;
    mcc_modellist  *ptmodel ;
    char            type = 'N';

    if(!(pttechnofile = mcc_gettechnofile(technoname)))
       return(0.0) ;

    ptmodel = pttechnofile->MODEL ;
    while (ptmodel) {
        if (ptmodel->TYPE != MCC_UNKNOWN)
            type = 'Y';
        if ((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_LEVEL) == MCC_SETVALUE) &&
                (type == 'Y'))
            break ;
        else
            ptmodel = ptmodel->NEXT ;
    }
    if (ptmodel)
       level = mcc_getparam_quick(ptmodel, __MCC_QUICK_LEVEL) ;

    return(level) ;
}

/******************************************************************************/
/* Obtention de l'index d'un modele                                           */
/******************************************************************************/
int mcc_gettransindex(char *technoname, char *transname, int transtype, 
                      int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    char *name ;

    if (( ptmodel = mcc_getmodel(technoname, transname, transtype, 
                                transcase, L, W, 0) )) {
      name = strchr(ptmodel->NAME, MCC_MOD_SEPAR) ;
      if(name == NULL)
         return (0) ;
      else
         return(atoi(name+1)) ;
    }
    avt_errmsg(MCC_ERRMSG, "014", AVT_ERROR,transname, technoname) ;
    return (0) ;
}

/******************************************************************************/
/* Obtention du nom complet (avec ou sans extension) d'un modele              */
/******************************************************************************/
char *mcc_getmodelname(char *technoname, char *transname, 
                       int transtype, int transcase, double L, double W)
{
    mcc_modellist *ptmodel ;

    if((ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)) != NULL)  
        return(mbkstrdup(ptmodel->NAME)) ;
    else
        avt_errmsg(MCC_ERRMSG, "015", AVT_ERROR, transname, technoname) ;
    return(NULL) ;
}

/******************************************************************************/
/* Calcul du parametre de XL <=> LMLT d'un modele                             */
/******************************************************************************/
double mcc_getXL (char *technoname, char *transname,
                   int transtype, int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    double lmlt = 1.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0))) {
        avt_errmsg(MCC_ERRMSG, "016", AVT_ERROR, transname, technoname) ;
        return(0.0) ;
    }
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_MM9      :
              case MCC_MOS2     : 
              case MCC_BSIM3V3  :
              case MCC_BSIM4    :
                                  lmlt = mcc_getparam_quick(ptmodel, __MCC_QUICK_LMLT) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     :
              case MCC_EXTMOD   :
                                  lmlt = 1.0 ;
                                  break ;
        }
    }
    
    return(lmlt) ;
}

/******************************************************************************/
/* Calcul du parametre de XW <=> WMLT d'un modele                             */
/******************************************************************************/
double mcc_getXW (char *technoname, char *transname,
                   int transtype, int transcase, double L, double W) 
{ 
    mcc_modellist *ptmodel ;
    double wmlt = 1.0 ;
    
    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0))) {
        avt_errmsg(MCC_ERRMSG, "017", AVT_ERROR, transname, technoname) ;
        return(0.0) ;
    }
    else  {
            switch(ptmodel->MODELTYPE) {
              case MCC_MM9      :
              case MCC_MOS2     : 
              case MCC_BSIM3V3  :
              case MCC_BSIM4    :
                                  wmlt = mcc_getparam_quick(ptmodel, __MCC_QUICK_WMLT) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     :
              case MCC_EXTMOD   :
                                  wmlt = 1.0 ;
                                  break ;
        }
    }
    
    return(wmlt) ;
}

/********************************************************************************************/
/* Fonction qui calcule la capacite de grille CGS d'un transistor                           */
/* CGS = Cox = epsox / TOX                                                                  */ 
/********************************************************************************************/
double mcc_calcCGS_com(mcc_modellist *ptmodel) 
{
    double cgs ;
    double Cox, TOX ;

    TOX = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX)   ;
    Cox = MCC_EPSOX / TOX                ;
    cgs = Cox ;

    return(cgs) ;
}
   
/********************************************************************************************/
/* Fonction qui calcule la capacite de grille CGP d'un transistor                           */
/* CGP = CGSO = CGDO                                                                        */ 
/********************************************************************************************/
double mcc_calcCGP_com(mcc_modellist *ptmodel) 
{
    double cgp ;
    double CGDO ;
    
    //CGDO = mcc_getprm(ptmodel, "CGDO")     ;
    CGDO = mcc_getprm_quick(ptmodel, __MCC_GETPRM_CGDO)     ;
    cgp  = CGDO ;

    return(cgp) ;
}

/******************************************************************************\
   Fonction qui calcule la capacite de drain CDS d'un transistor            
   Contribution surfacique.

   CDS = CJ                                                                
\******************************************************************************/
double  mcc_calcCDS_com(mcc_modellist *ptmodel, double temp, double vbx1, double vbx2) 
{
    double cds = 0.0 ;
    double T, Tnom, dT, VtT, VtTnom ;
    double Egeff_T, Egeff_Tnom ;
    double MJ, CJ, PB ;
    double CJ_T = 0.0 ;
    double PB_T = 0.0 ;
    double EG, GAP1, GAP2 ;             
    double Dpb = 0.0 ;                                      
    /* param pour diolev = 9 */        
    double VR, VDBR, CJBR, TRDIO9 ;   
    double VtTR = 0.0 ;
    double Egeff_TR = 0.0 ;
    double Ftd = 0.0 ;
    double VdbT = 0.0 ;
    double CjbT = 0.0 ;
    double Fcb = 0.0 ;
    double Vlb = 0.0 ;
    double Clb = 0.0 ;
          
    if( ptmodel->TYPE == MCC_TRANS_P ) {
      vbx1 = -vbx1 ;
      vbx2 = -vbx2 ;
    }

    T       = temp + MCC_KELVIN     ;
    Tnom    = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM)+MCC_KELVIN ;
    dT      = T - Tnom ;
    VtT     = MCC_KB*T/MCC_Q ;
    VtTnom  = MCC_KB*Tnom/MCC_Q ;

    GAP1 = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP1) ;
    GAP2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP2) ;
    EG   = mcc_getparam_quick(ptmodel, __MCC_QUICK_EG)   ;
        
    if((mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEV) == 0.0) ||
       (mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEV) == 1.0)) {
        Egeff_T = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
        Egeff_Tnom = 1.16-7.02e-4*pow(Tnom, 2.0)/(Tnom+1108.0) ;
    }
    else {
        Egeff_T = EG - GAP1*pow(T, 2.0)/(T+GAP2) ;
        Egeff_Tnom = EG - GAP1*pow(Tnom, 2.0)/(Tnom+GAP2) ;
    }
    
    
    if(mcc_getparam_quick(ptmodel, __MCC_QUICK_DIOLEV) != 9.0) {
        CJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJ)    ;
        MJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJ)    ;
        PB = mcc_getparam_quick(ptmodel, __MCC_QUICK_PB)    ;
        if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_OPTACM) == MCC_SETVALUE) &&
                (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) >= 10.0)){
            PB_T = PB ;
            CJ_T = CJ ;
        }
        else {
            if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 0.0) {
                PB_T = PB*T/Tnom-VtT*(Egeff_Tnom/VtTnom-Egeff_T/VtT+3.0*log(T/Tnom)) ;
                CJ_T = CJ*(1.0+MJ*(1.0+4.0e-4*dT-PB_T/PB)) ;
            }
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 1.0) {
                //PB_T = PB - mcc_getprm(ptmodel, "PTA")*dT ;
                //CJ_T = CJ*(1.0+mcc_getprm(ptmodel, "CTA")*dT) ;
                PB_T = PB - mcc_getprm_quick(ptmodel, __MCC_GETPRM_PTA)*dT ;
                CJ_T = CJ*(1.0+mcc_getprm_quick(ptmodel, __MCC_GETPRM_CTA)*dT) ;
            }
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 2.0) {
                //PB_T = PB - mcc_getprm(ptmodel, "PTA")*dT ;
                PB_T = PB - mcc_getprm_quick(ptmodel, __MCC_GETPRM_PTA)*dT ;
                CJ_T = CJ*pow((PB/PB_T), MJ) ;
            }
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 3.0) {
                Dpb = PB - Egeff_Tnom-3.0*VtTnom+(Egeff_Tnom-EG)*(2.0-Tnom/(Tnom+GAP2)) ;
                PB_T = PB + Dpb * (dT/Tnom) ;
                CJ_T = CJ*(1.0-0.5*(Dpb/PB)*(dT/Tnom)) ;
            }
            if ( PB_T < 0.1 )
              PB_T = 0.1;
            if ( CJ_T < 0.0 )
              CJ_T = 0.0;
        }
    /* influence de vbx */
        // seulement valable pour les vbx < 0 
        cds = mcc_integfordw ( CJ_T, vbx1, vbx2, PB_T, MJ);

    }
    else {
        VR     = mcc_getparam_quick(ptmodel, __MCC_QUICK_VR)   ;
        PB     = mcc_getparam_quick(ptmodel, __MCC_QUICK_PB)   ;
        VDBR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_VDBR) ;
        CJBR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJBR) ;
        TRDIO9 = mcc_getparam_quick(ptmodel, __MCC_QUICK_TRDIO9)+MCC_KELVIN ;
        /* Temperature update */
        VtTR     = MCC_KB*TRDIO9/MCC_Q ;
        Egeff_T  = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
        Egeff_TR = 1.16-7.02e-4*pow(TRDIO9, 2.0)/(TRDIO9+1108.0) ;
        Ftd = pow((T/TRDIO9), 1.5)*exp((Egeff_TR/(2.0*VtTR))-Egeff_T/(2.0*VtT)) ;

        VdbT = VDBR*T/TRDIO9-2.0*VtT*log(Ftd) ;
        CjbT = CJBR*pow(((VDBR-VR)/VdbT), PB) ;
        
        Fcb  = 1.0-pow(((1.0+PB)/3.0), (1.0/PB)) ;
        Vlb  = Fcb*VdbT ;
        Clb  = CjbT*pow((1.0-Fcb), -PB) ;

        cds = mcc_integfordw ( CjbT, vbx1, vbx2, VdbT, PB);
    }
        
    return cds ;
}

/******************************************************************************\
   Fonction qui calcule la capacite de drain CDP d'un transistor               
   Contribution perimetrique.

   CDP = CJSW                                                                 
\******************************************************************************/
double  mcc_calcCDP_com(mcc_modellist *ptmodel, double temp, double vbx1, double vbx2) 
{
    double cdp = 0.0 ;
    double T = 0.0 ;
    double Tnom = 0.0 ;
    double dT = 0.0 ;
    double VtT = 0.0 ;
    double VtTnom = 0.0 ;
    double Egeff_T = 0.0 ;
    double Egeff_Tnom = 0.0 ;
    double MJSW = 0.0 ; 
    double CJSW = 0.0 ; 
    double PBSW = 0.0 ;
    double CJSW_T = 0.0 ;
    double PBSW_T = 0.0 ;
    double EG = 0.0 ;
    double GAP1 = 0.0 ;
    double GAP2 = 0.0 ;
    double Dpbsw = 0.0 ;
    /* param pour diolev = 9 */        
    double VR, PS, VDSR, CJSR, TRDIO9 ;   
    double VtTR = 0.0 ;
    double Egeff_TR = 0.0 ;
    double Ftd = 0.0 ;
    double VdsT = 0.0 ;
    double CjsT = 0.0 ;
    double Fcs = 0.0 ;
    double Vls = 0.0 ;
    double Cls = 0.0 ;
    int ACM;
          
    if( ptmodel->TYPE == MCC_TRANS_P ) {
      vbx1 = -vbx1 ;
      vbx2 = -vbx2 ;
    }
    
    T       = temp + MCC_KELVIN     ;
    Tnom    = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM)+MCC_KELVIN ;
    dT      = T - Tnom ;
    VtT     = MCC_KB*T/MCC_Q ;
    VtTnom  = MCC_KB*Tnom/MCC_Q ;
    
    GAP1 = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP1) ;
    GAP2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP2) ;
    EG   = mcc_getparam_quick(ptmodel, __MCC_QUICK_EG)   ;

    if((mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEV) == 0.0) ||
       (mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEV) == 1.0)) {
        Egeff_T = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
        Egeff_Tnom = 1.16-7.02e-4*pow(Tnom, 2.0)/(Tnom+1108.0) ;
    }
    else {
        Egeff_T = EG - GAP1*pow(T, 2.0)/(T+GAP2) ;
        Egeff_Tnom = EG - GAP1*pow(Tnom, 2.0)/(Tnom+GAP2) ;
    }
    
    if(mcc_getparam_quick(ptmodel, __MCC_QUICK_DIOLEV) != 9.0) {
        CJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW)    ;
        MJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSW)    ;
        PBSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSW)    ;
        if(ptmodel->MODELTYPE == MCC_BSIM3V3) {
          if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
            //ACM = mcc_check_hsp_acm_bsim3v3 (ptmodel);
            ACM = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_ACM));
            if ( ACM >= 0 && ACM <= 3 ) 
              PBSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_PHP)    ;
          }
        }

        if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_OPTACM) == MCC_SETVALUE) &&
                (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) >= 10.0)){
            PBSW_T = PBSW ;
            CJSW_T = CJSW ;
        }
        else { 
            if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 0.0) {
                PBSW_T = PBSW*T/Tnom-VtT*(Egeff_Tnom/VtTnom-Egeff_T/VtT+3.0*log(T/Tnom)) ;
                CJSW_T = CJSW*(1.0+MJSW*(1.0+4.0e-4*dT-PBSW_T/PBSW)) ;
            }
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 1.0) {
                //PBSW_T = PBSW - mcc_getprm(ptmodel, "PTP")*dT ;
                //CJSW_T = CJSW*(1.0+mcc_getprm(ptmodel, "CTP")*dT) ;
                PBSW_T = PBSW - mcc_getprm_quick(ptmodel, __MCC_GETPRM_PTP)*dT ;
                CJSW_T = CJSW*(1.0+mcc_getprm_quick(ptmodel, __MCC_GETPRM_CTP)*dT) ;
            }
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 2.0) {
                //PBSW_T = PBSW-mcc_getprm(ptmodel, "PTP")*dT ;
                PBSW_T = PBSW-mcc_getprm_quick(ptmodel, __MCC_GETPRM_PTP)*dT ;
                CJSW_T = CJSW*pow((PBSW/PBSW_T), MJSW) ;
            }
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC) == 3.0) {
                Dpbsw = PBSW - Egeff_Tnom-3.0*VtTnom+(Egeff_Tnom-EG)*(2.0-Tnom/(Tnom+GAP2)) ;
                PBSW_T = PBSW + Dpbsw*(dT/Tnom) ; 
                CJSW_T = CJSW*(1.0-0.5*(Dpbsw/PBSW)*(dT/Tnom)) ;
            }
            if ( PBSW_T < 0.1 )
              PBSW_T = 0.1;
            if ( CJSW_T < 0.0 )
              CJSW_T = 0.0;
        }
        cdp = mcc_integfordw ( CJSW_T, vbx1, vbx2, PBSW_T, MJSW);
    }
    else {
        VR     = mcc_getparam_quick(ptmodel, __MCC_QUICK_VR)   ;
        PS     = mcc_getparam_quick(ptmodel, __MCC_QUICK_PS)   ;
        VDSR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_VDSR) ;
        CJSR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSR) ;
        TRDIO9 = mcc_getparam_quick(ptmodel, __MCC_QUICK_TRDIO9)+MCC_KELVIN ;
        /* Temperature update */
        VtTR     = MCC_KB*TRDIO9/MCC_Q ;
        Egeff_T  = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
        Egeff_TR = 1.16-7.02e-4*pow(TRDIO9, 2.0)/(TRDIO9+1108.0) ;
        Ftd = pow((T/TRDIO9), 1.5)*exp((Egeff_TR/(2.0*VtTR))-Egeff_T/(2.0*VtT)) ;

        VdsT = VDSR*T/TRDIO9-2.0*VtT*log(Ftd) ;
        CjsT = CJSR*pow(((VDSR-VR)/VdsT), PS) ;
        
        Fcs  = 1.0-pow(((1.0+PS)/3.0), (1.0/PS)) ;
        Vls  = Fcs*VdsT ;
        Cls  = CjsT*pow((1.0-Fcs), -PS) ;
        /* vbx < Vls */
        cdp = mcc_integfordw ( CjsT, vbx1, vbx2, VdsT, PS);
    }

    return cdp ;
}

/********************************************************************************************\
 Fonction qui calcule la capacite de grille CDW d'un transistor                           
 CDW = CSW =  CGP  + Capa (contribion en largeur face a la grille)                       

\********************************************************************************************/
double mcc_calcCDW_com(mcc_modellist *ptmodel, double temp, double vbx1, double vbx2, double vgx, double L, double W) 
{
    double cdw = 0.0 ;
    double CJSWG, CJGR , CJGATE ;
    double T, Tnom ;
    /* param pour CJGR */
    double VtT, VtTnom, VtTR, Egeff_T, Egeff_TR, Ftd ;
    double VdgT = 0.0 ;
    double CjgT = 0.0 ;
    double TRDIO9 ;
    double VR, VDGR, PG ; 
    double Cjgv = 0.0 ;
    /* param pour CJSWG */
    double MJSWG, PBSWG ; 
    double Cbxg = 0.0 ;
    /* param pour CJGATE */
    double TLEVC, MJSW, dT ;
    double PbswT = 0.0 ;
    double PBSW = 0.0 ;
    double CjgateT = 0.0 ;
    double CTP, GAP2, EG, PTP, Dpbsw, Egeff_Tnom ;
    /* contribution de CGP */
    double Cgp = 0.0 ;
    double Cdp = 0.0 ;
    int ACM;

    T     = temp + MCC_KELVIN                           ;
    Tnom  = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) + MCC_KELVIN  ;
    dT    = T - Tnom                                    ;
    VtT   = MCC_KB*T/MCC_Q                              ;
    VtTnom= MCC_KB*Tnom/MCC_Q                           ;

    CJSWG = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSWG)    ;
    CJGR  = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGR)     ;
    CJGATE= mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE)   ;
    if(ptmodel->MODELTYPE == MCC_BSIM3V3) {
      if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) 
        //ACM = mcc_check_hsp_acm_bsim3v3 (ptmodel);
        ACM = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_ACM));
    }

    /* Contribution de la capacite Cgp (overlap capacitance) */
    if( ptmodel->MODELTYPE == MCC_BSIM3V3) {
        // Before taking CJSWG, test if it is HSPICE models and take CJGATE instead
        if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
          if ( ACM >= 0 && ACM <= 3 ) 
            CJSWG = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE)    ;
        }
        Cgp = mcc_calcCGP_bsim3v3(ptmodel, vgx, L, W, NULL) ;
        Cdp = mcc_calcCDP_com(ptmodel, temp, vbx1, vbx2 );
    }
    else 
        Cgp = mcc_calcCGP_com(ptmodel) ;
          
    if( ptmodel->TYPE == MCC_TRANS_P ) {
      vbx1 = -vbx1 ;
      vbx2 = -vbx2 ;
    }

    /* Debut du calcul de la capacite cdw en fonction de diolev */
    if(mcc_getparam_quick(ptmodel, __MCC_QUICK_DIOLEV) == 9.0) {
        /* temp effect */
        TRDIO9 = mcc_getparam_quick(ptmodel, __MCC_QUICK_TRDIO9)+MCC_KELVIN ;
        VDGR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_VDGR)          ;
        PG     = mcc_getparam_quick(ptmodel, __MCC_QUICK_PG)            ;
        VR     = mcc_getparam_quick(ptmodel, __MCC_QUICK_VR)            ;

        VtTR     = MCC_KB*TRDIO9/MCC_Q ;
        Egeff_T  = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
        Egeff_TR = 1.16-7.02e-4*pow(TRDIO9, 2.0)/(TRDIO9+1108.0) ;
        Ftd = pow((T/TRDIO9), 1.5)*exp((Egeff_TR/(2.0*VtTR))-Egeff_T/(2.0*VtT)) ;
    
        VdgT = VDGR*T/TRDIO9-2.0*VtT*log(Ftd) ;
        CjgT = CJGR*pow(((VDGR-VR)/VdgT), PG) ;
   
        Cjgv = mcc_integfordw ( CjgT, vbx1, vbx2, VdgT, PG);

        cdw = Cgp+ Cjgv ;  
    }
    else if(ptmodel->MODELTYPE == MCC_BSIM3V3) {
        MJSWG  = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSWG);
        PBSWG  = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWG);
        if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
          if ( ACM >= 0 && ACM <= 3 ) {
            MJSWG  = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSW);
            PBSWG  = mcc_getparam_quick(ptmodel, __MCC_QUICK_PHP);
          }
        }
        /* no temp effect */

        /* vbx effect */
        Cbxg = mcc_integfordw ( CJSWG, vbx1, vbx2, PBSWG, MJSWG);

        cdw  = Cgp+ Cbxg ; 
    }
    else {
        /* temp effect */
        TLEVC = mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC)          ;
        PBSW  = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSW)           ;
        if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE )
          if ( ACM >= 0 && ACM <= 3 ) 
            PBSW  = mcc_getparam_quick(ptmodel, __MCC_QUICK_PHP)           ;

        MJSW  = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSW)           ;
        
        if(TLEVC == 0.0) {
            Egeff_T    = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
            Egeff_Tnom = 1.16-7.02e-4*pow(Tnom, 2.0)/(Tnom+1108.0) ;
            PbswT      = PBSW*T/Tnom-VtT*(Egeff_Tnom/VtTnom-Egeff_T/VtT+3.0*log(T/Tnom)) ;
            CjgateT    = CJGATE*(1.0+MJSW*(1.0+4.0e-4*dT-PbswT/PBSW)) ;
        }
        else if(TLEVC == 1.0) {
            //CTP = mcc_getprm(ptmodel, "CTP")          ;
            //PTP = mcc_getprm(ptmodel, "PTP")          ;
            CTP = mcc_getprm_quick(ptmodel, __MCC_GETPRM_CTP)          ;
            PTP = mcc_getprm_quick(ptmodel, __MCC_GETPRM_PTP)          ;
            PbswT      = PBSW - PTP*dT                  ;
            CjgateT    = CJGATE*(1.0+CTP*dT)            ;
        }
        else if(TLEVC == 2.0) {
            //PTP = mcc_getprm(ptmodel, "PTP")          ;
            PTP = mcc_getprm_quick(ptmodel, __MCC_GETPRM_PTP)          ;
            PbswT      = PBSW - PTP*dT                  ;
            CjgateT    = CJGATE*pow((PBSW/PbswT), MJSW) ;
        }
        else if(TLEVC == 3.0) {
            GAP2  = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP2)          ;
            EG    = mcc_getparam_quick(ptmodel, __MCC_QUICK_EG)            ;
            Egeff_Tnom = 1.16-7.02e-4*pow(Tnom, 2.0)/(Tnom+1108.0) ;
            Dpbsw = PBSW - Egeff_Tnom-3.0*VtTnom+(Egeff_Tnom-EG)
                    *(2.0-Tnom/(Tnom+GAP2)) ;
            PbswT   = PBSW + Dpbsw*(dT/Tnom)                  ;
            CjgateT = CJGATE*(1.0-0.5*(Dpbsw/PBSW)*(dT/Tnom)) ;
        }
        if ( PbswT < 0.1 )
          PbswT = 0.1;
        if ( CjgateT < 0.0 )
          CjgateT = 0.0;
        /* vbx effect */
        Cbxg = mcc_integfordw ( CjgateT, vbx1, vbx2, PBSW, MJSW);

        cdw = Cgp+ Cbxg ; 
    }
    MCC_CBXG = Cbxg;
    return cdw - Cdp ;
}


/**************************************************************************/
/* CALCUL de DL                                                           */
/**************************************************************************/
double  mcc_calcDL_com (mcc_modellist *ptmodel)
{
    double XL, LD, DELL, DL ;
    
    //XL = mcc_getprm(ptmodel, "XL")    ;
    //LD = mcc_getprm(ptmodel, "LD")  ;
    XL = mcc_getprm_quick(ptmodel, __MCC_GETPRM_XL)    ;
    LD = mcc_getprm_quick(ptmodel, __MCC_GETPRM_LD)  ;

    DELL = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELL)    ;

    DL   = XL - 2.0*LD + DELL ;  

    return(DL) ;
}

/**************************************************************************/
/* CALCUL de DW                                                           */
/**************************************************************************/
double  mcc_calcDW_com (mcc_modellist *ptmodel) 
{
    double XW, WD, DELW, DW ;
    
    //XW = mcc_getprm(ptmodel, "XW")    ; 
    //WD = mcc_getprm(ptmodel, "WD")      ;
    XW = mcc_getprm_quick(ptmodel, __MCC_GETPRM_XW)    ; 
    WD = mcc_getprm_quick(ptmodel, __MCC_GETPRM_WD)      ;

    DELW = mcc_getparam_quick(ptmodel, __MCC_QUICK_DELW)    ;

    DW   = XW - 2.0*WD + DELW ;  

    return(DW) ;
}

/********************************************************************************************/
/* Fonction commune d'initialisation des parametres en commun des differents modeles        */
/********************************************************************************************/
void mcc_initparam_com(mcc_modellist *ptmodel) 
{
  int versiond ;

  versiond = (int)( 10.0 * mcc_getparam_quick( ptmodel, __MCC_QUICK_VERSION ) + 0.5 );

	mcc_initparam_quick(ptmodel, __MCC_QUICK_LMIN, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LMAX, MCC_D_LWMAX, NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WMIN, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WMAX, MCC_D_LWMAX, NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LMLT, 1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WMLT, 1.0, NULL, MCC_INITVALUE)        ;

    mcc_initparam_quick(ptmodel, __MCC_QUICK_NJ, 1.0, NULL, MCC_INITVALUE)          ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_JS, 1.0e-4, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_JSW, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_XTI, 0.0, NULL, MCC_INITVALUE)         ;
    /* Initialisation des parametres relatifs a la temperature */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TNOM, V_FLOAT_TAB[__SIM_TNOM].VALUE, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TMOD, mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM), NULL, MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEV, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEVC, 0.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CTA, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CTP, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PTA, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PTP, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAP1, 7.02e-4, NULL, MCC_INITVALUE)    ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAP2, 1108.0, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEVI, 0.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_ISTMP, 10.0, NULL, MCC_INITVALUE)      ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEVR, 1.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TRD1, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TRD2, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TRS1, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TRS2, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TRSH1, 0.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TRSH2, 0.0, NULL, MCC_INITVALUE)       ;

    /* Initialisation des parametres en fonction de ACM */
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_OPTACM) == MCC_SETVALUE) {
        if(mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 0.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 0.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 0.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 2.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 3.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 1.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 1.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 1.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 2.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 3.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 2.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 2.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 2.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 2.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 5.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 3.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 3.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 3.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 2.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 0.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 10.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 0.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 0.0, NULL, MCC_INITVALUE) ;
            if( versiond == 31 )
               mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 6.0, NULL, MCC_INITVALUE) ;
            else if( versiond == 32 )
                    mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 7.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 4.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 11.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 0.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 1.0, NULL, MCC_INITVALUE) ;
            if( versiond == 31 )
               mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 6.0, NULL, MCC_INITVALUE) ;
            else if( versiond == 32 )
                    mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 7.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 4.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 12.0) {
            if((mcc_getparam_quick(ptmodel, __MCC_QUICK_CALCACM) == 0.0) ||
                (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CALCACM) == MCC_INITVALUE))
                 mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 0.0, NULL, MCC_INITVALUE) ;
            else if(mcc_getparam_quick(ptmodel, __MCC_QUICK_CALCACM) == 1.0)
                 mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 2.0, NULL, MCC_INITVALUE) ;

            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 2.0, NULL, MCC_INITVALUE) ;
            if( versiond == 31 )
               mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 6.0, NULL, MCC_INITVALUE) ;
            else if( versiond == 32 )
                    mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 7.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 4.0, NULL, MCC_INITVALUE) ;
        }
        else if (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) == 13.0) {
            mcc_initparam_quick(ptmodel, __MCC_QUICK_ALEV, 0.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_RLEV, 3.0, NULL, MCC_INITVALUE) ;
            if( versiond == 31 )
               mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 6.0, NULL, MCC_INITVALUE) ;
            else if( versiond == 32 )
                    mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 7.0, NULL, MCC_INITVALUE) ;
            mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAPLEV, 4.0, NULL, MCC_INITVALUE) ;
        }
    }

    /* Initialisation des parametres en fonction de DIOLEV */
    if(mcc_getparam_quick(ptmodel, __MCC_QUICK_DIOLEV) == 9.0) {
        mcc_initparam_quick(ptmodel, __MCC_QUICK_VR, 0.0, NULL, MCC_INITVALUE)          ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGBR, 1.0e-3, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDBR, 1.0e-3, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGSR, 1.0e-3, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDSR, 1.0e-3, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGGR, 1.0e-3, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDGR, 1.0e-3, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_NBJ, 1.0, NULL, MCC_INITVALUE)         ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_NSJ, 1.0, NULL, MCC_INITVALUE)         ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_NGJ, 1.0, NULL, MCC_INITVALUE)         ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_CJBR, 1.0e-12, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSR, 1.0e-12, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_CJGR, 1.0e-12, NULL, MCC_INITVALUE)    ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_VDBR, 1.0, NULL, MCC_INITVALUE)        ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_VDSR, 1.0, NULL, MCC_INITVALUE)        ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_VDGR, 1.0, NULL, MCC_INITVALUE)        ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 0.4, NULL, MCC_INITVALUE)          ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_PS, 0.4, NULL, MCC_INITVALUE)          ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_PG, 0.4, NULL, MCC_INITVALUE)          ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_TRDIO9, mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_TNOM), MCC_INITVALUE) ;
    }
    else {
        mcc_initparam_quick(ptmodel, __MCC_QUICK_CBD, 0.0, NULL, MCC_INITVALUE)         ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_CBS, 0.0, NULL, MCC_INITVALUE)         ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_CJGATE, 0.0, NULL, MCC_INITVALUE)      ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_MJ, 0.5, NULL, MCC_INITVALUE)          ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSW, 0.33, NULL, MCC_INITVALUE)       ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_FC, 0.5, NULL, MCC_INITVALUE)          ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_TT, 0.0, NULL, MCC_INITVALUE)          ;
        /* a verifier vrai dans eldo bsim3     */
        if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PHP) == MCC_SETVALUE)
          {
           mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSW, mcc_getparam_quick(ptmodel, __MCC_QUICK_PHP), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PHP), MCC_INITVALUE) ;
          }
        if(versiond != 31)
          {
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CJ, 0.0, NULL, MCC_INITVALUE)          ;
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSW, 0.0, NULL, MCC_INITVALUE)        ;
           mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 0.8, NULL, MCC_INITVALUE)          ;
           mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSW, mcc_getparam_quick(ptmodel, __MCC_QUICK_PB), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PB), MCC_INITVALUE) ;
           mcc_initparam_quick(ptmodel, __MCC_QUICK_PHP, mcc_getparam_quick(ptmodel, __MCC_QUICK_PB), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PB), MCC_INITVALUE) ;
          }
    }
}

/*******************************************************************************************\
FUNC : mcc_initparam_diode
\*******************************************************************************************/
void mcc_initparam_diode (mcc_modellist *ptmodel) 
{
  int level,tlev;

  if ( ptmodel->TYPE == MCC_DIODE ) {
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LEVEL, 1.0, NULL, MCC_INITVALUE);
    level = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_LEVEL));
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TNOM, V_FLOAT_TAB[__SIM_TNOM].VALUE, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TREF, V_FLOAT_TAB[__SIM_TNOM].VALUE, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TMOD, mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_TNOM), MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEV, 0.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEVC, 0.0, NULL, MCC_INITVALUE);
    switch ( level ) {
      case 1:
             //---------------
             // LEVEL 1
             //---------------
             // Scaling factor
             mcc_initparam_quick(ptmodel, __MCC_QUICK_SCALEV, 2.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_AREA, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PERIM, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_SCALM, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_SHRINK, 1.0, NULL, MCC_INITVALUE);
             // DC Related model param
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IS, 1.0e-14, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JS, 1.0e-14, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_ISA, 1.0e-14, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_ISW, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSW, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_ISP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_N, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NF, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IBV, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IK, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IKR, 0.0, NULL, MCC_INITVALUE);
             // Capacitance Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_DCAP, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJO, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJ0, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJ, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJA, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_M, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_MJ, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_EXA, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSW, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSW, 0.33, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_EXP, 0.33, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_FC, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_FCS, mcc_getparam_quick(ptmodel, __MCC_QUICK_FC), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_FC), MCC_INITVALUE) ;
             mcc_initparam_quick(ptmodel, __MCC_QUICK_KMS, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TT, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VJ, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PHI, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PHP, mcc_getparam_quick(ptmodel, __MCC_QUICK_PB), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PB), MCC_INITVALUE) ;
             // Metal and Polisillicon Capacitor Parameter
             mcc_initparam_quick(ptmodel, __MCC_QUICK_LM, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_LP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_WM, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_WP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XM, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XOI, 1.0e3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XOM, 1.0e3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XP, 0.0, NULL, MCC_INITVALUE);
             // Parasistic Resistance Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_RS, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_RB, 0.0, NULL, MCC_INITVALUE);
             // Noise Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_AF, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_KF, 1.0, NULL, MCC_INITVALUE);
             // Temperature Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_EG, 1.11, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CTA, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CTP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_GAP1, 7.02e-4, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_GAP2, 1108.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TCV, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TM1, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TM2, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TPB, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TVJ, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TPHP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TRS, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TTT1, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TTT2, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XTI, 3.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PT, 3.0, NULL, MCC_INITVALUE);
             break;
      case 2:
             //---------------
             // LEVEL 2
             //---------------
             // DC Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IS, 1.0e-14, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_ISR, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NR, 2.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_N, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NF, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IBV, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IBVL, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NBV, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NBVL, 1.0, NULL, MCC_INITVALUE);
             // Capacitance Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJO, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJ0, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_M, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_EG, 1.11, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_FC, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TT, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VJ, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_RS, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_AF, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_KF, 0.0, NULL, MCC_INITVALUE);
             // Temperature Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TRS1, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TRS2, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TBV1, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TBV2, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TIKF, 0.0, NULL, MCC_INITVALUE);
             break;
      case 3:
             //---------------
             // LEVEL 3
             //---------------
             // DC Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_EF, 1.0e-8, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_ER, mcc_getparam_quick(ptmodel, __MCC_QUICK_EF), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_EF), MCC_INITVALUE) ;
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JF, 1.0e-10, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JR, mcc_getparam_quick(ptmodel, __MCC_QUICK_JF), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_JF), MCC_INITVALUE) ;
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TOX, 100.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_L, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_W, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XW, 0.0, NULL, MCC_INITVALUE);
             break;
/*      case 4:
             //---------------
             // LEVEL 4
             //---------------
             // DC Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_AB, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_LS, 1.0e-6, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_LG, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_DTA, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TR, 25.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VR, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGBR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDBR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGSR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDBR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGGR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDGR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NB, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NS, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NG, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VB, 0.9, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJBR, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSR, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJGR, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VDBR, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VDSR, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VDGR, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 0.4, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PS, 0.4, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PG, 0.4, NULL, MCC_INITVALUE);
             break;
*/
      case 8:
             //---------------
             // LEVEL 8
             //---------------
             // DC Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOLEV, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JS, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSW, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_IS, 1.0e-14, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_N, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NDS, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VNDS, -1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_SBTH, 0.0, NULL, MCC_INITVALUE);
             // Bulk Diode Capacitance Related Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CBD, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CBS, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJ, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJGATE, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSW, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_FC, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_MJ, 0.5, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSW, 0.33, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 0.8, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSW, mcc_getparam_quick(ptmodel, __MCC_QUICK_PB), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PB), MCC_INITVALUE) ;
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TT, 0.0, NULL, MCC_INITVALUE);
             // Juncap Related Parameters (DIOLEV = 9)
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VR, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGBR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDBR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGSR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDSR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSGGR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_JSDGR, 1.0e-3, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NBJ, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NSJ, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_NGJ, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJBR, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSR, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CJGR, 1.0e-12, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VDBR, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VDSR, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_VDGR, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 0.4, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PS, 0.4, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PG, 0.4, NULL, MCC_INITVALUE);
             // Temperature Effect Related Model Parameters
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CTA, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_CTP, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PTA, 0.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_PTP, 0.0, NULL, MCC_INITVALUE);
             tlev =  MCC_ROUND(mcc_getparam_quick(ptmodel,__MCC_QUICK_TLEV));
             if ( tlev == 0 || tlev == 1 )
               mcc_initparam_quick(ptmodel, __MCC_QUICK_EG, 1.11, NULL, MCC_INITVALUE);
             else
               mcc_initparam_quick(ptmodel, __MCC_QUICK_EG, 1.16, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_GAP1, 7.02e-4, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_GAP2, 1108.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_TLEVI, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_LIS, 1.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_ISTEMP, 10.0, NULL, MCC_INITVALUE);
             mcc_initparam_quick(ptmodel, __MCC_QUICK_XTI, 0.0, NULL, MCC_INITVALUE);
             break;
      default : avt_errmsg(MCC_ERRMSG, "018", AVT_ERROR,ptmodel->NAME,level) ;
                break;
        
    }
  }
}
#ifdef MCC_RESI_CODE
// zinaps
void mcc_initparam_resi (mcc_modellist *ptmodel) 
{
  int level,tlev;

  if ( ptmodel->TYPE == MCC_MODEL_RESI ) {
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CAP, 1.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPSW, 27.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_COX, 27.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DI, mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_TNOM), MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DLR, 0.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_L, 0.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_W, 0.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_RES, 0.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_SHRINK, 0.0, NULL, MCC_INITVALUE);
    mcc_initparam_quick(ptmodel, __MCC_QUICK_L, 0.0, NULL, MCC_INITVALUE);
  }
}
#endif
/********************************************************************************************/
/* Les derating sont calcules en fct du rapport des courants maximums                       */
/********************************************************************************************/
/********************************************************************************************/
/* Fonction de calcul du derating en temperature                                            */
/* Exemple : tpfinal = tpinitial*(1.0+dT*derate_temp)                                       */
/********************************************************************************************/
double mcc_calcRapIdsTemp(char *technoname, char *transname, int transtype, 
                       int transcase, double L, double W, double vdd, 
                       double T0, double T1) 
{
    mcc_modellist *ptmodel ;
    double RapportIds = 1.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : RapportIds =  mcc_calcIDS_bsim3v3(ptmodel, 0.0, vdd, vdd, W, L, T1,NULL)
                                               /mcc_calcIDS_bsim3v3(ptmodel, 0.0, vdd, vdd, W, L, T0,NULL) ;
                                  break ;
              case MCC_BSIM4    : RapportIds =  mcc_calcIDS_bsim4 (ptmodel, 0.0, vdd, vdd, W, L, T1,NULL)
                                               /mcc_calcIDS_bsim4 (ptmodel, 0.0, vdd, vdd, W, L, T0,NULL) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : RapportIds =  mcc_calcIDS_psp (ptmodel, 0.0, vdd, vdd, W, L, T1,NULL)
                                               /mcc_calcIDS_psp (ptmodel, 0.0, vdd, vdd, W, L, T0,NULL) ;
                                  break ;
              case MCC_EXTMOD   : RapportIds =  mcc_calcIDS_ext(ptmodel, 0.0, vdd, vdd, W, L, T1,NULL)
                                               /mcc_calcIDS_ext(ptmodel, 0.0, vdd, vdd, W, L, T0,NULL) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     : RapportIds = 0.0 ; 
                                  break ;
        }
    }
    return RapportIds ;
}

/********************************************************************************************/
/* Fonction de calcul du derating en tension                                                */
/********************************************************************************************/
double mcc_calcRapIdsVolt(char *technoname, char *transname, int transtype, 
                          int transcase, double L, double W, double temp, double V0, double V1) 
{
    mcc_modellist *ptmodel ;
    double RapportIds = 1.0 ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return(0.0) ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : RapportIds = mcc_calcIDS_bsim3v3 (ptmodel, 0.0, V1, V1, W, L, temp,NULL)
                                              /mcc_calcIDS_bsim3v3 (ptmodel, 0.0, V0, V0, W, L, temp,NULL) ;
                                  break ;
              case MCC_BSIM4    : RapportIds = mcc_calcIDS_bsim4 (ptmodel, 0.0, V1, V1, W, L,temp,NULL)
                                              /mcc_calcIDS_bsim4 (ptmodel, 0.0, V0, V0, W, L,temp,NULL) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : RapportIds = mcc_calcIDS_psp (ptmodel, 0.0, V1, V1, W, L,temp,NULL)
                                              /mcc_calcIDS_psp (ptmodel, 0.0, V0, V0, W, L,temp,NULL) ;
                                  break ;
              case MCC_EXTMOD   : RapportIds = mcc_calcIDS_ext(ptmodel, 0.0, V1, V1, W, L,temp,NULL)
                                              /mcc_calcIDS_ext(ptmodel, 0.0, V0, V0, W, L,temp,NULL) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     : RapportIds = 0.0 ; 
                                  break ;
        }
    }
    return RapportIds ;
}

/******************************************************************************\

  Calcul des tensions degradees

\******************************************************************************/
struct mcc_vdeg_fn {
  mcc_modellist   *ptmodel ;
  char            *transname ;
  double           L, W ;
  double           temp ;
  elp_lotrs_param *lotrsparam ;
  double           vdd ;
};

/******************************************************************************\
FUNCTION : mcc_calcVDDDEG_com_fn
On recherche la tension degradee (Vs) qui satisfait : VGS = VTH (vd,vbs)
Vg = VDD, on obtient donc : VDD - Vs = VTH <=> VTH + Vs - VDD = 0
\******************************************************************************/

int mcc_calcVDDDEG_com_fn( struct mcc_vdeg_fn *data, double vs, double *vth )
{
  *vth = mcc_calcVTH( data->ptmodel->TECHNO->NAME,
                      data->transname, 
                      data->ptmodel->TYPE,
                      data->ptmodel->CASE,
                      data->L,
                      data->W,
                      data->temp,
                      data->lotrsparam->VBULK-vs,
                      data->vdd-vs,
                      data->lotrsparam,
                      MCC_NO_LOG
                    ) + vs - data->vdd ;
  return 1;
}

double  mcc_calcVDDDEG_com (mcc_modellist *ptmodel, char *transname, 
                            double L, double W, 
                            double vdd, double temp, double step, 
                            elp_lotrs_param *lotrsparam) 
{
    static int nbcall=0;
    struct mcc_vdeg_fn data ;
    double x0, x1, vdeg;
    int ret, it ;
    
    if (ptmodel->TYPE == MCC_PMOS) {
      avt_errmsg(MCC_ERRMSG, "019", AVT_ERROR , ptmodel->NAME) ;
      return 0.0 ;
    }
    nbcall++;
    
    data.ptmodel    = ptmodel ;
    data.transname  = transname ;
    data.L          = L ;
    data.W          = W ;
    data.temp       = temp ;
    data.lotrsparam = lotrsparam ;
    data.vdd        = vdd ;
    
    x0 = 0 ;
    x1 = vdd ;
    it = 10000 ;
    ret = mbk_dichotomie( (int(*)(void*, double, double*)) mcc_calcVDDDEG_com_fn,
                          NULL,
                          &data,
                          &x0,
                          &x1,
                          MBK_DICHO_EQUAL,
                          &it,
                          step,
                          DBL_MAX,
                          &vdeg
                        );
                        
    if( ret != MBK_DICHO_OK ) {
      fflush( stdout ) ;
      avt_errmsg(MCC_ERRMSG, "020", AVT_ERROR , ptmodel->NAME,L,W, MCC_VDDmax,MCC_VTN,MCC_VDDmax - MCC_VTN);
      return MCC_VDDmax - MCC_VTN;
    }
    return vdeg ;
}

/******************************************************************************\
FUNCTION : mcc_calcVDDDEG_com_fn
On recherche la tension degradee (Vs) qui satisfait : VGS = VTH (vds,vbs)
Vg = 0, on obtient donc : - Vs = VTH <=> VTH + Vs = 0
\******************************************************************************/

int mcc_calcVSSDEG_com_fn( struct mcc_vdeg_fn *data, double vgs, double *vth )
{
  *vth = mcc_calcVTH( data->ptmodel->TECHNO->NAME,
                      data->transname, 
                      data->ptmodel->TYPE,
                      data->ptmodel->CASE,
                      data->L,
                      data->W,
                      data->temp,
                      data->lotrsparam->VBULK-vgs,
                      -vgs,
                      data->lotrsparam,
                      MCC_NO_LOG
                    ) + vgs ;
  return 1;
}

double  mcc_calcVSSDEG_com (mcc_modellist *ptmodel, char *transname,double L, double W, 
                            double vdd, double temp, double step, 
                            elp_lotrs_param *lotrsparam) 
{
    static int nbcall=0;
    struct mcc_vdeg_fn data ;
    double x0, x1, vdeg;
    int ret, it ;
    
    if(ptmodel->TYPE == MCC_NMOS) {
      avt_errmsg(MCC_ERRMSG, "021", AVT_ERROR, ptmodel->NAME) ;
        return 0.0 ;
    }
    
    nbcall++;
    data.ptmodel    = ptmodel ;
    data.transname  = transname ;
    data.L          = L ;
    data.W          = W ;
    data.temp       = temp ;
    data.lotrsparam = lotrsparam ;
    data.vdd        = vdd ;
    
    x0 = 0 ;
    x1 = vdd ;
    it = 10000 ;
    ret = mbk_dichotomie( (int(*)(void*, double, double*)) mcc_calcVSSDEG_com_fn,
                          NULL,
                          &data,
                          &x0,
                          &x1,
                          MBK_DICHO_EQUAL,
                          &it,
                          step,
                          DBL_MAX,
                          &vdeg
                        );
                        
    if( ret != MBK_DICHO_OK ) {
      avt_errmsg(MCC_ERRMSG, "022", AVT_ERROR, ptmodel->NAME,L,W,MCC_VTP );
      return MCC_VTP;
    }
    return vdeg ;
}

/******************************************************************************\
Function : mcc_get_min_max ()
\******************************************************************************/
void mcc_get_min_max (double *max,double *min, double a, double b, double c, double d)
{
  if ( *max < a )
    *max = a;
  if ( *max < b )
    *max = b;
  if ( *max < c )
    *max = c;
  if ( *max < d )
    *max = d;
  if ( *min > a )
    *min = a;
  if ( *min > b )
    *min = b;
  if ( *min > c )
    *min = c;
  if ( *min > d )
    *min = d;
}

/******************************************************************************\
Function : mcc_PrintQint ()
\******************************************************************************/
void mcc_PrintQint (char *technoname, char *transname, 
                    int transtype, int transcase, double L, double W,
                    double temp, double vdd,elp_lotrs_param *lotrsparam, char *optnamevdd, char *optnamevss, int usechannel, char location )
{
  mcc_modellist *ptmodel ;
  double var,vbs=0;
  //double max=-1.0e50,min=+1.0e50;

  double La_Wa = 1.0 ;
  double Qg = 0.0 ;
  double Qb = 0.0 ;
  double Qs = 0.0 ;
  double Qd = 0.0 ;
  double Qtot = 0.0;
  FILE *file;
  char filename[1024];
  char spifilename[1024];
  double signe=1;
  int dec=0, decusechannel=0, j, i, k, idx, l, m;
  double vds, vgs, vfixe, vs ;
  char *nvar ;

  if( !usechannel ) 
    decusechannel=0 ;
  else
    decusechannel=-1 ;

  if( location=='G' )
    m=0 ;
  else
    m=1 ;

  if (transtype == MCC_NMOS) j=0; else j=1;
  if (!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
    return ;
  else {
    if( lotrsparam->ISVBSSET )
      vbs = lotrsparam->VBS ;
    else
      vbs = ( transtype == MCC_NMOS ) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax ; 

   for (i=0; i<4; i++) for (k=0;k<2;k++) TRS_CURVS.QINT_BSIM[j][k][m][i]=mbkalloc(sizeof(double)*TRS_CURVS.nb);
   
    switch(ptmodel->MODELTYPE) {
      case MCC_BSIM4    :
      case MCC_MPSPB    :
      case MCC_MPSP     :
      case MCC_EXTMOD   :
      case MCC_BSIM3V3  : //=======> Influence de VGS
                          switch( ptmodel->MODELTYPE ) {
                          case MCC_BSIM4 :
                            La_Wa = (L + mcc_calcDLC_bsim4 (ptmodel, lotrsparam, L, W))
                                    *(W+mcc_calcDWC_bsim4 (ptmodel, lotrsparam, L, W));
                            break ;
                          case MCC_BSIM3V3  :
                            La_Wa = (L + mcc_calcDLC_bsim3v3(ptmodel, L, W))
                                    *(W+mcc_calcDWC_bsim3v3(ptmodel, L, W));
                            break ;
                          case MCC_MPSP:
                          case MCC_MPSPB:
                          case MCC_EXTMOD:
                            La_Wa = L*W ;
                            break ;
                          }

                          for( l=0 ; l<=1 ; l++ ) {
                          
                            if( l== 0 )
                              vfixe = vdd ;
                            else
                              vfixe = 0 ;

                            if( !usechannel )
                              dec=l*5+1;
                            else
                              dec=l*4+1;

                            sprintf (filename,"VGS_%s_L%ld_W%ld_vdd", transname,(long)(L*1.0e9+0.5),(long)(W*1.0e9+0.5));
                            if( l == 0 && optnamevdd!=NULL ) 
                               sprintf( filename, "%s_%c", mcc_debug_prefix(optnamevdd), location );
                            if( l == 1 && optnamevss!=NULL )
                               sprintf( filename, "%s_%c", mcc_debug_prefix(optnamevss), location );

                            if (( file = mbkfopen (filename,"dat",WRITE_TEXT))) {
                              avt_printExecInfo(file, "#", "", "");
                              if( location == 'G' )
                                nvar = "vg" ;
                              else
                                nvar = "vd" ;
                             
                              if( transtype==MCC_NMOS ) 
                                vs = 0.0 ;
                              else
                                vs = vdd ;
                                
                              if( location == 'G' ) {
                                vds = vfixe-vs ;
                                fprintf( file, "# vds = %g\n", vds );
                              }
                              else {
                                vgs = vfixe-vs ;
                                fprintf( file, "# vgs = %g\n", vgs );
                              }
                              if (!usechannel) fprintf (file,"#    %s       Qg      Qb        Qs      Qd      Qtot\n",nvar);
                              else fprintf (file,"#    %s       Qg      Qb        Qd      Qtot\n",nvar);
                              
                              for (var = 0.0, i=0 ; var <= vdd ; var+=MCC_DC_STEP, i++) {

                                if( location == 'G' )
                                  vgs = var-vs  ; 
                                else
                                  vds = var-vs ;

                                switch( ptmodel->MODELTYPE ) {
                                case MCC_BSIM4 :
                                  mcc_calcQint_bsim4 (ptmodel, L, W,
                                                      temp, vgs, vbs, vds,
                                                      &Qg, &Qs, &Qd, &Qb,
                                                      lotrsparam);
                                  break ;
                                case MCC_MPSP:
                                case MCC_MPSPB:
                                  mcc_calcQint_psp (ptmodel, L, W,
                                                      temp, vgs, vbs, vds,
                                                      &Qg, &Qs, &Qd, &Qb,
                                                      lotrsparam);
                                  break ;
                                case MCC_EXTMOD:
                                  mcc_calcQint_ext(ptmodel, L, W,
                                                      temp, vgs, vbs, vds,
                                                      &Qg, &Qs, &Qd, &Qb,
                                                      lotrsparam);
                                  break ;
                                default :
                                  mcc_calcQint_bsim3v3 (ptmodel, L, W,
                                                      temp, vgs, vbs, vds,
                                                      &Qg, &Qs, &Qd, &Qb,
                                                      lotrsparam);
                                }
                                
                                Qg*=La_Wa;
                                Qb*=La_Wa;
                                Qs*=La_Wa;
                                Qd*=La_Wa;
                                Qtot = Qg+Qs+Qd+Qb;

                                if (transtype==MCC_NMOS) idx=i; else idx=TRS_CURVS.nb-i-1;
                                if (idx>=0 && idx<TRS_CURVS.nb)
                                {
                                  TRS_CURVS.QINT_BSIM[j][l][m][0][idx]=signe*Qg;
                                  TRS_CURVS.QINT_BSIM[j][l][m][1][idx]=signe*Qb;
                                  TRS_CURVS.QINT_BSIM[j][l][m][2][idx]=signe*Qs;
                                  TRS_CURVS.QINT_BSIM[j][l][m][3][idx]=signe*Qd;
                                }
                                if (!usechannel)
                                  fprintf (file,"%.3e %.3e %.3e %.3e %.3e %.3e\n",
                                              var,signe*Qg,signe*Qb,signe*Qs,signe*Qd,signe*Qtot);
                                else
                                  fprintf (file,"%.3e %.3e %.3e %.3e %.3e\n",
                                              var,signe*Qg,signe*Qb,signe*Qs+signe*Qd,signe*Qtot);
                                    
                              }
                              fclose (file);
                              if ((file = mbkfopen (filename,"plt",WRITE_TEXT))) {
                                avt_printExecInfo(file, "#", "", "");
                                if ( transtype == MCC_NMOS )
                                  sprintf (spifilename,"nmos_qint_%c.spice.dat",location);
                                else
                                  sprintf (spifilename,"pmos_qint_%c.spice.dat",location);
                                fprintf (file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE);
                                if( location =='G' ) {
                                  fprintf (file,"set xlabel \"VG (V)\"\n");
                                  fprintf (file,"set title \"Intrinsic charge, VDS=%gV\"\n",vds);
                                }
                                else {
                                  fprintf (file,"set xlabel \"VD (V)\"\n");
                                  fprintf (file,"set title \"Intrinsic charge, VGS=%gV\"\n",vgs);
                                }
                                fprintf (file,"set ylabel \"Q (C)\"\n");
                                fprintf (file,"set grid\n");
                                fprintf(file,"plot '%s.dat' using 1:2 title 'Qg %s'  with lines,\\\n", filename, mcc_getmccname(ptmodel)) ;
                                fprintf(file,"'%s' using 1:%d title 'Qg spice'  with lines,\\\n",mcc_debug_prefix(spifilename),dec+1);
                                fprintf(file,"'%s.dat' using 1:3 title 'Qb %s'  with lines,\\\n",filename, mcc_getmccname(ptmodel));
                                fprintf(file,"'%s' using 1:%d title 'Qb spice'  with lines,\\\n",mcc_debug_prefix(spifilename),dec+2);
                                if (!usechannel)
                                {
                                  fprintf(file,"'%s.dat' using 1:4 title 'Qs %s'  with lines,\\\n", filename, mcc_getmccname(ptmodel));
                                  fprintf(file,"'%s' using 1:%d title 'Qs spice'  with lines,\\\n", mcc_debug_prefix(spifilename),dec+3);
                                }
                                fprintf(file,"'%s.dat' using 1:%d title 'Qd %s'  with lines,\\\n", filename,5+decusechannel, mcc_getmccname(ptmodel));
                                fprintf(file,"'%s' using 1:%d title 'Qd spice'  with lines,\\\n", mcc_debug_prefix(spifilename),dec+4);
                                fprintf(file,"'%s.dat' using 1:%d title 'Qtot %s'  with lines,\\\n", filename,6+decusechannel, mcc_getmccname(ptmodel));
                                fprintf(file,"'%s' using 1:%d title 'Qtot spice'  with lines\\\n\n", mcc_debug_prefix(spifilename),dec+5 );
                                fprintf(file,"pause -1 'Hit CR to finish'");
                                fclose(file);
                              }
                            }
                          }
                          //=======> Influence de VGS a VDS=0
                          
                          break ;
      case MCC_MM9      :
      case MCC_MOS2     : 
                          break ;
    }
  }
}


/******************************************************************************\

Function :

\******************************************************************************/
void  mcc_DisplayInfos (char *technoname, char *transname,
                        int transtype, int transcase, double L, double W, 
                        double temp, double vgs, double vbs, double vds,
                        double vdd,elp_lotrs_param *lotrsparam)
{
  double vbx,cgs,cgp,cgd,cgdc,cgsi,cgsic,cds,cdp,cdw,cgsu,cgsd ;
  double cbxgu,cbxgd;
  double cbxgucal,cbxgdcal;
  double cdsu,cdsd,cdpu,cdpd,cdwu,cdwd;
  double cgscal,cgpcal,cgdcal,cgdccal,cgsical,cgsiccal,cdscal,cdpcal,cdwcal ;
  double cdsucal,cdpucal,cdwucal;
  double cdsdcal,cdpdcal,cdwdcal;
  double cgsucal,cgsdcal;
  double xw,xl,Leff, Weff, La, Wa;
  double aire,aireScaled,perim,perimScaled,Weffcj,WeffcjScaled;
  double LaWa,LaWaScaled;

  temp = temp;
  vgs = vgs;
  vdd = vdd;
  
  mcc_get_area_perim();

  //===> Dimensions
  aire = W*MCC_DIF*1.0e-6 ;
  perim = 2.0*(MCC_DIF*1.0e-6+W) ;
  xl = mcc_getXL (technoname, transname, transtype, transcase, L,W);
  xw = mcc_getXW (technoname, transname, transtype, transcase, L,W);
  Leff = L*xl + mcc_calcDL (technoname, transname, transtype, transcase, L, W, lotrsparam);
  Weff = W*xw + mcc_calcDW (technoname, transname, transtype, transcase, L, W, lotrsparam);
  La   = L*xl + mcc_calcDLC (technoname, transname, transtype, transcase, L, W, lotrsparam);
  Wa   = W*xw + mcc_calcDWC (technoname, transname, transtype, transcase, L, W, lotrsparam);
  LaWa = La*Wa;
  LaWaScaled = LaWa*1.0e12;
  Weffcj = W*xw + mcc_calcDWCJ (technoname, transname, transtype, transcase, L, W, lotrsparam, temp);


  vbx = vbs - vds;
  //===> Capacitances
  if ( transtype == MCC_NMOS ) {
    aire  = MCC_ADN*1.0e-6;
    perim = MCC_PDN*1.0e-6;
    cgsu = MCC_CGSUN;
    cgsd = MCC_CGSDN;
    cgs  = 0.5*(cgsu+cgsd);
    cgp  = MCC_CGPN;
    cgd  = MCC_CGDN;
    cgdc = MCC_CGDCN;
    cgsi = MCC_CGSIN;
    cgsic = MCC_CGSICN;
    cdsu = MCC_CDS_U_N;
    cdsd = MCC_CDS_D_N;
    cds  = 0.5*(cdsu+cdsd);
    cdpu = MCC_CDP_U_N;
    cdpd = MCC_CDP_D_N;
    cdp  = 0.5*(cdpu+cdpd);
    cdwu = MCC_CDW_U_N;
    cdwd = MCC_CDW_D_N;
    cdw  = 0.5*(cdwu+cdwd);
  }
  else {
    aire  = MCC_ADP*1.0e-6;
    perim = MCC_PDP*1.0e-6;
    cgsu = MCC_CGSUP;
    cgsd = MCC_CGSDP;
    cgs  = 0.5*(cgsu+cgsd);
    cgp  = MCC_CGPP;
    cgd  = MCC_CGDP;
    cgdc = MCC_CGDCP;
    cgsi  = MCC_CGSIP;
    cgsic = MCC_CGSICP;
    cdsu = MCC_CDS_U_P;
    cdsd = MCC_CDS_D_P;
    cds  = 0.5*(cdsu+cdsd);
    cdpu = MCC_CDP_U_P;
    cdpd = MCC_CDP_D_P;
    cdp  = 0.5*(cdpu+cdpd);
    cdwu = MCC_CDW_U_P;
    cdwd = MCC_CDW_D_P;
    cdw  = 0.5*(cdwu+cdwd);
  }
  cbxgu = MCC_CBXGU;
  cbxgd = MCC_CBXGD;

  //==> display Infos :
  avt_log(LOGMCC,2,"Dimensions : L      = %8.3g  W      = %8.3g  Aire          = %8.3g  Perim = %8.3g\n",L,W,aire,perim);
  avt_log(LOGMCC,2,"           : Leff   = %8.3g  Weff   = %8.3g  Aire (eff)    = %8.3g\n",Leff,Weff,Leff*Weff);
  avt_log(LOGMCC,2,"           : Lactif = %8.3g  Wactif = %8.3g  Aire (active) = %8.3g\n",La,Wa,La*Wa);
  avt_log(LOGMCC,2,"           : Weffcj = %8.3g\n\n",Weffcj);
  WeffcjScaled = Weffcj*1.0e6;
  aireScaled = aire*1.0e12;
  perimScaled = perim*1.0e6;

  cgscal = cgs*LaWaScaled;
  cgsucal = cgsu*LaWaScaled;
  cgsdcal = cgsd*LaWaScaled;
  cgpcal = cgp*WeffcjScaled;
  cgpcal = cgp*Wa*1.0e6;
  cgdcal = cgd*LaWaScaled;
  cgdccal = cgdc*LaWaScaled;
  cgsical = cgsi*LaWaScaled;
  cgsiccal = cgsic*LaWaScaled;
  cdscal = cds*aireScaled;
  cdsucal = cdsu*aireScaled;
  cdsdcal = cdsd*aireScaled;
  cdpcal = cdp*perimScaled;
  cdpucal = cdpu*perimScaled;
  cdpdcal = cdpd*perimScaled;
  cbxgucal = cbxgu*WeffcjScaled;
  cbxgdcal = cbxgd*WeffcjScaled;
  cdwcal = cdw*WeffcjScaled;
  cdwucal = cdwu*WeffcjScaled;
  cdwdcal = cdwd*WeffcjScaled;
  cdwcal = cdw*Weff*1.0e6;

  avt_log(LOGMCC,2,"Capacitances (pF) : CGS  = %g \n",cgscal);
  avt_log(LOGMCC,2,"                  : CGSU = %g \n",cgsucal);
  avt_log(LOGMCC,2,"                  : CGSD = %g \n",cgsdcal);
  avt_log(LOGMCC,2,"                  : CGP (Weffcj) = %g \n",cgpcal);
  avt_log(LOGMCC,2,"                  : CGP (Wactif) = %g \n",cgpcal);
  avt_log(LOGMCC,2,"                  : CGD  = %g \n",cgdcal);
  avt_log(LOGMCC,2,"                  : CGDC = %g \n",cgdccal);
  avt_log(LOGMCC,2,"                  : CGSI = %g \n",cgsical);
  avt_log(LOGMCC,2,"                  : CGSIC = %g \n",cgsiccal);
  avt_log(LOGMCC,2,"                  : CDS  = %g \n",cdscal);
  avt_log(LOGMCC,2,"                  : CDSU = %g \n",cdsucal);
  avt_log(LOGMCC,2,"                  : CDSD = %g \n",cdsdcal);
  avt_log(LOGMCC,2,"                  : CDP  = %g \n",cdpcal);
  avt_log(LOGMCC,2,"                  : CDPU = %g \n",cdpucal);
  avt_log(LOGMCC,2,"                  : CDPD = %g \n",cdpdcal);
  avt_log(LOGMCC,2,"                  : CBXGU (Weffcj) = %g \n",cbxgucal);
  avt_log(LOGMCC,2,"                  : CBXGD (Weffcj) = %g \n",cbxgdcal);
  avt_log(LOGMCC,2,"                  : CDW (Weffcj) = %g \n",cdwcal);
  avt_log(LOGMCC,2,"                  : CDWU (Weffcj) = %g \n",cdwucal);
  avt_log(LOGMCC,2,"                  : CDWD (Weffcj) = %g \n",cdwdcal);
  avt_log(LOGMCC,2,"                  : CDW (Weff) = %g \n",cdwcal);
  avt_log(LOGMCC,2,"Capacitances (pF) ==> Total Grid capa : %g\n",cgscal+2.0*cgpcal);
  avt_log(LOGMCC,2,"                  ==> Total Grid (UP) capa : %g\n",cgsucal+2.0*cgpcal);
  avt_log(LOGMCC,2,"                  ==> Total Grid (DN) capa : %g\n",cgsdcal+2.0*cgpcal);
  avt_log(LOGMCC,2,"                  ==> Total Source/Drain capa : %g\n",
                                                    cdscal+cdpcal+cdwcal);
  avt_log(LOGMCC,2,"                  ==> Total Source/Drain (UP) capa : %g\n",
                                                    cdsucal+cdpucal+cdwucal);
  avt_log(LOGMCC,2,"                  ==> Total Source/Drain (DN) capa : %g\n",
                                                    cdsdcal+cdpdcal+cdwdcal);
  avt_log(LOGMCC,2,"                  ==> Total Source/Drain without CGP (UP) capa : %g\n",
                                                    cdsucal+cdpucal+cbxgucal);
  avt_log(LOGMCC,2,"                  ==> Total Source/Drain without CGP (DN) capa : %g\n",
                                                    cdsdcal+cdpdcal+cbxgdcal);
  // Min and Max Input Capacitances
  if ( transtype == MCC_NMOS ) {
      avt_log(LOGMCC,2,"== Model %s NMOS (L = %gU W = %gU)  ==\n",MCC_TNMODEL,MCC_LN,MCC_WN);
      avt_log(LOGMCC,2,"*************************\n");
      avt_log(LOGMCC,2,"*    UP transition      *\n");
      avt_log(LOGMCC,2,"*************************\n");
   
      avt_log(LOGMCC,2,"--> input capa Min  = %g pF\n",MCC_CGSU_N_MIN*LaWa+MCC_CGPU_N_MIN*2.0*Wa);
      avt_log(LOGMCC,2,"--> input capa Max  = %g pF\n",MCC_CGSU_N_MAX*LaWa+MCC_CGPU_N_MAX*2.0*Wa);
   
      avt_log(LOGMCC,2,"*************************\n");
      avt_log(LOGMCC,2,"*    DOWN transition    *\n");
      avt_log(LOGMCC,2,"*************************\n");
      avt_log(LOGMCC,2,"--> input capa Min  = %g pF\n",MCC_CGSD_N_MIN*LaWa+MCC_CGPD_N_MIN*2.0*Wa);
      avt_log(LOGMCC,2,"--> input capa Max  = %g pF\n\n",MCC_CGSD_N_MAX*LaWa+MCC_CGPD_N_MAX*2.0*Wa);
  }
  else {
    avt_log(LOGMCC,2,"== Model %s PMOS (L = %gU W = %gU ) ==\n",MCC_TPMODEL,MCC_LP,MCC_WP);
    avt_log(LOGMCC,2,"*************************\n");
    avt_log(LOGMCC,2,"*    UP transition      *\n");
    avt_log(LOGMCC,2,"*************************\n");
    avt_log(LOGMCC,2,"--> input capa Min  = %g pF\n",MCC_CGSU_P_MIN*LaWa+MCC_CGPU_P_MIN*2.0*Wa);
    avt_log(LOGMCC,2,"--> input capa Max  = %g pF\n",MCC_CGSU_P_MAX*LaWa+MCC_CGPU_P_MAX*2.0*Wa);
  
    avt_log(LOGMCC,2,"*************************\n");
    avt_log(LOGMCC,2,"*    DOWN transition    *\n");
    avt_log(LOGMCC,2,"*************************\n");
    avt_log(LOGMCC,2,"--> input capa Min  = %g pF\n",MCC_CGSD_P_MIN*LaWa+MCC_CGPD_P_MIN*2.0*Wa);
    avt_log(LOGMCC,2,"--> input capa Max  = %g pF\n\n",MCC_CGSD_P_MAX*LaWa+MCC_CGPD_P_MAX*2.0*Wa);
  }
}

/******************************************************************************\

  Function : mcc_calcQint
    
  Compute Intrinsic Charges.

\******************************************************************************/
void mcc_calcQint (char *technoname, char *transname,
                   int transtype, int transcase, double L, double W, 
                   double temp, double vgs,double vbs, double vds,
                   double *ptQg,double *ptQs, double *ptQd, double *ptQb,
                   elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return ;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : mcc_calcQint_bsim3v3 (ptmodel, L, W, temp, vgs,vbs,vds,ptQg,ptQs,ptQd,ptQb,lotrsparam) ;
                                  break ;
              case MCC_BSIM4    : mcc_calcQint_bsim4 (ptmodel, L, W, temp, vgs,vbs,vds,ptQg,ptQs,ptQd,ptQb,lotrsparam) ;
                                  break ;
              case MCC_MPSPB    :
              case MCC_MPSP     : mcc_calcQint_psp (ptmodel, L, W, temp, vgs,vbs,vds,ptQg,ptQs,ptQd,ptQb,lotrsparam) ;
                                  break ;
              case MCC_EXTMOD   : mcc_calcQint_ext(ptmodel, L, W, temp, vgs,vbs,vds,ptQg,ptQs,ptQd,ptQb,lotrsparam) ;
                                  break ;
              case MCC_MM9      :
              case MCC_MOS2     :
                                  break ;
        }
    }
}

double mcc_calcCGS( char *technoname, 
                    char *transname,
                    int transtype, 
                    int transcase, 
                    double L, 
                    double W, 
                    double temp, 
                    double vgsi, 
                    double vgsf, 
                    double vdsi, 
                    double vdsf, 
                    elp_lotrs_param *lotrsparam
                  )
{
  mcc_modellist *ptmodel ;
  double Qinit=0.0,Qfinal=0.0,cgs=0.0,vbs;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return(0.0) ;
  else {
      if( lotrsparam->ISVBSSET )
        vbs = lotrsparam->VBS ;
      else
        vbs = ( transtype == MCC_NMOS ) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax ; 
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  :
            case MCC_BSIM4    :
            case MCC_MPSP     :
            case MCC_MPSPB    :
            case MCC_EXTMOD   :
                                // DOWN transition
                                if ( mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART) < 0.0)
                                  return 1.0e-30;
                                mcc_calcQint (technoname, transname,
                                              transtype,transcase,L, W,
                                              temp, vgsi, vbs, vdsi,
                                              &Qinit, NULL, NULL, NULL,lotrsparam);
                                mcc_calcQint (technoname, transname,
                                              transtype,transcase,L, W,
                                              temp, vgsf, vbs, vdsf, 
                                              &Qfinal, NULL, NULL, NULL,lotrsparam);
                                cgs = fabs((Qfinal-Qinit)/(vgsf-vgsi));
                                break ;
            case MCC_MM9      :
            case MCC_MOS2     : cgs = mcc_calcCGS_com (ptmodel) ;
                                break ;
      }
  }
  return cgs;
}

/******************************************************************************\

  Funciton : mcc_calcCGSD

  Compute CGS for a down transition

\******************************************************************************/
double mcc_calcCGSD (char *technoname, char *transname,
                   int transtype, int transcase, double L, double W, 
                   double temp, double vdd, double vfinal, int vdsnull, elp_lotrs_param *lotrsparam)
{
  mcc_modellist *ptmodel ;
  double Qinit=0.0,Qfinal=0.0,cgs=0.0,vbs;
  double vdsi,vdsf;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return(0.0) ;
  else {
      if( lotrsparam->ISVBSSET )
        vbs = lotrsparam->VBS ;
      else
        vbs = ( transtype == MCC_NMOS ) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax ; 
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  :
            case MCC_BSIM4    :
            case MCC_MPSP     :
            case MCC_MPSPB    :
            case MCC_EXTMOD   :
                                // DOWN transition
                                if ( mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART) < 0.0)
                                  return 1.0e-30;
                                if (ptmodel->TYPE == MCC_NMOS) {
                                  if ( vdsnull ) {
                                   vdsi = 0.0;
                                   vdsf = 0.0;
                                  }
                                  else {
                                   vdsi = 0.0;
                                   //vdsf = vdd/2.0 - MCC_VTN;
                                   vdsf=vdsi;
                                  }
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, vdd, vbs, vdsi,
                                                 &Qinit, NULL, NULL, NULL,lotrsparam);
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, vfinal, vbs, vdsf, 
                                                 &Qfinal, NULL, NULL, NULL,lotrsparam);
                                }
                                else {
                                  if ( vdsnull ) {
                                   vdsi = 0.0;
                                   vdsf = 0.0;
                                  }
                                  else {
                                   vdsi = vdd;
                                   //vdsf = vdd/2.0 + MCC_VTP;
                                   vdsf=vdsi;
                                  }
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, 0.0, vbs, vdsi,
                                                 &Qinit, NULL, NULL, NULL,
                                                 lotrsparam);
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, vfinal, vbs, vdsf,
                                                 &Qfinal, NULL, NULL, NULL,
                                                 lotrsparam);
                                }
                                cgs = fabs((Qfinal-Qinit)/(vdd/2.0));
                                break ;
            case MCC_MM9      :
            case MCC_MOS2     : cgs = mcc_calcCGS_com (ptmodel) ;
                                break ;
      }
  }
  return cgs;
}

/******************************************************************************\

  Funciton : mcc_calcCGSU

  Compute CGS for an up transition

\******************************************************************************/
double mcc_calcCGSU (char *technoname, char *transname,
                   int transtype, int transcase, double L, double W, 
                   double temp, double vdd, double vfinal, int vdsnull,elp_lotrs_param *lotrsparam)
{
  mcc_modellist *ptmodel ;
  double Qinit=0.0,Qfinal=0.0,cgs=0.0,vbs;
  double vdsi,vdsf;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
    return 0.0;

  else {
      if( lotrsparam->ISVBSSET )
        vbs = lotrsparam->VBS ;
      else
        vbs = ( transtype == MCC_NMOS ) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax ; 
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  :
            case MCC_MPSP     :
            case MCC_MPSPB    :
            case MCC_EXTMOD   :
            case MCC_BSIM4    : // UP transition
                                if ( mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART) < 0.0)
                                  return 1.0e-30;
                                if (ptmodel->TYPE == MCC_NMOS) {
                                  if ( vdsnull ) {
                                   vdsi = 0.0;
                                   vdsf = 0.0;
                                  }
                                  else {
                                   vdsi = vdd;
                                   //vdsf = MCC_VTN + vdd/2.0;
                                   vdsf=vdsi;
                                  }
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, 0.0, vbs, vdsi,
                                                 &Qinit, NULL, NULL, NULL,
                                                 lotrsparam);
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, vfinal, vbs, vdsf, 
                                                 &Qfinal, NULL, NULL, NULL,
                                                 lotrsparam);
                                }
                                else {
                                  if ( vdsnull ) {
                                   vdsi = 0.0;
                                   vdsf = 0.0;
                                  }
                                  else {
                                   vdsi = 0.0;
                                   //vdsf = -MCC_VTP + vdd/2.0;
                                   vdsf=vdsi;
                                  }
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, vdd, vbs, vdsi,
                                                 &Qinit, NULL, NULL, NULL,
                                                 lotrsparam);
                                   mcc_calcQint (technoname, transname,
                                                 transtype,transcase,L, W,
                                                 temp, vfinal, vbs, vdsf,
                                                 &Qfinal, NULL, NULL, NULL,
                                                 lotrsparam);
                                }
                                cgs = fabs((Qfinal-Qinit)/(vdd/2.0));
                                break ;
            case MCC_MM9      :
            case MCC_MOS2     : cgs = mcc_calcCGS_com (ptmodel) ;
                                break ;
      }
  }
  return cgs;
}

double mcc_calcCGPO( char *technoname, char *transname,
                     int transtype, int transcase, double L, double W, 
                     double temp, double vg,
                     double vd1, double vd2,
                     elp_lotrs_param *lotrsparam )
{
  double vgd1, vgd2 ;
  double ptQov1, ptQov2 ;
  double cgpo ;
  
  vgd1 = vg - vd1 ;
  vgd2 = vg - vd2 ;
  mcc_calcCGP (technoname, transname, transtype, transcase, L, W, vgd1, &ptQov1, lotrsparam, temp );
  mcc_calcCGP (technoname, transname, transtype, transcase, L, W, vgd2, &ptQov2, lotrsparam, temp );
  cgpo =fabs( ( ptQov2 - ptQov1 ) / ( vgd2 - vgd1 ) );

  return cgpo ;
  
}

/******************************************************************************\

 Function : mcc_GetInputCapa

 CGS, CGD, CGP

\******************************************************************************/

void mcc_GetInputCapa ( char *technoname, char *transname,
                        int transtype, int transcase, double L, double W, 
                        double temp, double vg1, double vg2, 
                        double vd1, double vd2, double vs1, double vs2,
                        elp_lotrs_param *lotrsparam, 
                        double *ptcgs, double *ptcgd, double *ptcgp)
{
  double delta_vg;
  double vgs1,vgs2,vds1,vds2,vgd1,vgd2;
  double ptQg1,ptQd1,ptQg2,ptQd2;
  double ptQov1,ptQov2;
  double cgp1,cgp2,cgs,cgp,cgd,vbs;

  delta_vg = vg2-vg1;
  vgs1 = vg1-vs1;
  vgs2 = vg2-vs2;
  vgd1 = vg1-vd1;
  vgd2 = vg2-vd2;
  vds1 = vd1-vs1;
  vds2 = vd2-vs2;

  if ( ptcgs || ptcgd ) {
    if ( lotrsparam ) {
      if( lotrsparam->ISVBSSET )
        vbs = lotrsparam->VBS ;
      else
        vbs = ( transtype == MCC_NMOS ) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax ; 
    }
    else
      vbs = 0.0;
    
    // ===> initial charges
    mcc_calcQint (technoname, transname,
                  transtype, transcase, L, W, 
                  temp, vgs1,vbs,vds1,
                  &ptQg1,NULL, &ptQd1, NULL,
                  lotrsparam);
    
    // ===> final charges
    mcc_calcQint (technoname, transname,
                  transtype, transcase, L, W, 
                  temp, vgs2,vbs,vds2,
                  &ptQg2,NULL, &ptQd2, NULL,
                  lotrsparam);
    
    cgs = ( ptQg2-ptQg1 ) / delta_vg;
    cgd = fabs (( ptQd2-ptQd1 ) / delta_vg );
  }
  
  if ( ptcgp ) {
    //===> overlap capa
    // drain side
    cgp1 = mcc_calcCGP (technoname, transname, 
                        transtype, transcase, L, W, vgd1,&ptQov1,lotrsparam,temp);
    cgp2 = mcc_calcCGP (technoname, transname, 
                        transtype, transcase, L, W, vgd2,&ptQov2,lotrsparam,temp);
    cgp = (ptQov2-ptQov1) / delta_vg;
    // source side
    cgp1 = mcc_calcCGP (technoname, transname, 
                        transtype, transcase, L, W, vgs1,&ptQov1,lotrsparam,temp);
    cgp2 = mcc_calcCGP (technoname, transname, 
                        transtype, transcase, L, W, vgs2,&ptQov2,lotrsparam,temp);
    cgp += (ptQov2-ptQov1) / delta_vg;
    cgp *= 0.5; // mult by 0.5 because we give the source and drain olverlap capa
                  // cgp will be mult by 2.0 in the future to have the total overlap capa of 
                  // a transistor
  }


  /*--------------------------------------------------------*\
   Caution : cgp is taking account of drain and source ovelap charges,
   so we only need to mult this capa by Wa and not by 2.0*Wa.
   We used to approximate this capa by considering that : Qov(drain) = Qov(source)
   which explains the mult factor 2.0. Now, we distinguish these 2 charges
  \*--------------------------------------------------------*/
 
  if ( ptcgs ) *ptcgs = fabs(cgs);
  if ( ptcgp ) *ptcgp = fabs(cgp);
  if ( ptcgd ) *ptcgd = fabs(cgd);
}

/******************************************************************************\
 FUNCTION : mcc_calc_vt ( temp )                                                
\******************************************************************************/
double mcc_calc_vt (double temp)
{
  double T = temp + MCC_KELVIN;
  double VtT;

  VtT    = MCC_KB * T / MCC_Q    ;
  return VtT;
}

/******************************************************************************\
 FUNCTION : mcc_calc_eg()                                                

 Energy band gap of Si
\******************************************************************************/
double mcc_calc_eg(double temp) 
{
  double Eg = 0.0;
  double T = temp + MCC_KELVIN;

  Eg = 1.16 - ((7.02e-4 * T*T) / (T+1108.0));

  return Eg;
}

/******************************************************************************\
 *
 * FUNC : mcc_compute_RD_RS 
   RSeff and RDeff computation
\******************************************************************************/
void mcc_compute_RD_RS ( mcc_modellist *ptmodel, double Weff,
                         double *ptRS, double *ptRD, elp_lotrs_param *lotrsparam )
{
  int RLEV;
  double RSH, RSC, RDC;
  double RS, RD, LDscal, LDIFscal, HDIFscal;
  double DW,DWscal, Wj;
  //double LDIFeff;
  double scalm, LD, LDIF, HDIF, WMLT;
  double nrs, nrd, M;

  if (!ptmodel || !lotrsparam) return;

  nrs = lotrsparam->PARAM[elpNRS];
  nrd = lotrsparam->PARAM[elpNRD];
  M = lotrsparam->PARAM[elpM];

  scalm = 1; //TODO: check how it can take other value...

  RLEV = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_RLEV));
  RSH = mcc_getparam_quick(ptmodel,__MCC_QUICK_RSH);
  RSC = mcc_getparam_quick(ptmodel,__MCC_QUICK_RSC);
  RDC = mcc_getparam_quick(ptmodel,__MCC_QUICK_RDC);
  RS  = mcc_getparam_quick(ptmodel,__MCC_QUICK_RS);
  RD  = mcc_getparam_quick(ptmodel,__MCC_QUICK_RD);
  LD  = mcc_getparam_quick(ptmodel,__MCC_QUICK_LD);
  DW  = mcc_getparam_quick(ptmodel,__MCC_QUICK_DW);
  LDIF= mcc_getparam_quick(ptmodel,__MCC_QUICK_LDIF);
  HDIF= mcc_getparam_quick(ptmodel,__MCC_QUICK_HDIF);
  WMLT= mcc_getparam_quick(ptmodel,__MCC_QUICK_WMLT);

  DWscal = DW*scalm;

  switch ( RLEV ) {
    case 0 : if ( ptRS ) {
               if ( nrs > 0.0 ) *ptRS = (RSH*nrs + RSC) / M;
               else *ptRS = ( RS + RSC ) / M;
             }
             if ( ptRD ) {
               if ( nrd > 0.0 ) *ptRD = (RSH*nrd + RDC) / M;
               else *ptRD = ( RD + RDC ) / M;
             }
             break;
    case 1 : LDscal = LD*scalm;
             LDIFscal = LDIF*scalm;
             Wj = Weff+DWscal;
             if ( ptRS )
               *ptRS = (RS*(LDscal+LDIFscal)/(M*Wj)+(nrs*RSH+RSC)/M);
             if ( ptRD )
               *ptRD = (RD*(LDscal+LDIFscal)/(M*Wj)+(nrd*RSH+RDC)/M);
             break;
    case 2 :
    case 3 : LDscal   = LD*scalm;
             LDIFscal = LDIF*scalm;
             HDIFscal = HDIF*scalm*WMLT;
             Wj = Weff+DWscal;
             if ( ptRS ) {
               if ( nrs > 0.0 )
                 *ptRS = (RS*(LDscal+LDIFscal)/(M*Wj) + (RSH*nrs+RSC)/M );
               else
                 *ptRS = (RS*(LDscal+LDIFscal)/(M*Wj) + (HDIFscal*RSH)/(M*Wj) + RSC/M );
             }
             if ( ptRD ) {
               if ( nrd > 0.0 )
                 *ptRD = (RD*(LDscal+LDIFscal)/(M*Wj) + (RSH*nrs+RDC)/M );
               else
                 *ptRD = (RD*(LDscal+LDIFscal)/(M*Wj) + (HDIFscal*RSH)/(M*Wj) + RDC/M );
             }
             break;
    case 4 : LDscal   = LD*scalm; 
             LDIFscal = LDIF*scalm;
             HDIFscal = HDIF*scalm*WMLT;
             Wj = Weff+DWscal;
             if ( ptRS ) {
               if ( LDIFscal < 0.0 )
                 *ptRS = RS/M;
               else if ( nrs > 0.0 )
                 *ptRS = (RS*(LDscal+LDIFscal)/(M*Weff)+(nrs*RSH)/M);
               else
                 *ptRS = (RS*(LDscal+LDIFscal)+HDIFscal*RSH)/(M*Weff);
             }
             if ( ptRD ) {
               if ( LDIFscal < 0.0 )
                 *ptRD = RD/M;
               else if ( nrd > 0.0 )
                 *ptRD = (RD*(LDscal+LDIFscal)/(M*Weff)+(nrd*RSH)/M);
               else
                 *ptRD = (RD*(LDscal+LDIFscal)+HDIFscal*RSH)/(M*Weff);
             }
             break;
    case 5 : if ( ptRS ) {
               if ( RS > 0.0 ) *ptRS = RS/M;
               else *ptRS = nrs*RSH/M;
             }
             if ( ptRD ) {
               if ( RD > 0.0 ) *ptRD = RD/M;
               else *ptRD = nrd*RSH/M;
             }
             break;
    case 6 : if ( ptRS ) {
               if ( nrs > 0.0 ) *ptRS = nrs*RSH/M;
               else *ptRS = RS/M;
             }
             if ( ptRD ) {
               if ( nrd > 0.0 ) *ptRD = nrd*RSH/M;
               else *ptRD = RD/M;
             }
             break;
             /*
    case 7 : LDIFeff = 
             if ( ptRS ) {
               if ( nrs > 0.0 ) *ptRS = nrs*RSH/M;
               else *ptRS = LDIFeff/(M*Weff);
             }
             if ( ptRD ) {
               if ( nrd > 0.0 ) *ptRD = nrd*RSH/M;
               else *ptRD = LDIFeff/(M*Weff);
             }
             break;
             */
    default: if ( ptRS ) *ptRS = 0.0;
             if ( ptRD ) *ptRD = 0.0;
             break;
  }
}

/******************************************************************************\
 *
 * FUNC : mcc_calcDioCapa

   return the total capacitance from a diode model
\******************************************************************************/
double mcc_calcDioCapa ( char *technoname, char *dioname,
                         int modtype, int modcase, 
                         double Va, double Vc, double temp,
                         double area, double perim
                       )
{
  mcc_modellist *ptmodel ;
  double Capa = 0.0 ;

  if ( (ptmodel = mcc_getmodel(technoname, dioname, modtype, modcase, area, perim, 0)) ) 
    Capa = mcc_dio_calcCapa ( ptmodel, Va, Vc, temp, area, perim);

  return Capa;
}

/******************************************************************************\
 *
 * FUNC : mcc_dio_calcCDEP
 * CDEP is Depletion capacitance
   Va: Anode Voltage
   Vc: Cathode Voltage

   return the total capacitance: Capa = CDEPa + CDEPp + CIDFF + CMETAL + CPOLY 
\******************************************************************************/
double mcc_dio_calcCapa ( mcc_modellist *ptmodel, 
                          double Va, double Vc, double temp,
                          double area, double perim
                        )
{
  double capa=0.0;
  int LEVEL;

  if (ptmodel->TYPE == MCC_DIODE) {
    LEVEL = MCC_ROUND(mcc_getparam_quick(ptmodel,__MCC_QUICK_LEVEL));
    switch ( LEVEL ) {
      case 1: capa = mcc_dio_calcCapa_l1 (ptmodel,Va,Vc,temp,area,perim);
              break;
      case 2: capa = mcc_dio_calcCapa_l2 (ptmodel,Va,Vc,temp,area);
              break;
      case 3: capa = mcc_dio_calcCapa_l3 (ptmodel,area);
              break;
      case 8: capa = mcc_dio_calcCapa_l8 (ptmodel,Va,Vc,temp,area,perim);
              break;
    }
  }
  return capa;
}

/******************************************************************************\
 *
 * FUNC : mcc_dio_calcCapa_l1
 * CDEP is Depletion capacitance
   Va: Anode Voltage
   Vc: Cathode Voltage

   return the total capacitance: Capa = CDEPa + CDEPp + CIDFF + CMETAL + CPOLY 
\******************************************************************************/
double mcc_dio_calcCapa_l1 ( mcc_modellist *ptmodel, 
                             double Va, double Vc, double temp,
                             double area, double perim
                           )
{
  double vd;
  double CDEPa,CDEPp;
  double FC,FCS,KMS,CJO,VJ,MJ;
  double MJSW,CJSW;
  double CJOeff,CJSWeff;
  double SHRINK,SCALE=1.0,SCALM,XOI,XOM,WP,XP,XM;
  double LM,WM,LP,M;
  double LMeff,WMeff,LPeff,WPeff,XPeff,XMeff;
  double CMETAL,CPOLY,Capa=0.0;
  double VJ_T, PHP_T, CJO_T,CJSW_T, dT,qT;
  double Vt_T,Vt_Tnom,Eg_T,Eg_Tnom;
  double dpbdt, dphpdt;
  double T,Tnom,TNOM;
  double TPB, PHP, TPHP, CTA, CTP,GAP2;
  double c0;
  int TLEVC,TLEV;
  int DCAP;

  vd = Va-Vc;

  TLEVC = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_TLEVC));
  TLEV = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_TLEV));
  DCAP = MCC_ROUND(mcc_getparam_quick(ptmodel, __MCC_QUICK_DCAP));
  FC = mcc_getparam_quick(ptmodel, __MCC_QUICK_FC);
  FCS = mcc_getparam_quick(ptmodel, __MCC_QUICK_FCS);
  KMS = mcc_getparam_quick(ptmodel, __MCC_QUICK_KMS);
  CJO = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJO);
  CJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW);
  VJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_VJ);
  MJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJ);
  MJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSW);
  KMS = mcc_getparam_quick(ptmodel, __MCC_QUICK_KMS);
  XOI = mcc_getparam_quick(ptmodel, __MCC_QUICK_XOI);
  XOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_XOM);
  WP = mcc_getparam_quick(ptmodel, __MCC_QUICK_WP);
  XP = mcc_getparam_quick(ptmodel, __MCC_QUICK_XP);
  LP = mcc_getparam_quick(ptmodel, __MCC_QUICK_LP);
  LM = mcc_getparam_quick(ptmodel, __MCC_QUICK_LM);
  WM = mcc_getparam_quick(ptmodel, __MCC_QUICK_WM);
  WP = mcc_getparam_quick(ptmodel, __MCC_QUICK_WP);
  XM = mcc_getparam_quick(ptmodel, __MCC_QUICK_XM);
  SHRINK = mcc_getparam_quick(ptmodel, __MCC_QUICK_SHRINK);
  SCALM = mcc_getparam_quick(ptmodel, __MCC_QUICK_SCALM);
  M = mcc_getparam_quick(ptmodel, __MCC_QUICK_M);
  TPB = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPB);
  PHP = mcc_getparam_quick(ptmodel, __MCC_QUICK_PHP);
  TPHP = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPHP);
  CTA = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTA);
  CTP = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTP);
  GAP2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP2);

  if (DCAP == 2)
    FC = FCS = 0.0;

  // Geometry
  LMeff = LM*SCALE*SHRINK;
  WMeff = WM*SCALE*SHRINK;
  LPeff = LP*SCALE*SHRINK;
  WPeff = WP*SCALE*SHRINK;
  XPeff = XP*SCALM;
  XMeff = XM*SCALM;
  
  // Temperature effect
  T = temp + MCC_KELVIN;
  TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
  Tnom = TNOM + MCC_KELVIN;
  dT = T - Tnom;
  qT = T / Tnom;

  Vt_T = mcc_calc_vt(temp);
  Vt_Tnom = mcc_calc_vt(TNOM);
  Eg_T = mcc_calc_eg (temp);
  Eg_Tnom = mcc_calc_eg (TNOM);
  switch ( TLEVC ) {
    case 0: c0 = (Eg_Tnom/Vt_Tnom-Eg_T/Vt_T+3.0*log(qT));
            VJ_T = VJ*qT-Vt_T*c0;
            PHP_T = PHP*qT-Vt_T*c0;
            CJO_T = CJO*(1.0+MJ*(1.0+4.0e-4*dT-VJ_T/VJ));
            CJSW_T = CJSW*(1.0+MJSW*(1.0+4.0e-4*dT-PHP_T/PHP));
            break;
    case 1: VJ_T = VJ-TPB*dT;
            PHP_T = PHP - TPHP*dT;
            CJO_T = CJO*(1.0+CTA*dT);
            CJSW_T = CJSW*(1.0+CTP*dT);
            break;
    case 2: VJ_T = VJ-TPB*dT;
            PHP_T = PHP-TPHP-dT;
            CJO_T = CJO*pow((VJ/VJ_T),MJ);
            CJSW_T = CJSW*pow((PHP/PHP_T),MJSW);
            break;
    case 3: switch (TLEV) {
              case 0:
              case 1: c0 = Eg_Tnom+3.0*Vt_Tnom+(1.16-Eg_Tnom)*(2.0-Tnom/(Tnom+1108.0));
                      dpbdt = -(c0-VJ)/Tnom;
                      dphpdt = -(c0-PHP)/Tnom;
                      break;
              case 2: c0 = Eg_Tnom+3.0*Vt_Tnom+(1.16-Eg_Tnom)*(2.0-Tnom/(Tnom+GAP2));
                      dpbdt = -(c0-VJ)/Tnom;
                      dphpdt = -(c0-PHP)/Tnom;
                      break;
              default : avt_errmsg(MCC_ERRMSG, "023", AVT_ERROR);
                        c0 = Eg_Tnom+3.0*Vt_Tnom+(1.16-Eg_Tnom)*(2.0-Tnom/(Tnom+1108.0));
                        dpbdt = -(c0-VJ)/Tnom;
                        dphpdt = -(c0-PHP)/Tnom;
                        break;
            }
            VJ_T = VJ+dpbdt*dT;
            PHP_T = PHP+dphpdt*dT;
            CJO_T = CJO*(1.0-0.5*dpbdt*dT/VJ);
            CJSW_T = CJSW*(1.0-0.5*dphpdt*dT/PHP);
            break;
  }

  // CJOeff computation...
  if ( !ptmodel->SUBCKTNAME ) { 
    // it means that CJO & CJSW haven't been updated by area & perim
    CJOeff = CJO_T*area;
    CJSWeff = CJSW_T*perim;
  }
  else {
    CJOeff = CJO_T;
    CJSWeff = CJSW_T;
  }
  
  if ( vd < FC*VJ-KMS ) {
    CDEPa = CJOeff*pow((1.0-(vd+KMS)/VJ_T),-MJ);
    CDEPp = CJSWeff*pow((1.0-(vd+KMS)/VJ_T),-MJSW);
  }
  else {
    CDEPa = (CJOeff/pow((1.0-FC),1+MJ))*(1.0-FC*(1.0+MJ)+MJ*(vd+KMS)/VJ_T);
    CDEPp = (CJSWeff/pow((1.0-FC),1+MJSW))*(1.0-FC*(1.0+MJSW)+MJSW*(vd+KMS)/VJ_T);
  }
  // Metal & Polysilicon capacitance
  CMETAL = (MCC_EPSOX/XOI)*(WPeff+XPeff)*(LPeff+XPeff)*M;

  CPOLY = (MCC_EPSOX/XOM)*(WMeff+XMeff)*(LMeff+XMeff)*M;

  // CDIFF = TT*dId/dVd = TT*gd;
  
  Capa = CDEPa+CDEPp+CMETAL+CPOLY;

  return Capa;
}

/******************************************************************************\
 *
 * FUNC : mcc_dio_calcCDEP_l2
 * CDEP is Depletion capacitance
   Va: Anode Voltage
   Vc: Cathode Voltage

   return the total capacitance: Capa = CDEP + CDIFF
\******************************************************************************/
double mcc_dio_calcCapa_l2 ( mcc_modellist *ptmodel, 
                             double Va, double Vc,
                             double temp,
                             double area
                           )
{
  double vd;
  double CDEP;
  double MJ,CJO,VJ,FC;
  double CJOeff;
  double VJ_T, CJO_T, dT,qT;
  double Vt_T,Vt_Tnom,Eg_T,Eg_Tnom;
  double T,Tnom,TNOM;

  vd = Va-Vc;

  CJO = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJO);
  VJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_VJ);
  MJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJ);
  FC = mcc_getparam_quick(ptmodel, __MCC_QUICK_FC);

  // Temperature effect
  T = temp + MCC_KELVIN;
  TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
  Tnom = TNOM + MCC_KELVIN;
  dT = T - Tnom;
  qT = T / Tnom;

  Vt_T = mcc_calc_vt(temp);
  Vt_Tnom = mcc_calc_vt(TNOM);
  Eg_T = mcc_calc_eg (temp);
  Eg_Tnom = mcc_calc_eg (TNOM);
  
  VJ_T = VJ*qT-Vt_T*(Eg_Tnom/Vt_Tnom-Eg_T/Vt_T+3.0*log(qT));
  CJO_T = CJO*(1.0+MJ*(1.0+4.0e-4*dT-VJ_T/VJ));
  

  // CJOeff computation...
  if ( !ptmodel->SUBCKTNAME ) // it means that CJO haven't been updated by area
    CJOeff = CJO_T*area;
  else 
    CJOeff = CJO_T;
  
  if ( vd < FC*VJ ) 
    CDEP = CJOeff*pow((1.0-vd/VJ),-MJ);
  else
    CDEP = (CJOeff/pow((1.0-FC),1+MJ))*(1.0-FC*(1.0+MJ)+MJ*vd/VJ);
  
  return CDEP;
}

/******************************************************************************\
 *
 * FUNC : mcc_dio_calcCapa_l3
 * CDEP is Depletion capacitance
   Va: Anode Voltage
   Vc: Cathode Voltage

   return the total capacitance: Capa = Cd (constant)
\******************************************************************************/
double mcc_dio_calcCapa_l3 ( mcc_modellist *ptmodel, 
                             double area
                           )
{
  double TOX;
  double AREAeff;
  double Cd;

  TOX = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX);

  AREAeff = area;

  Cd = AREAeff*MCC_EPSO/TOX;

  return Cd;
}

/******************************************************************************\
 *
 * FUNC : mcc_dio_calcCDEP_l8
   Va: Anode Voltage
   Vc: Cathode Voltage


   DIOLEV  = 9: Cbx = Cbxbot+Cbxsid+Cbxdiff (Cbxdiff assumed to be 0)
   DIOLEV != 9: Cbx = AREA*Cjbv+PERI*Cjsv+PGATE*Cjgv
   
   return the total capacitance: Cbx
\******************************************************************************/
double mcc_dio_calcCapa_l8 ( mcc_modellist *ptmodel, 
                             double Va,double Vc,
                             double temp,
                             double area, double perim
                           )
{
  double Cbx,Cbxbot,Cbxsid,Cjax,Cjpx;
  double JS,JSW,CJ,CJSW,CJGATE,c0;
  double PB,PBSW,TNOM,T,Tnom,dT,qT,FC;
  double CJ_T,CJSW_T,PB_T,PBSW_T,Vt_T,Vt_Tnom,Eg_T,Eg_Tnom;
  double vd;
  int M=1; //number of parallel transistor
  double CTA,PTA,CTP,GAP2,Dpb,Dpbsw,MJ,MJSW,PTP;
  int TLEVC;
  int DIOLEV;

  TLEVC = MCC_ROUND(mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEVC));
  TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
  JS  = mcc_getparam_quick(ptmodel, __MCC_QUICK_JS);
  JSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_JSW);
  CJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJ);
  CJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW);
  CJGATE = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJGATE);
  DIOLEV = MCC_ROUND(mcc_getparam_quick(ptmodel, __MCC_QUICK_DIOLEV));
  FC = mcc_getparam_quick(ptmodel, __MCC_QUICK_FC);
  PB = mcc_getparam_quick(ptmodel, __MCC_QUICK_PB);
  PBSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSW);
  PTA = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTA);
  PTP = mcc_getparam_quick(ptmodel, __MCC_QUICK_PTP);
  CTA = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTA);
  CTP = mcc_getparam_quick(ptmodel, __MCC_QUICK_CTP);
  GAP2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAP2);
  MJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJ);
  MJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSW);

  T = temp+MCC_KELVIN;
  Tnom = TNOM+MCC_KELVIN;
  dT = T-Tnom;
  qT = T/Tnom;

  Vt_T = mcc_calc_vt(temp);
  Vt_Tnom = mcc_calc_vt(TNOM);
  Eg_T = mcc_calc_eg (temp);
  Eg_Tnom = mcc_calc_eg (TNOM);


  vd = Va-Vc;

  if ( DIOLEV != 9 ) {
    // Temperature update
    switch ( TLEVC ) {
      case 0: c0 = (Eg_Tnom/Vt_Tnom-Eg_T/Vt_T+3.0*log(qT));
              PB_T = PB*qT-Vt_T*c0;
              PBSW_T = PBSW*qT-Vt_T*c0;
              CJ_T = CJ*(1.0+MJ*(1.0+4.0e-4*dT-PB_T/PB));
              CJSW_T = CJSW*(1.0+MJSW*(1.0+4.0e-4*dT-PBSW_T/PBSW));
              break;
      case 1: PB_T = PTA*dT;
              PBSW_T = PBSW-PTP*dT;
              CJ_T = CJ*(1.0+CTA*dT);
              CJSW_T = CJSW*(1.0+CTP*dT);
              break;
      case 2: PB_T = PB-PTA*dT;
              PBSW_T = PBSW - PTP*dT;
              CJ_T = CJ*pow(PB/PB_T,MJ);
              CJSW_T = CJSW*pow(PBSW/PBSW_T,MJSW);
              break;
      case 3: c0 = Eg_Tnom-3.0*Vt_Tnom+(Eg_Tnom-Eg_T)*(2.0-Tnom/(Tnom+GAP2));
              Dpb = PB-c0;
              Dpbsw = PBSW-c0;
              PB_T = PB+Dpb*dT/Tnom;
              PBSW_T = PBSW+Dpbsw*dT/Tnom;
              CJ_T = CJ*(1.0-0.5*Dpb/PB*dT/Tnom);
              CJSW_T = CJSW*(1.0-0.5*Dpb/PBSW*dT/Tnom);
              break;
    }
    if ( CJ > 0.0 ) 
      Cjax = M*area*CJ_T;
    else
      Cjax = 0.0;
    if ( CJSW > 0.0 ) 
      Cjpx = M*perim*CJSW_T;
    else
      Cjpx = 0.0;
    if ( vd <= FC*PB ) 
      Cbxbot = Cjax*pow((1.0-vd/PB_T),-MJ);
    else
      Cbxbot = (Cjax/pow(1.0-FC,1.0+MJ))*(1.0-FC*(1.0+MJ)+MJ*vd/PB_T);
    if ( vd <= FC*PBSW ) 
      Cbxsid = Cjpx*pow((1.0-vd/PBSW_T),-MJSW);
    else
      Cbxsid = (Cjpx/pow(1.0-FC,1.0+MJSW))*(1.0-FC*(1.0+MJSW)+MJSW*vd/PBSW_T);
    // if TT>0.0 Cbxdiff = TT*dIbx/dVbx else Cbxdiff=0
    Cbx = Cbxbot+Cbxsid;
  }
  else {
    // unsupported because no info on pgate...
    fprintf (stderr,"[MCC ERR] Diode capacitance for level 8 and diolev = 9 is not supported yet!\n");
    Cbx = 0.0;
  }

  return Cbx;
}

/******************************************************************************\
 * Func mcc_calcIgb
\******************************************************************************/
double mcc_calcIgb (char *technoname, char *transname,
                    int transtype, int transcase, 
                    double vbs, double vgs, double vds, 
                    double L, double W, 
                    double temp,elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double igb = 0.0;

    if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
        return 0.0;
    else {
        switch(ptmodel->MODELTYPE) {
              case MCC_BSIM3V3  : igb = 0.0;
                                  break ;
              case MCC_BSIM4    : igb = mcc_calcIgb_bsim4 (ptmodel, L, W, temp,
                                                           vgs,vds,vbs, 
                                                           lotrsparam) ;
                                  break ;
              default           : igb = 0.0;
                                  break;
        }
    }
    return igb;
}

/******************************************************************************\
 * Func mcc_calcIgixl
\******************************************************************************/
void mcc_calcIgixl (char *technoname, char *transname,
                    int transtype, int transcase, 
                    double vbs, double vgs, double vds, 
                    double L, double W, double temp,
                    double *ptIgidl, double *ptIgisl,
                    elp_lotrs_param *lotrsparam) 
{
  mcc_modellist *ptmodel ;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return;
  else {
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  : if ( ptIgisl ) *ptIgisl = 0.0;
                                if ( ptIgidl ) *ptIgidl = 0.0;
                                break ;
            case MCC_BSIM4    : mcc_calcIgixl_bsim4 (ptmodel, L, W, 
                                                     ptIgidl,ptIgisl,
                                                     temp,
                                                     vgs,vds,vbs, 
                                                     lotrsparam) ;
                                break ;
            default           : if ( ptIgisl ) *ptIgisl = 0.0;
                                if ( ptIgidl ) *ptIgidl = 0.0;
                                break;
      }
  }
}

/******************************************************************************\
 * Func mcc_calcIxb
\******************************************************************************/
void mcc_calcIxb (char *technoname, char *transname,
                  int transtype, int transcase, 
                  double vbs, double vds, 
                  double L, double W, double temp,
                  double AD, double PD, double AS, double PS,
                  double *ptIdb, double *ptIsb,
                  elp_lotrs_param *lotrsparam) 
{
  mcc_modellist *ptmodel ;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return;
  else {
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  : if ( ptIdb ) *ptIdb = 0.0;
                                if ( ptIsb ) *ptIsb = 0.0;
                                break ;
            case MCC_BSIM4    : mcc_calcIxb_bsim4 (ptmodel, L, W, 
                                                   ptIdb,ptIsb,
                                                   temp,vds,vbs, 
                                                   AD,PD,AS,PS,
                                                   lotrsparam) ;
                                break ;
            default           : if ( ptIdb ) *ptIdb = 0.0;
                                if ( ptIsb ) *ptIsb = 0.0;
                                break;
      }
  }
}

/******************************************************************************\
 * Func mcc_calcIgx
\******************************************************************************/
void mcc_calcIgx (char *technoname, char *transname,
                  int transtype, int transcase, 
                  double vds, double vgs, 
                  double L, double W, double temp,
                  double *ptIgd, double *ptIgs,
                  elp_lotrs_param *lotrsparam) 
{
  mcc_modellist *ptmodel ;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return ;
  else {
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  : if ( ptIgd ) *ptIgd = 0.0;
                                if ( ptIgs ) *ptIgs = 0.0;
                                break ;
            case MCC_BSIM4    :
                                mcc_calcIgx_bsim4 (ptmodel, L, W, 
                                                   ptIgd,ptIgs,
                                                   temp,vds,vgs, 
                                                   lotrsparam) ;
                                break ;
            default           : if ( ptIgd ) *ptIgd = 0.0;
                                if ( ptIgs ) *ptIgs = 0.0;
                                break;
      }
  }
}

/******************************************************************************\
 * Func mcc_calcIgcx
\******************************************************************************/
void mcc_calcIgcx (char *technoname, char *transname,
                   int transtype, int transcase, 
                   double vds, double vgs, double vbs,
                   double L, double W, double temp,
                   double *ptIgcd, double *ptIgcs,
                   elp_lotrs_param *lotrsparam) 
{
  mcc_modellist *ptmodel ;

  if(!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return;
  else {
      switch(ptmodel->MODELTYPE) {
            case MCC_BSIM3V3  : if ( ptIgcd ) *ptIgcd = 0.0;
                                if ( ptIgcs ) *ptIgcs = 0.0;
                                break ;
            case MCC_BSIM4    :
                                mcc_calcIgcx_bsim4 (ptmodel, L, W, 
                                                    ptIgcd,ptIgcs,
                                                    temp,vds,vgs,vbs,
                                                    lotrsparam) ;
                                break ;
            default           : if ( ptIgcd ) *ptIgcd = 0.0;
                                if ( ptIgcs ) *ptIgcs = 0.0;
                                break;
      }
  }
}

/******************************************************************************\
 * Func mcc_calcILeakage
\******************************************************************************/
double mcc_calcILeakage (char *technoname, char *transname,
                         int transtype, int transcase, 
                         double vbs, double vds, double vgs,
                         double L, double W, double temp,
                         double AD, double PD, double AS, double PS,
                         double *BLeak, double *DLeak, double *SLeak, 
                         elp_lotrs_param *lotrsparam) 
{
  double Ileak = 0.0;
  double Igb=0.0,Igisl=0.0,Igidl=0.0,Isb=0.0,
         Idb=0.0,Igs=0.0,Igd=0.0,Igcs=0.0,Igcd=0.0;
  double vth, Isubth=0.0;

  Igb = mcc_calcIgb (technoname, transname,
                     transtype, transcase, 
                     vbs, vgs, vds, 
                     L, W, temp,lotrsparam);

  mcc_calcIgixl (technoname, transname,
                 transtype, transcase, 
                 vbs, vgs, vds, 
                 L, W, temp,
                 &Igidl, &Igisl,
                 lotrsparam);

  mcc_calcIxb (technoname, transname,
               transtype, transcase, 
               vbs, vds, 
               L, W, temp,
               AD, PD, AS, PS,
               &Idb, &Isb,
               lotrsparam);

  mcc_calcIgx (technoname, transname,
               transtype, transcase, 
               vds, vgs, 
               L, W, temp,
               &Igd, &Igs,
               lotrsparam); 

  mcc_calcIgcx (technoname, transname,
                transtype, transcase, 
                vds, vgs, vbs,
                L,W, temp,
                &Igcd, &Igcs,
                lotrsparam);

  //----> Subthreshold current

  vth = mcc_calcVTH (technoname, transname, 
                     transtype, transcase, L, W, 
                     temp, vbs, vds,lotrsparam,MCC_NO_LOG) ;

  if ( fabs(vgs) < fabs(vth) ) {
    Isubth = mcc_calcIDS (technoname, transname, 
                          transtype, transcase, vbs, vgs, 
                          vds, L, W, 
                          temp,lotrsparam);
  }
  if ( BLeak ) *BLeak = Igb+Isb+Idb+Igisl+Igidl;
  if ( DLeak ) *DLeak = Igd+Igcd;
  if ( SLeak ) *SLeak = Igs+Igcs+Isubth;
  
  Ileak = Isubth+Igb+Igisl+Igidl+Isb+Idb+Igs+Igd+Igcs+Igcd;

  return Ileak;
}


/******************************************************************************\
FUNCTION : mcc_calcVTI_nmos_com_fn
Soit 2 transistors nmos mn0 et mn1 en serie:
mn0 vdd vdd Vti vbulk nmos
mn1 Vti 0 0 vbulk nmos
On resoud Ids(mn0) = Ids(mn1) pour un Vti trouve 
      <=> Ids_mn0(vds,vgs,vbs) - Ids_mn1(vds,vgs,vbs) = 0;
\******************************************************************************/

int mcc_calcVTI_nmos_com_fn ( struct mcc_vdeg_fn *data, double vti, double *res )
{
  double ids0,ids1;

  ids0= mcc_calcIDS( data->ptmodel->TECHNO->NAME,
                     data->transname, 
                     data->ptmodel->TYPE,
                     data->ptmodel->CASE,
                     data->lotrsparam->VBULK-vti,
                     data->vdd-vti,
                     data->vdd-vti,
                     data->L,
                     data->W,
                     data->temp,
                     data->lotrsparam
                   )   ;
  ids1= mcc_calcIDS( data->ptmodel->TECHNO->NAME,
                     data->transname, 
                     data->ptmodel->TYPE,
                     data->ptmodel->CASE,
                     data->lotrsparam->VBULK,
                     0.0,
                     vti,
                     data->L,
                     data->W,
                     data->temp,
                     data->lotrsparam
                   ) ;
  *res = ids0-ids1;
  return 1;
}

/******************************************************************************\
FUNCTION : mcc_calcVTI_nmos_com
\******************************************************************************/
double  mcc_calcVTI_nmos_com (mcc_modellist *ptmodel, char *transname, 
                            double L, double W, 
                            double vdd, double temp, double step, 
                            elp_lotrs_param *lotrsparam) 
{
    static int nbcall=0;
    struct mcc_vdeg_fn data ;
    double x0, x1, vti;
    int ret, it ;
    
    if (ptmodel->TYPE == MCC_PMOS) {
      avt_errmsg(MCC_ERRMSG, "019", AVT_ERROR, ptmodel->NAME) ;
      return 0.0 ;
    }
    nbcall++;
    
    data.ptmodel    = ptmodel ;
    data.transname  = transname ;
    data.L          = L ;
    data.W          = W ;
    data.temp       = temp ;
    data.lotrsparam = lotrsparam ;
    data.vdd        = vdd ;
    
    x0 = 0 ;
    x1 = vdd ;
    it = 10000 ;
    ret = mbk_dichotomie( (int(*)(void*, double, double*)) mcc_calcVTI_nmos_com_fn,
                          NULL,
                          &data,
                          &x0,
                          &x1,
                          MBK_DICHO_EQUAL,
                          &it,
                          step,
                          DBL_MAX,
                          &vti
                        );
                        
    if( ret != MBK_DICHO_OK ) {
      //avt_errmsg(MCC_ERRMSG, "024", AVT_WARNING) ;
    }
    return vti;
}

/******************************************************************************/
/* Calcul de la tension degradee VDDDEG d'un modele de transistor             */
/******************************************************************************/
double mcc_calcVTI_nmos (char *technoname, char *transname, 
                         int transtype, int transcase, double L, double W, 
                         double vdd, double temp, 
                         double step, elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double vti = 0.0 ;

    if (!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return 0.0;
    else 
      vti = mcc_calcVTI_nmos_com(ptmodel, transname, L, W, 
                                 vdd, temp, step, 
                                 lotrsparam) ; 

    return vti;
}

/******************************************************************************\
FUNCTION : mcc_calcVTI_pmos_com_fn
Soit 2 transistors pmos mp0 et mp1 en serie:
mp0 0 0 Vti vbulk pmos
mp1 Vti vdd vdd vbulk pmos
On resoud Ids(mp0) = Ids(mp1) pour un Vti trouve 
      <=> Ids_mp0(vds,vgs,vbs) - Ids_mp1(vds,vgs,vbs) = 0;
\******************************************************************************/

int mcc_calcVTI_pmos_com_fn ( struct mcc_vdeg_fn *data, double vti, double *res )
{
  double ids0,ids1;

  ids0= mcc_calcIDS( data->ptmodel->TECHNO->NAME,
                     data->transname, 
                     data->ptmodel->TYPE,
                     data->ptmodel->CASE,
                     data->lotrsparam->VBULK-vti, //vbs
                     -vti, //vgs
                     -vti, //vds
                     data->L,
                     data->W,
                     data->temp,
                     data->lotrsparam
                   )   ;
  ids1= mcc_calcIDS( data->ptmodel->TECHNO->NAME,
                     data->transname, 
                     data->ptmodel->TYPE,
                     data->ptmodel->CASE,
                     data->lotrsparam->VBULK-data->vdd,
                     0.0,
                     vti-data->vdd,
                     data->L,
                     data->W,
                     data->temp,
                     data->lotrsparam
                   ) ;
  *res = ids0-ids1;
  return 1;
}

/******************************************************************************\
FUNCTION : mcc_calcVTI_pmos_com
\******************************************************************************/
double  mcc_calcVTI_pmos_com (mcc_modellist *ptmodel, char *transname, 
                            double L, double W, 
                            double vdd, double temp, double step, 
                            elp_lotrs_param *lotrsparam) 
{
    static int nbcall=0;
    struct mcc_vdeg_fn data ;
    double x0, x1, vti;
    int ret, it ;
    
    if (ptmodel->TYPE == MCC_NMOS) {
      return 0.0 ;
    }
    nbcall++;
    
    data.ptmodel    = ptmodel ;
    data.transname  = transname ;
    data.L          = L ;
    data.W          = W ;
    data.temp       = temp ;
    data.lotrsparam = lotrsparam ;
    data.vdd        = vdd ;
    
    x0 = 0 ;
    x1 = vdd ;
    it = 10000 ;
    ret = mbk_dichotomie( (int(*)(void*, double, double*)) mcc_calcVTI_pmos_com_fn,
                          NULL,
                          &data,
                          &x0,
                          &x1,
                          MBK_DICHO_EQUAL,
                          &it,
                          step,
                          DBL_MAX,
                          &vti
                        );
                        
    if( ret != MBK_DICHO_OK ) {
   //   avt_errmsg(MCC_ERRMSG, "025", AVT_WARNING) ;
    }
    return vti;
}

/******************************************************************************/
/******************************************************************************/
double mcc_calcVTI_pmos (char *technoname, char *transname, 
                         int transtype, int transcase, double L, double W, 
                         double vdd, double temp, 
                         double step, elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double vti = 0.0 ;

    if (!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return 0.0;
    else 
      vti = mcc_calcVTI_pmos_com(ptmodel, transname, L, W, 
                                 vdd, temp, step, 
                                 lotrsparam) ; 

    return vti;
}


/******************************************************************************/
void mcc_calcRACCESS( char *technoname, char *transname, 
                      int transtype, int transcase, double L, double W,
                      elp_lotrs_param *lotrsparam,
                      double *RS, double *RD
                    )
{
    mcc_modellist *ptmodel ;
    double Weff, xw ;
    

    ptmodel = mcc_getmodel( technoname, transname, transtype, transcase, L, W, 0 );

    if( !ptmodel ) {
      if( RS ) *RS = 0.0 ;
      if( RD ) *RD = 0.0 ;
      return ;
    }

  xw = mcc_getXW (technoname, transname, transtype, transcase, L,W);
  Weff = W*xw + mcc_calcDW (technoname, transname, transtype, transcase, L, W, lotrsparam);
  mcc_compute_RD_RS( ptmodel, Weff, RS, RD, lotrsparam );
}

/******************************************************************************/
double mcc_calcVTI (char *technoname, char *transname, 
                    int transtype, int transcase, double L, double W, 
                    double vdd, double temp, 
                    double step, elp_lotrs_param *lotrsparam) 
{
    mcc_modellist *ptmodel ;
    double vti = 0.0 ;

    if (!(ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0)))
      return 0.0;
    else {
      if ( ptmodel->TYPE == MCC_PMOS ) {
        vti = mcc_calcVTI_pmos_com(ptmodel, transname, L, W, 
                                   vdd, temp, step, 
                                   lotrsparam) ; 
      }
      else {
        vti = mcc_calcVTI_nmos_com(ptmodel, transname, L, W, 
                                   vdd, temp, step, 
                                   lotrsparam) ; 
      }
    }
    return vti;
}

void mcc_calcPAfromgeomod( lotrs_list      *lotrs,
                           char            *modelname,
                           int              type,
                           int              lotrscase,
                           double           vdd,
                           elp_lotrs_param *lotrsparam,
                           double          *as, 
                           double          *ad, 
                           double          *ps, 
                           double          *pd 
                         )
{
  double         l, 
                 w ;
  double         lscale, 
                 wscale ;
  mcc_modellist *model ;

  l = ((double)lotrs->LENGTH) / ((double)SCALE_X) ;
  w = ((double)lotrs->WIDTH)  / ((double)SCALE_X) ;
  
  mcc_update_technoparams( modelname, type, l, w, lotrs, lotrscase );

  switch( lotrscase ) {
    case MCC_BEST  : MCC_VDDmax = MCC_VDD_BEST;
                     MCC_VGS = MCC_VDDmax/2.0;
                     MCC_TEMP = MCC_TEMP_BEST;
                     break;
    case MCC_WORST : MCC_VDDmax = MCC_VDD_WORST;
                     MCC_VGS = MCC_VDDmax/2.0;
                     MCC_TEMP = MCC_TEMP_WORST;
                     break;
    default        : MCC_VDDmax = vdd;
                     MCC_VGS = MCC_VDDmax/2.0;
                     break;
  }
 
  if(type == MCC_TRANS_N) {   
    lscale = MCC_LN*1.0e-6 ;
    wscale = MCC_WN*1.0e-6 ;
  }
  else {
    lscale = MCC_LP*1.0e-6 ;
    wscale = MCC_WP*1.0e-6 ;
  }

  MCC_CALC_CUR = MCC_CALC_MODE ;

  model = mcc_getmodel( MCC_MODELFILE, modelname, type, lotrscase, lscale, wscale, 0 );

  if( model ) {

    switch( model->MODELTYPE ) {
    case MCC_BSIM4 :
      mcc_calcPAfromgeomod_bsim4( lotrs, model, lotrsparam, as, ad, ps, pd ) ;
      break ;
    case MCC_BSIM3V3 :
      mcc_calcPAfromgeomod_bsim3( lotrs, model, lotrsparam, as, ad, ps, pd ) ;
      break ;
    case MCC_EXTMOD :
      mcc_calcPAfromgeomod_extmod( lotrs, model, lotrsparam, as, ad, ps, pd ) ;
      break ;
    }
  }
}

int mcc_getspicetechno( char *technoname, char *transname, int transtype, int transcase, double L, double W )
{
  mcc_modellist *model ;
  int            tectype = MCC_NOMODEL ;

  model = mcc_getmodel( technoname, transname, transtype, transcase, L, W, 0 );
  if( model ) 
    tectype = model->MODELTYPE ;

  return tectype ;
}

void mcc_check_capa_print( char trans, 
                           char *technoname, 
                           char *transname, 
                           int transtype, 
                           int transcase, 
                           double L, double W,
                           double temp, double vdd,
                           elp_lotrs_param *lotrsparam
                         )
{
  mcc_modellist *ptmodel ;
  double La_Wa ;
  double vgs, vds, vbs, v, vi, vf, v1, v2 ;
  int flag, flaghalf, flagvt ;
  double Qg, Qb, Qs, Qd ;
  double Qg0, Qd0, Qde0 ;
  double qgs, qgs0 ;
  double cgs, cgsf ;
  double qgd, qgd0 ;
  double cgd, cgdf ;
  double qgde, qgde0 ;
  double cgde0, cgde1, cgde2 ;
  char filename[1024] ;
  FILE *file ;
  int j,n;
  char *label ;
  
  ptmodel = mcc_getmodel(technoname, transname, transtype, transcase, L, W, 0);
  if( !ptmodel )
    return ;
    
  if( lotrsparam->ISVBSSET )
    vbs = lotrsparam->VBS ;
  else
    vbs = ( transtype == MCC_NMOS ) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax ; 
    
  switch( ptmodel->MODELTYPE ) {
  case MCC_BSIM4 :
    La_Wa = (L + mcc_calcDLC_bsim4 (ptmodel, lotrsparam, L, W)) *(W+mcc_calcDWC_bsim4 (ptmodel, lotrsparam, L, W));
    break ;
  case MCC_MPSP:
  case MCC_MPSPB    :
  case MCC_EXTMOD:
    La_Wa = L*W ;
    break ;
  default :
    La_Wa = (L + mcc_calcDLC_bsim3v3(ptmodel, L, W)) *(W+mcc_calcDWC_bsim3v3(ptmodel, L, W));
  }

  if( trans==1 ) {
    label = "ud" ;
    vi = 0.0 ;
    vf = vdd ;
    if( transtype == MCC_NMOS ) {
      cgs  = MCC_CGSUN ;
      cgsf = MCC_CGSUFN ;
      cgd  = MCC_CGDN ;
      cgdf = MCC_CGDN ;
      vds  = vdd ;

      if( MCC_VT0N < MCC_VDDmax/2.0 ) {
        v1 = MCC_VT0N ;
        v2 = MCC_VDDmax/2.0 ;
      }
      else {
        v1 = MCC_VDDmax/2.0 ;
        v2 = MCC_VT0N ;
      }
      cgde0 = MCC_CGD0N ;
      cgde1 = MCC_CGD1N ;
      cgde2 = MCC_CGD2N ;
    }
    else {
      cgs  = MCC_CGSUP ;
      cgsf = MCC_CGSUFP ;
      cgd  = MCC_CGDCP ;
      cgdf = MCC_CGDCP ;
      vds = 0.0 ;

      if( MCC_VT0P < MCC_VDDmax/2.0 ) {
        v1 = MCC_VDDmax/2.0 ;
        v2 = MCC_VDDmax-MCC_VT0P ;
      }
      else {
        v1 = MCC_VDDmax-MCC_VT0P ;
        v2 = MCC_VDDmax/2.0 ;
      }
      cgde0 = MCC_CGDC2P ;
      cgde1 = MCC_CGDC1P ;
      cgde2 = MCC_CGDC0P ;
    }
  }
  else {
    label = "du" ;
    vi = vdd ;
    vf = 0.0 ;
    if( transtype == MCC_NMOS ) {
      cgs  = MCC_CGSDN ;
      cgsf = MCC_CGSDFN ;
      cgd  = MCC_CGDCN ;
      cgdf = MCC_CGDCN ;
      vds = 0.0 ;

      if( MCC_VT0N < MCC_VDDmax/2.0 ) {
        v1 = MCC_VDDmax/2.0 ;
        v2 = MCC_VT0N ;
      }
      else {
        v1 = MCC_VT0N ;
        v2 = MCC_VDDmax/2.0 ;
      }
      cgde0 = MCC_CGDC2N ;
      cgde1 = MCC_CGDC1N ;
      cgde2 = MCC_CGDC0N ;
    }
    else {
      cgs  = MCC_CGSDP ;
      cgsf = MCC_CGSDFP ;
      cgd  = MCC_CGDP ;
      cgdf = MCC_CGDP ;
      vds  = -vdd ;

      if( MCC_VT0P < MCC_VDDmax/2.0 ) {
        v1 = MCC_VDDmax-MCC_VT0P ;
        v2 = MCC_VDDmax/2.0 ;
      }
      else {
        v1 = MCC_VDDmax/2.0 ;
        v2 = MCC_VDDmax-MCC_VT0P ;
      }
      cgde0 = MCC_CGD0P ;
      cgde1 = MCC_CGD1P ;
      cgde2 = MCC_CGD2P ;
    }
  }

  sprintf( filename, "%s_%c_%s", mcc_debug_prefix( "intrinsic" ), transtype==MCC_NMOS ? 'N':'P', label ) ;
  file = mbkfopen( filename, "dat", "w" );
  if( !file )
    return ;

  fprintf( file, "#vgs qg(int) qg(cgs) qd(int) qd(cgd) qd(cgde)\n" );

  n = 30.0 ;
  flag = 0 ;
  flaghalf = 0 ;
  flagvt   = 0 ;

  for( j=0 ; j<=n ; j++ ) {

    v = vi+(vf-vi)*((float)j)/((float)n);

    if( transtype == MCC_NMOS )
      vgs = v ;
    else
      vgs = v-MCC_VDDmax ;
      
    switch( ptmodel->MODELTYPE ) {
    case MCC_BSIM4 :
      mcc_calcQint_bsim4 (ptmodel, L, W,
                          temp, vgs, vbs, vds,
                          &Qg, &Qs, &Qd, &Qb,
                          lotrsparam);
      break ;
    case MCC_MPSP:
    case MCC_MPSPB:
      mcc_calcQint_psp (ptmodel, L, W,
                        temp, vgs, vbs, vds,
                        &Qg, &Qs, &Qd, &Qb,
                        lotrsparam);
      break ;
    case MCC_EXTMOD:
      mcc_calcQint_ext(ptmodel, L, W,
                        temp, vgs, vbs, vds,
                        &Qg, &Qs, &Qd, &Qb,
                        lotrsparam);
      break ;
    default :
      mcc_calcQint_bsim3v3 (ptmodel, L, W,
                          temp, vgs, vbs, vds,
                          &Qg, &Qs, &Qd, &Qb,
                          lotrsparam);
    }
    Qg*=La_Wa;
    Qb*=La_Wa;
    Qs*=La_Wa;
    Qd*=-La_Wa;

    if( !flag ) {
      flag = 1 ;
      Qg0  = Qg - cgs   * La_Wa * v ;
      Qd0  = Qd - cgd   * La_Wa * v ;
      Qde0 = Qd - cgde0 * La_Wa * v ;
    }

    
    if( !flaghalf ) {

      qgs = cgs * La_Wa * v + Qg0 ;
      qgd = cgd * La_Wa * v + Qd0 ;
      
      if( ( vf>vi && v > MCC_VDDmax/2.0 ) || ( vf<vi && v < MCC_VDDmax/2.0 ) ) {
      
        flaghalf = 1 ;
        qgs0 = cgs * La_Wa * MCC_VDDmax/2.0 ;
        qgd0 = cgd * La_Wa * MCC_VDDmax/2.0 ;
      }
    }

    if( flaghalf ) {
      qgs = cgsf * La_Wa * (v-MCC_VDDmax/2.0) + qgs0 + Qg0 ;
      qgd = cgdf * La_Wa * (v-MCC_VDDmax/2.0) + qgd0 + Qd0 ;
    }

    if( flagvt==0 ) {
      qgde = cgde0 * La_Wa * v + Qde0 ;
      if( ( vf>vi && v > v1 ) || ( vf<vi && v < v1 ) ) {
        flagvt = 1 ;
        qgde0 = cgde0 * La_Wa * v1 ;
      }
    }

    if( flagvt==1 ) {
      qgde = cgde1 * La_Wa * (v-v1) + qgde0 + Qde0 ;
      if( ( vf>vi && v > v2 ) || ( vf<vi && v < v2 ) ) {
        flagvt = 2 ;
        qgde0 = La_Wa * ( cgde0*v1 + cgde1*(v2-v1) ) ;
      }
    }

    if( flagvt==2 ) {
      qgde = cgde2 * La_Wa * (v-v2) + qgde0 + Qde0 ;
    }
    
    fprintf( file, "%g %g %g %g %g %g\n", v, Qg, qgs, Qd, qgd, qgde );
  }

  fclose( file );
  
  file = mbkfopen( filename, "plt", "w" );
  fprintf( file, "set xlabel 'Vin'\n" );
  fprintf( file, "set ylabel 'Q'\n" );
  fprintf( file, "plot \\\n" );
  fprintf( file, "'%s.dat' using 1:2 title 'Qg int', \\\n", filename );
  fprintf( file, "'%s.dat' using 1:3 title 'Q cgs', \\\n", filename );
  fprintf( file, "'%s.dat' using 1:4 title 'Qd int', \\\n", filename );
  fprintf( file, "'%s.dat' using 1:5 title 'Q cgd', \\\n", filename );
  fprintf( file, "'%s.dat' using 1:6 title 'Q cgde'\n", filename );
  fprintf( file, "pause -1\n" );
  fclose( file );
}

int mcc_get_swjuncap( char *technoname, char *transname, int transtype, int transcase, double L, double W )
{
  mcc_modellist *model ;
  double         swjuncap ;
  int            i ;
  
  model = mcc_getmodel( technoname, transname, transtype, transcase, L, W, 0 );
  swjuncap = mcc_getparam_quick(model, __MCC_QUICK_SWJUNCAP );
  i = ((int)( swjuncap+0.5 ));
  
  return i ;
}

void mcc_cleanmodel( mcc_modellist *model )
{
  if (model->TYPE == MCC_DIODE) {
  }
  else
    switch ( model->MODELTYPE ) {
            case MCC_BSIM3V3  :
            case MCC_BSIM4    :
            case MCC_MM9      : 
            case MCC_MOS2     :
                                break ;
            case MCC_MPSPB:
            case MCC_MPSP     : mcc_clean_psp( model );
                                break ;
            case MCC_EXTMOD   : mcc_clean_ext( model );
                                break ;
    }
}

#ifdef MCC_RESI_CODE

double mcc_calcResiCapa ( char *technoname, char *resiname,
                      int modtype, int modcase, 
                      double L, double W, double R, double SCALE, double C, double CRATIO, double M,
                      double *c1, double *c2, double *r
                    )
{
  mcc_modellist *ptmodel ;
  double Capa = 0.0, SCALM=1, RES, RSH, CAP, THICK, CAPSW ;
  double Lscaled, Wscaled, DWeff, DLeff, DW, DL, DI, Weff, Leff, Ceff, SHRINK, COX;
  *c1=*c2;
  *r=R;

  if ( (ptmodel = mcc_getmodel(technoname, resiname, modtype, modcase, 0, 0, 0)) )
  {
    SHRINK=mcc_getparam_quick(ptmodel, __MCC_QUICK_SHRINK);
    if (L<0)
    {
      L=mcc_getparam_quick(ptmodel, __MCC_QUICK_L);
      Lscaled=L*SCALM*SHRINK;
    }
    else
      Lscaled=L*SCALE*SHRINK;
    if (W<0)
    {
      W=mcc_getparam_quick(ptmodel, __MCC_QUICK_W);
      Wscaled=W*SCALM*SHRINK;
    }
    else
      Wscaled=W*SCALE*SHRINK;
    DL=mcc_getparam_quick(ptmodel, __MCC_QUICK_DL);
    DW=mcc_getparam_quick(ptmodel, __MCC_QUICK_DW);
    DWeff=DW*SCALM;
    DLeff=DL*SCALM;
    Leff=Lscaled*SCALE*SHRINK-2*DLeff;
    Weff=Wscaled*SCALE*SHRINK-2*DWeff;

    if (R>=0)
      *r=R*SCALE/M;
    else
    {
      RSH=mcc_getparam_quick(ptmodel, __MCC_QUICK_RSH);
      if (Weff*Leff*RSH>0)
        *r=Leff*RSH*SCALE/(M*Weff);
      else
       {
        RES=mcc_getparam_quick(ptmodel, __MCC_QUICK_RES);
        *r=RES*SCALE/M;
       }
    }
    
    if (C>=0)
    {
      Ceff=C*SCALE*M;
    }
    else
    {
      THICK=mcc_getparam_quick(ptmodel, __MCC_QUICK_THICK);
      COX=mcc_getparam_quick(ptmodel, __MCC_QUICK_COX);
      if (!COX && THICK==0)
       {
         if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CAP) == MCC_SETVALUE)
         {
           CAP=mcc_getparam_quick(ptmodel, __MCC_QUICK_CAP);
           Ceff=CAP*SCALE*M;
         }
         else
           Ceff=0; //error
       }
      else if (!COX && THICK!=0)
      {
         if (THICK!=0)
         {
           if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DI) == MCC_SETVALUE)
           {
             DI=mcc_getparam_quick(ptmodel, __MCC_QUICK_DI);
             COX=DI*MCC_EPSO/THICK;
           }
           else
             COX=MCC_EPSOX/THICK;
         }
      }
     CAPSW=mcc_getparam_quick(ptmodel, __MCC_QUICK_CAPSW);
     Ceff=M*SCALE*(Leff*Weff*COX)+2*(Leff+Weff)*CAPSW;
    }
    if (CRATIO<0) CRATIO=0.5;
    *c1=Ceff*CRATIO;
    *c2=Ceff*(1-CRATIO);
  }
}

#endif

double mcc_calcResiSimple ( double R, double TC1, double TC2, double DTEMP)
{
  double DELTAT;
  DELTAT=V_FLOAT_TAB[__SIM_TEMP].VALUE+DTEMP-V_FLOAT_TAB[__SIM_TNOM].VALUE;
  return R*( 1 + TC1*DELTAT + TC2*DELTAT*DELTAT );
}

