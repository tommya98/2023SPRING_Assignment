----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/06 21:05:42
-- Design Name: 
-- Module Name: address_decoder - Behavioral
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
use ieee.std_logic_arith.all; --arith library 추가.
use ieee.std_logic_unsigned.all; --unsigned library 추가.

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity address_decoder is
  Port (
	s_address 		: in std_logic_vector(8 downto 0); -- PC_latch를 통과한 USB의 9 bit address signal
	s_cmd_data 		: in std_logic;
	pcs_addr_b 		: out std_logic; --180H~183H System clock setting(8524 setting) --active low
	reset_addr 		: out std_logic; --170H software reset (모드 동작 중에도 reset)
	reset8254_addr 	: out std_logic; --171H 8254 reset
	pc_RAM_addr 	: out std_logic; --160H PC mode. wen, ren signal에 따라 PC_RAM에 쓰기, 읽기.
	da_start_addr	: out std_logic; --150H PC_RAM의 데이터를 DAC로 출력함 (DA mode start)
	da_stop_addr	: out std_logic; --151H 위의 DA mode DAC출력을 멈춤.
	ad_RAM_addr		: out std_logic; --140H PC에서 요청한 데이터 갯수만큼 ADC하여, AD_RAM을 통하여 PC_RAM으로 옮기기.
	adr_RAM_addr	: out std_logic; --141H ren signal에 따라 AD_RAM에 있는 데이터 읽기.
	opt_step1_addr	: out std_logic; --130H 
	opt_step2_addr	: out std_logic; --131H
	opt_step3_addr	: out std_logic --132H
	);
end address_decoder;

architecture Behavioral of address_decoder is


begin
pcs_addr_b 		<= '0' when ((s_address = x"180" or s_address = x"181" or s_address = x"182" or s_address = x"183") and s_cmd_data = '1') else '1'; --active low
reset_addr		<= '1' when (s_address = x"170" and s_cmd_data = '1') else '0';
reset8254_addr 	<= '1' when (s_address = x"171" and s_cmd_data = '1') else '0';
pc_RAM_addr 	<= '1' when (s_address = x"160" and s_cmd_data = '1') else '0';
da_start_addr  	<= '1' when (s_address = x"150" and s_cmd_data = '1') else '0';
da_stop_addr  	<= '1' when (s_address = x"151" and s_cmd_data = '1') else '0';
ad_RAM_addr  	<= '1' when (s_address = x"140" and s_cmd_data = '1') else '0';
adr_RAM_addr  	<= '1' when (s_address = x"141" and s_cmd_data = '1') else '0';
opt_step1_addr 	<= '1' when (s_address = x"130" and s_cmd_data = '1') else '0';
opt_step2_addr 	<= '1' when (s_address = x"131" and s_cmd_data = '1') else '0';
opt_step3_addr  <= '1' when (s_address = x"132" and s_cmd_data = '1') else '0';

end Behavioral;
