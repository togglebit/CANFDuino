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

# Example Code
All of the examples below are found in the C:\Users\YOUR USERNAME\Documents\Arduino\hardware\CANFDuino\samd\libraries\CANFDuino folder. This can also be accessed in the IDE by going to File->Examples->CANFDuino->.

## CAN/CANFD Packet Monitoring

***CANFduino_CANTerm.ino***


![Image](https://togglebit.net/wp-content/uploads/2021/11/ezgif.com-gif-maker-1.gif)

**CANTerm** is a cheapo 2 port CAN/CANFD packet monitor that can be used in simple serial terminal programs regardless of OS without special PC software. The CANFDuino is used to print packet payloads to the screen using terminal commands in static locations for easy viewing. Supports multiple CAN and CANFD baud rates, ID range filtering and stores the last settings into flash.

CANTerm has been tested with the PuTTY terminal program (2MBaud). PuTTY is probably the most flexible terminal tool for Windows, where the font size, number of lines, size of display can be configured to pack more data on one screen. [https://www.chiark.greenend.org.uk/~sgtatham/putty/](https://www.chiark.greenend.org.uk/~sgtatham/putty/)


Monitor CAN bus traffic in the comfort of a Chrome browser with serial terminal from google's chromelabs (2MBaud). Just click the link below and connect to the COM port.
[https://googlechromelabs.github.io/serial-terminal/](https://googlechromelabs.github.io/serial-terminal/)

Note: The Arduino Serial Monitor is not a terminal program and will not work.

**10 Steps for using CANTerm**

**Step 1.** - Do everything above in getting started section to make sure yoru hardware works. Hook up the CAN/FD ports to the buss(es) you want to monitor. 

**Step 2.** - Open ArduinoIDE, go to File->Examples->CANFDuino_CANTerm.ino

**Step 3.** - Upload it with the IDE.

**Step 4.** - Open up your favorite terminal program (PuTTY, or click the chromelabs serial terminal link above), select the baud rate to 2MBaud, select the correct COM port and click connect. Wait for a few seconds for on-screen instructions to appear and follow them (if no on-screen instructions, check the baud rate and COM port setting and connect again).

**Step 5.** - Select if you want CAN0 or CAN1 or both ports to do packet monitoring.

**Step 6.** - Select the baud rate for CAN packets (or none)

**Step 7.** - Select the baud rate for CANFD packets (or none)

**Step 8.** - Type in the hex ID for the LOWEST ID you want to monitor (or type ALL for all packets to be monitored)

**Step 9.** - Type in the hex ID for the HIGHEST ID you want to monitor. All ID's between these ranges will pass through

**Step 10.** - If you selected 2 ports to monitor....you will be asked to repeat settings for the second port. Watch for traffic to hit the screen.

Note: if you do not want to wait the several seconds between power cycles or new terminal connections use the bootloader bypass jumper detailed in the hookup guide.

## Gateway CAN Messages 

***CANFDuino_GatewayCAN02CAN1.ino***

This is a simple example of receiving a message on CAN0, modifying that message and re-sending it on CAN1. The example is very simple and looks for ID 0x100, modifies byte 0 and re-transmits on CAN1. The example uses a virtual function overload **CallbackRx** to implement the transmission when the message is detected as being received by **RxMsgs** which is simply a polling function that de-queues packets and can fireoff CallbackRx when a specific ID is detected. The user must define what happens in CallbackRx per the example below: 

```C:

bool RxTx::CallbackRx(RX_QUEUE_FRAME *R)
{
  bool retVal = false;
    UINT8 i;
  if (R)
  {
    digitalWrite(3, HIGH);
    
    tx.id = R->rxMsgInfo.id;
    tx.len = R->rxMsgInfo.data_len;
    
    //copy all data bytes except byte 0
    for(i=1;i<tx.len;i++)
    {
      tx.data[i] - R->data[i];
    }
    //lastly modify byte 0, re-transmit on can port 1
    tx.data[0] = 0x80;
    CanPort1.TxMsg(&tx);
  }
  return(retVal);
}
```

Note, as used in CANTerm, if alot of messages are expected, the following macro can be used to increase the reception buffer size **#define MAX_NUM_RXFRAMES  64**

## OBD2 datalogger to SD Card 

***CANFDuino_OBD2Logger.ino***

This is an example sketch that uses and OBD2 library to continously poll the OBD2 port of a vehicle and log the resulting data in CSV format to the SD card of the CANFDuino. The data is also printed to the serial monitor (LogScreen). This particular example polls for RPM, Speed, Throttle Position, Coolant Temp, Engine Load, Mass Airflow Rate, and Intake Air Temp. More data can be obtained by adding an **cOBDParameter** object for the item of interest, OBD2 is well documented a reference of parameters can be found here [https://en.wikipedia.org/wiki/OBD-II_PIDs] (https://en.wikipedia.org/wiki/OBD-II_PIDs). Have a look at the OBD2.h file and the parameters given in the example to get an idea how to properly add a new parameter (you will also need to modify the LogScreen, WriteOBD2header and logOBDData functions). **Note** that if you are using an OBD2 to DB9 cable, you need to watch the pinout of the DB9 as the CANFDuino uses 2,7 and 3 (see hookup guide), many OBD2 to DB9 cables use pins 3,5 and 1. 
```
/***** DEFINITIONS FOR OBD MESSAGES ON CAN PORT 0, see https://en.wikipedia.org/wiki/OBD-II_PIDs to add your own ***************/
//char _name[10], char _units[10], OBD_PID pid,  uint8_t OBD_PID_SIZE size, bool _signed, OBD_MODE_REQ mode, float32 slope, float32 offset, cAcquireCAN *, extended ID;

cOBDParameter OBD_Speed(      "Speed "        , " KPH"    ,  SPEED       , _8BITS,   false,   CURRENT,  1,      0,  &CanPort0, false);
cOBDParameter OBD_EngineSpeed("Engine Speed " , " RPM"    ,  ENGINE_RPM  , _16BITS,  false,   CURRENT,  0.25,   0,  &CanPort0, false);
cOBDParameter OBD_Throttle(   "Throttle "     , " %"      ,  THROTTLE_POS, _8BITS,   false,   CURRENT,  0.3922, 0,  &CanPort0, false);
cOBDParameter OBD_Coolant(    "Coolant "      , " C"      ,  COOLANT_TEMP, _8BITS,   false ,  CURRENT,  1,    -40,  &CanPort0, false);
cOBDParameter OBD_EngineLoad( "Load "         , " %"      ,  ENGINE_LOAD , _8BITS,   false,   CURRENT,  0.3922, 0,  &CanPort0, false);
cOBDParameter OBD_MAF(        "MAF "          , " grams/s",  ENGINE_MAF  , _16BITS,  false,   CURRENT,  0.01,   0,  &CanPort0, false);
cOBDParameter OBD_IAT(        "IAT "          , " C"      ,  ENGINE_IAT  , _8BITS,   false ,  CURRENT,  1,    -40,  &CanPort0, false);
```

## Analog to CAN 

***CANFDuino_Analog2CAN.ino***

This example reads analog inputs A0 to A7 and sends the results over CAN bus in two messages 0x100 and 0x200. The values are sent most signficant byte first, and keep in mind the SAMC21 M0 is a 3.3V part, with a 12bit ADC. This means 0xFFF = 4096 = 3.3V by default (internal reference for ADC can be changed see Arduino library). The example code uses a simple periodic time poll to send them every xmSec and a DEBUG macro to optionaly print the values to screen. The packets could be stuffed a bit tighter but for simplicity we are sending 16bits for 12bit vavlues. Also, a single CANFD message could handle all of the analog inputs in one message. 

## CAN to PWM 

***CANFDuino_CAN2PWM.ino***

This example reads the 8 byte contents of message 0x100 and drives 8 PWM outputs based upon these packet values. There are 16 pins that can bused as PWM output (see PWM output example code), in this exapmle we are using pins 18-26. The default PWM base frequency is about 700Hz. 

## CAN/CANFD Testing 

***CANFDuino_Test500kb.ino, CANFDuino_Test5Mb.ino***

These are the sketches used in the getting started. With CAN0 and CAN1 wired together, they exchange messages to verify functionality and the outcome is printed to the serial monitor. The 500KBaud sketch uses standard CAN, the 5MB sketch uses CANFD.

## IO Testing 

***CANFDuino_DigitalOut.ino, CANFDuino_DigitalPWM.ino, CANFDuino_DigitalInput.ino, CANFDuino_Analog.ino***

These are all very simple test sketches for the basic Arduino analog inputs, digital inputs, and digital output functionality. All of the pins used in the sketches are labeled on the silkscreen.

## Communications Testing 

***CANFDuino_2WireMaster.ino, CANFDuino_2WireSlave.ino, CANFDuino_SerialTest.ino, CANFDuino_SPI.ino***

These are the simple test functions for the serial interfaces that exist in the Arduino platform. Open the sketches to see the pinout or consult the **"variant.cpp"** file in the CANFDuino\samd\variants\ folder to see the mapping to the arduino IO.


