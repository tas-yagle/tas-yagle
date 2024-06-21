/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : sim_message.h                                               */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef SIM_MESSAGE_H
#define SIM_MESSAGE_H

#include MUT_H
#include <stdarg.h>

void sim_war(char *text, ... );
void sim_err(char *text, ... );

#endif
