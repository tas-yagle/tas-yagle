#!/usr/bin/env avt_shell

# Timing Configuration
inf_SetFigureName "circuit"
source circuit.sdc

# Timing Database Generation
set fig [ttv_LoadSpecifiedTimingFigure "circuit"]

# Load the directives
avt_LoadFile directives.inf inf

# Running stability analysis
set sb [stb $fig]

# Report configuration
ttv_SetupReport ps

# Driving slack report
set ff [fopen "slack.log" "w"]
stb_DisplaySlackReport $ff $sb * * ?? 0 "all" 10000e-12
fclose $ff
