/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : TLF Version 1.00                                            */
/*    Fichier : tlf_parse.h                                                 */
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

extern void  tlf3_drive            (chain_list*, chain_list*, char*, char*) ;
extern char *tlf3_unsuffix         (char*, char*) ;
extern void  tlf3_driveheader      (FILE*, char*, ttvinfo_list*) ;
extern void  tlf3_drivemodels      (FILE*, timing_model*, timing_model*, char) ;
extern void  tlf3_drivemodel       (FILE *f, timing_model *model, char);
extern void  tlf3_drivetemplate    (FILE*) ;
extern void  tlf3_drivecell        (FILE*, ttvfig_list*, befig_list*, char*) ;
extern void  tlf3_drivepin         (FILE*, ttvfig_list*, ttvsig_list*, befig_list*, cbhseq*) ;
extern void  tlf3_drivecapacitance (FILE*, ttvfig_list*, ttvsig_list*) ;
extern void  tlf3_drivedirection   (FILE*, ttvsig_list*, bepor_list*) ;
extern void  tlf3_drivepath        (FILE*, ttvline_list*) ;
extern void  tlf3_driveinsert      (FILE*, char*, ttvfig_list*, ttvline_list*) ; 
extern void  tlf3_driveinsertion   (FILE*, ttvfig_list*) ; 
extern void  tlf3_driveseqfunc     (FILE*, cbhseq*);
extern void  tlf3_drivecombfunc    (FILE*, char*, befig_list*, cbhseq*);
#endif
