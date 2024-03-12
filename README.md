# Raspi-FootageCapture

main.cpp is for footage capture on Raspberry Pi.
- uses c++ code to call cmd commands to record picam footage (via libcamera commands) to a USB drive named "KINGSTON"
- also logs console output (stdout and stderr) in file named "{date}-log.txt" and manages counters for capture sessions in file named "cam-log.txt"
- also flashes indicator lights via GPIO (assuming GPIO27/Pin13 is Yellow and GPIO17/Pin11 is Red):
  - Yellow ON = camera is recording
  - Yellow OFF AND Red ON = camera is NOT recording (probably camera wire connection issues)
  - Yellow ON AND Red ON = camera is recording, Red light just blinks every recorded block to show code isn't frozen

__

setoff.sh is for execution at startup
- calls the executable in the project file, based on the current project organization

__

not included is /etc/rc.local which is used in the pi to cause execution on startup. added code to file is as follows:
> sleep 10
> 
> sh /home/FishCenS/Documents/FishCode/FootageCapture/setoff.sh &

- sleep to allow for time for raspi to connect to camera
- & to allow for this execution to occur in background

upon editing /etc/rc.local, requires following cmd command to have autorun work:
 > sudo chmod +x /etc/rc.local
