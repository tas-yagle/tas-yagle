/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VCD Version 1.00                                            */
/*    Fichier : vcd_parse.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include VCD_H

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/


extern int       vcd_line;
extern t_vcd_db *vcd_db;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern long  vcd_getindex  (char *alias);
extern void  vcd_init_db   ();
extern void  vcd_addvar    (char *signal, char *alias);
extern void  vcd_set_ts    (int time_base, float time_unit);
extern int   vcd_gettime   (char *line);
extern char *vcd_getval    (char *line, long *alias);
extern char  vcd_getmodule (char *line);
extern void  vcd_parseline (char *line);
extern void  vcd_update    ();
extern void  vcd_parsefile (char *name, char *modulename);
extern void  vcd_free_db   ();
