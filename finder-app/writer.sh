#!/bin/sh

if [ "$#" -ne 2 ] ;
then
	echo "Incorrect number of arguments";
	exit 1
fi

directory=$(dirname "$1");

# Check if the directory exists, if not, create it
if [ ! -d "$directory" ]; then
    mkdir -p "$directory" || { echo "Failed to create directory $directory"; exit 1; }
fi

echo "$2" > "$1"

