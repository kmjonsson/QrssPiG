#!/bin/sh
set -ex

wget -O hackrf-v2015.07.2.tar.gz https://github.com/mossmann/hackrf/archive/v2015.07.2.tar.gz
tar xf hackrf-v2015.07.2.tar.gz

mkdir -p hackrf-2015.07.2/host/build && cd hackrf-2015.07.2/host/build && cmake ../ && make -j 4 && sudo make install && sudo ldconfig
