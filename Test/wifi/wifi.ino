#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <BlynkSimpleEsp8266.h>
#include "ThingSpeak.h"
#include <String.h>

char token[] = "5_ksH1Dl4UIXwsE8w8iR5eeyX-sHyxJA";
// mqtt server
const char* mqtt_server = "broker.mqtt-dashboard.com";

// Thingspeak channel
unsigned long myChannelNumber = 1282766;
const char * myWriteAPIKey = "RMF501JWUVH75ZYU";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

/*
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe("/home/human/come");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(message); 
}


void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.autoConnect("Trungg");
  Serial.println("Connected...");
  Blynk.config(token);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Thingspeak
  ThingSpeak.begin(espClient);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  Blynk.run();
  Blynk.email("trungdinhviet1999@gmail.com", "Hello");
  Blynk.notify("Gas Warning Condition!!");
  Serial.println("Send");
  
  client.loop();
  // Publish on localhost using MQTT
  
  // DHT sensor code
  client.publish("/dht/temp", "11");
  client.publish("/dht/humi", "78");
  
  // Gas sensor code
  client.publish("/gas", "53");
  
  // Dustbin fill level
  client.publish("/dustbin-fill", "60");

  // Thingspeak

  float temperature = random(100);
  float humidity = random(70);
  float gas = random(70);
  float dustbinFill = random(70); 
  
  //Write data to field
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, gas);
  ThingSpeak.setField(4, dustbinFill);
  
  int returncode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  // Check return Code
  if (returncode == 200) {
    Serial.println("Channel update sucessful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(returncode));
  }
 
  //Serial.println(ThingSpeak.readIntField(myChannelNumber, 3, myReadAPIKey));
  delay(200);
}
