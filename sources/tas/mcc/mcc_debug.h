/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_debug.h                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"
#include MCC_H

typedef struct
{
  int nb;
  int nbdyna;
  // MCC_IDNSAT, MCC_IDNRES, MCC_IDNVGS, MCC_IDPSAT, MCC_IDPRES, MCC_IDPVGS, MCC_VDDDEG, MCC_VSSDEG
  double *I_SIMU[8];
  double *I_OPTIM[8];
  double *I_ELP[8]; // only 6 used
  double *I_BSIM[8];

  double *I_VDS_RSATLIN[2][3][2][2];
  double *QINT_SPICE[2][2][2][4];
  double *QINT_BSIM[2][2][2][4];
  double tabspin[4], tabspip[4] ;
  double tabmccn[4], tabmccp[4] ;
  double tabelpn[4], tabelpp[4] ;
  double *SIMUINV[2][2];
/*  double *I_bsim;
  double *I_mcc;
  double *I_elp;*/
} Icurvs;

extern Icurvs TRS_CURVS; 
extern char *MCC_DEBUG_PREFIX;

extern void mcc_calc_jct_capa __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_rapisat_fqmulu0  __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_rapisat_fdmulu0  __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_isat_fmulu0  __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_isat_fdelvt0 __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_abr_fdelvt0  __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_abr_fmulu0   __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_rsat         __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_rlin         __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_vsat         __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_vth          __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_qint         __P((elp_lotrs_param*,elp_lotrs_param*,char)) ;
extern void mcc_sim_cg(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, char trs, char con);
extern void mcc_calcul_trans        __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_transneg     __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_deg          __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_genbench            __P((void)) ;
extern void mcc_calcul_leak         __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern char *mcc_debug_prefix(char *name);
void mcc_cal_con_capa(char trs, char con);
void mcc_runtas_tcl(char *gate,float slope,float capa, char *output,char*figname);
void mcc_fill_current_for_rsat(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p);
void mcc_bilan_capa(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p);
void mcc_bilan_capa_simulated_inverter(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, int *maxl);
void mcc_drive_summary(char *bsim);
void mcc_calc_coupling_capa(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p);
void mcc_check_capa( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p );
