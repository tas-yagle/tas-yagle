%module sim_API
%{
#include AVT_H
#include MUT_H
#define ORIGINAL_TYPES
#include "sim_API.h"
%}

%include ../tcl/avt_tcl_types.i
%include sim_API.i
