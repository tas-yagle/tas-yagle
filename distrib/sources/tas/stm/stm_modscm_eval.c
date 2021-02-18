/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_eval.c                                           */
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

float stm_modscm_cconf (timing_scm *scm, float slew)
{
    if (!scm)
        return 0.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_cconf (scm->PARAMS.DUAL, slew);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_cconf (scm->PARAMS.GOOD, slew);
            break;
        case STM_MODSCM_FALSE:
            return 0.0;
            break;
        case STM_MODSCM_PATH:
            return 0.0;
            break;
        case STM_MODSCM_CST:
            return 0.0;
            break;
    }
    return 0.0;
}

/****************************************************************************/

float stm_modscm_imax (timing_scm *scm)
{
    if (!scm)
        return 0.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_imax (scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_imax (scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_imax (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_imax (scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            return stm_modscm_cst_imax (scm->PARAMS.CST);
            break;
    }
    return 0.0;
}

/****************************************************************************/
    

float stm_modscm_vth (timing_scm *scm)
{
    if (!scm)
        return 0.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_vth (scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_vth (scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_vth (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_vth (scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            return stm_modscm_cst_vth (scm->PARAMS.CST);
    }
    return 0.0;
}

/****************************************************************************/
    

float stm_modscm_slew (timing_scm *scm, float slew, stm_pwl *pwl, stm_pwl **ptpwl, float load)
{
    if (!scm)
        return 0.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_slew (scm->PARAMS.DUAL, slew, pwl, ptpwl, load);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_slew (scm->PARAMS.GOOD, slew, load);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_slew (scm->PARAMS.FALS, slew, load);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_slew (scm->PARAMS.PATH, slew, load);
            break;
        case STM_MODSCM_CST:
            return stm_modscm_cst_slew (scm->PARAMS.CST);
    }
    return 0.0;
}

/****************************************************************************/
    
float stm_modscm_slope (timing_scm *scm, float slew, float load)
{
    if (!scm)
        return 0.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_slope (scm->PARAMS.DUAL, slew, load);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_slope (scm->PARAMS.GOOD, slew, load);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_slope (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_slope (scm->PARAMS.PATH, slew, load);
            break;
        case STM_MODSCM_CST:
            return stm_modscm_cst_slope (scm->PARAMS.CST);
    }
    return 0.0;
}

/****************************************************************************/

void  stm_modscm_timing( timing_scm *dscm,
                         timing_scm *fscm,
                         float       fin, 
                         stm_pwl    *pwlin, 
                         stm_driver *driver,
                         float       r,
                         float       c1,
                         float       c2,
                         float      *delay,
                         float      *fout,
                         stm_pwl   **pwlout,
                         char *modelname
                       )
{
  float load ;

  if( c2>0.0 )
    load = c1+c2 ;
  else
    load = c1 ;

  if( delay ) *delay = 0.0 ;
  if( fout  ) *fout  = fin ;
  if( pwlout ) *pwlout = NULL ;

  if( dscm == fscm && dscm->TYPE == STM_MODSCM_DUAL ) {
      stm_modscm_dual_timing( dscm->PARAMS.DUAL , fin, pwlin, driver, r, c1, c2, delay, fout, pwlout,modelname);
  }
  else {
 
      if( delay && dscm ) {
          switch (dscm->TYPE) {
              case STM_MODSCM_DUAL:
                  *delay = stm_modscm_dual_delay( dscm->PARAMS.DUAL, fin, pwlin, load );
                  break;
              case STM_MODSCM_GOOD:
                  *delay = stm_modscm_good_delay (dscm->PARAMS.GOOD, fin, load);
                  break;
              case STM_MODSCM_FALSE:
                  *delay = stm_modscm_false_delay (dscm->PARAMS.FALS, fin, load);
                  break;
              case STM_MODSCM_PATH:
                  *delay = stm_modscm_path_delay (dscm->PARAMS.PATH, fin, load);
                  break;
              case STM_MODSCM_CST:
                  *delay = stm_modscm_cst_delay (dscm->PARAMS.CST);
                  break ;
          }
      }
      
      if( fout && fscm ) {
          switch (fscm->TYPE) {
              case STM_MODSCM_DUAL:
                  *fout = stm_modscm_dual_slew( fscm->PARAMS.DUAL, fin, pwlin, pwlout, load );
                  break;
              case STM_MODSCM_GOOD:
                  *fout = stm_modscm_good_slew (fscm->PARAMS.GOOD, fin, load);
                  break;
              case STM_MODSCM_FALSE:
                  *fout = stm_modscm_false_slew (fscm->PARAMS.FALS, fin, load);
                  break;
              case STM_MODSCM_PATH:
                  *fout = stm_modscm_path_slew (fscm->PARAMS.PATH, fin, load);
                  break;
              case STM_MODSCM_CST:
                  *fout = stm_modscm_cst_slew (fscm->PARAMS.CST);
                  break ;
          }
      }
  }
}

/****************************************************************************/
float stm_modscm_delay (timing_scm *scm, float slew, stm_pwl *pwl, float load)
{
    if (!scm)
        return 0.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_delay (scm->PARAMS.DUAL, slew, pwl, load);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_delay (scm->PARAMS.GOOD, slew, load);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_delay (scm->PARAMS.FALS, slew, load);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_delay (scm->PARAMS.PATH, slew, load);
            break;
        case STM_MODSCM_CST:
            return stm_modscm_cst_delay (scm->PARAMS.CST);
    }
    return 0.0;
}

/****************************************************************************/
    
float stm_modscm_loadparam (timing_scm *scm, float slew, float load)
{
    float ci, cf, ti, tf;

    if (!scm)
        return 0.0;

    ci = load - 0.1 * load;
    cf = load + 0.1 * load;
    ti = stm_modscm_delay (scm, slew, NULL, ci);
    tf = stm_modscm_delay (scm, slew, NULL, cf);

    return (tf - ti) / (cf - ci);
}

/****************************************************************************/
    
float stm_modscm_slewparam (timing_scm *scm, float slew, float load)
{
    float si, sf, ti, tf;

    if (!scm)
        return 0.0;

    si = slew - 0.1 * slew;
    sf = slew + 0.1 * slew;
    ti = stm_modscm_delay (scm, si, NULL, load);
    tf = stm_modscm_delay (scm, sf, NULL, load);

    return (tf - ti) / (sf - si);
}

/****************************************************************************/

float stm_modscm_capaeq (timing_scm *scm, float slew, float r, float c1, float c2, char *signame )
{
    float c;
    float imax;
    float vth;
    float cconf;

    c = c1 + c2 ;

    vth   = stm_modscm_vth ( scm );
    imax  = stm_modscm_imax ( scm );
    cconf = stm_modscm_cconf ( scm, slew );

    switch (scm->TYPE) {
    
        case STM_MODSCM_DUAL:
            c = stm_modscm_dual_capaeq ( scm->PARAMS.DUAL, NULL, slew, r, c1, c2, vth, signame );
            break;
            
        case STM_MODSCM_GOOD:
        case STM_MODSCM_FALSE:
        case STM_MODSCM_PATH:
        case STM_MODSCM_CST:

            c = stm_capaeq (imax, r, c1 + cconf, c2, vth, signame);
            c = c - cconf;
            break;
            
    }

    return c ;
}

/****************************************************************************/
    
float stm_modscm_vf_input (timing_scm *scm)
{
    if (!scm)
        return -1.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_vf_input (scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_vf_input (scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_vf_input (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_vf_input (scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            return -1.0;
    }
    return -1.0;
}
/****************************************************************************/
    
float stm_modscm_vdd_input (timing_scm *scm)
{
    if (!scm)
        return -1.0;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_vdd_input (scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            return stm_modscm_good_vdd_input (scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            return stm_modscm_false_vdd_input (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            return stm_modscm_path_vdd_input (scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            return -1.0;
    }
    return -1.0;
}

