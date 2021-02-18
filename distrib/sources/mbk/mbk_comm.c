#include MUT_H
#include AVT_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>

#define COM_BEGIN     0
#define COM_DATA      1
#define COM_TOKEN     2
#define COM_END       3 
#define COM_CHECK     4

#define STAT_COM_FREE    1
#define STAT_COM_BEGIN   2
#define STAT_COM_RUNNING 3
#define STAT_COM_CLOSING 4
#define STAT_COM_CLOSED  5

#define COM_PART_HEADER 1
#define COM_PART_DATA   2 

#define COM_CHECKLOFIG  0
#define COM_CHECKCNSFIG 1
#define COM_CHECKTTVFIG 2

int          MBK_SOCK = -1 ;
unsigned int COM_KEY  = -1 ;

unsigned int calc_crc( char *data, int length );
int          decodeavtdata( struct sockaddr_in *name, int *id, char *avtdata );
void         comm_sendmsg( unsigned int type, char *data, int length );
int          mbk_activatecomslave( char *connexion );
int          opensocket( int *port );
mbk_comdata* mbk_activatecommaster( int n );
void         handleincomingconnection( mbk_comdata *comdata );
int          handleincomingdataslot( mbk_comdata *comdata, int n );
int          mbk_comwaiteventfromnet( mbk_comdata *comdata );
int          mbk_commgetdata( mbk_comdata *comdata, int id, char **data, int *size );
void         mbk_terminatecommaster( mbk_comdata *comdata ); 
int          mbk_comslavegivetoken( char *env, char *token );
void         mbk_commastertoken( comslot *slot );
void         mbk_commastercheck( comslot *slot, unsigned int keyid, unsigned int cryptedkey );
void         mbk_comcheck( unsigned int keylofig, unsigned int keycnsfig, unsigned int keyttvfig ) ;
unsigned int mbk_comcheckkey( int checkid, unsigned int key );

void mbk_comsend( int fd, void *data, int l )
{
  int t ;
  int n ;
  
  n = 0 ;

  do {
    t = send( fd, data+n, l-n, 0 ) ;
    if( t == -1 ) {
      if( errno != EINTR ) {
//        perror( "send() " );
        fprintf(stdout,"%d/%d\n",l,n);
        fprintf(stdout,"%s\n",(char *)data);
        avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 18, strerror(errno) );
      }
    }
    else {
      if( t == 0 ) {
        avt_errmsg( MBK_ERRMSG, "064", AVT_FATAL );
      }
      n = n + t ;
    }
  }
  while( n < l );
}

unsigned int calc_crc( char *data, int length )
{
  unsigned int a,b ;
  int i ;

  a = 0 ;

  for( i=0 ; i<length ; i++ ) {
    b = ((unsigned char)data[i]) ;
    a = ( (a^b)<<1 ) | ( a >> (8*sizeof(int)-1) ) ;
  }
 
  return a ;
}

int decodeavtdata( struct sockaddr_in *name, int *id, char *avtdata )
{
  char *hostname ;
  char *port ;
  char *pt ;
  char *workdata ;
  struct hostent *host ;

  workdata = alloca( sizeof(char)*(strlen(avtdata)+1) );
  strcpy( workdata, avtdata );

  pt = workdata ;

  hostname = pt ;
  pt = strchr( pt, ':' );
  if( !pt ) {
    return 0 ;
  }
  *pt = '\0' ;
  pt++ ;

  port = pt ;
  pt = strchr( pt, ':' );
  if( !pt ) {
    return 0 ;
  }
  *pt = '\0' ;
  pt++ ;

  *id = atoi(pt) ;

  name->sin_family = AF_INET ;
  host = gethostbyname( hostname );
  if( !host ) {
    return 0 ;
  }
  name->sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list[0]))->s_addr ;
  name->sin_port = htons( atoi(port) ) ;

  return 1 ;
}

void comm_sendmsg( unsigned int type, char *data, int length )
{
  unsigned int header[ COM_HEADER_LENGTH ];
  int l ;

  header[0] = htonl( type ) ;
  header[1] = htonl( (unsigned int)length ) ;
  header[2] = htonl( calc_crc( data, length ) ) ;
 
  l = COM_HEADER_LENGTH * sizeof( unsigned int );
  mbk_comsend( MBK_SOCK, header, l );

  if( length > 0 ) {
    mbk_comsend( MBK_SOCK, data, length );
  }
}

void mbk_sendmsg( unsigned int type, char *data, int length )
{
  switch( type ) {
  case MBK_COM_DATA :
    comm_sendmsg( COM_DATA, data, length );
    break ;
  case MBK_COM_BEGIN :
    comm_sendmsg( COM_BEGIN, data, length );
    break ;
  case MBK_COM_END :
    comm_sendmsg( COM_END, NULL, 0 );
    close( MBK_SOCK );
    MBK_SOCK = -1 ;
    break ;
  default :
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 1, "" );
  }
}

/* activatecomfromslave : connect to the master, puts socket in global variable 
   MBK_SOCK.
*/

int mbk_activatecomslave( char *connexion )
{
  struct sockaddr_in    name ;
  int                   sock ;
  int                   t ;
  int                   id ;
  unsigned int          packet[2] ;

  if( !decodeavtdata( &name, &id, connexion ) ) {
    return 0 ;
  }
 
//  printf( "Running avt_shell in slave mode.\n" );

  sock = socket( PF_INET, SOCK_STREAM, 0 );
  if( sock == -1 ) {
//    perror( "socket() " );
    avt_errmsg( MBK_ERRMSG, "054", AVT_ERROR, 2, strerror(errno) );
    return 0 ;
  }
  
  t = connect( sock, (struct sockaddr*)&name, sizeof(name) );
  if( t == -1 ) {
//    perror( "connect() " );
    avt_errmsg( MBK_ERRMSG, "054", AVT_ERROR, 3, strerror(errno) );
    close( sock );
    return 0 ;
  }

  MBK_SOCK = sock ;
  COM_KEY  = rand() ;

  packet[0] = htonl( id ) ;
  packet[1] = htonl( COM_KEY );

  avt_log( LOGMBKCOM, 1, "sending start to master.\n" );
  mbk_sendmsg( MBK_COM_BEGIN, (char*)packet, 2*sizeof( unsigned int ) );

  return 1 ;
}

int opensocket( int *port )
{
  int sock ;
  struct sockaddr_in name ;
  int t ;
 
  sock = socket( PF_INET, SOCK_STREAM, 0 );
  if( sock == -1 ) {
//    perror( "socket() " );
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 4, strerror(errno) );
    return -1 ;
  }
 
  name.sin_family = AF_INET ;
  name.sin_addr.s_addr = htonl( INADDR_ANY );
  name.sin_port = htons( *port ) ;
  do {
    t = bind( sock, (const struct sockaddr*) &name, sizeof( name ) );
    if( t == -1 ) {
      if( errno == EADDRINUSE ) {
        avt_errmsg( MBK_ERRMSG, "055", AVT_WARNING, port );
        sleep( 10 );
      }
      else {
//        perror( "bind() " );
        avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 5, strerror(errno) );
        close( sock );
        return -1 ;
      }
    }
  }
  while( t == -1 );

  t = sizeof( name );
  getsockname( sock, (struct  sockaddr*)&name, &t);
  *port = ntohs(name.sin_port);
  
  t = listen( sock, 5 );
  if( t == -1 ) {
//    perror( "listen() " );
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 6, strerror(errno) );
    return -1 ;
  }

  return sock ;
}

mbk_comdata* mbk_activatecommaster( int n )
{
  mbk_comdata *comdata ;
  int          i ;

  if( MBK_SOCK != -1 ) {
    avt_errmsg( MBK_ERRMSG, "056", AVT_FATAL );
  }
  
  comdata          = mbkalloc( sizeof( mbk_comdata ) ) ;
  comdata->n       = n ;
  comdata->tabslot = mbkalloc( n*sizeof( comslot ) ) ;
  
  for( i=0 ; i<comdata->n ; i++ ) {
    comdata->tabslot[i].state  = STAT_COM_FREE ;
    comdata->tabslot[i].event  = MBK_COM_NONE ;
    comdata->tabslot[i].fd     = -1 ;
    comdata->tabslot[i].id     = -1 ;
  }
  
  comdata->port = 0 ;
  comdata->sock = opensocket( &(comdata->port) );
  if( comdata->sock < 0 )
    return NULL ;

  avt_log( LOGMBKCOM, 1, "listening results on port %d\n", comdata->port );

  return comdata ;
}

void handleincomingconnection( mbk_comdata *comdata )
{
  int i;
  int x ;
  struct sockaddr origin ;
  
  for( i=0 ; i<comdata->n ; i++ ) {
    if( comdata->tabslot[i].state == STAT_COM_FREE ) {
      avt_log( LOGMBKCOM, 1, "available communication slot #%d for incomming connection\n", i );
      break ;
    }
  }
  
  if( i>=comdata->n ) {
    avt_errmsg( MBK_ERRMSG, "057", AVT_FATAL );
  }

  comdata->tabslot[i].state = STAT_COM_BEGIN ;
  do {
    x = sizeof( origin );
    comdata->tabslot[i].fd = accept( comdata->sock, (struct sockaddr*)&origin, &x );
    if( comdata->tabslot[i].fd == -1 ) {
      if( errno != EINTR ) {
//        perror( "accept " );
        avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 7, strerror(errno) );
      }
    }
  }
  while( comdata->tabslot[i].fd == -1 );

  comdata->tabslot[i].total   = COM_HEADER_LENGTH * sizeof( unsigned int );
  comdata->tabslot[i].current = 0 ;
  comdata->tabslot[i].id      = -1 ;
  comdata->tabslot[i].part    = COM_PART_HEADER ;

  avt_log( LOGMBKCOM, 1, "new incoming connection\n" );
}

int handleincomingdataslot( mbk_comdata *comdata, int n ) 
{
  char         *ptbuf ;
  int           max ;
  unsigned int  crc ;
  int           r ;
  int           event ;
  
  max = comdata->tabslot[n].total - comdata->tabslot[n].current ;
  if( comdata->tabslot[n].part == COM_PART_HEADER )
    ptbuf = (char*)(comdata->tabslot[n].header);
  else 
    ptbuf = comdata->tabslot[n].buffer ;

  event = 0 ;

  r = recv( comdata->tabslot[n].fd, ptbuf + comdata->tabslot[n].current, max, 0 );
 
  if( r==-1 ) {
    if( errno != EINTR ) {
//      perror( "recv() " );
//      printf("%d %d/%d\n",n,comdata->tabslot[n].current,max);
      avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 8, strerror(errno) );
    }
    return event ;
  }
  if( r == 0 ) { /* the client has closed the connection */
  
    if( comdata->tabslot[n].state == STAT_COM_CLOSING ) {
      avt_log( LOGMBKCOM, 1, "client job %d disconnected properly\n", comdata->tabslot[n].id );
      comdata->tabslot[n].state = STAT_COM_CLOSED ;
      comdata->tabslot[n].event = MBK_COM_END ;
      event = 1 ;
    }
    else {
      avt_log( LOGMBKCOM, 1, "client job %d disconnected abnormaly\n", comdata->tabslot[n].id );
      comdata->tabslot[n].state = STAT_COM_CLOSED ;
      comdata->tabslot[n].event = MBK_COM_ABORT ;
      event = 1 ;
    }

    close( comdata->tabslot[n].fd ) ;
    comdata->tabslot[n].fd = -1 ;
  }
  else {

    if( r + comdata->tabslot[n].current > comdata->tabslot[n].total ) { 
      /* overflow detection (impossible) */
      avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 9, "" );
    }

    comdata->tabslot[n].current = comdata->tabslot[n].current + r ;

    if( comdata->tabslot[n].current == comdata->tabslot[n].total ) { 
      /* data enterly received */

      if( comdata->tabslot[n].part == COM_PART_HEADER ) {

        comdata->tabslot[n].type    = ntohl( comdata->tabslot[n].header[0] ) ;
        comdata->tabslot[n].total   = ntohl( comdata->tabslot[n].header[1] ) ;
        comdata->tabslot[n].crc     = ntohl( comdata->tabslot[n].header[2] ) ;

        avt_log( LOGMBKCOM, 1, "header received from %d (%d)\n", comdata->tabslot[n].id, comdata->tabslot[n].type);

        switch( comdata->tabslot[n].type ) {
        case COM_BEGIN :
          comdata->tabslot[n].buffer = (char*)comdata->tabslot[n].header ;
          comdata->tabslot[n].part = COM_PART_DATA ;
          break ;
        case COM_DATA :
          comdata->tabslot[n].buffer = mbkalloc( comdata->tabslot[n].total );
          comdata->tabslot[n].part = COM_PART_DATA ;
          break ;
        case COM_TOKEN :
          comdata->tabslot[n].buffer = mbkalloc( comdata->tabslot[n].total );
          comdata->tabslot[n].part = COM_PART_DATA ;
          break ;
        case COM_CHECK :
          comdata->tabslot[n].buffer = (char*)comdata->tabslot[n].header ;
          comdata->tabslot[n].part = COM_PART_DATA ;
          break ;
        case COM_END :
          comdata->tabslot[n].state = STAT_COM_CLOSING ;
          comdata->tabslot[n].part  = COM_PART_HEADER ;
          comdata->tabslot[n].total = 1 ;
          break ;
        }
      }
      else {
        avt_log( LOGMBKCOM, 1, "data received from %d\n", comdata->tabslot[n].id );
        switch( comdata->tabslot[n].state ) {
        
        case STAT_COM_BEGIN :
      
          comdata->tabslot[n].id  = ntohl( comdata->tabslot[n].header[0] );
          comdata->tabslot[n].key = ntohl( comdata->tabslot[n].header[1] );
          comdata->tabslot[n].state = STAT_COM_RUNNING ;
          comdata->tabslot[n].event = MBK_COM_BEGIN ;
          event = 1 ;
          avt_log( LOGMBKCOM, 1, "identified begin from %d (%d)\n", comdata->tabslot[n].id, n );
          break ;

        case STAT_COM_RUNNING :
        
          crc = calc_crc( comdata->tabslot[n].buffer, comdata->tabslot[n].total ) ;
          if( crc != comdata->tabslot[n].crc )
            avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 10, "" );

          switch( comdata->tabslot[n].type ) {
          case COM_DATA :
            comdata->tabslot[n].event = MBK_COM_DATA ;
            comdata->tabslot[n].size = comdata->tabslot[n].total ;
            event = 1 ;
            break ;
          case COM_TOKEN :
            avt_log( LOGMBKCOM, 1, "token requested from %d : %s\n", 
                    comdata->tabslot[n].id,
                    comdata->tabslot[n].buffer
                  );
            mbk_commastertoken( &(comdata->tabslot[n]) ); 
            mbkfree( comdata->tabslot[n].buffer );
            break ;
          case COM_CHECK :
            mbk_commastercheck( &(comdata->tabslot[n]),
                                ntohl( comdata->tabslot[n].header[0] ),
                                ntohl( comdata->tabslot[n].header[1] )  
                              );
            break ;
          default :
            avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 11, "" );
            EXIT(1);
          }

          break ;

        default :
          avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 12, "" );
          break ;
        }

        comdata->tabslot[n].part  = COM_PART_HEADER ;
        comdata->tabslot[n].total = COM_HEADER_LENGTH * sizeof( unsigned int );
      }
      comdata->tabslot[n].current = 0 ;
    }
  }

  return event ;
}

int mbk_comwaiteventfromnet( mbk_comdata *comdata )
{
  fd_set set ;
  int    i ;
  int    max ;
  int    ret ;

  if( MBK_SOCK != -1 ) {
    avt_errmsg( MBK_ERRMSG, "056", AVT_FATAL );
  }
  ret = MBK_COM_NOEVENT ;
  
  FD_ZERO( &set );
  FD_SET( comdata->sock, &set );
  max = comdata->sock ;
  
  for( i=0 ; i < comdata->n ; i++ ) {
  
    if( comdata->tabslot[i].state == STAT_COM_BEGIN   ||
        comdata->tabslot[i].state == STAT_COM_RUNNING ||
        comdata->tabslot[i].state == STAT_COM_CLOSING    ) {
        
      if( comdata->tabslot[i].event == MBK_COM_NONE ) {
      
        FD_SET( comdata->tabslot[i].fd, &set );
//              fprintf(stdout,"=> %d: %d / %d\n", i, comdata->tabslot[i].fd, comdata->n);
        if( comdata->tabslot[i].fd > max )
          max = comdata->tabslot[i].fd ;
      }
    }
  }
  
  do {
    i = select( max+1, &set, NULL, NULL, NULL ) ;
    if( i==-1 ) {
      if( errno != EINTR ) {
//        perror( "select() " );
        avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 13, strerror(errno) );
      }
    }
  }
  while( i==-1 );

  if( FD_ISSET( comdata->sock, &set ) ) {
    handleincomingconnection( comdata );
  }
  else { 
  
    for( i=0 ; i<comdata->n ; i++ ) {
    
      if( comdata->tabslot[i].state != STAT_COM_FREE ) {
        if( FD_ISSET( comdata->tabslot[i].fd, &set ) )
          if( handleincomingdataslot( comdata, i ) ) {
            ret = comdata->tabslot[i].id ;
            break ;
        }
      }
    }
  }

  return ret ;
}

int mbk_commgetdata( mbk_comdata *comdata, int id, char **data, int *size ) 
{
  int i ;
  int event ;
  char buf[32];

  if( MBK_SOCK != -1 )
    avt_errmsg( MBK_ERRMSG, "056", AVT_FATAL );

  event = MBK_COM_NONE ;
  
  for( i=0 ; i<comdata->n ; i++ ) {
  
    if( comdata->tabslot[i].state != STAT_COM_FREE ) {
    
      if( comdata->tabslot[i].id == id ) {
      
        event                     = comdata->tabslot[i].event ;
        comdata->tabslot[i].event = MBK_COM_NONE ;
        *data                     = comdata->tabslot[i].buffer ;
        *size                     = comdata->tabslot[i].size;
        
        if( comdata->tabslot[i].state == STAT_COM_CLOSED ) {
          comdata->tabslot[i].state = STAT_COM_FREE ;
          comdata->tabslot[i].id     = -1 ;
        }
          
        break ;
      }
    }
  }

  if( i >= comdata->n )
  {
    sprintf(buf, "job %d", id);
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 14, buf );
  }
    
  if( event == MBK_COM_NONE )
  {
    sprintf(buf, "job %d", id);
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 15, buf );
  }

  return event ;
}

void mbk_terminatecommaster( mbk_comdata *comdata ) 
{
  int i ;
  for( i=0 ; i<comdata->n ; i++ ) {
    if( comdata->tabslot[i].fd != -1                ||
        comdata->tabslot[i].state != STAT_COM_FREE  ||
        comdata->tabslot[i].event != MBK_COM_NONE      ) {
      avt_errmsg( MBK_ERRMSG, "060", AVT_WARNING );
    }
  }

  mbkfree( comdata->tabslot );
  mbkfree( comdata );
}

unsigned int rollkey( unsigned int i )
{
  unsigned int j ;

  j = i ^ ( i << 3 ) ^ ( i >> 7 ) ^ ( i << 17 ) ^ ( i >> 27 ) ;

  return j ;
}

void mbk_comrecvslavemessage( void *buffer, int l )
{
  int r ;
  int t ;
  int max ;

  r=0 ;

  while( r < l ) {
    max = l - r ;
    t = recv( MBK_SOCK, buffer + r, max, 0 );
    if( t<0 ) {
      if( errno != EINTR ) {
//        perror( "recv() " );
        avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 16, strerror(errno) );
      }
    }
    if( t==0 ) 
    {
      fprintf(stdout,"max=%d l=%d\n",max, l);
      avt_errmsg( MBK_ERRMSG, "064", AVT_FATAL );
    }
    r = r + t ;
  }
}

int mbk_comslavegivetoken( char *env, char *token )
{
  char buffer[1024];
  unsigned int answer ;

  avt_log( LOGMBKCOM, 1, "sending token request \"%s\" to master\n", token );
  
  sprintf( buffer, "%s %s", env, token );
  comm_sendmsg( COM_TOKEN, buffer, strlen( buffer )+1 );
  mbk_comrecvslavemessage( (void*)&answer, sizeof(int) );
  
  answer = ntohl( answer );

  COM_KEY = rollkey( COM_KEY );
  
  avt_log( LOGMBKCOM, 1, "  com : answer from token request : 0x%08X - 0x%08X\n", answer, COM_KEY );
  if( answer == COM_KEY )
    return AVT_VALID_TOKEN ;
  return 0 ;
}

void mbk_commastertoken( comslot *slot )
{
  char          *pt ;
  char          *env ;
  char          *tool ;
  int            t ;
  unsigned int   nkey ;

  pt = strchr( slot->buffer, (int)' ' );
  if( !pt )
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 17, "" );
  
  *pt = '\0' ;

  env = slot->buffer ;
  tool = pt+1 ;

#ifdef AVERTEC_LICENSE
  t = avt_givetoken( env, tool );
#else
  t = AVT_VALID_TOKEN ;
#endif

  if( t == AVT_VALID_TOKEN )
    slot->key = rollkey( slot->key );

  nkey = htonl( slot->key );
  
  mbk_comsend( slot->fd, &nkey, sizeof(unsigned int) );
}

unsigned int mbk_comcheckkey( int checkid, unsigned int key )
{
  unsigned int data[2] ;
  unsigned int answer ;
  unsigned int cryptedanswer ;
  char *label[]={"lofig","cnsfig","ttvfig"};
  unsigned int cryptedkey ;
  
  cryptedkey = key ^ COM_KEY ;

  data[0] = htonl(checkid) ;
  data[1] = htonl(cryptedkey) ;
  
  comm_sendmsg( COM_CHECK, (void*)data, 2*sizeof(unsigned int) );
  mbk_comrecvslavemessage( (void*)&cryptedanswer, sizeof(int) );
  
  COM_KEY = rollkey( COM_KEY );

  cryptedanswer = ntohl( cryptedanswer );
  answer = cryptedanswer ^ COM_KEY ;

  avt_log( LOGMBKCOM, 1, "resquested %s key : 0x%08X, returned key : 0x%08X\n",
          label[checkid],
          key,
          answer
        ) ;
  return answer ;
}

void mbk_comcheck( unsigned int keylofig,
                   unsigned int keycnsfig,
                   unsigned int keyttvfig
                 )
{
  static unsigned int validkeylofig  = 0 ;
  static unsigned int validkeycnsfig = 0 ;
  static unsigned int validkeyttvfig = 0 ;

  if( MBK_SOCK == -1 )
    return ;

  if( keylofig  != 0 ) {
    if( validkeylofig == 0 )
      validkeylofig = mbk_comcheckkey( COM_CHECKLOFIG, keylofig );
    if( keylofig != validkeylofig )
      avt_errmsg( MBK_ERRMSG, "061", AVT_FATAL );
  }

  if( keycnsfig  != 0 ) {
    if( validkeycnsfig == 0 )
      validkeycnsfig = mbk_comcheckkey( COM_CHECKCNSFIG, keycnsfig );
    if( keycnsfig != validkeycnsfig )
      avt_errmsg( MBK_ERRMSG, "062", AVT_FATAL );
  }

  if( keyttvfig  != 0 ) {
    if( validkeyttvfig == 0 )
      validkeyttvfig = mbk_comcheckkey( COM_CHECKTTVFIG, keyttvfig );
    if( keyttvfig != validkeyttvfig ) 
      avt_errmsg( MBK_ERRMSG, "063", AVT_FATAL );
  }
}

void mbk_commastercheck( comslot *slot, 
                         unsigned int keyid, 
                         unsigned int cryptedkey 
                       )
{
  static unsigned int validkeylofig  = 0 ;
  static unsigned int validkeycnsfig = 0 ;
  static unsigned int validkeyttvfig = 0 ;
  unsigned int        key ;
  unsigned int        skey ;
  unsigned int        cryptedskey ;
  char               *label ;

  key = cryptedkey ^ slot->key ;

  slot->key = rollkey( slot->key );

  switch( keyid ) {
  
  case COM_CHECKLOFIG :
    label = "lofig" ;
    if( validkeylofig == 0 )
      validkeylofig = key ;
    skey = validkeylofig ;
    break ;
    
  case COM_CHECKCNSFIG :
    label = "cnsfig" ;
    if( validkeycnsfig == 0 )
      validkeycnsfig = key ;
    skey = validkeycnsfig ;
    break ;
    
  case COM_CHECKTTVFIG :
    label = "ttvfig" ;
    if( validkeyttvfig == 0 )
      validkeyttvfig = key ;
    skey = validkeyttvfig ;
    break ;
  }

  avt_log( LOGMBKCOM, 1, "check key from job %d for %s. requested 0x%X, previous 0x%X\n",
          slot->id,
          label,
          key,
          skey
        );

  cryptedskey = skey ^ slot->key ;
  cryptedskey = htonl( cryptedskey );
  mbk_comsend( slot->fd, &cryptedskey, sizeof(unsigned int) );
}

void mbk_commputdata( mbk_comdata *comdata, int id, char *data, int l ) 
{
  int i ;
  int event ;
  char buf[32];

  if( MBK_SOCK != -1 )
    avt_errmsg( MBK_ERRMSG, "056", AVT_FATAL );

  for( i=0 ; i<comdata->n ; i++ ) {
  
    if( comdata->tabslot[i].state != STAT_COM_FREE ) {
    
      if( comdata->tabslot[i].id == id ) {
        int zs=l, conv;
        conv=htonl(zs);
        mbk_comsend( comdata->tabslot[i].fd, (char *)&conv, sizeof(int));
        mbk_comsend( comdata->tabslot[i].fd, data, l);
        break ;
      }
    }
  }

  if( i >= comdata->n )
  {
    sprintf(buf, "job %d", id);
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 14, buf );
  }
    
  if( event == MBK_COM_NONE )
  {
    sprintf(buf, "job %d", id);
    avt_errmsg( MBK_ERRMSG, "054", AVT_FATAL, 15, buf );
  }
}

char *mbk_commtalk( char *msg, int size )
{
  char *res;
  int nb, conv;
  mbk_sendmsg( MBK_COM_DATA, msg, size>=0?size:strlen(msg)+1);
  mbk_comrecvslavemessage((char *)&conv, sizeof(int));
  nb=ntohl(conv);
  res=malloc(nb);
  mbk_comrecvslavemessage(res, nb);
  return res;
}

int mbk_comm_send_text_file(char *name)
{
  char *res;
  int ok, nb;
  char buf[16030];
  FILE *f;
  if (MBK_SOCK != -1 )
  { 
    sprintf(buf, "ask:getsetflag:%s", name);
    res=mbk_commtalk(buf, -1); ok=(strcmp(res, "ok")==0); free(res);
    if (ok)
    {
      if ((f=mbkfopen(name, NULL, "r"))==NULL)
        avt_errmsg( MBK_ERRMSG, "068", AVT_FATAL, name );
      res=strrchr(name,'/');
      if (res!=NULL) res++;
      else res=name;
      sprintf(buf, "ask:createfile:%s", res);
      res=mbk_commtalk(buf, -1); ok=(strcmp(res, "ok")==0); free(res);
      if (ok)
      {
        strcpy(buf,"ask:writefile:");
//        while (ok && nb=fgets(&buf[14], 16000, f)!=NULL)
        while (ok && (nb=fread(&buf[14], sizeof(char), 16000, f))>0)
        {
          buf[14+nb]='\0';
          res=mbk_commtalk(buf, -1); ok=(strcmp(res, "ok")==0); free(res);
        }
        if (ok)
        {
           res=mbk_commtalk("ask:closefile", -1);
           free(res);
        }
      }
      if (!ok)
        avt_errmsg( MBK_ERRMSG, "068", AVT_FATAL, name );
    }
  }
 return ok; 
}

