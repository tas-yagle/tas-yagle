#!/usr/bin/env avt_shell

# Global Configuration
avt_config simToolModel hspice
avt_config avtVddName "vdd*"
avt_config avtVssName "vss*"

#avt_config yagleTasTiming max
#avt_config avtOutputBehaviorFormat vlg

# Technology Parameters
avt_LoadFile ../techno/bsim4_dummy.hsp spice

avt_LoadFile cpu2901_018.spi spice

yagle cpu2901
