# makefile for zen libraries
# date    : 15/10/99

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

ZEN_OBJ_LIB = zen_error.o zen_beh.o zen_all.o zen_util.o zen_zenfig.o zen_zennod.o 

ZEN_OBJ = zen_main.o

ZENTEST = fpath


ZEN_HEADER   = zen_lib.h

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
				 -DEQT_H='<$(EQT_H)>' \
                 -DBGL_H='<$(BGL_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DTLC_H='<$(TLC_H)>' \
                 -DZEN_H='"$(ZEN_H)"' \
                 -DZEN_VERSION='"$(ZEN_VERSION)"' 

LOCAL_LIB  = -L$(TARGET_LIB) \
             -L$(GNU_LIB) \
             $(AVT_L) \
             $(BEF_L) \
             $(BGL_L) \
             $(BVL_L) \
             $(BEG_L) \
             $(BSC_L) \
             $(BHL_L) \
             $(BEH_L) \
			 $(EQT_L) \
             $(LOG_L) \
             $(MLO_L) \
             $(MLU_L) \
             $(MUT_L) \
             $(RPC_L) \
             $(ZEN_L) \
             -lm

all : $(TARGET_INCLUDE)/$(ZEN_H) $(TARGET_LIB)/$(ZEN_LIB) $(ZEN_OBJ)

headers : $(TARGET_INCLUDE)/$(ZEN_H)

test : all $(TARGET_BIN)/$(ZENTEST)

$(TARGET_BIN)/$(ZENTEST) : $(TARGET_LIB)/$(ZEN_LIB) $(ZEN_OBJ)
	$(CC) $(LOCAL_CFLAGS) $(ZEN_OBJ) -o $(TARGET_BIN)/$(ZENTEST) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/$(ZENTEST)

$(TARGET_LIB)/$(ZEN_LIB) : $(ZEN_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(ZEN_LIB) $(ZEN_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(ZEN_LIB)
.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

$(TARGET_INCLUDE)/$(ZEN_H) :
	$(CP) $(ZEN_HEADER) $(TARGET_INCLUDE)/$(ZEN_H)
 
realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(ZEN_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(ZEN_H)
	-$(RM) -f $(TARGET_BIN)/$(ZENTEST)
 
clean :
	-$(RM) -f $(ZEN_OBJ) $(ZEN_OBJ_LIB)

