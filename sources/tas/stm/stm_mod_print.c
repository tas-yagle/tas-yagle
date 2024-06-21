/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_print.c                                             */
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

void stm_mod_plotprint (timing_model *model)
{
    FILE *f;
    char fname[STM_BUFSIZE], fpname[STM_BUFSIZE];

    sprintf (fname, "%s.dat", model->NAME);
    f = fopen (fname, "w+");
    switch (model->UTYPE) {
        case STM_MOD_MODTBL:
            stm_modtbl_datprint (f, model->UMODEL.TABLE);
            break;
        case STM_MOD_MODSCM:
            break;
        case STM_MOD_MODPLN:
            break;
    }
    fclose (f);

    sprintf (fpname, "%s.plot", model->NAME);
    f = fopen (fpname, "w+");
    switch (model->UTYPE) {
        case STM_MOD_MODTBL:
            stm_modtbl_plotprint (f, fname, model->UMODEL.TABLE);
            break;
        case STM_MOD_MODSCM:
            break;
        case STM_MOD_MODPLN:
            break;
    }
    fclose (f);
}

/****************************************************************************/

void stm_mod_print (FILE *f, timing_model *model)
{
    fprintf (f, "\tmodel (\n");
    fprintf (f, "\t\tname (%s)\n", model->NAME);
    if(model->VTH != STM_UNKNOWN)
    fprintf (f, "\t\tvth (%.5g)\n", model->VTH);
    else
        fprintf (f, "\t\tvth (unknown)\n");
    if(model->VDD != STM_UNKNOWN)
    fprintf (f, "\t\tvdd (%.5g)\n", model->VDD);
    else
        fprintf (f, "\t\tvdd (unknown)\n");
    if(model->VT != STM_UNKNOWN)
    fprintf (f, "\t\tvt (%.5g)\n", model->VT);
    else
        fprintf (f, "\t\tvt (unknown)\n");
    if(model->VF != STM_UNKNOWN)
        fprintf (f, "\t\tvf (%.5g)\n", model->VF);
    else
        fprintf (f, "\t\tvf (unknown)\n");
    if(model->TTYPE == STM_HL)
        fprintf (f, "\t\ttran (ud)\n");
    else if(model->TTYPE == STM_HH)
        fprintf (f, "\t\ttran (uu)\n");
    else if(model->TTYPE == STM_LH)
        fprintf (f, "\t\ttran (du)\n");
    else if(model->TTYPE == STM_LL)
        fprintf (f, "\t\ttran (dd)\n");


    switch (model->UTYPE) {
        case STM_MOD_MODTBL:
            stm_modtbl_print (f, model->UMODEL.TABLE);
            break;
        case STM_MOD_MODSCM:
            stm_modscm_print (f, model->UMODEL.SCM);
            break;
        case STM_MOD_MODPLN:
            stm_modpln_print (f, model->UMODEL.POLYNOM);
            break;
        case STM_MOD_MODFCT:
            stm_modfct_print (f, model->UMODEL.FUNCTION);
            break;
        case STM_MOD_MODIV:
            stm_modiv_print( f, model->UMODEL.IV );
            break;
    }

    stm_noise_print( f, model );
    stm_energy_print( f, model );

    fprintf (f, "\t)\n");
}
