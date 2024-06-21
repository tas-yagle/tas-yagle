# makefile for xtas
# date    : 15/02/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

XTAS_NAME = Xtas
XTAS_BIN = xtas
XTAS_ABOUT   = Timing Analyzer

LOCAL_XTAS_OBJ = xtas_main.o

LOCAL_MAIN_OBJ = xtas_desk.o xtas_common.o xtas_unix.o xtas_init.o \
		 xtas_files.o xtas_help.o xtas_exit.o
		
LOCAL_TTV_OBJ  = xtas_ttv.o xtas_signals.o xtas_detsig.o xtas_getpath.o	\
			     xtas_paths.o xtas_detpath.o xtas_hierarchical.o \
				 xtas_stb_pbar.o xtas_stb.o xtas_getdetail.o \
                 xtas_getdelay.o xtas_flspaths.o xtas_debug.o \
                 xtas_getpara.o xtas_graphic.o xtas_newdisp_util.o \
				 xtas_xyagle.o xtas_simu_path.o xtas_ctk_noise.o  \
				 xtas_visualise_path.o 

LOCAL_TAS_OBJ  = xtas_tas.o

XTAS_OBJ = $(LOCAL_TAS_OBJ) $(LOCAL_TTV_OBJ) $(LOCAL_MAIN_OBJ) $(LOCAL_XTAS_OBJ)

LOCAL_INC = $(INCLUDE64BIT) \
            -I$(TARGET_INCLUDE) \
            -I$(MOTIF_INCLUDE) \
            -I$(X11_INCLUDE) \
            -I$(XPM_INCLUDE)

LOCAL_CFLAGS   = $(CFLAGS) 
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D_NO_PROTO \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -D$(AVERTEC_OS) \
                 -DXTAS_ABOUT='"$(XTAS_ABOUT)"' \
                 -DXTAS_NAME='"$(XTAS_NAME)"' \
                 -DBIN_NAME='"$(XTAS_BIN)"' \
                 -DAVT_VERSION=$(AVT_VERSION) \
                 -DPATCH_NUM='$(DISTRIB_PATCHVERSION)' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DINF_H='<$(INF_H)>' \
				 -DBHL_H='<$(BHL_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DXAL_H='<$(XAL_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DTTV_H='<$(TTV_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DYAG_H='<$(YAG_H)>' \
                 -DGEN_H='<$(GEN_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DSLB_H='<$(SLB_H)>' \
                 -DCGV_H='<$(CGV_H)>' \
                 -DXYA_H='<$(XYA_H)>' \
                 -DTLC_H='<$(TLC_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DSTB_H='<$(STB_H)>' \
                 -DTAS_H='<$(TAS_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DTMA_H='<$(TMA_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DTUT_H='<$(TUT_H)>' \
                 -DLIB_H='<$(LIB_H)>' \
                 -DTLF_H='<$(TLF_H)>' \
                 -DFCL_H='<$(FCL_H)>' \
                 -DZEN_H='<$(ZEN_H)>' 
                 
REGULAR_LIB = $(XYA_L) \
              $(CGV_L) \
			  $(SLB_L) \
			  $(XAL_L) \
			  $(ZEN_L) \
			  $(TMA_L) \
			  $(TAS_L) \
			  $(STB_L) \
			  $(LIB_L) \
			  $(TLF_L) \
			  $(TUT_L) \
			  $(MCC_L) \
			  $(CBH_L) \
			  $(EQT_L) \
			  $(TTV_L) \
			  $(STM_L) \
			  $(EFG_L) \
			  $(GSP_L) \
			  $(SIM_L) \
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
			  $(RPC_L)
			  
LOCAL_LIB =  $(FLEXOBJS) $(STATIC_LIB) -L$(TARGET_LIB) $(WHOLE) $(REGULAR_LIB) $(NOWHOLE) $(TCL_L)\
            -L$(MOTIF_LIB) -lXm -L$(XPM_LIB) -lXpm -L$(X11_LIB) -ledit -ltermcap -lXt -lX11 -lpthread  -ldl -lm $(FLEXLIBS) $(PSP_L)
			
API_LIB = $(WHOLE) $(APIALL_L) $(NOWHOLE)
	
API_STATICLIB = $(FLEXOBJS) $(STATIC_LIB) -L$(TARGET_LIB) $(WHOLE) $(APIALL_LIB) $(TCL_L) $(REGULAR_LIB) $(NOWHOLE) \
            -L$(MOTIF_LIB) -lXm -L$(XPM_LIB) -lXpm -L$(X11_LIB) -lXt -lX11 -ldl -lm $(FLEXLIBS)
			
$(TARGET_BIN)/xtas : $(XTAS_OBJ)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(DYNAMIC) $(XTAS_OBJ) -o $(TARGET_BIN)/xtas $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/xtas

link:
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(DYNAMIC) $(XTAS_OBJ) -o $(TARGET_BIN)/xtas $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/xtas

api_link:
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(DYNAMIC) $(XTAS_OBJ) -o $(TARGET_BIN)/xtas $(API_STATICLIB)
	$(STRIP) $(TARGET_BIN)/xtas

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

realclean : clean
	-$(RM) -f $(TARGET_BIN)/xtas

clean :
	-$(RM) -f $(XTAS_OBJ)

headers : ;
