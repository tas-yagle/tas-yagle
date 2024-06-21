
proc stb_DisplaySlackReport {file args} {
  global tcl_interactive
  set sf ""
  if {[string first "StabilityFigure" [lindex $args 0]]!=-1} {
    set sf [lindex $args 0]
    set arg5 [string tolower [lindex $args 5]]
    if {[llength $args]==7 && [string index $arg5 0]!="-" &&\
      ([string first "hold" $arg5]!=-1 || [string first "setup" $arg5]!=-1 || [string first "all" $arg5]!=-1)} {
      return [stb_DisplaySlackReport_sub $file $sf [lindex $args 1] [lindex $args 2] [lindex $args 3] [lindex $args 4] [lindex $args 5] [lindex $args 6]]
    }
  }

  set dir ??
  set from "*"
  set to "*"
  set nb -1
  set marginval 0
  set what "all"
  set summary ""
  set margins ""
  set thru ""
  set simple ""
  set thru_node ""
  set noprechlag ""
  set slacklist "none"
  
  set lst $args
  if {$sf==""} {
    set i 0
  } else {
    set i 1
  }
  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-setuponly"} {
      set what "setup"
    } elseif {$opt=="-holdonly"} {
      set what "hold"
    } elseif {$opt=="-summary"} {
      set summary "summary"
    } elseif {$opt=="-displaythru"} {
      set thru "thru"
    } elseif {$opt=="-simple"} {
      set simple "simple"
    } elseif {$opt=="-noprechlag"} {
      set noprechlag "noprechlag"
    } elseif {$opt=="-from"} {
      set from [lindex $lst $i]
      incr i
    } elseif {$opt=="-to"} {
      set to [lindex $lst $i]
      incr i
    } elseif {$opt=="-thru_node"} {
      set thru_node "thru=[lindex $lst $i]"
      incr i
    } elseif {$opt=="-dir"} {
      set dir [lindex $lst $i]
      incr i
    } elseif {$opt=="-slacks"} {
      set slacklist [lindex $lst $i]
      incr i
    } elseif {$opt=="-displaymargins"} {
      set margins "margins"
    } elseif {$opt=="-margin"} {
      set marginval [lindex $lst $i]
      incr i
    } elseif {$opt=="-nbslacks"} {
      set nb [lindex $lst $i]
      incr i
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $lst $prei]'"
      puts "Usage: stb_DisplaySlackReport <file> \[sf] \[-from <nodename>] \[-to <nodename>] \[-thru_node <nodename>] \[-dir <dir>] \[-nbslacks <nb>]"
      puts "                                     \[-margin <marginval>] \[-setuponly] \[-holdonly] \[-slacks <stabilityslackslist>] \[-noprechlag]"
      puts "       display options:              \[-simple] \[-summary] \[-displaythru] \[-displaymargins]"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }
  if {$slacklist=="none"} {
    return [stb_DisplaySlackReport_sub $file $sf $from $to $dir $nb "$what $margins $thru $thru_node $simple $noprechlag $summary" $marginval]
  } else {
    return [stb_DisplaySlackReport_sub2 $file $slacklist $nb "$what $margins $thru $simple $summary"]
  }
}

#-----------------------------------------------------

proc stb_sort_so {a b} {
 set va [stb_GetSlackProperty $a VALUE]
 set vb [stb_GetSlackProperty $b VALUE]
 if {$va<$vb} {
   return -1
 } elseif {$vb<$va} {
   return 1
 }
 return 0
}

proc stb_SortSlacks {args} {
 set allso {}
 foreach ag $args {
   set allso [concat $allso $ag]
 }
 set solist [lsort -command stb_sort_so $allso]
 return $solist
}

proc stb_ComputeSlacks {args} {
  global tcl_interactive
  set sf ""
  set fault ""
  set margin -1
  set nextcycle 0
  set nosync 0
  set i 0
  if {[string first "StabilityFigure" [lindex $args 0]]!=-1} {
    set sf [lindex $args 0]
    set i 1
  }
  set datavalidspec [lindex $args $i]
  incr i
  set operation [lindex $args $i]
  incr i
  set datarequiredspec [lindex $args $i]
  incr i
  
  set op -1
  if {[string tolower $operation]=="before"} {
    set op 1
    set val_minmax -max
    set req_minmax -min
  } elseif {[string tolower $operation]=="after"} {
    set op 0
    set val_minmax -min
    set req_minmax -max
  } else {
    set fault "Unknown operation '$operation'"
  }

   while {$fault=="" && [lindex $args $i]!=""} {
    set opt [lindex $args $i]
    incr i
    if {$opt=="-nextcycle"} {
      set nextcycle 1
    } elseif {$opt=="-nosync"} {
      set nosync 1
    } elseif {$opt=="-margin"} {
      set margin [lindex $args $i]
      incr i
    } else {
      set fault "Unknown option '$opt'"
    } 
  }

  if {$fault==""} {  
    if {$sf!=""} {
      set fig [stb_GetStabilityFigureProperty $sf TIMING_FIGURE]
      set lowstr [string tolower $datavalidspec]
      set minmax ""
      if {[string first "-min" $lowstr]==-1 && [string first "-max" $lowstr]==-1} {
        set minmax $val_minmax
      }
      set datavalid_paths [eval ttv_ProbeDelay $fig $datavalidspec $minmax]
      set lowstr [string tolower $datarequiredspec]
      set minmax ""
      if {[string first "-min" $lowstr]==-1 && [string first "-max" $lowstr]==-1} {
        set minmax $req_minmax
      }
      set datarequired_paths [eval ttv_ProbeDelay $fig $datarequiredspec $minmax]
    } else {
      set datavalid_paths $datavalidspec
      set datarequired_paths $datarequiredspec
    }
    set res {}
    foreach dvp $datavalid_paths {
      foreach drp $datarequired_paths {
        set so [stb_ComputeOneSlack $op $dvp $drp $margin $nextcycle $nosync]
        lappend res $so
      }
    }
    ttv_FreePathList $datavalid_paths
    ttv_FreePathList $datarequired_paths
    set solist [lsort -command stb_sort_so $res]
    return $solist
  }
  
  puts "$fault"
  puts "Usage:   stb_ComputeSlacks <stabilityfigure> <probedelay specification> (before|after) <probedelay specification>"
  puts "  or     stb_ComputeSlacks <path_list> (before|after) <path_list>"
  puts "options:   \[-nextperiod] \[-margin <marginval>] \[-nosync]"
  if {!$tcl_interactive} {
    exit
  }
}


#-----------------------------------------------------

proc stb_GetSlacks {sf args} {
  global tcl_interactive
  set arg4 [string tolower [lindex $args 4]]
  if {[llength $args]==6 && [string index $arg4 0]!="-" &&\
    ([string first "hold" $arg4]!=-1 || [string first "setup" $arg4]!=-1)} {
    return [stb_GetSlacks_sub $sf [lindex $args 0] [lindex $args 1] [lindex $args 2] [lindex $args 3] [lindex $args 4] [lindex $args 5] "*" 0]
  }

  set dir ??
  set from "*"
  set to "*"
  set thru_node "*"
  set nb -1
  set marginval 0
  set what "all"
  set noprechlag 0
  
  set lst $args
  set i 0
  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-setuponly"} {
      set what "setup"
    } elseif {$opt=="-holdonly"} {
      set what "hold"
    } elseif {$opt=="-noprechlag"} {
      set noprechlag 1
    } elseif {$opt=="-from"} {
      set from [lindex $lst $i]
      incr i
    } elseif {$opt=="-to"} {
      set to [lindex $lst $i]
      incr i
    } elseif {$opt=="-thru_node"} {
      set thru_node [lindex $lst $i]
      incr i
    } elseif {$opt=="-dir"} {
      set dir [lindex $lst $i]
      incr i
    } elseif {$opt=="-margin"} {
      set marginval [lindex $lst $i]
      incr i
    } elseif {$opt=="-nbslacks"} {
      set nb [lindex $lst $i]
      incr i
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $lst $prei]'"
      puts "Usage: stb_GetSlacks <sf> \[-from <nodename>] \[-to <nodename>] \[-thru_node <nodename>] \[-dir <dir>] \[-nbslacks <nb>]"
      puts "                          \[-margin <marginval>] \[-setuponly] \[-holdonly] \[-noprechlag]"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }

  set res {}
  if {$what=="all" || $what=="setup"} {
    set res [concat $res [stb_GetSlacks_sub $sf $from $to $dir $nb "setup" $marginval $thru_node $noprechlag]]
  }
  if {$what=="all" || $what=="hold"} {
    set res [concat $res [stb_GetSlacks_sub $sf $from $to $dir $nb "hold" $marginval $thru_node $noprechlag]]
  }
  return $res
}

#-----------------------------------------------------

proc stb_DisplayCoverage {file sf args} {
  global tcl_interactive

  set detail 0
  
  set lst $args
  set i 0
  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-detail"} {
      set detail 1
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $lst $prei]'"
      puts "Usage: stb_DisplayCoverage <file> <sf> \[-detail]"
      if {!$tcl_interactive} {
        exit
      } else {
        return
      }
    } 
  }
  stb_DisplayCoverage_sub $file $sf $detail
}


