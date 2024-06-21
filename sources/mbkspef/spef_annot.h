/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_annot.h                                                 */
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

#include SPE_H

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern lofig_list *Lofig ;
extern int         Line ;
extern int         Mode ;
extern char *spef_ParsedFile;
extern char        SPEF_PRELOAD;
extern int         SPEF_LOFIG_CACHE, SPEF_IN_CACHE;
extern int         nbloadelem;
extern int         pool;
/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void spef_cache_free( lofig_list *lofig );
extern unsigned long int spef_cache_parse( lofig_list *lofig, losig_list *losig );

//extern lofig_list *spf_Annotate(lofig_list *lofig, char *spf_file_name);
