/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   Panel.h                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                                                             |
|                         Include Files                       |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_EDIT_PANEL
# define XYAG_EDIT_PANEL

/*------------------------------------------------------------\
|                           Constants                         |
\------------------------------------------------------------*/

# define XYAG_EDIT_VIEW_SEARCH_X      790
# define XYAG_EDIT_VIEW_SEARCH_Y      450

# define XYAG_EDIT_IDENTIFY_X         330
# define XYAG_EDIT_IDENTIFY_Y         280

# define XYAG_EDIT_SELECT_X           330
# define XYAG_EDIT_SELECT_Y           280

# define XYAG_EDIT_BEH_X    230
# define XYAG_EDIT_BEH_Y    380

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

  extern void CallbackEditSearchViewContinue();
  extern void CallbackEditSearchViewAbort();

  extern void CallbackEditCloseIdentify();

  extern void CallbackEditSelectList();
  extern void CallbackEditSelectAccept();
  extern void CallbackEditSelectCancel();

  extern void XyagDisplaySelectList();

  extern void CallbackEditCloseBeh();

# endif 
