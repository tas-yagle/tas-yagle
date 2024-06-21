#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config avtLibraryDirs .
avt_config avtVddName "vdd*"
avt_config avtVssName "vss*"

# Technology Parameters

avt_LoadFile ../../techno/bsim4_dummy.hsp spice
avt_LoadFile ex_m8x8_018.spi spice
avt_LoadFile ex_m32x32_018.spi spice

avt_config tasGenerateConeFile yes

hitas ex_m8x8 
hitas ex_m32x32
