/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_table_print.c                                    */
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

void    stm_energy_table_print (FILE *f, timing_table *table )
{
    int   i, j;

    if (!table->NX && !table->NY)
        fprintf (f, "\t\tenergy_const (\n");
    else
        fprintf (f, "\t\tenergy_table (\n");
    
    /* axe X */
    
    if (table->NX > 0) {
        fprintf (f, "\t\t\t");
        switch (table->XTYPE) {
            case STM_INPUT_SLEW :
                fprintf (f, "input_slew_axis (");
                break;
            case STM_CLOCK_SLEW :
                fprintf (f, "clock_slew_axis (");
                break;
            case STM_LOAD :
                fprintf (f, "load_axis (");
                break;
        }
        for (i = 0; i < table->NX; i++) 
            if (!i)
                fprintf (f, "%.2f", table->XRANGE[i]);
            else
                fprintf (f, " %.2f", table->XRANGE[i]);
        fprintf (f, ")\n");
    }
    
    /* axe Y */
    
    if (table->NY > 0) {
        fprintf (f, "\t\t\t");
        switch (table->YTYPE) {
            case STM_INPUT_SLEW :
                fprintf (f, "input_slew_axis (");
                break;
            case STM_CLOCK_SLEW :
                fprintf (f, "clock_slew_axis (");
                break;
            case STM_LOAD :
                fprintf (f, "load_axis (");
                break;
        }
        for (i = 0; i < table->NY; i++) 
            if (!i)
                fprintf (f, "%.2f", table->YRANGE[i]);
            else
                fprintf (f, " %.2f", table->YRANGE[i]);
        fprintf (f, ")\n");
    }

    /* const */
    
    if (!stm_modtbl_isnull (table->CST))
        fprintf(f, "\t\t\tdata (%.2g)\n", table->CST);
    
    /* table 1D */

    if (table->SET1D) {
        fprintf(f, "\t\t\tdata (\n");
        fprintf(f, "\t\t\t\t(");
        for (i = 0; i < table->NX - 1; i++)
            fprintf (f, "%.2g ", table->SET1D[i]);
        fprintf (f, "%.2g)\n", table->SET1D[table->NX - 1]);
        fprintf (f, "\t\t\t)\n");
    }
    
    /* table 2D */

    if (table->SET2D) {
        fprintf (f, "\t\t\tdata (\n");
        for (i = 0; i < table->NX - 1; i++) {
            fprintf (f, "\t\t\t\t(");
            for (j = 0; j < table->NY - 1; j++) {
                fprintf (f, "%.2g ", table->SET2D[i][j]);
            }
            fprintf (f, "%.2g)\n", table->SET2D[i][table->NY - 1]);
        }
        fprintf (f, "\t\t\t\t(");
        for (j = 0; j < table->NY - 1; j++) {
            fprintf (f, "%.2g ", table->SET2D[table->NX - 1][j]);
        }
        fprintf (f, "%.2g)\n", table->SET2D[table->NX - 1][table->NY - 1]);
        fprintf (f, "\t\t\t)\n");
    }

    fprintf (f, "\t\t)\n");
}
