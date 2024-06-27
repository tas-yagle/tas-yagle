# makefile for ttv libraries
# date    : 26/06/96

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

SHELL = /bin/sh

TTV_OBJ      = ttv_alloc.o ttv_fig.o ttv_error.o ttv_fact.o ttv_falsepath.o \
               ttv_util.o ttv_drittv.o ttv_critic.o ttv_parsttv.o ttv_ctx.o \
               lex.ctx.o ctx.tab.o ttv_propagation.o ttv_directives.o ttv_ssta.o

TTV_HEADER   = ttv_lib.h

LOCAL_INC = -I$(TARGET_INCLUDE)

LOCAL_YFLAGS = $(YACCFLAGS) -y -d

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -DAVERTEC \
                 -DAVT_DATE_A='$(AVT_DATE_A)' \
                 -D$(AVERTEC_LICENSE) \
                 -D$(AVERTEC_OS) \
                 -DMGN_H='<$(MGN_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMPU_H='<$(MPU_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMPH_H='<$(MPH_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DSTM_H='<$(STM_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DTLC_H='<$(TLC_H)>' \
                 -DTRC_H='<$(TRC_H)>' \
                 -DMCC_H='"$(MCC_H)"' \
                 -DTTV_H='"$(TTV_H)"' \
                 -DSTB_H='"$(STB_H)"'

all : $(TARGET_INCLUDE)/$(TTV_H) $(TARGET_LIB)/$(TTV_LIB)

headers : $(TARGET_INCLUDE)/$(TTV_H)

$(TARGET_LIB)/$(TTV_LIB) : $(TTV_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(TTV_LIB) $(TTV_OBJ) 
	$(RANLIB) $(TARGET_LIB)/$(TTV_LIB)

$(TARGET_INCLUDE)/$(TTV_H) : $(TTV_HEADER)
	$(CP) $(TTV_HEADER) $(TARGET_INCLUDE)/$(TTV_H)

ctx.tab.h : ctx.tab.c

ctx.tab.c : ctx.y
	$(YACC) $(LOCAL_YFLAGS) -b ctx -p ctx ctx.y

lex.ctx.c : ctx.l ctx.tab.h
	$(LEX) $(LEXFLAGS) -Pctx ctx.l

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(TTV_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(TTV_H)

clean : 
	-$(RM) -f $(TTV_OBJ) lex.ctx.c ctx.tab.h ctx.tab.c
