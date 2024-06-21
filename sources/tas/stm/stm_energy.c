/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy.c                                                */
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

void    stm_energy_init (timing_model *model)
{
    model->ENERGYTYPE = STM_ENERGY_NULL;
}

void    stm_energy_destroy (timing_model *model)
{
    switch (model->ENERGYTYPE) {
        case STM_ENERGY_TABLE:
            stm_energy_table_destroy (model->ENERGYMODEL.ETABLE);
            break;

        case STM_ENERGY_PARAMS:
            stm_energy_params_destroy (model->ENERGYMODEL.EPARAMS);
            break;
    }

    model->ENERGYTYPE = STM_ENERGY_NULL;
}

void    stm_energy_duplicate (timing_model *src_model, timing_model *dst_model)
{
    switch( src_model->ENERGYTYPE ) {
        case STM_ENERGY_TABLE:
            dst_model->ENERGYTYPE         = STM_ENERGY_TABLE;
            dst_model->ENERGYMODEL.ETABLE = stm_energy_table_duplicate( src_model->ENERGYMODEL.ETABLE );
            break;
        case STM_ENERGY_PARAMS:
            dst_model->ENERGYTYPE          = STM_ENERGY_PARAMS;
            dst_model->ENERGYMODEL.EPARAMS = stm_energy_params_duplicate( src_model->ENERGYMODEL.EPARAMS );
            break;
        default:
            dst_model->ENERGYTYPE          = STM_ENERGY_NULL;
    }
}



float   stm_energy_eval (timing_model *model, float load, float slew)
{
    if (model) {
        switch (model->ENERGYTYPE) {
            case STM_ENERGY_PARAMS :
                return stm_energy_params_eval (model->ENERGYMODEL.EPARAMS, load, slew, model->VDD);
                break;
            case STM_ENERGY_TABLE : 
                return stm_energy_table_eval (model->ENERGYMODEL.ETABLE, load, slew);
                break;
        }
    }
    return 0;
}


timing_model    *stm_energy_energy2tbl (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0)
{
    switch (model->ENERGYTYPE) {
        case STM_ENERGY_PARAMS  :
            return stm_energy_params2tbl (model, slews, ns, loads, nl, ci0);
        case STM_ENERGY_TABLE   :
            return model;
        default :
            model->ENERGYMODEL.ETABLE = NULL;
            return model;
    }
}


void    stm_energy_defaultenergy2tbl (timing_model *model, float slew, float load)
{
    float *s_axis;
    float *l_axis;
    float vf, imax, vt;
    int ns, nl;

    vf = stm_mod_vf (model);
    vt = stm_mod_vt (model);
    if (vf < vt)
        vf = stm_mod_vdd(model) - vt;
    imax = stm_mod_imax (model);

    
    if(slew < 0.0 && STM_AXIS_NSLOPE){
        s_axis = STM_AXIS_SLOPEIN;
        ns = STM_AXIS_NSLOPE;
    }else{
        s_axis = stm_dyna_slews (STM_DEF_AXIS_BASE, slew);
        ns = STM_DEF_AXIS_BASE * 2 + 1;
    }
    if(load < 0.0 && STM_AXIS_NCAPA){
        l_axis = STM_AXIS_CAPAOUT;
        nl = STM_AXIS_NCAPA;
    }else{
        l_axis = stm_dyna_loads (STM_DEF_AXIS_BASE, vf / imax, load);
        nl = STM_DEF_AXIS_BASE * 2 + 1;
    }

    if(getptype (model->USER, STM_CARAC_VALUES))
        stm_energy_params2tbl (model, NULL, 0, NULL, 0, 0.0);
    else
        stm_energy_params2tbl (model, s_axis, ns, l_axis, nl, 0.0);

    if(!STM_AXIS_NSLOPE)
        mbkfree (s_axis);
    if(!STM_AXIS_NCAPA)
        mbkfree (l_axis);
}
