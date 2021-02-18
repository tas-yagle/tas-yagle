library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_textio.all;
use std.textio.all;

entity tb_ram4x4096 is
end tb_ram4x4096;

architecture struct of tb_ram4x4096 is

    component ram4x4096_yagroot
        port (
            adr: in std_logic_vector(11 downto 0);
            ck: in std_logic;
            dout: inout std_logic_vector(3 downto 0);
            en: in std_logic;
            vdd: in std_logic;
            vss: in std_logic;
            write: in std_logic
        );
    end component;

    signal sig_adr: std_logic_vector(11 downto 0);
    signal sig_ck: std_logic;
    signal sig_dout: std_logic_vector(3 downto 0);
    signal sig_en: std_logic;
    signal sig_vdd: std_logic;
    signal sig_vss: std_logic;
    signal sig_write: std_logic;

begin

    ins_ram4x4096_yagroot: ram4x4096_yagroot
        port map (
            adr => sig_adr,
            ck => sig_ck,
            dout => sig_dout,
            en => sig_en,
            vdd => sig_vdd,
            vss => sig_vss,
            write => sig_write
        );

    testbench: process
        file infile: text is in "ram4x4096.pat";
        variable lin: line;
        variable lout: line;
        variable mode: character;

        constant sko: string := "SIMULATION MISMATCH";
        constant sprocess: string := "--- Processing pattern ---";
        -- assigned values
        variable var_adr: std_logic_vector(11 downto 0);
        variable var_ck: std_logic;
        variable var_dout: std_logic_vector(3 downto 0);
        variable var_en: std_logic;
        variable var_write: std_logic;

        -- expected values
        variable var_expected_adr: std_logic_vector(11 downto 0);
        variable var_expected_ck: std_logic;
        variable var_expected_dout: std_logic_vector(3 downto 0);
        variable var_expected_en: std_logic;
        variable var_expected_write: std_logic;

        -- check flags
        variable check_adr: boolean := false;
        variable check_ck: boolean := false;
        variable check_dout: boolean := false;
        variable check_en: boolean := false;
        variable check_write: boolean := false;

    begin
        while not (endfile (infile)) loop
            readline (infile, lin);

            -------------------------------------
            -- read values
            -------------------------------------

            -- adr
            read (lin, mode); -- skipping white space
            read (lin, mode);
            read (lin, var_adr);
            if (mode = 'A') then
                sig_adr <= var_adr;
                check_adr := false;
            else
                var_expected_adr := var_adr;
                check_adr := true;
            end if;

            -- ck
            read (lin, mode); -- skipping white space
            read (lin, mode);
            read (lin, var_ck);
            if (mode = 'A') then
                sig_ck <= var_ck;
                check_ck := false;
            else
                var_expected_ck := var_ck;
                check_ck := true;
            end if;

            -- dout
            read (lin, mode); -- skipping white space
            read (lin, mode);
            read (lin, var_dout);
            if (mode = 'A') then
                sig_dout <= var_dout;
                check_dout := false;
            else
                var_expected_dout := var_dout;
                check_dout := true;
            end if;

            -- en
            read (lin, mode); -- skipping white space
            read (lin, mode);
            read (lin, var_en);
            if (mode = 'A') then
                sig_en <= var_en;
                check_en := false;
            else
                var_expected_en := var_en;
                check_en := true;
            end if;

            -- write
            read (lin, mode); -- skipping white space
            read (lin, mode);
            read (lin, var_write);
            if (mode = 'A') then
                sig_write <= var_write;
                check_write := false;
            else
                var_expected_write := var_write;
                check_write := true;
            end if;

            -------------------------------------
            -- wait
            -------------------------------------

            wait for 10 ps;


            -------------------------------------
            -- check values
            -------------------------------------
            write (lout, sprocess);
            writeline (output, lout);

             -- adr
             if (check_adr = true) then
                 if (var_expected_adr /= sig_adr) then
                     write (lout, sko);
                     writeline (output, lout);
                 end if;
             end if;

             -- ck
             if (check_ck = true) then
                 if (var_expected_ck /= sig_ck) then
                     write (lout, sko);
                     writeline (output, lout);
                 end if;
             end if;

             -- dout
             if (check_dout = true) then
                 if (var_expected_dout /= sig_dout) then
                     write (lout, sko);
                     writeline (output, lout);
                 end if;
             end if;

             -- en
             if (check_en = true) then
                 if (var_expected_en /= sig_en) then
                     write (lout, sko);
                     writeline (output, lout);
                 end if;
             end if;

             -- write
             if (check_write = true) then
                 if (var_expected_write /= sig_write) then
                     write (lout, sko);
                     writeline (output, lout);
                 end if;
             end if;
        end loop;
        wait;
    end process;

end struct;

configuration tb of tb_ram4x4096 is
    for struct
    end for;
end tb;
