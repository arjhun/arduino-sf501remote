#include <sf501-remote.h>

Sf501Remote remote;
const int remoteId = 25134;

void setup() {

  remote.startTransmitter(11);
}

void loop() {

  int button = 1; // up to 15 buttons (4 bits)
  boolean onOff = true; // true or false

  remote.sendCommand(remoteId, button , onOff);

  //wait 2 seconds
  delay(2 * 1000);

  button = 2;
  int repeat = 6;
  remote.sendCommand(remoteId, button , onOff, repeat);

  //wait 2 seconds
  delay(2 * 1000);

  // Tou can also create a packet and send it!
  Sf501Packet packet;
  packet.remoteId = remoteId;
  packet.state = onOff;
  packet.channel = 3;

  remote.sendPacket(packet);

  //wait 2 seconds
  delay(2 * 1000);

  //You can also send a raw command, usefull when implementing a learning mechanism
  long command = 556793876; // remote id 25134, channel 4, button ON
  remote.sendRaw(command);

  //wait 2 seconds
  delay(2 * 1000);

  //turn everything off
  int numberOfChannels = 4;
  onOff = false;

  remote.switchAll(remoteId, numberOfChannels , onOff);

  //wait 6 seconds
  delay(6 * 1000);

}
