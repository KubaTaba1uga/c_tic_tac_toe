#!/bin/bash

builddir="$1"
test_name="$2"

if [ -z "$1" ]
then
    builddir="build"
fi

meson test --wrap='valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 -s' $test_name -C $builddir

exit_code=$?
if [ $exit_code -ne 0 ]
then
    exit $exit_code
fi

cat $builddir/meson-logs/testlog-valgrind.txt
