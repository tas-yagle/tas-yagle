#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config simToolModel hspice
avt_config avtVddName "vdd*"
avt_config avtVssName "vss*"
avt_config yagleAnalysisDepth 6
avt_config yagSimplifyExpressions yes

#avt_config yagleTasTiming max
#avt_config avtOutputBehaviorFormat vlg

# Technology Parameters
avt_LoadFile ../techno/bsim4_dummy.hsp spice

avt_LoadFile ex_shift4_018.spi spice
avt_LoadFile ex_shift32_018.spi spice

yagle ex_shift4
yagle ex_shift32
