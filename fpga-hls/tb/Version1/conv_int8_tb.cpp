// ============================================================================
//  conv_int8_tb.cpp  —  TESTBENCH v3 (pointer signature ตาม AXI)
//  เกณฑ์: tolerance +-2 (golden = quantized float, ไม่ใช่ true int8 inference)
// ============================================================================
#include <cstdio>
#include <cstdlib>
#include "conv_int8.h"

static bool load_bin(const char* path, int8_t* buf, int count) {
    FILE* f = fopen(path, "rb");
    if (!f) { printf("[ERROR] open fail: %s\n", path); return false; }
    fseek(f, 0, SEEK_END); long bytes = ftell(f); fseek(f, 0, SEEK_SET);
    if (bytes != (long)count) {
        printf("[ERROR] %s size %ld != expected %d\n", path, bytes, count);
        fclose(f); return false;
    }
    size_t got = fread(buf, 1, count, f);
    fclose(f);
    return (int)got == count;
}

static int8_t input [N_IN];
static int8_t weight[N_W];
static int8_t output[N_OUT];
static int8_t golden[N_OUT];

int main() {
    printf("=== HLS C-sim INT8 (AXI): conv features.0.0 ===\n");

    if (!load_bin("test_input_int8.bin",          input,  N_IN))  return 1;
    if (!load_bin("features_0_0_weight_int8.bin", weight, N_W))   return 1;
    if (!load_bin("test_output_int8.bin",         golden, N_OUT)) return 1;
    printf("[OK] loaded int8 files\n");

    conv2d_int8(input, weight, output);

    int  max_diff = 0, n_over = 0;
    const int TOL = 2;
    for (long i = 0; i < N_OUT; i++) {
        int d = output[i] - golden[i];
        if (d < 0) d = -d;
        if (d > max_diff) max_diff = d;
        if (d > TOL) n_over++;
    }
    printf("max diff = %d | เกิน tol(%d) = %d / %d\n", max_diff, TOL, n_over, N_OUT);

    if (n_over == 0) { printf(">>> PASS (within +-%d)\n", TOL); return 0; }
    printf(">>> FAIL\n"); return 1;
}
