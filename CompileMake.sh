#!/bin/bash
echo "Compiling All /src .c file headers"
cd src
touch addon.cc
gcc -Wall -fPIC -c portal.c portal.h serial.c serial.h

echo "MAKE"
cd ../build
sudo make

echo "RUN TEST "
cd ../test
sudo node test_binding.js

read -p  "press any key to continue"
