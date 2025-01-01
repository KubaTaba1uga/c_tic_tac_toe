#!/bin/bash

builddir="$1"
test_name="$2"

if [ -z "$1" ]
then
    builddir="build"
fi

meson test --wrap='valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --error-exitcode=1 -s' $test_name -C $builddir 


exit_code=$?
if [ $exit_code -ne 0 ]
then
    cat $builddir/meson-logs/testlog-valgrind.txt
    echo "Failed valgrind test with exit code: $exit_code"
    exit $exit_code
fi


