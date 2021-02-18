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

#               ,,,
#              (o o)
# ####=====oOO--(_)--OOO==========================================####
# ##                                                                ##
# ##   File            :  Makefike for LOGIC                        ##
# ##   Author          :  Olivier SIROL                             ##
# ##   Date            :  Oct 1996                                  ##
# ##                                                                ##
# ##   E-mail support  :  cao-vlsi@masi.ibp.fr                      ##
# ##                                                                ##
# ####============================================================####
#
#
# $Id: Makefile,v 1.10 2005/05/03 08:26:28 antony Exp $
#
#

include $(AVERTEC_TOP)/etc/$(AVERTEC_OS).mk
include $(AVERTEC_TOP)/etc/libraries.mk
include $(AVERTEC_TOP)/etc/Avertec.mk

LOCAL_OBJ     = log_bdd0.o log_bdd1.o log_thashbdd.o log_thashloc.o \
				log_prefbib.o log_vectabl.o

LOCAL_CFLAGS   = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c -I. -I$(TARGET_INCLUDE) -D$(AVERTEC_LICENSE) \
				 -DLOG_H='"$(LOG_H)"' -DMUT_H='"$(MUT_H)"' \
				 -DAVT_H='"$(AVT_H)"'

all : $(TARGET_INCLUDE)/$(LOG_H) $(TARGET_LIB)/$(LOG_LIB)

headers : $(TARGET_INCLUDE)/$(LOG_H)

.c.o :
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

$(TARGET_INCLUDE)/$(LOG_H) : log_lib.h
	$(CP) log_lib.h $(TARGET_INCLUDE)/$(LOG_H)

$(TARGET_LIB)/$(LOG_LIB) : $(LOCAL_OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(LOG_LIB) $(LOCAL_OBJ)
	-$(RANLIB) $(TARGET_LIB)/$(LOG_LIB)


clean :
	-$(RM) $(LOCAL_OBJ)

realclean : clean
	-$(RM) $(TARGET_INCLUDE)/$(LOG_H) $(TARGET_LIB)/$(LOG_LIB)



