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
OS_RELEASE = $(shell awk -F= '/^NAME/{print $2}' /etc/os-release)

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
  ifneq ($(findstring Ubuntu.,$(OS_RELEASE)),)
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
FIND             = /usr/bin/env find
SED              = /usr/bin/env sed
AWK              = /usr/bin/env awk
TR               = /usr/bin/env tr
TOUCH            = /bin/touch
ECHO             = /bin/echo
STRIP            = /usr/bin/env strip
RANLIB           = /usr/bin/env ranlib

MAKE             = /usr/bin/env make
MAKEFLAGS        =

CC               = /usr/bin/env gcc -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
SCC              = /usr/bin/env gcc -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
CPLUSPLUS        = /usr/bin/env g++
CFLAGS           = -I${PACKAGING_TOP}/include
CPPFLAGS         =

TCL_H            = $(shell pkg-config --cflags tcl)
TCL_L            = $(shell pkg-config --libs tcl)

ifeq ($(shell uname -m),x86_64)
  AVT_COMPILATION_64BIT = yes
endif

ifeq ($(shell uname -m),aarch64)
  AVT_COMPILATION_64BIT = yes
endif

ifeq ($(shell uname -m),riscv64)
  AVT_COMPILATION_64BIT = yes
endif



OPTIM            = -O3

ENABLE_STATIC    = -Xlinker -Bstatic
DISABLE_STATIC   = -Xlinker -Bdynamic

PURIFY           = purify

YACC             = /usr/bin/env bison
YACCFLAGS        = -y 

LEX              = /usr/bin/env flex
LEXFLAGS         =

AR               = /usr/bin/env ar
ARFLAGS          = rv

SWIG             = /usr/bin/env swig

WHOLE            = -Xlinker --whole-archive
NOWHOLE          = -Xlinker --no-whole-archive


ifeq ($(BUILD_VARIANT)$(LIB_SUFFIX_),Linux.slsoc6x_64)
  JAVA_HOME = /usr/lib/jvm/java-1.6.0-openjdk.x86_64
  JAVA      = $(JAVA_HOME)/bin/java
  SAXON     = $(JAVA) -jar /usr/share/java/saxon9.jar
else
  ifeq ($(BUILD_VARIANT)$(LIB_SUFFIX_),Linux.slsoc6x)
    JAVA_HOME = /usr/lib/jvm/java-1.6.0-openjdk
    JAVA      = $(JAVA_HOME)/bin/java
    SAXON     = $(JAVA) -jar /usr/share/java/saxon9.jar
  else
    ifeq ($(BUILD_VARIANT),Linux.el7)
      JAVA_HOME = /usr/lib/jvm/java-1.7.0-openjdk
      JAVA      = $(JAVA_HOME)/bin/java
      SAXON     = $(JAVA) -jar /usr/share/java/saxon.jar
    else
      ifeq ($(BUILD_VARIANT),Linux.ubuntu)
	JAVA_HOME = $(shell dirname $$(dirname $$(update-alternatives --list javac 2>&1 | head -n 1)))
        JAVA      = $(JAVA_HOME)/bin/java
        SAXON     = saxonb-xslt -ext:on
       #SAXON     = CLASSPATH=/usr/share/java/saxonb.jar $(JAVA) net.sf.saxon.Transform
      else
        JAVA_HOME = /usr/lib/jvm/java-1.6.0-openjdk
        JAVA      = $(JAVA_HOME)/bin/java
        SAXON     = $(JAVA) -jar /usr/share/java/saxon9.jar
      endif
    endif
  endif
endif

RPC_L = -lcrypt
RPCGEN = /usr/bin/env rpcgen
RPCGENFLAGS = -C

LDL = -ldl
BIN_EXT=

OSCFLAGS = -fno-inline

DYNAMIC = -rdynamic

GENDOCS = /users10/chaos1/avertec/gendocs

FOP = export JAVA_HOME=$(JAVA_HOME);/usr/bin/fop


ifndef FLEX_HEARTBEAT
FLEXOSLIBS = -lpthread
endif

# EOF
