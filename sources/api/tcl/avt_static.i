%module avt

%{
#define AVTWIG_AVOID_CONFLICT
#include AVT_H
#include MUT_H
#include "avt_API.h"
#include "../ams/ams_API.h"
#include "../cns/cns_API.h"
#include "../ctk/ctk_API.h"
#include "../hierPathExtract/hpe_API.h"
#include "../inf/inf_API.h"
#include "../inf/inf_sdc.h"
#include "../lbt/lbt_API.h"
#include "../mbk/mbk_API.h"
#include "../mbkspice/spi_API.h"
#include "../sim/sim_API.h"
#include "../stb/stb_API.h"
#include "../tas/tas_API.h"
#include "../tcl/avt_API.h"
#include "../tma/tma_API.h"
#include "../ttv/ttv_API.h"
#include "../power/power_API.h"
#include "../trc/trc_API.h"
#include "../yagle/yagle_API.h"
#include "../genius/gen_API.h"
#include "../stm/stm_API.h"
#include "../fcl/fcl_API.h"
#include "../beg/beg_API.h"
char *avt_gettcldistpath ();
%}

%include ../tcl/avt_tcl_types.i
%include ../tma/tma_API.i
%include ../tas/tas_API.i
%include ../inf/inf_API.i
%include ../lbt/lbt_API.i
%include ../ttv/ttv_API.i
%include ../power/power_API.i
%include ../stb/stb_API.i
%include ../yagle/yag_API.i
%include ../ams/ams_API.i
%include ../cns/cns_API.i
%include ../ctk/ctk_API.i
%include ../hierPathExtract/hpe_API.i
%include ../mbk/mbk_API.i
%include ../trc/trc_API.i
%include ./avt_API.i
%include ../sim/sim_API.i
%include ../genius/gen_API.i
%include ../stm/stm_API.i
%include ../fcl/fcl_API.i
%include ../beg/beg_API.i
