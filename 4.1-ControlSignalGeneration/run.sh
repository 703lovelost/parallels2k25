if [ ! -d "./log" ]; then
  mkdir ./log
fi
python3 signal_control.py --camIndex 0 --width 640 --height 480 --fps 1