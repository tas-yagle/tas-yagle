/******************************************************************************\
Permet de contruire une figure hierarchique avec pour intances uniquement des
cellules de base
\******************************************************************************/

#include "efg.h"

efg_global *EFGCONTEXT = NULL;

/******************************************************************************\
ALLOCATIONS
\******************************************************************************/

efg_fullsiglist* efg_addchainfullsig( efg_fullsiglist *head )
{
  efg_fullsiglist *ptfullsig;

  ptfullsig = (efg_fullsiglist*) mbkalloc (sizeof (efg_fullsiglist));
  
  ptfullsig->NEXT    = head;
  ptfullsig->SIGLIST = NULL;
  ptfullsig->DESTSIG = NULL;

  head = ptfullsig;

  return head;
}

void efg_freechainfullsig( efg_fullsiglist *head )
{
  efg_fullsiglist *ptfullsig;

  while (head) {
      ptfullsig = head;
      head = head->NEXT;
      efg_freechainsrcsig (ptfullsig->SIGLIST);
      mbkfree (ptfullsig);
  }
}

efg_srcsiglist* efg_addchainsrcsig( efg_srcsiglist* head )
{
    efg_srcsiglist *ptsrcsig;

    ptsrcsig = (efg_srcsiglist*) mbkalloc (sizeof(efg_srcsiglist));

    ptsrcsig->NEXT    = head;
    ptsrcsig->ROOT    = NULL;
    ptsrcsig->LEVEL   = -1;
    ptsrcsig->CONTEXT = NULL;
    ptsrcsig->LOSIG   = NULL;
    ptsrcsig->LOFIG   = NULL;
    ptsrcsig->TABCONV = NULL;

    head = ptsrcsig;
    return head;
}

void efg_freechainsrcsig( efg_srcsiglist* head )
{
    efg_srcsiglist *ptsrcsig;

    while (head) {
        ptsrcsig = head;
        head = head->NEXT;
        mbkfree (ptsrcsig);
    }
}

efg_ctxlist* efg_getcontext( char *context )
{
  efg_ctxlist   *newcontext;

  if( EFGCONTEXT->HTCONTEXT ) {
    newcontext = (efg_ctxlist*)gethtitem( EFGCONTEXT->HTCONTEXT, context );
    if( newcontext != (efg_ctxlist*)EMPTYHT &&
        newcontext != (efg_ctxlist*)DELETEHT   )
      return newcontext;
  }
  else
    EFGCONTEXT->HTCONTEXT = addht(10);
  
  newcontext = (efg_ctxlist*)mbkalloc( sizeof( efg_ctxlist ) );
  newcontext->LOCONS      = NULL;
  newcontext->SRCSIG      = NULL;
  newcontext->LOINSCOPIED = NULL;

  newcontext->NEXT = EFGCONTEXT->CONTEXT;
  EFGCONTEXT->CONTEXT = newcontext;

  addhtitem( EFGCONTEXT->HTCONTEXT, context, (long)newcontext );
  return newcontext;
}

void efg_createcontext( void )
{
    if (EFGCONTEXT != NULL)
        efg_freecontext () ;

    EFGCONTEXT = (efg_global*) mbkalloc (sizeof(efg_global));
    EFGCONTEXT->SRCFIG      = NULL;
    EFGCONTEXT->DESTFIG     = NULL;
    EFGCONTEXT->VSSDESTFIG  = NULL;
    EFGCONTEXT->VDDDESTFIG  = NULL;
    EFGCONTEXT->CONTEXT     = NULL;
    EFGCONTEXT->HTCONTEXT   = NULL;
    EFGCONTEXT->CONTEXT     = NULL;
    EFGCONTEXT->SPISIG      = NULL;
    EFGCONTEXT->ALIMS       = NULL;
    EFGCONTEXT->ALLADDEDCONES       = NULL;
}

void efg_freecontext( void )
{
  efg_ctxlist   *scan;
  efg_ctxlist   *next;
  cone_list *cn;
  
    if (EFGCONTEXT != NULL) {
        if(  EFGCONTEXT->CONTEXT ) {
          for( scan = EFGCONTEXT->CONTEXT ; scan ; scan = next ) {
            next = scan->NEXT;
            if( scan->LOCONS ) delht( scan->LOCONS );
            if( scan->SRCSIG ) delht( scan->SRCSIG );
            if( scan->LOINSCOPIED ) delht( scan->LOINSCOPIED );
            mbkfree( scan );
          }
          delht(EFGCONTEXT->HTCONTEXT); 
        }
        freechain(EFGCONTEXT->ALLADDEDCONES);
        freeptype(EFGCONTEXT->ALIMS);
        mbkfree (EFGCONTEXT);
        EFGCONTEXT = NULL;
    }
}

void efg_setsrcfig2context (lofig_list *fig)
{
    EFGCONTEXT->SRCFIG = fig;
}

void efg_setdestfig2context (lofig_list *fig)
{
    EFGCONTEXT->DESTFIG = fig;
}

lofig_list *efg_getextfig ()
{
    return EFGCONTEXT->DESTFIG;
}

void efg_setspisig2context (spisig_list *spisig)
{
    EFGCONTEXT->SPISIG = spisig;
}

void efg_setdestlocon( char *context, locon_list *locon, locon_list *newlocon )
{
  efg_ctxlist *ctx;
 
  ctx = efg_getcontext( context );
  if( !ctx->LOCONS ) 
    ctx->LOCONS = addht( 10 );

  addhtitem( ctx->LOCONS, locon, (long)newlocon );
}

locon_list *efg_getdestlocon( char *context, locon_list *locon )
{
  efg_ctxlist *ctx;
  locon_list  *newlocon;

  ctx = efg_getcontext( context );
  if( !ctx->LOCONS )
    return NULL;

  newlocon = (locon_list*)gethtitem( ctx->LOCONS, locon );
  if( newlocon == (locon_list*)EMPTYHT || newlocon == (locon_list*)DELETEHT ) 
    return NULL;

  return newlocon;
}

/******************************************************************************\
Gestion du contexte
\******************************************************************************/

/*
Permet de retrouver à partir d'un losig dans l'ensemble de figures original
son fullsig correspondant.
*/

void efg_set_sig2srcsig ( char *context, 
                          losig_list *losig, 
                          efg_srcsiglist *srcsig 
                        )
{
  efg_ctxlist *ctx;
  
  ctx = efg_getcontext( context );
  if( !ctx->SRCSIG )
    ctx->SRCSIG = addht(10);

  addhtitem( ctx->SRCSIG, losig, (long)srcsig );
}

void efg_setfullsig( char            *context, 
                     lofig_list      *lofig,
                     losig_list      *losig, 
                     int              level,
                     efg_fullsiglist *head
                   )
{
  head->SIGLIST = efg_addchainsrcsig( head->SIGLIST );
  head->SIGLIST->ROOT    = head;
  head->SIGLIST->CONTEXT = context;
  head->SIGLIST->LOSIG   = losig;
  head->SIGLIST->LOFIG   = lofig;
  head->SIGLIST->LEVEL   = level;

  if (!context || !losig || !head) return;
  efg_set_sig2srcsig (context,losig, head->SIGLIST);
}

efg_srcsiglist* efg_getsrcsig( char *context, 
                               losig_list *losig
                             )
{
  efg_ctxlist    *ctx;
  efg_srcsiglist *res;
  
  ctx = efg_getcontext( context );
  if( !ctx->SRCSIG ) return NULL;

  res = (efg_srcsiglist*)gethtitem(ctx->SRCSIG,losig);
  if ((res != (efg_srcsiglist*)EMPTYHT) && (res != (efg_srcsiglist*)DELETEHT))
    return res;
  return NULL;
}

efg_fullsiglist* efg_getfullsig( char *context, 
                                 losig_list *losig
                               )
{
  efg_fullsiglist *fullsig = NULL;
  efg_srcsiglist *srcsig = NULL;

  srcsig = efg_getsrcsig( context, losig);
  if (srcsig != NULL)
      fullsig = srcsig->ROOT;

  return fullsig;
}

/*
void efg_clearfullsig( char *context, 
                        losig_list *losig 
                      )
{
}
*/

/******************************************************************************\
Savoir si une loins de base a été copiée
\******************************************************************************/

int efg_isloinscopied( char *context, char *insname )
{
    efg_ctxlist *ctx;
    long         res;

    ctx = efg_getcontext( context );
    if( !ctx->LOINSCOPIED ) return 0;
    
    res = gethtitem( ctx->LOINSCOPIED, insname );
    if ((res != EMPTYHT) && (res != DELETEHT))
        return 1;
    return 0;
}

void efg_setloinscopied( char *context, char *insname )
{
    efg_ctxlist *ctx;

    ctx = efg_getcontext( context );
    if( !ctx->LOINSCOPIED )
      ctx->LOINSCOPIED = addht(10);

    addhtitem (ctx->LOINSCOPIED,(void*)insname,0l);
}

/******************************************************************************\
A travers la hiérarchie
\******************************************************************************/

char* efg_getsigname( efg_fullsiglist *fullsig )
{
  efg_srcsiglist *topsig;
  char *top,*signame,*fullsrcname;

  topsig = efg_gettopsig( fullsig );
  if (!topsig) return NULL;
  fullsrcname = concatname (topsig->CONTEXT,efg_getlosigname(topsig->LOSIG));
  leftunconcatname (fullsrcname,&top,&signame);
  return signame; 
}

locon_list* efg_getexternallocon( efg_fullsiglist *fullsig )
{
  efg_srcsiglist *topsig;
  ptype_list     *ptl;
  chain_list     *scan;
  locon_list     *locon;

  topsig = efg_gettopsig( fullsig );
  
  if( topsig->LOSIG->TYPE == 'I' ) 
    return NULL;
    
  ptl = getptype( topsig->LOSIG->USER, LOFIGCHAIN );
  if( !ptl ) 
    return NULL;

  for( scan = (chain_list*)ptl->DATA ; scan ; scan = scan->NEXT ) {
    locon = (locon_list*)scan->DATA;
    if( locon->TYPE == 'E' )
      return locon;
  }

  return NULL;
}

char efg_getdir( efg_fullsiglist *fullsig )
{
    // je sens ke c est pas bon
    char dir;
    char *context;
    losig_list *losig;
    efg_srcsiglist *srcsig;

    srcsig = fullsig->SIGLIST;
    context = srcsig->CONTEXT;
    losig = srcsig->LOSIG;
    if (efg_sig_is_beg (losig,context) == 1)
        dir = 'I';
    else if (efg_sig_is_end (losig,context) == 1)
        dir = 'O';
    else 
        dir = 'X';
    
    return dir;
}

efg_srcsiglist* efg_gettopsig( efg_fullsiglist *fullsig )
{
  int             minlevel=INT_MAX ;
  efg_srcsiglist *siglist ;
  efg_srcsiglist *topsig = NULL;

  // Construction des tables de conversion des index des noeuds, à partir du
  // signal de plus haut niveau.
  for( siglist = fullsig->SIGLIST; siglist ; siglist = siglist->NEXT ) {
    if( siglist->LEVEL < minlevel ) {
      minlevel = siglist->LEVEL;
      topsig = siglist;
    }
  }

  return topsig;
}

/******************************************************************************\
Fonctions concernant le travail à faire
\******************************************************************************/

/******************************************************************************\
Fonction : efg_getsigtobecopied

renvoie chain_list des signaux de la figure originale
\******************************************************************************/

chain_list* efg_getsigtobecopied( char *context )
{
    lofig_list *figins;
    losig_list *losig;
    chain_list *chainsig = NULL;

    figins = efg_get_fig_from_ctxt (NULL,context);
    if (!figins) return NULL;
    for (losig = figins->LOSIG ; losig ; losig = losig->NEXT) {
        if (efg_sig_is_onpath (losig,context) == 1)
            chainsig = addchain (chainsig,losig);
    }
    return chainsig;
}

/******************************************************************************\
Fonction : efg_isloinsbasic
supposons que le niveau le plus bas soit atteind si l'instance a des
transistors
\******************************************************************************/
int efg_isloinsbasic( char *context )
{
    lofig_list *figins;
    int res = 0;

    figins = efg_get_fig_from_ctxt (NULL,context);
    if (!figins) return 0;
    if (figins->LOTRS != NULL)
        res = 1;

    return res;
}

/******************************************************************************\
Fonction : efg_getloinstobetreated
renvoie chain_list des instances a sensibiliser (de la figure originale)
\******************************************************************************/
chain_list* efg_getloinstobetreated( char *context )
{
    lofig_list *figins;
    loins_list *loins;
    chain_list *chainins = NULL;
    char *newcontext;

    figins = efg_get_fig_from_ctxt (NULL,context);
    if (!figins) return 0;
    for (loins = figins->LOINS ; loins ; loins = loins->NEXT) {
        newcontext = concatname( context, loins->INSNAME );
        if ((efg_loins_is2analyse (newcontext)))
            chainins = addchain (chainins,newcontext);
    }
    return chainins;
}

/******************************************************************************\
Construit tous les fullsig de la lofig context.
\******************************************************************************/

efg_fullsiglist* efg_buildfullsig( char            *context,
                                   char             withexternal,
                                   efg_fullsiglist *head,
                                   int              level
                                 )
{
  chain_list    *tobecopied;
  chain_list    *inslist;
  losig_list    *losig;

  tobecopied = efg_getsigtobecopied( context );
  
  for( inslist = tobecopied ; inslist ; inslist = inslist->NEXT ) {
    losig = (losig_list*)inslist->DATA;
    if( losig->TYPE == 'E' && !withexternal ) 
      continue;

    head = efg_getmerge( head, context, losig, level );
  }

  freechain( tobecopied );
  return head;
}

/******************************************************************************\
Regroupe tous les signaux equipotentiel à currentsig à travers la hiérarchie.
\******************************************************************************/

void efg_recgetmerge( efg_fullsiglist *head,
                      lofig_list      *lofig,
                      char            *context,
                      losig_list      *losig,
                      int              level
                    )
{
  ptype_list *ptl;
  chain_list *scan;
  locon_list *locon,*loconins;
  loins_list *loins;
  lofig_list *figins;
  char       *name;
  
  efg_setfullsig( context, lofig, losig, level, head );

  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( ptl ) {
    for( scan = (chain_list*)ptl->DATA ; scan ; scan = scan->NEXT ) {
      locon = (locon_list*)scan->DATA;
      if( locon->TYPE == 'I' ) {
        loins = (loins_list*)locon->ROOT;
        name = concatname( context, loins->INSNAME );
        if( !efg_isloinsbasic( name ) ) {
          figins = getlofig( loins->FIGNAME, 'A' );
          loconins = getlocon( figins, locon->NAME );
          efg_recgetmerge( head, 
                           figins,
                           name,
                           loconins->SIG,
                           level + 1
                         );
        }
      }
    }
  }
}

efg_fullsiglist* efg_getmerge( efg_fullsiglist *head,
                               char            *context,
                               losig_list      *currentsig,
                               int              level
                             )
{
  head = efg_addchainfullsig( head );
  efg_recgetmerge( head,
                   efg_get_fig_from_ctxt ( NULL , context ),
                   context,
                   currentsig,
                   level );
  
  return head;
}

/******************************************************************************\
Construit tous les fullsig de la lofig context, et de ses niveaux inférieurs.

Fonction de haut niveau : efg_buildhierfullsig( lofig->NAME, 1, NULL, 1 );
\******************************************************************************/

efg_fullsiglist* efg_buildhierfullsig( char            *context,
                                       char             withexternal,
                                       efg_fullsiglist *head,
                                       int              level
                                     )
{
  chain_list *tobetreated;
  chain_list *inlist;
  char       *newcontext;
  
  head = efg_buildfullsig( context, withexternal, head, level );

  tobetreated = efg_getloinstobetreated( context );

  for( inlist = tobetreated ; inlist ; inlist = inlist->NEXT ) {
    newcontext = (char*)inlist->DATA;
    head = efg_buildhierfullsig( newcontext, 0, head, level+1 );
  }

  freechain( tobetreated );
  return head;
}

/******************************************************************************\
Création des losig dans la figure destination. Ajoute les locons sur les signaux
externes.
\******************************************************************************/

void efg_createalllosig( efg_fullsiglist *siglist )
{
  efg_fullsiglist *fullsig;
  losig_list      *newsig;
  locon_list      *locon;
  locon_list      *newcon;
  char *name;
  int   indexsig;

  indexsig = getnumberoflosig( EFGCONTEXT->DESTFIG ) + 1;
  
  for( fullsig = siglist ; fullsig ; fullsig = fullsig->NEXT ) {
    locon  = efg_getexternallocon( fullsig );
    name   = efg_getsigname( fullsig );
    newsig = addlosig( EFGCONTEXT->DESTFIG,
                       indexsig++,
                       addchain( NULL, name ),
                       locon ? 'E' : 'I'
                     );
    if( locon ) {
      newcon =addlocon( EFGCONTEXT->DESTFIG, 
                        name, 
                        newsig, 
                        efg_getdir( fullsig ) 
                      );
      efg_setdestlocon( efg_gettopsig( fullsig )->CONTEXT,
                        locon,
                        newcon
                      );
    }
    fullsig->DESTSIG = newsig ;
  }

  if( !EFGCONTEXT->VSSDESTFIG ) {
    EFGCONTEXT->VSSDESTFIG = addlosig( EFGCONTEXT->DESTFIG,
                                       indexsig++,
                                       addchain( NULL, namealloc( "VSS" ) ),
                                       'E'
                                     );
    addlorcnet( EFGCONTEXT->VSSDESTFIG );
    newcon = addlocon( EFGCONTEXT->DESTFIG, 
                       namealloc( "VSS" ), 
                       EFGCONTEXT->VSSDESTFIG,
                       UNKNOWN
                     );
    setloconnode( newcon, 1 );
  }

  if( !EFGCONTEXT->VDDDESTFIG ) {
    EFGCONTEXT->VDDDESTFIG = addlosig( EFGCONTEXT->DESTFIG,
                                       indexsig++,
                                       addchain( NULL, namealloc( "VDD" ) ),
                                       'E'
                                     );
    addlorcnet( EFGCONTEXT->VDDDESTFIG );
    newcon = addlocon( EFGCONTEXT->DESTFIG, 
                       namealloc( "VDD" ), 
                       EFGCONTEXT->VDDDESTFIG,
                       UNKNOWN
                     );
    setloconnode( newcon, 1 );
  }
}

locon_list *efg_getinslocon (loins_list *ptins,char *name)
{
  locon_list *ptcon = NULL;

  for (ptcon = ptins->LOCON ; ptcon ; ptcon=ptcon->NEXT) {
      if (ptcon->NAME == name)
          break;
  }
  return ptcon;
}

/******************************************************************************\
Recupere les index des noeuds RC d'une instance
\******************************************************************************/
void efg_set_loins_rc_node (loins_list *loins)
{
  locon_list  *locon;
  spisig_list *spisig;

  if (!loins) return;

  for (locon = loins->LOCON ; locon ; locon = locon->NEXT) {
    if (locon->SIG->PRCN && 
        locon->SIG->PRCN->PWIRE && 
        (locon->PNODE) && 
        getptype (locon->SIG->USER,EFG_SIG_SET) != NULL ) {
      if ((spisig = efg_GetSpiSig (EFGCONTEXT->SPISIG,efg_getlosigname(locon->SIG)))) { 
        if (locon->DIRECTION == 'I' || locon->DIRECTION == 'O') {
          if (locon->PNODE && !efg_is_conname_ext (EFGCONTEXT->DESTFIG,locon->SIG,locon->PNODE->DATA))
            efg_SetSpiSigLoconRc (spisig, concatname(loins->INSNAME,locon->NAME),locon->DIRECTION,NULL);
        }
      }
    }
  }
}

/******************************************************************************\
Construction des instances.
Ajoute les losig et les locons des entrées des instances qui ne sont pas sur 
le chemin.
Appelle efg_setdestlocon().
\******************************************************************************/

void efg_createloins( char *context, loins_list *loins )
{
  lofig_list      *figins;
  locon_list      *locon;
  losig_list      *newsig;
  chain_list      *head;
  char            *conname,*name,*top,*shortinsname,*shortsigname;
  char            *fullinsname,dir;
  int              sigindex;
  efg_fullsiglist *full;
  loins_list      *newloins;
  locon_list      *newlocon;
  ptype_list      *ph_interf;
  
  sigindex = getnumberoflosig( EFGCONTEXT->DESTFIG ) + 1 ;
  figins = getlofig( loins->FIGNAME, 'P' );

  head=NULL;
  for( locon = figins->LOCON ; locon ; locon = locon->NEXT ) {
    full = efg_getfullsig( context, efg_getinslocon(loins,locon->NAME)->SIG );
    if( full ) {
      head = addchain( head, full->DESTSIG );
    }
    else {
      if( mbk_LosigIsVDD (locon->SIG) ) {
        head = addchain( head, EFGCONTEXT->VDDDESTFIG );
      }
      else 
      if( mbk_LosigIsVSS( locon->SIG ) ) {
        head = addchain( head, EFGCONTEXT->VSSDESTFIG );
      }
      else {      
        conname = concatname (loins->INSNAME,locon->NAME);
        name = concatname( context, conname );
        leftunconcatname (name,&top,&shortsigname);
        newsig = addlosig( EFGCONTEXT->DESTFIG,
                           sigindex++,
                           addchain( NULL, shortsigname),
                           'E'
                         );
        dir = getlocon (figins,locon->NAME)->DIRECTION;
        addlocon( EFGCONTEXT->DESTFIG, shortsigname, newsig, dir);
        head = addchain( head, newsig );
      }
    }
  }
  fullinsname = concatname (context,loins->INSNAME);
  leftunconcatname (fullinsname,&top,&shortinsname);
  newloins = addloins( EFGCONTEXT->DESTFIG, shortinsname, figins,reverse(head));
  if ((ph_interf = getptype (loins->USER,PH_INTERF)) != NULL) 
      newloins->USER = addptype (newloins->USER,
                                 PH_INTERF,
                                 (void*)dupchainlst(ph_interf->DATA));

  for( locon = loins->LOCON ; locon ; locon = locon->NEXT ) {
    full = efg_getfullsig( context, locon->SIG );
    if( full ) {
      for( newlocon = newloins->LOCON ; 
           newlocon->NAME != locon->NAME ;
           newlocon = newlocon->NEXT ); 
      efg_setdestlocon( context, locon, newlocon );
    }
  }
  if (head != NULL) 
      freechain (head);
}

/******************************************************************************\
Construction des instances.
\******************************************************************************/
void efg_createallloins( efg_fullsiglist *siglist )
{
  efg_fullsiglist *fullsig;
  efg_srcsiglist  *srcsig;
  ptype_list *ptl;
  chain_list *scan;
  locon_list *locon;
  loins_list *loins;
  char       *fullname;

  for( fullsig = siglist ; fullsig ; fullsig = fullsig->NEXT ) {
    for( srcsig = fullsig->SIGLIST ; srcsig ; srcsig = srcsig->NEXT ) {
      ptl = getptype( srcsig->LOSIG->USER, LOFIGCHAIN );
      if( ptl ) {
        for( scan = (chain_list*)ptl->DATA ; scan ; scan = scan->NEXT ) {
          locon = (locon_list*)scan->DATA;
          if( locon->TYPE == 'I' ) {
            loins = (loins_list*)(locon->ROOT);
            fullname = concatname( srcsig->CONTEXT, loins->INSNAME );
            if( efg_isloinsbasic( fullname ) ) {
              if( !efg_isloinscopied( srcsig->CONTEXT, loins->INSNAME ) ) {
                efg_setloinscopied( srcsig->CONTEXT, loins->INSNAME );
                efg_createloins( srcsig->CONTEXT, loins );
              }
            }
          }
        }
      }
    }
  }
}

/******************************************************************************\
Construction d'une table de correspondance des noeud
\******************************************************************************/

void efg_addtabconv( efg_srcsiglist *siglist, int decalage )
{
  int             i,n;
  int            *tabconv;
  num_list       *pnodeint,*pnodeext;
  ptype_list     *ptl;
  chain_list     *scan;
  locon_list     *loconint;
  locon_list     *loconext;
  char           *figname;
  char           *insname;
  lofig_list     *lofig;
  loins_list     *loins;
  efg_srcsiglist *extsiglist;
  
  if( !siglist->LOSIG->PRCN ) efg_exit(3);

  n = siglist->LOSIG->PRCN->NBNODE + 1;
  tabconv = (int*) mbkalloc( sizeof( int ) * n );
  siglist->TABCONV = tabconv;
  
  for( i=1 ; i<n ; i++ )
    tabconv[i] = i+decalage ;
  
  ptl = getptype( siglist->LOSIG->USER, LOFIGCHAIN );
  if( ptl ) {

    for( scan = (chain_list*)ptl->DATA ; scan ; scan = scan->NEXT ) {
    
      loconint = (locon_list*)scan->DATA;
      
      if( loconint->TYPE == 'E' ) {
      
        rightunconcatname( siglist->CONTEXT, &figname, &insname );
        
        if( insname ) {
        
          lofig = efg_get_fig_from_ctxt ( NULL, figname);
          if (lofig != NULL)
              loins = getloins( lofig, insname );
          
          for( loconext = loins->LOCON ; loconext ; loconext = loconext->NEXT ) 
            if( loconext->NAME == loconint->NAME )
              break;
              
          if( !loconext ) efg_exit(1);
          
          for( pnodeint = loconint->PNODE, pnodeext = loconext->PNODE ;
               pnodeint && pnodeext ;
               pnodeint = pnodeint->NEXT , pnodeext = pnodeext->NEXT
             ) {
            extsiglist = efg_getsrcsig( figname, loconext->SIG );
            tabconv[pnodeint->DATA] = efg_getequivnode( extsiglist,
                                                        pnodeext->DATA 
                                                      );
          }
          if( pnodeint || pnodeext ) efg_exit(2);
        }
      }
    }
  }
}

/******************************************************************************\
Recupère la valeur d'un noeud dans la figure de destination.
\******************************************************************************/

int efg_getequivnode( efg_srcsiglist *sig, int index )
{
  return sig->TABCONV[index];
}

/******************************************************************************\
Construit les tables de correspondance des noeuds, du haut vers le bas.
\******************************************************************************/

int efg_buildtabconv( efg_fullsiglist *fullsig, 
                       efg_srcsiglist  *topsig,
                       int              decalage
                     )
{
  efg_srcsiglist        *siglist;
  int                    n;
  char                   doitagain;
  int                    level;

  n = decalage ;
  for( level = topsig->LEVEL, doitagain=1 ; doitagain ; level++ ) {
    doitagain = 0;
    for( siglist = fullsig->SIGLIST ; siglist ; siglist = siglist->NEXT ) {
      if( siglist->LEVEL == level ) {
        efg_addtabconv( siglist, n );
        n = n + siglist->LOSIG->PRCN->NBNODE ;
        doitagain = 1;
      }
    }
  }
  return n;
}

/******************************************************************************\
Libère les tables de correspondance des noeuds.
\******************************************************************************/

void efg_cleantabconv( efg_fullsiglist *fullsig )
{
  efg_srcsiglist *siglist;

  for( siglist = fullsig->SIGLIST ; siglist ; siglist = siglist->NEXT ) {
    if( siglist->TABCONV ) {
      mbkfree( siglist->TABCONV );
      siglist->TABCONV = NULL;
    }
  }
}

/******************************************************************************\
Duplique les réseaux RC.
\******************************************************************************/
void efg_copyrc( efg_srcsiglist *srcsig, losig_list *dstsig, losig_list *dstvss )
{
  lowire_list   *scanwire;
  loctc_list    *pctc;
  chain_list    *scanctc;
  ptype_list    *ptl;
  num_list      *newpnode;
  num_list      *pnode;
  chain_list    *scan;
  locon_list    *newcon;
  locon_list    *locon;

  if( !dstsig->PRCN ) 
    addlorcnet( dstsig );

  for( scanwire = srcsig->LOSIG->PRCN->PWIRE ; 
       scanwire ; 
       scanwire = scanwire->NEXT 
     ) {
    addlowire( dstsig,
               scanwire->FLAG,
               scanwire->RESI,
               scanwire->CAPA,
               efg_getequivnode( srcsig, scanwire->NODE1 ),
               efg_getequivnode( srcsig, scanwire->NODE2 )
             );

  }

  for( scanctc = srcsig->LOSIG->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT )
  {
    pctc = (loctc_list*)scanctc->DATA;
    addloctc( dstsig,
              efg_getequivnode( srcsig, rcn_ctcnode( pctc, srcsig->LOSIG ) ),
              dstvss,
              1,
              pctc->CAPA
            );
  }

  ptl = getptype( srcsig->LOSIG->USER, LOFIGCHAIN );
  if( ptl ) {
    for( scan = (chain_list*)ptl->DATA ; scan ; scan = scan->NEXT ) {
      locon = (locon_list*)scan->DATA ;
      newcon = efg_getdestlocon( srcsig->CONTEXT, locon );
      if( newcon ) {
        newpnode=NULL;
        for( pnode = locon->PNODE ; pnode ; pnode = pnode->NEXT ) {
          newpnode=addnum(newpnode, efg_getequivnode( srcsig, pnode->DATA ) );
        }
        for( pnode = newpnode ; pnode ; pnode = pnode->NEXT ) {
          setloconnode( newcon, pnode->DATA );
        }
      }
    }
  }
}

/******************************************************************************\
Construit les réseaux rc dans la figure destination.
\******************************************************************************/

void efg_createrc( efg_fullsiglist *fullsig )
{
  efg_srcsiglist *siglist;

  efg_buildtabconv( fullsig, efg_gettopsig(fullsig), 0 );

  for( siglist = fullsig->SIGLIST ; siglist ; siglist = siglist->NEXT ) 
    efg_copyrc( siglist, fullsig->DESTSIG, EFGCONTEXT->VSSDESTFIG );
  
  efg_cleantabconv( fullsig );
}

/******************************************************************************\
Construit tous les réseaux RC.
\******************************************************************************/

void efg_createallrc( efg_fullsiglist *headsig )
{
  for( ; headsig ; headsig = headsig->NEXT )
    efg_createrc( headsig );
}

/******************************************************************************\
Rajoute les ptypes pour la sensibilisation du chemin
\******************************************************************************/

void efg_set_path_info ( efg_fullsiglist *headsig )
{
  efg_fullsiglist *fullsig;
  efg_srcsiglist *topsig;
  losig_list *sig;
  losig_list *sigdest;
  void       *data;
  char       *context;

  for(fullsig = headsig ; fullsig ; fullsig = fullsig->NEXT ) {
      topsig = efg_gettopsig (fullsig);
      sig = topsig->LOSIG;
      context = topsig->CONTEXT;
      sigdest = fullsig->DESTSIG;
      if ((data = efg_get_hier_sigptype (sig,context,EFG_SIG_SET_HIER)) != NULL) 
          sigdest->USER = addptype (sigdest->USER,EFG_SIG_SET,data);
      if ((data = efg_get_hier_sigptype (sig,context,EFG_SIG_BEG_HIER)) != NULL) 
          sigdest->USER = addptype (sigdest->USER,EFG_SIG_BEG,data);
      if ((data = efg_get_hier_sigptype (sig,context,EFG_SIG_END_HIER)) != NULL) 
          sigdest->USER = addptype (sigdest->USER,EFG_SIG_END,data);
  }
}

/******************************************************************************\
Recupere les instances du chemin
\******************************************************************************/

chain_list *efg_get_loins_on_path( void )
{
    chain_list *chaininstance = NULL;
    loins_list *loins;
    char       *fullinsname;

    if (EFGCONTEXT && EFGCONTEXT->DESTFIG) {
        for (loins = EFGCONTEXT->DESTFIG->LOINS ; loins ; loins=loins->NEXT) {
            fullinsname = concatname (EFGCONTEXT->SRCFIG->NAME,loins->INSNAME);
            if (efg_loins_is2analyse (fullinsname) == 1)
                chaininstance = addchain (chaininstance,loins);
        }
    }
    return chaininstance;
}

/******************************************************************************\
Construction de la figure destination.
Fonction de haut niveau : efg_buildlofig( headlist );
\******************************************************************************/

void efg_buildlofig( efg_fullsiglist *siglist )
{
  efg_createalllosig( siglist );
  efg_createallloins( siglist );
  efg_createallrc( siglist );
}

/******************************************************************************\
Prise en compte 'clean' des bug
\******************************************************************************/
void efg_exit( int index )
{
  avt_errmsg(EFG_ERRMSG, "004", AVT_FATAL, index);
/*  fflush( stdout );
  fprintf( stderr, "\n\nFatal error n° %d in efg module.\n",index );*/
  EXIT( 1 );
}
