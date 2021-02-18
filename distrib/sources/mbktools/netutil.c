#include <stdio.h>
#include <string.h>
#include MUT_H
#include AVT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include BEH_H
#include BEF_H
#include BEG_H
#include BHL_H
#include BVL_H

/***********************************************************************/

void logical_usage ()
{
	fprintf (stderr, "usage: netutil --logical_flat <options> logical_figure instance output_name\n");
	fputs ("        flatten the instance in the logical_figure\n", stderr);
	fprintf (stderr, "usage: netutil --logical_flat -r <options> logical_figure output_name\n");
	fputs ("        flatten logical_figure to the catalog\n", stderr);
	fprintf (stderr, "usage: netutil --logical_flat -t <options> logical_figure output_name\n");
	fputs ("        flatten logical_figure to the transistor\n", stderr);
    fprintf(stderr, "options : -norc        strip parasitics\n");
	fprintf(stderr, "          -fig=<name>  take <name> as top-level\n");
    fprintf (stderr, "\n");
}

/***********************************************************************/

int logical_flat (int argc, char **argv)
{
	lofig_list *p;
    losig_list *ptsig;
    int mode = 0;
    int striprc = 0;
    char *topname = NULL;
    int i;

	if (argc < 4) {
		logical_usage ();
        return 1;
    }

    for (i=1; i<argc; i++) {
       if (*argv[i] != '-') break;
       if (!strcmp(argv[i], "-r")) mode = 1;
       else if (!strcmp(argv[i], "-t")) mode = 2;
       else if (!strcmp(argv[i], "-norc")) striprc = 1;
       else if (!strncmp(argv[i], "-fig=", 5)) topname = argv[i]+5;
    }
    if (i==argc) {
        logical_usage(argv[0]);
        return 1;
    }

	if (mode == 1) {
		p = getlofig(argv[i],'A');
        if (topname != NULL) p = getloadedlofig(topname);
        if (p == NULL) {
            logical_usage(argv[0]);
            return 1;
        }
		rflattenlofig(p, YES, YES);
    }
	else if (mode == 2) {
        p = getlofig(argv[i],'A');
        if (topname != NULL) p = getloadedlofig(topname);
        if (p == NULL) {
            logical_usage(argv[0]);
            return 1;
        }
        flattenlofig(p, NULL, YES);
    }
	else {
		p = getlofig(argv[i], 'A');
        if (topname != NULL) p = getloadedlofig(topname);
        if (p == NULL) {
            logical_usage(argv[0]);
            return 1;
        }
        i++;
		flattenlofig(p, argv[i], YES);
    }

    if (striprc == 1) {
        for (ptsig = p->LOSIG; ptsig; ptsig = ptsig->NEXT) freelorcnet(ptsig);
    }

    p->NAME=argv[i+1];
	savelofig (p);
	return 0;
}

/***********************************************************************
	CP		layout VTI
	HNS	 	netlist VTI
	FNE	 	extracted netlist VTI
??	FDN	 	extracted netlist DAX
??	HDN	 	extracted hierarchical netlist DAX

	AP	 	layout ALLIANCE
	AL	 	netlist ALLIANCE

	EDI	 	netlist or layout EDIF

	CCT	 	structurel HILO
	VST	 	structurel VHDL 

	SPI	 	structurel SPICE 
***********************************************************************/

void convert_usage ()
{
    fprintf (stdout, "usage: netutil --convert in_format out_format in_file out_file\n");
    fprintf (stderr, "\n");
}

/***********************************************************************/

int convert (int argc, char **argv)
{
	lofig_list *LogicalFigure;
	char *InputName;
	char *OutputName;
	char *InputFormat;
	char *OutputFormat;
	int MbkIn;
	int MbkOut;

	if (argc != 5) {
        convert_usage ();
		return 1;
	}

	InputFormat = namealloc (argv[1]);
	OutputFormat = namealloc (argv[2]);
	InputName = namealloc (argv[3]);
	OutputName = namealloc (argv[4]);

	if ((!strcmp (InputFormat, "al")) ||
		(!strcmp (InputFormat, "vst")) ||
		(!strcmp (InputFormat, "cct")) ||
		(!strcmp (InputFormat, "edi")) ||
		(!strcmp (InputFormat, "vlg")) ||
		(!strcmp (InputFormat, "spi")) || (!strcmp (InputFormat, "fne")) || (!strcmp (InputFormat, "hns"))) {
		MbkIn = 1;
		strcpy (IN_LO, InputFormat);
	}
	else {
		fprintf (stderr, "Bad mbk input file format %s\n", InputFormat);
		return (1);
	}

	if ((!strcmp (OutputFormat, "al")) ||
		(!strcmp (OutputFormat, "vst")) ||
		(!strcmp (OutputFormat, "cct")) ||
		(!strcmp (OutputFormat, "edi")) ||
		(!strcmp (OutputFormat, "vlg")) ||
		(!strcmp (OutputFormat, "spi")) || (!strcmp (OutputFormat, "fne")) || (!strcmp (OutputFormat, "hns"))) {
		MbkOut = 1;
		strcpy (OUT_LO, OutputFormat);
	}
	else {
		fprintf (stderr, "Bad mbk output file format %s\n", InputFormat);
		return 1;
	}

	if (MbkOut != MbkIn) {
		fprintf (stderr, "Incompatible mbk input/output file format !\n");
		return 1;
	}

	fprintf (stdout, "\t--> Conversion of %s(%s) to %s(%s)\n", InputName, InputFormat, OutputName, OutputFormat);

	if (MbkIn == 1)
		LogicalFigure = getlofig (InputName, 'A');

	if (MbkOut == 1) {
		LogicalFigure->NAME = OutputName;
		savelofig (LogicalFigure);
	}
	fprintf (stdout, "\t--> File format translation's done !\n");

	return 0;
}

/************************************************************************/

void rtl_convert_usage ()
{
    fprintf (stdout, "usage: netutil --rtl_convert in_format out_format in_file out_file\n");
    fprintf (stderr, "\n");
}

/***********************************************************************/

int rtl_convert (int argc, char **argv)
{
	befig_list *BehFigure;
	char *InputName;
	char *OutputName;
	char *InputFormat;
	char *OutputFormat;

	if (argc != 5) {
        rtl_convert_usage ();
		return 1;
	}

	InputFormat = namealloc (argv[1]);
	OutputFormat = namealloc (argv[2]);
	InputName = namealloc (argv[3]);
	OutputName = namealloc (argv[4]);

	if ((!strcmp (InputFormat, "vhd")) || (!strcmp (InputFormat, "vlg")) || (!strcmp (InputFormat, "vbe"))) {
		BEH_IN = InputFormat;
	}
	else {
		fprintf (stderr, "Bad rtl input file format %s\n", InputFormat);
		return (1);
	}

	if ((!strcmp (InputFormat, "vhd")) || (!strcmp (InputFormat, "vlg")) || (!strcmp (InputFormat, "vbe"))) {
		BEH_OUT = InputFormat;
	}
	else {
		fprintf (stderr, "Bad rtl output file format %s\n", InputFormat);
		return 1;
	}

	fprintf (stdout, "\t--> Conversion of %s(%s) to %s(%s)\n", InputName, InputFormat, OutputName, OutputFormat);

    BehFigure = loadbefig (NULL, InputName, 0);

    BehFigure->NAME = OutputName;
    savebefig (BehFigure, 0);

	fprintf (stdout, "\t--> File format translation's done !\n");

	return 0;
}

/***********************************************************************/

int main (int argc, char **argv)
{
    avtenv ();
    mbkenv ();
    cbhenv ();

    avt_banner ("NetUtil", "Netlist Utilitaries","2002");

    if (argc == 1) {
        logical_usage ();
        convert_usage ();
        rtl_convert_usage ();
        EXIT(1) ;
    }
    
#ifdef AVERTEC_LICENSE
if(avt_givetoken("AVT_LICENSE_SERVER","avt") != AVT_VALID_TOKEN) {
 EXIT(1) ;
}
#endif

    if (!strcmp (argv[1], "--logical_flat") || !strcmp (argv[1], "-lflat")) {
        EXIT( logical_flat (argc - 1, argv + 1));
    }
    else
    if (!strcmp (argv[1], "--convert") || !strcmp (argv[1], "-conv")) {
        EXIT( convert (argc - 1, argv + 1));
    }
    else
    if (!strcmp (argv[1], "--rtl_convert") || !strcmp (argv[1], "-rtl")) {
        EXIT( rtl_convert (argc - 1, argv + 1));
    }
    else {
        logical_usage ();
        convert_usage ();
        rtl_convert_usage ();
        EXIT(1) ;
    }
}
