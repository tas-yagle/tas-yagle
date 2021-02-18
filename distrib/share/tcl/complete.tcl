#	$LAAS: complete.tcl,v 1.2 2003/07/08 15:43:30 mallet Exp $

#
#  Copyright (c) 2001 LAAS/CNRS                       --  Tue Oct  9 2001
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

# Completion-related procedures

namespace eval el {

    # builtin default completion rules
    variable completionPatterns {
	{ ^(::)?after$ {p 1 {cancel idle info}} }
	{ ^(::)?array$
	    {p 1 {
		anymore donesearch exists get names nextelement
		set size startsearch unset 
	    }}
	    { p 2 A }
	}
	{ ^(::)?after$ {p 1 {format scan}} }
	{ ^(::)?catch$ {p 2 v} }
	{ ^(::)?cd$ {p 1 d} }
	{ ^(::)?clock$
	    {p 1 {format scan seconds}}
	    {N ^format$ {-format -gmt}}
	    {N ^scan$ {-base -gmt}}
	    {N ^-(format|base)$ {-gmt}}
	    {N ^-gmt$ {-format -base}}
	}
	{ ^(::)?close$ {p 1 {[file channels]}}}
	{ ^(::)?encoding$ {p 1 {convertfrom convertto names system}}}
	{ ^(::)?eof$ {p 1 {[file channels]}}}
	{ ^(::)?exec$
	    {C {@} {[file channels]}}
	    {C {<|>|&} F}
	    {p 1 {-keepnewline}}
	}
	{ ^(::)?l?append$ {p 1 v} }
	{ ^(::)?fblocked$ {p 1 {[file channels]}}}
	{ ^(::)?fconfigure$
	    {p 1 {[file channels]}}
	    {n ^-encoding$ {[encoding names]}}
	    {n ^-translation$ { auto binary cr crlf lf }}
	    {n ^- v}
	    {c {} {
		-blocking -buffering -buffersize -encoding
		-eofchar -translation
	    }}
	}
	{ ^(::)?fcopy$
	    {p 1|2 {[file channels]}}
	    {p 3|5 {-size -command}}
	}
	{ ^(::)?file$
	    {p 1 {
		atime attributes channels copy delete dirname executable
		exists extension isdirectory isfile join lstat mkdir
		mtime nativename owned pathtype readable readlink rename
		rename rootname size split stat tail type volume writable
	    }}
	    {n {^(copy|delete|rename)$} {-force --}}
	    {N ^atime$ {time}}
	    {N ^l?stat$ v}
	    {c {} F}
	}
	{ ^(::)?fileevent$
	    {p 1 {[file channels]}}
	    {p 2 {readable writable}}
	}
	{ ^(::)?flush$ {p 1 {[file channels]}}}
	{ ^(::)?foreach$ {p 1 v}}
	{ ^(::)?gets$ {p 1 {[file channels]}} {p 2 v}}
	{ ^(::)?glob$
	    {p 1 {-directory -join -nocomplain -path -types --}}
	    {n {^-(directory|path)$} d}
	    {C {[bcdflpsrwx] } {b c d f l p s r w x}}
	    {n {^-types$} {b c d f l p s r w x}}
	}
	{ ^(::)?global$ {p .* v}}
	{ ^(::)?history$ 
	    {p 1 {add change clear event info keep nextid redo}}
	}
	{ ^(::)?incr$ {p 1 v}}
	{ ^(::)?info$
	    {p 1 {
		args body cmdcount commands complete default exists
		globals hostname level library loaded locals
		nameofexecutable patchlevel procs script
		sharedlibextension tclversion vars 
	    }}
	    {n ^args|body|default$ p}
	}
	{ ^(::)?interp$
	    {p 1 {
		alias aliases create delete eval exists expose hide
		hidden invokehidden issafe marktrusted share slaves
		target transfer
	    }}
	    {p 2 i}
	    {n ^-global$ {[interp hidden]}}
	    {N ^invokehidden$ {-global}}
	    {N ^share|transfer$ {[file channels]}}
	    {N ^expose$ {[interp hidden]}}
	    {p 3 c}
	}
	{ ^(::)?load$
	    {p 1 F}
	    {p 3 {[interp slaves]}}
	}
	{ ^(::)?lsearch$ {p 1 {-exact -glob -regexp}}}
	{ ^(::)?lsort$
	    {p 1 {
		-ascii -dictionary -integer -real -command -increasing 
		-decreasing -index -unique
	    }}
	    {n ^-command$ c}
	}
	{ ^(::)?namespace$
	    { p 1 {
		children code current delete eval export forget import
		inscope origin parent qualifiers tail which
	    }}
	    {n ^code|origin$ c}
	    {n ^export$ {-clear}}
	    {n ^import$ {-force}}
	    {n ^qualifiers|tail$ N}
	    {n ^which$ {-command -variable}}
	    {n ^-command$ c}
	    {n ^-variable$ v}
	}
	{ ^(::)?open$
	    {p 1 F}
	    {p 2 {
		r r+ w w+ a a+
		RDONLY WRONLY RDWR APPEND CREAT EXCL NOCTTY NONBLOCK TRUNC
	    }}
	}
	{ ^(::)?package$
	    {p 1 {
		forget ifneeded names present provide require unknown
		vcompare versions vsatisfies
	    }}
	    {n ^present|require$ {[eval list -exact [package names]]}}
	    {n ^forget|ifneeded|versions$ k}
	    {n ^unknown$ c}
	}
	{ ^(::)?pid$ {p 1 {[file channels]}}}
	{ ^(::)?pkg::create$
	    {p 1 {-name -version -load -source}}
	    {n ^-name$ k}
	    {n ^-(load|source)$ F}
	}
	{ ^(::)?pkg_mkIndex$
	    {c ^- {-lazy -load -verbose --}}
	    {c {} d}
	}
	{ ^(::)?puts$
	    {c ^- {-nonewline}}
	    {p 1|2 {[file channels]}}
	}
	{ ^(::)?read$
	    {c ^- {-nonewline}}
	    {p 1|2 {[file channels]}}
	}
	{ ^(::)?regexp$
	    {c ^- {
		-about -expanded -indices -line -linestop -lineanchor
		-nocase -all -inline -start -- 
	    }}
	}
	{ ^(::)?regsub$ {c ^- {-nocase -all -start --}}}
	{ ^(::)?rename$ {p 1|2 c}}
	{ ^(::)?return$
	    {c ^- {-code -errorinfo -errorcode}}
	    {n ^-code$ {ok error return break continue}}
	}
	{ ^(::)?scan$ {p 3 v}}
	{ ^(::)?seek$
	    {p 1 {[file channels]}}
	    {p 3 {start current end}}
	}
	{ ^(::)?set$ {p 1 v}}
	{ ^(::)?signal$
	    {p 1 {[eval list names [signal names]]}}
	    {c {^-} {-default -ignore -block -unblock}}
	    {c {} c}
	}
	{ ^(::)?socket$ {c ^- {-myaddr -myport -async}}}
	{ ^(::)?source$ {p 1 F}}
	{ ^(::)?string$
	    {p 1 {
		bytelength compare equal first index is last length map
		match range repeat replace tolower totitle toupper trim
		trimleft trimright wordend wordstart
	    }}
	    {n ^is$ {
		alnum alpha ascii boolean control digit double false
		graph integer lower print punct space true upper wordchar
		xdigit 
	    }}
	    {n ^-failindex$ v}
	    {c ^- {-nocase -length -strict -failindex}}
	}
	{ ^(::)?subst$ {c ^- {-nobackslashes -nocommands -novariables}}}
	{ ^(::)?switch$ {c ^- {-exat -glob -regexp --}}}
	{ ^(::)?tell$ {p 1 {[file channels]}}}
	{ ^(::)?trace$
	    {p 1 {variable vdelete vinfo}}
	    {p 2 v}
	    {n ^variable|vdelete$ {r w u}}
	    {p 4 c}
	}
	{ ^(::)?unknown$ {p 1 c}}
	{ ^(::)?update$ {p 1 idletasks}}
	{ ^(::)?vwait$ {p 1 v}}
    }

    # This procedure is the core of the completion engine: it
    # generates a list of completion matches for the given string.
    # It must return a list made up of two intergers 'start' and 'end'
    # followed by 3-uplets { word append display } where 'word' is a
    # possible match, 'append' a string to append if the match is unique
    # and 'display' a string to append if there are several matches.
    # 'start' and 'end' are the indexes of the replacement text.
    proc matches { string } {
	variable completionPatterns

	# the matching rule (empty if none applies)
	set rule ""

	# just one weird case which is best worked around like this
	if {[string index $string end] == ":" &&
	    [string index $string end-1] != ":"} {
	    append string ":"
	}

	# break the string up into its constituent elements
	set parse [el::parse $string]
	set end [lindex $parse 2] 
	set items [lindex $parse 3]
	set nitems [llength $items]

	# get the last item: this is the one we want to complete on
	set last [lindex $items end]
	if { $nitems == 0 || $end != [lindex $last 3] } {
	    # add an empty token if we are at the beginning of a new word
	    set last [list simple-word "" [expr $end+1] [expr $end+1] \
			  [list [list text "" \
				     [expr $end+1] [expr $end+1] {} ]]]
	    lappend items $last
	    incr nitems
	}

	# get the namespace hierarchy
	set new [namespace qualifiers "[lindex $last 1]"]
	if { $new == "" } { set new "::" }
	while { $new != "" } {
	    set current $new
	    set new ""
	    foreach name $current { catch {
		eval lappend new [namespace children $name]
	    }}
	    eval lappend namespaces $new
	}

	# compute the kind of completion wanted, based on last item
	set lasttokens [lindex $last 4]
	set lasttoken [lindex $lasttokens end]
	set lasttokendescr [lindex [lindex $lasttoken 4] end]

	if { [lindex $lasttoken 0] == "variable" && 
	     [lindex $lasttokendescr 3] == [lindex $lasttoken 3]} {
	    # special case of an incomplete variable name (we known it is
	    # incomplete thanks to the length test above): just complete
	    # on variables that match
	    set vardescr [lindex $lasttoken 4]
	    if { [llength $vardescr] == 2 &&
		 [lindex [lindex $vardescr 0] 0] == "text" &&
		 [lindex [lindex $vardescr 1] 0] == "text" } {
		# array name
		set start [lindex [lindex $vardescr 1] 2]
		set end  [lindex [lindex $vardescr 1] 3]
		set name1 [lindex [lindex $vardescr 0] 1]
		set name2 [lindex [lindex $vardescr 1] 1]

		set completeon "array"

	    } else {
		# scalar variable (or weird variable name)
		set start [lindex [lindex $vardescr 0] 2]
		foreach component $vardescr {
		    append name1 [lindex $component 1]
		}
		set end [lindex $component 3]

		set completeon "variable namespace"
	    }

	} elseif {[lindex $lasttoken 0] == "text" && 
	    [lindex $lasttoken 1] == "$"} {
	    # special case of an empty $ sign: complete on variables and
	    # namespaces 
	    set start [expr [lindex $lasttoken 2]+1]
	    set end $start

	    set name1 ""
	    set completeon "variable namespace"

	} else {
	    # other cases: get the first item
	    set first [lindex $items 0]
	    set word [lindex $first 1]

	    # and the last word to complete: if it's a single token, and
	    # it begins with some quoting stuff (either \{ or \"), just
	    # strip the quoting char, unless the closing quote is also
	    # present (yeah... that's tricky)
	    if { [lindex $last 0] == "simple-word" } {
		set text [lindex [lindex $last 4] 0]
		if { [lindex $last 3] == [lindex $text 3] } {
		    set last $text
		}
	    }
	    set start [lindex $last 2]
	    set end [lindex $last 3]
	    set name1 [lindex $last 1]

	    # find the first completion rule that match
	    set rule ""
	    foreach test $completionPatterns {
		if {[regexp [lindex $test 0] $word]} {
		    set rule $test
		    break
		}
	    }

	    # if we've got one rule: see which of its subset applies
	    if { $rule != "" } {
		set rule [lreplace $rule 0 0]
		set subset ""
		foreach subitem $rule {
		    switch -regexp -- [lindex $subitem 0] {
			"C" {
			    # current word (append)
			    set pattern [lindex $subitem 1]
			    if {[regexp -indices -all -- \
				     $pattern $name1 pos]} {
				set name1 [string replace \
					       $name1 0 [lindex $pos 1]]
				incr start [expr 1+[lindex $pos 1]]
				set subset $subitem
				break
			    }
			}

			"c" {
			    # current word (replace)
			    set pattern [lindex $subitem 1]
			    if { [regexp -- $pattern $name1] } {
				set subset $subitem
				break
			    }
			}

			"n|N" {
			    # next word
			    if { [lindex $subitem 0] == "n" } {
				set count 1
			    } else {
				set count 2
			    }
			    if { $nitems>$count } {
				set word [lindex [lindex $items end-$count] 1]
				set pattern [lindex $subitem 1]
				if { [regexp -- $pattern $word] } {
				    set subset $subitem
				    break
				}
			    }
			}

			"p" {
			    # positional parameter
			    if { [regexp -- \
				      [lindex $subitem 1] \
				      [expr $nitems-1] match]} {
				if {"$match" == "[expr $nitems-1]"} {
				    set subset $subitem
				    break
				}
			    }
			}
		    }
		}

		if { $subset != "" } {
		    # determine action for this subset
		    set action [lindex $subset 2]
		    switch -glob -- $action {
			"A" { lappend completeon arrayname namespace }
			"v" { lappend completeon variable namespace }
			"n" { lappend completeon namespace }
			"N" { lappend completeon variable command namespace }
			"d" { lappend completeon directory }
			"f" { lappend completeon file }
			"F" { lappend completeon file directory }
			"c" { lappend completeon command namespace }
			"C" { lappend completeon command shell namespace }
			"p" { lappend completeon proc }
			"k" { lappend completeon package }
			"i" { lappend completeon slave }

			{\[*\]} {
			    lappend completeon "dictionary"
			    set dictionary [string range $action 1 end-1]
			    if {[catch {
				set dictionary [uplevel \#0 $dictionary]
			    }]} {
				error "bad completion rule \"$subset\"."
			    }
			}

			default {
			    lappend completeon "dictionary"
			    set dictionary $action
			}
		    }
		} else {
		    set rule ""
		}
	    }

	    if { $rule == "" } {
		# no matching rule
		# complete on commands for first item and variables
		# for others 
		if { $nitems == 1 } {
		    set completeon "command namespace"
		} else {
		    set completeon "variable namespace"
		}
	    }
	}

	# perform actual matching
	set matches ""

	# array element
	if { [lsearch $completeon "array"] >= 0 } {
	    if { $name1 == "" } {
		error "empty array name"
	    }

	    if { ![uplevel \#0 array exists [list [list $name1]]] } {
		error "no such array: $name1"
	    }

	    set list [uplevel \#0 array names [list [list $name1]]]
	    foreach match $list {
		if {[string match ${name2}* $match]} {
		    lappend matches "$match {) } {}"
		}
	    }
	}

	# array name
	if { [lsearch $completeon "arrayname"] >= 0 } {
	    set list [uplevel \#0 info vars [list [list ${name1}*]]]
	    if { [llength $list] == 0 } {
		# dig into children namespaces
		foreach namespace $namespaces {
		    eval lappend list \
			[uplevel \#0 info vars \
			     [list [list ${namespace}::${name1}*]]]
		}
	    }
	    foreach match $list {
		if { [uplevel \#0 array exists [list [list $match]]] } {
		    lappend matches "$match { } ()"
		}
	    }
	}

	# variable
	if { [lsearch $completeon "variable"] >= 0 } {
	    set list [uplevel \#0 info vars [list [list ${name1}*]]]
	    if { [llength $list] == 0 } {
		# dig into children namespaces
		foreach namespace $namespaces {
		    eval lappend list \
			[uplevel \#0 info vars \
			     [list [list ${namespace}::${name1}*]]]
		}
	    }
	    foreach match $list {
		if { [uplevel \#0 array exists [list [list $match]]] } {
		    lappend matches "$match ( ()"
		} else {
		    lappend matches "$match { } {}"
		}
	    }
	}

	# commands and procedures
	foreach corp { command proc } {
	    if { [lsearch $completeon $corp] >= 0 } {
		set list [uplevel \#0 info $corp [list [list ${name1}*]]]
		if { [llength $list] == 0 } {
		    # dig into children namespaces
		    foreach namespace $namespaces {
			eval lappend list \
			    [uplevel \#0 info $corp \
				 [list [list ${namespace}::${name1}*]]]
		    }
		}
		foreach match $list {
		    lappend matches "$match { } {}"
		}
	    }
	}
		
	# namespaces
	if { [lsearch $completeon "namespace"] >= 0 } {
	    foreach namespace $namespaces {
		if {[string match ${name1}* ${namespace}]} {
		    lappend matches "${namespace}:: {} {}"
		} elseif {[string match ::${name1}* ${namespace}]} {
		    lappend matches \
			"[string range ${namespace} 2 end]:: {} {}"
		}
	    }
	}

	# interpreters
	if { [lsearch $completeon "slave"] >= 0 } {
	    lappend completeon "dictionary"
	    set new [interp slaves]
	    eval lappend dictionary $new
	    while { $new != "" } {
		set current $new
		set new ""
		foreach name $current { catch {
		    foreach slave [interp slaves $name] {
			lappend new "{$name $slave}"
		    }
		}}
		eval lappend dictionary $new
	    }
	}

	# packages
	if { [lsearch $completeon "package"] >= 0 } {
	    lappend completeon "dictionary"
	    eval lappend dictionary [package names]
	}

	# dictionary (a given list of words)
	if { [lsearch $completeon "dictionary"] >= 0 } {
	    foreach word $dictionary {
		if {[string match ${name1}* $word]} {
		    lappend matches [list $word { } {}]
		}
	    }
	}

	# if we did not match anything at this point, force file and
	# directory or shell commands search, if they're not already
	# present and if not in a rule
	if { $rule == "" && [llength $matches] == 0 } { 
	    if { [lsearch $completeon "file"] < 0 &&
		 [lsearch $completeon "directory"] < 0 &&
		 [lsearch $completeon "shell"] < 0 } {
		if { $nitems == 1 } {
		    lappend completeon shell directory
		} else {
		    lappend completeon file directory
		}
	    }
	}

	# files
	if { [lsearch $completeon "file"] >= 0 } {
	    foreach file [glob -type "f" -nocomplain -- ${name1}*] {
		lappend matches "$file { } {}"
	    }
	    foreach file [glob -type "b c" -nocomplain -- ${name1}*] {
		lappend matches "$file { } {%}"
	    }
	    foreach file [glob -type "l" -nocomplain -- ${name1}*] {
		if { ![file isdirectory $file] } {
		    lappend matches "$file { } {@}"
		}
	    }
	}

	# directories
	if { [lsearch $completeon "directory"] >= 0 } {
	    foreach file [glob -type "d" -nocomplain -- ${name1}*] {
		lappend matches "$file {/} {/}"
	    }
	    foreach file [glob -type "l" -nocomplain -- ${name1}*] {
		if { [file isdirectory $file] } {
		    lappend matches "$file {/} {/@}"
		}
	    }
	}

	# shell commands
	if { [lsearch $completeon "shell"] >= 0 } {
	    global env

	    set findmatch {
		set list [glob -type "f l" -nocomplain -- $path]
		foreach file $list {
		    if { ! [file isdirectory $file] &&
			 [file readable $file] &&
			 [file executable $file]} {
			# must remove path
			set pos [string last $name1 $file]
			if { $pos >= 0 } {
			    lappend matches \
				"[string range $file $pos end] { } {*}"
			}
		    }
		}
	    }

	    if { [llength [file split ${name1}*]] == 1 } {
		foreach path [split $env(PATH) :] {
		    set path [file join $path ${name1}*]
		    eval $findmatch
		}
	    } else {
		set path ${name1}*
		eval $findmatch
	    }
	}

	# last thing: if the incomplete word has a namespace or directory
	# pattern which is present in all the matches, it is not
	# necessary to replace it (completion will just append characters
	# and not change the beginning of the word). This is at least the
	# way tcsh works. 

	foreach pattern { :: / } {
	    set slash [string last $pattern $name1]
	    if { $slash >= 0 } {
		# look if it is present everywhere
		set ok 1
		incr slash [string length $pattern]
		foreach match $matches {
		    set string [lindex $match 0]
		    if { ! [string equal -length $slash $name1 $match]} {
			set ok 0
			break
		    }
		}
		if { $ok } {
		    # ok so replace
		    set tmpmatches ""
		    foreach match $matches {
			set string [lindex $match 0]
			set match [lreplace $match 0 0 \
				       [string range $string $slash end]]
			lappend tmpmatches $match
		    }
		    set matches $tmpmatches
		    incr start $slash
		}
	    }
	}

	# sort the stuff and go home
	set matches [lsort -unique $matches]
	return "$start $end $matches"
    }
}

package provide el::complete 1.0
