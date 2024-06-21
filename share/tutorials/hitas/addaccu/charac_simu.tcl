#!/usr/bin/env avt_shell

set figname addaccu

# Simulation Config
avt_config avtSpiceString     "../../bin/Solaris/ngspice -b $"
avt_config SimToolModel hspice
avt_config SimTool ngspice
avt_config simTechnologyName ../techno/bsim4_dummy.hsp

# Simulation speed-up
avt_config simOutLoad dynamic
avt_config avtTechnologyName ../techno/bsim4_dummy.hsp

# Characterization conditions
inf_SetFigureName $figname
create_clock -period 3000 -waveform {0 1500} ck
inf_DefineSlopeRange default {100ps 200ps} custom
inf_DefineCapacitanceRange default {16fF 32fF} custom
#inf_DefineSlopeRange default {25ps 50ps 100ps 200ps 400ps} custom
#inf_DefineCapacitanceRange default {8fF 16fF 32fF 64fF} custom

# Load Timing Database
set fig [ttv_LoadSpecifiedTimingFigure $figname]

# Timing Abstraction
set abs [tmabs $fig NULL * * * -verbose -simulate * -enablecache]
lib_drivefile [list $abs] NULL addaccu_golden.lib max
