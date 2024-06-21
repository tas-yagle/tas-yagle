# makefile for stm
# date    : 15/05/98

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

STM_BIN = stm

SHELL = /bin/sh

STM_OBJ     = stm_main.o

STM_H_LIB   = stm_cache_model.h			 \
			  stm_capa.h                 \
			  stm_cell.h                 \
			  stm_cell_print.h           \
			  stm_curve.h                \
			  stm_curve_mcc.h            \
			  stm_curve_plot.h           \
              stm_drive.h                \
              stm_env.h                  \
              stm_ht.h                   \
              stm_name_interface.h       \
              stm_noise.h                \
              stm_noise_print.h          \
              stm_noise_scr.h            \
              stm_noise_scr_print.h      \
              stm_energy.h               \
              stm_energy_print.h         \
              stm_energy_table.h         \
              stm_energy_table_print.h   \
              stm_energy_params.h        \
              stm_energy_params_print.h  \
              stm_mod.h                  \
			  stm_mod_display.h	         \
			  stm_mod_eval.h             \
              stm_mod_merge.h            \
              stm_mod_meta.h             \
              stm_mod_modif.h            \
              stm_mod_name.h             \
              stm_mod_print.h            \
              stm_mod_sign.h             \
              stm_modiv.h                \
              stm_modiv_eval.h           \
              stm_modiv_print.h          \
              stm_modfct.h               \
              stm_modfct_eval.h          \
              stm_modfct_print.h         \
              stm_modfct_share.h         \
              stm_modpln.h               \
              stm_modpln_eval.h          \
              stm_modpln_print.h         \
              stm_modpln_sign.h          \
			  stm_modtbl.h               \
              stm_modtbl_eval.h          \
              stm_modtbl_print.h         \
              stm_modtbl_merge.h         \
			  stm_modtbl_meta.h          \
              stm_modtbl_modif.h         \
              stm_modtbl_templ.h         \
			  stm_modtbl_sign.h          \
			  stm_modscm.h               \
			  stm_modscm_cst.h           \
			  stm_modscm_cst_eval.h      \
			  stm_modscm_cst_print.h     \
			  stm_modscm_cst_sign.h      \
			  stm_modscm_dual.h          \
			  stm_modscm_dual_eval.h     \
			  stm_modscm_dual_print.h    \
			  stm_modscm_dual_sign.h     \
			  stm_modscm_eval.h          \
			  stm_modscm_false.h         \
			  stm_modscm_false_eval.h    \
			  stm_modscm_false_print.h   \
			  stm_modscm_false_sign.h    \
			  stm_modscm_good.h          \
			  stm_modscm_good_eval.h     \
			  stm_modscm_good_print.h    \
			  stm_modscm_good_sign.h     \
			  stm_modscm_path.h          \
			  stm_modscm_path_eval.h     \
			  stm_modscm_path_print.h    \
			  stm_modscm_path_sign.h     \
			  stm_modscm_print.h         \
			  stm_modscm_sign.h          \
              stm_parse.h                \
              stm_prop.h                 \
              stm_prop_eval.h            \
              stm_simu.h                 \
              stm_solver.h               \
              stm_string.h               \
              stm_pwth.h                 \
              stm_pwl.h

STM_OBJ_LIB = $(STM_H_LIB:%.h=%.o) lex.stm.o stm.tab.o lex.stmmodel.o stmmodel.tab.o
			  
LOCAL_INC = -I$(TARGET_INCLUDE)
			
LOCAL_LIB  = -L$(TARGET_LIB) \
               $(STM_L) \
               $(API_L) \
               $(GEN_L) \
               $(AVT_L) \
               $(MCC_L) \
               $(ELP_L) \
               $(SIM_L) \
               $(EQT_L) \
               $(RCN_L) \
               $(MLO_L) \
               $(MLU_L) \
               $(MCL_L) \
               $(MAL_L) \
               $(MSL_L) \
               $(MGL_L) \
               $(MVL_L) \
               $(MEL_L) \
               $(MHL_L) \
               $(MLO_L) \
               $(RCN_L) \
               $(SPF_L) \
               $(SPE_L) \
               $(MUT_L) \
               $(EQT_L) \
               $(LOG_L) \
               -ldl -lm 

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INC) \
                 -D$(AVERTEC_OS) \
                 -DAVERTEC \
                 -D$(AVERTEC_LICENSE) \
                 -DSTM_H='"$(STM_H)"' \
                 -DBEH_H='<$(BEH_H)>' \
                 -DBEG_H='<$(BEG_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DCNS_H='<$(CNS_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DCBH_H='<$(CBH_H)>' \
                 -DGSP_H='<$(GSP_H)>' \
                 -DEFG_H='<$(EFG_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DINF_H='<$(INF_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DGEN_H='"$(GEN_H)"' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMCC_H='<$(MCC_H)>' \
                 -DELP_H='<$(ELP_H)>' \
                 -DSIM_H='<$(SIM_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMUT_H='<$(MUT_H)>'

LOCAL_YFLAGS = $(YACCFLAGS) -d  #-t -v

all : $(TARGET_INCLUDE)/$(STM_H) $(TARGET_LIB)/$(STM_LIB)

headers : $(TARGET_INCLUDE)/$(STM_H)

$(TARGET_LIB)/$(STM_LIB) : $(STM_OBJ_LIB)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(STM_LIB) $(STM_OBJ_LIB)
	$(RANLIB) $(TARGET_LIB)/$(STM_LIB)
	
# bin 

bin : $(TARGET_BIN)/$(STM_BIN)

$(TARGET_BIN)/$(STM_BIN) : $(STM_OBJ) $(TARGET_INCLUDE)/$(STM_H)
	$(CC) $(LOCAL_CFLAGS) $(STM_OBJ) -o $(TARGET_BIN)/$(STM_BIN) $(LOCAL_LIB)

# obj

$(TARGET_INCLUDE)/$(STM_H) : stmx.h $(STM_H_LIB)
	$(CP) stmx.h $(TARGET_INCLUDE)/$(STM_H)
	$(AWK) '{if ($$1 == "extern" || $$1 == "#ifdef" || $$1 == "#ifndef" || $$1 == "#else" || $$1 == "#endif") print; next}' stm_*.h >> $(TARGET_INCLUDE)/$(STM_H)
	$(ECHO) "#endif" >> $(TARGET_INCLUDE)/$(STM_H)
	
stm.tab.h : stm.tab.c

stm.tab.c : stm.y
	$(YACC) $(LOCAL_YFLAGS) -b stm -p stm stm.y

lex.stm.c : stm.l stm.tab.h
	$(LEX) $(LEXFLAGS) -Pstm stm.l

stmmodel.tab.h : stmmodel.tab.c

stmmodel.tab.c : stm_model.y
	$(YACC) $(LOCAL_YFLAGS) -b stmmodel -p stmmodel stm_model.y

lex.stmmodel.c : stm_model.l stmmodel.tab.h
	$(LEX) $(LEXFLAGS) -Pstmmodel stm_model.l

.cc.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)  $<

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(STM_LIB)
	-$(RM) -f $(TARGET_INCLUDE)/$(STM_H)

clean : 
	-$(RM) -f $(STM_OBJ_LIB) $(STM_OBJ) lex.stm.c stm.tab.h stm.tab.c lex.stmmodel.c stmmodel.tab.h stmmodel.tab.c 

	
