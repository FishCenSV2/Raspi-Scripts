#!/bin/bash

rpicam-vid -t 0 -n --framerate 56 --mode 2304:1296 --width 1920 --height 1080  --inline -o - | gst-launch-1.0 fdsrc fd=0 ! h264parse ! rtph264pay ! udpsink host=192.168.0.103 port=5000