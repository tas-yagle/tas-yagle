LDL = -ldl
BIN_EXT=

OSCFLAGS = -fno-inline

DYNAMIC = -rdynamic

GENDOCS = /users10/chaos1/avertec/gendocs

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

FOP = export JAVA_HOME=$(JAVA_HOME);/usr/bin/fop


ifndef FLEX_HEARTBEAT
FLEXOSLIBS = -lpthread
else
FLEXOSLIBS = 
endif

LICENSE_API =license_api
EDITLINELIBS = "-ledit -ltermcap"
