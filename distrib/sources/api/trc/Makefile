include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC = -I$(TARGET_API_INCLUDE) -I$(TARGET_INCLUDE) 

WRAP = $(TARGET_BIN)/wrap -silent

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DBASIC_TOKEN\
                 -DAVT_H='<$(AVT_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DYAG_H='<$(YAG_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DGEN_H='<$(GEN_H)>' \
                 -DFCL_H='<$(FCL_H)>'

WRAPPER = trc_API_wrap.c
TCL_WRAPPER = trc_TCL_wrap.c

OBJ = trc_API_l.o trc_API_wrap.o
GENERATED = trc_API_l.c

$(TARGET_LIB)/$(APITRC_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APITRC_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APITRC_LIB)
	cp trc_API.h $(TARGET_API_INCLUDE)

dynamic_link: $(OBJ)
	$(SCC) -shared $(OBJ) trc_TCL_wrap.o -o $(TARGET_API_LIB)/trc_API.so
	cp trc_API.h $(TARGET_API_INCLUDE)

$(WRAPPER): trc_API.h
	$(WRAP) -p $(TARGET_API_MAN) trc_API.h
	@$(RM) -f gmon.out

# TCL wrapper
$(TCL_WRAPPER): trc.i
	$(SWIG) -o $(TCL_WRAPPER) -tcl8 trc.i 

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/$(LICENSE_API)
	$(TARGET_BIN)/$(LICENSE_API) $< API_TEST_TOKEN > $@

clean :
	-@$(RM) -f $(WRAPPER) $(OBJ) $(GENERATED) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_API_LIB)/trc_API.so $(TARGET_API_INCLUDE)/trc_API.h > /dev/null 2>&1
	-@$(RM) -f $(TARGET_LIB)/$(APITRC_LIB) > /dev/null 2>&1
