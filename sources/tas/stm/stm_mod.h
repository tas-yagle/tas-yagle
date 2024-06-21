/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod.h                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_H
#define STM_MOD_H

#include STM_H

/* constructors */
extern timing_model *stm_mod_create (char *modelname);
extern timing_model *stm_mod_create_fcst (char *name, float value, long modeltype);
extern timing_model *stm_mod_create_ftable (char *name, int nx, int ny, char xtype, char ytype, long modeltype);
extern timing_model *stm_mod_create_table (char *name, int nx, int ny, char xtype, char ytype);
extern timing_model *stm_mod_create_fequac (char *name, float sdt, float sck, float fdt, float fck, float t, long modeltype);
extern timing_model *stm_mod_create_fequa (char *name, float r, float s, float c, float f, float t, long modeltype);
extern timing_model *stm_mod_duplicate (char *dupname, timing_model *model);
extern void stm_mod_update (timing_model *model, float vth, float vdd, float vt, float vf);
extern void stm_mod_update_transition (timing_model *model, char transition);

/* destructors */
extern void stm_mod_destroy (timing_model *model);
// only destroy the model ..anto..
extern void stm_mod_destroy_model(timing_model *model);

// zinaps:
void stm_mod_update_dynamic_info (timing_model *model, char *newinsname);


extern long stm_get_last_index(char *cellname);

#endif
