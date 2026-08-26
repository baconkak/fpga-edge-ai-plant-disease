# Role 4: Embedded Software Engineering - Handover Document

เอกสารนี้สรุปโครงสร้างสถาปัตยกรรมปัจจุบัน (Current Architecture) และความสามารถของโค้ดฝั่ง Embedded Software (Role 4) สำหรับรันโมเดล MobileNetV2 ตรวจจับโรคพืช 31 คลาส บนบอร์ด PYNQ-Z2 เพื่อใช้ส่งต่องานให้แชทใหม่

---

## 1. สถาปัตยกรรมปัจจุบัน (HW/SW Co-Design Pipeline)

ระบบถูกออกแบบให้ทำงานร่วมกันระหว่าง **FPGA (Hardware Accelerator)** และ **ARM CPU (Software Fallback)** อย่างสมบูรณ์แบบ เพื่อให้ได้ความแม่นยำ 100% ตามโมเดลของ PyTorch

### ⚡ ส่วนที่รันบนชิป FPGA (`conv_engine_0` Bitstream V2)
FPGA ถูกใช้เร่งความเร็วเลเยอร์ที่เรารู้พารามิเตอร์ Quantization (`m_fix`) ครบถ้วน โดยระบบมี **Memory Tiling** เพื่อหั่นข้อมูลไม่ให้เกินขนาด BRAM 150 KB (`MAX_IN_BUF`) ของฮาร์ดแวร์:
* **Layer 0 (Standard Conv):** รันบน FPGA (`mode=0`) เต็มใบ เนื่องจากขนาดข้อมูล (150 KB) พอดีกับ BRAM
* **Block 1 Depthwise Conv:** รันบน FPGA (`mode=1`) โดยใช้ **Channel Chunking** ซอยส่งทีละแชนเนล
* **Block 1 Pointwise Conv:** รันบน FPGA (`mode=2`) โดยใช้ **Spatial & Output-Channel Tiling** ซอยส่งตามความสูงภาพ

### 🖥️ ส่วนที่รันบน ARM CPU (NumPy Vectorized)
เนื่องจากเราขาดข้อมูล **Activation Scale** จากฝั่ง AI (Role 1) สำหรับ Layer 2 ถึง 18 ทำให้ไม่สามารถคำนวณหาค่าตัวคูณ `m_fix` สำหรับวงจร INT8 บน FPGA ได้ 
* **Layer 2 ถึง 18 (Inverted Residuals):** ถูกส่งมารันบน CPU ทั้งหมดโดยคณิตศาสตร์ทศนิยม (Float32 NumPy) แบบ Fast Vectorized (`as_strided`) เพื่อหลีกเลี่ยงอาการ Overflow (ภาพล็อกค้าง)
* **Classifier:** รันบน CPU ด้วย Float32 เพื่อสรุปผลความน่าจะเป็น 31 คลาส

---

## 2. โครงสร้างไฟล์และหน้าที่ (File Structure)

โค้ดทั้งหมดอยู่ในโฟลเดอร์ `pynq-embedded/src/` และทำงานประสานกันดังนี้:

### `pynq_driver.py` (หัวใจหลักในการคุยกับ FPGA)
* คลาส `PlantDiseaseAccelerator`: จัดการ DMA Memory Buffers (`input_buf`, `output_buf`, `weight_buf`, `mfix_buf`) และเชื่อมต่อกับ `conv_engine_0`
* **ความสามารถหลัก:**
  * `run_layer0_fpga()`: คุยกับ AXI Register สั่งรัน Mode 0 (Standard) 
  * `run_depthwise_tiled()`: สับข้อมูลภาพ (Channel Chunking) ส่งไปรัน Mode 1 (Depthwise)
  * `run_pointwise_tiled()`: สับข้อมูลภาพ (Spatial Tiling) ส่งไปรัน Mode 2 (Pointwise)
  * จัดการ Cache Coherency (`flush`, `invalidate`) ป้องกัน Data ค้าง

### `model_pipeline.py` (ตัวประสานงาน Co-Design)
* คลาส `LayerDispatcher`: โหลดไฟล์น้ำหนัก (Weights) `.npy` ทั้ง 314 ไฟล์เข้า RAM
* **ความสามารถหลัก:**
  * `forward_full_model()`: รับภาพที่ผ่าน Preprocessing มาจ่ายงานต่อ
  * สั่งยิง Layer 0 และ Block 1 เข้า FPGA (เรียกใช้ Tiling จากไดรเวอร์)
  * วนลูป Layer 2 ถึง 18 ทำ Inverted Residual Blocks บนนอกฮาร์ดแวร์ผ่าน `_fast_depthwise` (ใช้ numpy strides) และ `_fast_pointwise` (einsum/matmul)

### `preprocessing.py`
* รับภาพจากกล้อง Resize เป็น $224 \times 224$
* แปลงเป็น Channel-First **CHW (3, 224, 224)** เพื่อให้หน่วยความจำเรียงติดกัน (Contiguous) ไม่ให้สีเพี้ยนตอนเข้า BRAM
* ทำ ImageNet Normalization และทำ **INT8 Quantization** (`Z_in = -2`, `Scale = 0.018`)

### `postprocessing.py`
* แปลง Output เป็น Softmax Probability
* แมปผลลัพธ์เข้ากับ `PLANT_DISEASE_CLASSES` ที่เรียงตัวอักษร 31 คลาส ตรงตาม PyTorch ImageFolder อย่างสมบูรณ์

---

## 3. ความต้องการข้อมูลเพิ่มเติมเพื่อการพัฒนาต่อ (Next Steps)

หากต้องการให้ระบบเร็วกว่านี้โดยการย้าย Layer 2 ถึง 18 ลงไปรันบน FPGA ทั้งหมด จะต้องขอความช่วยเหลือจากทีมอื่นดังนี้:

1. **ขอจากทีม AI (Role 1):** ไฟล์ `quantization_results.json` ต้องมีค่า **Scale ของ Activation (Feature Map)** ครบทั้งโครงข่าย ไม่ใช่มีแค่ Scale ของ Weights เพื่อให้ Role 4 นำมาคำนวณค่า `m_fix` สำหรับป้อนเข้า Register `0x34`
2. **ขอจากทีม HLS (Role 2):** เปลี่ยนตัวแปรใน `conv_engine.h` ให้ `#define MAX_IN_BUF (401408)` (ขนาด 401 KB) เพื่อรองรับ Layer ช่วงต้นได้ทั้งก้อน ช่วยลบ Software Tiling Overhead ออกจาก Python Driver ทั้งหมด
