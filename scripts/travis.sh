#!/usr/bin/env sh
set -evx
env | sort

mkdir build || true
cd build
#mkdir build/$SAFECASS_TARGET || true
#cd build/$SAFECASS_TARGET

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    export CMAKE_BIN="../cmake332/bin/cmake";
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    export CMAKE_BIN="../cmake332/cmake-3.3.2-Darwin-x86_64/CMake.app/Contents/bin/cmake";
else
    # fallback
    export CMAKE_BIN="cmake";
fi

${CMAKE_BIN} \
      -DSAFECASS_ENABLE_UNIT_TEST=ON \
      -DSAFECASS_USE_G2LOG=ON \
      ..
      #../$SAFECASS_TARGET
make
make test
