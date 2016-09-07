#!/bin/bash

if [[ ! -d ./release ]]; then
	mkdir release
fi
cd ./image
tar cJf ./mlsScanner.tar.xz ./*
cd ..
mv ./image/mlsScanner.tar.xz ./release
