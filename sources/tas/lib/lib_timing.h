/******************************************/
/* lib_timing.h                           */
/******************************************/

/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#ifndef LIB_TIMING_H
#define LIB_TIMING_H

#include "lib_global.h"

// zinaps: valeur a redifinir car surement fausses par defaut
#define LIB_DEFAULT_SA_SLEW_UP_FALL_UNIT    0.8
#define LIB_DEFAULT_SA_SLEW_LOW_FALL_UNIT   0.2
#define LIB_DEFAULT_SA_SLEW_UP_RISE_UNIT    0.8
#define LIB_DEFAULT_SA_SLEW_LOW_RISE_UNIT   0.2
#define LIB_DEFAULT_SA_INPUT_DTH_RISE       0.5
#define LIB_DEFAULT_SA_INPUT_DTH_FALL       0.5
#define LIB_DEFAULT_SA_OUTPUT_DTH_RISE      0.5
#define LIB_DEFAULT_SA_OUTPUT_DTH_FALL      0.5
#define LIB_DEFAULT_SA_NOM_VOLTAGE          5
#define LIB_DEFAULT_SA_NOM_TEMP             70
#define LIB_DEFAULT_SA_SLOPE_RISE           0
#define LIB_DEFAULT_SA_SLOPE_FALL           0

/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/
extern  chain_list      *LIB_TTVFIG_LIST;

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
extern  char             lib_tim_get_axe_type(lib_group *g, long attr_type);
extern  chain_list      *lib_tim_get_axe_values(lib_group *g, long attr_type);
extern  void             lib_tim_store_template(void);
extern  void             lib_tim_store_wire_load(void);
extern  void             lib_tim_store_wire_load_table(void);
extern  void             lib_tim_store_default_wire_load(void);
extern  char             lib_tim_give_sense(lib_group *g);
extern  char             lib_tim_give_timing_type(lib_group *g);
extern  char            *lib_tim_give_mname_with_number(char *cellname, char *prefixe, char *namebuf, char *suffixe);
extern  char            *lib_tim_give_modelname_group(char *cellname, long gtype, char *namebuf, char tim_type, int itype, int otype);
extern  chain_list      *lib_tim_give_tbl_values(chain_list *val);
extern  timing_model    *lib_tim_store_model_group(char *cname, char *pname, lib_group *g, char ttype, int itype, int otype, char *relpin);
extern  char            *lib_tim_get_str_type(long type);
extern  void             lib_tim_treat_pin(lib_group *g, ttvfig_list *f, char *cellname, ht *pinht, ht *pincapaht);
extern  void             lib_tim_treat_cell(lib_group *g, lofig_list *plofig);
extern  chain_list      *lib_tim_addrefsig(lib_group *g, ttvfig_list *fig, ht *pht, ht *pcapht);
extern  long             lib_tim_get_long_dir(char dir, char *ts);
extern  void             lib_tim_addcaracline(lib_group *g, ttvfig_list *fig, char *cellname, char *pinname, ht *pht, ht *pcapht);
extern  void             lib_tim_addline_const(lib_group *g, ttvfig_list *fig, ht *pinht, long linetype, char tim_type, char sense, char *pinname, char *relpin);
extern  int              lib_tim_isckpin (chain_list *ckchain, char *pname);
extern  void             lib_tim_getsigtype(ttvfig_list *fig, lofig_list *pfig);
extern  void             lib_tim_allocbreaklatch(ttvfig_list *fig);
extern  void             lib_tim_treat_power_supply(void);

#endif
