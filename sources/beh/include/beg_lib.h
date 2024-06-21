/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEG_GEN Version 1.00                                        */
/*    Fichier : beg_lib.h                                                   */
/*                                                                          */
/*    (c); copyright 2002 AVERTEC                                           */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s); :   Antony PINTO                                           */
/*                                                                          */
/****************************************************************************/

#ifndef BEH_LIB_H
#define BEH_LIB_H

#include MUT_H
#include BEH_H

#define BEG_PRE                (long) 0x00000002
#define BEG_REG                (long) 0x00000004
#define BEG_BUS                (long) 0x00000008
#define BEG_CMPLX              (long) 0x00000010

#define BEG_MASK_OUT           (long) 0x00000010
#define BEG_MASK_AUX           (long) 0x00000020
#define BEG_MASK_BUS           (long) 0x00000030
#define BEG_MASK_BUX           (long) 0x00000040
#define BEG_MASK_REG           (long) 0x00000050
#define BEG_MASK_POR           (long) 0x00000060


void 			 beg_def_befig			(char *name);
void             beg_def_por            (char *name, char direction);
void 			 beg_def_sig			(char *name, char *expr, unsigned int time);
void 			 beg_def_mux			(char *name, char *cndexpr, char *bitstr, char *valexpr, int time);
void 			 beg_def_process		(char *name, char *cndexpr, char *valexpr, int time, long flag);
void			 beg_def_loop			(char *name, char *cndexpr, char *valexpr, char *varname, int time, long flag);

void 			 beg_sav_befig	        (int tracemode);
chain_list 		*beg_str2Abl			(char *str);
befig_list      *beg_get_befig          ();
void             beg_freeBefig          ();

void             beg_eatFigure          (befig_list *eater,befig_list *eaten);

#endif
