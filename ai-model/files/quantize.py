import numpy as np
import json

UP = '/mnt/user-data/uploads/'

def load_bin(path, dtype=np.float32):
    return np.fromfile(path, dtype=dtype)

def round_half_away_from_zero(x):
    """FPGA-friendly rounding: round(x) where ties go away from zero.
    Equivalent to floor(x+0.5) for x>=0 and ceil(x-0.5) for x<0.
    This is just a hardware-cheap symmetric rounding (no even/odd check)."""
    return np.sign(x) * np.floor(np.abs(x) + 0.5)

def quantize_per_tensor(w, qmin=-128, qmax=127, symmetric=True):
    """Per-tensor int8 quantization.
    symmetric=True -> zero_point fixed at 0 (typical for weights on FPGA, avoids
    an extra add at inference time).
    """
    if symmetric:
        max_abs = np.max(np.abs(w))
        if max_abs == 0:
            max_abs = 1e-8
        scale = max_abs / qmax
        zero_point = 0
    else:
        w_min, w_max = w.min(), w.max()
        scale = (w_max - w_min) / (qmax - qmin)
        if scale == 0:
            scale = 1e-8
        zero_point = qmin - round_half_away_from_zero(w_min / scale)
        zero_point = int(np.clip(zero_point, qmin, qmax))

    q = round_half_away_from_zero(w / scale) + zero_point
    q = np.clip(q, qmin, qmax).astype(np.int8)
    return q, scale, zero_point

def quantize_per_channel(w, axis=0, qmin=-128, qmax=127, symmetric=True):
    """Per-channel (per output-channel, axis=0 for conv weight (O,I,H,W)) int8 quant."""
    out_ch = w.shape[axis]
    w_moved = np.moveaxis(w, axis, 0).reshape(out_ch, -1)

    scales = np.zeros(out_ch, dtype=np.float64)
    zero_points = np.zeros(out_ch, dtype=np.int32)
    q_flat = np.zeros_like(w_moved, dtype=np.int8)

    for c in range(out_ch):
        wc = w_moved[c]
        if symmetric:
            max_abs = np.max(np.abs(wc))
            if max_abs == 0:
                max_abs = 1e-8
            scale = max_abs / qmax
            zp = 0
        else:
            wmin, wmax = wc.min(), wc.max()
            scale = (wmax - wmin) / (qmax - qmin)
            if scale == 0:
                scale = 1e-8
            zp = qmin - round_half_away_from_zero(wmin / scale)
            zp = int(np.clip(zp, qmin, qmax))

        qc = round_half_away_from_zero(wc / scale) + zp
        qc = np.clip(qc, qmin, qmax).astype(np.int8)

        scales[c] = scale
        zero_points[c] = zp
        q_flat[c] = qc

    q = q_flat.reshape([out_ch] + list(np.moveaxis(w, axis, 0).shape[1:]))
    q = np.moveaxis(q, 0, axis)
    return q, scales, zero_points

def dequantize_per_tensor(q, scale, zero_point):
    return (q.astype(np.float32) - zero_point) * scale

def dequantize_per_channel(q, scales, zero_points, axis=0):
    out_ch = q.shape[axis]
    q_moved = np.moveaxis(q, axis, 0).reshape(out_ch, -1).astype(np.float32)
    deq = np.zeros_like(q_moved)
    for c in range(out_ch):
        deq[c] = (q_moved[c] - zero_points[c]) * scales[c]
    deq = deq.reshape([out_ch] + list(np.moveaxis(q, axis, 0).shape[1:]))
    return np.moveaxis(deq, 0, axis)

def mse(a, b):
    return float(np.mean((a.astype(np.float64) - b.astype(np.float64)) ** 2))


# -------------------- Load all tensors --------------------
weight_files = {
    'features.0.0.weight': ('features_0_0_weight.bin', (32, 3, 3, 3)),
    'features.1.conv.0.0.weight': ('features_1_conv_0_0_weight.bin', (32, 1, 3, 3)),
    'features.1.conv.2.weight': ('features_1_conv_2_weight.bin', None),  # ambiguous shape, treat as 1D
    'features.0.1.bias': ('features_0_1_bias.npy', None),
    'features.0.1.running_var': ('features_0_1_running_var.bin', None),
    'classifier.1.bias': ('classifier_1_bias.bin', None),
}

results = {}

for name, (fname, shape) in weight_files.items():
    path = UP + fname
    if fname.endswith('.npy'):
        arr = np.load(path).astype(np.float32)
    else:
        arr = load_bin(path)
    if shape is not None:
        arr = arr.reshape(shape)

    entry = {'shape': list(arr.shape), 'n_elements': arr.size}

    # Per-tensor
    q_pt, scale_pt, zp_pt = quantize_per_tensor(arr)
    deq_pt = dequantize_per_tensor(q_pt, scale_pt, zp_pt)
    entry['per_tensor'] = {
        'scale': float(scale_pt),
        'zero_point': int(zp_pt),
        'mse': mse(arr, deq_pt),
        'max_abs_error': float(np.max(np.abs(arr - deq_pt))),
    }

    # Per-channel (only meaningful if ndim >=2 and shape[0] > 1, e.g. conv weights)
    if arr.ndim >= 2 and arr.shape[0] > 1:
        q_pc, scales_pc, zps_pc = quantize_per_channel(arr, axis=0)
        deq_pc = dequantize_per_channel(q_pc, scales_pc, zps_pc, axis=0)
        entry['per_channel'] = {
            'scale': scales_pc.tolist(),
            'zero_point': zps_pc.tolist(),
            'mse': mse(arr, deq_pc),
            'max_abs_error': float(np.max(np.abs(arr - deq_pc))),
        }
    else:
        entry['per_channel'] = 'N/A (1D tensor or single output channel - per-tensor only)'

    results[name] = entry

# -------------------- test_input / test_output (always per-tensor, activations) --------------------
test_input = load_bin(UP + 'test_input.bin').reshape(1, 3, 224, 224)
test_output = load_bin(UP + 'test_output.bin')

q_in, scale_in, zp_in = quantize_per_tensor(test_input, symmetric=False)
deq_in = dequantize_per_tensor(q_in, scale_in, zp_in)

q_out, scale_out, zp_out = quantize_per_tensor(test_output, symmetric=False)
deq_out = dequantize_per_tensor(q_out, scale_out, zp_out)

results['test_input'] = {
    'shape': list(test_input.shape),
    'per_tensor': {
        'scale': float(scale_in),
        'zero_point': int(zp_in),
        'mse': mse(test_input, deq_in),
        'max_abs_error': float(np.max(np.abs(test_input - deq_in))),
    }
}
results['test_output'] = {
    'shape': list(test_output.shape),
    'per_tensor': {
        'scale': float(scale_out),
        'zero_point': int(zp_out),
        'mse': mse(test_output, deq_out),
        'max_abs_error': float(np.max(np.abs(test_output - deq_out))),
    }
}

with open('/home/claude/quantization_results.json', 'w') as f:
    json.dump(results, f, indent=2)

print(json.dumps(results, indent=2))
