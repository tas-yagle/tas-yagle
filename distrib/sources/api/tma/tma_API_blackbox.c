/****************************************************************************/
/*                                                                          */
/*                 Chaine de CAO & VLSI   Alliance                          */
/*                                                                          */
/*   Produit : TMA Version 1                                                */
/*   Fichier : tma_blackbox.c                                               */
/*                                                                          */
/*   (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                   */
/*   Tous droits reserves                                                   */
/*   Support : e-mail alliance-support@asim.lip6.fr                         */
/*                                                                          */
/*   Auteur(s) : Gilles AUGUSTINS                                           */
/*                                                                          */
/****************************************************************************/

#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include MSL_H
#include AVT_H
#include INF_H
#include ELP_H
#include CNS_H
#include YAG_H
#include TLC_H
#include TRC_H
#include STM_H
#include TTV_H
#include MCC_H
#include SIM_H
#include TAS_H
#include FCL_H
#include BEH_H
#include BHL_H
#include BEF_H
#include BVL_H
#include CBH_H
#include TUT_H
#include LIB_H
#include TLF_H
#include STB_H
#include TMA_H

#define API_USE_REAL_TYPES
#include "tma_API.h"
#include "ttv_API.h"

float Compute_Delay (ttvfig_list *fig, float slope, float load, ttvpath_list *path, char *max)
{
    char          start_tran, 
                  end_tran;
    ttvsig_list  *start_sig, 
                 *end_sig;
    char         *start_name, 
                 *end_name;
    chain_list   *res_path;
    ttvpath_list *pt_res_path;
    int           prop;
    float         delay;
    char          tran[8];

    start_tran = ttv_GetPathStartDirection (path);
    end_tran   = ttv_GetPathEndDirection (path);
    start_sig  = ttv_GetPathStartSignal (path);
    end_sig    =  ttv_GetPathEndSignal (path);
    start_name = ttv_GetSignalName (start_sig);
    end_name   = ttv_GetSignalName (end_sig);
    sprintf (tran, "%c%c", start_tran, end_tran);
    // no slope propagation         prop = 0
    // full slope propagation       prop = 1 
    // 1-stage slope propagation    prop = 2
    prop = 1;
    res_path = ttv_CharacPaths (fig, slope, start_name, end_name, tran, 1, "critic", "path", max, load, prop);
    pt_res_path = (ttvpath_list*)res_path->DATA;
    freechain (res_path);
    delay = ttv_GetPathRefDelay (pt_res_path);
    return delay;
}
/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */

chain_list *Global_Prop (ttvfig_list *fig, float *slope_tab, float *load_tab, ttvpath_list *path, char *max, int nb_slope, int nb_load)
{
    int i, j;
    float delay;

    for (i = 0; i < nb_slope; i++) 
        for (j = 0; j < nb_load; j++) {
            delay = Compute_Delay (fig, slope_tab[i], load_tab[j], path, max);
            fprintf (stdout, "delay (%g, %g) = %g\n", slope_tab[i], load_tab[j], delay); 
        }

    return NULL;
}
/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */

chain_list **extractSetupHold (float *slope_tab, int nb_slope, ttvfig_list *fig, char *clock, char *pin, char *mode)
{
    return NULL;
}

/* -------------------------------------------------------------------------------------------------- */

chain_list **extractAccess (float *slope_tab, float *load_tab, int nb_slope, int nb_load, ttvfig_list *fig, char *clock, char *pin, char *mode, char *minmax)
{
    return NULL;
}

/* -------------------------------------------------------------------------------------------------- */

chain_list **extractPropDelay (float *slope_tab, float *load_tab, int nb_slope, int nb_load, ttvfig_list *fig, char *input, char *output, char *mode, char *minmax)
{
    chain_list   *prop_rr = NULL,
                 *prop_rf = NULL,
                 *prop_fr = NULL,
                 *prop_ff = NULL;
    chain_list   *prop_list;
    ttvpath_list *pt_path;
    chain_list   *path, *path_list;
    ttvsig_list  *start_sig, *end_sig;
    char          start_tran, end_tran;
    chain_list  **clist = (chain_list**)malloc (4 * sizeof (chain_list*));
    
    if (!strcmp (minmax, "max")) 
        path_list = ttv_GetPaths (fig, input, output, "??", 1000, "critic", "path", "max");
    else 
        path_list = ttv_GetPaths (fig, input, output, "??", 1000, "critic", "path", "min");

    for (path = path_list; path; path = path->NEXT) {
        pt_path = (ttvpath_list*)path->DATA;
        start_sig  = ttv_GetPathStartSignal (pt_path);
        end_sig    = ttv_GetPathEndSignal (pt_path);
        start_tran = ttv_GetPathStartDirection (pt_path); 
        end_tran   = ttv_GetPathEndDirection (pt_path);

        prop_list = Global_Prop (fig, slope_tab, load_tab, pt_path, minmax, nb_slope, nb_load);
    
        if (start_tran == 'u') {
            if (end_tran == 'u') prop_rr = prop_list;
            if (end_tran == 'd') prop_rf = prop_list;
        } else if (start_tran == 'd') {
            if (end_tran == 'u') prop_fr = prop_list;
            if (end_tran == 'd') prop_ff = prop_list;
        }
    }

    clist[0] = prop_rr;
    clist[1] = prop_rf;
    clist[2] = prop_fr;
    clist[3] = prop_ff;

    return clist;
}

/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */

void tma_addSetupHold (float *slope_tab, int nb_slope, ttvfig_list *fig, ttvfig_list *bb, char *clock, char *input, char *mode) 
{
    chain_list **clist = NULL;
    chain_list *su_rr = NULL,
               *su_rf = NULL,
               *su_fr = NULL,
               *su_ff = NULL,
               *ho_rr = NULL,
               *ho_rf = NULL,
               *ho_fr = NULL,
               *ho_ff = NULL;

    fprintf (stdout, "SETUP/HOLD %s -> %s\n", clock, input);

    if ((clist = extractSetupHold (slope_tab, nb_slope, fig, clock, input, mode))) {
        su_rr = clist [0];
        su_rf = clist [1];
        ho_rr = clist [2];
        ho_rf = clist [3];
        su_fr = clist [4];
        su_ff = clist [5];
        ho_fr = clist [6];
        ho_ff = clist [7];
    }

    if (su_rr) freechain (su_rr);
    if (su_rf) freechain (su_rf);
    if (ho_rr) freechain (ho_rr);
    if (ho_rf) freechain (ho_rf);
    if (su_fr) freechain (su_fr);
    if (su_ff) freechain (su_ff);
    if (ho_fr) freechain (ho_fr);
    if (ho_ff) freechain (ho_ff);

    if (clist) free (clist);
}

/* -------------------------------------------------------------------------------------------------- */

void tma_addPropDelay (float *slope_tab, float *load_tab, int nb_slope, int nb_load, ttvfig_list *fig, ttvfig_list *bb, char *input, char *output, char *mode) 
{
    chain_list **clist = NULL;
    chain_list *prop_rr_min = NULL,
               *prop_rf_min = NULL,
               *prop_fr_min = NULL,
               *prop_ff_min = NULL;
    chain_list *prop_rr_max = NULL,
               *prop_rf_max = NULL,
               *prop_fr_max = NULL,
               *prop_ff_max = NULL;

    fprintf (stdout, "PROP %s -> %s\n", input, output);

    if ((clist = extractPropDelay (slope_tab, load_tab, nb_slope, nb_load, fig, input, output, mode, "min"))) {
        prop_rr_min = clist[0];
        prop_rf_min = clist[1];
        prop_fr_min = clist[2];
        prop_ff_min = clist[3];
    }

    if ((clist = extractPropDelay (slope_tab, load_tab, nb_slope, nb_load, fig, input, output, mode, "max"))) {
        prop_rr_max = clist[0];
        prop_rf_max = clist[1];
        prop_fr_max = clist[2];
        prop_ff_max = clist[3];
    }

    if (prop_rr_min) freechain (prop_rr_min);
    if (prop_rf_min) freechain (prop_rf_min);
    if (prop_fr_min) freechain (prop_fr_min);
    if (prop_ff_min) freechain (prop_ff_min);
    if (prop_rr_max) freechain (prop_rr_max);
    if (prop_rf_max) freechain (prop_rf_max);
    if (prop_fr_max) freechain (prop_fr_max);
    if (prop_ff_max) freechain (prop_ff_max);

    if (clist) free (clist);
}

/* -------------------------------------------------------------------------------------------------- */

void tma_addAccess (float *slope_tab, float *load_tab, int nb_slope, int nb_load, ttvfig_list *fig, ttvfig_list *bb, char *clock, char *output, char *mode) 
{
    chain_list **clist = NULL;
    chain_list *acs_rr_min = NULL,
               *acs_rf_min = NULL,
               *acs_fr_min = NULL,
               *acs_ff_min = NULL;
    chain_list *acs_rr_max = NULL,
               *acs_rf_max = NULL,
               *acs_fr_max = NULL,
               *acs_ff_max = NULL;

    fprintf (stdout, "ACCESS %s -> %s\n", clock, output);
    
    if ((clist = extractAccess (slope_tab, load_tab, nb_slope, nb_load, fig, clock, output, mode, "min"))) {
        acs_rr_min = clist[0];
        acs_rf_min = clist[1];
        acs_fr_min = clist[2];
        acs_ff_min = clist[3];
    }

    if ((clist = extractAccess (slope_tab, load_tab, nb_slope, nb_load, fig, clock, output, mode, "max"))) {
        acs_rr_max = clist[0];
        acs_rf_max = clist[1];
        acs_fr_max = clist[2];
        acs_ff_max = clist[3];
    }

    if (acs_rr_min) freechain (acs_rr_min);
    if (acs_rf_min) freechain (acs_rf_min);
    if (acs_fr_min) freechain (acs_fr_min);
    if (acs_ff_min) freechain (acs_ff_min);
    if (acs_rr_max) freechain (acs_rr_max);
    if (acs_rf_max) freechain (acs_rf_max);
    if (acs_fr_max) freechain (acs_fr_max);
    if (acs_ff_max) freechain (acs_ff_max);
    
    if (clist) free (clist);
}

/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------------------- */
ttvfig_list *tma_APIBlackBox (ttvfig_list *fig, char *suffix)
{
    int verbose = V_BOOL_TAB[__TMA_VERBOSE].VALUE;
    stbfig_list   *stbfig = NULL;
    ttvsig_list   *pt_pin;
    char          dir, *name, *figname, *bbname;
    chain_list    *pin_list, *pin,
                  *input_list = NULL,
                  *output_list = NULL,
                  *clock_list = NULL;
    chain_list    *input, *output, *clock;
    float          slope_unit, cap_unit;
    char          buf[4096];
    float        *slope_tab, *load_tab;
    int           nb_slope, nb_load;
    ttvfig_list  *blackbox;

    /* CONFIG REQUIRED HERE */
    slope_unit = 1e-12; 
    cap_unit = 1e-15;
    nb_slope = 3;
    nb_load  = 4;
    slope_tab = (float*)malloc (nb_slope * sizeof (float));
    load_tab  = (float*)malloc (nb_load * sizeof (float));
    load_tab[0] = 10 * cap_unit;
    load_tab[1] = 50 * cap_unit;
    load_tab[2] = 100 * cap_unit;
    slope_tab[0] = 100 * slope_unit;
    slope_tab[1] = 200 * slope_unit;
    slope_tab[2] = 300 * slope_unit;
    clock_list = addchain (clock_list, (void*)"ck");

    if (!fig) return NULL;

    stbfig = stb_getstbfig(fig);

    figname = fig->INFO->FIGNAME;

    avt_log( LOGTMA, 1, "timing figure is %s\n", figname );

    if (suffix) {
        sprintf (buf, "%s_%s", figname, suffix);
        bbname = namealloc (buf);
    } else
        bbname = figname;

    blackbox = tma_DupTtvFigHeader (bbname, fig, "*");
    stm_addcell (bbname);
    tma_DupConnectorList (blackbox, fig);

    pin_list = ttv_GetTimingSignalList (blackbox, "connector", "interface");
    for (pin = pin_list; pin; pin = pin->NEXT) {
        pt_pin = (ttvsig_list*)(pin->DATA);
        dir = ttv_GetConnectorDirection (pt_pin);
        if (dir == 'i') {
            name = ttv_GetSignalName (pt_pin);
            input_list = addchain (input_list, name);
        }
        if (dir == 'o' || dir == 'b' || dir == 't' || dir == 'z') {
            name = ttv_GetSignalName (pt_pin);
            output_list = addchain (output_list, name);
        }
    }

    for (input = input_list; input; input = input->NEXT) {
        for (clock = clock_list; clock; clock = clock->NEXT)
            tma_addSetupHold (slope_tab, nb_slope, fig, blackbox, (char*)clock->DATA, (char*)input->DATA, "debug");
                               
        for (output = output_list; output; output = output->NEXT)
            tma_addPropDelay (slope_tab, load_tab, nb_slope, nb_load, fig, blackbox, (char*)input->DATA, (char*)output->DATA, "debug");
    }

    for (output = output_list; output; output = output->NEXT)
        for (clock = clock_list; clock; clock = clock->NEXT)
            tma_addAccess (slope_tab, load_tab, nb_slope, nb_load, fig, blackbox, (char*)clock->DATA, (char*)output->DATA, "debug");

    freechain (clock_list);
    freechain (input_list);
    freechain (output_list);
    free (slope_tab);
    free (load_tab);
                              
    return blackbox;
}

void tma_SetMaxCapacitance(ttvfig_list *bbox, char *name, double value)
{
  chain_list *cl;
  ttvsig_list *tvs;
  ptype_list *pt;

  if ((cl=ttv_GetMatchingSignal(bbox, name, "connector"))==NULL)
     avt_errmsg(TMA_API_ERRMSG, "002", AVT_WARNING, name);
  
  while (cl!=NULL)
   {
     tvs=(chain_list *)cl->DATA;
     if ((pt=getptype(tvs->USER, LIB_MAX_CAPACITANCE))==NULL)
       pt=tvs->USER=addptype(tvs->USER, LIB_MAX_CAPACITANCE, NULL);
     *(float *)&pt->DATA=value;
     cl=delchain(cl,cl);
   }
}

