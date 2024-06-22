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
 
/*******************************************************************************
* mbk     : generic_target.h chooses correct includes for each target          *
*                                                                              *
* version : 5.00                                                               *
* date    : 10/05/95                                                           *
*******************************************************************************/

#ifndef _generic_target_
#define _generic_target_

#ifdef pc
#include <math.h>
#include <stdlib.h>
#include <time.h>
#undef HUGE_VAL  /* I do love that kind of stuff */
#include <limits.h>
#elif sv4
#include <math.h>
#include <stdlib.h>
#include <time.h>
#undef HUGE_VAL  /* I do love that kind of stuff */
#include <limits.h>
#elif dec
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#elif sun
#include <floatingpoint.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#elif ncr
#include <math.h>
#include <malloc.h>
#include <sys/types.h>
#include <time.h>
#elif sps9
#include <math.h>
#include <malloc.h>
#include <sys/types.h>
#include <time.h>
#else /* default value is sparc */
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#endif

#endif /* !_generic_target_ */
