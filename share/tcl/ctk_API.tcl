#
# Copyright (c) 1998-2005, AVERTEC All Rights Reserved
#
# <StabilityFigure> loadcrosstalkfile <TimingFigure>
#
# 
#
#

proc ctk_LoadCrosstalkResults tf {

  global avtdisplay
  if { ![ info exists avtdisplay ] } {
    set avtdisplay 0
  }

  if { $tf != "NULL" } {
    set figname [ ttv_GetTimingFigureProperty $tf FIGNAME ]

    if { $avtdisplay == 1 } {
      puts "reading crosstalk delays"
    }
    ttv_LoadCrosstalkFile $tf
    
    if { $avtdisplay == 1 } {
      puts "reading switching windows"
    }
    set sf [ stb_LoadSwitchingWindows $tf "${figname}.sto" ]

    if { $avtdisplay == 1 } {
      puts "loading agression file"
    }
    ctk_LoadAggressionFile $sf
    return $sf
  } else {
    return NULL
  }
}
