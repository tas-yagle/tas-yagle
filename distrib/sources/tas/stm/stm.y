/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit: STM Version 1.00                                               */
/*    Fichier: stm.y                                                          */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s): Gilles Augustins                                             */
/*                                                                            */
/******************************************************************************/

%{
#define API_HIDE_TOKENS
#include API_H
#include STM_H
#include "stm.h" 

/******************************************************************************/
/* private                                                                    */
/******************************************************************************/

static chain_list   *stm_ch, *stm_chp, *stm_xchain, *stm_ychain, *stm_xydata;
static chain_list   *stm_energy_xchain, *stm_energy_ychain, *stm_energy_xydata;
static char          stm_xtype, stm_ytype;
static char          stm_energy_xtype, stm_energy_ytype;
static char          stm_dimension;
static char          stm_energy_dimension;
static int           stm_nx, stm_ny;
static int           stm_energy_nx, stm_energy_ny;
static float        *stm_ptfl;
static timing_model *stm_model;
static char         *stm_cname = NULL;
static int           stm_index;
static float         stm_d_params[STM_NB_DUAL_PARAMS];
static float         stm_f_params[STM_NB_FALSE_PARAMS];
static float         stm_p_params[STM_NB_PATH_PARAMS];
static float         stm_c_params[STM_NB_CST_PARAMS];
static float         stm_i_params[1 + STM_NB_LINK_I_PARAMS * 16]; /* max = 16 pass transistors */
static float         stm_0_params[STM_NB_LINK_0_PARAMS];
static float         stm_n_params[STM_NB_LINK_N_PARAMS];
static float         stm_e_params[STM_NB_ENERGY_PARAMS];
static long          stm_deg_params[10];
static long          stm_coefdef_params[1024];
static float         stm_coeftab_params[1024];
static timing_varpolynom *stm_var_params[3];
static timing_varpolynom **stm_var_p;
static long         *stm_deg_p;
static long         *stm_coefdef_p;
static float        *stm_coeftab_p;
static int           stm_nb_var = 0;
static int           stm_deg_index = 0;
static int           stm_var_index = 0;
static int           stm_coefdef_index = 0;
static int           stm_coefdef_nb = 0;
static int           stm_coeftab_index = 0;
static float        *stm_d_p;
static float        *stm_f_p;
static float        *stm_p_p;
static float        *stm_c_p;
static float        *stm_i_p;
static float        *stm_0_p;
static float        *stm_n_p;
static float        *stm_e_p;
static char          stm_d;
static char          stm_f;
static char          stm_p;
static char          stm_c;
static char          stm_i;
static char          stm_0;
static char          stm_n;
static char          stm_e;
static long          stm_coefdef_mask = STM_COEF_DEFMASK;
static float         vth;
static float         vdd;
static float         vt;
static float         vf;
static char          tran;
static char          vnoise_model;
static float         vnoise_scr;
static float         vnoise_invth;
static noise_scr    *scr_model;
static char          stm_energy_model_type;
static char         *stm_fct_ins;
static char         *stm_fct_file;
static char         *stm_fct_localname;
static APICallFunc  *stm_fct_fct;
static int           sim_model_number;
static float         stm_iv_conf[4];
static float         stm_iv_input[4];
static float         stm_iv_init[1];
static int           stm_iv_nve;
static int           stm_iv_nvs;
static chain_list   *stm_iv_tabve;
static chain_list   *stm_iv_tabvs;
static chain_list   *stm_iv_tabiv;
static int           stm_iv_e;
static int           stm_iv_s;
static chain_list   *stm_iv_scane;
static chain_list   *stm_iv_scans;

/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror ();
int yylex ();

%}

%union {
  char           *t_pchar;
  struct {
    float         t_float;
    char          t_float_string[128];
  }               t_struct3;

  struct chain   *t_pchain;

  struct {
    struct chain *CHAIN;
    char          TYPE;
  }               t_struct1;
  
  struct {
    struct chain *CHAIN;
    char          DIM;
  }               t_struct2;

  char            t_char;
  long            t_long;
  APICallFunc    *t_cf;
};

%token            _SPDM
%token            _ORDERS
%token            _COEFS
%token            _DUAL
%token            _LINK_OUT
%token            _LINK_PASS
%token            _LINK_DUAL
%token            _FALSE
%token            _PATH
%token            _SCM_DUAL
%token            _SCM_GOOD
%token            _SCM_FALSE
%token            _SCM_PATH
%token            _SCM_CST
%token            _PCONF0
%token            _PCONF1
%token            _CAPAI
%token            _CAPAI0
%token            _CAPAI1
%token            _CAPAI2
%token            _CAPAI3
%token            _VT0
%token            _VT0C
%token            _IRAP
%token            _VDDIN
%token            _VT
%token            _THRESHOLD
%token            _INPUT_THR
%token            _RINT
%token            _VINT
%token            _CHALF
%token            _QINIT
%token            _RCONF
%token            _KR
%token            _IMAX
%token            _AN
%token            _BN
%token            _ACTI
%token            _BCTI
%token            _CI
%token            _CF
%token            _K3
%token            _K4
%token            _K5
%token            _RTOT
%token            _KF
%token            _VDDMAX
%token            _VF_INPUT
%token            _DATA
%token            _DELAY
%token            _SLEW
%token            _CELL
%token            _CELLTYPE
%token            _CLOCK_SLEW_AXIS
%token            _COMB
%token            _CST
%token            _DATA
%token            _DATE
%token <t_struct3>  _ENUMBER
%token            _ENVIRONMENT
%token            _HEADER
%token <t_pchar>  _IDENTIFIER
%token            _IGNORE 
%token            _INPUT_SLEW_AXIS
%token            _LIBRARY
%token            _LOAD_AXIS
%token            _MODEL
%token <t_struct3>  _NUMBER
%token <t_pchar>  _QSTRING
%token            _SEQ
%token            _SLEW_AXIS
%token            _SPLINE
%token            _TECHNOLOGY
%token            _VENDOR
%token            _VERSION
%token            _NAME
%token            _VTH
%token            _VDD
%token            _VF
%token            _VARTHMIN
%token            _VARTHMAX
%token            _UNKNOWN
%token            _NOISE_SCR
%token            _ENERGY_TABLE
%token            _ENERGY_CONST
%token            _ENERGY_MODEL
%token            _RSAT
%token            _RLIN
%token            _DRC
%token            _RBR
%token            _CBR
%token            _FCT
%token            _INSTANCE
%token            _LOCALNAME
%token            _SIM_MODEL
%token            _FUNCTION
%token            _FILE
%token            _IV
%token            _NVE
%token            _NVS
%token            _VE
%token            _VS
%token            _CONFLICT
%token            _CURRENT
%token            _INPUT
%token            _INITIAL
%token <t_char>   _ECHAR
%token            _TRAN

%start stm_file 

%type <t_pchar>   modelname
%type <t_pchar>   cellname
%type <t_pchain>  val_list
%type <t_struct2> modeldata 
%type <t_struct2> data 
%type <t_pchain>  array1D 
%type <t_pchain>  array1D_list
%type <t_pchain>  array2D
%type <t_struct1> axis 
%type <t_char>    axistype 
%type <t_struct3> val
%type <t_long>    tmodel
%type <t_long>    fctmodel
%type <t_long>    tblmodel
%type <t_long>    tbltype
%type <t_long>    scmmodel
%type <t_long>    scmtype
%type <t_long>    plnmodel
%type <t_long>    energytblmodel
%type <t_long>    energytbltype
%type <t_long>    energyparamsmodel
%type <t_pchain>  param_list param
%type <t_cf>      function
%type <t_long>    nbve
%type <t_pchain>  voltageve
%type <t_pchain>  voltagevs
%type <t_pchain>  current
%type <t_long>    ivmodel

%%

stm_file    : header cell 
            ;

header      : _HEADER '(' helem_list ')'
            ;

helem_list  : empty
            | helem_list helem
            ;

helem       : library
            | technology
            | date
            | vendor
            | environment
            | version
            ;

library     : _LIBRARY '(' _QSTRING ')'
            {
              mbkfree($3);
            }
            ;

technology  : _TECHNOLOGY '(' _QSTRING ')'
            {
              mbkfree($3);
            }
            ;

date        : _DATE '(' _QSTRING ')'
            {
              mbkfree($3);
            }
            ;

vendor      : _VENDOR '(' _QSTRING ')'
            {
              mbkfree($3);
            }
            ;

environment : _ENVIRONMENT '(' _QSTRING ')'
            {
              mbkfree($3);
            }
            ;

version     : _VERSION '(' _QSTRING ')'
            {
              mbkfree($3);
            }
            ;

cell        : _CELL '(' celldef model_list ')' 
            ;
             
celldef     : cellname 
            {
                stm_cname = $1;
                stm_addcell ($1);
            }
            ;

model_list  : empty
            | model_list model 
            ;

model       : _MODEL '(' modelname tmodel ')' 
            { 
                switch ($4 & STM_MOD_TYPEMASK) {
                
                    /*************/
                    /* TBL model */
                    /*************/

                    case STM_MOD_MODTBL :
                        if (stm_xydata) {
                             for (stm_nx = 0, stm_ch = stm_xchain; stm_ch; stm_ch = stm_ch->NEXT) stm_nx++;
                             for (stm_ny = 0, stm_ch = stm_ychain; stm_ch; stm_ch = stm_ch->NEXT) stm_ny++;
                             stm_model = stm_addtblmodel (stm_cname, $3, stm_nx, stm_ny, stm_xtype, stm_ytype);
                             stm_model->VTH = vth;
                             stm_model->VDD = vdd;
                             stm_model->VT = vt;
                             stm_model->VF = vf;
                             stm_model->TTYPE = tran;
                             switch( vnoise_model ) {
                                 case STM_NOISE_SCR:
                                     stm_noise_create( stm_model, STM_NOISE_SCR );
                                     scr_model = stm_noise_getmodel_scr( stm_model );
                                     stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                     break;
                             }
                             stm_modtbl_setXrange (stm_model->UMODEL.TABLE, stm_xchain, 1);
                             stm_modtbl_setYrange (stm_model->UMODEL.TABLE, stm_ychain, 1);
                             switch (stm_dimension) { 
                                 case STM_CONST:
                                     if (stm_nx > 0 || stm_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                     else {
                                         stm_modtbl_setconst (stm_model->UMODEL.TABLE, *(float*)stm_xydata->DATA);
                                         mbkfree((float*)stm_xydata->DATA);
                                         freechain (stm_xydata);
                                    }
                                 break;
                                 case STM_1D:
                                     if (stm_nx > 0 && stm_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                     else {
                                         stm_modtbl_set1Dset (stm_model->UMODEL.TABLE, stm_xydata, 1);
                                         for (stm_ch = stm_xydata; stm_ch; stm_ch = stm_ch->NEXT)
                                             mbkfree((float*)stm_ch->DATA);
                                         freechain (stm_xydata);
                                    }
                                 break;
                                 case STM_2D:
                                     if (stm_nx == 0 || stm_ny == 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                     else {
                                         stm_modtbl_set2Dset (stm_model->UMODEL.TABLE, stm_xydata, 1);
                                         for (stm_ch = stm_xydata; stm_ch; stm_ch = stm_ch->NEXT) {
                                             for (stm_chp = (chain_list*)stm_ch->DATA; stm_chp; stm_chp = stm_chp->NEXT)
                                                 mbkfree((float*)stm_chp->DATA);
                                             freechain ((chain_list*)stm_ch->DATA);
                                        }
                                         freechain (stm_xydata);
                                    }
                                 break;
                            }
                            switch (stm_energy_model_type) {
                                case STM_ENERGY_PARAMS :    
                                    stm_e_p = stm_e ? stm_e_params : NULL;
                                    stm_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                    stm_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stm_e_p);
                                    break;
                                case STM_ENERGY_TABLE:
                                    stm_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                    for (stm_energy_nx=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_nx++;
                                    for (stm_energy_ny=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_ny++;
                                    stm_modtbl_setXrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_xchain, 1);
                                    stm_modtbl_setYrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_ychain, 1);
                                    switch (stm_energy_dimension) { 
                                        case STM_CONST:
                                            if (stm_energy_nx > 0 || stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_setconst (stm_model->ENERGYMODEL.ETABLE, *(float*)stm_energy_xydata->DATA);
                                                mbkfree((float*)stm_energy_xydata->DATA);
                                                freechain (stm_energy_xydata);
                                            }
                                            break;
                                        case STM_1D:
                                            if (stm_energy_nx > 0 && stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set1Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                                for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT)
                                                    mbkfree((float*)stm_ch->DATA);
                                                freechain (stm_energy_xydata);
                                            }
                                            break;
                                        case STM_2D:
                                            if (stm_energy_nx == 0 || stm_energy_ny == 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set2Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                                for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT) {
                                                    for (stm_chp = (chain_list*)stm_ch->DATA; stm_chp; stm_chp = stm_chp->NEXT)
                                                        mbkfree((float*)stm_chp->DATA);
                                                    freechain ((chain_list*)stm_ch->DATA);
                                                }
                                                freechain (stm_energy_xydata);
                                            }
                                            break;
                                    }
                                    for (stm_ch = stm_energy_xchain; stm_ch; stm_ch = stm_ch->NEXT)  
                                        mbkfree((float*)stm_ch->DATA);
                                    for (stm_ch = stm_energy_ychain; stm_ch; stm_ch = stm_ch->NEXT)  
                                        mbkfree((float*)stm_ch->DATA);
                                    freechain (stm_energy_xchain);
                                    freechain (stm_energy_ychain);
                                    break;
                            }
                        } 
                        for (stm_ch = stm_xchain; stm_ch; stm_ch = stm_ch->NEXT)  
                            mbkfree((float*)stm_ch->DATA);
                        for (stm_ch = stm_ychain; stm_ch; stm_ch = stm_ch->NEXT)  
                            mbkfree((float*)stm_ch->DATA);
                        freechain (stm_xchain);
                        freechain (stm_ychain);
                        break;

                    /*************/
                    /* SCM model */
                    /*************/
                        
                    case STM_MOD_MODSCM:
                        stm_model = stm_addmodel (stm_cname, $3);
                        stm_model->VTH = vth;
                        stm_model->VDD = vdd;
                        stm_model->VT = vt;
                        stm_model->VF = vf;
                        stm_model->TTYPE = tran;
                        switch( vnoise_model ) {
                            case STM_NOISE_SCR:
                                stm_noise_create( stm_model, STM_NOISE_SCR );
                                scr_model = stm_noise_getmodel_scr( stm_model );
                                stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                break;
                        }
                        stm_model->UTYPE = STM_MOD_MODSCM;
                        switch ($4 & STM_MODSCM_TYPEMASK) {
                            case STM_MODSCM_DUAL:
                                stm_d_p = stm_d ? stm_d_params : NULL;
                                stm_model->UMODEL.SCM = stm_modscm_dual_create (stm_d_p);
                                break;
                            case STM_MODSCM_GOOD:
                                stm_0_p = stm_0 ? stm_0_params : NULL;
                                stm_i_p = stm_i ? stm_i_params : NULL;
                                stm_n_p = stm_n ? stm_n_params : NULL;
                                stm_d_p = stm_d ? stm_d_params : NULL;
                                stm_model->UMODEL.SCM = stm_modscm_good_create (stm_0_p, stm_i_p, stm_n_p, stm_d_p);
                                break;
                            case STM_MODSCM_FALSE:
                                stm_0_p = stm_0 ? stm_0_params : NULL;
                                stm_i_p = stm_i ? stm_i_params : NULL;
                                stm_n_p = stm_n ? stm_n_params : NULL;
                                stm_f_p = stm_f ? stm_f_params : NULL;
                                stm_model->UMODEL.SCM = stm_modscm_false_create (stm_0_p, stm_i_p, stm_n_p, stm_f_p);
                                break;
                            case STM_MODSCM_PATH:
                                stm_0_p = stm_0 ? stm_0_params : NULL;
                                stm_i_p = stm_i ? stm_i_params : NULL;
                                stm_n_p = stm_n ? stm_n_params : NULL;
                                stm_p_p = stm_p ? stm_p_params : NULL;
                                stm_model->UMODEL.SCM = stm_modscm_path_create (stm_0_p, stm_i_p, stm_n_p, stm_p_p);
                                break;
                            case STM_MODSCM_CST:
                                stm_c_p = stm_c ? stm_c_params : NULL;
                                stm_model->UMODEL.SCM = stm_modscm_cst_create (stm_c_p);
                                break;
                        }
                        switch (stm_energy_model_type) {
                            case STM_ENERGY_PARAMS :    
                                stm_e_p = stm_e ? stm_e_params : NULL;
                                stm_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                stm_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stm_e_p);
                                break;
                            case STM_ENERGY_TABLE:
                                stm_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                for (stm_energy_nx=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_nx++;
                                for (stm_energy_ny=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_ny++;
                                stm_modtbl_setXrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_xchain, 1);
                                stm_modtbl_setYrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_ychain, 1);
                                switch (stm_energy_dimension) { 
                                    case STM_CONST:
                                        if (stm_energy_nx > 0 || stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_setconst (stm_model->ENERGYMODEL.ETABLE, *(float*)stm_energy_xydata->DATA);
                                            mbkfree((float*)stm_energy_xydata->DATA);
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                    case STM_1D:
                                        if (stm_energy_nx > 0 && stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_set1Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                            for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT)
                                                mbkfree((float*)stm_ch->DATA);
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                    case STM_2D:
                                        if (stm_energy_nx == 0 || stm_energy_ny == 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_set2Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                            for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT) {
                                                for (stm_chp = (chain_list*)stm_ch->DATA; stm_chp; stm_chp = stm_chp->NEXT)
                                                    mbkfree((float*)stm_chp->DATA);
                                                freechain ((chain_list*)stm_ch->DATA);
                                            }
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                }
                                for (stm_ch = stm_energy_xchain; stm_ch; stm_ch = stm_ch->NEXT)  
                                    mbkfree((float*)stm_ch->DATA);
                                for (stm_ch = stm_energy_ychain; stm_ch; stm_ch = stm_ch->NEXT)  
                                    mbkfree((float*)stm_ch->DATA);
                                freechain (stm_energy_xchain);
                                freechain (stm_energy_ychain);
                                break;
                        }
                        break;

                    /*************/
                    /* PLN model */
                    /*************/
                        
                    case STM_MOD_MODPLN:
                        stm_model = stm_addmodel (stm_cname, $3);
                        stm_model->VTH = vth;
                        stm_model->VDD = vdd;
                        stm_model->VT = vt;
                        stm_model->VF = vf;
                        stm_model->TTYPE = tran;
                        switch( vnoise_model ) {
                            case STM_NOISE_SCR:
                                stm_noise_create( stm_model, STM_NOISE_SCR );
                                scr_model = stm_noise_getmodel_scr( stm_model );
                                stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                break;
                        }
                        stm_model->UTYPE = STM_MOD_MODPLN;
                        stm_var_p = stm_var_params;
                        stm_deg_p = stm_deg_params;
                        stm_coefdef_p = stm_coefdef_params;
                        stm_coeftab_p = stm_coeftab_params;
                        stm_model->UMODEL.POLYNOM = stm_modpln_create (stm_nb_var, stm_deg_p, stm_coefdef_p, stm_coeftab_p, stm_var_p);
                        switch (stm_energy_model_type) {
                            case STM_ENERGY_PARAMS :    
                                stm_e_p = stm_e ? stm_e_params : NULL;
                                stm_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                stm_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stm_e_p);
                                break;
                            case STM_ENERGY_TABLE:
                                stm_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                for (stm_energy_nx=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_nx++;
                                for (stm_energy_ny=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_ny++;
                                stm_modtbl_setXrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_xchain, 1);
                                stm_modtbl_setYrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_ychain, 1);
                                switch (stm_energy_dimension) { 
                                    case STM_CONST:
                                        if (stm_energy_nx > 0 || stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_setconst (stm_model->ENERGYMODEL.ETABLE, *(float*)stm_energy_xydata->DATA);
                                            mbkfree((float*)stm_energy_xydata->DATA);
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                    case STM_1D:
                                        if (stm_energy_nx > 0 && stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_set1Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                            for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT)
                                                mbkfree((float*)stm_ch->DATA);
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                    case STM_2D:
                                        if (stm_energy_nx == 0 || stm_energy_ny == 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_set2Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                            for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT) {
                                                for (stm_chp = (chain_list*)stm_ch->DATA; stm_chp; stm_chp = stm_chp->NEXT)
                                                    mbkfree((float*)stm_chp->DATA);
                                                freechain ((chain_list*)stm_ch->DATA);
                                            }
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                }
                                for (stm_ch = stm_energy_xchain; stm_ch; stm_ch = stm_ch->NEXT)  
                                    mbkfree((float*)stm_ch->DATA);
                                for (stm_ch = stm_energy_ychain; stm_ch; stm_ch = stm_ch->NEXT)  
                                    mbkfree((float*)stm_ch->DATA);
                                freechain (stm_energy_xchain);
                                freechain (stm_energy_ychain);
                                break;
                        }
                        break;

                    /*************/
                    /* FCT model */
                    /*************/
                        
                    case STM_MOD_MODFCT:
                        stm_model = stm_addmodel (stm_cname, $3);
                        stm_model->VTH = vth;
                        stm_model->VDD = vdd;
                        stm_model->VT = vt;
                        stm_model->VF = vf;
                        stm_model->TTYPE = tran;
                        stm_model->UTYPE = STM_MOD_MODFCT;
                        stm_model->UMODEL.FUNCTION = stm_modfct_create (stm_fct_ins, stm_fct_localname, stm_fct_fct);
                        //stm_model->UMODEL.FUNCTION = stm_modfct_create (stm_fct_fct, stm_fct_file, stm_fct_ins, NULL, sim_model_number);
                        break;

                    /*************/
                    /* IV model */
                    /*************/
                        
                    case STM_MOD_MODIV:
                    
                        stm_model = stm_addmodel (stm_cname, $3);
                        
                        stm_model->VTH = vth;
                        stm_model->VDD = vdd;
                        stm_model->VT = vt;
                        stm_model->VF = vf;
                        stm_model->TTYPE = tran;
                        stm_model->UTYPE = STM_MOD_MODIV;
                        stm_model->UMODEL.IV = stm_modiv_alloc( stm_iv_nve, stm_iv_nvs );
                        
                        for( stm_iv_e = stm_iv_nve-1, stm_iv_scane = stm_iv_tabve ; stm_iv_e >= 0 ; stm_iv_e--, stm_iv_scane = stm_iv_scane->NEXT )
                          stm_model->UMODEL.IV->VE[stm_iv_e] = *((float*)(stm_iv_scane->DATA));
                          
                        for( stm_iv_s = stm_iv_nvs-1, stm_iv_scans = stm_iv_tabvs ; stm_iv_s >= 0 ; stm_iv_s--, stm_iv_scans = stm_iv_scans->NEXT )
                          stm_model->UMODEL.IV->VS[stm_iv_s] = *((float*)(stm_iv_scans->DATA));

                        for( stm_iv_e = stm_iv_nve-1, stm_iv_scane = stm_iv_tabiv ; stm_iv_e >= 0 ; stm_iv_e--, stm_iv_scane = stm_iv_scane->NEXT ) {
                          for( stm_iv_s = stm_iv_nvs-1, stm_iv_scans = (chain_list*)(stm_iv_scane->DATA) ; stm_iv_s >= 0 ; stm_iv_s-- , stm_iv_scans = stm_iv_scans->NEXT ) {
                            stm_modiv_setis( stm_model->UMODEL.IV, stm_iv_nve, stm_iv_nvs, *((float*)(stm_iv_scans->DATA)));
                          }
                        }

                        stm_modiv_set_cf( stm_model->UMODEL.IV, stm_iv_conf[3], stm_iv_conf[0], stm_iv_conf[1], stm_iv_conf[2] );
                        stm_modiv_set_in( stm_model->UMODEL.IV, stm_iv_input[0], stm_iv_input[1], stm_iv_input[2], stm_iv_input[3] );
                        stm_modiv_set_ti( stm_model->UMODEL.IV, stm_iv_init[0] );

                        for( stm_iv_scane = stm_iv_tabve ; stm_iv_scane ; stm_iv_scane = stm_iv_scane->NEXT )
                          mbkfree((float*)(stm_iv_scane->DATA));
                        freechain( stm_iv_tabve );

                        for( stm_iv_scans = stm_iv_tabvs ; stm_iv_scans ; stm_iv_scans = stm_iv_scans->NEXT )
                          mbkfree((float*)(stm_iv_scans->DATA));
                        freechain( stm_iv_tabvs );

                        for( stm_iv_scane = stm_iv_tabiv ; stm_iv_scane ; stm_iv_scane = stm_iv_scane->NEXT ) {
                          for( stm_iv_scans = (chain_list*)(stm_iv_scane->DATA) ; stm_iv_scans ; stm_iv_scans = stm_iv_scans->NEXT )
                            mbkfree((float*)(stm_iv_scans->DATA));
                          freechain( (chain_list*)(stm_iv_scane->DATA) );
                        }
                        freechain( stm_iv_tabiv );

                        switch( vnoise_model ) {
                            case STM_NOISE_SCR:
                                stm_noise_create( stm_model, STM_NOISE_SCR );
                                scr_model = stm_noise_getmodel_scr( stm_model );
                                stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                break;
                        }
                        switch (stm_energy_model_type) {
                            case STM_ENERGY_PARAMS :    
                                stm_e_p = stm_e ? stm_e_params : NULL;
                                stm_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                stm_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stm_e_p);
                                break;
                            case STM_ENERGY_TABLE:
                                stm_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                for (stm_energy_nx=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_nx++;
                                for (stm_energy_ny=0,stm_ch=stm_energy_xchain ; stm_ch ; stm_ch=stm_ch->NEXT) stm_energy_ny++;
                                stm_modtbl_setXrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_xchain, 1);
                                stm_modtbl_setYrange (stm_model->ENERGYMODEL.ETABLE, stm_energy_ychain, 1);
                                switch (stm_energy_dimension) { 
                                    case STM_CONST:
                                        if (stm_energy_nx > 0 || stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_setconst (stm_model->ENERGYMODEL.ETABLE, *(float*)stm_energy_xydata->DATA);
                                            mbkfree((float*)stm_energy_xydata->DATA);
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                    case STM_1D:
                                        if (stm_energy_nx > 0 && stm_energy_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_set1Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                            for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT)
                                                mbkfree((float*)stm_ch->DATA);
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                    case STM_2D:
                                        if (stm_energy_nx == 0 || stm_energy_ny == 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                        else {
                                            stm_modtbl_set2Dset (stm_model->ENERGYMODEL.ETABLE, stm_energy_xydata, 1);
                                            for (stm_ch = stm_energy_xydata; stm_ch; stm_ch = stm_ch->NEXT) {
                                                for (stm_chp = (chain_list*)stm_ch->DATA; stm_chp; stm_chp = stm_chp->NEXT)
                                                    mbkfree((float*)stm_chp->DATA);
                                                freechain ((chain_list*)stm_ch->DATA);
                                            }
                                            freechain (stm_energy_xydata);
                                        }
                                        break;
                                }
                                for (stm_ch = stm_energy_xchain; stm_ch; stm_ch = stm_ch->NEXT)  
                                    mbkfree((float*)stm_ch->DATA);
                                for (stm_ch = stm_energy_ychain; stm_ch; stm_ch = stm_ch->NEXT)  
                                    mbkfree((float*)stm_ch->DATA);
                                freechain (stm_energy_xchain);
                                freechain (stm_energy_ychain);
                                break;
                        }
                        break;
                } 
            }
            ;
            
tmodel      : thresholds tblmodel noisemodel energymodel
            {
                $$ = $2;
            }
            | thresholds scmmodel noisemodel energymodel 
            {
                $$ = $2;
            }
            | thresholds plnmodel noisemodel energymodel
            {
                $$ = $2;
            }
            | thresholds fctmodel
            {
                $$ = $2;
            }
            | thresholds ivmodel noisemodel energymodel
            {
                $$ = $2;
            }
            ;

thresholds  : _VTH '(' val ')' _VDD '(' val ')' _VT '(' val ')' _VF '(' val ')' transition
            {
                vth = $3.t_float;
                vdd = $7.t_float;
                vt = $11.t_float;
                vf = $15.t_float;
            }
            ;

transition  : empty
            {
              tran = STM_HL;
            }
            | _TRAN '(' _IDENTIFIER ')'
            {
              if(!strcmp($3,"uu"))
                  tran = STM_HH;
              else if(!strcmp($3,"ud"))
                  tran = STM_HL;
              else if(!strcmp($3,"du"))
                  tran = STM_LH;
              else if(!strcmp($3,"dd"))
                  tran = STM_LL;
              else
                  tran = STM_HL;
              mbkfree($3);
            }
            ;

/*****************************************************************************/
/* FCT model                                                                 */
/*****************************************************************************/

fctmodel    : _FCT '(' file function instance localname number ')'
            { 
                $$ = STM_MOD_MODFCT; 
            } 
            ;

file        : _FILE '(' _IDENTIFIER ')'
            {
              stm_fct_file = sensitive_namealloc($3); mbkfree($3);
            }
            | empty
            {
                stm_fct_file = NULL;
            }
            ;

localname   : _LOCALNAME '(' _IDENTIFIER ')'
            {
              stm_fct_localname = namealloc($3); mbkfree($3);
            }
            | empty
            {
                stm_fct_localname = NULL;
            }
            ;

instance    : _INSTANCE '(' _IDENTIFIER ')'
            {
              stm_fct_ins = namealloc($3); mbkfree($3);
            }
            | empty
            {
                stm_fct_ins = NULL;
            }
            ;

function    : _FUNCTION '(' _IDENTIFIER param_list ')'
            {
                stm_fct_fct = (APICallFunc *)mbkalloc(sizeof(APICallFunc));
                stm_fct_fct->NAME=sensitive_namealloc($3); mbkfree($3);
                stm_fct_fct->ARGS=$4;
            }
            | empty
            {
                stm_fct_fct = NULL;
            }
            ;

param_list  : param_list param
            {
              $$ = append($1, $2);
            }
            | empty
            {
              $$ = NULL;
            }
            ;

param       : _NUMBER
            {
              if (strchr($1.t_float_string,'.')!=NULL)
                $$=APIAddDoubleTARG(NULL, "?", $1.t_float);
              else
                $$=APIAddIntTARG(NULL, "?", atoi($1.t_float_string));
            }
            | _ECHAR
            { 
              $$ = APIAddCharTARG(NULL, "?", $1);
            }
            ;

number      : _SIM_MODEL '(' _NUMBER ')'
            {
                sim_model_number = $3.t_float;
            }
            | empty
            {
                sim_model_number = -1;
            }
            ;

/*****************************************************************************/
/* TBL model                                                                 */
/*****************************************************************************/

tbltype     : _SPLINE
            {
                $$ = STM_MOD_MODTBL;
            }
            | _CST
            {
                $$ = STM_MOD_MODTBL;
            }
            ;

tblmodel    : tbltype '(' axis axis data ')'
            { 
                stm_xchain    = $3.CHAIN;
                stm_xtype     = $3.TYPE;
                stm_ychain    = $4.CHAIN;
                stm_ytype     = $4.TYPE;
                stm_xydata    = $5.CHAIN;
                stm_dimension = $5.DIM;
                $$ = $1;
            } 
            | tbltype '(' axis data ')'
            { 
                stm_xchain    = $3.CHAIN;
                stm_xtype     = $3.TYPE;
                stm_ychain    = NULL;
                stm_ytype     = STM_NOTYPE;
                stm_xydata    = $4.CHAIN;
                stm_dimension = $4.DIM;
                $$ = $1;
            }
            | tbltype '(' data ')'
            { 
                stm_xchain    = NULL;
                stm_xtype     = STM_NOTYPE;
                stm_ychain    = NULL;
                stm_ytype     = STM_NOTYPE;
                stm_xydata    = $3.CHAIN;
                stm_dimension = $3.DIM;
                $$ = $1;
            } 
            ;

axis        : axistype '(' val val val_list ')' 
            { 
                stm_ptfl = (float*)mbkalloc (sizeof (float));
                *stm_ptfl = $4.t_float;
                $$.CHAIN = addchain ($5, stm_ptfl);
                stm_ptfl = (float*)mbkalloc (sizeof (float));
                *stm_ptfl = $3.t_float;
                $$.CHAIN = addchain ($$.CHAIN, stm_ptfl);
                $$.TYPE = $1;
            } 
            ;

axistype    : _INPUT_SLEW_AXIS 
            { 
                $$ = STM_INPUT_SLEW;
            }
            | _CLOCK_SLEW_AXIS       
            { 
                $$ = STM_CLOCK_SLEW;
            }
            | _SLEW_AXIS       
            { 
                $$ = STM_INPUT_SLEW;
            }
            | _LOAD_AXIS       
            { 
                $$ = STM_LOAD;
            } 
            ;

val_list    : empty        
            { 
                $$ = NULL; 
            }
            | val val_list  
            { 
                stm_ptfl = (float*)mbkalloc (sizeof (float));
                *stm_ptfl = $1.t_float;
                $$ = addchain ($2, stm_ptfl);
            } 
            ;

data        : _DATA '(' modeldata ')'       
            { 
                $$ = $3;
            } 
            ;
   
modeldata   : val     
            { 
                stm_ptfl = (float*)mbkalloc (sizeof(float));
                *stm_ptfl = $1.t_float;
                $$.CHAIN = addchain (NULL, stm_ptfl); 
                $$.DIM = STM_CONST;
            } 
            | array1D 
            { 
                $$.CHAIN = $1;
                $$.DIM = STM_1D;
            }
            | array2D 
            { 
                $$.CHAIN = $1;
                $$.DIM = STM_2D;
            } 
            ;

array2D     : array1D array1D array1D_list 
            { 
                $$ = addchain ($3, $2); 
                $$ = addchain ($$, $1); 
            } 
            ;

array1D     : '(' val val val_list ')' 
            { 
                stm_ptfl = (float*)mbkalloc (sizeof (float));
                *stm_ptfl = $3.t_float;
                $$ = addchain ($4, stm_ptfl);
                stm_ptfl = (float*)mbkalloc (sizeof(float));
                *stm_ptfl = $2.t_float;
                $$ = addchain ($$, stm_ptfl);
            } 
            ;

array1D_list: empty 
            { 
                $$ = NULL;
            }
            | array1D array1D_list 
            { 
                $$ = addchain ($2, $1);
            } 
            ;

/*****************************************************************************/
/* SCM model                                                                 */
/*****************************************************************************/

scmtype     : _SCM_DUAL
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_DUAL;
                stm_d = 0;
            }
            | _SCM_GOOD 
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_GOOD;
                stm_d = 0;
                stm_i = 0;
                stm_0 = 0;
                stm_n = 0;
                
            }
            | _SCM_FALSE
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_FALSE;
                stm_f = 0;
                stm_i = 0;
                stm_0 = 0;
                stm_n = 0;
            }
            | _SCM_PATH
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_PATH;
                stm_p = 0;
                stm_i = 0;
                stm_0 = 0;
                stm_n = 0;
            }
            | _SCM_CST
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_CST;
                stm_c = 0;
            }
            ;
            
scmmodel    : scmtype '(' params ')'
            ;

params      : dualparams
            | goodparams
            | falseparams
            | pathparams
            | cstparams
            ;

/* cst */

cstparams  : _CST '(' '(' _DELAY _SLEW ')'
              '(' val val ')' ')'
            {
                stm_c_params[STM_CST_DELAY] = $8.t_float;
                stm_c_params[STM_CST_SLEW]  = $9.t_float;
                stm_c = 1;
            }
            ;

/* dual */

dualparams  : _DUAL '(' '(' _PCONF0 _PCONF1 _CAPAI _CAPAI0 _CAPAI1 _CAPAI2 _CAPAI3 _VT0 _VT0C _IRAP _VDDIN _VT _THRESHOLD _IMAX _AN _BN _VDDMAX _RSAT _RLIN _DRC _RBR _CBR _INPUT_THR _RINT _VINT _CHALF _RCONF _KF _KR _QINIT ')'
              '(' val val val val val val val val val val val val val val val val val val val val val val val val val val val val val val ')' ')'
            {
                stm_d_params[STM_PCONF0]    = $36.t_float;
                stm_d_params[STM_PCONF1]    = $37.t_float;
                stm_d_params[STM_CAPAI]     = $38.t_float;
                stm_d_params[STM_CAPAI0]    = $39.t_float;
                stm_d_params[STM_CAPAI1]    = $40.t_float;
                stm_d_params[STM_CAPAI2]    = $41.t_float;
                stm_d_params[STM_CAPAI3]    = $42.t_float;
                stm_d_params[STM_VT0]       = $43.t_float;
                stm_d_params[STM_VT0C]      = $44.t_float;
                stm_d_params[STM_IRAP]      = $45.t_float;
                stm_d_params[STM_VDDIN]     = $46.t_float;
                stm_d_params[STM_VT]        = $47.t_float;
                stm_d_params[STM_THRESHOLD] = $48.t_float;
                stm_d_params[STM_IMAX]      = $49.t_float;
                stm_d_params[STM_AN]        = $50.t_float;
                stm_d_params[STM_BN]        = $51.t_float;
                stm_d_params[STM_VDDMAX]    = $52.t_float;
                stm_d_params[STM_RSAT]      = $53.t_float;
                stm_d_params[STM_RLIN]      = $54.t_float;
                stm_d_params[STM_DRC]       = $55.t_float;
                stm_d_params[STM_RBR]       = $56.t_float;
                stm_d_params[STM_CBR]       = $57.t_float;
                stm_d_params[STM_INPUT_THR] = $58.t_float;
                stm_d_params[STM_RINT]      = $59.t_float;
                stm_d_params[STM_VINT]      = $60.t_float;
                stm_d_params[STM_CHALF]     = $61.t_float;
                stm_d_params[STM_RCONF]     = $62.t_float;
                stm_d_params[STM_KF]        = $63.t_float;
                stm_d_params[STM_KRT]       = $64.t_float;
                stm_d_params[STM_QINIT]     = $65.t_float;
                stm_d_params[STM_CAPAO]     = stm_d_params[STM_CAPAI];
                stm_d = 1;
            }
            | _DUAL '(' '(' _PCONF0 _PCONF1 _CAPAI _IRAP _VDDIN _VT _THRESHOLD _IMAX _AN _BN _VDDMAX _RSAT _RLIN _DRC _RBR _CBR _INPUT_THR _RINT _VINT _CHALF _RCONF _KF _KR _QINIT ')'
              '(' val val val val val val val val val val val val val val val val val val val val val val val val ')' ')'
            {
                stm_d_params[STM_PCONF0]    = $30.t_float;
                stm_d_params[STM_PCONF1]    = $31.t_float;
                stm_d_params[STM_CAPAI]     = $32.t_float;
                stm_d_params[STM_CAPAI0]    = $32.t_float;
                stm_d_params[STM_CAPAI1]    = $32.t_float;
                stm_d_params[STM_CAPAI2]    = $32.t_float;
                stm_d_params[STM_CAPAI3]    = $32.t_float;
                stm_d_params[STM_VT0]       = $35.t_float;
                stm_d_params[STM_VT0C]      = $40.t_float-$35.t_float;
                stm_d_params[STM_IRAP]      = $33.t_float;
                stm_d_params[STM_VDDIN]     = $34.t_float;
                stm_d_params[STM_VT]        = $35.t_float;
                stm_d_params[STM_THRESHOLD] = $36.t_float;
                stm_d_params[STM_IMAX]      = $37.t_float;
                stm_d_params[STM_AN]        = $38.t_float;
                stm_d_params[STM_BN]        = $39.t_float;
                stm_d_params[STM_VDDMAX]    = $40.t_float;
                stm_d_params[STM_RSAT]      = $41.t_float;
                stm_d_params[STM_RLIN]      = $42.t_float;
                stm_d_params[STM_DRC]       = $43.t_float;
                stm_d_params[STM_RBR]       = $44.t_float;
                stm_d_params[STM_CBR]       = $45.t_float;
                stm_d_params[STM_INPUT_THR] = $46.t_float;
                stm_d_params[STM_RINT]      = $47.t_float;
                stm_d_params[STM_VINT]      = $48.t_float;
                stm_d_params[STM_CHALF]     = $49.t_float;
                stm_d_params[STM_RCONF]     = $50.t_float;
                stm_d_params[STM_KF]        = $51.t_float;
                stm_d_params[STM_KRT]       = $52.t_float;
                stm_d_params[STM_QINIT]     = $53.t_float;
                stm_d_params[STM_CAPAO]     = stm_d_params[STM_CAPAI];
                stm_d = 1;
            }

            ;

/* good */

goodparams  : link0p linkips linknp dualparams
            ;

/* false */

falseparams : link0p linkips linknp falselink
            ;
            
/* path */

pathparams  : link0p linkips pathlink
            ;

links       : '(' val ')'
            {
                stm_i_params[STM_NB_I_LINKS] = $2.t_float;
                stm_index = 1;
                stm_i = 1;
            }
            ;

link0p      : _LINK_OUT '(' '(' _CI _CF _K3 _K4 _K5 ')' '(' val val val val val ')' ')'
            {
                stm_0_params[STM_CI_0] = $11.t_float;
                stm_0_params[STM_CF_0] = $12.t_float;
                stm_0_params[STM_K3_0] = $13.t_float;
                stm_0_params[STM_K4_0] = $14.t_float;
                stm_0_params[STM_K5_0] = $15.t_float;
                stm_0 = 1;
            }
            ;

linkips     : _LINK_PASS '(' links '(' _CI _CF _ACTI _BCTI _K3 _K4 _K5 ')' linkip_list ')'
            | empty
            ;

linkip_list : linkip_list linkip
            | linkip
            ;

linkip      : '(' val val val val val val val')'
            {
                stm_i_params[stm_index + STM_CI_I]   = $2.t_float;
                stm_i_params[stm_index + STM_CF_I]   = $3.t_float;
                stm_i_params[stm_index + STM_ACTI_I] = $4.t_float;
                stm_i_params[stm_index + STM_BCTI_I] = $5.t_float;
                stm_i_params[stm_index + STM_K3_I]   = $6.t_float;
                stm_i_params[stm_index + STM_K4_I]   = $7.t_float;
                stm_i_params[stm_index + STM_K5_I]   = $8.t_float;
                stm_index += STM_NB_LINK_I_PARAMS;
            }
            ;
            
linknp      : _LINK_DUAL '(' '(' _CI _CF _ACTI _BCTI ')' '(' val val val val ')' ')'
            {
                stm_n_params[STM_CI_N]   = $10.t_float;
                stm_n_params[STM_CF_N]   = $11.t_float;
                stm_n_params[STM_ACTI_N] = $12.t_float;
                stm_n_params[STM_BCTI_N] = $13.t_float;
                stm_n = 1;
            }
            | empty
            ;

/* falselink */

falselink   : _FALSE '(' '(' _PCONF0 _PCONF1 _RTOT _KF _VDDMAX _DRC ')' '(' val val val val val val ')' ')'
            {
                stm_f_params[STM_PCONF0_F] = $12.t_float;
                stm_f_params[STM_PCONF1_F] = $13.t_float;
                stm_f_params[STM_RTOT_F]   = $14.t_float;
                stm_f_params[STM_K_F]      = $15.t_float;
                stm_f_params[STM_VDDMAX_F] = $16.t_float;
                stm_f_params[STM_DRC_F]    = $17.t_float;
                stm_f_params[STM_VF_INPUT_F] = (float)(-1.0);
                stm_f = 1;
            }
            | _FALSE '(' '(' _PCONF0 _PCONF1 _RTOT _KF _VDDMAX _DRC _VF_INPUT ')' '(' val val val val val val val ')' ')'
            {
                stm_f_params[STM_PCONF0_F]   = $13.t_float;
                stm_f_params[STM_PCONF1_F]   = $14.t_float;
                stm_f_params[STM_RTOT_F]     = $15.t_float;
                stm_f_params[STM_K_F]        = $16.t_float;
                stm_f_params[STM_VDDMAX_F]   = $17.t_float;
                stm_f_params[STM_DRC_F]      = $18.t_float;
                stm_f_params[STM_VF_INPUT_F] = $19.t_float;
                stm_f = 1;
            }

            ;

/* pathlink */

pathlink    : _PATH '(' '(' _PCONF0 _VDDMAX ')' '(' val val ')' ')'
            {
                stm_p_params[STM_PCONF0_F] = $8.t_float;
                stm_p_params[STM_VDDMAX_P] = $9.t_float;
                stm_p = 1;
            }
            | _PATH '(' '(' _PCONF0 _VDDMAX _VF_INPUT ')' '(' val val val ')' ')'
            {
                stm_p_params[STM_PCONF0_F]   = $9.t_float;
                stm_p_params[STM_VDDMAX_P]   = $10.t_float;
                stm_p = 1;
            }

            ;

/*****************************************************************************/
/* IV model                                                                  */
/*****************************************************************************/

ivmodel     : _IV '(' nbve nbvs voltageve voltagevs current conflict input initial ')'
            {
              $$=STM_MOD_MODIV;
            }
            ;

nbve        : _NVE '(' val ')'
            {
              stm_iv_nve = atoi( $3.t_float_string );
            }
            ;

nbvs        : _NVS '(' val ')'
            {
              stm_iv_nvs = atoi( $3.t_float_string );
            }
            ;

voltageve   : _VE array1D
            {
              stm_iv_tabve = $2;
            }
            ;

voltagevs   : _VS array1D
            {
              stm_iv_tabvs = $2;
            }
            ;

current     : _CURRENT '(' array2D ')'
            {
              stm_iv_tabiv = $3;
            }
            ;

conflict    : _CONFLICT '(' val val val val ')'
            {
              stm_iv_conf[0]=$3.t_float;
              stm_iv_conf[1]=$4.t_float;
              stm_iv_conf[2]=$5.t_float;
              stm_iv_conf[3]=$6.t_float;
            }
            ;

input       : _INPUT '(' val val val val ')'
            {
              stm_iv_input[0]=$3.t_float;
              stm_iv_input[1]=$4.t_float;
              stm_iv_input[2]=$5.t_float;
              stm_iv_input[3]=$6.t_float;
            }
            ;

initial     : _INITIAL '(' val ')'
            {
              stm_iv_init[0]=$3.t_float;
            }
            ;

/*****************************************************************************/
/* PLN model                                                                 */
/*****************************************************************************/

plnmodel    : _SPDM '(' varparams_list ordersparams coefsparams ')'
            {
                stm_nb_var = stm_deg_index;
                stm_deg_index = 0;
                stm_var_index = 0;
                stm_coeftab_index = 0;
                stm_coefdef_index = 0;
                stm_coefdef_nb = 0;
                stm_coefdef_mask = STM_COEF_DEFMASK;
                $$ = STM_MOD_MODPLN;
            }
            ;

/* varparams */

varparams_list  : empty
                | varparams_list varparams
                ;

varparams    : axistype _VARTHMIN '(' val ')' _VARTHMAX '(' val ')'
             {
                stm_var_params[stm_var_index] = (timing_varpolynom*)mbkalloc (sizeof (struct timing_varpolynom));
                stm_var_params[stm_var_index]->TYPE = $1;
                stm_var_params[stm_var_index]->THMIN = $4.t_float;
                stm_var_params[stm_var_index]->THMAX = $8.t_float;
                stm_var_index++;
             }
             ;

             
/* orders */

orders_list  : orders_list orders_i
             | orders_i
             ;

orders_i     : val 
             {
                stm_deg_params[stm_deg_index] = $1.t_float;
                stm_deg_index++;
             }
             ;
             
ordersparams : _ORDERS '(' orders_list ')'
             ;

/* coefs */

coefs_list  : coefs_list coefs_i
            | coefs_i
            ;

coefs_i     : val 
            {
                stm_coefdef_nb++;
                if($1.t_float){
                    stm_coeftab_params[stm_coeftab_index] = $1.t_float;
                    stm_coeftab_index ++;
                    stm_coefdef_params[stm_coefdef_index] = stm_coefdef_params[stm_coefdef_index] | stm_coefdef_mask;
                }
                stm_coefdef_mask = stm_coefdef_mask << 1;
                if(!((stm_coefdef_nb) % (sizeof(long) * 8))){
                    stm_coefdef_index++;
                    stm_coefdef_mask = STM_COEF_DEFMASK;
                }    
            }
            ;
            
coefsparams : _COEFS '(' coefs_list ')'
            ;

/*****************************************************************************/
/* NOISE model                                                               */
/*****************************************************************************/

noisemodel  : _NOISE_SCR '(' val val ')'
            {
               vnoise_model = STM_NOISE_SCR;
               vnoise_scr = $3.t_float;
               vnoise_invth = $4.t_float;
            }
            | _NOISE_SCR '(' val ')'
            {
               vnoise_model = STM_NOISE_SCR;
               vnoise_scr = $3.t_float;
               vnoise_invth = vdd/2 ; 
            }
            | empty
            {
               vnoise_model = STM_NOISE_NULL;
            }
            ;


/*****************************************************************************/
/* ENERGY model                                                              */
/*****************************************************************************/
eparams :   '(' _PCONF0 _PCONF1 ')'
            '(' val val ')'
        {
            stm_e_params [STM_EPCONF0] = $6.t_float;
            stm_e_params [STM_EPCONF1] = $7.t_float;
            stm_e = 1;
        }
        ;

energyparamsmodel   : _ENERGY_MODEL 
                    {
                        stm_e = 0;
                    }
                    '(' eparams ')'
                    {
                        $$ = STM_ENERGY_PARAMS;
                    }
                    ;

energytbltype   : _ENERGY_TABLE
                {
                    $$ = STM_ENERGY_TABLE;
                }
                | _ENERGY_CONST
                {
                    $$ = STM_ENERGY_TABLE;
                }
                ;

energytblmodel  : energytbltype '(' axis axis data ')'
                {
                    stm_energy_xchain    = $3.CHAIN;
                    stm_energy_xtype     = $3.TYPE;
                    stm_energy_ychain    = $4.CHAIN;
                    stm_energy_ytype     = $4.TYPE;
                    stm_energy_xydata    = $5.CHAIN;
                    stm_energy_dimension = $5.DIM;
                    $$ = $1;
                }
                | energytbltype '(' axis data ')'
                {
                    stm_energy_xchain    = $3.CHAIN;
                    stm_energy_xtype     = $3.TYPE;
                    stm_energy_ychain    = NULL;
                    stm_energy_ytype     = STM_NOTYPE;
                    stm_energy_xydata    = $4.CHAIN;
                    stm_energy_dimension = $4.DIM;
                    $$ = $1;
                }
                | energytbltype '(' data ')'
                {
                    stm_energy_xchain    = NULL;
                    stm_energy_xtype     = STM_NOTYPE;
                    stm_energy_ychain    = NULL;
                    stm_energy_ytype     = STM_NOTYPE;
                    stm_energy_xydata    = $3.CHAIN;
                    stm_energy_dimension = $3.DIM;
                    $$ = $1;
                }
                ;

energymodel : energytblmodel
            {
                stm_energy_model_type = $1;
            }
            | energyparamsmodel
            {
                stm_energy_model_type = $1;
            }
            | empty
            {
                stm_energy_model_type = STM_ENERGY_NULL;
            }
            ;

/*****************************************************************************/


modelname   : _NAME '(' _IDENTIFIER ')'
            { 
              $$ = namealloc($3); mbkfree($3);
            } 
            ;

cellname    : _NAME '(' _IDENTIFIER ')' 
            { 
              $$ = namealloc($3); mbkfree($3);
            }
            ;

val         : _NUMBER 
            { 
                $$ = $1;
            }
            | _ENUMBER 
            { 
                $$ = $1;
            }
            | _UNKNOWN
            {
                $$.t_float = STM_UNKNOWN;
                strcpy($$.t_float_string,"");
            }
            ;

empty       : /* empty */ 
            ;

%%

int yyerror ()
{
    avt_errmsg (STM_ERRMSG, "001", AVT_FATAL, STM_FILE, STM_LINE);
    return 0;
}

