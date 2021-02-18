include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC      = -I. -I$(TARGET_INCLUDE) 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -D$(AVERTEC_LICENSE) \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEF_H='<$(BEF_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBVL_H='<$(BVL_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DBGL_H='<$(BGL_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>'

LOCAL_OBJECTS  = bef_file.o bef_drive_utils.o

all : $(TARGET_INCLUDE)/$(BEF_H) $(TARGET_LIB)/$(BEF_LIB)

$(TARGET_INCLUDE)/$(BEF_H) : bef_lib.h
	$(CP) bef_lib.h $(TARGET_INCLUDE)/$(BEF_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

$(TARGET_LIB)/$(BEF_LIB) : $(LOCAL_OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(BEF_LIB) $(LOCAL_OBJECTS)
	-$(RANLIB) $(TARGET_LIB)/$(BEF_LIB)

clean :
	$(RM) -f $(LOCAL_OBJECTS)

realclean : clean
	$(RM) -f $(TARGET_INCLUDE)/$(BEF_H) $(TARGET_LIB)/$(BEF_LIB)
