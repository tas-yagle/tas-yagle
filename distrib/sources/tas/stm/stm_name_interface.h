/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_name_interface.h                                        */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef __STM_NAME_INTERFACE_H__
#define __STM_NAME_INTERFACE_H__

#include STM_H

/* add */
extern timing_cell *stm_addcell (char *cellname) ;
extern timing_model *stm_addmodel (char *cellname, char *modelname);
extern timing_model *stm_addscmmodel (char *cellname, char *modelname);
extern timing_model *stm_addtblmodel (char *cellname, char *modelname, int nx, int ny, char xtype, char ytype);
extern timing_model *stm_addfctmodel (char *cellname, char *modelname, char *ins, char *localname, APICallFunc *cf);
extern void stm_addequacmodel (char *cell, char *model, float sdt, float sck, float fdt, float fck, float t);
extern void stm_addequamodel (char *cell, char *model, float r, float s, float c, float f, float t);
extern void stm_addconstmodel (char *cell, char *model, float value);
extern char *stm_storemodel (char *cellname, char *modelname, timing_model *model, int noshare);
extern void stm_addreducedcmodel (char *rcell, char *rmodel, char *cell, char *model, float slew, float ckslew, char redmode);
extern void stm_addreducedmodel (char *rcell, char *rmodel, char *cell, char *model, float slew, float load, char redmode, char modeltype);
extern void stm_addmergedcmodel (char *cellAB, char *modAB, char *cellA, char *dtmod, char *cellB, char *ckmod, float constr, float load, float ckload);
extern void stm_addmergedmodel (char *cellAB, char *modAB, char *cellA, char *modA, char *cellB, char *modB, float delay, float load, float slew);
extern void stm_dupmodelbyname (char *dupcell, char *dupname, char *cell, char *model);
extern char *stm_dupmodelbyname_and_updatedynamicmodelinfo(char *dupcell, char *dupname, char *cell, char *model, char *insname);
extern void stm_fctparse (timing_cell *cell);

/* free */
extern void stm_freecell (char *cellname) ;
extern void stm_freemodel (char *cellname, char *modelname);
extern void stm_freemodellist (char *cellname, chain_list *modellist);

/* get */
extern timing_cell *stm_getcell (char *cellname) ;
extern timing_cell *stm_getmemcell (char *cellname) ;
extern timing_model *stm_getmodel (char *cellname, char *modelname);

/* utils */
extern char *stm_getmodelname (char *cellname, char *modelname);
extern char *stm_generate_name (char *cell, char *input, char itr, char *output, char otr, unsigned int mtype, char minmax);
extern void stm_delreferences (char *cellname, char *modelname, int n);
extern void stm_addreferences (char *cellname, char *modelname, int n);
extern int stm_getreferences (char *cellname, char *modelname);
extern char* stm_createname( char *cellname, char *name );
extern int stm_renamecell( char *oldcellname, char *newcellname );


/* eval */
extern void stm_gettiming( char *cell, char *dmodel, char *fmodel, float fin, stm_pwl *pwlin, stm_driver *driver, float load, float *delay, float *fout, stm_pwl **pwlout, char *signame, char *inputname, char dir );
extern void stm_gettiming_pi( char *cell, char *dmodel, char *fmodel, float fin, stm_pwl *pwlin, stm_driver *driver, float c1, float c2, float r, float *delay, float *fout, stm_pwl **pwlout, char *signame, char *inputname, char dir );
extern float stm_getconstraint (char *cell, char *model, float inputslew, float clockslew);
extern float stm_getslew_pi (char *cell, char *model, float c1, float c2, float r, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame);
extern float stm_getdelay_pi (char *cell, char *model, float c1, float c2, float r, float slew, stm_pwl *pwl, char *signame);
extern float stm_getslew (char *cell, char *model, float load, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame);
extern float stm_getdelay (char *cell, char *model, float load, float slew, stm_pwl *pwl, char *signame);
extern float stm_getloadparam (char *cell, char *model, float load, float slew);
extern float stm_getclockslewparam (char *cell, char *model, float clockslew, float slew);
extern float stm_getdataslewparam (char *cell, char *model, float clockslew, float slew);
extern float stm_getslewparam (char *cell, char *model, float load, float slew);
extern float stm_getVTH (char *cell, char *model);
extern float stm_getVDD (char *cell, char *model);
extern float stm_getVT (char *cell, char *model);
extern float stm_getVF (char *cell, char *model);
extern void stm_addaxisvalues(ht *htslope, ht *htcapa, char *namein, char *nameout);
extern void stm_delaxisvalues(void);

#endif
