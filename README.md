Simple Rust Scheduler
=====================

This is a simple C/C++ scheduler for a STM32F4 Discovery board. The purpose is to schedule several simple tasks (LEDs) in a round robin manner.


### How to build
STM32F4 Discovery requires the arm-none-eabi tool chain to build and debug the project.

So it is required to install the following:

- A toolchain for arm-none-eabi. ( https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads ). This project has been developed in Linux, although it is possible to use other platforms such as Windows.


Once the toolchain is installed, Clone the repo:

    $ git clone 
    $ cd 



### How to flash
In order to flash the binary and also debug it, another open source tool is required. This is OpenOCD or Open On Chip Debuggger:

- OpenOCD ( http://openocd.org/ )


Once this tool is properly installed, attach an STM32F4 Discovery board via USB and run:

    $ ./buildFlashBoard.sh


### Use an IDE
I used Eclipse to create and develop this project, however I got rid of Eclipse files and created a simple script to build the final binary for simplicity. Therefore it is possible to use Eclipse + GNU MCU Eclipse Plug-In + OpenOCD Plug-In to debug and keep evolving this project. 


