#!/bin/bash
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

MYSELF="${0}"

__check__()
{
	DIE=0

	(test -f $srcdir/configure.ac) || {
		__ERROR__ "Directory "\`$srcdir\'" does not look like the top-level package directory"
		exit 1
	}

	(autoconf --version) < /dev/null > /dev/null 2>&1 || {
		__ERROR__ "You must have 'autoconf' installed."
		echo "Download the appropriate package for your distribution,"
		echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
		exit 1
	}

	(libtoolize --version) < /dev/null > /dev/null 2>&1 || {
	(glibtoolize --version) < /dev/null > /dev/null 2>&1 || {
	         __ERROR__ "You must have 'libtool' installed."
        	 echo "You can get it from:"
	         echo "  http://www.gnu.org/software/libtool/"
	         exit 1
	     }
	}

	(pkg-config --version) < /dev/null > /dev/null 2>&1 || {
	        __ERROR__ "You must have pkg-config installed to compile $package."
	        echo "Download the appropriate package for your distribution."
		exit 1
	}

	(automake --version) < /dev/null > /dev/null 2>&1 || {
		__ERROR__ "**Error**: You must have \`automake' (1.7 or later) installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 1
	}


	# if no automake, don't bother testing for aclocal
	test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
		__ERROR__ "Missing \`aclocal'.  The version of \`automake'"
		echo "installed doesn't appear recent enough."
		echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/"
		exit 1
	}

	__INFO__ "=============== Checking action was completed.  ==============+"
}

__execute__()
{
	ERROR=0

	__INFO__ "Processing libtoolize --copy --force || glibtoolize --copy --force"
	libtoolize --copy --force || glibtoolize --copy --force
	ERROR_NO="$?"
	if [ "${ERROR_NO}" -ne "0" ]; then
		__ERROR__ "Do 'libtoolize' or 'glibtoolize' return ${ERROR_NO}"
		__clean__
		exit 1		
	fi

	__INFO__ "Processing aclocal"
	aclocal
	ERROR_NO="$?"
	if [ "${ERROR_NO}" -ne "0" ]; then
		__ERROR__ "Do 'aclocal' return ${ERROR_NO}"
		__clean__
		exit 1
	fi

	__INFO__ "Processing automake --add-missing --copy --gnu"
	automake --add-missing --copy --gnu
	ERROR_NO="$?"
	if [ "${ERROR_NO}" -ne "0" ]; then
		__ERROR__ "Do 'automake' return ${ERROR_NO}"
		__clean__
		exit 1
	fi

	__INFO__ "Processing autoconf"
	autoconf
	ERROR_NO="$?"
	if [ "${ERROR_NO}" -ne "0" ]; then
		__ERROR__ "Do 'autoconf' return ${ERROR_NO}"
		__clean__
		exit 1
	fi

	__INFO__ "Done. Configure file and other files skipped."
	__INFO__ "Now only do './configure && make && make install'"
	exit 0
	
}
__clean__()
{
	rm -rf configure Makefile.in src/Makefile.in examples/Makefile.in m4 aclocal.m4 build-aux autom4te.cache
}

__help__()
{
	echo ""
	echo "USAGE:"
	echo "   ${MYSELF}     .......  Auto check exist tools needed and generate 'configure' and 'Makefile.in' files"
	echo "   ${MYSELF} -r  .......  Clean project, remove autoconf and automake's files"
	echo "   ${MYSELF} -h  .......  Show this information"
	echo ""
}

__ERROR__()
{
	echo "[${MYSELF}] ERROR: ${1}"
}
__INFO__()
{
	echo "[${MYSELF}] INFO: ${1}"
}

if [ ! -z "${1}" ]; then
	if [ "${1}" == "-r" ]; then
		__clean__
		__INFO__ "Clean project was completed."
		exit 0
	else
		__help__
		exit 1
	fi
else
	__check__

	__execute__
fi
