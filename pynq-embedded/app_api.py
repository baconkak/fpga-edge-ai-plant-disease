import cv2
import numpy as np
from fastapi import FastAPI, File, UploadFile, BackgroundTasks
from fastapi.responses import JSONResponse, StreamingResponse
from fastapi.middleware.cors import CORSMiddleware
import io
import time

# Import our custom modules
from src.pynq_driver import PlantDiseaseAccelerator
from src.preprocessing import preprocess_image
from src.postprocessing import process_output
from src.utils import PerformanceTracker

app = FastAPI(title="FPGA Plant Disease Detection API")

# Allow CORS for Web Dashboard (Role 5)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Initialize Hardware and Utils
try:
    # Assuming bitstream is placed in overlays/
    accel = PlantDiseaseAccelerator(bitstream_path="overlays/plant_disease.bit")
except Exception as e:
    print(f"Error loading accelerator: {e}")
    accel = None

tracker = PerformanceTracker()

@app.get("/")
def read_root():
    return {"message": "Plant Disease Detection API is running"}

@app.get("/api/v1/status")
def get_status():
    if accel is None:
        mode = "error"
    else:
        mode = accel.mode
        
    return {
        "status": "online",
        "hardware_mode": mode,
        "metrics": tracker.get_metrics()
    }

@app.post("/api/v1/predict")
async def predict_image(file: UploadFile = File(...)):
    if not accel:
        return JSONResponse(status_code=500, content={"error": "Accelerator not initialized."})
        
    try:
        # Read image
        contents = await file.read()
        nparr = np.frombuffer(contents, np.uint8)
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        
        if img is None:
            return JSONResponse(status_code=400, content={"error": "Invalid image file."})
            
        # 1. Preprocess
        quantized_input = preprocess_image(img)
        
        # 2. Hardware Inference
        tracker.start_inference()
        accel.write_input(quantized_input)
        accel.run_inference()
        raw_output = accel.read_output()
        tracker.end_inference()
        
        # 3. Postprocess
        result = process_output(raw_output)
        
        # Append metrics
        result["metrics"] = tracker.get_metrics()
        
        return result
        
    except Exception as e:
        return JSONResponse(status_code=500, content={"error": str(e)})

# --- Stream Endpoint (for Demo) ---
camera = None

def generate_frames():
    global camera
    if camera is None:
        camera = cv2.VideoCapture(0) # Open USB Camera
        
    while True:
        success, frame = camera.read()
        if not success:
            break
        else:
            # Optionally do inference here and draw bounding box / label on frame
            # For performance in demo, maybe we just stream and do inference periodically
            
            # Encode frame
            ret, buffer = cv2.imencode('.jpg', frame)
            frame_bytes = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

@app.get("/api/v1/camera/stream")
def video_feed():
    return StreamingResponse(generate_frames(), media_type="multipart/x-mixed-replace; boundary=frame")
