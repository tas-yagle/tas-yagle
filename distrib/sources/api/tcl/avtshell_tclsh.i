// $Header: /users10/chaos1/sources/sources/distrib/sources/api/tcl/avtshell_tclsh.i,v 1.12 2008/04/15 14:49:08 fabrice Exp $
//
// SWIG File for building new tclsh program
// Dave Beazley
// April 25, 1996
//

#ifdef AUTODOC
%subsection "tclsh.i"
%text %{
This module provides the Tcl_AppInit() function needed to build a 
new version of the tclsh executable.   This file should not be used
when using dynamic loading.   To make an interface file work with
both static and dynamic loading, put something like this in your
interface file :

     #ifdef STATIC
     %include tclsh.i
     #endif
%}
#endif

%{

/* A TCL_AppInit() function that lets you build a new copy
 * of tclsh.
 *
 * The macro SWIG_init contains the name of the initialization
 * function in the wrapper file.
 */

char *avtshell_RcFileName = "~/.avt_shellrc";

static char avtshell_initScript[] = 
"if {[namespace which -command tclInit] eq \"\"} {\n"
"  proc tclInit {} {\n"
"    global tcl_libPath tcl_library env tclDefaultLibrary\n"
"    set tcl_library $env(AVTDISTPATH)\n"
"    rename tclInit {}\n"
"    if {[info exists tcl_library]} {\n"
"	set scripts {{set tcl_library}}\n"
"    } else {\n"
"	set scripts {}\n"
"	if {[info exists env(TCL_LIBRARY)] && ($env(TCL_LIBRARY) ne {})} {\n"
"	    lappend scripts {set env(TCL_LIBRARY)}\n"
"	    lappend scripts {\n"
"if {[regexp ^tcl(.*)$ [file tail $env(TCL_LIBRARY)] -> tail] == 0} continue\n"
"if {$tail eq [info tclversion]} continue\n"
"file join [file dirname $env(TCL_LIBRARY)] tcl[info tclversion]}\n"
"	}\n"
"	if {[info exists tclDefaultLibrary]} {\n"
"	    lappend scripts {set tclDefaultLibrary}\n"
"	} else {\n"
"	    lappend scripts {::tcl::pkgconfig get scriptdir,runtime}\n"
"	}\n"
"	lappend scripts {\n"
"set parentDir [file dirname [file dirname [info nameofexecutable]]]\n"
"set grandParentDir [file dirname $parentDir]\n"
"file join $parentDir lib tcl[info tclversion]} \\\n"
"	{file join $grandParentDir lib tcl[info tclversion]} \\\n"
"	{file join $parentDir library} \\\n"
"	{file join $grandParentDir library} \\\n"
"	{file join $grandParentDir tcl[info patchlevel] library} \\\n"
"	{\n"
"file join [file dirname $grandParentDir] tcl[info patchlevel] library}\n"
"	if {[info exists tcl_libPath]\n"
"		&& [catch {llength $tcl_libPath} len] == 0} {\n"
"	    for {set i 0} {$i < $len} {incr i} {\n"
"		lappend scripts [list lindex \\$tcl_libPath $i]\n"
"	    }\n"
"	}\n"
"    }\n"
"    set dirs {}\n"
"    set errors {}\n"
"    foreach script $scripts {\n"
"	lappend dirs [eval $script]\n"
"	set tcl_library [lindex $dirs end]\n"
"	set tclfile [file join $tcl_library init.tcl]\n"
"	if {[file exists $tclfile]} {\n"
"	    if {[catch {uplevel #0 [list source $tclfile]} msg opts]} {\n"
"		append errors \"$tclfile: $msg\n\"\n"
"		append errors \"[dict get $opts -errorinfo]\n\"\n"
"		continue\n"
"	    }\n"
"	    unset -nocomplain tclDefaultLibrary\n"
"	    return\n"
"	}\n"
"    }\n"
"    unset -nocomplain tclDefaultLibrary\n"
"    set msg \"Can't find a usable init.tcl in the following directories: \n\"\n"
"    append msg \"    $dirs\n\n\"\n"
"    append msg \"$errors\n\n\"\n"
"    append msg \"This probably means that avt_shell wasn't installed properly.\n\"\n"
"    error $msg\n"
"  }\n"
"}\n"
;


#ifdef MAC_TCL
extern int		MacintoshInit _ANSI_ARGS_((void));
#endif

#include "eltclsh.h"
#include AVT_H

int avt_shell_AppInit(ElTclInterpInfo *iinfo) {
   Tcl_Obj *obj;

   TclSetPreInitScript (avtshell_initScript);
//   Tcl_SetVar(iinfo->interp, (char *) "tcl_library",avt_gettcldistpath(),TCL_GLOBAL_ONLY);
   TCL_INTERPRETER=iinfo->interp;

   /* configure standard path for packages */
   obj = Tcl_NewListObj(0, NULL);
   Tcl_ListObjAppendElement(iinfo->interp, obj, Tcl_NewStringObj(avt_gettcldistpath(), -1));
   TclSetLibraryPath(obj);

  if (elTclAppInit(iinfo) == TCL_ERROR)
    return TCL_ERROR;

  /* Now initialize our functions */

  if (SWIG_init(iinfo->interp) == TCL_ERROR)
    return TCL_ERROR;
#if TCL_MAJOR_VERSION > 7 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION >= 5
   Tcl_SetVar(iinfo->interp, (char *) "tcl_rcFileName",avtshell_RcFileName,TCL_GLOBAL_ONLY);
#else
   tcl_RcFileName = avtshell_RcFileName;
#endif
#ifdef SWIG_RcRsrcName
  Tcl_SetVar(iinfo->interp, (char *) "tcl_rcRsrcName",SWIG_RcRsrcName,TCL_GLOBAL);
#endif
  
  return TCL_OK;
}

#if TCL_MAJOR_VERSION > 7 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION >= 4
int main(int argc, char **argv) {
#ifdef MAC_TCL
    char *newArgv[2];
    
    if (MacintoshInit()  != TCL_OK) {
	Tcl_Exit(1);
    }

    argc = 1;
    newArgv[0] = "tclsh";
    newArgv[1] = NULL;
    argv = newArgv;
#endif

  elTclshLoop(argc, argv, avt_shell_AppInit);
  return(0);

}
#else
extern int main();
#endif

%}

