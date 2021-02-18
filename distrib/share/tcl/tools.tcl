#	$LAAS: tools.tcl,v 1.4 2005/10/03 21:10:43 matthieu Exp $

#
#  Copyright (c) 2001 LAAS/CNRS                       --  Wed Oct 10 2001
#  All rights reserved.                                    Anthony Mallet
#
#
# Redistribution  and  use in source   and binary forms,  with or without
# modification, are permitted provided that  the following conditions are
# met:
#
#   1. Redistributions  of  source code must  retain  the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must  reproduce the above copyright
#      notice,  this list of  conditions and  the following disclaimer in
#      the  documentation   and/or  other  materials   provided with  the
#      distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE  AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY  EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES   OF MERCHANTABILITY AND  FITNESS  FOR  A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO  EVENT SHALL THE AUTHOR OR  CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING,  BUT  NOT LIMITED TO, PROCUREMENT  OF
# SUBSTITUTE  GOODS OR SERVICES;  LOSS   OF  USE,  DATA, OR PROFITS;   OR
# BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

package require el::base 1.0

# shell's shift (should be added to tcl language :)
proc shift { varName } {
    upvar $varName l 
    set h [lindex $l 0]
    set l [lrange $l 1 end]
    return $h
}

# return the same list in reverse order
proc lreverse { list } {

    set reverse ""
    foreach e $list {
	set reverse [linsert $reverse -1 $e]
    }
    return $reverse
}

# aply command map on each element of list, returning the a new list 
proc lmap { list map } {
    set mapped {}
    foreach e $list {
	lappend mapped [eval $map $e]
    }
    return $mapped
}


# ask user interactively for input
proc aska { type prompt defaultValue args } {

    if { [llength $args] > 1 } {
	error "wrong \# args: should be \"aska type prompt default ?arg?\""
    }

    while { 1 } {

	# get supplied argument or ask user interactively
	if { [llength $args] == 0 } {

	    switch -regexp $type {
		{^(bool|short|int|float|double|string|char\s?\*)$} { }
		default {
		    set old $::el::completionPatterns
		    set ::el::completionPatterns \
			[list [list {} "c {} [list $type]"]]
		}
	    }
	    
	    set code [catch {
		set input [el::gets "$prompt \[$defaultValue\]> "]
	    } message ]

	    switch -regexp $type {
		{^(bool|short|int|float|double|string|char\s?\*)$} { }
		default { set ::el::completionPatterns $old }
	    }

	    if {$code} { error $message }

	} else {
	    set input $args
	}
	set args ""

	# user might want default value
	if {[catch {
	    if { "$input" == "" || "$input" == "."} {
		set input $defaultValue
	    }
	} message]} {
	    puts "$message"
	    continue
	}

	# check input validity
	set answer ""
	set expect $type
	set code [catch {
	    switch -regexp $type {
		"^bool" {
		    set expect "boolean"
		    if { $input } { set answer 1 } else { set answer 0 }
		}
	    
		"^(short|int)$" {
		    set expect "integer"
		    set value [uplevel \#0 expr $input]
		    set answer [expr int($value)]
		    if { $answer != $value } {
			error ""
		    }
		}

		"^(float|double)$" {
		    set answer [uplevel \#0 expr $input]
		}
	    
		{^(string|char\s?\*)$} {
		    set expect "string"
		    set answer [string trim [uplevel \#0 subst "$input"]]
		}
	    
		default {
		    set expect "enum \{$type\}"
		    set answer [string trim [uplevel \#0 subst "$input"]]
		    if { [lsearch $type $answer] < 0 } {
			error ""
		    }
		}
	    }
	} message]

	if { $code != 0 } {
	    puts "expecting $expect but got $input"
	    continue
	}
	
	break
    }

    return $answer
}


# returns a list scanned interactively with the supplied <format>
# where:
#  <format> = {  <arg> [<arg> [ ... ]] }
#  <arg> = { ?opt? type prompt default }
#  <cmdLine> = list of argument already present
#
proc mapscan { format cmdLine } {

    # check <format> structure
    foreach line $format {
	set usage \
	    "bad format $line: should be \"{ ?opt? type prompt default }\""
	if { [llength $line]<3 || [llength $line]>4 } {
	    error $usage
	}
	if { [llength $line] == 4 && "[lindex $line 0]" != "opt" } {
	    error $usage
	}
    }

    # those vars should have more explicit names, shouldn't they?
    set result {}
    set ad 0
    set v ""

    foreach line $format {
	if { "[lindex $line 0]" == "opt" } {
	    set o 1
	} else {
	    set o 0
	}

	set t [lindex $line $o]
	set p [lindex $line [expr $o + 1]]
	set d [lindex $line [expr $o + 2]]
	set nov 0
	
	if { "$v" == ".." || $ad } {
	    set v [lindex $line [expr $o + 2]]
	    set ad 1
	} elseif { [llength $cmdLine] == 0 } {
	    if { $o } {
		set v [lindex $line 3]
	    } else {
		set nov 1
	    }
	} else {
	    set v [shift cmdLine]
	}
	
	while { 1 } {
	    if { $nov } {
		set r [catch { 
		    uplevel \#0 aska [list $t] [list $p] [list $d]
		} m]
	    } else {
		set r [catch {
		    uplevel \#0 aska [list $t] [list $p] [list $d] [list $v]
		} m]
	    }
	    if { $r == 3 } { return -code return $m }
	    if { $r != 0 && ! $nov } { return -code error $m }
	    if { $r == 0 } { lappend result $m; break }
	    puts $m
	}
    }

    if { [llength $cmdLine] != 0 } {
	puts "warning: ignoring extra args \"$cmdLine\""
    }
    return $result
}


package provide el::tools 1.0

