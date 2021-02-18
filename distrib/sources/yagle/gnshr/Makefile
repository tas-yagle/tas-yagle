include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I./ -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC  -D$(AVERTEC_LICENSE) \
                 -DAVT_H='"$(AVT_H)"' \
                 -DAPI_H='"$(API_H)"' \
                 -DMUT_H='"$(MUT_H)"' \
                 -DMLO_H='"$(MLO_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DBEH_H='"$(BEH_H)"' \
                 -DBHL_H='"$(BHL_H)"' \
                 -DBVL_H='"$(BVL_H)"' \
                 -DBEF_H='"$(BEF_H)"' \
                 -DBEG_H='"$(BEG_H)"' \
                 -DLOG_H='"$(LOG_H)"' \
                 -DELP_H='"$(ELP_H)"' \
                 -DXX1_H='"$(XX1_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DTLC_H='"$(TLC_H)"' \
                 -DTRC_H='"$(TRC_H)"' \
                 -DSTM_H='"$(STM_H)"' \
                 -DTTV_H='"$(TTV_H)"' \
                 -DSTB_H='"$(STB_H)"' \
                 -DTAS_H='"$(TAS_H)"' \
                 -DLIB_H='"$(LIB_H)"' \
                 -DTLF_H='"$(TLF_H)"' \
                 -DTUT_H='"$(TUT_H)"' \
                 -DSIM_H='"$(SIM_H)"' \
                 -DGSP_H='"$(GSP_H)"' \
                 -DEFG_H='"$(EFG_H)"' \
                 -DCNS_H='"$(CNS_H)"' \
                 -DINF_H='"$(INF_H)"' \
                 -DFCL_H='"$(FCL_H)"' \
                 -DMSL_H='"$(MSL_H)"' \
                 -DRCN_H='"$(RCN_H)"' \
                 -DCBH_H='"$(CBH_H)"' \
                 -DGEN_H='"$(GEN_H)"' \
                 -DYAG_H='"$(YAG_H)"'

OBJ = gnshr.o

REGULAR_LIB 	= $(YAG_L) $(TAS_L) $(STB_L) $(RPC_L) $(SIM_L) \
				  $(GSP_L) $(EFG_L) $(MCC_L) $(CBH_L) $(EQT_L) \
				  $(TTV_L) $(STM_L) $(TRC_L) $(LIB_L) $(TLF_L) \
				  $(TUT_L) $(TLC_L) $(GEN_L) $(API_L) \
				  $(FCL_L) $(INF_L) $(CNS_L) $(ELP_L) $(XX1_L) \
				  $(AVT_L) $(MLU_L) $(MCL_L) $(MAL_L) $(MSL_L) \
				  $(MGL_L) $(MVL_L) $(MEL_L) $(MHL_L) $(MLO_L) \
				  $(SPF_L) $(SPE_L) $(BEF_L) $(BGL_L) $(BVL_L) \
				  $(BEG_L) $(BHL_L) $(BEH_L) $(LOG_L) $(RCN_L) \
				  $(MUT_L) 

LIB =  $(FLEXOBJS) $(WHOLE) -L$(TARGET_LIB) $(REGULAR_LIB) -lm $(NOWHOLE) -ldl $(FLEXLIBS) 

API_STATICLIB = -L$(TARGET_LIB) $(STATIC_LIB) $(WHOLE) $(REGULAR_LIB) $(APIALL_L) $(TCL_L) -lm $(NOWHOLE) -ldl
	  
all : $(TARGET_BIN)/gnshr

lib : $(TARGET_INCLUDE)/$(YAG_H) $(TARGET_LIB)/$(YAG_LIB)

headers : $(TARGET_INCLUDE)/$(YAG_H)

%.o : %.cc
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

$(TARGET_BIN)/gnshr : gnshr.h $(OBJ)
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/gnshr $(OBJ) $(LIB)
	$(STRIP) $(TARGET_BIN)/gnshr

api_link : 
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/gnshr $(OBJ) $(API_STATICLIB)
	$(STRIP) $(TARGET_BIN)/gnshr

link :
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/gnshr $(OBJ) $(LIB)
	$(STRIP) $(TARGET_BIN)/gnshr

$(TARGET_LIB)/$(YAG_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(YAG_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(YAG_LIB)

clean:
	-@$(RM) -f $(OBJ) .depend > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_BIN)/gnshr > /dev/null 2>&1

SOURCES_LIB=$(OBJ:.o=.cc)

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

