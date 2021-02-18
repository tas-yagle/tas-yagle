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

LOCAL_INCLUDE = -I. -I$(TARGET_INCLUDE)

LOCAL_CFLAGS = $(CFLAGS)
LOCAL_CPPFLAGS = $(CPPFLAGS) -c $(LOCAL_INCLUDE) \
                 -D$(AVERTEC_LICENSE) \
                 -DMGL_H='<$(MGL_H)>' \
                 -DAVT_H='<$(AVT_H)>' \
                 -DMUT_H='<$(MUT_H)>' \
                 -DMLO_H='<$(MLO_H)>' \
                 -DEQT_H='<$(EQT_H)>' \
                 -DLOG_H='<$(LOG_H)>' \
                 -DRCN_H='<$(RCN_H)>' \
                 -DMLU_H='<$(MLU_H)>'

LOCAL_YFLAGS = $(YACCFLAGS) -d

LOCAL_LEXFLAGS = $(LEXFLAGS)

OBJ = mgl_drive.o mgl_parse.o lex.mgl_scomp.o mgl_scomp.tab.o mgl_util.o mgl_error.o

PRE_OBJ = lex.mgl_scomp.cc mgl_scomp.tab.c mgl_scomp.tab.h

all: $(TARGET_INCLUDE)/$(MGL_H) $(TARGET_LIB)/$(MGL_LIB)

headers : $(TARGET_INCLUDE)/$(MGL_H)

$(TARGET_INCLUDE)/$(MGL_H): mgl_lib.h
	cp mgl_lib.h $(TARGET_INCLUDE)/$(MGL_H)

$(TARGET_LIB)/$(MGL_LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET_LIB)/$(MGL_LIB) $(OBJ)
	$(RANLIB) $(TARGET_LIB)/$(MGL_LIB)

mgl_scomp.tab.h : mgl_scomp.tab.c

mgl_scomp.tab.c : mgl_scomp.yac
	$(YACC) $(LOCAL_YFLAGS) -p mgl_scomp -o mgl_scomp.tab.c mgl_scomp.yac

mgl_scomp.tab.o : mgl_scomp.tab.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

lex.mgl_scomp.cc : mgl_scomp.lex mgl_scomp.tab.h
	$(LEX) $(LOCAL_LEXFLAGS) -Pmgl_scomp mgl_scomp.lex

lex.mgl_scomp.o : lex.mgl_scomp.cc
	$(CPLUSPLUS) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

mgl_drive.o : mgl_drive.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

mgl_parse.o : mgl_parse.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

mgl_util.o : mgl_util.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

mgl_error.o : mgl_error.c
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS) $<

clean :
	-@$(RM) -f $(OBJ) $(PRE_OBJ) > /dev/null 2>&1

realclean : clean
	-@$(RM) -f $(TARGET_LIB)/$(MGL_LIB) $(TARGET_INCLUDE)/$(MGL_H) > /dev/null 2>&1
	-@$(RM) -f mgl_scomp.output
