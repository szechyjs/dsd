#!/bin/sh
set -ex

# Install mbelib
cd /tmp
wget https://github.com/szechyjs/mbelib/archive/master.tar.gz -O /tmp/mbelib.tar.gz
tar -xzf mbelib.tar.gz
cd mbelib-master
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install

# Install IT++ from SF
cd /tmp
wget http://sourceforge.net/projects/itpp/files/itpp/4.3.1/itpp-4.3.1.tar.gz/download -O /tmp/itpp-4.3.1.tar.gz
tar -xzf itpp-4.3.1.tar.gz
cd itpp-4.3.1
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
