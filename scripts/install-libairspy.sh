#!/bin/sh

wget -O airspyone_host-1.0.9.tar.gz https://github.com/airspy/airspyone_host/archive/v1.0.9.tar.gz
tar xf airspyone_host-1.0.9.tar.gz

mkdir airspyone_host-1.0.9/build
cd airspyone_host-1.0.9/build

cmake ../ -DINSTALL_UDEV_RULES=ON
make

sudo make install
sudo ldconfig
