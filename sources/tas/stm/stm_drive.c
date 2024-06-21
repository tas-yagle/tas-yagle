/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_drive.c                                                 */
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

void stm_driveall ()
{
    chain_list *mch;

    for (mch = STM_CELLS_CHAIN; mch; mch = mch->NEXT)
        stm_drivecell ((char*)mch->DATA);
}

/****************************************************************************/

void stm_driveheader (FILE *f)
{
    time_t    counter;
    char     *date;
    
    time (&counter);
    date = ctime (&counter);
    date[strlen (date) - 1] = '\0';
    fprintf (f, "header (\n");
    fprintf (f, "\tdate (\"%s\")\n", date);
    fprintf (f, ")\n\n");
    
}

/****************************************************************************/

void stm_drivecell (char *cellname)
{
    FILE        *f;
    char         file[STM_BUFSIZE];
    timing_cell *cell;
    chain_list  *ch;

    if (STM_CELLS_HT==NULL) return;

    cell = (timing_cell*)gethtitem (STM_CELLS_HT, namealloc (cellname));

    if (((long)cell != EMPTYHT) && ((long)cell != DELETEHT)) {
        if (!strcmp (STM_OUT, "plot")) { 
            for (ch = cell->MODEL_LIST; ch; ch = ch->NEXT)
                stm_mod_plotprint ((timing_model*)ch->DATA);
        } else {

            if (!(f = mbkfopen (cell->NAME, STM_OUT, WRITE_TEXT))) {
                sprintf (file, "%s.%s", cell->NAME, STM_OUT);
                avt_errmsg (STM_ERRMSG,"007", AVT_ERROR, file);
                return;
            }
            stm_driveheader (f);
            stm_cell_print (f, cell);
            fclose (f);
/*
            if (cell->SIM_MODEL_LIST) {
                if (!(f = mbkfopen (cell->NAME, "sim", WRITE_TEXT))) {
                    sprintf (file, "%s.%s", cell->NAME, "sim");
                    avt_errmsg (STM_ERRMSG,"007", AVT_ERROR, file);
                    return;
                }
                stm_cell_printfct (f, cell);
                fclose (f);
            }
*/
        }
    } 
}
