import os
import numpy as np
import time

try:
    from pynq import Overlay, allocate
    PYNQ_AVAILABLE = True
except ImportError:
    PYNQ_AVAILABLE = False
    print("Warning: 'pynq' module not found. Running in Hardware Mock Mode.")

try:
    from src.model_pipeline import LayerDispatcher
except ImportError:
    try:
        from model_pipeline import LayerDispatcher
    except ImportError:
        LayerDispatcher = None

class PlantDiseaseAccelerator:
    def __init__(self, bitstream_path="plant_disease.bit", weights_dir="../weights"):
        self.mode = "hardware" if PYNQ_AVAILABLE else "mock"
        
        # Candidate paths for bitstream
        candidate_paths = [
            bitstream_path,
            "../overlays/plant_disease.bit",
            "overlays/plant_disease.bit",
            "../overlays/v2/plant_disease.bit",
            "../bitstreams/v2/plant_disease.bit",
            "fpga-hardware/bitstreams/v2/plant_disease.bit"
        ]
        self.bitstream_path = bitstream_path
        for p in candidate_paths:
            if os.path.exists(p):
                self.bitstream_path = p
                break
                
        self.input_shape = (3, 224, 224)
        self.conv_out_shape = (32, 112, 112)
        self._last_input = np.zeros(self.input_shape, dtype=np.int8)
        self.is_v2_engine = False
        
        if self.mode == "hardware":
            self._init_hardware(weights_dir)
        else:
            self._init_mock()
            
        if LayerDispatcher is not None:
            self.dispatcher = LayerDispatcher(driver=self, weights_dir=weights_dir, num_classes=31)
        else:
            self.dispatcher = None
            
    def _init_hardware(self, weights_dir):
        """Initialize PYNQ Overlay and allocate DMA buffers for Tiled Execution."""
        try:
            print(f"Loading bitstream from: {self.bitstream_path}")
            self.overlay = Overlay(self.bitstream_path)
            
            if hasattr(self.overlay, "conv_engine_0"):
                self.accel = self.overlay.conv_engine_0
                self.is_v2_engine = True
                print("✅ Detected V2 Accelerator: conv_engine_0 (Tiling Enabled)")
            elif hasattr(self.overlay, "conv2d_int8_0"):
                self.accel = self.overlay.conv2d_int8_0
                self.is_v2_engine = False
                print("✅ Detected V1 Accelerator: conv2d_int8_0")
            else:
                first_key = list(self.overlay.ip_dict.keys())[0]
                self.accel = getattr(self.overlay, first_key)
                self.is_v2_engine = "conv_engine" in first_key.lower()
                print(f"Using detected IP block: {first_key}")
            
            # Allocate DMA buffers capable of holding full tensors (DDR side)
            # The FPGA BRAM side (in_buf 150KB, w_buf 36KB) is protected by the Tiling logic.
            self.input_buf = allocate(shape=(401408,), dtype=np.int8)
            self.output_buf = allocate(shape=(401408,), dtype=np.int8)
            self.weight_buf = allocate(shape=(409600,), dtype=np.int8)
            self.mfix_buf = allocate(shape=(1280,), dtype=np.int32)
            
            print("Hardware initialized successfully.")
        except Exception as e:
            print(f"Failed to initialize hardware: {e}")
            print("Falling back to mock mode.")
            self.mode = "mock"
            self._init_mock()
            
    def _init_mock(self):
        self.input_buf = np.zeros((401408,), dtype=np.int8)
        self.output_buf = np.zeros((401408,), dtype=np.int8)
        self.weight_buf = None
        self.mfix_buf = None
        print("Mock mode initialized.")
        
    def write_input(self, data: np.ndarray):
        if data.shape == (224, 224, 3):
            data = np.ascontiguousarray(np.transpose(data, (2, 0, 1)))
        np.copyto(self._last_input, data)

    def run_fpga_chunk(self, input_chunk, weight_chunk, mfix_val, mode, cin, hin, win, cout, hout, wout, kh, kw, stride, pad, requant_shift, z_in, z_out):
        """Low-level function: Executes a single chunk on FPGA that is guaranteed to fit in BRAM."""
        if not self.is_v2_engine:
            return np.zeros((cout, hout, wout), dtype=np.int8)
            
        n_in = input_chunk.size
        n_w = weight_chunk.size
        
        # Copy to DMA
        np.copyto(self.input_buf[:n_in], input_chunk.flatten().astype(np.int8))
        np.copyto(self.weight_buf[:n_w], weight_chunk.flatten().astype(np.int8))
        if isinstance(mfix_val, np.ndarray):
            np.copyto(self.mfix_buf[:cout], mfix_val.astype(np.int32))
        else:
            self.mfix_buf[:cout].fill(mfix_val)
        
        if hasattr(self.input_buf, 'flush'): self.input_buf.flush()
        if hasattr(self.weight_buf, 'flush'): self.weight_buf.flush()
        if hasattr(self.mfix_buf, 'flush'): self.mfix_buf.flush()
        
        # Write Registers
        self.accel.write(0x10, self.input_buf.physical_address)
        self.accel.write(0x1C, self.weight_buf.physical_address)
        self.accel.write(0x28, self.output_buf.physical_address)
        self.accel.write(0x34, self.mfix_buf.physical_address)
        self.accel.write(0x40, mode)
        self.accel.write(0x48, cin)
        self.accel.write(0x50, hin)
        self.accel.write(0x58, win)
        self.accel.write(0x60, cout)
        self.accel.write(0x68, hout)
        self.accel.write(0x70, wout)
        self.accel.write(0x78, kh)
        self.accel.write(0x80, kw)
        self.accel.write(0x88, stride)
        self.accel.write(0x90, pad)
        self.accel.write(0x98, requant_shift)
        self.accel.write(0xa0, z_in & 0xFF)
        self.accel.write(0xa8, z_out & 0xFF)
        
        # Execute
        self.accel.write(0x00, 1)
        t0 = time.time()
        while (self.accel.read(0x00) & 0x2) == 0:
            if time.time() - t0 > 0.4:
                print(f"FPGA Timeout! Mode:{mode} Cin:{cin} Hin:{hin}")
                break
                
        if hasattr(self.output_buf, 'invalidate'): self.output_buf.invalidate()
        
        return np.copy(self.output_buf[:cout * hout * wout]).reshape((cout, hout, wout))

    def run_layer0_fpga(self, input_chw: np.ndarray, weight_arr: np.ndarray) -> np.ndarray:
        """Layer 0 fits entirely in BRAM."""
        if self.mode == "hardware" and self.is_v2_engine:
            return self.run_fpga_chunk(
                input_chw, weight_arr, 10934627, mode=0,
                cin=3, hin=224, win=224, cout=32, hout=112, wout=112,
                kh=3, kw=3, stride=2, pad=1, requant_shift=31, z_in=254, z_out=7
            )
        else:
            time.sleep(0.003)
            return np.random.randint(-128, 127, self.conv_out_shape, dtype=np.int8)

    def run_depthwise_tiled(self, input_chw, weight, mfix_val=10934627, stride=1, pad=1):
        """Channel Chunking for Depthwise Conv (Mode 1)"""
        if self.mode != "hardware" or not self.is_v2_engine:
            return np.random.randint(-128, 127, (input_chw.shape[0], input_chw.shape[1], input_chw.shape[2]), dtype=np.int8)
            
        cin, hin, win = input_chw.shape
        cout = cin
        hout = (hin + 2 * pad - 3) // stride + 1
        wout = (win + 2 * pad - 3) // stride + 1
        
        output = np.zeros((cout, hout, wout), dtype=np.int8)
        
        # Max input BRAM is 150KB. 
        # Calculate safe channel chunk size
        spatial_size = hin * win
        chunk_size = max(1, 140000 // spatial_size)
        
        for c_start in range(0, cin, chunk_size):
            c_end = min(c_start + chunk_size, cin)
            c_len = c_end - c_start
            
            in_chunk = input_chw[c_start:c_end, :, :]
            w_chunk = weight[c_start:c_end, :, :, :]
            
            out_chunk = self.run_fpga_chunk(
                in_chunk, w_chunk, mfix_val, mode=1,
                cin=c_len, hin=hin, win=win, cout=c_len, hout=hout, wout=wout,
                kh=3, kw=3, stride=stride, pad=pad, requant_shift=31, z_in=0, z_out=0
            )
            output[c_start:c_end, :, :] = out_chunk
            
        return output

    def run_pointwise_tiled(self, input_chw, weight, mfix_val=10934627, cout=16):
        """Spatial & Output-Channel Tiling for Pointwise Conv (Mode 2)"""
        cin, hin, win = input_chw.shape
        hout, wout = hin, win
        
        if self.mode != "hardware" or not self.is_v2_engine:
             return np.random.randint(-128, 127, (cout, hout, wout), dtype=np.int8)
        
        output = np.zeros((cout, hout, wout), dtype=np.int8)
        
        # Max input size: 150KB -> Tiling Rows
        row_chunk_size = max(1, 140000 // (cin * win))
        # Max weight size: 36KB -> Tiling Output Channels
        cout_chunk_size = max(1, 35000 // cin)
        
        for h_start in range(0, hin, row_chunk_size):
            h_end = min(h_start + row_chunk_size, hin)
            h_len = h_end - h_start
            in_chunk = input_chw[:, h_start:h_end, :]
            
            for cout_start in range(0, cout, cout_chunk_size):
                cout_end = min(cout_start + cout_chunk_size, cout)
                cout_len = cout_end - cout_start
                w_chunk = weight[cout_start:cout_end, :, :, :]
                
                out_chunk = self.run_fpga_chunk(
                    in_chunk, w_chunk, mfix_val, mode=2,
                    cin=cin, hin=h_len, win=win, cout=cout_len, hout=h_len, wout=wout,
                    kh=1, kw=1, stride=1, pad=0, requant_shift=31, z_in=0, z_out=0
                )
                output[cout_start:cout_end, h_start:h_end, :] = out_chunk
                
        return output

    def run_inference(self):
        pass
        
    def read_output(self) -> np.ndarray:
        if self.dispatcher is not None:
            return self.dispatcher.forward_full_model(self._last_input)
        return np.zeros(31, dtype=np.float32)

    def cleanup(self):
        if self.mode == "hardware":
            self.input_buf.freebuffer()
            self.output_buf.freebuffer()
            if self.weight_buf is not None:
                self.weight_buf.freebuffer()
            if self.mfix_buf is not None:
                self.mfix_buf.freebuffer()
