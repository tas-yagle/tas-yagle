/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm.h                                                       */
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

/* Cell Interface */
#include "stm_cell.h"
#include "stm_cell_print.h"
#include "stm_name_interface.h"

/* High Level Timing Model */
#include "stm_mod.h"
#include "stm_mod_eval.h"
#include "stm_mod_sign.h"
#include "stm_mod_modif.h"
#include "stm_mod_meta.h"
#include "stm_mod_merge.h"
#include "stm_mod_print.h"
#include "stm_mod_name.h"

/* Polynom Table Timing Model */
#include "stm_modpln.h"
#include "stm_modpln_eval.h"
#include "stm_modpln_print.h"
#include "stm_modpln_sign.h"

/* Lookup Table Timing Model */
#include "stm_modtbl.h"
#include "stm_modtbl_eval.h"
#include "stm_modtbl_print.h"
#include "stm_modtbl_modif.h"
#include "stm_modtbl_templ.h"
#include "stm_modtbl_print.h"
#include "stm_modtbl_merge.h"
#include "stm_modtbl_meta.h"
#include "stm_modtbl_sign.h"

/* Short Channel Timing Model */
#include "stm_modscm.h"
#include "stm_modscm_dual.h"
#include "stm_modscm_good.h"
#include "stm_modscm_false.h"
#include "stm_modscm_path.h"
#include "stm_modscm_cst.h"
#include "stm_modscm_eval.h"
#include "stm_modscm_dual_eval.h"
#include "stm_modscm_good_eval.h"
#include "stm_modscm_false_eval.h"
#include "stm_modscm_path_eval.h"
#include "stm_modscm_cst_eval.h"
#include "stm_modscm_sign.h"
#include "stm_modscm_dual_sign.h"
#include "stm_modscm_good_sign.h"
#include "stm_modscm_false_sign.h"
#include "stm_modscm_path_sign.h"
#include "stm_modscm_cst_sign.h"
#include "stm_modscm_print.h"
#include "stm_modscm_dual_print.h"
#include "stm_modscm_good_print.h"
#include "stm_modscm_false_print.h"
#include "stm_modscm_path_print.h"
#include "stm_modscm_cst_print.h"

/* Slope PWTH Model */
#include "stm_pwl.h"
#include "stm_pwth.h"


/* IV Model */
#include "stm_modiv.h"
#include "stm_modiv_eval.h"
#include "stm_modiv_print.h"

/* FCT Model */
#include "stm_modfct_share.h"

/* Parser Driver */
#include "stm_parse.h"
#include "stm_drive.h"

/* Noise parameter */
#include "stm_noise.h"
#include "stm_noise_print.h"
#include "stm_noise_scr.h"
#include "stm_noise_scr_print.h"

/* Energy parameter */
#include "stm_energy.h"
#include "stm_energy_print.h"
#include "stm_energy_table.h"
#include "stm_energy_table_print.h"
#include "stm_energy_params.h"
#include "stm_energy_params_print.h"

/* Util */
#include "stm_env.h"
#include "stm_math.h"
#include "stm_string.h"
#include "stm_ht.h"
#include "stm_capa.h"

/* Simulator */
#include "stm_simu.h"
#include "stm_solver.h"

/* Curve */
#include "stm_curve.h"
#include "stm_curve_mcc.h"
#include "stm_curve_plot.h"

#include "stm_mod_display.h"

/* Cache */
#include "stm_cache_model.h"

