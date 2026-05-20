// frontend/src/app/page.tsx
'use client';

import { usePynqSocket } from '@/hooks/usePynqSocket';
import { Cpu, Activity, Thermometer, ShieldAlert, Sun, Moon, Play, Square } from 'lucide-react';
import { useState, useEffect } from 'react';
import { LineChart, Line, XAxis, YAxis, ResponsiveContainer } from 'recharts';

// กำหนดโครงสร้างข้อมูลจำลองในฝั่งหน้าบ้าน
interface LocalData {
  device_id: string;
  disease_detected: string;
  confidence: number;
  hardware_metrics: { fpga_temp: number; inference_time_ms: number; fps: number; power_watt: number; };
  image_base64?: string;
}

export default function DashboardPage() {
  const { latestData: socketData } = usePynqSocket(); // ดึงข้อมูลจากหลังบ้านจริง
  const [isDarkMode, setIsDarkMode] = useState(true);
  
  // 🔘 ข้อที่ 1: ปุ่ม Toggle Demo Mode (สถานะเปิด/ปิด โหมดจำลองบนหน้าเว็บ)
  const [isDemoMode, setIsDemoMode] = useState(false);
  const [localDemoData, setLocalDemoData] = useState<LocalData | null>(null);

  // 📈 ตัวแปรเก็บประวัติอุณหภูมิย้อนหลังเพื่อเอาไปวาดกราฟ
  const [tempHistory, setTempHistory] = useState<{ name: string; temp: number }[]>([]);

  // เลือกใช้ข้อมูล: ถ้าเปิด Demo Mode ให้ใช้ค่าสุ่มในเครื่อง ถ้าปิดให้ใช้ค่าจากบอร์ดจริง (Socket)
  const activeData = isDemoMode ? localDemoData : socketData;

  // 🔄 กลไกข้อที่ 1: ใช้ setInterval ใน React สุ่มพ่นข้อมูลเมื่อเปิด Demo Mode
  useEffect(() => {
    let intervalId: NodeJS.Timeout;

    if (isDemoMode) {
      const diseases = ['Healthy', 'Powdery_Mildew', 'Leaf_Spot', 'Rust'];
      
      intervalId = setInterval(() => {
        const mock: LocalData = {
          device_id: 'pynq-z2-portfolio-demo',
          disease_detected: diseases[Math.floor(Math.random() * diseases.length)],
          confidence: 0.75 + Math.random() * 0.23,
          hardware_metrics: {
            fpga_temp: Math.floor(48 + Math.random() * 15),
            inference_time_ms: +(32.4 + Math.random() * 8).toFixed(2),
            fps: Math.floor(24 + Math.random() * 6),
            power_watt: +(2.1 + Math.random() * 0.8).toFixed(2) // เพิ่มมิติวัดพลังงาน (Watts)
          }
        };
        setLocalDemoData(mock);
      }, 2000); // สุ่มค่าใหม่ทุกๆ 2 วินาทีให้กรรมการเห็นกราฟขยับเร้าใจ
    }

    return () => clearInterval(intervalId);
  }, [isDemoMode]);

  // คอยอัปเดตกราฟสถิติตามข้อมูลที่ไหลเข้ามา
  useEffect(() => {
    if (activeData) {
      setTempHistory(prev => {
        const next = [...prev, { name: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' }), temp: activeData.hardware_metrics.fpga_temp }];
        if (next.length > 7) next.shift(); // ให้เก็บประวัติไว้แค่ 7 จุดล่าสุดพอ กราฟจะได้ไม่แน่นเกินไป
        return next;
      });
    }
  }, [activeData]);

  return (
    <div className={`p-6 space-y-6 min-h-screen font-sans transition-colors duration-300 ${
      isDarkMode ? 'bg-slate-950 text-slate-50' : 'bg-slate-50 text-slate-900'
    }`}>
      
      {/* 1. TOP HEADER STATUS */}
      <div className={`flex justify-between items-center border-b pb-5 ${isDarkMode ? 'border-slate-800' : 'border-slate-200'}`}>
        <div>
          <h1 className="text-2xl font-bold tracking-tight">FPGA Edge AI Plant Disease Monitor</h1>
          <p className={`text-sm ${isDarkMode ? 'text-slate-400' : 'text-slate-600'}`}>
            Device ID: <span className="font-mono text-emerald-500 font-bold">{activeData?.device_id || 'Connecting...'}</span>
          </p>
        </div>
        
        {/* RIGHT CONTROLS */}
        <div className="flex items-center gap-3">
          
          {/* 🔥 ปุ่ม Toggle Demo Mode (ข้อที่ 1) */}
          <button
  onClick={() => setIsDemoMode(!isDemoMode)}
  className={`flex items-center gap-2 px-4 py-2 rounded-lg text-sm font-semibold border transition-all ${
    isDemoMode 
      ? 'bg-amber-500/20 border-amber-500 text-amber-500 hover:bg-amber-500/30' 
      : isDarkMode
        ? 'bg-slate-900 border-slate-800 text-slate-400 hover:text-slate-200 hover:bg-slate-800' // โหมดมืดปกติ
        : 'bg-white border-slate-300 text-slate-600 hover:text-slate-900 hover:bg-slate-100 shadow-sm' 
  }`}
>
  {isDemoMode ? (
    <>
      <Square className="h-4 w-4 fill-amber-500" /> Stop Demo
    </>
  ) : (
    <>
      <Play className={`h-4 w-4 ${isDarkMode ? 'fill-slate-400' : 'fill-slate-500'}`} /> Toggle Demo Mode
    </>
  )}
</button>

          {/* ปุ่มเปลี่ยนธีม */}
          <button
            onClick={() => setIsDarkMode(!isDarkMode)}
            className={`p-2 rounded-lg border transition-all ${
              isDarkMode ? 'bg-slate-900 border-slate-800 text-amber-400 hover:bg-slate-800' : 'bg-white border-slate-300 text-purple-600 hover:bg-slate-100'
            }`}
          >
            {isDarkMode ? <Sun className="h-5 w-5" /> : <Moon className="h-5 w-5" />}
          </button>

          <div className={`flex items-center gap-2 px-4 py-2 rounded-full border ${isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200'}`}>
            <span className={`h-3 w-3 rounded-full ${activeData ? 'bg-emerald-500 animate-pulse' : 'bg-rose-500'}`} />
            <span className={`text-sm font-medium ${isDarkMode ? 'text-slate-300' : 'text-slate-700'}`}>
              {isDemoMode ? 'Running Frontend Simulation' : activeData ? 'PYNQ Link Active' : 'Searching for Board...'}
            </span>
          </div>
        </div>
      </div>

      {/* 2. TELEMETRY CARDS GRID (ข้อที่ 3: มิติตัวชี้วัดวิศวกรรมฮาร์ดแวร์จริง) */}
      <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-5">
        
        {/* Card 1: ผลวินิจฉัยโรค */}
        <div className={`border p-6 rounded-xl space-y-2 ${isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'}`}>
          <div className="flex justify-between items-center text-slate-400">
            <span className="text-sm font-medium">Latest Detection</span>
            <ShieldAlert className="h-5 w-5 text-amber-500" />
          </div>
          <div className="text-2xl font-bold text-emerald-500 tracking-tight">
            {activeData?.disease_detected.replace('_', ' ') || '--'}
          </div>
          <p className="text-xs text-slate-500">
            Confidence: {activeData ? `${(activeData.confidence * 100).toFixed(1)}%` : '--'}
          </p>
        </div>

        {/* Card 2: อุณหภูมิชิป FPGA */}
        <div className={`border p-6 rounded-xl space-y-2 ${isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'}`}>
          <div className="flex justify-between items-center text-slate-400">
            <span className="text-sm font-medium">FPGA Core Temp</span>
            <Thermometer className="h-5 w-5 text-rose-500" />
          </div>
          <div className="text-2xl font-bold font-mono">
            {activeData?.hardware_metrics.fpga_temp ?? '--'} <span className="text-sm font-normal text-slate-400">°C</span>
          </div>
          <p className="text-xs text-slate-500">Safe Limit: &lt; 75.0 °C</p>
        </div>

        {/* Card 3: ความเร็วโมเดล AI */}
        <div className={`border p-6 rounded-xl space-y-2 ${isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'}`}>
          <div className="flex justify-between items-center text-slate-400">
            <span className="text-sm font-medium">Inference Speed</span>
            <Cpu className="h-5 w-5 text-cyan-500" />
          </div>
          <div className="text-2xl font-bold font-mono">
            {activeData?.hardware_metrics.inference_time_ms ?? '--'} <span className="text-sm font-normal text-slate-400">ms</span>
          </div>
          <p className="text-xs text-slate-500">Hardware Accelerated</p>
        </div>

        {/* Card 4: เฟรมเรตกล้อง */}
        <div className={`border p-6 rounded-xl space-y-2 ${isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'}`}>
          <div className="flex justify-between items-center text-slate-400">
            <span className="text-sm font-medium">Camera Stream</span>
            <Activity className="h-5 w-5 text-purple-500" />
          </div>
          <div className="text-2xl font-bold font-mono">
            {activeData?.hardware_metrics.fps ?? '--'} <span className="text-sm font-normal text-slate-400">FPS</span>
          </div>
          <p className="text-xs text-slate-500">Real-time Video Feed</p>
        </div>

        {/* ⚡ CARD ยกระดับเป็นคอมพิวเตอร์วิศวกร: Power Consumption (Watts) */}
        <div className={`border p-6 rounded-xl space-y-2 ${isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'}`}>
          <div className="flex justify-between items-center text-slate-400">
            <span className="text-sm font-medium">Power Consumption</span>
            <Activity className="h-5 w-5 text-emerald-500" />
          </div>
          <div className="text-2xl font-bold font-mono text-emerald-400">
            {activeData?.hardware_metrics.power_watt ?? '--'} <span className="text-sm font-normal text-slate-400">W</span>
          </div>
          <p className="text-xs text-slate-500">Zynq-7000 Rail Estimate</p>
        </div>
      </div>

      {/* 3. LOWER AREA */}
      <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-7">
        
        {/* 🎥 ข้อที่ 2: ช่อง Live Camera Feed พร้อมติดตั้งระบบ Skeleton Screen อนิเมชันวิ่งผ่าน */}
        <div className={`lg:col-span-4 h-87.5 rounded-xl overflow-hidden border relative flex items-center justify-center ${
  isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'
        }`}>
          {activeData?.image_base64 ? (
            // 🖼️ สถานะที่ 1: เมื่อมีภาพส่งมาจากบอร์ด PYNQ หรือ Demo
            <img 
              src={`data:image/jpeg;base64,${activeData.image_base64}`} 
              className="w-full h-full object-cover"
              alt="PYNQ Live Feed"
            />
          ) : (
            // ⏳ สถานะที่ 2: ระหว่างรอภาพ โชว์ Skeleton UI (ปรับสีตัวหนังสือด้านในให้เด่นขึ้น)
            <div className="absolute inset-0 flex flex-col items-center justify-center p-6 space-y-4">
              <div className="w-full h-full bg-slate-800/40 rounded-lg animate-pulse overflow-hidden relative">
                <div className="absolute inset-0 -translate-x-full animate-[shimmer_2s_infinite] bg-linear-to-r from-transparent via-slate-700/20 to-transparent" />
                <div className="absolute inset-0 flex flex-col items-center justify-center">
                  {/* 🔥 ปรับสีข้อความหัวข้อหลักให้สว่างคมชัด (ไม่ซีด) */}
                  <span className={`text-sm font-mono font-bold tracking-wider animate-bounce ${
                    isDarkMode ? 'text-slate-200' : 'text-slate-700'
                  }`}>
                    [ STREAMS WAITING SKELETON ]
                  </span>
                  {/* 💡 ปรับสีคำอธิบายรองให้ซอฟต์ลงมานิดนึงเพื่อความสวยงามแต่ยังอ่านง่าย */}
                  <span className={`text-xs mt-1 ${
                    isDarkMode ? 'text-slate-400' : 'text-slate-500'
                  }`}>
                    Ready for PYNQ Framebuffer Base64 data
                  </span>
                </div>
              </div>
            </div>
          )}
        </div>
        
        {/* กราฟฝั่งขวา */}
        <div className={`lg:col-span-3 p-5 h-87.5 rounded-xl border flex flex-col justify-between ${
          isDarkMode ? 'bg-slate-900 border-slate-800' : 'bg-white border-slate-200 shadow-sm'
        }`}>
          <div>
            <h3 className="text-sm font-semibold tracking-tight">FPGA Temperature Analytics (Real-time)</h3>
            <p className="text-xs text-slate-500 mb-4">Monitoring core silicon thermal trends</p>
          </div>
          
          <div className="w-full h-55">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={tempHistory}>
                <XAxis dataKey="name" stroke="#64748b" fontSize={10} />
                <YAxis domain={[30, 80]} stroke="#64748b" fontSize={11} />
                <Line 
                  type="monotone" 
                  dataKey="temp" 
                  stroke="#f43f5e" 
                  strokeWidth={3} 
                  dot={{ r: 3, strokeWidth: 1 }}
                />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>

      </div>

    </div>
  );
}