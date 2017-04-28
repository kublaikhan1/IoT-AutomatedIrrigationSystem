#! /bin/bash

# chmod +x background.sh
# ./background.sh
# GUI button should run this script to execute common.c
# when common.c retuns 0 the script restarts common.c

function updg(){

make
cd main
make
cd ..
./smart_plants

} 

#MAIN PROGRAM


echo -e "----------------------- START SCRIPT -----------------------" 
sleep 1
updg			#updg() FUNCTION CALL

