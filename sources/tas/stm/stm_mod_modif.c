/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit: STM Version 1.00                                             */
/*    Fichier: stm_mod_modif.c                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s): Gilles Augustins                                           */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

timing_model *stm_mod_Creduce (char *name, timing_model *tmodel, float slew, float ckslew, char redmode)
{
    timing_model *rcstrmod;
    timing_model *tmodel_tbl;

    if (!tmodel)
        return NULL;

    switch (tmodel->UTYPE) {
        case STM_MOD_MODNULL:
            rcstrmod = NULL;
            break;
        case STM_MOD_MODSCM:
            rcstrmod = stm_mod_create (name);
            stm_mod_update (rcstrmod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            stm_mod_update_transition(rcstrmod, tmodel->TTYPE);
            tmodel_tbl = stm_mod_duplicate (NULL, tmodel);
            stm_mod_defaultscm2tbl_delay (tmodel_tbl, -1.0, -1.0);
            rcstrmod->UMODEL.TABLE = stm_modtbl_Creduce (tmodel_tbl->UMODEL.TABLE, slew, ckslew, redmode);
            rcstrmod->UTYPE = STM_MOD_MODTBL;
            stm_mod_destroy (tmodel_tbl);
            break;
        case STM_MOD_MODPLN:
            rcstrmod = stm_mod_create (name);
            stm_mod_update (rcstrmod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            stm_mod_update_transition(rcstrmod, tmodel->TTYPE);
            tmodel_tbl = stm_mod_duplicate (NULL, tmodel);
            stm_mod_defaultpln2tbl_delay (tmodel_tbl);
            rcstrmod->UMODEL.TABLE = stm_modtbl_Creduce (tmodel_tbl->UMODEL.TABLE, slew, ckslew, redmode);
            rcstrmod->UTYPE = STM_MOD_MODTBL;
            stm_mod_destroy (tmodel_tbl);
            break;
        case STM_MOD_MODTBL:
            rcstrmod = stm_mod_create (name);
            stm_mod_update (rcstrmod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            stm_mod_update_transition(rcstrmod, tmodel->TTYPE);
            rcstrmod->UMODEL.TABLE = stm_modtbl_Creduce (tmodel->UMODEL.TABLE, slew, ckslew, redmode);
            rcstrmod->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODFCT:
            fprintf (stderr, " - warning - stm_mod_Creduce: STM_MOD_MODFCT not implemented\n");
            break;
    }
    return rcstrmod;
}

/****************************************************************************/

timing_model *stm_mod_reduce (char *name, timing_model *tmodel, float slew, float load, char redmode, char modeltype)
    
{
    timing_model *rdelaymod;
    timing_model *tmodel_tbl;

    if (!tmodel)
        return NULL;

    switch (tmodel->UTYPE) {
        case STM_MOD_MODNULL:
            rdelaymod = NULL;
            break;
        case STM_MOD_MODSCM:
            rdelaymod = stm_mod_create (name);
            stm_mod_update (rdelaymod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            stm_mod_update_transition(rdelaymod, tmodel->TTYPE);
            tmodel_tbl = stm_mod_duplicate (NULL, tmodel);
            if(modeltype == STM_DELAY){
                if (redmode == STM_SLEWFIX_RED) {
                    stm_mod_defaultscm2tbl_delay (tmodel_tbl, slew, -1.0);
                }else if (redmode == STM_LOADFIX_RED) {
                    stm_mod_defaultscm2tbl_delay (tmodel_tbl, -1.0, load);
                }else {
                    stm_mod_defaultscm2tbl_delay (tmodel_tbl, -1.0, -1.0);
                }
            }else{
                if (redmode == STM_SLEWFIX_RED) {
                    stm_mod_defaultscm2tbl_slew (tmodel_tbl, slew, -1.0);
                }else if (redmode == STM_LOADFIX_RED) {
                    stm_mod_defaultscm2tbl_slew (tmodel_tbl, -1.0, load);
                }else{
                    stm_mod_defaultscm2tbl_slew (tmodel_tbl, -1.0, -1.0);
                }
            }
            rdelaymod->UMODEL.TABLE = stm_modtbl_reduce (tmodel_tbl->UMODEL.TABLE, slew, load, redmode);
            rdelaymod->UTYPE = STM_MOD_MODTBL;
            stm_mod_destroy (tmodel_tbl);
            break;
        case STM_MOD_MODPLN:
            rdelaymod = stm_mod_create (name);
            stm_mod_update (rdelaymod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            stm_mod_update_transition(rdelaymod, tmodel->TTYPE);
            tmodel_tbl = stm_mod_duplicate (NULL, tmodel);
            if(modeltype == STM_DELAY)
                stm_mod_defaultpln2tbl_delay (tmodel_tbl);
            else
                stm_mod_defaultpln2tbl_slew (tmodel_tbl);
            rdelaymod->UMODEL.TABLE = stm_modtbl_reduce (tmodel_tbl->UMODEL.TABLE, slew, load, redmode);
            rdelaymod->UTYPE = STM_MOD_MODTBL;
            stm_mod_destroy (tmodel_tbl);
            break;
        case STM_MOD_MODTBL:
            rdelaymod = stm_mod_create (name);
            stm_mod_update (rdelaymod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            stm_mod_update_transition(rdelaymod, tmodel->TTYPE);
            rdelaymod->UMODEL.TABLE = stm_modtbl_reduce (tmodel->UMODEL.TABLE, slew, load, redmode);
            rdelaymod->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODFCT:
            rdelaymod = stm_mod_create (name);
            stm_mod_update (rdelaymod, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VF);
            tmodel_tbl = stm_mod_duplicate (NULL, tmodel);
            stm_mod_defaultfct2tbl_delay (tmodel_tbl, ~STM_NOTYPE, ~STM_NOTYPE);
            rdelaymod->UMODEL.TABLE = stm_modtbl_reduce (tmodel_tbl->UMODEL.TABLE, slew, load, redmode);
            rdelaymod->UTYPE = STM_MOD_MODTBL;
            stm_mod_destroy (tmodel_tbl);

//            fprintf (stderr, " - warning - stm_mod_reduce: STM_MOD_MODFCT not implemented\n");
            break;
    }
    return rdelaymod;
}

/****************************************************************************/

void stm_mod_shift (timing_model *model, float load)
{
    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODTBL:
                stm_modtbl_shift (model->UMODEL.TABLE, load);
                break;
            case STM_MOD_MODSCM:
                avt_errmsg (STM_ERRMSG,"031", AVT_ERROR);
                break;
            case STM_MOD_MODPLN:
                avt_errmsg (STM_ERRMSG,"023", AVT_ERROR);
                break;
            case STM_MOD_MODFCT:
                fprintf (stderr, " - warning - stm_mod_shift: STM_MOD_MODFCT not implemented\n");
                break;
        }
}

/****************************************************************************/

void stm_mod_shrinkslewaxis (timing_model *model, double thmin, double thmax, int type)
{
    return; /* edge normalisation */
    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODTBL:
                stm_modtbl_shrinkslewaxis (model->UMODEL.TABLE, thmin, thmax, type);
                break;
            case STM_MOD_MODSCM:
                break;
            case STM_MOD_MODPLN:
                break;
            case STM_MOD_MODFCT:
                break;
        }
}

/****************************************************************************/

void stm_mod_shrinkslewaxis_with_rate (timing_model *model, double rate)
{
    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODTBL:
                stm_modtbl_shrinkslewaxis_with_rate (model->UMODEL.TABLE, rate);
                break;
            case STM_MOD_MODSCM:
                break;
            case STM_MOD_MODPLN:
                break;
            case STM_MOD_MODFCT:
                break;
        }
}

/****************************************************************************/

void stm_mod_shrinkslewdata (timing_model *model, double thmin, double thmax, int type)
{
    return; /* edge normalisation */
    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODTBL:
                stm_modtbl_shrinkslewdata (model->UMODEL.TABLE, thmin, thmax, type);
                break;
            case STM_MOD_MODSCM:
                break;
            case STM_MOD_MODPLN:
                break;
            case STM_MOD_MODFCT:
                break;
        }
}

/****************************************************************************/

void stm_mod_shrinkslewdata_with_rate (timing_model *model, double rate)
{
    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODTBL:
                stm_modtbl_shrinkslewdata_with_rate (model->UMODEL.TABLE, rate);
                break;
            case STM_MOD_MODSCM:
                break;
            case STM_MOD_MODPLN:
                break;
            case STM_MOD_MODFCT:
                break;
        }
}

/****************************************************************************/

timing_model *stm_mod_neg (timing_model *model)
{
    timing_model *n_model = NULL;
    
    if (!model)
        return NULL;

    switch (model->UTYPE) {
        case STM_MOD_MODTBL:
            n_model = stm_mod_create (NULL);
            n_model->UMODEL.TABLE = stm_modtbl_neg (model->UMODEL.TABLE);
            n_model->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODSCM:
            n_model = NULL;
            break;
        case STM_MOD_MODPLN:
            n_model = NULL;
            break;
        case STM_MOD_MODFCT:
            n_model = NULL;
            break;
    }

    return n_model;
}
