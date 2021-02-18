/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : gsp_get_patterns.h                                          */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* Obtentions des patterns pour la sensibilisation d'un chemin sous spice   */
/****************************************************************************/

/****************************************************************************/
/* Define                                                                   */
/****************************************************************************/

/****************************************************************************/
/* variable                                                                 */
/****************************************************************************/

/****************************************************************************/
/* Fonction                                                                 */
/****************************************************************************/
extern ptype_list    *gsp_verif_pat (cnsfig_list *cf, lofig_list *fig,ptype_list *patterns);
extern char           gsp_is_pat_indpd (ptype_list *patterns);
extern int            gsp_FindSigSlope      ( losig_list* );
extern char          *gsp_FindState         ( lofig_list*,
                                              char* , 
                                              ptype_list*);
extern void           gsp_traversecones     ( cone_list*, int  );
extern chain_list    *gsp_numbercones       ( chain_list* );
extern pCircuit       gsp_buildspicecct     ( chain_list* );
extern pCircuit       gsp_buildconecct      ( cone_list*  );
extern pCircuit       gsp_buildexpconecct   (pCircuit ptcct, chain_list *conelist);
extern pNode          gsp_SetConstraints    ( pCircuit ,
                                              chain_list* ,
                                              chain_list* ,
                                              pNode ,
                                              pNode ,
                                              char ,
                                              char,pNode *,pNode *
                                            );
extern ptype_list    *gsp_spisetinputs_from_cone ( char*, chain_list*, char, chain_list **); 
extern int            gsp_con_is_constraint (ptype_list *constraints,char *namecon);
//extern void           gsp_fix_unset_input2zero (lofig_list *figext,ptype_list **constraints,cnsfig_list *cnsfig);
extern chain_list    *gsp_calcconetransfer  ( char*,cone_list*,char,int,int, chain_list **, chain_list **, chain_list **);
extern ptype_list    *gsp_SpiceFindInput    ( 
                                              ptype_list* ,
                                              pNode ,
                                              pCircuit,
                                              char,
                                              int *,
                                              int *
                                            );
extern ptype_list    *gsp_get_switch_cst    (ptype_list *globalcst,
                                              chain_list *chaincone);
extern ptype_list    *gsp_get_patterns      ( lofig_list  *lofig,
                                              cnsfig_list *cnsfig,
                                              lofig_list  *figext,
                                              chain_list  *chaincone,
                                              chain_list  *chaininstance,
                                              char corner,
                                              spisig_list *spisig,
                                              chain_list **
                                            );
extern chain_list    *gsp_GetSupFromCone    ( chain_list* );
/*extern chain_list    *gsp_AddConeOutPath    ( chain_list* , 
                                              cone_list*,
                                              chain_list**,
                                              int,int);*/
extern char           gsp_SigInInterface    ( losig_list*);
extern ptype_list    *gsp_spisetinputs_from_loins ( loins_list* );

extern ptype_list    *gsp_FillPtypelistCst  ( chain_list*, locon_list* );
extern ptype_list    *gsp_BuildListCst      ( ptype_list* );
extern ptype_list    *gsp_VerifGoodCstList  ( ptype_list* );
extern void           gsp_PrintCstLst       ( ptype_list* );
extern ptype_list    *gsp_MergeCst          ( ptype_list* , ptype_list*);
extern ptype_list    *gsp_FixSigCst         ( ptype_list*, char*, int);
extern ptype_list    *gsp_ModifCstLst       ( ptype_list*, ptype_list*);
extern chain_list    *gsp_get_hz_cst        (loins_list* ins);
extern chain_list    *gsp_get_ins_out_path  (lofig_list *fig);
extern ptype_list    *gsp_merge_all_hz_cst  (lofig_list *fig,
                                             ptype_list *cstraints);
void gsp_constraint_abl_with_switch( ptype_list *switchlist, chain_list *sup_abl, chain_list *sdn_abl );
