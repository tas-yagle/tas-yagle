#!/usr/bin/env avt_shell

# Global Configuration
avt_config avtLogFile circuit.log
avt_config avtLogEnable "stat:2"

avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes
avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config simToolModel hspice

# Blackboxing config
#avt_SetBlackBoxes {msdp2_y}
#avt_config tasBlackboxRequiresTimings yes

inf_SetFigureName circuit

inf_DefineIgnore resistances R1
#inf_DefineIgnore instances INV1

# Files
avt_LoadFile circuit.spi spice
#avt_LoadFile ./msdp2_y.lib lib

set fig [hitas circuit]

