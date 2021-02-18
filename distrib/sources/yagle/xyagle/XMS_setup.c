/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Setup.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                        Picault Stephane                     |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>

#include MUT_H
#include MLO_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XMF_H
#include XME_H
#include XMV_H
#include XMT_H
#include XMS_H
#include XMH_H

#include "XMS_setup.h"

static FILE    *FileConfig;

/*------------------------------------------------------------\
|                      Set Panel New Values                   |
\------------------------------------------------------------*/

void 
XyagSetPanelValues(Panel, Values)
    XyagPanelItem  *Panel;
    int            *Values;
{
    Panel->COMPUTE = 1;
    Panel->X = Values[0];
    Panel->Y = Values[1];
    Panel->WIDTH = Values[2];
    Panel->HEIGHT = Values[3];
    Panel->MANAGED = Values[4];

    XtVaSetValues(Panel->PANEL,
                  XmNx, Values[0],
                  XmNy, Values[1],
                  XmNwidth, Values[2],
                  XmNheight, Values[3],
                  NULL);

    if (Panel->MANAGED) {
        XyagEnterPanel(Panel);
    }
    else {
        XtUnmanageChild(Panel->PANEL);
    }
}

/*------------------------------------------------------------\
|                      Write TopLevel Values                  |
\------------------------------------------------------------*/

void 
XyagWriteTopLevelValues()
{
    Dimension       Values[5];

    XtVaGetValues(XyagTopLevel,
                  XmNx, &Values[0],
                  XmNy, &Values[1],
                  XmNwidth, &Values[2],
                  XmNheight, &Values[3],
                  NULL);

    Values[0] = Values[0] - XYAG_TOPLEVEL_TRANSLATE_X;
    Values[1] = Values[1] - XYAG_TOPLEVEL_TRANSLATE_Y;
    Values[4] = 1;

    fprintf(FileConfig, "X: %d, Y: %d, WIDTH: %d, HEIGHT: %d, MANAGED: %d\n",
            Values[0], Values[1], Values[2], Values[3], Values[4]);
}

/*------------------------------------------------------------\
|                       Write Panel Values                    |
\------------------------------------------------------------*/

void 
XyagWritePanelValues(Panel)
    XyagPanelItem  *Panel;
{
    Dimension       Values[5];

    if (Panel->COMPUTE == 0) {
        XtVaGetValues(Panel->PANEL,
                      XmNx, &Values[0],
                      XmNy, &Values[1],
                      XmNwidth, &Values[2],
                      XmNheight, &Values[3],
                      NULL);

        Values[0] = Values[0] - XYAG_PANEL_TRANSLATE_X;
        Values[1] = Values[1] - XYAG_PANEL_TRANSLATE_Y;
    }
    else {
        Values[0] = Panel->X - XYAG_PANEL_TRANSLATE_X;
        Values[1] = Panel->Y - XYAG_PANEL_TRANSLATE_Y;
        Values[2] = Panel->WIDTH;
        Values[3] = Panel->HEIGHT;
    }

    Values[4] = Panel->MANAGED;

    fprintf(FileConfig, "X: %d, Y: %d, WIDTH: %d, HEIGHT: %d, MANAGED: %d\n",
            Values[0], Values[1], Values[2], Values[3], Values[4]);
}

/*------------------------------------------------------------\
|                        Read Panel Values                    |
\------------------------------------------------------------*/

void 
XyagReadPanelValues(Panel)
    XyagPanelItem  *Panel;
{
    int             Values[5];

    fscanf(FileConfig, "X: %d, Y: %d, WIDTH: %d, HEIGHT: %d, MANAGED: %d\n",
           &Values[0], &Values[1], &Values[2], &Values[3], &Values[4]);

    XyagSetPanelValues(Panel, Values);
}

/*------------------------------------------------------------\
|                       Read  Panel Values                    |
\------------------------------------------------------------*/

char 
XyagReadTopLevelValues()
{
    int             Values[5];
    char            Version[64];

    fscanf(FileConfig, "X: %d, Y: %d, WIDTH: %d, HEIGHT: %d, MANAGED: %d\n",
           &Values[0], &Values[1], &Values[2], &Values[3], &Values[4]);

    XtVaSetValues(XyagTopLevel,
                  XmNx, Values[0],
                  XmNy, Values[1],
                  XmNwidth, Values[2],
                  XmNheight, Values[3],
                  NULL);
    return (1);
}

/*------------------------------------------------------------\
|                         ReadActiveLayers                    |
\------------------------------------------------------------*/

void 
XyagReadActiveLayers()
{
    short           Layer;
    int             Value;

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        fscanf(FileConfig, "ACTIVE: %d\n", &Value);

        XYAG_ACTIVE_LAYER_TABLE[Layer] = Value;
    }

    for (Layer = 0; Layer < XYAG_MAX_ACTIVE_NAME; Layer++) {
        fscanf(FileConfig, "ACTIVE: %d\n", &Value);

        XYAG_ACTIVE_NAME_TABLE[Layer] = Value;
    }
}

/*------------------------------------------------------------\
|                        WriteActiveLayers                    |
\------------------------------------------------------------*/

void 
XyagWriteActiveLayers()
{
    int             Layer;

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        fprintf(FileConfig, "ACTIVE: %d\n", XYAG_ACTIVE_LAYER_TABLE[Layer]);
    }

    for (Layer = 0; Layer < XYAG_MAX_ACTIVE_NAME; Layer++) {
        fprintf(FileConfig, "ACTIVE: %d\n", XYAG_ACTIVE_NAME_TABLE[Layer]);
    }
}

/*------------------------------------------------------------\
|                    XyagDefaultTopLevelValues                |
\------------------------------------------------------------*/

void 
XyagDefaultTopLevelValues()
{
    XtVaSetValues(XyagTopLevel,
                  XmNheight, XYAG_TOPLEVEL_HEIGHT,
                  XmNwidth, XYAG_TOPLEVEL_WIDTH,
                  XmNx, XYAG_TOPLEVEL_X,
                  XmNy, XYAG_TOPLEVEL_Y,
                  NULL
        );
}

/*------------------------------------------------------------\
|                         XyagDefaultConfig                   |
\------------------------------------------------------------*/

void 
XyagDefaultConfig()
{
    short           Layer;

    XyagDefaultTopLevelValues();

    XyagSetPanelValues(&XyagViewLayerPanel, XyagViewLayerDefaultValues);
    XyagSetPanelValues(&XyagEditIdentifyPanel, XyagEditIdentifyDefaultValues);
    XyagSetPanelValues(&XyagViewMessagePanel, XyagViewMessageDefaultValues);
    XyagSetPanelValues(&XyagSetupInformationsPanel, XyagSetupInformationsDefaultValues);
    XyagSetPanelValues(&XyagHelpPresentPanel, XyagHelpPresentDefaultValues);

    XyagSetPanelValues(&XyagEditBehPanel, XyagEditBehDefaultValues);

    XYAG_ACTIVE_LAYER_TABLE[XYAG_CGVBOX_LAYER] = 1;
    XYAG_ACTIVE_LAYER_TABLE[XYAG_CGVCONIN_LAYER] = 1;
    XYAG_ACTIVE_LAYER_TABLE[XYAG_CGVCONOUT_LAYER] = 1;
    XYAG_ACTIVE_LAYER_TABLE[XYAG_CGVNET_LAYER] = 1;
    XYAG_ACTIVE_LAYER_TABLE[XYAG_CONSTRUCTION_LAYER] = 0;

    for (Layer = 0; Layer < XYAG_MAX_ACTIVE_NAME; Layer++) {
        XYAG_ACTIVE_NAME_TABLE[Layer] = 1;
    }

    XyagInitializeLayer();
}

/*------------------------------------------------------------\
|                    XyagLoadTopLevelConfig                   |
\------------------------------------------------------------*/

void 
XyagLoadTopLevelConfig()
{
    FileConfig = fopen(XYAG_XMS_FILE_NAME, "r");

    if (FileConfig == (FILE *) NULL) {
        XyagDefaultTopLevelValues();
    }
    else {
        if (!XyagReadTopLevelValues()) {
            XyagDefaultTopLevelValues();
        }

        fclose(FileConfig);
    }
}

/*------------------------------------------------------------\
|                           XyagLoadConfig                    |
\------------------------------------------------------------*/

void 
XyagLoadConfig(Message)
    char            Message;
{
    FileConfig = fopen(XYAG_XMS_FILE_NAME, "r");

    if (FileConfig == (FILE *) NULL) {
        if (Message) {
            XyagErrorMessage(XyagMainWindow, "Unable to open config file !");
        }
        else {
            XyagDefaultConfig();
        }
    }
    else {
        if (XyagReadTopLevelValues()) {
            XyagReadPanelValues(&XyagViewLayerPanel);
            XyagReadPanelValues(&XyagEditIdentifyPanel);
            XyagReadPanelValues(&XyagViewMessagePanel);
            XyagReadPanelValues(&XyagSetupInformationsPanel);
            XyagReadPanelValues(&XyagHelpPresentPanel);
            XyagReadPanelValues(&XyagEditBehPanel);
            XyagReadActiveLayers();
            XyagInitializeLayer();
        }
        else if (Message) {
            XyagErrorMessage(XyagMainWindow, "Bad version, unable to open config file !");
        }

       fclose(FileConfig);
   }
}

/*------------------------------------------------------------\
|                           XyagSaveConfig                    |
\------------------------------------------------------------*/

void 
XyagSaveConfig()
{
    FileConfig = fopen(XYAG_XMS_FILE_NAME, "w");

    if (FileConfig == (FILE *) NULL) {
        XyagErrorMessage(XyagMainWindow, "Unable to open config file !");
    }
    else {
        XyagWriteTopLevelValues();

        XyagWritePanelValues(&XyagViewLayerPanel);
        XyagWritePanelValues(&XyagEditIdentifyPanel);
        XyagWritePanelValues(&XyagViewMessagePanel);
        XyagWritePanelValues(&XyagSetupInformationsPanel);
        XyagWritePanelValues(&XyagHelpPresentPanel);
        XyagWritePanelValues(&XyagEditBehPanel);

        XyagWriteActiveLayers();

        fclose(FileConfig);
    }
}




