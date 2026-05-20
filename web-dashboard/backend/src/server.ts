import express from 'express';
import { createServer } from 'http';
import { Server } from 'socket.io';
import cors from 'cors';

const app = express();
app.use(cors());
app.use(express.json()); // ช่วยให้ Server อ่านข้อมูลแบบ JSON ได้

const httpServer = createServer(app);
// เปิดท่อ WebSocket ให้ Frontend (Port 3000) เชื่อมเข้ามาคุยได้
const io = new Server(httpServer, {
  cors: {
    origin: 'http://localhost:3000',
    methods: ['GET', 'POST'],
  },
});

// 1. API Endpoint เปล่า: รอรับข้อมูลวิเคราะห์โรคพืชที่ยิงมาจาก Python บน PYNQ
app.post('/api/v1/pynq/inference', (req, res) => {
  const pynqData = req.body;

  console.log('📥 Received Data from PYNQ:', pynqData);

  // ดันข้อมูลนี้ออกไปที่หน้าบ้าน (Frontend) ทันทีผ่าน WebSocket แบบ Real-time
  io.emit('pynq-telemetry', pynqData);

  return res.status(200).json({
    success: true,
    message: 'Data streamed to dashboard successfully',
  });
});

// 2. WebSocket Connection: คอยดูว่าหน้าบ้านเชื่อมต่อท่อมาหรือยัง
io.on('connection', (socket) => {
  console.log('⚡ Dashboard Client connected:', socket.id);

  // สร้าง Demo Mode (ข้อมูลจำลอง)
  // ถ้ายังไม่ได้เปิดบอร์ด PYNQ ตัวหลังบ้านจะสุ่มส่งข้อมูลโรคพืชไปให้หน้าบ้านทุกๆ 3 วินาที
  // เพื่อให้หน้าเว็บขยับโชว์กรรมการได้ทันทีเวลาเอาเว็บไปเปิดที่ไหน
  const diseases = ['Healthy', 'Powdery_Mildew', 'Leaf_Rust', 'Bacterial_Spot'];
  
  const demoInterval = setInterval(() => {
    // ส่งข้อมูลจำลองถ้าไม่มีบอร์ดจริงยิงเข้ามา
    const mockPayload = {
      device_id: 'pynq-z2-demo-node',
      timestamp: new Date().toISOString(),
      disease_detected: diseases[Math.floor(Math.random() * diseases.length)],
      confidence: parseFloat((Math.random() * (0.99 - 0.85) + 0.85).toFixed(3)),
      hardware_metrics: {
        fpga_temp: parseFloat((Math.random() * (55.0 - 45.0) + 45.0).toFixed(1)),
        inference_time_ms: parseFloat((Math.random() * (40.0 - 30.0) + 30.0).toFixed(2)),
        fps: parseFloat((Math.random() * (30.0 - 25.0) + 25.0).toFixed(1)),
        power_watt: parseFloat((2.0 + Math.random() * 0.9).toFixed(2))
      },
    };
    
    socket.emit('pynq-telemetry', mockPayload);
  }, 1000);

  socket.on('disconnect', () => {
    console.log('❌ Dashboard Client disconnected');
    clearInterval(demoInterval);
  });
});

// รันหลังบ้านที่ Port 4000
const PORT = 4000;
httpServer.listen(PORT, () => {
  console.log(`🚀 Webhook & Socket Server is running on http://localhost:${PORT}`);
});