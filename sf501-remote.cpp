/*
sf501-remote, a library to control sf501 remotes by flamingo/ smartwares.eu
Arjen Klaverstijn - https://github.com/arjhun - http://www.arjenklaverstijn.com
Project repo: https://github.com/arjhun/arduino-sf501remote
version 0.0.1

This library was inspired by rf-switch: https://github.com/sui77/rc-switch

*/

#include "Arduino.h"
#include "sf501-remote.h"


#if defined(ESP8266) || defined(ESP32)
// interrupt handler and related code must be in RAM on ESP8266,
// according to rf-switch repo issue #46.
#define RECEIVE_ATTR ICACHE_RAM_ATTR
#else
#define RECEIVE_ATTR
#endif

void Sf501Remote::startTransmitter(int pin)
{
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

void Sf501Remote::sendRaw(unsigned long command, int repeat){

  while(repeat){
    _pulseStart();

    for(int i = 31; i >= 0 ; i--){
      if ((command >> i) & 1 ){
        _pulseLong();
        _pulseShort();
      }else{
        _pulseShort();
        _pulseLong();
      }
    }
    //end pulse
    _pulse();

    if(repeat > 1) delayMicroseconds(_pulseLength * (_waitPulses-1));
    repeat--;
  }

}

void Sf501Remote::sendPacket(Sf501Packet packet, int repeat){
  sendCommand(packet.remoteId, packet.channel, packet.state, repeat);
};

void Sf501Remote::sendCommand(int remoteId, int channel, int onOff, int repeat)
{
  long command = 0;
  command += remoteId;
  command <<= 16;
  command |= onOff << 4;
  command |= constrain(channel,1,15);
  sendRaw(command, repeat);
}

void Sf501Remote::switchAll(int remoteId, int numChannels, int onOff, int repeat){

  for(int i = 1; i <= constrain(numChannels,1,15); i++)
  {
    sendCommand(i, onOff, repeat);
  }

}

void Sf501Remote::_pulse()
{
  digitalWrite(_pin, HIGH);
  delayMicroseconds(_pulseLength);
  digitalWrite(_pin, LOW);
}

void Sf501Remote::_pulseStart()
{
  _pulse();
  delayMicroseconds(_pulseLength * _beginPulses);
}

void Sf501Remote::_pulseShort()
{
  _pulse();
  delayMicroseconds(_pulseLength * _shortPulses);
}

void Sf501Remote::_pulseLong()
{
  _pulse();
  delayMicroseconds(_pulseLength * _longPulses);
}

#ifndef NO_RECEIVER

  //Receiver

  void Sf501Remote::startReceiver(int pin){
    _rPin = pin;
    attachInterrupt(digitalPinToInterrupt(pin), _pulseReceived, RISING);
  }

  void Sf501Remote::stopReceiver(){
    detachInterrupt(digitalPinToInterrupt(_rPin));
  };

  volatile long Sf501Remote::_message = 0;
  Sf501Packet Sf501Remote::_packet = {0,0,0};
  volatile boolean Sf501Remote::_messageReady = false;

  boolean Sf501Remote::packetAvailable()
  {
    return _messageReady;
  }

  long Sf501Remote::getRawData(){
    return _message;
  }

  void Sf501Remote::nextPacket()
  {
    _message = 0;
    _messageReady = false;
  }

  Sf501Packet Sf501Remote::getPacket(){
    return _packet;
  }

  boolean RECEIVE_ATTR Sf501Remote::_checkPulse(long pulseLength, long comp){
    return pulseLength < comp + _error && pulseLength > comp - _error;
  }

  void RECEIVE_ATTR Sf501Remote::_pulseReceived() {

    static unsigned long lastChange = 0;
    static unsigned int lastPulseLength = 0;
    static int  pulseCounter = 0;
    static int bitCounter = 0;
    static boolean started = false;
    static boolean receiving = true;
    static unsigned long message = 0;
    int now = micros();


    const unsigned long pulseWidth =  now - lastChange;

    if (  started   &&
          receiving &&
          _checkPulse(pulseWidth, _pulseLength * (_longPulses+1)) ||
          _checkPulse(pulseWidth, _pulseLength * (_shortPulses+1))
        ) {

        if (pulseCounter != 0 && pulseCounter % 2) { //every second incoming pulse is the end of a bit

          if (_checkPulse(lastPulseLength, _pulseLength * (_shortPulses+1))) { // 0
            message <<= 1;
            }
          else if (_checkPulse(lastPulseLength, _pulseLength * (_longPulses+1))) { // 1
            message <<= 1;
            message |= 1;
          }

          if(bitCounter == _numBits-1){
            bitCounter = 0;
            started = false;
            receiving = false;
            _packet.remoteId = message >> 16;
            _packet.state = message >> 4 & 1;
            _packet.channel = message & 15;
            _message = message;
            _messageReady = true;
            receiving = false;
          }else{
            bitCounter++;
          }
        }
        pulseCounter++;
    }else{
      if(_checkPulse(pulseWidth, _pulseLength * (_beginPulses+1))){
        pulseCounter = 0;
        started = true;
        message = 0;
      }else if(pulseWidth > _pulseLength * (_maxPulses+1)){
        started = false;
        receiving = true;
      }
    }

    lastChange = now;
    lastPulseLength = pulseWidth;

  }// pulseReceived isr

#endif
