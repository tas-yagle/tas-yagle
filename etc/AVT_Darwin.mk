LDL = -ldl
BIN_EXT=
DYNAMIC = 

FOP = /Users/anthony/dev/doctools/fop-0.95/fop
SAXON = /usr/bin/java -jar /Users/anthony/dev/doctools/saxonb9-1-0-2j/saxon9.jar

WITH_FLEXLM = NOFLEX
ifndef WITH_FLEXLM
FLEX = /Users/Shared/FlexNet/flexlm/v10.1/ppc_mac10

ifndef FLEX_INCLUDE
FLEX_INCLUDE = $(FLEX)/../machind
endif

FLEXOBJS = $(FLEX)/lm_new.o
FLEXLIBS = -L$(FLEX) -llmgr_nomt -lcrvs -lsb $(FLEXOSLIBS)
#FLEXLIBS = -L$(FLEX) -llmgr_as -llmgr_s -llmgr -lcrvs -lsb 
FLEXLM = $(FLEXOBJS) $(FLEXLIBS) 

FLEX_client = $(FLEX)/../machind/lmclient.h
FLEX_attr = $(FLEX)/../machind/lm_attr.h
WITH_FLEXLM = WITH_FLEXLM
endif

EDITLINELIBS = -ledit -ltermcap
LICENSE_API = license_api
