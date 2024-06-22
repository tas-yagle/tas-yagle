include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

XYAGLE_BIN = xyagle
XYA_HEADER = xya_lib.h

XYAGLE_NAME = Xyagle
XYAGLE_ABOUT   = Graphical Cone & Netlist Viewer

LOCAL_INC = $(INCLUDE64BIT) -I./ -I$(TARGET_INCLUDE)\
            -I$(MOTIF_INCLUDE) -I$(X11_INCLUDE) -I$(XPM_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS) -D$(AVERTEC_OS)

# /*------------------------------------------------------------\
# |                       Preprocessor defines                  |
# \------------------------------------------------------------*/

LOCAL_X11_DEFINE   = -D_NO_PROTO 

LOCAL_MBK_DEFINE = -DMLO_H='"$(MLO_H)"' \
                   -DBEH_H='"$(BEH_H)"' \
                   -DCNS_H='"$(CNS_H)"' \
                   -DMUT_H='"$(MUT_H)"' \
                   -DEQT_H='"$(EQT_H)"' \
                   -DELP_H='"$(ELP_H)"' \
                   -DSLB_H='"$(SLB_H)"' \
                   -DTUT_H='"$(TUT_H)"' \
                   -DCBH_H='"$(CBH_H)"' \
                   -DLIB_H='"$(LIB_H)"' \
                   -DTLF_H='"$(TLF_H)"' \
                   -DTTV_H='"$(TTV_H)"' \
                   -DSTM_H='"$(STM_H)"' \
                   -DRCN_H='"$(RCN_H)"' \
                   -DAPI_H='"$(API_H)"' \
                   -DMLU_H='"$(MLU_H)"' \
                   -DMCC_H='"$(MCC_H)"' \
                   -DTLC_H='"$(TLC_H)"' \
                   -DTRC_H='"$(TRC_H)"' \
                   -DLOG_H='"$(LOG_H)"' \
                   -DGSP_H='"$(GSP_H)"' \
                   -DBHL_H='"$(BHL_H)"' \
                   -DBEF_H='"$(BEF_H)"' \
                   -DEFG_H='"$(EFG_H)"' \
                   -DBEG_H='"$(BEG_H)"' \
                   -DBVL_H='"$(BVL_H)"' \
                   -DCGV_H='"$(CGV_H)"' \
                   -DSIM_H='"$(SIM_H)"' \
                   -DAVT_H='"$(AVT_H)"' \
                   -DINF_H='"$(INF_H)"' \
                   -DFCL_H='"$(FCL_H)"' \
                   -DMSL_H='"$(MSL_H)"' \
                   -DYAG_H='"$(YAG_H)"' 


LOCAL_XYAG_DEFINE = -DXTB_H='"XTB.h"' -DXSB_H='"XSB.h"' \
                    -DXMF_H='"XMF.h"' -DXME_H='"XME.h"' \
                    -DXMV_H='"XMV.h"' -DXMT_H='"XMT.h"' \
                    -DXMS_H='"XMS.h"' -DXMH_H='"XMH.h"' \
                    -DXMX_H='"XMX.h"' -DXYA_H='"XYA.h"' \
                    -DXMS_FILE_NAME='".$(XYAGLE_BIN).cfg"' \
                    -DXYAG_TOOL_NAME='"$(XYAGLE_BIN)"' \
                    -DXYAGLE_ABOUT='"$(XYAGLE_ABOUT)"' \
                    -DXYAGLE_NAME='"$(XYAGLE_NAME)"' \
                    -DPATCH_NUM='$(DISTRIB_PATCHVERSION)' \
                    -DAVERTEC -D$(AVERTEC_LICENSE) \
                    -D$(XYAGLE_BIN)

LOCAL_CPPFLAGS = $(CPPFLAGS) $(LOCAL_INC) \
                 $(LOCAL_X11_DEFINE) $(LOCAL_MBK_DEFINE) $(LOCAL_XYAG_DEFINE) 

# /*------------------------------------------------------------\
# |                        Required libraries                   |
# \------------------------------------------------------------*/

LOCAL_MOTIF_LIB = -L$(MOTIF_LIB) -lXm

LOCAL_X11_LIB   = -L$(X11_LIB) -lXt -lX11

LOCAL_XPM_LIB   = -L$(XPM_LIB) -lXpm

REGULAR_LIB	= $(CGV_L) \
                  $(SLB_L) \
                  $(YAG_L) \
                  $(TAS_L) \
                  $(STB_L) \
                  $(LIB_L) \
                  $(TLF_L) \
                  $(TUT_L) \
                  $(RPC_L) \
                  $(SIM_L) \
                  $(GSP_L) \
                  $(EFG_L) \
                  $(MCC_L) \
                  $(CBH_L) \
                  $(EQT_L) \
                  $(TTV_L) \
                  $(BCK_L) \
                  $(STM_L) \
                  $(TRC_L) \
                  $(TLC_L) \
                  $(GEN_L) \
                  $(API_L) \
                  $(FCL_L) \
                  $(INF_L) \
                  $(CNS_L) \
                  $(ELP_L) \
                  $(AVT_L) \
                  $(MLU_L) \
                  $(MCL_L) \
                  $(MAL_L) \
                  $(MSL_L) \
                  $(MGL_L) \
                  $(MVL_L) \
                  $(MEL_L) \
                  $(MHL_L) \
                  $(MLO_L) \
                  $(SPF_L) \
                  $(SPE_L) \
                  $(BEF_L) \
                  $(BGL_L) \
                  $(BVL_L) \
                  $(BEG_L) \
                  $(BHL_L) \
                  $(BEH_L) \
                  $(LOG_L) \
                  $(RCN_L) \
                  $(MUT_L)

LOCAL_MBK_LIB = -L$(TARGET_LIB) $(WHOLE) $(REGULAR_LIB) $(NOWHOLE) -ldl $(PSP_L)

API_STATICLIB = $(FLEXOBJS) $(STATIC_LIB) $(LOCAL_MOTIF_LIB) $(LOCAL_X11_LIB) $(LOCAL_XPM_LIB) \
				-L$(TARGET_LIB) $(WHOLE) $(APIALL_L) $(TCL_L) $(REGULAR_LIB) $(NOWHOLE) $(EDITLINELIBS) -ldl -lm $(FLEXLIBS)

LOCAL_LIB =  $(FLEXOBJS) $(STATIC_LIB) $(LOCAL_MOTIF_LIB) $(LOCAL_X11_LIB) $(LOCAL_XPM_LIB) $(LOCAL_MBK_LIB) $(TCL_L) $(EDITLINELIBS) -lpthread -lm $(FLEXLIBS)

# /*------------------------------------------------------------\
# |                          Local objects                      |
# \------------------------------------------------------------*/

LOCAL_XSB_OBJ = XSB_share.o

LOCAL_XYA_OBJ = XYA_bound.o XYA_error.o XYA_object.o XYA_cgvfig.o \
                XYA_place.o XYA_select.o

LOCAL_XTB_OBJ = XTB_icon.o XTB_panel.o XTB_menu.o XTB_pixmap.o    \
                XTB_cursor.o XTB_dialog.o

LOCAL_XMX_OBJ = XMX_motif.o XMX_color.o XMX_graphic.o XMX_panel.o  \
                XMX_menu.o XMX_grid.o XMX_event.o XMX_scroll.o \
                XMX_cursor.o XMX_view.o XMX_icon.o

LOCAL_XMF_OBJ = XMF_menu.o XMF_file.o

LOCAL_XME_OBJ = XME_edit.o XME_select.o XME_search.o XME_menu.o \
                XME_panel.o XME_beh.o

LOCAL_XMV_OBJ = XMV_view.o XMV_panel.o XMV_menu.o

LOCAL_XMT_OBJ = XMT_menu.o XMT_panel.o

LOCAL_XMS_OBJ = XMS_setup.o XMS_menu.o XMS_panel.o 

LOCAL_XMH_OBJ = XMH_menu.o XMH_panel.o 

LOCAL_OBJ = $(LOCAL_XSB_OBJ) $(LOCAL_XYA_OBJ) $(LOCAL_XTB_OBJ) \
            $(LOCAL_XMX_OBJ) $(LOCAL_XMF_OBJ) $(LOCAL_XME_OBJ) \
            $(LOCAL_XMV_OBJ) $(LOCAL_XMT_OBJ) $(LOCAL_XMS_OBJ) \
            $(LOCAL_XMH_OBJ) xyagle.o

LOCAL_OBJ_LIB = $(LOCAL_XSB_OBJ) $(LOCAL_XYA_OBJ) $(LOCAL_XTB_OBJ) \
                $(LOCAL_XMX_OBJ) $(LOCAL_XMF_OBJ) $(LOCAL_XME_OBJ) \
                $(LOCAL_XMV_OBJ) $(LOCAL_XMT_OBJ) $(LOCAL_XMS_OBJ) \
                $(LOCAL_XMH_OBJ) default_slib.o \
		default_corresp.o

GENERATED_C = default_slib.c default_corresp.c

LOCAL_OBJ = xyagle.o

SOURCES_LIB=$(LOCAL_OBJ_LIB:.o=.c) $(LOCAL_OBJ:.o=.c)

# /*------------------------------------------------------------\
# |                              Xyagle                         |
# \------------------------------------------------------------*/

all : $(TARGET_BIN)/$(XYAGLE_BIN)

lib : $(TARGET_INCLUDE)/$(XYA_H) $(TARGET_LIB)/$(XYA_LIB)

headers : $(TARGET_INCLUDE)/$(XYA_H)

$(TARGET_BIN)/$(XYAGLE_BIN) : $(TARGET_INCLUDE)/$(XYA_H) $(TARGET_LIB)/$(XYA_LIB) $(LOCAL_OBJ) $(TARGET_LIB)/$(YAG_LIB) $(TARGET_LIB)/$(CGV_LIB) $(TARGET_LIB)/$(GEN_LIB) $(TARGET_LIB)/$(FCL_LIB)
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/$(XYAGLE_BIN) $(LOCAL_OBJ) $(LOCAL_OBJ_LIB) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/$(XYAGLE_BIN)

link :
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/$(XYAGLE_BIN) $(LOCAL_OBJ) $(LOCAL_OBJ_LIB) $(LOCAL_LIB)
	$(STRIP) $(TARGET_BIN)/$(XYAGLE_BIN)

api_link :
	$(CC) $(LOCAL_CFLAGS) $(DYNAMIC) -o $(TARGET_BIN)/$(XYAGLE_BIN) $(LOCAL_OBJ) $(LOCAL_OBJ_LIB) $(API_STATICLIB)
	$(STRIP) $(TARGET_BIN)/$(XYAGLE_BIN)
	
$(TARGET_LIB)/$(XYA_LIB) : $(LOCAL_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(XYA_LIB) $(LOCAL_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(XYA_LIB)

$(TARGET_INCLUDE)/$(XYA_H) : $(XYA_HEADER)
	$(CP) $(XYA_HEADER) $(TARGET_INCLUDE)/$(XYA_H)

default_slib.o : default_slib.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c default_slib.c
	
default_slib.c : default.slib slib2c.awk
	$(AWK) -vfname=slib -f slib2c.awk default.slib > default_slib.c

default_corresp.o : default_corresp.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c default_corresp.c

default_corresp.c : slib.corresp slib2c.awk
	$(AWK) -vfname=corresp -f slib2c.awk slib.corresp > default_corresp.c

xyagle.o : xyagle.c xyagle.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c xyagle.c

# /*------------------------------------------------------------\
# |                          Xyag Share Box                     |
# \------------------------------------------------------------*/

XSB_share.o  : XSB.h XSB_share.c XSB_share.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XSB_share.c

# /*------------------------------------------------------------\
# |                          Xyag Base                          |
# \------------------------------------------------------------*/

XYA_bound.o  : XSB.h XYA.h XYA_bound.c XYA_bound.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XYA_bound.c

XYA_error.o  : XSB.h XYA.h XYA_error.c XYA_error.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XYA_error.c

XYA_cgvfig.o : XSB.h XYA.h XYA_cgvfig.c XYA_cgvfig.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XYA_cgvfig.c

XYA_place.o : XSB.h XYA.h XYA_place.c XYA_place.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XYA_place.c

XYA_object.o : XSB.h XYA.h XYA_object.c XYA_object.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XYA_object.c

XYA_select.o : XSB.h XYA.h XYA_select.c XYA_select.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XYA_select.c

# /*------------------------------------------------------------\
# |                         Xyag Tools Box                      |
# \------------------------------------------------------------*/

XTB_cursor.o  : XTB.h XTB_cursor.c XTB_cursor.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XTB_cursor.c

XTB_dialog.o  : XTB.h XTB_dialog.c XTB_dialog.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XTB_dialog.c

XTB_icon.o    : XTB.h XTB_icon.c XTB_icon.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XTB_icon.c

XTB_menu.o    : XTB.h XTB_menu.c XTB_menu.h 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XTB_menu.c

XTB_panel.o   : XTB.h XTB_panel.c XTB_panel.h XTB_dialog.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XTB_panel.c

XTB_pixmap.o  : XTB.h XTB_pixmap.c XTB_pixmap.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XTB_pixmap.c

# /*------------------------------------------------------------\
# |                          Xyag Motif X11                     |
# \------------------------------------------------------------*/

XMX_icon.o    : XSB.h XMX.h XMX_icon.c XMX_icon.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_icon.c

XMX_color.o   : XSB.h XMX.h XMX_color.c XMX_color.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_color.c

XMX_cursor.o  : XSB.h XMX.h XMX_cursor.c XMX_cursor.h XMX_view.h XMX_grid.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_cursor.c

XMX_graphic.o : XSB.h XMX.h XMX_graphic.c XMX_graphic.h XMX_view.h XMX_grid.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_graphic.c

XMX_grid.o    : XSB.h XMX.h XMX_grid.c XMX_grid.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_grid.c

XMX_menu.o    : XSB.h XTB.h XMX.h XMF.h XME.h XMV.h XMS.h XMT.h XMH.h \
                XMX_menu.c XMX_menu.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_menu.c

XMX_motif.o   : XSB.h XTB.h XMX.h XMX_motif.c XMX_motif.h XMX_graphic.h \
                XMX_event.h XMX_menu.h XMX_color.h XMX_panel.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_motif.c

XMX_event.o   : XSB.h XTB.h XYA.h XMX.h XMV.h XMX_event.c XMX_grid.h \
                XMX_event.h XMX_cursor.h XMX_event.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_event.c

XMX_panel.o   : XSB.h XTB.h XMF.h XME.h XMV.h XMT.h XMS.h XMH.h XMX.h \
                XMX_panel.c XMX_panel.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_panel.c

XMX_view.o    : XSB.h XYA.h XMX_view.c XMX_view.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_view.c

XMX_scroll.o  : XSB.h XMX_scroll.c XMX_scroll.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c XMX_scroll.c

# /*------------------------------------------------------------\
# |                          Xyag Menu File                     |
# \------------------------------------------------------------*/

XMF_menu.o   : XSB.h XYA.h XTB.h XMX.h XMF.h XMF_menu.c XMF_menu.h \
               XMF_file.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMF_menu.c

XMF_file.o  :  XSB.h XYA.h XTB.h XMX.h XMF.h XMF_file.c XMF_file.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMF_file.c

# /*------------------------------------------------------------\
# |                          Xyag Menu Edit                     |
# \------------------------------------------------------------*/

XME_beh.o   : XME.h XME_beh.c XME_beh.h 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XME_beh.c

XME_edit.o   : XTB.h XSB.h XMX.h XYA.h XME.h XME_edit.c XME_edit.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XME_edit.c

XME_select.o : XTB.h XSB.h XMX.h XYA.h XME.h XME_select.c XME_select.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XME_select.c

XME_search.o : XTB.h XSB.h XMX.h XYA.h XME.h XME_search.c XME_search.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XME_search.c

XME_menu.o    : XTB.h XSB.h XMX.h XYA.h XME.h  XME_menu.c XME_menu.h \
                XME_edit.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XME_menu.c

XME_panel.o   : XTB.h XSB.h XMX.h XYA.h XME.h  XME_panel.c XME_panel.h \
                XME_edit.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XME_panel.c

# /*------------------------------------------------------------\
# |                          Xyag Menu View                     |
# \------------------------------------------------------------*/

XMV_panel.o   : XSB.h XTB.h XMX.h XMV.h XMV_panel.c XMV_panel.h \
                XMV_view.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMV_panel.c

XMV_menu.o    : XSB.h XTB.h XMX.h XMV.h XMV_menu.c XMV_menu.h\
                XMV_view.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMV_menu.c

XMV_view.o    : XSB.h XTB.h XMX.h XMV.h XMV_view.c XMV_view.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMV_view.c

# /*------------------------------------------------------------\
# |                          Xyag Menu Tools                    |
# \------------------------------------------------------------*/

XMT_menu.o    : XTB.h XSB.h XMX.h XYA.h XMT.h  XMT_menu.c XMT_menu.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMT_menu.c

XMT_panel.o   : XTB.h XSB.h XMX.h XYA.h XMT.h  XMT_panel.c XMT_panel.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMT_panel.c

# /*------------------------------------------------------------\
# |                          Xyag Menu Setup                    |
# \------------------------------------------------------------*/

XMS_menu.o   : XTB.h XSB.h XMS.h XMS_menu.c XMS_menu.h XMS_setup.h 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMS_menu.c

XMS_panel.o  : XTB.h XSB.h XMS.h XMS_panel.c XMS_panel.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMS_panel.c

XMS_setup.o  : XTB.h XSB.h XMX.h XMF.h XMV.h XMT.h XMS.h XMH.h \
               XMS_setup.c XMS_setup.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMS_setup.c

# /*------------------------------------------------------------\
# |                          Xyag Menu Help                     |
# \------------------------------------------------------------*/

XMH_menu.o   : XTB.h XSB.h XMH.h XMH_menu.c XMH_menu.h 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMH_menu.c

XMH_panel.o : XTB.h XSB.h XMH.h XMH_panel.c XMH_panel.h
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c  XMH_panel.c


# /*------------------------------------------------------------\
# |                            Clean                            |
# \------------------------------------------------------------*/

clean :
	-@$(RM) -f $(LOCAL_OBJ) $(LOCAL_OBJ_LIB) $(GENERATED_C) .depend

realclean : clean
	-@$(RM) -f $(TARGET_BIN)/$(XYAGLE_BIN)
	-@$(RM) -f $(TARGET_LIB)/$(XYA_LIB)
	-@$(RM) -f $(TARGET_INCLUDE)/$(XYA_H)

dep: $(SOURCES_LIB) $(SOURCES)
	$(SCC) -MM $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $(SOURCES_LIB) > .depend

-include .depend

