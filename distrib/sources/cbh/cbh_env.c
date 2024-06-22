/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_env.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Karim Dioury                                            */
/*                                                                          */
/****************************************************************************/

#include "cbh_util.h"
#include MSL_H
#include BVL_H
#include BGL_H
#include SPF_H
#include SPE_H
#include AVT_H
#include INF_H

void cbh_spiceload(filename)
char *filename ;
{
  avt_error("library", -1, AVT_INFO, "loading SPICE '¤2%s¤.'\n", filename);
  parsespice(filename) ;
}

void cbh_spfload(filename)
char *filename ;
{
  avt_error("library", -1, AVT_INFO, "loading DSPF  '¤2%s¤.'\n", filename);
  parsespf(filename) ;
}

void cbh_spefload(filename)
char *filename ;
{
  avt_error("library", -1, AVT_INFO, "loading SPEF  '¤2%s¤.'\n", filename);
  spef_quiet=1;
  parsespef(filename) ;
  spef_quiet=0;
}

void cbh_vhdlload(filename)
char *filename ;
{
  befig_list *befig ;

  avt_error("library", -1, AVT_INFO, "loading VHDL  '¤2%s¤.'\n", filename);

  parsevhdlnetlist(filename);
  befig = NULL ;
  cbh_befiglisttocbhlist(befig) ;
}

void cbh_verilogload(filename)
char *filename ;
{
  befig_list *befig ;

  avt_error("library", -1, AVT_INFO, "loading VRLOG '¤2%s¤.'\n", filename);

  parseverilognetlist(filename);
}

void cbh_infload(filename)
char *filename ;
{
  avt_error("library", -1, AVT_INFO, "loading INF   '¤2%s¤.'\n", filename);

  _infRead(".unknown.", filename, 0);
}

void cbhenv()
{
  static int doneenv=0;
  if (doneenv!=0) return;
  doneenv=1;
  readlibfile("inf",cbh_infload,0) ;
  readlibfile("vhdl",cbh_vhdlload,0) ;
  readlibfile("verilog",cbh_verilogload,0) ;
  readlibfile("spice",cbh_spiceload,0) ;
  readlibfile("spf",cbh_spfload,0) ;
  readlibfile("dspf",cbh_spfload,0) ;
  readlibfile("spef",cbh_spefload,0) ;
}

