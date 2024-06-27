# -*- Mode: Makefile -*-
#
####---------------------------------------------------------###
# description	: Alliance include file for Makefiles
# architecture	: Linux avertec.lip6.fr 2.2.5-15 #2 ven oct 1 10:39:45 CEST 1999 i686 unknown
# date		: Mon Oct  4 17:38:18 CEST 1999
# file		: Linux.mk
#

# The variables $ALLIANCE_* are set by
# alc_env.[c]sh script or libraries.mk

PROGRAM_SUFFIX=

GNU_LIB         = /usr/local/lib
GNU_INCLUDE     = /usr/local/include

X11_LIB         = /usr/X11R6/lib 
X11_INCLUDE     = /usr/X11R6/include

MOTIF_LIB       = /usr/X11R6/lib
MOTIF_INCLUDE   = /usr/X11R6/include '-DXmNwrap="wrap"'

XPM_LIB         = /usr/X11R6/lib
XPM_INCLUDE     = /usr/X11R6/include

SHELL		    = /bin/sh
CSH		        = /bin/csh
CP		        = /bin/cp
CAT		        = /bin/cat
MV		        = /bin/mv
RM		        = /bin/rm
MKDIR		    = /bin/mkdir
FIND		    = /usr/bin/find
SED		        = /bin/sed
AWK		        = /bin/gawk
TR		        = /usr/bin/tr
TOUCH		    = /bin/touch
ECHO 		    = /bin/echo
STRIP		    = /usr/bin/strip
RANLIB		    = /usr/bin/ranlib

MAKE            = /usr/bin/make
MAKEFLAGS       = 

CC              = /usr/bin/gcc
SCC             = /usr/bin/gcc
CPLUSPLUS       = /usr/bin/g++
CFLAGS          =  
CPPFLAGS        = 

OPTIM           = 

ENABLE_STATIC   = -Xlinker -Bstatic
DISABLE_STATIC  = -Xlinker -Bdynamic

PURIFY          = purify

YACC            = /usr/bin/bison
YACCFLAGS       =

LEX             = /usr/bin/flex
LEXFLAGS        = 

AR              = /usr/bin/ar
ARFLAGS         = rv

SWIG            = /usr/local/bin/swig

WHOLE           = -Xlinker --whole-archive
NOWHOLE         = -Xlinker --no-whole-archive

TCL_L           = $(ENABLE_STATIC)  -ltcl85 $(DISABLE_STATIC)

# EOF
