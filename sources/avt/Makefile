# makefile for avt libraries
# date    : 15/02/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk


SHELL = /bin/sh

AVTMAN      = avtman

AVT_OBJ_LIB = avt_error.o avt_env.o avt_banner.o avt_trace.o \
			  avt_trap.o avt_date.o errmsg.o avt_init_funcs.o

AVT_LIC_OBJ = avt_license_util.o

AVT_MAN_OBJ = avt_man.o
AVT_HEADER   = avt_lib.h
ERR_FILE = errmsg.c
XML_ERR = errmsg.xml

XML_ERR_FILE_LIST = $(shell $(CAT) $(XML_ERR) | $(AWK) '/ENTITY/ {split( $$4, t, "\""); print t[2] }')

LOCAL_INC = -I$(TARGET_INCLUDE) -I$(FLEX_INCLUDE) -I../api/tcl

LOCAL_CFLAGS = $(CFLAGS) 
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) $(EVAL_FLAG) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -D$(WITH_FLEXLM) \
                 -D$(FLEX_HEARTBEAT) \
                 -D$(AVERTEC_OS) \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DRPC_SVC_FG \
                 -DAVT_VERSION='$(AVT_VERSION)' \
                 -DPATCH_NUM='$(DISTRIB_PATCHVERSION)' \
                 -DAVT_DATE_A='$(AVT_DATE_A)' 
				 
LOCAL_LIB  = $(FLEXOBJS) -L$(TARGET_LIB) $(STATIC_LIB) \
             $(AVT_L) \
             $(MUT_L) \
             $(RPC_L) \
             -lm $(FLEXLIBS)

all : $(TARGET_INCLUDE)/$(AVT_H) $(ERR_FILE) $(TARGET_BIN)/$(AVTMAN)

headers : $(TARGET_INCLUDE)/$(AVT_H)

$(TARGET_BIN)/$(AVTMAN) : $(TARGET_LIB)/$(AVT_LIB) $(AVT_MAN_OBJ)
	$(CC) $(LOCAL_CFLAGS) $(AVT_MAN_OBJ) -o $(TARGET_BIN)/$(AVTMAN) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/$(AVTMAN)

$(TARGET_LIB)/$(AVT_LIB) : $(AVT_RPC) $(AVT_OBJ_LIB) $(AVT_LIC_OBJ) 
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(AVT_LIB) $(AVT_OBJ_LIB) $(AVT_LIC_OBJ)  
	$(RANLIB) $(TARGET_LIB)/$(AVT_LIB)

$(TARGET_INCLUDE)/$(AVT_H) : $(AVT_HEADER)
	$(CP) $(AVT_HEADER) $(TARGET_INCLUDE)/$(AVT_H)

$(ERR_FILE) : $(XML_ERR) $(AVT_HEADER) ./generrmsg_tab.sh $(XML_ERR_FILE_LIST)
	$(SAXON) $(XML_ERR) h.xsl > $(ERR_FILE)
	AWK='$(AWK)' ./generrmsg_tab.sh

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<
 
realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(AVT_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(AVT_H)
	-$(RM) -f $(TARGET_BIN)/$(AVTMAN)
 
clean :
	-$(RM) -f $(AVT_OBJ_LIB) $(AVT_LIC_OBJ) $(AVT_MAN_OBJ) $(AVT_ERR_OBJ) $(ERR_FILE)


