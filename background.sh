#! /bin/bash

function updg(){

gcc -o a.out common.c
./a.out

while ! ~/a.out

do
	echo "Restarting"
	./a.out
	sleep 1
done
} 

#MAIN PROGRAM


echo -e "----------------------- RESTART SCRIPT -----------------------" 
sleep 1
updg			#updg() FUNCTION CALL

