if [ ! -d "./log" ]; then
  mkdir ./log
fi
python3 signal_control.py --camIndex 0 --width 1280 --height 720 --fps 1