// ==========================================
// Layer 1: Depthwise Convolution 3x3
// ==========================================
#define DW_CIN 32
#define DW_COUT 32
#define DW_HIN 4
#define DW_WIN 4
#define DW_HOUT 4
#define DW_WOUT 4
#define DW_KERNEL_SIZE 3
#define DW_STRIDE 1
#define DW_PADDING 1
#define DW_GROUPS 32

// ==========================================
// Layer 2: Pointwise Convolution 1x1
// ==========================================
#define PW_CIN 32
#define PW_COUT 16
#define PW_HIN 4
#define PW_WIN 4
#define PW_HOUT 4
#define PW_WOUT 4
#define PW_KERNEL_SIZE 1
#define PW_STRIDE 1
#define PW_PADDING 0