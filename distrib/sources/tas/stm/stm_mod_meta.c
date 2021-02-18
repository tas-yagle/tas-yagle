/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit: STM Version 1.00                                             */
/*    Fichier: stm_mod_meta.c                                               */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s): Gilles Augustins                                           */
/*               Antony Pinto                                               */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

float stm_round (float d, int precision)
{
    long   l;
    int    i = 0;
    float dp;
    int    cs;

    dp = d < 0 ? -d : d;

    if (dp == 0.0)
        return 0.0;

    cs = (int)pow ((float)10, (float)precision);
    if (dp >= cs) {
        l = (long)dp;
        while (l / cs) {
            l /= 10;
            i++;
        }
        while (i--)
            l *= 10;
        if (d < 0)
            return -(float)l;
        else
            return (float)l;
    } else {
        cs /= 10;
        while (dp < cs) {
            dp *= 10;
            i++;
        }
        l = (long)dp;
        if (l == LONG_MAX)
            avt_errmsg (STM_ERRMSG, "050", AVT_ERROR);
        dp = (float)l;
        while (i--)
            dp /= 10;
        if (d < 0)
            return -dp;
        else
            return dp;
    }
}

/****************************************************************************/

float *stm_dyna_slews (int n, float slew)
{
    int i ;
    float *slopes;
    
    if(slew < 0.0)
        slew = STM_DEF_SLEW;

    slopes = (float*)mbkalloc ((2 * n + 1) * sizeof (float));
    for (i = 0 ; i < 2 * n + 1 ; i++)
        slopes[i] = stm_round (slew / pow ((float)((pow (n - 5, 2) + 12) / 10), (float)(n - i)), 2);

    return slopes;
}
/****************************************************************************/

float *stm_dyna_slewsforpln (int n, float thmin, float thmax)
{
    int i ;
    float step;
    float *slopes;

    slopes = (float*)mbkalloc ((2 * n + 1) * sizeof (float));
    step = (thmax - thmin) / (2 * n);
    for (i = 0 ; i < 2 * n + 1 ; i++)
        slopes[i] = stm_round (thmin + (i * step), 2);

    return slopes;
}

/****************************************************************************/

float *stm_dyna_loads (int n, float avg_r, float load)
{
    int i ;
    float *dyna_slews;
    float *loads ;

    
    dyna_slews = stm_dyna_slews (n, -1.0);

    loads = (float*)mbkalloc ((2 * n + 1) * sizeof (float)) ;
    for (i = 0 ; i < 2 * n + 1 ; i++){
        if(load < 0.0)
         loads[i] = stm_round (stm_thr2scm (dyna_slews[i], STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR, STM_DEFAULT_VTN, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_UP) / avg_r, 2) * 1000;
        else
         loads[i] = stm_round (load / pow ((float)((pow (n - 5, 2) + 12) / 10), (float)(n - i)), 2);
    }

    mbkfree (dyna_slews);

    return loads;
}
/****************************************************************************/

float *stm_dyna_loadsforpln (int n, float thmin, float thmax)
{
    int i ;
    float step;
    float *loads ;


    loads = (float*)mbkalloc ((2 * n + 1) * sizeof (float)) ;
    step = (thmax - thmin) / (2 * n);
    for (i = 0 ; i < 2 * n + 1 ; i++)
        loads[i] = stm_round (thmin + (i * step), 2);


    return loads;
}


/****************************************************************************/

float stm_mod_getcaracslewmin (timing_model *model)
{
    ptype_list *ptype;

    if (!model)
        return STM_NOVALUE;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES)))
        return STM_NOVALUE;
    else
        return ((stm_carac_values*)ptype->DATA)->SLEW_MIN;
}

/****************************************************************************/

float stm_mod_getcaracslewoutmin (timing_model *model)
{
    ptype_list *ptype;

    if (!model)
        return STM_NOVALUE;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES)))
        return STM_NOVALUE;
    else
        return ((stm_carac_values*)ptype->DATA)->SLEWOUT_MIN;
}

/****************************************************************************/

float stm_mod_getcaracloadmin (timing_model *model)
{
    ptype_list *ptype;

    if (!model)
        return STM_NOVALUE;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES)))
        return STM_NOVALUE;
    else
        return ((stm_carac_values*)ptype->DATA)->LOAD_MIN;
}

/****************************************************************************/

float stm_mod_getcaracslewmax (timing_model *model)
{
    ptype_list *ptype;

    if (!model)
        return STM_NOVALUE;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES)))
        return STM_NOVALUE;
    else
        return ((stm_carac_values*)ptype->DATA)->SLEW_MAX;
}

/****************************************************************************/

float stm_mod_getcaracslewoutmax (timing_model *model)
{
    ptype_list *ptype;

    if (!model)
        return STM_NOVALUE;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES)))
        return STM_NOVALUE;
    else
        return ((stm_carac_values*)ptype->DATA)->SLEWOUT_MAX;
}

/****************************************************************************/

float stm_mod_getcaracloadmax (timing_model *model)
{
    ptype_list *ptype;

    if (!model)
        return STM_NOVALUE;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES)))
        return STM_NOVALUE;
    else
        return ((stm_carac_values*)ptype->DATA)->LOAD_MAX;
}

/****************************************************************************/

void stm_mod_setcaracslew (timing_model *model, float slew)
{
    ptype_list *ptype;
    stm_carac_values *cvalues;

    if (!model)
        return;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES))) {
        cvalues = (stm_carac_values*)mbkalloc (sizeof (struct stm_carac_values));
        cvalues->LOAD_MIN = FLT_MAX;
        cvalues->LOAD_MAX = FLT_MIN;
        cvalues->SLEW_MIN = FLT_MAX;
        cvalues->SLEW_MAX = FLT_MIN;
        cvalues->SLEWOUT_MIN = FLT_MAX;
        cvalues->SLEWOUT_MAX = FLT_MIN;
        model->USER = addptype (model->USER, STM_CARAC_VALUES, cvalues);
    } else
        cvalues = (stm_carac_values*)ptype->DATA;

    cvalues->SLEW_MIN = cvalues->SLEW_MIN < slew ? cvalues->SLEW_MIN : slew;
    cvalues->SLEW_MAX = cvalues->SLEW_MAX > slew ? cvalues->SLEW_MAX : slew;
}

/****************************************************************************/

void stm_mod_setcaracslewout (timing_model *model, float slew)
{
    ptype_list *ptype;
    stm_carac_values *cvalues;

    if (!model)
        return;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES))) {
        cvalues = (stm_carac_values*)mbkalloc (sizeof (struct stm_carac_values));
        cvalues->LOAD_MIN = FLT_MAX;
        cvalues->LOAD_MAX = FLT_MIN;
        cvalues->SLEW_MIN = FLT_MAX;
        cvalues->SLEW_MAX = FLT_MIN;
        cvalues->SLEWOUT_MIN = FLT_MAX;
        cvalues->SLEWOUT_MAX = FLT_MIN;
        model->USER = addptype (model->USER, STM_CARAC_VALUES, cvalues);
    } else
        cvalues = (stm_carac_values*)ptype->DATA;

    cvalues->SLEWOUT_MIN = cvalues->SLEWOUT_MIN < slew ? cvalues->SLEWOUT_MIN : slew;
    cvalues->SLEWOUT_MAX = cvalues->SLEWOUT_MAX > slew ? cvalues->SLEWOUT_MAX : slew;
}

/****************************************************************************/

void stm_mod_setcaracload (timing_model *model, float load)
{
    ptype_list *ptype;
    stm_carac_values *cvalues;

    if (!model)
        return;

    if (!(ptype = getptype (model->USER, STM_CARAC_VALUES))) {
        cvalues = (stm_carac_values*)mbkalloc (sizeof (struct stm_carac_values));
        cvalues->LOAD_MIN = FLT_MAX;
        cvalues->LOAD_MAX = FLT_MIN;
        cvalues->SLEW_MIN = FLT_MAX;
        cvalues->SLEW_MAX = FLT_MIN;
        cvalues->SLEWOUT_MIN = FLT_MAX;
        cvalues->SLEWOUT_MAX = FLT_MIN;
        model->USER = addptype (model->USER, STM_CARAC_VALUES, cvalues);
    } else
        cvalues = (stm_carac_values*)ptype->DATA;

    cvalues->LOAD_MIN = cvalues->LOAD_MIN < load ? cvalues->LOAD_MIN : load;
    cvalues->LOAD_MAX = cvalues->LOAD_MAX > load ? cvalues->LOAD_MAX : load;
}

/****************************************************************************/

long stm_genloadaxis (double *loads, float load_min, float load_max)
{
    int i, n = 4;
    float step = (load_max - load_min) / (pow(2, n) - 1);

    loads[0] = load_min;
    for (i = 0; i < n; i++)
        loads[i + 1] = loads[i] + step * pow(2, i);

    return (n + 1);
}

/****************************************************************************/

long stm_genslewaxis (double *slews, float slew_min, float slew_max)
{
    int i, n = 4;
    float step = (slew_max - slew_min) / (pow(2, n) - 1);

    slews[0] = slew_min;
    for (i = 0; i < n; i++)
        slews[i + 1] = slews[i] + step * pow(2, i);

    return (n + 1);
}

/****************************************************************************/
/*{{{                    Generals                                           */
/****************************************************************************/
/*{{{                    stm_mod_stm2tbl_delay()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_stm2tbl_delay(timing_model *model, float *slews,
                                    long ns, float *loads, long nl,
                                    float ci0)
{
    switch (model->UTYPE)
    {
        case STM_MOD_MODSCM:
             return stm_mod_scm2tbl_delay(model,slews,ns,loads,nl,ci0,STM_DEF_SLEW,STM_DEF_LOAD);
        case STM_MOD_MODPLN:
             return stm_mod_pln2tbl_delay(model,slews,ns,loads,nl,ci0);
        case STM_MOD_MODFCT:
             return stm_mod_fct2tbl_delay(model,slews,ns,loads,nl,ci0);
        case STM_MOD_MODTBL:
             return model;
        default :
             model->UMODEL.TABLE = NULL;
             return model;
    }
}

/*}}}************************************************************************/
/*{{{                    stm_mod_stm2tbl_slew()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_stm2tbl_slew(timing_model *model, float *slews,
                                   long ns, float *loads, long nl, float ci0)
{
    switch (model->UTYPE)
    {
        case STM_MOD_MODSCM:
             return stm_mod_scm2tbl_slew(model,slews,ns,loads,nl,ci0,STM_DEF_SLEW,STM_DEF_LOAD);
        case STM_MOD_MODPLN:
             return stm_mod_pln2tbl_slew(model,slews,ns,loads,nl,ci0);
        case STM_MOD_MODFCT:
             return stm_mod_fct2tbl_slew(model,slews,ns,loads,nl,ci0);
        case STM_MOD_MODTBL:
             return model;
        default :
             model->UMODEL.TABLE = NULL;
             return NULL;
    }
}

/*}}}************************************************************************/
/*{{{                    stm_mod_defaultstm2tbl_delay()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_mod_defaultstm2tbl_delay(timing_model *model, char xtype, char ytype, float slew, float load)
{
    switch (model->UTYPE)
    {
        case STM_MOD_MODSCM :
             stm_mod_defaultscm2tbl_delay(model, slew, load);
             break;
        case STM_MOD_MODPLN :
             stm_mod_defaultpln2tbl_delay(model);
             break;
        case STM_MOD_MODFCT :
             stm_mod_defaultfct2tbl_delay(model,xtype,ytype);
             break;
        case STM_MOD_MODTBL :
             break;
        default :
             model->UMODEL.TABLE = NULL;
    }

}

/*}}}************************************************************************/
/*{{{                    stm_mod_defaultstm2tbl_slew()                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_mod_defaultstm2tbl_slew(timing_model *model, char xtype, char ytype, float slew, float load)
{
    switch (model->UTYPE)
    {
        case STM_MOD_MODSCM:
             stm_mod_defaultscm2tbl_slew(model, slew, load);
             break;
        case STM_MOD_MODPLN:
             stm_mod_defaultpln2tbl_slew(model);
             break;
        case STM_MOD_MODFCT:
             stm_mod_defaultfct2tbl_slew(model,xtype,ytype);
             break;
        case STM_MOD_MODTBL :
             break;
        default :
             model->UMODEL.TABLE = NULL;
    }

}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    SCM                                                */
/****************************************************************************/

/****************************************************************************/

timing_model *stm_mod_scm2tbl_delay (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0, float slew, float load)
{
    ptype_list          *ptype;
    stm_carac_values    *cvalues = NULL;
    double               loc_slews[1024];
    double               loc_slews_scaled[1024];
    double               loc_loads[1024];
    long                 loc_ns = 0;
    long                 loc_nl = 0;
    timing_table        *tbl;
    int                  i;
    float                slew_scaled;

    if (!model)
        return NULL;

    if (model->UTYPE != STM_MOD_MODSCM)
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
    if(slew < 0.0 && load >= 0.0) loc_nl = 0;
    if(load < 0.0 && slew >= 0.0) loc_ns = 0;

    if (!loc_ns && !loc_nl)
      return NULL;
    else
    {
      model->NAME   = stm_dlymname_extract(model->NAME);
      if(stm_if_thresholds()){
          slew_scaled = stm_mod_shrinkslew_thr2scm(model, slew);
          stm_mod_shrinkslewaxis_thr2scm(model, loc_slews_scaled, loc_slews, loc_ns);
      }else{
          for (i = 0; i < loc_ns; i++)
             loc_slews_scaled[i] = loc_slews[i];
      }

    if (loc_ns && loc_nl)
        tbl   = stm_modtbl_create_delay2D_fstm(model,loc_slews_scaled,loc_ns,loc_loads,loc_nl,ci0);
    else if (!loc_ns && loc_nl)
        tbl   = stm_modtbl_create_delay1Dslewfix_fstm(model,loc_loads,loc_nl,slew_scaled,ci0);
    else if (loc_ns && !loc_nl)
        tbl   = stm_modtbl_create_delay1Dloadfix_fstm(model,loc_slews_scaled,loc_ns,load);
    stm_mod_destroy_model (model);
    model->UTYPE          = STM_MOD_MODTBL;
    model->UMODEL.TABLE   = tbl;

        
      if(stm_if_thresholds()){
          stm_mod_shrinkslewaxis_scm2thr (model, loc_slews);
      }

      return model;
    }
}

/****************************************************************************/

timing_model *stm_mod_scm2tbl_slew (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0, float slew, float load)
{
    ptype_list *ptype;
    stm_carac_values *cvalues = NULL;
    double loc_slews[1024];
    double loc_slews_scaled[1024];
    double loc_loads[1024];
    long  loc_ns = 0;
    long  loc_nl = 0;
    timing_table *tbl;
    int i;
    float slew_scaled;

    if (!model)
        return NULL;

    if (model->UTYPE != STM_MOD_MODSCM)
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
    if(slew < 0.0 && load >= 0.0) loc_nl = 0;
    if(load < 0.0 && slew >= 0.0) loc_ns = 0;

    if (!loc_ns && !loc_nl)
      return NULL;
    else
    {
      model->NAME   = stm_slwmname_extract(model->NAME);
      if(stm_if_thresholds()){
          slew_scaled = stm_mod_shrinkslew_thr2scm(model, slew);
          stm_mod_shrinkslewaxis_thr2scm(model, loc_slews_scaled, loc_slews, loc_ns);
      }else{
          for (i = 0; i < loc_ns; i++)
             loc_slews_scaled[i] = loc_slews[i];
      }
    if (loc_ns && loc_nl)
        tbl   = stm_modtbl_create_slew2D_fstm(model,loc_slews_scaled,loc_ns,loc_loads,loc_nl,ci0);
    else if (!loc_ns && loc_nl)
        tbl   = stm_modtbl_create_slew1Dslewfix_fstm(model,loc_loads,loc_nl,slew_scaled,ci0);
    else if (loc_ns && !loc_nl)
        tbl   = stm_modtbl_create_slew1Dloadfix_fstm(model,loc_slews_scaled,loc_ns,load);
    stm_mod_destroy_model (model);
    model->UTYPE          = STM_MOD_MODTBL;
    model->UMODEL.TABLE   = tbl;
      if(stm_if_thresholds()){
          stm_mod_shrinkslewaxis_scm2thr (model, loc_slews);
          stm_mod_shrinkslewdata_scm2thr (model);
      }

      return model;
    }
}

/****************************************************************************/

void stm_mod_defaultscm2tbl_slew (timing_model *model, float slew, float load)
{
    float *s_axis;
    float *l_axis;
    float vf, imax, vt;
    int ns, nl, frees=0, freel=0;


    if(slew < 0.0 && STM_AXIS_NSLOPE){
        s_axis = STM_AXIS_SLOPEIN;
        ns = STM_AXIS_NSLOPE;
    }else if(getptype (model->USER, STM_CARAC_VALUES)){
        s_axis = NULL;
        ns = 0;
    }else{
        s_axis = stm_dyna_slews (STM_DEF_AXIS_BASE, slew);
        ns = STM_DEF_AXIS_BASE * 2 + 1;
        frees=1;
    }
    if(load < 0.0 && STM_AXIS_NCAPA){
        l_axis = STM_AXIS_CAPAOUT;
        nl = STM_AXIS_NCAPA;
    }else if(getptype (model->USER, STM_CARAC_VALUES)){
        l_axis = NULL;
        nl = 0;
    }else{
        vf = stm_mod_vf (model);
        vt = stm_mod_vt (model);
        if (vf < vt)
            vf = stm_mod_vdd(model) - vt;
        imax = stm_mod_imax (model);
        l_axis = stm_dyna_loads (STM_DEF_AXIS_BASE, vf / imax, load);
        nl = STM_DEF_AXIS_BASE * 2 + 1;
        freel=1;
    }

    stm_mod_scm2tbl_slew (model, s_axis, ns, l_axis, nl, 0.0, slew, load);

    if(frees)
        mbkfree (s_axis);
    if(freel)
        mbkfree (l_axis);
}

/****************************************************************************/

void stm_mod_defaultscm2tbl_delay (timing_model *model, float slew, float load)
{
    float *s_axis;
    float *l_axis;
    float vf, imax, vt;
    int ns, nl, frees=0, freel=0;

    
    if(slew < 0.0 && STM_AXIS_NSLOPE){
        s_axis = STM_AXIS_SLOPEIN;
        ns = STM_AXIS_NSLOPE;
    }else if(getptype (model->USER, STM_CARAC_VALUES)){
        s_axis = NULL;
        ns = 0;
    }else{
        s_axis = stm_dyna_slews (STM_DEF_AXIS_BASE, slew);
        ns = STM_DEF_AXIS_BASE * 2 + 1;
        frees=1;
    }
    if(load < 0.0 && STM_AXIS_NCAPA){
        l_axis = STM_AXIS_CAPAOUT;
        nl = STM_AXIS_NCAPA;
    }else if(getptype (model->USER, STM_CARAC_VALUES)){
        l_axis = NULL;
        nl = 0;
    }else{
        vf = stm_mod_vf (model);
        vt = stm_mod_vt (model);
        if (vf < vt)
            vf = stm_mod_vdd(model) - vt;
        imax = stm_mod_imax (model);
        l_axis = stm_dyna_loads (STM_DEF_AXIS_BASE, vf / imax, load);
        nl = STM_DEF_AXIS_BASE * 2 + 1;
        freel=1;
    }

    stm_mod_scm2tbl_delay (model, s_axis, ns, l_axis, nl, 0.0, slew, load);

    if(frees)
        mbkfree (s_axis);
    if(freel)
        mbkfree (l_axis);
}

/*}}}************************************************************************/
/*{{{                    PLN                                                */
/****************************************************************************/
timing_model *stm_mod_pln2tbl_delay (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0)
{
    ptype_list *ptype;
    stm_carac_values *cvalues = NULL;
    double loc_slews[1024];
    double loc_loads[1024];
    long  loc_ns = 0;
    long  loc_nl = 0;
    timing_table *tbl;
    int i;

    if (!model)
        return NULL;

    if (model->UTYPE != STM_MOD_MODPLN)
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
      model->NAME   = stm_dlymname_extract(model->NAME);
    if (loc_ns && loc_nl)
        tbl   = stm_modtbl_create_delay2D_fstm(model,loc_slews,loc_ns,loc_loads,loc_nl,ci0);
    else if (!loc_ns && loc_nl)
        tbl   = stm_modtbl_create_delay1Dslewfix_fstm(model,loc_loads,loc_nl,STM_DEF_SLEW,ci0);
    else if (loc_ns && !loc_nl)
        tbl   = stm_modtbl_create_delay1Dloadfix_fstm(model,loc_slews,loc_ns,STM_DEF_LOAD);
    stm_mod_destroy_model (model);
    model->UTYPE          = STM_MOD_MODTBL;
    model->UMODEL.TABLE   = tbl;

      return model;
    }
}

/****************************************************************************/

timing_model *stm_mod_pln2tbl_slew (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0)
{
    ptype_list *ptype;
    stm_carac_values *cvalues = NULL;
    double loc_slews[1024];
    double loc_loads[1024];
    long  loc_ns = 0;
    long  loc_nl = 0;
    timing_table *tbl;
    int i;

    if (!model)
        return NULL;

    if (model->UTYPE != STM_MOD_MODPLN)
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
      model->NAME   = stm_slwmname_extract(model->NAME);
    if (loc_ns && loc_nl)
        tbl   = stm_modtbl_create_slew2D_fstm(model,loc_slews,loc_ns,loc_loads,loc_nl,ci0);
    else if (!loc_ns && loc_nl)
        tbl   = stm_modtbl_create_slew1Dslewfix_fstm(model,loc_loads,loc_nl,STM_DEF_SLEW,ci0);
    else if (loc_ns && !loc_nl)
        tbl   = stm_modtbl_create_slew1Dloadfix_fstm(model,loc_slews,loc_ns,STM_DEF_LOAD);
    stm_mod_destroy_model (model);
    model->UTYPE          = STM_MOD_MODTBL;
    model->UMODEL.TABLE   = tbl;

      return model;
    }
}

/****************************************************************************/

void stm_mod_defaultpln2tbl_slew (timing_model *model)
{
    float *s_axis = NULL;
    float *l_axis = NULL;
    float thcapamin = -1.0, thcapamax = -1.0;
    float thslewmin = -1.0, thslewmax = -1.0;
    int i, nbvar;
    long nbvarmask, ns = 0, nl = 0;
    long nbvar_degree = model->UMODEL.POLYNOM->DEG_NBVAR;

    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = nbvar_degree & nbvarmask;

    for(i = 0; i < nbvar; i++){
        if(model->UMODEL.POLYNOM->VAR[i]->TYPE == STM_INPUT_SLEW){
            thslewmin = model->UMODEL.POLYNOM->VAR[i]->THMIN;
            thslewmax = model->UMODEL.POLYNOM->VAR[i]->THMAX;
            break;
        }
    }
    for(i = 0; i < nbvar; i++){
        if(model->UMODEL.POLYNOM->VAR[i]->TYPE == STM_LOAD){
            thcapamin = model->UMODEL.POLYNOM->VAR[i]->THMIN;
            thcapamax = model->UMODEL.POLYNOM->VAR[i]->THMAX;
            break;
        }
    }

    if((thslewmin >= 0.0) && (thslewmax >= 0.0)){
        s_axis = stm_dyna_slewsforpln (STM_DEF_AXIS_BASE, thslewmin, thslewmax);
        ns = STM_DEF_AXIS_BASE * 2 + 1;
    }
    if((thcapamin >= 0.0) && (thcapamax >= 0.0)){
        l_axis = stm_dyna_loadsforpln (STM_DEF_AXIS_BASE, thcapamin, thcapamax);
        nl = STM_DEF_AXIS_BASE * 2 + 1;
    }

    stm_mod_pln2tbl_slew (model, s_axis, ns, l_axis, nl, 0.0);

    mbkfree (s_axis);
    mbkfree (l_axis);
}

/****************************************************************************/

void stm_mod_defaultpln2tbl_delay (timing_model *model)
{
    float *s_axis = NULL;
    float *l_axis = NULL;
    float thcapamin = -1.0, thcapamax = -1.0;
    float thslewmin = -1.0, thslewmax = -1.0;
    int i, nbvar;
    long nbvarmask, ns = 0, nl = 0;
    long nbvar_degree = model->UMODEL.POLYNOM->DEG_NBVAR;

    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = nbvar_degree & nbvarmask;

    for(i = 0; i < nbvar; i++){
        if(model->UMODEL.POLYNOM->VAR[i]->TYPE == STM_INPUT_SLEW){
            thslewmin = model->UMODEL.POLYNOM->VAR[i]->THMIN;
            thslewmax = model->UMODEL.POLYNOM->VAR[i]->THMAX;
            break;
        }
    }
    for(i = 0; i < nbvar; i++){
        if(model->UMODEL.POLYNOM->VAR[i]->TYPE == STM_LOAD){
            thcapamin = model->UMODEL.POLYNOM->VAR[i]->THMIN;
            thcapamax = model->UMODEL.POLYNOM->VAR[i]->THMAX;
            break;
        }
    }

    if((thslewmin >= 0.0) && (thslewmax >= 0.0)){
        s_axis = stm_dyna_slewsforpln (STM_DEF_AXIS_BASE, thslewmin, thslewmax);
        ns = STM_DEF_AXIS_BASE * 2 + 1;
    }
    if((thcapamin >= 0.0) && (thcapamax >= 0.0)){
        l_axis = stm_dyna_loadsforpln (STM_DEF_AXIS_BASE, thcapamin, thcapamax);
        nl = STM_DEF_AXIS_BASE * 2 + 1;
    }

    stm_mod_pln2tbl_delay (model, s_axis, ns, l_axis, nl, 0.0);

    mbkfree (s_axis);
    mbkfree (l_axis);
}

/*}}}************************************************************************/
/*{{{                    FCT                                                */
/****************************************************************************/
/*{{{                    stm_mod_fct2tbl_delay()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_fct2tbl_delay(timing_model *model, float *slews,
                                    long ns, float *loads, long nl, float ci0)
{
  stm_carac_values  *cvalues = NULL;
  ptype_list        *ptype;
  double             loc_slews[1024], loc_loads[1024];
  long               loc_ns = 0, loc_nl = 0;
  int                i;
  timing_table      *tbl;

  if (!model || model->UTYPE != STM_MOD_MODFCT)
    return NULL;
  else
  {
    if ((ptype = getptype(model->USER, STM_CARAC_VALUES)))
      cvalues =(stm_carac_values*)ptype->DATA;
	
    if (slews || ns)
    {
      loc_ns = ns;
      for (i = 0; i < loc_ns; i++)
        loc_slews[i] = slews[i];
    }
    else if (cvalues)
	  loc_ns = stm_genslewaxis(loc_slews, cvalues->SLEW_MIN,
							   cvalues->SLEW_MAX);
    if (loads || nl)
    {
      loc_nl = nl;
      for (i = 0; i < loc_nl; i++)
        loc_loads[i] = loads[i];
    }
    else if (cvalues)
	  loc_nl = stm_genloadaxis(loc_loads, cvalues->LOAD_MIN,
							   cvalues->LOAD_MAX);
	
    if (!loc_ns && !loc_nl)
      return NULL;
    else
    {
      model->NAME   = stm_dlymname_extract(model->NAME);
      if (loc_ns && loc_nl)
          tbl   = stm_modtbl_create_delay2D_fstm(model,loc_slews,loc_ns,loc_loads,loc_nl,ci0);
      else if (!loc_ns && loc_nl)
          tbl   = stm_modtbl_create_delay1Dslewfix_fstm(model,loc_loads,loc_nl,STM_DEF_SLEW,ci0);
      else if (loc_ns && !loc_nl)
          tbl   = stm_modtbl_create_delay1Dloadfix_fstm(model,loc_slews,loc_ns,STM_DEF_LOAD);
      stm_mod_destroy_model (model);
      model->UTYPE          = STM_MOD_MODTBL;
      model->UMODEL.TABLE   = tbl;
      return model;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    stm_mod_fct2tbl_slew()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_fct2tbl_slew(timing_model *model, float *slews, long ns, float *loads, long nl, float ci0)
{
  stm_carac_values  *cvalues = NULL;
  timing_table      *tbl;
  ptype_list        *ptype;
  double             loc_slews[1024], loc_loads[1024];
  long               loc_ns = 0, loc_nl = 0;
  int                i;

  if (!model || model->UTYPE != STM_MOD_MODPLN)
    return NULL;
  else
  {
    if ((ptype = getptype(model->USER, STM_CARAC_VALUES)))
      cvalues =(stm_carac_values*)ptype->DATA;
    if (!slews && !ns)
    {
      if (cvalues)
        loc_ns = stm_genslewaxis(loc_slews, cvalues->SLEW_MIN,
                                 cvalues->SLEW_MAX);
    }
    else
    {
      loc_ns = ns;
      for (i = 0; i < loc_ns; i++)
        loc_slews[i] = slews[i];
    }
    if (!loads && !nl)
    {
      if (cvalues)
        loc_nl = stm_genloadaxis(loc_loads, cvalues->LOAD_MIN,
                                 cvalues->LOAD_MAX);
    }
    else
    {
      loc_nl = nl;
      for (i = 0; i < loc_nl; i++)
        loc_loads[i] = loads[i];
    }
    if (!loc_ns && !loc_nl)
      return NULL;
    else
    {
      model->NAME   = stm_slwmname_extract(model->NAME);
    if (loc_ns && loc_nl)
        tbl   = stm_modtbl_create_slew2D_fstm(model,loc_slews,loc_ns,loc_loads,loc_nl,ci0);
    else if (!loc_ns && loc_nl)
        tbl   = stm_modtbl_create_slew1Dslewfix_fstm(model,loc_loads,loc_nl,STM_DEF_SLEW,ci0);
    else if (loc_ns && !loc_nl)
        tbl   = stm_modtbl_create_slew1Dloadfix_fstm(model,loc_slews,loc_ns,STM_DEF_LOAD);
    stm_mod_destroy_model (model);
    model->UTYPE          = STM_MOD_MODTBL;
    model->UMODEL.TABLE   = tbl;

      return model;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    stm_getAxis()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_getAxis(timing_model *model,
                 float *x, long *nx,
                 float *y, long *ny)
{
  ptype_list        *ptype;
  stm_carac_values  *cvalues = NULL;
  float              vf, imax, vt, *loc_axis;
  double             slews[1024], loads[1024];
  int                i;

  if(STM_AXIS_NSLOPE){
    *nx = STM_AXIS_NSLOPE;
    for (i = 0; i < *nx; i ++)
      x[i] = STM_AXIS_SLOPEIN[i];
  }else{
    loc_axis = stm_dyna_slews (STM_DEF_AXIS_BASE, -1.0);
    *nx = STM_DEF_AXIS_BASE * 2 + 1;
    for (i = 0; i < *nx; i ++)
      x[i] = loc_axis[i];
    mbkfree (loc_axis);
  }
  if(STM_AXIS_NCAPA){
    *ny = STM_AXIS_NCAPA;
    for (i = 0; i < *ny; i ++)
      y[i] = STM_AXIS_CAPAOUT[i];
  }else{
    avt_errmsg (STM_ERRMSG,"039", AVT_ERROR);
    *ny = 5;
    for (i = 0; i < *ny; i ++)
      y[i]     = 10.0 + i*10.0 ;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_mod_defaultfct2tbl_slew()                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_mod_defaultfct2tbl_slew(timing_model *model, char xtype, char ytype)
{
  float          s_axis[2048], l_axis[2048];
  long           ns = 0, nl = 0;
  ptype_list	*ptype;

  stm_getAxis(model, s_axis, &ns, l_axis, &nl);
  
  ptype		= getptype(model->USER,STM_CARAC_VALUES);
  if (ptype)
	model->USER	= delptype(model->USER,STM_CARAC_VALUES);
  
  if (xtype == STM_NOTYPE)
  {
    stm_mod_fct2tbl_slew(model, NULL, 0, l_axis, nl, 0.0);
  }
  else if (ytype == STM_NOTYPE)
  {
    stm_mod_fct2tbl_slew(model, s_axis, ns, NULL, 0, 0.0);
  }
  else
  {
    stm_mod_fct2tbl_slew(model, s_axis, ns, l_axis, nl, 0.0);
  }

  if (ptype)
  {
	ptype->NEXT	= model->USER;
	model->USER = ptype;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_mod_defaultfct2tbl_delay()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_mod_defaultfct2tbl_delay(timing_model *model, char xtype, char ytype)
{
  float      	 s_axis[2048], l_axis[2048];
  long       	 ns = 0, nl = 0;
  ptype_list	*ptype;
  void          *data;
  long           type;

  stm_getAxis(model, s_axis, &ns, l_axis, &nl);
  
  ptype		    = getptype(model->USER,STM_CARAC_VALUES);
  if (ptype)
  {
    data        = ptype->DATA;
    type        = ptype->TYPE;
	model->USER	= delptype(model->USER,STM_CARAC_VALUES);
  }
  if (xtype == STM_NOTYPE)
    stm_mod_fct2tbl_delay (model, NULL, 0, l_axis, nl, 0.0);
  else if (ytype == STM_NOTYPE)
    stm_mod_fct2tbl_delay (model, s_axis, ns, NULL, 0, 0.0);
  else
    stm_mod_fct2tbl_delay (model, s_axis, ns, l_axis, nl, 0.0);

  if (ptype)
  {
	model->USER = addptype(model->USER,type,data);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/

double stm_thr2scm(double fin, double thmin, double thmax, double vt, double vf, double vdd, char type)
{
    char elptype;
    if(type == STM_UP)
        elptype = elpRISE;
    else if(type == STM_DN)
        elptype = elpFALL;
    return elpThr2Scm (fin, thmin, thmax, vt, vf, vdd, elptype);
}

double stm_scm2thr(double fout, double thmin, double thmax, double vt, double vf, double vdd, char type)
{
    char elptype;
    if(type == STM_UP)
        elptype = elpRISE;
    else if(type == STM_DN)
        elptype = elpFALL;
    return elpScm2Thr (fout, thmin, thmax, vt, vf, vdd, elptype);
}

void stm_mod_shrinkslewaxis_thr2scm(timing_model *model, double *slews_scaled, double *slews, long ns)
{
    int i;
    double vt, vdd, vf, thmin, thmax;
    char type;

    vt = stm_mod_vt(model);
    vdd = stm_mod_vdd_input(model);
    vf = stm_mod_vf_input(model);
    if((model->TTYPE == STM_HL) || (model->TTYPE == STM_HH)){
        thmin = STM_DEFAULT_SMINR;
        thmax = STM_DEFAULT_SMAXR;
        type = STM_UP;
    }else if((model->TTYPE == STM_LH) || (model->TTYPE == STM_LL)){
        thmin = STM_DEFAULT_SMINF;
        thmax = STM_DEFAULT_SMAXF;
        type = STM_DN;
    }
    for (i = 0; i < ns; i++)
        slews_scaled[i] = stm_thr2scm (slews[i], thmin, thmax, vt, vf, vdd, type);
}

double stm_mod_shrinkslew_thr2scm(timing_model *model, double fin)
{
    double vt, vdd, vf, thmin, thmax;
    char type;

    if(model->UTYPE != STM_MOD_MODSCM)
        return fin;
    vt = stm_mod_vt(model);
    vdd = stm_mod_vdd_input(model);
    vf = stm_mod_vf_input(model);
    if((model->TTYPE == STM_HL) || (model->TTYPE == STM_HH)){
        thmin = STM_DEFAULT_SMINR;
        thmax = STM_DEFAULT_SMAXR;
        type = STM_UP;
    }else if((model->TTYPE == STM_LH) || (model->TTYPE == STM_LL)){
        thmin = STM_DEFAULT_SMINF;
        thmax = STM_DEFAULT_SMAXF;
        type = STM_DN;
    }
    return stm_thr2scm (fin, thmin, thmax, vt, vf, vdd, type);
}

double stm_mod_shrinkslew_scm2thr(timing_model *model, double fout)
{
    double vt, vdd, vf, thmin, thmax;
    char type;

    if(model->UTYPE != STM_MOD_MODSCM)
        return fout;
    vt = stm_mod_vt(model);
    vdd = stm_mod_vdd(model);
    vf = stm_mod_vf(model);
    if((model->TTYPE == STM_LH) || (model->TTYPE == STM_HH)){
        thmin = STM_DEFAULT_SMINR;
        thmax = STM_DEFAULT_SMAXR;
        type = STM_UP;
    }else if((model->TTYPE == STM_HL) || (model->TTYPE == STM_LL)){
        thmin = STM_DEFAULT_SMINF;
        thmax = STM_DEFAULT_SMAXF;
        type = STM_DN;
    }
    return stm_scm2thr (fout, thmin, thmax, vt, vf, vdd, type);
}

void stm_mod_shrinkslewaxis_scm2thr (timing_model *model, double *slews)
{
   int i;
/*   double vt, vdd, vf, thmin, thmax;
   char type;*/
   timing_table *table = model->UMODEL.TABLE;
   
/*   vt = stm_mod_vt(model);
   vdd = stm_mod_vdd_input(model);
   vf = stm_mod_vf_input(model);
   if((model->TTYPE == STM_HL) || (model->TTYPE == STM_HH)){
       thmin = STM_DEFAULT_SMINR;
       thmax = STM_DEFAULT_SMAXR;
       type = STM_UP;
   }else if((model->TTYPE == STM_LH) || (model->TTYPE == STM_LL)){
       thmin = STM_DEFAULT_SMINF;
       thmax = STM_DEFAULT_SMAXF;
       type = STM_DN;
   }*/
   if (table->XTYPE == STM_INPUT_SLEW || table->XTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NX; i++) {
         table->XRANGE[i] = /*stm_scm2thr (table->XRANGE[i], thmin, thmax, vt, vf, vdd, type)*/slews[i];
      }
   }

   if (table->YTYPE == STM_INPUT_SLEW || table->YTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NY; i++) {
         table->YRANGE[i] = /*stm_scm2thr (table->YRANGE[i], thmin, thmax, vt, vf, vdd, type)*/slews[i];
      }
   }
}

void stm_mod_shrinkslewdata_scm2thr (timing_model *model)
{
   int i, j;
   double vt, vdd, vth, vf, thmin, thmax;
   char type;
   timing_table *table = model->UMODEL.TABLE;

   vt = stm_mod_vt(model);
   vdd = stm_mod_vdd(model);
   vf = stm_mod_vf(model);
   vth = stm_mod_vth(model);
   if((model->TTYPE == STM_LH) || (model->TTYPE == STM_HH)){
       thmin = STM_DEFAULT_SMINR;
       thmax = STM_DEFAULT_SMAXR;
       type = STM_UP;
   }else if((model->TTYPE == STM_HL) || (model->TTYPE == STM_LL)){
       thmin = STM_DEFAULT_SMINF;
       thmax = STM_DEFAULT_SMAXF;
       type = STM_DN;
   }

   if (!stm_modtbl_isnull (table->CST)) {
     stm_modtbl_setconst (table, stm_scm2thr (table->CST, thmin, thmax, vt, vf, vdd, type));
   }
   else
   if (table->SET1D) {
      for (i = 0; i < table->NX; i++) {
        table->SET1D[i] = stm_scm2thr (table->SET1D[i], thmin, thmax, vt, vf, vdd, type);
      }
   }
   else
   if (table->SET2D) {
      for (i = 0; i < table->NX; i++) {
         for (j = 0; j < table->NY; j++) {
           table->SET2D[i][j] = stm_scm2thr (table->SET2D[i][j], thmin, thmax, vt, vf, vdd, type);
         }
      }
   }
}


int stm_if_thresholds(void)
{
    if((STM_DEFAULT_SMINR > 0.0) && 
       (STM_DEFAULT_SMAXR > 0.0) && 
       (STM_DEFAULT_SMINF > 0.0) && 
       (STM_DEFAULT_SMAXF > 0.0))
        return 1;
    else
        return 0;
}
