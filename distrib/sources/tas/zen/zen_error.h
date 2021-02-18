/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : zen                                                         */
/*    Fichier : zen_error.h                                                 */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef ZEN_ERROR_H
#define ZEN_ERROR_H

extern  void            zen_error       ( int            Error          ,
                                          char          *String         ,
                                          char          *File           ,
                                          long           Line           );
extern  void            zen_warning     ( int            Warning        ,
                                          char          *String         ,
                                          char          *File           ,
                                          long           Line           );
extern  void            zen_trace       ( int            Trace          ,
                                          char          *String         ,
                                          char          *File           ,
                                          long           Line           );
extern  void            zen_test        ( int            test           ,
                                          char          *string         ,
                                          char          *file           ,
                                          long           line           );

#endif
