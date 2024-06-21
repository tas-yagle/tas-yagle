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

X11_LIB          = /usr/openwin/lib/
X11_INCLUDE      = /usr/openwin/include 

MOTIF_LIB        = /usr/dt/lib
MOTIF_INCLUDE    = /usr/dt/include

XPM_LIB          = /usr/local/lib/X11
XPM_INCLUDE      = /usr/local/include/X11

SHELL		= /bin/sh
CSH		= /bin/csh
CP		= /bin/cp
CAT		= /bin/cat
MV		= /bin/mv
RM		= /bin/rm
MKDIR		= /bin/mkdir
FIND		= /bin/find
SED		= /usr/ucb/sed
AWK		= /usr/local/bin/gawk
TR		= /usr/ucb/tr
TOUCH		= /usr/ucb/touch
ECHO            = /bin/echo
STRIP		= /usr/local/bin/strip
RANLIB		= /usr/local/bin/ranlib

MAKE             = /usr/local/bin/make
MAKEFLAGS        = 

CC               = /usr/local/gcc346_staticlibs/bin/gcc
SCC              = /usr/local/gcc346_staticlibs/bin/gcc
CPLUSPLUS        = /usr/local/gcc346_staticlibs/bin/g++
CFLAGS           = 
CPPFLAGS         = 

ENABLE_STATIC    = -Xlinker -Bstatic
DISABLE_STATIC    = -Xlinker -Bdynamic

OPTIM            = -O3

PURIFY           = purify

YACC             = /usr/local/bin/bison
YACCFLAGS        = -y 

LEX              = /usr/local/bin/flex
LEXFLAGS         = 

AR               = /usr/local/bin/ar
ARFLAGS          = rv

SWIG             = /usr/local/bin/swig

WHOLE            = -Xlinker -z -Xlinker allextract
NOWHOLE          = -Xlinker -z -Xlinker defaultextract

TCL_L            = $(ENABLE_STATIC) -ltcl8.5 $(DISABLE_STATIC) -lsocket -lnsl

# EOF
