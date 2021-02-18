include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC      = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -D$(AVERTEC_LICENSE) \
                 -DMLO_H='<$(MLO_H)>' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DBGL_H='<$(BGL_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>'

LOCAL_OBJECTS  = beh_chkbefig.o   beh_debug.o      beh_depend.o     \
                 beh_freabl.o     beh_indexbdd.o   beh_makbdd.o     \
                 beh_makgex.o     beh_namewrap.o     \
                 beh_makquad.o    beh_namelist.o   beh_unamlist.o   \
				 beh_rename.o
                #beh_makderiv.o beh_maknode.o beh_makvarlist.o 

all : $(TARGET_INCLUDE)/$(BHL_H) $(TARGET_LIB)/$(BHL_LIB)

$(TARGET_INCLUDE)/$(BHL_H) : bhl_lib.h
	$(CP) bhl_lib.h $(TARGET_INCLUDE)/$(BHL_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

$(TARGET_LIB)/$(BHL_LIB) : $(LOCAL_OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(BHL_LIB) $(LOCAL_OBJECTS)
	$(RANLIB) $(TARGET_LIB)/$(BHL_LIB)

clean : 
	$(RM) -f $(LOCAL_OBJECTS)

realclean : clean
	$(RM) -f $(TARGET_INCLUDE)/$(BHL_H) $(TARGET_LIB)/$(BHL_LIB)
