%module avt

%{
#include MUT_H
#include "avt_API.h"
char *avt_gettcldistpath();
%}

%include avt_tcl_types.i
%include avt_API.i
