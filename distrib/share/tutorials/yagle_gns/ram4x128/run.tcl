#!/usr/bin/env avt_shell

# Global Configuration
avt_config avtVddName "vdd:vddp"
avt_config avtVssName "vss:vssp"
avt_config simPowerSupply 1.2
avt_config simTemperature 25

avt_config yagTristateIsMemory yes
avt_config yagMinimizeInvertors yes
avt_config yagVectorize yes
avt_config yagGenerateConeFile yes
avt_config yagUseGenius yes
#avt_config yagleTasTiming max

avt_config avtOutputBehaviorFormat vlg

# Technology Parameters
avt_LoadFile ../techno/bsim4_dummy.hsp spice

avt_LoadFile ram4x128.spi spice

yagle ram4x128
