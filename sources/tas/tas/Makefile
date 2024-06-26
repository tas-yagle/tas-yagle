# makefile for hitas 

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

TAS_OBJ = tas.o
MCC_GENCURV_OBJ = mcc_genelp_main.o
MCCGENELP = genelp

TAS_OBJ_LIB = tas_lut.o tas_util.o tas_class.o tas_drislo.o tas_spice.o \
              tas_visu.o tas_err.o tas_tec.o tas_pre.o \
              tas_tp.o tas_tpiv.o tas_alloc.o tas_parscns.o tas_builthfig.o tas_main.o \
              tas_noise.o tas_models.o tas_cnsmemory.o

TAS_HEADER   = tas_lib.h

LOCAL_INC = -I$(TARGET_INCLUDE) 

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) -c $(LOCAL_INC) \
                  -DAVERTEC \
                  -DAVT_VERSION='$(AVT_VERSION)' \
				  -D$(AVERTEC_OS) \
                  -D$(AVERTEC_LICENSE) \
                  -DDEFAULTIN='"spi"' \
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
                  -DBCK_H='<$(BCK_H)>' \
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
                  -DHITAS_VERSION='$(HITAS_VERSION)' \
                  -DAVT_DATE_A='$(AVT_DATE_A)' 
                  
REGULAR_LIB = $(TAS_L) \
              $(STB_L) \
              $(LIB_L) \
              $(TLF_L) \
              $(TUT_L) \
              $(SIM_L) \
              $(GSP_L) \
              $(EFG_L) \
              $(MCC_L) \
              $(CBH_L) \
              $(EQT_L) \
              $(BCK_L) \
              $(TTV_L) \
              $(STM_L) \
              $(TRC_L) \
              $(TLC_L) \
              $(YAG_L) \
              $(GEN_L) \
              $(API_L) \
              $(FCL_L) \
              $(INF_L) \
              $(CNS_L) \
              $(ELP_L) \
              $(AVT_L) \
              $(MLU_L) \
              $(MCL_L) \
              $(MAL_L) \
              $(MSL_L) \
              $(MGL_L) \
              $(MVL_L) \
              $(MEL_L) \
              $(MHL_L) \
              $(MLO_L) \
              $(SPF_L) \
              $(SPE_L) \
              $(BEF_L) \
              $(BGL_L) \
              $(BVL_L) \
              $(BEG_L) \
              $(BHL_L) \
              $(BEH_L) \
              $(LOG_L) \
              $(RCN_L) \
              $(MUT_L) \
              $(RPC_L) \

LOCAL_LIB  = $(FLEXOBJS) -L$(TARGET_LIB) $(WHOLE) $(REGULAR_LIB) $(TCL_L) $(EDITLINELIBS) -ldl -lm $(NOWHOLE) $(FLEXLIBS)
API_STATICLIB  = $(FLEXOBJS) -L$(TARGET_LIB) $(STATIC_LIB) $(WHOLE) $(REGULAR_LIB) $(TCL_L) $(APIALL_L) $(NOWHOLE) $(EDITLINELIBS) -ldl -lm $(FLEXLIBS)

all : lib

bin : $(TARGET_BIN)/$(MCCGENELP) $(TARGET_BIN)/hitas

headers : $(TARGET_INCLUDE)/$(TAS_H)

$(TARGET_BIN)/hitas : $(TARGET_INCLUDE)/$(TAS_H) $(TARGET_LIB)/$(TAS_LIB) $(TAS_OBJ) 
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(DYNAMIC) $(TAS_OBJ) -o $(TARGET_BIN)/hitas $(LOCAL_LIB) 
	$(STRIP) $(TARGET_BIN)/hitas

$(TARGET_BIN)/$(MCCGENELP) : $(TARGET_LIB)/$(MCC_LIB) $(MCC_GENCURV_OBJ) $(TAS_OBJ) $(TARGET_LIB)/$(TAS_LIB)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(MCC_GENCURV_OBJ) -o $(TARGET_BIN)/$(MCCGENELP) $(LOCAL_LIB) 
	$(STRIP) $(TARGET_BIN)/$(MCCGENELP)

link: $(TARGET_BIN)/hitas

api_link: $(TARGET_INCLUDE)/$(TAS_H) $(TARGET_LIB)/$(TAS_LIB) $(TAS_OBJ)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(DYNAMIC) $(TAS_OBJ) -o $(TARGET_BIN)/hitas $(API_STATICLIB) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/hitas

lib: $(TARGET_INCLUDE)/$(TAS_H) $(TARGET_LIB)/$(TAS_LIB)

$(TARGET_LIB)/$(TAS_LIB) : $(TAS_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TAS_LIB) $(TAS_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(TAS_LIB)

$(TARGET_INCLUDE)/$(TAS_H) : $(TAS_HEADER)
	$(CP) $(TAS_HEADER) $(TARGET_INCLUDE)/$(TAS_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/hitas
	-$(RM) -f $(TARGET_LIB)/$(TAS_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TAS_H)

clean :
	-$(RM) -f $(TAS_OBJ) $(TAS_OBJ_LIB)

