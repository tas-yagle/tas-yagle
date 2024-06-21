/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   View.h                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

#ifndef XYAG_VIEW
#define XYAG_VIEW

typedef struct xyagview_list {
    struct xyagview_list *NEXT;
    xyagobj_list   *OBJECT;
} xyagview_list;

#endif
