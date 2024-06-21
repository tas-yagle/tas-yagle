/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_bsim3v3.c                                             */
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

#include MCC_H
#include "mcc_util.h"
#include "mcc_mod_util.h"
#include "mcc_mod_bsim3v3.h"
#include "mcc_mod_bsim4.h"
#include "mcc_mod_spice.h"

/******************************************************************************/
/* EXTERNS                                                                    */
/******************************************************************************/

/******************************************************************************/
/* FUNCTION : mcc_initparam_bsim3v3 ()                                        */
/******************************************************************************/
void mcc_initparam_bsim3v3(mcc_modellist *ptmodel) 
{
    double Cox ;
    double LEVEL;
    int version,ACM;

    LEVEL = mcc_getparam_quick(ptmodel,__MCC_QUICK_LEVEL);

    mcc_initparam_com(ptmodel)                                ;

	mcc_initparam_quick(ptmodel, __MCC_QUICK_VERSION, 3.2, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PARAMCHK, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BINUNIT, 1.0, NULL, MCC_INITVALUE)    ;

/*-------  THRESHOLD PARAMETERS  ------------*/
	
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DELVT0, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VFB, -1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VBM, -5.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K1, 0.53, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K2, -0.0186, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K3, 80.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K3B, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_W0, 2.5e-6, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NLX, 1.74e-7, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT0W, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT1W, 5.3e6, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT2W, -3.2e-2, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT0, 2.2, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT1, 0.53, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT2, -3.2e-2, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NPEAK, 1.7e17, NULL, MCC_INITVALUE)    ;

/*-------  SUBTHRESHOLD PARAMETERS  ---------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_VOFF, -0.08, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFACTOR, 1.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETA0, 0.08, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAB, -0.07, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIT, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CDSC, 2.4e-4, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CDSCB, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CDSCD, 0.0, NULL, MCC_INITVALUE)       ;

/*-------  SATURATION PARAMETERS  -----------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_MOBMOD, 1.0, NULL, MCC_INITVALUE)      ;

	if(ptmodel->TYPE == MCC_NMOS)
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_U0, 0.067, NULL, MCC_INITVALUE)    ;
	else
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_U0, 0.025, NULL, MCC_INITVALUE)    ;
		
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MULU0, 1.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UA, 2.25e-9, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UB, 5.87e-19, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VSAT, 8.0e4, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AGS, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A0, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_B0, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_B1, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A1, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A2, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDSW, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PRWG, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PRWB, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WR, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PCLM, 1.3, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDIBLC1, 0.39, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDIBLC2, 0.0086, NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDIBLCB, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DROUT, 0.56, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PSCBE1, 4.24e8, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PSCBE1, 1.0e-5, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PVAG, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DELTA, 0.01, NULL, MCC_INITVALUE)      ;
	
/*----  IONIZATION CURRENT PARAMETERS  ------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPHA0, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPHA1, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BETA0, 30.0, NULL, MCC_INITVALUE)      ;

/*----  TEMPERATURE EFFECT PARAMETERS  ------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_KT1, -0.11, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KT1L, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KT2, -0.022, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UTE, -1.5, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UA1, 4.31e-9, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UB1, -7.61e-18, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AT, 3.3e4, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PRT, 0.0, NULL, MCC_INITVALUE)         ;

/*---------  DYNAMIC PARAMETERS  ------------*/

    version = MCC_ROUND(10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION));
    if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
      switch (version) {
        case 30: mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 1.0, NULL, MCC_INITVALUE)  ;
                 break;
        case 31: if (MCC_ROUND (LEVEL) == 49)
                   mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 0.0, NULL, MCC_INITVALUE)  ;
                 else
                   mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 2.0, NULL, MCC_INITVALUE)  ;
                 break;
        case 32: mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 3.0, NULL, MCC_INITVALUE)  ;
                 break;
      }
    }
    else {
      if(version == 31) 
         mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 1.0, NULL, MCC_INITVALUE)  ;
      else
         mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 3.0, NULL, MCC_INITVALUE)  ;
    }
        
    if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
      if ( MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_LEVEL) == 49 ))
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_XPART, 1.0, NULL, MCC_INITVALUE)       ;
      else
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_XPART, 0.0, NULL, MCC_INITVALUE)       ;
    }
    else 
	  mcc_initparam_quick(ptmodel, __MCC_QUICK_XPART, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGBO, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSL, 0.0, NULL, MCC_INITVALUE)        ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDL, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CKAPPA, 0.6, NULL, MCC_INITVALUE)      ;
	//mcc_initparam_quick(ptmodel, __MCC_QUICK_CF, 7.3e-5, NULL, MCC_INITVALUE)       ;
	//mcc_initparam_quick(ptmodel, __MCC_QUICK_CF, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CLC, 1.0e-7, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CLE, 0.6, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NOFF, 1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VOFFCV, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ACDE, 1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MOIN, 15.0, NULL, MCC_INITVALUE)       ;
	
/*----  NON QUASI STATIC PARAMETERS  --------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_NQSMOD, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ELM, 5.0, NULL, MCC_INITVALUE)         ;

/*---------  PROCESS PARAMETERS  ------------*/
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XT, 1.55e-7, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TOX, 1.5e-8, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XJ, 1.5e-7, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NSUB, 6.0e16, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NGATE, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ND, 1.0e20, NULL, MCC_INITVALUE)       ;

/*----  WIDTH AND LEGNTH PARAMETERS  --------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_LINT, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WINT, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DWG, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DWB, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WL, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WLN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WW, 0.0, NULL, MCC_INITVALUE)          ;  
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WWN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WWL, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LL, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LLN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LW, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LWN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LWL, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XL, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XW, 0.0, NULL, MCC_INITVALUE)          ;
	
/*---------  NOISE PARAMETERS  --------------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIMOD, 1.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AF, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KF, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EF, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EM, 4.1e7, NULL, MCC_INITVALUE)        ;

	if(ptmodel->TYPE == MCC_NMOS) {
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIA, 1.0e20, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIB, 5.0e4, NULL, MCC_INITVALUE)  ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIC, -1.4e-12, NULL, MCC_INITVALUE) ;
	}
	else {
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIA, 9.9e18, NULL, MCC_INITVALUE) ;
		mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIB, 2.4e3, NULL, MCC_INITVALUE)  ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIC, 1.4e-12, NULL, MCC_INITVALUE) ;
	}
	
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NSTAR, 2.0e20, NULL, MCC_INITVALUE)    ;

/*- JUNCTION CAPACITANCE / CHARGE MODEL PARAMETERS -*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_TPBSWG, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TCJSWG, 0.0, NULL, MCC_INITVALUE)      ;
	
/*-- Parameters for INTRINSIC CHARGE MODEL --*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_VFCBV, -1.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LVFCBV, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WVFCBV, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PVFCBV, 0.0, NULL, MCC_INITVALUE)      ;

/*************************************************************/
/*  Initialisation des param en fonction des param existants */
/*************************************************************/
    Cox  = MCC_EPSOX/mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX) ;
    
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TOXM, mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_TOX), 
                                   MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_DSUB, mcc_getparam_quick(ptmodel, __MCC_QUICK_DROUT), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_DROUT),
                                   MCC_INITVALUE) ; 
    
    mcc_initparam_quick(ptmodel, __MCC_QUICK_VFBCV, mcc_getparam_quick(ptmodel, __MCC_QUICK_VFB), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_VFB),
                                    MCC_INITVALUE) ; 

    /*- DIODE PARAMETERS (added in Version 3.2) -*/
    if(version == 32) {
       mcc_initparam_quick(ptmodel, __MCC_QUICK_TPB, 0.0, NULL, MCC_INITVALUE)     ;
       mcc_initparam_quick(ptmodel, __MCC_QUICK_TPBSW, 0.0, NULL, MCC_INITVALUE)   ;
       mcc_initparam_quick(ptmodel, __MCC_QUICK_TCJ, 0.0, NULL, MCC_INITVALUE)     ;
       mcc_initparam_quick(ptmodel, __MCC_QUICK_TCJSW, 0.0, NULL, MCC_INITVALUE)   ;
    }

    if ((MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)) == 1) || 
        (MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)) == 2))
         mcc_initparam_quick(ptmodel, __MCC_QUICK_UC, -4.65e-11, NULL, MCC_INITVALUE)    ;
    else if (MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)) == 3)
             mcc_initparam_quick(ptmodel, __MCC_QUICK_UC, -0.0465, NULL, MCC_INITVALUE) ;
  
    if ((MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)) == 1)
        || (MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)) == 2))
         mcc_initparam_quick(ptmodel, __MCC_QUICK_UC1, -5.6e-11, NULL, MCC_INITVALUE)    ;
    else if (MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)) == 3)
             mcc_initparam_quick(ptmodel, __MCC_QUICK_UC1, -0.056, NULL, MCC_INITVALUE) ;

    mcc_initparam_quick(ptmodel, __MCC_QUICK_DLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LINT), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_DWC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WINT), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WINT), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_LLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LL), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LL), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_WLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WL), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WL), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_LWC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LW), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LW), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_WWC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WW), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WW), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_LWLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LWL), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LWL), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_WWLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WWL), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WWL), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJ, 5e-04, NULL, MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSW, 5e-10, NULL, MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSWG, mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSW), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_CJSW), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSW, 0.33, NULL, MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSWG, mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSW), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_MJSW), MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_PJSWG, mcc_getparam_quick(ptmodel, __MCC_QUICK_PJSW), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PJSM), MCC_INITVALUE) ; 

    if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
      LEVEL = mcc_getparam_quick(ptmodel,__MCC_QUICK_LEVEL);
      if (MCC_ROUND (LEVEL) == 49) 
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ACM, 0.0, NULL, MCC_INITVALUE);
      else if (MCC_ROUND (LEVEL) == 53) 
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ACM, 10.0, NULL, MCC_INITVALUE);
      mcc_check_hsp_acm_bsim3v3 (ptmodel) ;
    // NB: sous Eldo aucune references concernant l initialisation de ACM
    //     cf ARLEV, ALEV, RLEV...
    }

    if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_HSPICE ) {
      mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSW, 1.0, NULL, MCC_INITVALUE) ; // p1316 doc hsp...
      ACM = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_ACM));
      if ( ACM >= 0 && ACM <= 3 ) 
        mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 0.8, NULL, MCC_INITVALUE) ;  // by test
      else
        mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 1.0, NULL, MCC_INITVALUE) ; 
    }
    else
      mcc_initparam_quick(ptmodel, __MCC_QUICK_PB, 1.0, NULL, MCC_INITVALUE) ; 

    mcc_initparam_quick(ptmodel, __MCC_QUICK_PHP, mcc_getparam_quick(ptmodel, __MCC_QUICK_PB), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PB), MCC_INITVALUE) ;

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PBSWG) == MCC_SETVALUE)
      {
       mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSW, mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWG), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PBSWG), MCC_INITVALUE) ;
      }
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSW, mcc_getparam_quick(ptmodel, __MCC_QUICK_PB), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PB), MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSWG, mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSW), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PBSW), MCC_INITVALUE) ; 

    if ((MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_NOIMOD)) == 2 ) ||
        (MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_NOIMOD)) == 3 )) 
         mcc_initparam_quick(ptmodel, __MCC_QUICK_FLKFLAG, 0.0, NULL, MCC_INITVALUE) ;

    /* Initialisation des parametres sans valeurs par defaut */

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDO) == MCC_INITVALUE) {
       if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DLC) == MCC_SETVALUE) 
            && (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) > 0.0)) {
            char buf[1024] ;
            char dlc[1024] ;
            char cgdl[1024] ;
            char *pt1 ;
            char *pt2 ;
            char *pt ;

            if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_DLC)) != NULL)
               sprintf(dlc,"%s",pt1) ;
            else
               sprintf(dlc,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC)) ;

            if((pt2 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_CGDL)) != NULL)
               sprintf(cgdl,"%s",pt2) ;
            else
               sprintf(cgdl,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDL)) ;

            if((pt1 != NULL) || (pt2 != NULL))
             {
              sprintf(buf,"%s * %g - %s",dlc,Cox,cgdl) ;
              pt = mbkstrdup(buf) ;
             }
            else
              pt = NULL ;

            mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) 
                         *Cox - mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDL)), pt, MCC_INITVALUE) ;
       if (mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO) < 0.0)
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, 0.0, NULL, MCC_INITVALUE) ;
       }
       else {
          char buf[1024] ;
          char xj[1024] ;
          char *pt1 ;
          char *pt ;
       
          if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_XJ)) != NULL)
             sprintf(xj,"%s",pt1) ;
          else
             sprintf(xj,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)) ;
       
          if(pt1 != NULL)
           {
            sprintf(buf,"0.6 * %s * %g",xj,Cox) ;
            pt = mbkstrdup(buf) ;
           }
          else
            pt = NULL ;
       
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, (0.6*mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)
                         *Cox), pt, MCC_INITVALUE) ;
       }
    }

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSO) == MCC_INITVALUE) {
       if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DLC) == MCC_SETVALUE) 
            && (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) > 0.0)) {
            char buf[1024] ;
            char dlc[1024] ;
            char cgsl[1024] ;
            char *pt1 ;
            char *pt2 ;
            char *pt ;

            if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_DLC)) != NULL)
               sprintf(dlc,"%s",pt1) ;
            else
               sprintf(dlc,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC)) ;

            if((pt2 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_CGSL)) != NULL)
               sprintf(cgsl,"%s",pt2) ;
            else
               sprintf(cgsl,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)) ;

            if((pt1 != NULL) || (pt2 != NULL))
             {
              sprintf(buf,"%s * %g - %s",dlc,Cox,cgsl) ;
              pt = mbkstrdup(buf) ;
             }
            else
              pt = NULL ;

            mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) 
                         *Cox - mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)), pt, MCC_INITVALUE) ;
       if (mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSO) < 0.0)
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, 0.0, NULL, MCC_INITVALUE) ;
       }
       else {
          char buf[1024] ;
          char xj[1024] ;
          char *pt1 ;
          char *pt ;
       
          if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_XJ)) != NULL)
             sprintf(xj,"%s",pt1) ;
          else
             sprintf(xj,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)) ;
       
          if(pt1 != NULL)
           {
            sprintf(buf,"0.6 * %s * %g",xj,Cox) ;
            pt = mbkstrdup(buf) ;
           }
          else
            pt = NULL ;
       
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, (0.6*mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)
                         *Cox), pt, MCC_INITVALUE) ;
       }
    }
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CF) == MCC_INITVALUE) {
        char buf[1024] ;
        char tox[1024] ;
        char *pt1 ;
        char *pt ;

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_TOX)) != NULL)
           sprintf(tox,"%s",pt1) ;
        else
           sprintf(tox,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX)) ;

        if(pt1 != NULL) 
         {
          sprintf(buf,"(2.0 * %g / %g) * log (1.0+4.07e-7 / %s)",
                       MCC_EPSOX,MCC_PI,tox) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_CF, ((2.0*MCC_EPSOX/MCC_PI)
                      *log(1.0+4.0e-7/mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX))), 
                      pt, MCC_INITVALUE) ;
    }

    if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NCH) == MCC_INITVALUE)
        && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA1) == MCC_SETVALUE)) {
        char buf[1024] ;
        char gamma1[1024] ;
        char *pt1 ;
        char *pt ;

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_GAMMA1)) != NULL)
           sprintf(gamma1,"%s",pt1) ;
        else
           sprintf(gamma1,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_GAMMA1)) ;

        if(pt1 != NULL) 
         {
          sprintf(buf,"((%s * %g )^2) / (2.0 * %g * %g)",
                       gamma1,Cox,MCC_Q,MCC_EPSSI) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_NCH, (pow(mcc_getparam_quick(ptmodel, __MCC_QUICK_GAMMA1)*Cox, 2.0) / (2.0*MCC_Q*MCC_EPSSI))
                      , pt, MCC_INITVALUE) ;
    }
    else if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NCH) == MCC_INITVALUE)
            && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA1) == MCC_INITVALUE))
        mcc_initparam_quick(ptmodel, __MCC_QUICK_NCH, 1.7e-17, NULL, MCC_INITVALUE) ;

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA1) == MCC_INITVALUE) {
        char buf[1024] ;
        char nch[1024] ;
        char *pt1 ;
        char *pt ;

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_NCH)) != NULL)
           sprintf(nch,"%s",pt1) ;
        else
           sprintf(nch,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_NCH)) ;

        if(pt1 != NULL) 
         {
          sprintf(buf,"sqrt (2.0 * %g * %g * %s) / %g",
                       MCC_Q,MCC_EPSSI,nch,Cox) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMMA1, (sqrt(2.0*MCC_Q*MCC_EPSSI*mcc_getparam_quick(ptmodel, __MCC_QUICK_NCH))/Cox)
                      , pt, MCC_INITVALUE) ;
    }

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA2) == MCC_INITVALUE) {
        char buf[1024] ;
        char nsub[1024] ;
        char *pt1 ;
        char *pt ;

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_NSUB)) != NULL)
           sprintf(nsub,"%s",pt1) ;
        else
           sprintf(nsub,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_NSUB)) ;

        if(pt1 != NULL) 
         {
          sprintf(buf,"sqrt (2.0 * %g * %g * %s) / %g",
                       MCC_Q,MCC_EPSSI,nsub,Cox) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMMA2, (sqrt(2.0*MCC_Q*MCC_EPSSI*mcc_getparam_quick(ptmodel, __MCC_QUICK_NSUB))/Cox)
                      , pt, MCC_INITVALUE) ;
    }
}

/****************************************************************************\
 FUNCTION : mcc_check_hsp_acm 
\****************************************************************************/
int mcc_check_hsp_acm_bsim3v3 (mcc_modellist *ptmodel) 
{
  int ACM=0; int acm_ko=0;
  int LEVEL;
  
  ACM = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_ACM));
  LEVEL = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_LEVEL));

  if ( ACM < 0 ) 
    acm_ko = 1;
  else if ( (ACM > 3 && ACM < 10) || ACM > 13 )
    acm_ko = 1;
  if ( acm_ko ) {
    avt_errmsg(MCC_ERRMSG, "004", AVT_ERROR,ACM);
    if ( LEVEL == 53 ) {
      avt_errmsg(MCC_ERRMSG, "005", AVT_ERROR);
      mcc_setparam ( ptmodel, "ACM", 10.0);
      ACM=10;
    }
    else if ( LEVEL == 49 ) {
      avt_errmsg(MCC_ERRMSG, "006", AVT_ERROR);
      mcc_setparam ( ptmodel, "ACM", 0.0);
      ACM=0;
    }
  }
  return ACM;
}


/************************************************************************/
/* Calcul du parametre de shrink DL d'un modele pour obtenir Leff       */
/* DL sera utilise pour le calcul du courant                            */
/************************************************************************/
double mcc_calcDL_bsim3v3(mcc_modellist *ptmodel, double L, double W)
{
    double Lprinted, Wprinted, dL ;
    double XL, XW, LINT, LL, LLN, LW, LWN, LWL ;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    LINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT)    ;
    LL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LL)      ;
    LW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LW)      ;
    LWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWL)     ;
    LLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLN)     ;
    LWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWN)     ;

    Lprinted   = L + XL ;
    Wprinted   = W + XW ;
    
    dL         = LINT + LL/pow(Lprinted, LLN) + LW/pow(Wprinted, LWN) 
                 + LWL/(pow(Lprinted,LLN) * pow(Wprinted, LWN)) ;

    return(XL - 2.0 * dL) ;
}

/************************************************************************/
/* Calcul du parametre de shrink DLC d'un modele pour obtenir Lactive   */
/* DLC sera utilise pour le calcul des capacites                        */
/************************************************************************/
double mcc_calcDLC_bsim3v3(mcc_modellist *ptmodel, double L, double W)
{
    double Lprinted, Wprinted, dLcv ;
    double XL, XW, DLC, LLC, LLN, LWC, LWN, LWLC ;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    DLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC)     ;
    LLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLC)     ;
    LWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWC)     ;
    LWLC = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWLC)    ;
    LLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLN)     ;
    LWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWN)     ;

    Lprinted   = L + XL ;
    Wprinted   = W + XW ;
    
    dLcv       = DLC + LLC/pow(Lprinted, LLN) + LWC/pow(Wprinted, LWN) 
                 + LWLC/(pow(Lprinted,LLN) * pow(Wprinted, LWN)) ;

    return(XL - 2.0 * dLcv) ;
}

/************************************************************************/
/* Calcul du parametre de shrink DW d'un modele pour obtenir Weff       */
/* DW sera utilise pour le calcul du courant                            */
/************************************************************************/
double mcc_calcDW_bsim3v3(mcc_modellist *ptmodel, double L, double W)
{
    double Lprinted, Wprinted, dW ;
    double XL, XW, WINT, WL, WLN, WW, WWN, WWL ;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    WINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_WINT)    ;
    WL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WL)      ;
    WW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WW)      ;
    WWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWL)     ;
    WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
    WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;

    Lprinted   = L + XL ;
    Wprinted   = W + XW ;
    
    dW         = WINT + WL/pow(Lprinted, WLN) + WW/pow(Wprinted, WWN) 
                 + WWL/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;

    return(XW - 2.0 * dW) ;
}

/************************************************************************/
/* Calcul du parametre de shrink DWC d'un modele pour obtenir Wactive   */
/* DWC sera utilise pour le calcul des capacites                        */
/************************************************************************/
double mcc_calcDWC_bsim3v3(mcc_modellist *ptmodel, double L, double W)
{
    double Lprinted, Wprinted, dWcv ;
    double XL, XW, DWC, WLC, WLN, WWC, WWN, WWLC ;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    DWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_DWC)     ;
    WLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLC)     ;
    WWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWC)     ;
    WWLC = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWLC)    ;
    WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
    WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;

    Lprinted   = L + XL ;
    Wprinted   = W + XW ;
    
    dWcv       = DWC + WLC/pow(Lprinted, WLN) + WWC/pow(Wprinted, WWN) 
                 + WWLC/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;

    return(XW - 2.0 * dWcv) ;
}

/*****************************************************************************************************/
/* Fonction qui calcule les tesions de seuil VTH d'un transistor en fct de L, W et de la temperature */ 
/* LA FONCTION DE CALCUL DE VTH CONSIDERE LE MODELE BSIM3V3 et EGALEMENT Vbstrue  = 0                */
/* La variation en fonction de la temperature est : VTH(T) = VTH(Tnom) + DVTH(T)                     */
/*                                                                                                   */
/* CAS PARTICULIER POUR LE CALCUL DE VTH D'UN TRANSISTOR P :                                         */
/* Pour calculer VTH d'un transistor P, il faut se mettre dans les memes conditions qu'un NMOS       */
/* 1) On inverse le parametre VTH0 du PMOS                                                           */
/* 2) On inverse les tensions appliquees aux bornes du PMOS <=> vbs et vds deviennent positifs       */
/* 3) On calcule donc VTH du PMOS comme pour un NMOS puis on inverse le resultat final car VTHP < 0  */
/*****************************************************************************************************/
double	mcc_calcVTH_bsim3v3 (mcc_modellist *ptmodel, double L, double W, double temp, double vbstrue, 
                             double vds, elp_lotrs_param *lotrsparam, int mcclog)
{
 static mcc_modellist *previous_model=NULL ;
 static double previous_L=0.0 ;
 static double previous_W=0.0 ;
 static int calcul_dimension ;

 // Parametres generaux ou presents dans les parametres du modele
 
 static int UPDATEPHI = 0 ;

 static double binunit ;
 static double KT2, Cox ;
 static double ND, NI, tnomk ;
 static double VTH0, K1, K2 ,K1ox, K2ox, K3, K3B, TOX, TOXM,W0, NLX;
 static double DVT0, DVT1, DVT2,DSUB ;
 static double NCH, ETA0, ETAB, DVT0W, DVT1W, DVT2W, KT1, KT1L ;
 
 // Parametres intermediaires pour le calcul de VTH
  
 static double T, VtT, EgT, niT, PhiT, VbiT ;  
 static double PhisT ;
 static double Leff, Weff, DVth0T, Xdep ;
 static double Xdep0 ;
 static double Lt, Ltw, Lt0 ;                              
 static double Pdibl, Theta0w, Theta0, DVth0w, DVth0 ;
 
 // Param pour Vbs
 static double delta1,vbsc, vbs ;           
 // finalement notre tension de seuil
 
 static double VTHfinal ; 
 static int version;
 
 static chain_list *previous_longkey=NULL;
 	
 /*************************************/
 /**   DEBUT des calculs             **/
 /*************************************/
 if ( !mcc_is_same_model (ptmodel,L,W,1.0,previous_model,previous_L,previous_W,1.0,lotrsparam,previous_longkey) ) 
     calcul_dimension = 1 ;
 else
     calcul_dimension = 0 ;
 
 /******* Initialisation des parametres generaux  *******/
 
  NI      = 1.45e10 *1e6      ; 
  delta1  = 1.0e-3            ;
  
 /******* Obtention des parametres du modele  ************/
 if ( calcul_dimension ) {
   ND   = 1.0e6*mcc_getparam_quick(ptmodel, __MCC_QUICK_ND)  ;
   // param pour le calcul des Leff et Weff
   TOX  = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX)   ;
   TOXM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM)   ;
   Leff = mcc_calcLeff_bsim3v3(ptmodel, L, W);
   Weff = mcc_calcWeff_bsim3v3(ptmodel, L, W);
   version = MCC_ROUND(10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION));
 }
 // Logs
 if ( avt_islog(2,LOGMCC) ) {
   avt_log(LOGMCC,2,"--------- [Begin mcc_calcVTH_bsim3v3] ---------\n");
   avt_log(LOGMCC,2,"model %s, L=%g, W=%g, temp=%g, vbs=%g, vds=%g\n",
                            ptmodel->NAME,L,W,temp,vbstrue,vds);
   if ( ptmodel->SUBCKTNAME )
     avt_log(LOGMCC,2,"Subcktname: %s\n",ptmodel->SUBCKTNAME);
   avt_log(LOGMCC,2,"Leff = %g, Weff = %g\n",Leff,Weff);
 }

 /*-----------------------------------------------*/
 /*   COMPUTATION OF BINNING PARAMETERS           */
 /*-----------------------------------------------*/
 if (calcul_dimension == 1) {
   binunit = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT)       ;   
   K1    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K1,mcclog)      ;
   K2    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K2,mcclog)      ;
   K3    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K3,mcclog)      ;
   K3B   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K3B,mcclog)     ;
   W0    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_W0,mcclog)      ;
   NLX   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NLX,mcclog)     ;
   DVT0  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT0,mcclog)    ;
   DVT1  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT1,mcclog)    ;
   DVT2  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT2,mcclog)    ;
   DVT0W =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT0W,mcclog)   ;
   DVT1W =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT1W,mcclog)   ;
   DVT2W =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT2W,mcclog)   ;
   DSUB  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DSUB,mcclog)    ;
   if ( mcc_getparamtype_quick(ptmodel,__MCC_QUICK_NCH) == MCC_SETVALUE )
     NCH   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NCH,mcclog); 
   else
     NCH   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NPEAK,mcclog); 
   if(NCH < 1.0e20)
       NCH = NCH *1.0e6 ;
   ETA0  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_ETA0,mcclog)    ;
   ETAB  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_ETAB,mcclog)    ;
   KT1   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KT1,mcclog)     ;
   KT2   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KT2,mcclog)     ;
   KT1L  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KT1L,mcclog)    ;
 }
 /* END OF BINNING PROCESS */

 
/******* Premiere serie de calculs pour les parametres intermediaires ********/
 
 tnomk   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) + MCC_KELVIN  ;
 T         = temp + MCC_KELVIN ;
 
 if (UPDATEPHI == 0){
 	VtT    = MCC_KB * tnomk / MCC_Q    ;
 	EgT    = 1.16-7.02e-4*pow(tnomk, 2.0)/(tnomk+1108.0) ;
 	niT    = NI * pow((tnomk/300.15), 1.5)*exp(21.5565981 - EgT/(2.0*VtT)) ;
 }
 else if (UPDATEPHI == 1){
 	VtT    = MCC_KB * T / MCC_Q    ;
 	EgT    = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ; 
 	niT    = NI * pow((T/300.15), 1.5)*exp(21.5565981 - EgT/(2.0*VtT)) ;
 }
 	PhiT   = 2.0 * VtT * log (NCH/niT) ;
 	VbiT   = VtT * log (ND*NCH/pow(niT, 2.0));
 
 if(version == 32) {
     K1ox = K1*(TOX/TOXM);
     K2ox = K2*(TOX/TOXM);
 }
 else {
     K1ox = K1 ;
     K2ox = K2 ;
 }

 if(ptmodel->TYPE == MCC_PMOS) {
	 vds = -vds ; 
	 vbstrue = -vbstrue ;
 }

 if((K2 != 0.0) && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_K2) == MCC_SETVALUE)) { 
     vbsc = 0.9*(PhiT-pow(K1ox, 2.0) / (4.0*pow(K2,2.0))) ;
     
     /*---------------------------------------------------------------------------------------------------*/
     vbs = vbsc + 0.5*( vbstrue - vbsc - delta1 + sqrt(pow(vbstrue-vbsc-delta1, 2.0) - 4.0*delta1*vbsc ) ) ;
     /*---------------------------------------------------------------------------------------------------*/
 }
 else 
     vbs = vbstrue ; 

 if(vbs <= 0.0)
	 PhisT  = PhiT - vbs ; 
 else {
	 PhisT  = pow((PhiT*sqrt(PhiT))/(PhiT + 0.5 * vbs), 2.0) ;
 }
 
 Cox        = MCC_EPSOX / TOX                      ;

 /*----------------------------------------*/
 if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTH0) == MCC_SETVALUE) {
 //    VTH0 = mcc_getprm (ptmodel,"VTH0") + mcc_getprm (ptmodel,"DELVT0");
     VTH0 = mcc_getprm_quick (ptmodel,__MCC_GETPRM_VTH0) + mcc_getprm_quick (ptmodel,__MCC_GETPRM_DELVTO);
     VTH0 = mcc_update_vth0 ( lotrsparam , VTH0 );
     VTH0 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VTH0,VTH0,mcclog) ;
 }
 else
     VTH0 = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VFB,mcclog) + PhiT + K1ox*sqrt(PhiT) ;
 if (ptmodel->TYPE == MCC_PMOS)
     VTH0 = -VTH0;
 
 DVth0T     = (KT1+KT1L/Leff+KT2*vbs)*(T/tnomk-1.0) ;
 Xdep       = sqrt((2.0*MCC_EPSSI/(MCC_Q*NCH)))*sqrt(PhisT) ; 
 Xdep0      = sqrt((2.0*MCC_EPSSI*PhiT)/(MCC_Q*NCH)) ;
 
 Lt         = sqrt(MCC_EPSSI*Xdep/Cox)*(1.0 + DVT2 * vbs) ;
 Lt0        = sqrt(MCC_EPSSI*Xdep0/Cox) ;
 Ltw        = sqrt(MCC_EPSSI*TOX*Xdep/MCC_EPSOX)*(1.0 + DVT2W * vbs) ; 
 Pdibl      = vds*(ETA0 + ETAB * vbs)*( exp(-DSUB*Leff/(2.0*Lt0)) + 2.0 * exp(-DSUB*Leff/Lt0) ) ;
 Theta0w    = exp(-DVT1W*Leff*Weff/(2.0*Ltw)) +2.0 * exp(-DVT1W*Leff*Weff/Ltw) ;
 Theta0     = exp(-DVT1*Leff/(2.0*Lt)) + 2.0 * exp(-DVT1*Leff/Lt) ;
 DVth0w     = DVT0W * (VbiT - PhiT)*Theta0w ;
 DVth0      = DVT0  * (VbiT - PhiT)*Theta0  ;
 
/********              Equation finale de VTH                        ********/
 VTHfinal =  VTH0 + K1ox * sqrt(PhisT) - K1 * sqrt(PhiT) - K2ox * vbs 
             - DVth0 - DVth0w + DVth0T - Pdibl + (K3 + K3B * vbs)*TOX*PhiT/(Weff+W0)
             + K1ox*sqrt(PhiT)*(sqrt(1.0+NLX/Leff)-1.0) ; 

 if (ptmodel->TYPE == MCC_PMOS) 
   VTHfinal =  - VTHfinal;

 // LOGS...
 if ( mcclog == MCC_DRV_LOG ) {
   avt_log(LOGMCC,2,"[Main params] VTH0=%g, K1ox=%g, sqrt(PhisT)=%g, sqrt(PhiT)=%g, TOX=%g\n",
                                          VTH0,K1ox,sqrt(PhisT),sqrt(PhiT), TOX);
   avt_log(LOGMCC,2,"[Main params] K2ox=%g, vbs=%g, DVth0=%g, DVth0w=%g, Pdibl=%g\n",
                                          K2ox,vbs,DVth0,DVth0w,Pdibl);
   avt_log(LOGMCC,2,"[params] Dvth0=%g, Dvth0w=%g, Theta0=%g, Theta0w=%g, Ltw=%g\n",
                                     DVth0,DVth0w,Theta0,Theta0w,Ltw);
   avt_log(LOGMCC,2,"[params] Lt0=%g, Lt=%g, Xdep0=%g, Xdep=%g, DVth0T=%g\n",
                                     Lt0,Lt,Xdep0,Xdep,DVth0T);
   avt_log(LOGMCC,2,"[Sub params] Cox=%g, PhisT=%g, PhiT=%g, VbiT=%g, tnomk=%g, niT=%g\n",
                                         Cox,PhisT,PhiT,VbiT,tnomk,niT);
   avt_log(LOGMCC,2,"### VTHfinal = %g ###\n",VTHfinal);
   avt_log(LOGMCC,2,"--------- [ end  mcc_calcVTH_bsim3v3] ---------\n\n");
 }

 previous_model = ptmodel ;
 previous_L = L ;
 previous_W = W ;
 if (calcul_dimension)
 {
   freechain(previous_longkey);
   previous_longkey=dupchainlst(lotrsparam->longkey);
 }

 return VTHfinal ;
 
}


/********************************************************************************************/
/* Fonction qui calcule le courant Ids pour BSIM3V3 en fonction des parametres du modele    */
/* ainsi que de Vbs, Vgs, Vds,VTH, W, L, temp                                               */
/* NB sur les valeurs effectives utilisees: Vgseff  = Vgs si param NGATE non fourni         */
/*                                          Vgsteff = (Vgs-Vth)eff                          */
/*                                                                                          */
/* CAS DU CALCUL DE IDS D'UN TRANSISTOR P :                                                 */
/* ELDO calcul ids comme pour un transistor N, il ne prend que les valeurs positives de VTH */
/* ainsi que des tensions Vgs et Vds. ELDO renvoi finalement l'oppose du courant calcule    */
/* dans le cas d'un transistor P.                                                           */                      
/********************************************************************************************/
static inline double mcc_calc_vgseff(double Vgs, double Vfb, double PhiT, double Cox, double NGATE)
{
  double T0, T1, T2, T3, T4, T5, T6, T7;
  double Vgseff;

  T0 = Vfb + PhiT;
  if ( Vgs > T0 ) {
    /* added to avoid the problem caused by ngate */
    T1 = MCC_Q * MCC_EPSSI * NGATE / (Cox*Cox);
    T4 = sqrt(1.0 + 2.0 * (Vgs - T0) / T1);
    T2 = T1 * (T4 - 1.0);
    T3 = 0.5 * T2 * T2 / T1; /* T3 = Vpoly */
    T7 = 1.12 - T3 - 0.05;
    T6 = sqrt(T7 * T7 + 0.224);
    T5 = 1.12 - 0.5 * (T7 + T6);
    Vgseff = Vgs - T5;
  }
  else
    Vgseff = Vgs;

  return Vgseff;
}

double mcc_calcIDS_bsim3v3(mcc_modellist *ptmodel, double Vbstrue, double Vgs, double Vds, 
                           double W, double L, double Temp,
                           elp_lotrs_param *lotrsparam)
{
 static int UPDATEPHI = 0 ; 
 static double binunit ;
 static mcc_modellist *previous_model=NULL ;
 static double previous_L=0.0 ;
 static double previous_W=0.0 ;
 static int    calcul_dimension ;

 //double IdiodeDrain = 0.0,IdiodeSource = 0.0,Vbd ;

 /*********************************************************/
 /* parametres principaux dans l'expression de Ids        */
 /*********************************************************/
  
 static double VTH     ;                    // TENSION DE SEUIL
 static double Ids     ;                    // Courant de drain
 static double Rds     ;                    // Channel resistance
 static double Vdseff  ;                    // Effective vds
 static double Va      ;                    // EARLY voltage
 static double Vascbe  ;                    // EARLY voltage due to substrate current
 static double Ids0    ;                    // Linear drain current in the channel

 static double delta1, K2, Vbsc, Vbs ;      // Pour recalculer Vbs avec precision (faible influence)    
 
 /*------------------------------*/
 /* parametres pour calculer Rds */
 /*------------------------------*/

 static double RDSW, PRT, tnom, T, RdswT ;
 static double PRWG, Vgsteff, PRWB, PhisT, PhiT, Weff, WR ;
 
 // Rds -> PhiT
 static double EgT, NI, niT ;
 
 
 // Rds -> Vgsteff
 static double Vgseff, NGATE, n, VtT, Cox, NCH, VOFF ; 

 // RDS -> Vgsteff -> Vgseff
 static double Vfb, K ;
 
 // Rds -> Vgsteff -> n
 static double NFACTOR, Cd, Lt, DVT1, DVT2, Theta0 ;
 static double CDSC, CDSCB, CDSCD, CIT ;
 
 // Rds -> Vgsteff -> n -> Cd
 static double Xdep0, Xdep ;

 // Rds -> Vgsteff -> Cox
 static double TOX,TOXM ;
 
 
 /*---------------------------------*/
 /* parametres pour calculer Vdseff */
 /*---------------------------------*/

 static double Vdsat, V1, DELTA ;

 /*  Parametres pour calculer Vdsat qui est calculee par rapport a la valeur de Rds et lambda */
 
 //  Vdseff -> Vdsat : param pour lambda
 static double A1, A2, lambda ;
 
 //  Vdseff -> Vdsat 
 static double Esat, Leff, a, b, c, Abulk ;
 
 // Vdseff -> Vdsat -> Esat : depend de VsatT et de ueffT
 static double VSAT, AT, VsatT ;
 
 static double MOBMOD, U0, UTE, u0T ;
 static double UA, UA1, uaT, UB, UB1, ubT, UC, UC1, ucT ;
 static double ueffT ;

 // Vdseff -> Vdsat -> Abulk  
 static double K1, K1ox, A0, B0, B1, XJ, KETA, Abulk0 ;
 static double AGS ;
 
 /*------------------------------*/
 /* parametres pour calculer Va  */
 /*------------------------------*/

 static double Vasat, Vaclm, Vadibl, PVAG ;

 // Va -> Vasat : dpd juste du Rfactor
 static double Rfactor ;

 // Va -> Vaclm 
 static double Litl, PCLM ;

 // Va -> Vadibl
 static double Thetarout, PDIBLCB ;
 
 // Va -> Vadibl -> Thetarout
 static double PDIBLC1, PDIBLC2, DROUT, Lt0 ;
	
 /*---------------------------------*/
 /* parametres pour calculer Vascbe */
 /*---------------------------------*/

 static double PSCBE1, PSCBE2 ;

 /*-------------------------------*/
 /* parametres pour calculer Ids0 */
 /*-------------------------------*/
 
 // All params have been defined

 
 /*-------------------------------*/
 /*      Dimensions effectives    */
 /*-------------------------------*/
 static double DWG, DWB ; // bias dependencies for Weff
 static double Weff_v ;
 static int version;
 static chain_list *previous_longkey=NULL;
 
 if (Vds == 0.0)
   return 0.0;

 /********************************************************************/
 /*                         VALEURS GENENRALES                       */
 /********************************************************************/

 NI      = 1.45e10*1.0e6;
 delta1  = 1.0e-3;
 
 
 /********************************************************************************/
 /*                         OBTENTION PARAMETRES DU MODEL                        */
 /********************************************************************************/
 
 if ( !mcc_is_same_model (ptmodel,L,W,1.0,previous_model,previous_L,previous_W,1.0,lotrsparam,previous_longkey) ) 
     calcul_dimension = 1 ;
 else
     calcul_dimension = 0 ;
 
 tnom = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) + MCC_KELVIN ;
 T    = Temp + MCC_KELVIN                          ;

 if ( calcul_dimension ) {
     
   TOX  = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOX)           ;
   TOXM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM)           ;

 /*****************************************************************/
 /*               Calcul de Leff et Weff                          */ 
 /*****************************************************************/
   Leff = mcc_calcLeff_bsim3v3(ptmodel, L, W);
   Weff = mcc_calcWeff_bsim3v3(ptmodel, L, W);
 }

 
 /*********************************************************/
 /* CALCUL DE VTH  et inversion des tensions pour le PMOS */
 /*********************************************************/
 
 VTH = mcc_calcVTH_bsim3v3(ptmodel, L, W, Temp, Vbstrue, Vds,lotrsparam,MCC_NO_LOG) ;
 if (ptmodel->TYPE == MCC_PMOS) {
 	VTH = -VTH ;
    Vbstrue = -Vbstrue ;
    Vgs = fabs(Vgs) ;
    Vds = fabs(Vds) ;
 }

 /*****************************************************************/
 /*                         BINNING PROCESS                       */
 /*****************************************************************/
 if (calcul_dimension == 1) {
   binunit = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT)       ;   
  
   DWG   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DWG,MCC_NO_LOG)       ;
   DWB   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DWB,MCC_NO_LOG)       ;
   if ( mcc_getparamtype_quick(ptmodel,__MCC_QUICK_NCH) == MCC_SETVALUE )
     NCH   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NCH,MCC_NO_LOG); 
   else
     NCH   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NPEAK,MCC_NO_LOG); 
   if(NCH <= 1.0e20)
       NCH = NCH *1.0e6 ;
   NGATE =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NGATE,MCC_NO_LOG)     ;
   if(NGATE <= 1.0e23)
       NGATE = NGATE *1.0e6 ;
   K1    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG)        ;
   K2    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K2,MCC_NO_LOG)        ;
   DVT1  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT1,MCC_NO_LOG)      ;
   DVT2  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT2,MCC_NO_LOG)      ;
   // For RDS
   RDSW  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_RDSW,MCC_NO_LOG)      ;
   PRT   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PRT,MCC_NO_LOG)       ; 
   PRWB  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PRWB,MCC_NO_LOG)      ;
   PRWG  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PRWG,MCC_NO_LOG)      ;
   WR    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_WR,MCC_NO_LOG)        ; 
   NFACTOR =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NFACTOR,MCC_NO_LOG) ; 
   CDSC  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSC,MCC_NO_LOG)      ;
   CDSCB =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSCB,MCC_NO_LOG)     ;
   CDSCD =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSCD,MCC_NO_LOG)     ;
   CIT   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CIT,MCC_NO_LOG)       ; 
   VOFF  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VOFF,MCC_NO_LOG)      ; 
   // For Vdseff
   A1    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_A1,MCC_NO_LOG)        ;
   A2    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_A2,MCC_NO_LOG)        ;
   VSAT  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VSAT,MCC_NO_LOG)      ;
   AT    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_AT,MCC_NO_LOG)         ;
   UA    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UA,MCC_NO_LOG)        ;
   UB    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UB,MCC_NO_LOG)        ;
   UC    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UC,MCC_NO_LOG)        ;
   UA1   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UA1,MCC_NO_LOG)       ;
   UB1   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UB1,MCC_NO_LOG)       ;
   UC1   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UC1,MCC_NO_LOG)       ; 
   UTE     =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_UTE,MCC_NO_LOG)     ;
   // For Vdseff -> Vdsat
   A0      =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_A0,MCC_NO_LOG)      ; 
   B0      =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_B0,MCC_NO_LOG)      ;
   B1      =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_B1,MCC_NO_LOG)      ;
   XJ      =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_XJ,MCC_NO_LOG)      ;
   KETA    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KETA,MCC_NO_LOG)    ;
   AGS     =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_AGS,MCC_NO_LOG)     ; 
   DELTA   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DELTA,MCC_NO_LOG)   ; 
   // For Vascbe
   PSCBE1  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PSCBE1,MCC_NO_LOG)  ; 
   PSCBE2  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PSCBE2,MCC_NO_LOG)  ;
   // For Va
   PDIBLC1 =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PDIBLC1,MCC_NO_LOG) ;
   PDIBLC2 =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PDIBLC2,MCC_NO_LOG) ;
   DROUT   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DROUT,MCC_NO_LOG)   ; 
   PDIBLCB =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PDIBLCB,MCC_NO_LOG) ;
   PCLM    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PCLM,MCC_NO_LOG)    ; 
   PVAG    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_PVAG,MCC_NO_LOG)    ; 
   version = MCC_ROUND(10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION));
 }
 U0 = mcc_getparam_quick(ptmodel,__MCC_QUICK_U0) * mcc_getparam_quick(ptmodel,__MCC_QUICK_MULU0);
 U0 = mcc_update_u0 ( lotrsparam, U0 );
 U0 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_U0,U0,MCC_NO_LOG) ;
 if(U0 > 1.0)
     U0 = U0 * 1.0e-4 ;
 

 /*****************************************************************/
 /*                         BEGIN EQUATIONS                       */
 /*****************************************************************/

 if(version == 32) 
   K1ox = K1*(TOX/TOXM) ;
 else 
   K1ox = K1 ;

 if (UPDATEPHI == 0) {
    VtT    = MCC_KB * tnom / MCC_Q    ;
 	EgT    = 1.16-7.02e-4*pow(tnom, 2.0)/(tnom+1108.0) ; 
 	niT    = NI * pow((tnom/300.15), 1.5)*exp(21.5565981 - EgT/(2.0*VtT)) ;
 }
 if (UPDATEPHI == 1){
    VtT    = MCC_KB * T / MCC_Q    ;
 	EgT    = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ; 
 	niT    = NI * pow((T/300.15), 1.5)*exp(21.5565981 - EgT/(2.0*VtT)) ;
 }
 PhiT   = 2.0 * VtT * log (NCH/niT) ;

 VtT    = MCC_KB * T / MCC_Q    ;
 
 
 /*---------------------------------------*/
 /* Calcul de vbsfinal                    */
 /*---------------------------------------*/
 
 if((K2 != 0.0) && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_K2) == MCC_SETVALUE)) { 
     Vbsc = 0.9*(PhiT-pow(K1ox, 2.0) / (4.0*pow(K2,2.0))) ;
 
     /*---------------------------------------------------------------------------------------------------*/
     Vbs = Vbsc + 0.5*( Vbstrue - Vbsc - delta1 + sqrt(pow(Vbstrue-Vbsc-delta1, 2.0) - 4.0*delta1*Vbsc ) ) ;
     /*---------------------------------------------------------------------------------------------------*/
 } 
 else 
     Vbs = Vbstrue ;
 
 if(Vbs <= 0.0)
	 PhisT  = PhiT - Vbs ;
 else
	 PhisT  = pow((PhiT*sqrt(PhiT)/(PhiT + 0.5 * Vbs)), 2.0) ;
	 
 Cox        = MCC_EPSOX / TOX                      ; 
 
 Xdep       = sqrt((2.0*MCC_EPSSI/(MCC_Q*NCH)))*sqrt(PhisT) ; 
 Xdep0      = sqrt((2.0*MCC_EPSSI*PhiT)/(MCC_Q*NCH)) ;
 
 
 Lt         = sqrt(MCC_EPSSI*Xdep/Cox)*(1.0 + DVT2 * Vbs) ;
 Lt0        = sqrt(MCC_EPSSI*Xdep0/Cox) ;
 
 
 /*########################*/
 /*  Calcul de Rds         */ 
 /*########################*/
 
 RdswT      = RDSW + PRT * (T/tnom - 1.0 )       ;
 
 Cd         = MCC_EPSSI / Xdep                     ;
 
 Theta0     = exp(-(DVT1*Leff)/(2.0*Lt)) + 2.0 * exp(-(DVT1*Leff)/Lt) ;
 n          = 1.0 + NFACTOR * Cd/Cox + Theta0*(CDSC + CDSCB*Vbs + CDSCD*Vds)/Cox + CIT/Cox ;
 
 /* Expression de Vgseff en fonction du parametre NGATE */
 
 if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NGATE) == MCC_SETVALUE) {
     if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VFB) != MCC_SETVALUE){
         // correction fabrice au bug sony avec une techno toshiba
         // anciennement: Vfb = VTH - PhiT - K1ox * sqrt(PhiT) ;
         double VTH0;

         //VTH0 = mcc_getprm (ptmodel,"VTH0") + mcc_getprm (ptmodel,"DELVT0");
         VTH0 = mcc_getprm_quick (ptmodel,__MCC_GETPRM_VTH0) + mcc_getprm_quick (ptmodel,__MCC_GETPRM_DELVTO);
         VTH0 = mcc_update_vth0 ( lotrsparam , VTH0 );
         VTH0 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VTH0,VTH0,MCC_NO_LOG) ;

         Vfb = fabs(VTH0) - PhiT - K1 * sqrt(PhiT) ;
         //Vfb = fabs(mcc_getparam_quick(ptmodel, __MCC_QUICK_VTH0)) - PhiT - K1 * sqrt(PhiT) ;
     }
     else{
         Vfb = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG) ;
     }
     K   = MCC_Q*MCC_EPSSI*NGATE/pow(Cox, 2.0) ;
     if (K != 0.0)
     {
       //Vgseff = Vfb + PhiT + K*( sqrt(1.0+(2.0*(Vgs-Vfb-PhiT)/K)) - 1.0 ) ;
       Vgseff = mcc_calc_vgseff(Vgs, Vfb, PhiT, Cox, NGATE);
     }
     else
       Vgseff = Vgs ;
 }
 else
     Vgseff = Vgs ;
 
 /******** SUBTHRESHOLD   *******/

 Vgsteff    = ( 2.0*n*VtT * log(1.0 + exp((Vgseff - VTH) / (2.0*n*VtT))) ) / 
              (1.0 + 2.0*n*Cox*sqrt(2.0*PhiT/(MCC_Q*MCC_EPSSI*NCH))*exp(-(Vgseff-VTH-2.0*VOFF)/(2.0*n*VtT))) ;

 /* bias dependencies for Weff_v */
 Weff_v = Weff-2.0*DWG*Vgsteff-2.0*DWB*(sqrt(PhisT)-sqrt(PhiT)) ;
 
 /*  Attention car le parametre RDSW represente une resistivite par rapport a W      */
 /*  ET RDSW est exprime en ohm.um   c est pourkoi on utilise (Weff*1e6) et non Weff   */ 
 
 /*-------------------------------------------------------------------------------------------------*/
 Rds        = (RdswT*(1.0 + PRWG*Vgsteff + PRWB*(sqrt(PhisT) - sqrt(PhiT)))) / (pow((Weff*1e6), WR)) ;
 /*-------------------------------------------------------------------------------------------------*/

 /*##########################*/
 /*  Calcul de Vdseff        */ 
 /*##########################*/
 
 lambda = A1*Vgsteff + A2                  ;

 VsatT  = VSAT - AT*(T/tnom - 1.0)         ;       
 u0T    = U0 * pow((T/tnom), UTE)          ; 
 uaT    = UA + UA1 * (T/tnom - 1.0)        ;
 ubT    = UB + UB1 * (T/tnom - 1.0)        ;
 ucT    = UC + UC1 * (T/tnom - 1.0)        ;
 
 // calcule de ueff en fct de la valeur de MOBMOD
 MOBMOD     = mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD)  ;
 
 if (MCC_ROUND (MOBMOD) == 1)
	 ueffT   = (u0T / ( 1.0 + (uaT + ucT*Vbs) * (Vgsteff+2.0*VTH)/TOX +ubT*pow((Vgsteff+2.0*VTH)/TOX, 2.0) )) ; 
 else if (MCC_ROUND (MOBMOD) == 2)
         ueffT = u0T / ( 1.0 + (uaT + ucT*Vbs) * Vgsteff/TOX +ubT*pow(Vgsteff/TOX, 2.0) ) ;
 else if (MCC_ROUND (MOBMOD) == 3)
         ueffT = u0T / ( 1.0 + (uaT*(Vgsteff+2.0*VTH)/TOX +ubT*pow((Vgsteff+2.0*VTH)/TOX, 2.0))*(1.0 + ucT*Vbs) ) ;
 else  {
     avt_errmsg(MCC_ERRMSG, "007", AVT_ERROR);
	 ueffT   = (u0T / ( 1.0 + (uaT + ucT*Vbs) * (Vgsteff+2.0*VTH)/TOX +ubT*pow((Vgsteff+2.0*VTH)/TOX, 2.0) )) ; 
 }
	 
 Esat       = 2.0*VsatT / ueffT                  ;
 Rfactor    = Weff_v*VsatT*Cox*Rds               ;

 /**** Calcule de Vdsat  ****/
 
 // D'abord Vdsat -> Abulk
 
 Abulk0     = (1.0 + K1ox/(2.0*sqrt(PhisT)) * (A0*Leff/(Leff + 2.0*sqrt(XJ*Xdep)) 
              + B0/(Weff + B1))) * 1.0 / (1.0+(KETA*Vbs)) ;
 
 if ( Abulk0 < 0.1 ) {
   a = 1.0 / (3.0 - 20.0 * Abulk0);
   Abulk0 = (0.2 - Abulk0) * a;
 }
 
 Abulk      = Abulk0 - AGS*A0*(Vgsteff/(1.0+KETA*Vbs))*(K1ox/(2.0*sqrt(PhisT)))
              *pow((Leff/(Leff+2.0*sqrt(XJ*Xdep))) , 3.0)  ;
 
 if ( Abulk < 0.1 ) {
   a = 1.0 / (3.0 - 20.0 * Abulk);
   Abulk = (0.2 - Abulk) * a;
 }
 
 a          = pow(Abulk, 2.0) * Rfactor + ((1.0/lambda)-1.0) * Abulk ;
 b          = -(Vgsteff+2.0*VtT)*((2.0/lambda)-1.0) - Abulk * (Esat*Leff + 3.0*Rfactor*(Vgsteff+2.0*VtT)) ;
 c          = Esat*Leff*(Vgsteff + 2.0*VtT) + 2.0*Rfactor*pow((Vgsteff+2.0*VtT), 2.0) ;
 
 if ((Rds == 0.0) && (lambda == 1.0)) {
       Vdsat = (Esat*Leff*(Vgsteff + 2.0*VtT)) / (Abulk*Esat*Leff + Vgsteff + 2.0*VtT) ;
 }
 
 else if ((Rds > 0.0)  || (lambda != 1.0)){ 
	   Vdsat = (-b - sqrt(pow(b, 2.0) - 4.0*a*c)) / (2.0*a) ; 
 }
 else {
   avt_errmsg(MCC_ERRMSG, "008", AVT_ERROR);
   Vdsat = (-b - sqrt(pow(b, 2.0) - 4.0*a*c)) / (2.0*a) ; 
 }

 
 V1         = Vdsat - Vds - DELTA              ;

 /*------------------------------------------------------------------*/
 Vdseff     = Vdsat - 0.5*(V1 + sqrt(pow(V1, 2.0) + 4.0*DELTA*Vdsat)) ;
 /*------------------------------------------------------------------*/
 if ( Vdseff == 0.0 ) return 0.0;

 
 /*##########################*/
 /*  Calcul de Vascbe        */ 
 /*##########################*/
 
 Litl       = sqrt(MCC_EPSSI*TOX*XJ/MCC_EPSOX) ;

 
 /*----------------------------------------------------------*/
 Vascbe     = (Leff/PSCBE2) * exp((PSCBE1*Litl)/(Vds-Vdseff)) ;
 /*----------------------------------------------------------*/

 
 /*##########################*/
 /*  Calcul de Va            */ 
 /*##########################*/
 
 Thetarout  = PDIBLC1*(exp(-DROUT*Leff/(2.0*Lt0))+ 2.0* exp(-DROUT*Leff/Lt0)) + PDIBLC2 ;

 // Rajout de fabs pour les valeurs de Vadibl et Vaclm par rapport a la doc
 Vadibl     = fabs(( (Vgsteff + 2.0*VtT)/(Thetarout*(1.0+PDIBLCB*Vbs)) ) 
              * ( 1.0 - (Abulk*Vdsat)/(Abulk*Vdsat+Vgsteff+2.0*VtT) )) ;
 
 Vaclm      = fabs((Abulk*Esat*Leff + Vgsteff)*(Vds - Vdseff) / (PCLM*Abulk*Esat*Litl)) ;
 
 Vasat      = ( Esat*Leff + Vdsat + 2.0*Rfactor*Vgsteff
              *(1.0 - (Abulk*Vdsat)/(2.0*(Vgsteff + 2.0*VtT))) ) / ( (2.0/lambda)-1.0+Rfactor*Abulk ) ; 

 
 /*-----------------------------------------------------------------------------------------*/
 Va         = Vasat + (1.0+(PVAG*Vgsteff)/(Esat*Leff)) * pow((1.0/Vaclm + 1.0/Vadibl), -1.0) ;
 /*-----------------------------------------------------------------------------------------*/
 

 /*##########################*/
 /*  Calcul de Ids0          */ 
 /*##########################*/

 /*------------------------------------------------------------------------------------------------------------------------------------*/
 Ids0       = (Weff_v/Leff)*Cox*ueffT*Vgsteff* ( (1.0-Abulk*(Vdseff/(2.0* (Vgsteff + 2.0*VtT) )))*Vdseff ) / (1.0+(Vdseff)/(Esat*Leff)) ; 
 /*------------------------------------------------------------------------------------------------------------------------------------*/
 
 /*##########################*/
 /*  Calcul de Ids           */ 
 /*##########################*/

 /*-----------------  FORMULE GLOBALE du courant IDS  -----------------------------------------*/
 /*---------------------------------------------------------------------------------------------------*/
 Ids        = Ids0/(1.0+(Rds*Ids0/Vdseff)) * (1.0 + (Vds - Vdseff)/Va) * (1.0 + (Vds - Vdseff)/Vascbe) ;
 /*---------------------------------------------------------------------------------------------------*/

 //IdiodeDrain = mcc_calcIdiode_bsim3v3(ptmodel, AX,PX,Temp,Vbd) ; 
 //IdiodeSource= mcc_calcIdiode_bsim3v3(ptmodel, AX,PX,Temp,Vbs) ; 
 //Ids = Ids + fabs(IdiodeDrain) + fabs(IdiodeSource) ;
 
 if (Vds == 0.0)
	 Ids = 0.0 ;
 
 /* Si on calcule le courant d'un transistor P, on revoie la valeur opposee du courant calcule */
 
 if (ptmodel->TYPE == MCC_PMOS) 
	Ids = -Ids ;

 previous_model = ptmodel ;
 previous_L = L ;
 previous_W = W ;
 if (calcul_dimension)
 {
   freechain(previous_longkey);
   previous_longkey=dupchainlst(lotrsparam->longkey);
 }
 return Ids ;
 
}

/********************************************************************************************/
/* Fonction qui calcule la longueur efficace Leff d'un transistor                           */
/********************************************************************************************/
double mcc_calcLeff_bsim3v3(mcc_modellist *ptmodel, double L, double W)
{
    double LL, LW, LWL, LLN, LWN, XL, XW, LINT ;
    double Lprinted, Wprinted, dL, Leff ;

    LL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LL)      ;
    LW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LW)      ;
    LWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWL)     ;
    LLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLN)     ;
    LWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWN)     ;
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    LINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT)    ;

    Lprinted   = L + XL ;
    Wprinted   = W + XW ;
    dL         = LINT + LL/pow(Lprinted, LLN) + LW/pow(Wprinted, LWN) 
                 + LWL/(pow(Lprinted,LLN) * pow(Wprinted, LWN)) ;
    Leff       = Lprinted - 2.0*dL ;

    if ( Leff <= 0.0 ) {
      avt_errmsg(MCC_ERRMSG, "009", AVT_ERROR, ptmodel->NAME,mcc_ftol(L*1e09),mcc_ftol(W*1e09),mcc_ftol(L*1e09));
      Leff = L;
    }

    return Leff ;
}

/********************************************************************************************/
/* Fonction qui calcule la largeur efficace Weff d'un transistor                            */
/********************************************************************************************/
double mcc_calcWeff_bsim3v3(mcc_modellist *ptmodel, double L, double W)
{
    double WL, WW, WWL, WLN, WWN, XL, XW, WINT ;
    double Lprinted, Wprinted, dW, Weff ;
    
    WL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WL)      ;
    WW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WW)      ;
    WWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWL)     ;
    WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
    WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    WINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_WINT)    ;

    Lprinted   = L + XL ;
    Wprinted   = W + XW ;
    dW         = WINT + WL/pow(Lprinted, WLN) + WW/pow(Wprinted, WWN) 
                 + WWL/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;
    Weff       = Wprinted - 2.0*dW ;

    if ( Weff <= 0.0 ) {
      avt_errmsg(MCC_ERRMSG, "010", AVT_ERROR, ptmodel->NAME,mcc_ftol(L*1e09),mcc_ftol(W*1e09),mcc_ftol(W*1e09));
      Weff = W;
    }

    return Weff ;
}
   
/********************************************************************************************/
/* Fonction qui calcule la capacite de grille CGP d'un transistor                           */
/********************************************************************************************/
double mcc_calcCGP_bsim3v3 (mcc_modellist *ptmodel, double vgx,
                            double L, double W, double *ptQov) 
{
    double Qov = 0.0 ;
    double cgp = 0.0 ;
    double CGDO = 0.0 ;
    double CGDL = 0.0 ;
    double CAPMOD, CKAPPA ;
    double Vgov, binunit ;
    double CF, Leff, Weff ;
   
    if ( ptmodel->TYPE == MCC_PMOS )
      vgx = -vgx;

    binunit =mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ; 
    CAPMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_CAPMOD) ;
    //CGDO   = mcc_getprm(ptmodel, "CGDO")     ;
    //CGDL   = mcc_getprm(ptmodel, "CGDL")     ;
    CGDO   = mcc_getprm_quick(ptmodel, __MCC_GETPRM_CGDO)     ;
    CGDL   = mcc_getprm_quick(ptmodel, __MCC_GETPRM_CGDL)     ;
 
    /* dimensions effectives */
    Leff = mcc_calcLeff_bsim3v3(ptmodel, L, W) ; 
    Weff = mcc_calcWeff_bsim3v3(ptmodel, L, W) ; 

    /* Binning parameters */
    CF     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CF,MCC_NO_LOG)          ; 
    CKAPPA = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CKAPPA,MCC_NO_LOG)      ;
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDL) == MCC_SETVALUE)
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGDL,MCC_NO_LOG)      ;
    else if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD1) == MCC_SETVALUE)
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGD1,MCC_NO_LOG)      ;
    else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSL) == MCC_SETVALUE)
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGSL,MCC_NO_LOG)      ;
    else
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGS1,MCC_NO_LOG)      ;

    /* Formules de Berkeley et non ELDO */

    /***************************/
    /* CAPMOD = 0              */
    /***************************/
    if(MCC_ROUND (CAPMOD) == 0) {
        cgp = CF + CGDO ;
        Qov = cgp*vgx;
    }
    
    /***************************/
    /* CAPMOD = 1              */
    /***************************/
    else if(MCC_ROUND (CAPMOD) == 1)
        if(vgx < 0.0) { 
            Qov = (CF+CGDO)*vgx + (CKAPPA*CGDL/2.0)*(-1.0+sqrt(1.0-4.0*vgx/CKAPPA)) ;
            if ( vgx != 0.0 )
              cgp = fabs(Qov/vgx) ;
            else
              cgp = 0.0;
        }
        else {
            cgp = CF+CGDO+CKAPPA*CGDL ;
            Qov = cgp*vgx;
        }

    /***************************/
    /* CAPMOD = 2 ou 3         */
    /***************************/
    else {
        Vgov = 0.5*(vgx+0.02-sqrt(pow((vgx+0.02), 2.0)+4.0*0.02)) ;
 
        Qov = (CF+CGDO)*vgx + CGDL*(vgx-Vgov-(CKAPPA/2.0)*(-1.0+sqrt(1.0-4.0*Vgov/CKAPPA))) ;
        if ( vgx != 0.0 )
          cgp = fabs(Qov/vgx) ;
        else 
          cgp = 0.0;
    }
    if ( ptQov ) {
      if ( ptmodel->TYPE == MCC_PMOS )
        Qov = -Qov;
      *ptQov = Qov;
    }
    return cgp ;
}

/********************************************************************************************/
/* Fonction de calcul du courant de diode                                                   */
/********************************************************************************************/
double mcc_calcIdiode_bsim3v3(mcc_modellist *ptmodel,  double AX, 
                              double PX, double temp, double vbx)
{
    double Isatbx, Ibx ;
    double M, NJ, JS, JSW, T, Tnom ;
    double JsT, JswT, XTI ;
    double VtT, VtTnom, EgT, EgTnom ;
    double GMIN = 1e-12 ;

    return 0.0;
    /* Obtention des parametres du modele */
    JS   = mcc_getparam_quick(ptmodel, __MCC_QUICK_JS )                ;
    NJ   = mcc_getparam_quick(ptmodel, __MCC_QUICK_NJ )                ;
    JSW  = mcc_getparam_quick(ptmodel, __MCC_QUICK_JSW )               ;
    XTI  = mcc_getparam_quick(ptmodel, __MCC_QUICK_XTI )               ;
    Tnom = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM ) + MCC_KELVIN ;
    T    = temp + MCC_KELVIN ;
    M    = 1.0 ;

    /* temperature effect */
 	VtT = MCC_KB * T / MCC_Q ;
 	VtTnom = MCC_KB * Tnom / MCC_Q ;
   	EgTnom = 1.16-7.02e-4*pow(Tnom, 2.0)/(Tnom+1108.0) ;
   	EgT    = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ;
    
    JsT   = JS*exp( (EgTnom/VtTnom-EgT/VtT+XTI*log(T/Tnom))/NJ ) ;
    JswT  = JSW*exp( (EgTnom/VtTnom-EgT/VtT+XTI*log(T/Tnom))/NJ ) ;

    Isatbx = M*(JsT*AX+JswT*PX) ;
    
    if(Isatbx < 0.0)
        Ibx = GMIN*vbx ;
    else
        Ibx = Isatbx*(exp(vbx/(NJ*VtT))-1.0) + GMIN*vbx ;
    
    if(ptmodel->TYPE == MCC_PMOS)
        Ibx = -Ibx ;

    return Ibx ;
}

/******************************************************************************\
Function : mcc_calcQint_bsim3v3

 Fonction qui calcule les charges intrinseques  d'un transistor              
 En considerant que le transistor fonctionne en lin/sature                    
\******************************************************************************/
void mcc_calcQint_bsim3v3(mcc_modellist *ptmodel, double L, double W,
                          double temp, double vgs, double vbs, double vds,
                          double *ptQg, double *ptQs, double *ptQd,
                          double *ptQb, elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *previous_model = NULL ;
  static double previous_L=0.0 ;
  static double previous_W=0.0 ;
  static int UPDATEPHI = 0, calcul_dimension ;
  double Qg1,Qb1;
  double Qg,Qs,Qb,Qd ;
  static double Cox, TOX ;
  static double T, tnomk ;
  static double Leff, Weff ;
  static double CAPMOD, VFB, VFBCV, CLC, CLE ;
  static double Abulk, Abulk0, K1, K1ox, TOXM, PhiT, A0, XJ, Xdep, B0, B1, KETA ;
  static double Vth,Vth_bias, vbseff, vdsat_cv, VtT, EgT, niT, Vgsteff_cv, VOFFCV, NOFF ; 
  static double K2, PhisT, vbsc, NCH, VOFF ;
  static double NI = 1.45e10*1e6 ; 
  static double delta1 = 1.0e-3 ;
  static double Vgseff, n, K, NGATE, binunit ;
  /* Vgsteff -> n */
  static double NFACTOR, Cd, Lt, DVT1, DVT2, Theta0 ;
  static double CDSC, CDSCB, CDSCD, CIT ;
  /* Accumulation charge        */
  static double Qacc, vgbacc, V0 ;
  /* Substrate depletion charge */
  static double Qsub0, dQsub ;
  /* Inversion charge           */
  static double Qinv ;
  /* for capmod = 3 */
  static double Ldebye,ACDE;
  static double MOIN ;
  static double Xdc ;    /* DC charge thickness */
  static double phib ;   /* Unknown : maybe bulk potential */
  static double BCT ;    /* Body Charge thickness in inversion */
  static double Ccen ;   /* Serial capacitance added to Cox */
  static double Coxeff ;
  static double delta, V3, Vfbeff, V1, Vcveff, Vdsat ;
  static double La;     /* Longueur active pour Abulk */
  static double VTH0, Coxcv,DTOXCV, VERSION, XPART ;
  double VgstNvt;
  double c0,c1,c2,c3,c4,c5;
  int xpart_int;
  double tvfb ;
  static chain_list *previous_longkey=NULL;

  if ( !mcc_is_same_model (ptmodel,L,W,1.0,previous_model,previous_L,previous_W,1.0,lotrsparam,previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;

  if ( calcul_dimension )
    XPART = mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART);
  if ( XPART < 0.0) {
    mcc_affect_value (ptQb,0.0);
    mcc_affect_value (ptQg,0.0);
    mcc_affect_value (ptQd,0.0);
    mcc_affect_value (ptQs,0.0);
    return ;
  }
  
if ( calcul_dimension ) {
  /* dimensions effectives */
  Leff = mcc_calcLeff_bsim3v3(ptmodel, L, W) ; 
  Weff = mcc_calcWeff_bsim3v3(ptmodel, L, W) ; 

  /* Obtention des parametres du modele */
  VERSION = mcc_getparam_quick(ptmodel, __MCC_QUICK_VERSION)                 ;
  binunit= mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT)                  ;
  TOX    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_TOX,MCC_NO_LOG)      ;
  ACDE   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_ACDE,MCC_NO_LOG)     ;
  Cox    = MCC_EPSOX / TOX                                   ;
  CAPMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_CAPMOD)                   ;
  VFBCV  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VFBCV,MCC_NO_LOG)    ;
  CLC    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CLC,MCC_NO_LOG)      ;
  CLE    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CLE,MCC_NO_LOG)      ;
  TOXM   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_TOXM,MCC_NO_LOG)     ;
  A0     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_A0,MCC_NO_LOG)       ;
  B0     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_B0,MCC_NO_LOG)       ;
  B1     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_B1,MCC_NO_LOG)       ;
  KETA   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KETA,MCC_NO_LOG)     ;
  if ( mcc_getparamtype_quick(ptmodel,__MCC_QUICK_NCH) == MCC_SETVALUE )
    NCH   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NCH,MCC_NO_LOG); 
  else
    NCH   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NPEAK,MCC_NO_LOG); 
  if(NCH <= 1.0e20)
      NCH = NCH*1.0e6 ;
  K1     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG)       ;
  K2     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K2,MCC_NO_LOG)       ;
  XJ     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_XJ,MCC_NO_LOG)       ;
  VOFFCV = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VOFFCV,MCC_NO_LOG)   ;
  NOFF   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NOFF,MCC_NO_LOG)     ;
  NFACTOR= binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NFACTOR,MCC_NO_LOG)  ;
  DVT1   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT1,MCC_NO_LOG)     ;
  DVT2   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT2,MCC_NO_LOG)     ;
  CDSC   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSC,MCC_NO_LOG)     ;
  CDSCB  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSCB,MCC_NO_LOG)    ;
  CDSCD  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSCD,MCC_NO_LOG)    ;
  CIT    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CIT,MCC_NO_LOG)      ;
  NGATE  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NGATE,MCC_NO_LOG)    ;
  if(NGATE <= 1.0e23)
      NGATE = NGATE*1.0e6 ;
  VOFF   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VOFF,MCC_NO_LOG)     ;
  MOIN   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_MOIN,MCC_NO_LOG)     ;

  /* Potentiel de surface PhiT */
  tnomk   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) + MCC_KELVIN  ;
  T       = temp + MCC_KELVIN ;
  if (UPDATEPHI == 0){
  	VtT    = MCC_KB * tnomk / MCC_Q    ;
  	EgT    = 1.16-7.02e-4*pow(tnomk, 2.0)/(tnomk+1108.0) ;
  	niT    = NI * pow((tnomk/300.15), 1.5)*exp(21.5565981 - EgT/(2.0*VtT)) ;
  }
  else if (UPDATEPHI == 1){
  	VtT    = MCC_KB * T / MCC_Q    ;
  	EgT    = 1.16-7.02e-4*pow(T, 2.0)/(T+1108.0) ; 
  	niT    = NI * pow((T/300.15), 1.5)*exp(21.5565981 - EgT/(2.0*VtT)) ;
  }
  PhiT = 2.0*VtT*log(NCH/niT) ;
  K1ox = K1*(TOX/TOXM) ;
}
// VTH0 = mcc_getprm (ptmodel,"VTH0") + mcc_getprm (ptmodel,"DELVT0");
 VTH0 = mcc_getprm_quick (ptmodel,__MCC_GETPRM_VTH0) + mcc_getprm_quick (ptmodel,__MCC_GETPRM_DELVTO);
 VTH0 = mcc_update_vth0 ( lotrsparam , VTH0 );
 VTH0 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VTH0,VTH0,MCC_NO_LOG) ;

 // obtain biased vth
 Vth_bias = mcc_calcVTH_bsim3v3(ptmodel, L, W, temp, vbs,vds, lotrsparam,MCC_NO_LOG) ;
 
 if (ptmodel->TYPE == MCC_PMOS) {
   VTH0 = -VTH0;
   Vth_bias = - Vth_bias;
   vds = fabs(vds);
   vgs = fabs(vgs);
 }

  /* vbseff */
  
  if((K2 != 0.0) && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_K2) == MCC_SETVALUE)) { 
      vbsc = 0.9*(PhiT-pow(K1ox, 2.0) / (4.0*pow(K2,2.0))) ;
      vbseff = vbsc + 0.5*( vbs - vbsc - delta1 
              + sqrt(pow(vbs-vbsc-delta1, 2.0) - 4.0*delta1*vbsc ) ) ;
  }
  else 
      vbseff = vbs ; 
 
  if(vbseff <= 0.0)
    PhisT  = PhiT - vbseff ; 
  else 
    PhisT  = pow((PhiT*sqrt(PhiT))/(PhiT + 0.5 * vbseff), 2.0) ;

  Xdep = sqrt((2.0*MCC_EPSSI/(MCC_Q*NCH)))*sqrt(PhisT) ; 

  /* Abulk */
  Abulk0 = (1.0 + K1ox/(2.0*sqrt(PhisT)) * (A0*Leff/(Leff + 2.0*sqrt(XJ*Xdep)) 
           + B0/(Weff + B1))) * 1.0 / (1.0+(KETA*vbs)) ;
  Abulk  = Abulk0*(1.0+pow((CLC/Leff), CLE)) ;

  /**************************************************************************/
  /*                        CAPMOD = 0                                      */
  /**************************************************************************/
  if(MCC_ROUND (CAPMOD) == 0) {
      if(MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_VFBFLAG)) == 1)
         Vth = VTH0 + K1ox*sqrt(PhisT) - K1ox*sqrt(PhiT) ; 
      else
         Vth = VFBCV + PhiT + K1ox*sqrt(PhisT) ;
      
      Vgseff = vgs;
      if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_SPICE ) {
        if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NGATE) == MCC_SETVALUE) {
          K   = MCC_Q*MCC_EPSSI*NGATE/pow(Cox, 2.0) ;
          if (K != 0.0) {
            VFB = VTH0 - PhiT - K1 * sqrt(PhiT) ;
            Vgseff = mcc_calc_vgseff(vgs, VFB, PhiT, Cox, NGATE);
          }
        }
      }
      /* fin modif, pour la suite de capmod=0, j'ai remplacé vgs par vgseff */

      //===> subthreshold region
      if ( Vgseff < Vth ) {
        // SUBTRESHOLD
        c0 = K1ox*K1ox;
        Qb = - Cox*c0/2.0 * (-1.0+sqrt (1.0+(4.0*(Vgseff-VFBCV-vbs)/(c0))));
        Qg = -Qb ;
        Qd = 0.0 ;
        Qs = 0.0 ;
      }
      else if ( Vgseff >= Vth ) {
        // STRONG INVERSION
        vdsat_cv = (Vgseff-Vth)/Abulk ;
        xpart_int = MCC_ROUND ( XPART*10.0 );
        if ( xpart_int == 5 ) {          //--> XPART = 0.5 : 50/50
          if ( vds < vdsat_cv ) {
            // Linear 
            c1 = Vgseff-Vth-Abulk*vds/2.0;
            c2 = Vgseff - Vth;
            c3 = Abulk*vds;
            c4 = 1.0-Abulk;
            
            Qg = Cox*(Vgseff-VFBCV-PhiT-vds/2.0+c3*vds/(12.0*c1));
            Qb = Cox*(VFBCV-Vth+PhiT+c4*vds/2.0-c4*c3*vds/(12.0*c1));
            Qinv = -Cox*(c2-c3/2.0+c3*c3/(12.0*c1));
            Qd = Qs = 0.5*Qinv;
          }
          else {
            // Saturation
            Qg = Cox*(Vgseff - VFBCV - PhiT - vdsat_cv/3.0) ;
            Qb = Cox*(VFBCV+PhiT-Vth+vdsat_cv*(1.0-Abulk)/3.0);
            Qs = Qd = -Cox*(Vgseff-Vth)/3.0;
          }
        }
        else if (xpart_int < 5) {                         //--> XPART = 0 : 40/60
          if ( vds < vdsat_cv ) {
            // Linear 
            c1 = Vgseff-Vth-Abulk*vds/2.0;
            c2 = Vgseff - Vth;
            c3 = Abulk*vds;

            Qg = Cox*(Vgseff - VFBCV - PhiT - vds/2.0 
                 + (Abulk*vds*vds /(12.0*c1)));
            Qb = Cox * (VFBCV-Vth+PhiT+ (1.0-Abulk)*vds/2.0 -
                 (1.0-Abulk)*(Abulk*vds*vds)/ (12.0*c1));
            Qd = -Cox * (c2/2.0 - c3 /2.0 + c3
                * ( c2*c2/6.0 - c3*c2/8.0 + c3*c3/40.0 ) / ( c1*c1 ) );
            Qs = - (Qg+Qb+Qd);
          }
          else {
            // Saturation
            Qg = Cox*(Vgseff - VFBCV - PhiT - vdsat_cv/3.0) ;
            Qb = Cox*(VFBCV+PhiT-Vth + vdsat_cv*(1.0-Abulk)/3.0);
            Qd = -4.0*Cox * (Vgseff-Vth)/15.0;
            Qs = - (Qg+Qb+Qd);
          }
        }
        else {                      //--> XPART = 1 : 0/100
          if ( vds < vdsat_cv ) {
            // Linear 
            c1 = Vgseff-Vth-Abulk*vds/2.0;
            c2 = Vgseff - Vth;
            c3 = Abulk*vds;

            Qg = Cox*(Vgseff-VFBCV-PhiT-vds/2.0+c3*vds/(12.0*c1));
            Qb = Cox*(VFBCV-Vth+PhiT+vds*(1.0-Abulk)/2.0 - 
                 (1.0-Abulk)*c3*vds/ (12.0*c1));
            /* documentation bsim3v3 fausse : qd non continue en vdsat_cv 
            Qd = -Cox* (c2/2.0+c3/4.0-c3*c3/(24.0*c1));
            correction d'après source de ngspice */
            Qd = Cox*( 0.75*c3 - 0.5*c2 - 0.125*c3*c3/(c2-0.5*c3) );
            Qs = - (Qg+Qb+Qd);
          }
          else {
            // Saturation
            Qg = Cox*(Vgseff - VFBCV - PhiT - vdsat_cv/3.0) ;
            Qb = Cox*(VFBCV+PhiT-Vth + vdsat_cv*(1.0-Abulk)/3.0);
            Qd = 0.0;
            Qs = - (Qg+Qb);
          }
        }
      }
  }
  else {
      /*******************************************************/
      /* Common parameter Vgsteff_cv for capmod = 1, 2, 3    */
      /* Expression de Vgseff en fonction du parametre NGATE */
      /*******************************************************/
      xpart_int = MCC_ROUND ( XPART*10.0 );
      if(mcc_getparam_quick(ptmodel, __MCC_QUICK_VERSION) < 3.2)
          Vth = Vth_bias;
      else {
          Vth = mcc_calcVTH_bsim3v3(ptmodel, L, W, temp, 0.0, 0.0,lotrsparam,MCC_NO_LOG) ;
          if ( ptmodel->TYPE == MCC_PMOS )
            Vth = -Vth;
      }
      // correction fabrice au bug sony avec une techno toshiba
      // anciennement: VFB = Vth - PhiT - K1ox * sqrt(PhisT) ;
      if( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_SPICE )
        VFB = Vth - PhiT - K1 * sqrt(PhisT) ;
      else
        VFB = VTH0 - PhiT - K1 * sqrt(PhiT) ;
      
      if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NGATE) == MCC_SETVALUE) {
          K   = MCC_Q*MCC_EPSSI*NGATE/pow(Cox, 2.0) ;
          if (K != 0.0) {
            tvfb = VFB ;
            if( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_SPICE ) {
              if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VFB) == MCC_SETVALUE)
                tvfb = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG);
              else
                tvfb = VTH0 - PhiT - K1 * sqrt(PhisT) ;
            }
            Vgseff = mcc_calc_vgseff(vgs, tvfb, PhiT, Cox, NGATE);
          }
          else
            Vgseff = vgs;
      }
      else
          Vgseff = vgs ;
      Lt = sqrt(MCC_EPSSI*Xdep/Cox)*(1.0 + DVT2 * vbs) ;
      Cd = MCC_EPSSI / Xdep ; 
      Theta0 = exp(-(DVT1*Leff)/(2.0*Lt)) + 2.0 * exp(-(DVT1*Leff)/Lt) ;

      n = 1.0 + NFACTOR * Cd/Cox + Theta0*(CDSC + CDSCB*vbs + CDSCD*vds)/Cox + CIT/Cox ;
      
      // from ng src
      VgstNvt = (Vgseff-Vth_bias-VOFFCV)/(n*NOFF*VtT);
      if ( VgstNvt > MCC_EXP_THRESHOLD )
        Vgsteff_cv = Vgseff-Vth_bias - VOFFCV;
      else if  ( VgstNvt < -MCC_EXP_THRESHOLD )
        Vgsteff_cv = NOFF*VtT * log(1.0 + MCC_MIN_EXP);
      else
        Vgsteff_cv = n*NOFF*VtT*log(1.0+exp(VgstNvt)) ; 
    
      /* Vfbeff */
      delta = 2.0e-2 ;
      V3 = VFB-(Vgseff-vbs)-delta ;
      if(VFB < 0.0) 
          Vfbeff = VFB-0.5*(V3+sqrt(pow(V3, 2.0)-4.0*delta*VFB)) ;
      else
          Vfbeff = VFB-0.5*(V3+sqrt(pow(V3, 2.0)+4.0*delta*VFB)) ;

      La = L + mcc_calcDLC_bsim3v3(ptmodel, L, W) ;
      /************************************************************************/
      /*                        CAPMOD = 1                                       */
      /************************************************************************/
      if(MCC_ROUND (CAPMOD) == 1) {
        c1 = K1ox*K1ox;
        if( V_INT_TAB[__SIM_TOOLMODEL].VALUE == SIM_TOOLMODEL_SPICE ) {
          if (Vgseff < VFB + vbs + Vgsteff_cv)
            Qg1 = -Cox*(Vgseff-VFB-vbs-Vgsteff_cv);
          else
            Qg1 = Cox*c1/2.0*(-1.0+sqrt (1.0+4.0*(Vgseff-VFB-Vgsteff_cv-vbseff)/c1));
        }
        else {
          if (vgs < VFB + vbs + Vgsteff_cv)
            Qg1 = -Cox*(vgs-VFB-vbs-Vgsteff_cv);
          else
            Qg1 = Cox*c1/2.0*(-1.0+sqrt (1.0+4.0*(vgs-VFB-Vgsteff_cv-vbseff)/c1));
        }
        Qb1 = -Qg1;

        vdsat_cv = Vgsteff_cv/Abulk ;
        V1 = Vgsteff_cv-vbs-VFB ;

        if (vds < vdsat_cv) {
          c1 = Vgsteff_cv-vds*Abulk/2.0;
          c2 = Abulk*vds;
          c3 = 1.0-Abulk;

          Qg = Qg1 + Cox*(Vgsteff_cv-vds/2.0+c2*vds/(12.0*c1));
          Qb = Qb1 + Cox*(c3*vds/2.0-c3*c2*vds/(12.0*c1));
          if ( xpart_int == 5 ) {
            Qs = Qd = -Cox/2.0*(Vgsteff_cv-c2/2.0+c2*c2*vds/(12.0*c1));
            Qs = -0.5*(Qg+Qb);
            Qd = -(Qg+Qb+Qs);
          }
          else if (xpart_int < 5) {                         //--> XPART = 0 : 40/60
            Qs = -Cox/(2.0*c1*c1)*(pow(Vgsteff_cv,3.0)-4.0*Vgsteff_cv*Vgsteff_cv*c2/3.0+
                  2.0*Vgsteff_cv*c2*c2/3.0-2.0*pow(c2,3.0)/15.0);
            Qd = -(Qg+Qb+Qs);
          }
          else {                      //--> XPART = 1 : 0/100
            Qs = -Cox*(Vgsteff_cv/2.0+c2/4.0-c2*c2/(24.0*c1));
            Qd = -(Qg+Qb+Qs);
          }
        }
        else {
          c1 = Vgsteff_cv-vdsat_cv/3.0;

          Qg = Qg1+Cox*c1;
          Qb = Qb1-Cox*(Vgsteff_cv-vdsat_cv)/3.0;
          if ( xpart_int == 5 )           //--> XPART = 0.5 : 50/50
            Qs = Qd = -Cox*Vgsteff_cv/3.0;
          else if ( xpart_int < 5 ) {                         //--> XPART = 0 : 40/60
            Qs = -2.0*Cox*Vgsteff_cv/5.0;
            Qd = -(Qg+Qb+Qs);
          }
          else {                      //--> XPART = 1 : 0/100
            Qs = -Cox*2.0/3.0*Vgsteff_cv;
            Qd = -(Qg+Qb+Qs);
          }
        }
      }
          
      /************************************************************************/
      /*                        CAPMOD = 2                                       */
      /************************************************************************/
      else if(MCC_ROUND (CAPMOD) == 2) {
          Abulk  = Abulk0*(1.0+pow((CLC/La), CLE)) ;

          Qacc = -Cox*(Vfbeff-VFB) ;
          Qsub0 = -Cox*pow(K1ox, 2.0)/2.0*( -1.0+sqrt(1.0
                   +4.0*(Vgseff-Vfbeff-vbseff-Vgsteff_cv)/pow(K1ox, 2.0)) ) ;
          
          vdsat_cv = Vgsteff_cv/Abulk ;
          V1 = vdsat_cv - vds - delta ;
          Vcveff = vdsat_cv -0.5*(V1+sqrt(V1*V1+4.0*delta*vdsat_cv)) ;

          Qinv = -Cox*( Vgsteff_cv-Abulk*Vcveff/2.0+pow((Abulk*Vcveff), 2.0)
                  /(12.0*(Vgsteff_cv-Abulk*Vcveff/2.0)) ) ;

          dQsub = Cox*(1.0-Abulk)*(Vcveff/2.0-Abulk*pow(Vcveff, 2.0)
                  /(12.0*(Vgsteff_cv-Abulk*Vcveff/2.0))) ;

          Qb = Qacc+Qsub0+dQsub;
          Qg = -(Qinv+Qb) ;

          c1 = Abulk*Vcveff;
          c2 = 2.0*pow((Vgsteff_cv-Abulk*Vcveff/2.0),2.0);
          c3 = pow(c1,2.0);
          c4 = pow(c1,3.0);

          if ( xpart_int == 5 )           //--> XPART = 0.5 : 50/50
            Qs = Qd = 0.5*Qinv;
          else if ( xpart_int < 5 ) {                         //--> XPART = 0 : 40/60
            double z0 = -Cox/c2;
            double z1 = pow(Vgsteff_cv,3.0);
            double z2 = Vgsteff_cv*Vgsteff_cv*c1;
            double z3 = Vgsteff_cv*c3;

            Qs = z0*(z1 - 4.0/3.0*z2 + 2.0/3.0*z3 - 2.0/15.0*c4); 
            Qd = z0*(z1 - 5.0/3.0*z2 + z3 - 1.0/5.0*c4);  
          }
          else {                      //--> XPART = 1 : 0/100
            c2 = (Vgsteff_cv-Abulk*Vcveff/2.0);

            Qs = -Cox*(Vgsteff_cv/2.0+c1/4.0-c3/(24.0*c2));
            Qd = -Cox*(Vgsteff_cv/2.0-3.0*c1/4.0+c3/(8.0*c2));
          }
      }
      /************************************************************************/
      /*                        CAPMOD = 3 : Charge Thickness Model              */
      /************************************************************************/
      else if(MCC_ROUND (CAPMOD) == 3) {
          double X0,dx,Xmax;
          double xdc_acc_dep = 0.0;
          double Coxeff_acc_dep;
                 
          DTOXCV = mcc_getparam_quick(ptmodel,__MCC_QUICK_DTOXCV);
          Coxcv = MCC_EPSOX/(TOX+DTOXCV);
          La = L + mcc_calcDLC_bsim3v3(ptmodel, L, W) ;
          Abulk  = Abulk0*(1.0+pow((CLC/La), CLE)) ;

          phib = PhiT ;
          
          // Xdc for depletion and accumulation region
          Ldebye = (1.0/3.0)*sqrt (MCC_EPSSI * mcc_calc_vt (mcc_getparam_quick(ptmodel,__MCC_QUICK_TNOM))/(MCC_Q*NCH));
          Xdc = Ldebye*exp (ACDE*pow(NCH/2.0e22,-0.25)*(Vgseff-vbseff-Vfbeff)/(1.0e8*TOX));
          Xmax = Ldebye;
          dx=1.0e-3*TOX;
          X0 = Xmax-Xdc-dx;
          Xdc = Xmax - 0.5*(X0+ sqrt(X0*X0+4.0*dx*Xmax));
          xdc_acc_dep = Xmax - 0.5*(X0+ sqrt(X0*X0+4.0*dx*Xmax));

          Ccen = MCC_EPSSI/xdc_acc_dep ;
          Coxeff_acc_dep = Cox*Ccen/(Coxcv+Ccen) ;

          // Xdc form strong inversion region
          Xdc = 1.9e-9/(1.0+pow((Vgsteff_cv+4.0*(Vth-VFB-phib)/(2.0e8*TOX)) , 0.7)) ;

          Ccen = MCC_EPSSI/Xdc;
          Coxeff = Cox*Ccen/(Coxcv+Ccen) ;
          
          /* Body Charge Thickness in inversion charge */
          BCT = VtT*log( (Vgsteff_cv*(Vgsteff_cv+2.0*K1ox*sqrt(phib)))
                  /(MOIN*K1ox*K1ox*VtT)+1.0 ) ;

          Vdsat = (Vgsteff_cv-BCT)/Abulk ;
          V1 = Vdsat-vds-delta ;
          Vcveff = Vdsat -0.5*(V1+sqrt(pow(V1, 2.0)+4.0*delta*Vdsat)) ;
          
          V0 = VFB+vbseff-Vgseff-delta ;
          vgbacc = 0.5*(V0+sqrt(pow(V0, 2.0)+4.0*delta*VFB)) ;
          
          Qacc =-Coxeff_acc_dep*vgbacc ;

          Qsub0 = -Coxeff_acc_dep*pow(K1ox, 2.0)/2.0*( -1.0+sqrt(1.0
                   +4.0*(Vgseff-Vfbeff-vbseff-Vgsteff_cv)/pow(K1ox, 2.0)) ) ;
          
          dQsub = Coxeff*( (1.0-Abulk)*Vcveff/2.0-(1.0-Abulk)*Abulk*pow(Vcveff, 2.0)
                  /(12.0*(Vgsteff_cv-BCT-Abulk*Vcveff/2.0)) ) ;

          Qinv = -Coxeff*(Vgsteff_cv-BCT-0.5*Abulk*Vcveff
                 + ( pow((Abulk*Vcveff), 2.0)/(12.0*(Vgsteff_cv-BCT-Abulk*Vcveff/2.0)) )) ;
          
          Qb = dQsub + Qsub0 + Qacc ;
          Qg = -(Qinv+Qb) ;

          c1 = Vgsteff_cv - BCT;
          c2 = pow (c1,2.0);
          c3 = pow (c1,3.0);
          c4 = Abulk*Vcveff;
          c5 = c1 - c4/2.0;

          if ( xpart_int == 5 )           //--> XPART = 0.5 : 50/50
            Qs = Qd = 0.5*Qinv;
          else if ( xpart_int < 5 ) {                         //--> XPART = 0 : 40/60
            Qs = -Coxeff/(2.0*c5*c5)*(c3-4.0/3.0*c2*c4
                  +2.0/3.0*c1*c4*c4-2.0/15.0*pow(c4,3.0));
            Qd = -Coxeff/(2.0*c5*c5)*(c3-5.0/3.0*c2*c4
                  +c1*c4*c4-1.0/5.0*pow(c4,3.0));
          }
          else {                      //--> XPART = 1 : 0/100
            Qs = -Coxeff/2.0*(c1+0.5*c4-c4*c4/(12.0*c5));
            Qd = -Coxeff/2.0*(c1-1.5*c4+c4*c4/(4.0*c5));
          }
      }
  }

  mcc_affect_value (ptQb,Qb);
  mcc_affect_value (ptQg,Qg);
  mcc_affect_value (ptQd,Qd);
  mcc_affect_value (ptQs,Qs);
  previous_model = ptmodel ;
  previous_L = L ;
  previous_W = W ;
  if (calcul_dimension)
   {
     freechain(previous_longkey);
     previous_longkey=dupchainlst(lotrsparam->longkey);
   }
}

/******************************************************************************\
 Fonction qui calcule la capacite de grille/drain CGD d'un transistor              
\******************************************************************************/
double mcc_calcCGD_bsim3v3 (mcc_modellist *ptmodel, double L, double W, double temp, 
                            double vgs0, double vgs1, double vbs, double vds,elp_lotrs_param *lotrsparam)
{
  double Qinit=0.0,Qfinal=0.0,cgd=0.0;

  mcc_calcQint_bsim3v3(ptmodel, L, W,
                       temp, vgs0, vbs, vds,
                       NULL, NULL, &Qinit, NULL,lotrsparam);
  mcc_calcQint_bsim3v3(ptmodel, L, W,
                       temp, vgs1, vbs, vds,
                       NULL, NULL, &Qfinal, NULL,lotrsparam);
  cgd = fabs((Qfinal-Qinit)/(vgs1-vgs0));
  return cgd;
}

/******************************************************************************\
 Fonction qui calcule la capacite de grille/source CGSI d'un transistor              
\******************************************************************************/
double mcc_calcCGSI_bsim3v3 (mcc_modellist *ptmodel, double L, double W, double temp, 
                            double vgs, double vbs, double vds,elp_lotrs_param *lotrsparam)
{
  double Qinit=0.0,Qfinal=0.0,cgsi=0.0;

  mcc_calcQint_bsim3v3(ptmodel, L, W,
                       temp, 0.0, vbs, vds,
                       NULL, &Qinit, NULL, NULL,lotrsparam);
  mcc_calcQint_bsim3v3(ptmodel, L, W,
                       temp, vgs, vbs, vds,
                       NULL, &Qfinal, NULL, NULL,lotrsparam);
  cgsi = fabs((Qfinal-Qinit)/vgs);
  return cgsi;
}

/* Calculate effective Area / Perimeter if not defined */

void
mcc_calcPAfromgeomod_bsim3(lotrs_list *lotrs, mcc_modellist *model, elp_lotrs_param *lotrsparam, double *as, double *ad, double *ps, double *pd)
{
    double l, w, wmlt, weff, xw, hdif, scalm ;
    int    acm, calcacm, geo, status;

    l = ((double)lotrs->LENGTH) / ((double)SCALE_X) * 1.0e-6;
    w = ((double)lotrs->WIDTH)  / ((double)SCALE_X) * 1.0e-6;
    
    wmlt = mcc_getparam_quick(model,__MCC_QUICK_WMLT);
    xw = mcc_getparam_quick(model,__MCC_QUICK_XW);
    hdif = mcc_getparam_quick(model,__MCC_QUICK_HDIF);
    scalm = 1; /* Should be configurable */ 
    acm = MCC_ROUND(mcc_getparam_quick(model,__MCC_QUICK_ACM));
    calcacm = MCC_ROUND(mcc_getparam_quick(model,__MCC_QUICK_CALCACM));
    
    weff = w*wmlt + xw*scalm;

    switch (acm) {
        case 0:
            if (as) *as = 0.0;
            if (ad) *ad = 0.0;
            if (ps) *ps = 0.0;
            if (pd) *pd = 0.0;
            break;
        case 1:
            if (as) *as = weff*wmlt;
            if (ad) *ad = weff*wmlt;
            if (ps) *ps = weff;
            if (pd) *pd = weff;
            break;
        case 2:
            if (as) *as = 2*hdif*scalm*weff;
            if (ad) *ad = 2*hdif*scalm*weff;
            if (ps) *ps = 4*hdif*scalm + 2*weff;
            if (pd) *pd = 4*hdif*scalm + 2*weff;
            break;
        case 3:
            geo = MCC_ROUND(getlotrsparam (lotrs, "geo", NULL, &status));
            if(status != 1) geo = 0;
            /* If GEO != 0 we may have problems elsewhere */
            switch (geo) {
                case 0:
                    if (as) *as = 2*hdif*scalm*weff;
                    if (ad) *ad = 2*hdif*scalm*weff;
                    if (ps) *ps = 4*hdif*scalm + weff;
                    if (pd) *pd = 4*hdif*scalm + weff;
                    break;
                case 1:
                    if (as) *as = 2*hdif*scalm*weff;
                    if (ad) *ad = hdif*scalm*weff;
                    if (ps) *ps = 2*hdif*scalm;
                    if (pd) *pd = 4*hdif*scalm + weff;
                    break;
                case 2:
                    if (as) *as = hdif*scalm*weff;
                    if (ad) *ad = 2*hdif*scalm*weff;
                    if (ps) *ps = 4*hdif*scalm + weff;
                    if (pd) *pd = 2*hdif*scalm;
                    break;
                case 3:
                    if (as) *as = hdif*scalm*weff;
                    if (ad) *ad = hdif*scalm*weff;
                    if (ps) *ps = 2*hdif*scalm;
                    if (pd) *pd = 2*hdif*scalm;
                    break;
            }
            break;
        case 12:
            if (calcacm == 1) {
                if (as) *as = 2*hdif*scalm*weff;
                if (ad) *ad = 2*hdif*scalm*weff;
                if (ps) *ps = 4*hdif*scalm + 2*weff;
                if (pd) *pd = 4*hdif*scalm + 2*weff;
            }
            else {
                if (as) *as = 0.0;
                if (ad) *ad = 0.0;
                if (ps) *ps = 0.0;
                if (pd) *pd = 0.0;
            }
            break;
        default:    
            if (as) *as = 0.0;
            if (ad) *ad = 0.0;
            if (ps) *ps = 0.0;
            if (pd) *pd = 0.0;
            break;
    }
}


