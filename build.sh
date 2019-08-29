#!/bin/bash 

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-../build}
TARGET="Server"

mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $SOURCE_DIR
make
mv $TARGET $SOURCE_DIR
cd $SOURCE_DIR
rm -rf $BUILD_DIR
