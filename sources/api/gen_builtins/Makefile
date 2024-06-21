include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

WRAP 	= $(TARGET_BIN)/wrap
WRAPPER = gen_builtin_functions_wrap.c
SRC 	= gen_builtin_functions_l.c
HEADER 	= gen_builtin_functions.h
LIB 	= gen_builtin_functions.so
OBJ		= gen_builtin_functions.o gen_builtin_functions_wrap.o
GENERATED = gen_builtin_functions_l.c

LOCAL_INC = -I$(TARGET_INCLUDE)

.PRECIOUS : $(GENERATED)

LOCAL_CFLAGS   =  $(CFLAGS) 
LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 -DAVERTEC -D$(AVERTEC_LICENSE) -DBASIC_TOKEN\
                 -DMUT_H='"$(MUT_H)"'\
                 -DAPI_H='"$(API_H)"'\
                 -DAVT_H='"$(AVT_H)"' 


$(TARGET_LIB)/$(APIGEN_BUILTINS_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(APIGEN_BUILTINS_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(APIGEN_BUILTINS_LIB)
	cp $(HEADER) $(TARGET_API_INCLUDE)

dynamic_link: $(SRC) $(WRAPPER)
	$(SCC) -shared $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(WRAPPER) $(SRC) -o $(TARGET_API_LIB)/$(LIB)
	cp $(HEADER) $(TARGET_API_INCLUDE)

$(WRAPPER): $(HEADER) $(WRAP)
	$(WRAP) -silent -p $(TARGET_API_MAN) $(HEADER) 
	@$(RM) -f gmon.out

.c.o :
	$(CC) $(LOCAL_CFLAGS) -c $(LOCAL_CPPFLAGS) $<

%_l.c : %.c $(TARGET_BIN)/license_api
	$(TARGET_BIN)/license_api $< API_TEST_TOKEN > $@


clean:
	-@$(RM) -f $(WRAPPER)
	-@$(RM) -f $(OBJ) $(GENERATED)

realclean: clean
	-$(RM) -f $(TARGET_API_LIB)/$(LIB) $(TARGET_API_INCLUDE)/$(HEADER)
	-@$(RM) -f $(TARGET_LIB)/$(APIGEN_BUILTINS_LIB) > /dev/null 2>&1
