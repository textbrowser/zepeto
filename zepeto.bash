#!/bin/bash

rc=1

if [ -x ./zepeto.bin ]
then
    filename="$(./zepeto.bin $*)"
elif [ -x /usr/local/bin/zepeto.bin ]
then
    filename="$(/usr/local/bin/zepeto.bin $*)"
fi

if [ -r "$filename" ]
then
    source $filename
    rc=$?
    rm -f $filename
fi

if [ -e "$filename" ]
then
    rm -f $filename
fi

return $rc
