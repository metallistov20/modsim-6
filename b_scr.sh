#!/bin/sh


CheckUsb()
{

	lsusb

	sleep 1

	clear

	CheckUsb "$1"

	exit 0
}

CheckUsb "$1"
