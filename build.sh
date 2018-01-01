#!/bin/bash
mkdir -p bin
cd src
g++ -o ../bin/imgarc *.cpp
chmod +x ../bin/imgarc
cd ../
