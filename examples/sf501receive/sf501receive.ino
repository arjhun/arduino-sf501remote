/*

sf501-remote Arduino Library

Sf501 receiver example

By:  Arjen Klaverstijn
git: https://github.com/arjhun/arduino-sf501remote


*/

#include <sf501-remote.h>

Sf501Remote remote;

void setup() {
    Serial.begin(9600);
    //choose a pin with interrupt capabilities
    remote.startReceiver(2);
}

void loop() {

  //Check if data received
  if (remote.packetAvailable()) {

    Sf501Packet result = remote.getPacket();

    Serial.println("remote ID: " + String(result.remoteId));
    Serial.println("switch state: " + String(result.state ? "on": "off"));
    Serial.println("channel: " + String(result.channel));
    Serial.print("raw(dec): ");
    Serial.println(remote.getRawData(), DEC);
    Serial.print("raw(bin): ");
    Serial.println(remote.getRawData(), BIN);
    Serial.println("");

    //Ready with data, next!
    remote.nextPacket();
  }
}
