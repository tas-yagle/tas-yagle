/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_tec.h                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* declaration de variable */


extern dt     *TAB_R         ,
              *TAB_A         ,
              *TAB_B         ,
              *TAB_RT        ,
              *TAB_VT        ,
              *TAB_deg       ,
              *TAB_VDDmax    ,
              *TAB_SEUIL     ,
              *TAB_RDD       ,
              *TAB_KDD       ,
              *TAB_RDF       ,
              *TAB_RUD       ,
              *TAB_KUD       ,
              *TAB_RUF       ,
              *TAB_KG        ,
              *TAB_Q         ,
              *TAB_K         ,
              *TAB_KT        ,
              *TAB_IFB       ,
              *TAB_CGP       ,
              *TAB_CDS       ,
              *TAB_MULU0     ,
              *TAB_DELTAVT0  ,
              *TAB_DL        ,
              *TAB_DW        ,
              *TAB_LMLT      ,
              *TAB_WMLT      ,
              *TAB_RACCS     ,
              *TAB_RACCD     ;

extern it     *TAB_INDEX ;


/* declaration de fonction */
extern int            tas_TechnoParameters __P((void)) ;
extern double         tas_getparam __P((
                                        lotrs_list*,
                                        int,
                                        int
                                      )) ;
extern int            tas_calparam __P((elpmodel_list*)) ;
