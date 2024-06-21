include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DCTK_TOKEN\
                 -DAVT_H='"$(AVT_H)"' \
                 -DAPI_H='"$(API_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DMLO_H='"$(MLO_H)"' \
                 -DELP_H='"$(ELP_H)"' \
                 -DLOG_H='"$(LOG_H)"' \
                 -DBVL_H='"$(BVL_H)"' \
                 -DBEH_H='"$(BEH_H)"' \
                 -DBEG_H='"$(BEG_H)"' \
                 -DBHL_H='"$(BHL_H)"' \
                 -DBEF_H='"$(BEF_H)"' \
                 -DMUT_H='"$(MUT_H)"' \
             	 -DTRC_H='"$(TRC_H)"' \
                 -DMSL_H='"$(MSL_H)"' \
                 -DRCN_H='"$(RCN_H)"' \
                 -DCNS_H='"$(CNS_H)"' \
                 -DYAG_H='"$(YAG_H)"' \
                 -DINF_H='"$(INF_H)"' \
                 -DGEN_H='"$(GEN_H)"' \
                 -DTAS_H='"$(TAS_H)"' \
                 -DTRC_H='"$(TRC_H)"' \
                 -DSTM_H='"$(STM_H)"' \
                 -DTTV_H='"$(TTV_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DSIM_H='"$(SIM_H)"' \
                 -DGSP_H='"$(GSP_H)"' \
                 -DEFG_H='"$(EFG_H)"' \
                 -DCBH_H='"$(CBH_H)"' \
                 -DFCL_H='"$(FCL_H)"' \
                 -DSTB_H='"$(STB_H)"'

WRAPPER = ctk_API_wrap.c
TCL_WRAPPER = ctk_TCL_wrap.c

OBJ = ctk_base_l.o ctk_default_actions_l.o ctk_signals_l.o ctk_API_wrap.o ctk_API_l.o
SOURCES_LIB=$(OBJ:.o=.c)
GENERATED = ctk_base_l.c ctk_default_actions_l.c ctk_signals_l.c ctk_API_l.c

$(TARGET_LIB)/$(APICTK_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APICTK_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APICTK_LIB)
	cp ctk_API.h $(TARGET_API_INCLUDE)

dynamic_link: $(OBJ) 
	$(SCC) $(CFLAGS) -shared $(OBJ) ctk_TCL_wrap.o -o $(TARGET_API_LIB)/ctk_API.so
	cp ctk_API.h $(TARGET_API_INCLUDE)

$(WRAPPER): ctk_API.h
	$(WRAP) -p $(TARGET_API_MAN) ctk_API.h
	@$(RM) -f gmon.out

# TCL wrapper
$(TCL_WRAPPER): ctk.i
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 ctk.i 

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) ${TCL_WRAPPER} $(OBJ) $(GENERATED) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/ctk_API.so $(TARGET_API_INCLUDE)/ctk_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APICTK_LIB) > /dev/null 2>&1
	-@$(RM) -f .depend

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

