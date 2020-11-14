#include <Arduino.h>
#include <SPIFFS.h>
#include "main.h"

void wavHeader(byte *header, int wavSize);

void listSPIFFS(void);

void SPIFFSInit();

File SPIFFSOpen();