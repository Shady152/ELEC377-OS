#!/bin/bash

if [ "$#" -lt 2 ] || [ "$#" -gt 5 ]; then
	echo "ERROR: Can only merge 2 - 5 file!" 	# Max of 5 in the provided main.c
	exit 1
fi

test_number="0" # default | we start at 1 so should be a good indicator
for file in "$@"; do
	if [ -f "$file" ]; then
		test_number=$(echo "$1" | grep -o '[1-9]' | head -n 1)
		break
	else
		echo "WARNING: '$file' not found | Will not be included in the merge"
	fi
done
	
merged_file="merged_out${test_number}"

if [ -f "$merged_file" ]; then
	echo "This file already exists | Exiting out of fear of doing a bad"
	exit 1
fi

for file in "$@"; do
	if [ -f "$file" ]; then
		cat "$file" >> "$merged_file"
	else
		echo "WARNING: '$file' not found | Will not be included in the merge"
	fi
done

echo "Merged File Saved as: '$merged_file'"

source sort_check_dups_out.sh "$merged_file"
