import cv2
import numpy as np

def preprocess_image(image: np.ndarray, target_size=(224, 224), quant_scale=0.018, quant_zero_point=-2, to_chw=True) -> np.ndarray:
    """
    Preprocess image for FPGA CNN Accelerator (Channel-First CHW layout).
    
    Pipeline:
    1. Resize image to (224, 224)
    2. Convert BGR (OpenCV) -> RGB
    3. Normalize using ImageNet standard: mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]
    4. Quantize to INT8: x_q = round(x_norm / scale + zero_point)
    5. Transpose from HWC (224, 224, 3) to CHW (3, 224, 224) for FPGA BRAM layout
    
    Args:
        image: Original OpenCV image (BGR uint8)
        target_size: Target resolution tuple (224, 224)
        quant_scale: INT8 quantization scale factor (~0.018)
        quant_zero_point: Zero point offset (Z_IN = -2 matching Role 1 model)
        to_chw: If True, transpose to (3, 224, 224) for FPGA. If False, keep (224, 224, 3)
        
    Returns:
        np.ndarray of dtype np.int8 and shape (3, 224, 224) if to_chw=True else (224, 224, 3)
    """
    # 1. Resize
    img_resized = cv2.resize(image, target_size, interpolation=cv2.INTER_LINEAR)
    
    # 2. BGR to RGB
    img_rgb = cv2.cvtColor(img_resized, cv2.COLOR_BGR2RGB)
    
    # 3. Normalize (ImageNet)
    img_float = img_rgb.astype(np.float32) / 255.0
    mean = np.array([0.485, 0.456, 0.406], dtype=np.float32)
    std = np.array([0.229, 0.224, 0.225], dtype=np.float32)
    img_normalized = (img_float - mean) / std
    
    # 4. Quantize to INT8
    img_quantized = np.round(img_normalized / quant_scale + quant_zero_point)
    img_quantized = np.clip(img_quantized, -128, 127).astype(np.int8)
    
    # 5. Transpose to Channel-First (CHW: 3, 224, 224)
    if to_chw:
        img_quantized = np.ascontiguousarray(np.transpose(img_quantized, (2, 0, 1)))
        
    return img_quantized

def preprocess_frame(frame: np.ndarray) -> np.ndarray:
    """Convenience wrapper for camera frame with default CHW layout."""
    return preprocess_image(frame, to_chw=True)
