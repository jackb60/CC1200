#include "SPI.h"
#include "CC1200.h"

#define MOSI PA7
#define MISO PA6
#define SCLK PA5
#define CS   PA4

SPIClass SPI_3(MOSI, MISO, SCLK, CS);
SPISettings settings(1000000, MSBFIRST, SPI_MODE0);

CC1200 cc(&SPI_3, settings);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    cc.begin();
    Serial.println("Checking part number (should return 0x20)");
    Serial.print("Part number: 0x");
    Serial.println(cc.partnum(), HEX);
}
  
void loop() {
    // put your main code here, to run repeatedly:
  
}
  