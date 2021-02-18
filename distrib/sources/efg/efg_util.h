/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : efg_util.h                                                  */
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
extern int            efg_cone_is_fonctionnal (cone_list *cone,int depth);
extern void           efg_del_corresp_alias_trs (lotrs_list *sig);
extern char          *efg_get_origtrsname (lotrs_list *sig);
extern void           efg_del_corresp_alias (losig_list *sig);
extern char          *efg_get_origsigname (losig_list *sig);
extern int            efg_is_conname_ext (lofig_list *fig, losig_list *sig, long pnode);
extern void           efg_add_lotrs_capa (lofig_list *fig, locon_list *con, int incr_index);
extern void           efg_add_lotrs_capa_dup (lofig_list *fig, locon_list *con);
extern void           efg_set_ins_ctxt (lofig_list *lofig, char *ctxt);
extern char          *efg_get_ins_ctxt (loins_list *ins);
extern locon_list    *efg_get_locon (lofig_list *lofig,char *loconname);
extern lofig_list    *efg_get_fig2ins (loins_list *loins);
extern void           efg_set_fig2ins (loins_list *loins, lofig_list *lofig);
extern int            efg_is_loins_to_copy (loins_list *loins);
extern int            efg_loins_is2analyse (char *context);
extern lofig_list    *efg_get_fig_from_ctxt (lofig_list *, char *context );
extern int            efg_sig_is_marked (losig_list *sig,char *ctxt,long type);
extern int            efg_sig_is_beg    (losig_list *sig,char *ctxt);
extern int            efg_sig_is_end    (losig_list *sig,char *ctxt);
extern int            efg_sig_is_onpath (losig_list *sig,char *ctxt);
extern void           efg_set_vdd_on_destfig (losig_list*);
extern losig_list    *efg_get_vdd_on_destfig (void);
extern void           efg_set_vss_on_destfig (losig_list*);
extern losig_list    *efg_get_vss_on_destfig (void);
extern char          *efg_revect (char *);
extern losig_list    *efg_getlosigcone (cone_list*);
extern void           efg_setctcnet (losig_list *,
                                     losig_list *,
                                     losig_list *,
                                     double );
extern int            efg_dup_ptype(losig_list *,losig_list *);
extern int            efg_dup_hier_ptype (losig_list *,losig_list *,char *ctxt);
extern locon_list    *efg_get_org_con(locon_list *);
extern lotrs_list    *efg_get_org_trs(lotrs_list *);
extern losig_list    *efg_get_org_sig(losig_list *);
extern locon_list    *efg_get_ext_con(locon_list *);
extern losig_list    *efg_get_ext_sig(losig_list *);
extern lotrs_list    *efg_get_ext_trs(lotrs_list *);
extern losig_list    *efg_addlosig (lofig_list *,losig_list *, int);
extern void           efg_rebuild_ctc (lofig_list *);
extern locon_list    *efg_add_global_alim (lofig_list *,losig_list *,int);
extern locon_list    *efg_addlocon (lofig_list *,locon_list *,int);
extern lotrs_list    *efg_add_blockedlotrs(lofig_list *,locon_list *,int);
extern lotrs_list    *efg_add_blockedlotrs_dup(lofig_list *,locon_list *);
extern lotrs_list    *efg_addlotrs (lofig_list *,lotrs_list *,int);
extern loins_list    *efg_addloins (lofig_list *,loins_list *,int);
extern char           efg_SigIsAlim         ( losig_list * );
extern void           efg_DelLofigPtype     ( lofig_list* , long );
extern losig_list    *efg_GetHierSigByName  ( lofig_list*, char*, char**,loins_list**,char);
extern void           efg_DelHierSigPtype   ( lofig_list*);
extern locon_list    *efg_FindLocon         ( lofig_list*, char * );
extern ptype_list    *efg_SetHierPtype      ( ptype_list*,
                                              long,       
                                              void*, 
                                              long,
                                              char*
                                            );
extern void          *efg_get_hier_sigptype (losig_list *sig,char *ctxt,long type);
extern char           efg_SigIsCst          ( char*, ptype_list*);
extern losig_list    *efg_GetSigByName      ( lofig_list* , char*);
extern locon_list    *efg_GetHierConByName  ( lofig_list*, char*);
extern losig_list    *efg_get_ht_sig        ( lofig_list*, char*);
extern chain_list    *efg_set_extra_capa    (lofig_list *figext,
                                             char *signame,
                                             float capaval
                                            );
extern void           efg_addctc            ( chain_list **added_ctclist,
                                              losig_list *sig,
                                              losig_list *vss,
                                              float capa
                                            );
extern void           efg_del_extra_capa    (chain_list *headctc);
extern int            efg_is_wire_on_sig      (losig_list* losig);
extern char          *efg_spisplitnodename (char *nodename, long *index);
extern void           efg_set_node_in_out_lotrs (lotrs_list *lotrs);
void efg_locon_add_lofigchain( locon_list *locon ); 
void efg_lotrs_add_lofigchain( lotrs_list *lotrs );
void efg_locon_remove_lofigchain( locon_list *locon ); 
void efg_lotrs_remove_lofigchain( lotrs_list *lotrs );

