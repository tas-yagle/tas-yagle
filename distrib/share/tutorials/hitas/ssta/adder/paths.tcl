#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Timing Analysis Parameters
avt_config avtLibraryDirs .:./include/
avt_config simToolModel hspice

runStatHiTas 50 -incremental -result paths.ssta -storedir store_paths

inf_SetFigureName addaccu

create_clock -name "ck" -period 3000 -waveform { 0 1500} "ck"

avt_LoadFile ./include/addaccu_schem.inc spice
set fig [hitas addaccu]

#############################################################
# Timing Database Analysis                                  #
#############################################################

set paths [concat [ttv_GetPaths $fig -access] [ttv_GetPaths $fig]]

ssta_PathReport -senddata $paths simple

