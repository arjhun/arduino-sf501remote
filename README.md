# arduino-sf501remote library by Arjen Klaverstijn
An Arduino library to control SF-501 RF switches by Flamingo (Smartwares.eu) . Theoratically you can control 65,536 remotes with each 15 sockets.

To start using the library include it and create a new Sf501Remote object

```c++
#include <sf501-remote.h>
Sf501Remote remote;
```

### How to start sending packages

#### 1. In your setup function add the following
  ```c++
  remote.startTransmitter(pin);
  ```
  - **pin:** The pin number that is connected to the transmit/ data pin of a 433.9 mhz transmitter. I just use an old rf remote.
  
#### 2. Use one of the following methods in your loop
      
```c++
void Sf501Remote::sendCommand(int remoteId, int channel, int onOff, int repeat = 4);
```
- **remoteId:** a 16 bit unsigned integer 0 - 65535 that will be used as the remote id.
- **channel:** an 4 bit value (1 - 15) discribing the button number (the remotes I have, have only 4 buttons).  
- **onOff:** 0 is off button 1 is on button.  
- **Repeat:** *(Optional)* the number of packages to send each transmit the remotes send them 8 times. *(default: 4)*  

```c++
void Sf501Remote::sendPacket(Sf501Packet packet, int repeat = 4);
```
- **packet:** A structure like the following  

```c++
struct Sf501Packet {
	int remoteId;
	int channel;
	boolean state;
}
```
	
- **Repeat:** *(Optional)* the number of packages to send each transmit the vendor's remotes send them 8 times. *(default: 4)*  

```c++
void Sf501Remote::sendRaw(long command, int repeat = 4);
```  
- **command:** a 32 bit unsigned long containing all the raw data for a packet, use the receiver method *getRawData()* to figure out the value if you want to copy the functionality of an existing remote.  
- **Repeat:** *(Optional)* the number of packages to send each transmit the vendor's remotes send them 8 times. *(default: 4)* 

```c++
void Sf501Remote::switchAll(int remoteId, int startChannel, int endChannel, int onOff, int repeat = 4);
```
This is a helper function to switch multiple sockets/ buttons at once.  

- **remoteId:** a 16 bit unsigned integer 0 - 65535 that will be used as the remote id.
- **channel:** an 4 bit value (1 - 15) discribing the button number (the remotes I have, have only 4 buttons).  
- **onOff:** 0 is off button 1 is on button.  
- **Repeat:** *(Optional)* the number of packages to send each transmit the vendor's remotes send them 8 times. *(default: 4)* 
	
### How to start receiving packages

**1. In your setup function add the following**
  ```c++
  remote.startReceiver(pin);
  ```
  - **pin**: a pin number with interrupt capabilities

**2. In your loop add the following**
```c++
  if (remote.packetAvailable()) {
    
    //Ready with data, next!
    remote.nextPacket();
  }
```
**3. Within this if-statement use the folowing methods to process received packages**

```c++
Sf501Packet Sf501Remote::getPacket();
```
returns the Sf501Packet struct (see above)

```c++
unsigned long Sf501Remote::getRawData();
```
returns the received package as a 32 bit unsigned long

### Protocol explained

A packet consists of 66 pulses in the following pattern:
Preamble (1 pulse) - Data (64 pulses) - End (1 pulse)

Data section format:
32 bits LSB data section
	
32 - 18: 16 bit remote id, 65535 different remotes on this system  
9 - 17: 8 bits are empty on all remotes, no clue where this is for
5 - 8: 4 bits a decimal value of 1 is switch on a 0 is switch off  
1 - 4: maximum of 15 buttons/channels per remote

Pulses are around 200µS.

|¯¯¯|_______________________________________
preamble = 1 pulse high 14 pulses low

|¯¯¯|______|¯¯¯|_____________________
data 0 = 1 pulse high & 2 pulses low, 1 pulses high & 7 pulses low

|¯¯¯|_____________________|¯¯¯|______
data 1 = 1 pulse high & 7 pulses low, 1 pulse high & 2 pulses low

|¯¯¯|
end = 1 pulse high
