#!/bin/bash
echoerr() { echo "$@" 1>&2; }

OS=$(hostnamectl | grep "Operating System")



# Check if already installed
if command -v meson &> /dev/null
then
    exit 0
fi




case "$OS" in
    *"Debian GNU/Linux 12"*)

	# Check packages required for installation.
	if ! command -v sudo &> /dev/null
	then
	    echoerr "`sudo` is required!"
	    exit 2
	fi

	# Install meson
	sudo apt-get update
	sudo apt-get install -y meson
	
	;;
    *"Ubuntu 22"*)
	# Install meson
        sudo apt install python3-pip ninja-build -y
        sudo pip3 install meson

	;;
    *)
	echoerr "$OS not supported"
	exit 1
	;;
    esac
