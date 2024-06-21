#!/usr/bin/env avt_shell

set figname addaccu

# Characterization conditions
inf_SetFigureName $figname
create_clock -period 3000 -waveform {0 1500} ck
inf_DefineSlopeRange default {25ps 50ps 100ps 200ps 400ps} custom
inf_DefineCapacitanceRange default {8fF 16fF 32fF 64fF} custom

# Load Timing Database
set fig [ttv_LoadSpecifiedTimingFigure $figname]

# Timing Abstraction
set abs [tmabs $fig NULL * * * -verbose -detailfile $figname.clog]
lib_drivefile [list $abs] NULL $figname.lib max
