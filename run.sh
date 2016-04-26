#!/bin/bash

export LD_LIBRARY_PATH=/home/arek/opencv-2.4.9/release/bin/lib:$LD_LIBRARY_PATH
make clean
make
./bin/recognizer
