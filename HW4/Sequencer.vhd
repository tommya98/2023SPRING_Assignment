library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity Sequencer_sim is
--  Port ( );
end Sequencer_sim;

architecture Behavioral of Sequencer_sim is
	signal Clock, PWR_RS, START : std_logic := '0';
	constant clk_period : time := 10 ns;
	
	component Sequencer is port (
		Clock : in std_logic;
		PWR_RS : in std_logic;
		START : in std_logic
		);
    end component;
begin
	portmap : Sequencer port map(
			Clock => Clock,
			PWR_RS => PWR_RS,
			START => START
	);
	
	Clock_process :process
	begin
        Clock <= '0';
        wait for clk_period/2;
        Clock <= '1';
        wait for clk_period/2;
	end process;

	test : process
	begin
		PWR_RS <= '1'; START <= '1'; wait for clk_period;
		
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		wait for clk_period;
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		wait for clk_period;
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		wait for clk_period;
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;
		wait for clk_period;
		wait for clk_period;
		START <= '0'; wait for clk_period; START <= '1';
		wait for clk_period;	
		wait;
	end process;
end Behavioral;
