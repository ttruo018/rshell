#! /bin/bash

r=$1
s=$2
#b=$3

domain=`cut -d "@" -f 2 <<< "$r"`

echo "open mail.cs.ucr.edu 25"
sleep 2

echo "helo $domain"
sleep 2

echo "mail from: $s"
sleep 2

echo "rcpt to: $r"
sleep 2

echo "data"
sleep 2

echo "Subject: This email is spam!"
sleep 2

echo ""
sleep 2

echo "$bodytemp"
sleep 2

echo "."
sleep 2

echo ""
sleep 2

echo "quit"
sleep 2

