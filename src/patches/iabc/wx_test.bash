#!/bin/bash

TARGET_VERSION=2
TARGET_REV=6

ERROR_MSG="*** wx is not >= $TARGET_VERSION.$TARGET_REV
"
ERROR_M0SG=${ERROR_MSG}"*** or I can't find wx-config. Compile will fail.
"
ERROR_MSG=${ERROR_MSG}"*** Visit www.wxwidgets.org and download version
" 
ERROR_MSG=${ERROR_MSG}"*** $TARGET_VERSION.$TARGET_REV of the wxGTK libraries.
"
SUCCESS_MSG="wxWidgets is >= $TARGET_VERSION.$TARGET_REVISION.  Cool.
"
# First, make sure that the thing is installed.
wx-config --libs>&/dev/null
if  [ $? -eq "0" ]
then
	echo "wx-config exists. Compiling iABC with wxWidgets version:"
	wx-config --version
else
    echo $ERROR_MSG
    exit 1
fi

# Now, check the version
WX_VER=`wx-config --version|sed -e 's/\([0-9]\).[0-9].[0-9]/\1/'`
WX_REV=`wx-config --version|sed -e 's/[0-9].\([0-9]\).[0-9]/\1/'`

if [ $WX_VER -eq $TARGET_VERSION ]
then
if [ $WX_REV -ge $TARGET_REV ]
then
	echo "wx is >= $TARGET_VERSION.$TARGET_REV"
else
	echo "$ERROR_MSG"
	exit 1
fi
elif [ $WX_VER -gt $TARGET_VERSION ]
then
    echo "wx is >= $TARGET_VERSION.$TARGET_REV"
else
    echo $ERROR_MSG
    exit 1
fi

