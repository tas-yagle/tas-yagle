include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INC      = -I. -I$(TARGET_INCLUDE) 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -D$(AVERTEC_LICENSE) \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DBHL_H='<$(BHL_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
				 

LOCAL_LIB	   = -L$(TARGET_LIB) \
      			 $(BEH_L) $(EQT_L) $(LOG_L) $(MUT_L)

LOCAL_OBJECTS  = beh_addbeaux.o  beh_addbebux.o  beh_addbebus.o  \
                 beh_addbefig.o  beh_addbegen.o  beh_addbemsg.o  \
                 beh_addbeout.o  beh_addbepor.o  beh_addbereg.o  \
                 beh_addberin.o  beh_addbiabl.o  beh_addbinod.o  \
                 beh_addbeder.o  beh_addbequad.o beh_delbeaux.o  \
                 beh_delbebux.o  beh_delbebus.o  beh_delbefig.o  \
                 beh_delbegen.o  beh_delbemsg.o  beh_delbeout.o  \
                 beh_delbepor.o  beh_delbereg.o  beh_delberin.o  \
                 beh_delbiabl.o  beh_delbinod.o  beh_delbeder.o  \
                 beh_delbequad.o beh_frebeaux.o  beh_frebebux.o  \
                 beh_frebebus.o  beh_frebefig.o  beh_frebegen.o  \
                 beh_frebemsg.o  beh_frebeout.o  beh_frebepor.o  \
                 beh_frebereg.o  beh_freberin.o  beh_frebiabl.o  \
                 beh_frebinod.o  beh_frebeder.o  beh_rmvbeaux.o  \
                 beh_rmvbebux.o  beh_rmvbebus.o  beh_rmvbefig.o  \
                 beh_rmvbegen.o  beh_rmvbemsg.o  beh_rmvbeout.o  \
                 beh_rmvbepor.o  beh_rmvbereg.o  beh_rmvberin.o  \
                 beh_rmvbeder.o  beh_getgenva.o  beh_globals.o   \
                 beh_dict.o      beh_message.o   beh_error.o     \
                 beh_toolbug.o	 beh_vect.o		 beh_duplicate.o \
				 beh_gnsConflictingName.o beh_warning.o

all : $(TARGET_INCLUDE)/$(BEH_H) $(TARGET_LIB)/$(BEH_LIB)

$(TARGET_INCLUDE)/$(BEH_H) : beh_lib.h
	$(CP) beh_lib.h $(TARGET_INCLUDE)/$(BEH_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $<

test : $(TARGET_BIN)/beh_vect_test 

$(TARGET_BIN)/beh_vect_test : all $(LOCAL_OBJECTS) beh_vect_test.o
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_OBJECTS) beh_vect_test.o -o $(TARGET_BIN)/beh_vect_test $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/beh_vect_test

$(TARGET_LIB)/$(BEH_LIB) : $(LOCAL_OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(BEH_LIB) $(LOCAL_OBJECTS)
	-$(RANLIB) $(TARGET_LIB)/$(BEH_LIB)

clean :
	$(RM) -f $(LOCAL_OBJECTS) beh_vect_test.o

realclean : clean
	$(RM) -f $(TARGET_INCLUDE)/$(BEH_H) $(TARGET_LIB)/$(BEH_LIB) $(TARGET_BIN)/beh_vect_test
