// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xconv2d_int8.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XConv2d_int8_CfgInitialize(XConv2d_int8 *InstancePtr, XConv2d_int8_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XConv2d_int8_Start(XConv2d_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL) & 0x80;
    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XConv2d_int8_IsDone(XConv2d_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XConv2d_int8_IsIdle(XConv2d_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XConv2d_int8_IsReady(XConv2d_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XConv2d_int8_EnableAutoRestart(XConv2d_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XConv2d_int8_DisableAutoRestart(XConv2d_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_AP_CTRL, 0);
}

void XConv2d_int8_Set_input_r(XConv2d_int8 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_INPUT_R_DATA, (u32)(Data));
    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_INPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XConv2d_int8_Get_input_r(XConv2d_int8 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_INPUT_R_DATA);
    Data += (u64)XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_INPUT_R_DATA + 4) << 32;
    return Data;
}

void XConv2d_int8_Set_weight(XConv2d_int8 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_WEIGHT_DATA, (u32)(Data));
    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_WEIGHT_DATA + 4, (u32)(Data >> 32));
}

u64 XConv2d_int8_Get_weight(XConv2d_int8 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_WEIGHT_DATA);
    Data += (u64)XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_WEIGHT_DATA + 4) << 32;
    return Data;
}

void XConv2d_int8_Set_output_r(XConv2d_int8 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_OUTPUT_R_DATA, (u32)(Data));
    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_OUTPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XConv2d_int8_Get_output_r(XConv2d_int8 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_OUTPUT_R_DATA);
    Data += (u64)XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_OUTPUT_R_DATA + 4) << 32;
    return Data;
}

void XConv2d_int8_InterruptGlobalEnable(XConv2d_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_GIE, 1);
}

void XConv2d_int8_InterruptGlobalDisable(XConv2d_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_GIE, 0);
}

void XConv2d_int8_InterruptEnable(XConv2d_int8 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_IER);
    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_IER, Register | Mask);
}

void XConv2d_int8_InterruptDisable(XConv2d_int8 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_IER);
    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_IER, Register & (~Mask));
}

void XConv2d_int8_InterruptClear(XConv2d_int8 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XConv2d_int8_WriteReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_ISR, Mask);
}

u32 XConv2d_int8_InterruptGetEnabled(XConv2d_int8 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_IER);
}

u32 XConv2d_int8_InterruptGetStatus(XConv2d_int8 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XConv2d_int8_ReadReg(InstancePtr->Control_BaseAddress, XCONV2D_INT8_CONTROL_ADDR_ISR);
}

