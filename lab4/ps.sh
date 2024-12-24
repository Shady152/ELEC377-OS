#!/bin/bash

# ELEC377 - Operating Systems
# Lab 4 - Shell Scripting, ps.sh

# Program Description:
# The purpose of this script is to show a list of all running processes on the system
# The script can include additional information such as resident set size, executing 
# command name, name of the command line in use, and the group ID of the process

# Init vars to 0
show_rss=0
show_comm=0
show_cl=0
show_group=0

# Look through the command line args
while [[ $# -gt 0 ]]; do
	# Determine what args we want to show
	if [[ $1 == "-rss" ]]; then
		show_rss=1
	elif [[ $1 == "-comm" ]]; then
		show_comm=1
	elif [[ $1 == "-command" ]]; then
		show_cl=1
	elif [[ $1 == "-group" ]]; then
		show_group=1
	else
		echo "ERROR: Argument $1 is not recognized!"
		exit 1
	fi
	
	# Shift to the next arg
	shift
done

# Checks if comm and command are both args | they are mutually exclusive
if [ $show_comm -eq 1 ] && [ $show_cl -eq 1 ]; then
	echo "ERROR: -comm and -command are mutually exclusive!"
	exit 1
fi

# Testing Flags
# echo "RSS: 	$show_rss"
# echo "COMM: 	$show_comm"
# echo "CL:	$show_cl"
# echo "GROUP: 	$show_group"

# Make the temp file
tmp_file="/tmp/tmpPs$$.txt"

# Loop through proc files in /proc dir
for p in /proc/[0-9]*; do
	# Check that the dir exists
	if [[ -d $p ]]; then
		# Get info
		pid=$(grep '^Pid:' "$p/status" | awk '{print $2}')
		name=$(grep '^Name:' "$p/status" | awk '{print $2}')
		uid=$(grep '^Uid:' "$p/status" | awk '{print $2}')
		gid=$(grep '^Gid:' "$p/status" | awk '{print $2}')
		rss=$(grep '^VmRSS:' "$p/status" | awk '{print $2}')
		
		# Replace null chars with spaces
		cmd_line=$(tr '\0' ' ' < "$p/cmdline")
		
		# Check that the cmd_line isn't empty
		if [[ -z "$cmd_line" ]]; then
			cmd_line=$name
		fi

		# Check if rss is empty
		if [[ -z "$rss" ]]; then
			rss=0
		fi
		
		# Convert user and group IDs from numberic to symbolic
		user=$(grep ":x:$uid:" /etc/passwd | cut -d: -f1)
		group=$(grep ":x:$gid:" /etc/group | cut -d: -f1)
		
		# Only copy the relevant headers to the temp file
		printf "%s\t\t%s" $pid $user >> $tmp_file
		if [[ $show_rss -eq 1 ]]; then
			printf "\t\t%s" $rss >> $tmp_file
		fi
		if [[ $show_group -eq 1 ]]; then
			printf "\t\t%s" $group >> $tmp_file
		fi
		if [[ $show_comm -eq 1 ]]; then
			printf "\t\t%s" $name >> $tmp_file
		fi
		if [[ $show_cl -eq 1 ]]; then
			printf "\t\t%s" $cmd_line >> $tmp_file
		fi

		printf "\n" >> $tmp_file
	
	fi
done

# Print relevant headers
printf "%s\t\t%s" "PID" "USER"
if [[ $show_rss -eq 1 ]]; then
	printf "\t\t%s" "RSS"
fi
if [[ $show_group -eq 1 ]]; then
	printf "\t\t%s" "GROUP"
fi
if [[ $show_comm -eq 1 ]]; then
	printf "\t\t%s" "COMMAND"
fi
if [[ $show_cl -eq 1 ]]; then
	printf "\t\t%s" "CMD LINE"
fi

printf "\n"

# Sort and display the file
cat $tmp_file | sort -n
rm "$tmp_file"

