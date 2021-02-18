/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : efg_ext_fig.h                                               */
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
/* Declaration des defines                                                  */
/****************************************************************************/

/****************************************************************************/
/* Declaration de fonction                                                  */
/****************************************************************************/
/*extern void           efg_extract_fig       ( lofig_list  *lofig,
                                              lofig_list **figext,
                                              cnsfig_list *cnsfig,
                                              spisig_list *spisig,
											  chain_list **chaincone,
											  chain_list **chaininstance,
											  chain_list  *loins2drv,
											  chain_list  *lotrs2drv,
											  chain_list  *locon2drv,
											  chain_list  *losig2drv,
											  chain_list  *cone_onpath,
                                              ptype_list  *usrlist,
                                              int          markfig
                                            );
                                            */
extern void           efg_UpdateSigPtype    ( lofig_list*,
                                                  char*,
                                                  char*,
                                                  char*,
                                                  int,
                                                  char,
                                                  char
                                               );
extern void           efg_SetSigDrive       ( losig_list*,
                                                  int,  
                                                  char,
                                                  char,
                                                  char,
                                                  char,
                                                  char *,
                                                  long flags
                                               );
extern list_list     *efg_BuildPathSig      ( list_list*,char*, char);
extern void           efg_FreePathList      ( list_list* );
extern void           efg_FreeMarksOnExtFig (lofig_list *fig_ext);
extern char           efg_con_is_on_path    (chain_list *cone_onpath,
                                             cone_list *cone);
extern void           efg_add_instance      (chain_list *chaincone,
                                             chain_list **chaininstance);
extern void           efg_MarkCnsfig        (lofig_list *origlofig,
                                             lofig_list *fig_ext,
                                             cnsfig_list *cnsfig,
                                             chain_list **chaincone,
                                             chain_list **chaininstance,
                                             chain_list *cone_onpath
                                             );

void efg_dellosig( lofig_list *lofig, losig_list *losig );
