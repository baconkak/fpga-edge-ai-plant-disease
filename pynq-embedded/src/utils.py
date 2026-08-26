import time

class PerformanceTracker:
    def __init__(self):
        self.start_time = None
        self.frame_count = 0
        self.fps = 0.0
        self.latency_ms = 0.0
        
    def start_inference(self):
        self.start_time = time.time()
        
    def end_inference(self):
        if self.start_time is not None:
            end_time = time.time()
            self.latency_ms = (end_time - self.start_time) * 1000.0
            self.frame_count += 1
            
            # Simple moving average for FPS, or just instantaneous
            if self.latency_ms > 0:
                inst_fps = 1000.0 / self.latency_ms
                if self.fps == 0.0:
                    self.fps = inst_fps
                else:
                    self.fps = 0.9 * self.fps + 0.1 * inst_fps

    def get_metrics(self):
        return {
            "fps": round(self.fps, 2),
            "latency_ms": round(self.latency_ms, 2)
        }
