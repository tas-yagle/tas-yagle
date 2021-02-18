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

LOCAL_CFLAGS      = $(CFLAGS)
LOCAL_CPPFLAGS    = $(CPPFLAGS) -c -I$(TARGET_INCLUDE) \
                    -D$(AVERTEC_LICENSE) -DAVT_H='<$(AVT_H)>' -DMUT_H='<$(MUT_H)>' -DMLO_H='<$(MLO_H)>'
LOCAL_YACCFLAGS   = $(YACCFLAGS) -d 
LOCAL_LEXFLAGS    = $(LEXFLAGS)

LOCAL_ARFLAGS     = $(ARFLAGS)

LOCAL_OBJECTS    = mvl_parse.o mvl_drive.o \
                   mvl_scomp.tab.o lex.mvl_scomp.o    \
                   mvl_sspec.o mvl_util.o

$(TARGET_LIB)/$(MVL_LIB) : $(LOCAL_OBJECTS)
	$(AR) $(LOCAL_ARFLAGS) $(TARGET_LIB)/$(MVL_LIB) $(LOCAL_OBJECTS)
	-$(RANLIB) $(TARGET_LIB)/$(MVL_LIB)

.c.o : 
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

mvl_scomp.tab.h : mvl_scomp.tab.c

mvl_scomp.tab.c : mvl_scomp.y
	$(YACC) $(LOCAL_YACCFLAGS) -p mvl_scomp -b mvl_scomp mvl_scomp.y

lex.mvl_scomp.c : mvl_scomp.l mvl_scomp.tab.h
	$(LEX) $(LOCAL_LEXFLAGS) -Pmvl_scomp mvl_scomp.l

headers :

clean :
	-$(RM) -f $(LOCAL_OBJECTS)
	-$(RM) -f mvl_scomp.tab.c mvl_scomp.tab.h lex.mvl_scomp.c mvl_scomp.output

realclean : clean
	-$(RM) -f $(TARGET_LIB)/$(MVL_LIB)

