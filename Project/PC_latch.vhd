----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/06 22:12:15
-- Design Name: 
-- Module Name: PC_latch - Behavioral
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


entity PC_latch is
  Port (
	reset_b : in std_logic;
	s_clk		: in std_logic; --PC latch에는 FPGA clock (40MHz)를 이용. 보강 자료 회로도에는 clock이 안 그려져 있음 - 규현
	m_cmd_data 	: in std_logic; --USB input port 신호
	m_wen 		: in std_logic; --USB input port 신호
	m_ren 		: in std_logic; --USB input port 신호
	m_oe_b	 	: in std_logic; --USB input port 신호
	m_address 	: in std_logic_vector (8 downto 0); --USB input port 신호
	s_cmd_data	: out std_logic; --FPGA 내부로 들어오는 신호
	s_wen 		: out std_logic; --FPGA 내부로 들어오는 신호
	s_ren 		: out std_logic; --FPGA 내부로 들어오는 신호
	s_oe_b 		: out std_logic; --FPGA 내부로 들어오는 신호
	s_address 	: out std_logic_vector (8 downto 0) --FPGA 내부로 들어오는 신호
	);
end PC_latch;

architecture Behavioral of PC_latch is

component dflipflop is
  Port (
	reset_b : in std_logic;
	din : in std_logic;
	clk : in std_logic;
	dout : out std_logic
	);
end component;

component dflipflop_8to0 is
  Port (
	reset_b : in std_logic;
	din : in std_logic_vector(8 downto 0);
	clk : in std_logic;
	dout : out std_logic_vector(8 downto 0)
	);
end component;

begin
m_cmd_data_latch : dflipflop port map(
	reset_b => reset_b,
	din => m_cmd_data,
	clk => s_clk,
	dout => s_cmd_data
	);
	
m_wen_latch : dflipflop port map(
	reset_b => reset_b,
	din => m_wen,
	clk => s_clk,
	dout => s_wen
	);
	
m_ren_latch : dflipflop port map(
	reset_b => reset_b,
	din => m_ren,
	clk => s_clk,
	dout => s_ren
	);
	
m_oe_b_latch : dflipflop port map(
	reset_b => reset_b,
	din => m_oe_b,
	clk => s_clk,
	dout => s_oe_b
	);

m_address_latch : dflipflop_8to0 port map(
	reset_b => reset_b,
	din => m_address,
	clk => s_clk,
	dout => s_address
	);



end Behavioral;
