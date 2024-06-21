/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_prop.h                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_PROP_H
#define STM_PROP_H

#include STM_H

/* constructors */
extern timing_props *stm_prop_create (timing_model *resmodel, timing_model *capmodel);
extern timing_props *stm_get_prop();
extern void stm_addresmodel (timing_props *properties, timing_model *resmodel);
extern void stm_addcapmodel (timing_props *properties, timing_model *capmodel);

/* destructors */
extern void stm_prop_destroy (timing_props *properties);

/*modifiers*/
extern timing_table *stm_prop_seg2tbl (chain_list *chainseg, char type);
extern void stm_scale_loadmodel (timing_model *model, float scale);


#endif
