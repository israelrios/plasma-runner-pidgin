#!/bin/bash

# Exit immediately if something fails
set -e

mkdir -p build
cd build
cmake ../ \
-DCMAKE_INSTALL_PREFIX=`kf5-config --prefix` \
-DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
kquitapp5 krunner 2> /dev/null; kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

