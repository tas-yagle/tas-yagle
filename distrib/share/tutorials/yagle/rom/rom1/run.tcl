#!/usr/bin/env avt_shell

# Global Configuration
avt_config simToolModel hspice
avt_config avtVddName vdd
avt_config avtVssName vss

avt_config yagleSimplifyExpressions yes
avt_config yagleMinimizeInvertors yes
avt_config yagleTristateIsMemory yes
avt_config yagleHzAnalysis yes

# Technology Parameters
avt_LoadFile ../../techno/bsim4_dummy.hsp spice

avt_LoadFile r256x8_1.spi spice

yagle r256x8_1
