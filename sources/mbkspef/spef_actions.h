/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                           */
/*    Fichier : spef_actions.h                                              */
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
/*     functions                                                            */
/****************************************************************************/

extern losig_list *spef_NewNet(char *sig, float capa);
extern void spef_AddResistor(char *namesig0, char *namesig1, float resi);
extern chain_list *spef_AddCapacitance(chain_list *ctclist, char *namesig0, char *namesig1, float capa);
extern void spef_AddConnector(char *sig, int nodenum);
extern void spef_AddInstanceConnector(char *allname, char *instname, char *signame, int nodenum);
extern void spef_SetGroundSignal(char *sig);
extern void spef_setneg2posnode(lofig_list *Lofig);

