#/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Please input ONE file"
	exit 1
fi

if [ ! -f "$1" ]; then
	echo "ERROR: File done not exist!"
	exit 1
fi

sort -n "$1" -o "$1"

dups=$(uniq -d "$1")

if [ -z "$dups" ]; then
	echo "No duplicates found."
else
	echo "DUPLICATES FOUND!"
	echo "$dups"
fi
