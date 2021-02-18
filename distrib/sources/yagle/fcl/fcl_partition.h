/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_partition.h                                             */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

typedef int fcl_label;

typedef struct partition {
     struct partition *NEXT;
     char              TYPE;    /* soit T (transistor), soit S (signal) */
     fcl_label         LABEL;
     int               NBELEM;
     void             *DATA;
 } partition_list;     

extern partition_list *addpartition();
extern partition_list *getpartition();
extern partition_list *delpartition();
extern partition_list *smallestpartition();
extern partition_list *comparepartition();
extern partition_list *removepartitionelement();

extern void printpartition();
extern void displaypartition();

