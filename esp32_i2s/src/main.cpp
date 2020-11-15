#include "main.h"
#include "mfs.h"
#include "mi2s.h"
#include "mwifi.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  initSPIFFS();
  initI2S();

  xTaskCreate(read_I2S_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL); // 1024 * 4 idk why?
  delay(500);
  xTaskCreate(connect_wifi, "wifi_Connect", 4096, NULL, 0, NULL);
}

void loop()
{
  // put your main code here, to run repeatedly:
}