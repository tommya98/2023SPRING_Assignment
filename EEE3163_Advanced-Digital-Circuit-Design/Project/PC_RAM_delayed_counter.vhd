----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/19 13:46:15
-- Design Name: 
-- Module Name: PC_RAM_delayed_counter - Behavioral
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
use ieee.std_logic_unsigned.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity PC_RAM_delayed_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end PC_RAM_delayed_counter;

architecture Behavioral of PC_RAM_delayed_counter is

signal s_cnt_val : std_logic_vector(10 downto 0) := (others => '0');

begin
	process(clk)
	begin
		if rising_edge(clk) then
			if reset_b = '0' then
				s_cnt_val <= (others => '0');
			elsif cnt_en = '1' then
				s_cnt_val <= s_cnt_val + '1';
			else
				s_cnt_val <= s_cnt_val;
			end if;
		end if;
	end process;
	
	cnt_out <= s_cnt_val;

end Behavioral;
