#!/usr/bin/env avt_shell

#############################################################
# Timing Reporting                                          #
#############################################################

inf_SetFigureName circuit
create_clock -period 1000 -waveform {0 500} ck
set fig [ttv_LoadSpecifiedTimingFigure circuit]

set log [fopen "path.log" "wt"]
set clist [ttv_GetPaths $fig * * ?? 0 critic path max]
ttv_DisplayPathListDetail $log $clist

set log [fopen "access.log" "wt"]
set clist [ttv_GetPaths $fig * * ?? 0 critic access max]
ttv_DisplayPathListDetail $log $clist
