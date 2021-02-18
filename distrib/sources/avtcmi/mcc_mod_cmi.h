/*
 * mcc_mod_cmi.h
 * avertec_distrib
 *
 * Created by Anthony on 29/04/2008.
 * Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include MCC_H

void mcc_calcPAfromgeomod_bsim4(lotrs_list *lotrs, mcc_modellist *model, elp_lotrs_param *lotrsparam, double *as, double *ad, double *ps, double *pd);
int mcc_initparam_cmi(mcc_modellist *ptmodel);
void mcc_clean_cmi(mcc_modellist *ptmodel);

void 
mcc_calcQint_cmi(mcc_modellist *ptmodel,
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
                 elp_lotrs_param *lotrsparam
);

double 
mcc_calcCGP_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double vgx,
                double L,
                double W,
                double temp,
                double *ptQov
);

double 
mcc_calcCGD_cmi(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vgs0,
                double vgs1,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam
);

double 
mcc_calcCGSI_cmi(mcc_modellist *ptmodel,
                 double L,
                 double W,
                 double temp,
                 double vgs,
                 double vbs,
                 double vds,
                 elp_lotrs_param *lotrsparam
);

double 
mcc_calcVTH_cmi(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam
);

double 
mcc_calcIDS_cmi(mcc_modellist *ptmodel,
                double vbs,
                double vgs,
                double vds,
                double W,
                double L,
                double Temp,
                elp_lotrs_param *lotrsparam
);

double mcc_calcDWCJ_cmi(mcc_modellist *ptmodel, 
                         elp_lotrs_param *lotrsparam, 
                         double temp,
                         double L, 
                         double W
);

double 
mcc_calcCDS_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx1,
                double vbx2,
                double L,
                double W
);

double 
mcc_calcCDP_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx1,
                double vbx2,
                double L,
                double W
);

double 
mcc_calcCDW_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx1,
                double vbx2,
                double L,
                double W
);

void
mcc_calcPA_cmi(lotrs_list *ptlotrs,
                            mcc_modellist *ptmodel,
                            elp_lotrs_param *lotrsparam,
                            double *as,
                            double *ad,
                            double *ps,
                            double *pd
);
