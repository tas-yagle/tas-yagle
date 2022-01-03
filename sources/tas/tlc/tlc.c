/******************************************************************************\
*                                                                              *
*                           Alliance VLSI CAD system                           *
*    +---+                                                                     *
*   / \ / \           Laboratoire LIP6 - Département ASIM                      *
*  /   /   \        Universite Paris 6 - Pierre et Marie Curie                 *
* +---+ \   \                                                                  *
*  \   \ +---+                 Address : LIP6                                  *
*   \   /   /                            4, place Jussieu                      *
*    \ / \ /                             75252 Paris Cedex 05                  *
*     +---+                              France                                *
*                                  Web : www-asim.lip6.fr                      *
*                              Support : alliance-support@lip6.fr              *
*                                                                              *
********************************************************************************
*                                                                              *
*  Tool        : TLC v 1.00                                                    *
*  Author(s)   : AVOT Gregoire                                                 *
*  Last update : 20 Mai 1998                                                   *
*                                                                              *
*******************************************************************************/

#include AVT_H
#include MUT_H
#include MLO_H
#include RCN_H
#include TLC_H
#include <stdlib.h>

float TLC_COEF_CTC = 1.0;
float TLC_MAXSCALE_CAPAFACTOR = 1.0;
float TLC_MINSCALE_CAPAFACTOR = 1.0;
float TLC_MAXSCALE_RESIFACTOR = 1.0;
float TLC_MINSCALE_RESIFACTOR = 1.0;
float TLC_CAPAFACTOR = 1.0;
float TLC_RESIFACTOR = 1.0;

void tlc_setmaxfactor()
{
 TLC_RESIFACTOR = TLC_MAXSCALE_RESIFACTOR ;
 TLC_CAPAFACTOR = TLC_MAXSCALE_CAPAFACTOR ;
}

void tlc_setminfactor()
{
 TLC_RESIFACTOR = TLC_MINSCALE_RESIFACTOR ;
 TLC_CAPAFACTOR = TLC_MINSCALE_CAPAFACTOR ;
}

float tlc_getcapafactor()
{
 return(TLC_CAPAFACTOR) ;
}

float tlc_getresifactor()
{
 return(TLC_RESIFACTOR) ;
}

void tlcenv()
{
     TLC_MAXSCALE_CAPAFACTOR = V_FLOAT_TAB[__TAS_MAXSCALE_CAPAFACTOR].VALUE ;

     TLC_MINSCALE_CAPAFACTOR = V_FLOAT_TAB[__TAS_MINSCALE_CAPAFACTOR].VALUE;

     TLC_MAXSCALE_RESIFACTOR = V_FLOAT_TAB[__TAS_MAXSCALE_RESIFACTOR].VALUE ;

     TLC_MINSCALE_RESIFACTOR = V_FLOAT_TAB[__TAS_MINSCALE_RESIFACTOR].VALUE ;
}

tlc_loconparam	*HEAP_PRM	= NULL;
int		MAX_HEAP_PRM	= 8;

void    tlc_mergecapa( dst, src )
locon_list      *dst;
locon_list      *src;
{
  tlc_loconparam        *tlcdst;
  tlc_loconparam        *tlcsrc;
  
  tlcdst = tlc_getloconparam( dst ); 
  tlcsrc = tlc_getloconparam( src );

  if( !tlcdst )
  {
    tlc_setloconparam( dst, 0.0 );
    tlcdst = tlc_getloconparam( dst ); 
  }

  if( tlcsrc )
    tlc_replaceloconparam( dst, tlcsrc->CAPA + tlcdst->CAPA );
}

void    tlc_setloconparam( ptlocon, c )
locon_list	*ptlocon;
float            c;
{
  tlc_loconparam	*newprm;
  int		 i;

  #ifdef TLC_CHECK
  if( getptype( ptlocon->USER, TLC_LOCONPRM ) )
  {
    fflush ( stdout );
    fprintf( stderr, "*** TLC error *** in tlc_setloconparam\n" );
    fprintf( stderr,
             "connector %s have already electrical parameters\n",
             ptlocon->NAME
           );
    EXIT(1);
  }
  #endif

  if(!HEAP_PRM)
  {
    HEAP_PRM=(tlc_loconparam*)mbkalloc( sizeof(tlc_loconparam)*MAX_HEAP_PRM );
    newprm=HEAP_PRM;

    for(i=1;i<MAX_HEAP_PRM;i++)
    {
      newprm->NEXT=newprm+1;
      newprm++;
    }
    newprm->NEXT=NULL;
  }

  newprm=HEAP_PRM;
  HEAP_PRM=HEAP_PRM->NEXT;

  newprm->NEXT=NULL;
  newprm->CAPA=c;

  ptlocon->USER=addptype(ptlocon->USER, TLC_LOCONPRM,newprm);
}

tlc_loconparam*      tlc_getloconparam( ptlocon )
locon_list	*ptlocon;
{
  ptype_list	*pt;

  pt=getptype(ptlocon->USER,TLC_LOCONPRM);
  
  if(pt)
    return((tlc_loconparam*)(pt->DATA));

  return(NULL);
}

void           tlc_delloconparam( ptl )
locon_list    *ptl;
{
  tlc_loconparam *pt ;
  ptype_list	*ptype;

  ptype = getptype( ptl->USER, TLC_LOCONPRM );
  if( ptype == NULL )
    return;

  pt = (tlc_loconparam*)ptype->DATA ;
  pt->NEXT=HEAP_PRM;
  HEAP_PRM=pt;

  ptl->USER = delptype( ptl->USER, TLC_LOCONPRM ) ;
}

void            tlc_replaceloconparam( ptcon, c )
locon_list      *ptcon;
float            c;
{
  ptype_list    *pt;
  tlc_loconparam    *prm;

  pt = getptype( ptcon->USER, TLC_LOCONPRM );

  if( !pt )
  {
    fflush ( stdout );
    fprintf( stderr, "*** TLC error *** in tlc_replaceloconparam()\n" );
    fprintf( stderr,
             "No electrical parameter on locon %s\n",
             ptcon->NAME
           );
    EXIT(1);
  }

  prm = (tlc_loconparam*)(pt->DATA);

  prm->CAPA = c;
}

void            tlc_setlosigalim( ptsig )
losig_list      *ptsig;
{
  ptype_list    *pt;
  
  pt = getptype( ptsig->USER, TLC_ALIM ); 
  if( pt )
    return;

  ptsig->USER = addptype( ptsig->USER, TLC_ALIM, NULL );
}

void            tlc_resetsigalim( ptsig )
losig_list      *ptsig;
{
  #ifdef TLC_CHECK
  ptype_list    *pt;

  pt = getptype( ptsig->USER, TLC_ALIM );

  if( !pt )
  {
    fflush ( stdout );
    fprintf( stderr, "*** TLC error *** in tlc_resetsigalim()\n" );
    fprintf( stderr, "Signal is not typed alim.\n" );
    EXIT(1);
  }
  #endif

  ptsig->USER = delptype( ptsig->USER, TLC_ALIM );
}

int             tlc_islosigalim( ptsig )
losig_list      *ptsig;
{
  if( getptype( ptsig->USER, TLC_ALIM ) )
    return(1);
  return(0);
}
