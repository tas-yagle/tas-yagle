proc gns_sensamp {} {
  global writesig
  begCreateInterface
  
  begAddMemDriver "bl_in" "not prech" "'1'" 0 NULL
  begAddMemElse "bl_in" "not ext@$writesig and wlen0 and (bl0 or not bl0)" "bl0" 0 NULL
  begAddMemElse "bl_in" "not ext@$writesig and wlen1 and (bl1 or not bl1)" "bl1" 0 NULL
  begAddMemElse "bl_in" "ext@$writesig and blen" "bl" 0 NULL
  begAddMemElse "bl_in" "not ext@$writesig and toone and not tozero and (blb_in or not blb_in)" "not blb_in" 0 NULL
  begAddMemDriver "bl_in" "not ext@$writesig and prech and prech'event" "'z'" 0 NULL

  begAddMemDriver "blb_in" "not prech" "'1'" 0 NULL
  begAddMemElse "blb_in" "not ext@$writesig and wlen0 and (blb0 or not blb0)" "blb0" 0 NULL
  begAddMemElse "blb_in" "not ext@$writesig and wlen1 and (blb1 or not blb1)" "blb1" 0 NULL
  begAddMemElse "blb_in" "ext@$writesig and blen" "blb" 0 NULL
  begAddMemElse "blb_in" "not ext@$writesig and toone and not tozero and (bl_in or not bl_in)" "not bl_in" 0 NULL
  begAddMemDriver "blb_in" "not ext@$writesig and prech and prech'event" "'z'" 0 NULL

  
  begAddMemDriver "bl" "not ext@$writesig and blen" "bl_in" 0 NULL
  begAddMemDriver "blb" "not ext@$writesig and blen" "blb_in" 0 NULL

  begAddMemDriver "bl0" "wlen0 and ext@$writesig" "bl_in" 0 NULL
  begAddMemDriver "bl0" "not wlen0" "'z'" 0 NULL
  begAddMemDriver "blb0" "wlen0 and ext@$writesig" "blb_in" 0 NULL
  begAddMemDriver "blb0" "not wlen0" "'z'" 0 NULL
  
  begAddMemDriver "bl1" "wlen1 and ext@$writesig" "bl_in" 0 NULL
  begAddMemDriver "bl1" "not wlen1" "'z'" 0 NULL
  begAddMemDriver "blb1" "wlen1 and ext@$writesig" "blb_in" 0 NULL
  begAddMemDriver "blb1" "not wlen1" "'z'" 0 NULL

  begKeepModel
}
