library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity Detector is
    Port (
        clk : in STD_LOGIC;
        reset : in STD_LOGIC;
        S : in STD_LOGIC;
        input : in STD_LOGIC;
        output : out STD_LOGIC
    );
end Detector;

architecture Behavioral of Detector is
	type st_mealy is (a,b,c,d,e,f,g);
	signal state : st_mealy := a;
	
	type st_mealy2 is (x,y);
	signal state2 : st_mealy2 := x;
	
	signal count : std_logic_vector(2 downto 0) := "000";
	signal stop : std_logic := '1';
	
begin 
    process(clk, reset) 
    begin
        if reset = '1' then
            output <= '0';
            state <= a;
        elsif stop = '1' then
            output <= '0';
            state <= a;
            count <= "000";
        elsif rising_edge(clk) and state2 = y then
            case state is
                when a =>
                    output <= '0';
                    if input = '0' then
                        state <= a;
                    else    
                        state <= b;
                    end if; 
                when b =>
                    output <= '0';
                    if input = '0' then
                        state <= a;                    
                    else    
                        state <= c;
                    end if; 
                when c =>
                    output <= '0';
                    if input = '0' then
                        state <= a;                
                    else    
                        state <= d;
                    end if; 
                when d =>
                    output <= '0';
                    if input = '0' then
                        state <= e;           
                    else    
                        state <= f;
                    end if; 
                when e =>
                    output <= '1';
                    count <= std_logic_vector(unsigned(count) + 1);
                    if input = '0' then
                        state <= a;                    
                    else    
                        state <= b;
                    end if; 
                when f =>
                    output <= '1';
                    count <= std_logic_vector(unsigned(count) + 1);
                    if input = '0' then
                        state <= e;                
                    else    
                        state <= g;
                    end if; 
                when g =>
                    if input = '0' then
                        state <= e;
                        output <= '1';
                        count <= std_logic_vector(unsigned(count) + 1);                 
                    else    
                        output <= '0';
                        state <= g;
                    end if;            
                when others =>
                    output <= '0';
                    state <= a;
            end case;
        end if;
    end process;  
    
    process(clk, reset)
    begin
        if rising_edge(clk) then
            case state2 is
                when x =>
                    if S = '1' then
                        state2 <= y;
                        stop <= '0';
                    else
                        state2 <= x;
                    end if;
                when y =>
                    if count = "111" then
                        state2 <= x;
                        stop <= '1';
                    else
                        state2 <= y;
                    end if;
            end case;
        end if;
    end process;  
end Behavioral;