#!/usr/bin/bash

if [ ! -f "$1" ]; then
   echo "$1 no existe"
   exit  1
fi

if [ $(./versions check $1) -gt 0 ]; then
	echo "La version actual esta almacenada en el repositorio"
else
	echo "La version actual no esta almacenada en el repositorio!"
fi