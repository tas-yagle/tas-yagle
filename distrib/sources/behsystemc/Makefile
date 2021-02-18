include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_CFLAGS    = $(CFLAGS)
LOCAL_ARFLAGS   = $(ARFLAGS)

LOCAL_INCLUDE   = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS    = $(CFLAGS)
LOCAL_CPPFLAGS  = $(CPPFLAGS) $(LOCAL_INCLUDE) \
                  -D$(AVERTEC_LICENSE) \
                  -DMUT_H='<$(MUT_H)>' \
                  -DBEH_H='<$(BEH_H)>' \
                  -DBEG_H='<$(BEG_H)>' \
                  -DBHL_H='<$(BHL_H)>' \
                  -DLOG_H='<$(LOG_H)>' \
                  -DBSC_H='<$(BSC_H)>'

LOCAL_BSC_OBJ   = bsc_drive.o bsc_util.o

LOCAL_OBJ = $(LOCAL_BSC_OBJ)

COMPIL_FILE 	= "COMPIL"

beta :
	@echo "nothing to do, Beta version"

all : init $(TARGET_INCLUDE)/$(BSC_H) $(TARGET_LIB)/$(BSC_LIB)

init :
	@echo "" > $(COMPIL_FILE);

$(TARGET_INCLUDE)/$(BSC_H) : bsc_lib.h
	@echo "[BSC make] -- copying :" $(BSC_H)
	@echo $(CP) bsc_lib.h $(TARGET_INCLUDE)/$(BSC_H) \
	>> $(COMPIL_FILE)
	@$(CP) bsc_lib.h $(TARGET_INCLUDE)/$(BSC_H)

.c.o :
	@echo "[BSC make] --" $(CC) $<
	@echo $(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $< \
	>> $(COMPIL_FILE)
	@$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

$(TARGET_LIB)/$(BSC_LIB) : $(LOCAL_OBJ)
	@echo "[BSC make] --" $(AR) $(BSC_LIB)
	@echo $(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(BSC_LIB) $(LOCAL_OBJ) \
    >> $(COMPIL_FILE)
	@$(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(BSC_LIB) \
							$(LOCAL_OBJ) >> $(COMPIL_FILE) 2>&1
	@echo "[BSC make] --" $(RANLIB) $(BSC_LIB)
	@echo $(RANLIB) $(TARGET_LIB)/$(BSC_LIB) >> $(COMPIL_FILE)
	@$(RANLIB) $(TARGET_LIB)/$(BSC_LIB)

info :
	@echo "[BVL make] -- Info"
	@echo "       CFLAGS    :" $(LOCAL_CFLAGS)
	@echo "       CPPFLAGS  :" $(LOCAL_CPPFLAGS)
	@echo "       LEXFLAGS  :" $(LOCAL_LEXFLAGS)
	@echo "       YACCFLAGS :" $(LOCAL_YACCFLAGS)
	@echo "       ARFLAGS   :" $(LOCAL_ARFLAGS)
	@echo "       TARGETLIB :" $(TARGET_LIB)
	@echo "       TARGETBIN :" $(TARGET_BIN)
	@echo "       TARGETINC :" $(TARGET_INCLUDE)

clean :
	@echo "[BSC make] -- Local  cleanning :" $(LOCAL_OBJ)
	@$(RM) -f $(LOCAL_OBJ)

realclean : clean
	@echo "[BSC make] -- Global cleanning :" $(BSC_LIB) $(BSC_H)
	@$(RM) -f $(TARGET_LIB)/$(BSC_LIB) $(TARGET_INCLUDE)/$(BSC_H)
