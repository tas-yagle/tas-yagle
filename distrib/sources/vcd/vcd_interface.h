/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VCD Version 2.00                                            */
/*    Fichier : vcd_interface.c                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include VCD_H 
#include "vcd_util.h"

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern void        vcd_parsefile            (char *name, char *modulename);
extern int         vcd_db_check             ();
extern void        vcd_list_sigs            ();
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
extern void        vcd_print_waveform       (trans_list *waveform);
extern trans_list *vcd_get_waveform         (char *signal);
extern trans_list *vcd_get_first_event      (trans_list *waveform);
extern trans_list *vcd_get_next_event       (trans_list *event);
extern trans_list *vcd_get_prev_event       (trans_list *event);
extern void        vcd_print_event          (trans_list *event);
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
extern chain_list *vcd_get_current_events   ();
extern void        vcd_print_current_events (chain_list *events);
extern long        vcd_count_events         (chain_list *events);
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
extern chain_list *vcd_get_state            ();
extern void        vcd_print_state          (chain_list *state);
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
extern void        vcd_goto_time            (long time);
extern char        vcd_goto_next_time       ();
extern void        vcd_goto_prev_time       ();
extern void        vcd_goto_init_time       ();
extern long        vcd_get_time             ();
extern void        vcd_add_nrj              (float nrj);
extern void        vcd_add_trans_nrj        (trans_list *trans, float nrj);
extern long        vcd_get_time_from_index  (long index);
extern float       vcd_get_nrj_from_index   (long index);
extern long        vcd_get_max_index        ();

