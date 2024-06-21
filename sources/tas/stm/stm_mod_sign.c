/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_sign.c                                              */
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

#ifdef NEWSIGN
float stm_mod_signature (timing_model *tmodel, char *s)
{
  float sign=0;
  if (tmodel) {
      strcpy(s,"");
      sign+=tmodel->VTH+tmodel->VDD+tmodel->VT+tmodel->VF;
      switch (tmodel->UTYPE) {
          case STM_MOD_MODTBL:
              sign+=stm_modtbl_signature (tmodel->UMODEL.TABLE, s);
              break;
          case STM_MOD_MODSCM:
              sign+=stm_modscm_signature (tmodel->UMODEL.SCM, s);
              break;
          case STM_MOD_MODPLN:
              sign+=stm_modpln_signature (tmodel->UMODEL.POLYNOM, s);
              break;
          default:
              avt_errmsg (STM_ERRMSG,"025", AVT_ERROR);
      }
  }
  return sign;
}
#else
void stm_mod_signature (timing_model *tmodel, char *s)
{
    if (tmodel) {
        sprintf (s, "vth_%.5g_", tmodel->VTH);
        sprintf (s + strlen (s), "vdd_%.5g_", tmodel->VDD);
        sprintf (s + strlen (s), "vt_%.5g_", tmodel->VT);
        sprintf (s + strlen (s), "vf_%.5g_", tmodel->VF);
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                stm_modtbl_signature (tmodel->UMODEL.TABLE, s);
                break;
            case STM_MOD_MODSCM:
                stm_modscm_signature (tmodel->UMODEL.SCM, s);
                break;
            case STM_MOD_MODPLN:
                stm_modpln_signature (tmodel->UMODEL.POLYNOM, s);
                break;
            default:
                avt_errmsg (STM_ERRMSG,"025", AVT_ERROR);
        }
    }
}
#endif
void stm_mod_change_params (timing_model *tmodel)
{
    float precision = 0.01;
    
    if (tmodel) {
#ifdef NEWSIGN
        tmodel->VTH = mbk_rounddouble(tmodel->VTH, STM_NEWSIGN_ROUND_PRECISION);
        tmodel->VDD = mbk_rounddouble(tmodel->VDD, STM_NEWSIGN_ROUND_PRECISION);
        tmodel->VT = mbk_rounddouble(tmodel->VT, STM_NEWSIGN_ROUND_PRECISION);
        tmodel->VF = mbk_rounddouble(tmodel->VF, STM_NEWSIGN_ROUND_PRECISION);
#endif
        switch (tmodel->UTYPE) {
            case STM_MOD_MODTBL:
                break;
            case STM_MOD_MODSCM:
                stm_modscm_change_params (tmodel->UMODEL.SCM);
                break;
            case STM_MOD_MODPLN:
                break;
            default:
                avt_errmsg (STM_ERRMSG,"025", AVT_ERROR);
        }
    }
}

#ifdef NEWSIGN
int stm_mod_same_params (timing_model *tmodel0, timing_model *tmodel1)
{
  if (tmodel0->UTYPE!=tmodel1->UTYPE
      || tmodel0->VTH!=tmodel1->VTH
      || tmodel0->VDD!=tmodel1->VDD
      || tmodel0->VT!=tmodel1->VT
      || tmodel0->VF!=tmodel1->VF) return 0;
  
  switch (tmodel0->UTYPE) {
           case STM_MOD_MODTBL:
               return 0;
           case STM_MOD_MODSCM:
               return stm_modscm_same_params (tmodel0->UMODEL.SCM, tmodel1->UMODEL.SCM);
           case STM_MOD_MODPLN:
               return 0;
           default:
               avt_errmsg (STM_ERRMSG,"025", AVT_ERROR);
       }
  return 0;
}
#endif
