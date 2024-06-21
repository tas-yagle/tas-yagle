#!/usr/bin/env avt_shell

ttv_SetupReport ps

avt_config SimPowerSupply 1.65
avt_config SimTemperature 70
avt_config avtVddName vdd
avt_config avtVssName vss
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes

avt_LoadFile "multicycle.spi" spice

set figure multicycle

inf_SetFigureName $figure

set_input_transition 100 *

create_clock -period 500 -waveform { 0 250 } ck

set_input_delay -min 50 -clock ck -clock_fall [all_inputs]
set_input_delay -max 100 -clock ck -clock_fall [all_inputs]

set_multicycle_path -setup -from "ff1.sff_s" -to "ff2.sff_m" 2
#set_multicycle_path -hold -to "ff2.sff_m" 2

set tf [hitas $figure]

set sf [stb $tf]

set ofile [ fopen "slack.log" "w" ]
stb_DisplaySlackReport $ofile $sf ck ff2.sff_m ?? 0 "all" 100e-9
inf_Drive xtas.inf

