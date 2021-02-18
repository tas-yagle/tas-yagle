/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VCD Version 2.00                                            */
/*    Fichier : vcd_cdl_interface.c                                         */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#ifndef VCD_CDL_INTERFACE_H
#define VCD_CDL_INTERFACE_H

#include VCD_H
#include "vcd_util.h"
#include "vcd_parse.h"
#include "vcd_parse_core.h"
#include "vcd_interface.h"

extern void        cdl_parse              (char *filename);
extern void        cdl_list_sigs          (void);
extern trans_list *cdl_get_waveform       (char *signal);
extern chain_list *cdl_get_current_events (void);
extern chain_list *cdl_get_state          (void);
extern chain_list *cdl_get_sig_list       (void);
extern char       *cdl_getsigname         (chain_list *sig);
extern chain_list *cdl_getnextsig         (chain_list *sig);
extern chain_list *cdl_getnextevent       (chain_list *event);
extern chain_list *cdl_geteventsig        (chain_list *event);
extern char        cdl_geteventval        (chain_list *event);
extern void        cdl_goto_next_time     ();
extern void        cdl_free_event_list    (chain_list *events);

#endif
