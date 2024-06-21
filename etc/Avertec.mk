AVERTEC_OS=Linux

include $(AVERTEC_TOP)/etc/AVT_$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/Versions.mk

ifndef AVT_COMPILATION_TYPE
AVT_COMPILATION_TYPE = distrib
endif

ifndef AVT_COMPILATION_64BIT
FLEX_EXT=32
else
FLEX_EXT=64
endif

include $(AVERTEC_TOP)/etc/AVT_$(AVT_COMPILATION_TYPE).mk

ifndef AVT_DISTRIB_DIR
AVT_DISTRIB_DIR = $(AVERTEC_TOP)
endif

WITH_FLEXLM = NOFLEX
ifndef WITH_FLEXLM
ifndef AVT_OS
FLEX = /users10/chaos1/avertec/flexnet/flexlm/$(AVERTEC_OS)_$(FLEX_EXT)
else
FLEX = /users10/chaos1/avertec/flexnet/flexlm/$(AVT_OS)
endif

ifndef FLEX_INCLUDE
FLEX_INCLUDE = $(FLEX)/../machind
endif

FLEXOBJS = $(FLEX)/lm_new.o
ifndef FLEX_HEARTBEAT
FLEXLIBS = -L$(FLEX) -llmgr -lcrvs -lsb $(FLEXOSLIBS)
else
FLEXLIBS = -L$(FLEX) -llmgr_nomt -lcrvs -lsb $(FLEXOSLIBS)
endif
FLEXLM = $(FLEXOBJS) $(FLEXLIBS) 

FLEX_client = $(FLEX)/../machind/lmclient.h
FLEX_attr = $(FLEX)/../machind/lm_attr.h
WITH_FLEXLM = WITH_FLEXLM
endif

ifndef FLEX_HEARTBEAT
FLEX_HEARTBEAT = AUTO_HEARTBEAT
endif

TARGET_BIN       = $(AVT_DISTRIB_DIR)/bin
TARGET_LIB       = $(AVT_DISTRIB_DIR)/lib
TARGET_INCLUDE   = $(AVT_DISTRIB_DIR)/include

TARGET_TCL       = $(AVT_DISTRIB_DIR)/tcl

TARGET_API_LIB       = $(AVT_DISTRIB_DIR)/api_lib
TARGET_API_INCLUDE   = $(AVT_DISTRIB_DIR)/api_include
TARGET_API_MAN       = $(AVT_DISTRIB_DIR)/man/man3

