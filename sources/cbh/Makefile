include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

CBH_HEADER  = cbh_lib.h 

LOCAL_CFLAGS  = $(CFLAGS) -c
LOCAL_ARFLAGS = $(ARFLAGS)

LOCAL_INCLUDE = -I. -I$(TARGET_INCLUDE)

LOCAL_CPPFLAGS   = $(CPPFLAGS) $(LOCAL_INCLUDE) \
                  -DMUT_H='<$(MUT_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DMLU_H='<$(MLU_H)>' \
                  -DAPI_H='<$(API_H)>' \
                  -DMSL_H='<$(MSL_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DINF_H='<$(INF_H)>' \
                  -DAVT_H='<$(AVT_H)>' \
                  -DELP_H='<$(ELP_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DCNS_H='<$(CNS_H)>' \
                  -DFCL_H='<$(FCL_H)>' \
                  -DYAG_H='<$(YAG_H)>' \
                  -DGEN_H='<$(GEN_H)>' \
                  -DRCN_H='<$(RCN_H)>' \
                  -DTLC_H='<$(TLC_H)>' \
                  -DTRC_H='<$(TRC_H)>' \
                  -DSTM_H='<$(STM_H)>' \
                  -DTTV_H='<$(TTV_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DCBH_H='<$(CBH_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DMCC_H='<$(MCC_H)>' \
                  -DEFG_H='<$(EFG_H)>' \
                  -DGSP_H='<$(GSP_H)>' \
                  -DSIM_H='<$(SIM_H)>' \
                  -DTUT_H='"$(TUT_H)"' \
                  -DLIB_H='"$(LIB_H)"' \
                  -DTLF_H='"$(TLF_H)"' \
                  -DSTB_H='"$(STB_H)"' \
                  -DTAS_H='"$(TAS_H)"' \
                  -DBHL_H='<$(BHL_H)>' \
                  -DBVL_H='<$(BVL_H)>' \
                  -DBGL_H='<$(BGL_H)>' \
                  -DBEF_H='<$(BEF_H)>' \
                  -DSPE_H='<$(SPE_H)>' \
                  -DSPF_H='<$(SPF_H)>' 

LOCAL_OBJ     = cbh_util.o cbh_seq.o cbh_comb.o cbh_cmp.o cbh_env.o cbh_sim.o \
				cbh_error.o

LIB = -L$(TARGET_LIB) $(CBH_L) \
      $(AVT_L) \
      $(MLU_L) $(MCL_L) $(MAL_L) $(MHL_L) $(MVL_L) $(MEL_L) $(MSL_L) $(MGL_L) \
      $(RCN_L) $(MLO_L) $(BEF_L) $(BGL_L) $(BVL_L) $(BHL_L) $(BEH_L) $(LOG_L) $(MUT_L) \
      -lm

all : $(TARGET_INCLUDE)/$(CBH_H) $(TARGET_LIB)/$(CBH_LIB)

headers : $(TARGET_INCLUDE)/$(CBH_H)

.c.o:
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

$(TARGET_LIB)/$(CBH_LIB) : $(LOCAL_OBJ) $(CBH_HEADER)
	$(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(CBH_LIB) $(LOCAL_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(CBH_LIB)

$(TARGET_INCLUDE)/$(CBH_H) : $(CBH_HEADER)
	$(CP) $(CBH_HEADER) $(TARGET_INCLUDE)/$(CBH_H)

test : $(TARGET_BIN)/cbhtest

$(TARGET_BIN)/cbhtest : $(TARGET_LIB)/$(CBH_LIB) cbh_main.o
	$(CC) -o $(TARGET_BIN)/cbhtest cbh_main.o $(LIB)
	$(STRIP) $(TARGET_BIN)/cbhtest

clean :
	-@$(RM) -f $(LOCAL_OBJ) cbh_main.o > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(CBH_LIB) $(TARGET_INCLUDE)/$(CBH_H) $(TARBET_BIN)/cbhtest > /dev/null 2>&1

