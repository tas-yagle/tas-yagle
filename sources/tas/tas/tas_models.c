/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 1                                               */
/*    Fichier : tas_models.c                                                */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

#include "tas.h"

#include <stdarg.h>


void tas_transformFCTmodels (ttvfig_list *ttvfig)
{
    ptype_list    *ptype;
    stm_carac_values    *cvalues = NULL;
    int            i;
    ttvline_list  *line;
    ttvlbloc_list *bloc;
    ttvsig_list   *sigroot, *signode;
    timing_model  *mdmax, *mfmax, *mdmin, *mfmin;
    chain_list    *ch, *fct_lines = NULL;
    int            slew, load;
    double         inslews[128], outloads[128];
    float          tinslews[128], toutloads[128];
//    double         ravg;
    long            n_inslews, n_outloads;
    char           newname[1024];
    char           event_root, event_node;

       
//    fprintf (stdout, "DBLOC\n");
    for(bloc = ttvfig->DBLOC; bloc; bloc = bloc->NEXT) {
        for(i = 0 ; i < TTV_MAX_LBLOC ; i++) {
            line = bloc->LINE + i ;
            if ((line->TYPE & TTV_LINE_FR) == TTV_LINE_FR) continue ;
//            fprintf (stdout, "-----> line %s -> %s\n", line->NODE->ROOT->NAME, line->ROOT->ROOT->NAME); 
            if ((mdmax = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMAX))) {
                if (mdmax->UTYPE == STM_MOD_MODFCT) {
                    fct_lines = addchain (fct_lines, line);
//                    fprintf (stdout, "     FCT model %s \n", mdmax->NAME);
                }
            }
        }
    }

    for (ch = fct_lines; ch; ch = ch->NEXT) {
        line = (ttvline_list*)ch->DATA;
        mdmax = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMAX);
        
        // Get the Slew and Load Axis
        /*
        stm_getAxis(mdmax, tinslews, &n_inslews, toutloads, &n_outloads);
        for (i = 0; i < n_inslews; i ++)
          inslews[i]     = tinslews[i];
        for (i = 0; i < n_outloads; i ++)
          outloads[i]    = toutloads[i];

        */
        if ((ptype = getptype (mdmax->USER, STM_CARAC_VALUES)))
           cvalues = (stm_carac_values*)ptype->DATA;
        
        if (cvalues) {
           n_inslews = stm_genslewaxis (inslews, cvalues->SLEW_MIN, cvalues->SLEW_MAX);
           n_outloads = stm_genloadaxis (outloads, cvalues->LOAD_MIN, cvalues->LOAD_MAX);
        }
        else {
           fprintf (stderr, " - warning [tas_transformFCTmodels]: no characterization values\n");
           n_inslews = 0;
           n_outloads = 0;
        }

        // Duplicate models if needed (same models)
        if (line->MDMAX == line->MFMAX) {
            mfmax = stm_mod_duplicate (NULL, mdmax);
            sprintf (newname, "%s_slew_max", line->MDMAX);
            line->MFMAX = stm_storemodel (ttvfig->INFO->FIGNAME, newname, mfmax, 1);
        }
        else
            mfmax = stm_getmodel (ttvfig->INFO->FIGNAME, line->MFMAX);

        if (line->MDMIN && line->MFMIN) {
            if (line->MDMIN == line->MDMAX) {
                sprintf (newname, "%s_delay_min", line->MDMAX);
                mdmin = stm_mod_duplicate (NULL, mdmax);
                line->MDMIN = stm_storemodel (ttvfig->INFO->FIGNAME, newname, mdmin, 1);
            }
            else
                mdmin = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMIN);
            if (line->MFMIN == line->MDMAX) {
                sprintf (newname, "%s_slew_min", line->MDMAX);
                mfmin = stm_mod_duplicate (NULL, mdmax);
                line->MFMIN = stm_storemodel (ttvfig->INFO->FIGNAME, newname, mfmin, 1);
            }
            else
                mfmin = stm_getmodel (ttvfig->INFO->FIGNAME, line->MFMIN);
        }
        else {
            mdmin = NULL;
            mfmin = NULL;
        }

        // Compute the dimension of the model 
        sigroot = line->ROOT->ROOT;
        signode = line->NODE->ROOT;
        slew = 0;
        load = 0;
        if (((signode->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((signode->TYPE & TTV_SIG_N) == TTV_SIG_N))
            slew = 1;
        if (((sigroot->TYPE & TTV_SIG_C) == TTV_SIG_C) || ((sigroot->TYPE & TTV_SIG_N) == TTV_SIG_N)) {
            load = 1;
            //ravg = TAS_CONTEXT->FRONT_CON / line->ROOT->FIND->OUTLINE->ROOT->ROOT->CAPA;
            //outloads = tas_DynamicCapas (ravg, TAS_CONTEXT->CARAC_VALUES);
            //n_outloads = TAS_CONTEXT->CARAC_VALUES * 2 + 1;
        }

        // Transform models
        if (slew && load) { // 2D-SLEW/LOAD
            avt_log (LOGTAS, 1, "Transform '%s' to 2D-SLEW/LOAD model\n", mdmax->NAME);
            stm_modtbl_morph2_delay2D_fstm (mdmax, inslews, n_inslews, outloads, n_outloads, 0);
            avt_log (LOGTAS, 1, "Transform '%s' to 2D-SLEW/LOAD model\n", mfmax->NAME);
            stm_modtbl_morph2_slew2D_fstm (mfmax, inslews, n_inslews, outloads, n_outloads, 0);
            if (mdmin) {
                avt_log (LOGTAS, 1, "Transform '%s' to 2D-SLEW/LOAD model\n", mdmin->NAME);
                stm_modtbl_morph2_delay2D_fstm (mdmin, inslews, n_inslews, outloads, n_outloads, 0);
            }
            if (mfmin) {
                avt_log (LOGTAS, 1, "Transform '%s' to 2D-SLEW/LOAD model\n", mfmin->NAME);
                stm_modtbl_morph2_slew2D_fstm (mfmin, inslews, n_inslews, outloads, n_outloads, 0);
            }
        }
        else if (slew) { // 1D-SLEW
            avt_log (LOGTAS, 1, "Transform '%s' to 1D-SLEW model\n", mdmax->NAME);
            stm_modtbl_morph2_delay1Dloadfix_fstm (mdmax, inslews, n_inslews, sigroot->CAPA);
            avt_log (LOGTAS, 1, "Transform '%s' to 1D-SLEW model\n", mfmax->NAME);
            stm_modtbl_morph2_slew1Dloadfix_fstm (mfmax, inslews, n_inslews, sigroot->CAPA);
            if (mdmin) {
                avt_log (LOGTAS, 1, "Transform '%s' to 1D-SLEW model\n", mdmin->NAME);
                stm_modtbl_morph2_delay1Dloadfix_fstm (mdmin, inslews, n_inslews, sigroot->CAPA);
            }
            if (mfmin) {
                avt_log (LOGTAS, 1, "Transform '%s' to 1D-SLEW model\n", mfmin->NAME);
                stm_modtbl_morph2_slew1Dloadfix_fstm (mfmin, inslews, n_inslews, sigroot->CAPA);
            }
        }
        else if (load) { // 1D-LOAD
            avt_log (LOGTAS,1, "Transform '%s' to 1D-LOAD model\n", mdmax->NAME);
            stm_modtbl_morph2_delay1Dslewfix_fstm (mdmax, outloads, n_outloads, TAS_CONTEXT->FRONT_CON, 0);
            avt_log (LOGTAS,1, "Transform '%s' to 1D-LOAD model\n", mfmax->NAME);
            stm_modtbl_morph2_slew1Dslewfix_fstm (mfmax, outloads, n_outloads, TAS_CONTEXT->FRONT_CON, 0);
            if (mdmin) {
                avt_log (LOGTAS, 1, "Transform '%s' to 1D-LOAD model\n", mdmin->NAME);
                stm_modtbl_morph2_delay1Dslewfix_fstm (mdmin, outloads, n_outloads, TAS_CONTEXT->FRONT_CON, 0);
            }
            if (mfmin) {
                avt_log (LOGTAS, 1, "Transform '%s' to 1D-LOAD model\n", mfmin->NAME);
                stm_modtbl_morph2_slew1Dslewfix_fstm (mfmin, outloads, n_outloads, TAS_CONTEXT->FRONT_CON, 0);
            }
        }

    }

    // Trace mode
    for (ch = fct_lines; ch; ch = ch->NEXT) {
        line = (ttvline_list*)ch->DATA;
        avt_log (LOGTAS, 1, "**************************************************\n");
        if ((line->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) event_node = 'U';
        else event_node = 'D';
        if ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) event_root = 'U';
        else event_root = 'D';
        avt_log (LOGTAS, 1, "LINE: %s %c -> %s %c\n", line->NODE->ROOT->NAME, event_node, line->ROOT->ROOT->NAME, event_root);
        avt_log (LOGTAS, 1, "\n--- MDMAX ---\n");
        mdmax = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMAX);
        if (mdmax->UTYPE == STM_MOD_MODTBL)
            stm_mod_display("tas_model",mdmax);
        avt_log (LOGTAS, 1, "\n--- MFMAX ---\n");
        mfmax = stm_getmodel (ttvfig->INFO->FIGNAME, line->MFMAX);
        if (mfmax->UTYPE == STM_MOD_MODTBL)
            stm_mod_display("tas_model",mfmax);
        avt_log (LOGTAS, 1, "\n--- MDMIN ---\n");
        mdmin = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMIN);
        if (mdmin->UTYPE == STM_MOD_MODTBL)
            stm_mod_display("tas_model",mdmin);
        avt_log (LOGTAS, 1, "\n--- MFMIN ---\n");
        mfmin = stm_getmodel (ttvfig->INFO->FIGNAME, line->MFMIN);
        if (mfmin->UTYPE == STM_MOD_MODTBL)
            stm_mod_display("tas_model",mfmin);
    }
}
