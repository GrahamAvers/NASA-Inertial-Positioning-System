# Onboard Code

This is all of the code loaded onto the arduino's for flight

## Adafruit1Control
This is the code responsible for the first single core Adalogger. It records data from the two altimeters and stores them to SD.

## Adafruit2Arducam
This is the code responsible for the second single core Adalogger. It is communicates with the first adalogger and takes and stores pictures at the correct altitudes. 
## ESP32xsens
This is the code responsible for the dual core feather. It powers the xsens, communicates with both of the adaloggers for synchronization, and it also stores data from the Xsens at 100 Hz.
