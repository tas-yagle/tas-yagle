# Makefile for swig 

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

BUILDNUM = $(shell date +%Y%m%d)

#TCL_L = -ltcl8.3g
LOCAL_INC = -I$(TARGET_INCLUDE) -I$(TARGET_API_INCLUDE)

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC \
				 -D$(AVERTEC_LICENSE) \
                 -D$(AVERTEC_OS) \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBGL_H='<$(BGL_H)>' \
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
                 -DSPF_H='"$(SPF_H)"' \
                 -DTLF_H='"$(TLF_H)"' \
                 -DSTB_H='"$(STB_H)"' \
                 -DSPE_H='"$(SPE_H)"' \
                 -DTAS_H='"$(TAS_H)"' 

LIB =  -L$(TARGET_LIB) $(STATIC_LIB) \
              $(CGV_L) \
              $(BCK_L) \
			  $(SLB_L) \
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
			  $(SDLR_L) \
			  $(PAVO_L) \
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
			  $(VCD_L) \
			  $(RPC_L) -ledit

API_STATICLIB  = -L$(TARGET_LIB) $(STATIC_LIB) $(WHOLE) $(LIB) $(TCL_L) $(APIALL_L) $(NOWHOLE) -ledit -ltermcap -lpthread -ldl -lm 

WRAP = $(TARGET_BIN)/wrap -silent

STATIC_SWIG_OBJ = avt_static_TCL_wrap.o 
SWIG_OBJ = avt_TCL_wrap.o 
#OBJ = avt.o avt_netlist.o avt_API_wrap.o router_API.o avt_API_COMMANDS.o
OBJ = avt.o avt_netlist.o avt_API_wrap.o router_API.o loop.o init.o complete.o el.o parse.o signal.o \
      avt_checktechno.o
TCL_RUNTIME = avt_TCL_wrap.c
STATIC_TCL_RUNTIME = avt_static_TCL_wrap.c
AVT_SHELL = $(TARGET_BIN)/avt_shell
WRAPPER = avt_API_wrap.c

# static Shell TCL
$(AVT_SHELL): headers $(STATIC_SWIG_OBJ) $(OBJ) 
	$(CPLUSPLUS)  $(CFLAGS) $(DYNAMIC) $(STATIC_SWIG_OBJ) $(OBJ) $(FLEXOBJS) $(API_STATICLIB) $(FLEXLIBS) -o $(AVT_SHELL)
	$(STRIP) $(AVT_SHELL)

# Shell TCL
dynamic_link: $(SWIG_OBJ) $(WRAPPER) $(OBJ)
	$(CPLUSPLUS)  $(CFLAGS) $(DYNAMIC) $(SWIG_OBJ) $(OBJ) $(TCL_L) $(FLEXOBJS) $(WHOLE) $(LIB) $(NOWHOLE) $(FLEXLIBS) -o $(AVT_SHELL)
	$(STRIP) $(AVT_SHELL)

headers: $(TARGET_API_INCLUDE)/avt_API_types.h

$(TARGET_API_INCLUDE)/avt_API_types.h: avt_API_types.h
	cp avt_API_types.h $(TARGET_API_INCLUDE)

# Objects
.c.o : avt_API.h
	$(CC) -DBUILDNUM=$(BUILDNUM) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

$(STATIC_SWIG_OBJ) :  $(STATIC_TCL_RUNTIME)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -Dmain=shell_main $<

$(SWIG_OBJ) :  $(TCL_RUNTIME)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -Dmain=shell_main $<

$(WRAPPER): avt_API.h
	#$(WRAP) -online avt_API_COMMANDS.c avt_API.h
	$(WRAP) -p $(TARGET_API_MAN) avt_API.h

# Main 
$(TCL_RUNTIME): avtshell_tclsh.i avt.i
	$(SWIG) -o $(TCL_RUNTIME) -tcl8 -l./avtshell_tclsh.i avt.i

$(STATIC_TCL_RUNTIME): avtshell_tclsh.i avt_static.i
	$(SWIG) -o $(STATIC_TCL_RUNTIME) -tcl8 -l./avtshell_tclsh.i avt_static.i

realclean : clean
	-$(RM) -f $(AVT_SHELL)

clean :
	-$(RM) -f $(STATIC_TCL_RUNTIME) $(OBJ) $(STATIC_SWIG_OBJ) 
	-$(RM) -f $(TCL_RUNTIME) $(OBJ) $(SWIG_OBJ) 
	-$(RM) -f $(WRAPPER)
	-$(RM) -f $(TARGET_API_INCLUDE)/avt_API_types.h

