/*
 *  mcc_mod_cmi_interface.c
 *  avertec_distrib
 *
 *  Created by Anthony on 29/04/2008.
 *  Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include <dlfcn.h>

#include "mcc_mod_cmi_interface.h"

/* The current active CMI model */
static CMI_MOSDEF *dyn_cmi_mosfet;

/* The global CMI environment */
static CMI_ENV *dyn_cmi_env;

/* The topology */
static char *cmi_topo;

/* The alternative models for parameter isolation */
static alter_model cmi_alter_models[CMI_NUM_ALTER_MODELS];
static int cmi_cur_alter_model;

int hspPrint(char *format, ...)
{
    va_list va;
    int res;

    va_start(va, format);
    res = vprintf(format, va);
    va_end(va);

    return res;
}

int
cmi_initlibrary(mcc_modellist *ptmodel)
{
    void *lib;
    char libname[]="libCMImodel.so";
    char namebuf[256];
    ptype_list *ptuser;
    void *symbol;

    /* Load shared library */
    printf("Opening dynamic library %s.\n", libname);
    lib = dlopen(libname, RTLD_NOW|RTLD_GLOBAL);
    if(!lib) {
        printf("%s\n", dlerror());
        return 0;
    }

    /* Setup environment */
    symbol = dlsym(lib, "pCMIenv");
    if(!symbol) {
        printf("can't find environment in shared library!\n");
        return 0 ;
    }
    dyn_cmi_env = *(CMI_ENV **)symbol;
    dyn_cmi_env->CKTtemp    = MCC_TEMP + 273.15;
    dyn_cmi_env->CKTnomTemp    = MCC_TEMP + 273.15;

    /* Obtain CMI model : selected by EXTNAME ptype in model */
    symbol = NULL;
    ptuser = getptype(ptmodel->USER, MCC_MODEL_EXTNAME);
    if (ptuser) {
        sprintf(namebuf, "pCMI_%sdef", (char *)ptuser->DATA);
        symbol = dlsym(lib, namebuf);
    }
    if(!symbol) {
        printf("can't find model in shared library!\n");
        return 0;
    }
    dyn_cmi_mosfet = *(CMI_MOSDEF **)symbol;
    /* Allocate structure for model topo : 4Kb should be enough for most cases */
    if ((cmi_topo = (char *)calloc(1, 4096)) == NULL) {
        printf("can't initialise topology!\n");
        return 0;
    }
    
    return 1 ;
}

int
cmi_initmodel(mcc_modellist *ptmodel, alter_param *changes)
{
    mcc_paramlist *ptparam;
    int i=0;

    if (dyn_cmi_mosfet->CMI_ResetModel(dyn_cmi_mosfet->pModel, (ptmodel->TYPE==MCC_PMOS)?1:0, 102) != 0) {
        printf( "Can't reset model '%s'\n", ptmodel->NAME );
        return 1;
    }
    if (ptmodel->NAME) strcpy(dyn_cmi_mosfet->ModelName, ptmodel->NAME);
    for (ptparam = ptmodel->PARAM; ptparam; ptparam = ptparam->NEXT) {
        if (dyn_cmi_mosfet->CMI_AssignModelParm(dyn_cmi_mosfet->pModel, ptparam->NAME, ptparam->VALUE) != 0) {
            printf( "Can't set model parameter '%s'\n", ptparam->NAME);
            return 1;
        }
    }
    if (changes) {
        while (changes[i].name) {
            if (dyn_cmi_mosfet->CMI_AssignModelParm(dyn_cmi_mosfet->pModel, changes[i].name, changes[i].value) != 0) {
                printf( "Can't set model parameter '%s'\n", changes[i].name);
                return 1;
            }
            i++;
        }
    }
    if (dyn_cmi_mosfet->CMI_SetupModel(dyn_cmi_mosfet->pModel) != 0) {
        printf( "Can't setup model '%s'\n", ptmodel->NAME);
        return 1;
    }
    return 0;
}

int
cmi_initinstance(mcc_modellist *ptmodel, int model_tag, elp_lotrs_param *lotrsparam, double l, double w, double ad, double pd, double as, double ps)
{
    char *label[20];
    double value[20];
    int i=0, j;
    ptype_list *ptuser;
    char *trsname = NULL;
    
    if (cmi_alter_models[model_tag].ModelData != NULL) return 0;
    if ((ptuser = getptype(ptmodel->USER, MCC_MODEL_LOTRS)) != NULL) {
        trsname = ((lotrs_list *)ptuser->DATA)->TRNAME;
    }
    if (trsname) strcpy(dyn_cmi_mosfet->InstanceName, trsname);
    
    if (dyn_cmi_mosfet->CMI_ResetInstance(dyn_cmi_mosfet->pInstance) != 0) {
        printf( "Can't reset instance '%s' of model '%s'\n", dyn_cmi_mosfet->InstanceName, dyn_cmi_mosfet->ModelName);
        return 1;
    }
    value[i] = l; label[i++] = "l";
    value[i] = w; label[i++] = "w";
    if (ad != -1) {
        value[i] = ad; label[i++] = "ad";
    }
    if (pd != -1) {
        value[i] = pd; label[i++] = "pd";
    }
    if (as != -1) {
        value[i] = as; label[i++] = "as";
    }
    if (ps != -1) {
        value[i] = ps; label[i++] = "ps";
    }
    if (lotrsparam) {
        if ((value[i] = lotrsparam->PARAM[elpMULU0]) != 1.0) label[i++] = "mulu0";
        if ((value[i] = lotrsparam->PARAM[elpDELVT0]) != 0.0) label[i++] = "delvt0";
        if ((value[i] = lotrsparam->PARAM[elpSA]) != ELPINITVALUE) label[i++] = "sa";
        if ((value[i] = lotrsparam->PARAM[elpSB]) != ELPINITVALUE) label[i++] = "sb";
        if ((value[i] = lotrsparam->PARAM[elpSD]) != ELPINITVALUE) label[i++] = "sd";
        if ((value[i] = lotrsparam->PARAM[elpNF]) != 1.0) label[i++] = "nf";
        if ((value[i] = lotrsparam->PARAM[elpNRS]) != 0.0) label[i++] = "nrs";
        if ((value[i] = lotrsparam->PARAM[elpNRD]) != 0.0) label[i++] = "nrd";
        if ((value[i] = lotrsparam->PARAM[elpSC]) != ELPINITVALUE) label[i++] = "sc";
        if ((value[i] = lotrsparam->PARAM[elpSCA]) != ELPINITVALUE) label[i++] = "sca";
        if ((value[i] = lotrsparam->PARAM[elpSCB]) != ELPINITVALUE) label[i++] = "scb";
        if ((value[i] = lotrsparam->PARAM[elpSCC]) != ELPINITVALUE) label[i++] = "scc";
    }
    for (j=0; j<i; j++) {
        if (dyn_cmi_mosfet->CMI_AssignInstanceParm(dyn_cmi_mosfet->pInstance, label[j], value[j]) != 0) {
            printf( "Can't set instance parameter '%s'\n", label[j]);
            return 1;
        }
    }
    if (dyn_cmi_mosfet->CMI_SetupInstance(dyn_cmi_mosfet->pModel, dyn_cmi_mosfet->pInstance) != 0) {
        printf( "Can't setup instance of model '%s'\n", ptmodel->NAME);
        return 1;
    }
    cmi_save_alter_model(model_tag);
    return 0;
}

int
cmi_cleaninstance(mcc_modellist *ptmodel)
{
    if (dyn_cmi_mosfet->CMI_ResetInstance(dyn_cmi_mosfet->pInstance) != 0) {
        printf( "Can't reset instance of model '%s'\n", ptmodel->NAME );
        return 1;
    }
    return 0;
}

int
cmi_evalinstance(mcc_modellist *ptmodel, CMI_VAR *pslot, double vgs, double vds, double vbs, int qflag)
{
    pslot->qflag = qflag;
    pslot->vgs = vgs;
    pslot->vds = vds;
    pslot->vbs = vbs;
    pslot->mode = CMI_NORMAL_MODE;
    pslot->topovar = cmi_topo;

    if (dyn_cmi_mosfet->CMI_Evaluate(pslot, dyn_cmi_mosfet->pModel, dyn_cmi_mosfet->pInstance) != 0) {
        printf( "Can't evaluate instance of model '%s'\n", ptmodel->NAME );
        return 1;
    }
    return 0;
}

int
cmi_save_alter_model(int ModelTag)
{
    void *ptr;
    
    ptr = malloc(dyn_cmi_mosfet->modelSize);
    memcpy(ptr, dyn_cmi_mosfet->pModel, dyn_cmi_mosfet->modelSize);
    cmi_alter_models[ModelTag].ModelData = ptr;
    cmi_alter_models[ModelTag].ModelSize = dyn_cmi_mosfet->modelSize;
    
    ptr = malloc(dyn_cmi_mosfet->instSize);
    memcpy(ptr, dyn_cmi_mosfet->pInstance, dyn_cmi_mosfet->instSize);
    cmi_alter_models[ModelTag].InstanceData = ptr;
    cmi_alter_models[ModelTag].InstanceSize = dyn_cmi_mosfet->instSize;

    cmi_cur_alter_model = ModelTag;
    return 0;
}

int
cmi_set_alter_model(int ModelTag)
{
    if (cmi_cur_alter_model == ModelTag) return 0;
    if (cmi_alter_models[ModelTag].ModelData != NULL) {
        memcpy(dyn_cmi_mosfet->pModel, cmi_alter_models[ModelTag].ModelData, dyn_cmi_mosfet->modelSize);
        memcpy(dyn_cmi_mosfet->pInstance, cmi_alter_models[ModelTag].InstanceData, dyn_cmi_mosfet->instSize);
        cmi_cur_alter_model = ModelTag;
        return 0;
    }
    cmi_cur_alter_model = -1;
    return 1;
}

void
cmi_clean_alter_models(void) {
    int i;
    
    for (i=0; i < CMI_NUM_ALTER_MODELS; i++) {
        if (cmi_alter_models[i].ModelData != NULL) free(cmi_alter_models[i].ModelData);
        if (cmi_alter_models[i].InstanceData != NULL) free(cmi_alter_models[i].InstanceData);
    }
}

void
cmi_reset_alter_models(void)
{
    int i;
    
    for (i=0; i < CMI_NUM_ALTER_MODELS; i++) cmi_alter_models[i].ModelData = NULL;
    cmi_cur_alter_model = -1;
}


