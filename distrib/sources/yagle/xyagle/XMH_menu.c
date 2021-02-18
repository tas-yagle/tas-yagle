/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                   Menu.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

# include <stdio.h>
# include <Xm/Xm.h>
# include <Xm/PushBG.h>

# include MUT_H
# include MLO_H
# include XTB_H
# include XSB_H
# include XMH_H
# include XMX_H

# include "XMH_menu.h"
# include "XMH_panel.h"

/*------------------------------------------------------------\
|                          Help Menu                          |
\------------------------------------------------------------*/

   XyagMenuItem XyagHelpMenu[]=

         {
           {
#ifdef xcones
             "About XCones",
#else             
             "About Xyagle",
#endif
             'G',
             NULL,
             NULL,
             &xmPushButtonGadgetClass,
             False,
             False,
             False,
             False,
             CallbackHelpAbout,
             (XtPointer)NULL,
             (Widget)NULL,
             (Widget)NULL,
             (XyagMenuItem *)NULL
           }
           ,
		   {
			 NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL
		   }
         };

/*------------------------------------------------------------\
|                      CallbackHelpAbout                      |
\------------------------------------------------------------*/

void CallbackHelpAbout( MyWidget, ClientData, CallData )

     Widget  MyWidget;
     caddr_t ClientData;
     caddr_t CallData;
{
  XyagEnterPresentPanel();
}
