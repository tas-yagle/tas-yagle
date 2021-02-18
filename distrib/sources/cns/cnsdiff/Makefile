include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh


CNSDIFF_H      = cnsdiff.h

LOCAL_CFLAGS  = $(CFLAGS)

LOCAL_INCLUDE = -I. -I$(TARGET_INCLUDE)

LOCAL_CPPFLAGS   = $(CPPFLAGS) -c $(LOCAL_INCLUDE) \
                   -DAVERTEC -D$(AVERTEC_LICENSE) \
                   -DAVT_H='"$(AVT_H)"' \
                   -DCNSDIFF_H='"$(CNSDIFF_H)"' \
                   -DMUT_H='"$(MUT_H)"' \
                   -DMLO_H='"$(MLO_H)"' \
				   -DMLU_H="<$(MLU_H)>" \
				   -DEQT_H="<$(EQT_H)>" \
                   -DBEG_H='"$(BEH_H)"' \
                   -DBEH_H='"$(BEH_H)"' \
                   -DCNS_H='"$(CNS_H)"' \
                   -DINF_H='"$(INF_H)"' \
                   -DRCN_H='"$(RCN_H)"' \
                   -DLOG_H='"$(LOG_H)"'

LOCAL_LIB  = $(FLEXOBJS) -L$(TARGET_LIB) \
			 -D$(AVERTEC_OS) \
             $(CNS_L) \
             $(MLO_L) \
             $(BEG_L) \
             $(BEH_L) \
             $(BHL_L) \
             $(EQT_L) \
             $(LOG_L) \
             $(RCN_L) \
             $(MUT_L) \
             $(AVT_L) \
			 $(RPC_L) \
             -lm $(FLEXLIBS)

CNSDIFF_OBJ     = cnsdiff_util.o cnsdiff.o 

all : $(TARGET_BIN)/cnsdiff

$(TARGET_BIN)/cnsdiff : $(CNSDIFF_OBJ) 
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(CNSDIFF_OBJ) -o $(TARGET_BIN)/cnsdiff $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/cnsdiff

.c.o:
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	-@$(RM) -f $(CNSDIFF_OBJ)

realclean : 
	-$(RM) -f $(TARGET_BIN)/cnsdiff


