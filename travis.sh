#!/usr/bin/env sh
set -evx
env | sort

mkdir build || true
cd build
#mkdir build/$SAFECASS_TARGET || true
#cd build/$SAFECASS_TARGET
../cmake332/bin/cmake \
      -DSAFECASS_BUILD_UNIT_TEST=ON \
      -DSAFECASS_USE_G2LOG=ON \
      ..
      #../$SAFECASS_TARGET
make
make test
