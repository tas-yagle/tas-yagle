include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_CFLAGS  = $(CFLAGS) -c
LOCAL_ARFLAGS = $(ARFLAGS)

LOCAL_INCLUDE = -I. -I$(TARGET_INCLUDE)

LOCAL_CPPFLAGS   = $(CPPFLAGS) $(LOCAL_INCLUDE) \
                   -DCGV_H='"$(CGV_H)"' \
                   -DEQT_H='"$(EQT_H)"' \
                   -DLOG_H='"$(LOG_H)"' \
                   -DMLO_H='"$(MLO_H)"' \
                   -DMLU_H='"$(MLU_H)"' \
                   -DBEH_H='"$(BEH_H)"' \
                   -DBEG_H='"$(BEG_H)"' \
                   -DCBH_H='"$(CBH_H)"' \
                   -DCNS_H='"$(CNS_H)"' \
                   -DRCN_H='"$(RCN_H)"' \
                   -DMUT_H='"$(MUT_H)"' \
                   -DAVT_H='"$(AVT_H)"' \
                   -DINF_H='"$(INF_H)"' \
                   -DSLB_H='"$(SLB_H)"' \
                   -DYAG_H='"$(YAG_H)"'
                   
LOCAL_OBJ     = cgv_main.o cgv_build.o cgv_cnsfig.o \
                cgv_view.o cgv_alloc.o cgv_dealloc.o cgv_cone.o \
		cgv_utils.o cgv_lofig.o cgv_files.o cgv_interactions.o

SOURCES_LIB=$(LOCAL_OBJ:.o=.c)

all : $(TARGET_INCLUDE)/$(CGV_H) $(TARGET_LIB)/$(CGV_LIB)

headers : $(TARGET_INCLUDE)/$(CGV_H)

$(TARGET_INCLUDE)/$(CGV_H) : cgv_lib.h
	$(CP) cgv_lib.h $(TARGET_INCLUDE)/$(CGV_H)

.c.o:
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

$(TARGET_LIB)/$(CGV_LIB) : $(LOCAL_OBJ) cgv_lib.h
	$(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(CGV_LIB) $(LOCAL_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(CGV_LIB)

clean :
	-@$(RM) -f $(LOCAL_OBJ) .depend

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(CGV_LIB) $(TARGET_INCLUDE)/$(CGV_H) > /dev/null 2>&1

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

