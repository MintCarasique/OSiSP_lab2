#!/bin/bash
ST_IFS=$IFS
IFS=$'\n'
#DIR="/home/snowqueen/work"
ERR="/tmp/err.log"

find $(readlink -f $1) -type f -name "$2" -printf "%p %s %Ad %Ab %AH:%AM %M %i\n" 2>$ERR   

rm $ERR

IFS=$ST_IFS