# 🎓 FPGA-Accelerated Edge AI System
## Project Proposal สำหรับยื่นทุน JAIST Scholarship

> **เอกสารนี้คืออะไร:** สรุปไอเดียโปรเจค, การแบ่งงาน, ไทม์ไลน์, และความรู้พื้นฐานทั้งหมด สำหรับทีม 5 คน ใช้พรีเซนต์ + อ้างอิงตลอดโปรเจค

---

## 📋 สารบัญ

1. [ภาพรวมโปรเจค](#1-ภาพรวมโปรเจค)
2. [ทำไมต้องเป็นโปรเจคนี้](#2-ทำไมต้องเป็นโปรเจคนี้)
3. [ความรู้พื้นฐาน FPGA + AI](#3-ความรู้พื้นฐาน-fpga--ai)
4. [สถาปัตยกรรมระบบ](#4-สถาปัตยกรรมระบบ)
5. [การแบ่งงาน 5 คน](#5-การแบ่งงาน-5-คน)
6. [ไทม์ไลน์ 12 สัปดาห์](#6-ไทม์ไลน์-12-สัปดาห์)
7. [งบประมาณและฮาร์ดแวร์](#7-งบประมาณและฮาร์ดแวร์)
8. [แหล่งเรียนรู้แต่ละ Role](#8-แหล่งเรียนรู้แต่ละ-role)
9. [คำศัพท์สำคัญ](#9-คำศัพท์สำคัญ)
10. [เคล็ดลับสู่ความสำเร็จ](#10-เคล็ดลับสู่ความสำเร็จ)
11. [ขั้นตอนถัดไป](#11-ขั้นตอนถัดไป)

---

## 1. ภาพรวมโปรเจค

### Project Name
**"FPGA-Accelerated Real-time Plant Disease Detection for Smart Agriculture"**

### One-Line Pitch
ระบบ Edge AI ที่ใช้ FPGA เร่งความเร็วการตรวจจับโรคพืชแบบเรียลไทม์ สำหรับการเกษตรอัจฉริยะ

### เป้าหมาย
1. ทำโปรเจคให้สำเร็จใน 3 เดือน (ช่วงปิดเทอมก่อนขึ้นปี 3 เทอม 2)
2. ใช้ยื่นทุน **JAIST** (Japan Advanced Institute of Science and Technology)
3. ได้ประสบการณ์ครบทั้ง FPGA + AI + IoT + Embedded Systems

### Deliverables ที่จะส่ง

| Item | รายละเอียด |
|---|---|
| 🖥️ Working Demo | ระบบที่ทำงานได้จริงบนฮาร์ดแวร์ (หรือ simulation ที่สมบูรณ์) |
| 📊 Performance Report | เปรียบเทียบ latency, FPS, power กับ CPU baseline |
| 💾 GitHub Repository | โค้ดทั้งหมด + README + architecture diagram |
| 📄 Technical Report | สไตล์ paper academic 10-15 หน้า |
| 🎥 Demo Video | 2-3 นาที โชว์ระบบทำงานจริง |
| 🎤 Presentation Deck | สไลด์สำหรับ pitch JAIST |

---

## 2. ทำไมต้องเป็นโปรเจคนี้

### 2.1 ทำไมโปรเจคนี้ขายดีกับ JAIST?

JAIST เป็นมหาวิทยาลัยวิจัย เน้น "ความคิด + ระเบียบวิธีวิจัย" มากกว่าผลงานสวย ๆ อย่างเดียว โปรเจคนี้ตรงกับเกณฑ์ที่ดี:

- ✅ **Smart Agriculture** = ทิศทางวิจัยใหญ่ของญี่ปุ่น (เพราะสังคมสูงวัย + ขาดเกษตรกร)
- ✅ **Edge AI + FPGA** = หัวข้อ hot ที่ JAIST วิจัยจริง (มี lab รองรับ)
- ✅ **Thailand-relevant** = พูดเรื่อง social impact ได้แท้จริง (เกษตรกรไทย)
- ✅ **Multi-disciplinary** = ผสม AI + Hardware + IoT + Software
- ✅ **Quantifiable Results** = วัดผลเป็นตัวเลขได้ชัดเจน (latency, FPS, power)

### 2.2 ทำไมต้อง FPGA ไม่ใช้ Raspberry Pi หรือ GPU?

**คำถามที่กรรมการ JAIST จะถามแน่ ๆ** — เราต้องตอบได้:

| ปัจจัย | CPU/Raspberry Pi | GPU | **FPGA** |
|---|---|---|---|
| Latency | สูง (10-100ms) | กลาง (5-20ms) | ✅ ต่ำมาก (<5ms) |
| Power | กลาง (5W) | สูง (50-300W) | ✅ ต่ำ (2-5W) |
| Parallelism | จำกัด | สูงมาก | ✅ ปรับแต่งได้ตามต้องการ |
| Customizable | ไม่ได้ | ไม่ได้ | ✅ ออกแบบวงจรเฉพาะงานได้ |
| Cost (production) | ถูก | แพง | ✅ ปานกลาง |

**สรุปการขาย:** "ในไร่ที่ไม่มีไฟฟ้าเสถียร ใช้แบตเตอรี่/โซลาร์เซลล์ → FPGA กินไฟต่ำ + ตอบสนองเร็ว = เหมาะที่สุด"

### 2.3 ขอบเขตที่ทำได้จริงใน 3 เดือน

- รัน **MobileNet** หรือ **YOLO-nano** ที่ quantize เป็น INT8 แล้ว
- Accelerate เฉพาะ **Convolution layers** บน FPGA (ไม่ใช่ทั้งโมเดล)
- รับภาพจาก USB camera → ประมวลผล → แสดงผ่าน Web Dashboard
- เปรียบเทียบกับ baseline บน CPU/Raspberry Pi
- **ไม่ใช่:** สร้าง CNN ทั้งโมเดลบน FPGA จากศูนย์ (ทำไม่ทันใน 3 เดือน)

---

## 3. ความรู้พื้นฐาน FPGA + AI

### 3.1 FPGA คืออะไรกันแน่?

**เปรียบเทียบ CPU vs FPGA:**

| | CPU | FPGA |
|---|---|---|
| ทำงานยังไง | รันคำสั่งทีละบรรทัด (เหมือนเชฟอ่านสูตรอาหาร) | เป็นวงจรที่คุณออกแบบเอง (เหมือนสายพานโรงงาน) |
| เปลี่ยนงาน | เปลี่ยนโปรแกรม | "ทอวงจรใหม่" ผ่าน bitstream |
| ความเร็ว | ทำได้ทุกอย่างแต่ทีละนิด | ทำงานเฉพาะทาง แต่ขนาน 100-1000 ตัวพร้อมกัน |
| เหมาะกับ | งานหลากหลาย, control logic | งานซ้ำ ๆ ที่ต้องเร็ว เช่น AI inference, DSP |

### 3.2 ทำไม AI เหมาะกับ FPGA?

Neural Network คือการ **คูณ + บวก** ตัวเลขซ้ำ ๆ เป็นล้านครั้ง:
- **CPU:** ทำทีละครั้ง (หรือ 8 ตัวพร้อมกันด้วย SIMD)
- **FPGA:** สร้างตัวคูณ 500-1000 ตัวให้ทำพร้อมกันได้

ผลคือ **เร็วกว่า 10-50 เท่า** สำหรับ task ที่เหมาะสม

### 3.3 PYNQ Board มีอะไร?

PYNQ ใช้ชิป **Zynq** ที่รวม 2 ส่วนในตัวเดียว:

```
┌─────────────────────────────────────────┐
│              Zynq SoC Chip              │
│                                         │
│  ┌──────────────┐    ┌──────────────┐   │
│  │   ARM CPU    │ ←→ │ FPGA Fabric  │   │
│  │ (Dual-core)  │    │ (Programmable│   │
│  │ Linux+Python │    │   Logic)     │   │
│  └──────┬───────┘    └──────┬───────┘   │
│         │      AXI Bus      │           │
│         └────────┬──────────┘           │
│                  ↕                      │
│         DDR Memory (Shared)             │
└─────────────────────────────────────────┘
        ↑                       ↑
   USB/Ethernet            HDMI/GPIO
```

- **ARM CPU** = คอมเล็ก ๆ รัน Linux ได้ เขียน Python ได้ปกติ
- **FPGA Fabric** = พื้นที่ว่างให้ออกแบบวงจรเร่งความเร็ว
- **DDR RAM** = หน่วยความจำที่ทั้ง ARM และ FPGA แชร์กัน
- **AXI Bus** = ทางหลวงเชื่อม ARM กับ FPGA (มาตรฐาน ARM)

---

## 4. สถาปัตยกรรมระบบ

### 4.1 Data Flow ของระบบ

```
┌────────┐    ┌──────────────┐    ┌──────────────┐    ┌──────────────┐    ┌────────────┐
│ กล้อง   │ →  │ Preprocess   │ →  │  CNN Accel   │ →  │ Postprocess  │ →  │ Dashboard  │
│  USB   │    │ Python (ARM) │    │  FPGA (HLS)  │    │ Python (ARM) │    │   Web UI   │
│ คนที่ 4 │    │   คนที่ 4    │    │   คนที่ 2    │    │   คนที่ 4    │    │  คนที่ 5   │
└────────┘    └──────────────┘    └──────┬───────┘    └──────────────┘    └────────────┘
                                         ↑
                              ┌──────────┴──────────┐
                              │  AI Model Weights   │
                              │   Trained CNN (INT8)│
                              │       คนที่ 1       │
                              └─────────────────────┘

       └──────────────── คนที่ 3 (AXI + Block Integration) ─────────────┘
```

### 4.2 ขั้นตอนการทำงานจริง (Step-by-step)

1. **กล้อง USB ถ่ายภาพใบไม้** → ส่งเป็น raw image
2. **ARM (Python)** รับภาพ → resize, normalize, แปลงให้พร้อมเข้า CNN
3. **ARM ส่งภาพไปยัง FPGA** ผ่าน AXI Bus → เก็บใน DDR/BRAM
4. **FPGA Accelerator** คำนวณ convolution layers แบบขนาน → ได้ feature map
5. **FPGA ส่งผลลัพธ์กลับ ARM** ผ่าน AXI
6. **ARM (Python)** ทำ postprocess (softmax, argmax) → ได้ class
7. **Web Dashboard** แสดงผล "ตรวจพบโรคใบไหม้ 87% confidence"

---

## 5. การแบ่งงาน 5 คน

### 👤 คนที่ 1 — AI Model Engineer

**สโลแกน:** "คนเทรนสมอง AI"

**งานหลัก:**
- หา/สร้าง dataset (เช่น PlantVillage มี 50,000+ รูป โรคพืช 38 class)
- Train โมเดล CNN ด้วย PyTorch บน Google Colab (มี GPU ฟรี)
- **Quantization:** แปลงโมเดลจาก float-32 เป็น int-8 → เล็กลง 4 เท่า เร็วขึ้น 4 เท่า
- Export weights ในรูปแบบที่ FPGA ใช้ได้ (ไฟล์ .bin หรือ .npy)
- Validate ว่า quantized model accuracy ไม่ตกเกิน 2% จาก baseline

**ทักษะที่ต้องมี:**
- Python (ใช้คล่อง)
- PyTorch หรือ TensorFlow
- พื้นฐาน Machine Learning, CNN

**ทักษะที่ต้องเรียนเพิ่ม:**
- Model quantization (PTQ, QAT)
- ONNX format
- การ profile โมเดล

**Deliverable ส่งให้ใคร:**
- ส่ง weights file + layer spec → **คนที่ 2** (ใช้ออกแบบ accelerator)
- ส่ง preprocessing requirements → **คนที่ 4** (ใช้เขียน Python pipeline)

**ใช้เครื่องมือ:** PyTorch, Google Colab, ONNX, Netron (สำหรับ visualize model)

---

### 👤 คนที่ 2 — FPGA HLS Engineer

**สโลแกน:** "คนสร้างเครื่องคิดเลขความเร็วสูง"

**งานหลัก:**
- เขียน Convolution accelerator IP ใน **Vivado HLS** (เขียนเป็น C++ → tool แปลงเป็นวงจร)
- ใส่ pragma เพื่อ optimize: `#pragma HLS PIPELINE`, `#pragma HLS UNROLL`, `#pragma HLS DATAFLOW`
- ทดสอบด้วย C-simulation (รันใน PC ปกติได้เลย)
- Optimize ให้ใช้ทรัพยากร FPGA (LUT, DSP, BRAM) คุ้มค่าและเร็ว
- Export เป็น **IP block** ที่ Vivado ใช้ได้

**ทักษะที่ต้องมี:**
- C/C++ (เข้าใจ loop, array, pointer)
- พื้นฐาน Computer Architecture (memory, pipeline)

**ทักษะที่ต้องเรียนเพิ่ม:**
- Vivado HLS pragma
- Loop transformations (unrolling, pipelining)
- Fixed-point arithmetic
- AXI Stream/Memory-Mapped interface

**ตัวอย่างโค้ด HLS:**
```cpp
void conv2d(int8_t input[28][28], int8_t weight[3][3], int output[26][26]) {
    #pragma HLS PIPELINE
    for (int i = 0; i < 26; i++)
        for (int j = 0; j < 26; j++) {
            int sum = 0;
            #pragma HLS UNROLL
            for (int ki = 0; ki < 3; ki++)
                for (int kj = 0; kj < 3; kj++)
                    sum += input[i+ki][j+kj] * weight[ki][kj];
            output[i][j] = sum;
        }
}
```

**Deliverable ส่งให้ใคร:**
- ส่ง IP block (.zip) ที่ผ่าน C-sim แล้ว → **คนที่ 3** (ใช้ integrate)

**ใช้เครื่องมือ:** Vivado HLS / Vitis HLS

---

### 👤 คนที่ 3 — Hardware Integration Engineer

**สโลแกน:** "คนต่อสายไฟทั้งระบบ"

**งานหลัก:**
- ใช้ **Vivado Block Design** ลากบล็อกต่อกัน (ARM, DMA, Accelerator IP, Memory)
- ตั้งค่า **AXI Interconnect** — กำหนด address space, clock, reset
- จัดการ memory mapping (ที่ไหนเก็บอะไร)
- กด **Generate Bitstream** → ได้ไฟล์ .bit (configuration file ของ FPGA)
- Debug timing issues, address conflicts

**Block diagram ตัวอย่าง:**
```
[ARM CPU]──┬──[AXI Smart-     ──[DMA]──┬──[Accelerator IP]──[BRAM]
           │   Connect]                │
           │                           │
           └──[DDR Controller]─────────┴──[DDR Memory]
```

**ทักษะที่ต้องมี:**
- เข้าใจ Computer Architecture (bus, memory, address)
- พื้นฐาน Digital Design

**ทักษะที่ต้องเรียนเพิ่ม:**
- AXI4 protocol (Full, Lite, Stream)
- Vivado workflow
- Memory mapping & address space
- Timing closure

**Deliverable ส่งให้ใคร:**
- ส่งไฟล์ .bit (bitstream) + .hwh (hardware definition) → **คนที่ 4**

**ใช้เครื่องมือ:** Vivado (IDE หลัก)

---

### 👤 คนที่ 4 — Embedded Software / PYNQ Engineer

**สโลแกน:** "คนเชื่อม Python กับฮาร์ดแวร์"

**งานหลัก:**
- Boot Linux บน PYNQ board ผ่าน SD card
- เขียน **PYNQ Overlay** = Python wrapper ที่คุย FPGA ได้
- รับภาพจากกล้อง USB ผ่าน OpenCV
- Preprocessing: resize, normalize, channel conversion
- ส่งภาพไป FPGA → รอผล → Postprocessing
- สร้าง Python API ที่ใช้งานง่าย ส่งต่อให้คนที่ 5

**ตัวอย่างโค้ด Python:**
```python
from pynq import Overlay, allocate
import numpy as np
import cv2

# โหลด bitstream จากคนที่ 3
ol = Overlay("plant_disease.bit")
accel = ol.cnn_accel

# จัดสรร memory buffer
input_buf = allocate(shape=(224, 224, 3), dtype=np.int8)
output_buf = allocate(shape=(38,), dtype=np.int32)

# Capture และ preprocess
frame = cv2.VideoCapture(0).read()[1]
input_buf[:] = preprocess(frame)

# เรียก FPGA accelerator
accel.write_input(input_buf.physical_address)
accel.write_output(output_buf.physical_address)
accel.start()
accel.wait_done()

# ตีความผล
disease_class = np.argmax(output_buf)
print(f"ตรวจพบ: {DISEASE_NAMES[disease_class]}")
```

**ทักษะที่ต้องมี:**
- Python (ใช้คล่อง)
- พื้นฐาน Linux command line

**ทักษะที่ต้องเรียนเพิ่ม:**
- PYNQ framework (pynq.io)
- OpenCV
- Memory-mapped I/O
- Hardware-software interface

**Deliverable ส่งให้ใคร:**
- ส่ง Python API + integration test → **คนที่ 5** (ใช้ทำ dashboard)

**ใช้เครื่องมือ:** PYNQ, Jupyter Notebook, OpenCV

---

### 👤 คนที่ 5 — System / Demo / Project Manager

**สโลแกน:** "คนทำให้ทุกอย่างมาบรรจบ + พรีเซนต์ได้"

**งานหลัก:**
- สร้าง **Web Dashboard** (Flask/FastAPI + HTML/JS หรือ React)
  - แสดงภาพจากกล้อง + ผล detection แบบเรียลไทม์
  - กราฟ confidence, latency, FPS
  - ปุ่มควบคุม start/stop, save log
- จัด Git workflow, code review schedule
- **Performance Benchmark:**
  - วัด latency, FPS, power consumption
  - เปรียบเทียบ FPGA vs CPU-only baseline
- เขียน Technical Report (paper-style)
- ทำ Demo Video 2-3 นาที
- ทำ Presentation Slides สำหรับ pitch JAIST

**ทักษะที่ต้องมี:**
- Full-stack basics (HTML/CSS/JS + backend)
- Technical writing
- Project coordination

**ทักษะที่ต้องเรียนเพิ่ม:**
- Flask หรือ FastAPI
- WebSocket (สำหรับ real-time update)
- LaTeX (สำหรับ report สวย ๆ)

**Deliverable:**
- Web dashboard ที่ run ได้
- Technical report
- Demo video
- Presentation deck

**ใช้เครื่องมือ:** VS Code, Flask/FastAPI, OBS Studio, Canva/PowerPoint, LaTeX

> **หมายเหตุสำคัญ:** คนที่ 5 ไม่ใช่ "คนเหลือ" แต่เป็น role ที่ต้องการคนสื่อสารเก่งและจัดการเป็น เพราะมักจะกลายเป็น project lead โดยปริยาย

---

## 6. ไทม์ไลน์ 12 สัปดาห์

### 📅 Month 1: Foundation (Week 1-4) — เรียน + Prototype เล็ก ๆ

#### Week 1: Group Bootcamp 🚨 (สำคัญมาก ห้ามข้าม)
**ทำพร้อมกันทั้งทีม:**
- เรียน FPGA basics, Zynq architecture, Vivado workflow ด้วยกัน
- ดู YouTube playlist (Mohammad Sadri, Adam Taylor)
- อ่าน paper อย่างน้อย 3 ฉบับ search: "CNN FPGA accelerator", "PYNQ object detection"
- **Lock scope:** ตัดสินใจ dataset และ application ให้แน่นอน
- Setup environment ทุกคน: Vivado, PYNQ image, Git
- สั่งบอร์ดถ้ายังไม่มี (PYNQ-Z2 หรือยืม)

#### Week 2-4: Parallel Ramp-up
**แต่ละคนเริ่มงานตัวเองพร้อมกัน:**

| คน | งาน Week 2 | งาน Week 3 | งาน Week 4 |
|---|---|---|---|
| 1 | หา dataset, EDA | Train baseline CNN | Validate accuracy >85% |
| 2 | Tutorial HLS | HLS conv layer ผ่าน C-sim | Optimize ด้วย pragma |
| 3 | Tutorial Vivado | Blink LED จาก Zynq | AXI GPIO sample |
| 4 | Boot PYNQ | Capture camera | Basic overlay test |
| 5 | Setup Git/CI | Mock dashboard | Architecture diagram |

**🎯 End of Month 1 Milestone:** ทุกคน demo อะไรได้อย่างน้อย 1 อย่างในโดเมนตัวเอง

---

### 📅 Month 2: Component Development (Week 5-8) — สร้างของจริง

#### Week 5-6
| คน | งาน |
|---|---|
| 1 | Quantize โมเดล, export weights |
| 2 | สร้าง HLS IP เต็มฟังก์ชัน + tune throughput |
| 3 | Block design + integrate accelerator IP ครั้งแรก |
| 4 | เขียน Python wrapper ที่ส่งข้อมูลให้ IP ได้ |
| 5 | Setup benchmark framework, logging |

**🔥 Week 6 Checkpoint:** First Integration Test
- ลอง run **1 conv layer end-to-end** ผ่านทั้ง stack
- ถ้าพังตรงไหน fix ก่อนเดินหน้า

#### Week 7-8
- ทุกคน push ให้ full pipeline ทำงานได้
- **🎯 End of Month 2 Milestone:** End-to-end demo ทำงานได้ "อย่างน้อย 1 ครั้ง" (อาจช้า/แม่นน้อยก็ไม่เป็นไร)

---

### 📅 Month 3: Integration & Polish (Week 9-12)

#### Week 9-10: Optimization
- ลด latency, เพิ่ม FPS
- เปรียบเทียบกับ baseline (CPU-only, Raspberry Pi)
- วัด power consumption จริง

#### Week 11: Testing & Demo
- ทดสอบในสภาพแสงต่าง ๆ, ของจริง
- บันทึก demo video

#### Week 12: Documentation & Presentation
- เขียน technical report
- ทำ presentation slides
- ซ้อม presentation
- Polish GitHub (README, architecture diagram, results)

**🎯 Final Milestone:** ส่งโปรเจคพร้อมยื่นทุน JAIST

---

## 7. งบประมาณและฮาร์ดแวร์

### 7.1 ทางเลือกบอร์ด FPGA (เรียงตามความน่าทำ)

#### 🆓 ทางเลือก A: ยืมจากภาควิชา/อาจารย์ ⭐ ทำก่อนเสมอ
- CPE มหิดลต้องมีบอร์ดในแล็บแน่นอน
- ไปคุยกับอาจารย์ Digital Design / Computer Architecture
- โบนัส: อาจได้ที่ปรึกษา + recommendation letter

#### 🆓 ทางเลือก B: Intel FPGA DevCloud
- ฟรี เข้าได้ออนไลน์
- ใช้ Stratix 10 / Agilex (แรงกว่า PYNQ มาก)
- ข้อจำกัด: ไม่มี physical demo ต้องโชว์ผ่าน log

#### 🆓 ทางเลือก C: Simulation-only
- ใช้ Verilator + Vivado Simulator (ฟรี)
- ได้ตัวเลข timing, resource, power จาก synthesis report
- JAIST รับงาน simulation-based เป็นปกติ

#### 💸 ทางเลือก D: Tang Nano 20K (~1,200 บาท)
- 5 คนหาร = คนละ 240 บาท
- ใส่ AI accelerator ขนาดเล็กได้
- ต้อง pivot โปรเจค (ใส่ทั้งโมเดลใหญ่ ๆ ไม่พอ)

#### 💸 ทางเลือก E: PYNQ-Z2 (~6,000-8,000 บาท)
- ตัวเลือกที่ดีที่สุดถ้ามีงบ
- 5 คนหาร = คนละ 1,200-1,600 บาท
- ทำโปรเจคเดิมได้เต็มสตรีม

### 7.2 คำแนะนำ

```
Week 0 Action Items:
[ ] ติดต่ออาจารย์ 2-3 คน ขอยืมบอร์ด (ทาง A)
[ ] สมัคร Intel DevCloud ทั้งทีม (ทาง B - free backup)
[ ] ลงขัน Tang Nano 20K 1 ตัวเผื่อไว้ (ทาง D - 240/คน)
[ ] ถ้าไม่ได้ทั้ง A และ D → ลงทุน PYNQ-Z2 (ทาง E)
```

---

## 8. แหล่งเรียนรู้แต่ละ Role

### 📚 Common (ทุกคน Week 1)

**YouTube:**
- Mohammad Sadri — "FPGA Tutorial" playlist
- Adam Taylor — "Zynq Adventures"
- FPGA Developer

**หนังสือ (ฟรี PDF):**
- "Parallel Programming for FPGAs" by Ryan Kastner et al.

**Paper อ่านก่อนเริ่ม (3 ฉบับ):**
- "FINN: A Framework for Fast, Scalable Binarized Neural Network Inference"
- "Efficient Hardware Architectures for Deep CNN Inference"
- ค้นด้วย Google Scholar: "CNN FPGA accelerator" filter by year 2022+

---

### 👤 คนที่ 1 (AI Model)
- **คอร์ส:** Fast.ai Practical Deep Learning, PyTorch tutorials
- **เครื่องมือ:** Google Colab (free GPU), Netron (visualize model)
- **อ่าน:** "Quantization and Training of Neural Networks for Efficient Integer-Arithmetic-Only Inference" (Google paper)

### 👤 คนที่ 2 (HLS)
- **Tutorial:** Xilinx Vitis HLS Tutorial บน GitHub
- **YouTube:** ZipCPU channel
- **อ่าน:** "Parallel Programming for FPGAs" บทที่ 3-5

### 👤 คนที่ 3 (Integration)
- **คอร์ส:** Vivado Design Suite tutorials บน Xilinx Learning Center
- **Blog:** Adam Taylor's MicroZed Chronicles
- **อ่าน:** AXI4 specification (ARM)

### 👤 คนที่ 4 (PYNQ)
- **เว็บ:** pynq.io — มี tutorial ละเอียดมาก
- **GitHub:** Xilinx/PYNQ examples
- **Course:** PYNQ workshops บน YouTube

### 👤 คนที่ 5 (System/Demo)
- **Flask:** freeCodeCamp Flask course
- **LaTeX:** Overleaf tutorials
- **Writing:** "How to Write a Good Research Paper" — IEEE guide

---

## 9. คำศัพท์สำคัญ

| คำ | ความหมายง่าย ๆ |
|---|---|
| **FPGA** | Field-Programmable Gate Array — ชิปที่สร้างวงจรเองได้ |
| **PYNQ** | Python on Zynq — framework คุย FPGA ผ่าน Python |
| **Zynq** | ชิปจาก AMD/Xilinx ที่มี ARM + FPGA ในตัวเดียว |
| **SoC** | System on Chip — ทุกอย่างในชิปเดียว |
| **HDL** | Hardware Description Language (Verilog/VHDL) |
| **HLS** | High-Level Synthesis — แปลง C++ → ฮาร์ดแวร์ |
| **IP Block** | Intellectual Property block — ฮาร์ดแวร์ที่ใช้ซ้ำได้ |
| **AXI** | Advanced eXtensible Interface — มาตรฐาน bus ของ ARM |
| **AXI Stream** | AXI สำหรับส่งข้อมูลต่อเนื่อง (streaming) |
| **AXI Lite** | AXI สำหรับส่งค่า control เล็ก ๆ |
| **Bitstream** | ไฟล์ config ของ FPGA (.bit) |
| **Overlay** | bitstream + Python API คู่กัน (PYNQ term) |
| **Vivado** | IDE หลักของ AMD/Xilinx สำหรับ FPGA |
| **Quantization** | แปลงตัวเลข float → int เพื่อลด size + เพิ่มความเร็ว |
| **CNN** | Convolutional Neural Network — AI สำหรับภาพ |
| **Convolution** | การคูณ-บวกแบบหน้าต่างเลื่อน |
| **Pipeline** | ทำหลายงานต่อเนื่องเหมือนสายพานโรงงาน |
| **Unroll** | ขยาย loop ให้ทำขนานกัน |
| **DMA** | Direct Memory Access — ย้ายข้อมูลไม่ต้องผ่าน CPU |
| **BRAM** | Block RAM — หน่วยความจำในตัว FPGA |
| **LUT** | Look-Up Table — หน่วยพื้นฐานของ FPGA logic |
| **DSP Block** | Digital Signal Processing block — ตัวคูณเร็วในตัว FPGA |
| **Inference** | การใช้ AI model ที่เทรนแล้วทำนายผล |
| **INT8** | จำนวนเต็ม 8-bit (-128 ถึง 127) |
| **FPS** | Frames Per Second |
| **Latency** | เวลาตั้งแต่รับ input ถึงได้ output |

---

## 10. เคล็ดลับสู่ความสำเร็จ

### ✅ ทำ
1. **อย่าทำทั้งโมเดลบน FPGA** — accelerate แค่ conv layers ก็พอ
2. **ใช้ HLS เป็นหลัก** — Verilog ใช้ตอน optimize ท้าย ๆ ถ้ามีเวลา
3. **Benchmark ละเอียด ๆ** — กรรมการชอบเลข เช่น "12ms vs Pi 87ms, 4.2W vs 6.1W"
4. **Code review ทุกสัปดาห์** — ทุกคนต้องเข้าใจภาพรวม ไม่ใช่แค่ส่วนตัวเอง
5. **GitHub สวย ๆ** — README พร้อม diagram + GIF + benchmark table
6. **Story-driven proposal** — เริ่มจากปัญหา → ทำไม FPGA → ทำไมเรา ไม่ใช่จาก "อยากเรียน FPGA"
7. **เผื่อ buffer 30%** — Hardware bug หา debug นานกว่า software มาก

### ❌ อย่าทำ
1. **อย่าเริ่มเขียน Verilog ตั้งแต่ Day 1** — learning curve ฆ่าโปรเจคได้
2. **อย่าทำงานแยกขาด** — Week 4 ต้องเริ่ม integrate ไม่ใช่รอ Month 3
3. **อย่าตั้ง scope ใหญ่เกิน** — full YOLO บน FPGA = 6 เดือน+ ไม่ใช่ 3 เดือน
4. **อย่ารอ "พร้อม" แล้วค่อยเริ่ม** — เรียนไปทำไป
5. **อย่าให้คนที่ 5 ทำงานคนเดียวตอนท้าย** — documentation ต้องเขียนระหว่างทาง

### 🎯 เคล็ดลับสำหรับ JAIST Proposal Story

**Bad opening:** "เราอยากเรียน FPGA จึงทำโปรเจคนี้..."

**Good opening:** "เกษตรกรไทย 80% ยังตรวจโรคพืชด้วยตาเปล่า ซึ่งผิดพลาดและล่าช้า ระบบ AI cloud-based มีปัญหา latency + internet ในไร่ — เราเสนอ Edge AI ที่ใช้ FPGA เพื่อ..."

---

## 11. ขั้นตอนถัดไป

### Week 0 (ทันที!)

**ทุกคนทำพร้อมกัน:**
- [ ] อ่านเอกสารนี้ให้จบ
- [ ] นัด meeting แรก (online หรือ offline ก็ได้)
- [ ] ตัดสินใจ role: ใครเอาคนที่ 1, 2, 3, 4, 5
- [ ] Setup Discord/LINE group สำหรับสื่อสาร
- [ ] Setup GitHub organization + repo
- [ ] Setup Notion/Google Drive สำหรับเอกสาร

**1 คน contact (อาจจะคนที่ 5):**
- [ ] ส่งอีเมลหาอาจารย์ภาควิชา CPE 2-3 คน ขอคุยเรื่องบอร์ด + ที่ปรึกษา
- [ ] เช็คเงื่อนไขทุน JAIST อีกครั้ง (deadline, requirement, format)

**1 คน research (อาจจะคนที่ 1):**
- [ ] Download PlantVillage dataset ดูก่อน
- [ ] อ่าน paper อ้างอิง 3 ฉบับ ส่ง summary ให้ทีม

### Meeting 1 Agenda (Suggested)

1. **Intro & Role Assignment** (15 นาที)
   - ใครรับ role ไหน
   - Backup plan ถ้าใครหยุดกลางทาง

2. **Scope Decision** (30 นาที)
   - ทำตามที่เสนอ (plant disease) หรือเปลี่ยน application?
   - Target dataset / number of classes
   - บอร์ดที่จะใช้

3. **Communication & Tools** (15 นาที)
   - Channel หลัก
   - Meeting schedule (เสนอ weekly 1 ชม.)
   - Git workflow

4. **Action Items for Week 1** (15 นาที)
   - แจกงานเรียน
   - ใครติดต่ออาจารย์ ใครสั่งของ
   - Next meeting

---

## 🚀 Last Words

> โปรเจคนี้ **ยาก แต่ทำได้** ถ้าทีมขนาน 5 คนเก่ง ๆ ทำ
> 
> ลงทุนเวลา 3 เดือน → ถ้าได้ทุน JAIST ก็คุ้มมาก
> 
> ถ้าไม่ได้ทุน → ก็ยังได้ผลงานเด่นในเรซูเม่ + ความรู้ทักษะที่หาง่าย ๆ ไม่ได้
> 
> **เริ่มเลย ดีกว่ารอให้พร้อม**

---

**Document version:** 1.0  
**สร้างเพื่อ:** ทีมงาน 5 คน วิศวะคอมมหิดล ปี 3 — โปรเจคยื่นทุน JAIST  
**ติดต่อ/แก้ไข:** [ระบุชื่อคนรับผิดชอบเอกสาร]
