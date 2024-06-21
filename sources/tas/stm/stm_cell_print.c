/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_cell_print.c                                            */
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

void stm_cell_print (FILE *f, timing_cell *cell)
{
    chain_list  *ch;

    fprintf (f, "cell (\n");
    fprintf (f, "\tname (%s)\n", cell->NAME);
        
    for (ch = cell->MODEL_LIST; ch; ch = ch->NEXT)
        stm_mod_print (f, (timing_model*)ch->DATA);
    
    fprintf(f, ")\n\n");
}

/****************************************************************************/
#if 0
void stm_cell_printfct (FILE *f, timing_cell *cell)
{
    chain_list  *ch;
    sim_model *sc;

    fprintf (f, "SimulationContext *SC;\n\n");
        
    for (ch = cell->SIM_MODEL_LIST; ch; ch = ch->NEXT) {
        sc = (sim_model*)ch->DATA;
        sim_DriveContext(sc, f);
    }
}

#endif
