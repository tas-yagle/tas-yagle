include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c -D$(AVERTEC_OS)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DTIMING_TOKEN\
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
                 -DSTB_H='"$(STB_H)"' \
                 -DSDC_H='"$(SDC_H)"' \
                 -DFCL_H='"$(FCL_H)"'

WRAPPER = inf_API_wrap.c

TCL_WRAPPER = inf_TCL_wrap.c
OBJ = inf_API.o inf_sdc.o\
	  inf_API_wrap.o

SOURCES_LIB=$(OBJ:.o=.c)

# Rules

# Static linking for Purify
$(TARGET_LIB)/$(APIINF_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APIINF_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APIINF_LIB)
	cp inf_API.h $(TARGET_API_INCLUDE)

# Shared lib
dynamic_link: $(OBJ) 
	$(SCC) $(CFLAGS) -shared $(OBJ) inf_TCL_wrap.o -o $(TARGET_API_LIB)/inf_API.so
	cp inf_API.h $(TARGET_API_INCLUDE)

# GNS wrapper
$(WRAPPER): inf_API.h
	$(WRAP) -p $(TARGET_API_MAN) inf_API.h
	@$(RM) -f gmon.out

# TCL wrapper
$(TCL_WRAPPER): inf.i 
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 inf.i 

# Objects
.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/license_api
	$(TARGET_BIN)/license_api $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(TCL_WRAPPER) $(OBJ) $(GENERATED) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/inf_API.so $(TARGET_API_INCLUDE)/inf_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APIINF_LIB) > /dev/null 2>&1
	-@$(RM) -f .depend

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

