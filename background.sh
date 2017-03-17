#! /bin/bash

# chmod +x background.sh
# ./background.sh
# GUI button should run this script to execute common.c
# when common.c retuns 0 the script restarts common.c

function updg(){

gcc -o a.out common.c

while ~/a.out

do
	echo "Restarting System"
	sleep 1
done
} 

#MAIN PROGRAM


echo -e "----------------------- RESTART SCRIPT -----------------------" 
sleep 1
updg			#updg() FUNCTION CALL

