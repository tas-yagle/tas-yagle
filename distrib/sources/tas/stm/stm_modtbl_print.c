/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_print.c                                          */
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

void stm_modtbl_datprint (FILE *f, timing_table *table)
{
    long  i, j;

    if (table->SET2D) {
        if (table->XTYPE == STM_LOAD && table->YTYPE == STM_INPUT_SLEW) {
            for (i = 0; i < table->NY; i++) {
                for (j = 0; j < table->NX; j++) 
                    fprintf (f, "%f %f %f\n", table->YRANGE[i], table->XRANGE[j], table->SET2D[i][j]);
                fprintf (f, "\n");
            }
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_LOAD) {
            for (i = 0; i < table->NX; i++) {
                for (j = 0; j < table->NY; j++) 
                    fprintf (f, "%f %f %f\n", table->XRANGE[i], table->YRANGE[j], table->SET2D[i][j]);
                fprintf (f, "\n");
            }
        }
        if (table->XTYPE == STM_CLOCK_SLEW && table->YTYPE == STM_INPUT_SLEW) {
            for (i = 0; i < table->NY; i++) {
                for (j = 0; j < table->NX; j++) 
                    fprintf (f, "%f %f %f\n", table->YRANGE[i], table->XRANGE[j], table->SET2D[i][j]);
                fprintf (f, "\n");
            }
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_CLOCK_SLEW) {
            for (i = 0; i < table->NX; i++) {
                for (j = 0; j < table->NY; j++) 
                    fprintf (f, "%f %f %f\n", table->XRANGE[i], table->YRANGE[j], table->SET2D[i][j]);
                fprintf (f, "\n");
            }
        }
    }

    if (table->SET1D)
        for (i = 0; i < table->NX; i++)
            fprintf (f, "%f %f\n", table->XRANGE[i], table->SET1D[i]);

    if (!stm_modtbl_isnull (table->CST)) {
        fprintf (f, "%f %f\n", 0.0, table->CST);
        fprintf (f, "%f %f\n", 1.0, table->CST);
    }
}

/****************************************************************************/

void stm_modtbl_plotprint (FILE *f, char *datfile, timing_table *table)
{
    char *xlabel, *ylabel, *zlabel;

    if (table->SET2D) {
        zlabel = "delay (ps)";
        if (table->XTYPE == STM_LOAD && table->YTYPE == STM_INPUT_SLEW) {
            xlabel = "load (fF)";
            ylabel = "slope (ps)";
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_LOAD) {
            xlabel = "slope (ps)";
            ylabel = "load (fF)";
        }
        if (table->XTYPE == STM_CLOCK_SLEW && table->YTYPE == STM_INPUT_SLEW) {
            xlabel = "clock slope (ps)";
            ylabel = "input slope (ps)";
        }
        if (table->XTYPE == STM_INPUT_SLEW && table->YTYPE == STM_CLOCK_SLEW) {
            xlabel = "input slope (ps)";
            ylabel = "clock slope (ps)";
        }
    }

    if (table->SET1D) {
        ylabel = "delay (ps)";
        if (table->XTYPE == STM_INPUT_SLEW)
            xlabel = "input slope (ps)";
        if (table->XTYPE == STM_CLOCK_SLEW)
            xlabel = "clock slope (ps)";
        if (table->XTYPE == STM_LOAD)
            xlabel = "load (fF)";
    }

    if (!stm_modtbl_isnull (table->CST)) {
        ylabel = "delay (ps)";
        xlabel = "";
    }
    
    fprintf (f, "set xlabel '%s'\n", xlabel);
    fprintf (f, "set ylabel '%s'\n", ylabel);
        
    if (table->SET2D) {
        fprintf (f, "set zlabel '%s'\n", zlabel);
        fprintf (f, "set hidden3d\n");
        fprintf (f, "splot '%s' with lines 2\n", datfile);
    }

    if (table->SET1D || !stm_modtbl_isnull (table->CST))
        fprintf (f, "plot '%s' with lines 2\n", datfile);
}

/****************************************************************************/

void stm_modtbl_templateprint (FILE *f, timing_ttable *templ)
{
    int   i;

    fprintf (f, "template (%s\n", templ->NAME);
    if (!templ->NX && !templ->NY)
        fprintf (f, "\tconst (\n");
    else
        fprintf (f, "\tspline (\n");
    
    /* axe X */
    
    if (templ->NX > 0) {
        fprintf (f, "\t\t");
        switch (templ->XTYPE) {
            case STM_INPUT_SLEW :
                fprintf (f, "input_slope_axis (");
                break;
            case STM_CLOCK_SLEW :
                fprintf (f, "clock_slope_axis (");
                break;
            case STM_LOAD :
                fprintf (f, "load_axis (");
                break;
        }
        for (i = 0; i < templ->NX; i++) 
            if (!i)
                fprintf (f, "%.2f", templ->XRANGE[i]);
            else
                fprintf (f, " %.2f", templ->XRANGE[i]);
        fprintf (f, ")\n");
    }
    
    /* axe Y */
    
    if (templ->NY > 0) {
        fprintf (f, "\t\t");
        switch (templ->YTYPE) {
            case STM_INPUT_SLEW :
                fprintf (f, "input_slope_axis (");
                break;
            case STM_CLOCK_SLEW :
                fprintf (f, "clock_slope_axis (");
                break;
            case STM_LOAD :
                fprintf (f, "load_axis (");
                break;
        }
        for (i = 0; i < templ->NY; i++) 
            if (!i)
                fprintf (f, "%.2f", templ->YRANGE[i]);
            else
                fprintf (f, " %.2f", templ->YRANGE[i]);
        fprintf (f, ")\n");
    }

    fprintf (f, "\t)\n");
    fprintf (f, ")\n\n");

}
/****************************************************************************/

void stm_modtbl_print (FILE *f, timing_table *table)
{
    int   i, j;

    if (!table->NX && !table->NY)
        fprintf (f, "\t\tconst (\n");
    else
        fprintf (f, "\t\tspline (\n");
    
    /* axe X */
    
    if (table->NX > 0) {
        fprintf (f, "\t\t\t");
        switch (table->XTYPE) {
            case STM_INPUT_SLEW :
                fprintf (f, "input_slope_axis (");
                break;
            case STM_CLOCK_SLEW :
                fprintf (f, "clock_slope_axis (");
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
                fprintf (f, "input_slope_axis (");
                break;
            case STM_CLOCK_SLEW :
                fprintf (f, "clock_slope_axis (");
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
        fprintf(f, "\t\t\tdata (%.2f)\n", table->CST);
    
    /* table 1D */

    if (table->SET1D) {
        fprintf(f, "\t\t\tdata (\n");
        fprintf(f, "\t\t\t\t(");
        for (i = 0; i < table->NX - 1; i++)
            fprintf (f, "%.2f ", table->SET1D[i]);
        fprintf (f, "%.2f)\n", table->SET1D[table->NX - 1]);
        fprintf (f, "\t\t\t)\n");
    }
    
    /* table 2D */

    if (table->SET2D) {
        fprintf (f, "\t\t\tdata (\n");
        for (i = 0; i < table->NX - 1; i++) {
            fprintf (f, "\t\t\t\t(");
            for (j = 0; j < table->NY - 1; j++) {
                fprintf (f, "%.2f ", table->SET2D[i][j]);
            }
            fprintf (f, "%.2f)\n", table->SET2D[i][table->NY - 1]);
        }
        fprintf (f, "\t\t\t\t(");
        for (j = 0; j < table->NY - 1; j++) {
            fprintf (f, "%.2f ", table->SET2D[table->NX - 1][j]);
        }
        fprintf (f, "%.2f)\n", table->SET2D[table->NX - 1][table->NY - 1]);
        fprintf (f, "\t\t\t)\n");
    }

    fprintf (f, "\t\t)\n");
}
