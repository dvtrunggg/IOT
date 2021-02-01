#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "HCMUS-I92"; 
const char* password = "phonghoc@i92"; 

const char* host = "maker.ifttt.com";
const uint16_t port = 80;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup()
{
  Serial.begin(115200);
  timeClient.begin();
}
void loop()
{
  /*
    timeClient.update();
    Serial.println(timeClient.getFormattedTime() + 7);
    delay(1000);
    */

    
    Serial.print("Connecting to...");
    Serial.print(host);
    Serial.print(":");
    Serial.println("port");

    //use wifi
    WiFiClient client;
    if (!client.connect(host, port))
    {
        Serial.println("connection failed");
        delay(5000);
        return;
    }

    client.print(String("GET /trigger/trung/with/key/k73bhP4Z3gzHBc31z7d7MKcDs8RYje1QVL7_AyZYD6G") + " HTTP/1.1\r\n" +
                 "Host" + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(500);

    while (client.available())
    {
        String line = client.readStringUntil('\R');
        Serial.println(line);
    }
    Serial.println();
}
