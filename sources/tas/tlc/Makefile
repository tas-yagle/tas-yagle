# makefile for tlc libraries
# date    : 15/02/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

TLC_OBJ = tlc.o

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -D$(AVERTEC_OS) \
                 -DMLO_H='<$(MLO_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DTLC_H='"$(TLC_H)"'

all : $(TARGET_INCLUDE)/$(TLC_H) $(TARGET_LIB)/$(TLC_LIB)

headers : $(TARGET_INCLUDE)/$(TLC_H)

$(TARGET_INCLUDE)/$(TLC_H) : tlc_lib.h
	$(CP) tlc_lib.h $(TARGET_INCLUDE)/$(TLC_H)	

$(TARGET_LIB)/$(TLC_LIB) : $(TLC_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TLC_LIB) $(TLC_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(TLC_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<
 
realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TLC_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TLC_H)
 
clean :
	-$(RM) -f $(TLC_OBJ)

