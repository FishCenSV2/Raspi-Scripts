# Raspi-Scripts

This repository contains a script for footage capture and for video streaming on the Raspberry Pi.

## Footage Capture

The main.cpp file is for footage capture on the Raspberry Pi. It works as follows:
- Uses c++ code to call cmd commands to record picam footage (via libcamera commands) to a USB drive named "KINGSTON"
- Also logs console output (stdout and stderr) in file named "{date}-log.txt" and manages counters for capture sessions in file named "cam-log.txt"
- Also flashes indicator lights via GPIO (assuming GPIO27/Pin13 is Yellow and GPIO17/Pin11 is Red):
  - Yellow ON = camera is recording
  - Yellow OFF AND Red ON = camera is NOT recording (probably camera wire connection issues)
  - Yellow ON AND Red ON = camera is recording, Red light just blinks every recorded block to show code isn't frozen

The footage.sh script is for execution at startup and calls the executable in the project file, based on the current project organization. Not included is `/etc/rc.local` which is used in the Pi to cause execution on startup. To edit this file use `sudo nano /etc/rc.local`. Next add the following to the file right before the `exit 0`:
```bash
sleep 10
 
sh /home/FishCenS/Documents/FishCode/FootageCapture/footage.sh &
```
Points to note:
- The sleep is to allow for time for Pi to connect to the camera
- `&` allows for this execution to occur in background

Upon editing `/etc/rc.local`, run `sudo chmod +x /etc/rc.local` to have autorun work.

## Streaming
The stream.sh script uses rpicam along with gstreamer to stream video to the Nvidia Jetson TX2 over UDP. There are two things to note:
- The framerate is 56 instead of 60 since the camera can only stream in sensor mode 2304:1296 at a max of 56 FPS. If the FPS is greater than this then the camera will use sensor mode 1532:864 instead which is automatically cropped and not native resolution.
- The UDP address is the Jetson's address. The address is also hardcoded since the Pi and Jetson are connected to a router which has a DHCP reservation for their IPs.
- The width and height are locked to 1920x1080. Any attempt to go higher will not work since the Pi is limited by its H.264 capabilities. You can use MJPG as the codec instead but this requires modifying the gstreamer flags.
  
The stream.sh script can be executed from startup by following the same process as above with the footage.sh script

