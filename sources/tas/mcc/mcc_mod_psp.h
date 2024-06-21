/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_psp.h                                                 */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2001 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gregoire AVOT                                               */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#define MCC_PSP_TUNE_NO_JUNCTION  0x01
#define MCC_PSP_TUNE_NO_EXTRINSIC 0x02

/* USER field of mcc_modellist* */

typedef struct {
  float qg, qb, qd, qs ;
  float qgsov, qgdov, qgb ;
  float qjbd, qjbs ;
} pspcharge ;

typedef struct scachepspcharge {
  struct scachepspcharge *next ;
  pspcharge               charge ;
  char                   *key ;
} cachepspcharge ;

typedef struct {
  int   nplus ;
  int   nminus ;
  float charge ;
} pspbrcharge ;

typedef struct {
  double ab, ls, lg ;
} pspjuncapconfig ;

void mcc_initparam_psp( mcc_modellist *ptmodel );
void mcc_clean_psp( mcc_modellist *ptmodel );

void mcc_calcQint_psp( mcc_modellist *ptmodel, 
                       double L, 
                       double W,
                       double temp, 
                       double vgs, 
                       double vbs, 
                       double vds,
                       double *ptQg, 
                       double *ptQs, 
                       double *ptQd,
                       double *ptQb,
                       elp_lotrs_param *lotrsparam
                     );
double mcc_calcCGP_psp( mcc_modellist *ptmodel,
                        elp_lotrs_param *lotrsparam, 
                        double vgx, 
                        double L, 
                        double W,
                        double temp,
                        double *ptQov 
                      );
double mcc_calcCGD_psp( mcc_modellist *ptmodel, 
                        double L, 
                        double W, 
                        double temp, 
                        double vgs0, 
                        double vgs1, 
                        double vbs, 
                        double vds,
                        elp_lotrs_param *lotrsparam
                      );
double mcc_calcCGSI_psp( mcc_modellist *ptmodel, 
                         double L, 
                         double W, 
                         double temp, 
                         double vgs, 
                         double vbs, 
                         double vds,
                         elp_lotrs_param *lotrsparam
                       );
double mcc_calcVTH_psp( mcc_modellist *ptmodel, 
                        double L, 
                        double W, 
                        double temp, 
                        double vbstrue, 
                        double vds, 
                        elp_lotrs_param *lotrsparam 
                      );
double mcc_calcIDS_psp( mcc_modellist *ptmodel, 
                        double Vbstrue,
                        double Vgs,
                        double Vds, 
                        double W,
                        double L, 
                        double Temp,
                        elp_lotrs_param *lotrsparam
                      );
double mcc_calcDWCJ_psp( mcc_modellist *mccmodel, 
                         elp_lotrs_param *lotrsparam, 
                         double temp,
                         double L, 
                         double W
                       );
double mcc_calcCDS_psp( mcc_modellist  *ptmodel, 
                        elp_lotrs_param *lotrsparam,
                        double          temp, 
                        double          vbx1, 
                        double          vbx2,
                        double          L,
                        double          W
                      );
double mcc_calcCDP_psp( mcc_modellist *ptmodel, 
                        elp_lotrs_param *lotrsparam,
                        double temp, 
                        double vbx1, 
                        double vbx2,
                        double L,
                        double W
                      );
double mcc_calcCDW_psp( mcc_modellist *ptmodel, 
                        elp_lotrs_param *lotrsparam,
                        double temp, 
                        double vbx1, 
                        double vbx2, 
                        double L, 
                        double W 
                      );
