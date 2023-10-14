----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 2023/02/13 19:08:41
-- Design Name: 
-- Module Name: pcfg_top - Behavioral
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
Library UNISIM;
use UNISIM.vcomponents.all;
--전체 top file의 Input, output 정의 부분은 절대 건드리지 말 것!
entity PCFG_TOP is
PORT( ---------------------------------------------INPUT
	 	m_fpga_clk                  : in std_logic; --FPGA 내부 clock 40MHz
        m_fpga_reset                : in std_logic;                     -- active low
        m_adc_d                     : in std_logic_vector (7 downto 0); --AD mode에서 function generator 값을 ADC를 통해 AD_RAM에 저장할 때의 8 bit data.
        m_address                   : IN std_logic_vector(8 downto 0);	--9 bit address
		m_cmd_data                  : IN std_logic;
		m_oe_b                      : IN std_logic; --m_oe_b 신호는 m_ren과 비슷한 역할을 하는 신호. Active low, Read cycle의 10clock 동안 '0'이 되어 PC쪽으로 데이터를 출력하라고 알려주는 신호라고 보시면 됩니다.
		m_wen                       : IN std_logic;
		m_ren                       : IN std_logic;
		
		---------------------------------------------OUTPUT	
		m_dac_d                     : out std_logic_vector (7 downto 0); 
        m_dac_gain_d                : out std_logic_vector (7 downto 0);
        m_adc_clk                   : out std_logic;
        m_dac_clk                   : out std_logic;
        m_dac_gain_clk              : out std_logic;
        
		-------------------------------------------------INOUT
		m_data                      : INOUT std_logic_vector(7 downto 0); --PC에서 들어오거나 나가는 data (8 bit)
		
        -------------------------------------------------debugging
		m_debug_header              : out std_logic_vector (15 downto 0);
        m_debug_sw                  : in std_logic_vector (7 downto 0);
        m_debug_led                 : out std_logic_vector (7 downto 0)
        
		);
end PCFG_TOP;
--전체 top file의 Input, output 정의 부분은 절대 건드리지 말 것!

architecture Behavioral of PCFG_TOP is


---=========== COMPONENT ===================
--8254 device
component TOP_8254 is
    Port ( m_clk0    : in  STD_LOGIC;
           m_clk1    : in  STD_LOGIC;
           m_clk2    : in  STD_LOGIC;
           m_clk_ctr : in  STD_LOGIC;
           m_reset   : in STD_LOGIC;
           m_data    : in  STD_LOGIC_VECTOR (7 downto 0);
           m_gate0   : in  STD_LOGIC;
           m_gate1   : in  STD_LOGIC;
           m_gate2   : in  STD_LOGIC;
           m_addr    : in  STD_LOGIC_VECTOR (1 downto 0);
           m_cs_b    : in  STD_LOGIC;
           m_wr_b    : in  STD_LOGIC;
          
		   m_out0    : out  STD_LOGIC;
           m_out1    : out  STD_LOGIC;
           m_out2    : out  STD_LOGIC);
end component;


--PC latch
component PC_latch is
  Port (
	reset_b : in std_logic;
	s_clk		: in std_logic;
	m_cmd_data 	: in std_logic;
	m_wen 		: in std_logic;
	m_ren 		: in std_logic;
	m_oe_b	 	: in std_logic;
	m_address 	: in std_logic_vector (8 downto 0);
	s_cmd_data	: out std_logic;
	s_wen 		: out std_logic;
	s_ren 		: out std_logic;
	s_oe_b 		: out std_logic;
	s_address 	: out std_logic_vector (8 downto 0)
	);
end component;


--pc latch 이후의 address decoder.
component address_decoder is
  Port (
	s_address 		: in std_logic_vector(8 downto 0); -- PC_latch를 통과한 USB의 9 bit address signal
	s_cmd_data		: in std_logic;
	pcs_addr_b		: out std_logic; --180H~183H System clock setting(8524 setting)  
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
	--pcs_addr은 8254 chip select용 signal. 나머지는 signal controller 내에 필요한 signal.
end component;

--RAM 갯수는 총 3개. PC_RAM, Option_RAM, AD_RAM.
--RAM 정보
--A port는 write 용, B port는 read 용.
--Data size 8 bit, Address는 11 bit.
component PC_RAM IS
  PORT (
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(10 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    clkb : IN STD_LOGIC;
    enb : IN STD_LOGIC;
    addrb : IN STD_LOGIC_VECTOR(10 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

component AD_RAM IS
  PORT (
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(10 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    clkb : IN STD_LOGIC;
    enb : IN STD_LOGIC;
    addrb : IN STD_LOGIC_VECTOR(10 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

component Option_RAM IS
  PORT (
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(10 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    clkb : IN STD_LOGIC;
    enb : IN STD_LOGIC;
    addrb : IN STD_LOGIC_VECTOR(10 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--tribuffer
component tribuffer IS
  PORT (
    tri_en_b : IN STD_LOGIC;
    Data_in : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Data_out : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    Data_inout : INOUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--IN_latch
component IN_latch IS
  PORT ( 
	reset_b : in std_logic;
    s_clk : IN STD_LOGIC;
    en : IN STD_LOGIC;
    Din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--OUT_latch
component OUT_latch IS
  PORT ( 
	reset_b : in std_logic;
    sys_clk : IN STD_LOGIC;
    en : IN STD_LOGIC;
    Din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--DA_latch
component DA_latch IS
  PORT ( 
	reset_b : in std_logic;
    sys_clk : IN STD_LOGIC;
    en : IN STD_LOGIC;
    Din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--AD_latch
component AD_latch IS
  PORT ( 
	reset_b : in std_logic;
    sys_clk : IN STD_LOGIC;
    en : IN STD_LOGIC;
    Din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--OUT_mux
component OUT_mux IS
  PORT ( 
    sel : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    Din0 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din1 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din2 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--PC_mux
component PC_mux IS
  PORT ( 
    sel : IN STD_LOGIC;
    Din0 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din1 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--DA_mux
component DA_mux IS
  PORT ( 
    sel : IN STD_LOGIC;
    Din0 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Din1 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    Dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--PC RAM address counter (Read/Write 겸용)
component PC_RAM_addr_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end component;

--AD RAM address counter (Port A, write)
component AD_RAM_addra_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end component;

--AD RAM address counter (Port B, Read)
component AD_RAM_addrb_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end component;

--Option RAM address counter (Port A, Write)
component Option_RAM_addra_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end component;

--Option RAM address counter (Port B, Read)
component Option_RAM_addrb_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end component;

--Filter
component Filter IS
  PORT (
  	m_reset : IN STD_LOGIC;
  	m_clk : IN STD_LOGIC;
  	m_en : IN STD_LOGIC;
  	m_xin : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
  	m_yout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;

--Option RAM write 용
component PC_RAM_delayed_counter is
  Port (
	clk 			: in std_logic;
	reset_b 		: in std_logic; --active low
	cnt_en 			: in std_logic;
	cnt_out 		: out std_logic_vector(10 downto 0)
	);
end component;

--Signal_Controller
component Signal_Controller is
  Port ( 
	m_reset_b : in std_logic;
    s_clk : IN STD_LOGIC;
	sys_clk : IN STD_LOGIC;
    s_wen : IN STD_LOGIC;
    s_ren : IN STD_LOGIC;
	s_oe_b : in std_logic;
	
	s_AD_write_finish : in std_logic;

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
	
	s_PC_RAM_cnt_reset_b : out std_logic;
	s_PC_RAM_cnt_ce : out std_logic;
	s_PC_RAM_ena : out std_logic;
	s_PC_RAM_wea : out std_logic_vector(0 downto 0);
	s_PC_RAM_enb : out std_logic;
	
	s_AD_RAM_cntb_reset_b : out std_logic;
	s_AD_RAM_cntb_ce : out std_logic;
	s_AD_RAM_enb : out std_logic;
	
	s_Option_RAM_cnta_reset_b : out std_logic;
	s_Option_RAM_cnta_ce : out std_logic;
	s_Option_RAM_ena : out std_logic;
	s_Option_RAM_wea : out std_logic_vector(0 downto 0);
	
	s_Option_RAM_cntb_reset_b : out std_logic;
	s_Option_RAM_cntb_ce : out std_logic;
	s_Option_RAM_enb : out std_logic;
	
    s_PC_mux_sel : OUT STD_LOGIC;
	s_DA_mux_sel : OUT STD_LOGIC;
    s_OUT_mux_sel : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
	
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
	
	s_Filter_reset : out std_logic;
	s_Filter_en : out std_logic;
	
	s_delayed_counter_reset_b : out std_logic;
	s_delayed_counter_cnt_en : out std_logic;
	
	s_adc_d : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	s_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
	
	s_debug_led : out std_logic_vector(6 downto 0);
	s_debug_sw : out std_logic_vector(7 downto 0);
	s_debug_header : out std_logic_vector(15 downto 2);
	
	s_sys_state_machine_on : out std_logic;
	s_AD_RAM_max_address : out std_logic_vector(10 downto 0)
	
 );
end component;

component sys_state_machine is
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
end component;

--==============================================================================================================
--==============================================================================================================

signal s_clk : std_logic;


--=== signals (architecture body에서 선언하는 중간 신호들)

---8254
signal s_m_8254_gate0				: std_logic; 
signal s_m_8254_gate1				: std_logic; 
signal s_m_8254_gate2				: std_logic; 

signal sys_clk : std_logic;
signal s_reset_b : std_logic;

signal s_debug_led : std_logic_vector(6 downto 0);
signal s_debug_sw     : std_logic_vector (7 downto 0);
signal s_debug_header : std_logic_vector (15 downto 2);

--PC latch mapping에 필요.
signal s_pc_latch_reset_b : std_logic;
signal s_cmd_data	: std_logic; --FPGA 내부로 들어오는 신호
signal s_wen 		: std_logic;--FPGA 내부로 들어오는 신호
signal s_ren 		: std_logic;--FPGA 내부로 들어오는 신호
signal s_oe_b 		: std_logic; --FPGA 내부로 들어오는 신호
signal s_address 	: std_logic_vector (8 downto 0); --FPGA 내부로 들어오는 신호

--address decoder mapping에 필요. -- 석훈 siganl은 다 s붙임
signal s_pcs_addr_b		: std_logic; --180H~183H System clock setting(8524 setting)  
signal s_reset_addr 	: std_logic; --170H software reset (모드 동작 중에도 reset)
signal s_reset8254_addr : std_logic; --171H 8254 reset
signal s_pc_RAM_addr 	: std_logic; --160H PC mode. wen, ren signal에 따라 PC_RAM에 쓰기, 읽기.
signal s_da_start_addr	: std_logic; --150H PC_RAM의 데이터를 DAC로 출력함 (DA mode start)
signal s_da_stop_addr	: std_logic; --151H 위의 DA mode DAC출력을 멈춤.
signal s_ad_RAM_addr	: std_logic; --140H PC에서 요청한 데이터 갯수만큼 ADC하여, AD_RAM을 통하여 PC_RAM으로 옮기기.
signal s_adr_RAM_addr	: std_logic; --141H ren signal에 따라 AD_RAM에 있는 데이터 읽기.
signal s_opt_step1_addr	: std_logic; --130H 
signal s_opt_step2_addr	: std_logic; --131H
signal s_opt_step3_addr	: std_logic; --132H

--PC_RAM
signal s_PC_RAM_Dout : std_logic_vector (7 downto 0);
signal s_PC_RAM_ena : std_logic;
signal s_PC_RAM_wea : std_logic_vector (0 DOWNTO 0);
signal s_PC_RAM_enb : std_logic;

--AD_RAM
signal s_AD_RAM_Dout : std_logic_vector (7 downto 0);
signal s_AD_RAM_ena : std_logic;
signal s_AD_RAM_wea : std_logic_vector (0 DOWNTO 0);
signal s_AD_RAM_enb : std_logic;

--Option_RAM
signal s_Option_RAM_Dout : std_logic_vector (7 downto 0);
signal s_Option_RAM_ena : std_logic;
signal s_Option_RAM_wea : std_logic_vector (0 DOWNTO 0);
signal s_Option_RAM_enb : std_logic;

--tri state buffer
signal s_tri_out : std_logic_vector (7 downto 0);
signal s_tri_en_b : std_logic;

--IN_latch
signal s_in_latch_reset_b : std_logic;
signal s_IN_latch_Dout : std_logic_vector (7 downto 0);

--OUT_latch
signal s_out_latch_reset_b : std_logic;
signal s_OUT_latch_Dout : std_logic_vector (7 downto 0);

--DA_latch
signal s_DA_latch_reset_b : std_logic;
--signal s_DA_latch_Dout : std_logic_vector (7 downto 0);

--AD_latch
signal s_AD_latch_reset_b : std_logic;
signal s_AD_latch_Dout : std_logic_vector (7 downto 0);

--OUT_mux
signal s_OUT_mux_Dout : std_logic_vector (7 downto 0);

--PC_mux
signal s_PC_mux_Dout : std_logic_vector (7 downto 0);

--DA_mux
signal s_DA_mux_Dout : std_logic_vector (7 downto 0);

--PC RAM Address Counter
signal s_PC_RAM_addr10 : std_logic_vector (10 downto 0);

--AD RAM address counter (A, write)
signal s_AD_RAM_addra10 : std_logic_vector (10 downto 0); -- s_AD_RAM_addr10 -> s_AD_RAM_addra10 과 s_AD_RAM_addrb10으로 따로 선언하여 port map

--AD RAM address counter (B, Read)
signal s_AD_RAM_addrb10 : std_logic_vector (10 downto 0); 

--Option RAM address counter (A, write)
signal s_Option_RAM_addra10 : std_logic_vector(10 downto 0);--s_Option_RAM_addr10 -> s_Option_RAM_addra10 과 s_Option_RAM_addrb10으로 따로 선언하여 port map

--Option RAM address counter (B, read)
signal s_Option_RAM_addrb10 : std_logic_vector(10 downto 0);

--Filter   : 
signal s_Filter_Dout : std_logic_vector (7 downto 0);
signal s_Filter_reset : std_logic;
signal s_Filter_en : std_logic;

--Option Write 용 counter
signal s_delayed_counter_reset_b : std_logic;
signal s_delayed_counter_cnt_en : std_logic;
signal s_delayed_counter_address : std_logic_vector(10 downto 0);

--Signal_Controller
--signal controller에서 s_PC_RAM_addr10, s_AD_RAM_addr10, s_Option_RAM_addr10 을 output으로 하지 않고, 각 counter의 reset_b, ce, load_b 신호를 output으로 하면 됨.

signal s_PC_RAM_cnt_reset_b : std_logic;
signal s_PC_RAM_cnt_ce : std_logic; 


signal s_AD_RAM_cnta_reset_b : std_logic;
signal s_AD_RAM_cnta_ce : std_logic; 

signal s_AD_RAM_cntb_reset_b : std_logic;
signal s_AD_RAM_cntb_ce : std_logic; 


signal s_Option_RAM_cnta_reset_b : std_logic;
signal s_Option_RAM_cnta_ce : std_logic; 

signal s_Option_RAM_cntb_reset_b : std_logic;
signal s_Option_RAM_cntb_ce : std_logic; 

signal s_IN_latch_en : STD_LOGIC;
signal s_OUT_latch_en : STD_LOGIC;
signal s_DA_latch_en : STD_LOGIC;
signal s_AD_latch_en : STD_LOGIC;

signal s_PC_mux_sel : STD_LOGIC;
signal s_OUT_mux_sel : STD_LOGIC_VECTOR(1 DOWNTO 0);
signal s_DA_mux_sel : STD_LOGIC;

--Additional
signal s_PC_RAM_data_len : STD_LOGIC_VECTOR(10 DOWNTO 0);
signal s_AD_RAM_data_len : STD_LOGIC_VECTOR(7 DOWNTO 0);
signal s_Option_RAM_data_len : STD_LOGIC_VECTOR(10 DOWNTO 0);
signal s_AD_RAM_max_address : std_logic_Vector(10 downto 0);

--sys state machine에 필요
signal s_sys_state_machine_on : std_logic;
signal s_AD_write_finish : std_logic;

--==============================================================================================================



                                                -- Code Input --


--==============================================================================================================

begin

s_reset_b <= m_fpga_reset;


--global iobuf
s_clk_g : IBUFG generic map (IOSTANDARD => "LVCMOS_33")
port map(I=>m_fpga_clk, O=>s_clk);

m_dac_clk   <= s_clk;    -- dac clk
m_adc_clk   <= sys_clk;    -- adc clk

--8254 port map
clk_gen : TOP_8254 port map( 
           m_clk0    => s_clk,
           m_clk1    => s_clk,
           m_clk2    => s_clk,
           m_clk_ctr => s_clk,
           m_reset   => (not s_reset_b) OR s_reset8254_addr, 
           m_data    => m_data,
           m_gate0   => s_m_8254_gate0,
           m_gate1   => s_m_8254_gate1,
           m_gate2   => s_m_8254_gate2,
           m_addr    => m_address(1 downto 0),
           m_cs_b    => s_pcs_addr_b,		
           m_wr_b    => not s_wen,
		   m_out0    => sys_clk,
           m_out1    => open,
           m_out2    => open
		   );
		   
s_m_8254_gate0	<= '1';
s_m_8254_gate1	<= '1';
s_m_8254_gate2	<= '1';		   

--PC latch port map   
PC_latch_port_map : PC_latch port map(
	reset_b => s_pc_latch_reset_b,
	s_clk 		=> s_clk,		
	m_cmd_data	=> m_cmd_data,
	m_wen 		=> m_wen,
	m_ren 		=> m_ren,
	m_oe_b	 	=> m_oe_b,
	m_address 	=> m_address,
	s_cmd_data	=> s_cmd_data,
	s_wen 		=> s_wen,
	s_ren 		=> s_ren,
	s_oe_b 		=> s_oe_b,
	s_address 	=> s_address
	);
	
--address decoder port map
address_decoder_port_map : address_decoder port map( 
	s_address 		=> s_address,
	s_cmd_data		=> s_cmd_data,
	pcs_addr_b 		=> s_pcs_addr_b, --active low 8254 chip select signal
	reset_addr 		=> s_reset_addr, 
	reset8254_addr 	=> s_reset8254_addr,
	pc_RAM_addr 	=> s_pc_RAM_addr,
	da_start_addr	=> s_da_start_addr,
	da_stop_addr	=> s_da_stop_addr,
	ad_RAM_addr		=> s_ad_RAM_addr, 
	adr_RAM_addr	=> s_adr_RAM_addr, 
	opt_step1_addr	=> s_opt_step1_addr, 
	opt_step2_addr	=> s_opt_step2_addr,
	opt_step3_addr	=> s_opt_step3_addr
	);
	
--RAM 3개 port map
PC_RAM_port_map : PC_RAM port map(
    clka => s_clk,  --A port: write
    ena => s_PC_RAM_ena,
    wea => s_PC_RAM_wea,
    addra => s_PC_RAM_addr10,
    dina => s_PC_mux_Dout,
    clkb => s_clk,  --B port: read
    enb => s_PC_RAM_enb,
    addrb => s_PC_RAM_addr10,
    doutb => s_PC_RAM_Dout
    );
    
AD_RAM_port_map : AD_RAM port map(
	clka => sys_clk, --A port: write
    ena => s_AD_RAM_ena,
    wea => s_AD_RAM_wea,
    addra => s_AD_RAM_addra10,
    dina => s_AD_latch_Dout,
    clkb => s_clk, --B port: read
    enb => s_AD_RAM_enb,
    addrb => s_AD_RAM_addrb10,
    doutb => s_AD_RAM_Dout
    );
    
Option_RAM_port_map : Option_RAM port map(
	clka => s_clk, --A port: write
    ena => s_Option_RAM_ena,
    wea => s_Option_RAM_wea,
    addra => s_Option_RAM_addra10,
    dina => s_Filter_Dout,
    clkb => sys_clk, --B port: read
    enb => s_Option_RAM_enb,
    addrb => s_Option_RAM_addrb10,
    doutb => s_Option_RAM_Dout
    );

--tribuffer port map
tribuffer_port_map : tribuffer port map(
	tri_en_b   => s_tri_en_b,
	Data_in    => s_OUT_latch_Dout,
	Data_out   => s_tri_out,
	Data_inout => m_data
    );
	
--나머지 latch 4개 port map
IN_latch_port_map : IN_latch port map(
	reset_b => s_in_latch_reset_b,
	s_clk => s_clk,
	en => s_IN_latch_en,
    Din => s_tri_out,
    Dout => s_IN_latch_Dout
    );

OUT_latch_port_map : OUT_latch port map(
	reset_b => s_out_latch_reset_b,
	sys_clk => sys_clk,
	en => s_OUT_latch_en,
    Din => s_OUT_mux_Dout,
    Dout => s_OUT_latch_Dout	
    );
    
DA_latch_port_map : DA_latch port map(
	reset_b => s_DA_latch_reset_b,
	sys_clk => sys_clk,
	en => s_DA_latch_en,
    Din => s_DA_mux_Dout,
    Dout => m_dac_d
    );
    
AD_latch_port_map : AD_latch port map(
	reset_b => s_AD_latch_reset_b,
	sys_clk => sys_clk,
	en => s_AD_latch_en,
    Din => m_adc_d,
    Dout => s_AD_latch_Dout
    );

--mux 3개 port map
OUT_mux_port_map : OUT_mux port map(
	sel => s_OUT_mux_sel,
    Din0 => s_PC_RAM_Dout,
    Din1 => s_Option_RAM_Dout,
    Din2 => s_AD_RAM_Dout,
    Dout => s_OUT_mux_Dout
    );


PC_mux_port_map : PC_mux port map(
	sel => s_PC_mux_sel,
    Din0 => s_IN_latch_Dout,
    Din1 => s_AD_RAM_Dout,
    Dout => s_PC_mux_Dout
    );


DA_mux_port_map : DA_mux port map(
	sel => s_DA_mux_sel,
    Din0 => s_Option_RAM_Dout,
    Din1 => s_PC_RAM_Dout,
    Dout => s_DA_mux_Dout
    );
	
PC_RAM_addr_counter_port_map : PC_RAM_addr_counter port map(
	clk => s_clk,
	reset_b => s_PC_RAM_cnt_reset_b,
	cnt_en 	=> s_PC_RAM_cnt_ce,
	cnt_out => s_PC_RAM_addr10
	);
	
AD_RAM_addra_counter_port_map : AD_RAM_addra_counter port map(
	clk 	=> sys_clk,
	reset_b => s_AD_RAM_cnta_reset_b,
	cnt_en  => s_AD_RAM_cnta_ce,
	cnt_out => s_AD_RAM_addra10
	);
	
AD_RAM_addrb_counter_port_map : AD_RAM_addrb_counter port map(
	clk 	=> s_clk,
	reset_b => s_AD_RAM_cntb_reset_b,
	cnt_en  => s_AD_RAM_cntb_ce,
	cnt_out => s_AD_RAM_addrb10
	);

Option_RAM_addra_counter_port_map : Option_RAM_addra_counter port map(
	clk 	=> s_clk,
	reset_b => s_Option_RAM_cnta_reset_b,
	cnt_en 	=> s_Option_RAM_cnta_ce,
	cnt_out => s_Option_RAM_addra10
	);
	
Option_RAM_addrb_counter_port_map : Option_RAM_addrb_counter port map(
	clk 	=> sys_clk,
	reset_b => s_Option_RAM_cntb_reset_b,
	cnt_en 	=> s_Option_RAM_cntb_ce,
	cnt_out => s_Option_RAM_addrb10
	);

--Filter port map
Filter_port_map : Filter port map(
    m_reset => s_Filter_reset,
    m_clk => s_clk,
    m_en => s_Filter_en,
    m_xin => s_PC_RAM_Dout,
    m_yout => s_Filter_Dout
    );
	
--Option RAM write용 counter
PC_RAM_delayed_counter_port_map : PC_RAM_delayed_counter port map(
	clk => s_clk,
	reset_b => s_delayed_counter_reset_b,
	cnt_en 	=> s_delayed_counter_cnt_en,
	cnt_out => s_delayed_counter_address
	);

--signal controller port map
Signal_Controller_port_map : Signal_Controller port map( 
	m_reset_b => m_fpga_reset,
    s_clk  => s_clk,
    sys_clk => sys_clk,
    s_wen => s_wen,
    s_ren => s_ren,
	s_oe_b => s_oe_b,
 --	s_cmd_data => s_cmd_data, 안쓸듯
 
	s_AD_write_finish => s_AD_write_finish,

    s_reset_addr => s_reset_addr,
    s_reset8254_addr => s_reset8254_addr,
    s_pc_RAM_addr => s_PC_RAM_addr,
    s_da_start_addr => s_da_start_addr,
    s_da_stop_addr => s_da_stop_addr,
    s_ad_RAM_addr => s_ad_RAM_addr,
	s_adr_RAM_addr => s_adr_RAM_addr,
    s_opt_step1_addr => s_opt_step1_addr,
    s_opt_step2_addr => s_opt_step2_addr,
    s_opt_step3_addr => s_opt_step3_addr,
    
    s_PC_RAM_addr10 => s_PC_RAM_addr10,
    s_AD_RAM_addra10 => s_AD_RAM_addra10,
    s_AD_RAM_addrb10 => s_AD_RAM_addrb10,
    s_Option_RAM_addra10 => s_Option_RAM_addra10,
    s_Option_RAM_addrb10 => s_Option_RAM_addrb10,
	s_delayed_counter_address => s_delayed_counter_address,
	
	
	s_PC_RAM_cnt_reset_b => s_PC_RAM_cnt_reset_b,
	s_PC_RAM_cnt_ce => s_PC_RAM_cnt_ce,
	s_PC_RAM_ena => s_PC_RAM_ena,
	s_PC_RAM_wea => s_PC_RAM_wea,
	s_PC_RAM_enb => s_PC_RAM_enb,
	
	s_AD_RAM_cntb_reset_b => s_AD_RAM_cntb_reset_b,
	s_AD_RAM_cntb_ce => s_AD_RAM_cntb_ce,
	s_AD_RAM_enb => s_AD_RAM_enb,
	
	s_Option_RAM_cnta_reset_b => s_Option_RAM_cnta_reset_b,
	s_Option_RAM_cnta_ce => s_Option_RAM_cnta_ce,
	s_Option_RAM_ena => s_Option_RAM_ena,
	s_Option_RAM_wea => s_Option_RAM_wea,
	
	s_Option_RAM_cntb_reset_b => s_Option_RAM_cntb_reset_b,
	s_Option_RAM_cntb_ce => s_Option_RAM_cntb_ce,
	s_Option_RAM_enb => s_Option_RAM_enb,
	
    s_PC_mux_sel => s_PC_mux_sel,
	s_DA_mux_sel => s_DA_mux_sel,
    s_OUT_mux_sel => s_OUT_mux_sel,
	
	s_pc_latch_reset_b => s_pc_latch_reset_b,
	s_in_latch_reset_b => s_in_latch_reset_b,
	s_IN_latch_en => s_IN_latch_en,
	s_out_latch_reset_b => s_out_latch_reset_b,
    s_OUT_latch_en => s_OUT_latch_en,
	s_DA_latch_reset_b => s_DA_latch_reset_b,
    s_DA_latch_en => s_DA_latch_en,
	s_AD_latch_reset_b => s_AD_latch_reset_b,
	s_AD_latch_en => s_AD_latch_en,
    
    s_tri_en_b => s_tri_en_b, 
    s_PC_RAM_data_len => s_PC_RAM_data_len,
	s_AD_RAM_data_len => s_AD_RAM_data_len, 
	s_Option_RAM_data_len => s_Option_RAM_data_len,
	
	s_Filter_reset => s_Filter_reset,
	s_Filter_en => s_Filter_en,
	
	s_delayed_counter_reset_b => s_delayed_counter_reset_b,
	s_delayed_counter_cnt_en => s_delayed_counter_cnt_en,
	
	s_adc_d => m_adc_d,
	s_data => m_data,
	
	s_debug_led => s_debug_led,
	s_debug_sw => s_debug_sw,
	s_debug_header => s_debug_header,
	
	s_sys_state_machine_on => s_sys_state_machine_on,
	s_AD_RAM_max_address => s_AD_RAM_max_address
  );
  
sys_state_machine_port_map : sys_state_machine port map(
	sys_clk => sys_clk,
	s_sys_state_machine_on => s_sys_state_machine_on,
	s_AD_RAM_addra10 => s_AD_RAM_addra10,
	s_AD_RAM_max_address => s_AD_RAM_max_address,
	s_AD_RAM_cnta_reset_b => s_AD_RAM_cnta_reset_b,
	s_AD_RAM_cnta_ce => s_AD_RAM_cnta_ce,
	s_AD_RAM_ena => s_AD_RAM_ena,
	s_AD_RAM_wea => s_AD_RAM_wea,
	s_AD_write_finish => s_AD_write_finish
	
	);
  
--==============================================================================================================




                                                -- Code Input --



--==============================================================================================================


--for debug 

-- Don't change this----------------
m_debug_header(0)	<= s_clk;   
m_debug_header(1)	<= sys_clk; 
m_debug_led(7)      <= s_reset_b;
------------------------------------
m_debug_led(6 downto 0)     <= s_debug_led(6 downto 0);
s_debug_sw(7 downto 0)      <= m_debug_sw(7 downto 0);
m_debug_header(15 downto 2) <= s_debug_header(15 downto 2);

-- s_debug_led, s_debug_sw, s_debug_header를 debugging에 적극 활용하세요.


end Behavioral;
