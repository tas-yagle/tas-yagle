/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_help.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

#include "xtas.h"

/*--------------------------------------------------------------------------*/
/*                             DEFINES                                      */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             GLOBALS                                      */
/*--------------------------------------------------------------------------*/

char *HELP_MESSAGES[HELP_SOURCES][HELP_TOPICS] = {
	/*  1. Desk window */
	{
		/* Main    */ "",
		/* File    */ 
"\tHelp on File Menu of Xtas Main Desk \n\
\n\t1. Open (first toolbar button) \n\
\tSelect the file to be analysed. \n\
\n\t2. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.\n\
\n\t3. Exit (second toolbar button) \n\
\tExit Xtas.",
		/* Tools   */ 
"\tHelp on Tools Menu of Xtas Main Desk  \n\
\n\t1. Get Paths (ninth toolbar button) \n\
\t This menu allows the browsing of all timing paths  \n\
\t through the loaded figure, and through the hierarchy  \n\
\t below it. Paths extremities are connectors, register  \n\
\t signals, command signals, precharged points and break  \n\
\t points.   \n\
\n\t2. Get Delays (tenth toolbar button)  \n\
\t This menu allows the browsing of elementary delays  \n\
\t in the loaded figure, and in the hierarchy below it.   \n\
\n\t3. Stability (eleventh toolbar button)  \n\
\t Choosing this menu performs the stability analysis  \n\
\t of the loaded figure (computation of setup times  \n\
\t and hold times). The stability analyzer can also \n\
\t be launched with the crosstalk analysis. ", /*\n\
\n\t4. Timing Analysis (twelfth toolbar button) \n\
\t This menu launches the static timing analyzer Hitas. \n\
\n\t5. Timing Model Abstraction (thirteenth toolbar button) \n\
\t This menu launches the timing model abstractor TMA.\n\
\n\t6. XYagle (fourteenth toolbar button) \n\
\t This menu launches the graphical circuit viewer.",*/
		/* View    */ 
"\tHelp on View Menu of Xtas Main Desk  \n\
\n\t1. Connectors (third toolbar button) \n\
\tThis menu displays the external connectors of  \n\
\tthe loaded figure.   \n\
\n\t2. Registers (fourth toolbar button)  \n\
\tThis menu displays the flip-flops and latches  \n\
\tof the loaded figure.  \n\
\n\t3. Commands (fifth toolbar button)  \n\
\tThis menu displays the signals commanding flips-flops,  \n\
\tlatches and precharged signals, of the loaded figure.  \n\
\n\t4. Precharged points (sixth toolbar button)  \n\
\tThis menu displays the precharged signals of the loaded  \n\
\tfigure.  \n\
\n\t5. Break points (seventh toolbar button)  \n\
\tBreak points can be user-defined signals in the figure, or  \n\
\tsignals on the interface of black boxes in the hierachy  \n\
\tbelow the loaded figure. This menu displays those signals.   \n\
\n\t6. Internal signals (eighth toolbar button)  \n\
\tThis menu displays all signals which don't belong to  \n\
\tthe preceding categories.  ",
		/* Options */ 
"\tHelp on Configuration Menu of Xtas Main Desk  \n\
\n\t1. Memory size  \n\
\tThis menu performs the tuning of cache parameters.   \n\
\n\t2. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  \n\
\n\t3. Display Type \n\
\tThis menu allows customization of display for path \n\
\tand delay windows. The graphical display uses static \n\
\ttext boxes and icons whereas the textual one displays \n\
\tinformations in plain text."
	},

	/* 2. Signals window */
	{
		/* Main    */ 
"\tHelp on Main Window \n\
\n\t1. Signal Mask Text Box \n\
\tThis text box describes the pattern (including *) to \n\
\tbe matched. Default is *. \n\
\n\t2. Instance Name Text Box \n\
\tThis text box permits the choice of the instance in \n\
\twhich the search will be performed. The List button \n\
\topens a window showing available instances in the \n\
\thierarchy. Default is *.  \n\
\tThe Figure Name Text Box is then automatically set  \n\
\twith the corresponding figure name. \n\
\n\t3. Search Button \n\
\tThis button displays the signals found with the \n\
\tgiven mask, in the indicated instance. \n\
\n\t4. Signals List \n\
\tThis window displays the found signals. The arrows \n\
\tpermit smooth browsing of the signals",
		/* File    */ 
"\tHelp on File Menu of Signals \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window. \n\
\n\t3. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
		/* Tools   */ "", 
		/* View    */ 
"\tHelp on View Menu of Signals  \n\
\n\t1. Signal Infos (third toolbar button or double-click \n\
\ton a signal) \n\
\tDisplays all available information on the selected signal.",
		/* Options */
"\tHelp on Options Menu of Signals \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  "
	},
	
	/* 3. Hier window */
	{
		/* Main    */ 
"\tHelp on Hierarchy Main Window \n\
\n\t1. All Level \n\
\tThis button selects recursively all the instances  \n\
\tin the hierarchy below the current level. \n\
\n\t2. List Window  \n\
\tThis window displays all available instances \n\
\tat the current level of hierarchy. Click to select.",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},
	
	/* 4. All Signals window */
	{
		/* Main    */ 
"\tHelp on Main Window \n\
\n\t1. Signal Mask Text Box \n\
\tThis text box describes the pattern (including *) to \n\
\tbe matched. Default is *. \n\
\n\t2. Instance Name Text Box \n\
\tThis text box permits the choice of the instance in \n\
\twhich the search will be performed. The List button \n\
\topens a window showing available instances in the \n\
\thierarchy. Default is *.  \n\
\tThe Figure Name Text Box is then automatically set  \n\
\twith the corresponding figure name. \n\
\n\t3. Search Button \n\
\tThis button displays the signals found with the \n\
\tgiven mask, in the indicated instance. \n\
\n\t4. Signals List \n\
\tThis window displays the found signals. The arrows \n\
\tpermit smooth browsing of the signals",
		/* File    */ 
"\tHelp on File Menu of Signals \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window. \n\
\n\t3. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
		/* Tools   */ "", 
		/* View    */ 
"\tHelp on View Menu of Signals  \n\
\n\t1. Signal Infos (third toolbar button or double-click \n\
\ton a signal) \n\
\tDisplays all available information on the selected signal.\n\
\n\t2. Crosstalk Infos (fourth toolbar button) \n\
\tDisplays crosstalk related informations on the selected signal.",
		/* Options */
"\tHelp on Options Menu of Signals \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  "
	},
	
	/* 5. Getpath window */
	{
		/* Main    */ 
"\tHelp on Path Selection Window \n\
\n\t1. Signals Bounds \n\
\n\t1.1 Start Text Box \n\
\tThis text box describes the pattern (including *) to be matched \n\
\tfor the beginning signal of the path. The Start button permits \n\
\tbrowsing of path extremity signals in the loaded figure. \n\
\n\t1.2 End Text Box \n\
\tThis text box describes the pattern (including *) to be matched \n\
\tfor the terminating signal of the path. The End button permits \n\
\tbrowsing of path extremity signals in the loaded figure. \n\
\n\t2. Slopes Mask \n\
\tSelect here the desired types of transitions between the start \n\
\tsignal and the end signal. Available types of transitions are : \n\
\t- Start Up / End Up \n\
\t- Start Up / End Down \n\
\t- Start Down / End Down \n\
\t- Start Down / End Up \n\
\n\t3. Max/Min \n\
\tThe Max button performs the search for the longest paths. \n\
\tThe Min button performs the search for the shortest paths. \n\
\n\t4. File Type \n\
\tThe paths search can be done from the .dtx file (detailed \n\
\ttiming file), or from the .ttx file (paths timing file). \n\
\tSearching from the .ttx is much more faster and equivalently \n\
\taccurate. \n\
\n\t5. Order by \n\
\tThose buttons select the source signal of the paths search.\n\
\tIn order to reduce computing time, the signal (start or end)\n\
\twith the most restrictive mask must be chosen.  \n\
\n\t6. Time Bounds \n\
\tThose text boxes indicate the time bounds between which paths \n\
\tmust be found. Only critical paths or all paths can be searched, \n\
\tby selecting or not the button Critical Paths. \n\
\n\t7. Level of search \n\
\tThose text boxes allow the search of paths in but chosen levels \n\
\tof hierarchy. The List button permits to select instances. By  \n\
\tdefault, search is performed in all levels of hierarchy.  ",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},
	
	/* 6. Paths window */
	{
		/* Main    */ 
"\tHelp on Critic Paths Window \n\
\n\tThis window displays the paths found with the bounds \n\
\tdefined in the Get Paths window. \n\
\tThe first column shows the beginning signal of the path.  \n\
\tThe second column shows the end signal of the path.  \n\
\tThe third column shows the slope at the end of the path.  \n\
\tThe fourth column shows the propagation time through the  \n\
\tpath.",
		/* File    */ 
"\tHelp on Critic Paths Window File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window.\n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file. \n\
\n\t4. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
        /* Tools   */ 
"\tHelp on Critic Paths Window Tools Menu \n\
\n\t1. Get Paths (fifth toolbar button) \n\
\tThis menu performs a new path search. \n\
\n\t2. Get Command Paths (sixth toolbar button) \n\
\tIf a register signal is selected, this menu\n\
\tperforms a new path search, looking only for those\n\
\tending on a command of this register. \n\
\n\t3. Path Detail (seventh toolbar button) \n\
\tThis menu gets all the elementary delays forming the path. \n\
\n\t5. All Paths (eigth toolbar button) \n\
\tThis menu gets the the parallel paths of a selected path  \n\
\t(paths with the same extremities, but different delays or  \n\
\tdifferent intermediary signals).",
/*\n\t6. False Path Test (ninth toolbar button) \n\
\tThis menu allows to detect if the selected path is or is not \n\
\tan impossible behavioral path. \n\
\tThis function is under development, use with caution.",*/
		/* View    */ 
"\tHelp on Critic Paths View Menu \n\
\n\t1. Signal Info (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  ",
		/* Options */ 
"\tHelp on Critic Paths Window Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons."
	},
	
	/* 7. Getdetail window */
	{
		/* Main    */ 
"\tHelp on Get Detail Paths Window \n\
\n\t1. Search Direction \n\
\tTheses buttons allows to set the direction of the search,\n\
\teither from the start or the end of the path. \n\
\n\t2. Level of search \n\
\tThose text boxes allow the search of paths in but chosen levels \n\
\tof hierarchy. The List button permits to select instances. By  \n\
\tdefault, search is performed in all levels of hierarchy.  ",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},

	/* 8. Detailed Path window simple*/
	{
		/* Main    */ 
"\tHelp on Detail Path Window \n\
\n\tThis windows displays consecutive elementary delays forming \n\
\tthe path with the simulation results. From left to right, columns\n\
\tshows the following informations:\n\
\t - The beginning signal of the delay.  \n\
\t - The input transition. \n\
\t - The type of delay (gate or RC interconnection). \n\
\t - The output transition. \n\
\t - The end signal of the delay.  \n\
\t - The output capacitance in picofarads \n\
\t - The slope on the end signal.  \n\
\t - The propagation time between the two signals. \n\
\t - The total propagation time since the beginning of the path.",
		
        /* File    */
"\tHelp on File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window.\n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file.\n\
\n\t4. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
        /* Tools   */ 
"\tHelp on Tools Menu \n\
\n\t1. Simulate a Path (seventh button) \n\
\tThis menu allows use an electrical simulator to provide\n\
\ttiming information on the selected path.",
		/* View    */ 
"\tHelp on View Menu  \n\
\n\t1. Signal Infos (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  \n\
\n\t2. Crosstalk Infos (fifth toolbar button) \n\
\tDisplays crosstalk related informations on the selected signal.\n\
\n\t3. Visualize a Path (sixth button)\n\
\tThis menu gives a graphical representation of the \n\
\tselected path.",
        /* Options */ 
"\tHelp on Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  "
	},
	
	/* 9. Detailed Path window with crosstalk results*/
	{
		/* Main    */ 
"\tHelp on Detail Path Window\n\
\n\tThis windows displays consecutive elementary delays forming \n\
\tthe path with the crosstalk ananlysis results. From left to right,\n\
\tcolumns shows the following informations:\n\
\t - The beginning signal of the delay.  \n\
\t - The input transition. \n\
\t - The type of delay (gate or RC interconnection). \n\
\t - The output transition. \n\
\t - The end signal of the delay.  \n\
\t - The output capacitance in picofarads \n\
\t - The slope on the end signal obtained with the static timing \n\
\t   analysis.\n\
\t - The slope on the end signal obtained with the crosstalk analysis\n\
\t - The propagation time between the two signals obtained with the\n\
\t   static timing analysis.\n\
\t - The propagation time between the two signals obtained with the\n\
\t   crosstalk analysis.\n\
\t - The total propagation time since the beginning of the path \n\
\t   obtained with the static timing analysis.\n\
\t - The total propagation time since the beginning of the path \n\
\t   obtained with the crosstalk analysis.",
		
        /* File    */
"\tHelp on File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window.\n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file. ",
        /* Tools   */ 
"\tHelp on Tools Menu \n\
\n\t1. Simulate a Path (seventh button) \n\
\tThis menu allows use an electrical simulator to provide\n\
\ttiming information on the selected path.",
		/* View    */ 
"\tHelp on View Menu  \n\
\n\t1. Signal Infos (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  \n\
\n\t2. Crosstalk Infos (fifth toolbar button) \n\
\tDisplays crosstalk related informations on the selected signal.\n\
\n\t3. Visualize a Path (sixth button)\n\
\tThis menu gives a graphical representation of the \n\
\tselected path.",
		/* Options */ 
"\tHelp on Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  "
    },

	/* 10. Getpara window */
	{
		/* Main    */ 
"\tHelp on Parallel Paths Selection Window \n\
\n\t1. Signals Bounds \n\
\tThe On Path text box describes the pattern (including *) to be  \n\
\tmatched for intermediary signals of the parallel path. The  \n\
\tOn Path button permits browsing of signals in the loaded  \n\
\tfigure. \n\
\n\t2. Slopes Mask \n\
\tSelect here the desired type of selection of the intermediary \n\
\tsignal. Available types of selection are : \n\
\t- And : All signals must appear on parallel paths. \n\
\t- Or : At least one signal must appear parallel paths. \n\
\t- Not : the signals must not appear on parallel paths. \n\
\n\t3. Max/Min \n\
\tThe Max button performs the search for the longest paths. \n\
\tThe Min button performs the search for the shortest paths. \n\
\n\t4. Search Direction \n\
\tThose buttons select the source signal of the paths search. \n\
\tIn order to reduce computing time, the signal (start or end) \n\
\twith the most restrictive mask must be chosen.  \n\
\n\t5. Time Bounds \n\
\tThose text boxes indicate the time bounds between which paths \n\
\tmust be found. Only critical paths or all paths can be searched,\n\
\tby selecting or not the button Critical Paths. \n\
\n\t6. Level of search \n\
\tThose text boxes allow the search of paths in but chosen levels \n\
\tof hierarchy. The List button permits to select instances. By  \n\
\tdefault, search is performed in all levels of hierarchy.  ",
		/* File    */ "",
		/* Tools   */ "",
     	/* View    */ "",
		/* Options */ ""
	},
	
	/* 11. Parallel Paths window */
	{
		/* Main    */ 
"\tHelp on Parallel Paths Window \n\
\n\tThis window displays the paths found with the bounds \n\
\tdefined in the Get Parallel Paths window. \n\
\tThe first column shows the beginning signal of the path.  \n\
\tThe second column shows the end signal of the path.  \n\
\tThe third column shows the slope at the end of the path.  \n\
\tThe fourth column shows the propagation time through the  \n\
\tpath.",
		/* File    */ 
"\tHelp on Parallel Paths Window File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window.\n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file. \n\
\n\t4. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
        /* Tools   */ 
"\tHelp on Parallel Paths Window Tools Menu \n\
\n\t1. Get Paths (fifth toolbar button) \n\
\tThis menu performs a new path search. \n\
\n\t2. Get Command Paths (sixth toolbar button) \n\
\tIf a register signal is selected, this menu\n\
\tperforms a new path search, looking only for those\n\
\tending on a command of this register. \n\
\n\t3. Path Detail (seventh toolbar button) \n\
\tThis menu gets all the elementary delays forming the path. \n\
\n\t5. All Paths (eigth toolbar button) \n\
\tThis menu gets the the parallel paths of a selected path  \n\
\t(paths with the same extremities, but different delays or  \n\
\tdifferent intermediary signals).",
/*\n\t6. False Path Test (ninth toolbar button) \n\
\tThis menu allows to detect if the selected path is or is not \n\
\tan impossible behavioral path. \n\
\tThis function is under development, use with caution.",*/
		/* View    */ 
"\tHelp on Parallel Paths View Menu \n\
\n\t1. Signal Info (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  ",
		/* Options */ 
"\tHelp on Parallel Paths Window Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons."
	},
	
	/* 12. False Paths Result window */
	{
		/* Main    */ 
"\tHelp on False Path Test window \n\
\n\t1. Path\n\
\tThis part is a reminder of the selected path.\n\
\n\t2. Result\n\
\tIf the path is a false path, the text box displays \"FALSE\".\n\
\tIf the path is not a false path, the text box displays \"TRUE\"\n\
\n\t3. Suppress False Path From\n\
\tThis section is only available if the selected path is a false\n\
\tpath. Check the box to remove this path from the database (the\n\
\tpath is added in the falsepath list of the INF file corresponding\n\
\tto the netlist).\n\
\tSuppressing the path from ttx (bound path), removes all the paths\n\
\tstarting and ending on the same connectors with the same transitions\n\
\tthan the selected path. \n\
\tSuppressing the path from dtx (detailed path), removes only the \n\
\tselected path.",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},

	/* 13. Visualize Path */
	{
		/* Main    */
"\tHelp on Visualise Path Window \n\
\n\tXtas use the Xyagle window to give a graphical representation\n\
\tof the selected path. There are two ways to visualizae this path:\n\
\n\t1. Highlight\n\
\tXyagle displays the whole circuit and highlights the selected \n\
\tpath.\n\
\n\t2. Extract\n\
\tXyagle displays only the selected path.",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},

	/* 14. Simulation param */
	{
		/* Main    */ 
"\tHelp on Simulation Parameterization Window \n\
\n\t1. Simulated Path\n\
\t It is a reminder of the selected path.\n\
\n\t2. Technology File\n\
\tClick on the Open button to choose the directory of the \n\
\ttechnology fileand select it in the list.\n\
\n\t3. Simulation Tool\n\
\tSelect the electrical simulator to use with the appropriate\n\
\tbutton.\n\
\n\t3.1 Command line\n\
\tEnter the command line corresponding to the simulator in \n\
\tthis text box.\n\
\n\t3.2 Output File Format\n\
\tThis text box allows to specify the format of the file generated\n\
\tby the simulator. The syntax is \"$.out\", where out is the \n\
\toutput format.\n\
\n\t3.3 Stdout Redirection File Format\n\
\tThis text box allows to specify the format of the file for \n\
\tstandard output redirection. The syntax is \"$.out\", where\n\
\tout is the format.\n\
\n\t3.4 Use Print\n\
\tCheck this box to get an array of value for a node.\n\
\n\t3.5 Use Measure\n\
\tCheck this box to extract value from waveform using combination\n\
\tof arithmetic expressions.\n\
\n\t4. Input/Output Constraints\n\
\n\t4.1 Input Start Time\n\
\tThis text box allows to set the starting time of the input \n\
\tslope (in picoseconds). \n\
\n\t4.2 Input Slope Time\n\
\tThis text box allows to set the transient time of the slope (in\n\
\tpicoseconds).\n\
\n\t4.3 Output Capacitance Value\n\
\tThis text box allows to set the output capacitance value (in\n\
\tfemtofarads). \n\
\n\t5. Conditions\n\
\n\t5.1 Time\n\
\tThis text box allows to set the duration of the simulation (in \n\
\tnanoseconds). \n\
\n\t5.2 VDD\n\
\tThis text box allows to set the value of the simulation voltage\n\
\t(in Volts).\n\
\n\t5.3 Temperature\n\
\tThis text box allows to set the value of the simulation \n\
\ttemperature (in degrees Celsius). \n\
\n\t5.4 Spice's Options\n\
\tSpecify the spice options to be driven into the spice file for \n\
\tsimulation.\n\
\n\t6. Thresholds\n\
\n\t6.1 VTH\n\
\tThis text box allows to set the value of the threshold as a\n\
\tpercentage of the VDD value.\n\
\n\t6.2 VTH high\n\
\tThis text box allows to set the high level of the slope as a \n\
\tpercentage of the VDD value. \n\
\n\t6.3 VTH low\n\
\tThis text box allows to set the low level of the slope as a \n\
\tpercentage of the VDD value.\n\
\n\t7. Step\n\
\tThis text box allows to set the calculation step for the simulation\n\
\t(in nanoseconds).",
        /* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},
    
	/* 15. Simulation Result window == Detail with simulation results*/
	{
		/* Main    */ 
"\tHelp on Simulated Path Detail Window \n\
\n\tThis windows displays consecutive elementary delays forming \n\
\tthe path with the simulation results. From left to right, columns\n\
\tshows the following informations:\n\
\t - The beginning signal of the delay.  \n\
\t - The input transition. \n\
\t - The type of delay (gate or RC interconnection). \n\
\t - The output transition. \n\
\t - The end signal of the delay.  \n\
\t - The output capacitance in picofarads \n\
\t - The slope on the end signal obtained with the static timing \n\
\t   analysis.\n\
\t - The slope on the end signal obtained with the simulation\n\
\t - The propagation time between the two signals obtained with the\n\
\t   static timing analysis.\n\
\t - The propagation time between the two signals obtained with the\n\
\t   simulation.\n\
\t - The total propagation time since the beginning of the path \n\
\t   obtained with the static timing analysis.\n\
\t - The total propagation time since the beginning of the path \n\
\t   obtained with the simulation.",
		
    /* File    */
"\tHelp on File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window.\n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file. ",
        /* Tools   */ 
"\tHelp on Tools Menu  \n\
\n\t1. Simulate a Path\n\
\tThis menu allows use an electrical simulator to provide\n\
\ttiming information on the selected path.",
		/* View    */ 
"\tHelp on View Menu  \n\
\n\t1. Signal Infos (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  \n\
\n\t2. Visualize a Path (fifth toolbar button)\n\
\tThis menu gives a graphical representation of the \n\
\tselected path.",
		/* Options */ 
"\tHelp on Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  "
	},
    
	/* 16. Getdelay window */
	{
		/* Main    */
"\tHelp on Delay Selection Window \n\
\n\t1. Signals Bounds \n\
\n\t1.1 Start Text Box \n\
\tThis text box describes the pattern (including *) to be matched \n\
\tfor the beginning signal of the delay. The Start button permits \n\
\tbrowsing of signals in the loaded figure. \n\
\n\t1.2 End Text Box \n\
\tThis text box describes the pattern (including *) to be matched \n\
\tfor the terminating signal of the delay. The End button permits \n\
\tbrowsing of signals in the loaded figure. \n\
\n\t2. Slopes Mask \n\
\tSelect here the desired types of transitions between the start \n\
\tsignal and the end signal. Available types of transitions are : \n\
\t- Start Up / End Up \n\
\t- Start Up / End Down \n\
\t- Start Down / End Down \n\
\t- Start Down / End Up \n\
\n\t3. Max/Min \n\
\tThe Max button performs the search of the maximum delays. \n\
\tThe Min button performs the search of the minimum delays. \n\
\n\t4. Order by \n\
\tThose buttons select the type of delay to be searched. RC \n\
\tmeans interconnect delays, Gate means Gate delays. \n\
\n\t5. Time Bounds \n\
\tThose text boxes indicate the time bounds between which paths\n\
\tmust be found. Only critical paths or all paths can be searched,\n\
\tby selecting or not the button Critical Paths. \n\
\n\t6. Level of search \n\
\tThose text boxes allow the search of paths in but chosen levels \n\
\tof hierarchy. The List button permits to select instances. By  \n\
\tdefault, search is performed in all levels of hierarchy.",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},
	
	/* 17. Delay window*/
	{
		/* Main    */ 
"\tHelp on Delay Window \n\
\n\tThis windows displays elementary delays or timing arcs.\n\
\tFrom left to right, columns shows the following informations:\n\
\t - The beginning signal of the delay.  \n\
\t - The input transition. \n\
\t - The type of delay (gate). \n\
\t - The output transition. \n\
\t - The end signal of the delay. \n\
\t - The output capacitance in picofarads \n\
\t - The slope on the end signal.  \n\
\t - The propagation time between the two signals.",
        /* File    */
"\tHelp on File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window.\n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file.\n\
\n\t4. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
        /* Tools   */ "",
		/* View    */ 
"\tHelp on View Menu  \n\
\n\t1. Signal Infos (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal. ",
        /* Options */ 
"\tHelp on Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons. "
	},

	/* 18. Stability Parameterization window */
	{
		/* Main    */ 
"\tHelp on Stability Parameterization Window \n\
\n\t1. Crosstalk Analysis \n\
\tCheck this box to launch the stability analyzer with the \n\
\tcrosstalk analysis. This makes the crosstalk parameterization\n\
\tarea accessible.\n\
\tSee the STB-CTK User Guide for more detailed explainations of\n\
\tthe configuration options. \n\
\n\t1.1. Crosstalk Analysis Type \n\
\t In the \"remove non-aggression\" mode, all aggression is assumed \n\
\tinitially.\n\
\tIn the \"detect aggression\" mode, no aggression is assumed\n\
\tinitially. In addition to this mode, check the box \n\
\t\"observable only\" in order to have less pessimistic results.\n\
\n\t1.2. No More Aggression / Stop Conditions \n\
\tThese are conditions for stopping slope recalculation when no \n\
\tfurther aggression is detected or removed. \"Min slope change\" \n\
\trepresents the minimum significant slope variation in \n\
\tpicoseconds.\n\
\t\"Max iteration number\" represents the maximum number of \n\
\trecalculation iterations. \n\
\n\t1.3. Crosstalk Model \n\
\n\t1.3.1. Capacitance For Delays \n\
\tModel used to compute elementary delay will take into account\n\
\tcrosstalk effect if an aggression is detected. \n\
\tWith the \"0C 1C 2C\" model, crosstalk capacitance is removed \n\
\tor replaced to a ground capacitance with a doubled value.\n\
\tWith the \"0C to 2C\" model, crosstalk capacitance is replaced \n\
\tto a ground capacitance with a value multiplied by 0.0 to 2.0 \n\
\taccording to relative slope computed without coupling effect \n\
\tbetween victim and its aggressor.\n\
\tThe \"-1c to 3C\" model is equivalent to the previous model,\n\
\texcept capacitance is multiplied by a value between -1.0 and \n\
\t3.0.\n\
\n\t1.3.2. Noise For Delays \n\
\tChoose Fine allows model to take into account the change\n\
\tof amplitude (noise effect) on a signal due to coupling effect.\n\
\tChoose Never makes model to not take into account that \n\
\tchange of amplitude .\n\
\n\t1.3.3. Slope For Noise \n\
\tThis section allows to specify the slew used for noise peak \n\
\tcalculation.\n\
\tChoose Nominal to use slopes computed without aggression.\n\
\tChoose CTK to use worst slopes with crosstalk. \n\
\tChoose Real to use the real slope on aggressor.\n\
\n\t1.3.4. Slope For Delays \n\
\tThis section allows to specify the slew used during delay\n\
\tcalculation.\n\
\tCTK mode is faster but with pessimistic results .\n\
\tEnhanced mode is more precise but require more computation\n\
\ttime.\n\
\n\t1.3.5. Aggression Margin \n\
\tThis text box allows to specify the minimum delay separating\n\
\ttwo instability interval to consider that there is no \n\
\tpossibility of aggression. \n\
\n\t1.4. Options\n\
\n\t1.4.1. Generate a report \n\
\tthe crosstalk analysis can generate a report file (.ctk), which\n\
\tcontains delay changes, detailed aggression reports and noise\n\
\testimation. Lower limits can be set to avoid reporting excessive\n\
\tinformation :\n\
\tThe \"Minimum delta delay\" text box allows to set the minimum \n\
\tvalue, in picoseconds, of the variation of the delay between the\n\
\tvalue of the delay with and without crosstalk analysis.\n\
\tThe \"Minimum delta slope\" text box allows to set the minimum \n\
\tvalue, in picoseconds, of the variation of the slope between the \n\
\tvalue of the slope with and without crosstalk analysis. \n\
\tThe \"Minimum noise\" text box allows to set the minimum value, \n\
\tin millivolts, of the electrical noise to be reported. \n\
\tThe \"Minimum crosstalk\" text box allows to set the minimum \n\
\tpercentage of coupling capacitance.\n\
\n\t1.4.2. Use Cache \n\
\tIn order to manage memory more efficiently, check this box.\n\
\tThe text box allows to set the size of the cache in Megabytes.\n\
\n\n\t2. Stability Analysis \n\
\n\t2.1. Analysis type \n\
\tThe best case analysis is performed by assuming that in the  \n\
\tinitial conditions, latch transparancy is maximum.   \n\
\tThe worst case analysis is performed by assuming that in the  \n\
\tinitial conditions, there is no latch transparancy. \n\
\tIn multi-interval mode, all stability intervals are maintained \n\
\twhereas in mono-interval mode, they are merged into a single \n\
\tinterval for setup/hold verifcation. To perform the crosstalk \n\
\tanalysis, the mono-interval mode is required.\n\
\tSee the STB User Guide for further information. \n\
\n\t2.2. Monophase Latch \n\
\tIn the Flip-Flop mode, a latch clocked on the same phase than  \n\
\tthe latch generating its input data is assumed to be a flip-flop. \n\
\tIn the Transparent mode, a latch clocked on the same phase \n\
\tthan the latch generating its input data is always transparent. \n\
\tIn the Error mode, a latch clocked on the same phase than the  \n\
\tlatch generating is input data is not allowed, and an error is \n\
\treported. \n\
\tSee the STB User Guide for further information. \n\
\n\t2.3. Level \n\
\tIn the All Levels mode, constraints are computed with the paths \n\
\tthrough all the hierarchy. \n\
\tIn the Top Level mode, constraints are computed, by taking only  \n\
\tinto account the paths at the top level (it is to say, the inter \n\
\tconnections at the top level). \n\
\n\t2.4. Error Type \n\
\tIn the Setup mode, only errors due to setup time violations are  \n\
\treported \n\
\tIn the Hold mode, only errors due to hold time violations are  \n\
\treported \n\
\n\t2.5. Error Reports \n\
\tIn sto mode, a .sto file is drived. This file contains the  \n\
\tstability state of all the signals in the figure. \n\
\tIn str mode, a .str file is drived. This contains the signals on  \n\
\twhich a setup or hold violation occurs. for each error signal, the  \n\
\torigin signals are also reported. \n\
\n\t2.6. File Type \n\
\tAnalysis can be performed either from the detailed timing file  \n\
\t(.dtx file), or from the path timing file (.ttx file). Performing  \n\
\tanalysis from the path timing file is much mode faster. \n\
\n\t2.7.Error Report \n\
\tThe Error Margin is added to the hold and setup constraints of  \n\
\tthe figure. For example, if there is an error margin of 100ps,  \n\
\ta signal with a setup or a hold time below 100ps will be reported  \n\
\tas an error.  ",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},
	
    /* 19. Stability Result window */
	{
		/* Main    */ 
"\tHelp on Stability Analysis Results Main Window \n\
\n\tThis window displays the stability analysis results. \n\
\tThe first column displays the error signals. \n\
\tThe second column displays the error signals types. \n\
\tThe third column displays the setup times. \n\
\tThe fourth column displays the hold times.",
		/* File    */
"\tHelp on Stability Analysis Results File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window. \n\
\n\t3. Save Screen (third toolbar button) \n\
\tSave the current screen in a data file.\n\
\n\t4. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
		/* Tools   */
"\tHelp on Stability Analysis Results Tools Menu \n\
\n\t1. Get Paths (fifth toolbar button) \n\
\tThis menu performs a new path search. \n\
\n\t2. Get Command Paths (sixth toolbar button) \n\
\tIf a register signal is selected, this menu\n\
\tperforms a new path search, looking only for those\n\
\tending on a command of this register. \n\
\n\t3. Debug Error (seventh toolbar button) \n\
\tThis menu opens a sub-window allowing debugging \n\
\tof the error. In this sub-window, timing diagrams and \n\
\torigin signals of the selected error signal are  \n\
\treported. \n\
\n\t4. Noise Analysis (eighth toolbar button) \n\
\tThis menu allows to calculate the peak voltages for all\n\
\tsignals in the circuit.",
		/* View    */ 
"\tHelp on Stability Analysis Results View Menu \n\
\n\t1. Signal Info (fourth toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  ",
		/* Options */
"\tHelp on Stability Analysis Results Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons."
	},
	
	
	/* 20. Debug window */
	{
		/* Main    */ 
"\tHelp on Debug Main Window \n\
\n\t1. Input Signals List \n\
\tThis window displays all the signals that are inputs of the error \n\
\tsignal. Selecting one of those signals leads to the display of the \n\
\ttiming diagram associated with this input \n\
\tThis window displays the timing diagrams of an error signal,  \n\
\n\t2. Timing Diagram \n\
\tThe timing diagram shows : \n\
\t- first grey waveform : the reference clock \n\
\t- second grey waveform (non present if the input signal is a  \n\
\t  connector) : the local clock commanding the input signal.  \n\
\t- orange stability chronogram : the input signal \n\
\t- third grey waveform (or second if the input signal is a  \n\
\t  connector) : the local clock commanding the input signal.  \n\
\t- red stability chronogram : the stability chronogram generated\n\
\t  by the input signal (and only by him). \n\
\n\t3. Zoom \n\
\t- Click on the middle button of the mouse to select the first \n\
\t  bound of the area to zoom \n\
\t- Hold this button until you reach the second bound \n\
\t- Click on the right button of the mouse to view the previous\n\
\t  zoom.",
		/* File    */ 
"\tHelp on Debug File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window. \n\
\n\t3. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",

		/* Tools   */ 
"\tHelp on Debug Tools Menu \n\
\n\t1. Get Paths (fourth toolbar button)\n\
\tWhen a signal is selected in the Input Signals List, this \n\
\tmenu automatically opens a Get Paths window, with Start and  \n\
\tEnd fields properly filled with the selected input signal  \n\
\tand the error signal \n\
\n\t2. Get Command Paths (fifth toolbar button) \n\
\tIf a register signal is selected, this menu\n\
\tperforms a new path search, looking only for those\n\
\tending on a command of this register.",
		/* View    */ 
"\tHelp on Debug View Menu \n\
\n\t1. Signal Infos (third toolbar button) \n\
\tDisplays all available information on the selected signal.",
		/* Options */ 
"\tHelp on Debug Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.  "
	},
	
	/* 21. Noise Result window */
	{
		/* Main    */ 
"\tHelp on Noise Analysis Results Main Window \n\
\n\tThis window displays the results of the noise analysis.\n\
\tFrom left to right, columns give the following informations \n\
\ton a signal:\n\
\t - Rank in descending order of rise peak noise value.\n\
\t - Static level of the signal (Low or High).\n\
\t - Signal name.\n\
\t - Net name of the signal.\n\
\t - Electrical model used to evaluate rise peak noise.\n\
\t - Maximum rise peak noise calculated with all aggressors\n\
\t    considerd active.\n\
\t - \"Real\" rise peak noise calculated considering possible\n\
\t    switching configurations of aggressors.\n\
\t - Electrical model used to evaluate fall peak noise.\n\
\t - Maximum fall peak noise calculated with all aggressors\n\
\t    considerd active.\n\
\t - \"Real\" fall peak noise calculated considering possible\n\
\t    switching configurations of aggressors.\n\
\t - Global score (out of 100) computed with weighting provided\n\
\t    by user.\n\
\t - Noise score (out of 10): the impact of the noise peak. A\n\
\t    mark of 10 means the peak noise reach or exceed the static\n\
\t    threshold of the following gate.\n\
\t - Interval score (out of 10): the part of aggressors which\n\
\t    crosstalk can be simultaneously active at the same time.\n\
\t - Crosstalk score (out of 10): the number of significant\n\
\t    aggressor. The more the mark is about 10, the more the most\n\
\t    significative part of crosstalk is due to a few number of\n\
\t    aggressor.\n\
\t - Activity score (out of 10): the activity of the aggressor.\n\
\t    for now, the only aggressor  located on a path clock are \n\
\t    supposed to be always active, weighted by the part of crosstalk\n\
\t    capacitance to this aggressor.",
		/* File    */ 
"\tHelp on Noise Analysis Results File Menu \n\
\n\t1. To Main (first toolbar button) \n\
\tReturn to the Main Desk Window. \n\
\n\t2. Close (second toolbar button) \n\
\tClose the current window. \n\
\n\t3. Load an INF File \n\
\tSelect the Information file (.inf) to load.\n\
\tChoose 'Complete' to add information to the current database.\n\
\tChoose 'Replace' to replace the current database by the new\n\
\tfile.",
        /* Tools   */ "",
		/* View    */ 
"\tHelp on Noise Analysis Results View Menu  \n\
\n\t1. Signal Infos (third toolbar button) \n\
\tDisplays all available information on the selected \n\
\tsignal.  \n\
\n\t2. Crosstalk Infos (fourth toolbar button) \n\
\tDisplays crosstalk related informations on the selected signal.",	
    /* Options */ 
"\tHelp on Noise Analysis Results Options Menu \n\
\n\t1. Buttons Menu  \n\
\tThis menu allows the customization of the buttons.\n\
\n\t2. Scores Configuration \n\
\tThis menu allows to configure two parameters for the following\n\
\tscores : Noise, Interval, Crosstalk and Activity. The first\n\
\tone is the weigthing (Coef) to compute the global score \n\
\tobtained by each signal. The second one is the minimum value \n\
\t(Min)required to be reported in the results list."
},
	
	/* 22. Crosstalk information window */
	{
		/* Main    */ 
"\tHelp on Crosstalk Infos Window \n\
\n\tThis window displays crosstalk informations from the \n\
\tcrosstalk analysis run with Xtas.\n\
\n\t1. General\n\
\tThis part gives the signal state and its name. It gives\n\
\talso the ground capacitance and the total crosstalk \n\
\tcapacitance on this signal.\n\
\n\t2. Noise\n\
\tFor both rise and fall peak noise, this part displays \n\
\tthe following informations:\n\
\t - Electrical model use to evaluate peak noise.\n\
\t - Maximum noise calculated with all aggressors considered\n\
\t   active.\n\
\t - \"Real\" noise calculated considering possible switching\n\
\t   configurations of aggressors.\n\
\n\t3. Aggressors List\n\
\tThis part gives the list of the signal aggressors. From left\n\
\tto right, each line gives the following informations about \n\
\tthe aggressor signal:\n\
\t - Signal name. If there is no corresponding timing node, \n\
\t   that information is missing. As no stability information\n\
\t   is provided for this signal, this signal is considered to\n\
\t   be always an active aggressor.\n\
\t - Net name of the signal.\n\
\t - The kind of influence the signal has on the victim. \n\
\t   If the character 'B' (Best case) appears on the line, \n\
\t   that means the aggressor can modify minimum propagation \n\
\t   delays.\n\
\t   If the character 'W' (Worst case) appears on the line, \n\
\t   that means the aggressor can modify maximum propagation \n\
\t   delays.\n\
\t   If the character 'R' (Rise) appears on the line, that\n\
\t   that means the aggressor has made a contribution to \n\
\t   calculate the real rise peak noise value.\n\
\t   If the character 'F' (Fall) appears on the line, that\n\
\t   that means the aggressor has made a contribution to \n\
\t   calculate the real fall peak noise value.\n\
\t   These four characters can appear in lower case \n\
\t   ('b','w','r','f') when crosstalk mutex are used. This\n\
\t   means the influence of the signal is ignored because of\n\
\t   the crosstalk mutex.\n\
\t - Coupling capacitance on the victim due to this aggressor.\n\
\tAgressors can be sorted by the signal name or by the \n\
\tcapacitance value using the button at the top of the \n\
\tcorresponding column. Successive pressions on the button \n\
\tsort alternatively in ascending or descending order.\n\
\n\tSee the STB-CTK User Guide for further information. ",
        /* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},

	/* 23. Tas Parameterization */
	{
		/* Main    */ 
"\tHelp on Timing Analysis Parameterization window \n\
\n\t1. Input Netlist Format\n\
\tThree input formats are available for Static Timing Analysis:\n\
\t     - SPICE.\n\
\t     - structural VHDL.\n\
\t     - structural Verilog.\n\
\n\t2. Input Files\n\
\n\t2.1 Netlist File\n\
\tClick on the Open button to select the directory of the \n\
\tnetlist file and select it in the list. the filename and \n\
\tits suffix will appear in the text box.\n\
\n\t2.2 Techno File\n\
\tClick on the Open button to select the directory of the \n\
\ttechnology file and select it in the list. the filename \n\
\tand its suffix will appear in the text box.\n\
\n\t2.3 Read Info File\n\
\tCheck the box to use informations contained in the \n\
\tinformation file (.inf).\n\
\n\t3. TAS Output Files\n\
\tThis part allows to select which files have to be generated \n\
\tby the analyser:\n\
\n\t3.1 General Perfmodule (TTX file)\n\
\tIt is the default output format of the HiTas tool. TTX or\n\
\tDTX file must be generated to browse timing analysis results\n\
\tin Xtas.\n\
\n\t3.2 Detailed Perfmodule (DTX file)\n\
\tThis file contains the gate delays. TTX or DTX file must be\n\
\tgenerated to browse timing analysis results in Xtas.\n\
\n\t3.3 Disassembled File (CNS file)\n\
\tThis file contains the cone view. It is generated only in\n\
\tflat analysis mode.\n\
\n\t3.4 Slope File (SLO file)\n\
\tThis file contains the signal slope for each input of a cone.\n\
\n\t3.5 Silent Mode\n\
\tCheck this box to not display the trace of the analysis. In \n\
\tthis case, this trace is written into two files: .tou for \n\
\tstandard output report and .ter for standard error report.\n\
\n\t4. TAS General Options\n\
\n\t4.1 Analysis Mode\n\
\tThis part allows to indicates if the input netlist is at \n\
\ttransistor level or hierarchical.\n\
\n\t4.2 Critical Paths\n\
\tChoose \"Long Paths Only\" to generate only maximum delay\n\
\tpaths.\n\
\tChoose \"Long Paths and Short Paths to generate both maximum\n\
\tand minimum delay paths.",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},

	/* 24. Tas Advanced Parameterization */
	{
		/* Main    */ 
"\tHelp on Timing Analysis Advanced Options window\n\
\n\t1. Calculation\n\
\n\t1.1 Precharged signals\n\
\tCheck this box to consider as precharged signals those\n\
\twhich name are suffixed by _p or declared in the INF file.\n\
\tThis option is valid only in flat analysis mode.\n\
\n\t1.2 Path factorization\n\
\tIf this box is checked, the analyzer keeps paths starting \n\
\tand stopping at factorization points that are not reference\n\
\tpoints. This is in order to decrease the number of path of\n\
\tthe general perfmodule.\n\
\n\t1.3 Output terminal charge\n\
\tThis text box allows to set the value of the capacitance\n\
\tthat will be added to all output terminals. Unchecking the\n\
\tbox is equivalent to setting a zero value.\n\
\n\t1.4 Out of path capacitance factor\n\
\tThis text box allows to set the percentage of the \'out-of-path\'\n\
\tcapacitances to take into account during a flatten analysis.\n\
\tUnchecking the box is equivalent to setting a zero value.\n\
\n\t1.5 Use stability analysis\n\
\tCheck this box to make the analyzer to run the stability \n\
\tanalysis.\n\
\n\t1.6 Connectors input slope\n\
\tThis text box allows to specify the input signal slew rate for\n\
\texternal input terminals. Unchecking the box is equivalent to \n\
\tsetting a zero value.\n\
\n\t1.7 Input pass-transistor capacitance factor\n\
\tThis text box allows to set the percentage of the \'out-of-path\'\n\
\tcapacitances associated with an input terminal to take into\n\
\taccount during a flatten analysis. It affects only TTx and DTX\n\
\tfile generation and does not change the delay computation.\n\
\tUnchecking the box is equivalent to setting a zero value.\n\
\n\t1.8 Ignore black boxes\n\
\tIf the box is checked, then the analyzer reads a hierarchical\n\
\tnetlist in which some of the instances are considered to be \n\
\tblack boxes. The name of these instances is given in a file \n\
\twhose name is given by the avtBlackBoxFile configuration \n\
\tvariable. The hierarchical netlist is then flattened to the \n\
\ttransistor level apart from the black box instances to generate\n\
\ta hybrid transistor and instance netlist. The timing analysis \n\
\tis performed on this hybrid netlist.\n\
\n\t1.9 Flat cells\n\
\tIf the box is checked, then the analyzer flattens the timing \n\
\tviews of all the models given in the catalogue by the \n\
\tavtCatalogueName configuration variable.\n\
\n\t2. Interconnect Options\n\
\n\t2.1 Calculate RC delays\n\
\tIf the box is checked, then only the capacitances and not the \n\
\tresistances of the interconnecting wires are taken into account\n\
\tto compute the propagation times.\n\
\n\t2.2 Merge RC and gate delays\n\
\tCheck this box to merge the gate delays and the interconnection\n\
\tRC delays in the flatten analysis mode.\n\
\n\t2.3 Break loops in RC networks\n\
\tCheck this box to allow the analyzer to cut loops in the RC \n\
\tinterconnection networks during the flat or hierarchical analysis.\n\
\n\t3. Characterization\n\
\tSpecify the model the analyzer will use for delay calculation.\n\
\tSee HiTas User Guide for further information.\n\
\tThe model \"scm models of all delays\" must be enable for \n\
\tcrosstalk analysis.\n\
\n\t4. Abstraction\n\
\n\t4.1 Latch / Flipflop\n\
\n\t4.1.1 Latch share command\n\
\tCheck this box to enable to share the same control signal \n\
\tbetween several master-slave flip-flops. To be used with \n\
\t\"Flipflop detection\".\n\
\n\t4.1.2 Flipflop detection\n\
\tCheck this box to enable the automatic detection of \n\
\tmaster-slave flip-flops based on a special latch \"with conflict\"\n\
\twhen possible. By default, the master latch is chosen to be the \n\
\tmemory point, this can be change checking the \"Slave as \n\
\tflipflop\" box.\n\
\n\t4.1.3 Slave as flipflop\n\
\tCheck this box to choose the slave latch as the memory point of \n\
\tthe flip-flop. To be used with \"Flipflop detection\".\n\
\n\t4.1.4 Latch detection\n\
\n\t4.1.4.a Use built-in library\n\
\tCheck this box to enable the detection of latches and memory\n\
\tpoints using the built-in latch library. Uncheck this box is\n\
\tuseful if all memory points are to be recognized by the use of\n\
\ta user-defined library with \"Transistor netlist recognition\" option. \n\
\n\t4.1.5.b Use automatic detection\n\
\tCheck this box to activate an advanced latch detection \n\
\talgorithm based on boolean loop analysis.\n\
\n\t4.2 Transistors\n\
\n\t4.2.1 Removes parallel transistors\n\
\tCheck this box to replace transistors connected in parallel with \n\
\tan equivalent transistor.\n\
\n\t4.2.2 Orientation\n\
\n\t4.2.2.a Take the \"_s\" convention into account\n\
\tCheck this box to activate transistor orientation taking into\n\
\taccount the _s convention on signal's names. This method is \n\
\tsometimes used instead of external constraints (INF file) if \n\
\tthere exists correlation between external connectors.\n\
\n\t4.2.3.b Use simple transistor orientation heuristic\n\
\tCheck this box to activate a simple transistor orientation \n\
\theuristic. Can sometimes accelerate the disassembly, however,\n\
\tit is more robust to rely exclusively upon the functional analysis.\n\
\n\t4.3 Pattern Recognition\n\
\n\t4.3.1 Transistor netlist recognition\n\
\tCheck this box to make the disassembler tool use the \n\
\tlibrary-based transistor netlist recognition. This allows to\n\
\tspecify a number of netlists to be identified within the circuit\n\
\tto be disassembled. These netlists are specified in the Spice\n\
\tformat and can contain a number of the special directives for \n\
\tthe marking of the identified signals and transistors in the \n\
\tcircuit. This option can be used also to detect certain types of\n\
\tlatches or flip-flops. \n\
\tSee FCL pattern matching chapter of the HiTas user Guide for \n\
\tdetails.\n\
\n\t4.3.2 Stops after transistor netlist recognition\n\
\tSame as \"Transistor netlist recognition\". In addition, it makes\n\
\tthe disassembler tool to stop after the recognition process.\n\
\n\t4.3.3 Hierarchical recognition\n\
\tCheck this box to extend the simple pattern recognition of FCL\n\
\tto allow the recognition of hierarchically defined structures of \n\
\n\t4.3.4 Stops after hierarchical recognition\n\
\tCheck this box to stop the execution after the hierarchical \n\
\tpattern recognition phase and drive a transistor netlist \n\
\tcontaining the remaining transistors and a structural hierarchy\n\
\tinstantiating this and the recognised instances.\n\
\n\t4.4 Functional Analysis\n\
\n\t4.4.1 Use functional analysis\n\
\tCheck this box to enable the functional analysis process. The \n\
\ttext box allows to specify the depth for the functional analysis.\n\
\tThis is the number of gates that will be taken into account for\n\
\tthe functional analysis, so that HiTas can detect re-convergence\n\
\tin the circuit.\n\
\n\t4.4.2 Exploits high impedance nodes\n\
\tCheck this box to allow, for instance, the resolution of false\n\
\tconflicts in circuits which use precharged logic. ",
		/* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	},

	/* 25. TMA Parameterization */
	{
		/* Main    */ 
"\tHelp on Timing Model Abstraction Parameterization window \n\
\n\t1. Input Netlist Format\n\
\tFive input formats are available for Timing Model Abstraction:\n\
\t     - SPICE.\n\
\t     - structural VHDL.\n\
\t     - structural Verilog.\n\
\t     - general perfmodule (.ttx)\n\
\t     - detailed perfmodule (.dtx)\n\
\tIf SPICE, VHDL or Verilog format is chosen, the timing model \n\
\tabstractor will perform the static timing analysis. So, the \n\
\ttiming analysis parameterization areas (\"TAS  Output Files\",\n\
\t\"TAS General Options\") and the timing analysis advanced options\n\
\twindow are accessible.\n\
\n\t2. Input Files\n\
\n\t2.1 Netlist File\n\
\tClick on the Open button to select the directory of the \n\
\tnetlist file and select it in the list. the filename and \n\
\tits suffix will appear in the text box.\n\
\n\t2.2 Techno File\n\
\tClick on the Open button to select the directory of the \n\
\ttechnology file and select it in the list. the filename \n\
\tand its suffix will appear in the text box.\n\
\n\t2.3 Read Info File\n\
\tCheck the box to use informations contained in the \n\
\tinformation file (.inf).\n\
\n\t3. TAS Output Files\n\
\tThis part allows to select which files have to be generated \n\
\tby the analyser:\n\
\n\t3.1 General Perfmodule (TTX file)\n\
\tIt is the default output format of the HiTas tool. TTX or\n\
\tDTX file must be generated to browse timing analysis results\n\
\tin Xtas.\n\
\n\t3.2 Detailed Perfmodule (DTX file)\n\
\tThis file contains the gate delays. TTX or DTX file must be\n\
\tgenerated to browse timing analysis results in Xtas.\n\
\n\t3.3 Disassembled File (CNS file)\n\
\tThis file contains the cone view. It is generated only in\n\
\tflat analysis mode.\n\
\n\t3.4 Slope File (SLO file)\n\
\tThis file contains the signal slope for each input of a cone.\n\
\n\t3.5 Silent Mode\n\
\tCheck this box to not display the trace of the analysis. In \n\
\tthis case, this trace is written into two files: .tou for \n\
\tstandard output report and .ter for standard error report.\n\
\n\t4. TAS General Options\n\
\n\t4.1 Analysis Mode\n\
\tThis part allows to indicates if the input netlist is at \n\
\ttransistor level or hierarchical.\n\
\n\t4.2 Critical Paths\n\
\tChoose \"Long Paths Only\" to generate only maximum delay\n\
\tpaths.\n\
\tChoose \"Long Paths and Short Paths to generate both maximum\n\
\tand minimum delay paths.",
        /* File    */ "",
		/* Tools   */ "",
		/* View    */ "",
		/* Options */ ""
	}


    

} ;
char *XtasOnVersion;
Widget HelpFather = NULL ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasHelpCallback                                               */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .warning : The warning widget to display for the moment.       */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is The futur Help function.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasLeaveHelpCallback (widget, client_data, call_data)
Widget    widget;
XtPointer client_data;
XtPointer call_data;
{
	//HelpFather = NULL ;
}

/*---------------------------------------------------------------------------*/

void XtasHelpCallback (widget, client_data, call_data)
Widget    widget;
XtPointer client_data;
XtPointer call_data;
{
	Widget helpwidget ;
	Widget textwidget ;
 	Arg    args[10] ;
	int    n ;
	char  *text ;


	n = 0 ;
	XtSetArg (args[n], XmNtitle,          "HELP") ;       n++ ;
	XtSetArg (args[n], XmNwidth,           500) ;         n++ ;
	XtSetArg (args[n], XmNheight,          300) ;         n++ ;
	XtSetArg (args[n], XmNdialogStyle,  XmDIALOG_APPLICATION_MODAL) ;         n++ ;
	XtSetArg (args[n], XmNdeleteResponse, XmDO_NOTHING) ; n++ ;
	/*
	if (HelpFather)
		helpwidget = XmCreateMessageDialog (HelpFather, "XtasHelpBox", args, n) ;
	else
	*/
		helpwidget = XmCreateMessageDialog (XtasDeskMainForm, "XtasHelpBox", args, n) ;

	XtAddCallback   (helpwidget, XmNokCallback, XtasLeaveHelpCallback, (XtPointer)NULL );
	XtUnmanageChild (XmMessageBoxGetChild (helpwidget, XmDIALOG_HELP_BUTTON)) ;
	XtUnmanageChild (XmMessageBoxGetChild (helpwidget, XmDIALOG_CANCEL_BUTTON)) ;

	text = HELP_MESSAGES[(((long)client_data) & 0xff00)>>8][((long)client_data) & 0x00ff] ;
	
    n = 0;
    XtSetArg (args[n], XmNvalue   , text) ;              n++ ;
	XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT) ; n++ ;
	XtSetArg (args[n], XmNeditable, False)             ; n++ ;
 	textwidget = XmCreateScrolledText (helpwidget, "XtasHelpText", args, n) ;
	XtManageChild (textwidget) ;
	XtManageChild (helpwidget) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasHelpOnVersionCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Help On Version command.         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasHelpOnVersionCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XalCreateOnVersion( XtParent(widget) , XtasOnVersion );
}

