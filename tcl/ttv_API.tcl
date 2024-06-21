proc ttv_DisplaySlewReport {f fig threshold_max threshold_min} {
  ttv_DumpHeader $f $fig
  set lines [ttv_GetLines $fig * * ?? all]
  fputs "----------------------------------------------------------------------------------\n" $f
  fputs "SLOPES >= $threshold_max\n" $f
  fputs "----------------------------------------------------------------------------------\n" $f
  fputs "[format %-32s FROM] [format %-32s TO] [format %16s VALUE]\n" $f
  fputs "----------------------------------------------------------------------------------\n" $f
  foreach line $lines {
    set slope_max [ttv_GetTimingLineProperty $line SLOPE_MAX]
    if {$slope_max >= $threshold_max} {
      set start_event [ttv_GetTimingLineProperty $line START_EVENT]
      set start_signal [ttv_GetTimingEventProperty $start_event SIGNAL]
      set start_name [ttv_GetTimingSignalProperty $start_signal NAME]
      set start_trans [ttv_GetTimingEventProperty $start_event TRANS]
      if {$start_trans == "u"} {
        set start_trans "R"
      } else {
        set start_trans "F"
      }
      set end_event [ttv_GetTimingLineProperty $line END_EVENT]
      set end_signal [ttv_GetTimingEventProperty $end_event SIGNAL]
      set end_name [ttv_GetTimingSignalProperty $end_signal NAME]
      set end_trans [ttv_GetTimingEventProperty $end_event TRANS]
      if {$end_trans == "u"} {
        set end_trans "R"
      } else {
        set end_trans "F"
      }
      fputs "$start_trans [format %-30s $start_name] $end_trans [format %-30s $end_name] [format %15s $slope_max]s\n" $f
    }
  }
  fputs "\n" $f
  fputs "----------------------------------------------------------------------------------\n" $f
  fputs "SLOPES < $threshold_min\n" $f
  fputs "----------------------------------------------------------------------------------\n" $f
  fputs "[format %-32s FROM] [format %-32s TO] [format %16s VALUE]\n" $f
  fputs "----------------------------------------------------------------------------------\n" $f
  foreach line $lines {
    set slope_min [ttv_GetTimingLineProperty $line SLOPE_MIN]
    if {$slope_min < $threshold_min} {
      set start_event [ttv_GetTimingLineProperty $line START_EVENT]
      set start_signal [ttv_GetTimingEventProperty $start_event SIGNAL]
      set start_name [ttv_GetTimingSignalProperty $start_signal NAME]
      set start_trans [ttv_GetTimingEventProperty $start_event TRANS]
      if {$start_trans == "u"} {
        set start_trans "R"
      } else {
        set start_trans "F"
      }
      set end_event [ttv_GetTimingLineProperty $line END_EVENT]
      set end_signal [ttv_GetTimingEventProperty $end_event SIGNAL]
      set end_name [ttv_GetTimingSignalProperty $end_signal NAME]
      set end_trans [ttv_GetTimingEventProperty $end_event TRANS]
      if {$end_trans == "u"} {
        set end_trans "R"
      } else {
        set end_trans "F"
      }
      fputs "$start_trans [format %-30s $start_name] $end_trans [format %-30s $end_name] [format %15s $slope_min]s\n" $f
    }
  }
}

proc ttv_FreeConstraints {objlist args} {
  global tcl_interactive
  set i 0
  set autoclean 1
  set fig [_NULL_]
  while {[lindex $args $i]!=""} {
    set opt [lindex $args $i]
    incr i
    if {$opt=="-fast"} {
      set autoclean 0
    } elseif {$opt=="-fullclean"} {
      set fig [lindex $args $i]
      set autoclean 1
      incr i
    } else {
      puts "ttv_FreeConstraints: unknown option '[lindex args $i]'"
      if {!$tcl_interactive} {
        exit
      }
    } 
  }
 
 ttv_FreeConstraints_sub $objlist $fig $autoclean
}

proc ttv_ProbeDelay {fig args} {
  global tcl_interactive
  if {[llength $args]==6 &&\
      ([string tolower [lindex $args 4]]=="path" || [string tolower [lindex $args 4]]=="access") &&\
      ([string tolower [lindex $args 5]]=="max" || [string tolower [lindex $args 5]]=="min")} {
    return [ttv_ProbeDelay_sub $fig [lindex $args 0] [lindex $args 1] [lindex $args 2] [lindex $args 3] [lindex $args 4] [lindex $args 5] 0 0]
  } else {
    set dir ??
    set minmax max
    set nb -1
    set slope -1
    set noprop 0
    set nosync 0
    set lst $args
    set nodelst [lindex $lst 0]
    set i 1
    while {[lindex $lst $i]!=""} {
      set opt [lindex $lst $i]
      incr i
      if {$opt=="-max"} {
        set minmax max
      } elseif {$opt=="-min"} {
        set minmax min
      } elseif {$opt=="-nosync"} {
        set nosync 1
      } elseif {$opt=="-noprop"} {
        set noprop 1
      } elseif {$opt=="-slope"} {
        set slope [lindex $lst $i]
        incr i
      } elseif {$opt=="-dir"} {
        set dir [lindex $lst $i]
        incr i
      } elseif {$opt=="-nbpaths"} {
        set nb [lindex $lst $i]
        incr i
      } elseif {$opt=="-nb"} {
        set nb [lindex $lst $i]
        incr i
      } else {
        set prei [expr $i-1]
        puts "Unknown option '[lindex $lst $prei]'"
        puts "Usage: ttv_ProbeDelay fig <nodelist> \[-dir <dir>] \[-max|-min] \[-slope <inputslope>] \[-nb <maxpath>]"
        puts "                                     \[-nosync] \[-noprop]"
        if {!$tcl_interactive} {
          exit
        } else {
          return [_NULL_]
        }
      } 
    }
    return [ttv_ProbeDelay_sub $fig $slope $nodelst $dir $nb - $minmax $nosync $noprop]
  }
}

proc ttv_DumpFigure {file fig args} {
  proc geteventname {ev} {
    set name [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $ev SIGNAL] NAME]
    if {[ttv_GetTimingEventProperty $ev TRANS] == "u"} {
        set start_trans "R"
      } else {
        set start_trans "F"
      }
    return "($start_trans) $name"
  }

  set type [lindex $args 0]
  if {$type==""} {
    set type all
  }
  
  set board [Board_CreateBoard]
  Board_SetSize $board 0 10 l
  Board_SetSize $board 1 10 l
  Board_SetSize $board 2 5 l
  Board_SetSize $board 3 5 r
  Board_SetSize $board 4 5 r
  Board_SetSize $board 5 10 l
  Board_NewLine $board
  Board_SetValue $board 0 "Start Event"
  Board_SetValue $board 1 "End Event"
  Board_SetValue $board 2 "Type"
  Board_SetValue $board 3 "DelayMax\[ps]"
  Board_SetValue $board 4 "SlopeMax\[ps]"
  Board_SetValue $board 5 "Command"
  Board_NewSeparation $board

  set ln [ttv_GetLines $fig * * ?? $type]
  foreach l $ln {
    set startev [ttv_GetTimingLineProperty $l START_EVENT]
    set endev [ttv_GetTimingLineProperty $l END_EVENT]
    set type [ttv_GetTimingLineProperty $l TYPE]
    set cmd [ttv_GetTimingLineProperty $l COMMAND]
    
    Board_NewLine $board
    Board_SetValue $board 0 [geteventname $startev]
    Board_SetValue $board 1 [geteventname $endev]
    Board_SetValue $board 2 $type
    if {$cmd!="NULL"} {
      Board_SetValue $board 5 "[geteventname $cmd]"
    }
    set delay [ttv_GetTimingLineProperty $l DELAY_MAX]
    set slope [ttv_GetTimingLineProperty $l SLOPE_MAX]
    Board_SetValue $board 3 "[format %.1f [expr $delay*1e12]]"
    Board_SetValue $board 4 "[format %.1f [expr $slope*1e12]]"
  }
  Board_NewSeparation $board
  Board_Display $file $board ""
  Board_FreeBoard $board
}


proc ttv_GetPaths {args} {
  global tcl_interactive
  proc ttv_gettimingfigure {tvf sigs} {
    if {$tvf==""} {
      foreach sig $sigs {
        set tvf [ttv_GetTimingSignalProperty $sig TOP_LEVEL]
      }
    }
    return $tvf
  }

  set tvf ""
  if {[string first "TimingFigure" [lindex $args 0]]!=-1} {
    set tvf [lindex $args 0]
    set arg7 [string tolower [lindex $args 7]]
    set arg6 [string tolower [lindex $args 6]]
    if {[llength $args]==8 &&\
      ($arg6=="path" || $arg6=="access") &&\
      ($arg7=="min" || $arg7=="max")} {
      set thru [lindex $args 1]
      if {$arg6=="access"} {
        set thru "*"
      }
      return [ttv_GetPaths_sub $tvf [lindex $args 1] $thru [lindex $args 2] [lindex $args 3] [lindex $args 4] [lindex $args 5] [lindex $args 6] [lindex $args 7]]
    }
  }

  set dir ??
  set nb -1
  set minmax "max"
  set critall "critic"
  set pathaccess "path"
  set addasync 0
  
  set lst $args
  if {$tvf==""} {
    set i 0
    set from {}
    set to {}
    set thru {}
  } else {
    set i 1
    set from "*"
    set to "*"
    set thru "*"
  }
  set mode $tvf
  
  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-min"} {
      set minmax "min"
    } elseif {$opt=="-max"} {
      set minmax "max"
    } elseif {$opt=="-critic"} {
      set critall "critic"
    } elseif {$opt=="-all"} {
      set critall "all"
    } elseif {$opt=="-path"} {
      set pathaccess "path"
    } elseif {$opt=="-access"} {
      set pathaccess "access"
    } elseif {$opt=="-findasync"} {
      set addasync 1
    } elseif {$opt=="-from"} {
      set from [lindex $lst $i]
      if {$tvf==""} {
        set tvf [ttv_gettimingfigure $mode $from]
      }
      incr i
    } elseif {$opt=="-to"} {
      set to [lindex $lst $i]
      if {$tvf==""} {
        set tvf [ttv_gettimingfigure $mode $to]
      }
      incr i
    } elseif {$opt=="-thru"} {
      set thru [lindex $lst $i]
      if {$tvf==""} {
        set tvf [ttv_gettimingfigure $mode $thru]
      }
      incr i
    } elseif {$opt=="-dir"} {
      set dir [lindex $lst $i]
      incr i
    } elseif {$opt=="-nb"} {
      set nb [lindex $lst $i]
      incr i
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $lst $prei]'"
      puts "Usage: ttv_GetPaths \[tvf] \[-from <nodelist>] \[-to <nodelist>] \[-thru <nodelist>]"
      puts "                    \[-dir <dir>] \[-nb <nb>] \[-critic|-all] \[-path|-access] \[-max|-min] [-findasync]"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }
  if {$tvf==""} {
    puts "At least the TimingFigure must be specified when calling 'ttv_GetPaths' like that"
    if {!$tcl_interactive} {
      exit
    } else {
      return [_NULL_]
    }
  }

  if {$mode==""} {
    if {$pathaccess=="path"} {
      set thru $from
      set from {}
    } else {
      if {$from=={}} {
        set from [ttv_GetClockList $tvf]
      }
      if {$addasync==1} {
        ttv_SetSearchMode "findasync"
      }
    }
    set res [ttv_internal_GetPaths_EXPLICIT $tvf $from $thru $to $dir $nb $critall $pathaccess $minmax]
    if {$addasync==1} {
      ttv_SetSearchMode "!findasync"
    }
    return $res
  } else {
    if {$pathaccess=="path"} {
      set thru $from
    } else {
      if {$addasync==1} {
        ttv_SetSearchMode "findasync"
      }
    }
    set from [join $from]
    set thru [join $thru]
    set to [join $to]

    set res [ttv_GetPaths_sub $tvf $from $thru $to $dir $nb $critall $pathaccess $minmax]
    if {$addasync==1} {
      ttv_SetSearchMode "!findasync"
    }
    return $res
  }
}

proc ttv_SimulatePath {tvf pth args} {
  set mode ""
  set mc 0
  set i 0
  global tcl_interactive
  
  set lst $args
  if {[lindex $lst 0]=="{}"} {
    set lst [lindex $lst 0]
  }
  if {[llength $lst]==1 && [lindex $lst $i]=="force"} {
    set mode $mode"force\ "
  } elseif {[llength $lst]==1 && [lindex $lst $i]==""} {
    set force ""
  } else {  
    while {[lindex $lst $i]!=""} {
      set opt [lindex $lst $i]
      incr i
      if {$opt=="-force"} {
        set mode $mode"force\ "
      } elseif {$opt==""} {
      } elseif {$opt=="-mc"} {
        set mc [lindex $lst $i]
        incr i
      } elseif {$opt=="-plot"} {
        set mode $mode"plot\ "
      } else {
        set prei [expr $i-1]
        puts "Unknown option '[lindex $lst $prei]'"
        puts "Usage: ttv_SimulatePath <tvf> <path> \[-force] \[-mc <nb>] \[-plot]"
        if {!$tcl_interactive} {
          exit
        } else {
          return [_NULL_]
        }
      } 
    }
  }
  return [ttv_SimulatePath_sub $tvf $pth $mode $mc]
}

proc ttv_SimulatePathDetail {tvf pth args} {
 return [ttv_SimulatePath $tvf $pth $args]
}

proc ttv_GetTimingFigure {figname args} {
 if {[llength $args]==0} {
   return [ttv_GetTimingFigure_sub $figname -1 -1]
 } else {
   return [ttv_GetTimingFigure_sub $figname [lindex $args 0] [lindex $args 1]]
 }
}

proc ttv_DumpLeakage {file fig} {

  set sl [ttv_GetMatchingSignal $fig * any]
  set board [Board_CreateBoard]
  Board_SetSize $board 0 5 l
  Board_SetSize $board 1 5 r
  Board_SetSize $board 2 5 r
  Board_SetSize $board 3 5 r
  Board_SetSize $board 4 5 r
  Board_NewLine $board
  Board_SetValue $board 0 "Signal Name"
  Board_SetValue $board 1 "Leakage UP Max"
  Board_SetValue $board 2 "Leakage UP Min"
  Board_SetValue $board 3 "Leakage DOWN Max"
  Board_SetValue $board 4 "Leakage DOWN Min"
  Board_NewSeparation $board

  foreach sig $sl {
    Board_NewLine $board
    Board_SetValue $board 0 "[ttv_GetTimingSignalProperty $sig NET_NAME]"
    Board_SetValue $board 1 "[format %.3g [ttv_GetTimingSignalProperty $sig LEAKAGE_UP_MAX]]"
    Board_SetValue $board 2 "[format %.3g [ttv_GetTimingSignalProperty $sig LEAKAGE_UP_MIN]]"
    Board_SetValue $board 3 "[format %.3g [ttv_GetTimingSignalProperty $sig LEAKAGE_DN_MAX]]"
    Board_SetValue $board 4 "[format %.3g [ttv_GetTimingSignalProperty $sig LEAKAGE_DN_MIN]]"

  }
  Board_NewSeparation $board
  Board_Display $file $board ""
  Board_FreeBoard $board

}
