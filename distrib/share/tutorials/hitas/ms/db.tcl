#!/usr/bin/env avt_shell

# Global Configuration
avt_config tasGenerateConeFile yes
avt_config avtVerboseConeFile yes

avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config simToolModel hspice

# Files
avt_LoadFile msdp2_y.spi spice

# Database Construction

set fig [hitas msdp2_y]
