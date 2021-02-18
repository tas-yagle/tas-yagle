#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Spice parser
avt_config avtSpiTolerance high


# Timing Analysis Parameters
avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes
avt_config simToolModel hspice

inf_SetFigureName cpu2901

set_case_analysis 0 test
set_case_analysis 1 fonc

# Technology Parameters
avt_LoadFile cpu2901.spi spice

set fig [hitas cpu2901]
