/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_newdisp_util.h                                         */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Marc KUOCH                                                */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
#define XTAS_MIN_CARLENGTH  13
#define XTAS_MAX_CARLENGTH 150

extern int  XTAS_DISPLAY_NEW ;
extern char XTAS_DISABLE_INFO ;

extern char *XtasPlaceNbStringTTVUnit        __P((
                                                  long num,
                                                  float unit
                                                ));
extern char *XtasPlaceNbString               __P(( 
                                                  long 
                                                )) ;

extern char *XtasPlaceFloatString            __P((
                                                  float 
                                                )) ;

extern char *XtasPlaceString                 __P((
                                                  char*, 
                                                  int
                                                )) ;

extern void XtasFillInfosForm                __P(( 
                                                   Widget,
                                                   char*
                                                )) ;
