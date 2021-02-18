/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_optim.h                                                 */
/*                                                                          */
/*    (c) copyright 1991-2003 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc KUOCH                                                */
/*                                                                          */
/****************************************************************************/

#include MCC_H
#include "mcc.h"

/****************************************************************************\
 GLOBALS
\****************************************************************************/
extern mbk_pwl *MCC_OPTIM_IDNSAT;
extern mbk_pwl *MCC_OPTIM_IDNRES;
extern mbk_pwl *MCC_OPTIM_IDNVGS;
extern mbk_pwl *MCC_OPTIM_IDPSAT;
extern mbk_pwl *MCC_OPTIM_IDPRES;
extern mbk_pwl *MCC_OPTIM_IDPVGS;

/****************************************************************************\
 DEFINES
\****************************************************************************/
#define MCC_DELAY_UD_FIT_A_LABEL ((char*)"D_FIT_A_UD")
#define MCC_DELAY_DU_FIT_A_LABEL ((char*)"D_FIT_A_DU")
#define MCC_SLOPE_UD_FIT_A_LABEL ((char*)"S_FIT_A_UD")
#define MCC_SLOPE_DU_FIT_A_LABEL ((char*)"S_FIT_A_DU")

#define MCC_DELAY_UD_FIT_CG_LABEL ((char*)"D_FIT_CG_UD")
#define MCC_DELAY_DU_FIT_CG_LABEL ((char*)"D_FIT_CG_DU")
#define MCC_SLOPE_UD_FIT_CG_LABEL ((char*)"S_FIT_CG_UD")
#define MCC_SLOPE_DU_FIT_CG_LABEL ((char*)"S_FIT_CG_DU")

#define MCC_DELAY_UD_FIT_CGNOCAPA_LABEL ((char*)"D_FIT_CGNOCAPA_UD")
#define MCC_DELAY_DU_FIT_CGNOCAPA_LABEL ((char*)"D_FIT_CGNOCAPA_DU")
#define MCC_SLOPE_UD_FIT_CGNOCAPA_LABEL ((char*)"S_FIT_CGNOCAPA_UD")
#define MCC_SLOPE_DU_FIT_CGNOCAPA_LABEL ((char*)"S_FIT_CGNOCAPA_DU")

#define MCC_DELAY_UD_FIT_CDN_LABEL ((char*)"D_FIT_CDN_UD")
#define MCC_DELAY_DU_FIT_CDN_LABEL ((char*)"D_FIT_CDN_DU")
#define MCC_SLOPE_UD_FIT_CDN_LABEL ((char*)"S_FIT_CDN_UD")
#define MCC_SLOPE_DU_FIT_CDN_LABEL ((char*)"S_FIT_CDN_DU")

#define MCC_DELAY_UD_FIT_CDP_LABEL ((char*)"D_FIT_CDP_UD")
#define MCC_DELAY_DU_FIT_CDP_LABEL ((char*)"D_FIT_CDP_DU")
#define MCC_SLOPE_UD_FIT_CDP_LABEL ((char*)"S_FIT_CDP_UD")
#define MCC_SLOPE_DU_FIT_CDP_LABEL ((char*)"S_FIT_CDP_DU")

#define MCC_UD_SIG1 ((char*)"s2")
#define MCC_UD_SIG2 ((char*)"s3")
#define MCC_DU_SIG1 ((char*)"s3")
#define MCC_DU_SIG2 ((char*)"s4")

/****************************************************************************\
 FUNCTION
\****************************************************************************/
extern void mcc_optim_addspidata(int type,elp_lotrs_param *lotrsparam_n,
                                 elp_lotrs_param *lotrsparam_p);
extern void mcc_add_print_meas (FILE *file, char *filename);
extern void mcc_optim_readspidata (int fit, elp_lotrs_param *lotrsparam_n,
                                            elp_lotrs_param *lotrsparam_p);
extern double mcc_optim_calcdiffmodelsat (int type, mbk_pwl *optim_ids,double exitvalue,mcc_corner_info *info);
double mcc_optim_calcdiffmodelsat_new(int type, mbk_pwl *pwl, double exitvalue, mcc_corner_info *info);


