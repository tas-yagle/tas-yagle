/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_init.h                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

/* definitions des constantes */

#define STB_NODE_CLOCK     ((long) 0x10000001 )
#define STB_NODE_HOLD      ((long) 0x61000001 )
#define STB_NODE_SETUP     ((long) 0x61000002 )

/* variables globales */

extern long         STB_DEF_SLEW ;

/* declarations des fonctions */

chain_list     *stb_levelise __P((stbfig_list *stbfig));
void            stb_initnode __P((stbfig_list *stbfig));
void            stb_init __P((stbfig_list *stbfig));
int            stb_initckpath __P((stbfig_list *stbfig, ttvevent_list *event));
void            stb_initcmd __P((stbfig_list *stbfig, ttvevent_list *cmd, char active, char verif, char type, char typesig, int ctk));
void            stb_initclock __P((stbfig_list *stbfig, int ctk));
void            stb_checkclock __P((stbfig_list *stbfig)) ;

void stb_init_backward (stbfig_list *stbfig, ttvevent_list *event);
void stb_propagate_signal(stbfig_list *sb, chain_list *clocks);
