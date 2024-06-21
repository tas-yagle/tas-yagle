#!/usr/bin/env avt_shell

#############################################################
# Timing Reporting                                          #
#############################################################

set log [fopen "path.log" "wt"]
set fig [ttv_LoadSpecifiedTimingFigure cpu2901]
set clist [ttv_GetPaths $fig * * rr 5 critic path max]
ttv_DisplayPathListDetail $log $clist
