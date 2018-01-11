/*
sf501-remote, a library to control sf501 remotes by flamingo/ smartwares.eu
Arjen Klaverstijn - https://github.com/arjhun - http://www.arjenklaverstijn.com
Project repo: https://github.com/arjhun/arduino-sf501remote
version 0.0.1

This library was inspired by rf-switch: https://github.com/sui77/rc-switch

*/

#ifndef SF501REMOTE_H
#define SF501REMOTE_H

    struct Sf501Packet {
      int remoteId;
      int channel;
      boolean state;
    };

    class Sf501Remote
    {

    public:
      /*
      - packet consists of
        Preamble (1 pulse) - Data (64 pulses) - End (1 pulse)

      - Data format:
        32 bits data section

        remoteId : a 16 bit value enabling 65535 different remotes on this system
        uknown data: 8 bits are empty on all remotes, no clue where this is for...
        button: 4 bits a decimal value of 1 is switch on a 0 is switch off
        channel: 4bits maximum of 15 buttons/channels per remote, higher values will be discarded by this library

      Pulses are around 200µS.

      |¯¯¯|_______________________________________
      preamble = 1 pulse high 14 pulses low

      |¯¯¯|______|¯¯¯|_____________________
      data 0 = 1 pulse high & 2 pulses low, 1 pulses high & 7 pulses low

      |¯¯¯|_____________________|¯¯¯|______
      data 1 = 1 pulse high & 7 pulses low, 1 pulse high & 2 pulses low

      |¯¯¯|
      end = 1 pulse high

      */

      void startTransmitter(int pin);
      void sendCommand(int remoteId, int channel, int onOff, int repeat = 4);
      void sendPacket(Sf501Packet packet, int repeat = 4);
      void sendRaw(unsigned long command, int repeat = 4);
      void switchAll(int remoteId, int numChannels, int onOff, int repeat = 4);

      #ifndef NO_RECEIVER

        void startReceiver(int pin);
        void stopReceiver();
        boolean packetAvailable();
        Sf501Packet getPacket();
        static unsigned long getRawData();
        void nextPacket();

      #endif

    private:

      //globals vars
      const static int _pulseLength = 190;
      const static long _beginPulses = 14, _shortPulses = 2, _longPulses = 7, _waitPulses = 55, _maxPulses = 400;
      const static int _min_packets = 3;
      const static int _error = 100;
      const static int _numBits = 32;

      //transmitter vars
      void _pulse(), _pulseStart(), _pulseLong(), _pulseShort();
      int _pin;

      #ifndef NO_RECEIVER
        //receiver vars
        int _rPin;
        volatile static unsigned long _message;
        static Sf501Packet _packet;
        volatile static boolean _messageReady;
        static boolean _checkPulse(long pulseLength, long comp);
        static void _pulseReceived();
      #endif
    };

#endif
