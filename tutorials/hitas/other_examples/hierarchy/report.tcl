#!/usr/bin/env avt_shell

#############################################################
# Timing Reporting                                          #
#############################################################

set fig [ttv_LoadSpecifiedTimingFigure m31]

set clist [ttv_GetPaths $fig * * uu 5 critic path max]
ttv_DisplayPathListDetail stdout $clist

