# makefile for VCD

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

VCD_HEADER   = vcd.h
VCD_H   = vcd100.h
VCD_LIB = libVcd100.a
VCD_L   = -lVcd100

EXPORT_VCD_LIB = ./libWScan100.a
EXPORT_VCD_H = ./wscan100.h
MBK_OBJ_LIB = ../mbk/mbk_ph.o       \
              ../mbk/mbk_ph_util.o  \
              ../mbk/mbk_lo.o       \
              ../mbk/mbk_lo_util.o  \
              ../mbk/mbk_sys.o      \
              ../mbk/rcn_lo.o       \
              ../mbk/rcn_lo_util.o  \
              ../mbk/rcn_cache.o    \
              ../mbk/mbk_util.o     \
              ../mbk/mbk_cache.o    \
              ../mbk/mbk_int.o      \
              ../mbk/mbk_advanced.o \
              ../mbk/mbk_math.o	    \
			  ../mbk/mbk_fileextract.o

VCD_OBJ_LIB = vcd_parse.o lex.vcd.o vcd.tab.o vcd_interface.o \
			  vcd_cdl_interface.o vcd_parse_core.o vcd_util.o vcd_map.o

VCD_SHELL = vcd_shell

SWIG_INC = /users/disk03/chaos/soft/Solaris/include
SWIG = /users/disk03/chaos/soft/Solaris/bin/swig

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_LIB  = -L$(TARGET_LIB) \
               $(VCD_L) \
               $(MUT_L) \
               -lm 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DVCD_H='"$(VCD_H)"' \
                 -DMUT_H='"$(MUT_H)"'
                 

LOCAL_YFLAGS = $(YACCFLAGS) -d # -t -v

lib : $(TARGET_INCLUDE)/$(VCD_H) $(TARGET_LIB)/$(VCD_LIB) 

exp : lib $(EXPORT_VCD_H) $(EXPORT_VCD_LIB)

shell : lib $(TARGET_BIN)/$(VCD_SHELL)

headers : $(TARGET_INCLUDE)/$(VCD_H)

# export lib

$(EXPORT_VCD_LIB) : $(VCD_OBJ_LIB) $(EXPORT_VCD_H)
	$(AR) $(ARFLAGS) $(EXPORT_VCD_LIB) $(VCD_OBJ_LIB) $(MBK_OBJ_LIB)
	$(RANLIB) $(EXPORT_VCD_LIB)
	$(STRIP) $(EXPORT_VCD_LIB)
	
$(EXPORT_VCD_H) :
	$(CP) vcdx.h $(EXPORT_VCD_H)

# lib

$(TARGET_LIB)/$(VCD_LIB) : $(VCD_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(VCD_LIB) $(VCD_OBJ_LIB) 
	$(RANLIB) $(TARGET_LIB)/$(VCD_LIB)
	
$(TARGET_INCLUDE)/$(VCD_H) :
	$(CP) $(VCD_HEADER) $(TARGET_INCLUDE)/$(VCD_H)

vcd.tab.h : vcd.tab.c

vcd.tab.c : vcd.y 
	$(YACC) $(LOCAL_YFLAGS) -b vcd -p vcd vcd.y 

lex.vcd.c : vcd.l vcd.tab.h
	$(LEX) $(LEXFLAGS) -Pvcd vcd.l 

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

# shell

TCL_WRAPPER = vcd_wrap.c
TCL_OBJ = vcd_wrap.o
TCL_INT = vcd.i
VCD_INT = vcd_interface.h

$(TARGET_BIN)/$(VCD_SHELL) : $(TCL_OBJ) $(TARGET_LIB)/$(VCD_LIB)
	$(CC) $(TCL_OBJ) $(LOCAL_LIB) -ltcl8.3 -lsocket -ldl -lnsl -o $(TARGET_BIN)/$(VCD_SHELL)

$(TCL_OBJ) : $(TCL_WRAPPER)
	$(CC) $(LOCAL_CFLAGS) -I$(SWIG_INC) $(LOCAL_CPPFLAGS) $(TCL_WRAPPER)

$(TCL_WRAPPER): $(TCL_INT) $(VCD_INT)
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 -ltclsh.i $(TCL_INT)

# clean

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(VCD_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(VCD_H)
	-$(RM) -f $(EXPORT_VCD_LIB) $(EXPORT_VCD_H)

clean : 
	-$(RM) -f $(VCD_OBJ_LIB) lex.vcd.c vcd.tab.c vcd.tab.h vcd.output
	-$(RM) -f $(TARGET_BIN)/$(VCD_SHELL) $(TCL_OBJ) $(TCL_WRAPPER)
	
