#!/usr/bin/env avt_shell

# Disassembly Configuration
avt_config yagDetectClockGating yes

# Drive partitionning report (facultative)
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes

# Timing Configuration
inf_SetFigureName "circuit"
create_clock -period 2800 -waveform { 0 1400 } CK

set_input_delay -min 0 -clock CK {EN1 D*}
set_input_delay -max 0 -clock CK {EN1 D*}
set_input_delay -min 0 -clock CK -clock_fall [all_inputs]
set_input_delay -max 0 -clock CK -clock_fall [all_inputs]

# Technology Parameters
avt_LoadFile "../techno/bsim4_dummy.hsp" spice

# Netlist loading
avt_LoadFile "circuit.spi" spice

# Timing Database Generation
set fig [hitas "circuit"]

# Driving clock gating directive (facultative)
inf_ExportSections directives.inf directives

inf_Drive circuit.inf

# Running stability analysis
set sb [stb $fig]

# Report configuration
ttv_SetupReport ps

# Driving slack report
set ff [fopen "slack.log" "w"]
stb_DisplaySlackReport $ff $sb * * ?? 0 "all" 10000e-12
fclose $ff
