# makefile for bck
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

BCK_HEADER   = bck.h

SHELL = /bin/sh

BCK_OBJ = bck_main.o
BCK_OBJ_LIB = bck_view.o bck_alloc.o bck_sdf.o lex.sdf.o sdf.tab.o \
              bck_post.o bck_bck2ttv.o

BCK_BIN = bckparse

LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_LIB  = -L$(TARGET_LIB) \
               $(AVT_L) \
               $(BCK_L) \
               $(TTV_L) \
               $(STM_L) \
               $(ELP_L) \
               $(INF_L) \
               $(MLU_L) \
               $(MCL_L) \
               $(MAL_L) \
               $(MSL_L) \
               $(MVL_L) \
               $(MGL_L) \
               $(MEL_L) \
               $(MHL_L) \
               $(MLO_L) \
               $(RCN_L) \
               $(MUT_L) \
               -lm 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DBCK_H='"$(BCK_H)"' \
                 -DMGN_H='<$(MGN_H)>' \
                 -DMGL_H='<$(MGL_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMPU_H='<$(MPU_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
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
                 -DTTV_H='"$(TTV_H)"'


LOCAL_YFLAGS = $(YACCFLAGS) -y -d

all : $(TARGET_INCLUDE)/$(BCK_H) $(TARGET_LIB)/$(BCK_LIB)

headers : $(TARGET_INCLUDE)/$(BCK_H)

$(TARGET_LIB)/$(BCK_LIB) : $(BCK_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(BCK_LIB) $(BCK_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(BCK_LIB)
	
$(TARGET_INCLUDE)/$(BCK_H) :
	$(CP) $(BCK_HEADER) $(TARGET_INCLUDE)/$(BCK_H)

sdf.tab.h : sdf.tab.c

sdf.tab.c : sdf.y 
	$(YACC) $(LOCAL_YFLAGS) -b sdf -p sdf sdf.y 

lex.sdf.c : sdf.l sdf.tab.h
	$(LEX) $(LEXFLAGS) -Psdf sdf.l 

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(BCK_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(BCK_H)

clean : 
	-$(RM) $(BCK_OBJ_LIB) lex.sdf.c sdf.tab.c sdf.tab.h
	-$(RM) $(BCK_OBJ)
	
