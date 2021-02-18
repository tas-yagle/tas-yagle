# makefile for gsp
# date    : 17/07/01

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

GSP_HEADER     = gsp_lib.h

SHELL 		   = /bin/sh

GSP_OBJ_LIB    = gsp_util.o gsp_get_patterns.o

LOCAL_INC	   = -I$(TARGET_INCLUDE) \
                 -DGSP_H='"$(GSP_H)"' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
				 -DMLO_H='<$(MLO_H)>' \
				 -DMLU_H='<$(MLU_H)>' \
				 -DMSL_H='<$(MSL_H)>' \
				 -DLOG_H='<$(LOG_H)>' \
				 -DCBH_H='<$(CBH_H)>' \
				 -DBEH_H='<$(BEH_H)>' \
				 -DBEG_H='<$(BEG_H)>' \
				 -DINF_H='<$(INF_H)>' \
				 -DRCN_H='<$(RCN_H)>' \
				 -DELP_H='<$(ELP_H)>' \
				 -DEFG_H='<$(EFG_H)>' \
				 -DEQT_H='<$(EQT_H)>' \
				 -DCNS_H='<$(CNS_H)>' 


LOCAL_CFLAGS   = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC)
                                
all : $(TARGET_INCLUDE)/$(GSP_H) $(TARGET_LIB)/$(GSP_LIB)

headers : $(TARGET_INCLUDE)/$(GSP_H)

$(TARGET_LIB)/$(GSP_LIB) : $(GSP_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(GSP_LIB) $(GSP_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(GSP_LIB)
	
$(TARGET_INCLUDE)/$(GSP_H) : $(GSP_HEADER)
	$(CP) $(GSP_HEADER) $(TARGET_INCLUDE)/$(GSP_H)
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS)  $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(GSP_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(GSP_H)

clean : 
	-$(RM) -f $(GSP_OBJ) $(GSP_OBJ_LIB)  

	
