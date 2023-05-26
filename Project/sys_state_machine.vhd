----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/25 00:49:00
-- Design Name: 
-- Module Name: sys_state_machine - Behavioral
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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;


--AD mode에서 필요한 sys_clk_state_machine
entity sys_state_machine is
  Port (
	sys_clk : in std_logic;
	s_sys_state_machine_on : in std_logic;
	s_AD_RAM_addra10 : in std_logic_vector(10 downto 0);
	s_AD_RAM_max_address : in std_logic_vector(10 downto 0);
	s_AD_RAM_cnta_reset_b : out std_logic;
	s_AD_RAM_cnta_ce : out std_logic;
	s_AD_RAM_ena : out std_logic;
	s_AD_RAM_wea : out std_logic_vector(0 downto 0);
	s_AD_write_finish : out std_logic
	
	);
end sys_state_machine;

architecture Behavioral of sys_state_machine is
type state is (sys_st_idle, sys_st_AD_ready, sys_st_AD_ready2, sys_st_AD_write, sys_st_ADW_done0, sys_st_ADW_done1, sys_st_ADW_done2);
signal sys_state : state; 


begin

process(sys_clk)
begin
	if rising_edge(sys_clk) then
	case sys_state is
		when sys_st_idle =>
			if(s_sys_state_machine_on = '1') then
				sys_state <= sys_st_AD_ready;
			else
				sys_state <= sys_st_idle;
			end if;
		
		when sys_st_AD_ready =>
				sys_state <= sys_st_AD_ready2;
		
		when sys_st_AD_ready2 =>
				sys_state <= sys_st_AD_write;
		
		when sys_st_AD_write =>
			if (s_AD_RAM_addra10 = s_AD_RAM_max_address) then
				sys_state <= sys_st_ADW_done0;
			else
				sys_state <= sys_st_AD_write;
			end if;
		
		when sys_st_ADW_done0 =>
				sys_state <= sys_st_ADW_done1;
		
		when sys_st_ADW_done1 =>
				sys_state <= sys_st_ADW_done2;
				
		when sys_st_ADW_done2 =>
				sys_state <= sys_st_idle;
				
		when others =>
				sys_state <= sys_st_idle;
	end case;
	end if;
end process;
				
	s_AD_RAM_cnta_reset_b <= '0' when (sys_state = sys_st_idle or sys_state = sys_st_ADW_done0 or sys_state = sys_st_ADW_done1 or sys_state = sys_st_ADW_done2) else '1';
	s_AD_RAM_cnta_ce <= '1' when (sys_state = sys_st_AD_write) else '0';
	s_AD_RAM_ena <= '1' when (sys_state = sys_st_AD_ready or sys_state = sys_st_AD_ready2 or sys_state = sys_st_AD_write or sys_state = sys_st_ADW_done0 or sys_state = sys_st_ADW_done1 or sys_state = sys_st_ADW_done2) else '0';
	s_AD_RAM_wea <= "1" when (sys_state = sys_st_AD_write) else "0";
	s_AD_write_finish <= '1' when (sys_state = sys_st_ADW_done0 or sys_state = sys_st_ADW_done1 or sys_state = sys_st_ADW_done2) else '0';			
				



end Behavioral;
