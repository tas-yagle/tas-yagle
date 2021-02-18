/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_extendmsl.h                                             */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define NUMNETTYPES 16
#define NUMTRANSTYPES 10

#define FCL_TRANSTYPE 2
#define FCL_NETTYPE   4
#define FCL_NETOUTPUT 5
#define FCL_CMPUP     8
#define FCL_CMPDN     9
#define FCL_MUXUP     10
#define FCL_MUXDN     11

extern char *nettype_names[NUMNETTYPES];
extern long  nettype_values[NUMNETTYPES];
extern char *transtype_names[NUMTRANSTYPES];
extern long  transtype_values[NUMTRANSTYPES];

typedef struct fcltype {
    struct fcltype *NEXT;
    char           *SUBCKT;
    char           *NAME;
    int             MARKTYPE;
    long            TYPE;
} fcltype_list;

