#!/usr/bin/env avt_shell

#############################################################
# Timing Database Generation                                #
#############################################################

# Global Configuration
avt_config avtVddName "vdd*"
avt_config avtVssName "vss*"

# Technology Parameters

avt_LoadFile ../../techno/bsim4_dummy.hsp spice
avt_LoadFile m11.spi spice
avt_LoadFile m12.spi spice
avt_LoadFile m22.spi spice
avt_LoadFile m21.spi spice
avt_LoadFile m31.spi spice

hitas m11
hitas m12
hitas m22

avt_config tasHierarchicalMode yes

hitas m21
hitas m31

# Static Timing Analysis

inf_SetFigureName m31

create_clock -period 3000 -waveform {0 1500} ck

set_input_delay -clock ck -clock_fall -min 400 *
set_input_delay -clock ck -clock_fall -max 600 *

inf_Drive m31
