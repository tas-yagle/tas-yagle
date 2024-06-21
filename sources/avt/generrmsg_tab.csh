#!/bin/csh -f

set ERR_CFILE="errmsg.c"

echo "" >> $ERR_CFILE
echo "" >> $ERR_CFILE
echo "/***********************************/" >> $ERR_CFILE
echo "/*                                 */" >> $ERR_CFILE
echo "/*  generated part from avt_lib.h  */" >> $ERR_CFILE
echo "/*                                 */" >> $ERR_CFILE
echo "/* ! ! ! ! ! DO NOT EDIT ! ! ! ! ! */" >> $ERR_CFILE
echo "/*                                 */" >> $ERR_CFILE
echo "/***********************************/" >> $ERR_CFILE
echo "" >> $ERR_CFILE
echo "#include AVT_H" >> $ERR_CFILE
#echo '#include "errmsg.h"' >> $ERR_CFILE

echo "" >> $ERR_CFILE
echo "struct msgtab errmsg_tab[] = {" >> $ERR_CFILE

if ( $AVERTEC_OS == "Solaris" ) then
  cat avt_lib.h | awk -v f1="$ERR_CFILE" \
  '/.+_ERRMSG/ { split($2,array,"_") ; vu=sprintf("/usr/xpg4/bin/grep -q -w %s errmsg.xml",tolower($2)); \
               if ( system(vu) == 0) \
                 print "        { " $2 ", " tolower ($2)  "tab, \"" array[1] "\" }," >> f1; \
               else \
                 print "[1;32;41mMissing " $2 "[m";}'
else
  cat avt_lib.h | awk -v f1="$ERR_CFILE" \
  '/.+_ERRMSG/ { split($2,array,"_") ;  vu=sprintf("grep -q -w %s errmsg.xml",tolower($2)); \
               if ( system(vu) == 0) \
                 print "        { " $2 ", " tolower ($2)  "tab, \"" array[1] "\" }," >> f1; \
               else \
                 print "[1;32;41mMissing " $2 "[m";}'
endif

echo "        { 0,    NULL,    NULL }" >> $ERR_CFILE
echo "};" >> $ERR_CFILE
