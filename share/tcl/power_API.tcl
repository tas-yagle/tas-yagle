proc GetSwitchingPowerSignal {fig signame args} {
  global tcl_interactive
  set interval -1
  set begindate 0
  set enddate -1
  set plot "NULL"
  set lst $args
  set i 0
  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-interval"} {
      set interval [lindex $lst $i]
      incr i
    } elseif {$opt=="-begindate"} {
      set begindate [lindex $lst $i]
      incr i
    } elseif {$opt=="-enddate"} {
      set enddate [lindex $lst $i]
      incr i
    } elseif {$opt=="-plot"} {
      set plot [lindex $lst $i]
      incr i
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $lst $prei]'"
      puts "Usage: GetSwitchingPowerSignal fig signame \[-interval <interval>] \[-begindate <begindate>]"
      puts "                                           \[-enddate <enddate>] \[-plot <filename>]"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }
  return [GetSwitchingPowerSignal_sub $fig $signame $interval $begindate $enddate $plot]
}

