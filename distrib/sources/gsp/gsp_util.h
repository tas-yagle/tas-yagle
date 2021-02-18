/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : gsp_util.h                                                  */
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
extern void           gsp_deltopname        (ptype_list *cst);
extern chain_list    *gsp_classlosig        ( chain_list* , losig_list* );
extern locon_list    *gsp_GetLoconIn        ( loins_list*, ptype_list*, long*);
extern locon_list    *gsp_GetLoconOut       ( loins_list*, long*);
extern long           gsp_GetTransition     ( locon_list*, locon_list* );
extern void           gsp_FreeMarksOnCnsfig (cnsfig_list *cnsfig);
