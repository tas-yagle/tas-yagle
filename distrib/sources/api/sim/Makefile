include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DSPI_TOKEN\
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DYAG_H='<$(YAG_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DGEN_H='<$(GEN_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DTTV_H='<$(TTV_H)>' \
                 -DTAS_H='<$(TAS_H)>' \
                 -DFCL_H='<$(FCL_H)>'

WRAPPER = sim_API_wrap.c
TCL_WRAPPER = sim_TCL_wrap.c
OBJ = sim_API_wrap.o sim_API_lowlevel_l.o  sim_API_highlevel_l.o sim_API_elp_l.o \
	  sim_API_extract_l.o sim_API_dump_l.o
GENERATED = sim_API_lowlevel_l.c  sim_API_highlevel_l.c sim_API_elp_l.c\
            sim_API_extract_l.c sim_API_dump_l.c sim_TCL_wrap.c
.precious : $(GENERATED)

# Static linking for Purify
$(TARGET_LIB)/$(APISIM_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APISIM_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APISIM_LIB)
	cp sim_API.h $(TARGET_API_INCLUDE)

# Shared lib
dynamic_link: $(OBJ)
	$(SCC) -shared $(OBJ) sim_TCL_wrap.o -o $(TARGET_API_LIB)/sim_API.so
	cp sim_API.h $(TARGET_API_INCLUDE)

# GNS wrapper
$(WRAPPER): sim_API.h
	$(WRAP) -p $(TARGET_API_MAN) sim_API.h
	@$(RM) -f gmon.out

# TCL wrapper
$(TCL_WRAPPER): sim.i
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 sim.i 

# Objects
.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(OBJ) $(GENERATED) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/sim_API.so $(TARGET_API_INCLUDE)/sim_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APISIM_LIB) > /dev/null 2>&1
