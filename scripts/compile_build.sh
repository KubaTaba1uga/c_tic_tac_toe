#!/bin/bash

builddir="$1"

if [ -z "$1" ]
then
    builddir="build"
fi

meson compile -C $builddir  
