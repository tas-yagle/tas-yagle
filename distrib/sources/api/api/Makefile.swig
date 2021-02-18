# makefile for api
# date    : 16/07/2002

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

API_HEADER = api100.h
API_LIB = libApi100.a
API_BIN = genapi

WRP_BIN = wrpgen

SWIG = ./SWIG1.3a5/swig

LSWIG_DIR = -L./SWIG1.3a5/Source/Swig/
LSWIG11_DIR = -L./SWIG1.3a5/Source/SWIG1.1/
LCPP_DIR = -L./SWIG1.3a5/Source/Preprocessor
LDOH_DIR = -L./SWIG1.3a5/Source/DOH

ISWIG_DIR = -I./SWIG1.3a5/Source/Swig/
ISWIG11_DIR = -I./SWIG1.3a5/Source/SWIG1.1/
ICPP_DIR = -I./SWIG1.3a5/Source/Preprocessor
IDOH_DIR = -I./SWIG1.3a5/Source/DOH/Include/
I_DIR = -I./SWIG1.3a5/Source/Include

SHELL = /bin/sh
WRP_INC = $(ISWIG_DIR) $(ISWIG11_DIR) $(ICPP_DIR) $(IDOH_DIR) $(I_DIR)
WRP_LIB = $(LSWIG_DIR) $(LSWIG11_DIR) $(LCPP_DIR) $(LDOH_DIR) -lswig11 -lcpp -lswig -ldoh
WRP_OBJ = api_wrp_gen.o api_wrp_gns.o
WRP_CPPFLAGS = $(CPPFLAGS) $(WRP_INC) -DUSE_SWIG

API_OBJ = api_util.o

$(TARGET_BIN)/$(WRP_BIN): $(SWIG) $(WRP_OBJ) $(TARGET_LIB)/$(API_LIB)
	$(CPLUSPLUS) $(CFLAGS) $(WRP_OBJ) $(WRP_INC) $(WRP_LIB) -o $(TARGET_BIN)/$(WRP_BIN)

$(SWIG):
	cd ./SWIG1.3a5/; configure; $(MAKE)
	
$(TARGET_LIB)/$(API_LIB): $(API_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(API_LIB) $(API_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(API_LIB)
	$(CP) $(API_HEADER) $(TARGET_INCLUDE)/
	$(CP) $(API_BIN) $(TARGET_BIN)
		 
.cc.o:
	$(CPLUSPLUS) $(WRP_CPPFLAGS) $(CFLAGS) -c $<

.c.o:
	$(CC) $(WRP_CPPFLAGS) $(CFLAGS) -c $<

clean:
	-$(RM) -f $(WRP_OBJ) $(API_OBJ)

realclean: clean
	-$(RM) -f $(TARGET_BIN)/$(WRP_BIN)
	-$(RM) -f $(TARGET_BIN)/$(API_BIN)
	-$(RM) -f $(TARGET_LIB)/$(API_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(API_HEADER)
	$(MAKE) -C ./SWIG1.3a5/ clean
	cd ./SWIG1.3a5/; $(RM) -f config.*
