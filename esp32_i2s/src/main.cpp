#include "main.h"
#include "mfs.h"
#include "mi2s.h"
#include "mwifi.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFSInit();
  i2sInit();
  xTaskCreate(i2s_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL);
  delay(500);
  xTaskCreate(wifiConnect, "wifi_Connect", 4096, NULL, 0, NULL);
}

void loop()
{
  // put your main code here, to run repeatedly:
}