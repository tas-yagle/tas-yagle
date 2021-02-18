/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : efg_ext_fig.c                                               */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* Extraction d'une lofig concernant un chemin                              */
/****************************************************************************/
#include "efg.h"

char  EFG_CALC_EQUI_CAPA = EFG_OUT_NONE; /* a passer en param de extract_fig */
int   EFG_TRS_INDEX = 0;
char *EFG_SET_FIG_NAME=NULL;

/*****************************************************************************\
                          function efg_env ()                         

\*****************************************************************************/
void efg_env ( void )
{
  static int done=0;
  char *env;

  EFG_TRS_INDEX = 0;

  switch ( SIM_OUTLOAD ) {
  case SIM_NO_OUTLOAD :         EFG_CALC_EQUI_CAPA = EFG_OUT_NONE;
    break;
  case SIM_DYNAMIC_OUTLOAD :    EFG_CALC_EQUI_CAPA = EFG_OUT_CAPA;
    break;
  case SIM_TRANSISTOR_OUTLOAD : EFG_CALC_EQUI_CAPA = EFG_OUT_TRANSISTOR;
    break;
  default : EFG_CALC_EQUI_CAPA = EFG_OUT_TRANSISTOR;
    break;
  }
}

/*****************************************************************************\
                          function efg_threat_alim ()                         

\*****************************************************************************/
void efg_threat_alim ( lofig_list *lofig, lofig_list *fig_ext,cnsfig_list *cnsfig )
{
  losig_list *losig;

  for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
   {
    if( ( mbk_LosigIsVDD (losig) ) )
      {
       if(getptype(losig->USER,EFG_SIG_VDD) == NULL)
        {
         losig->USER = addptype(losig->USER,EFG_SIG_VDD,NULL) ;
         if(getptype(losig->USER,EFG_SIG_ALIM) == NULL)
           losig->USER = addptype(losig->USER,EFG_SIG_ALIM,NULL) ;
         if (cnsfig) efg_addlosig (fig_ext,losig,1);
        }
      }
    if( ( mbk_LosigIsVSS(losig) ) )
      {
       if(getptype(losig->USER,EFG_SIG_VSS) == NULL)
        {
         losig->USER = addptype(losig->USER,EFG_SIG_VSS,NULL) ;
         if(getptype(losig->USER,EFG_SIG_ALIM) == NULL)
           losig->USER = addptype(losig->USER,EFG_SIG_ALIM,NULL) ;
         if (cnsfig) efg_addlosig (fig_ext,losig,1);
        }
      }
   }
  lofigchain (fig_ext);
}

/*****************************************************************************\
                          function efg_report_extcon ()                         

\*****************************************************************************/

static locon_list *choose_best_node_for_connector(losig_list *sig, int out)
{
  ptype_list *ptype;
  chain_list *chain;
  locon_list *con, *bestcon=NULL;
  
  ptype = getptype ( sig->USER, LOFIGCHAIN );
  for ( chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT ) 
  {
    con = (locon_list *)chain->DATA;
    if ( con->PNODE ) 
    {
      if (bestcon==NULL) bestcon=con;
      if (out)
      {
        if (con->TYPE=='E' || (con->TYPE=='T' && con->NAME==MBK_GRID_NAME)) bestcon=con;
      }
      else
      {
        if (con->TYPE=='E' || (con->TYPE=='T' && (con->NAME==MBK_DRAIN_NAME || con->NAME==MBK_SOURCE_NAME))) bestcon=con;
      }
      if (bestcon->TYPE=='E') break;
    }
  }
  
  return bestcon;
}


void efg_report_extcon ( lofig_list *fig_ext, spisig_list *spisig)
{
  int con2ext = 1;
  int need_lofigchain = 0, haslonode;
  ptype_list *ptype;
  chain_list *chain;
  lotrs_list *lotrs,*trs;
  losig_list *grid;
  locon_list *locon,*con;
  losig_list *sig;
  char       *signame;
  spisig_list *scanspisig;
  long nodenum, rcxnode;

  // report any possible flotting gate in the interface...
  for (locon = fig_ext->LOCON ; locon ; locon=locon->NEXT) 
    if (locon->SIG->TYPE != 'E')
      locon->SIG->TYPE = 'E';

  for (lotrs = (fig_ext)->LOTRS ; lotrs ; lotrs=lotrs->NEXT)  {
    grid = lotrs->GRID->SIG;

    if ( grid->TYPE == 'I' ) {
      ptype = getptype ( grid->USER, LOFIGCHAIN );
      con2ext = 1;
      for ( chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT ) {
        locon = (locon_list *)chain->DATA;
        if ( locon->TYPE == 'T' ) {
          trs = (lotrs_list*)locon->ROOT;
          if ( trs->FLAGS != EFG_BLOCK_TRS && locon != trs->GRID ) {
            con2ext = 0;
            break;
          }
        }
      }
      if ( con2ext ) {
        need_lofigchain = 1;
        locon = addlocon ( (fig_ext), efg_getlosigname (grid), grid, 'I' );
        locon->SIG->TYPE = 'E';
        if ( lotrs->GRID->PNODE )
           locon->PNODE = dupnumlst(lotrs->GRID->PNODE);
      }
    }
  }

  for( scanspisig = spisig ; scanspisig ; scanspisig = scanspisig->NEXT ) {
    if((scanspisig->START || scanspisig->END) && scanspisig->START!=scanspisig->END ) {
      sig = efg_GetDestSigBySpiSig( scanspisig );
      signame = efg_getlosigname ( sig );
      if ( (locon=efg_get_locon ( fig_ext,signame ))==NULL ) {
        sig->TYPE = 'E';
        need_lofigchain = 1;
        locon = addlocon (fig_ext,signame,sig,'O');
        nodenum=-1;
        if (scanspisig->END) rcxnode=scanspisig->OUTCONRC_NODE;
        else rcxnode=scanspisig->INCONRC_NODE;

        efg_checkvalidnode(scanspisig->DESTSIG, rcxnode, &haslonode);

        if (haslonode) nodenum=rcxnode;
        else if ((con=choose_best_node_for_connector(efg_GetSrcSigBySpiSig(scanspisig), scanspisig->START?0:1))!=NULL && con->PNODE!=NULL)
           nodenum=con->PNODE->DATA;

        if (nodenum!=-1)
          locon->PNODE = addnum(NULL, nodenum);
/*
        ptype = getptype ( sig->USER, LOFIGCHAIN );
        for ( chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT ) {
          con = (locon_list *)chain->DATA;
          if ( con->PNODE ) {
             locon->PNODE = (num_list*)dupnumlst((chain_list*)con->PNODE);
             break;
          }
        }
*/
      } else {
        // correct locon pnode
        if (scanspisig->END) rcxnode=scanspisig->OUTCONRC_NODE;
        else rcxnode=scanspisig->INCONRC_NODE;

        efg_checkvalidnode(scanspisig->DESTSIG, rcxnode, &haslonode);

        if (haslonode)
        {
          freenum(locon->PNODE);
          locon->PNODE = addnum(NULL, rcxnode);
        }
      }
    }
  }

  if ( need_lofigchain ) lofigchain (fig_ext);
}

spisig_list *efg_AddLatchLoopFeebackCone(lofig_list *lofig, cnsfig_list *cnsfig, spisig_list *origspisig)
{
  spisig_list *spisig;
  edge_list  *ptinedge ;
  ptype_list dummy, *crade=NULL, *pt;
  cone_list *cone;
  int err;
  chain_list *path;

  if (cnsfig==NULL || lofig==NULL) return origspisig;

  dummy.NEXT=NULL;
  
  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)  
    {
      if ((cone->TYPE & CNS_LATCH) == CNS_LATCH)
      {
        spisig = efg_GetSpiSig ( origspisig, cone->NAME );
        if (spisig!=NULL && !spisig->START)
        {
          for (ptinedge = cone->INCONE; ptinedge; ptinedge = ptinedge->NEXT)
          {
            if ((ptinedge->TYPE & CNS_FEEDBACK) != 0 || (ptinedge->TYPE & CNS_MEMSYM) != 0)
            {
              if (efg_GetSpiSig ( origspisig, ptinedge->UEDGE.CONE->NAME )==NULL)
              {
                for (pt=crade; pt!=NULL && pt->DATA!=ptinedge->UEDGE.CONE->NAME; pt=pt->NEXT); 
                if (pt==NULL) crade=addptype(crade, (long)spisig, ptinedge->UEDGE.CONE);
              }
            }
          }
        }
      }
    }

  for (pt=crade; pt!=NULL; pt=pt->NEXT)
  {
    char *cmd=NULL, cmddir=0;
    spisig=(spisig_list *)pt->TYPE;
    cone=(cone_list *)pt->DATA;
    if ((cone->TYPE & CNS_MEMSYM)!=0) cmd=spisig->LATCHCMD, cmddir=spisig->LATCHCMDDIR;
    path=efg_AddPathEvent(NULL, cone->NAME, spisig->EVENT==(long)EFG_SIG_SET_RISE?EFG_FALL:EFG_RISE,0,spisig->MINDATE+1, -1, -1, -1, cmd, cmddir, spisig->NAME);
    origspisig=efg_BuildSpiSigList (lofig,path,origspisig,0,&err,0);
    spisig->USER=addptype(spisig->USER, EFG_SPISIG_LOOP_SPISIG, origspisig);
    origspisig->USER=addptype(origspisig->USER, EFG_SPISIG_LOOP_SPISIG, spisig);
    efg_FreePathEvent(path);
    origspisig->NUM=-1;
  }
  freeptype(crade);
  return origspisig;
}

/*****************************************************************************\
                          function efg_extract_fig()                         

   drive le spice des signaux du .inf                                        
\*****************************************************************************/
static int checkprechargememsym(cone_list *cone, char *input, cone_list **other)
{
  edge_list *ptinedge;
  int res=0;
  *other=NULL;
  if ((cone->TYPE & CNS_MEMSYM)==0 || V_BOOL_TAB[__CPE_PRECHARGED_MEMSYM].VALUE==0) return 0;
  for (ptinedge = cone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
     if ((ptinedge->TYPE & CNS_EXT) == 0)
     {
       if (input!=NULL && !mbk_casestrcmp(ptinedge->UEDGE.CONE->NAME,input))
         if ((ptinedge->TYPE & CNS_COMMAND)!=0) res=1;
       if ((ptinedge->TYPE & CNS_MEMSYM)!=0) *other=ptinedge->UEDGE.CONE;
     }
   }
  return res;
}

chain_list *efg_getcone_onpath (spisig_list *hdspisig,cnsfig_list *cnsfig )
{
 spisig_list *spis;
 cone_list *cone, *other;
 chain_list *hdchain=NULL;
 gsp_cone_info *ci, *other_ci;
 
 for ( spis = hdspisig ; spis ; spis=spis->NEXT ) {
   if ( (cone = getcone(cnsfig, 0, mbk_devect (spis->critic_NAME,"[","]"))) ) {
     spis->CONE=cone;
     if (!getptype (spis->SRCSIG->USER,EFG_SIG2CONE ))
       spis->SRCSIG->USER = addptype (spis->SRCSIG->USER,EFG_SIG2CONE,cone);

     ci=gsp_create_cone_info(cone);
     ci->Date=spis->NUM;
     ci->MinDate=spis->MINDATE;
     ci->cmd=spis->LATCHCMD;
     ci->cmd_state=spis->LATCHCMDDIR;
     if (checkprechargememsym(cone, spis->PREVIOUS, &other))
       ci->precharged=1;
     if (other!=NULL && (other_ci=gsp_get_cone_info(other))!=NULL)
     {
       if (other_ci->precharged) ci->precharged=1;
       else if (ci->precharged) other_ci->precharged=1;
     }
     if (!efg_test_signal_flags(spis->SRCSIG, EFG_SIG_SET_OUTPATH))
     {
       if (spis->EVENT==(long)EFG_SIG_SET_RISE) ci->BeforeTransitionState=0, ci->AfterTransitionState=1;
       else ci->BeforeTransitionState=1, ci->AfterTransitionState=0;
     }
     hdchain = addchain(hdchain,cone);
   }
   else {
     if (!(spis->START && spis->SRCSIG->TYPE=='E'))
     {
       avt_errmsg(EFG_ERRMSG, "001", AVT_FATAL, spis->critic_NAME);
       exit(0);
     }
     else hdchain = addchain(hdchain,NULL);
   }
 }

 return hdchain;
}


/*****************************************************************************\
                          function efg_extract_fig()                         

   drive le spice des signaux du .inf                                        
\*****************************************************************************/
char *efg_get_fig_name(lofig_list *lf, char *buf)
{
  if (EFG_SET_FIG_NAME==NULL)
    sprintf (buf,"%s_ext",lf->NAME);
  else
    strcpy(buf, EFG_SET_FIG_NAME);
  return buf;
}
void efg_extract_fig (lofig_list  *lofig,
                      lofig_list **fig_ext,
                      cnsfig_list *cnsfig,
                      spisig_list *spisig,
                      chain_list **chaincone,
                      chain_list **chaininstance,
                      chain_list  *loins2drv,       // usr instance to drv
                      chain_list  *lotrs2drv ,     // usr lotrs to drv
                      chain_list  *locon2drv,     // usr external locon 2 drv
                      chain_list  *losig2drv,    // usr losig to drv
                      chain_list  *cone_onpath, // accelerate search of cone
                      chain_list *usrlist,     // pathsig list from user
                      int         markfig     // allow to obtain patterns
                      )
{
 loins_list *loins ;
 char        buf[2048] ;
 char       *newfigname;
 efg_fullsiglist* fullsiglist = NULL;
 chain_list *chain;
 ptype_list *ptype;
 spisig_list *spis;
 int create_local_onpath, err;

 if (!lofig || (!spisig && !usrlist)) return;

 efg_env ();

 if (spisig == NULL)
  {
   spisig = efg_BuildSpiSigList (lofig,usrlist,NULL,1,&err,0);
   spisig = efg_AddLatchLoopFeebackCone(lofig, cnsfig, spisig);
  }

 if(*fig_ext == NULL)
  {
   /* init */
   efg_get_fig_name(lofig, buf);

   newfigname = namealloc (buf);
   *fig_ext = addlofig (newfigname);

   efg_createcontext ();
   efg_setsrcfig2context  ( lofig );
   efg_setdestfig2context ( *fig_ext );
   efg_setspisig2context ( spisig );
 
   if (chaincone != NULL) *chaincone = NULL;
   if (chaininstance != NULL) *chaininstance = NULL;
  }


/*****************************************************************************/
/* positionne les alimentations sur les signaux d'alims                      */
/*****************************************************************************/
  efg_threat_alim ( lofig, *fig_ext, cnsfig );
  if ( !efg_get_vdd_on_destfig () ) {
    avt_errmsg(EFG_ERRMSG, "002", AVT_ERROR);
    //fprintf (stderr,"[EFG ERR] can't get vdd\n");
    efg_FreeMarksOnExtFig (*fig_ext);
    efg_FreeMarksOnFig (lofig);
    efg_freecontext ();
    if (*fig_ext != NULL) dellofig ((*fig_ext)->NAME);
    *fig_ext = NULL;
    return;
  }
  if ( !efg_get_vss_on_destfig () ) {
    avt_errmsg(EFG_ERRMSG, "003", AVT_ERROR);
    //fprintf (stderr,"[EFG ERR] can't get vss\n");
    efg_FreeMarksOnExtFig (*fig_ext);
    efg_FreeMarksOnFig (lofig);
    efg_freecontext ();
    if (*fig_ext != NULL) dellofig ((*fig_ext)->NAME);
    *fig_ext = NULL;
    return;
  }
    
  create_local_onpath = 0 ;
  if ( !cone_onpath ) {
    create_local_onpath = 1 ;
    cone_onpath = efg_getcone_onpath (spisig,cnsfig );
    if (cone_onpath && cone_onpath->DATA==NULL)
      cone_onpath=delchain(cone_onpath,cone_onpath);
  }
  

/*****************************************************************************/
/* ouverture du fichier                                                      */
/*****************************************************************************/
  if (cnsfig != NULL)
  {
    if (spisig!=NULL && spisig->START && spisig->END)
    {
      efg_addlosig(*fig_ext, spisig->SRCSIG, 1);
      spisig->SRCSIG->USER = addptype (spisig->SRCSIG->USER, EFG_SIG_DRV, (void *)1);
    }

    efg_MarkCnsfig (lofig,*fig_ext,cnsfig,chaincone,
                    chaininstance,cone_onpath);
  }
/*****************************************************************************\
 
Construction de la figure extraite a deux niveaux de hierarchie :
 
le top level qui instancie les cellules de base
 
\*****************************************************************************/
  if (lofig->LOINS != NULL && !cnsfig) {
    fullsiglist = efg_buildhierfullsig ( lofig->NAME, 1, NULL, 1);
    efg_buildlofig ( fullsiglist );
    efg_set_path_info ( fullsiglist );
    for (loins = (*fig_ext)->LOINS ; loins ; loins = loins->NEXT)
      efg_set_loins_rc_node (loins);
    if (chaininstance != NULL)
      *chaininstance = efg_get_loins_on_path ();
    efg_freechainfullsig( fullsiglist );
  }

/*****************************************************************************\

Traitement des objets (instances, transistors) a rajouter

\*****************************************************************************/
  for (chain = loins2drv ; chain ; chain=chain->NEXT)
    efg_addloins (*fig_ext,(loins_list*)chain->DATA,0);
  for (chain = lotrs2drv ; chain ; chain=chain->NEXT) {
    efg_addlotrs (*fig_ext,(lotrs_list*)chain->DATA,1);
    efg_set_node_in_out_lotrs ((lotrs_list*)chain->DATA); 
  }
  for (chain = locon2drv ; chain ; chain=chain->NEXT) {
    int exist = 0;
    chain_list *lofigchain=NULL,*chaincon;
    losig_list *sig;
    locon_list *newlocon,
               *loconx,
               *con = (locon_list*)chain->DATA;

    for (loconx = (*fig_ext)->LOCON ; loconx ; loconx=loconx->NEXT) {
      if (efg_getlosigname (loconx->SIG) == efg_getlosigname (con->SIG))
        exist = 1;
    }
    if ( !exist ) {
      sig = efg_addlosig (*fig_ext,con->SIG,1);
      newlocon = addlocon (*fig_ext,con->NAME,sig,con->DIRECTION);
      if (con->SIG->PRCN && con->SIG->PRCN->PWIRE) {
        ptype = getptype (con->SIG->USER,LOFIGCHAIN);
        if ( ptype )
          lofigchain = (chain_list*)ptype->DATA;
        for (chaincon = lofigchain ; chaincon ; chaincon=chaincon->NEXT) {
          locon_list *conx = (locon_list*)chaincon->DATA;
          if (conx->PNODE != NULL) {
            newlocon->PNODE = dupnumlst(conx->PNODE);
            break;
          }
        }
      }
      sig->TYPE = 'E';
      newlocon->TYPE = 'E';
    }
  }
  for (chain = losig2drv ; chain ; chain=chain->NEXT) {
    chain_list *chainc;
    losig_list *sig = (losig_list *)chain->DATA;

    if ( (ptype = getptype (sig->USER,LOFIGCHAIN)) )
      for (chainc = (chain_list *)ptype->DATA ; chainc ; chainc=chainc->NEXT) {
        locon_list *con = (locon_list *)chainc->DATA;
        if (con->TYPE == 'T' || con->TYPE == 'I')
          efg_add_lotrs_capa (*fig_ext,con,1);
      }
  }


  lofigchain (*fig_ext);

  efg_report_extcon ( *fig_ext, spisig);

  if (!markfig) {
    efg_FreeMarksOnExtFig (*fig_ext);
    efg_FreeMarksOnFig (lofig);
    efg_freecontext ();
  }
  if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE )
    efg_update_spisigname (spisig);
  if ( avt_getlibloglevel(LOGEFG) > 0 ) {
    for ( spis = spisig ; spis ; spis=spis->NEXT ) {
      avt_log(LOGEFG,2,"SpiSig %s number %d\n",spis->NAME,spis->NUM);
      if ( spis->INCONRC) 
        avt_log(LOGEFG,2,"       -> INCONRC =  %s\n",spis->INCONRC);
      if ( spis->OUTCONRC ) 
        avt_log(LOGEFG,2,"       -> OUTCONRC =  %s\n",spis->OUTCONRC);
    }
  }

  if( create_local_onpath ) {
    freechain( cone_onpath );
    cone_onpath = NULL ;
  }
  efg_correct_rc_nodes_based_on_rcx(spisig);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_UpdateSigPtype                                             */
/*                                                                           */
/* Marque hierarchiquement les signaux des cellules.                         */
/*                                                                           */
/* reporte egalement le nom le plus significatif du signal : le + haut nivo  */
/*                                                                           */
/*****************************************************************************/
void efg_UpdateSigPtype (lofig,loconname,insname,context,num,slope,
                         sigend)
    lofig_list *lofig;
    char       *loconname;
    char       *insname;
    char       *context;
    int         num;
    char        slope;
    char        sigend;
{
    ptype_list *ptype;
    chain_list *chain;
    loins_list *loins;
    locon_list *locon;
    losig_list *losig;
    char       *name;
    char        buf[1024];

    if (!lofig) return;
    for (locon = lofig->LOCON ; locon ; locon = locon->NEXT) {
        if (locon->NAME == loconname) {
            losig = locon->SIG;
            if (lofig->LOTRS != NULL) {
                if (context != NULL) {
                    sprintf(buf,"%s%c%s",context,SEPAR,insname);
                    context = namealloc(buf);
                    efg_SetSigDrive (losig,num,slope,num==0?'Y':'N',sigend,'N',context,0);
                }
            }
            ptype = getptype(locon->SIG->USER,LOFIGCHAIN);
            if (ptype != NULL) {
                for (chain = (chain_list*)ptype->DATA ; chain ;
                     chain = chain->NEXT) {
                    locon_list *locon = (locon_list*)chain->DATA;
                    if (locon->TYPE == 'I') {
                        loins = (loins_list*)locon->ROOT;
                        if (context != NULL)
                            sprintf(buf,"%s%c%s",context,SEPAR,insname);
                        else
                            sprintf(buf,"%s",loins->INSNAME);
                        name = namealloc(buf);
                        /* pour marquer les sig des instances egalement */
                        efg_SetSigDrive (locon->SIG,num,slope,num==0?'Y':'N',sigend,'N',name,0);
                        efg_UpdateSigPtype(
                                 getloadedlofig(loins->FIGNAME),
                                 locon->NAME,loins->INSNAME,name,
                                 num,slope,sigend);
                    }
                }
            }
        }
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_SetSigDrive                                                */
/*                                                                           */
/* Marque les signaux du .inf                                                */
/*                                                                           */
/*****************************************************************************/
void efg_updateflag(losig_list *ls, int set, long mask)
{
  ptype_list *pt;
  if ((pt=getptype(ls->USER,EFG_SIG_DRV)) != NULL)
  {
    if (set)
      pt->DATA=(void *)(((long)pt->DATA) | mask);
    else
      pt->DATA=(void *)(((long)pt->DATA) & ~mask);
  }
}

void efg_SetSigDrive (losig_list *losig,int num, char slope,char sigstart,char sigend, char sigclk, char *insname, long flags)
{
    long        front;

    if (slope == 'U')
        front = (long)EFG_SIG_SET_RISE;
    else
        front = (long)EFG_SIG_SET_FALL;

    if (getptype(losig->USER,EFG_SIG_DRV) == NULL) {
        losig->USER = addptype(losig->USER,
                               EFG_SIG_DRV,
                               (void *)flags);
    }
    /* garde la compatibilite avec analyse a plat de la figure */
    if (getptype (losig->USER,EFG_SIG_SET) == NULL) {
        losig->USER = addptype (losig->USER,
                                EFG_SIG_SET,
                                (void*)front);
    }

    if ( sigclk =='Y' && getptype(losig->USER,EFG_SIG_CLK) == NULL) {
        losig->USER = addptype (losig->USER,
                                EFG_SIG_CLK,
                                (void*)front);
    }

    /*-------------   Numero du signal dans le .inf ----------------*/
    losig->USER = efg_SetHierPtype (losig->USER,
                                    EFG_SIG_SET_NUM,
                                    (void*)insname,
                                    num,
                                    NULL);

    /*-------------   Info concernant le front du sig --------------*/
    losig->USER = efg_SetHierPtype (losig->USER,
                                    EFG_SIG_SET_HIER,
                                    (void*)insname,
                                    front,
                                    NULL);
    if (sigstart == 'Y') {
        losig->USER = efg_SetHierPtype (losig->USER,
                                        EFG_SIG_BEG_HIER,
                                        (void*)insname,
                                        front,
                                        NULL);
        /* garde la compatibilite avec analyse a plat de la figure */
        if (getptype (losig->USER,EFG_SIG_BEG) == NULL) {
            losig->USER = addptype (losig->USER,
                                    EFG_SIG_BEG,
                                    (void*)front);
        }
    }

    if (sigend == 'Y') {
        losig->USER = efg_SetHierPtype (losig->USER,
                                        EFG_SIG_END_HIER,
                                        (void*)insname,
                                        front,
                                        NULL);
        /* garde la compatibilite avec analyse a plat de la figure */
        if (getptype (losig->USER,EFG_SIG_END) == NULL) {
            losig->USER = addptype (losig->USER,
                                    EFG_SIG_END,
                                    (void*)front);
        }
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_BuildPathSig                                               */
/*                                                                           */
/* Construit la liste des signaux presents sur le chemin a sensibiliser      */
/*                                                                           */
/* <=> equivalent a l'utilisation du .inf                                    */
/*                                                                           */
/* La fonction cree une list_list comme lors du traitement classique de      */
/* tas lors de la lecture du .inf.                                           */
/*                                                                           */
/*****************************************************************************/
list_list *efg_BuildPathSig (pathlist,signame,slope)
    list_list  *pathlist;
    char       *signame;
    char        slope;
{
    list_list  *newlist = NULL;
    list_list  *list;
    ptype_list *user;
    char       *left;
    char       *shortsigname;

    signame = namealloc (signame);
    leftunconcatname (signame,&left,&shortsigname);
    if (slope == 'U') 
        user = addptype (NULL,INF_UP,shortsigname);
    else if (slope == 'D')
        user = addptype (NULL,INF_DOWN,shortsigname);
    else 
        return pathlist;

    newlist = mbkalloc(sizeof(struct list)) ;
    newlist->NEXT = NULL;
    newlist->DATA = shortsigname;
    newlist->TYPE = INF_LL_PATHSIGS;
    newlist->USER = user ;

    if (pathlist != NULL) {
        for (list = pathlist ; list->NEXT ; list = list->NEXT);
            list->NEXT = newlist;
    }
    else
        pathlist = newlist;

    return pathlist;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_FreePathList                                               */
/*                                                                           */
/*****************************************************************************/
void efg_FreePathList (pathlist)
    list_list *pathlist;
{
    list_list *list;

    while (pathlist != NULL) {
        list = pathlist;
        pathlist = pathlist->NEXT;
        freeptype (list->USER);
        mbkfree(list);
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_FreeMarksOnExtFig                                          */
/*                                                                           */
/* Supprime tous les marquages de la figure extraite.                        */
/*                                                                           */
/*****************************************************************************/
void efg_FreeMarksOnExtFig (lofig_list *fig_ext)
{
    efg_DelHierSigPtype (fig_ext);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_FreeMarksOnExtFig                                          */
/*                                                                           */
/* Supprime tous les marquages de la figure extraite.                        */
/*                                                                           */
/*****************************************************************************/
void efg_FreeMarksOnFig (lofig_list *fig)
{
    efg_DelHierSigPtype (fig);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_con_is_on_path                                             */
/*                                                                           */
/* retourne 'Y' si le cone est dans la liste, 'N' sinon                      */
/*                                                                           */
/*****************************************************************************/
char efg_con_is_on_path (chain_list *cone_onpath,cone_list *cone)
{
    chain_list *chain;
    char        res = 'N';

    for (chain = cone_onpath ; chain ; chain = chain->NEXT) {
        if ((cone_list*)chain->DATA == cone) {
            res = 'Y';
            break;
        }
    }
    return res;
}

/*****************************************************************************\
 FUNCTION : efg_add_instance                                           
\*****************************************************************************/
void efg_add_instance (chain_list *chaincone,chain_list **chaininstance)
{
  cone_list  *cone;
  ptype_list *ptype,*sigbeg,*sigend;
  chain_list *chainc,*chain;
  losig_list *losig;
  loins_list *loins;
  locon_list *locon;
  char *context;

  if (!chaincone) return;
  for (chainc = chaincone ; chainc ; chainc=chainc->NEXT) {
    sigbeg = sigend = NULL;
    cone = (cone_list *)chainc->DATA;
    if (!cone || !(losig = efg_getlosigcone (cone))) 
      return;
    sigbeg = getptype (losig->USER,EFG_SIG_BEG);
    sigend = getptype (losig->USER,EFG_SIG_END);
    ptype = getptype (losig->USER,LOFIGCHAIN);
    for (chain = (chain_list*)ptype->DATA ; chain ; chain=chain->NEXT) {
      locon = (locon_list*)chain->DATA;
      if (locon->TYPE == 'I') {
        loins = (loins_list*)locon->ROOT;
        if ((context = efg_get_ins_ctxt (loins))) {
          if (efg_is_loins_to_copy (loins) && efg_loins_is2analyse (context)) {
            efg_addloins (EFGCONTEXT->DESTFIG,loins,0);
            if (chaininstance != NULL && !sigbeg && !sigend)
              *chaininstance = addchain (*chaininstance,loins);
          }
        }
      }
    }
  }
}

/*****************************************************************************\
FUNC : efg_report_rc_lotrs
\*****************************************************************************/
void efg_report_rc_lotrs ( lofig_list *fig_ext,locon_list *con )
{
  ptype_list *ptype;
  spisig_list *spisig;
  lotrs_list *corresplotrs,*lotrs;

  lotrs = (lotrs_list*)con->ROOT;
  ptype = getptype (lotrs->USER,EFG_CORRESP);
  if ( !ptype ) return;
  corresplotrs = (lotrs_list*)ptype->DATA;

  if ( con == lotrs->GRID ) {
    if (getptype (lotrs->GRID->SIG->USER,EFG_SIG_DRV) && 
        efg_is_wire_on_sig (lotrs->GRID->SIG) == 1 &&
        (spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(lotrs->GRID->SIG))) != NULL) {
      if (lotrs->GRID->PNODE && 
          !efg_is_conname_ext (fig_ext,lotrs->GRID->SIG,lotrs->GRID->PNODE->DATA) ) {
          efg_SetSpiSigLoconRc (spisig, concatname(corresplotrs->TRNAME,lotrs->GRID->NAME), 'O',lotrs);
      }
      // to avoid shift result if incorrect rc network is detected
      if (lotrs->GRID->PNODE && !efg_GetSpiSigLoconRc(spisig,'O') ) 
        efg_SetSpiSigLoconRc (spisig, concatname(corresplotrs->TRNAME,lotrs->GRID->NAME),'O',lotrs);
    }

  }
  else if ( con == lotrs->DRAIN) {
    if (getptype (lotrs->DRAIN->SIG->USER,EFG_SIG_DRV) && 
        efg_is_wire_on_sig (lotrs->DRAIN->SIG) == 1 &&
        (spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(lotrs->DRAIN->SIG))) != NULL) {
      if (lotrs->DRAIN->PNODE && 
          !efg_is_conname_ext (fig_ext,lotrs->DRAIN->SIG,lotrs->DRAIN->PNODE->DATA) ) {
          efg_SetSpiSigLoconRc (spisig,  concatname(corresplotrs->TRNAME,lotrs->DRAIN->NAME),'I',lotrs);
          if ( !efg_GetSpiSigLoconRc(spisig,'I') ) 
            efg_SetSpiSigLoconRc (spisig, concatname(corresplotrs->TRNAME,lotrs->DRAIN->NAME),'I',lotrs);
      }
      // to avoid shift result if incorrect rc network is detected
      if (lotrs->DRAIN->PNODE && !efg_GetSpiSigLoconRc(spisig,'I') ) 
        efg_SetSpiSigLoconRc (spisig, concatname(corresplotrs->TRNAME,lotrs->DRAIN->NAME),'I',lotrs);
    }
  }
  else {
    if (getptype (lotrs->SOURCE->SIG->USER,EFG_SIG_DRV) && 
        efg_is_wire_on_sig (lotrs->SOURCE->SIG) == 1 &&
        (spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(lotrs->SOURCE->SIG))) != NULL) {
      if (lotrs->SOURCE->PNODE && 
          !efg_is_conname_ext (fig_ext,lotrs->SOURCE->SIG,lotrs->SOURCE->PNODE->DATA) ) {
          efg_SetSpiSigLoconRc (spisig,  concatname(corresplotrs->TRNAME,lotrs->SOURCE->NAME),'I',lotrs);
      }
      // to avoid shift result if incorrect rc network is detected
      if ( lotrs->SOURCE->PNODE && !efg_GetSpiSigLoconRc(spisig,'I') ) 
        efg_SetSpiSigLoconRc (spisig, concatname(corresplotrs->TRNAME,lotrs->SOURCE->NAME),'I',lotrs);
    }
  }
}

/*****************************************************************************\
                                                                          
 FUNCTION : efg_addlotrs_conefct

 3 possiblilities :

 - add a fonctionnal trs
 - add a non fonctionnal trs
 - add an equivalent capa
                                                                        
\*****************************************************************************/
void efg_addlotrs_conefct (lofig_list *fig_ext, lotrs_list *lotrs, 
                           locon_list *con, int index, int cone_is_fct)
{
 if ( cone_is_fct )
   efg_addlotrs (fig_ext,lotrs,index);
 else
   efg_add_lotrs_capa (fig_ext,con,index);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_MarkCnsfig                                                 */
/*                                                                           */
/* Analyse la cnsfig par rapport au chemin a sensibilier                     */
/*                                                                           */
/*****************************************************************************/

#define TEMPMARKEFGPARALLEL 0xfab80916

static void efg_mark_parallel_path(chain_list *conelist)
{
  cone_list *cn, *cn0, *ptinputcone;
  edge_list *ptinedge;
  int tog=0;
  losig_list *ls;
  chain_list *res;
  while (conelist!=NULL)
  {
   cn=(cone_list *)conelist->DATA;
  
   for (ptinedge = cn->INCONE; ptinedge; ptinedge = ptinedge->NEXT) 
     {
       if ((ptinedge->TYPE & CNS_VDD) != 0) continue;
       if ((ptinedge->TYPE & CNS_VSS) != 0) continue;
       if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
       if ((ptinedge->TYPE & CNS_EXT) != 0) continue;

       ptinputcone = ptinedge->UEDGE.CONE;
       ls = efg_getlosigcone(ptinputcone) ;
       res=NULL;
       if (getptype(ls->USER,EFG_SIG_SET)==NULL && (ls=gsp_FindSimpleCorrelationForToBeStuckConnectors(ptinputcone, 0, 10, &tog,1,&res))!=NULL)
       {
         res=addchain(res, ptinputcone);
         while (res!=NULL)
         {
           cn0=(cone_list *)res->DATA;
           if (getptype(cn0->USER, TEMPMARKEFGPARALLEL)==NULL)
           {
             avt_log(LOGEFG,2,"Parallel path ('%s' -> '%s') : cone '%s' added to spicedeck\n",getsigname(ls), cn->NAME, cn0->NAME);
             cn0->USER=addptype(cn0->USER, TEMPMARKEFGPARALLEL, 0);
           }
           res=delchain(res,res);
         }
       }
     }
   conelist=conelist->NEXT;
  }
}

void efg_MarkCnsfig (lofig_list *lofig,lofig_list *fig_ext,cnsfig_list *cnsfig,
                     chain_list **chaincone, chain_list **chaininstance,
                     chain_list *cone_onpath)
{
 lotrs_list *lotrs, *swlotrs ;
 locon_list *locon ;
 losig_list *losig, *srcsig,*nxtsig ;
 cone_list *cone ;
 branch_list *branch[3] ;
 link_list *link ;
 int i, cone_is_fct ;
 char flag ;
 spisig_list *spisig;
 ptype_list *ptype, *pt;
 chain_list *chain;
 chain_list *headlotrs, *stop;
 locon_list *othercon;

 if((cnsfig != NULL) && (fig_ext != NULL))
  {
   for ( locon = lofig->LOCON ; locon ; locon = locon->NEXT) 
    {
     if ( getptype (locon->SIG->USER,EFG_SIG_DRV) )
       efg_addlocon (fig_ext,locon,1);
    }

   efg_mark_parallel_path(cone_onpath);
   
   // 1ere passe pour rajoute EFG_SIG_DRV pour tous les signaux des cones sur chemin
   for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)  
    {
     losig = efg_getlosigcone(cone) ;
     if (getptype(cone->USER, TEMPMARKEFGPARALLEL)==NULL && (cone_onpath != NULL ) && 
         (efg_con_is_on_path (cone_onpath,cone) == 'N'))
         continue;
     cone_is_fct = 0;
     if (getptype(cone->USER, TEMPMARKEFGPARALLEL)!=NULL)
       cone_is_fct=1;
     else if ( getptype(losig->USER,EFG_SIG_SET))
       cone_is_fct = efg_cone_is_fonctionnal (cone,0);
     if (cone_is_fct)
     {
       branch[0] = cone->BRVDD ;
       branch[1] = cone->BRVSS ;
       branch[2] = cone->BREXT ;

       for(i = 0 ; i < 3 ; i++)
        {
         for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
          {
           for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT)
            {
             if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
              {
               lotrs = link->ULINK.LOTRS ;
               headlotrs = addchain( NULL, lotrs );
               ptype = getptype( lotrs->USER, CNS_SWITCH ) ;
               if( ptype ) {
                 headlotrs = addchain( headlotrs, ptype->DATA );
               }
               for( chain = headlotrs ; chain ; chain = chain->NEXT ) {
                 lotrs = (lotrs_list*)chain->DATA ;
                 if((getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_DRV) == NULL) &&
                    (getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                   lotrs->SOURCE->SIG->USER = addptype (lotrs->SOURCE->SIG->USER,
                                                        EFG_SIG_DRV,
                                                        NULL);
                   avt_log(LOGEFG,2,"Sig %s marked as EFG_SIG_DRV\n",efg_getlosigname(lotrs->SOURCE->SIG));
                 }
                 if((getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_DRV) == NULL) &&
                    (getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                   lotrs->DRAIN->SIG->USER = addptype (lotrs->DRAIN->SIG->USER,
                                                        EFG_SIG_DRV,
                                                        NULL);
                   avt_log(LOGEFG,2,"Sig %s marked as EFG_SIG_DRV\n",efg_getlosigname(lotrs->DRAIN->SIG));
                 }
                 if((getptype(lotrs->GRID->SIG->USER,EFG_SIG_DRV) == NULL) &&
                    (getptype(lotrs->GRID->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                   lotrs->GRID->SIG->USER = addptype (lotrs->GRID->SIG->USER,
                                                        EFG_SIG_DRV,
                                                        NULL);
                   avt_log(LOGEFG,2,"Sig %s marked as EFG_SIG_DRV\n",efg_getlosigname(lotrs->GRID->SIG));
                 }
               }
               freechain(headlotrs);
              }
            }
          }
        }
     }
    }
   stop=cone_onpath;
     // rajoute les transistors dans la figure extraite
   for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)  
    {
     losig = efg_getlosigcone(cone) ;
     if ( losig && !getptype (losig->USER,EFG_SIG2CONE ))
       losig->USER = addptype (losig->USER,EFG_SIG2CONE,cone);
     if (getptype(cone->USER, TEMPMARKEFGPARALLEL)==NULL && (cone_onpath != NULL ) && 
         (efg_con_is_on_path (cone_onpath,cone) == 'N'))
         continue;
     flag = 'N' ;
     cone_is_fct = 0;
     if (getptype(cone->USER, TEMPMARKEFGPARALLEL)!=NULL)
      cone_is_fct =1;
     else if ( getptype(losig->USER,EFG_SIG_SET) )
       cone_is_fct = efg_cone_is_fonctionnal (cone,0);
     branch[0] = cone->BRVDD ;
     branch[1] = cone->BRVSS ;
     branch[2] = cone->BREXT ;

     spisig = efg_GetSpiSig ( EFGCONTEXT->SPISIG, cone->NAME );

     for(i = 0 ; i < 3 ; i++)
      {
       for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
        {
         for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT)
          {
           if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
            {
             lotrs = link->ULINK.LOTRS ;
             headlotrs = addchain( NULL, lotrs );
             swlotrs=NULL;
             ptype = getptype( lotrs->USER, CNS_SWITCH ) ;
             if( ptype ) {
               swlotrs=(lotrs_list *)ptype->DATA;
               headlotrs = addchain( headlotrs, swlotrs);
             }
             
             if ((ptype=getptype(lotrs->USER, MBK_TRANS_PARALLEL))!=NULL) {
               for (chain=(chain_list *)ptype->DATA; chain!=NULL; chain=chain->NEXT)
                 if (chain->DATA!=lotrs) headlotrs = addchain( headlotrs, chain->DATA);
             }
             if (swlotrs!=NULL && (ptype=getptype(swlotrs->USER, MBK_TRANS_PARALLEL))!=NULL) {
               for (chain=(chain_list *)ptype->DATA; chain!=NULL; chain=chain->NEXT)
                 if (chain->DATA!=swlotrs) headlotrs = addchain( headlotrs, chain->DATA);
             }

             for( chain = headlotrs ; chain ; chain = chain->NEXT ) {
               lotrs = (lotrs_list*)chain->DATA ;

               if ( cone_is_fct ) efg_addlotrs (fig_ext,lotrs,1);
               if((getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_DRV) != NULL) &&
                  (getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                 flag = 'Y' ;
                 //efg_addlotrs_conefct (fig_ext, lotrs, lotrs->DRAIN, 1, cone_is_fct);
                 efg_add_lotrs_capa (fig_ext,lotrs->DRAIN,1);
               }
               if((getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_DRV) != NULL) &&
                  (getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                 //efg_addlotrs_conefct (fig_ext, lotrs, lotrs->SOURCE, 1, cone_is_fct);
                 efg_add_lotrs_capa (fig_ext,lotrs->SOURCE,1);
                 flag = 'Y' ;
               }
               if((getptype(lotrs->GRID->SIG->USER,EFG_SIG_DRV) != NULL) &&
                  (getptype(lotrs->GRID->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                 flag = 'Y' ;
                 //efg_addlotrs_conefct (fig_ext, lotrs, lotrs->GRID, 1, cone_is_fct);
                 efg_add_lotrs_capa (fig_ext,lotrs->GRID,1);
               }
             }
             freechain( headlotrs );
            }
           else
            {
             locon = link->ULINK.LOCON ;
             if((getptype(locon->SIG->USER,EFG_SIG_DRV) != NULL) &&
                (getptype(locon->SIG->USER,EFG_SIG_ALIM) == NULL))
               flag = 'Y' ;
            }
          }
        }
      }
     if(flag == 'Y')
      {
       if (getptype(cone->USER, TEMPMARKEFGPARALLEL)!=NULL)
       {
          EFGCONTEXT->ALLADDEDCONES=addchain(EFGCONTEXT->ALLADDEDCONES, cone);
          avt_log(LOGEFG,2,"Cone %s ADDED (//)\n",cone->NAME);
          cone_onpath=addchain(cone_onpath, cone);
       }
       else if (getptype(losig->USER,EFG_SIG_SET) != NULL) 
        {
         if (spisig!=NULL && spisig->NUM==-1) {
            *chaincone = addchain(*chaincone,cone) ;
            avt_log(LOGEFG,2,"Cone %s ADDED (out of path feedback loop)\n",cone->NAME);
            EFGCONTEXT->ALLADDEDCONES=addchain(EFGCONTEXT->ALLADDEDCONES, cone);
         }
		 else if (chaincone != NULL && !getptype (losig->USER,EFG_SIG_BEG)  ) 
          {
           if ( cone_is_fct ) 
            {
             *chaincone = addchain(*chaincone,cone) ;
             avt_log(LOGEFG,2,"Cone %s ADDED\n",cone->NAME);
             EFGCONTEXT->ALLADDEDCONES=addchain(EFGCONTEXT->ALLADDEDCONES, cone);
            }
		  }
        }
       if(getptype(losig->USER,EFG_SIG_CONE) == NULL)
         losig->USER = addptype(losig->USER,EFG_SIG_CONE,NULL) ;
      }

     if ( getptype (cone->USER, EFG_CONE_FCT) )
       cone->USER = delptype ( cone->USER, EFG_CONE_FCT );
    }

   // report RC in (Drain/Source) and RC out (Gate) on Spisig
   for ( chain = cone_onpath ; chain ; chain=chain->NEXT )
    {
     cone = (cone_list *)chain->DATA;

     branch[0] = cone->BRVDD ;
     branch[1] = cone->BRVSS ;
     branch[2] = cone->BREXT ;

     for(i = 0 ; i < 3 ; i++)
      {
       for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
        {
         for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT)
          {
           if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
            {
             lotrs = link->ULINK.LOTRS ;
             if((getptype(lotrs->GRID->SIG->USER,EFG_SIG_SET) != NULL) &&
                (getptype(lotrs->GRID->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                efg_report_rc_lotrs ( fig_ext,lotrs->GRID);
             }
             if((getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_SET) != NULL) &&
                (getptype(lotrs->DRAIN->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                efg_report_rc_lotrs ( fig_ext,lotrs->DRAIN);
             }
             if((getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_SET) != NULL) &&
                (getptype(lotrs->SOURCE->SIG->USER,EFG_SIG_ALIM) == NULL)) {
                efg_report_rc_lotrs ( fig_ext,lotrs->SOURCE);
             }
            }
          }
        }
      }
    }

   while (cone_onpath!=stop) cone_onpath=delchain(cone_onpath, cone_onpath);

    // zinaps: ptype non liberes correctement a cause de la recursivite
    // dans la fonction qui les positionnent o_O
    for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)  
    {
      if ( getptype (cone->USER, EFG_CONE_FCT) )
       cone->USER = delptype ( cone->USER, EFG_CONE_FCT );
    }
    // 
   if (chaincone != NULL && *chaincone != NULL && lofig->LOINS!=NULL)
    {
     efg_set_ins_ctxt (lofig,lofig->NAME);
     efg_add_instance (*chaincone,chaininstance);
    }
 
   lofigchain( fig_ext );
   for (losig = fig_ext->LOSIG ; losig ; losig=nxtsig) 
    {
     locon=NULL;
     nxtsig =losig->NEXT;
     srcsig = efg_get_org_sig ( losig );
     if (srcsig!=NULL && (ptype=getptype(srcsig->USER, EFG_SIG_DRV))!=NULL && ptype->DATA==NULL)
     {
       ptype = getptype ( losig->USER, LOFIGCHAIN);
       if ( !ptype->DATA || !((chain_list*)ptype->DATA)->NEXT ) {
         if ((chain = (chain_list*)ptype->DATA))
           locon = (locon_list*)(chain->DATA);
         if ( losig != efg_get_vss_on_destfig () && losig != efg_get_vdd_on_destfig () )
         if ( !locon || (locon && locon->TYPE != 'T' )) {
           avt_log(LOGEFG,2,"Sig %s deleted...\n",efg_getlosigname(losig));
           efg_dellosig( fig_ext, losig );
           if ( locon ) {
             avt_log(LOGEFG,2,"locon %s deleted...\n",locon->NAME);
             dellocon (fig_ext,locon->NAME);
           }
           continue;
         }
       }
     }

     if ( !srcsig ) continue;
     if ( efg_SigIsAlim (srcsig) == 'Y' ) continue;
     ptype = getptype ( srcsig->USER, LOFIGCHAIN);
    
     for ( chain = (chain_list *)ptype->DATA ; chain ; chain=chain->NEXT )
       efg_add_lotrs_capa (fig_ext,(locon_list*)chain->DATA,1);
    }

    efg_rebuild_ctc( fig_ext );
  }
 for ( locon = fig_ext->LOCON ; locon ; locon=othercon) {
   othercon=locon->NEXT;

   ptype = getptype ( locon->SIG->USER, LOFIGCHAIN);
   if ( locon->SIG != efg_get_vss_on_destfig () && locon->SIG != efg_get_vdd_on_destfig () )
   if ( !ptype->DATA || !((chain_list*)ptype->DATA)->NEXT ) {
     avt_log(LOGEFG,2,"Sig %s and Con %s deleted...\n",
                                efg_getlosigname(locon->SIG),locon->NAME);
     efg_dellosig (fig_ext,locon->SIG);
     dellocon (fig_ext,locon->NAME);
   }
 }
}

void efg_dellosig( lofig_list *lofig, losig_list *losig )
{
  ptype_list *ptype ;
  losig_list *sigsrc ;

  ptype = getptype( losig->USER,EFG_CORRESP);
  if( ptype ) {
    sigsrc = (losig_list*)ptype->DATA ;
    losig->USER = delptype( losig->USER,EFG_CORRESP);
    ptype = getptype( sigsrc->USER, EFG_CORRESP);
    if( ptype ) {
      sigsrc->USER = delptype( sigsrc->USER,EFG_CORRESP);
    }
  }
  dellosig( lofig, losig->INDEX );
}
