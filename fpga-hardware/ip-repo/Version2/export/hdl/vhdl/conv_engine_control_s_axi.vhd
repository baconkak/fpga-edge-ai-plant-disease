-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
-- Tool Version Limit: 2024.05
-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
-- 
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity conv_engine_control_s_axi is
generic (
    C_S_AXI_ADDR_WIDTH    : INTEGER := 8;
    C_S_AXI_DATA_WIDTH    : INTEGER := 32);
port (
    ACLK                  :in   STD_LOGIC;
    ARESET                :in   STD_LOGIC;
    ACLK_EN               :in   STD_LOGIC;
    AWADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    AWVALID               :in   STD_LOGIC;
    AWREADY               :out  STD_LOGIC;
    WDATA                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    WSTRB                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH/8-1 downto 0);
    WVALID                :in   STD_LOGIC;
    WREADY                :out  STD_LOGIC;
    BRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    BVALID                :out  STD_LOGIC;
    BREADY                :in   STD_LOGIC;
    ARADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    ARVALID               :in   STD_LOGIC;
    ARREADY               :out  STD_LOGIC;
    RDATA                 :out  STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    RRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    RVALID                :out  STD_LOGIC;
    RREADY                :in   STD_LOGIC;
    interrupt             :out  STD_LOGIC;
    input_r               :out  STD_LOGIC_VECTOR(63 downto 0);
    weight                :out  STD_LOGIC_VECTOR(63 downto 0);
    output_r              :out  STD_LOGIC_VECTOR(63 downto 0);
    m_fix                 :out  STD_LOGIC_VECTOR(63 downto 0);
    mode                  :out  STD_LOGIC_VECTOR(31 downto 0);
    cin                   :out  STD_LOGIC_VECTOR(31 downto 0);
    hin                   :out  STD_LOGIC_VECTOR(31 downto 0);
    win                   :out  STD_LOGIC_VECTOR(31 downto 0);
    cout                  :out  STD_LOGIC_VECTOR(31 downto 0);
    hout                  :out  STD_LOGIC_VECTOR(31 downto 0);
    wout                  :out  STD_LOGIC_VECTOR(31 downto 0);
    kh                    :out  STD_LOGIC_VECTOR(31 downto 0);
    kw                    :out  STD_LOGIC_VECTOR(31 downto 0);
    stride                :out  STD_LOGIC_VECTOR(31 downto 0);
    pad                   :out  STD_LOGIC_VECTOR(31 downto 0);
    requant_shift         :out  STD_LOGIC_VECTOR(31 downto 0);
    z_in                  :out  STD_LOGIC_VECTOR(7 downto 0);
    z_out                 :out  STD_LOGIC_VECTOR(7 downto 0);
    ap_start              :out  STD_LOGIC;
    ap_done               :in   STD_LOGIC;
    ap_ready              :in   STD_LOGIC;
    ap_idle               :in   STD_LOGIC
);
end entity conv_engine_control_s_axi;

-- ------------------------Address Info-------------------
-- Protocol Used: ap_ctrl_hs
--
-- 0x00 : Control signals
--        bit 0  - ap_start (Read/Write/COH)
--        bit 1  - ap_done (Read/COR)
--        bit 2  - ap_idle (Read)
--        bit 3  - ap_ready (Read/COR)
--        bit 7  - auto_restart (Read/Write)
--        bit 9  - interrupt (Read)
--        others - reserved
-- 0x04 : Global Interrupt Enable Register
--        bit 0  - Global Interrupt Enable (Read/Write)
--        others - reserved
-- 0x08 : IP Interrupt Enable Register (Read/Write)
--        bit 0 - enable ap_done interrupt (Read/Write)
--        bit 1 - enable ap_ready interrupt (Read/Write)
--        others - reserved
-- 0x0c : IP Interrupt Status Register (Read/TOW)
--        bit 0 - ap_done (Read/TOW)
--        bit 1 - ap_ready (Read/TOW)
--        others - reserved
-- 0x10 : Data signal of input_r
--        bit 31~0 - input_r[31:0] (Read/Write)
-- 0x14 : Data signal of input_r
--        bit 31~0 - input_r[63:32] (Read/Write)
-- 0x18 : reserved
-- 0x1c : Data signal of weight
--        bit 31~0 - weight[31:0] (Read/Write)
-- 0x20 : Data signal of weight
--        bit 31~0 - weight[63:32] (Read/Write)
-- 0x24 : reserved
-- 0x28 : Data signal of output_r
--        bit 31~0 - output_r[31:0] (Read/Write)
-- 0x2c : Data signal of output_r
--        bit 31~0 - output_r[63:32] (Read/Write)
-- 0x30 : reserved
-- 0x34 : Data signal of m_fix
--        bit 31~0 - m_fix[31:0] (Read/Write)
-- 0x38 : Data signal of m_fix
--        bit 31~0 - m_fix[63:32] (Read/Write)
-- 0x3c : reserved
-- 0x40 : Data signal of mode
--        bit 31~0 - mode[31:0] (Read/Write)
-- 0x44 : reserved
-- 0x48 : Data signal of cin
--        bit 31~0 - cin[31:0] (Read/Write)
-- 0x4c : reserved
-- 0x50 : Data signal of hin
--        bit 31~0 - hin[31:0] (Read/Write)
-- 0x54 : reserved
-- 0x58 : Data signal of win
--        bit 31~0 - win[31:0] (Read/Write)
-- 0x5c : reserved
-- 0x60 : Data signal of cout
--        bit 31~0 - cout[31:0] (Read/Write)
-- 0x64 : reserved
-- 0x68 : Data signal of hout
--        bit 31~0 - hout[31:0] (Read/Write)
-- 0x6c : reserved
-- 0x70 : Data signal of wout
--        bit 31~0 - wout[31:0] (Read/Write)
-- 0x74 : reserved
-- 0x78 : Data signal of kh
--        bit 31~0 - kh[31:0] (Read/Write)
-- 0x7c : reserved
-- 0x80 : Data signal of kw
--        bit 31~0 - kw[31:0] (Read/Write)
-- 0x84 : reserved
-- 0x88 : Data signal of stride
--        bit 31~0 - stride[31:0] (Read/Write)
-- 0x8c : reserved
-- 0x90 : Data signal of pad
--        bit 31~0 - pad[31:0] (Read/Write)
-- 0x94 : reserved
-- 0x98 : Data signal of requant_shift
--        bit 31~0 - requant_shift[31:0] (Read/Write)
-- 0x9c : reserved
-- 0xa0 : Data signal of z_in
--        bit 7~0 - z_in[7:0] (Read/Write)
--        others  - reserved
-- 0xa4 : reserved
-- 0xa8 : Data signal of z_out
--        bit 7~0 - z_out[7:0] (Read/Write)
--        others  - reserved
-- 0xac : reserved
-- (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

architecture behave of conv_engine_control_s_axi is
    type states is (wridle, wrdata, wrresp, wrreset, rdidle, rddata, rdreset);  -- read and write fsm states
    signal wstate  : states := wrreset;
    signal rstate  : states := rdreset;
    signal wnext, rnext: states;
    constant ADDR_AP_CTRL              : INTEGER := 16#00#;
    constant ADDR_GIE                  : INTEGER := 16#04#;
    constant ADDR_IER                  : INTEGER := 16#08#;
    constant ADDR_ISR                  : INTEGER := 16#0c#;
    constant ADDR_INPUT_R_DATA_0       : INTEGER := 16#10#;
    constant ADDR_INPUT_R_DATA_1       : INTEGER := 16#14#;
    constant ADDR_INPUT_R_CTRL         : INTEGER := 16#18#;
    constant ADDR_WEIGHT_DATA_0        : INTEGER := 16#1c#;
    constant ADDR_WEIGHT_DATA_1        : INTEGER := 16#20#;
    constant ADDR_WEIGHT_CTRL          : INTEGER := 16#24#;
    constant ADDR_OUTPUT_R_DATA_0      : INTEGER := 16#28#;
    constant ADDR_OUTPUT_R_DATA_1      : INTEGER := 16#2c#;
    constant ADDR_OUTPUT_R_CTRL        : INTEGER := 16#30#;
    constant ADDR_M_FIX_DATA_0         : INTEGER := 16#34#;
    constant ADDR_M_FIX_DATA_1         : INTEGER := 16#38#;
    constant ADDR_M_FIX_CTRL           : INTEGER := 16#3c#;
    constant ADDR_MODE_DATA_0          : INTEGER := 16#40#;
    constant ADDR_MODE_CTRL            : INTEGER := 16#44#;
    constant ADDR_CIN_DATA_0           : INTEGER := 16#48#;
    constant ADDR_CIN_CTRL             : INTEGER := 16#4c#;
    constant ADDR_HIN_DATA_0           : INTEGER := 16#50#;
    constant ADDR_HIN_CTRL             : INTEGER := 16#54#;
    constant ADDR_WIN_DATA_0           : INTEGER := 16#58#;
    constant ADDR_WIN_CTRL             : INTEGER := 16#5c#;
    constant ADDR_COUT_DATA_0          : INTEGER := 16#60#;
    constant ADDR_COUT_CTRL            : INTEGER := 16#64#;
    constant ADDR_HOUT_DATA_0          : INTEGER := 16#68#;
    constant ADDR_HOUT_CTRL            : INTEGER := 16#6c#;
    constant ADDR_WOUT_DATA_0          : INTEGER := 16#70#;
    constant ADDR_WOUT_CTRL            : INTEGER := 16#74#;
    constant ADDR_KH_DATA_0            : INTEGER := 16#78#;
    constant ADDR_KH_CTRL              : INTEGER := 16#7c#;
    constant ADDR_KW_DATA_0            : INTEGER := 16#80#;
    constant ADDR_KW_CTRL              : INTEGER := 16#84#;
    constant ADDR_STRIDE_DATA_0        : INTEGER := 16#88#;
    constant ADDR_STRIDE_CTRL          : INTEGER := 16#8c#;
    constant ADDR_PAD_DATA_0           : INTEGER := 16#90#;
    constant ADDR_PAD_CTRL             : INTEGER := 16#94#;
    constant ADDR_REQUANT_SHIFT_DATA_0 : INTEGER := 16#98#;
    constant ADDR_REQUANT_SHIFT_CTRL   : INTEGER := 16#9c#;
    constant ADDR_Z_IN_DATA_0          : INTEGER := 16#a0#;
    constant ADDR_Z_IN_CTRL            : INTEGER := 16#a4#;
    constant ADDR_Z_OUT_DATA_0         : INTEGER := 16#a8#;
    constant ADDR_Z_OUT_CTRL           : INTEGER := 16#ac#;
    constant ADDR_BITS         : INTEGER := 8;

    signal waddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal wmask               : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal aw_hs               : STD_LOGIC;
    signal w_hs                : STD_LOGIC;
    signal rdata_data          : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal ar_hs               : STD_LOGIC;
    signal raddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal AWREADY_t           : STD_LOGIC;
    signal WREADY_t            : STD_LOGIC;
    signal ARREADY_t           : STD_LOGIC;
    signal RVALID_t            : STD_LOGIC;
    -- internal registers
    signal int_ap_idle         : STD_LOGIC := '0';
    signal int_ap_ready        : STD_LOGIC := '0';
    signal task_ap_ready       : STD_LOGIC;
    signal int_ap_done         : STD_LOGIC := '0';
    signal task_ap_done        : STD_LOGIC;
    signal int_task_ap_done    : STD_LOGIC := '0';
    signal int_ap_start        : STD_LOGIC := '0';
    signal int_interrupt       : STD_LOGIC := '0';
    signal int_auto_restart    : STD_LOGIC := '0';
    signal auto_restart_status : STD_LOGIC := '0';
    signal auto_restart_done   : STD_LOGIC;
    signal int_gie             : STD_LOGIC := '0';
    signal int_ier             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_isr             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_input_r         : UNSIGNED(63 downto 0) := (others => '0');
    signal int_weight          : UNSIGNED(63 downto 0) := (others => '0');
    signal int_output_r        : UNSIGNED(63 downto 0) := (others => '0');
    signal int_m_fix           : UNSIGNED(63 downto 0) := (others => '0');
    signal int_mode            : UNSIGNED(31 downto 0) := (others => '0');
    signal int_cin             : UNSIGNED(31 downto 0) := (others => '0');
    signal int_hin             : UNSIGNED(31 downto 0) := (others => '0');
    signal int_win             : UNSIGNED(31 downto 0) := (others => '0');
    signal int_cout            : UNSIGNED(31 downto 0) := (others => '0');
    signal int_hout            : UNSIGNED(31 downto 0) := (others => '0');
    signal int_wout            : UNSIGNED(31 downto 0) := (others => '0');
    signal int_kh              : UNSIGNED(31 downto 0) := (others => '0');
    signal int_kw              : UNSIGNED(31 downto 0) := (others => '0');
    signal int_stride          : UNSIGNED(31 downto 0) := (others => '0');
    signal int_pad             : UNSIGNED(31 downto 0) := (others => '0');
    signal int_requant_shift   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_z_in            : UNSIGNED(7 downto 0) := (others => '0');
    signal int_z_out           : UNSIGNED(7 downto 0) := (others => '0');


begin
-- ----------------------- Instantiation------------------


-- ----------------------- AXI WRITE ---------------------
    AWREADY_t <=  '1' when wstate = wridle else '0';
    AWREADY   <=  AWREADY_t;
    WREADY_t  <=  '1' when wstate = wrdata else '0';
    WREADY    <=  WREADY_t;
    BRESP     <=  "00";  -- OKAY
    BVALID    <=  '1' when wstate = wrresp else '0';
    wmask     <=  (31 downto 24 => WSTRB(3), 23 downto 16 => WSTRB(2), 15 downto 8 => WSTRB(1), 7 downto 0 => WSTRB(0));
    aw_hs     <=  AWVALID and AWREADY_t;
    w_hs      <=  WVALID and WREADY_t;

    -- write FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                wstate <= wrreset;
            elsif (ACLK_EN = '1') then
                wstate <= wnext;
            end if;
        end if;
    end process;

    process (wstate, AWVALID, WVALID, BREADY)
    begin
        case (wstate) is
        when wridle =>
            if (AWVALID = '1') then
                wnext <= wrdata;
            else
                wnext <= wridle;
            end if;
        when wrdata =>
            if (WVALID = '1') then
                wnext <= wrresp;
            else
                wnext <= wrdata;
            end if;
        when wrresp =>
            if (BREADY = '1') then
                wnext <= wridle;
            else
                wnext <= wrresp;
            end if;
        when others =>
            wnext <= wridle;
        end case;
    end process;

    waddr_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (aw_hs = '1') then
                    waddr <= UNSIGNED(AWADDR(ADDR_BITS-1 downto 2) & (1 downto 0 => '0'));
                end if;
            end if;
        end if;
    end process;

-- ----------------------- AXI READ ----------------------
    ARREADY_t <= '1' when (rstate = rdidle) else '0';
    ARREADY <= ARREADY_t;
    RDATA   <= STD_LOGIC_VECTOR(rdata_data);
    RRESP   <= "00";  -- OKAY
    RVALID_t  <= '1' when (rstate = rddata) else '0';
    RVALID    <= RVALID_t;
    ar_hs   <= ARVALID and ARREADY_t;
    raddr   <= UNSIGNED(ARADDR(ADDR_BITS-1 downto 0));

    -- read FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                rstate <= rdreset;
            elsif (ACLK_EN = '1') then
                rstate <= rnext;
            end if;
        end if;
    end process;

    process (rstate, ARVALID, RREADY, RVALID_t)
    begin
        case (rstate) is
        when rdidle =>
            if (ARVALID = '1') then
                rnext <= rddata;
            else
                rnext <= rdidle;
            end if;
        when rddata =>
            if (RREADY = '1' and RVALID_t = '1') then
                rnext <= rdidle;
            else
                rnext <= rddata;
            end if;
        when others =>
            rnext <= rdidle;
        end case;
    end process;

    rdata_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (ar_hs = '1') then
                    rdata_data <= (others => '0');
                    case (TO_INTEGER(raddr)) is
                    when ADDR_AP_CTRL =>
                        rdata_data(9) <= int_interrupt;
                        rdata_data(7) <= int_auto_restart;
                        rdata_data(3) <= int_ap_ready;
                        rdata_data(2) <= int_ap_idle;
                        rdata_data(1) <= int_task_ap_done;
                        rdata_data(0) <= int_ap_start;
                    when ADDR_GIE =>
                        rdata_data(0) <= int_gie;
                    when ADDR_IER =>
                        rdata_data(1 downto 0) <= int_ier;
                    when ADDR_ISR =>
                        rdata_data(1 downto 0) <= int_isr;
                    when ADDR_INPUT_R_DATA_0 =>
                        rdata_data <= RESIZE(int_input_r(31 downto 0), 32);
                    when ADDR_INPUT_R_DATA_1 =>
                        rdata_data <= RESIZE(int_input_r(63 downto 32), 32);
                    when ADDR_WEIGHT_DATA_0 =>
                        rdata_data <= RESIZE(int_weight(31 downto 0), 32);
                    when ADDR_WEIGHT_DATA_1 =>
                        rdata_data <= RESIZE(int_weight(63 downto 32), 32);
                    when ADDR_OUTPUT_R_DATA_0 =>
                        rdata_data <= RESIZE(int_output_r(31 downto 0), 32);
                    when ADDR_OUTPUT_R_DATA_1 =>
                        rdata_data <= RESIZE(int_output_r(63 downto 32), 32);
                    when ADDR_M_FIX_DATA_0 =>
                        rdata_data <= RESIZE(int_m_fix(31 downto 0), 32);
                    when ADDR_M_FIX_DATA_1 =>
                        rdata_data <= RESIZE(int_m_fix(63 downto 32), 32);
                    when ADDR_MODE_DATA_0 =>
                        rdata_data <= RESIZE(int_mode(31 downto 0), 32);
                    when ADDR_CIN_DATA_0 =>
                        rdata_data <= RESIZE(int_cin(31 downto 0), 32);
                    when ADDR_HIN_DATA_0 =>
                        rdata_data <= RESIZE(int_hin(31 downto 0), 32);
                    when ADDR_WIN_DATA_0 =>
                        rdata_data <= RESIZE(int_win(31 downto 0), 32);
                    when ADDR_COUT_DATA_0 =>
                        rdata_data <= RESIZE(int_cout(31 downto 0), 32);
                    when ADDR_HOUT_DATA_0 =>
                        rdata_data <= RESIZE(int_hout(31 downto 0), 32);
                    when ADDR_WOUT_DATA_0 =>
                        rdata_data <= RESIZE(int_wout(31 downto 0), 32);
                    when ADDR_KH_DATA_0 =>
                        rdata_data <= RESIZE(int_kh(31 downto 0), 32);
                    when ADDR_KW_DATA_0 =>
                        rdata_data <= RESIZE(int_kw(31 downto 0), 32);
                    when ADDR_STRIDE_DATA_0 =>
                        rdata_data <= RESIZE(int_stride(31 downto 0), 32);
                    when ADDR_PAD_DATA_0 =>
                        rdata_data <= RESIZE(int_pad(31 downto 0), 32);
                    when ADDR_REQUANT_SHIFT_DATA_0 =>
                        rdata_data <= RESIZE(int_requant_shift(31 downto 0), 32);
                    when ADDR_Z_IN_DATA_0 =>
                        rdata_data <= RESIZE(int_z_in(7 downto 0), 32);
                    when ADDR_Z_OUT_DATA_0 =>
                        rdata_data <= RESIZE(int_z_out(7 downto 0), 32);
                    when others =>
                        NULL;
                    end case;
                end if;
            end if;
        end if;
    end process;

-- ----------------------- Register logic ----------------
    interrupt            <= int_interrupt;
    ap_start             <= int_ap_start;
    task_ap_done         <= (ap_done and not auto_restart_status) or auto_restart_done;
    task_ap_ready        <= ap_ready and not int_auto_restart;
    auto_restart_done    <= auto_restart_status and (ap_idle and not int_ap_idle);
    input_r              <= STD_LOGIC_VECTOR(int_input_r);
    weight               <= STD_LOGIC_VECTOR(int_weight);
    output_r             <= STD_LOGIC_VECTOR(int_output_r);
    m_fix                <= STD_LOGIC_VECTOR(int_m_fix);
    mode                 <= STD_LOGIC_VECTOR(int_mode);
    cin                  <= STD_LOGIC_VECTOR(int_cin);
    hin                  <= STD_LOGIC_VECTOR(int_hin);
    win                  <= STD_LOGIC_VECTOR(int_win);
    cout                 <= STD_LOGIC_VECTOR(int_cout);
    hout                 <= STD_LOGIC_VECTOR(int_hout);
    wout                 <= STD_LOGIC_VECTOR(int_wout);
    kh                   <= STD_LOGIC_VECTOR(int_kh);
    kw                   <= STD_LOGIC_VECTOR(int_kw);
    stride               <= STD_LOGIC_VECTOR(int_stride);
    pad                  <= STD_LOGIC_VECTOR(int_pad);
    requant_shift        <= STD_LOGIC_VECTOR(int_requant_shift);
    z_in                 <= STD_LOGIC_VECTOR(int_z_in);
    z_out                <= STD_LOGIC_VECTOR(int_z_out);

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_interrupt <= '0';
            elsif (ACLK_EN = '1') then
                if (int_gie = '1' and (int_isr(0) or int_isr(1)) = '1') then
                    int_interrupt <= '1';
                else
                    int_interrupt <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_start <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1' and WDATA(0) = '1') then
                    int_ap_start <= '1';
                elsif (ap_ready = '1') then
                    int_ap_start <= int_auto_restart; -- clear on handshake/auto restart
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_done <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_done <= ap_done;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_task_ap_done <= '0';
            elsif (ACLK_EN = '1') then
                if (task_ap_done = '1') then
                    int_task_ap_done <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_AP_CTRL) then
                    int_task_ap_done <= '0'; -- clear on read
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_idle <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_idle <= ap_idle;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_ready <= '0';
            elsif (ACLK_EN = '1') then
                if (task_ap_ready = '1') then
                    int_ap_ready <= '1';
                elsif (ar_hs = '1' and raddr = ADDR_AP_CTRL) then
                    int_ap_ready <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_auto_restart <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1') then
                    int_auto_restart <= WDATA(7);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                auto_restart_status <= '0';
            elsif (ACLK_EN = '1') then
                if (int_auto_restart = '1') then
                    auto_restart_status <= '1';
                elsif (ap_idle = '1') then
                    auto_restart_status <= '0';
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_gie <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_GIE and WSTRB(0) = '1') then
                    int_gie <= WDATA(0);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ier <= (others=>'0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_IER and WSTRB(0) = '1') then
                    int_ier <= UNSIGNED(WDATA(1 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(0) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(0) = '1' and ap_done = '1') then
                    int_isr(0) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(0) <= int_isr(0) xor WDATA(0); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(1) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(1) = '1' and ap_ready = '1') then
                    int_isr(1) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(1) <= int_isr(1) xor WDATA(1); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_input_r(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_INPUT_R_DATA_0) then
                    int_input_r(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_input_r(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_input_r(63 downto 32) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_INPUT_R_DATA_1) then
                    int_input_r(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_input_r(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_weight(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_WEIGHT_DATA_0) then
                    int_weight(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_weight(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_weight(63 downto 32) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_WEIGHT_DATA_1) then
                    int_weight(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_weight(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_output_r(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_OUTPUT_R_DATA_0) then
                    int_output_r(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_output_r(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_output_r(63 downto 32) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_OUTPUT_R_DATA_1) then
                    int_output_r(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_output_r(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_m_fix(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_M_FIX_DATA_0) then
                    int_m_fix(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_m_fix(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_m_fix(63 downto 32) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_M_FIX_DATA_1) then
                    int_m_fix(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_m_fix(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_mode(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_MODE_DATA_0) then
                    int_mode(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_mode(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_cin(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_CIN_DATA_0) then
                    int_cin(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_cin(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_hin(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HIN_DATA_0) then
                    int_hin(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_hin(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_win(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_WIN_DATA_0) then
                    int_win(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_win(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_cout(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_COUT_DATA_0) then
                    int_cout(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_cout(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_hout(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_HOUT_DATA_0) then
                    int_hout(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_hout(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_wout(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_WOUT_DATA_0) then
                    int_wout(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_wout(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_kh(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_KH_DATA_0) then
                    int_kh(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_kh(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_kw(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_KW_DATA_0) then
                    int_kw(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_kw(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_stride(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_STRIDE_DATA_0) then
                    int_stride(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_stride(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_pad(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PAD_DATA_0) then
                    int_pad(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_pad(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_requant_shift(31 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_REQUANT_SHIFT_DATA_0) then
                    int_requant_shift(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_requant_shift(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_z_in(7 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_Z_IN_DATA_0) then
                    int_z_in(7 downto 0) <= (UNSIGNED(WDATA(7 downto 0)) and wmask(7 downto 0)) or ((not wmask(7 downto 0)) and int_z_in(7 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_z_out(7 downto 0) <= (others => '0');
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_Z_OUT_DATA_0) then
                    int_z_out(7 downto 0) <= (UNSIGNED(WDATA(7 downto 0)) and wmask(7 downto 0)) or ((not wmask(7 downto 0)) and int_z_out(7 downto 0));
                end if;
            end if;
        end if;
    end process;


-- ----------------------- Memory logic ------------------

end architecture behave;
