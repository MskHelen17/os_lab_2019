#!/bin/bash

    for ((i=0; i < 150; i++))
    do
        od -An -td -N1 /dev/random >> numbers.txt
    done
exit 0