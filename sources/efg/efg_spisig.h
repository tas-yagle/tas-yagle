/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : efg_spisig.h                                                */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/* Declaration des defines                                                  */
/****************************************************************************/

/****************************************************************************/
/* Declaration de fonction                                                  */
/****************************************************************************/

/* fonctions de la nouvelle structure spisig */
extern void           efg_update_spisigname (spisig_list *spisig);
extern spisig_list   *efg_GetSpiSigByName (spisig_list *head,char *name);
extern void           efg_UpdateSpiSigName (spisig_list *spisig, char *name);
extern void           efg_PrintSpiSigList   ( spisig_list*);
extern char          *efg_GetSpiSigName     ( spisig_list*);
extern long           efg_GetSpiSigEvent    ( spisig_list*);
extern int            efg_IsWireOnSpiSig  (spisig_list *spisig);
extern void           efg_SetSpiSigLoconRc  (spisig_list *spisig,char *name,char dir,lotrs_list *mytrs);
extern char          *efg_GetSpiSigLoconRc  (spisig_list *spisig,char dir);
extern void           efg_SetSpiSigNodeRc  (spisig_list *spisig,int index);
extern chain_list    *efg_GetSpiSigNodeRc  (spisig_list *spisig);
extern void           efg_FreeSpiSigNodeRc  (spisig_list *spisig);
extern void           efg_SetDestSig2SpiSig ( spisig_list*, losig_list *destsig);
extern losig_list    *efg_GetDestSigBySpiSig ( spisig_list*);
extern losig_list    *efg_GetSrcSigBySpiSig ( spisig_list*);
extern int            efg_GetSpiSigNum      ( spisig_list*);
extern spisig_list   *efg_AddSpiSig         ( spisig_list*, losig_list*, char*, int, long, int);
extern void           efg_FreeSpiSigList    ( spisig_list*);
extern spisig_list   *efg_GetSpiSig         ( spisig_list*,
                                              char*
                                            );
extern spisig_list   *efg_GetSpiSigByNum    ( spisig_list*,
                                              int
                                            );
extern void           efg_AddCkSpiSig       ( spisig_list*,
                                              char*,
                                              long ,
                                              long ,
                                              long
                                            );
extern void           efg_InitSigVolt       ( spisig_list*, char*, float );
//extern ptype_list    *efg_AddPathEvent      ( ptype_list *, char*, char, int );
extern ptype_list    *efg_CreatePathByInf   (list_list *infpathsig);
//extern spisig_list   *efg_BuildSpiSigList   ( lofig_list* , ptype_list*, spisig_list *, char, int *, int);
extern spisig_list   *efg_BuildSpiSigFromInf (lofig_list*,list_list*);
extern chain_list    *efg_GetSig2Print (spisig_list *headspisig);
extern spisig_list   *efg_GetFirstSpiSig (spisig_list *head);
extern spisig_list   *efg_GetLastSpiSig (spisig_list *head);
void efg_correct_rc_nodes_based_on_rcx(spisig_list *head);
char *efg_checkvalidnode(losig_list *ls, long num, int *haslonode);

