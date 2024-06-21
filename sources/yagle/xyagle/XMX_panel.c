/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   Panel.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                        Picault Stephane                     |
|                                                             |
\------------------------------------------------------------*/

# include <stdio.h>
# include <Xm/Xm.h>
# include <Xm/PushB.h>
# include <Xm/Form.h>
# include <Xm/Frame.h>
 
# include MUT_H
# include MLO_H
# include XSB_H
# include XTB_H
# include XMF_H
# include XME_H
# include XMV_H
# include XMT_H
# include XMS_H
# include XMH_H
# include XMX_H

# include "XMX_panel.h"

/*------------------------------------------------------------\
|                     XyagInitializePanel                     |
\------------------------------------------------------------*/

void XyagInitializePanel()

{
  XyagBuildPanel( XyagMainWindow, &XyagViewLayerPanel         );
  XyagBuildPanel( XyagMainWindow, &XyagEditIdentifyPanel      );
  XyagBuildPanel( XyagMainWindow, &XyagViewMessagePanel       );

  XyagBuildPanel( XyagMainWindow, &XyagEditBehPanel           );

  XyagBuildPanel( XyagMainWindow, &XyagSetupInformationsPanel );

  XyagBuildPresentPanel();
}
