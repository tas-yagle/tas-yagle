#!/usr/bin/env avt_shell
 
#############################################################
# Timing Reporting                                          #
#############################################################

set fig [ttv_LoadSpecifiedTimingFigure adder]
set clist [ttv_GetPaths $fig * * rr 5 critic path max]
ttv_DisplayPathListDetail stdout $clist
