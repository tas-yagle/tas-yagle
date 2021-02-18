/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modiv.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODIV_H
#define STM_MODIV_H
void stm_modiv_setis( timing_iv *iv, int ne, int ns, float is );
float stm_modiv_getis( timing_iv *iv, int ne, int ns );
extern timing_iv* stm_modiv_create( int nve, int nvs, float vemax, float vsmax, char (*fnct)( void *data, float ve, float vs, float *is ), void *data );
timing_iv* stm_modiv_duplicate( timing_iv *orig );
timing_iv* stm_modiv_alloc( int nve, int nvs );
extern void stm_modiv_destroy( timing_iv *iv );

extern void stm_modiv_set_cf( timing_iv *iv, float ci, float p0, float p1, float irap );

extern void stm_modiv_set_in( timing_iv *iv, float vt, float vi, float vf, float vth );
float stm_modiv_in_vi( timing_iv *iv );
float stm_modiv_in_vf( timing_iv *iv );
float stm_modiv_in_vth( timing_iv *iv );
float stm_modiv_in_vt( timing_iv *iv );

extern void stm_modiv_set_ti( timing_iv *iv, float vi );
float stm_modiv_ti_vi( timing_iv *iv );

#endif
