# makefile for tbg
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

TBG_INSTALL_VERSION = 100

TBG_HEADER   = tbg$(TBG_INSTALL_VERSION).h

TBG_H   = tbg$(TBG_INSTALL_VERSION).h
TBG_L   = -lTbg$(TBG_INSTALL_VERSION)
TBG_LIB = libTbg$(TBG_INSTALL_VERSION).a
TBG_SHELL = gentest

SHELL = /bin/sh

TBG_OBJ_LIB = tbg_Assign.o \
              tbg_Design.o \
              tbg_Period.o \
              tbg_Options.o \
              tbg_GenerateTB.o \
              tbg_ExecuteCycle.o \
              tbg_ValMax.o \
              tbg_Check.o \
			  tbg_GetPort.o \
              tbg.o \
              lex.vhdl.o \
              y.tab.o

LOCAL_YFLAGS = $(YACCFLAGS) -d  -t -v

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>'

# bin
$(TARGET_BIN)/$(TBG_SHELL) : $(TBG_SHELL) $(TARGET_LIB)/$(TBG_LIB)
	$(CP) $(TBG_SHELL) $(TARGET_BIN)/
	chmod +x $(TARGET_BIN)/$(TBG_SHELL)

# lib

headers :

$(TARGET_LIB)/$(TBG_LIB) : $(TBG_H) $(TBG_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TBG_LIB) $(TBG_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(TBG_LIB)
	$(CP) $(TBG_HEADER) $(TARGET_INCLUDE)/
	
# obj

y.tab.h : y.tab.c

y.tab.c : vhdl.y
	$(YACC) $(LOCAL_YFLAGS) -p vhdl vhdl.y

lex.vhdl.c : vhdl.l y.tab.h
	$(LEX) $(LEXFLAGS) -Pvhdl vhdl.l

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TBG_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TBG_HEADER)
	-$(RM) -f $(TARGET_BIN)/$(TBG_SHELL)

clean : 
	-$(RM) -f $(TBG_OBJ_LIB) lex.vhdl.c y.tab.h y.tab.c y.output
	
