%module mbk_API

%{
#include <stdio.h>
#include MUT_H
#define AVTWIG_AVOID_CONFLICT
#include "mbk_API.h"
%}

%include ../tcl/avt_tcl_types.i
%include mbk_API.i
