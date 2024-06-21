/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_print.c                                          */
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

void stm_modscm_print (FILE *f, timing_scm *scm)
{
    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            stm_modscm_dual_print (f, scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            stm_modscm_good_print (f, scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            stm_modscm_false_print (f, scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            stm_modscm_path_print (f, scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            stm_modscm_cst_print (f, scm->PARAMS.CST);
            break;
    }
}
