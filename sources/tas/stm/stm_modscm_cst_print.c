/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual_print.c                                     */
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

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void stm_modscm_cst_print (FILE *f, cstparams *params)
{
    fprintf (f, "\t\tscm_cst (\n");
    
    fprintf (f, "\t\t\tconst (\n");
    fprintf (f, "\t\t\t\t(delay slew)\n");
    fprintf (f, "\t\t\t\t(");
    fprintf (f, "%.5g ", params->DS[STM_CST_DELAY]);
    fprintf (f, "%.5g)\n", params->DS[STM_CST_SLEW]);
    
    fprintf (f, "\t\t\t)\n");
    fprintf (f, "\t\t)\n");
}
