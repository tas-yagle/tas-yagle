/******************************************/
/* lib_func.h                             */
/******************************************/

/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#ifndef LIB_FUNC_H
#define LIB_FUNC_H

#include "lib_global.h"

/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/
/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
extern  lofig_list  *lib_func_treat_cell(lib_group *g);
extern  cbhseq      *lib_func_create_cbhseq(lib_group *g);
extern  cbhcomb     *lib_func_create_cbhcomb(char *cname, char *pname, lofig_list *plofig, lib_group *g, cbhseq *pseq, char *m);
extern  void         lib_func_treat_pin(char *cellname, lib_group *g, lofig_list *plofig, cbhseq *pseq, char *m);
extern  locon_list  *lib_func_complete_locon(char *pname, lofig_list *plofig, char dir);
extern  void         lib_func_multout(char *cellname, lofig_list *p, cbhseq *pseq, cbhcomb *pcomb);
extern  void         lib_func_complete_pin (char *cellname, char *pinname, lofig_list *plofig, lib_group *p, cbhseq *pseq, char *m, char *dir);

#endif


