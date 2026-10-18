# Arduino sf-501 library by Arjen Klaverstijn
An Arduino library to control SF-501 RF switches by Flamingo (Smartwares.eu) . Theoratically you can control 65,536 remotes with each 15 sockets/ button pairs (on/ off).

>NOTE: THIS IS NOT A LIBRARY FOR THE SF-501SHC!!! The SHC version is a smart controller with app, that commuicates with a cloud api and is able to control the standard 'dumb' sf-501 rf sockets.  

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

### Protocol Explained

A packet contains **66 HIGH pulses**: one preamble pulse, two pulses for each of the 32 data bits, and one end pulse.

Data is transmitted **most-significant bit first (MSB first)**, from bit 31 down to bit 0.

#### Command Format

```text
command = (remoteId << 16) | (state << 4) | channel
```

| Bits (zero-based) | Description                          |
| ----------------- | ------------------------------------ |
| 31–16             | 16-bit remote ID: 0–65535            |
| 15–8              | Unused byte, set to zero             |
| 7–4               | State field: `1` for ON, `0` for OFF |
| 3–0               | Channel/button ID: 1–15              |

For example, remote ID `25134`, channel `1`, produces `0x622E0011` for ON and `0x622E0001` for OFF.

#### Pulse Timing

Using a **200 µs base duration**, the pulse sequences are:

| Element      | HIGH/LOW sequence                    | Signed durations (µs) |
| ------------ | ------------------------------------ | --------------------- |
| Preamble     | HIGH × 1, LOW × 14                   | `200, 2800`           |
| Data bit `0` | HIGH × 1, LOW × 2, HIGH × 1, LOW × 7 | `200, 400, 200, 1400` |
| Data bit `1` | HIGH × 1, LOW × 7, HIGH × 1, LOW × 2 | `200, 1400, 200, 400` |
| End          | HIGH × 1, then return LOW            | `200`                 |

One frame contains **131 duration entries**: two for the preamble, 128 for the data, and one for the end pulse.

Between repeated frames, hold the output LOW for **10800 µs** (54 base units). The default transmission count is **4 frames**, configurable through the `repeat` argument.

#### graph

|¯¯¯|_______________________________________
preamble = 1 pulse high 14 pulses low

|¯¯¯|______|¯¯¯|_____________________
data 0 = 1 pulse high & 2 pulses low, 1 pulses high & 7 pulses low

|¯¯¯|_____________________|¯¯¯|______
data 1 = 1 pulse high & 7 pulses low, 1 pulse high & 2 pulses low

|¯¯¯|
end = 1 pulse high
