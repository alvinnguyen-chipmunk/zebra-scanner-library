#!/bin/bash

function main()
{
	do_compile
	do_package
}

function do_compile()
{
	# Compile static object
	${CC} -Wall -c mlsBarcode.c -I. -L.
	# Archive static lib
	${AR} -csr libstylssi.a mlsBarcode.o

	# Compile demo
	${CC} -o stylagps_demo_static example/barcode_demo.c -lstylssi -I. -L.
}

function do_package()
{
	tar cJf stylssi_static.tar.xz libstylssi.a stylagps_demo_static
}

main
