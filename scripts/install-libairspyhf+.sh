#!/bin/sh
set -ex

wget -O airspyhf-v1.0.tar.gz https://github.com/airspy/airspyhf/archive/1.0.tar.gz
tar xf airspyhf-v1.0.tar.gz

mkdir airspyhf-1.0/build
cd airspyhf-1.0/build

cmake ../ -DINSTALL_UDEV_RULES=ON
make -j 4

sudo make install
sudo ldconfig
