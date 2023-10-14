----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/02/13 19:24:05
-- Design Name: 
-- Module Name: filter - Behavioral
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
use IEEE.STD_LOGIC_SIGNED.ALL;
use IEEE.NUMERIC_STD.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity filter is
 Port ( 
		m_reset			: in std_logic; --active high
		m_clk			: in std_logic; --s_clk
		m_en			: in std_logic; --enable 신호
		m_xin			: in std_logic_vector(7 downto 0);		-- signed
		m_yout			: out std_logic_vector(7 downto 0)		-- signed
		-- m_yout_valid	: out std_logic			-- 필요할 경우 사용
		-- 자유롭게 signal을 추가하셔도 됩니다.
		);
end filter;


architecture Behavioral of filter is

component filter_multiplier is
	Port( 
		CLK : IN STD_LOGIC;
		A : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		B : IN STD_LOGIC_VECTOR(8 DOWNTO 0);
		CE : IN STD_LOGIC;
		P : OUT STD_LOGIC_VECTOR(16 DOWNTO 0)
	);
end component;

type std_8bit_array is array (natural range <>) of std_logic_vector(7 downto 0);
type std_9bit_array is array (natural range <>) of std_logic_vector(8 downto 0);
type std_17bit_array is array (natural range <>) of std_logic_vector(16 downto 0);
type std_18bit_array is array (natural range <>) of std_logic_vector(17 downto 0);
type std_19bit_array is array (natural range <>) of std_logic_vector(18 downto 0);
type std_20bit_array is array (natural range <>) of std_logic_vector(19 downto 0);

signal s_start : std_logic_vector(7 downto 0); --m_xin을 넣어줄 곳.

signal s_din : std_8bit_array(14 downto 0); --delay 된 data(8bit) (0~14번째까지)

signal s_multiply_val : std_17bit_array(14 downto 0):=(others=>(others=>'0')); --곱한 값 출력(17bit) (0~14번째까지)

signal s_add1 : std_18bit_array(7 downto 0):=(others=>(others=>'0')); --1번 더한 값 (18bit x 8개) (마지막 1개는 bit extension)

signal s_add2 : std_19bit_array(3 downto 0):=(others=>(others=>'0')); --2번 더한 값 (19bit x 4개)

signal s_add3 : std_20bit_array(1 downto 0):=(others=>(others=>'0')); --3번 더한 값 (20bit x 2개)
 
signal s_filter_result : std_logic_vector(20 downto 0):=(others=>'0'); --최종 21 bit 출력




signal s_filter_coef : std_9bit_array(14 downto 0) := (others=>(others=>'0')); -- array 선언 : 9bit가 총 15개. Filter coefficient.

begin

-- 15 tap filter (signed)
s_filter_coef(0) <= conv_std_logic_vector(-1,9);
s_filter_coef(1) <= conv_std_logic_vector(-3,9);
s_filter_coef(2) <= conv_std_logic_vector(-6,9);
s_filter_coef(3) <= conv_std_logic_vector(0,9);
s_filter_coef(4) <= conv_std_logic_vector(25,9);
s_filter_coef(5) <= conv_std_logic_vector(67,9);
s_filter_coef(6) <= conv_std_logic_vector(110,9);
s_filter_coef(7) <= conv_std_logic_vector(128,9);
s_filter_coef(8) <= conv_std_logic_vector(110,9);
s_filter_coef(9) <= conv_std_logic_vector(67,9);
s_filter_coef(10) <= conv_std_logic_vector(25,9);
s_filter_coef(11) <= conv_std_logic_vector(0,9);
s_filter_coef(12) <= conv_std_logic_vector(-6,9);
s_filter_coef(13) <= conv_std_logic_vector(-3,9);
s_filter_coef(14) <= conv_std_logic_vector(-1,9);

multiplier_gen : for i in 0 to 14 generate
mul : filter_multiplier port map(
	CLK => m_clk,
	A => s_din(i), --A 8bit
	B => s_filter_coef(i), --B 9bit
	CE => m_en,
	P => s_multiply_val(i) --곱셈 결과 17 bit
	);
end generate;

--D flipflop
process(m_clk) -- m_en = 1일 때만 flipflop으로 data가 한 클락씩 넘어가게 함.
begin
	if rising_edge(m_clk) then
		if m_reset = '1' then
			s_start <= (others => '0');
			s_din <= (others=>(others=>'0'));
		elsif m_en = '1' then
			s_start <= m_xin;
			s_din(0) <= s_start;
			s_din(14 downto 1) <= s_din(13 downto 0);
		end if;
	end if;
end process;



add1 : for i in 0 to 6 generate
process(m_clk,m_reset)
begin
	if m_reset = '1' then
		s_add1(i) <= (others => '0');
	elsif rising_edge(m_clk) then
		s_add1(i) <= (s_multiply_val(i*2+0)(16) & s_multiply_val(i*2+0)) + (s_multiply_val(i*2+1)(16) & s_multiply_val(i*2+1)); --sign extension 하고 더하기
	end if;
end process;
end generate;
process(m_clk,m_reset)
begin
	if m_reset = '1' then
		s_add1(7) <= (others => '0');
	elsif rising_edge(m_clk) then
		s_add1(7) <= (s_multiply_val(14)(16) & s_multiply_val(14));
	end if;
end process;

add2 : for i in 0 to 3 generate
process(m_clk,m_reset)
begin
	if m_reset = '1' then
		s_add2(i) <= (others => '0');
	elsif rising_edge(m_clk) then
		s_add2(i) <= (s_add1(i*2+0)(17) & s_add1(i*2+0)) + (s_add1(i*2+1)(17) & s_add1(i*2+1)); --sign extension 하고 더하기
	end if;
end process;
end generate;

add3 : for i in 0 to 1 generate
process(m_clk,m_reset)
begin
	if m_reset = '1' then
		s_add3(i) <= (others => '0');
	elsif rising_edge(m_clk) then
		s_add3(i) <= (s_add2(i*2+0)(18) & s_add2(i*2+0)) + (s_add2(i*2+1)(18) & s_add2(i*2+1)); --sign extension 하고 더하기
	end if;
end process;
end generate;

add4 : process(m_clk,m_reset)
begin
	if m_reset = '1' then
		s_filter_result <= (others => '0');
	elsif rising_edge(m_clk) then
		s_filter_result <= (s_add3(0)(19) & s_add3(0)) + (s_add3(1)(19) & s_add3(1)); --sign extension 하고 더하기
	end if;
end process;


m_yout <= s_filter_result(20) & s_filter_result(15 downto 9);

end Behavioral;
