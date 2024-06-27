# makefile for elp libraries
# date    : 31/07/95

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

OBJ          = elp_error.o elp_util.o y.tab.o lex.elp.o 
SHARED_OBJ   = elp_API.o
OBJ_BIN      = elp_main.o

ELP_HEADER   = elp_lib.h

ELP_BIN      = elpparse

ELP_API      = elp_API.so

WRAP = $(TARGET_BIN)/wrap

WRAPPER      = elp_API_wrap.c

LOCAL_INC    = -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
               -D$(AVERTEC_OS) \
               -D$(AVERTEC_LICENSE) \
               -DMLO_H="<$(MLO_H)>" \
               -DMLU_H="<$(MLU_H)>" \
               -DEQT_H="<$(EQT_H)>" \
               -DMUT_H="<$(MUT_H)>" \
               -DAVT_H="<$(AVT_H)>" \
               -DRCN_H="<$(RCN_H)>" \
               -DBEH_H="<$(BEH_H)>" \
               -DBEG_H="<$(BEG_H)>" \
               -DCBH_H="<$(CBH_H)>" \
               -DLOG_H="<$(LOG_H)>" \
               -DINF_H="<$(INF_H)>" \
               -DCNS_H="<$(CNS_H)>" \
               -DTECHNOLOGY='"/etc/prol05.elp"' \
               -DELP_H='"$(ELP_H)"'

LOCAL_LIB  = -L$(TARGET_LIB) \
             -L$(GNU_LIB) \
             $(ELP_L) \
             $(MLO_L) \
             $(RCN_L) \
             $(MUT_L) \
             $(RPC_L) \
             -lm

LOCAL_YFLAGS = $(YACCFLAGS) -y -d

all : lib

headers : $(TARGET_INCLUDE)/$(ELP_H)

# lib

lib : $(TARGET_INCLUDE)/$(ELP_H) $(TARGET_LIB)/$(ELP_LIB)

$(TARGET_LIB)/$(ELP_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(ELP_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(ELP_LIB)

$(TARGET_INCLUDE)/$(ELP_H) : $(ELP_HEADER)
	$(CP) $(ELP_HEADER) $(TARGET_INCLUDE)/$(ELP_H)

bin : $(TARGET_BIN)/$(ELP_BIN)

$(TARGET_BIN)/$(ELP_BIN) : $(TARGET_LIB)/$(ELP_LIB) $(OBJ_BIN)
	$(CC) $(LOCAL_CFLAGS) $(OBJ_BIN) -o $(TARGET_BIN)/$(ELP_BIN) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/$(ELP_BIN)

y.tab.h : y.tab.c

y.tab.c : elp.y
	$(YACC) $(LOCAL_YFLAGS) -p elp elp.y

lex.elp.c : elp.l y.tab.h
	$(LEX) $(LEXFLAGS) -Pelp elp.l

# api

api : $(TARGET_API_LIB)/$(ELP_API)

$(TARGET_API_LIB)/$(ELP_API) : $(WRAPPER)
	$(SCC) $(LOCAL_CFLAGS) -shared $(WRAPPER) $(SHARED_OBJ) -o $(TARGET_API_LIB)/$(ELP_API)

$(WRAPPER) : elp_API.h
	$(WRAP) -p $(TARGET_API_MAN) -m "elp-api" elp_API.h

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) $(TARGET_LIB)/$(ELP_LIB)
	-$(RM) $(TARGET_INCLUDE)/$(ELP_H)

clean :
	-$(RM) *.o lex.elp.c y.tab.h y.tab.c
	-$(RM) $(WRAPPER)
