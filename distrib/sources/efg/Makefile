# makefile for efg
# date    : 17/07/01

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

EFG_HEADER     = efg_lib.h

SHELL 		   = /bin/sh

EFG_OBJ_LIB    = efg_util.o efg_spisig.o efg_ext_fig.o efg_flatten.o

LOCAL_INC	   = -I$(TARGET_INCLUDE) \
                 -DEFG_H='"$(EFG_H)"' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DCNS_H='<$(CNS_H)>' 


LOCAL_CFLAGS   = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC)
                                
all : $(TARGET_INCLUDE)/$(EFG_H) $(TARGET_LIB)/$(EFG_LIB)

headers : $(TARGET_INCLUDE)/$(EFG_H)

$(TARGET_LIB)/$(EFG_LIB) : $(EFG_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(EFG_LIB) $(EFG_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(EFG_LIB)
	
$(TARGET_INCLUDE)/$(EFG_H) : $(EFG_HEADER)
	$(CP) $(EFG_HEADER) $(TARGET_INCLUDE)/$(EFG_H)
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS)  $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(EFG_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(EFG_H)

clean : 
	-$(RM) -f $(EFG_OBJ) $(EFG_OBJ_LIB)  

	
