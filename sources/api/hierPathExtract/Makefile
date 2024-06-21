include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c -D$(AVERTEC_OS)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DBASIC_TOKEN\
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
                 -DSPE_H='"$(SPE_H)"' \
                 -DSTM_H='"$(STM_H)"' \
                 -DTTV_H='"$(TTV_H)"' \
                 -DEQT_H='"$(EQT_H)"' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DSIM_H='"$(SIM_H)"' \
                 -DGSP_H='"$(GSP_H)"' \
                 -DEFG_H='"$(EFG_H)"' \
                 -DCBH_H='"$(CBH_H)"' \
                 -DSTB_H='"$(STB_H)"' \
                 -DFCL_H='"$(FCL_H)"'

STATIC_LIB = $(APIHPE_LIB)

WRAPPER = hpe_API_wrap.c

TCL_WRAPPER = hpe_TCL_wrap.c
OBJ = hpe_API_l.o hpe_API_wrap.o
SOURCES_LIB=$(OBJ:.o=.c)
.precious : $(SOURCES_LIB)

# Rules

# Static linking for Purify
$(TARGET_LIB)/$(STATIC_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(STATIC_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(STATIC_LIB)
	cp hpe_API.h $(TARGET_API_INCLUDE)

# Shared lib
dynamic_link: $(OBJ) 
	$(SCC) $(CFLAGS) -shared $(OBJ) hpe_TCL_wrap.o -o $(TARGET_API_LIB)/hpe_API.so
	cp hpe_API.h $(TARGET_API_INCLUDE)

# GNS wrapper
$(WRAPPER): hpe_API.h
	$(WRAP) -p $(TARGET_API_MAN) hpe_API.h
	@$(RM) -f gmon.out

# TCL wrapper
$(TCL_WRAPPER): hpe.i 
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 hpe.i 

# Objects
.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(TCL_WRAPPER) $(OBJ) $(SOURCES_LIB) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/hpe_API.so $(TARGET_API_INCLUDE)/hpe_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(STATIC_LIB) > /dev/null 2>&1
	-@$(RM) -f .depend

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM  $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

