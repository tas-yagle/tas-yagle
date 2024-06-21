/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Icon.c                          |
|                                                             |
| Authors :              Lester Anthony                       |
|                                                             |
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
# include XMX_H

# include "XMX_icon.h"
# include "Icon.xpm"

/*------------------------------------------------------------\
|                    XyagInitializeIcon                      |
\------------------------------------------------------------*/

void
XyagInitializeIcon()
{
    XyagSetColorIcon(XyagTopLevel, Icon_xpm);
}
