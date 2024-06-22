/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : efg_spisig.c                                                */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
#include "efg.h"


char *efg_getlosigname(losig_list *ls)
{
  ptype_list *pt;
  chain_list *cl;
  locon_list *lc;
  if (ls->TYPE!='E' || (ls->NAMECHAIN!=NULL && ls->NAMECHAIN->NEXT==NULL)) return getsigname(ls);
  if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL)
  {
    for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='E') break;
    }
    if (cl!=NULL) return lc->NAME;
  }
  return getsigname(ls);
}

/*****************************************************************************\
 FUNCTION : efg_update_spisigname
\*****************************************************************************/
void efg_update_spisigname ( spisig_list *spisiglist )
{
  spisig_list *spisig;

  for ( spisig = spisiglist ; spisig ; spisig=spisig->NEXT ) 
    spisig->NAME = efg_getlosigname (spisig->DESTSIG);
}

/*****************************************************************************\
 FUNCTION : efg_GetSpiSigByName                                            
\*****************************************************************************/
spisig_list *efg_GetSpiSigByName (spisig_list *head,char *name)
{
    spisig_list *ptspisig = NULL ;
    
    for (ptspisig = head ; ptspisig ; ptspisig = ptspisig->NEXT) {
        //if (efg_GetSpiSigName (ptspisig) == name)
        if (efg_getlosigname(ptspisig->SRCSIG) == name)
            break;
    }
    return ptspisig;
}

/*****************************************************************************\
 FUNCTION : efg_UpdateSpiSigName                                            

 Met le nom original dans un ptype pour pouvoir le retrouver avec les noms
 originaux
\*****************************************************************************/
void efg_UpdateSpiSigName (spisig_list *spisig, char *name)
{
  if ( spisig ) {
    if ((getptype (spisig->USER,EFG_SPISIG_ORG_NAME)))
      spisig->USER = delptype (spisig->USER,EFG_SPISIG_ORG_NAME);
    spisig->USER = addptype (spisig->USER,EFG_SPISIG_ORG_NAME,spisig->NAME);
    spisig->NAME = name;
  }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_PrintSpiSigList                                             */
/*                                                                           */
/*****************************************************************************/
void efg_PrintSpiSigList (head)
  spisig_list *head;
{
  spisig_list *ptspisig;

  fprintf(stdout,"\n");
  for (ptspisig = head ; ptspisig ; ptspisig = ptspisig->NEXT) {
      char *slope;

      fprintf(stdout,"---------------------------------\n");
      fprintf(stdout,"Sig name     : %s\n",ptspisig->NAME);
      fprintf(stdout," |- numsig   : %d\n",ptspisig->NUM);
      if ((ptspisig->EVENT & EFG_SIG_SET_MASK) == (long)EFG_SIG_SET_RISE)
          slope = namealloc ("RISING"); 
      else if ((ptspisig->EVENT  & EFG_SIG_SET_MASK) == (long)EFG_SIG_SET_FALL)
          slope = namealloc ("FALLING"); 
      else
          slope = namealloc ("UNKNOWN"); 
      fprintf(stdout," |- slope    : %s\n",slope);
      /*
      fprintf(stdout," |- tpckup   : %ld\n",ptspisig->TPCKUP);
      fprintf(stdout," |- tpckdn   : %ld\n",ptspisig->TPCKDN);
      fprintf(stdout," |- tckstab  : %ld\n",ptspisig->TCKSTAB);
      */
      fprintf(stdout," |- addcapa  : %f\n",ptspisig->ADDCAPA);
      fprintf(stdout," |- initvolt : %f\n",ptspisig->INITVOLT);
  }
  fprintf(stdout,"\n");
  fflush (stdout);
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSpiSigName                                              */
/*                                                                           */
/*****************************************************************************/
char *efg_GetSpiSigName (spisig_list *spisig)
{
    if (spisig != NULL)
        return spisig->NAME;
    else return NULL;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_IsWireOnSpiSig                                             */
/*                                                                           */
/*****************************************************************************/
int efg_IsWireOnSpiSig (spisig_list *spisig)
{
  int res=0;

  if (spisig) {
    if ( spisig->INCONRC || spisig->OUTCONRC || spisig->HASRCLINE)
      res = 1;
  }
  return res;
  /*
  if (spisig && (spisig->LOCONRC != NULL ))
    return 1;
  else
    return 0;
    */
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSpiSigEvent                                             */
/*                                                                           */
/*****************************************************************************/
long efg_GetSpiSigEvent (spisig_list *spisig)
{
    if (spisig != NULL)
        return spisig->EVENT;
    else {
        avt_errmsg(EFG_ERRMSG, "005", AVT_ERROR);
        //fprintf (stderr,"[EFG ERR] efg_GetSpiSigEvent : spisig is NULL ! \n");
        return 0;
    }
}

/*****************************************************************************\
                                                                           
 FUNCTION : efg_SetSpiSigLoconRc                                          
                                                                           
 Positionne le nom du locon relation au reseau RC du signal                 
                                                                           
 TMP : le champ LOCONRC contient 2 elmts au max                             
                                                                           
\*****************************************************************************/
void efg_SetSpiSigLoconRc (spisig_list *spisig,char *name, char dir,lotrs_list *mytrs)
  // dir is 'I' for In or 'O' for Out
{
  int goodtrs=0;
  if (!spisig ) return;

  if ( dir == 'I' ) {
    if (mytrs==NULL) goodtrs=1;
    else if (MLO_IS_TRANSN (mytrs->TYPE) && spisig->EVENT==(long)EFG_SIG_SET_FALL) goodtrs=1;
    else if (MLO_IS_TRANSP (mytrs->TYPE) && spisig->EVENT==(long)EFG_SIG_SET_RISE) goodtrs=1;
    if (!spisig->INCONRC || (spisig->GOODRC & EFG_GOODRC_INPUT)==0)
      if (spisig->INCONRC==NULL || strcmp(name,spisig->INCONRC)<0)
      {
        spisig->INCONRC = name;
        if (goodtrs) spisig->GOODRC|=EFG_GOODRC_INPUT;
      }
  }
  else {
    if (mytrs==NULL) goodtrs=1;
    else if (MLO_IS_TRANSN (mytrs->TYPE) && spisig->EVENT==(long)EFG_SIG_SET_RISE) goodtrs=1;
    else if (MLO_IS_TRANSP (mytrs->TYPE) && spisig->EVENT==(long)EFG_SIG_SET_FALL) goodtrs=1;
    if (!spisig->OUTCONRC || (spisig->GOODRC & EFG_GOODRC_OUTPUT)==0)
      if (spisig->OUTCONRC==NULL || strcmp(name,spisig->OUTCONRC)<0)
      {
        spisig->OUTCONRC = name;
        if (goodtrs) spisig->GOODRC|=EFG_GOODRC_OUTPUT;
      }
  }
}

/*****************************************************************************\
                                                                           
 FUNCTION : efg_GetSpiSigLoconRc                                           
                                                                         
\*****************************************************************************/
char *efg_GetSpiSigLoconRc (spisig_list *spisig, char dir)
{
    if (!spisig ) return NULL;
    if ( dir == 'I' )
      return spisig->INCONRC;
    else
      return spisig->OUTCONRC;
    //return spisig->LOCONRC;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_SetDestSig2SpiSig                                          */
/*                                                                           */
/* Positionne le signal extrait dans la structure des spisig                 */
/*                                                                           */
/*****************************************************************************/
void efg_SetDestSig2SpiSig (spisig_list *spisig,losig_list *destsig)
{
    if (!spisig || !destsig) return;
    spisig->DESTSIG = destsig;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetDestSigBySpiSig                                         */
/*                                                                           */
/* Recupere le signal extrait par un spisig                                  */
/*                                                                           */
/*****************************************************************************/
losig_list *efg_GetDestSigBySpiSig (spisig_list *spisig)
{
    if (!spisig) return NULL;
    return spisig->DESTSIG;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSrcSigBySpiSig                                          */
/*                                                                           */
/* Recupere le signal extrait par un spisig                                  */
/*                                                                           */
/*****************************************************************************/
losig_list *efg_GetSrcSigBySpiSig (spisig_list *spisig)
{
    if (!spisig) return NULL;
    return spisig->SRCSIG;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetDestSigBySpiSig                                         */
/*                                                                           */
/* Recupere le signal extrait par un spisig                                  */
/*                                                                           */
/*****************************************************************************/
int efg_GetSpiSigNum (spisig_list *spisig)
{
    if (!spisig) return -1;
    return spisig->NUM;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_AddSpiSig                                                  */
/*                                                                           */
/* Rajoute un nouveau signal contenant les infos pour la simulation spice.   */
/*                                                                           */
/* le nom ne doit pas contenir le nom du top level                           */
/*                                                                           */
/*****************************************************************************/
spisig_list *efg_AddSpiSig (spisig_list *head,losig_list *srcsig,char *name,int numsig,long slope, int mindate)
{
    spisig_list *ptspisig;

    ptspisig = (spisig_list*)mbkalloc(sizeof(spisig_list));

    ptspisig->NEXT     = head;
    ptspisig->NAME     = namealloc(name);
    ptspisig->critic_NAME = NULL;
    ptspisig->SRCSIG   = srcsig;
    ptspisig->DESTSIG  = NULL;
    //ptspisig->LOCONRC  = NULL;
    ptspisig->INCONRC  = NULL;
    ptspisig->OUTCONRC  = NULL;
    ptspisig->INCONRC_NODE  = -1;
    ptspisig->OUTCONRC_NODE  = -1;
    ptspisig->GOODRC  = 0;
    ptspisig->HASRCLINE  = 0;
    ptspisig->NUM      = numsig;
    if (mindate==-1)
      ptspisig->MINDATE  = numsig;
    else
      ptspisig->MINDATE  = mindate;
    ptspisig->EVENT    = slope;
    /*
    ptspisig->TPCKUP   = -1;
    ptspisig->TPCKDN   = -1;
    ptspisig->TCKSTAB  = -1;
    */
    ptspisig->ADDCAPA  = -1.0;
    ptspisig->ADDIC    = 0;
    ptspisig->INITVOLT = -1000.0;
    ptspisig->VTHLOW   = -1.0;
    ptspisig->VTHHIGH  = -1.0;
    ptspisig->VTH      = -1.0;
    ptspisig->VTHHZ    = -1.0;
    ptspisig->USER     = NULL;
    ptspisig->CONE     = NULL;
    ptspisig->PATHNUM  = 0;
    ptspisig->PREVIOUS = NULL;
    ptspisig->START = getptype( srcsig->USER, EFG_SIG_BEG ) ? 1 : 0 ;
    ptspisig->END   = getptype( srcsig->USER, EFG_SIG_END ) ? 1 : 0 ;
    ptspisig->CLK   = getptype( srcsig->USER, EFG_SIG_CLK ) ? 1 : 0 ;

    head = ptspisig;

    return ptspisig;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_FreeSpiSigList                                             */
/*                                                                           */
/*****************************************************************************/
void efg_FreeSpiSigList (head)
    spisig_list *head;
{
    spisig_list *ptspisig;

    while (head != NULL) {
        ptspisig = head;
        head = head->NEXT;
        if (ptspisig->USER) 
          freeptype (ptspisig->USER);
        mbkfree(ptspisig);
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSpiSig                                                  */
/*                                                                           */
/* le nom doit contenir le nom du top level                                  */
/*                                                                           */
/*****************************************************************************/
spisig_list *efg_GetSpiSig (head,name)
    spisig_list *head;
    char        *name;
{
    spisig_list *ptspisig = NULL ;
    ptype_list  *origname; // nom original du spisig
    
    name = namealloc (name);

    for (ptspisig = head ; ptspisig ; ptspisig = ptspisig->NEXT) {
        if (ptspisig->NAME == name)
            break;
        if (efg_getlosigname(ptspisig->SRCSIG) == name)
            break;
        if ((origname = getptype (ptspisig->USER,EFG_SPISIG_ORG_NAME) )) {
          if ((char*)origname->DATA == name) 
            break;
        }
    }
    return ptspisig;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSpiSig                                                  */
/*                                                                           */
/*****************************************************************************/
spisig_list *efg_GetSpiSigByNum (spisig_list *head,int num)
{
    spisig_list *ptspisig = NULL ;
    
    for (ptspisig = head ; ptspisig ; ptspisig = ptspisig->NEXT) {
        if (efg_GetSpiSigNum(ptspisig) == num)
            break;
    }
    return ptspisig;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_AddCkSpiSig                                                */
/*                                                                           */
/* le nom doit contenir le nom du top level                                  */
/*                                                                           */
/*****************************************************************************/
/*
void efg_AddCkSpiSig (head,name,tpckup,tpckdn,tckstab)
    spisig_list *head;
    char        *name;
    long         tpckup;
    long         tpckdn;
    long         tckstab;
{
    spisig_list *ptspisig = NULL ;

    if ((ptspisig = efg_GetSpiSig(head,name)) != NULL) {
        ptspisig->TPCKUP  = tpckup;
        ptspisig->TPCKDN  = tpckdn;
        ptspisig->TCKSTAB = tckstab;
    }
}
*/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_AddExtraCapa                                               */
/*                                                                           */
/* le nom doit contenir le nom du top level                                  */
/*                                                                           */
/*****************************************************************************/
void efg_AddExtraCapa (spisig_list *ptspisig, float capa)
{
        ptspisig->ADDCAPA = capa;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_InitSigValue                                               */
/*                                                                           */
/* le nom doit contenir le nom du top level                                  */
/*                                                                           */
/*****************************************************************************/
void efg_InitSigVolt (head,name,volt)
    spisig_list *head;
    char        *name;
    float        volt;
{
    spisig_list *ptspisig = NULL ;

    if ((ptspisig = efg_GetSpiSig(head,name)) != NULL) {
        ptspisig->INITVOLT = volt;
    }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_AddPathEvent                                               */
/*                                                                           */
/* NB : le 1er element a ajouter doit etre le dernier node                   */
/*                                                                           */
/*****************************************************************************/
chain_list *efg_AddPathEvent (chain_list *path,char *node,char event, int outpath, int mindate, int date, long inrc, long outrc, char *latchcmd, char latchcmddir,char *previous)
{
  efg_path_event_info *pei;

  pei=(efg_path_event_info *)mbkalloc(sizeof(efg_path_event_info));
  pei->DATA=node;
  if (event == EFG_RISE)
      pei->TYPE = (long)EFG_SIG_SET_RISE;
  else if (event == EFG_FALL)
      pei->TYPE = (long)EFG_SIG_SET_FALL;
  else
      pei->TYPE = 0;
  pei->flags=0;
  pei->mindate=mindate;
  pei->date=date;
  pei->innode=inrc;
  pei->outnode=outrc;
  pei->latchcmd=latchcmd;
  pei->latchcmddir=latchcmddir;
  pei->previous=previous;
  if (outpath) pei->flags|=EFG_SIG_SET_OUTPATH;
  path = addchain (path,pei);
  return path;
}

void efg_FreePathEvent (chain_list *path)
{
  while (path!=NULL)
  {
    mbkfree(path->DATA);
    path=delchain(path, path);
  }
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_CreatePathByInf                                            */
/*                                                                           */
/*****************************************************************************/
ptype_list *efg_CreatePathByInf (list_list *infpathsig)
{
    list_list  *list;
    ptype_list *path = NULL;
    long        slope;

    if ((!infpathsig) || (infpathsig->TYPE != INF_LL_PATHSIGS))
        return NULL;
    for (list = infpathsig ; list ; list = list->NEXT) {
        if (getptype((ptype_list*)list->USER,INF_UP) != NULL)
            slope    = (long)EFG_SIG_SET_RISE;
        else
            slope    = (long)EFG_SIG_SET_FALL;
        path = addptype (path,slope,list->DATA);
    }
    return path;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_BuildSpiSigList                                            */
/*                                                                           */
/* 1) Marque les differents signaux de la lofig par une ptype_list contenant */
/*    l'event dans le TYPE et le nom du signal  dans la DATA                 */
/*                                                                           */
/* 2) Construit une struct spisig                                            */
/*                                                                           */
/* NB : la liste represente le chemin, le premier element est le 1er sig     */
/*                                                                           */
/* AJOUT : event_list peut contenir plusieurs chemins séparés par des ptype  */
/* dont les champs valent 0. Dans ce cas le premier chemin est un chemin de  */
/* DATA, et les suivant sont des chemins de COMMAND. Sert uniquement pour    */
/* la génération d'un spicedeck permettant de mesurer le SETUP et le HOLD.   */
/*                                                                           */
/*****************************************************************************/

char *efg_checkvalidnode(losig_list *ls, long num, int *haslonode)
{
  chain_list *cl;
  lonode_list *ln;
  locon_list *lc;
  if (haslonode) *haslonode=0;
  if (num<0) return NULL;
  if (ls->PRCN!=NULL)
  {
     if (num>=ls->PRCN->NBNODE) return NULL;
     if ((ln=getlonode(ls, num))==NULL) return NULL;
     if (haslonode) *haslonode=1;
     cl=getloconnode(ln);
     if (cl!=NULL)
     {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='T')
        return concatname(((lotrs_list *)lc->ROOT)->TRNAME,lc->NAME);
     }
  }
  return NULL;
}
spisig_list *efg_BuildSpiSigList (lofig_list *lofig,chain_list *event_list,spisig_list *spisig,char markstartend, int *error, int quiet)
{
    losig_list  *losig = NULL;
    chain_list  *event;
    int          numsig = 0;
    long         slope; 
    char        *loinsname,*devectname;
    char         sigslope;
    char         sigend;
    char         sigstart;
    char         sigclk;
    ptype_list  *ptype;
    chain_list  *chain;
    char        *name2report,*left,*right;
    ht *htname;
    NameAllocator na;
    char *name, *latchcmd, latchcmddir;
    long flags, l;
    efg_path_event_info *pei, *pei2, *prevpath=NULL;
    locon_list *lc;
    int pathnum=1;
    spisig_list *ssig;
    
    *error=0;

    if ((lofig != NULL) && (event_list != NULL)) {
    
        sigstart = 'Y' ;
        sigclk   = 'N' ;
        
        htname = addht(50);
        CreateNameAllocator( 50, &na, 'y' );

        for (event = event_list; event ; event = event->NEXT) {
        
            pei=(efg_path_event_info *)event->DATA;
            latchcmd=pei->latchcmd, latchcmddir=pei->latchcmddir;
            if( !pei->DATA && !pei->TYPE ) {
                sigstart = 'Y';
                sigclk   = 'Y';
                pathnum=pathnum<<1;
                continue ;
            }

            name = NameAlloc( &na, (char*)pei->DATA );
            l = gethtitem( htname, name );
        
            if( l == EMPTYHT ) {
            
              addhtitem( htname, name, (long)pei );

              devectname = mbk_devect ((char*)pei->DATA,"[","]");
              losig = efg_GetHierSigByName (lofig,devectname,
                                            &loinsname,NULL,'Y');
              if ((pei->TYPE & EFG_SIG_SET_MASK) == (long)EFG_SIG_SET_RISE) {
                  sigslope = 'U';
                  slope    = (long)EFG_SIG_SET_RISE;
              }
              else {
                  sigslope = 'D';
                  slope    = (long)EFG_SIG_SET_FALL;
              }
              if (losig != NULL) {
                  if (event->NEXT!=NULL) pei2=(efg_path_event_info *)event->NEXT->DATA;
                  else pei2=NULL;
                  if ( pei2==NULL || pei2->DATA==NULL || (strcmp(pei2->DATA,pei->DATA)==0 && event->NEXT->NEXT==NULL))
                  {
                      sigend = 'Y';
                      if (pei2!=NULL) latchcmd=pei2->latchcmd, latchcmddir=pei2->latchcmddir;
                  }
                  else
                      sigend = 'N';
                  flags=0;
                  if (pei->flags & EFG_SIG_SET_OUTPATH) flags|=EFG_SIG_SET_OUTPATH;
                  if( markstartend )
                    efg_SetSigDrive (losig,pei->date!=-1?pei->date:numsig,sigslope,numsig==0||sigstart=='Y'?'Y':'N',sigend,sigclk,loinsname,flags);
                  else
                    efg_SetSigDrive (losig,pei->date!=-1?pei->date:numsig,sigslope,'N','N',sigclk,loinsname,flags);
                  name2report = concatname (loinsname,efg_getlosigname (losig));
                  leftunconcatname (name2report,&left,&right);
                  spisig = efg_AddSpiSig (spisig,losig,right,pei->date!=-1?pei->date:numsig,slope,pei->mindate);
                  spisig->critic_NAME = (char*)pei->DATA; 
                  spisig->LATCHCMD=latchcmd;
                  spisig->LATCHCMDDIR=latchcmddir;
                  spisig->PATHNUM=pathnum;
                  spisig->PREVIOUS=pei->previous;
                  if (pei2!=NULL && pei2->DATA!=NULL && strcmp((char *)pei->DATA, (char *)pei2->DATA)==0)
                  {
                    // line rc
                    spisig->INCONRC_NODE = pei2->innode; 
                    spisig->OUTCONRC_NODE = pei2->outnode; 
                    spisig->HASRCLINE=1;
                  }
                  else
                  {
                    spisig->INCONRC_NODE = pei->innode; 
                    spisig->OUTCONRC_NODE = pei->outnode; 
                  }
                  if (lofig->LOINS!=NULL)
                  {
                   ptype = getptype(losig->USER,LOFIGCHAIN);
                   if (ptype != NULL) {
                      for (chain = (chain_list*)ptype->DATA ; chain ;
                           chain = chain->NEXT) {
                          locon_list *locon = (locon_list*)chain->DATA;
                          if (locon->TYPE == 'I') {
                              efg_UpdateSigPtype(
                                      getloadedlofig(((loins_list*)locon->ROOT)->FIGNAME),
                                      locon->NAME,
                                      ((loins_list*)locon->ROOT)->INSNAME
                                      ,loinsname,numsig,sigslope,
                                       sigend);
                          }
                      }
                   }
                  }
                  numsig++ ; /* markage des ptypes du sig */
              }
              else {
                  if (!quiet)
                    avt_errmsg(EFG_ERRMSG, "006", AVT_ERROR, (char*)pei->DATA,lofig->NAME);
//                   printf ("\n!!! efg warning : signal %s is not in figure %s !!!\n", (char*)pei->DATA,lofig->NAME);
                  (*error)++;
              }
            }
            else {
              ssig=efg_GetSpiSig(spisig,namealloc((char*)pei->DATA));
              if (ssig!=NULL) ssig->PATHNUM|=pathnum;
              prevpath=(efg_path_event_info *)l;
              if((prevpath->TYPE & EFG_SIG_SET_MASK)!= (pei->TYPE & EFG_SIG_SET_MASK)) {
                if (!quiet)
                  avt_errmsg(EFG_ERRMSG, "007", AVT_WARNING, (char*)pei->DATA);
//                  fprintf( stderr, "[TAS WAR] event %s appear with different slopes\n", (char*)pei->DATA );
                (*error)++;
              }
            }
            sigstart = 'N' ;
        }
        delht( htname );
        DeleteNameAllocator( &na );
    }
    return spisig;
}

/* creation de la spisig_list directement par les infos du .inf */

spisig_list *efg_BuildSpiSigFromInf (lofig,headlist)
    lofig_list *lofig;
    list_list  *headlist;
{
    losig_list  *losig = NULL;
    list_list   *list;
    int          numsig = 1;
    long         slope; 
    spisig_list *spisig = NULL;
    char        *loinsname;
    char         sigslope;
    char         sigend;
    ptype_list  *ptype;
    chain_list  *chain;
    char         buf[2048];
    char        *name2report,*devectname;
    char        *left,*right;

    if ((lofig != NULL) && (headlist != NULL) && (headlist->TYPE == INF_LL_PATHSIGS)) {
        for (list = headlist ; list ; list = list->NEXT) {
            devectname = mbk_devect ((char*)list->DATA,"[","]");
            losig = efg_GetHierSigByName (lofig,devectname,
                                          &loinsname,NULL,'Y');
            if (getptype((ptype_list*)list->USER,INF_UP) != NULL) {
                sigslope = 'U';
                slope    = (long)EFG_SIG_SET_RISE;
            }
            else {
                sigslope = 'D';
                slope    = (long)EFG_SIG_SET_FALL;
            }
            if (losig != NULL) {
                if (list->NEXT == NULL)
                    sigend = 'Y';
                else
                    sigend = 'N';
                efg_SetSigDrive (losig,numsig,sigslope,numsig==0?'Y':'N',sigend,'N',loinsname,0);
                sprintf(buf,"%s%c%s",loinsname,SEPAR,efg_getlosigname(losig));
                name2report = namealloc (buf);
                leftunconcatname (name2report,&left,&right);
                spisig = efg_AddSpiSig (spisig,losig,right,numsig,slope,-1);
                
                ptype = getptype(losig->USER,LOFIGCHAIN);
                if (ptype != NULL) {
                    for (chain = (chain_list*)ptype->DATA ; chain ;
                         chain = chain->NEXT) {
                        locon_list *locon = (locon_list*)chain->DATA;
                        if (locon->TYPE == 'I') {
                            efg_UpdateSigPtype(
                                    getloadedlofig(((loins_list*)locon->ROOT)->FIGNAME),
                                    locon->NAME,
                                    ((loins_list*)locon->ROOT)->INSNAME
                                    ,loinsname,numsig,sigslope,
                                     sigend);
                        }
                    }
                }
                numsig++ ; /* markage des ptypes du sig */
            }
            else {
                    avt_errmsg(EFG_ERRMSG, "006", AVT_ERROR, (char*)list->DATA,lofig->NAME);
/*                printf ("\n!!! efg warning : signal %s is not in figure %s !!!\n",
                        (char*)list->DATA,lofig->NAME);*/
            }
        }
    }
    return spisig;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetSig2Print                                               */
/*                                                                           */
/* Renvoie une liste de signaux a ploter par rapport aux signaux du .inf     */
/* Gere egalement le cas des reseaux rc.                                     */
/*                                                                           */
/*****************************************************************************/
chain_list *efg_GetSig2Print (spisig_list *headspisig)
{
 spisig_list *spisig;
 chain_list *chain,*sig2print = NULL;
 loins_list *loins;
 locon_list *locon;
 losig_list *destsig;
 ptype_list *ptype;
 chain_list *lofigchain;
 num_list   *pnode;
 ht         *htnode;
 char       *nodename;
 char       *loinsname, *signame;

 htnode = addht (100);
 for (spisig = headspisig ; spisig ; spisig=spisig->NEXT) {
     if ((destsig = efg_GetDestSigBySpiSig (spisig)) != NULL) {
         ptype = getptype (destsig->USER,LOFIGCHAIN);
         if (ptype != NULL) {
             lofigchain = (chain_list*)ptype->DATA;
             for (chain = lofigchain ; chain ; chain=chain->NEXT) {
                 locon = (locon_list*)chain->DATA;
                 if (locon->TYPE == 'E') {
                     for (pnode = locon->PNODE ; pnode ; pnode=pnode->NEXT) {
                         nodename = spinamenode (locon->SIG,(long)pnode->DATA);
                         if (gethtitem(htnode,nodename) == EMPTYHT) {
                             fprintf (stderr,"sig %s pnode : %ld\n",
                                     efg_getlosigname(locon->SIG),(long)pnode->DATA);
                             addhtitem (htnode,nodename,0);
                             sig2print = addchain (sig2print,nodename);
                         }
                     }
                 }
                 else if (locon->TYPE == 'I') {
                     loins = (loins_list*)locon->ROOT;
                     loinsname = concatname (EFGCONTEXT->SRCFIG->NAME,
                                             efg_GetSpiSigName(spisig));
                     rightunconcatname (loinsname,&loinsname,&signame);
                     loinsname = concatname (loinsname,loins->INSNAME);
                     if ((efg_loins_is2analyse (loinsname))) {
                         for (pnode = locon->PNODE ; pnode ; pnode=pnode->NEXT) {
                             nodename = spinamenode (locon->SIG,(long)pnode->DATA);
                             if (gethtitem(htnode,nodename) == EMPTYHT) {
                                 fprintf (stderr,"sig %s pnode : %ld\n",
                                         efg_getlosigname(locon->SIG),(long)pnode->DATA);
                                 addhtitem (htnode,nodename,0);
                                 sig2print = addchain (sig2print,nodename);
                             }
                         }
                     }
                 }
                 else if (locon->TYPE == 'T') {
                     for (pnode = locon->PNODE ; pnode ; pnode=pnode->NEXT) {
                         nodename = spinamenode (locon->SIG,(long)pnode->DATA);
                         if (gethtitem(htnode,nodename) == EMPTYHT) {
                             fprintf (stderr,"sig %s pnode : %ld\n",
                                     efg_getlosigname(locon->SIG),(long)pnode->DATA);
                             addhtitem (htnode,nodename,0);
                             sig2print = addchain (sig2print,nodename);
                         }
                     }
                 }
             }
         }
     }
 }
 delht (htnode);
 return sig2print;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetFirstSpiSig                                             */
/*                                                                           */
/*****************************************************************************/
spisig_list *efg_GetFirstSpiSig (spisig_list *head)
{
  spisig_list *spisig;
  for (spisig = head ; spisig ; spisig=spisig->NEXT) {
    if (efg_GetSpiSigNum(spisig) == 0)
      break;
  }
  return spisig;
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION : efg_GetLastSpiSig                                              */
/*                                                                           */
/*****************************************************************************/
spisig_list *efg_GetLastSpiSig (spisig_list *head)
{
/*  spisig_list *spisig = head;
 *  
  if (!spisig) return NULL;
  if (!spisig->NEXT) return spisig;
  if (efg_GetSpiSigNum (spisig) > efg_GetSpiSigNum(spisig->NEXT))
    return spisig;
  for (spisig = head ; spisig->NEXT ; spisig=spisig->NEXT);
  return spisig;
  */
  spisig_list *spisig, *best=NULL;
  if (head==NULL) return NULL;
  for (spisig = head ; spisig!=NULL ; spisig=spisig->NEXT)
    if (best==NULL || (efg_GetSpiSigNum (spisig)!=-1 && efg_GetSpiSigNum (spisig) > efg_GetSpiSigNum(best))) best=spisig;
  return best;
}

void efg_correct_rc_nodes_based_on_rcx(spisig_list *head)
{
  char *rcnodename;
  while (head!=NULL)
  {
    if ((rcnodename=efg_checkvalidnode(head->DESTSIG, head->INCONRC_NODE, NULL))!=NULL)
      head->INCONRC=rcnodename;
    if ((rcnodename=efg_checkvalidnode(head->DESTSIG, head->OUTCONRC_NODE, NULL))!=NULL)
      head->OUTCONRC=rcnodename;
    head=head->NEXT;
  }
}

