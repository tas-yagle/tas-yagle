# makefile for xal libraries
# date    : 27/09/97
 
include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

XAL_HEADER     = xal_lib.h

LOCAL_INC = -I$(TARGET_INCLUDE) \
            -I$(MOTIF_INCLUDE) \
            -I$(X11_INCLUDE) \
            -I$(XPM_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DMUT_H='"$(MUT_H)"' \
                 -DXAL_H='"$(XAL_H)"'

XAL_OBJ        = ForceUpdate.o FrontPage.o OnVersion.o BuildMenus.o \
                 Cursor.o SetIcon.o LimitedLoop.o Boxes.o ButtonMenus.o \
		 PixmapLabel.o

all : $(TARGET_INCLUDE)/$(XAL_H) $(TARGET_LIB)/$(XAL_LIB)

headers : $(TARGET_INCLUDE)/$(XAL_H)

$(TARGET_LIB)/$(XAL_LIB) : $(XAL_OBJ) 
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(XAL_LIB) $(XAL_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(XAL_LIB)

$(TARGET_INCLUDE)/$(XAL_H) :
	$(CP) $(XAL_HEADER) $(TARGET_INCLUDE)/$(XAL_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(XAL_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(XAL_H)

clean :
	-$(RM) -f $(XAL_OBJ)
