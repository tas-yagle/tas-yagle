/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal   Version 1.01                                          */
/*    File :      BuildMenus.c                                              */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This file describes a very useful function of the libXal   library. It   */
/* provides the possibility  to  create your menu bar with all their        */
/* functions and their submenus functions.                                  */
/*                                                                          */
/* It's a recursive routine that works on a XalMenuItem structure.          */
/*                                                                          */
/* Concerning the help button, we concider the label "Help" or "Aide" as    */
/* the Motif default help push button. It had to appear once and in the     */
/* main menu items.                                                         */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
/*----------------*/
/* Motif includes */
/*----------------*/
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalBuildMenus                                                  */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget. MUST BE the result of an              */
/*                      XmCreateMenuBar motif call.                          */
/*            .menu : Pointer to the main menu structure.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creates all menus and submenus of a menu bar widget.           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalBuildMenus( parent, menu )
Widget parent;
struct XalMenuItem *menu;
{
char     new_name[80];
Widget   button, submenu;
XmString motif_text;
Arg      args[10];
int      i, n;

for (i=0; menu[i].label != NULL; i++)
    {
    /*-------------------------------------------------*/
    /* If needs Separator before the menu, then add it */
    /*-------------------------------------------------*/
    if (menu[i].separator == True)
	XtCreateManagedWidget( "sep", xmSeparatorWidgetClass, parent, NULL, 0 );

    /*----------------------*/
    /* Create the menu Item */
    /*----------------------*/
    button = XtVaCreateManagedWidget( menu[i].label, *menu[i].wid_class, parent, 
                                      XmNmnemonic, menu[i].mnemonic,
				      XmNshadowThickness, 1,
                                      NULL );

    /*---------------------*/
    /* Add the accelerator */
    /*---------------------*/
    if (menu[i].accelerator != NULL)
        XtVaSetValues( button, XmNaccelerator, menu[i].accelerator, NULL);

    if (menu[i].accel_text != NULL)
        {
        motif_text = XmStringCreateSimple( menu[i].accel_text );
        XtVaSetValues( button, XmNacceleratorText, motif_text, NULL);
        XmStringFree( motif_text );
        }

    /*---------------------------*/
    /* Add the callback function */
    /*---------------------------*/
    if (menu[i].callback != NULL)
        XtAddCallback( button, XmNactivateCallback, menu[i].callback, menu[i].callback_data );

    /*---------------------------------------------------------*/
    /* If HELP then set up this widget for the menu bar's help */
    /*---------------------------------------------------------*/
    if (( strcmp( menu[i].label, "Help" ) == 0 ) ||
	( strcmp( menu[i].label, "Aide" ) == 0 ))
	{
	n = 0;
	XtSetArg( args[n], XmNmenuHelpWidget, button ); n++;
	XtSetValues( parent, args, n );
	}

    /*----------------------------------------------------------------*/
    /* If a subitems structure is provided, then treat it recursively */
    /*----------------------------------------------------------------*/
    if (menu[i].subitems != NULL)
        {
	/*---------------------------------------*/
	/* Create a new name for the actual menu */
	/*---------------------------------------*/
	strcpy( new_name, menu[i].label );
	strcat( new_name, " Menu" );

        n = 0;
	XtSetArg( args[n], XmNshadowThickness, 1 ); n++;
        submenu = XmCreatePulldownMenu( parent, new_name, args, n );

        if (menu[i].sub_title == True)
            {
	    /*-------------------------------*/
	    /* Fill in the title in the menu */
	    /*-------------------------------*/
	    motif_text = XmStringCreateLtoR( new_name, XmSTRING_DEFAULT_CHARSET );
	    XtVaCreateManagedWidget( "MenuTitle", xmLabelWidgetClass, submenu,
				     XmNlabelString, motif_text,
				     NULL );
	    XmStringFree( motif_text );

	    /*--------------------*/
	    /* Add in a separator */
	    /*--------------------*/
	    XtCreateManagedWidget( "sep", xmSeparatorWidgetClass, submenu, NULL, 0 );
	    }

	/*-------------------------------------------------*/
	/* Set up button on menu_bar to pull down our menu */
	/*-------------------------------------------------*/
	n = 0;
	XtSetArg( args[n], XmNsubMenuId, submenu ); n++;
	XtSetValues( button, args, n );

	/*--------------------*/
	/* The Recursive call */
	/*--------------------*/
	XalBuildMenus( submenu, menu[i].subitems );
        }
    }
}

