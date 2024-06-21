include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC      = -I. -I../yagle -I../inf -I../genius -I$(TARGET_INCLUDE)

LOCAL_CFLAGS   = $(CFLAGS) -c
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) \
                 -DAVT_H='<$(AVT_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
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

OBJ = fcl_library.o fcl_constrain.o fcl_extendmsl.o fcl_findcell.o fcl_phase1.o \
      fcl_partition.o fcl_phase2.o fcl_matrix.o fcl_util.o fcl_solutions.o

all : $(TARGET_INCLUDE)/$(FCL_H) libfcl

headers : $(TARGET_INCLUDE)/$(FCL_H)

$(TARGET_INCLUDE)/$(FCL_H) : fcl_lib.h
	$(CP) fcl_lib.h $(TARGET_INCLUDE)/$(FCL_H)

libfcl: $(TARGET_LIB)/$(FCL_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

$(TARGET_LIB)/$(FCL_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(FCL_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(FCL_LIB)

clean :
	-@$(RM) -f $(OBJ) .depend > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(FCL_LIB) $(TARGET_INCLUDE)/$(FCL_H) > /dev/null 2>&1

SOURCES_LIB=$(OBJ:.o=.c)

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

