# makefile for router
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

ROU_VERSION = "1.0"
ROU_OBJ = router.o

NET_OBJ = netutil.o

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) -c $(LOCAL_INC) \
                  -DAVERTEC \
                  -D$(AVERTEC_LICENSE) \
                  -DROU_VERSION='$(ROU_VERSION)' \
                  -DAVT_H='<$(AVT_H)>' \
                  -DMUT_H='<$(MUT_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBHL_H='<$(BHL_H)>' \
                  -DBVL_H='<$(BVL_H)>' \
                  -DBEF_H='<$(BEF_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DMPU_H='<$(MPU_H)>' \
                  -DMPH_H='<$(MPH_H)>' \
                  -DSPE_H='<$(SPE_H)>' \
                  -DRCN_H='<$(RCN_H)>'

LOCAL_LIB  =  -L$(TARGET_LIB) \
             $(STB_L) \
             $(TTV_L) \
             $(TAS_L) \
             $(STM_L) \
             $(TRC_L) \
             $(TLC_L) \
             $(GEN_L) \
             $(FCL_L) \
             $(YAG_L) \
             $(MLU_L) \
             $(RCN_L) \
             $(MCL_L) \
             $(MAL_L) \
             $(MSL_L) \
             $(MGL_L) \
             $(MVL_L) \
             $(MEL_L) \
             $(MHL_L) \
             $(EQT_L) \
             $(MCC_L) \
             $(ELP_L) \
             $(TUT_L) \
             $(SIM_L) \
             $(EFG_L) \
             $(GSP_L) \
             $(CBH_L) \
             $(LIB_L) \
             $(TLF_L) \
             $(BEF_L) \
             $(BGL_L) \
             $(BVL_L) \
             $(BEG_L) \
             $(BHL_L) \
             $(BEH_L) \
             $(CNS_L) \
             $(LOG_L) \
             $(MLO_L) \
             $(SPF_L) \
             $(SPE_L) \
             $(INF_L) \
             $(MUT_L) \
             $(AVT_L) \
             $(API_L) \
             $(RPC_L) \
             -lm -ldl  

all : $(TARGET_BIN)/router $(TARGET_BIN)/netutil 

headers :

$(TARGET_BIN)/router : $(ROU_OBJ)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(ROU_OBJ) -o $(TARGET_BIN)/router $(FLEXOBJS) $(WHOLE) $(LOCAL_LIB) $(TCL_L) $(NOWHOLE) $(FLEXLIBS) -leditline -ltermcap -ldl
	$(STRIP) $(TARGET_BIN)/router

$(TARGET_BIN)/netutil : $(NET_OBJ)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(NET_OBJ) -o $(TARGET_BIN)/netutil $(FLEXOBJS) $(WHOLE) $(LOCAL_LIB) $(TCL_L) $(NOWHOLE) $(FLEXLIBS) -leditline -ltermcap -ldl
	$(STRIP) $(TARGET_BIN)/netutil

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/router $(TARGET_BIN)/netutil

clean :
	-$(RM) -f $(ROU_OBJ) $(NET_OBJ)

