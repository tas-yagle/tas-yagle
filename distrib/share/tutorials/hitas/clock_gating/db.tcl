#!/usr/bin/env avt_shell

# Disassembly Configuration
avt_config yagDetectClockGating yes

# Drive partitionning report (facultative)
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes

# Timing Configuration
inf_SetFigureName "circuit"
source circuit.sdc

# Technology Parameters
avt_LoadFile "../techno/bsim4_dummy.hsp" spice

# Netlist loading
avt_LoadFile "circuit.spi" spice

# Timing Database Generation
set fig [hitas "circuit"]

inf_ExportSections directives.inf directives
