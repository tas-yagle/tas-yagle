/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*******************************************************************************
*                                                                              *
*  Tool        : Spice parser / driver v 7.00                                  *
*  Author(s)   : Gregoire AVOT                                                 *
*  Updates     : August, 17th 1998                                             *
*                                                                              *
*******************************************************************************/

#define SPI_MAX_COL 80

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include AVT_H
#include MUT_H
#include MLO_H
#include RCN_H
#include EQT_H
#include MSL_H
#include MCC_H
#include "spi_drive.h"
#include "spi_int.h"

/*static chain_list    *TNMOS ;
static chain_list    *TPMOS ;*/
extern char          *SPI_NETNAME;
int                   SPI_DRV_TRSPARAM=1;
char                  SPI_NAMENODES;
char                  SPI_COMPOSE_NAME_SEPAR;
char                  SPI_INS_SEPAR='.';
char                  SPI_REPLACE_INS_SEPAR='_';
int SPI_LOWRESISTANCE_AS_TENSION=0;

float CAPAMINI = 0;
float RESIMINI = 0.001;

#define SPI_NONODES (-1l)
#define SPI_MAXSTATICNAME 16


chain_list *UNIT_LIST=NULL;

#define SPI_FULL_DRIVE 1
#define SPI_DRIVE_ALL 2
static int SPI_FLAGS=0;

static mbk_options_pack_struct spice_opack[]=
  {
    {"DriveInstanceParameters", SPI_FULL_DRIVE},
    {"IgnoreGlobalParameters", SPI_DRIVE_ALL}
  };

void (*SPI_LOCON_INODE_FUNC)( FILE *ptf, lofig_list *lofig, void *data ) = NULL;
void *SPI_LOCON_INODE_DATA = NULL;

void spi_cleanextlocon( lofig_list *lofig )
{
  locon_list *locon ;
  ptype_list *ptl ;
  losig_list *losig ;

  for( locon = lofig->LOCON ; locon ; locon = locon->NEXT ) {
    losig = locon->SIG ;
    ptl = getptype( losig->USER, SPI_EXTLOCON );
    if( ptl )
      losig->USER = delptype( losig->USER, SPI_EXTLOCON );
  }
}

locon_list* spi_getextlocon( losig_list *losig )
{
  ptype_list *ptl ;
  chain_list *scan;
  locon_list *locon;
  
  if( losig->TYPE != 'E' )
    return NULL;

  ptl = getptype( losig->USER, SPI_EXTLOCON );
  if( ptl )
    return (locon_list*)ptl->DATA;

  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( !ptl )
    return NULL;

  for( scan = (chain_list*) ptl->DATA ; scan ; scan = scan->NEXT ) {
    locon = (locon_list*) scan->DATA ;
    if( locon->TYPE == 'E' ) 
      break;
  }

  if( !scan )
    locon = NULL;

  losig->USER = addptype( losig->USER, SPI_EXTLOCON, (void*)locon );
  return locon;
}

/******************************************************************************\
spi_makename()
Renvoie un pointeur sur un buffer statique contenant un nom compatible avec
Spice, c'est à dire sans caractère séparateur.
\******************************************************************************/
char* spi_makename( char *alc_name )
{
  int            i; 
  int            l;
  static char    spi_name[1024];

  l = strlen( alc_name );
  for( i=0 ; i < l ; i++ ) {
    if( alc_name[i] == SEPAR )
      spi_name[i] = SPI_REPLACE_INS_SEPAR;
    else
      spi_name[i] = alc_name[i];
  }
  spi_name[l]='\0';

  return spi_name;
}

char*           spinamenode( losig, node )
losig_list      *losig;
long            node;
{
  return spinamenodedetail( losig, node, 0 );
}

char*           spinamenodedetail( losig, node, force )
losig_list      *losig;
long            node;
char            force;
{
  char           tmp[1024];
  static char    names[SPI_MAXSTATICNAME][1024];
  static int     curnames = 0;
  locon_list    *locon;
  ptype_list    *ptptype;
  convindex     *cvx;

  spi_env();

  curnames++;
  if( curnames == SPI_MAXSTATICNAME )
    curnames=0;

  if( SPI_NAMENODES == TRUE ) {
    if( ( node == SPI_NONODES || losig->PRCN->NBNODE<=2 ) && force==0 ) {
      if( losig->NAMECHAIN ) {
        strcpy( tmp, spi_makename( getsigname(losig) ));
        spi_vect( tmp );
        sprintf( names[curnames], "%s", tmp );
      }
      else
        sprintf( names[curnames], "sig%ld", losig->INDEX );
    }
    else {
      strcpy( tmp, spi_makename( getsigname(losig) ) );
      spi_vect( tmp );
      if( losig->TYPE != 'E' ) {
        // On choisi un noeud pour driver un nom "court". On en prend un sur
        // sur le premier connecteur sur le signal.
        locon=spichooseonelocon( losig );
        if( locon && locon->PNODE->DATA == node )
          sprintf( names[curnames], "%s", tmp );
        else {
          sprintf( names[curnames], 
                   "%s%c%ld", 
                   tmp, 
                   SPI_COMPOSE_NAME_SEPAR, 
                   node 
                 );
        }
      }
      else { // signal externe : on ne change pas le noeud sur l'interface.
        locon = spi_getextlocon( losig );
        if( locon && locon->PNODE && locon->PNODE->DATA==node ) {
          sprintf( names[curnames], "%s", tmp );
        }
        else {
          sprintf( names[curnames], 
                   "%s%c%ld", 
                   tmp, 
                   SPI_COMPOSE_NAME_SEPAR, 
                   node 
                 );
        }
      }
    }
  }
  else {
    ptptype = getptype( losig->USER, SPI_DRIVER_PTYPE );
    cvx     = (convindex*)(ptptype->DATA);
    if( node == SPI_NONODES )
      sprintf( names[curnames], "%d", cvx->premier );
    else
      sprintf( names[curnames], "%ld", cvx->premier + node - 1 );
  }

  return( names[curnames] );
}

char *spi_gettransname(type)
char type ;
{
  chain_list *chain ;
 
  if(MLO_IS_TRANSN(type))
    chain = TNMOS ;
  else if (MLO_IS_TRANSP(type))
    chain = TPMOS ;
  else
    chain = TNMOS ;
    
  if (!chain) avt_errmsg(SPI_ERRMSG, "081", AVT_FATAL);
   
  return((char *)chain->DATA) ;
}

int getanode(losig_list *signal)
{
  if (signal->PRCN==NULL) return 0;
  if (signal->PRCN->PWIRE!=NULL) return signal->PRCN->PWIRE->NODE1;
  if (signal->PRCN->PCTC!=NULL)
    {
      loctc_list *tmp=(loctc_list *)signal->PRCN->PCTC->DATA;
      if (tmp->SIG1==signal) return tmp->NODE1;
      if (tmp->SIG2==signal) return tmp->NODE2;
    }
  return 0;
}

void		cherche_alim( ptfig, vdd, vss )
lofig_list	*ptfig;
char		**vdd;
char		**vss;
{
  locon_list	*scancon;
  losig_list	*signal;
  int node;
  static char    stvss[1024], stvdd[1024];

  *vdd = NULL;
  *vss = NULL;

  for( scancon = ptfig->LOCON ;
       scancon && !(*vdd && *vss) ;
       scancon = scancon->NEXT
     )
  {
    if( mbk_LosigIsVDD( scancon->SIG ) )
    {
      signal  = scancon->SIG;

      if( scancon->PNODE )
        strcpy( stvdd, spinamenode( signal, scancon->PNODE->DATA ) );
      else
        strcpy( stvdd, spinamenode( signal, SPI_NONODES ) );

      *vdd = stvdd;
    }

    if( mbk_LosigIsVSS( scancon->SIG ) )
    {
      signal  = scancon->SIG;

      if( scancon->PNODE )
        strcpy( stvss, spinamenode( signal, scancon->PNODE->DATA ) );
      else
        strcpy( stvss, spinamenode( signal, SPI_NONODES ) );

      *vss = stvss;
    }
  }

  // zinaps le 20/8/2002
  for( signal = ptfig->LOSIG ;
       signal && !(*vdd && *vss) ;
       signal = signal->NEXT
     )
  {
    if(mbk_LosigIsVDD(signal))
    {
      if((node=getanode(signal))!=0) 
        strcpy( stvdd, spinamenode( signal, node ) );
      else
        strcpy( stvdd, spinamenode( signal, SPI_NONODES ) );
      *vdd = stvdd;
    }

    if(mbk_LosigIsVSS(signal))
    {
      if((node=getanode(signal))!=0) 
        strcpy( stvss, spinamenode( signal, node ) );
      else
        strcpy( stvss, spinamenode( signal, SPI_NONODES ) );
      *vss = stvss;
    }
  }
}

void            sortrcn( ptfig, df, vss )
lofig_list	*ptfig;
FILE		*df;
char 		*vss;
{
  losig_list	*scanlosig;
  int		 nbr;
  lowire_list	*scanlowire;
  int            nbctc, cnt;
//  chain_list    *scanchain;
  chain_list    *headctc;
  loctc_list    *ptctc;
  char *name1, *name2;
  float resival;
  lonode_list *ptnode1;
  lonode_list *ptnode2;
  static char drivewirenotconnected='u';
  char *env;
  ptype_list *pt;
  char siname[1024];
  int cachemode;

  if( drivewirenotconnected=='u' ) {

    /* variable temporaire permettant de contourner le bug de sim qui
       drive des netlists avec des noeuds connectés à un seul wire, ce
       qui n'est pas supporté par titan. solution bourrin car ca ne prend
       pas en compte un ensemble de résistances arrivant nul part. */

    env = NULL; //getenv("MBK_SPI_DRIVE_WIRE_NOT_CONNECTED");
    if( env ) {
      if( strcasecmp( env, "yes" )==0 )
        drivewirenotconnected = 'y';
      if( strcasecmp( env, "no" )==0 )
        drivewirenotconnected = 'n';
    }
    if( drivewirenotconnected == 'u' )
      drivewirenotconnected = 'y' ; // défaut.
  }

  nbctc = 0;
  cachemode=rcn_hascache(ptfig);

  for( scanlosig = ptfig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
  {
    if( !scanlosig->PRCN )
      continue;

    if (cachemode && (mbk_LosigIsVDD(scanlosig) || mbk_LosigIsVSS(scanlosig))) continue;

    rcn_lock_signal(ptfig, scanlosig);
   
    if( scanlosig->PRCN->PWIRE || scanlosig->PRCN->PCTC )
    {
      nbr = 1;


      for( scanlowire = scanlosig->PRCN->PWIRE ;
           scanlowire ;
           scanlowire = scanlowire->NEXT )
      {
        resival=scanlowire->RESI;

        if( drivewirenotconnected == 'n' ) {
          ptnode1 = getlonode( scanlosig, scanlowire->NODE1 );
          ptnode2 = getlonode( scanlosig, scanlowire->NODE2 );

          /* Teste si cette resistance est connectée à quelque chose. */
          if(  RCN_GETFLAG( ptnode1->FLAG, RCN_FLAG_ONE )   &&
              !RCN_GETFLAG( ptnode1->FLAG, RCN_FLAG_LOCON ) &&
               scanlowire->CAPA == 0.0                      &&
              !ptnode1->CTCLIST
            )
            continue ;
          if(  RCN_GETFLAG( ptnode2->FLAG, RCN_FLAG_ONE )   &&
              !RCN_GETFLAG( ptnode2->FLAG, RCN_FLAG_LOCON ) &&
               scanlowire->CAPA == 0.0                      &&
              !ptnode2->CTCLIST
            )
            continue ;
        }    

        if (resival<RESIMINI) resival=RESIMINI;
        if (SPI_LOWRESISTANCE_AS_TENSION==0)
          {
            if ((pt=getptype(scanlowire->USER, RESINAME))!=NULL)
              strcpy(siname, spi_makename((char *)pt->DATA));
            else
              sprintf(siname, "%ld_%d",scanlosig->INDEX, nbr);
            spi_print( df,
                       "R%s %s %s %g\n",
                       siname,
                       spinamenode( scanlosig, scanlowire->NODE1 ),
                       spinamenode( scanlosig, scanlowire->NODE2 ),
                       resival
                       );
          }
        else 
          { 
            spi_print( df,
                       "V%d_%d %s %s 0\n",
                       scanlosig->INDEX,
                       nbr,
                       spinamenode( scanlosig, scanlowire->NODE1 ),
                       spinamenode( scanlosig, scanlowire->NODE2 )
                       );
          }

	/* HSpice ne supporte pas 1e-6P : Il ne prendra pas en compte le P, et
	 * on aura 1 micron au lieu de 1e-18. */

        if( scanlowire->CAPA!=0 && scanlowire->CAPA / 2.0 >= CAPAMINI )
        {
          spi_print( df,
                    "C%d_%d1 %s %s %g\n",
                    scanlosig->INDEX,
                    nbr,
                    spinamenode( scanlosig, scanlowire->NODE1 ),
                    "0"/*vss*/,
                    (scanlowire->CAPA / 2.0) * 1e-12
                  );

          spi_print( df,
                    "C%d_%d2 %s %s %g\n",
                    scanlosig->INDEX,
                    nbr,
                    spinamenode( scanlosig, scanlowire->NODE2 ),
                    "0"/*vss*/,
                    (scanlowire->CAPA / 2.0) * 1e-12
                  );
        }

        nbr++;
      }
      if (nbr > 1) 
        spi_print( df, "\n" );

      for (headctc=scanlosig->PRCN->PCTC, cnt=0; headctc!=NULL; headctc=headctc->NEXT)
        {
          ptctc=(loctc_list *)headctc->DATA;
          if (rcn_capacitancetooutput(cachemode, scanlosig, ptctc))
            {
              if( ptctc->CAPA >= CAPAMINI )
                {
                  name1 = spinamenode( ptctc->SIG1, ptctc->NODE1 > 0 ? ptctc->NODE1 : SPI_NONODES );
                  name2 = spinamenode( ptctc->SIG2, ptctc->NODE2 > 0 ? ptctc->NODE2 : SPI_NONODES );

                  if (mbk_LosigIsVSS( ptctc->SIG1 ) || mbk_LosigIsVSS( ptctc->SIG2 ) )
                  {
                    if ((pt=getptype(ptctc->USER, MSL_CAPANAME))!=NULL)
                      strcpy(siname, spi_makename((char *)pt->DATA));
                    else
                      sprintf(siname, "%d",nbctc);
                    spi_print( df, "C%s %s %s %g\n", siname, name1, name2, ptctc->CAPA * 1e-12);
                  }
                  else
                  {
                    if ((pt=getptype(ptctc->USER, MSL_CAPANAME))!=NULL)
                      strcpy(siname, spi_makename((char *)pt->DATA));
                    else
                      sprintf(siname, "_ctc_%d",nbctc);
                    spi_print( df, "C%s %s %s %g\n", siname, name1, name2, ptctc->CAPA * 1e-12 );
                  }
                  cnt++;
                  nbctc++;
                }
            }

        }
      
      if (cnt !=0) 
        spi_print( df, "\n" );
    }
    
    /* On ne sort la capa totale que si on a ni RCN, ni CTC */

    if( !scanlosig->PRCN->PWIRE && !scanlosig->PRCN->PCTC )
    {
      if( rcn_getcapa( ptfig,scanlosig ) >= CAPAMINI )
      {
        spi_print( df,
                  "Ct%d %s %s %g\n",
                  scanlosig->INDEX,
                  spinamenode( scanlosig, SPI_NONODES ),
                  "0"/*vss*/,
                  rcn_getcapa( ptfig,scanlosig ) * 1e-12
                );
      }
    }
    //spi_print( df, "\n" );
    rcn_unlock_signal(ptfig, scanlosig);
  }
#if 0
  headctc = getallctc( ptfig );

  for( scanchain = headctc ; scanchain ; scanchain = scanchain->NEXT )
  {
    ptctc = (loctc_list*)scanchain->DATA ;

    if( ptctc->CAPA >= CAPAMINI )
    {
      name1 = spinamenode( ptctc->SIG1, ptctc->NODE1 > 0 ? ptctc->NODE1 : SPI_NONODES );
      name2 = spinamenode( ptctc->SIG2, ptctc->NODE2 > 0 ? ptctc->NODE2 : SPI_NONODES );
      if (mbk_LosigIsVSS( ptctc->SIG1 ) || mbk_LosigIsVSS( ptctc->SIG2 ) )
        spi_print( df, "C%d %s %s %g\n", nbctc, name1, name2, ptctc->CAPA * 1e-12 );
      else
        spi_print( df, "C_ctc_%d %s %s %g\n", nbctc, name1, name2, ptctc->CAPA * 1e-12 );
    }

    nbctc++;
  }

  freechain( headctc );
#endif
}

void		signalnoeud( ptfig )
lofig_list	*ptfig;
{
  losig_list	*scanlosig;
  convindex	*nouveau;
  int		 dernier;

  dernier = 1;

  for( scanlosig = ptfig -> LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
  {
    nouveau          = ( convindex* ) mbkalloc( sizeof( convindex ) );
    nouveau->sig     = scanlosig;
    nouveau->premier = dernier;

    /* Evaluation de gauche à droite */
    if( scanlosig->PRCN && scanlosig->PRCN->NBNODE > 0 )
      dernier += scanlosig->PRCN->NBNODE ;
    else
      dernier++; 

    scanlosig->USER = addptype( scanlosig->USER, SPI_DRIVER_PTYPE, nouveau );
  }
}

void		freesignalnoeud( lofig_list	*ptfig )
{
  losig_list	*scanlosig;
  ptype_list *pt;

  for( scanlosig = ptfig -> LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
  {
    if ((pt=getptype(scanlosig->USER, SPI_DRIVER_PTYPE))!=NULL)
    {
       mbkfree(pt->DATA);
       scanlosig->USER = delptype( scanlosig->USER, SPI_DRIVER_PTYPE);
    }
  }
}



void sortconnecteur( df, c )
FILE		*df;
locon_list	*c;
{
  losig_list	*signal;
  num_list	*tetenum,*scannum;
  char           force;

  signal    = c->SIG;

  if( c->PNODE )
  {
    tetenum = spi_getinode( c );
    if( !tetenum ) {
      tetenum = c->PNODE;
      force = 0;
    }
    else
      force = 1;

    for( scannum = tetenum ; scannum ; scannum = scannum->NEXT )
      spi_print( df, "%s ", spinamenodedetail( signal, scannum->DATA, force) );
  }
  else
  {
    tetenum = spi_getinode( c );
    if( !tetenum )
      spi_print( df, "%s ", spinamenode( signal, SPI_NONODES ) );
    else {
      for( scannum = tetenum ; scannum ; scannum = scannum->NEXT )
      {
        spi_print( df, "%s ", spinamenodedetail( signal, scannum->DATA, 1) );
      }
    }
  }
}

void sortconnecteur_ordre( df, ordre, liste )
FILE            *df;
chain_list      *ordre;
locon_list      *liste;
{
  chain_list    *scanordre;
  locon_list    *scanlocon;
  int            num;
  int            n;
  chain_list    *cpteordre;
  losig_list    *signal;
  ptype_list    *pt;
  convindex     *noeudbase;
  num_list      *scannum;
  num_list      *tetenum;
  char           force;
  
  for( scanordre = ordre ; scanordre ; scanordre = scanordre->NEXT )
  {
    for( scanlocon = liste ; scanlocon ; scanlocon = scanlocon->NEXT )
      if( scanlocon->NAME == ((char*)(scanordre->DATA)) )
        break;

    if( !scanlocon ) avt_errmsg (SPI_ERRMSG, "005", AVT_FATAL); 

    num = 0;
    for( cpteordre = ordre ;
         cpteordre != scanordre ;
         cpteordre = cpteordre->NEXT 
       )
      if( ((char*)(cpteordre->DATA)) == scanlocon->NAME )
        num++;

    signal    = scanlocon->SIG;
    pt        = getptype( signal->USER, SPI_DRIVER_PTYPE );
    noeudbase = (convindex*)(pt->DATA);

    if( scanlocon->PNODE )
    {
      tetenum = spi_getinode( scanlocon );
      if( !tetenum ) {
        tetenum = scanlocon->PNODE;
        force = 0;
      }
      else
        force = 1;

      for( scannum = tetenum, n=0 ; n<num && scannum!=NULL; scannum = scannum->NEXT, n++ );
      if (scannum!=NULL && n<=num)
        spi_print( df, "%s ", spinamenodedetail( signal, scannum->DATA, force ) );
      else // zinaps : 25/4/2003
        spi_print( df, "%s ", spinamenode( signal, SPI_NONODES ) );

    }
    else
      spi_print( df, "%s ", spinamenode( signal, SPI_NONODES ) );
  }
}

void		sortnet( ptfig, df )
lofig_list	*ptfig;
FILE		*df;
{
  losig_list	*scanlosig;
  locon_list	*scanlocon;
  chain_list	*scanchain;
  ptype_list	*ptl;
  convindex	*conv;
  tableint	*noeudunique;
  num_list	*scannum;
  lowire_list	*scanlowire;
  loctc_list	*ptctc;
  int		 i;
  char		 v[1024];

  for( scanlosig = ptfig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
  {
    if ( ( scanlosig->NAMECHAIN ) ||
         ( scanlosig->TYPE == EXTERNAL ) )
    {
      ptl  = getptype(scanlosig->USER,SPI_DRIVER_PTYPE);
      conv = (convindex*)(ptl->DATA);

      spi_print( df, "* %s ", SPI_NETNAME );

      /* Evaluation de gauche à droite */
      if( scanlosig->PRCN && scanlosig->PRCN->NBNODE > 1 )
      {
        noeudunique = creattableint();

        for( scanlowire = scanlosig->PRCN->PWIRE ;
             scanlowire ;
             scanlowire = scanlowire->NEXT
           )
        {
          settableint( noeudunique, scanlowire->NODE1, (void*)1 );
          settableint( noeudunique, scanlowire->NODE2, (void*)1 );
        }

        for( scanchain = scanlosig->PRCN->PCTC ;
             scanchain ;
             scanchain = scanchain->NEXT
           )
        {
          ptctc = (loctc_list*)scanchain->DATA;
          if( ptctc->NODE1 )
            settableint( noeudunique, ptctc->NODE1, (void*)1 );
          if( ptctc->NODE2 )
            settableint( noeudunique, ptctc->NODE2, (void*)1 );
        }

        ptl = getptype( scanlosig->USER, LOFIGCHAIN );
        if( !ptl )
        {
          fflush( stdout );
          fprintf( stderr, "*** spi error *** : LOFIGCHAIN missing.\n" );
          EXIT(1); 
        }

        for( scanchain = (chain_list*)(ptl->DATA) ; 
             scanchain ;
             scanchain = scanchain->NEXT
           )
        {
          for( scannum = ((locon_list*)(scanchain->DATA))->PNODE ;
               scannum ;
               scannum = scannum->NEXT
             )
            settableint( noeudunique, scannum->DATA, (void*)1 );
        }

        i = 0 ;
	/*
        while( (i = scanint( noeudunique, i ) ) )
          tooutput( df, "%d ", i + conv->premier - 1 ); 
	*/
	/* On ne sort qu'un seul noeud */
	i = scanint( noeudunique, i );
        spi_print( df, "%d ", i + conv->premier - 1 ); 

        freetableint( noeudunique );
      }
      else
        spi_print( df, "%d ", conv->premier);

      spi_print( df, "= " );

      /*
      for( scanchain = scanlosig->NAMECHAIN ;
           scanchain ;
           scanchain=scanchain->NEXT
         )
      */

      if(scanlosig->TYPE == EXTERNAL)
      {
       for(scanlocon = ptfig->LOCON ; scanlocon != NULL && 
           scanlocon->SIG != scanlosig ; 
           scanlocon = scanlocon->NEXT) 

       if(scanlocon == NULL)
        {
          fflush( stdout );
          fprintf( stderr, "*** spi error *** : LOFIGCHAIN missing.\n" );
          EXIT(1); 
        }

        strcpy( v, scanlocon->NAME);
        spi_vect( v );
        spi_print( df, "%s ", v);
      }
      else
       {
        scanchain = scanlosig->NAMECHAIN;
        {
          strcpy( v, (char*)(scanchain->DATA) );
          spi_vect( v );
          spi_print( df, "%s ", v );
        }
       }

      spi_print( df, "\n" );
    }
  }

}

void sort_unlinked_net( lofig_list *ptfig, FILE *df )
{
  losig_list	*scanlosig;
  ptype_list	*ptl;
  chain_list *cl;

  for( scanlosig = ptfig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
  {
    ptl=getptype(scanlosig->USER, LOFIGCHAIN);
    cl=(chain_list *)ptl->DATA;
    if ( cl==NULL && scanlosig->NAMECHAIN)
    {
      spi_print( df, "* |UNCONNECTED_NET %s\n", spinamenode( scanlosig, SPI_NONODES ));
    }
  }
}

void		sortinstance( ptfig, df )
lofig_list	*ptfig;
FILE		*df;
{
  loins_list	*scanloins;
  locon_list	*scanloconins;
  ptype_list    *pt;
  eqt_param     *ep;
  int i;
  ht *globparam;

  if (SPI_FLAGS & SPI_FULL_DRIVE)
    {
      globparam=addht(2048);
      if (MBK_GLOBALPARAMS!=NULL)
        {
          for (i=0; i<MBK_GLOBALPARAMS->NUMBER; i++)
            addhtitem(globparam, MBK_GLOBALPARAMS->EBI[i].NAME, 0);
        }
    }

  for( scanloins = ptfig->LOINS ; scanloins ; scanloins = scanloins->NEXT )
  {
    spi_print( df, "x%s ", scanloins->INSNAME );
    
    /* L'ordre des connecteurs entre la lofig et sa version instanciée n'est 
       pas le meme */

    pt = getptype( scanloins->USER, PH_INTERF );    
    if( pt )
      sortconnecteur_ordre( df,
                            (chain_list*)(pt->DATA),
                            scanloins->LOCON
                          );
    else
      for( scanloconins = scanloins->LOCON ;
           scanloconins ;
           scanloconins = scanloconins->NEXT
         )
        sortconnecteur( df, scanloconins );
   
    spi_print( df, "%s", scanloins->FIGNAME );


    if (SPI_FLAGS & SPI_FULL_DRIVE)
      {
        optparam_list *optparams, *ptopt;
        // drive les parametres des instances
        if ((pt = getptype (scanloins->USER, PARAM_CONTEXT))!=NULL)
          {
            ep=(eqt_param *)pt->DATA;
            for (i=0; i<ep->NUMBER; i++)
              {
                if ((SPI_FLAGS & SPI_DRIVE_ALL)!=0 || ((SPI_FLAGS & SPI_DRIVE_ALL)==0 && gethtitem(globparam, ep->EBI[i].NAME)==EMPTYHT))
                  spi_print( df, " %s=%g", ep->EBI[i].NAME, ep->EBI[i].VAL );
              }
          }

        if ((pt = getptype (scanloins->USER, OPT_PARAMS))) {
          optparams = (optparam_list *) pt->DATA;
          
          for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT) {
            if (ptopt->TAG == '$') {
              spi_print( df, " %s='%s'", ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR );
            }
            else {
              spi_print( df, " %s=%g", ptopt->UNAME.SPECIAL, ptopt->UDATA.VALUE );
            }
          }
        }
      }

    spi_print( df, "\n");

  }
  if (SPI_FLAGS & SPI_FULL_DRIVE) delht(globparam);
}

/* param doit etre en minuscule */
static float getbaseunit(lofig_list *lf, char *param)
{
  char temp[100], *c;
  chain_list *cl;
  sprintf(temp," %s ",param);
  for (cl=UNIT_LIST; cl!=NULL; cl=cl->NEXT)
    {
      c=(char *)cl->DATA;
      if (strstr(c,temp)!=NULL && (c=strchr(c,':'))!=NULL)
        return atof(&c[1]);
    }
  return 0;

  lf    = NULL;
}



static char *modelname(lotrs_list *tr, int in_SPI_TRANSISTOR_AS_INSTANCE)
{
  char *subcktname=NULL, *modelname;
  ptype_list *ptype;

  if(tr->MODINDEX < 0)
    return spi_gettransname(tr->TYPE);

  modelname=getlotrsmodel(tr);

  if ( in_SPI_TRANSISTOR_AS_INSTANCE )
    {
      if (( ptype = getptype ( tr->USER, TRANS_FIGURE )))
        subcktname = (char*)ptype->DATA;
      else 
        subcktname = mcc_getsubckt( MCC_MODELFILE, modelname, 
                                    MLO_IS_TRANSN(tr->TYPE)?MCC_NMOS:MCC_PMOS,
                                    MCC_TYPICAL, ((float)tr->LENGTH/SCALE_X)*1.0e-6,
                                    ((float)tr->WIDTH/SCALE_X)*1.0e-6);
    }

  if (subcktname==NULL) return modelname;
  return subcktname;
}



// zinaps: le 27/9/2002
static char *strip(char *name)
{
  static char temp[1024];
  int i,j;
  for (i=0, j=0;name[i]!='\0';i++)
    {
      if (name[i]!='*') temp[j++]=name[i];
    }
  temp[j]='\0';
  return temp;
}

static float bestunit(float val, char **unit)
{
  if (val<1e3) { *unit="U"; return val; }
//  if (val<1e6) { *unit='m'; return val*1e-6; }
  *unit=""; return val*1e-6;
}

#define DONE_W        0x00000001
#define DONE_L        0x00000002
#define DONE_AS       0x00000004
#define DONE_AD       0x00000008
#define DONE_PS       0x00000010
#define DONE_PD       0x00000020
#define DONE_NRS      0x00000040
#define DONE_NRD      0x00000080
#define DONE_DELVTO   0x00000100
#define DONE_SA       0x00000200
#define DONE_SB       0x00000400
#define DONE_SD       0x00000800
#define DONE_NF       0x00001000
#define DONE_GEOMOD   0x00002000
#define DONE_MULUO    0x00004000
#define DONE_M        0x00008000
#define DONE_SC       0x00010000
#define DONE_SCA      0x00020000
#define DONE_SCB      0x00040000
#define DONE_SCC      0x00080000
#define DONE_ABSOURCE 0x00100000
#define DONE_LSSOURCE 0x00200000
#define DONE_LGSOURCE 0x00400000
#define DONE_ABDRAIN  0x00800000
#define DONE_LSDRAIN  0x01000000
#define DONE_LGDRAIN  0x02000000

void            sorttransistormos(lofig_list *ptfig, FILE *df, char *vss, char *vdd )
{
  lotrs_list	*scantrs;
  int		 nb;
  ht            *trname;
  char           name[1024], *ptr, tag ;
  char          *alim, *modname, *paramname;
  int            isdiode, isjfet;
  float          w_unit, l_unit, as_unit, ad_unit, ps_unit, pd_unit, val;
  double         delvt0,mulu0;
  double         sa,sb,sd;
  double         sca,scb,scc,sc;
  double         nrs,nrd,m;
  double         nf,nfing;
  double         geomod;
  double         absource, lssource, lgsource ;
  double         abdrain, lsdrain, lgdrain ;
  int            status, done, in_SPI_TRANSISTOR_AS_INSTANCE ;
  ptype_list *pt;

  if ((w_unit=getbaseunit(ptfig, "w"))==0) w_unit=1;
  if ((l_unit=getbaseunit(ptfig, "l"))==0) l_unit=1;
  if ((as_unit=getbaseunit(ptfig, "as"))==0) as_unit=1;
  if ((ad_unit=getbaseunit(ptfig, "ad"))==0) ad_unit=1;
  if ((ps_unit=getbaseunit(ptfig, "ps"))==0) ps_unit=1;
  if ((pd_unit=getbaseunit(ptfig, "pd"))==0) pd_unit=1;

  for( scantrs = ptfig->LOTRS, nb=1 ; scantrs ; scantrs = scantrs->NEXT, nb++ );

  trname = addht(nb);

  nb = 0;

  for( scantrs = ptfig->LOTRS; scantrs; scantrs = scantrs->NEXT )
    {
      done=0;
      modname = getlotrsmodel(scantrs);
      isdiode = 0; //(mbk_isdioden(modname) || mbk_isdiodep(modname));
      isjfet = (mbk_isjfetn(modname) || mbk_isjfetp(modname));
      if (V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE==2)
      {
        ptype_list *ptype;
        if (( ptype = getptype ( scantrs->USER, TRANS_FIGURE ))!=NULL)
          in_SPI_TRANSISTOR_AS_INSTANCE=1;
        else
           in_SPI_TRANSISTOR_AS_INSTANCE=0;
      }
      else in_SPI_TRANSISTOR_AS_INSTANCE=V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE;
         
      if( scantrs->TRNAME )
        {
          if( gethtitem( trname, scantrs->TRNAME ) != EMPTYHT )
            {
              do
                {
                  nb++;
                  sprintf( name, "%s_%d", scantrs->TRNAME, nb );
                  ptr = namealloc( name );
                }
              while( gethtitem( trname, ptr ) != EMPTYHT );
              addhtitem( trname, ptr, 1 );
              if (isdiode)
                spi_print( df, "D%s ", spi_makename(name) );
              else if (isjfet)
                spi_print( df, "J%s ", spi_makename(name) );
              else if (!in_SPI_TRANSISTOR_AS_INSTANCE)
                spi_print( df, "M%s ", spi_makename(name) );  
              else
                spi_print( df, "XM%s ", spi_makename(name) );  
            }
          else
            {
              if (isdiode)
                spi_print( df, "D%s ", spi_makename(scantrs->TRNAME) );
              else if (isjfet)
                spi_print( df, "J%s ", spi_makename(scantrs->TRNAME) );
              else if (!in_SPI_TRANSISTOR_AS_INSTANCE)
                spi_print( df, "M%s ", spi_makename(scantrs->TRNAME) );
              else
                spi_print( df, "XM%s ", spi_makename(scantrs->TRNAME) );
              addhtitem( trname, scantrs->TRNAME, 1 );
            }
        }
      else
        {
          do
            {
              nb++;
              sprintf( name, "%d", nb );
              ptr = namealloc( name );
            }
          while( gethtitem( trname, ptr ) != EMPTYHT );
          if (isdiode)
            spi_print( df, "D%s ", spi_makename(name) );
          else if (isjfet)
            spi_print( df, "J%s ", spi_makename(name) );
          else if (!in_SPI_TRANSISTOR_AS_INSTANCE)
            spi_print( df, "M%s ", spi_makename(name) );
          else
            spi_print( df, "XM%s ", spi_makename(name) );
          addhtitem( trname, ptr, 1 );
        }
      if (isdiode)
        {
          if (mbk_isdioden(modname))
            {
              sortconnecteur( df, scantrs->GRID   );
              sortconnecteur( df, scantrs->DRAIN  );
            }
          else
            {
              sortconnecteur( df, scantrs->DRAIN  );
              sortconnecteur( df, scantrs->GRID   );
            }
          spi_print( df, "%s ", modname);
          if( scantrs->XD != 0 && scantrs->PD != 0 )
            spi_print( df, "AREA=%gP ", (float) scantrs->XD * scantrs->PD / ( 4 * SCALE_X * SCALE_X ) );
          if( scantrs->PD != 0 )
            spi_print( df, "PERI=%gU ", (float)scantrs->PD/SCALE_X );
        }
      else if (isjfet)
        {
          sortconnecteur( df, scantrs->DRAIN  );
          sortconnecteur( df, scantrs->GRID   );
          sortconnecteur( df, scantrs->SOURCE  );
          spi_print( df, "%s ", modname);
        }
      else
        {
          sortconnecteur( df, scantrs->DRAIN  );
          sortconnecteur( df, scantrs->GRID   );
          sortconnecteur( df, scantrs->SOURCE );
          // zinaps le 20/8/2002
          if( scantrs->BULK && scantrs->BULK->SIG )
            {
              sortconnecteur( df, scantrs->BULK  );
              modname=modelname(scantrs, in_SPI_TRANSISTOR_AS_INSTANCE);
              spi_print( df, "%s ", modname);
            }
          else
            {
              if (MLO_IS_TRANSN(scantrs->TYPE))
                {
                  if( !vss ) alim=strip(VSS); else alim=vss;
                  modname=modelname(scantrs, in_SPI_TRANSISTOR_AS_INSTANCE);
                  spi_print( df, "%s %s ", alim, modname);
                }
              else
                {
                  if( !vdd ) alim=strip(VDD); else alim=vdd;
                  modname=modelname(scantrs, in_SPI_TRANSISTOR_AS_INSTANCE);
                  spi_print( df, "%s %s ", alim, modname);
                }
            }
          if (!in_SPI_TRANSISTOR_AS_INSTANCE)
            {
              if(scantrs->LENGTH!=0)
                {
                  if (l_unit!=1 || !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE)
                    spi_print( df, "L=%g ", ((float)scantrs->LENGTH/SCALE_X)*1e-6/l_unit );
                  else 
                    {
                      val=bestunit((float)scantrs->LENGTH/SCALE_X, &ptr);
                      spi_print( df, "L=%g%s ", val, ptr );
                    }
                  done|=DONE_L;
                }
        
              if(scantrs->WIDTH!=0)
                {
                  if (w_unit!=1 || !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE)
                    spi_print( df, "W=%g ", ((float)scantrs->WIDTH/SCALE_X)*1e-6/w_unit );
                  else
                    {
                      val=bestunit((float)scantrs->WIDTH/SCALE_X, &ptr);
                      spi_print( df, "W=%g%s ", val, ptr);
                    }
                  done|=DONE_W;
                }
        
              if( scantrs->XS >= 0 )
                {
                  if (as_unit!=1 || !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE)
                    spi_print( df,
                               "AS=%g ",
                               ((float) scantrs->XS * scantrs->WIDTH / ( SCALE_X * SCALE_X ))*1e-12/as_unit
                               );
                  else
                    spi_print( df,
                               "AS=%gP ",
                               (float) scantrs->XS * scantrs->WIDTH / ( SCALE_X * SCALE_X )
                               );
                  done|=DONE_AS;
                }
        
              if( scantrs->XD >= 0 )
                {
                  if (ad_unit!=1 || !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE)
                    spi_print( df,
                               "AD=%g ",
                               ((float) scantrs->XD * scantrs->WIDTH / ( SCALE_X * SCALE_X ))*1e-12/ad_unit
                               );
                  else
                    spi_print( df,
                               "AD=%gP ",
                               (float) scantrs->XD * scantrs->WIDTH / ( SCALE_X * SCALE_X )
                               );
                  done|=DONE_AD;
                }
              if( scantrs->PS >= 0 )
                {
                  if (ps_unit!=1 || !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE)
                    spi_print( df, "PS=%g ", ((float)scantrs->PS/SCALE_X)*1e-6/ps_unit );
                  else
                    spi_print( df, "PS=%gU ", (float)scantrs->PS/SCALE_X );
                  done|=DONE_PS;
                }
        
              if( scantrs->PD >= 0 )
                {
                  if (pd_unit!=1 || !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE)                  
                    spi_print( df, "PD=%g ", ((float)scantrs->PD/SCALE_X)*1e-6/pd_unit );
                  else
                    spi_print( df, "PD=%gU ", (float)scantrs->PD/SCALE_X );
                  done|=DONE_PD;
                }

              if ( SPI_DRV_TRSPARAM )
                {
                  nrs = getlotrsparam (scantrs,MBK_NRS,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "nrs=%g ",nrs);
                    done = done | DONE_NRS ;
                  }
        
                  nrd = getlotrsparam (scantrs,MBK_NRD,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "nrd=%g ",nrd);
                    done = done | DONE_NRD ;
                  }
        
                  mulu0 = getlotrsparam (scantrs,MBK_MULU0,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "mulu0=%g ",mulu0);
                    done = done | DONE_MULUO ;
                  }
        
                  delvt0 = getlotrsparam (scantrs,MBK_DELVT0,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "delvto=%g ",delvt0);
                    done = done | DONE_DELVTO ;
                  }
        
                  sa = getlotrsparam (scantrs,MBK_SA,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "sa=%g ",sa);
                    done = done | DONE_SA ;
                  }
        
                  sb = getlotrsparam (scantrs,MBK_SB,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "sb=%g ",sb);
                    done = done | DONE_SB ;
                  }
        
                  sd = getlotrsparam (scantrs,MBK_SD,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "sd=%g ",sd);
                    done = done | DONE_SD ;
                  }

                  nf = getlotrsparam (scantrs,MBK_NF,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "nf=%g ",nf);
                    done = done | DONE_NF ;
                  }
        
                  nfing = getlotrsparam (scantrs,MBK_NFING,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "nfing=%g ",nfing);
                    done = done | DONE_NF ;
                  }
        
                  m = getlotrsparam (scantrs,MBK_M,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "m=%g ",m);
                    done = done | DONE_M ;
                  }
          
                  geomod = getlotrsparam (scantrs,MBK_GEOMOD,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "geomod=%g ",geomod);
                    done = done | DONE_GEOMOD ;
                  }
          
                  sc = getlotrsparam (scantrs,MBK_SC,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "sc=%g ",sc);
                    done = done | DONE_SC ;
                  }
        
                  sca = getlotrsparam (scantrs,MBK_SCA,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "sca=%g ",sca);
                    done = done | DONE_SCA ;
                  }
        
                  scb = getlotrsparam (scantrs,MBK_SCB,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "scb=%g ",scb);
                    done = done | DONE_SCB ;
                  }
        
                  scc = getlotrsparam (scantrs,MBK_SCC,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "scc=%g ",scc);
                    done = done | DONE_SCC ;
                  }
                  
                  absource = getlotrsparam (scantrs,MBK_ABSOURCE,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "absource=%g ",absource);
                    done = done | DONE_ABSOURCE ;
                  }

                  lssource = getlotrsparam (scantrs,MBK_LSSOURCE,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "lssource=%g ",lssource);
                    done = done | DONE_LSSOURCE ;
                  }

                  lgsource = getlotrsparam (scantrs,MBK_LGSOURCE,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "lgsource=%g ",lgsource);
                    done = done | DONE_LGSOURCE ;
                  }
                  
                  abdrain = getlotrsparam (scantrs,MBK_ABDRAIN,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "abdrain=%g ",abdrain);
                    done = done | DONE_ABDRAIN ;
                  }

                  lsdrain = getlotrsparam (scantrs,MBK_LSDRAIN,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "lsdrain=%g ",lsdrain);
                    done = done | DONE_LSDRAIN ;
                  }

                  lgdrain = getlotrsparam (scantrs,MBK_LGDRAIN,NULL,&status);
                  if(status == 1) {
                    spi_print( df, "lgdrain=%g ",lgdrain);
                    done = done | DONE_LGDRAIN ;
                  }
                }
              if (done==0 || (SPI_FLAGS & SPI_FULL_DRIVE)!=0)
                {
                  optparam_list *optparams, *ptopt;
                  if ((pt = getptype (scantrs->USER, OPT_PARAMS))) {
                    optparams = (optparam_list *) pt->DATA;
          
                    for (ptopt = optparams; ptopt; ptopt = ptopt->NEXT) {
                      if (((done & DONE_L)!=0 && strcasecmp(ptopt->UNAME.SPECIAL, "l")==0)
                          || ((done & DONE_W)!=0 && strcasecmp(ptopt->UNAME.SPECIAL, "w")==0)
                          || ((done & DONE_AS)!=0 && strcasecmp(ptopt->UNAME.SPECIAL, "as")==0)
                          || ((done & DONE_AD)!=0 && strcasecmp(ptopt->UNAME.SPECIAL, "ad")==0)
                          || ((done & DONE_PS)!=0 && strcasecmp(ptopt->UNAME.SPECIAL, "ps")==0)
                          || ((done & DONE_PD)!=0 && strcasecmp(ptopt->UNAME.SPECIAL, "pd")==0)) continue;
            
                      if (isknowntrsparam (ptopt->UNAME.STANDARD)) { paramname=*ptopt->UNAME.STANDARD; tag=paramname[0]; }
                      else { paramname=ptopt->UNAME.SPECIAL; tag=ptopt->TAG; }
                      if (tag == '$') {
                        spi_print( df, " %s='%s'", *paramname=='$'?paramname+1:paramname, ptopt->UDATA.EXPR );
                      }
                      else {
                        spi_print( df, " %s=%g", paramname, ptopt->UDATA.VALUE );
                      }
                    }
                  }
                }
            }
          else
            {
              pt = getptype( scantrs->USER, PARAM_CONTEXT );
              if( pt ) {
                eqt_param *param ;
                int        i ;

                param = (eqt_param*)pt->DATA ;
                for( i = 0 ; i < param->NUMBER ; i++ ) {
                  if( ( ( done & DONE_W      ) && !strcasecmp( param->EBI[i].NAME, "W"      ) )  ||
                      ( ( done & DONE_L      ) && !strcasecmp( param->EBI[i].NAME, "L"      ) )  ||
                      ( ( done & DONE_AS     ) && !strcasecmp( param->EBI[i].NAME, "AS"     ) )  ||
                      ( ( done & DONE_AD     ) && !strcasecmp( param->EBI[i].NAME, "AD"     ) )  ||
                      ( ( done & DONE_PS     ) && !strcasecmp( param->EBI[i].NAME, "PS"     ) )  ||
                      ( ( done & DONE_PD     ) && !strcasecmp( param->EBI[i].NAME, "PD"     ) )  ||
                      ( ( done & DONE_DELVTO ) && !strcasecmp( param->EBI[i].NAME, "DELVT0" ) )  ||
                      ( ( done & DONE_MULUO  ) && !strcasecmp( param->EBI[i].NAME, "MULU0"  ) )  ||
                      ( ( done & DONE_SA     ) && !strcasecmp( param->EBI[i].NAME, "SA"     ) )  ||
                      ( ( done & DONE_SB     ) && !strcasecmp( param->EBI[i].NAME, "SB"     ) )  ||
                      ( ( done & DONE_SD     ) && !strcasecmp( param->EBI[i].NAME, "SD"     ) )  ||
                      ( ( done & DONE_GEOMOD ) && !strcasecmp( param->EBI[i].NAME, "GEOMOD" ) )  ||
                      ( ( done & DONE_M      ) && !strcasecmp( param->EBI[i].NAME, "M"      ) )  ||
                      ( ( done & DONE_NF     ) && !strcasecmp( param->EBI[i].NAME, "NF"     ) )  ||
                      ( ( done & DONE_NF     ) && !strcasecmp( param->EBI[i].NAME, "NFING"  ) )  ||
                      ( ( done & DONE_NRS    ) && !strcasecmp( param->EBI[i].NAME, "NRS"    ) )  ||
                      ( ( done & DONE_NRD    ) && !strcasecmp( param->EBI[i].NAME, "NRD"    ) )  ||
                      ( ( done & DONE_SC     ) && !strcasecmp( param->EBI[i].NAME, "SC"     ) )  ||
                      ( ( done & DONE_SCA    ) && !strcasecmp( param->EBI[i].NAME, "SCA"    ) )  ||
                      ( ( done & DONE_SCB    ) && !strcasecmp( param->EBI[i].NAME, "SCB"    ) )  ||
                      ( ( done & DONE_SCC    ) && !strcasecmp( param->EBI[i].NAME, "SCC"    ) )     ) 
                    continue ;
                  spi_print( df, " %s=%g", param->EBI[i].NAME, param->EBI[i].VAL );
                }
              }
            }
        }
      spi_print( df, "\n" );
    }

  delht( trname );
}

void		sortcircuit( ptfig, df )
lofig_list	*ptfig;
FILE		*df;
{
  char		*vdd, *vss;
  locon_list	*scancon;
  ptype_list    *pt;
  char temp[1024];

  cherche_alim( ptfig, &vdd, &vss );

  /* Sortie des instances et des transistors */

  spi_print( df, "\n.subckt %s ", ptfig->NAME );
  
  pt = getptype( ptfig->USER, PH_INTERF );
  if( pt )
    sortconnecteur_ordre( df, (chain_list*)(pt->DATA), ptfig->LOCON );
  else
    for( scancon = ptfig->LOCON ; scancon ; scancon = scancon->NEXT )
      sortconnecteur( df, scancon );

  spi_print( df,"\n" );

  {
    int count=0;
    for (scancon=ptfig->LOCON; scancon!=NULL; scancon=scancon->NEXT)
      {
        if (scancon->DIRECTION!=UNKNOWN)
          {
            strcpy(temp,scancon->NAME); spi_vect(temp);
            if (count==0) fprintf( df, "* |CONDIR ");
            else fprintf( df, ", ");
            fprintf( df, temp);
            switch (scancon->DIRECTION)
              {
              case IN: fprintf( df, " IN"); break;
              case OUT: fprintf( df, " OUT"); break;
              case INOUT: fprintf( df, " INOUT"); break;
              case TRANSCV: fprintf( df, " TRANSCV"); break;
              case TRISTATE: fprintf( df, " TRISTATE"); break;                
              default: fprintf( df, " IN"); break;                
              }
            count++;
            if (count==5)
              {
                fprintf( df, "\n");
                count=0;
              }
          }
      }
    if (count!=0) fprintf( df, "\n");
  }

  if( SPI_NAMENODES == FALSE )
    sortnet( ptfig, df );

  sortinstance( ptfig, df );
  sorttransistormos( ptfig, df, vss, vdd);
  sortrcn( ptfig, df, vss );
  sort_unlinked_net(ptfig, df );

  if( spi_getfuncinode() ) {
    (spi_getfuncinode())( df, ptfig, spi_getdatainode() );
    spi_setfuncinode( NULL, NULL );
  }

  spi_print( df, ".ends %s\n\n", ptfig->NAME );
}



void            spi_env()
{
  char          *env;
 
  RESIMINI = V_FLOAT_TAB[__MBK_SPI_DRIVE_RESI_MINI].VALUE;
  CAPAMINI = V_FLOAT_TAB[__MBK_SPI_DRIVE_CAPA_MINI].VALUE;
  SPI_DRV_TRSPARAM = V_BOOL_TAB[__MBK_SPI_DRV_TRSPARAM].VALUE;
  SPI_NETNAME = V_STR_TAB[__MBK_SPI_NETNAME].VALUE;
  SPI_NAMENODES = V_BOOL_TAB[__MBK_SPI_NAMENODES].VALUE;

  env = V_STR_TAB[__MBK_SPI_COMPOSE_NAME_SEPAR].VALUE;
  if( env )
  {
    switch( strlen( env ) )
    {
      case 0:
        SPI_COMPOSE_NAME_SEPAR = 0;
        break;
      case 1:
        SPI_COMPOSE_NAME_SEPAR = *env;
        break;
      default :
        avt_errmsg (SPI_ERRMSG, "007", AVT_FATAL); 
        break;
    }
  }
  else {
    SPI_COMPOSE_NAME_SEPAR = '_';
  }

  if (UNIT_LIST==NULL)
    {
      env = V_STR_TAB[__MBK_DRIVE_DEFAULT_UNITS].VALUE;
      if (env!=NULL)
        {
          char *start=env;
          int i=1, tag=0;
          char temp[1000];
          strcpy(temp," ");

          while (*start!='\0')
            {
              if (*start==';')
                {
                  if (tag!=1) 
                    fprintf(stderr,"error parsing env '%s=%s'\n","MBK_DRIVE_DEFAULT_UNITS",env);
                  temp[i]='\0';
                  UNIT_LIST=addchain(UNIT_LIST, strdup(temp));
                  i=1;
                  tag=0;
                  start++;
                }
              if (*start==',') { temp[i++]=' '; start++; }
              else if (*start==':') { temp[i++]=' '; tag++; }
              if (*start!=' ') temp[i++]=tolower(*start);
              start++;
            }
          if (i!=1)
            {
              if (tag!=1) 
                fprintf(stderr,"error parsing env '%s=%s'\n","MBK_DRIVE_DEFAULT_UNITS",env);
              temp[i]='\0';
              UNIT_LIST=addchain(UNIT_LIST, strdup(temp));
            }
        }
    }

  SPI_FLAGS=mbk_ReadFlags(__MBK_SPICE_DRIVER_FLAGS, spice_opack, sizeof(spice_opack)/sizeof(*spice_opack), 0, 0);

}

void		spicesavelofiginfile( lofig_list *ptfig, FILE *df, int driveincludes )
{
  chain_list    *scanchain;
  locon_list    *scancon;
  num_list      *scannum;
  char		 v[1024];
  ptype_list    *pt;
  char          *nom;
  int            colonne;
  int            lgmot;

  spi_env();

  // Zinaps: coherence entre les formats mbk
  {
    loins_list *ls;
    // connecteurs primaux
    ptfig->LOCON=(locon_list *)reverse((chain_list *)ptfig->LOCON);
    for (ls=ptfig->LOINS;ls!=NULL;ls=ls->NEXT)
      ls->LOCON=(locon_list *)reverse((chain_list *)ls->LOCON);
  }
  // Zinaps: c'est tout

  /* On va travailler sur les fonctions RCN : */
  lofigchain( ptfig );
  
  signalnoeud( ptfig );		/* Calcule les noeuds Spice */

  /* Sort la ligne *interf */

  if( SPI_NAMENODES == FALSE ) {
    colonne = strlen( "* INTERF " );
    tooutput( df, "* INTERF " );
   
    pt = getptype( ptfig->USER, PH_INTERF );
    if( pt )
    {
      for( scanchain = (chain_list*)(pt->DATA) ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        nom = ((char*)(scanchain->DATA));
        strcpy( v, nom );
        spi_vect( v );

        lgmot = strlen(v)+1;
        if( colonne+lgmot >= SPI_MAX_COL-2 )
        {
          colonne = strlen( "* INTERF " );
          tooutput( df, "\n* INTERF " );
        }
        colonne = colonne + lgmot;

        tooutput( df, "%s ", v );

      }
    }
    else
    {
      for( scancon = ptfig->LOCON ; scancon ; scancon = scancon->NEXT )
      {
        if( scancon->PNODE )
        {
          for( scannum = scancon->PNODE ; scannum ; scannum = scannum->NEXT )
          {
            strcpy( v, scancon->NAME );
            spi_vect( v );

            lgmot = strlen(v)+1;
            if( colonne+lgmot >= SPI_MAX_COL-2 )
            {
              colonne = strlen( "* INTERF " );
              tooutput( df, "\n* INTERF " );
            }
            colonne = colonne + lgmot;

            tooutput( df, "%s ", v );

          }
        }
        else
        {
          strcpy( v, scancon->NAME );
          spi_vect( v );
          
          lgmot = strlen(v)+1;
          if( colonne+lgmot >= SPI_MAX_COL-2 )
          {
            colonne = strlen( "* INTERF " );
            tooutput( df, "\n* INTERF " );
          }
          colonne = colonne + lgmot;

          tooutput( df, "%s ", v );
        }
      }
    }

    tooutput( df, "\n\n" );
  }

  /* Sort les .include */
  
  if (driveincludes)
    {
      for( scanchain = ptfig->MODELCHAIN; scanchain; scanchain = scanchain->NEXT )
        {
          spi_print( df, ".INCLUDE \"%s.%s\"\n", (char*)scanchain->DATA, OUT_LO ); 
        }
    }
  sortcircuit( ptfig, df );

  // Zinaps: coherence entre les formats mbk, retour a la normal
  {
    loins_list *ls;
    // connecteurs primaux
    ptfig->LOCON=(locon_list *)reverse((chain_list *)ptfig->LOCON);
    for (ls=ptfig->LOINS;ls!=NULL;ls=ls->NEXT)
      ls->LOCON=(locon_list *)reverse((chain_list *)ls->LOCON);
  }
  // Zinaps: c'est tout

  spi_cleanextlocon(ptfig);
  freesignalnoeud(ptfig );

}

void		spicesavelofig( ptfig )
lofig_list	*ptfig;
{
  FILE		*df;		/* descripteur de fichier de sortie */

  spi_env();

  /* Ouverture du fichier de sortie */
  df = mbkfopen( ptfig->NAME, OUT_LO, WRITE_TEXT );
  if( !df )
  {
    fflush( stdout );
    fprintf( stderr, "*** mbk error : savelofig impossible.\n" );
    fprintf( stderr,
             "Can't open file %s.%s for writing.\n",
             ptfig->NAME,
             OUT_LO
           );
    EXIT(1);
  }
  
  /* header */
  spiceprintdate( ptfig->NAME, df );


//---------->
  spicesavelofiginfile( ptfig, df, 1);

  if( fclose(df) == -1 )
  {
    fflush( stdout );
    fprintf( stderr,
             "*** mbk error *** : Can't close file %s.\n,",
             ptfig->NAME
           );
    EXIT(1);
  }
}

void spicesavelofigsinfile( chain_list *cl, FILE *df)
{
  time_t         secondes;
  struct tm     *jours;
  lofig_list *ptfig;

  spi_env();

  /* Ouverture du fichier de sortie */

  time( &secondes );
  jours = localtime( &secondes );

  while (cl!=NULL)
    {
      ptfig=(lofig_list *)cl->DATA;
      spicesavelofiginfile( ptfig, df, 0);
      cl=cl->NEXT;
    }
}


void spiceprintdate( char *circuit, FILE *df )
{
  char       buf[1024];

  sprintf(buf,"Spice description of %s\n", circuit );
  avt_printExecInfo(df, "*", buf, "");
}



void            tooutput( FILE *fd, ...)
{
  va_list       index;
  char          *fmt;

  va_start( index, fd );
  
  fmt = va_arg( index, char* );

  if( vfprintf( fd, fmt, index ) < 0 )
  {
    fflush( stdout );
    fprintf( stderr, "*** spi error *** : Error while writing file.\n" );
    perror( "System say " );
    EXIT( 1 );
  }
}





void spi_vect( s )
char *s;
{
  int i,p1;

//  if (!MBK_DEVECT) return;

  if( s == NULL )
    return;
  if( s[0] == '\0' )
    return;

 
  /* Positionne i sur le premier caractere non espace a la fin de la chaine */
  i = strlen( s ) ;
  do
    i--;
  while( s[i] == ' ' && i >0 );

  /* passe un eventuel paquet de nombres */
  if( i )
  {
    p1 = i;
    while( isdigit( (int)s[i] ) && i >0 )
      i--;
    if( p1 != i && s[i] == ' ' )
    {
      if (!strcmp (SPI_VECTOR, "[]"))
      {
        s[i]    = '[';
        s[p1+1] = ']';
        s[p1+2] = '\0' ;
      }
      else if (!strcmp (SPI_VECTOR, "()"))
      {
        s[i]    = '(';
        s[p1+1] = ')';
        s[p1+2] = '\0' ;
      }
      else if (!strcmp (SPI_VECTOR, "<>"))
      {
        s[i]    = '<';
        s[p1+1] = '>';
        s[p1+2] = '\0' ;
      }
      else if (!strcmp (SPI_VECTOR, "_"))
      {
        s[i]    = '_';
        s[p1+1] = '\0' ;
      }
    }
  }
}

void spi_print( FILE *fd, ...)
{
  va_list       index;
  char          *fmt;
  static int    lgcurrent=0;
  char          buf[1000];
  int           l;
  
  va_start( index, fd );
  
  fmt = va_arg( index, char* );

  vsprintf( buf, fmt, index ) ;
  l = strlen(buf);
 
  if( lgcurrent==0 || l + lgcurrent < 80 ) {
    fputs( buf, fd );
    lgcurrent = lgcurrent + l ;
  }
  else {
    fputs( "\n+ ", fd );
    fputs( buf, fd );
    lgcurrent = l + 2;
  }
  
  if( buf[l-1] == '\n' ) 
    lgcurrent = 0;
}

/* Fonctions d'accès pour insérer des mesures dans le fichier de sortie sur les
locons sans avoir à modifier la lofig.

   locon->PNODE =  1  2  3
          INODE = 12 13 14

Les inodes sont drivés en priorités par rapport aux pnodes. Ensuite, la 
fonction utilisateur est appellés avant le .ends. C'est l'utilisateur qui
doit placer les mesures entre chaque paire de noeud pnode/inode :

  exemple pour des mesures de courant :

    Vmeas1  1 12 dc 0v
    Vmeas2  2 13 dc 0v
    Vmeas3  3 14 dc 0v
*/

void spi_setfuncinode( void (*fn)( FILE*, lofig_list*, void* ), void *data )
{
  SPI_LOCON_INODE_FUNC = fn;
  SPI_LOCON_INODE_DATA = data;
}

void *spi_getdatainode(void)
{
  return SPI_LOCON_INODE_DATA;
}

void (*spi_getfuncinode(void))( FILE*, lofig_list*, void* ) 
{
  return SPI_LOCON_INODE_FUNC;
}

num_list* spi_getinode( locon_list *locon )
{
  ptype_list *ptl;

  ptl = getptype( locon->USER, SPI_LOCON_INODE );
  if( !ptl ) return NULL;
  return (num_list*)ptl->DATA;
}

void spi_setinode( locon_list *locon, num_list *head )
{
  ptype_list *ptl;
 
  if( !locon->SIG )
    return;

  ptl = getptype( locon->USER, SPI_LOCON_INODE );
  if( ptl ) {
    freenum( ptl->DATA );
  }
  else {
    locon->USER = addptype( locon->USER, SPI_LOCON_INODE, NULL );
    ptl = locon->USER;
  }
  
  ptl->DATA = head;

  ptl = getptype( locon->SIG->USER, SPI_SIG_INODE );
  if( !ptl )
    locon->SIG->USER = addptype( locon->SIG->USER, 
                                 SPI_SIG_INODE, 
                                 (void*)1l 
                               );
  else 
    (*(long *)&ptl->DATA)++;
}

void spi_clearinode( locon_list *locon )
{
  ptype_list *ptl;

  ptl = getptype( locon->USER, SPI_LOCON_INODE );
  if( ptl ) {
    freenum( ptl->DATA );
    locon->USER = delptype( locon->USER, SPI_LOCON_INODE );
  }

  if( locon->SIG ) {
    ptl = getptype( locon->SIG->USER, SPI_SIG_INODE );
    if( ptl ) { // C'est obligé...
      (*(long *)&ptl->DATA)--;
      if( ((long)ptl->DATA)==0 )
        locon->SIG->USER = delptype( locon->SIG->USER, SPI_SIG_INODE );
    }
  }
}

/* renvoie un locon sur le signal pour la détermination d'un nom de
signal représentatif. renvoie NULL pas de solution. */

static char *nameofdevice(locon_list *lc)
{
  if (lc->TYPE=='T' && ((lotrs_list *)lc->ROOT)->TRNAME!=NULL) return ((lotrs_list *)lc->ROOT)->TRNAME; 
  if (lc->TYPE=='I' && ((loins_list *)lc->ROOT)->INSNAME!=NULL) return ((loins_list *)lc->ROOT)->INSNAME; 
  return "";
}

locon_list* spichooseonelocon( losig_list *losig )
{
  ptype_list *ptype ;
  locon_list *minlocon = NULL ;
  locon_list *locon ;
  chain_list *chain ;
  int ret;
 
  ptype = getptype( losig->USER, SPI_CHOOSEN_LOCON );
  if( ptype )
    return (locon_list*)ptype->DATA ;

  ptype = getptype( losig->USER, LOFIGCHAIN );
  if( ptype ) {

    for( chain = ptype->DATA ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)chain->DATA ;
      if( locon->PNODE && 
          ( !minlocon ||
              (ret=strcmp(locon->NAME, minlocon->NAME))<0 ||
              (ret==0 && strcmp(nameofdevice(locon), nameofdevice(minlocon))<0))
          )
        minlocon = locon ;
    }
 
    losig->USER = addptype( losig->USER, SPI_CHOOSEN_LOCON, minlocon );
  }

  return minlocon ;
}
