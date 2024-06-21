/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_eval.c                                              */
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

float stm_mod_constraint (timing_model *tmodel, float inputslew, float clockslew)
{
    if (tmodel) {
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                return stm_modtbl_constraint (tmodel->UMODEL.TABLE, inputslew, clockslew);
                break;
            case STM_MOD_MODSCM:
                avt_errmsg (STM_ERRMSG, "010", AVT_ERROR);
                return 0.0;
                break;
            case STM_MOD_MODPLN:
                return stm_modpln_constraint (tmodel->UMODEL.POLYNOM, inputslew, clockslew);
                break;
            case STM_MOD_MODFCT:
                return stm_modfct_constraint (tmodel->UMODEL.FUNCTION, inputslew, clockslew);
                break;
        }
    }

    return 0.0;
}

/****************************************************************************/

float stm_mod_slew_pi (timing_model* tmodel, float c1, float c2, float r, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame)
{
    float vth;
    float vdd;
    float imax;
    float load;
    float cconf;
    float f = STM_DEF_SLEW;

    if (tmodel) {
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                load = c1 + c2;
                f = stm_modtbl_slew (tmodel->UMODEL.TABLE, load, slew);
                break;
            case STM_MOD_MODSCM:
                if( STM_IMAX_FOR_PILOAD ) {
                  vth = stm_modscm_vth (tmodel->UMODEL.SCM);
                  imax = stm_modscm_imax (tmodel->UMODEL.SCM);
                  cconf = stm_modscm_cconf (tmodel->UMODEL.SCM, slew);
                  load = stm_capaeq (imax, r, c1 + cconf, c2, vth, signame);
                  load -= cconf;
                }
                else {
                  //load = c1 + c2;
                  load = stm_modscm_capaeq (tmodel->UMODEL.SCM, slew, r, c1, c2, signame );
                }
                f = stm_modscm_slew (tmodel->UMODEL.SCM, slew, pwl, ptpwl, load);
                break;
            case STM_MOD_MODPLN:
                load = c1 + c2;
                f = stm_modpln_slew (tmodel->UMODEL.POLYNOM, slew, load);
                break;
            case STM_MOD_MODFCT:
                load = c1 + c2;
                f = stm_modfct_slew (tmodel->UMODEL.FUNCTION, slew, load);
                break;
            case STM_MOD_MODIV:
                vth = stm_mod_vth(tmodel);
                vdd = stm_mod_vdd(tmodel);
                f = stm_modiv_slew_pi (tmodel->UMODEL.IV, slew, r, c1, c2, vth, vdd );
                break;
        }
    }

    if( f < 1.0 ) f = 1.0 ;
    return f;
}

/****************************************************************************/

float stm_mod_delay_pi (timing_model *tmodel, float c1, float c2, float r, float slew, stm_pwl *pwl, char *signame)
{
    float vth;
    float vdd;
    float load;
    float imax;
    float cconf;

    if (tmodel) {
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                load = c1 + c2;
                return stm_modtbl_delay (tmodel->UMODEL.TABLE, load, slew);
                break;
            case STM_MOD_MODSCM:
                if( STM_IMAX_FOR_PILOAD ) {
                  vth = stm_modscm_vth (tmodel->UMODEL.SCM);
                  imax = stm_modscm_imax (tmodel->UMODEL.SCM);
                  cconf = stm_modscm_cconf (tmodel->UMODEL.SCM, slew);
                  load = stm_capaeq (imax, r, c1 + cconf, c2, vth, signame);
                  load -= cconf;
                }
                else
                  load = stm_modscm_capaeq (tmodel->UMODEL.SCM, slew, r, c1, c2, signame );
                return stm_modscm_delay (tmodel->UMODEL.SCM, slew, pwl, load);
                break;
            case STM_MOD_MODPLN:
                load = c1 + c2;
                return stm_modpln_delay (tmodel->UMODEL.POLYNOM, slew, load);
                break;
            case STM_MOD_MODFCT:
                load = c1 + c2;
                return stm_modfct_delay (tmodel->UMODEL.FUNCTION, slew, load);
                break;
            case STM_MOD_MODIV:
                vth = stm_mod_vth(tmodel);
                vdd = stm_mod_vdd(tmodel);
                return stm_modiv_delay_pi (tmodel->UMODEL.IV, slew, r, c1, c2, vth, vdd );
                break;
        }
    }
    return 0;
}


/****************************************************************************/

float stm_mod_slew (timing_model* tmodel, float load, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame)
{
    float vth;
    float vdd;
    float f = STM_DEF_SLEW;

    if (tmodel) {
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                f = stm_modtbl_slew (tmodel->UMODEL.TABLE, load, slew);
                break;
            case STM_MOD_MODSCM:
                f = stm_modscm_slew (tmodel->UMODEL.SCM, slew, pwl, ptpwl, load);
                break;
            case STM_MOD_MODPLN:
                f = stm_modpln_slew (tmodel->UMODEL.POLYNOM, slew, load);
                break;
            case STM_MOD_MODFCT:
                f = stm_modfct_slew (tmodel->UMODEL.FUNCTION, slew, load);
                break;
            case STM_MOD_MODIV:
                vth = stm_mod_vth(tmodel);
                vdd = stm_mod_vdd(tmodel);
                f = stm_modiv_slew_c (tmodel->UMODEL.IV, slew, load, vth, vdd);
        }
    }
    if( f < 1.0 ) f = 1.0 ;
    return f;
}

/****************************************************************************/

float stm_mod_delay (timing_model *tmodel, float load, float slew, stm_pwl *pwl, char *signame)
{
    float vth;
    float vdd;

    if (tmodel) {
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                return stm_modtbl_delay (tmodel->UMODEL.TABLE, load, slew);
                break;
            case STM_MOD_MODSCM:
                return stm_modscm_delay (tmodel->UMODEL.SCM, slew, pwl, load);
                break;
            case STM_MOD_MODPLN:
                return stm_modpln_delay (tmodel->UMODEL.POLYNOM, slew, load);
                break;
            case STM_MOD_MODFCT:
                return stm_modfct_delay (tmodel->UMODEL.FUNCTION, slew, load);
                break;
            case STM_MOD_MODIV:
                vth = stm_mod_vth(tmodel);
                vdd = stm_mod_vdd(tmodel);
                return stm_modiv_delay_c (tmodel->UMODEL.IV, slew, load, vth, vdd );
                break;
        }
    }
    return 0;
}

/****************************************************************************/

void stm_mod_timing_pi( timing_model *dmodel, 
                        timing_model *fmodel,
                        float         fin, 
                        stm_pwl      *pwlin, 
                        stm_driver   *driver,
                        float         c1,
                        float         c2,
                        float         r,
                        float        *delay,
                        float        *fout,
                        stm_pwl     **pwlout,
                        char          *signame,
                        char          *inputname,
                        char dirin,
                        char dirout
                      )
{
    float         vth;
    float         vdd;
    float         load;
    float         imax;
    float         cconf;

    if( delay ) *delay = 0.0 ;
    if( fout  ) *fout  = fin ;
    if( pwlout ) *pwlout = NULL ;

    /* Si les modèles ne sont pas les meme pour le front et le delai, il ne faut pas faire l'hypothèse
       que la capacité equivalente à la charge en pi sera la meme */
    if( dmodel->UTYPE      == STM_MOD_MODSCM     && 
        fmodel->UTYPE      == STM_MOD_MODSCM     &&
        dmodel->UMODEL.SCM == fmodel->UMODEL.SCM    ) {
   
            load = c1 + c2;
           
            if( V_BOOL_TAB[ __AVT_PRECISE_PILOAD ].VALUE ) {
              stm_modscm_timing (dmodel->UMODEL.SCM, fmodel->UMODEL.SCM, fin, pwlin, driver, r, c1, c2, delay, fout, pwlout, dmodel->NAME );
            }
            else {
              if( STM_IMAX_FOR_PILOAD ) {
                vth = stm_modscm_vth (dmodel->UMODEL.SCM);
                imax = stm_modscm_imax (dmodel->UMODEL.SCM);
                cconf = stm_modscm_cconf (dmodel->UMODEL.SCM, fin);
                load = stm_capaeq (imax, r, c1 + cconf, c2, vth, signame);
                load -= cconf;
              }
              else
                load = stm_modscm_capaeq (dmodel->UMODEL.SCM, fin, r, c1, c2, signame );
              stm_modscm_timing (dmodel->UMODEL.SCM, fmodel->UMODEL.SCM, fin, pwlin, driver, -1.0, load, -1.0, delay, fout, pwlout, dmodel->NAME );
            }
              
    }
    else {
      if( dmodel == fmodel && dmodel->UTYPE == STM_MOD_MODIV ) {
        vth = stm_mod_vth(dmodel);
        vdd = stm_mod_vdd(dmodel);
        stm_modiv_timing_pi( dmodel->UMODEL.IV, fin, r, c1, c2, vth, vdd, delay, fout );
      }
      else {

        if( delay && dmodel ) {
            switch (dmodel->UTYPE) {
                case STM_MOD_MODTBL:
                    load = c1 + c2;
                    *delay = stm_modtbl_delay (dmodel->UMODEL.TABLE, load, fin);
                    break;
                case STM_MOD_MODSCM:
                    if( STM_IMAX_FOR_PILOAD ) {
                      vth = stm_modscm_vth (dmodel->UMODEL.SCM);
                      imax = stm_modscm_imax (dmodel->UMODEL.SCM);
                      cconf = stm_modscm_cconf (dmodel->UMODEL.SCM, fin);
                      load = stm_capaeq (imax, r, c1 + cconf, c2, vth, signame);
                      load -= cconf;
                    }
                    else
                      load = stm_modscm_capaeq (dmodel->UMODEL.SCM, fin, r, c1, c2, signame );
                    *delay = stm_modscm_delay (dmodel->UMODEL.SCM, fin, pwlin, load);
                    break;
                case STM_MOD_MODPLN:
                    load = c1 + c2;
                    *delay = stm_modpln_delay (dmodel->UMODEL.POLYNOM, fin, load);
                    break;
                case STM_MOD_MODFCT:
                    load = c1 + c2;
                    *delay = stm_modfct_delay (dmodel->UMODEL.FUNCTION, fin, load);
                    break;
                case STM_MOD_MODIV:
                    load = c1 + c2;
                    vth = stm_mod_vth(dmodel);
                    vdd = stm_mod_vdd(dmodel);
                    *delay = stm_modiv_delay_c (dmodel->UMODEL.IV, fin, load, vth, vdd );
                    break;
            }
        }

        if( fout && fmodel ) {
            switch (fmodel->UTYPE) {
                case STM_MOD_MODTBL:
                    load = c1 + c2;
                    *fout = stm_modtbl_slew (fmodel->UMODEL.TABLE, load, fin);
                    break;
                case STM_MOD_MODSCM:
                    if( STM_IMAX_FOR_PILOAD ) {
                      vth = stm_modscm_vth (fmodel->UMODEL.SCM);
                      imax = stm_modscm_imax (fmodel->UMODEL.SCM);
                      cconf = stm_modscm_cconf (fmodel->UMODEL.SCM, fin);
                      load = stm_capaeq (imax, r, c1 + cconf, c2, vth, signame);
                      load -= cconf;
                    }
                    else
                      load = stm_modscm_capaeq (fmodel->UMODEL.SCM, fin, r, c1, c2, signame );
                    *fout = stm_modscm_slew (fmodel->UMODEL.SCM, fin, pwlin, pwlout, load);
                    break;
                case STM_MOD_MODPLN:
                    load = c1 + c2;
                    *fout = stm_modpln_slew (fmodel->UMODEL.POLYNOM, fin, load);
                    break;
                case STM_MOD_MODFCT:
                    load = c1 + c2;
                    *fout = stm_modfct_slew (fmodel->UMODEL.FUNCTION, fin, load);
                    break;
                case STM_MOD_MODIV:
                    load = c1 + c2;
                    vth = stm_mod_vth(fmodel);
                    vdd = stm_mod_vdd(fmodel);
                    *fout = stm_modiv_slew_c (fmodel->UMODEL.IV, fin, load, vth, vdd );
                    break;
            }
        }
      }
    }

    if( fout ) {
      if( *fout < 1.0 ) *fout = 1.0 ;
    }
    if (delay && V_BOOL_TAB[__STM_PRECISION_WARNING].VALUE) {
        if (fin > V_INT_TAB[__STM_PRECISION_THRESHOLD].VALUE * (*delay)) {
            float load=0;
            if (c1>0) load+=c1;
            if (c2>0) load+=c2;
            avt_errmsg(STM_ERRMSG, "052", AVT_WARNING, inputname, dirin, signame, dirout, (int)fin, (int)load);
        }
    }
}

/****************************************************************************/

void stm_mod_timing( timing_model *dmodel, 
                     timing_model *fmodel,
                     float         fin, 
                     stm_pwl      *pwlin, 
                     stm_driver   *driver,
                     float         load,
                     float        *delay,
                     float        *fout,
                     stm_pwl     **pwlout,
                     char         *signame,
                     char         *inputname,
                     char dirin,
                     char dirout
                   )
{
    float vth;
    float vdd;

    if( delay ) *delay = 0.0 ;
    if( fout  ) *fout  = fin ;
    if( pwlout ) *pwlout = NULL ;

    if( dmodel->UTYPE      == STM_MOD_MODSCM && 
        fmodel->UTYPE      == STM_MOD_MODSCM    ) {
        stm_modscm_timing (dmodel->UMODEL.SCM, fmodel->UMODEL.SCM, fin, pwlin, driver, -1.0, load, -1.0, delay, fout, pwlout, dmodel->NAME );
    }
    else {
      if( dmodel == fmodel && dmodel->UTYPE == STM_MOD_MODIV ) {
        vth = stm_mod_vth(dmodel);
        vdd = stm_mod_vdd(dmodel);
        stm_modiv_timing_c( dmodel->UMODEL.IV, fin, load, vth, vdd, delay, fout );
      }
      else {

        if( delay && dmodel ) {
            switch (dmodel->UTYPE) {
                case STM_MOD_MODTBL:
                    *delay = stm_modtbl_delay (dmodel->UMODEL.TABLE, load, fin);
                    break;
                case STM_MOD_MODSCM:
                    stm_modscm_timing (dmodel->UMODEL.SCM, fmodel->UMODEL.SCM, fin, pwlin, driver, -1.0, load, -1.0, delay, fout, pwlout, dmodel->NAME );
                    break;
                case STM_MOD_MODPLN:
                    *delay = stm_modpln_delay (dmodel->UMODEL.POLYNOM, fin, load);
                    break;
                case STM_MOD_MODFCT:
                    *delay = stm_modfct_delay (dmodel->UMODEL.FUNCTION, fin, load);
                    break;
                case STM_MOD_MODIV:
                    vth = stm_mod_vth(dmodel);
                    vdd = stm_mod_vdd(dmodel);
                    *delay = stm_modiv_delay_c (dmodel->UMODEL.IV, fin, load, vth, vdd );
                    break;
            }
        }

        if( fout && fmodel ) {
            switch (fmodel->UTYPE) {
                case STM_MOD_MODTBL:
                    *fout = stm_modtbl_slew (fmodel->UMODEL.TABLE, load, fin);
                    break;
                case STM_MOD_MODSCM:
                    stm_modscm_timing (dmodel->UMODEL.SCM, fmodel->UMODEL.SCM, fin, pwlin, driver, -1.0, load, -1.0, delay, fout, pwlout, dmodel->NAME );
                    break;
                case STM_MOD_MODPLN:
                    *fout = stm_modpln_slew (fmodel->UMODEL.POLYNOM, fin, load);
                    break;
                case STM_MOD_MODFCT:
                    *fout = stm_modfct_slew (fmodel->UMODEL.FUNCTION, fin, load);
                    break;
                case STM_MOD_MODIV:
                    vth = stm_mod_vth(fmodel);
                    vdd = stm_mod_vdd(fmodel);
                    *fout = stm_modiv_slew_c (fmodel->UMODEL.IV, fin, load, vth, vdd );
                    break;
            }
        }
      }
    }

    if( fout ) {
      if( *fout < 1.0 ) *fout = 1.0 ;
    }
    if (delay && V_BOOL_TAB[__STM_PRECISION_WARNING].VALUE) {
        if (fin > V_INT_TAB[__STM_PRECISION_THRESHOLD].VALUE * (*delay)) {
            avt_errmsg(STM_ERRMSG, "052", AVT_WARNING, inputname, dirin, signame, dirout, (int)fin, (int)load);
        }
    }
}

/****************************************************************************/

float stm_mod_loadparam (timing_model *tmodel, float load, float slew)
{
    if (tmodel)
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                return stm_modtbl_loadparam (tmodel->UMODEL.TABLE, load, slew);
                break;
            case STM_MOD_MODSCM:
                return stm_modscm_loadparam (tmodel->UMODEL.SCM, load, slew);
                break;
            case STM_MOD_MODPLN:
                avt_errmsg (STM_ERRMSG, "013", AVT_ERROR);
                break;
            case STM_MOD_MODFCT:
                avt_errmsg (STM_ERRMSG, "013", AVT_ERROR);
                break;
        }
    return 0;
}

/****************************************************************************/

float stm_mod_clockslewparam (timing_model *tmodel, float clockslew, float slew)
{
    if (tmodel)
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                return stm_modtbl_clockslewparam (tmodel->UMODEL.TABLE, clockslew, slew);
                break;
            case STM_MOD_MODSCM:
                avt_errmsg (STM_ERRMSG, "014", AVT_ERROR);
                break;
            case STM_MOD_MODPLN:
                avt_errmsg (STM_ERRMSG, "015", AVT_ERROR);
                break;
            case STM_MOD_MODFCT:
                avt_errmsg (STM_ERRMSG, "015", AVT_ERROR);
                break;
        }
    return 0;
}
    
/****************************************************************************/

float stm_mod_dataslewparam (timing_model *tmodel, float clockslew, float slew)
{
    if (tmodel)
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                return stm_modtbl_dataslewparam (tmodel->UMODEL.TABLE, clockslew, slew);
                break;
            case STM_MOD_MODSCM:
                avt_errmsg (STM_ERRMSG, "016", AVT_ERROR);
                break;
            case STM_MOD_MODPLN:
                avt_errmsg (STM_ERRMSG, "017", AVT_ERROR);
                break;
            case STM_MOD_MODFCT:
                avt_errmsg (STM_ERRMSG, "017", AVT_ERROR);
                break;
        }
    return 0;
}
    
/****************************************************************************/

float stm_mod_imax (timing_model *model)
{
    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODTBL:
                avt_errmsg (STM_ERRMSG, "018", AVT_ERROR);
                break;
            case STM_MOD_MODSCM:
                return stm_modscm_imax (model->UMODEL.SCM);
                break;
            case STM_MOD_MODPLN:
                avt_errmsg (STM_ERRMSG, "019", AVT_ERROR);
                break;
            case STM_MOD_MODFCT:
                avt_errmsg (STM_ERRMSG, "019", AVT_ERROR);
                break;
        }
    return 0;
}
    
/****************************************************************************/

float stm_mod_slewparam (timing_model *tmodel, float load, float slew)
{
    if (tmodel)
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                return stm_modtbl_slewparam (tmodel->UMODEL.TABLE, load, slew);
                break;
            case STM_MOD_MODSCM:
                return stm_modscm_slewparam (tmodel->UMODEL.SCM, load, slew);
                break;
            case STM_MOD_MODPLN:
                avt_errmsg (STM_ERRMSG, "020", AVT_ERROR);
                break;
            case STM_MOD_MODFCT:
                avt_errmsg (STM_ERRMSG, "020", AVT_ERROR);
                break;
        }
    return 0;
}
    
/****************************************************************************/

float stm_mod_vt (timing_model *model)
{
    if (!model)
        return STM_DEFAULT_VT;
    else {
        if( model->UTYPE == STM_MOD_MODSCM )
          if( model->UMODEL.SCM->TYPE == STM_MODSCM_DUAL )
            return model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VT];
        return model->VT;
    }
}
    
/****************************************************************************/
    
float stm_mod_vf (timing_model *model)
{
    if (!model)
        return STM_DEFAULT_VFU;
    else
        return model->VF;
}
    
/****************************************************************************/
    
float stm_mod_vf_input (timing_model *model)
{
    float vf = -1.0;
    if (!model)
        vf = STM_DEFAULT_VFU;
    else if(model->UTYPE == STM_MOD_MODSCM){
        if((model->TTYPE == STM_HL) || (model->TTYPE == STM_HH))
            vf = stm_modscm_vf_input (model->UMODEL.SCM);
        else if((model->TTYPE == STM_LL) || (model->TTYPE == STM_LH))
            vf =  0.0;
    }
    if(vf < 0.0){
        if((model->TTYPE == STM_HL) || (model->TTYPE == STM_HH))
            vf = stm_mod_vdd(model);
        else if((model->TTYPE == STM_LL) || (model->TTYPE == STM_LH))
            vf =  0.0;
    }
    return vf;
}
/****************************************************************************/
    
float stm_mod_default_vt (void)
{
    return STM_DEFAULT_VT;
}

/****************************************************************************/
    
float stm_mod_default_vdd (void)
{
    return V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
}

/****************************************************************************/
    
float stm_mod_default_vth (void)
{
    return V_FLOAT_TAB[__SIM_VTH].VALUE * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
}

/****************************************************************************/
    
float stm_mod_default_vfd (void)
{
    return STM_DEFAULT_VFD;
}

/****************************************************************************/
    
float stm_mod_default_vfu (void)
{
    return STM_DEFAULT_VFU;
}

/****************************************************************************/
    
float stm_mod_vth (timing_model *model)
{
    if (!model)
        return V_FLOAT_TAB[__SIM_VTH].VALUE * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    else
        return model->VTH;
}
    
/****************************************************************************/
    
float stm_mod_vdd (timing_model *model)
{
    if (!model)
        return V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    else
        return model->VDD;
}
    
/****************************************************************************/
    
float stm_mod_vdd_input (timing_model *model)
{
    float vdd = -1.0;
    if (!model)
        vdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    else if(model->UTYPE == STM_MOD_MODSCM)
        vdd = stm_modscm_vf_input (model->UMODEL.SCM);
    if(vdd < 0.0)
        vdd = stm_mod_vdd(model);
    return vdd;
}
   /****************************************************************************/

void stm_mod_driver( timing_model *model, float *r, float *v )
{
  if( r ) *r = -1.0 ;
  if( v ) *v = -1.0 ;

  if( model ) {

    switch( model->UTYPE ) 
    {
      case STM_MOD_MODSCM:
      
        if (model->UMODEL.SCM->TYPE == STM_MODSCM_DUAL ) {
          if( r ) 
            *r = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RLIN] ;
          if( v )
            *v = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_KRT] * model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VDDMAX] ;
        }
        break;
    }
  }
}
   /****************************************************************************/

float stm_mod_rlin (timing_model *model)
{
    float rlin;

    rlin = -1.0 ;

    if (model)
        switch (model->UTYPE) {
            case STM_MOD_MODSCM:
                if (model->UMODEL.SCM->TYPE == STM_MODSCM_DUAL )
                        rlin = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RLIN] ;
                break;
        }
        
    return rlin ;
}

/****************************************************************************/

float stm_mod_default_rlin (void)
{
  return STM_DEFAULT_RLIN ;
}
    
/****************************************************************************/

float stm_mod_vsat (timing_model *model)
{
    float vsat ;
    float rsat ;
    float rsatmin ;
    float rlin ;
    float vddmax ;
    float imax ;

    vsat = -1.0 ;

    if (model) {
    
        switch (model->UTYPE) {
        
            case STM_MOD_MODSCM:
            
                if (model->UMODEL.SCM->TYPE == STM_MODSCM_DUAL ) {
                
                    rsat   = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RSAT] ;
                    rlin   = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RLIN] ;
                    imax   = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_IMAX] ;
                    vddmax = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VDDMAX] ;
                  
                    rsatmin = vddmax/imax ;

                    if( rsat > 0.0 && rsat > rsatmin && rsatmin > rlin ) {
                        vsat = vddmax - 
                               ( imax - vddmax / rsat ) * rlin * rsat / ( rsat - rlin ) ;
                    }
                }
                
                break;
        }
    }
        
    return vsat ;
}
    
/****************************************************************************/

float stm_mod_default_vsat(void)
{
    return STM_DEFAULT_VSAT ;
}
