#include <Arduino.h>
#include <SPIFFS.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Credentials.h"

#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE (16000)
#define I2S_SAMPLE_BITS (16)
#define I2S_READ_LEN (16 * 1024)
#define RECORD_TIME (10) //Seconds
#define I2S_CHANNEL_NUM (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)
//I2S built-in ADC unit
#define I2S_ADC_UNIT              ADC_UNIT_1
//I2S built-in ADC channel
#define I2S_ADC_CHANNEL           ADC1_CHANNEL_0

// bool isWIFIConnected;

// unsigned long now;
// unsigned long then;
// uint8_t interval = 2;

File file;
const char filename[] = "/record.wav";
const int headerSize = 44;

void wavHeader(byte *header, int wavSize)
{
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;
  header[21] = 0x00;
  header[22] = 0x01;
  header[23] = 0x00;
  header[24] = 0x80;
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;
  header[33] = 0x00;
  header[34] = 0x10;
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);
}

// void listFiles(void)
// {
//   Serial.println(F("\r\nListing SPIFFS files:"));
//   static const char line[] PROGMEM = "=================================================";

//   Serial.println(FPSTR(line));
//   Serial.println(F("  File name                              Size"));
//   Serial.println(FPSTR(line));

//   fs::File root = SPIFFS.open("/");
//   if (!root)
//   {
//     Serial.println(F("Failed to open directory"));
//     return;
//   }
//   if (!root.isDirectory())
//   {
//     Serial.println(F("Not a directory"));
//     return;
//   }

//   fs::File file = root.openNextFile();
//   while (file)
//   {

//     if (file.isDirectory())
//     {
//       Serial.print("DIR : ");
//       String fileName = file.name();
//       Serial.print(fileName);
//     }
//     else
//     {
//       String fileName = file.name();
//       Serial.print("  " + fileName);
//       // File path can be 31 characters maximum in SPIFFS
//       int spaces = 33 - fileName.length(); // Tabulate nicely
//       if (spaces < 1)
//         spaces = 1;
//       while (spaces--)
//         Serial.print(" ");
//       String fileSize = (String)file.size();
//       spaces = 10 - fileSize.length(); // Tabulate nicely
//       if (spaces < 1)
//         spaces = 1;
//       while (spaces--)
//         Serial.print(" ");
//       Serial.println(fileSize + " bytes");
//     }

//     file = root.openNextFile();
//   }

//   Serial.println(FPSTR(line));
//   Serial.println();
//   delay(1000);
// }

void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      yield();
  }

  Serial.println("Formating SPIFFS...");
  SPIFFS.format();
  // SPIFFS.remove(filename);
  // file = SPIFFS.open(filename, FILE_WRITE);
  // if (!file)
  // {
  //   Serial.println("File is not available!");
  // }

  // byte header[headerSize];
  // wavHeader(header, FLASH_RECORD_SIZE);

  // file.write(header, headerSize);
  // listFiles();
}

void createFile()
{
  SPIFFS.remove(filename);
  file = SPIFFS.open(filename, FILE_WRITE);
  if (!file)
  {
    Serial.println("File not created!");
  }

  byte header[headerSize];
  wavHeader(header, FLASH_RECORD_SIZE);

  file.write(header, headerSize);
  // listFiles();
}

void openFile()
{
  file = SPIFFS.open(filename, FILE_READ);
}

void connect_wifi()
{
  // isWIFIConnected = false;
  char *ssid = SSID;
  char *password = PWD;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    // vTaskDelay(500);
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wifi");
  // isWIFIConnected = true;

  // vTaskDelete(NULL);
  // while (true)
  // {
  //     vTaskDelay(1000);
  // }
}

void uploadFile()
{
  if (WiFi.isConnected())
  {
    // listFiles();
    openFile(); // this opnes file to global defined variable: File file
    if (!file)
    {
      ets_printf("File is not available");
      return;
    }

    Serial.println("Uploading file to server");

    HTTPClient client;
    // client.setConnectTimeout(2000);
    client.begin("http://192.168.0.113:8008/upload");

    client.addHeader("Content-Type", "audio/wav");
    client.sendRequest("POST", &file, file.size());
    // client.setTimeout(2000);
    // int httpResponseCode = client.sendRequest("POST", &file, file.size());
    // Serial.print("httpResponseCode : ");
    // Serial.println(httpResponseCode);
    file.close();

    // if (httpResponseCode == 200)
    // {
    //   Serial.println("File uploaded");
    // }
    // else
    // {
    //   Serial.println("Error in uploading file");
    // }
    client.end();
  }
  else
  {
    Serial.println("Wifi not connected");
  }
}

/**
 * @brief Scale data to 8bit for data from ADC.
 *        Data from ADC are 12bit width by default.
 *        DAC can only output 8 bit data.
 *        Scale each 12bit ADC data to 8bit DAC data.
 */
void i2sScaleDataADC(uint8_t *d_buff, uint8_t *s_buff, uint32_t len)
{
  uint32_t j = 0;
  uint32_t dac_value = 0;
  for (int i = 0; i < len; i += 2)
  {
    dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
    d_buff[j++] = 0;
    d_buff[j++] = dac_value * 256 / 2048;
  }
}

void read_I2S_data()
{

  // while (1)
  // {

  int i2s_read_len = I2S_READ_LEN;
  int flash_wr_size = 0;
  size_t bytes_read;

  char *i2s_read_buff = (char *)calloc(i2s_read_len, sizeof(char));
  uint8_t *flash_write_buff = (uint8_t *)calloc(i2s_read_len, sizeof(char));

  i2s_read(I2S_PORT, (void *)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
  i2s_read(I2S_PORT, (void *)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

  Serial.println(" *** Recording Start *** ");
  // File file = createFile();
  createFile();
  // i2s_adc_enable(I2S_PORT);
  while (flash_wr_size < FLASH_RECORD_SIZE)
  {
    //read data from I2S bus, in this case, from ADC.
    // ets_printf("t=%ld\n", millis());
    i2s_read(I2S_PORT, (void *)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY); //0.5 sec
    // ets_printf("t1=%ld\n", millis());

    //example_disp_buf((uint8_t*) i2s_read_buff, 64);
    //save original data from I2S(ADC) into flash.
    i2sScaleDataADC(flash_write_buff, (uint8_t *)i2s_read_buff, i2s_read_len);
    file.write((const byte *)flash_write_buff, i2s_read_len); //0.1 sec
    // ets_printf("t2=%ld\n", millis());
    flash_wr_size += i2s_read_len;
    ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
    ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
  }
  file.close();
// i2s_adc_disable(I2S_PORT);
  free(i2s_read_buff);
  i2s_read_buff = NULL;
  free(flash_write_buff);
  flash_write_buff = NULL;

  // listFiles();

  // if(isWIFIConnected){
  //   uploadFile();
  // }
  uploadFile();
  // }

  // vTaskDelete(NULL); //deleting task itself
}

void initI2S()
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = I2S_SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = 0,
      .dma_buf_count = 64,
      .dma_buf_len = 1024,
      .use_apll = 1};

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  // i2s_set_adc_mode(I2S_ADC_UNIT, I2S_ADC_CHANNEL);

  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  initSPIFFS();
  initI2S();
  connect_wifi();

  // xTaskCreate(read_I2S_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL); // 1024 * 4 idk why?
  delay(500);
  // xTaskCreate(connect_wifi, "wifi_Connect", 4096, NULL, 0, NULL);
}

void loop()
{
  // now = millis();
  // if (now - then > interval){
  //   then = now;
  // }
  read_I2S_data();
  // put your main code here, to run repeatedly:
}