/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* 
 * Purpose : mbk_sys.c header file for functions declarations
 * Date    : 18/12/91
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 * $Id: mbk_sys.h,v 1.6 2004/01/08 18:14:36 antony Exp $
 */

#ifndef _MBK_SYS_H_
#define _MBK_SYS_H_

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

  extern    FILE* mbkfopen __P((const char *name, const char *extension, const char *mode));
  extern    FILE* mbkfopen_ext __P((const char *name, const char *extension, const char *mode, char access, char allowcompress ));
  extern    FILE* mbkfopentrace __P((const char *name, const char *extension, const char *mode ,char access, char allowcompress ));
  extern    void  mbkfopen_infos __P((FILE*, const char*, const char*, const char*, const char*));
  extern    void* mbkalloc __P((size_t nbytes));
  extern    void* mbkrealloc __P((void *pt, unsigned int nbytes));
  extern    void  mbkfree __P((void *ptr));
  void ps();
  extern    void  mbkackchld __P((int));

  extern int mbkpprint __P((int fildes, void *data, int size));
  extern int mbkpscan __P((int fildes, void *data, int size));
  extern pid_t mbkpcreate __P((int *file_des_write, int *file_des_read));

#endif

