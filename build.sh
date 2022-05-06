#!/bin/sh

for d in $PWD/*/ ; do
    BUILD_DIR=${d}build
    rm -fr $BUILD_DIR
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake ..
    make -j$(nproc)
done
