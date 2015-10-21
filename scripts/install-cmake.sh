#!/bin/sh
set -ex

# TEMP
export TRAVIS_OS_NAME="osx";


# Detect travis OS and set cmake package file name to download
travis_on_linux=true
if [ "$TRAVIS_OS_NAME" = "osx" ]; then
    export CMAKE_PACKAGE_NAME="cmake-3.3.2-Darwin-x86_64.tar.gz";
    travis_on_linux=false
elif if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    export CMAKE_PACKAGE_NAME="cmake-3.3.2-Linux-x86_64.sh";
fi

echo CMake package to download (${TRAVIS_OS_NAME}): ${CMAKE_PACKAGE_NAME}

# Download cmake pacakge (minimum 3.0 due to external project support)
wget --no-check-certificate https://cmake.org/files/v3.3/${CMAKE_PACKAGE_NAME}

# For Linux
if [ "$travis_on_linux" = true ]; then
  chmod +x ${CMAKE_PACKAGE_NAME}
  mkdir cmake332
  cd cmake332
  ../${CMAKE_PACKAGE_NAME} --skip-license
  cd ..
# For OS X
else
  mkdir cmake332
  cd cmake332
  tar -zxf ../${CMAKE_PACKAGE_NAME}
  cd ${CMAKE_PACKAGE_NAME}
  echo ${PATH}
  export PATH=`pwd`/cmake-3.3.2-Darwin-x86_64/CMake.app/Contents/bin:${PATH}
  cd ../../
fi
