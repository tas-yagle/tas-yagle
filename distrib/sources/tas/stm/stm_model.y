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

static chain_list   *stmmodel_ch, *stmmodel_chp, *stmmodel_xchain, *stmmodel_ychain, *stmmodel_xydata;
static chain_list   *stmmodel_energy_xchain, *stmmodel_energy_ychain, *stmmodel_energy_xydata;
static char          stmmodel_xtype, stmmodel_ytype;
static char          stmmodel_energy_xtype, stmmodel_energy_ytype;
static char          stmmodel_dimension;
static char          stmmodel_energy_dimension;
static int           stmmodel_nx, stmmodel_ny;
static int           stmmodel_energy_nx, stmmodel_energy_ny;
static float        *stmmodel_ptfl;
static timing_model *stmmodel_model;
static timing_cell  *stmmodel_cell;
static int           stmmodel_index;
static float         stmmodel_d_params[STM_NB_DUAL_PARAMS];
static float         stmmodel_f_params[STM_NB_FALSE_PARAMS];
static float         stmmodel_p_params[STM_NB_PATH_PARAMS];
static float         stmmodel_c_params[STM_NB_CST_PARAMS];
static float         stmmodel_i_params[1 + STM_NB_LINK_I_PARAMS * 16]; /* max = 16 pass transistors */
static float         stmmodel_0_params[STM_NB_LINK_0_PARAMS];
static float         stmmodel_n_params[STM_NB_LINK_N_PARAMS];
static float         stmmodel_e_params[STM_NB_ENERGY_PARAMS];
static long          stmmodel_deg_params[10];
static long          stmmodel_coefdef_params[1024];
static float         stmmodel_coeftab_params[1024];
static timing_varpolynom *stmmodel_var_params[3];
static timing_varpolynom **stmmodel_var_p;
static long         *stmmodel_deg_p;
static long         *stmmodel_coefdef_p;
static float        *stmmodel_coeftab_p;
static int           stmmodel_nb_var = 0;
static int           stmmodel_deg_index = 0;
static int           stmmodel_var_index = 0;
static int           stmmodel_coefdef_index = 0;
static int           stmmodel_coefdef_nb = 0;
static int           stmmodel_coeftab_index = 0;
static float        *stmmodel_d_p;
static float        *stmmodel_f_p;
static float        *stmmodel_p_p;
static float        *stmmodel_c_p;
static float        *stmmodel_i_p;
static float        *stmmodel_0_p;
static float        *stmmodel_n_p;
static float        *stmmodel_e_p;
static char          stmmodel_d;
static char          stmmodel_f;
static char          stmmodel_p;
static char          stmmodel_c;
static char          stmmodel_i;
static char          stmmodel_0;
static char          stmmodel_n;
static char          stmmodel_e;
static long          stmmodel_coefdef_mask = STM_COEF_DEFMASK;
static float         vth;
static float         vdd;
static float         vt;
static float         vf;
static char          tran;
static char          vnoise_model;
static float         vnoise_scr;
static float         vnoise_invth;
static noise_scr    *scr_model;
static char          stmmodel_energy_model_type;
static char         *stmmodel_fct_ins;
static char         *stmmodel_fct_file;
static char         *stmmodel_fct_localname;
static APICallFunc  *stmmodel_fct_fct;
static int           sim_model_number;
static float         stmmodel_iv_conf[4];
static float         stmmodel_iv_input[4];
static float         stmmodel_iv_init[1];
static int           stmmodel_iv_nve;
static int           stmmodel_iv_nvs;
static chain_list   *stmmodel_iv_tabve;
static chain_list   *stmmodel_iv_tabvs;
static chain_list   *stmmodel_iv_tabiv;
static int           stmmodel_iv_e;
static int           stmmodel_iv_s;
static chain_list   *stmmodel_iv_scane;
static chain_list   *stmmodel_iv_scans;

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
%token            _RINT
%token            _VINT
%token            _CHALF
%token            _QINIT
%token            _RCONF
%token            _KR
%token            _INPUT_THR
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
%token            _CLOCK_SLEW_AXIS
%token            _CST
%token <t_struct3>  _ENUMBER
%token <t_pchar>  _IDENTIFIER
%token            _INPUT_SLEW_AXIS
%token            _LOAD_AXIS
%token            _MODEL
%token <t_struct3>  _NUMBER
%token            _SLEW_AXIS
%token            _SPLINE
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

%start first_model 

%type <t_pchar>   modelname
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
first_model : model 
            {
                YYACCEPT;
            }
            ;

model       : _MODEL '(' modelname tmodel ')' 
            { 
                STM_MNAME = $3;
                switch ($4 & STM_MOD_TYPEMASK) {
                
                    /*************/
                    /* TBL model */
                    /*************/

                    case STM_MOD_MODTBL :
                        if ((STM_CACHE==NULL)||(STM_PRELOAD=='Y')) {
                            stmmodel_model = stm_addmodel (STM_CNAME, $3); 
                            stmmodel_model->VTH = vth;
                            stmmodel_model->VDD = vdd;
                            stmmodel_model->VT = vt;
                            stmmodel_model->VF = vf;
                            stmmodel_model->TTYPE = tran;
                            stmmodel_model->UTYPE = STM_MOD_MODTBL;
                        }
                        if ((STM_PRELOAD=='N') && stmmodel_xydata) {
                  //    /* When chache is not used, STM_PRELOAD== 'N' anyway */
//                             printf ("\tPARSE model: %s\n", $3);
                             if (STM_CACHE != NULL) {
                                stmmodel_cell = stm_getcell (STM_CNAME);
                                stmmodel_model = (timing_model*)gethtitem (stmmodel_cell->MODEL_HT, namealloc ($3));
                             }
                             for (stmmodel_nx = 0, stmmodel_ch = stmmodel_xchain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) stmmodel_nx++;
                             for (stmmodel_ny = 0, stmmodel_ch = stmmodel_ychain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) stmmodel_ny++;
                             stmmodel_model->UMODEL.TABLE = stm_modtbl_create (stmmodel_nx, stmmodel_ny, stmmodel_xtype, stmmodel_ytype);
                             switch( vnoise_model ) {
                                 case STM_NOISE_SCR:
                                     stm_noise_create( stmmodel_model, STM_NOISE_SCR );
                                     scr_model = stm_noise_getmodel_scr( stmmodel_model );
                                     stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                     break;
                             }
                             stm_modtbl_setXrange (stmmodel_model->UMODEL.TABLE, stmmodel_xchain, 1);
                             stm_modtbl_setYrange (stmmodel_model->UMODEL.TABLE, stmmodel_ychain, 1);
                             switch (stmmodel_dimension) { 
                                 case STM_CONST:
                                     if (stmmodel_nx > 0 || stmmodel_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                     else {
                                         stm_modtbl_setconst (stmmodel_model->UMODEL.TABLE, *(float*)stmmodel_xydata->DATA);
                                         mbkfree((float*)stmmodel_xydata->DATA);
                                         freechain (stmmodel_xydata);
                                    }
                                 break;
                                 case STM_1D:
                                     if (stmmodel_nx > 0 && stmmodel_ny > 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                     else {
                                         stm_modtbl_set1Dset (stmmodel_model->UMODEL.TABLE, stmmodel_xydata, 1);
                                         for (stmmodel_ch = stmmodel_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)
                                             mbkfree((float*)stmmodel_ch->DATA);
                                         freechain (stmmodel_xydata);
                                    }
                                 break;
                                 case STM_2D:
                                     if (stmmodel_nx == 0 || stmmodel_ny == 0)
                                         avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                     else {
                                         stm_modtbl_set2Dset (stmmodel_model->UMODEL.TABLE, stmmodel_xydata, 1);
                                         for (stmmodel_ch = stmmodel_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) {
                                             for (stmmodel_chp = (chain_list*)stmmodel_ch->DATA; stmmodel_chp; stmmodel_chp = stmmodel_chp->NEXT)
                                                 mbkfree((float*)stmmodel_chp->DATA);
                                             freechain ((chain_list*)stmmodel_ch->DATA);
                                        }
                                         freechain (stmmodel_xydata);
                                     }
                                 break;
                            }
                            switch (stmmodel_energy_model_type) {
                                case STM_ENERGY_PARAMS :    
                                    stmmodel_e_p = stmmodel_e ? stmmodel_e_params : NULL;
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                    stmmodel_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stmmodel_e_p);
                                    break;
                                case STM_ENERGY_TABLE:
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                    for (stmmodel_energy_nx=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_nx++;
                                    for (stmmodel_energy_ny=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_ny++;
                                    stm_modtbl_setXrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xchain, 1);
                                    stm_modtbl_setYrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_ychain, 1);
                                    switch (stmmodel_energy_dimension) { 
                                        case STM_CONST:
                                            if (stmmodel_energy_nx > 0 || stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_setconst (stmmodel_model->ENERGYMODEL.ETABLE, *(float*)stmmodel_energy_xydata->DATA);
                                                mbkfree((float*)stmmodel_energy_xydata->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_1D:
                                            if (stmmodel_energy_nx > 0 && stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set1Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)
                                                    mbkfree((float*)stmmodel_ch->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_2D:
                                            if (stmmodel_energy_nx == 0 || stmmodel_energy_ny == 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set2Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) {
                                                    for (stmmodel_chp = (chain_list*)stmmodel_ch->DATA; stmmodel_chp; stmmodel_chp = stmmodel_chp->NEXT)
                                                        mbkfree((float*)stmmodel_chp->DATA);
                                                    freechain ((chain_list*)stmmodel_ch->DATA);
                                                }
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                    }
                                    for (stmmodel_ch = stmmodel_energy_xchain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    for (stmmodel_ch = stmmodel_energy_ychain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    freechain (stmmodel_energy_xchain);
                                    freechain (stmmodel_energy_ychain);
                                    break;
                            }                            
                        } 
                        for (stmmodel_ch = stmmodel_xchain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                            mbkfree((float*)stmmodel_ch->DATA);
                        for (stmmodel_ch = stmmodel_ychain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                            mbkfree((float*)stmmodel_ch->DATA);
                        freechain (stmmodel_xchain);
                        freechain (stmmodel_ychain);
                        break;

                    /*************/
                    /* SCM model */
                    /*************/
                        
                    case STM_MOD_MODSCM:
                        if ((STM_CACHE==NULL)||(STM_PRELOAD=='Y')) {
                            stmmodel_model = stm_addmodel (STM_CNAME, $3);
                            stmmodel_model->VTH = vth;
                            stmmodel_model->VDD = vdd;
                            stmmodel_model->VT = vt;
                            stmmodel_model->VF = vf;
                            stmmodel_model->TTYPE = tran;
                            stmmodel_model->UTYPE = STM_MOD_MODSCM;
                        }
                        if (STM_PRELOAD=='N') {
                  //    /* When chache is not used, STM_PRELOAD== 'N' anyway */
//                             printf ("\tPARSE model: %s\n", $3);
                            if (STM_CACHE != NULL){
                                stmmodel_cell = stm_getcell (STM_CNAME);
                                stmmodel_model = (timing_model*)gethtitem (stmmodel_cell->MODEL_HT, namealloc ($3));
                            }
                            switch( vnoise_model ) {
                                case STM_NOISE_SCR:
                                    stm_noise_create( stmmodel_model, STM_NOISE_SCR );
                                    scr_model = stm_noise_getmodel_scr( stmmodel_model );
                                    stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                    break;
                            }
                            switch ($4 & STM_MODSCM_TYPEMASK) {
                                case STM_MODSCM_DUAL:
                                    stmmodel_d_p = stmmodel_d ? stmmodel_d_params : NULL;
                                    stmmodel_model->UMODEL.SCM = stm_modscm_dual_create (stmmodel_d_p);
                                    break;
                                case STM_MODSCM_GOOD:
                                    stmmodel_0_p = stmmodel_0 ? stmmodel_0_params : NULL;
                                    stmmodel_i_p = stmmodel_i ? stmmodel_i_params : NULL;
                                    stmmodel_n_p = stmmodel_n ? stmmodel_n_params : NULL;
                                    stmmodel_d_p = stmmodel_d ? stmmodel_d_params : NULL;
                                    stmmodel_model->UMODEL.SCM = stm_modscm_good_create (stmmodel_0_p, stmmodel_i_p, stmmodel_n_p, stmmodel_d_p);
                                    break;
                                case STM_MODSCM_FALSE:
                                    stmmodel_0_p = stmmodel_0 ? stmmodel_0_params : NULL;
                                    stmmodel_i_p = stmmodel_i ? stmmodel_i_params : NULL;
                                    stmmodel_n_p = stmmodel_n ? stmmodel_n_params : NULL;
                                    stmmodel_f_p = stmmodel_f ? stmmodel_f_params : NULL;
                                    stmmodel_model->UMODEL.SCM = stm_modscm_false_create (stmmodel_0_p, stmmodel_i_p, stmmodel_n_p, stmmodel_f_p);
                                    break;
                                case STM_MODSCM_PATH:
                                    stmmodel_0_p = stmmodel_0 ? stmmodel_0_params : NULL;
                                    stmmodel_i_p = stmmodel_i ? stmmodel_i_params : NULL;
                                    stmmodel_n_p = stmmodel_n ? stmmodel_n_params : NULL;
                                    stmmodel_p_p = stmmodel_p ? stmmodel_p_params : NULL;
                                    stmmodel_model->UMODEL.SCM = stm_modscm_path_create (stmmodel_0_p, stmmodel_i_p, stmmodel_n_p, stmmodel_p_p);
                                    break;
                                case STM_MODSCM_CST:
                                    stmmodel_c_p = stmmodel_c ? stmmodel_c_params : NULL;
                                    stmmodel_model->UMODEL.SCM = stm_modscm_cst_create (stmmodel_c_p);
                                    break;
                            }
                            switch (stmmodel_energy_model_type) {
                                case STM_ENERGY_PARAMS :    
                                    stmmodel_e_p = stmmodel_e ? stmmodel_e_params : NULL;
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                    stmmodel_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stmmodel_e_p);
                                    break;
                                case STM_ENERGY_TABLE:
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                    for (stmmodel_energy_nx=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_nx++;
                                    for (stmmodel_energy_ny=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_ny++;
                                    stm_modtbl_setXrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xchain, 1);
                                    stm_modtbl_setYrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_ychain, 1);
                                    switch (stmmodel_energy_dimension) { 
                                        case STM_CONST:
                                            if (stmmodel_energy_nx > 0 || stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_setconst (stmmodel_model->ENERGYMODEL.ETABLE, *(float*)stmmodel_energy_xydata->DATA);
                                                mbkfree((float*)stmmodel_energy_xydata->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }   
                                            break;
                                        case STM_1D:
                                            if (stmmodel_energy_nx > 0 && stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set1Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)
                                                    mbkfree((float*)stmmodel_ch->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_2D:
                                            if (stmmodel_energy_nx == 0 || stmmodel_energy_ny == 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set2Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) {
                                                    for (stmmodel_chp = (chain_list*)stmmodel_ch->DATA; stmmodel_chp; stmmodel_chp = stmmodel_chp->NEXT)
                                                        mbkfree((float*)stmmodel_chp->DATA);
                                                    freechain ((chain_list*)stmmodel_ch->DATA);
                                                }
                                                freechain (stmmodel_energy_xydata);
                                            }   
                                            break;
                                    }
                                    for (stmmodel_ch = stmmodel_energy_xchain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    for (stmmodel_ch = stmmodel_energy_ychain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    freechain (stmmodel_energy_xchain);
                                    freechain (stmmodel_energy_ychain);
                                    break;
                            }                            
                        }
                        break;

                    /*************/
                    /* PLN model */
                    /*************/
                        
                    case STM_MOD_MODPLN:
                        if ((STM_CACHE==NULL)||(STM_PRELOAD=='Y')) {
                            stmmodel_model = stm_addmodel (STM_CNAME, $3);
                            stmmodel_model->VTH = vth;
                            stmmodel_model->VDD = vdd;
                            stmmodel_model->VT = vt;
                            stmmodel_model->VF = vf;
                            stmmodel_model->TTYPE = tran;
                            stmmodel_model->UTYPE = STM_MOD_MODPLN;
                        }
                        if (STM_PRELOAD=='N') {
                            if (STM_CACHE != NULL){
                                stmmodel_cell = stm_getcell (STM_CNAME);
                                stmmodel_model = (timing_model*)gethtitem (stmmodel_cell->MODEL_HT, namealloc ($3));
                            }
                            switch( vnoise_model ) {
                                case STM_NOISE_SCR:
                                    stm_noise_create( stmmodel_model, STM_NOISE_SCR );
                                    scr_model = stm_noise_getmodel_scr( stmmodel_model );
                                    stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                    break;
                            }
                            stmmodel_var_p = stmmodel_var_params;
                            stmmodel_deg_p = stmmodel_deg_params;
                            stmmodel_coefdef_p = stmmodel_coefdef_params;
                            stmmodel_coeftab_p = stmmodel_coeftab_params;
                            stmmodel_model->UMODEL.POLYNOM = stm_modpln_create (stmmodel_nb_var, stmmodel_deg_p, stmmodel_coefdef_p, stmmodel_coeftab_p, stmmodel_var_p);

                            switch (stmmodel_energy_model_type) {
                                case STM_ENERGY_PARAMS :    
                                    stmmodel_e_p = stmmodel_e ? stmmodel_e_params : NULL;
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                    stmmodel_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stmmodel_e_p);
                                    break;
                                case STM_ENERGY_TABLE:
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                    for (stmmodel_energy_nx=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_nx++;
                                    for (stmmodel_energy_ny=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_ny++;
                                    stm_modtbl_setXrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xchain, 1);
                                    stm_modtbl_setYrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_ychain, 1);
                                    switch (stmmodel_energy_dimension) { 
                                        case STM_CONST:
                                            if (stmmodel_energy_nx > 0 || stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_setconst (stmmodel_model->ENERGYMODEL.ETABLE, *(float*)stmmodel_energy_xydata->DATA);
                                                mbkfree((float*)stmmodel_energy_xydata->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_1D:
                                            if (stmmodel_energy_nx > 0 && stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set1Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)
                                                    mbkfree((float*)stmmodel_ch->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_2D:
                                            if (stmmodel_energy_nx == 0 || stmmodel_energy_ny == 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set2Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) {
                                                    for (stmmodel_chp = (chain_list*)stmmodel_ch->DATA; stmmodel_chp; stmmodel_chp = stmmodel_chp->NEXT)
                                                        mbkfree((float*)stmmodel_chp->DATA);
                                                    freechain ((chain_list*)stmmodel_ch->DATA);
                                                }
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                    }
                                    for (stmmodel_ch = stmmodel_energy_xchain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    for (stmmodel_ch = stmmodel_energy_ychain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    freechain (stmmodel_energy_xchain);
                                    freechain (stmmodel_energy_ychain);
                                    break;
                            }
                        }
                        break;

                    /*************/
                    /* FCT model */
                    /*************/
                        
                    case STM_MOD_MODFCT:
//                        printf("warning: parsing functions in stm is not done yet...\n");
                        if ((STM_CACHE==NULL)||(STM_PRELOAD=='Y')) {
                            stmmodel_model = stm_addmodel (STM_CNAME, $3);
                            stmmodel_model->VTH = vth;
                            stmmodel_model->VDD = vdd;
                            stmmodel_model->VT = vt;
                            stmmodel_model->VF = vf;
                            stmmodel_model->TTYPE = tran;
                            stmmodel_model->UTYPE = STM_MOD_MODFCT;
                        }
                        if (STM_PRELOAD=='N') {
                            if (STM_CACHE != NULL) {
                                stmmodel_cell = stm_getcell (STM_CNAME);
                                stmmodel_model = (timing_model*)gethtitem (stmmodel_cell->MODEL_HT, namealloc ($3));
                            }
                            stmmodel_model->UMODEL.FUNCTION = stm_modfct_create (stmmodel_fct_ins, stmmodel_fct_localname, stmmodel_fct_fct);
                            //stmmodel_model->UMODEL.FUNCTION = stm_modfct_create (stmmodel_fct_fct, stmmodel_fct_file, stmmodel_fct_ins, NULL, sim_model_number);
                        }    
                        break;

                    /*************/
                    /* IV model */
                    /*************/
                        
                    case STM_MOD_MODIV:
                        if ((STM_CACHE==NULL)||(STM_PRELOAD=='Y')) {
                            stmmodel_model = stm_addmodel (STM_CNAME, $3);
                            stmmodel_model->VTH = vth;
                            stmmodel_model->VDD = vdd;
                            stmmodel_model->VT = vt;
                            stmmodel_model->VF = vf;
                            stmmodel_model->TTYPE = tran;
                            stmmodel_model->UTYPE = STM_MOD_MODIV;
                        }
                        if (STM_PRELOAD=='N') {
                            if (STM_CACHE != NULL) {
                                stmmodel_cell = stm_getcell (STM_CNAME);
                                stmmodel_model = (timing_model*)gethtitem (stmmodel_cell->MODEL_HT, namealloc ($3));
                            }
                            stmmodel_model->UMODEL.IV = stm_modiv_alloc( stmmodel_iv_nve, stmmodel_iv_nvs );
                        
                            for( stmmodel_iv_e = stmmodel_iv_nve-1, stmmodel_iv_scane = stmmodel_iv_tabve ; stmmodel_iv_e >= 0 ; stmmodel_iv_e--, stmmodel_iv_scane = stmmodel_iv_scane->NEXT )
                              stmmodel_model->UMODEL.IV->VE[stmmodel_iv_e] = *((float*)(stmmodel_iv_scane->DATA));
                              
                            for( stmmodel_iv_s = stmmodel_iv_nvs-1, stmmodel_iv_scans = stmmodel_iv_tabvs ; stmmodel_iv_s >= 0 ; stmmodel_iv_s--, stmmodel_iv_scans = stmmodel_iv_scans->NEXT )
                              stmmodel_model->UMODEL.IV->VS[stmmodel_iv_s] = *((float*)(stmmodel_iv_scans->DATA));
    
                            for( stmmodel_iv_e = stmmodel_iv_nve-1, stmmodel_iv_scane = stmmodel_iv_tabiv ; stmmodel_iv_e >= 0 ; stmmodel_iv_e--, stmmodel_iv_scane = stmmodel_iv_scane->NEXT ) {
                              for( stmmodel_iv_s = stmmodel_iv_nvs-1, stmmodel_iv_scans = (chain_list*)(stmmodel_iv_scane->DATA) ; stmmodel_iv_s >= 0 ; stmmodel_iv_s-- , stmmodel_iv_scans = stmmodel_iv_scans->NEXT ) {
                                stm_modiv_setis( stmmodel_model->UMODEL.IV, stmmodel_iv_nve, stmmodel_iv_nvs, *((float*)(stmmodel_iv_scans->DATA)));
                              }
                            }

                            stm_modiv_set_cf( stmmodel_model->UMODEL.IV, stmmodel_iv_conf[3], stmmodel_iv_conf[0], stmmodel_iv_conf[1], stmmodel_iv_conf[2] );
                            stm_modiv_set_in( stmmodel_model->UMODEL.IV, stmmodel_iv_input[0], stmmodel_iv_input[1], stmmodel_iv_input[2], stmmodel_iv_input[3] );
                            stm_modiv_set_ti( stmmodel_model->UMODEL.IV, stmmodel_iv_init[0] );
    
                            for( stmmodel_iv_scane = stmmodel_iv_tabve ; stmmodel_iv_scane ; stmmodel_iv_scane = stmmodel_iv_scane->NEXT )
                              mbkfree((float*)(stmmodel_iv_scane->DATA));
                            freechain( stmmodel_iv_tabve );
    
                            for( stmmodel_iv_scans = stmmodel_iv_tabvs ; stmmodel_iv_scans ; stmmodel_iv_scans = stmmodel_iv_scans->NEXT )
                              mbkfree((float*)(stmmodel_iv_scans->DATA));
                            freechain( stmmodel_iv_tabvs );
    
                            for( stmmodel_iv_scane = stmmodel_iv_tabiv ; stmmodel_iv_scane ; stmmodel_iv_scane = stmmodel_iv_scane->NEXT ) {
                              for( stmmodel_iv_scans = (chain_list*)(stmmodel_iv_scane->DATA) ; stmmodel_iv_scans ; stmmodel_iv_scans = stmmodel_iv_scans->NEXT )
                                mbkfree((float*)(stmmodel_iv_scans->DATA));
                              freechain( (chain_list*)(stmmodel_iv_scane->DATA) );
                            }
                            freechain( stmmodel_iv_tabiv );
    
                            switch( vnoise_model ) {
                                case STM_NOISE_SCR:
                                    stm_noise_create( stmmodel_model, STM_NOISE_SCR );
                                    scr_model = stm_noise_getmodel_scr( stmmodel_model );
                                    stm_noise_scr_update( scr_model, vnoise_scr, vnoise_invth );
                                    break;
                            } 
                            switch (stmmodel_energy_model_type) {
                                case STM_ENERGY_PARAMS :    
                                    stmmodel_e_p = stmmodel_e ? stmmodel_e_params : NULL;
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_PARAMS;
                                    stmmodel_model->ENERGYMODEL.EPARAMS = stm_energy_params_create (stmmodel_e_p);
                                    break;
                                case STM_ENERGY_TABLE:
                                    stmmodel_model->ENERGYTYPE = STM_ENERGY_TABLE;
                                    for (stmmodel_energy_nx=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_nx++;
                                    for (stmmodel_energy_ny=0,stmmodel_ch=stmmodel_energy_xchain ; stmmodel_ch ; stmmodel_ch=stmmodel_ch->NEXT) stmmodel_energy_ny++;
                                    stm_modtbl_setXrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xchain, 1);
                                    stm_modtbl_setYrange (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_ychain, 1);
                                    switch (stmmodel_energy_dimension) { 
                                        case STM_CONST:
                                            if (stmmodel_energy_nx > 0 || stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_setconst (stmmodel_model->ENERGYMODEL.ETABLE, *(float*)stmmodel_energy_xydata->DATA);
                                                mbkfree((float*)stmmodel_energy_xydata->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_1D:
                                            if (stmmodel_energy_nx > 0 && stmmodel_energy_ny > 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set1Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)
                                                    mbkfree((float*)stmmodel_ch->DATA);
                                                freechain (stmmodel_energy_xydata);
                                            }
                                            break;
                                        case STM_2D:
                                            if (stmmodel_energy_nx == 0 || stmmodel_energy_ny == 0)
                                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                                            else {
                                                stm_modtbl_set2Dset (stmmodel_model->ENERGYMODEL.ETABLE, stmmodel_energy_xydata, 1);
                                                for (stmmodel_ch = stmmodel_energy_xydata; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT) {
                                                    for (stmmodel_chp = (chain_list*)stmmodel_ch->DATA; stmmodel_chp; stmmodel_chp = stmmodel_chp->NEXT)
                                                        mbkfree((float*)stmmodel_chp->DATA);
                                                    freechain ((chain_list*)stmmodel_ch->DATA);
                                                }
                                                freechain (stmmodel_energy_xydata);
                                            }   
                                            break;
                                    }
                                    for (stmmodel_ch = stmmodel_energy_xchain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    for (stmmodel_ch = stmmodel_energy_ychain; stmmodel_ch; stmmodel_ch = stmmodel_ch->NEXT)  
                                        mbkfree((float*)stmmodel_ch->DATA);
                                    freechain (stmmodel_energy_xchain);
                                    freechain (stmmodel_energy_ychain);
                                    break;
                            }                            
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
                if ((STM_CACHE==NULL)||(STM_PRELOAD == 'Y')) {
                    vth = $3.t_float;
                    vdd = $7.t_float;
                    vt = $11.t_float;
                    vf = $15.t_float;
                }
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
              stmmodel_fct_file = sensitive_namealloc($3); mbkfree($3);
            }
            | empty
            {
                stmmodel_fct_file = NULL;
            }
            ;

localname   : _LOCALNAME '(' _IDENTIFIER ')'
            {
              stmmodel_fct_localname = namealloc($3); mbkfree($3);
            }
            | empty
            {
                stmmodel_fct_localname = NULL;
            }
            ;

instance    : _INSTANCE '(' _IDENTIFIER ')'
            {
              stmmodel_fct_ins = namealloc($3); mbkfree($3);
            }
            | empty
            {
                stmmodel_fct_ins = NULL;
            }
            ;

function    : _FUNCTION '(' _IDENTIFIER param_list ')'
            {
                stmmodel_fct_fct = (APICallFunc *)mbkalloc(sizeof(APICallFunc));
                stmmodel_fct_fct->NAME=sensitive_namealloc($3); mbkfree($3);
                stmmodel_fct_fct->ARGS=$4;
            }
            | empty
            {
                stmmodel_fct_fct = NULL;
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
                stmmodel_xchain    = $3.CHAIN;
                stmmodel_xtype     = $3.TYPE;
                stmmodel_ychain    = $4.CHAIN;
                stmmodel_ytype     = $4.TYPE;
                stmmodel_xydata    = $5.CHAIN;
                stmmodel_dimension = $5.DIM;
                $$ = $1;
            } 
            | tbltype '(' axis data ')'
            { 
                stmmodel_xchain    = $3.CHAIN;
                stmmodel_xtype     = $3.TYPE;
                stmmodel_ychain    = NULL;
                stmmodel_ytype     = STM_NOTYPE;
                stmmodel_xydata    = $4.CHAIN;
                stmmodel_dimension = $4.DIM;
                $$ = $1;
            }
            | tbltype '(' data ')'
            { 
                stmmodel_xchain    = NULL;
                stmmodel_xtype     = STM_NOTYPE;
                stmmodel_ychain    = NULL;
                stmmodel_ytype     = STM_NOTYPE;
                stmmodel_xydata    = $3.CHAIN;
                stmmodel_dimension = $3.DIM;
                $$ = $1;
            } 
            ;

axis        : axistype '(' val val val_list ')' 
            {  
                if (STM_PRELOAD=='Y') {
                    $$.CHAIN = NULL;
                    $$.TYPE = STM_NOTYPE;
                }
                else {
                    stmmodel_ptfl = (float*)mbkalloc (sizeof (float));
                    *stmmodel_ptfl = $4.t_float;
                    $$.CHAIN = addchain ($5, stmmodel_ptfl);
                    stmmodel_ptfl = (float*)mbkalloc (sizeof (float));
                    *stmmodel_ptfl = $3.t_float;
                    $$.CHAIN = addchain ($$.CHAIN, stmmodel_ptfl);
                    $$.TYPE = $1;
                }    
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
                if (STM_PRELOAD=='Y')
                    $$ = NULL;
                else {
                    stmmodel_ptfl = (float*)mbkalloc (sizeof (float));
                    *stmmodel_ptfl = $1.t_float;
                    $$ = addchain ($2, stmmodel_ptfl);
                }
            } 
            ;

data        : _DATA '(' modeldata ')'       
            { 
                $$ = $3;
            } 
            ;
   
modeldata   : val     
            { 
                if (STM_PRELOAD == 'Y') {
                   $$.CHAIN = NULL; 
                    $$.DIM = STM_NODIM;  
                }
                else {
                    stmmodel_ptfl = (float*)mbkalloc (sizeof(float));
                    *stmmodel_ptfl = $1.t_float;
                    $$.CHAIN = addchain (NULL, stmmodel_ptfl); 
                    $$.DIM = STM_CONST;
                }
            } 
            | array1D 
            { 
                $$.CHAIN = $1;
                if (STM_PRELOAD=='Y')
                    $$.DIM = STM_NODIM;
                else
                    $$.DIM = STM_1D;
            }
            | array2D 
            { 
                $$.CHAIN = $1;
                if (STM_PRELOAD=='Y')
                    $$.DIM = STM_NODIM;
                else
                    $$.DIM = STM_2D;
            } 
            ;

array2D     : array1D array1D array1D_list 
            { 
                if (STM_PRELOAD=='Y')
                    $$ = NULL;
                else {
                    $$ = addchain ($3, $2); 
                    $$ = addchain ($$, $1); 
                }
            } 
            ;

array1D     : '(' val val val_list ')' 
            { 
                if (STM_PRELOAD=='Y')
                    $$ = NULL;
                else {
                    stmmodel_ptfl = (float*)mbkalloc (sizeof (float));
                    *stmmodel_ptfl = $3.t_float;
                    $$ = addchain ($4, stmmodel_ptfl);
                    stmmodel_ptfl = (float*)mbkalloc (sizeof(float));
                    *stmmodel_ptfl = $2.t_float;
                    $$ = addchain ($$, stmmodel_ptfl);
                }
            } 
            ;

array1D_list: empty 
            { 
                $$ = NULL;
            }
            | array1D array1D_list 
            { 
                if (STM_PRELOAD == 'Y')
                    $$ = NULL;
                else
                    $$ = addchain ($2, $1);
            } 
            ;

/*****************************************************************************/
/* SCM model                                                                 */
/*****************************************************************************/

scmtype     : _SCM_DUAL
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_DUAL;
                stmmodel_d = 0;
            }
            | _SCM_GOOD 
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_GOOD;
                stmmodel_d = 0;
                stmmodel_i = 0;
                stmmodel_0 = 0;
                stmmodel_n = 0;
                
            }
            | _SCM_FALSE
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_FALSE;
                stmmodel_f = 0;
                stmmodel_i = 0;
                stmmodel_0 = 0;
                stmmodel_n = 0;
            }
            | _SCM_PATH
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_PATH;
                stmmodel_p = 0;
                stmmodel_i = 0;
                stmmodel_0 = 0;
                stmmodel_n = 0;
            }
            | _SCM_CST
            {
                $$ = STM_MOD_MODSCM | STM_MODSCM_CST;
                stmmodel_c = 0;
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
                if (STM_PRELOAD=='Y') {
                    stmmodel_c_params[STM_CST_DELAY] = 0;
                    stmmodel_c_params[STM_CST_SLEW]  = 0;
                    stmmodel_c = 0;
                }
                else {
                    stmmodel_c_params[STM_CST_DELAY] = $8.t_float;
                    stmmodel_c_params[STM_CST_SLEW]  = $9.t_float;
                    stmmodel_c = 1;
                }
            }
            ;

/* dual */

dualparams  : _DUAL '(' '(' _PCONF0 _PCONF1 _CAPAI _CAPAI0 _CAPAI1 _CAPAI2 _CAPAI3 _VT0 _VT0C _IRAP _VDDIN _VT _THRESHOLD _IMAX _AN _BN _VDDMAX _RSAT _RLIN _DRC _RBR _CBR _INPUT_THR _RINT _VINT _CHALF _RCONF _KF _KR _QINIT ')'
              '(' val val val val val val val val val val val val val val val val val val val val val val val val val val val val val val ')' ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_d_params[STM_PCONF0]    = 0;
                    stmmodel_d_params[STM_PCONF1]    = 0;
                    stmmodel_d_params[STM_CAPAI]     = 0;
                    stmmodel_d_params[STM_CAPAI0]    = 0;
                    stmmodel_d_params[STM_CAPAI1]    = 0;
                    stmmodel_d_params[STM_CAPAI2]    = 0;
                    stmmodel_d_params[STM_CAPAI3]    = 0;
                    stmmodel_d_params[STM_VT0]       = 0;
                    stmmodel_d_params[STM_VT0C]      = 0;
                    stmmodel_d_params[STM_IRAP]      = 0;
                    stmmodel_d_params[STM_VDDIN]     = 0;
                    stmmodel_d_params[STM_VT]        = 0;
                    stmmodel_d_params[STM_THRESHOLD] = 0;
                    stmmodel_d_params[STM_IMAX]      = 0;
                    stmmodel_d_params[STM_AN]        = 0;
                    stmmodel_d_params[STM_BN]        = 0;
                    stmmodel_d_params[STM_VDDMAX]    = 0;
                    stmmodel_d_params[STM_RSAT]      = 0;
                    stmmodel_d_params[STM_RLIN]      = 0;
                    stmmodel_d_params[STM_DRC]       = 0;
                    stmmodel_d_params[STM_RBR]       = 0;
                    stmmodel_d_params[STM_CBR]       = 0;
                    stmmodel_d_params[STM_INPUT_THR] = 0;
                    stmmodel_d_params[STM_RINT]      = 0;
                    stmmodel_d_params[STM_VINT]      = 0;
                    stmmodel_d_params[STM_CHALF]     = 0;
                    stmmodel_d_params[STM_RCONF]     = 0;
                    stmmodel_d_params[STM_KF]        = 0;
                    stmmodel_d_params[STM_KRT]       = 0;
                    stmmodel_d_params[STM_QINIT]     = 0;
                    stmmodel_d_params[STM_CAPAO]     = stmmodel_d_params[STM_CAPAI];
                    stmmodel_d = 0;
                }
                else {
                    stmmodel_d_params[STM_PCONF0]    = $36.t_float;
                    stmmodel_d_params[STM_PCONF1]    = $37.t_float;
                    stmmodel_d_params[STM_CAPAI]     = $38.t_float;
                    stmmodel_d_params[STM_CAPAI0]    = $39.t_float;
                    stmmodel_d_params[STM_CAPAI1]    = $40.t_float;
                    stmmodel_d_params[STM_CAPAI2]    = $41.t_float;
                    stmmodel_d_params[STM_CAPAI3]    = $42.t_float;
                    stmmodel_d_params[STM_VT0]       = $43.t_float;
                    stmmodel_d_params[STM_VT0C]      = $44.t_float;
                    stmmodel_d_params[STM_IRAP]      = $45.t_float;
                    stmmodel_d_params[STM_VDDIN]     = $46.t_float;
                    stmmodel_d_params[STM_VT]        = $47.t_float;
                    stmmodel_d_params[STM_THRESHOLD] = $48.t_float;
                    stmmodel_d_params[STM_IMAX]      = $49.t_float;
                    stmmodel_d_params[STM_AN]        = $50.t_float;
                    stmmodel_d_params[STM_BN]        = $51.t_float;
                    stmmodel_d_params[STM_VDDMAX]    = $52.t_float;
                    stmmodel_d_params[STM_RSAT]      = $53.t_float;
                    stmmodel_d_params[STM_RLIN]      = $54.t_float;
                    stmmodel_d_params[STM_DRC]       = $55.t_float;
                    stmmodel_d_params[STM_RBR]       = $56.t_float;
                    stmmodel_d_params[STM_CBR]       = $57.t_float;
                    stmmodel_d_params[STM_INPUT_THR] = $58.t_float;
                    stmmodel_d_params[STM_RINT]      = $59.t_float;
                    stmmodel_d_params[STM_VINT]      = $60.t_float;
                    stmmodel_d_params[STM_CHALF]     = $61.t_float;
                    stmmodel_d_params[STM_RCONF]     = $62.t_float;
                    stmmodel_d_params[STM_KF]        = $63.t_float;
                    stmmodel_d_params[STM_KRT]       = $64.t_float;
                    stmmodel_d_params[STM_QINIT]     = $65.t_float;
                    stmmodel_d_params[STM_CAPAO]     = stmmodel_d_params[STM_CAPAI];
                    stmmodel_d = 1;
                }
            }
            | _DUAL '(' '(' _PCONF0 _PCONF1 _CAPAI _IRAP _VDDIN _VT _THRESHOLD _IMAX _AN _BN _VDDMAX _RSAT _RLIN _DRC _RBR _CBR _INPUT_THR _RINT _VINT _CHALF _RCONF _KF _KR _QINIT ')'
              '(' val val val val val val val val val val val val val val val val val val val val val val val val ')' ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_d_params[STM_PCONF0]    = 0;
                    stmmodel_d_params[STM_PCONF1]    = 0;
                    stmmodel_d_params[STM_CAPAI]     = 0;
                    stmmodel_d_params[STM_CAPAI0]    = 0;
                    stmmodel_d_params[STM_CAPAI1]    = 0;
                    stmmodel_d_params[STM_CAPAI2]    = 0;
                    stmmodel_d_params[STM_CAPAI3]    = 0;
                    stmmodel_d_params[STM_VT0]       = 0;
                    stmmodel_d_params[STM_VT0C]      = 0;
                    stmmodel_d_params[STM_IRAP]      = 0;
                    stmmodel_d_params[STM_VDDIN]     = 0;
                    stmmodel_d_params[STM_VT]        = 0;
                    stmmodel_d_params[STM_THRESHOLD] = 0;
                    stmmodel_d_params[STM_IMAX]      = 0;
                    stmmodel_d_params[STM_AN]        = 0;
                    stmmodel_d_params[STM_BN]        = 0;
                    stmmodel_d_params[STM_VDDMAX]    = 0;
                    stmmodel_d_params[STM_RSAT]      = 0;
                    stmmodel_d_params[STM_RLIN]      = 0;
                    stmmodel_d_params[STM_DRC]       = 0;
                    stmmodel_d_params[STM_RBR]       = 0;
                    stmmodel_d_params[STM_CBR]       = 0;
                    stmmodel_d_params[STM_INPUT_THR] = 0;
                    stmmodel_d_params[STM_RINT]      = 0;
                    stmmodel_d_params[STM_VINT]      = 0;
                    stmmodel_d_params[STM_CHALF]     = 0;
                    stmmodel_d_params[STM_RCONF]     = 0;
                    stmmodel_d_params[STM_KF]        = 0;
                    stmmodel_d_params[STM_KRT]       = 0;
                    stmmodel_d_params[STM_QINIT]     = 0;
                    stmmodel_d_params[STM_CAPAO]     = stmmodel_d_params[STM_CAPAI];
                    stmmodel_d = 0;
                }
                else {
                    stmmodel_d_params[STM_PCONF0]    = $30.t_float;
                    stmmodel_d_params[STM_PCONF1]    = $31.t_float;
                    stmmodel_d_params[STM_CAPAI]     = $32.t_float;
                    stmmodel_d_params[STM_CAPAI0]    = $32.t_float;
                    stmmodel_d_params[STM_CAPAI1]    = $32.t_float;
                    stmmodel_d_params[STM_CAPAI2]    = $32.t_float;
                    stmmodel_d_params[STM_CAPAI3]    = $32.t_float;
                    stmmodel_d_params[STM_VT0]       = $35.t_float;
                    stmmodel_d_params[STM_VT0C]      = $40.t_float-$35.t_float;
                    stmmodel_d_params[STM_IRAP]      = $33.t_float;
                    stmmodel_d_params[STM_VDDIN]     = $34.t_float;
                    stmmodel_d_params[STM_VT]        = $35.t_float;
                    stmmodel_d_params[STM_THRESHOLD] = $36.t_float;
                    stmmodel_d_params[STM_IMAX]      = $37.t_float;
                    stmmodel_d_params[STM_AN]        = $38.t_float;
                    stmmodel_d_params[STM_BN]        = $39.t_float;
                    stmmodel_d_params[STM_VDDMAX]    = $40.t_float;
                    stmmodel_d_params[STM_RSAT]      = $41.t_float;
                    stmmodel_d_params[STM_RLIN]      = $42.t_float;
                    stmmodel_d_params[STM_DRC]       = $43.t_float;
                    stmmodel_d_params[STM_RBR]       = $44.t_float;
                    stmmodel_d_params[STM_CBR]       = $45.t_float;
                    stmmodel_d_params[STM_INPUT_THR] = $46.t_float;
                    stmmodel_d_params[STM_RINT]      = $47.t_float;
                    stmmodel_d_params[STM_VINT]      = $48.t_float;
                    stmmodel_d_params[STM_CHALF]     = $49.t_float;
                    stmmodel_d_params[STM_RCONF]     = $50.t_float;
                    stmmodel_d_params[STM_KF]        = $51.t_float;
                    stmmodel_d_params[STM_KRT]       = $52.t_float;
                    stmmodel_d_params[STM_QINIT]     = $53.t_float;
                    stmmodel_d_params[STM_CAPAO]     = stmmodel_d_params[STM_CAPAI];
                    stmmodel_d = 1;
                }
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
                if (STM_PRELOAD=='Y') {
                    stmmodel_i_params[STM_NB_I_LINKS] = 0;
                    stmmodel_index = 0;
                    stmmodel_i = 0;
                }
                else {
                    stmmodel_i_params[STM_NB_I_LINKS] = $2.t_float;
                    stmmodel_index = 1;
                    stmmodel_i = 1;
                }
            }
            ;

link0p      : _LINK_OUT '(' '(' _CI _CF _K3 _K4 _K5 ')' '(' val val val val val ')' ')'
            {
               if (STM_PRELOAD=='Y') {
                    stmmodel_0_params[STM_CI_0] = 0;
                    stmmodel_0_params[STM_CF_0] = 0;
                    stmmodel_0_params[STM_K3_0] = 0;
                    stmmodel_0_params[STM_K4_0] = 0;
                    stmmodel_0_params[STM_K5_0] = 0;
                    stmmodel_0 = 0;
                }
                else {
                    stmmodel_0_params[STM_CI_0] = $11.t_float;
                    stmmodel_0_params[STM_CF_0] = $12.t_float;
                    stmmodel_0_params[STM_K3_0] = $13.t_float;
                    stmmodel_0_params[STM_K4_0] = $14.t_float;
                    stmmodel_0_params[STM_K5_0] = $15.t_float;
                    stmmodel_0 = 1;
                }    
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
                if (STM_PRELOAD=='Y'){
                    stmmodel_i_params[stmmodel_index + STM_CI_I]   = 0;
                    stmmodel_i_params[stmmodel_index + STM_CF_I]   = 0;
                    stmmodel_i_params[stmmodel_index + STM_ACTI_I] = 0;
                    stmmodel_i_params[stmmodel_index + STM_BCTI_I] = 0;
                    stmmodel_i_params[stmmodel_index + STM_K3_I]   = 0;
                    stmmodel_i_params[stmmodel_index + STM_K4_I]   = 0;
                    stmmodel_i_params[stmmodel_index + STM_K5_I]   = 0;
                    stmmodel_index = 0;
                }
                else {
                    stmmodel_i_params[stmmodel_index + STM_CI_I]   = $2.t_float;
                    stmmodel_i_params[stmmodel_index + STM_CF_I]   = $3.t_float;
                    stmmodel_i_params[stmmodel_index + STM_ACTI_I] = $4.t_float;
                    stmmodel_i_params[stmmodel_index + STM_BCTI_I] = $5.t_float;
                    stmmodel_i_params[stmmodel_index + STM_K3_I]   = $6.t_float;
                    stmmodel_i_params[stmmodel_index + STM_K4_I]   = $7.t_float;
                    stmmodel_i_params[stmmodel_index + STM_K5_I]   = $8.t_float;
                    stmmodel_index += STM_NB_LINK_I_PARAMS;
                }
            }
            ;
            
linknp      : _LINK_DUAL '(' '(' _CI _CF _ACTI _BCTI ')' '(' val val val val ')' ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_n_params[STM_CI_N]   = 0;
                    stmmodel_n_params[STM_CF_N]   = 0;
                    stmmodel_n_params[STM_ACTI_N] = 0;
                    stmmodel_n_params[STM_BCTI_N] = 0;
                    stmmodel_n = 0;
                }
                else {
                    stmmodel_n_params[STM_CI_N]   = $10.t_float;
                    stmmodel_n_params[STM_CF_N]   = $11.t_float;
                    stmmodel_n_params[STM_ACTI_N] = $12.t_float;
                    stmmodel_n_params[STM_BCTI_N] = $13.t_float;
                    stmmodel_n = 1;
                }
            }
            | empty
            ;

/* falselink */

falselink   : _FALSE '(' '(' _PCONF0 _PCONF1 _RTOT _KF _VDDMAX _DRC ')' '(' val val val val val val ')' ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_f_params[STM_PCONF0_F] = 0;
                    stmmodel_f_params[STM_PCONF1_F] = 0;
                    stmmodel_f_params[STM_RTOT_F]   = 0;
                    stmmodel_f_params[STM_K_F]      = 0;
                    stmmodel_f_params[STM_VDDMAX_F] = 0;
                    stmmodel_f_params[STM_DRC_F]    = 0;
                    stmmodel_f = 0;
                }
                else {
                    stmmodel_f_params[STM_PCONF0_F] = $12.t_float;
                    stmmodel_f_params[STM_PCONF1_F] = $13.t_float;
                    stmmodel_f_params[STM_RTOT_F]   = $14.t_float;
                    stmmodel_f_params[STM_K_F]      = $15.t_float;
                    stmmodel_f_params[STM_VDDMAX_F] = $16.t_float;
                    stmmodel_f_params[STM_DRC_F]    = $17.t_float;
                    stmmodel_f = 1;
                }
            }
            | _FALSE '(' '(' _PCONF0 _PCONF1 _RTOT _KF _VDDMAX _DRC _VF_INPUT ')' '(' val val val val val val val ')' ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_f_params[STM_PCONF0_F]   = 0;
                    stmmodel_f_params[STM_PCONF1_F]   = 0;
                    stmmodel_f_params[STM_RTOT_F]     = 0;
                    stmmodel_f_params[STM_K_F]        = 0;
                    stmmodel_f_params[STM_VDDMAX_F]   = 0;
                    stmmodel_f_params[STM_DRC_F]      = 0;
                    stmmodel_f_params[STM_VF_INPUT_F] = 0;
                    stmmodel_f = 0;
                }
                else {
                    stmmodel_f_params[STM_PCONF0_F]   = $13.t_float;
                    stmmodel_f_params[STM_PCONF1_F]   = $14.t_float;
                    stmmodel_f_params[STM_RTOT_F]     = $15.t_float;
                    stmmodel_f_params[STM_K_F]        = $16.t_float;
                    stmmodel_f_params[STM_VDDMAX_F]   = $17.t_float;
                    stmmodel_f_params[STM_DRC_F]      = $18.t_float;
                    stmmodel_f_params[STM_VF_INPUT_F] = $19.t_float;
                    stmmodel_f = 1;
                }
            }
            ;

/* pathlink */

pathlink    : _PATH '(' '(' _PCONF0 _VDDMAX ')' '(' val val ')' ')'
            {
                if (STM_PRELOAD == 'Y') {
                    stmmodel_p_params[STM_PCONF0_F] = 0;
                    stmmodel_p_params[STM_VDDMAX_P] = 0;
                    stmmodel_p = 0;
                }
                else {
                    stmmodel_p_params[STM_PCONF0_F] = $8.t_float;
                    stmmodel_p_params[STM_VDDMAX_P] = $9.t_float;
                    stmmodel_p = 1;
                }
            }
            | _PATH '(' '(' _PCONF0 _VDDMAX _VF_INPUT ')' '(' val val val ')' ')'
            {
                if (STM_PRELOAD == 'Y') {
                    stmmodel_p_params[STM_PCONF0_F]   = 0;
                    stmmodel_p_params[STM_VDDMAX_P]   = 0;
                    stmmodel_p = 0;
                }
                else {
                    stmmodel_p_params[STM_PCONF0_F]   = $9.t_float;
                    stmmodel_p_params[STM_VDDMAX_P]   = $10.t_float;
                    stmmodel_p = 1;
                }
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
                if (STM_PRELOAD=='Y')
                    stmmodel_iv_nve = 0;
                else
                    stmmodel_iv_nve = atoi( $3.t_float_string );
            }
            ;

nbvs        : _NVS '(' val ')'
            {
                if (STM_PRELOAD=='Y')
                    stmmodel_iv_nvs = 0;
                else
                    stmmodel_iv_nvs = atoi( $3.t_float_string );
            }
            ;

voltageve   : _VE array1D
            {
              stmmodel_iv_tabve = $2;
            }
            ;

voltagevs   : _VS array1D
            {
              stmmodel_iv_tabvs = $2;
            }
            ;

current     : _CURRENT '(' array2D ')'
            {
              stmmodel_iv_tabiv = $3;
            }
            ;

conflict    : _CONFLICT '(' val val val val ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_iv_conf[0]=0;
                    stmmodel_iv_conf[1]=0;
                    stmmodel_iv_conf[2]=0;
                    stmmodel_iv_conf[3]=0;
                }
                else {
                    stmmodel_iv_conf[0]=$3.t_float;
                    stmmodel_iv_conf[1]=$4.t_float;
                    stmmodel_iv_conf[2]=$5.t_float;
                    stmmodel_iv_conf[3]=$6.t_float;
                }
            }
            ;

input       : _INPUT '(' val val val val ')'
            {
                if (STM_PRELOAD=='Y') {
                    stmmodel_iv_input[0]=0;
                    stmmodel_iv_input[1]=0;
                    stmmodel_iv_input[2]=0;
                    stmmodel_iv_input[3]=0;
                }
                else {
                    stmmodel_iv_input[0]=$3.t_float;
                    stmmodel_iv_input[1]=$4.t_float;
                    stmmodel_iv_input[2]=$5.t_float;
                    stmmodel_iv_input[3]=$6.t_float;
                }
            }
            ;

initial     : _INITIAL '(' val ')'
            {
                if (STM_PRELOAD=='Y') 
                  stmmodel_iv_init[0]=0;
                else
                  stmmodel_iv_init[0]=$3.t_float;
            }
            ;

/*****************************************************************************/
/* PLN model                                                                 */
/*****************************************************************************/

plnmodel    : _SPDM '(' varparams_list ordersparams coefsparams ')'
            {
              if (STM_PRELOAD=='Y') {
                  stmmodel_nb_var = 0;
                  stmmodel_deg_index = 0;
                  stmmodel_var_index = 0;
                  stmmodel_coeftab_index = 0;
                  stmmodel_coefdef_index = 0;
                  stmmodel_coefdef_nb = 0;
                  stmmodel_coefdef_mask = STM_COEF_DEFMASK;
              }
              else {
                    stmmodel_nb_var = stmmodel_deg_index;
                    stmmodel_deg_index = 0;
                    stmmodel_var_index = 0;
                    stmmodel_coeftab_index = 0;
                    stmmodel_coefdef_index = 0;
                    stmmodel_coefdef_nb = 0;
                    stmmodel_coefdef_mask = STM_COEF_DEFMASK;
                }
                $$ = STM_MOD_MODPLN;
            }
            ;

/* varparams */

varparams_list  : empty
                | varparams_list varparams
                ;

varparams    : axistype _VARTHMIN '(' val ')' _VARTHMAX '(' val ')'
             {
              if (STM_PRELOAD=='Y') {
                  stmmodel_var_params[stmmodel_var_index] = NULL;
                  stmmodel_var_params[stmmodel_var_index]->TYPE = $1;
                  stmmodel_var_params[stmmodel_var_index]->THMIN = 0;
                  stmmodel_var_params[stmmodel_var_index]->THMAX = 0;
                  stmmodel_var_index=0;
              }
              else {
                    stmmodel_var_params[stmmodel_var_index] = (timing_varpolynom*)mbkalloc (sizeof (struct timing_varpolynom));
                    stmmodel_var_params[stmmodel_var_index]->TYPE = $1;
                    stmmodel_var_params[stmmodel_var_index]->THMIN = $4.t_float;
                    stmmodel_var_params[stmmodel_var_index]->THMAX = $8.t_float;
                    stmmodel_var_index++;
                }
             }
             ;

             
/* orders */

orders_list  : orders_list orders_i
             | orders_i
             ;

orders_i     : val 
             {
              if (STM_PRELOAD=='Y') {
                  stmmodel_deg_params[stmmodel_deg_index] = 0;
                  stmmodel_deg_index=0;
              }
              else {
                    stmmodel_deg_params[stmmodel_deg_index] = $1.t_float;
                    stmmodel_deg_index++;
                }
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
              if (STM_PRELOAD=='Y') {
                  stmmodel_coefdef_nb = 0;
                  stmmodel_coeftab_params[stmmodel_coeftab_index] = 0;
                  stmmodel_coeftab_index = 0;
                  stmmodel_coefdef_params[stmmodel_coefdef_index] = 0;
                  stmmodel_coefdef_mask = 0;
                  stmmodel_coefdef_index = 0;
              }
              else {
                    stmmodel_coefdef_nb++;
                    if($1.t_float){
                        stmmodel_coeftab_params[stmmodel_coeftab_index] = $1.t_float;
                        stmmodel_coeftab_index ++;
                        stmmodel_coefdef_params[stmmodel_coefdef_index] = stmmodel_coefdef_params[stmmodel_coefdef_index] | stmmodel_coefdef_mask;
                    }
                    stmmodel_coefdef_mask = stmmodel_coefdef_mask << 1;
                    if(!((stmmodel_coefdef_nb) % (sizeof(long) * 8))){
                        stmmodel_coefdef_index++;
                    stmmodel_coefdef_mask = STM_COEF_DEFMASK;
                    }
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
                if (STM_PRELOAD=='Y')
                   vnoise_model = STM_NOISE_NULL;
                else {
                    vnoise_model = STM_NOISE_SCR;
                    vnoise_scr = $3.t_float;
                    vnoise_invth = $4.t_float;
                }
            }
            | _NOISE_SCR '(' val ')'
            {
                if (STM_PRELOAD=='Y')
                   vnoise_model = STM_NOISE_NULL;
                else {
                    vnoise_model = STM_NOISE_SCR;
                    vnoise_scr = $3.t_float;
                    vnoise_invth = vdd/2;
                }
            }
            |  empty
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
            stmmodel_e_params [STM_EPCONF0] = $6.t_float;
            stmmodel_e_params [STM_EPCONF1] = $7.t_float;
            stmmodel_e = 1;
        }
        ;

energyparamsmodel   : _ENERGY_MODEL 
                    {
                        stmmodel_e = 0;
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
                    stmmodel_energy_xchain    = $3.CHAIN;
                    stmmodel_energy_xtype     = $3.TYPE;
                    stmmodel_energy_ychain    = $4.CHAIN;
                    stmmodel_energy_ytype     = $4.TYPE;
                    stmmodel_energy_xydata    = $5.CHAIN;
                    stmmodel_energy_dimension = $5.DIM;
                    $$ = $1;
                }
                | energytbltype '(' axis data ')'
                {
                    stmmodel_energy_xchain    = $3.CHAIN;
                    stmmodel_energy_xtype     = $3.TYPE;
                    stmmodel_energy_ychain    = NULL;
                    stmmodel_energy_ytype     = STM_NOTYPE;
                    stmmodel_energy_xydata    = $4.CHAIN;
                    stmmodel_energy_dimension = $4.DIM;
                    $$ = $1;
                }
                | energytbltype '(' data ')'
                {
                    stmmodel_energy_xchain    = NULL;
                    stmmodel_energy_xtype     = STM_NOTYPE;
                    stmmodel_energy_ychain    = NULL;
                    stmmodel_energy_ytype     = STM_NOTYPE;
                    stmmodel_energy_xydata    = $3.CHAIN;
                    stmmodel_energy_dimension = $3.DIM;
                    $$ = $1;
                }
                ;

energymodel : energytblmodel
            {
                stmmodel_energy_model_type = $1;
            }
            | energyparamsmodel
            {
                stmmodel_energy_model_type = $1;
            }
            | empty
            {
                stmmodel_energy_model_type = STM_ENERGY_NULL;
            }
            ;

/*****************************************************************************/


modelname   : _NAME '(' _IDENTIFIER ')'
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
    return 1;
}

