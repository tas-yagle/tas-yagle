/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_mm9.c                                                 */
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
#include "mcc_mod_util.h"
#include "mcc_mod_mm9.h"

/*****************************************************************/
/* Fonction D initialisation                                     */
/*****************************************************************/
double mcc_hyp1(double x, double eps)
{
    return(0.5*(x+sqrt(pow(x, 2.0)+4.0*pow(eps, 2.0)))) ;
}

double mcc_hyp2(double x, double x0, double eps)
{
    return(x-mcc_hyp1(x-x0, eps)) ;
}

double mcc_hyp3(double x, double x0, double eps)
{
    return(mcc_hyp2(x, x0, eps)-mcc_hyp2(0, x0, eps)) ;
}

double mcc_hyp4(double x, double x0, double eps)
{
    return(mcc_hyp1(x-x0, eps)-mcc_hyp1(-x0, eps)) ;
}   

double mcc_hyp5(double x, double x0, double eps)
{
    return(x0-mcc_hyp1(x0-x-pow(eps, 2.0)/x0, eps)) ;
}


/*****************************************************************/
/* FUNCTION mcc_initparam_mm9                                    */
/*****************************************************************/
void mcc_initparam_mm9(mcc_modellist *ptmodel) 
{
    mcc_initparam_com(ptmodel) ;

	mcc_initparam_quick(ptmodel, __MCC_QUICK_AF, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TOX, 2.5e-8, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGBO, 0.0  , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, 0.0  , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, 0.0  , NULL, MCC_INITVALUE)      ;

    if (mcc_getparam_quick(ptmodel,__MCC_QUICK_INIT) == 1)
    /* completer les initialisations pour INIT = 1               */
        mcc_initparam1_mm9(ptmodel) ;
    else
    /* les initialisations ki suivent sont valides pour INIT = 0 */
        mcc_initparam2_mm9(ptmodel) ;
}

/*****************************************************************/
/* FUNCTION mcc_initparam1_mm9                                   */
/*                                                               */
/* Used when parameter "INIT" = 1                                */
/*                                                               */
/*****************************************************************/
void mcc_initparam1_mm9(mcc_modellist *ptmodel) 
{
	mcc_initparam_quick(ptmodel, __MCC_QUICK_COL, 0.0   , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LER, 0.0    , NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WER, 0.0    , NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VTOR, 1.0 , NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STVTO, 0.0    , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVTO, 0.0    , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SL2VTO, 0.0   , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWVTO, 0.0    , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KOR, 1.0    , NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLKO, 0.0    , NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWKO, 0.0    , NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KR, 1.0    , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLK, 0.0   , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWK, 0.0   , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PHIBR, 0.6 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VSBXR, 0.6 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVSBX, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWVSBX, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BETSQ, 1.e-4, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETABET, 0.0 , NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WDOG, 0.0 ,   NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_FTE1, 0.0 ,   NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_THE1R,0.0 ,   NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE1R,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE1R,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE1,0.0 ,   NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_THE2R,0.0 ,   NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE2R,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE2R,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STLTHE2,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE2,0.0 ,   NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_THE3R,0.0 ,   NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE3R,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE3R,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STLTHE3,0.0 ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE3,0.0  ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_GAM1R, 0.0  ,   NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLGAM1, 0.0  ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWGAM1, 0.0  ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETADSR, 1.0  ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPR, 0.0    ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAALP, 0.0  ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLALP, 0.0   ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWALP, 0.0   ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VPR, 1.0e-12 ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMOOR, 0.0  ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLGAMOO, 0.0 ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAGAMR, 0.0 ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MOR, 1.0e-12 ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STMO, 0.0    ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLMO, 0.0    ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAMR, 0.0   ,   NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ZET1R, 1.0e-12,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAZET, 0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLZET1, 0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VSBTR,  0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVSBT, 0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A1R,    0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STA1,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA1,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA1,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A2R, 1.0e-12  ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA2,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA2,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A3R,    0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA3,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA3,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NTR,    0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFR,    0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFMOD,  0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFAR,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFBR,   0.0   ,  NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFCR,   0.0   ,  NULL, MCC_INITVALUE)  ;
}

/*****************************************************************/
/* FUNCTION mcc_initparam2_mm9                                   */
/*****************************************************************/
void mcc_initparam2_mm9(mcc_modellist *ptmodel) 
{
	mcc_initparam_quick(ptmodel, __MCC_QUICK_INIT, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_COL, 3.2e-4, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WER, 20.0e-6, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SL2VTO, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PHIBR, 0.65, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVSBX, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETABET, 1.6, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WDOG, 1.6  , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_FTE1, 0.0  , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_THE1R, 0.19, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE1R, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STLTHE1, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE2R, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STLTHE2, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETADSR,  0.6, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STMO,    0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STA1,    0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFMOD,   0.0, NULL, MCC_INITVALUE)     ;

    /*--- Initialisation en fonction du type du tranistor   */

    if (ptmodel->TYPE == MCC_NMOS) {
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_LER, 1.1e-6, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VTOR, 0.73, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_STVTO, -1.2e-3, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVTO, -1.35e-7, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWVTO, 1.3e-7, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_KOR,  0.65 , NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLKO, -1.3e-7, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWKO,  2.0e-9, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_KR,  0.11 ,  NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLK, -2.8e-7, NULL, MCC_INITVALUE)  ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWK, 2.75e-7,  NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VSBXR, 0.66, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWVSBX, -6.75e-7, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_BETSQ, 8.3e-5, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE1R, 1.4e-7, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE1, -5.8e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_THE2R, -1.2e-2, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE2R, -3.3e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE2, 3.0e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_THE3R, 0.145 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE3R, -6.6e-4, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE3R, 1.85e-7, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_STLTHE3, -6.2e-10, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE3,  2.0e-8 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAM1R,  0.145  , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLGAM1, 1.6e-7 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWGAM1,-1.0e-8 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPR, 3.0e-3 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAALP, 0.15   , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLALP, -5.65e-3, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWALP, 1.67e-9, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VPR, 0.34   , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMOOR, 1.8e-2 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLGAMOO,2.0e-14, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAGAMR, 2.0   , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_MOR, 0.5   , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLMO, 2.8e-4, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAMR, 2.0, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ZET1R,0.42, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAZET, 0.17  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLZET1, -0.39  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VSBTR,  2.1   , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVSBT,-4.4e-6 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_A1R,   6.0  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA1, 1.3e-6, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA1, 3.0e-6, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_A2R, 38.0  , NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA2, 1.0e-6, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA2, 2.0e-6, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_A3R, 0.65  , NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA3,-5.5e-7, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA3, 0.0   , NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NTR, 2.44e-20, NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFR,  7.0e-11, NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFAR, 7.15e22 , NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFBR, 2.16e7  , NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFCR,  0.0    , NULL, MCC_INITVALUE)    ;
    }
    else {
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_LER, 1.25e-6, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VTOR,  1.1, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_STVTO, -1.7e-3, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVTO, 3.5e-8, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWVTO, 5.0e-8, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_KOR,  0.47 , NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLKO, -2.0e-7, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWKO, 1.15e-7, NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_KR,  0.47 ,  NULL, MCC_INITVALUE)   ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLK, -2.0e-7, NULL, MCC_INITVALUE)  ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWK, 1.15e-7, NULL, MCC_INITVALUE)  ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VSBXR, 1.0e-12, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWVSBX,  0.0  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_BETSQ,2.61e-5, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE1R, 7.0e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE1, -8.0e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_THE2R,  0.165 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE2R, -7.5e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE2, 2.0e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_THE3R, 2.7e-2, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_STTHE3R, 0.0  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLTHE3R, 2.7e-8, NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_STLTHE3, 0.0, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWTHE3,  1.1e-8 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAM1R,  7.7e-2 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLGAM1,1.05e-7 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWGAM1,-1.1e-8 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPR, 4.4e-2 , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAALP, 0.17   , NULL, MCC_INITVALUE);
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLALP,  9.0e-3, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWALP, 1.8e-10, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VPR, 0.235  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMOOR, 7.0e-3 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLGAMOO,1.1e-14, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAGAMR, 1.0   , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_MOR, 0.375 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLMO, 4.7e-5, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAMR, 1.0, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ZET1R, 1.3, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAZET, 3.0e-2, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLZET1, -2.8   , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_VSBTR,  1.0e2 , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLVSBT, 0.0    , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_A1R,  10.0  , NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA1,-1.5e-5, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA1, 3.0e-5, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_A2R, 59.0  , NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA2,-8.0e-6, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA2, 1.5e-5, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_A3R, 0.52  , NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SLA3,-4.5e-7, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_SWA3,-1.4e-7, NULL, MCC_INITVALUE)     ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NTR, 2.11e-20, NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFR, 2.14e-11, NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFAR, 1.53e22 , NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFBR, 4.06e6  , NULL, MCC_INITVALUE)    ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFCR, 2.92e-10, NULL, MCC_INITVALUE)    ;
    }

/*---------------------------------------------------------------*/
/* Deuxieme initialisation                                       */
/*---------------------------------------------------------------*/
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, 0.0, NULL, MCC_INITVALUE)        ;
}

/*****************************************************************/
/* Calcul de VTH                                                 */
/* VTH = VT1 + dVT1                                              */
/*****************************************************************/
double mcc_calcVTH_mm9(mcc_modellist *ptmodel, double L, double W, 
                       double temp, double vbs, double vds, double vgs)
{
    double VTH ;
    double kelvin  =  273.15           ;
    double Leff, Weff, LVAR, WVAR, LAP, WOT ;
    double Vt0_til, VTOR, tnom, dT, STVTO ;
    double Vt0, LER, WER, SLVTO, SL2VTO, SWVTO ; 
    double k, KR, SLK, SWK ;
    double k0, KOR, SLKO, SWKO ;
    double Vsbx, VSBXR, SLVSBX, SWVSBX ;
    double vsb, h1, phib, PHIBR, vs, vs0, vst, vsx, Vt1 ;
    double Vsbt, VSBTR, SLVBST ;
    double dVt0, dVt1, vs1, gam0, gam00, ETAGAMR ;
    double GAMOOR, SLGAMOO, gam1, GAM1R, SLGAM1, SWGAM1, Vgt1 ;
    double ETADSR ;

    
    /*****************************************/
    /* Obtention des parametres du model     */
    /*****************************************/

    /* Pour les dim eff */
    LVAR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LVAR)    ;
    WVAR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WVAR)    ;
    LAP    = mcc_getparam_quick(ptmodel, __MCC_QUICK_LAP)     ;
    WOT    = mcc_getparam_quick(ptmodel, __MCC_QUICK_WOT)     ;

    /* Pour Vt0_til */
    PHIBR  = mcc_getparam_quick(ptmodel, __MCC_QUICK_PHIBR)   ;

    /* Pour Vt0_til */
    VTOR   = mcc_getparam_quick(ptmodel, __MCC_QUICK_VTOR)    ;
    STVTO  = mcc_getparam_quick(ptmodel, __MCC_QUICK_STVTO)   ;
    
    /* Pour Vt0 */
    LER    = mcc_getparam_quick(ptmodel, __MCC_QUICK_LER)     ;
    WER    = mcc_getparam_quick(ptmodel, __MCC_QUICK_WER)     ;
    SLVTO  = mcc_getparam_quick(ptmodel, __MCC_QUICK_SLVTO)   ;
    SL2VTO = mcc_getparam_quick(ptmodel, __MCC_QUICK_SL2VTO)  ;
    SWVTO  = mcc_getparam_quick(ptmodel, __MCC_QUICK_SWVTO)   ;

    /* Pour dVt0 : k, k0, Vsbx */
    KR     = mcc_getparam_quick(ptmodel, __MCC_QUICK_KR)      ;
    KOR    = mcc_getparam_quick(ptmodel, __MCC_QUICK_KOR)     ;
    SLK    = mcc_getparam_quick(ptmodel, __MCC_QUICK_SLK)     ;
    SLKO   = mcc_getparam_quick(ptmodel, __MCC_QUICK_SLKO)    ;
    SWK    = mcc_getparam_quick(ptmodel, __MCC_QUICK_SWK)     ;
    SWKO   = mcc_getparam_quick(ptmodel, __MCC_QUICK_SWKO)    ;
    VSBXR  = mcc_getparam_quick(ptmodel, __MCC_QUICK_VSBXR)   ;
    SLVSBX = mcc_getparam_quick(ptmodel, __MCC_QUICK_SLVSBX)  ;
    SWVSBX = mcc_getparam_quick(ptmodel, __MCC_QUICK_SWVSBX)  ;

    /* Pour Vsbt */
    VSBTR  = mcc_getparam_quick(ptmodel, __MCC_QUICK_VSBTR)   ;
    SLVBST = mcc_getparam_quick(ptmodel, __MCC_QUICK_SLVBST)  ;

    /* Pour gam00, gam0, gam1 */
    GAMOOR = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAMOOR)  ;
    SLGAMOO= mcc_getparam_quick(ptmodel, __MCC_QUICK_SLGAMOO) ;
    ETAGAMR= mcc_getparam_quick(ptmodel, __MCC_QUICK_ETAGAMR) ;
    GAM1R  = mcc_getparam_quick(ptmodel, __MCC_QUICK_GAM1R)   ;
    SLGAM1 = mcc_getparam_quick(ptmodel, __MCC_QUICK_SLGAM1)  ;
    SWGAM1 = mcc_getparam_quick(ptmodel, __MCC_QUICK_SWGAM1)  ;

    /* Pour dVt1 */
    ETADSR = mcc_getparam_quick(ptmodel, __MCC_QUICK_ETADSR)  ;

    /*** Fin d'obtention des parametres    ***/
    
    /*------  Just return the main param ----*/
    return VTOR ;                /* test     */
    /*---------------------------------------*/

    vsb = -vbs ;      

    /* Temperature */
    tnom = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) + kelvin ;
    temp = temp + kelvin ;
    dT   = temp - tnom ;
    
    /* Calcul de phib */
    phib = PHIBR+dT*((PHIBR-1.13-2.5e-4*tnom)/300.0) ;

    /* Calcul des dimensions effectives */
    Leff = L + LVAR - 2.0*LAP ;
    Weff = W + WVAR - 2.0*WOT ;
    
    /* Calcul de Vt0_til */
    Vt0_til = VTOR + dT*STVTO ;
    
    /* Calcul de Vt0     */
    Vt0 = Vt0_til + (1.0/Leff - 1/LER)*SLVTO+(1.0/pow(Leff, 2.0)-1.0/pow(LER, 2.0))
          *SL2VTO + (1.0/Weff-1.0/WER)*SWVTO ;

    /* Calcul de k, k0, Vsbx */
    k    = KR + (1.0/Leff-1.0/LER)*SLK+(1.0/Weff-1.0/WER)*SWK ;
    k0   = KOR + (1.0/Leff-1.0/LER)*SLKO+(1.0/Weff-1.0/WER)*SWKO ;
    Vsbx = VSBXR+(1.0/Leff-1.0/LER)*SLVSBX+(1/Weff-1.0/WER)*SWVSBX ;
    
    /* Calculation of drain-feedback parameters */
    gam1 = GAM1R + (1.0/Leff-1.0/LER)*SLGAM1+(1.0/Weff-1.0/WER)*SWGAM1 ;

    /* Subthreshold parameters */
    Vsbt = VSBTR + (1.0/Leff-1.0/LER)*SLVBST ;
    
    /* Extended equations */
    h1   = mcc_hyp1(vsb+0.5*phib, MCC_EPS1) + 0.5*phib ;
    vs   = sqrt(h1) ;
    vs0  = sqrt(phib) ;
    vst  = sqrt(Vsbt+phib) ;
    vsx  = sqrt(Vsbx+phib) ;

    dVt0 = k*(sqrt(mcc_hyp4(vsb, Vsbx, MCC_EPS2)+pow((k/k0), 2.0)*pow(vsx, 2.0))
            -(k/k0)*vsx) + k0*(sqrt(h1-mcc_hyp4(vsb, Vsbx, MCC_EPS2))-vs0) ;

    Vt1  = Vt0 + dVt0 ;

    vs1  = mcc_hyp2(vs, vst, MCC_EPS3) ;
    gam00 = GAMOOR+(1.0/pow(Leff, 2.0)-1.0/pow(LER, 2.0))*SLGAMOO ;
    gam0 = gam00*pow((vs1/vs0), ETAGAMR) ;
    Vgt1 = mcc_hyp1(vgs-Vt1, MCC_EPS4) ;

    dVt1 = (-gam0-(gam1*pow((vds+MCC_LAMBDA2), (ETADSR-1.0))-gam0)*pow(Vgt1, 2.0)
            /(0.5+pow(Vgt1, 2.0)))*pow(vds, 2.0)/(vds+MCC_LAMBDA1) ;

    if(ptmodel->TYPE == MCC_NMOS)
        VTH = Vt1 + dVt1 ;
    else
        VTH = -(Vt1 + dVt1) ;

    return(VTH) ;
}
