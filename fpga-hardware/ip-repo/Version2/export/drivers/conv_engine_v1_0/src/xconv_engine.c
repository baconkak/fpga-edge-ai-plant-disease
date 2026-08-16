// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xconv_engine.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XConv_engine_CfgInitialize(XConv_engine *InstancePtr, XConv_engine_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XConv_engine_Start(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL) & 0x80;
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XConv_engine_IsDone(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XConv_engine_IsIdle(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XConv_engine_IsReady(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XConv_engine_EnableAutoRestart(XConv_engine *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XConv_engine_DisableAutoRestart(XConv_engine *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_AP_CTRL, 0);
}

void XConv_engine_Set_input_r(XConv_engine *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_INPUT_R_DATA, (u32)(Data));
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_INPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XConv_engine_Get_input_r(XConv_engine *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_INPUT_R_DATA);
    Data += (u64)XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_INPUT_R_DATA + 4) << 32;
    return Data;
}

void XConv_engine_Set_weight(XConv_engine *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WEIGHT_DATA, (u32)(Data));
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WEIGHT_DATA + 4, (u32)(Data >> 32));
}

u64 XConv_engine_Get_weight(XConv_engine *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WEIGHT_DATA);
    Data += (u64)XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WEIGHT_DATA + 4) << 32;
    return Data;
}

void XConv_engine_Set_output_r(XConv_engine *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_OUTPUT_R_DATA, (u32)(Data));
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_OUTPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XConv_engine_Get_output_r(XConv_engine *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_OUTPUT_R_DATA);
    Data += (u64)XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_OUTPUT_R_DATA + 4) << 32;
    return Data;
}

void XConv_engine_Set_m_fix(XConv_engine *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_M_FIX_DATA, (u32)(Data));
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_M_FIX_DATA + 4, (u32)(Data >> 32));
}

u64 XConv_engine_Get_m_fix(XConv_engine *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_M_FIX_DATA);
    Data += (u64)XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_M_FIX_DATA + 4) << 32;
    return Data;
}

void XConv_engine_Set_mode(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_MODE_DATA, Data);
}

u32 XConv_engine_Get_mode(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_MODE_DATA);
    return Data;
}

void XConv_engine_Set_cin(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_CIN_DATA, Data);
}

u32 XConv_engine_Get_cin(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_CIN_DATA);
    return Data;
}

void XConv_engine_Set_hin(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_HIN_DATA, Data);
}

u32 XConv_engine_Get_hin(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_HIN_DATA);
    return Data;
}

void XConv_engine_Set_win(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WIN_DATA, Data);
}

u32 XConv_engine_Get_win(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WIN_DATA);
    return Data;
}

void XConv_engine_Set_cout(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_COUT_DATA, Data);
}

u32 XConv_engine_Get_cout(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_COUT_DATA);
    return Data;
}

void XConv_engine_Set_hout(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_HOUT_DATA, Data);
}

u32 XConv_engine_Get_hout(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_HOUT_DATA);
    return Data;
}

void XConv_engine_Set_wout(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WOUT_DATA, Data);
}

u32 XConv_engine_Get_wout(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_WOUT_DATA);
    return Data;
}

void XConv_engine_Set_kh(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_KH_DATA, Data);
}

u32 XConv_engine_Get_kh(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_KH_DATA);
    return Data;
}

void XConv_engine_Set_kw(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_KW_DATA, Data);
}

u32 XConv_engine_Get_kw(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_KW_DATA);
    return Data;
}

void XConv_engine_Set_stride(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_STRIDE_DATA, Data);
}

u32 XConv_engine_Get_stride(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_STRIDE_DATA);
    return Data;
}

void XConv_engine_Set_pad(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_PAD_DATA, Data);
}

u32 XConv_engine_Get_pad(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_PAD_DATA);
    return Data;
}

void XConv_engine_Set_requant_shift(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_REQUANT_SHIFT_DATA, Data);
}

u32 XConv_engine_Get_requant_shift(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_REQUANT_SHIFT_DATA);
    return Data;
}

void XConv_engine_Set_z_in(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_Z_IN_DATA, Data);
}

u32 XConv_engine_Get_z_in(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_Z_IN_DATA);
    return Data;
}

void XConv_engine_Set_z_out(XConv_engine *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_Z_OUT_DATA, Data);
}

u32 XConv_engine_Get_z_out(XConv_engine *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_Z_OUT_DATA);
    return Data;
}

void XConv_engine_InterruptGlobalEnable(XConv_engine *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_GIE, 1);
}

void XConv_engine_InterruptGlobalDisable(XConv_engine *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_GIE, 0);
}

void XConv_engine_InterruptEnable(XConv_engine *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_IER);
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_IER, Register | Mask);
}

void XConv_engine_InterruptDisable(XConv_engine *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_IER);
    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_IER, Register & (~Mask));
}

void XConv_engine_InterruptClear(XConv_engine *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv_engine_WriteReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_ISR, Mask);
}

u32 XConv_engine_InterruptGetEnabled(XConv_engine *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_IER);
}

u32 XConv_engine_InterruptGetStatus(XConv_engine *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XConv_engine_ReadReg(InstancePtr->Control_BaseAddress, XCONV_ENGINE_CONTROL_ADDR_ISR);
}

