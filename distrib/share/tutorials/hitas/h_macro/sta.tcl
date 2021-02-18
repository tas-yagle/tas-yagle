#!/usr/bin/env avt_shell

#############################################################
# Stability Analysis                                        #
#############################################################

# Filter unimportant warning
avt_config avtWarningFilter "STB-018"

avt_config avtLibraryDirs ".:../adder:../cpu2901:../models"

inf_SetFigureName top

create_clock -period 10000 -waveform {5000 0} ck

set_input_delay -min 2000 -clock ck -clock_fall [all_inputs]
set_input_delay -max 3000 -clock ck -clock_fall [all_inputs]

inf_Drive top.inf

set fig [ttv_LoadSpecifiedTimingFigure top]

set stbfig [stb $fig]

stb_DisplaySlackReport [fopen slack.rep w] $stbfig * * ?? 10  all 10000
