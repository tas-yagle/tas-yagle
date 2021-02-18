#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config simToolModel hspice
avt_config avtVddName "vdd*"
avt_config avtVssName "vss*"

#avt_config avtVectorize "_"

#avt_config yagleTasTiming max
#avt_config avtOutputBehaviorFormat vlg

# Technology Parameters
avt_LoadFile ../techno/bsim4_dummy.hsp spice

avt_LoadFile ex_m8x8_018.spi spice
avt_LoadFile ex_m32x32_018.spi spice

yagle ex_m8x8
yagle ex_m32x32
