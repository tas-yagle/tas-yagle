/*------------------------------------------------------------\
|                                                             |
| Tool    :                  XYAG XMT                         |
|                                                             |
| File    :                 Yag Panel.c                       |
|                                                             |
| Author  :              Picault Stephane                     |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include BEH_H
#include BHL_H
#include MUT_H
#include MLO_H
#include LOG_H
#include CGV_H
#include BVL_H
#include BEF_H

#include XSB_H
#include XYA_H
#include XMX_H
#include XTB_H
#include XMT_H
#include INF_H
#include YAG_H

#include "XMT_panel.h"

#include XMV_H
#include XME_H

#include "XME_panel.h"
#include "XME_beh.h"
#include "XMF_file.h"
#include "XMV_view.h"
#include "XYA_cgvfig.h"

extern xyagzoom       *XyagHeadZoom;

static void     (*XyagOldExitHandler) () = NULL;
int xya_pfd[2];
int xya_out[2];

extern int XYADEBUG;

sigjmp_buf XyagMyEnv ;
sigjmp_buf *XyagJmpEnv = NULL ;
void XyagSetJmpEnv(sigjmp_buf *buf) 
{
    XyagJmpEnv = buf;
}

/*------------------------------------------------------------\
|                       XyagInitFileErr                        |
\------------------------------------------------------------*/
void XyagInitFileErr()
{
 char *str ;

 if (XYADEBUG) return;
 if (XyagCallbackFileQuitFonction != NULL) return;
 
 if (pipe( xya_pfd ) != -1)
  {
   close(2);
   dup2( xya_pfd[1],2 );
   close( xya_pfd[1] );
  }

 if (pipe( xya_out ) != -1)
  {
   close(1);
   dup2( xya_out[1],1 );
   close( xya_out[1] );
  }

}

/*------------------------------------------------------------\
|                     XyagGetWarningMess                      |
\------------------------------------------------------------*/
void XyagGetWarningMess( )
{
char buffer[4096];
int  i   = 0;
char *pt ;

if (XYADEBUG) return;
if (XyagCallbackFileQuitFonction != NULL) {
    XyagGetWarningFunction ();
    return;
}

fprintf(stderr,"\n") ;
fflush( stderr );
i = read(xya_pfd[0],buffer,4096) ;
buffer[i] = '\0' ;

if(i == -1)
  return ;

pt = buffer ;

while((isspace((int)*pt) != 0) && (*pt != '\0'))
  pt++ ;

if(strlen(pt) == 0)
  return ;

XyagWarningMessage( XyagMainWindow, pt );
}



/*------------------------------------------------------------\
|                       XyagExit                              |
\------------------------------------------------------------*/
void XyagExit(x)
    int x ;
{
    MBK_EXIT_KILL = 'N' ;
    EXIT(x);
}

/*------------------------------------------------------------\
|                       XyagFirePasqua                        |
\------------------------------------------------------------*/
void XyagFirePasqua()
{
char c;
int  nb = 1;

 if(XyagCallbackFileQuitFonction != NULL) {
     XyagSetJmpEnv (NULL);
     XyagFirePasquaFunction ();
     return; 
 }
/*-------------------*/
/* Clean up the pipe */
/*-------------------*/
while ((nb != -1) && (nb != 0))
    {
    XyagSetBlock( xya_pfd[0], False );
    nb = read(xya_pfd[0], &c, 1);
    }

nb = 1;
while ((nb != -1) && (nb != 0))
    {
    XyagSetBlock( xya_out[0], False );
    nb = read(xya_out[0], &c, 1);
    }

 signal(SIGTERM, XyagOldExitHandler);
}



/*------------------------------------------------------------\
|                     XyagFatalAppError                       |
\------------------------------------------------------------*/
void XyagFatalAppError( message )
char *message;
{
//XtasDestroyAllFig() ;
XyagFirePasqua();
XyagErrorMessage( XyagMainWindow, message );
siglongjmp(XyagMyEnv, 1);
}

/*------------------------------------------------------------\
|                      XyagGetStdoutMess                      |
\------------------------------------------------------------*/
void XyagGetStdoutMess()
{
static char buffer[4096];
static int  i   = 0;

fflush( stderr );
i = read(xya_pfd[0],buffer,4096) ;
buffer[i] = '\0' ;

if(strlen(buffer) == 0)
  return ;

if(i == -1) 
strcpy(buffer,"Fatal Error Occuring While Reading File") ;

 XyagFatalAppError(buffer) ;
}





/*------------------------------------------------------------\
|                           XyagSetBlock                      |
\------------------------------------------------------------*/
void XyagSetBlock( fd, on )
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

/*------------------------------------------------------------\
|                           XyagPasqua                        |
\------------------------------------------------------------*/
void XyagPasqua()
{
/*---------------------------------*/
/* Catch all stdout messages !!!   */
/*---------------------------------*/
 if(XyagCallbackFileQuitFonction != NULL) {
     XyagPasquaFunction ();
     return; 
 }
 XyagSetBlock( 2, False );
 XyagSetBlock( 1, False );
 XyagOldExitHandler = signal( SIGTERM, XyagGetStdoutMess );
}

void updatebacklist(cgvfig_list *cgvf)
{
  chain_list *cl=OPEN_STACK, *ch;
  if (OPEN_STACK==NULL) return;
  if (cl->DATA==cgvf) 
    {
      OPEN_STACK=OPEN_STACK->NEXT; cl->NEXT=NULL;      
      freechain(cl);
      XyagDelConnect();
      XyagDelSelect();
      XyagDelZoom();
      return;
    }
  while (cl->NEXT!=NULL && cl->NEXT->DATA!=cgvf) 
    {
      cl=cl->NEXT;
    }
  if (cl->NEXT!=NULL) 
    {
      void *sel, *con, *zoom;
      con=XyagHeadConnect;
      sel=XyagHeadSelect;      
      zoom=XyagHeadZoom;

      XyagHeadConnect=cgvf->data0;
      XyagHeadSelect=cgvf->data1;
      XyagHeadZoom=cgvf->data_ZOOM;

      XyagDelConnect();
      XyagDelSelect();
      XyagDelZoom();

      ch=cl->NEXT;cl->NEXT=ch->NEXT; ch->NEXT=NULL;
      freechain(ch);

      XyagHeadConnect=con;
      XyagHeadSelect=sel;
      XyagHeadZoom=zoom;      
    }
//  else printf("not found??!!\n");
}

