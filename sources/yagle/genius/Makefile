include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC      = -I. -I../yagle -I$(TARGET_INCLUDE)

LOCAL_YACCFLAGS = $(YACCFLAGS) -d  
LOCAL_CFLAGS   =  $(CFLAGS) $(TCL_H)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) \
                 -DAVT_H='"$(AVT_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DMLO_H='"$(MLO_H)"' \
                 -DLOG_H='"$(LOG_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DELP_H='"$(ELP_H)"' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DBVL_H='"$(BVL_H)"' \
                 -DBEH_H='"$(BEH_H)"' \
                 -DBHL_H='"$(BHL_H)"' \
                 -DBEG_H='"$(BEG_H)"' \
                 -DCBH_H='"$(CBH_H)"' \
                 -DMUT_H='"$(MUT_H)"' \
                 -DMSL_H='"$(MSL_H)"' \
                 -DRCN_H='"$(RCN_H)"' \
                 -DCNS_H='"$(CNS_H)"' \
                 -DYAG_H='"$(YAG_H)"' \
                 -DINF_H='"$(INF_H)"' \
                 -DGEN_H='"$(GEN_H)"' \
                 -DAPI_H='"$(API_H)"' \
                 -DFCL_H='"$(FCL_H)"'

REGULAR_LIB 	= $(YAG_L) $(TAS_L) $(STB_L) $(RPC_L) $(SIM_L) \
				  $(GSP_L) $(EFG_L) $(MCC_L) $(CBH_L) $(EQT_L) \
				  $(TTV_L) $(STM_L) $(TRC_L) $(LIB_L) $(TLF_L) \
				  $(TUT_L) $(TLC_L) $(GEN_L) $(API_L) \
				  $(FCL_L) $(INF_L) $(CNS_L) $(ELP_L) $(XX1_L) \
				  $(AVT_L) $(MLU_L) $(MCL_L) $(MAL_L) $(MSL_L)\
				  $(MGL_L) $(MVL_L) $(MEL_L) $(MHL_L) $(MLO_L) \
				  $(SPF_L) $(SPE_L) $(BEF_L) $(BGL_L) $(BVL_L) \
				  $(BEG_L) $(BHL_L) $(BEH_L) $(LOG_L) $(RCN_L) \
				  $(MUT_L) 

LIB =  $(FLEXOBJS) -L$(TARGET_LIB) $(WHOLE) $(REGULAR_LIB) -lm $(NOWHOLE) -ldl $(FLEXLIBS) $(TCL_L)

OBJ = gen_lofig.o gen_corresp.o gen_globals.o gen_drive_corresp.o gen_parse_corresp.o\
      gen_verif_utils.o gen_verif_exp_VHDL.o gen_verif_vector.o \
      gen_verif_exclude.o gen_verif_PortMap.o gen_verif_global.o \
      gen_model_utils.o gen_model_transistor.o gen_model_FCL.o gen_model_GENIUS.o \
      gen_model_global.o gen_execute_VHDL.o gen_search_utils.o \
      gen_search_recursive.o gen_search_iterative.o gen_search_global.o \
      gen_MBK_utils.o gen_env.o gen_main.o gen_expansion.o gen_globals_corresp.o \
      gen_symmetric.o gen_display.o gen_optimizations.o gen_new_kind.o
	  

SOURCES_LIB=$(OBJ:.o=.c)

PRE_OBJ = 

libgenius: $(TARGET_INCLUDE)/$(GEN_H) $(TARGET_LIB)/$(GEN_LIB)

build_gnstest : $(TARGET_BIN)/gnstest

headers : $(TARGET_INCLUDE)/$(GEN_H)

$(TARGET_INCLUDE)/$(GEN_H): genius_lib.h
	cp genius_lib.h $(TARGET_INCLUDE)/$(GEN_H)

$(TARGET_LIB)/$(GEN_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(GEN_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(GEN_LIB)

$(TARGET_BIN)/gnstest : gen_gnstest.o
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/gnstest gen_gnstest.o $(LIB)
	$(STRIP) $(TARGET_BIN)/gnstest

%.o: %.c 
	$(CC) -c $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	-@$(RM) -f $(OBJ) $(PRE_OBJ) gen_gnstest.o .depend > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_BIN)/gnstest $(TARGET_LIB)/$(GEN_LIB) $(TARGET_INCLUDE)/$(GEN_H) > /dev/null 2>&1

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES) $(SOURCES_LIB) > .depend

-include .depend

