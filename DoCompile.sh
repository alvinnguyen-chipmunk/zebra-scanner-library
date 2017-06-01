#!/bin/bash

${CC} -Wall -c mlsBarcode.c ssi_utils.c -I. -L.
${AR} -csr libstylssi.a mlsBarcode.o ssi_utils.o

${CC} -o stylagps_demo_static example/barcode_demo.c -lstylssi -I. -L.


