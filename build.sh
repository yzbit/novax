#!/bin/bash

cd $(dirname $0)
root=$(pwd)

cd lib/googletest
rm -rf build/
mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=../../gtest .. 
make -j10 && make install

cd $root
echo "-----$root"
rm -rf build/
mkdir build && cd build/

cmake -DCMAKE_INSTALL_PREFIX=../output/ ..
make -j10 && make install
cd -

