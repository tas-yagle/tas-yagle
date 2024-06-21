#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes
avt_config simToolModel hspice

# Technology Parameters

avt_LoadFile adder.spi spice

set fig [hitas adder]
