# makefile for x2v
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

X2V_OBJ = x2v_main.o

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) -c $(LOCAL_INC) \
                  -DAVERTEC \
                  -D$(AVERTEC_LICENSE) \
                  -DAVT_H='<$(AVT_H)>' \
                  -DMUT_H='<$(MUT_H)>' \
                  -DCNS_H='<$(CNS_H)>' \
                  -DGSP_H='<$(GSP_H)>' \
                  -DEFG_H='<$(EFG_H)>' \
                  -DMSL_H='<$(MSL_H)>' \
                  -DRCN_H='<$(RCN_H)>' \
                  -DTRC_H='<$(TRC_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DAPI_H='<$(API_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DMGL_H='<$(MGL_H)>' \
                  -DSIM_H='<$(SIM_H)>' \
                  -DELP_H='<$(ELP_H)>' \
                  -DINF_H='<$(INF_H)>' \
                  -DSTM_H='<$(STM_H)>' \
                  -DTTV_H='<$(TTV_H)>' \
                  -DBCK_H='<$(BCK_H)>' \
                  -DMCC_H='<$(MCC_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DCBH_H='<$(CBH_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DTUT_H='<$(TUT_H)>' \
                  -DTLF_H='<$(TLF_H)>' \
                  -DLIB_H='<$(LIB_H)>'

LOCAL_LIB  = $(FLEXOBJS) -L$(TARGET_LIB) \
             $(BCK_L) \
             $(TLF_L) \
             $(LIB_L) \
             $(TUT_L) \
             $(TTV_L) \
             $(STB_L) \
             $(TTV_L) \
             $(STM_L) \
             $(GEN_L) \
             $(TRC_L) \
             $(TLC_L) \
             $(CBH_L) \
             $(EQT_L) \
             $(BEF_L) \
             $(BVL_L) \
             $(BGL_L) \
             $(BEG_L) \
             $(BHL_L) \
             $(BEH_L) \
             $(LOG_L) \
             $(MLU_L) \
             $(MCL_L) \
             $(MAL_L) \
             $(MSL_L) \
             $(MVL_L) \
             $(MGL_L) \
             $(MEL_L) \
             $(MHL_L) \
             $(MLO_L) \
             $(SPF_L) \
             $(SPE_L) \
             $(RCN_L) \
             $(ELP_L) \
             $(MCC_L) \
             $(SIM_L) \
             $(EFG_L) \
             $(GSP_L) \
             $(CNS_L) \
             $(API_L) \
             $(INF_L) \
             $(AVT_L) \
             $(MUT_L) \
             $(RPC_L) \
             -lm $(FLEXLIBS) -ldl

all : $(TARGET_BIN)/x2v

$(TARGET_BIN)/x2v : $(X2V_OBJ) 
	$(CC) $(LOCAL_CFLAGS) $(X2V_OBJ) -o $(TARGET_BIN)/x2v $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/x2v

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/x2v

clean :
	-$(RM) -f $(X2V_OBJ)

