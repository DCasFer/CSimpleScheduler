#!/bin/sh -e

PROJECT=$(pwd)
OPENOCD=/home/pitvid/openOcd/code/src

#Building
cd Debug
make all

#Deploying/Flashing
cd $OPENOCD

openocd -f /home/pitvid/openOcd/code/tcl/board/stm32f4discovery_Script.cfg \
  -c "init" \
  -c "reset init" \
  -c "flash write_image erase $PROJECT/Debug/C_Scheduler.elf" \
  -c "reset halt" \
  -c "resume" \
  -c "shutdown"
