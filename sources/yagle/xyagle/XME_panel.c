/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Panel.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                        Picault Stephane                     |
|                                                             |
\------------------------------------------------------------*/

# include <stdio.h>
# include <string.h>
# include <Xm/Xm.h>
# include <Xm/FileSB.h>
# include <Xm/SelectioB.h>
# include <Xm/PushBG.h>
# include <Xm/Text.h>
# include <Xm/List.h>

# include MUT_H
# include MLO_H
# include CGV_H
# include XSB_H
# include XYA_H
# include XMX_H
# include XTB_H
# include XME_H

# include "XME_panel.h"
# include "XME_edit.h"
# include "XME_search.h"
# include "XME_select.h"

/*------------------------------------------------------------\
|                       Panel Identify                        |
\------------------------------------------------------------*/

   static char *XyagPanelIdentifyButtonName[] =

   {
     "Text",
     "Close"
   };

 XyagPanelButtonItem XyagEditIdentifyButton[] =

         {
           {
             &XyagPanelIdentifyButtonName[0],
             "Nothing", NULL, 0, 0,
             NULL,
             NULL,
              0, 0,
              8, 9,
             NULL,
             (XtPointer)NULL,
             (Widget)NULL
           }
           ,
           {
             &XyagPanelIdentifyButtonName[1],
             NULL, NULL, 0, 0,
             NULL,
             NULL,
             3, 9,
             2, 1,
             CallbackEditCloseIdentify,
             (XtPointer)NULL,
             (Widget)NULL
           }
           ,
           {
             0, NULL, NULL, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL
           }
		 };

   XyagPanelItem XyagEditIdentifyPanel =

         {
           "Gate Structure",
           1,
           0,
           XYAG_EDIT_IDENTIFY_X,
           XYAG_EDIT_IDENTIFY_Y,
           360,
           250,
           8,
           10,
           (Widget)NULL,
           (Widget)NULL,
           (Widget)NULL,
           (Widget)NULL,
           XyagEditIdentifyButton
         };

   int XyagEditIdentifyDefaultValues[ 5 ] =

         {
           XYAG_EDIT_IDENTIFY_X,
           XYAG_EDIT_IDENTIFY_Y,
           360, 250, 0
         };

/*------------------------------------------------------------\
|                        Behaviour Panel                      |
\------------------------------------------------------------*/

static char *XyagEditBehButtonName[] =
{
   "Text",
   "Close"
};

XyagPanelButtonItem XyagEditBehButton[] =
{
   {
      &XyagEditBehButtonName[0],
      "Nothing", NULL, 0, 0,
      NULL,
      NULL,
      0, 0,
      8, 9,
      NULL,
      (XtPointer)NULL,
      (Widget)NULL
   }
   ,
   {
      &XyagEditBehButtonName[1],
      NULL, NULL, 0, 0,
      NULL,
      NULL,
      3, 9,
      2, 1,
      CallbackEditCloseBeh,
      (XtPointer)NULL,
      (Widget)NULL
   }
   ,
   {
	 0, NULL, NULL, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL
   }
};

XyagPanelItem XyagEditBehPanel =
{
   "Gate Behaviour",
   1,
   0,
   XYAG_EDIT_BEH_X,
   XYAG_EDIT_BEH_Y,
   360,
   250,
   8,
   10,
   (Widget)NULL,
   (Widget)NULL,
   (Widget)NULL,
   (Widget)NULL,
   XyagEditBehButton
};

int XyagEditBehDefaultValues[ 5 ] =
{
   XYAG_EDIT_BEH_X,
   XYAG_EDIT_BEH_Y,
   360, 250, 0
};

/*------------------------------------------------------------\
|                     Callback For Identify                   |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                     CallbackEditCloseIdentify               |
\------------------------------------------------------------*/

void CallbackEditCloseIdentify( MyWidget, ClientData, CallData )

     Widget  MyWidget;
     caddr_t ClientData;
     caddr_t CallData;
{
  XyagExitPanel( &XyagEditIdentifyPanel );
}

/*------------------------------------------------------------\
|                    XyagDisplayEditIdentify                  |
\------------------------------------------------------------*/

void XyagDisplayEditIdentify( Message )

  char *Message;
{
  XmTextSetString( XyagEditIdentifyButton[0].BUTTON, Message );
}

/*------------------------------------------------------------\
|                     Callback For Behaviour                  |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                     CallbackEditCloseBeh                    |
\------------------------------------------------------------*/

void CallbackEditCloseBeh( MyWidget, ClientData, CallData )

     Widget  MyWidget;
     caddr_t ClientData;
     caddr_t CallData;
{
  XyagExitPanel( &XyagEditBehPanel );
}

/*------------------------------------------------------------\
|                    XyagDisplayEditBeh                       |
\------------------------------------------------------------*/

void XyagDisplayEditBeh( Message )

  char *Message;
{
  XmTextSetString( XyagEditBehButton[0].BUTTON, Message );
}


