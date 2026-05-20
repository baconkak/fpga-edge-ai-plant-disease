// frontend/src/hooks/usePynqSocket.ts
import { useEffect, useState } from 'react';
import { io } from 'socket.io-client';

// กำหนดโครงสร้างข้อมูล (TypeScript Interface) ให้ตรงกับหลังบ้าน
export interface PynqPayload {
  device_id: string;
  timestamp: string;
  disease_detected: string;
  confidence: number;
  hardware_metrics: {
    fpga_temp: number;
    inference_time_ms: number;
    fps: number;
    power_watt: number;
  };
  image_base64?: string;
}

export const usePynqSocket = () => {
  const [latestData, setLatestData] = useState<PynqPayload | null>(null);

  useEffect(() => {
    // เชื่อมต่อไปยังหลังบ้าน Node.js Port 4000
    const socket = io('http://localhost:4000');

    // นั่งรอฟัง Event ชื่อ 'pynq-telemetry' ที่หลังบ้านจะพ่นมาทุกๆ 3 วินาที
    socket.on('pynq-telemetry', (data: PynqPayload) => {
      setLatestData(data);
    });

    // ตัดการเชื่อมต่อเมื่อปิดหน้าเว็บเพื่อไม่ให้เปลืองแรม (Memory Leak)
    return () => {
      socket.disconnect();
    };
  }, []);

  return { latestData };
};