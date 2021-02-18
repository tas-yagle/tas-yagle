
/* ###--------------------------------------------------------------### */
/* file		: bhl110.h						*/
/* date		: Oct 30 1995						*/
/* version	: v110							*/
/* author	: Pirouz BAZARGAN SABET					*/
/* contents	: high level library for behavioural description	*/
/* ###--------------------------------------------------------------### */

#ifndef BEH_BHLDEF
#define BEH_BHLDEF

	/* ###------------------------------------------------------### */
	/*    defines							*/
	/* ###------------------------------------------------------### */

	/* ###------------------------------------------------------### */
	/*    structure definitions					*/
	/* ###------------------------------------------------------### */

	/* ###------------------------------------------------------### */
	/*    functions							*/
	/* ###------------------------------------------------------### */

extern int           beh_chkbefig   ();
extern void          beh_debug      ();
extern void          beh_makbdd     ();
extern void          beh_makgex     ();
extern void          beh_makderiv   ();
extern void          beh_makvarlist ();
extern void          beh_makquad    ();
extern void          beh_freabl     ();
extern void          beh_depend     ();
extern struct chain *beh_namelist   ();
extern struct chain *beh_unamlist   ();
extern void          beh_indexbdd   ();

#endif
