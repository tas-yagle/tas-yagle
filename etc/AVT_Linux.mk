LDL = -ldl
BIN_EXT=

OSCFLAGS = -fno-inline

DYNAMIC = -rdynamic

GENDOCS = /users10/chaos1/avertec/gendocs

FOP = export JAVA_HOME=$(JAVA_HOME);/usr/bin/fop


ifndef FLEX_HEARTBEAT
FLEXOSLIBS = -lpthread
else
FLEXOSLIBS = 
endif

LICENSE_API =license_api
EDITLINELIBS = $(shell pkg-config --libs libedit)
