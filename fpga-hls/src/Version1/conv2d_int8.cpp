// ============================================================================
//  conv2d_int8.cpp  —  INT8 v3: AXI interface (m_axi + s_axilite) สำหรับ PYNQ
//
//  โครงบนบอร์ดจริง:
//    ARM(Python) วาง input/weight ใน DDR -> ส่ง address ผ่าน s_axilite ->
//    สั่ง start -> วงจร burst-copy เข้า BRAM -> คำนวณ -> เขียน output กลับ DDR
//
//  flow เดิม (compute) ไม่แตะ: int8 MAC -> int32 acc -> fixed-point requantize
// ============================================================================
#include "conv_int8.h"

static int8_t requantize(int32_t acc) {
    int64_t prod = (int64_t)acc * M_FIX;
    const int64_t half = 1LL << (REQUANT_SHIFT - 1);
    int32_t q;
    if (prod >= 0) q = (int32_t)( (prod + half) >> REQUANT_SHIFT );
    else           q = -(int32_t)( ((-prod) + half) >> REQUANT_SHIFT );
    q += Z_OUT;
    if (q > 127)  q = 127;
    if (q < -128) q = -128;
    return (int8_t)q;
}

void conv2d_int8(
    const int8_t* input,
    const int8_t* weight,
    int8_t*       output)
{
    // ---------------- AXI INTERFACES ----------------
    // m_axi: ท่อข้อมูลไป DDR (แยก bundle ให้อ่าน/เขียนขนานกันได้)
    // offset=slave: ARM ส่ง address ผ่าน control register
    // depth: บอกขนาดให้ COSIM จำลองถูก
    #pragma HLS INTERFACE m_axi     port=input  offset=slave bundle=gmem0 depth=150528
    #pragma HLS INTERFACE m_axi     port=weight offset=slave bundle=gmem1 depth=864
    #pragma HLS INTERFACE m_axi     port=output offset=slave bundle=gmem2 depth=401408
    // s_axilite: control (start/done/idle + address registers) ให้ PYNQ คุย
    #pragma HLS INTERFACE s_axilite port=input  bundle=control
    #pragma HLS INTERFACE s_axilite port=weight bundle=control
    #pragma HLS INTERFACE s_axilite port=output bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // ---------------- LOCAL BUFFERS (BRAM) ----------------
    // อ่านจาก DDR ตรง ๆ ทีละค่าช้า + partition ไม่ได้
    // จึง burst-copy เข้า BRAM ก่อน แล้วคำนวณจาก BRAM (partition ได้เหมือนเดิม)
    static int8_t in_buf[C_IN][H_IN][W_IN];
    #pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1
    #pragma HLS ARRAY_PARTITION variable=in_buf cyclic factor=3 dim=2
    #pragma HLS ARRAY_PARTITION variable=in_buf cyclic factor=3 dim=3

    static int8_t w_buf[C_OUT][C_IN][KH][KW];
    #pragma HLS ARRAY_PARTITION variable=w_buf cyclic factor=UF dim=1
    #pragma HLS ARRAY_PARTITION variable=w_buf complete dim=2
    #pragma HLS ARRAY_PARTITION variable=w_buf complete dim=3
    #pragma HLS ARRAY_PARTITION variable=w_buf complete dim=4

    // ---------------- 1) โหลด weight (864 bytes, แป๊บเดียว) ----------------
    RD_W: for (int i = 0; i < N_W; i++) {
        #pragma HLS PIPELINE II=1
        int oc =  i / (C_IN*KH*KW);
        int r  =  i % (C_IN*KH*KW);
        int ic =  r / (KH*KW);
        int k  =  r % (KH*KW);
        w_buf[oc][ic][k/KW][k%KW] = weight[i];
    }

    // ---------------- 2) โหลด input (burst 150KB) ----------------
    RD_IN: for (int i = 0; i < N_IN; i++) {
        #pragma HLS PIPELINE II=1
        int ic =  i / (H_IN*W_IN);
        int r  =  i % (H_IN*W_IN);
        in_buf[ic][r/W_IN][r%W_IN] = input[i];
    }

    // ---------------- 3) คำนวณ + เขียน output กลับ DDR ----------------
    // output เขียนเรียง index -> HLS รวมเป็น burst ให้เอง
    OCO_LOOP: for (int oco = 0; oco < C_OUT/UF; oco++) {
        OH_LOOP: for (int oh = 0; oh < H_OUT; oh++) {
            OW_LOOP: for (int ow = 0; ow < W_OUT; ow++) {
                #pragma HLS PIPELINE II=1

                OCU_LOOP: for (int ocu = 0; ocu < UF; ocu++) {
                    #pragma HLS UNROLL
                    int oc = oco*UF + ocu;

                    int32_t acc = 0;
                    IC_LOOP: for (int ic = 0; ic < C_IN; ic++) {
                        #pragma HLS UNROLL
                        KH_LOOP: for (int kh = 0; kh < KH; kh++) {
                            #pragma HLS UNROLL
                            KW_LOOP: for (int kw = 0; kw < KW; kw++) {
                                #pragma HLS UNROLL
                                int ih = oh * STRIDE + kh - PAD;
                                int iw = ow * STRIDE + kw - PAD;
                                int32_t in_val = (ih >= 0 && ih < H_IN && iw >= 0 && iw < W_IN)
                                                 ? (int32_t)in_buf[ic][ih][iw]
                                                 : (int32_t)Z_IN;
                                acc += (in_val - Z_IN) * (int32_t)w_buf[oc][ic][kh][kw];
                            }
                        }
                    }
                    output[oc*(H_OUT*W_OUT) + oh*W_OUT + ow] = requantize(acc);
                }
            }
        }
    }
}
