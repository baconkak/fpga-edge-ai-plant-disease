import numpy as np

# 31 Classes from Role 1 Dataset (Cabbage, Grape, Melon, Rice, Strawberry, Tea)
PLANT_DISEASE_CLASSES = [
    {"en": "Cabbage_Alternaria_Leaf_Spot", "th": "กะหล่ำปลี - โรคใบจุดอัลเทอร์นาเรีย"},
    {"en": "Cabbage_Bacterial_Soft_Rot", "th": "กะหล่ำปลี - โรคเน่าเละจากแบคทีเรีย"},
    {"en": "Cabbage_Balck_Rot", "th": "กะหล่ำปลี - โรคเน่าดำ"},
    {"en": "Cabbage_Downy_Mildew", "th": "กะหล่ำปลี - โรคราน้ำค้าง"},
    {"en": "Cabbage_Healthy", "th": "กะหล่ำปลี - แข็งแรงปกติ"},
    {"en": "Grape_Anthracnose", "th": "องุ่น - โรคแอนแทรคโนส"},
    {"en": "Grape_Black_Rot", "th": "องุ่น - โรคเน่าดำ"},
    {"en": "Grape_Downey_Mildew", "th": "องุ่น - โรคราน้ำค้าง"},
    {"en": "Grape_Gray_Mold", "th": "องุ่น - โรคราสีเทา"},
    {"en": "Grape_Healthy", "th": "องุ่น - แข็งแรงปกติ"},
    {"en": "Melon_Alternaria", "th": "เมล่อน - โรคใบจุดอัลเทอร์นาเรีย"},
    {"en": "Melon_Anthracnose", "th": "เมล่อน - โรคแอนแทรคโนส"},
    {"en": "Melon_Downy_Mildew", "th": "เมล่อน - โรคราน้ำค้าง"},
    {"en": "Melon_Healthy", "th": "เมล่อน - แข็งแรงปกติ"},
    {"en": "Melon_Powdery_Mildew", "th": "เมล่อน - โรคราแป้ง"},
    {"en": "Rice_Bacterial_Leaf_Blight", "th": "ข้าว - โรคขอบใบแห้ง"},
    {"en": "Rice_Blast", "th": "ข้าว - โรคไหม้"},
    {"en": "Rice_Brown_Spot", "th": "ข้าว - โรคใบจุดสีน้ำตาล"},
    {"en": "Rice_Healthy", "th": "ข้าว - แข็งแรงปกติ"},
    {"en": "Rice_Sheath_Blight", "th": "ข้าว - โรคกาบใบแห้ง"},
    {"en": "Straw_Angular", "th": "สตรอว์เบอร์รี - โรคใบจุดเหลี่ยม"},
    {"en": "Straw_Anthracnose", "th": "สตรอว์เบอร์รี - โรคแอนแทรคโนส"},
    {"en": "Straw_Gray_Mold", "th": "สตรอว์เบอร์รี - โรคราสีเทา"},
    {"en": "Straw_Healthy_Fruit", "th": "สตรอว์เบอร์รี - ผลแข็งแรงปกติ"},
    {"en": "Straw_Healthy_Leaf", "th": "สตรอว์เบอร์รี - ใบแข็งแรงปกติ"},
    {"en": "Straw_Powdery_Fruit", "th": "สตรอว์เบอร์รี - โรคราแป้งที่ผล"},
    {"en": "Straw_Powdery_Leaf", "th": "สตรอว์เบอร์รี - โรคราแป้งที่ใบ"},
    {"en": "Tea_Anthracnose", "th": "ชา - โรคแอนแทรคโนส"},
    {"en": "Tea_Blister_Blight", "th": "ชา - โรคใบพอง"},
    {"en": "Tea_Brown_Blight", "th": "ชา - โรคใบไหม้สีน้ำตาล"},
    {"en": "Tea_Healthy", "th": "ชา - แข็งแรงปกติ"}
]


def softmax(x: np.ndarray) -> np.ndarray:
    """Compute softmax values for each sets of scores in x."""
    # Ensure numerical stability by subtracting max
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum()

def process_output(output_buffer: np.ndarray):
    """
    Process the output from the FPGA + CPU classifier pipeline.
    
    Args:
        output_buffer: np.ndarray containing logits (shape may vary: 31 or 38)
        
    Returns:
        dict with class_id, class_name_en, class_name_th, confidence
    """
    logits = output_buffer.astype(np.float32)
    
    # Clamp to valid class range
    num_classes = min(len(logits), len(PLANT_DISEASE_CLASSES))
    logits = logits[:num_classes]
    
    probabilities = softmax(logits)
    class_id = int(np.argmax(probabilities))
    confidence = float(probabilities[class_id])
    
    if class_id < len(PLANT_DISEASE_CLASSES):
        class_info = PLANT_DISEASE_CLASSES[class_id]
    else:
        class_info = {"en": f"Unknown_{class_id}", "th": f"ไม่ทราบ_{class_id}"}
    
    return {
        "class_id": class_id,
        "class_name_en": class_info["en"],
        "class_name_th": class_info["th"],
        "confidence": confidence
    }
