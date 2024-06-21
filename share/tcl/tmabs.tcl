
set tmabs_SIMMODE         0
set tmabs_MAXSIM          1
set tmabs_SIMLIST         2
set tmabs_SCALE           3
set tmabs_VERBOSE         4
set tmabs_MINMAX          5
set tmabs_CSTMODE         6
set tmabs_CACHEMODE       7
set tmabs_DUMPDTX         8
set tmabs_EXITONERROR     9
set tmabs_IGNOREMARGINS   10
set tmabs_DETECTFALSEPATH 11
set tmabs_DETAILFILE      12
set tmabs_A_BEH           13
set tmabs_A_CK            14
set tmabs_A_INPUT         15
set tmabs_A_OUTPUT        16
set tmabs_A_INTERNAL      17
set tmabs_A_ADDITION_PINS 18

array set tmabs_quick_name_to_signal {}

proc tmabs_addcorresp {lst} {
  global tmabs_quick_name_to_signal
  foreach l $lst {
    set tmabs_quick_name_to_signal([lindex $l 0]) [lindex $l 1]
  }
}

proc tmabs_getcorresp {name} {
  global tmabs_quick_name_to_signal
  if {[info exists tmabs_quick_name_to_signal($name)]} {
    return $tmabs_quick_name_to_signal($name)
  }
  return ""
}

#-- setup hold computation methods
proc tmabs_Setup_Computation {dvt dloop hitasintrinsic} {
  return [expr $dloop-$dvt]
}

proc tmabs_Hold_Computation {dvt dloop hitasintrinsic} {
  return $dvt
}

proc tmabs_SetupHold_ReflectHiTasValue {dvt dloop hitasintrinsic} {
  return $hitasintrinsic
}

proc tmabs_Computation_Intrinsic {setuphold dvttab dlooptab sx_s sy_s hitasintrinsic cfg} {
  if {[llength $dvttab]==0 || [llength $dlooptab]==0} {
    set func_to_call tmabs_SetupHold_ReflectHiTasValue
  } elseif {$setuphold=="setup"} {
    set func_to_call tmabs_Setup_Computation
  } else {
    set func_to_call tmabs_Hold_Computation
  }
  set intrtab {}
  for {set sx 0} {$sx<$sx_s} {incr sx} {
    set subintrtab {}
    for {set sy 0} {$sy<$sy_s} {incr sy} {
      set dloopval [lindex [lindex $dlooptab $sx] 0]
      set dvtval [lindex [lindex $dvttab $sy] 0]
      set intr [$func_to_call $dvtval $dloopval $hitasintrinsic]
      lappend subintrtab $intr
    }
    lappend intrtab $subintrtab
  }
#  puts "dv: $dvttab"
#  puts "dl: $dlooptab"
  tmabs_Verbose 2 $cfg "Computation_Intrinsic: ($setuphold)=$intrtab / loop: $dloopval , dvt: $dvtval"
  return $intrtab
}

#-- simulated delay and slope cache to accelerate delay and slope computations
set tmabs_table_done {}

proc tmabs_lsearch {lst val} {
  foreach l $lst {
    if {[lindex $l 0]==$val} {
      return 1;
    }
  }
  return -1;
}

proc tmabs_Verbose {lv cfg str} {
  
  global tmabs_VERBOSE
  
  set verbose [lindex $cfg $tmabs_VERBOSE]
  if {$lv<=$verbose} {
    puts "\[tmabs:$lv\] $str"
  }
}
proc tmabs_CheckSimulationMode {con ckcon cfg} {

  global tmabs_SIMLIST

  set SimList [lindex $cfg $tmabs_SIMLIST]

  if {$SimList==""} {
    return 0
  }

  foreach sm $SimList {
    set thiscon [lindex $sm 0]
    set thisckcon [lindex $sm 1]
    if {[avt_RegexIsMatching $con $thiscon] && ($thisckcon=="" || [avt_RegexIsMatching $ckcon $thisckcon])} {
      return 1
    }
  }
  return -1
}
proc tmabs_TruncAxis { axis } {
  set truncaxis {}
  foreach ax $axis {
    set truncv [format %.5g $ax]
    lappend truncaxis $truncv
  }
  return $truncaxis
}
#-- creates a list with a delay table and a slope table depending on given slope_axis
#-- and capa_axis
proc tmabs_CodePathDetail { path } {
  set detstring ""
  set detail [ttv_GetPathDetail $path ]
  foreach det $detail {
    set detname [ttv_GetTimingDetailProperty $det NODE_NAME]
    set detdir [ttv_GetTimingDetailProperty $det TRANS]
    set detstring "$detstring$detname$detdir"
  }
  set code [avt_CodeName $detstring]
  return $code
}
proc tmabs_PathInfo {path masterclock} {
  set hz ""
  if {[ttv_GetTimingPathProperty $path IS_HZ]=="yes"} {
   set hz " | HZ"
  }
  set start [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $path START_SIG] NAME]
  if {$masterclock=="" || $masterclock==$start} {
    set masterclock ""
  } else {
    set masterclock "($masterclock) - "
  }
  set labstart "$masterclock$start\([ttv_GetTimingPathProperty $path START_TRANS])"
  set al [ttv_GetTimingPathProperty $path ACCESS_LATCH]
  if {$al!="NULL"} {
    set mid [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $al SIGNAL] NAME]
    set labmid " - $mid\([ttv_GetTimingEventProperty $al TRANS])"
  } else {
    set labmid ""
  }
  set end [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $path END_SIG] NAME]
  set labend " - $end\([ttv_GetTimingPathProperty $path END_TRANS])"
  return "$labstart$labmid$labend$hz | [ttv_GetTimingPathProperty $path TYPE]"
}

proc tmabs_BuildPathKey {path opp latch} {
  set hz ""
  if {[ttv_GetTimingPathProperty $path IS_HZ]=="yes"} {
   set hz "(hz)"
  }
  set start [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $path START_SIG] NAME]
  set labstart "$start\([ttv_GetTimingPathProperty $path START_TRANS])"
  set al [ttv_GetTimingPathProperty $path ACCESS_LATCH]
  if {$al!="NULL"} {
    set mid [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $al SIGNAL] NAME]
    set labmid "$mid\([ttv_GetTimingEventProperty $al TRANS])"
  } else {
    set labmid ""
  }
  set end [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $path END_SIG] NAME]
  set labend "$end\([ttv_GetTimingPathProperty $path END_TRANS])"

  if {$latch!=""} {
    set lat [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $latch SIGNAL] NAME]
    set lablat "$lat\([ttv_GetTimingEventProperty $latch TRANS])"
  } else {
    set lablat ""
  }
  set ptype [ttv_GetTimingPathProperty $path TYPE]
  if {$opp==1} {
    if {$ptype=="min"} {
      set ptype "max"
    } else {
      set ptype "min"
    }
  }
  set detailcode [format "%x" [tmabs_CodePathDetail $path]]
  return "$labstart$labmid$labend$lablat$hz$ptype\($detailcode)"
}

proc tmabs_CharacPathTables {bbox fig path slope_axis capa_axis forcesimu cfg masterclock latch} {

 global tmabs_table_done
 global tmabs_MAXSIM
 global tmabs_SIMMODE
 global tmabs_CACHEMODE
 global tmabs_EXITONERROR

 set tmabs_Simulate [lindex $cfg $tmabs_SIMMODE]
 set tmabs_MaxSim [lindex $cfg $tmabs_MAXSIM]

 tmabs_Verbose 1 $cfg "  - characterizing path : [tmabs_PathInfo $path $masterclock]"
# ttv_DisplayPathListDetail stdout $path
 set mode $tmabs_Simulate
 if {$forcesimu==-1} {
   set mode 0
 } elseif {$forcesimu==1}  {
   set mode 1
 }
 if {$mode==0} {
   set res [ttv_CharacPathTables $path $slope_axis $capa_axis 1]
   return $res
 } else {
   set pathid [tmabs_BuildPathKey $path 0 $latch]
   set pathidaxis "[join [tmabs_TruncAxis $slope_axis] /]-[join [tmabs_TruncAxis $capa_axis] /]"
   set key "$pathid/$pathidaxis"
   foreach table $tmabs_table_done {
     set thiskey [lindex $table 0]
     if {$thiskey==$key} {
       tmabs_Verbose 2 $cfg "  - path in cache : [tmabs_PathInfo $path $masterclock]"
       return [lindex $table 1]
     }
   }
 
   set failed 0
   set simcharac [ttv_SimulateCharacPathTables $fig $path $slope_axis $capa_axis $tmabs_MaxSim]
   if {$simcharac==""} {
     set failed 1
   }
   set slopetab {}
   set delaytab {}
   set dvttab {}
   set dlooptab {}
   set sx_s [llength $slope_axis]
   set cx_s [llength $capa_axis]
   for {set sx 0} {$sx<$sx_s} {incr sx} {
     set subslopetab {}
     set subdelaytab {}
     set subdloop {}
     set subdvt {}
     set run_sx [lindex $simcharac $sx]
     for {set cx 0} {$cx<$cx_s} {incr cx} {
        if {$simcharac!=""} {
          set cpath [lindex $run_sx $cx]
          set slope [ttv_GetTimingPathProperty $cpath SIM_SLOPE]
          set delay [ttv_GetTimingPathProperty $cpath SIM_DELAY]
          set dvt [ttv_GetTimingPathProperty $cpath SIM_DELAY_TO_VT]
          set dloop [ttv_GetTimingPathProperty $cpath SIM_LOOP_DELAY]
          #-- sometime the loop delay does not exist, eg. for non moving precharged node
          if {$dloop==-1} {
            set dloop 0
          }
          ttv_FreePathList $cpath
        } else {
          set slope -1
          set delay -1
          set dvt 0
          set dloop 0
        }
        if {$slope<-0.5 || $delay<-0.5} {
          set failed 1
        }
        lappend subdelaytab $delay
        lappend subslopetab $slope
        lappend subdvt $dvt
        lappend subdloop $dloop
     }
     lappend slopetab $subslopetab
     lappend delaytab $subdelaytab
     lappend dvttab $subdvt
     lappend dlooptab $subdloop
   }
   if {[ttv_GetTimingPathProperty $path DELAY]!=0} {
     set res [list $delaytab $slopetab {} $dvttab $dlooptab] 
   } else {
     #-- special case: connector at the interface: use Hitas loop delay and delay to vt
     set res [list $delaytab $slopetab {}]
   }
   lappend tmabs_table_done [list $key $res]
   if {$failed==1} {
     set startnode [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $path START_SIG] NAME]
     set startdir [ttv_GetTimingPathProperty $path START_TRANS]
     set endnode [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $path END_SIG] NAME]
     set enddir [ttv_GetTimingPathProperty $path END_TRANS]
     puts "\[tmabs error\] Simulation failed for path from $startnode to $endnode $startdir$enddir"
     if {[lindex $cfg $tmabs_EXITONERROR]==1} {
       exit 2
     }
   } else {
     if {[lindex $cfg $tmabs_CACHEMODE]==1} {
       set fl [open "[ttv_GetTimingFigureProperty $bbox NAME].simulations.cache" a]
       puts $fl "{$key} {$res}"
       close $fl
     }
     if {[ttv_Simulate_FoundSolutions]==1} {
       set pathid [tmabs_BuildPathKey $path 1 $latch]
       set key "$pathid/$pathidaxis"
       lappend tmabs_table_done [list $key $res]
       tmabs_Verbose 1 $cfg "    - one solution found, min = max"
       if {[lindex $cfg $tmabs_CACHEMODE]==1} {
         set fl [open "[ttv_GetTimingFigureProperty $bbox NAME].simulations.cache" a]
         puts $fl "{$key} {$res}"
         close $fl
       }
     }
    }
   return $res
 }
}

#-- create default connector or capacitance axis
proc tmabs_CreateAxis {fig type signalname cfg} {

  global tmabs_SCALE

  set tmabs_scale_values [lindex $cfg $tmabs_SCALE]

  set scale_values $tmabs_scale_values
  set signal [tmabs_getcorresp $signalname]
  if {$signal==""} {
    set signal [lindex [ttv_GetMatchingSignal $fig $signalname ""] 0]
    tmabs_addcorresp [list $signalname $signal]
  }
  if {$type=="capacitance"} {
    if {$signal=="" || [string first "connector" [ttv_GetTimingSignalProperty $signal TYPE]]==-1} {
      return {-1}
    } else {
      set val [expr [ttv_GetTimingSignalProperty $signal CAPA]-[ttv_GetTimingSignalProperty $signal LOAD]]
    }
  } else {
    if {[string first "connector" [ttv_GetTimingSignalProperty $signal TYPE]]==-1} {
      return {-1}
    } else {
      set riseval [ttv_GetTimingSignalProperty $signal RISING_SLOPE]
      set fallval [ttv_GetTimingSignalProperty $signal FALLING_SLOPE]
      set val [expr ($riseval+$fallval)/2]
    }
  }
  set axis {}
  foreach scale_value $scale_values {
    set newval [expr $val*$scale_value]
    lappend axis $newval
  }
  return $axis
}

proc tmabs_GetConnectorAxis {fig type signal cfg} {
 set axis [tma_GetConnectorAxis $fig $type $signal]
 if {[lindex $axis 0]<=0} {
   return [tmabs_CreateAxis $fig $type $signal $cfg]
 }
 return $axis
}

#-- retreives clocks information
proc tmabs_FindClockInfo {lst event minmax} {
  foreach cinfo $lst {
    if {[lindex $cinfo 0]==$event && [lindex $cinfo 1]==$minmax} {
      return $cinfo
    }
  }
  return ""
}

#-- converts a Nx1 table to a N-element list
proc tmabs_Table_Nx1_to_N {table} {
  set newtab {}
  foreach N $table {
    lappend newtab [lindex $N 0]
  }
  return $newtab
}

#-- returns the sum of 2 N-element list
proc tmabs_Table_N_Sum {t1 t2} {
  if {[llength $t1]!=[llength $t2]} {
    puts "\[tmabs error\] NSum: Mismathing list length [llength $t1] != [llength $t2]"
    exit
  }
  set i 0
  set newtab {}
  foreach N1 $t1 {
    set N2 [lindex $t2 $i]
    set val [expr $N1+$N2]
    lappend newtab $val
    incr i
  }
  return $newtab
}

#-- returns the sum of a NxM table with a N-element list
proc tmabs_Table_NxM_N_Sum {t1 t2} {
  if {[llength $t1]!=[llength $t2]} {
    puts "\[tmabs error\] NMNSum: Mismathing list length [llength $t1] != [llength $t2]"
    exit
  }
  set newtab {}
  set i 0
  foreach N $t1 {
    set subtab {}
    set t2val [lindex $t2 $i]
    foreach M $N {
      set val [expr $M+$t2val]
      lappend subtab $val
    }
    lappend newtab $subtab
    incr i
  }
  return $newtab
}

#-- returns the sum of a NxM table with a N-element list
proc tmabs_Table_NxM_NxM_Sum {t1 t2} {
  if {[llength $t1]!=[llength $t2]} {
    puts "\[tmabs error\] NMNSum: Mismathing list length [llength $t1] != [llength $t2]"
    exit
  }
  set newtab {}
  set i 0
  foreach N $t1 {
    set subtab {}
    set t2val [lindex $t2 $i]
    foreach M $N {
      set val [expr $M+$t2val]
      lappend subtab $val
    }
    lappend newtab $subtab
  }
  return $newtab
}

#--
proc tmabs_OutputDetail {cfg txt path} {
  global tmabs_DETAILFILE
  set filename [lindex $cfg $tmabs_DETAILFILE]
  if {$filename!=""} {
    set of [fopen $filename a+]
    if {$txt!=""} {
      fputs $txt $of
    }
    if {$path!=""} {
      ttv_DisplayPathDetail $of -1 $path
    }
    fclose $of
  }
}

#-- adds divide_by/multiply_by, dutycycle, master clock information on generated clocks 
proc tmabs_AddGeneratedClocksInfo {bbox fig cfg} {
  tmabs_Verbose 1 $cfg "- **** GENERATED CLOCKS INFORMATION *****"
  set allclocks [ttv_GetClockList $fig]
  foreach clock $allclocks {
   set clockname [ttv_GetTimingSignalProperty $clock NAME]   
   set clockinfos [ttv_GetClockInfos $fig [ttv_GetTimingSignalProperty $clock NET_NAME] max]
   set clockinfos [split $clockinfos]
   set master [lindex $clockinfos 7]
   if {$master!="none"} {
     tmabs_Verbose 2 $cfg "  - clock info: $clockname :  $clockinfos"
     set rise [lindex $clockinfos 1]
     set fall [lindex $clockinfos 3]
     set period [lindex $clockinfos 5]
     set mperiod [lindex $clockinfos 13]
     set dutycycle [expr abs(($rise-$fall)/$period)*100]
     if {$mperiod>$period} {
       set oper [expr -round($mperiod/$period)]
     } else {
       set oper [expr round($period/$mperiod)]
     }
     tmabs_Verbose 1 $cfg "  - operation for $clockname: x$oper dutycycle=[format %.2g $dutycycle] master=$master"
     tma_SetGeneratedClockInfo $bbox $clockname "$oper [format %.2g $dutycycle]% $master"
   }
  }
 set bboxname [ttv_GetTimingFigureProperty $bbox NAME]
 inf_SetFigureName $bboxname
 foreach clock $allclocks {
   set clockname [ttv_GetTimingSignalProperty $clock NAME]
   set inbbox [ttv_GetTimingSignal $bbox $clockname]
   if {$inbbox!="NULL"} {
     inf_DefineClock $clockname
     tmabs_Verbose 1 $cfg "  - clock in blackbox: $clockname"
     if {[ttv_GetTimingSignalProperty $clock IS_ASYNCHRONOUS]=="yes"} {
       inf_DefineAsynchron $clockname
       tmabs_Verbose 1 $cfg "  - asynchronous in blackbox: $clockname"
     }
   }
 }
 # to apply inf changes:
 ttv_UpdateInf $bbox
}

#-- creates a list of slope axis and clock latency 
proc tmabs_BuildClockConnectorInformations {bbox fig cfg} {
  global tmabs_A_INTERNAL
  tmabs_Verbose 1 $cfg "- **** CLOCKS & GENERATED CLOCKS *****"
  set allclockinfo {}
  set allclocks [ttv_GetClockList $fig]
  set internals [lindex $cfg $tmabs_A_INTERNAL]
  set internals [tmabs_FilterInternal $fig $internals yes]
  foreach clock $allclocks {
    set clockname [ttv_GetTimingSignalProperty $clock NAME]
    if {[tmabs_lsearch $internals $clockname]!=-1} {
      set destclock [ttv_AddBreakpoint $clockname]
      tma_TransfertSignalInformation $clock $destclock
    }
  }
  foreach clock $allclocks {
    foreach event {"EVENT_UP" "EVENT_DOWN"} {
      foreach type {"min" "max"} {
         set clockevent [ttv_GetTimingSignalProperty $clock $event]
         if {[tmabs_FindClockInfo $allclockinfo $clockevent $type]==""} {
            set clockname [ttv_GetTimingSignalProperty $clock NAME]
            set generationpaths {}
            if {[tmabs_lsearch $internals $clockname]==-1} {
              set generationpaths [ttv_GetGeneratedClockPaths $fig $clockevent $type]
            }
            
            if {$generationpaths=={}} {
              #-- clocks at the interface
              set slope_axis [tmabs_GetConnectorAxis $fig "slope" $clockname $cfg]
              set slope_latencies {}
              foreach sv $slope_axis {
                lappend slope_latencies 0
              }
              lappend allclockinfo [list $clockevent $type $slope_axis $slope_latencies $clockevent]
              tmabs_Verbose 2 $cfg "  - master clock [ttv_GetTimingSignalProperty $clock NAME] / $event / $type"
              tmabs_Verbose 2 $cfg "    slope axis: $slope_axis"
            } else {
              #-- generated clocks
              set masterck ""
              foreach gp $generationpaths {
                set gp_start [ttv_GetTimingPathProperty $gp START_SIG]
                set start_event [ttv_GetTimingPathProperty $gp START_EVENT]
                if {$masterck==""} {
                  set masterck $start_event
                }
                set gp_end [ttv_GetTimingPathProperty $gp END_SIG]
                set end_event [ttv_GetTimingPathProperty $gp END_EVENT]
                set start_info [tmabs_FindClockInfo $allclockinfo $start_event $type]
                if {$start_info==""} {
                  set slope_axis [tmabs_GetConnectorAxis $fig "slope" [ttv_GetTimingSignalProperty $gp_start NAME] $cfg]
                  set start_latencies {}
                  foreach sv $slope_axis {
                    lappend start_latencies 0
                  }
                  lappend allclockinfo [list $start_event $type $slope_axis $start_latencies $masterck]
                  tmabs_Verbose 2 $cfg "  - master clock [ttv_GetTimingSignalProperty $gp_start NAME] / $event / $type"
                  tmabs_Verbose 2 $cfg "    slope axis: $slope_axis"
                } else {
                  set slope_axis [lindex $start_info 2]
                  set start_latencies [lindex $start_info 3]
                }
                set masterckname [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $masterck SIGNAL] NAME]
                set simmode [tmabs_CheckSimulationMode [ttv_GetTimingSignalProperty $gp_end NAME] [ttv_GetTimingSignalProperty $gp_start NAME] $cfg]
                set table [tmabs_CharacPathTables $bbox $fig $gp $slope_axis {-1} $simmode $cfg $masterckname ""]
                set output_slope_axis [tmabs_Table_Nx1_to_N [lindex $table 1]]
                set output_latencies [tmabs_Table_Nx1_to_N [lindex $table 0]]
                set output_latencies_total [tmabs_Table_N_Sum $start_latencies $output_latencies]
                lappend allclockinfo [list $end_event $type $output_slope_axis $output_latencies_total $masterck]
                tmabs_Verbose 2 $cfg "  - generated clock [ttv_GetTimingSignalProperty $gp_end NAME] / $event / $type from clock [ttv_GetTimingSignalProperty $gp_start NAME]"
                tmabs_Verbose 2 $cfg "    origin clock axis: $slope_axis"
                tmabs_Verbose 2 $cfg "    computed slope axis: $output_slope_axis"
                tmabs_Verbose 2 $cfg "    computed delay axis: $output_latencies"
                tmabs_Verbose 2 $cfg "    computed delay axis from master clock $masterckname : $output_latencies_total"
                tmabs_OutputDetail $cfg "**** $type generated clock path to clock [ttv_GetTimingSignalProperty $gp_end NAME] [ttv_GetTimingEventProperty $end_event TRANS]\n" $gp                
              }
            }
            ttv_FreePathList $generationpaths
         }
      }
    }
  }
  return $allclockinfo
}

#-- retreives and filters the regex connector name list given
proc tmabs_FilterConnector {fig lst dir type} {
 set result {}
 foreach sig $lst {
   set siglst [ttv_GetMatchingSignal $fig $sig connector]
   foreach lsig $siglst {
     if {[ttv_GetConnectorDirection $lsig]=="t" || ($dir=="input" && [ttv_GetConnectorDirection $lsig]=="i") || ($dir=="output" && [ttv_GetConnectorDirection $lsig]!="i")} {
       set connector_name [ttv_GetTimingSignalProperty $lsig NAME]
       if {[tmabs_lsearch $result $connector_name]==-1 && ($type!="clock" || [ttv_GetTimingSignalProperty $lsig IS_CLOCK]=="yes")} {
         lappend result [list "$connector_name" $lsig]
       }
     }
   }
 }
 if {$type=="clock"} {
   foreach sig $lst {
     set siglst [ttv_GetMatchingSignal $fig $sig "breakpoint latch"]
     foreach lsig $siglst {
       set name [ttv_GetTimingSignalProperty $lsig NAME]
       if {[tmabs_lsearch $result $name]==-1 && [ttv_GetTimingSignalProperty $lsig IS_CLOCK]=="yes"} {
           lappend result [list "$name" $lsig]
       }
     }
   }
 }
 tmabs_addcorresp $result
 return $result
}
proc tmabs_FilterInternal {fig lst clockyesno} {
 set result {}
 foreach sig $lst {
   set siglst [ttv_GetMatchingSignal $fig $sig "breakpoint latch precharge"]
   foreach lsig $siglst {
     set name [ttv_GetTimingSignalProperty $lsig NAME]
     if {[tmabs_lsearch $result $name]==-1 && ($clockyesno=="any" || [ttv_GetTimingSignalProperty $lsig IS_CLOCK]==$clockyesno)} {
       lappend result [list "$name" $lsig]
     }
   }
 }
 tmabs_addcorresp $result
 return $result
}

#-- compare 2 paths
proc tmabs_ComparePaths {a b} {
  if {$a==""} {
    return 1
  }
  if {$b==""} {
    return -1
  }
  set a0_hz [ttv_GetTimingPathProperty $a IS_HZ]
  set b0_hz [ttv_GetTimingPathProperty $b IS_HZ]

  set a0_ssig [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $a END_SIG] NAME]
  set b0_ssig [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $b END_SIG] NAME]

  set a0_str [ttv_GetTimingPathProperty $a END_TRANS]
  set b0_str [ttv_GetTimingPathProperty $b END_TRANS]

  set a0_esig [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $a START_SIG] NAME]
  set b0_esig [ttv_GetTimingSignalProperty [ttv_GetTimingPathProperty $b START_SIG] NAME]

  set a0_etr [ttv_GetTimingPathProperty $a START_TRANS]
  set b0_etr [ttv_GetTimingPathProperty $b START_TRANS]

  set a0pth "$a0_hz $a0_ssig $a0_str $a0_esig $a0_etr"
  set b0pth "$b0_hz $b0_ssig $b0_str $b0_esig $b0_etr"

  set res [string compare $a0pth $b0pth]
  return $res
}

#-- computes table delay and slope values for a set of input slopes and output capacitance
#-- + adds path margin contributions to the delays
proc tmabs_CreatePathTable {bbox fig slopes capas path simmode cfg masterclock latch} {

  global tmabs_IGNOREMARGINS

  set table [tmabs_CharacPathTables $bbox $fig $path $slopes $capas $simmode $cfg $masterclock $latch]

  if {[lindex $cfg $tmabs_IGNOREMARGINS]==0} {
  #-- adding path margin effect to table
    set mp_factor [ttv_GetTimingPathProperty $path PATH_MARGIN_FACTOR]
    set mp_delta [ttv_GetTimingPathProperty $path PATH_MARGIN_DELTA]
    set delay_table [lindex $table 0]
    if {$mp_factor!=1 || $mp_delta!=0} {
      set newlist {}
      for {set j 0} {$j<[llength $slopes]} {incr j} {
        set sublist [lindex $delay_table $j]
        set newsublist {}
        for {set i 0} {$i<[llength $capas]} {incr i} {
          set newval [expr [lindex $sublist $i]*$mp_factor+$mp_delta]
          lappend newsublist $newval
        }
        lappend newlist $newsublist
      }
      set table [list $newlist [lindex $table 1] [lindex $table 2] [lindex $table 3] [lindex $table 4]]
    }
  }
  return $table
}

#-- translate capacitance values in case of tmaDriveCapaout=yes
proc tmabs_TranslateCapacitances {output_signal output_capa_axis} {
  set internal_capacitance 0
  if {[avt_GetConfig tmaDriveCapaout]=="yes"} {
    set internal_capacitance [ttv_GetTimingSignalProperty $output_signal CAPA]
  }
  set new_capa_axis {}
  foreach capa $output_capa_axis {
    set newval [expr $capa-$internal_capacitance]
    if {$newval<0} {
      set newval 0
    }
    lappend new_capa_axis $newval
  }
  return $new_capa_axis
}

#-- creates a table model name for a path
proc tmabs_BuildModelName {path pref type} {
  set inputsig [ttv_GetTimingPathProperty $path START_SIG]
  set inputdir [ttv_GetTimingPathProperty $path START_TRANS]
  set input [ttv_GetTimingSignalProperty $inputsig NAME]
  set outputsig [ttv_GetTimingPathProperty $path END_SIG]
  set outputdir [ttv_GetTimingPathProperty $path END_TRANS]
  set output [ttv_GetTimingSignalProperty $outputsig NAME]
  set mname "m_$pref$input+$output+$inputdir$outputdir$type"
  return $mname
}

proc tmabs_GetAsyncList {fig} {
  set connectorlist [ttv_GetMatchingSignal $fig * connector]
  set asynclist {}
  foreach conn $connectorlist {
    if {[ttv_GetTimingSignalProperty $conn IS_ASYNCHRONOUS]=="yes"} {
      lappend asynclist $conn
    }
  }
  return $asynclist
}

proc tmabs_GetCriticPathOrAccess {fig inputs output path_or_access type allclockinfo findasync inputsS} {
  set asyncmode ""
  if {$inputs!={}} {
    if {$path_or_access=="path"} {
      set paths [ttv_GetPaths -from $inputsS -to [lindex $output 1] -$path_or_access -$type]
    } else {
      if {$findasync==0} {
        set tobeclock [ttv_GetClockList $fig]
      } else {
        set tobeclock [tmabs_GetAsyncList $fig]
        if {$findasync==2} {
          set asyncmode "-findasync"
        }
      }
      set paths {}
      if {[llength $tobeclock]>0} {
        foreach cks $tobeclock {
          set ckname [ttv_GetTimingSignalProperty $cks NAME]
          set ck_info [tmabs_FindClockInfo $allclockinfo [ttv_GetTimingSignalProperty $cks EVENT_UP] $type]
          if {$ck_info!=""} {
            set ck_info [tmabs_FindClockInfo $allclockinfo [ttv_GetTimingSignalProperty $cks EVENT_DOWN] $type]
          }
          if {$ck_info==""} {
            set checkname $ckname
          } else {
            set cksig [ttv_GetTimingEventProperty [lindex $ck_info 4] SIGNAL]
            if {[string first "connector" [ttv_GetTimingSignalProperty $cksig TYPE]]==-1} {
              set checkname ":)"
            } else {
              set checkname [ttv_GetTimingSignalProperty $cksig NAME]
            }
          }
          if {[tmabs_lsearch $inputs $checkname]!=-1} { 
            set access_r [ttv_GetPaths -from $cks -to [lindex $output 1] -dir ?r/ -nb 1 -$path_or_access -$type $asyncmode]
            lappend paths $access_r
            set access_f [ttv_GetPaths -from $cks -to [lindex $output 1] -dir ?f/ -nb 1 -$path_or_access -$type $asyncmode]
            lappend paths $access_f
          }
        }
      }
    }
    set paths [lsort -command tmabs_ComparePaths $paths]
    return $paths
  }
  return {}
}

#-- ----------------------------------------------------------------------------------------
#-- adds data paths (accesses or paths)
proc tmabs_AddDataPath {bbox fig inputs outputs path_or_access type allclockinfo cfg} {

 set findasync 0
 set searchtype $path_or_access
 
 if {$path_or_access=="async_m1"} {
   set findasync 1
   set searchtype "access"
 } elseif {$path_or_access=="async_m2"} {
   set findasync 2
   set searchtype "access"
 }

 set inputsS {}
 foreach input $inputs {
   lappend inputsS [lindex $input 1]
 }

 foreach outputL $outputs {
   set output [lindex $outputL 0]

   set output_capa_axis [tmabs_GetConnectorAxis $fig "capacitance" $output $cfg]

#-- retreives max paths
   if {$type=="both" || $type=="max"} {
     set paths_max [tmabs_GetCriticPathOrAccess $fig $inputs $outputL $searchtype max $allclockinfo $findasync $inputsS]
   } else {
     set paths_max {}
   }

#-- retreives min paths
   if {$type=="both" || $type=="min"} {
     set paths_min [tmabs_GetCriticPathOrAccess $fig $inputs $outputL $searchtype min $allclockinfo $findasync $inputsS]
   } else {
     set paths_min {}
   }

   if {$searchtype=="path"} {
     set mtype "Combi"
   } else {
     set mtype "Access"
   }
   set imax 0
   set imin 0

#-- runs thru min and max paths & computes matching path min and max path tables
   while {[lindex $paths_max $imax]!="" && [lindex $paths_min $imin]!=""} {
     set pmax [lindex $paths_max $imax]
     set pmin [lindex $paths_min $imin]
     set res [tmabs_ComparePaths $pmax $pmin]
 
#-- max timing tables --------------------
     if {$res==-1 || $res==0} {
#-- creates max delay tables
        set hzmode [ttv_GetTimingPathProperty $pmax IS_HZ]
        set inputsig [ttv_GetTimingPathProperty $pmax START_SIG]
        set outputsig [ttv_GetTimingPathProperty $pmax END_SIG]
        set input [ttv_GetTimingSignalProperty $inputsig NAME]
        set translated_output_capa_axis [tmabs_TranslateCapacitances $outputsig $output_capa_axis]

#-- retreiving propagated slopes from master clocks in case of generated clocks
        set ck_info [tmabs_FindClockInfo $allclockinfo [ttv_GetTimingPathProperty $pmax START_EVENT] "max"]
        if {$ck_info==""} {
          set mc_input_slope_axis [tmabs_GetConnectorAxis $fig "slope" $input $cfg]
          set input_slope_axis $mc_input_slope_axis
          set mc_name $input 
          set mc_pref ""
          set start_dir [ttv_GetTimingPathProperty $pmax START_TRANS]
        } else {
          set mc [lindex $ck_info 4]
          set mc_ck_info [tmabs_FindClockInfo $allclockinfo $mc "max"]
          set mc_input_slope_axis [lindex $mc_ck_info 2]
          set input_slope_axis [lindex $ck_info 2]
          set mc_name [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $mc SIGNAL] NAME]
          set mc_pref "$mc_name'_"
          set start_dir [ttv_GetTimingEventProperty $mc TRANS]
        }
        set end_dir [ttv_GetTimingPathProperty $pmax END_TRANS]

        set simmode [tmabs_CheckSimulationMode $output $mc_name $cfg]
        set maxtable [tmabs_CreatePathTable $bbox $fig $input_slope_axis $translated_output_capa_axis $pmax $simmode $cfg $mc_name ""]
        
        tmabs_OutputDetail $cfg "**** $mtype Max related_pin($mc_name) $start_dir to pin($output) $end_dir\n" $pmax

#-- adding generated clock latencies to delay table
        if {$ck_info!=""} {
          set latenced_table [tmabs_Table_NxM_N_Sum [lindex $maxtable 0] [lindex $ck_info 3]]
          set maxtable [list $latenced_table [lindex $maxtable 1] [lindex $maxtable 2]]
        }
        
        set maxmodelname_delay [tmabs_BuildModelName $pmax $mc_pref "__$mtype+Delay+Max"]
        ttv_CreateTimingTableModel $bbox $maxmodelname_delay $mc_input_slope_axis $output_capa_axis [lindex $maxtable 0] "slope-capa"
        if {[lindex $maxtable 2] != {}} {
          ttv_CreateEnergyTableModel $bbox $maxmodelname_delay $mc_input_slope_axis $output_capa_axis [lindex $maxtable 2] "slope-capa"
        }
        if {$hzmode=="no"} {
          set maxmodelname_slope [tmabs_BuildModelName $pmax $mc_pref "__$mtype+Slope+Max"]
          ttv_CreateTimingTableModel $bbox $maxmodelname_slope $mc_input_slope_axis $output_capa_axis [lindex $maxtable 1] "slope-capa"
        } else {
          set maxmodelname_slope ""
        }
        incr imax
     }

#-- min timing tables --------------------
     if {$res==1 || $res==0} {
#-- creates min delay tables
        set hzmode [ttv_GetTimingPathProperty $pmax IS_HZ]
        set inputsig [ttv_GetTimingPathProperty $pmin START_SIG]
        set outputsig [ttv_GetTimingPathProperty $pmin END_SIG]
        set input [ttv_GetTimingSignalProperty $inputsig NAME]
        set translated_output_capa_axis [tmabs_TranslateCapacitances $outputsig $output_capa_axis]

#-- retreiving propagated slopes from master clocks in case of generated clocks
        set ck_info [tmabs_FindClockInfo $allclockinfo [ttv_GetTimingPathProperty $pmin START_EVENT] "min"]
        if {$ck_info==""} {
          set mc_input_slope_axis [tmabs_GetConnectorAxis $fig "slope" $input $cfg]
          set input_slope_axis $mc_input_slope_axis
          set mc_name $input 
          set mc_pref ""
          set start_dir [ttv_GetTimingPathProperty $pmax START_TRANS]
        } else {
          set mc [lindex $ck_info 4]
          set mc_ck_info [tmabs_FindClockInfo $allclockinfo $mc "min"]
          set mc_input_slope_axis [lindex $mc_ck_info 2]
          set input_slope_axis [lindex $ck_info 2]
          set mc_name [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $mc SIGNAL] NAME]
          set mc_pref "$mc_name'_"
          set start_dir [ttv_GetTimingEventProperty $mc TRANS]
        }

        set end_dir [ttv_GetTimingPathProperty $pmin END_TRANS]
        set simmode [tmabs_CheckSimulationMode $input $mc_name $cfg]
        set mintable [tmabs_CreatePathTable $bbox $fig $input_slope_axis $translated_output_capa_axis $pmin $simmode $cfg $mc_name ""]

        tmabs_OutputDetail $cfg "**** $mtype Min related_pin($mc_name) $start_dir to pin($output) $end_dir\n" $pmin
        
#-- adding generated clock latencies to delay table
        if {$ck_info!=""} {
          set latenced_table [tmabs_Table_NxM_N_Sum [lindex $mintable 0] [lindex $ck_info 3]]
          set mintable [list $latenced_table [lindex $mintable 1] [lindex $mintable 2]]
        }

        set minmodelname_delay [tmabs_BuildModelName $pmin $mc_pref "__$mtype+Delay+Min"]
        ttv_CreateTimingTableModel $bbox $minmodelname_delay $mc_input_slope_axis $output_capa_axis [lindex $mintable 0] "slope-capa"
        if {$hzmode=="no"} {
          set minmodelname_slope [tmabs_BuildModelName $pmin $mc_pref "__$mtype+Slope+Min"]
          ttv_CreateTimingTableModel $bbox $minmodelname_slope $mc_input_slope_axis $output_capa_axis [lindex $mintable 1] "slope-capa"
        } else {
          set minmodelname_slope ""
        }
        incr imin
     }
     
#-- sets min to max or max to min if one of them does not exist
     if {$res==-1} {
       set pmin $pmax
       set minmodelname_delay $maxmodelname_delay
       set minmodelname_slope $maxmodelname_slope
     } 
     if {$res==1} {
       set pmax $pmin
       set maxmodelname_delay $minmodelname_delay
       set maxmodelname_slope $minmodelname_slope
     }

#-- creates timing lines
     set dir "$start_dir$end_dir"

     set nom_slope_min [ttv_GetTimingPathProperty $pmin SLOPE]
     set nom_slope_max [ttv_GetTimingPathProperty $pmax SLOPE]
     set nom_delay_min [ttv_GetTimingPathProperty $pmin DELAY]
     set nom_delay_max [ttv_GetTimingPathProperty $pmax DELAY]

     if {$path_or_access=="path"} {
       if {$hzmode=="no"} {
         set tl [ttv_AddTiming $mc_name $output $nom_delay_max $nom_slope_max $nom_delay_min $nom_slope_min $dir]
       } else {
         set tl [ttv_AddHZTiming $mc_name $output $nom_delay_max $nom_delay_min $dir]
       }
     } else {
       if {$hzmode=="no"} {
         set tl [ttv_AddAccess $mc_name $output $nom_delay_max $nom_slope_max $nom_delay_min $nom_slope_min $dir]
       } else {
         set tl [ttv_AddHZAccess $mc_name $output $nom_delay_max $nom_delay_min $dir]
       }
     }
     
#-- associates tables to created lines
     ttv_SetLineModel $tl $minmodelname_delay "delay min"
     ttv_SetLineModel $tl $maxmodelname_delay "delay max"
     if {$hzmode=="no"} {
       ttv_SetLineModel $tl $minmodelname_slope "slope min"
       ttv_SetLineModel $tl $maxmodelname_slope "slope max"
     }
   }
   ttv_FreePathList $paths_max
   ttv_FreePathList $paths_min
 }
}

proc tmabs_AddCombinatorial {bbox fig inputs outputs type cfg} {

  tmabs_Verbose 1 $cfg "- **** COMBINATORIAL PATHS *****"
  set inputs [tmabs_FilterConnector $fig $inputs "input" ""]
  set outputs [tmabs_FilterConnector $fig $outputs "output" ""]
  
  tmabs_AddDataPath $bbox $fig $inputs $outputs path $type {} $cfg
}

proc tmabs_AddCombinatorialInternal {bbox fig inputs outputs type cfg} {

  tmabs_Verbose 1 $cfg "- **** COMBINATORIAL PATHS FOR INTERNAL PINS *****"
  set inputs [tmabs_FilterInternal $fig $inputs any]
  foreach i $inputs {
    ttv_AddBreakpoint $i
  }
  set outputs [tmabs_FilterConnector $fig $outputs "output" ""]
  
  tmabs_AddDataPath $bbox $fig $inputs $outputs path $type {} $cfg
}

proc tmabs_AddAccesses {bbox fig clocks outputs type allclockinfo cfg} {

  tmabs_Verbose 1 $cfg "- **** ACCESS PATHS *****"
  set clocks [tmabs_FilterConnector $fig $clocks "input" "clock"]
  set outputs [tmabs_FilterConnector $fig $outputs "output" ""]
  
  tmabs_AddDataPath $bbox $fig $clocks $outputs access $type $allclockinfo $cfg
}

proc tmabs_AddAsynchronousAccesses {bbox fig inputs outputs type cfg} {

  tmabs_Verbose 1 $cfg "- **** ASYNCHRONOUS ACCESS PATHS *****"
  set inputs [tmabs_FilterConnector $fig $inputs "input" ""]
  set outputs [tmabs_FilterConnector $fig $outputs "output" ""]
  
  tmabs_AddDataPath $bbox $fig $inputs $outputs async_m1 $type {} $cfg
  tmabs_AddDataPath $bbox $fig $inputs $outputs async_m2 $type {} $cfg
}

#-- compare 2 constraints
proc tmabs_CompareConstraints {a b cmpdelay} {
  if {$a==""} {
    return 1
  }
  if {$b==""} {
    return -1
  }
  set typea [ttv_GetTimingConstraintProperty $a TYPE]
  set typeb [ttv_GetTimingConstraintProperty $b TYPE]
  if {$typea<$typeb} {
    return -1
  } elseif {$typea>$typeb} {
    return 1
  }
  set dpa [ttv_GetTimingConstraintProperty $a DATA_PATH]
  set cpa [ttv_GetTimingConstraintProperty $a CLOCK_PATH]
  set dpb [ttv_GetTimingConstraintProperty $b DATA_PATH]
  set cpb [ttv_GetTimingConstraintProperty $b CLOCK_PATH]
  
  set a0 [ttv_GetTimingPathProperty $dpa START_SIG]
  set b0 [ttv_GetTimingPathProperty $dpb START_SIG]
  if {$a0 < $b0} {
      return -1
  } elseif {$a0 > $b0} {
      return 1
  }
  set a0 [ttv_GetTimingPathProperty $dpa START_TRANS]
  set b0 [ttv_GetTimingPathProperty $dpb START_TRANS]
  if {$a0 < $b0} {
      return -1
  } elseif {$a0 > $b0} {
      return 1
  }

  set a0 [ttv_GetTimingConstraintProperty $a MASTER_CLOCK]
  set b0 [ttv_GetTimingConstraintProperty $b MASTER_CLOCK]
  
#  set a0 [ttv_GetTimingPathProperty $cpa START_SIG]
#  set b0 [ttv_GetTimingPathProperty $cpb START_SIG]
  if {$a0 < $b0} {
      return -1
  } elseif {$a0 > $b0} {
      return 1
  }

  if {$cmpdelay=="yes"} {
    set a0 [ttv_GetTimingConstraintProperty $a VALUE]
    set b0 [ttv_GetTimingConstraintProperty $b VALUE]
    if {$a0 > $b0} {
        return -1
    } elseif {$a0 < $b0} {
        return 1
    }
  }
  return 0
}

#-- compare 2 constraints and their value
proc tmabs_CompareConstraintsPlusValue {a b} {
  return [tmabs_CompareConstraints $a $b yes]
}

#-- builds a 2-dimension-table using the intrinsic model
proc tmabs_Computation_Intrinsic_From_Model {fig input_slope clock_slope modelname} {
  set input_slope1D [tmabs_Table_Nx1_to_N $input_slope]
  set clock_slope1D [tmabs_Table_Nx1_to_N $clock_slope]
  return [ttv_CharacTimingLineModel $fig $modelname $input_slope1D $clock_slope1D "slope-slope"]
}

#-- builds a 2-dimension-table from 2 1-dimension_table
proc tmabs_AddBuildConstraintTable {fig dp_table cp_table constraint cfg} {
  set dp_delay_table [lindex $dp_table 0]
  set cp_delay_table [lindex $cp_table 0]
  set dvttab [lindex $cp_table 3]
  set dlooptab [lindex $dp_table 4]

  set hitas_margin [ttv_GetTimingConstraintProperty $constraint INTRINSIC_MARGIN]
  set hitas_margin_model [ttv_GetTimingConstraintProperty $constraint INTRINSIC_MARGIN_MODEL]
  set type [ttv_GetTimingConstraintProperty $constraint TYPE]
  set sx_s [llength $dp_delay_table]
  set sy_s [llength $cp_delay_table]
  if {$hitas_margin_model=="NULL" || $dvttab!=""} {
    set intrtab [tmabs_Computation_Intrinsic $type $dvttab $dlooptab $sx_s $sy_s $hitas_margin $cfg]
  } else {
    set intrtab [tmabs_Computation_Intrinsic_From_Model $fig [lindex $dp_table 1] [lindex $cp_table 1] $hitas_margin_model]
  }
  set newtable {}
  for {set sx 0} {$sx<$sx_s} {incr sx} {
    set d_val [lindex [lindex $dp_delay_table $sx] 0] 
    set subtable {}
    for {set sy 0} {$sy<$sy_s} {incr sy} {
      set c_val [lindex [lindex $cp_delay_table $sy] 0]
      set margin [lindex [lindex $intrtab $sx] $sy]
      set value [expr $d_val+$margin-$c_val]
      if {$type=="hold"} {
        set value [expr -$value]
      }
      lappend subtable $value
    }
    lappend newtable $subtable
  }
  return $newtable
}

#-- adds a setup or hold line from a constraint to the blackbox figure
proc tmabs_AddThisConstraint {bbox fig constraint allclockinfo cfg} {
  set cp [ttv_GetTimingConstraintProperty $constraint CLOCK_PATH]
  set dp [ttv_GetTimingConstraintProperty $constraint DATA_PATH]
  set type [ttv_GetTimingConstraintProperty $constraint TYPE]

#-- creates datapath 1D constraint table
  set dp_end [ttv_GetTimingPathProperty $dp END_SIG]
  set dp_end_ev [ttv_GetTimingPathProperty $dp END_EVENT]
  set dp_start [ttv_GetTimingPathProperty $dp START_SIG]
  set dp_start_dir [ttv_GetTimingPathProperty $dp START_TRANS]
  set dp_end_name [ttv_GetTimingSignalProperty $dp_end NAME]
  set dp_start_name [ttv_GetTimingSignalProperty $dp_start NAME]
  set output_capa_axis [tmabs_GetConnectorAxis $fig "capacitance" $dp_end_name $cfg]
  if {([llength $output_capa_axis]!=1 || [lindex $output_capa_axis 0]!=-1) && [ttv_GetTimingPathProperty $dp DELAY]!=0} {
    puts "Warning: Constraint End Datapath Signal '$dp_end_name' at the interface, capacitance axis ignored"
  }
  set output_capa_axis {-1}
  set dp_input_slope_axis [tmabs_GetConnectorAxis $fig "slope" $dp_start_name $cfg]
  set mc [ttv_GetTimingConstraintProperty $constraint MASTER_CLOCK]
  set mc_name [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $mc SIGNAL] NAME]

  set simmode [tmabs_CheckSimulationMode $dp_start_name $mc_name $cfg]
  set dp_table [tmabs_CreatePathTable $bbox $fig $dp_input_slope_axis $output_capa_axis $dp $simmode $cfg "" ""]

#-- creates clockpath 1D constraint table
  set cp_end [ttv_GetTimingPathProperty $cp END_SIG]
  set cp_start [ttv_GetTimingPathProperty $cp START_SIG]
  set cp_start_dir [ttv_GetTimingPathProperty $cp START_TRANS]
  set cp_end_name [ttv_GetTimingSignalProperty $cp_end NAME]
  set cp_start_name [ttv_GetTimingSignalProperty $cp_start NAME]
  set output_capa_axis [tmabs_GetConnectorAxis $fig "capacitance" $cp_end_name $cfg]
  if {([llength $output_capa_axis]!=1 || [lindex $output_capa_axis 0]!=-1) && [ttv_GetTimingPathProperty $cp DELAY]!=0} {
    puts "Warning: Constraint End Clockpath Signal '$cp_end_name' at the interface, capacitance axis ignored"
  }
  set output_capa_axis {-1}

#-- retreiving propagated slopes from master clocks in case of generated clocks
  set cp_start_event [ttv_GetTimingPathProperty $cp START_EVENT]

  if {$type=="setup"} {
    set minmax "min"
  } else {
    set minmax "max"
  }
  
  set ck_info [tmabs_FindClockInfo $allclockinfo $cp_start_event $minmax]
  set mc_ck_info [tmabs_FindClockInfo $allclockinfo $mc $minmax]

  if {$ck_info==""} {
    set cp_input_slope_axis [tmabs_GetConnectorAxis $fig "slope" $cp_start_name $cfg]
    set mc_input_slope_axis [tmabs_GetConnectorAxis $fig "slope" $mc_name $cfg]
    set mc_pref ""
  } else {
    set cp_input_slope_axis [lindex $ck_info 2]
    set mc_input_slope_axis [lindex $mc_ck_info 2]
    set mc_pref "$mc_name'_"
    set cp_start_dir [ttv_GetTimingEventProperty $mc TRANS]
#    puts $cp_input_slope_axis
  }

  if {[string first "latch" [ttv_GetTimingSignalProperty $dp_end TYPE]]!=-1} { 
    ttv_Simulate_AddDelayToVT $fig $cp [ttv_GetTimingPathProperty $dp END_EVENT]
  }
  set cp_table [tmabs_CreatePathTable $bbox $fig $cp_input_slope_axis $output_capa_axis $cp $simmode $cfg $mc_name $dp_end_ev]

#-- adding generated clock latencies to delay table
  if {$ck_info!=""} {
    set latenced_table [tmabs_Table_NxM_N_Sum [lindex $cp_table 0] [lindex $ck_info 3]]
    set cp_table [list $latenced_table [lindex $cp_table 1] [lindex $cp_table 2] [lindex $cp_table 3] [lindex $cp_table 4]]
  }

#-- creates 2D constraint table
  set table [tmabs_AddBuildConstraintTable $fig $dp_table $cp_table $constraint $cfg]

#-- build table model
  set dp_name [tmabs_BuildModelName $dp "" ""]
  set cp_name [tmabs_BuildModelName $cp $mc_pref ""]
  if {$type=="setup"} {
    set modelname "$dp_name\__$cp_name\__Setup"
  } else {
    set modelname "$dp_name\__$cp_name\__Hold"
  }

  set dir "$dp_start_dir$cp_start_dir"
  ttv_CreateTimingTableModel $bbox $modelname $dp_input_slope_axis $mc_input_slope_axis $table "slope-ckslope"
  
  tmabs_OutputDetail $cfg "**** $type related_pin($mc_name) $cp_start_dir to pin($dp_start_name) $dp_start_dir\n  CLOCKPATH:\n" $cp
  tmabs_OutputDetail $cfg "  DATAPATH:\n" $dp
        
#-- creates constraint line
  set nomvalue [ttv_GetTimingConstraintProperty $constraint VALUE]
  if {$type=="setup"} {
    set tl [ttv_AddSetup $dp_start_name $mc_name $nomvalue $dir]
  } else {
    set tl [ttv_AddHold $dp_start_name $mc_name $nomvalue $dir]
  }
#-- associates table to created lines
  ttv_SetLineModel $tl $modelname "delay max"
  ttv_SetLineModel $tl $modelname "delay min"
}

#-- ----------------------------------------------------------------------------------------
#-- adds data paths (accesses or paths)
proc tmabs_AddConstraints {bbox fig inputs clocks type allclockinfo cfg} {

  tmabs_Verbose 1 $cfg "- **** CONSTRAINTS *****"
  set clocks [tmabs_FilterConnector $fig $clocks "input" "clock"]
  if {[llength $clocks]>0} {
    set inputs [tmabs_FilterConnector $fig $inputs "input" ""]

    foreach inputL $inputs {
      set input [lindex $inputL 0]
      tmabs_Verbose 1 $cfg "- Computing constraints for pin '$input'"
      set constraints [ttv_GetConstraints $fig $input $type]
      tmabs_Verbose 2 $cfg "  total constaints = [llength $constraints]"
      set constraints [lsort -command tmabs_CompareConstraintsPlusValue $constraints]
      set prevconstraint ""
      foreach constraint $constraints {
        if {[tmabs_CompareConstraints $prevconstraint $constraint "no"]!=0} {
           set mc [ttv_GetTimingConstraintProperty $constraint MASTER_CLOCK]
           set thisclock [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $mc SIGNAL] NAME]
           if {[tmabs_lsearch $clocks $thisclock]!=-1} {
              tmabs_AddThisConstraint $bbox $fig $constraint $allclockinfo $cfg
           } else {
              tmabs_Verbose 2 $cfg "  skipping unwanted clock '$thisclock'"
           }
        }
        set prevconstraint $constraint
      }
      ttv_FreeConstraints $constraints -fast
    }
  } else {
    tmabs_Verbose 1 $cfg "  - no defined clocks, constraints ignored"
  }
}

proc tmabs_CreateBlackboxInterface {fig bef clocks inputs outputs} {
  set figname [ttv_GetTimingFigureProperty $fig FIGNAME]
  
  set allcon $clocks
  lappend allcon $inputs
  lappend allcon $inputs
  lappend allcon $outputs

  set bbox [tma_DuplicateInterface $fig "$figname'bbox" $allcon] 

  return $bbox
}

proc tmabs_ReadConfig {lst i} {
  global tcl_interactive
  set simmode 0
  set maxsim 1
  set simlist {}
  set scaleval {0.25 0.5 1 2 4}
  set verbose 0
  set minmax both
  set cstmode all
  set cachemode 0
  set dumpdtx 0
  set exitonerror 0
  set ignoremargins 0
  set detectfp 0
  set detailfile ""
  set clocks "*"
  set inputs "*"
  set outputs "*"
  set internals {}
  set bef [_NULL_]
  set addpins {}
  
  if {$i==0} {
    set newmode 1
  } else {
    set newmode 0
  }

  while {[lindex $lst $i]!=""} {
    set opt [lindex $lst $i]
    incr i
    if {$opt=="-simulate"} {
      set val [lindex $lst $i]
      incr i
      set simlist $val
    } elseif {$opt=="-maxsim"} {
      set val [lindex $lst $i]
      incr i
      set maxsim $val
    } elseif {$opt=="-scalevalues"} {
      set val [lindex $lst $i]
      incr i
      set scaleval $val
    } elseif {$opt=="-detailfile"} {
      set val [lindex $lst $i]
      incr i
      set detailfile $val
    } elseif {$opt=="-verbose"} {
      set verbose 1
    } elseif {$opt=="-debug"} {
      set verbose 2
    } elseif {$opt=="-minonly"} {
      set minmax min
    } elseif {$opt=="-maxonly"} {
      set minmax max
    } elseif {$opt=="-setuponly"} {
      set cstmode setup
    } elseif {$opt=="-holdonly"} {
      set cstmode hold
    } elseif {$opt=="-enablecache"} {
      set cachemode 1
    } elseif {$opt=="-dumpdtx"} {
      set dumpdtx 1
    } elseif {$opt=="-exitonerror"} {
      set exitonerror 1
    } elseif {$opt=="-ignoremargins"} {
      set ignoremargins 1
    } elseif {$opt=="-detectfalsepath"} {
      set detectfp 1
    } elseif {$newmode==1 && $opt=="-inputs"} {
      set inputs [lindex $lst $i]
      incr i
    } elseif {$newmode==1 && $opt=="-outputs"} {
      set outputs [lindex $lst $i]
      incr i
    } elseif {$newmode==1 && $opt=="-addpins"} {
      set addpins [lindex $lst $i]
      incr i
    } elseif {$newmode==1 && $opt=="-internal"} {
      set internals [lindex $lst $i]
      incr i
    } elseif {$newmode==1 && $opt=="-clocks"} {
      set clocks [lindex $lst $i]
      incr i
    } elseif {$newmode==1 && $opt=="-behaviour"} {
      set bef [lindex $lst $i]
      incr i
    } else {
      set prei [expr $i-1]
      puts "\[tmabs\] unknown option '[lindex $lst $prei]'"
      puts "\[tmabs\] usage: tmabs fig bef clocks inputs outputs \[-simulate <list>] \[-maxsim <#>] \[-scalevalues <list>] \[-verbose]"
      puts "\[tmabs\]        \[-minonly|-maxonly] \[-setuponly|-holdonly] \[-enablecache] \[-exitonerror] \[-ignoremargins] \[-detectfalsepath]"
      puts "\[tmabs\] or     tmabs fig \[-behaviour <bef>] \[-clocks <clocklist>] \[-inputs <inputlist>] \[-outputs <outputlist>] \[-internal <internallist>]"
      puts "\[tmabs\]        \[-simulate <list>] \[-maxsim <#>] \[-scalevalues <list>] \[-verbose] \[-detailfile <filename>]"
      puts "\[tmabs\]        \[-minonly|-maxonly] \[-setuponly|-holdonly] \[-enablecache] \[-exitonerror] \[-ignoremargins] \[-detectfalsepath]"
      puts "\[tmabs\]        \[-addpins <pinlist>]"
      if {!$tcl_interactive} {
        exit
      } else {
        return {}
      }
    } 
  }
  set cfg [list $simmode $maxsim $simlist $scaleval $verbose $minmax $cstmode $cachemode $dumpdtx $exitonerror $ignoremargins $detectfp $detailfile\
                $bef $clocks $inputs $outputs $internals $addpins]
  return $cfg
}

proc tmabs_ReadCache {bbox cfg} {

  global tmabs_CACHEMODE
  global tmabs_table_done

  set tmabs_table_done {}

  if {[lindex $cfg $tmabs_CACHEMODE]==1} {
    tmabs_Verbose 1 $cfg "- **** READING CACHE FILE *****"
    set cnt 0
    set fname [ttv_GetTimingFigureProperty $bbox NAME].simulations.cache
    if {[file exists $fname]==1} {
      set fl [open $fname r]
      set data [read $fl]
      close $fl
      set lines [split $data \n]
      foreach line $lines {
        set key [lindex $line 0]
        set data [lindex $line 1]
        lappend tmabs_table_done [list $key $data]
        incr cnt
      }
    }
    tmabs_Verbose 1 $cfg "  - $cnt entries read"
  }
}

#-- adds user pins to blackbox
proc tmabs_AddAdditionnalPins {cfg} {
  global tmabs_A_ADDITION_PINS
  foreach ap [lindex $cfg $tmabs_A_ADDITION_PINS] {
    set dir [lindex $ap 1]
    if {$dir==""} {
      set dir i
    }
    tmabs_Verbose 1 $cfg "  - adding pin '[lindex $ap 0]' direction=$dir"
    ttv_AddConnector [lindex $ap 0] $dir
  }
}

proc tmabs_AddInternalPowerTable {bbox fig connectors type allclockinfo cfg outputmode} {

 set lst {}
 foreach con $connectors {
   set sig [lindex $con 0]

   if {$outputmode==1} {
     set input_slope_axis {-1}
     set output_capa_axis [tmabs_GetConnectorAxis $fig "capacitance" $sig $cfg]
     set paths [ttv_GetPaths -to [lindex $con 1] -path -$type]
   } else {
     set input_slope_axis [tmabs_GetConnectorAxis $fig "slope" $sig $cfg]
     set output_capa_axis {-1}
     set paths [ttv_GetPaths -from [lindex $con 1] -path -$type]
   }

   set simmode 0

   foreach trans {"u" "d"} {
     set etable ""
     set cnt 0
     foreach p $paths {
       if {$outputmode==1} {
         set other [ttv_GetTimingPathProperty $p START_SIG]
         set other_trans [ttv_GetTimingPathProperty $p START_TRANS]
         set me [ttv_GetTimingPathProperty $p END_SIG]
         set me_trans [ttv_GetTimingPathProperty $p END_TRANS]

       } else {
         set other [ttv_GetTimingPathProperty $p END_SIG]
         set other_trans [ttv_GetTimingPathProperty $p END_TRANS]
         set me [ttv_GetTimingPathProperty $p START_SIG]
         set me_trans [ttv_GetTimingPathProperty $p START_TRANS]
       }
       set hzmode [ttv_GetTimingPathProperty $p IS_HZ]

       if {$hzmode=="no"} {
         set other_type [ttv_GetTimingSignalProperty $other TYPE]
         if {[string first "connector" $other_type]==-1} {
           if {$me_trans==$trans} {
             set table [lindex [tmabs_CreatePathTable $bbox $fig $input_slope_axis $output_capa_axis $p $simmode $cfg "" ""] 2]
             if {$table!={}} {
               if {$etable==""} {
                 set etable $table
               } else {
                 set etable [tmabs_Table_NxM_NxM_Sum $etable $table]
               }
               incr cnt
             }
           }
         }
       }
     }
   
     tmabs_Verbose 1 $cfg "  - Connector $sig\($trans) : $cnt paths"
     if {$etable!=""} {
       set emodelname "m_Energy+$sig+$trans+$type"
       ttv_CreateTimingTableModel $bbox $emodelname $input_slope_axis $output_capa_axis $etable "slope-capa"
       ttv_CreateEnergyTableModel $bbox $emodelname $input_slope_axis $output_capa_axis $etable "slope-capa"
       lappend lst [list $sig $trans $emodelname]
     }
   }
   ttv_FreePathList $paths
 }
 return $lst
}

proc tmabs_SetupEnergyModelNames {bbox lst} {
  foreach l $lst {
    set sig [lindex $l 0]
    set trans [lindex $l 1]
    set model [lindex $l 2]
    tma_SetEnergyInformation $bbox $sig $trans $model
  }
}

proc tmabs_AddInternalPowerInformation {bbox fig inputs outputs clocks type cfg} {

  set inputs [tmabs_FilterConnector $fig $inputs "input" ""]
  set outputs [tmabs_FilterConnector $fig $outputs "output" ""]
  set clocks [tmabs_FilterConnector $fig $clocks "input" "clock"]
  set lst {}

  tmabs_Verbose 1 $cfg "- **** INPUTS INTERNAL POWER *****"
  set lst [concat $lst [tmabs_AddInternalPowerTable $bbox $fig $inputs $type {} $cfg 0]]
  tmabs_Verbose 1 $cfg "- **** CLOCKS INTERNAL POWER *****"
  set lst [concat $lst [tmabs_AddInternalPowerTable $bbox $fig $clocks $type {} $cfg 0]]
#  tmabs_Verbose 1 $cfg "- **** OUTPUTS INTERNAL POWER *****"
#  set lst [concat $lst [tmabs_AddInternalPowerTable $bbox $fig $outputs $type {} $cfg 1]]
  return $lst
}


proc tmabs {fig args} {
  
  global tmabs_table_done
  global tmabs_MINMAX
  global tmabs_CSTMODE
  global tmabs_DUMPDTX
  global tmabs_DETECTFALSEPATH
  global tmabs_DETAILFILE
  global tmabs_A_BEH   
  global tmabs_A_CK    
  global tmabs_A_INPUT 
  global tmabs_A_OUTPUT
  global tmabs_A_INTERNAL
  global tmabs_quick_name_to_signal

  array set tmabs_quick_name_to_signal {}
  set argstart 0
  
  set oldPrecisionLevel [avt_GetConfig "avtPrecisionLevel"]
  avt_Config "avtPrecisionLevel" 1


  if {[string index [lindex $args 0] 0]!="-" && [lindex $args 3]!=""} {
    set bef [lindex $args 0]
    set clocks [lindex $args 1]
    set inputs [lindex $args 2]
    set outputs [lindex $args 3]
    set argstart 4
  }
  
  set cfg [tmabs_ReadConfig $args $argstart]
  if {$cfg=={}} {
    return [__NULL__]
  }
  if {$argstart==0} {
    set clocks [lindex $cfg $tmabs_A_CK]
    set inputs [lindex $cfg $tmabs_A_INPUT]
    set outputs [lindex $cfg $tmabs_A_OUTPUT]
    set bef [lindex $cfg $tmabs_A_BEH]
  }

  tma_DetectClocksFromBeh $fig $bef

  set bbox [tmabs_CreateBlackboxInterface $fig $bef $clocks $inputs $outputs]

  if {[lindex $cfg $tmabs_DETECTFALSEPATH]==1} {
    ttv_DetectFalseClockPath $fig
    ttv_DetectFalsePath $fig * *
#    inf_SetFigureName [ttv_GetTimingFigureProperty $fig NAME]
#    inf_Drive [ttv_GetTimingFigureProperty $fig NAME].t.inf
  }

  ttv_EditTimingFigure $bbox

  tmabs_ReadCache $bbox $cfg
  
  set filename [lindex $cfg $tmabs_DETAILFILE]
  if {$filename!=""} {
    file delete $filename
  }
  
  
  set clockinformation [tmabs_BuildClockConnectorInformations $bbox $fig $cfg]
  
  ttv_SetSearchMode usenodenameonly

#-- adds combinatorial lines
  tmabs_AddCombinatorial $bbox $fig $inputs $outputs [lindex $cfg $tmabs_MINMAX] $cfg

#-- adds access lines
  tmabs_AddAccesses $bbox $fig $clocks $outputs [lindex $cfg $tmabs_MINMAX] $clockinformation $cfg

#-- adds asynchronous access lines
  tmabs_AddAsynchronousAccesses $bbox $fig $clocks $outputs [lindex $cfg $tmabs_MINMAX] $cfg

#-- adds constraint lines
  tmabs_AddConstraints $bbox $fig $inputs $clocks [lindex $cfg $tmabs_CSTMODE] $clockinformation $cfg
  
#-- adds combinatorial lines
  set internals [lindex $cfg $tmabs_A_INTERNAL]
  tmabs_AddCombinatorialInternal $bbox $fig $internals $outputs [lindex $cfg $tmabs_MINMAX] $cfg

#-- adds internal power information
#  set powervar [string tolower [avt_GetConfig avtPowerCalculation]]
  set powervar [avt_GetConfig avtPowerCalculation]
#  puts $powervar
  if {$powervar==1 || $powervar==5} {
    set powerinfo [tmabs_AddInternalPowerInformation $bbox $fig $inputs $outputs $clocks max $cfg]
  } else {
    set powerinfo {}
  }

  ttv_SetSearchMode !usenodenameonly

  ttv_FreeConstraints {} -fullclean $fig

  tmabs_AddAdditionnalPins $cfg
  
  set finalbbox [ttv_FinishTimingFigure]
  ttv_SetFigureFlag $finalbbox "tmaDriveCapaout_Is_Handled"
  
  tmabs_AddGeneratedClocksInfo $finalbbox $fig $cfg
  tma_UpdateSetReset $finalbbox $bef
  tmabs_SetupEnergyModelNames $finalbbox $powerinfo
  
  if {[lindex $cfg $tmabs_DUMPDTX]==1} {
    ttv_DriveTimingFigure $finalbbox "[ttv_GetTimingFigureProperty $bbox NAME].dtx" dtx
  }
  avt_Config "avtPrecisionLevel" $oldPrecisionLevel
  
  array unset tmabs_quick_name_to_signal

  return $finalbbox
}

proc tma_abstract {fig bef} {
  return [tmabs $fig $bef * * *]
}
proc tma_Abstract {fig bef} {
  return [tma_abstract $fig $bef]
}


