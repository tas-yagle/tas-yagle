/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : STM Version 2.00                                             */
/*    Fichier : stm200.h                                                     */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef STM
#define STM

#define EXPNEW
#define NEWSIGN
#define STM_NEWSIGN_ROUND_PRECISION 1e5

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <ctype.h>

#include MUT_H
#define API_HIDE_TOKENS
#include API_H
#include SIM_H
#include ELP_H
#ifdef AVERTEC
#include AVT_H
#endif

/*****************************************************************************/
/*     Defines                                                               */
/*****************************************************************************/
/* Arguments of Name Generation                                              */
/*****************************************************************************/

#define STM_ANY         0
#define STM_ONLY_ONE    1

#define STM_DELAY       0x01
#define STM_SLEW        0x02
#define STM_ACCESS      0x04
#define STM_HOLD        0x08
#define STM_SETUP       0x10
#define STM_UP          'U'
#define STM_DN          'D'
#define STM_MIN         'm'
#define STM_MAX         'M'

/*****************************************************************************/

#define STM_SEVERITY_ERR 1
#define STM_SEVERITY_WAR 2

/*****************************************************************************/
/*     Ptypes                                                                */
/*****************************************************************************/
#define STM_CARAC_VALUES     0x7000001         /* USER field of timing_model */
#define STM_SHARE_INFO       0x7000002         /* USER field of timing_model */
#define STM_SLEW_FIX        (long)0xc1c00263
#define STM_LOAD_FIX        (long)0xc1c10263

/*****************************************************************************/
/* Special Values                                                            */               
/*****************************************************************************/
#define STM_UNKNOWN        -FLT_MAX
#define STM_NOVALUE        FLT_MAX
#define STM_DONTCARE       FLT_MAX 
#define STM_DEF_AXIS_BASE  2  /* real axis size is 2 * STM_AXIS_DEF_BASE + 1 */
                             
/*****************************************************************************/
/* Cell Type                                                                 */              
/*****************************************************************************/
#define STM_COMB             'C'
#define STM_SEQ              'S'
#define STM_IGNORE           'I'
                             
/*****************************************************************************/
/* Model Type                                                                */
/*****************************************************************************/
#define STM_MOD_MODNULL      0x0
#define STM_MOD_MODTBL       0x1
#define STM_MOD_MODSCM       0x2
#define STM_MOD_MODPLN       0x3
#define STM_MOD_MODFCT       0x4
#define STM_MOD_MODIV        0x5
                         
#define STM_MOD_TYPEMASK     0xf

/*****************************************************************************/
/* Short Channel Model Parameters                                            */        
/*****************************************************************************/
/* Model Type */
#define STM_MODSCM_DUAL      0x10
#define STM_MODSCM_GOOD      0x20
#define STM_MODSCM_FALSE     0x30
#define STM_MODSCM_PATH      0x40
#define STM_MODSCM_CST       0x50

#define STM_MODSCM_TYPEMASK  0xf0

/* Cst Parameters */
#define STM_CST_DELAY        0
#define STM_CST_SLEW         1
#define STM_NB_CST_PARAMS    2


/* Dual Parameters */
#define STM_PCONF0           0
#define STM_PCONF1           1
#define STM_CAPAI            2
#define STM_IRAP             3
#define STM_VDDIN            4
#define STM_VT               5
#define STM_THRESHOLD        6
#define STM_IMAX             7
#define STM_AN               8
#define STM_BN               9
#define STM_VDDMAX          10
#define STM_RSAT            11
#define STM_RLIN            12
#define STM_DRC             13
#define STM_RBR             14
#define STM_CBR             15
#define STM_INPUT_THR       16
#define STM_RINT            17
#define STM_VINT            18
#define STM_CHALF           19
#define STM_RCONF           20
#define STM_KF              21
#define STM_QINIT           22
#define STM_KRT             23
// --- unused ---
#define STM_CAPAO           24
#define STM_CAPAI0          25
#define STM_CAPAI1          26
#define STM_CAPAI2          27
#define STM_CAPAI3          28
#define STM_VT0             29
#define STM_VT0C            30
#define STM_NB_DUAL_PARAMS  31

/* Pass Links Parameters */
/* link 0 */
#define STM_CI_0             0
#define STM_CF_0             1
#define STM_K3_0             2
#define STM_K4_0             3
#define STM_K5_0             4
#define STM_NB_LINK_0_PARAMS 5

/* Pass Links Definition */
#define STM_NB_I_LINKS       0

/* link 1 -> n - 1 */
#define STM_CI_I             0
#define STM_CF_I             1
#define STM_ACTI_I           2
#define STM_BCTI_I           3
#define STM_K3_I             4
#define STM_K4_I             5
#define STM_K5_I             6
#define STM_NB_LINK_I_PARAMS 7

/* link n (dual) */
#define STM_CI_N             0
#define STM_CF_N             1
#define STM_ACTI_N           2
#define STM_BCTI_N           3
#define STM_NB_LINK_N_PARAMS 4

/* link false */
#define STM_PCONF0_F         0
#define STM_PCONF1_F         1
#define STM_RTOT_F           2
#define STM_K_F              3
#define STM_VDDMAX_F         4
#define STM_DRC_F            5
#define STM_VF_INPUT_F       6
#define STM_NB_FALSE_PARAMS  7

/*****************************************************************************/
/* Polynom Parameters                                                        */
/*****************************************************************************/
#define STM_DEGSHIFT         3    /* shift degre des variables de 3 bits */
#define STM_DEGMASK          0x7  /* masque degre des variables sur 3 bits */
#define STM_COEF_DEFMASK     0x1  /* masque position des coefs */
#define STM_RCSF_DEG         0x9  /* degre des variables du poly rc + sf */
#define STM_RCSF_NBVAR       0x2  /* nb de variables du poly rc + sf */
#define STM_RCSF_CDEF        0x7  /* position des coefs du poly rc + sf */
#define STM_PRESCALE         1000 /* precision des variables */

/*****************************************************************************/
/* Path Parameters                                                           */
/*****************************************************************************/
#define STM_VDDMAX_P        1
#define STM_NB_PATH_PARAMS  2

/*****************************************************************************/
/* Lookup Table Model Parameters                                             */
/*****************************************************************************/
/* X & Y types */           
#define STM_NOTYPE           'N'
#define STM_LOAD             'L'
#define STM_INPUT_SLEW       'I'
#define STM_CLOCK_SLEW       'C'
                             
/*****************************************************************************/
/* Model Reduction Mode                                                      */
/*****************************************************************************/
#define STM_LOADFIX_RED      'L'
#define STM_SLEWFIX_RED      'S'
#define STM_CLOCKSLEWFIX_RED 'K'
#define STM_COPY             'C'

/*****************************************************************************/
/* Noise model and InVth model                                               */
/*****************************************************************************/
#define STM_NOISE_NULL       0x00  /* No model of noise */
#define STM_NOISE_SCR        0x01  /* Simple constant resistance */
#define STM_INVTH_NULL       0x00  /* No model of inVth */
#define STM_INVTH_SPL        0x10  /* simple model of inVth */

/*****************************************************************************/
/* Energy model                                                              */
/*****************************************************************************/
#define STM_ENERGY_NULL       0x0  /* No model of energy */
#define STM_ENERGY_PARAMS     0x1  
#define STM_ENERGY_TABLE      0x2  

/* Energy Parameters */
#define STM_EPCONF0     0
#define STM_EPCONF1     1
#define STM_NB_ENERGY_PARAMS    2

/*****************************************************************************/
/* Energy model                                                              */
/*****************************************************************************/
#define STM_HL 0
#define STM_LH 1
#define STM_HH 2
#define STM_LL 3
/*****************************************************************************/
/* MCC convergence parameter                                                 */
/*****************************************************************************/
#define STM_MCC_EPSILON      ((float)0.0001)

/*****************************************************************************/
/* PWL Ptype                                                                 */
/*****************************************************************************/
#define STM_SLOPEMIN_PWL_PTYPE 0x0001
#define STM_SLOPEMAX_PWL_PTYPE 0x0002

/*****************************************************************************/
/*     globals                                                               */
/*****************************************************************************/
extern float STM_DEFAULT_VT;                                                                              
extern float STM_DEFAULT_VTN;                                                                              
extern float STM_DEFAULT_VTP;                                                                              
extern float STM_DEFAULT_VFD;
extern float STM_DEFAULT_VFU;
extern float STM_DEFAULT_TEMP;
extern float STM_DEFAULT_SMINR;
extern float STM_DEFAULT_SMAXR;
extern float STM_DEFAULT_SMINF;
extern float STM_DEFAULT_SMAXF;
extern float STM_DEF_SLEW;    /* global default input slew, in picoseconds  */         
extern float STM_DEF_LOAD;    /* global default output capa, in femtofarhad */ 
extern float STM_DEFAULT_RLIN;
extern float STM_DEFAULT_VSAT;
extern char *STM_TECHNONAME;
                                                                              
/*****************************************************************************/
/*     structures                                                            */
/*****************************************************************************/
/* Hash Tables Management Defines */
#define STM_EMPTYHT     -1
#define STM_DELETEHT    -2
#define STM_HMAX_CALLS  20

/*****************************************************************************/
/* Slope PWTH */

typedef struct stm_pwl {
    float *TAB;                              /* tab[0] = t0, tab[1] = V0  ...*/
    int    N;                                /* size of tab = 2 * n          */ 
} stm_pwl;

typedef struct stm_pwth {
  float F ;
  float DT ;
  float T ;
} stm_pwth ;

typedef struct {
  stm_pwth   *PWTH ;
  int         NTH ;
  double      VT ;
  double      VDD ;
} stm_pwth_fn ;

typedef struct {
  float r ;
  float c ;
  float v ;
} stm_driver ;

typedef struct {
  double  DT ;
  FILE   *FIN ;
  FILE   *FOUT ;
} newctktrace ;

/*****************************************************************************/
/* STM Hash Table */
typedef struct stm_htitem {                   /* hash table element          */
    char *key;                                /* pointer for access          */
    long  value;                              /* associated value            */
} stm_htitem;

typedef struct stm_htable {                   /* hash table itself           */
    long        length;                       /* table length                */
    stm_htitem *pElem;                        /* pointer to table start      */
    long        count;                        /* number of inserted elements */
} stm_ht;

typedef struct stm_carac {
	float  *VALUES ;
	int     NVALUES ;
} stm_carac ; 

/*****************************************************************************/
/* Cell */
typedef struct timing_cell {
    char       *NAME;
    chain_list *MODEL_LIST;
    ht         *MODEL_HT;
/*    chain_list *SIM_MODEL_LIST;
    ht         *SIM_MODEL_HT;*/
#ifdef EXPNEW
    ht         *MODEL_HTSIGN;
#else
    stm_ht     *MODEL_HTSIGN;
#endif
#ifndef NEWSIGN
    NameAllocator na;
#endif
    int        FILE_D; /*info for the cache */
} timing_cell;

/*****************************************************************************/
/* Templates */
typedef struct timing_ttable {
    char   *NAME;
    float  *XRANGE;
    float  *YRANGE;
    int     NX;
    int     NY;
    char    XTYPE;
    char    YTYPE;
    char    XTYPEBIS;
} timing_ttable;
    
/* Polynom variables definition */

typedef struct timing_varpolynom {
    char    TYPE;
    float   THMIN;
    float   THMAX;
} timing_varpolynom;

/* Polynom Model */

typedef struct timing_polynom {
    timing_varpolynom **VAR;
    long    DEG_NBVAR;
    long   *COEF_DEF;
    float  *COEF_TAB;
} timing_polynom;

/* Table Model */

typedef struct timing_table {
    timing_ttable *TEMPLATE;
    float          CST;
    float         *SET1D;
    float        **SET2D;
    float         *XRANGE;
    float         *YRANGE;
    char           NX;
    char           NY;
    char           XTYPE;
    char           YTYPE;
    char           XTYPEBIS;
} timing_table;

/* IV Model */

// Initial state
typedef struct timing_iv_ti {
  float            VI;
} timing_iv_ti;

// Conflict
typedef struct timing_iv_cf {
    float          PCONF0;
    float          PCONF1;
    float          IRAP;
    float          CI;
} timing_iv_cf;

// Input slope characteristic
typedef struct timing_iv_in {
    float          VI;
    float          VF;
    float          VTH;
    float          VT;
} timing_iv_in;

typedef struct timing_iv {
    int            NVE;
    float         *VE;
    int            NVS;
    float         *VS;
    float         *IS;
    timing_iv_cf   IV_CONF;
    timing_iv_in   IV_INPUT;
    timing_iv_ti   IV_INIT;
} timing_iv;

/* Scm Model */

typedef struct cstparams {
    float  *DS;
} cstparams;

typedef struct dualparams {
    float  *DP;
} dualparams;

typedef struct goodparams {
    float  *DP;
    float  *L0;
    float  *LI;
    float  *LN;
} goodparams;

typedef struct falseparams {
    float  *FP;
    float  *L0;
    float  *LI;
    float  *LN;
} falseparams;

typedef struct pathparams {
    float  *PP;
    float  *L0;
    float  *LI;
    float  *LN;
} pathparams;

typedef union uparams {
    dualparams  *DUAL;
    goodparams  *GOOD;
    falseparams *FALS;
    pathparams  *PATH;
    cstparams   *CST;
} uparams;

typedef struct timing_scm {
    union uparams    PARAMS;
    long             TYPE;
} timing_scm;

/* structures shared */
typedef struct stm_share
{
  ptype_list        *USER;
}
stm_share;

typedef struct timing_function {
    char            *INS;
    char            *LOCALNAME;
    APICallFunc     *FCT;
    stm_share       *SHARE;
} timing_function;

/* High Level Model */
typedef union umodel {
    timing_table    *TABLE;
    timing_scm      *SCM;
    timing_polynom  *POLYNOM;
    timing_function *FUNCTION;
    timing_iv       *IV;
} umodel;

/* Noise model : simple constant resistance */
typedef struct noise_scr {
  float                 R;
  float                 INVTH;
} noise_scr;

/* High level parameters for noise calculation */
typedef union unoisemodel {
  void                 *PTR;
  struct noise_scr     *SCR;
} unoisemodel;

/* Energy Model */
typedef struct energyparams {
    float   *EP;    
} energyparams;

typedef union uenergymodel {
    energyparams    *EPARAMS;
    timing_table    *ETABLE;
} uenergymodel;

typedef struct timing_model {
    char               *NAME;
    char                UTYPE;
    char                NOISETYPE; 
    char                ENERGYTYPE;
    char                TTYPE;
    union umodel        UMODEL;
    ptype_list         *USER;
    union unoisemodel   NOISEMODEL;
    union uenergymodel  ENERGYMODEL;
    float               VT;
    float               VF;
    float               VTH;
    float               VDD;
    MBK_OFFSET_MAX      OFFSET; /* info for the cache */
    chain_list         *POS_IN_LIST; // position dans la liste des models
} timing_model;

typedef struct timing_props {
    timing_model *RESMODEL;
    timing_model *CAPMODEL;
} timing_props;

typedef struct segment {
    float X0;
    float X1;
    float INTER;
    float SLO;
} segment;

typedef struct stm_carac_values {
    float SLEW_MIN;
    float SLEW_MAX;
    float SLEWOUT_MIN;
    float SLEWOUT_MAX;
    float LOAD_MIN;
    float LOAD_MAX;
} stm_carac_values;

typedef struct {
  double    IMAX ;
  double    A ;
  double    B ;
  double    U ;
  double    F ;
  double    VT ;
  int       NTH ;
  stm_pwth *PWTH ;
} stm_param_isat ;

typedef struct {
  mbk_laplace_data DATA;
  double           k1, k2, k3, k4, k5, k6, k7;
} stm_laplace_capaeq;

/******************************************************************************\
Definition d'un transistor : ids=f(vgs,vds) et vds=f(vgs,ids), ainsi que sa
polarisation.
\******************************************************************************/

typedef struct {
  char (*FN_VDS)( void *dat_vds, float vs, float ids, float *vds );
  char (*FN_IDS)( void *dat_ids, float vs, float vds, float *ids );
  chain_list                         *MODEL_VDS;
  chain_list                         *MODEL_IDS;
  float                               VD;
  float                               VS;
  float                               IDS;
} stm_solver_maillon;

/******************************************************************************\
Definition d'une liste de transistor d'une branche.
Le premier transistor est la sortie de la branche.
\******************************************************************************/
typedef struct stm_solver_maillon_list {
  struct stm_solver_maillon_list     *NEXT;
  struct stm_solver_maillon_list     *PREV;
  stm_solver_maillon                 *MAILLON;
} stm_solver_maillon_list;

/******************************************************************************\
Paramètres de simulation.
\******************************************************************************/
typedef struct {
  float         DEFAULT_TIME_STEP;      // Fs
  float         MIN_TIME_STEP;          // Fs
  float         MAX_TIME_STEP;          // Fs
  float         MAX_DELTA_CURRENT;      // A
  float         MIN_DELTA_CURRENT;      // A
  float         MAX_DELTA_VOLTAGE;      // V
  float         MIN_DELTA_VOLTAGE;      // V
  int           MAX_ITER;
  char          VARIABLE_STEP;
  char         *PLOTFILE;
  /* statistics */
  int           STAT_ITER;
  int           STAT_CHANGE_STEP;
  float         STAT_MIN_TIME_STEP;
  float         STAT_MAX_TIME_STEP;
  float         STAT_MIN_DELTA_VOLTAGE;
  float         STAT_MAX_DELTA_VOLTAGE;
} stm_solver_parameter ;

/******************************************************************************\
Courbes
\******************************************************************************/

typedef struct {
  float T ;
  float VE ;
  float VS ;
  float I ;
  float ICONF ;
} stm_curve_data;

typedef struct {
  int             NBPOINTS ;
  int             DATAFREE ;
  stm_curve_data *DATA ;
} stm_curve_time;

typedef struct {
  
} stm_curve_current;

typedef struct {
  int             NBVE ;
  int             NBVS ;
  float          *VE;
  float          *VS;
  float          *I;
} stm_curve_static;

#define STM_CURVE_STATIC_GET_PTR_I(curve,ne,ns) (curve->STATIC.I + ne*curve->STATIC.NBVS + ns)

typedef struct {
  timing_model     *MODEL ;
  stm_curve_time    TIME ;
  stm_curve_static  STATIC ;
  float             T0 ;
  float             TMAX ;
} stm_curve ;

/******************************************************************************\
Simulation
\******************************************************************************/

typedef struct {
  float         DEFAULT_TIME_STEP;      // Fs
  float         MIN_TIME_STEP;          // Fs
  float         MAX_TIME_STEP;          // Fs
  float         MAX_DELTA_CURRENT;      // A
  float         MIN_DELTA_CURRENT;      // A
  float         MAX_DELTA_VOLTAGE;      // V
  float         MIN_DELTA_VOLTAGE;      // V
  int           MAX_ITER;
  char          VARIABLE_STEP;
  /* statistics */
  int           STAT_ITER;
  int           STAT_CHANGE_STEP;
  float         STAT_MIN_TIME_STEP;
  float         STAT_MAX_TIME_STEP;
  float         STAT_MIN_DELTA_VOLTAGE;
  float         STAT_MAX_DELTA_VOLTAGE;
  float         STAT_MIN_DELTA_CURRENT;
  float         STAT_MAX_DELTA_CURRENT;
} stm_simu_pi_load_parameter ;

typedef struct {
  float VT;
  float A;
  float B;
  float RNT;
  float RNS;
  float VDD;
  char  NEG;
  float IMAX;
  float k;
  float kres;
} param_mcc_ids;

typedef struct {
  float VI;
  float VF;
  float VT;
  float F;
} stm_simu_tanh_param;

/*****************************************************************************/
/*     globals                                                               */
/*****************************************************************************/

extern char       *STM_IN;       /* input format, default = "stm"           */
extern char       *STM_OUT;      /* output format, default = "stm"          */
extern chain_list *STM_TEMPLATES_CHAIN;
extern timing_props *STM_PROPERTIES;
extern float    *STM_AXIS_SLOPEIN;
extern float    *STM_AXIS_CAPAOUT;
extern int       STM_AXIS_NSLOPE;
extern int       STM_AXIS_NCAPA;
//extern int       STM_USE_MSC;


typedef struct {
  stm_pwth      PWTHSHRK[1000] ;
  int           NTHSHRK ;
  double        IMAX ;
  double        AN ;
  double        BN ;
  double        U ;
  double        CAPA ;
  double        CAPA0 ;
  double        F ;
  double        VT ;
  double        VDDIN ;
  double        VDDMAX ;
  double        CAPAI ;
  double        CAPAIE[4] ;
  double        VCAP[3] ;
  double        FIN ;
  double        VSAT ;
  double        RLIN ;
  double        RSAT ;
  double        RINT ;
  double        VINT ;
  double        FOUT ;
  double        T0 ;
  double        TE ;
  double        THRESHOLD ;
  double        RBR ;
  double        CBR ;
  double        CHALF ;
  char          INTEGNUMERIC ;
  char          COMPAT30P3 ;
  double        QINIT ;
  double        QSAT_VE ;
  double        QSAT_TE ;
  double        QSAT_Q0 ;
  double        DTPWL ;
  double        ICF0 ;
  double        STRANS ;
  
  /* driver parameter */
  stm_driver    DRIVER ;
  int           NEWCTKMODEL ;
  char          EVTIN ; /* debug */
  double        VIN ;   /* debug */
  double        TIN ;   /* debug */

  /* results */
  double        CEQRSAT ;
  double        DT ;
} stm_dual_param_timing ;

