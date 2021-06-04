# CANFDuino
Steps for installing and testing CANFDuino support for the Arduino IDE, running and testing your first CAN sketch:

Step 1. - Download the CANFDuino folder zipped in this reporsitory.

Step 2. - Close the Arduino IDE if you have it open. Unzip the folder and copy the whole folder to your local arduino hardware folder. The path for this hardware folder is: C:\Users\YOUR USERNAME\Documents\Arduino\hardware. The whole folder should be pasted directly into the hardware folder creating: C:\Users\YOUR USERNAME\Documents\Arduino\hardware\CANFDuino.

Step 3. - Open the Arduino IDE, Go to the "Tools->Board->CANFDuino" to select the CANFDuino board as your target board.

Step 4. - Go to File->Examples->CANFDuino->CANFDuino_Test500Kb.ino (have a look at all the other examples as well)

Step 5. - Wire the CANbus DB9 connectors or jump the 2pin headers on the PCB to connect CAN0 bus to the CAN1 bus. CAN0 High->CAN1 High, CAN1 Low->CAN1 Low.

Step 6. - Jumper at least one of the two termination resistors on the PCB enabling termnination resistors on the bus. (two ideally)

Step 7. - Connect USB cable to CANFDuino and the PC, this will power the unit an prompt for a dirver install. Wait for the driver to install if needed (skip obtaining from windows). Go to Tools->Port and select the COMM port that is labled CANFDuino. You may need to shut down the IDE and re-open it.

Step 8. - Go to Sketch->Upload. After compiling the IDE will send a signal to the CANFDuino to enter the bootloader. Note when the unit is bootloading the orange LED will stay on and strobe off quickly for 1-2 seconds. This occurs anytime the processor is reset (initial power on etc). You should see the green/red comms LED's flicker during sketch upload.

Step 9. - Go to Tools->Serial Monitor, note that this will also cause entry into bootloader for about 4-5 seconds before the sketch runs (for permanant code/bootloader bypass there is a jumper on the PCB).

Step 10. - See diagnostic printout in the comms window indicating pass/fail of testing. Typically the cause of failure is improper wiring or no termination resistor.

