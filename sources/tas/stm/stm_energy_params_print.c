/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_params_print.c                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void    stm_energy_params_print (FILE *f, energyparams *params )
{
    fprintf (f, "\t\tenergy_model (\n");
    
    fprintf (f, "\t\t\t(pconf0 pconf1)\n");
    fprintf (f, "\t\t\t(");
    fprintf (f, "%.5g ",    params->EP[STM_EPCONF0]);
    fprintf (f, "%.5g)\n",  params->EP[STM_EPCONF1]);
    
    fprintf (f, "\t\t)\n");

}
