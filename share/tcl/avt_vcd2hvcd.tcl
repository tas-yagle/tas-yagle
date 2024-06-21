#!/usr/bin/env avt_shell

set ignore_rc_sig 0
set ignore_interm 0

proc parse_aliases { fname signals_u reverse_aliases_u} {
  upvar $signals_u signals
  upvar $reverse_aliases_u reverse_aliases
  puts ". Reading '$fname'"
  set fl [open $fname r]
  set cnt 0
  set cntalias 0
  while {[gets $fl line]>=0} {
    set names [split $line ,]
    set ref [string tolower [lindex $names 0]]
    set radical [lindex $ref 0]
    set index [lindex $ref 1]
    if {![info exists signals($radical)]} {
      set signals($radical) [mbk_QuickListCreate]
    }
    mbk_QuickListAppend $signals($radical) [list $index $names]
    incr cnt
    set cntalias [expr $cntalias+[llength $names]]
    foreach n $names {
      set reverse_aliases([string tolower $n]) $ref
    }
  }
  close $fl
  puts "  $cnt signals, $cntalias aliases"
}

proc build_tree {tree_top line signals_u reverse_aliases_u ignore_sig_u} {
  upvar $signals_u signals
  upvar $reverse_aliases_u reverse_aliases
  upvar $ignore_sig_u ignore_sig
  global ignore_rc_sig
  global ignore_interm
  set hasaliases 1
  if {[lindex $line 0]=="\$var"} {
    set sigtype [lindex $line 1]
    set nbbytes [lindex $line 2]
    set code [lindex $line 3]
    set signame [lindex $line 4]

    if {$ignore_interm && ([string first "_prereg" $signame]>=0 || [string first "_bebus" $signame]>=0 || [string first "_bereg" $signame]>=0)} {
      set ignore_sig($code) 1
    } elseif {$ignore_rc_sig && [string first "|" $signame]>=0} {
      set ignore_sig($code) 1
    } else {
      set range [lindex $line 5]
      set originfo [list $signame $sigtype $code $nbbytes $range]
      if {[string index $signame 0]=="\\"} {
        set signame [string range $signame 1 10000]
      }
      set signame [string tolower $signame]

      if {$range=="\$end"} {
        set newrange {}
      } else {
        set tmp [split $range ":\[\]"]
        set newrange [list [lindex $tmp 1] [lindex $tmp 2]]
      }

      if {[info exists signals($signame)]} {
        set aliases [mbk_QuickListToTCL $signals($signame)]
      } else {
        set aliases [list [list $newrange [list $signame]]]
        if {$newrange!={}} {
          set hasaliases 0
          set first [lindex $newrange 0]
          set last [lindex $newrange 1]
          if {$first>$last} {
            set first [lindex $newrange 1]
            set last [lindex $newrange 0]
          }
        }
      }
      foreach indexes $aliases {
        set this_index [lindex $indexes 0]
        set cnt 0
        foreach name [lindex $indexes 1] {
          set hierlist [split $name .]
          set nb [llength $hierlist]
          set where $tree_top
          for {set i 0} {$i<[expr $nb-1]} {incr i} {
            set inst [string tolower [lindex $hierlist $i]]
            set hash [lindex $where 1]
            set found [mbk_GetStringHashItem $hash $inst]
            if {$found=="?"} {
              set newhash [mbk_NewHashTable 5]
              set lst [mbk_QuickListCreate]
              set where [list $lst $newhash]
              mbk_AddStringHashItem $hash $inst $where
            } else {
              set where $found
            }
          }
          set lst [lindex $where 0]
          set lastname [string tolower [lindex $hierlist $i]]
          set lowname [string tolower $name]
          if {$hasaliases} {
            if {[info exists reverse_aliases($lowname)]} {
              set revalias $reverse_aliases($lowname)
            } else {
              set revalias "$signame $this_index"
            }
            mbk_QuickListAppend $lst [list $lastname $code $sigtype $nbbytes $newrange $cnt [lindex $revalias 1]]
            incr cnt
          } else {
            while {$first<=$last} {
              mbk_QuickListAppend $lst [list "$lastname $first" $code $sigtype $nbbytes $newrange $cnt $first]
              incr first
            }
            set cnt [expr $cnt+$last-$first+1]
          }
        }
      }      
    }
  }
}

proc sortdown {a b} {
  set na [lindex $a 0]
  set nb [lindex $b 0]
  set res [string compare $na $nb]
  if {$res!=0} {
    return $res
  }
  set na [lindex $a 1]
  set nb [lindex $b 1]
  if {$na=="" && $nb!=""} {
    return -1
  } elseif {$nb=="" && $na!=""} {
    return 1
  } elseif {$nb=="" && $na==""} {
    return 0
  }

  if {$na>$nb} {
    return -1
  } elseif {$nb>$na} {
    return 1
  }
  return 0
}

set startcode 1000000

proc getnextfreecode {} {
  global startcode
  set base 94
  set start 33
  set b94 {}
  set tmp $startcode
  while {$tmp!=0} {
    set m [expr int($tmp / $base)]
    set r [expr int($tmp % $base)]
    lappend b94 [format %c [expr $r+$start]]
    set tmp $m
  }
  set res [join $b94 ""]
  incr startcode
  return $res
}

proc samevector {range vbitlist} {
  set vn ""
  foreach vb $vbitlist {
    if {$range!=[lindex $vb 2] || ([lindex $vb 3]=={} && $range!={})} {
      return 0
    }
    if {$vn!="" && $vn!=[lindex $vb 1]} {
      return 0
    }
    set vn [lindex $vb 1]
  }
  return 1
}

proc singlebitref {vbitlist} {
  if {[lindex [lindex $vbitlist 0] 2]=={}} {
    return 1
  }
  return 0
}

proc drive_hier_recur {flout tree reverse_aliases_u update_sig_u} {
  upvar $reverse_aliases_u reverse_aliases
  upvar $update_sig_u update_sig
  set lst [mbk_QuickListToTCL [lindex $tree 0]]
  set hash [lindex $tree 1]

  set lst [lsort -command sortdown -index 0 $lst]

  set cur ""
  set first ""
  set last ""
  set vsiglist {}
  set vbitlist {}
  foreach l $lst {
    set cname [lindex $l 0]
    set revalias_index [lindex $l 6]
    set radical [lindex $cname 0]
    set index [lindex $cname 1]
    set refrange [lindex $l 4]
    if {$cur==$radical} {
      set last $index
      set refcode [lindex $l 1]
      lappend vbitlist [list $index $refcode $refrange $revalias_index] 
    } else {
      if {$cur!=""} {
        if {$first==""} {
          set sigsize 1
          set range {}
        } else {
          set sigsize [expr abs($first-$last)+1]
          set range [list $first $last]
        }
        if {($sigsize==1 && [singlebitref $vbitlist]) || [samevector $range $vbitlist]} {
          set code $refcode
        } else {
          set urange [list $last $first]
          if {[samevector $urange $vbitlist]} {
            set code $refcode
            set range $urange
            set vbitlist [lreverse $vbitlist]
          } else {
            set code [getnextfreecode]
          }
        }

        set sig [list $cur $sigtype $sigsize $code $range $vbitlist]
        lappend vsiglist $sig
        if {$code!=$refcode} {
          array set tmp {}
          foreach vb $vbitlist {
            set refcode0 [lindex $vb 1]
            if {![info exists tmp($refcode0)]} {
              if {![info exists update_sig($refcode0)]} {
                set update_sig($refcode0) {}
              }
              lappend update_sig($refcode0) $sig
              set tmp($refcode0) 1
            }
          }
          array unset tmp
        }
      }
      set first $index
      set last $index
      set cur $radical
      set sigtype [lindex $l 2]
      set refcode [lindex $l 1]
      set vbitlist [list [list $index $refcode $refrange $revalias_index]]
    }
  }
 
  if {$lst!=""} {
    if {$first==""} {
      set sigsize 1
      set range ""      
    } else {
      set sigsize [expr abs($first-$last)+1]
      set range [list $first $last]
    }
    if {($sigsize==1 && [singlebitref $vbitlist]) || [samevector $range $vbitlist]} {
      set code $refcode
    } else {
      set urange [list $last $first]
      if {[samevector $urange $vbitlist]} {
        set code $refcode
        set range $urange
        set vbitlist [lreverse $vbitlist]
      } else {
        set code [getnextfreecode]
      }
    }
    set sig [list $cur $sigtype $sigsize $code $range $vbitlist]
    lappend vsiglist $sig
    if {$code!=$refcode} {
      array set tmp {}
      foreach vb $vbitlist {
        set refcode0 [lindex $vb 1]
        if {![info exists tmp($refcode0)]} {
          if {![info exists update_sig($refcode0)]} {
            set update_sig($refcode0) {}
          }
          lappend update_sig($refcode0) $sig
          set tmp($refcode0) 1
        }
      }
      array unset tmp
    }
  }

  foreach vs $vsiglist {
    set range [lindex $vs 4]
    if {[lindex $range 0]==""} {
      set disprange " "
    } else {
      set disprange "\[[lindex $range 0]:[lindex $range 1]\] "
    }
    puts $flout "\$var [lindex $vs 1] [lindex $vs 2] [lindex $vs 3] [lindex $vs 0] $disprange\$end"
  }

  unset lst
  unset vsiglist

  set insts [mbk_GetStringHashTableKeys $hash]
  set insts [lsort -dictionary $insts]
  foreach inst $insts {
    puts $flout "\$scope module $inst \$end"
    set subtree [mbk_GetStringHashItem $hash $inst]
    drive_hier_recur $flout $subtree reverse_aliases update_sig
    puts $flout "\$upscope \$end"
  }
}

proc parse_vcd_header { fl flout figname signals_u reverse_aliases_u update_sig_u ignore_sig_u} {
  upvar $signals_u signals
  upvar $reverse_aliases_u reverse_aliases
  upvar $update_sig_u update_sig
  upvar $ignore_sig_u ignore_sig
  puts ". Reading VCD header"
  set trig 0
  set stop 0
  set depth 0
  set tree_top [list [mbk_QuickListCreate] [mbk_NewHashTable 5]]
  while {!$stop && [gets $fl tline]>=0} {
    set line [split $tline]
    if {$trig} {
      if {[lindex $line 0]=="\$scope"} {
        incr depth
      } elseif {[lindex $line 0]=="\$upscope"} {
        if {$depth==0} {
          set trig 0
          puts ". Rebuilding hierarchy" 
          drive_hier_recur $flout $tree_top reverse_aliases update_sig
          puts $flout $tline
        } else {
          set depth [expr $depth-1]
          puts $flout $tline
        }
      } else {
        if {$depth==0} {
          build_tree $tree_top $line signals reverse_aliases ignore_sig
        }
      }
      if {$depth>0} {
        puts $flout $tline
      }
    } else {
      if {[lindex $line 0]=="\$dumpvars"} {
        set stop 1
      } elseif {[lindex $line 0]=="\$scope" && [lindex $line 1]=="module" && [lindex $line 2]=="$figname"} {
        set trig 1
        puts ". Found instance $figname"
      }
      puts $flout $tline
    }
  }
}

proc getbitvalue {range value index} {
  if {$range=="" || $index==""} {
    return $value
  }
  set left [lindex $range 0]
  set right [lindex $range 1]
  set max [string length $value]
  if {$left<=$right} {
    set rindex [expr $index-$left]
    if {$rindex>=$max} {
      set rindex [expr $max-1]
    }
  } else {
    set rindex [expr $index-$right]
    set sup [expr $max-1]
    if {$rindex>$sup} {
      set val [string index $value 0]
      if {$val!="1"} {
        return $val
      } else {
        return 0
      }
    }
    set rindex [expr $sup-$rindex]
  }
  return [string index $value $rindex]
}

proc buildvalue {vectsig values_u} {
  upvar $values_u values
  set bitlist [lindex $vectsig 5]
  set val {}
  set range [lindex $vectsig 4]
  set i [lindex $range 0]
  set last [lindex $range 1]
  if {$i==""} {
    set once 1
  } else {
    set once 0
    if {$last<$i} {
      set dir -1
    } else {
      set dir 1
    }
    set stop [expr $last+$dir]
  }
  set run 0
  while {$once || $i!=$stop} {
    set bl [lindex $bitlist $run]
    set curbit [lindex $bl 0]
    if {$curbit!=$i} {
      lappend val "x"
    } else {
      set refrange [lindex $bl 2]
      set refcode [lindex $bl 1]
      set refindex [lindex $bl 3]
      if {[info exists values($refcode)]} {
        set refvalue $values($refcode)
      } else {
        set refvalue "x"
      }
      lappend val [getbitvalue $refrange $refvalue $refindex]
      incr run
    }    
    if {$once} {
      break
    }
    set i [expr $i+$dir]
  }
  return $val
}

proc parse_vcd_values {fl flout signals_u update_sig_u ignore_sig_u} {
  upvar $signals_u signals
  upvar $update_sig_u update_sig
  upvar $ignore_sig_u ignore_sig
  puts ". Reading VCD values"
  while {[gets $fl tline]>=0} {
    set line [split $tline]
    set firstchar [string index $tline 0]
    if {$firstchar=="b" || $firstchar=="0" || $firstchar=="1" || $firstchar=="x" || $firstchar=="z"} {
      if {$firstchar=="b"} {
        set value [string range [lindex $line 0] 1 10000000]
        set code [lindex $line 1]
      } else {
        set value $firstchar
        set code [string range [lindex $line 0] 1 10000000]
      }
      
      if {![info exists ignore_sig($code)]} {
        puts $flout $tline
        if {[info exists update_sig($code)]} {
          set values($code) $value

          set lst $update_sig($code)
          foreach l $lst {
            set veccode [lindex $l 3]
            set vecrange [lindex $l 4]
            set res [buildvalue $l values]
            set joinedres [join $res ""]
            if {!([info exists values($veccode)] && $joinedres==$values($veccode))} {
              set values($veccode) $joinedres
              if {$vecrange==""} {
                puts $flout "$joinedres$veccode"
              } else {
                puts $flout "b$joinedres $veccode"
              }
            }
          }
        }
      }
    } else {
      puts $flout $tline
    }
  }
}

proc vcd_Hierarchy {figname vcdname outvcdname aliasfile} {
  parse_aliases "$aliasfile" signals reverse_aliases
  
  set flout [open $outvcdname w]
  set fl [open $vcdname r]
  
  puts ". Reading '$vcdname'"
  puts ". Writing $outvcdname"
  parse_vcd_header $fl $flout $figname signals reverse_aliases update_sig ignore_rc_sig
  
  array unset reverse_aliases
  
  parse_vcd_values $fl $flout signals update_sig ignore_rc_sig
  
  close $fl
  close $flout
}

avt_banner "vcd2Hvcd" "Flat VCD to Hierarchical VCD converter" 2008

set i 0
set stop 0
while {!$stop && [lindex $argv $i]!=""} {
  set opt [lindex $argv $i]
  if {$opt=="-striprc"} {
    set ignore_rc_sig 1
    incr i
  } elseif {$opt=="-strip_interm"} {
    set ignore_rc_sig 1
    incr i
  } else {
     set stop 1
  }
}


set vcdname [lindex $argv $i]
incr i
set figname [lindex $argv $i]
incr i
set aliasfilename [lindex $argv $i]
incr i
set outvcdname [lindex $argv $i]

if {$vcdname=="" || $figname=="" || $aliasfilename==""} {
 puts "Usage:" 
 puts "  $argv0 \[options\] <input vcd filename> <instance name> <aliases file name> \[output vcd name\]" 
 puts "  options:"
 puts "    -striprc : remove RC signals from result .vcd"
 puts "    -strip_interm : remove intermediate signals from result .vcd"
 puts "  by default: <output vcd name> = <instance name>.vcd"
 exit 1
}

if {$outvcdname==""} {
  set outvcdname "$figname.vcd"
}

vcd_Hierarchy $figname $vcdname $outvcdname $aliasfilename
