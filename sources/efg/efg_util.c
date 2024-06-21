/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : efg_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-2002 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* Extraction d'une lofig concernant un chemin                              */
/****************************************************************************/
#include "efg.h"

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_getconename                                            */
/*                                                                           */
/*****************************************************************************/
char *efg_getconename ( losig_list *sig )
{
  char *conename;
  ptype_list *ptype;

  if ( (ptype = getptype (sig->USER,EFG_SIG2CONE)) ) 
    conename = ((cone_list*)ptype->DATA)->NAME;
  else
    conename = efg_getlosigname(sig);

  return conename;
}

cone_list *efg_getcone ( losig_list *sig )
{
  ptype_list *ptype;

  if ( (ptype = getptype (sig->USER,EFG_SIG2CONE)) ) 
    return (cone_list*)ptype->DATA;
  return NULL;
}

void efg_copy_lotrs_param(lotrs_list *source, lotrs_list *dest)
{
  ptype_list *pt;
  if ((pt = getptype(source->USER, OPT_PARAMS)) != NULL)
  {
    dest->USER=addptype(dest->USER, OPT_PARAMS, dupoptparamlst ((optparam_list *) pt->DATA));
  }
  if ((pt = getptype(source->USER, PARAM_CONTEXT)) != NULL)
  {
    dest->USER=addptype(dest->USER, PARAM_CONTEXT, eqt_dupvars((eqt_param *)pt->DATA));
  }
}

/*****************************************************************************\
FUNCTION : efg_cone_is_fonctionnal
\*****************************************************************************/
int efg_cone_is_fonctionnal (cone_list *cone, int depth)
{
  int res = 0;
  edge_list  *ptinedge;
  locon_list *locon;
  losig_list *losig;
  losig_list *losigcone;
  cone_list  *ptinputcone;
  ptype_list *ptype;

  ptype = getptype ( cone->USER, EFG_CONE_FCT );
  if ( !ptype ) {
    losigcone = efg_getlosigcone (cone);
    for (ptinedge = cone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
          if ((ptinedge->TYPE & CNS_VDD) != 0) continue;
          if ((ptinedge->TYPE & CNS_VSS) != 0) continue;
          if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
          if ((ptinedge->TYPE & CNS_EXT) != 0) {
            locon = ptinedge->UEDGE.LOCON;
            if (losigcone == locon->SIG) continue;
            if ( getptype (locon->SIG->USER,EFG_SIG_SET) ) {
              res = 1;
              break;
            }
            continue;
          }
          ptinputcone = ptinedge->UEDGE.CONE;
          losig = efg_getlosigcone (ptinputcone);
          if (losigcone == losig) continue; 
          if ( losig && getptype (losig->USER,EFG_SIG_SET ) ) {
            res = 1;
            break;
          }
          else if ( depth != V_INT_TAB[__EFG_MAX_DEPTH].VALUE )
            res = efg_cone_is_fonctionnal ( ptinputcone, ++depth );
    }
    cone->USER = addptype ( cone->USER, EFG_CONE_FCT, (void*)(long)res );
  }
  else
    res = (int)(long)ptype->DATA;
  return res;
}

void efg_remove_figure_alias( lofig_list *lofig ) 
{
  losig_list *losig ;
  lotrs_list *lotrs ;

  for ( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) 
    efg_del_corresp_alias( losig );
  for ( lotrs = lofig->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) 
    efg_del_corresp_alias_trs( lotrs );
}
/*****************************************************************************\
FUNCTION : efg_del_corresp_alias_trs
\*****************************************************************************/
void efg_del_corresp_alias_trs (lotrs_list *trs)
{
  if ( getptype (trs->USER,EFG_ALIAS_ORG) )
    trs->USER = delptype (trs->USER,EFG_ALIAS_ORG);
}

/*****************************************************************************\
FUNCTION : efg_get_origtrsname
\*****************************************************************************/
char *efg_get_origtrsname (lotrs_list *trs)
{
  ptype_list *ptype;

  ptype = getptype (trs->USER,EFG_ALIAS_ORG);
  if ( ptype )
    return (char*)ptype->DATA;
  else
    return NULL;
}

/*****************************************************************************\
FUNCTION : efg_del_corresp_alias
\*****************************************************************************/
void efg_del_corresp_alias (losig_list *sig)
{
  if ( getptype (sig->USER,EFG_ALIAS_ORG) )
    sig->USER = delptype (sig->USER,EFG_ALIAS_ORG);
}

/*****************************************************************************\
FUNCTION : efg_get_origsigname
\*****************************************************************************/
char *efg_get_origsigname (losig_list *sig)
{
  ptype_list *ptype;

  ptype = getptype (sig->USER,EFG_ALIAS_ORG);
  if ( ptype )
    return (char*)ptype->DATA;
  else
    return NULL;
}

/*****************************************************************************\
FUNCTION : efg_sig_rename
\*****************************************************************************/
char *efg_sig_rename (losig_list *sig)
{
  char buf[1024];
  char *alias;

  if (strcmp(alias=getsigname(sig),"0")!=0)
  {
    sprintf ( buf, "%s%ld", V_STR_TAB[__EFG_SIG_ALIAS].VALUE,sig->INDEX );
    alias = namealloc (buf);
  }

  return alias;
}

/*****************************************************************************\
FUNCTION : efg_is_conname_ext
\*****************************************************************************/
int efg_is_conname_ext (lofig_list *fig,losig_list *sig, long pnode)
{
  char *nodename;
  locon_list *loconext;
  num_list *num;
  int res = 0;

  if ( sig->TYPE == 'E' ) {
    nodename = efg_getlosigname (sig);
    if ( (loconext = efg_get_locon (fig,nodename))) {
      for ( num = loconext->PNODE ; num ; num=num->NEXT ) {
        if ( num->DATA == pnode )
          break;
      }
      if ( num )
        res = 1;
    }
  }
  return res;
}

/*****************************************************************************\
FUNCTION : efg_add_lotrs_capa

Rajoute une transistor ou capacite
\*****************************************************************************/
void efg_add_lotrs_capa (lofig_list *fig,locon_list *locon,int incr_index)
{
  chain_list *chain;
  ptype_list *ptype;
  losig_list *destsig;
  long node_vss=1,node;
  float capaval;
  int capatype = ELP_CAPA_TYPICAL;
  char *signame;
  spisig_list *spisig;
  loctc_list *ctc;

  if ( !fig || !locon || 
       efg_SigIsAlim (locon->SIG) == 'Y' ||
       (EFG_CALC_EQUI_CAPA == EFG_OUT_NONE))
    return;
  if (locon->TYPE == 'T') {
    if ( getptype (((lotrs_list*)locon->ROOT)->USER, EFG_CORRESP) )
      return;
    if (EFG_CALC_EQUI_CAPA == EFG_OUT_TRANSISTOR)
      efg_add_blockedlotrs (fig, locon, incr_index);
    else {
      // get event on sig
      signame = efg_getlosigname (locon->SIG);
      if ( EFGCONTEXT &&  EFGCONTEXT->SPISIG && 
          (spisig = efg_GetSpiSigByName (EFGCONTEXT->SPISIG, signame)) ) {
        if ( spisig->EVENT == (long)EFG_SIG_SET_FALL ) 
          capatype = ELP_CAPA_DN;
        else if ( spisig->EVENT == (long)EFG_SIG_SET_RISE ) 
          capatype = ELP_CAPA_UP;
        else
          capatype = ELP_CAPA_TYPICAL;
      }
      if ( !getptype (locon->USER, EFG_LOCON_CAPA) ) {
        //if (!getptype (locon->SIG->USER,EFG_SIG_SET)) return;
        if ( locon->PNODE ) 
          node = locon->PNODE->DATA;
        else
          node = 1;
        capaval = elpGetCapaFromLocon(locon,capatype,elpTYPICAL);
        destsig = efg_addlosig (fig,locon->SIG,incr_index);
        if (!destsig->PRCN) {
            addlorcnet (destsig);
            ctc=addloctc( destsig, node, EFGCONTEXT->VSSDESTFIG, node_vss, capaval);
        }
        else {
          if (destsig->PRCN->PCTC) {
             /* zinaps : to reactivate
            chainctc = destsig->PRCN->PCTC;
            for ( chain = chainctc ; chain ; chain=chain->NEXT ) {
              ctc = (loctc_list *)chain->DATA;
              if (ctc->SIG1==destsig && ctc->SIG2==EFGCONTEXT->VSSDESTFIG &&
                  ctc->NODE1==node && ctc->NODE2==node_vss) {
                ctc->CAPA += capaval;
                break;
              }
            }
            if ( !chain )*/
              ctc=addloctc( destsig, node, EFGCONTEXT->VSSDESTFIG, node_vss, capaval);
          }
          else
            ctc=addloctc( destsig, node, EFGCONTEXT->VSSDESTFIG, node_vss, capaval);
        }
        locon->USER = addptype ( locon->USER, EFG_LOCON_CAPA, ctc );
      }
    }
  }
  else if (locon->TYPE == 'I') {
    if ( (ptype = getptype (locon->SIG->USER,LOFIGCHAIN)) )
      for (chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT) {
        locon_list *con = (locon_list *)chain->DATA;
        
        if (con->TYPE == 'T')
          efg_add_blockedlotrs (fig,con,incr_index);
      }
  }
}

/*****************************************************************************\
FUNCTION : efg_set_loins_ctxt
\*****************************************************************************/
void efg_set_ins_ctxt (lofig_list *lofig, char *ctxt)
{
  loins_list *loins;
  char *newcontext;

  if (lofig) {
    for (loins = lofig->LOINS ; loins ; loins=loins->NEXT) {
      if (!getptype (loins->USER,EFG_INS_CTXT)) {
        newcontext = concatname(ctxt,loins->INSNAME); 
        loins->USER = addptype (loins->USER,
                                EFG_INS_CTXT,
                                newcontext
                                );
        efg_set_ins_ctxt (getloadedlofig (loins->FIGNAME),newcontext);
      }
    }
  }
}

/*****************************************************************************\
FUNCTION : efg_get_loins_ctxt
\*****************************************************************************/
char *efg_get_ins_ctxt (loins_list *ins) 
{
  ptype_list *ptype;
  char *ctxt = NULL;
  
  if (ins != NULL) {
    if ((ptype = getptype (ins->USER,EFG_INS_CTXT)))
      ctxt = (char*)ptype->DATA;
  }
  return ctxt;
}

/*****************************************************************************\
FUNCTION : efg_get_locon
\*****************************************************************************/
locon_list *efg_get_locon (lofig_list *lofig,char *loconname)
{
  locon_list *locon;

  if (!lofig || !loconname) return NULL;

  for (locon = lofig->LOCON ; locon ; locon=locon->NEXT) {
    if (locon->NAME == loconname)
      break;
  }
  return locon;
}

/******************************************************************************\
Fonction : efg_get_fig2ins

\******************************************************************************/
lofig_list *efg_get_fig2ins (loins_list *loins)
{
  lofig_list *fig = NULL;
  ptype_list *ptype;

  if (!loins)
    return NULL;
  ptype = getptype (loins->USER,EFG_INS_FIG);
  if ( ptype )
    fig = (lofig_list*)ptype->DATA;
  return fig;
}

/******************************************************************************\
Fonction : efg_set_fig2ins   

Attache une figure speciale (gns) a une instance

\******************************************************************************/
void efg_set_fig2ins (loins_list *loins,lofig_list *lofig)
{
  if (!loins || !lofig) return;

  if (!efg_get_fig2ins (loins))
    loins->USER = addptype (loins->USER,EFG_INS_FIG,lofig);
}

/******************************************************************************\
Fonction : efg_dont_copy_ins

\******************************************************************************/
void efg_dont_copy_ins (loins_list *loins)
{
  if (!getptype (loins->USER,EFG_INS_NO_COPY))
    loins->USER = addptype (loins->USER,
                            EFG_INS_NO_COPY,
                            NULL);
}

/******************************************************************************\
Fonction : efg_is_loins_to_copy     

\******************************************************************************/
int efg_is_loins_to_copy (loins_list *loins)
{
  int res = 1;

  if ((getptype (loins->USER,EFG_INS_NO_COPY)))
    res = 0;
  return res;
}

/******************************************************************************\
Fonction : efg_get_hier_loins   

renvoie l'instance par un nom hierarchique
\******************************************************************************/
loins_list *efg_get_hier_loins (lofig_list *currentfig,char *insname)
{
  char *left,*right;
  lofig_list *newlofig;
  loins_list *loins,*instance = NULL;

  if (!currentfig || !insname) 
      return NULL;

  leftunconcatname (insname,&left,&right);
  if (left == NULL) {
    for (loins = currentfig->LOINS ; loins ; loins=loins->NEXT) {
      if (loins->INSNAME == insname) {
        instance = loins;
        break;
      }
    }
  }
  else {
    newlofig = getloadedlofig (left);
    return efg_get_hier_loins (newlofig,right);
  }
  return instance;
}

/******************************************************************************\
Fonction : efg_get_fig_from_ctxt

renvoie la figure du contexte
\******************************************************************************/
lofig_list *efg_get_fig_from_ctxt (lofig_list *currentfig,char *context)
{
  char *left,*right;
  char *left2,*right2;
  lofig_list *figins = NULL,*newlofig;
  loins_list *loins;

  if (!currentfig) 
      currentfig = EFGCONTEXT->SRCFIG;
  if (context != NULL) {
      leftunconcatname (context,&left,&right);
      if (left == NULL)
          figins = currentfig;
      else {
          leftunconcatname (right,&left2,&right2);
          if (!left2)
          for (loins = currentfig->LOINS ; loins ; loins = loins->NEXT) {
              if (loins->INSNAME == right) {
                  newlofig = getloadedlofig (loins->FIGNAME);
                  figins = efg_get_fig_from_ctxt (newlofig,right);
              }
          }
          else
          for (loins = currentfig->LOINS ; loins ; loins = loins->NEXT) {
              if (loins->INSNAME == left2) {
                  newlofig = getloadedlofig (loins->FIGNAME);
                  figins = efg_get_fig_from_ctxt (newlofig,right);
              }
          }
      }
  }
  return figins;
}

/****************************************************************************\
Function efg_loins_is2analyse

renvoie 1 si oui
        0 sinon

\*****************************************************************************/
int efg_loins_is2analyse (char *context)
{
  lofig_list *figins;
  losig_list *losig;
  int         res = 0, sigdrv = 0;

  figins = efg_get_fig_from_ctxt (NULL,context);
  if (figins != NULL) {
      for (losig = figins->LOSIG ; losig ; losig=losig->NEXT) {
          if (efg_sig_is_onpath (losig,context))
              sigdrv++;
      }
  }
  if (sigdrv >= 2)
      res = 1;
  return res;
}

/****************************************************************************\
Function efg_sig_is_marked

renvoie 1 si oui
        0 sinon

\*****************************************************************************/
int efg_sig_is_marked (losig_list *sig,char *ctxt,long type)
{
  ptype_list *ptype;
  ht         *hash;
  long        newtype;
  long        resht;
  long        res = 0;

  if (!sig) return 0;
  if (ctxt != NULL) {
    ptype = getptype (sig->USER,type);
    if (ptype != NULL) {
        hash = (ht*)ptype->DATA;
        resht = gethtitem (hash,(void*)ctxt);
        if ((resht != EMPTYHT) && (resht != DELETEHT))
            res = 1;
    }
  }
  else {
    switch ( type ) {
      case EFG_SIG_SET_HIER : newtype = EFG_SIG_SET ;
                              break;
      case EFG_SIG_BEG_HIER : newtype = EFG_SIG_BEG ;
                              break;
      case EFG_SIG_END_HIER : newtype = EFG_SIG_END ;
                              break;
    }
    if (getptype (sig->USER,newtype) != NULL)
      res = 1;
  }
  return res;
}

/****************************************************************************\
Function efg_sig_is_beg   

renvoie 1 si oui
        0 sinon

\*****************************************************************************/
int efg_sig_is_beg (losig_list *sig,char *ctxt)
{
  return efg_sig_is_marked (sig,ctxt,EFG_SIG_BEG_HIER);
}

/****************************************************************************\
Function efg_sig_is_end   

renvoie 1 si oui
        0 sinon

\*****************************************************************************/
int efg_sig_is_end (losig_list *sig,char *ctxt)
{
  return efg_sig_is_marked (sig,ctxt,EFG_SIG_END_HIER);
}

/****************************************************************************\
Function efg_sig_is_onpath

renvoie 1 si oui
        0 sinon

\*****************************************************************************/
int efg_sig_is_onpath (losig_list *sig,char *ctxt)
{
    return efg_sig_is_marked (sig,ctxt,EFG_SIG_SET_HIER);
}

/*****************************************************************************
*                        fonction  efg_set_vdd_on_destfig()                  *
*    positionne le signal vss dans un ptype de la figure                     *
*****************************************************************************/
void efg_set_vdd_on_destfig (losig_list *vdd)
{
    if (!EFGCONTEXT || !vdd) return;
    EFGCONTEXT->VDDDESTFIG = vdd;
}

/*****************************************************************************
*                        fonction  efg_get_vdd_on_destfig()                  *
*    positionne le signal vss dans un ptype de la figure                     *
*****************************************************************************/
losig_list *efg_get_vdd_on_destfig ()
{
    if (!EFGCONTEXT) return NULL;
    return EFGCONTEXT->VDDDESTFIG;
}

/*****************************************************************************
*                        fonction  efg_set_vss_on_destfig()                  *
*    positionne le signal vss dans un ptype de la figure                     *
*****************************************************************************/
void efg_set_vss_on_destfig (losig_list *vss)
{
    if (!EFGCONTEXT || !vss) return;
    EFGCONTEXT->VSSDESTFIG = vss;
    if ( !getptype (EFGCONTEXT->DESTFIG->USER,EFG_SIG_VSS))
      EFGCONTEXT->DESTFIG->USER = addptype (EFGCONTEXT->DESTFIG->USER,
                                            EFG_SIG_VSS,
                                            vss);
}

/*****************************************************************************
*                        fonction  efg_get_vss_on_destfig()                  *
*    positionne le signal vss dans un ptype de la figure                     *
*****************************************************************************/
losig_list *efg_get_vss_on_destfig ()
{
    if (!EFGCONTEXT) return NULL;
    return EFGCONTEXT->VSSDESTFIG;
}

/*****************************************************************************/
/*                        function efg_revect()                              */
/* parametres :                                                              */
/* name : nom a vectoriser                                                   */
/*                                                                           */
/* revectorise le nom passe en parametre s'il contient des blancs ils seront */
/* remplaces par des '[]'.                                                   */
/* renvoie :                                                                 */
/* le pointeur sur le nom vectoriser                                         */
/*****************************************************************************/
char *efg_revect(name)
char *name ;
{
 char  namex[512] ;
 short i = strlen(name) ;
 short j = i - 1 ;
 char vect = 'N' ;

 strcpy(namex,name) ;

 do
   {
    while((--i != -1) && (namex[i] != ' '))
      if(namex[i] == '.')
        j = i-1 ;

    if(i != -1)
       {
        short k ;

        vect = 'Y' ;
        namex[i] = '[' ;

        for(k = strlen(namex) ; k != j ; k--)
        namex[k+1] = namex[k] ;

        namex[j+1] = ']' ;
        j = i - 1 ;
       }
    }
 while(i != -1) ;

 if (vect == 'N')
  return(name) ;
 else
  return(namealloc(namex)) ;
}

/*****************************************************************************
*                        fonction  efg_getlosigcone()                        *
*    renvoie le signal du cone s'il existe                                   *
*****************************************************************************/
losig_list *efg_getlosigcone(cone)
cone_list *cone ;
{

 if ( EFGCONTEXT )
   return mbk_quickly_getlosigbyname ( EFGCONTEXT->SRCFIG, cone->NAME);
 else
   return NULL;
}

/*****************************************************************************/
/* Add capa between net and vss                                              */
/*****************************************************************************/

void efg_setctcnet (losig_list *sig_dest,
                    losig_list *sig_src,
                    losig_list *sigvss,
                    double multcapa)
{
    chain_list *chainctc;
    loctc_list *ctc;
    losig_list *sig2bevss;
    losig_list *sig1,*sig2;
    long        node1,node2;
    
    if ((sig_src->PRCN != NULL) && (sig_src->PRCN->PCTC != NULL)) {
        for (chainctc = sig_src->PRCN->PCTC ; 
             chainctc ; chainctc = chainctc->NEXT) {
            ctc = (loctc_list*)chainctc->DATA;
            sig2bevss = rcn_ctcothersig(ctc,sig_src);
            if (sig2bevss == ctc->SIG1) {
                sig1 = sigvss;
                sig2 = sig_dest;
                node1 = 1;
                node2 = rcn_ctcnode(ctc,sig_src);
            }
            else {
                sig1 = sig_dest;
                sig2 = sigvss;
                node1 = rcn_ctcnode(ctc,sig_src);
                node2 = 1;
            }
            addloctc( sig1, node1, sig2, node2, ctc->CAPA*multcapa);
        }
    }
}

/*****************************************************************************/
/* efg_dup_hier_ptype                                                        */
/*****************************************************************************/

int efg_dup_hier_ptype (losig_list *newsig,losig_list *sig_src,char *ctxt)
{
    int res = 0;
    void *front;
    char *fullctxt;
    char *insname;

    fullctxt = concatname (EFGCONTEXT->SRCFIG->NAME,ctxt);
    rightunconcatname (fullctxt,&fullctxt,&insname);
    
    front = efg_get_hier_sigptype (sig_src,fullctxt,EFG_SIG_SET_HIER);
    if (front != NULL) {
      if (!getptype (newsig->USER,EFG_SIG_SET))
        newsig->USER = addptype (newsig->USER,EFG_SIG_SET,front);
      res = 1;
    }
    front = efg_get_hier_sigptype (sig_src,fullctxt,EFG_SIG_BEG_HIER);
    if (front != NULL) {
      if (!getptype (newsig->USER,EFG_SIG_BEG))
        newsig->USER = addptype (newsig->USER,EFG_SIG_BEG,front);
      res = 1;
    }
    else {
      front = efg_get_hier_sigptype (sig_src,fullctxt,EFG_SIG_END_HIER);
      if (front != NULL) {
        if (!getptype (newsig->USER,EFG_SIG_END))
          newsig->USER = addptype (newsig->USER,EFG_SIG_END,front);
        res = 1;
      }
    }
    return res;
}

/*****************************************************************************/
/* Duplique les eventuels ptypes concernant les info du .inf                 */
/*****************************************************************************/

int efg_dup_ptype (losig_list *newsig,losig_list *sig_src)
{
    int res = 0;
	ptype_list *ptype;

	if (!newsig || !sig_src) return 0;

	if ((ptype = getptype (sig_src->USER,EFG_SIG_SET)) != NULL) {
        res = 1;
        newsig->USER = addptype (newsig->USER,
                                 EFG_SIG_SET,
                                 ptype->DATA);
        if (getptype (sig_src->USER,EFG_SIG_BEG)) {
            newsig->USER = addptype (newsig->USER,
                                     EFG_SIG_BEG,
                                     ptype->DATA);
        }
        if (getptype (sig_src->USER,EFG_SIG_END)) {
            newsig->USER = addptype (newsig->USER,
                                     EFG_SIG_END,
                                     ptype->DATA);
        }
	}
    return res;
}

/*****************************************************************************/
/* Recupere le locon  equivalent dans la figure original : ext -> orig       */
/*****************************************************************************/

locon_list *efg_get_org_con (locon_list *con_ext)
{
	ptype_list *ptype;
	locon_list *con_org = NULL;
	
	if ((ptype = getptype (con_ext->USER,EFG_CORRESP)) != NULL)
		con_org = (locon_list*)ptype->DATA;
	return con_org;
}


/*****************************************************************************/
/* Recupere le lotrs  equivalent dans la figure original : ext -> orig       */
/*****************************************************************************/

lotrs_list *efg_get_org_trs (lotrs_list *trs_ext)
{
	ptype_list *ptype;
	lotrs_list *trs_org = NULL;
	
	if ((ptype = getptype (trs_ext->USER,EFG_CORRESP)) != NULL)
		trs_org = (lotrs_list*)ptype->DATA;
	return trs_org;
}

/*****************************************************************************/
/* Recupere le signal equivalent dans la figure original : ext -> orig       */
/*****************************************************************************/

losig_list *efg_get_org_sig (losig_list *sig_ext)
{
	ptype_list *ptype;
	losig_list *sig_org = NULL;
	
	if ((ptype = getptype (sig_ext->USER,EFG_CORRESP)) != NULL)
		sig_org = (losig_list*)ptype->DATA;
	return sig_org;
}
/*****************************************************************************/
/* Recupere le locon  equivalent dans la figure extraite                     */
/*****************************************************************************/

locon_list *efg_get_ext_con (locon_list *con_original)
{
	ptype_list *ptype;
	locon_list *conext = NULL;
	
	if ((ptype = getptype (con_original->USER,EFG_CORRESP)) != NULL)
		conext = (locon_list*)ptype->DATA;
	return conext;
}

/*****************************************************************************/
/* Recupere le signal equivalent dans la figure extraite                     */
/*****************************************************************************/

losig_list *efg_get_ext_sig (losig_list *sig_original)
{
	ptype_list *ptype;
	losig_list *sigext = NULL;
	
	if ((ptype = getptype (sig_original->USER,EFG_CORRESP)) != NULL)
		sigext = (losig_list*)ptype->DATA;
	return sigext;
}

/*****************************************************************************/
/* Recupere le lotrs  equivalent dans la figure extraite                     */
/*****************************************************************************/

lotrs_list *efg_get_ext_trs (lotrs_list *trs_original)
{
	ptype_list *ptype;
	lotrs_list *trsext = NULL;
	
	if ((ptype = getptype (trs_original->USER,EFG_CORRESP)) != NULL)
		trsext = (lotrs_list*)ptype->DATA;
	return trsext;
}

/*****************************************************************************/

losig_list *efg_try_to_add_alim_to_the_context(losig_list *sig, float value)
{
  long lval=mbk_long_round(value*10000);
  ptype_list *pt;
  char buf[64], *signame;
  for (pt=EFGCONTEXT->ALIMS; pt!=NULL && pt->TYPE!=lval; pt=pt->NEXT) ;
  if (pt!=NULL) return (losig_list *)pt->DATA;
  if (sig!=NULL)
    {
      EFGCONTEXT->ALIMS=addptype(EFGCONTEXT->ALIMS, lval, sig);
    }
  else
    {
      sprintf(buf,"efgalim%ld",lval);
      sig=addlosig (EFGCONTEXT->DESTFIG,getnumberoflosig (EFGCONTEXT->DESTFIG)+1, addchain(NULL,namealloc(buf)), INTERNAL);
      if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE ) 
      {
        signame = efg_sig_rename ( sig );
        freechain(sig->NAMECHAIN);
        sig->NAMECHAIN=addchain(NULL, signame);
      }
      addlosigalim(sig, value, NULL);
      EFGCONTEXT->ALIMS=addptype(EFGCONTEXT->ALIMS, lval, sig);
    }
  return sig;
}

losig_list *efg_addlosig (lofig_list *fig,losig_list *sig, int incr_index)
{
    spisig_list *spisig;
    losig_list *losig;
    ptype_list *ptype;
    ht         *htsig;
    float       alim;
    int         indexsig;
    char       *signame;
    int         flagvss=0,flagvdd=0;

    if ((!fig) || (!sig))
        return NULL;
    if ((ptype = getptype (sig->USER,EFG_CORRESP)) == NULL) {
      if (incr_index > 0)
        indexsig = sig->INDEX + getnumberoflosig (fig)+1;
      else
        indexsig = sig->INDEX ;

      signame = efg_getlosigname (sig);

      if ( mbk_LosigIsVSS(sig) ) {
        flagvss = 1;
        if ( !getlosigalim (sig,NULL) ) {
          losig = efg_get_vss_on_destfig ();
          if (losig && !getlosigalim (losig,NULL))
            return losig;
        }
      }
      if ( mbk_LosigIsVDD(sig) ) {
        flagvdd = 1;
        if ( !getlosigalim (sig,NULL) ) {
          losig = efg_get_vdd_on_destfig ();
          if ( losig && !getlosigalim (losig,NULL))
            return losig;
        }
      }
      spisig = efg_GetSpiSig(EFGCONTEXT->SPISIG,signame);
      if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE ) 
        signame = efg_sig_rename ( sig );

      losig = addlosig (fig,indexsig,
                        addchain(NULL,signame),
                        sig->TYPE);

      efg_SetDestSig2SpiSig ( spisig, losig);

      if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE )
        losig->USER = addptype ( losig->USER,
                                 EFG_ALIAS_ORG,
                                 efg_getlosigname(sig));

      // remplit une ht pour les signaux de la figure extraite
      ptype = getptype (fig->USER,EFG_FIG_HTSIG);
      if (!ptype) {
          htsig = addht (50);
          fig->USER = addptype (fig->USER,EFG_FIG_HTSIG,htsig);
      }
      else 
          htsig = (ht*)ptype->DATA;
      addhtitem (htsig,efg_getlosigname(losig),(long)losig);

      if ( flagvss ) {
          losig->USER = addptype (losig->USER,EFG_SIG_VSS,NULL);
          losig->USER = addptype (losig->USER,EFG_SIG_ALIM,NULL);
          efg_set_vss_on_destfig (losig);
          losig->ALIMFLAGS = sig->ALIMFLAGS;
      }
      if ( flagvdd ) {
          losig->USER = addptype (losig->USER,EFG_SIG_VDD,NULL);
          losig->USER = addptype (losig->USER,EFG_SIG_ALIM,NULL);
          efg_set_vdd_on_destfig (losig);
          losig->ALIMFLAGS = sig->ALIMFLAGS;
      }
      sig->USER = addptype (sig->USER,
                            EFG_CORRESP,
                            losig );
      losig->USER = addptype (losig->USER,
                              EFG_CORRESP,
                              sig );
      // on ne traite pas les RC des signaux d'alim
      if (sig->PRCN != NULL) {
          if (!getptype(sig->USER,EFG_SIG_ALIM)) {
              duplorcnet( losig, sig );
          }
          else
              addlorcnet (losig);
      }

	  // duplique les ptypes concernant les infos pour la sensibilisation
	  efg_dup_ptype (losig,sig);
	  // duplique les infos d alimentation
      if ( getlosigalim(sig, &alim) )
      {
        addlosigalim(losig, alim, NULL);
        efg_try_to_add_alim_to_the_context(losig, alim);
      }
      else if (getptype(sig->USER,EFG_SIG_ALIM)) {
        if (mbk_LosigIsVDD(sig))
          efg_try_to_add_alim_to_the_context(losig, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
        else
          efg_try_to_add_alim_to_the_context(losig, 0);
      }
    }
    else
        losig = (losig_list*)ptype->DATA;
    return losig;
}


/*****************************************************************************/

void efg_rebuild_ctc( lofig_list *figure )
{
  losig_list *losig ;
  losig_list *vss ;
  losig_list *ctksig ;
  int         node ;
  int         ctknode ;
  losig_list *losig_on_src ;
  losig_list *ctksig_on_src ;
  chain_list *chainctc ;
  loctc_list *loctc ;
  ptype_list *ptl ;

  
  vss = efg_get_vss_on_destfig();
 
  for( losig = figure->LOSIG ; losig ; losig = losig->NEXT ) {
 
    if( losig->PRCN ) {
   
      ptl = getptype( losig->USER, EFG_CORRESP );
      if( ptl ) { 

        losig_on_src = (losig_list*)ptl->DATA;

        for( chainctc = losig_on_src->PRCN->PCTC ; chainctc ; chainctc = chainctc->NEXT ) {
        
          loctc         = (loctc_list*)chainctc->DATA ;
          node          = rcn_ctcnode( loctc, losig_on_src );
          ctknode       = rcn_ctcothernode( loctc, losig_on_src );
          ctksig_on_src = rcn_ctcothersig( loctc, losig_on_src );

          ptl           = getptype( ctksig_on_src->USER, EFG_CORRESP );

          if( getptype( losig_on_src->USER, EFG_SIG_VSS ) ||
              getptype( losig_on_src->USER, EFG_SIG_VDD )    )
            node = 1 ;

          if( getptype( ctksig_on_src->USER, EFG_SIG_VSS ) ||
              getptype( ctksig_on_src->USER, EFG_SIG_VDD )    )
            ctknode = 1 ;

          if( ptl ) {
            ctksig = (losig_list*)ptl->DATA ;
            if( losig_on_src < ctksig_on_src ) {
              addloctc( losig, node, ctksig, ctknode, loctc->CAPA );
            }
          }
          else {
            addloctc( losig, node, vss, 1, loctc->CAPA );
          }
        }
      }
    }
  }
}

/*****************************************************************************/

locon_list *efg_add_global_alim (lofig_list *fig,losig_list *sig,int incr_index)
{
    losig_list *losig;
    locon_list *locon;
    ptype_list *ptype;
    char       *signame;

    if ((!fig) || (!sig))
        return NULL;
    signame = efg_getlosigname (sig);
    if ((losig = efg_get_ht_sig (fig,signame )) == NULL ) {
        losig = efg_addlosig (fig,sig,incr_index);
        locon = addlocon (fig,signame,sig,'I');
        efg_locon_add_lofigchain( locon );
        losig->USER = addptype (losig->USER,
                                EFG_SIG_LOCON,
                                locon);
    }
    else {
        if ((ptype = getptype (losig->USER, EFG_SIG_LOCON)) != NULL ) 
            locon = (locon_list*)ptype->DATA;
        else
            avt_errmsg(EFG_ERRMSG, "008", AVT_WARNING, efg_getlosigname(sig));
/*            fprintf (stderr,"[EFG WAR] can't add global alim for sig %s !\n",
                            efg_getlosigname(sig));*/
    }
    return locon;
}

/*****************************************************************************/

locon_list *efg_addlocon (lofig_list *fig,locon_list *con, int incr_index)
{
    losig_list *sig;
    locon_list *locon;
    ptype_list *ptype;
    char       *signame;
    char        dir;

    if ((!fig) || (!con))
        return NULL;
    if ((ptype = getptype (con->USER,EFG_CORRESP)) == NULL) {
        sig = efg_addlosig(fig,con->SIG,incr_index);
        
        if ((ptype = getptype (sig->USER,EFG_SIG_LOCON)))
          return (locon_list *)ptype->DATA;
        
        dir = con->DIRECTION;
        if (dir != 'I' && dir != 'O') dir = 'X';
        signame = efg_getlosigname(sig);
        locon = addlocon (fig,signame,sig,dir);
        efg_locon_add_lofigchain( locon );
        if ((mbk_LosigIsVDD (con->SIG) || (mbk_LosigIsVSS (con->SIG)))
            && !getptype (sig->USER,EFG_SIG_LOCON)) {
          setloconnode (locon,1);
        }
        sig->USER = addptype (sig->USER,
                              EFG_SIG_LOCON,
                              locon);
        con->USER = addptype (con->USER,
                              EFG_CORRESP,
                              locon);
        locon->USER = addptype (locon->USER,
                                EFG_CORRESP,
                                con);
        if (getptype(con->SIG->USER,EFG_SIG_VSS) == NULL)
           locon->PNODE = dupnumlst(con->PNODE);
    }
    else 
        locon = (locon_list*)ptype->DATA;
    return locon;
}

/*****************************************************************************/
void efg_clearaddedequivcapa(lotrs_list *trs)
{
  ptype_list *pt;
  if ((pt=getptype(trs->GRID->USER, EFG_LOCON_CAPA))!=NULL)
   {
//     printf("ERROR G %s %g\n",trs->TRNAME,((loctc_list *)pt->DATA)->CAPA);
     delloctc((loctc_list *)pt->DATA);
     trs->GRID->USER=delptype(trs->GRID->USER, EFG_LOCON_CAPA);
   }
  if ((pt=getptype(trs->DRAIN->USER, EFG_LOCON_CAPA))!=NULL)
   {
//     printf("ERROR D %s %g\n",trs->TRNAME,((loctc_list *)pt->DATA)->CAPA);
     delloctc((loctc_list *)pt->DATA);
     trs->DRAIN->USER=delptype(trs->DRAIN->USER, EFG_LOCON_CAPA);
   }
  if ((pt=getptype(trs->SOURCE->USER, EFG_LOCON_CAPA))!=NULL)
   {
//     printf("ERROR S %s %g\n",trs->TRNAME,((loctc_list *)pt->DATA)->CAPA);
     delloctc((loctc_list *)pt->DATA);
     trs->SOURCE->USER=delptype(trs->SOURCE->USER, EFG_LOCON_CAPA);
   }
  if (trs->BULK!=NULL && (pt=getptype(trs->BULK->USER, EFG_LOCON_CAPA))!=NULL)
   {
//     printf("ERROR B %s %g\n",trs->TRNAME,((loctc_list *)pt->DATA)->CAPA);
     delloctc((loctc_list *)pt->DATA);
     trs->BULK->USER=delptype(trs->BULK->USER, EFG_LOCON_CAPA);
   }
}

static losig_list *check_already_alim(losig_list *ls, losig_list *vss, losig_list *vdd)
{
  if (ls!=NULL)
  {
    if (mbk_LosigIsVDD(ls)) return vdd;
    if (mbk_LosigIsVSS(ls)) return vss;
  }
  return NULL;
}

lotrs_list *efg_add_blockedlotrs (lofig_list *fig,locon_list *con,int incr_index)
{
    ptype_list *ptype;
    losig_list *grid,*source,*drain,*bulk,*vss,*vdd;
    lotrs_list *lotrs,*trs;
    char buf[1024];
    int newsig=0;
    int pnode_drain=0,pnode_source=0,pnode_grid=0;
	alim_list *alim;

    if ( !fig || !con  || efg_SigIsAlim (con->SIG) == 'Y')
        return NULL;
    trs = (lotrs_list*)con->ROOT;
    if ((ptype = getptype (trs->USER,EFG_CORRESP)) == NULL) {
        // zinaps : error check
        efg_clearaddedequivcapa(trs);
        //
		if ((alim=cns_get_lotrs_multivoltage(trs))!=NULL)
		{
	      vss=efg_try_to_add_alim_to_the_context(NULL, alim->VSSMIN);
	      vdd=efg_try_to_add_alim_to_the_context(NULL, alim->VDDMAX);
		}
		else
		{
          vss = EFGCONTEXT->VSSDESTFIG;
          vdd = EFGCONTEXT->VDDDESTFIG;
		}
        if (trs->BULK==NULL || (bulk=check_already_alim(trs->BULK->SIG, vss, vdd))==NULL)
          bulk =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;

        if ( con == trs->DRAIN ) {
          newsig++;
          drain = efg_addlosig (fig,trs->DRAIN->SIG,incr_index);
          pnode_drain=1;
          if (getptype (trs->SOURCE->SIG->USER,EFG_SIG_DRV)) {
            source = efg_addlosig (fig,trs->SOURCE->SIG,incr_index);
            newsig++;
            pnode_source=1;
          }
          else
          {
            if ((source=check_already_alim(trs->SOURCE->SIG, vss, vdd))==NULL)
              source = MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
          }
          if (getptype (trs->GRID->SIG->USER,EFG_SIG_DRV)) {
            grid = efg_addlosig (fig,trs->GRID->SIG,incr_index);
            newsig++;
            pnode_grid=1;
          }
          else
          {
            if ((grid=check_already_alim(trs->GRID->SIG, vss, vdd))==NULL)
              grid =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
          }
        }
        else if ( con == trs->SOURCE ) {
          source = efg_addlosig (fig,trs->SOURCE->SIG,incr_index);
          newsig++;
          pnode_source=1;
          if (getptype (trs->DRAIN->SIG->USER,EFG_SIG_DRV)) {
            drain = efg_addlosig (fig,trs->DRAIN->SIG,incr_index);
            newsig++;
            pnode_drain=1;
          }
          else
          {
            if ((drain=check_already_alim(trs->DRAIN->SIG, vss, vdd))==NULL)
              drain =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
          }
          if (getptype (trs->GRID->SIG->USER,EFG_SIG_DRV)) {
            grid = efg_addlosig (fig,trs->GRID->SIG,incr_index);
            newsig++;
            pnode_grid=1;
          }
          else
          {
            if ((grid=check_already_alim(trs->GRID->SIG, vss, vdd))==NULL)
              grid =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
          }
        }
        else if ( con == trs->GRID ) { 
          int drainalim=0, sourcealim=0;
          newsig++;
          grid =  efg_addlosig (fig,trs->GRID->SIG,incr_index);
          pnode_grid=1;
          if (getptype (trs->DRAIN->SIG->USER,EFG_SIG_DRV)) {
            drain = efg_addlosig (fig,trs->DRAIN->SIG,incr_index);
            newsig++;
            pnode_drain=1;
          }
          else
          {
            if ((drain=check_already_alim(trs->DRAIN->SIG, vss, vdd))==NULL)
              drain =  vss; //MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
            else
              drainalim=1;
          }
          if (getptype (trs->SOURCE->SIG->USER,EFG_SIG_DRV)) {
            source = efg_addlosig (fig,trs->SOURCE->SIG,incr_index);
            newsig++;
            pnode_source=1;
          }
          else
          {
            if ((source=check_already_alim(trs->SOURCE->SIG, vss, vdd))==NULL)
              source = vdd; //MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
            else
              sourcealim=1;

          }
          if (sourcealim!=drainalim && newsig==1)
          {
            if (sourcealim && source==vss) drain=vdd;
            else if (drainalim && drain==vdd) source=vss;
          }
        }
        else { // con = bulk
          return NULL;
        }
        // rajoute un transisor bloque en plus si le 1er transitor fait partie
        // d une branche non fonctionnel (a cause d evenetuelles contraintes)
        if (newsig == 2) {
          if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE ) 
            sprintf ( buf, "nFct%s%d",V_STR_TAB[__EFG_TRS_ALIAS].VALUE,EFG_TRS_INDEX);
          else
            sprintf ( buf, "nFct%s",trs->TRNAME);
          lotrs=NULL;
          if (( drain == vss || drain == vdd) && !(mbk_LosigIsVDD(trs->DRAIN->SIG) || mbk_LosigIsVSS(trs->DRAIN->SIG))) {
            pnode_drain=1;
            drain = efg_addlosig (fig,trs->DRAIN->SIG,incr_index);
            avt_log(LOGEFG,2,"Extra transistor added on drain for transistor %s\n",trs->TRNAME);
            if ( MLO_IS_TRANSN (trs->TYPE) ) {
              lotrs = addlotrs (fig, trs->TYPE, trs->X, trs->Y, trs->WIDTH, trs->LENGTH,
                                trs->PS, trs->PD, trs->XS, trs->XD, vss, vss, drain, vss, namealloc(buf)
                               );
              efg_lotrs_add_lofigchain( lotrs );
            }
            else {
              lotrs = addlotrs (fig, trs->TYPE, trs->X, trs->Y, trs->WIDTH, trs->LENGTH,
                                trs->PS, trs->PD, trs->XS, trs->XD, vdd, vdd, drain, vdd, namealloc(buf)
                               );
              efg_lotrs_add_lofigchain( lotrs );
            }
          }
          else if ( (source == vss || source == vdd)  && !(mbk_LosigIsVDD(trs->SOURCE->SIG) || mbk_LosigIsVSS(trs->SOURCE->SIG))) {
            pnode_source=1;
            source = efg_addlosig (fig,trs->SOURCE->SIG,incr_index);
            avt_log(LOGEFG,2,"Extra transistor added on source for transistor %s\n",trs->TRNAME);
            if ( MLO_IS_TRANSN (trs->TYPE) ) {
              lotrs = addlotrs (fig, trs->TYPE, trs->X, trs->Y, trs->WIDTH, trs->LENGTH,
                        trs->PS, trs->PD, trs->XS, trs->XD, vss, source, vss, vss, namealloc(buf)
                       );
              efg_lotrs_add_lofigchain( lotrs );
            }
            else {
              lotrs = addlotrs (fig, trs->TYPE, trs->X, trs->Y, trs->WIDTH, trs->LENGTH,
                                trs->PS, trs->PD, trs->XS, trs->XD, vdd, source, vdd, vdd, namealloc(buf)
                               );
              efg_lotrs_add_lofigchain( lotrs );
            }
          }
          if (lotrs)
          {
            lotrs->MODINDEX = trs->MODINDEX;
            // subcktname of the figure 
            if ( (ptype = getptype ( trs->USER, TRANS_FIGURE )) )
              lotrs->USER = addptype ( lotrs->USER, TRANS_FIGURE, ptype->DATA );
            efg_copy_lotrs_param(trs, lotrs);
          }
        }
        
        lotrs = addlotrs (fig,
                          trs->TYPE,
                          trs->X,
                          trs->Y,
                          trs->WIDTH,
                          trs->LENGTH,
                          trs->PS,
                          trs->PD,
                          trs->XS,
                          trs->XD,
                          grid,
                          source,
                          drain,
                          bulk,
                          trs->TRNAME
                         );
        efg_lotrs_add_lofigchain( lotrs );
        lotrs->FLAGS = EFG_BLOCK_TRS;
        if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE ) {
          sprintf ( buf, "%s%d",V_STR_TAB[__EFG_TRS_ALIAS].VALUE,EFG_TRS_INDEX++);
          lotrs->TRNAME = namealloc (buf);
          if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE )
            lotrs->USER = addptype (lotrs->USER,EFG_ALIAS_ORG,trs->TRNAME);
        }
        // champ pour le nom du transitor
        lotrs->MODINDEX = trs->MODINDEX;

        if ( pnode_drain )
           lotrs->DRAIN->PNODE = dupnumlst(trs->DRAIN->PNODE);
        if ( pnode_source )
           lotrs->SOURCE->PNODE = dupnumlst(trs->SOURCE->PNODE);
        if ( pnode_grid )
           lotrs->GRID->PNODE = dupnumlst(trs->GRID->PNODE);
        
        // subcktname of the figure 
        if ( (ptype = getptype ( trs->USER, TRANS_FIGURE )) )
          lotrs->USER = addptype ( lotrs->USER, TRANS_FIGURE, ptype->DATA );

        trs->USER = addptype (trs->USER,EFG_CORRESP,lotrs);
        lotrs->USER = addptype (lotrs->USER,EFG_CORRESP,trs);

        // specific instance params
        efg_copy_lotrs_param(trs, lotrs);
/*        val = getlotrsparam (trs,MBK_MULU0,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_MULU0, val, NULL);
        val = getlotrsparam (trs,MBK_M,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_M, val, NULL);
        val = getlotrsparam (trs,MBK_DELVT0,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_DELVT0, val , NULL);
        val= getlotrsparam (trs,MBK_SA,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_SA, val, NULL);
        val= getlotrsparam (trs,MBK_SB,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_SB, val, NULL);
        val = getlotrsparam (trs,MBK_SD,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_SD, val, NULL);
        val = getlotrsparam (trs,MBK_NF,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_NF, val, NULL);
        val = getlotrsparam (trs,MBK_NRS,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_NRS, val, NULL);
        val = getlotrsparam (trs,MBK_NRD,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_NRD, val, NULL);
        val = getlotrsparam (trs,MBK_GEOMOD,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_GEOMOD, val, NULL);
 */
    }
    else 
        lotrs = (lotrs_list*)ptype->DATA;

    return lotrs;
}

void efg_lotrs_add_lofigchain( lotrs_list *lotrs )
{
  efg_locon_add_lofigchain( lotrs->DRAIN );
  efg_locon_add_lofigchain( lotrs->GRID );
  efg_locon_add_lofigchain( lotrs->SOURCE );
  efg_locon_add_lofigchain( lotrs->BULK );
}

void efg_locon_add_lofigchain( locon_list *locon ) 
{
  ptype_list *ptl ;

  if (locon->SIG!=NULL) {
    ptl = getptype( locon->SIG->USER, LOFIGCHAIN );
    if( !ptl ) {
      locon->SIG->USER = addptype( locon->SIG->USER, LOFIGCHAIN, NULL );
      ptl = locon->SIG->USER ;
    }

    ptl->DATA = (void*)addchain( (chain_list*)ptl->DATA, locon );
  }
}

void efg_lotrs_remove_lofigchain( lotrs_list *lotrs )
{
  efg_locon_remove_lofigchain( lotrs->DRAIN );
  efg_locon_remove_lofigchain( lotrs->GRID );
  efg_locon_remove_lofigchain( lotrs->SOURCE );
  efg_locon_remove_lofigchain( lotrs->BULK );
}


void efg_locon_remove_lofigchain( locon_list *locon )
{
  ptype_list *ptl ;
  ptl = getptype( locon->SIG->USER, LOFIGCHAIN ) ;
  if( ptl /* always true */ ) {
    ptl->DATA = (void*)delchaindata( (chain_list*)ptl->DATA, locon );
  }
}

/*****************************************************************************\
 *
 * Func : efg_addloconfromlosig
 
 Add external connector from an external signal

\*****************************************************************************/

locon_list *efg_addloconfromlosig (lofig_list *figext,losig_list *origsig,int incr_index)
{
  ptype_list *ptype;
  chain_list *lofigchain,*chain;
  locon_list *locon;

  ptype = getptype ( origsig->USER, LOFIGCHAIN );
  if ( ptype ) {
    lofigchain = (chain_list *)ptype->DATA;
    for ( chain = lofigchain ; chain ; chain=chain->NEXT ) {
      locon = (locon_list *)chain->DATA;
      //if ( locon->TYPE == 'E' && !efg_get_locon ( figext, locon->NAME ) )
      if ( locon->TYPE == 'E' )
        return efg_addlocon ( figext, locon, incr_index);
    }
  }
  return NULL;
}

/*****************************************************************************/
lotrs_list *efg_addlotrs (lofig_list *fig,lotrs_list *trs,int incr_index)
{
    ptype_list *ptype;
    losig_list *grid,*source,*drain,*bulk=NULL;
    lotrs_list *lotrs;
    locon_list *locon;
    char buf[1024];
    char *signame;

    if ( !fig || !trs )
        return NULL;
    if ((ptype = getptype (trs->USER,EFG_CORRESP)) == NULL) {
        // zinaps: error check
        efg_clearaddedequivcapa(trs);
        //
        grid = efg_addlosig (fig,trs->GRID->SIG,incr_index);
        if ( trs->GRID->SIG->TYPE == 'E' ) 
          efg_addloconfromlosig (fig,trs->GRID->SIG,incr_index);
        else if ( efg_sig_is_beg (trs->GRID->SIG,NULL)) {
            signame = efg_getlosigname (grid);
            if (!efg_get_locon ( fig, signame ) ) {
               locon = addlocon (fig,signame,grid,'I');
               efg_locon_add_lofigchain( locon );
               // zinaps: marquage pour eviter le multiple ajout d'un meme locon
               trs->GRID->USER = addptype (trs->GRID->USER, EFG_CORRESP, locon);
               locon->USER = addptype (locon->USER, EFG_CORRESP, trs->GRID);
               //
               grid->TYPE = 'E';
               locon->TYPE = 'E';
               if ( trs->GRID->PNODE )
                 locon->PNODE = dupnumlst(trs->GRID->PNODE);
            }
        }

        source = efg_addlosig (fig,trs->SOURCE->SIG,incr_index);
        if ( trs->SOURCE->SIG->TYPE == 'E' ) 
          efg_addloconfromlosig (fig,trs->SOURCE->SIG,incr_index);
        drain = efg_addlosig (fig,trs->DRAIN->SIG,incr_index);
        if ( trs->DRAIN->SIG->TYPE == 'E' ) 
          efg_addloconfromlosig (fig,trs->DRAIN->SIG,incr_index);
        if ( trs->BULK->SIG ) {
          bulk = efg_addlosig (fig,trs->BULK->SIG,incr_index);
          if ( trs->BULK->SIG->TYPE == 'E' ) 
            efg_addloconfromlosig (fig,trs->BULK->SIG,incr_index);
        }
        lotrs = addlotrs (fig,
                          trs->TYPE,
                          trs->X,
                          trs->Y,
                          trs->WIDTH,
                          trs->LENGTH,
                          trs->PS,
                          trs->PD,
                          trs->XS,
                          trs->XD,
                          grid,
                          source,
                          drain,
                          bulk,
                          trs->TRNAME
                         );
        efg_lotrs_add_lofigchain( lotrs );
        lotrs->FLAGS = EFG_GOOD_TRS;
        if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE ) {
          sprintf ( buf, "%s%d",V_STR_TAB[__EFG_TRS_ALIAS].VALUE,EFG_TRS_INDEX++);
          lotrs->TRNAME = namealloc (buf);
          if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE )
            lotrs->USER = addptype (lotrs->USER,EFG_ALIAS_ORG,trs->TRNAME);
        }
        // champ pour le nom du transitor
        lotrs->MODINDEX = trs->MODINDEX;
        
        // locon GRID specific duplication
        if (getptype(trs->GRID->SIG->USER,EFG_SIG_VSS) == NULL)
           lotrs->GRID->PNODE = dupnumlst(trs->GRID->PNODE);
        // locon DRAIN specific duplication
        if (getptype(trs->DRAIN->SIG->USER,EFG_SIG_VSS) == NULL)
           lotrs->DRAIN->PNODE = dupnumlst(trs->DRAIN->PNODE);
        // locon SOURCE specific duplication
        if (getptype(trs->SOURCE->SIG->USER,EFG_SIG_VSS) == NULL)
           lotrs->SOURCE->PNODE = dupnumlst(trs->SOURCE->PNODE);
        // locon BULK specific duplication
        if (trs->BULK->SIG && getptype(trs->BULK->SIG->USER,EFG_SIG_VSS) == NULL)
           lotrs->BULK->PNODE = dupnumlst(trs->BULK->PNODE);

        // subcktname of the figure 
        if ( (ptype = getptype ( trs->USER, TRANS_FIGURE )) )
          lotrs->USER = addptype ( lotrs->USER, TRANS_FIGURE, ptype->DATA );
        
        if ( (ptype = getptype ( trs->USER, PARAM_CONTEXT )) ){
          eqt_ctx *ctx = eqt_init (50);
          eqt_import_vars (ctx, ptype->DATA);
          lotrs->USER = addptype ( lotrs->USER, PARAM_CONTEXT, eqt_export_vars (ctx));
          eqt_term (ctx);
        }

        trs->USER = addptype (trs->USER,EFG_CORRESP,lotrs);
        lotrs->USER = addptype (lotrs->USER,EFG_CORRESP,trs);
        
        // specific instance params
        efg_copy_lotrs_param(trs, lotrs);
/*
        mulu0 = getlotrsparam (trs,MBK_MULU0,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_MULU0, mulu0, NULL);
        val = getlotrsparam (trs,MBK_M,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_M, val, NULL);
        delvt0 = getlotrsparam (trs,MBK_DELVT0,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_DELVT0, delvt0, NULL);
        sa = getlotrsparam (trs,MBK_SA,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_SA, sa, NULL);
        sb = getlotrsparam (trs,MBK_SB,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_SB, sb, NULL);
        sd = getlotrsparam (trs,MBK_SD,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_SD, sd, NULL);
        nf = getlotrsparam (trs,MBK_NF,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_NF, nf, NULL);
        val = getlotrsparam (trs,MBK_NRS,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_NRS, val, NULL);
        val = getlotrsparam (trs,MBK_NRD,NULL,&status);
        if ( status == 1 )
          addlotrsparam(lotrs, *MBK_NRD, val, NULL);
          */
    }
    else {
        lotrs = (lotrs_list*)ptype->DATA;
        if ( lotrs->FLAGS == EFG_BLOCK_TRS ) { // if it was added blocked by another not fonctionnal cone
          efg_lotrs_remove_lofigchain( lotrs );
          dellotrs ( fig, lotrs );
          trs->USER = delptype (trs->USER,EFG_CORRESP);
          return efg_addlotrs ( fig, trs, incr_index );
        }
    }

    return lotrs;
}

/**** Duplique une instance si elle ne l avais pas ete faite *****************/

loins_list *efg_addloins (lofig_list *fig,loins_list *ins,int incr_index)
{
  ptype_list *ptype,*ph_interf;
  loins_list *newloins;
  losig_list *sigdest,*vss,*vdd,*sigsrc;
  locon_list *locon,*figlocon,*loconins,*newextlocon;
  chain_list *sigchain = NULL;
  lofig_list *figins;

  if (!ins || !(figins = getloadedlofig(ins->FIGNAME)))
    return NULL;
  if ((ptype = getptype (ins->USER,EFG_CORRESP)) == NULL) {

    vss = efg_get_vss_on_destfig (); 
    vdd = efg_get_vdd_on_destfig (); 

    for (locon = ins->LOCON ; locon ; locon=locon->NEXT) {
      figlocon = getlocon (figins,locon->NAME);
      figlocon->USER = addptype (figlocon->SIG->USER,
                                 EFG_CONFIG_CONINS,
                                 locon);
    }
    for (locon = figins->LOCON ; locon ; locon=locon->NEXT) {
      if ((ptype = getptype (locon->USER,
                             EFG_CONFIG_CONINS)) != NULL) {
        loconins = (locon_list*)ptype->DATA;
        sigsrc = loconins->SIG;
        if (mbk_LosigIsVSS(locon->SIG))
        sigdest = vss;
        else if (mbk_LosigIsVDD(locon->SIG))
          sigdest = vdd;
        else 
          sigdest = efg_addlosig (fig,sigsrc,incr_index);
        sigchain = addchain (sigchain,sigdest);
        if (sigdest != vss && sigdest != vdd && 
            !efg_dup_hier_ptype (sigdest,sigsrc,ins->INSNAME)) {
          newextlocon = efg_addlocon (fig,
                                      loconins,
                                      0);
          newextlocon->SIG->TYPE = EXTERNAL;
          newextlocon->TYPE = EXTERNAL;
        }
        if ((getptype (locon->USER,EFG_CONFIG_CONINS)))
          locon->USER = delptype (locon->USER,EFG_CONFIG_CONINS);
      }
    }
    if ((sigchain)) sigchain = reverse (sigchain);

    newloins = addloins( fig, ins->INSNAME, figins,sigchain);

    // update locon name
    for (loconins = newloins->LOCON ; loconins ; loconins=loconins->NEXT) {
      loconins->NAME = efg_getlosigname (loconins->SIG);
      for (locon = ins->LOCON ; locon ; locon=locon->NEXT) {
        if (efg_SigIsAlim (locon->SIG) == 'N' && 
            efg_getlosigname (locon->SIG) == loconins->NAME) {
          if (locon->PNODE != NULL)
            loconins->PNODE = dupnumlst(locon->PNODE);
        }
      }
    }

    if ((ph_interf = getptype (ins->USER,PH_INTERF)) != NULL) 
        newloins->USER = addptype (newloins->USER,
                                   PH_INTERF,
                                   (void*)dupchainlst(ph_interf->DATA));
  }
  else 
    newloins = (loins_list*)ptype->DATA;

  return newloins;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_SigIsAlim                                                  */
/*                                                                           */
/* renvoi 'Y' si le signal est une alimentation.                             */
/*                                                                           */
/*****************************************************************************/
char efg_SigIsAlim( losig_list *sig )
{
    if ( (mbk_LosigIsVDD(sig) ) || ( mbk_LosigIsVSS(sig) ))
        return 'Y';
    else
        return 'N';
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_DelLofigPtype                                              */
/*                                                                           */
/* Supprime un ptype d'une lofig                                             */
/*                                                                           */
/*****************************************************************************/
void efg_DelLofigPtype(lofig,type)
    lofig_list *lofig;
    long        type;
{
 loins_list *loins;

 for(loins = lofig->LOINS ; loins ; loins = loins->NEXT)
     efg_DelLofigPtype(getloadedlofig(loins->FIGNAME),type);

 if (getptype(lofig->USER,type) != NULL)
     lofig->USER = delptype(lofig->USER,type);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetHierSigByName                                           */
/*                                                                           */
/* Recupere un signal a partir de son nom hierarchique.                      */
/*                                                                           */
/* NB : le nom hier ne comporte pas le nom du top level                      */
/*                                                                           */
/*****************************************************************************/
losig_list *efg_GetHierSigByName (lofig,name,loinsname,instance,firstcall)
    lofig_list  *lofig;
    char        *name;
    char       **loinsname;/*nom complet du top level jusqu'a la derniere inst*/
    loins_list **instance;
    char         firstcall;
{
    char        *left;
    char        *right;
    loins_list  *loins;
    losig_list  *losig;
    lofig_list  *newlofig;

    //name = namealloc (name);
    if ((lofig != NULL) && (name != NULL)) {
      if ((losig=mbk_quickly_getlosigbyname(lofig, name))!=NULL)
      {
        if (firstcall == 'Y') *loinsname = lofig->NAME;
        return losig;
      }
/*      for (losig = lofig->LOSIG ; losig ; losig = losig->NEXT) {
          if (mbkissignalname (losig, name )) {
            if (firstcall == 'Y')
              *loinsname = lofig->NAME;
            return losig;
          }
      }*/
      // ca sert a quoi??? vvvvv
      leftunconcatname (name,&left,&right);
      if (left == NULL) {
        if ((losig=mbk_quickly_getlosigbyname(lofig, right))!=NULL)
        {
          if (firstcall == 'Y') *loinsname = lofig->NAME;
          return losig;
        }
      // ^^^^^
/*        for (losig = lofig->LOSIG ; losig ; losig = losig->NEXT) {
          if (mbkissignalname (losig, right )) {
            if (firstcall == 'Y')
              *loinsname = lofig->NAME;
            return (losig);
          }
        }*/
      }
      else {
        for (loins = lofig->LOINS ; loins ; loins = loins->NEXT) {
          if (loins->INSNAME == left) {
            if (firstcall == 'Y') {
              if (loinsname)
                *loinsname = concatname (lofig->NAME,left);
              if (instance != NULL) 
                *instance = loins;
            }
            else {
              if (loinsname)
                *loinsname = concatname (*loinsname,left);
            }
            newlofig = efg_get_fig2ins (loins);
            if (!newlofig)
              newlofig = getloadedlofig (loins->FIGNAME);
            return efg_GetHierSigByName (newlofig,right,loinsname,instance,'N');
          }
        }
      }
    }
    return NULL;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_DelHierSigPtype                                            */
/*                                                                           */
/* Supression hierarchique des ptype des signaux                             */
/*                                                                           */
/*****************************************************************************/
void efg_DelHierSigPtype (lofig)
    lofig_list *lofig;
{
    loins_list *loins;
    losig_list *losig;
    ptype_list *ptype;
    chain_list *chain;
    lotrs_list *lotrs;
    locon_list *locon;

    if (lofig != NULL) {
      for (loins = lofig->LOINS ; loins ; loins = loins->NEXT) {
          if (getptype(loins->USER,EFG_INS_OUTPATH) != NULL)
                loins->USER = delptype (loins->USER,EFG_INS_OUTPATH);
          if (getptype(loins->USER,EFG_CORRESP) != NULL)
                loins->USER = delptype (loins->USER,EFG_CORRESP);
          if (getptype(loins->USER,EFG_INS_NO_COPY) != NULL)
                loins->USER = delptype (loins->USER,EFG_INS_NO_COPY);
          if (getptype( loins->USER,EFG_INS_FIG ) != NULL)
                loins->USER = delptype ( loins->USER, EFG_INS_FIG );
          if (getptype( loins->USER,EFG_INS_CTXT) != NULL)
                loins->USER = delptype ( loins->USER, EFG_INS_CTXT);
          efg_DelHierSigPtype (getloadedlofig (loins->FIGNAME));
      }
    }
    if (lofig != NULL) {
        if ((ptype = getptype (lofig->USER,EFG_FIG_HTSIG)) != NULL) {
            delht ((ht*)ptype->DATA);
            lofig->USER = delptype(lofig->USER,EFG_FIG_HTSIG) ;
        }
        if (getptype (lofig->USER,EFG_SIG_VSS) != NULL)
            lofig->USER = delptype (lofig->USER,EFG_SIG_VSS);
        for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT) {
            if(getptype(lotrs->USER,EFG_CORRESP) != NULL)
               lotrs->USER = delptype(lotrs->USER,EFG_CORRESP) ;
        }
        for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT) {
            if(getptype(locon->USER,EFG_CORRESP) != NULL)
               locon->USER = delptype(locon->USER,EFG_CORRESP) ;
        }
        for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT) {
            if((ptype = getptype(losig->USER,EFG_SIG_SET_HIER)) != NULL) {
               delht ((ht*)ptype->DATA);
               losig->USER = delptype(losig->USER,EFG_SIG_SET_HIER) ;
            }
            if((ptype = getptype(losig->USER,EFG_SIG_BEG_HIER)) != NULL) {
               delht ((ht*)ptype->DATA);
               losig->USER = delptype(losig->USER,EFG_SIG_BEG_HIER) ;
            }
            if((ptype = getptype(losig->USER,EFG_SIG_END_HIER)) != NULL) {
               delht ((ht*)ptype->DATA);
               losig->USER = delptype(losig->USER,EFG_SIG_END_HIER) ;
            }
            if((ptype = getptype(losig->USER,EFG_SIG_SET_NUM)) != NULL) {
               delht ((ht*)ptype->DATA);
               losig->USER = delptype(losig->USER,EFG_SIG_SET_NUM) ;
            }
            losig->USER = testanddelptype(losig->USER,EFG_FORCE_IC) ;
            if(getptype(losig->USER,EFG_SIG2CONE) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG2CONE) ;
            if(getptype(losig->USER,EFG_SIG_DRV) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_DRV) ;
            if(getptype(losig->USER,EFG_SIG_CLK) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_CLK) ;
            if(getptype(losig->USER,EFG_SIG_SET) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_SET) ;
            if(getptype(losig->USER,EFG_SIG_BEG) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_BEG) ;
            if(getptype(losig->USER,EFG_SIG_END) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_END) ;
            if(getptype(losig->USER,EFG_SIG_VDD) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_VDD) ;
            if(getptype(losig->USER,EFG_SIG_VSS) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_VSS) ;
            if(getptype(losig->USER,EFG_SIG_ALIM) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_ALIM) ;
            if(getptype(losig->USER,EFG_SIG_TERMINAL) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_TERMINAL) ;
            if(getptype(losig->USER,EFG_SIG_INITIAL) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_INITIAL) ;
            if(getptype(losig->USER,EFG_SIG_CONE) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_CONE) ;
            if(getptype(losig->USER,EFG_SIG_LOCON) != NULL)
               losig->USER = delptype(losig->USER,EFG_SIG_LOCON) ;
            if(getptype(losig->USER,EFG_CORRESP) != NULL)
               losig->USER = delptype(losig->USER,EFG_CORRESP) ;
			ptype = getptype(losig->USER,LOFIGCHAIN) ;
            if (ptype != NULL) {
                for (chain = (chain_list *)ptype->DATA ; 
                     chain != NULL ;chain = chain->NEXT) {
                    locon = (locon_list *)chain->DATA ;
                    if(getptype(locon->USER,EFG_LOCON_CAPA) != NULL) 
                       locon->USER = delptype(locon->USER,EFG_LOCON_CAPA) ;
                    if(getptype(locon->USER,EFG_CORRESP) != NULL) {
                       locon->USER = delptype(locon->USER,EFG_CORRESP) ;
                       if (getptype(locon->SIG->USER,EFG_CORRESP))
                         locon->SIG->USER = delptype(locon->SIG->USER,EFG_CORRESP) ;
					}
                }
            }
        }
    }
}

void efg_DelHierSigPtypeFromSpisig (spisig_list *spisig)
{
  spisig_list *sl;
  losig_list *losig;
  ptype_list *ptype;

  for (sl=spisig; sl!=NULL; sl=sl->NEXT)
  {
    if ((losig = sl->SRCSIG)!=NULL)
    {
      if((ptype = getptype(losig->USER,EFG_SIG_SET_HIER)) != NULL) {
         delht ((ht*)ptype->DATA);
         losig->USER = delptype(losig->USER,EFG_SIG_SET_HIER) ;
      }
      if((ptype = getptype(losig->USER,EFG_SIG_BEG_HIER)) != NULL) {
         delht ((ht*)ptype->DATA);
         losig->USER = delptype(losig->USER,EFG_SIG_BEG_HIER) ;
      }
      if((ptype = getptype(losig->USER,EFG_SIG_END_HIER)) != NULL) {
         delht ((ht*)ptype->DATA);
         losig->USER = delptype(losig->USER,EFG_SIG_END_HIER) ;
      }
      if((ptype = getptype(losig->USER,EFG_SIG_SET_NUM)) != NULL) {
         delht ((ht*)ptype->DATA);
         losig->USER = delptype(losig->USER,EFG_SIG_SET_NUM) ;
      }
      losig->USER = testanddelptype(losig->USER,EFG_SIG2CONE) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_DRV) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_CLK) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_SET) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_BEG) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_END) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_VDD) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_VSS) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_ALIM) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_TERMINAL) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_INITIAL) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_CONE) ;
      losig->USER = testanddelptype(losig->USER,EFG_SIG_LOCON) ;
      losig->USER = testanddelptype(losig->USER,EFG_CORRESP) ;
      losig->USER = testanddelptype(losig->USER,EFG_FORCE_IC) ;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_FindLocon                                                  */
/*                                                                           */
/* Trouve le connecteur equivalent entre loins <=> lofig                     */
/*                                                                           */
/* loconname : nom du connecteur de la loins                                 */
/*                                                                           */
/* Renvoie le connecteur de la figure                                        */
/*                                                                           */
/*****************************************************************************/
locon_list *efg_FindLocon (lofig,loconname)
    lofig_list *lofig;
    char       *loconname;
{
    locon_list *locon;

    if (!lofig) {
        return NULL;
    }
    for (locon = lofig->LOCON ; locon  ;locon = locon->NEXT) {
        if (locon->NAME == loconname) {
            return locon;
        }
    }
    return NULL;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_SetHierPtype                                               */
/*                                                                           */
/* Rajoute un ptype sur un signal dans un contexte hierarchique.             */
/* La data du ptype est une table de hash dont la cle est le context hier.   */
/*                                                                           */
/*****************************************************************************/
ptype_list *efg_SetHierPtype (user,type,hashkey,hashval,flag)
    ptype_list *user;
    long        type;
    void       *hashkey;
    long        hashval;
    char       *flag;
{
    ptype_list *ptype;
    ht         *hash;
    char        set = 'N';

    if((ptype = getptype (user,type)) == NULL) {
        hash = addht (50);
        set = 'Y';
    }
    else {
        hash = (ht*)ptype->DATA;
        if (gethtitem (hash,hashkey) == EMPTYHT) {
            set = 'Y';
        }
    }
    if (set == 'Y') {
        addhtitem(hash,hashkey,hashval);
        if ((getptype (user,type)) != NULL)
            user = delptype (user,type);
        user = addptype (user,
                         type,
                         (void*)hash);
        if (flag != NULL)
            *flag = 'Y';
    }
    return user;
}

/****************************************************************************\

\*****************************************************************************/
void *efg_get_hier_sigptype (losig_list *sig,char *ctxt,long type)
{
    ptype_list *ptype;
    ht         *hash;
    long        resht;
    void *res = NULL;

    if (!sig) return 0;
    ptype = getptype (sig->USER,type);
    if (ptype != NULL) {
        hash = (ht*)ptype->DATA;
        resht = gethtitem (hash,(void*)ctxt);
        if ((resht != EMPTYHT) && (resht != DELETEHT))
            res = (void*)resht;
    }
    return res;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSigByName                                               */
/*                                                                           */
/*****************************************************************************/
losig_list *efg_GetSigByName (lofig,name)
    lofig_list *lofig;
    char       *name;
{
    losig_list *losig;

    for (losig = lofig->LOSIG ; losig ; losig = losig->NEXT) {
        if (efg_getlosigname(losig) == name)
            return losig;
    }
    return NULL;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetHierConByName                                           */
/*                                                                           */
/* Recupere un signal a partir de son nom hierarchique.                      */
/*                                                                           */
/* NB : le nom hier ne comporte pas le nom du top level                      */
/*                                                                           */
/*****************************************************************************/
locon_list *efg_GetHierConByName (lofig,name)
    lofig_list  *lofig;
    char        *name;
{
    char        *left;
    char        *right;
    loins_list  *loins;
    locon_list  *locon;
    lofig_list  *newlofig;

    if ((lofig != NULL) && (name != NULL)) {
        leftunconcatname (namealloc(name),&left,&right);
        if (left == NULL) {
            
            for (locon = lofig->LOINS->LOCON ; locon ; locon = locon->NEXT) {
                if (locon->NAME == right) {
                    return (locon);
                }
            }
        }
        else {
            for (loins = lofig->LOINS ; loins ; loins = loins->NEXT) {
                if (loins->INSNAME == left) {
                    if (strrchr(right,(int)('.')) == NULL) {
                        for (locon = loins->LOCON ; locon ; locon = locon->NEXT) {
                            if (locon->NAME == right)
                                return (locon);
                        }
                    }
                    else {
                        newlofig = efg_get_fig2ins (loins);
                        if (!newlofig)
                          newlofig = getloadedlofig (loins->FIGNAME);
                        return efg_GetHierConByName (newlofig,right);
                    }
                }
            }
        }
    }
    return NULL;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_get_ht_sig                                                 */
/*                                                                           */
/* Return a losig previously added in ht in lofig->USER                      */
/*                                                                           */
/*****************************************************************************/
losig_list *efg_get_ht_sig (lofig_list *fig,char *signame)
{
    ptype_list *ptype;
    losig_list *signal = NULL;
    long res;
    ht *htsig;

    ptype = getptype (fig->USER,EFG_FIG_HTSIG);
    if (ptype != NULL) {
        htsig = (ht*)ptype->DATA;
        res = gethtitem (htsig,signame);
        if (res != EMPTYHT)
            signal= (losig_list*)res;
    }
    return signal;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_set_extra_capa                                             */
/*                                                                           */
/* Add Extra capa on a signal                                                */
/*                                                                           */
/* unit : capaval is in pF                                                   */
/*                                                                           */
/* Return a chain_list of loctc to del after simulation                      */
/*                                                                           */
/*****************************************************************************/
chain_list *efg_set_extra_capa (lofig_list *figext,char *signame, float capaval)
{
    losig_list *losig,*vss;
    chain_list *added_ctclist = NULL;
    ptype_list *ptype;
    
    // retrouve le signal <=> nom

    ptype = getptype (figext->USER,EFG_FIG_HTSIG);
    if (ptype != NULL) {
        losig = efg_get_ht_sig (figext,signame);
        vss   = efg_get_vss_on_destfig ();
        if ( !vss ) {
          ptype = getptype (figext->USER,EFG_SIG_VSS);
          if  ( ptype )
            vss = (losig_list*)ptype->DATA;
        }
        if (((long)losig == EMPTYHT) || (!vss)) return NULL;
        efg_addctc (&added_ctclist,losig,vss,capaval);
    }

    return added_ctclist;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_addctc                                                     */
/*                                                                           */
/* Rajoute une capacite entre un connecteur et le signal de masse            */
/*                                                                           */
/*****************************************************************************/
void efg_addctc (chain_list **added_ctclist,
                 losig_list *sig,
                 losig_list *vss,
                 float capaval)
{
    num_list   *pnode;
    ptype_list *lofigchain;
    locon_list *locon;
    loctc_list *ctc;
    chain_list *chain;
    long        node_vss;

    if (!sig || !vss) return ;
    if (!vss->PRCN) {
        addlorcnet (sig);
        lofigchain = getptype (vss->USER,LOFIGCHAIN);
        for (chain = (chain_list*)lofigchain->DATA; chain; chain=chain->NEXT) {
            locon = (locon_list*)chain->DATA;
            if (locon->TYPE == 'E')
                setloconnode (locon,1);
        }
        node_vss = 1;
    }
    else 
        node_vss = 1;
    if (!sig->PRCN) {
        addlorcnet (sig);
        lofigchain = getptype (sig->USER,LOFIGCHAIN);
        for (chain = (chain_list*)lofigchain->DATA; chain; chain=chain->NEXT) {
            locon = (locon_list*)chain->DATA;
            if (locon->TYPE == 'E') {
                setloconnode (locon,1);
                ctc = addloctc( locon->SIG, 1, vss, node_vss, capaval);
                if (added_ctclist != NULL)
                    *added_ctclist = addchain (*added_ctclist,ctc);
            }
        }
    }
    else {
        lofigchain = getptype (sig->USER,LOFIGCHAIN);
        for (chain = (chain_list*)lofigchain->DATA; chain; chain=chain->NEXT) {
            locon = (locon_list*)chain->DATA;
            if (locon->TYPE == 'E') {
                for (pnode = locon->PNODE ; pnode ; pnode = pnode->NEXT) {
                    ctc = addloctc( locon->SIG, (long)pnode->DATA, vss, node_vss, capaval);
                    if (added_ctclist != NULL)
                        *added_ctclist = addchain (*added_ctclist,ctc);
                }
            }
        }
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_del_extra_capa                                             */
/*                                                                           */
/*****************************************************************************/
void efg_del_extra_capa (chain_list *added_ctclist)
{
    chain_list *chain;

    for (chain = added_ctclist ; chain ; chain=chain->NEXT)
        delloctc ((loctc_list*)chain->DATA);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_is_wire_on_sig                                             */
/*                                                                           */
/*****************************************************************************/
int efg_is_wire_on_sig (losig_list* losig)
{
  int res = 0;

  if (losig->PRCN != NULL && losig->PRCN->PWIRE != NULL)
    res = 1;
  return res;
}

/*****************************************************************************\
 FUNCTION : efg_set_node_in_out_lotrs                                     

\*****************************************************************************/
void efg_set_node_in_out_lotrs (lotrs_list *lotrs)
{
  spisig_list *spisig;
  lofig_list  *lofig;
  lotrs_list  *corresplotrs;
  ptype_list  *ptype;
  
  if (!lotrs) return;
  lofig = EFGCONTEXT->DESTFIG;
  if ((getptype(lotrs->GRID->SIG->USER,EFG_SIG_SET) != NULL)) {
    if (efg_is_wire_on_sig (lotrs->GRID->SIG) == 1 &&
        (spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(lotrs->GRID->SIG))) != NULL) {
      if (lotrs->GRID->PNODE && !efg_is_conname_ext (lofig,lotrs->GRID->SIG,lotrs->GRID->PNODE->DATA) ) {
          ptype = getptype (lotrs->USER,EFG_CORRESP);
          corresplotrs = (lotrs_list*)ptype->DATA;
          efg_SetSpiSigLoconRc (spisig,concatname(corresplotrs->TRNAME,lotrs->GRID->NAME),'O',lotrs);
      }
    }
  }
  if ((getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_SET) != NULL)) {
    if (efg_is_wire_on_sig (lotrs->DRAIN->SIG) == 1 &&
        (spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(lotrs->DRAIN->SIG))) != NULL) {
      if (lotrs->DRAIN->PNODE && !efg_is_conname_ext (lofig,lotrs->DRAIN->SIG,lotrs->DRAIN->PNODE->DATA) ) {
          ptype = getptype (lotrs->USER,EFG_CORRESP);
          corresplotrs = (lotrs_list*)ptype->DATA;
          efg_SetSpiSigLoconRc (spisig,concatname(corresplotrs->TRNAME,lotrs->DRAIN->NAME),'I',lotrs);
      }
    }
  }
  if ((getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_SET) != NULL)) {
    if (efg_is_wire_on_sig (lotrs->SOURCE->SIG) == 1 &&
        (spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(lotrs->SOURCE->SIG))) != NULL) {
      if (lotrs->SOURCE->PNODE && !efg_is_conname_ext (lofig,lotrs->SOURCE->SIG,lotrs->SOURCE->PNODE->DATA) ) {
          ptype = getptype (lotrs->USER,EFG_CORRESP);
          corresplotrs = (lotrs_list*)ptype->DATA;
          efg_SetSpiSigLoconRc (spisig,concatname(corresplotrs->TRNAME,lotrs->SOURCE->NAME),'I',lotrs);
      }
    }
  }
}

/*****************************************************************************\
FUNCTION : efg_add_lotrs_capa_dup

Rajoute une transistor ou capacite
\*****************************************************************************/
void efg_add_lotrs_capa_dup (lofig_list *fig,locon_list *locon)
{
  // le locon est un locon de la figure extraite
  chain_list *chain;
  ptype_list *ptype;
  losig_list *destsig;
  long node_vss=1,node;
  float capaval;
  int capatype = ELP_CAPA_TYPICAL;
  char *signame;
  spisig_list *spisig;
  loctc_list *ctc;

  if ( !fig || !locon || 
       efg_SigIsAlim (locon->SIG) == 'Y' ||
       (EFG_CALC_EQUI_CAPA == EFG_OUT_NONE))
    return;
  if (locon->TYPE == 'T') {
    if (EFG_CALC_EQUI_CAPA == EFG_OUT_TRANSISTOR)
      efg_add_blockedlotrs_dup (fig, locon);
    else {
      // get event on sig
      signame = efg_getlosigname (locon->SIG);
      if ( EFGCONTEXT &&  EFGCONTEXT->SPISIG && 
          (spisig = efg_GetSpiSigByName (EFGCONTEXT->SPISIG, signame)) ) {
        if ( spisig->EVENT == (long)EFG_SIG_SET_FALL ) 
          capatype = ELP_CAPA_DN;
        else if ( spisig->EVENT == (long)EFG_SIG_SET_RISE ) 
          capatype = ELP_CAPA_UP;
        else
          capatype = ELP_CAPA_TYPICAL;
      }
      if ( !getptype (locon->USER, EFG_LOCON_CAPA) ) {
        if ( locon->PNODE ) 
          node = locon->PNODE->DATA;
        else
          node = 1;
        capaval = elpGetCapaFromLocon(locon,capatype,elpTYPICAL);
        destsig = locon->SIG;
        if (!destsig->PRCN) {
            addlorcnet (destsig);
            ctc=addloctc( destsig, node, EFGCONTEXT->VSSDESTFIG, node_vss, capaval);
        }
        else {
          if (destsig->PRCN->PCTC) 
            ctc=addloctc( destsig, node, EFGCONTEXT->VSSDESTFIG, node_vss, capaval);
          else
            ctc=addloctc( destsig, node, EFGCONTEXT->VSSDESTFIG, node_vss, capaval);
        }
        locon->USER = addptype ( locon->USER, EFG_LOCON_CAPA, ctc );
      }
    }
  }
  else if (locon->TYPE == 'I') {
    if ( (ptype = getptype (locon->SIG->USER,LOFIGCHAIN)) )
      for (chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT) {
        locon_list *con = (locon_list *)chain->DATA;
        
        if (con->TYPE == 'T')
          efg_add_blockedlotrs_dup (fig,con);
      }
  }
}


lotrs_list *efg_add_blockedlotrs_dup (lofig_list *fig,locon_list *con)
{
    losig_list *grid,*source,*drain,*bulk,*vss,*vdd;
    lotrs_list *lotrs,*trs;
    char buf[1024];
    ptype_list *ptype;
	alim_list *alim;

    if ( !fig || !con || efg_SigIsAlim (con->SIG) == 'Y')
        return NULL;
    
    trs = (lotrs_list*)con->ROOT; //trs de la figure extraite

   	if ((alim=cns_get_lotrs_multivoltage(trs))!=NULL)
	{
	  vss=efg_try_to_add_alim_to_the_context(NULL, alim->VSSMIN);
	  vdd=efg_try_to_add_alim_to_the_context(NULL, alim->VDDMAX);
	}
	else
	{
      vss = EFGCONTEXT->VSSDESTFIG;
      vdd = EFGCONTEXT->VDDDESTFIG;
	}
    if (trs->BULK==NULL || (bulk=check_already_alim(trs->BULK->SIG, vss, vdd))==NULL) bulk =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
    if ( con == trs->GRID ) { 
      int drainalim=0, sourcealim=0;
      grid = con->SIG;
      if ((drain=check_already_alim(trs->DRAIN->SIG, vss, vdd))==NULL) drain =  vss; //MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
      else drainalim=1;
      if ((source=check_already_alim(trs->SOURCE->SIG, vss, vdd))==NULL) source = vdd; //MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
      else sourcealim=1;
      if (sourcealim!=drainalim)
        {
          if (sourcealim && source==vss) drain=vdd;
          else if (drainalim && drain==vdd) source=vss;
        }
    }
    else if ( con == trs->DRAIN ) {
      drain = con->SIG;
      if ((source=check_already_alim(trs->SOURCE->SIG, vss, vdd))==NULL) source = MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
      if ((grid=check_already_alim(trs->GRID->SIG, vss, vdd))==NULL) grid =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
    }
    else if ( con == trs->SOURCE ) {
      source = con->SIG;
      if ((drain=check_already_alim(trs->DRAIN->SIG, vss, vdd))==NULL) drain =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
      if ((grid=check_already_alim(trs->GRID->SIG, vss, vdd))==NULL) grid =  MLO_IS_TRANSN (trs->TYPE) ? vss : vdd;
    }
    else { // con = bulk
      return NULL;
    }
    sprintf ( buf, "dup%s",trs->TRNAME);
    lotrs = addlotrs (fig,
                      trs->TYPE,
                      trs->X,
                      trs->Y,
                      trs->WIDTH,
                      trs->LENGTH,
                      trs->PS,
                      trs->PD,
                      trs->XS,
                      trs->XD,
                      grid,
                      source,
                      drain,
                      bulk,
                      buf
                     );
    lotrs->FLAGS = EFG_BLOCK_TRS;
    if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE ) {
      sprintf ( buf, "dup%s%d",V_STR_TAB[__EFG_TRS_ALIAS].VALUE,EFG_TRS_INDEX++);
      lotrs->TRNAME = namealloc (buf);
      if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE )
        lotrs->USER = addptype (lotrs->USER,EFG_ALIAS_ORG,trs->TRNAME);
    }
    // champ pour le nom du transitor
    lotrs->MODINDEX = trs->MODINDEX;

    if ( (con == trs->DRAIN) && (trs->DRAIN->PNODE) )
       lotrs->DRAIN->PNODE = dupnumlst(trs->DRAIN->PNODE);
    if ( (con == trs->SOURCE) && (trs->SOURCE->PNODE) )
       lotrs->SOURCE->PNODE = dupnumlst(trs->SOURCE->PNODE);
    if ( (con == trs->GRID) && (trs->GRID->PNODE) )
       lotrs->GRID->PNODE = dupnumlst(trs->GRID->PNODE);
    
    // subcktname of the figure 
    if ( (ptype = getptype ( trs->USER, TRANS_FIGURE )) )
      lotrs->USER = addptype ( lotrs->USER, TRANS_FIGURE, ptype->DATA );
        
    lofigchain (fig);

    // specific instance params
    efg_copy_lotrs_param(trs, lotrs);
/*    // specific instance params
    val = getlotrsparam (trs,MBK_MULU0,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_MULU0, val, NULL);
    val = getlotrsparam (trs,MBK_M,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_M, val, NULL);
    val = getlotrsparam (trs,MBK_DELVT0,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_DELVT0, val , NULL);
    val= getlotrsparam (trs,MBK_SA,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_SA, val, NULL);
    val= getlotrsparam (trs,MBK_SB,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_SB, val, NULL);
    val = getlotrsparam (trs,MBK_SD,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_SD, val, NULL);
    val = getlotrsparam (trs,MBK_NF,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_NF, val, NULL);
    val = getlotrsparam (trs,MBK_NRS,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_NRS, val, NULL);
    val = getlotrsparam (trs,MBK_NRD,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_NRD, val, NULL);
    val = getlotrsparam (trs,MBK_GEOMOD,NULL,&status);
    if ( status == 1 )
      addlotrsparam(lotrs, *MBK_GEOMOD, val, NULL);
*/
    return lotrs;
}

int efg_test_signal_flags(losig_list *ls, long flags)
{
 ptype_list *pt;
 if ((pt=getptype(ls->USER, EFG_SIG_DRV))==NULL) return 0;
 return ((long)pt->DATA & flags)==flags;
}
