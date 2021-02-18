/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_table.c                                          */
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

void    stm_energy_table_destroy (timing_table *table)
{
    stm_modtbl_destroy (table);
}

timing_table    *stm_energy_table_duplicate (timing_table *table)
{
    return (stm_modtbl_duplicate (table));
}

float   stm_energy_table_eval (timing_table *etable, float load, float slew)
{
    float energy;

    if (!etable) {
        return 0;
    }
    
    if (etable->SET2D) {
        if (etable->XTYPE == STM_LOAD && etable->YTYPE == STM_INPUT_SLEW)
            energy = stm_modtbl_interpol2Dbilinear (etable, load, slew);
        if (etable->XTYPE == STM_INPUT_SLEW && etable->YTYPE == STM_LOAD)
            energy = stm_modtbl_interpol2Dbilinear (etable, slew, load);
    }
    else
    if (etable->SET1D) {
        if (etable->XTYPE == STM_LOAD)
            energy = stm_modtbl_interpol1Dlinear (etable, load);
        if (etable->XTYPE == STM_INPUT_SLEW)
            energy = stm_modtbl_interpol1Dlinear (etable, slew);
    }
    if (!stm_modtbl_isnull (etable->CST))
        energy = etable->CST;

    return energy;
}


void    stm_energy_table_morph2_energy_fparams (timing_model *model, double *s_axis, int ns, double *l_axis, int nl, double slew, double load, double ci0)
{
    if (model) {
        if (ns && nl)
            stm_energy_table_morph2_energy2D_fparams (model, s_axis, ns, l_axis, nl, ci0);
        else if (!ns && nl)
            stm_energy_table_morph2_energy1Dslewfix_fparams (model, l_axis, nl, slew, ci0);
        else if (ns && !nl)
            stm_energy_table_morph2_energy1Dloadfix_fparams (model, s_axis, ns, load + ci0);
    }
}

void    stm_energy_table_morph2_energy2D_fparams (timing_model *model, double *s_axis, int ns, double *l_axis, int nl, double ci0)
{
    timing_table *tbl;

    tbl = stm_energy_table_create_energy2D_fparams (model->ENERGYMODEL.EPARAMS, s_axis, ns, l_axis, nl, ci0, model->VDD);
    if (tbl) {
        stm_energy_destroy (model);
        model->ENERGYTYPE = STM_ENERGY_TABLE;
        model->ENERGYMODEL.ETABLE = tbl;
    }
}

void    stm_energy_table_morph2_energy1Dslewfix_fparams (timing_model *model, double *l_axis, int nl, double slew, double ci0) 
{
  timing_table  *tbl;

  tbl   = stm_energy_table_create_energy1Dslewfix_fparams (model->ENERGYMODEL.EPARAMS, l_axis, nl, slew, ci0, model->VDD);
  if (tbl)
  {
    stm_energy_destroy (model);
    model->ENERGYTYPE         = STM_ENERGY_TABLE;
    model->ENERGYMODEL.ETABLE = tbl;
  }
}

void    stm_energy_table_morph2_energy1Dloadfix_fparams (timing_model *model, double *s_axis, int ns, double load)
{
  timing_table  *tbl;

  tbl   = stm_energy_table_create_energy1Dloadfix_fparams (model->ENERGYMODEL.EPARAMS, s_axis, ns, load, model->VDD);
  if (tbl)
  {
    stm_energy_destroy (model);
    model->ENERGYTYPE         = STM_ENERGY_TABLE;
    model->ENERGYMODEL.ETABLE = tbl;
  }
    

}

timing_table    *stm_energy_table_create_energy2D_fparams (energyparams *eparams, double *s_axis, int ns, double *l_axis, int nl, double ci0, float vdd)
{
    int x, y;
    timing_table *tbl;
    
    tbl = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);

    /* axis */
    if (nl)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);
    if (ns)
        for (y = 0; y < nl; y++)
            stm_modtbl_setYrangeval (tbl, y, l_axis[y]);

    /* values */
    if (nl && ns)
        for (x = 0; x < ns; x++)
            for (y = 0; y < nl; y++)
                stm_modtbl_set2Dval (tbl, x, y, stm_energy_params_eval (eparams, l_axis[y] + ci0, s_axis[x], vdd));

    return tbl;
}


timing_table    *stm_energy_table_create_energy1Dslewfix_fparams (energyparams *eparams, double *l_axis, int nl, double slew, double ci0, float vdd)
{
    int x;
    timing_table *tbl;

    tbl = stm_modtbl_create (nl, 0, STM_LOAD, STM_NOTYPE);

    /* axis */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_setXrangeval (tbl, x, l_axis[x]);

    /* values */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_set1Dval (tbl, x, stm_energy_params_eval (eparams, l_axis[x] + ci0, slew, vdd));

    return tbl;
}

timing_table    *stm_energy_table_create_energy1Dloadfix_fparams (energyparams *eparams, double *s_axis, int ns, double load, float vdd)
{
    int x;
    timing_table *tbl;
    
    tbl = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);

    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);

    /* values */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_set1Dval (tbl, x, stm_energy_params_eval (eparams, load, s_axis[x], vdd));

    return tbl;
}
