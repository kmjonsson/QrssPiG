# QrssPiG

QrssPiG is short for QRSS (Raspberry)Pi Grabber.

Haven't found a headless standalone grabber for my Pi. So I try to create my own.
If i find one working for my needs I might well using the existing one and stop this.

## Functionality
 - Headless standalone daemon
 - Able to process I/Q stream from an rtl-sdr, HackRF or other sdr devices
 - Optionally control the sdr device
 - Optionally process audio from stream or audio input
 - Generate pretty horizontal waterfall graphs, aka curtain
 - Upload them via scp
 - Optionally upload them wia (s)ftp

## Build
To build QrssPiG you need cmake

It depends on following dev libs:
 - libboost-program-options-dev
 - libgd-dev
 - libssh-dev
 - libfftw3-dev

To build:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
