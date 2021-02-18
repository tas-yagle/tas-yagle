/*
 * mcc_mod_cmi.c
 * avertec_distrib
 *
 * Created by Anthony on 29/04/2008.
 * Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include "mcc_mod_cmi_interface.h"
#include "mcc_mod_cmi.h"

static int CMI_DEBUG;

static mcc_modext AVTCMI = {
    mcc_initparam_cmi,
    mcc_clean_cmi,
    mcc_calcQint_cmi,
    mcc_calcCGP_cmi,
    mcc_calcCGD_cmi,
    mcc_calcCGSI_cmi,
    mcc_calcVTH_cmi,
    mcc_calcIDS_cmi,
    mcc_calcDWCJ_cmi,
    mcc_calcCDS_cmi,
    mcc_calcCDP_cmi,
    mcc_calcCDW_cmi,
    mcc_calcPA_cmi
};

/* exported */
mcc_modext *pAVTCMI = &AVTCMI;

int
mcc_initparam_cmi(mcc_modellist *ptmodel)
{
    static int  f=0;

    if (!f) {
        if (!cmi_initlibrary(ptmodel)) exit(1);
        if (getenv("CMI_DEBUG") != NULL) CMI_DEBUG = 1;
        else CMI_DEBUG = 0;
        f = 1;
    }
    cmi_reset_alter_models();
    return 1;
}

void
mcc_clean_cmi(mcc_modellist *ptmodel)
{
    cmi_cleaninstance(ptmodel);
    cmi_clean_alter_models();
    cmi_reset_alter_models();
}

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
                 elp_lotrs_param *lotrsparam)
{
    CMI_VAR slot;
    alter_param changes[5];
    double s;
    int i=0;


    if (cmi_set_alter_model(CMI_QINT_MODEL) != 0) {
        changes[i].name = "cgdo"; changes[i++].value = 0.0;
        changes[i].name = "cgdl"; changes[i++].value = 0.0;
        changes[i].name = "cgso"; changes[i++].value = 0.0;
        changes[i].name = "cgdl"; changes[i++].value = 0.0;
        changes[i].name = NULL;
        cmi_initmodel(ptmodel, changes);
        cmi_initinstance(ptmodel, CMI_QINT_MODEL, lotrsparam, L, W, -1, -1, -1, -1);
    }
    
    cmi_evalinstance(ptmodel, &slot, vgs, vds, vbs, 1);
    s = W*L;
    if (ptQg) *ptQg = slot.qg/s;
    if (ptQs) *ptQs = slot.qs/s;
    if (ptQd) *ptQd = slot.qd/s;
    if (ptQb) *ptQb = slot.qb/s;
    if (CMI_DEBUG) printf("[CMI] mcc_calcQint_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=%g, Vbs=%g): Qg=%g, Qs=%g, Qd=%g, Qb=%g\n", ptmodel->NAME, W, L, vgs, vds, vbs, ptQg?(*ptQg):-1, ptQs?(*ptQs):-1, ptQd?(*ptQd):-1, ptQb?(*ptQb):-1);
}

/* Perimeter contribution of gate capacitance */
double 
mcc_calcCGP_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double vgx,
                double L,
                double W,
                double temp,
                double *ptQov)
{
    double cgp, qgunitint, qgint, qgtot;
    CMI_VAR slot;

    if (cmi_set_alter_model(CMI_DEFAULT_MODEL) != 0) {
        cmi_initmodel(ptmodel, NULL);
        cmi_initinstance(ptmodel, CMI_DEFAULT_MODEL, lotrsparam, L, W, -1, -1, -1, -1);
    }

    cmi_evalinstance(ptmodel, &slot, vgx, 0, 0, 1);
    cgp = slot.cgdo/W;
    if (ptQov) {
        qgtot = slot.qg;
        mcc_calcQint_cmi(ptmodel, L, W, temp, vgx, 0, 0, &qgunitint, NULL, NULL, NULL, lotrsparam);
        qgint  = qgunitint*(W*L);
        *ptQov = (qgtot-qgint)/(2*W);
    }
    if (CMI_DEBUG) printf("[CMI] mcc_calcCGP_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=%g, Vbs=%g): Cgp=%g, Qov=%g\n", ptmodel->NAME, W, L, vgx, vgx, vgx, cgp, ptQov?(*ptQov):-1);
    return cgp;
}

/* Surface contribution of gate-drain capacitance */
double 
mcc_calcCGD_cmi(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vgs0,
                double vgs1,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam)
{
    double qinit=0.0, qfinal=0.0, cgd=0.0;
    CMI_VAR slot;

    if (mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART) < 0.0) return 0.0;

    if (cmi_set_alter_model(CMI_DEFAULT_MODEL) != 0) {
        cmi_initmodel(ptmodel, NULL);
        cmi_initinstance(ptmodel, CMI_DEFAULT_MODEL, lotrsparam, L, W, -1, -1, -1, -1);
    }

    cmi_evalinstance(ptmodel, &slot, vgs0, vds, vbs, 1);
    qinit = slot.qd;
    cmi_evalinstance(ptmodel, &slot, vgs1, vds, vbs, 1);
    qfinal = slot.qd;
    
    cgd = fabs((qfinal-qinit)/(vgs1-vgs0))/(W*L);
    if (CMI_DEBUG) printf("[CMI] mcc_calcCGD_cmi for \"%s\" (W=%g, L=%g) (Vgs=[%g:%g], Vds=%g, Vbs=%g): Cgd=%g, Qinit=%g, Qfinal=%g\n", ptmodel->NAME, W, L, vgs0, vgs1, vds, vbs, cgd, qinit, qfinal);
    return cgd;
}


/* Surface contribution of gate-drain capacitance */
double 
mcc_calcCGSI_cmi(mcc_modellist *ptmodel,
                 double L,
                 double W,
                 double temp,
                 double vgs,
                 double vbs,
                 double vds,
                 elp_lotrs_param *lotrsparam)
{
    double qinit=0.0, qfinal=0.0, cgsi=0.0;
    CMI_VAR slot;

    if (cmi_set_alter_model(CMI_DEFAULT_MODEL) != 0) {
        cmi_initmodel(ptmodel, NULL);
        cmi_initinstance(ptmodel, CMI_DEFAULT_MODEL, lotrsparam, L, W, -1, -1, -1, -1);
    }

    cmi_evalinstance(ptmodel, &slot, 0.0, vds, vbs, 1);
    qinit = slot.qd;
    cmi_evalinstance(ptmodel, &slot, vgs, vds, vbs, 1);
    qfinal = slot.qd;
    
    cgsi = fabs((qfinal-qinit)/vgs)/(W*L);
    if (CMI_DEBUG) printf("[CMI] mcc_calcCGSI_cmi for \"%s\" (W=%g, L=%g) (Vgs=[%g:%g], Vds=%g, Vbs=%g): Cgp=%g, Qinit=%g, Qfinal=%g\n", ptmodel->NAME, W, L, 0.0, vgs, vds, vbs, cgsi, qinit, qfinal);
    return cgsi;
}

double 
mcc_calcVTH_cmi(mcc_modellist *ptmodel,
                double L,
                double W,
                double temp,
                double vbs,
                double vds,
                elp_lotrs_param *lotrsparam)
{
    double vth;
    CMI_VAR slot;

    if (cmi_set_alter_model(CMI_DEFAULT_MODEL) != 0) {
        cmi_initmodel(ptmodel, NULL);
        cmi_initinstance(ptmodel, CMI_DEFAULT_MODEL, lotrsparam, L, W, -1, -1, -1, -1);
    }

    cmi_evalinstance(ptmodel, &slot, vds, vds, vbs, 0);

    vth = slot.von;
//    if (ptmodel->TYPE == MCC_TRANS_P) vth = -vth ;
    if (CMI_DEBUG) printf("[CMI] mcc_calcVTH_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=%g, Vbs=%g): Vth=%g\n", ptmodel->NAME, W, L, vds, vds, vbs, vth);
    return vth;
}

double 
mcc_calcIDS_cmi(mcc_modellist *ptmodel,
                double vbs,
                double vgs,
                double vds,
                double W,
                double L,
                double Temp,
                elp_lotrs_param *lotrsparam)
{
    CMI_VAR slot;

    if (cmi_set_alter_model(CMI_DEFAULT_MODEL) != 0) {
        cmi_initmodel(ptmodel, NULL);
        cmi_initinstance(ptmodel, CMI_DEFAULT_MODEL, lotrsparam, L, W, -1, -1, -1, -1);
    }

    cmi_evalinstance(ptmodel, &slot, vgs, vds, vbs, 0);
    if (CMI_DEBUG) printf("[CMI] mcc_calcIDS_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=%g, Vbs=%g): Ids=%g\n", ptmodel->NAME, W, L, vgs, vds, vbs, slot.ids);
    return slot.ids;
}

/* Offset for effective drain channel junction width */
double mcc_calcDWCJ_cmi( mcc_modellist *ptmodel, 
                         elp_lotrs_param *lotrsparam, 
                         double temp,
                         double L, 
                         double W
                       )
{
    /* OK for Hynix model but we need a general solution */
    return 0.0;
}

/* Area contribution of channel junction capacitance for drain */
double 
mcc_calcCDS_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx0,
                double vbx1,
                double L,
                double W)
{
    CMI_VAR slot;
    alter_param changes[3];
    double qinit=0.0, qfinal=0.0, cds=0.0;
    int i=0;

    if (cmi_set_alter_model(CMI_CDS_MODEL) != 0) {
        changes[i].name = "cjswd"; changes[i++].value = 0.0;
        changes[i].name = "cjswgd"; changes[i++].value = 0.0;
        changes[i].name = NULL;
        cmi_initmodel(ptmodel, changes);
        cmi_initinstance(ptmodel, CMI_CDS_MODEL, lotrsparam, L, W, W*W, 0, W*W, 0);
    }
    
    cmi_evalinstance(ptmodel, &slot, 0.0, vbx0, 0.0, 1);
    qinit = slot.qbd;
    cmi_evalinstance(ptmodel, &slot, 0.0, vbx1, 0.0, 1);
    qfinal = slot.qbd;
    cmi_set_alter_model(CMI_DEFAULT_MODEL);

    cds = fabs((qfinal-qinit)/(vbx1-vbx0))/(W*W);
    if (CMI_DEBUG) printf("[CMI] mcc_calcCDS_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=[%g:%g], Vbs=%g): Cgp=%g, Qinit=%g, Qfinal=%g\n", ptmodel->NAME, W, L, 0.0, vbx0, vbx1, 0.0, cds, qinit, qfinal);
    return cds;
}

/* Sidewall contribution of channel junction capacitance for drain */
double 
mcc_calcCDP_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx0,
                double vbx1,
                double L,
                double W)
{
    CMI_VAR slot;
    alter_param changes[3];
    double qinit=0.0, qfinal=0.0, cdp=0.0;
    int i=0;

    if (cmi_set_alter_model(CMI_CDP_MODEL) != 0) {
        changes[i].name = "cjd"; changes[i++].value = 0.0;
        changes[i].name = "cjswgd"; changes[i++].value = 0.0;
        changes[i].name = NULL;
        cmi_initmodel(ptmodel, changes);
        cmi_initinstance(ptmodel, CMI_CDP_MODEL, lotrsparam, L, W, 0, W, 0, W);
    }
    
    cmi_evalinstance(ptmodel, &slot, 0.0, vbx0, 0.0, 1);
    qinit = slot.qbd;
    cmi_evalinstance(ptmodel, &slot, 0.0, vbx1, 0.0, 1);
    qfinal = slot.qbd;
    cmi_set_alter_model(CMI_DEFAULT_MODEL);

    cdp = fabs((qfinal-qinit)/(vbx1-vbx0))/W;
    if (CMI_DEBUG) printf("[CMI] mcc_calcCDP_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=[%g:%g], Vbs=%g): Cdp=%g, Qinit=%g, Qfinal=%g\n", ptmodel->NAME, W, L, 0.0, vbx0, vbx1, 0.0, cdp, qinit, qfinal);
    return cdp;
}

/* Gate sidewall contribution of channel junction capacitance for drain */
double 
mcc_calcCDW_cmi(mcc_modellist *ptmodel,
                elp_lotrs_param *lotrsparam,
                double temp,
                double vbx0,
                double vbx1,
                double L,
                double W)
{
    CMI_VAR slot;
    alter_param changes[3];
    double qinit=0.0, qfinal=0.0, cdw=0.0;
    int i=0;

    if (cmi_set_alter_model(CMI_CDW_MODEL) != 0) {
        changes[i].name = "cjd"; changes[i++].value = 0.0;
        changes[i].name = "cjswd"; changes[i++].value = 0.0;
        changes[i].name = NULL;
        cmi_initmodel(ptmodel, changes);
        cmi_initinstance(ptmodel, CMI_CDW_MODEL, lotrsparam, L, W, 0, 0, 0, 0);
    }
    
    cmi_evalinstance(ptmodel, &slot, 0.0, vbx0, 0.0, 1);
    qinit = slot.qbd;
    cmi_evalinstance(ptmodel, &slot, 0.0, vbx1, 0.0, 1);
    qfinal = slot.qbd;
    cmi_set_alter_model(CMI_DEFAULT_MODEL);

    cdw = fabs((qfinal-qinit)/(vbx1-vbx0))/W;
    if (CMI_DEBUG) printf("[CMI] mcc_calcCDW_cmi for \"%s\" (W=%g, L=%g) (Vgs=%g, Vds=[%g:%g], Vbs=%g): Cdw=%g, Qinit=%g, Qfinal=%g\n", ptmodel->NAME, W, L, 0.0, vbx0, vbx1, 0.0, cdw, qinit, qfinal);
    return cdw;
}

/* Calculate effective Perimeter & Area if undefined */
void
mcc_calcPA_cmi(lotrs_list *ptlotrs,
                            mcc_modellist *ptmodel,
                            elp_lotrs_param *lotrsparam,
                            double *as,
                            double *ad,
                            double *ps,
                            double *pd)
{
    mcc_calcPAfromgeomod_bsim4(ptlotrs, ptmodel, lotrsparam, as, ad, ps, pd);
}
