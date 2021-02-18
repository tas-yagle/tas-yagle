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

# makefile for vti logical and physical views
# version : 4.9
# date    : 23/06/93
# slight modification to include the TARGET_LIB directory
# Frederic Petrot

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

VERSION     = 5.0
RCSSOURCES  = Makefile \
              drive_vti_l.c \
              drive_vti_p.c \
              parse_vti_l.c \
              drive_vti_l.h \
              drive_vti_p.h \
              parse_vti_p.c
PH_OBJ      = drive_vti_p.o parse_vti_p.o
LO_OBJ      = drive_vti_l.o parse_vti_l.o

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -I$(TARGET_INCLUDE) \
              -D$(AVERTEC_LICENSE) \
              -DMLU_H='"$(MLU_H)"' \
              -DEQT_H='"$(EQT_H)"' \
              -DLOG_H='"$(LOG_H)"' \
              -DMPU_H='"$(MPU_H)"' \
              -DMLO_H='"$(MLO_H)"' \
              -DMPH_H='"$(MPH_H)"' \
              -DMUT_H='"$(MUT_H)"' \
              -DRCN_H='"$(RCN_H)"'

all: $(TARGET_LIB)/$(MCP_LIB) $(TARGET_LIB)/$(MCL_LIB)

distrib : all

$(TARGET_LIB)/$(MCP_LIB) : $(PH_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MCP_LIB) $(PH_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(MCP_LIB)

$(TARGET_LIB)/$(MCL_LIB) : $(LO_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MCL_LIB) $(LO_OBJ)
	$(RANLIB) $(TARGET_LIB)/$(MCL_LIB)

parse_vti_l.o : parse_vti_l.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c parse_vti_l.c 

parse_vti_p.o : parse_vti_p.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c parse_vti_p.c

drive_vti_l.o : drive_vti_l.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -DTECHNO='"idps_symb"' -c drive_vti_l.c

drive_vti_p.o : drive_vti_p.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -DTECHNO='"idps_symb"' -c drive_vti_p.c

checkin : 
	echo "y" > 1
	for i in $(RCSSOURCES); do \
		ci -l -f -m"rcs version number consistency" $$i < 1 ; \
	done
	tail +7 Makefile | head -6 > ./RCS/rcs$(VERSION)
	-$(RM) 1

headers :

clean :
	$(RM) -f $(PH_OBJ) $(LO_OBJ)

realclean: clean
	$(RM) -f $(TARGET_LIB)/$(MCP_LIB) $(TARGET_LIB)/$(MCL_LIB)

