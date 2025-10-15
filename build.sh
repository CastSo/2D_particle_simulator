#!/bin/sh

mkdir -p build
cmake -c "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -B build
cd build
make all
cd ..