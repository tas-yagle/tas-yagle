/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   Menu.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
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
#include XMT_H

#include "XMF_menu.h"
#include "XMT_menu.h"
#include "XMT_panel.h"
#include "XMF_file.h"

static int      XyagFirstFileOpen = 1;

static void     CallbackFileOpenOk();
static void     CallbackDisassembleOk();
void (* XyagCallbackFileQuitFonction)() = NULL ;
void (* XyagPasquaFunction)() = XyagPasqua ;
void (* XyagFirePasquaFunction)() = XyagFirePasqua ;
void (* XyagGetWarningFunction)() = XyagGetWarningMess ;

/*------------------------------------------------------------\
|                        FILE Menu                            |
\------------------------------------------------------------*/

XyagMenuItem    XyagFileMenu[] =
{
    {
        "Open...",
        'O',
        "Ctrl<Key>O",
        "Ctrl+O",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackFileOpen,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
#if 0
    {
        "Disassemble...",
        'D',
        "Ctrl<Key>D",
        "Ctrl+D",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackDisassembleOk,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
#endif
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Quit",
        'Q',
        "Ctrl<Key>Q",
        "Ctrl+Q",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackFileQuit,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL
    }
};

/*------------------------------------------------------------\
|                     CallbackFileOpen                        |
\------------------------------------------------------------*/

void
CallbackFileOpen(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XmString        Filter;

    if (XyagFirstFileOpen) {
        XyagFirstFileOpen = 0;
/*
        if (XyagFileFilter[2] == '\0') {
            strcat(XyagFileFilter, XYAGLE_IN_CGV);
            strcat(XyagFileExtention, XYAGLE_IN_CGV);
        }
*/
    }

    Filter = XmStringCreateSimple(XyagFileFilter);
    XyagFileSelect(Filter, "Select netlist to view:", CallbackFileOpenOk, NULL, NULL);
    XmStringFree(Filter);
}

/*------------------------------------------------------------\
|                     CallbackFileQuit                        |
\------------------------------------------------------------*/

void
CallbackFileQuit(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    if(XyagCallbackFileQuitFonction != NULL)
      XyagCallbackFileQuitFonction() ;
    else
      XyagConfirmQuit();
}

/*------------------------------------------------------------\
|                    CallbackFileOpenOk                       |
\------------------------------------------------------------*/

static void
CallbackFileOpenOk(MyWidget, ClientData, FileStruct )
    Widget                            MyWidget;
    caddr_t                           ClientData;
    XmFileSelectionBoxCallbackStruct *FileStruct;
{
  char *FileName;

  XtUnmanageChild(MyWidget);

  if ( FileStruct->value != NULL )
  {
    XmStringGetLtoR(FileStruct->value, XmSTRING_DEFAULT_CHARSET, &FileName);

    FileName = XyagPostTreatString( FileName ); 

    if (FileName != NULL)
    {
//      removecgvfiles(FileName, updatebacklist);
      XyagFileOpen(FileName);
      XyagChangeEditMode(XYAG_EDIT_SELECT);
    }
    else
    {
      XyagErrorMessage( XyagMainWindow, "Unable to load this file !" );
    }
  }
}

/*------------------------------------------------------------\
|                 CallbackDisassembleOk                       |
\------------------------------------------------------------*/
#if 0
static void
CallbackDisassembleOk(MyWidget, ClientData, FileStruct )
    Widget                            MyWidget;
    caddr_t                           ClientData;
    XmFileSelectionBoxCallbackStruct *FileStruct;
{
    XyagEnterPanel(&XyagToolsDisaPanel);
}
#endif
