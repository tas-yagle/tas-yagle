/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim100.h                                                     */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                Grégoire Avot                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef SIM_HEADER
#define SIM_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <ctype.h>

#include MUT_H
#include MLO_H
#include RCN_H
#include BEH_H
#include LOG_H
#include CNS_H
#include GSP_H
#include EFG_H
#include MSL_H
#include MLU_H

#ifdef AVERTEC
#include AVT_H
#endif


extern int    SIM_OUTLOAD;
extern char  *SIM_SPICE_OPTIONS;
extern double SIM_OUT_CAPA_VAL;
extern double SIM_INPUT_START;
extern double SIM_TECSIZE;
extern double SIM_VTH;
extern double SIM_VTH_HIGH;
extern double SIM_VTH_LOW;
extern double SIM_SLOP;
extern double SIM_SIMU_STEP;
extern double SIM_DC_STEP;
extern char  *SIM_SPICESTRING;
extern char  *SIM_SPICEOUT;
extern char  *SIM_SPICENAME;
extern char  *SIM_SPICESTDOUT;
extern char  *SIM_TECHFILE;
extern int    SIM_SPI_TRANSISTOR_AS_INSTANCE;
extern void  *SIM_EXTRACT_RULE;
extern char   SIM_DEFAULT_DRIVE_NETLIST ;
extern char   SIM_DEFAULT_OVR_FILES ;
extern char   SIM_DEFAULT_REMOVE_FILES ;
extern float  SIM_VT_SLOPEIN;

/* Labels for measure                                                         */
#define  SIM_DELAY_LABEL_STRING    ((char*)"SIM_DELAY")
#define  SIM_SLOPE_LABEL_STRING    ((char*)"SIM_SLOPE")
#define  SIM_DELAY_LABEL    ((long)0xFF000002)
#define  SIM_SLOPE_LABEL    ((long)0xFF000003)
#define  SIM_RC_DELAY_LABEL ((long)0xFF000000)
#define  SIM_RC_SLOPE_LABEL ((long)0xFF000001)
#define  SIM_LOOP_FEEDBACK_DELAY_LABEL "sim_loop_feed_back_delay"
#define  SIM_LOOP_FEEDBACK_DELAY_PTYPE 0xFF000004
#define  SIM_DELAY_TO_VT_LABEL "sim_command_delay_to_vt"
#define  SIM_DELAY_TO_VT_PTYPE 0xFF000005

/* Precision for calcul and storage of real number                            */
#define SIM_FLOAT double

typedef SIM_FLOAT (*PWL_FUNCTION) (SIM_FLOAT, void *);

/* Different outload type                                                     */

#define SIM_NO_OUTLOAD         ((int)0) 
#define SIM_DYNAMIC_OUTLOAD    ((int)1) 
#define SIM_TRANSISTOR_OUTLOAD ((int)2) 

/* Model of simulator - for netlist & technology interpretation               */
#define SIM_TOOLMODEL_SPICE   ((int)0)    /* Berkeley or old ngspice          */
#define SIM_TOOLMODEL_HSPICE  ((int)1)    /* Hspice family                    */
#define SIM_TOOLMODEL_ELDO    ((int)2)    /* Eldo family                      */
#define SIM_TOOLMODEL_TITAN   ((int)3)    /* Titan family                     */

/* Simulator - for external simulator linking                            */
#define SIM_TOOL_SPICE   ((int)0)    /* Berkeley or old Ngspice          */
#define SIM_TOOL_HSPICE  ((int)1)    /* Hspice                           */
#define SIM_TOOL_ELDO    ((int)2)    /* Eldo                             */
#define SIM_TOOL_TITAN   ((int)3)    /* Titan                            */
#define SIM_TOOL_TITAN7  ((int)4)    /* Titanv7                          */
#define SIM_TOOL_LTSPICE ((int)5)    /* Ltspice                          */
#define SIM_TOOL_MSPICE  ((int)6)    /* Mspice                           */
#define SIM_TOOL_NGSPICE ((int)7)    /* Ngspice                          */

/* Logical value                                                              */
#define SIM_ZERO ((char)'0')
#define SIM_ONE  ((char)'1')

/* Configuration                                                              */
#define SIM_YES  ((char)'y')
#define SIM_NO   ((char)'n')

/* Input description                                                          */
#define SIM_STUCK ((char)'t')   /* Constant input                             */
#define SIM_SLOPE ((char)'s')   /* Slope                                      */
#define SIM_FUNC  ((char)'f')   /* Function                                   */
#define SIM_MIMIC ((char)'m')   /* Mimic                                      */
#define SIM_NC    ((char)'z')   /* Not connected                              */

#define SIM_INPUT_SIGNAL      ((char)'s')
#define SIM_INPUT_LOCON       ((char)'l')

/* Measure description                                                        */
#define SIM_MEASURE_SIGNAL      ((char)'s')
//#define SIM_MEASURE_SIGNAL_NODE ((char)'n')
#define SIM_MEASURE_LOCON       ((char)'l')

/* Measured value                                                             */
#define SIM_MEASURE_VOLTAGE ((char)'v')
#define SIM_MEASURE_CURRENT ((char)'i')
#define SIM_MEASURE_NO      ((char)'n')
#define SIM_MEASURE_LAST    ((long) 1000000)

/* Stuck description                                                          */
#define SIM_STUCK_LEVEL  ((char)'l')  /* Logical 0 or 1 input                 */
#define SIM_STUCK_VALUE  ((char)'v')  /* Constant voltage generator           */

/* Slope description                                                          */
#define SIM_SLOPE_SINGLE  ((char)'s') /* Single slope                         */
#define SIM_SLOPE_PATTERN ((char)'p') /* Multiple splopes                     */

/* Return code from execution                                                 */
#define SIM_SIMU_COMPLETED      ((char)'y')
#define SIM_SIMU_CANTRUN        ((char)'n')
#define SIM_SIMU_NONIMPLEMENTED ((char)'i')
#define SIM_SIMU_CANTDRIVEFILE  ((char)'f')

/* Define for ICs                                                             */
#define SIM_IC_LEVEL            ((char)'l')
#define SIM_IC_VOLTAGE          ((char)'v')
#define SIM_IC_NOIC             ((char)'n')

#define SIM_IC_LOCON            ((char)'c')
#define SIM_IC_SIGNAL           ((char)'s')
//#define SIM_IC_SIGNAL_NODE      ((char)'n')

/* Unit management                                                            */
#define SIM_UNIT_TIME                    ((SIM_FLOAT)1.0)
#define SIM_UNIT_LABEL_SPICE_TIME        "s"
#define SIM_UNIT_CAPACITANCE             ((SIM_FLOAT)1.0)
#define SIM_UNIT_LABEL_SPICE_CAPACITANCE "F"
#define SIM_UNIT_RESISTANCE              ((SIM_FLOAT)1.0)
#define SIM_UNIT_LABEL_SPICE_RESISTANCE  ""
#define SIM_UNIT_VOLTAGE                 ((SIM_FLOAT)1.0)
#define SIM_UNIT_LABEL_SPICE_VOLTAGE     "V"

#define SIM_UNIT_X_TO_Y(x,unitx,unity) ((x)*(unitx)/(unity))

#define SIM_FALL  'D'
#define SIM_RISE  'U'

#define SIM_RAMP 'r'
#define SIM_TANH 't'

#define SIM_MIN 'm'
#define SIM_MAX 'M'
#define SIM_ALL 'A'

#define SIM_RUN_DRIVE 1
#define SIM_RUN_EXEC  2
#define SIM_RUN_READ  4
#define SIM_RUN_MULTI 8
#define SIM_RUN_ALL  (SIM_RUN_DRIVE|SIM_RUN_READ|SIM_RUN_EXEC)

#define SIM_EXT_CAPA 0xfab11105
/* Definition of stuck                                                        */
typedef struct {
  char          VALUE;      // SIM_ZERO, SIM_ONE
} sim_stuck_level;

typedef struct {
  SIM_FLOAT         VALUE;
} sim_stuck_voltage;

typedef union {
  sim_stuck_voltage STUCK_VOLTAGE;
  sim_stuck_level   STUCK_LEVEL;
} sim_ustuck;

typedef struct {
  char          TYPE;        // SIM_STUCK_LEVEL, SIM_STUCK_VALUE
  sim_ustuck    MODEL;
} sim_stuck;

/* Definition of slope                                                        */
typedef struct {
  char          TRANSITION;  // SIM_RISE, SIM_FALL
  SIM_FLOAT     TRISE;
  SIM_FLOAT     TSTART;
} sim_slope_single;

typedef struct {
  SIM_FLOAT     TRISE;
  SIM_FLOAT     TFALL;
  SIM_FLOAT     PERIOD;
  char         *PATTERN;
} sim_slope_pattern;

typedef union {
  sim_slope_single  SLOPE_SINGLE;
  sim_slope_pattern SLOPE_PATTERN;
} sim_uslope;

typedef struct {
  char          TYPE;    // SIM_SLOPE_SINGLE, SIM_SLOPE_PATTERN
  sim_uslope    MODEL;
} sim_slope;

typedef struct {
  SIM_FLOAT   (*FUNC)(SIM_FLOAT t, void *data);
  void        *USER_DATA;
} sim_func;

typedef struct {
  char *REF_VSSNODE;
  char *REF_VDDNODE;
  char *REF_NODE;
  int revert;
} sim_mimic_voltage;

/* Definition of input parameter                                              */
typedef union {
  sim_slope    INPUT_SLOPE;
  sim_stuck    INPUT_STUCK;
  sim_func     INPUT_FUNC;
  sim_mimic_voltage INPUT_MIMIC;
} sim_uinput;

/* Input connector definition                                                 */
typedef struct sim_input {
  struct sim_input     *NEXT;
  char                 *LOCON_NAME;
  char                 *LOSIG_NAME;
  char                  TYPE;  // SIM_STUCK, SIM_SLOPE, SIM_FUNC, SIM_LOAD
  sim_uinput            UINPUT;
  char                 *DELTAVAR;
  SIM_FLOAT VDD, VSS;
} sim_input ;

/* Measure                                                                    */
/* Important : due to hierarchical representation of name, there is no
   namealloc() allocation for name.                                           */

typedef struct sim_measure_detail {
  struct sim_measure_detail  *NEXT;
  char                       *NODE_NAME;  // nom original du noeud
  char                       *PRINT_NAME; // nom drive (alias du nom original)
  SIM_FLOAT                  *DATA;
} sim_measure_detail;

typedef union sim_measure_where {
  char *LOCON_NAME;
  char *SIGNAL_NAME;
} sim_measure_uwhere;

typedef struct sim_measure {
  struct sim_measure   *NEXT;
  char                  TYPE; // SIM_MEASURE_SIGNAL_NODE,SIM_MEASURE_SIGNAL, SIM_MEASURE_LOCON
  char                  WHAT; // SIM_MEASURE_VOLTAGE, SIM_MEASURE_CURRENT
  sim_measure_uwhere    WHERE;
  sim_measure_detail   *DETAIL;
  chain_list           *NODENAME; 
} sim_measure;

/* Initial condition                                                          */

typedef struct sim_ic_level {
  char  LEVEL;  // SIM_ZERO, SIM_ONE
} sim_ic_level;

typedef struct sim_ic_voltage {
  SIM_FLOAT VOLTAGE;
} sim_ic_voltage;

typedef union sim_uic {
  sim_ic_level           LEVEL;
  sim_ic_voltage         VOLTAGE;
} sim_uic;

typedef union sim_ic_where {
  char *LOCON_NAME;
  char *SIGNAL_NAME;
} sim_ic_uwhere;

typedef struct sim_ic {
  struct sim_ic         *NEXT;
  sim_ic_uwhere          WHERE;
  char                   LOCATE; // SIM_IC_LOCON, SIM_IC_SIGNAL
  char                   TYPE;   // SIM_IC_LEVEL, SIM_IC_VOLTAGE.
  sim_uic                UIC;
  chain_list            *NODENAME;
  SIM_FLOAT VDD, VSS;
} sim_ic;

/* Simulator configuration                                                    */
typedef struct {
  SIM_FLOAT     ALIM_VOLTAGE_IN;
  SIM_FLOAT     VSS_VOLTAGE_IN;
  SIM_FLOAT     ALIM_VOLTAGE_OUT;
  SIM_FLOAT     VSS_VOLTAGE_OUT;
  SIM_FLOAT     SLOPE;
  SIM_FLOAT     TEMP;
  SIM_FLOAT     VTHSTART, VTHEND;
  SIM_FLOAT     VTH_HIGH;
  SIM_FLOAT     VTH_LOW;
  SIM_FLOAT     TRANSIANT_TMAX;
  SIM_FLOAT     TRANSIANT_STEP;
  SIM_FLOAT     SIMULATOR_STEP;
  char         *TOOL_CMD;
  char         *TOOL_OUTFILE;
  char         *TOOL_STDOUTFILE;
  int           TOOL;
  chain_list   *TECHNOFILELIST;
  char          DRIVE_NETLIST; // SIM_YES, SIM_NO.
  char          OVR_FILES;     // SIM_YES, SIM_NO.
  char          REMOVE_FILES;  // SIM_YES, SIM_NO.
  char          NOISE_TYPE;    // SIM_MIN, SIM_MAX.
  char          TRANSISTOR_AS_INSTANCE; // SIM_YES, SIM_NO.
  PWL_FUNCTION  PWL_FUNC;
} sim_parameter ;

typedef struct sim_translate
{
  struct sim_translate *NEXT;
  char *signal;
  char *equiv;
} sim_translate;


/* Main structure                                                             */
typedef struct {
  lofig_list           *FIG;  
  char                 *FIGNAME;
  sim_parameter         PARAMETER;
  sim_ic               *LIC;
  sim_input            *LINPUT;
  sim_measure          *LMEASURE;
  sim_translate        *TRANSLATION;
  ht                   *HTINPUT;
//  ht                   *CONTEXT_HT;
  char                  OUTPUT_READ;
  char                 *OUTPUT_FILE;
  long                  NUMBER;
  ptype_list           *USER;
  void                 *LRULES;
  void                 *LLABELS;
  void                 *LARGS;
  void                 *LPRN;
  char                 SUBCKTFILENAME[256];
  int                   NBMC;
} sim_model;

/* Results structure */

typedef struct sim_timing {
    struct sim_timing *NEXT;
    char              *ROOT4USR;  // noeud origine vu par l'utilisateur
    char              *NODE4USR;  // noeud destination vu par l'utilisateur
    char              *ROOT;      // noeud origine
    char              *NODE;      // noeud destination
    SIM_FLOAT          DELAY;     // delay entre root et node
    SIM_FLOAT          SLOPE;     // slope du noeud destinataire
    char               ROOTEVENT; // SIM_FALL ou SIM_RISE
    int                IDXREVENT; // INDEX of ROOT EVENT 
    char               NODEEVENT; // SIM_FALL ou SIM_RISE
    int                IDXNEVENT; // INDEX of NODE EVENT 
} sim_timing;


typedef struct sim_noise {
    struct sim_noise *NEXT;
    char             *NAME;       // nom du noeud vu par l'utilisateur
    char             *NODENAME;   // nom du noeud dans le circuit
    SIM_FLOAT         VTHNOISE;   // valeur du seuil de detection du bruit
    SIM_FLOAT        *TAB;        // tableau contenant toutes les tensions du noeud
    chain_list       *IDXPEAK;    // indice du bruit : DATA = int (indice du TAB)
    chain_list       *IDXC;       // indice de passage au seuil : DATA = int (indice du TAB)
} sim_noise;

typedef struct sim_slopes
{
  float time;
  char direction;
  float time_low, time_high;
} sim_slopes;

#define SIM_UNDEF ((SIM_FLOAT)3.14159e27)

