LDL = -ldl
BIN_EXT=

OSCFLAGS = -fno-inline

DYNAMIC = -rdynamic

FOP = /usr/bin/env fop
SAXON = /usr/bin/env saxon

ifndef FLEX_HEARTBEAT
FLEXOSLIBS = -lpthread
else
FLEXOSLIBS = 
endif

LICENSE_API =license_api
EDITLINELIBS = $(shell pkg-config --libs libedit)
