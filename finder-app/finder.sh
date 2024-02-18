#!/bin/sh

if [ "$#" -ne 2 ] ;
then
	echo "Incorrect number of arguments";
	exit 1
fi

if [ ! -d "$1" ] ;
then
	echo "$1 is not a directory";
	exit 1
fi

files=$(find "$1" -type f| wc -l);
lines=$(grep -r "$2" "$1"| wc -l);
echo "The number of files are $files and the number of matching lines are $lines"

