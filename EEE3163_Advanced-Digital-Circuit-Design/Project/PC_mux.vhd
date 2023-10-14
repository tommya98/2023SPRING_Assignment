----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/11 21:07:06
-- Design Name: 
-- Module Name: PC_mux - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity PC_mux is
  Port ( 
    sel : IN STD_LOGIC;
    Din0 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din1 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
end PC_mux;

architecture Behavioral of PC_mux is
begin

	process(sel, Din0, Din1)
	begin
		if sel = '0' then
			Dout <= Din0;
		elsif sel = '1' then
			Dout <= Din1;
		else
			Dout <= (others => '0');
		end if;
	end process;

end Behavioral;
