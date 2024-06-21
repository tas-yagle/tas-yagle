#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Timing Analysis Parameters
avt_config avtLibraryDirs .:./include/
avt_config simToolModel hspice

runStatHiTas 50 -incremental -result slacks.ssta -storedir store

inf_SetFigureName addaccu

create_clock -name "ck" -period 3000 -waveform { 0 1500} "ck"

avt_LoadFile ./include/addaccu_schem.inc spice
set fig [hitas addaccu]

#############################################################
# Timing Database Analysis                                  #
#############################################################

# STA 
set stbfig [stb $fig]

ssta_SlackReport -senddata $stbfig simple
