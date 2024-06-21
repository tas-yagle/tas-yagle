/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_newdisp_util.c                                         */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Marc KUOCH                                                */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
int         XTAS_DISPLAY_NEW = 1 ;
char        XTAS_DISABLE_INFO = 'N' ;

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPlaceNbString                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char *XtasPlaceNbString(long num)
{
    char number[16] ;
    char buf[16] ;

    sprintf(number, "%ld",num) ;
    sprintf(buf, "%8s",number) ;

    return mbkstrdup(buf) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPlaceNbStringTTVUnit                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char *XtasPlaceNbStringTTVUnit(long num,float unit)
{
    char number[16] ;
    char buf[16] ;

    sprintf(number, "%.1f",num/unit) ;
    sprintf(buf, "%8s",number) ;

    return mbkstrdup(buf) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*      XtasPlaceFloatString                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char *XtasPlaceFloatString(float num)
{
    char   number[16] ;
    char   buf[16] ;

    sprintf(number, "%g",num) ; 
    
    /* colonnes de temps fixee a 8 caracteres et alignement a droite */
    sprintf(buf, "%8s", number) ;

    return(mbkstrdup(buf)) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPlaceSring                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char *XtasPlaceString(char *name, int maxlength)
{
    char  buffer[2048] ;
    int   i, nbspace ;
    char *space ;

    nbspace = maxlength - strlen(name) ;
    space   = buffer ;

    sprintf(space, "%s",name) ;
    space += strlen(name) ;

    for(i = 0 ; i < nbspace ; i++) {
        *space = ' ' ;
        space++ ;
    }
    *space = '\0' ;

    return mbkstrdup(buffer) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFillInfosForm                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillInfosForm( mainform, item )
Widget   mainform;
char    *item;
{
int             n ;
Arg             args[20];
XmString        text;
Widget          label_widget ;
Widget          button_widget ;
ttvsig_list    *ptsig ;
chain_list     *chain ;
chain_list     *chainx ;
char            buf[2048] ;
char            buf2[2048] ;
char            text_line[2048] ;
char           *direction = NULL ;

if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
   ptsig = ttv_getsig(XtasMainParam->ttvfig,item) ;
   if(!ptsig) {
       for (chainx=XtasMainParam->ttvfig->INS ; !ptsig && chainx ; chainx=chainx->NEXT)
           ptsig = ttv_getsig((ttvfig_list*)chainx->DATA, item);
   }
       
  }
 else
  return ;

if(ptsig != NULL)
 {
  chain = ttv_getlrcmd(XtasMainParam->ttvfig,ptsig) ;
  if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
    direction = XtasSignalDirection(ptsig) ;
 }

sprintf(text_line,"%s%s","Signal: ",item) ;
text = XmStringCreateSimple( text_line );
n = 0;
XtSetArg( args[n], XmNlabelString,      text              ); n++;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
XmStringFree( text );

text = XmStringCreateSimple("Keep this information") ;
button_widget = XtVaCreateManagedWidget(       "XtasButton",
                                         xmPushButtonGadgetClass,
                                         mainform,
                                         XmNlabelString,      text,
                                         XmNalignment,        XmALIGNMENT_BEGINNING,
                                         XmNtopAttachment, XmATTACH_FORM,
                                         XmNrightAttachment, XmATTACH_FORM,
                                         NULL) ;
XmStringFree(text) ;

/* Call back to keep information */ 
XtAddCallback(button_widget,
               XmNactivateCallback, 
               XtasSignalFocusCallback,
              (XtPointer)item); 

XtAddCallback (button_widget,
               XmNactivateCallback,
               XtasSignalInfoCallback,
               (XtPointer)mainform );

if(ptsig != NULL)
 {
  sprintf(text_line,"%s%s","Net: ", ptsig->NETNAME) ;
  text = XmStringCreateSimple( text_line );
  n = 0;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
  XmStringFree( text );

  sprintf(buf,"%g %s",ptsig->CAPA/1000.0,"pF") ;
  sprintf(text_line,"%s%s","Capa: ", buf) ;
  text = XmStringCreateSimple( text_line );
  n = 0;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
  XmStringFree( text );

  sprintf(text_line,"%s%s","Type: ", XtasSignalsType(ptsig)) ;
  text = XmStringCreateSimple( text_line );

  n = 0;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
  XmStringFree( text );

  if(direction != NULL)
  {
  sprintf(text_line,"%s%s","Direction: ", direction) ;
  text = XmStringCreateSimple( text_line );

  n = 0;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
  XmStringFree( text );
  }
  sprintf(text_line,"%s%s","Figure: ", ptsig->ROOT->INFO->FIGNAME) ;
  text = XmStringCreateSimple( text_line );

  n = 0;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
  XmStringFree( text );

  sprintf(text_line,"%s%s","Instance: ", ptsig->ROOT->INSNAME) ;
  text = XmStringCreateSimple( text_line );

  n = 0;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
  XmStringFree( text );
  
  if(chain != NULL)
    {
     for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
      {
	   ttv_getsigname(XtasMainParam->ttvfig, buf, ((ttvevent_list *)chainx->DATA)->ROOT);
	   
	   if ((((ttvevent_list *)chainx->DATA)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
       		sprintf (buf2, "%s (high opens)", buf) ;	
	   else if ((((ttvevent_list *)chainx->DATA)->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
       		sprintf (buf2, "%s (low opens)", buf) ;	

       sprintf(text_line,"%s%s","Command: ", 
               buf2) ;
       text = XmStringCreateSimple( text_line );

       n = 0;
       XtSetArg( args[n], XmNlabelString,      text              ); n++;
       XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
       XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
       XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
       XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
       label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, mainform, args, n );
       XmStringFree( text );
      }
     freechain(chain) ;
    }    
 }

}
