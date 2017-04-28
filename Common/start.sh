#! /bin/bash

function updg(){

make
cd main
make
cd ..
./smart_plants

} 

#MAIN PROGRAM


echo -e "\n\n----------------------- START SCRIPT -----------------------\n\n" 
sleep 1
updg			#updg() FUNCTION CALL

