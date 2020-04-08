# Alexa Enabled Home School Tracker and Timer

This repository is to accompany my Alexa Enabled eLearning Tracker Video:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/ZWKHSF1f65w/0.jpg)](https://www.youtube.com/watch?v=ZWKHSF1f65w)


## Initial Setup

1. Download the ClassTimer.bin file from the RELEASES tab in this github repository
2. Upload the .bin file to your NodeMCU using a tool like [NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher/releases)
3. Connect D5 to GND on the NodeMCU to enter setup mode to configure your WiFi
4. Connect to the "School Timer Setup" SSID and enter your credentials

## Alexa Specific Commands

1. Setting any of the 5 devices to 99% will clear all LEDs on the timer.
2. The brightness of a specific device corresponds to the timer length for that subject (20% brightness = 20 minute timer)
3. Setting a device to "off" will turn on its LEDs to signify that the subject is complete.



## Parts (Amazon Links)

NodeMCU Microcontroller: https://amzn.to/3e6SDSM

Addressible LEDS: https://amzn.to/3aTr5yd

Jumper Wires: https://amzn.to/3e1PFPj
