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

# makefile for hilo driver
# version : 403
# date    : 22/09/94

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

HILO_LIB = $(MHL_LIB)
HILO_C   = drive_hilo.c
HILO_OBJ = drive_hilo.o

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -I$(TARGET_INCLUDE) \
	-D$(AVERTEC_LICENSE) -DMLO_H='<$(MLO_H)>' -DMUT_H='<$(MUT_H)>'

distrib : $(TARGET_LIB)/$(HILO_LIB)

headers :

$(TARGET_LIB)/$(HILO_LIB) : $(HILO_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(HILO_LIB) $(HILO_OBJ) 
	$(RANLIB) $(TARGET_LIB)/$(HILO_LIB)

$(HILO_OBJ) : $(HILO_C)
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) -c $(HILO_C)

clean :
	$(RM) -f $(HILO_OBJ) 

realclean : clean
	$(RM) -f $(TARGET_LIB)/$(HILO_LIB)


