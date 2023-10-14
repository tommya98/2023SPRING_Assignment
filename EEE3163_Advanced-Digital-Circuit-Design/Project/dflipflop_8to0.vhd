----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/23 22:13:37
-- Design Name: 
-- Module Name: dflipflop_8to0 - Behavioral
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

entity dflipflop_8to0 is
  Port (
	reset_b : in std_logic;
	din : in std_logic_vector(8 downto 0);
	clk : in std_logic;
	dout : out std_logic_vector(8 downto 0)
	);
end dflipflop_8to0;

architecture Behavioral of dflipflop_8to0 is

begin

	process(reset_b, clk)
	begin
		if reset_b = '0' then
			dout <= "000000000";
		elsif rising_edge(clk) then
			dout <= din;
		end if;
	end process;

end Behavioral;