%module beg_API
%{
#include AVT_H
#include MUT_H
#define ORIGINAL_TYPES
#include "beg_API.h"
%}

%include ../tcl/avt_tcl_types.i
%include beg_API.i
