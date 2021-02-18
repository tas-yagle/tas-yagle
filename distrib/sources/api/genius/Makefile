include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DBASIC_TOKEN\
                 -DAVT_H='"$(AVT_H)"' \
                 -DAPI_H='"$(API_H)"' \
                 -DMLU_H='"$(MLU_H)"' \
                 -DMLO_H='"$(MLO_H)"' \
                 -DELP_H='"$(ELP_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
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
                 -DCBH_H='"$(CBH_H)"' \
                 -DEFG_H='"$(EFG_H)"' \
                 -DGSP_H='"$(GSP_H)"' \
                 -DSIM_H='"$(SIM_H)"' \
                 -DYAG_H='"$(YAG_H)"' \
                 -DSTM_H='"$(STM_H)"' \
                 -DINF_H='"$(INF_H)"' \
                 -DGEN_H='"$(GEN_H)"' \
                 -DFCL_H='"$(FCL_H)"'

WRAPPER = gen_API_wrap.c
TCL_WRAPPER = gen_TCL_wrap.c
GENERATED = gen_API_l.c gen_API_netlist_l.c gen_API_signals_l.c gen_API_recursive_gns_l.c gen_TCL_wrap.c
.precious : $(GENERATED)
OBJ = gen_API_wrap.o gen_API_l.o gen_API_netlist_l.o gen_API_signals_l.o\
      gen_API_recursive_gns_l.o
SOURCES_LIB=$(OBJ:.o=.c)

$(TARGET_LIB)/$(APIGENIUS_LIB): $(OBJ) $(TARGET_INCLUDE)/api_common_structures.h
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APIGENIUS_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APIGENIUS_LIB)
	cp gen_API.h $(TARGET_API_INCLUDE)

# Shared lib
dynamic_link: $(TARGET_INCLUDE)/api_common_structures.h $(OBJ)
	$(SCC) $(CFLAGS) -shared $(OBJ) gen_TCL_wrap.o -o $(TARGET_API_LIB)/gen_API.so
	cp gen_API.h $(TARGET_API_INCLUDE)

$(TARGET_INCLUDE)/api_common_structures.h: api_common_structures.h
	cp api_common_structures.h $(TARGET_INCLUDE)

# GNS wrapper
$(WRAPPER): gen_API.h
	$(WRAP) -p $(TARGET_API_MAN) gen_API.h
	@$(RM) -f gmon.out

# TCL wrapper
$(TCL_WRAPPER): genius.i
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 genius.i 

# Objects
.c.o :
	$(SCC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(OBJ) $(GENERATED) gen_API_corresp.o gen_API_lofig.o $(TARGET_INCLUDE)/api_common_structures.h > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/gen_API.so $(TARGET_API_INCLUDE)/gen_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APIGENIUS_LIB) > /dev/null 2>&1

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

