#!/usr/bin/env avt_shell

ttv_SetupReport ps

avt_config SimPowerSupply 1.65
avt_config SimTemperature 70
avt_config avtVddName vdd
avt_config avtVssName vss
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes

ttv_SetupReport "ps OnlyNetName hiderc"
ttv_DisplayPathDetailHideColumn "dt.nodename"

avt_LoadFile "lag.spi" spice

set figure lag

inf_SetFigureName $figure

set_input_transition 100 *

create_clock -period 600 -waveform { 0 300 } ck

set_input_delay -min 50 -clock ck -clock_fall [all_inputs]
set_input_delay -max 100 -clock ck -clock_fall [all_inputs]


set tf [hitas $figure]

set sf [stb $tf]

set setup_slacks [stb_GetSlacks $sf -setuponly -margin 1]
set hold_slacks [stb_GetSlacks $sf -holdonly -margin 1]

set ofile [fopen "slack.log" w]
stb_DisplaySlackReport $ofile $sf -slacks $setup_slacks
stb_DisplaySlackReport $ofile $sf -slacks $hold_slacks
fclose $ofile

set setup_slack [stb_GetSlacks $sf -setuponly -from ck -to l2.li -nbslacks 1]

set ofile [fopen "lag_debug.log" w]
stb_FindLagPaths $ofile $setup_slack -closingpath
fclose $ofile
