#include <cstdio>
#include <cstdlib>
#include <cmath>
#if __has_include("../source/conv_engine.h")
#include "../source/conv_engine.h"
#elif __has_include("conv_engine.h")
#include "conv_engine.h"
#else
#include "../source/conv_engine.h"
#endif

static bool load_bin(const char* path, void* buf, int bytes) {
    FILE* f = fopen(path, "rb");
    if (!f) { printf("[ERROR] open fail: %s\n", path); return false; }
    fseek(f, 0, SEEK_END); long size = ftell(f); fseek(f, 0, SEEK_SET);
    if (size != (long)bytes) {
        printf("[ERROR] %s size %ld != expected %d\n", path, size, bytes);
        fclose(f); return false;
    }
    size_t got = fread(buf, 1, bytes, f);
    fclose(f);
    return (int)got == bytes;
}

static int8_t input [MAX_IN_BUF];
static int8_t weight[MAX_W_BUF];
static int8_t output[MAX_OUT_BUF];
static int8_t golden[MAX_OUT_BUF];
static int32_t m_fix_arr[MAX_CHANNELS];

// Calculate M_FIX and SHIFT to prevent int32 overflow
void calc_m_fix(float m, int32_t* m_fix, int* shift) {
    *shift = 31;
    while (m * (1LL << *shift) > 2147483647LL || m * (1LL << *shift) < -2147483648LL) {
        (*shift)--;
    }
    *m_fix = (int32_t)round(m * (1LL << *shift));
}

bool test_layer(const char* name, int mode, const char* in_path, const char* w_path, const char* out_path, const char* scale_path,
                const char* fp32_path, const char* mac_path,
                int cin, int hin, int win, int cout, int hout, int wout, int kh, int kw, int stride, int pad, int8_t z_in,
                int32_t fixed_m_fix = 0, int fixed_shift = 31, int8_t fixed_z_out = 0) {
    
    printf("\n=== Running Layer: %s (Mode %d) ===\n", name, mode);
    
    int n_in = cin * hin * win;
    int n_out = cout * hout * wout;
    int n_w = 0;
    if (mode == 0) n_w = cout * cin * kh * kw;
    else if (mode == 1) n_w = cout * kh * kw;
    else if (mode == 2) n_w = cout * cin;
    
    if (!load_bin(in_path, input, n_in)) return false;
    if (!load_bin(w_path, weight, n_w)) return false;
    if (!load_bin(out_path, golden, n_out)) return false;
    
    int32_t m_fix;
    int shift;
    int8_t z_out;

    if (scale_path != NULL) {
        float scale_zp[2];
        if (!load_bin(scale_path, scale_zp, 8)) return false;
        float s_out = scale_zp[0];
        z_out = (int8_t)scale_zp[1];
        
        float fp32_val[4096];
        int32_t mac_val[4096];
        float m_val = 1.0f;
        if (fp32_path && mac_path && load_bin(fp32_path, fp32_val, n_out * 4) && load_bin(mac_path, mac_val, n_out * 4)) {
            for (int i = 0; i < n_out; i++) {
                if (mac_val[i] != 0) {
                    float s_in_w = fp32_val[i] / (float)mac_val[i];
                    m_val = s_in_w / s_out;
                    break;
                }
            }
        } else {
            m_val = s_out;
        }
        calc_m_fix(m_val, &m_fix, &shift);
        printf("[INFO] S_out=%.4f, Derived M=%.6f -> m_fix=%d, shift=%d, z_out=%d\n", s_out, m_val, m_fix, shift, z_out);
    } else {
        m_fix = fixed_m_fix;
        shift = fixed_shift;
        z_out = fixed_z_out;
        printf("[INFO] Fixed M -> m_fix=%d, shift=%d, z_out=%d\n", m_fix, shift, z_out);
    }
    
    for (int i = 0; i < cout; i++) m_fix_arr[i] = m_fix;
    
    conv_engine(input, weight, output, m_fix_arr, mode, cin, hin, win, cout, hout, wout, kh, kw, stride, pad, shift, z_in, z_out);
    
    int max_diff = 0, n_over = 0;
    const int TOL = 2;
    for (long i = 0; i < n_out; i++) {
        int d = output[i] - golden[i];
        if (d < 0) d = -d;
        if (d > max_diff) max_diff = d;
        if (d > TOL) n_over++;
    }
    printf("max diff = %d | Over tol(%d) = %d / %d\n", max_diff, TOL, n_over, n_out);
    
    if (n_over == 0) {
        printf(">>> PASS: %s\n", name);
        return true;
    } else {
        printf(">>> FAIL: %s\n", name);
        return false;
    }
}

int main() {
    bool all_pass = true;
    
    // Layer 0: Standard Conv (features.0.0)
    all_pass &= test_layer("Standard Conv", 0,
               "d:/AllProject/conv/bin/standardconv/test_input_int8.bin",
               "d:/AllProject/conv/bin/standardconv/features_0_0_weight_int8.bin",
               "d:/AllProject/conv/bin/standardconv/test_output_int8.bin",
               NULL, NULL, NULL,
               3, 224, 224, 32, 112, 112, 3, 3, 2, 1, -2,
               10934627LL, 31, 7);

    // Layer 1: Depthwise Conv
    all_pass &= test_layer("Depthwise Conv", 1, 
               "d:/AllProject/conv/bin/depthwiseconv/dw_input_int8.bin", 
               "d:/AllProject/conv/bin/depthwiseconv/dw_weight_int8.bin", 
               "d:/AllProject/conv/bin/depthwiseconv/dw_output_expected_int8.bin", 
               "d:/AllProject/conv/bin/depthwiseconv/dw_output_scale_zp.bin",
               "d:/AllProject/conv/bin/depthwiseconv/dw_output_expected_fp32.bin",
               "d:/AllProject/conv/bin/depthwiseconv/dw_output_mac_int32.bin",
               32, 4, 4, 32, 4, 4, 3, 3, 1, 1, 0); 

    // Layer 2: Pointwise Conv
    all_pass &= test_layer("Pointwise Conv", 2, 
               "d:/AllProject/conv/bin/pointwiseconv/pw_input_int8.bin", 
               "d:/AllProject/conv/bin/pointwiseconv/pw_weight_int8.bin", 
               "d:/AllProject/conv/bin/pointwiseconv/pw_output_expected_int8.bin", 
               "d:/AllProject/conv/bin/pointwiseconv/pw_output_scale_zp.bin",
               "d:/AllProject/conv/bin/pointwiseconv/pw_output_expected_fp32.bin",
               "d:/AllProject/conv/bin/pointwiseconv/pw_output_mac_int32.bin",
               32, 4, 4, 16, 4, 4, 1, 1, 1, 0, 0); 
               
    if (all_pass) {
        printf("\nALL MODES PASSED!\n");
        return 0;
    } else {
        printf("\nSOME MODES FAILED!\n");
        return 1;
    }
}
