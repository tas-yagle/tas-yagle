# -*- Mode: Makefile -*-
#
####---------------------------------------------------------###
# description	: Alliance include file for Makefiles
# architecture	: SunOS londres 5.6 Generic_105181-16 sun4u sparc SUNW,Ultra-5_10
# date		: lundi,  4 octobre 1999, 10:16:59 WET DST
# file		: Solaris.mk
#

# The variables $ALLIANCE_* are set by
# alc_env.[c]sh script or libraries.mk

PROGRAM_SUFFIX=

GNU_LIB          = /usr/local/lib
GNU_INCLUDE      = /usr/local/include

X11_LIB          = /usr/X11R6/lib/
X11_INCLUDE      = /usr/X11R6/include 

MOTIF_LIB        = /usr/OpenMotif/lib
MOTIF_INCLUDE    = /usr/OpenMotif/include

XPM_LIB          = /usr/X11R6/lib
XPM_INCLUDE      = /usr/X11R6/include

SHELL		= /bin/sh
CSH		= /bin/csh
CP		= /bin/cp
CAT		= /bin/cat
MV		= /bin/mv
RM		= /bin/rm
MKDIR		= /bin/mkdir
FIND		= /bin/find
SED		= /usr/bin/sed
AWK		= /usr/local/bin/gawk
TR		= /usr/bin/tr
TOUCH		= /usr/bin/touch
ECHO            = /bin/echo
STRIP		= /usr/bin/strip
RANLIB		= /usr/bin/ranlib

MAKE             = /usr/bin/make
MAKEFLAGS        = 

CC               = /usr/bin/gcc -L/usr/local/lib -I/usr/local/include -fno-common
SCC              = /usr/bin/gcc -I/usr/local/include -fno-common -Xlinker -undefined -Xlinker suppress -Xlinker -flat_namespace -dynamiclib
CPLUSPLUS        = /usr/bin/g++
CFLAGS           = -O3
CPPFLAGS         = 

OPTIM            = -O2 -Wall

PURIFY           = purify

YACC             = /usr/bin/bison
YACCFLAGS        = -y 

LEX              = /usr/local/bin/flex
LEXFLAGS         = 

AR               = /usr/bin/ar
ARFLAGS          = rv

SWIG             = /usr/local/bin/swig

TCL_L            = -ltcl8.5

WHOLE            = -all_load
NOWHOLE          = 

# EOF
