/**************************/
/*     eqt_lib.h          */
/**************************/


/*****************************************************************************/
/*     includes                                                              */
/*****************************************************************************/

#ifndef EQT
#define EQT

#define EQT_PRECISION 1e6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include MUT_H
#include LOG_H

/*****************************************************************************/
/*     defines                                                               */
/*****************************************************************************/

/* arithmetic operators */
#define EQTPLUS    ((long)0x1)
#define EQTMIN     ((long)0x2)
#define EQTMULT    ((long)0x3)
#define EQTDIV     ((long)0x4)
#define EQTEXP     ((long)0x5)
#define EQTSQRT    ((long)0x6)
#define EQTPOW     ((long)0x7)
#define EQTLOG     ((long)0x8)
#define EQTINV     ((long)0x9)
#define EQTUSER    ((long)0x19)

#define EQTSIN     ((long)0xb)
#define EQTCOS     ((long)0xc)
#define EQTTAN     ((long)0xd)
#define EQTASIN    ((long)0xe)
#define EQTACOS    ((long)0xf)
#define EQTATAN    ((long)0x10)
#define EQTATAN2   ((long)0x11)
#define EQTSINH    ((long)0x12)
#define EQTCOSH    ((long)0x13)
#define EQTTANH    ((long)0x14)
#define EQTLOG10   ((long)0x15)
#define EQTCEIL    ((long)0x16)
#define EQTFLOOR   ((long)0x17)
#define EQTFABS    ((long)0x18)
#define EQTABS     ((long)0xf9)

/* logic operators */
#define EQTOR      ((long)0x20)
#define EQTAND     ((long)0x21)
#define EQTXOR     ((long)0x22)
#define EQTNOT     ((long)0x23)
#define EQTEQ      ((long)0x24)
#define EQTINFEQ   ((long)0x25)
#define EQTSUPEQ   ((long)0x26)
#define EQTINF     ((long)0x27)
#define EQTSUP     ((long)0x28)
#define EQTIF      ((long)0x29)
#define EQTNOTEQ   ((long)0x2a)

#define EQTNOMMOD  ((long)0x30)
#define EQTNOMDEV  ((long)0x31)
#define EQTNOMSUB  ((long)0x32)
#define EQTSPECFUNC_BASE EQTNOMMOD

#define EQTSPECFUNC_NAME(x) (x==EQTNOMMOD?"nom_mod":x==EQTNOMDEV?"nom_dev":"unknown")

/* types of node */
#define EQTUNOP_T     ((long)0x1)  
#define EQTBINOP_T    ((long)0x2)  
#define EQTTERNOP_T   ((long)0x4)  
#define EQTVARNAME_T  ((long)0x8)  
#define EQTVALUE_T    ((long)0x10) 
#define EQTDOTFUNC_T  ((long)0x20)  

/* for H TABLE */
#define EQT_NB_VARS   150
#define EQT_NO_VALUE  LONG_MAX

/* for type of resolve */
#define EQTFAST   1
#define EQTNORMAL 0

/* node reduce flag */
#define EQT_COMPLETE   1
#define EQT_INCOMPLETE 0

#define EQTMAXNFUNC 128 
#define EQTNBSPECFUNC 3

#define EQTSPECFUNC_NOM_MOD 0
#define EQTSPECFUNC_NOM_DEV 1
#define EQTSPECFUNC_NOM_SUB 2

/*****************************************************************************/
/*     structures                                                            */
/*****************************************************************************/

typedef struct eqt_unary {
	long               OPERATOR;
	struct eqt_node   *OPERAND;
} eqt_unary;

typedef struct eqt_ternary {
	long               OPERATOR;
	struct eqt_node   *OPERAND1;
	struct eqt_node   *OPERAND2;
	struct eqt_node   *OPERAND3;
} eqt_ternary;

typedef struct eqt_binary {
	long               OPERATOR;
	struct eqt_node   *OPERAND1;
	struct eqt_node   *OPERAND2;
} eqt_binary;

typedef struct eqt_func {
	char *FUNCNAME;
    chain_list *ARGS;
} eqt_func;

typedef union eqt_unode {
	struct eqt_unary   *UNOP  ;
	struct eqt_binary  *BINOP ;
	struct eqt_ternary *TERNOP;
    eqt_func           *FUNCOP;
	char               *VARNAME;
	double              VALUE;
} eqt_unode;

typedef struct eqt_node {
  struct eqt_node     *FATHER;
  union eqt_unode      UNODE; /* operateur unaire, 
                                  operateur binaire, 
                                  nom de variable,
                                  valeur */
  long                 TYPE;
  int REF_COUNT;
} eqt_node;
	
typedef struct {
  char *FUNCNAME;
  chain_list *ARGS;
  char *EXPR;
  eqt_node *NODE_EXPR;
} eqt_func_entry;

typedef struct eqt_ctx {
  ht                  *VAR_HT;
  ht                  *UNIT_HT;
  int                  USE_SPICE_UNITS;
  struct eqt_node     *EQUA_NODE;
  char                *EQUA_STR;
  double               EQUA_VAL;
  int                  EQT_VAR_INVOLVED;
  int                  EQT_MODE_CHOICE;
  int                  EQTINDEX;
  char                *NAMETAB[EQTMAXNFUNC];
  double               (*FUNCTAB[EQTMAXNFUNC]) (double);
  double               (*FUNCTAB2[EQTMAXNFUNC]) (double, double);
  double               (*FUNCTAB3[EQTMAXNFUNC]) (double, double, double);
  double               (*SPECIAL_FUNC[EQTNBSPECFUNC])(char *);
  chain_list          *FUNCTIONS;
  char                *FAULTY_VAR;
  char                 EQT_RES_CALC;  
  char                 EQT_GOT_RANDOM;  
} eqt_ctx;

typedef struct 
{
  char *NAME;
  double VAL;
} eqt_biinfo;

typedef struct eqt_param {
    eqt_biinfo *EBI;
    int    NUMBER;
} eqt_param;

extern eqt_ctx *EQT_CONTEXT_HIERARCHY[5];

/*****************************************************************************/
/*     functions                                                             */
/*****************************************************************************/

extern int          eqt_var_involved  (eqt_ctx *ctx);
extern double        eqt_eval         (eqt_ctx *ctx, char *equa, int choix);
extern double        eqt_calcval      (eqt_ctx *ctx, eqt_node *node);
extern double        eqt_calcval_rec  (eqt_ctx *ctx, eqt_node *node);
extern void          eqt_reduce       (eqt_ctx *ctx, eqt_node *node);
extern eqt_node     *eqt_create       (eqt_ctx *ctx, char *name);
extern void          eqt_freenode     (eqt_node *node);
extern eqt_ctx      *eqt_init         (int nbvars);
extern eqt_ctx      *eqt_dupctx       (eqt_ctx *ctx);
extern void          eqt_display_vars (eqt_param *params);
extern void          eqt_import_vars  (eqt_ctx *to, eqt_param *from);
extern eqt_param    *eqt_export_vars  (eqt_ctx *ctx);
extern void          eqt_free_param   (eqt_param *param);
extern void          eqt_term         (eqt_ctx *ctx);
extern void          eqt_addvar       (eqt_ctx *ctx, char *var_name, double value);
extern double        eqt_getvar       (eqt_ctx *ctx, char *var_name);
extern void          eqt_addunit      (eqt_ctx *ctx, char *unit_name, double factor);
extern double        eqt_getunit      (eqt_ctx *ctx, char *unit_name);
extern void          eqt_addfunction  (eqt_ctx *ctx, char *name, double (*function) (double));
extern void          eqt_addfunction2 (eqt_ctx *ctx, char *name, double (*function) (double, double));
extern void          eqt_addfunction3 (eqt_ctx *ctx, char *name, double (*function) (double, double, double));
int eqt_adddotfunc (eqt_ctx *ctx, char *name, chain_list *args, char *expr);
extern int           eqt_resistrue    (eqt_ctx *ctx);

extern void	         eqt_print              (eqt_ctx *ctx, eqt_node *node);
extern chain_list   *eqt_NodeToAbl          (eqt_node *node);
extern chain_list   *eqt_StrToAbl           (eqt_ctx *ctx, char *str);
extern char         *eqt_ConvertStr         (char *str);
extern char         *eqt_getSimpleEquation  (char *equation);
extern char         *eqt_getEvaluedEquation (eqt_ctx *ctx, char *equation);
extern chain_list *eqt_GetVariables(eqt_ctx *ctx, char *equa, int all);

eqt_node *eqt_adddotfuncnode (char *name, chain_list *args);
double eqt_execdotfunc(eqt_ctx *ctx, char *funcname, chain_list *args, int quick);

chain_list *eqt_export_func(chain_list *old, eqt_ctx *ctx);
void eqt_import_func(chain_list *old, eqt_ctx *ctx);
void eqt_add_spice_extension(eqt_ctx *ctx);
int eqt_isdefined(eqt_ctx *ctx, char *varname, int global);
eqt_param *eqt_dupvars (eqt_param *sourceparam);
void eqt_srand(unsigned int seed);
void eqt_setspecialfunc(eqt_ctx *ctx, int index, double (*func)(char *var));
double eqt_getvar_in_context_only (eqt_ctx *ctx, char *var_name);
unsigned int eqt_get_current_srand_seed();
int eqt_resisrandom (eqt_ctx *ctx);
void eqt_set_failedres (eqt_ctx *ctx);
void eqt_initseed(int mode, unsigned int value);
unsigned int eqt_getinitseed();
void eqt_resetvars(eqt_ctx *ctx, int nbvars);
char *eqt_lookup_expr(char *expr);

#endif
