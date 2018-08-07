#!/bin/sh -e

PROJECT=$(pwd)

#Building
cd Debug
make all

#Deploying/Flashing
openocd -f /board/stm32f4discovery_Script.cfg \
  -c "init" \
  -c "reset init" \
  -c "flash write_image erase $PROJECT/Debug/C_Scheduler.elf" \
  -c "reset halt" \
  -c "resume" \
  -c "shutdown"
