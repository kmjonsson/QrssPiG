#!/bin/sh

wget -O airspyhf-1.0.tar.gz https://github.com/airspy/airspyhf/archive/1.0.tar.gz
tar xf airspyhf-1.0.tar.gz

mkdir airspyhf-1.0/build
cd airspyhf-1.0/build

cmake ../ -DINSTALL_UDEV_RULES=ON
make

sudo make install
sudo ldconfig
