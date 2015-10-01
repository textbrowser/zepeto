#!/bin/sh

filename="`/usr/local/bin/zepeto.bin "$@"`"

if [ -r "$filename" ]
then
    . $filename
    rc=$?
    rm -f $filename
else
    rc=1
fi

return $rc
