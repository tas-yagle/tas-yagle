/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : RCX                                                         */
/*    Fichier : trc_fifo.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.6 $
Author   : $Author: gregoire $
Date     : $Date: 2002/07/02 13:22:22 $

*/

#include "trc.h"

// Création d'une fifo de taille initiale 'taille'

trc_fifo* trc_fifocreate( void )
{
  trc_fifo *fifo;

  fifo       = (trc_fifo*)mbkalloc( sizeof( trc_fifo ) );
  fifo->LIST = NULL;
  fifo->CURSIZE = 0;
  fifo->MAXSIZE = 0;
  return( fifo );
}

// Effacement de la fifo

void trc_fifodelete( trc_fifo *fifo )
{
  if( fifo->LIST )
    freechain( fifo->LIST );
  mbkfree( fifo );
}

// Effacement d'un élément à l'intérieur de la fifo

int trc_fiforemove( trc_fifo *fifo, void *elem )
{
  chain_list *scan, *prev=NULL;

  if( !fifo || !fifo->LIST ) return 0;
  
  for( scan = fifo->LIST ; scan ; scan = scan->NEXT ) {
    if( scan->DATA == elem )
      break;
    prev = scan;  
  }

  if( !scan )
    return(0);
 
  if( prev )
    prev->NEXT = scan->NEXT;
  else
    fifo->LIST = scan->NEXT;
  scan->NEXT = NULL;
  freechain( scan );

  fifo->CURSIZE--;
  return(1);
}

// Push

void trc_fifopush( trc_fifo *fifo, void *elem )
{
  chain_list *scan;
  
  if( !fifo->LIST ) {
  
    fifo->LIST = addchain( NULL, elem );
    
  } else {
  
    for( scan = fifo->LIST ; scan->NEXT ; scan = scan->NEXT );
    scan->NEXT = addchain( NULL, elem );
  }

  fifo->CURSIZE++;
  if( fifo->CURSIZE > fifo->MAXSIZE )
    fifo->MAXSIZE = fifo->CURSIZE;
}

// Pop

void* trc_fifopop( trc_fifo *fifo )
{
  chain_list *head;
  void       *elem;
  
  if( !fifo->LIST )
    return( NULL );
    
  head = fifo->LIST;
  if( head )
    fifo->LIST = fifo->LIST->NEXT;
  
  head->NEXT = NULL;
  elem=head->DATA;
  freechain( head );

  fifo->CURSIZE--;
  return( elem );
}
