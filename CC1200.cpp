#include "Arduino.h"
#include "SPI.h"
#include "CC1200.h"

CC1200::CC1200(SPIClass* SPI, SPISettings settings, int cs) {
    _SPI = SPI;
    _settings = settings;
    _cs = cs;
}

void CC1200::begin() {
    digitalWrite(_cs, 1);
    _SPI->begin();
    _SPI->beginTransaction(_settings);
    delay(1);
    reset();
    delay(1);
}

byte CC1200::reset() {
    return _strobe(0x30);
}

void CC1200::_writeReg(unsigned int reg, byte val) {
    digitalWrite(_cs, 0);
    if(reg < 0x002F) {
        _SPI->transfer((byte) reg);
        _SPI->transfer(val);
    } else {
        _SPI->transfer(0x2F);
        _SPI->transfer((byte) reg);
        _SPI->transfer(val);
    }
    digitalWrite(_cs, 1);
}

byte CC1200::_readReg(unsigned int reg) {
    byte ret;
    digitalWrite(_cs, 0);
    if(reg < 0x002F) {
        _SPI->transfer(((byte) reg) | READ);
        ret = _SPI->transfer(0x00);
    } else {
        _SPI->transfer(0x2F | READ);
        _SPI->transfer(((byte) reg));
        ret = _SPI->transfer(0x00);
    }
    digitalWrite(_cs, 1);
    return ret;
}

byte CC1200::_strobe(byte cmd) {
    digitalWrite(_cs, 0);
    byte ret = _SPI->transfer(cmd);
    digitalWrite(_cs, 1);
}

byte CC1200::status() {
    //0 : IDLE
    //1 : RX
    //2 : TX
    //3 : FSTXON
    //4 : CALIBRATE
    //5 : SETTLING
    //6 : RX FIFO ERROR
    //7 : TX FIFO ERROR
    digitalWrite(_cs, 0);
    byte ret = (_SPI->transfer(0x3D) >> 4) & 0x07;
    digitalWrite(_cs, 1);
    return ret;
}


void CC1200::simpleConfig() {

}

void CC1200::narrowConfig() {
    _writeReg(0x000A, 0xD1); //Frequency Deviation Configuration
    _writeReg(0x000B, 0x00); //Modulation Format and Frequency Deviation Configuration
    _writeReg(0x000C, 0x5D); //Digital DC Removal Configuration
    _writeReg(0x000E, 0x8A); //Preamble Detection Configuration Reg. 0
    _writeReg(0x000F, 0xCB); //Digital Image Channel Compensation Configuration
    _writeReg(0x0010, 0xA6); //Channel Filter Configuration
    _writeReg(0x0011, 0x40); //General Modem Parameter Configuration Reg. 1
    _writeReg(0x0012, 0x05); //General Modem Parameter Configuration Reg. 0
    _writeReg(0x0013, 0x3F); //Symbol Rate Configuration Exponent and Mantissa [19:16]
    _writeReg(0x0014, 0x75); //Symbol Rate Configuration Mantissa [15:8]
    _writeReg(0x0015, 0x10); //Symbol Rate Configuration Mantissa [7:0]
    _writeReg(0x0016, 0x20); //AGC Reference Level Configuration
    _writeReg(0x0017, 0xEC); //Carrier Sense Threshold Configuration
    _writeReg(0x001B, 0x51); //Automatic Gain Control Configuration Reg. 1
    _writeReg(0x001C, 0x87); //Automatic Gain Control Configuration Reg. 0
    _writeReg(0x001D, 0x00); //FIFO Configuration
    _writeReg(0x0020, 0x12); //Frequency Synthesizer Configuration
    _writeReg(0x0026, 0x00); //Packet Configuration Reg. 2
    _writeReg(0x2F00, 0x1C); //IF Mix Configuration
    _writeReg(0x2F01, 0x22); //Frequency Offset Correction Configuration
    _writeReg(0x2F05, 0x0C); //General Modem Parameter Configuration Reg. 2
    _writeReg(0x2F0C, 0x5B); //Frequency Configuration [23:16]
    _writeReg(0x2F0D, 0x80); //Frequency Configuration [15:8]
    _writeReg(0x2F10, 0xEE); //Analog to Digital Converter Configuration Reg. 1
    _writeReg(0x2F11, 0x10); //Analog to Digital Converter Configuration Reg. 0
    _writeReg(0x2F12, 0x07); //Frequency Synthesizer Digital Reg. 1
    _writeReg(0x2F13, 0xAF); //Frequency Synthesizer Digital Reg. 0
    _writeReg(0x2F16, 0x40); //Frequency Synthesizer Calibration Reg. 1
    _writeReg(0x2F17, 0x0E); //Frequency Synthesizer Calibration Reg. 0
    _writeReg(0x2F19, 0x03); //Frequency Synthesizer Divide by 2
    _writeReg(0x2F1B, 0x33); //FS Digital Synthesizer Module Configuration Reg. 0
    _writeReg(0x2F1D, 0x17); //Frequency Synthesizer Divider Chain Configuration ..
    _writeReg(0x2F1F, 0x00); //Frequency Synthesizer Phase Frequency Detector Con..
    _writeReg(0x2F20, 0x6E); //Frequency Synthesizer Prescaler Configuration
    _writeReg(0x2F21, 0x1C); //Frequency Synthesizer Divider Regulator Configurat..
    _writeReg(0x2F22, 0xAC); //Frequency Synthesizer Spare
    _writeReg(0x2F27, 0xB5); //FS Voltage Controlled Oscillator Configuration Reg..
    _writeReg(0x2F32, 0x0E); //Crystal Oscillator Configuration Reg. 5
    _writeReg(0x2F36, 0x03); //Crystal Oscillator Configuration Reg. 1
}

bool CC1200::ready() {
    digitalWrite(_cs, 0);
    bool rdy = !(_SPI->transfer(0x3D) & 0x80);
    digitalWrite(_cs, 1);
    return rdy;
}

byte CC1200::partnum() {
    return _readReg(0x2F8F);
}

bool CC1200::testTx() {
    byte data[3] = {0xAA, 0xBB, 0xCC};
    digitalWrite(_cs, 0);
    _SPI->transfer(0x3F | BURST); //FIFO ADR
    _SPI->transfer(0xAABBCC, 3); //TEST DATA
    digitalWrite(_cs, 1);

    _strobe(0x35); //STX

    unsigned long stime = millis();
    while (status() != 0x00) {
        if(millis() - stime > 1000) {
            return false;
        }
    }
    return true;
}

void CC1200::testRx() {
    _strobe(0x34); //SRX
}

byte CC1200::avail() {
    return _readReg(0x2FD7); //NUM_RXBYTES
}

void CC1200::read(byte* buf) {
    digitalWrite(_cs, 0);
    _SPI->transfer(0x3F | READ | BURST); //FIFO ADR
    _SPI->transfer(buf, 3); //TEST DATA
    digitalWrite(_cs, 1);
}