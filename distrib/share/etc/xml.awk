#!/usr/local/bin/gawk -f

# This script creates an .xml file from a .txt file
# The .txt file should come from an MS-Word .doc file 
# The .txt file must be saved with the 'LF only' option
# (Even with this option, ^M may remain in the .txt file. In this case,
# it is necessary to remove them manually).
# The .doc file must be well-formed

BEGIN {
    line = 0;
    chapter_opened = 0;
    section_opened = 0;
    list_opened = 0;
    level = 0;
}
{
    filexml = "YAG_introduction.xml";

    if (line == 0)
        printf ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n") > filexml;

    #chapter or section
    if ((int ($1) >= 1) && (int ($1) <= 9)) {

        #level
        level = split ($1, word, ".");

        #chapter
        if (level == 1) {
            while (section_opened > 0) {
                printf ("</section>\n\n") > filexml;
                section_opened--;
            }
            if (chapter_opened == 1) {
                printf ("</chapter>\n\n\n") > filexml;
                chapter_opened = 0;
            }
            printf ("<chapter>\n") > filexml;
            chapter_opened = 1;
        }

        #section
        if (list_opened == 1) {
            printf ("</list>\n") > filexml;
            list_opened = 0;
        }
        while (section_opened > level - 1) {
            printf ("</section>\n\n") > filexml;
            section_opened--;
        }
        printf ("<section niv='%d'><title>", level) > filexml;
        for (i = 2; i < NF; i++) printf ("%s ", $i) > filexml; 
        printf ("%s</title>\n", $NF) > filexml; 
        section_opened++;
	} 

    #body
    else {
        if (length ($0) > 0) {

            #list
            if ($1 == "*") {
                if (list_opened == 0) {
                    printf ("<list>\n") > filexml;
                    list_opened = 1;
                }
                printf ("<item>", level) > filexml;
                for (i = 2; i < NF; i++) printf ("%s ", $i) > filexml; 
                printf ("%s</item>\n", $NF) > filexml; 
            }

            #other
            else {
                if (list_opened == 1) {
                    printf ("</list>\n") > filexml;
                    list_opened = 0;
                }
                printf ("<p>%s</p>\n", $0) > filexml;
            }
        }
    }

    line++;
}


END {
    while (section_opened > 0) {
        printf ("</section>\n\n") > filexml;
        section_opened--;
    }
    if (chapter_opened == 1) {
        printf ("</chapter>\n\n\n") > filexml;
        chapter_opened = 0;
    }
}
