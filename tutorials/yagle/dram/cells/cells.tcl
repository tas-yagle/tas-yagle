
proc cells {} {
  global writesig
  begCreateInterface
  begAddMemDriver "mem00" "ext@$writesig and wl0 and (bl0_up xor blb0_up)" "bl0_up" 0 "fastwrite_delay"
  begAddMemDriver "mem01" "ext@$writesig and wl1 and (bl0_up xor blb0_up)" "blb0_up" 0 "slowwrite_delay"
  begAddMemDriver "mem10" "ext@$writesig and wl0 and (bl1_dn xor blb1_dn)" "bl1_dn" 0 "slowwrite_delay"
  begAddMemDriver "mem11" "ext@$writesig and wl1 and (bl1_dn xor blb1_dn)" "blb1_dn" 0 "fastwrite_delay"

  begAddMemDriver "bl0_up" "not ext@$writesig and wl0" "mem00" 0 "fastread_delay"
  begAddMemDriver "blb0_up" "not ext@$writesig and wl1" "mem01" 0 "slowread_delay"
  begAddMemDriver "bl1_dn" "not ext@$writesig and wl0" "mem10" 0 "slowread_delay"
  begAddMemDriver "blb1_dn" "not ext@$writesig and wl1" "mem11" 0 "fastread_delay"
  begKeepModel
}
