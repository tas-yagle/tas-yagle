/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Error.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#include MUT_H
#include SLB_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XYA_H

#include "XYA_error.h"

static FILE    *XyagStreamErr;
static FILE    *XyagStreamOut;
static FILE    *XyagStreamAll;

static char     XyagErrFileName[40];
static char     XyagOutFileName[40];
static char     XyagAllFileName[40];

static char     XyagNormalMode = 1;

static char    *XyagErrorBuffer = (char *) NULL;

static char     XyagInformationsBuffer[XYAG_INFORMATIONS_BUFFER_SIZE];
extern void (* XyagCallbackFileQuitFonction)();

/*------------------------------------------------------------\
|                    XyagExitErrorMessage                     |
\------------------------------------------------------------*/

int XYADEBUG=0;

void 
XyagExitErrorMessage(Error)
    int             Error;
{
//  return ;

    if (XyagStreamErr != (FILE *) 0) {
        fclose(XyagStreamErr);
        unlink(XyagErrFileName);
    }

    if (XyagStreamAll != (FILE *) 0) {
        fclose(XyagStreamAll);
        unlink(XyagAllFileName);
    }

    if ((XyagNormalMode) &&
        (XyagStreamOut != (FILE *) 0)) {
        fclose(XyagStreamOut);
        unlink(XyagOutFileName);
    }
}

/*------------------------------------------------------------\
|                    XyagInitializeErrorMessage               |
\------------------------------------------------------------*/

void 
XyagInitializeErrorMessage(Debug)
    char            Debug;
{
    XyagNormalMode = !Debug;

    if (XyagCallbackFileQuitFonction != NULL)
        XYADEBUG=0;
    else
        XYADEBUG=Debug;
        
/*
    if (!XYADEBUG)
      {
        XyagStreamOut = freopen("/dev/null", "w", stdout);
      }

    return;
*/
    if (XyagNormalMode) {
        sprintf(XyagOutFileName, "/tmp/%s_out_%ld", XYAG_TOOL_NAME, (long)getpid());
    }

    sprintf(XyagErrFileName, "/tmp/%s_err_%ld", XYAG_TOOL_NAME, (long)getpid());
    sprintf(XyagAllFileName, "/tmp/%s_all_%ld", XYAG_TOOL_NAME, (long)getpid());

//    XyagStreamErr = freopen(XyagErrFileName, "w+", stderr);
    XyagStreamAll = fopen(XyagAllFileName, "w+");
    XyagStreamErr=stderr;
    if (XyagNormalMode) {
        XyagStreamOut = freopen(XyagOutFileName, "w+", stdout);
    }
    else {
        XyagStreamOut = XyagStreamErr;
    }

    if ((XyagStreamAll == NULL) || (XyagStreamOut == NULL) || (XyagStreamErr == NULL)) {
        fprintf(stdout, "XYAG: Unable to open trace window !\n");
        XyagExit(1);
    }

    if (XyagNormalMode) {
        unlink(XyagOutFileName);
    }

    unlink(XyagErrFileName);

    signal(SIGINT, XyagExitErrorMessage);
}

/*------------------------------------------------------------\
|                      XyagFlushErrorMessage                 |
\------------------------------------------------------------*/

void 
XyagFlushErrorMessage()
{
    int             Data;

//    return ;

    fflush(XyagStreamErr);
    fseek(XyagStreamErr, 0L, 0);

    while ((Data = fgetc(XyagStreamErr)) != EOF) {
        fputc(Data, XyagStreamAll);
    }

//    fclose(XyagStreamErr);

//    XyagStreamErr = freopen(XyagErrFileName, "w+", stderr);

    if (XyagNormalMode) {
        fflush(XyagStreamOut);
        fseek(XyagStreamOut, 0L, 0);

        while ((Data = fgetc(XyagStreamOut)) != EOF) {
            fputc(Data, XyagStreamAll);
        }

        fclose(XyagStreamOut);

        XyagStreamOut = freopen(XyagOutFileName, "w+", stdout);
    }
}

/*------------------------------------------------------------\
|                      XyagGetErrorMessage                    |
\------------------------------------------------------------*/
void XyagCleanErrorMessage()
{
//  return ;
  XyagFlushErrorMessage();
  fflush(XyagStreamAll);
  rewind(XyagStreamAll);
}

char *
XyagGetErrorMessage()
{
    char           *Message;
    long            Length;
    long            Index;
    int             Data;

//    return NULL;

    XyagFlushErrorMessage();

    fflush(XyagStreamAll);

    Length = ftell(XyagStreamAll);

    if (XyagErrorBuffer != (char *) NULL) {
        mbkfree(XyagErrorBuffer);
    }

    XyagErrorBuffer = mbkalloc(Length + 1);
    Index = 0;

    while (((Data = fgetc(XyagStreamAll)) != EOF) &&
           (Index < Length)) {
        XyagErrorBuffer[Index++] = (char) Data;
    }

    rewind(XyagStreamAll);

    XyagErrorBuffer[Index] = '\0';

    if (Index == 0) {
        Message = (char *) NULL;
    }
    else {
        Message = XyagErrorBuffer;
    }
    return (Message);
}

/*------------------------------------------------------------\
|                       XyagGetInformations                   |
\------------------------------------------------------------*/

char           *
XyagGetInformations()
{
    char           *Scan;

    XyagComputeBound();

    Scan = XyagInformationsBuffer;

    if (XyagFigureCgv != (cgvfig_list *) NULL) {
        sprintf(Scan, "  FIGURE : %s\n\n",
                XyagFigureCgv->NAME);
    }
    else {
        sprintf(Scan, "  FIGURE : No current figure !\n\n");
    }

    Scan = Scan + strlen(Scan);

    sprintf(Scan,
            "  BOUNDING BOX : \n\n  XMIN : %ld\n  YMIN : %ld\n  XMAX : %ld\n  YMAX : %ld\n\n",
            XyagBoundXmin / XYAG_UNIT, XyagBoundYmin / XYAG_UNIT,
            XyagBoundXmax / XYAG_UNIT, XyagBoundYmax / XYAG_UNIT);

    Scan = Scan + strlen(Scan);
    return (XyagInformationsBuffer);
}
