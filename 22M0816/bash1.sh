#!/bin/bash
for ((i=1; i<=$1; i++))
do
    bash bashscript.sh 8080 "response$i" 50 &
done