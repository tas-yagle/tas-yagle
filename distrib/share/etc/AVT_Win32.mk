RPC_L = -lrpc -lcrypt
RPCGEN = /usr/bin/rpcgen
LDL = 
RPCGENFLAGS =

OSCFLAGS = -fno-inline
BIN_EXT =.exe

DYNAMIC = -dynamic

JAVA = /cygdrive/c/WINDOWS/system32/java
#JAVA = /usr/java/j2re1.4.2_08/bin/java
#JAVA = /usr/java/j2re1.4.2_03/bin/java


#FOP = /users/disk02/avertec/gendocs/Linux/Fop
FOP = /home/antony/ressources/Fop

#FOP_HOME = /users/disk02/avertec/gendocs/fop_home
FOP_HOME = .

SAXON = $(JAVA) -jar `cygpath -wp $(HOME)/saxon8.jar`

WITH_FLEXLM = NOFLEX
FLEXLIBS = -lpthread

LICENSE_API = license_api.exe
