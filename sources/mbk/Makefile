# This file is part of the Alliance CAD System
# Copyright (C) Laboratoire LIP6 - Département ASIM
# Universite Pierre et Marie Curie
# 
# Home page          : http://www-asim.lip6.fr/alliance/
# E-mail support     : mailto:alliance-support@asim.lip6.fr
# 
# This library is free software; you  can redistribute it and/or modify it
# under the terms  of the GNU Library General Public  License as published
# by the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
# 
# Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
# useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.
# 
# You should have received a copy  of the GNU General Public License along
# with the GNU C Library; see the  file COPYING. If not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 

#
# Purpose : makefile for mbk libraries
# Date    : 31/08/93
# Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
# Modifications included for the TARGET macros 03/03/95
# 
# "$Id: Makefile,v 1.41 2008/04/14 15:57:44 anthony Exp $";
# 

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

MUTOBJ		 = mbk_sys.o 			\
			   mbk_util.o			\
			   mbk_cache.o 			\
			   mbk_advanced.o		\
			   mbk_int.o			\
			   mbk_math.o			\
			   mbk_fileextract.o	\
			   mbk_vector.o			\
			   mbk_string_tools.o   \
			   mbk_stats.o          \
			   mbk_more_lofig_util.o\
			   mbk_matrix.o         \
			   mbk_regex.o          \
			   mbk_comm.o           \
			   mbk_crypt.o          \
			   mbk_rijndael.o       \
			   mbk_md5.o       \
			   mbk_board.o

OBJ          = mbk_ph.o      		\
               mbk_ph_util.o 		\
               mbk_lo.o      		\
               mbk_lo_util.o 		\
               rcn_lo.o      		\
               rcn_lo_util.o 		\
               rcn_cache.o   		\
			   $(MUTOBJ)

HEADERS      = $(TARGET_INCLUDE)/$(MLU_H) \
               $(TARGET_INCLUDE)/$(MLO_H) \
               $(TARGET_INCLUDE)/$(MPU_H) \
               $(TARGET_INCLUDE)/$(MPH_H) \
               $(TARGET_INCLUDE)/$(RCN_H) \
               $(TARGET_INCLUDE)/$(MUT_H)

LIBRARIES    = $(MPH_LIB)    \
               $(MPU_LIB)    \
               $(MLO_LIB)    \
               $(MLU_LIB)    \
               $(MUT_LIB)    \
               $(RCN_LIB)

RCSSOURCES   = mbk_ph.c      \
               mbk_lo.c      \
               mbk_util.c    \
               mbk_lo_util.c \
               mbk_ph_util.c \
               mbk_sys.c     \
               mbk_cache.c   \
               mbk_ph.h      \
               mbk_lo.h      \
               mbk_util.h    \
               mbk_lo_util.h \
               mbk_ph_util.h \
               mbk_sys.h     \
               mbk_cache.h   \
               mbk_int.h   \
               rcn_lo.c      \
               rcn_lo.h      \
               rcn_lo_util.c \
               rcn_lo_util.h \
               rcn_cache.c   \
               rcn_cache.h   \
               $(HEADERS)    \
               Makefile

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -D$(AVERTEC_OS) \
               -D$(AVERTEC_LICENSE) \
               -I$(TARGET_INCLUDE)  \
               -DEQT_H='"$(EQT_H)"' \
               -DLOG_H='"$(LOG_H)"' \
               -DMCC_H='"$(MCC_H)"' \
               -DTTV_H='<$(TTV_H)>' \
               -DSTM_H='<$(STM_H)>' \
               -DSIM_H='<$(SIM_H)>' \
               -DCNS_H='<$(CNS_H)>' \
               -DGSP_H='<$(GSP_H)>' \
               -DEFG_H='<$(EFG_H)>' \
               -DCBH_H='<$(CBH_H)>' \
               -DBEH_H='<$(BEH_H)>' \
               -DBEG_H='<$(BEG_H)>' \
               -DELP_H='<$(ELP_H)>' \
               -DMLU_H='"$(MLU_H)"' \
               -DMLO_H='"$(MLO_H)"' \
               -DMPU_H='"$(MPU_H)"' \
               -DMPH_H='"$(MPH_H)"' \
               -DRCN_H='"$(RCN_H)"' \
               -DAVT_H='"$(AVT_H)"' \
               -DINF_H='"$(INF_H)"' \
               -DAPI_H='<$(API_H)>' \
               -DMSL_H='"$(MSL_H)"' \
               -DMUT_H='"$(MUT_H)"'

# Overall target to ensure up to date libraries
all: $(TARGET_LIB)/$(MPH_LIB) \
     $(TARGET_LIB)/$(MPU_LIB) \
     $(TARGET_LIB)/$(MLO_LIB) \
     $(TARGET_LIB)/$(MLU_LIB) \
     $(TARGET_LIB)/$(MUT_LIB) \
     $(TARGET_LIB)/$(RCN_LIB)
	 
distrib : all

# Archive construction
$(TARGET_LIB)/$(MPH_LIB) : mbk_ph.o
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MPH_LIB) $?
	$(RANLIB) $(TARGET_LIB)/$(MPH_LIB)

$(TARGET_LIB)/$(MPU_LIB) : mbk_ph_util.o
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MPU_LIB) $?
	$(RANLIB) $(TARGET_LIB)/$(MPU_LIB)

$(TARGET_LIB)/$(MLO_LIB) : mbk_lo.o
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MLO_LIB) $?
	$(RANLIB) $(TARGET_LIB)/$(MLO_LIB)

$(TARGET_LIB)/$(MLU_LIB) : mbk_lo_util.o
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MLU_LIB) $?
	$(RANLIB) $(TARGET_LIB)/$(MLU_LIB)

$(TARGET_LIB)/$(MUT_LIB) : $(MUTOBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MUT_LIB) $?
	$(RANLIB) $(TARGET_LIB)/$(MUT_LIB)

$(TARGET_LIB)/$(RCN_LIB) : rcn_lo.o rcn_lo_util.o rcn_cache.o
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(RCN_LIB) $?
	$(RANLIB) $(TARGET_LIB)/$(RCN_LIB)

# Object file compilation
mbk_ph.o : mbk_ph.c mbk_ph.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_ph.c

mbk_ph_util.o : mbk_ph_util.c mbk_ph_util.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_ph_util.c

mbk_lo.o : mbk_lo.c mbk_lo.h $(HEADER)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_lo.c

mbk_lo_util.o : mbk_lo_util.c mbk_lo_util.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_lo_util.c

mbk_sys.o : mbk_sys.c mbk_sys.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_sys.c

mbk_util.o : mbk_util.c mbk_util.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_util.c

mbk_math.o : mbk_math.c mbk_math.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_math.c

mbk_string_tools.o : mbk_string_tools.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_string_tools.c

mbk_stats.o : mbk_stats.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_stats.c

mbk_more_lofig_util.o : mbk_more_lofig_util.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_more_lofig_util.c

mbk_advanced.o : mbk_advanced.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_advanced.c

mbk_matrix.o : mbk_matrix.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_matrix.c

mbk_regex.o : mbk_regex.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_regex.c

mbk_board.o : mbk_board.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_board.c

mbk_comm.o : mbk_comm.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_comm.c

mbk_cache.o : mbk_cache.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_cache.c

mbk_int.o : mbk_int.c $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_int.c

mbk_fileextract.o : mbk_fileextract.c mbk_fileextract.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_fileextract.c
	
mbk_vector.o : mbk_vector.c mbk_vector.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_vector.c

mbk_crypt.o : mbk_crypt.c mbk_crypt.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_crypt.c

mbk_rijndael.o : mbk_rijndael.c mbk_rijndael.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_rijndael.c

mbk_md5.o : mbk_md5.c mbk_md5.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c mbk_md5.c

rcn_lo.o : rcn_lo.c rcn_lo.h rcn.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c rcn_lo.c

rcn_cache.o : rcn_cache.c rcn_cache.h rcn.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c rcn_cache.c

rcn_lo_util.o : rcn_lo_util.c rcn_lo_util.h rcn.h $(HEADERS)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c rcn_lo_util.c

headers : $(TARGET_INCLUDE)/$(MPH_H) $(TARGET_INCLUDE)/$(MPU_H) $(TARGET_INCLUDE)/$(MLO_H) $(TARGET_INCLUDE)/$(MLU_H) $(TARGET_INCLUDE)/$(MUT_H) $(TARGET_INCLUDE)/$(RCN_H)

$(TARGET_INCLUDE)/$(MPH_H) : mph_lib.h
	$(CP) mph_lib.h $(TARGET_INCLUDE)/$(MPH_H) 
        
$(TARGET_INCLUDE)/$(MPU_H) : mpu_lib.h
	$(CP) mpu_lib.h $(TARGET_INCLUDE)/$(MPU_H)
        
$(TARGET_INCLUDE)/$(MLO_H) : mlo_lib.h
	$(CP) mlo_lib.h $(TARGET_INCLUDE)/$(MLO_H)
        
$(TARGET_INCLUDE)/$(MLU_H) : mlu_lib.h
	$(CP) mlu_lib.h $(TARGET_INCLUDE)/$(MLU_H) 
        
$(TARGET_INCLUDE)/$(MUT_H) : mut_lib.h
	$(CP) mut_lib.h $(TARGET_INCLUDE)/$(MUT_H)
        
$(TARGET_INCLUDE)/$(RCN_H) : rcnx.h rcn_lo.h rcn_lo_util.h rcn_cache.h
	( echo "/* This is a generated file. Do not edit */" ; echo ; cat rcnx.h ; cat rcn_lo.h rcn_lo_util.h rcn_cache.h | grep "^extern" ; echo "#endif" ) > $(TARGET_INCLUDE)/$(RCN_H)

clean : 
	$(RM) -f $(OBJ)

realclean: clean
	-@for i in $(LIBRARIES); do                  \
	   $(RM) -f $(TARGET_LIB)/$$i ;              \
   done;                                             \
   for i in $(HEADERS); do                           \
	   $(RM) -f $$i ;                            \
   done;                                            
