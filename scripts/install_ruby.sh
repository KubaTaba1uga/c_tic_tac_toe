#!/bin/bash
OS=$(hostnamectl | grep "Operating System")


if command -v ruby &> /dev/null
then
    exit 0
fi

case "$OS" in
    *"Debian GNU/Linux"*)
	sudo apt update
	sudo apt install ruby-full

	if ! command -v bundle &> /dev/null
	then
	    echo "alias bundle='bundle3.1'" >> ~/.bashrc
	fi
	;;
    *)
	echo "OS not supported"
	exit 1
	;;
    esac
