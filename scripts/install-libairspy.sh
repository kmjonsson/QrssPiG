#!/bin/sh
set -ex

wget -O airspy-v1.0.9.tar.gz https://github.com/airspy/airspyone_host/archive/v1.0.9.tar.gz
tar xf airspy-v1.0.9.tar.gz

mkdir airspyone_host-1.0.9/build
cd airspyone_host-1.0.9/build

cmake ../ -DINSTALL_UDEV_RULES=ON
make -j 4

sudo make install
sudo ldconfig
