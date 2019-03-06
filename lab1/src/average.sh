#!/bin/bash
    i=0
    s=0
    for arg in "$@"
    do 
      echo "Аргумент #$i = $arg" 
      let "s+=arg"
      let "i+=1"
    done
    echo "Количество входных аргументов $i"
    let "s=s/i"
    echo "Среднее арифметическое входных аргументов $s"
exit 0