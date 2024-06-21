/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_print.c                                          */
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

void    stm_energy_print (FILE *f, timing_model *model )
{
    if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE != 1 )
     &&( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE != 5 ))
        return ;

    switch (model->ENERGYTYPE) {
        case STM_ENERGY_TABLE:
            stm_energy_table_print (f, model->ENERGYMODEL.ETABLE);
            break;
        case STM_ENERGY_PARAMS:
            stm_energy_params_print (f, model->ENERGYMODEL.EPARAMS);
            break;
    }
}
