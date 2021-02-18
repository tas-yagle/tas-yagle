/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_noise.c                                                  */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                Marc   Kuoch                                               */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"

chain_list *SIM_HEAD_NOISE = NULL; // init for each action !!!!


/*---------------------------------------------------------------------------*/

sim_noise *sim_noise_extract (sim_model *model,char type, char *name, 
                              SIM_FLOAT vthnoise,SIM_FLOAT tinit, SIM_FLOAT tfinal)
{
    SIM_FLOAT  step,vthl,vthh;
    SIM_FLOAT  tc = 1.0;         // temps de commutation
    SIM_FLOAT  peak;
    SIM_FLOAT *tab,slope,vdd;
    char      *md_name;
    char       event,noisetype;
    int        add_in_list = 1,ind1 = -1.0;
    int        ind2 = -1.0;
    int        indice,indice_peak,valmin = -1;
    char       goodevent = 'X';
    sim_measure        *m  = NULL;
    sim_measure_detail *md = NULL;
    sim_noise *noise,*ptnoise = NULL;

    vthl = sim_parameter_get_slopeVTHL (model);
    vthh = sim_parameter_get_slopeVTHH (model);
    step = sim_parameter_get_tran_step (model);
    vdd  = sim_parameter_get_alim (model, 'i');
    indice = (int)((tinit / step) + 0.5);

    m = sim_measure_get ( model,
                          name,
                          sim_gettype(type), 
                          SIM_MEASURE_VOLTAGE
                        );
    if (!m) return NULL;
    noisetype = sim_parameter_get_noise_type (model);
    while ((md = sim_measure_detail_scan(m,md))) {
        md_name = sim_measure_detail_get_name(md);
        tab = sim_measure_detail_get_data( md );
        if (!tab) return NULL;
        if ((noise = sim_noise_get (md_name,noisetype)) != NULL) {
            add_in_list = 0;
            ptnoise = noise;
        }
        else
            ptnoise = sim_noise_add (ptnoise,name,md_name);
        while (tc > 0) {
            sim_get_delay_slope (model,tab,&tc,&slope,&indice,&event,
                                 vthnoise,vthl,vthh,tfinal);
            if (tc > 0) {
                if ((valmin == -1) && (event == SIM_FALL)) {
                    goodevent = event;
                    valmin = 1;
                }
                else if ((valmin == -1) && (event == SIM_RISE)) {
                    goodevent = event;
                    valmin = 0;
                }
                if ((ind1 < 0) && (event == goodevent))
                    ind1 = indice;
                else if (ind1 > 0) {
                    ind2 = indice;
                    if (valmin == 1)
                        peak = sim_find_vmin (tab,ind1,ind2,&indice_peak);
                    else
                        peak = sim_find_vmax (tab,ind1,ind2,&indice_peak);
                    sim_noise_set_vth (ptnoise,vthnoise*vdd);
                    sim_noise_set_tab (ptnoise,tab);
                    sim_noise_set_idxc (ptnoise, ind1);
                    sim_noise_set_idxc (ptnoise, ind2);
                    sim_noise_set_idxpeak (ptnoise, indice_peak);
                    ind1 = -1.0;
                    ind2 = -1.0;
                }
            }
        }
    }
    if (ptnoise) {
        ptnoise->IDXPEAK = reverse (ptnoise->IDXPEAK);
        ptnoise->IDXC = reverse (ptnoise->IDXC);
    }
    if (add_in_list)
        SIM_HEAD_NOISE = addchain (SIM_HEAD_NOISE,ptnoise);

    return ptnoise;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  Fonctions d'acces a la structure sim_noise                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
sim_noise *sim_noise_add (sim_noise *head_noise,char *name,char *nodename)
{
    sim_noise *pt_noise = (sim_noise*)mbkalloc (sizeof (struct sim_noise));

    pt_noise->NEXT     = NULL; 
    pt_noise->NAME     = name;
    pt_noise->NODENAME = strdup(nodename);
    pt_noise->VTHNOISE = 0.0;
    pt_noise->TAB      = NULL;
    pt_noise->IDXPEAK  = NULL;
    pt_noise->IDXC     = NULL;

    if (head_noise)
        pt_noise->NEXT = head_noise;
    
    return pt_noise;
}

/*---------------------------------------------------------------------------*/

void sim_noise_set_vth (sim_noise *noise, SIM_FLOAT vthn)
{
    if (noise)
        noise->VTHNOISE = vthn;
}

/*---------------------------------------------------------------------------*/

void sim_noise_set_tab (sim_noise *noise, SIM_FLOAT *tab)
{
    if (noise)
        noise->TAB = tab;
}

/*---------------------------------------------------------------------------*/

void sim_noise_set_idxpeak (sim_noise *noise, int peakindex)
{
    if (noise)
        noise->IDXPEAK = addchain (noise->IDXPEAK,(void*)(long)peakindex); 
}

/*---------------------------------------------------------------------------*/

void sim_noise_set_idxc (sim_noise *noise, int cindex)
{
    if (noise)
        noise->IDXC = addchain (noise->IDXC,(void*)(long)cindex); 
}

/*---------------------------------------------------------------------------*/

sim_noise *sim_noise_get (char *name,char noisetype)
{
    chain_list *chain;
    SIM_FLOAT   min,min_noise = 0.0;
    SIM_FLOAT   max,max_noise = 0.0;
    sim_noise  *noise = NULL,*ptnoise;
    sim_noise  *noisemin = NULL,
               *noisemax = NULL,
               *res = NULL;

    for (chain = SIM_HEAD_NOISE ; chain ; chain = chain->NEXT) {
        ptnoise = (sim_noise*)chain->DATA;
        if (!strcasecmp(name,ptnoise->NAME) ||
            !strcasecmp(name,ptnoise->NODENAME)) {
            noise = ptnoise;
            break;
        }
    }
    if (noise) {
        min_noise = sim_noise_get_min_peak_value (noise);
        max_noise = sim_noise_get_max_peak_value (noise);
    }
    for (ptnoise = noise ; ptnoise ; ptnoise = ptnoise->NEXT) {
        min = sim_noise_get_min_peak_value (ptnoise);
        max = sim_noise_get_max_peak_value (ptnoise);
        if (min <= min_noise) {
            min_noise = min;
            noisemin = ptnoise;
        }
        if (max >= max_noise) {
            max_noise = max;
            noisemax = ptnoise;
        }
    }
    if (noisetype == SIM_MIN)
        res = noisemin;
    else if (noisetype == SIM_MAX)
        res = noisemax;

    return res;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_vth (sim_noise *noise)
{
    if (noise)
        return noise->VTHNOISE;
    else
        return 0.0;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT *sim_noise_get_tab (sim_noise *noise)
{
    if (noise)
        return noise->TAB;
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

chain_list *sim_noise_get_idxpeak (sim_noise *noise)
{
    if (noise) {
        return noise->IDXPEAK;
    }
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

chain_list *sim_noise_get_idxc (sim_noise *noise)
{
    if (noise) {
        return noise->IDXC;
    }
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

chain_list *sim_noise_get_vpeak (sim_noise *noise)
{
    if (noise)
        return noise->IDXPEAK; 
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

void sim_noise_free (sim_noise *noise)
{
    if (noise) {
        mbkfree(noise->NODENAME);
        freechain (noise->IDXPEAK);
        freechain (noise->IDXC);
        mbkfree(noise);
    }
}

/*---------------------------------------------------------------------------*/

void sim_noise_free_all (void)
{
    sim_noise *pt_noise;
    chain_list *chain;

    for (chain = SIM_HEAD_NOISE ; chain ; chain = chain->NEXT) {
        pt_noise = (sim_noise*)chain->DATA;
        sim_noise_free (pt_noise);
    }
    if (SIM_HEAD_NOISE) {
        freechain (SIM_HEAD_NOISE);
        SIM_HEAD_NOISE = NULL;
    }
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_time_byindex (sim_model *model,int index)
{
    SIM_FLOAT  res = 0.0;
    SIM_FLOAT  step;

    if (model) {
        step = sim_parameter_get_tran_step (model);
        res = step*index;
    }
    return res;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_volt_byindex (sim_noise *noise,int index)
{
    SIM_FLOAT  res = 0.0;
    SIM_FLOAT *tab;

    if (noise) {
        tab = sim_noise_get_tab (noise);
        if (tab)
            res = tab[index];
    }
    return res;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_min_peak_value (sim_noise *noise)
{
    SIM_FLOAT  minpeak = 0.0, cur_peak;
    SIM_FLOAT  vnoise;
    SIM_FLOAT  dV,old_dV = 10e10;
    chain_list *peak,*ch;

    if (noise) {
        peak   = sim_noise_get_idxpeak (noise);
        vnoise = sim_noise_get_vth (noise);
        for (ch = peak ; ch ; ch = ch->NEXT) {
            cur_peak = sim_noise_get_volt_byindex (noise,(int)(long)ch->DATA);
            dV = fabs (cur_peak - vnoise);
            if (dV <= old_dV) {
                minpeak = cur_peak;
                old_dV = dV;
            }
        }
    }
    return minpeak;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_max_peak_value (sim_noise *noise)
{
    SIM_FLOAT  maxpeak = 0.0, cur_peak;
    SIM_FLOAT  vnoise;
    SIM_FLOAT  dV,old_dV = 0.0;
    chain_list *peak,*ch;

    if (noise) {
        peak   = sim_noise_get_idxpeak (noise);
        vnoise = sim_noise_get_vth (noise);
        for (ch = peak ; ch ; ch = ch->NEXT) {
            cur_peak = sim_noise_get_volt_byindex (noise,(int)(long)ch->DATA);
            dV = fabs (cur_peak - vnoise);
            if (dV >= old_dV) {
                maxpeak = cur_peak;
                old_dV = dV;
            }
        }
    }
    return maxpeak;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_min_peak_time (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT  minpeak = 0.0, cur_peak;
    SIM_FLOAT  vnoise;
    SIM_FLOAT  dV,old_dV = 10e10;
    chain_list *peak,*ch;

    if (noise) {
        peak   = sim_noise_get_idxpeak (noise);
        vnoise = sim_noise_get_vth (noise);
        for (ch = peak ; ch ; ch = ch->NEXT) {
            cur_peak = sim_noise_get_time_byindex (model,(int)(long)ch->DATA);
            dV = fabs (cur_peak - vnoise);
            if (dV <= old_dV) {
                minpeak = cur_peak;
                old_dV = dV;
            }
        }
    }
    return minpeak;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_max_peak_time (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT  maxpeak = 0.0, cur_peak;
    SIM_FLOAT  vnoise;
    SIM_FLOAT  dV,old_dV = 0.0;
    chain_list *peak,*ch;

    if (noise) {
        peak   = sim_noise_get_idxpeak (noise);
        vnoise = sim_noise_get_vth (noise);
        for (ch = peak ; ch ; ch = ch->NEXT) {
            cur_peak = sim_noise_get_time_byindex (model,(int)(long)ch->DATA);
            dV = fabs (cur_peak - vnoise);
            if (dV >= old_dV) {
                maxpeak = cur_peak;
                old_dV = dV;
            }
        }
    }
    return maxpeak;
}

/*---------------------------------------------------------------------------*/

int sim_noise_get_max_peak_idx (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT  cur_peak;
    SIM_FLOAT  vnoise;
    SIM_FLOAT  dV,old_dV = 0.0;
    chain_list *peak,*ch;
    int         idx;

    if (noise) {
        peak   = sim_noise_get_idxpeak (noise);
        vnoise = sim_noise_get_vth (noise);
        for (ch = peak ; ch ; ch = ch->NEXT) {
            cur_peak = sim_noise_get_time_byindex (model,(int)(long)ch->DATA);
            dV = fabs (cur_peak - vnoise);
            if (dV >= old_dV) {
                idx = (int)(long)ch->DATA;
                old_dV = dV;
            }
        }
    }
    return idx;
}

/*---------------------------------------------------------------------------*/

int sim_noise_get_min_peak_idx (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT  cur_peak;
    SIM_FLOAT  vnoise;
    SIM_FLOAT  dV,old_dV = 10.0e10;
    chain_list *peak,*ch;
    int         idx;

    if (noise) {
        peak   = sim_noise_get_idxpeak (noise);
        vnoise = sim_noise_get_vth (noise);
        for (ch = peak ; ch ; ch = ch->NEXT) {
            cur_peak = sim_noise_get_time_byindex (model,(int)(long)ch->DATA);
            dV = fabs (cur_peak - vnoise);
            if (dV <= old_dV) {
                idx = (int)(long)ch->DATA;
                old_dV = dV;
            }
        }
    }
    return idx;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_min_ci_before_peak (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT instant = 0.0;
    int       idx = 0;

    idx = sim_noise_get_min_peak_idx (model,noise);
    instant = sim_noise_get_time_before_peak (model,noise,idx);
    return instant;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_max_ci_before_peak (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT instant = 0.0;
    int       idx = 0;

    idx = sim_noise_get_max_peak_idx (model,noise);
    instant = sim_noise_get_time_before_peak (model,noise,idx);
    return instant;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_min_ci_after_peak (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT instant = 0.0;
    int       idx = 0;

    idx = sim_noise_get_min_peak_idx (model,noise);
    instant = sim_noise_get_time_after_peak (model,noise,idx);
    return instant;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_max_ci_after_peak (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT instant = 0.0;
    int       idx = 0;

    idx = sim_noise_get_max_peak_idx (model,noise);
    instant = sim_noise_get_time_after_peak (model,noise,idx);
    return instant;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_min_peak_duration (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT duration = 0.0;
    int       idx = 0;

    idx = sim_noise_get_min_peak_idx (model,noise);
    duration = sim_noise_get_peak_duration (model,noise,idx);
    return duration;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_max_peak_duration (sim_model *model,sim_noise *noise)
{
    SIM_FLOAT duration = 0.0;
    int       idx = 0;

    idx = sim_noise_get_max_peak_idx (model,noise);
    duration = sim_noise_get_peak_duration (model,noise,idx);
    return duration;
}

/*---------------------------------------------------------------------------*/

void sim_noise_get_idx_around_peak (sim_noise *noise,
                                    int peak_indice,
                                    int *before_peak,
                                    int *after_peak)
{
    chain_list *commut_instant,*ci;

    if (noise) {
        commut_instant = sim_noise_get_idxc (noise);
        for (ci = commut_instant ; ci ; ci = ci->NEXT) {
            if ((int)(long)ci->DATA <= peak_indice) {
                if ((ci->NEXT) && ((int)(long)ci->NEXT->DATA >= peak_indice)) {
                    if (before_peak != NULL)
                        *before_peak = (int)(long)ci->DATA;
                    if (after_peak != NULL)
                        *after_peak  = (int)(long)ci->NEXT->DATA;
                    break;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_time_before_peak (sim_model *model,
                                          sim_noise *noise, 
                                          int peak_indice)
{
    SIM_FLOAT instant = 0.0;
    int       before_indice = -1;

    sim_noise_get_idx_around_peak (noise,peak_indice,&before_indice,NULL);
    if (before_indice > 0)
        instant = sim_noise_get_time_byindex (model,before_indice);
    return instant;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_time_after_peak (sim_model *model,
                                         sim_noise *noise, 
                                         int peak_indice)
{
    SIM_FLOAT instant = 0.0;
    int       after_indice = -1;

    sim_noise_get_idx_around_peak (noise,peak_indice,NULL,&after_indice);
    if (after_indice > 0)
        instant = sim_noise_get_time_byindex (model,after_indice);
    return instant;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_noise_get_peak_duration (sim_model *model,
                                       sim_noise *noise, 
                                       int peak_indice)
{
    SIM_FLOAT ti = 0.0;
    SIM_FLOAT tf = 0.0;
    int       after  = -1;
    int       before = -1;

    sim_noise_get_idx_around_peak (noise,peak_indice,&before,&after);
    if ((after > 0) && (before > 0)) {
        tf = sim_noise_get_time_byindex (model,after);
        ti = sim_noise_get_time_byindex (model,before);
    }
    return tf-ti;
}
