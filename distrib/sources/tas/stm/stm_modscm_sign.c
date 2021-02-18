/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_sign.c                                           */
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
float stm_modscm_signature (timing_scm *scm, char *s)
{
  switch (scm->TYPE) {
      case STM_MODSCM_DUAL:
          return stm_modscm_dual_signature (scm->PARAMS.DUAL, s);
      case STM_MODSCM_GOOD:
          return stm_modscm_good_signature (scm->PARAMS.GOOD, s);
      case STM_MODSCM_FALSE:
          return stm_modscm_false_signature (scm->PARAMS.FALS, s);
      case STM_MODSCM_PATH:
          return stm_modscm_path_signature (scm->PARAMS.PATH, s);
      case STM_MODSCM_CST:
          return stm_modscm_cst_signature (scm->PARAMS.CST, s);
  }
  return 0;
}
#else
void stm_modscm_signature (timing_scm *scm, char *s)
{
    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            stm_modscm_dual_signature (scm->PARAMS.DUAL, s);
            break;
        case STM_MODSCM_GOOD:
            stm_modscm_good_signature (scm->PARAMS.GOOD, s);
            break;
        case STM_MODSCM_FALSE:
            stm_modscm_false_signature (scm->PARAMS.FALS, s);
            break;
        case STM_MODSCM_PATH:
            stm_modscm_path_signature (scm->PARAMS.PATH, s);
            break;
        case STM_MODSCM_CST:
            stm_modscm_cst_signature (scm->PARAMS.CST, s);
            break;
    }
}
#endif
void stm_modscm_change_params (timing_scm *scm)
{
    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            stm_modscm_dual_change_params (scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            stm_modscm_good_change_params (scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            stm_modscm_false_change_params (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            stm_modscm_path_change_params (scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            stm_modscm_cst_change_params (scm->PARAMS.CST);
            break;
    }
}

#ifdef NEWSIGN
int stm_modscm_same_params (timing_scm *scm0, timing_scm *scm1)
{
    switch (scm0->TYPE) {
        case STM_MODSCM_DUAL:
            return stm_modscm_dual_same_params (scm0->PARAMS.DUAL,scm1->PARAMS.DUAL);
        case STM_MODSCM_GOOD:
            return stm_modscm_good_same_params (scm0->PARAMS.GOOD,scm1->PARAMS.GOOD);
        case STM_MODSCM_FALSE:
            return stm_modscm_false_same_params (scm0->PARAMS.FALS,scm1->PARAMS.FALS);
        case STM_MODSCM_PATH:
            return stm_modscm_path_same_params (scm0->PARAMS.PATH,scm1->PARAMS.PATH);
        case STM_MODSCM_CST:
            return stm_modscm_cst_same_params (scm0->PARAMS.CST,scm1->PARAMS.CST);
    }
    return 0;
}
#endif
