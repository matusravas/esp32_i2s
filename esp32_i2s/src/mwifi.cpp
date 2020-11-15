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
    ets_printf("Connected to Wifi: %s", &ssid);
    isWIFIConnected = true;
    while (true)
    {
        vTaskDelay(1000);
    }
}

void uploadFile()
{
    if (!isWIFIConnected) // Todo remove !
    {

        file = getFile();
        if (!file)
        {
            Serial.println("FILE IS NOT AVAILABLE!");
            return;
        }

        Serial.println("Uploading file to server");

        HTTPClient client;
        client.begin("http://192.168.1.124:5000/upload");
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
    else{
        Serial.println("Wifi not connected");
    }
}