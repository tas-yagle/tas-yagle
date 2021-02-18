#!/usr/bin/env avt_shell

set figname addaccu

# Simulation Config
avt_config avtSpiceString     "../../bin/Solaris/ngspice -b $"
avt_config SimToolModel hspice
avt_config SimTool ngspice
avt_config simTechnologyName ../techno/bsim4_dummy.hsp

ttv_DisplayActivateSimulation yes
# Set clocks
inf_SetFigureName $figname
create_clock -period 3000 -waveform {0 1500} ck
set_load 0.01 s\[0\]

# Load Timing Database
set fig [ttv_LoadSpecifiedTimingFigure $figname]

# Setup / Hold paths
#set file [fopen $figname.setuphold w]
#ttv_DisplayConnectorToLatchMargin $file $fig a\[0\] "split all"
#fclose $file

# Max access paths
set file [fopen $figname.accessmax w]
set pathlist [ttv_GetPaths $fig * s\[0\] ?f 1 critic access max]
ttv_DisplayPathListDetail $file $pathlist
fclose $file

# Min access paths
set file [fopen $figname.accessmin w]
set pathlist [ttv_GetPaths $fig * s\[0\] ?f 1 critic access min]
ttv_DisplayPathListDetail $file $pathlist
fclose $file

# Combinational paths
set file [fopen $figname.comb w]
set pathlist [ttv_GetPaths $fig a\[0\] s\[0\] ?? 1 critic path max]
ttv_DisplayPathListDetail $file $pathlist
fclose $file
