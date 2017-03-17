#! /bin/bash

# sudo chmod +x background.sh
# ./background.sh
# Final make file should compile this script and run it


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


echo -e "-----------------------CRASH SCRIPT-----------------------" 
sleep 1
updg			#updg() FUNCTION CALL

