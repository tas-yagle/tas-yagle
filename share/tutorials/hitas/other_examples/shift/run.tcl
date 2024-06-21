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
avt_LoadFile ex_shift4_018.spi spice
avt_LoadFile ex_shift32_018.spi spice

avt_config tasGenerateConeFile yes

hitas ex_shift4
hitas ex_shift32
