/*
 *  mcc_mod_cmi_interface.h
 *  avertec_distrib
 *
 *  Created by Anthony on 29/04/2008.
 *  Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include MCC_H
#include ELP_H
#include "../cmi/include/CMIdef.h"

#define CMI_NMOS 1
#define CMI_PMOS -1

#define CMI_DEFAULT_MODEL 0
#define CMI_QINT_MODEL 1
#define CMI_CDS_MODEL 2
#define CMI_CDP_MODEL 3
#define CMI_CDW_MODEL 4
#define CMI_NUM_ALTER_MODELS 5

typedef struct {
    char *name;
    double value;
} alter_param;

typedef struct {
    void *ModelData;
    int ModelSize;
    void *InstanceData;
    int InstanceSize;
} alter_model;

int cmi_initlibrary(mcc_modellist *ptmodel);
int cmi_initmodel(mcc_modellist *ptmodel, alter_param *changes);
int cmi_initinstance(mcc_modellist *ptmodel, int model_tag, elp_lotrs_param *lotrsparam, double l, double w, double ad, double pd, double as, double ps);
int cmi_cleaninstance(mcc_modellist *ptmodel);
int cmi_evalinstance(mcc_modellist *ptmodel, CMI_VAR *pslot, double vgs, double vds, double vbs, int qflag);
int cmi_save_alter_model(int ModelTag);
int cmi_set_alter_model(int ModelTag);
void cmi_clean_alter_models(void);
void cmi_reset_alter_models(void);
