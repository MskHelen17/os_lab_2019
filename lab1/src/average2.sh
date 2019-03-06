#!/bin/bash
    exec 0<numbers.txt
    i=0
    s=0
    while read line
    do 
      echo "Аргумент #$i = $line" 
      let "s+=line"
      let "i+=1"
    done
    echo "Количество входных аргументов $i"
    let "s=s/i"
    echo "Среднее арифметическое входных аргументов $s"
exit 0