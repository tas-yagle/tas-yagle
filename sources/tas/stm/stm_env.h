/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_env.h                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_ENV_H
#define STM_ENV_H

/*****************************************************************************/
/*     defines                                                               */            
/*****************************************************************************/
#define STM_NBCELLS          16 
#define STM_NBMODELS         16 
#define STM_NBTEMPLATES      16 
                             
/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

extern chain_list *STM_CELLS_CHAIN;
extern chain_list *STM_TEMPLATES_CHAIN;
extern chain_list *STM_TEMPLATE_NAME;

extern ht *STM_CELLS_HT;
extern ht *STM_TEMPLATES_HT;

extern char *STM_IN;
extern char *STM_OUT;
 
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
extern float STM_DEF_SLEW;
extern float STM_DEF_LOAD;
extern float STM_DEFAULT_RLIN;
extern float STM_DEFAULT_VSAT;

extern int STM_DEBUG;
extern int STM_SHARE_MODEL;
extern int STM_SOLVER_PILOAD;
extern int STM_IMAX_FOR_PILOAD;
extern float STM_OVERSHOOT;

extern int STM_BUFSIZE;

extern char *STM_TECHNONAME;

extern unsigned long int STM_CACHE_SIZE;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void stm_exit ();
extern void stmenv ();
extern void stm_init ();

#endif
