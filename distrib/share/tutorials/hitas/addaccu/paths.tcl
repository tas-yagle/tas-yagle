#!/usr/bin/env avt_shell

set figname addaccu

# Set clocks
inf_SetFigureName $figname
create_clock -period 3000 -waveform {0 1500} ck

# Load Timing Database
set fig [ttv_LoadSpecifiedTimingFigure $figname]

# Setup / Hold paths
set file [fopen $figname.setuphold w]
ttv_DisplayConnectorToLatchMargin $file $fig * "split all"
fclose $file

# Max access paths
set file [fopen $figname.accessmax w]
set pathlist [ttv_GetPaths $fig * s\[*\] ?? 0 critic access max]
ttv_DisplayPathListDetail $file $pathlist
fclose $file

# Min access paths
set file [fopen $figname.accessmin w]
set pathlist [ttv_GetPaths $fig * s\[*\] ?? 0 critic access min]
ttv_DisplayPathListDetail $file $pathlist
fclose $file

# Combinational paths
set file [fopen $figname.comb w]
set pathlist [ttv_GetPaths $fig a\[*\] s\[*\] ?? 0 critic path max]
ttv_DisplayPathListDetail $file $pathlist
set pathlist [ttv_GetPaths $fig b\[*\] s\[*\] ?? 0 critic path max]
ttv_DisplayPathListDetail $file $pathlist
set pathlist [ttv_GetPaths $fig sel s\[*\] ?? 0 critic path max]
ttv_DisplayPathListDetail $file $pathlist
set pathlist [ttv_GetPaths $fig ck s\[*\] ?? 0 critic path max]
ttv_DisplayPathListDetail $file $pathlist
fclose $file
