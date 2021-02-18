/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_parse.c                                                 */
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

#include LIB_H
#include "lib_parse.h" 

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern int   libparse() ;
extern void  librestart();
extern FILE *libin ;
eqt_ctx     *libEqtCtx;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void lib_parse_this_file (char *filename, char *ext)
{
 static char viergelib = 1 ;
 float slope = STM_DEF_SLEW;
 float load = STM_DEF_LOAD;
 
 if ((libin = mbkfopen (filename, ext, "r"))) {
     if(viergelib == 0) {
         librestart(libin) ;
         LIB_LINE = 1;
     }
     viergelib = 0 ;
     libEqtCtx = eqt_init (EQT_NB_VARS);
     if(V_FLOAT_TAB[__SIM_INPUT_SLOPE].SET)
         STM_DEF_SLEW = V_FLOAT_TAB[__SIM_INPUT_SLOPE].VALUE * 1e12;
     if(V_FLOAT_TAB[__SIM_OUT_CAPA_VAL].SET)
         STM_DEF_LOAD = V_FLOAT_TAB[__SIM_OUT_CAPA_VAL].VALUE * 1e15;
     libparse () ;
     STM_DEF_SLEW = slope;
     STM_DEF_LOAD = load;
     eqt_term (libEqtCtx);
 	fclose (libin) ; 
    stm_exit();
 }
 else { 
     fprintf (stderr, "LIB ERROR : no file %s\n", filename);
     EXIT (EXIT_FAILURE) ;
 }

}

void lib_parse (char *filename)
{
  lib_parse_this_file (filename, "lib");
}

chain_list *lib_load (char *filename)
{
    lib_parse(filename);
    return LIB_TTVFIG_LIST;

}

