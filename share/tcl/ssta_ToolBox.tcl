

#set ssta_MasterPath "."
set ssta_VerboseMode yes

proc ssta_Verbose {str} {
  
  global ssta_VerboseMode
  
  if {$ssta_VerboseMode=="yes"} {
    fputs "\[ssta\] $str\n" stdout
  }
}

# totalpathstat
# return a list of delays for the path corresponding to the monte carlo run
proc totalpathstat { path } {
  set details [ ttv_GetPathDetail $path ]
  foreach detail $details {
    set mclist [ ttv_GetTimingDetailProperty $detail MCDELAY ]

    if { [ llength $mclist ] > 0 } {
      if { [ info exist total ] } {
        if { [ info exist v ] } {
          unset v
        }
        for { set i 0 } { $i < [ llength $mclist ] } { incr i } {
          set t [ expr "[ lindex $mclist $i ] + [ lindex $total $i ]" ]
          if { [ info exist v ] } {
            set v "$v $t"
          } else {
            set v "$t"
          }
        }
      } else {
        set v $mclist
      }
      set total $v
    }
  }

  return $total
}

proc ssta_make_distr { lst nb step } {
  set min 1
  set max -1
  set all 0
  foreach l $lst {
    if {$l>$max} {
      set max $l
    }
    if {$l<$min} {
      set min $l
    }
    incr all
  }
  if {$nb!=0} {
    set step [expr ($max-$min)/$nb]
  }
  set newlst {}
  foreach l $lst {
    set il [expr int(($l-$min)/$step)]
    if {$nb!=0 && $il==$nb} {
      set il [expr $nb-1]
    }
    lappend newlst $il
  }
  set res [lsort -increasing -integer $newlst]
  set cnt 0
  set tot 0
  set prev ""
  set dist {}
  foreach l $res {
    if {$prev!=$l} {
      if {$cnt!=0} {
        set tot [expr $tot+$cnt]
        lappend dist [list [format %g [expr ($min+$prev*$step)]] [format %g [expr ($min+($prev+1)*$step)]] $cnt $tot [format %g [expr $tot*100/$all]]]
      }
      set cnt 0
      set prev $l
    }
    incr cnt
  }
  if {$cnt!=0} {
    set tot [expr $tot+$cnt]
    lappend dist [list [format %g [expr ($min+$prev*$step)]] [format %g [expr ($min+($prev+1)*$step)]] $cnt $tot [format %g [expr $tot*100/$all]]]
  }
  return $dist
}

proc ssta_make_gnuplot { dist filename title mean var} {
 set f [open "$filename.dat" w]
 set prevmax ""
 set prevprc 0
 set surf 0
 foreach sd $dist {
   set min [lindex $sd 0]
   set max [lindex $sd 1]
   set nb [lindex $sd 2]
   set prc [lindex $sd 4]
   if {$min!=$prevmax} {
     if {$prevmax==""} {
       set prevmax [expr $min-($max-$min)]
     }
     puts $f "[expr $prevmax+($max-$min)/2] 0 $prevprc"
   }
   set prevmax $max
   set prevprc $prc
   puts $f "[expr $min+($max-$min)/2] $nb $prc"
   set surf [expr $surf+$nb*($max-$min)]
 }
 puts $f "[expr $prevmax+($max-$min)/2] 0 $prc"
 close $f
 set f [open "$filename" w]
 puts $f "set title \"$title\""
 puts $f "set grid"
 puts $f "set size 1,0.5"
 puts $f "set origin 0,0"
 puts $f "set multiplot"
 puts $f "gauss(x,m,s)=($surf)/(sqrt(2.0*pi)*s)*exp(-(x-m)*(x-m)/(2.0*s*s))"
 puts $f "m=$mean"
 puts $f "s=sqrt($var)"
 puts $f "plot \"$filename.dat\" using 1:2 title \"Distribution\" with boxes fs solid 0.7, gauss(x,m,s) title \"Distribution Approximative Gaussian\""
 puts $f "set origin 0,0.5"
 puts $f "plot \"$filename.dat\" using 1:3 title \"Cumulative % of runs\" with linespoint"
 puts $f "set nomultiplot"
 puts $f "pause -1"
 close $f
}

proc ssta_get_field { filename pos } {

    set rpos [expr $pos-1]
    set file [ open $filename ]
    set data_list {}
    set line [ gets $file ]
    while { ! [ eof $file ] } {
      if {$line!=""} {
        set data [ lindex [ split $line ] $rpos ]
        lappend data_list $data
      }
      set line [ gets $file ]
    }
    return $data_list
}

proc ssta_GetEventName {event} {
  set signal  [ ttv_GetTimingEventProperty  $event "SIGNAL" ]
  set signame [ ttv_GetTimingSignalProperty $signal "NET_NAME" ]
  set trans [ ttv_GetTimingEventProperty $event TRANS ] 
  set eventname "$signame $trans"
  return $eventname 
}

proc ssta_parsedata { name process_func } {
  proc ssta_getbeginid { line } {
  
    set id -1
    set str_begin "-- begin "
    set len_begin [ string length $str_begin ]
  
    
    if { [ string compare -length $len_begin $line $str_begin ] == 0 } {
  
      set tidrun [ string range $line $len_begin end ]
      set pos [ string first " "  $tidrun ]
      set pos [ expr "$pos - 1" ]
      set id [ string range $tidrun 0 $pos ]
    } 
  
    return $id
  }
  proc ssta_getendid { line } {
  
    set id -1
    set str_end "-- end "
    set len_end [ string length $str_end ]
    
    if { [ string compare -length $len_end $line $str_end ] == 0 } {
  
      set tidrun [ string range $line $len_end end ]
      set pos [ string first " "  $tidrun ]
      set pos [ expr "$pos - 1" ]
      set id [ string range $tidrun 0 $pos ]
    } 
  
    return $id
  }

  set file [ open $name ]
  set alldata {}
  set linenum 0

  while { ! [ eof $file ] } {
  
    set line [ gets $file ]
    incr linenum
    set match 0
  
    set id [ ssta_getbeginid $line ]
    if { $id > 0 } {
      set beginid $id
      set match 1
    }
  
    set id [ ssta_getendid $line ]
    if { $id > 0 } {
      if { $id != $beginid } {
        puts "file $name, line $linenum: incorrect data file: mismatching start and end id !"
        exit 0
      }
      set data [list $id $parsedline]
      if {$process_func!=""} {
        $process_func $data
      } else {
        lappend alldata $data
      }
      set match 1
    }
  
    if { $match == 0 } {
      set parsedline $line
    }
  }

  return $alldata
}

# ----------------------------------------------------------------
# DB analysis (setup & hold slacks)
# Get all negative slacks - if all slacks > 0, get worst positive one

proc ssta_slack_report_simple {sf} {
  proc build_slacklist {sf setuporhold} {
  # --- retreives the faulty slacks
   set sl [ stb_GetSlacks $sf $setuporhold -nbslacks 10000 ]
   if { [ llength $sl  ] == 0 } {
  # --- retreives the worst slack if no faulty slacks are found
     set sl [ stb_GetSlacks $sf -nbslacks 1 $setuporhold -margin 1e9 ]
   }
  
  # --- builds the slacklist
   set reslist {}
   foreach s $sl {
     set start_name [ ssta_GetEventName [ stb_GetSlackProperty $s START_EVENT ] ]
     set end_name   [ ssta_GetEventName [ stb_GetSlackProperty $s END_EVENT   ] ]
     set thru_evt   [ stb_GetSlackProperty $s THRU_EVENT  ]
     set slack      [format %g [ stb_GetSlackProperty $s VALUE ]]
     
     if { $thru_evt == "NULL" } {
       set thru_name "-"
     } else {
       set thru_name [ ssta_GetEventName $thru_evt ]
     }
   
     if {[ stb_GetSlackProperty $s IS_HZ  ]=="yes"} {
       set end_name [list [lindex $end_name 0] [lindex $end_name 1]z]
     }

     set val [list $slack $start_name $thru_name $end_name]
     lappend reslist $val
   }
   stb_FreeSlackList $sl
   return $reslist
  }
  
  set setups [build_slacklist $sf -setuponly]
  set holds [build_slacklist $sf -holdonly]
  set pvtcount [hitas_pvt_count]
  set figname [ttv_GetTimingFigureProperty [stb_GetStabilityFigureProperty $sf TIMING_FIGURE] NAME]
  set finalres [list SR0 $figname [avt_GetGlobalSeed] [avt_GetMainSeed] $pvtcount "setup" $setups "hold" $holds]
  
  # --- sends results and finish statistical session
#  puts $finalres
  avt_McPostData $finalres
}

proc ssta_ToolBox {args} {
  global tcl_interactive
  set i 0
  set title ""
  set filename "file_name_not_set"
  set values {}
  set distrib {}
  set act_gdist 0
  set act_dplot 0
  set act_gcol 0
  set act_disp 0
  set act_pdata 0
  set parsefunc ""
  set nbrange 20
  set fixedrange 0
    
  while {[lindex $args $i]!=""} {
    set opt [lindex $args $i]
    incr i
    if {$opt=="-values"} {
      set values [lindex $args $i]
      incr i
    } elseif {$opt=="-dist"} {
      set distrib [lindex $args $i]
      incr i
    } elseif {$opt=="-filename"} {
      set filename [lindex $args $i]
      incr i
    } elseif {$opt=="-title"} {
      set title [lindex $args $i]
      incr i
    } elseif {$opt=="-getdistrib"} {
      set act_gdist 1
    } elseif {$opt=="-plot"} {
      set act_dplot 1
    } elseif {$opt=="-display"} {
      set act_disp 1
    } elseif {$opt=="-parsedatafile"} {
      set act_pdata 1
    } elseif {$opt=="-parsefunction"} {
      set parsefunc [lindex $args $i]
      incr i
    } elseif {$opt=="-getfield"} {
      set act_gcol [lindex $args $i]
      incr i
    } elseif {$opt=="-fixedrange"} {
      set fixedrange [lindex $args $i]
      set nbrange 0
      incr i
    } elseif {$opt=="-nbrange"} {
      set nbrange [lindex $args $i]
      incr i
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $args $prei]'"
      puts "Usage: ssta_ToolBox \[-values <vallist>] \[-dist <distribution>] \[-filename <fname>] \[-title <titlestring>]"
      puts "                    \[-nbrange <nbrange>] \[-parsedatafile] \[-parsefunction <funcname>]"
      puts "       commands:   \[-getdistrib] \[-plot] \[-getfield]"
      puts "       ssta_ToolBox \[-postdata <stability figure> <method>]"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }

  if {$act_pdata!=0} {
    return [ssta_parsedata $filename $parsefunc]
  }

  if {$act_gcol!=0} {
    return [ssta_get_field $filename $act_gcol]
  }

  if {$act_gdist!=0} {
    return [ssta_make_distr $values $nbrange $fixedrange]
  }
  
  if {$values!={}} {
    set n [llength $values]
    set mean 0
    foreach v $values {
      set v [expr $v]
      set mean [expr $mean+$v]
    }
    set mean [expr $mean/$n]
    set var 0
    foreach v $values {
      set v [expr $v]
      set var [expr $var+($v-$mean)*($v-$mean)]
    }
    set var [expr $var/$n]
  }

  if {$act_dplot!=0} {
    set distrib [ssta_make_distr $values $nbrange $fixedrange]
    ssta_make_gnuplot $distrib $filename $title $mean $var
  }
  
  if {$act_disp!=0} {
    if {$distrib=={}} {
      set distrib [ssta_make_distr $values $nbrange $fixedrange]
    }
    puts "title: $title"
    foreach sd $distrib {
      set range "[lindex $sd 0] -> [lindex $sd 1]"
      puts "  [format %-30s $range] : [format %-10s [lindex $sd 2]] [format %-10s [lindex $sd 3]] [format %-10s ([lindex $sd 4]%)]"
    }
    if {$values!={}} {
      set ototal [ lsort -real $values ]
      set median [ lindex $ototal [ expr "int($n/2)" ] ]
      puts "gaussian: mean=[format %g $mean] variance=[format %g $var], median=[format %g $median]"
    }
  }

}

proc runStatHiTas {nbrun args} {
  
  proc change_avtLibraryDirs {pwd} {
    set current [avt_GetConfig avtLibraryDirs]
    regsub -line -all {(:)([^/])} $current "\\1$pwd/\\2" current
    regsub -line -all {^([^/])} $current "$pwd/\\1" current
    regsub -line -all {([^/])(:)} $current "\\1/\\2" current
    avt_config avtLibraryDirs $current
  }
  global tcl_interactive
  global ssta_MasterPath

  set i 0
  set script "[avt_gettcldistpath]/exec_ssta_local.sh"
  set datafile "ssta_data.log"
  set incremental 0
  set storedir "NULL"
    
  if {!$tcl_interactive} {
    if {![avt_McIsSlave]} {
      set slavescript [info script]
      while {[lindex $args $i]!=""} {
        set opt [lindex $args $i]
        incr i
        if {$opt=="-jobscript"} {
          set script [lindex $args $i]
          incr i
        } elseif {$opt=="-override"} {
          set incremental -1
        } elseif {$opt=="-incremental"} {
          set incremental 1
        } elseif {$opt=="-slavescript"} {
          set slavescript [lindex $args $i]
          incr i
        } elseif {$opt=="-storedir"} {
          set storedir [lindex $args $i]
          incr i
        } elseif {$opt=="-result"} {
          set datafile [lindex $args $i]
          incr i
        } else {
          set prei [expr $i-1]
          puts "Unknown option '[lindex $args $prei]'"
          puts "Usage: runStatHiTas ..."
          exit
        } 
      }
      if {[string index $slavescript 0]!="/"} {
        set slavescript "[pwd]/$slavescript"
      }
      runStatHiTas_sub $nbrun $script $slavescript $datafile $incremental $storedir
      exit
    } else {
      avt_config spiActivateStatisticalFunctions yes
      avt_config rcxMinRCSignal 0
      set ssta_MasterPath [avt_McAsk pwd]
      change_avtLibraryDirs $ssta_MasterPath
    }
  }
}

set ssta_nbfailed 0
set ssta_nbsetup 0
set ssta_nbhold 0
set ssta_nbpvt 0
set ssta_holds {}
set ssta_setups {}
set ssta_num_run 0
set rundesc {}
array set setupdict {}
array set holddict {}
set ssta_runlist {}

proc proccess_mc_data { data } {
  global ssta_num_run
  global ssta_nbfailed
  global ssta_nbsetup
  global ssta_nbhold
  global ssta_nbpvt
  global ssta_setups
  global ssta_holds
  global setupdict
  global holddict
  global rundesc
  global ssta_figname
  global ssta_runlist

  set id [lindex $data 0]
  set filedata [lindex $data 1]
  if {[lindex $filedata 0]!="SR0"} {
    puts "** Invalid montecarlo slack data input file"
    exit
  }

  set ssta_figname [lindex $filedata 1]
  set gseed [lindex $filedata 2]
  set lseed [lindex $filedata 3]
  lappend ssta_runlist [list $gseed $lseed]
  set pvt [lindex $filedata 4]
  set allsetuplist [lindex $filedata 6]
  set allholdlist [lindex $filedata 8]
  set setupv [lindex [lindex $allsetuplist 0] 0]
  set holdv [lindex [lindex $allholdlist 0] 0]
  lappend rundesc [list $gseed $lseed $pvt]
  if {$pvt!=0} {
    incr ssta_nbpvt
  } else {
    lappend ssta_setups $setupv
    lappend ssta_holds $holdv
  }
  if {$setupv<=0} {
    incr ssta_nbsetup
  }
  if {$holdv<=0} {
    incr ssta_nbhold
  }
  if {$setupv<=0 || $holdv<=0 || $pvt!=0} {
    incr ssta_nbfailed
  }

  foreach d $allsetuplist {
    if {[lindex $d 0]<=0} {
      set slackdescr [list [lindex $d 1] [lindex $d 2] [lindex $d 3]]
      set slackval [lindex $d 0]

      if {[info exists setupdict($slackdescr)]} {
        set val $setupdict($slackdescr)
        set min [lindex $val 0]
        set minval [lindex $val 1]
        set allval [lindex $val 2]
      } else {
        set min -1
        set allval [mbk_QuickListCreate]
      }
      mbk_QuickListAppendDouble $allval $slackval
      if {$min==-1 || $minval>$slackval} {
        set min $ssta_num_run
        set minval $slackval
      }
      set setupdict($slackdescr) [list $min $minval $allval]
    }
  }
  foreach d $allholdlist {
    if {[lindex $d 0]<=0} {
      set slackdescr [list [lindex $d 1] [lindex $d 2] [lindex $d 3]]
      set slackval [lindex $d 0]
      if {[info exists holddict($slackdescr)]} {
        set val $holddict($slackdescr)
        set min [lindex $val 0]
        set minval [lindex $val 1]
        set allval [lindex $val 2]
      } else {
        set min -1
        set allval [mbk_QuickListCreate]
      }
      mbk_QuickListAppendDouble $allval $slackval
      if {$min==-1 || $minval>$slackval} {
        set min $ssta_num_run
        set minval $slackval
      }
      set holddict($slackdescr) [list $min $minval $allval]
    }
  }
  incr ssta_num_run
}

proc ssta_readstafile {filename} {
  global ssta_num_run
  global ssta_nbfailed
  global ssta_nbsetup
  global ssta_nbhold
  global ssta_nbpvt
  global ssta_setups
  global ssta_holds
  global setupdict
  global holddict
  global rundesc
  global ssta_figname
  set ssta_nbfailed 0
  set ssta_nbsetup 0
  set ssta_nbhold 0
  set ssta_nbpvt 0
  set ssta_holds {}
  set ssta_setups {}
  set ssta_num_run 0
  set rundesc {}
  array set setupdict {}
  array set holddict {}
  set ssta_runlist {}
  set ssta_figname ""
  
  array unset setupdict *
  array unset holddict *
  ssta_ToolBox -filename $filename -parsedatafile -parsefunction proccess_mc_data

}

proc ssta_getmeanandvar {values mean_u var_u median_u} {
  upvar $mean_u mean
  upvar $median_u median
  upvar $var_u var
  set median 0
  set mean 0
  set var 0
  if {$values!={}} {
   set mean 0
   set n 0
   foreach v $values {
     set v [expr $v]
     set mean [expr $mean+$v]
     incr n
   }
   set mean [expr $mean/$n]
   set var 0
   foreach v $values {
     set v [expr $v]
     set var [expr $var+($v-$mean)*($v-$mean)]
   }
   set var [expr $var/$n]

#   set intlst {}
#   foreach v $values {
#     lappend intlst [expr int($v*1e13+0.5)]
#   }
#  set ototal [lsort -integer $intlst]
#  set median [expr [lindex $ototal [expr $n/2]]*1e-13]
   set ototal [ lsort -real $values ]
   set median [lindex $ototal [expr $n/2]]
 }
}

proc ssta_print_slack_simple {filept fig} {

  global pathdict
  global ssta_num_run
  global setupdict
  global holddict
  global ssta_nbfailed
  global ssta_nbsetup
  global ssta_nbhold
  global ssta_nbpvt

  proc print_occur {filept lst num_run} {
    proc getdir {a} {
      if {[lindex $a 1]=="u"} {
        return "(R)"
      } else {
        return "(F)"
      }
    }
    set c_number 0
    set c_occur 1
    set c_mean 2
    set c_var 3
    set c_median 4
    set c_min   6
    set c_from_dir 15
    set c_from 16
    set c_thru_dir 17
    set c_thru 18
    set c_to_dir   19
    set c_to   20
    set b [Board_CreateBoard]
    Board_SetSize $b $c_number 7 r
    Board_SetSize $b $c_from 10 l
    Board_SetSize $b $c_to 10 l
    Board_SetSize $b $c_thru 10 l
    Board_SetSize $b $c_occur 7 r
    Board_SetSize $b $c_mean 12 r
    Board_SetSize $b $c_median 12 r
    Board_SetSize $b $c_var 12 r
    Board_SetSize $b $c_from_dir 1 r
    Board_SetSize $b $c_thru_dir 1 r
    Board_SetSize $b $c_to_dir 1 r
    Board_SetSize $b $c_min 4 r
    Board_NewSeparation $b
    Board_NewLine $b
    Board_SetValue $b $c_number "Number"
    Board_SetValue $b $c_occur "# Occurrence"
    Board_SetValue $b $c_from "From_node"
    Board_SetValue $b $c_to "To_node"
    Board_SetValue $b $c_thru "Thru_node"
    Board_SetValue $b $c_median "Median"
    Board_SetValue $b $c_var "Variance"
    Board_SetValue $b $c_mean "Mean"
    Board_SetValue $b $c_min "Min value/Run number"
    Board_NewSeparation $b

    set nb 1
    foreach o $lst {
      Board_NewLine $b
      Board_SetValue $b $c_number $nb
      Board_SetValue $b $c_occur "[lindex $o 0] ([format %3.1f [expr [lindex $o 0]*100.0/$num_run]]%)"
      set pathdesc [lindex $o 1]
      Board_SetValue $b $c_from [lindex [lindex $pathdesc 0] 0]
      Board_SetValue $b $c_from_dir [getdir [lindex $pathdesc 0]]
      Board_SetValue $b $c_to [lindex [lindex $pathdesc 2] 0]
      Board_SetValue $b $c_to_dir [getdir [lindex $pathdesc 2]]
      if {[lindex $pathdesc 1]!="-"} {
        Board_SetValue $b $c_thru [lindex [lindex $pathdesc 1] 0]
        Board_SetValue $b $c_thru_dir [getdir [lindex $pathdesc 1]]
      }
      Board_SetValue $b $c_median [format %g [lindex $o 2]]
      Board_SetValue $b $c_var [format %g [lindex $o 3]]
      Board_SetValue $b $c_mean [format %g [lindex $o 4]]

      set values [lindex $o 5]
      set min [lindex $values 0]
      set minval [lindex $values 1]
      incr min
      Board_SetValue $b $c_min "[format %g $minval] #[format %05d $min]"
      incr nb
    }
    Board_Display $filept $b ""
    Board_FreeBoard $b
  }
  
  proc get_slack {fig sslack_u hslack_u soccurlst hoccurlst donerun_u num} {
    upvar $sslack_u sslack 
    upvar $hslack_u hslack 
    upvar $donerun_u donerun
    set oldmode [ttv_AutomaticDetailBuild on]

    set old_stbOutFile [avt_GetConfig "stbOutFile"]
    set old_stbReportFile [avt_GetConfig "stbReportFile"]
    set old_stbCrosstalkMode [avt_GetConfig "stbCrosstalkMode"]
    set old_stbSetupHoldUpdate [avt_GetConfig "stbSetupHoldUpdate"]
    avt_Config stbOutFile no
    avt_Config stbReportFile no
    avt_Config stbCrosstalkMode no
    avt_config stbSetupHoldUpdate no
    
    if {![info exists donerun($num)]} {
       set donerun($num) 1
       set runstb 0
       set occur(0) $soccurlst
       set occur(1) $hoccurlst
       set opt(0) -setuponly
       set opt(1) -holdonly
       for {set j 0} {$j<=1} {incr j} {
         set cnt 1
         foreach s $occur($j) {
           set desc [lindex $s 1]
           set values [lindex $s 5]
           set min [lindex $values 0]
           if {$j==0} {
              set alreadyexists [info exists sslack($desc)]
           } else {
              set alreadyexists [info exists hslack($desc)]
           }
           if {!$alreadyexists && $min==$num} {
              if {$runstb==0} {
                if {[ttv_SetSSTARunNumber $fig [expr $num+1]]==0} {
                  ssta_Verbose "-- running STB on run #[expr $num+1] for index=$cnt"
                  set sb [stb $fig]
                } else {
                  set sb "NULL"
                }
                set runstb 1
              }
              if {$sb!="NULL"} {
                set thru 0
                set c_from [lindex [lindex $desc 0] 0]
                set c_from_dir [lindex [lindex $desc 0] 1]
                set c_to [lindex [lindex $desc 2] 0]
                set c_to_dir [lindex [lindex $desc 2] 1]
                if {[lindex $desc 1]!="-"} {
                  set c_thru [lindex [lindex $desc 1] 0]
                  set c_thru_dir [lindex [lindex $desc 1] 1]
                  set thru 1
                }
  
                ssta_Verbose "  -- slack $desc $opt($j) for index=$cnt"
                if {$thru==0} {
                  set slack [stb_GetSlacks $sb -from $c_from -to $c_to -nbslacks 1 $opt($j) -dir $c_from_dir$c_to_dir -margin 1]
                } else {
                  set slack [stb_GetSlacks $sb -from $c_from -to $c_to -thru_node $c_thru -nbslacks 1 $opt($j) -dir $c_from_dir$c_to_dir -margin 1]
                }

                if {[llength $slack]!=0} {
                  if {$j==0} {
                    set sslack($desc) $slack
                  } else {
                    set hslack($desc) $slack
                  }
                  set dvp [stb_GetSlackProperty $slack DATA_VALID_PATH]
                  if {$dvp=="NULL"} {
                     # internal error
                     if {$j==0} {
                      unset sslack($desc)
                    } else {
                      unset hslack($desc)
                    }
                    puts "internal error (1) retreiving slack $desc (run #[expr $num+1])"
                  }
                  set drp [stb_GetSlackProperty $slack DATA_REQUIRED_PATH]
                } else {
                    puts "internal error (2) retreiving slack $desc (run #[expr $num+1])"
                }
             }
           }
           incr cnt
         }
       }
       if {$runstb==1 && $sb!="NULL"} {
         stb_FreeStabilityFigure $sb
       }
    }
    ttv_AutomaticDetailBuild $oldmode
    avt_Config stbOutFile $old_stbOutFile
    avt_Config stbReportFile $old_stbReportFile
    avt_Config stbCrosstalkMode $old_stbCrosstalkMode
    avt_config stbSetupHoldUpdate $old_stbSetupHoldUpdate
  }

  proc print_worst_occur_slack {filept occurlst slackl_u donerun_u fig sslack_u hslack_u soccurlst hoccurlst} {
    upvar $slackl_u slackl
    upvar $donerun_u donerun
    upvar $sslack_u sslack 
    upvar $hslack_u hslack 
    set num 1

    fputs "\n\n" $filept

    foreach o $occurlst {
      set desc [lindex $o 1]
      set values [lindex $o 5]
      set min [lindex $values 0]
      fputs "Slack Detail ($num) (from run #[format %05d [expr $min+1]]):\n" $filept
      get_slack $fig sslack hslack $soccurlst $hoccurlst donerun $min
      if {[info exists slackl($desc)]} {
         stb_DisplaySlackReport $filept -slacks $slackl($desc)
         stb_FreeSlackList $slackl($desc)
      } else {
        fputs "\n **** ERROR: FAILED TO RETREIVE SLACK ****\n\n" $filept
      }
      incr num
#      if {$num>10} {
#        break
#      }
    }
  }
  
  set yield [ expr 100*($ssta_num_run-$ssta_nbfailed)/$ssta_num_run ]
  fputs "\n      *** SSTA slack information ***\n\n" $filept
  fputs "  * Failed runs :            $ssta_nbfailed/$ssta_num_run\n" $filept
  fputs "  * Fails due to holds:      $ssta_nbhold\n" $filept
  fputs "  * Fails due to setups:     $ssta_nbsetup\n" $filept
  fputs "  * Fails due to PVT errors: $ssta_nbpvt\n" $filept
  fputs "\n" $filept
  fputs "  * Yield: [format %3.1f $yield]%\n" $filept

  set occurlst [mbk_QuickListCreate]
  set entries [array names setupdict]
  foreach slackdescr $entries {
    set value $setupdict($slackdescr)
    set values [lindex $value 2]
    set count [mbk_QuickListLength $values]
    set statval [mbk_QuickListComputeMeanVarMedian $values]
    set mean [lindex $statval 0]
    set var [lindex $statval 1]
    set median [lindex $statval 2]
    mbk_QuickListDoubleFree $values
#    ssta_getmeanandvar [mbk_QuickListToTCL $values] mean var median
    mbk_QuickListAppend $occurlst [list $count $slackdescr $mean $var $median [list [lindex $value 0] [lindex $value 1]]]
  }
  set soccurlst [ lsort -decreasing -integer -index 0 [mbk_QuickListToTCL $occurlst] ]
  
  set occurlst [mbk_QuickListCreate]
  set entries [array names holddict]
  foreach slackdescr $entries {
    set value $holddict($slackdescr)
    set values [lindex $value 2]
    set count [mbk_QuickListLength $values]
    set statval [mbk_QuickListComputeMeanVarMedian $values]
    set mean [lindex $statval 0]
    set var [lindex $statval 1]
    set median [lindex $statval 2]
    mbk_QuickListDoubleFree $values
#     ssta_getmeanandvar [mbk_QuickListToTCL $values] mean var median
    mbk_QuickListAppend $occurlst [list $count $slackdescr $mean $var $median [list [lindex $value 0] [lindex $value 1]]]
  }
  set hoccurlst [ lsort -decreasing -integer -index 0 [mbk_QuickListToTCL $occurlst] ]

  array set donerun {}

  set figname [ttv_GetTimingFigureProperty $fig NAME]
  inf_SetFigureName $figname
  inf_CleanFigure
  avt_LoadFile "$figname.ssta.stb.inf" inf
  set sstatclconfig "$figname.ssta.stb.tcl"
  if {[file exists $sstatclconfig]} {
    ssta_Verbose "  -- sourcing file '$sstatclconfig'"
    source $sstatclconfig
  }
  fputs "\n      *** SSTA setup slack report ($ssta_num_run runs) (unit:\[seconds]) ***\n\n" $filept
  print_occur $filept $soccurlst $ssta_num_run
  if {$fig!="NULL"} {
    print_worst_occur_slack $filept $soccurlst sslack donerun $fig sslack hslack $soccurlst $hoccurlst
  }

  fputs "\n      *** SSTA hold slack report ($ssta_num_run runs) (unit:\[seconds]) ***\n\n" $filept
  print_occur $filept $hoccurlst $ssta_num_run
  if {$fig!="NULL"} {
    print_worst_occur_slack $filept $hoccurlst hslack donerun $fig sslack hslack $soccurlst $hoccurlst
  }

}

proc ssta_SlackReport {args} {
  global tcl_interactive
  global ssta_setups ssta_holds
  global ssta_figname
  global ssta_runlist
  set i 0
    
  while {[lindex $args $i]!=""} {
    set opt [lindex $args $i]
    incr i
    if {$opt=="-plot"} {
      set act_dplot 1
      set filename [lindex $args $i]
      incr i
      set outputfilename [lindex $args $i]
      incr i
      ssta_readstafile $filename
      set distrib [ssta_make_distr $ssta_setups 20 -1]
      ssta_getmeanandvar $ssta_setups mean var median
      ssta_make_gnuplot $distrib $outputfilename.setups.plt "Setup Slacks" $mean $var
      set distrib [ssta_make_distr $ssta_holds 20 -1]
      ssta_getmeanandvar $ssta_holds mean var median
      ssta_make_gnuplot $distrib $outputfilename.holds.plt "Hold Slacks" $mean $var
     } elseif {$opt=="-display"} {
      set fig [_NULL_]
      set filename [lindex $args $i]
      incr i
      set ofile [lindex $args $i]
      incr i
      ssta_Verbose "-- loading file '$filename'"
      ssta_readstafile $filename
      if {[lindex $args $i]=="-storedir"} {
        incr i
        set store [lindex $args $i]
        incr i
        set res [catch "glob $store/*.ssta.ctk.stored" allfiles]
        if {$res!=0 || $allfiles=={}} {
          set res [catch "glob $store/*.ssta.stored" allfiles]
          if {$res!=0} {
            set allfiles {}
          }
        }
        set fig [ttv_LoadSpecifiedTimingFigure $ssta_figname]
        ssta_Verbose "-- loading [llength $allfiles] stored results in directory '$store/'"
        set tot [ttv_LoadSSTAResults $fig $allfiles $ssta_runlist]
        if {$tot>0} {
          ssta_Verbose "-- building timing line stats"
          ttv_BuildSSTALineStats $fig
        }
      }
      ssta_Verbose "-- driving setup & hold report"
      ssta_print_slack_simple $ofile $fig
      ssta_print_runs_info $ofile
   } elseif {$opt=="-senddata"} {
      set stbfig [lindex $args $i]
      incr i
      set method [lindex $args $i]
      incr i
      ssta_slack_report_$method $stbfig
      return
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $args $prei]'"
      puts "Usage: ssta_SlackReport -plot <input ssta filename> <output filename>"
      puts "  or   ssta_SlackReport -senddata <stability figure> <methodname>"
      puts "  or   ssta_SlackReport -display <ssta result file> <filedescriptor>"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }
  
}

# ----------------------------------------------------------------

proc ssta_path_report_simple {paths} {
  
  # --- builds the slacklist
  set oneev [_NULL_]
  set reslist {}
  foreach p $paths {
    set oneev [ ttv_GetTimingPathProperty $p START_EVENT ]
    set start_name [ ssta_GetEventName [ ttv_GetTimingPathProperty $p START_EVENT ] ]
    set end_name   [ ssta_GetEventName [ ttv_GetTimingPathProperty $p END_EVENT   ] ]
    set thru_evt   [ ttv_GetTimingPathProperty $p ACCESS_LATCH  ]
    set delay      [format %g [ ttv_GetTimingPathProperty $p DELAY ]]
    
    if { $thru_evt == "NULL" } {
      set thru_name "-"
    } else {
      set thru_name [ ssta_GetEventName $thru_evt ]
    }

    if {[ ttv_GetTimingPathProperty $p IS_HZ  ]=="yes"} {
      set end_name [list [lindex $end_name 0] [lindex $end_name 1]z]
    }
    
    set ptype "M"
    if {[ ttv_GetTimingPathProperty $p TYPE  ]=="min"} {
      set ptype "m"
    }

    set val [list $delay $start_name $thru_name $end_name $ptype]
    lappend reslist $val
  }
  
  set figname [ttv_GetTimingFigureProperty [ttv_GetTimingSignalProperty [ttv_GetTimingEventProperty $oneev SIGNAL] TOP_LEVEL] NAME]
  set totalres [list "PR0" $figname [avt_GetGlobalSeed] [avt_GetMainSeed] [hitas_pvt_count] $reslist]

  # --- sends results and finish statistical session
  avt_McPostData $totalres
}

array set pathdict {}

proc proccess_mc_path_data { data } {
  global pathdict
  global ssta_num_run
  global rundesc
  global ssta_figname
  global ssta_runlist

  set id [lindex $data 0]
  set line [lindex $data 1]
  if {[lindex $line 0]!="PR0"} {
    puts "** Invalid montecarlo path data input file"
    exit
  }
  set ssta_figname [lindex $line 1]
  set gseed [lindex $line 2]
  set lseed [lindex $line 3]
  lappend ssta_runlist [list $gseed $lseed]
  set pvt [lindex $line 4]

  set pathlist [lindex $line 5]
  foreach d $pathlist {
    set delay [lindex $d 0]
    set pathdescr [list [lindex $d 1] [lindex $d 2] [lindex $d 3]]
    
    if {[info exists pathdict($pathdescr)]} {
      set val $pathdict($pathdescr)
      set min [lindex $val 0]
      set max [lindex $val 1]
      set allval [lindex $val 2]
    } else {
      set min -1
      set max -1
      set allval {}
    }
    lappend allval $delay
    if {$min==-1 || [lindex $allval $min]>$delay} {
      set min $ssta_num_run
    }
    if {$max==-1 || [lindex $allval $max]<$delay} {
      set max $ssta_num_run
    }

    set pathdict($pathdescr) [list $min $max $allval]
  }
  lappend rundesc [list $gseed $lseed $pvt]
  incr ssta_num_run
}

proc ssta_readpathfile {filename} {
  global pathdict
  global ssta_num_run
  global rundesc
  global ssta_figname
  array set pathdict {}
  set ssta_num_run 0
  set rundesc {}
  set ssta_figname ""
  
  ssta_ToolBox -filename $filename -parsedatafile -parsefunction proccess_mc_path_data
}
proc ssta_print_path_detail_simple {filept fig occurlst} {
  fputs "\n      *** SSTA path detail report ***\n\n" $filept
  set nb 1
  foreach o $occurlst {
    set pathdesc [lindex $o 1]
    set c_from [lindex [lindex $pathdesc 0] 0]
    set c_from_dir [lindex [lindex $pathdesc 0] 1]
    set c_to [lindex [lindex $pathdesc 2] 0]
    set c_to_dir [lindex [lindex $pathdesc 2] 1]
    if {[lindex $pathdesc 3]=="m"} {
      set ptype "-min"
    } else {
      set ptype "-max"
    }
    if {[lindex $pathdesc 1]!="-"} {
      set c_thru [lindex [lindex $pathdesc 1] 0]
    } else {
      set c_thru ""
    }
    set values [lindex $o 5]
    set min [lindex $values 0]
    set minval [lindex [lindex $values 2] $min]
    incr min
    set max [lindex $values 1]
    set maxval [lindex [lindex $values 2] $max]
    incr max
   
    fputs "\nPath detail ($nb)\n\n" $filept
    fputs "\n   Path For minimum value $minval (run #[format %05d $min])\n\n" $filept
    
    if {[ttv_SetSSTARunNumber $fig $min]==0} {
      if {$c_thru==""} {
        set path [ttv_GetPaths $fig -from $c_from -to $c_to -dir $c_from_dir$c_to_dir -nb 1 $ptype]
      } else {
        set path [ttv_GetPaths $fig -from $c_from -to $c_to -dir $c_from_dir$c_to_dir -nb 1 $ptype -thru $c_thru -access]
      }
      if {[llength $path]>0} {
        ttv_DisplayPathDetail $filept -1 $path
        ttv_FreePathList $path
      } else {
        puts "internal error (3) retreiving path $pathdesc (run #$min)"
        fputs "\n **** ERROR: FAILED TO RETREIVE THIS PATH ****\n\n" $filept
      }
    } else {
      fputs "\n **** ERROR: FAILED TO RETREIVE THIS PATH ****\n\n" $filept
    }
    
    fputs "\n   Path For maximum value $maxval (run #[format %05d $max])\n\n" $filept
    if {[ttv_SetSSTARunNumber $fig $max]==0} {
      if {$c_thru==""} {
        set path [ttv_GetPaths $fig -from $c_from -to $c_to -dir $c_from_dir$c_to_dir -nb 1 $ptype]
      } else {
        set path [ttv_GetPaths $fig -from $c_from -to $c_to -dir $c_from_dir$c_to_dir -nb 1 $ptype -thru $c_thru -access]
      }

      if {[llength $path]>0} {
        ttv_DisplayPathDetail $filept -1 $path
        ttv_FreePathList $path
      } else {
        puts "internal error (3) retreiving path $pathdesc (run #$max)"
        fputs "\n **** ERROR: FAILED TO RETREIVE THIS PATH ****\n\n" $filept
      }
      
    } else {
      fputs "\n **** ERROR: FAILED TO RETREIVE THIS PATH ****\n\n" $filept
    }

    incr nb
  }
}

proc ssta_print_path_simple {filept fig} {

  global pathdict
  global ssta_num_run
  set occurlst {}
  set entries [array names pathdict]
  foreach pathdescr $entries {
    set value $pathdict($pathdescr)
    ssta_getmeanandvar [lindex $value 2] mean var median
    set maxval [lindex [lindex $value 2] [lindex $value 1]]
    set minval [lindex [lindex $value 2] [lindex $value 0]]
    set crit [expr abs(($maxval-$minval)/$mean)]

    lappend occurlst [list $crit $pathdescr $mean $var $median $value]
    #puts $value
  }

  fputs "\n      *** SSTA path report ($ssta_num_run runs) ***\n\n" $filept
  set occurlst [ lsort -decreasing -real -index 0 $occurlst ]

  set c_number 0
  set c_mean 1
  set c_var 2
  set c_median 3
  set c_min   5
  set c_max   6
  set c_from_dir 15
  set c_from 16
  set c_thru_dir 17
  set c_thru 18
  set c_to_dir   19
  set c_to   20
  set c_pathtype   21
  
  set b [Board_CreateBoard]
  Board_SetSize $b $c_number 7 r
  Board_SetSize $b $c_from 10 l
  Board_SetSize $b $c_to 10 l
  Board_SetSize $b $c_thru 10 l
  Board_SetSize $b $c_mean 12 r
  Board_SetSize $b $c_median 12 r
  Board_SetSize $b $c_var 12 r
  Board_SetSize $b $c_from_dir 5 r
  Board_SetSize $b $c_thru_dir 1 r
  Board_SetSize $b $c_to_dir 1 r
  Board_SetSize $b $c_min 4 r
  Board_SetSize $b $c_max 4 r
  Board_SetSize $b $c_pathtype 15 r
  Board_NewSeparation $b
  Board_NewLine $b
  Board_SetValue $b $c_number "Number"
  Board_SetValue $b $c_from "From_node"
  Board_SetValue $b $c_to "To_node"
  Board_SetValue $b $c_thru "Thru_node"
  Board_SetValue $b $c_median "Median"
  Board_SetValue $b $c_var "Variance"
  Board_SetValue $b $c_mean "Mean"
  Board_SetValue $b $c_min "   Min value/Run number"
  Board_SetValue $b $c_max "Max value/Run number"
  Board_SetValue $b $c_pathtype "Path type"
  Board_NewSeparation $b

  proc getdir {a} {
    if {[lindex $a 1]=="u"} {
      return "(R)"
    } else {
      return "(F)"
    }
  }

  set nb 1
  foreach o $occurlst {
    Board_NewLine $b
    Board_SetValue $b $c_number $nb
    set pathdesc [lindex $o 1]
    Board_SetValue $b $c_from [lindex [lindex $pathdesc 0] 0]
    Board_SetValue $b $c_from_dir [getdir [lindex $pathdesc 0]]
    Board_SetValue $b $c_to [lindex [lindex $pathdesc 2] 0]
    Board_SetValue $b $c_to_dir [getdir [lindex $pathdesc 2]]
    if {[lindex $pathdesc 1]!="-"} {
      Board_SetValue $b $c_thru [lindex [lindex $pathdesc 1] 0]
      Board_SetValue $b $c_thru_dir [getdir [lindex $pathdesc 1]]
    }
    Board_SetValue $b $c_median [format %g [lindex $o 4]]
    Board_SetValue $b $c_var [format %g [lindex $o 3]]
    Board_SetValue $b $c_mean [format %g [lindex $o 2]]
    
    if {[lindex $pathdesc 3]=="m"} {
      set ptype "critic-min"
    } else {
      set ptype "critic-max"
    }

    Board_SetValue $b $c_pathtype $ptype

    set values [lindex $o 5]
    set min [lindex $values 0]
    set minval [lindex [lindex $values 2] $min]
    incr min
    set max [lindex $values 1]
    set maxval [lindex [lindex $values 2] $max]
    incr max
    Board_SetValue $b $c_min "[format %g $minval] #[format %05d $min]"
    Board_SetValue $b $c_max "[format %g $maxval] #[format %05d $max]"
    incr nb
  }
  Board_Display $filept $b ""
  Board_FreeBoard $b

  if {$fig!="NULL"} {
    ssta_print_path_detail_simple $filept $fig $occurlst
  }
}

proc ssta_print_runs_info {filept} {
  global rundesc

  set c_number 0
  set c_gseed 1
  set c_lseed 2
  set c_pvt 3
  
  fputs "\n      *** SSTA run information ***\n\n" $filept
  set b [Board_CreateBoard]
  Board_SetSize $b $c_number 7 r
  Board_SetSize $b $c_gseed 12 r
  Board_SetSize $b $c_lseed 12 r
  Board_SetSize $b $c_pvt 7 r
  Board_NewSeparation $b
  Board_NewLine $b
  Board_SetValue $b $c_number "Run Id"
  Board_SetValue $b $c_gseed "Global seed"
  Board_SetValue $b $c_lseed "Local seed"
  Board_SetValue $b $c_pvt "PVT errors"
  Board_NewSeparation $b

  set nb 1
  foreach o $rundesc {
    Board_NewLine $b
    Board_SetValue $b $c_number "#[format %05d $nb]"
    Board_SetValue $b $c_gseed [lindex $o 0]
    Board_SetValue $b $c_lseed [lindex $o 1]
    Board_SetValue $b $c_pvt [lindex $o 2]
    incr nb
  }
  Board_Display $filept $b ""
  Board_FreeBoard $b
}

proc ssta_PathReport {args} {
  global tcl_interactive
  global ssta_figname
  global ssta_runlist

  set i 0
    
  while {[lindex $args $i]!=""} {
    set opt [lindex $args $i]
    incr i
    if {$opt=="-display"} {
      set fig [_NULL_]
      set filename [lindex $args $i]
      incr i
      set ofile [lindex $args $i]
      incr i
      ssta_Verbose "-- loading file '$filename'"
      ssta_readpathfile $filename
      if {[lindex $args $i]=="-storedir"} {
        incr i
        set store [lindex $args $i]
        incr i
        set res [catch "glob $store/*.ssta.ctk.stored" allfiles]
        if {$res!=0 || $allfiles=={}} {
          set res [catch "glob $store/*.ssta.stored" allfiles]
          if {$res!=0} {
            set allfiles {}
          }
        }
        set fig [ttv_LoadSpecifiedTimingFigure $ssta_figname]
        ssta_Verbose "-- loading [llength $allfiles] stored results in directory '$store/'"
        set tot [ttv_LoadSSTAResults $fig $allfiles $ssta_runlist]
        if {$tot>0} {
          ssta_Verbose "-- building timing line stats"
          ttv_BuildSSTALineStats $fig
        }
      }
      ssta_Verbose "-- driving path report"
      ssta_print_path_simple $ofile $fig
      ssta_print_runs_info $ofile
    } elseif {$opt=="-senddata"} {
      set paths [lindex $args $i]
      incr i
      set method [lindex $args $i]
      incr i
      ssta_path_report_$method $paths
      return
    } else {
      set prei [expr $i-1]
      puts "Unknown option '[lindex $args $prei]'"
      puts "Usage: ssta_PathReport -display <ssta result file> <filedescriptor>"
      puts "  or   ssta_PathReport -senddata <path list> <methodname>"
      if {!$tcl_interactive} {
        exit
      } else {
        return [_NULL_]
      }
    } 
  }
}

