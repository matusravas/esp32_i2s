#include "mwifi.h"

bool isWIFIConnected;

extern File file;

void connect_wifi(void *pvParameters)
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
    if (isWIFIConnected)
    {

        file = getFile();
        if (!file)
        {
            ets_printf("File: %s is not available", file);
            return;
        }

        Serial.println("Uploading file to server");

        HTTPClient client;
        client.begin("http://192.168.0.113:8008/upload");
        client.addHeader("Content-Type", "audio/wav");
        int httpResponseCode = client.sendRequest("POST", &file, file.size());
        Serial.print("httpResponseCode : ");
        Serial.println(httpResponseCode);

        if (httpResponseCode == 200)
        {
            String response = client.getString();
            ets_printf("Response %s", response);
        }
        else
        {
            Serial.println("Error in uploading file");
        }
        file.close();
        client.end();
    }
    else
    {
        Serial.println("Wifi not connected");
    }
}