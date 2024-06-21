/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_mm9.h                                                 */
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

#define MCC_EPS1     1.0e-2
#define MCC_EPS2     0.1
#define MCC_EPS3     1.0e-2
#define MCC_EPS4     5.0e-4
#define MCC_EPS6     0.03 
#define MCC_EPS7     1.0e-12
#define MCC_LAMBDA1  0.1   
#define MCC_LAMBDA2  1.0e-4   
#define MCC_LAMBDA3  1.0e-8   
#define MCC_LAMBDA4  0.3
#define MCC_LAMBDA5  0.1
#define MCC_LAMBDA6  0.3
#define MCC_LAMBDA7  37.0
#define MCC_LAMBDA8  0.1 

/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/

extern double mcc_hyp1(double x, double eps) ;
extern double mcc_hyp2(double x, double x0, double eps) ;
extern double mcc_hyp3(double x, double x0, double eps) ;
extern double mcc_hyp4(double x, double x0, double eps) ;
extern double mcc_hyp5(double x, double x0, double eps) ;

extern void   mcc_initparam_mm9(mcc_modellist *ptmodel) ;
extern void   mcc_initparam1_mm9(mcc_modellist *ptmodel) ;
extern void   mcc_initparam2_mm9(mcc_modellist *ptmodel) ;
extern double mcc_calcVTH_mm9(mcc_modellist *ptmodel,
                              double L, double W, 
                              double temp, double vbs,
                              double vds, double vgs) ;
