/**************************************************************************/
/*                                                                        */
/*                      Chaine de CAO & VLSI   AVERTEC                    */
/*                                                                        */
/*    Fichier : mcc_mod_mos2.c                                            */
/*                                                                        */
/*                                                                        */
/*    (c) copyright 2001 AVERTEC                                          */
/*    Tous droits reserves                                                */
/*                                                                        */
/*    Auteur(s) : Marc KUOCH                                              */
/*------------------------------------------------------------------------*/
/*                                                                        */
/*                                                                        */
/*------------------------------------------------------------------------*/

#include MCC_H
#include "mcc_mod_util.h"
#include "mcc_mod_mos2.h"

/**************************************************************************/
/* Premiere initialisation des parametres                                 */
/**************************************************************************/
void mcc_initparam_mos2(mcc_modellist *ptmodel) 
{
    mcc_initparam_com(ptmodel) ;

    /* CURRENT PARAMETERS                    */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LEVEL, 1.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_VTH0, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_KP, 6.8e-6, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMMA, 0.528, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PHI, 0.596, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LAMBDA, 0.0, NULL, MCC_INITVALUE)      ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_UO, 200.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_VMAX, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_THETA, 0.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_UCRIT, 1.0e4, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_UEXP, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_NEFF, 1.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_ETA, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_KAPPA, 0.2, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DELTA, 0.0, NULL, MCC_INITVALUE)       ;
    /* MOBILITY PARAMETERS                   */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LMOB, 1.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LVSAT, 1.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LCLM, 1.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_K1, 0.0, NULL, MCC_INITVALUE)          ;
    /* TEMPERATURE PARAMETERS                */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TCV, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_BEX, -1.5, NULL, MCC_INITVALUE)        ;
    /* PROCESS PARAMETERS                    */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DELL, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DELW, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_NSUB, 1.0e15, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_NSS, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_NFS, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TPG, 1.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TOX, 1.0e-7, NULL, MCC_INITVALUE)      ;
    
    /* CAPACITANCE PARAMETERS                */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CGBO, 0.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_XQC, 1.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_COX, 3.45e-4, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LCAP, 1.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_METO, 0.0, NULL, MCC_INITVALUE)        ;
    /* RESISTANCE PARAMETERS                 */
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LDIF, 0.0, NULL, MCC_INITVALUE)        ;

    /***********************************************************/
    /* initialisation des param en fonction des param existant */
    /***********************************************************/
    if((mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEV) == 0.0) || (mcc_getparam_quick(ptmodel, __MCC_QUICK_TLEV) == 1.0))
        mcc_initparam_quick(ptmodel, __MCC_QUICK_EG, 1.11, NULL, MCC_INITVALUE) ;
    else
        mcc_initparam_quick(ptmodel, __MCC_QUICK_EG, 1.16, NULL, MCC_INITVALUE) ;
}

/**************************************************************************/
/* CALCUL de VTH                                                          */
/**************************************************************************/
double  mcc_calcVTH_mos2(mcc_modellist *ptmodel)  
{
    double VTO, VTH ;
    
    VTO = mcc_getprm(ptmodel, "VTH0")     ;
    
    VTH  = VTO ;

    return(VTH) ;
}

