#!/bin/bash
#---------------------------------------------------------------------------------
# Set the target and compiler flags
#---------------------------------------------------------------------------------
CXXFLAGS=""
CFLAGS=""
export DEBUG_FLAGS=''
CONF_FLAGS=""
FULL_PKG_NAME=""
#---------------------------------------------------------------------------------
# Build and install
#---------------------------------------------------------------------------------
_configure_()
{
	if [ "$OS_TYPE" = "NA" ]; then
		OS_TYPE=$(expr substr "$(lsb_release -i)" 17 33)
	fi
	if [ $IS_MAKE -eq $TRUE ]; then
		if [ "$IS_RELEASE" = "NA" ]; then
                        FOLDER=build_${OS_TYPE}_debug
                else
                        FOLDER=build_${OS_TYPE}_release
                fi
	else
		if [ "$IS_RELEASE" = "NA" ]; then
			FOLDER=build_${OS_TYPE}_package_debug
		else
			FOLDER=build_${OS_TYPE}_package_release
		fi	
	fi	
	if [ "$DIR_INSTALL" = "NA" ]; then 
		DIR_INSTALL=/usr/local
	fi
	
	if [ "$IS_RELEASE" = "NA" ]; then
		CONF_FLAGS="--prefix=$DIR_INSTALL --enable-debug --enable-console"
	else
		CONF_FLAGS="--prefix=$DIR_INSTALL --enable-console"
	fi
}

_make_()
{
	
	mkdir -p $FOLDER && cd $FOLDER
	if [ ! -f Makefile ]; then
		echo ../configure $CONF_FLAGS 

		if [ "$IS_RELEASE" = "NA" ]; then
			export  CXXFLAGS="$CXXFLAGS"
			export  CFLAGS="$CFLAGS"
		else
			export  CXXFLAGS="$CXXFLAGS"
			export  CFLAGS="$CFLAGS"
		fi
		../configure $CONF_FLAGS 
	fi
	make V=s && $SUDO make install
}
create_install_sh()
{
	mkdir -p $FOLDER
	F_INSTALL=$FOLDER"/install.sh"
	rm -rf ${F_INSTALL}
	echo "make install" > "${F_INSTALL}"
	chmod +x ${FOLDER}/install.sh
}

TYPE_PACK_OP="-D"

_package_()
{
	create_install_sh

	echo mkdir -p $FOLDER 
	mkdir -p $FOLDER && cd $FOLDER
	if [ ! -f Makefile ]; then
		echo ../configure $CONF_FLAGS && 
		if [ "$IS_RELEASE" = "NA" ]; then
			export  CFLAGS="$CFLAGS"
			export  CXXFLAGS="$CXXFLAGS"

		else
			export  CXXFLAGS="$CXXFLAGS"
			export  CFLAGS="$CFLAGS"
		fi

		../configure $CONF_FLAGS 
	fi

	if [ "$OS_TYPE" != "Debian" ]; then
		TYPE_PACK_OP="-R"	
	fi

	ARCH=$(uname -m)
	if [ "$ARCH" = "i686" ]; then
		ARCH="i386"
	elif [ "$ARCH" = "x86_64" ]; then
		ARCH="amd64"
	elif [ "$ARCH" = "armv7l" ]; then
		ARCH="armhf"
	fi

	FULL_PKG_NAME=${PACKAGE_NAME}_${PACKAGE_VERSION}-${PACKAGE_RELEASE}_${ARCH}
	PUSH_PKG_NAME=${PACKAGE_NAME}_${PACKAGE_VERSION}_${ARCH}
	make V=s && 
	
	sudo checkinstall $TYPE_PACK_OP --docdir=../doc-pak/ \
				-y --install=no --fstrans=no \
				--exclude=/selinux \
				--reset-uids=yes \
				--pkgname=$PACKAGE_NAME  \
				--pkgversion=$PACKAGE_VERSION  \
				--pkgrelease=$PACKAGE_RELEASE \
				--pkgarch=$ARCH \
				--maintainer="STYL Solutions" \
				--pakdir=$DIR_PACKAGE --backup=no -D $(pwd)/install.sh \
				--requires="glib-2.0 libudev" && 
	cd - || exit 1
}

_help_()
{
	echo "=========================================================================="
	echo "=========================================================================="
	echo ""
	echo "Usage: $0 [OPTION] [BUILD_OPTION] | $0 [OPTION] [PACKAGE_OPTION]"
	echo ""
	echo "OPTION:"
	echo "  -su     		: build/package STYLAGPS library with sudo"
	echo "  -r              	: package STYLAGPS library with disable debug"
	echo "BUILD_OPTIONS:"
	echo "  -m              	: make and install STYLAGPS library"
	echo "  -d <dir>     		: dir install of STYLAGPS library"
	echo "PACKAGE_OPTIONS:"
	echo "  -p              	: package STYLAGPS library"
	echo "  -pkg_name <name>	: set name of package of STYLAGPS library"
	echo ""
	echo "============================================================================"
	echo "============================================================================"
}

FALSE=0
TRUE=1
OS_TYPE="NA"
ENABLE_STATIC=""
DIR_INSTALL="NA"
DIR_PACKAGE=""
PACKAGE_NAME=LIBSTYL_AGPS
PACKAGE_VERSION="0.0.1"
PACKAGE_RELEASE=$(git log --pretty=format:\'\' | wc -l)
IS_MAKE=$FALSE
IS_PACKAGE=$FALSE
IS_RELEASE="NA"
SUDO=""
for PARAM in $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12}
do
	case $PRE_PAR in
		-d) DIR_INSTALL=$PARAM
			PRE_PAR=""
			continue;;
		-pkg_name)	
			PACKAGE_NAME=$PARAM
			PRE_PAR=""
			continue;;
		-pkg_version)	
			PACKAGE_VERSION=$PARAM
			echo PACKAGE_VERSION=$PACKAGE_VERSION
			PRE_PAR=""
			continue;;
		-pkg_release)	
			PACKAGE_RELEASE=$PARAM
			PRE_PAR=""
			continue;;

		*);;
	esac
	case $PARAM in
		-su) 		SUDO="sudo";;
		-r) 		IS_RELEASE="-D__RELEASE__";;
		-d) 		;;
		-m)     	IS_MAKE=$TRUE;;
                -p)     	IS_PACKAGE=$TRUE;;
                -push)  	IS_PUSH=$TRUE;;
		-pkg_name) 	;;
		-pkg_version) 	;;	
		-pkg_release) 	;;	
		*) 		_help_
				exit 1;;
	esac
	PRE_PAR=$PARAM
done
IS_DO=$FALSE
_configure_

if [ $IS_MAKE -eq $TRUE ]; then
	IS_DO=$TRUE
	_make_
elif [ $IS_PACKAGE -eq $TRUE ]; then
	IS_DO=$TRUE
	_package_
fi
if [ $IS_DO -eq $FALSE ]; then
	_help_
	exit 1
fi
