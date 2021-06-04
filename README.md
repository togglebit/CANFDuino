# CANFDuino
Steps for installing and testing CANFDuino support to the Arduino IDE

Step 1. - Download the CANFDuino folder

Step 2. - Close the Arduino IDE if you have it open. Unzip the folder and copy the whole folder to your local arduino hardware folder. The path for this is: C:\Users\YOUR USERNAME\Documents\Arduino\hardware\.

Step 3. - Open the Arduino IDE, Go to the "Tool->Board->CANFDuino" to select the CANFDuino board as your target board.

Step 4. - Go to File->Examples->CANFDuino->CANFDuino_Test500Kb.ino (have a look at all the other examples as well)

Step 5. - Wire the DB9 connectors or 2pin headers on the PCB to connect CAN0 to CAN1. CAN0 High->CAN1 High, CAN1 Low->CAN1 Low.

Step 6. - Jumper at least one of the two termination resistors on the PCB, enabling termnination

Step 7. - Connect USB cable to CANFDuino and the PC, wait for the driver to install if needed (skip obtaining from windows). Go to Tools->Port and select the COMM port that is labled CANFDuino

Step 8. - Go to Sketch->Upload. After compiling the IDE will send a signal to the CANFDuino to enter the bootloader. Note when the unit is bootloading the orange LED will stay on and strobe off quickly for 1-2 seconds. This occurs anytime the processor is reset (initial power on etc). 
You should see the green/red comms LED's flicker during upload.

Step 9. - Go to Tools->Serial Monitor, note that this will also cause entry into bootloader for about 4-5 seconds before the applicaton runs

Step 10. - See diagnostic printout in the comms window indating pass/fail of testing.

