library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity Detector_sim is
--  Port ( );
end Detector_sim;

architecture Behavioral of Detector_sim is
    signal clk, reset, S, input, output : std_logic := '0';
    constant clk_period : time := 10 ns;
    
    component Detector is port (
      clk : in std_logic;
      reset : in std_logic;
      S : in std_logic;
      input : in std_logic;
      output : out std_logic
      );
    end component;

begin
    portmap : Detector port map(
			clk => clk,
			reset => reset,
			S => S,
			input => input,
			output => output
	);
   
   clk_process :process
   begin
        clk <= '0';
        wait for clk_period/2;
        clk <= '1';
        wait for clk_period/2;
   end process;

   test: process
   begin     
      S <= '1';
      input <= '0'; wait for clk_period;
      
      S <= '0';
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      S <= '1';
      input <= '0'; wait for clk_period;
      S <= '0';
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '1'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;
      input <= '0'; wait for clk_period;

      wait;
   end process;
end Behavioral;