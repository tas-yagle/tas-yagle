LDL = -ldl
BIN_EXT=
DYNAMIC = 

GENDOCS = /users10/chaos1/avertec/gendocs

JAVA_HOME = $(GENDOCS)/Solaris/jre1.6.0_11
JAVA = $(JAVA_HOME)/bin/java

FOP = JAVA_HOME=$(JAVA_HOME);export JAVA_HOME;$(GENDOCS)/fop-0.95/fop
SAXON = $(JAVA) -jar $(GENDOCS)/saxonb9-1-0-2j/saxon9.jar

FLEXOSLIBS = -lpthread -lsocket -lnsl -lrt

ifdef AVT_COMPILATION_64BIT

CC               = /usr/local/gcc346_staticlibs/bin/gcc -m64
SCC              = /usr/local/gcc346_staticlibs/bin/gcc -m64
CPLUSPLUS        = /usr/local/gcc346_staticlibs/bin/g++ -m64

INCLUDE64BIT =  -I/usr/local/sparcv9/include
STATIC_LIB =    -L/usr/local/static_libs/sparcv9
else
STATIC_LIB =    -L/usr/local/static_libs

endif


LICENSE_API = license_api
