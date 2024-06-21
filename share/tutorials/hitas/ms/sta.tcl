#!/usr/bin/env avt_shell

set fig [ttv_LoadSpecifiedTimingFigure msdp2_y]

# Static Timing Analysis

inf_SetFigureName msdp2_y

create_clock -period 1000 -waveform {500 0} ck
set_input_delay -clock ck -clock_fall -min 200 di
set_input_delay -clock ck -clock_fall -max 300 di

set_output_delay -clock ck -clock_fall -min 200 t
set_output_delay -clock ck -clock_fall -max 400 t

set stbfig [stb $fig]

stb_DisplaySlackReport [fopen slack.rep w] $stbfig * * ?? 10  all 10000
