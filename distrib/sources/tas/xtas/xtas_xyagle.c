#include "xtas.h"
char XyagleRunOnce = 'N'  ;


void XtasXyagleCallback (widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
 XtManageChild (XyagTopLevel) ;
 XyagleRunOnce = 'Y'  ;
}

void XtasXyagleQuitFOnction()
{
 XtUnmanageChild (XyagTopLevel) ;
// XtasInitFileErr();
// XtasClearAllStream();
}

void XtasSetXyagleQuitFOnction()
{

 XyagCallbackFileQuitFonction = XtasXyagleQuitFOnction ;
 XyagPasquaFunction = XtasPasqua;
 XyagFirePasquaFunction = XtasFirePasqua;
 XyagGetWarningFunction = XtasGetWarningMess;
}
