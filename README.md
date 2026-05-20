# fpga-edge-ai-plant-disease
ระบบ Edge AI ที่ใช้ FPGA เร่งความเร็วการตรวจจับโรคพืชแบบเรียลไทม์ สำหรับการเกษตรอัจฉริยะ

fpga-edge-ai-plant-disease/
│
├── .gitignore
├── README.md               # หน้าแรกของโปรเจค (คนที่ 5 ดูแล: ใส่ Arch Diagram, ผล Benchmark)
│
├── ai-model/               # 👤 Domain คนที่ 1 (AI Model Engineer)
│   ├── datasets/           # โฟลเดอร์เก็บข้อมูลจำลอง หรือ Script ดึง PlantVillage Dataset
│   ├── notebooks/          # Google Colab / Jupyter Notebooks สำหรับ Train & Quantize
│   ├── src/                # Python scripts สำหรับเทรนและแปลงไฟล์โมเดล
│   └── exported-weights/   # Target output: ไฟล์โมเดล (.bin, .npy, .onnx) ที่ผ่าน INT8 แล้ว
│
├── fpga-hls/               # 👤 Domain คนที่ 2 (FPGA HLS Engineer)
│   ├── src/                # โค้ด C/C++ สำหรับทำ Accelerator IP (เช่น conv2d.cpp, conv2d.h)
│   ├── tb/                 # Testbench สำหรับทำ C-Simulation (tb_conv2d.cpp)
│   └── script.tcl          # Script สำหรับสั่งรันรวบยอดใน Vitis/Vivado HLS
│
├── fpga-hardware/          # 👤 Domain คนที่ 3 (Hardware Integration Engineer)
│   ├── vivado-project/     # ไฟล์โปรเจค Vivado (แนะนำให้ทำ gitignore ไฟล์ชั่วคราวไว้)
│   ├── ip-repo/            # โฟลเดอร์เก็บ IP Block (.zip) ที่ได้มาจากคนที่ 2
│   └── bitstreams/         # Target output: ไฟล์ .bit และ .hwh สำหรับส่งต่อให้คนที่ 4
│
├── pynq-embedded/          # 👤 Domain คนที่ 4 (Embedded Software / PYNQ Engineer)
│   ├── notebooks/          # Jupyter Notebook ที่รันบนบอร์ด PYNQ สำหรับทดสอบด่วน
│   ├── overlays/           # ที่เก็บไฟล์ .bit และ .hwh ที่รับมาจากคนที่ 3
│   ├── src/                # Core Python Code (Pipeline: รับภาพกล้อง -> ส่งเข้า FPGA -> ผลลัพธ์)
│   └── app_api.py          # Python API (เช่น Flask/FastAPI backend) สำหรับส่งต่อให้คนที่ 5
│
├── web-dashboard/          # 👤 Domain คนที่ 5 (System / Demo / Project Manager)
│   ├── backend/            # Web server (กรณีแยกส่วนกับคนที่ 4)
│   ├── frontend/           # โค้ดหน้าเว็บ UI (HTML/CSS/JS หรือ React) สำหรับโชว์ผลเรียลไทม์
│   └── assets/             # รูปภาพ, โลโก้, สไลด์พรีเซนต์
│
└── docs/                   # เอกสารประกอบโปรเจค (ช่วยกันเขียน)
    ├── architecture/       # รูปภาพระบบ/Data Flow Diagram
    ├── reports/            # ร่าง Technical Report (ไฟล์ .tex หรือ .md)
    └── presentation/       # สไลด์สำหรับ Pitch ทุน JAIST