/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : vcd_parse_core.h                                            */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef VCD_PARSE_CORE_H
#define VCD_PARSE_CORE_H

#include MUT_H

int   vcd_cdl_parsed(void);
void  vcd_parse_core(char *filename,char *ext);
void  vcd_newCDLToVCDName(char *cdl, char *vcd);
char *vcd_getVCDFromCDLName(char *cdl);
void  vcd_newVCDToCDLName(char *vcd, char *cdl);
char *vcd_getCDLFromVCDName(char *vcd);

#endif
