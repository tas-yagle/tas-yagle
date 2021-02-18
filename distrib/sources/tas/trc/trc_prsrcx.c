/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_prcrcx.c                                                */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.101 $
Author   : $Author: fabrice $
Date     : $Date: 2008/11/28 15:31:42 $

*/

#include "trc.h"

#define RCX_PARSE_NOTHING 0x00
#define RCX_PARSE_RCN     0x01

#define RCX_PRSCTKLIST 0x52435805

#define	LINESIZE 8096
#define	WORDSIZE 1024

/*******************************************************************************
*                                                                              *
* rcxparsesignalparasitics()                                                   *
*                                                                              *
* Lecture du détail d'un signal.                                               *
* La première ligne lue doit être "beginparasitics", et la dernière            *
* "endparasitics".                                                             *
*                                                                              *
* mode = RCXPARSE_LOAD : le signal est lu normalement.                         *
* mode = RCXPARSE_CACHE : le signal est lu dans un contexte d'utilisation d'un *
*                         cache sur les RC.                                    *
* mode = RCXPARSE_MEMPOS : Le signal n'est pas lu, seule une information de    *
*                          mémorisation de son emplacement est crée.           *
* mode = RCXPARSE_SKIP :   Ignore le signal.                                   *
*                                                                              *
* Renvoie le nb d'éléments parasites lus.                                      *
*******************************************************************************/

unsigned long int
rcxparsesignalparasitics( FILE *datafile,       /* Le fichier.                */
                          char *filename,       /* Le nom du fich (parse err).*/
                          int *nbligne,         /* Le n° de ligne (parse err).*/
  
                          lofig_list *lofig,    /* La destination.            */
                          loins_list *ptins,
                          losig_list *ptsig,
                          long       *sigindex,

                          losig_list *pseudovss,
                          losig_list *pseudovssni,
                          char mode,            /* Mode de lecture            */
                          chain_list **siglist  /* La liste des signaux ajoutés
                                                   sur des ctc                */
                        )
{
  char	        *mot;
  chain_list    *ligne;
  chain_list    *pos;
  losig_list    *agresseur;
  int            status;
  int            thisctc;
  int            thisgnd;
  long		 n1,n2;
  float          resi, capa;
  loctc_list    *ctc;
  char          *agrname;
  MBK_OFFSET_MAX posfrombegin;
  char           skipalldetail;
  int            nbloadelem=0;
  char          *t1;
  float          netcapa=0.0;
  char           errbuf[2048], *signame;

  if( ptsig ) 
    signame = rcx_getsigname( ptsig );
  else
    signame = "(no signal)";

  if( lofig ) 
    sprintf( errbuf, "file %s figure %s signal %s", filename, lofig->NAME, signame );
  else
    sprintf( errbuf, "file %s instance %s signal %s", filename, ptins->INSNAME, signame );

  if( mode == RCXPARSE_MEMPOS ) {
    if( !mbkftell( datafile, &posfrombegin ) )
      avt_errmsg( TRC_ERRMSG, "034", AVT_FATAL, errbuf );
  }

  if( mode == RCXPARSE_CACHE ) {
    datafile = rcx_cache_getlofigfile( lofig );
    if( !datafile ) rcx_error( 51,NULL, AVT_FATAL );
    if( mbk_losig_getfilepos( lofig, 
                              ptsig, 
                              datafile, 
                              &posfrombegin, 
                              RCX_FLAGTAB 
                            ) == 0 ) return 0;
    if( !mbkfseek( datafile, &posfrombegin, SEEK_SET ) )
      avt_errmsg( TRC_ERRMSG, "035", AVT_FATAL, errbuf );
  }

  ligne = rcx_readline( datafile, &status );
  if( status )
    avt_errmsg( TRC_ERRMSG, "036", AVT_FATAL, errbuf );

  mot=(char*)ligne->DATA;
  pos = ligne->NEXT;

  if( strcasecmp( mot, "beginparasitics" )==0 ) {
    if( nbfield( ligne ) != 1 )
      fatalerror( filename, *nbligne );
  }
  else
    fatalerror( filename, *nbligne );
 
  (*nbligne)++;
  freechain( ligne );

  if( mode == RCXPARSE_MEMPOS ) {
    mbk_losig_setfilepos( lofig, 
                          ptsig, 
                          datafile, 
                          &posfrombegin, 
                          RCX_FLAGTAB, 
                          RCX_FLAGHEAP
                        );
    skipalldetail=YES;
  }
  else
    skipalldetail=NO;

  if( mode == RCXPARSE_SKIP )
    skipalldetail=YES;

  do {
  
    ligne = rcx_readline( datafile, &status );
    if( status )
      avt_errmsg( TRC_ERRMSG, "036", AVT_FATAL, errbuf );

    if( skipalldetail==NO ) {

      mot=(char*)ligne->DATA;
      pos = ligne->NEXT;

      if( strcasecmp( mot, "k" ) == 0 ) {

        thisgnd = 0;
        thisctc = 0;
        ctc     = NULL;

        switch( nbfield( ligne ) ) {
          case 4 : thisgnd=1; break;
          case 5 : thisctc=1; break;
          default : avt_errmsg( TRC_ERRMSG, "037", AVT_FATAL, errbuf );
        }

        if( thisctc > 0 ) {
           
          // ligne de la forme K n1 s2 n2 capa

          n1 = getint( (char*)pos->DATA, &status );
          pos = pos->NEXT;
          if(!status)
            fatalerror( filename, *nbligne );
          
          agrname = rcx_devect( (char*)pos->DATA ) ;
          pos = pos->NEXT;

          n2 = getint( (char*)pos->DATA, &status );
          pos = pos->NEXT;
          if(!status)
            fatalerror( filename, *nbligne );
          
          capa = getfloat( (char*)pos->DATA, &status );
          pos = pos->NEXT;
          if(!status)
            fatalerror( filename, *nbligne );

          if( ptins )
            agresseur = rcx_gethtrcxsig( ptins, NULL, agrname );
          else
            agresseur = rcx_gethtrcxsig( NULL, lofig, agrname );
    
          if( mode == RCXPARSE_CACHE ) {
            if( !agresseur ) {
              avt_errmsg( TRC_ERRMSG, "038", AVT_FATAL, agrname, errbuf );
            }
            if( rcn_cache_addable_ctc( ptsig, agresseur ) ) {
              if( ptsig == agresseur )
                avt_log(LOGTRC,2, "ctc on two same signal %s\n", 
                         rcx_getsigname( ptsig )
                       );
              ctc = addloctc( ptsig, n1, agresseur, n2, capa ) ;
              if( ctc ) 
                netcapa = netcapa + ctc->CAPA;
              nbloadelem+=RCN_SIZEOFLOCTC;
            }
          }
          else {
            if( !agresseur ) {
              if( ptins ) {
                agresseur = rcx_loinsaddlosig();
                agresseur->INDEX     = *sigindex;
                agresseur->NAMECHAIN = addchain( NULL, agrname );
                (*sigindex)++;
                rcx_addhtrcxsig( ptins, NULL, agresseur ) ;
                *siglist = addchain( *siglist, agresseur );
              }
              else {
                agresseur = addlosig( lofig, 
                                      *sigindex, 
                                      addchain( NULL, agrname ),
                                      INTERNAL
                                    );
                (*sigindex)++;
                rcx_addhtrcxsig( NULL, lofig, agresseur );
                *siglist = addchain( *siglist, agresseur );
              }

              agresseur->USER = addptype( agresseur->USER,
                                          RCX_UNRESOLVEDSIG, 
                                          NULL 
                                        );
              addlorcnet( agresseur );
            }

            if( getptype( agresseur->USER, RCX_UNRESOLVEDSIG ) ) {
              if( ptsig == agresseur )
                avt_log(LOGTRC,2, "ctc on two same signal %s\n", 
                         rcx_getsigname( ptsig )
                       );
              ctc = addloctc( ptsig, n1, agresseur, n2, capa ) ;
              if( ctc )
                netcapa = netcapa + ctc->CAPA;
              nbloadelem+=RCN_SIZEOFLOCTC;
            }
          }
        }
       
        if( thisgnd > 0 ) {
          // ligne de la forme K n1 G|N capa
          
          n1 = getint( (char*)pos->DATA, &status );
          pos = pos->NEXT;
          if(!status)
            fatalerror( filename, *nbligne );
         
          t1 = (char*)pos->DATA;
          pos = pos->NEXT;

          capa = getfloat( (char*)pos->DATA, &status );
          pos = pos->NEXT ;
          if(!status)
            fatalerror( filename, *nbligne );

          if( *t1 == RCXFILECAR_NI ) {
            if( ptsig == pseudovssni )
              avt_log(LOGTRC,2, "ctc on two same signal %s\n", 
                       rcx_getsigname( ptsig )
                     );
            ctc = addloctc( ptsig, n1, pseudovssni, (long)0, capa ) ;
            if( ctc )
              netcapa = netcapa + ctc->CAPA;
          }
          else {
            if( ptsig == pseudovss )
              avt_log(LOGTRC,2, "ctc on two same signal %s\n", 
                       rcx_getsigname( ptsig )
                     );
            ctc = addloctc( ptsig, n1, pseudovss, (long)0, capa ) ;
            if( ctc ) 
              netcapa = netcapa + ctc->CAPA;
          }

          nbloadelem+=RCN_SIZEOFLOCTC;
        }
      }
        
      if( strcasecmp(mot,"w") == 0 )
      {
        if(nbfield(ligne)!=5)  
          fatalerror(filename,*nbligne);

        n1=getint((char*)pos->DATA,&status);
        pos = pos->NEXT;
        if(!status)
          fatalerror(filename,*nbligne);

        n2=getint((char*)pos->DATA,&status);
        pos = pos->NEXT;
        if(!status)
          fatalerror(filename,*nbligne);

        resi=getfloat( (char*)pos->DATA, &status );
        pos = pos->NEXT;
        if(!status)
          fatalerror(filename,*nbligne);
    
        capa=getfloat( (char*)pos->DATA,&status);
        pos = pos->NEXT;
        if(!status)
          fatalerror(filename,*nbligne);

        addlowire( ptsig,
                   0,
                   resi,
                   capa,
                   n1,
                   n2
                 );
        netcapa = netcapa + capa;
        nbloadelem+=RCN_SIZEOFLOWIRE;
      }

    }
    (*nbligne)++;
    freechain( ligne );
  }
  while( strcasecmp( mot, "endparasitics") != 0 );

  if( mode == RCXPARSE_LOAD )
    rcn_addcapa( ptsig, netcapa );

  return nbloadelem;
}

/*******************************************************************************
*                                                                              *
* rcxparse(loins_list*,lofig_list *,char*)                                     *
*                                                                              *
* Lecture du fichier .rcx de l'instance si il existe. Si il n'existe pas ou    *
* si une erreur survient, la fonction renvoie la valeur 0. En cas de           *
* succes, cette fonction renvoie la valeur 1. Les erreures de parse provoquent *
* la terminaison du programme.                                                 *
*                                                                              *
*******************************************************************************/

int rcxparse( ptins, lofig, filename, loadspecified )
loins_list *ptins;
lofig_list *lofig ;
char       *filename;
ht         *loadspecified;
{
  FILE		*datafile;
  char           mot[WORDSIZE];
  chain_list    *ligne;
  chain_list    *pos;
  int		 status,nbligne,version;

  int 		 modele;
  chain_list    *chain ;

  char          *signame;
  rcx_list	*newrcx = NULL ;
  locon_list	*scancon;
  locon_list	*newcon;
  locon_list	*locon;
  char           dir = '\0' ;
  char          *devec;

  ptype_list	*pt, *ptl ;
 
  /* data for RCN model */
  lorcnet_list	*newrcnet;
  long		 n = 0l;
  long           sigindex;

  losig_list    *pseudovss;
  losig_list    *pseudovssni;
  losig_list    *ptsig;
  losig_list    *losig;
  char          *conname = NULL ;
  char           type ;
  char          *insname ;
  char          *figname ;
  chain_list    *siglist=NULL ;
  chain_list    *scanctc;
  loctc_list    *ctc;
  char           validflag;
  RCXFLOAT       capa[32];
  num_list      *headpnode, *scanpnode;
  num_list      *headpnodeup, *headpnodedn ;
  int            nbcapa ;
  char           errbuf[2048];

  if( (lofig && ptins) || (!lofig && !ptins) ) {
    rcx_error(54, losig, AVT_FATAL );
  }
  
  if( lofig ) 
    sprintf( errbuf, "file %s figure %s", filename, lofig->NAME );
  else
    sprintf( errbuf, "file %s instance %s", filename, ptins->INSNAME );

  version = 0 ;
  modele  = RCX_PARSE_NOTHING ;
  pseudovss = NULL ;
  sigindex = 1;
  
  if(lofig != NULL) {
 
    sigindex = getnumberoflosig( lofig )+1;
   
    pseudovss = rcx_getvss( lofig );

    if(pseudovss == NULL) {
      chain = addchain(NULL,mbk_getvssname()) ;
      pseudovss = addlosig(lofig, sigindex++, chain, EXTERNAL) ;
      addlorcnet( pseudovss );
      locon = addlocon(lofig, (char *)chain->DATA, pseudovss, IN) ;
      rcx_addhtrcxcon(NULL,lofig,locon) ;
      rcx_addhtrcxsig(NULL,lofig,pseudovss) ;
      tlc_setlosigalim( pseudovss );
      pseudovss->USER = addptype( pseudovss->USER, 
                                  LOFIGCHAIN,
                                  addchain( NULL, locon )
                                );
    }

    pseudovssni = rcx_getvssni( lofig );
    sigindex = getnumberoflosig( lofig )+1;
    if( !loadspecified ) {
      rcx_set_cache_lofig( lofig, 
                           filename, RCXFILEEXTENTION,
                           rcx_cache_parse, rcx_parse_free 
                         );
    }
  }
  else {
    ptins->USER = addptype( ptins->USER, RCX_UNRESOLVEDSIG, NULL );
    ptins->USER = addrcxpseudovss( ptins->USER, sigindex, &pseudovss);
    sigindex++;
    addlorcnet( pseudovss );
    pseudovssni = rcx_loinsgetvssni( ptins );
  }

  nbligne = 0 ;

  datafile = mbkfopen( filename, RCXFILEEXTENTION, "r" ) ;

  if(!datafile)
    return(0);


  if( lofig && rcx_isset_cache_lofig( lofig ) ) {
    rcx_cache_setlofigfile( lofig, datafile, filename );
  }

  do
  {
    mot[0]='\0';

    nbligne++;
    
    ligne = rcx_readline( datafile, &status );

    if( status )
    {
      avt_errmsg( TRC_ERRMSG, "036", AVT_FATAL, errbuf );
    }
    else
    {
      if(nbligne==1)
      {
        if(nbfield(ligne)!=3)
        {
          fatalerror(filename,nbligne);
        }

        if( strcmp((char*)ligne->DATA,"RCX")==0 &&
            strcmp((char*)ligne->NEXT->DATA,"VERSION")==0 )
        {
          version=getint((char*)ligne->NEXT->NEXT->DATA,&status);

          if(!status)
          {
            fatalerror(filename,nbligne);
          }
          if( version != 4 && version != 5 ) {
            avt_errmsg( TRC_ERRMSG, "013", AVT_FATAL, version );
          }
        }
        else
        {
          fatalerror(filename,nbligne);
        }
      }
      else
      {
        if(ligne)
        {
          strcpy(mot, (char*)ligne->DATA);
          pos = ligne->NEXT;

          if( strcmp(mot,"#") == 0 )
          {
            /* C'est un commentaire : On ne fait rien */
          }

          else if( strcasecmp( mot, "ENDEXTERNAL" ) == 0 ) {
            if( ptins )
            // Si on est en train de lire une instance pour la construction
            // hiérarchique, on s'arrête à la fin des signaux externes.
              break;
          }

          else if( strcasecmp(mot,"signal") == 0 )
          {
            char *namecon ;
            char *sigtype ;
            ptype_list *allcon=NULL, *pt;

            if( nbfield(ligne) < 4 )
            {
              fatalerror(filename,nbligne);
            }

            signame = (char*)pos->DATA;
            pos = pos->NEXT;
            
            namecon = rcx_devect( signame ) ; 
            allcon=addptype(allcon, 0, namecon);

            do
            {
              sigtype = (char*) pos->DATA ;
              if (sigtype[0]=='+') allcon=addptype(allcon, 0, namealloc(&sigtype[1]));
              pos = pos->NEXT;
            } while (sigtype[0]=='+');
            
            if(strcmp(sigtype,"INTERNAL") == 0) {
              type = INTERNAL ;
            }
            else if(strcmp(sigtype,"EXTERNAL") == 0) {
              type = EXTERNAL ;
              
              for (pt=allcon; pt!=NULL; pt=pt->NEXT)
              {
                if( ptins )
                  scancon = ptins->LOCON ;
                else
                  scancon = lofig->LOCON ;

                for( ; scancon ; scancon = scancon->NEXT )
                {
                  if( scancon->NAME == pt->DATA )
                    break;
                }
                pt->TYPE=(long)scancon;

                // Si on lit une instance, il faut impérativement qu'on ai un
                // locon externe.
                if(ptins && !scancon)
                  fatalerror( filename, nbligne );

                // Si on lit une figure, il n'y a pas de raisons pour qu'on ai
                // déjà un connecteur externe.
                if(lofig && scancon)
                  fatalerror( filename, nbligne );
              }
            }
            else
              fatalerror( filename, nbligne );

            allcon=(ptype_list *)reverse((chain_list *)allcon);
            for (pt=allcon; pt!=NULL; pt=pt->NEXT)
            {
              namecon=pt->DATA;
              scancon=(locon_list *)pt->TYPE;
              if( !loadspecified || 
                  gethtitem( loadspecified, namecon ) != EMPTYHT ) {

                // Ce signal est peut être déjà apparu lors de la lecture d'une
                // CTC. Dans ce cas, on enleve le ptype RCX_UNRESOLVED.
                
                if (pt==allcon)
                {
                  if( ptins )
                    ptsig = rcx_gethtrcxsig( ptins, NULL, namecon );
                  else
                    ptsig = rcx_gethtrcxsig( NULL, lofig, namecon );
                
                  if( ptsig ) {
                    ptl = getptype( ptsig->USER, RCX_UNRESOLVEDSIG );
                    if( !ptl ) 
                      avt_errmsg( TRC_ERRMSG, "039", AVT_FATAL, rcx_getsigname( ptsig ), errbuf  );
                    ptsig->USER = delptype( ptsig->USER, RCX_UNRESOLVEDSIG );
                  } 
                } 
                if(lofig) {
                
                  if (pt==allcon)
                  {
                    if( !ptsig ) {
                      ptsig = addlosig(lofig,sigindex++,addchain(NULL,namecon), type);
                      rcx_addhtrcxsig(NULL,lofig,ptsig) ;
                      siglist = addchain( siglist, ptsig );
                    }
                    else 
                      ptsig->TYPE = type;
                  }
                  if(type == EXTERNAL) {
                    scancon = addlocon(lofig, namecon , ptsig, UNKNOWN) ;
                    rcx_addhtrcxcon(NULL,lofig,scancon) ;
                  } else {
                    scancon = NULL ;
                  }
                } 
                else {
                
                  if (pt==allcon)
                  {
                    if( !ptsig ) {
                      ptsig = rcx_loinsaddlosig();
                      ptsig->NAMECHAIN = addchain( ptsig->NAMECHAIN, namecon );
                      rcx_addhtrcxsig( ptins, NULL, ptsig );
                      siglist = addchain( siglist, ptsig );
                      ptsig->INDEX = sigindex ;
                      sigindex++;
                    }
                  }
                  
                  scancon->USER = addptype( scancon->USER, PTYPERCX, ptsig );
                  if (pt==allcon) ptsig->USER   = addptype( ptsig->USER, RCX_LOCONINS, scancon );
                }

                if (pt!=allcon) ptsig->NAMECHAIN = append( ptsig->NAMECHAIN, addchain(NULL, namecon) );
                else newrcx = rcx_alloc( ptsig );
                
                if(ptins) {
                  locon_list *lc; 
                  
                  lc=addrcxlocon( );
                  lc->SIG = ptsig ;
                  lc->NAME = namecon ;
                  rcx_addhtrcxcon(ptins,NULL,lc) ;
                  newrcx->RCXEXTERNAL=append(newrcx->RCXEXTERNAL, addchain(NULL, lc));
                }
                else {
                  if( scancon )
                    newrcx->RCXEXTERNAL=append(newrcx->RCXEXTERNAL, addchain(NULL, scancon));
                }
                // ----------

                modele=RCX_PARSE_NOTHING;

                if (pt->NEXT==NULL)
                {
                  if( strcasecmp( (char*)pos->DATA,"rcn" )==0 ) {
                  
                    pos = pos->NEXT;
                    
                    modele=RCX_PARSE_RCN;
                    if( !ptsig->PRCN )
                      newrcnet=addlorcnet(ptsig);
                    newrcx->VIEW=newrcnet;
                    SETFLAG(newrcx->FLAG,MODELRCN);
                  }

                  if(modele==RCX_PARSE_NOTHING) { 
                    fatalerror(filename,nbligne);
                  }

                  /* Lecture des flags */
                  while( pos ) {
                    validflag='N';
                    if( strcmp( (char*)pos->DATA, "B" )==0 ) {
                      SETFLAG( newrcx->FLAG, RCXBREAKLOOP );
                      validflag='Y';
                    }
                    if( strcmp( (char*)pos->DATA, "G" )==0 ) {
                      SETFLAG( newrcx->FLAG, RCXNOCTC );
                      validflag='Y';
                    }
                    if( strcmp( (char*)pos->DATA, "W" )==0 ) {
                      SETFLAG( newrcx->FLAG, RCXNOWIRE );
                      validflag='Y';
                    }
                    if( strcmp( (char*)pos->DATA, "I" )==0 ) {
                      SETFLAG( newrcx->FLAG, RCXIGNORE );
                      validflag='Y';
                    }
                    if( validflag != 'Y' ) fatalerror(filename,nbligne);
                    pos = pos->NEXT;
                  }

                  if( lofig && rcx_isset_cache_lofig( lofig ) )
                    rcxparsesignalparasitics(  datafile,
                                               filename,
                                              &nbligne,
                                               lofig,
                                               ptins,
                                               ptsig,
                                              &sigindex,
                                               pseudovss,
                                               pseudovssni,
                                               RCXPARSE_MEMPOS,
                                               &siglist
                                             );
                  else
                    rcxparsesignalparasitics(  datafile,
                                               filename,
                                              &nbligne,
                                               lofig,
                                               ptins,
                                               ptsig,
                                              &sigindex,
                                               pseudovss,
                                               pseudovssni,
                                               RCXPARSE_LOAD,
                                               &siglist
                                             );
                }

              }
              else {
                if (pt->NEXT==NULL)
                rcxparsesignalparasitics(  datafile,
                                           filename,
                                          &nbligne,
                                           lofig,
                                           ptins,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           RCXPARSE_SKIP,
                                           NULL 
                                         );
                newrcx = NULL ;
                ptsig  = NULL ;
              }
            }
            freeptype(allcon);
          }
          else if( strcasecmp(mot,"c") == 0)
          {
            chain_list *cl;
            locon_list *lc;
            if( newrcx )
            {
              headpnode = NULL;
              headpnodeup = NULL ;
              headpnodedn = NULL ;
              if(modele==RCX_PARSE_RCN)
              {
                if(nbfield(ligne) < 6)
                  fatalerror(filename,nbligne);
               
                conname=rcx_devect( (char*)pos->DATA );
                pos = pos->NEXT;

                dir = *(char*)pos->DATA;
                pos = pos->NEXT;
            
                nbcapa = 0;
                do {
                  if( nbcapa >= 32 )
                    fatalerror(filename,nbligne);

                  capa[nbcapa]=getfloat((char*)pos->DATA, &status );
                  nbcapa++;
                  pos = pos->NEXT;
                  if(!status)
                    fatalerror(filename,nbligne);
                }
                while( strcmp((char*)pos->DATA,":") != 0 );
                pos = pos->NEXT ;

                while( pos ) {
                  n=getint((char*)pos->DATA, &status);
                  if(!status)
                    fatalerror(filename,nbligne);
                  headpnode = addnum( headpnode, n );
                  pos = pos->NEXT ;
                  if( pos && strcmp((char*)pos->DATA,":") == 0 )
                    break ;
                }
                if( pos ) {
                  pos = pos->NEXT ;
                  while( pos ) {
                    n=getint((char*)pos->DATA, &status);
                    if(!status)
                      fatalerror(filename,nbligne);
                    headpnodeup = addnum( headpnodeup, n );
                    pos = pos->NEXT ;
                    if( !pos ) fatalerror( filename, nbligne );
                    if( strcmp((char*)pos->DATA,":") == 0 )
                      break ;
                  }
                  pos = pos->NEXT ;
                  while( pos ) {
                    n=getint((char*)pos->DATA, &status);
                    if(!status)
                      fatalerror(filename,nbligne);
                    headpnodedn = addnum( headpnodedn, n );
                    pos = pos->NEXT ;
                    if( pos && strcmp((char*)pos->DATA,":") != 0 )
                      break ;
                  }
                }
              }

              for (cl=newrcx->RCXEXTERNAL; cl!=NULL && strcmp((lc=(locon_list *)cl->DATA)->NAME,conname)!=0; cl=cl->NEXT);
              if(/*(newrcx->RCXEXTERNAL != NULL) && */
                 cl!=NULL/*(strcmp(conname,newrcx->RCXEXTERNAL->NAME)==0)*/)
              {
                if(modele==RCX_PARSE_RCN) {
                  for( scanpnode = headpnode ; 
                       scanpnode ; 
                       scanpnode = scanpnode->NEXT 
                     )
                    setloconnode(lc,scanpnode->DATA);
                  if( headpnodeup && headpnodedn ) {
                    rcx_setnodebytransition( lc, TRC_SLOPE_UP,   headpnodeup );
                    rcx_setnodebytransition( lc, TRC_SLOPE_DOWN, headpnodedn );
                  }
                }

                rcx_parselocon( lofig, ptins, lc/*newrcx->RCXEXTERNAL*/, capa, nbcapa, version, filename, nbligne );
              }
              else
              {
                devec = rcx_devect(conname);
                newcon = addrcxlocon();
                
                if(ptins != NULL)
                  newcon->NAME = concatname( rcx_getinsname(ptins), devec );
                else
                  newcon->NAME=devec;

                newcon->SIG = ptsig;

                addloconrcxname(newcon,newcon->NAME);

                if(ptins != NULL)
                  rcx_addhtrcxcon(ptins,NULL,newcon) ;
                else
                  rcx_addhtrcxcon(NULL,lofig,newcon) ;

                newrcx->RCXINTERNAL = addchain(newrcx->RCXINTERNAL,newcon);

                if(modele==RCX_PARSE_RCN) {
                  for( scanpnode = headpnode ; 
                       scanpnode ; 
                       scanpnode = scanpnode->NEXT 
                     )
                    setloconnode(newcon,scanpnode->DATA);
                  if( headpnodeup && headpnodedn ) {
                    rcx_setnodebytransition( newcon, TRC_SLOPE_UP,   headpnodeup );
                    rcx_setnodebytransition( newcon, TRC_SLOPE_DOWN, headpnodedn );
                  }
                }

                rcx_parselocon( lofig, ptins, newcon, capa, nbcapa, version, filename, nbligne );

                trc_addlocondir(newcon,dir) ;
                /* note : les connecteurs internes sont ceux dont on a qu'un
                          exemplaire car la résistance interne des cellules leur
                          correspondant est négligée. */
              }
              freenum(headpnodeup);
              freenum(headpnodedn);
              freenum( headpnode );
            }
          }
          
          else if( strcasecmp(mot,"driverup") == 0 )
          {
            if( newrcx && ptsig ) {
              headpnode = NULL ;
              while( pos ) {
                n=getint((char*)pos->DATA, &status);
                if(!status)
                  fatalerror(filename,nbligne);
                headpnode = addnum( headpnode, n );
                pos = pos->NEXT ;
              }
              headpnode = (num_list*)reverse( (chain_list*)headpnode );
              rcx_setdriver( ptsig, headpnode, NULL );
            }
          }
          
          else if( strcasecmp(mot,"driverdown") == 0 )
          {
            if( newrcx && ptsig ) {
              headpnode = NULL ;
              while( pos ) {
                n=getint((char*)pos->DATA, &status);
                if(!status)
                  fatalerror(filename,nbligne);
                headpnode = addnum( headpnode, n );
                pos = pos->NEXT ;
              }
              headpnode = (num_list*)reverse( (chain_list*)headpnode );
              rcx_setdriver( ptsig, NULL, headpnode );
            }
          }
          
          else if( strcasecmp(mot,"bellow") == 0 ) {
            if( !loadspecified ) {
            if( nbfield( ligne ) != 3 )
              fatalerror( filename, nbligne );
            
            signame = rcx_devect( (char*)pos->DATA );
            pos = pos->NEXT;

            ptsig = rcx_gethtrcxsig( NULL, lofig, signame );

            if( ! ptsig ) {
              ptsig = addlosig( lofig,
                                sigindex++,
                                addchain( NULL, signame ),
                                INTERNAL
                              ) ;
              rcx_addhtrcxsig( NULL, lofig, ptsig );
              siglist = addchain( siglist, ptsig );
              addlorcnet( ptsig );
            }
            else {
              ptl = getptype( ptsig->USER, RCX_UNRESOLVEDSIG );
              if( !ptl ) 
                rcx_error( 15, ptsig, AVT_ERROR );
              else
                ptsig->USER = delptype( ptsig->USER, RCX_UNRESOLVEDSIG );
            }
            rcx_addbellow( ptsig, rcx_devect( (char*)pos->DATA ) );
            }
          }

          else if( strcasecmp( mot, "INSTANCE" ) == 0 ) {
            if( !loadspecified )
            {
              if( nbfield( ligne ) != 4 )
                fatalerror( filename, nbligne );
              if( lofig ) {
                insname = namealloc( (char*)pos->DATA ) ;
                pos = pos->NEXT->NEXT ;
                figname = namealloc( (char*)pos->DATA ) ;
                rcx_addhtrcxmod( lofig, insname, figname ) ;
                ptl = getptype( lofig->USER, RCX_LISTINSNAME );
                if( !ptl ) {
                  lofig->USER = addptype( lofig->USER, RCX_LISTINSNAME, NULL );
                  ptl = lofig->USER;
                }
                ptl->DATA = (void*)addchain( (chain_list*)(ptl->DATA), insname );
              }
            }
          }

          else if( strcasecmp( mot, "ORIGIN" ) == 0 ) {
            if( ptsig )
            {
              if( nbfield( ligne ) != 2 )
                fatalerror( filename, nbligne );

              if( lofig ) {
                signame = namealloc( (char*)pos->DATA ) ;
                pos = pos->NEXT;
                rcx_addorigin( lofig, ptsig, signame );
              }
            }
          }

          else if( strcasecmp( mot, "EOF" ) != 0 ) {
            fatalerror( filename, nbligne );
          }
          
        }
      }
    }

    freechain( ligne );
    ligne = NULL;
  }
  while( strcmp( mot, "EOF" ) );

  if( ligne ) {
    freechain( ligne );
    ligne = NULL;
  }

  if( !lofig || !rcx_isset_cache_lofig( lofig ) )
    fclose(datafile);

  /* Post traitement : Création de la liste des signaux non résolu et mise
     à 0 des noeud des CTC pour lesquels cette information n'est pas
     significative. */

  if( ptins )
    ptl = getptype( ptins->USER, RCX_UNRESOLVEDSIG );
  else
    ptl = NULL;
    
  for( chain = siglist ; chain ; chain = chain->NEXT ) {
  
    losig = (losig_list*)chain->DATA;

    if( losig->PRCN ) {
      if( getptype( losig->USER, RCX_UNRESOLVEDSIG ) ||
          rcx_isbellow( losig )                         ) {
        for( scanctc = losig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
          ctc = (loctc_list*)scanctc->DATA;
          if( ctc->SIG1 == losig ) ctc->NODE1 = 0;
          if( ctc->SIG2 == losig ) ctc->NODE2 = 0;
        }
      }
    }
    
    if( getptype( losig->USER, RCX_UNRESOLVEDSIG ) && !loadspecified ) {
    
      if( lofig ) {
        avt_errmsg( TRC_ERRMSG, "040", AVT_FATAL, rcx_getsigname( losig ), errbuf );
      }
        
      ptl->DATA = (void*)( addchain( (chain_list*)ptl->DATA, (void*)losig ) );
    }

  }

  freechain( siglist );

  /* Inverse le champ LOCON_NODE des connecteurs rcx->EXTERNAL */

  if(ptins != NULL)
    scancon = ptins->LOCON ;
  else
    scancon = lofig->LOCON ;
  
  for(;scancon;scancon=scancon->NEXT)
  {
    losig = NULL ;

    if(ptins != NULL) {
      pt = getptype(scancon->USER,PTYPERCX) ;
      if( pt ) {
        losig = (losig_list*)pt->DATA ;
      }
    }
    else {
      losig = scancon->SIG ;
    }

  }

  /* Met à jour le lofigchain quand on est en mode lofig */

  if( lofig ) {
    for( ptsig = lofig->LOSIG ; ptsig ; ptsig = ptsig->NEXT ) {
      newrcx = getrcx( ptsig );
      if( newrcx ) {
        ptsig->USER = addptype( ptsig->USER, LOFIGCHAIN, NULL );
        ptl = ptsig->USER;
        for( chain = newrcx->RCXEXTERNAL ; chain ; chain = chain->NEXT )
          ptl->DATA = addchain( (chain_list*)ptl->DATA, 
                                (locon_list*)chain->DATA 
                              );
        /*if( newrcx->RCXEXTERNAL )
          ptl->DATA = addchain( (chain_list*)ptl->DATA, newrcx->RCXEXTERNAL );*/
        for( chain = newrcx->RCXINTERNAL ; chain ; chain = chain->NEXT )
          ptl->DATA = addchain( (chain_list*)ptl->DATA, 
                                (locon_list*)chain->DATA 
                              );

      }
    }
  }

  return(1);
}



/*******************************************************************************
*                                                                              *
* readline(FILE*,int*)                                                         *
*                                                                              *
* Lecture d'une ligne dans le fichier FILE. La valeur de retour est un         *
* pointeur vers une chaine de caractres statique (NE PAS ESSAYER DE LA         *
* LIBERER. En cas d'erreur, cette fonction renvoie NULL et status contient     *
* le code d'erreur :                                                           *
*            1 Ligne trop longue.                                              *
*            2 La fin de fichier est atteinte avant un retour à la ligne.      *
*            3 Le fichier n'est pas un fichier texte.                          *
* Le caractère \n de fin de ligne est remplacé par 0.                          *
*                                                                              *
*******************************************************************************/

static char rcx_buffer[LINESIZE];

chain_list* rcx_readline(ptfile,status)
FILE	*ptfile;
int	*status;
{
  int p,c;
  chain_list *head;
  char n;
  head = NULL;

  p=0;

  if(fgets(rcx_buffer,LINESIZE,ptfile) == NULL)
   {
    avt_errmsg( TRC_ERRMSG, "014", AVT_FATAL );
   }

  n=0;
  do
  {
    if( rcx_buffer[p] == '\n' )
      rcx_buffer[p] = 0;

    c = rcx_buffer[p];

    if( c != 0 && c != EOF )
    {
      if(c<32 || c>126)
      {
        *status=3;
        freechain( head );
        return(NULL);
      }
    }
    if(c==EOF)
    {
      *status=2;
      freechain( head );
      return(NULL);
    }

    if( c != ' ' && c != 0 && n == 0 ) {
      head = addchain( head, rcx_buffer+p );
      n=1;
    }
    else if( c == ' ' && n == 1 ) {
      n=0;
      rcx_buffer[p]='\0';
    }
    p++;
  }
  while( c!=0 && c!=EOF );

  rcx_buffer[p-1]='\0';
  *status=0;
  return(reverse( head ));
}

/*******************************************************************************
*                                                                              *
* nbfield(char*)                                                               *
*                                                                              *
* Compte le nombre de champs dans une ligne. Les champs sont matérialisés par  *
* un ou plusieurs espaces consécutifs.                                         *
*                                                                              *
*******************************************************************************/


int nbfield(ligne)
chain_list *ligne;
{
  int num=0;

  while( ligne ) {
    num++;
    ligne = ligne->NEXT;
  }

  return(num);
}

/*******************************************************************************
*                                                                              *
* float getfloat(int,ligne,status);                                            *
*                                                                              *
* Renvoie le nombre en virgule flottante se trouvant à la position nb de ligne *
* La valeur status contient :                                                  *
*       1 Tout c'est bien passé                                                *
*       0 La chaîne ne représente pas un nombre.                               *
*                                                                              *
*******************************************************************************/

float getfloat(field,status)
char            *field;
int		*status;
{
  double	 value;
  char		*endfield;

  endfield=NULL;

  value=strtod(field,&endfield);  

  if(field!=(char*)endfield && (*endfield==' ' || *endfield==0) )
    *status=1;
  else
    *status=0;

  return((float)value);
}

/*******************************************************************************
*                                                                              *
* long  getint(int,ligne,status);                                              *
*                                                                              *
* Renvoie le nombre entier se trouvant à la position nb de ligne               *
* La valeur status contient :                                                  *
*       1 Tout c'est bien passé                                                *
*       0 La chaîne ne représente pas un nombre.                               *
*                                                                              *
*******************************************************************************/

long getint(field,status)
char            *field;
int		*status;
{
  long int	 value;
  char		*endfield;

  endfield=NULL;
  value=strtol(field,&endfield,10);  

  if(field!=(char*)endfield && (*endfield==' ' || *endfield==0) )
    *status=1;
  else
    *status=0;

  return(value);
}

void trc_addlocondir( locon, dir )
locon_list *locon ;
char        dir ;
{
  switch(dir)
  {
    case IN :
      locon->DIRECTION = IN ;
      break ;
    case OUT :
      locon->DIRECTION = OUT ;
      break ;
    case INOUT :
      locon->DIRECTION = INOUT ;
      break ;
    case UNKNOWN :
      locon->DIRECTION = UNKNOWN ;
      break ;
    case TRISTATE :
      locon->DIRECTION = TRISTATE ;
      break ;
    case TRANSCV :
      locon->DIRECTION = TRANSCV ;
      break ;
    default :
      locon->DIRECTION = UNKNOWN ;
      break ;
  }
}

char* rcx_devect( nom )
char            *nom;
{
  int           taille;
  int           i;
  int           j;
  char          nouv[1024];
  int           fin;
  int           modif;
  
  taille = strlen( nom );
  if( taille == 0 )
  {
    rcx_error(17,NULL,AVT_FATAL);
  }

  fin   = taille - 1 ;
  modif = 0;

  if( nom[fin] == '}' )
  { 
    /* Retrouve le crochet ouvrant */
    for( i = fin-1 ; i >= 0 && isdigit( (int)nom[i] ) ; i-- );
    
    if( nom[i] == '{' )
    {
      if( !(i == 0 || i == fin - 1) )
      {
        /* bon vecteur : toto[32] */
        for( j = 0 ; j < fin ; j++ )
          if( i != j )
            nouv[j] = nom[j];
          else
            nouv[i] = ' ';
        nouv[j] = '\0';

        modif = 1;
      }
    }
  }
 
  if( ! modif )
    strcpy( nouv, nom );

  return(namealloc(nouv));
}

/*******************************************************************************
*                                                                              *
* fatalerror()                                                                 *
* Met en forme et affiche un message d'erreur lorsqu'une erreur de parse du    *
* fichier .rcx est survenue.                                                   *
*                                                                              *
*******************************************************************************/

void fatalerror(filename,lgn)
char *filename;
int lgn;
{
  avt_errmsg( TRC_ERRMSG, "015", AVT_FATAL, lgn, filename, RCXFILEEXTENTION );
}

void rcx_parse_free( lofig_list *lofig )
{
  FILE *file=NULL;

  mbk_losig_clearfilepos( lofig, file, RCX_FLAGTAB, RCX_FLAGHEAP );
  rcx_cache_clearlofigfile( lofig );
}

/******************************************************************************\
Fonction appellée à partir de buildrcx. Le fichier a été drivé, et les offset
des signaux ont été mémorisé. Il faut juste mettre en place le mécanisme de
cache. 
\******************************************************************************/
void rcx_reload( lofig_list *lofig, char fileacces )
{
  FILE       *file;

  rcx_set_cache_lofig( lofig, 
                       lofig->NAME, RCXFILEEXTENTION, 
                       rcx_cache_parse, rcx_parse_free 
                     );
  file = mbkfopen_ext( lofig->NAME, RCXFILEEXTENTION, "r", fileacces, 1 ) ;
  rcx_cache_setlofigfile( lofig, file, lofig->NAME );
}

void rcx_parselocon( lofig_list *lofig, loins_list *loins, locon_list *locon, RCXFLOAT *capa, int nbcapa, int version, char *filename, int nbligne )
{
  if( version == 4 ) {
    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    
    case ELP_CAPA_LEVEL0 :
      switch( nbcapa ) {
      case 1 :
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_HALF, capa[0], capa[0] );
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_END,  capa[0], capa[0] );
        break ;
      case 2 :
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_HALF, capa[0], capa[1] );
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_END,  capa[0], capa[1] );
        break ;
      default :
        fatalerror(filename,nbligne);
      }
      break ;

    case ELP_CAPA_LEVEL1 :
      switch( nbcapa ) {
      case 4 :
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[0], capa[1], capa[1]
                           );
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_END,
                             capa[2], capa[2], capa[3], capa[3]
                           );
        break ;
      case 8 :
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[1], capa[2], capa[3]
                           );
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_END,
                             capa[4], capa[5], capa[6], capa[7]
                           );
        break ;
      default :
        fatalerror(filename,nbligne);
      }
      break ;
    
    case ELP_CAPA_LEVEL2 :
      switch( nbcapa ) {
      case 6 :
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[1], capa[1], capa[2],
                             capa[3], capa[4], capa[4], capa[5]
                           );
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_END,
                             -1.0, capa[1], capa[1], -1.0,
                             -1.0, capa[4], capa[4], -1.0
                           );
        break ;
      case 8 :
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[1], capa[2], capa[3],
                             capa[4], capa[5], capa[6], capa[7]
                           );
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_HALF,
                             -1.0, capa[1], capa[2], -1.0,
                             -1.0, capa[5], capa[6], -1.0
                           );
        break ;
      default :
        fatalerror(filename,nbligne);
      }
      break ;

    default :
      fatalerror(filename,nbligne);
    }
  }
  else {
    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    
    case ELP_CAPA_LEVEL0 :
      switch( nbcapa ) {
      case 2 :
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_HALF, capa[0], capa[0] );
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_END,  capa[1], capa[1] );
        break ;
      case 4 :
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_HALF, capa[0], capa[1] );
        rcx_setloconcapa_l0( lofig, loins, locon, TRC_END,  capa[2], capa[3] );
        break ;
      default :
        fatalerror(filename,nbligne);
      }
      break ;

    case ELP_CAPA_LEVEL1 :
      switch( nbcapa ) {
      case 4 :
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[0], capa[1], capa[1]
                           );
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_END,
                             capa[2], capa[2], capa[3], capa[3]
                           );
        break ;
      case 8 :
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[1], capa[2], capa[3]
                           );
        rcx_setloconcapa_l1( lofig, loins, locon, TRC_END,
                             capa[4], capa[5], capa[6], capa[7]
                           );
        break ;
      default :
        fatalerror(filename,nbligne);
      }
      break ;
    
    case ELP_CAPA_LEVEL2 :
      switch( nbcapa ) {
      case 8 :
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[1], capa[1], capa[2],
                             capa[3], capa[4], capa[4], capa[5]
                           );
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_END,
                             -1.0, capa[6], capa[6], -1.0,
                             -1.0, capa[7], capa[7], -1.0
                           );
        break ;
      case 12 :
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_HALF,
                             capa[0], capa[1], capa[2], capa[3],
                             capa[4], capa[5], capa[6], capa[7]
                           );
        rcx_setloconcapa_l2( lofig, loins, locon, TRC_HALF,
                             -1.0, capa[8], capa[9], -1.0,
                             -1.0, capa[10], capa[11], -1.0
                           );
        break ;
      default :
        fatalerror(filename,nbligne);
      }
      break ;

    default :
      fatalerror(filename,nbligne);
    }
  }
}
