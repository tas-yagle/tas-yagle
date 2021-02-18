/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : gen_API_signals.h                                           */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef GEN_API_SIGNALS_H
#define GEN_API_SIGNALS_H

#include "gen_API_netlist.h"
#include "gen_API.h"

#ifndef __func__
#define __func__ "gen_API_signals"
#endif

void gns_AddCapa(lofig_list *lofig, char *con_name, double capa);
void gns_AddResi(lofig_list *lofig, char *con1_name, char *con2_name, double resi);
void gns_AddLineRC(lofig_list *lofig, char *con1_name, char *con2_name, double resi, double capa1, double capa2);

#endif
