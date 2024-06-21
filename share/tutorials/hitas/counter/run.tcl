#!/usr/bin/env avt_shell

set figure counter

avt_config SimPowerSupply 0.9
avt_config SimTemperature 60 
avt_config avtVddName vdd
avt_config avtVssName vss
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes
avt_config tasTreatPrecharge yes
avt_config yagDetectPrecharge yes
avt_config avtlogfile $figure.log
avt_config avtSpiKeepCards all
ttv_SetupReport ps

inf_SetFigureName $figure

set_case_analysis 0 reset

create_clock -period 200 -waveform { 0 100 } ck
create_generated_clock -source ck -edges {2 4 6} xo1_fflop.xs.li
create_generated_clock -source xo1_fflop.xs.li -edges {2 4 6} xo2_fflop.xs.li
create_generated_clock -source xo2_fflop.xs.li -edges {2 4 6} xo3_fflop.xs.li
create_generated_clock -source xo3_fflop.xs.li -edges {2 4 6} xo4_fflop.xs.li
create_generated_clock -source xo4_fflop.xs.li -edges {2 4 6} xo5_fflop.xs.li
create_generated_clock -source xo5_fflop.xs.li -edges {2 4 6} xo6_fflop.xs.li

set_input_delay -min 0 -clock ck -clock_fall [all_inputs]
set_input_delay -max 0 -clock ck -clock_fall [all_inputs]

set_output_delay -clock ck -clock_fall 500 o*

inf_Drive counter.inf

avt_LoadFile ../techno/bsim4_dummy.hsp spice
avt_LoadFile "counter.spi" spice

set fig [hitas $figure]

set sf [stb $fig]

set ofile [ fopen "slack.log" "w" ]
stb_DisplaySlackReport $ofile $sf * * ?? 0 "all thru margins" 1
fclose $ofile

