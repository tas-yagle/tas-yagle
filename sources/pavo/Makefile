# makefile for pavo
# date    : 25/07/05

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

PAVO_HEADER     = pavo_lib.h

SHELL 		   = /bin/sh

PAVO_OBJ_LIB    = pavo_util.o Cone_Netlist_Simulator.o

LOCAL_INC	   = -I$(TARGET_INCLUDE) \
                 -DPAVO_H='"$(PAVO_H)"' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
				 -DEQT_H='<$(EQT_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DSDLR_H='<$(SDLR_H)>' \
                 -DCNS_H='<$(CNS_H)>' 


LOCAL_CFLAGS   = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC)
                                
all : $(TARGET_INCLUDE)/$(PAVO_H) $(TARGET_LIB)/$(PAVO_LIB)

headers : $(TARGET_INCLUDE)/$(PAVO_H)

$(TARGET_LIB)/$(PAVO_LIB) : $(PAVO_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(PAVO_LIB) $(PAVO_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(PAVO_LIB)
	
$(TARGET_INCLUDE)/$(PAVO_H) : $(PAVO_HEADER)
	$(CP) $(PAVO_HEADER) $(TARGET_INCLUDE)/$(PAVO_H)
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS)  $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(PAVO_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(PAVO_H)

clean : 
	-$(RM) -f $(PAVO_OBJ) $(PAVO_OBJ_LIB)  

	
