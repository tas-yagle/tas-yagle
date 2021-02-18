

proc stb_FindLagPaths {file slacklist args} {
  proc computelagtime {path} {
    set time 0
    set det [ttv_GetPathDetail $path]
    set latch [ttv_GetTimingPathProperty $path ACCESS_LATCH]
    foreach d $det {
      set delay [ttv_GetTimingDetailProperty $d DELAY]
      set time [expr $time+$delay]
      if {[ttv_GetTimingDetailProperty $d EVENT]==$latch} {
        break
      }
    }
    set delay [ttv_GetTimingPathProperty $path START_TIME]
    set time [expr $time+$delay]
    set delay [ttv_GetTimingPathProperty $path DATA_LAG]
    set time [expr $time+$delay]
    return $time
  }
  proc syncslopes {starttime endtime endperiode} {
    set per 0
    if {$endtime<$starttime} {
    puts "$starttime-$endtime)/$endperiode)"
      set per [expr ceil(($starttime-$endtime)/$endperiode)*$endperiode]
    }
    return $per
  }
  proc getclosingpath {endpath} {
    set cmd [ttv_GetTimingPathProperty $endpath ACCESS_COMMAND]
    set latch [ttv_GetTimingPathProperty $endpath ACCESS_LATCH]
    if {$cmd=="NULL" || $latch==$cmd} {
      return {}
    }
    if {[ttv_GetTimingEventProperty $cmd TRANS]=="u"} {
      set dir "d"
    } else {
      set dir "u"
    }
    set cmdsig [ttv_GetTimingEventProperty $cmd SIGNAL]
    set clksig [ttv_GetTimingPathProperty $endpath START_SIG]
    set type [ttv_GetTimingPathProperty $endpath TYPE]
    set path [ttv_GetPaths -from $clksig -to $cmdsig -dir "?$dir" -$type -nb 1]
    return $path
  }

  global tcl_interactive
  set i 0
  set margin 1
  set lagmargin 0
  set maxdepth 3
  set debug 0
  set displayclosing 0
  
  set lst $args
  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-margin"} {
      set margin [lindex $lst $i]
      incr i
    } elseif {$opt=="-lagmargin"} {
      set lagmargin [lindex $lst $i]
      incr i
    } elseif {$opt=="-maxdepth"} {
      set maxdepth [lindex $lst $i]
      incr i
    } elseif {$opt=="-debug"} {
      set debug 1
    } elseif {$opt=="-closingpath"} {
      set displayclosing 1
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $lst $prei]'"
      puts "Usage: stb_FindLagPaths <file> <slacklist> \[-margin <value>] \[-lagmargin <value>] \[-maxdepth <value>] \[-closingpath]"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }

  set connectors {}
  set num 1
  foreach sl $slacklist {
    if {[stb_GetSlackProperty $sl VALUE]<=$margin} {
      if {[stb_GetSlackProperty $sl TYPE]=="setup"} {
        set type max
      } else {
        set type min
      }
      set datapath [stb_GetSlackProperty $sl DATA_VALID_PATH]
      set lagvalue [ttv_GetTimingPathProperty $datapath DATA_LAG]
      if {$lagvalue>$lagmargin} {
        set depth 0
        set lst [list [list $datapath 0 [list]]]
        set allpaths {}
        set latch [ttv_GetTimingPathProperty $datapath ACCESS_LATCH]
        set topclock [ttv_GetTimingPathProperty $datapath START_EVENT]
        set latchcmd [ttv_GetTimingPathProperty $datapath ACCESS_COMMAND]
        set totdec 0
        while {$depth<$maxdepth && $latch!="NULL" && $lagvalue>$lagmargin} {
          set clock_end [ttv_GetTimingPathProperty $datapath START_SIG]
          set clock_end_event [ttv_GetTimingPathProperty $datapath START_EVENT]
          set endslope [ttv_GetTimingPathProperty $datapath START_TIME]
          set fig [ttv_GetTimingSignalProperty $clock_end TOP_LEVEL]
          set stbfig [ttv_GetTimingFigureProperty $fig STABILITY_FIGURE]
          if {$connectors=={}} {
            set connectors [ttv_GetTimingSignalList $fig connector all]
          }
          set clockinfo [ttv_GetClockInfos $fig [ttv_GetTimingSignalProperty $clock_end NET_NAME] max]
          set period [lindex [split $clockinfo] 5]
          set lagtime [computelagtime $datapath]
          set dir [ttv_GetTimingEventProperty $latch TRANS]
          set sig [ttv_GetTimingEventProperty $latch SIGNAL]
          set inputpaths [ttv_GetPaths -to $sig -access -dir ?$dir -$type]
          set paths_from_input [ttv_GetPaths -from $connectors -to $sig -path -dir ?$dir -$type]
          set inputpaths [concat $inputpaths $paths_from_input]
          if {$debug} {
            fputs " = $endslope => $lagtime" stdout
          }
          set best ""
          set bestdec 0
          set bestdiff 1
          set allpaths [concat $inputpaths $allpaths]
          foreach ip $inputpaths {
            set startslope [ttv_GetTimingPathProperty $ip START_TIME]
            #set dec [syncslopes $startslope $endslope $period]
            set start_node [ttv_GetTimingPathProperty $ip START_EVENT]
            set dec [stb_synchronized_slopes_move $stbfig $start_node $clock_end_event]
            set ip_delay [ttv_GetTimingPathProperty $ip DELAY]
            set ip_lag [ttv_GetTimingPathProperty $ip DATA_LAG]
            set arrivaltime [expr $startslope+$ip_delay+$ip_lag-$dec]
            set diff [expr $arrivaltime-$lagtime]

            set platch [ttv_GetTimingPathProperty $ip ACCESS_LATCH]
            if {$platch == "NULL"} {
              set startsigname "N/A"
              set startsigdir "N/A"
            } else {
              set startsigname [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $platch SIGNAL] NAME]
              set startsigdir  [ttv_GetTimingEventProperty $platch TRANS]
            }
            set startckname [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $ip START_SIG] NAME]
            set startckdir  [ttv_GetTimingEventProperty [ttv_GetTimingPathProperty $ip START_EVENT] TRANS]
            set false [path_false_slack_check $ip $latchcmd $topclock]
            if {$false==0} {
              if {$debug} {
                fputs "   s:$startslope d:$ip_delay m:$ip_lag dec:$dec => $arrivaltime <$diff> $startckname\($startckdir) $startsigname\($startsigdir)\n" stdout
                fputs " /=> [computelagtime $ip]\n" stdout
              }
#              if {$startsigname=="F1867251"} {
#                ttv_DisplayPathListDetail stdout $ip
#              }
              if {$diff>=-1e-14 && $diff<$bestdiff} {
                set bestdiff $diff
                set best $ip
                set bestdec $dec
#                puts "   *best* $diff"
                if {$bestdiff<1e-14} {
                  break
                }
              }
            }
          }
#          if {$best==""} {
#            fputs "   *failed*\n" stdout
#            break
#          }
#          fputs "   ***found best $bestdiff\n" stdout

          set cpath {}
          if {$bestdiff>1e-14 && $displayclosing} {
            set cpath [getclosingpath $datapath]
            if {$cpath!={} && [ttv_GetTimingPathProperty $cpath START_TIME]<[ttv_GetTimingPathProperty $datapath START_TIME]} {
              set newstart [expr [ttv_GetTimingPathProperty $cpath START_TIME]+$period]
              ttv_ChangePathStartTime $cpath $newstart
#              puts "news $newstart $period"
              set allpaths [concat $cpath $allpaths]
            }
          }

          incr depth
          set datapath $best
          lappend lst [list $best $bestdec $cpath]
          set totdec [expr $totdec-$bestdec]
#          puts "b: $totdec $bestdec"
          set latch [ttv_GetTimingPathProperty $datapath ACCESS_LATCH]
          set lagvalue [ttv_GetTimingPathProperty $datapath DATA_LAG]
          set topclock [ttv_GetTimingPathProperty $datapath START_EVENT]
          set latchcmd [ttv_GetTimingPathProperty $datapath ACCESS_COMMAND]
        }
        fputs "*********** LAGGING PATHS FOR SLACK #$num ****************************\n" $file
        fputs "*********** SLACK DETAIL *********************************\n" $file
        stb_DisplaySlackReport $file -slacks $sl
        fputs "*********** LAG PATHS ************************************\n" $file
        set lst [lreverse $lst]
        foreach l $lst {
          set pth [lindex $l 0]
          set cpth [lindex $l 2]
          set newstart [expr [ttv_GetTimingPathProperty $pth START_TIME]+$totdec]
#          puts "d: $totdec $newstart"
          ttv_ChangePathStartTime $pth $newstart
          ttvapi_setdislaytab 1
          ttv_DisplayPathDetail $file $depth $pth
          set totdec [expr $totdec+[lindex $l 1]]
          if {$cpth!={}} {
            fputs "*********** CLOSING CLOCK PATH ************************************\n" $file
#            puts "dc: $totdec [ttv_GetTimingPathProperty $cpth START_TIME]"
            set newstart [expr [ttv_GetTimingPathProperty $cpth START_TIME]+$totdec]
            ttv_ChangePathStartTime $cpth $newstart
            ttvapi_setdislaytab 2
            ttv_DisplayPathDetail $file $depth $cpth
            fputs "\n" $file
          }
          set depth [expr $depth-1]
          ttvapi_setdislaytab 0
        }
        ttv_FreePathList $allpaths
        fputs "*********** END ******************************************\n\n" $file
      }
    }
    incr num
  }

}
