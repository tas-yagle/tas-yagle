#!/usr/bin/env avt_shell

#############################################################
# Xtalk Analysis                                            #
#############################################################

avt_config stbDetailedAnalysis yes
avt_config stbCrosstalkMode yes

inf_SetFigureName cpu2901

create_clock -period 10000 -waveform {5000 0} ck

set_input_delay -min 2000 -clock ck -clock_fall [all_inputs]
set_input_delay -max 3000 -clock ck -clock_fall [all_inputs]

inf_Drive cpu2901.inf


set fig [ttv_LoadSpecifiedTimingFigure cpu2901]

set stbfig [stb $fig]

stb_DisplaySlackReport [fopen slack_xtalk.rep w] $stbfig * * ?? 10  all 10000
