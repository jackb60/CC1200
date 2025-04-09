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
    delay(1000);
    Serial.begin(115200);
    cc.begin();
}
  
void loop() {
    // put your main code here, to run repeatedly:
    Serial.print("Status: ");
    Serial.println(cc.status());
    delay(1000);
}
