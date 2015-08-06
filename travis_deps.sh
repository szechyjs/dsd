#!/bin/sh
set -ex

# Install mbelib
wget https://github.com/szechyjs/mbelib/archive/master.tar.gz -O mbelib.tar.gz
tar -xzf mbelib.tar.gz
cd mbelib-master
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/mbelib ..
make
make install

# Install IT++ from SF
if [ ! -d "$HOME/itpp/lib" ]; then
  wget http://sourceforge.net/projects/itpp/files/itpp/4.3.1/itpp-4.3.1.tar.gz/download -O itpp-4.3.1.tar.gz
  tar -xzf itpp-4.3.1.tar.gz
  cd itpp-4.3.1
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=$HOME/itpp ..
  make
  make install
else
  echo 'Using cached itpp.';
fi
