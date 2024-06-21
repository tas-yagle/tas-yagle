/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : mgl_error.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void mgl_error(int code, char *str1, mgl_scompcontext *context);
void mgl_warning(int code, char *str1);
void mgl_toolbug(int code, char *str1, char *str2, int nbr1, mgl_scompcontext *context);
void mgl_message(int code, char *str1, int nmb1);

