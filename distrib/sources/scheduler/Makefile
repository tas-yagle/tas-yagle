# makefile for efg
# date    : 17/07/01

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SDLR_HEADER     = scheduler_lib.h

SHELL 		   = /bin/sh

OBJ_LIB    = Scheduler_transactions.o Scheduler_simulator.o
SOURCES_LIB=$(OBJ_LIB:.o=.c)

LOCAL_INC	   = -I$(TARGET_INCLUDE) \
                 -DMUT_H='<$(MUT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DSDLR_H='<$(SDLR_H)>' \
                 -DMLU_H='<$(MLU_H)>'

REGULAR_LIB 	= $(YAG_L) $(TAS_L) $(STB_L) $(RPC_L) $(SIM_L) \
				  $(GSP_L) $(EFG_L) $(MCC_L) $(CBH_L) $(EQT_L) \
				  $(TTV_L) $(STM_L) $(TRC_L) $(LIB_L) $(TLF_L) \
				  $(TUT_L) $(TLC_L) $(GEN_L) $(API_L) \
				  $(FCL_L) $(INF_L) $(CNS_L) $(ELP_L) $(XX1_L) \
				  $(AVT_L) $(MLU_L) $(MCL_L) $(MAL_L) $(MSL_L)\
				  $(MGL_L) $(MVL_L) $(MEL_L) $(MHL_L) $(MLO_L) \
				  $(SPF_L) $(SPE_L) $(BEF_L) $(BGL_L) $(BVL_L) \
				  $(BEG_L) $(BHL_L) $(BEH_L) $(LOG_L) $(RCN_L) \
				  $(MUT_L) $(SDLR_L)
				  
LIB =  $(FLEXOBJS) -L$(TARGET_LIB) $(WHOLE) $(REGULAR_LIB) -lm $(NOWHOLE) -ldl $(FLEXLIBS)

LOCAL_CFLAGS   = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC)
                                
all : $(TARGET_INCLUDE)/$(SDLR_H) $(TARGET_LIB)/$(SDLR_LIB)

headers : $(TARGET_INCLUDE)/$(SDLR_H)

$(TARGET_LIB)/$(SDLR_LIB) : $(OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(SDLR_LIB) $(OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(SDLR_LIB)
	
$(TARGET_INCLUDE)/$(SDLR_H) : $(SDLR_HEADER)
	$(CP) $(SDLR_HEADER) $(TARGET_INCLUDE)/$(SDLR_H)
	
.c.o : 
	$(CC) $(LOCAL_CFLAGS)  $(LOCAL_CPPFLAGS)  $<

test: $(TARGET_INCLUDE)/$(SDLR_H) $(TARGET_LIB)/$(SDLR_LIB) scheduler_test.o
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/schedulertest scheduler_test.o $(LIB)
	$(STRIP) $(TARGET_BIN)/scheduler_test
	
realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(SDLR_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(SDLR_H)

clean : 
	-$(RM) -f $(OBJ_LIB)  

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) scheduler_test.c > .depend

-include .depend	

