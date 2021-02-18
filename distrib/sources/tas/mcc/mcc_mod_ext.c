/*
 *  mcc_mod_ext.c
 *  avertec_distrib
 *
 *  Created by Anthony on 21/05/2008.
 *  Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include <dlfcn.h>

#include MCC_H

static mcc_modext *mcc_ext_model;

int
mcc_initparam_ext(mcc_modellist *ptmodel)
{
    static int  f=0;
    void *lib;
    char libname[256];
    char symname[256];
    void *symbol;

    if (!f) {

        /* Load shared library */
        sprintf(libname, "lib%s.so", V_STR_TAB[__MCC_EXTTECHNO].VALUE);
        printf("Opening dynamic library %s.\n", libname);
        lib = dlopen(libname, RTLD_NOW|RTLD_GLOBAL);
        if(!lib) {
            printf("%s\n", dlerror());
            exit(1);
        }

        /* Obtain EXT model */
        sprintf(symname, "p%s", V_STR_TAB[__MCC_EXTTECHNO].VALUE);
        symbol = dlsym(lib, symname);
        if(!symbol) {
            printf("can't find model in shared library! (%s)\n", dlerror());
            exit(1);
        }
        mcc_ext_model = *(mcc_modext **)symbol;
        f = 1;
    }
    
    mcc_ext_model->initparam(ptmodel);
    
    return 1;
}

void
mcc_clean_ext(mcc_modellist *ptmodel)
{
    mcc_ext_model->clean(ptmodel);
}

void 
mcc_calcQint_ext(mcc_modellist *ptmodel,
                 double L,
                 double W,
                 double temp,
                 double vgs,
                 double vbs,
                 double vds,
                 double *ptQg,
                 double *ptQs,
                 double *ptQd,
                 double *ptQb,
                 elp_lotrs_param *lotrsparam)
{
    mcc_ext_model->calcQint(ptmodel, L, W, temp, vgs,vbs,vds,ptQg,ptQs,ptQd,ptQb,lotrsparam);
}

/* Perimeter contribution of gate capacitance */
double 
mcc_calcCGP_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double vgx,
                double L,
                double W,
                double temp,
                double *ptQov)
{
    return mcc_ext_model->calcCGP(ptmodel, lotrsparam, vgx, L, W, temp, ptQov);
}

/* Surfacic gate-drain capacitance */
double 
mcc_calcCGD_ext(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vgs0,
                double vgs1,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam)
{
    return mcc_ext_model->calcCGD(ptmodel, L, W, temp, vgs0, vgs1, vbs, vds,lotrsparam);
}


/* Surface contribution of gate-drain capacitance */
double 
mcc_calcCGSI_ext(mcc_modellist *ptmodel,
                 double L,
                 double W,
                 double temp,
                 double vgs,
                 double vbs,
                 double vds,
                 elp_lotrs_param *lotrsparam)
{
    return mcc_ext_model->calcCGSI(ptmodel, L, W, temp, vgs, vbs, vds,lotrsparam);
}

double 
mcc_calcVTH_ext(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam)
{
    return mcc_ext_model->calcVTH(ptmodel, L, W, temp, vbs, vds,lotrsparam);
}

double 
mcc_calcIDS_ext(mcc_modellist *ptmodel,
                double vbs,
                double vgs,
                double vds,
                double W,
                double L,
                double Temp,
                elp_lotrs_param *lotrsparam)
{
    return mcc_ext_model->calcIDS(ptmodel, vbs, vgs, vds, W, L, Temp, lotrsparam);
}

/* Offset for effective drain channel junction width */
double mcc_calcDWCJ_ext( mcc_modellist *ptmodel, 
                         elp_lotrs_param *lotrsparam, 
                         double temp,
                         double L, 
                         double W
                       )
{
    return mcc_ext_model->calcDWCJ(ptmodel, lotrsparam, temp, L, W);
}

/* Area contribution of channel junction capacitance for drain */
double 
mcc_calcCDS_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx0,
                double vbx1,
                double L,
                double W)
{
    return mcc_ext_model->calcCDS(ptmodel, lotrsparam, temp, vbx0, vbx1, L, W);
}

/* Sidewall contribution of channel junction capacitance for drain */
double 
mcc_calcCDP_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx0,
                double vbx1,
                double L,
                double W)
{
    return mcc_ext_model->calcCDP(ptmodel, lotrsparam,temp, vbx0, vbx1, L, W);
}

/* Gate sidewall contribution of channel junction capacitance for drain */
double 
mcc_calcCDW_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx0,
                double vbx1,
                double L,
                double W)
{
    return mcc_ext_model->calcCDW(ptmodel, lotrsparam, temp, vbx0, vbx1, L, W);
}

/* Calculate effective Perimeter & Area if undefined */
void
mcc_calcPAfromgeomod_extmod(lotrs_list *ptlotrs,
                            mcc_modellist *ptmodel,
                            elp_lotrs_param *lotrsparam,
                            double *as,
                            double *ad,
                            double *ps,
                            double *pd)
{
    mcc_ext_model->calcPA(ptlotrs, ptmodel, lotrsparam, as, ad, ps, pd);
}
