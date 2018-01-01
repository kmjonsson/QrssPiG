#!/bin/sh
set -ex

wget -O limesuite-v17.12.0.tar.gz https://github.com/myriadrf/LimeSuite/archive/v17.12.0.tar.gz
tar xf limesuite-v17.12.0.tar.gz

mkdir -p LimeSuite-17.12.0/build && cd LimeSuite-17.12.0/build && cmake ../ && make -j 4 && sudo make install && sudo ldconfig
