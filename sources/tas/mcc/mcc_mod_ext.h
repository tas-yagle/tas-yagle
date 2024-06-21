/*
 *  mcc_mod_ext.h
 *  avertec_distrib
 *
 *  Created by Anthony on 21/05/2008.
 *  Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include MCC_H

int             mcc_initparam_ext(mcc_modellist *ptmodel);
void            mcc_clean_ext(mcc_modellist *ptmodel);

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
                 elp_lotrs_param *lotrsparam
);

double 
mcc_calcCGP_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double vgx,
                double L,
                double W,
                double temp,
                double *ptQov
);

double 
mcc_calcCGD_ext(mcc_modellist *ptmodel,
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
mcc_calcCGSI_ext(mcc_modellist *ptmodel,
                 double L,
                 double W,
                 double temp,
                 double vgs,
                 double vbs,
                 double vds,
                 elp_lotrs_param *lotrsparam
);

double 
mcc_calcVTH_ext(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam
);

double 
mcc_calcIDS_ext(mcc_modellist *ptmodel,
                double vbs,
                double vgs,
                double vds,
                double W,
                double L,
                double Temp,
                elp_lotrs_param *lotrsparam
);

double mcc_calcDWCJ_ext( mcc_modellist *ptmodel, 
                         elp_lotrs_param *lotrsparam, 
                         double temp,
                         double L, 
                         double W
);

double 
mcc_calcCDS_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx1,
                double vbx2,
                double L,
                double W
);

double 
mcc_calcCDP_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx1,
                double vbx2,
                double L,
                double W
);

double 
mcc_calcCDW_ext(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx1,
                double vbx2,
                double L,
                double W
);

void mcc_calcPAfromgeomod_extmod( lotrs_list      *lotrs,
                                 mcc_modellist   *model,
                                 elp_lotrs_param *lotrsparam,
                                 double          *as,
                                 double          *ad,
                                 double          *ps,
                                 double          *pd
                               );
