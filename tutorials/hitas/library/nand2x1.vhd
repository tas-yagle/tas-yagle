-- VHDL data flow description generated from `nand2x1`
--		date : Wed Oct 31 16:59:43 2001

library IEEE;
use IEEE.std_logic_1164.all;

-- Entity Declaration

ENTITY nand2x1 IS
  PORT (
  vdd : in std_logic;
  vss : in std_logic;
  a : in std_logic;
  b : in std_logic;
  y : out std_logic
  );
END nand2x1;

-- Architecture Declaration

ARCHITECTURE RTL OF nand2x1 IS

BEGIN

  y <= (not (b) or not (a));
END;
