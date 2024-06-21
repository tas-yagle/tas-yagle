#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config simToolModel hspice
avt_config avtVddName "vdd"
avt_config avtVssName "vss"

#avt_config avtOutputBehaviorFormat vlg
#avt_config yagleTasTiming max

# Technology Parameters

avt_LoadFile ../techno/bsim4_dummy.hsp spice
avt_LoadFile addaccu_018.spi spice

yagle addaccu
