#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config simToolModel hspice
avt_config avtVddName vdd
avt_config avtVssName vss

avt_config simPowerSupply 1.2
avt_config simTemperature 25

#avt_config avtVerboseConeFile yes
#avt_config yagleSplitTimingRatio 0
#avt_config yagleSensitiveTimingRatio 1
#avt_config yagleTasTiming max

#avt_config avtOutputBehaviorFormat vlg

# Technology Parameters

avt_LoadFile ../techno/bsim4_dummy.hsp spice
avt_LoadFile comb.spi spice

yagle comb
