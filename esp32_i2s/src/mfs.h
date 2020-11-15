#include <Arduino.h>
#include <SPIFFS.h>
#include "main.h"

void wavHeader(byte *header, int wavSize);

void listFiles(void);

void initSPIFFS();

File getFile();