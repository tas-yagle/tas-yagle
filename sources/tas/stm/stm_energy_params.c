/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_params.c                                         */
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

void    stm_energy_params_destroy (energyparams *params) 
{
    if (!params)
        return;

    if (params->EP)
        mbkfree (params->EP);

    mbkfree (params);
}

energyparams   *stm_energy_params_duplicate (energyparams *params)
{
    energyparams   *dupparams;

    dupparams = stm_energy_params_create (params->EP);
    return dupparams;
}

energyparams   *stm_energy_params_create (float *params)
{
    energyparams   *eparams = (energyparams*)mbkalloc (sizeof (struct energyparams));

    eparams->EP = (float*)mbkalloc (STM_NB_ENERGY_PARAMS*sizeof (float));
    
    eparams->EP[STM_EPCONF0] = params[STM_EPCONF0];
    eparams->EP[STM_EPCONF1] = params[STM_EPCONF1];
    
    return eparams;
}

float   stm_energy_params_eval (energyparams *eparams, float load, float slew, float vdd)
{
    float   total_load;
    float   pconf0;
    float   pconf1;
    
    if (!eparams) {
        return 0;
    }

    pconf0 = eparams->EP[STM_EPCONF0];
    pconf1 = eparams->EP[STM_EPCONF1];
   
    total_load  = (pconf0 + (pconf1*slew) + load);
    return (float)(total_load*vdd*vdd/2.0*1e-15);
}

timing_model    *stm_energy_params2tbl (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0)
{
    ptype_list          *ptype;
    stm_carac_values    *cvalues = NULL;
    double               loc_slews[1024];
    double               loc_loads[1024];
    long                 loc_ns = 0;
    long                 loc_nl = 0;
    int                  i;

    if (!model)
        return NULL;

    if (model->ENERGYTYPE != STM_ENERGY_PARAMS)
        return NULL;

    
    if ((ptype = getptype (model->USER, STM_CARAC_VALUES)))
        cvalues = (stm_carac_values*)ptype->DATA;

    if (!slews && !ns) {
        if (cvalues)
            loc_ns = stm_genslewaxis (loc_slews, cvalues->SLEW_MIN, cvalues->SLEW_MAX);
    } else {
        loc_ns = ns;
        for (i = 0; i < loc_ns; i++)
            loc_slews[i] = slews[i];
    }

    if (!loads && !nl) {
        if (cvalues)
            loc_nl = stm_genloadaxis (loc_loads, cvalues->LOAD_MIN, cvalues->LOAD_MAX);
    } else {
        loc_nl = nl;
        for (i = 0; i < loc_nl; i++)
            loc_loads[i] = loads[i];
    }

    if (!loc_ns && !loc_nl)
      return NULL;
    else
    {
        stm_energy_table_morph2_energy_fparams (model, loc_slews, loc_ns, loc_loads, loc_nl, STM_DEF_SLEW, STM_DEF_LOAD, ci0); 
        return model;
    }

    
}
