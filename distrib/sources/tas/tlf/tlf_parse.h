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

#ifndef TLF_PARSE_H
#define TLF_PARSE_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include TLF_H

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/****************************************************************************/
/* GCF                                                                      */
/****************************************************************************/
#define CLOCK_DETECT       'C'
#define CLOCK_DUMP         'K'
#define DISABLE_DUMP       'D'
#define SPECIN_DUMP        'I'
#define SPECOUT_DUMP       'O'
#define VIRTUAL_CLOCK      'V'
#define REAL_CLOCK         'R'
#define DC                  FLT_MAX /* don't care */
#define GCF_SIZEHCK        100 
#define GCF_MAX_CONSTRAINT 1000000 
#define GCF_MIN_CONSTRAINT (-1000000) 

/****************************************************************************/
/* TLF                                                                      */
/****************************************************************************/
#define ATTRIBUTE_HT_SIZE 128
#define TLF_NO_CAPA   0
#define TLF_CONST 0
#define TLF_1D    1
#define TLF_2D    2
#define TLF_NODIM 3

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

extern int         TLF_LINE ;
extern char       *TLF_TOOLNAME ;                   /* default is "tlf2ttv" */
extern char       *TLF_TOOLVERSION ;                /* default is "1.0"     */
extern char       *TLF_TECHNOVERSION ;              /* default is "0.0"     */
extern chain_list *TLF_TTVFIG_LIST ;
extern char        TLF_TIME_UNIT ;
extern char        TLF_CAPA_UNIT ;
extern char        TLF_RES_UNIT ;

extern int         GCF_LINE ;
extern char        GCF_MODE ;
extern FILE       *GCF_OUT ;
extern eqt_ctx    *tlfEqtCtx ;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void        parsetlf3          (char *tlf_file) ;
extern void        parsetlf4          (char *tlf_file) ;
extern void        tlf_parse          (char *tlf_file, int version) ;
extern chain_list *tlf_load           (char *filename, int version) ;
extern void        tlf_stb2gcf        (char *gcf_file) ;
extern long        ftol               (float f) ;
extern long        ftolstretch        (float f) ;
extern int         is_attribute       (char *ident) ;
extern void        set_attribute      (char *attribute, char target) ;
extern void        callback_attribute (char *attribute, char *arg) ;
extern int         isclockpin         (chain_list *isclockpinchain, char *pname) ;
extern char        getoutputtype      (char *pname, chain_list *flipflops, chain_list *latchs) ;
extern void        tlf3reset          (void) ;
extern void        tlf4reset          (void) ;

#endif
