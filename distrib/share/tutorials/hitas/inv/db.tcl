#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# General config
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes
avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config simSlope 20e-12
avt_config simToolModel hspice

# Files
avt_LoadFile ./inv.spi spice

# Database generation
set fig [hitas inv]


puts ""
puts "Power supply: [ttv_GetTimingFigureProperty $fig DEF_SUPPLY]V"
puts "Temperature: [ttv_GetTimingFigureProperty $fig TEMP]°C"
set sig [ttv_GetTimingSignal $fig y]
puts ""
puts "y signal capacitance: [ttv_GetTimingSignalProperty $sig CAPA]fF"
