%module inf_API
%{
#include AVT_H
#include MUT_H
#include INF_H
#define ORIGINAL_TYPES
#include "inf_API.h"
#include "inf_sdc.h"
%}

%include ../tcl/avt_tcl_types.i
%include inf_API.i
