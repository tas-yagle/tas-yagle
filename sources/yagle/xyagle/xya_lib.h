/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                  xyag.h                          |
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

# ifndef XYAG_XYAG
# define XYAG_XYAG

/*------------------------------------------------------------\
|                                                             |
|                           Constants                         |
|                                                             |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                                                             |
|                            Types                            |
|                                                             |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                                                             |
|                          Variables                          |
|                                                             |
\------------------------------------------------------------*/
extern XtAppContext XyagleApplication ;
extern Widget XyagMainWindow ;
extern Widget XyagTopLevel ;
extern void (* XyagCallbackFileQuitFonction)() ;
extern void (* XyagPasquaFunction)() ;
extern void (* XyagFirePasquaFunction)() ;
extern void (* XyagGetWarningFunction)() ;

/*------------------------------------------------------------\
|                                                             |
|                          Functions                          |
|                                                             |
\------------------------------------------------------------*/
extern Widget XyagCreateMainWindow (Widget,XtAppContext,Colormap *) ;
extern int XyagFileOpenByType(char *Name, int type);
extern void XyagExtract(cgv_interaction *itr, int complete);
extern void XyagHiLight(cgv_interaction *itr, int complete);
extern void Xyaggetenv();
extern sigjmp_buf   *XyagJmpEnv;
/*------------------------------------------------------------\
|                                                             |
|                   XyagInitializeRessources                 |
|                                                             |
\------------------------------------------------------------*/

# endif
