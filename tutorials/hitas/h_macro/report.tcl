#!/usr/bin/env avt_shell

#############################################################
# Timing Reporting                                          #
#############################################################

avt_config avtLibraryDirs ".:../adder:../cpu2901:../models"

set fig [ttv_LoadSpecifiedTimingFigure top]
#ttv_LoadCrosstalkFile $fig

set clist [ttv_GetPaths $fig * * uu 5 critic path max]
ttv_DisplayPathListDetail stdout $clist

