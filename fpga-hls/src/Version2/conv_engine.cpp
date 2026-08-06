#include "conv_engine.h"

// ---------------- LOCAL BUFFERS (BRAM) ----------------
static int8_t in_buf[MAX_IN_BUF];
static int8_t w_buf[MAX_W_BUF];
static int32_t m_fix_buf[MAX_CHANNELS];

static int8_t requantize(int32_t acc, int32_t m_fix, int requant_shift, int8_t z_out) {
    int64_t prod = (int64_t)acc * m_fix;
    const int64_t half = 1LL << (requant_shift - 1);
    int32_t q;
    if (prod >= 0) q = (int32_t)( (prod + half) >> requant_shift );
    else           q = -(int32_t)( ((-prod) + half) >> requant_shift );
    q += z_out;
    if (q > 127)  q = 127;
    if (q < -128) q = -128;
    return (int8_t)q;
}

void conv_engine(
    const int8_t* input,
    const int8_t* weight,
    int8_t*       output,
    const int32_t* m_fix,
    int mode,
    int cin, int hin, int win,
    int cout, int hout, int wout,
    int kh, int kw, int stride, int pad,
    int requant_shift, int8_t z_in, int8_t z_out
) {
    // ---------------- AXI INTERFACES ----------------
    #pragma HLS INTERFACE m_axi     port=input  offset=slave bundle=gmem0 depth=150528
    #pragma HLS INTERFACE m_axi     port=weight offset=slave bundle=gmem1 depth=864
    #pragma HLS INTERFACE m_axi     port=output offset=slave bundle=gmem2 depth=401408
    #pragma HLS INTERFACE m_axi     port=m_fix  offset=slave bundle=gmem3 depth=32
    
    #pragma HLS INTERFACE s_axilite port=input  bundle=control
    #pragma HLS INTERFACE s_axilite port=weight bundle=control
    #pragma HLS INTERFACE s_axilite port=output bundle=control
    #pragma HLS INTERFACE s_axilite port=m_fix  bundle=control
    
    #pragma HLS INTERFACE s_axilite port=mode   bundle=control
    #pragma HLS INTERFACE s_axilite port=cin    bundle=control
    #pragma HLS INTERFACE s_axilite port=hin    bundle=control
    #pragma HLS INTERFACE s_axilite port=win    bundle=control
    #pragma HLS INTERFACE s_axilite port=cout   bundle=control
    #pragma HLS INTERFACE s_axilite port=hout   bundle=control
    #pragma HLS INTERFACE s_axilite port=wout   bundle=control
    #pragma HLS INTERFACE s_axilite port=kh     bundle=control
    #pragma HLS INTERFACE s_axilite port=kw     bundle=control
    #pragma HLS INTERFACE s_axilite port=stride bundle=control
    #pragma HLS INTERFACE s_axilite port=pad    bundle=control
    #pragma HLS INTERFACE s_axilite port=requant_shift bundle=control
    #pragma HLS INTERFACE s_axilite port=z_in   bundle=control
    #pragma HLS INTERFACE s_axilite port=z_out  bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // 1) Read weights
    int n_w = 0;
    if (mode == 0) n_w = cout * cin * kh * kw;
    else if (mode == 1) n_w = cout * kh * kw; 
    else if (mode == 2) n_w = cout * cin;

    RD_W: for (int i = 0; i < n_w; i++) {
        #pragma HLS PIPELINE II=1
        w_buf[i] = weight[i];
    }

    // 2) Read M_FIX array (Per-channel quantization)
    RD_MFIX: for (int i = 0; i < cout; i++) {
        #pragma HLS PIPELINE II=1
        m_fix_buf[i] = m_fix[i];
    }

    // 3) Read input
    int n_in = cin * hin * win;
    RD_IN: for (int i = 0; i < n_in; i++) {
        #pragma HLS PIPELINE II=1
        in_buf[i] = input[i];
    }

    // 4) Compute based on mode
    int hw_in = hin * win;
    int hw_out = hout * wout;
    int kh_kw = kh * kw;
    int c_kh_kw = cin * kh_kw;

    if (mode == 0) {
        // ==========================================
        // STANDARD CONVOLUTION
        // ==========================================
        for (int oc = 0; oc < cout; oc++) {
            int out_oc_base = oc * hw_out;
            int w_oc_base = oc * c_kh_kw;
            for (int oh = 0; oh < hout; oh++) {
                int oh_wout = oh * wout;
                int oh_stride = oh * stride - pad;
                for (int ow = 0; ow < wout; ow++) {
                    int32_t acc = 0;
                    int ow_stride = ow * stride - pad;
                    for (int ic = 0; ic < cin; ic++) {
                        int in_ic_base = ic * hw_in;
                        int w_ic_base = w_oc_base + ic * kh_kw;
                        for (int k_h = 0; k_h < kh; k_h++) {
                            int ih = oh_stride + k_h;
                            bool ih_valid = (ih >= 0 && ih < hin);
                            int in_ih_base = in_ic_base + ih * win;
                            int w_kh_base = w_ic_base + k_h * kw;
                            for (int k_w = 0; k_w < kw; k_w++) {
                                #pragma HLS PIPELINE II=1
                                int iw = ow_stride + k_w;
                                int32_t in_val = z_in;
                                if (ih_valid && iw >= 0 && iw < win) {
                                    in_val = in_buf[in_ih_base + iw];
                                }
                                int32_t w_val = w_buf[w_kh_base + k_w];
                                acc += (in_val - z_in) * w_val;
                            }
                        }
                    }
                    output[out_oc_base + oh_wout + ow] = requantize(acc, m_fix_buf[oc], requant_shift, z_out);
                }
            }
        }
    } else if (mode == 1) {
        // ==========================================
        // DEPTHWISE CONVOLUTION (Groups = Channels)
        // ==========================================
        for (int c = 0; c < cout; c++) {
            int out_c_base = c * hw_out;
            int in_c_base = c * hw_in;
            int w_c_base = c * kh_kw;
            for (int oh = 0; oh < hout; oh++) {
                int oh_wout = oh * wout;
                int oh_stride = oh * stride - pad;
                for (int ow = 0; ow < wout; ow++) {
                    int32_t acc = 0;
                    int ow_stride = ow * stride - pad;
                    for (int k_h = 0; k_h < kh; k_h++) {
                        int ih = oh_stride + k_h;
                        bool ih_valid = (ih >= 0 && ih < hin);
                        int in_ih_base = in_c_base + ih * win;
                        int w_kh_base = w_c_base + k_h * kw;
                        for (int k_w = 0; k_w < kw; k_w++) {
                            #pragma HLS PIPELINE II=1
                            int iw = ow_stride + k_w;
                            int32_t in_val = z_in;
                            if (ih_valid && iw >= 0 && iw < win) {
                                in_val = in_buf[in_ih_base + iw];
                            }
                            int32_t w_val = w_buf[w_kh_base + k_w];
                            acc += (in_val - z_in) * w_val;
                        }
                    }
                    output[out_c_base + oh_wout + ow] = requantize(acc, m_fix_buf[c], requant_shift, z_out);
                }
            }
        }
    } else if (mode == 2) {
        // ==========================================
        // POINTWISE CONVOLUTION (1x1)
        // ==========================================
        for (int oc = 0; oc < cout; oc++) {
            int out_oc_base = oc * hw_out;
            int w_oc_base = oc * cin;
            for (int oh = 0; oh < hout; oh++) {
                int oh_wout = oh * wout;
                int in_oh_base = oh * win;
                for (int ow = 0; ow < wout; ow++) {
                    int32_t acc = 0;
                    int in_hw_offset = in_oh_base + ow;
                    for (int ic = 0; ic < cin; ic++) {
                        #pragma HLS PIPELINE II=1
                        int32_t in_val = in_buf[ic * hw_in + in_hw_offset];
                        int32_t w_val = w_buf[w_oc_base + ic];
                        acc += (in_val - z_in) * w_val;
                    }
                    output[out_oc_base + oh_wout + ow] = requantize(acc, m_fix_buf[oc], requant_shift, z_out);
                }
            }
        }
    }
}
