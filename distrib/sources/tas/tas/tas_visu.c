/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_visu.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* visualisation des parametres technologie                                 */
/****************************************************************************/

#include "tas.h"

/******************************************************************************/
/*                        fonction tas_visu()                                 */
/* affichage des parametres techno.                                           */
/******************************************************************************/
void tas_visu()
{
    elpmodel_list *model ;
    int i ;

    if (avt_islog(3,LOGTAS)) {
    for(i = 0 ; i < TAB_INDEX->length ; i++) {
        if(getititem(TAB_INDEX,i) == EMPTYHT)
        continue ;
        model = elpGetModelIndex(i) ;
        avt_log (LOGTAS, 3, "Param for Model %s\n",model->elpModelName) ;
        avt_log (LOGTAS, 3, "DW      = %e\n",getdtitem(TAB_DW,i)) ;
        avt_log (LOGTAS, 3, "DL      = %e\n",getdtitem(TAB_DL,i)) ;
        avt_log (LOGTAS, 3, "VT      = %e\n",getdtitem(TAB_VT,i)) ;
        avt_log (LOGTAS, 3, "A       = %e\n",getdtitem(TAB_A,i)) ;
        avt_log (LOGTAS, 3, "B       = %e\n",getdtitem(TAB_B,i)) ;
        avt_log (LOGTAS, 3, "RT      = %e\n",getdtitem(TAB_RT,i)) ;
        avt_log (LOGTAS, 3, "LMLT    = %e\n",getdtitem(TAB_LMLT,i)) ;
        avt_log (LOGTAS, 3, "WMLT    = %e\n",getdtitem(TAB_WMLT,i)) ;
        avt_log (LOGTAS, 3, "VDDmax  = %e\n",getdtitem(TAB_VDDmax,i)) ;
        avt_log (LOGTAS, 3, "SEUIL   = %e\n",getdtitem(TAB_SEUIL,i)) ;
        avt_log (LOGTAS, 3, "Vdeg    = %e\n",getdtitem(TAB_deg,i)) ;
        avt_log (LOGTAS, 3, "R       = %e\n",getdtitem(TAB_R,i)) ;
        avt_log (LOGTAS, 3, "RDD     = %e\n",getdtitem(TAB_RDD,i)) ;
        avt_log (LOGTAS, 3, "KDD     = %e\n",getdtitem(TAB_KDD,i)) ;
        avt_log (LOGTAS, 3, "RDF     = %e\n",getdtitem(TAB_RDF,i)) ;
        avt_log (LOGTAS, 3, "RUD     = %e\n",getdtitem(TAB_RUD,i)) ;
        avt_log (LOGTAS, 3, "KUD     = %e\n",getdtitem(TAB_KUD,i)) ;
        avt_log (LOGTAS, 3, "RUF     = %e\n",getdtitem(TAB_RUF,i)) ;
        avt_log (LOGTAS, 3, "Q       = %e\n",getdtitem(TAB_Q,i)) ;
        avt_log (LOGTAS, 3, "KG      = %e\n",getdtitem(TAB_KG,i)) ;
        avt_log (LOGTAS, 3, "IFB     = %e\n",getdtitem(TAB_IFB,i)) ;
        avt_log (LOGTAS, 3, "CGS     = %e\n",model->elpCapa[elpCGS]) ;
        avt_log (LOGTAS, 3, "CGP     = %e\n",model->elpCapa[elpCGP]) ;
        avt_log (LOGTAS, 3, "CGD     = %e\n",model->elpCapa[elpCGD]) ;
        avt_log (LOGTAS, 3, "CDS     = %e\n",model->elpCapa[elpCDS]) ;
        avt_log (LOGTAS, 3, "CDP     = %e\n",model->elpCapa[elpCDP]) ;
        avt_log (LOGTAS, 3, "CDW     = %e\n",model->elpCapa[elpCDW]) ;
        avt_log (LOGTAS, 3, "CSS     = %e\n",model->elpCapa[elpCSS]) ;
        avt_log (LOGTAS, 3, "CSP     = %e\n",model->elpCapa[elpCSP]) ;
        avt_log (LOGTAS, 3, "CSW     = %e\n",model->elpCapa[elpCSW]) ;
        avt_log (LOGTAS, 3, "SCALE_X  = %ld\n",SCALE_X) ;
        avt_log (LOGTAS, 3, "TAS_CONTEXT->FRONT_CON = %e\n",TAS_CONTEXT->FRONT_CON) ;
        avt_log (LOGTAS, 3, "\n") ;
    }
    }
}
