#include "SPI.h"
#include "CC1200.h"

#define MOSI PA7
#define MISO PA6
#define SCLK PA5
#define CS   PA4

SPIClass SPI_3(MOSI, MISO, SCLK, CS);
SPISettings settings(1000000, MSBFIRST, SPI_MODE0);

CC1200 cc(&SPI_3, settings);

byte recBuf[3];

void setup() {
    // put your setup code here, to run once:
    delay(1000);
    Serial.begin(115200);
    cc.begin();
    Serial.println("Checking part number (should return 0x20)");
    Serial.print("Part number: 0x");
    Serial.println(cc.partnum(), HEX);
    Serial.println("Rx Test:");
    cc.testRx();
}
  
void loop() {
    // put your main code here, to run repeatedly:
    if(cc.avail()) {
        Serial.println("PACKET AVAILABLE");
        cc.read(recBuf);
        for(byte i = 0; i < 3; i++) {
            Serial.print("BYTE ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(recBuf[i], HEX);
        }
        
    }
}
  