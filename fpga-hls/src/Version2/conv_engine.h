#ifndef CONV_ENGINE_H
#define CONV_ENGINE_H

#include <stdint.h>

// Maximum buffer sizes for BRAM allocation
// Sized to fit on PYNQ-Z2 (Zynq-7020 has ~630KB BRAM total)
#define MAX_IN_BUF  (150528) // 224x224x3 or up to 150KB
#define MAX_W_BUF   (36864)  // Weights max per layer tile
#define MAX_OUT_BUF (401408) // 112x112x32 max output
#define MAX_CHANNELS 1024    // Max channels for M_FIX array

void conv_engine(
    const int8_t* input,
    const int8_t* weight,
    int8_t*       output,
    const int32_t* m_fix,  // Array of size cout (Per-channel M_FIX)
    int mode,              // 0: Standard, 1: Depthwise, 2: Pointwise
    int cin, int hin, int win,
    int cout, int hout, int wout,
    int kh, int kw, int stride, int pad,
    int requant_shift, int8_t z_in, int8_t z_out
);

#endif
