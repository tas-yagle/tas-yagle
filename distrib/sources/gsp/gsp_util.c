/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : gsp_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* simulation spice d'un chemin                                             */
/****************************************************************************/
#include "gsp.h"

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_deltopname                                                 */
/*                                                                           */
/* del top level name                                                        */
/*                                                                           */
/*****************************************************************************/

void gsp_deltopname (ptype_list *cst)
{
    ptype_list *ptype;
    char       *left,*right;

    if (gsp_is_pat_indpd (cst) == 'Y')
        return ;
    for (ptype = cst; ptype; ptype = ptype->NEXT) {
        if (ptype->TYPE == GSP_STUCK_INDPD) continue;
        else {
            leftunconcatname((char*)ptype->DATA,&left,&right);
            ptype->DATA = right;
        }
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION :  gsp_classlosig                                                */
/*                                                                           */
/* Fonction qui met la losig contenant le signal le + tardif                 */
/* dans la tete d'une chaine list                                            */
/*                                                                           */
/*****************************************************************************/
chain_list *gsp_classlosig (headchain, losig2class)
    chain_list *headchain;
    losig_list *losig2class;
{
    chain_list *chain;
    losig_list *losig;
    chain_list *prev=NULL;

    if ( headchain && ((losig_list*)headchain->DATA != losig2class) ) {
      for (chain = headchain ; chain ; chain = chain->NEXT) {
        losig = (losig_list*)chain->DATA;
        if (losig == losig2class) {
          if ( prev ) 
            prev->NEXT = chain->NEXT;
          chain->NEXT = headchain;
          headchain = chain;
          break;
        }
        prev = chain;
      }
    }

    return headchain ;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_GetLoconIn ()                                              */
/*                                                                           */
/* Trouve l entree la plus tardive sur une instance                          */
/* Retourne egalement une ptype_list si plusieurs entrees connues attaquent  */
/* cette instance.                                                           */
/*                                                                           */
/*****************************************************************************/
locon_list *gsp_GetLoconIn(loins,listptype,slope)
    loins_list *loins;
    ptype_list *listptype;
    long       *slope;
{
    lofig_list *lofig;
    locon_list *locon;
    locon_list *loconin = NULL;
    chain_list *chainmarksig = NULL;
    chain_list *chain;
    ptype_list *ptype;
    int         maxnum = 0;
    char        direction,*loins_context;
    int         num;
    ht *hash;

    if (loins == NULL) return NULL;
    if ((lofig = getloadedlofig(loins->FIGNAME)) == NULL) 
        return NULL;

    for (locon = loins->LOCON ; locon ; locon = locon->NEXT) {
        direction = locon->DIRECTION;
        if (direction == 'X')
            direction = (efg_FindLocon (lofig,locon->NAME))->DIRECTION;
        if ((direction == 'I') || (direction == 'B') || (direction == 'T')) {
            if ((ptype = getptype(locon->SIG->USER,EFG_SIG_SET)) != NULL) {
               chainmarksig = addchain (chainmarksig,(void*)locon);
               if (slope != NULL)
                   *slope = (long)ptype->DATA;
            }
        }
    }
    if (chainmarksig == NULL) 
        return NULL;
    else if (chainmarksig->NEXT == NULL)
        loconin = (locon_list*)chainmarksig->DATA;
    else {
        /* repere le sig le + tardif */
        loins_context = loins->INSNAME;
        for (chain = chainmarksig ; chain ; chain = chain->NEXT) {
            locon = (locon_list*)chain->DATA;
            if ((ptype = getptype(locon->SIG->USER,EFG_SIG_SET_NUM)) != NULL) {
                hash = (ht*)ptype->DATA;
                num = gethtitem(hash,(void*)(loins_context));
                if ((num != EMPTYHT) && ((int)num > maxnum)) {
                    loconin = locon; 
                    maxnum = (int)num;
                }
            }
        }
        listptype = gsp_FillPtypelistCst(chainmarksig,loconin);
    }
    freechain(chainmarksig);
    return loconin;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_GetLoconOut ()                                             */
/*                                                                           */
/* Trouve la transition sur le connecteur de sortie                          */
/*                                                                           */
/*****************************************************************************/
locon_list *gsp_GetLoconOut(loins,slope)
    loins_list *loins;
    long       *slope;
{
    lofig_list *lofig;
    locon_list *locon;
    locon_list *loconout = NULL;
    char        direction;
    ptype_list *ptype;

    if (loins == NULL) return NULL;
    if ((lofig = getloadedlofig(loins->FIGNAME)) == NULL) 
        return NULL;

    for (locon = loins->LOCON ; locon ; locon = locon->NEXT) {
        direction = locon->DIRECTION;
        if (direction == 'X')
            direction = (efg_FindLocon (lofig,locon->NAME))->DIRECTION;
        if ((direction == 'O') || (direction == 'B') || 
            (direction == 'Z') || (direction == 'T')) {
            if ((ptype = getptype(locon->SIG->USER,EFG_SIG_SET)) != NULL) {
                loconout = locon; 
                if (slope != NULL)
                    *slope = (long)ptype->DATA;
            }
        }
    }
    return loconout;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_GetTransition()                                            */
/*                                                                           */
/* Obtient la transition entre deux connecteurs                              */
/*                                                                           */
/*****************************************************************************/
long gsp_GetTransition(loconin,loconout)
    locon_list *loconin;
    locon_list *loconout;
{
    ptype_list *ptype;
    char        infront;
    char        outfront;
    long        trans;

    if ((ptype = getptype(loconin->SIG->USER,EFG_SIG_SET)) != NULL) {
        if (ptype->DATA == EFG_SIG_SET_RISE)
            infront = 'U';
        else
            infront = 'D';
    }
    if ((ptype = getptype(loconout->SIG->USER,EFG_SIG_SET)) != NULL) {
        if (ptype->DATA == EFG_SIG_SET_RISE)
            outfront = 'U';
        else
            outfront = 'D';
    }
    if (infront == 'U') {
        if (outfront == 'U')
            trans = CBH_TRANS_UU ;
        else
            trans = CBH_TRANS_UD ;
    }
    else {
        if (outfront == 'U')
            trans = CBH_TRANS_DU ;
        else
            trans = CBH_TRANS_DD ;
    }
    return trans;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : gsp_FreeMarksOnCnsfig                                          */
/*                                                                           */
/*****************************************************************************/
gsp_cone_info *gsp_get_cone_info(cone_list *cone)
{
  ptype_list *pt;
  if ((pt=getptype (cone->USER,GSP_CONE_NUMBER)) != NULL)
    return (gsp_cone_info *)pt->DATA;
  return NULL;
}

void gsp_delete_cone_info(cone_list *cone)
{
  gsp_cone_info *ci;
  ptype_list *pt;
  if ((pt=getptype (cone->USER,GSP_CONE_NUMBER)) != NULL)
    {
      mbkfree(pt->DATA);
      cone->USER=delptype(cone->USER,GSP_CONE_NUMBER);
    }
}

gsp_cone_info *gsp_create_cone_info(cone_list *cone)
{
  gsp_cone_info *ci;
  ptype_list *pt;
  if ((pt=getptype (cone->USER,GSP_CONE_NUMBER)) == NULL)
    {
      ci=(gsp_cone_info *)mbkalloc(sizeof(gsp_cone_info));
      cone->USER=addptype(cone->USER,GSP_CONE_NUMBER, ci);
      ci->Date=ci->MinDate=-1;
      ci->BeforeTransitionState=ci->AfterTransitionState=-1;
      ci->cmd=NULL;
      ci->cmd_state=-1;
      ci->precharged=0;
      ci->usestate=0;
      ci->used_ic=-1;
    }
  else
    ci=(gsp_cone_info *)pt->DATA;
  return ci;
}

void gsp_FreeMarksOnCnsfig (cnsfig_list *cnsfig)
{
  cone_list *cone;
  
  if (!cnsfig) return;
  for (cone = cnsfig->CONE ; cone ; cone=cone->NEXT) 
    {
      gsp_delete_cone_info(cone);
    }
}

void gsp_FreeMarksOnCnsfigFromChainCone (chain_list *cl)
{
    cone_list *cone;

    while (cl!=NULL)
    {
      cone = (cone_list *)cl->DATA;
      gsp_delete_cone_info(cone);
      cl=cl->NEXT;
    }
}


