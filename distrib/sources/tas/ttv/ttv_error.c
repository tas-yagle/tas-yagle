/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_error.c                                                 */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fonction d'erreur de ttv                                                 */
/****************************************************************************/

#include "ttv.h"

/*****************************************************************************/
/*                        function ttv_error()                               */
/* parametres :                                                              */
/* errorindex : numero de l'erreur                                           */
/* message : message a ajouter au message d'erreur                           */
/* type : type d'erreur TTV_ERROR ou TTV_WARNING                             */
/*                                                                           */
/* renvoie un message d'erreur et sort du programme ou un message            */
/* d'avertissement et continue l'execution du programme                      */
/* renvoie :                                                                 */
/* le numero de l'erreur correspondante                                      */
/*****************************************************************************/
int ttv_error(errorindex,message,type)
int errorindex ;
char *message ;
int type ;
{
  int avttype ;

  if( type == TTV_ERROR )
    avttype = AVT_FATAL ;
  else
    avttype = AVT_WARNING ;

  
 switch(errorindex)
  {
   case 10 : avt_errmsg( TTV_ERRMSG, "010", avttype, message );
             break ;
   case 11 : avt_errmsg( TTV_ERRMSG, "011", avttype, message );
             break ;
   case 20 : avt_errmsg( TTV_ERRMSG, "020", avttype, message );
             break ;
   case 21 : avt_errmsg( TTV_ERRMSG, "021", avttype, message );
             break ;
   case 22 : avt_errmsg( TTV_ERRMSG, "022", avttype, message );
             break ;
   case 30 : avt_errmsg( TTV_ERRMSG, "030", avttype, message );
             break ;
   case 31 : avt_errmsg( TTV_ERRMSG, "031", avttype, message );
             break ;
   case 40 : avt_errmsg( TTV_ERRMSG, "040", avttype, message );
             break ;
   case 41 : avt_errmsg( TTV_ERRMSG, "041", avttype, message );
             break ;
   case 42 : avt_errmsg( TTV_ERRMSG, "042", avttype, message );
             break ;
   case 43 : avt_errmsg( TTV_ERRMSG, "043", avttype, message );
             break ;
   case 44 : avt_errmsg( TTV_ERRMSG, "044", avttype, message );
             break ;
   case 45 : avt_errmsg( TTV_ERRMSG, "045", avttype, message );
             break ;
   case 46 : avt_errmsg( TTV_ERRMSG, "046", avttype, message );
             break ;
   case 50 : avt_errmsg( TTV_ERRMSG, "050", avttype, message );
             break ;
   case 51 : avt_errmsg( TTV_ERRMSG, "051", avttype, message );
             break ;
   case 52 : avt_errmsg( TTV_ERRMSG, "052", avttype, SEPAR, message );
             break ;
   case 53 : avt_errmsg( TTV_ERRMSG, "053", avttype, message );
             break ;
   case 54 : avt_errmsg( TTV_ERRMSG, "054", avttype, message );
             break ;
   case 55 : avt_errmsg( TTV_ERRMSG, "055", avttype, message );
             break ;
   case 56 : avt_errmsg( TTV_ERRMSG, "056", avttype, message );
             break ;
   default : break ;
  }
 
 if(type == TTV_ERROR)
 {
  EXIT(errorindex) ;
 }

 return(errorindex);
}

