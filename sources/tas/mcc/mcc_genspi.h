/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_genspi.h                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"

extern long MCC_SPICEUD_FITA ;
extern long MCC_SPICEDU_FITA ;
extern long MCC_SPICEUD_FITCG ;
extern long MCC_SPICEDU_FITCG ;
extern long MCC_SPICEUD_FITCGNOCAPA ;
extern long MCC_SPICEDU_FITCGNOCAPA ;
extern long MCC_SPICEUD_FITDN ;
extern long MCC_SPICEDU_FITDN ;
extern long MCC_SPICEDU_FITDP ;
extern long MCC_SPICEUD_FITDP ;
extern long MCC_SPICEFUD_FITA ;
extern long MCC_SPICEFDU_FITA ;
extern long MCC_SPICEFUD_FITCG ;
extern long MCC_SPICEFDU_FITCG ;
extern long MCC_SPICEFUD_FITCGNOCAPA ;
extern long MCC_SPICEFDU_FITCGNOCAPA ;
extern long MCC_SPICEFUD_FITDN ;
extern long MCC_SPICEFDU_FITDN ;
extern long MCC_SPICEFDU_FITDP ;
extern long MCC_SPICEFUD_FITDP ;

extern double *MCC_IDNSAT ;
extern double *MCC_IDNRES ;
extern double *MCC_IDNVGS ;
extern double *MCC_IDPSAT ;
extern double *MCC_IDPRES ;
extern double *MCC_IDPVGS ;
extern double *MCC_VDDDEG ;
extern double *MCC_VSSDEG ;
extern double **MCC_FITS ;

extern char surf_string_n[1024];
extern char surf_string_p[1024];
extern elp_lotrs_param mcc_user_lotrsparam_n, mcc_user_lotrsparam_p;

#include MCC_H

void mcc_trs_corner( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, mcc_corner_info *info );
extern double mcc_calcul_isat       __P((double,
                                         double,
                                         int
                                       )) ;
extern double mcc_calcul_ures       __P((double,
                                         double,
                                         int
                                       )) ;
extern double mcc_calcul_ires       __P((double,
                                         double,
                                         int
                                       )) ;
extern double mcc_current           __P((double,
                                         double,
                                         double,
                                         int
                                       )) ;
extern double mcc_spicecurrent      __P((double,
                                         double,
                                         double,
                                         int,
                                         elp_lotrs_param*
                                       )) ;
extern double mcc_spicecalccurrent  __P((double,
                                         double,
                                         double,
                                         int,
                                         elp_lotrs_param*
                                       )) ;
extern double mcc_spicesimcurrent   __P((double,
                                         double,
                                         double,
                                         int
                                       )) ;
extern double mcc_spicevdeg          __P((
                                         int,
                                         elp_lotrs_param*
                                       )) ;

extern double mcc_calcdiffmodelsat  __P((double,
                                         double,
                                         int,
                                         elp_lotrs_param*,
                                         double,
                                         mcc_corner_info*
                                       )) ;
extern void mcc_calcul_vt           __P((int,elp_lotrs_param*,elp_lotrs_param*,mcc_corner_info *info)) ;
extern double mcc_calcul_a          __P((double,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double
                                       )) ;
extern double mcc_calcul_b          __P((double,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double
                                       )) ;
extern double mcc_calcul_r          __P((double,
                                         double,
                                         double,
                                         double
                                       )) ;
extern double mcc_calcul_rs         __P((double,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double
                                       )) ;
extern void mcc_calcul_k            __P((int,elp_lotrs_param*)) ;
extern void mcc_calcul_abr          __P((int,double,double,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_vti          __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_calcul_vdeg         __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_drvspitab           __P((int,
                                         double**,
                                         int,
                                         int,
                                         elp_lotrs_param*,
                                         elp_lotrs_param*
                                       )) ;
extern void mcc_prsspifile          __P((int,
                                         int,
                                         int,
                                         double **
                                       )) ;
extern void mcc_addspidata          __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_readspidata         __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_allocspidata        __P((int)) ;
extern void mcc_freespidata         __P((int,int)) ;
extern void mcc_runspice            __P((int)) ;
//extern void mcc_execspice           __P((char *,int)) ;
extern void mcc_genspicom           __P((FILE *,
                                         char *,
                                         elp_lotrs_param *,
                                         elp_lotrs_param *
                                       )) ;
extern void mcc_genspi              __P(( 
                                         int,
                                         elp_lotrs_param *,
                                         elp_lotrs_param *
                                       )) ;
extern void mcc_calcspicedelay      __P((char *,
                                         chain_list *,
                                         double,
                                         double,
                                         double,
                                         double,
                                         double **
                                       )) ;
extern void mcc_genparam            __P((char *,
                                         int,
                                         double,
                                         double,
                                         double,
                                         lotrs_list*,
                                         int,
                                         elp_lotrs_param*
                                       )) ;
extern void mcc_setelpparam         __P((void)) ;
extern double  mcc_get_best_abr_from_vt ( int type,
                                          elp_lotrs_param *lotrsparam_n, 
                                          elp_lotrs_param *lotrsparam_p,
                                          mcc_corner_info *info
                                        );
extern void mcc_update_technoparams(char*,int,double,double,lotrs_list*,int);
extern double mcc_calcIleakage_from_lotrs (char *modelname,int type, double l, double w,
                                    lotrs_list *lotrs, int lotrscase,
                                    double vgs,double vds,double vbs, 
                                    double AD,double PD,double AS, double PS, 
                                    double *BLeak,double *DLeak,double *SLeak,
                                    elp_lotrs_param *lotrsparam);
extern void mcc_calcul_raccess(int,elp_lotrs_param*,elp_lotrs_param*);
double mcc_calcul_vt_error( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, double testvt, mcc_corner_info *info );
void mcc_calcul_rst( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p );
void mcc_printf_instance_specific(FILE *file, elp_lotrs_param *lotrsparam_n);
void mcc_get_np_vgs(float *vgsn, float *vgsp);
void mcc_genspi_for_inverter_simulator(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p);
void mcc_calcul_ibranch(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p);
void mcc_calcul_ibranch_spice(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, double *tabin, double *tabip);
void mcc_calcul_ibranch_mcc( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, double *tabin, double *tabip, int n, char mode );
char *mcc_check_subckt(char *nmos, char *pmos);
void mcc_check_techno_with_subckt(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, char *subcktmodeln, char *subcktmodelp);
void mcc_update_technoparams (char *modelname,int type, double l, double w,lotrs_list *lotrs,int lotrscase);
void mcc_optim_resetsearch_for_new_pwl();

