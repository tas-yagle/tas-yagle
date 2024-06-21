/*---------------------------------------------------------------------
Do not change this data manually,
it will be automatically updated by RCS

$RCSfile: sk.h,v $

----------------------------------------------------------------------*/
/**
 * Copyright (c) 2002 Nederlandse NXP Semiconductors
 * NXP Research Laboratories
 * Eindhoven, The Netherlands
 *
 * File       : sk.h
 * Author     : Andreas Gessner
 * Co-Author  : Bart Kost, Rob Klaassen
 * Date       : 05.11.2002
 * Version    : 0.2
 *
 * History    : <date>         <name>       <changes>
 *              30-01-02  Andreas Gessner   Updated according to the type names
 *                                          stated in the SDD V0.7
 *              13-02-02  Andreas Gessner   Updated according to the SDD V0.8
 *              05-03-02  Andreas Gessner   Added: err_parameter_not_readable,
 *                                          err_parameter_not_writeable
 *                                          reset_inst_params()
 *                                          -> p_model_data added as argument
 *                                          (needed to select default values,
 *                                          depend on type).
 *              13-03-02  Andreas Gessner   Nodal voltages added to
 *                                          instance variables structure.
 *                                          Initial values added, controlling
 *                                          nodes.
 *
 *              20-03-02  Andreas Gessner   Branch seperated -> branch type
 *                                          not needed anymore, branch_voltages
 *                                          deleted, branch charges added,
 *                                          gmin_status included
 *                                          in branch_status     .
 *
 *              05-11-02  Bart Kost         Changed interface to support
 *                                          different clipping values for N/P,
 *                                          removed pp_op_info (duplicate),
 *                                          renamed from PMK_ to SK_.
 *
 * @file
 * This file declares the data structures of the SiMKit
 */

#ifndef _SK_H
#define _SK_H

#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <ctype.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include "stdarg.h"         /* Needed for debugging */

/**
 * The SiMKit model interface version number.
 * Any time the interface changes this value has to be increased.
 * The SiMKit adapters for Pstar, spectre and ADS use this define also
 * to check whether their code supports the interface version the compiled
 * model library reports.
 *
 * Version 1: February 2007 The first numbered version of the SK interface
 * Version 2: February 2007 Added parameter 'p_is_param_changed' to
 *            set_model_params and to set_inst_params (ER18977)
 *            Added p_ev_ig_available array to SK_MODEL (ER19057)
 */
#define SK_INTERFACE_VERSION            2

/* SiMKit types */
typedef char                            sk_boolean;
typedef int                             sk_integer;
typedef double                          sk_real;
typedef short                           sk_short_integer;
typedef float                           sk_short_real;

/********************************************************************
 *                         M A C R O S                              *
 ********************************************************************/

/* matrix access */
#define SK_BRANCH_CURRENT( branch_nr) \
    p_i_data->inst_variables.p_output_ev[branch_nr].value
#define SK_CONDUCTANCE( branch_nr, ctrl_ev_nr) \
    p_i_data->inst_variables.p_output_ev[branch_nr].p_derivatives[ctrl_ev_nr]
#define SK_BRANCH_CHARGE( branch_nr) \
    p_i_data->inst_variables.p_output_ev[branch_nr].value
#define SK_CAPACITANCE( branch_nr, ctrl_ev_nr) \
    p_i_data->inst_variables.p_output_ev[branch_nr].p_derivatives[ctrl_ev_nr]

/* Insert noise in the instance variables structure                        */
#define SK_SET_BIAS_DEP_NOISE( source_nr, value ) p_i_data->inst_variables.p_bias_dep_noise_parts[source_nr] = value;
#define SK_SET_FREQ_DEP_NOISE( source_nr, value ) p_i_data->inst_variables.p_freq_dep_noise_parts[source_nr] = value;


/********************************************************************
 *                 D A T A   D E C L A R A T I O N S                *
 ********************************************************************/

typedef unsigned int sk_unint;
typedef unsigned int SK_MASK_TYPE;

typedef SK_MASK_TYPE SK_AFFECTS_TYPE;
#define SK_AT_AFFECTS_NONE              0x0000
#define SK_AT_TEMP_AFFECTS              0x0001
#define SK_AT_AFFECTS_OP_POINT          0x0002
#define SK_AT_OP_POINT_AFFECTS          0x0004

typedef SK_MASK_TYPE SK_INIT_CONTROL;
#define SK_IC_EVAL_COMMON_PARAMS        0x0001
#define SK_IC_GEOM_SCALING              0x0002
#define SK_IC_TEMP_SCALING              0x0004
#define SK_IC_MULT_SCALING              0x0008

typedef SK_MASK_TYPE SK_EVAL_CONTROL;
#define SK_EC_CURRENTS                  0x0001
#define SK_EC_CHARGES                   0x0002
#define SK_EC_CONDUCTANCES              0x0004
#define SK_EC_CAPACITANCES              0x0008
#define SK_EC_OP_INFO                   0x0010
#define SK_EC_BIAS_DEP_NOISE            0x0020
#define SK_EC_FREQ_DEP_NOISE            0x0040
#define SK_EC_H_NOISE                   0x0080
#define SK_EC_U_NOISE                   0x0100
#define SK_EC_H_NOISE_ADS               0x0200

typedef SK_MASK_TYPE SK_CLIP_TYPE;
#define SK_CB_NONE                      0x0000
#define SK_CB_LOWER                     0x0001
#define SK_CB_UPPER                     0x0010
#define SK_CB_BOTH                      0x0011

/**
 * Parameter scaling types to support the Spectre 'scale' parameter.
 * Maxiset instance parameters that have a length in their unit, are scaled.
 * For example, the value of a parameter with unit "m^2" is
 * multiplied by factor^2. This also goes for complex units, e.g. m^2/V.
 */
typedef enum
{
    SK_SF_NONE,          /**< no length unit */
    SK_SF_DIVIDE,        /**< 1/m   */
    SK_SF_POW_MIN_TWO,   /**< 1/m^2 */
    SK_SF_POW_MIN_THREE, /**< 1/m^3 */
    SK_SF_POW_MIN_FOUR,  /**< 1/m^4 */
    SK_SF_MULTIPLY,      /**< m     */
    SK_SF_POW_TWO,       /**< m^2   */
    SK_SF_POW_THREE,     /**< m^3   */
    SK_SF_POW_FOUR       /**< m^4   */
} SK_SCALING_FACTOR;

/* Model family type for several uses:
 * 1) Determine which initial guess to use by default
 * 2) Determine if Vds limiting should be performed (unwanted SD interchange)
 *    (Pstar only)
 */
typedef enum
{
    SK_MODEL_FAMILY_MOST,      /* SD interchange limiting */
    SK_MODEL_FAMILY_BJT,       /* exponent limiting */
    SK_MODEL_FAMILY_GENERIC    /* no special processing */
} SK_MODEL_FAMILY;

/* SK_device_type for internal use (p_data->type) and calling of init_model() */
#define SK_DT_N                         0x0001
#define SK_DT_P                         0x0002
#define SK_DT_NPN                       0x0004
#define SK_DT_PNP                       0x0008
#define SK_DT_NPNV                      0x0010
#define SK_DT_PNPV                      0x0020
#define SK_DT_NPNL                      0x0040
#define SK_DT_PNPL                      0x0080
#define SK_DT_NONE                      0x4000
#define SK_DT_DEFAULT                   0x8000

/* ADS known only N or P, so use these. NEVER use these in the models! */
#define SK_DT_GENERIC_N         (SK_DT_N | SK_DT_NPN | SK_DT_NPNV | SK_DT_NPNL)
#define SK_DT_GENERIC_P         (SK_DT_P | SK_DT_PNP | SK_DT_PNPV | SK_DT_PNPL)

#define SK_NODE_NUMBER_NONE             -1

typedef enum
{
    SK_ND_NONE,
    SK_ND_TERMINAL,
    SK_ND_INTERNAL_NODE
} SK_NODE_TYPE;

typedef enum
{
    SK_ST_ON,
    SK_ST_OFF,
    SK_ST_SHORT
} SK_STATUS_TYPE;

typedef enum
{
    SK_ERR_NONE,
    SK_ERR_PARAMETER_NOT_READABLE,
    SK_ERR_PARAMETER_NOT_WRITEABLE,
    SK_ERR_UNKNOWN_PARAMETER,
    SK_ERR_UNKNOWN_TYPE_STRING,
    SK_ERR_LIMITED
} SK_ERROR;

typedef struct
{
    const int                           number;
    const char                         *p_name;
    const SK_NODE_TYPE                  type;
} SK_NODE;

typedef struct
{
    const sk_unint                         number;
    const char                         *p_name;
    const SK_NODE                      *p_pos_node;
    const SK_NODE                      *p_neg_node;
    const sk_unint                         n_ctrl_ev;
    const sk_unint                        *p_ctrl_ev_index;
} SK_BRANCH;


/* 
 * Noise types
 * SK_NT_THERMAL   : noise is neither function of frequency nor time varying
 * SK_NT_SHOT      : noise is time varying, but not a function of frequency
 * SK_NT_FLICKER   : noise = |h(t)|/frequency
 * SK_NT_UNCORR    : general case where noise = |h(t)|*|U(|h(t)|,f)| 
 * SK_NT_CORR_REAL : general case of real correlated sources 
 * SK_NT_CORR_IMAG : general case of imag correlated sources 
 */ 
typedef enum
{
    SK_NT_THERMAL,
    SK_NT_SHOT,
    SK_NT_FLICKER,
    SK_NT_UNCORR,
    SK_NT_CORR_REAL,
    SK_NT_CORR_IMAG
} SK_NOISE_TYPE;

/* 
 * noise sources:
 * The spectral current density is  given by noise = |h(t)|*|U(|h|,f)|.
 * So for each source there is a bias dep part h(t) and a 
 * freq dep part u(|h|,f). If a u-function also depends on h-functions 
 * of other sources, then p_foreign_h_index is used to store 
 * source number of these noise sources.
 * The p_foreign_h_index is set in the model code. 
 * Currently only mos1100 and mos1101 have noise source sight for which
 * p_foreign_h_index is used.
 * The p_foreign_h_index can be used for all noise types (SK_NOISE_TYPE). 
 * The nodes p_pos/neg_node_corr are used to model correlated noise sources. 
 */
typedef struct
{
    const sk_unint                         number;
    const char                         *p_name;
    const SK_NOISE_TYPE                 type;
    const SK_NODE                      *p_pos_node;
    const SK_NODE                      *p_neg_node;
    const SK_NODE                      *p_pos_node_corr;
    const SK_NODE                      *p_neg_node_corr;
    const sk_unint                         n_foreign_h;
    const sk_unint                        *p_foreign_h_index;
} SK_NOISE_SOURCE;

/* limiting */
typedef enum
{
    SK_LIM_MOS,
    SK_LIM_BJT
} SK_LIM_TYPE;

typedef struct 
{  
    sk_real                               result;
    sk_real                               result_rhs;    
    sk_real                               dresult_dV;        
    sk_real                               dresult_dT;   
    sk_real                               d2result_ddV;
    sk_real                               d2result_dTdV;                   
} SK_EXPLIM_RESULTS;


typedef struct 
{ 
    sk_real                                Imax; 
    sk_real                                Is;
    sk_real                                dIs_dT; 
    sk_real                                One_over_Vt; 
    sk_real                                dOne_over_Vt_dT;  
    /* V_check  */  
    sk_real                                V_check;    
    
} SK_EXPLIM_PARAMS;


typedef struct
{
    SK_LIM_TYPE                         limiting_type;
    sk_real                                Vmax;
    sk_real                                Imax;
    sk_boolean                             inverse_function_applied;
    sk_boolean                             dc_or_disc_timestep;
    sk_unint                               iter_count;

    SK_ERROR (*p_eval_diode) (
        sk_real                        ISt,
        sk_real                       *p_Vd,
        sk_real                        Vd_prev,
        sk_real                        nvt,
        sk_real                       *p_rhs,
        sk_real                       *p_jac,
        sk_real                        v_check,
        sk_real                        min_jac,
        sk_boolean                     spec_approx,
        sk_boolean                     dc_or_disc_timestep,
        sk_integer                     iter_count,
        sk_boolean                    *p_inverse_func_applied,
        sk_real                       *p_value );

    SK_ERROR (*p_eval_exp) (
        sk_real                        x,                  /* in */
        sk_real                        x0,                /* in */
        sk_real                       *p_value,              /* out */
        sk_real                       *p_dvalue   );         /* out */

    SK_ERROR (*p_eval_v_check) (
        sk_real                        Is,                     /* in */
        sk_real                        Imax,                   /* in */
        sk_real                        Vt,                     /* in */
        sk_real                       *p_v_check   );         /* out */

    SK_ERROR (*p_limit_temperature) (
        sk_real                       *p_T,
        sk_real                        T_prev,
        sk_boolean                     spec_approx,
        sk_integer                     iter_count,
        sk_boolean                    *p_inverse_func_applied );
 /* Functions eval_v_check_new, eval_exp, eval_diode with temperature derivatives */
 /* Init values for linearized exp(V/Vt) */
    SK_ERROR (*p_eval_v_check_new) (
        sk_real                       Imax,                /* in */
        sk_real                       Is,                  /* in */
        sk_real                       dIs_dT,              /* in */
        sk_real                       One_over_Vt,         /* in */
        sk_real                       dOne_over_Vt_dT,     /* in */
        SK_EXPLIM_PARAMS           *p_params );         /* out */

 /* Calculates the possibly linearized exp(V/Vt)  */     
     SK_ERROR (*p_eval_explim_new) (
        sk_real                        V,                   /* in */
        SK_EXPLIM_PARAMS         *p_params,              /* in */
        SK_EXPLIM_RESULTS        *p_results);            /* out */       
 
 /* Calculates the possibly linearized Is*(exp(V/Vt) -1)  */     
    SK_ERROR (*p_eval_diode_new) ( 
        sk_real                       *p_Vd,
        sk_real                        Vd_prev,
        sk_real                        min_jac,
        sk_boolean                     spec_approx,
        sk_boolean                     dc_or_disc_timestep,
        sk_integer                     iter_count,
        sk_boolean                    *p_inverse_func_applied,
        SK_EXPLIM_PARAMS           *p_params,
        SK_EXPLIM_RESULTS          *p_results );  
        
} SK_LIMITING;

/* New initial guess */

typedef enum
{
    /* Spectre */
    SK_IG_OFF_SPECTRE,
    SK_IG_TRIODE,
    SK_IG_SATURATION,
    SK_IG_SUBTHRESHOLD,
    SK_IG_REVERSE,
    SK_IG_FORWARD,
    SK_IG_BREAKDOWN,

    /* Pstar */
    SK_IG_DEFAULT,
    SK_IG_ON,
    SK_IG_OFF_PSTAR,

    SK_IG_N_REGION
} SK_INITIAL_GUESS_TYPE;

typedef sk_real                         SK_INITIAL_EV;
typedef unsigned                        SK_REGION;

/* Definition structure for EVs. One per EV. */
typedef struct
{
    const SK_INITIAL_EV                *p_initial_value;
    const SK_NODE                      *p_pos_node;
    const SK_NODE                      *p_neg_node;
} SK_EV_DESCRIP;

/* Value storage for EVs. One per EV. */
typedef struct SK_ev
{
    sk_real                                value;
    sk_real                                previous_value;
    sk_real                                not_truncated_value;
    sk_boolean                             is_truncated;
    sk_boolean                             is_initial_guess;
    const SK_EV_DESCRIP                *p_ev_descrip;
} SK_EV;

/* Value storage for output variables (currents, charges and their derivatives).
   One per branch. */
typedef struct
{
    sk_real                                value;
    sk_real                               *p_derivatives;
    const SK_BRANCH                    *p_ev_descrip;
} SK_OUTPUT_EV;


/* data structure used to transfer the bias from the adapter       */
/* to the Modelkit and to transfer the calculated quantities       */
/* from the Modelkit to the simulator adapters                     */

typedef struct
{
  sk_real                                  Real;
  sk_real                                  Imag;
} SK_COMPLEX;

typedef struct
{
    /* EVs. Number of entries = N_EV                               */
    SK_EV                              *p_ev;

    /* Output values and derivatives. Number of entries = N_TOTAL_BRANCHES */
    SK_OUTPUT_EV                       *p_output_ev;

    /* noise                                                       */
    /* entries : [N_NOISE_SOURCES]                                 */
    sk_real                               *p_bias_dep_noise_parts;
    sk_real                               *p_freq_dep_noise_parts;

    /* topology modifier */
    SK_STATUS_TYPE                     *p_branch_status;
                                        /* N_TOTAL_BRANCHES entries */
    SK_STATUS_TYPE                     *p_noise_source_status; 
                                        /* N_NOISE_SOURCES entries */

    sk_real                               *p_opo_values;
    /* points to start of operating parameter section
    within values array of inst_data structure (JvP, May 16 2002) */
    /* limiting*/
    SK_LIMITING                        *p_limit_info;

    /* didv_min enabler */
    sk_boolean                             insert_didvmin;
} SK_INST_VARIABLES;


/* transfers simulation data from the simulator to the model       */
typedef struct
{
    sk_real                                temperature;   
    /* needed for temperature scaling */
    /* ( eval_int_params() )          */

    sk_real                                frequency;
    /* needed for frequency dependent */
    /* noise density calculation      */
    /* ( eval_model() )               */

    sk_real                                inst_scale;
    /* needed for scaling in spectre  */
    /* ( eval_int_params() )          */
    /* ( get_inst_params() )          */
} SK_SIM_DATA;


/* Parameter types */
#define SK_PT_NONE                      0x0000
#define SK_PT_MAXISET_MODEL             0x0001
#define SK_PT_MAXISET_INST              0x0002
#define SK_PT_MINISET                   0x0004
#define SK_PT_ELECTRICALSET             0x0008
#define SK_PT_OPERATINGPOINT            0x0010
typedef SK_MASK_TYPE                    SK_PARAM_TYPE;

/* parameter descriptor           */
typedef struct
{
    const sk_unint                         number;
    const char                         *p_name;
    const char                         *p_description;
    const char                         *p_unit;
    const sk_boolean                       is_readable;
    const sk_boolean                       is_writeable;
    const sk_boolean                       is_model_param;
    const SK_SCALING_FACTOR             scaling_type;
    const SK_CLIP_TYPE                  clip_type_n;
    const SK_CLIP_TYPE                  clip_type_p;
    const sk_real                          clip_low_n;
    const sk_real                          clip_low_p;
    const sk_real                          clip_high_n;
    const sk_real                          clip_high_p;
    const sk_real                          default_value_n;
    const sk_real                          default_value_p;
    const SK_AFFECTS_TYPE               dependency;
    const SK_PARAM_TYPE                 param_type;
} SK_PARAM_DESCRIPTOR;


/* parameter descriptor define for unused fields */
#define SK_NOT_USED                     0


/* Model and instance parameters together form the maxiset */
#define SK_PARAM_MAXI_MODEL(name, p_descr, p_unit, \
    clip_type_n, clip_type_p, \
    clip_low_n, clip_low_p, clip_high_n, clip_high_p, default_n, default_p) \
    static SK_PARAM_DESCRIPTOR PARAM_MAXI_MODEL_ ## name = \
    { \
        PARNUM_MAXI_MODEL_ ## name, \
        #name, \
        p_descr, \
        p_unit, \
        TRUE, TRUE, TRUE, \
        SK_SF_NONE, \
        clip_type_n,    clip_type_p, \
        clip_low_n,     clip_low_p, \
        clip_high_n,    clip_high_p, \
        default_n,      default_p, \
        SK_AT_AFFECTS_OP_POINT, \
        SK_PT_MAXISET_MODEL \
    }

#define SK_PARAM_MAXI_INST(name, p_descr, p_unit, scaling_type, \
    clip_type_n, clip_type_p, \
    clip_low_n, clip_low_p, clip_high_n, clip_high_p, default_n, default_p) \
    static SK_PARAM_DESCRIPTOR PARAM_MAXI_INST_ ## name = \
    { \
        PARNUM_MAXI_INST_ ## name, \
        #name, \
        p_descr, \
        p_unit, \
        TRUE, TRUE, FALSE, \
        scaling_type, \
        clip_type_n,    clip_type_p, \
        clip_low_n,     clip_low_p, \
        clip_high_n,    clip_high_p, \
        default_n,      default_p, \
        SK_AT_AFFECTS_OP_POINT, \
        SK_PT_MAXISET_INST \
    }

/* miniset */
#define SK_PARAM_MINI(name, p_descr, p_unit, clip_type_n, clip_type_p, \
    clip_low_n, clip_low_p, clip_high_n, clip_high_p) \
    static SK_PARAM_DESCRIPTOR PARAM_MINI_ ## name = \
    { \
        PARNUM_MINI_ ## name, \
        #name, \
        p_descr, \
        p_unit, \
        FALSE, FALSE, FALSE, \
        SK_SF_NONE, \
        clip_type_n,    clip_type_p, \
        clip_low_n,     clip_low_p, \
        clip_high_n,    clip_high_p, \
        SK_NOT_USED,    SK_NOT_USED, \
        SK_AT_AFFECTS_OP_POINT, \
        SK_PT_MINISET \
    }

/* electrical set */
#define SK_PARAM_ELEC(name, p_descr, p_unit, clip_type_n, clip_type_p, \
    clip_low_n, clip_low_p, clip_high_n, clip_high_p) \
    static SK_PARAM_DESCRIPTOR PARAM_ELEC_ ## name = \
    { \
        PARNUM_ELEC_ ## name, \
        #name, \
        p_descr, \
        p_unit, \
        TRUE, FALSE, FALSE, \
        SK_SF_NONE, \
        clip_type_n,    clip_type_p, \
        clip_low_n,     clip_low_p, \
        clip_high_n,    clip_high_p, \
        SK_NOT_USED,    SK_NOT_USED, \
        SK_AT_AFFECTS_OP_POINT | SK_AT_TEMP_AFFECTS, \
        SK_PT_ELECTRICALSET \
    }

/* Operating point */
#define SK_PARAM_OP(name, p_descr, p_unit) \
    static SK_PARAM_DESCRIPTOR PARAM_OP_ ## name = \
    { \
        PARNUM_OP_ ## name, \
        #name, \
        p_descr, \
        p_unit, \
        TRUE, FALSE, FALSE, \
        SK_SF_NONE, \
        SK_NOT_USED,    SK_NOT_USED, \
        SK_NOT_USED,    SK_NOT_USED, \
        SK_NOT_USED,    SK_NOT_USED, \
        SK_NOT_USED,    SK_NOT_USED, \
        SK_AT_OP_POINT_AFFECTS, \
        SK_PT_OPERATINGPOINT \
    }


/* Access parameter values */
#define MM(a)           p_m_data->values[PARNUM_MAXI_MODEL_##a]
#define MI(a)           p_i_data->values[PARNUM_MAXI_INST_##a-N_MODEL_PARAMS]
#define MS(a)           p_i_data->values[PARNUM_MINI_##a-N_MODEL_PARAMS]
#define ES(a)           p_i_data->values[PARNUM_ELEC_##a-N_MODEL_PARAMS]
#define OP(a)           p_i_data->values[PARNUM_OP_##a-N_MODEL_PARAMS]
#ifdef SELFHEATING
#define ES_dVT(a)       p_i_data->values_dVT[PARNUM_ELEC_##a-N_MODEL_PARAMS]
#endif


/* Macros for testing parameter states */
#define SK_IS_PARAM_READABLE(p_param) ((p_param)->is_readable)
#define SK_IS_PARAM_WRITEABLE(p_param) ((p_param)->is_writeable)
#define SK_IS_TEMP_AFFECTS_PARAM(p_param) \
        ((p_param)->dependency & SK_AT_TEMP_AFFECTS)
#define SK_IS_OP_AFFECTS_PARAM(p_param) \
        ((p_param)->dependency & SK_AT_OP_POINT_AFFECTS)
#define SK_IS_PARAM_AFFECTS_OP(p_param) \
    ((p_param)->dependency & SK_AT_AFFECTS_OP_POINT)


/* model descriptor */
typedef struct
{
    const sk_real                          gmin;
    const sk_real                          didv_min;
    const sk_real                          version_nr;
    const char                         *p_name;
    const char                         *p_title;
    const sk_unint                         n_terminals;
    const sk_unint                         n_static_branches;
    const sk_unint                         n_static_linear_branches;
    const sk_unint                         n_dynamic_branches;
    const sk_unint                         n_dynamic_linear_branches;
    const sk_unint                         n_gmin_branches;
    const sk_unint                         n_didvmin_branches;
    const sk_unint                         n_noise_sources;
    const sk_unint                         n_int_nodes;
    const SK_BRANCH                    *p_branch_DS; /* For MOS unwanted SD interch.*/
    const SK_NODE                     **pp_term_nodes;
    const SK_NODE                     **pp_int_nodes;
    const sk_unint                         n_model_params;
    const sk_unint                         n_inst_params;
    const sk_unint                         n_op_info;
    const sk_unint                         model_data_size;
    const sk_unint                         inst_data_size;
    const SK_PARAM_DESCRIPTOR         **pp_params;    /* Please note: it is assumed
                                                   this array contains
                                                   n_model_params model params
                                                   followed by n_inst_params
                                                   instance parameters followed
                                                   by n_op_info op point
                                                   parameters. */
    const SK_BRANCH                   **pp_static_branches;
    const SK_BRANCH                   **pp_static_linear_branches;
    const SK_BRANCH                   **pp_dynamic_branches;
    const SK_BRANCH                   **pp_dynamic_linear_branches;
    const SK_BRANCH                   **pp_gmin_branches;
    const SK_BRANCH                   **pp_didvmin_branches;
    const SK_NOISE_SOURCE             **pp_noise_sources;

    /* interface functions */
    SK_ERROR (*p_init_model)        ( void  *p_model_data,
                                   int model_type );

    SK_ERROR (*p_init_inst)         ( const void  *p_model_data,
                                   void  *p_inst_data,
                                   SK_INST_VARIABLES **pp_inst_variables,
                                   const char* p_inst_name,
                                   const char** pp_occ_ind,
                                   const char** pp_sit );

    SK_ERROR (*p_set_model_params)  ( void  *p_model_data,
                                   const sk_unint *p_param_id_list,
                                   const sk_real  *p_value_list,
                                   const sk_unint  n_params,
                                   sk_boolean *p_is_param_changed );

    SK_ERROR (*p_set_inst_params)   ( void  *p_inst_data,
                                   const sk_unint *p_param_id_list,
                                   const sk_real  *p_value_list,
                                   const sk_unint  n_params,
                                   sk_boolean *p_is_param_changed );

    SK_ERROR (*p_get_model_params)  ( const void  *p_model_data,
                                   const sk_unint *p_param_id_list,
                                   sk_real  *p_value_list,
                                   const sk_unint  n_params,
                                   const SK_SIM_DATA *p_sim_data  );

    SK_ERROR (*p_get_inst_params)   ( const void  *p_inst_data,
                                   const sk_unint *p_param_id_list,
                                   sk_real  *p_value_list,
                                   const sk_unint  n_params,
                                   const SK_SIM_DATA *p_sim_data  );

    SK_ERROR (*p_reset_model_params)( void  *p_model_data,
                                   const sk_unint *p_param_id_list,
                                   const sk_unint  n_params  );

    SK_ERROR (*p_reset_inst_params) ( const void  *p_model_data,
                                   void  *p_inst_data,
                                   const sk_unint *p_param_id_list,
                                   const sk_unint  n_params  );

    SK_ERROR (*p_eval_int_params)   ( void  *p_model_data,
                                   void  *p_inst_data,
                                   const SK_SIM_DATA     *p_sim_data,
                                   const SK_INIT_CONTROL  flag  );

    SK_ERROR (*p_eval_model)        ( const void  *p_model_data,
                                   void  *p_inst_data,
                                   const SK_SIM_DATA     *p_sim_data,
                                   const SK_EVAL_CONTROL  flag );

    SK_ERROR (*p_eval_u_noise)      ( const void  *p_model_data,
                                   const void *p_inst_data,
                                   sk_unint noise_src_number,
                                   const sk_real frequency,
                                   SK_COMPLEX *p_h_noise,
				   SK_COMPLEX *p_u_noise);

    /* start new initial guess */
    const sk_unint                      n_ev;
    const sk_boolean                   *p_ev_ig_available;
    const SK_EV_DESCRIP               **pp_ev_descrip;

    SK_MODEL_FAMILY                     model_family;
} SK_MODEL;

/******************************************************************************/

/* Interface that the models use to report info / warnings / errors to the
   _simulator_ (as opposed to the adapter, which uses SK_ERROR). The adapter
   implements the function below which converts this information to simulator
   specific status reporting. */

typedef enum
{
    SK_REP_NONE,
    SK_REP_COPYRIGHT_NOTICE,
    SK_REP_PARAMETER_CLIPPED,
    SK_REP_NEGATIVE_WEFF,
    SK_REP_NEGATIVE_LEFF,
    SK_REP_INFO_TEXT,
    SK_REP_PARAMETER_READONLY,
    SK_REP_PARAMETER_UNKNOWN,
    SK_REP_SIMKIT_MESSAGE,
    SK_REP_DEVICE_TOO_HOT,
    SK_REP_OUT_OF_MEMORY,
    SK_REP_FAILED_GEOM_CHECK
} SK_REPORT_STATUS;

/**
 * Type of a function to report warnings, errors and debug output in a simulator 
 * specific way. This function may be called by the adapter
 * itself or any model.
 * @param aStatus Message type
 * @param anInfo  Strings to be put in the message
 */
typedef void (*SK_REPORT_TO_SIMULATOR_FUNC)(SK_REPORT_STATUS aStatus, const char* anInfo[]);

#define SK_REPORT_INFO_SIZE 6

#define COMPLEX_MULT_ASSIGN(u, v)  { \
    sk_real r_real,r_imag; \
    r_real=(u).Real*(v).Real-(u).Imag*(v).Imag; \
    r_imag=(u).Real*(v).Imag+(u).Imag*(v).Real; \
    (u).Real=r_real; \
    (u).Imag=r_imag; \
    }

#define COMPLEX_DIV_ASSIGN(u, v)  { \
    sk_real r_real,r_imag,norm2; \
    norm2=(v).Real*(v).Real+(v).Imag*(v).Imag; \
    r_real=(u).Real*(v).Real+(u).Imag*(v).Imag; \
    r_imag=-(u).Real*(v).Imag+(u).Imag*(v).Real; \
    (u).Real=r_real/norm2; \
    (u).Imag=r_imag/norm2; \
    }


/******************************************************************************/

/* DATA DECLARATIONS (no structures) */
/*   defines / typedefs / macros ... */

/*----------------------- Type definitions ---------------------------------*/

/* Defines for boolean values */

#ifndef FALSE
#define  FALSE                  0
#define  TRUE                   1
#endif

/*------------------------- Release flag dependent -------------------------*/

#ifdef RELEASE
#define     MAINTENANCE                 FALSE        /* Release version */
#else
#define     MAINTENANCE                 TRUE       /* Development version */
#endif

/*------------------------- Computer system macros -------------------------*/


/*----------------------- Computer system definition -----------------------*/

#define     EBCDIC                      FALSE
#define     CASE_SENSITIVE_FILENAMES    TRUE
#define     LOWERCASE_FILENAMES         TRUE
#define     FLEX_LM                     TRUE
#define     MIXED_SIGNAL_SUPPORT        TRUE

#undef      tolower
#define     tolower(c)  ( isupper(c) ? ((c)-'A'+'a') : (c) )

#undef      toupper
#define     toupper(c)  ( islower(c) ? ((c)-'a'+'A') : (c) )


/*------------------------ Definitions for Constants -----------------------*/

                            /* VAX-11 constants !! */
#define  MIN_INTEGER            ((long int) -2147483647)
#define  MAX_INTEGER            2147483647
#define  MIN_NUMBER             3.0E-39
#define  MAX_NUMBER             1.7E38
#define  UNDEF_NUMBER           1.654321E38
#define  UNDEF_BOF_RESULT       1.6E38
#define  N_SIGNIFICANT_DIGITS   14

        /* constants dependent on VAX-11 constants;
         * requirements: 1/P_INFINITY > MIN_NUMBER
         *               and
         *               TOLERANCE exponent <= N_SIGNIFICANT_DIGITS
         */
#define  P_INFINITY               (MAX_NUMBER/100.0)
#define  TOLERANCE              1.0E-12

#ifdef LN_MINDOUBLE
#undef LN_MINDOUBLE
#endif
#define LN_MINDOUBLE -800.0


/*---------------------------------------------------------------------------*
 *  Fundamental Physical Constants                                           *
 *  source: Handbook of Chemistry and Physics, 54th edition, 1973            *
 *---------------------------------------------------------------------------*/
#define  PI                  3.14159265358979323844 /* 21 decimal digits     */
#define  TWOPI               6.28318530717958647688 /* 21 decimal digits     */
#define  SQRT2               1.41421356237309504880 /* 21 decimal digits     */
#define  DEGPERRAD          57.29577951308232087721 /* 22 decimal digits     */
#define  Q_ELECTRON          1.6021918E-19      /* Electron charge      (C)  */
#define  K_BOLTZMANN         1.3806226E-23      /* Boltzmann's constant (J/K)*/
#define  EPSILON_0           8.854214871e-12    /* diele.con. (F/m)          */
#define  EPSILON_OX          3.453143800e-11    /* permittivity of SiO2 (F/m)*/
#define  EPSILON_SI        (11.7*EPSILON_0)     /* diele.con. of silicon(F/m)*/
#define  KELVIN_CONVERSION 273.15               /* Celsius to Kelvin    (K)  */

/* Derived from these:                                                       */
#define  K_B_div_Q_EL        8.61708691805812512584e-5

/* Macro to get rid of GCC's unused parameter warning. When using the GCC
   compiler this applies the unused attribute and also mangles the variable
   name so that you really can't use it. */
#ifdef __GNUC__
#define SK_UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#define SK_UNUSED(x) x
#endif


#endif /* _SK_H */

