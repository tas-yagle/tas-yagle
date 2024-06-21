#
# This file is part of the Alliance CAD System
# Copyright (C) Laboratoire LIP6 - Département ASIM
# Universite Pierre et Marie Curie
# 
# Home page          : http://www-asim.lip6.fr/alliance/
# E-mail support     : mailto:alliance-support@asim.lip6.fr
# 
# This progam is  free software; you can redistribute it  and/or modify it
# under the  terms of the GNU  General Public License as  published by the
# Free Software Foundation;  either version 2 of the License,  or (at your
# option) any later version.
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


include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_INCLUDE = -I$(TARGET_INCLUDE) -I./

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INCLUDE) \
                 -D$(AVERTEC_LICENSE) \
				 -D$(AVERTEC_OS) \
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
                 -DINF_H='<$(INF_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DMLU_H='<$(MLU_H)>' \
                 -DAPI_H='<$(API_H)>' \
                 -DMSL_H='<$(MSL_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DMUT_H='<$(MUT_H)>'

OBJ = spi_drive.o spi_hash.o spi_int.o spi_parse.o spi2spef.o

all: distrib 

distrib : $(TARGET_INCLUDE)/$(MSL_H) $(TARGET_LIB)/$(MSL_LIB) 

headers : $(TARGET_INCLUDE)/$(MSL_H)

$(TARGET_INCLUDE)/$(MSL_H) : msl.h
	$(CP) msl.h $(TARGET_INCLUDE)/$(MSL_H) 
	
$(TARGET_LIB)/$(MSL_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MSL_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(MSL_LIB)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	$(RM) -f $(OBJ)

realclean : clean
	$(RM) -f $(TARGET_LIB)/$(MSL_LIB) $(TARGET_INCLUDE)/$(MSL_H)
