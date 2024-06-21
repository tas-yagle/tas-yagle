#!/usr/bin/env avt_shell

set figname addaccu

# Global Configuration
avt_config simVthHigh 0.8
avt_config simVthLow 0.2
avt_config SimToolModel hspice

# Database Construction Options
avt_config tasGenerateConeFile          yes
avt_config avtVerboseConeFile           yes

# Files
avt_LoadFile addaccu_schem.spi spice

# Generate Database

set fig [hitas $figname]
