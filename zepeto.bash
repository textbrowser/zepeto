#!/bin/bash

filename="$(/usr/local/bin/zepeto.bin $*)"

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
