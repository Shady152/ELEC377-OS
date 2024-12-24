#!/bin/bash

bs=./ps.sh
output=./tests

# TEST 0: NO OPTIONS
$bs > $output/test0_lab.txt
ps -eo pid,user > $output/test0_sys.txt
echo "TEST 0 DONE"

# TEST 1: RSS
$bs -rss > $output/test1_lab.txt
ps -eo pid,user,rss > $output/test1_sys.txt
echo "TEST 1 DONE"

# TEST 2: GROUP
$bs -group > $output/test2_lab.txt
ps -eo pid,user,group > $output/test2_sys.txt
echo "TEST 2 DONE"

# TEST 3: COMM
$bs -comm > $output/test3_lab.txt
ps -eo pid,user,comm > $output/test3_sys.txt
echo "TEST 3 DONE"

# TEST 4: CMD LINE
$bs -command > $output/test4_lab.txt
ps -eo pid,user,command > $output/test4_sys.txt
echo "TEST 4 DONE"

# TEST 5: RSS, GROUP
$bs -rss -group > $output/test5_lab.txt
ps -eo pid,user,rss,group > $output/test5_sys.txt
echo "TEST 5 DONE"

# TEST 6: RSS, COMM
$bs -rss -comm > $output/test6_lab.txt
ps -eo pid,user,rss,comm > $output/test6_sys.txt
echo "TEST 6 DONE"

# TEST 7: RSS, CMD LINE
$bs -rss -command > $output/test7_lab.txt
ps -eo pid,user,rss,command > $output/test7_sys.txt
echo "TEST 7 DONE"

# TEST 8: RSS, COMM, GROUP
$bs -rss -comm -group > $output/test8_lab.txt
ps -eo pid,user,rss,comm,group > $output/test8_sys.txt
echo "TEST 8 DONE"

# TEST 9: RSS, GROUP, CMD LINE 
$bs -rss -group -command > $output/test9_lab.txt
ps -eo pid,user,rss,command,group > $output/test9_sys.txt
echo "TEST 9 DONE"

# TEST 10:  GROUP, COMM
$bs -comm -group > $output/test10_lab.txt
ps -eo pid,user,comm,group > $output/test10_sys.txt
echo "TEST 10 DONE"

# TEST 11: GROUP, CMD LINE
$bs -command -group > $output/test11_lab.txt
ps -eo pid,user,command,group > $output/test11_sys.txt
echo "TEST 11 DONE"

# TEST 12: COMM, CMD LINE
$bs -comm -command > $output/test12_lab.txt
echo "TEST 12 DONE"

# TEST 13: -test
$bs -test > $output/test13_lab.txt
echo "TEST 13 DONE"
