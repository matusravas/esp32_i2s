#include "mwifi.h"

bool isWIFIConnected;

extern File file;

void wifiConnect(void *pvParameters)
{
    isWIFIConnected = false;
    char *ssid = "cerova_449b";
    char *password = "adka2002";

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500);
        Serial.print(".");
    }
    isWIFIConnected = true;
    while (true)
    {
        vTaskDelay(1000);
    }
}

void uploadFile()
{
    file = SPIFFSOpen();
    if (!file)
    {
        Serial.println("FILE IS NOT AVAILABLE!");
        return;
    }

    Serial.println("===> Upload FILE to Node.js Server");

    HTTPClient client;
    client.begin("http://192.168.1.124:8888/uploadAudio");
    client.addHeader("Content-Type", "audio/wav");
    int httpResponseCode = client.sendRequest("POST", &file, file.size());
    Serial.print("httpResponseCode : ");
    Serial.println(httpResponseCode);

    if (httpResponseCode == 200)
    {
        String response = client.getString();
        Serial.println("==================== Transcription ====================");
        Serial.println(response);
        Serial.println("====================      End      ====================");
    }
    else
    {
        Serial.println("Error");
    }
    file.close();
    client.end();
}