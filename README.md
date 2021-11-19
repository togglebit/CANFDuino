# CANFDuino
![Image](https://i1.wp.com/togglebit.net/wp-content/uploads/2021/08/canfduino-front-open-enclosure_jpg_project-main.jpg?fit=749%2C421&ssl=1)

## Intro
CANFDuino is the best platform for open source CAN bus Arduino projects. It combines several essential features into one complete "ready-to-modify" package for the real world, including dual CANFD bus ports, SD card slot, and a multitude of analog/digital IO. It also includes a built-in prototyping shield for SMT and through-hole parts, and rugged packaging and connections.

## Hardware Hookups
For making connections the **hookup guide** is here: [https://togglebit.net/canfduino-hookup-guide/](https://togglebit.net/canfduino-hookup-guide/)

## Installing the Library and First Sketch
The following are steps for installing and testing CANFDuino support for the Arduino IDE, running and testing your first CAN sketch:

Note this content can also be found here [https://togglebit.net/getting-started-with-canfduino/](https://togglebit.net/getting-started-with-canfduino/)

**Step 1.** - Download the entie codebase, un-zip it. Rename "CANFDuino-master" to "CANFDuino".

**Step 2.** - Close the Arduino IDE if you have it open. Copy the whole "CANFDuino" folder to your local arduino hardware folder. The path for this hardware folder is: C:\Users\YOUR USERNAME\Documents\Arduino\hardware. The whole folder should be pasted directly into the hardware folder creating: C:\Users\YOUR USERNAME\Documents\Arduino\hardware\CANFDuino. If you do not have a hardware folder, create one and place CANFDuino folder inside of it.

**Step 3.** - Open the Arduino IDE, Go to the "Tools->Board->CANFDuino" to select the CANFDuino board as your target board.

**Step 4.** - Go to Tools->Board->-Boards Manager-Wait for the downloads to finish. Click on “Arduino SAMD Boards (32-bits ARM Cortex-M3), including Arduino M0” and Install. This can take a while to download if you don't already have the library.

**Step 5.** - Go to File->Examples->CANFDuino->CANFDuino_Test500Kb.ino (have a look at all the other examples as well)

**Step 6.** - Wire the CANbus DB9 connectors or jump the 2pin headers on the PCB to connect CAN0 bus to the CAN1 bus. CAN0 High->CAN1 High, CAN1 Low->CAN1 Low.

**Step 7.** - Jumper at least one of the two termination resistors on the PCB enabling termnination resistors on the bus. (two ideally)

**Step 8.** - Connect USB cable to CANFDuino and the PC, this will power the unit an prompt for a dirver install. Wait for the driver to install if needed (one option is to skip obtaining from windows and selecting the driver in the hardware\samd\drivers folder). Go to Tools->Port and select the COMM port that is labled CANFDuino. You may need to shut down the IDE and re-open it after installing the driver. (if needed, use FTDI exe in this repository to install driver)

**Step 9.** - Go to Sketch->Upload. After compiling the IDE will send a signal to the CANFDuino to enter the bootloader. Note when the unit is bootloading the orange LED will stay on and strobe off quickly for 1-2 seconds. This occurs anytime the processor is reset (initial power on etc). You should see the green/red comms LED's flicker during sketch upload.

**Step 10.** - Go to Tools->Serial Monitor, note that this will also cause entry into bootloader for about 4-5 seconds before the sketch runs (for permanant code/bootloader bypass there is a jumper on the PCB).

**Step 11.** - See diagnostic printout in the comms window indicating pass/fail of testing. Typically the cause of failure is improper wiring or no termination resistor.

## Example Code
**CANFduino_CANTerm.ino**

CANTerm is a cheapo 2 port CAN/CANFD packet monitor that can be used in simple terminal programs regardless of OS without special PC software. The CANFDuino is used to print packet payloads to the screen in static locations for easy viewing. Supports multiple CAN and CANFD baud rates, ID range filtering and stores the last settings into flash.

CANTerm has been tested with PuTTY terminal program (2MBaud)
https://www.chiark.greenend.org.uk/~sgtatham/putty/

Monitor CAN bus traffic in the comfort of a Chrome browser with serial terminal from google's chromelabs! (2MBaud) 
https://googlechromelabs.github.io/serial-terminal/


