#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config simToolModel hspice
avt_config simPowerSupply 1.2
avt_config simTemperature 25
avt_config avtVddName VDD
avt_config avtVssName VSS
avt_config yagleSplitTimingRatio 1
avt_config yagleTasTiming max

# Technology Parameters
avt_LoadFile ../techno/bsim4_dummy.hsp spice

avt_LoadFile glitcher.spi spice


yagle glitcher
