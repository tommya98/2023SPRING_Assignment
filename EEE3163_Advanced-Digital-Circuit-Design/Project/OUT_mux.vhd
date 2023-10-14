----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/11 21:06:50
-- Design Name: 
-- Module Name: OUT_mux - Behavioral
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

entity OUT_mux is
  Port ( 
    sel : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    Din0 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din1 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din2 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
end OUT_mux;

architecture Behavioral of OUT_mux is

begin
	process(sel, Din0, Din1, Din2)
	begin
		if sel = "00" then
			Dout <= Din0;
		elsif sel = "01" then
			Dout <= Din1;
		elsif sel = "10" then
			Dout <= Din2;
		else
			Dout <= (others => '0');
		end if;
	end process;

end Behavioral;
