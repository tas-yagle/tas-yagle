/* functions for the pattern.c file */

#include "tbg.h"

int CHECK = 0;

void GenerateTestbench (char *name)
{
    FILE *testbench;
    char  tb_name[1024];
    char  sig_name[1024];
    char  var_name[1024];
    int b0, b1;
    t_port *port;
    chain_list *ch, *v;

    sprintf (tb_name, "%s.vhd", name);

    testbench = fopen (tb_name, "w+"); 

    /* libraries */
    fprintf (testbench, "library IEEE;\n");
    fprintf (testbench, "use IEEE.std_logic_1164.all;\n");
    fprintf (testbench, "use IEEE.std_logic_textio.all;\n");
    fprintf (testbench, "use std.textio.all;\n\n");
    /* entity */
    fprintf (testbench, "entity %s is\n", name);
    fprintf (testbench, "end %s;\n\n", name);

    /* architecture */
    fprintf (testbench, "architecture struct of %s is\n\n", name);
    fprintf (testbench, "    component %s\n", DESIGN_NAME);
    if (GENERICS_CHAIN) {
    fprintf (testbench, "        generic (\n");
        for (ch = GENERICS_CHAIN; ch->NEXT; ch = ch->NEXT)
            fprintf (testbench, "            %s: integer;\n", (char*)ch->DATA);
        fprintf (testbench, "            %s: integer\n", (char*)ch->DATA);
        fprintf (testbench, "        );\n");
    }
    fprintf (testbench, "        port (\n");
    if (HEAD_PORT) {
        for (port = HEAD_PORT; port->NEXT; port = port->NEXT) {
            b0 = port->B0;
            b1 = port->B1;
            if (b0 == b1) {
                if (port->DIRECTION == T_IN)
                    fprintf (testbench, "            %s: in std_logic;\n", port->NAME);
                else
                if (port->DIRECTION == T_OUT)
                    fprintf (testbench, "            %s: out std_logic;\n", port->NAME);
                else
                if (port->DIRECTION == T_INOUT)
                    fprintf (testbench, "            %s: inout std_logic;\n", port->NAME);
            } else
            if (b0 > b1) {
                if (port->DIRECTION == T_IN)
                    fprintf (testbench, "            %s: in std_logic_vector(%d downto %d);\n", port->NAME, b0, b1);
                else
                if (port->DIRECTION == T_OUT)
                    fprintf (testbench, "            %s: out std_logic_vector(%d downto %d);\n", port->NAME, b0, b1);
                else
                if (port->DIRECTION == T_INOUT)
                    fprintf (testbench, "            %s: inout std_logic_vector(%d downto %d);\n", port->NAME, b0, b1);
            } else
            if (b0 < b1) {
                if (port->DIRECTION == T_IN)
                    fprintf (testbench, "            %s: in std_logic_vector(%d to %d);\n", port->NAME, b0, b1);
                else
                if (port->DIRECTION == T_OUT)
                    fprintf (testbench, "            %s: out std_logic_vector(%d to %d);\n", port->NAME, b0, b1);
                else
                if (port->DIRECTION == T_INOUT)
                    fprintf (testbench, "            %s: inout std_logic_vector(%d to %d);\n", port->NAME, b0, b1);
            }
        }
        b0 = port->B0;
        b1 = port->B1;
        if (b0 == b1) {
            if (port->DIRECTION == T_IN)
                fprintf (testbench, "            %s: in std_logic\n", port->NAME);
            else
            if (port->DIRECTION == T_OUT)
                fprintf (testbench, "            %s: out std_logic\n", port->NAME);
            else
            if (port->DIRECTION == T_INOUT)
                fprintf (testbench, "            %s: inout std_logic\n", port->NAME);
        } else
        if (b0 > b1) {
            if (port->DIRECTION == T_IN)
                fprintf (testbench, "            %s: in std_logic_vector(%d downto %d)\n", port->NAME, b0, b1);
            else
            if (port->DIRECTION == T_OUT)
                fprintf (testbench, "            %s: out std_logic_vector(%d downto %d)\n", port->NAME, b0, b1);
            else
            if (port->DIRECTION == T_INOUT)
                fprintf (testbench, "            %s: inout std_logic_vector(%d downto %d)\n", port->NAME, b0, b1);
        } else
        if (b0 < b1) {
            if (port->DIRECTION == T_IN)
                fprintf (testbench, "            %s: in std_logic_vector(%d to %d)\n", port->NAME, b0, b1);
            else
            if (port->DIRECTION == T_OUT)
                fprintf (testbench, "            %s: out std_logic_vector(%d to %d)\n", port->NAME, b0, b1);
            else
            if (port->DIRECTION == T_INOUT)
                fprintf (testbench, "            %s: inout std_logic_vector(%d to %d)\n", port->NAME, b0, b1);
        }
    }
    fprintf (testbench, "        );\n");
    fprintf (testbench, "    end component;\n\n");

    /* assignation signals */
    for (port = HEAD_PORT; port; port = port->NEXT) {
        b0 = port->B0;
        b1 = port->B1;
        sprintf (sig_name, "sig_%s", port->NAME);
        if (b0 == b1) 
            fprintf (testbench, "    signal %s: std_logic;\n", sig_name);
        else
        if (b0 > b1) 
            fprintf (testbench, "    signal %s: std_logic_vector(%d downto %d);\n", sig_name, b0, b1);
        else
        if (b0 < b1) 
            fprintf (testbench, "    signal %s: std_logic_vector(%d to %d);\n", sig_name, b0, b1);
    }

    /* begin */
    fprintf (testbench, "\nbegin\n\n");

    /* port map */
    fprintf (testbench, "    ins_%s: %s\n", DESIGN_NAME, DESIGN_NAME);
    if (HEAD_PORT) {
        if (GENERICS_CHAIN) {
            fprintf (testbench, "        generic map (\n");
            for (ch = GENERICS_CHAIN, v = VALUES_CHAIN; ch->NEXT; ch = ch->NEXT, v = v->NEXT)
                fprintf (testbench, "            %s => %d,\n", (char*)ch->DATA, (int)v->DATA);
            fprintf (testbench, "            %s => %d\n", (char*)ch->DATA, (int)v->DATA);
            fprintf (testbench, "        )\n");
        }
    }
    fprintf (testbench, "        port map (\n");
    if (HEAD_PORT) {
        for (port = HEAD_PORT; port->NEXT; port = port->NEXT)
            fprintf (testbench, "            %s => sig_%s,\n", port->NAME, port->NAME);
        fprintf (testbench, "            %s => sig_%s\n", port->NAME, port->NAME);
        fprintf (testbench, "        );\n\n");
    }

    /* bench process */
    fprintf (testbench, "    testbench: process\n");
    fprintf (testbench, "        file infile: text is in \"%s\";\n", PAT_FILE_NAME);
    fprintf (testbench, "        file outfile: text is out \"%s\";\n", OUT_FILE_NAME);
    fprintf (testbench, "        variable lin: line;\n");
    fprintf (testbench, "        variable lout: line;\n");
    fprintf (testbench, "        variable mode: character;\n");
    fprintf (testbench, "        constant space: string := \" \";\n");
    /* variables for assigned values */
    fprintf (testbench, "        -- assigned values\n");
    for (port = HEAD_PORT; port; port = port->NEXT) {
        if (!port->FLAG) continue;
        b0 = port->B0;
        b1 = port->B1;
        sprintf (var_name, "var_%s", port->NAME);
        if (b0 == b1)
            fprintf (testbench, "        variable %s: std_logic;\n", var_name);
        else
        if (b0 > b1) 
            fprintf (testbench, "        variable %s: std_logic_vector(%d downto %d);\n", var_name, b0, b1);
        else
        if (b0 < b1)
            fprintf (testbench, "        variable %s: std_logic_vector(%d to %d);\n", var_name, b0, b1);
    }

    /* process body */
    fprintf (testbench, "\n    begin\n");
    fprintf (testbench, "        while not (endfile (infile)) loop\n");
    fprintf (testbench, "            readline (infile, lin);\n");

    fprintf (testbench, "\n            -------------------------------------\n");
    fprintf (testbench, "            -- read values\n"); 
    fprintf (testbench, "            -------------------------------------\n");
        
    for (port = HEAD_PORT; port; port = port->NEXT) {
        if (!port->FLAG) continue;
        sprintf (var_name, "var_%s", port->NAME);
        sprintf (sig_name, "sig_%s", port->NAME);

        fprintf (testbench, "\n            -- %s\n", port->NAME);
        fprintf (testbench, "            read (lin, mode); -- skipping white space\n");
        fprintf (testbench, "            read (lin, %s);\n", var_name);
        fprintf (testbench, "            %s <= %s;\n", sig_name, var_name);
    }
    fprintf (testbench, "\n            -------------------------------------\n");
    fprintf (testbench, "            -- wait\n"); 
    fprintf (testbench, "            -------------------------------------\n");
        
    switch (UNIT) {
        case 'P': 
            fprintf (testbench, "\n            wait for %d ps;\n\n", PERIOD);
            break;
        case 'N': 
            fprintf (testbench, "\n            wait for %d ns;\n\n", PERIOD);
            break;
        case 'U': 
            fprintf (testbench, "\n            wait for %d us;\n\n", PERIOD);
            break;
        case 'M': 
            fprintf (testbench, "\n            wait for %d ms;\n\n", PERIOD);
            break;
        case 'S': 
            fprintf (testbench, "\n            wait for %d s;\n\n", PERIOD);
            break;
    }

    fprintf (testbench, "\n            -------------------------------------\n");
    fprintf (testbench, "            -- write values\n"); 
    fprintf (testbench, "            -------------------------------------\n");
    for (port = HEAD_PORT; port; port = port->NEXT) {
        sprintf (sig_name, "sig_%s", port->NAME);

        fprintf (testbench, "\n            -- %s\n", port->NAME);
        fprintf (testbench, "            write (lout, %s);\n", sig_name);
        fprintf (testbench, "            write (lout, space);\n");
    }
    fprintf (testbench, "\n            writeline (output, lout);\n");
    
    fprintf (testbench, "        end loop;\n");
    fprintf (testbench, "        wait;\n");
    fprintf (testbench, "    end process;\n\n");
    fprintf (testbench, "end struct;\n\n");
    fprintf (testbench, "configuration tb of %s is\n", name);
    fprintf (testbench, "    for struct\n");
    fprintf (testbench, "    end for;\n");
    fprintf (testbench, "end tb;\n");
                                                
    fclose (testbench);

    if (PAT_FILE)
        fclose (PAT_FILE);
}
