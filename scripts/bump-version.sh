#! /usr/bin/env bash

if [ "$1" != "Major" ] && [ "$1" != "Minor" ] && [ "$1" != "Patch" ]; then
    echo Missing or invalid Bump operation "$1"
    echo Syntax: bump-version.sh [Major|Minor|Patch]
    exit 1
fi

INIT_VERSION=$(cat version.txt)
a=(${INIT_VERSION//./ })

if [ "$1" = "Major" ]; then
   ((a[0]++))
   a[1]=0
   a[2]=0
elif [ "$1" = "Minor" ]; then
   ((a[1]++))
   a[2]=0
elif [ "$1" = "Patch" ]; then
   ((a[2]++))
else
   exit -1
fi

NEWVER="${a[0]}.${a[1]}.${a[2]}"

echo "Version $INIT_VERSION -> $NEWVER"
echo -n $NEWVER > version.txt