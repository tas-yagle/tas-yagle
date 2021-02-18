/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : RCX                                                         */
/*    Fichier : trc_fifo.rh                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

typedef struct trc_fifo {
  chain_list    *LIST;
  int            MAXSIZE;
  int            CURSIZE;
} trc_fifo;

trc_fifo* trc_fifocreate( void );
void trc_fifodelete( trc_fifo *fifo );
int trc_fiforemove( trc_fifo *fifo, void *elem );
void trc_fifopush( trc_fifo *fifo, void *elem );
void* trc_fifopop( trc_fifo *fifo );

