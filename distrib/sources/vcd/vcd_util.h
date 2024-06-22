/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : vcd_util.h                                                  */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef VCD_UTIL_H
#define VCD_UTIL_H

#include VCD_H
#include "vcd_parse.h"
#include "vcd_parse_core.h"

chain_list *cdl_gettranssig (trans_list *trn);
trans_list *vcd_dupTransList(trans_list *trans);
int vcd_getVectSize(char *name);
trans_list *vcd_dupTrans(trans_list *trans);
void vcd_unVectEvent(chain_list **chain, trans_list *trans);
void vcd_unVectState(chain_list **chain, trans_list *trans);
float vcd_TimeUnit (char *name);
float vcd_get_ts(void);

#endif
