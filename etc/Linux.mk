# -*- Mode: Makefile -*-
#
####---------------------------------------------------------###
# description  : Alliance include file for Makefiles
# architecture : Linux avertec.lip6.fr 2.2.5-15 #2 ven oct 1 10:39:45 CEST 1999 i686 unknown
# date         : Mon Oct  4 17:38:18 CEST 1999
# file         : Linux.mk
#

# The variables $ALLIANCE_* are set by
# alc_env.[c]sh script or libraries.mk

UNAME_S = $(shell uname -s)
UNAME_R = $(shell uname -r)
UNAME_M = $(shell uname -m)

LIB_SUFFIX  = ""
LIB_SUFFIX_ = ""
ifeq ($(UNAME_M),x86_64)
  LIB_SUFFIX  = 64
  LIB_SUFFIX_ = _64
endif

BUILD_VARIANT = Linux
ifeq ($(UNAME_S),Linux)
  ifneq ($(findstring .el6.,$(UNAME_R)),)
    BUILD_VARIANT    = Linux.slsoc6x
  endif
  ifneq ($(findstring .slsoc6.,$(UNAME_R)),)
    BUILD_VARIANT    = Linux.slsoc6x
  endif
  ifneq ($(findstring .el7.,$(UNAME_R)),)
    BUILD_VARIANT    = Linux.el7
  endif
  ifneq ($(findstring ubuntu.,$(UNAME_R)),)
    BUILD_VARIANT    = Linux.ubuntu
  endif
endif


PROGRAM_SUFFIX   =

GNU_LIB          = /usr/lib
GNU_INCLUDE      = /usr/include

X11_LIB          = /usr/lib 
X11_INCLUDE      = /usr/include

MOTIF_LIB        = /usr/lib64 -L/usr/lib
MOTIF_INCLUDE    = /usr/include

XPM_LIB          = /usr/lib
XPM_INCLUDE      = /usr/include

SHELL            = /bin/sh
CSH              = /bin/csh
CP               = /bin/cp
CAT              = /bin/cat
MV               = /bin/mv
RM               = /bin/rm
MKDIR            = /bin/mkdir
FIND             = /usr/bin/find
SED              = /bin/sed
ifeq ($(findstring Ubuntu,$(shell uname -v)),Ubuntu)
AWK              = /usr/bin/awk
else			 
AWK              = /bin/awk
endif			 
TR               = /usr/bin/tr
TOUCH            = /bin/touch
ECHO             = /bin/echo
STRIP            = /usr/bin/strip
RANLIB           = /usr/bin/ranlib

MAKE             = /usr/bin/make
MAKEFLAGS        = 

CC               = /usr/bin/gcc -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
SCC              = /usr/bin/gcc -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
CPLUSPLUS        = /usr/bin/g++
CFLAGS           =
CPPFLAGS         =

ifeq ($(findstring Ubuntu,$(shell uname -v)),Ubuntu)
  CC            += -I/usr/include/tcl8.5 
  SCC           += -I/usr/include/tcl8.5 
  CPLUSPLUS     += -I/usr/include/tcl8.5 
endif

ifeq ($(PACKAGING_TOP),)
  CC            += -I${HOME}/softs/$(BUILD_VARIANT)$(LIB_SUFFIX_)/install/include
  SCC           += -I${HOME}/softs/$(BUILD_VARIANT)$(LIB_SUFFIX_)/install/include
  CPLUSPLUS     += -I${HOME}/softs/$(BUILD_VARIANT)$(LIB_SUFFIX_)/install/include
else
  CC            += -I${PACKAGING_TOP}/include
  SCC           += -I${PACKAGING_TOP}/include
  CPLUSPLUS     += -I${PACKAGING_TOP}/include
endif

ifeq ($(shell uname -m),x86_64)
  AVT_COMPILATION_64BIT = yes
endif

OPTIM            = -O3

ENABLE_STATIC    = -Xlinker -Bstatic
DISABLE_STATIC   = -Xlinker -Bdynamic

PURIFY           = purify

YACC             = /usr/bin/bison
YACCFLAGS        = -y 

#LEX             = flex
LEX              = ${HOME}/softs/$(BUILD_VARIANT)$(LIB_SUFFIX_)/install/bin/flex
LEXFLAGS         =

AR               = /usr/bin/ar
ARFLAGS          = rv

SWIG             = /usr/bin/swig

WHOLE            = -Xlinker --whole-archive
NOWHOLE          = -Xlinker --no-whole-archive

TCL_L            = -ltcl8.5

# EOF
