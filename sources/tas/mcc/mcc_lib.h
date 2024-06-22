/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc100.h                                                    */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#ifndef MCC
#define MCC

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include MUT_H
#include ELP_H
#include EQT_H
#include SIM_H
#include STM_H
#include MCC_H

/****************************************************************************\
 DEFINES
\****************************************************************************/

/* mccLog   */
#define MCC_NO_LOG  (int)0
#define MCC_DRV_LOG (int)1

/* elpDrive */
#define MCC_DONOT_DRV_MODEL  -1
#define MCC_DRV_ONE_MODEL     0
#define MCC_DRV_ALL_MODEL     1

/* Flag pour fittage */
#define MCC_FIT_KO 0
#define MCC_FIT_OK 1

/* Les differents types de modele de transistor */

#define MCC_NOMODEL     -1      
#define MCC_GETMODEL    -2      
#define MCC_NOMODELTYPE MCC_MOS2
#define MCC_MOS2        0
#define MCC_BSIM3V3     1
#define MCC_MM9         2
#define MCC_OTHER       3
#define MCC_BSIM4       4
#define MCC_MPSP        5   /* because MCC_PSP exists... */
#define MCC_MPSPB       6   /* because MCC_PSP exists... */
#define MCC_EXTMOD         7

#define MCC_NB_MOD      8
#define MCC_VALID_MOD   7

#define MCC_NAME_MOS2        "MOS2"
#define MCC_NAME_BSIM3V3     "BSIM3V3"
#define MCC_NAME_MM9         "MM9"     
#define MCC_NAME_OTHER       "OTHER" 
#define MCC_NAME_BSIM4       "BSIM4"
#define MCC_NAME_MPSP        "PSP"
#define MCC_NAME_MPSPB       "PSPB"
#define MCC_NAME_CMI         "CMI"

#define MCC_TABMOD_NAME   {MCC_NAME_MOS2,   \
                           MCC_NAME_BSIM3V3,\
                           MCC_NAME_MM9,    \
                           MCC_NAME_OTHER,  \
                           MCC_NAME_BSIM4,  \
                           MCC_NAME_MPSP,   \
                           MCC_NAME_MPSPB   \
                           }

/* Les types des transistors */
#define MCC_NMOS        elpNMOS
#define MCC_PMOS        elpPMOS
#define MCC_UNKNOWN     2
#define MCC_DIODE       3
#define MCC_MODEL_RESI        4
#define MCC_MODEL_CAPA        5

/* Les cas des transistors */
#define MCC_BEST        elpBEST
#define MCC_WORST       elpWORST
#define MCC_TYPICAL     elpTYPICAL

#define MCC_NB_CASE     3

#define MCC_NAME_WORST       "WORST"
#define MCC_NAME_BEST        "BEST"
#define MCC_NAME_TYPICAL     "TYPICAL"

#define MCC_TABCASE_NAME  {MCC_NAME_WORST,\
                           MCC_NAME_BEST,\
                           MCC_NAME_TYPICAL}


/* Les longueurs et largeurs maximales des transistors */
#define MCC_LWMAX   ELPMAXLONG
#define MCC_D_LWMAX (double)(1.0e99)

#define MCC_TRANS_N 0
#define MCC_TRANS_P 1
#define MCC_TRANS_B 2
#define MCC_CURRENT_MIN 1.0e-18

#define MCC_FIT_A 0
#define MCC_FIT_CG 1
#define MCC_FIT_CG_NOCAPA 2
#define MCC_FIT_CDN 3
#define MCC_FIT_CDP 4
#define MCC_PARAM 5
#define MCC_FIT 6
#define MCC_OPT_PARAM 7
#define MCC_CALC_PARAM 8

#define MCC_NORMAL_MODE 0
#define MCC_EXPERT_MODE 1
#define MCC_FAST_MODE 2
#define MCC_NEXT_MODE 3
#define MCC_SIM_MODE 0
#define MCC_CALC_MODE 1

/* Parametres de la structure paramlist */
#define MCC_INITVALUE 0
#define MCC_SETVALUE  1

#define MCC_IS_CONSTANT 1

/* Constantes */
#define MCC_EPSSI      (double)(1.03594313991e-10)
#define MCC_EPSOX      (double)(3.45314379969e-11)
#define MCC_EPSO       (double)(8.85418e-12)
#define MCC_Q          (double)(1.60206e-19)      /* charge elementaire */
#define MCC_KELVIN     (double)(273.15)
#define MCC_KB         (double)(1.380658e-23)     /* constante de Boltzman */
#define MCC_PI         (double)(3.14159265359)    
#define MCC_MAX_EXP    (double)(5.834617425e14)   /* From ngspice rwk14 src*/
#define MCC_MIN_EXP    (double)(1.713908431e-15)
#define MCC_EXP_THRESHOLD  (double)(34.0)

/* champs user des lofigs  */
#define MCC_MODEL_EXPR    (long)(0x81000001)
#define MCC_DEFMODEL_EXPR (long)(0x81000002)

/* champs user des models  */
#define MCC_MODEL_LOTRS    (long)(0x81000003)
#define MCC_MODEL_EXTNAME  (long)(0x81000004)

/* transitions pour debug  */
#define MCC_DU 0
#define MCC_UD 1
#define MCC_DD 2
#define MCC_UU 3

#define MCC_LOTRS_SEED (long)(0x81000003)

enum mcc_quick_params
{
__MCC_QUICK_K1, __MCC_QUICK_LK1, __MCC_QUICK_WK1, __MCC_QUICK_PK1,
__MCC_QUICK_K2, __MCC_QUICK_LK2, __MCC_QUICK_WK2, __MCC_QUICK_PK2,
__MCC_QUICK_K3, __MCC_QUICK_LK3, __MCC_QUICK_WK3, __MCC_QUICK_PK3,
__MCC_QUICK_K3B, __MCC_QUICK_LK3B, __MCC_QUICK_WK3B, __MCC_QUICK_PK3B,
__MCC_QUICK_W0, __MCC_QUICK_LW0, __MCC_QUICK_WW0, __MCC_QUICK_PW0,
__MCC_QUICK_NLX, __MCC_QUICK_LNLX, __MCC_QUICK_WNLX, __MCC_QUICK_PNLX,
__MCC_QUICK_DVT0, __MCC_QUICK_LDVT0, __MCC_QUICK_WDVT0, __MCC_QUICK_PDVT0,
__MCC_QUICK_DVT1, __MCC_QUICK_LDVT1, __MCC_QUICK_WDVT1, __MCC_QUICK_PDVT1,
__MCC_QUICK_DVT2, __MCC_QUICK_LDVT2, __MCC_QUICK_WDVT2, __MCC_QUICK_PDVT2,
__MCC_QUICK_DVT0W, __MCC_QUICK_LDVT0W, __MCC_QUICK_WDVT0W, __MCC_QUICK_PDVT0W,
__MCC_QUICK_DVT1W, __MCC_QUICK_LDVT1W, __MCC_QUICK_WDVT1W, __MCC_QUICK_PDVT1W,
__MCC_QUICK_DVT2W, __MCC_QUICK_LDVT2W, __MCC_QUICK_WDVT2W, __MCC_QUICK_PDVT2W,
__MCC_QUICK_DSUB, __MCC_QUICK_LDSUB, __MCC_QUICK_WDSUB, __MCC_QUICK_PDSUB,
__MCC_QUICK_NCH, __MCC_QUICK_LNCH, __MCC_QUICK_WNCH, __MCC_QUICK_PNCH,
__MCC_QUICK_NPEAK, __MCC_QUICK_LNPEAK, __MCC_QUICK_WNPEAK, __MCC_QUICK_PNPEAK,
__MCC_QUICK_ETA0, __MCC_QUICK_LETA0, __MCC_QUICK_WETA0, __MCC_QUICK_PETA0,
__MCC_QUICK_ETAB, __MCC_QUICK_LETAB, __MCC_QUICK_WETAB, __MCC_QUICK_PETAB,
__MCC_QUICK_KT1, __MCC_QUICK_LKT1, __MCC_QUICK_WKT1, __MCC_QUICK_PKT1,
__MCC_QUICK_KT2, __MCC_QUICK_LKT2, __MCC_QUICK_WKT2, __MCC_QUICK_PKT2,
__MCC_QUICK_KT1L, __MCC_QUICK_LKT1L, __MCC_QUICK_WKT1L, __MCC_QUICK_PKT1L,
__MCC_QUICK_VTH0, __MCC_QUICK_LVTH0, __MCC_QUICK_WVTH0, __MCC_QUICK_PVTH0,
__MCC_QUICK_VFB, __MCC_QUICK_LVFB, __MCC_QUICK_WVFB, __MCC_QUICK_PVFB,
__MCC_QUICK_DWG, __MCC_QUICK_LDWG, __MCC_QUICK_WDWG, __MCC_QUICK_PDWG,
__MCC_QUICK_DWB, __MCC_QUICK_LDWB, __MCC_QUICK_WDWB, __MCC_QUICK_PDWB,
__MCC_QUICK_NGATE, __MCC_QUICK_LNGATE, __MCC_QUICK_WNGATE, __MCC_QUICK_PNGATE,
__MCC_QUICK_RDSW, __MCC_QUICK_LRDSW, __MCC_QUICK_WRDSW, __MCC_QUICK_PRDSW,
__MCC_QUICK_PRT, __MCC_QUICK_LPRT, __MCC_QUICK_WPRT, __MCC_QUICK_PPRT,
__MCC_QUICK_PRWB, __MCC_QUICK_LPRWB, __MCC_QUICK_WPRWB, __MCC_QUICK_PPRWB,
__MCC_QUICK_PRWG, __MCC_QUICK_LPRWG, __MCC_QUICK_WPRWG, __MCC_QUICK_PPRWG,
__MCC_QUICK_WR, __MCC_QUICK_LWR, __MCC_QUICK_WWR, __MCC_QUICK_PWR,
__MCC_QUICK_NFACTOR, __MCC_QUICK_LNFACTOR, __MCC_QUICK_WNFACTOR, __MCC_QUICK_PNFACTOR,
__MCC_QUICK_CDSC, __MCC_QUICK_LCDSC, __MCC_QUICK_WCDSC, __MCC_QUICK_PCDSC,
__MCC_QUICK_CDSCB, __MCC_QUICK_LCDSCB, __MCC_QUICK_WCDSCB, __MCC_QUICK_PCDSCB,
__MCC_QUICK_CDSCD, __MCC_QUICK_LCDSCD, __MCC_QUICK_WCDSCD, __MCC_QUICK_PCDSCD,
__MCC_QUICK_CIT, __MCC_QUICK_LCIT, __MCC_QUICK_WCIT, __MCC_QUICK_PCIT,
__MCC_QUICK_VOFF, __MCC_QUICK_LVOFF, __MCC_QUICK_WVOFF, __MCC_QUICK_PVOFF,
__MCC_QUICK_A1, __MCC_QUICK_LA1, __MCC_QUICK_WA1, __MCC_QUICK_PA1,
__MCC_QUICK_A2, __MCC_QUICK_LA2, __MCC_QUICK_WA2, __MCC_QUICK_PA2,
__MCC_QUICK_VSAT, __MCC_QUICK_LVSAT, __MCC_QUICK_WVSAT, __MCC_QUICK_PVSAT,
__MCC_QUICK_AT, __MCC_QUICK_LAT, __MCC_QUICK_WAT, __MCC_QUICK_PAT,
__MCC_QUICK_UA, __MCC_QUICK_LUA, __MCC_QUICK_WUA, __MCC_QUICK_PUA,
__MCC_QUICK_UB, __MCC_QUICK_LUB, __MCC_QUICK_WUB, __MCC_QUICK_PUB,
__MCC_QUICK_UC, __MCC_QUICK_LUC, __MCC_QUICK_WUC, __MCC_QUICK_PUC,
__MCC_QUICK_UA1, __MCC_QUICK_LUA1, __MCC_QUICK_WUA1, __MCC_QUICK_PUA1,
__MCC_QUICK_UB1, __MCC_QUICK_LUB1, __MCC_QUICK_WUB1, __MCC_QUICK_PUB1,
__MCC_QUICK_UC1, __MCC_QUICK_LUC1, __MCC_QUICK_WUC1, __MCC_QUICK_PUC1,
__MCC_QUICK_UTE, __MCC_QUICK_LUTE, __MCC_QUICK_WUTE, __MCC_QUICK_PUTE,
__MCC_QUICK_A0, __MCC_QUICK_LA0, __MCC_QUICK_WA0, __MCC_QUICK_PA0,
__MCC_QUICK_B0, __MCC_QUICK_LB0, __MCC_QUICK_WB0, __MCC_QUICK_PB0,
__MCC_QUICK_B1, __MCC_QUICK_LB1, __MCC_QUICK_WB1, __MCC_QUICK_PB1,
__MCC_QUICK_XJ, __MCC_QUICK_LXJ, __MCC_QUICK_WXJ, __MCC_QUICK_PXJ,
__MCC_QUICK_KETA, __MCC_QUICK_LKETA, __MCC_QUICK_WKETA, __MCC_QUICK_PKETA,
__MCC_QUICK_AGS, __MCC_QUICK_LAGS, __MCC_QUICK_WAGS, __MCC_QUICK_PAGS,
__MCC_QUICK_DELTA, __MCC_QUICK_LDELTA, __MCC_QUICK_WDELTA, __MCC_QUICK_PDELTA,
__MCC_QUICK_PSCBE1, __MCC_QUICK_LPSCBE1, __MCC_QUICK_WPSCBE1, __MCC_QUICK_PPSCBE1,
__MCC_QUICK_PSCBE2, __MCC_QUICK_LPSCBE2, __MCC_QUICK_WPSCBE2, __MCC_QUICK_PPSCBE2,
__MCC_QUICK_PDIBLC1, __MCC_QUICK_LPDIBLC1, __MCC_QUICK_WPDIBLC1, __MCC_QUICK_PPDIBLC1,
__MCC_QUICK_PDIBLC2, __MCC_QUICK_LPDIBLC2, __MCC_QUICK_WPDIBLC2, __MCC_QUICK_PPDIBLC2,
__MCC_QUICK_DROUT, __MCC_QUICK_LDROUT, __MCC_QUICK_WDROUT, __MCC_QUICK_PDROUT,
__MCC_QUICK_PDIBLCB, __MCC_QUICK_LPDIBLCB, __MCC_QUICK_WPDIBLCB, __MCC_QUICK_PPDIBLCB,
__MCC_QUICK_PCLM, __MCC_QUICK_LPCLM, __MCC_QUICK_WPCLM, __MCC_QUICK_PPCLM,
__MCC_QUICK_PVAG, __MCC_QUICK_LPVAG, __MCC_QUICK_WPVAG, __MCC_QUICK_PPVAG,
__MCC_QUICK_U0, __MCC_QUICK_LU0, __MCC_QUICK_WU0, __MCC_QUICK_PU0,
__MCC_QUICK_CF, __MCC_QUICK_LCF, __MCC_QUICK_WCF, __MCC_QUICK_PCF,
__MCC_QUICK_CKAPPA, __MCC_QUICK_LCKAPPA, __MCC_QUICK_WCKAPPA, __MCC_QUICK_PCKAPPA,
__MCC_QUICK_CGDL, __MCC_QUICK_LCGDL, __MCC_QUICK_WCGDL, __MCC_QUICK_PCGDL,
__MCC_QUICK_CGD1, __MCC_QUICK_LCGD1, __MCC_QUICK_WCGD1, __MCC_QUICK_PCGD1,
__MCC_QUICK_CGSL, __MCC_QUICK_LCGSL, __MCC_QUICK_WCGSL, __MCC_QUICK_PCGSL,
__MCC_QUICK_CGS1, __MCC_QUICK_LCGS1, __MCC_QUICK_WCGS1, __MCC_QUICK_PCGS1,
__MCC_QUICK_TOX, __MCC_QUICK_LTOX, __MCC_QUICK_WTOX, __MCC_QUICK_PTOX,
__MCC_QUICK_ACDE, __MCC_QUICK_LACDE, __MCC_QUICK_WACDE, __MCC_QUICK_PACDE,
__MCC_QUICK_VFBCV, __MCC_QUICK_LVFBCV, __MCC_QUICK_WVFBCV, __MCC_QUICK_PVFBCV,
__MCC_QUICK_CLC, __MCC_QUICK_LCLC, __MCC_QUICK_WCLC, __MCC_QUICK_PCLC,
__MCC_QUICK_CLE, __MCC_QUICK_LCLE, __MCC_QUICK_WCLE, __MCC_QUICK_PCLE,
__MCC_QUICK_TOXM, __MCC_QUICK_LTOXM, __MCC_QUICK_WTOXM, __MCC_QUICK_PTOXM,
__MCC_QUICK_VOFFCV, __MCC_QUICK_LVOFFCV, __MCC_QUICK_WVOFFCV, __MCC_QUICK_PVOFFCV,
__MCC_QUICK_NOFF, __MCC_QUICK_LNOFF, __MCC_QUICK_WNOFF, __MCC_QUICK_PNOFF,
__MCC_QUICK_MOIN, __MCC_QUICK_LMOIN, __MCC_QUICK_WMOIN, __MCC_QUICK_PMOIN,
__MCC_QUICK_NSD, __MCC_QUICK_LNSD, __MCC_QUICK_WNSD, __MCC_QUICK_PNSD,
__MCC_QUICK_NDEP, __MCC_QUICK_LNDEP, __MCC_QUICK_WNDEP, __MCC_QUICK_PNDEP,
__MCC_QUICK_VBM, __MCC_QUICK_LVBM, __MCC_QUICK_WVBM, __MCC_QUICK_PVBM,
__MCC_QUICK_MINV, __MCC_QUICK_LMINV, __MCC_QUICK_WMINV, __MCC_QUICK_PMINV,
__MCC_QUICK_TVOFF, __MCC_QUICK_LTVOFF, __MCC_QUICK_WTVOFF, __MCC_QUICK_PTVOFF,
__MCC_QUICK_LPEB, __MCC_QUICK_LLPEB, __MCC_QUICK_WLPEB, __MCC_QUICK_PLPEB,
__MCC_QUICK_EU, __MCC_QUICK_LEU, __MCC_QUICK_WEU, __MCC_QUICK_PEU,
__MCC_QUICK_UD, __MCC_QUICK_LUD, __MCC_QUICK_WUD, __MCC_QUICK_PUD,
__MCC_QUICK_UD1, __MCC_QUICK_LUD1, __MCC_QUICK_WUD1, __MCC_QUICK_PUD1,
__MCC_QUICK_UP, __MCC_QUICK_LUP, __MCC_QUICK_WUP, __MCC_QUICK_PUP,
__MCC_QUICK_LP, __MCC_QUICK_LLP, __MCC_QUICK_WLP, __MCC_QUICK_PLP,
__MCC_QUICK_PHIN, __MCC_QUICK_LPHIN, __MCC_QUICK_WPHIN, __MCC_QUICK_PPHIN,
__MCC_QUICK_XT, __MCC_QUICK_LXT, __MCC_QUICK_WXT, __MCC_QUICK_PXT,
__MCC_QUICK_LPE0, __MCC_QUICK_LLPE0, __MCC_QUICK_WLPE0, __MCC_QUICK_PLPE0,
__MCC_QUICK_DVTP0, __MCC_QUICK_LDVTP0, __MCC_QUICK_WDVTP0, __MCC_QUICK_PDVTP0,
__MCC_QUICK_DVTP1, __MCC_QUICK_LDVTP1, __MCC_QUICK_WDVTP1, __MCC_QUICK_PDVTP1,
__MCC_QUICK_FPROUT, __MCC_QUICK_LFPROUT, __MCC_QUICK_WFPROUT, __MCC_QUICK_PFPROUT,
__MCC_QUICK_PDITS, __MCC_QUICK_LPDITS, __MCC_QUICK_WPDITS, __MCC_QUICK_PPDITS,
__MCC_QUICK_PDITSL, __MCC_QUICK_LPDITSL, __MCC_QUICK_WPDITSL, __MCC_QUICK_PPDITSL,
__MCC_QUICK_PDITSD, __MCC_QUICK_LPDITSD, __MCC_QUICK_WPDITSD, __MCC_QUICK_PPDITSD,
__MCC_QUICK_VTL, __MCC_QUICK_LVTL, __MCC_QUICK_WVTL, __MCC_QUICK_PVTL,
__MCC_QUICK_XN, __MCC_QUICK_LXN, __MCC_QUICK_WXN, __MCC_QUICK_PXN,
__MCC_QUICK_LC, __MCC_QUICK_LLC, __MCC_QUICK_WLC, __MCC_QUICK_PLC,
__MCC_QUICK_CKAPPAD, __MCC_QUICK_LCKAPPAD, __MCC_QUICK_WCKAPPAD, __MCC_QUICK_PCKAPPAD,
__MCC_QUICK_NTOX, __MCC_QUICK_LNTOX, __MCC_QUICK_WNTOX, __MCC_QUICK_PNTOX,
__MCC_QUICK_AIGBACC, __MCC_QUICK_LAIGBACC, __MCC_QUICK_WAIGBACC, __MCC_QUICK_PAIGBACC,
__MCC_QUICK_BIGBACC, __MCC_QUICK_LBIGBACC, __MCC_QUICK_WBIGBACC, __MCC_QUICK_PBIGBACC,
__MCC_QUICK_CIGBACC, __MCC_QUICK_LCIGBACC, __MCC_QUICK_WCIGBACC, __MCC_QUICK_PCIGBACC,
__MCC_QUICK_NIGBACC, __MCC_QUICK_LNIGBACC, __MCC_QUICK_WNIGBACC, __MCC_QUICK_PNIGBACC,
__MCC_QUICK_AIGBINV, __MCC_QUICK_LAIGBINV, __MCC_QUICK_WAIGBINV, __MCC_QUICK_PAIGBINV,
__MCC_QUICK_BIGBINV, __MCC_QUICK_LBIGBINV, __MCC_QUICK_WBIGBINV, __MCC_QUICK_PBIGBINV,
__MCC_QUICK_CIGBINV, __MCC_QUICK_LCIGBINV, __MCC_QUICK_WCIGBINV, __MCC_QUICK_PCIGBINV,
__MCC_QUICK_NIGBINV, __MCC_QUICK_LNIGBINV, __MCC_QUICK_WNIGBINV, __MCC_QUICK_PNIGBINV,
__MCC_QUICK_EIGBINV, __MCC_QUICK_LEIGBINV, __MCC_QUICK_WEIGBINV, __MCC_QUICK_PEIGBINV,
__MCC_QUICK_AGIDL, __MCC_QUICK_LAGIDL, __MCC_QUICK_WAGIDL, __MCC_QUICK_PAGIDL,
__MCC_QUICK_BGIDL, __MCC_QUICK_LBGIDL, __MCC_QUICK_WBGIDL, __MCC_QUICK_PBGIDL,
__MCC_QUICK_CGIDL, __MCC_QUICK_LCGIDL, __MCC_QUICK_WCGIDL, __MCC_QUICK_PCGIDL,
__MCC_QUICK_EGIDL, __MCC_QUICK_LEGIDL, __MCC_QUICK_WEGIDL, __MCC_QUICK_PEGIDL,
__MCC_QUICK_AGISL, __MCC_QUICK_LAGISL, __MCC_QUICK_WAGISL, __MCC_QUICK_PAGISL,
__MCC_QUICK_BGISL, __MCC_QUICK_LBGISL, __MCC_QUICK_WBGISL, __MCC_QUICK_PBGISL,
__MCC_QUICK_CGISL, __MCC_QUICK_LCGISL, __MCC_QUICK_WCGISL, __MCC_QUICK_PCGISL,
__MCC_QUICK_EGISL, __MCC_QUICK_LEGISL, __MCC_QUICK_WEGISL, __MCC_QUICK_PEGISL,
__MCC_QUICK_JSD, __MCC_QUICK_LJSD, __MCC_QUICK_WJSD, __MCC_QUICK_PJSD,
__MCC_QUICK_JSSWD, __MCC_QUICK_LJSSWD, __MCC_QUICK_WJSSWD, __MCC_QUICK_PJSSWD,
__MCC_QUICK_JSSWGD, __MCC_QUICK_LJSSWGD, __MCC_QUICK_WJSSWGD, __MCC_QUICK_PJSSWGD,
__MCC_QUICK_BVD, __MCC_QUICK_LBVD, __MCC_QUICK_WBVD, __MCC_QUICK_PBVD,
__MCC_QUICK_NJD, __MCC_QUICK_LNJD, __MCC_QUICK_WNJD, __MCC_QUICK_PNJD,
__MCC_QUICK_XJBVD, __MCC_QUICK_LXJBVD, __MCC_QUICK_WXJBVD, __MCC_QUICK_PXJBVD,
__MCC_QUICK_XTID, __MCC_QUICK_LXTID, __MCC_QUICK_WXTID, __MCC_QUICK_PXTID,
__MCC_QUICK_JSS, __MCC_QUICK_LJSS, __MCC_QUICK_WJSS, __MCC_QUICK_PJSS,
__MCC_QUICK_JSSWS, __MCC_QUICK_LJSSWS, __MCC_QUICK_WJSSWS, __MCC_QUICK_PJSSWS,
__MCC_QUICK_JSSWGS, __MCC_QUICK_LJSSWGS, __MCC_QUICK_WJSSWGS, __MCC_QUICK_PJSSWGS,
__MCC_QUICK_BVS, __MCC_QUICK_LBVS, __MCC_QUICK_WBVS, __MCC_QUICK_PBVS,
__MCC_QUICK_NJS, __MCC_QUICK_LNJS, __MCC_QUICK_WNJS, __MCC_QUICK_PNJS,
__MCC_QUICK_XJBVS, __MCC_QUICK_LXJBVS, __MCC_QUICK_WXJBVS, __MCC_QUICK_PXJBVS,
__MCC_QUICK_XTIS, __MCC_QUICK_LXTIS, __MCC_QUICK_WXTIS, __MCC_QUICK_PXTIS,
__MCC_QUICK_POXEDGE, __MCC_QUICK_LPOXEDGE, __MCC_QUICK_WPOXEDGE, __MCC_QUICK_PPOXEDGE,
__MCC_QUICK_DLCIGD, __MCC_QUICK_LDLCIGD, __MCC_QUICK_WDLCIGD, __MCC_QUICK_PDLCIGD,
__MCC_QUICK_AIGSD, __MCC_QUICK_LAIGSD, __MCC_QUICK_WAIGSD, __MCC_QUICK_PAIGSD,
__MCC_QUICK_BIGSD, __MCC_QUICK_LBIGSD, __MCC_QUICK_WBIGSD, __MCC_QUICK_PBIGSD,
__MCC_QUICK_CIGSD, __MCC_QUICK_LCIGSD, __MCC_QUICK_WCIGSD, __MCC_QUICK_PCIGSD,
__MCC_QUICK_DLCIG, __MCC_QUICK_LDLCIG, __MCC_QUICK_WDLCIG, __MCC_QUICK_PDLCIG,
__MCC_QUICK_AIGS, __MCC_QUICK_LAIGS, __MCC_QUICK_WAIGS, __MCC_QUICK_PAIGS,
__MCC_QUICK_BIGS, __MCC_QUICK_LBIGS, __MCC_QUICK_WBIGS, __MCC_QUICK_PBIGS,
__MCC_QUICK_CIGS, __MCC_QUICK_LCIGS, __MCC_QUICK_WCIGS, __MCC_QUICK_PCIGS,
__MCC_QUICK_VFBSDOFF, __MCC_QUICK_LVFBSDOFF, __MCC_QUICK_WVFBSDOFF, __MCC_QUICK_PVFBSDOFF,
__MCC_QUICK_TVFBSDOFF, __MCC_QUICK_LTVFBSDOFF, __MCC_QUICK_WTVFBSDOFF, __MCC_QUICK_PTVFBSDOFF,
__MCC_QUICK_AIGC, __MCC_QUICK_LAIGC, __MCC_QUICK_WAIGC, __MCC_QUICK_PAIGC,
__MCC_QUICK_BIGC, __MCC_QUICK_LBIGC, __MCC_QUICK_WBIGC, __MCC_QUICK_PBIGC,
__MCC_QUICK_CIGC, __MCC_QUICK_LCIGC, __MCC_QUICK_WCIGC, __MCC_QUICK_PCIGC,
__MCC_QUICK_NIGC, __MCC_QUICK_LNIGC, __MCC_QUICK_WNIGC, __MCC_QUICK_PNIGC,
__MCC_QUICK_PIGCD, __MCC_QUICK_LPIGCD, __MCC_QUICK_WPIGCD, __MCC_QUICK_PPIGCD,
__MCC_QUICK_KVTH0WE, __MCC_QUICK_LKVTH0WE, __MCC_QUICK_WKVTH0WE, __MCC_QUICK_PKVTH0WE,
__MCC_QUICK_K2WE, __MCC_QUICK_LK2WE, __MCC_QUICK_WK2WE, __MCC_QUICK_PK2WE,
__MCC_QUICK_KU0WE, __MCC_QUICK_LKU0WE, __MCC_QUICK_WKU0WE, __MCC_QUICK_PKU0WE,
__MCC_QUICK_PJSM,
__MCC_QUICK_PJSW,
__MCC_QUICK_ZET1R,
__MCC_QUICK_XRCRG2,
__MCC_QUICK_XRCRG1,
__MCC_QUICK_XQC,
__MCC_QUICK_XGW,
__MCC_QUICK_XGL,
__MCC_QUICK_WVFCBV,
__MCC_QUICK_WDOG,
__MCC_QUICK_W,
__MCC_QUICK_VPR,
__MCC_QUICK_VNDS,
__MCC_QUICK_VMAX,
__MCC_QUICK_VFCBV,
__MCC_QUICK_VBX,
__MCC_QUICK_VB,
__MCC_QUICK_UO,
__MCC_QUICK_UEXP,
__MCC_QUICK_UCRIT,
__MCC_QUICK_TVJ,
__MCC_QUICK_TTT2,
__MCC_QUICK_TTT1,
__MCC_QUICK_TT,
__MCC_QUICK_TRSH2,
__MCC_QUICK_TRSH1,
__MCC_QUICK_TRS2,
__MCC_QUICK_TRS1,
__MCC_QUICK_TRS,
__MCC_QUICK_TRNQSMOD,
__MCC_QUICK_TREF,
__MCC_QUICK_TRD2,
__MCC_QUICK_TRD1,
__MCC_QUICK_TR,
__MCC_QUICK_TPG,
__MCC_QUICK_TNOIMOD,
__MCC_QUICK_TNOIB,
__MCC_QUICK_TNOIA,
__MCC_QUICK_TM2,
__MCC_QUICK_TM1,
__MCC_QUICK_TLEVR,
__MCC_QUICK_TLEVI,
__MCC_QUICK_TIKF,
__MCC_QUICK_THETA,
__MCC_QUICK_THE3R,
__MCC_QUICK_THE2R,
__MCC_QUICK_THE1R,
__MCC_QUICK_TCV,
__MCC_QUICK_TBV2,
__MCC_QUICK_TBV1,
__MCC_QUICK_SWTHE3,
__MCC_QUICK_SWTHE2,
__MCC_QUICK_SWTHE1,
__MCC_QUICK_SWALP,
__MCC_QUICK_SWA3,
__MCC_QUICK_SWA2,
__MCC_QUICK_SWA1,
__MCC_QUICK_STTHE3R,
__MCC_QUICK_STTHE2R,
__MCC_QUICK_STTHE1R,
__MCC_QUICK_STMO,
__MCC_QUICK_STLTHE3,
__MCC_QUICK_STLTHE2,
__MCC_QUICK_STLTHE1,
__MCC_QUICK_STA1,
__MCC_QUICK_SLZET1,
__MCC_QUICK_SLVSBT,
__MCC_QUICK_SLTHE3R,
__MCC_QUICK_SLTHE2R,
__MCC_QUICK_SLTHE1R,
__MCC_QUICK_SLMO,
__MCC_QUICK_SLALP,
__MCC_QUICK_SLA3,
__MCC_QUICK_SLA2,
__MCC_QUICK_SLA1,
__MCC_QUICK_SCALEV,
__MCC_QUICK_SC,
__MCC_QUICK_SBTH,
__MCC_QUICK_RSHG,
__MCC_QUICK_RNOIB,
__MCC_QUICK_RNOIA,
__MCC_QUICK_RGEOMOD,
__MCC_QUICK_RGATEMOD,
__MCC_QUICK_RDWMIN,
__MCC_QUICK_RDW,
__MCC_QUICK_RDSMIN,
__MCC_QUICK_RDS,
__MCC_QUICK_RBSB,
__MCC_QUICK_RBPS,
__MCC_QUICK_RBPD,
__MCC_QUICK_RBPB,
__MCC_QUICK_RBODYMOD,
__MCC_QUICK_RBDB,
__MCC_QUICK_RB,
__MCC_QUICK_PVFCBV,
__MCC_QUICK_PT,
__MCC_QUICK_PHI,
__MCC_QUICK_PERIM,
__MCC_QUICK_PDITLS,
__MCC_QUICK_NTR,
__MCC_QUICK_NTNOI,
__MCC_QUICK_NSTAR,
__MCC_QUICK_NSS,
__MCC_QUICK_NSJ,
__MCC_QUICK_NS,
__MCC_QUICK_NRS,
__MCC_QUICK_NRD,
__MCC_QUICK_NR,
__MCC_QUICK_NQSMOD,
__MCC_QUICK_NOIC,
__MCC_QUICK_NOIB,
__MCC_QUICK_NOIA,
__MCC_QUICK_NGJ,
__MCC_QUICK_NGCON,
__MCC_QUICK_NG,
__MCC_QUICK_NFS,
__MCC_QUICK_NFR,
__MCC_QUICK_NFMOD,
__MCC_QUICK_NFCR,
__MCC_QUICK_NFBR,
__MCC_QUICK_NFAR,
__MCC_QUICK_NEFF,
__MCC_QUICK_NDS,
__MCC_QUICK_NBVL,
__MCC_QUICK_NBV,
__MCC_QUICK_NBJ,
__MCC_QUICK_NB,
__MCC_QUICK_N,
__MCC_QUICK_MOR,
__MCC_QUICK_METO,
__MCC_QUICK_LVFCBV,
__MCC_QUICK_LS,
__MCC_QUICK_LMOB,
__MCC_QUICK_LIS,
__MCC_QUICK_LG,
__MCC_QUICK_LCLM,
__MCC_QUICK_LCAP,
__MCC_QUICK_L,
__MCC_QUICK_KP,
__MCC_QUICK_KF,
__MCC_QUICK_KAPPA,
__MCC_QUICK_JSGSR,
__MCC_QUICK_JSGGR,
__MCC_QUICK_JSGBR,
__MCC_QUICK_JSDSR,
__MCC_QUICK_JSDGR,
__MCC_QUICK_JSDBR,
__MCC_QUICK_JF,
__MCC_QUICK_ISW,
__MCC_QUICK_ISTMP,
__MCC_QUICK_ISTEMP,
__MCC_QUICK_ISR,
__MCC_QUICK_ISP,
__MCC_QUICK_ISA,
__MCC_QUICK_IS,
__MCC_QUICK_IKR,
__MCC_QUICK_IK,
__MCC_QUICK_IGBMOD,
__MCC_QUICK_IBVL,
__MCC_QUICK_IBV,
__MCC_QUICK_GBMIN,
__MCC_QUICK_GAMMA,
__MCC_QUICK_FTE1,
__MCC_QUICK_FNOIMOD,
__MCC_QUICK_FLKFLAG,
__MCC_QUICK_EXP,
__MCC_QUICK_EXA,
__MCC_QUICK_ETAZET,
__MCC_QUICK_ETAMR,
__MCC_QUICK_ETABET,
__MCC_QUICK_ETAALP,
__MCC_QUICK_ETA,
__MCC_QUICK_EM,
__MCC_QUICK_ELM,
__MCC_QUICK_EF,
__MCC_QUICK_DTOX,
__MCC_QUICK_DTA,
__MCC_QUICK_DMCGT,
__MCC_QUICK_DIOMOD,
__MCC_QUICK_DCAPLEV,
__MCC_QUICK_COX,
__MCC_QUICK_CJP,
__MCC_QUICK_CJA,
__MCC_QUICK_CJ0,
__MCC_QUICK_CGBO,
__MCC_QUICK_CBS,
__MCC_QUICK_CBD,
__MCC_QUICK_BEX,
__MCC_QUICK_BETSQ,
__MCC_QUICK_BETA0,
__MCC_QUICK_AREA,
__MCC_QUICK_ALPR,
__MCC_QUICK_ALPHA1,
__MCC_QUICK_ALPHA0,
__MCC_QUICK_ALEV,
__MCC_QUICK_AF,
__MCC_QUICK_ACNQSMOD,
__MCC_QUICK_AB,
__MCC_QUICK_A3R,
__MCC_QUICK_A2R,
__MCC_QUICK_A1R,
__MCC_QUICK_BINU,
__MCC_QUICK_DELTVTO,
__MCC_QUICK_DELTVT0,
__MCC_QUICK_COL,
__MCC_QUICK_CGS0,
__MCC_QUICK_CGD0,
__MCC_QUICK_WD,
__MCC_QUICK_VER,
__MCC_QUICK_DELVTO,
__MCC_QUICK_VT0,
__MCC_QUICK_VTO,
__MCC_QUICK_VTHO,
__MCC_QUICK_SWJUNCAP,
__MCC_QUICK_PTP,
__MCC_QUICK_PTA,
__MCC_QUICK_CTP,
__MCC_QUICK_CTA,
__MCC_QUICK_TPHP,
__MCC_QUICK_M,
__MCC_QUICK_SCALM,
__MCC_QUICK_SHRINK,
__MCC_QUICK_XM,
__MCC_QUICK_WM,
__MCC_QUICK_LM,
__MCC_QUICK_XP,
__MCC_QUICK_WP,
__MCC_QUICK_XOM,
__MCC_QUICK_XOI,
__MCC_QUICK_VJ,
__MCC_QUICK_CJO,
__MCC_QUICK_KMS,
__MCC_QUICK_FC,
__MCC_QUICK_DCAP,
__MCC_QUICK_HDIF,
__MCC_QUICK_LDIF,
__MCC_QUICK_DW,
__MCC_QUICK_LD,
__MCC_QUICK_RD,
__MCC_QUICK_RS,
__MCC_QUICK_RDC,
__MCC_QUICK_RSC,
__MCC_QUICK_RSH,
__MCC_QUICK_RLEV,
__MCC_QUICK_JR,
__MCC_QUICK_ER,
__MCC_QUICK_FCS,
__MCC_QUICK_CALCACM,
__MCC_QUICK_TMOD,
__MCC_QUICK_DELW,
__MCC_QUICK_DELL,
__MCC_QUICK_PG,
__MCC_QUICK_VDGR,
__MCC_QUICK_CJGATE,
__MCC_QUICK_CJGR,
__MCC_QUICK_CJSR,
__MCC_QUICK_VDSR,
__MCC_QUICK_PS,
__MCC_QUICK_MJSW,
__MCC_QUICK_CJSW,
__MCC_QUICK_TRDIO9,
__MCC_QUICK_CJBR,
__MCC_QUICK_VDBR,
__MCC_QUICK_VR,
__MCC_QUICK_TLEVC,
__MCC_QUICK_PB,
__MCC_QUICK_MJ,
__MCC_QUICK_CJ,
__MCC_QUICK_DIOLEV,
__MCC_QUICK_EG,
__MCC_QUICK_GAP2,
__MCC_QUICK_GAP1,
__MCC_QUICK_WMLT,
__MCC_QUICK_LMLT,
__MCC_QUICK_TLEV,
__MCC_QUICK_ETADSR,
__MCC_QUICK_SWGAM1,
__MCC_QUICK_SLGAM1,
__MCC_QUICK_GAM1R,
__MCC_QUICK_ETAGAMR,
__MCC_QUICK_SLGAMOO,
__MCC_QUICK_GAMOOR,
__MCC_QUICK_SLVBST,
__MCC_QUICK_VSBTR,
__MCC_QUICK_SWVSBX,
__MCC_QUICK_SLVSBX,
__MCC_QUICK_VSBXR,
__MCC_QUICK_SWKO,
__MCC_QUICK_SWK,
__MCC_QUICK_SLKO,
__MCC_QUICK_SLK,
__MCC_QUICK_KOR,
__MCC_QUICK_KR,
__MCC_QUICK_SWVTO,
__MCC_QUICK_SL2VTO,
__MCC_QUICK_SLVTO,
__MCC_QUICK_WER,
__MCC_QUICK_LER,
__MCC_QUICK_STVTO,
__MCC_QUICK_VTOR,
__MCC_QUICK_PHIBR,
__MCC_QUICK_WOT,
__MCC_QUICK_LAP,
__MCC_QUICK_WVAR,
__MCC_QUICK_LVAR,
__MCC_QUICK_INIT,
__MCC_QUICK_SCREF,
__MCC_QUICK_WEC,
__MCC_QUICK_WEB,
__MCC_QUICK_MIN,
__MCC_QUICK_DMDG,
__MCC_QUICK_GEOMOD,
__MCC_QUICK_TOXREF,
__MCC_QUICK_IGCMOD,
__MCC_QUICK_TCJSWG,
__MCC_QUICK_TPBSWG,
__MCC_QUICK_PERMOD,
__MCC_QUICK_TCJSW,
__MCC_QUICK_TPBSW,
__MCC_QUICK_OPTACM,
__MCC_QUICK_TCJ,
__MCC_QUICK_TPB,
__MCC_QUICK_LAMBDA,
__MCC_QUICK_PBSWGD,
__MCC_QUICK_PBSWGS,
__MCC_QUICK_PBSWS,
__MCC_QUICK_PBSWD,
__MCC_QUICK_PBS,
__MCC_QUICK_PBD,
__MCC_QUICK_MJSWGD,
__MCC_QUICK_MJSWGS,
__MCC_QUICK_CJSWGD,
__MCC_QUICK_CJSWGS,
__MCC_QUICK_CJSWS,
__MCC_QUICK_CJSWD,
__MCC_QUICK_MJSWS,
__MCC_QUICK_MJSWD,
__MCC_QUICK_MJS,
__MCC_QUICK_MJD,
__MCC_QUICK_CJS,
__MCC_QUICK_CJD,
__MCC_QUICK_JSWGS,
__MCC_QUICK_JSWGD,
__MCC_QUICK_JSWS,
__MCC_QUICK_JSWD,
__MCC_QUICK_IJTHSFWD,
__MCC_QUICK_IJTHDFWD,
__MCC_QUICK_IJTHSREV,
__MCC_QUICK_IJTHDREV,
__MCC_QUICK_DWJ,
__MCC_QUICK_DMCG,
__MCC_QUICK_DMCI,
__MCC_QUICK_CKAPPAS,
__MCC_QUICK_CIGD,
__MCC_QUICK_BIGD,
__MCC_QUICK_AIGD,
__MCC_QUICK_NDEF,
__MCC_QUICK_TOXP,
__MCC_QUICK_EPSROX,
__MCC_QUICK_PARAMCHK,
__MCC_QUICK_RDSWMIN,
__MCC_QUICK_RDSMOD,
__MCC_QUICK_TEMPMOD,
__MCC_QUICK_TOXE,
__MCC_QUICK_VOFFL,
__MCC_QUICK_SBREF,
__MCC_QUICK_SAREF,
__MCC_QUICK_TKU0,
__MCC_QUICK_WLODKU0,
__MCC_QUICK_LLODKU0,
__MCC_QUICK_PKU0,
__MCC_QUICK_WKU0,
__MCC_QUICK_LKU0,
__MCC_QUICK_KU0,
__MCC_QUICK_WLOD,
__MCC_QUICK_WLODVTH,
__MCC_QUICK_LLODVTH,
__MCC_QUICK_PKVTH0,
__MCC_QUICK_WKVTH0,
__MCC_QUICK_LKVTH0,
__MCC_QUICK_KVTH0,
__MCC_QUICK_LODK2,
__MCC_QUICK_STK2,
__MCC_QUICK_LODETA0,
__MCC_QUICK_STETA0,
__MCC_QUICK_KVSAT,
__MCC_QUICK_SD,
__MCC_QUICK_SB,
__MCC_QUICK_SA,
__MCC_QUICK_NF,
__MCC_QUICK_DTOXCV,
__MCC_QUICK_VFBFLAG,
__MCC_QUICK_XPART,
__MCC_QUICK_XTI,
__MCC_QUICK_JSW,
__MCC_QUICK_NJ,
__MCC_QUICK_JS,
__MCC_QUICK_CAPMOD,
__MCC_QUICK_TNOM,
__MCC_QUICK_BINUNIT,
__MCC_QUICK_ND,
__MCC_QUICK_WWN,
__MCC_QUICK_WLN,
__MCC_QUICK_WWL,
__MCC_QUICK_WW,
__MCC_QUICK_WL,
__MCC_QUICK_WINT,
__MCC_QUICK_LWN,
__MCC_QUICK_LLN,
__MCC_QUICK_LWL,
__MCC_QUICK_LW,
__MCC_QUICK_LL,
__MCC_QUICK_LINT,
__MCC_QUICK_XW,
__MCC_QUICK_XL,
__MCC_QUICK_NSUB,
__MCC_QUICK_GAMMA2,
__MCC_QUICK_GAMMA1,
__MCC_QUICK_CGSO,
__MCC_QUICK_,
__MCC_QUICK_CGDO,
__MCC_QUICK_NOIMOD,
__MCC_QUICK_PBSW,
__MCC_QUICK_PBSWG,
__MCC_QUICK_PHP,
__MCC_QUICK_ACM,
__MCC_QUICK_PJSWG,
__MCC_QUICK_MJSWG,
__MCC_QUICK_CJSWG,
__MCC_QUICK_WWLC,
__MCC_QUICK_LWLC,
__MCC_QUICK_WWC,
__MCC_QUICK_LWC,
__MCC_QUICK_DWC,
__MCC_QUICK_DLC,
__MCC_QUICK_MOBMOD,
__MCC_QUICK_VERSION,
__MCC_QUICK_LEVEL,
__MCC_QUICK_WMIN,
__MCC_QUICK_WMAX,
__MCC_QUICK_LMIN,
__MCC_QUICK_LMAX,
__MCC_QUICK_DELVT0,
__MCC_QUICK_MULU0,
#ifdef MCC_RESI_CODE
__MCC_QUICK_CAP,
__MCC_QUICK_CAPSW,
__MCC_QUICK_DI,
__MCC_QUICK_RES,
__MCC_QUICK_THICK,
__MCC_QUICK_DL,
#endif
__MCC_QUICK_LAST_ITEM
};

#define __MCC_QUICK_LAST_BINNABLE_ITEM __MCC_QUICK_KU0WE
#define __MCC_QUICK_BIN_L_DEC 1
#define __MCC_QUICK_BIN_W_DEC 2
#define __MCC_QUICK_BIN_P_DEC 3

/****************************************************************************\
 STRUCTURES
\****************************************************************************/
typedef struct mcc_exp {
    struct mcc_exp   *NEXT ;
    char             *NAME ;
    char             *EXPR ;
    eqt_node         *EXPR_TO_TREE;
    double           DEFAULT ;
    } mcc_explist ;
                           
extern struct mcc_modelx *MODEL ;

typedef struct mcc_param {
  struct mcc_param *NEXT ;
  struct mcc_modelx *MODEL ;
  struct mcc_exp   *EXPR ;
  char             *NAME ;
  double           VALUE ;
  double           MCVALUE ;
  char             TYPEVALUE ;
  char             CONSTVALUE;
} mcc_paramlist ;

typedef struct mcc_modelx {
    struct mcc_modelx *NEXT ;
    struct mcc_techno *TECHNO ;
    char              *NAME ;
    char              *SUBCKTNAME ;
    int                MODELTYPE ;
    int                TYPE ;
    int                CASE ;
    mcc_paramlist     *PARAM ;
    ht                *HPARAM ;           
    chain_list        *origparams;
    ptype_list        *USER ;
    long               MCSEED;
    mcc_paramlist     *QUICK[__MCC_QUICK_LAST_ITEM];
    } mcc_modellist ;

typedef struct mcc_techno {
    struct mcc_techno *NEXT ;
    char              *NAME ;
    struct mcc_exp    *EXPR ;
    mcc_modellist     *MODEL ;
    } mcc_technolist ;

typedef struct {
  float VMAX, A, B ;
} mcc_model ;

typedef struct {
  long                 TRWIDTH ;
  long                 TRLENGTH ;
  float                VDD ;
  float                A ;
  float                B ;
  float                RNT ;
  float                RNS ;
  float                VT ; 
  float                KVT ;
  float                VLIN ;
  float                VSAT ;
  float                KR ;
  float                KS ;
  float                RD ;
  float                RS ;
  char                 TYPE ;
  float                VG ;
  float                VB ;
} mcc_trans_mcc;

typedef struct {
  char                *MODELFILE ;
  int                  MODELTYPE ;
  char                *TRANSNAME ;
  int                  TRANSTYPE ;
  int                  TRANSCASE ;
  float                TRLENGTH ;
  float                TRWIDTH ;
  float                AD ;
  float                PD ;
  float                AS ;
  float                PS ;
  float                TEMP ;
  float                VDD ;
  float                RD ;
  float                RS ;
  elp_lotrs_param     *PARAM;
  float                VG ;
  float                VB ;
  lotrs_list          *LOTRS ;
} mcc_trans_spice;

typedef struct {
  mcc_trans_spice *TRS;
  double           VGS;
  double           VDS;
  double           VBS;
} mcc_optim_ids;

typedef struct {
  double I1, I2, I3, I4, I5, I6, IDS ;
} mcc_debug_internal_trans ;

typedef struct {
  mcc_debug_internal_trans TN ;
  mcc_debug_internal_trans TP ;
} mcc_debug_internal ;

typedef struct {
  char   *LABEL ;
  double  VALUE ;
}  mcc_modifiedparam ;

typedef struct {
  double vdsmin ;
  double vgsmin ;
} mcc_corner_info ;


/****************************************************************************\
 GLOBALS   
\****************************************************************************/
extern char *MCC_TN_SUBCKT, *MCC_TP_SUBCKT;
extern lotrs_list *MCC_CURRENT_LOTRS;
extern char *MCC_CURRENT_SUBCKT;
extern mcc_technolist *MCC_HEADTECHNO ;
extern elp_lotrs_param *MCC_MOD_INST_N;
extern elp_lotrs_param *MCC_MOD_INST_P;
extern int     MCC_CALC_VT;
extern int     MCC_OPTIM_MODE;
extern int     MCC_NEW_CALC_ABR;
extern int     MCC_USE_SPI_PARSER;
extern char    MCC_MOD_SEPAR;
extern int     MCC_EQUATE_RANGE ;
extern int     MCC_RESIZE_RANGE ;
extern double  MCC_RANGE_MARGIN ;
extern double  MCC_TECSIZE ;
extern double  MCC_AN ;
extern double  MCC_BN ;
extern double  MCC_AP ;
extern double  MCC_BP ;
extern double  MCC_RNT ;
extern double  MCC_RPT ;
extern double  MCC_KRNT ;
extern double  MCC_KRPT ;
extern double  MCC_RNS ;
extern double  MCC_RPS ;
extern double  MCC_KRNS ;
extern double  MCC_KRPS ;
extern double  MCC_KSN ;
extern double  MCC_KSP ;
extern double  MCC_KRN ;
extern double  MCC_KRP ;
extern double  MCC_VDDmax ;
extern double  MCC_VDD_BEST ;
extern double  MCC_VDD_WORST ;
extern double  MCC_VDDdeg ;
extern double  MCC_VTIN;
extern double  MCC_VTIP;
extern double  MCC_VSSdeg ;
extern double  MCC_VTP ;
extern double  MCC_VT0P ;
extern double  MCC_VTN ;
extern double  MCC_VT0N ;
extern double  MCC_KTN ;
extern double  MCC_KTP ;
extern double  MCC_WN ;
extern double  MCC_LN ;
extern double  MCC_WP ;
extern double  MCC_LP ;
extern double  MCC_ASN ;
extern double  MCC_ASP ;
extern double  MCC_ADN ;
extern double  MCC_ADP ;
extern double  MCC_PSN ;
extern double  MCC_PSP ;
extern double  MCC_PDN ;
extern double  MCC_PDP ;
extern double  MCC_LNMAX ;
extern double  MCC_LNMIN ;
extern double  MCC_WNMAX ;
extern double  MCC_WNMIN ;
extern double  MCC_LPMAX ;
extern double  MCC_LPMIN ;
extern double  MCC_WPMAX ;
extern double  MCC_WPMIN ;
extern double  MCC_DWN ;
extern double  MCC_DLN ;
extern double  MCC_DWP ;
extern double  MCC_DLP ;
extern double  MCC_DWCN ;
extern double  MCC_DWCJN ;
extern double  MCC_DLCN ;
extern double  MCC_DWCP ;
extern double  MCC_DWCJP ;
extern double  MCC_DLCP ;
extern double  MCC_DIF ;
extern double  MCC_XWN ;
extern double  MCC_XLN ;
extern double  MCC_XWP ;
extern double  MCC_XLP ;
extern double  MCC_RACCNS ;
extern double  MCC_RACCND ;
extern double  MCC_RACCPS ;
extern double  MCC_RACCPD ;
extern double  MCC_CGSN ;
extern double  MCC_CGS0N ;
extern double  MCC_CGSUN ;
extern double  MCC_CGSUFN ;
extern double  MCC_CGSU0N ;
extern double  MCC_CGSDN ;
extern double  MCC_CGSDFN ;
extern double  MCC_CGSD0N ;
extern double  MCC_CGSU_N_MAX ;
extern double  MCC_CGSU_N_MIN ;
extern double  MCC_CGSD_N_MAX ;
extern double  MCC_CGSD_N_MIN ;
extern double  MCC_CGSP ;
extern double  MCC_CGS0P ;
extern double  MCC_CGSUP ;
extern double  MCC_CGSUFP ;
extern double  MCC_CGSU0P ;
extern double  MCC_CGSDP ;
extern double  MCC_CGSDFP ;
extern double  MCC_CGSD0P ;
extern double  MCC_CGSU_P_MAX ;
extern double  MCC_CGSU_P_MIN ;
extern double  MCC_CGSD_P_MAX ;
extern double  MCC_CGSD_P_MIN ;
extern double  MCC_CGDN ;
extern double  MCC_CGD0N ;
extern double  MCC_CGD1N ;
extern double  MCC_CGD2N ;
extern double  MCC_CGDCN ;
extern double  MCC_CGDC0N ;
extern double  MCC_CGDC1N ;
extern double  MCC_CGDC2N ;
extern double  MCC_CGDP ;
extern double  MCC_CGD0P ;
extern double  MCC_CGD1P ;
extern double  MCC_CGD2P ;
extern double  MCC_CGDCP ;
extern double  MCC_CGDC0P ;
extern double  MCC_CGDC1P ;
extern double  MCC_CGDC2P ;
extern double  MCC_CGSIN ;
extern double  MCC_CGSICN ;
extern double  MCC_CGSIP ;
extern double  MCC_CGSICP ;
extern double  MCC_CGPN ;
extern double  MCC_CGPU_N_MAX ;
extern double  MCC_CGPU_N_MIN ;
extern double  MCC_CGPD_N_MAX ;
extern double  MCC_CGPD_N_MIN ;
extern double  MCC_CGPP ;
extern double  MCC_CGPU_P_MAX ;
extern double  MCC_CGPU_P_MIN ;
extern double  MCC_CGPD_P_MAX ;
extern double  MCC_CGPD_P_MIN ;
extern double  MCC_CGPO_N ;
extern double  MCC_CGPOC_N ;
extern double  MCC_CGPO_P ;
extern double  MCC_CGPOC_P ;
extern double  MCC_CDSN ;
extern double  MCC_CDS_U_N ;
extern double  MCC_CDS_D_N ;
extern double  MCC_CDSP ;
extern double  MCC_CDS_U_P ;
extern double  MCC_CDS_D_P ;
extern double  MCC_CDPN ;
extern double  MCC_CDP_U_N ;
extern double  MCC_CDP_D_N ;
extern double  MCC_CDPP ;
extern double  MCC_CDP_U_P ;
extern double  MCC_CDP_D_P ;
extern double  MCC_CDWN ;
extern double  MCC_CDW_U_N ;
extern double  MCC_CDW_D_N ;
extern double  MCC_CDWP ;
extern double  MCC_CDW_U_P ;
extern double  MCC_CDW_D_P ;
extern double  MCC_CSSN ;
extern double  MCC_CSS_U_N ;
extern double  MCC_CSS_D_N ;
extern double  MCC_CSSP ;
extern double  MCC_CSS_U_P ;
extern double  MCC_CSS_D_P ;
extern double  MCC_CSPN ;
extern double  MCC_CSP_U_N ;
extern double  MCC_CSP_D_N ;
extern double  MCC_CSPP ;
extern double  MCC_CSP_U_P ;
extern double  MCC_CSP_D_P ;
extern double  MCC_CSWN ;
extern double  MCC_CSW_U_N ;
extern double  MCC_CSW_D_N ;
extern double  MCC_CSWP ;
extern double  MCC_CSW_U_P ;
extern double  MCC_CSW_D_P ;
extern double  MCC_VGS ;
extern double  MCC_VBULKN ;
extern double  MCC_VBULKP ;
extern double  MCC_TEMP ;
extern double  MCC_TEMP_BEST ;
extern double  MCC_TEMP_WORST ;
extern double  MCC_SLOPE ;
extern double  MCC_DC_STEP ;
extern double  MCC_VTH_HIGH ;
extern double  MCC_VTH_LOW ;
extern double  MCC_VTH ;
extern double  MCC_ERROR ;
extern double  MCC_WNeff ;
extern double  MCC_LNeff ;
extern double  MCC_WPeff ;
extern double  MCC_LPeff ;
extern int     MCC_NINDEX ;
extern int     MCC_PINDEX ;
extern int     MCC_NCASE ;
extern int     MCC_PCASE ;
extern int     MCC_SWJUNCAPN ;
extern int     MCC_SWJUNCAPP ;

extern char *MCC_ELPFILE ;
extern char *MCC_TECHFILE ;
extern char *MCC_MODELFILE ;
extern char *MCC_MODELFILE_BEST ;
extern char *MCC_MODELFILE_WORST ;
extern char *MCC_SPICESTRING ;
extern char *MCC_SPICENAME ;
extern char *MCC_SPICEOPTIONS ;
extern char *MCC_TASNAME ;
extern char *MCC_SPICESTDOUT ;
extern char *MCC_SPICEOUT ;
extern char *MCC_TNMODEL ;
extern char *MCC_TPMODEL ;
extern char *MCC_TNMODELTYPE ;
extern char *MCC_TPMODELTYPE ;
extern char *MCC_ELPVERSION ;
extern int MCC_SPICEMODELTYPE ;
extern int MCC_INSNUMB ;
extern int MCC_TRANSNUMB ;
extern int MCC_INVNUMB ;
extern int MCC_NBMAXLOOP ;
extern float MCC_CAPA ;

extern char *MCC_SPICEFILE[] ;
extern int MCC_SPICENB ;
extern char *MCC_TASFILE[] ;
extern int MCC_TASNB ;

extern char *MCC_MOD_NAME[MCC_NB_MOD] ;
extern char *MCC_CASE_NAME[MCC_NB_CASE] ;

extern char MCC_MODE ;
extern char MCC_CALC_CUR ;
extern char MCC_CALC_ORG ;
extern int  MCC_FLAG_FIT ;

extern int MCC_DEBUG_MODE ;
extern char *MCC_DEBUG_GATE ;
extern char MCC_DEBUG_INPUT ;
extern char MCC_DEBUG_TRANS ;
extern int  MCC_DEBUG_SLOPE ;
extern int  MCC_DEBUG_CAPA ;
extern char MCC_DEBUG_RSAT ;
extern char MCC_DEBUG_LEAK ;
extern char MCC_DEBUG_RLIN ;
extern char MCC_DEBUG_VTH ;
extern char MCC_DEBUG_QINT ;
extern char MCC_DEBUG_JCT_CAPA ;
extern char MCC_DEBUG_INPUTCAPA ;
extern char MCC_DEBUG_CARAC ;
extern char MCC_DEBUG_CARACNEG ;
extern char MCC_DEBUG_DEG ;
extern char MCC_DEBUG_PASS_TRANS ;
extern char MCC_DEBUG_SWITCH ;   
extern char MCC_DEBUG_BLEEDER ;   
extern char MCC_DEBUG_INTERNAL ;
extern mcc_debug_internal MCC_DEBUG_INTERNAL_DATA ;

/****************************************************************************\
 FUNCTION  
\****************************************************************************/
extern void mcc_initcalcparam            __P((int)) ;
extern void mcc_fit                      __P(()) ;
extern void mcc_optim_readspidata              __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_readspidata              __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_vt                __P((int,elp_lotrs_param*,elp_lotrs_param*,mcc_corner_info *info)) ;
extern void  mcc_calcul_abr               __P((int,double,double,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_vdeg              __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern double mcc_current                __P(()) ;
extern void mcc_freespidata              __P((int,int)) ;
extern void mcc_genspi                   __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_drvelp                   __P(()) ;
extern void mcc_gencurv                  __P(()) ;
extern void mccenv                       __P(()) ;

extern void mcc_calcspicedelay      __P((char *,
                                         chain_list *,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double **
                                       )) ;
extern void mcc_setelpparam             (void);
extern void mcc_genparam            __P((char *,
                                         int,
                                         double,
                                         double,
                                         double,
                                         lotrs_list *,
                                         int,
                                         elp_lotrs_param*
                                       )) ;

extern int     mcc_getmodeltype(char *technoname) ;
extern int     mcc_get_modeltype (mcc_modellist *ptmodel, char *name) ;

extern void    mcc_deltechnofile(char *technoname) ;

extern mcc_technolist  *mcc_gettechnofile (char *technoname) ;

extern void    mcc_initallparam (char *technoname) ;
extern void    mcc_initmodel (mcc_modellist *ptmodel ) ;
extern void    mcc_initparam_com(mcc_modellist *ptmodel) ;

extern char   *mcc_getmodelname(char *technoname, char *transname,
                                int transtype, int transcase, double L, double W) ;
extern int     mcc_gettransindex(char *technoname, char *transname,
                                 int transtype, int transcase, double L, double W) ;

extern double  mcc_getLMIN (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_getLMAX (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_getWMIN (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_getWMAX (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_calcDL (char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, elp_lotrs_param *lotrsparam_p) ;

extern double  mcc_calcDW (char *technoname, char *transname, 
                           int transtype, int transcase, 
                           double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcDWCJ (char *technoname, char *transname, 
                             int transtype, int transcase,
                             double L, double W, elp_lotrs_param *lotrsparam, double temp) ;

extern double  mcc_calcDLC (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcDWC (char *technoname, char *transname, 
                            int transtype, int transcase, 
                            double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_getXL (char *technoname, char *transname,
                          int transtype, int transcase, double L, double W) ;

extern double  mcc_getXW (char *technoname, char *transname,
                          int transtype, int transcase, double L, double W) ;

extern double  mcc_calcVTH(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vbs, double vds,elp_lotrs_param*,int mcclog) ;

extern double  mcc_calcIDS(char *technoname, char *transname, 
                           int transtype, int transcase, double vbs, double vgs, 
                           double vds, double L, double W, 
                           double temp,elp_lotrs_param*) ;


extern double  mcc_calcVDDDEG(char *technoname, char *transname,
                              int transtype, int transcase, double L, double W, 
                              double vdd, double temp,
                              double step,elp_lotrs_param *loins_param) ;

extern double  mcc_calcVSSDEG(char *technoname, char *transname,
                              int transtype, int transcase, double L, double W, 
                              double vdd, double temp, 
                              double step, elp_lotrs_param *loins_param) ;

extern void mcc_PrintQint (char *technoname, char *transname, 
                    int transtype, int transcase, double L, double W,
                    double temp, double vdd,elp_lotrs_param *lotrsparam, char *optnamevdd, char *optnamevss, int usechannel, char location);
extern void mcc_DisplayInfos (char *technoname, char *transname,
                              int transtype, int transcase, double L, double W, 
                              double temp, double vgs, double vbs, double vds,
                              double vdd,elp_lotrs_param *lotrsparam);

extern void mcc_GetInputCapa ( char *technoname, char *transname,
                        int transtype, int transcase, double L, double W, 
                        double temp, double vg1, double vg2, 
                        double vd1, double vd2, double vs1, double vs2,
                        elp_lotrs_param *lotrsparam, 
                        double *ptcgs, double *ptcgd, double *ptcgp);

extern double  mcc_calcCGD(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W,
                           double temp, double vgs0, double vgs1, double vbs, double vds,
                           elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCGSI(char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W,
                            double temp, double vgs, double vbs, double vds,
                           elp_lotrs_param *lotrsparam) ;

extern void mcc_calcQint (char *technoname, char *transname,
                          int transtype, int transcase, double L, double W, 
                          double temp, double vgs,double vbs, double vds,
                          double *ptQg,double *ptQs, double *ptQd, double *ptQb,
                          elp_lotrs_param *lotrsparam);
extern double mcc_calcCGSD (char *technoname, char *transname,
                            int transtype, int transcase, double L, double W, 
                            double temp, double vdd, double vfinal, int vdsnull,
                            elp_lotrs_param *lotrsparam);
extern double mcc_calcCGSU (char *technoname, char *transname,
                            int transtype, int transcase, double L, double W, 
                            double temp, double vdd, double vfinal, int vdsnull,
                            elp_lotrs_param *lotrsparam);
double mcc_calcCGS( char *technoname, char *transname, int transtype, int transcase, double L, double W, double temp, double vgsi, double vgsf, double vdsi, double vdsf, elp_lotrs_param *lotrsparam );
extern double  mcc_calcCGP(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double vdd, double *ptQov,elp_lotrs_param *lotrsparam,double temp
                           ) ;

extern double  mcc_calcCGPU (char *technoname, char *transname, 
                             int transtype, int transcase, double L, double W, 
                             double vdd ) ;

extern double  mcc_calcCGPD (char *technoname, char *transname, 
                             int transtype, int transcase, double L, double W, 
                             double vdd ) ;

extern double  mcc_calcCDS(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCDP(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W,
                           double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCDW(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vbx1, double vbx2, double vgx,
                           elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCSS(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCSP(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W,
                           double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCSW(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vbx1, double vbx2, double vgx,
                           elp_lotrs_param *lotrsparam) ;

extern double mcc_calcCGPO( char *technoname, char *transname,
                            int transtype, int transcase, double L, double W, 
                            double temp, double vg,
                            double vd1, double vd2,
                            elp_lotrs_param *lotrsparam );

extern double  mcc_gettechnolevel(char *technoname ) ;

extern double mcc_calcRapIdsTemp(char *technoname, char *transname, int transtype,
                              int transcase, double L, double W, double vdd, double T0, double T1) ;
extern double mcc_calcRapIdsVolt(char *technoname, char *transname, int transtype,
                                 int transcase, double L, double W, double temp, double V0, double V1) ;

extern void mcc_calcspicedelay      __P((char *,
                                         chain_list *,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double **
                                       )) ;
extern char *mcc_getjoker               __P((char *,
                                             char *
                                           )) ;
extern mcc_trans_mcc* mcc_create_trans_mcc( lotrs_list *lotrs, float vdd, int transcase, losig_list *losigdrain, float vg, float vb );
extern void mcc_delete_trans_mcc( mcc_trans_mcc *trs );
extern char mcc_mcc_characteristic( mcc_trans_mcc *trs, float vgs, float vbs, mcc_model *model, int *nb, int max );
extern char mcc_mcc_ids( mcc_trans_mcc *trs, float vs, float vd, float *ids );
extern char mcc_mcc_vds( mcc_trans_mcc *trs, float vs, float ids, float *vds );
extern char mcc_mcc_vds_list( chain_list *headtrs, float vs, float ids, float *vds );
char mcc_mcc_vth( mcc_trans_mcc *trs, float vbs, float vds, float *vth );

extern mcc_trans_spice* mcc_create_trans_spice( lotrs_list *lotrs, float vdd, losig_list *losigdrain, float vg, float vb );
void mcc_delete_trans_spice( mcc_trans_spice *trs );
extern char mcc_spice_ids( mcc_trans_spice *trs, float vgs, float vbs, float vds, float *ids );
extern char mcc_spice_ids_pol( mcc_trans_spice *trs, float vgs, float vbs, float vds, float *ids );
extern char mcc_spice_vds( mcc_trans_spice *trs, float vgs, float vbs, float ids, float *vds );
char mcc_spice_vth( mcc_trans_spice *trs, float vbs, float vds, float *vth );
char mcc_spice_trace_vth( mcc_trans_spice *trs, float vbsmin, float vbsmax, float vds, char  *filename );
char mcc_mcc_vds_list( chain_list *headtrs, float vs, float ids, float *vds );
char mcc_mcc_ids_list( chain_list *headtrs, float vs, float vd, float *ids );

extern char  *mcc_MakeScript          __P((lofig_list *fig, double m, char *options));
// mcc_mod_util
extern mcc_modellist   *mcc_addmodel(char *technoname, 
                                     char *transname, 
                                     char *subcktname,
                                     int transtype, 
                                     int transcase) ;	
extern void             mcc_setparam(mcc_modellist *ptmodel,
                                     char *name,
                                     double value) ;
extern mcc_technolist *mcc_addtechno(char *technoname);
extern mcc_technolist *mcc_gettechno(char *technoname);
extern mcc_explist     *mcc_addexp(mcc_explist *head,
                                   char *name, 
                                   char *exp ,
                                   double defaut) ;
extern double	        mcc_getparam(mcc_modellist *ptmodel,
                                     char *name) ;
double mcc_getparam_quick(mcc_modellist *ptmodel, int idx);
extern mcc_paramlist   *mcc_chrparam(mcc_modellist *ptmodel,
                                     char *name) ;
mcc_paramlist *mcc_chrparam_quick(mcc_modellist *ptmodel, int idx);
extern double  mcc_get_best_abr_from_vt ( int type,
                                          elp_lotrs_param *lotrsparam_n, 
                                          elp_lotrs_param *lotrsparam_p,
                                          mcc_corner_info *info
                                        );

extern char            *mcc_getsubckt ( char *technoname,
                                        char *transname,
                                        int transtype, 
                                        int transcase,
                                        double L,
                                        double W) ;

extern int             mcc_use_multicorner (void);
extern char           *mcc_get_modelfilename ( int corner );
extern double mcc_calcul_a (double,double,double,double,double,double,double);
extern double mcc_calcul_b (double,double,double,double,double,double,double);
void mcc_calcul_ab (double,double,double,double,double,double,double,double*,double*);
extern double mcc_calcDioCapa ( char *technoname, char *dioname,
                                int modtype, int modcase, 
                                double Va, double Vc, double temp,
                                double area, double perim
                              );
extern double mcc_calcILeakage (char *technoname, char *transname,
                                int transtype, int transcase, 
                                double vbs, double vds, double vgs,
                                double L, double W, double temp,
                                double AD, double PD, double AS, double PS,
                                double *BLeak, double *DLeak, double *SLeak,
                                elp_lotrs_param *lotrsparam);
extern double mcc_calcIleakage_from_lotrs (char *modelname,int type, double l, double w,
                                    lotrs_list *lotrs, int lotrscase,
                                    double vgs,double vds,double vbs, 
                                    double AD,double PD,double AS, double PS, 
                                    double *BLeak,double *DLeak,double *SLeak,
                                    elp_lotrs_param *lotrsparam);

extern double mcc_calcVTI (char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double vdd, double temp, 
                           double step, elp_lotrs_param *lotrsparam);
extern void mcc_calcPAfromgeomod( lotrs_list      *lotrs,
                                  char            *modelname,
                                  int              type,
                                  int              lotrscase,
                                  double           vdd,
                                  elp_lotrs_param *lotrsparam,
                                  double          *as, 
                                  double          *ad, 
                                  double          *ps, 
                                  double          *pd 
                                );
char mcc_spice_vds_list( chain_list *trs, float vs, float ids, float *vds );
char mcc_spice_ids_list( chain_list *trs, float vs, float vd, float *ids );
char mcc_mcc_vds_list_estim_vds( chain_list *headtrs, float vs, float ids, float *vds );
char mcc_mcc_vds_list_compute_sum_current( chain_list *headtrs, float vs, float vd, float *i );
void mcc_mcc_vds_list_compute_raccess( chain_list *headtrs, float *rs, float *rd );
int mcc_get_sat_parameter( double v1, double i1,
                           double v2, double i2,
                           double v3, double i3,
                           double *a,
                           double *b,
                           double *vt
                         );
int mcc_check_sat_param( double a,
                         double b,
                         double vt,
                         double vdd,
                         char   type /* 'B'=branch, 'T'=transistor */
                       );

typedef void (*hitas_tcl_pt_t)(char *name);
extern hitas_tcl_pt_t hitas_tcl_pt;

void mcc_CheckTechno(char *label, char *tn, char *tp);

/******* hack des netlist *******/

#define MCC_HACK_NONE            0
#define MCC_HACK_REMOVE_LOTRS_CAPA 1

void mccHackNetlist( lofig_list *lofig );
void mcc_hacktechno( lofig_list *lofig, int whatjob );
void mcc_restoreparamlist( mcc_modellist *model, mcc_modifiedparam *param );
mcc_modifiedparam* mcc_setparamlist( mcc_modellist *model, chain_list *head, double value );
void mcc_InitGlobalMonteCarloDistributions(lofig_list *lf);

/* External model interface */

typedef struct mcc_modext {
    int (*initparam)(mcc_modellist *ptmodel);
    void (*clean)(mcc_modellist *ptmodel);
    void (*calcQint)(mcc_modellist *ptmodel, double L, double W, double temp, double vgs, double vbs, double vds, double *ptQg, double *ptQs, double *ptQd, double *ptQb, elp_lotrs_param *lotrsparam);
    double (*calcCGP)(mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double vgx, double L, double W, double temp, double *ptQov);
    double (*calcCGD)(mcc_modellist *ptmodel, double L, double W, double temp, double vgs0, double vgs1, double vbs, double vds, elp_lotrs_param *lotrsparam);
    double (*calcCGSI)(mcc_modellist *ptmodel, double L, double W, double temp, double vgs, double vbs, double vds, elp_lotrs_param *lotrsparam);
    double (*calcVTH)(mcc_modellist *ptmodel, double L, double W, double temp, double vbs, double vds, elp_lotrs_param *lotrsparam);
    double (*calcIDS)(mcc_modellist *ptmodel, double vbs, double vgs, double vds, double W, double L, double Temp, elp_lotrs_param *lotrsparam);
    double (*calcDWCJ)(mcc_modellist *mccmodel, elp_lotrs_param *lotrsparam, double temp, double L, double W);
    double (*calcCDS)(mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double temp, double vbx1, double vbx2, double L, double W);
    double (*calcCDP)(mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double temp, double vbx1, double vbx2, double L, double W);
    double (*calcCDW)(mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double temp, double vbx1, double vbx2, double L, double W);
    void (*calcPA)(lotrs_list *ptlotrs, mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double *as, double *ad, double *ps, double *pd);
} mcc_modext;

double mcc_calcResiSimple ( double R, double TC1, double TC2, double DTEMP);

#endif
