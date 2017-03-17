BY: HAMAD KHAWAJA

Installing GTK+ Library on RaspberryPi Linux
--------------------------------------------

sudo apt-get install libgtk2.0-dev
sudo apt-get install libgtk-3-dev

echo `pkg-config --cflags --libs gtk+-3.0`


Installing Glade
----------------

sudo apt-get install glade (GUI EDITOR)

Installing Geany
----------------

sudo apt-get install geany (TEXT EDITOR/ NOT MANDATORY)


Dependencies
------------
gcc -o main gui.c `pkg-config --cflags --libs gtk+-3.0`

running using make 
------------------

make 
make clean

