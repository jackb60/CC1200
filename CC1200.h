#include "Arduino.h"
#include "SPI.h"

#define BURST 0x40
#define READ 0x80

class CC1200 {
    public:
        CC1200(SPIClass* SPI, SPISettings settings);
        void begin();
        byte status();
        void simpleConfig();
        bool ready();
        byte partnum();
        bool testTx();
        void testRx();
        byte avail();
        void read(byte* buf);
        byte reset();
    private:
        SPIClass* _SPI;
        SPISettings _settings;
        void _writeReg(unsigned int reg, byte val);
        byte _readReg(unsigned int reg);
        byte _strobe(byte cmd);
};