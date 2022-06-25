# BruteForceOTG

This project is based on:
* ALPFINDER: https://github.com/lpinqn/alpfinder from Sebastian Gomez - 2018 -  Gabinete de Pericias Informaticas - Neuquen - Argentina
* BFARDUINO: https://github.com/rafaelcsch/BFARDUINO/ from Rafael Schneider - 2021 - Polícia Científica Santa Catarina - Brazil

BruteForceOTG uses an approach master-slave to brute force attack passwords on compatible devices:

* Master: BFTerminal is a C++ Qt app that supports dictionary loading, photocell calibration (for unlock detection), brute-force resume and bluetooth connection with the slave (client) device.
* Slave: BFClientBt is an .ino script for Arduino Leonardo that should be flashed for each combination of device/kind of password (like a PC driver, e.g. MOTOROLA_XT2053_PIN or Redmi_M1908_Pattern). Some scripts are available in "misc" folder.

## How does it work?

BFTerminal works by loading a wordlist and sending one password after another, taking a cooldown after X seconds per wrong password penalty, to BFClientBt which is a Leonardo board with bluetooth module and OTG cable connected to the device. 

A few seconds after guessing a password BFTerminal probes a photocell sensor attached to the display and if it's bright enough it stops (i.e last password was correct).

## Schematic

<img src="https://github.com/matnunes/BruteForceOTG/blob/main/misc/BruteForceOTG.png?raw=true" height=350px>


## User interface

<img src="https://github.com/matnunes/BruteForceOTG/blob/main/misc/UI.png?raw=true" height=500px>
