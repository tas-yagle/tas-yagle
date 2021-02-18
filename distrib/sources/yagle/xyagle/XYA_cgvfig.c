/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Cgv.c                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

#include MUT_H
#include SLB_H
#include BEH_H
#include BEF_H
#include BHL_H
#include BVL_H
#include MLO_H
#include LOG_H
#include CGV_H
#include XSB_H
#include XYA_H

#include "XYA_cgvfig.h"
#include "XYA_error.h"
#include "XME_beh.h"

cgvfig_list    *XyagFigureCgv = (cgvfig_list *) NULL;

/*------------------------------------------------------------\
|                       For parser exit handler               |
\------------------------------------------------------------*/

static sigjmp_buf  XyagJumpBuffer;
static void     (*OldExitHandler) () = NULL;

/*------------------------------------------------------------\
|                      XyagExitHandler                        |
\------------------------------------------------------------*/

void 
XyagExitHandler()
{
//    XyagFlushErrorMessage();
  signal(SIGTERM, OldExitHandler);
  siglongjmp(XyagJumpBuffer, 1);
}

/*------------------------------------------------------------\
|                      XyagActiveExitHandler                  |
\------------------------------------------------------------*/

void 
XyagActiveExitHandler()
{
    OldExitHandler = signal(SIGTERM, XyagExitHandler);
}

/*------------------------------------------------------------\
|                       XyagResetExitHandler                  |
\------------------------------------------------------------*/

void 
XyagResetExitHandler()
{
    signal(SIGTERM, OldExitHandler);
}

/*------------------------------------------------------------\
|                          Xyaggetcgvfig                      |
\------------------------------------------------------------*/

cgvfig_list *
Xyaggetcgvfig(char *Name, int Mode, char *filename)
{
    cgvfig_list    *FigureCgv;

#ifndef __ALL__WARNING__
	filename = NULL;
#endif
//    for (FigureCgv = HEAD_CGVFIG; FigureCgv; FigureCgv = rmvcgvfig(FigureCgv));

    XyagInitFileErr();
    XyagPasqua();   
    XyagActiveExitHandler();

    if (sigsetjmp(XyagJumpBuffer, 1) == 0) {
        XyagSetJmpEnv (&XyagJumpBuffer);
        FigureCgv = getcgvfig(Name, Mode, NULL);
//        buildcgvfig(FigureCgv);
        XyagGetWarningMess() ;
        XyagFirePasqua() ;
        XyagResetExitHandler();
        return (FigureCgv);
    }
    printf("error\n");
    XyagGetWarningMess();
    XyagFirePasqua();
    delcgvfig(Name);
    return NULL;
}

/*------------------------------------------------------------\
|                          Xyaggetbefig                       |
\------------------------------------------------------------*/

befig_list *
Xyaggetbefig(FileName)
    char           *FileName;
{
    befig_list     *FigureBeh;

    if (XyagEditBehBefig != NULL) {
        XyagEditBehBefig = beh_delbefig(XyagEditBehBefig, XyagEditBehBefig, 'Y');
        printf("remove BEH\n");
    }

    if (findbefig(FileName)==NULL) return NULL;

    XyagPasqua();
    XyagActiveExitHandler();

    if (sigsetjmp(XyagJumpBuffer, 1) == 0) {
        XyagSetJmpEnv (&XyagJumpBuffer);
        FigureBeh = loadbefig(NULL, FileName, BVL_TRACE | BVL_KEEPAUX);
        XyagFirePasqua() ;
        XyagResetExitHandler();
        return FigureBeh;
    }

    XyagFirePasqua();
    return NULL;
}
