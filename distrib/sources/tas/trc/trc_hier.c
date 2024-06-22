/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_hier.c                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#include "trc.h"

/* CVS informations :

Revision : $Revision: 1.10 $
Author   : $Author: gregoire $
Date     : $Date: 2003/10/27 13:06:07 $

*/

losig_list* rcx_getdownlosig( losig_list *signal, 
                              lofig_list *figure,
                              char       **insname,
                              chain_list **figlist
                            );
losig_list* rcx_getuplosig( losig_list *signal,
                            char       *nominstance,
                            chain_list *chainfig,
                            char       **insnametop,
                            lofig_list **lofig
                          );
chain_list* rcx_getchainfig( char *nomhier ) ;

/*

Récupère un signal à son plus haut niveau de hiérarchie où il est défini.
Renvoie NULL si il n'existe pas : cas où il est connecté à une alimentation.

Arguments :

Entrée :
  signal : le signal.
  insname  = nom d'instance de la figure courante.
  chainfig = liste chainée des figures, du niveau courant jusqu'au plus haut
             niveau de la hiérarchie. Cette liste est automatiquement libérée.
           
retour     = le losig

*/

losig_list* rcx_gethierlosig( losig_list *signal,
                              char       *insname, 
                              chain_list *chainfig,
                              char       **siginsname,
                              lofig_list **siglofig
                            )
{
  char       *downinsname;
  chain_list *downfiglist;
  losig_list *downsignal;
  chain_list *localchainfig;

  // Dans la plupart des cas, le signal est local
  if( !rcx_isbellow( signal ) &&
      ( signal->TYPE == INTERNAL || chainfig->NEXT == NULL )
    ) {
    *siginsname = insname ;
    *siglofig = (lofig_list*)(chainfig->DATA) ;
    return( signal );
  }

  // On cherche d'abord en dessous
  downsignal = rcx_getdownlosig( signal,
                                 (lofig_list*)(chainfig->DATA),
                                 &downinsname,
                                 &downfiglist
                               );
  
  if( downsignal != signal ) {
    insname = concatname( insname, downinsname );
    localchainfig = dupchainlst( chainfig );
    localchainfig = append( downfiglist, localchainfig );
  }
  else
    localchainfig = chainfig ;
  
  // Puis on remonte
  signal = rcx_getuplosig( downsignal, 
                           insname, 
                           localchainfig, 
                           siginsname, 
                           siglofig 
                         );

  if( localchainfig != chainfig )
    freechain( localchainfig );
  
  return( signal );
}

/*

Renvoie le losig au niveau au niveau ou il est défini (pas de BELLOW).

Arguments :

Entrée :
  signal : le signal dans l'instance courante.
  figure : la figure courante dans laquelle est déclaré le signal.
Sortie :
  insname : une chaîne de caractère qui contiendra le nom d'instance relatif
            où a été trouvé la définition du losig.
  figlist : liste chaînée des figure où est défini le losig.
          
*/

losig_list* rcx_getdownlosig( losig_list *signal, 
                              lofig_list *figure,
                              char       **insname,
                              chain_list **figlist
                            )
{
  char          *nomsignal ;
  char          *nominstance ;
  
  *figlist = NULL;
  *insname = NULL;

  while( ( nomsignal = rcx_isbellow( signal ) ) ) {
  
    leftunconcatname( nomsignal, &nominstance, &nomsignal );
    
    figure = rcx_getlofig( rcx_gethtrcxmod( figure, nominstance ), NULL );

    signal = rcx_gethtrcxsig( NULL, figure, nomsignal );

    *figlist = addchain( *figlist, figure );
    if( *insname )
      *insname = concatname( *insname, nominstance );
    else
      *insname = nominstance ;
  }

  return( signal );
}

/*

Renvoie le losig au niveau maximum ou il existe : soit il devient internal,
soit le dernier niveau de hiérarchie est atteind.
On peut ne pas trouver de signal (la fonction renvoie NULL) si il le connecteur
n'a pas de vue RCX au niveau supérieur. Ce cas arrive lorsqu'on connecte une
entree directement à une alimentation.

Arguments :

Entrée :
  signal : le signal dans l'instance courante.
  nominstance : le nom courant de l'instance.
  chainfig : la liste des lofig (le premier élément est la figure courante).

*/

losig_list* rcx_getuplosig( losig_list *signal,
                            char       *nominstance,
                            chain_list *chainfig,
                            char       **nominstancetop,
                            lofig_list **lofig
                          )
{
  char       *nomsignal;
  char       *nominstancecourante;
  lofig_list *figurecourante;

  figurecourante = (lofig_list*)(chainfig->DATA) ;

  while( rcx_islosigexternal( signal ) && chainfig->NEXT ) {
 
    // Récupère la figure au dessus.
    rightunconcatname( nominstance, &nominstance, &nominstancecourante );
    chainfig = chainfig->NEXT ;
    figurecourante = (lofig_list*)chainfig->DATA;
    
    // Le nom du signal est maintenant hiérarchique.
    nomsignal = rcx_getsigname( signal );
    nomsignal = concatname( nominstancecourante, nomsignal );
    
    signal = rcx_gethtrcxsig( NULL, figurecourante, nomsignal );
    if( !signal ) {
      *nominstancetop = NULL;
      *lofig          = NULL;
      return NULL;
    }
  }

  *nominstancetop = nominstance ;
  *lofig          = figurecourante ;
  return( signal );
}

chain_list* rcx_getchainfig( char *nomhier )
{
  chain_list *head ;
  lofig_list *ptfig ;
  char       *insname ;
  char       *figname ;

  head = NULL;
 
  do
  {
    leftunconcatname( nomhier, &insname, &nomhier );
    
    if( !head ) { // C'est le nom de la figure racine
      if( insname ) 
        ptfig = rcx_getlofig( insname, NULL );
      else
        ptfig = rcx_getlofig( nomhier, NULL );
      
    }
    else if( insname ) {
      
      figname = rcx_gethtrcxmod( ptfig, insname );
      ptfig   = rcx_getlofig( figname, NULL );

    }
    else {
      
      figname = rcx_gethtrcxmod( ptfig, nomhier );
      ptfig   = rcx_getlofig( figname, NULL );

    }

    head = addchain( head, ptfig );
  }
  while( insname );

  return( head );
}
