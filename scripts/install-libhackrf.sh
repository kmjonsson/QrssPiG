#!/bin/sh
set -ex

wget -O hackrf-v2017.02.1.tar.gz https://github.com/mossmann/hackrf/archive/v2017.02.1.tar.gz
tar xf hackrf-v2017.02.1.tar.gz

mkdir -p hackrf-2017.02.1/host/build && cd hackrf-2017.02.1/host/build && cmake ../ && make -j 4 && sudo make install && sudo ldconfig
