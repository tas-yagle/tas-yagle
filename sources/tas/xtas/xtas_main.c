/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_main.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                       Date : 01/04/1993    */
/*                                                                          */
/*    Modified by :                                    Date : ../../....    */
/*    Modified by :                                    Date : ../../....    */
/*    Modified by :                                    Date : ../../....    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : main                                                           */
/*                                                                           */
/* IN  ARGS : .You can give some X resources in your argv. They will be      */
/*            passed to the X server in the XtVaAppInitialize call.          */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The main routine of the  Extase    program.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int main( argc, argv )
int  argc;
char *argv[];
{
    int     i;

    for (i=1 ; i<argc ; i++) {
         if (!strcmp (argv[i],  "-install"))
             XTAS_SWITCH_COLOR_MAP = 'Y';
    }

 XtasInitializeRessources( &argc, argv ); 

 XtAppMainLoop( XtasApplicationContext );
 XtasExit(0);
}
