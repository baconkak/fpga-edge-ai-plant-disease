# 🎓 FPGA-Accelerated Real-time Plant Disease Detection for Smart Agriculture

[![Project Status: Active](https://img.shields.io/badge/Project%20Status-Active-green.svg)]()
[![Target](https://img.shields.io/badge/Target-JAIST%20Scholarship%20Proposal-blue)]()
[![Platform](https://img.shields.io/badge/Platform-PYNQ--Z2%20%7C%20Zynq%207000-orange)]()

ระบบ Edge AI ประสิทธิภาพสูงสำหรับการตรวจจับโรคพืชแบบเรียลไทม์ โดยใช้ FPGA (Field-Programmable Gate Array) เร่งความเร็วการประมวลผลในส่วนของ Convolutional Layers ออกแบบมาเพื่อให้สามารถทำงานได้ในพื้นที่การเกษตรห่างไกลที่มีข้อจำกัดด้านพลังงานและโครงข่ายอินเทอร์เน็ต ซึ่งเป็นโปรเจควิจัยและพัฒนาเพื่อยื่นสมัครทุน **JAIST Scholarship**

---

## 📋 สารบัญ (Table of Contents)
1. [ภาพรวมสถาปัตยกรรมระบบ (System Architecture)](#1-ภาพรวมสถาปัตยกรรมระบบ-system-architecture)
2. [โครงสร้างโฟลเดอร์ของโปรเจค (Repository Structure)](#2-โครงสร้างโฟลเดอร์ของโปรเจค-repository-structure)
3. [การแบ่งบทบาทหน้าที่ในทีม (Team Roles & Deliverables)](#3-การ-แบ่งบทบาทหน้าที่ในทีม-team-roles--deliverables)
4. [ผลการทดสอบและเปรียบเทียบ (Benchmarks & Experimental Results)](#4-ผลการทดสอบและเปรียบเทียบ-benchmarks--experimental-results)
5. [ขั้นตอนการติดตั้งและเริ่มใช้งาน (Getting Started)](#5-ขั้นตอนการติดตั้งและเริ่มใช้งาน-getting-started)

---

## 1. ภาพรวมสถาปัตยกรรมระบบ (System Architecture)

ระบบประกอบด้วยการทำงานร่วมกันระหว่าง Hardware-Software Co-design บนชิป Zynq SoC โดยแบ่งหน้าที่การประมวลผลออกเป็น 2 ส่วนหลัก:
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

Data Flow ของระบบ
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

## 2. โครงสร้างโฟลเดอร์ของโปรเจค (Repository Structure)

คลังโค้ดนี้ถูกจัดสรรแยกตามโดเมนงานวิจัยอย่างชัดเจน เพื่อรองรับกระบวนการพัฒนาแบบคู่ขนาน (Parallel Development):

```text
fpga-edge-ai-plant-disease/
│
├── ai-model/               # [👤 Role 1] การพัฒนาและเทรนโมเดล AI
│   ├── datasets/           # สคริปต์ดาวน์โหลดและการจัดการ PlantVillage Dataset
│   ├── notebooks/          # Jupyter Notebook สำหรับกระบวนการ Train & Quantization (INT8)
│   ├── src/                # โค้ดต้นฉบับภาษา Python สำหรับ Pipeline ของโมเดล
│   └── exported-weights/   # ไฟล์ค่าน้ำหนักโมเดล (.bin, .npy, .onnx) ที่พร้อมส่งต่อให้ FPGA
│
├── fpga-hls/               # [👤 Role 2] การออกแบบวงจรเร่งความเร็วด้วย High-Level Synthesis
│   ├── src/                # โค้ดต้นฉบับ C/C++ ของ Hardware IP (conv2d.cpp, conv2d.h)
│   ├── tb/                 # Testbench สำหรับตรวจสอบความถูกต้องผ่าน C-Simulation
│   └── script.tcl          # สคริปต์อัตโนมัติสำหรับการจดสิทธิบัตรและ Export IP Block
│
├── fpga-hardware/          # [👤 Role 3] การรวมระบบฮาร์ดแวร์บน Vivado
│   ├── vivado-project/     # โครงสร้างโปรเจคหลักบนโปรแกรม Vivado
│   ├── ip-repo/            # คลังเก็บ Custom IP Blocks ที่ได้มาจากพาร์ท HLS
│   └── bitstreams/         # ไฟล์บิตสตรีมสำเร็จรูป (.bit, .hwh) สำหรับโปรแกรมลงบอร์ด
│
├── pynq-embedded/          # [👤 Role 4] ซอฟต์แวร์ฝังตัวบนระบบปฏิบัติการ Linux (PYNQ)
│   ├── notebooks/          # Jupyter Notebook สำหรับการทำ Rapid Prototyping บนบอร์ดจริง
│   ├── overlays/           # โฟลเดอร์จัดเก็บไฟล์ดีพลอยฮาร์ดแวร์ (.bit และ .hwh)
│   ├── src/                # โค้ดหลักระบบควบคุม (Camera -> Hardware Buffer -> Hardware Execute)
│   └── app_api.py          # API Gateway (FastAPI/Flask) เชื่อมโยงระบบเข้ากับเครือข่าย
│
├── web-dashboard/          # [👤 Role 5] ส่วนติดต่อผู้ใช้งานและระบบแสดงผล
│   ├── backend/            # ระบบเซิร์ฟเวอร์ย่อยสำหรับการส่งผ่านข้อมูลผ่าน WebSockets
│   ├── frontend/           # หน้าต่างเว็บแอปพลิเคชัน (HTML/JS หรือ React) แสดงผลแบบเรียลไทม์
│   └── assets/             # ไฟล์สื่อประสม รูปภาพประกอบ และเอกสารนำเสนอ
│
└── docs/                   # เอกสารประกอบงานวิจัยทางวิชาการ
    ├── architecture/       # แผนผังทางสถาปัตยกรรมและแผนภูมิกระแสข้อมูล (Data Flow)
    ├── reports/            # ร่างรายงานวิจัยฉบับสมบูรณ์ (Technical Report 10-15 หน้า)
    └── presentation/       # สไลด์สำหรับใช้นำเสนอต่อคณะกรรมการทุน JAIST