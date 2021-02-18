/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : TLF Version 1.00                                            */
/*    Fichier : tlf4_parse.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef TLF_DRIVE_H
#define TLF_DRIVE_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include TLF_H
#include STM_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BEF_H
#include BVL_H
#include CBH_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void  tlf4_drive            (chain_list*, chain_list*, char*, char*) ;
extern char *tlf4_unsuffix         (char *name, char* suffix);
extern void  tlf4_driveheader      (FILE*, char*);
extern void  tlf4_driveproperty    (FILE*, ttvinfo_list*);
extern void  tlf4_drivemodels      (FILE*, timing_model*, timing_model*, char);
extern void  tlf4_drivemodel       (FILE *f, timing_model *model, char);
extern void  tlf4_drivetemplate    (FILE*);
extern void  tlf4_drivecell        (FILE*, ttvfig_list*, befig_list*, char*);
extern void  tlf4_drivepin         (FILE*, ttvsig_list*, befig_list*, cbhseq*, int);
extern void  tlf4_drivecapacitance (FILE*, ttvsig_list*, int);
extern void  tlf4_drivebusdirection (FILE *f, ttvsig_list *ttvsig, befig_list *ptcellbefig);
extern void  tlf4_drivedirection   (FILE*, ttvsig_list*, bepor_list*, int);
extern void  tlf4_drivepath        (FILE*, ttvline_list*, chain_list*);
extern void  tlf4_driveinsert      (FILE*, char*, ttvfig_list*, ttvline_list*) ; 
extern void  tlf4_driveinsertion   (FILE*, ttvfig_list*) ; 
extern void  tlf4_driveseqfunc     (FILE*, cbhseq*);
extern void  tlf4_drivecombfunc    (FILE*, char*, befig_list*, cbhseq*, int);
extern char *ptlf4long               (char*, long);
extern char *ptlf4float               (char*, float);

#endif
