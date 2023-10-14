----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/11 11:04:41
-- Design Name: 
-- Module Name: tribuffer - Behavioral
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

entity tribuffer is
  Port ( 
    tri_en_b : IN STD_LOGIC;
    Data_in : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Data_out : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    Data_inout : INOUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
end tribuffer;

architecture Behavioral of tribuffer is

begin
	Data_out <= Data_inout;
	Data_inout <= Data_in when tri_en_b = '0' else (others=>'Z');
end Behavioral;
