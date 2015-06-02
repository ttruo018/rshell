#! /bin/bash

r=$1
s=$2

export bodytemp=""
while read body; do bodytemp="$bodytemp $body"; done

./body.sh $r $s | telnet
