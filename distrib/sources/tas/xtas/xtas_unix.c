/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_unix.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

int tas_pfd[2];
int tas_out[2];
sigjmp_buf XtasMyEnv;
static void     (*XtasOldExitHandler) () = NULL;
char XTAS_DEBUG_MODE = 'N' ;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSetBlock                                                   */
/*                                                                           */
/* IN  ARGS : .fd : The file descriptor.                                     */
/*            .on : Indicator of a blocking read or not.                     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSetBlock( fd, on )
int fd, on;
{
static int blockf, nonblockf;
static int first = True;
int flags;

if ( first == True )
    {
    first = False;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        {
        fprintf(stderr,"ERREUR FCNTL\n");fflush(stderr);
                                                                   
        }
    blockf    = flags & ~O_NDELAY;
    blockf    = flags & ~O_NONBLOCK;
    nonblockf = flags |  O_NDELAY;
    nonblockf = flags |  O_NONBLOCK;
    }

if (fcntl(fd, F_SETFL, on ? blockf : nonblockf ) == -1)
    {
    fprintf(stderr,"ERREUR FCNTL\n");fflush(stderr);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasInitFileErr                                                */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Catch stderr                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasInitFileErr()
{
 char *str ;

 str = getenv("XTAS_DEBUG_MODE") ;
 if(str != NULL)
    {
    XTAS_DEBUG_MODE = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
    }

 if (pipe( tas_pfd ) != -1)
  {
   close(2);
   dup2( tas_pfd[1],2 );
   close( tas_pfd[1] );
  }

 if (pipe( tas_out ) != -1)
  {
   close(1);
   dup2( tas_out[1],1 );
   close( tas_out[1] );
  }

if(XTAS_DEBUG_MODE == 'N')
 {
 signal( SIGBUS,  XtasFatalSysError );
 signal( SIGSEGV, XtasFatalSysError );
 }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetStdoutMess                                              */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Catch messages of stdout from the pipe.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetStdoutMess()
{
static char buffer[4096];
static int  i   = 0;

fflush( stderr );
i = read(tas_pfd[0],buffer,4095) ;

if(i == -1) 
  strcpy(buffer,"Fatal Error Occuring While Reading File") ;
else
  buffer[i] = '\0' ;
    
if(strlen(buffer) == 0)
  return ;

 XtasFatalAppError(buffer) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPasqua                                                     */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Activates the stdout watching.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPasqua()
{
/*---------------------------------*/
/* Catch all stdout messages !!!   */
/*---------------------------------*/
 XtasSetBlock( 2, False );
 XtasSetBlock( 1, False );
 XtasOldExitHandler = signal( SIGTERM, XtasGetStdoutMess );
 XtasClearAllStream (); /* avoid warnings from Motif */
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFirePasqua                                                 */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Deactivates the stdout watching.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFirePasqua()
{
char c;
int  nb = 1;

XtasSetLabelString( XtasDeskMessageField, XTAS_NULINFO );

/*-------------------*/
/* Clean up the pipe */
/*-------------------*/
while ((nb != -1) && (nb != 0))
    {
    XtasSetBlock( tas_pfd[0], False );
    nb = read(tas_pfd[0], &c, 1);
    }

nb = 1;
while ((nb != -1) && (nb != 0))
    {
    XtasSetBlock( tas_out[0], False );
    nb = read(tas_out[0], &c, 1);
    }

 signal(SIGTERM, XtasOldExitHandler);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFatalSysError                                              */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of System Fatal Errors.                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFatalSysError()
{
XtasDestroyAllFig() ;
XtasFirePasqua();

XalDrawMessage( XtasErrorWidget, XTAS_FSYSERR );
XtCloseDisplay( XtDisplay( XtasTopLevel ) );

XtasExit(1) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasExit                                                       */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of exit                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasExit(x)
int x ;
{
MBK_EXIT_KILL = 'N' ;
XyagExitErrorMessage(x);
EXIT(x);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFatalAppError                                              */
/*                                                                           */
/* IN  ARGS : .message : The message to be drawn to the user.                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of Application's Fatal Errors.                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFatalAppError( message )
char *message;
{
XtasDestroyAllFig() ;
XtasDestroyStbInProgress();
XtasFirePasqua();

if ( XtasErrorWidget )
    {
    XtasSetLabelString( XtasDeskMessageField, XTAS_FAPPERR );
    XalForceUpdate(  XtasDeskMessageField  );
    XalDrawMessage( XtasErrorWidget, message );
    }

if (XyagJmpEnv != NULL)
    siglongjmp(*XyagJmpEnv, 1);
else    
    siglongjmp(XtasMyEnv, 1);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetWarningMess                                             */
/*                                                                           */
/* IN  ARGS : .message : The message to be drawn to the user.                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of Application's Warning                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetWarningMess( )
{
char buffer[4096];
int  i   = 0;
char *pt ;

fprintf(stderr,"\n") ;
fflush( stderr );
i = read(tas_pfd[0],buffer,4095) ;

if(i == -1)
  return ;

buffer[i] = '\0' ;


pt = buffer ;

while((isspace((int)*pt) != 0) && (*pt != '\0'))
  pt++ ;

if(strlen(pt) == 0)
  return ;

if ( XtasWarningWidget )
    {
    XalForceUpdate(  XtasDeskMessageField  );
    XalDrawMessage( XtasWarningWidget, pt );
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetOutMess                                                 */
/*                                                                           */
/* IN  ARGS : .message : The message to be drawn to the user.                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of Application's Warning                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetOutMess( )
{
char buffer[65535];
unsigned int  i   = 0;
char *pt ;

fprintf(stdout,"\n") ;
fflush( stdout );
i = read(tas_out[0],buffer,65534) ;

if(i == -1)
  return ;

buffer[i] = '\0' ;

pt = buffer ;

//while((isspace((int)*pt) != 0) && (*pt != '\0'))
//  pt++ ;

if(strlen(pt) == 0)
  return ;

if ( XtasTraceWidget )
    {
    XalForceUpdate(  XtasDeskMessageField  );
    XalDrawMessage( XtasTraceWidget, pt );
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasClearAllStream                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasClearAllStream( )
{
char buffer[65536];

fprintf(stdout,"\n") ;
fflush( stdout );
read(tas_out[0],buffer,65536) ;
fprintf(stderr,"\n") ;
fflush( stderr );
read(tas_pfd[0],buffer,65536) ;

}

