```
    # Single-threaded inference
    python3 inference.py video.mp4 --mode single output_video.mp4

    # Multi-threaded inference (4 threads)
    python3 inference.py video.mp4 --mode multi --threads 4 output_video.mp4
```

Single-threaded time measure: 87.72 sec
Multi-threaded time measure (4 threads): 26.06 sec
Multi-threaded time measure (8 threads): 17.68 sec
Multi-threaded time measure (16 threads): 16.34 sec