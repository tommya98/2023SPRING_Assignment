----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/05/11 21:07:35
-- Design Name: 
-- Module Name: Signal_Controller - Behavioral
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

entity Signal_Controller is
  Port ( 
	--signal controller 입력
	  --PC_latch 신호들
	m_reset_b : in std_logic;
    s_clk : IN STD_LOGIC;
    sys_clk : IN STD_LOGIC;
    s_wen : IN STD_LOGIC;
    s_ren : IN STD_LOGIC;
	s_oe_b : IN STD_LOGIC;
--	s_cmd_data : in std_logic;


	--AD mode sys state machine에 필요.
	s_AD_write_finish : in std_logic;


	  --address decoder 신호들
    s_reset_addr : IN STD_LOGIC;
    s_reset8254_addr : IN STD_LOGIC;
    s_pc_RAM_addr : IN STD_LOGIC;
    s_da_start_addr : IN STD_LOGIC;
    s_da_stop_addr : IN STD_LOGIC;
    s_ad_RAM_addr : IN STD_LOGIC;
	s_adr_RAM_addr : IN STD_LOGIC;
    s_opt_step1_addr : IN STD_LOGIC;
    s_opt_step2_addr : IN STD_LOGIC;
    s_opt_step3_addr : IN STD_LOGIC;
    
    s_PC_RAM_addr10 : IN std_logic_vector (10 downto 0);
    s_AD_RAM_addra10 : IN std_logic_vector (10 downto 0);
    s_AD_RAM_addrb10 : IN std_logic_vector (10 downto 0);
    s_Option_RAM_addra10 : IN std_logic_vector (10 downto 0);
    s_Option_RAM_addrb10 : IN std_logic_vector (10 downto 0);
	s_delayed_counter_address : in std_logic_vector(10 downto 0);
	
	s_adc_d : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	s_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	
	--signal controller 출력
	
	--RAM과 counter의 control 신호들
	  --PC RAM
	s_PC_RAM_cnt_reset_b : out std_logic;
	s_PC_RAM_cnt_ce : out std_logic;
	s_PC_RAM_ena : out std_logic;
	s_PC_RAM_wea : out std_logic_vector(0 downto 0);
	s_PC_RAM_enb : out std_logic;
	
	  --AD RAM B port
	s_AD_RAM_cntb_reset_b : out std_logic;
	s_AD_RAM_cntb_ce : out std_logic;
	s_AD_RAM_enb : out std_logic;
	
	  --Option RAM A port
	s_Option_RAM_cnta_reset_b : out std_logic;
	s_Option_RAM_cnta_ce : out std_logic;
	s_Option_RAM_ena : out std_logic;
	s_Option_RAM_wea : out std_logic_vector(0 downto 0);
	
	  --Option RAM B port
	s_Option_RAM_cntb_reset_b : out std_logic;
	s_Option_RAM_cntb_ce : out std_logic;
	s_Option_RAM_enb : out std_logic;
	
	--MUX control 신호들
    s_PC_mux_sel : OUT STD_LOGIC;
    s_DA_mux_sel : OUT STD_LOGIC;
    s_OUT_mux_sel : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
	
	--Latch control 신호들
	s_pc_latch_reset_b : out std_logic;
	s_in_latch_reset_b : out std_logic;
    s_IN_latch_en : OUT STD_LOGIC;
	s_out_latch_reset_b : out std_logic;
    s_OUT_latch_en : OUT STD_LOGIC;
	s_DA_latch_reset_b : out std_logic;
    s_DA_latch_en : OUT STD_LOGIC;
	s_AD_latch_reset_b : out std_logic;
    s_AD_latch_en : OUT STD_LOGIC;
    
    s_tri_en_b : OUT STD_LOGIC;
	s_PC_RAM_data_len : OUT STD_LOGIC_VECTOR(10 DOWNTO 0);
	s_AD_RAM_data_len : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
	s_Option_RAM_data_len : OUT STD_LOGIC_VECTOR(10 DOWNTO 0);
	
	--Filter control 신호들
	s_Filter_reset : out std_logic;
	s_Filter_en : out std_logic;

	--Option Write 용 counter의 신호들
	s_delayed_counter_reset_b : out std_logic;
	s_delayed_counter_cnt_en : out std_logic;
	
	--debug 확인용 신호들
	s_debug_led : out std_logic_vector(6 downto 0);
	s_debug_sw : out std_logic_vector(7 downto 0);
	s_debug_header : out std_logic_vector(15 downto 2);
	
	--AD mode에 쓰이는 sys_state_machine에 필요.
	s_sys_state_machine_on : out std_logic;
	s_AD_RAM_max_address : out std_logic_vector (10 downto 0)
	
  );
end Signal_Controller;

architecture Behavioral of Signal_Controller is
type state_t is (st_reset, st_idle, st_pc_ready, st_pc_w1, st_pc_w11, st_pc_w12, st_pc_w2, st_pc_w3, st_pc_r1, st_pc_r11, st_pc_r12, st_pc_r13, st_pc_r2, st_pc_r21, st_pc_r3, st_da_ready, st_da_ready2, st_da_ready3, st_da_start, st_da_stop, st_ad_ready, st_ad_AD_RAM_writing, st_ad_AD_RAM_write_done, st_ad_PC0, st_ad_PC1, st_ad_PC2, st_ad_PC3, st_ad_PC4, st_ad_PC_RAM_writing, st_ad_PC5, st_ad_PC6, st_ad_PC7, st_ad_wait, st_adr_ready, st_adr_r1, st_adr_r11, st_adr_r12, st_adr_r13, st_adr_r2, st_adr_r21, st_adr_r3, st_op1_ready, st_op1_ready2 ,trans0, trans1, trans2, trans3, trans4, trans5, trans6, trans7, trans8, trans9, trans10, trans11, trans12, st_op_write0, st_op_write1, st_op_write2, st_op_write3, st_op1_wait, st_op2_ready, st_op2_r1, st_op2_r11, st_op2_r12, st_op2_r13, st_op2_r2, st_op2_r21, st_op2_r3, st_op3_ready, st_op3_ready2, st_op3_ready3, st_op3_start );
signal current_state : state_t;
signal s_PC_RAM_len : STD_LOGIC_VECTOR(10 DOWNTO 0);
signal s_AD_RAM_len : STD_LOGIC_VECTOR(7 DOWNTO 0);
signal s_ADR_RAM_len : STD_LOGIC_VECTOR(10 DOWNTO 0);
signal s_Option_RAM_len : STD_LOGIC_VECTOR(10 DOWNTO 0);
signal s_s_AD_RAM_max_address : std_logic_vector(10 downto 0);



begin
	s_PC_RAM_cnt_reset_b <= '0' when (current_state = st_idle or current_state = st_pc_ready or current_state = st_da_ready or (current_state = st_da_start and s_PC_RAM_len = s_PC_RAM_addr10) or current_state = st_ad_ready or current_state = st_ad_AD_RAM_write_done or current_state = st_ad_wait or current_state = st_op1_ready or current_state = st_op1_wait) else '1';
	s_PC_RAM_cnt_ce <= '1' when (current_state = st_pc_w2 or current_state = st_pc_r2 or current_state = st_da_ready2 or current_state = st_da_ready3 or current_state = st_da_start or current_state = st_ad_PC_RAM_writing or current_state = st_ad_PC5 or current_state = trans0 or current_state = trans1 or current_state = trans2 or current_state = trans3 or current_state = trans4 or current_state = trans5 or current_state = trans6 or current_state = trans7 or current_state = trans8 or  current_state = trans9 or current_state = trans10 or current_state = trans11 or current_state = trans12 or current_state = st_op_write0 or current_state = st_op_write1 or current_state = st_op_write2 or current_state = st_op_write3) else '0';
	s_PC_RAM_ena <= '1' when (current_state = st_pc_w1 or current_state = st_pc_w1 or current_state = st_pc_w11 or current_state = st_pc_w12 or current_state = st_ad_PC0 or current_state = st_ad_PC1 or current_state = st_ad_PC2 or current_state = st_ad_PC3 or current_state = st_ad_PC4 or current_state = st_ad_PC_RAM_writing or current_state = st_ad_PC5 or current_state = st_ad_PC6 or current_state = st_ad_PC7) else '0';
    s_PC_RAM_wea <= "1" when ((current_state /= st_ad_ready and current_state /= st_ad_AD_RAM_writing and s_wen = '1') or current_state = st_ad_PC_RAM_writing or current_state = st_ad_PC5) else "0";
	s_PC_RAM_enb <= '1' when (current_state = st_pc_r1 or current_state = st_pc_r11 or current_state = st_pc_r12 or current_state = st_da_ready2 or current_state = st_da_ready3 or current_state = st_da_start or current_state = st_da_stop or current_state = trans0 or current_state = trans1 or current_state = trans2 or current_state = trans3 or current_state = trans4 or current_state = trans5 or current_state = trans6 or current_state = trans7 or current_state = trans8 or current_state = trans9 or current_state = trans10 or current_state = trans11 or current_state = trans12 or current_state = st_op_write0 or current_state = st_op_write1 or current_state = st_op_write2 or current_state = st_op_write3) else '0';
	
	
	s_AD_RAM_cntb_reset_b <= '0' when (current_state = st_idle or current_state = st_ad_AD_RAM_write_done or current_state = st_ad_PC5 or current_state = st_ad_PC6 or current_state = st_ad_PC7 or current_state = st_ad_wait or current_state = st_adr_ready) else '1';
	s_AD_RAM_cntb_ce <= '1' when (current_state = st_ad_PC3 or current_state = st_ad_PC4 or current_state = st_ad_PC_RAM_writing or current_state = st_adr_r2) else '0';
	s_AD_RAM_enb <= '1' when (current_state = st_ad_PC0 or current_state = st_ad_PC1 or current_state = st_ad_PC2 or current_state = st_ad_PC3 or current_state = st_ad_PC4 or current_state = st_ad_PC_RAM_writing or current_state = st_ad_PC5 or current_state = st_adr_r1 or current_state = st_adr_r11 or current_state = st_adr_r12) else '0';
		
	
	s_PC_mux_sel <= '1' when (current_state = st_ad_PC0 or current_state = st_ad_PC1 or current_state = st_ad_PC2 or current_state = st_ad_PC3 or current_state = st_ad_PC4 or current_state = st_ad_PC_RAM_writing or current_state = st_ad_PC5 or current_state = st_ad_PC6 or current_state = st_ad_PC7 or current_state = st_ad_wait) else '0';
    s_DA_mux_sel <= '1' when (current_state = st_da_start or current_state = st_da_stop) else '0';
    s_OUT_mux_sel <= "00" when (current_state = st_pc_r1) else
					"10" when (current_state = st_adr_ready or current_state = st_adr_r1 or current_state = st_adr_r11 or current_state = st_adr_r12 or current_state = st_adr_r13 or current_state = st_adr_r2) else
					"01" when (current_state = st_op2_ready or current_state = st_op2_r1 or current_state = st_op2_r11 or current_state = st_op2_r12 or current_state = st_op2_r13 or current_state = st_op2_r2) else
					"00";
					
	
	s_pc_latch_reset_b <= '0' when (current_state = st_reset) else '1';
	s_in_latch_reset_b <= '0' when (current_state = st_reset) else '1';
    s_IN_latch_en <= '1' when (current_state = st_pc_w1 or current_state = st_pc_w11 or current_state = st_pc_w12) else '0';
	s_out_latch_reset_b <= '0' when (current_state = st_reset) else '1';
    s_OUT_latch_en <= '1' when (current_state = st_pc_r1 or current_state = st_pc_r11 or current_state = st_pc_r12 or current_state = st_pc_r13 or current_state = st_pc_r2 or current_State = st_pc_r21 or current_state = st_adr_r1 or current_state = st_adr_r11 or current_state = st_adr_r12 or current_state = st_adr_r13 or current_state = st_adr_r2 or current_State = st_adr_r21 or current_state = st_op2_ready or current_state = st_op2_r1 or current_state = st_op2_r11 or current_state = st_op2_r12 or current_state = st_op2_r13 or current_state = st_op2_r2) else '0';
	s_DA_latch_reset_b <= '0' when (current_state = st_reset) else '1';
    s_DA_latch_en <= '1' when (current_state = st_da_start or current_state = st_op3_start) else '0';
	s_AD_latch_reset_b <= '0' when (current_state = st_reset) else '1';
    s_AD_latch_en <= '1' when (current_state = st_ad_ready or current_state = st_ad_AD_RAM_writing) else '0';
    
	s_tri_en_b <= '0' when (current_state = st_pc_r12 or current_state = st_pc_r13 or current_state = st_pc_r2 or current_state = st_adr_r12 or current_state = st_adr_r13 or current_state = st_adr_r2 or current_state = st_op2_r12 or current_state = st_op2_r13 or current_state = st_op2_r2) else '1';

	s_delayed_counter_reset_b <= '0' when (current_state = st_idle or current_state = st_op1_ready or current_state = st_op1_wait) else '1';
	s_delayed_counter_cnt_en <= '1' when (current_state = st_op_write0 or current_state = st_op_write1 or current_state = st_op_write2 or current_state = st_op_write3) else'0';
	
	s_Filter_reset <= '1' when (current_state = st_idle or current_state = st_op1_ready or current_state = st_op1_wait) else '0';
	s_Filter_en <= '1' when ( current_state = trans2 or current_state = trans3 or current_state = trans4 or current_state = trans5 or current_state = trans6 or current_state = trans7 or current_state = trans8 or current_state = trans9 or current_state = trans10 or current_state = trans11 or current_state = trans12 or current_state = st_op_write0 or current_state = st_op_write1 or current_state = st_op_write2 or current_state = st_op_write3 or current_state = st_op1_wait) else '0';
	
	s_Option_RAM_cnta_reset_b <= '0' when (current_state = st_idle or current_state = st_op1_ready or current_state = st_op1_wait) else '1'; 
	s_Option_RAM_cnta_ce <= '1' when (current_state = st_op_write3) else '0';
	s_Option_RAM_ena <= '1' when (current_state = st_op1_ready2 or current_state = trans0 or current_state = trans1 or current_state = trans2 or current_state = trans3 or current_state = trans4 or current_state = trans5 or current_state = trans6 or current_state = trans7 or current_state = trans8 or  current_state = trans9 or current_state = trans10 or current_state = trans11 or current_state = trans12 or current_state = st_op_write0 or current_state = st_op_write1 or current_state = st_op_write2 or current_state = st_op_write3) else '0';
	s_Option_RAM_wea <= "1" when (current_state = st_op_write3) else "0";
	
	s_Option_RAM_cntb_reset_b <= '0' when (current_state = st_idle or current_state = st_op2_ready or current_state = st_op3_ready or (current_state = st_op3_start and s_Option_RAM_len = s_Option_RAM_addrb10)) else '1';
	s_Option_RAM_cntb_ce <= '1' when (current_state = st_op2_r2 or current_state = st_op3_ready2 or current_state = st_op3_ready3 or current_state = st_op3_start) else '0';
	s_Option_RAM_enb <= '1' when (current_state = st_op2_r1 or current_state = st_op2_r11 or current_state = st_op2_r12 or current_state = st_op3_ready2 or current_state = st_op3_ready3 or current_state = st_op3_start  ) else '0';
	
	
	s_sys_state_machine_on <= '1' when (current_state = st_ad_AD_RAM_writing) else '0';
	s_AD_RAM_max_address <= s_s_AD_RAM_max_address;

		
	contro_proc: process(s_clk)
	begin
		if rising_edge(s_clk) then
		case current_state is
			when st_reset =>
				current_state <= st_idle;
				
			when st_idle =>
				if(s_pc_RAM_addr = '1') then
					current_state <= st_pc_ready;
				elsif(s_da_start_addr = '1') then
					current_state <= st_da_ready;
				elsif(s_da_stop_addr = '1' ) then
					current_state <= st_da_stop;
				elsif(s_ad_RAM_addr = '1') then
					current_state <= st_ad_ready;
				elsif(s_adr_RAM_addr = '1' ) then
					current_state <= st_adr_ready;
				elsif(s_opt_step1_addr = '1' ) then
					current_state <= st_op1_ready;
				elsif(s_opt_step2_addr = '1' ) then
					current_state <= st_op2_ready;
				elsif(s_opt_step3_addr = '1') then
					current_state <= st_op3_ready;
				else
					current_state <= st_idle;
				end if;
			
			when st_pc_ready =>
				if(s_oe_b = '1' and s_pc_RAM_addr = '1') then
					current_state <= st_pc_w1;
				elsif(s_oe_b = '0' and s_pc_RAM_addr = '1') then
					current_state <= st_pc_r1;
				else
					current_state <= st_reset;
				end if;
			
			when st_pc_w1 =>
				if (s_reset_addr = '1' ) then
					current_state <= st_idle;
				elsif(s_wen = '1') then
					current_state <= st_pc_w11;
				else
					current_state <= st_pc_w1;
				end if;
				
			when st_pc_w11 =>
				current_state <= st_pc_w12;
				
			when st_pc_w12 =>
				if (s_wen = '1') then
					current_state <= st_pc_w12;
				else
					current_state <= st_pc_w2;
				end if;
			
			when st_pc_w2 =>
				current_state <= st_pc_w3;
				
			when st_pc_w3 =>
				if(s_oe_b = '1' and s_pc_RAM_addr = '1') then
					current_state <= st_pc_w1;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_pc_w3;
				else
					current_state <= st_idle;
					s_PC_RAM_data_len <= s_PC_RAM_addr10;
					s_PC_RAM_len <= s_PC_RAM_addr10 - "00000000001";
				end if;
			
			when st_pc_r1 =>
				if(s_ren = '1') then
					current_state <= st_pc_r11;
				else
					current_state <= st_pc_r1;	
				end if;		
				
			when st_pc_r11 =>
				current_state <= st_pc_r12;
				
			when st_pc_r12 =>
				if (s_ren = '0') then
					current_state <= st_pc_r13;
				else
					current_state <= st_pc_r12;
				end if;
				
			when st_pc_r13 =>
					current_state <= st_pc_r2;
			
			when st_pc_r2 =>
				current_state <= st_pc_r21;
				
			when st_pc_r21 =>
				current_state <= st_pc_r3;
				
			when st_pc_r3 =>
				if(s_oe_b = '0' and s_pc_RAM_addr = '1') then
					current_state <= st_pc_r1;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_pc_r3;
				else
					current_state <= st_idle;		
				end if;			
				
			when st_da_ready =>
				if(s_reset_addr = '1' ) then
					current_state <= st_idle;
				elsif(s_da_start_addr = '0' ) then
					current_state <= st_da_ready2;
				else
					current_state <= st_da_ready;
				end if;

				
			when st_da_ready2 =>
					current_state <= st_da_ready3;
					
			when st_da_ready3 =>
					current_state <= st_da_start;
				
			
			when st_da_start =>
				if(s_da_stop_addr = '1' ) then
					current_state <= st_da_stop;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_da_start;
				else
					current_state <= st_idle;
				end if;

				
			when st_da_stop => 
				if(s_da_start_addr = '1' ) then 
					current_state <= st_da_ready;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_da_stop;
				else
					current_state <= st_idle;
				end if;

				
			when st_ad_ready =>
				current_state <= st_ad_AD_RAM_writing;
				s_s_AD_RAM_max_address <= s_data - "00000000001";
			
			when st_ad_AD_RAM_writing =>
				if (s_AD_write_finish = '1') then
					current_state <= st_ad_AD_RAM_write_done;
				else
					current_state <= st_ad_AD_RAM_writing;
				end if;
			
			when st_ad_AD_RAM_write_done =>
				if (s_AD_write_finish = '0') then
					current_state <= st_ad_PC0;
				else
					current_state <= st_ad_AD_RAM_write_done;
				end if;
				
			when st_ad_PC0 =>
				current_state <= st_ad_PC1;
			
			when st_ad_PC1 =>
				current_state <= st_ad_PC2;
				
			when st_ad_PC2 =>
				current_state <= st_ad_PC3;	
				
			when st_ad_PC3 =>
				current_state <= st_ad_PC4;
			
			when st_ad_PC4 =>
				current_state <= st_ad_PC_RAM_writing;
				
			when st_ad_PC_RAM_writing =>
				if(s_AD_RAM_addrb10 = s_s_AD_RAM_max_address) then
					current_state <= st_ad_PC5;
				else
					current_state <= st_ad_PC_RAM_writing;
				end if;
				
			when st_ad_PC5 =>
				if(s_PC_RAM_addr10 = s_s_AD_RAM_max_address) then
					current_state <= st_ad_pc6;
					s_PC_RAM_len <= s_PC_RAM_addr10; --최종 번지 수 기록
				else
					current_state <= st_ad_PC5;
				end if;
			
			when st_ad_PC6 =>
				current_state <= st_ad_PC7;
				
			when st_ad_PC7 =>
				current_state <= st_ad_wait;
			
			when st_ad_wait =>
				if(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_ad_wait;
				else
					current_state <= st_idle;
				end if;

				
			when st_adr_ready =>
				if(s_oe_b = '0' and s_adr_RAM_addr = '1') then
					current_state <= st_adr_r1;
				else
					current_state <= st_reset;
				end if;
			
			when st_adr_r1 =>
				if(s_ren = '1') then
					current_state <= st_adr_r11;
				else
					current_state <= st_adr_r1;	
				end if;		
				
			when st_adr_r11 =>
				current_state <= st_adr_r12;
				
			when st_adr_r12 =>
				if (s_ren = '0') then
					current_state <= st_adr_r13;
				else
					current_state <= st_adr_r12;
				end if;	
				
			when st_adr_r13 =>
					current_state <= st_adr_r2;
			
			when st_adr_r2 =>
				current_state <= st_adr_r21;
				
			when st_adr_r21 =>
				current_state <= st_adr_r3;
				
			when st_adr_r3 =>
				if(s_oe_b = '0' and s_adr_RAM_addr = '1') then
					current_state <= st_adr_r1;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_adr_r3;
				else
					current_state <= st_idle;		
				end if;
			
			when st_op1_ready =>
				if (s_wen = '1') then
					current_state <= st_op1_ready2;
				else
					current_state <= st_op1_ready;
				end if;
			
			when st_op1_ready2 =>
				if (s_wen = '0') then
					current_state <= trans0;
				else
					current_state <= st_op1_ready2;
				end if;
			
			when trans0 =>
					current_state <= trans1;
			
			when trans1 =>
					current_state <= trans2;
						
			when trans2 =>
					current_state <= trans3;
			
			when trans3 =>
					current_state <= trans4;
			
			when trans4 =>
					current_state <= trans5;
			
			when trans5 =>
					current_state <= trans6;
			
			when trans6 =>
					current_state <= trans7;
					
			when trans7 =>
					current_state <= trans8;	

			when trans8 =>
					current_state <= trans9;
			
			when trans9 =>
					current_state <= trans10;
					
			when trans10 =>
					current_state <= trans11;
					
			when trans11 =>
					current_state <= trans12;
					
			when trans12 =>
					current_state <= st_op_write0;
					
			when st_op_write0 =>
				if (s_delayed_counter_address = s_PC_RAM_len) then
					current_state <= st_op1_wait;
					s_Option_RAM_len <= s_Option_RAM_addra10; --Option RAM data 수 저장.
				else
					current_state <= st_op_write1;
				end if;
			
			when st_op_write1 =>
				if (s_delayed_counter_address = s_PC_RAM_len) then
					current_state <= st_op1_wait;
					s_Option_RAM_len <= s_Option_RAM_addra10; --Option RAM data 수 저장.
				else
					current_state <= st_op_write2;
				end if;
			
			when st_op_write2 =>
				if (s_delayed_counter_address = s_PC_RAM_len) then
					current_state <= st_op1_wait;
					s_Option_RAM_len <= s_Option_RAM_addra10; --Option RAM data 수 저장.
				else
					current_state <= st_op_write3;
				end if;
			
			when st_op_write3 =>
				if (s_delayed_counter_address = s_PC_RAM_len) then
					current_state <= st_op1_wait;
					s_Option_RAM_len <= s_Option_RAM_addra10; --Option RAM data 수 저장.
				else
					current_state <= st_op_write0;
				end if;
				
			when st_op1_wait =>
				if(s_opt_step1_addr = '1' ) then
					current_state <= st_op1_ready;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0'  and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_op1_wait;
				else
					current_state <= st_idle;
				end if;
					
			when st_op2_ready =>
				if(s_oe_b = '0' and s_opt_step2_addr = '1') then
					current_state <= st_op2_r1;
				else
					current_state <= st_reset;
				end if;
			
			when st_op2_r1 =>
				if(s_ren = '1') then
					current_state <= st_op2_r11;
				else
					current_state <= st_op2_r1;	
				end if;	

			when st_op2_r11 =>
				current_state <= st_op2_r12;
				
			when st_op2_r12 =>
				if (s_ren = '0') then
					current_state <= st_op2_r13;
				else
					current_state <= st_op2_r12;
				end if;	
				
			when st_op2_r13 =>
					current_state <= st_op2_r2;
			
			when st_op2_r2 =>
				current_state <= st_op2_r21;
				
			when st_op2_r21 =>
				current_state <= st_op2_r3;
				
			when st_op2_r3 =>
				if(s_oe_b = '0' and s_opt_step2_addr = '1') then
					current_state <= st_op2_r1;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_op2_r3;
				else
					current_state <= st_idle;		
				end if;		
				
			when st_op3_ready =>
				if(s_opt_step3_addr = '0' ) then
					current_state <= st_op3_ready2;
				else
					current_state <= st_op3_ready;
				end if;
				
			when st_op3_ready2 =>
					current_state <= st_op3_ready3;
				
			when st_op3_ready3 =>
					current_state <= st_op3_start;
			
			when st_op3_start =>
				if(s_da_stop_addr = '1' ) then
					current_state <= st_da_stop;
				elsif(s_reset_addr = '0' and s_reset8254_addr = '0' and s_pc_RAM_addr = '0' and s_da_start_addr = '0' and s_da_stop_addr = '0' and s_ad_RAM_addr = '0' and s_adr_RAM_addr = '0' and s_opt_step1_addr = '0' and s_opt_step2_addr = '0' and s_opt_step3_addr = '0') then
					current_state <= st_op3_start;
				else
					current_state <= st_idle;
				end if;
			
						
			when others =>
				current_state <= st_reset;
		end case;
		end if;
	end process;
	
	--debug 확인용

	s_debug_led(1) <= '1' when (current_state = st_pc_ready or current_state = st_da_ready or current_state = st_ad_ready or current_state = st_adr_ready or current_state = st_op1_ready or current_state = st_op2_ready or current_state = st_op3_ready) else '0';
	s_debug_led(2) <= '1' when (current_state = st_pc_w1 or current_state = st_pc_r1 or current_state = st_da_ready2 or current_state = st_ad_AD_RAM_writing or current_state = st_adr_r1 or current_state = st_op1_ready2 or current_state = st_op2_r1 or current_state = st_op3_ready2) else '0';
	s_debug_led(3) <= '1' when (current_state = st_pc_w11 or current_state = st_pc_r11 or current_state = st_da_ready3 or current_state = st_ad_AD_RAM_write_done or current_state = st_adr_r11 or current_state = trans0 or current_state = st_op2_r11 or current_state = st_op3_ready3) else '0';
	s_debug_led(4) <= '1' when (current_state = st_pc_w12 or current_state = st_pc_r12 or current_state = st_da_start or current_state = st_ad_PC0 or current_state = st_adr_r12 or current_state = trans12 or current_state = st_op2_r12 or current_state = st_op3_start) else '0';
	s_debug_led(5) <= '1' when (current_state = st_pc_w2 or current_state = st_pc_r13 or current_state = st_da_stop or current_state = st_ad_PC1 or current_state = st_adr_r13 or current_state = st_op_write0 or current_state = st_op2_r13) else '0';
	s_debug_led(6) <= '1' when (current_state = st_pc_w3 or current_state = st_pc_r2 or current_state = st_ad_PC2 or current_state = st_adr_r2 or current_state = st_op_write1 or current_state = st_op2_r2) else '0';
	s_debug_header(2) <= '1' when (current_state = st_pc_r21 or current_state = st_ad_PC3 or current_state = st_adr_r21 or current_state = st_op_write2 or current_state = st_op2_r21) else '0';
	s_debug_header(3) <= '1' when (current_state = st_pc_r3 or current_state = st_ad_PC4 or current_state = st_adr_r3 or current_state = st_op_write3 or current_state = st_op2_r3) else '0';
	s_debug_header(4) <= '1' when (current_state = st_ad_PC_RAM_writing or current_state = st_op1_wait) else '0';
	s_debug_header(5) <= '1' when (current_state = st_ad_PC5) else '0';
	s_debug_header(6) <= '1' when (current_state = st_ad_wait) else '0';
	s_debug_header(15) <= '1' when (current_state = st_idle) else '0';
	
end Behavioral;
