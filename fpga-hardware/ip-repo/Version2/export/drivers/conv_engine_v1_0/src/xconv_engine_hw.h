// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
// control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read/COR)
//        bit 7  - auto_restart (Read/Write)
//        bit 9  - interrupt (Read)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0 - enable ap_done interrupt (Read/Write)
//        bit 1 - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0 - ap_done (Read/TOW)
//        bit 1 - ap_ready (Read/TOW)
//        others - reserved
// 0x10 : Data signal of input_r
//        bit 31~0 - input_r[31:0] (Read/Write)
// 0x14 : Data signal of input_r
//        bit 31~0 - input_r[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of weight
//        bit 31~0 - weight[31:0] (Read/Write)
// 0x20 : Data signal of weight
//        bit 31~0 - weight[63:32] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of output_r
//        bit 31~0 - output_r[31:0] (Read/Write)
// 0x2c : Data signal of output_r
//        bit 31~0 - output_r[63:32] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of m_fix
//        bit 31~0 - m_fix[31:0] (Read/Write)
// 0x38 : Data signal of m_fix
//        bit 31~0 - m_fix[63:32] (Read/Write)
// 0x3c : reserved
// 0x40 : Data signal of mode
//        bit 31~0 - mode[31:0] (Read/Write)
// 0x44 : reserved
// 0x48 : Data signal of cin
//        bit 31~0 - cin[31:0] (Read/Write)
// 0x4c : reserved
// 0x50 : Data signal of hin
//        bit 31~0 - hin[31:0] (Read/Write)
// 0x54 : reserved
// 0x58 : Data signal of win
//        bit 31~0 - win[31:0] (Read/Write)
// 0x5c : reserved
// 0x60 : Data signal of cout
//        bit 31~0 - cout[31:0] (Read/Write)
// 0x64 : reserved
// 0x68 : Data signal of hout
//        bit 31~0 - hout[31:0] (Read/Write)
// 0x6c : reserved
// 0x70 : Data signal of wout
//        bit 31~0 - wout[31:0] (Read/Write)
// 0x74 : reserved
// 0x78 : Data signal of kh
//        bit 31~0 - kh[31:0] (Read/Write)
// 0x7c : reserved
// 0x80 : Data signal of kw
//        bit 31~0 - kw[31:0] (Read/Write)
// 0x84 : reserved
// 0x88 : Data signal of stride
//        bit 31~0 - stride[31:0] (Read/Write)
// 0x8c : reserved
// 0x90 : Data signal of pad
//        bit 31~0 - pad[31:0] (Read/Write)
// 0x94 : reserved
// 0x98 : Data signal of requant_shift
//        bit 31~0 - requant_shift[31:0] (Read/Write)
// 0x9c : reserved
// 0xa0 : Data signal of z_in
//        bit 7~0 - z_in[7:0] (Read/Write)
//        others  - reserved
// 0xa4 : reserved
// 0xa8 : Data signal of z_out
//        bit 7~0 - z_out[7:0] (Read/Write)
//        others  - reserved
// 0xac : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XCONV_ENGINE_CONTROL_ADDR_AP_CTRL            0x00
#define XCONV_ENGINE_CONTROL_ADDR_GIE                0x04
#define XCONV_ENGINE_CONTROL_ADDR_IER                0x08
#define XCONV_ENGINE_CONTROL_ADDR_ISR                0x0c
#define XCONV_ENGINE_CONTROL_ADDR_INPUT_R_DATA       0x10
#define XCONV_ENGINE_CONTROL_BITS_INPUT_R_DATA       64
#define XCONV_ENGINE_CONTROL_ADDR_WEIGHT_DATA        0x1c
#define XCONV_ENGINE_CONTROL_BITS_WEIGHT_DATA        64
#define XCONV_ENGINE_CONTROL_ADDR_OUTPUT_R_DATA      0x28
#define XCONV_ENGINE_CONTROL_BITS_OUTPUT_R_DATA      64
#define XCONV_ENGINE_CONTROL_ADDR_M_FIX_DATA         0x34
#define XCONV_ENGINE_CONTROL_BITS_M_FIX_DATA         64
#define XCONV_ENGINE_CONTROL_ADDR_MODE_DATA          0x40
#define XCONV_ENGINE_CONTROL_BITS_MODE_DATA          32
#define XCONV_ENGINE_CONTROL_ADDR_CIN_DATA           0x48
#define XCONV_ENGINE_CONTROL_BITS_CIN_DATA           32
#define XCONV_ENGINE_CONTROL_ADDR_HIN_DATA           0x50
#define XCONV_ENGINE_CONTROL_BITS_HIN_DATA           32
#define XCONV_ENGINE_CONTROL_ADDR_WIN_DATA           0x58
#define XCONV_ENGINE_CONTROL_BITS_WIN_DATA           32
#define XCONV_ENGINE_CONTROL_ADDR_COUT_DATA          0x60
#define XCONV_ENGINE_CONTROL_BITS_COUT_DATA          32
#define XCONV_ENGINE_CONTROL_ADDR_HOUT_DATA          0x68
#define XCONV_ENGINE_CONTROL_BITS_HOUT_DATA          32
#define XCONV_ENGINE_CONTROL_ADDR_WOUT_DATA          0x70
#define XCONV_ENGINE_CONTROL_BITS_WOUT_DATA          32
#define XCONV_ENGINE_CONTROL_ADDR_KH_DATA            0x78
#define XCONV_ENGINE_CONTROL_BITS_KH_DATA            32
#define XCONV_ENGINE_CONTROL_ADDR_KW_DATA            0x80
#define XCONV_ENGINE_CONTROL_BITS_KW_DATA            32
#define XCONV_ENGINE_CONTROL_ADDR_STRIDE_DATA        0x88
#define XCONV_ENGINE_CONTROL_BITS_STRIDE_DATA        32
#define XCONV_ENGINE_CONTROL_ADDR_PAD_DATA           0x90
#define XCONV_ENGINE_CONTROL_BITS_PAD_DATA           32
#define XCONV_ENGINE_CONTROL_ADDR_REQUANT_SHIFT_DATA 0x98
#define XCONV_ENGINE_CONTROL_BITS_REQUANT_SHIFT_DATA 32
#define XCONV_ENGINE_CONTROL_ADDR_Z_IN_DATA          0xa0
#define XCONV_ENGINE_CONTROL_BITS_Z_IN_DATA          8
#define XCONV_ENGINE_CONTROL_ADDR_Z_OUT_DATA         0xa8
#define XCONV_ENGINE_CONTROL_BITS_Z_OUT_DATA         8

