/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_drive.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef LIB_DRIVE_H
#define LIB_DRIVE_H

#include LIB_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BEF_H
#include BVL_H
#include CBH_H
#include TMA_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/


extern void lib_driveheader       (FILE*, char*, ttvinfo_list*);
extern void lib_drivelut1d        (FILE*, char, char*, char, float*, float*);
extern void lib_drivelut2d        (FILE*, char, char*, char, char, float*, float*, float**);
extern void lib_drivelut          (FILE*, timing_model*, char);
extern void lib_drivelutconst     (FILE*, float, char);
extern void lib_drivetemplate     (FILE*);
extern void lib_drivebustype      (FILE*, chain_list*);
extern int  lib_drive_rail_connection(FILE *f, ttvfig_list *ttvfig);
extern void lib_drive_power_supply_group(FILE*, chain_list*, ttvinfo_list*);
extern void lib_drivedelaymodel   (FILE*, ttvfig_list*, char*, char, float);
extern char *lib_get_timing_sense (ttvsig_list*, ttvsig_list*);
extern void lib_driveconstraintmodel   (FILE*, ttvfig_list*, char*, char);
extern void lib_driveslewmodel    (FILE*, ttvfig_list*, char*, char, float);
extern void lib_driveintrinsic    (FILE*, float, char);
extern void lib_drive_cst_constraint (FILE*, float, char);
extern void lib_driveslew         (FILE*, float, char);
extern void lib_drive             (chain_list*, chain_list*, char*, char*);
extern void lib_drivecell         (FILE*, ttvfig_list*, befig_list*, char*);
extern void lib_drivepin          (FILE*, ttvfig_list*, ttvsig_list*, befig_list*, cbhseq*, int);
extern chain_list *lib_drivecombfunc     (FILE*, char*, befig_list*, cbhseq*);
extern void lib_driveseqfunc      (FILE*, cbhseq*);
extern void lib_drivecapacitance  (FILE*, ttvsig_list*);
extern void lib_drivedirection    (FILE*, ttvsig_list*, bepor_list*, chain_list*, int);
extern void lib_drivetiming       (FILE*, ttvfig_list*, ttvsig_list*, chain_list*, cbhseq*, befig_list*);
extern void lib_drivepower        (FILE*, ttvfig_list*, ttvsig_list*, chain_list*, befig_list*);
extern void lib_drivesetup_group  (FILE*, ttvfig_list*, ttvsig_list*, ttvevent_list*);
extern void lib_drivehold_group   (FILE*, ttvfig_list*, ttvsig_list*, ttvevent_list*);
extern void lib_driveaccess_group (FILE*, ttvfig_list*, ttvsig_list*, ttvevent_list*, cbhseq*, char);
extern void lib_drivesetup        (FILE*, ttvfig_list*, ttvline_list*);
extern void lib_drivehold         (FILE*, ttvfig_list*, ttvline_list*);
extern void lib_drivedelaymax     (FILE*, ttvfig_list*, ttvline_list*);
extern void lib_drivedelaymin     (FILE*, ttvfig_list*, ttvline_list*);
extern chain_list *addevent       (chain_list*, ttvevent_list*);
extern char *lib_unsuffix         (char *name, char* suffix);
extern void space                 (FILE*, int);
extern void tab                   (FILE*);
extern void saut                  (FILE *, int);
extern chain_list *addname        (chain_list*, char*);
extern char *pfloat               (char*, float);
extern char *ppower               (char*, float);
extern void lib_driveopcond (FILE *f, char *opcondname, double process, double temp,
                             double volt) ;
extern double lib_derate_tension(double V0, double V1, int transition) ;
extern double lib_derate_temp(double T0, double T1, int transition) ;
void lib_drive_remove_null( chain_list*, chain_list*, chain_list**, chain_list** );
extern char *lib_get_tg_pinname(char *str);

#endif
