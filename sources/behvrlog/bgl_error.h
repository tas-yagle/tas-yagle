/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : bgl_error.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void bgl_error(int code, char *str1, bgl_bcompcontext *context);
void bgl_warning(int code, char *str1);
void bgl_toolbug(int code, char *str1, char *str2, int nbr1, bgl_bcompcontext *context);
void bgl_message(int code, char *str1, int nmb1);

