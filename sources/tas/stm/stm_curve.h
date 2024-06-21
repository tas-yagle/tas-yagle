/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot, Olivier Bichaut                            */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CURVE_H
#define STM_CURVE_H

#define STM_CURVE_DUAL 'd'
#define STM_CURVE_IV   'i'

typedef struct {
  stm_dual_param_timing PARAMTIMING ;
  char                  EVTOUT ;
  float                 VDD ;
} curveparamdual ;

typedef struct {
  float      FIN ;
  float      LOAD ;
  float      VDD ;
  timing_iv *MODEL ;
} curveparamiv ;

typedef union {
  curveparamdual DUAL ;
  curveparamiv   IV ;
} curveparammodels ;

typedef struct {
  char             TYPE ;
  curveparammodels MODEL ;
} curveparam ;

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
stm_curve* stm_curve_alloc( timing_model *model, float t0, float tmax, int nbpoints );
void stm_alloc_static( stm_curve *curve, int nbe, int nbs );
extern void stm_curve_free( stm_curve *curve );

extern stm_curve* stm_curve_c( timing_model *model, float fin, float cout, float t0, float tmax );
extern stm_curve* stm_curve_pi( timing_model *model, float fin, float c1out, float c2out, float rout, float t0, float tmax );
void stm_curve_add_time_data( stm_curve *curve, float t, float ve, float vs, float i, float iconf);

extern void stm_model_plot( char *filename, timing_model *model, float fin, stm_pwl *pwlin, stm_driver *driver, float r, float c1, float c2, char evtin, char evtout, float delaytas, float slewtas, char *config );

#endif
