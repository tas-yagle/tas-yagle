/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod.c                                                   */
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

timing_model *stm_mod_duplicate (char *dupname, timing_model *model)
{
    timing_model *dupmodel;
    ptype_list *ptype;
    stm_carac_values *cvalues;

    if (!model)
        return NULL;

    dupmodel = stm_mod_create (dupname);
    stm_mod_update (dupmodel, model->VTH, model->VDD, model->VT, model->VF);
    stm_mod_update_transition (dupmodel, model->TTYPE);

    switch (model->UTYPE) {
        case STM_MOD_MODTBL:
            dupmodel->UMODEL.TABLE = stm_modtbl_duplicate (model->UMODEL.TABLE);
            dupmodel->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODSCM:
            dupmodel->UMODEL.SCM = stm_modscm_duplicate (model->UMODEL.SCM);
            dupmodel->UTYPE = STM_MOD_MODSCM;
            break;
        case STM_MOD_MODPLN:
            dupmodel->UMODEL.POLYNOM = stm_modpln_duplicate (model->UMODEL.POLYNOM);
            dupmodel->UTYPE = STM_MOD_MODPLN;
            break;
        case STM_MOD_MODFCT:
            dupmodel->UMODEL.FUNCTION = stm_modfct_duplicate (model->UMODEL.FUNCTION);
            dupmodel->UTYPE = STM_MOD_MODFCT;
            break;
    }

    stm_noise_duplicate( model, dupmodel );
    stm_energy_duplicate( model, dupmodel );

    if ((ptype = getptype (model->USER, STM_CARAC_VALUES))) {
        cvalues = (stm_carac_values*)mbkalloc (sizeof (struct stm_carac_values));
        
        cvalues->SLEW_MIN = ((stm_carac_values*)ptype->DATA)->SLEW_MIN;
        cvalues->SLEW_MAX = ((stm_carac_values*)ptype->DATA)->SLEW_MAX;
        cvalues->SLEWOUT_MIN = ((stm_carac_values*)ptype->DATA)->SLEWOUT_MIN;
        cvalues->SLEWOUT_MAX = ((stm_carac_values*)ptype->DATA)->SLEWOUT_MAX;
        cvalues->LOAD_MIN = ((stm_carac_values*)ptype->DATA)->LOAD_MIN;
        cvalues->LOAD_MAX = ((stm_carac_values*)ptype->DATA)->LOAD_MAX;
        
        dupmodel->USER = addptype (dupmodel->USER, STM_CARAC_VALUES, cvalues);
    }
    
    return dupmodel;
}

/****************************************************************************/

timing_model *stm_mod_create (char *modelname)
{
    timing_model *model;
    
    model = (timing_model*)mbkalloc (sizeof (struct timing_model));
    
    if (modelname)
        model->NAME = namealloc (modelname);
    else
        model->NAME = NULL;

    model->UTYPE = STM_MOD_MODNULL;
    model->USER = NULL;
    model->VTH = V_FLOAT_TAB[__SIM_VTH].VALUE * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    model->VDD = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    model->VT = STM_DEFAULT_VT;
    model->VF = model->VDD;
    model->TTYPE = STM_LH;
    model->POS_IN_LIST = NULL;
//    model->VF = STM_UNKNOWN;

    stm_noise_init( model );
    stm_energy_init( model );

    return model;
}

/****************************************************************************/

void stm_mod_update (timing_model *model, float vth, float vdd, float vt, float vf)
{
    if(!model)
        return;
    if (vth != STM_NOVALUE)
        model->VTH = vth; 
    if (vdd != STM_NOVALUE)
        model->VDD = vdd;
    if (vt != STM_NOVALUE)
        model->VT = vt;
    if (vf != STM_NOVALUE)
        model->VF = vf;
}

/****************************************************************************/

void stm_mod_update_transition (timing_model *model, char transition)
{
    if(!model)
        return;
    model->TTYPE = transition; 
}

/****************************************************************************/

void stm_mod_destroy (timing_model *model)
{
    ptype_list *ptype;


    if (model) {
        if (STM_CACHE != NULL) 
            mbk_cache_release (STM_CACHE, NULL, model);
        
        else {
            switch (model->UTYPE) {
                case STM_MOD_MODTBL:
                    stm_modtbl_destroy (model->UMODEL.TABLE);
                    break;
                case STM_MOD_MODSCM:
                    stm_modscm_destroy (model->UMODEL.SCM);
                    break;
                case STM_MOD_MODPLN:
                    stm_modpln_destroy (model->UMODEL.POLYNOM);
                    break;
                case STM_MOD_MODFCT:
                    stm_modfct_destroy (model->UMODEL.FUNCTION);
                    break;
            }
            if ((ptype = getptype (model->USER, STM_CARAC_VALUES))) {
                mbkfree ((stm_carac_values*)ptype->DATA);
                model->USER = delptype (model->USER, STM_CARAC_VALUES);
            }
            stm_noise_destroy( model );
            stm_energy_destroy ( model );
        }
        freeptype(model->USER);
        mbkfree (model);
    }
}


void stm_mod_update_dynamic_info (timing_model *model, char *newinsname)
{
  char temp[1024];
  if (model) {
    switch (model->UTYPE) {
    case STM_MOD_MODTBL:
      break;
    case STM_MOD_MODSCM:
      break;
    case STM_MOD_MODPLN:
      break;
    case STM_MOD_MODFCT:
      if (strcmp(model->UMODEL.FUNCTION->INS,"?")==0)
        model->UMODEL.FUNCTION->INS=namealloc(newinsname);
      else
        {
          sprintf(temp,"%s.%s",newinsname, model->UMODEL.FUNCTION->INS);
          model->UMODEL.FUNCTION->INS=namealloc(temp);
        }
      break;
    }
  }
}

/****************************************************************************/

timing_model *stm_mod_create_fcst (char *modelname, float value, long modeltype)
{
    timing_model *model = stm_mod_create (modelname);

    switch (modeltype) {
        case STM_MOD_MODTBL:
            model->UMODEL.TABLE = stm_modtbl_create_fcst (value);
            model->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODSCM:
            /* TODO */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODPLN:
            /* TODO */
            model->UTYPE = STM_MOD_MODNULL;
        case STM_MOD_MODFCT:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
    }

    return model;
}

/****************************************************************************/

timing_model *stm_mod_create_fequac (char *modelname, float sdt, float sck, float fdt, float fck, float t, long modeltype)
{
    timing_model *model = stm_mod_create (modelname);
    switch (modeltype) {
        case STM_MOD_MODTBL:
            model->UMODEL.TABLE = stm_modtbl_create_fequac (sdt, sck, fdt, fck, t);
            model->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODSCM:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODPLN:
            /* TODO */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODFCT:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
    }

    return model;
}

/****************************************************************************/

timing_model *stm_mod_create_fequa (char *modelname, float r, float s, float c, float f, float t, long modeltype)
{
    timing_model *model = stm_mod_create (modelname);
    switch (modeltype) {
        case STM_MOD_MODTBL:
            model->UMODEL.TABLE = stm_modtbl_create_fequa (r, s, c, f, t);
            model->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODSCM:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODFCT:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODPLN:
            model->UMODEL.POLYNOM = stm_modpln_create_fequa (r, s, c, f, t);
            model->UTYPE = STM_MOD_MODPLN;
            break;
    }

    return model;
}

/****************************************************************************/

timing_model *stm_mod_create_table (char *name, int nx, int ny, char xtype, char ytype)
{
    timing_model *model = stm_mod_create (name);
    model->UMODEL.TABLE = stm_modtbl_create (nx, ny, xtype, ytype);
    model->UTYPE = STM_MOD_MODTBL;

    return model;
}
/****************************************************************************/

timing_model *stm_mod_create_ftable (char *name, int nx, int ny, char xtype, char ytype, long modeltype)
{
    timing_model *model = stm_mod_create (name);
    switch (modeltype) {
        case STM_MOD_MODTBL:
            model->UMODEL.TABLE = stm_modtbl_create (nx, ny, xtype, ytype);
            model->UTYPE = STM_MOD_MODTBL;
            break;
        case STM_MOD_MODSCM:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODPLN:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
        case STM_MOD_MODFCT:
            /* no meaning */
            model->UTYPE = STM_MOD_MODNULL;
            break;
    }

    return model;
}
/****************************************************************************/

long stm_get_last_index(char *cellname)
{
    timing_cell* cell = NULL;
    timing_model* model = NULL;
    chain_list *chain;
    long i, counter = 0;
    if(!cellname)
        return 0;
    cell = stm_getcell(cellname);
    if(cell){
        for(chain = cell->MODEL_LIST; chain; chain = chain->NEXT){
            model = (timing_model*)chain->DATA;
            if(model->NAME){
                i = atol(model->NAME + 1);
                if(i >= counter)
                    counter = i;
            }
        }
        return (counter + 1);
    }else{
        return 0;
    }
}
