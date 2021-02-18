/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_tp.c                                                    */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* calcul des temps de propagation entre une sortie de cone                 */
/* et toutes ses entrees                                                    */
/****************************************************************************/

#include "tas.h"

long FHH;
long FLL;
long CCUSED;
char TAS_PATH_TYPE = 'M';
int tas_counter_model = 0;

double tas_get_rlin(link_list * link)
{
  double r;
  double tp_rt;
  long l, w;
  float rate;

  tp_rt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_RT);

  if ((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
    rate = tas_get_current_rate(link);
  else
    rate = 1.0;

  l = TAS_GETLENGTH(link);
  w = TAS_GETWIDTH(link) * rate;

  r = tp_rt * ((double) l) / ((double) w);

  return r;
}

/*****************************************************************************/
/*                          fonction tas_get_rlin_br()                       */
/* calcule la resistance lineaire d'une branche                              */
/*****************************************************************************/
double tas_get_rlin_br(link)
link_list *link;
{
  double tp_rt = 0.0, Rtr = 0.0;
  long l, w;
  float rate;
  double rs, rd;

  while (link && ((link->TYPE & CNS_EXT) != CNS_EXT)) {

    tas_getraccess(link, &rs, &rd);

    tp_rt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_RT);

    if ((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
      rate = tas_get_current_rate(link);
    else
      rate = 1.0;

    l = TAS_GETLENGTH(link);
    w = TAS_GETWIDTH(link) * rate;
    Rtr += tp_rt * ((double) l) / ((double) w) + rs + rd;
    link = link->NEXT;
  }

  return Rtr;
}

double tas_get_rlin_br_krt( link_list *link )
{
  double krt ;
  double tp_krt ;
  
  krt = 1000.0 ;
  
  while (link && ((link->TYPE & CNS_EXT) != CNS_EXT)) {

    tp_krt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_KRT);
    if( tp_krt < krt )
      krt = tp_krt ;

    link = link->NEXT;
  }

  return krt ;
}

/*****************************************************************************/
/*                          fonction tas_delayrc()                           */
/* calcule le delay rc entre deux maillons                                   */
/*****************************************************************************/
float tas_delayrc(maillon1, maillon2)
link_list *maillon1;
link_list *maillon2;
{
  lotrs_list *lotrs1;
  lotrs_list *lotrs2;
  locon_list *locon1;
  locon_list *locon2;
  rcx_list *ptrcx;
  float delay;
  long resis;

  if (TAS_CONTEXT->TAS_CALCRCX == 'N')
    return (0.0);

  lotrs1 = NULL;
  lotrs2 = NULL;
  locon1 = NULL;
  locon2 = NULL;
  delay = 0.0;
  resis = (long) 0;


  if ((maillon1->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
    lotrs1 = maillon1->ULINK.LOTRS;
  } else {
    locon1 = maillon1->ULINK.LOCON;
  }

  if ((maillon2->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
    lotrs2 = maillon2->ULINK.LOTRS;
  } else {
    locon2 = maillon2->ULINK.LOCON;
  }

  if ((lotrs1 != NULL) && (lotrs2 != NULL)) {
    if ((lotrs1->DRAIN == NULL) || (lotrs2->DRAIN == NULL)
        || (lotrs1->SOURCE == NULL) || (lotrs2->SOURCE == NULL))
      return (delay);

    if ((lotrs1->DRAIN->SIG == NULL) || (lotrs2->DRAIN->SIG == NULL) ||
        (lotrs1->SOURCE->SIG == NULL) || (lotrs2->SOURCE->SIG == NULL))
      return (delay);

    if (lotrs1->DRAIN->SIG == lotrs2->DRAIN->SIG) {
      locon1 = lotrs1->DRAIN;
      locon2 = lotrs2->DRAIN;
    } else if (lotrs1->SOURCE->SIG == lotrs2->SOURCE->SIG) {
      locon1 = lotrs1->SOURCE;
      locon2 = lotrs2->SOURCE;
    } else if (lotrs1->DRAIN->SIG == lotrs2->SOURCE->SIG) {
      locon1 = lotrs1->DRAIN;
      locon2 = lotrs2->SOURCE;
    } else if (lotrs1->SOURCE->SIG == lotrs2->DRAIN->SIG) {
      locon1 = lotrs1->SOURCE;
      locon2 = lotrs2->DRAIN;
    }
  } else if ((lotrs1 != NULL) && (locon2 != NULL)) {
    if ((lotrs1->DRAIN == NULL) || (locon2->SIG == NULL)
        || (lotrs1->SOURCE == NULL))
      return (delay);

    if ((lotrs1->DRAIN->SIG == NULL) || (lotrs1->SOURCE->SIG == NULL))
      return (delay);

    if (lotrs1->DRAIN->SIG == locon2->SIG) {
      locon1 = lotrs1->DRAIN;
    } else if (lotrs1->SOURCE->SIG == locon2->SIG) {
      locon1 = lotrs1->SOURCE;
    }
  } else if ((lotrs2 != NULL) && (locon1 != NULL)) {
    if ((lotrs2->DRAIN == NULL) || (locon1->SIG == NULL)
        || (lotrs2->SOURCE == NULL))
      return (delay);

    if ((lotrs2->DRAIN->SIG == NULL) || (lotrs2->SOURCE->SIG == NULL))
      return (delay);

    if (lotrs2->DRAIN->SIG == locon1->SIG) {
      locon2 = lotrs2->DRAIN;
    } else if (lotrs2->SOURCE->SIG == locon1->SIG) {
      locon2 = lotrs2->SOURCE;
    }
  }

  if ((locon1 == NULL) || (locon2 == NULL))
    return (delay);
  else if (locon1 == locon2)
    return (delay);
  else if (locon1->SIG != locon2->SIG)
    return (delay);
  else if (getptype(locon1->SIG->USER, TAS_SIG_NORCDELAY) != NULL)
    return (delay);

  ptrcx = getrcx(locon1->SIG);

  if (TAS_PATH_TYPE == 'm')
    delay = rcx_getdelay(TAS_CONTEXT->TAS_LOFIG, locon1, locon2);
  else
    delay = rcx_getdelay(TAS_CONTEXT->TAS_LOFIG, locon1, locon2);

  return (delay);
}

/*****************************************************************************/
/*                          fonction tas_delayrcchemin()                     */
/* calcule le delay rc sur une branche                                       */
/*****************************************************************************/
float tas_delayrcchemin(branch_list *chemin, link_list *activelink)
{
  link_list *link;
  float delay = 0.0;

  if (TAS_CONTEXT->TAS_CALCRCX == 'N')
    return (0.0);
  
  for (link = chemin->LINK; link != NULL; link = link->NEXT) {
    
    if (V_BOOL_TAB[__AVT_OLD_FEATURE_30].VALUE==0 && link==activelink) break;
    if (link->NEXT != NULL) {
      delay += tas_delayrc(link, link->NEXT);
    }
  }


  avt_log(LOGTAS, 4, "        tas_delayrcchemin: delay = %.2f\n", delay);

  return (delay);
}

/*****************************************************************************/
/*                          fonction tas_getslope()                          */
/* renvoie le front d'entree d'un cone                                       */
/*****************************************************************************/
front_list *tas_getslope(cone, cone_avant)
cone_list *cone;
cone_list *cone_avant;
{
  edge_list *incone;
  ptype_list *ptype;
  front_list *front;

  if (TAS_CONTEXT->TAS_CALCRCX == 'Y') {
    incone = NULL;
    if (getptype(cone_avant->USER, TAS_RC_CONE) != NULL) {
      for (incone = cone->INCONE; incone != NULL; incone = incone->NEXT) {
        if ((incone->TYPE & CNS_EXT) != CNS_EXT) {
          if (incone->UEDGE.CONE == cone_avant)
            break;
        }
      }
      if (incone != NULL) {
        if (TAS_PATH_TYPE == 'm')
          ptype = getptype(incone->USER, TAS_SLOPE_MIN);
        else
          ptype = getptype(incone->USER, TAS_SLOPE_MAX);

        if (ptype != NULL) {
          front = (front_list *) ptype->DATA;
          if ((front->FUP != TAS_NOFRONT)
              || (front->FDOWN != TAS_NOFRONT))
            return (front);
        }
      }
    }
  }

  if (TAS_PATH_TYPE == 'm')
    front =
        (front_list *) ((getptype(cone_avant->USER, TAS_SLOPE_MIN))->DATA);
  else
    front =
        (front_list *) ((getptype(cone_avant->USER, TAS_SLOPE_MAX))->DATA);

  return front;
}

/*****************************************************************************/
/*                          fonction tas_getcapabl()                         */
/* renvoie la capacite de bleeder d'un maillon                               */
/*****************************************************************************/
double tas_getcapabl(link)
link_list *link;
{
  ptype_list *ptype;
  double cb;

  if ((TAS_PATH_TYPE == 'm')
      || ((link->TYPE & TAS_LINK_BL) != TAS_LINK_BL))
    return (double) 0;
  cb = (double) 0;
  if ((ptype = getptype(link->USER, TAS_LINK_BLDUP)) != NULL)
    cb += (double) ((long) ptype->DATA) / 1000;
  if ((ptype = getptype(link->USER, TAS_LINK_BLDDOWN)) != NULL)
    cb += (double) ((long) ptype->DATA) / 1000;

  avt_log(LOGTAS, 4, "        tas_getcapabl: cb = %f\n", cb);

  return (cb);
}

/*****************************************************************************/
/*                          fonction tas_PathFonc()                          */
/* dit si la branche de type BLEEDER du cone est fonctionnelle ou non, c'est */
/* a dire si elle appartient au champ USER du cone ou non. Renvoie '1' si la */
/* branche est fonctionnelle, 0 sinon.                                       */
/*****************************************************************************/
short tas_PathFonc(cone, path)
cone_list *cone;
branch_list *path;

{
  ptype_list *ptype;
  chain_list *chain;

  /* branche pas typee BLEEDER => elle est fonctionnelle */
  if (((path->TYPE & CNS_BLEEDER) != CNS_BLEEDER)
      && ((path->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK))
    return (1);
  /* si le cone n'est pas de type BLEEDER, alors la branche est non-fonc. */
  else if (((ptype =
             (ptype_list *) getptype(cone->USER, CNS_BLEEDER)) == NULL)
           &&
           ((cone->
             TYPE & (CNS_LATCH | CNS_FLIP_FLOP | CNS_MEMSYM | CNS_MASTER
                     | CNS_SLAVE)) == 0))
    return (0);

  if (((cone->
        TYPE & (CNS_LATCH | CNS_FLIP_FLOP | CNS_MEMSYM | CNS_MASTER |
                CNS_SLAVE)) != 0)
      && ((path->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK))
    return (1);

  if (ptype == NULL)
    tas_error(33, (char *) cone, TAS_ERROR);

  /* recherche de la branche path dans la liste des feedback */
  for (chain = (chain_list *) ptype->DATA; chain != NULL;
       chain = chain->NEXT)
    if (chain->DATA == (char *) path)
      break;

  /* on renvoie 1 si chain est != NULL (branche existante), '0' sinon */
  return ((chain == NULL) ? 0 : 1);
}

/****************************************************************************/
/*                         fonction tas_GetRint()                           */
/* renvoie la resistance de la sortie lorsque 'entree' commute.             */
/****************************************************************************/
double tas_GetRint(cone, incone, typebr, typetr)
cone_list *cone;
edge_list *incone;
long typebr;
char typetr;
{
  branch_list *path = NULL;
  link_list *link, *prevlink = NULL;
  long Ptype;
  char typel;
  double rs = 0.0;
  float rdd, rud;

  if ((incone->TYPE & CNS_EXT) == CNS_EXT) {
    branch_list *auxpath;

    for (auxpath = cone->BREXT; auxpath != NULL; auxpath = auxpath->NEXT) {
      for (link = (link_list *) auxpath->LINK; link->NEXT != NULL;
           link = link->NEXT);
      if (link->ULINK.LOCON == incone->UEDGE.LOCON)
        if ((path == NULL)
            || ((long) getptype(path->USER, TAS_RESIST)->DATA <
                (long) getptype(auxpath->USER, TAS_RESIST)->DATA))
          path = auxpath;
    }
  } else
    path = existe_tpd(cone, incone->UEDGE.CONE, typebr, typetr, &link);

  if (path == NULL)
    return (0.0);

  link = (link_list *) path->LINK;
  if ((link->TYPE & (CNS_IN | CNS_INOUT)) != 0)
    return (0.0);

  for ( /* initialise */ ; link->NEXT != NULL; link = link->NEXT)
    prevlink = link;
  if ((link->TYPE & (CNS_IN | CNS_INOUT)) != 0)
    link = prevlink;

  Ptype = path->TYPE;
  typel = path->LINK->ULINK.LOTRS->TYPE;

  if (((Ptype & CNS_DEGRADED) == 0) ||
      (((Ptype & CNS_EXT) != CNS_EXT) && (((typebr == CNS_VDD) &&
                                           ((typel & CNS_TP) == CNS_TP))
                                          || ((typebr == CNS_VSS)
                                              && ((typel & CNS_TN) ==
                                                  CNS_TN)))))
    rs = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax) / (2 * tas_get_current( path, path->LINK, 1.0, NO ));
  else {                        /* branche degradante */
    for (link = (link_list *) path->LINK; link != NULL; link = link->NEXT) {
      if ((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) {
        link = NULL;
        break;
      }

      if ((((path->TYPE & CNS_VDD) == CNS_VDD)
           && ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP))
          || (((path->TYPE & CNS_VSS) == CNS_VSS)
              && ((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN))
          || (((path->TYPE & CNS_EXT) == CNS_EXT)
              && ((typebr == CNS_VSS)
                  && ((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)))
          || (((path->TYPE & CNS_EXT) == CNS_EXT)
              && ((typebr == CNS_VDD)
                  && ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP))))
        break;

      if ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
        rdd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_RDD);
        if (!finite(rdd)) {
          TAS_PVT_COUNT++;
          tas_error(71, cone->NAME, TAS_WARNING);
          return -1.0;
        }
        rs +=
            rdd * (double) TAS_GETLENGTH(link) /
            (double) TAS_GETWIDTH(link);
      } else {
        rud = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_RUD);
        if (!finite(rud)) {
          TAS_PVT_COUNT++;
          tas_error(71, cone->NAME, TAS_WARNING);
          return -1.0;
        }
        rs +=
            rud * (double) TAS_GETLENGTH(link) /
            (double) TAS_GETWIDTH(link);
      }

    }

    if (link) {
      if ((path->TYPE & CNS_EXT) == CNS_EXT) {
        link_list *linkx;
        link_list *linksav;
        for (linkx = path->LINK; linkx->NEXT != NULL; linkx = linkx->NEXT)
          if ((linkx->NEXT->TYPE & (CNS_IN | CNS_INOUT)) != 0) {
            linksav = linkx->NEXT;
            linkx->NEXT = NULL;
            rs += (tas_getparam(linkx->ULINK.LOTRS, TAS_CASE, TP_VDDmax) / (2 * tas_get_current( path, link, 1.0, NO )));
            linkx->NEXT = linksav;
          }
      } else
        rs += (tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax) / (2 * tas_get_current(path, link, 1.0, NO )));
    }
  }
  return (rs);
}

/*-----------------------------------------------------------------------------
*                    fonction affiche_maillon()                               *
*                                                                             *
* fonction de debuggage: affiche le numero de l'index du cone qui attaque la  *
* grille de transistor du maillon                                             *
-----------------------------------------------------------------------------*/
int affiche_maillon(maillon0)
link_list *maillon0;

{
  if (maillon0 == NULL)
    avt_log(LOGTAS, 3, "maillon : NULL\n");
  else
    avt_log(LOGTAS, 3, "maillon : %ld  type : %d  L=%ld W=%ld  (%p)\n",
            ((cone_list *)
             getptype(maillon0->ULINK.LOTRS->USER,
                      CNS_DRIVINGCONE)->DATA)->INDEX,
            maillon0->ULINK.LOTRS->TYPE, TAS_GETLENGTH(maillon0),
            TAS_GETWIDTH(maillon0), maillon0);

  return (0);
}

/*-----------------------------------------------------------------------------
*                    affiche_time()                                           *
*                                                                             *
* fonction d'affichage de la valeur du temps de propagation de l'input_grid0  *
* de cone0  et de type TPxx                                                   *
-----------------------------------------------------------------------------*/
int affiche_time(cone0, input_grid0, type, valeur)
cone_list *cone0, *input_grid0;
char *type;
long valeur;

{
  if (valeur != TAS_NOTIME)
    avt_log(LOGTAS, 3, "                %s = %ld\n", type, valeur);
  /*
     if (valeur==TAS_NOTIME)
     printf("   %s = TAS_NOTIME\n", input_grid0->INDEX,cone0->INDEX,type);
     else   
     printf("   %s = %ld\n", input_grid0->INDEX,cone0->INDEX,type,valeur);
   */
  cone0 = NULL;
  input_grid0 = NULL;
  return 0;
}

/*-----------------------------------------------------------------------------
*                    existe_tpd_maillon()                                     *
*                                                                             *
* recherche l'existence d'un transistor de type type_transistor dont la       *
* grille de transistor est attaque par l'input_grid0 et qui appartient a la   *
* branche dont le premier element est maillon0                                *
* retourne le maillon ou se trouve le transistor si ce maillon existe         *
* sinon retourne NULL                                                         *
-----------------------------------------------------------------------------*/
link_list *existe_tpd_maillon(chemin0, maillon0, input_grid0,
                              type_transistor)
branch_list *chemin0;
link_list *maillon0;
cone_list *input_grid0;
char type_transistor;

{
  link_list *maillon1 = maillon0;

  while (maillon1) {
    if (maillon1->TYPE & (CNS_IN | CNS_INOUT)) {
      maillon1 = NULL;
      break;
    }

    if (((long) maillon1->ULINK.LOTRS->TYPE & type_transistor) ==
        type_transistor)
      if ((cone_list *)
          getptype(maillon1->ULINK.LOTRS->USER,
                   CNS_DRIVINGCONE)->DATA == input_grid0) {
        if (chemin0->TYPE & TAS_SWITCH_DEG) {
          if (maillon1->TYPE & CNS_SWITCH)
            break;
        } else
          break;
      }

    maillon1 = maillon1->NEXT;
  }

  return maillon1;
}

/*-----------------------------------------------------------------------------
*                    existe_tpd()                                             *
*                                                                             *
* recherche l'existence d'un transistor de type type_transistor dont la       *
* grille de transistor est attaque par l'input_grid0 et qui appartient aux    *
* branches  de type type_branche du cone0                                     *
* retourne le 1 er maillon de la branche ou se trouve le maillon du           *
* transistor s'il exite sinon retourne NULL                                   *
* retourne par adresse dans le champs maillon_trans le maillon du transistor  *
* en question                                                                 *
-----------------------------------------------------------------------------*/
static int branch_is_also_driving(branch_list *bref, branch_list *bnew, cone_list *ignore)
{
  link_list *lref, *lnew;
  cone_list *drcone;
  for (lnew=bnew->LINK; lnew!=NULL; lnew=lnew->NEXT)
  {
    if (lnew->TYPE & (CNS_IN | CNS_INOUT)) continue;    
    drcone=(cone_list *)getptype(lnew->ULINK.LOTRS->USER,CNS_DRIVINGCONE)->DATA;
    if (drcone==ignore) return 0;
    for (lref=bref->LINK; lref!=NULL; lref=lref->NEXT)
    {
      if (lref->TYPE & (CNS_IN | CNS_INOUT)) continue;    
      if (drcone == (cone_list *)getptype(lref->ULINK.LOTRS->USER, CNS_DRIVINGCONE)->DATA
          && (lnew->ULINK.LOTRS->TYPE & (TRANSN|TRANSP))==(lref->ULINK.LOTRS->TYPE & (TRANSN|TRANSP))
         )
        break;
    }
    if (lref==NULL) return 0;
  }
  return 1;
}

static int branch_is_false(branch_list *bref, branch_list *bnewlist, cone_list *ignore, long type_branche)
{
  while (bnewlist!=NULL)
  {
     if(!((type_branche == CNS_VDD && ((bnewlist->TYPE & CNS_NOT_UP) == CNS_NOT_UP))
        || (type_branche == CNS_VSS && ((bnewlist->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN))))
       {
         if (bnewlist!=bref && branch_is_also_driving(bref, bnewlist, ignore)) return 1;
       }
    bnewlist=bnewlist->NEXT;
  }
  return 0;
}

branch_list *existe_tpd(cone0, input_grid0, type_branche, type_transistor,
                        maillon_trans)
cone_list *cone0;
cone_list *input_grid0;
long type_branche;
char type_transistor;
link_list **maillon_trans;

{
  branch_list *chemin0, *ref;
  branch_list *chemin_ext = NULL, *chemin_v = NULL, *chemin_final;
  link_list *maillon0, *maillon1, *maillon2, *maillon3, *maillonx;
  char existe_ext = 'n', existe_v = 'n';

  for (chemin0 = cone0->BREXT; chemin0; chemin0 = chemin0->NEXT) {
    if((type_branche == CNS_VDD) && ((chemin0->TYPE & CNS_NOT_UP) == CNS_NOT_UP)) continue;
    if((type_branche == CNS_VSS) && ((chemin0->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN)) continue;
    if (!(chemin0->TYPE & CNS_BLEEDER)) {
      maillon0 = (link_list *) chemin0->LINK;
      if ((maillonx =
           existe_tpd_maillon(chemin0, maillon0, input_grid0,
                              type_transistor))) {
        if (!branch_is_false(chemin0, cone0->BREXT, input_grid0, type_branche))
        {
          existe_ext = 'o';
          if (chemin_ext==NULL || TAS_PATH_TYPE != 'M' || branch_is_also_driving(chemin_ext, chemin0, NULL))
          {
            chemin_ext = chemin0;
            maillon1 = maillonx;
          }
          /*if (TAS_PATH_TYPE == 'M')
            break;*/
        }
      }
    }
  }

  if (type_branche == CNS_VDD)
    chemin0 = cone0->BRVDD;
  else
    chemin0 = cone0->BRVSS;

  ref=NULL;
  for (; chemin0; chemin0 = chemin0->NEXT) {
    if (!(chemin0->TYPE & CNS_BLEEDER)) {
      maillon2 = (link_list *) (chemin0->LINK);
      maillonx =
          existe_tpd_maillon(chemin0, maillon2, input_grid0,
                             type_transistor);

      if (maillonx) {
        if (!branch_is_false(chemin0, type_branche == CNS_VDD?cone0->BRVDD:cone0->BRVSS, input_grid0, 0))
        {
          existe_v = 'o';
          if (chemin_v==NULL || TAS_PATH_TYPE != 'M' || branch_is_also_driving(chemin_v, chemin0, NULL))
          {
            chemin_v = chemin0;
            maillon3 = maillonx;
          }
          /*if (TAS_PATH_TYPE == 'M')
            break;*/
        }
      }
    }
  }
/*
  if (TAS_PATH_TYPE == 'M')
  {
    // existe t-il une autre branch qui passe tjs avec cette entree
    for (; chemin0; chemin0 = chemin0->NEXT) {
      if (!(chemin0->TYPE & CNS_BLEEDER) && chemin0->LINK->NEXT==NULL) {
          maillon2 = (link_list *) (chemin0->LINK);
          maillonx =
              existe_tpd_maillon(chemin0, maillon2, input_grid0,
                                 type_transistor);

          if (maillonx) {
            existe_v = 'o';
            chemin_v = chemin0;
            maillon3 = maillonx;
            break;
          }
        }
    }
  }
*/
  chemin_final = decision(existe_ext, existe_v, chemin_ext, chemin_v);

  if ((TAS_PATH_TYPE == 'm') && (existe_v == 'o') && (existe_ext == 'o')) {
    if (chemin_final == chemin_ext)
      chemin_final = chemin_v;
    else
      chemin_final = chemin_ext;
  }

  if (chemin_final) {
    if ((chemin_final->TYPE & CNS_EXT) == CNS_EXT)
      (*maillon_trans) = maillon1;
    else
      (*maillon_trans) = maillon3;
  } else
    (*maillon_trans) = NULL;

  return (chemin_final);
}


/*-----------------------------------------------------------------------------
*                    existe_conflit()                                         *
*                                                                             *
* presque identique a existe_tpd() sinon qu'on a ajoute un index qui permet   *
* de recuperer la n ieme branche ou se trouve le transistor pour le conflit   *
* les arguments sont donc les memes que pour existe_tpd() plus l'index        *
-----------------------------------------------------------------------------*/
branch_list *existe_conflit(cone0, input_grid0, type_branche,
                          type_transistor, index, ignored, maillon_trans)
cone_list *cone0;
cone_list *input_grid0;
long type_branche;
char type_transistor;
int index;
int ignored;
link_list **maillon_trans;

{
  branch_list *chemin0;
  branch_list *trig_always=NULL;
  link_list *maillon0 = NULL, *maillon1 = NULL;

  if (type_branche == CNS_VDD)
    chemin0 = cone0->BRVDD;
  else
    chemin0 = cone0->BRVSS;

  if ((TAS_PATH_TYPE == 'm') && (index-ignored >= 2))
    return NULL;

  while (chemin0) {
    if ((chemin0->TYPE & CNS_BLEEDER)
        || tas_is_switchdegraded_branch(chemin0)) {
      chemin0 = chemin0->NEXT;
      continue;
    }

    maillon0 = (link_list *) chemin0->LINK;
    if ((maillon1 =
         existe_tpd_maillon(chemin0, maillon0, input_grid0,
                            type_transistor)))
    {
      index--;
      if (TAS_PATH_TYPE == 'M' && maillon0->NEXT==NULL) trig_always=chemin0;
    }

    if (!index)
      break;

    chemin0 = chemin0->NEXT;
  }

  if (TAS_PATH_TYPE == 'M')
  {
    // y a t-il un chemin avec un maillon (qui switch tout le temps)
    if (trig_always!=NULL && trig_always!=chemin0) chemin0=NULL;
    else if (trig_always==NULL)
    {
      link_list *maillonx;
      for (trig_always=chemin0; trig_always!=NULL; trig_always=trig_always->NEXT)
      {
        if (!((chemin0->TYPE & CNS_BLEEDER)
          || tas_is_switchdegraded_branch(chemin0)) && trig_always->LINK->NEXT==NULL)
        {
          maillon0 = (link_list *) trig_always->LINK;
          if ((maillonx =
               existe_tpd_maillon(trig_always, maillon0, input_grid0,
                                  type_transistor))) break;
        }
      }
      if (trig_always!=NULL) chemin0=trig_always, maillon1=maillonx;
    }
  }

  if (!chemin0) {
    return NULL;
  } else {
    *maillon_trans = maillon1;
    return chemin0;
  }
}

/*--------------------------------------------------------------------------*/
/*     i_brmcc                                                              */
/*--------------------------------------------------------------------------*/

double i_brmcc(maillon0, maillon1, level)
link_list *maillon0;
link_list *maillon1;
float level;
{
  link_list *maillonc;
  double i0,
      bx = 0.0,
      Rtr = 0.0, Rs = 0.0, imax =
      0.0, k, ugs, uds, usat, fsat, fi, ui, vi, Ri, ut, a, b, vddmax;
  /* ui */
  /* ui-1 c'est vi */
  vddmax = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  ut = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
  a = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_A);
  b = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_B);
  bx = (2 * (vddmax - ut) * vddmax -
        vddmax * vddmax) / ((vddmax - ut) * (vddmax - ut));
  i0 = (a * (vddmax - ut) * (vddmax - ut)) / (1.0 + b * (vddmax - ut));
  k = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_RT) * i0) /
      (vddmax - ut);

  Rs = 0.0;
  Rtr = 0.0;
  for (maillonc = maillon0;
       maillonc && ((maillonc->TYPE & (CNS_IN | CNS_INOUT)) == 0);
       maillonc = maillonc->NEXT) {
    Rtr =
        (double) TAS_GETLENGTH(maillonc) / (double) TAS_GETWIDTH(maillonc);
    Rs += Rtr;
  }

  Ri = Rs;
  ui = level * vddmax ;

  vi = 0.0;
  imax = 0.0;

  for (maillonc = maillon0;
       maillonc && ((maillonc->TYPE & (CNS_IN | CNS_INOUT)) == 0);
       maillonc = maillonc->NEXT) {

    if (maillonc != maillon0)
      ui = (vddmax - ut) * (1.0 - sqrt(1.0 - (Ri / Rs) * bx));

    Rtr =
        (double) TAS_GETLENGTH(maillonc) / (double) TAS_GETWIDTH(maillonc);
    Ri -= Rtr;
    vi = (vddmax - ut) * (1.0 - sqrt(1.0 - (Ri / Rs) * bx));
    ugs = (vddmax - vi - ut);
    uds = ui - vi;
    usat = k * ugs;
    fsat = (a * ugs * ugs) / (1.0 + (b * ugs));
    fi = fsat;

    if (uds < usat)
      fi *= uds / usat;

    imax += fi / Rs;

    avt_log(LOGTAS, 4, "        i_brmcc: imax = %.2e", imax);
    avt_log(LOGTAS, 4, " i0   = %.2e\n", i0);
    avt_log(LOGTAS, 4, "        i_brmcc: vi   = %.2e", vi);
    avt_log(LOGTAS, 4, " ui   = %.2e\n", ui);
    avt_log(LOGTAS, 4, "        i_brmcc: ugs  = %.2e", ugs);
    avt_log(LOGTAS, 4, " uds  = %.2e\n", uds);
    avt_log(LOGTAS, 4, "        i_brmcc: fi   = %.2e", fi);
    avt_log(LOGTAS, 4, " Rs   = %.2e", Rs);
    avt_log(LOGTAS, 4, " k    = %.2e\n", k);
    avt_log(LOGTAS, 4, "        i_brmcc: bx   = %.2e", bx);
    avt_log(LOGTAS, 4, " usat = %.2e\n", usat);
  }

  return imax;
}

/******************************************************************************/

int tas_is_switchdegraded_branch(branch_list * br)
{
  if (br->TYPE & TAS_SWITCH_DEG)
    return 1;
  return 0;
}

int tas_set_input_to_propagate_lotrs( lotrs_list *lotrs )
{
  long        level ;
  ptype_list *ptype ;

  level = -1 ;
  if( MLO_IS_TRANSN( lotrs->TYPE ) ) level = 1 ;
  if( MLO_IS_TRANSP( lotrs->TYPE ) ) level = 0 ;

  if( level == -1 ) {
    return 0 ;
  }

  ptype = getptype( lotrs->GRID->SIG->USER, TAS_SIGNAL_LEVEL );
  if( !ptype ) 
    lotrs->GRID->SIG->USER = addptype( lotrs->GRID->SIG->USER, TAS_SIGNAL_LEVEL, (void*)level );
  else {
    if( ptype->DATA != (void*)level ) {
      return 0;
    }
  }

  return 1 ;
}

/* remove information added by tas_set_input_to_propagate */
void tas_clear_input_to_propagate( link_list *link0 )
{
  link_list  *link ;
  lotrs_list *lotrs ;
  lotrs_list *trspair ;
  ptype_list *ptype ;
  
  for( link = link0 ; link ; link = link->NEXT ) {

    if( ( link->TYPE & CNS_EXT ) != CNS_EXT ) {

      lotrs = link->ULINK.LOTRS ;
      lotrs->GRID->SIG->USER = testanddelptype( lotrs->GRID->SIG->USER, TAS_SIGNAL_LEVEL );

      if( ( link->TYPE & CNS_SWITCH ) == CNS_SWITCH ) {
        ptype = getptype( lotrs->USER , TAS_TRANS_SWITCH );
        if( ptype ) {
          trspair = (lotrs_list*)ptype->DATA ;
          trspair->GRID->SIG->USER = testanddelptype( trspair->GRID->SIG->USER, TAS_SIGNAL_LEVEL );
        }
      }
    }
  }
}

/* set the input value required to propagate the active
   branch.
*/
void tas_set_input_to_propagate( link_list *link0, link_list *activelink )
{
  link_list  *link ;
  lotrs_list *lotrs ;
  lotrs_list *trspair ;
  ptype_list *ptype ;
  losig_list *activesig ; 

  activesig = activelink->ULINK.LOTRS->GRID->SIG ;
  
  for( link = link0 ; link ; link = link->NEXT ) {

    if( ( link->TYPE & CNS_EXT ) != CNS_EXT ) {
    
      lotrs = link->ULINK.LOTRS ;

      if( lotrs->GRID->SIG == activesig )
        continue ;

      if( !tas_set_input_to_propagate_lotrs( lotrs ) )
        break ;

      if( ( link->TYPE & CNS_SWITCH ) == CNS_SWITCH ) {
        ptype = getptype( lotrs->USER, TAS_TRANS_SWITCH );
        if( ptype ) {
          trspair = (lotrs_list*)ptype->DATA ;
          if( !tas_set_input_to_propagate_lotrs( trspair ) )
            break ;
        }
      }
    }
  }

  if( link )
    tas_clear_input_to_propagate( link0 );
}

/* determine, according to the input value required to propagate the 
   active branch, if the testlink branch can be propagate or not. */
int tas_check_if_input_propagate( link_list *testlink )
{
  link_list  *link ;
  lotrs_list *lotrs ;
  ptype_list *ptype ;
  long        level ;

  for( link = testlink ; link ; link = link->NEXT ) {
    
    if( ( link->TYPE & CNS_EXT ) != CNS_EXT ) {

      lotrs = link->ULINK.LOTRS ;

      ptype = getptype( lotrs->GRID->SIG->USER, TAS_SIGNAL_LEVEL ) ;
      if( ptype ) {

        level = (long)ptype->DATA ;

        if( level == 0 && MLO_IS_TRANSN( lotrs->TYPE ) )
          return 0 ;

        if( level == 1 && MLO_IS_TRANSP( lotrs->TYPE ) )
          return 0 ;
      }
    }
  }

  return 1 ;
}

/******************************************************************************/

/*-----------------------------------------------------------------------------
*                    tas_getconflictparam()                                   *
*                                                                             *
* extrait les parametres de la capacite de conflit: capa = p0 + p1 * fin      *
-----------------------------------------------------------------------------*/
void
tas_getconflictparam(cone0, input_grid0, type_branche, type_transistor,
                     maillona, maillonc, p0, p1, pqy, gate_th, vtc, vtc0, cgpc,
                     cgdc, cgpoc, cgdce, brconf, event )
cone_list *cone0;
cone_list *input_grid0;
long type_branche;
char type_transistor;
link_list *maillona;
link_list *maillonc;
double *p0;
double *p1;
double *pqy;
double *gate_th;
double *vtc;
double *vtc0;
double *cgpc;
double *cgdc;
double *cgpoc;
double *cgdce;
branch_list **brconf ;
char event;
{
  branch_list *chemin0[3];
  branch_list *conflbranch ;
  link_list *maillon0, *maillon1, *maillon0sav;
  int index = 1;
  double capa = 0.0,
      capa1 = 0.0,
      capa2 = 0.0,
      capai = 0.0, QX = 0.0, QY = 0.0, Yeq = 0.0, Yc = 0.0, seuil =
      0.0, bn, bp, vddmax, imax, vtn, vtp, kp, kn, rt, r2 = 0.0, wl =
      0.0, an, ap, AX, BX;
  long ct;
  short i;
  double sum_cgp, sum_cgd, sum_cgpo, c, ci, sum_cgde[3] ;
  double rap;
  chain_list *chainmarklotrscapai;
  chain_list *chain;
  lotrs_list *lotrs;
  double rconf, ractv;
  char notonswitch ;
  char usenewswitchmodel ;
  int ignored ;
  float rlin, rlinmin ;

  usenewswitchmodel =  V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ;

  notonswitch     = usenewswitchmodel ;
  
  *p0 = 0.0;
  *p1 = 0.0;
  *pqy = 0.0;
  *vtc = -1.0;
  if( vtc0 )
    *vtc0 = -1.0;
  if (cgpc)
    *cgpc = -1.0;
  if (cgdc)
    *cgdc = -1.0;
  if (cgpoc)
    *cgpoc = -1.0;
  if( cgdce ) {
    for( i=0;i<3;i++)
      cgdce[i]=0.0;
  }
  sum_cgp = 0.0;
  sum_cgpo = 0.0;
  sum_cgd = 0.0;
  for( i=0;i<3;i++)
    sum_cgde[i]=0.0;

  if( brconf )
    *brconf = NULL ;

  CCUSED = 0L;
  if (maillonc == NULL)
    return;

  if ((maillonc->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
    *gate_th = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
  } else {
    vddmax = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    *gate_th =
        vddmax - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
  }

  chemin0[0] = cone0->BREXT;
  chemin0[1] = cone0->BRVDD;
  chemin0[2] = cone0->BRVSS;

  for (i = 0; i < 3; i++)
    for (; chemin0[i]; chemin0[i] = chemin0[i]->NEXT) {
      if (tas_is_switchdegraded_branch(chemin0[i]))
        continue;
      for (maillon0 = (link_list *) chemin0[i]->LINK; maillon0;
           maillon0 = maillon0->NEXT)
        if ((maillon0->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
          if (getptype(maillon0->ULINK.LOTRS->USER, TAS_BTCONFL)
              != NULL)
            maillon0->ULINK.LOTRS->USER =
                delptype(maillon0->ULINK.LOTRS->USER, TAS_BTCONFL);
          if (getptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL)
              != NULL)
            maillon0->ULINK.LOTRS->USER =
                delptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL);
        }
    }
  chemin0[0] = cone0->BREXT;
  chemin0[1] = cone0->BRVDD;
  chemin0[2] = cone0->BRVSS;

  for (i = 0; i < 3; i++)
    for (; chemin0[i]; chemin0[i] = chemin0[i]->NEXT) {
      ct = chemin0[i]->TYPE;
      if (tas_is_switchdegraded_branch(chemin0[i]))
        continue;
      if (((ct & type_branche) == type_branche)
          && ((ct & (CNS_EXT | CNS_BLEEDER)) == 0))
        for (maillon0 = (link_list *) chemin0[i]->LINK; maillon0;
             maillon0 = maillon0->NEXT)
          if (((maillon0->TYPE & (CNS_IN | CNS_INOUT)) == 0L)
              && (getptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL)
                  == NULL))
            maillon0->ULINK.LOTRS->USER =
                addptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL, NULL);
      /*maillon marque conflictuel */
    }
  chemin0[0] = cone0->BREXT;
  chemin0[1] = cone0->BRVDD;
  chemin0[2] = cone0->BRVSS;

  for (i = 0; i < 3; i++)
    for (; chemin0[i]; chemin0[i] = chemin0[i]->NEXT) {
      ct = chemin0[i]->TYPE;
      if (tas_is_switchdegraded_branch(chemin0[i]))
        continue;
      if ((ct & type_branche) != type_branche)
        for (maillon0 = (link_list *) chemin0[i]->LINK; maillon0;
             maillon0 = maillon0->NEXT)
          if (((maillon0->TYPE & (CNS_IN | CNS_INOUT)) == 0L)
              && (getptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL)
                  != NULL))
            maillon0->ULINK.LOTRS->USER =
                delptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL);
      /*maillon marque conflictuel */
    }
  {
    chain_list *sw;
    ptype_list *paux = getptype(cone0->USER, CNS_SWITCH);
    if (paux != NULL)
      for (sw = (chain_list *) paux->DATA; sw; sw = sw->NEXT) {
        chain_list *tr = (chain_list *) sw->DATA;
        ((lotrs_list *) tr->DATA)->USER =
            addptype(((lotrs_list *) tr->DATA)->USER, TAS_BTCONFL, NULL);
        ((lotrs_list *) tr->NEXT->DATA)->USER =
            addptype(((lotrs_list *) tr->NEXT->DATA)->USER,
                     TAS_BTCONFL, NULL);
      }
  }

  if( notonswitch ) {
    if( maillona != maillonc ) {
      for( maillon0 = maillona->NEXT ; maillon0 ; maillon0 = maillon0->NEXT ) {
        if( !getptype( maillon0->SIG->USER, TAS_ONACTIVEPATH ) )
          maillon0->SIG->USER = addptype( maillon0->SIG->USER, TAS_ONACTIVEPATH, NULL );
      }
    }

    tas_set_input_to_propagate( maillona, maillonc );
  }

  chainmarklotrscapai = NULL;
  

  ignored = 0 ;
  rlinmin = -1.0 ;

  while ((conflbranch =
          existe_conflit(cone0, input_grid0, type_branche,
                         type_transistor, index, ignored, &maillon1))) {

    index++;

    maillon0 = conflbranch->LINK ;
    if( notonswitch && ! tas_check_if_input_propagate( maillon0 ) ) {
      ignored++;
      continue ;
    }

    if( brconf ) {
      rlin = tas_get_rlin_br( maillon0 );
      if( rlinmin < 0.0 || rlin < rlinmin ) {
        rlinmin = rlin ;
        *brconf = conflbranch ;
      }
    }

    *vtc = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
    if( vtc0 )
      *vtc0 = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT0);
    imax = tas_get_current(conflbranch, maillon0, 1.0, NO);
    maillon0sav = maillon0;

    if( maillon0 != maillon1 ) {
      if( !getptype( maillon0->NEXT->SIG->USER, TAS_ONACTIVEPATH ) ) {
        if( !getptype(maillon0->ULINK.LOTRS->USER, TAS_MARK_LOTRS_CAPAI)) {
          chainmarklotrscapai = addchain(chainmarklotrscapai, maillon0->ULINK.LOTRS);
          maillon0->ULINK.LOTRS->USER = addptype(maillon0->ULINK.LOTRS->USER, TAS_MARK_LOTRS_CAPAI, NULL);
          ci = tas_get_cgd_for_conflict_branch(NULL, maillon0);
          capa = capa + ci;
        }
      }
    }
    rconf = 0.0;
    ractv = tas_get_rlin(maillona);

    for (; maillon0 != maillon1; maillon0 = maillon0->NEXT) {

      if( ! getptype( maillon0->NEXT->SIG->USER, TAS_ONACTIVEPATH ) ) {
      

        rconf = rconf + tas_get_rlin(maillon0);

        if ((getptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL))
            && (getptype(maillon0->NEXT->ULINK.LOTRS->USER, TAS_UCONFL))) {

          rap = (rconf + ractv) / ractv;
          capa += ((double)
                   ((tas_getcapalink(NULL, maillon0->NEXT, event) +
                     TAS_GETCAPARA(maillon0->NEXT)) *
                    tlc_getcapafactor() +
                    tas_getcapabl(maillon0->NEXT))) / rap;

          ci = tas_get_cgd_for_conflict_branch(maillon0,
                                               maillon0->NEXT ==
                                               maillon1 ? NULL :
                                               maillon0->NEXT);
          capa = capa + ci / rap;


          maillon0->ULINK.LOTRS->USER =
              delptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL);
          if (!getptype(maillon0->ULINK.LOTRS->USER, TAS_BTCONFL))
            maillon0->ULINK.LOTRS->USER =
                addptype(maillon0->ULINK.LOTRS->USER, TAS_BTCONFL, NULL);
        }
      }
    }

    if (getptype(maillon1->ULINK.LOTRS->USER, TAS_UCONFL)) {

      /* if( !V_BOOL_TAB[ __TAS_USE_ALL_CAPAI ].VALUE ) */
      {
        c = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE,
                         TP_CGP) * 1000.0 *
            (double) TAS_GETWIDTH(maillon1) / SCALE_X;
        sum_cgp = sum_cgp + c;
        c = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGDC) *
             1000.0 *
             (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
             (double) tas_get_width_active(maillon1->ULINK.LOTRS,
                                           TAS_GETWIDTH(maillon1))) /
            (SCALE_X * SCALE_X);
        sum_cgd = sum_cgd + c;
      }
      c = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE,
                       TP_CGPOC) * 1000.0 *
          (double) TAS_GETWIDTH(maillon1) / SCALE_X;
      sum_cgpo = sum_cgpo + c;

      c  = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGDC0) * 1000.0 *
           (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
           (double) tas_get_width_active(maillon1->ULINK.LOTRS, TAS_GETWIDTH(maillon1))) /
          (SCALE_X * SCALE_X);
      sum_cgde[0] = sum_cgde[0] + c;
      c  = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGDC1) * 1000.0 *
           (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
           (double) tas_get_width_active(maillon1->ULINK.LOTRS, TAS_GETWIDTH(maillon1))) /
          (SCALE_X * SCALE_X);
      sum_cgde[1] = sum_cgde[1] + c;
      c  = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGDC2) * 1000.0 *
           (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
           (double) tas_get_width_active(maillon1->ULINK.LOTRS, TAS_GETWIDTH(maillon1))) /
          (SCALE_X * SCALE_X);
      sum_cgde[2] = sum_cgde[2] + c;

      QY = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGP) * 1000.0;
      QY +=
          (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGD) *
           1000.0 *
           (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
           (double) tas_get_width_active(maillon1->ULINK.LOTRS,
                                         TAS_GETWIDTH(maillon1)) /
           (double) TAS_GETWIDTH(maillon1)) / SCALE_X;
      QY *= 1.0;
      Yeq +=
          (double) TAS_GETWIDTH(maillon1) /
          (double) TAS_GETLENGTH(maillon1);
      maillon1->ULINK.LOTRS->USER =
          delptype(maillon1->ULINK.LOTRS->USER, TAS_UCONFL);
      if (getptype(maillon1->ULINK.LOTRS->USER, TAS_BTCONFL) == NULL)
        maillon1->ULINK.LOTRS->USER =
            addptype(maillon1->ULINK.LOTRS->USER, TAS_BTCONFL, NULL);

      capa2 = QY * (double) TAS_GETWIDTH(maillon1) / SCALE_X;
      capai += capa2;
    }
  }

  for (chain = chainmarklotrscapai; chain; chain = chain->NEXT) {
    lotrs = (lotrs_list *) chain->DATA;
    lotrs->USER = delptype(lotrs->USER, TAS_MARK_LOTRS_CAPAI);
  }
  freechain(chainmarklotrscapai);

  if( notonswitch )
    tas_clear_input_to_propagate( maillona );
  
  if (Yeq == 0.0)
    return;

  Yc = (double) TAS_GETWIDTH(maillonc) / (double) TAS_GETLENGTH(maillonc);

  if (TAS_CONTEXT->TAS_LEVEL == 0) {
    if ((type_transistor & CNS_TP) == CNS_TP) {
      bn = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_B);
      vddmax = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
      vtn = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      kp = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_K);
      kn = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_K);
      QX = TAS_F5(Yeq, Yc, kp, kn);
      seuil = ((tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT)
                +
                QX *
                (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
                 - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE,
                                TP_VT))) / (1.0 + QX));
      QX = seuil - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      QX = QX / (tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
                 - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT));
      capa1 = Yc * tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_Q)
          * TAS_F6(QX, bn * (vddmax - vtn))
          * (1000 /
             tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_SEUIL));
    } else {
      bp = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_B);
      vddmax = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
      vtp = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      kn = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_K);
      kp = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_K);
      QX = TAS_F5(Yc, Yeq, kp, kn);
      seuil = ((tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT)
                +
                QX *
                (tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
                 - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE,
                                TP_VT))) / (1.0 + QX));
      QX = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE,
                        TP_VDDmax) - seuil -
          tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      QX = QX /
          (tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax) -
           tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT));
      capa1 =
          Yc * tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE,
                            TP_Q) * TAS_F6(QX,
                                           bp * (vddmax -
                                                 vtp)) * (1000 /
                                                          tas_getparam
                                                          (maillonc->
                                                           ULINK.
                                                           LOTRS,
                                                           TAS_CASE,
                                                           TP_SEUIL));
    }
  } else {
    if ((type_transistor & CNS_TP) == CNS_TP) {
      //bn = tas_getparam (maillonc->ULINK.LOTRS, TAS_CASE, TP_B);
      AX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_A);
      BX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_B);
      vddmax = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
      vtn = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      vtp = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
      kp = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_K);
      rt = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_RT);
      kn = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_K);
      tas_getwlr2effdual(maillon1, rt, &wl, &r2);
      //tas_getresforconflit (maillon0sav, maillon1, &wl, &r2);
      ap = tas_get_an(r2, wl, AX, BX);
      bp = tas_get_bn(r2, wl, AX, BX);
      AX = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_A);
      BX = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_B);
      tas_getwlr2effdual(maillonc, rt, &wl, &r2);
      an = tas_get_an(r2, wl, AX, BX);
      bn = tas_get_bn(r2, wl, AX, BX);
      //QX = TAS_F5 (Yeq, Yc, kp, kn);
      QX = TAS_F5(1, 1, ap / (1 + bp), an / (1 + bn));
      seuil = tas_threshold(an, bn, vtn, ap, bp, vtp, vddmax);

      QX = seuil - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      QX = QX / (tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
                 - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT));
      capa1 =
          an * (vddmax - vtn) * (vddmax - vtn) / (1.0 +
                                                  bn * (vddmax - vtn))
          * TAS_F6(QX, bn * (vddmax - vtn))
          * (1000 /
             tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_SEUIL));
    } else {
      //bp = tas_getparam (maillonc->ULINK.LOTRS, TAS_CASE, TP_B);
      AX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_A);
      BX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_B);
      vddmax = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
      vtp = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      vtn = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
      kn = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_K);
      rt = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_RT);
      kp = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_K);
      tas_getwlr2effdual(maillon1, rt, &wl, &r2);
      //tas_getresforconflit (maillon0sav, maillon1, &wl, &r2);
      an = tas_get_an(r2, wl, AX, BX);
      bn = tas_get_bn(r2, wl, AX, BX);
      AX = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_A);
      BX = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_B);
      tas_getwlr2effdual(maillonc, rt, &wl, &r2);
      ap = tas_get_an(r2, wl, AX, BX);
      bp = tas_get_bn(r2, wl, AX, BX);
      //QX = TAS_F5 (Yc, Yeq, kp, kn);
      QX = TAS_F5(1, 1, ap / (1 + bp), an / (1 + bn));
      seuil = tas_threshold(an, bn, vtn, ap, bp, vtp, vddmax);
      QX = tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
          - seuil - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT);
      QX = QX / (tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
                 - tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_VT));
      capa1 =
          ap * (vddmax - vtp) * (vddmax - vtp) / (1.0 +
                                                  bp * (vddmax - vtp))
          * TAS_F6(QX, bp * (vddmax - vtp))
          * (1000 /
             tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_SEUIL));
    }
  }

  avt_log(LOGTAS, 4,
          "        capa_confl: seuil = %.2e Yc = %.2e Yeq = %.2e QX = %.2e\n",
          seuil, Yc, Yeq, QX);
  avt_log(LOGTAS, 4,
          "        capa_confl: cconf = %.2e cc = %.2e capa_tot = %.2e\n",
          capa1, capa2, capa);

  if (capa > 0.0)
    *p0 = capa;
  if (capa1 > 0.0)
    *p1 = capa1;
  if (capai > 0.0)
    *pqy = capai;
  *gate_th = seuil;

  if (cgdc)
    *cgdc = sum_cgd;
  if (cgpc)
    *cgpc = sum_cgp;
  if (cgpoc)
    *cgpoc = sum_cgpo;
  if( cgdce )
    for(i=0 ; i<3 ; i++ )
      cgdce[i]=sum_cgde[i];

  if( notonswitch ) {
    for( maillon0 = maillona->NEXT ; maillon0 ; maillon0 = maillon0->NEXT ) {
      maillon0->SIG->USER = testanddelptype( maillon0->SIG->USER, TAS_ONACTIVEPATH );
    }
  }
}

double
tas_get_cgd_for_conflict_branch(link_list * link_source,
                                link_list * link_drain)
{
  double r;
  double vdd;
  double vt;
  double cgd;
  double cgdc;
  double ci;
  double ct;

  ct = 0.0;

  if (link_source) {
    r = 1000.0 *
        (double) tas_get_length_active(link_source->ULINK.LOTRS) *
        (double) tas_get_width_active(link_source->ULINK.LOTRS,
                                      TAS_GETWIDTH(link_source)) /
        (SCALE_X * SCALE_X);
    vdd = tas_getparam(link_source->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    vt = tas_getparam(link_source->ULINK.LOTRS, TAS_CASE, TP_VT);

    cgd = tas_getparam(link_source->ULINK.LOTRS, TAS_CASE, TP_CGSI) * r;
    cgdc = tas_getparam(link_source->ULINK.LOTRS, TAS_CASE, TP_CGSIC) * r;

    ci = tas_get_cgd(cgd, cgdc, vdd, 0.0, vt, vdd, vdd / 2.0, 0.0,
                     vdd / 2.0);
    ct = ct + ci;
  }

  if (link_drain) {
    r = 1000.0 *
        (double) tas_get_length_active(link_drain->ULINK.LOTRS) *
        (double) tas_get_width_active(link_drain->ULINK.LOTRS,
                                      TAS_GETWIDTH(link_drain)) /
        (SCALE_X * SCALE_X);
    vdd = tas_getparam(link_drain->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    vt = tas_getparam(link_drain->ULINK.LOTRS, TAS_CASE, TP_VT);

    cgd = tas_getparam(link_drain->ULINK.LOTRS, TAS_CASE, TP_CGD) * r;
    cgdc = tas_getparam(link_drain->ULINK.LOTRS, TAS_CASE, TP_CGDC) * r;

    ci = tas_get_cgd(cgd, cgdc, vdd, 0.0, vt, vdd, vdd / 2.0, 0.0,
                     vdd / 2.0);
    ct = ct + ci;
  }
  return ct;
}

double
tas_threshold(double an, double bn, double vtn, double ap, double bp,
              double vtp, double vdd)
{
  double vmin, vmax, v;
  double ip, in;
  int iter;

  vmin = vtn;
  vmax = vdd - vtp;
  iter = 100;

  if (vmin > vmax)
    return vdd / 2.0;

  do {
    v = (vmax + vmin) / 2.0;
    in = an * (v - vtn) * (v - vtn) / (1.0 + bn * (v - vtn));
    ip = ap * (vdd - v - vtp) * (vdd - v - vtp) / (1.0 +
                                                   bp * (vdd - v - vtp));
    if (in > ip)
      vmax = v;
    else
      vmin = v;
    iter--;
  }
  while (fabs(ip - in) / ip > 0.001 && iter);

  return (vmax + vmin) / 2.0;
}

/*-----------------------------------------------------------------------------
*                    capa_eqparam()                                           *
*                                                                             *
* calcule pour chaque maillon les paramètres ci et cf de la capacite          *
* equivalente pour la branche qui le separe de la sortie pour good et false   *
-----------------------------------------------------------------------------*/
void
capa_eqparam(link0, srcil, srl, srcih, srh, tab0, tabi, tabn, t0, ti, tn)
link_list *link0;
double srcil;
double srl;
double srcih;
double srh;
float *tab0;
float *tabi;
float *tabn;
char *t0;
char *ti;
char *tn;

{
  double rt = 0.0;
  long cpt = 0;
  link_list *ptlink;
  int index;

  if (!link0)
    return;

  /* capa intrinseque du premier maillon */
  tab0[STM_CI_0] = (float) (( /*link0->CAPA + */ TAS_GETCAPARA(link0)) *
                            tlc_getcapafactor() + tas_getcapabl(link0));
  *t0 = 1;

  if (!getptype(link0->ULINK.LOTRS->USER, TAS_BTCONFL) && !CCUSED) {
    /* flag de conflit */
    tab0[STM_CF_0] = (float) 1;
    CCUSED = 1L;
  } else
    tab0[STM_CF_0] = (float) (-1);

  /*capa = ci0 + pconf1 * fcarac + ccarac */

  /* capa intrinseque de chaque maillon (a partir du deuxieme) et
   * calcul des paramètres de capa ramenée acti et bcti*/
  ptlink = link0->NEXT;
  if (ptlink) {
    while (ptlink->NEXT) {
      if (ptlink->TYPE & (CNS_IN | CNS_INOUT))
        break;

      *ti = 1;
      index = cpt * 7 + 1;
      rt = tas_getparam(link0->ULINK.LOTRS, TAS_CASE, TP_RT);
      if ((link0->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
        /* acti */
        tabi[index + STM_ACTI_I] =
            (float) (srcih / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        /* bcti */
        tabi[index + STM_BCTI_I] =
            (float) (srh / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        /* acti + bcti * ccarac */
      } else {
        tabi[index + STM_ACTI_I] =
            (float) (srcil / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        tabi[index + STM_BCTI_I] =
            (float) (srl / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
      }

      /* capa intrinseque */
      tabi[index + STM_CI_I] =
          (float) (((tas_getcapalink(NULL, ptlink, TAS_UNKNOWN_EVENT)
                     +
                     TAS_GETCAPARA(ptlink)) * tlc_getcapafactor()) +
                   tas_getcapabl(ptlink));

      if (!getptype(ptlink->ULINK.LOTRS->USER, TAS_BTCONFL)
          && !CCUSED) {
        /* flag de conflit */
        tabi[index + STM_CF_I] = (float) 1;
        CCUSED = 1L;
      } else
        tabi[index + STM_CF_I] = (float) (-1);
      /*capa = ci1 + pconf1 * fcarac + ((acti + bcti * ccarac) * capa) / (acti + bcti * ccarac + capa) */
      cpt++;
      ptlink = ptlink->NEXT;
      link0 = link0->NEXT;
      tabi[STM_NB_I_LINKS] = (float) cpt;
    }
    /* link n */
    while (ptlink) {
      if (ptlink->TYPE & (CNS_IN | CNS_INOUT))
        break;

      *tn = 1;
      rt = tas_getparam(link0->ULINK.LOTRS, TAS_CASE, TP_RT);
      if ((link0->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
        /* acti */
        tabn[STM_ACTI_N] =
            (float) (srcih / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        /* bcti */
        tabn[STM_BCTI_N] =
            (float) (srh / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        /* acti + bcti * ccarac */
      } else {
        tabn[STM_ACTI_N] =
            (float) (srcil / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        tabn[STM_BCTI_N] =
            (float) (srl / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
      }
      /* capa intrinseque */
      tabn[STM_CI_N] =
          (float) (((tas_getcapalink(NULL, ptlink, TAS_UNKNOWN_EVENT)
                     +
                     TAS_GETCAPARA(ptlink)) * tlc_getcapafactor()) +
                   tas_getcapabl(ptlink));

      if (!getptype(ptlink->ULINK.LOTRS->USER, TAS_BTCONFL)
          && !CCUSED) {
        /* flag de conflit */
        tabn[STM_CF_N] = (float) 1;
        CCUSED = 1L;
      } else
        tabn[STM_CF_N] = (float) (-1);
      /*capa = ci1 + pconf1 * fcarac + ((acti + bcti * ccarac) * capa) / (acti + bcti * ccarac + capa) */
      ptlink = ptlink->NEXT;
      link0 = link0->NEXT;
    }
  }
}

/*-----------------------------------------------------------------------------
*                    capa_eqparampath()                                       *
*                                                                             *
* calcule pour chaque maillon les paramètres ci et cf de la capacite          *
* equivalente pour la branche qui le separe de la sortie pour tpd_path        *
-----------------------------------------------------------------------------*/
void capa_eqparampath(link0, srcil, srl, srcih, srh, tab0, tabi, t0, ti)
link_list *link0;
double srcil;
double srl;
double srcih;
double srh;
float *tab0;
float *tabi;
char *t0;
char *ti;
{
  double rt = 0.0;
  long cpt = 0;
  link_list *ptlink;
  int index;

  if (!link0)
    return;

  /* capa intrinseque du premier maillon */
  tab0[STM_CI_0] = (float) (( /*link0->CAPA + */ TAS_GETCAPARA(link0)) *
                            tlc_getcapafactor() + tas_getcapabl(link0));
  *t0 = 1;

  if (!getptype(link0->ULINK.LOTRS->USER, TAS_BTCONFL) && !CCUSED) {
    /* flag de conflit */
    tab0[STM_CF_0] = (float) 1;
    CCUSED = 1L;
  } else
    tab0[STM_CF_0] = (float) (-1);

  /*capa = ci0 + pconf1 * fcarac + ccarac */

  /* capa intrinseque de chaque maillon (a partir du deuxieme) et
   * calcul des paramètres de capa ramenée acti et bcti*/
  ptlink = link0->NEXT;
  if (ptlink) {
    while (ptlink) {
      if (ptlink->TYPE & (CNS_IN | CNS_INOUT))
        break;

      *ti = 1;
      index = cpt * 7 + 1;
      rt = tas_getparam(link0->ULINK.LOTRS, TAS_CASE, TP_RT);
      if ((link0->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
        /* acti */
        tabi[index + STM_ACTI_I] =
            (float) (srcih / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        /* bcti */
        tabi[index + STM_BCTI_I] =
            (float) (srh / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        /* acti + bcti * ccarac */
      } else {
        tabi[index + STM_ACTI_I] =
            (float) (srcil / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
        tabi[index + STM_BCTI_I] =
            (float) (srl / rt * (double) TAS_GETWIDTH(link0) /
                     (double) TAS_GETLENGTH(link0) * 1000);
      }

      /* capa intrinseque */
      tabi[index + STM_CI_I] =
          (float) (((tas_getcapalink(NULL, ptlink, TAS_UNKNOWN_EVENT)
                     +
                     TAS_GETCAPARA(ptlink)) * tlc_getcapafactor()) +
                   tas_getcapabl(ptlink));

      if (!getptype(ptlink->ULINK.LOTRS->USER, TAS_BTCONFL)
          && !CCUSED) {
        /* flag de conflit */
        tabi[index + STM_CF_I] = (float) 1;
        CCUSED = 1L;
      } else
        tabi[index + STM_CF_I] = (float) (-1);
      /*capa = ci1 + pconf1 * fcarac + ((acti + bcti * ccarac) * capa) / (acti + bcti * ccarac + capa) */
      cpt++;
      ptlink = ptlink->NEXT;
      link0 = link0->NEXT;
      tabi[STM_NB_I_LINKS] = (float) cpt;
    }
  }
}

int tas_calc_k3_k4_k5(link_list *maillonc, char type_front, float *k3, float *k4, float *k5, float *diffv)
{
  double lw;
  double a, b, vt, vt1, rdd, rud, kdd, kud, rpo, rno, rt, rdf, ruf, vmax, vdeg, seuil, kt, b1, vgate;
  lotrs_list *lotrs = maillonc->ULINK.LOTRS;
  
  vgate = tas_get_vdd_input (maillonc);
  vmax = tas_getparam(lotrs, TAS_CASE, TP_VDDmax);
  if(!V_BOOL_TAB[ __AVT_OLD_FEATURE_28 ].VALUE && (vgate > vmax)){
    *diffv = vgate - vmax;
  }else{
    *diffv = 0.0;
  }
  seuil = tas_getparam(lotrs, TAS_CASE, TP_SEUIL);
  vdeg = tas_getparam(lotrs, TAS_CASE, TP_deg);
  vt = tas_getparam(lotrs, TAS_CASE, TP_VT);
  a = tas_getparam(lotrs, TAS_CASE, TP_A);
  b = tas_getparam(lotrs, TAS_CASE, TP_B);
  kt = tas_getparam(lotrs, TAS_CASE, TP_KT);
  rt = tas_getparam(lotrs, TAS_CASE, TP_RT);
  
  lw = (double) TAS_GETLENGTH(maillonc) / (double) TAS_GETWIDTH(maillonc);
  if ((maillonc->TYPE & CNS_SWITCH) == CNS_SWITCH)
    lw /= tas_get_current_rate(maillonc);
  if (type_front == 'D') {
/*----------------------------------------*/      
    if(MLO_IS_TRANSN(lotrs->TYPE))
      rdd = (vmax-seuil)*(b+(1.0/(vmax-vt)))/(a*(vmax-vt)) ;
    else if(MLO_IS_TRANSP(lotrs->TYPE)){
      if(kt > 0.0)
          vt1 = vt;
      else
          vt1 = (vt + vdeg)/2.0;
      b1 = b*(vmax-vt1);
      rdd = vmax*(kt+1.0) / (vmax*(1.0-kt)-vt1-vt1);
      rdd += b1 * log((2*(vmax-vt1))/(vmax*(1.0-kt)-vt1-vt1));
      rdd /= (a*(vmax-vt1));
      rdd *= (kt+1.0);
    }
    if(MLO_IS_TRANSP(lotrs->TYPE)){
      rdf = rdd ;
    }
    else if(MLO_IS_TRANSN(lotrs->TYPE)){
      rdf = rt ;
    }
    if(MLO_IS_TRANSP(lotrs->TYPE)){
      if(kt > 0.0)
          vt1 = vt;
      else
          vt1 = (vt + 2*vdeg)/3.0 ;
      b1 = b*(vmax-vt1);
      rpo = (rt*(2*(vmax-vt1) + b1*(vmax*(1.0-kt)-2*vt1))) / ((vmax*(1.0-kt)-2*vt1)*(1.0+b1)) ;
      rdd = (rdd + rpo) / 2.0; 
      rpo = rdd * 1.0; 
      rdd = rdd * 1.3; 
      kdd = rdd / rpo;
    }
    else if(MLO_IS_TRANSN(lotrs->TYPE)){
      b1=b*(vmax-vt);
      kdd = b1/(b1-1.0);
      kdd *= log((2*b1)/(b1+1.0));
      kdd -= 0.5*log(3.0-(4.0*vt/vmax)) ;
    }
/*----------------------------------------*/      
     
    if (!finite (rdd = (float) (rdd * lw)))
      return 0;
    *k3 = rdd;
    if (!finite (kdd = (float) (kdd)))
      return 0;
    *k4 = kdd;
    *k5 = (float) (rdf * lw);
  } else {
/*----------------------------------------*/      
    if(MLO_IS_TRANSP(lotrs->TYPE))
      rud = seuil * (b+(1.0/(vmax-vt)))/(a*(vmax-vt)) ;
    else if(MLO_IS_TRANSN(lotrs->TYPE)){
      if(kt > 0.0)
          vt1 = vt - *diffv;
      else
          vt1 = (vmax + vt - vdeg + *diffv)/2.0;
      b1=b*(vmax-vt1);
      rud = vmax*(kt+1.0) / (vmax*(1.0-kt)-vt1-vt1);
      rud += b1 * log((2*(vmax-vt1))/(vmax*(1.0-kt)-vt1-vt1));
      rud /= (a*(vmax-vt1)) ;
      rud *= (kt+1.0);
    }
    if(MLO_IS_TRANSP(lotrs->TYPE)){
      ruf = rt ;
    }
    else if(MLO_IS_TRANSN(lotrs->TYPE)){
      ruf = rud ;
    }
    if(MLO_IS_TRANSP(lotrs->TYPE)){
      b1=b*(vmax-vt);
      kud = b1/(b1-1.0);
      kud *= log((2*b1)/(b1+1.0));
      kud -= 0.5*log(3.0-(4.0*vt/vmax)) ;
    }
    else if(MLO_IS_TRANSN(lotrs->TYPE)){
      if(kt > 0.0)
          vt1 = vt - *diffv;
      else
          vt1 = (vt + 2*(vmax-vdeg+ *diffv))/3.0 ;
      b1 = b*(vmax-vt1);
      rno = (rt*(2*(vmax-vt1) + b1*(vmax*(1.0-kt)-2*vt1))) / ((vmax*(1.0-kt)-2*vt1)*(1.0+b1)) ;
      rud = (rud + rno) / 2.0;
      rno = rud * 1.0;
      rud = rud * 1.3;
      kud = rud / rno;
    }
/*----------------------------------------*/      
    if (!finite (rud = (float) (rud * lw)))
      return 0;
    *k3 = rud;
    if (!finite (kud = (float) (kud)))
      return 0;
    *k4 = kud;
    *k5 = (float) (ruf * lw);
  }
  
  return 1;
}

/*-----------------------------------------------------------------------------
*                    tpd_pathparams()                                         *
*                                                                             *
* calcule les paramètres k3, k4 et k5 de la chaine pass-transistors           *
-----------------------------------------------------------------------------*/
int tpd_pathparams(cone, maillon0, type_front, tab0, tabi, tabn)
cone_list *cone;
link_list *maillon0;
char type_front;
float *tab0;
float *tabi;
float *tabn;

{
  link_list *maillonc;
  long nombre, p = 0L;
  int index;
  float k3,k4,k5,diffv;



  if (maillon0 == NULL) {
    tab0[STM_K3_0] = (float) (-1);
    tab0[STM_K4_0] = (float) (-1);
    tab0[STM_K5_0] = (float) (-1);
    return 1;
  }

  for (maillonc = maillon0, nombre = 0; maillonc != NULL;
       maillonc = maillonc->NEXT, nombre++);

  while (nombre > 1) {
    maillonc = maillon0;
    p = nombre;
    while (p > 1) {
      maillonc = maillonc->NEXT;
      p--;
    }
    nombre--;
    index = 1 + (nombre - 1) * STM_NB_LINK_I_PARAMS;
    if (!tas_calc_k3_k4_k5(maillonc, type_front, &k3, &k4, &k5, &diffv)) return 0;

    tabi[index + STM_K3_I] = k3;
    tabi[index + STM_K4_I] = k4;
    tabi[index + STM_K5_I] = k5;
  }
  maillonc = maillon0;
  if (!tas_calc_k3_k4_k5(maillonc, type_front, &k3, &k4, &k5, &diffv)) return 0;
  tab0[STM_K3_0] = k3;
  tab0[STM_K4_0] = k4;
  tab0[STM_K5_0] = k5;

  if (diffv > 0.0){
      if (!getptype(cone->USER, TAS_VDD_NOTDEG))
          cone->USER = addptype(cone->USER, TAS_VDD_NOTDEG, NULL);
          *(float*)&cone->USER->DATA = diffv;
  }

  tabn = NULL;
  return 1;
}

/* FIN de tpd_pathparams()    */

/*-----------------------------------------------------------------------------
*                    tas_get_vdd_path()                                       *
*                                                                             *
-----------------------------------------------------------------------------*/
double tas_get_vdd_path(maillon0)
link_list *maillon0;
{
  return tas_getparam(maillon0->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
}

/*-----------------------------------------------------------------------------
*                    tas_get_vdd_input()                                      *
*                                                                             *
-----------------------------------------------------------------------------*/
double tas_get_vdd_input(activelink)
link_list *activelink;
{
  cone_list *cone_avant;
//    ptype_list *ptype;
  alim_list *power;
  float alim;

  cone_avant =
      (cone_list *) getptype(activelink->ULINK.LOTRS->USER,
                             CNS_DRIVINGCONE)->DATA;

  power = cns_get_multivoltage(cone_avant);
  if (power) {
    if (TAS_PATH_TYPE == 'M')
      alim = power->VDDMIN - power->VSSMAX;
    else
      alim = power->VDDMAX - power->VSSMIN;
  } else {
    alim = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  }

  return (double) alim;
}

/*-----------------------------------------------------------------------------
*                    tas_get_vf_input()                                       *
*                                                                             *
-----------------------------------------------------------------------------*/
double tas_get_vf_input(activelink)
link_list *activelink;
{
  cone_list *cone_avant;
//    ptype_list *ptype;
  alim_list *power;
  float alim, vf;
  ptype_list *ptype;

  cone_avant =
      (cone_list *) getptype(activelink->ULINK.LOTRS->USER,
                             CNS_DRIVINGCONE)->DATA;

  power = cns_get_multivoltage(cone_avant);
  if (power) {
    if (TAS_PATH_TYPE == 'M')
      alim = power->VDDMIN - power->VSSMAX;
    else
      alim = power->VDDMAX - power->VSSMIN;
  } else {
    alim = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  }


  if ((activelink->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
    if ((cone_avant->TECTYPE & CNS_VSS_DEGRADED) != CNS_VSS_DEGRADED)
      vf = alim;
    else
      vf = alim - tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_deg);
  } else {
    if ((cone_avant->TECTYPE & CNS_VDD_DEGRADED) != CNS_VDD_DEGRADED)
      vf = alim;
    else{
      vf = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_deg);
      if((ptype = getptype(cone_avant->USER, TAS_VDD_NOTDEG)) != NULL){
          vf += *(float*)(&ptype->DATA);
      }

    }
  }
  return (double) vf;
}

/*-----------------------------------------------------------------------------
*                    affich_conec()                                           *
*                                                                             *
* fonction d'affichage du temps pour un connecteur HR :f                      *
-----------------------------------------------------------------------------*/
/*int affiche_conec (cone0, conec, type, valeur)
cone_list *cone0;
locon_list *conec;
char *type;
long valeur;

{
	if (valeur == TAS_NOTIME)
		printf ("---tas--- affich_conec	: cone(%ld) conecteur( %s ) %s = TAS_NOTIME\n", cone0->INDEX, conec->NAME,
				type);
	else
		printf ("---tas--- affich_conec	: cone(%ld) conecteur( %s ) %s = %ld\n", cone0->INDEX, conec->NAME, type,
				valeur);

	return (0);
}*/

/*****************************************************************************/
/*                             fonction GiveActivExtPath()                   */
/* renvoie la branche externe la mieux placee pour commuter.                 */
/*****************************************************************************/
branch_list *tas_GiveActivExtPath(cone, locon, level)
cone_list *cone;
locon_list *locon;
char level;

{
  branch_list *path;

  for (path = cone->BREXT; (path != NULL); path = path->NEXT){
    if((level == 'U') && ((path->TYPE & CNS_NOT_UP) == CNS_NOT_UP)) continue;
    if((level == 'D') && ((path->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN)) continue;
    if ((path->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL) {
      link_list *link;

      if ((path->TYPE & TAS_SWITCH) != TAS_SWITCH) {
        for (link = (link_list *) path->LINK; link->NEXT != NULL;
             link = link->NEXT);
        if (link->ULINK.LOCON == locon)
          break;
      } else {                  /* si branche contenant des switches-cmos */
        char prevtype = '\0';

        for (link = (link_list *) path->LINK; link->NEXT != NULL;
             link = link->NEXT) {
          if ((link->TYPE & CNS_SWITCH) == CNS_SWITCH) {
            if ((((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
                 && (prevtype == 'P'))
                || (((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
                    && (prevtype == 'N')))
              break;

            if ((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
              prevtype = 'N';
            else
              prevtype = 'P';
          }                     /* fin du maillon switch */
        }                       /* fin de parcours des maillons */

        if ((link->NEXT == NULL) && (link->ULINK.LOCON == locon))
          if (((prevtype == 'P') && (level == 'U'))
              || ((prevtype == 'N') && (level == 'D')))
            break;
      }                         /* fin des branches contenant des switches-cmos */
    }
  }

  if (path == NULL)
    return (NULL);
  else
    return (((path->TYPE & CNS_EXT) == CNS_EXT) ? path : NULL);
}

/*-----------------------------------------------------------------------------
*                    tas_getfirstdual()                                       *
*                                                                             *
* renvoie le 1er link de la partie duale de tpd_transgood                     *
-----------------------------------------------------------------------------*/
link_list *tas_getfirstdual(link0, activelink)
link_list *link0, *activelink;
{
  link_list *ptlink, *duallink;
  char type;

  if (link0 != activelink) {
    ptlink = link0;
    while (ptlink != activelink) {

      if ((ptlink->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
        type = CNS_TP;
      else
        type = CNS_TN;

      duallink = ptlink;

      for (;
           (ptlink->ULINK.LOTRS->TYPE & type) == type
           && ptlink != activelink; ptlink = ptlink->NEXT);
    }
    if ((activelink->ULINK.LOTRS->TYPE & type) != type)
      duallink = activelink;
  } else
    duallink = link0;

  return duallink;
}

/*-----------------------------------------------------------------------------
*                     tas_splitbranch()                                       *
*                                                                             *
* separe la partie duale de la partie pass transistor pour tpd_transgood      *
-----------------------------------------------------------------------------*/
link_list *tas_splitbranch(link_list * link0, link_list * duallink)
{
  link_list *preduallink, *ptlink = link0;

  if (ptlink != duallink) {
    while (ptlink->NEXT != duallink)
      ptlink = ptlink->NEXT;
    preduallink = ptlink;
    ptlink->NEXT = NULL;
    return preduallink;
  } else
    return NULL;
}

/*-----------------------------------------------------------------------------
*                    tas_getpathlink()                                        *
*                                                                             *
* renvoie le 1er link de la chaine de pass transistors                        *
-----------------------------------------------------------------------------*/
link_list *tas_getpathlink(link_list * link0, link_list * duallink)
{
  if (link0 != duallink)
    return link0;
  else
    return NULL;
}

/*-----------------------------------------------------------------------------
*                    tas_getcapaparamsgood()                                  *
*                                                                             *
* renvoie les parametres de la capa intrinseque capai et irap pour tpd_good   *
-----------------------------------------------------------------------------*/
void tas_getcapaparamsgood(branch, duallink, activelink, imax, QY, capai, irap)
branch_list *branch ;
link_list *duallink, *activelink;
double imax;
double QY;
double *capai;
double *irap;
{
  link_list *ptlink;

  *irap = 0.0;
  *capai = 0.0;

  ptlink = duallink;
  while (ptlink != activelink) {
    ptlink = ptlink->NEXT;

    *capai +=
        (((tas_getcapalink(NULL, ptlink, TAS_UNKNOWN_EVENT) +
           TAS_GETCAPARA(ptlink)) * tlc_getcapafactor() +
          tas_getcapabl(ptlink) +
          (QY * (double) TAS_GETWIDTH(ptlink)) / SCALE_X));

    if ((getptype(ptlink->ULINK.LOTRS->USER, TAS_BTCONFL) == NULL)
        && (CCUSED == 0L)) {
      *irap = imax / tas_get_current( branch, ptlink, 1.0, NO );
      CCUSED = 1L;
    }
  }
}

/*-----------------------------------------------------------------------------
*                       tpd_trans_goodmodel()                                 *
-----------------------------------------------------------------------------*/
timing_scm *tpd_trans_goodmodel(cone, branch, activelink, pconf0, pconf1,
                                srcil, srl, srcih, srh, delayrc, fin, capa,
                                fb, gate_th)
cone_list *cone;
branch_list *branch ;
link_list *activelink;
double pconf0;
double pconf1;
double srcil;
double srl;
double srcih;
double srh;
float delayrc;
float fin;
float capa;
float fb;
double gate_th;
{
  link_list *link0 ;
  double AX = 0.0,
      BX = 0.0,
      rx = 0.0,
      rtx = 0.0,
      QY = 0.0,
      QX = 0.0,
      vddmax = 0.0,
      wl = 0.0,
      r2 = 0.0,
      imax = 0.0,
      an = 0.0,
      bn = 0.0,
      VT = 0.0,
      vddin = 0.0, thr = 0.0, capai = 0.0, Rbr = 0.0, Cbr = 0.0, irap =
      0.0;
  link_list *ptlink, *preduallink, *pathlink, *duallink;
  char t0 = 0;
  char ti = 0;
  char tn = 0;
  float *ptab0;
  float *ptabi;
  float *ptabn;
  float tab0[STM_NB_LINK_0_PARAMS];
  float tabi[1024];
  float tabn[STM_NB_LINK_N_PARAMS];
  float tabd[STM_NB_DUAL_PARAMS];
  int good;

  link0 = branch->LINK ;
  /* premier link dual */
  duallink = tas_getfirstdual(link0, activelink);

  /* capa equivalente de la chaine pass */
  ptlink = duallink->NEXT;
  duallink->NEXT = NULL;
  /* calcul des paramètres permettant le calcul des capas de
   * la chaine de path transistors (4 params par transistor)*/
  capa_eqparam(link0, srcil, srl, srcih, srh, tab0, tabi, tabn, &t0, &ti,
               &tn);
  duallink->NEXT = ptlink;

  /* split de la chaine en parties pass et duale */
  preduallink = tas_splitbranch(link0, duallink);
  pathlink = tas_getpathlink(link0, duallink);

  /* partie duale */
  tas_getmcc( link0, 
              activelink, 
              &vddin, 
              &AX, 
              &BX, 
              &rtx, 
              &rx, 
              &VT, 
              NULL, 
              &QX, 
              &QY,
              &thr, 
              &vddmax, 
              NULL, 
              NULL, 
              NULL, 
              NULL
            );

  if (!tas_check_prop(activelink, vddmax, vddin, VT, gate_th)) {
    TAS_PVT_COUNT++;
    tas_error(70, cone->NAME, TAS_WARNING);
    /* recollage */
    if (pathlink)
      preduallink->NEXT = duallink;
    return NULL;
  }

  tas_getwlr2effgood(activelink, rx, &wl, &r2);
  tas_getRCbranch(activelink, &Rbr, &Cbr);
  an = tas_get_an(r2, wl, AX, BX);
  bn = tas_get_bn(r2, wl, AX, BX);

  imax = tas_get_current( branch, duallink, 1.0, NO );
  if (imax < 1e-8){
    TAS_PVT_COUNT++;
    tas_error(70, cone->NAME, TAS_WARNING);
    /* recollage */
    if (pathlink)
      preduallink->NEXT = duallink;
    return NULL;
  }

  /* calcul des paramètres de capa intrinseque
   * capa = capai + cconf / irap avec cconf=pconf0_cfb + pconf1 * fcarac */
  tas_getcapaparamsgood(branch, duallink, activelink, imax, QY, &capai, &irap);

  /* partie non duale, calcul des paramètres k3, k4, k5 */
  if ((activelink->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
    good = tpd_pathparams(cone, pathlink, 'U', tab0, tabi, tabn);
  else
    good = tpd_pathparams(cone, pathlink, 'D', tab0, tabi, tabn);

  if (!good) {
    TAS_PVT_COUNT++;
    tas_error(71, cone->NAME, TAS_WARNING);
    /* recollage */
    if (pathlink)
      preduallink->NEXT = duallink;
    return NULL;
  }

  /* recollage */
  if (pathlink)
    preduallink->NEXT = duallink;

  if (t0)
    fin = (fin + tab0[STM_K5_0] * (capa + pconf0 + pconf1 * fin) / 1000.0);
  pconf0 += (float) (fb * fin);

  tabd[STM_PCONF0] = (float) pconf0;
  tabd[STM_PCONF1] = (float) pconf1;
  tabd[STM_CAPAI] = (float) capai;
  tabd[STM_CAPAI0] = -1.0 ;
  tabd[STM_CAPAI1] = -1.0 ;
  tabd[STM_CAPAI2] = -1.0 ;
  tabd[STM_CAPAI3] = -1.0 ;
  tabd[STM_VT0]    = -1.0 ;
  tabd[STM_VT0C]   = -1.0 ;
  tabd[STM_CAPAO] = (float)capai;
  tabd[STM_IRAP] = (float) irap;
  tabd[STM_VDDIN] = (float) vddin;
  tabd[STM_VT] = (float) VT;
  tabd[STM_THRESHOLD] = (float) thr;
  tabd[STM_IMAX] = (float) imax;
  tabd[STM_AN] = (float) an;
  tabd[STM_BN] = (float) bn;
  tabd[STM_VDDMAX] = (float) vddmax;
  tabd[STM_RSAT] =
      (float) (tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_RS) /
               wl);
//      tabd[STM_RLIN]   = (float)(tas_getparam (activelink->ULINK.LOTRS, TAS_CASE, TP_RT) / wl);
  tabd[STM_RLIN] = (float) tas_get_rlin_br(link0);      /*rlin pour la branche */
  tabd[STM_DRC] = (float) delayrc;
  tabd[STM_RBR] = (float) Rbr;
  tabd[STM_CBR] = (float) Cbr;
  tabd[STM_INPUT_THR] =
      (float) (tas_get_vdd_input(activelink) * thr / vddmax);
  tabd[STM_RINT] = -1.0 ;
  tabd[STM_VINT] = -1.0 ;
  tabd[STM_CHALF] = 0.0 ;
  tabd[STM_RCONF] = -1.0 ;
  tabd[STM_KF]    = -1.0 ;
  tabd[STM_QINIT] = 0.0 ;
  tabd[STM_KRT] = tas_get_rlin_br_krt( link0 );

  ptab0 = t0 ? tab0 : NULL;
  ptabi = ti ? tabi : NULL;
  ptabn = tn ? tabn : NULL;

  return stm_modscm_good_create(ptab0, ptabi, ptabn, tabd);
}

/*-----------------------------------------------------------------------------
*                        tas_getfalseparam()                                  *
*                                                                             *
* extrait les parametres rtot et K de tpd_transfalse                          *
-----------------------------------------------------------------------------*/
int tas_getfalseparam(activelink, rtot, K)
link_list *activelink;
double *rtot;
double *K;
{
  double Rf;
  double Rg;
  link_list *ptlink;

  if ((activelink->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
    Rf = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_RDD);
  } else if ((activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
    Rf = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_RUD);
  }
  *rtot =
      Rf * (double) TAS_GETLENGTH(activelink) /
      (double) TAS_GETWIDTH(activelink);
  *K = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_KG);

  if (!finite(*rtot) || !finite(*K))
    return 0;

  for (ptlink = activelink->NEXT; ptlink != NULL; ptlink = ptlink->NEXT) {
    if ((ptlink->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
      if ((ptlink->ULINK.LOTRS->TYPE) == (activelink->ULINK.LOTRS->TYPE)) {
        *rtot +=
            Rf * (double) TAS_GETLENGTH(ptlink) /
            (double) TAS_GETWIDTH(ptlink);
      } else if ((ptlink->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
        Rg = tas_getparam(ptlink->ULINK.LOTRS, TAS_CASE, TP_RT);
        *rtot +=
            Rg * (double) TAS_GETLENGTH(ptlink) /
            (double) TAS_GETWIDTH(ptlink);
      }
    }
  }
  return 1;
}

/*-----------------------------------------------------------------------------
*                        tpd_trans_falsemodel()                               *
-----------------------------------------------------------------------------*/
timing_scm *tpd_trans_falsemodel(cone, link0, activelink, pconf0, pconf1,
                                 srcil, srl, srcih, srh, delayrc, fin,
                                 capa, fb)
cone_list *cone;
link_list *link0;
link_list *activelink;
double pconf0;
double pconf1;
double srcil;
double srl;
double srcih;
double srh;
float delayrc;
float fin;
float capa;
float fb;
{
  link_list *stucklink, *pathlink, *ptlink;
  double rtot = 0.0, K = 0.0, vdd = 0.0, vf = 0.0;
  char t0 = 0;
  char ti = 0;
  char tn = 0;
  float *ptab0;
  float *ptabi;
  float *ptabn;
  float tab0[STM_NB_LINK_0_PARAMS];
  float tabi[1024];
  float tabn[STM_NB_LINK_N_PARAMS];
  float tabf[STM_NB_FALSE_PARAMS];
  int good;

  ptlink = activelink->NEXT;
  activelink->NEXT = NULL;
  capa_eqparam(link0, srcil, srl, srcih, srh, tab0, tabi, tabn, &t0, &ti,
               &tn);
  activelink->NEXT = ptlink;

  /* split de la chaine en parties pass et duale */
  stucklink = tas_splitbranch(link0, activelink);
  pathlink = tas_getpathlink(link0, activelink);

  if (!tas_getfalseparam(activelink, &rtot, &K)) {
    TAS_PVT_COUNT++;
    tas_error(71, cone->NAME, TAS_WARNING);
    if (pathlink)
      stucklink->NEXT = activelink;
    return NULL;
  }

  vdd = tas_get_vdd_path(activelink);
  vf = tas_get_vf_input(activelink);

  if ((activelink->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
    good = tpd_pathparams(cone, pathlink, 'D', tab0, tabi, tabn);
  else
    good = tpd_pathparams(cone, pathlink, 'U', tab0, tabi, tabn);

  if (pathlink)
    stucklink->NEXT = activelink;

  if (!good) {
    TAS_PVT_COUNT++;
    tas_error(71, cone->NAME, TAS_WARNING);
    return NULL;
  }

  if (t0)
    fin = (fin + tab0[STM_K5_0] * (capa + pconf0 + pconf1 * fin) / 1000.0);
  pconf0 += (float) (fb * fin);

  tabf[STM_PCONF0_F] = (float) pconf0;
  tabf[STM_PCONF1_F] = (float) pconf1;
  tabf[STM_RTOT_F] = (float) rtot;
  tabf[STM_K_F] = (float) K;
  tabf[STM_VDDMAX_F] = (float) vdd;
  tabf[STM_DRC_F] = (float) delayrc;
  tabf[STM_VF_INPUT_F] = (float) vf;

  ptab0 = t0 ? tab0 : NULL;
  ptabi = ti ? tabi : NULL;
  ptabn = tn ? tabn : NULL;

  return stm_modscm_false_create(ptab0, ptabi, ptabn, tabf);
}

/*-----------------------------------------------------------------------------
*                        tpd_pathmodel()                                      *
-----------------------------------------------------------------------------*/
timing_scm *tpd_pathmodel(cone, link, type_front, cconf, srcil, srl, srcih, srh,
                          fin, capa)
cone_list *cone;
link_list *link;
char type_front;
double cconf;
double srcil;
double srl;
double srcih;
double srh;
float fin;
float capa;
{
  char t0 = 0;
  char ti = 0;
  char tn = 0;
  float *ptab0;
  float *ptabi;
  float *ptabn;
  float tab0[STM_NB_LINK_0_PARAMS];
  float tabi[1024];
  float tabn[STM_NB_LINK_N_PARAMS];
  float tabp[STM_NB_PATH_PARAMS];
  double vdd, vf;

  capa_eqparampath(link, srcil, srl, srcih, srh, tab0, tabi, &t0, &ti);

  if (!tpd_pathparams(cone, link, type_front, tab0, tabi, tabn))
    return NULL;

  /*tabp[STM_PCONF0_F] = (float)cconf; */
  if (t0)
    fin = (fin + tab0[STM_K5_0] * (capa + cconf * fin) / 1000.0);
  tabp[STM_PCONF0_F] = (float) (cconf * fin);

  vdd = tas_get_vdd_path(link);
  vf = tas_get_vdd_path(link);
  tabp[STM_VDDMAX_P] = (float) vdd;

  ptab0 = t0 ? tab0 : NULL;
  ptabi = ti ? tabi : NULL;
  ptabn = tn ? tabn : NULL;

  return stm_modscm_path_create(ptab0, ptabi, ptabn, tabp);
}

/*-----------------------------------------------------------------------------
*                    calcul_FB(chemin0,'U ou D')                              *
*                                                                             *
*    calcule pour le cone qui commence par ch0 la capa de FeedBack Up ou Dn   *
-----------------------------------------------------------------------------*/
double calcul_FB(cone, chemin0, commutation)
cone_list *cone;
branch_list *chemin0;
char commutation;
{
  branch_list *cheminc;
  link_list *maillonc;
  double Cch = 0.0, cm = 0.0, Cup = 0.0, Cdown = 0.0;
  int flag = 0;
  long w, l, fr;
  front_list *front;

  for (cheminc = chemin0; cheminc; cheminc = cheminc->NEXT) {
    if (tas_PathFonc(cone, cheminc) != 0) {
      Cch = 0.0;
      flag = 0;
      if ((((cheminc->TYPE & CNS_BLEEDER) == CNS_BLEEDER) ||
           ((cheminc->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)) &&
          ((cheminc->TYPE & CNS_NOTCONFLICTUAL) != CNS_NOTCONFLICTUAL)) {
        for (maillonc = (link_list *) cheminc->LINK; maillonc;
             maillonc = maillonc->NEXT) {
          cone_list *cone0 =
              (cone_list *) getptype(maillonc->ULINK.LOTRS->USER,
                                     CNS_DRIVINGCONE)->DATA;
          front = tas_getslope(cone, cone0);
          w = TAS_GETWIDTH(maillonc);
          l = TAS_GETLENGTH(maillonc);
          if ((cheminc->TYPE & CNS_VDD) == CNS_VDD) {
            fr = front->FUP;

            if (fr == TAS_NOFRONT)
              continue;

            cm = ((double) w / (double) l) /* * (double)fr */ *
                tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_IFB);
            if (!flag) {
              Cch = cm;
              flag = 1;
            } else
              Cch = (cm * Cch) / (cm + Cch);
          }
          if ((cheminc->TYPE & CNS_VSS) == CNS_VSS) {
            fr = front->FDOWN;

            if (fr == TAS_NOFRONT)
              continue;

            cm = ((double) w / (double) l) /* * (double)fr */ *
                tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_IFB);
            if (!flag) {
              Cch = cm;
              flag = 1;
            } else
              Cch = (cm * Cch) / (cm + Cch);
          }
        }
      }
      if ((cheminc->TYPE & CNS_VDD) == CNS_VDD)
        Cup += Cch;
      if ((cheminc->TYPE & CNS_VSS) == CNS_VSS)
        Cdown += Cch;
    }
  }
  if (chemin0) {
    Cdown *=
        2.0 / tas_getparam(chemin0->LINK->ULINK.LOTRS, TAS_CASE,
                           TP_VDDmax);
    Cup *=
        2.0 / tas_getparam(chemin0->LINK->ULINK.LOTRS, TAS_CASE,
                           TP_VDDmax);
  }

  avt_log(LOGTAS, 4, "        feedback: CRup = %f, CRdn = %f\n", Cup,
          Cdown);

  return ((commutation == 'U') ? Cup : Cdown);
}

/*----------------------------------------------------------------------------
*                    ComputeSlopes()                                         *
*                                                                            *
* rempli les champs RXX et FXX de la structure delay                         *
----------------------------------------------------------------------------*/
#ifdef USEOLDTEMP
char
ComputeSlopes(delay_list * delay0, cone_list * cone0, edge_list * input0,
              double capa)
{
  double res;
  char warres = 'N';
  front_list *slope;

  if (TAS_PATH_TYPE == 'M')
    slope = (front_list *) getptype(cone0->USER, TAS_SLOPE_MAX)->DATA;
  else
    slope = (front_list *) getptype(cone0->USER, TAS_SLOPE_MIN)->DATA;


  if (delay0->TPHH != TAS_NOTIME) {
    res = tas_GetRint(cone0, input0, CNS_VDD, CNS_TN);
    if (res < 1.0) {
      res = 1.0;
      warres = 'Y';
    }

#ifdef USEOLDTEMP
    delay0->RHH = (long) (res + 0.5);
#endif
/*		if ((cone0->TYPE & (CNS_LATCH | CNS_FLIP_FLOP)) != 0)
			delay0->FHH = slope->FUP;*/
  }

  if (delay0->TPLL != TAS_NOTIME) {
    res = tas_GetRint(cone0, input0, CNS_VSS, CNS_TP);
    if (res < 1.0) {
      res = 1.0;
      warres = 'Y';
    }
#ifdef USEOLDTEMP
    delay0->RLL = (long) (res + 0.5);
#endif
/*		if ((cone0->TYPE & (CNS_LATCH | CNS_FLIP_FLOP)) != 0)
			delay0->FLL = slope->FDOWN;*/
  }
  if (delay0->TPHL != TAS_NOTIME) {
    res = tas_GetRint(cone0, input0, CNS_VSS, CNS_TN);
    if (res < 1.0) {
      res = 1.0;
      warres = 'Y';
    }
#ifdef USEOLDTEMP
    delay0->RHL = (long) (res + 0.5);
#endif
/*		if ((cone0->TYPE & (CNS_LATCH | CNS_FLIP_FLOP)) != 0)
			delay0->FHL = slope->FDOWN;*/
  }
  if (delay0->TPLH != TAS_NOTIME) {
    res = tas_GetRint(cone0, input0, CNS_VDD, CNS_TP);
    if (res < 1.0) {
      res = 1.0;
      warres = 'Y';
    }
#ifdef USEOLDTEMP
    delay0->RLH = (long) (res + 0.5);
#endif
/*		if ((cone0->TYPE & (CNS_LATCH | CNS_FLIP_FLOP)) != 0)
			delay0->FLH = slope->FUP;*/
  }
  capa = 0.0;
  return warres;
}
#endif
/*----------------------------------------------------------------------------
*                    FitPrecharge()                                          *
----------------------------------------------------------------------------*/
void FitPrecharge(delay_list * delay0, cone_list * cone, edge_list * input)
{
  front_list *front;
  long delay;
  link_list *link;
  cone_list *conein;
  double seuil;
  double vt;
  double vdd;
  double fup, fdown;
  float csttab[2];
  timing_model *tmodel;
  char *name;
  char *modname;
  char buf[1024];


  if ((input->TYPE & CNS_EXT) != CNS_EXT) {
    conein = input->UEDGE.CONE;
    front = tas_getslope(cone, conein);
    fup =
        stm_thr2scm(front->FUP / TTV_UNIT, STM_DEFAULT_SMINR,
                    STM_DEFAULT_SMAXR, STM_DEFAULT_VTN,
                    V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_UP);
    fdown =
        stm_thr2scm(front->FDOWN / TTV_UNIT, STM_DEFAULT_SMINF,
                    STM_DEFAULT_SMAXF, STM_DEFAULT_VTP, 0.0,
                    V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, STM_DN);
  }
  else
    conein = NULL ;

  if ((delay0->TPHH == TAS_NOTIME) && (delay0->TPLH != TAS_NOTIME)) {
    delay = 0L;
    if (conein != NULL) {
      if (existe_tpd(cone, conein, CNS_VDD, CNS_TP, &link)) {
        vt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VT);
        vdd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
        seuil = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_SEUIL);
        delay =
            (long) ((stm_get_t(vt, vt, vdd, 0.0, fup) -
                     stm_get_t(seuil, vt, vdd, 0.0, fup)) * TTV_UNIT);
        if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        {
         tas_newname(buf, 0);
        }
        else
         strcpy(buf, stm_mod_name (conein->NAME, 'U', cone->NAME, 'U', STM_DELAY|STM_SLEW, TAS_PATH_TYPE, 0));
       
        modname=stm_createname(CELL, buf);
        delay0->TMHH = stm_mod_create (modname);
        tmodel = delay0->TMHH;
        stm_mod_update (tmodel, seuil, vdd, vt, vdd);
        stm_mod_update_transition (tmodel, STM_HH);
        delay0->TMHH->UTYPE = STM_MOD_MODSCM;
        csttab[0] = delay / TTV_UNIT;
        csttab[1] = 0.0;
        delay0->TMHH->UMODEL.SCM = stm_modscm_cst_create(csttab);
        name = stm_storemodel(CELL, NULL, delay0->TMHH, 0);
        if (name == delay0->TMHH->NAME)
            DETAILED_MODELS = addchain (DETAILED_MODELS, name);
        else {
            stm_mod_destroy(delay0->TMHH);
            delay0->TMHH = stm_getmodel(CELL, name);
        }
        delay0->FMHH = delay0->TMHH;
      }
    }

    delay0->TPHH = delay;
    delay0->FHH = 0L;
#ifdef USEOLDTEMP
    delay0->SHH = TAS_NOS;
#endif
#ifdef USEOLDTEMP
    delay0->RHH = TAS_NORES;
#endif
  }
  if ((delay0->TPLH == TAS_NOTIME) && (delay0->TPHH != TAS_NOTIME)) {
    delay = 0L;
    if (conein != NULL) {
      if (existe_tpd(cone, conein, CNS_VDD, CNS_TN, &link)) {
        vt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VT);
        vdd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
        seuil = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_SEUIL);
        delay =
            (long) ((stm_get_t(vt, vt, vdd, 0.0, fdown) -
                     stm_get_t(seuil, vt, vdd, 0.0, fdown)) * TTV_UNIT);
        if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        {
         tas_newname(buf, 0);
        }
        else
         strcpy(buf, stm_mod_name (conein->NAME, 'D', cone->NAME, 'U', STM_DELAY|STM_SLEW, TAS_PATH_TYPE, 0));
       
        modname=stm_createname(CELL, buf);
        delay0->TMLH = stm_mod_create (modname);
        tmodel = delay0->TMLH;
        stm_mod_update (tmodel, seuil, vdd, vt, vdd);
        stm_mod_update_transition (tmodel, STM_LH);
        delay0->TMLH->UTYPE = STM_MOD_MODSCM;
        csttab[0] = delay / TTV_UNIT;
        csttab[1] = 0.0;
        delay0->TMLH->UMODEL.SCM = stm_modscm_cst_create(csttab);
        name = stm_storemodel(CELL, NULL, delay0->TMLH, 0);
        if (name == delay0->TMLH->NAME)
            DETAILED_MODELS = addchain (DETAILED_MODELS, name);
        else {
            stm_mod_destroy(delay0->TMLH);
            delay0->TMLH = stm_getmodel(CELL, name);
        }
        delay0->FMLH = delay0->TMLH;
      }
    }

    delay0->TPLH = delay;
    delay0->FLH = 0L;
#ifdef USEOLDTEMP
    delay0->SLH = TAS_NOS;
#endif
#ifdef USEOLDTEMP
    delay0->RLH = TAS_NORES;
#endif
  }
  if ((delay0->TPLL == TAS_NOTIME) && (delay0->TPHL != TAS_NOTIME)) {
    delay = 0L;
    if (conein != NULL) {
      if (existe_tpd(cone, conein, CNS_VSS, CNS_TN, &link)) {
        vt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VT);
        vdd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
        seuil = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_SEUIL);
        delay =
            (long) ((stm_get_t(vt, vt, vdd, 0.0, fdown) -
                     stm_get_t(seuil, vt, vdd, 0.0, fdown)) * TTV_UNIT);
        if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        {
         tas_newname(buf, 0);
        }
        else
         strcpy(buf, stm_mod_name (conein->NAME, 'D', cone->NAME, 'D', STM_DELAY|STM_SLEW, TAS_PATH_TYPE, 0));
       
        modname=stm_createname(CELL, buf);
        delay0->TMLL = stm_mod_create (modname);
        tmodel = delay0->TMLL;
        stm_mod_update (tmodel, seuil, vdd, vt, 0.0);
        stm_mod_update_transition (tmodel, STM_LL);
        delay0->TMLL->UTYPE = STM_MOD_MODSCM;
        csttab[0] = delay / TTV_UNIT;
        csttab[1] = 0.0;
        delay0->TMLL->UMODEL.SCM = stm_modscm_cst_create(csttab);
        name = stm_storemodel(CELL, NULL, delay0->TMLL, 0);
        if (name == delay0->TMLL->NAME)
            DETAILED_MODELS = addchain (DETAILED_MODELS, name);
        else {
            stm_mod_destroy(delay0->TMLL);
            delay0->TMLL = stm_getmodel(CELL, name);
        }
        delay0->FMLL = delay0->TMLL;
      }
    }

    delay0->TPLL = delay;
    delay0->FLL = 0L;
#ifdef USEOLDTEMP
    delay0->SLL = TAS_NOS;
#endif
#ifdef USEOLDTEMP
    delay0->RLL = TAS_NORES;
#endif
  }
  if ((delay0->TPHL == TAS_NOTIME) && (delay0->TPLL != TAS_NOTIME)) {
    delay = 0L;
    if (conein != NULL) {
      if (existe_tpd(cone, conein, CNS_VSS, CNS_TP, &link)) {
        vt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VT);
        vdd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
        seuil = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_SEUIL);
        delay =
            (long) ((stm_get_t(vt, vt, vdd, 0.0, fup) -
                     stm_get_t(seuil, vt, vdd, 0.0, fup)) * TTV_UNIT);
        if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        {
         tas_newname(buf, 0);
        }
        else
         strcpy(buf, stm_mod_name (conein->NAME, 'U', cone->NAME, 'D', STM_DELAY|STM_SLEW, TAS_PATH_TYPE, 0));
       
        modname=stm_createname(CELL, buf);
        delay0->TMHL = stm_mod_create (modname);
        tmodel = delay0->TMHL;
        stm_mod_update (tmodel, seuil, vdd, vt, 0.0);
        stm_mod_update_transition (tmodel, STM_HL);
        delay0->TMHL->UTYPE = STM_MOD_MODSCM;
        csttab[0] = delay / TTV_UNIT;
        csttab[1] = 0.0;
        delay0->TMHL->UMODEL.SCM = stm_modscm_cst_create(csttab);
        name = stm_storemodel(CELL, NULL, delay0->TMHL, 0);
        if (name == delay0->TMHL->NAME)
            DETAILED_MODELS = addchain (DETAILED_MODELS, name);
        else {
            stm_mod_destroy(delay0->TMHL);
            delay0->TMHL = stm_getmodel(CELL, name);
        }
        delay0->FMHL = delay0->TMHL;
      }
    }

    delay0->TPHL = delay;
    delay0->FHL = 0L;
#ifdef USEOLDTEMP
    delay0->SHL = TAS_NOS;
#endif
#ifdef USEOLDTEMP
    delay0->RHL = TAS_NORES;
#endif
  }
}

void tas_ReplaceModelDelayMax(timing_model *modelmin, timing_model *modelmax)
{
    if(modelmin && modelmax){
        if((modelmin->UTYPE == STM_MOD_MODSCM) && (modelmax->UTYPE == STM_MOD_MODSCM)){
            if((modelmin->UMODEL.SCM->TYPE == STM_MODSCM_CST) && (modelmax->UMODEL.SCM->TYPE == STM_MODSCM_CST)){
                modelmax->UMODEL.SCM->PARAMS.CST->DS[STM_CST_DELAY] = modelmin->UMODEL.SCM->PARAMS.CST->DS[STM_CST_DELAY];
            }
        }
    }
}

/*----------------------------------------------------------------------------
*                    UpdateDelayMax()                                        *
*                                                                            *
* met a jour delaymax si delaymin est superieur                              *
----------------------------------------------------------------------------*/
void UpdateDelayMax(delay_list * delay0, delay_list * delay1)
{
  if ((delay0->TPLH != TAS_NOTIME) && (delay1->TPLH != TAS_NOTIME))
    if (delay0->TPLH > delay1->TPLH){
      delay1->TPLH = delay0->TPLH;
      tas_ReplaceModelDelayMax(delay0->TMLH, delay1->TMLH);
    }
  if ((delay0->TPHH != TAS_NOTIME) && (delay1->TPHH != TAS_NOTIME))
    if (delay0->TPHH > delay1->TPHH){
      delay1->TPHH = delay0->TPHH;
      tas_ReplaceModelDelayMax(delay0->TMHH, delay1->TMHH);
    }
  if ((delay0->TPHL != TAS_NOTIME) && (delay1->TPHL != TAS_NOTIME))
    if (delay0->TPHL > delay1->TPHL){
      delay1->TPHL = delay0->TPHL;
      tas_ReplaceModelDelayMax(delay0->TMHL, delay1->TMHL);
    }
  if ((delay0->TPLL != TAS_NOTIME) && (delay1->TPLL != TAS_NOTIME))
    if (delay0->TPLL > delay1->TPLL){
      delay1->TPLL = delay0->TPLL;
      tas_ReplaceModelDelayMax(delay0->TMLL, delay1->TMLL);
    }
}

/*----------------------------------------------------------------------------*/
/*                    tas_extract()                                     */
/*----------------------------------------------------------------------------*/
delay_list *tas_extract(cone_list * cone0, edge_list * input0,
                        double *capa)
{

  delay_list *delay = NULL;

  if (((input0->TYPE & (CNS_FEEDBACK | TAS_IN_MEMSYM)) == 0) ||
      ((input0->TYPE & TAS_IN_NOTMEMSYM) == TAS_IN_NOTMEMSYM)) {
    if ((input0->TYPE & CNS_CONE) == CNS_CONE) {
      if (TAS_PATH_TYPE == 'm')
        delay = (delay_list *) getptype(input0->USER, TAS_DELAY_MIN)->DATA;
      else
        delay = (delay_list *) getptype(input0->USER, TAS_DELAY_MAX)->DATA;

      delay->TMLH=delay->TMHH=delay->TMHL=delay->TMLL=NULL;
      delay->TPLH=delay->TPHH=delay->TPHL=delay->TPLL=TAS_NOTIME;
      delay->FLH=delay->FHH=delay->FHL=delay->FLL=TAS_NOFRONT;

      if ((cone0->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS)
        delay =
            val_dual(cone0, (cone_list *) input0->UEDGE.CONE,
                     delay, 0, 0.0);
      else
        delay =
            val_non_dual(cone0, (cone_list *) input0->UEDGE.CONE,
                         delay, 0, 0.0);
    } else {
      if ((input0->TYPE & CNS_EXT) == CNS_EXT) {
        if (TAS_PATH_TYPE == 'm')
          delay =
              (delay_list *) getptype(input0->USER, TAS_DELAY_MIN)->DATA;
        else
          delay =
              (delay_list *) getptype(input0->USER, TAS_DELAY_MAX)->DATA;
        delay =
            val_con(cone0, (locon_list *) input0->UEDGE.LOCON,
                    delay, 0, 0.0);
      }
    }
  }
  if (cone0->BRVDD)
    *capa = tas_getcapalink(NULL, cone0->BRVDD->LINK, TAS_UNKNOWN_EVENT);
  else if (cone0->BRVSS)
    *capa = tas_getcapalink(NULL, cone0->BRVSS->LINK, TAS_UNKNOWN_EVENT);
  else if (cone0->BREXT)
    *capa = tas_getcapalink(NULL, cone0->BREXT->LINK, TAS_UNKNOWN_EVENT);

  return delay;
}

/*----------------------------------------------------------------------------
*                    tas_thr2scm()                                           *
----------------------------------------------------------------------------*/
double tas_thr2scm(double fthr, link_list * activelink, char ttype)
{
  double vt, vdd, vf, thmin, thmax;
  char stype;

  vt = tas_getparam(activelink->ULINK.LOTRS, TAS_CASE, TP_VT);
  vf = tas_get_vf_input(activelink);
  vdd = vf;
  if (ttype == elpRISE) {
    stype = STM_UP;
    thmin = STM_DEFAULT_SMINR;
    thmax = STM_DEFAULT_SMAXR;
  } else if (ttype == elpFALL) {
    stype = STM_DN;
    thmin = STM_DEFAULT_SMINF;
    thmax = STM_DEFAULT_SMAXF;
    vf = 0.0;
  }
  return stm_thr2scm(fthr, thmin, thmax, vt, vf, vdd, stype);
}

/*----------------------------------------------------------------------------*/
/*                    tas_DupTabFloat()                                            */
/*----------------------------------------------------------------------------*/
void tas_DupTabFloat(float *duptab, double *tab, int n)
{
  int i;

  for (i = 0; i < n; i++)
    duptab[i] = (float) tab[i];

}

/*----------------------------------------------------------------------------
*                    tas_evalmodel1Dloadfix()                                *
----------------------------------------------------------------------------*/
void
tas_evalmodel1Dloadfix(timing_model * tmodeld, timing_model * tmodels,
                       double *fcarac, double *f_scaled, int nfcarac,
                       char *bufd, char *bufs, double ci0)
{
  timing_scm *scm;
  timing_table *tbld;
  timing_table *tbls;
  char *name;

  scm = tmodeld->UMODEL.SCM;
  if (stm_if_thresholds())
    stm_mod_shrinkslewaxis_thr2scm(tmodeld, f_scaled, fcarac, nfcarac);
  tbld =
      stm_modtbl_create_delay1Dloadfix_fscm(scm, f_scaled, nfcarac, ci0);
  tbls = stm_modtbl_create_slew1Dloadfix_fscm(scm, f_scaled, nfcarac, ci0);
  stm_modscm_destroy(scm);
  tmodeld->UTYPE = STM_MOD_MODTBL;
  tmodeld->UMODEL.TABLE = tbld;
  if (stm_if_thresholds()) {
    stm_mod_shrinkslewaxis_scm2thr(tmodeld, fcarac);
  }
  tmodeld->NAME = stm_createname( CELL, bufd );
  name = stm_storemodel(CELL, NULL, tmodeld, 0);
  if (name == tmodeld->NAME)
    DETAILED_MODELS = addchain(DETAILED_MODELS, name);
  else {
    stm_mod_destroy(tmodeld);
    tmodeld = stm_getmodel(CELL, name);
  }
  tmodels->UTYPE = STM_MOD_MODTBL;
  tmodels->UMODEL.TABLE = tbls;
  if (stm_if_thresholds()) {
    stm_mod_shrinkslewaxis_scm2thr(tmodels, fcarac);
    stm_mod_shrinkslewdata_scm2thr(tmodels);
  }
  tmodels->NAME = stm_createname( CELL, bufs );
  name = stm_storemodel(CELL, NULL, tmodels, 0);
  if (name == tmodels->NAME)
    DETAILED_MODELS = addchain(DETAILED_MODELS, name);
  else {
    stm_mod_destroy(tmodels);
    tmodels = stm_getmodel(CELL, name);
  }
}

/*----------------------------------------------------------------------------
*                    tas_evaldelay1Dloadfix()                                *
----------------------------------------------------------------------------*/
void
tas_evaldelay1Dloadfix(cone_list * cone0, edge_list * input0,
                       delay_list * delay, double *fcarac,
                       double *f_scaled, int nfcarac, char type,
                       double ci0)
{
  timing_model *tmodeld;
  timing_model *tmodels;
  char bufd[1024];
  char bufs[1024];
  char *input_name;

  if ((input0->TYPE & CNS_CONE) == CNS_CONE) {
    if (input0->UEDGE.CONE->NAME)
      input_name = input0->UEDGE.CONE->NAME;
  } else if (input0->UEDGE.LOCON->NAME) {
    input_name = input0->UEDGE.LOCON->NAME;
  }

  if (delay) {
    if (delay->TMHL) {
      tmodeld = delay->TMHL;
      delay->FMHL = stm_mod_create(NULL);
      stm_mod_update(delay->FMHL, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMHL, STM_HL);
      tmodels = delay->FMHL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dloadfix(tmodeld, tmodels, fcarac, f_scaled,
                             nfcarac, bufd, bufs, ci0);
    }
    if (delay->TMLH) {
      tmodeld = delay->TMLH;
      delay->FMLH = stm_mod_create(NULL);
      stm_mod_update(delay->FMLH, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMLH, STM_LH);
      tmodels = delay->FMLH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dloadfix(tmodeld, tmodels, fcarac, f_scaled,
                             nfcarac, bufd, bufs, ci0);
    }
    if (delay->TMLL) {
      tmodeld = delay->TMLL;
      delay->FMLL = stm_mod_create(NULL);
      stm_mod_update(delay->FMLL, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMLL, STM_LL);
      tmodels = delay->FMLL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dloadfix(tmodeld, tmodels, fcarac, f_scaled,
                             nfcarac, bufd, bufs, ci0);
    }
    if (delay->TMHH) {
      tmodeld = delay->TMHH;
      delay->FMHH = stm_mod_create(NULL);
      stm_mod_update(delay->FMHH, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMHH, STM_HH);
      tmodels = delay->FMHH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dloadfix(tmodeld, tmodels, fcarac, f_scaled,
                             nfcarac, bufd, bufs, ci0);
    }
  }
}

/*----------------------------------------------------------------------------
*                    tas_eval1Dloadfix()                                     *
----------------------------------------------------------------------------*/
void
tas_eval1Dloadfix(cone_list * cone0, edge_list * input0,
                  delay_list * delaymin, delay_list * delaymax,
                  double *fcarac, double *f_scaled, int nfcarac,
                  double ci0)
{

  if (delaymin)
    tas_evaldelay1Dloadfix(cone0, input0, delaymin, fcarac, f_scaled,
                           nfcarac, 'm', ci0);
  if (delaymax)
    tas_evaldelay1Dloadfix(cone0, input0, delaymax, fcarac, f_scaled,
                           nfcarac, 'M', ci0);
}

/*----------------------------------------------------------------------------
*                    tas_evalmodel1Dslewfix()                                *
----------------------------------------------------------------------------*/
void
tas_evalmodel1Dslewfix(timing_model * tmodeld, timing_model * tmodels,
                       double *ccarac, int nccarac, char *bufd, char *bufs,
                       double fin, double ci0)
{
  timing_scm *scm;
  timing_table *tbld;
  timing_table *tbls;
  char *name;

  scm = tmodeld->UMODEL.SCM;
  fin = stm_mod_shrinkslew_thr2scm(tmodeld, fin / TTV_UNIT);
  tbld =
      stm_modtbl_create_delay1Dslewfix_fscm(scm, ccarac, nccarac, fin,
                                            ci0);
  tbls =
      stm_modtbl_create_slew1Dslewfix_fscm(scm, ccarac, nccarac, fin, ci0);
  stm_modscm_destroy(scm);
  tmodeld->UTYPE = STM_MOD_MODTBL;
  tmodeld->UMODEL.TABLE = tbld;
  tmodeld->NAME = stm_createname( CELL, bufd );
  name = stm_storemodel(CELL, NULL, tmodeld, 0);
  if (name == tmodeld->NAME)
    DETAILED_MODELS = addchain(DETAILED_MODELS, name);
  else {
    stm_mod_destroy(tmodeld);
    tmodeld = stm_getmodel(CELL, name);
  }
  tmodels->UTYPE = STM_MOD_MODTBL;
  tmodels->UMODEL.TABLE = tbls;
  if (stm_if_thresholds()) {
    stm_mod_shrinkslewdata_scm2thr(tmodels);
  }
  tmodels->NAME = stm_createname( CELL, bufs );
  name = stm_storemodel(CELL, NULL, tmodels, 0);
  if (name == tmodels->NAME)
    DETAILED_MODELS = addchain(DETAILED_MODELS, name);
  else {
    stm_mod_destroy(tmodels);
    tmodels = stm_getmodel(CELL, name);
  }
}

/*----------------------------------------------------------------------------
*                    tas_evaldelay1Dslewfix()                                *
----------------------------------------------------------------------------*/
void
tas_evaldelay1Dslewfix(cone_list * cone0, edge_list * input0,
                       delay_list * delay, double *ccarac, int nccarac,
                       char type, double ci0)
{
  timing_model *tmodeld;
  timing_model *tmodels;
  char bufd[1024];
  char bufs[1024];
  front_list *front;
  long finup;
  long findown;
  char *input_name;

  if ((input0->TYPE & CNS_CONE) == CNS_CONE) {
    if (input0->UEDGE.CONE->NAME)
      input_name = input0->UEDGE.CONE->NAME;
  } else if (input0->UEDGE.LOCON->NAME) {
    input_name = input0->UEDGE.LOCON->NAME;
  }

  if (type == 'M') {
    front = tas_getslope(cone0, input0->UEDGE.CONE);
  } else {
    TAS_PATH_TYPE = 'm';
    front = tas_getslope(cone0, input0->UEDGE.CONE);
    TAS_PATH_TYPE = 'M';
  }

  finup = front->FUP;
  findown = front->FDOWN;

  if (delay) {
    if (delay->TMHL) {
      tmodeld = delay->TMHL;
      delay->FMHL = stm_mod_create(NULL);
      stm_mod_update(delay->FMHL, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMHL, STM_HL);
      tmodels = delay->FMHL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dslewfix(tmodeld, tmodels, ccarac, nccarac, bufd,
                             bufs, finup, ci0);
    }
    if (delay->TMLH) {
      tmodeld = delay->TMLH;
      delay->FMLH = stm_mod_create(NULL);
      stm_mod_update(delay->FMLH, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMLH, STM_LH);
      tmodels = delay->FMLH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dslewfix(tmodeld, tmodels, ccarac, nccarac, bufd,
                             bufs, findown, ci0);
    }
    if (delay->TMLL) {
      tmodeld = delay->TMLL;
      delay->FMLL = stm_mod_create(NULL);
      stm_mod_update(delay->FMLL, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMLL, STM_LL);
      tmodels = delay->FMLL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dslewfix(tmodeld, tmodels, ccarac, nccarac, bufd,
                             bufs, findown, ci0);
    }
    if (delay->TMHH) {
      tmodeld = delay->TMHH;
      delay->FMHH = stm_mod_create(NULL);
      stm_mod_update(delay->FMHH, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMHH, STM_HH);
      tmodels = delay->FMHH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel1Dslewfix(tmodeld, tmodels, ccarac, nccarac, bufd,
                             bufs, finup, ci0);
    }
  }
}

/*----------------------------------------------------------------------------
*                    tas_eval1Dslewfix()                                     *
----------------------------------------------------------------------------*/
void
tas_eval1Dslewfix(cone_list * cone0, edge_list * input0,
                  delay_list * delaymin, delay_list * delaymax,
                  double *ccarac, int nccarac, double ci0)
{

  if (delaymin)
    tas_evaldelay1Dslewfix(cone0, input0, delaymin, ccarac, nccarac,
                           'm', ci0);
  if (delaymax)
    tas_evaldelay1Dslewfix(cone0, input0, delaymax, ccarac, nccarac,
                           'M', ci0);
}

/*----------------------------------------------------------------------------
*                    tas_evalmodel2D()                                       *
----------------------------------------------------------------------------*/
void
tas_evalmodel2D(timing_model * tmodeld, timing_model * tmodels,
                double *fcarac, double *f_scaled, int nfcarac,
                double *ccarac, int nccarac, char *bufd, char *bufs,
                double ci0)
{
  timing_scm *scm;
  timing_table *tbld;
  timing_table *tbls;
  char *name;

  scm = tmodeld->UMODEL.SCM;
  if (stm_if_thresholds())
    stm_mod_shrinkslewaxis_thr2scm(tmodeld, f_scaled, fcarac, nfcarac);
  tbld =
      stm_modtbl_create_delay2D_fscm(scm, f_scaled, nfcarac, ccarac,
                                     nccarac, ci0);
  tbls =
      stm_modtbl_create_slew2D_fscm(scm, f_scaled, nfcarac, ccarac,
                                    nccarac, ci0);
  stm_modscm_destroy(scm);
  tmodeld->UTYPE = STM_MOD_MODTBL;
  tmodeld->UMODEL.TABLE = tbld;
  if (stm_if_thresholds()) {
    stm_mod_shrinkslewaxis_scm2thr(tmodeld, fcarac);
  }
  tmodeld->NAME = stm_createname( CELL, bufd );
  name = stm_storemodel(CELL, NULL, tmodeld, 0);
  if (name == tmodeld->NAME)
    DETAILED_MODELS = addchain(DETAILED_MODELS, name);
  else {
    stm_mod_destroy(tmodeld);
    tmodeld = stm_getmodel(CELL, name);
  }
  tmodels->UTYPE = STM_MOD_MODTBL;
  tmodels->UMODEL.TABLE = tbls;
  if (stm_if_thresholds()) {
    stm_mod_shrinkslewaxis_scm2thr(tmodels, fcarac);
    stm_mod_shrinkslewdata_scm2thr(tmodels);
  }
  tmodels->NAME = stm_createname( CELL, bufs );
  name = stm_storemodel(CELL, NULL, tmodels, 0);
  if (name == tmodels->NAME)
    DETAILED_MODELS = addchain(DETAILED_MODELS, name);
  else {
    stm_mod_destroy(tmodels);
    tmodels = stm_getmodel(CELL, name);
  }
}

/*----------------------------------------------------------------------------
*                    tas_evaldelay2D()                                       *
----------------------------------------------------------------------------*/
void
tas_evaldelay2D(cone_list * cone0, edge_list * input0, delay_list * delay,
                double *fcarac, double *f_scaled, int nfcarac,
                double *ccarac, int nccarac, char type, double ci0)
{
  timing_model *tmodeld;
  timing_model *tmodels;
  char bufd[1024];
  char bufs[1024];
  char *input_name;

  if ((input0->TYPE & CNS_CONE) == CNS_CONE) {
    if (input0->UEDGE.CONE->NAME)
      input_name = input0->UEDGE.CONE->NAME;
  } else if (input0->UEDGE.LOCON->NAME) {
    input_name = input0->UEDGE.LOCON->NAME;
  }

  if (delay) {
    if (delay->TMHL) {
      tmodeld = delay->TMHL;
      delay->FMHL = stm_mod_create(NULL);
      stm_mod_update(delay->FMHL, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMHL, STM_HL);
      tmodels = delay->FMHL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel2D(tmodeld, tmodels, fcarac, f_scaled, nfcarac,
                      ccarac, nccarac, bufd, bufs, ci0);
    }
    if (delay->TMLH) {
      tmodeld = delay->TMLH;
      delay->FMLH = stm_mod_create(NULL);
      stm_mod_update(delay->FMLH, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMLH, STM_LH);
      tmodels = delay->FMLH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel2D(tmodeld, tmodels, fcarac, f_scaled, nfcarac,
                      ccarac, nccarac, bufd, bufs, ci0);
    }
    if (delay->TMLL) {
      tmodeld = delay->TMLL;
      delay->FMLL = stm_mod_create(NULL);
      stm_mod_update(delay->FMLL, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMLL, STM_LL);
      tmodels = delay->FMLL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel2D(tmodeld, tmodels, fcarac, f_scaled, nfcarac,
                      ccarac, nccarac, bufd, bufs, ci0);
    }
    if (delay->TMHH) {
      tmodeld = delay->TMHH;
      delay->FMHH = stm_mod_create(NULL);
      stm_mod_update(delay->FMHH, stm_mod_vth(tmodeld),
                     stm_mod_vdd(tmodeld), stm_mod_vt(tmodeld),
                     stm_mod_vf(tmodeld));
      stm_mod_update_transition(delay->FMHH, STM_HH);
      tmodels = delay->FMHH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y') {
        tas_newname(bufd, 0);
        tas_newname(bufs, 0);
      } else {
        sprintf(bufd,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_DELAY, type, 0));
        sprintf(bufs,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_SLEW, type, 0));
      }
      tas_evalmodel2D(tmodeld, tmodels, fcarac, f_scaled, nfcarac,
                      ccarac, nccarac, bufd, bufs, ci0);
    }
  }
}

/*----------------------------------------------------------------------------
*                    tas_eval2D()                                            *
----------------------------------------------------------------------------*/
void
tas_eval2D(cone_list * cone0, edge_list * input0, delay_list * delaymin,
           delay_list * delaymax, double *fcarac, double *f_scaled,
           int nfcarac, double *ccarac, int nccarac, double ci0)
{

  if (delaymin)
    tas_evaldelay2D(cone0, input0, delaymin, fcarac, f_scaled, nfcarac,
                    ccarac, nccarac, 'm', ci0);
  if (delaymax)
    tas_evaldelay2D(cone0, input0, delaymax, fcarac, f_scaled, nfcarac,
                    ccarac, nccarac, 'M', ci0);
}

/*----------------------------------------------------------------------------
*                    tas_newname()                                           *
----------------------------------------------------------------------------*/
void tas_newname(char *buf, int reset)
{
  static long i = 0;

  if (reset) {
    i = 0;
    return;
  }

  sprintf(buf, "m%ld", i);
  i++;
}

/*----------------------------------------------------------------------------
*                    tas_getslew_caracmax()                                  *
----------------------------------------------------------------------------*/
float
tas_getslew_caracmax(cone_list * cone, edge_list * input, char type,
                     char minormax)
{
  edge_list *edge;
  float smax = FLT_MIN;
  ptype_list *ptype;
  delay_list *delay;
  double *fcarac;
  double *ccarac;
  int nfcarac;
  int nccarac;
  int mode;
  front_list *front;

  mode = caracmode(cone, input, &ccarac, &nccarac, &fcarac, &nfcarac);

  if ((mode != CARAFRONT) && (mode != CARALES2)
      && ((input->TYPE & CNS_CONE) == CNS_CONE)) {
    for (edge = input->UEDGE.CONE->INCONE; edge; edge = edge->NEXT) {
      if (minormax == 'M')
        ptype = getptype(edge->USER, TAS_DELAY_MAX);
      else
        ptype = getptype(edge->USER, TAS_DELAY_MIN);
      if (ptype) {
        delay = (delay_list *) ptype->DATA;
        if (delay) {
          if (type == 'H') {
            if (delay->TMLH)
              if (smax < stm_mod_getcaracslewoutmax(delay->TMLH))
                smax = stm_mod_getcaracslewoutmax(delay->TMLH);
            if (delay->TMHH)
              if (smax < stm_mod_getcaracslewoutmax(delay->TMHH))
                smax = stm_mod_getcaracslewoutmax(delay->TMHH);
          } else if (type == 'L') {
            if (delay->TMHL)
              if (smax < stm_mod_getcaracslewoutmax(delay->TMHL))
                smax = stm_mod_getcaracslewoutmax(delay->TMHL);
            if (delay->TMLL)
              if (smax < stm_mod_getcaracslewoutmax(delay->TMLL))
                smax = stm_mod_getcaracslewoutmax(delay->TMLL);
          }
        }
      }
    }
    if (((input->TYPE & CNS_CONE) == CNS_CONE) && smax == FLT_MIN) {
      if (minormax == 'M')
        ptype = getptype(input->UEDGE.CONE->USER, TAS_SLOPE_MAX);
      else
        ptype = getptype(input->UEDGE.CONE->USER, TAS_SLOPE_MIN);
      if (ptype) {
        front = (front_list *) ptype->DATA;
        if (type == 'H')
          smax = front->FUP;
        else if (type == 'L')
          smax = front->FDOWN;
      }
    }
  } else if ((mode == CARAFRONT) || (mode == CARALES2)) {
    smax = (float) fcarac[nfcarac - 1];
  }

  return smax;
}

/*----------------------------------------------------------------------------
*                    tas_getslew_caracmin()                                  *
----------------------------------------------------------------------------*/
float
tas_getslew_caracmin(cone_list * cone, edge_list * input, char type,
                     char minormax)
{
  edge_list *edge;
  float smin = FLT_MAX;
  ptype_list *ptype;
  delay_list *delay;
  double *fcarac;
  double *ccarac;
  int nfcarac;
  int nccarac;
  int mode;
  front_list *front;

  mode = caracmode(cone, input, &ccarac, &nccarac, &fcarac, &nfcarac);

  if ((mode != CARAFRONT) && (mode != CARALES2)
      && ((input->TYPE & CNS_CONE) == CNS_CONE)) {
    for (edge = input->UEDGE.CONE->INCONE; edge; edge = edge->NEXT) {
      if (minormax == 'M')
        ptype = getptype(edge->USER, TAS_DELAY_MAX);
      else
        ptype = getptype(edge->USER, TAS_DELAY_MIN);
      if (ptype) {
        delay = (delay_list *) ptype->DATA;
        if (delay) {
          if (type == 'H') {
            if (delay->TMLH)
              if (smin > stm_mod_getcaracslewoutmin(delay->TMLH))
                smin = stm_mod_getcaracslewmin(delay->TMLH);
            if (delay->TMHH)
              if (smin > stm_mod_getcaracslewoutmin(delay->TMHH))
                smin = stm_mod_getcaracslewoutmin(delay->TMHH);
          } else if (type == 'L') {
            if (delay->TMHL)
              if (smin > stm_mod_getcaracslewoutmin(delay->TMHL))
                smin = stm_mod_getcaracslewoutmin(delay->TMHL);
            if (delay->TMLL)
              if (smin > stm_mod_getcaracslewoutmin(delay->TMLL))
                smin = stm_mod_getcaracslewoutmin(delay->TMLL);
          }
        }
      }
    }
    if (((input->TYPE & CNS_CONE) == CNS_CONE) && smin == FLT_MAX) {
      if (minormax == 'M')
        ptype = getptype(input->UEDGE.CONE->USER, TAS_SLOPE_MAX);
      else
        ptype = getptype(input->UEDGE.CONE->USER, TAS_SLOPE_MIN);
      if (ptype) {
        front = (front_list *) ptype->DATA;
        if (type == 'H')
          smin = front->FUP;
        else if (type == 'L')
          smin = front->FDOWN;
      }
    }
  } else if ((mode == CARAFRONT) || (mode == CARALES2)) {
    smin = (float) fcarac[0];
  }

  return smin;
}

/*----------------------------------------------------------------------------
*                    tas_setcarac_scm()                                      *
----------------------------------------------------------------------------*/
void
tas_setcarac_scm(cone_list * cone0, edge_list * input0, delay_list * delay,
                 char minormax)
{
  float smax, smin;
  float scalecapa_tbl = 0.2;

  if (delay) {
    if (delay->TMHL) {
      smax = tas_getslew_caracmax(cone0, input0, 'H', minormax);
      smin = tas_getslew_caracmin(cone0, input0, 'H', minormax);
      stm_mod_setcaracslew(delay->TMHL, smax);
      stm_mod_setcaracslew(delay->TMHL, smin);
      smax =
          stm_mod_slew(delay->TMHL,
                       (2 - scalecapa_tbl) * tas_getcapai(cone0),
                       smax, NULL, NULL, cone0->NAME);
      smin =
          stm_mod_slew(delay->TMHL,
                       scalecapa_tbl * tas_getcapai(cone0), smin,
                       NULL, NULL, cone0->NAME);
      stm_mod_setcaracslewout(delay->TMHL, smax);
      stm_mod_setcaracslewout(delay->TMHL, smin);
      stm_mod_setcaracload(delay->TMHL,
                           scalecapa_tbl * tas_getcapai(cone0));
      stm_mod_setcaracload(delay->TMHL,
                           (2 - scalecapa_tbl) * tas_getcapai(cone0));
    }
    if (delay->TMLH) {
      smax = tas_getslew_caracmax(cone0, input0, 'L', minormax);
      smin = tas_getslew_caracmin(cone0, input0, 'L', minormax);
      stm_mod_setcaracslew(delay->TMLH, smax);
      stm_mod_setcaracslew(delay->TMLH, smin);
      smax =
          stm_mod_slew(delay->TMLH,
                       (2 - scalecapa_tbl) * tas_getcapai(cone0),
                       smax, NULL, NULL, cone0->NAME);
      smin =
          stm_mod_slew(delay->TMLH,
                       scalecapa_tbl * tas_getcapai(cone0), smin,
                       NULL, NULL, cone0->NAME);
      stm_mod_setcaracslewout(delay->TMLH, smax);
      stm_mod_setcaracslewout(delay->TMLH, smin);
      stm_mod_setcaracload(delay->TMLH,
                           scalecapa_tbl * tas_getcapai(cone0));
      stm_mod_setcaracload(delay->TMLH,
                           (2 - scalecapa_tbl) * tas_getcapai(cone0));
    }
    if (delay->TMLL) {
      smax = tas_getslew_caracmax(cone0, input0, 'L', minormax);
      smin = tas_getslew_caracmin(cone0, input0, 'L', minormax);
      stm_mod_setcaracslew(delay->TMLL, smax);
      stm_mod_setcaracslew(delay->TMLL, smin);
      smax =
          stm_mod_slew(delay->TMLL,
                       (2 - scalecapa_tbl) * tas_getcapai(cone0),
                       smax, NULL, NULL, cone0->NAME);
      smin =
          stm_mod_slew(delay->TMLL,
                       scalecapa_tbl * tas_getcapai(cone0), smin,
                       NULL, NULL, cone0->NAME);
      stm_mod_setcaracslewout(delay->TMLL, smax);
      stm_mod_setcaracslewout(delay->TMLL, smin);
      stm_mod_setcaracload(delay->TMLL,
                           scalecapa_tbl * tas_getcapai(cone0));
      stm_mod_setcaracload(delay->TMLL,
                           (2 - scalecapa_tbl) * tas_getcapai(cone0));
    }
    if (delay->TMHH) {
      smax = tas_getslew_caracmax(cone0, input0, 'H', minormax);
      smin = tas_getslew_caracmin(cone0, input0, 'H', minormax);
      stm_mod_setcaracslew(delay->TMHH, smax);
      stm_mod_setcaracslew(delay->TMHH, smin);
      smax =
          stm_mod_slew(delay->TMHH,
                       (2 - scalecapa_tbl) * tas_getcapai(cone0),
                       smax, NULL, NULL, cone0->NAME);
      smin =
          stm_mod_slew(delay->TMHH,
                       scalecapa_tbl * tas_getcapai(cone0), smin,
                       NULL, NULL, cone0->NAME);
      stm_mod_setcaracslewout(delay->TMHH, smax);
      stm_mod_setcaracslewout(delay->TMHH, smin);
      stm_mod_setcaracload(delay->TMHH,
                           scalecapa_tbl * tas_getcapai(cone0));
      stm_mod_setcaracload(delay->TMHH,
                           (2 - scalecapa_tbl) * tas_getcapai(cone0));
    }
  }
}

/*----------------------------------------------------------------------------
*                    tas_setcarac_scm_minandmax()                            *
----------------------------------------------------------------------------*/
void
tas_setcarac_scm_minandmax(cone_list * cone0, edge_list * input0,
                           delay_list * delaymin, delay_list * delaymax)
{

  if (delaymin)
    tas_setcarac_scm(cone0, input0, delaymin, 'm');
  if (delaymax)
    tas_setcarac_scm(cone0, input0, delaymax, 'M');
}

/*----------------------------------------------------------------------------
*                    tas_store_scm()                                         *
----------------------------------------------------------------------------*/
void
tas_store_scm(cone_list * cone0, edge_list * input0, delay_list * delay,
              char type)
{
  timing_model *tmodel;
  char buf[1024];
  char *name;
  char *input_name;

  if ((input0->TYPE & CNS_CONE) == CNS_CONE) {
    if (input0->UEDGE.CONE->NAME)
      input_name = input0->UEDGE.CONE->NAME;
  } else if (input0->UEDGE.LOCON->NAME) {
    input_name = input0->UEDGE.LOCON->NAME;
  }

  if (delay) {
    if (delay->TMHL) {
      tmodel = delay->TMHL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        tas_newname(buf, 0);
      else
        sprintf(buf,
                stm_mod_name(input_name, 'U', cone0->NAME, 'D',
                             STM_DELAY | STM_SLEW, type, 0));
      delay->TMHL->NAME = stm_createname( CELL, buf );
      name = stm_storemodel(CELL, NULL, delay->TMHL, 0);
      if (name == delay->TMHL->NAME)
        DETAILED_MODELS = addchain(DETAILED_MODELS, name);
      else {
        stm_mod_destroy(delay->TMHL);
        delay->TMHL = stm_getmodel(CELL, name);
      }
      if ((delay->TMHL->UTYPE == STM_MOD_MODSCM || delay->TMHL->UTYPE == STM_MOD_MODIV ) && (!delay->FMHL))
        delay->FMHL = delay->TMHL;
    }
    if (delay->TMLH) {
      tmodel = delay->TMLH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        tas_newname(buf, 0);
      else
        sprintf(buf,
                stm_mod_name(input_name, 'D', cone0->NAME, 'U',
                             STM_DELAY | STM_SLEW, type, 0));
      delay->TMLH->NAME = stm_createname( CELL, buf );
      name = stm_storemodel(CELL, NULL, delay->TMLH, 0);
      if (name == delay->TMLH->NAME)
        DETAILED_MODELS = addchain(DETAILED_MODELS, name);
      else {
        stm_mod_destroy(delay->TMLH);
        delay->TMLH = stm_getmodel(CELL, name);
      }
      if ((delay->TMLH->UTYPE == STM_MOD_MODSCM || delay->TMLH->UTYPE == STM_MOD_MODIV ) && (!delay->FMLH))
        delay->FMLH = delay->TMLH;
    }
    if (delay->TMLL) {
      tmodel = delay->TMLL;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        tas_newname(buf, 0);
      else
        sprintf(buf,
                stm_mod_name(input_name, 'D', cone0->NAME, 'D',
                             STM_DELAY | STM_SLEW, type, 0));
      delay->TMLL->NAME = stm_createname( CELL, buf);
      name = stm_storemodel(CELL, NULL, delay->TMLL, 0);
      if (name == delay->TMLL->NAME)
        DETAILED_MODELS = addchain(DETAILED_MODELS, name);
      else {
        stm_mod_destroy(delay->TMLL);
        delay->TMLL = stm_getmodel(CELL, name);
      }
      if ((delay->TMLL->UTYPE == STM_MOD_MODSCM) && (!delay->FMLL))
        delay->FMLL = delay->TMLL;
    }
    if (delay->TMHH) {
      tmodel = delay->TMHH;
      if (TAS_CONTEXT->TAS_SHORT_MODELNAME == 'Y')
        tas_newname(buf, 0);
      else
        sprintf(buf,
                stm_mod_name(input_name, 'U', cone0->NAME, 'U',
                             STM_DELAY | STM_SLEW, type, 0));
      delay->TMHH->NAME = stm_createname( CELL, buf );
      name = stm_storemodel(CELL, NULL, delay->TMHH, 0);
      if (name == delay->TMHH->NAME)
        DETAILED_MODELS = addchain(DETAILED_MODELS, name);
      else {
        stm_mod_destroy(delay->TMHH);
        delay->TMHH = stm_getmodel(CELL, name);
      }
      if ((delay->TMHH->UTYPE == STM_MOD_MODSCM) && (!delay->FMHH))
        delay->FMHH = delay->TMHH;
    }
  }
}


/*----------------------------------------------------------------------------
*                    tas_store_scm_minandmax()                               *
----------------------------------------------------------------------------*/
void
tas_store_scm_minandmax(cone_list * cone0, edge_list * input0,
                        delay_list * delaymin, delay_list * delaymax)
{

  if (delaymin)
    tas_store_scm(cone0, input0, delaymin, 'm');
  if (delaymax)
    tas_store_scm(cone0, input0, delaymax, 'M');
}


/*----------------------------------------------------------------------------
*                    tas_destroymodscm()                                     *
----------------------------------------------------------------------------*/
void tas_destroymodscm(delaymin, delaymax)
delay_list *delaymin;
delay_list *delaymax;
{
  if (delaymin) {
    if (delaymin->TMLH) {
      stm_mod_destroy(delaymin->TMLH);
      delaymin->TMLH = NULL;
    }
    if (delaymin->TMHL) {
      stm_mod_destroy(delaymin->TMHL);
      delaymin->TMHL = NULL;
    }
    if (delaymin->TMLL) {
      stm_mod_destroy(delaymin->TMLL);
      delaymin->TMLL = NULL;
    }
    if (delaymin->TMHH) {
      stm_mod_destroy(delaymin->TMHH);
      delaymin->TMHH = NULL;
    }
    if (delaymin->FMLH) {
      stm_mod_destroy(delaymin->FMLH);
      delaymin->FMLH = NULL;
    }
    if (delaymin->FMHL) {
      stm_mod_destroy(delaymin->FMHL);
      delaymin->FMHL = NULL;
    }
    if (delaymin->FMLL) {
      stm_mod_destroy(delaymin->FMLL);
      delaymin->FMLL = NULL;
    }
    if (delaymin->FMHH) {
      stm_mod_destroy(delaymin->FMHH);
      delaymin->FMHH = NULL;
    }
  }
  if (delaymax) {
    if (delaymax->TMLH) {
      stm_mod_destroy(delaymax->TMLH);
      delaymax->TMLH = NULL;
    }
    if (delaymax->TMHL) {
      stm_mod_destroy(delaymax->TMHL);
      delaymax->TMHL = NULL;
    }
    if (delaymax->TMLL) {
      stm_mod_destroy(delaymax->TMLL);
      delaymax->TMLL = NULL;
    }
    if (delaymax->TMHH) {
      stm_mod_destroy(delaymax->TMHH);
      delaymax->TMHH = NULL;
    }
    if (delaymax->FMLH) {
      stm_mod_destroy(delaymax->FMLH);
      delaymax->FMLH = NULL;
    }
    if (delaymax->FMHL) {
      stm_mod_destroy(delaymax->FMHL);
      delaymax->FMHL = NULL;
    }
    if (delaymax->FMLL) {
      stm_mod_destroy(delaymax->FMLL);
      delaymax->FMLL = NULL;
    }
    if (delaymax->FMHH) {
      stm_mod_destroy(delaymax->FMHH);
      delaymax->FMHH = NULL;
    }
  }
}

/*----------------------------------------------------------------------------
*                    tas_getcapai()                                          *
----------------------------------------------------------------------------*/
float tas_getcapai(cone_list * cone)
{
  if (cone->BRVDD) {
    if (cone->BRVDD->LINK)
      return tas_getcapalink(NULL, cone->BRVDD->LINK, TAS_UNKNOWN_EVENT);
  } else if (cone->BRVSS) {
    if (cone->BRVSS->LINK)
      return tas_getcapalink(NULL, cone->BRVSS->LINK, TAS_UNKNOWN_EVENT);
  } else if (cone->BREXT) {
    if (cone->BREXT->LINK)
      return tas_getcapalink(NULL, cone->BREXT->LINK, TAS_UNKNOWN_EVENT);
  } else if (cone->BRGND) {
    if (cone->BRGND->LINK)
      return tas_getcapalink(NULL, cone->BRGND->LINK, TAS_UNKNOWN_EVENT);
  } else
    return 0.0;
  return 0.0;
}

static locon_list *tas_getextconeconnector(cone_list *cn)
{
  branch_list *br;
  edge_list *ed;
  for (br=cn->BREXT; br!=NULL; br=br->NEXT)
    if (br->LINK!=NULL && br->LINK->NEXT==NULL && (br->LINK->TYPE & (CNS_IN|CNS_INOUT))!=0)
       return br->LINK->ULINK.LOCON;

  for (ed=cn->OUTCONE; ed!=NULL; ed=ed->NEXT)
    if ((ed->TYPE & CNS_EXT)!=0)
       return ed->UEDGE.LOCON;

  return NULL;
}


char
tas_FP_UDM(delay_list * delaymin, delay_list * delaymax, cone_list * cone0,
           edge_list * input0, double capamin, double capamax)
{
  char warres = 'N';
  char save = TAS_PATH_TYPE;
  locon_list *extcon;
  int ignore=0;

  capamin = 0.0 ;
  capamax = 0.0 ;

  if ((cone0->TYPE & CNS_EXT)!=0 && (extcon=tas_getextconeconnector(cone0))!=NULL && extcon->SIG->TYPE=='I')
    ignore=1;
    
  if (delaymin) {
    TAS_PATH_TYPE = 'm';
#ifdef USEOLDTEMP
    warres = ComputeSlopes(delaymin, cone0, input0, capamin);
#endif
    if (((cone0->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE) ||
        ((input0->TYPE & CNS_HZCOM) == CNS_HZCOM && !ignore))
      FitPrecharge(delaymin, cone0, input0);
  }
  if (delaymax) {
    TAS_PATH_TYPE = 'M';
#ifdef USEOLDTEMP
    warres = ComputeSlopes(delaymax, cone0, input0, capamax);
#endif
    if (((cone0->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE) ||
        ((input0->TYPE & CNS_HZCOM) == CNS_HZCOM && !ignore))
      FitPrecharge(delaymax, cone0, input0);
  }
  if (delaymin && delaymax)
    UpdateDelayMax(delaymin, delaymax);

  TAS_PATH_TYPE = save;

  return warres;
}



/*----------------------------------------------------------------------------
*                    tas_tpd()                                               *
----------------------------------------------------------------------------*/
// si un break point n'a pas de délai calculé, on force le front par delaut pour
// que des lines partent du breakpoint
void tas_check_break_point_with_no_delay(cone_list *cone, char minmax)
{
  ptype_list *ptype;
  front_list *fr;

  if ((cone->TYPE & TAS_BREAK)!=0 || (cone->TYPE & (CNS_LATCH|CNS_PRECHARGE))!=0)
  {
    if (minmax == 'm')
      ptype = getptype(cone->USER, TAS_SLOPE_MIN);
    else
      ptype = getptype(cone->USER, TAS_SLOPE_MAX);

    if (ptype!=NULL)
    {
      fr = (front_list *) ptype->DATA;

      if (fr->FUP==TAS_NOFRONT && fr->FDOWN==TAS_NOFRONT)
      {
        fr->FUP=TAS_CONTEXT->FRONT_CON;
        fr->FDOWN=TAS_CONTEXT->FRONT_CON;
      }
    }
  }
}

void tas_Leakage_Calculation(cone_list *cone)
{
  double ilupmax = 0.0;
  double ilupmin = 0.0;
  double ildnmax = 0.0;
  double ildnmin = 0.0;
  double ilext   = 0.0;
  branch_list *branch;
  leakage_list *leakage;
  char   save=TAS_PATH_TYPE;
  
//  if (cone->BREXT && (cone->BREXT->LINK->TYPE & (CNS_IN | CNS_INOUT))) return;
  
  TAS_PATH_TYPE = 'M';
  for(branch = cone->BRVSS; branch; branch = branch->NEXT){
      ilupmax += tas_get_current_leakage( branch, branch->LINK, branch->LINK);
  }
  for(branch = cone->BRVDD; branch; branch = branch->NEXT){
      ildnmax += tas_get_current_leakage( branch, branch->LINK, branch->LINK);
  }
  for(branch = cone->BREXT; branch; branch = branch->NEXT){
      if(branch->LINK->TYPE & (CNS_IN | CNS_INOUT)) continue;
      ilext = tas_get_current_leakage( branch, branch->LINK, branch->LINK);
      ilupmax += ilext;
      ildnmax += ilext;
  }
  TAS_PATH_TYPE = 'm';
  for(branch = cone->BRVSS; branch; branch = branch->NEXT){
      ilupmin += tas_get_current_leakage( branch, branch->LINK, branch->LINK);
  }
  for(branch = cone->BRVDD; branch; branch = branch->NEXT){
      ildnmin += tas_get_current_leakage( branch, branch->LINK, branch->LINK);
  }
  TAS_PATH_TYPE = save;
  
  leakage = (leakage_list*)mbkalloc(sizeof(leakage_list)) ;
  leakage->L_UP_MAX = ilupmax;
  leakage->L_UP_MIN = ilupmin;
  leakage->L_DN_MAX = ildnmax;
  leakage->L_DN_MIN = ildnmin;
  cone->USER = addptype(cone->USER, TAS_LEAKAGE, leakage);
  
/*  fprintf(stdout, "Ileakage cone %s:\n", cone->NAME);
  fprintf(stdout, "Ileakage UP Max %g:\n", ilupmax);
  fprintf(stdout, "Ileakage UP Min %g:\n", ilupmin);
  fprintf(stdout, "Ileakage DN Max %g:\n", ildnmax);
  fprintf(stdout, "Ileakage DN Min %g:\n", ildnmin);
  fprintf(stdout, "\n");*/
}

int tas_tpd(cone0)
cone_list *cone0;
{
  branch_list *chemin0[3];
  link_list *maillon0;
  edge_list *input0;
  delay_list *delaymin;
  delay_list *delaymax;
  double capamin = 0.0;
  double capamax = 0.0;
  int i, j;
  char warres = 'N';
  int mode;
  double *fcarac;
  double *ccarac;
  double f_scaled[1024];
  int nfcarac;
  int nccarac;

  if ((cone0->TYPE & (CNS_VDD | CNS_VSS)))
    return (0);

  if((( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 ) 
     || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 2)
     || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 4)) 
     && !getptype(cone0->USER, TAS_LEAKAGE))
    tas_Leakage_Calculation(cone0);

  avt_log(LOGTAS, 2, "Computing delay for cone %ld '%s'\n", cone0->INDEX,
          cone0->NAME);

  input0 = cone0->INCONE;

  while (input0) {
    delaymin = NULL;
    delaymax = NULL;
    tas_counter_model++;

    TAS_PATH_TYPE = 'M';

    if ((input0->TYPE & CNS_EXT) == CNS_EXT)
      avt_log(LOGTAS, 3, "        Delay MAX from connector '%s'\n",
              ((locon_list *) input0->UEDGE.LOCON)->NAME);
    else
      avt_log(LOGTAS, 3, "        Delay MAX from cone %ld '%s'\n",
              ((cone_list *) input0->UEDGE.CONE)->INDEX,
              ((cone_list *) input0->UEDGE.CONE)->NAME);

    delaymax = tas_extract(cone0, input0, &capamax);


    if (TAS_CONTEXT->TAS_FIND_MIN == 'Y') {
      TAS_PATH_TYPE = 'm';

      if ((input0->TYPE & CNS_EXT) == CNS_EXT)
        avt_log(LOGTAS, 3,
                "        Delay MIN from connector '%s'\n",
                ((locon_list *) input0->UEDGE.LOCON)->NAME);
      else
        avt_log(LOGTAS, 3,
                "        Delay MIN from cone %ld '%s'\n",
                ((cone_list *) input0->UEDGE.CONE)->INDEX,
                ((cone_list *) input0->UEDGE.CONE)->NAME);

      delaymin = tas_extract(cone0, input0, &capamin);
    }


    switch (TAS_CONTEXT->TAS_CARACMODE) {
    case TAS_LUT_INT:          /* LUT interface */
      mode =
          caracmode(cone0, input0, &ccarac, &nccarac, &fcarac, &nfcarac);

      switch (mode) {
      case CARAFRONT:          /* carac 1D  pour les fronts */
        for (j = 0; j < nfcarac; j++)
          f_scaled[j] = fcarac[j];
        tas_eval1Dloadfix(cone0, input0, delaymin, delaymax,
                          fcarac, f_scaled, nfcarac, tas_getcapai(cone0));
        warres =
            tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin,
                       capamax);
        break;

      case CARACAPA:           /* carac 1D pour les capas */
        tas_eval1Dslewfix(cone0, input0, delaymin, delaymax,
                          ccarac, nccarac, 0.0);
        warres =
            tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin,
                       capamax);
        break;

      case CARALES2:           /* carac 2D  fronts et capas */
        for (j = 0; j < nfcarac; j++)
          f_scaled[j] = fcarac[j];
        tas_eval2D(cone0, input0, delaymin, delaymax, fcarac,
                   f_scaled, nfcarac, ccarac, nccarac, 0.0);
        warres =
            tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin,
                       capamax);
        break;
      default:
        warres =
            tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin,
                       capamax);
        if (TAS_CONTEXT->TAS_VALID_SCM != 'Y')
          tas_destroymodscm(delaymin, delaymax);
      }
      break;

    case TAS_LUT_CPL:          /* LUT complet */
      tas_store_scm_minandmax(cone0, input0, delaymin, delaymax);
/*      if (TTV_HTCAPA_AXIS) {
        if (((long) gethtitem(TTV_HTCAPA_AXIS, cone0->NAME) == EMPTYHT)
            && ((long)
                gethtitem(TTV_HTCAPA_AXIS,
                          namealloc("default")) == EMPTYHT))
          tas_setcarac_scm_minandmax(cone0, input0, delaymin, delaymax);
      } else {*/
        tas_setcarac_scm_minandmax(cone0, input0, delaymin, delaymax);
/*      }*/
      warres =
          tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin, capamax);
      break;

    case TAS_SCM_INT:          /* SCM interface */
      mode =
          caracmode(cone0, input0, &ccarac, &nccarac, &fcarac, &nfcarac);

      if ((mode == CARAFRONT) || (mode == CARACAPA)
          || (mode == CARALES2)) {
        tas_store_scm_minandmax(cone0, input0, delaymin, delaymax);
//                if(TAS_CONTEXT->TAS_CAPAOUT > 0.0001){
        if (tas_get_cone_output_capacitance(cone0) > 0.0001) {
/*          if (TTV_HTCAPA_AXIS) {
            if (((long) gethtitem(TTV_HTCAPA_AXIS, cone0->NAME)
                 == EMPTYHT)
                && ((long)
                    gethtitem(TTV_HTCAPA_AXIS,
                              namealloc("default")) == EMPTYHT))
              tas_setcarac_scm_minandmax(cone0, input0,
                                         delaymin, delaymax);
          } else {*/
            tas_setcarac_scm_minandmax(cone0, input0, delaymin, delaymax);
/*          }*/
        }
      }
      warres =
          tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin, capamax);
      if ((mode != CARAFRONT) && (mode != CARACAPA)
          && (mode != CARALES2))
        if (TAS_CONTEXT->TAS_VALID_SCM != 'Y')
          tas_destroymodscm(delaymin, delaymax);
      break;

    case TAS_SCM_CPL:          /* SCM complet */
      tas_store_scm_minandmax(cone0, input0, delaymin, delaymax);
//            if(TAS_CONTEXT->TAS_CAPAOUT > 0.0001){
      if (tas_get_cone_output_capacitance(cone0) > 0.0001) {
/*        if (TTV_HTCAPA_AXIS) {
          if (((long) gethtitem(TTV_HTCAPA_AXIS, cone0->NAME) == EMPTYHT)
              && ((long)
                  gethtitem(TTV_HTCAPA_AXIS,
                            namealloc("default")) == EMPTYHT))
            tas_setcarac_scm_minandmax(cone0, input0, delaymin, delaymax);
        } else {*/
          tas_setcarac_scm_minandmax(cone0, input0, delaymin, delaymax);
/*        }*/
      }
      warres =
          tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin, capamax);
      break;

    case TAS_PLY_INT:          /* POLY interface */
      break;

    case TAS_PLY_CPL:          /* POLY complet */
      break;

    default:                   /* pas de carac pour cone interne */
      warres =
          tas_FP_UDM(delaymin, delaymax, cone0, input0, capamin, capamax);
      if (TAS_CONTEXT->TAS_VALID_SCM != 'Y')
        tas_destroymodscm(delaymin, delaymax);
    }
    input0 = input0->NEXT;
  }
  chemin0[0] = cone0->BREXT;
  chemin0[1] = cone0->BRVDD;
  chemin0[2] = cone0->BRVSS;
  for (i = 0; i < 3; i++)
    for (; chemin0[i]; chemin0[i] = chemin0[i]->NEXT)
      for (maillon0 = (link_list *) chemin0[i]->LINK; maillon0;
           maillon0 = maillon0->NEXT)
        if ((maillon0->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
          if (getptype(maillon0->ULINK.LOTRS->USER, TAS_BTCONFL))
            maillon0->ULINK.LOTRS->USER =
                delptype(maillon0->ULINK.LOTRS->USER, TAS_BTCONFL);
          if (getptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL))
            maillon0->ULINK.LOTRS->USER =
                delptype(maillon0->ULINK.LOTRS->USER, TAS_UCONFL);
        }

  if (warres == 'Y')
    tas_error(39, cone0->NAME, TAS_WARNING);
  
  if (TAS_CONTEXT->TAS_FIND_MIN == 'Y')
    tas_check_break_point_with_no_delay(cone0, 'm');
    
  tas_check_break_point_with_no_delay(cone0, 'M');

  return 0;
}

/*-----------------------------------------------------------------------------
*                    tas_getfin()                                             *
*                                                                             *
* renvoie le front d'entree du cone dans le cas de dual, good et false        *
-----------------------------------------------------------------------------*/
double
tas_getfin(link_list * maillon1, cone_list * cone, float fcarac, stm_pwl ** pwl, float *rin, float *cin, float *vsat)
{
  long fin0 = 0L;
  cone_list *cone_avant;
  front_list *front0;
  ptype_list *ptl ;
  losig_list *losig ;
  rcx_slope slope ;
  RCXFLOAT c1, c2, r ;
  char type ;
  locon_list *locon ;
  
  cone_avant =
      (cone_list *) getptype(maillon1->ULINK.LOTRS->USER,
                             CNS_DRIVINGCONE)->DATA;
  front0 = tas_getslope(cone, cone_avant);
  if (pwl)
    *pwl = NULL;

  if ((maillon1->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
    fin0 = front0->FDOWN;
    if (pwl)
      *pwl = front0->PWLDN;
    if (rin) {
      *rin = front0->DRIVERDN.R ;
      if( V_BOOL_TAB[ __AVT_CTK_ON_INPUT ].VALUE && V_BOOL_TAB[ __AVT_CTK_ON_INPUT_NET ].VALUE ) {
        r = tas_getrintercone( cone, cone_avant );
        if( *rin > 0.0 )
          *rin = *rin+r ;
        else
          *rin = r ;
      }
    }
    if (vsat)
      *vsat = front0->DRIVERDN.V ;
  } else {
    fin0 = front0->FUP;
    if (pwl)
      *pwl = front0->PWLUP;
    if (rin) {
      *rin = front0->DRIVERUP.R ;
      if( V_BOOL_TAB[ __AVT_CTK_ON_INPUT ].VALUE && V_BOOL_TAB[ __AVT_CTK_ON_INPUT_NET ].VALUE ) {
        r = tas_getrintercone( cone, cone_avant );
        if( *rin > 0.0 )
          *rin = *rin+r ;
        else
          *rin = r ;
      }
    }
    if (vsat)
      *vsat = front0->DRIVERUP.V ;
  }

  if( cin ) {
  
    *cin = -1.0 ;

    if( ( cone_avant->TYPE & CNS_EXT ) != CNS_EXT ) {
      ptl = getptype( cone_avant->USER, CNS_SIGNAL );
      
      if( ptl ) {
      
        losig = ptl->DATA ;
        locon = NULL ;
        ptl = getptype( losig->USER, LOFIGCHAIN );
        if( ptl ) {
          if( ptl->DATA ) {
            locon= (locon_list*)( ( (chain_list*)(ptl->DATA) )->DATA )  ;
          }
        }
          
        if( locon && locon->PNODE ) {
        
          slope.F0MAX = 0.0;
          slope.FCCMAX = 0.0;
          slope.CCA = -1.0;
          if(( maillon1->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
            slope.SENS = TRC_SLOPE_UP;
          else
            slope.SENS = TRC_SLOPE_DOWN;
          type = rcx_rcnload(TAS_CONTEXT->TAS_LOFIG, losig,
                             locon->PNODE, &r, &c1, &c2,
                             RCX_BESTLOAD,
                             tas_get_cone_output_capacitance(cone_avant)
                             , &slope, 'M');
          switch( type ) {
          case RCX_CAPALOAD :
            *cin = c1 ;
            break ;
          case RCX_PILOAD :
            *cin = c1+c2 ;
            break ;
          }
        }
      }
    }
  }
    
  if (fin0 == TAS_NOFRONT)
    return TAS_NOFRONT_DBL;
  else if (fcarac)
    return fcarac;
  else
    return (double) fin0;
}

/*-----------------------------------------------------------------------------
*                    tas_getwlr2effgood()                                     *
*                                                                             *
* renvoie les parametres wl(taille activelink) et r2(resistance des links     *
* entre activelink et l'alimentation) pour tpd_transgood                      *
-----------------------------------------------------------------------------*/
void
tas_getwlr2effgood(link_list * maillon1, double rx, double *wl, double *r2)
{
  link_list *maillonc = maillon1->NEXT;
  double lw;

  while (maillonc && !(maillonc->TYPE & (CNS_IN | CNS_INOUT))) {
    lw = (double) TAS_GETLENGTH(maillonc) /
        (double) TAS_GETWIDTH(maillonc);
    if ((maillonc->TYPE & CNS_SWITCH) == CNS_SWITCH)
      lw /= tas_get_current_rate(maillonc);
    //lw /= 1.7;
    *r2 = lw * tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_R);
    maillonc = maillonc->NEXT;
  }

  *wl = (double) TAS_GETWIDTH(maillon1) / (double) TAS_GETLENGTH(maillon1);
  if ((maillon1->TYPE & CNS_SWITCH) == CNS_SWITCH)
    *wl *= tas_get_current_rate(maillon1);
  rx = 0.0;
}

void tas_getraccess(link_list * maillon, double *rs, double *rd)
{
  lotrs_list *lotrs;

  lotrs = maillon->ULINK.LOTRS;

  if (maillon->SIG == lotrs->DRAIN->SIG) {
    if (rs)
      *rs = tas_getparam(lotrs, TAS_CASE, TP_RACCS);
    if (rd)
      *rd = tas_getparam(lotrs, TAS_CASE, TP_RACCD);
  } else {
    if (rs)
      *rs = tas_getparam(lotrs, TAS_CASE, TP_RACCD);
    if (rd)
      *rd = tas_getparam(lotrs, TAS_CASE, TP_RACCS);
  }
}

/*-----------------------------------------------------------------------------
*                    tas_getwlr2effdual()                                     *
*                                                                             *
* renvoie les parametres wl(taille activelink) et r2(resistance des links     *
* entre activelink et l'alimentation) pour tpd_dual                           *
-----------------------------------------------------------------------------*/
void
tas_getwlr2effdual(link_list * maillon1, double rx, double *wl, double *r2)
{
  link_list *maillonc;
  double lw;
  double rd, rs, rt;

  *r2 = 0.0;

  tas_getraccess(maillon1, &rs, NULL);

  *r2 = *r2 + rs;

  maillonc = maillon1->NEXT;

  while (maillonc && !(maillonc->TYPE & (CNS_IN | CNS_INOUT))) {

    tas_getraccess(maillonc, &rs, &rd);

    lw = (double) TAS_GETLENGTH(maillonc) /
        (double) TAS_GETWIDTH(maillonc);

    if ((maillonc->TYPE & CNS_SWITCH) == CNS_SWITCH)
      lw /= tas_get_current_rate(maillonc);

    rt = lw * tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_RT);

    *r2 = *r2 + rt + rs + rd;
    maillonc = maillonc->NEXT;
  }

  *wl = (double) TAS_GETWIDTH(maillon1) / (double) TAS_GETLENGTH(maillon1);
  if ((maillon1->TYPE & CNS_SWITCH) == CNS_SWITCH)
    *wl *= tas_get_current_rate(maillon1);
  rx = 0.0;
}

/*-----------------------------------------------------------------------------
*                    tas_getresforconflit()                                   *
* fonction inutilisee qui sert pour pconf1 renvoie r totale - r maillonc      *
-----------------------------------------------------------------------------*/
void
tas_getresforconflit(link_list * maillon0, link_list * maillon1,
                     double *wl, double *r)
{
  link_list *maillonc = maillon0;
  double lw;
  *r = 0.0;

  while (maillonc && !(maillonc->TYPE & (CNS_IN | CNS_INOUT))) {
    if (maillonc != maillon1) {
      lw = (double) TAS_GETLENGTH(maillonc) /
          (double) TAS_GETWIDTH(maillonc);
      if ((maillonc->TYPE & CNS_SWITCH) == CNS_SWITCH)
        lw /= tas_get_current_rate(maillonc);
      *r += lw * tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE, TP_RT);
    }
    maillonc = maillonc->NEXT;
  }
  *wl = (double) TAS_GETWIDTH(maillon1) / (double) TAS_GETLENGTH(maillon1);
}

/*-----------------------------------------------------------------------------
*                    tas_getRCbranch()                                        *
*                                                                             *
-----------------------------------------------------------------------------*/
void tas_getRCbranch(link_list * maillon1, double *Rbr, double *Cbr)
{

  link_list *maillonc;
  chain_list *chainlink = NULL, *chain;
  double capa, lw, y0, y1, yd0, yd1;
  double rs, rd, rt, cs, cd;
  lotrs_list *lotrs;
  link_list *nextmaillon;
  lotrs_list *nextlotrs;
  double nextcs;
  double nextrs;
  int flagfirstresi;


  maillonc = maillon1;

  while (maillonc && !(maillonc->TYPE & (CNS_IN | CNS_INOUT))) {
    chainlink = addchain(chainlink, maillonc);
    maillonc = maillonc->NEXT;
  }

  nextmaillon = NULL;
  flagfirstresi = 1;

  for (chain = chainlink; chain; chain = chain->NEXT) {

    if (nextmaillon) {
      maillonc = nextmaillon;
      lotrs = nextlotrs;
    } else {
      maillonc = (link_list *) chain->DATA;
      lotrs = maillonc->ULINK.LOTRS;
    }

    if (chain->NEXT) {
      nextmaillon = (link_list *) chain->NEXT->DATA;
      nextlotrs = nextmaillon->ULINK.LOTRS;
    } else {
      nextmaillon = NULL;
    }

    if (lotrs->DRAIN->SIG == maillonc->SIG) {
      rs = tas_getparam(lotrs, TAS_CASE, TP_RACCS);
      rd = tas_getparam(lotrs, TAS_CASE, TP_RACCD);
      cs = 1000.0 * elpGetCapaFromLocon(lotrs->SOURCE,
                                        ELP_CAPA_TYPICAL, TAS_CASE);
      cd = 1000.0 * elpGetCapaFromLocon(lotrs->DRAIN,
                                        ELP_CAPA_TYPICAL, TAS_CASE);
    } else {
      rd = tas_getparam(lotrs, TAS_CASE, TP_RACCS);
      rs = tas_getparam(lotrs, TAS_CASE, TP_RACCD);
      cd = 1000.0 * elpGetCapaFromLocon(lotrs->SOURCE,
                                        ELP_CAPA_TYPICAL, TAS_CASE);
      cs = 1000.0 * elpGetCapaFromLocon(lotrs->DRAIN,
                                        ELP_CAPA_TYPICAL, TAS_CASE);
    }

    lw = (double) TAS_GETLENGTH(maillonc) /
        (double) TAS_GETWIDTH(maillonc);

    if ((maillonc->TYPE & CNS_SWITCH) == CNS_SWITCH)
      lw /= tas_get_current_rate(maillonc);

    rt = lw * (double) tas_getparam(maillonc->ULINK.LOTRS, TAS_CASE,
                                    TP_RT);
    capa = (double) tas_getcapalink(NULL, maillonc, TAS_UNKNOWN_EVENT);

    if (nextmaillon) {
      if (nextlotrs->DRAIN->SIG == nextmaillon->SIG) {
        nextcs =
            1000.0 * elpGetCapaFromLocon(nextlotrs->SOURCE,
                                         ELP_CAPA_TYPICAL, TAS_CASE);
        nextrs = tas_getparam(nextlotrs, TAS_CASE, TP_RACCS);
      } else {
        nextcs =
            1000.0 * elpGetCapaFromLocon(nextlotrs->DRAIN,
                                         ELP_CAPA_TYPICAL, TAS_CASE);
        nextrs = tas_getparam(nextlotrs, TAS_CASE, TP_RACCD);
      }
    } else {
      nextcs = 0.0;
      nextrs = 0.0;
    }

    if (rd > 0.0) {
      capa = capa - cd;
      if (nextrs > 0.0)
        capa = capa - nextcs;
    } else {
      if (nextrs > 0.0)
        capa = capa - nextcs;
      else
        cd = capa;
    }

    /* source */

    if (rs > 0.0) {
      if (flagfirstresi) {
        y0 = 1.0 / rs;
        y1 = cs;
        flagfirstresi = 0;
      } else {
        y0 = (1.0 / rs * yd0) / (1.0 / rs + yd0);
        y1 = ((1.0 / rs - y0) * yd1) / (1.0 / rs + yd0) + cs;
      }

      yd0 = y0;
      yd1 = y1;
    }

    if (maillonc != maillon1) {

      /* drain */

      if (flagfirstresi) {
        y0 = 1.0 / rt;
        y1 = cd;
        flagfirstresi = 0;
      } else {
        y0 = (1.0 / rt * yd0) / (1.0 / rt + yd0);
        y1 = ((1.0 / rt - y0) * yd1) / (1.0 / rt + yd0) + cd;
      }

      yd0 = y0;
      yd1 = y1;

      /* external drain signal */

      if (rd > 0.0) {

        if (flagfirstresi) {
          y0 = 1.0 / rd;
          y1 = capa;
          flagfirstresi = 0;
        } else {
          y0 = (1.0 / rd * yd0) / (1.0 / rd + yd0);
          y1 = ((1.0 / rd - y0) * yd1) / (1.0 / rd + yd0) + capa;
        }

        yd0 = y0;
        yd1 = y1;
      }
    }
  }

  if (!flagfirstresi) {
    *Rbr = 1.0 / y0;
    *Cbr = y1;
  } else {
    *Rbr = -1.0;
    *Cbr = -1.0;
  }

  freechain(chainlink);
}

double tas_get_input_deg(link_list * maillon1)
{
  cone_list *cone_avant;
  ptype_list *ptype;
  alim_list *power;
  float alim;
  double deg;
  link_list *link;
  branch_list *branch;

  cone_avant =
      (cone_list *) getptype(maillon1->ULINK.LOTRS->USER,
                             CNS_DRIVINGCONE)->DATA;

  power = cns_get_multivoltage(cone_avant);
  if (power) {
    if (TAS_PATH_TYPE == 'M')
      alim = power->VDDMIN - power->VSSMAX;
    else
      alim = power->VDDMAX - power->VSSMIN;
  } else {
    alim = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  }
  if ((maillon1->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
    deg = 0.0;
    if ((cone_avant->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED) {
      for (branch = cone_avant->BRVSS; branch; branch = branch->NEXT) {
        if ((branch->TYPE & CNS_DEGRADED) == CNS_DEGRADED) {
          for (link = branch->LINK; link; link = link->NEXT) {
            if ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
              if ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
                if (deg <=
                    tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg))
                  deg = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg);
                break;
              }
          }
        }
      }
      for (branch = cone_avant->BREXT; branch; branch = branch->NEXT) {
        if ((branch->TYPE & CNS_DEGRADED) == CNS_DEGRADED) {
          for (link = branch->LINK; link; link = link->NEXT) {
            if ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
              if ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
                if (deg <=
                    tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg))
                  deg = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg);
                break;
              }
          }
        }
      }
    }
  } else {
    deg = alim;
    if ((cone_avant->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED) {
      for (branch = cone_avant->BRVDD; branch; branch = branch->NEXT) {
        if ((branch->TYPE & CNS_DEGRADED) == CNS_DEGRADED) {
          for (link = branch->LINK; link; link = link->NEXT) {
            if ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
              if ((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
                if (deg >=
                    tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg))
                  deg = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg);
                break;
              }
          }
        }
      }
      for (branch = cone_avant->BREXT; branch; branch = branch->NEXT) {
        if ((branch->TYPE & CNS_DEGRADED) == CNS_DEGRADED) {
          for (link = branch->LINK; link; link = link->NEXT) {
            if ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
              if ((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
                if (deg >=
                    tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg))
                  deg = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg);
                break;
              }
          }
        }
      }
      if((deg < alim) && (ptype = getptype(cone_avant->USER, TAS_VDD_NOTDEG)) != NULL){
          deg += *(float*)(&ptype->DATA);
      }
    }
  }
  return deg;
}

/*-----------------------------------------------------------------------------
*                    tas_getmcc()                                             *
*                                                                             *
* renvoie les parametres necessaires au calcul des temps en dual et good      *
-----------------------------------------------------------------------------*/
void
tas_getmcc(maillon0, maillon1, vddin, AX, BX, RTX, RX, VT, VT0, QX, QY, seuil, vddmax,
           cgpa, cgda, cgdae, cgpoa)
link_list *maillon0;
link_list *maillon1;
double *vddin;
double *AX;
double *BX;
double *RTX;
double *RX;
double *VT;
double *VT0;
double *QX;
double *QY;
double *seuil;
double *vddmax;
double *cgpa;
double *cgda;
double *cgdae;
double *cgpoa;
{
  cone_list *cone_avant;
  link_list *maillon;
//    ptype_list *ptype;
  alim_list *power;
  float alim;

  cone_avant =
      (cone_list *) getptype(maillon1->ULINK.LOTRS->USER,
                             CNS_DRIVINGCONE)->DATA;

  power = cns_get_multivoltage(cone_avant);
  if (power) {
    if (TAS_PATH_TYPE == 'M')
      alim = power->VDDMIN - power->VSSMAX;
    else
      alim = power->VDDMAX - power->VSSMIN;
  } else {
    alim = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  }

  if ((maillon1->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP) {
    if ((cone_avant->TECTYPE & CNS_VSS_DEGRADED) != CNS_VSS_DEGRADED)
      *vddin = alim - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
    else
      *vddin = alim - tas_get_input_deg(maillon1)
          - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);

    *QX = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
           - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT))
        / tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  } else {
    if ((cone_avant->TECTYPE & CNS_VDD_DEGRADED) != CNS_VDD_DEGRADED)
      *vddin = alim - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
    else
      *vddin = tas_get_input_deg(maillon1)
          - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);

    *QX = (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax)
           - tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT))
        / tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  }
  *AX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_A);
  *BX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_B);
  *RTX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_RT);
  *RX = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_R);
  *VT = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT);
  if( VT0 ) *VT0 = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VT0);
  *QY = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGP) * 1000.0;
  *QY +=
      (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGD) * 1000.0 *
       (double) tas_get_length_active(maillon1->ULINK.LOTRS)
       * (double) tas_get_width_active(maillon1->ULINK.LOTRS,
                                       TAS_GETWIDTH(maillon1)) /
       (double) TAS_GETWIDTH(maillon1)) / SCALE_X;
  *QY *= 1.0;

  if( cgpa || cgda ) {
    if( cgpa ) *cgpa = 0.0 ;
    if( cgda ) *cgda = 0.0 ;

    for( maillon = maillon0 ; maillon ; maillon = maillon->NEXT ) {
      if( maillon == maillon1 || V_BOOL_TAB[ __TAS_USE_ALL_CAPAI ].VALUE ) {
        if( maillon->ULINK.LOTRS->GRID->SIG == maillon1->ULINK.LOTRS->GRID->SIG ) {
          if( cgpa )
            *cgpa += tas_getparam( maillon->ULINK.LOTRS, TAS_CASE, TP_CGP) 
                     * 1000.0 
                     * (double) TAS_GETWIDTH(maillon) 
                     / SCALE_X ;
          if( cgda )
            *cgda += tas_getparam(maillon->ULINK.LOTRS, TAS_CASE, TP_CGD)
                     * 1000.0 
                     * (double) tas_get_length_active(maillon1->ULINK.LOTRS) 
                     * (double) tas_get_width_active( maillon1->ULINK.LOTRS, 
                                                      TAS_GETWIDTH(maillon) )
                     / (SCALE_X * SCALE_X) ;
        }
      }
    }
  }

  if( cgpoa )
    *cgpoa =
        tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE,
                     TP_CGPO) * 1000.0 *
        (double) TAS_GETWIDTH(maillon1) / SCALE_X;

  if(cgdae) {
    cgdae[0] =
        (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGD0) * 1000.0 *
         (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
         (double) tas_get_width_active(maillon1->ULINK.LOTRS, TAS_GETWIDTH(maillon1))) /
        (SCALE_X * SCALE_X);
    cgdae[1] =
        (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGD1) * 1000.0 *
         (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
         (double) tas_get_width_active(maillon1->ULINK.LOTRS, TAS_GETWIDTH(maillon1))) /
        (SCALE_X * SCALE_X);
    cgdae[2] =
        (tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_CGD2) * 1000.0 *
         (double) tas_get_length_active(maillon1->ULINK.LOTRS) *
         (double) tas_get_width_active(maillon1->ULINK.LOTRS, TAS_GETWIDTH(maillon1))) /
        (SCALE_X * SCALE_X);
  }

  *seuil = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_SEUIL);
  *vddmax = tas_getparam(maillon1->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
}

/*-----------------------------------------------------------------------------
*                    tas_getcapaparams()                                      *
*                                                                             *
* renvoie les parametres de la capa intrinseque capai et irap pour tpd_dual   *
-----------------------------------------------------------------------------*/
float
tas_getcapaparams(cone, branch, activelink, imax, QX, QY, capai, irap, t_int,
                  event, usersat )
cone_list *cone;
branch_list *branch;
link_list *activelink;
double imax;
double QX;
double QY;
double *capai;
double *irap;
double *t_int;
char event;
int usersat ;
{
  link_list *link0 ;
  link_list *ptlink;
  double c;
  double cconf = 0.0, factor, i_int, v_int, v_intn, vdd, vt, v0, dv;
  double cgdr, cgsr, vsi, vsf, vds, vgsi, vgsf;
  char onswitchpath ;
  double vint_init ;

  link0 = branch->LINK ;
  *irap = 0.0;
  *capai = 0.0;
  *t_int = 0.0;

  onswitchpath = V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ;

  if( usersat )
    imax = tas_get_current( branch, link0, 0.5, NO );

  for (ptlink = link0; ptlink != activelink && ptlink;
       ptlink = ptlink->NEXT) {

    if( ( ptlink->TYPE & CNS_SWITCH ) != CNS_SWITCH ) {
      onswitchpath = 0 ;
    }
    
    c = ((((ptlink == link0) ? 0.0 :
           tas_getcapalink(NULL, ptlink, event)) +
          TAS_GETCAPARA(ptlink)) * tlc_getcapafactor() +
         tas_getcapabl(ptlink));

    vdd = tas_getparam(ptlink->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    vt = tas_getparam(ptlink->ULINK.LOTRS, TAS_CASE, TP_VT);

    if( onswitchpath == 0 )
      vint_init = vdd-vt ;
    else
      vint_init = vdd ;

    v_int = tas_get_vpol(ptlink);
    v_intn = tas_get_vpol(ptlink->NEXT);

    if (ptlink->ULINK.LOTRS->GRID->SIG ==
        activelink->ULINK.LOTRS->GRID->SIG) {
      v_int = 0.0;
      v_intn = 0.0;
      dv = 0.0;
      vgsi = 0.0;
      vgsf = vdd;
    } else {
      v_int = tas_get_vpol(ptlink) ;
      v_intn = tas_get_vpol(ptlink->NEXT) ;
      dv = vint_init - v_int ;
      vgsi = vdd - vint_init ;
      vgsf = vdd - v_intn;
    }

    /* capa drain */

    if (ptlink == link0) {
      vsi = vdd;
      vsf = vdd / 2.0;
      vds = vdd - vint_init ;
    } else {
      vsi = vint_init;
      vsf = v_int;
      vds = 0.0;
    }

    if (ptlink->ULINK.LOTRS->GRID->SIG !=
        activelink->ULINK.LOTRS->GRID->SIG)
      cgdr = tas_get_cgd_from_link( cone, ptlink, vdd, vds, vt, vgsi, vgsf, vsi, vsf, 'D', onswitchpath );
    else
      cgdr = 0.0;

    if (ptlink != link0) {

      if( usersat )
        i_int = tas_get_current( branch, ptlink, 0.5, NO );
      else
        i_int = tas_get_current( branch, ptlink, 1.0, NO );
    
      factor = imax / i_int;
      cconf += (2 * dv / vdd) * (c + cgdr) * factor;
    } else {
      cconf += cgdr;
    }

    /* capa source */

    vsi = vint_init ;
    vsf = v_intn;

    if (ptlink == link0)
      vds = vdd - vint_init;
    else
      vds = 0.0;

    if (ptlink->ULINK.LOTRS->GRID->SIG !=
        activelink->ULINK.LOTRS->GRID->SIG)
      cgsr = tas_get_cgd_from_link( cone, ptlink, vdd, vds, vt, vgsi, vgsf, vsi, vsf, 'S', onswitchpath );
    else
      cgsr = 0.0;

    if( usersat )
      i_int = tas_get_current( branch, ptlink->NEXT, 0.5, NO);
    else
      i_int = tas_get_current( branch, ptlink->NEXT, 1.0, NO);

    factor = imax / i_int;
    cconf += (2 * (vdd - vt - v_intn) / vdd) * cgsr;

    if (!getptype(ptlink->ULINK.LOTRS->USER, TAS_BTCONFL) && !CCUSED) {
      if( usersat )
        *irap = imax / tas_get_current( branch, ptlink, 0.5, NO );
      else
        *irap = imax / tas_get_current( branch, ptlink, 1.0, NO );
      CCUSED = 1L;
    }
  }

  if (!ptlink) {
    printf("Erreur 1  dans l'apport des arguments de valtpd()");
    tas_GetKill(1);
  }

  c = ((((ptlink == link0) ? 0.0 : tas_getcapalink(NULL, ptlink,
                                                   event)) +
        TAS_GETCAPARA(ptlink)) * tlc_getcapafactor() +
       tas_getcapabl(ptlink));

  if (ptlink != link0) {

    if( usersat )
      i_int = tas_get_current(branch, ptlink, 0.5, NO );
    else
      i_int = tas_get_current(branch, ptlink, 1.0, NO );

    vdd = tas_getparam(ptlink->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    vt = tas_getparam(ptlink->ULINK.LOTRS, TAS_CASE, TP_VT);
    v0 = vt / 2.0;
    v_int = tas_get_vpol(ptlink);
    factor = imax / i_int;
    
    if( onswitchpath == 0 )
      vint_init = vdd-vt ;
    else
      vint_init = vdd ;

    dv = vint_init - v_int;
    cconf += (2 * dv / vdd) * c * factor;
  }else if (V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE != 0) {
    cconf += c ;
  }

  if (!CCUSED) {
    if( usersat )
      *irap = imax / tas_get_current( branch, ptlink, 0.5, NO);
    else
      *irap = imax / tas_get_current( branch, ptlink, 1.0, NO);
    CCUSED = 1L;
  }

  QX = 0.0;
  QY = 0.0;
  return (float) cconf;
}

double tas_get_cgd_from_link( cone_list  *cone,
                              link_list  *link,
                              double      vdd,
                              double      vds,
                              double      vt,
                              double      vgsi,
                              double      vgsf,
                              double      vsi,
                              double      vsf,
                              char        side,      /* 'D' for Drain, 'S' for source */
                              char        addswitch  /* 1 or 0 */
                             )
{
  double       r ;
  double       lactive ;
  double       wactive ;
  double       cgd ;
  double       cgdc ;
  double       cgdr ;
  double       cgdrpair ;
  ptype_list  *ptype ;
  lotrs_list  *trspair ;
  branch_list *headbranch[3] ;
  branch_list *branch ;
  int          i ;
  link_list   *tlink ;

  lactive = (double)tas_get_length_active( link->ULINK.LOTRS ) ;
  wactive = (double)tas_get_width_active( link->ULINK.LOTRS, TAS_GETWIDTH( link ) ) ;
  
  r = 1000.0 * lactive * wactive / (SCALE_X * SCALE_X);

  if( side == 'D' ) {
    cgd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_CGD) * r;
    cgdc = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_CGDC) * r;
  }
  else {
    cgd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_CGSI) * r;
    cgdc = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_CGSIC) * r;
  }
  
  cgdr = tas_get_cgd(cgd, cgdc, vdd, vds, vt, vgsi, vgsf, vsi, vsf);

  cgdrpair = 0.0 ;
  if( addswitch ) {
 
    ptype = getptype( link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH );
    if( ptype ) {
    
      trspair = (lotrs_list*)ptype->DATA ;
      
      /* retrouve un link correspondant à ce transistor */
      
      headbranch[0] = cone->BRVDD ;
      headbranch[1] = cone->BRVSS ;
      headbranch[2] = cone->BREXT ;

      for( i=0 ; i<=2 ; i++ ) {
        for( branch = headbranch[i] ; branch ; branch = branch->NEXT ) {
          for( tlink = branch->LINK ; tlink ; tlink = tlink->NEXT ) {
            if( ( tlink->TYPE & CNS_EXT ) != CNS_EXT ) {
              if( tlink->ULINK.LOTRS == trspair )
                break ;
            }
          }
          if( tlink )
            break ;
        }
        if( branch )
          break ;
      }

      if( tlink ) {
        cgdrpair = tas_get_cgd_from_link( cone, tlink, vdd, vds, vt, vgsi, vgsf, vsi, vsf, side, 0 );
      }
    }
  }

  cgdr = cgdr + cgdrpair ;
  return cgdr ;
}
                               
double
tas_get_cgd(double cgd,
            double cgdc,
            double vdd,
            double vds,
            double vt, double vgsi, double vgsf, double vsi, double vsf)
{
  double cgdvds, qgdc, qi, qf, c;

  cgdvds = (cgd - cgdc) * vds / vdd + cgdc;

  qgdc = cgdvds * vdd;

  if (vgsi < vt)
    qi = 0.0;
  else
    qi = qgdc * (vgsi - vt) / (vdd - vt);

  if (vgsf < vt)
    qf = 0.0;
  else
    qf = qgdc * (vgsf - vt) / (vdd - vt);

  if(!V_BOOL_TAB[ __AVT_OLD_FEATURE_29 ].VALUE){
      c = (qf - qi) / vdd;
  }else{
      c = (qf - qi) / (vsf - vsi);
  }

  if (c < 0.0)
    c = -c;
  return c;
}

/*-----------------------------------------------------------------------------
*                    tas_get_an()                                             *
-----------------------------------------------------------------------------*/
double tas_get_an(r2, wl, AX, BX)
double r2;
double wl;
double AX;
double BX;
{
  double an1, bn1;
  double e1, e2, i1, i2;

  an1 = AX * wl;
  bn1 = BX;

  if (r2 != 0.0) {
    if( bn1 > 0.0 ) 
      return TAS_F1(an1, bn1, r2);
    else {
      e1 = 1.0 / bn1 ;
      e2 = 1.0 / (2.0*bn1) ;
      i1 = tas_get_i( an1, bn1, r2, e1 );
      i2 = tas_get_i( an1, bn1, r2, e2 );
      return tas_get_a( e1, i1, tas_get_b( e1, e2, i1, i2 ) );
    }
  }
  else
    return an1;
}

/*-----------------------------------------------------------------------------
*                    tas_get_bn()                                             *
-----------------------------------------------------------------------------*/
double tas_get_bn(r2, wl, AX, BX)
double r2;
double wl;
double AX;
double BX;
{
  double an1, bn1;
  double e1, e2, i1, i2;

  an1 = AX * wl;
  bn1 = BX;

  if (r2 != 0.0) {
    if( bn1 > 0.0 ) 
      return TAS_F2(an1, bn1, r2);
    else {
      e1 = 1.0 / bn1 ;
      e2 = 1.0 / (2.0*bn1) ;
      i1 = tas_get_i( an1, bn1, r2, e1 );
      i2 = tas_get_i( an1, bn1, r2, e2 );
      return tas_get_b( e1, e2, i1, i2 );
    }
  }
  else
    return bn1;
}

double tas_get_i(double a, double b, double r, double v)
{
  return ((2.0 * a * pow(v, 2)) /
          (sqrt(pow(1.0 + b * v, 2) + 4.0 * r * a * v) + b * v +
           2.0 * r * a * v + 1.0));
}

double tas_get_b(double e1, double e2, double imax, double i2)
{
  return ((i2 * pow(e1, 2) -
           imax * pow(e2, 2)) / (e1 * e2 * (imax * e2 - i2 * e1)));
}

double tas_get_a(double e1, double imax, double b)
{
  return (imax * (1.0 + b * e1) / pow(e1, 2));
}

char isbranchcontainswitch(link_list * link0)
{
  link_list *link;
  for (link = link0; link; link = link->NEXT) {
    if ((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
      break;
  }

  if (link)
    return 1;
  return 0;
}

double tas_getcmddelayforswitch( lotrs_list *active, lotrs_list *trspair )
{
  cone_list  *drvactive ;
  cone_list  *drvpair ;
  char        position ;
  edge_list  *edge ;
  delay_list *delay ;
  long        delta ;
  ptype_list *ptype ;
 
  ptype = getptype( active->USER,  CNS_DRIVINGCONE );
  if( !ptype )
    return 0.0 ;
  drvactive = (cone_list*)( ptype->DATA );

  ptype = getptype( trspair->USER,  CNS_DRIVINGCONE );
  if( !ptype )
    return 0.0 ;
  drvpair   = (cone_list*)( ptype->DATA );

  position = 'x' ;

  for( edge = drvactive->INCONE ; edge ; edge = edge->NEXT ) {
  
    if( ( edge->TYPE & CNS_EXT ) == CNS_EXT )
      continue ;

    if( edge->UEDGE.CONE == drvpair ) {

      position = 'a' ;
      break ;
    }
  }
  
  if( position == 'x' ) {
    for( edge = drvpair->INCONE ; edge ; edge = edge->NEXT ) {
    
      if( ( edge->TYPE & CNS_EXT ) == CNS_EXT )
        continue ;

      if( edge->UEDGE.CONE == drvactive ) {

        position = 'b' ;
        break ;
      }
    }
  }

  if( position == 'x' )
    return 0.0 ;

  ptype = getptype( edge->USER, TAS_DELAY_MAX );
  if( !ptype )
    return 0.0 ;
  delay = (delay_list*)( ptype->DATA );
  
  delta = TAS_NOTIME ;

  if( MLO_IS_TRANSN( active->TYPE  ) &&
      MLO_IS_TRANSP( trspair->TYPE )    ) {
    delta = delay->TPHL ;
  }
  
  if( MLO_IS_TRANSP( active->TYPE  ) &&
      MLO_IS_TRANSN( trspair->TYPE )    ) {
    delta = delay->TPHL ;
  }

  if( delta == TAS_NOTIME )
    return 0.0 ;

  if( position == 'a' )
    delta = -delta ;
  delta = ( delta>>1 ) / TTV_UNIT ;
  return delta ;
}

float tas_get_point_rsat_br_delta( float *tabi, float *tabv, int n, int k, int l )
{
  int   i ;
  float it ;
  float d ;
  float delta ;
  float p1 ;
  float p2 ;
  float p3 ;

  delta = 0.0 ;
 
  p1 = tabi[k]/tabv[k] ;
  p2 = ( tabi[l] - tabi[k] ) / ( tabv[l] - tabv[k] ) ;
  p3 = ( tabi[n-1] - tabi[l] ) / ( tabv[n-1] - tabv[l] ) ;
  
  for( i=0 ; i<n ; i++ ) {

    if( i <= k ) 
      it = tabv[i] * p1 ;
    else {
      if( i <= l )
        it = tabi[l] - (tabv[l]-tabv[i])*p2 ;
      else
        it = tabi[n-1] - (tabv[n-1]-tabv[i])*p3 ;
    }

    d = tabi[i]-it ;
    delta = delta + d*d ;

  }

  return delta ;
}

/* 
  calcule le point isat/vsat qui permet d'obtenir la meilleur caracteristique 
  statique rlin/rsat d'une branche.
  cette fonction est utilisée pour les switchs.
*/
void tas_get_point_rsat_br( branch_list *branch, 
                            double       vddmax, 
                            double       imax, 
                            double      *isat, 
                            double      *vsat,
                            double      *ilin, 
                            double      *vlin
                          )
{

  #define RSAT_NB_POINT 10

  float i[RSAT_NB_POINT] ;
  float v[RSAT_NB_POINT] ;
  int   k, l ;
  int   kmin ;
  int   lmin ;
  float delta ;
  float deltamin ;
  float r ;
  
  for( k=1 ; k < RSAT_NB_POINT ; k++ ) {
    r    =  ((float)k)/((float)RSAT_NB_POINT) ;
    v[k-1] = vddmax*r ;
    i[k-1] = tas_get_current( branch, branch->LINK, r, NO );
  }

  v[ RSAT_NB_POINT - 1 ] = vddmax ;
  i[ RSAT_NB_POINT - 1 ] = imax ;

  for( k=0 ; k<RSAT_NB_POINT-1 ; k++ ) {

    for( l=k+1 ; l<RSAT_NB_POINT ; l++ ) {

      delta = tas_get_point_rsat_br_delta( i, v, RSAT_NB_POINT, k, l );
      if( ( k==0 && l==1 ) || delta < deltamin ) {
        kmin = k ;
        lmin = l ;
        deltamin = delta ;
      }
    }
  }

  *isat = i[lmin] ;
  *vsat = v[lmin] ;
  *ilin = i[kmin] ;
  *vlin = v[kmin] ;
}

float tas_get_best_ab_for_switch_error( float *tabv, float *tabi, int n, float an, float bn, float vt )
{
  int   i ;
  float ecart ;
  float v ;
  float lv ;
  float imcc ;
  float imes ;
  float limcc ;
  float limes ;
  float u ;
  float s ;
  float dv ;
  float p ;
  float lp ;
  
  ecart = 0.0 ;
 
  
  for( i=1 ; i<n ; i++ ) {
  
    v    = tabv[i-1] ;
    imes = tabi[i-1] ;
    u    = v - vt ;

    imcc = an*u*u/(1.0+bn*u) ;
    
    if( i>=2 ) {
   
      dv    = v - lv ;
      p     = (imes-limes)/dv ;
      
      if( i >=3 && p < lp )
        break ;

      s     = 0.5 * dv * ( imes + limes - imcc - limcc );
      ecart = ecart + s ;
    }
    else {
    }
    
    limes = imes ;
    limcc = imcc ;
    lv    = v ;
    lp    = p ;
  }

  return fabs(ecart) ;
}

int tas_get_best_ab_for_switch( cone_list *cone, link_list *link0, link_list *activelink, double vdd, double vt, lotrs_list *trspair, double *a, double *b, double *nvt )
{
 
  float   v1 ;
  int     r ;
  double  stepvt ;
  double  testvt ;
  int     onemoretime ;
  double  an ;
  double  bn ;
  int     i ;
  int     j ;
  int     n ;
  float   tabi[50] ;
  float   tabv[50] ;
  double  anmax ;
  double  bnmax ;
  float   ecart ;
  float   ecartmax ;
  float   i1 ;
  double  vgsdeg ;
  link_list *pairlink ;
  long    ldelta ;
  float   delta=0.0 ;

  if( trspair ) {
    tas_getinverterforswitch( cone, activelink, trspair, &pairlink, &ldelta ) ;
    if( ldelta != TAS_NOTIME )
      delta = ldelta / TTV_UNIT ;
  }

  /* attention : complexite en (n-1)^2 */
  n = 5 ;

  for( i=1 ; i<n ; i++ ) {
  
    v1 = vt + i*(vdd-vt)/n ;
    vgsdeg = v1 ;

    if( ( activelink->ULINK.LOTRS->TYPE & CNS_TN ) == CNS_TN ) 
      r = tpiv_get_i_multi_input(link0, activelink, vdd, 0.0, v1, vdd-vgsdeg, TAS_TRMODEL_MCCRSAT, &i1);
    else 
      r = tpiv_get_i_multi_input(link0, activelink, 0.0, vdd, vdd - v1, vgsdeg, TAS_TRMODEL_MCCRSAT, &i1);

    if( !r ) {
      avt_log( LOGTAS, 1, "tpiv_get_i_multi_input() failled in tas_get_best_ab_for_switch()\n" );
      return 0 ;
    }

    tabi[i-1] = fabs(i1) ;
    tabv[i-1] = v1 ;
      
  }

  /* 1er test : trouve A et B positifs sans toucher au VT */

  ecartmax = FLT_MAX ;
  for( i=1 ; i<n ; i++ ) {
    for( j=i+1 ; j<n ; j++ ) {
      mcc_calcul_ab( tabv[i-1], tabv[j-1], tabi[i-1], tabi[j-1], vt, 1.0, 1.0, &an, &bn );
      if( an > 0.0 && bn > 0.0 ) {
        ecart = tas_get_best_ab_for_switch_error( tabv, tabi, n, an, bn, vt );
        if( ecart < ecartmax ) {
          ecartmax = ecart ;
          anmax    = an ;
          bnmax    = bn ;
        }
      }
    }
  }

  if( ecartmax != FLT_MAX ) {
    *a   = anmax ;
    *b   = bnmax ;
    *nvt = vt ;
  }
  else {

    /* 2eme test : modifie le vt */
    for( i=1 ; i<n ; i++ ) {
    
      for( j=i+1 ; j<n ; j++ ) {

        stepvt = (vdd - vt) / 30.0 ;
        testvt = vt +stepvt ;
        
        do {

          onemoretime = 1 ;
        
          mcc_calcul_ab( tabv[i-1], tabv[j-1], tabi[i-1], tabi[j-1], stepvt, 1.0, 1.0, &an, &bn );
        
          if( an > 0.0 && bn > 0.0 )
            onemoretime = 0 ;
          else {
            testvt = testvt + stepvt ;
            if( testvt >= vdd ) 
              onemoretime = 0 ;
          }
        }
        while( onemoretime );
       
       if( testvt < vdd )
         break ;
      }

      if( j<n )
        break ;
    }

    if( i<n ) {
      *a = an ;
      *b = bn ;
      *nvt = testvt ;
    }
    else

    return 0 ;
  }

  return 1 ;
}

int tas_get_ab_for_branch_0( double ax,
                             double bx,
                             double wl,
                             double r2,
                             double vdd,
                             double vt,
                             double imax,
                             double *pta,
                             double *ptb
                           )
{
  double i ;
  int    found ;

  found = 0 ;
  
  if( r2 > 0.0 ) {
    i = tas_get_i( ax * wl, bx, r2, vdd / 4.0);
    *ptb = tas_get_b( vdd - vt, vdd / 4.0, imax, i);
    *pta = tas_get_a( vdd - vt, imax, *ptb );

    if( mcc_check_sat_param( *pta, *ptb, vt, vdd, 'B' ) )
      found = 1 ;
  }

  return found ;
}

int tas_get_ab_for_branch_1_2( link_list *headlink,
                               link_list *activelink,
                               double     vdd,
                               double     vt,
                               double     imaxbr,
                               double     *pta,
                               double     *ptb
                             )
{
  double v1 ;
  float  fi ;
  double i_int ;
  int    found ;
  int    r ;
  int    onemoretime ;

  v1 = vt + (vdd - vt) / 2.0;

  do {

    if ((activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
      r = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  vdd, 
                                  0.0,
                                  v1, 
                                  vdd - v1, 
                                  TAS_TRMODEL_MCCRSAT,
                                  &fi
                                );
    else
      r = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  0.0, 
                                  vdd,
                                  vdd - v1, 
                                  v1, 
                                  TAS_TRMODEL_MCCRSAT,
                                  &fi
                                );

    i_int = fabs(fi);

    if( r ) {

      *pta = mcc_calcul_a( v1, vdd, i_int, imaxbr, vt, 1.0, 1.0);
      *ptb = mcc_calcul_b( v1, vdd, i_int, imaxbr, vt, 1.0, 1.0);

      if( mcc_check_sat_param( *pta, *ptb, vt, vdd, 'B' ) ) {
        onemoretime = 0 ;
        found = 1 ;
      }
      else {
        onemoretime = 1 ;
        v1 = v1 + 0.05 * vt;
        if( v1 >= vdd ) {
          found = 0 ;
          onemoretime = 0 ;
        }
      }
    }
    else {
      onemoretime = 0 ;
      found = 0 ;
    }
  }
  while( onemoretime );

  return found ;
}

int tas_get_ab_for_branch_1_1( link_list *headlink,
                               link_list *activelink,
                               double     vdd,
                               double     vt,
                               double     imaxbr,
                               double     *pta,
                               double     *ptb,
                               double     *ptvt
                             )
{
  double v1 ;
  double stepvt ;
  double i_int ;
  float  fi ;
  int    r ;
  int    found ;
  int    onemoretime ;

  v1     = vt + (vdd - vt) / 2.0 ;
  stepvt = (vdd - vt) / 30.0 ;
  (*ptvt) = vt + stepvt ;

  if ((activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
    r = tpiv_get_i_multi_input( headlink, 
                                activelink, 
                                vdd, 
                                0.0,
                                v1, 
                                vdd - v1, 
                                TAS_TRMODEL_MCCRSAT,
                                &fi
                              ) ;
  else
    r = tpiv_get_i_multi_input( headlink, 
                                activelink, 
                                0.0, 
                                vdd,
                                vdd - v1, 
                                v1, 
                                TAS_TRMODEL_MCCRSAT,
                                &fi
                              ) ;

  if( !r )
    return 0 ;

  i_int = fabs(fi) ;
  
  do {

    (*pta) = mcc_calcul_a( v1, vdd, i_int, imaxbr, (*ptvt), 1.0, 1.0) ;
    (*ptb) = mcc_calcul_b( v1, vdd, i_int, imaxbr, (*ptvt), 1.0, 1.0);

    if( mcc_check_sat_param( *pta, *ptb, (*ptvt), vdd, 'B' ) ) {
      onemoretime = 0 ;
      found = 1 ;
    }
    else {
      onemoretime = 1 ;
      (*ptvt) = (*ptvt) + stepvt ;
      if( (*ptvt) >= v1 ) {
        onemoretime = 0 ;
        found = 0 ;
      }
    }
  }
  while( onemoretime );
  
  return found ;
}

int tas_get_ab_for_branch_1( link_list *headlink,
                             link_list *activelink,
                             double     vdd,
                             double     vt,
                             double     imax,
                             double     *pta,
                             double     *ptb,
                             double     *ptvt
                           )
{
  int r ;

  r = tas_get_ab_for_branch_1_2( headlink, 
                                 activelink, 
                                 vdd, 
                                 vt, 
                                 imax, 
                                 pta, 
                                 ptb 
                               );

  if( r == 1 ) 
    *ptvt = vt ;
  else
    r = tas_get_ab_for_branch_1_1( headlink, 
                                   activelink, 
                                   vdd, 
                                   vt, 
                                   imax, 
                                   pta, 
                                   ptb,
                                   ptvt
                                 );
  
  return r ;
}

int tas_get_ab_for_branch_2( link_list *headlink,
                             link_list *activelink,
                             double     vdd,
                             double     vt,
                             double     imaxbr,
                             double     *pta,
                             double     *ptb,
                             double     *ptvt
                           )
{

  double v1 ;
  double v2 ;
  float  i_int1 ;
  float  i_int2 ;
  int    r_1 ;
  int    r_2 ;
  int    r ;
  int    ret ;

  ret = 0 ;
  
  v1 = vt + 1.0*(vdd - vt) / 3.0;
  v2 = vt + 2.0*(vdd - vt) / 3.0;
  
  if ((activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
  
    r_1 = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  vdd, 
                                  0.0,
                                  v1, 
                                  vdd-v1, 
                                  TAS_TRMODEL_MCCRSAT,
                                  &i_int1
                                );
    r_2 = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  vdd, 
                                  0.0,
                                  v2, 
                                  vdd-v2, 
                                  TAS_TRMODEL_MCCRSAT,
                                  &i_int2
                                );
  }
  else {
    r_1 = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  0.0, 
                                  vdd,
                                  vdd - v1, 
                                  v1, 
                                  TAS_TRMODEL_MCCRSAT,
                                  &i_int1
                                );
    r_2 = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  0.0, 
                                  vdd,
                                  vdd - v2, 
                                  v2, 
                                  TAS_TRMODEL_MCCRSAT,
                                  &i_int2
                                );
  }

  if( r_1 && r_2 ) {
    i_int1 = fabs( i_int1 );
    i_int2 = fabs( i_int2 );

    r = mcc_get_sat_parameter( v1, i_int1,
                               v2, i_int2,
                               vdd, imaxbr,
                               pta,
                               ptb,
                               ptvt
                             );
    if( r ) 
      ret = 1 ;
  }

  return ret ;
}

int tas_get_ab_for_branch_3( link_list *headlink,
                             link_list *activelink,
                             double     vdd,
                             double     vt,
                             double     imax,
                             double    *pta,
                             double    *ptb,
                             double    *ptvt
                           )
{
  double dv ;
  double valim ;
  double vout ;
  double sgn ;
  double vgs[3] ;
  double v[3] ;
  double imcc[3] ;
  float  ir ;
  int    n ;
  int    r ;
  int    i ;
  int    onemoretime ;
  int    found ;
  static int fordebug ;

  fordebug++ ;
  
  dv = vdd-vt ;

  if( (activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN ) {
    valim = 0.0 ;
    vout  = vdd ;
    sgn   = 1 ;
  }
  else {
    valim = vdd ;
    vout  = 0.0 ;
    sgn   = -1 ;
  }

  n = 0 ;
  
  do {

    n++ ;
 
    vgs[0] = vt + 1.0*dv/3.0 ;
    vgs[1] = vt + 2.0*dv/3.0 ;
    vgs[2] = vt + 3.0*dv/3.0 ;

    for( i=0 ; i<=2 ; i++ )
      v[i] = sgn * vgs[i] + valim ;

    for( i=0 ; i<=2 ; i++ ) {
      r = tpiv_get_i_multi_input( headlink, 
                                  activelink, 
                                  vout,
                                  valim, 
                                  v[i], 
                                  vdd-v[i], 
                                  TAS_TRMODEL_MCCRSAT,
                                  &ir
                                );
      if( !r ) {
        /*
        printf( "tas_get_ab_for_branch_3() : current failled (fordebug=%d)\n", fordebug );
        exit(1);
        */
        return 0 ;
      }

      if( (activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN ) 
        imcc[i] = -ir ;
      else
        imcc[i] = ir ;
    }

    r = mcc_get_sat_parameter( vgs[0], imcc[0],
                               vgs[1], imcc[1],
                               vgs[2], imcc[2],
                               pta,
                               ptb,
                               ptvt
                             );

    if( r ) {
      onemoretime = 0 ;
      found = 1 ;
    }
    else {
      n++ ;
      if( n>=20 ) {
        onemoretime = 0 ;
        found = 0 ;
      }
      else {
        dv = pow(0.9,n)*dv ;
        onemoretime = 1 ;
      }
    }
  }
  while( onemoretime );

  /*
  if( !found ) {
    printf( "tas_get_ab_for_branch_3() : convergence failled (fordebug=%d)\n", fordebug );
    exit(1);
  }
  */
  return found ;
}
                           

void check_branch_model( branch_list *branch, 
                         link_list   *active, 
                         double       vt, 
                         double       a, 
                         double       b, 
                         double       imax,
                         double       vdd 
                       )
{
  static FILE   *file ;
  link_list     *link ;
  double         valim ;
  double         vout ;
  double         vin0 ;
  double         vin1 ;
  double         iref ;
  double         ieqv ;
  double         liref ;
  double         lieqv ;
  double         seqv ;
  double         sref ;
  double         stref ;
  double         error ;
  double         s ;
  double         v ;
  double         dv ;
  double         adv ;
  double         vgs ;
  float          ir ;
  char          *nbr ;
  int            n ;
  int            i ;
  int            r ;

  if( ( branch->TYPE & CNS_PARALLEL ) == CNS_PARALLEL )
    return ;

  for( link = branch->LINK ; link ; link = link->NEXT ) {
    if( ( link->TYPE & CNS_EXT ) == CNS_EXT )
      break ;
    if( getptype( link->ULINK.LOTRS->USER, MBK_TRANS_PARALLEL ) )
      break ;
    if( getptype( link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH ) )
      break ;
  }

  if( !link ) {
  
    if( ( branch->TYPE & CNS_VDD ) == CNS_VDD ) {
      valim = vdd ;
      vout  = 0.0 ;
      vin0  = vdd ;
      vin1  = 0.0 ;
      nbr   = "vdd" ;
    }
    else {
      valim = 0.0 ;
      vout  = vdd ;
      vin0  = 0.0 ;
      vin1  = vdd ;
      nbr   = "vss" ;
    }

    n = 100 ; /* nbr d'intervalles */
    dv = (vin1-vin0)/((double)n) ;
    adv = fabs(dv) ;

    s     = 0.0 ;
    stref = 0.0 ;
    
    for( i=0 ; i<=n ; i++ ) {
    
      v = vin0 + ((double)i)*dv ;
      
      r = tpiv_get_i_multi_input( branch->LINK,
                                  active,
                                  vout,
                                  valim,
                                  v,
                                  vdd-v,
                                  TAS_TRMODEL_SPICE,
                                  &ir
                                );
                                
      if( ( branch->TYPE & CNS_VDD ) == CNS_VDD ) 
        iref = ir ;
      else
        iref = -ir ;

      vgs = fabs(v-valim) ;
      if( vgs>vt ) {
        ieqv = a*(vgs-vt)*(vgs-vt)/(1.0+b*(vgs-vt));
        if( ieqv > imax )
          ieqv = imax ;
      }
      else
        ieqv = 0.0 ;

      if( i>=1 ) {
        seqv = adv*(ieqv+lieqv)/2.0 ;
        sref = adv*(iref+liref)/2.0 ;
        s = s + fabs(seqv-sref);
        stref = stref + sref ;
      }

      lieqv = ieqv ;
      liref = iref ;
    }
    
    error = s/stref ;

    if( !file ) {
      file = fopen( "brancherror.txt", "w" ) ;
    }

    fprintf( file, "%s %g\n", nbr, error );
  }

}

/*-----------------------------------------------------------------------------
*                    tpd_dualmodel()                                          *
-----------------------------------------------------------------------------*/
timing_scm *tpd_dualmodel(cone, branch, activelink, pconf0, pconf1, brconf, pqy,
                          delayrc, fin0, capa, fb, vtp, vtp0, cgpc, cgdc, cgpoc, cgdce, event,
                          gate_th)
cone_list *cone;
branch_list *branch;
link_list *activelink;
double pconf0;
double pconf1;
branch_list *brconf;
double pqy;
float delayrc;
float fin0;
float capa;
float fb;
float vtp;
float vtp0;
double cgpc;
double cgdc;
double cgpoc;
double *cgdce;
char event;
double gate_th;
{
  link_list *link0;
  double r2 = 0.0,
      wl = 0.0,
      AX = 0.0,
      BX = 0.0,
      rx = 0.0,
      rtx = 0.0,
      QY = 0.0,
      QX = 0.0,
      vddmax = 0.0,
      imax = 0.0,
      imaxbr = 0.0,
      an = 0.0,
      bn = 0.0,
      anbr = 0.0,
      bnbr = 0.0,
      vtbr = 0.0,
      VT = 0.0,
      vddin = 0.0,
      thr = 0.0,
      capai = 0.0, 
      Rbr = 0.0, 
      Cbr = 0.0, 
      irap = 0.0, 
      t_int = 0.0 ;
  double capaie[4];
  double capao=0.0;
  float tab[STM_NB_DUAL_PARAMS];
  char fname[255];
//  static char env_SIMUINV = 'u', *SIMUINVCONENAME = NULL;
  static int fordebug = 0;
  double cgpa, cgda, cgdae[3], cgpoa, kf;
  char *env;
  link_list *lastlink;
  link_list *baselink;
  double i_rsat;
  ptype_list *ptype ;
  lotrs_list *trspair ;
  char branchcontainswitch ;
  double ihalf ;
  double isat, vsat ;
  double iint, vint ;
  static char env_DISPLAYCURRENT = 'n', *DISPLAYCURRENT = NULL ;
  float valim ;
  float vout_d ;
  float vout_f ;
  float vin_d ;
  float vin_f ;
  double chalf ;
  double qinit ;
  double im ;
  double rsat ;
  double vcap[3] ;
  double vin ;
  float fin ;
  float rconf ;
  double va, vb, vac, vbc, vt0c, vt0 ;
  int level ;
  int n;
  timing_scm *stmmodel=NULL ;
  int gfound ;

  fordebug++;

  link0 = branch->LINK ;

  if (!link0)
    return NULL;

  branchcontainswitch = isbranchcontainswitch( link0 );
  /* parametres MCC */
  tas_getmcc( link0, 
              activelink, 
              &vddin, 
              &AX, 
              &BX, 
              &rtx, 
              &rx, 
              &VT, 
              &vt0, 
              &QX, 
              &QY,
              &thr, 
              &vddmax, 
              &cgpa, 
              &cgda, 
              cgdae, 
              &cgpoa
            );

  if(vtp < 0.0) vtp = VT;
  if(vtp0 < 0.0) vtp0 = vt0;
  if (!tas_check_prop(activelink, vddmax, vddin, VT, gate_th)) {
    TAS_PVT_COUNT++;
    tas_error(70, cone->NAME, TAS_WARNING);
    return NULL;
  }

  tas_getwlr2effdual(activelink, rtx, &wl, &r2);
  tas_getRCbranch(activelink, &Rbr, &Cbr);
  an = tas_get_an(r2, wl, AX, BX);
  bn = tas_get_bn(r2, wl, AX, BX);

  /* courant de branche */
  TPIV_DEBUG_IBR = 1 ;
  imax = tas_get_current( branch, link0, 1.0, NO);
/*  if (TAS_PATH_TYPE == 'M'){
    ileakage = tas_get_current_leakage( branch, link0, activelink);
    fprintf(stdout, "Ileakage %s %s (%s): %g\n", ((cone_list *)getptype( activelink->ULINK.LOTRS->USER, CNS_DRIVINGCONE )->DATA)->NAME, cone->NAME, ( ( branch->TYPE & CNS_VSS ) == CNS_VSS )? "VSS":"VDD", ileakage);
  }*/
  if (imax < 1e-8){
    TAS_PVT_COUNT++;
    tas_error(70, cone->NAME, TAS_WARNING);
    return NULL;
  }

  gfound = 0 ;

  if( V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE && branchcontainswitch ) {

    trspair = NULL ;
    if( ( activelink->TYPE & CNS_SWITCH ) == CNS_SWITCH ) {
      ptype = getptype( activelink->ULINK.LOTRS->USER, TAS_TRANS_SWITCH );
      if( ptype ) {
        trspair = (lotrs_list*)ptype->DATA ;
      }    
    }

    gfound = tas_get_best_ab_for_switch( cone, link0, activelink, vddmax, VT, trspair, &anbr, &bnbr, &vtbr ) ;
  }
  else {

    baselink = NULL ;
    lastlink = tas_get_last_active_link(activelink);
    
    if( link0->NEXT ) {
      if (lastlink != activelink ) {
        baselink = link0 ;
        imaxbr = imax ;
      }
      else {
        if( V_INT_TAB[ __TAS_USE_ENHANCED_BRANCH_FIT ].VALUE > 0 ) {
          baselink = activelink ;
          /* courant de branche */
          imaxbr = tas_get_current( branch, baselink, 1.0, NO);
          if (imaxbr < 1e-8){
            tas_error(70, cone->NAME, TAS_WARNING);
            return NULL;
          }
        }
      }
    }

    level = V_INT_TAB[ __TAS_USE_ENHANCED_BRANCH_FIT ].VALUE ;
    if( activelink != lastlink ) 
      if( level < 1 )
        level = 1 ;

    switch( level )
    {
    case 3 :
      if( link0->NEXT ) {
        gfound = tas_get_ab_for_branch_3( branch->LINK,
                                          activelink,
                                          vddmax,
                                          VT,
                                          imax,
                                          &anbr,
                                          &bnbr,
                                          &vtbr
                                        );
        if( gfound ) break ;
      }
    case 2 :
      if( baselink && activelink->NEXT ) {
        gfound = tas_get_ab_for_branch_2( baselink,
                                     activelink,
                                     vddmax,
                                     VT,
                                     imaxbr,
                                     &anbr,
                                     &bnbr,
                                     &vtbr
                                   );
      }
      if( gfound ) break ;
    case 1 :
      if( baselink && activelink->NEXT ) {
        gfound = tas_get_ab_for_branch_1( baselink,
                                     activelink,
                                     vddmax,
                                     VT,
                                     imaxbr,
                                     &anbr,
                                     &bnbr,
                                     &vtbr
                                   );
      }
      if( gfound ) break ;
    default :
      gfound = tas_get_ab_for_branch_0( AX,
                                        BX,
                                        wl,
                                        r2,
                                        vddmax,
                                        VT,
                                        imax,
                                        &anbr,
                                        &bnbr
                                      );
      vtbr = VT ;
    }
  }

  if( gfound ) {
    vddin = vddin + VT - vtbr ;
    VT = vtbr ;
    an = anbr;
    bn = bnbr;
  }

  if( V_BOOL_TAB[ __TAS_CHECK_BRANCH_MODEL ].VALUE ) 
    check_branch_model( branch, activelink, VT, an, bn, imax, vddmax );
  
  tas_get_current( branch, link0, 0.5, YES);

  /* For now, uses only the rsat effect in tas_getcapaparams() if 
  the branch contains switches and the new model is enabled */
  chalf = 0.0 ;

  if( V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE && branchcontainswitch ) {
    chalf =
      tas_getcapaparams(cone, branch, tas_get_last_active_link(activelink),
                        imax, QX, QY, &capai, &irap, &t_int, event, 1 );
    pconf0 = pconf0 + chalf ;
  }
  else
    pconf0 +=
      tas_getcapaparams(cone, branch, tas_get_last_active_link(activelink),
                        imax, QX, QY, &capai, &irap, &t_int, event, 0 );

  /* parametres de capacite intrinseque */

  tas_clean_vpol(link0);

  kf = 1.0 ;

  if (cgda >= 0.0 && cgpa >= 0.0 && cgdc >= 0.0 && cgpc >= 0.0) {

    /* facteur de prise en compte de l'overshoot déchargé par le 
       transistor conflictuel */
    kf = 1.0 - (vddmax - vtp - VT) / (vddmax - vtp);

    if( kf > 1.0 )
      kf = 1.0 ;

    capai = 0.0 ;

    capai += ( cgpa + cgpc ) * ( 1.0 + VT*(kf-1.0)/vddmax ) ;
    capai += cgda ;
    capai += cgdc * ( 1.0 + VT*(kf-1.0)/(vddmax-vtp) );

  } else {
    capai += ((QY * (double) TAS_GETWIDTH(activelink) / SCALE_X) + pqy);
  }

  vt0c = vddmax-vtp0 ;

  if( !V_BOOL_TAB[ __TAS_ENHANCED_CAPAI ].VALUE ) {
    vt0 = VT ;
    vt0c = vddmax-vtp ;
  }

  if( cgdae[0] >= 0.0 && cgdae[1] >= 0.0 && cgdae[2] >= 0.0 &&
      cgdce[0] >= 0.0 && cgdce[1] >= 0.0 && cgdce[2] >= 0.0 && 
      cgpa >= 0.0 && cgpc >= 0.0 ) {

    if( vt0 < vddmax/2.0 ) {
      va = vt0 ;
      vb = vddmax/2.0 ;
    }
    else {
      va = vddmax/2.0 ;
      vb = vt0 ;
    }


    if( vt0c < vddmax/2.0 ) {
      vac = vt0c ;
      vbc = vddmax/2.0 ;
    }
    else {
      vac = vddmax/2.0 ;
      vbc = vt0c ;
    }

    vcap[0] = vt0 ;
    vcap[1] = vddmax/2.0 ;
    vcap[2] = vt0c ;
    
    qsort( vcap, 3, sizeof(double), (int(*)(const void*,const void*))mbk_qsort_dbl_cmp );
    vcap[3] = vddmax ;
    
    if( !V_BOOL_TAB[ __TAS_USE_KF ].VALUE )
      kf=1.0;

    for( n=0 ; n<4 ; n++ ) {
      capaie[n] = cgpa+cgpc ;
      
      if( vcap[n] <= va )
        capaie[n] = capaie[n]+cgdae[0] ;
      else {
        if( vcap[n] <= vb )
          capaie[n] = capaie[n]+cgdae[1] ;
        else
          capaie[n] = capaie[n]+cgdae[2] ;
      }
      
      if( vcap[n] <= vac )
        capaie[n] = capaie[n]+cgdce[2] ;
      else {
        if( vcap[n] <= vbc )
          capaie[n] = capaie[n]+cgdce[1] ;
        else
          capaie[n] = capaie[n]+cgdce[0] ;
      }

      if( vcap[n] <= vt0 )
        capaie[n] = capaie[n]*kf ;
    }
    
  }
  else {
    capaie[0] = -1.0 ;
    capaie[1] = -1.0 ;
    capaie[2] = -1.0 ;
    capaie[3] = -1.0 ;
  }

  capao = cgpoa + cgpoc ;

  if (activelink->TYPE != CNS_EXT) {
    losig_list *sigdrain = NULL;
    lotrs_list *t1, *t2;
    link_list *ptlink;
    link_list *lastlink;
    float c;
    float k;

    if (activelink == link0) {
      k = 1.0;
      sigdrain = tas_getlosigcone(cone);
    } else {
      k = QX;
      lastlink = NULL;
      for (ptlink = link0; ptlink != activelink; ptlink = ptlink->NEXT)
        lastlink = ptlink;
      t1 = lastlink->ULINK.LOTRS;
      t2 = activelink->ULINK.LOTRS;
      if (t1->SOURCE->SIG == t2->DRAIN->SIG)
        sigdrain = t1->SOURCE->SIG;
      if (t1->SOURCE->SIG == t2->SOURCE->SIG)
        sigdrain = t1->SOURCE->SIG;
      if (t1->DRAIN->SIG == t2->DRAIN->SIG)
        sigdrain = t1->DRAIN->SIG;
      if (t1->DRAIN->SIG == t2->SOURCE->SIG)
        sigdrain = t1->DRAIN->SIG;
    }
    if (sigdrain) {
      c = 1000.0 * rcn_getcouplingcapacitance(TAS_CONTEXT->TAS_LOFIG,
                                              sigdrain,
                                              activelink->ULINK.
                                              LOTRS->GRID->SIG) * k;

      if( V_BOOL_TAB[ __AVT_CAPAI_KF_RC ].VALUE )
        capai = capai + c * ( 1.0 + VT*(kf-1.0)/vddmax ) ;
      else
        capai = capai + c ;

      if( capaie[0] >= 0.0 ) {
        for( n=0 ; n<4 ; n++ ) 
          capaie[n] = capaie[n]+c ;
      }
    }

  }

  if( event=='D' ) 
    /* event D : input is U */
    fin = stm_thr2scm( fin0, 
                       STM_DEFAULT_SMINR,
                       STM_DEFAULT_SMAXR,
                       VT,
                       tas_get_vf_input(activelink),
                       tas_get_vf_input(activelink), 
                       STM_UP
                     );
  else
    /* event U : input is D */
    fin = stm_thr2scm( fin0, 
                       STM_DEFAULT_SMINF,
                       STM_DEFAULT_SMAXF,
                       VT,
                       0.0,
                       tas_get_vf_input(activelink), 
                       STM_DN
                     );
  if (fb > 0.0)
    pconf0 += fb * fin;

  qinit = 0.0 ;

  
  if(  V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE     &&
      !V_BOOL_TAB[ __AVT_SYMETRICAL_SWITCH ].VALUE    &&
      ( activelink->TYPE & CNS_SWITCH ) == CNS_SWITCH    ) {

    tas_getcdeltaswitch( cone, 
                         link0, 
                         activelink,
                         brconf ? brconf->LINK : NULL,
                         pconf0+tas_getcapalink(NULL,link0,TAS_UNKNOWN_EVENT),
                         fin,
                         event=='U'?'D':'U',
                         &pconf0,
                         &pconf1,
                         &chalf,
                         &qinit
                       );
  }

  rconf = -1.0 ;
  if( brconf )
    rconf = tas_get_rlin_br( brconf->LINK );
  
  kf = -1.0 ;
  if( V_INT_TAB[ __STM_OVERSHOOT_LEVEL ].VALUE > 0 && brconf ) {
    if( rconf > 0.0 ) {
      if( VT < vddmax-vtp ) 
        kf = VT/(vddmax-VT) ;
      else {
        if( V_INT_TAB[ __STM_OVERSHOOT_LEVEL ].VALUE == 2 ) {
          kf = (vddmax-vtp)/(vddmax-VT);
        }
      }
    }
  }

  tab[STM_PCONF0] = (float) pconf0 ;
  tab[STM_CHALF] = chalf ;
  tab[STM_PCONF1] = (float) pconf1;
  tab[STM_CAPAI] = (float) capai;

  if( V_BOOL_TAB[ __TAS_ENHANCED_CAPAI ].VALUE ) {
    tab[STM_CAPAI0] = capaie[0] ;
    tab[STM_CAPAI1] = capaie[1] ;
    tab[STM_CAPAI2] = capaie[2] ;
    tab[STM_CAPAI3] = capaie[3] ;
  }
  else {
    tab[STM_CAPAI0] = capai ;
    tab[STM_CAPAI1] = capai ;
    tab[STM_CAPAI2] = capai ;
    tab[STM_CAPAI3] = capai ;
  }
  tab[STM_VT0]     = vt0 ;
  tab[STM_VT0C]    = vt0c ;

  if( V_BOOL_TAB[ __AVT_CAPAO_FOR_CTK ].VALUE )
    tab[STM_CAPAO] = (float) capao;
  else
    tab[STM_CAPAO] = (float) capai;
  tab[STM_IRAP] = (float) irap;
  tab[STM_VDDIN] = (float) vddin;
  tab[STM_VT] = (float) VT;
  tab[STM_THRESHOLD] = (float) thr;
  tab[STM_IMAX] = (float) imax;
  tab[STM_AN] = (float) an;
  tab[STM_BN] = (float) bn;
  tab[STM_VDDMAX] = (float) vddmax;
  tab[STM_RLIN] = (float) tas_get_rlin_br(link0);
  tab[STM_KRT] = tas_get_rlin_br_krt( link0 );
  tab[STM_RINT] = -1.0 ;
  tab[STM_VINT] = -1.0 ;
  switch (TAS_CONTEXT->TAS_LEVEL) {
  case 1:
  case 2:
    if( link0->NEXT ) {
      if( branchcontainswitch && V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ) {
        tas_get_point_rsat_br(branch, vddmax, imax, &isat, &vsat, &iint, &vint );
        rsat = (vddmax-vsat)/(imax-isat);
        tab[STM_RLIN] = vint/iint ;
        tab[STM_RINT] = (vsat-vint)/(isat-iint) ;
        tab[STM_VINT] = vint ;
      }
      else {
        ihalf = tas_get_current( branch, link0, 0.5, NO );
        i_rsat = imax - ihalf ;
        if( i_rsat/imax < 0.01 )
          rsat = -1.0;
        else
          rsat = 0.5*vddmax / i_rsat;
      }
      tab[STM_RSAT] = rsat;
    } else {
      im = an*(vddmax-VT)*(vddmax-VT)/(1.0+bn*(vddmax-VT));
      vin = tpiv_get_voltage_driving_lotrs( link0->ULINK.LOTRS );
      if( MLO_IS_TRANSP( activelink->ULINK.LOTRS->TYPE ) )
        vin = vddmax - vin ;
      stm_modscm_dual_calc_rsat( im,   /* maximum current when all input are set to vdd */
                                 imax, /* maximum current when all input are set to their maximum value */
                                 an, 
                                 bn, 
                                 tas_getparam( activelink->ULINK.LOTRS, TAS_CASE, TP_RT) / wl,
                                 tas_getparam( activelink->ULINK.LOTRS, TAS_CASE, TP_RS) / wl,
                                 -1.0,
                                 -1.0,
                                 vddmax,
                                 VT,
                                 vin,
                                 &rsat,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL
                               );
      tab[STM_RSAT] = rsat ;
    }

    break;
  default:
    tab[STM_RSAT] = -1.0;
  }
  tab[STM_DRC] = (float) (delayrc + t_int );
  tab[STM_RBR] = (float) Rbr;
  tab[STM_CBR] = (float) Cbr;
  tab[STM_INPUT_THR] =
      (float) (tas_get_vdd_input(activelink) * thr / vddmax);
  tab[STM_RCONF] = rconf ;
  tab[STM_KF] = kf ;
  tab[STM_QINIT] = qinit ;
  
  stmmodel = stm_modscm_dual_create(tab);

  if( env_DISPLAYCURRENT == 'n' ) {
    env = getenv( "DISPLAYCURRENT" );
    if( env ) 
      DISPLAYCURRENT = namealloc( env );
    env_DISPLAYCURRENT = 'y' ;
  }

  if( DISPLAYCURRENT == cone->NAME ) {
    if ((activelink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN) {
      sprintf( fname, "current_ud_%s", activelink->ULINK.LOTRS->TRNAME );
      valim=0.0 ;
      vin_d=0.0 ;
      vin_f=vddmax ;
      vout_d=0.0 ;
      vout_f=vddmax ;
    }
    else {
      sprintf( fname, "current_du_%s", activelink->ULINK.LOTRS->TRNAME );
      valim=vddmax ;
      vin_d=vddmax ;
      vin_f=0.0 ;
      vout_d=vddmax ;
      vout_f=0.0 ;
    }
    tpiv_i_trace_brdual( fname, link0, activelink, TAS_TRMODEL_MCCRSAT, vin_d, vin_f, vout_d, vout_f, valim, vddmax );
  }

  capa = 0.0 ;
  return stmmodel ;
}

/*-----------------------------------------------------------------------------
*                    tas_add_power_model()                                    *
*                                                                             *
-----------------------------------------------------------------------------*/
void tas_add_power_model( timing_model *tm, timing_scm *scm )
{
    float stm_e_params[STM_NB_ENERGY_PARAMS];
    float pconf0;
    float pconf1;
    
    if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 )
     ||( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 5 )){
        switch (scm->TYPE) {
            case STM_MODSCM_DUAL:
                pconf0 = scm->PARAMS.DUAL->DP[STM_PCONF0];
                pconf1 = scm->PARAMS.DUAL->DP[STM_PCONF1];
                break;
            case STM_MODSCM_GOOD:
                pconf0 = scm->PARAMS.GOOD->DP[STM_PCONF0];
                pconf1 = scm->PARAMS.GOOD->DP[STM_PCONF1];
                break;
            case STM_MODSCM_FALSE:
                pconf0 = scm->PARAMS.FALS->FP[STM_PCONF0];
                pconf1 = scm->PARAMS.FALS->FP[STM_PCONF1];
                break;
            case STM_MODSCM_PATH:
                pconf0 = scm->PARAMS.PATH->PP[STM_PCONF0];
                pconf1 = 0.0;
                break;
            case STM_MODSCM_CST:
                pconf0 = 0.0;
                pconf1 = 0.0;
                break;
        }
        stm_e_params [STM_EPCONF0] = pconf0;
        stm_e_params [STM_EPCONF1] = pconf1;
        tm->ENERGYTYPE = STM_ENERGY_PARAMS;
        tm->ENERGYMODEL.EPARAMS = stm_energy_params_create (stm_e_params);
    }
}

/*-----------------------------------------------------------------------------
*                    val_dual()                                               *
*                                                                             *
* fonction qui calcule  les  4 temps pour les cones en dual cmos              *
* les mets dans la structure delay_list                                       *
-----------------------------------------------------------------------------*/

delay_list *val_dual(cone, input, delay, fcarac, ccarac)
cone_list *cone;
cone_list *input;
delay_list *delay;
long fcarac;
double ccarac;
{
  link_list *link0, *activelink;
  branch_list *branch;
  double pconf0 = 0.0, pconf1 = 0.0, pqy = 0.0, gate_th = 0.0, vtp = -1.0, vtp0=-1.0;
  double fin;
  double fin0;
  double cgpc, cgdc, cgpoc, cgdce[3];
  timing_scm *scm;
  timing_iv *iv;
  float vt, vth, vend, vdd;
  float vtin, vthin, vendin, vddin;
  stm_pwl *pwlin = NULL, *pwlout = NULL, **ptpwl;
  float rin, cin, vsatin;
  branch_list *brconf;


  if (V_BOOL_TAB[__STM_USE_MSC].VALUE)
    ptpwl = &pwlout;
  else
    ptpwl = NULL;

  /* TPHL */
  branch = existe_tpd(cone, input, CNS_VSS, CNS_TN, &activelink);
  if (!branch)
    link0 = NULL;
  else
    link0 = (link_list *) (branch->LINK);

  if (link0) {
    fin0 = tas_getfin(activelink, cone, fcarac, &pwlin, &rin, &cin, &vsatin);
    if ((fin0 == TAS_NOFRONT_DBL) || tas_disable_gate_delay(input->NAME, cone, 1, 0) || tas_is_degraded_memsym(branch, cone, 'D')) {
      delay->TPHL = TAS_NOTIME;
    } else {
      if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
        delay->TMHL =
            stm_getmodel(CELL,
                         stm_mod_name(input->NAME, 'U', cone->NAME,
                                      'D', STM_DELAY | STM_SLEW,
                                      TAS_PATH_TYPE, 0));
      } else {
        tas_getconflictparam(cone, input, CNS_VDD, CNS_TP, link0,
                             activelink, &pconf0, &pconf1, &pqy,
                             &gate_th, &vtp, &vtp0, &cgpc, &cgdc, &cgpoc, cgdce, &brconf, 'D');
        tas_getparamsforslope(cone, activelink, 'D', &vt, &vth,
                              &vend, &vdd);

        switch (TAS_CONTEXT->TAS_LEVEL) {
        case 0:
        case 1:
        case 2:
          scm =
              tpd_dualmodel(cone, branch, activelink, pconf0,
                            pconf1, brconf, pqy, 0.0, fin0/TTV_UNIT,
                            tas_getcapai(cone), -1.0, vtp, vtp0, cgpc,
                            cgdc, cgpoc, cgdce, 'D', gate_th);
          if (scm) {
            delay->TMHL = stm_mod_create(NULL);
            delay->FMHL = NULL;
            delay->TMHL->UMODEL.SCM = scm;
            delay->TMHL->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMHL, scm);
          }
          break;
        case 3:
        case 4:
          delay->TMHL = stm_mod_create(NULL);
          delay->FMHL = NULL;
          tas_getparamsforinputslope(input,
                                     'U', &vtin, &vthin, &vendin, &vddin);
          iv = tpiv_dualmodel(cone, branch, activelink, 0.0, pconf0,
                              pconf1, 0.0, vendin, vthin, vdd);
          delay->TMHL->UMODEL.IV = iv;
          delay->TMHL->UTYPE = STM_MOD_MODIV;
          break;
        }
        if (delay->TMHL) {
          stm_mod_update(delay->TMHL, vth, vdd, vt, vend);
          stm_mod_update_transition(delay->TMHL, STM_HL);
          if (stm_noise_getmodeltype(delay->TMHL) == STM_NOISE_NULL)
            stm_noise_create(delay->TMHL, STM_NOISE_SCR);
          stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                     (delay->TMHL), (float) gate_th);
        }
      }
      if (delay->TMHL) {
        fin =
            stm_thr2scm( fin0 / TTV_UNIT, 
                         STM_DEFAULT_SMINR,
                         STM_DEFAULT_SMAXR,
                         stm_mod_vt( delay->TMHL ),
                         tas_get_vf_input(activelink),
                         tas_get_vf_input(activelink), 
                         STM_UP
                       );
        tas_compute_delays(delay->TMHL, cone, fin, rin, cin, vsatin, pwlin, ptpwl,
                           &delay->TPHL, &delay->FHL,
                           &delay->CARAC, link0, input->NAME,
                           SIM_RISE, SIM_FALL);
#ifdef USEOLDTEMP
        stm_pwl_destroy(delay->PWLTPHL);
        if (pwlout)
          delay->PWLTPHL = pwlout;
        else
          delay->PWLTPHL = NULL;
#endif
#ifdef USEOLDTEMP
        if (delay->TMHL->UTYPE == STM_MOD_MODSCM)
          delay->SHL =
              (long) (stm_modscm_slope
                      (delay->TMHL->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
      } else {
        delay->TPHL = TAS_NOTIME;
      }
    }
  }
  affiche_time(cone, input, "TPHL", delay->TPHL);
  affiche_time(cone, input, "FHL", delay->FHL);

  /* TPLH */
  branch = existe_tpd(cone, input, CNS_VDD, CNS_TP, &activelink);
  if (!branch)
    link0 = NULL;
  else
    link0 = (link_list *) (branch->LINK);

  if (link0) {
    fin0 = tas_getfin(activelink, cone, fcarac, &pwlin, &rin, &cin, &vsatin);
    if ((fin0 == TAS_NOFRONT_DBL) || tas_disable_gate_delay(input->NAME, cone, 0, 1) || tas_is_degraded_memsym(branch, cone, 'U')) {
      delay->TPLH = TAS_NOTIME;
    } else {
      if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
        delay->TMLH =
            stm_getmodel(CELL,
                         stm_mod_name(input->NAME, 'D', cone->NAME,
                                      'U', STM_DELAY | STM_SLEW,
                                      TAS_PATH_TYPE, 0));
      } else {
        tas_getconflictparam(cone, input, CNS_VSS, CNS_TN, link0,
                             activelink, &pconf0, &pconf1, &pqy,
                             &gate_th, &vtp, &vtp0, &cgpc, &cgdc, &cgpoc, cgdce, &brconf, 'U');
        tas_getparamsforslope(cone, activelink, 'U', &vt, &vth,
                              &vend, &vdd);
        switch (TAS_CONTEXT->TAS_LEVEL) {
        case 0:
        case 1:
        case 2:
          scm =
              tpd_dualmodel(cone, branch, activelink, pconf0,
                            pconf1, brconf, pqy, 0.0, fin0/TTV_UNIT,
                            tas_getcapai(cone), -1.0, vtp, vtp0, cgpc,
                            cgdc, cgpoc, cgdce, 'U', gate_th);
          if (scm) {
            delay->TMLH = stm_mod_create(NULL);
            delay->FMLH = NULL;
            delay->TMLH->UMODEL.SCM = scm;
            delay->TMLH->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMLH, scm);
          }
          break;
        case 3:
        case 4:
          delay->TMLH = stm_mod_create(NULL);
          delay->FMLH = NULL;

          tas_getparamsforinputslope(input,
                                     'D', &vtin, &vthin, &vendin, &vddin);
          iv = tpiv_dualmodel(cone, branch, activelink, vdd, pconf0,
                              pconf1, vtin, vendin, vthin, 0.0);
          delay->TMLH->UMODEL.IV = iv;
          delay->TMLH->UTYPE = STM_MOD_MODIV;
          break;
        }
        if (delay->TMLH) {
          stm_mod_update(delay->TMLH, vth, vdd, vt, vend);
          stm_mod_update_transition(delay->TMLH, STM_LH);
          if (stm_noise_getmodeltype(delay->TMLH) == STM_NOISE_NULL)
            stm_noise_create(delay->TMLH, STM_NOISE_SCR);
          stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                     (delay->TMLH), (float) gate_th);
        }
      }
      if (delay->TMLH) {
        fin =
            stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINF,
                        STM_DEFAULT_SMAXF,
                        stm_mod_vt(delay->TMLH), 
                        0.0,
                        tas_get_vf_input(activelink), STM_DN);
        tas_compute_delays(delay->TMLH, cone, fin, rin, cin, vsatin, pwlin, ptpwl,
                           &delay->TPLH, &delay->FLH,
                           &delay->CARAC, link0, input->NAME,
                           SIM_FALL, SIM_RISE);
#ifdef USEOLDTEMP
        stm_pwl_destroy(delay->PWLTPLH);
        if (pwlout)
          delay->PWLTPLH = pwlout;
        else
          delay->PWLTPLH = NULL;
#endif
#ifdef USEOLDTEMP
        if (delay->TMLH->UTYPE == STM_MOD_MODSCM) {
          delay->SLH =
              (long) (stm_modscm_slope
                      (delay->TMLH->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
        }
#endif
      } else {
        delay->TPLH = TAS_NOTIME;
      }
    }
  }
  affiche_time(cone, input, "TPLH", delay->TPLH);
  affiche_time(cone, input, "FLH", delay->FLH);

  /* TPHH */
  delay->TPHH = TAS_NOTIME;
  delay->FHH = TAS_NOFRONT;

  /* TPLL */
  delay->TPLL = TAS_NOTIME;
  delay->FLL = TAS_NOFRONT;

  ccarac = 0.0;
  return delay;
}

/*-----------------------------------------------------------------------------
*                    tas_memsym_loop_adjust()                                 *
-----------------------------------------------------------------------------*/
long tas_memsym_loop_adjust(long delay, cone_list *cone, cone_list *input, link_list *activelink)
{
    edge_list *edge;
    branch_list *branch;
    return delay;
    if ((cone->TYPE & CNS_MEMSYM) != CNS_MEMSYM) return delay;

    for (edge = cone->INCONE; edge != NULL; edge = edge->NEXT){
        if ((edge->TYPE & (CNS_LOOP|CNS_MEMSYM)) == (CNS_LOOP|CNS_MEMSYM)){
            if (edge->UEDGE.CONE == input){
                if (MLO_IS_TRANSP(activelink->ULINK.LOTRS->TYPE)){
                    for (branch = cone->BRVDD; branch != NULL; branch = branch->NEXT){
                        if ((branch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED){
                            return delay*0.5;
                        }
                    }
                }
                if (MLO_IS_TRANSN(activelink->ULINK.LOTRS->TYPE)){
                    for (branch = cone->BRVSS; branch != NULL; branch = branch->NEXT){
                        if ((branch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED){
                            return delay*0.5;
                        }
                    }
                }
            }
        }
    }
    return delay;
}

/*-----------------------------------------------------------------------------
*                    val_non_dual()                                           *
*                                                                             *
* fonction qui calcule  les  4 temps pour les cones non dual cmos             *
* les mets dans la structure delay_list                                       *
-----------------------------------------------------------------------------*/
delay_list *val_non_dual(cone, input, delay, fcarac, ccarac)
cone_list *cone, *input;
delay_list *delay;
long fcarac;
double ccarac;
{
  branch_list *branch;
  link_list *activelink;
  double Cconfup = 0.0, Cconfdown = 0.0, srcil = 0.0, srl = 0.0, srcih =
      0.0, srh = 0.0, pconf0 = 0.0, pconf1 = 0.0, pqy = 0.0, gate_th =
      0.0, vtp, vtp0;
  double fin;
  double fin0;
  timing_scm *scm;
  float vt, vth, vend, vdd;
  double cgpc, cgdc, cgpoc, cgdce[3];
  float rin, cin, vsatin ;
  branch_list *brconf ;

  tas_getslewparamsduo(cone, &srcil, &srl, &srcih, &srh);

  if (((cone->
        TYPE & (CNS_LATCH | CNS_FLIP_FLOP | CNS_MEMSYM | CNS_MASTER |
                CNS_SLAVE)))
      || (getptype(cone->USER, CNS_BLEEDER) != NULL)) {

    branch = (branch_list *) cone->BRVDD;
    Cconfup = calcul_FB(cone, branch, 'U');
    branch = (branch_list *) cone->BRVSS;
    Cconfdown = calcul_FB(cone, branch, 'D');
  }

  branch = existe_tpd(cone, input, CNS_VSS, CNS_TN, &activelink);
  if ((!branch) || tas_disable_gate_delay(input->NAME, cone, 1, 0) || tas_is_degraded_memsym(branch, cone, 'D'))
    delay->TPHL = TAS_NOTIME;
  else {
    fin0 = tas_getfin(activelink, cone, fcarac, NULL, &rin, &cin, &vsatin);
    if (fin0 == TAS_NOFRONT_DBL) {
      delay->TPHL = TAS_NOTIME;
    } else {
      tas_getconflictparam(cone, input, CNS_VDD, CNS_TP,
                           branch->LINK, activelink, &pconf0,
                           &pconf1, &pqy, &gate_th, &vtp, &vtp0, &cgpc,
                           &cgdc, &cgpoc, cgdce, &brconf, 'X');
      /*pconf0 += Cconfup; */
      if ((branch->TYPE & CNS_VSS_DEGRADED) != CNS_VSS_DEGRADED) {
        if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
          delay->TMHL =
              stm_getmodel(CELL,
                           stm_mod_name(input->NAME, 'U',
                                        cone->NAME, 'D',
                                        STM_DELAY | STM_SLEW,
                                        TAS_PATH_TYPE, 0));
        } else {
          scm =
              tpd_dualmodel(cone, branch, activelink,
                            pconf0, pconf1, brconf, pqy,
                            tas_delayrcchemin(branch, activelink), fin0/TTV_UNIT,
                            tas_getcapai(cone), Cconfup, vtp, vtp0,
                            cgpc, cgdc, cgpoc, cgdce, TAS_UNKNOWN_EVENT, gate_th);
          if (scm) {
            delay->TMHL = stm_mod_create(NULL);
            delay->FMHL = NULL;
            tas_getparamsforslope(cone, activelink, 'D', &vt,
                                  &vth, &vend, &vdd);
            stm_mod_update(delay->TMHL, vth, vdd, vt, vend);
            stm_mod_update_transition(delay->TMHL, STM_HL);
            delay->TMHL->UMODEL.SCM = scm;
            delay->TMHL->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMHL, scm);
          } else {
            delay->TMHL = NULL;
            delay->FMHL = NULL;
          }
        }
        if (delay->TMHL) {
          if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE ) 
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINR,
                          STM_DEFAULT_SMAXR,
                          stm_mod_vt( delay->TMHL ),
                          tas_get_vf_input(activelink),
                          tas_get_vf_input(activelink), STM_UP);
          else
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINR,
                          STM_DEFAULT_SMAXR,
                          tas_getparam(activelink->ULINK.LOTRS,TAS_CASE, TP_VT),
                          tas_get_vf_input(activelink),
                          tas_get_vf_input(activelink), STM_UP);

          tas_compute_delays(delay->TMHL, cone, fin, rin, cin, vsatin, NULL, NULL,
                             &delay->TPHL, &delay->FHL,
                             &delay->CARAC, branch->LINK,
                             input->NAME, SIM_RISE, SIM_FALL);
          if ((cone->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
              delay->TPHL = tas_memsym_loop_adjust(delay->TPHL, cone, input, activelink);
#ifdef USEOLDTEMP
          delay->SHL =
              (long) (stm_modscm_slope
                      (delay->TMHL->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
        } else {
          delay->TPHL = TAS_NOTIME;
        }
      } else {
        if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
          delay->TMHL =
              stm_getmodel(CELL,
                           stm_mod_name(input->NAME, 'U',
                                        cone->NAME, 'D',
                                        STM_DELAY | STM_SLEW,
                                        TAS_PATH_TYPE, 0));
        } else {
          if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE ) 
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINR,
                          STM_DEFAULT_SMAXR,
                          stm_mod_vt( delay->TMHL ),
                          tas_get_vf_input(activelink),
                          tas_get_vf_input(activelink), STM_UP);
          else
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINR,
                          STM_DEFAULT_SMAXR,
                          tas_getparam(activelink->ULINK.LOTRS,TAS_CASE, TP_VT),
                          tas_get_vf_input(activelink),
                          tas_get_vf_input(activelink), STM_UP);

          scm =
              tpd_trans_goodmodel(cone, branch, activelink,
                                  pconf0 + pqy, pconf1, srcil,
                                  srl, srcih, srh,
                                  tas_delayrcchemin(branch, activelink), fin,
                                  tas_getcapai(cone), Cconfup, gate_th);
          if (scm) {
            delay->TMHL = stm_mod_create(NULL);
            delay->FMHL = NULL;
            tas_getparamsforslope(cone, activelink, 'D', &vt,
                                  &vth, &vend, &vdd);
            stm_mod_update(delay->TMHL, vth, vdd, vt, vend);
            stm_mod_update_transition(delay->TMHL, STM_HL);
            delay->TMHL->UMODEL.SCM = scm;
            delay->TMHL->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMHL, scm);
          } else {
            delay->TMHL = NULL;
            delay->FMHL = NULL;
            if (!getptype
                (activelink->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)) {
              activelink->USER =
                  addptype(activelink->USER,
                           TAS_LINK_UNUSED_SWITCH_COMMAND, (void *) 1);
            }
          }
        }
        if (delay->TMHL) {
          tas_compute_delays(delay->TMHL, cone, fin, rin, cin, vsatin, NULL, NULL,
                             &delay->TPHL, &delay->FHL,
                             &delay->CARAC, branch->LINK,
                             input->NAME, SIM_RISE, SIM_FALL);
#ifdef USEOLDTEMP
          delay->SHL =
              (long) (stm_modscm_slope
                      (delay->TMHL->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
        } else {
          delay->TPHL = TAS_NOTIME;
        }
      }
      if (delay->TMHL) {
        if (stm_noise_getmodeltype(delay->TMHL) == STM_NOISE_NULL)
          stm_noise_create(delay->TMHL, STM_NOISE_SCR);
        stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                   (delay->TMHL), (float) gate_th);
      }
    }
  }
  affiche_time(cone, input, "TPHL", delay->TPHL);
  affiche_time(cone, input, "FHL", delay->FHL);

  branch = existe_tpd(cone, input, CNS_VDD, CNS_TP, &activelink);
  if ((!branch) || tas_disable_gate_delay(input->NAME, cone, 0, 1) || tas_is_degraded_memsym(branch, cone, 'U'))
    delay->TPLH = TAS_NOTIME;
  else {
    fin0 = tas_getfin(activelink, cone, fcarac, NULL, &rin, &cin, &vsatin);
    if (fin0 == TAS_NOFRONT_DBL) {
      delay->TPLH = TAS_NOTIME;
    } else {
      tas_getconflictparam(cone, input, CNS_VSS, CNS_TN,
                           branch->LINK, activelink, &pconf0,
                           &pconf1, &pqy, &gate_th, &vtp, &vtp0, &cgpc,
                           &cgdc, &cgpoc, cgdce, &brconf, 'X');
      /*pconf0 += Cconfdown; */
      if ((branch->TYPE & CNS_VDD_DEGRADED) != CNS_VDD_DEGRADED) {
        if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
          delay->TMLH =
              stm_getmodel(CELL,
                           stm_mod_name(input->NAME, 'D',
                                        cone->NAME, 'U',
                                        STM_DELAY | STM_SLEW,
                                        TAS_PATH_TYPE, 0));
        } else {
          scm =
              tpd_dualmodel(cone, branch, activelink,
                            pconf0, pconf1, brconf, pqy,
                            tas_delayrcchemin(branch, activelink), fin0/TTV_UNIT,
                            tas_getcapai(cone), Cconfdown, vtp, vtp0,
                            cgpc, cgdc, cgpoc, cgdce, TAS_UNKNOWN_EVENT, gate_th);
          if (scm) {
            delay->TMLH = stm_mod_create(NULL);
            delay->FMLH = NULL;
            tas_getparamsforslope(cone, activelink, 'U', &vt,
                                  &vth, &vend, &vdd);
            stm_mod_update(delay->TMLH, vth, vdd, vt, vend);
            stm_mod_update_transition(delay->TMLH, STM_LH);
            delay->TMLH->UMODEL.SCM = scm;
            delay->TMLH->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMLH, scm);
          } else {
            delay->TMLH = NULL;
            delay->FMLH = NULL;
          }
        }
        if (delay->TMLH) {
          if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE )
          fin = stm_thr2scm( fin0 / TTV_UNIT, 
                             STM_DEFAULT_SMINF,
                             STM_DEFAULT_SMAXF,
                             stm_mod_vt( delay->TMLH ),
                             0.0,
                             tas_get_vf_input(activelink), 
                             STM_DN
                           );
          else
          fin = stm_thr2scm( fin0 / TTV_UNIT, 
                             STM_DEFAULT_SMINF,
                             STM_DEFAULT_SMAXF,
                             tas_getparam(activelink->ULINK.LOTRS,TAS_CASE, TP_VT),
                             0.0,
                             tas_get_vf_input(activelink), 
                             STM_DN
                           );
          tas_compute_delays(delay->TMLH, cone, fin, rin, cin, vsatin, NULL, NULL,
                             &delay->TPLH, &delay->FLH,
                             &delay->CARAC, branch->LINK,
                             input->NAME, SIM_FALL, SIM_RISE);
          if ((cone->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
              delay->TPLH = tas_memsym_loop_adjust(delay->TPLH, cone, input, activelink);
#ifdef USEOLDTEMP
          delay->SLH =
              (long) (stm_modscm_slope
                      (delay->TMLH->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
        } else {
          delay->TPLH = TAS_NOTIME;
        }
      } else {
        if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
          delay->TMLH =
              stm_getmodel(CELL,
                           stm_mod_name(input->NAME, 'D',
                                        cone->NAME, 'U',
                                        STM_DELAY | STM_SLEW,
                                        TAS_PATH_TYPE, 0));
        } else {
          if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE ) 
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINF,
                          STM_DEFAULT_SMAXF,
                          stm_mod_vt( delay->TMLH ), 0.0,
                          tas_get_vf_input(activelink), STM_DN);
          else
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINF,
                          STM_DEFAULT_SMAXF,
                          tas_getparam(activelink->ULINK.LOTRS,TAS_CASE, TP_VT), 0.0,
                          tas_get_vf_input(activelink), STM_DN);
          scm =
              tpd_trans_goodmodel(cone, branch, activelink,
                                  pconf0 + pqy, pconf1, srcil,
                                  srl, srcih, srh,
                                  tas_delayrcchemin(branch, activelink), fin,
                                  tas_getcapai(cone), Cconfdown, gate_th);
          if (scm) {
            delay->TMLH = stm_mod_create(NULL);
            delay->FMLH = NULL;
            tas_getparamsforslope(cone, activelink, 'U', &vt,
                                  &vth, &vend, &vdd);
            stm_mod_update(delay->TMLH, vth, vdd, vt, vend);
            stm_mod_update_transition(delay->TMLH, STM_LH);
            delay->TMLH->UMODEL.SCM = scm;
            delay->TMLH->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMLH, scm);
          } else {
            delay->TMLH = NULL;
            delay->FMLH = NULL;
            if (!getptype
                (activelink->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)) {
              activelink->USER =
                  addptype(activelink->USER,
                           TAS_LINK_UNUSED_SWITCH_COMMAND, (void *) 1);
            }
          }
        }
        if (delay->TMLH) {
          tas_compute_delays(delay->TMLH, cone, fin, rin, cin, vsatin, NULL, NULL,
                             &delay->TPLH, &delay->FLH,
                             &delay->CARAC, branch->LINK,
                             input->NAME, SIM_FALL, SIM_RISE);
#ifdef USEOLDTEMP
          delay->SLH =
              (long) (stm_modscm_slope
                      (delay->TMLH->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
        } else {
          delay->TPLH = TAS_NOTIME;
        }
      }
      if (delay->TMLH) {
        if (stm_noise_getmodeltype(delay->TMLH) == STM_NOISE_NULL)
          stm_noise_create(delay->TMLH, STM_NOISE_SCR);
        stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                   (delay->TMLH), (float) gate_th);
      }
    }
  }
  affiche_time(cone, input, "TPLH", delay->TPLH);
  affiche_time(cone, input, "FLH", delay->FLH);

  branch = existe_tpd(cone, input, CNS_VDD, CNS_TN, &activelink);
  if ((!branch) || tas_disable_gate_delay(input->NAME, cone, 1, 1) || tas_is_degraded_memsym(branch, cone, 'U'))
    delay->TPHH = TAS_NOTIME;
  else {
    fin0 = tas_getfin(activelink, cone, fcarac, NULL, &rin, &cin, &vsatin);
    if (fin0 == TAS_NOFRONT_DBL) {
      delay->TPHH = TAS_NOTIME;
    } else {
      if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
        delay->TMHH =
            stm_getmodel(CELL,
                         stm_mod_name(input->NAME, 'U', cone->NAME,
                                      'U', STM_DELAY | STM_SLEW,
                                      TAS_PATH_TYPE, 0));
      } else {
        tas_getconflictparam(cone, input, CNS_VSS, CNS_TP,
                             branch->LINK, activelink, &pconf0,
                             &pconf1, &pqy, &gate_th, &vtp, NULL,
                             NULL, NULL, NULL, NULL, NULL, 'X');
        /*pconf0 += Cconfdown; */
        if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE ) 
        fin =
            stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINR,
                        STM_DEFAULT_SMAXR,
                        stm_mod_vt( delay->TMHH ),
                        tas_get_vf_input(activelink),
                        tas_get_vf_input(activelink), STM_UP);
        else
        fin =
            stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINR,
                        STM_DEFAULT_SMAXR,
                        tas_getparam(activelink->ULINK.LOTRS,TAS_CASE, TP_VT),
                        tas_get_vf_input(activelink),
                        tas_get_vf_input(activelink), STM_UP);
        scm =
            tpd_trans_falsemodel(cone, branch->LINK, activelink,
                                 pconf0 + pqy, pconf1, srcil, srl,
                                 srcih, srh,
                                 tas_delayrcchemin(branch, activelink), fin,
                                 tas_getcapai(cone), Cconfdown);
        if (scm) {
          delay->TMHH = stm_mod_create(NULL);
          delay->FMHH = NULL;
          tas_getparamsforslope(cone, activelink, 'U', &vt, &vth,
                                &vend, &vdd);
          stm_mod_update(delay->TMHH, vth, vdd, vt, vend);
          stm_mod_update_transition(delay->TMHH, STM_HH);
          delay->TMHH->UMODEL.SCM = scm;
          delay->TMHH->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMHH, scm);
        } else {
          delay->TMHH = NULL;
          delay->FMHH = NULL;
          if (!getptype(activelink->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)) {
            activelink->USER =
                addptype(activelink->USER,
                         TAS_LINK_UNUSED_SWITCH_COMMAND, (void *) 1);
          }
        }
      }
      if (delay->TMHH) {
        tas_compute_delays(delay->TMHH, cone, fin, rin, cin, vsatin, NULL, NULL,
                           &delay->TPHH, &delay->FHH,
                           &delay->CARAC, branch->LINK,
                           input->NAME, SIM_RISE, SIM_RISE);
#ifdef USEOLDTEMP
        delay->SHH =
            (long) (stm_modscm_slope
                    (delay->TMHH->UMODEL.SCM, fin,
                     tas_getcapai(cone)) + 0.5);
#endif
      } else {
        delay->TPHH = TAS_NOTIME;
      }
      if (delay->TMHH) {
        if (stm_noise_getmodeltype(delay->TMHH) == STM_NOISE_NULL)
          stm_noise_create(delay->TMHH, STM_NOISE_SCR);
        stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                   (delay->TMHH), (float) gate_th);
      }
    }
  }
  affiche_time(cone, input, "TPHH", delay->TPHH);
  affiche_time(cone, input, "FHH", delay->FHH);

  branch = existe_tpd(cone, input, CNS_VSS, CNS_TP, &activelink);
  if ((!branch) || tas_disable_gate_delay(input->NAME, cone, 0, 0) || tas_is_degraded_memsym(branch, cone, 'D'))
    delay->TPLL = TAS_NOTIME;
  else {
    fin0 = tas_getfin(activelink, cone, fcarac, NULL, &rin, &cin, &vsatin);
    if (fin0 == TAS_NOFRONT_DBL) {
      delay->TPLL = TAS_NOTIME;
    } else {
      tas_getconflictparam(cone, input, CNS_VDD, CNS_TN,
                           branch->LINK, activelink, &pconf0,
                           &pconf1, &pqy, &gate_th, &vtp, &vtp0, &cgpc,
                           &cgdc, &cgpoc, cgdce, NULL, 'X');
      /*pconf0 += Cconfup; */
      if ((branch->TYPE & CNS_VSS_DEGRADED) != CNS_VSS_DEGRADED) {
        if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
          delay->TMLL =
              stm_getmodel(CELL,
                           stm_mod_name(input->NAME, 'D',
                                        cone->NAME, 'D',
                                        STM_DELAY | STM_SLEW,
                                        TAS_PATH_TYPE, 0));
        } else {
          scm =
              tpd_dualmodel(cone, branch, activelink,
                            pconf0, pconf1, NULL, pqy,
                            tas_delayrcchemin(branch, activelink), fin0/TTV_UNIT,
                            tas_getcapai(cone), Cconfup, vtp, vtp0,
                            cgpc, cgdc, cgpoc, cgdce, TAS_UNKNOWN_EVENT, gate_th);
          if (scm) {
            delay->TMLL = stm_mod_create(NULL);
            delay->FMLL = NULL;
            tas_getparamsforslope(cone, activelink, 'D', &vt,
                                  &vth, &vend, &vdd);
            stm_mod_update(delay->TMLL, vth, vdd, vt, vend);
            stm_mod_update_transition(delay->TMLL, STM_LL);
            delay->TMLL->UMODEL.SCM = scm;
            delay->TMLL->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMLL, scm);
          } else {
            delay->TMLL = NULL;
            delay->FMLL = NULL;
          }
        }
        if (delay->TMLL) {
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINF,
                          STM_DEFAULT_SMAXF,
                          stm_mod_vt( delay->TMLL ),
                          0.0,
                          tas_get_vf_input(activelink), STM_DN);
          tas_compute_delays(delay->TMLL, cone, fin, rin, cin, vsatin, NULL, NULL,
                             &delay->TPLL, &delay->FLL,
                             &delay->CARAC, branch->LINK,
                             input->NAME, SIM_FALL, SIM_FALL);
#ifdef USEOLDTEMP
          delay->SLL =
              (long) (stm_modscm_slope
                      (delay->TMLL->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
        } else {
          delay->TPLL = TAS_NOTIME;
        }
      } else {
        if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
          delay->TMLL =
              stm_getmodel(CELL,
                           stm_mod_name(input->NAME, 'D',
                                        cone->NAME, 'D',
                                        STM_DELAY | STM_SLEW,
                                        TAS_PATH_TYPE, 0));
        } else {
          if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE )
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINF,
                          STM_DEFAULT_SMAXF,
                          stm_mod_vt( delay->TMLL ), 0.0,
                          tas_get_vf_input(activelink), STM_DN);
          else
          fin =
              stm_thr2scm(fin0 / TTV_UNIT, STM_DEFAULT_SMINF,
                          STM_DEFAULT_SMAXF,
                          tas_getparam(activelink->ULINK.LOTRS,
                                       TAS_CASE, TP_VT), 0.0,
                          tas_get_vf_input(activelink), STM_DN);
          scm =
              tpd_trans_falsemodel(cone, branch->LINK,
                                   activelink, pconf0 + pqy,
                                   pconf1, srcil, srl, srcih,
                                   srh,
                                   tas_delayrcchemin(branch, activelink),
                                   fin, tas_getcapai(cone), Cconfup);
          if (scm) {
            delay->TMLL = stm_mod_create(NULL);
            delay->FMLL = NULL;
            tas_getparamsforslope(cone, activelink, 'D', &vt,
                                  &vth, &vend, &vdd);
            stm_mod_update(delay->TMLL, vth, vdd, vt, vend);
            stm_mod_update_transition(delay->TMLL, STM_LL);
            delay->TMLL->UMODEL.SCM = scm;
            delay->TMLL->UTYPE = STM_MOD_MODSCM;
            tas_add_power_model(delay->TMLL, scm);
          } else {
            delay->TMLL = NULL;
            delay->FMLL = NULL;
            if (!getptype
                (activelink->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)) {
              activelink->USER =
                  addptype(activelink->USER,
                           TAS_LINK_UNUSED_SWITCH_COMMAND, (void *) 1);
            }
          }
        }
        if (delay->TMLL) {
          tas_compute_delays(delay->TMLL, cone, fin, rin, cin, vsatin, NULL, NULL,
                             &delay->TPLL, &delay->FLL,
                             &delay->CARAC, branch->LINK,
                             input->NAME, SIM_FALL, SIM_FALL);
#ifdef USEOLDTEMP
          delay->SLL =
              (long) (stm_modscm_slope
                      (delay->TMLL->UMODEL.SCM, fin,
                       tas_getcapai(cone)) + 0.5);
#endif
        } else {
          delay->TPLL = TAS_NOTIME;
        }
      }
      if (delay->TMLL) {
        if (stm_noise_getmodeltype(delay->TMLL) == STM_NOISE_NULL)
          stm_noise_create(delay->TMLL, STM_NOISE_SCR);
        stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                   (delay->TMLL), (float) gate_th);
      }
    }
  }
  affiche_time(cone, input, "TPLL", delay->TPLL);
  affiche_time(cone, input, "FLL", delay->FLL);

  ccarac = 0.0;
  return (delay);
}

/*-----------------------------------------------------------------------------
*                      tas_getfinup()                                         *
*                                                                             *
* renvoie le front montant pour la chaine de pass transistors dans val_con    *
-----------------------------------------------------------------------------*/
double tas_getfinup(cone_list * cone, locon_list * locon, float fcarac)
{
  front_list *front1;
  edge_list *incone;
  ptype_list *ptype = NULL;
  double finup;

  if (TAS_CONTEXT->TAS_CALCRCX == 'Y') {
    for (incone = cone->INCONE; incone != NULL; incone = incone->NEXT) {
      if ((incone->TYPE & CNS_EXT) == CNS_EXT) {
        if (incone->UEDGE.LOCON == locon)
          break;
      }
    }
    if (incone != NULL) {
      if (TAS_PATH_TYPE == 'm')
        ptype = getptype(incone->USER, TAS_SLOPE_MIN);
      else
        ptype = getptype(incone->USER, TAS_SLOPE_MAX);
    }
  }
  if (ptype == NULL) {
    if ((ptype = getptype(locon->USER, CNS_EXT)) != NULL) {
      if (TAS_PATH_TYPE == 'm') {
        ptype = getptype(((cone_list *) ptype->DATA)->USER, TAS_SLOPE_MIN);
      } else {
        ptype = getptype(((cone_list *) ptype->DATA)->USER, TAS_SLOPE_MAX);
      }
    } else {
      if (TAS_PATH_TYPE == 'm') {
        ptype = getptype(locon->USER, TAS_SLOPE_MIN);
      } else {
        ptype = getptype(locon->USER, TAS_SLOPE_MAX);
      }
    }
  }
  front1 = (front_list *) ptype->DATA;
  if (fcarac)
    finup = fcarac;
  else
    finup = front1->FUP;

  if (finup == TAS_NOFRONT)
    return TAS_NOFRONT_DBL;
  else
    return (double) finup;
}

/*-----------------------------------------------------------------------------
*                      tas_getfindown()                                       *
*                                                                             *
* renvoie le front descendant pour la chaine de pass transistors dans val_con *
-----------------------------------------------------------------------------*/
double tas_getfindown(cone_list * cone, locon_list * locon, float fcarac)
{
  front_list *front1;
  edge_list *incone;
  ptype_list *ptype = NULL;
  double findown;

  if (TAS_CONTEXT->TAS_CALCRCX == 'Y') {
    for (incone = cone->INCONE; incone != NULL; incone = incone->NEXT) {
      if ((incone->TYPE & CNS_EXT) == CNS_EXT) {
        if (incone->UEDGE.LOCON == locon)
          break;
      }
    }
    if (incone != NULL) {
      if (TAS_PATH_TYPE == 'm')
        ptype = getptype(incone->USER, TAS_SLOPE_MIN);
      else
        ptype = getptype(incone->USER, TAS_SLOPE_MAX);
    }
  }
  if (ptype == NULL) {
    if ((ptype = getptype(locon->USER, CNS_EXT)) != NULL) {
      if (TAS_PATH_TYPE == 'm') {
        ptype = getptype(((cone_list *) ptype->DATA)->USER, TAS_SLOPE_MIN);
      } else {
        ptype = getptype(((cone_list *) ptype->DATA)->USER, TAS_SLOPE_MAX);
      }
    } else {
      if (TAS_PATH_TYPE == 'm') {
        ptype = getptype(locon->USER, TAS_SLOPE_MIN);
      } else {
        ptype = getptype(locon->USER, TAS_SLOPE_MAX);
      }
    }
  }
  front1 = (front_list *) ptype->DATA;
  if (fcarac)
    findown = fcarac;
  else
    findown = front1->FDOWN;

  if (findown == TAS_NOFRONT)
    return TAS_NOFRONT_DBL;
  else
    return (double) findown;
}


/*-----------------------------------------------------------------------------
*                      val_con()                                              *
-----------------------------------------------------------------------------*/
delay_list *val_con(cone, locon, delay, fcarac, ccarac)
cone_list *cone;
locon_list *locon;
delay_list *delay;
long fcarac;
double ccarac;
{
  branch_list *path;
  link_list *auxlink = NULL, *link;
  double Cconfup = 0.0, Cconfdown = 0.0, srcil = 0.0, srl = 0.0, srcih =
      0.0, srh = 0.0;
  double finup, findown;
  timing_scm *scm;
  float vt, vth, vend, vdd;

  tas_getslewparamsduo(cone, &srcil, &srl, &srcih, &srh);

  if (((cone->
        TYPE & (CNS_LATCH | CNS_FLIP_FLOP | CNS_MEMSYM | CNS_MASTER |
                CNS_SLAVE)) != 0)
      || (getptype(cone->USER, CNS_BLEEDER) != NULL)) {
    Cconfup = calcul_FB(cone, cone->BRVDD, 'U');
    Cconfdown = calcul_FB(cone, cone->BRVSS, 'D');
  }

  CCUSED = 0;

  if ((path = tas_GiveActivExtPath(cone, locon, 'U')) != NULL) {
    for (link = (link_list *) path->LINK; link->NEXT != NULL;
         link = link->NEXT)
      auxlink = link;

    finup = tas_getfinup(cone, locon, fcarac);
    if ((auxlink != NULL) && (finup != TAS_NOFRONT_DBL) && !tas_disable_gate_delay(locon->NAME, cone, 1, 1) && !tas_is_degraded_memsym(path, cone, 'U')) {
      auxlink->NEXT = NULL;
      if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
        delay->TMHH =
            stm_getmodel(CELL,
                         stm_mod_name(locon->NAME, 'U', cone->NAME,
                                      'U', STM_DELAY | STM_SLEW,
                                      TAS_PATH_TYPE, 0));
      } else {
        if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE )
        finup =
            stm_thr2scm(finup / TTV_UNIT, STM_DEFAULT_SMINR,
                        STM_DEFAULT_SMAXR,
                        stm_mod_vt( delay->TMHH ),
                        tas_get_vdd_path(path->LINK),
                        tas_get_vdd_path(path->LINK), STM_UP);
        else
        finup =
            stm_thr2scm(finup / TTV_UNIT, STM_DEFAULT_SMINR,
                        STM_DEFAULT_SMAXR,
                        tas_getparam(path->LINK->ULINK.LOTRS,TAS_CASE, TP_VT),
                        tas_get_vdd_path(path->LINK),
                        tas_get_vdd_path(path->LINK), STM_UP);
        scm =
            tpd_pathmodel(cone, path->LINK, 'U', Cconfdown, srcil, srl,
                          srcih, srh, finup, tas_getcapai(cone));
        if (scm) {
          delay->TMHH = stm_mod_create(NULL);
          delay->FMHH = NULL;
          tas_getparamsforslope(cone, path->LINK, 'U', &vt, &vth,
                                &vend, &vdd);
          stm_mod_update(delay->TMHH, vth, vdd, vt, vend);
          stm_mod_update_transition(delay->TMHH, STM_HH);
          delay->TMHH->UMODEL.SCM = scm;
          delay->TMHH->UTYPE = STM_MOD_MODSCM;
          tas_add_power_model(delay->TMHH, scm);
          if (stm_noise_getmodeltype(delay->TMHH) == STM_NOISE_NULL)
            stm_noise_create(delay->TMHH, STM_NOISE_SCR);
          stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                     (delay->TMHH),
                                     (vend - vdd / 2.0) / 2.0);
        } else {
          delay->TMHH = NULL;
          delay->FMHH = NULL;
          if (!getptype(path->LINK->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)) {
            path->LINK->USER =
                addptype(path->LINK->USER,
                         TAS_LINK_UNUSED_SWITCH_COMMAND, (void *) 1);
          }
          TAS_PVT_COUNT++;
          tas_error(71, cone->NAME, TAS_WARNING);
        }
      }
      if (delay->TMHH) {
        tas_compute_delays(delay->TMHH, cone, finup, -1.0, -1.0, -1.0, NULL, NULL,
                           &delay->TPHH, &delay->FHH,
                           &delay->CARAC, path->LINK, locon->NAME,
                           SIM_RISE, SIM_RISE);
#ifdef USEOLDTEMP
        delay->SHH =
            (long) (stm_modscm_slope
                    (delay->TMHH->UMODEL.SCM, finup,
                     tas_getcapai(cone)) + 0.5);
#endif
      } else {
        delay->TPHH = TAS_NOTIME;
      }
      auxlink->NEXT = link;
    }
  }
  auxlink = NULL;
  CCUSED = 0;

  if ((path = tas_GiveActivExtPath(cone, locon, 'D')) != NULL) {
    for (link = (link_list *) path->LINK; link->NEXT != NULL;
         link = link->NEXT)
      auxlink = link;

    findown = tas_getfindown(cone, locon, fcarac);
    if ((auxlink != NULL) && (findown != TAS_NOFRONT_DBL) && !tas_disable_gate_delay(locon->NAME, cone, 0, 0) && !tas_is_degraded_memsym(path, cone, 'D')) {
      auxlink->NEXT = NULL;
      if (TAS_CONTEXT->TAS_VALID_SCM == 'Y') {
        delay->TMLL =
            stm_getmodel(CELL,
                         stm_mod_name(locon->NAME, 'D', cone->NAME,
                                      'D', STM_DELAY | STM_SLEW,
                                      TAS_PATH_TYPE, 0));
      } else {
      if( V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL ].VALUE )
        findown =
            stm_thr2scm(findown / TTV_UNIT, STM_DEFAULT_SMINF,
                        STM_DEFAULT_SMAXF,
                        stm_mod_vt( delay->TMLL ), 0.0,
                        tas_get_vdd_path(path->LINK), STM_DN);
        else
        findown =
            stm_thr2scm(findown / TTV_UNIT, STM_DEFAULT_SMINF,
                        STM_DEFAULT_SMAXF,
                        tas_getparam(path->LINK->ULINK.LOTRS,TAS_CASE, TP_VT), 0.0,
                        tas_get_vdd_path(path->LINK), STM_DN);
        scm =
            tpd_pathmodel(cone, path->LINK, 'D', Cconfup, srcil, srl,
                          srcih, srh, findown, tas_getcapai(cone));
        if (scm) {
          delay->TMLL = stm_mod_create(NULL);
          delay->FMLL = NULL;
          tas_getparamsforslope(cone, path->LINK, 'D', &vt, &vth,
                                &vend, &vdd);
          stm_mod_update(delay->TMLL, vth, vdd, vt, vend);
          stm_mod_update_transition(delay->TMLL, STM_LL);
          delay->TMLL->UMODEL.SCM = scm;
          delay->TMLL->UTYPE = STM_MOD_MODSCM;
          tas_add_power_model(delay->TMLL, scm);
          if (stm_noise_getmodeltype(delay->TMLL) == STM_NOISE_NULL)
            stm_noise_create(delay->TMLL, STM_NOISE_SCR);
          stm_noise_scr_update_invth(stm_noise_getmodel_scr
                                     (delay->TMLL),
                                     ((vdd - vend) - vdd / 2.0) / 2.0);
        } else {
          delay->TMLL = NULL;
          delay->FMLL = NULL;
          if (!getptype(path->LINK->USER, TAS_LINK_UNUSED_SWITCH_COMMAND)) {
            path->LINK->USER =
                addptype(path->LINK->USER,
                         TAS_LINK_UNUSED_SWITCH_COMMAND, (void *) 1);
          }
          TAS_PVT_COUNT++;
          tas_error(71, cone->NAME, TAS_WARNING);
        }
      }
      if (delay->TMLL) {
        tas_compute_delays(delay->TMLL, cone, findown, -1.0, -1.0, -1.0, NULL, NULL,
                           &delay->TPLL, &delay->FLL,
                           &delay->CARAC, path->LINK, locon->NAME,
                           SIM_FALL, SIM_FALL);
#ifdef USEOLDTEMP
        delay->SLL =
            (long) (stm_modscm_slope
                    (delay->TMLL->UMODEL.SCM, findown,
                     tas_getcapai(cone)) + 0.5);
#endif
      } else {
        delay->TPLL = TAS_NOTIME;
      }
      auxlink->NEXT = link;
    }
  }
  ccarac = 0.0;
  return (delay);
}

/*-----------------------------------------------------------------------------
*                      tas_getparamsforslope()                                *
-----------------------------------------------------------------------------*/
void
tas_getparamsforslope(cone_list * cone, link_list * link, char type,
                      float *vt, float *vth, float *vend, float *vdd)
{

  float vdeg;
  ptype_list *ptype;

  *vt = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VT);
  *vth = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_SEUIL);
  *vdd = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  if (type == 'U') {
    if ((cone->TECTYPE & CNS_VDD_DEGRADED) != CNS_VDD_DEGRADED) {
      *vend = *vdd;
    } else {
      vdeg = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg);
      if((ptype = getptype(cone->USER, TAS_VDD_NOTDEG)) != NULL){
          vdeg -= *(float*)(&ptype->DATA);
      }
      if ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
        *vend = *vdd - vdeg;
      else
        *vend = vdeg;
    }
  } else if (type == 'D') {
    if ((cone->TECTYPE & CNS_VSS_DEGRADED) != CNS_VSS_DEGRADED) {
      *vend = 0.0;
    } else {
      vdeg = tas_getparam(link->ULINK.LOTRS, TAS_CASE, TP_deg);
      if ((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
        *vend = vdeg;
      else
        *vend = *vdd - vdeg;
    }
  }
}

void
tas_getparamsforinputslope(cone_list * cone, char type, float *vt,
                           float *vth, float *vend, float *vdd)
{
  link_list *link = NULL;

  if (type == 'U') {
    if (cone->BRVDD)
      link = cone->BRVDD->LINK;
  }

  if (type == 'D') {
    if (cone->BRVSS)
      link = cone->BRVSS->LINK;
  }

  if (link) {
    tas_getparamsforslope(cone, link, type, vt, vth, vend, vdd);
  } else {
    *vdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    if (type == 'U') {
      *vt = STM_DEFAULT_VTN;
      *vend = *vdd;
    } else {
      *vt = (*vdd) - STM_DEFAULT_VTP;
      *vend = 0.0;
    }
    *vth = V_FLOAT_TAB[__SIM_VTH].VALUE * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
  }
}

/******************************************************************/
void
tas_curve(timing_model * tmodel, char *inputname, char eventin,
          char *outputname, char eventout, float fin, RCXFLOAT c1,
          RCXFLOAT r, RCXFLOAT c2)
{
  stm_curve *curve;
  char buf[1024];
  long slope = (long) (TAS_CONTEXT->FRONT_NOT_SHRINKED / TTV_UNIT + 0.5);
  long cout = (long) (TAS_CONTEXT->TAS_CAPAOUT * 1000.0 + 0.5);
  int evin;
  int evout;
  char *namefile = TAS_CONTEXT->TAS_FILENAME;
  float start = TAS_CONTEXT->TAS_CURVE_START;
  float end = TAS_CONTEXT->TAS_CURVE_END;

  if (eventin == 'U')
    evin = 1;
  else
    evin = 0;
  if (eventout == 'U')
    evout = 1;
  else
    evout = 0;

  sprintf(buf, "f%ld_%s%d%s%d_%s_c%ld_l%d", slope, inputname, evin,
          outputname, evout, namefile, cout, TAS_CONTEXT->TAS_LEVEL);

  if ((r < 0.0) || (c2 < 0.0))
    curve = stm_curve_c(tmodel, fin, c1, start, end);
  else
    curve = stm_curve_pi(tmodel, fin, c1, r, c2, start, end);
  stm_curve_plot(curve, buf);

}

/******************************************************************/
void
tas_compute_delays(timing_model * tmodel, cone_list * cone, float fin, float rin, float cin, float vsatin,
                   stm_pwl * pwl, stm_pwl ** ptpwl, long *delay,
                   long *slew, output_carac ** carac, link_list * link,
                   char *inputname, char eventin, char eventout)
{

  locon_list *loconcone;
  //chain_list *chaincone;
  //chain_list *chainc;
  rcx_list *ptrcx;
  RCXFLOAT r;
  RCXFLOAT c1;
  RCXFLOAT c2;
  RCXFLOAT load;
  char type = 0;
  losig_list *losig;
  double delaydbe, slewdbe, findbe, capaout;
  float delaytas, slewtas;
  rcx_slope slope;
  static char env_debug_pwl = 'y';
  static char *env_debug_pwl_name = NULL;
  char filename[1024];
  stm_driver driver;

  if (env_debug_pwl == 'y') {
    env_debug_pwl = 'n';
    if( V_STR_TAB[ __TAS_DEBUG_PWL ].VALUE )
      env_debug_pwl_name = namealloc(V_STR_TAB[ __TAS_DEBUG_PWL ].VALUE);
  }

  if ((losig = tas_getlosigcone(cone)) != NULL) {

    if ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
      if (link->ULINK.LOTRS->DRAIN->SIG == losig)
        loconcone = link->ULINK.LOTRS->DRAIN;
      else if (link->ULINK.LOTRS->SOURCE->SIG == losig)
        loconcone = link->ULINK.LOTRS->SOURCE;
      else
        loconcone = NULL;
    } else {
      loconcone = link->ULINK.LOCON;
    }
    if (loconcone) {
      ptrcx = getrcx(loconcone->SIG);
      if (ptrcx) {
        slope.F0MAX = 0.0;
        slope.FCCMAX = 0.0;
        slope.CCA = -1.0;
        switch (eventout) {
        case SIM_FALL:
          slope.SENS = TRC_SLOPE_DOWN;
          break;
        case SIM_RISE:
          slope.SENS = TRC_SLOPE_UP;
          break;
        default:
          slope.SENS = TRC_SLOPE_UNK;
          break;
        }
        type =
            rcx_rcnload(TAS_CONTEXT->TAS_LOFIG, losig,
                        loconcone->PNODE, &r, &c1, &c2,
                        RCX_BESTLOAD, tas_get_cone_output_capacitance(cone)
                        /*TAS_CONTEXT->TAS_CAPAOUT */
                        , &slope, 'M');
      }
    }
  }

  if (TAS_CONTEXT->TAS_SIMU_CONE == 'Y') {
    findbe = (double) fin / 1.0e12;
    capaout = tas_get_cone_output_capacitance(cone) / 1.0e-12;  //TAS_CONTEXT->TAS_CAPAOUT / 1.0e12;
    sim_get_delay_slope_cone(TAS_CONTEXT->TAS_LOFIG,
                             TAS_CONTEXT->TAS_CNSFIG, cone, inputname,
                             eventin, findbe, eventout, capaout,
                             &delaydbe, &slewdbe,
                             TAS_PATH_TYPE == 'm' ? SIM_MIN : SIM_MAX);
    *delay = (long) (delaydbe * 1.0e12 + 0.5);
    *slew = (long) (slewdbe * 1.0e12 + 0.5);
  } else {


    driver.r = rin ;
    driver.c = cin*1000.0 ;
    driver.v = vsatin ;
    
    if (type == RCX_CAPALOAD) {
    
      stm_mod_timing(tmodel, tmodel, fin, pwl, &driver, c1 * 1000.0, &delaytas, &slewtas, ptpwl, cone->NAME, inputname, eventin == 'U'?'R':'F', eventout == 'U'?'R':'F');

      if (env_debug_pwl_name == cone->NAME) {
        if (SIMUINV_PREFIX==NULL)
          sprintf(filename, "pwl_%c%c_%s_%s", eventin, eventout, inputname, cone->NAME);
        else
          sprintf(filename, "%s.%c%c.stm", SIMUINV_PREFIX, tolower(eventin), tolower(eventout));
        stm_model_plot(filename, tmodel, fin, pwl, &driver, -1.0, c1 * 1000.0, -1.0,
                       eventin, eventout,
                       delaytas, slewtas,
                       NULL/*getenv("DEBUG_PWL_CONFIG")*/);
      }
     
      if (env_SIMUINV != ' ' && cone->NAME == namealloc(SIMUINVCONENAME) && TAS_PATH_TYPE == 'M' ) {
      
        if( !cone->BRVDD      || 
            !cone->BRVSS      ||
            cone->BRVDD->NEXT ||
            cone->BRVSS->NEXT ||
            cone->BRVDD->LINK->NEXT ||
            cone->BRVSS->LINK->NEXT    ) {
          printf( "inverter simulator : not an inverter gate !\n" );
        }
        else {
          if (SIMUINV_PREFIX==NULL)
            sprintf( filename, "simuinv_%c%c_%s_%s.dat", tolower(eventin), tolower(eventout), inputname, cone->NAME);
          else
            sprintf( filename, "%s.%s.%c%c.dat", SIMUINV_PREFIX, env_SIMUINV=='T'?"tas":"mcc", tolower(eventin), tolower(eventout) );
            
          tpiv_inverter( filename, 
                         cone->BRVSS->LINK->ULINK.LOTRS,
                         cone->BRVDD->LINK->ULINK.LOTRS, 
                         eventin,
                         fin,
                         -1.0,
                         c1*1000.0, 
                         -1.0,
                         &driver,
                         tpiv_inverter_config_tmax*1e12, 
                         tmodel->UMODEL.SCM->PARAMS.DUAL,
                         env_SIMUINV 
                       );
        }
      }
                    
      if (TAS_CONTEXT->TAS_CURVE == 'Y')
        tas_curve(tmodel, inputname, eventin, cone->NAME, eventout, fin, c1 * 1000, -1.0, -1.0);
        
    } else if (type == RCX_PILOAD) {
    
      stm_mod_timing_pi(tmodel, tmodel, fin, pwl, &driver, c1 * 1000.0,
                        c2 * 1000.0, r, &delaytas, &slewtas, ptpwl, cone->NAME, inputname, eventin == 'U'?'R':'F', eventout == 'U'?'R':'F');
                        
      if (env_debug_pwl_name == cone->NAME) {
        if (SIMUINV_PREFIX==NULL)
          sprintf(filename, "pwl_%c%c_%s_%s", eventin, eventout, inputname, cone->NAME);
        else
          sprintf(filename, "%s.%c%c.stm", SIMUINV_PREFIX, tolower(eventin), tolower(eventout));

        if( V_BOOL_TAB[ __AVT_PRECISE_PILOAD ].VALUE )
          stm_model_plot(filename, tmodel, fin, pwl, &driver, r, c1*1000.0, c2*1000.0, eventin, eventout,
                         delaytas, slewtas, NULL);
        else {
          if (tmodel->UTYPE == STM_MOD_MODSCM)
            load =
                stm_modscm_capaeq(tmodel->UMODEL.SCM, fin, r,
                                  c1 * 1000.0, c2 * 1000.0, cone->NAME);
          else
            load = (c1 + c2) * 1000.0;
          stm_model_plot(filename, tmodel, fin, pwl, &driver, -1.0, load, -1.0, eventin,
                         delaytas, slewtas,
                         eventout, NULL);
        }
      }
      
      if (env_SIMUINV != ' ' && cone->NAME == namealloc(SIMUINVCONENAME) && TAS_PATH_TYPE == 'M' ) {
      
        if( !cone->BRVDD      || 
            !cone->BRVSS      ||
            cone->BRVDD->NEXT ||
            cone->BRVSS->NEXT ||
            cone->BRVDD->LINK->NEXT ||
            cone->BRVSS->LINK->NEXT    ) {
          printf( "inverter simulator : not an inverter gate !\n" );
        }
        else {
          if (SIMUINV_PREFIX==NULL)
            sprintf( filename, "simuinv_%c%c_%s_%s.dat", tolower(eventin), tolower(eventout), inputname, cone->NAME);
          else
            sprintf( filename, "%s.%s.%c%c.dat", SIMUINV_PREFIX, env_SIMUINV=='T'?"tas":"mcc", tolower(eventin), tolower(eventout) );
            
          tpiv_inverter( filename, 
                         cone->BRVSS->LINK->ULINK.LOTRS,
                         cone->BRVDD->LINK->ULINK.LOTRS, 
                         eventin,
                         fin,
                         r,
                         c1*1000.0,
                         c2*1000.0,
                         &driver,
                         tpiv_inverter_config_tmax*1e12, 
                         tmodel->UMODEL.SCM->PARAMS.DUAL,
                         env_SIMUINV 
                       );
        }
      }
      
      if (TAS_CONTEXT->TAS_CURVE == 'Y')
        tas_curve(tmodel, inputname, eventin, cone->NAME, eventout,
                  fin, c1 * 1000, r, c2 * 1000);
    } else {
      stm_mod_timing(tmodel, tmodel, fin, pwl, &driver, tas_getcapai(cone), &delaytas, &slewtas, ptpwl, cone->NAME, inputname, eventin == 'U'?'R':'F', eventout == 'U'?'R':'F');
      
      if (TAS_CONTEXT->TAS_CURVE == 'Y')
        tas_curve(tmodel, inputname, eventin, cone->NAME, eventout, fin, tas_getcapai(cone), -1.0, -1.0);
    }
    if (type)
      tas_set_output_carac(carac, tmodel, eventout, type, r, c1, c2, slewtas);
    *delay = (long) (delaytas * TTV_UNIT + 0.5);
    if (stm_if_thresholds())
      *slew =
          (long) (stm_mod_shrinkslew_scm2thr(tmodel, slewtas) *
                  TTV_UNIT + 0.5);
    else
      *slew = (long) (slewtas * TTV_UNIT + 0.5);

  }
}

link_list *tas_get_last_active_link(link_list * active)
{
  link_list *link;
  link_list *last;

  last = active;
  for (link = active->NEXT; link; link = link->NEXT) {
    if (link->ULINK.LOTRS->GRID->SIG == active->ULINK.LOTRS->GRID->SIG)
      last = link;
  }

  return last;
}

void tas_getinverterforswitch( cone_list *cone, link_list *activelink, lotrs_list *trspair, link_list **retlink, long *retdelay )
{
  ptype_list *ptype ;
  cone_list  *drvactive ;
  cone_list  *drvpair ;
  char        position ;
  edge_list  *edge ;
  chain_list *pairlinklist ;
  link_list  *pairlink ;
  delay_list *delay ;
  long        delta ;
  
  if( retlink )
    *retlink = NULL ;
  if( retdelay )
    *retdelay = TAS_NOTIME ;

  /*
    Détermine la position du maillon actif par rapport au maillon dégradé.
    maillon = 'a' (after)  : le maillon actif commute après le maillon dégradé
    maillon = 'b' (before) : le maillon actif commute avant le maillon dégradé
    ( après ou avant c'est par rapport à l'inverseur entre les deux maillons : 
    ça ne considère pas un éventuel temps de propagation négatif pour
    l'inverseur ).
  */
  
  ptype = getptype( activelink->ULINK.LOTRS->USER,  CNS_DRIVINGCONE );
  if( !ptype )
    return ;
  drvactive = (cone_list*)( ptype->DATA );

  ptype = getptype( trspair->USER,  CNS_DRIVINGCONE );
  if( !ptype )
    return ;
  drvpair   = (cone_list*)( ptype->DATA );

  position = 'x' ;

  for( edge = drvactive->INCONE ; edge ; edge = edge->NEXT ) {
  
    if( ( edge->TYPE & CNS_EXT ) == CNS_EXT )
      continue ;

    if( edge->UEDGE.CONE == drvpair ) {

      position = 'a' ;
      break ;
    }
  }
  
  if( position == 'x' ) {
    for( edge = drvpair->INCONE ; edge ; edge = edge->NEXT ) {
    
      if( ( edge->TYPE & CNS_EXT ) == CNS_EXT )
        continue ;

      if( edge->UEDGE.CONE == drvactive ) {

        position = 'b' ;
        break ;
      }
    }
  }

  if( position == 'x' )
    return ;

  pairlinklist = tas_find_corresp_link_from_lotrs( cone, trspair, MLO_IS_TRANSN( trspair->TYPE ) ? CNS_VSS : CNS_VDD );
  if( !pairlinklist )
    return ;
  pairlink     = (link_list*)(pairlinklist->DATA);
  freechain( pairlinklist );

  /* delta mesure le retard entre le maillon actif et le maillon dégradé.
     delta > 0 : le maillon dégradé commute après le maillon actif 
     delta < 0 : le maillon dégradé commuté avant le maillon actif
  */

  ptype = getptype( edge->USER, TAS_DELAY_MAX );
  if( !ptype )
    return ;
  delay = (delay_list*)( ptype->DATA );
  
  delta = TAS_NOTIME ;

  if( MLO_IS_TRANSP( activelink->ULINK.LOTRS->TYPE ) && position == 'a' )
    delta = - delay->TPHL ;

  if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) && position == 'b' )
    delta = delay->TPHL ;
 
  if( MLO_IS_TRANSP( activelink->ULINK.LOTRS->TYPE ) && position == 'b' )
    delta = delay->TPHL ;
    
  if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) && position == 'a' )
    delta = - delay->TPLH ;

  if( delta != TAS_NOTIME ) {
    if( retlink )
      *retlink = pairlink ;
    if( retdelay )
      *retdelay = delta ;
  }
}

double tas_getcdeltaswitch_delta( double vdd, double fact, double vta, double fdeg, double vtd, double delta, double vth )
{
  double t0 ;
  double tea ;
  double ted ;

  tea = fact * atanh( (vth-vta)/(vdd-vta) ) ; 
  ted = fdeg * atanh( (vth-vtd)/(vdd-vtd) ) ;
  t0  = tea - ted - delta ;

  return t0 ;
}

link_list* tas_getcdeltaswitch_branch( link_list *bractive, link_list *brconf, link_list *activelink, link_list **linkout )
{  
  link_list *branch ;
  link_list *link ;
  link_list *brvdd ;
  link_list *brvss ;

  if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) ) {
    brvss = bractive ;
    brvdd = brconf ;
  }
  else {
    brvdd = bractive ;
    brvss = brconf ;
  }

  branch = NULL ;
  for( link = brvss ; link ; link = link->NEXT ) {
    branch = addlink( branch, link->TYPE, link->ULINK.PTR, link->SIG );
    branch->USER = link->USER ;
  }
  branch = (link_list*)reverse( (chain_list*)branch );

  if( linkout )
    *linkout = branch ;
  
  for( link = brvdd ; link ; link = link->NEXT ) {
    branch = addlink( branch, link->TYPE, link->ULINK.PTR, link->SIG );
    branch->USER = link->USER ;
  }

  return branch ;
}

double tas_getcdeltaswitch_static( link_list *branch, 
                                   link_list *linkout,
                                   link_list *activelink, 
                                   link_list *pairlink,
                                   double vgactive, 
                                   double vgpair,
                                   double vdd,
                                   float *imax
                                 )
{
  double     vout ;

  /* polarisation de la branche */
  tpiv_set_voltage_driving_lotrs( activelink->ULINK.LOTRS, vgactive );
  tpiv_set_voltage_driving_lotrs( pairlink->ULINK.LOTRS,   vgpair   );

  /* calcule le courant de la branche et récupère la polarisation de la sortie */
  tpiv_i_brdual( branch, vdd, 0.0, imax, YES );
  vout = tas_get_vpol( linkout );
  
  /* nettoyage de la branche */
  tpiv_clean_voltage_driving_lotrs( activelink->ULINK.LOTRS );
  tpiv_clean_voltage_driving_lotrs( pairlink->ULINK.LOTRS );
  tas_clean_vpol(branch);

  return vout ;
}

void tas_getcdeltaswitch_conf( link_list *branch, 
                               link_list *linkout,
                               link_list *activelink, 
                               link_list *pairlink, 
                               double     deltaref,
                               double     fact,
                               double     fdeg,
                               double     vta,
                               double     vtd,
                               double     vdd,
                               int        debug,
                               double     deltadebug,
                               double    *fullconf,
                               double    *conf
                             )
{
  double tmin ; 
  double tmax ; 
  double tstep ; 
  double t ; 
  double vga ;
  double vgd ;
  double vout ;
  double ts ;
  double lt ;
  double ltp ;
  double lvout ;
  double q ;
  double qfull ;
  float  imax ;
  float  ibr ;
  float  libr ;
  float  librp ;
  int    blocked ;
  int    jobdone ;
  char   filename[256] ;
  FILE  *ptf ;
  int    iter = 1000 ;

  *fullconf = 0.0 ;
  *conf     = 0.0 ;

  ptf = NULL ;
  if( debug ) {
    sprintf( filename, "switch_conf_%s", activelink->ULINK.LOTRS->TRNAME );
    ptf = mbkfopen( filename, NULL, "w" );
  }

  if( deltaref < 0.0 ) {
    tmin = deltaref ;
    tmax = fact*atanh( (0.5*vdd-vta)/(vdd-vta) );
  }
  else {
    tmin = 0.0 ;
    tmax = deltaref + fdeg*atanh( (0.5*vdd-vtd)/(vdd-vtd) );
  }
 
  t = tmin ;
  tstep = (tmax-tmin)/10.0 ;
  blocked = 1 ;
  
  if( !tpiv_i_brdual( branch, vdd, 0.0, &imax, NO ) ) {
    avt_log( LOGTAS, 1, "tas_getcdeltaswitch_conf() : tpiv_i_brdual() failled #1\n" );
    return ;
  }

  lt = t ;
  if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) ) 
    lvout = vdd ;
  else
    lvout = 0.0 ;

  qfull = 0.0 ;
  q = 0.0 ;
 
  do {
  
    if( t<0 )
      vga = 0.0 ;
    else 
      vga = vta+(vdd-vta)*tanh(t/fact);
    
    if( t-deltaref < 0.0 )
      vgd = 0.0 ;
    else
      vgd = vtd+(vdd-vtd)*tanh((t-deltaref)/fdeg);

    if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) ) {
      vga = vga ;
      vgd = vdd-vgd ;
    }
    else {
      vga = vdd-vga ;
      vgd = vgd ;
    }

    tpiv_set_voltage_driving_lotrs( activelink->ULINK.LOTRS, vga );
    tpiv_set_voltage_driving_lotrs( pairlink->ULINK.LOTRS,   vgd );

    if( !tpiv_i_brdual( branch, vdd, 0.0, &ibr, YES ) ) {
      avt_log( LOGTAS, 1, "tas_getcdeltaswitch_conf() : tpiv_i_brdual() failled #2\n" );
      tas_clean_vpol( branch );
      return ;
    }
    
    if( blocked && fabs(ibr) > fabs(imax/1000.0) )
      blocked = 0 ;

    if( blocked ) {
      if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) )
        vout = vdd ;
      else
        vout = 0.0 ;
    }
    else
      vout = tas_get_vpol( linkout );

    tas_clean_vpol( branch );

    jobdone = 0 ;
    if( !blocked ) {
      if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) ) {
        if( vout < vdd/2.0 )
          jobdone = 1 ;
      }
      else {
        if( vout > vdd/2.0 )
          jobdone = 1 ;
      }
    }

    if( !jobdone ) {

      if( ptf ) 
        fprintf( ptf, "%g %g %g %g %g\n", t*1e-12+deltadebug, vga, vgd, vout, ibr );

      if( t > lt ) {
      
        qfull = qfull + 0.5*(t-lt)*(ibr+libr) ;

        if( t > 0.0 ) {
          if( lt > 0.0 ) {
            ltp   = lt ;
            librp = libr ;
          }
          else {
            ltp   = 0.0 ;
            librp = libr + (ltp-lt)/(t-lt)*(ibr-libr) ;
          }
          q = q + 0.5*(t-ltp)*(ibr+librp) ;
        }
      }
      
      lt = t ;
      lvout = vout ;
      t = t + tstep ;
      libr = ibr ;
    }

    iter-- ;
  }
  while ( !jobdone && iter );

  if( !iter )
    return ;

  ts = lt + (vdd/2.0-lvout)/(vout-lvout)*(t-lt) ;

  if( ts<0 )
    vga = 0.0 ;
  else
    vga = vta+(vdd-vta)*tanh(ts/fact);
  
  if( ts-deltaref < 0.0 )
    vgd = 0.0 ;
  else
    vgd = vtd+(vdd-vtd)*tanh((ts-deltaref)/fdeg);

  if( MLO_IS_TRANSN( activelink->ULINK.LOTRS->TYPE ) ) {
    vga = vga ;
    vgd = vdd-vgd ;
  }
  else {
    vga = vdd-vga ;
    vgd = vdd ;
  }

  tpiv_set_voltage_driving_lotrs( activelink->ULINK.LOTRS, vga );
  tpiv_set_voltage_driving_lotrs( pairlink->ULINK.LOTRS,   vgd );

  if( !tpiv_i_brdual( branch, vdd, 0.0, &ibr, NO ) ) {
    avt_log( LOGTAS, 1, "tas_getcdeltaswitch_conf() : tpiv_i_brdual() failled #3\n" );
    return ;
  }

  if( ptf ) 
    fprintf( ptf, "%g %g %g %g %g\n", ts*1e-12+deltadebug, vga, vgd, vdd/2.0, ibr );

  qfull = qfull + 0.5*(ts-lt)*(ibr+libr);
  if( ts > 0.0 ) {
    if( lt > 0.0 ) {
      ltp   = lt ;
      librp = libr ;
    }
    else {
      ltp   = 0.0 ;
      librp = libr + (ltp-lt)/(ts-lt)*(ibr-libr) ;
    }
    q = q + 0.5*(ts-ltp)*(ibr+librp) ;
  }

  tpiv_clean_voltage_driving_lotrs( activelink->ULINK.LOTRS );
  tpiv_clean_voltage_driving_lotrs( pairlink->ULINK.LOTRS );

  if( ptf )
    fclose( ptf );

  *fullconf = fabs(qfull/(vdd/2.0));
  *conf     = fabs(q/(vdd/2.0));
}

void tas_getcdeltaswitch( cone_list *cone, 
                          link_list *link0, 
                          link_list *activelink, 
                          link_list *linkconf0, 
                          double c0, 
                          float fin, 
                          char sensactif,
                          double *pconf0,
                          double *pconf1,
                          double *chalf,
                          double *qinit
                        )
{
  long        delta ;
  ptype_list *ptype ;
  lotrs_list *trspair ;
  link_list  *pairlink ;
  link_list  *headlink ;
  link_list  *link ;
  double vdd ;      double q ;       double ceq ;   double v1 ;
  double r ;        double an ;      double bn ;    double vg_sym ;
  double ax ;       double bx ;      double t1 ;    double vg_real ;
  double cx ;       double vta ;     double d ;     float  i_sym ;
  double vgs ;      double vgs1 ;    double vgs2 ;  float  i_real ;
  double vs ;       double vg ;      double t ;     float  i0_sym ;
  double fdeg ;     int    step ;    double nvg ;   float  i0_real ;
  double lt ;       int    i ;       double it ;    int    r_sym ;   
  double dt ;       double te ;      int    n ;     int    r_real ;
  double vs_sym ;   double vs_real ; double t0 ;    double fdelta ;
  double vtc ;      double vgdeg ;   double vgact ; double vsstatic ;
  double deltaref ; double c ;       double cconf ; double fullcconf ;
  static int debugenvdone = 0 ;
  static char *debuglinkname = NULL ;
  static float debugorigin = 0.0 ;
  float debugdelta = 0.0 ;
  char *env ;
  FILE *ptf ;
  char filename[256] ;
  float imax ;
  link_list *fullbranch ;
  link_list *linkout ;

  if( !debugenvdone ) {
    debugenvdone = 1 ;
    env = getenv( "DEBUGSWITCH_LINKNAME" );
    if( env )
      debuglinkname = namealloc( env );
    env = getenv( "DEBUGSWITCH_ORIGIN" );
    if( env )
      debugorigin = atof( env );
  }

  if( c0<=0.0 )
    return ;

  trspair = NULL ;
  if( ( activelink->TYPE & CNS_SWITCH ) == CNS_SWITCH ) {
    ptype = getptype( activelink->ULINK.LOTRS->USER, TAS_TRANS_SWITCH );
    if( ptype ) {
      trspair = (lotrs_list*)ptype->DATA ;
    }    
  }

  if( !trspair )
    return ;

  tas_getinverterforswitch( cone, activelink, trspair, &pairlink, &delta ) ;
  
  if( delta == TAS_NOTIME )
    return ;

  vdd  = tas_getparam ( activelink->ULINK.LOTRS, TAS_CASE, TP_VDDmax ) ;
  vta  = tas_getparam ( activelink->ULINK.LOTRS, TAS_CASE, TP_VT ) ;
  vtc  = tas_getparam ( pairlink->ULINK.LOTRS, TAS_CASE, TP_VT ) ;

  ptf = NULL ;
  if( debuglinkname == activelink->ULINK.LOTRS->TRNAME ) {
    sprintf( filename, "switch_%s.dat", debuglinkname );
    ptf = mbkfopen( filename, NULL, "w" );
    debugdelta = debugorigin - 1e-12*stm_modscm_dual_calte ( vdd-vta, vta, vdd/2.0, fin );
  }

  fdeg = tas_getfin( pairlink, cone, 0.0, NULL, NULL, NULL, NULL );
  fdeg = stm_thr2scm( fdeg / TTV_UNIT, STM_DEFAULT_SMINR,
                      STM_DEFAULT_SMAXR,
                      vtc,
                      tas_get_vf_input(activelink),
                      tas_get_vf_input(activelink), STM_UP
                    );

  deltaref = tas_getcdeltaswitch_delta( vdd, fin, vta, fdeg, vtc, -delta/TTV_UNIT, vdd/2.0 );

  if( linkconf0 ) {
    fullbranch = tas_getcdeltaswitch_branch( link0, linkconf0, activelink, &linkout );
    tas_getcdeltaswitch_conf( fullbranch, linkout, activelink, pairlink, deltaref, fin, fdeg, vta, vtc, vdd, ptf?1:0, debugdelta, &fullcconf, &cconf );
    *pconf1 = *pconf1 + 1000.0*cconf/fin ;
  }
  else {
    fullbranch = NULL ;
    cconf      = 0.0 ;
    fullcconf  = 0.0 ;
  }
  
  /* 
    cas où le maillon dégradé commute avant le maillon actif : il faut compter une charge
    initiale déchargée par le maillon dégradé.
  */

  vs   = vdd ;
  ceq  = 0.0 ;
  c = c0 / 1000.0 + fullcconf ;

  if( deltaref < 0 ) {

    /*
    Calcul du delta de charge : on prend un modèle extrèmement simple car :
    - on suppose que les fronts sur les 2 commandes sont  dans le modèle
      scm, sans prendre en compte qu'ici on calcule l'effet que ces 2 fronts ne sont
      pas identiques.
    - on a que les branches duales, pas les branches contenant le maillon
      dégradé
    - effet constant, indépendant du front d'entrée
    */
   
    /* recrée la branche */
    headlink = NULL ;
    for( link=link0 ; link != activelink ; link = link->NEXT ) {
      headlink = addlink( headlink, link->TYPE, link->ULINK.PTR, link->SIG );
      headlink->USER = link->USER ;
    }

    if( headlink ) {
      headlink = (link_list*)reverse( (chain_list*)headlink ) ;
      r = tas_get_rlin_br( headlink ) ;
    }
    else 
      r = 0.0 ;

    /* modèle RC pour calculer la charge perdue dans la branche pendant un temps delta */

    if( headlink )
      freelklist( headlink ) ;

    an  = tas_getparam ( pairlink->ULINK.LOTRS, TAS_CASE, TP_A ) ;
    bn  = tas_getparam ( pairlink->ULINK.LOTRS, TAS_CASE, TP_B ) ;

    te    = fdeg * atanh( (vdd/2.0-vtc)/(vdd-vtc));
    fdelta = -deltaref + te ;

    step = 10 ;
    nvg  = vdd-vtc ;
    lt    = 0.0 ; 

    for( i=step ; i>=0 ; i-- ) {

      vg = nvg ;

      if( r>0.0 ) {
        ax = an + bn/r ;
        bx = 2.0*an*vtc - 1.0/r + bn*vtc/r + bn*(vs-vg)/r ;
        cx = an * vtc * vtc - (vs-vg)/r + bn*vtc*(vs-vg)/r ;

        d = bx*bx-4*ax*cx ;
        if( d < 0.0 )
          vgs = vs-vg ;
        else {
        
          vgs1 = -(-bx+sqrt(d))/(2.0*ax);
          vgs2 = -(-bx-sqrt(d))/(2.0*ax);

          vgs = FLT_MAX ;
          if( vgs1 > 0.0 && vgs1 < vdd )
            vgs = vgs1 ;
          if( vgs2 > 0.0 && vgs2 < vdd )
            vgs = vgs2 ;
        }
      }
      else {
        vgs = vs-vg ;
      }

      if( vgs >= vtc && vgs <= vdd )
        it = an*(vgs-vtc)*(vgs-vtc)/(1.0+bn*(vgs-vtc));
      else
        it = 0.0 ; 

      nvg = (i-1)*(vdd-vtc)/step ;
      if( nvg > 0.01 )
        t   = -fdeg*atanh( (nvg-(vdd-vtc))/(vdd-vtc) ); 
      else
        t   = FLT_MAX ;

      if( t >= fdelta ) {
        t = fdelta ;
        i = -1 ;
      }

      dt  = t - lt ;

      vs  = vs - it*dt/c ;

     if( ptf )
        fprintf( ptf, "%g %g %g %g %g\n", (t-fdelta)*1e-12+debugdelta, 0.0, nvg, vs, i_real );
 
      if( vs < 0.0 )
        return ;
      lt  = t ;
    }

    /* Ici, on a pas compté le conflit. On vérifie qu'à l'instant où le maillon 
    non dégradé devient passant (son entrée atteind vt) la tension vs n'est
    pas inférieur à la tension statique */

    if( linkconf0 ) {
      vgdeg = vtc + (vdd-vtc)*tanh(-deltaref/fdeg);
      if( sensactif == 'U' ) {
        vgact = 0.0 ;
        vgdeg = vdd-vgdeg ;
      }
      else {
        vgact = vdd ;
        vgdeg = vgdeg ;
      }

      if( fullbranch ) {
        vsstatic = tas_getcdeltaswitch_static( fullbranch, linkout, activelink, pairlink, vgact, vgdeg, vdd, &imax );

        if( vsstatic > vs ) {
          vs = vsstatic ;
        }
      }
    }

    *qinit   = - (c0/1000.0+cconf)*(vdd-vs);

  }

  /*
  Calcule en faisant une simulation très simplifiée l'écart de charge entre une
  commutation symétrique des deux maillons et la commutation décalée.
  Pb : l'impacte de vs est très significatif sur les switchs car Rsat est très
  faible.
  La tension initiale de la sortie vaut vs.
  */
 
  n=10 ;

  if( ( activelink->ULINK.LOTRS->TYPE & CNS_TN ) == CNS_TN ) {
    vs_sym  = vs ;
    vs_real = vs ;
  }
  else {
    vs_sym  = vdd-vs ;
    vs_real = vdd-vs ;
  }

  t0 = -1.0 ;

  q = 0.0 ;

  fdelta = delta/TTV_UNIT ;

  if( ptf )
    fprintf( ptf, "%g %g %g %g %g\n", debugdelta, vta, nvg, vs, 0.0 );

  for( i=1 ; i<n ; i++ ) {

    v1 = vta + i*(vdd-vta)/n ;
    t1 = fin*atanh( (v1-vta)/(vdd-vta) );

    vg_sym  = v1 ;
    vg_real = tas_getvgsdeg( pairlink, fdelta, fdeg, t1 ) ;
    
    if( ( activelink->ULINK.LOTRS->TYPE & CNS_TN ) == CNS_TN ) {
      r_sym  = tpiv_get_i_multi_input( link0, activelink, vs_sym,  0.0, v1, vdd - vg_sym,  TAS_TRMODEL_MCCRSAT, &i_sym  );
      r_real = tpiv_get_i_multi_input( link0, activelink, vs_real, 0.0, v1, vdd - vg_real, TAS_TRMODEL_MCCRSAT, &i_real );
    }
    else {
      r_sym  = tpiv_get_i_multi_input( link0, activelink, vs_sym,  vdd, vdd-v1, vg_sym,  TAS_TRMODEL_MCCRSAT, &i_sym  );
      r_real = tpiv_get_i_multi_input( link0, activelink, vs_real, vdd, vdd-v1, vg_real, TAS_TRMODEL_MCCRSAT, &i_real );
    }

    if( !r_sym || !r_real ) {
      avt_log( LOGTAS, 1, "tpiv_get_i_multi_input() failled in tas_getcdeltaswitch()\n" );
      return ;
    }

    if( t0 < 0.0 ) {
      i0_sym  = 0.0 ;
      i0_real = i_real ;
      t0      = 0.0 ;
    }
    q = q + ( (i0_real+i_real) - (i0_sym+i_sym) ) * (t1-t0) / 2.0 ;

    if( t0 >=0.0 ) {
      vs_sym  = vs_sym  + i_sym * (t1-t0)/c ;
      vs_real = vs_real + i_real * (t1-t0)/c ;
    }

    if( ptf )
      fprintf( ptf, "%g %g %g %g %g\n", t1*1e-12+debugdelta, vg_sym, vdd - vg_real, vs_real, i_real );

    if( ( activelink->ULINK.LOTRS->TYPE & CNS_TN ) == CNS_TN ) {
      if( vs_real < vdd/2.0 )
        break ;
    }
    else {
      if( vs_real > vdd/2.0 )
        break ;
    }

    t0      = t1 ;
    i0_real = i_real ;
    i0_sym  = i_sym ;
  }

  if( ptf )
    fclose( ptf );

  if( delta > 0.0 )
    q = fabs(q);
  else
    q = -fabs(q);

  ceq = ceq + 1000.0 * q/(vdd/2.0) ;

  if( fullbranch )
    freelklist( fullbranch );

  *pconf0 = *pconf0 + ceq ;
  *chalf  = *chalf + ceq ;
}

/* Pour les switchs :
   Renvoie la valeur de la tension de grille du maillon dégradé à l'instant t
   du maillon actif (tjs vue comme une transition montante).
   Renvoie < 0.0 si problème.
*/

float tas_getvgsdeg( link_list *pairlink, float delta, float fdeg, float t )
{
  float      vgsdeg ;
  float      vt ;
  float      vdd ;

  if( t-delta <= 0 )
    return 0.0 ;

  vt  = tas_getparam( pairlink->ULINK.LOTRS, TAS_CASE, TP_VT );
  vdd = tas_getparam( pairlink->ULINK.LOTRS, TAS_CASE, TP_VDDmax );
  vgsdeg = vt + (vdd-vt)*tanh((t-delta)/fdeg);

  return vgsdeg ;
}

float tas_getvgsdeg_old( cone_list *cone, link_list *pairlink, float delta, float t )
{
  float      vgsdeg ;
  float      vt ;
  float      vdd ;
  float      fdeg ;

  if( t-delta <= 0 )
    return 0.0 ;

  /* approximation : fdeg = fin
     consequence : le temps qui sépare les vt des maillons actifs et 
     dégradés vaut delta.
  */
  fdeg = tas_getfin( pairlink, cone, 0.0, NULL, NULL, NULL, NULL );
  fdeg = stm_thr2scm( fdeg / TTV_UNIT, STM_DEFAULT_SMINR, STM_DEFAULT_SMAXR,
                      tas_getparam(pairlink->ULINK.LOTRS, TAS_CASE, TP_VT),
                      tas_get_vf_input(pairlink),
                      tas_get_vf_input(pairlink), 
                      STM_UP
                    );

  vt  = tas_getparam( pairlink->ULINK.LOTRS, TAS_CASE, TP_VT );
  vdd = tas_getparam( pairlink->ULINK.LOTRS, TAS_CASE, TP_VDDmax );
  vgsdeg = vt + (vdd-vt)*tanh((t-delta)/fdeg);

  return vgsdeg ;
}

/*
return the current for any kind of branch.
level is the coefficient applied to the output value (use 1.0 as default)
if savepol is set to YES, then save the voltage on internal nodes of
the branch.
*/
float tas_get_current( branch_list *branch, link_list *head, float level, char savepol )
{
  float valim ;
  float vout  ;
  float imax  ;
  float vdd   ;

  if( V_BOOL_TAB[ __TAS_USE_BSIM_CURRENT ].VALUE )
    TPIV_DEBUG_IBR=1;

  if( ( branch->TYPE & CNS_PARALLEL ) == CNS_PARALLEL )
    TPIV_DEBUG_IBR=0;

  vdd = tas_getparam(head->ULINK.LOTRS, TAS_CASE, TP_VDDmax);

  if( ( branch->TYPE & CNS_VDD ) == CNS_VDD ) {
    valim = vdd ;
    vout  = (1.0-level) * vdd ;
  } else if( ( branch->TYPE & CNS_VSS ) == CNS_VSS ){
    valim = 0.0 ;
    vout  = level * vdd ;
  } else {
    if (MLO_IS_TRANSP( head->ULINK.LOTRS->TYPE )){
      valim = vdd ;
      vout  = (1.0-level) * vdd ;    
    }else{
      valim = 0.0 ;
      vout  = level * vdd ;
    }
  }
  
  if (tpiv_i_brdual(head, vout, valim, &imax, savepol))
    imax = fabs(imax);
  else {
    imax = i_brmcc( head, head, level );
  }

  return imax ;
}

float tas_get_current_leakage( branch_list *branch, link_list *head, link_list *activelink)
{
  float valim ;
  float vout  ;
  float imax  ;
  float vdd   ;
  float delta=0.0 ;

  if( V_BOOL_TAB[ __TAS_USE_BSIM_CURRENT ].VALUE )
    TPIV_DEBUG_IBR=1;

  if( ( branch->TYPE & CNS_PARALLEL ) == CNS_PARALLEL )
    TPIV_DEBUG_IBR=0;

  vdd = tas_getparam(head->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  if (V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].SET && (TAS_PATH_TYPE == 'm')){
    delta = vdd * V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].VALUE;
  }
  if( ( branch->TYPE & CNS_VDD ) == CNS_VDD ) {
    valim = vdd ;
    vout  = 0.0 + delta;
  } else if( ( branch->TYPE & CNS_VSS ) == CNS_VSS ){
    valim = 0.0 ;
    vout  = vdd - delta;
  } else {
    if (MLO_IS_TRANSP( head->ULINK.LOTRS->TYPE )){
      valim = vdd ;
      vout  = 0.0 + delta;    
    }else{
      valim = 0.0 ;
      vout  = vdd - delta;
    }
  }
  
  if (tpiv_i_brdual_leakage(head, activelink, vout, valim, &imax))
    imax = fabs(imax);
  else {
    imax = 0.0;
  }

  return imax ;
}

float tas_get_current_leakage_2( branch_list *branch, link_list *head, link_list *activelink)
{
  float valim ;
  float vout  ;
  float imax  ;
  float vdd   ;
  float delta=0.0 ;

  if( V_BOOL_TAB[ __TAS_USE_BSIM_CURRENT ].VALUE )
    TPIV_DEBUG_IBR=1;

  if( ( branch->TYPE & CNS_PARALLEL ) == CNS_PARALLEL )
    TPIV_DEBUG_IBR=0;

  vdd = tas_getparam(head->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
  if (V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].SET && (TAS_PATH_TYPE == 'm')){
    delta = vdd * V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].VALUE;
  }
  if( ( branch->TYPE & CNS_VDD ) == CNS_VDD ) {
    valim = vdd ;
    vout  = 0.0 + delta;
  } else if( ( branch->TYPE & CNS_VSS ) == CNS_VSS ){
    valim = 0.0 ;
    vout  = vdd - delta;
  } else {
    if (MLO_IS_TRANSP( head->ULINK.LOTRS->TYPE )){
      valim = vdd ;
      vout  = 0.0 + delta;    
    }else{
      valim = 0.0 ;
      vout  = vdd - delta;
    }
  }
  
  if (tpiv_i_brdual_leakage_2(head, activelink, vout, valim, &imax))
    imax = fabs(imax);
  else {
    imax = 0.0;
  }

  return imax ;
}

float tas_getrintercone( cone_list *cone, cone_list *cone_avant )
{
  edge_list  *incone ;
  ptype_list *ptl ;
  ptype_list *ptl_rc_cone_avant ;
  ptype_list *ptl_rc_cone_edge ;
  chain_list *head_avant, *chain_avant ;
  chain_list *head_edge, *chain_edge ;
  chain_list *headpathrc, *chain ;
  locon_list *locon_avant ;
  locon_list *locon_edge ;
  losig_list *losig ;
  float       r;

  r = 0.0 ;
  if (TAS_CONTEXT->TAS_CALCRCX == 'Y') {
  
    incone = NULL;
    losig  = NULL ;

    ptl_rc_cone_avant = getptype(cone_avant->USER, TAS_RC_CONE) ;
    ptl = getptype( cone_avant->USER, CNS_SIGNAL );
    if( ptl )
      losig = (losig_list*)ptl->DATA ;
    
    if( ptl_rc_cone_avant && losig ) {
    
      for (incone = cone->INCONE; incone != NULL; incone = incone->NEXT) {
        if ((incone->TYPE & CNS_EXT) != CNS_EXT) {
          if (incone->UEDGE.CONE == cone_avant)
            break;
        }
      }
      
      if (incone != NULL) {
        ptl_rc_cone_edge = getptype(incone->USER, TAS_RC_LOCON) ;
        if( ptl_rc_cone_edge ) {
          head_avant = (chain_list*)ptl_rc_cone_avant->DATA ;
          head_edge  = (chain_list*)ptl_rc_cone_edge->DATA ;
          headpathrc = NULL ;

          /* Pour l'instant, récupère le premier délai RC qui vient. */
          for( chain_avant = head_avant ; chain_avant ; chain_avant = chain_avant->NEXT ) {
            locon_avant = (locon_list*)chain_avant->DATA ;
            if( locon_avant->PNODE ) {
              for( chain_edge = head_edge ; chain_edge ; chain_edge = chain_edge->NEXT ) {
                locon_edge  = (locon_list*)chain_edge->DATA ;
                if( locon_edge->PNODE ) {
                  headpathrc = getway( losig, locon_avant->PNODE->DATA, locon_edge->PNODE->DATA );
                  if( headpathrc )
                    break ;
                }
              }
              if( headpathrc )
                break ;
            }
          }
          if( headpathrc ) {
            
            for( chain = headpathrc ; chain ; chain = chain->NEXT )
              r = r + ((lowire_list*)chain->DATA)->RESI ;
            freechain( headpathrc );
          }
        }
      }
    }
  }

  return r ;
}
