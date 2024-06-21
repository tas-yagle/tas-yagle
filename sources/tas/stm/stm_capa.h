/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_capa.h                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot                                             */
/*                                                                          */
/****************************************************************************/

extern float stm_solvepi (float i, float r, float c1, float c2, float vth);
extern float stm_capaeq  (float i, float r, float c1, float c2, float vth, char *signame);

void stm_calc_pi_pwl_plot( mbk_laplace *laplace, mbk_pwl *lines, char (*fn)( void*, double, double* ), void *data, double r, double c1, double c2, double c, double tmax, char *filename );
double stm_calc_c_pwl( mbk_laplace *laplace, double t, double c );
double stm_calc_pi_pwl( mbk_laplace *laplace, double t, double r, double c1, double c2 );
double stm_calc_pi_pwl_instant( mbk_laplace *laplace, double vth, double r, double c1, double c2, double testim );
double stm_capaeq_laplace( mbk_laplace *laplace, double tpi, double vth );
float stm_capaeq_fn( char (*fn)( void*, double, double* ), void *data, double tmax, float r, float c1, float c2, float vth, float vsat, float rlin, float vdd, char *signame );
double stm_calc_pi_pwl_2( mbk_laplace *laplace, double t, double r, double c1, double c2 );
double stm_calc_pi_rlin( double rl, double vdd, double r, double c1, double c2, double v1, double v2, double vth );
