/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   File.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushBG.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XYA_H
#include XMX_H
#include XTB_H
#include XMF_H
#include XMV_H
#include XMT_H

#include "XMF_file.h"

extern void updatebacklist(cgvfig_list *cgvf);

char            XyagFileFilter[10] = "*";
char            XyagFileExtention[10] = ".";

static char     XyagFileBuffer[128];
char            XyagDirectoryBuffer[512];

char           *XYAG_WORK_LIB = (char *) NULL;

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

void 
XyagSetFilename(Name)
    char           *Name;
{
    strcpy(XyagFileBuffer, Name);
}

char *
XyagGetFilename()
{
    return (XyagFileBuffer);
}

/*------------------------------------------------------------\
|                         XyagFileOpen                        |
\------------------------------------------------------------*/

int XyagFileOpenByType(char *Name, int type)
{
    int             Index;
    int bad;
    mbkSwitchContext(XYAG_ctx); // dans XYAGLE
    cnsSwitchContext(CNS_ctx); // dans XYAGLE

    XyagSetMouseCursor(XyagGraphicWindow, XYAG_WATCH_CURSOR);


    xyagSaveState();
//    XyagDelConnect();
//    XyagDelFigure();

    XyagRecomputeBound = XYAG_TRUE;

    // removing existing figure of that name in the list
//    removecgvfiles(Name);

    bad=XyagLoadFigureByType(Name, type, 1); // in quiet mode

    xyagRetreiveState();

    if (!bad) {
        XyagChangeTopLevelTitle(Name);
    }
    else {
        XyagChangeTopLevelTitle(NULL);
    }

    XyagDisplayMessage();

    XyagInitializeZoom();
    XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);

    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);

    XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);

    cnsSwitchContext(CNS_ctx); // hors XYAGLE
    mbkSwitchContext(XYAG_ctx); // hors XYAGLE
    return bad;
}

void 
XyagFileOpen(FileName)
    char           *FileName;
{
    int             Index;

    mbkSwitchContext(XYAG_ctx); // dans XYAGLE
    cnsSwitchContext(CNS_ctx); // dans XYAGLE


    XyagSetMouseCursor(XyagGraphicWindow, XYAG_WATCH_CURSOR);


    xyagSaveState();
//    XyagDelConnect();
//    XyagDelFigure();

    XyagRecomputeBound = XYAG_TRUE;

    if (XYAG_WORK_LIB == (char *) NULL) {
        XYAG_WORK_LIB = WORK_LIB;
    }

    for (Index = strlen(FileName); Index >= 0; Index--) {
        if (FileName[Index] == '/') break;
    }

    if (Index >= 0) {
        strcpy(XyagDirectoryBuffer, FileName);
        strcpy(XyagFileBuffer, FileName + Index + 1);
        XyagDirectoryBuffer[Index + 1] = '\0';
        WORK_LIB = XyagDirectoryBuffer;
    }
    else {
        strcpy(XyagFileBuffer, FileName);
        WORK_LIB = XYAG_WORK_LIB;
    }
/*
    Index = strlen(XyagFileBuffer) - strlen(XyagFileExtention);

    if (Index >= 0) {
        if (!strcmp(XyagFileBuffer + Index, XyagFileExtention)) {
            XyagFileBuffer[Index] = '\0';
        }
    }
*/
    removeallcgvfiles(updatebacklist);
    XyagFigureCgv=NULL;

    XyagLoadFigure(XyagFileBuffer);


    if (XyagFigure != (xyagfig_list *) NULL) {
      xyagRetreiveState();
      XyagChangeTopLevelTitle(XyagFileBuffer);
    }
    else {
        XyagChangeTopLevelTitle((char *) NULL);
    }

//    XyagDisplayMessage();

    XyagInitializeZoom();
    XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);

    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);

    XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);

    cnsSwitchContext(CNS_ctx); // hors XYAGLE
    mbkSwitchContext(XYAG_ctx); // hors XYAGLE
}
