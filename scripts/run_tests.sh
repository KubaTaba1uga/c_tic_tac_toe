#!/bin/bash

builddir="$1"
test_name="$2"

if [ -z "$1" ]
then
    builddir="build"
fi

meson test $test_name -C $builddir

exit_code=$?
if [ $exit_code -ne 0 ]
then
    echo "Failed test with exit code: $exit_code"    
    exit $exit_code
fi

cat $builddir/meson-logs/testlog.txt
