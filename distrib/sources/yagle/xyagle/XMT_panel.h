/*------------------------------------------------------------\
|                                                             |
| Tool    :                  XYAG XMT                         |
|                                                             |
| File    :                 Yag Panel.h                       |
|                                                             |
| Author  :              Picault Stephane            AVERTEC  |
|                                                             |
| Date    :                  04.11.99                         |
|                                                             |
\------------------------------------------------------------*/

#ifndef XYAG_TOOLS_YAGLE
#define XYAG_TOOLS_YAGLE

/*------------------------------------------------------------\
|                         Default Panel Locations             |
\------------------------------------------------------------*/

#define XYAG_TOOLS_RUNYAGLE_X        80
#define XYAG_TOOLS_RUNYAGLE_Y        80

#define XYAG_RUN_YAGLE_MESSAGE_X    230
#define XYAG_RUN_YAGLE_MESSAGE_Y    380

#define XYAG_RUNYAGLE_MESSAGE_SIZE  64000
#define XYAG_RUNYAGLE_BUFFER_SIZE   64000


/*------------------------------------------------------------\
|                      Variables                              |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                 Tools Panels Callbacks                      |
\------------------------------------------------------------*/

void            CallbackBackwardExtract();
void            CallbackForwardExtract();
void            CallbackBothExtract();
void            CallbackFullExtract();
void            CallbackPreviousExtract();

void            CallbackBackwardHilite();
void            CallbackForwardHilite();
void            CallbackBothHilite();

void            CallbackSetDepth();
void XyagSetBlock( int fd, int on );
#endif
