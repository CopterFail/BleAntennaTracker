# Antenna Tracker Software

This software is build to drive an Antenna tracker with 360 degree pan an 90 degree tilt. 
Tilt is always driven by a servo. Pan can be drive by a servo or perfered by a stepper motor.

The software is an ESP32 / Arduino project. It runs on an ESP32 evalboard (doit) or on an ESP32C3 supermini (see deinitions in hardware.h).

Communication is optional (see deinitions in hardware.h):
- BLE, this requires ELRS with a special pull request (2305) for additional BLE communication in the TX software. The release without the pull will not work.
- 2 ELRS channels with an additional ELRS Rx in the tracker. The CRSF protokoll is used, PWM output is not required. ELRS 3.4.x allow 16 channels. The calculations for the tracker were done by lua scripts, running on the transmitter. I've test this with EdgeTx 2.10. You will find the lua scrips in the project EDGETX-LUA-micro-antenna-tracker 

## The mechanical tracker

See my 3D printed parts for my version of the antenna tracker: 
https://www.thingiverse.com/thing:6624610
