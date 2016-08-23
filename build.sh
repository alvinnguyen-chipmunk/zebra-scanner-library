#!/bin/bash

make lib
make
export LD_LIBRARY_PATH=$PWD
