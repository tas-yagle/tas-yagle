##############################################################################
##                                                                          ##
##   Makefile for BEG (behavioral generator)                                ##
##                                                                          ##
##   Author : Antony Pinto  october, the 28th 2002                          ##
##                                                                          ##
##############################################################################


include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_CFLAGS    = $(CFLAGS)
LOCAL_YACCFLAGS = $(YACCFLAGS) -d -t
LOCAL_LEXFLAGS  = $(LEXFLAGS) -t
LOCAL_ARFLAGS   = $(ARFLAGS)

LOCAL_INCLUDE   = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS    = $(CFLAGS)
LOCAL_CPPFLAGS  = $(CPPFLAGS) $(LOCAL_INCLUDE) \
                  -D$(AVERTEC_LICENSE) \
                  -DAVT_H='<$(AVT_H)>' \
                  -DMUT_H='<$(MUT_H)>' \
                  -DMLO_H='<$(MLO_H)>' \
                  -DEQT_H='<$(EQT_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBHL_H='<$(BHL_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DBEF_H='<$(BEG_H)>' \
                  -DBVL_H='<$(BVL_H)>' \
                  -DBGL_H='<$(BGL_H)>'
REGULAR_LIB 	= $(YAG_L) $(TAS_L) $(STB_L) $(RPC_L) $(SIM_L) \
				  $(GSP_L) $(EFG_L) $(MCC_L) $(CBH_L) $(EQT_L) \
				  $(TTV_L) $(STM_L) $(TRC_L) $(LIB_L) $(TLF_L) \
				  $(TUT_L) $(TLC_L) $(GEN_L) $(API_L) \
				  $(FCL_L) $(INF_L) $(CNS_L) $(ELP_L) $(XX1_L) \
				  $(AVT_L) $(MLU_L) $(MCL_L) $(MAL_L) $(MSL_L) \
				  $(MGL_L) $(MVL_L) $(MEL_L) $(MHL_L) $(MLO_L) \
				  $(SPF_L) $(SPE_L) $(BEF_L) $(BGL_L) $(BVL_L) \
				  $(BEG_L) $(BHL_L) $(BEH_L) $(LOG_L) $(RCN_L) \
				  $(MUT_L) 

LOCAL_LIB 		= $(FLEXOBJS) $(WHOLE) -L$(TARGET_LIB) $(REGULAR_LIB) -lm $(NOWHOLE) -ldl $(FLEXLIBS) 

LOCAL_BEG_OBJ = beg_gen.tab.o beg_gen.yy.o beg_gen.o beg_topLevel.o

LOCAL_OBJ = $(LOCAL_BEG_OBJ)

LOC_TEST_OBJ = beg_gen.tab.o beg_gen.yy.o beg_gen.o

all : $(TARGET_INCLUDE)/$(BEG_H) $(TARGET_LIB)/$(BEG_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

$(TARGET_INCLUDE)/$(BEG_H) : beg_lib.h
	$(CP) beg_lib.h $(TARGET_INCLUDE)/$(BEG_H)

$(TARGET_LIB)/$(BEG_LIB) : $(LOCAL_OBJ)
	$(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(BEG_LIB) $(LOCAL_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(BEG_LIB)

beg_gen.c : 
	$(TOUCH) beg_gen.c

beg_gen.tab.c : beg_gen.y
	$(YACC) $(LOCAL_YACCFLAGS) -p beg_gen -o beg_gen.tab.c beg_gen.y

beg_gen.tab.h : beg_gen.tab.c

beg_gen.yy.c : beg_gen.l beg_gen.tab.h
	$(LEX) $(LOCAL_LEXFLAGS) -Pbeg_gen beg_gen.l > beg_gen.yy.c

# test
$(TARGET_BIN)/beg_main : all beg_main.o
	$(CC) beg_main.o $(LOCAL_CFLAGS) -o $@ $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/beg_main

test : all $(LOC_TEST_OBJ) $(TARGET_BIN)/beg_main

cleantest :
	$(RM) -f $(TARGET_BIN)/beg_main beg_main.o

clean : cleantest
	$(RM) -f beg_gen.tab.h   beg_gen.tab.c   beg_gen.yy.c beg_gen.output
	$(RM) -f $(LOCAL_OBJ)

realclean : clean
	$(RM) -f $(TARGET_LIB)/$(BEG_LIB) $(TARGET_INCLUDE)/$(BEG_H)
