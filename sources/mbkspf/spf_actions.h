/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_actions.h                                               */
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

#include SPF_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern double spf_femto        () ;
extern double spf_pico         () ;
extern double spf_nano         () ;
extern double spf_micro        () ;
extern double spf_milli        () ;
extern double spf_kilo         () ; 

void spf_ResetLocalHTable();
void spf_NewNet(char *sig, float capa);
void spf_AddResistor(char *namesig0, char *namesig1, float resi);
void spf_AddCapacitance(char *namesig0, char *namesig1, float capa);
void spf_AddSubNode(char *sig);
void spf_AddConnector(char *sig);
void spf_AddInstanceConnector(char *allname, char *instname, char *signame);
void spf_SetGroundSignal(char *sig);
int spf_find_pending_node(char *name, losig_list **ls, long *nodenum);
void spf_finish_pending_capacitances();
void spf_associate_node(char *name, losig_list *ls, long nodenum);

