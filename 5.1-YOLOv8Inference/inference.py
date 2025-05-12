import argparse
import cv2
import time
import threading
from queue import Queue
from ultralytics import YOLO
import numpy as np
from functools import partial

class ModelWrapper:
    def __init__(self):
        self.model = YOLO('yolov8s-pose.pt')
        self.model.to('cpu')
    def __del__(self):
        del self.model

    def predict(self, frame):
        return self.model.predict(frame, verbose=False)[0]

class VideoProcessor:
    def __init__(self, video_path, output_path, num_threads=1):
        self.video_path = video_path
        self.output_path = output_path
        self.num_threads = num_threads
        self.cap = cv2.VideoCapture(video_path)
        self.fps = self.cap.get(cv2.CAP_PROP_FPS)
        self.fourcc = cv2.VideoWriter_fourcc(*'mp4v')
        self.frame_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        self.frame_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        self.out = cv2.VideoWriter(output_path, self.fourcc, self.fps, (self.frame_width, self.frame_height))
        
    def __del__(self):
        self.cap.release()
        self.out.release()
        cv2.destroyAllWindows()
        
    def process_single_thread(self):
        model = ModelWrapper()

        frame_count = 0
        start_time = time.time()
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret:
                break
            result = model.predict(frame)
            annotated = result.plot(boxes=False, labels=False)
            self.out.write(annotated)
            frame_count += 1
        end_time = time.time()

        self.cap.release()
        self.out.release()

        return end_time - start_time

    def process_multi_thread(self):
        input_queue = Queue()
        output_dict = {}
        lock = threading.Lock()
        stop_event = threading.Event()

        def worker():
            model = ModelWrapper()
            while not stop_event.is_set():
                try:
                    index, frame = input_queue.get(timeout=0.1)
                    result = model.predict(frame)
                    annotated = result.plot(boxes=False, labels=False)
                    with lock:
                        output_dict[index] = annotated
                    input_queue.task_done()
                except:
                    continue

        index = 0
        start_time = time.time()

        for _ in range(self.num_threads * 3):
            ret, frame = self.cap.read()
            if not ret:
                break
            input_queue.put((index, frame))
            index += 1

        threads = []
        for _ in range(self.num_threads):
            t = threading.Thread(target=worker)
            t.daemon = True
            t.start()
            threads.append(t)

        while True:
            ret, frame = self.cap.read()
            if not ret:
                break
            input_queue.put((index, frame))
            index += 1

        self.cap.release()

        input_queue.join()
        stop_event.set()

        for t in threads:
            t.join()

        print("Generating video...")

        for i in range(index):
            self.out.write(output_dict[i])

        self.out.release()
        end_time = time.time()
        
        return end_time - start_time

def main():
    parser = argparse.ArgumentParser(description='YOLOv8s-pose video processing')
    parser.add_argument('input_video', help='Path to input video file (640x480)')
    parser.add_argument('--mode', choices=['single', 'multi'], required=True, 
                       help='Processing mode: single-threaded or multi-threaded')
    parser.add_argument('output_video', help='Path to output video file')
    parser.add_argument('--threads', type=int, default=4, 
                       help='Number of threads for multi-threaded mode')
    
    args = parser.parse_args()
    
    processor = VideoProcessor(args.input_video, args.output_video, args.threads)
    
    if args.mode == 'single':
        print("Running in single-threaded mode...")
        processing_time = processor.process_single_thread()
    else:
        print(f"Running in multi-threaded mode with {args.threads} threads...")
        processing_time = processor.process_multi_thread()
    
    print(f"Processing completed in {processing_time:.2f} seconds")

if __name__ == '__main__':
    main()