// ============================================================================
//  conv_int8.h  —  features.0.0 int8 (v3: AXI interface สำหรับ PYNQ)
// ============================================================================
#ifndef CONV_INT8_H
#define CONV_INT8_H

#include <stdint.h>

#define C_IN     3
#define H_IN     224
#define W_IN     224
#define C_OUT    32
#define KH       3
#define KW       3
#define STRIDE   2
#define PAD      1
#define H_OUT    112
#define W_OUT    112

#define N_IN   (C_IN*H_IN*W_IN)        // 150528
#define N_W    (C_OUT*C_IN*KH*KW)      // 864
#define N_OUT  (C_OUT*H_OUT*W_OUT)     // 401408

// ---- Quantization (per-tensor, symmetric weight) ----
#define Z_IN    (-2)
#define Z_OUT   (7)
#define REQUANT_SHIFT 31
#define M_FIX  (10934627LL)   // round(M * 2^31), M = s_in*s_w/s_out

// ---- ระดับขนาน (1 = ประหยัดพื้นที่ตามที่ตัดสินใจ, เพิ่มได้ถ้าพื้นที่เหลือ) ----
#define UF 1

// top function — pointer args สำหรับ m_axi (ข้อมูลอยู่ DDR)
void conv2d_int8(
    const int8_t* input,     // N_IN bytes
    const int8_t* weight,    // N_W bytes
    int8_t*       output);   // N_OUT bytes

#endif
