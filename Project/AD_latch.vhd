----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/11 21:06:33
-- Design Name: 
-- Module Name: AD_latch - Behavioral
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

entity AD_latch is
  Port ( 
	reset_b : in std_logic;
    sys_clk : IN STD_LOGIC;
    en : IN STD_LOGIC;
    Din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
end AD_latch;

architecture Behavioral of AD_latch is

begin

	process(reset_b, sys_clk)
	begin
		if reset_b = '0' then
			Dout <= "00000000";
		elsif rising_edge(sys_clk) then
		 if en = '1' then
		    Dout <= Din;
		 end if;
		end if;
	end process;

end Behavioral;
